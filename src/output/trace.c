// CHIRAG : 18-03-26 : 18:59 ::
// so we have 3 functions :
// trace_init : we create trace_log variable and call DYNARRAY_INIT on it 
// treace record adds signal change values and append it to trace_log 
// then trace hash will simply put hash the entry and time ideally we will use a very basic hash function
// its called djb2 so its like this take a start hash number multiply it by 31 and add the entry time or value onto it

#include <stdio.h>
#include "../output/trace.h"
#include "../core/utils.h"
#include <string.h>
DynArray_signal_change trace_log;
void trace_init()
{
    DYNARRAY_INIT(trace_log);
}
void trace_record(Signal s, double time)
{
    signal_change sc;
    sc.signal_name =s.name;
    sc.time = time;
    // sc.value = s.value; // was taking old value will fix now 
    // CHIRAG 02-04-26 :: record value_next not value
    // trace_record is called before signal_apply_updates swap
    // so value still has old state, value_next has the correct new value
    sc.value = s.value_next;
    DYNARRAY_INSERT(trace_log, sc);
}

unsigned int trace_hash()
{
    // CHIRAG 15-04-26 :: why do we sort before hashing?
    //
    // problem ... parallel simulator runs multiple processes simultaneously
    // process 0 and process 1 might finish in any order depending on thread scheduling
    // so trace_log entries for same timestep get recorded in random order each run
    // hash is order dependent ... same events different order = different hash every run
    // proved this by running eight_channel 6 times ... got 5 different hashes
    //
    // solution 1 we considered ... sort merged event queue before extraction
    // problem ... fixes order of events entering queue but still depends on thread timing
    //
    // solution 2 we considered ... commutative hash like XOR
    // problem ... XOR is too weak ... many different logs can produce same hash by accident
    //
    // how systemc and vhdl handle it ...
    // real simulators define deterministic signal update ordering within a delta cycle
    // usually by declaration order in source file ... parallel computation but sequential recording
    // the key insight ... compute in parallel but record in fixed deterministic order
    //
    // our solution ... sort trace_log by (time, signal_name) before hashing
    // time is primary sort key ... signal_name alphabetical is secondary
    // now regardless of which thread recorded Y0 or Y1 first ...
    // after sort Y0 always comes before Y1 at same timestep
    // sequential and parallel both produce identical sorted logs ... identical hashes
    // this matches the spirit of VHDL deterministic ordering

    // bubble sort trace_log by (time, signal_name)
    // small N so bubble sort is fine ... trace log wont be huge for our circuits
    for(int i = 0; i < trace_log.size - 1; i++)
    {
        for(int j = 0; j < trace_log.size - i - 1; j++)
        {
            signal_change* a = &trace_log.data[j];
            signal_change* b = &trace_log.data[j+1];

            // compare by time first
            int should_swap = 0;
            if(a->time > b->time)
                should_swap = 1;
            else if(a->time == b->time)
            {
                // same time ... sort by signal name alphabetically
                // strcmp returns positive if a->signal_name comes after b->signal_name
                if(strcmp(a->signal_name, b->signal_name) > 0)
                    should_swap = 1;
            }

            if(should_swap)
            {
                signal_change temp = *a;
                *a = *b;
                *b = temp;
            }
        }
    }

    // hash the sorted log ... same djb2 as before
    // now order is deterministic ... seq and parallel produce same hash
    unsigned int hash = 5381;
    for(int i = 0; i < trace_log.size; i++)
    {
        hash = hash * 31 + (unsigned int)trace_log.data[i].time;
        hash = hash * 31 + trace_log.data[i].value;
        for(int j = 0; trace_log.data[i].signal_name[j] != '\0'; j++)
            hash = hash * 31 + trace_log.data[i].signal_name[j];
    }
    return hash;
}