#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<unistd.h>
#include<pthread.h>
#include "arguments.h"
#include "receiver.h"


//65532 - 1025

pthread_t receiverThread;
pthread_t printThread;
static pthread_cond_t receiverCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t receiverMutex = PTHREAD_MUTEX_INITIALIZER;
List* receiverList;
static bool flag=false;

void* f_receiverThread(void* input){  

    Arguements* a = (struct Arguements*) input;
    int myPort = a->myPort;
    int remotePort = a->remotePort;
    char* machineName = a->name;
    int bufferLength = a->charBufferLength;
    
    struct sockaddr_in sockSend;
    memset(&sockSend, 0, sizeof(sockSend));
    sockSend.sin_family = AF_INET;
    sockSend.sin_addr.s_addr = htonl(INADDR_ANY);
    sockSend.sin_port = htons(myPort);

    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    if(socketDescriptor == -1){
        printf("Error: Socket could not be created\n");
        return NULL;
    }
    
    int bindCheck = bind(socketDescriptor, (struct sockaddr *) &sockSend, sizeof(sockSend));
    if(bindCheck == -1){
        printf("Error: Socket bind failure\n");
        return NULL;
    }

    struct hostent *machine;
    struct in_addr ip_addr;
    machine = gethostbyname(machineName);

    if (!machine) {
        printf("Error: Invalid machine name\n");
        return NULL;
    }

    ip_addr = *(struct in_addr *)(machine->h_addr);

    struct sockaddr_in sockRemote;
    memset(&sockRemote, 0 , sizeof(sockRemote));
    sockRemote.sin_family = AF_INET;
    sockRemote.sin_addr = ip_addr;
    sockRemote.sin_port = htons(remotePort);
    unsigned int socketSize = sizeof(sockRemote);

    while(1)
    {
        char *message = malloc(sizeof(char)*bufferLength);
        memset(message, 0, sizeof(message));
        //error 
        int bytesRecd = recvfrom(socketDescriptor, message, bufferLength, 
            0, (struct sockaddr* ) &sockRemote, &socketSize);
        if(bytesRecd < 0){
            printf("Error: message not received\n");
        }
        int terminateInd;
        if(bytesRecd < bufferLength){
            terminateInd = bytesRecd;
        }
        else
        {
            terminateInd = bufferLength --;
        }

        message[terminateInd] = 0;
        
        pthread_mutex_lock(&receiverMutex);
        int node = List_append(receiverList, message);
        if(node == 0){
            pthread_cond_signal(&receiverCondition);
        }
        else{
            printf("Received message cannot be displayed\n");
        }
        pthread_mutex_unlock(&receiverMutex);
    }
    return NULL;
}

void* f_printerThread(void* input)
{   
    Arguements* a = (struct Arguements*) input;
    int bufferLength = a->charBufferLength;

    while(1){
        
        pthread_mutex_lock(&receiverMutex);
        pthread_cond_wait(&receiverCondition, &receiverMutex); 
        
        if(List_count(receiverList) != 0){
            char *message = (char *) List_remove(receiverList);
            char exc[bufferLength];
            sprintf(exc, "!");
            int x=strcmp(message,exc);
            if(x==0)
            {
                pthread_cancel(receiverThread);
                pthread_cancel(printThread);
                pthread_t* t1 = a->sender;
                pthread_t* t2 = a->scanner;
                pthread_cancel(*t1);
                pthread_cancel(*t2);
                return NULL;
            }
            printf("%s\n", message);
            free(message);
        }
        pthread_mutex_unlock(&receiverMutex);
    }

    return NULL;
}

void receiver_create(Arguements* a)
{   
    receiverList = List_create();
    a->receiver = &receiverThread;
    a->printer = &printThread;
    pthread_create(&receiverThread, NULL, f_receiverThread, a);
    pthread_create(&printThread, NULL, f_printerThread, a);

    return;
}

void receiver_join()
{
    pthread_join(receiverThread, NULL);
    pthread_join(printThread, NULL);
    return;
}