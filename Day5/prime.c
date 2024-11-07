#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int x, y;
int found = 0;  // Flag to indicate if a prime has been found
pid_t child1, child2;

// Function to check if a number is prime
int is_prime(int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

// Signal handler for the parent process
void parent_signal_handler(int sig) {
    if (sig == SIGUSR1 || sig == SIGUSR2) {
        found = 1;  // A prime number has been found
        printf("Parent: Prime number found by child %d, terminating both children.\n", sig == SIGUSR1 ? child1 : child2);
        
        // Send SIGINT to both children to terminate them
        kill(child1, SIGINT);
        kill(child2, SIGINT);
    }
}

// Signal handler for the child process
void child_signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("Child %d: Terminating.\n", getpid());
        exit(0);  // Terminate the child process
    }
}

// Function for child processes to find a prime number
void find_prime(int start, int end, int signal_to_send) {
    for (int i = start; i <= end; i++) {
        if (found) break;  // Stop if a prime has already been found by another process
        if (is_prime(i)) {
            printf("Child %d: Found prime number %d.\n", getpid(), i);
            kill(getppid(), signal_to_send);  // Notify the parent
            break;
        }
    }
    while (1) pause();  // Wait for the termination signal
}

int main() {
    printf("Enter the range [x, y]: ");
    scanf("%d %d", &x, &y);

    signal(SIGUSR1, parent_signal_handler);
    signal(SIGUSR2, parent_signal_handler);

    int mid = x + (y - x) / 2;

    if ((child1 = fork()) == 0) {
        // First child process
        signal(SIGINT, child_signal_handler);
        find_prime(x, mid, SIGUSR1);
    } else if ((child2 = fork()) == 0) {
        // Second child process
        signal(SIGINT, child_signal_handler);
        find_prime(mid + 1, y, SIGUSR2);
    } else {
        // Parent process
        while (!found) pause();  // Wait for one of the children to find a prime number
    }

    return 0;
}