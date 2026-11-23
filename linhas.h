#ifndef LINHAS_H
#define LINHAS_H
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    const int *a, *b;
    long *produto;
    const unsigned char *entrada;
    unsigned char *saida;
    int n, inicio, fim;
} Trabalho;

static void checar(int erro) {
    if (erro) {
        fprintf(stderr, "pthread: %s\n", strerror(erro));
        exit(EXIT_FAILURE);
    }
}

static int executar(int argc, char **argv, Trabalho dados, void *(*calcular)(void *)) {
    int p = 1;
    if (argc == 4 && strcmp(argv[1], "P") == 0) {
        if (strcmp(argv[3], "2") == 0) p = 2;
        else if (strcmp(argv[3], "4") == 0) p = 4;
        else if (strcmp(argv[3], "8") == 0) p = 8;
        else return 1;
    } else if (argc != 3 || strcmp(argv[1], "S") != 0) {
        return 1;
    }

    Trabalho partes[8];
    pthread_t threads[8];
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    if (p == 1) {
        dados.inicio = 0;
        dados.fim = dados.n;
        calcular(&dados);
    } else {
        for (int i = 0; i < p; i++) {
            partes[i] = dados;
            partes[i].inicio = i * dados.n / p;
            partes[i].fim = (i + 1) * dados.n / p;
            checar(pthread_create(&threads[i], NULL, calcular, &partes[i]));
        }
        for (int i = 0; i < p; i++) checar(pthread_join(threads[i], NULL));
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double tempo = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    printf("Tempo de execucao: %.9f\n", tempo);
    return 0;
}

static int tamanho(const char *s) {
    if (strcmp(s, "256") == 0) return 256;
    if (strcmp(s, "512") == 0) return 512;
    if (strcmp(s, "1024") == 0) return 1024;
    return 0;
}

/* SAIDA indica um arquivo opcional para comparar todos os elementos. */
static int gravar(const void *dados, size_t bytes) {
    const char *nome = getenv("SAIDA");
    if (!nome) return 0;
    FILE *f = fopen(nome, "wb");
    if (!f) { perror(nome); return 1; }
    size_t escritos = fwrite(dados, 1, bytes, f);
    return fclose(f) != 0 || escritos != bytes;
}
#endif
