// Grupo 09:
// Gabriel Dezejácomo Maruschi - 14571525
// Pedro Fuziwara Filho - 13676840
// Matheus Cavalcanti de Santana - 13217506
// 
// Para compilar, execute no terminal:
// gcc -O3 -fopenmp studentpar-alt.c -o par -lm
//
// Para executar, execute no terminal:
// ./par entrada.txt

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

// Função para quickselect
int partition(float *arr, int left, int right) {
    float pivot = arr[right];
    int i = left;

    for (int j = left; j < right; j++) {
        if (arr[j] < pivot) {
            float tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
        }
    }

    float tmp = arr[i];
    arr[i] = arr[right];
    arr[right] = tmp;

    return i;
}

// Quickselect
float quickselect(float *arr, int left, int right, int k) {
    while (1) {
        int pivot_index = partition(arr, left, right);

        if (pivot_index == k)
            return arr[k];
        else if (pivot_index > k)
            right = pivot_index - 1;
        else
            left = pivot_index + 1;
    }
}

void minMedMax(Resultados *res, float *notas, int n, float *temp)
{
    // min e max em O(n)
    float min = notas[0], max = notas[0];

    for (int i = 1; i < n; i++) {
        if (notas[i] < min) min = notas[i];
        if (notas[i] > max) max = notas[i];
    }

    res->min = min;
    res->max = max;

    for (int i = 0; i < n; i++)
        temp[i] = notas[i];

    // mediana
    if (n % 2 == 1) {
        res->mediana = quickselect(temp, 0, n-1, n/2);
    } else {
        float m1 = quickselect(temp, 0, n-1, n/2 - 1);
        float m2 = quickselect(temp, 0, n-1, n/2);
        res->mediana = (m1 + m2) / 2.0f;
    }
}

void media_global(Resultados *res, float *notas, int n)
{
    float soma = 0.0f;

    #pragma omp simd reduction(+:soma)
    for (int i = 0; i < n; i++)
        soma += notas[i];
    
    res->media = soma / n;
}

void desvio_padrao(Resultados *res, float *notas, int n)
{
    float soma_variancia = 0.0f;
    float media = res->media;
    
    #pragma omp simd reduction(+:soma_variancia)
    for(int i = 0; i < n; i++)
        soma_variancia += (notas[i] - media) * (notas[i] - media);
    res->desvio_padrao = sqrtf(soma_variancia / n);
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
        float *temp1 = (float *)malloc(A * sizeof(float));
        #pragma omp for nowait
        for(int i = 0; i < R*C; i++) {
            offset1 = i * A;

            minMedMax(&(est_cidades[i]), &medias[offset1], A, temp1);
            media_global(&(est_cidades[i]), &medias[offset1], A);
            desvio_padrao(&(est_cidades[i]), &medias[offset1], A);
        }
        free(temp1);

        // Calculo das estatisticas por regiao,
        float *temp2 = (float *)malloc(C * A * sizeof(float));
        #pragma omp for nowait
        for(int r = 0; r < R; r++) {
            offset2 = r * C * A;

            minMedMax(&(est_regioes[r]), &medias[offset2], C * A, temp2);
            media_global(&(est_regioes[r]), &medias[offset2], C * A);
            desvio_padrao(&(est_regioes[r]), &medias[offset2], C * A);
        }
        free(temp2);

        // Calculo da estatistica para o Brasil
        #pragma omp single
        {
            float *temp3 = (float *)malloc(R * C * A * sizeof(float));
            minMedMax(&est_brasil, medias, R * C * A, temp3);
            media_global(&est_brasil, medias, R * C * A);
            desvio_padrao(&est_brasil, medias, R * C * A);
            free(temp3);
        }

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