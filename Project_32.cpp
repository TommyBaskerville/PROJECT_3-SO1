#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/wait.h>

#define SHM_KEY 1234

struct rw_semaphore {
    sem_t mutex;
    sem_t write_lock;
    int *read_count;
};

void init_rwsem(struct rw_semaphore *rwsem) {
    sem_init(&rwsem->mutex, 1, 1); // Inicializado con 1 para evitar race conditions en la creación de la memoria compartida
    sem_init(&rwsem->write_lock, 1, 1); // Inicializado con 1 para evitar race conditions en la creación de la memoria compartida

    int shm_id = shmget(SHM_KEY, sizeof(int), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    rwsem->read_count = static_cast<int*>(shmat(shm_id, NULL, 0));
    if (rwsem->read_count == (int *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    *(rwsem->read_count) = 0;
}

void down_read(struct rw_semaphore *rwsem) {
    sem_wait(&rwsem->mutex);
    (*(rwsem->read_count))++;
    if (*(rwsem->read_count) == 1) {
        sem_wait(&rwsem->write_lock);
    }
    sem_post(&rwsem->mutex);
}

void up_read(struct rw_semaphore *rwsem) {
    sem_wait(&rwsem->mutex);
    (*(rwsem->read_count))--;
    if (*(rwsem->read_count) == 0) {
        sem_post(&rwsem->write_lock);
    }
    sem_post(&rwsem->mutex);
}

void down_write(struct rw_semaphore *rwsem) {
    sem_wait(&rwsem->write_lock);
}

void up_write(struct rw_semaphore *rwsem) {
    sem_post(&rwsem->write_lock);
}

void reader(struct rw_semaphore *rwsem) {
    while (1) {
        down_read(rwsem);
        // Perform reading
        printf("Reader reading... Read count: %d\n", *(rwsem->read_count));
        up_read(rwsem);
        sleep(1);
    }
}

void writer(struct rw_semaphore *rwsem) {
    while (1) {
        down_write(rwsem);
        // Perform writing
        printf("Writer writing...\n");
        up_write(rwsem);
        sleep(1);
    }
}

int main() {
    struct rw_semaphore rwsem;
    init_rwsem(&rwsem);

    pid_t pid;

    // Create reader processes
    for (int i = 0; i < 3; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            reader(&rwsem);
            exit(EXIT_SUCCESS);
        }
    }

    // Create writer processes
    for (int i = 0; i < 2; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            writer(&rwsem);
            exit(EXIT_SUCCESS);
        }
    }

    // Wait for child processes to finish

    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }

    // Detach and remove shared memory
    shmdt(rwsem.read_count);
    shmctl(shmget(SHM_KEY, sizeof(int), 0), IPC_RMID, NULL);

    return 0;
}
