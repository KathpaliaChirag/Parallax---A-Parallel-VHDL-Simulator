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
#include "./src/core/event.h"
#include "./src/core/event_queue.h"
// <event.h> → look in system/library folders
// "event.h" → look in your own project folder
#include "./src/core/event_queue.c"
#include "./src/core/signal.h"
#include "./src/core/signal.c"
#include "./src/core/process.c"
#include "./src/core/process.h"
#include "./src/core/scheduler.h"
#include "./src/core/scheduler.c"
void my_run(void)
{
    printf("\nHello world by CK\n");
}
int main()
{

    //well crap what now? there is no priority queue in c
    //will have to do myself
    //maybe use tree in try remember... all i can think of is parent of a node is (i-1)/2, left node is 2i+1 and right is 2i+2
    //since i know that why not try with an array? maybe try 0 is an array use
    //well what if size becomes a problem? maybe i should consider dynamic array??
    // okay so update after making event_queue.c... we have some function to make a queue, add elements into it and extract min
    //i feel i should add time stramp of comments too... may get confusing later on
    //----------- Test for event add/remove manual-------------
    EventQueue temp = init_queue();
    Event e;
    e.time= 1;
    e.type= 1;
    Event f;
    f.time= 3;
    f.type= 3;
    Event g;
    g.time= 2;
    g.type= 2;
    insert_ele(&temp, e);
    insert_ele(&temp, f);
    insert_ele(&temp, g);
    Event test = extract_min(&temp);
    printf("%f\n", test.time);
    test = extract_min(&temp);
    printf("%f\n", test.time);
    //------------------------------------------------

    //CHIRAG 28-02-26 (17:07)
    //since i already added the signals here is a sorta test we will play on signals and see if all works well for now
    //---------Test for signal add remove manual ----------
    Signal *temp2 = init_signal("morphy");
    update_signal(temp2, 10,2);
    int t2= get_signal_value(temp2);
    double t3=get_signal_last_change(temp2);
    printf("%d \n%f\n", t2, t3);
    //-----------------------------------------------------

    //-----------------------------------------------------
    //CHIRAG 10-03-25 (13:05) :
    //okay so since i added process implementation now its time to write up a static test and try to check it 
    Process p= process_init("murphy", my_run);
    process_add_signal(&p, *temp2);
    process_print_signals(&p);

    // CHIRAG 11-03-26 21:50 : IDEA is to simply make a dummy my run function that prints hello world
    // then scheduler_init and ceate empty schduler 
    // add the dummy process murphy to it which wakes up my_run function to print hello world
    // add process to signal, and schduler add that process then we change signal and it should call sheduler notify 
    // maybe where we update signal value there itself we can add scheduler notify  
    printf("\n \n \n THE SCHEDULER TEST STARTS HERE");
    Scheduler sch= scheduler_init();
    scheduler_add_process(&sch, p);
    
    scheduler_notify(&sch, *temp2);
    return 0;
}