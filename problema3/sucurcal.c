/*
 * Practica No. 2 - Sistemas Operativos 2
 * Problema 3: Tuberias con nombre (FIFO)
 *
 * Para cerrar el dia (avisa a centro.c que ya no llegaran mas
 * reportes y que debe imprimir el resumen final): ./sucursal cerrar
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_NAME "/tmp/central_fifo"
#define TAM_BUFFER 256
#define NOMBRE_MAX 60

// Limpia una linea erronea
void limpiar_buffer_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Descartar
    }
}

/* Pide el nombre de la sucursal, validando que no este vacio y que
   no contenga ':'*/
void leer_nombre_validado(char *nombre, int tam) {
    int valido;
    do {
        printf("Nombre de la sucursal: ");
        fgets(nombre, tam, stdin);
        nombre[strcspn(nombre, "\n")] = '\0';

        valido = 1;
        if (nombre[0] == '\0') {
            printf("Error: el nombre no puede estar vacio.\n");
            valido = 0;
        } else if (strchr(nombre, ':') != NULL) {
            printf("Error: el nombre no puede contener el caracter ':'.\n");
            valido = 0;
        }
    } while (!valido);
}

// Pide el monto, validando que sea un numero valido y no negativo
double leer_monto_validado(void) {
    double monto;
    int resultado;

    do {
        printf("Total de pagos procesados en el turno (Q): ");
        resultado = scanf("%lf", &monto);
        limpiar_buffer_entrada();

        if (resultado != 1) {
            printf("Error: debe ingresar un valor numerico valido.\n");
            monto = -1;
        } else if (monto < 0) {
            printf("Error: el monto no puede ser negativo.\n");
        }
    } while (monto < 0);

    return monto;
}

int main(int argc, char *argv[]) {
    char mensaje[TAM_BUFFER];

    if (argc == 2 && strcmp(argv[1], "cerrar") == 0) {
        strcpy(mensaje, "cerrar");
    } else {
        char nombre[NOMBRE_MAX];
        double monto;

        leer_nombre_validado(nombre, NOMBRE_MAX);
        monto = leer_monto_validado();

        snprintf(mensaje, TAM_BUFFER, "%s:%.2f", nombre, monto);
    }

    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("Error al abrir la FIFO. Esta centro.c ejecutandose?");
        exit(EXIT_FAILURE);
    }

    write(fd, mensaje, strlen(mensaje) + 1);
    close(fd);

    printf("Reporte enviado al centro de operaciones.\n");
    return 0;
}