#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>

#define PHILOSOPHERS 5

int running = 1;

void handle_signal(int signal) {
    if (signal == SIGINT) {
        running = 0;
    }
}

void philosopher(int i, sem_t *left_fork, sem_t *right_fork) {
    while (running) {
        printf("Philosopher %d is thinking\n", i);
        sleep(rand() % 3 + 1); // thinking time

        printf("Philosopher %d is hungry\n", i);
        sem_wait(left_fork);
        if (sem_trywait(right_fork) == 0) {
            // Philosopher acquired both forks
            printf("Philosopher %d is eating\n", i);
            sleep(rand() % 3 + 1); // eating time
            sem_post(right_fork);
        } else {
            // Philosopher couldn't acquire both forks, release left fork
            sem_post(left_fork);
        }

        sem_post(left_fork); // release left fork
        sleep(rand() % 2 + 1); // small delay before thinking again
    }
    printf("Philosopher %d stopped\n", i);
}

int main() {
    sem_t *forks[PHILOSOPHERS];
    signal(SIGINT, handle_signal);

    for (int i = 0; i < PHILOSOPHERS; ++i) {
        char sem_name[10];
        sprintf(sem_name, "/fork%d", i);
        forks[i] = sem_open(sem_name, O_CREAT, 0644, 1);
    }

    for (int i = 0; i < PHILOSOPHERS; ++i) {
        if (fork() == 0) {
            philosopher(i, forks[i], forks[(i + 1) % PHILOSOPHERS]);
            exit(0);
        }
    }

    // Esperar a que se reciba la señal SIGINT para finalizar los procesos hijos
    while (running) {
        // Se podría realizar alguna tarea aquí si es necesario mientras se espera la señal
    }

    // Cerrar y liberar recursos
    for (int i = 0; i < PHILOSOPHERS; ++i) {
        sem_close(forks[i]);
        char sem_name[10];
        sprintf(sem_name, "/fork%d", i);
        sem_unlink(sem_name);
    }

    return 0;
}
