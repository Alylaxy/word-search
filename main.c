// Programinha top criado como nosso próprio filho.
// Pais: Jão e Lance
// Propósito: Encontra palavras em um caça palavras
// Forma de Utilização: Chame o programa com os parâmetros "arquivo", "tamanho da linha", "tamanho da coluna"
// Arquivo deve ser uma matriz de letras, simulando um caça palavras

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <omp.h>
#include <limits.h>

int main(int *argc, char **argv){
    // Definindo variáveis e tratando parâmetros
    FILE *file;
    char *arquivo = argv[1];
    int tamanho_linha = atoi(argv[2]);
    int tamanho_coluna = atoi(argv[3]);

    char linha = (char *)malloc(tamanho_linha * sizeof(char));
    file = fopen(arquivo, "r");

    int n_palavras;
    scanf("%d", &n_palavras);
    char **dicionario = (char **)malloc(n_palavras * sizeof(char *));

    // Não é possível que uma palavra tenha mais caracteres que o tamanho da linha ou da coluna
    int tamanho_palavra = max(tamanho_linha, tamanho_coluna);
    for(int i = 0; i < n_palavras; i++){
        dicionario[i] = (char *)malloc(tamanho_palavra * sizeof(char));
        scanf("%s", dicionario[i]);
    }

    // Caso o arquivo não exista
    if(file == NULL){
        printf("Erro ao abrir o arquivo \"%s\"\nCheque se o nome esta correto\n", arquivo);
        return 1;
    }

    // Lê cada linha do arquivo e salva em linha. Preferi fazer com um for e numero de colunas para facilitar a paralelização.
    for(int i = 0; i < tamanho_coluna; i++){
        fgets(linha, tamanho_linha, file);
        printf("%s", linha);
    }

    fclose(file);
    return 0;
}