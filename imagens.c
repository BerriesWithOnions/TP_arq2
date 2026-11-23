#include "linhas.h"

static void *filtrar(void *arg) {
    Trabalho *t = arg;
    int n = t->n;
    for (int i = t->inicio; i < t->fim; i++) {
        for (int j = 0; j < n; j++) {
            if (i == 0 || j == 0 || i == n-1 || j == n-1) {
                t->saida[i*n+j] = t->entrada[i*n+j];
                continue;
            }
            int soma = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    soma += t->entrada[(i+di)*n + j+dj];
            t->saida[i*n+j] = soma / 9;
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    int n = argc >= 3 ? tamanho(argv[2]) : 0;
    if (!n) { fprintf(stderr, "Uso: ./imagens S N ou ./imagens P N threads\n"); return 1; }
    char nome[64];
    snprintf(nome, sizeof nome, "entradas/imagem_%d.raw", n);
    FILE *f = fopen(nome, "rb");
    if (!f) { perror(nome); return 1; }
    size_t pixels = (size_t)n * n;
    unsigned char *entrada = malloc(pixels), *saida = malloc(pixels);
    int erro = 1;
    if (entrada && saida && fread(entrada, 1, pixels, f) == pixels && fgetc(f) == EOF) {
        Trabalho dados = {.entrada=entrada, .saida=saida, .n=n};
        erro = executar(argc, argv, dados, filtrar);
        if (!erro) {
            printf("Pixel central: %u\n", (unsigned)saida[(n/2)*n+n/2]);
            erro = gravar(saida, pixels);
        }
    }
    fclose(f);
    free(entrada);
    free(saida);
    return erro;
}
