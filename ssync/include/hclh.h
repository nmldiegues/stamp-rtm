//hierarchical clh lock
#ifndef _HCLH_H_
#define _HCLH_H_

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

typedef struct node_fields {
     volatile uint8_t successor_must_wait;
     volatile uint8_t tail_when_spliced;
     volatile uint8_t cluster_id;
} node_fields;

typedef struct qnode {
    union {
        volatile uint32_t data;
        node_fields fields;
#ifdef ADD_PADDING
        volatile uint8_t padding[CACHE_LINE_SIZE];
#endif
    };
} qnode;

typedef volatile qnode *qnode_ptr;
typedef qnode_ptr local_queue;
typedef qnode_ptr global_queue;

//global parameters needed to oerate with a lock
typedef struct hclh_global_params {
    global_queue* shared_queue;
    local_queue** local_queues;
    volatile uint32_t* init_done;
#ifdef ADD_PADDING
#if CACHE_LINE_SIZE == 16
#else
    volatile uint8_t padding[CACHE_LINE_SIZE-20];
#endif
#endif

} hclh_global_params;

//thread local parameters
typedef struct hclh_local_params {
    qnode* my_qnode;
    qnode* my_pred;
    local_queue* my_queue;
} hclh_local_params;



/*
 *  Methods aiding with array of locks manipulation
 */

hclh_global_params** init_hclh_array_global(uint32_t num_locks);


hclh_local_params** init_hclh_array_local(uint32_t thread_num, uint32_t num_locks, hclh_global_params** the_params);


void end_hclh_array_local(hclh_local_params** local_params, uint32_t size);


void end_hclh_array_global(hclh_global_params** global_params, uint32_t size);

//single lock manipluation
hclh_global_params* init_hclh_global();


hclh_local_params* init_hclh_local(uint32_t thread_num, hclh_global_params* the_params);


void end_hclh_local(hclh_local_params* local_params);


void end_hclh_global(hclh_global_params* global_params);


//lock
volatile qnode * hclh_acquire(local_queue *lq, global_queue *gq, qnode *my_qnode);

//unlock
qnode * hclh_release(qnode *my_qnode, qnode * my_pred, uint8_t th_cluster);


int is_free_hclh(local_queue *lq, global_queue *gq, qnode *my_qnode);

#endif
