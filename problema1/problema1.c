/*
 * Practica No. 2 - Sistemas Operativos 2
 * Problema 1: Comunicacion unidireccional y bidireccional
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define TAM_BUFFER 256

// Comprobacion del Input
int es_numero(const char *texto) {
    if (texto == NULL || texto[0] == '\0') {
        return 0;
    }
    for (int i = 0; texto[i] != '\0'; i++) {
        if (!isdigit((unsigned char)texto[i])) {
            return 0;
        }
    }
    return 1;
}

// Limpia una linea erronea
void limpiar_buffer_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        //Descarta
    }
}

int main(void) {
    int fd_padre_hijo[2]; // el padre escribe [1], el hijo lee [0]
    int fd_hijo_padre[2]; // el hijo escribe [1], el padre lee [0]
    pid_t pid;
    char opcion;
    char palabra[TAM_BUFFER];
    char tarjeta[TAM_BUFFER];

    if (pipe(fd_padre_hijo) == -1 || pipe(fd_hijo_padre) == -1) {
        perror("Error al crear las tuberias");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("Error en fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ---------- Proceso hijo ----------
        close(fd_padre_hijo[1]); //no escribe en esta tuberia
        close(fd_hijo_padre[0]); // no lee de esta tuberia

        char buffer[TAM_BUFFER];
        ssize_t bytes;

        // ---- Fase de verificacion de canal ----
        bytes = read(fd_padre_hijo[0], buffer, TAM_BUFFER - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';

            if (strcmp(buffer, "SKIP") != 0) {
                // Invertir la cadena caracter por caracter
                int len = 0;
                while (buffer[len] != '\0') {
                    len++;
                }

                char invertida[TAM_BUFFER];
                for (int i = 0; i < len; i++) {
                    invertida[i] = buffer[len - 1 - i];
                }
                invertida[len] = '\0';

                write(fd_hijo_padre[1], invertida, len + 1);
            }
        }

        // ---- Fase de pago ----
        bytes = read(fd_padre_hijo[0], buffer, TAM_BUFFER - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            int numero = atoi(buffer);
            const char *respuesta =
                (numero % 2 == 0) ? "PAGO_APROBADO" : "PAGO_RECHAZADO";
            write(fd_hijo_padre[1], respuesta, strlen(respuesta) + 1);
        }

        close(fd_padre_hijo[0]);
        close(fd_hijo_padre[1]);
        exit(EXIT_SUCCESS);

    } else {
        // ---------- Proceso padre ----------
        close(fd_padre_hijo[0]); // no lee de esta tuberia
        close(fd_hijo_padre[1]); // no escribe en esta tuberia

        char respuesta[TAM_BUFFER];
        ssize_t bytes;

        do {
            printf("Desea verificar que el canal esta activo? (s/n): ");
            if (scanf(" %c", &opcion) != 1) {
                fprintf(stderr, "Entrada invalida. Intente de nuevo.\n");
                clearerr(stdin);
            }
            limpiar_buffer_entrada();

            if (opcion != 's' && opcion != 'S' &&
                opcion != 'n' && opcion != 'N') {
                printf("Opcion invalida. Escriba 's' o 'n'.\n");
            }
        } while (opcion != 's' && opcion != 'S' &&
                 opcion != 'n' && opcion != 'N');

        if (opcion == 's' || opcion == 'S') {
            printf("Escriba una palabra para verificar el canal: ");
            fgets(palabra, TAM_BUFFER, stdin);
            palabra[strcspn(palabra, "\n")] = '\0';

            write(fd_padre_hijo[1], palabra, strlen(palabra) + 1);

            bytes = read(fd_hijo_padre[0], respuesta, TAM_BUFFER - 1);
            if (bytes > 0) {
                respuesta[bytes] = '\0';
                printf("Canal activo. Respuesta del hijo (invertida): %s\n",
                       respuesta);
            }
        } else {
            write(fd_padre_hijo[1], "SKIP", 5);
        }

        int numero_tarjeta;
        do {
            printf("Ingrese el numero de tarjeta (1000-9999): ");
            fgets(tarjeta, TAM_BUFFER, stdin);
            tarjeta[strcspn(tarjeta, "\n")] = '\0';

            if (!es_numero(tarjeta)) {
                printf("Error: debe ingresar solo digitos numericos.\n");
                numero_tarjeta = -1;
                continue;
            }

            numero_tarjeta = atoi(tarjeta);
            if (numero_tarjeta < 1000 || numero_tarjeta > 9999) {
                printf("Error: el numero debe estar entre 1000 y 9999.\n");
                numero_tarjeta = -1;
            }
        } while (numero_tarjeta == -1);

        write(fd_padre_hijo[1], tarjeta, strlen(tarjeta) + 1);

        bytes = read(fd_hijo_padre[0], respuesta, TAM_BUFFER - 1);
        if (bytes > 0) {
            respuesta[bytes] = '\0';
            printf("Resultado del pago: %s\n", respuesta);
        }

        close(fd_padre_hijo[1]);
        close(fd_hijo_padre[0]);
        wait(NULL);
    }

    return 0;
}