// Programinha top criado como nosso próprio filho.
// Pais: Jão e Lance
// Propósito: Encontra palavras em um caça palavras
// Forma de Utilização: Chame o programa com os parâmetros "arquivo", "tamanho das colunas", "tamanho das linhas"
// Arquivo deve ser uma matriz de letras, simulando um caça palavras

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <limits.h>

#ifndef max
    #define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
    #define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

int main(int *argc, char **argv){
    // Definindo variáveis e tratando parâmetros
    FILE *file;
    char *arquivo = argv[1];
    int n_linhas = atoi(argv[2]);
    int n_colunas = atoi(argv[3]);

    char *linha = (char *)malloc(n_colunas * sizeof(char));
    file = fopen(arquivo, "r");

    int n_palavras;
    printf("Digite o numero de palavras a serem encontradas: ");
    scanf("%d", &n_palavras);
    char **dicionario = (char **)malloc(n_palavras * sizeof(char *));

    // Não é possível que uma palavra tenha mais caracteres que o tamanho da linha ou da coluna
    int tamanho_palavra = max(n_linhas, n_colunas);
    for(int i = 0; i < n_palavras; i++){
        dicionario[i] = (char *)malloc(tamanho_palavra * sizeof(char));
        printf("Digite a palavra %d: ", i+1);
        scanf("%s", dicionario[i]);
    }

    // Caso o arquivo não exista
    if(file == NULL){
        printf("Erro ao abrir o arquivo \"%s\"\nCheque se o nome esta correto\n", arquivo);
        return 1;
    }

    // Lê cada linha do arquivo e salva em linha. Preferi fazer com um for e numero de colunas para facilitar a paralelização.
    for(int i = 0; i < n_linhas; i++){
        if (fgets(linha, n_colunas + 2, file) != NULL) {
            printf("%s", linha);
        } else {
            break; // Sai do loop se não conseguir ler mais linhas
        }
    }

    // Free em tudo que foi alocado
    for(int i = 0; i < n_palavras; i++){
        free(dicionario[i]);
    }
    free(dicionario);
    free(linha);
    fclose(file);
    return 0;
}