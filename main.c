// Programinha top criado como nosso próprio filho.
// Pais: Jão e Lance
// Propósito: Encontra palavras em um caça palavras
// Forma de Utilização: Chame o programa com os parâmetros "arquivo", "numero de threads"
// Arquivo deve ser uma matriz de letras, simulando um caça palavras


/*
bug no seguinte input:

Digite o numero de palavras a serem encontradas: 4
Digite a palavra 1: kwa
Digite a palavra 2: wsa
Digite a palavra 3: kay
Digite a palavra 4: wfl


Suspeita: Diagonais de borda estão bugadas
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <limits.h>
#include <string.h>

#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define LIGHT_BLUE "\x1b[94m"
#define LIGHT_GREEN "\x1b[92m"

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

double inicio, fim;
typedef struct
{
    char letra;
    int cor; // Veja os defines acima. 0 = sem cor, 1 = RED, ... 8 = DARK_GRAY
} elemento;

void acha_tamanho_caca_palavras(FILE *file, int *n_linhas, int *n_colunas)
{
    char linha[10000];
    *n_linhas = 0;
    *n_colunas = 0;

    while (fgets(linha, sizeof(linha), file) != NULL)
    {
        (*n_linhas)++;
        int len = strlen(linha);
        if (len > *n_colunas)
        {
            *n_colunas = len;
        }
    }
    // Desconsidera o \n do final
    --(*n_colunas);
    rewind(file);
}

elemento **cria_matriz(int n_linhas, int n_colunas)
{
    elemento **matriz = (elemento **)malloc(n_linhas * sizeof(elemento *));
    elemento *dados = (elemento *)malloc(n_linhas * n_colunas * sizeof(elemento));

    for (int i = 0; i < n_linhas; i++)
    {
        // Artimanha ensinada pelo copilot.
        // De forma resumida, como dados foi inicializado como um array de elementos, ele é um ponteiro para o primeiro elemento.
        // Então, para acessar o elemento i da linha, basta acessar o endereço de dados + i * n_colunas.
        // Se eu quiser acessar o elemento da linha i e coluna j, basta acessar o endereço de dados + i * n_colunas + j.
        matriz[i] = &dados[i * n_colunas];
    }
    return matriz;
}

// Lê cada linha do arquivo e armazena os caracteres na matriz
void preenche_matriz(FILE *file, elemento **matriz, int n_linhas, int n_colunas)
{
    char *linha = (char *)malloc((n_colunas + 1) * sizeof(char)); // +1 para o '\n' ou '\0'
    for (int i = 0; i < n_linhas; i++)
    {
        if (fgets(linha, n_colunas + 2, file) != NULL) // +2 para o '\n' e '\0'
        {
            for (int j = 0; j < n_colunas; j++)
            {
                matriz[i][j].letra = linha[j];
                matriz[i][j].cor = 0;
            }
        }
    }
    free(linha);
}

void print_matriz(elemento **matriz, int n_linhas, int n_colunas)
{
    printf("\n\nMatriz:\n");
    for (int i = 0; i < n_linhas; i++)
    {
        for (int j = 0; j < n_colunas; j++)
        {
            switch (matriz[i][j].cor)
            {
            case 0:
                printf(RESET "%c " RESET, matriz[i][j].letra);
                break;
            case 1:
                printf(RED "%c " RESET, matriz[i][j].letra);
                break;
            case 2:
                printf(GREEN "%c " RESET, matriz[i][j].letra);
                break;
            case 3:
                printf(YELLOW "%c " RESET, matriz[i][j].letra);
                break;
            case 4:
                printf(BLUE "%c " RESET, matriz[i][j].letra);
                break;
            case 5:
                printf(MAGENTA "%c " RESET, matriz[i][j].letra);
                break;
            case 6:
                printf(CYAN "%c " RESET, matriz[i][j].letra);
                break;
            case 7:
                printf(LIGHT_BLUE "%c " RESET, matriz[i][j].letra);
                break;
            case 8:
                printf(LIGHT_GREEN "%c " RESET, matriz[i][j].letra);
                break;
            default:
                break;
            }
        }
        printf("\n");
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
        scanf("%s", palavra_temp);
        if (strlen(palavra_temp) > tamanho_palavra)
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

//matriz, linha_n, i, 1, strlen(dicionario[j])
void pinta_palavra(elemento **matriz, int linha, int coluna, int cor, int tamanho_palavra, int tamanho_linha, int tamanho_coluna)
{
    // printf("\n\nPintando palavra\n");
    switch (cor)
    {
    //horizontal
    case 1:
        for(int i = 0; i < tamanho_palavra; i++){
            if(coluna + i < tamanho_coluna){
                matriz[linha][coluna + i].cor = 1;
            }else{
                matriz[linha][coluna + i - tamanho_coluna].cor = 1;
            }
        }
        break;
    //horizontal reversa
    case 2:
        for(int i = 0; i < tamanho_palavra; i++){
            if(coluna - i >= 0){
                matriz[linha][coluna - i].cor = 2;
            }else{
                matriz[linha][coluna - i + tamanho_coluna].cor = 2;
            }
        }
        break;
    //vertical
    case 3:
        for(int i = 0; i < tamanho_palavra; i++){
            if(linha + i < tamanho_linha){
                matriz[linha + i][coluna].cor = 3;
            }else{
                matriz[linha + i - tamanho_linha][coluna].cor = 3;
            }
        }
        break;
    //vertical reversa
    case 4:
        for(int i = 0; i < tamanho_palavra; i++){
            if(linha - i >= 0){
                matriz[linha - i][coluna].cor = 4;
            }else{
                matriz[linha - i + tamanho_linha][coluna].cor = 4;
            }
        }
        break;
    //direita baixo
    case 5:
        for(int i = 0; i < tamanho_palavra; i++){
            matriz[linha + i][coluna + i].cor = 5;
        }
        break;
    //direita cima
    case 6:
        for(int i = 0; i < tamanho_palavra; i++){
            matriz[linha - i][coluna + i].cor = 6;
        }
        break;
    //esquerda baixo
    case 7:
        for(int i = 0; i < tamanho_palavra; i++){
            matriz[linha + i][coluna - i].cor = 7;
        }
        break;
    //esquerda cima
    case 8:
        for(int i = 0; i < tamanho_palavra; i++){
            matriz[linha - i][coluna - i].cor = 1;
        }
        break;
    
    default:
        break;
    }
}

void adiciona_palavra_encontrada(char **dicionario, int j, char **palavras_encontradas, int count_palavras_encontradas)
{
    char *palavra = (char *)malloc(strlen(dicionario[j] + 1) * sizeof(char));
    if (palavra == NULL)
    {
        perror("Erro ao alocar memória");
        exit(1);
    }
    strcpy(palavra, dicionario[j]);
    palavras_encontradas[count_palavras_encontradas] = palavra;
}
// Descomente para debugar
void procura_palavra_na_linha(elemento **matriz, int n_colunas, int n_linhas, int linha_n, char **dicionario, int numero_palavras)
{
    int matches;
    char scan;
    char **palavras_encontradas = (char **)malloc(numero_palavras * sizeof(char *));
    int count_palavras_encontradas = 0;
    int linha_de_busca;
    int coluna_de_busca;
    int achou = 0;
    for (int i = 0; i < n_colunas; i++)
    {
        for (int j = 0; j < numero_palavras; j++)
        {
            // printf("\n=====\n=====\nProcurando palavra %s\nLinha %d; Coluna %d", dicionario[j], linha_n, i);
            // scanf("%c", &scan);
            achou = 0;
            if (matriz[linha_n][i].letra == dicionario[j][0])
            {
                // printf("\n\n-------------------------------\n");
                // printf("Procura a palavra na horizontal\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while (matriz[linha_de_busca][coluna_de_busca].letra == dicionario[j][matches])
                {
                    // printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca].letra);
                    // printf("matches = %d\n", matches);
                    matches++;
                    if (matches == strlen(dicionario[j]))
                    {
                        achou = 1;
                        adiciona_palavra_encontrada(dicionario, j, palavras_encontradas, count_palavras_encontradas);
                        count_palavras_encontradas++;
                        pinta_palavra(matriz, linha_n, i, 1, strlen(dicionario[j]), n_linhas, n_colunas);
                        break;
                    }
                    // Checa se a próxima iteração transbordaria a matriz, antes de alterar os índices
                    // printf("COLUNA: %d\n", coluna_de_busca);
                    if (coluna_de_busca + 1 > n_colunas - 1)
                    {
                        coluna_de_busca = 0;
                    }else{
                        coluna_de_busca += 1;
                    }
                }
                if(achou) continue;
                // printf("\n\n-------------------------------\n");
                // printf("Procura a palavra na horizontal reversa\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while (matriz[linha_de_busca][coluna_de_busca].letra == dicionario[j][matches])
                {
                    // printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca].letra);
                    // printf("matches = %d\n", matches);
                    matches++;
                    if (matches == strlen(dicionario[j]))
                    {
                        achou = 1;
                        adiciona_palavra_encontrada(dicionario, j, palavras_encontradas, count_palavras_encontradas);
                        count_palavras_encontradas++;
                        pinta_palavra(matriz, linha_n, i, 2, strlen(dicionario[j]), n_linhas, n_colunas);
                        break;
                    }
                    // Checa se a próxima iteração transbordaria a matriz, antes de alterar os índices
                    if (coluna_de_busca - 1 < 0)
                    {
                        coluna_de_busca = n_colunas - 1;
                    }else{
                        coluna_de_busca--;
                    }
                }
                if(achou) continue;
                // printf("\n\n-------------------------------\n");
                // printf("Procura a palavra na vertical\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while (matriz[linha_de_busca][coluna_de_busca].letra == dicionario[j][matches])
                {
                    // printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca].letra);
                    // printf("matches = %d\n", matches);
                    matches++;
                    if (matches == strlen(dicionario[j]))
                    {
                        achou = 1;
                        adiciona_palavra_encontrada(dicionario, j, palavras_encontradas, count_palavras_encontradas);
                        count_palavras_encontradas++;
                        pinta_palavra(matriz, linha_n, i, 3, strlen(dicionario[j]), n_linhas, n_colunas);
                        break;
                    }
                    // printf("linha_de_busca atual: %d\n", linha_de_busca);
                    // Checa se a próxima iteração transbordaria a matriz, antes de alterar os índices
                    if (linha_de_busca + 1 > n_linhas - 1)
                    {
                        linha_de_busca = 0;
                        // printf("linha_de_busca acessada: %d\n", linha_de_busca);
                    }else{
                        linha_de_busca += 1;
                        // printf("linha_de_busca acessada: %d\n", linha_de_busca);
                    }
                }
                if(achou) continue;
                // printf("\n\n-------------------------------\n");
                // printf("Procura a palavra na vertical reversa\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while (matriz[linha_de_busca][coluna_de_busca].letra == dicionario[j][matches])
                {
                    // printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca].letra);
                    matches++;
                    // printf("matches = %d\n", matches);
                    if (matches == strlen(dicionario[j]))
                    {
                        achou = 1;
                        adiciona_palavra_encontrada(dicionario, j, palavras_encontradas, count_palavras_encontradas);
                        count_palavras_encontradas++;
                        pinta_palavra(matriz, linha_n, i, 4, strlen(dicionario[j]), n_linhas, n_colunas);
                        break;
                    }
                    // Checa se a próxima iteração transbordaria a matriz, antes de alterar os índices
                    if (linha_de_busca - 1 < 0)
                    {
                        linha_de_busca = n_linhas - 1;
                    }else{
                        linha_de_busca -= 1;
                    }
                }
                if(achou) continue;
                // printf("\n\n-------------------------------\n");
                // printf("Procura a palavra na diagonal direita baixo\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while (matriz[linha_de_busca][coluna_de_busca].letra == dicionario[j][matches])
                {
                    matches++;
                    // printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca].letra);
                    // printf("matches = %d\n", matches);
                    if (matches == strlen(dicionario[j]))
                    {
                        achou = 1;
                        adiciona_palavra_encontrada(dicionario, j, palavras_encontradas, count_palavras_encontradas);
                        count_palavras_encontradas++;
                        pinta_palavra(matriz, linha_n, i, 5, strlen(dicionario[j]), n_linhas, n_colunas);
                        break;
                    }
                    // Checa se a próxima iteração transbordaria a matriz, antes de alterar os índices
                    if (linha_de_busca + 1 > n_linhas - 1 || coluna_de_busca + 1 > n_colunas - 1)
                    {
                        break;
                    }else{
                        linha_de_busca += 1;
                        coluna_de_busca += 1;
                    }
                }
                if(achou) continue;
                // printf("\n\n-------------------------------\n");
                // printf("Procura a palavra na diagonal direita cima\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while (matriz[linha_de_busca][coluna_de_busca].letra == dicionario[j][matches])
                {
                    matches++;
                    // printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca].letra);
                    // printf("matches = %d\n", matches);
                    if (matches == strlen(dicionario[j]))
                    {
                        achou = 1;
                        adiciona_palavra_encontrada(dicionario, j, palavras_encontradas, count_palavras_encontradas);
                        count_palavras_encontradas++;
                        pinta_palavra(matriz, linha_n, i, 6, strlen(dicionario[j]), n_linhas, n_colunas);
                        break;
                    }
                    // Checa se a próxima iteração transbordaria a matriz, antes de alterar os índices
                    if (linha_de_busca - 1 < 0 || coluna_de_busca + 1 > n_colunas - 1)
                    {
                        break;
                    }else{
                        linha_de_busca -= 1;
                        coluna_de_busca += 1;
                    }
                }
                if(achou) continue;
                // printf("\n\n-------------------------------\n");
                // printf("Procura a palavra na diagonal esquerda baixo\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while (matriz[linha_de_busca][coluna_de_busca].letra == dicionario[j][matches])
                {
                    matches++;
                    // printf("Matriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca].letra);
                    // printf("matches = %d\n", matches);
                    if (matches == strlen(dicionario[j]))
                    {
                        achou = 1;
                        adiciona_palavra_encontrada(dicionario, j, palavras_encontradas, count_palavras_encontradas);
                        count_palavras_encontradas++;
                        pinta_palavra(matriz, linha_n, i, 7, strlen(dicionario[j]), n_linhas, n_colunas);
                        break;
                    }
                    // Checa se a próxima iteração transbordaria a matriz, antes de alterar os índices
                    if (linha_de_busca + 1 > n_linhas - 1 || coluna_de_busca - 1 < 0)
                    {
                        break;
                    }else{
                        linha_de_busca += 1;
                        coluna_de_busca -= 1;
                    }
                }
                if(achou) continue;
                // printf("\n\n-------------------------------\n");
                // printf("Procura a palavra na diagonal esquerda cima\n");
                matches = 0;
                linha_de_busca = linha_n;
                coluna_de_busca = i;
                while (matriz[linha_de_busca][coluna_de_busca].letra == dicionario[j][matches])
                {
                    matches++;
                    // printf("\n---\nMatriz[%d][%d] = %c\n", linha_de_busca, coluna_de_busca, matriz[linha_de_busca][coluna_de_busca].letra);
                    // printf("matches = %d\n", matches);
                    if (matches == strlen(dicionario[j]))
                    {
                        achou = 1;
                        adiciona_palavra_encontrada(dicionario, j, palavras_encontradas, count_palavras_encontradas);
                        count_palavras_encontradas++;
                        pinta_palavra(matriz, linha_n, i, 8, strlen(dicionario[j]), n_linhas, n_colunas);
                        break;
                    }
                    // Checa se a próxima iteração transbordaria a matriz, antes de alterar os índices
                    if (linha_de_busca - 1 < 0 || coluna_de_busca - 1 < 0)
                    {
                        break;
                    }else{
                        linha_de_busca -= 1;
                        coluna_de_busca -= 1;
                    }
                }
            }
        }
    }
    for (int i = 0; i < count_palavras_encontradas; i++)
    {
        printf("\n--------\nPalavras encontradas na linha %d: %s\n", linha_n, palavras_encontradas[i]);
    }

    for (int i = 0; i < count_palavras_encontradas; i++)
    {
        free(palavras_encontradas[i]);
    }
    free(palavras_encontradas);
}

int main(int argc, char **argv)
{
    // Definindo variáveis e tratando parâmetros
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
    // printf("Linhas: %d\nColunas: %d\n", n_linhas, n_colunas);

    elemento **caca_palavras = cria_matriz(n_linhas, n_colunas);
    int n_palavras;
    printf("Digite o numero de palavras a serem encontradas: ");
    scanf("%d", &n_palavras);
    char **dicionario = cria_dicionario(n_palavras, n_linhas, n_colunas);
    preenche_matriz(file, caca_palavras, n_linhas, n_colunas);
    print_matriz(caca_palavras, n_linhas, n_colunas);

    inicio = omp_get_wtime();
    #pragma omp parallel for
    for (int i = 0; i < n_linhas; i++)
    {
        procura_palavra_na_linha(caca_palavras, n_colunas, n_linhas, i, dicionario, n_palavras);
    }

    print_matriz(caca_palavras, n_linhas, n_colunas);
    fim = omp_get_wtime();

    // Free em tudo que foi alocado
    for (int i = 0; i < n_palavras; i++)
    {
        free(dicionario[i]);
    }
    free(dicionario);
    fclose(file);
    for(int i = 0; i < n_linhas; i++){
        free(caca_palavras[i]);
    }
    free(caca_palavras);


    printf("\n\nTempo de execucao com %d threads: %.5f\n", n_threads, fim - inicio);
    return 0;
}