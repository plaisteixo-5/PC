#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id)
#define DIR(id)	(id+1)%N


sem_t s[N];

int fome[N];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#define FAMINTO 1
#define COMENDO 2
#define	PENSANDO 0

void *filosofos (void *arg);

void pega_talher (int n);
void devolve_talher (int n);

int main () {
	int i;
	int *id;
	//semaforo 
	for (i=0; i<N; i++) {
		sem_init(&s[i], 0, 0);
	}
	pthread_t r[N]; 

	//criacao das threads de filosofos
       for (i = 0; i < N ; i++) {
	     id = (int *) malloc(sizeof(int));
	     *id = i;
	     pthread_create(&r[i], NULL, filosofos, (void*) (id));
	}

        pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {
	int n = *((int *) arg);
	while(1) {
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

void pega_talher (int n) {
	pthread_mutex_lock(&lock);
		fome[n] = FAMINTO;
		if(fome[ESQ(n)] != COMENDO && fome[DIR(n)] != COMENDO){
				fome[n] = COMENDO;
				sem_post(&s[n]);

			//printf("\nFilosofo %d pegou o talher\n",n);
		}
	pthread_mutex_unlock(&lock);
	sem_wait(&s[n]);

}

void devolve_talher (int n) {
    pthread_mutex_lock(&lock);
	fome[n] = PENSANDO;

	//deixa o da esquerda comer
	if(fome[ESQ(n)]=FAMINTO){
		if (fome[ESQ(ESQ(n))] != COMENDO && fome[DIR(ESQ(n))] != COMENDO){
			fome[ESQ(n)] = COMENDO;
			sem_post(&s[ESQ(n)]);
		}
	}

	//deixa o da direita comer
	if(fome[DIR(n)]=FAMINTO){
		if(fome[ESQ(DIR(n))] != COMENDO && fome[DIR(DIR(n))] != COMENDO){
			fome[DIR(n)] = COMENDO;
			sem_post(&s[DIR(n)]);
		}
	}
	pthread_mutex_unlock(&lock);
}

