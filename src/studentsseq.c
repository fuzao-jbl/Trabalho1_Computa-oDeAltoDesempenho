// Grupo 09:
// Gabriel Dezejácomo Maruschi - 14571525
// Pedro Fuziwara Filho - 13676840
// Matheus Cavalcanti de Santana - 13217506
// 
// Para compilar, execute no terminal:
// gcc studentsseq.c -o seq -lm
//
// Para executar, execute no terminal:
// ./seq entrada.txt

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//Estrutura para armazenar os resultados
typedef struct {
    float min;
    float max;
    float mediana;
    float media;
    float desvio_padrao;
} Resultados;

//Funcao de comparacao para o qsort
int cmp_float(const void *a, const void *b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

//Funcao que calcula as estatisticas de um conjunto de notas
Resultados calcula_estatisticas(float *notas, int n) {
    Resultados est;
    float soma = 0.0f;
    
    // Aloca vetor temporario para nao modificar o array original durante a ordenacao
    float *temp = (float *)malloc(n * sizeof(float));
    
    est.min = notas[0];
    est.max = notas[0];
    
    for (int i = 0; i < n; i++) {
        temp[i] = notas[i];
        soma += notas[i];
        if (notas[i] < est.min) est.min = notas[i];
        if (notas[i] > est.max) est.max = notas[i];
    }
    
    // Media
    est.media = soma / n;
    
    // Desvio padrao
    float soma_variancia = 0.0f;
    for (int i = 0; i < n; i++) {
        soma_variancia += (notas[i] - est.media) * (notas[i] - est.media);
    }
    est.desvio_padrao = sqrt(soma_variancia / n);
    
    // Mediana
    qsort(temp, n, sizeof(float), cmp_float);
    if (n % 2 == 0) {
        est.mediana = (temp[n / 2 - 1] + temp[n / 2]) / 2.0f;
    } else {
        est.mediana = temp[n / 2];
    }
    
    free(temp);
    return est;
}

float **gera_tabela(int R, int C, int A, int N, int seed)
{
    srand(seed);
    int total_alunos = R*C*A;

    // a tabela vai ser uma matriz, que é um ponteiro de ponteiro
    float **tabela = (float **)malloc(total_alunos*sizeof(float *));

    for (int aluno = 0; aluno < total_alunos; aluno++)
    {
        tabela[aluno] = (float *)malloc(N*sizeof(float));
        for (int parcial = 0; parcial < N; parcial++)
           tabela[aluno][parcial] = (rand()%1001)/10.0;
    }

    return tabela;
}

// libera a memória alocada pela tabela
void destroi_tabela(float **tabela, int R, int C, int A, int N)
{
    int total_alunos = R*C*A;
    for (int aluno = 0; aluno < total_alunos; aluno++)
        free(tabela[aluno]);
    free(tabela);
}

void printa_tabela(float **tabela, int R, int C, int A, int N, int seed)
{
    printf("TABELA DE NOTAS\n\n");
    printf("R: %d\n", R);
    printf("C: %d\n", C);
    printf("A: %d\n", A);
    printf("N: %d\n", N);
    printf("seed: %d\n", seed);

    int linha = 0;
    for (int r = 0; r < R; r++)
        for (int c = 0; c < C; c++)
            for (int a = 0; a < A; a++)
            {
                printf("R: %d ; C: %d ; A: %d |", r, c, a);
                for (int n = 0; n < N; n++)
                    printf(" %3.1f |", tabela[linha][n]);
                printf("\n");
                linha++;
            }
}

float *calcula_medias(float **tabela, int R, int C, int A, int N)
{
    int total_alunos = R*C*A;
    float soma;

    float *medias = (float *)malloc(total_alunos*sizeof(float));

    for (int aluno = 0; aluno < total_alunos; aluno++)
    {
        soma = 0.0f;
        for (int parcial = 0; parcial < N; parcial++)
            soma += tabela[aluno][parcial];
        medias[aluno] = soma/N;
    }

    return medias;
}

/*
*   FUNÇÃO DE DEBUG, RETIRAR NA VERSÃO FINAL
*/
void printa_medias(float *medias, int R, int C, int A)
{
    int aluno = 0;
    printf("MÉDIAS\n\n");
    for (int r = 0; r < R; r++)
        for (int c = 0; c < C; c++)
            for (int a = 0; a < A; a++)
            {
                printf("R: %d ; C: %d ; A: %d | %3.1f\n", r, c, a, medias[aluno]);
                aluno++;
            }
}

void imprime_resultados(Resultados *est_cidades, Resultados *est_regioes, Resultados est_brasil, 
                        int R, int C, 
                        int id_melhor_regiao, float melhor_media_regiao, 
                        int regiao_melhor_cidade, int id_melhor_cidade, float melhor_media_cidade, 
                        double tempo) {
    
    printf("Cidades\t\tMin Nota\tMax Nota\tMediana\t\tMédia\t\tDsvPdr\n");
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            Resultados est = est_cidades[r * C + c];
            printf("R=%d, C=%d\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\n", 
                   r, c, est.min, est.max, est.mediana, est.media, est.desvio_padrao);
        }
    }
    printf("\n");

    printf("Regiões\t\tMin Nota\tMax Nota\tMediana\t\tMédia\t\tDsvPdr\n");
    for (int r = 0; r < R; r++) {
        Resultados est = est_regioes[r];
        printf("R=%d\t\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\n", 
               r, est.min, est.max, est.mediana, est.media, est.desvio_padrao);
    }
    printf("\n");

    printf("Brasil\t\tMin Nota\tMax Nota\tMediana\t\tMédia\t\tDsvPdr\n");
    printf("\t\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\n\n", 
           est_brasil.min, est_brasil.max, est_brasil.mediana, est_brasil.media, est_brasil.desvio_padrao);

    printf("Premiação\tReg/Cid\t\tMedia Arit\n");
    printf("Melhor região:\tR%d\t\t%.1f\n", id_melhor_regiao, melhor_media_regiao);
    printf("Melhor cidade:\tR%d-C%d\t\t%.1f\n\n", regiao_melhor_cidade, id_melhor_cidade, melhor_media_cidade);

    printf("Tempo de resposta em segundos, sem considerar E/S: %.9fs\n", tempo);
}

int main(int argc, char *argv[])
{
    int R, C, A, N, T, seed;
    struct timespec inicio, fim;

    if (argc != 2) {
        printf("Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    FILE *arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    //Leitura de entradas
    fscanf(arquivo, "%d", &R);
    fscanf(arquivo, "%d", &C);
    fscanf(arquivo, "%d", &A);
    fscanf(arquivo, "%d", &N);
    fscanf(arquivo, "%d", &T); //T lido, mas não utilizado na versão sequencial do programa
    fscanf(arquivo, "%d", &seed);
    fclose(arquivo);

    // gera tabela
    float **tabela = gera_tabela(R, C, A, N, seed);
    printa_tabela(tabela, R, C, A , N, seed);
    
    // fazemos a computação
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    // Médias de todos os alunos
    float *medias = calcula_medias(tabela, R, C, A, N);

    // Variaveis para premiacao
    float melhor_media_cidade = -1.0f;
    int regiao_melhor_cidade = -1;
    int id_melhor_cidade = -1;

    float melhor_media_regiao = -1.0f;
    int id_melhor_regiao = -1;

    // Alocamos arrays para armazenar os resultados, garantindo que o calculo
    // ocorra dentro do monitoramento de tempo sem printar no terminal ainda.
    Resultados *est_cidades = malloc(R * C * sizeof(Resultados));
    Resultados *est_regioes = malloc(R * sizeof(Resultados));
    
    // Passo 2: Calcular estatisticas por Cidade
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            int offset = (r * C + c) * A;
            Resultados est = calcula_estatisticas(&medias[offset], A);
            est_cidades[r * C + c] = est;
            
            if (est.media > melhor_media_cidade) {
                melhor_media_cidade = est.media;
                regiao_melhor_cidade = r;
                id_melhor_cidade = c;
            }
        }
    }

    // Passo 3: Calcular estatisticas por Regiao
    for (int r = 0; r < R; r++) {
        int offset = r * C * A;
        Resultados est = calcula_estatisticas(&medias[offset], C * A);
        est_regioes[r] = est;
        
        if (est.media > melhor_media_regiao) {
            melhor_media_regiao = est.media;
            id_melhor_regiao = r;
        }
    }

    // Passo 4: Calcular estatisticas do Brasil
    Resultados est_brasil = calcula_estatisticas(medias, R * C * A);

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec)/1e9;

    // --- IMPRESSOES FORA DA CONTAGEM DE TEMPO ---
    imprime_resultados(est_cidades, est_regioes, est_brasil, R, C, 
                       id_melhor_regiao, melhor_media_regiao, 
                       regiao_melhor_cidade, id_melhor_cidade, melhor_media_cidade, 
                       tempo);

    //Liberação de matrizes e arrays
    destroi_tabela(tabela, R, C, A, N);
    free(medias);
    free(est_cidades);
    free(est_regioes);

    return 0;
}

