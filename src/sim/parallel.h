// CHIRAG : 13-04-26 ::
//okay so now our parallel engine would be same maybe have a bit ore on the end of dependecy graphs in it?

#ifndef PARALLEL_H
#define PARALLEL_H
#include "../core/event.h"
#include "../core/event_queue.h"
#include "../core/signal.h"
#include "../core/process.h"
#include "../core/scheduler.h"
#include "../core/delta.h"
// added graph.h for for Dependency graphs
#include "../analysis/graph.h"
// HAHA NOT DOING SMAE MISTAKE TO ADD .c FIILE Sickkkk.....

void run_parallel_simulation(EventQueue*, Scheduler*, DynArray_Signal*, DepGraph* );

#endif