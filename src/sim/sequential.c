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
#include "../output/vcd.h"
// #include "scheduler.c"
#include "../output/trace.h"
// #include "../output/trace.c"/
#include "../core/signal.h"
#include <string.h>

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
                // CHIRAG 02-04-26 :: write to value_next not value directly
                // processes will read frozen value during this delta
                // value_next holds the pending update until delta boundary
                // signal->data[i].prev_value = signal->data[i].value;
                // signal->data[i].value= e.new_value;
                signal->data[i].value_next= e.new_value;
                signal->data[i].last_change_delta = delta; 
                signal->data[i].last_change_on = current_time;
                    
                // CHIRAG 17-03-26 : will add VCD Change here 
                // vcd_write_change();
                vcd_write_change(signal->data[i], current_time);
                trace_record(signal->data[i], current_time);
                // scheduler_notify(sch, signal->data[i]);
                changed=1;
                }
                
            }
            // CHIRAG 02-04-26 :: delta boundary sweep
            // all processes have run for this delta, now apply pending writes
            // value_next becomes value for every signal that changed
            // CHIRAG 02-04-26 :: swap value_next into value BEFORE notifying processes
            // so when processes run they see correct new value and correct prev_value
            signal_apply_updates(signal);
            // CHIRAG 02-04-26 :: notify AFTER swap — processes see stable correct state
            for(int i=0; i < signal->size; i++)
                if(signal->data[i].value != signal->data[i].prev_value)
                {
                    scheduler_notify(sch, signal->data[i]);
                     // CHIRAG 02-04-26 :: reset prev_value after notify so next delta
                     // doesn't re-notify for same change
                     signal->data[i].prev_value = signal->data[i].value;
                }
            // CHIRAG 15-04-26 :: reset notified flags after all processes have been woken
            // must reset here so next delta starts fresh ... every process can be woken again
            // if we dont reset ... process only ever fires once in entire simulation
            for(int i = 0; i < sch->process_ARRAY.size; i++)
                sch->process_ARRAY.data[i].notified = 0;
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

        // if(p->size > 0 && p->data[0].time > current_time)
        //     advance_time(p);
        // CHIRAG 17-03-26 00:08 :: another subtle infinite loop.... took a while to spot this one
        // problem was after processing event at (t=1, d=0) we advance_delta so delta becomes 1
        // do-while exits correctly since changed=0.... but then advance_time check fails
        // because next event is at time=1 same as current_time so condition p->data[0].time > current_time is false
        // so we never advance time OR delta.... outer while loops forever stuck at (t=1, d=1)
        // fix: if next event is at same time but higher delta.... just jump delta forward directly
        // lesson: time advancement alone isnt enough.... need to handle delta jumps within same timestamp too
        if(p->size > 0)
            printf("next event: t=%f d=%d\n", p->data[0].time, p->data[0].delta);
        // if(p->size > 0 && p->data[0].time > current_time)
        //     advance_time(p);
        // else if(p->size > 0 && p->data[0].delta > delta)
        //    delta = p->data[0].delta; // jump delta forward to next event's delta
        if(p->size > 0 && p->data[0].time > current_time)
            advance_time(p);
        else if(p->size > 0 && p->data[0].time == current_time 
                && p->data[0].delta > delta)
            delta = p->data[0].delta;
    }
}