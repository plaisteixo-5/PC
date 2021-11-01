#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id)
#define DIR(id) (id + 1) % N
#define FAMINTO 1
#define PENSANDO 2
#define COMENDO 3

sem_t s[N];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int filosofo_estado[N];

void *filosofos(void *arg);

void pega_talher(int n);
void devolve_talher(int n);

int main()
{
	int i;
	int *id;
	//semaforo
	for (i = 0; i < N; i++)
	{
		sem_init(&s[i], 0, 1);
	}
	pthread_t r[N];

	//criacao das threads de filosofos
	for (i = 0; i < N; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&r[i], NULL, filosofos, (void *)(id));
	}

	pthread_join(r[0], NULL);
	return 0;
}

void *filosofos(void *arg)
{
	int n = *((int *)arg);
	while (1)
	{
		//pensar
		printf("Filosofo %d pensando ...\n", n);
		sleep(3);
		pega_talher(n);
		//comer
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(3);
		printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n);
	}
}

void pega_talher(int n)
{
	pthread_mutex_lock(&mutex);
	filosofo_estado[n] = FAMINTO;

	if (filosofo_estado[ESQ(N)] != COMENDO && filosofo_estado[DIR(n)] != COMENDO)
	{
		filosofo_estado[n] = COMENDO;
		sem_post(&s[n]);
	}
	pthread_mutex_unlock(&mutex);
	sem_wait(&s[n]);
}

void devolve_talher(int n)
{
	pthread_mutex_lock(&mutex);
	filosofo_estado[n] = PENSANDO;

	if (filosofo_estado[ESQ(ESQ(n))] != COMENDO && filosofo_estado[ESQ(n)] == FAMINTO)
	{
		filosofo_estado[ESQ(n)] = COMENDO;
		sem_post(&s[ESQ(n)]);
	}

	if (filosofo_estado[DIR(DIR(n)) != COMENDO] && filosofo_estado[DIR(n)] == FAMINTO)
	{
		filosofo_estado[DIR(n)] = COMENDO;
		sem_post(&s[DIR(n)]);
	}

	pthread_mutex_unlock(&mutex);
}
