//test-and-test-and-set lock with backoff

#ifndef _TTAS_H_
#define _TTAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <numa.h>
#include <pthread.h>
#include "atomic_ops.h"
#include "utils.h"


#define MIN_DELAY 100
#define MAX_DELAY 1000

typedef volatile uint32_t ttas_index_t;
typedef uint8_t ttas_lock_data_t;

typedef struct ttas_lock_t {
    union {
        ttas_lock_data_t lock;
#ifdef ADD_PADDING
        uint8_t padding[CACHE_LINE_SIZE];
#else
        uint8_t padding;
#endif
    };
}ttas_lock_t;


static inline uint32_t backoff(uint32_t limit) {
    uint32_t delay = rand()%limit;
    limit = MAX_DELAY > 2*limit ? 2*limit : MAX_DELAY;
    cdelay(delay);
    return limit;

}

//lock the 
void ttas_lock(ttas_lock_t* the_lock, uint32_t* limit);
int ttas_trylock(ttas_lock_t* the_lock, uint32_t* limit);

//unlock the lock with the given index
void ttas_unlock(ttas_lock_t* the_lock);


int is_free_ttas(ttas_lock_t * the_lock);
/*
    Some methods for easy lock array manipluation
 */

ttas_lock_t* init_ttas_array_global(uint32_t num_locks);


uint32_t* init_ttas_array_local(uint32_t thread_num, uint32_t size);


void end_ttas_array_local(uint32_t* limits);


void end_ttas_array_global(ttas_lock_t* the_locks);

ttas_lock_t init_ttas_global();


uint32_t init_ttas_local(uint32_t thread_num);


void end_ttas_local();


void end_ttas_global();


#endif


