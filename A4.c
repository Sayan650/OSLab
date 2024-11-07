#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>

void log_process_info(int fd) {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    time_t now;
    time(&now);

    char buffer[256];
    int n = snprintf(buffer, sizeof(buffer), "Timestamp: %sPID: %d, PPID: %d\n", ctime(&now), pid, ppid);

    // Write to the log file
    write(fd, buffer, n);

    // Print to the terminal
    printf("PID: %d, PPID: %d\n", pid, ppid);
}

void create_binary_tree(int depth, int fd) {
    if (depth == 0) {
        return;
    }

    pid_t left_pid = fork();
    if (left_pid == 0) {
        // Left child process
        log_process_info(fd);
        sleep(60);  // Sleep for 1 minute
        create_binary_tree(depth - 1, fd);
        exit(0);
    } else if (left_pid > 0) {
        pid_t right_pid = fork();
        if (right_pid == 0) {
            // Right child process
            log_process_info(fd);
            sleep(60);  // Sleep for 1 minute
            create_binary_tree(depth - 1, fd);
            exit(0);
        } else if (right_pid > 0) {
            // Parent process waits for both children to finish
            wait(NULL);
            wait(NULL);
        } else {
            // Fork failed for the right child
            perror("Fork failed");
            close(fd);
            exit(1);
        }
    } else {
        // Fork failed for the left child
        perror("Fork failed");
        close(fd);
        exit(1);
    }
}

int main() {
    int n;

    printf("Enter the depth of the binary tree: ");
    scanf("%d", &n);

    // Open the log file
    int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open log file");
        exit(1);
    }

    // Log the parent process info
    log_process_info(fd);

    // Create the binary tree of processes
    create_binary_tree(n, fd);

    // Close the file descriptor in the parent process
    close(fd);

    return 0;
}