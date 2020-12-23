#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include "pelecom.h"


struct mesg_buffer { 
    long msgType; 
    customer *customer; 
} message; 

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

    while(isOpen){
        //first get customer from the entry.
        if(msgrcv(entryMsgID,&message,sizeof(message),0,1) == -1)
            perror("recption process: msgrcv(): ");
        
        if(message.customer->c_data.type != TYPE_QUIT){
            //real customers, move the customer to its wnated services queue
            if(message.customer->c_data.type == TYPE_NEW){
                msgsnd(newCstMsgID,&message,sizeof(message),0);
            }else if(message.customer->c_data.type == TYPE_REPAIR){
                msgsnd(repairCstMsgID,&message,sizeof(message),0);
            }else
                msgsnd(upgradeCstMsgID,&message,sizeof(message),0);
        }else{
            //close message
            if(closing_signal == 0){
                msgsnd(newCstMsgID,&message,sizeof(message),0);
            }else if(closing_signal == 1){
                msgsnd(repairCstMsgID,&message,sizeof(message),0);
            }else{
                msgsnd(upgradeCstMsgID,&message,sizeof(message),0);
            }
            closing_signal++;
            if(closing_signal == 3)
                isOpen = false;
        }
    }
    //when the store close the recption proccess close the message queue with entry process
    msgctl(entryMsgID,IPC_RMID,NULL); 
    return 0;
}
