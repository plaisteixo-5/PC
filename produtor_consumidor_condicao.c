#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 1 // número de produtores
#define CN 4 // número de consumidores
#define N 5	 // tamanho do buffer

void *produtor(void *meuid);
void *consumidor(void *meuid);

int quant_consumidor = 0, quant_produtor = 0;
int request_produtor = 0, buffer = N;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t produtor_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumidor_cond = PTHREAD_COND_INITIALIZER;

void main(argc, argv) int argc;
char *argv[];
{

	int erro;
	int i, n, m;
	int *id;

	pthread_t tid[PR];

	for (i = 0; i < PR; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		erro = pthread_create(&tid[i], NULL, produtor, (void *)(id));

		if (erro)
		{
			printf("erro na criacao do thread %d\n", i + 1);
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
			printf("erro na criacao do thread %d\n", i + 1);
			exit(1);
		}
	}

	pthread_join(tid[0], NULL);
}

void *produtor(void *pi)
{
	int i = *((int *)pi);

	while (1)
	{
		sleep(rand() % (i + 1));
		printf("O produtor %d entrou\n", i + 1);

		pthread_mutex_lock(&mutex); // Fecha a região crítica
		request_produtor++;			// Soma quantidade de produtores pedindo para produzir
		printf("O produtor %d requisitou\n", i + 1);
		// while (quant_consumidor > 0 || buffer == 5)
		while (buffer == 5) // Repetição para verificar se o buffer já não está cheio
		{
			printf("O produtor %d tá esperando\n", i + 1);
			pthread_cond_wait(&produtor_cond, &mutex); // Bota o produtor pra dormir
		}
		request_produtor--;			  // Diminui a quantidade de produtores fazendo request
		quant_produtor++;			  // Aumenta a quantidade de produtores realizando a tarefa
		pthread_mutex_unlock(&mutex); // Abre da região critica

		printf("O produtor %d tá produzindo!\n", i + 1);
		sleep(1);

		pthread_mutex_lock(&mutex); // Fecha a região crítica
		quant_produtor--;			// Remove o produtor após ele ter produzido
		buffer++;					// Adiciona o que o produtor produziu ao buffer
		printf("O produtor %i produziu e agora o tamanho do buffer é %d\n", i + 1, buffer);
		pthread_cond_broadcast(&consumidor_cond); // Acorda os consumidores
		pthread_mutex_unlock(&mutex);			  // Fecha a região crítica
	}
	pthread_exit(0);
}

void *consumidor(void *pi)
{
	int i = *((int *)pi);

	while (1)
	{
		sleep(rand() % (i + 1));
		printf("O consumidor %d entrou\n", i + 1);

		pthread_mutex_lock(&mutex); // Fecha a região crítica

		while (buffer <= 0) // Bota o consumidor para dormir sempre que o buffer ficar vazio
		{
			printf("O consumidor %d tá esperando\n", i + 1);
			pthread_cond_wait(&consumidor_cond, &mutex);
		}
		quant_consumidor++;			  // Adiciona o consumidor aos que estão consumindo
		buffer--;					  // Remove o que foi consumido do buffer
		pthread_mutex_unlock(&mutex); // Fecha a região crítica

		printf("O consumidor %d tá tentando consumir\n", i + 1);
		sleep(1);

		pthread_mutex_lock(&mutex); // Fecha a região crítica
		quant_consumidor--;			// Remove o consumidor
		printf("O consumidor %d terminou de consumir e agora o tamanho do buffer é %d\n", i + 1, buffer);
		if (quant_consumidor == 0)
			pthread_cond_broadcast(&produtor_cond);
		pthread_mutex_unlock(&mutex); // Fecha a região crítica
	}
	pthread_exit(0);
}
