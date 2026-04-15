//CHIRAG : 09-03-2026 19:49 : this file is for the base header of process
// so again a process here is not the process of an os here a process means that we have a name, id, senstivitylist, function pointer to actually run it 
// so how do you store something which you will need to run later? you make a function pointer
// how?
// well you write       void (*run) (void)' 
// so what it means :
// function it points to returns void..... (*run) is a pointer and (void) at end means that it takes no arguments
// so basically ....run is a variable that stores which functions to call 
// as in later we can do 
// run = my function;
// run();

#ifndef PROCESS_H
#define PROCESS_H
#include "utils.h"
#include "signal.h"
// DYNARRAY_TYPE(Signal)
// typedef DynArray_Signal signals
typedef struct {
    int id;
    char *name;
    int ctx_idx; //CHIRAG 01-04-26 :: decided to fix it properly and added the int for process number 
    DynArray_Signal senstivity_list;
    void (*run)(int);
    // CHIRAG 15-04-26 :: notified flag added... tracks if process already woken this delta
    // current problem is that seq code updates twice say its an AND and A changes 
    // to 1 then B changes then update occures twice making hash doing duplicate work
    // why do we need this?
    // VHDL spec says a process wakes exactly once per delta cycle
    // even if multiple signals in its sensitivity list changed in same delta
    // without this flag ... process(A,B) with both A and B changing = fires twice
    // double eval = double trace_record = hash mismatch between seq and parallel
    // parallel already handles this correctly via goto next_process in triggered list
    // sequential needs this flag to match that behavior
    int notified;
} Process;
Process process_init(char* name, void (*run)(int),int ctx_idx);
void process_add_signal(Process* p, Signal s);
void process_print_signals(Process* p);
#endif