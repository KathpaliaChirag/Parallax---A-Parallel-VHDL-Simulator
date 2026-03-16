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
#include "./src/core/delta.h"
#include "./src/core/delta.c"
#include"./src/sim/sequential.h"
#include"./src/sim/sequential.c"
void my_run(void)
{
    printf("\nHello world by CK-A\n");
}
void my_run1(void)
{
    printf("\nHello world by CK-B\n");
}
void my_run2(void)
{
    printf("\nHello world by CK-C\n");
}
int main()
{

    // //well crap what now? there is no priority queue in c
    // //will have to do myself
    // //maybe use tree in try remember... all i can think of is parent of a node is (i-1)/2, left node is 2i+1 and right is 2i+2
    // //since i know that why not try with an array? maybe try 0 is an array use
    // //well what if size becomes a problem? maybe i should consider dynamic array??
    // // okay so update after making event_queue.c... we have some function to make a queue, add elements into it and extract min
    // //i feel i should add time stramp of comments too... may get confusing later on
    // //-----------------------------------------------------------
    // //CHIRAG 12-03-26- 2:58 :: testing time and delta here 
    // init_run();
    // print_state();
    // advance_delta();
    // print_state();
    // advance_time();
    // print_state();
    // //-------------------------------------------------------------
    
    // //----------- Test for event add/remove manual-------------
    // EventQueue temp = init_queue();
    // Event e;
    // // e.time= 1;
    // e.type= 1;
    // Event f;
    // // f.time= 3;
    // f.type= 3;
    // Event g;
    // // g.time= 2;
    // g.type= 2;
    // // CHIRAG 12-03-26 : 00:33 :: okay so have added the time and deltas and testing how ot works now 
    // e.time = 1; e.delta = 0;
    // g.time = 1; g.delta = 1;
    // f.time = 1; f.delta = 2;

    // insert_ele(&temp, e);
    // insert_ele(&temp, f);
    // insert_ele(&temp, g);
    // Event test = extract_min(&temp);
    // printf("time : %f delta : %d\n", test.time, test.delta);
    
    // test = extract_min(&temp);
    // printf("time : %f delta : %d\n", test.time, test.delta);
    
    // test = extract_min(&temp);
    // printf("time : %f delta : %d\n", test.time, test.delta);
    // //------------------------------------------------

    // //CHIRAG 28-02-26 (17:07)
    // //since i already added the signals here is a sorta test we will play on signals and see if all works well for now
    // //---------Test for signal add remove manual ----------
    // Signal *temp2 = init_signal("morphy");
    // update_signal(temp2, 10,2);
    // int t2= get_signal_value(temp2);
    // double t3=get_signal_last_change(temp2);
    // printf("%d \n%f\n", t2, t3);
    // //-----------------------------------------------------

    // //-----------------------------------------------------
    // //CHIRAG 10-03-25 (13:05) :
    // //okay so since i added process implementation now its time to write up a static test and try to check it 
    // Process p= process_init("murphy", my_run);
    // process_add_signal(&p, *temp2);
    // process_print_signals(&p);

    // // CHIRAG 11-03-26 21:50 : IDEA is to simply make a dummy my run function that prints hello world
    // // then scheduler_init and ceate empty schduler 
    // // add the dummy process murphy to it which wakes up my_run function to print hello world
    // // add process to signal, and schduler add that process then we change signal and it should call sheduler notify 
    // // maybe where we update signal value there itself we can add scheduler notify  
    // printf("\n \n \n THE SCHEDULER TEST STARTS HERE");
    // Scheduler sch= scheduler_init();
    // scheduler_add_process(&sch, p);
    
    // scheduler_notify(&sch, *temp2);

    // CHIRAG 16-03-2026 22:27 
    /* 
    lets test the simulation :
     1. init delta state
     2. create signal A
     3. create event targeting A
     4. create queue, insert event
     5. create DynArray_Signal, insert A
     6. create process watching A
     7. create scheduler, add process
     8. run_simulation(&queue, &sch, &signals)
    */
   // CHIRAG 16-03-26 22:27 :: first real simulation test.... signal A changes at t=1,d=0 and process A should wake up
    init_run(); // start everything at time=0, delta=0, changed=0

    // create signal A .... this is what the process will be watching
    Signal* A = init_signal("A");
    Signal *B = init_signal("B");
    Signal* C = init_signal("C");

    // seed the event queue with one event .... A changes to value 1 at time=1, delta=0
    Event e;
    e.signal_name = A->name; // which signal this targets
    e.new_value = 1;         // what value it changes to
    e.time = 1;              // when it happens
    e.delta = 0;             // first delta at this time
    e.type = 0;

    Event f;
    f.signal_name = B->name; // which signal this targets
    f.new_value = 1;         // what value it changes to
    f.time = 1;              // when it happens
    f.delta = 1;            
    f.type = 0;

    Event g;
    g.signal_name = C->name; // which signal this targets
    g.new_value = 1;         // what value it changes to
    g.time = 2;              // when it happens
    g.delta = 0;            
    g.type = 0;

    // queue it up
    EventQueue EQ = init_queue();
    insert_ele(&EQ, e);
    insert_ele(&EQ, g);
    insert_ele(&EQ, f);

    // signals list .... run_simulation needs this to look up signals by name
    DynArray_Signal signals;
    DYNARRAY_INIT(signals)
    DYNARRAY_INSERT(signals, *A)
    DYNARRAY_INSERT(signals, *B)
    DYNARRAY_INSERT(signals, *C)

    // process A watches signal A .... wakes up and calls my_run when A changes
    Process p = process_init("A", my_run);
    Process Q = process_init("B", my_run1);
    Process R = process_init("C", my_run2);
    process_add_signal(&p, *A);
    process_add_signal(&Q, *B);
    process_add_signal(&R, *C);

    // scheduler holds all processes .... notifies them when signals change
    Scheduler sch = scheduler_init();
    scheduler_add_process(&sch, p);
    scheduler_add_process(&sch, Q);
    scheduler_add_process(&sch, R);

    // lets gooo
    run_simulation(&EQ, &sch, &signals);
    return 0;
}