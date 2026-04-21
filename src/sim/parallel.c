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
// my idea is more or less acquired from the idea of independent sets and stuff....
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
//
// CHIRAG 21-04-26 :: MAJOR ARCHITECTURE CHANGE ... persistent thread model
// old approach ... #pragma omp parallel for INSIDE delta loop
// problem ... OMP spawns and joins threads EVERY delta cycle
// measured ... 946 deltas x ~500us spawn cost = 473ms of pure thread overhead
// actual work ... 9308 process firings x ~5ns = 0.05ms of real computation
// overhead was 10000x the actual work ... parallel always lost to sequential
//
// new approach ... #pragma omp parallel OUTSIDE all loops
// threads spawned ONCE at start of simulation ... kept alive entire run
// thread 0 handles all serial work (event extraction, signal swap, queue merge)
// all threads participate in process execution via #pragma omp for
// synchronization via omp single and omp for barriers ... NOT thread spawn/join
// barrier cost ~1us vs spawn cost ~500us ... 500x improvement in sync overhead
//
// this is the correct way to use OpenMP for simulation loops
// spawn-per-iteration is a classic parallel programming anti-pattern
// persistent threads + barriers is the standard approach in HPC

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
// CHIRAG 21-04-26 :: bumped to 256 ... wide_and128 needs 128+ thread slots
extern EventQueue* walker_queues[256];
static EventQueue local_queues[256];

void run_parallel_simulation(EventQueue* p, Scheduler* sch, 
                              DynArray_Signal* signal, DepGraph* g)
{
    // CHIRAG 21-04-26 :: pre-allocate local queues ONCE before simulation
    // old code called init_queue() inside the delta loop ... every delta = malloc
    // 946 deltas x 256 threads = 246000 malloc calls ... pure overhead
    // fix ... allocate once here ... reset size=0 each delta instead of reinit
    // this eliminates all queue malloc overhead from the hot path
    int num_threads = omp_get_max_threads();
    for(int t = 0; t < num_threads; t++)
        local_queues[t] = init_queue();

    // CHIRAG 21-04-26 :: static arrays for triggered list
    // must be static so all threads in omp parallel region can see them
    // stack variables inside omp parallel are thread-private ... we need shared
    static int triggered[256];
    static int num_triggered;
    static int keep_going;
    static int should_exit;

    // CHIRAG 21-04-26 :: spawn threads ONCE here ... persistent thread model
    // old code had omp parallel for inside delta loop ... threads spawned every delta
    // new code spawns threads once ... they loop internally ... joined only at end
    // all shared state (p, sch, signal, g) is accessible to all threads
    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();

        // set this thread's walker queue slot ... run_proc_generic reads walker_queues[tid]
        // CHIRAG 15-04-26 :: no conflict ... thread 0 sets slot 0 ... thread 1 sets slot 1
        walker_queues[tid] = &local_queues[tid];

        // CHIRAG 21-04-26 :: outer loop ... each iteration is one simulation timestep
        // thread 0 checks termination ... broadcasts via should_exit static var
        while(1)
        {
            // check if simulation is done ... only thread 0 reads p->size to avoid races
            #pragma omp single
            {
                should_exit = (p->size == 0);
            }
            // implicit barrier after omp single ... all threads see updated should_exit
            if(should_exit) break;

            // CHIRAG 21-04-26 :: inner loop ... delta cycle loop
            // same do-while structure as sequential.c ... delta advances while changed
            #pragma omp single
            { keep_going = 1; }

            while(keep_going)
            {
                // PHASE 1 ... event extraction ... thread 0 only
                // single threaded ... only one thread should touch the event queue here
                // reason ... event queue is a min heap ... concurrent extract_min would corrupt it
                #pragma omp single
                {
                    print_state();
                    while(p->size > 0 && p->data[0].time == current_time 
                          && p->data[0].delta == delta)
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
                            // CHIRAG 20-04-26 :: stat ... count every event that applies to a signal
                            stat_event_count++;
                        }
                    }

                    // PHASE 2 ... delta boundary swap ... thread 0 only
                    // CHIRAG 02-04-26 :: swap value_next into value BEFORE notifying processes
                    // so when processes run they see correct new value and correct prev_value
                    signal_apply_updates(signal);
                }
                // implicit barrier after omp single ... all threads wait here
                // all threads now see updated signal values before phase 3

                // PHASE 3 ... find triggered processes ... thread 0 only
                // builds triggered list shared by all threads in phase 4
                #pragma omp single
                {
                    num_triggered = 0;
                    for(int i = 0; i < sch->process_ARRAY.size; i++)
                    {
                        Process* proc = &sch->process_ARRAY.data[i];
                        for(int j = 0; j < proc->senstivity_list.size; j++)
                        {
                            for(int k = 0; k < signal->size; k++)
                            {
                                if(strcmp(signal->data[k].name, 
                                          proc->senstivity_list.data[j].name) == 0
                                   && signal->data[k].value != signal->data[k].prev_value)
                                {
                                    triggered[num_triggered++] = i;
                                    goto next_process;
                                }
                            }
                        }
                        next_process:;
                    }

                    // reset prev_value after building triggered list
                    for(int i = 0; i < signal->size; i++)
                        signal->data[i].prev_value = signal->data[i].value;

                    // CHIRAG 21-04-26 :: reset queues instead of reinitializing
                    // old code ... init_queue() every delta ... malloc every time ... killer overhead
                    // new code ... just reset size to 0 ... reuse existing heap memory
                    // data array stays allocated ... no malloc ... no free ... just counter reset
                    for(int t = 0; t < num_threads; t++)
                        local_queues[t].size = 0;
                }
                // implicit barrier after omp single ... all threads see triggered list

                // PHASE 4 ... parallel process execution by color batch
                // THIS is the hot path ... threads already alive ... no spawn/join cost
                // for each color batch ... omp for splits iterations across threads
                // processes in same color = no shared signals = safe to parallelize
                // processes in different colors = potential conflicts = run in separate batches
                for(int color = 0; color < g->num_colors; color++)
                {
                    // CHIRAG 13-04-26 :: omp for ... NOT omp parallel for
                    // omp parallel for would try to spawn new threads ... wrong
                    // omp for distributes iterations among ALREADY RUNNING threads
                    // this is the key difference in persistent thread model
                    // schedule(dynamic,1) ... dynamic load balancing
                    // if one process finishes fast it picks up next one ... no idle threads
                    #pragma omp for schedule(dynamic, 1)
                    for(int i = 0; i < num_triggered; i++)
                    {
                        int proc_idx = triggered[i];
                        if(g->color[proc_idx] != color) continue;

                        // CHIRAG 15-04-26 :: thread local queue ... no contention
                        // each thread writes events to its own queue
                        // merged into global queue after parallel section
                        sch->process_ARRAY.data[proc_idx].run(proc_idx);

                        // CHIRAG 20-04-26 :: stat ... count process firings in parallel mode
                        stat_process_firings++;
                    }
                    // implicit barrier after omp for ... all threads finish color batch
                    // before next color batch starts ... correct VHDL semantics preserved
                }

                // PHASE 5 ... merge local queues + advance delta ... thread 0 only
                #pragma omp single
                {
                    // merge all thread local queues back into global event queue
                    // single threaded again ... safe to touch global queue now
                    for(int t = 0; t < num_threads; t++)
                        while(local_queues[t].size > 0)
                            insert_ele(p, extract_min(&local_queues[t]));

                    if(changed)
                        advance_delta();

                    // check if delta loop should continue
                    keep_going = detect_change();

                    // if delta loop done ... advance time for next timestamp
                    if(!keep_going)
                    {
                        if(p->size > 0)
                            printf("next event: t=%f d=%d\n", 
                                   p->data[0].time, p->data[0].delta);
                        if(p->size > 0 && p->data[0].time > current_time)
                            advance_time(p);
                        else if(p->size > 0 && p->data[0].time == current_time
                                && p->data[0].delta > delta)
                            delta = p->data[0].delta;
                    }
                }
                // implicit barrier after omp single ... all threads sync before next delta
            }
            // end of delta loop ... threads loop back to outer while(1)
        }
        // end of outer while ... threads exit here ... joined by runtime
    }
    // all threads joined ... simulation complete
}