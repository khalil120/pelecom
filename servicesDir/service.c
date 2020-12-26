#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "pelecom.h"
#include "stopwatch.h"



int main(int argc, char* argv[]){

    printf("%s process is running now\n",argv[0]);

    int msgKey = atoi(argv[1]);
    bool isOpen = true;
    int total = 0;
    long timeInStore = 0;
    int queuID = msgget(msgKey, 0666 | IPC_CREAT);
    if(queuID == -1){
        perror("services process: msgget(): ");
        exit(-1);
    }



    stopwatch *sw = (stopwatch *)malloc(sizeof(stopwatch));
    if(sw == NULL){
        printf("service process: faild to allocate memory for sw\n");
        exit(-1);
    }

    int size = sizeof(customer);
    customer cst;;
    swstart(sw);
    long alltime;
    while(isOpen){
        //get customer frim the queue
        if(msgrcv(queuID,&cst,size,0,MSG_NOERROR) == -1)
            perror("recption process: msgrcv(): ");
        if(cst.c_data.type == TYPE_QUIT)
            break;
        printf("customer %ld arrived to %s, handling his request... \n",cst.c_id,argv[0]);
        total++;
        cst.c_data.start_time = swlap(sw);
        usleep(cst.c_data.process_time);
        cst.c_data.exit_time = swlap(sw);
        alltime = cst.c_data.exit_time - cst.c_data.enter_time;
        cst.c_data.elapse_time = alltime;
        timeInStore += alltime;
        printf("customer %ld served in %s and going home... \n",cst.c_id,argv[0]);
    }
    long runTime = swlap(sw);
    printf("total %s for today is: %d\n",argv[0],total);
    printf("total services time for %s is: %ld ms\n",argv[0],runTime);
    printf("average services time for %s is: %lf ms\n",argv[0],(double)runTime / total);
    printf("average time for %s in store is: %lf ms\n",argv[0],(double)timeInStore / total);

    msgctl(queuID,IPC_RMID,NULL);
    free(sw);
    printf("%s services done and closed.\n",argv[0]);
    return 0;
}
