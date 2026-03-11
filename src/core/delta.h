//CHIRAG 12-03-26 2:13 :: 
// okay so idea goes somewhat like this i have decided on making delta file hich takes 3 variables basically 
// 1 curret time, 2 current delta, 3 change flag
// all three ofc global variables
// what next i want is to make 5 functions in total , the initialization function, advance delta, advance time, 
// detect if anything changed, print all tings that change at a certain time with their deltas
#ifndef DELTA_H
#define DELTA_H
//declaring variables directly in a header is problematic - 
//if two files include delta.h, both get their own copy of 
// current_time, delta, and changed. They won't share the same state.
#include "process.h"
#include "event.h"
extern double current_time;
extern int delta;
extern int changed;
// well just doing definitions here and implementation as usual goes to .c
void init_run();
void advance_delta();
void advance_time();
int detect_change();
void print_state();
#endif