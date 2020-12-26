//entry process, resposible to create customers
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "pelecom.h"
#include "random.h"
#include "stopwatch.h"


int main(int argc, char* argv[]){

    printf("entry process running now\n");

    //get the memKey from from argv[1]
    int memkey = atoi(argv[1]);
    int cstNum = 0;
    int size = sizeof(customer);
    stopwatch *sw;
    int msgID = msgget(memkey,0666 | IPC_CREAT); //create message queue
    if(msgID == -1)
        perror("entry process: msgget() failed: ");

    sw = (stopwatch *)malloc(sizeof(stopwatch));
    if(sw == NULL){
        printf("from entry: error allocating stop watch\n");
        exit(-1);
    }
    //the store is open, start the stopwach
    initrand();
    swstart(sw);
    while(swlap(sw) < 10 ){ //10 sec
        customer cst;
        cst.c_data.enter_time = swlap(sw);
        int value =(int) urand(0,100);
        if(value <= POP_NEW){
            cst.c_data.type = TYPE_NEW;
            cst.c_data.process_time = (int)pnrand(AVRG_NEW,SPRD_NEW,MIN_NEW);
        }else if(value <= POP_UPGRADE){
            cst.c_data.type = TYPE_UPGRADE;
            cst.c_data.process_time = (int)pnrand(AVRG_UPGRADE,SPRD_UPGRADE,MIN_UPGRADE);
        }else{
            cst.c_data.type = TYPE_REPAIR;
            cst.c_data.process_time = (int)pnrand(AVRG_REPAIR,SPRD_REPAIR,MIN_REPAIR); 
        }
        cst.c_id = ++cstNum;
        printf("new customer arrive: ID: %d, type: %d\n",cstNum,cst.c_data.type);
        if( msgsnd(msgID,&cst,size,0) == -1)
            perror("entry Process: msgsnd(): ");

    }
    //store closed, send exit customer
    customer quit;
    quit.c_data.type = TYPE_QUIT;
    quit.c_id = cstNum + 1;

    if(msgsnd(msgID,&quit,size,0) == -1)
        perror("entry Process(quit1): msgsnd(): ");
    free(sw);
    printf("total arrivals today is: %d customers.\n",cstNum);
    return 0;
}