#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int chairs;
int clientCounter = 0;
int open = 0;
int barberworkingchair = -1;



sem_t custumers;
sem_t barberChairMutex;

pthread_mutex_t clientLooks;

pthread_cond_t barberChairCond;
pthread_cond_t barberturn;


pthread_mutex_t barberwork;
int waiting = 0;


void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

void * barberFunction (void * arg){
    int last = -1;
    while(1){
        if(clientCounter >= 20){
            open = 0;
            if(waiting == 0){
                pthread_exit(NULL);
            }
        }
        pthread_mutex_lock(&barberwork);
        if(waiting == 0) {
            printf("Barber go to sleep! Zzz...\n\n");
            pthread_mutex_unlock(&barberwork);
            pthread_cond_wait(&barberturn, &barberwork);
        }
        else if(last != barberworkingchair){

            sem_post(&custumers);
            waiting--;

            printf("\033[0;32m Barber works on client %d\n\n", barberworkingchair);

            delay(5);

            printf("\033[0;32m Barber finished working on client %d\n\n", barberworkingchair);
            pthread_mutex_unlock(&barberwork);
            last = barberworkingchair;
            sem_post(&barberChairMutex);
        }
        pthread_mutex_unlock(&barberwork);
    }

}

void * clientFunction ( void * arg) {
    // Verification for full barbershop
    pthread_mutex_lock(&clientLooks);
    int number = clientCounter;
    clientCounter++;

    if(waiting == chairs){
        printf("\033[0;31m Barber shop its full for client %d\n\n", number);
        pthread_mutex_unlock(&clientLooks);
        return NULL;
    }
    pthread_mutex_unlock(&clientLooks);

    //Client enters process line
    waiting++;
    sem_wait(&custumers);

    printf("\033[0;31m Client %d is waiting\n\n", number);

    sem_wait(&barberChairMutex);

    barberworkingchair = number;
    pthread_cond_signal(&barberturn);

}

int startThreads(){

    while(open){
        delay(rand() % 7);
        pthread_t new;
        pthread_create(&new, NULL, clientFunction, NULL);
    }
}



int main(){
    srand(time(NULL));
    printf("Numero de cadeiras: ");
    scanf("%d", &chairs);

    sem_init(&barberChairMutex, 0, 1);
    sem_init(&custumers, 0, chairs);

    pthread_t barber;
    pthread_create(&barber, NULL, barberFunction, NULL);
    open = 1;
    startThreads();

    pthread_join(barber, NULL);
    printf("Terminou tudo\n");

}
