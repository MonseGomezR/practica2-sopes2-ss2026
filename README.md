# Práctica No. 2 — Tuberías (Pipe)
**Sistemas Operativos 2** · Universidad de San Carlos de Guatemala, CUNOC

Instrucciones de compilación y ejecución de los 3 problemas en **Linux nativo o WSL*

---

## Contenido

| Archivo | Problema | Descripción |
|---|---|---|
| `problema1.c` | 1 | Verificación de canal + pago con tarjeta (pipes anónimos, `fork()`) |
| `problema2.c` | 2 | Productor-consumidor con 2 estaciones compitiendo por un pipe |
| `centro.c` | 3 | Centro de operaciones (recibe reportes vía FIFO) |
| `sucursal.c` | 3 | Sucursal (envía reportes vía FIFO) |

---

## Requisitos previos

Se necesita un compilador de C (`gcc`) con soporte para llamadas POSIX (`fork()`, `pipe()`, `mkfifo()`).

### Linux (Ubuntu/Debian) o Windows con WSL
```bash
sudo apt update
sudo apt install gcc
```

---

## Problema 1 — Verificación de canal y pago con tarjeta

### Compilar
```bash
gcc problema1.c -o problema1
```
### Ejecutar
```bash
./problema1
```

### Flujo esperado
1. Pregunta si deseas verificar el canal (`s`/`n`).
   - Si respondes `s`: pide una palabra, la envía al proceso hijo por un pipe, el hijo la invierte carácter por carácter y responde por un segundo pipe.
   - Si respondes `n`: omite este paso.
2. Pide el número de tarjeta (debe estar entre 1000 y 9999; el programa valida que sea numérico y esté en rango, repitiendo la pregunta si no).
3. El hijo responde `PAGO_APROBADO` (par) o `PAGO_RECHAZADO` (impar), y el padre lo imprime.

No requiere ninguna terminal adicional ni configuración extra: es un solo programa con `fork()` interno.

---

## Problema 2 — Productor-Consumidor

### Compilar
```bash
gcc problema2.c -o problema2
```

### Ejecutar
```bash
./problema2
```

### Flujo esperado
1. Pide 20 valores (unidades por pedido, entre 1 y 100), validando que cada uno sea numérico y esté en rango.
2. Al ingresar el pedido #20, coloca todos los valores en la "banda transportadora" (un pipe).
3. Dos procesos hijo ("Estación 1" y "Estación 2") compiten por leer los pedidos disponibles.
4. Al vaciarse la banda, cada estación imprime su reporte final (pedidos procesados y total de unidades).

> El reparto de pedidos entre estaciones es **no determinista** (depende del scheduler del sistema operativo) — es normal, e incluso deseable, que varíe entre ejecuciones. Es la evidencia de la *race condition* que pide el enunciado.

No requiere ninguna terminal adicional: es un solo programa con dos `fork()` internos.

---

## Problema 3 — FIFO (tubería con nombre)

A diferencia de los dos problemas anteriores, este ejercicio usa **dos programas independientes** (`centro.c` y `sucursal.c`) que **no tienen relación de parentesco** (no hay `fork()` entre ellos) y deben ejecutarse en **terminales separadas**, simulando sucursales reales enviando reportes en distintos momentos.

### Compilar (ambos programas)
```bash
gcc centro.c -o centro
gcc sucursal.c -o sucursal
```

### Ruta de la FIFO
Ambos programas usan `/tmp/central_fifo` como ruta de la tubería con nombre. Esta ruta funciona igual en Linux, WSL y en la terminal MSYS2 MSYS (que sí tiene un `/tmp` funcional), así que **no es necesario modificar el código**.

### Ejecutar

**Paso 1 — Levanta el centro de operaciones (Terminal 1)**
```bash
./centro
```
Se queda corriendo y esperando reportes. Verás:
```
Centro de operaciones activo. Esperando reportes de sucursales...
(FIFO: /tmp/central_fifo)
```
No cierres esta terminal mientras pruebas.

**Paso 2 — Envía reportes desde una o varias sucursales (Terminal 2, 3, 4...)**

Abre una **nueva ventana o pestaña de terminal**, navega a la misma carpeta (`cd /ruta/a/la/carpeta`) y ejecuta:
```bash
./sucursal
```
Te pedirá el nombre de la sucursal y el monto total de pagos del turno. Puedes repetir este paso en la misma terminal o en otras distintas, tantas veces como sucursales quieras simular. Cada envío aparecerá de inmediato en la Terminal 1 (`centro`), con la hora exacta de llegada.

**Paso 3 — Cierra el día**

Cuando ya no vayan a llegar más reportes, desde cualquier terminal ejecuta:
```bash
./sucursal cerrar
```
Esto le indica a `centro` que debe imprimir el resumen final (total de reportes recibidos y monto acumulado) y terminar su ejecución.

### Ejemplo de salida esperada

Terminal 2 (sucursal):
```
Nombre de la sucursal: Internacional
Total de pagos procesados en el turno (Q): 1500
Reporte enviado al centro de operaciones.
```

Terminal 1 (centro):
```
[10:33:21] Reporte recibido de Internacional -> Q1500.00
...
===== RESUMEN DEL DIA =====
Reportes recibidos: 3
Total de pagos procesados: Q4400.00
============================
```

---

## Solución de problemas comunes

| Problema | Causa probable | Solución |
|---|---|---|
| `gcc: command not found` | El compilador no está instalado, o (en MSYS2) la terminal no recargó el PATH | Instala gcc con `pacman -S gcc` o `apt install gcc`, y reinicia la terminal |
| `Error al abrir la FIFO. ¿Esta centro.c ejecutandose?` | Intentaste correr `./sucursal` sin haber levantado `./centro` primero | Ejecuta `./centro` en otra terminal antes de correr `./sucursal` |
