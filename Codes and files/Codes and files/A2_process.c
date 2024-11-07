#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>


#define NUM_PROCESSES 100000

int main() {
    struct timespec start, end;
    pid_t pid;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_PROCESSES; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork() failed");
            exit(1);
        }
        if (pid == 0) {
            _exit(0);
        } else {
            wait(NULL);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken to create %d processes: %f seconds\n", NUM_PROCESSES, time_taken);

    return 0;
}

