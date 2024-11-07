#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

#define LOG_FILE "log.txt"

void *log_pid_ppid(void *arg) {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    
    // Log to terminal
    printf("PID: %d, PPID: %d\n", pid, ppid);
    
    // Log to file
    FILE *file = fopen(LOG_FILE, "a");
    if (file != NULL) {
        time_t now = time(NULL);
        fprintf(file, "Time: %sPID: %d, PPID: %d\n", ctime(&now), pid, ppid);
        fclose(file);
    } else {
        perror("File open error");
    }
    
    return NULL;
}

int main() {
    int n;
    printf("Enter number of child threads: ");
    scanf("%d", &n);
    
    pthread_t threads[n];
    
    for (int i = 0; i < n; i++) {
        if (pthread_create(&threads[i], NULL, log_pid_ppid, NULL)) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }
    
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}
