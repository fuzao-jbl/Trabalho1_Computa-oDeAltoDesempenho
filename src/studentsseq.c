// esse trabalho foi feito por:
// Pedro Fuziwara Filho - 13676840
// Coloquem aí os nomes de vocês
//
// Para compilar esse programa, rode:
//
// gcc studentsseq.c -o seq
// ./seq

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void destroi_medias(float *medias)
{
    free(medias);
}

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

int main()
{
    int R, C, A, N, seed;
    struct timespec inicio, fim;

    printf("R: ");
    scanf("%d", &R);
    printf("C: ");
    scanf("%d", &C);
    printf("A: ");
    scanf("%d", &A);
    printf("N: ");
    scanf("%d", &N);
    printf("seed: ");
    scanf("%d", &seed);

    // gera tabela
    float **tabela = gera_tabela(R, C, A, N, seed);
    
    // fazemos a computação
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    float *medias = calcula_medias(tabela, R, C, A, N);
    // temos mais computação para fazer aqui dentro
    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec)/1e9;
    printf("tempo sequencial: %.9f segundos\n", tempo);

    // vai fazer várias coisas com a tabela e depois vai liberar ela
    destroi_tabela(tabela, R, C, A, N);
    destroi_medias(medias);
}

