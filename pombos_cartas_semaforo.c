#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

void *f_usuario(void *arg);
void *f_pombo(void *arg);

sem_t semaforo_usuario;
sem_t semaforo_pombo;
int cont_cartas = CARTAS;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv)
{
    int i;

    sem_init(&semaforo_usuario, 0, CARTAS);
    sem_init(&semaforo_pombo, 0, 0);

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
    int i;

    while (1)
    {
        sem_wait(&semaforo_pombo);
        printf("O pombo foi acordado e está partindo!\n");

        pthread_mutex_lock(&mutex);
        cont_cartas = CARTAS;
        sleep(4);

        printf("PRU PRU PRU!!\n");
        printf("Pombo is back\n");

        pthread_mutex_unlock(&mutex);
        for (i = 0; i < CARTAS; i++)
            sem_post(&semaforo_usuario);
    }
}

void *f_usuario(void *arg)
{

    int id = *((int *)arg);

    while (1)
    {
        sleep(2);

        sem_wait(&semaforo_usuario);
        pthread_mutex_lock(&mutex);

        cont_cartas--;
        printf("O usuário %d escreveu um poema para sua amada <3\nSobrou %d espaços na bolsoa do pombo!\n", id, cont_cartas);
        sleep(2);

        pthread_mutex_unlock(&mutex);
        if (cont_cartas == 0)
        {
            printf("A bolsa está cheia de poemas. Acordando o pombo para espalhar o amor!\n");
            sem_post(&semaforo_pombo);
        }
    }
}
