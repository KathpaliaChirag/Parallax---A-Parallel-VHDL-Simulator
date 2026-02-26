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
#include "event_queue.h"
// <event.h> → look in system/library folders
// "event.h" → look in your own project folder
#include "event_queue.c"

int main()
{

    //well crap what now? there is no priority queue in c
    //will have to do myself
    //maybe use tree in try remember... all i can think of is parent of a node is (i-1)/2, left node is 2i+1 and right is 2i+2
    //since i know that why not try with an array? maybe try 0 is an array use
    //well what if size becomes a problem? maybe i should consider dynamic array??
    // okay so update after making event_queue.c... we have some function to make a queue, add elements into it and extract min
    //i feel i should add time stramp of comments too... may get confusing later on
    EventQueue* temp = init_queue();
    Event e;
    e.time= 1;
    e.type= 1;
    Event f;
    f.time= 3;
    f.type= 3;
    Event g;
    g.time= 2;
    g.type= 2;
    insert_ele(temp, e);
    insert_ele(temp, f);
    insert_ele(temp, g);
    Event test = extract_min(temp);
    printf("%f", test.time);
    test = extract_min(temp);
    printf("%f", test.time);
    return 0;
}