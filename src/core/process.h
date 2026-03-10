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
DYNARRAY_TYPE(Signal)
// typedef DynArray_Signal signals
typedef struct {
    int id;
    char *name;
    DynArray_Signal senstivity_list;
    void (*run)(void);
} Process;
#endif