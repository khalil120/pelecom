#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include "pelecom.h"


int main(int argc, char* argv[]){

    //argv[1] -> the key of the queue to recieve message
    //argv[2] -> the key of message queue of new customer services
    //argv[3] -> the key of message queue of new repair services
    //argv[4] -> the key of message queue of new upgrade services

    int entrykey = atoi(argv[1]);
    int newkey = atoi(argv[2]);
    int repairKey = atoi(argv[3]);
    int upgradeKey = atoi(argv[4]);
    int closing_signal = 0;
    bool isOpen = true;

    printf("recption process running now\n");
    

    //message queue to read message from entry proccess
    int entryMsgID = msgget(entrykey, 0666 | IPC_CREAT);
    if(entryMsgID == -1)
        perror("recption process: msgget() faild: "); 


    //3 messages queue to send the message to its relevant service
    int newCstMsgID = msgget(newkey, 0666 | IPC_CREAT);
    if(newCstMsgID == -1)
        perror("recption process: msgget() faild: ");

    int repairCstMsgID = msgget(repairKey, 0666 | IPC_CREAT);
    if(repairCstMsgID == -1)
        perror("recption process: msgget() faild: ");

    int upgradeCstMsgID = msgget(upgradeKey, 0666 | IPC_CREAT);
    if(upgradeCstMsgID == -1)
        perror("recption process: msgget() faild: ");

    size_t size = sizeof(customer);
    customer cst;
    while(isOpen){
        //first get customer from the entry.
        if(msgrcv(entryMsgID,&cst,size,0,MSG_NOERROR) < 0){
            perror("recption process: msgrcv(): ");
        }
        printf("customer %ld arrived to the recption \n",cst.c_id);
        if(cst.c_data.type != TYPE_QUIT){
            //real customers, move the customer to its wnated services queue
            if(cst.c_data.type == TYPE_NEW){
               if(msgsnd(newCstMsgID,&cst,size,0) == -1)
                    perror("recption process: msgsnd(): ");
            }else if(cst.c_data.type == TYPE_REPAIR){
                if(msgsnd(repairCstMsgID,&cst,size,0) == -1)
                    perror("recption process: msgsnd(): ");
            }else
                if(msgsnd(upgradeCstMsgID,&cst,size,0) == -1)
                    perror("recption process: msgsnd(): ");
        }else{
            //close message - send exit commands to the 3 service types.
            if(msgsnd(newCstMsgID,&cst,size,0) == -1)
                perror("recption process: msgsnd(): ");

            if(msgsnd(repairCstMsgID,&cst,size,0) == -1)
                perror("recption process: msgsnd(): ");
 
            if(msgsnd(upgradeCstMsgID,&cst,size,0))
                perror("recption process: msgsnd(): ");
            isOpen = false;
            break;
        }
        
    }
    printf("recption closed, all customers moved to the queues\n");
    //when the store close the recption proccess close the message queue with entry process
    msgctl(entryMsgID,IPC_RMID,NULL); 
    return 0;
}