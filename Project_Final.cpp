#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define PHILOSOPHERS 5

void philosopher(int i, sem_t *left_fork, sem_t *right_fork)
{
    while (1)
    {
        printf("Philosopher %d is thinking\n", i);
        sleep(rand() % 3 + 1); // thinking time

        printf("Philosopher %d is hungry\n", i);
        sem_wait(left_fork);
        if (sem_trywait(right_fork) == 0)
        {
            // Philosopher acquired both forks
            printf("Philosopher %d is eating\n", i);
            sleep(rand() % 3 + 1); // eating time
            sem_post(right_fork);
        }
        else
        {
            // Philosopher couldn't acquire both forks, release left fork
            sem_post(left_fork);
        }

        sem_post(left_fork);   // release left fork
        sleep(rand() % 2 + 1); // small delay before thinking again
    }
}

// ... (Código del segundo programa)

// ... (Código del tercer programa)

int main()
{
    int opcion;

    while (1)
    {
        printf("Selecciona un programa a ejecutar:\n");
        printf("1. Problema de los filósofos cenando\n");
        printf("2. Lectores y escritores con semáforos\n");
        printf("3. Problema del barbero dormilón\n");
        printf("4. Salir\n");
        printf("Opción: ");

        scanf("%d", &opcion);

        switch (opcion)
        {
        case 1:
        {
            sem_t *forks[PHILOSOPHERS];
            for (int i = 0; i < PHILOSOPHERS; ++i)
            {
                char sem_name[10];
                sprintf(sem_name, "/fork%d", i);
                forks[i] = sem_open(sem_name, O_CREAT, 0644, 1);
            }

            for (int i = 0; i < PHILOSOPHERS; ++i)
            {
                if (fork() == 0)
                {
                    philosopher(i, forks[i], forks[(i + 1) % PHILOSOPHERS]);
                    exit(0);
                }
            }

            for (int i = 0; i < PHILOSOPHERS; ++i)
            {
                wait(NULL);
            }

            for (int i = 0; i < PHILOSOPHERS; ++i)
            {
                sem_close(forks[i]);
                char sem_name[10];
                sprintf(sem_name, "/fork%d", i);
                sem_unlink(sem_name);
            }
        }
        break;
        case 2:
            // Ejecutar el código de lectores y escritores con semáforos
            // Insertar aquí el código del segundo programa
            break;
        case 3:
            // Ejecutar el código del problema del barbero dormilón
            // Insertar aquí el código del tercer programa
            break;
        case 4:
            printf("Saliendo del menú...\n");
            return 0;
        default:
            printf("Opción inválida. Por favor, selecciona una opción válida.\n");
        }
    }

    return 0;
}
