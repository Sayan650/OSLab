#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

#define ITERATIONS 10000
#define DEPOSIT_AMOUNT 10
#define WITHDRAW_AMOUNT 10

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
void semaphore_wait(int sem_id) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = -1; 
    op.sem_flg = 0;
    semop(sem_id, &op, 1);
}
void semaphore_signal(int sem_id) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1; 
    op.sem_flg = 0;
    semop(sem_id, &op, 1);
}
int main() {
    int shm_id, sem_id;
    int *balance;
    union semun sem_union;
    shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }
    balance = (int *) shmat(shm_id, NULL, 0);
    if (balance == (int *) -1) {
        perror("shmat");
        exit(1);
    }
    *balance = 1000;
    sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (sem_id < 0) {
        perror("semget");
        exit(1);
    }
    sem_union.val = 1; 
    semctl(sem_id, 0, SETVAL, sem_union);
    pid_t pid1 = fork();
    if (pid1 == 0) {
        for (int i = 0; i < ITERATIONS; i++) {
            semaphore_wait(sem_id); 
            *balance += DEPOSIT_AMOUNT;
            semaphore_signal(sem_id); 
        }
        exit(0);
    }
    pid_t pid2 = fork();
    if (pid2 == 0) {
        for (int i = 0; i < ITERATIONS; i++) {
            semaphore_wait(sem_id); 
            *balance -= WITHDRAW_AMOUNT;
            semaphore_signal(sem_id); 
        }
        exit(0);
    }
    
    wait(NULL);
    wait(NULL);
    printf("Final Balance (with synchronization): %d\n", *balance);
    shmdt(balance);
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID, sem_union); 

    return 0;
}
