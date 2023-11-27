#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define FILE_PATH "sharedfile.txt"

struct rw_semaphore {
    sem_t mutex;
    sem_t write_lock;
};

void init_rwsem(struct rw_semaphore *rwsem) {
    sem_init(&rwsem->mutex, 1, 1); // Mutex para control de acceso a la sección crítica
    sem_init(&rwsem->write_lock, 1, 1); // Semáforo para bloquear escritura
}

void down_read(struct rw_semaphore *rwsem) {
    sem_wait(&rwsem->mutex);
}

void up_read(struct rw_semaphore *rwsem) {
    sem_post(&rwsem->mutex);
}

void down_write(struct rw_semaphore *rwsem) {
    sem_wait(&rwsem->write_lock);
}

void up_write(struct rw_semaphore *rwsem) {
    sem_post(&rwsem->write_lock);
}

void reader(struct rw_semaphore *rwsem, int id) {
    FILE *file;
    char buffer[256];

    while (1) {
        down_read(rwsem);

        file = fopen(FILE_PATH, "r");
        if (file == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        // Leer del archivo
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("Reader %d read: %s", id, buffer);
        }

        fclose(file);

        up_read(rwsem);
        sleep(1);
    }
}

void writer(struct rw_semaphore *rwsem, int id) {
    FILE *file;

    while (1) {
        down_write(rwsem);

        file = fopen(FILE_PATH, "a");
        if (file == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        // Escribir en el archivo
        fprintf(file, "Writer %d wrote.\n", id);

        fclose(file);

        up_write(rwsem);
        sleep(1);
    }
}

int main() {
    struct rw_semaphore rwsem;
    init_rwsem(&rwsem);

    pid_t pid;
    int reader_count = 3;
    int writer_count = 2;

    // Crear procesos lectores
    for (int i = 0; i < reader_count; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Proceso hijo
            reader(&rwsem, i + 1);
            exit(EXIT_SUCCESS);
        }
    }

    // Crear procesos escritores
    for (int i = 0; i < writer_count; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Proceso hijo
            writer(&rwsem, i + 1);
            exit(EXIT_SUCCESS);
        }
    }

    // Esperar a que los procesos hijos terminen
    for (int i = 0; i < reader_count + writer_count; i++) {
        wait(NULL);
    }

    return 0;
}
