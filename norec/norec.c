#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include "platform.h"
#include "norec.h"
#include "tmalloc.h"
#include "util.h"


__INLINE__ long ReadSetCoherent (Thread*);


enum norec_config {
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
    tmalloc_t* allocPtr; /* CCM: speculatively allocated */
    tmalloc_t* freePtr;  /* CCM: speculatively free'd */
    Log rdSet;
    Log wrSet;
    Log LocalUndo;
    sigjmp_buf* envPtr;
};

typedef struct
{
    long value;
    long padding1;
    long padding2;
    long padding3;
    long padding4;
    long padding5;
    long padding6;
    long padding7;
} aligned_type_t ;

__attribute__((aligned(64))) volatile aligned_type_t* LOCK;


static pthread_key_t    global_key_self;
static struct sigaction global_act_oldsigbus;
static struct sigaction global_act_oldsigsegv;

void TxIncClock() {
    LOCK->value+=2;
}

static void
useAfterFreeHandler (int signum, siginfo_t* siginfo, void* context)
{
    Thread* Self = (Thread*)pthread_getspecific(global_key_self);

    if (ReadSetCoherent(Self) == -1) {
        TxAbort(Self);
    }

    psignal(signum, NULL);
    abort();
}


/* =============================================================================
 * registerUseAfterFreeHandler
 * =============================================================================
 */
static void
registerUseAfterFreeHandler ()
{
    struct sigaction act;

    memset(&act, sizeof(struct sigaction), 0);
    act.sa_sigaction = &useAfterFreeHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART | SA_SIGINFO;

    if (sigaction(SIGBUS, &act, &global_act_oldsigbus) != 0) {
        perror("Error: Failed to register SIGBUS handler");
        exit(1);
    }

    if (sigaction(SIGSEGV, &act, &global_act_oldsigsegv) != 0) {
        perror("Error: Failed to register SIGSEGV handler");
        exit(1);
    }
}


/* =============================================================================
 * restoreUseAfterFreeHandler
 * =============================================================================
 */
static void
restoreUseAfterFreeHandler ()
{
    if (sigaction(SIGBUS, &global_act_oldsigbus, NULL) != 0) {
        perror("Error: Failed to restore SIGBUS handler");
        exit(1);
    }

    if (sigaction(SIGSEGV, &global_act_oldsigsegv, NULL) != 0) {
        perror("Error: Failed to restore SIGSEGV handler");
        exit(1);
    }
}

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
WriteBackForward (Log* k)
{
    AVPair* e;
    AVPair* End = k->put;
    for (e = k->List; e != End; e = e->Next) {
        *(e->Addr) = e->Valu;
    }
}


__INLINE__ void
RecordStore (Log* k, volatile intptr_t* Addr, intptr_t Valu)
{
    AVPair* e = k->put;
    if (e == NULL) {
        k->ovf++;
        e = ExtendList(k->tail);
        k->end = e;
    }
    ASSERT(Addr != NULL);
    k->tail    = e;
    k->put     = e->Next;
    e->Addr    = Addr;
    e->Valu    = Valu;
}

void
TxOnce ()
{
    LOCK = (aligned_type_t*) malloc(sizeof(aligned_type_t));
    LOCK->value = 0;

    pthread_key_create(&global_key_self, NULL); /* CCM: do before we register handler */

    registerUseAfterFreeHandler();
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

    restoreUseAfterFreeHandler();

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

    AtomicAdd((volatile intptr_t*)((void*)(&LocalOverflowTally)), t->LocalUndo.ovf);

    AtomicAdd((volatile intptr_t*)((void*)(&StartTally)),         t->Starts);
    AtomicAdd((volatile intptr_t*)((void*)(&AbortTally)),         t->Aborts);

    tmalloc_free(t->allocPtr);
    tmalloc_free(t->freePtr);

    FreeList(&(t->rdSet),     NOREC_INIT_RDSET_NUM_ENTRY);
    FreeList(&(t->wrSet),     NOREC_INIT_WRSET_NUM_ENTRY);
    FreeList(&(t->LocalUndo), NOREC_INIT_LOCAL_NUM_ENTRY);

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

    t->LocalUndo.List = MakeList(NOREC_INIT_LOCAL_NUM_ENTRY, t);
    t->LocalUndo.put = t->LocalUndo.List;

    t->allocPtr = tmalloc_alloc(1);
    assert(t->allocPtr);
    t->freePtr = tmalloc_alloc(1);
    assert(t->freePtr);

}

__INLINE__ void
txReset (Thread* Self)
{
    Self->wrSet.put = Self->wrSet.List;
    Self->wrSet.tail = NULL;

    Self->wrSet.BloomFilter = 0;
    Self->rdSet.put = Self->rdSet.List;
    Self->rdSet.tail = NULL;

    Self->LocalUndo.put = Self->LocalUndo.List;
    Self->LocalUndo.tail = NULL;
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
    long time;
    while (1) {
        time = LOCK->value;
        if ((time & 1) != 0) {
            continue;
        }

        Log* const rd = &Self->rdSet;
        AVPair* const EndOfList = rd->put;
        AVPair* e;

        for (e = rd->List; e != EndOfList; e = e->Next) {
            if (e->Valu != LDNF(e->Addr)) {
                return -1;
            }
        }

        if (LOCK->value == time)
            break;
    }
    return time;
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

    while (CAS(&(LOCK->value), Self->snapshot, Self->snapshot + 1) != Self->snapshot) {
        long newSnap = ReadSetCoherent(Self);
        if (newSnap == -1) {
            TxAbort(Self);
        }
        Self->snapshot = newSnap;
    }

    {
        WriteBackForward(wr); /* write-back the deferred stores */
    }
    MEMBARSTST(); /* Ensure the above stores are visible  */
    LOCK->value = Self->snapshot + 2;
    MEMBARSTLD();

    return 1; /* success */
}

__INLINE__ void
WriteBackReverse (Log* k)
{
    AVPair* e;
    for (e = k->tail; e != NULL; e = e->Prev) {
        *(e->Addr) = e->Valu;
    }
}

void
TxAbort (Thread* Self)
{
    if (Self->LocalUndo.put != Self->LocalUndo.List) {
        WriteBackReverse(&Self->LocalUndo);
    }

    Self->Retries++;
    Self->Aborts++;

    if (Self->Retries > 3) { /* TUNABLE */
        backoff(Self, Self->Retries);
    }

    tmalloc_releaseAllReverse(Self->allocPtr);
    tmalloc_clear(Self->freePtr);

    SIGLONGJMP(*Self->envPtr, 1);
    ASSERT(0);
}


void
TxStore (Thread* Self, volatile intptr_t* addr, intptr_t valu)
{
    Log* wr = &Self->wrSet;
    MEMBARLDLD();

    wr->BloomFilter |= FILTERBITS(addr);
    RecordStore(wr, addr, valu);
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
            ASSERT(e->Addr != NULL);
            if (e->Addr == Addr) {
                return e->Valu;
            }
        }
    }

    MEMBARLDLD();
    Valu = LDNF(Addr);
    while (LOCK->value != Self->snapshot) {
        long newSnap = ReadSetCoherent(Self);
        if (newSnap == -1) {
            TxAbort(Self);
        }
        Self->snapshot = newSnap;
        MEMBARLDLD();
        Valu = LDNF(Addr);
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
    e->Valu = Valu;

    return Valu;
}

__INLINE__ void
SaveForRollBack (Log* k, volatile intptr_t* Addr, intptr_t Valu)
{
    AVPair* e = k->put;
    if (e == NULL) {
        k->ovf++;
        e = ExtendList(k->tail);
        k->end = e;
    }
    k->tail    = e;
    k->put     = e->Next;
    e->Addr    = Addr;
    e->Valu    = Valu;
}

void
TxStoreLocal (Thread* Self, volatile intptr_t* Addr, intptr_t Valu)
{
    SaveForRollBack(&Self->LocalUndo, Addr, *Addr);
    *Addr = Valu;
}

void
TxStart (Thread* Self, sigjmp_buf* envPtr)
{
    txReset(Self);

    MEMBARLDLD();

    Self->envPtr= envPtr;

    ASSERT(Self->wrSet.put == Self->wrSet.List);

    Self->Starts++;

    do {
        Self->snapshot = LOCK->value;
    } while ((Self->snapshot & 1) != 0);

}

int
TxCommit (Thread* Self)
{
    /* Fast-path: Optional optimization for pure-readers */
    if (Self->wrSet.put == Self->wrSet.List)
    {
        /* Given NOREC the read-set is already known to be coherent. */
        txCommitReset(Self);
        tmalloc_clear(Self->allocPtr);
        tmalloc_releaseAllForward(Self->freePtr);
        return 1;
    }

    if (TryFastUpdate(Self)) {
        txCommitReset(Self);
        tmalloc_clear(Self->allocPtr);
        tmalloc_releaseAllForward(Self->freePtr);
        return 1;
    }

    TxAbort(Self);
    ASSERT(0);

    return 0;
}

long TxValidate (Thread* Self) {
    if (Self->wrSet.put == Self->wrSet.List) {
        return -1;
    } else {
        long local_global_clock = LOCK->value;

        while (1) {
            Log* const rd = &Self->rdSet;
            AVPair* const EndOfList = rd->put;
            AVPair* e;

            for (e = rd->List; e != EndOfList; e = e->Next) {
                if (e->Valu != LDNF(e->Addr)) {
                    TxAbort(Self);
                }
            }

            long tmp = LOCK->value;
            if (local_global_clock == tmp) {
                return local_global_clock;
            } else {
                local_global_clock = tmp;
            }
        }
        return local_global_clock;
    }
}


long TxFinalize (Thread* Self, long clock) {
    if (Self->wrSet.put == Self->wrSet.List) {
        txCommitReset(Self);
        tmalloc_clear(Self->allocPtr);
        tmalloc_releaseAllForward(Self->freePtr);
        return 0;
    }

    if (LOCK->value != clock) {
        return 1;
    }

    Log* const wr = &Self->wrSet;
    WriteBackForward(wr); /* write-back the deferred stores */
    LOCK->value+=2;

    txCommitReset(Self);
    tmalloc_clear(Self->allocPtr);
    tmalloc_releaseAllForward(Self->freePtr);

    return 0;
}



void*
TxAlloc (Thread* Self, size_t size)
{
    void* ptr = tmalloc_reserve(size);
    if (ptr) {
        tmalloc_append(Self->allocPtr, ptr);
    }

    return ptr;
}

void
TxFree (Thread* Self, void* ptr)
{
    tmalloc_append(Self->freePtr, ptr);
}


