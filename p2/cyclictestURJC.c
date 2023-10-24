#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS 8
#define NUM_ITERATIONS 10000
#define CPU_LATENCY_FILE "/dev/cpu_dma_latency"
#define CSV_FILE "cyclictestURJC.csv"

static int32_t latency_target_value = 0;
static int32_t latency_target_fd = -1;
static uint64_t latency_sum[NUM_THREADS] = {0};
static uint64_t latency_max[NUM_THREADS] = {0};

void *thread_func(void *arg) {
    int cpu = *(int *)arg;
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

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        usleep(1000);
        clock_gettime(CLOCK_MONOTONIC, &end);
        latency = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
        latency_sum[cpu] += latency;
        if (latency > latency_max[cpu]) {
            latency_max[cpu] = latency;
        }
        FILE *csv = fopen(CSV_FILE, "a");
        if (csv == NULL) {
            fprintf(stderr, "Error opening CSV file: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        fprintf(csv, "%d,%d,%lu\n", cpu, i, latency);
        fclose(csv);
    }

    return NULL;
}

int main() {
    int ret;
    pthread_t threads[NUM_THREADS];
    int cpu_list[NUM_THREADS];
    uint64_t total_latency_sum = 0;
    uint64_t total_latency_max = 0;

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
        if (latency_max[i] > total_latency_max) {
            total_latency_max = latency_max[i];
        }
    }

    printf("[0] latencia media = %09lu ns. | max = %09lu ns\n", latency_sum[0] / NUM_ITERATIONS, latency_max[0]);
    printf("[1] latencia media = %09lu ns. | max = %09lu ns\n", latency_sum[1] / NUM_ITERATIONS, latency_max[1]);
    printf("[2] latencia media = %09lu ns. | max = %09lu ns\n", latency_sum[2] / NUM_ITERATIONS, latency_max[2]);
    printf("[3] latencia media = %09lu ns. | max = %09lu ns\n", latency_sum[3] / NUM_ITERATIONS, latency_max[3]);
    printf("[4] latencia media = %09lu ns. | max = %09lu ns\n", latency_sum[4] / NUM_ITERATIONS, latency_max[4]);
    printf("[5] latencia media = %09lu ns. | max = %09lu ns\n", latency_sum[5] / NUM_ITERATIONS, latency_max[5]);
    printf("[6] latencia media = %09lu ns. | max = %09lu ns\n", latency_sum[6] / NUM_ITERATIONS, latency_max[6]);
    printf("[7] latencia media = %09lu ns. | max = %09lu ns\n", latency_sum[7] / NUM_ITERATIONS, latency_max[7]);
    printf("Total latencia media = %09lu ns. | max = %09lu ns\n", total_latency_sum / (NUM_THREADS * NUM_ITERATIONS), total_latency_max);

    close(latency_target_fd);

    return 0;
}
