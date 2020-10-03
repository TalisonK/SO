#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 

#define bufferSize 10 //Max number of data in buffer;
#define totalData 20 //Max pushs to the buffer;

//structs
typedef struct no{
    int data;
    struct no * prox;
}no;

typedef struct head{
    struct no * top;
    int length;
    int writeable;
}header;

//globals
header * buffer;
int prodTime=3;
int consTime=7;
int duration = 0;
int data = 0;
pthread_mutex_t mutex;

// Database managers
int push() {
    if(buffer->length == bufferSize){
        printf("buffer cheio\n");
        return -1;
    }
    no * new = (no*) malloc( sizeof(no));

    new -> data = data;
    new -> prox = NULL;
    data++;

    

    if(buffer->top == NULL){
        buffer->top = new;
    }
    else{
        no * aux = buffer->top;
        for(;aux->prox != NULL; aux = aux->prox);
        aux -> prox = new;
    }

    buffer -> length ++;
    printf("w     %d/%d    Stored: %d\n\n", buffer->length, bufferSize, data);
    return data;
}

int pop(){
    if(buffer ->top == NULL || buffer -> length == 0){
        return -1;
    }
    no * data = buffer->top;
    buffer -> top = data->prox;
    buffer -> length--;

    int ret = data -> data;
	printf("r     %d/%d    Retrieved: %d\n\n",buffer->length, bufferSize, ret);
    free(data);
    return ret;
}

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

// producer Thread method
void *producer() {
    while (prodTime > 0){
        pthread_mutex_lock(&mutex); //Obtain exclusive mutex lock
        if(data == totalData) { 
            printf("Produção Concluida\n"); 
            prodTime = 0;
            buffer->writeable = 0;
        }
        else{
            if(buffer -> writeable == 1 && buffer -> length == bufferSize){
                    buffer -> writeable = 0;
                    printf("w     buffer cheio\n\n");
                    prodTime = 12;
                }
                else{ push();}
        }
        pthread_mutex_unlock(&mutex); //Release mutex lock
        delay(prodTime);
    } 
}

//consumer Thread method
void * consumer(){
    while(1){
        pthread_mutex_lock(&mutex); //Obtain exclusive mutex lock
        
        if(buffer -> length == 0){
			printf("r     buffer vazio\n");
            if(data == totalData) { 
                pthread_exit(NULL);
            }
            prodTime = 3;
        }
        else{
            pop();
            buffer->writeable = 1;
        }
        pthread_mutex_unlock(&mutex); //Release mutex lock

        delay(consTime);
    }
}


int main(){
    buffer = (header*) malloc(sizeof(header));
    buffer -> length = 0;
    buffer -> writeable = 1;

    pthread_attr_t * attr = NULL;
    
    pthread_t prodThread, consThread;

    srand(time(NULL)); /* initialise the rng */

    pthread_create(&prodThread, attr, producer, NULL);
    pthread_create(&consThread, attr, consumer, NULL);

    pthread_join(consThread, NULL);
    printf("consumidor Concluido\n\n"); 
    printf("Processo finalizado\n");
    
}