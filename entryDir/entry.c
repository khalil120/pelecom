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

struct mesg_buffer { 
    long msgType; 
    customer *customer; 
} message; 

int main(int argc, char* argv[]){

    //get the memKey from from argv[1]
    int memkey = atoi(argv[1]);
    int cstNum = 0;
    stopwatch *sw;
    long time = 1000000 * 43 ; //43 seconds
    int msgID = msgget(memkey,0666 | IPC_CREAT); //create message queue
    if(msgID == -1)
        perror("entry process: msgget() failed: ");
    
    message.msgType = 1;

    sw = (stopwatch *)malloc(sizeof(stopwatch));
    if(sw == NULL){
        printf("from entry: error allocatinf stop watch\n");
        exit(-1);
    }
    //the store is open, start the stopwach
    initrand();
    swstart(sw);
    while(swlap(sw) < time){
        customer *cst = (customer *)malloc(sizeof(customer));
        cst->c_data.enter_time = swlap(sw);
        int value =(int) urand(0,100);
        if(value <= POP_NEW){
            cst->c_data.type = TYPE_NEW;
            cst->c_data.process_time = (int)pnrand(AVRG_NEW,SPRD_NEW,MIN_NEW);
        }else if(value <= POP_UPGRADE){
            cst->c_data.type = TYPE_UPGRADE;
            cst->c_data.process_time = (int)pnrand(AVRG_UPGRADE,SPRD_UPGRADE,MIN_UPGRADE);
        }else{
            cst->c_data.type = TYPE_REPAIR;
            cst->c_data.process_time = (int)pnrand(AVRG_REPAIR,SPRD_REPAIR,MIN_REPAIR); 
        }
        cst->c_id = ++cstNum;
        message.customer = cst;
        msgsnd(msgID,&message,sizeof(message),0);
    }
    //store closed, send 3 exit customers
    customer *quit1 = (customer *)malloc(sizeof(customer));
    customer *quit2 = (customer *)malloc(sizeof(customer));
    customer *quit3 = (customer *)malloc(sizeof(customer));
    quit1->c_data.type = TYPE_QUIT;
    msgsnd(msgID,quit1,sizeof(customer),0);
    quit2->c_data.type = TYPE_QUIT;
    msgsnd(msgID,quit2,sizeof(customer),0);
    quit3->c_data.type = TYPE_QUIT;
    msgsnd(msgID,quit3,sizeof(customer),0);
    printf("total arrivals today is: %d\n",cstNum);
    free(sw);
    return 0;
}
