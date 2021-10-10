/*
 * Problema do barbeiro dorminhoco.
 */
#include "pthread.h"
#include "semaphore.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#define N_CLIENTES 50
#define N_CADEIRAS 5

sem_t sem_cadeiras;

sem_t acorda_barbeiro;
sem_t cortou_cabelo;
pthread_mutex_t cadeira_barbeiro = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cont_clientes = PTHREAD_MUTEX_INITIALIZER;
int cabelos_cortados = 0, clientes_perdidos = 0;

void *f_barbeiro(void *v)
{

	while (1)
	{
		sem_wait(&acorda_barbeiro); // Espera o cliente

		sleep(1); //Cortar o cabelo do cliente
		printf("Barbeiro cortou o cabelo de um cliente\n");

		sem_post(&cortou_cabelo); //...Liberar/desbloquear o cliente
	}
	pthread_exit(0);
}

void *f_cliente(void *v)
{
	int id = *(int *)v;
	sleep(id % 3);
	if (sem_trywait(&sem_cadeiras) == 0)
	{ //conseguiu pegar uma cadeira de espera
		printf("Cliente %d entrou na barbearia \n", id);
		pthread_mutex_lock(&cadeira_barbeiro);	 //... pegar/sentar a cadeira do barbeiro
		sem_post(&sem_cadeiras);				 //... liberar a sua cadeira de espera
		sem_post(&acorda_barbeiro);				 //... acordar o barbeiro para cortar seu cabelo
		sem_wait(&cortou_cabelo);				 //... aguardar o corte do seu cabelo
		cabelos_cortados++;						 // contabiliza o cabelo cortado
		pthread_mutex_unlock(&cadeira_barbeiro); //... liberar a cadeira do barbeiro
		printf("Cliente %d cortou o cabelo e foi embora \n", id);
	}
	else
	{ //barbearia cheia
		pthread_mutex_lock(&cont_clientes);
		clientes_perdidos++; // contabiliza o cabelo não cortado
		pthread_mutex_unlock(&cont_clientes);
		printf("Barbearia cheia, cliente %d indo embora\n", id);
	}

	pthread_exit(0);
}

int main()
{
	pthread_t thr_clientes[N_CLIENTES], thr_barbeiro;
	int i, id[N_CLIENTES];

	sem_init(&sem_cadeiras, 0, N_CADEIRAS);
	sem_init(&acorda_barbeiro, 0, 0);

	for (i = 0; i < N_CLIENTES; i++)
	{
		id[i] = i;
		pthread_create(&thr_clientes[i], NULL, f_cliente, (void *)&id[i]);
	}

	pthread_create(&thr_barbeiro, NULL, f_barbeiro, NULL);

	for (i = 0; i < N_CLIENTES; i++)
		pthread_join(thr_clientes[i], NULL);

	printf("====================================================================\n");
	printf("O barbeiro cortou %d cabelos e perdeu %d clientes\n", cabelos_cortados, clientes_perdidos);
	if (cabelos_cortados < clientes_perdidos)
		printf("O barbeiro dormiu mais do que cortou cabelo!\nO barbeiro morreu de desgosto e por isso o programa foi finalizado\n");
	else
		printf("O barbeiro está começando a parar de ser preguiçoso, cortou mais cabelos do que perdeu clientes!\nO barbeiro ficou tão feliz que morreu de pressão baixa\n");

	/* Barbeiro morreu */

	return 0;
}
