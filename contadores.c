#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define MAX 100000000L

typedef struct {
    int id;
    long inicio;
    long fim;
    long parcial;
} Args;

volatile long cont_global = 0;
pthread_mutex_t mutex_global = PTHREAD_MUTEX_INITIALIZER;


void *thread_B(void *arg){
    volatile long local = 0;
    Args *a = (Args *) (arg);
    for (long i = a->inicio; i < a->fim; i ++){
        local ++;
    }
    a->parcial = local;
    return NULL;
}

void *thread_A(void *arg){
    Args *a = (Args *) (arg);
    for (long i = a->inicio; i < a->fim; i++){
        pthread_mutex_lock(&mutex_global);
        cont_global++;
        pthread_mutex_unlock(&mutex_global);
    }
    
    return NULL;
}


long cont_seq(void){
    volatile long contador = 0;
    for (long i = 0; i < MAX; i++){
        contador++;
    }
    return contador;
}


long cont_A(int p){
    pthread_t threads[8];
    Args args[8];
    cont_global = 0;
    long bloco = MAX / p;
    for (int i = 0; i < p; i++){
        args[i].inicio = bloco * i;
        args[i].id = i;
        if (i == p-1){
            args[p-1].fim = MAX;
        } else{
            args[i].fim = args[i].inicio + bloco;
        }

        pthread_create(&threads[i], NULL, thread_A, &args[i]);
    }
    

    for (int i = 0; i < p; i++){
        pthread_join(threads[i], NULL);
    }

    return cont_global;
    
}

long cont_B(int p){
    pthread_t threads[8];
    Args args[8];
    long bloco = MAX / p;
    long total = 0;
    for (int i = 0; i < p; i++){
        args[i].inicio = bloco * i;
        args[i].id = i;
        if (i == p-1){
            args[p-1].fim = MAX;
        } else{
            args[i].fim = args[i].inicio + bloco;
        }

        pthread_create(&threads[i], NULL, thread_B, &args[i]);
    }

    for (int i =0; i < p; i++){
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < p; i++){
        total += args[i].parcial;
    }
    return total;
}



int main(int argc, char *argv[]){
    struct timespec t0, t1;
    long contador = 0;
    double segundos = 0.0;
    if (argc == 2 && strcmp(argv[1], "S") == 0){
        clock_gettime(CLOCK_MONOTONIC, &t0);
        contador = cont_seq();
        clock_gettime(CLOCK_MONOTONIC, &t1);
        segundos = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

        fprintf(stdout, "\nContador: %li", contador);
        fprintf(stdout, "\nTempo de execucao: %f\n", segundos);
        return 0;
    } 
    else if (argc == 3){
        int p = atoi(argv[2]);
        if (p == 2 || p == 4 || p == 8){
            if (strcmp(argv[1], "A") == 0){

                clock_gettime(CLOCK_MONOTONIC, &t0);
                contador = cont_A(p);
                clock_gettime(CLOCK_MONOTONIC, &t1);

                segundos = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

                fprintf(stdout, "\nContador: %li", contador);
                fprintf(stdout, "\nTempo de execucao: %f\n", segundos);
                return 0;
            }
            else if (strcmp(argv[1], "B") == 0){

                clock_gettime(CLOCK_MONOTONIC, &t0);
                contador = cont_B(p);
                clock_gettime(CLOCK_MONOTONIC, &t1);

                segundos = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

                fprintf(stdout, "\nContador: %li", contador);
                fprintf(stdout, "\nTempo de execucao: %f\n", segundos);
                return 0;
            }
            else{
                fprintf(stderr, "\nError!(Tipo de execucao)\n---------- MODO DE USO ----------\n./<nome_modulo> <tipo_execucao> <qtde_threads>");
                fprintf(stderr, "\nTIPOS DE EXECUCAO\n - S (Sequencial)\n - A (Paralela c/ contador unico)\n - B (Paralela c/ contador intervalado)");
                fprintf(stderr, "\nQUANTIDADE DE THREADS\n 2 - 4 - 8\n");
                return 1;
            }
            
        } else {
            fprintf(stderr, "\nError!(Quantidade de threads)\n---------- MODO DE USO ----------\n./<nome_modulo> <tipo_execucao> <qtde_threads>");
            fprintf(stderr, "\nTIPOS DE EXECUCAO\n - S (Sequencial)\n - A (Paralela c/ contador unico)\n - B (Paralela c/ contador intervalado)");
            fprintf(stderr, "\nQUANTIDADE DE THREADS\n 2 - 4 - 8\n");
            return 1;
        }

    }
    else{
        fprintf(stderr, "\nError!(Quantidade de Argumentos)\n---------- MODO DE USO ----------\n./<nome_modulo> <tipo_execucao> <qtde_threads>");
        fprintf(stderr, "\nTIPOS DE EXECUCAO\n - S (Sequencial)\n - A (Paralela c/ contador unico)\n - B (Paralela c/ contador intervalado)");
        fprintf(stderr, "\nQUANTIDADE DE THREADS\n 2 - 4 - 8\n");
        return 1;
    }
    return 0;
}