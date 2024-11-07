#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

#define LOG_FILE "log.txt"

pid_t root_pid; // Store the PID of the root (parent) process

void log_process_info(const char *process_type) {
    int fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Failed to open log file");
        exit(1);
    }

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0'; // Remove the newline character

    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), "[%s] %s: PID=%d, PPID=%d\n", timestamp, process_type, getpid(), getppid());

    write(fd, log_entry, strlen(log_entry));
    close(fd);

    printf("%s", log_entry); // Print to terminal as well
}

void create_process_tree(int depth) {
    if (depth == 0) {
        log_process_info("Leaf");
        sleep(60); // Sleep for 1 minute
        while (1) {
            // Infinite loop to keep the process alive
        }
    } else {
        pid_t left_child, right_child;

        log_process_info("Internal Node");
        sleep(60); // Sleep for 1 minute

        // Create left child
        left_child = fork();
        if (left_child == 0) {
            // In left child process
            setpgid(0, root_pid); // Set the process group ID to the parent's PID
            create_process_tree(depth - 1);
        } else if (left_child > 0) {
            // In parent process
            // Create right child
            right_child = fork();
            if (right_child == 0) {
                // In right child process
                setpgid(0, root_pid); // Set the process group ID to the parent's PID
                create_process_tree(depth - 1);
            } else if (right_child > 0) {
                // In parent process, wait for both children
                wait(NULL); // Wait for the left child to finish (if ever)
                wait(NULL); // Wait for the right child to finish (if ever)
                while (1) {
                    // Infinite loop to keep the parent process alive
                }
            } else {
                perror("Failed to create right child process");
                exit(1);
            }
        } else {
            perror("Failed to create left child process");
            exit(1);
        }
    }
}

void handle_sigint(int sig) {
    if (getpid() != root_pid) {
        // Terminate all child processes except the parent
        exit(0);
    } else {
        printf("\nRoot process (PID=%d) received SIGINT. Child processes will be terminated.\n", getpid());
    }
}

int main() {
    int n;

    printf("Enter the depth of the binary tree (n): ");
    scanf("%d", &n);

    // Store the PID of the root process
    root_pid = getpid();

    // Set up the signal handler for SIGINT
    signal(SIGINT, handle_sigint);

    // Clear the log file at the start
    int fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open log file");
        exit(1);
    }
    close(fd);

    // Create the process tree
    create_process_tree(n);

    return 0;
}