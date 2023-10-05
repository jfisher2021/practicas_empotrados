#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS 4
#define NUM_ITERATIONS 5
#define PERIOD_MS 900000000
#define EXECUTION_TIME_MS 400000000ULL
#define Ciclo 0.5

// function to get the current time
double get_time() {
    /* variable to store the current time
    it returns a structure of time divided into seconds and nanoseconds */
    struct timespec t;
    // clock_gettime() returns the current time of the clock CLOCK_REALTIME
    clock_gettime(CLOCK_REALTIME, &t);

    return (double)t.tv_sec + t.tv_nsec / 1e9;
}
void* periodic_thread(void* thread_id) {
    int tid = *((int*)thread_id);
    double start_time, end_time, cost;
    // loop to execute the thread multiple times
    for (int i = 1; i <= NUM_ITERATIONS; i++) {
        /* Calculate the start time with the function get_time()
         returns the current time on this format (seconds.nanoseconds) */
        start_time = get_time();

        volatile unsigned long long j;
        for (j = 0; j < EXECUTION_TIME_MS; j++);
        end_time = get_time();

        // Calculate the cost of the execution
        cost = end_time - start_time;

        // Coheck if the cost is greater than the period
        if (cost > Ciclo) {
            printf("[%f] Thread %d - Iteration %d: Cost=%f s. (fallo temporal)\n",
                   start_time, tid, i, cost);
        } else {
            printf("[%f] Thread %d - Iteration %d: Cost=%f s.\n",
                   start_time, tid, i, cost);
        }

        // Wait the remaining time to complete the period
        sleep((double)PERIOD_MS / 1e9 - cost);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // create the threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, periodic_thread, &thread_ids[i])) {
            fprintf(stderr, "Error al crear el hilo %d\n", i + 1);
            return 1;
        }
    }

    // wait for the threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
