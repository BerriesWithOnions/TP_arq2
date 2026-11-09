#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>

#define MAX 100000000L


long cont_seq(void){
    volatile long contador = 0;
    for (long i = 0; i < MAX; i++){
        contador++;
    }
    return contador;
}



int main(int argc, char *argv[]) {
    struct timespec t0, t1;
    
    if (argc >= 2){
        if (strcmp(argv[1], "S") == 0){
            printf("\n ----------- EXECUCAO SEQUENCIAL -----------\n");

            clock_gettime(CLOCK_MONOTONIC, &t0);
            long contador = cont_seq();
            clock_gettime(CLOCK_MONOTONIC, &t1);

            double seg = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
            printf("\nTempo de execução: %lfs", seg);
            printf("\nValor final do Contador: %ld\n", contador);
        }
        else if (strcmp(argv[1], "A") == 0){


            if (strcmp(argv[2], "2") == 0){
                printf("\n ----------- EXECUÇÃO POR THREADS - CONTADOR UNICO - 2 THREADS -----------\n");
            }


            else if (strcmp(argv[2], "4") == 0){
               printf("\n ----------- EXECUÇÃO POR THREADS - CONTADOR UNICO - 4 THREADS -----------\n");
            }


            else if (strcmp(argv[2], "8") == 0){
                printf("\n ----------- EXECUÇÃO POR THREADS - CONTADOR UNICO - 8 THREADS -----------\n");
            }


            else{
                printf("\nError!(Quantidade de Threads Invalida!)\n---------- MODO DE USO ----------\n./<nome_modulo> <tipo_execucao> <qtde_threads> <---- incorrect argument\n");
                printf("\nQUANTIDADE DE THREADS\n 2 - 4 - 8\n");
            }

            
        }

    } else{
        printf("\nError!(Argumentos faltando)\n---------- MODO DE USO ----------\n./<nome_modulo> <tipo_execucao> <qtde_threads>");
        printf("\nTIPOS DE EXECUCAO\n - S (Sequencial)\n - A (Paralela c/ contador unico)\n - B (Paralela c/ contador intervalado)");
        printf("\nQUANTIDADE DE THREADS\n 2 - 4 - 8\n");
    }
}