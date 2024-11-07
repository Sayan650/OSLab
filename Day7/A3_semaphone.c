#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>   
#include <sys/mman.h> 
#include <unistd.h>  
#include <sys/wait.h> 

#define SHARED_MEMORY_NAME "/student_count_shared"
#define MUTEX_SEM_NAME "/hod_room_mutex"
#define FIRST_SEM_NAME "/first_turn"
#define FINAL_SEM_NAME "/final_turn"

struct shared_memory {
    int first_year_count;
    int final_year_count;
    int first_year_busy; 
    int final_year_busy; 
};

int main() {
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(struct shared_memory));
    struct shared_memory *shared_data = (struct shared_memory *)
        mmap(0, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    
    shared_data->first_year_count = 0;
    shared_data->final_year_count = 0;
    shared_data->first_year_busy = 0;
    shared_data->final_year_busy = 0;

    sem_t *mutex = sem_open(MUTEX_SEM_NAME, O_CREAT, 0666, 1); 
    sem_t *first_turn = sem_open(FIRST_SEM_NAME, O_CREAT, 0666, 1); 
    sem_t *final_turn = sem_open(FINAL_SEM_NAME, O_CREAT, 0666, 0); 

    if (mutex == SEM_FAILED || first_turn == SEM_FAILED || final_turn == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    void first_year_student() {
        sem_wait(mutex);
        shared_data->first_year_count++;
        sem_post(mutex);

        if (shared_data->final_year_busy) {
            sem_wait(mutex);
            printf("1st year student is inside the HOD's room while final year is busy.\n");
            shared_data->first_year_count--;
            sem_post(mutex);
            sleep(1);
        } else {
            sem_wait(first_turn); 
            sem_wait(mutex); 

            printf("1st year student is inside the HOD's room.\n");
            sleep(1); 
            shared_data->first_year_count--;
            sem_post(mutex);
            sem_post(final_turn); 
        }
    }
    void final_year_student() {
        sem_wait(mutex);
        shared_data->final_year_count++;
        sem_post(mutex);

        if (shared_data->first_year_busy) {
            sem_wait(mutex);
            printf("Final year student is inside the HOD's room while 1st year is busy.\n");
            shared_data->final_year_count--;
            sem_post(mutex);
            sleep(1);
        } else {
            sem_wait(final_turn); 
            sem_wait(mutex); 

            printf("Final year student is inside the HOD's room.\n");
            sleep(1); 
            shared_data->final_year_count--;
            sem_post(mutex);
            sem_post(first_turn); 
        }
    }
    void first_year_busy_task() {
        sem_wait(mutex);
        shared_data->first_year_busy = 1;
        sem_post(mutex);
        printf("1st year students are busy with another task.\n");
        sleep(3); 
        sem_wait(mutex);
        shared_data->first_year_busy = 0;
        sem_post(mutex);
        printf("1st year students are back from the busy task.\n");
    }
    void final_year_busy_task() {
        sem_wait(mutex);
        shared_data->final_year_busy = 1;
        sem_post(mutex);
        printf("Final year students are busy with another task.\n");
        sleep(3); 
        sem_wait(mutex);
        shared_data->final_year_busy = 0;
        sem_post(mutex);
        printf("Final year students are back from the busy task.\n");
    }
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        for (int i = 0; i < 3; i++) {
            first_year_student();
        }
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
        for (int i = 0; i < 3; i++) {
            final_year_student();
        }
        exit(0);
    }
    if (fork() == 0) {
        first_year_busy_task();
        exit(0);
    }

    if (fork() == 0) {
        final_year_busy_task();
        exit(0);
    }
    while (wait(NULL) > 0);
    sem_close(mutex);
    sem_close(first_turn);
    sem_close(final_turn);
    sem_unlink(MUTEX_SEM_NAME);
    sem_unlink(FIRST_SEM_NAME);
    sem_unlink(FINAL_SEM_NAME);

    munmap(shared_data, sizeof(struct shared_memory));
    shm_unlink(SHARED_MEMORY_NAME);

    printf("HOD's room access simulation completed.\n");
    return 0;
}
