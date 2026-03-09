//CHIRAG : 26-02-26
//okay so event_queue.h was the header file to tell what exists
//this file tells how part...
// idea behind eventqueue.c
// 1. it will be storing basic initialisation of queue init_queue
// 2. it will also be responsible to inclrease capacity when its close to full
// 3. maybe add and remove events
//CHIRAG 09-03-26 
// so well i have decided to change the static implemenatation of event queue to marcos type DYNARRAY so everywhere EventQueue becomes DynArray_event
// \ or i can do a simple rename well thats better actually will do change name in .h file

#include "event_queue.h"
#include <stdio.h>
#include<stdlib.h> //used for malloc
#include "utils.h"
// CHIRAG : 09-03-26 18:57 --------------
//idea is to relace this whole with new marcos implementation
EventQueue init_queue()
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

    // EventQueue *p1 = (EventQueue *)malloc(sizeof(EventQueue));
    // p1->data= (Event *) malloc(sizeof(Event)*10);
    // p1->size =0;
    // p1->capacity =10;

    //in new implementation marco simply replaces a part of code 
    EventQueue p1;
    DYNARRAY_INIT(p1)
    return p1;
}
//---------------------------------------------------------------


//CHIRAG 26-02-26
/*idea of insert ele goes like __this
we first will be making a void function, taking 2 par queue and event e
then we will add at the sie and increment size and then do heapify
*/ 
void insert_ele(EventQueue* p1, Event e)
{
    //CHIRAG 09-03-26 19:20 : Updated it and added marcos call
    DYNARRAY_INSERT((*p1), e)
    int tempsize = p1->size-1;
    // p1->data[p1->size]=e;
    // p1->size++;
    //heapify starts here basically if its smaller than parent swap its place
    while(p1->data[tempsize].time<p1->data[((tempsize)-1)/2].time)
    {
        Event temp = p1->data[tempsize];
        p1->data[tempsize] = p1->data[(tempsize-1)/2];
        p1->data[(tempsize-1)/2] = temp;
        tempsize = (tempsize-1)/2;
        if(tempsize ==0)
        break;

    }

    return ;
}

//CHIRAG 26-02-26
// next function i might need would be extract minimum
// idea goes as follows ....
// 1. extract root and save it,
// 2. element at last index i will move to root
// 3. then shrink size
// 4. lastly i will simple heapify it down as long as it goes... maybe it might need more thought like what if both childern are same...maybe then we always pick left one

Event extract_min(EventQueue *q)
{
    Event temp = q->data[0];
    q->data[0]= q->data[q->size-1];
    q->size--;
    int i =0;
    while(1)
    {
        if((2*i+1<q->size)&& q->data[i].time>q->data[2*i+1].time)
        {
            Event t= q->data[i];
            q->data[i]= q->data[2*i+1];
            q->data[2*i+1]= t;
            i= 2*i+1;
        }
        else if((2*i+2<q->size) && q->data[i].time>q->data[2*i+2].time)
        {
            Event t= q->data[i];
            q->data[i]= q->data[2*i+2];
            q->data[2*i+2]= t;
            i= 2*i+2;    
        }
        else
        break;
    }
    return temp;
}