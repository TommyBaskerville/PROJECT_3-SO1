#include <semaphore.h>
#include <thread>

// Definición de los semáforos
sem_t x, wsem;

// Definición de semWait
void semWait(sem_t &sem) {
    sem_wait(&sem);
}

// Definición de semSignal
void semSignal(sem_t &sem) {
    sem_post(&sem);
}

// Definición de READUNIT
void READUNIT() {
    // Aquí va el código para leer el recurso
}

// Definición de WRITEUNIT
void WRITEUNIT() {
    // Aquí va el código para escribir en el recurso
}

// Definición de parbegin
void parbegin(void (*reader)(), void (*writer)()) {
    std::thread t1(reader);
    std::thread t2(writer);
    t1.join();
    t2.join();
}