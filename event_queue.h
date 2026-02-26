//CHIRAG : 26/2/26
//something that holds events is event quueue which is defined here

#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H
#include "event.h"

typedef struct{
    int size;
    int capacity;
    Event *data;
} EventQueue;

#endif