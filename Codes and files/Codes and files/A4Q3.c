#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int depth;
} ThreadData;

void *create_tree(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    if (data->depth == 0) return NULL;

    pthread_t left, right;
    ThreadData left_data = {data->depth - 1};
    ThreadData right_data = {data->depth - 1};

    printf("Thread (PID: %d) creating left child at depth %d\n", getpid(), data->depth - 1);
    pthread_create(&left, NULL, create_tree, &left_data);
    
    printf("Thread (PID: %d) creating right child at depth %d\n", getpid(), data->depth - 1);
    pthread_create(&right, NULL, create_tree, &right_data);

    pthread_join(left, NULL);
    pthread_join(right, NULL);
    
    return NULL;
}

int main() {
    int n;
    printf("Enter depth of binary tree: ");
    scanf("%d", &n);

    pthread_t root_thread;
    ThreadData root_data = {n};

    pthread_create(&root_thread, NULL, create_tree, &root_data);
    pthread_join(root_thread, NULL);

    return 0;
}
