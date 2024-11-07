#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define N 1000000000  // Size of the array

// Function for each child to search a portion of the array
void search_in_array(int *array, int start, int end, int target) {
    for (int i = start; i <= end; i++) {
        if (array[i] == target) {
            printf("Child %d: Found element %d at index %d\n", getpid(), target, i);
            exit(1);  // Exit with status code 1 if found
        }
    }
    exit(0);  // Exit with status code 0 if not found
}

int main() {
    //printf("Enter the element to search for: ");
    //int N;
    //scanf("%d", &N);
    int *array = (int *)malloc(N * sizeof(int));
    int target, n;

    // Seed for random number generation
    srand(time(NULL));

    // Fill the array with random numbers in the range [0, N)
    for (int i = 0; i < N; i++) {
        array[i] = rand() % N;
    }

    // Input the element to search and the number of child processes
    printf("Enter the element to search for: ");
    scanf("%d", &target);
    printf("Enter the number of child processes to create: ");
    scanf("%d", &n);

    int segment_size = N / n;
    pid_t pid;
    int status;

    for (int i = 0; i < n; i++) {
        int start = i * segment_size;
        int end = (i == n - 1) ? N - 1 : (start + segment_size - 1);

        if ((pid = fork()) == 0) {
            // Child process
            search_in_array(array, start, end, target);
        }
    }

    // Parent process waits for any child to exit
    for (int i = 0; i < n; i++) {
        pid = wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
            printf("Parent: Child %d found the element. Terminating search.\n", pid);
            // Terminate all remaining child processes
            for (int j = 0; j < n; j++) {
                if (j != i) {
                    kill(pid, SIGKILL);
                }
            }
            break;
        }
    }

    // Cleanup
    free(array);

    return 0;
}