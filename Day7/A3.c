#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/types.h>

#define FIRST_YEAR_COUNT 200
#define FINAL_YEAR_COUNT 200

typedef struct {
    int first_year_waiting;
    int final_year_waiting;
} shared_data_t;

#define ROOM_ACCESS_SEM "/room_access"
#define BATCH_TOGGLE_SEM "/batch_toggle"
#define MUTEX_SEM "/mutex"

sem_t *room_access;
sem_t *batch_toggle;
sem_t *mutex;

int shm_id;
shared_data_t *shared_data;

void* first_year_student(void* arg) {
    int id = *(int*)arg;
    printf("First year student %d waiting...\n", id);

    sem_wait(mutex);
    shared_data->first_year_waiting++;
    sem_post(mutex);

    sem_wait(batch_toggle);
    sem_wait(room_access);

    printf("First year student %d enters the HOD's room.\n", id);
    sleep(1); 
    printf("First year student %d leaves the HOD's room.\n", id);
    sem_post(room_access);
    sem_wait(mutex);
    shared_data->first_year_waiting--;
    sem_post(mutex);
    sem_post(batch_toggle); 

    return NULL;
}

void* final_year_student(void* arg) {
    int id = *(int*)arg;
    printf("Final year student %d waiting...\n", id);
    sem_wait(mutex);
    shared_data->final_year_waiting++;
    sem_post(mutex);
    sem_wait(batch_toggle);
    sem_wait(room_access);

    printf("Final year student %d enters the HOD's room.\n", id);
    sleep(1); 

    printf("Final year student %d leaves the HOD's room.\n", id);
    sem_post(room_access);
    sem_wait(mutex);
    shared_data->final_year_waiting--;
    sem_post(mutex);
    sem_post(batch_toggle); 

    return NULL;
}

int main() {
    pthread_t threads[FIRST_YEAR_COUNT + FINAL_YEAR_COUNT];
    int student_ids[FIRST_YEAR_COUNT + FINAL_YEAR_COUNT];
    int i;

    shm_id = shmget(IPC_PRIVATE, sizeof(shared_data_t), IPC_CREAT | 0666);
    shared_data = (shared_data_t *)shmat(shm_id, NULL, 0);
    shared_data->first_year_waiting = 0;
    shared_data->final_year_waiting = 0;
    room_access = sem_open(ROOM_ACCESS_SEM, O_CREAT, 0644, 1); 
    batch_toggle = sem_open(BATCH_TOGGLE_SEM, O_CREAT, 0644, 1); 
    mutex = sem_open(MUTEX_SEM, O_CREAT, 0644, 1); 
    for (i = 0; i < FIRST_YEAR_COUNT; i++) {
        student_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, first_year_student, &student_ids[i]);
    }
    for (i = 0; i < FINAL_YEAR_COUNT; i++) {
        student_ids[FIRST_YEAR_COUNT + i] = i + 1;
        pthread_create(&threads[FIRST_YEAR_COUNT + i], NULL, final_year_student, &student_ids[FIRST_YEAR_COUNT + i]);
    }
    for (i = 0; i < FIRST_YEAR_COUNT + FINAL_YEAR_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    shmdt(shared_data);
    shmctl(shm_id, IPC_RMID, NULL);
    sem_unlink(ROOM_ACCESS_SEM);
    sem_unlink(BATCH_TOGGLE_SEM);
    sem_unlink(MUTEX_SEM);
    sem_close(room_access);
    sem_close(batch_toggle);
    sem_close(mutex);

    return 0;
}
