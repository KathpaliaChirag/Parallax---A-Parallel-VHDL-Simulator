//CHIRAG 12-03-26 02:31 ::
// starting off will add some implemenatations here of 5 functions from .h file
#include <stdio.h>
#include "delta.h"
double current_time;
int delta;
int changed;

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