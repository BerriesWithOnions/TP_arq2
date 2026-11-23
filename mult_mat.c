#include "linhas.h"

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

void mult_seq(const int *A, const int *B, long *C, int n, int inicio, int fim) {

    for (int i = inicio; i < fim; i++) {
        for (int j = 0; j <= n-1; j++){
            long soma = 0;
            for (int k = 0; k <= n -1; k++){
                soma = soma + (long) A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = soma;

        }
    }
}

void *calcular_linhas(void *arg) {
    Trabalho *t = (Trabalho *)arg;

    mult_seq(t->a, t->b, t->produto,
             t->n, t->inicio, t->fim);

    return NULL;
}


int main(int argc, char **argv) {
    int n = argc >= 3 ? tamanho(argv[2]) : 0;

    if (!n) {
        fprintf(stderr, "Uso: ./mult_mat S N ou ./mult_mat P N threads\n");
        return 1;
    }

    size_t total = (size_t)n * n;

    int *A = malloc(total * sizeof(int));
    int *B = malloc(total * sizeof(int));
    long *C = malloc(total * sizeof(long));

    if (!A || !B || !C) {
        fprintf(stderr, "Erro ao alocar as matrizes\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }

    char nome[64], bnome[64];
    snprintf(nome, sizeof(nome), "A_%d.txt", n);
    snprintf(bnome, sizeof(bnome), "B_%d.txt", n);

    if (le_matriz(nome, A, n) || le_matriz(bnome, B, n)) {
        free(A);
        free(B);
        free(C);
        return 1;
    }

    Trabalho dados = {.a=A, .b=B, .produto=C, .n=n};

    int erro = executar(argc, argv, dados, calcular_linhas);

    if (!erro) {
        printf("C[0][0]: %ld\n", C[0]);
        printf("C[%d][%d]: %ld\n", n-1, n-1, C[total-1]);
        erro = gravar(C, total * sizeof(long));
    }

    free(A);
    free(B);
    free(C);

    return erro;
}
