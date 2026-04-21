// CHIRAG 18-03-26 : 18:55 :: trace module.... idea is simple
// every time a signal changes we log it as (time, name, value)
// at end of simulation we hash the entire log into one number
// later when we build parallel simulator we compare hashes
// sequential hash == parallel hash means results are identical.... correctness verified
// different hash means bug somewhere in parallel implementation
#ifndef TRACE_H
#define TRACE_H
#include <stdio.h>
#include "../core/signal.h"
#include "../core/utils.h"


// one log entry.... a single signal change event
typedef struct {
    double time;        // when it happened
    char* signal_name;  // which signal changed
    int value;          // what value it changed to
} signal_change;

// dynamic array of signal changes.... grows as simulation runs
DYNARRAY_TYPE(signal_change)

// global log.... lives in trace.c, declared here so other files can see it
extern DynArray_signal_change trace_log;

void trace_init();                          // set up empty log
void trace_record(Signal s, double time);   // add a change to log
unsigned int trace_hash();                  // hash entire log → one number

#endif