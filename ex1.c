#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "unistd.h"

#define N 10

int contador = 0;
void * pthread_func(void * arg){
    int id = *((int *) arg);
    printf("Criou um pthread com id = %d\n", id);

    contador++;

    pthread_exit(0);
}

int main(){

    pthread_t a[N];
    printf("Meu id: %lu\n", pthread_self());

    int i;
    int* id;
    for(i = 0 ; i < N ; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&a[i], NULL, pthread_func, (void *) (id));
    }

    for(i = 0 ; i < N ; i++){
        pthread_join(a[i], NULL);
    }

    free(id);

    printf("Terminado contador = %d\n", contador);

    return 0;
}