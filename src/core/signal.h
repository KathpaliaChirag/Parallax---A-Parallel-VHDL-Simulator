//CHIRAG : 28-02-2026 (15:50) 
//so basically idea os to define a signal thats also the 1st goal of day
//we need 4 things in signal for now may add more later
// ideally we are storing name, value, type, lastchange time

#ifndef SIGNAL_H
#define SIGNAL_H
#include "utils.h"
#include <stdlib.h>
typedef struct
{
int value;
char* name;
double last_change_on;
int last_change_delta;
int type;
int prev_value; //CHIRAG -17-03-26 : 1:35 ::  was just thinking and realised that we would sorta need prev value to implement clock 
// say i wanna make a D Flipflop now the thing is D FF will change output only when clock goes 0 to 1 so yeah we need it  
char vcd_symbol; //CHIRAG : 17-03-26: 14:40 ::  every signal will have a symbol in the vcd file thus adding it to list
int value_next; // CHIRAG 02-04-26  08:20 :: added simply put for the idea of parallel execution needing to store updates somewhere
// CHIRAG 02-04-26 :: value_next is the double buffer
// during a delta cycle all processes write here instead of value directly
// at delta boundary sequential.c sweeps value_next into value for every signal
// this means all processes read frozen state during evaluation - correct VHDL semantics
// and under parallelism threads never conflict on reads vs writes
} Signal;
DYNARRAY_TYPE(Signal) //added here as a part of design choice CHIRAG-10-03-26 12:26
Signal* init_signal(char* name);
void update_signal(Signal* s, int val, double time);
int get_signal_value(Signal* s);
double get_signal_last_change(Signal* s);
//CHIRAG 02-04-26 :: added function definition to update signal
int signal_apply_updates(DynArray_Signal* signals);
#endif
//interestingly I also got to know that there is this unsaid convention in C to name your struct starting with a capital letter
//it does not affect the code but helps to understand in a good code that 'Signal' is a type and not a name

