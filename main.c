//CHIRAG : 26-02-26
//so idea goes like this
//1. i will create an event queue this bcz ot will help me extract the element eith least time in O(1) time
// and insertion would also hardly need log n time same for deletion
//2. then i will seed some initial events like the traffic light example i studied
//3. loop up 
    // -pick the smalles and 
    // -execute it then 
    // -handler may add new event to queue
//4. stop when queue is empty

#include <stdio.h>
#include "event.h"
// <event.h> → look in system/library folders
// "event.h" → look in your own project folder
#include "event_queue.h"

int main()
{

    //well crap what now? there is no priority queue in c
    //will have to do myself
    //maybe use tree in try remember... all i can think of is parent of a node is (i-1)/2, left node is 2i+1 and right is 2i+2
    //since i know that why not try with an array? maybe try 0 is an array use
    //well what if size becomes a problem? maybe i should consider dynamic array??

    return 0;
}