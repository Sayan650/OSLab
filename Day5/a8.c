#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// Function to check if a number is prime
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num <= 3) return 1;
    if (num % 2 == 0 || num % 3 == 0) return 0;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return 0;
    }
    return 1;
}

// Function for child processes to find and write primes
void search_and_write_primes(int start, int end) {
    int fd = open("primes.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Failed to open file");
        exit(1);
    }
    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            char buf[256];
            int len = snprintf(buf, sizeof(buf), "[%d, %d]\n", getpid(), i);
            write(fd, buf, len);
        }
    }
    close(fd);
    exit(0);
}

int main() {
    int x, y;
    printf("Enter the range [x, y]: ");
    scanf("%d %d", &x, &y);

    // Create or truncate the file
    int fd = open("primes.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open file");
        exit(1);
    }
    close(fd);

    pid_t child1 = fork();
    if (child1 == 0) {
        search_and_write_primes(x, (x + y) / 2);
    }

    pid_t child2 = fork();
    if (child2 == 0) {
        search_and_write_primes((x + y) / 2 + 1, y);
    }

    int status;
    // Wait for both child processes to complete
    waitpid(child1, &status, 0);
    waitpid(child2, &status, 0);

    printf("Parent process: All child processes have completed.\n");
    return 0;
}