#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/wait.h>

#define FILENAME "primes.txt"

volatile sig_atomic_t child1_done = 0;
volatile sig_atomic_t child2_done = 0;

void signal_handler(int signo) {
    if (signo == SIGUSR1) {
        child1_done = 1;
    } else if (signo == SIGUSR2) {
        child2_done = 1;
    }
}

bool is_prime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

void find_primes(int start, int end, int child_num) {
    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Lock the file for exclusive access
    flock(fileno(file), LOCK_EX);

    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            fprintf(file, "[%d, %d]\n", getpid(), i);
            // Flush after each write to ensure data is written immediately
            fflush(file);
        }
    }

    // Unlock the file
    flock(fileno(file), LOCK_UN);

    fclose(file);

    if (child_num == 1) {
        kill(getppid(), SIGUSR1);
    } else {
        kill(getppid(), SIGUSR2);
    }

    // Child process exits after sending signal
    exit(0);
}

int main() {
    int x, y;
    printf("Enter the range [x, y]: ");
    scanf("%d %d", &x, &y);

    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    // Clear the file content before starting
    FILE *file = fopen(FILENAME, "w");
    if (file != NULL) {
        fclose(file);
    }

    pid_t pid1, pid2;

    pid1 = fork();

    if (pid1 == 0) {
        // Child 1
        find_primes(x, (x + y) / 2, 1);
    } else {
        pid2 = fork();

        if (pid2 == 0) {
            // Child 2
            find_primes((x + y) / 2 + 1, y, 2);
        } else {
            // Parent
            while (!child1_done || !child2_done) {
                pause();
            }

            printf("Both children have completed their tasks.\n");
            printf("Prime numbers have been written to %s\n", FILENAME);

            // Wait for both children to exit properly
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);

            printf("Parent process exiting.\n");
        }
    }

    return 0;
}
