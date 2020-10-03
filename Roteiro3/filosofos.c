#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//Globals
int fullbellys = 0;                                                                                                     // Numero de filosofos satisfeitos;
pthread_mutex_t mutex;                                                                                                  // Mutex para discriminação dos filosofos;
pthread_mutex_t forkMutex;                                                                                              // Mutex para discriminação dos garfos;
int philoNumber = 4;                                                                                                    // Número de filosofos;
int bellySize = 5;                                                                                                      // Capacidade padrão das barrigas dos filosofos;

typedef struct philo{
    pthread_t Thread;                                                                                                   // Long Long que representará um ID da thread;
    int number;                                                                                                         // Número representante do filosofo;
    int belly;                                                                                                          // Barriga do filosofo;
    struct philo * next;                                                                                                // Ponteiro para o proximo filosofo;
} philo;

typedef struct fork{
    int number;                                                                                                         // Numero representante do garfo;
    int inUse;                                                                                                          // Variável condicional de uso dos garfos;
} fork;

typedef struct Table{
    struct philo * head;                                                                                                // Ponteiro para o filosofo inicial;
    fork * forks;                                                                                                       // Ponteiro para localização do array de garfos;
}Table;


Table * table;                                                                                                          // Ponteiro global principal;

void delay(int secs) { //utility function                                                                               // Função ultilitária;
    time_t beg = time(NULL),
    end = beg + (rand() % secs);
    do ; while (time(NULL) < end);
}

// initialize table
void initializate(){                                                                                                    // Função para inicializar as estruturas
    table = (Table*) malloc(sizeof(Table));                                                                             // Alocação da estrutura principal
    philo * aux = NULL;                                                                                                 // Variavel auxiliar para criação dos filosofos

    for(int i = 1; i <= philoNumber; i++ ){
        philo * novo = (philo*) malloc(sizeof(philo));                                                                  // Alocação dos filosofos
        novo->number = i;                                                                                                   // Definição do número representante
        novo->belly = 0;                                                                                                    // Inicialização da variavel belly
        if(aux == NULL){
            table->head = novo;                                                                                             // Aquisição do ponteiro na mesa
        }
        else{
            aux->next = novo;                                                                                               // Caso não seja o primeiro filosofo a entrar na mesa
            if(i == philoNumber){novo->next = table->head;}                                                                 // Conecção do fim da lista com o inicio
            else{ novo->next = NULL; }                                                                                      // Inicialização do ponteiro para o proximo filosofo
        }
        aux = novo;                                                                                                         // "Incremento" da variável aux para o proximo endereço
    }

    // cria e armazena os forks
    table->forks = malloc(philoNumber * sizeof(fork));                                                             // Alocação dos garfos
    for(int i = 1; i <= philoNumber; i++){
        fork* new = (fork*) malloc(sizeof(fork));                                                                           // Alocação do novo garfo
        new->number = i;                                                                                                    // Número de representação do garfo
        new->inUse = 0;                                                                                                     // Reset da variável de controle

        table->forks[i] = *new;                                                                                         // Armazenamento do novo garfo
    }
}

void * consume(void * arg){
    philo * p = (philo*) arg;                                                                                           // conversão do ponteiro arg
    while(1){                                                                                                           // Loop infinitoish
        if(p->belly == bellySize){                                                                                      // verificação se o filosofo esta de barriga cheia
            fullbellys++;                                                                                               // incrementa o número de filosofos cheios
            printf("O filosofo %d tá de bucho cheio   %d/%d\n\n", p->number, fullbellys, philoNumber);
            break;                                                                                                      // quebra o laço
        }
        else {                                                                                                          // O filosofo ainda está com fome
            printf("filosofo %d começa a pensar\n\n", p->number);
            delay(rand() % 10 + 2);                                                                                // delay de largura randomica
            printf("filosofo %d tenta comer\n\n", p->number);
            pthread_mutex_lock(&mutex);                                                                                 // trava do mutex do filosofo
            if (table->forks[p->number].inUse + table->forks[p->next->number].inUse == 0) {                             // Verificação se os garfos requeridos pelo filosofo estão livres
                printf("filosofo %d pega o garfo %d e o %d e começa a comer\n\n", p->number, p->number,
                       p->next->number);
                pthread_mutex_lock(&forkMutex);                                                                         // trava do mutex dos garfos
                table->forks[p->number].inUse = 1;
                table->forks[p->next->number].inUse = 1;
                pthread_mutex_unlock(&forkMutex);                                                                       // destrava do mutex dos garfos
                pthread_mutex_unlock(&mutex);                                                                           // destrava o mutex dos filosofos

                delay(5);                                                                                          // Tempo que o filosofo leva para comer

                printf("filosofo %d terminou de comer!\n\n", p->number);
                p->belly++;                                                                                             // incrementa o tamanho da barriga
                pthread_mutex_lock(&forkMutex);                                                                         // trava do mutex dos garfos
                table->forks[p->number].inUse = 0;
                table->forks[p->next->number].inUse = 0;
                pthread_mutex_unlock(&forkMutex);                                                                       // destrava do mutex dos garfos
            } else {
                printf("filosofo %d não conseguiu comer\n\n", p->number);
            }
        }
        pthread_mutex_unlock(&mutex);                                                                                   // destrava o mutex dos filosofos
    }
}

//Initialization for all Threads
void startThreads(){                                                                                                    // função para inicializar todas as threads
    pthread_attr_t * attr = NULL;                                                                                       // atributos para as threads
    philo * aux = table->head;                                                                                          // inicialização da variável para percorrer a lista de filosofos

    for(int i =0; i < philoNumber; i++){
        pthread_create(&aux->Thread, attr, consume, aux);                                                               // inicialização das threads
        aux = aux->next;                                                                                                // incrementa variavel de controle
    }
}

// Print all philosophers structs
//void printa(){
//    int j = 0;
//    for(philo *i = table->head ; i != NULL; i = i->next){
//        printf("Philo %d\n", i->number);
//        if(j == 7){
//            break;
//        }
//        j++;
//    }
//    for (int i = 0; i < philoNumber; i++) {
//        printf("fork %d\n", table->forks[i].number);
//    }
//}
//

int main() {
    printf("Digite o número de filosofos!: ");
    scanf("%d", &philoNumber);                                                                                   // set do philoNumber
    srand(time(NULL));
    initializate();                                                                                                     // inicialização da estrutura de dados
    //printa();

    startThreads();                                                                                                     // inicializa as threads
    while(fullbellys < philoNumber){};                                                                                  // Espera enquanto todos os filosofos não estirevem de barriga cheia
    free(table);                                                                                                        // desaloca a mesa
}


