#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

void log_process_info(const char *process_type, pid_t pid, pid_t ppid) {
    FILE *log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        exit(1);
    }
    
    time_t current_time;
    time(&current_time);
    char *timestamp = ctime(&current_time);
    timestamp[strlen(timestamp) - 1] = '\0'; // Remove the newline character

    fprintf(log_file, "%s - %s: PID = %d, PPID = %d\n", timestamp, process_type, pid, ppid);
    fclose(log_file);
}

int main() {
    int n;
    printf("Enter the number of child processes: ");
    scanf("%d", &n);

    pid_t parent_pid = getpid();
    printf("Parent: PID = %d, PPID = %d\n", parent_pid, getppid());
    log_process_info("Parent", parent_pid, getppid());

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            // Fork failed
            fprintf(stderr, "Fork Failed\n");
            exit(1);
        } else if (pid == 0) {
            // Child process
            printf("Child %d: PID = %d, PPID = %d\n", i+1, getpid(), getppid());
            log_process_info("Child", getpid(), getppid());
            sleep(60); // Sleep for 1 minute
            exit(0);
        } else {
            // Parent process continues to create the next child
            continue;
        }
    }

    // Parent waits for all child processes to complete
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    return 0;
}