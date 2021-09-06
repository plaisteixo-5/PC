#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

void *canibal(void *meuid);
void *cozinheiro(int m);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_cozinheiro = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_canibal = PTHREAD_COND_INITIALIZER;

int porcoes = 0; // Variável para contar o número de porções diponíveis

void main(argc, argv) int argc;
char *argv[];
{
    int erro;
    int i, n, m;
    int *id;

    pthread_t tid[MAXCANIBAIS];

    if (argc != 3)
    {
        printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
        exit(1);
    }

    n = atoi(argv[1]); //número de canibais
    m = atoi(argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
    printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

    if (n > MAXCANIBAIS)
    {
        printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
        exit(1);
    }

    for (i = 0; i < n; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&tid[i], NULL, canibal, (void *)(id));

        if (erro)
        {
            printf("erro na criacao do thread %d\n", i);
            exit(1);
        }
    }

    cozinheiro(m);
}

void *canibal(void *pi)
{

    while (1)
    {
        //pegar uma porção de comida e acordar o cozinheiro se as porções acabaram
        pthread_mutex_lock(&mutex); // Região crítica
        while (porcoes == 0)
            pthread_cond_wait(&cond_canibal, &mutex); // Se as porções já tiverem acabado, coloca o canibal para esperar
        porcoes--;

        if (porcoes == 0) // Se as porções acabarem, acorda o cozinheiro
        {
            pthread_cond_signal(&cond_cozinheiro);
        }
        pthread_mutex_unlock(&mutex); // Região crítica

        printf("Canibal %d: vou comer a porcao que peguei\nSobraram %d pedaços\n", *(int *)(pi), porcoes);
        sleep(1);
    }
}

void *cozinheiro(int m)
{

    while (1)
    {
        pthread_mutex_lock(&mutex); // Região crítica
        while (porcoes != 0)        // Enquanto ainda tiver porção, o cozinheiro dorme
            pthread_cond_wait(&cond_cozinheiro, &mutex);
        printf("Cozinheiro: A comida acabou, vou cozinhar\n");
        sleep(1);
        porcoes += m;                          // Soma as porções preparadas à variável
        pthread_cond_broadcast(&cond_canibal); // Acorda todos os canibais
        pthread_mutex_unlock(&mutex);          // Região crítica
    }
}
