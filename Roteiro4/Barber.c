#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define limit 20; // limite de usuários para fechar a barbearia

int chairs; // número de cadeiras
int clientCounter = 0; // número de cliente que vieram a barbearia
int open = 0; // se a barbearia esta fechada ou não
int barberworkingchair = -1; // número do cliente que se encontra na cadeira



sem_t custumers; // semaforo para as cadeiras de esperas

pthread_mutex_t barberChairMutex; // mutex de controle da cadeira de trabalho do barbeiro 
pthread_mutex_t clientLooks; // mutex para checagem de valores compartilhados
pthread_mutex_t barberwork; // mutex para atomisidade do barbeiro

pthread_cond_t barberturn; // condição de parada para o barbeiro



int waiting = 0; // variável para coletar o número de lugares vagos


void delay(int number_of_seconds) // utility function
{
    // Converting time into milli_seconds
    int milli_seconds = 400000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

void * barberFunction (void * arg){ // função do barbeiro
    int last = -1; // variável para guardar o número do ultimo valor
    while(1){
        if(clientCounter >= 20){ // verificação para fechar a barbearia
            open = 0;
            if(waiting == 0){ // condição para que o barbeiro termine todos os clientes que esperam, para depois morrer
                pthread_exit(NULL);
            }
        }
        pthread_mutex_lock(&barberwork); // trava do trabalho do barbeiro
        if(waiting == 0) { // caso não haja cliente, o barbeiro dorme
            printf("Barber go to sleep! Zzz...\n\n");
            pthread_cond_wait(&barberturn, &barberwork); // condição de espera do barbeiro
        }
        pthread_mutex_unlock(&barberwork);
        if(last != barberworkingchair){ // verifica se um novo cliente se sentou na cadeira
            last = barberworkingchair; // atribuição do ultimo na cadeira
            printf("\033[0;32m Barber works on client %d\n\n", barberworkingchair);

            delay(4); // delay do corte de cabelo

            printf("\033[0;32m Barber finished working on client %d\n\n", barberworkingchair);
            
            pthread_mutex_unlock(&barberChairMutex); // desbloqueia o mutex da cadeira do barbeiro para o proximo cliente
        }
        
    }
    return NULL;
}

void * clientFunction ( void * arg) { // função do cliente
    // Verification for full barbershop
    pthread_mutex_lock(&clientLooks); // trava o mutex de verificações criticas
    int number = clientCounter++; // gravação do número do cliente localmente e incremento da variavel compartilhada

    if(waiting == chairs){ // verificação se a espaço de espera na barbearia
        printf("\033[0;31m Barber shop its full for client %d\n\n", number);
        pthread_mutex_unlock(&clientLooks); // desbloqueia o mutex de verificação
        return NULL;
    }
    pthread_mutex_unlock(&clientLooks); // libera o mutex de verificações criticas

    //Client enters process line
    
    sem_wait(&custumers); // decremento do semaforo das cadeiras
    waiting++; // incremento da variavel de controle
    printf("\033[0;31m Client %d is waiting\n\n", number);

    pthread_mutex_lock(&barberChairMutex); // trava o mutex da cadeira do barbeiro

    barberworkingchair = number; // coloca o número do cliente na cadeira
    pthread_cond_signal(&barberturn); // sinaliza para que o barbeiro acorde
    sem_post(&custumers); // libera uma cadeira de espera no semaforo
    waiting--; // decrementa o número de controle

    
    return NULL;
}

void startThreads(){ // função para gerar as threads dos clientes

    while(open){
        delay(rand() % 2); // delay randomico de geração do cliente
        pthread_t new;
        pthread_create(&new, NULL, clientFunction, NULL); // inicialização da thread
    }
}



int main(){
    srand(time(NULL));
    printf("Numero de cadeiras: ");
    scanf("%d", &chairs); // número de cadeiras de espera
    sem_init(&custumers, 0, chairs); // inicialização do semaforo 

    pthread_t barber;
    pthread_create(&barber, NULL, barberFunction, NULL); // inicialização da thread do barbeiro
    open = 1; // abre a barbearia
    startThreads();  // inicio dos clientes

    pthread_join(barber, NULL); // espera que a thread do barbeiro termine 
    printf("Terminou tudo\n");
    return 0;
}
