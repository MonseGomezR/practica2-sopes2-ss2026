/*
 * Practica No. 2 - Sistemas Operativos 2
 * Problema 2: Productor - Consumidor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PEDIDOS 20

// Limpia una linea erronea
void limpiar_buffer_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // descartar
    }
}

// Comprobacion del Input
int leer_entero_validado(const char *mensaje, int minimo, int maximo) {
    int valor;
    int resultado;

    do {
        printf("%s", mensaje);
        resultado = scanf("%d", &valor);

        if (resultado != 1) {
            printf("Error: debe ingresar solo digitos numericos.\n");
            limpiar_buffer_entrada();
            valor = minimo - 1;
            continue;
        }

        limpiar_buffer_entrada();

        if (valor < minimo || valor > maximo) {
            printf("Error: el valor debe estar entre %d y %d.\n",
                   minimo, maximo);
        }
    } while (valor < minimo || valor > maximo);

    return valor;
}


void estacion(int fd_lectura, const char *nombre) {
    int pedido;
    int contador = 0;
    int total_unidades = 0;
    ssize_t bytes;

    while ((bytes = read(fd_lectura, &pedido, sizeof(int))) > 0) {
        contador++;
        total_unidades += pedido;

        char linea[128];
        snprintf(linea, sizeof(linea),
                 "[%s] (PID %d) tomo un pedido de %d unidades\n",
                 nombre, getpid(), pedido);
        write(STDOUT_FILENO, linea, strlen(linea));
    }

    char reporte[512];
    snprintf(reporte, sizeof(reporte),
             "\n===== Reporte final %s (PID %d) =====\n"
             "Pedidos procesados: %d\n"
             "Total de unidades despachadas: %d\n"
             "==========================================\n\n",
             nombre, getpid(), contador, total_unidades);
    write(STDOUT_FILENO, reporte, strlen(reporte));
}

int main(void) {
    int fd[2];
    pid_t pid1, pid2;

    if (pipe(fd) == -1) {
        perror("Error al crear la tuberia (banda transportadora)");
        exit(EXIT_FAILURE);
    }

    // ---------- Estacion 1 ----------
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0) {
        close(fd[1]);
        estacion(fd[0], "Estacion 1");
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }

    // ---------- Estacion 2 ----------
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0) {
        close(fd[1]);
        estacion(fd[0], "Estacion 2");
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }

    // ---------- Proceso padre ----------
    close(fd[0]);

    int pedidos[NUM_PEDIDOS];
    char mensaje[64];
    for (int i = 0; i < NUM_PEDIDOS; i++) {
        snprintf(mensaje, sizeof(mensaje),
                 "Ingrese unidades del pedido #%d (1-100): ", i + 1);
        pedidos[i] = leer_entero_validado(mensaje, 1, 100);
    }

    printf("\nRegistro cerrado. Colocando los 20 pedidos en la banda "
           "transportadora...\n\n");

    for (int i = 0; i < NUM_PEDIDOS; i++) {
        write(fd[1], &pedidos[i], sizeof(int));
    }

    close(fd[1]); 

    wait(NULL);
    wait(NULL);

    printf("Banda transportadora vacia. Turno finalizado.\n");
    return 0;
}