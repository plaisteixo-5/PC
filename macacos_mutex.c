#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_travessia = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_conta_macaco = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_B = PTHREAD_MUTEX_INITIALIZER;
// ========================================================================================================== //
// A lógica do programa atual foi baseada no Estudo Dirigido anterior. É utilizado um lock para quando os 	  //
// os macacos de uma árvore estão realizando a travessia chamado lock_travessia. Enquanto é realizada a       //
// a travessia, é utilizado um lock chamado lock_A/lock_B para garantir que aS variáveis                      //
// num_macacos_AB/num_macacosBA consigam ser manipuladas de maneira exclusiva. O lock lock_conta_macaco serve //
// para garantir a exclusividade dos processos dos gorilas.                                                   //
// ========================================================================================================== //


int num_macacos_AB = 0, num_macacos_BA = 0;

void * macacoAB(void * a) {
  int i = *((int *) a);    
  while(1){
    //Procedimentos para acessar a corda
      pthread_mutex_lock(&lock_conta_macaco);
        pthread_mutex_lock(&lock_A);
          num_macacos_AB++;
          
          if(num_macacos_AB == 1){
            pthread_mutex_lock(&lock_travessia);
          }
        pthread_mutex_unlock(&lock_A);
      pthread_mutex_unlock(&lock_conta_macaco);
    
    printf("Macaco %d passado de A para B \n",i);
    sleep(1);

    pthread_mutex_lock(&lock_A);
      num_macacos_AB--;
      if(num_macacos_AB == 0) {
        pthread_mutex_unlock(&lock_travessia);
      }
    pthread_mutex_unlock(&lock_A);
    //Procedimentos para quando sair da corda
  }
  pthread_exit(0);
}

void * macacoBA(void * a) {
  int i = *((int *) a);    
  while(1){
    //Procedimentos para acessar a corda
      pthread_mutex_lock(&lock_conta_macaco);
        pthread_mutex_lock(&lock_B);
          if(num_macacos_BA) num_macacos_BA++;

          if(num_macacos_BA == 1){
            pthread_mutex_lock(&lock_travessia);
          }
        pthread_mutex_unlock(&lock_B);
      pthread_mutex_unlock(&lock_conta_macaco);
    
    printf("Macaco %d passado de B para A \n",i);
    sleep(1);

    pthread_mutex_lock(&lock_B);
      num_macacos_BA--;
      if(num_macacos_BA == 0) {
        pthread_mutex_unlock(&lock_travessia);
      }
    pthread_mutex_unlock(&lock_B);
    //Procedimentos para quando sair da corda
  }
  pthread_exit(0);
}


void * gorila(void * a){
  while(1){
    pthread_mutex_lock(&lock_conta_macaco);
      pthread_mutex_lock(&lock_travessia);
        //Procedimentos para acessar a corda
        printf("Gorila passado de A para B \n");
        sleep(5);
        //Procedimentos para quando sair da corda
      pthread_mutex_unlock(&lock_travessia);
    pthread_mutex_unlock(&lock_conta_macaco);
  }
  pthread_exit(0);
}

int main(int argc, char * argv[]){
  pthread_t macacos[MA+MB];
  int *id;
  int i = 0;

  for(i = 0; i < MA+MB; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;

    if(i%2 == 0){
      if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
        printf("Não pode criar a thread %d\n", i);
        return -1;
      }
    } else {
      if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
        printf("Não pode criar a thread %d\n", i);
        return -1;
      }
    }
  }

  pthread_t g;
  pthread_create(&g, NULL, &gorila, NULL);


  pthread_join(macacos[0], NULL);
  return 0;
}
