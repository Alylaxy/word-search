// Programinha top criado como nosso próprio filho.
// Pais: Jão e Lance
// Propósito: Encontra palavras em um caça palavras
// Forma de Utilização: Chame o programa com os parâmetros "arquivo", "tamanho das colunas", "tamanho das linhas", "numero de threads"
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

//TODO: Segurança ao redor do tamanho máximo de uma palavra
char **cria_dicionario(int n_palavras, int n_linhas, int n_colunas)
{
    char **dicionario = (char **)malloc(n_palavras * sizeof(char *));

    // Não é possível que uma palavra tenha mais caracteres que o tamanho da linha ou da coluna
    int tamanho_palavra = max(n_linhas, n_colunas);
    for (int i = 0; i < n_palavras; i++)
    {
        dicionario[i] = (char *)malloc(tamanho_palavra * sizeof(char));
        printf("Digite a palavra %d: ", i + 1);
        scanf("%s", dicionario[i]);
    }
}

int main(int *argc, char **argv)
{
    // Definindo variáveis e tratando parâmetros
    double inicio = omp_get_wtime(), fim;
    FILE *file;
    char *arquivo = argv[1];
    int n_linhas = atoi(argv[2]);
    int n_colunas = atoi(argv[3]);
    int n_threads = atoi(argv[4]);
    omp_set_num_threads(n_threads);

    file = fopen(arquivo, "r");
    // Caso o arquivo não exista
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo \"%s\"\nCheque se o nome esta correto\n", arquivo);
        return 1;
    }

    int n_palavras;
    printf("Digite o numero de palavras a serem encontradas: ");
    scanf("%d", &n_palavras);
    char **dicionario = cria_dicionario(n_palavras, n_linhas, n_colunas);

    char *linha = (char *)malloc(n_colunas * sizeof(char));
    // Lê cada linha do arquivo e salva em linha. Preferi fazer com um for e numero de colunas para facilitar a paralelização.
    // TODO: Paralelizar a leitura de linhas
    for(int i = 0; i < n_linhas; i++)
    {
        if (fgets(linha, n_colunas + 2, file) != NULL)
        {
            printf("%s", linha);
        }
        else
        {
            break; // Sai do loop se não conseguir ler mais linhas
        }
    }

    // Free em tudo que foi alocado
    for(int i = 0; i < n_palavras; i++)
    {
        free(dicionario[i]);
    }
    free(dicionario);
    free(linha);
    fclose(file);

    fim = omp_get_wtime();

    printf("Tempo de execução: %f\n", fim - inicio);
    return 0;
}