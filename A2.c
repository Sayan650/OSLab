#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

void log_process_info(const char *process_type, pid_t pid, pid_t ppid) {
    FILE *log_file = fopen("log2.txt", "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        exit(1);
    }   
    time_t current_time;
    time(&current_time);
    char *timestamp = ctime(&current_time);
    timestamp[strlen(timestamp) - 1] = '\0'; 

    fprintf(log_file, "%s - %s: PID = %d, PPID = %d\n", timestamp, process_type, pid, ppid);
    fclose(log_file);
}
int main() {
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    pid_t pid;
    pid_t ppid = getpid(); 

    for (int i = 1; i <= n; i++) {
        pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Fork Failed\n");
            exit(1);
        } else if (pid == 0) {
            printf("Child %d: PID = %d, PPID = %d\n", i, getpid(), getppid());
            log_process_info("Child", getpid(), getppid());
            sleep(60); 
        } else {
            wait(NULL); 
            break;
        }
    }
    if (pid == 0 && n > 0) {
        printf("Last Child %d: PID = %d, PPID = %d\n", n, getpid(), getppid());
        log_process_info("Last Child", getpid(), getppid());
    }
  return 0;
}