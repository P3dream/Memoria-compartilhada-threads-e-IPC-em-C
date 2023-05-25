#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#define MAX_TAM 100
#define NUM_ITERACOES 30

typedef struct message{
    long tipo;
    char texto[MAX_TAM];
}msg;

pthread_mutex_t mutexsum;
sem_t *sema;
int *var_shared;
 
void *countSum(void *tid){
    long id = (long)tid;
    int local;
    for (int i=0;i<NUM_ITERACOES;i++){
        sem_wait(sema);
        local = var_shared[0];
        local -= 1;
        
        usleep((rand() + 200000) % 2000000);
                    
        var_shared[0] = local;
        var_shared[1] = var_shared[1] + 1;
        printf("thread %ld, - iteracao: %d - Na thread var_shared[0] = %d var_shared[1] = %d\n",id,i,var_shared[0],var_shared[1]);
        sem_post(sema);
        usleep((rand() + 200000) % 2000000);
 
    }
    printf("Finalizando thread %ld \n",id);
    pthread_exit(NULL);
}

int main(){

    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);

    
    if(msqid == -1){
        perror ("Erro msgget()") ;
        exit(1) ;
    }
    
    int lg;
    long type = 1;     
    int size_msg = MAX_TAM; 
    
    sema = mmap(NULL, sizeof(sem_t),PROT_READ | PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, -1,0);
    var_shared = mmap(NULL, (2*sizeof(int)),PROT_READ | PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, -1,0);
    
    if(sem_init(sema ,1 ,1)<0){
        perror("sem_init");
        exit(EXIT_FAILURE); 
    }
    
    var_shared[0] = 120;
    var_shared[1] = 0;
    
    int process = fork();
    
    if(process != 0){//processo pai 
        
        int pid = getpid();
        
        int process2 = fork();
        
        if(process2!=0){// processo pai
            int pid = getpid();
            int process3 = fork();
            
            
            if(process3!=0){
                int pid = getpid();
                int id = 1;
                
                for(int i=0;i<3;i++){
                    msg mensagem;
                    mensagem.tipo = 1;
                    msgrcv(msqid,&mensagem,size_msg,type,0);
                    printf("Pai recebeu mensagem: %s\n",mensagem.texto);
                }
                printf("Encerrando programa...\n");
                printf("Pai PID: %d Valor final das variaveis: var_shared[0] = %d var_shared[1] = %d\n",getpid(),var_shared[0],var_shared[1]);
            }
            else{
                int local;
                for(int i = 0; i < NUM_ITERACOES; i++){
                    sem_wait(sema);
                    local = var_shared[0];
                    local -= 1;
                    
                    usleep((rand() + 200000) % 2000000);
                    
                    var_shared[0] = local;
                    var_shared[1] = var_shared[1] + 1;
                    
                    sem_post(sema);
                    printf("Processo filho 3 - iteracao: %d - com PID: %d - var_shared[0] = %d var_shared[1] = %d\n",i,getpid(),var_shared[0],var_shared[1]);
                    usleep((rand() + 200000) % 2000000);
                }
        
                printf("Filho 3 informa que acabou\n");
                msg mensagem;
                mensagem.tipo = 1;
                sprintf(mensagem.texto,"Filho 3 informando que acabou");
                    
                if(msgsnd(msqid,&mensagem,strlen(mensagem.texto),IPC_NOWAIT) == -1){
                    perror("Não foi possivel enviar a mensagem do filho 3");
                    exit(-1) ;
                }
                exit(0);
                
            }
        }
        else{ //processo filho2
            int local;
            for(int i = 0; i < NUM_ITERACOES; i++){
                sem_wait(sema);
                local = var_shared[0];
                local -= 1;
                
                usleep((rand() + 200000) % 2000000);
                    
                var_shared[0] = local;
                var_shared[1] = var_shared[1] + 1;
                sem_post(sema);
                 printf("Processo filho 2 - iteracao: %d - com PID: %d - var_shared[0] = %d var_shared[1] = %d\n",i,getpid(),var_shared[0],var_shared[1]);
                usleep((rand() + 200000) % 2000000);
            }
            printf("Filho 2 informa que acabou\n");
            
            msg mensagem;
            mensagem.tipo = 1;
            
            
            sprintf(mensagem.texto,"Filho 2 informando que acabou");
                
            if(msgsnd(msqid,&mensagem,strlen(mensagem.texto),IPC_NOWAIT) == -1){
                perror("Não foi possivel enviar a mensagem do filho 2");
                exit(-1) ;
            }
            exit(0);
            
        }
       
    }
    else{ // processo filho1
        int pid = getpid();
        printf("Sou o processo filho 1 com PID: %d\n",pid);
        //sem_wait(sem);
        pthread_t t1, t2; //criando t1 e t2 do tipo thread
        int create; //variável que recebe o retorno da função pthread_create()
        long num = 1; //identificador da thread
        
        printf("Filho 1: criando a thread %ld\n", num);
        
        create = pthread_create(&t1, NULL, countSum, (void *)num); //criando thread 1
     
        num = 2;
        
        printf("Filho 1: criando a thread %ld\n", num);
        create = pthread_create(&t2, NULL, countSum, (void *)num); //criando thread 2
     
     
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        
        printf("Filho 1 informa que acabou\n");
        
        msg mensagem;
        mensagem.tipo = 1;
        sprintf(mensagem.texto,"Filho 1 informando que acabou");
                
        if(msgsnd(msqid,&mensagem,strlen(mensagem.texto),IPC_NOWAIT) == -1){
            perror("Envio de mensagem impossivel");
            exit(-1) ;
        }
        exit(0);
    }

    return 0;
}









