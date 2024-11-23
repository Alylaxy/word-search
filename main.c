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

void preenche_matriz(FILE *file, char **matriz, int n_linhas, int n_colunas)
{
    char *linha = (char *)malloc(n_colunas * sizeof(char));
    // Lê cada linha do arquivo e salva em `linha`. Preferi fazer com um 'for' e numero de linhas para facilitar a paralelização.
    // TODO: Paralelizar a leitura de linhas
    for(int i = 0; i < n_linhas; i++)
    {
        if (fgets(linha, n_colunas + 2, file) != NULL)
        {
            strcpy(matriz[i], linha);
        }
    }
    rewind(file);
    free(linha);
}

void print_matriz(char **matriz, int n_linhas)
{
    printf("\n\nMatriz:\n");
    for (int i = 0; i < n_linhas; i++)
    {
        printf("%s", matriz[i]);
    }
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

void procura_palavra_na_linha(char **matriz, int n_colunas, int n_linhas, int linha_n, char **dicionario, int numero_palavras){
    int matches;
    char scan;
    char **palavras_encontradas = (char **)malloc(numero_palavras * sizeof(char *));
    int count_palavras_encontradas = 0;
    int linha_de_busca;
    int coluna_de_busca;
    for(int i = 0; i < n_colunas; ++i){
        for(int j = 0; j < numero_palavras; ++j){
            printf("\n=====\n=====\n[i = %d][j = %d]Procurando palavra %s\n", i, j, dicionario[j]);
            // scanf("%c", &scan);
            if(matriz[linha_n][i] == dicionario[j][0]){
                // Procura a palavra na horizontal
                printf("\n-------------------------------\n");
                printf("Procura a palavra na horizontal\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while(matriz[linha_de_busca][coluna_de_busca] == dicionario[j][matches]){
                    printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca]);
                    printf("matches = %d\n", matches);
                    matches++;
                    coluna_de_busca++;
                    if(matches == strlen(dicionario[j])){
                        printf("Achei a palavra %s\n", dicionario[j]);
                        char *palavra = (char *)malloc(strlen(dicionario[j]) * sizeof(char));
                        strcpy(palavra, dicionario[j]);
                        count_palavras_encontradas++;
                        palavras_encontradas[count_palavras_encontradas - 1] = palavra;
                    }
                    if(coluna_de_busca + 1 > n_colunas){
                        coluna_de_busca = 0;
                    }
                }
                // Procura a palavra na horizontal reversa
                printf("\n\n-------------------------------\n");
                printf("Procura a palavra na horizontal reversa\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while(matriz[linha_de_busca][coluna_de_busca] == dicionario[j][matches]){
                    printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca]);
                    printf("matches = %d\n", matches);
                    matches++;
                    coluna_de_busca--;
                    if(matches == strlen(dicionario[j])){
                        char *palavra = (char *)malloc(strlen(dicionario[j]) * sizeof(char));
                        strcpy(palavra, dicionario[j]);
                        count_palavras_encontradas++;
                        palavras_encontradas[count_palavras_encontradas] = palavra;
                    }
                    if(coluna_de_busca - 1 < 0){
                        coluna_de_busca = n_colunas - 1;
                    }
                }
                // Procura a palavra na vertical
                printf("\n\n-------------------------------\n");
                printf("Procura a palavra na vertical\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while(matriz[linha_de_busca][coluna_de_busca] == dicionario[j][matches]){
                    printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca]);
                    printf("matches = %d\n", matches);
                    matches++;
                    linha_de_busca++;
                    if(matches == strlen(dicionario[j])){
                        char *palavra = (char *)malloc(strlen(dicionario[j]) * sizeof(char));
                        strcpy(palavra, dicionario[j]);
                        count_palavras_encontradas++;
                        palavras_encontradas[count_palavras_encontradas] = palavra;
                    }
                    if(linha_de_busca + 1 > n_linhas){
                        linha_de_busca = 0;
                    }
                }
                // Procura a palavra na vertical reversa
                printf("\n\n-------------------------------\n");
                printf("Procura a palavra na vertical reversa\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while(matriz[linha_de_busca][coluna_de_busca] == dicionario[j][matches]){
                    printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca]);
                    printf("matches = %d\n", matches);
                    matches++;
                    linha_de_busca--;
                    if(matches == strlen(dicionario[j])){
                        char *palavra = (char *)malloc(strlen(dicionario[j]) * sizeof(char));
                        strcpy(palavra, dicionario[j]);
                        count_palavras_encontradas++;
                        palavras_encontradas[count_palavras_encontradas] = palavra;
                    }
                    if(linha_de_busca - 1 < 0){
                        linha_de_busca = n_linhas - 1;
                    }
                }
                // Diagonal direita baixo
                printf("\n\n-------------------------------\n");
                printf("Procura a palavra na diagonal direita baixo\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while(matriz[linha_de_busca][coluna_de_busca] == dicionario[j][matches]){
                    printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca]);
                    printf("matches = %d\n", matches);
                    matches++;
                    linha_de_busca++;
                    coluna_de_busca++;
                    if(matches == strlen(dicionario[j])){
                        char *palavra = (char *)malloc(strlen(dicionario[j]) * sizeof(char));
                        strcpy(palavra, dicionario[j]);
                        count_palavras_encontradas++;
                        palavras_encontradas[count_palavras_encontradas] = palavra;
                    }
                    if(linha_de_busca + 1 > n_linhas || coluna_de_busca + 1 > n_colunas){
                        break;
                    }
                }
                // Diagonal direita cima
                printf("\n\n-------------------------------\n");
                printf("Procura a palavra na diagonal direita cima\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while(matriz[linha_de_busca][coluna_de_busca] == dicionario[j][matches]){
                    printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca]);
                    printf("matches = %d\n", matches);
                    matches++;
                    linha_de_busca--;
                    coluna_de_busca++;
                    if(matches == strlen(dicionario[j])){
                        char *palavra = (char *)malloc(strlen(dicionario[j]) * sizeof(char));
                        strcpy(palavra, dicionario[j]);
                        count_palavras_encontradas++;
                        palavras_encontradas[count_palavras_encontradas] = palavra;
                    }
                    if(linha_de_busca - 1 < 0 || coluna_de_busca + 1 > n_colunas){
                        break;
                    }
                }
                // Diagonal esquerda baixo
                printf("\n\n-------------------------------\n");
                printf("Procura a palavra na diagonal esquerda baixo\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while(matriz[linha_de_busca][coluna_de_busca] == dicionario[j][matches]){
                    printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca]);
                    printf("matches = %d\n", matches);
                    matches++;
                    linha_de_busca++;
                    coluna_de_busca--;
                    if(matches == strlen(dicionario[j])){
                        char *palavra = (char *)malloc(strlen(dicionario[j]) * sizeof(char));
                        strcpy(palavra, dicionario[j]);
                        count_palavras_encontradas++;
                        palavras_encontradas[count_palavras_encontradas] = palavra;
                    }
                    if(linha_de_busca + 1 > n_linhas || coluna_de_busca - 1 < 0){
                        break;
                    }
                }
                // Diagonal esquerda cima
                printf("\n\n-------------------------------\n");
                printf("Procura a palavra na diagonal esquerda cima\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while(matriz[linha_de_busca][coluna_de_busca] == dicionario[j][matches]){
                    printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca]);
                    printf("matches = %d\n", matches);
                    matches++;
                    linha_de_busca--;
                    coluna_de_busca--;
                    if(matches == strlen(dicionario[j])){
                        char *palavra = (char *)malloc(strlen(dicionario[j]) * sizeof(char));
                        strcpy(palavra, dicionario[j]);
                        count_palavras_encontradas++;
                        palavras_encontradas[count_palavras_encontradas] = palavra;
                    }
                    if(linha_de_busca - 1 < 0 || coluna_de_busca - 1 < 0){
                        break;
                    }
                }
            }
        }
    }
    for(int i = 0; i < count_palavras_encontradas; i++)
    {
        printf("Palavra encontrada: %s\n", palavras_encontradas[i]);
    }
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
    preenche_matriz(file, caca_palavras, n_linhas, n_colunas);
    print_matriz(caca_palavras, n_linhas);

    procura_palavra_na_linha(caca_palavras, n_colunas, n_linhas, 0, dicionario, n_palavras);
    // Free em tudo que foi alocado
    for(int i = 0; i < n_palavras; i++)
    {
        free(dicionario[i]);
    }
    free(dicionario);
    fclose(file);

    fim = omp_get_wtime() - pausa_total;

    printf("\n\nTempo de execucao: %.5f\n", fim - inicio);
    return 0;
}