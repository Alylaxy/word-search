// Programinha top criado como nosso próprio filho.
// Pais: Jão e Lance
// Propósito: Encontra palavras em um caça palavras
// Forma de Utilização: Chame o programa com os parâmetros "arquivo", "numero de threads"
// Arquivo deve ser uma matriz de letras, simulando um caça palavras

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <limits.h>
#include <string.h>

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

double inicio, fim, pausa_total, pausa;

void acha_tamanho_caca_palavras(FILE *file, int *n_linhas, int *n_colunas){
    char linha[10000];
    *n_linhas = 0;
    *n_colunas = 0;

    while (fgets(linha, sizeof(linha), file) != NULL) {
        (*n_linhas)++;
        int len = strlen(linha);
        if (len > *n_colunas) {
            *n_colunas = len;
        }
    }
    // Desconsidera o \n do final
    --(*n_colunas);
    rewind(file);
}

void scan_com_pausa(char *format, void *var)
{
    pausa = omp_get_wtime();
    scanf(format, var);
    pausa_total += omp_get_wtime() - pausa;
}

char **cria_matriz(int n_linhas, int n_colunas)
{
    char **matriz = (char **)malloc(n_linhas * sizeof(char *));
    for (int i = 0; i < n_linhas; i++)
    {
        matriz[i] = (char *)malloc(n_colunas * sizeof(char));
    }
    return matriz;
}

void print_matriz(char **matriz, int n_linhas)
{
    printf("\n\nMatriz:\n");
    for (int i = 0; i < n_linhas; i++)
    {
        printf("%s", matriz[i]);
    }
    printf("\n\n");
}

char **cria_dicionario(int n_palavras, int n_linhas, int n_colunas)
{
    char **dicionario = (char **)malloc(n_palavras * sizeof(char *));

    // Não é possível que uma palavra tenha mais caracteres que o tamanho da linha ou da coluna
    int tamanho_palavra = max(n_linhas, n_colunas);
    for (int i = 0; i < n_palavras; i++)
    {
        char palavra_temp[1000];
        printf("Digite a palavra %d: ", i + 1);
        scan_com_pausa("%s", palavra_temp);
        if(strlen(palavra_temp) > tamanho_palavra)
        {
            printf("Palavra muito grande, tente novamente.\n Palavra precisa ser menor ou igual a max(numero de linhas, tamanho da linha)\n\n");
            i--;
        }
        else
        {
            dicionario[i] = (char *)malloc(tamanho_palavra * sizeof(char));
            strcpy(dicionario[i], palavra_temp);
        }
    }
    return dicionario;
}

int main(int argc, char **argv)
{
    // Definindo variáveis e tratando parâmetros
    inicio = omp_get_wtime();
    pausa_total = 0.0;
    FILE *file;
    char *arquivo = argv[1];
    int n_threads = atoi(argv[2]);
    int n_linhas;
    int n_colunas;
    omp_set_num_threads(n_threads);

    file = fopen(arquivo, "r");
    // Caso o arquivo não exista
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo \"%s\"\nCheque se o nome esta correto\n", arquivo);
        return 1;
    }

    acha_tamanho_caca_palavras(file, &n_linhas, &n_colunas);
    printf("Linhas: %d\nColunas: %d\n", n_linhas, n_colunas);

    char **caca_palavras = cria_matriz(n_linhas, n_colunas);
    int n_palavras;
    printf("Digite o numero de palavras a serem encontradas: ");
    scan_com_pausa("%d", &n_palavras);
    char **dicionario = cria_dicionario(n_palavras, n_linhas, n_colunas);

    char *linha = (char *)malloc(n_colunas * sizeof(char));
    // Lê cada linha do arquivo e salva em `linha`. Preferi fazer com um 'for' e numero de linhas para facilitar a paralelização.
    // TODO: Paralelizar a leitura de linhas
    for(int i = 0; i < n_linhas; i++)
    {
        if (fgets(linha, n_colunas + 2, file) != NULL)
        {
            strcpy(caca_palavras[i], linha);
        }
    }

    print_matriz(caca_palavras, n_linhas);

    // Free em tudo que foi alocado
    for(int i = 0; i < n_palavras; i++)
    {
        free(dicionario[i]);
    }
    free(dicionario);
    free(linha);
    fclose(file);

    fim = omp_get_wtime() - pausa_total;

    printf("\n\nTempo de execucao: %.5f\n", fim - inicio);
    return 0;
}