#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define N 100
#define x 10000000000

long long int contador = 0;
pthread_mutex_t lock_contador = PTHREAD_MUTEX_INITIALIZER;  

void* incrementar(void* arg){
    int id = *((int *) arg);
    printf("Criou uma pthread com id = %d\n", id);

    int i;
        pthread_mutex_lock(&lock_contador);
    for(i = 0 ; i < x ; i++){
        contador++;
    }
        pthread_mutex_unlock(&lock_contador);
}

int main(){
    // Podemos inicializar o locker como está ali em cima ou podemos usar:
    // pthread_mutex_init(&lock_contador);


    pthread_t a[N];
    int i;
    int* id;
    
    for(i = 0 ; i < N ; i++){
        id = (int*) malloc(sizeof(int));
        *id = i;
        pthread_create(&a[i], NULL, incrementar, (void*) (id));
    }

    for(i = 0 ; i < N ; i++) {
        pthread_join(a[i], NULL);
    }

    printf("Valor final do contador: %lld\n", contador);
}