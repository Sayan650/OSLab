#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>

#define STACK_SIZE (1024 * 1024) 
typedef struct {
    int start;
    int end;
    int thread_id;
} thread_data;
bool is_prime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}
int prime_finder(void *arg) {
    thread_data *data = (thread_data *)arg;
    printf("Thread %d finding primes in range [%d, %d]\n", data->thread_id, data->start, data->end);

    for (int i = data->start; i <= data->end; i++) {
        if (is_prime(i)) {
            printf("Thread %d found prime: %d\n", data->thread_id, i);
        }
    }
    return 0;
}

int main() {
    int N = 100;    
    int M = 2;    
    char *stacks[M];
    for (int i = 0; i < M; i++) {
        stacks[i] = malloc(STACK_SIZE);
        if (stacks[i] == NULL) {
            perror("Failed to allocate stack");
            exit(1);
        }
    }
    thread_data t_data[M];
    int range_per_thread = N / M;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    pid_t pids[M];
    for (int i = 0; i < M; i++) {
        t_data[i].start = i * range_per_thread + 1;
        t_data[i].end = (i == M - 1) ? N : (i + 1) * range_per_thread;
        t_data[i].thread_id = i + 1;
        
        pids[i] = clone(prime_finder, stacks[i] + STACK_SIZE, SIGCHLD, &t_data[i]);
        if (pids[i] == -1) {
            perror("clone failed");
            exit(1);
        }
    }
    for (int i = 0; i < M; i++) {
        waitpid(pids[i], NULL, 0);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    for (int i = 0; i < M; i++) {
        free(stacks[i]);
    }
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken: %f seconds\n", time_taken);

    return 0;
}

