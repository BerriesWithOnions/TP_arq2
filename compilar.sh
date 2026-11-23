#!/usr/bin/env bash
set -eu
set -x
gcc -O2 -Wall -Wextra -pthread contadores.c -o contadores
gcc -O2 -Wall -Wextra gera_matriz.c -o gera_matriz
gcc -O2 -Wall -Wextra -pthread mult_mat.c -o mult_mat
gcc -O2 -Wall -Wextra -pthread imagens.c -o imagens
