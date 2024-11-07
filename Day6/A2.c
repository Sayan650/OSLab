#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_DEPTH 10

pid_t child_pids[1 << MAX_DEPTH] = {0};
int num_children = 0;

void signal_handler(int signo) {
    if (signo == SIGINT) {
        printf("Parent process caught SIGINT. Terminating child processes.\n");
        for (int i = 0; i < num_children; i++) {
            if (child_pids[i] > 0) {
                kill(child_pids[i], SIGTERM);
            }
        }
        while (wait(NULL) > 0);
        exit(0);
    }
}

void create_process_tree(int depth, int max_depth, int index) {
    if (depth == max_depth) {
        while (1) {
            sleep(1);
        }
    }

    for (int i = 0; i < 2; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            // Child process
            setpgid(0, 0);  // Set new process group
            signal(SIGINT, SIG_IGN);  // Ignore SIGINT in child processes
            create_process_tree(depth + 1, max_depth, 2 * index + i + 1);
        } else {
            // Parent process
            child_pids[num_children++] = pid;
        }
    }

    while (1) {
        sleep(1);
    }
}

int main() {
    int n;
    printf("Enter the depth of the process tree (max %d): ", MAX_DEPTH);
    scanf("%d", &n);

    if (n < 1 || n > MAX_DEPTH) {
        fprintf(stderr, "Invalid depth. Please enter a value between 1 and %d.\n", MAX_DEPTH);
        return 1;
    }

    signal(SIGINT, signal_handler);

    printf("Creating process tree with depth %d. Press Ctrl+C to terminate child processes.\n", n);
    create_process_tree(0, n, 0);

    return 0;
}