//test-and-test-and-set lock
#include "ttas.h"

#define UNLOCKED 0
#define LOCKED 1

__thread unsigned long * ttas_seeds;


int ttas_trylock(ttas_lock_t * the_lock, uint32_t * limits) {
    if (CAS_U8(&(the_lock->lock),UNLOCKED,LOCKED)==UNLOCKED) return 0;
    return 1;
}

void ttas_lock(ttas_lock_t * the_lock, uint32_t* limit) {
    uint32_t delay;
    volatile ttas_lock_data_t* l = &(the_lock->lock);
    while (1){
        while ((*l)==1) {}
        if (CAS_U8(l,UNLOCKED,LOCKED)==UNLOCKED) {
            return;
        } else {
            //backoff
            delay = my_random(&(ttas_seeds[0]),&(ttas_seeds[1]),&(ttas_seeds[2]))%(*limit);
            *limit = MAX_DELAY > 2*(*limit) ? 2*(*limit) : MAX_DELAY;
            cdelay(delay);
        }
    }
}


int is_free_ttas(ttas_lock_t * the_lock){
    if (the_lock->lock==UNLOCKED) return 1;
    return 0;
}

void ttas_unlock(ttas_lock_t *the_lock) 
{
    the_lock->lock=0;
}


/*
    Some methods for easy lock array manipulation
 */


//ttas
ttas_lock_t* init_ttas_array_global(uint32_t num_locks) {

    ttas_lock_t* the_locks;
    the_locks = (ttas_lock_t*)malloc(num_locks * sizeof(ttas_lock_t));
    uint32_t i;
    for (i = 0; i < num_locks; i++) {
        the_locks[i].lock=0;
    }
    return the_locks;
}

uint32_t* init_ttas_array_local(uint32_t thread_num, uint32_t size){
    //assign the thread to the correct core
    set_cpu(thread_num);
    ttas_seeds = seed_rand();

    uint32_t* limits;
    limits = (uint32_t*)malloc(size * sizeof(uint32_t));
    uint32_t i;
    for (i = 0; i < size; i++) {
        limits[i]=1;
    }
    return limits;
}

void end_ttas_array_local(uint32_t* limits) {
    free(limits);
}

void end_ttas_array_global(ttas_lock_t* the_locks) {
    free(the_locks);
}

ttas_lock_t init_ttas_global() {

    ttas_lock_t the_lock;
    the_lock.lock=0;
    return the_lock;
}

uint32_t init_ttas_local(uint32_t thread_num){
    //assign the thread to the correct core
    set_cpu(thread_num);
    ttas_seeds = seed_rand();
    return 1;
}

void end_ttas_local() {
    //function not needed
}

void end_ttas_global() {
    //function not needed
}

