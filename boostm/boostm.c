#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include "platform.h"
#include "boostm.h"
#include "util.h"


__INLINE__ long ReadSetCoherent (Thread*);


enum boostm_config {
    NOREC_INIT_WRSET_NUM_ENTRY = 1024,
    NOREC_INIT_RDSET_NUM_ENTRY = 8192,
    NOREC_INIT_LOCAL_NUM_ENTRY = 1024,
};


typedef int            BitMap;

/* Read set and write-set log entry */
typedef struct _AVPair {
    struct _AVPair* Next;
    struct _AVPair* Prev;
    volatile intptr_t* Addr;
    intptr_t Valu;
    long Ordinal;
} AVPair;

typedef struct _Log {
    AVPair* List;
    AVPair* put;        /* Insert position - cursor */
    AVPair* tail;       /* CCM: Pointer to last valid entry */
    AVPair* end;        /* CCM: Pointer to last entry */
    long ovf;           /* Overflow - request to grow */
    BitMap BloomFilter; /* Address exclusion fast-path test */
} Log;

struct _Thread {
    long UniqID;
    volatile long Retries;
    long Starts;
    long Aborts; /* Tally of # of aborts */
    long snapshot;
    unsigned long long rng;
    unsigned long long xorrng [1];
    Log rdSet;
    Log wrSet;
    sigjmp_buf* envPtr;
};

typedef struct
{
    long value;
    long counter;
    long padding2;
    long padding3;
    long padding4;
    long padding5;
    long padding6;
    long padding7;
} aligned_type_t ;

__attribute__((aligned(64))) volatile aligned_type_t* LOCK;

static pthread_key_t    global_key_self;

#ifndef NOREC_CACHE_LINE_SIZE
#  define NOREC_CACHE_LINE_SIZE           (64)
#endif

__INLINE__ unsigned long long
MarsagliaXORV (unsigned long long x)
{
    if (x == 0) {
        x = 1;
    }
    x ^= x << 6;
    x ^= x >> 21;
    x ^= x << 7;
    return x;
}

__INLINE__ unsigned long long
MarsagliaXOR (unsigned long long* seed)
{
    unsigned long long x = MarsagliaXORV(*seed);
    *seed = x;
    return x;
}

__INLINE__ unsigned long long
TSRandom (Thread* Self)
{
    return MarsagliaXOR(&Self->rng);
}

__INLINE__ intptr_t
AtomicAdd (volatile intptr_t* addr, intptr_t dx)
{
    intptr_t v;
    for (v = *addr; CAS(addr, v, v+dx) != v; v = *addr) {}
    return (v+dx);
}

volatile long StartTally         = 0;
volatile long AbortTally         = 0;
volatile long ReadOverflowTally  = 0;
volatile long WriteOverflowTally = 0;
volatile long LocalOverflowTally = 0;
#define NOREC_TALLY_MAX          (((unsigned long)(-1)) >> 1)

#define FILTERHASH(a)                   ((UNS(a) >> 2) ^ (UNS(a) >> 5))
#define FILTERBITS(a)                   (1 << (FILTERHASH(a) & 0x1F))

#  define TABMSK                        (_TABSZ-1)
#define COLOR                           (128)

#define PSSHIFT                         ((sizeof(void*) == 4) ? 2 : 3)
#  define _TABSZ  (1<< 20)
static volatile version_t** LockTab;
#  define PSLOCK(a) (LockTab + (((UNS(a)+COLOR) >> PSSHIFT) & TABMSK)) /* PS1M */
#  define OFFSET(a) (((UNS(a)+COLOR) >> PSSHIFT) & TABMSK)

__INLINE__ AVPair*
MakeList (long sz, Thread* Self)
{
    AVPair* ap = (AVPair*) malloc((sizeof(*ap) * sz) + NOREC_CACHE_LINE_SIZE);
    assert(ap);
    memset(ap, 0, sizeof(*ap) * sz);
    AVPair* List = ap;
    AVPair* Tail = NULL;
    long i;
    for (i = 0; i < sz; i++) {
        AVPair* e = ap++;
        e->Next    = ap;
        e->Prev    = Tail;
        e->Ordinal = i;
        Tail = e;
    }
    Tail->Next = NULL;

    return List;
}

 void FreeList (Log*, long) __attribute__ ((noinline));
/*__INLINE__*/ void
FreeList (Log* k, long sz)
{
    /* Free appended overflow entries first */
    AVPair* e = k->end;
    if (e != NULL) {
        while (e->Ordinal >= sz) {
            AVPair* tmp = e;
            e = e->Prev;
            free(tmp);
        }
    }

    /* Free continguous beginning */
    free(k->List);
}

__INLINE__ AVPair*
ExtendList (AVPair* tail)
{
    AVPair* e = (AVPair*)malloc(sizeof(*e));
    assert(e);
    memset(e, 0, sizeof(*e));
    tail->Next = e;
    e->Prev    = tail;
    e->Next    = NULL;
    e->Ordinal = tail->Ordinal + 1;
    return e;
}

__INLINE__ void
WriteBackForward (Log* k, long* versionPtr)
{
    AVPair* e;
    AVPair* End = k->put;
    for (e = k->List; e != End; e = e->Next) {
        *(e->Addr) = e->Valu;

        unsigned int offset = OFFSET(e->Addr);
        const version_t* LockFor = LockTab[offset];

        version_t* newVersion = (version_t*) malloc(sizeof(version_t));

        newVersion->addr = e->Addr;
        newVersion->data = e->Valu;
        newVersion->version = versionPtr;
        newVersion->previous = LockFor;
        LockTab[offset] = newVersion;
    }
}

void
TxOnce ()
{
	LockTab = (version_t**)malloc(_TABSZ * sizeof(version_t*));
	unsigned int i = 0;
	long* ver = (long*) malloc(sizeof(long));
	*ver = 0;
	for (; i < _TABSZ; i++) {
		LockTab[i] = (version_t*)malloc(sizeof(version_t));
		LockTab[i]->addr = NULL;
		LockTab[i]->data = NULL;
		LockTab[i]->version = ver;
		LockTab[i]->previous = NULL;
	}

    LOCK = (aligned_type_t*) malloc(sizeof(aligned_type_t));
    LOCK->value = 0;
    LOCK->counter = 0;

    pthread_key_create(&global_key_self, NULL); /* CCM: do before we register handler */

}


void
TxShutdown ()
{
    printf("NOREC system shutdown:\n"
           "  Starts=%li Aborts=%li\n"
           "  Overflows: R=%li W=%li L=%li\n"
           , StartTally, AbortTally,
           ReadOverflowTally, WriteOverflowTally, LocalOverflowTally);

    pthread_key_delete(global_key_self);

    MEMBARSTLD();
}


Thread*
TxNewThread ()
{
    Thread* t = (Thread*)malloc(sizeof(Thread));
    assert(t);

    return t;
}


void
TxFreeThread (Thread* t)
{
    AtomicAdd((volatile intptr_t*)((void*)(&ReadOverflowTally)),  t->rdSet.ovf);

    long wrSetOvf = 0;
    Log* wr;
    wr = &t->wrSet;
    {
        wrSetOvf += wr->ovf;
    }
    AtomicAdd((volatile intptr_t*)((void*)(&WriteOverflowTally)), wrSetOvf);

    AtomicAdd((volatile intptr_t*)((void*)(&StartTally)),         t->Starts);
    AtomicAdd((volatile intptr_t*)((void*)(&AbortTally)),         t->Aborts);

    FreeList(&(t->rdSet),     NOREC_INIT_RDSET_NUM_ENTRY);
    FreeList(&(t->wrSet),     NOREC_INIT_WRSET_NUM_ENTRY);

    free(t);
}

void
TxInitThread (Thread* t, long id)
{
    /* CCM: so we can access NOREC's thread metadata in signal handlers */
    pthread_setspecific(global_key_self, (void*)t);

    memset(t, 0, sizeof(*t));     /* Default value for most members */

    t->UniqID = id;
    t->rng = id + 1;
    t->xorrng[0] = t->rng;

    t->wrSet.List = MakeList(NOREC_INIT_WRSET_NUM_ENTRY, t);
    t->wrSet.put = t->wrSet.List;

    t->rdSet.List = MakeList(NOREC_INIT_RDSET_NUM_ENTRY, t);
    t->rdSet.put = t->rdSet.List;


}

__INLINE__ void
txReset (Thread* Self)
{
    Self->wrSet.put = Self->wrSet.List;
    Self->wrSet.tail = NULL;

    Self->wrSet.BloomFilter = 0;
    Self->rdSet.put = Self->rdSet.List;
    Self->rdSet.tail = NULL;
}

__INLINE__ void
txCommitReset (Thread* Self)
{
    txReset(Self);
    Self->Retries = 0;
}

// returns -1 if not coherent
__INLINE__ long
ReadSetCoherent (Thread* Self)
{
	Log* const rd = &Self->rdSet;
	AVPair* const EndOfList = rd->put;
	AVPair* e;

	long mySnapshot = Self->snapshot;

	for (e = rd->List; e != EndOfList; e = e->Next) {
		volatile version_t* LockFor = *(PSLOCK(e->Addr));
		if (*(LockFor->version) > mySnapshot) {
			return 1;
		}
	}
	return 0;
}


__INLINE__ void
backoff (Thread* Self, long attempt)
{
    unsigned long long stall = TSRandom(Self) & 0xF;
    stall += attempt >> 2;
    stall *= 10;
    /* CCM: timer function may misbehave */
    volatile typeof(stall) i = 0;
    while (i++ < stall) {
        PAUSE();
    }
}


__INLINE__ long
TryFastUpdate (Thread* Self)
{
    Log* const wr = &Self->wrSet;
    long ctr;

    do {
    	while (LOCK->value == 1) {
    		//_mm_pause();
    	}
    } while (CAS(&(LOCK->value), 0, 1) == 1);

    if (ReadSetCoherent(Self)) {
    	TxAbort(Self);
    }

    long* versionPtr = (long*) malloc(sizeof(long));

    {
        WriteBackForward(wr, versionPtr); /* write-back the deferred stores */
    }

    MEMBARSTST(); /* Ensure the above stores are visible  */
    long counter = LOCK->counter + 1;
    LOCK->counter = counter;
    *versionPtr = counter;
    LOCK->value = 0;
    MEMBARSTLD();

    return 1; /* success */
}

void
TxAbort (Thread* Self)
{
    Self->Retries++;
    Self->Aborts++;

    if (Self->Retries > 3) { /* TUNABLE */
        backoff(Self, Self->Retries);
    }

    SIGLONGJMP(*Self->envPtr, 1);
    ASSERT(0);
}


void
TxStore (Thread* Self, volatile intptr_t* addr, intptr_t valu)
{
    Log* k = &Self->wrSet;

    k->BloomFilter |= FILTERBITS(addr);

    AVPair* e = k->put;
    if (e == NULL) {
    	k->ovf++;
    	e = ExtendList(k->tail);
    	k->end = e;
    }
    k->tail    = e;
    k->put     = e->Next;
    e->Addr    = addr;
    e->Valu    = valu;
}


intptr_t
TxLoad (Thread* Self, volatile intptr_t* Addr)
{
    intptr_t Valu;

    intptr_t msk = FILTERBITS(Addr);
    if ((Self->wrSet.BloomFilter & msk) == msk) {
        Log* wr = &Self->wrSet;
        AVPair* e;
        for (e = wr->tail; e != NULL; e = e->Prev) {
            if (e->Addr == Addr) {
                return e->Valu;
            }
        }
    }

    MEMBARLDLD();
    intptr_t ret = *(Addr);
    volatile version_t* LockFor = *(PSLOCK(Addr));

    while (LockFor->addr != NULL && LockFor->addr != Addr) {
    	LockFor = LockFor->previous;
    }

    if (*(LockFor->version) > Self->snapshot) {
    	TxAbort(Self);
    }

    Log* k = &Self->rdSet;
    AVPair* e = k->put;
    if (e == NULL) {
        k->ovf++;
        e = ExtendList(k->tail);
        k->end = e;
    }
    k->tail    = e;
    k->put     = e->Next;
    e->Addr = Addr;

    if (LockFor->addr == Addr) {
    	return LockFor->data;
    } else {
    	return ret;
    }
}


void
TxStart (Thread* Self, sigjmp_buf* envPtr)
{
    txReset(Self);

    MEMBARLDLD();

    Self->envPtr= envPtr;

    Self->Starts++;

    Self->snapshot = LOCK->counter;
}

int
TxCommit (Thread* Self)
{
    /* Fast-path: Optional optimization for pure-readers */
    if (Self->wrSet.put == Self->wrSet.List)
    {
        txCommitReset(Self);
        return 1;
    }

    if (TryFastUpdate(Self)) {
        txCommitReset(Self);
        return 1;
    }

    TxAbort(Self);
    return 0;
}
