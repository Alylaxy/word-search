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

typedef struct
{
    int linha;
    int coluna;
} coordenadas_caracter;

typedef struct
{
    int tamanho;
    int achada;
    int direcao;
    coordenadas_caracter *coordenadas;
} coordenadas_palavra;


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
            printf("Adicionando Palavra %d: %s\n", i + 1, palavra_temp);
            dicionario[i] = (char *)malloc(tamanho_palavra * sizeof(char));
            strcpy(dicionario[i], palavra_temp);
        }
    }
    return dicionario;
}

//matriz, linha_n, i, 1, strlen(dicionario[j])
void pinta_palavra(elemento **matriz, int linha, int coluna, int direcao, int tamanho_palavra, int tamanho_linha, int tamanho_coluna)
{
    // printf("\n\nPintando palavra\n");
    switch (direcao)
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
            matriz[linha - i][coluna - i].cor = 8;
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

void procura_palavra_na_matriz(elemento **matriz, int n_colunas, int n_linhas, char *palavra, coordenadas_palavra *coordenadas_palavra)
{
    int matches;
    int achou = 0;
    int tamanho_palavra = strlen(palavra);
    for (int linha = 0; linha < n_linhas; linha++)
    {
        for (int coluna = 0; coluna < n_colunas; coluna++)
        {
            if (matriz[linha][coluna].letra == palavra[0])
            {
                // Horizontal
                matches = 0;
                int linha_de_busca = linha;
                int coluna_de_busca = coluna;
                while (matriz[linha_de_busca][coluna_de_busca].letra == palavra[matches])
                {
                    matches++;
                    if (matches == tamanho_palavra)
                    {
                        achou = 1;
                        coordenadas_palavra->achada = 1;
                        coordenadas_palavra->direcao = 1;
                        coordenadas_palavra->coordenadas[0].linha = linha;
                        coordenadas_palavra->coordenadas[0].coluna = coluna;
                        break;
                    }
                    if (coluna_de_busca + 1 > n_colunas - 1)
                    {
                        coluna_de_busca = 0;
                    }
                    else
                    {
                        coluna_de_busca += 1;
                    }
                }
                if (achou) continue;

                // Horizontal reversa
                matches = 0;
                linha_de_busca = linha;
                coluna_de_busca = coluna;
                while (matriz[linha_de_busca][coluna_de_busca].letra == palavra[matches])
                {
                    matches++;
                    if (matches == tamanho_palavra)
                    {
                        achou = 1;
                        coordenadas_palavra->achada = 1;
                        coordenadas_palavra->direcao = 2;
                        coordenadas_palavra->coordenadas[0].linha = linha;
                        coordenadas_palavra->coordenadas[0].coluna = coluna;
                        break;
                    }
                    if (coluna_de_busca - 1 < 0)
                    {
                        coluna_de_busca = n_colunas - 1;
                    }
                    else
                    {
                        coluna_de_busca -= 1;
                    }
                }
                if (achou) continue;

                // Vertical
                matches = 0;
                linha_de_busca = linha;
                coluna_de_busca = coluna;
                while (matriz[linha_de_busca][coluna_de_busca].letra == palavra[matches])
                {
                    matches++;
                    if (matches == tamanho_palavra)
                    {
                        achou = 1;
                        coordenadas_palavra->achada = 1;
                        coordenadas_palavra->direcao = 3;
                        coordenadas_palavra->coordenadas[0].linha = linha;
                        coordenadas_palavra->coordenadas[0].coluna = coluna;
                        break;
                    }
                    if (linha_de_busca + 1 > n_linhas - 1)
                    {
                        linha_de_busca = 0;
                    }
                    else
                    {
                        linha_de_busca += 1;
                    }
                }
                if (achou) continue;

                // Vertical reversa
                matches = 0;
                linha_de_busca = linha;
                coluna_de_busca = coluna;
                while (matriz[linha_de_busca][coluna_de_busca].letra == palavra[matches])
                {
                    matches++;
                    if (matches == tamanho_palavra)
                    {
                        achou = 1;
                        coordenadas_palavra->achada = 1;
                        coordenadas_palavra->direcao = 4;
                        coordenadas_palavra->coordenadas[0].linha = linha;
                        coordenadas_palavra->coordenadas[0].coluna = coluna;
                        break;
                    }
                    if (linha_de_busca - 1 < 0)
                    {
                        linha_de_busca = n_linhas - 1;
                    }
                    else
                    {
                        linha_de_busca -= 1;
                    }
                }
                if (achou) continue;

                // Diagonal direita baixo
                matches = 0;
                linha_de_busca = linha;
                coluna_de_busca = coluna;
                while (matriz[linha_de_busca][coluna_de_busca].letra == palavra[matches])
                {
                    matches++;
                    if (matches == tamanho_palavra)
                    {
                        achou = 1;
                        coordenadas_palavra->achada = 1;
                        coordenadas_palavra->direcao = 5;
                        coordenadas_palavra->coordenadas[0].linha = linha;
                        coordenadas_palavra->coordenadas[0].coluna = coluna;
                        break;
                    }
                    if (linha_de_busca + 1 > n_linhas - 1 || coluna_de_busca + 1 > n_colunas - 1)
                    {
                        break;
                    }
                    else
                    {
                        linha_de_busca += 1;
                        coluna_de_busca += 1;
                    }
                }
                if (achou) continue;

                // Diagonal direita cima
                matches = 0;
                linha_de_busca = linha;
                coluna_de_busca = coluna;
                while (matriz[linha_de_busca][coluna_de_busca].letra == palavra[matches])
                {
                    matches++;
                    if (matches == tamanho_palavra)
                    {
                        achou = 1;
                        coordenadas_palavra->achada = 1;
                        coordenadas_palavra->direcao = 6;
                        coordenadas_palavra->coordenadas[0].linha = linha;
                        coordenadas_palavra->coordenadas[0].coluna = coluna;
                        break;
                    }
                    if (linha_de_busca - 1 < 0 || coluna_de_busca + 1 > n_colunas - 1)
                    {
                        break;
                    }
                    else
                    {
                        linha_de_busca -= 1;
                        coluna_de_busca += 1;
                    }
                }
                if (achou) continue;

                // Diagonal esquerda baixo
                matches = 0;
                linha_de_busca = linha;
                coluna_de_busca = coluna;
                while (matriz[linha_de_busca][coluna_de_busca].letra == palavra[matches])
                {
                    matches++;
                    if (matches == tamanho_palavra)
                    {
                        achou = 1;
                        coordenadas_palavra->achada = 1;
                        coordenadas_palavra->direcao = 7;
                        coordenadas_palavra->coordenadas[0].linha = linha;
                        coordenadas_palavra->coordenadas[0].coluna = coluna;
                        break;
                    }
                    if (linha_de_busca + 1 > n_linhas - 1 || coluna_de_busca - 1 < 0)
                    {
                        break;
                    }
                    else
                    {
                        linha_de_busca += 1;
                        coluna_de_busca -= 1;
                    }
                }
                if (achou) continue;

                // Diagonal esquerda cima
                matches = 0;
                linha_de_busca = linha;
                coluna_de_busca = coluna;
                while (matriz[linha_de_busca][coluna_de_busca].letra == palavra[matches])
                {
                    matches++;
                    if (matches == tamanho_palavra)
                    {
                        achou = 1;
                        coordenadas_palavra->achada = 1;
                        coordenadas_palavra->direcao = 8;
                        coordenadas_palavra->coordenadas[0].linha = linha;
                        coordenadas_palavra->coordenadas[0].coluna = coluna;
                        break;
                    }
                    if (linha_de_busca - 1 < 0 || coluna_de_busca - 1 < 0)
                    {
                        break;
                    }
                    else
                    {
                        linha_de_busca -= 1;
                        coluna_de_busca -= 1;
                    }
                }
            }
        }
    }
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

    // Inicializa a struct de coordenadas
    coordenadas_palavra *coordenadas_palavras = (coordenadas_palavra *)malloc(n_palavras * sizeof(coordenadas_palavra));
    for (int i = 0; i < n_palavras; i++)
    {
        int tamanho_palavra = strlen(dicionario[i]);
        coordenadas_palavras[i].tamanho = tamanho_palavra;
        coordenadas_palavras[i].achada = 0;
        coordenadas_palavras[i].direcao = 0;
        coordenadas_palavras[i].coordenadas = (coordenadas_caracter *)malloc(tamanho_palavra * sizeof(coordenadas_caracter));
    }

    inicio = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp for schedule(dynamic, 1)
        for (int i = 0; i < n_palavras; i++)
        {
            // printf("Thread %d procurando palavra %s\n", omp_get_thread_num(), dicionario[i]);
            procura_palavra_na_matriz(caca_palavras, n_colunas, n_linhas, dicionario[i], &coordenadas_palavras[i]);
        }
        // printf("Thread %d terminou\n", omp_get_thread_num());
    }
    fim = omp_get_wtime();
    for(int i = 0; i < n_palavras; i++){
        if(coordenadas_palavras[i].achada){
            pinta_palavra(caca_palavras, coordenadas_palavras[i].coordenadas[0].linha, coordenadas_palavras[i].coordenadas[0].coluna, coordenadas_palavras[i].direcao, coordenadas_palavras[i].tamanho, n_linhas, n_colunas);
        }
    }

    print_matriz(caca_palavras, n_linhas, n_colunas);

    // Free em tudo que foi alocado
    for (int i = 0; i < n_palavras; i++)
    {
        free(coordenadas_palavras[i].coordenadas);
        free(dicionario[i]);
    }
    free(coordenadas_palavras);
    free(dicionario);
    fclose(file);
    for(int i = 0; i < n_linhas; i++){
        free(caca_palavras[i]);
    }
    free(caca_palavras);


    printf("\n\nTempo de execucao com %d threads: %.16f\n", n_threads, fim - inicio);
    printf("inicio: %.16f\nfim: %.16f\n", inicio, fim);
    return 0;
}