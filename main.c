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
#include"./src/output/vcd.h"
#include"./src/output/vcd.c"
// #include "../output/trace.h"
// #include "../output/trace.c"
#include "./src/output/trace.h"
// #include "./src/output/trace.c"
// #include"./src/sim/sequential.c"
#include "./src/output/trace.h"
#include "./src/output/trace.c"
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

Signal *X, *Y, *Z, *D, *Q, *CLK;
EventQueue *dff_queue;
void adder(int ctx_idx)
{
    print_state();
    // printf("\n Sum is " (X->value&Y->value));
    // CHIRAG 17-03-26: 15:03 :: okay so Z output of AND gate was not showing in waveform.... stayed 0 always
    // reason was adder() was computing X&Y and printing it but never actually updating Z->value
    // and since vcd_write_change only gets called from sequential.c when an event updates a signal
    // Z never had an event.... so VCD never recorded any Z change.... flatline on GTKWave
    //
    // TEMPORARY FIX : directly set Z->value = X->value & Y->value inside adder()
    // and manually call vcd_write_change(*Z, current_time) from there
    // this works for the demo but bypasses the event system entirely
    // Z update is not going through the queue.... no event, no delta ordering for Z
    //
    // PROPER FIX FOR LATER : adder() should insert a new Z event into the queue
    // same pattern as dff_logic.... need a global adder_queue pointer
    // then sequential.c picks it up, updates Z properly, calls vcd_write_change automatically
    // this way Z participates fully in delta cycle ordering like a real circuit output should
    // for now the demo works and waveform is correct so moving on
     Z->value = X->value & Y->value;
    vcd_write_change(*Z, current_time);
    printf("AND: X=%d Y=%d Z=%d\n", X->value, Y->value, X->value & Y->value);
}

// void dff_logic(void)
// {
//     if(CLK->prev_value==0 && CLK->value==1)
//     Q->value= D->value;
// }

void dff_logic(int ctx_idx) {
    if(CLK->prev_value == 0 && CLK->value == 1 
       && CLK->last_change_on == current_time
       && CLK->last_change_delta == delta) 
       {
        Q->value = D->value;
        //Tempoary fix added to resolve it for now
        vcd_write_change(*Q, current_time);
        printf("DFF triggered: D=%d Q=%d at t=%f d=%d\n", 
               D->value, Q->value, current_time, delta);
    }
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
    // init_run(); // start everything at time=0, delta=0, changed=0

    // // create signal A .... this is what the process will be watching
    // Signal* A = init_signal("A");
    // Signal *B = init_signal("B");
    // Signal* C = init_signal("C");

    // // seed the event queue with one event .... A changes to value 1 at time=1, delta=0
    // Event e;
    // e.signal_name = A->name; // which signal this targets
    // e.new_value = 1;         // what value it changes to
    // e.time = 1;              // when it happens
    // e.delta = 0;             // first delta at this time
    // e.type = 0;

    // Event f;
    // f.signal_name = B->name; // which signal this targets
    // f.new_value = 1;         // what value it changes to
    // f.time = 1;              // when it happens
    // f.delta = 1;            
    // f.type = 0;

    // Event g;
    // g.signal_name = C->name; // which signal this targets
    // g.new_value = 1;         // what value it changes to
    // g.time = 2;              // when it happens
    // g.delta = 0;            
    // g.type = 0;

    // // queue it up
    // EventQueue EQ = init_queue();
    // insert_ele(&EQ, e);
    // insert_ele(&EQ, g);
    // insert_ele(&EQ, f);

    // // signals list .... run_simulation needs this to look up signals by name
    // DynArray_Signal signals;
    // DYNARRAY_INIT(signals)
    // DYNARRAY_INSERT(signals, *A)
    // DYNARRAY_INSERT(signals, *B)
    // DYNARRAY_INSERT(signals, *C)

    // // process A watches signal A .... wakes up and calls my_run when A changes
    // Process p = process_init("A", my_run);
    // Process Q = process_init("B", my_run1);
    // Process R = process_init("C", my_run2);
    // process_add_signal(&p, *A);
    // process_add_signal(&Q, *B);
    // process_add_signal(&R, *C);

    // // scheduler holds all processes .... notifies them when signals change
    // Scheduler sch = scheduler_init();
    // scheduler_add_process(&sch, p);
    // scheduler_add_process(&sch, Q);
    // scheduler_add_process(&sch, R);

    // // lets gooo
    // run_simulation(&EQ, &sch, &signals);

    //-----------Simulation of AND Hardcoded starts here for sanity check -------------
    // CHIRAG : 16-03-2026 - 23:35 ::
    init_run(); // step 1 : starts simulation
    
    // // initialise signals of input and output 
    X = init_signal("input-X");
    Y = init_signal("input-Y");
    Z = init_signal("Output-Z");

    // two events
    Event I1;
    I1.signal_name = X->name;
    I1.new_value= 1;
    I1.time=1;
    I1.delta=0;
    I1.type=0;
    
    EventQueue Adder= init_queue();
    insert_ele(&Adder, I1);
    Event I2;
    I2.signal_name = Y->name;
    I2.new_value= 1;
    I2.time=1;
    I2.delta=5;
    I2.type=0;
    insert_ele(&Adder, I2);
    I1.new_value= 0;
    I1.time=3;
    I1.delta=2;
    // I1.type=0;
    insert_ele(&Adder, I1);
    I1.new_value= 1;
    I1.time=5;
    I1.delta=2;
    // I2.type=0;
    insert_ele(&Adder, I1);
    // updated each change in event queue
    //made sestivity list of signals

    DynArray_Signal Adder_sig;
    DYNARRAY_INIT(Adder_sig);
    DYNARRAY_INSERT(Adder_sig, *X);
    DYNARRAY_INSERT(Adder_sig, *Y);
    DYNARRAY_INSERT(Adder_sig, *Z);
    // CHIRAG 17-03-26 00:20 :: subtle pointer bug.... took a moment to figure out
    // when we do DYNARRAY_INSERT(Adder_sig, *X) it inserts a COPY of the signal struct
    // so now there are two separate copies in memory.... X pointer and Adder_sig.data[0]
    // run_simulation updates Adder_sig.data[0].value correctly when event fires
    // but adder() reads X->value which is the original copy.... still 0 forever
    // fix: after inserting, point X Y Z directly at the array entries
    // now X->value and Adder_sig.data[0].value are the SAME memory location
    // so when run_simulation updates the array, adder() sees it too
    X = &Adder_sig.data[0];
    Y = &Adder_sig.data[1];
    Z = &Adder_sig.data[2];

    Process adding = process_init("Input_add", adder, -1);
    process_add_signal(&adding, *X );
    process_add_signal(&adding, *Y );

    Scheduler adding_sch= scheduler_init();
    scheduler_add_process(&adding_sch, adding);
    vcd_init("output-AND.vcd");
    vcd_write_header(&Adder_sig);  // or &Dflipflop for dff test
    trace_init();
    run_simulation(&Adder, &adding_sch, &Adder_sig);
    vcd_close();
    printf("AND gate trace hash: %u\n", trace_hash());
    //--------------ADDER SIMULATION ENDS HERE-------------------

    //--------------D Flipflop --------------------------------
    // CHIRAG 17-03-26 : 3:15 ::
    // okay so idea goes here  will have 1 input D, output Q and a clk signal for clock
    // one process dff which will watch clk and D
    // in dff will chk if clock rose to 1 then will try for updating Q
    init_run();
    D   = init_signal("Input-D");
    Q   = init_signal("Output-Q");
    CLK = init_signal("CLOCK-CLK");
    
    // t=1: D=1 before clock rises, CLK rises → Q should capture 1
    EventQueue Change = init_queue();
    
    Event ev;
    
    // D goes to 1 slightly before CLK rises (same time, lower delta)
    ev.signal_name = D->name; ev.new_value = 1;
    ev.time = 1; ev.delta = 0; ev.type = 0;
    insert_ele(&Change, ev);
    
    // CLK rises at t=1, d=1 → Q should become 1
    ev.signal_name = CLK->name; ev.new_value = 1;
    ev.time = 1; ev.delta = 1; ev.type = 0;
    insert_ele(&Change, ev);
    
    // CLK falls at t=2
    ev.signal_name = CLK->name; ev.new_value = 0;
    ev.time = 2; ev.delta = 0; ev.type = 0;
    insert_ele(&Change, ev);
    
    // D goes to 0 at t=3 before CLK rises
    ev.signal_name = D->name; ev.new_value = 0;
    ev.time = 3; ev.delta = 0; ev.type = 0;
    insert_ele(&Change, ev);
    
    // CLK rises at t=3, d=1 → Q should become 0
    ev.signal_name = CLK->name; ev.new_value = 1;
    ev.time = 3; ev.delta = 1; ev.type = 0;
    insert_ele(&Change, ev);
    
    // CLK falls at t=4
    ev.signal_name = CLK->name; ev.new_value = 0;
    ev.time = 4; ev.delta = 0; ev.type = 0;
    insert_ele(&Change, ev);
    
    DynArray_Signal Dflipflop;
    DYNARRAY_INIT(Dflipflop);
    DYNARRAY_INSERT(Dflipflop, *D);
    DYNARRAY_INSERT(Dflipflop, *Q);
    DYNARRAY_INSERT(Dflipflop, *CLK);
    D   = &Dflipflop.data[0];
    Q   = &Dflipflop.data[1];
    CLK = &Dflipflop.data[2];
    
    Process Dffs = process_init("Dff", dff_logic, -1);
    //ctx is -1 bcz hardcoded functions do not use proc_contexts
    process_add_signal(&Dffs, *D);
    process_add_signal(&Dffs, *CLK);
    
    Scheduler Update = scheduler_init();
    scheduler_add_process(&Update, Dffs);
    dff_queue = &Change;
    printf("Queue contents:\n");
    for(int i=0; i<Change.size; i++)
        printf("  t=%f d=%d sig=%s val=%d\n", 
               Change.data[i].time, Change.data[i].delta,
               Change.data[i].signal_name, Change.data[i].new_value);
    vcd_init("output-DFF.vcd");
    vcd_write_header(&Dflipflop);  // or &Dflipflop for dff test
    trace_init();
    run_simulation(&Change, &Update, &Dflipflop);
    //CHIRAG : 18-03-26 : 20:35 :: trace calculation added after... run_simulation ...
    vcd_close();
    printf("Trace hash: %u\n", trace_hash());
    //---------------------------------------------------------
    return 0;
}