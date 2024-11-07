#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int counter = 0;
void handle_sigint(int sig) {
    printf("Received SIGINT, exiting now...\n");
    _exit(0);
}
void handle_sigint_case3(int signum) {
    counter++;
    printf("\nReceived signal: %d\n", signum);
    if (counter < 3) {
        printf("Custom Message: Ignoring termination %d time(s), continuing execution.\n", counter);
    } else {
        printf("Custom Message: Terminating the process after 3 signals.\n");
        exit(0);
    }
}


int main() {
    signal(SIGINT, handle_sigint_case3);
    printf("Waiting for a signal (Ctrl+C)...\n");
    pause();  // Wait indefinitely for a signal
    return 0;
}