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
#include "event_queue.h"
extern double current_time;
extern int delta;
extern int changed;
// well just doing definitions here and implementation as usual goes to .c
void init_run();
void advance_delta();
void advance_time(EventQueue * q);
int detect_change();
void print_state();

// CHIRAG 20-04-26 :: okay so now i introduce a new way to measue stats... is what we call stats counters ...
// idea was to track what the sim actually did... 
// idea ... after simulation ends we want to know how much the circuit worked
// problem ... right now sim prints nothing useful ... just final values and hash
// we have no idea how many delta cycles ran ... how many events fired ... nothing

// solution ... three global counters updated inside sim loop
// stat_delta_count ... incremented every advance_delta() call
// stat_event_count ... incremented every extract_min() that actually applies to a signal
// stat_process_firings ... incremented every p.run() call

// why globals in delta.h? ... current_time and delta already live here
// sim state all in one place ... sequential.c and parallel.c both include delta.h
// so both can update counters without any extra includes

// also three was this alternative I considered ... pass a Stats struct pointer into run_simulation()
// rejected it...cause i felt it  would require changing function signatures everywhere ... too invasive
// globals are fine here ... simulation is single-instance anyway
extern int stat_delta_count;
extern int stat_event_count;
extern int stat_process_firings;
extern int stat_max_delta_depth;
extern double stat_last_timestamp;
#endif