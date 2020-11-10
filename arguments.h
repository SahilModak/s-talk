#ifndef _ARGUMENTS_H_
#define _ARGUMENTS_H_
#include <stdbool.h>
#include <pthread.h>

typedef struct inpArgs Arguements;
struct inpArgs{
    char* name;
    int myPort;
    int remotePort;
    int charBufferLength;
    pthread_t* receiver;
    pthread_t* sender;
    pthread_t* printer;
    pthread_t* scanner;
};

#endif