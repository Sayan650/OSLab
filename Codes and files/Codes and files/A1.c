#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sched.h>
#include <string.h>

#define STACK_SIZE (1024 * 1024) 
int clone_func(void* arg) {
    printf("World (from clone)\n");
    return 0;
}
void use_fork() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("World (from fork)\n");
        exit(0);
    } else {
        printf("Hello (from parent using fork)\n");
        wait(NULL);
    }
}
void use_exec() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Child process replacing itself with exec()...\n");
        execlp("/bin/echo", "echo", "World (from exec)", NULL);
        perror("exec failed"); 
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);  
        printf("Hello (from parent using exec)\n");
    }
}
void use_clone() {
    char *stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    if (clone(clone_func, stack + STACK_SIZE, SIGCHLD, NULL) == -1) {
        perror("clone failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    printf("Hello (from parent using clone)\n");
    free(stack);
}

int main() {
    printf(" Using fork()\n");
    use_fork();

    printf("\n\nUsing exec()\n");
    use_exec();

    printf("\n\nUsing clone()\n");
    use_clone();

    return 0;
}

