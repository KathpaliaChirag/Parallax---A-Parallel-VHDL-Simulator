//CHIRAG : 26-02-26
//okay so event_queue.h was the header file to tell what exists
//this file tells how part...
// idea behind eventqueue.c
// 1. it will be storing basic initialisation of queue init_queue
// 2. it will also be responsible to inclrease capacity when its close to full
// 3. maybe add and remove events
#include "event_queue.h"
#include <stdio.h>
#include<stdlib.h> //used for malloc
EventQueue* init_queue()
{
    // just had some doubt about syntax so took help of internet
    // was just thinking whats need of (EventQueue *)before malloc
    //technically malloc returns a void ptr so it should be fine to write something like :
    //EventQueue *p1 = malloc(sizeof(EventQueue));
    //so... well i guess we know ... i was right XD
    // we can use it but in old c like c99 it wasn't the case ... modern c do this coversion auto
    // The cast tells the compiler "treat this memory as a pointer to EventQueue" or "pointer to Event".

    //interestingly this cast is about mapping...as in i could do a void ptr too but how would the copiler understand how much of memory to allocate ?
    //like if i do p->size =0... it wouldn't know...why or till where?
    
    EventQueue *p1 = (EventQueue *)malloc(sizeof(EventQueue));
    p1->data= (Event *) malloc(sizeof(Event)*10);
    p1->size =0;
    p1->capacity =10;
    return p1;
}
