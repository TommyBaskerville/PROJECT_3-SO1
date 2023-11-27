#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>

#define READCOUNT_KEY 1234
#define SEM_KEY 5678

void reader(int semid, int* readcount) {
    while(1) {
        // Wait on semaphore
        struct sembuf op = {0, -1, 0};
        semop(semid, &op, 1);

        (*readcount)++;

        if (*readcount == 1) {
            // First reader locks the resource for writers
            op.sem_num = 1;
            semop(semid, &op, 1);
        }

        // Release semaphore
        op.sem_num = 0;
        op.sem_op = 1;
        semop(semid, &op, 1);

        // Read
        printf("Reading\n");

        // Wait on semaphore
        op.sem_num = 0;
        op.sem_op = -1;
        semop(semid, &op, 1);

        (*readcount)--;

        if (*readcount == 0) {
            // Last reader unlocks the resource for writers
            op.sem_num = 1;
            op.sem_op = 1;
            semop(semid, &op, 1);
        }

        // Release semaphore
        op.sem_num = 0;
        op.sem_op = 1;
        semop(semid, &op, 1);
    }
}

void writer(int semid) {
    while(1) {
        // Wait on semaphore
        struct sembuf op = {1, -1, 0};
        semop(semid, &op, 1);

        // Write
        printf("Writing\n");

        // Release semaphore
        op.sem_op = 1;
        semop(semid, &op, 1);
    }
}

int main() {
    // Create shared memory for readcount
    int shmid = shmget(READCOUNT_KEY, sizeof(int), 0666 | IPC_CREAT);
    int* readcount = (int*) shmat(shmid, 0, 0);
    *readcount = 0;

    // Create semaphores
    int semid = semget(SEM_KEY, 2, 0666 | IPC_CREAT);
    semctl(semid, 0, SETVAL, 1); // x
    semctl(semid, 1, SETVAL, 1); // wsem

    // Create reader and writer processes
    if (fork() == 0) {
        reader(semid, readcount);
    } else {
        writer(semid);
    }

    return 0;
}