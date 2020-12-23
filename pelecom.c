/* Khalil Qablawi
*  Advanced Unix - HW3
*  this is the main process prestents the pelecom 
*/
#include "pelecom.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <stdlib.h>

key_t getKey(char* name); //generate key to use in shared memory

int main(int argc, char* argv[]){

    pid_t entryProcess;
    pid_t recption;
    pid_t newCustServ, repairServ, upgradeServ;
    key_t key1 = getKey("communicate.c");
    key_t repairKey = getKey("repairServices");
    key_t newCstKey = getKey("newCstServices");
    key_t upgradeKey = getKey("upgradeServices");
    int status;
    char strID[10];
    char strID2[10];
    char repair[10];
    char new[10];
    char upgrade[10];
    sprintf(strID,"%d",(int)key1); //converts id to char* to send it in argv
    sprintf(repair,"%d",(int)repairKey);
    sprintf(new,"%d",(int)newCstKey);
    sprintf(upgrade,"%d",(int)upgradeKey);

    printf("initializing the store...\n");
    
    newCustServ = fork();
    if(newCustServ == -1)
        perror("fork() faild: ");
    if(newCustServ == 0){
        argv[0] = "New Customers";
        argv[1] = new;
        execv("./servicesDir/service",argv);
    }
    //parent process
    repairServ = fork();
    if(repairServ == -1)
        perror("fork() faild: ");
    if(repairServ == 0){     
        argv[0] = "repair Customers";
        argv[1] = repair;
        execv("./servicesDir/service",argv);
    }
    //parent process
    upgradeServ = fork();
    if(upgradeServ == -1)
        perror("fork() faild: ");
    if(upgradeServ == 0){
        argv[0] = "upgrade Customer";
        argv[1] = upgrade;
        execv("./servicesDir/service",argv);
    }
    //parent process
    if((recption = fork())==-1)
        perror("fork() faild: ");
    if(recption == 0){
        //recption process
        argv[0] = "recption.c";
        argv[1] = strID;
        argv[2] = new;
        argv[3] = repair;
        argv[4] = upgrade;
        execv("./recptionDir/recption",argv);
    }
    
   //parent process
    entryProcess = fork();
    if(entryProcess == -1)
        perror("fork() faild: ");
    if(entryProcess == 0){
        //create the entry process
        argv[0] = "entry.c";
        argv[1] = strID;
        execv("./entryDir/entry",argv);
    }
    pid_t parent = wait(&status);
    return 0;
}


key_t getKey(char* name){
    return ftok(name,'B');
}
