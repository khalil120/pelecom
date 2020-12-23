#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "pelecom.h"
#include "stopwatch.h"



struct mesg_buffer { 
    long msgType; 
    customer *customer; 
} message; 

union semun{
    int  val;    
    struct semid_ds *buf;    
    unsigned short  *array;
};

int semID;

int initSem();
int deleteSem();
int semLock();
int semUnlock();

int main(int argc, char* argv[]){

    int msgKey = atoi(argv[1]);
    bool isOpen = true;
    int total = 0;
    long timeInStore = 0;
    int queuID = msgget(msgKey, 0666 | IPC_CREAT);
    if(queuID == -1){
        perror("srvices process: msgget(): ");
        exit(-1);
    }
    //create and init binary semaphore
    initSem();

    stopwatch *sw = (stopwatch *)malloc(sizeof(stopwatch));
    if(sw == NULL){
        printf("service process: faild to allocate memory for sw\n");
        exit(-1);
    }

    customer *cst;
    swstart(sw);
    long alltime;
    while(isOpen){
        //get customer frim the queue
        if(msgrcv(queuID,&message,sizeof(message),0,1) == -1)
            perror("recption process: msgrcv(): ");
        
        //customer is ready
        cst = message.customer;
        cst->c_data.start_time = swlap(sw);
        //lock the semaphore
        if(cst->c_data.type == TYPE_QUIT){
            free(cst);
            break;
        }
        semLock();
        usleep(cst->c_data.process_time);
        semUnlock();
        cst->c_data.exit_time = swlap(sw);
        alltime = cst->c_data.exit_time - cst->c_data.enter_time;
        cst->c_data.elapse_time = alltime;
        timeInStore += alltime;
        free(cst); //delete the customer when done.
    }
    long runTime = swlap(sw);
    printf("total %s for today is: %d\n",argv[0],total);
    printf("total services time for %s is: %ld ms\n",argv[0],runTime);
    printf("average services time for %s is: %lf ms\n",argv[0],(double)runTime / total);
    printf("average time for %s in store is: %lf ms\n",argv[0],(double)timeInStore / runTime); //not sure of this/////////////////////////////////

    msgctl(queuID,IPC_RMID,NULL); //delete the queue when process done
    deleteSem(); //delete the semaphore when process done
    return 0;
}


int initSem(){
    semID = semget(IPC_PRIVATE,1,0600 | IPC_CREAT);
    if(semID == -1){
        perror("service process - semget(): ");
        return -1;
    }

    union semun param;
    param.val = 1;
    if(semctl(semID,0,SETVAL, param) == -1){
        perror("service process - semctl(): ");
        return -1;
    }

    return 0;
}

int deleteSem(){
    if(semctl(semID,0 ,IPC_RMID, NULL) == -1){
        perror("service process - semctl destroy: ");
        return -1;
    }

    return 0;
}

int semLock(){
    struct sembuf buffer[1];
    buffer[0].sem_num = 0;
    buffer[0].sem_op = -1;
    buffer[0].sem_flg = 0;
    
    if(semop(semID, buffer, 1) == -1){
        perror("semop lock: ");
        return -1;
    }
    return 0;
}

int semUnlock(){
    struct sembuf buffer[1];
    buffer[0].sem_num = 0;
    buffer[0].sem_op = 1;
    buffer[0].sem_flg = 0;
    
    if(semop(semID, buffer, 1) == -1){
        perror("semop unlock: ");
        return -1;
    }
    return 0;
}
