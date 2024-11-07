#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Global counter to keep track of the number of SIGINT signals received
int counter = 0;

// Case 1: Terminate the process with a custom message
void handle_sigint_case1(int signum) {
    printf("\nReceived signal: %d\n", signum);
    printf("Custom Message: Terminating the process gracefully.\n");
    exit(0);
}

// Case 2: Display custom message and continue running, ignoring default action
void handle_sigint_case2(int signum) {
    printf("\nReceived signal: %d\n", signum);
    printf("Custom Message: Ignoring termination, continuing execution.\n");
    // No exit() call, so the process continues running
}

// Case 3: Ignore termination three times, then terminate with a custom message
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
    printf("Running infinite loop. Press Ctrl+C to send SIGINT signal.\n");

    // Case 1: Process terminates with custom message
    // signal(SIGINT, handle_sigint_case1);

    // Case 2: Process continues running with custom message
    // signal(SIGINT, handle_sigint_case2);

    // Case 3: Process ignores first two signals, terminates on third
    signal(SIGINT, handle_sigint_case3);

    while (1) {
        sleep(1);
    }

    return 0;
}