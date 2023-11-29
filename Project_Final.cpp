#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FILE_PATH "sharedfile.txt"

#define PHILOSOPHERS 5

#define N 5
#define M 10

// Codigo del primer programa

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

// Codigo del segundo programa

struct rw_semaphore
{
    sem_t mutex;
    sem_t write_lock;
};

void init_rwsem(struct rw_semaphore *rwsem)
{
    sem_init(&rwsem->mutex, 1, 1);      // Mutex para control de acceso a la sección crítica
    sem_init(&rwsem->write_lock, 1, 1); // Semáforo para bloquear escritura
}

void down_read(struct rw_semaphore *rwsem)
{
    sem_wait(&rwsem->mutex);
}

void up_read(struct rw_semaphore *rwsem)
{
    sem_post(&rwsem->mutex);
}

void down_write(struct rw_semaphore *rwsem)
{
    sem_wait(&rwsem->write_lock);
}

void up_write(struct rw_semaphore *rwsem)
{
    sem_post(&rwsem->write_lock);
}

void reader(struct rw_semaphore *rwsem, int id)
{
    FILE *file;
    char buffer[256];

    while (1)
    {
        down_read(rwsem);

        file = fopen(FILE_PATH, "r");
        if (file == NULL)
        {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        // Leer del archivo
        while (fgets(buffer, sizeof(buffer), file) != NULL)
        {
            printf("Reader %d read: %s", id, buffer);
        }

        fclose(file);

        up_read(rwsem);
        sleep(1);
    }
}

void writer(struct rw_semaphore *rwsem, int id)
{
    FILE *file;

    while (1)
    {
        down_write(rwsem);

        file = fopen(FILE_PATH, "a");
        if (file == NULL)
        {
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

// Codigo del tercer programa
struct
{
    sem_t barber;
    sem_t customers;
    sem_t mutex;
    int waiting;
} *shared;

void barber_function()
{
    while (1)
    {
        sem_wait(&shared->customers);
        sem_wait(&shared->mutex);
        shared->waiting--;
        sem_post(&shared->barber);
        sem_post(&shared->mutex);
        printf("Barber is cutting hair\n");
        sleep(2);
        printf("Barber finished cutting hair\n");
    }
}

void customer_function(int i)
{
    sem_wait(&shared->mutex);
    if (shared->waiting < N)
    {
        printf("Customer %d entering the shop\n", i);
        shared->waiting++;
        sem_post(&shared->customers);
        sem_post(&shared->mutex);
        sem_wait(&shared->barber);
    }
    else
    {
        sem_post(&shared->mutex);
        printf("Customer %d leaving, no chairs available\n", i);
    }
}

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
        {
            struct rw_semaphore rwsem;
            init_rwsem(&rwsem);

            pid_t pid;
            int reader_count = 3;
            int writer_count = 2;

            // Crear procesos lectores
            for (int i = 0; i < reader_count; i++)
            {
                pid = fork();
                if (pid == -1)
                {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0)
                { // Proceso hijo
                    reader(&rwsem, i + 1);
                    exit(EXIT_SUCCESS);
                }
            }

            // Crear procesos escritores
            for (int i = 0; i < writer_count; i++)
            {
                pid = fork();
                if (pid == -1)
                {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0)
                { // Proceso hijo
                    writer(&rwsem, i + 1);
                    exit(EXIT_SUCCESS);
                }
            }

            // Esperar a que los procesos hijos terminen
            for (int i = 0; i < reader_count + writer_count; i++)
            {
                wait(NULL);
            }
        }
        break;
        case 3:
        {
            shared = reinterpret_cast<decltype(shared)>(mmap(NULL, sizeof(*shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));

            sem_init(&shared->barber, 1, 0);
            sem_init(&shared->customers, 1, 0);
            sem_init(&shared->mutex, 1, 1);
            shared->waiting = 0;

            if (fork() == 0)
            {
                barber_function();
                _exit(0);
            }

            for (int i = 0; i < M; i++)
            {
                if (fork() == 0)
                {
                    customer_function(i + 1);
                    _exit(0);
                }
                sleep(1);
            }

            for (int i = 0; i < M; i++)
            {
                wait(NULL);
            }
        }
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
