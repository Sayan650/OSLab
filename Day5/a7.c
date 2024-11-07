#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

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

pid_t child1, child2;

// Child process function
void search_prime(int start, int end) {
    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            printf("Child %d found prime: %d\n", getpid(), i);
            exit(0);  // Exit after finding the prime
        }
    }
    exit(0);  // Exit if no prime is found
}

int main() {
    int x, y;

    // Input the range [x, y]
    printf("Enter the range [x, y]: ");
    scanf("%d %d", &x, &y);

    child1 = fork();
    if (child1 == 0) {
        // First child searches the first half of the range
        search_prime(x, (x + y) / 2);
    }

    child2 = fork();
    if (child2 == 0) {
        // Second child searches the second half of the range
        search_prime((x + y) / 2 + 1, y);
    }

    int status;
    pid_t terminated_pid = waitpid(-1, &status, 0);

    // One child found a prime; terminate the other
    if (terminated_pid == child1) {
        kill(child2, SIGINT);
    } else {
        kill(child1, SIGINT);
    }

    printf("Parent process: Prime found. Terminating...\n");
    return 0;
}