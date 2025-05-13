#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define LIMITE 1000   // Límite superior (de 0 a 999)
#define MITAD 500     // Punto medio para dividir el trabajo

int main() {
    // Crear un segmento de memoria compartida
    int shmid;
    int *suma_compartida;
    
    // Solicitamos un segmento de memoria compartida
    shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("Error en shmget");
        exit(1);
    }
    
    // Adjuntamos el segmento de memoria compartida al espacio de direcciones de este proceso
    suma_compartida = (int *) shmat(shmid, NULL, 0);
    if (suma_compartida == (int *) -1) {
        perror("Error en shmat");
        exit(1);
    }
    
    // Inicializamos la suma en 0
    *suma_compartida = 0;
    
    // Creamos un proceso hijo con fork()
    pid_t pid = fork();
    
    if (pid < 0) {
        // Error al crear el proceso hijo
        perror("Error en fork");
        exit(1);
    } else if (pid == 0) {
        // Código ejecutado por el proceso hijo
        // El hijo suma de 0 a MITAD-1 (0 a 499)
        int suma_hijo = 0;
        for (int i = 0; i < MITAD; i++) {
            suma_hijo += i;
        }
        
        printf("Proceso hijo: suma de 0 a %d = %d\n", MITAD-1, suma_hijo);
        
        // Almacenamos el resultado en la memoria compartida
        *suma_compartida = suma_hijo;
        
        // Desvinculamos la memoria compartida
        shmdt(suma_compartida);
        
        exit(0);
    } else {
        // Código ejecutado por el proceso padre
        // El padre suma de MITAD a LIMITE-1 (500 a 999)
        int suma_padre = 0;
        for (int i = MITAD; i < LIMITE; i++) {
            suma_padre += i;
        }
        
        printf("Proceso padre: suma de %d a %d = %d\n", MITAD, LIMITE-1, suma_padre);
        
        // Esperamos a que el hijo termine
        int status;
        wait(&status);
        
        // Calculamos la suma total
        int suma_total = suma_padre + *suma_compartida;
        printf("Suma total de 0 a %d = %d\n", LIMITE-1, suma_total);
        
        // Verificamos el resultado con la fórmula n*(n-1)/2
        int verificacion = LIMITE * (LIMITE - 1) / 2;
        printf("Verificación con fórmula: %d\n", verificacion);
        
        // Liberamos la memoria compartida
        shmdt(suma_compartida);
        shmctl(shmid, IPC_RMID, NULL);
    }
    
    return 0;
}
