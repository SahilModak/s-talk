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
#include "arguments.h"
#include "sender.h"
#include "list.h"





pthread_t senderThread;
pthread_t scanThread;
static pthread_cond_t senderCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t senderMutex = PTHREAD_MUTEX_INITIALIZER;
List* senderList;



void* f_senderThread(void* input){
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

    while(1)
    {   

        unsigned int socketSize = sizeof(sockRemote);

        pthread_mutex_lock(&senderMutex);
        pthread_cond_wait(&senderCondition, &senderMutex); 

        if(List_count(senderList) != 0){
            char * message = (char *) List_remove(senderList);
            int bytesSent = sendto(socketDescriptor, message, strlen(message), 
            0, (struct sockaddr* ) &sockRemote, socketSize);
            if(bytesSent == -1){
                printf("Error: Message not send\n");
            }

            char exc[bufferLength];
            sprintf(exc, "!");
            int x=strcmp(message,exc);
            if(x==0)
            {   
                pthread_t* t1 = a->receiver;
                pthread_t* t2 = a->printer;
                pthread_cancel(*t1);
                pthread_cancel(*t2);
                pthread_cancel(senderThread);
                pthread_cancel(scanThread);
                return NULL;
            }

        }
        pthread_mutex_unlock(&senderMutex);


    }
    return NULL;
}


void* f_scannerThread(void* input)
{   
    Arguements* a = (struct Arguements*) input;
    int bufferLength = a->charBufferLength;
    while(1){
        char message[bufferLength];
        scanf("%[^\n]%*c", message);
        pthread_mutex_lock(&senderMutex);
        int node = List_append(senderList, &message);
        if(node == 0){
            pthread_cond_signal(&senderCondition);
        }
        else{
            printf("Entered message could not be sent\n");
        }
        pthread_mutex_unlock(&senderMutex);
    }
    return NULL;
}


void sender_create(Arguements* a)
{
    senderList = List_create();
    a->sender = &senderThread;
    a->scanner = &scanThread;
    pthread_create(&senderThread, NULL, f_senderThread, a);
    pthread_create(&scanThread, NULL, f_scannerThread, a);
    return;
}

void sender_join()
{
    pthread_join(senderThread, NULL);
    pthread_join(scanThread, NULL);
    return;
}
