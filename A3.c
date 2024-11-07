#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>

bool is_prime(int num) {
    if (num <= 1) return false;
    if (num == 2 || num == 3) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return false;
    }
    return true;
}

void find_primes(int start, int end) {
    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            printf("%d is a prime number.\n", i);
        }
    }
}

int main() {
    int N, M;
    printf("Enter the value of N: ");
    scanf("%d", &N);
    printf("Enter the number of child processes M: ");
    scanf("%d", &M);

    clock_t start_time = clock();

    int range = N / M;
    for (int i = 0; i < M; i++) {
        int start = i * range + 1;
        int end = (i == M - 1) ? N : (i + 1) * range;

        int pid = fork();
        if (pid == 0) {
            // Child process
            find_primes(start, end);
            exit(0);
        }
    }

    // Parent process waits for all children to finish
    for (int i = 0; i < M; i++) {
        wait(NULL);
    }

    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", execution_time);

    return 0;
}