#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int is_prime(int num) {
    if (num <= 1) return 0;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

typedef struct {
    int start, end;
} Range;

void *find_primes(void *arg) {
    Range *range = (Range *)arg;
    for (int i = range->start; i <= range->end; i++) {
        if (is_prime(i)) {
            printf("%d ", i);
        }
    }
    return NULL;
}

int main() {
    int N, M;
    printf("Enter the range [1, N]: ");
    scanf("%d", &N);
    printf("Enter number of threads: ");
    scanf("%d", &M);

    pthread_t threads[M];
    Range ranges[M];
    int segment = N / M;

    clock_t start = clock();
    
    for (int i = 0; i < M; i++) {
        ranges[i].start = i * segment + 1;
        ranges[i].end = (i == M - 1) ? N : (i + 1) * segment;
        pthread_create(&threads[i], NULL, find_primes, &ranges[i]);
    }

    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_t end = clock();
    printf("\nExecution time: %lf seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
    
    return 0;
}
