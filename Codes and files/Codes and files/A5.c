#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* even_thread_function(void* arg) {
    int thread_num = *(int*)arg;
    int* return_value = malloc(sizeof(int));  
    *return_value = thread_num * 2; 
    
    printf("Even-numbered thread %d returning value: %d\n", thread_num, *return_value);
    pthread_exit(return_value); 
}

void* odd_thread_function(void* arg) {
    int thread_num = *(int*)arg;
    printf("Odd-numbered thread %d running in detached mode.\n", thread_num);
    pthread_exit(NULL); 
}

int main() {
    int n;
    printf("Enter the number of child threads: ");
    scanf("%d", &n);

    pthread_t threads[n];
    int thread_numbers[n];
    pthread_attr_t attr;

    for (int i = 0; i < n; i++) {
        thread_numbers[i] = i + 1; 

        if (thread_numbers[i] % 2 == 0) {

            pthread_attr_init(&attr); 
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);  

            if (pthread_create(&threads[i], &attr, even_thread_function, &thread_numbers[i]) != 0) {
                perror("Failed to create even thread");
                exit(1);
            }

            pthread_attr_destroy(&attr);  
        } else {
            pthread_attr_init(&attr);  
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 

            if (pthread_create(&threads[i], &attr, odd_thread_function, &thread_numbers[i]) != 0) {
                perror("Failed to create odd thread");
                exit(1);
            }

            pthread_attr_destroy(&attr);
        }
    }
    for (int i = 0; i < n; i++) {
        if (thread_numbers[i] % 2 == 0) {
            int* return_value;
            if (pthread_join(threads[i], (void**)&return_value) == 0) {
                printf("Parent received value %d from even-numbered thread %d\n", *return_value, thread_numbers[i]);
                free(return_value); 
            } else {
                perror("Failed to join even thread");
            }
        }
    }

    printf("All threads finished execution.\n");
    return 0;
}

