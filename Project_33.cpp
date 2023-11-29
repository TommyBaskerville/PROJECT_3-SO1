#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
 #include <fcntl.h>

#define N 5
#define M 10

struct {
    sem_t barber;
    sem_t customers;
    sem_t mutex;
    int waiting;
} *shared;

void barber_function() {
    while(1) {
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

void customer_function(int i) {
    sem_wait(&shared->mutex);
    if(shared->waiting < N) {
        printf("Customer %d entering the shop\n", i);
        shared->waiting++;
        sem_post(&shared->customers);
        sem_post(&shared->mutex);
        sem_wait(&shared->barber);
    } else {
        sem_post(&shared->mutex);
        printf("Customer %d leaving, no chairs available\n", i);
    }
}

int main() {

    shared = reinterpret_cast<decltype(shared)>(mmap(NULL, sizeof(*shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));

    sem_init(&shared->barber, 1, 0);
    sem_init(&shared->customers, 1, 0);
    sem_init(&shared->mutex, 1, 1);
    shared->waiting = 0;

    if (fork() == 0) {
        barber_function();
        _exit(0);
    }

    for(int i = 0; i < M; i++) {
        if(fork() == 0) {
            customer_function(i+1);
            _exit(0);
        }
        sleep(1);
    }

    for(int i = 0; i < M; i++) {
        wait(NULL);
    }

    return 0;
}