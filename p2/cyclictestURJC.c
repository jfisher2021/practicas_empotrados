#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define CPU_LATENCY_FILE "/dev/cpu_dma_latency"
#define CSV_FILE "cyclictestURJC.csv"

int NUM_THREADS;
int *num_iterations;
uint64_t *latency_sum;
uint64_t *latency_max;

static int32_t latency_target_value = 0;
static int32_t latency_target_fd = -1;

double get_time() {
    /* variable to store the current time
    it returns a structure of time divided into seconds and nanoseconds */
    struct timespec t;
    // clock_gettime() returns the current time of the clock CLOCK_REALTIME
    clock_gettime(CLOCK_REALTIME, &t);

    return (double)t.tv_sec + t.tv_nsec / 1e9;
}

void *thread_func(void *arg) {
    int cpu = *(int *)arg;
    double start_time = get_time(), current_time;
    cpu_set_t cpuset;
    pthread_t thread = pthread_self();
    struct sched_param param;
    uint64_t latency;
    int ret;

    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);

    ret = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (ret != 0) {
        fprintf(stderr, "Error setting thread affinity: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    param.sched_priority = 99;
    ret = pthread_setschedparam(thread, SCHED_FIFO, &param);
    if (ret != 0) {
        fprintf(stderr, "Error setting thread priority: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    FILE *csv = fopen(CSV_FILE, "w");
    start_time = get_time();                     // Obtener el tiempo al inicio
    while ((current_time - start_time) < 60.0) {  // Comprobar si han pasado 60 segundos
        num_iterations[cpu]++;
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
	usleep(1000);
        clock_gettime(CLOCK_MONOTONIC, &end);
        latency = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
        latency_sum[cpu] += latency;
        if (latency > latency_max[cpu]) {
            latency_max[cpu] = latency;
        }
        if (csv == NULL) {
            fprintf(stderr, "Error opening CSV file: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        fprintf(csv, "%d,%d,%lu\n", cpu, num_iterations[cpu], latency);

        current_time = get_time();  // Actualizar el tiempo actual
    }

    fclose(csv);

    return NULL;
}
int main() {
    int NUM_THREADS = (int)sysconf(_SC_NPROCESSORS_ONLN);  // Obtener el número de núcleos
    pthread_t threads[NUM_THREADS];
    int cpu_list[NUM_THREADS], ret, total_num_iterations = 0;
    uint64_t total_latency_sum = 0;
    uint64_t total_latency_max = 0;
    NUM_THREADS = sysconf(_SC_NPROCESSORS_ONLN);
    num_iterations = (int *)calloc(NUM_THREADS, sizeof(int));
    latency_sum = (uint64_t *)calloc(NUM_THREADS, sizeof(uint64_t));
    latency_max = (uint64_t *)calloc(NUM_THREADS, sizeof(uint64_t));
    // Inicializar num_iterations como un array
    latency_target_fd = open(CPU_LATENCY_FILE, O_RDWR);
    if (latency_target_fd < 0) {
        fprintf(stderr, "Error opening CPU latency file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    ret = write(latency_target_fd, &latency_target_value, sizeof(latency_target_value));
    if (ret < 0) {
        fprintf(stderr, "Error setting CPU latency: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        cpu_list[i] = i;
        ret = pthread_create(&threads[i], NULL, thread_func, &cpu_list[i]);
        if (ret != 0) {
            fprintf(stderr, "Error creating thread: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        ret = pthread_join(threads[i], NULL);
        if (ret != 0) {
            fprintf(stderr, "Error joining thread: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        total_latency_sum += latency_sum[i];
        total_num_iterations += num_iterations[i];

        if (latency_max[i] > total_latency_max) {
            total_latency_max = latency_max[i];
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        printf("[%d] latencia media = %09lu ns. | max = %09lu ns | num_iterations[] == %d \n", i, latency_sum[i] / num_iterations[i], latency_max[i], num_iterations[i]);
    }
    printf("Total latencia media = %09lu ns. | max = %09lu ns, total_num_iterations= %d \n", total_latency_sum / (NUM_THREADS * (total_num_iterations / NUM_THREADS)), total_latency_max, total_num_iterations);

    close(latency_target_fd);
    free(num_iterations);
    free(latency_sum);
    free(latency_max);

    return 0;
}
