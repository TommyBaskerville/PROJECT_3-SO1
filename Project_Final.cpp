#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

// ... (Código del primer programa)

// ... (Código del segundo programa)

// ... (Código del tercer programa)

int main() {
    int opcion;

    while (1) {
        printf("Selecciona un programa a ejecutar:\n");
        printf("1. Problema de los filósofos cenando\n");
        printf("2. Lectores y escritores con semáforos\n");
        printf("3. Problema del barbero dormilón\n");
        printf("4. Salir\n");
        printf("Opción: ");

        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                // Ejecutar el código del problema de los filósofos cenando
                // Insertar aquí el código del primer programa
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
