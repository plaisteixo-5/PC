#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1

#define NE 3 //numero de escritores
#define NL 10 //numero de leitores

pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_wr = PTHREAD_MUTEX_INITIALIZER; // Lock para controlar o fluxo de escritores e leitores. //
// ========================================================================================================== //
// Sempre antes do leitor executar seu processo, ele vai verificar se o lock_wr, que é utilizado também no 	  //
//escritor, está bloqueado ou não. Se estiver, ele vai segurar os leitores até que o escritor termine de      //
// executar seu processo.																					  //
// ========================================================================================================== //

int num_leitores = 0;

void * reader(void *arg);
void * writer(void *arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main() {

	pthread_t r[NL], w[NE];
	int i;
        int *id;
        /* criando leitores */
    	for (i = 0; i < NL ; i++) {
	   id = (int *) malloc(sizeof(int));
           *id = i;
		 pthread_create(&r[i], NULL, reader, (void *) (id));
	}
	 /* criando escritores */
	for (i = 0; i< NE; i++) {
	   id = (int *) malloc(sizeof(int));
           *id = i;
		 pthread_create(&w[i], NULL, writer, (void *) (id));
	}
	pthread_join(r[0],NULL);
	return 0;
}

void * reader(void *arg) {
	int i = *((int *) arg);
	while(TRUE) {

		pthread_mutex_lock(&lock_wr);               // Só continua executando o processo caso nenhum escritor tenha bloqueado o lock.    //
			pthread_mutex_lock(&lock_nl);
			num_leitores++;
			if(num_leitores == 1){
				pthread_mutex_lock(&lock_bd);
			}
			pthread_mutex_unlock(&lock_nl);
		pthread_mutex_unlock(&lock_wr);             // Ao adicionar o leitor, caso o escritor pegue o acesso do lock_wr, ele vai esperar //
													// o leitor atual terminar seu processo e liberar o lock_bd e executará o processo.  //

		read_data_base(i);       					// Acessa o dado.                                                                    //
	
		pthread_mutex_lock(&lock_nl);
		if(num_leitores) num_leitores--;
		if(num_leitores == 0){ 
			pthread_mutex_unlock(&lock_bd);
		}
		pthread_mutex_unlock(&lock_nl);
		
		use_data_read(i);        				    // Região não crítica. 																 //

	}

	pthread_exit(0);
}

void * writer(void *arg) {
	int i = *((int *) arg);
	while(TRUE) {
		think_up_data(i);       					// Região não critica.																//

		pthread_mutex_lock(&lock_wr);				// Fecha o lock_wr para o escritor executar seu processo e espera o leitor liberar  //
													// o lock_bd.   																	//
			pthread_mutex_lock(&lock_bd);
				write_data_base(i);      			// Atualiza os dados. 																//
			pthread_mutex_unlock(&lock_bd);
		pthread_mutex_unlock(&lock_wr);

    }

	pthread_exit(0);
}

void read_data_base(int i) {
	printf("Leitor %d está lendo os dados\n", i);
	sleep(rand() % 40);
}

void use_data_read(int i) {
	printf("Leitor %d está usando os dados lidos! Número de leitores: %d\n", i,num_leitores);
	sleep(rand() % 40);
}

void think_up_data(int i) {
	printf("Escritor %d está pensando no que escrever!\n", i);
	sleep(rand() % 40);
}

void write_data_base(int i) {
	printf("Escritor %d está escrevendo os dados! Número de leitores: %d\n", i,num_leitores);
	sleep( rand() % 40);
}