#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10      //número de usuários
#define CARTAS 20 //quantidade de cartas na mochila

void *f_usuario(void *arg);
void *f_pombo(void *arg);

int cont_cartas = CARTAS;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_usuario = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_pombo = PTHREAD_COND_INITIALIZER;

int main(int argc, char **argv)
{
    int i;

    pthread_t usuario[N];
    int *id;
    for (i = 0; i < N; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&(usuario[i]), NULL, f_usuario, (void *)(id));
    }

    pthread_t pombo;
    id = (int *)malloc(sizeof(int));
    *id = 0;
    pthread_create(&(pombo), NULL, f_pombo, (void *)(id));

    pthread_join(pombo, NULL);
}

void *f_pombo(void *arg)
{

    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (cont_cartas != 0)
        {
            pthread_cond_wait(&cond_pombo, &mutex);
        }
        printf("O pombo acordou e foi entregar as cartas\n");

        sleep(2);

        printf("PRU PRU PRU!!\n");
        printf("O pombo voltou!!\n");
        cont_cartas = CARTAS;
        pthread_cond_broadcast(&cond_usuario);
        pthread_mutex_unlock(&mutex);
        //Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20 cartas)
        //Leva as cartas para B e volta para A
        //Acordar os usuários
    }
}

void *f_usuario(void *arg)
{
    int id = *((int *)arg);

    sleep(3);

    while (1)
    {
        pthread_mutex_lock(&mutex);
        printf("O usuário %d quer enviar uma carta\n", id);
        while (cont_cartas == 0)
        {
            printf("O usuário %d está esperando o pombo voltar\n", id);
            pthread_cond_wait(&cond_usuario, &mutex);
        }
        cont_cartas--;
        printf("Quantidade de vagas na bolsa: %d\n", cont_cartas);
        pthread_mutex_unlock(&mutex);

        printf("O usuário %d escreveu uma carta e deu ao pombo!\n", id);
        sleep(1);

        pthread_mutex_lock(&mutex);
        if (cont_cartas == 0)
        {
            printf("A bolsa do pombo está cheia. Acordando ele para trabalhar!\n");
            pthread_cond_broadcast(&cond_pombo);
        }
        pthread_mutex_unlock(&mutex);

        //Escreve uma carta
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme
        //Posta sua carta na mochila do pombo
        //Caso a mochila fique cheia, acorda o ṕombo
    }
}
