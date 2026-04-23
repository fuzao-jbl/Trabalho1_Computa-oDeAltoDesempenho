// Grupo 09:
// Gabriel Dezejácomo Maruschi - 14571525
// Pedro Fuziwara Filho - 13676840
// Matheus Cavalcanti de Santana - 13217506
// 
// Para compilar, execute no terminal:
// gcc -O3 -fopenmp studentpar-alt.c -o par
//
// Para executar, execute no terminal:
// ./alt entrada.txt

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

void minMedMax(Resultados *res, float *notas, int n)
{
    // Aloca vetor temporario para nao modificar o array original durante a ordenacao
    float *temp = (float *)malloc(n * sizeof(float));

    for (int i = 0; i < n; i++)
        temp[i] = notas[i];

    qsort(temp, n, sizeof(float), cmp_float);

    res->min = temp[0];
    res->max = temp[n-1];
    
    if(n%2 == 0) res->mediana = (temp[n / 2 - 1] + temp[n / 2]) / 2.0f;
    else res->mediana = temp[(n-1)/2];
    free(temp);
}

void media_global(Resultados *res, float *notas, int n)
{
    float soma = 0.0f;

    for (int i = 0; i < n; i++)
        soma += notas[i];
    
    res->media = soma / n;
}

void desvio_padrao(Resultados *res, float *notas, int n)
{
    float soma_variancia = 0.0f;

    for(int i = 0; i < n; i++)
        soma_variancia += (notas[i] - res->media) * (notas[i] - res->media);
    res->desvio_padrao = sqrt(soma_variancia / n);
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

int main(int argc, char *argv[]) {
    int R, C, A, N, T, seed, offset1, offset2, i;
    int aluno, parcial, total_alunos;
    float soma;
    double tempo;
    struct timespec inicio, fim;

    // Definicao de posicoes de memoria
    float **tabela, *medias;
    Resultados *est_regioes, *est_cidades, est_brasil;

    // Variaveis para premiacao
    float melhor_media_cidade_gl = -1.0f;
    int regiao_melhor_cidade_gl = -1;
    int id_melhor_cidade_gl = -1;

    float melhor_media_regiao_gl = -1.0f;
    int id_melhor_regiao_gl = -1;

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
    fscanf(arquivo, "%d", &T);
    fscanf(arquivo, "%d", &seed);
    fclose(arquivo);

    // Alocacao de memoria para os resultados das cidades e regioes
    est_cidades = malloc(R * C * sizeof(Resultados));
    est_regioes = malloc(R * sizeof(Resultados));

    total_alunos = R*C*A;

    // gera tabelas
    tabela = gera_tabela(R, C, A, N, seed);
    medias = (float *)malloc(total_alunos*sizeof(float));

    // Inicio da medida do tempo
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    #pragma omp parallel num_threads(T)
    {
        // Calculo das medias
        // Barreira implicita no for
        #pragma omp for private(aluno, parcial, soma) 
        for (aluno = 0; aluno < total_alunos; aluno++)
        {
            soma = 0.0f;
            #pragma omp simd reduction(+:soma)
            for (parcial = 0; parcial < N; parcial++)
                soma += tabela[aluno][parcial];
            medias[aluno] = soma/N;
        }

        
        // Calculo das estatisticas por cidade.
        #pragma omp for 
        for(int r = 0; r < R; r++) {
            for(int c = 0; c < C; c++) {
                offset1 = (r * C + c) * A;

                #pragma omp task firstprivate(offset1)
                minMedMax(&(est_cidades[r * C + c]), &medias[offset1], A);

                #pragma omp task firstprivate(offset1) depend(out: est_cidades[r * C + c].media)
                media_global(&(est_cidades[r * C + c]), &medias[offset1], A);

                #pragma omp task firstprivate(offset1) depend(in: est_cidades[r * C + c].media)
                desvio_padrao(&(est_cidades[r * C + c]), &medias[offset1], A);
            }
        }

        #pragma omp single
        {
            // Calculo das estatisticas por regiao
            for(int r = 0; r < R; r++) {
                offset2 = r * C * A;

                #pragma omp task firstprivate(offset2)
                minMedMax(&(est_regioes[r]), &medias[offset2], C * A);

                #pragma omp task firstprivate(offset2) depend(out: est_regioes[r].media)
                media_global(&(est_regioes[r]), &medias[offset2], C * A);

                #pragma omp task firstprivate(offset2) depend(in: est_regioes[r].media)
                desvio_padrao(&(est_regioes[r]), &medias[offset2], C * A);
            }

            // Calculo da estatistica para o Brasil
            #pragma omp task
            {
            minMedMax(&est_brasil, medias, R * C * A);
            media_global(&est_brasil, medias, R * C * A);
            desvio_padrao(&est_brasil, medias, R * C * A);
            }
        }
        // Aguarda a finalizacao das tasks para calculo das premiacoes
        #pragma omp taskwait

        // Calculo das premiacoes para cidade
        float melhor_cidade_lc = -1.0f;
        int regiao_melhor_cidade_lc = -1;
        int id_cidade_lc = -1;

        #pragma omp for
        for(i = 0; i < R*C; i++) {
            if(est_cidades[i].media > melhor_cidade_lc) {
                melhor_cidade_lc = est_cidades[i].media;
                regiao_melhor_cidade_lc = i/C;
                id_cidade_lc = i%C;
            }
        }

        #pragma omp critical
        {
            if(melhor_cidade_lc >  melhor_media_cidade_gl) {
                melhor_media_cidade_gl = melhor_cidade_lc;
                regiao_melhor_cidade_gl = regiao_melhor_cidade_lc;
                id_melhor_cidade_gl = id_cidade_lc;
            }
        }

        // Calculo das premiacoes para regiao
        float melhor_regiao_lc = -1.0f;
        int id_regiao_lc = -1;

        #pragma omp for
        for(i = 0; i < R; i++) {
            if(est_regioes[i].media > melhor_regiao_lc) {
                melhor_regiao_lc = est_regioes[i].media;
                id_regiao_lc = i;
            }
        }

        #pragma omp critical
        {
            if(melhor_regiao_lc > melhor_media_regiao_gl) {
                melhor_media_regiao_gl = melhor_regiao_lc;
                id_melhor_regiao_gl = id_regiao_lc;
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec)/1e9;

    // --- IMPRESSOES FORA DA CONTAGEM DE TEMPO ---
    imprime_resultados(est_cidades, est_regioes, est_brasil, R, C, 
                       id_melhor_regiao_gl, melhor_media_regiao_gl, 
                       regiao_melhor_cidade_gl, id_melhor_cidade_gl, melhor_media_cidade_gl, 
                       tempo);

    //Liberação de matrizes e arrays
    destroi_tabela(tabela, R, C, A, N);
    free(medias);
    free(est_cidades);
    free(est_regioes);

    return 0;
}