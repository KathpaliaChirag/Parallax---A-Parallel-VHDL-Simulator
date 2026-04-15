// CHIRAG 13-04-26 :: parallel.c
// okay so this is the parallel version of sequential.c
// the outer structure is IDENTICAL as i have kept in sequential.c ... same while loop, same do/while delta loop
// same event extraction, same signal_apply_updates, same notify
//
// the ONE thing that changes is how processes get executed
// so I have taken help of AI in discussion and understanding of this part of project... convo is more about discussion 
// sequential.c ... scheduler_notify calls p.run() one by one ... one thread
// parallel.c .... we build a triggered list ... group by color ... run each group with openmp
//
// my idea is more or less acquired from the iddea of independent sets and stuff....
// so one of my assignments in parallel prog has this idea to be implemented where i had to find clique of with max profit in a graph and 
// why color groups? well here is how it should work 
// processes in same color = no shared signals = safe to run simultaneously
// processes in different colors = might share signals = must run in separate batches
//
// so flow per delta is:
// 1. extract events ... write value_next ... same as sequential
// 2. signal_apply_updates ... swap ... same as sequential  
// 3. find triggered processes ... build list
// 4. for each color batch ... omp parallel for ... run all processes in batch simultaneously
// 5. advance delta ... same as sequential

#include "parallel.h"
#include "../core/scheduler.h"
#include "../core/delta.h"
#include "../output/vcd.h"
#include "../output/trace.h"
#include "../core/signal.h"
#include "../analysis/graph.h"
#include <string.h>
#include <omp.h>

// CHIRAG 13-04-26 :: per thread local event queues
// this is critical ... without this all threads would write to same global walker_queue
// that causes heap corruption ... two threads doing insert_ele simultaneously = disaster
// solution ... each thread gets its own local queue ... merge all into global after parallel section
// extern EventQueue* walker_queue;
extern EventQueue* walker_queues[64];
static EventQueue local_queues[64];  // one per thread ... 64 is more than enough

void run_parallel_simulation(EventQueue* p, Scheduler* sch, 
                              DynArray_Signal* signal, DepGraph* g)
{
    while(p->size)
    {
        do
        {
            print_state();

            // PHASE 1 ... event extraction ... identical to sequential.c
            // single threaded ... only one thread should touch the event queue here
            // reason ... event queue is a min heap ... concurrent extract_min would corrupt it
            while(p->size > 0 && p->data[0].time == current_time && p->data[0].delta == delta)
            {
                Event e = extract_min(p);
                for(int i = 0; i < signal->size; i++)
                if(strcmp(signal->data[i].name, e.signal_name) == 0)
                {
                    signal->data[i].value_next = e.new_value;
                    signal->data[i].last_change_delta = delta;
                    signal->data[i].last_change_on = current_time;
                    vcd_write_change(signal->data[i], current_time);
                    trace_record(signal->data[i], current_time);
                    changed = 1;
                }
            }

            // PHASE 2 ... delta boundary swap ... identical to sequential.c
            signal_apply_updates(signal);

            // PHASE 3 ... find triggered processes
            // which processes need to run this delta?
            // same logic as sequential ... check which signals changed ... find processes watching them
            int triggered[64];
            int num_triggered = 0;
            for(int i = 0; i < sch->process_ARRAY.size; i++)
            {
                Process* proc = &sch->process_ARRAY.data[i];
                for(int j = 0; j < proc->senstivity_list.size; j++)
                {
                    // find the signal this process watches
                    for(int k = 0; k < signal->size; k++)
                    {
                        if(strcmp(signal->data[k].name, proc->senstivity_list.data[j].name) == 0
                           && signal->data[k].value != signal->data[k].prev_value)
                        {
                            triggered[num_triggered++] = i;
                            goto next_process;  // process already added ... skip other signals
                        }
                    }
                }
                next_process:;
            }

            // reset prev_value after building triggered list
            for(int i = 0; i < signal->size; i++)
                signal->data[i].prev_value = signal->data[i].value;

            // PHASE 4 ... parallel process execution by color batch
            // for each color ... run all triggered processes of that color in parallel
            // processes in same color = no shared signals = safe to parallelize
            // processes in different colors = potential conflicts = run in separate batches
            //
            // why omp parallel for here and not outer loop?
            // outer loop is color batches ... must run sequentially ... batch 1 before batch 2
            // inner loop is processes within a batch ... these are independent ... parallelize here
            int num_threads = omp_get_max_threads();
            for(int t = 0; t < num_threads; t++)
                local_queues[t] = init_queue();

            // point walker at local queue for this thread ... set before parallel region
            for(int color = 0; color < g->num_colors; color++)
            {
                // CHIRAG 13-04-26 :: omp parallel for ... this is the magic line
                // compiler splits iterations across threads automatically
                // schedule(dynamic,1) ... dynamic load balancing ... 
                // if one process finishes fast it picks up next one ... no idle threads
                #pragma omp parallel for schedule(dynamic, 1)
                for(int i = 0; i < num_triggered; i++)
                {
                    int proc_idx = triggered[i];
                    // only run processes of current color
                    if(g->color[proc_idx] != color) continue;

                    // CHIRAG 13-04-26 :: thread local queue ... no contention
                    // each thread writes events to its own queue
                    // merged into global queue after parallel section
                    int tid = omp_get_thread_num();
                    // walker_queue = &local_queues[tid];
                    // CHIRAG 15-04-26 :: set this thread's queue slot ... run_proc_generic reads walker_queues[tid]
                    // no conflict ... thread 0 sets slot 0 ... thread 1 sets slot 1 ... completely separate
                    walker_queues[tid] = &local_queues[tid];

                    // run the process ... same run_proc_generic as sequential
                    sch->process_ARRAY.data[proc_idx].run(proc_idx);
                }
                // implicit omp barrier here ... all threads finish color batch before next batch starts
            }

            // PHASE 5 ... merge local queues back into global queue
            // single threaded again ... safe to touch global queue now
            for(int t = 0; t < num_threads; t++)
                while(local_queues[t].size > 0)
                    insert_ele(p, extract_min(&local_queues[t]));

            if(changed)
                advance_delta();

        } while(detect_change());

        if(p->size > 0)
            printf("next event: t=%f d=%d\n", p->data[0].time, p->data[0].delta);
        if(p->size > 0 && p->data[0].time > current_time)
            advance_time(p);
        else if(p->size > 0 && p->data[0].time == current_time
                && p->data[0].delta > delta)
            delta = p->data[0].delta;
    }
}