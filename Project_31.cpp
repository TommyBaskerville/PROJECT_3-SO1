#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define READCOUNT_KEY 1234
#define SEM_KEY 5678

void reader(int semid, int* readcount) {
    struct sembuf op;
    while(1) {
        // Error checking for semop
        op.sem_num = 0; op.sem_op = -1; op.sem_flg = 0;
        if (semop(semid, &op, 1) == -1) {
            perror("semop");
            exit(1);
        }

        (*readcount)++;

        if (*readcount == 1) {
            op.sem_num = 1;
            if (semop(semid, &op, 1) == -1) {
                perror("semop");
                exit(1);
            }
        }

        op.sem_num = 0; op.sem_op = 1; op.sem_flg = 0;
        if (semop(semid, &op, 1) == -1) {
            perror("semop");
            exit(1);
        }

        printf("Reading\n");

        op.sem_num = 0; op.sem_op = -1; op.sem_flg = 0;
        if (semop(semid, &op, 1) == -1) {
            perror("semop");
            exit(1);
        }

        (*readcount)--;

        if (*readcount == 0) {
            op.sem_num = 1; op.sem_op = 1; op.sem_flg = 0;
            if (semop(semid, &op, 1) == -1) {
                perror("semop");
                exit(1);
            }
        }

        op.sem_num = 0; op.sem_op = 1; op.sem_flg = 0;
        if (semop(semid, &op, 1) == -1) {
            perror("semop");
            exit(1);
        }
    }
}

void writer(int semid) {
    struct sembuf op;
    while(1) {
        op.sem_num = 1; op.sem_op = -1; op.sem_flg = 0;
        if (semop(semid, &op, 1) == -1) {
            perror("semop");
            exit(1);
        }

        printf("Writing\n");

        op.sem_num = 1; op.sem_op = 1; op.sem_flg = 0;
        if (semop(semid, &op, 1) == -1) {
            perror("semop");
            exit(1);
        }
    }
}

int main() {
    int shmid = shmget(READCOUNT_KEY, sizeof(int), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    int* readcount = (int*) shmat(shmid, 0, 0);
    if (readcount == (void *) -1) {
        perror("shmat");
        exit(1);
    }
    *readcount = 0;

    int semid = semget(SEM_KEY, 2, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    if (semctl(semid, 0, SETVAL, 1) == -1 || semctl(semid, 1, SETVAL, 1) == -1) {
        perror("semctl");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        reader(semid, readcount);
    } else {
        writer(semid);
    }

    return 0;
}