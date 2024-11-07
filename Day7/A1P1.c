#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>

#define ITERATIONS 10000
#define DEPOSIT_AMOUNT 10
#define WITHDRAW_AMOUNT 10

int main() {
    int shm_id;
    int *balance;
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
    pid_t pid1 = fork();
    if (pid1 == 0) {
        for (int i = 0; i < ITERATIONS; i++) {
            *balance += DEPOSIT_AMOUNT;
        }
        exit(0);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) {
        for (int i = 0; i < ITERATIONS; i++) {
            *balance -= WITHDRAW_AMOUNT;
        }
        exit(0);
    }
    wait(NULL);
    wait(NULL);
    printf("Final Balance (without synchronization): %d\n", *balance);
    shmdt(balance);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
