#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

int le_matriz(const char *nome, int *m, int n){
    FILE *f = fopen(nome, "r");
    if (f == NULL) {
        fprintf(stderr, "Erro na abertura do arquivo %s!!\n", nome);
        return 1;
    }

    for (int k = 0; k <= (n * n -1); k++){
        if (fscanf(f, "%d", &m[k]) != 1) {
            fprintf(stderr, "Erro de leitura em %s (posicao %d)\n", nome, k);
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int main(void){
    int n = 256;
    int *A = malloc(n * n * sizeof(int));
    if (A == NULL){
        free(A);
        return 1;
    }
    char nome[64];
    snprintf(nome, sizeof(nome), "A_%d.txt", n);
    if (le_matriz(nome, A, n) == 0){

        for (int i = 0; i < 5; i++){
        fprintf(stdout, "Elemento [%i]: %i\n", i, A[i]);
        }
        printf("Ultimo: %d\n", A[n * n - 1]);
    } else{
        free(A);
        return 1;
    }
    free(A);
    return 0;
    
}