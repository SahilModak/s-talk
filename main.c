#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include "receiver.h"
#include "sender.h"
#include "arguments.h"

#define BUFFER_LENGTH 2048

int main(int argCount, char** args){
    if(argCount >  4)
    {
        printf("Too many arguements provided\n");
        return 0;
    }
    else if(argCount < 4){
        printf("Not enough arguementss provided\n");
        return 0;
    }

    int myPort = atoi(args[1]);
    int remotePort = atoi(args[3]);

    if(myPort < 1025 || myPort > 65532){
        printf("Invalid local port number provided\n");
    }

    if(remotePort < 1025 || remotePort > 65532){
        printf("Invalid local port number provided\n");
    }

    struct inpArgs arg;
    
    arg.myPort = myPort;
    arg.remotePort = remotePort;
    arg.name = args[2];
    arg.charBufferLength = BUFFER_LENGTH;

    receiver_create(&arg);
    sender_create(&arg);

    printf("The s-talk session has started\n");
    
    receiver_join();
    sender_join();
    printf("The s-talk session has ended\n");

    //ToDo:
    //check for malloc
    
    
    return 0;

}