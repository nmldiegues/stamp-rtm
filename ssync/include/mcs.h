//mcs lock
#ifndef _MCS_H_
#define _MCS_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <numa.h>
#include <pthread.h>
#include "utils.h"
#include "atomic_ops.h"

typedef struct mcs_qnode {
    volatile uint8_t waiting;
    volatile struct mcs_qnode *volatile next;
#ifdef ADD_PADDING
#if CACHE_LINE_SIZE == 16
#else
    uint8_t padding[CACHE_LINE_SIZE - 16];
#endif
#endif
} mcs_qnode;

typedef volatile mcs_qnode *mcs_qnode_ptr;
typedef mcs_qnode_ptr mcs_lock; //initialized to NULL


typedef struct mcs_global_params {
    mcs_lock* the_lock;
#ifdef ADD_PADDING
    uint8_t padding[CACHE_LINE_SIZE - 8];
#endif
} mcs_global_params;

//initializes the lock
//mcs_lock *init_mcs(uint32_t capacity, int do_global_init, char *key);

/*
    Methods for easy lock array manipulation
 */

mcs_global_params* init_mcs_array_global(uint32_t num_locks);

mcs_qnode** init_mcs_array_local(uint32_t thread_num, uint32_t num_locks);

void end_mcs_array_local(mcs_qnode** the_qnodes, uint32_t size);

void end_mcs_array_global(mcs_global_params* the_locks, uint32_t size);
/*
    single lock manipulation
 */

mcs_global_params init_mcs_global();

mcs_qnode* init_mcs_local(uint32_t thread_num);

void end_mcs_local(mcs_qnode* the_qnodes);

void end_mcs_global(mcs_global_params the_locks);

//lock
void mcs_acquire(mcs_lock *the_lock, mcs_qnode_ptr I);

//unlock
void mcs_release(mcs_lock *the_lock, mcs_qnode_ptr I);

int is_free_mcs(mcs_lock *L );

int mcs_trylock(mcs_lock *L, mcs_qnode_ptr I);
#endif
