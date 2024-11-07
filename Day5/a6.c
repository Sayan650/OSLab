#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <math.h>

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

// Function for each child to find primes in its range
void find_primes_in_range(int start, int end, int file_desc) {
    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            dprintf(file_desc, "%d\n", i);  // Write prime number to file
        }
    }
    exit(0);  // Exit when done
}

int main() {
    int x, y, n;

    // Input the range [x, y] and the number of child processes
    printf("Enter the range [x, y] (x > 1000, y > 1000): ");
    scanf("%d %d", &x, &y);
    printf("Enter the number of child processes to create: ");
    scanf("%d", &n);

    // Open or create a common file for writing prime numbers
    int file_desc = open("primes.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_desc < 0) {
        perror("Failed to open file");
        exit(1);
    }

    int range_per_child = (y - x + 1) / n;
    pid_t pid;

    for (int i = 0; i < n; i++) {
        int start = x + i * range_per_child;
        int end = (i == n - 1) ? y : start + range_per_child - 1;

        if ((pid = fork()) == 0) {
            // Child process
            find_primes_in_range(start, end, file_desc);
        }
    }

    // Parent process waits for all child processes to finish
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    // Close the file descriptor
    close(file_desc);

    printf("All child processes completed. Prime numbers are stored in 'primes.txt'.\n");

    return 0;
}