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
#define TIME 6

int *num_iterations;
uint64_t *latency_sum;
uint64_t *latency_max;

static int32_t latency_target_value = 0;
static int32_t latency_target_fd = -1;

struct file_csv {
    int *cpu_array;
    int *num_iterations_array;
    uint64_t *latency_array;
};

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
    struct file_csv *my_struct = (struct file_csv *)malloc(sizeof(struct file_csv));
    memset(my_struct, 0, sizeof(struct file_csv));
    double start_time = get_time();
    double current_time = 0.0;
    cpu_set_t cpuset;
    pthread_t thread = pthread_self();
    struct sched_param param;
    uint64_t latency;
    int ret;

    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);

    if (my_struct == NULL) {
        fprintf(stderr, "Error allocating memory for my_struct\n");
        exit(EXIT_FAILURE);
    }

    my_struct->cpu_array = (int *)malloc(6000 * sizeof(int));
    memset(my_struct->cpu_array, 0, 6000 * sizeof(int));
    my_struct->num_iterations_array = (int *)malloc(6000 * sizeof(int));
    memset(my_struct->num_iterations_array, 0, 6000 * sizeof(int));
    my_struct->latency_array = (uint64_t *)malloc(6000 * sizeof(uint64_t));
    memset(my_struct->latency_array, 0, 6000 * sizeof(uint64_t));

    if (my_struct->cpu_array == NULL || my_struct->num_iterations_array == NULL || my_struct->latency_array == NULL) {
        fprintf(stderr, "Error allocating memory for arrays in my_struct\n");
        free(my_struct);
        exit(EXIT_FAILURE);
    }

    ret = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (ret != 0) {
        fprintf(stderr, "Error setting thread affinity: %s\n", strerror(errno));
        free(my_struct);
        exit(EXIT_FAILURE);
    }

    param.sched_priority = 99;
    ret = pthread_setschedparam(thread, SCHED_FIFO, &param);
    if (ret != 0) {
        fprintf(stderr, "Error setting thread priority: %s\n", strerror(errno));
        free(my_struct);
        exit(EXIT_FAILURE);
    }

    start_time = get_time();                      // Obtener el tiempo al inicio
    while ((current_time - start_time) < TIME) {  // Comprobar si han pasado 60 segundos
        num_iterations[cpu]++;

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        usleep(10000);  // 10 ms
        clock_gettime(CLOCK_MONOTONIC, &end);
        latency = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec) - 10000000;
        latency_sum[cpu] += latency;
        if (latency > latency_max[cpu]) {
            latency_max[cpu] = latency;
        }

        current_time = get_time();  // Actualizar el tiempo actual
        my_struct->num_iterations_array[num_iterations[cpu]] = num_iterations[cpu];
        my_struct->cpu_array[num_iterations[cpu]] = cpu;
        my_struct->latency_array[num_iterations[cpu]] = latency;
    }

    pthread_exit((void *)my_struct);
}

int main() {
    int NUM_THREADS = (int)sysconf(_SC_NPROCESSORS_ONLN);  // Obtener el número de núcleos
    pthread_t threads[NUM_THREADS];
    int cpu_list[NUM_THREADS], ret = 0, total_num_iterations = 0;
    uint64_t total_latency_sum = 0;
    uint64_t total_latency_max = 0;
    NUM_THREADS = sysconf(_SC_NPROCESSORS_ONLN);
    num_iterations = (int *)malloc(NUM_THREADS * sizeof(int));
    memset(num_iterations, 0, NUM_THREADS * sizeof(int));
    latency_sum = (uint64_t *)malloc(NUM_THREADS * sizeof(uint64_t));
    memset(latency_sum, 0, NUM_THREADS * sizeof(uint64_t));
    latency_max = (uint64_t *)malloc(NUM_THREADS * sizeof(uint64_t));
    memset(latency_max, 0, NUM_THREADS * sizeof(uint64_t));
    struct file_csv *my_structs[NUM_THREADS];

    if (num_iterations == NULL || latency_sum == NULL || latency_max == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
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
    FILE *csv = fopen(CSV_FILE, "w");
    if (csv == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        ret = pthread_join(threads[i], (void **)&my_structs[i]);
        if (ret != 0) {
            fprintf(stderr, "Error joining thread: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        total_latency_sum += latency_sum[i];
        total_num_iterations += num_iterations[i];

        if (latency_max[i] > total_latency_max) {
            total_latency_max = latency_max[i];
        }
        for (int j = 0; j < 6000; j++) {
            fprintf(csv, "%d,%d,%lu\n", my_structs[i]->cpu_array[j], my_structs[i]->num_iterations_array[j], my_structs[i]->latency_array[j]);
        }

        free(my_structs[i]->cpu_array);
        free(my_structs[i]->num_iterations_array);
        free(my_structs[i]->latency_array);
        free(my_structs[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        printf("i = %d\n", i);
        printf("[%d] latencia media = %06lu ns. | max = %07lu ns | num_iterations[%d] == %d \n", i,
         latency_sum[i] / num_iterations[i], latency_max[i], i, num_iterations[i]);
    }
    printf("Total latencia media = %06lu ns. | max = %07lu ns, total_num_iterations= %d \n",
     total_latency_sum / (NUM_THREADS * (total_num_iterations / NUM_THREADS)), total_latency_max, total_num_iterations);
    fclose(csv);
    close(latency_target_fd);
    free(num_iterations);
    free(latency_sum);
    free(latency_max);

    return 0;
}
