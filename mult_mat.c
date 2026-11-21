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

void mult_seq(const int *A, const int *B, long *C, int n){
    for (int i = 0; i <= n-1; i++){
        for (int j = 0; j <= n-1; j++){
            long soma = 0;
            for (int k = 0; k <= n -1; k++){
                soma = soma + (long) A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = soma;

        }
    }
}

int main(void){
    int n = 256;
    int *A = malloc(n * n * sizeof(int));
    if (A == NULL){
        free(A);
        return 1;
    }
    int *B = malloc(n *n * sizeof(int));
    if (B == NULL){
        free(B);
        return 1;
    }
    long *C = calloc(n *n, sizeof(long));
    if (C == NULL) {
        free(C);
        return 1;
    }

    char nome[64];
    char bnome[64];
    snprintf(nome, sizeof(nome), "A_%d.txt", n);
    snprintf(bnome, sizeof(bnome), "B_%d.txt", n);
    if (le_matriz(nome, A, n) == 0){

        for (int i = 0; i < 5; i++){
        fprintf(stdout, "Elemento [%i]: %i\n", i, A[i]);
        }
        printf("Ultimo: %d\n", A[n * n - 1]);
    } else{
        free(A);
        return 1;
    }

    if (le_matriz(bnome, B, n) == 0){

        mult_seq(A, B, C, n);
        fprintf(stdout, "C[0][0]: %ld\n", C[0]);
        fprintf(stdout, "C[0][1]: %ld\n", C[1]);
        fprintf(stdout, "C[255][255]: %ld\n", C[n*n -1]);
    }else{
        free(B);
        return 1;
    }
    free(A);
    free(B);
    free(C);
    return 0;
    
}