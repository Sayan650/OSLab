#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int x, y;
int pipefd[2]; // Pipe for communication between parent and children

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
    if (sig == SIGUSR1) {
        // Child 1 has finished processing
        char buffer[100];
        int bytes_read = read(pipefd[0], buffer, sizeof(buffer));
        buffer[bytes_read] = '\0';
        printf("Parent received from child 1:\n%s\n", buffer);
    } else if (sig == SIGUSR2) {
        // Child 2 has finished processing
        char buffer[100];
        int bytes_read = read(pipefd[0], buffer, sizeof(buffer));
        buffer[bytes_read] = '\0';
        printf("Parent received from child 2:\n%s\n", buffer);
    }
}

// Function to find primes in the range and write to the file
void find_primes(int start, int end) {
    char buffer[100];
    int fd = open("primes.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("File open error");
        exit(1);
    }
    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            snprintf(buffer, sizeof(buffer), "[%d, %d]\n", getpid(), i);
            write(fd, buffer, strlen(buffer));
        }
    }
    close(fd);
}

// Function for child processes
void child_process(int start, int end, int signal_to_send) {
    find_primes(start, end);
    char msg[50];
    snprintf(msg, sizeof(msg), "Child %d finished.\n", getpid());
    write(pipefd[1], msg, strlen(msg));
    kill(getppid(), signal_to_send); // Notify the parent
    while (1) pause(); // Keep the process alive to handle signals
}

int main() {
    printf("Enter the range [x, y]: ");
    scanf("%d %d", &x, &y);

    pipe(pipefd);
    signal(SIGUSR1, parent_signal_handler);
    signal(SIGUSR2, parent_signal_handler);

    int mid = x + (y - x) / 2;
    pid_t child1, child2;

    if ((child1 = fork()) == 0) {
        // First child process
        close(pipefd[0]); // Close unused read end of the pipe
        child_process(x, mid, SIGUSR1);
    } else if ((child2 = fork()) == 0) {
        // Second child process
        close(pipefd[0]); // Close unused read end of the pipe
        child_process(mid + 1, y, SIGUSR2);
    } else {
        // Parent process
        close(pipefd[1]); // Close unused write end of the pipe
        while (1) pause(); // Wait for signals from children
    }

    return 0;
}