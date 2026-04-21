//CHIRAG 12-03-26 02:31 ::
// starting off will add some implemenatations here of 5 functions from .h file
#include <stdio.h>
#include "delta.h"
double current_time;
int delta;
int changed;
// CHIRAG 20-04-26 :: stat counter default/start values... declared extern in delta.h
// initialized to 0 ... reset happens implicitly at program start
// no need to reset between runs ... each parser_test.exe is one simulation run
int stat_delta_count = 0;
int stat_event_count = 0;
int stat_process_firings = 0;
int stat_max_delta_depth = 0;
double stat_last_timestamp = 0.0;
void init_run()
{
    current_time=0.0;
    delta =0;
    changed=0;
}

void advance_delta()
{
delta ++;
changed =0;
// CHIRAG 20-04-26 :: count every delta cycle that advances
// this is where we know a delta actually happened ... signal changed ... processes ran
// stat_max_delta_depth tracks deepest delta at any single timestamp
// why track max depth? ... tells us circuit feedback complexity ... chainof4 goes 4 deep
stat_delta_count++;
if(delta > stat_max_delta_depth)
    stat_max_delta_depth = delta;
}

void advance_time(EventQueue* q)
{
    // current_time++;
    printf("advancing time to %f\n", q->data[0].time);
    current_time = q->data[0].time;
    delta =0;
    changed=0;
}
int detect_change()
{
    return changed;
}
void print_state()
{
    printf("current time is %f, delta is %d \n", current_time, delta);
}