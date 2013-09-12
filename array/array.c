#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "thread.h"
#include "tm.h"
#include "timer.h"

enum param_types {
    PARAM_SIZE = (unsigned char)'s',
    PARAM_THREADS = (unsigned char)'t',
    PARAM_OPERATIONS = (unsigned char)'o',
    PARAM_INTERVAL = (unsigned char)'i',
};

#define PARAM_DEFAULT_SIZE (1024)
#define PARAM_DEFAULT_THREADS (2)
#define PARAM_DEFAULT_OPERATIONS (10000)
#define PARAM_DEFAULT_INTERVAL (1000)

double global_params[256];

long* global_array;

static void setDefaultParams() {
    global_params[PARAM_SIZE] = PARAM_DEFAULT_SIZE;
    global_params[PARAM_THREADS] = PARAM_DEFAULT_THREADS;
    global_params[PARAM_OPERATIONS] = PARAM_DEFAULT_OPERATIONS;
    global_params[PARAM_INTERVAL] = PARAM_DEFAULT_INTERVAL;
}

static void parseArgs(long argc, char* const argv[]) {
    long i;
    long opt;
    opterr = 0;

    setDefaultParams();

    while ((opt = getopt(argc, argv, "s:t:o:i:")) != -1) {
        switch (opt) {
        case 's':
        case 'o':
        case 't':
        case 'i':
            global_params[(unsigned char)opt] = atol(optarg);
            break;
        case '?':
        default:
            opterr++;
            break;
        }
    }

    for (i = optind; i < argc; i++) {
        fprintf(stderr, "Non-option argument: %s\n", argv[i]);
        opterr++;
    }

    if (opterr) {
        printf("Wrong usage\n");
        exit(1);
    }
}

volatile long dummy = 1;

void client_run (void* argPtr) {
    TM_THREAD_ENTER();

    // unsigned long myId = thread_getId();
    // long numThread = *((long*)argPtr);
    long operations = (long)global_params[PARAM_OPERATIONS];

    long total = 0;

    long i = 0;
    for (; i < operations; i++) {
        long random_number = ((long) rand()) % ((long)global_params[PARAM_SIZE]);
        long random_number2 = ((long) rand()) % ((long)global_params[PARAM_SIZE]);
        if (random_number == random_number2) {
            random_number2 = (random_number2 + 1) % ((long)global_params[PARAM_SIZE]);
        }
        TM_BEGIN();
        long r1 = (long)TM_SHARED_READ(global_array[random_number]);
        long r2 = (long)TM_SHARED_READ(global_array[random_number2]);
        r1 = r1 + 1;
        r2 = r2 - 1;
        TM_SHARED_WRITE(global_array[random_number], r1);
        TM_SHARED_WRITE(global_array[random_number2], r2);
        TM_END();

        long k = 0;
        for (;k < (long)global_params[PARAM_INTERVAL]; k++) {
            long ru = (long) dummy % 2;
            total += ru;
        }
    }

    TM_THREAD_EXIT();
    printf("ru ignore %ld\n", total);
}

MAIN(argc, argv) {
    TIMER_T start;
    TIMER_T stop;

    GOTO_REAL();

    parseArgs(argc, (char** const)argv);
    long sz = (long)global_params[PARAM_SIZE];
    global_array = (long*) malloc(sz * sizeof(long));

    long numThread = global_params[PARAM_THREADS];
    SIM_GET_NUM_CPU(numThread);
    TM_STARTUP(numThread);
    P_MEMORY_STARTUP(numThread);
    thread_startup(numThread);

    printf("Running clients... ");
    fflush(stdout);

    TIMER_READ(start);
    GOTO_SIM();

    thread_start(client_run, (void*)&numThread);

    GOTO_REAL();
    TIMER_READ(stop);
    puts("done.");
    printf("Time = %0.6lf\n", TIMER_DIFF_SECONDS(start, stop));
    fflush(stdout);

    long i = 0;
    long sum = 0;
    for (;i < sz; i++) {
        sum += global_array[i];
    }
    if (sum != 0) {
        printf("Problem, sum was not zero!: %ld\n", sum);
    }

    TM_SHUTDOWN();
    P_MEMORY_SHUTDOWN();
    GOTO_SIM();
    thread_shutdown();
    MAIN_RETURN(0);
}


