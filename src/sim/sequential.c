// CHIRAG 16-01-26 20:59 ::
// okay so idea goes like this we need to keep increasing timne till there is something in queue to execute
// if there is nothing then we move on and say we are done
// at each event we keep increasing delta as long as changed ==1 is there
// so  extract all events ate current rtime and current delta
// execute them all and wake process if any 
// lkeep increasing as long as it needs to be 
// if yees advance delta
// if no stable break and incease time 
#include "sequential.h"
#include "../core/scheduler.h"
#include "../core/delta.h"
// #include "scheduler.c"

void run_simulation(EventQueue* p, Scheduler* sch, DynArray_Signal* signal)
{
    while(p->size)
    {
        do
        {
            // int  t =p->data->time;
            // int d = p->data->delta;
            print_state();
            while(p->size > 0 &&  p->data[0].time == current_time &&  p->data[0].delta == delta)
            {
                // print_state();
                Event e = extract_min(p);
                // execute e — wake up processes via scheduler
                for(int i=0 ; i < signal->size; i++)
                if(strcmp(signal->data[i].name, e.signal_name) == 0)
                {
                signal->data[i].value= e.new_value;

                scheduler_notify(sch, signal->data[i]);
                changed=1;
                }
                
            }
            if(changed)
            advance_delta();
            
            
        } while(detect_change());
        // advance_time(p); // CHIRAG 16-03-26 22:47 :: found a bug makes it run ifdeinately
        // if(p->size > 0)
        //     advance_time();
        // CHIRAG 16-03-26 23:15 :: okay so ran into a nasty infinite loop bug here.... took a while to figure out
// problem was advance_time() was running every iteration even when there were still events at current_time
// so it would jump to next event time, find no events at new delta, changed stays 0, do-while exits...
// but then advance_time runs AGAIN setting current_time to same event time since queue didnt move.... infinite
// fix was simple in hindsight.... only advance time when next event in queue is actually at a FUTURE time
// i.e. p->data[0].time > current_time .... that way we dont skip ahead prematurely
// lesson learned.... always check boundary conditions on time advancement in event driven sims

        if(p->size > 0 && p->data[0].time > current_time)
            advance_time(p);
    }
}