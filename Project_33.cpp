#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5
#define M 10

sem_t barber, customers;
pthread_mutex_t mutex;
int waiting = 0;


void* barber_function(void* arg) {
    while(1) {
        sem_wait(&customers);
        pthread_mutex_lock(&mutex);
        waiting--;
        sem_post(&barber);
        pthread_mutex_unlock(&mutex);
        printf("Barber is cutting hair\n");
        sleep(2);
        printf("Barber finished cutting hair\n");
    }
}

void* customer_function(void* arg) {
    pthread_mutex_lock(&mutex);
    if(waiting < N) {
        printf("Customer %d entering the shop\n", *(int*)arg);
        waiting++;
        sem_post(&customers);
        pthread_mutex_unlock(&mutex);
        sem_wait(&barber);
    } else {
        pthread_mutex_unlock(&mutex);
        printf("Customer %d leaving, no chairs available\n", *(int*)arg);
    }
}

int main() {
    pthread_t barber_thread, customer_threads[M];
    sem_init(&barber, 0, 0);
    sem_init(&customers, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&barber_thread, NULL, barber_function, NULL);
    int ids[M];
    for(int i = 0; i < M; i++) {
        ids[i] = i+1;
        pthread_create(&customer_threads[i], NULL, customer_function, (void*)&ids[i]);
    }

    for(int i = 0; i < M; i++) {
        pthread_join(customer_threads[i], NULL);
    }

    return 0;
}