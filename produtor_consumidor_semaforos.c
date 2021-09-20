#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 10 //número de produtores
#define CN 5  // número de consumidores
#define N 5	  //tamanho do buffer

void *produtor(void *meuid);
void *consumidor(void *meuid);

pthread_t thread_consumidor[CN];
pthread_t thread_produtor[CN];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t livres;
sem_t ocupadas;

int buffer[N];
int posicao_cons = 0;
int posicao_pro = 0;
int value = 0;

void main(argc, argv) int argc;
char *argv[];
{

	int erro;
	int i, n, m;
	int *id;
	sem_init(&livres, 0, N);
	sem_init(&ocupadas, 0, 0);

	pthread_t tid[PR];

	for (i = 0; i < PR; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		erro = pthread_create(&tid[i], NULL, produtor, (void *)(id));

		if (erro)
		{
			printf("erro na criacao do thread %d\n", i);
			exit(1);
		}
	}

	pthread_t tCid[CN];

	for (i = 0; i < CN; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		erro = pthread_create(&tCid[i], NULL, consumidor, (void *)(id));

		if (erro)
		{
			printf("erro na criacao do thread %d\n", i);
			exit(1);
		}
	}

	pthread_join(tid[0], NULL);
}

void *produtor(void *pi)
{
	while (1)
	{
		sleep(5);
		sem_wait(&livres);
		pthread_mutex_lock(&mutex);
		value++;
		buffer[posicao_pro] = value;
		printf("Produzindo %d na posição %d\n", value, posicao_pro);
		posicao_pro = (posicao_pro == N) ? 0 : posicao_pro + 1;
		pthread_mutex_unlock(&mutex);
		sem_post(&ocupadas);
	}
	pthread_exit(0);
}

void *consumidor(void *pi)
{
	while (1)
	{
		sleep(5);
		sem_wait(&ocupadas);
		pthread_mutex_lock(&mutex);
		value++;
		buffer[posicao_cons] = value;
		printf("Consumindo %d na posição %d\n", value, posicao_cons);
		posicao_cons = (posicao_cons == N) ? 0 : posicao_cons + 1;
		pthread_mutex_unlock(&mutex);
		sem_post(&livres);
	}
	pthread_exit(0);
}
