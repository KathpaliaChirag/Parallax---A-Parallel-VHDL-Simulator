// CHIRAG : 16-03-26 ::
//okay so seq execution would need it all events, queue, signal, process, delta scheduler so we include all here
// we also define the function to run simulation
#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H
#include "../core/event.h"
#include "../core/event_queue.h"
#include "../core/signal.h"
#include "../core/process.h"
#include "../core/scheduler.h"
#include "../core/delta.h"
// CHIRAG 16-03-26 : 21:07 ::
//  never include .c files in a header - only .h files in headers. .c files are compiled separately and linked together.
//  Including .c files causes duplicate definition errors.
// #include "event.c"
// #include "event_queue.c"
// #include "signal.c"
// #include "process.c"
// #include "scheduler.c"
// #include "delta.c"

void run_simulation(EventQueue*, Scheduler*, DynArray_Signal* );

#endif