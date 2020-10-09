#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


//globals
int bathroom = 0;
int bathGender = -1;

// settable variables
int boxes;
int numGenders;

// semaphores
sem_t bath;

//mutexs
pthread_mutex_t checker;
pthread_mutex_t bathMutex;
pthread_cond_t waitCond;


void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 400000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

void * user( void * arg ){
  int gender = rand() % numGenders;
  
  printf("\033[39muser with gender %d wants to join the bathroom\n", gender);
  while(1){
    pthread_mutex_lock(&checker);
    if(bathGender != -1 && gender != bathGender){
      printf("\033[31mUser waits on line, the bathroom now is in %d mode\n\n", bathGender);
      pthread_cond_wait(&waitCond, & checker);
      pthread_mutex_unlock(&checker);
      continue;
    }
    bathGender = gender;
    pthread_mutex_unlock(&checker);

    sem_wait(&bath);
    bathroom++;
    printf("\033[32mUser %d enters in the bathroom!\n\n", gender);
    delay(10);
    printf("\033[32mUser %d finished his job\n\n", gender);
    pthread_mutex_lock(&bathMutex);
    bathroom--;
    if(bathroom == 0){
      bathGender = -1;
    }
    sem_post(&bath);
    pthread_mutex_unlock(&bathMutex);
    pthread_cond_signal(&waitCond);
    break;
  }
  pthread_exit(NULL);
}

int main(void) {
  printf("Número de boxes: ");
  scanf("%d", &boxes);
  printf("Número de generos: ");
  scanf("%d", &numGenders);

  //Pré sets

  sem_init(&bath, 0, boxes);

  while(1){
    pthread_t ut;
    pthread_create(&ut, NULL, user, NULL);
    delay(5);
  }
  return 0;
}