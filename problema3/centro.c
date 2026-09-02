/*
 * Practica No. 2 - Sistemas Operativos 2
 * Problema 3: Tuberias con nombre (FIFO)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#define FIFO_NAME "/tmp/central_fifo"
#define TAM_BUFFER 256

int main(void) {
    // Crear la FIFO si todavia no existe
    if (mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST) {
        perror("Error al crear la FIFO");
        exit(EXIT_FAILURE);
    }

    printf("Centro de operaciones activo. Esperando reportes de "
           "sucursales...\n");
    printf("(FIFO: %s)\n\n", FIFO_NAME);

    double total_general = 0.0;
    int reportes_recibidos = 0;
    int activo = 1;

    while (activo) {
        int fd = open(FIFO_NAME, O_RDONLY);
        if (fd == -1) {
            perror("Error al abrir la FIFO para lectura");
            break;
        }

        char buffer[TAM_BUFFER];
        ssize_t bytes;

        while ((bytes = read(fd, buffer, TAM_BUFFER - 1)) > 0) {
            buffer[bytes] = '\0';

            if (strcmp(buffer, "cerrar") == 0) {
                activo = 0;
                break;
            }

            char nombre[TAM_BUFFER];
            double monto;
            if (sscanf(buffer, "%[^:]:%lf", nombre, &monto) == 2) {
                time_t ahora = time(NULL);
                struct tm *t = localtime(&ahora);
                reportes_recibidos++;
                total_general += monto;
                printf("[%02d:%02d:%02d] Reporte recibido de %s -> Q%.2f\n",
                       t->tm_hour, t->tm_min, t->tm_sec, nombre, monto);
            } else {
                printf("Mensaje no reconocido: %s\n", buffer);
            }
        }

        close(fd);
    }

    printf("\n===== RESUMEN DEL DIA =====\n");
    printf("Reportes recibidos: %d\n", reportes_recibidos);
    printf("Total de pagos procesados: Q%.2f\n", total_general);
    printf("============================\n");

    unlink(FIFO_NAME);
    return 0;
}