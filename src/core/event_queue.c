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
    // while(p1->data[tempsize].time<p1->data[((tempsize)-1)/2].time)
    //CHIRAG 12-03-26 00:14 :: okay so we need to update heapify condition as we have a change in while loop
    // so maybe condition becomes 
    while((p1->data[tempsize].time < p1->data[((tempsize)-1)/2].time)  
    || ((p1->data[tempsize].time == p1->data[((tempsize)-1)/2].time) 
    && (p1->data[tempsize].delta < p1->data[((tempsize)-1)/2].delta)) )
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
    // while(1)
    // {
    //     // if((2*i+1<q->size)&& q->data[i].time>q->data[2*i+1].time )
    //     if(
    //     (2*i+1<q->size) 
    //     && ( (q->data[i].time>q->data[2*i+1].time) 
    //     || ((q->data[i].time==q->data[2*i+1].time) && (q->data[i].delta>q->data[2*i+1].delta) ) 
    //     )  
    //     )
        
    //     {
    //         Event t= q->data[i];
    //         q->data[i]= q->data[2*i+1];
    //         q->data[2*i+1]= t;
    //         i= 2*i+1;
    //     }
    //     // else if((2*i+2<q->size) && q->data[i].time>q->data[2*i+2].time)
    //     else if(
    //     (2*i+2<q->size) 
    //     && ( (q->data[i].time>q->data[2*i+2].time) 
    //     || ((q->data[i].time==q->data[2*i+2].time) && (q->data[i].delta>q->data[2*i+2].delta) ) 
    //     )  
    //     )
        
    //     {
    //         Event t= q->data[i];
    //         q->data[i]= q->data[2*i+2];
    //         q->data[2*i+2]= t;
    //         i= 2*i+2;    
    //     }
    //     else
    //     break;
    // }
    // CHIRAG 17-03-26 04:35 :: okay so this was a nasty one.... took almost 2 hours to track down
    // the flip flop was going into infinite loops and events were coming out of the heap in wrong order
    // like t=2 event appearing after t=3 which makes zero sense for a min heap
    //
    // what we tried first:
    // 1. thought it was prev_value not initialised.... fixed that, helped a bit but loop remained
    // 2. thought it was last_change_on not being set in sequential.c.... fixed that too
    // 3. thought it was last_change_delta check missing.... added that field to Signal struct
    // 4. tried static clk_handled flag to stop repeated firing.... got messy
    // 5. tried inserting Q events into queue from dff_logic.... caused its own infinite loop
    // 6. simplified dff to just set Q directly.... still had heap corruption underneath
    //
    // root cause turned out to be in extract_min heapify down all along....
    // old code checked left child first, swapped if smaller than parent, then checked right child
    // bug: if left < parent but right < left.... we swapped with left anyway
    // this means the minimum child wasnt always being chosen.... heap property slowly broke down
    // over multiple extractions the ordering got corrupted silently
    //
    // fix: changed heapify down to first find the smallest among parent, left, right
    // then only swap if smallest is not the parent.... classic correct heap implementation
    // after this fix events came out in perfect (time, delta) order and flip flop worked first try
    //

    while(1)
    {
        int smallest = i;
        int left = 2*i+1;
        int right = 2*i+2;

        // check if left child is smaller than current smallest
        if(left < q->size && 
           (q->data[left].time < q->data[smallest].time ||
           (q->data[left].time == q->data[smallest].time && 
            q->data[left].delta < q->data[smallest].delta)))
            smallest = left;

        // check if right child is smaller than current smallest
        if(right < q->size && 
           (q->data[right].time < q->data[smallest].time ||
           (q->data[right].time == q->data[smallest].time && 
            q->data[right].delta < q->data[smallest].delta)))
            smallest = right;

        if(smallest != i)
        {
            Event t = q->data[i];
            q->data[i] = q->data[smallest];
            q->data[smallest] = t;
            i = smallest;
        }
        else
            break;
    }
    return temp;
}