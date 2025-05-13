#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    
    // Llamar a fork para crear un proceso hijo
    pid = fork();
    
    // Verificar si hubo error en fork
    if (pid < 0) {
        // Error al crear el proceso hijo
        perror("Error en fork");
        exit(1);
    } 
    else if (pid == 0) {
        // Este código se ejecuta en el proceso hijo
        printf("Soy el proceso HIJO, mi PID es %d\n", getpid());
        printf("El PID de mi padre es %d\n", getppid());
    } 
    else {
        // Este código se ejecuta en el proceso padre
        printf("Soy el proceso PADRE, mi PID es %d\n", getpid());
        printf("He creado un proceso hijo con PID %d\n", pid);
        
        // Esperar a que el hijo termine
        int status;
        wait(&status);
        printf("Mi hijo ha terminado con estado: %d\n", WEXITSTATUS(status));
    }
    
    return 0;
}
