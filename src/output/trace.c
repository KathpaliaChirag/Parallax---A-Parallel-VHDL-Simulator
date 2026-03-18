// CHIRAG : 18-03-26 : 18:59 ::
// so we have 3 functions :
// trace_init : we create trace_log variable and call DYNARRAY_INIT on it 
// treace record adds signal change values and append it to trace_log 
// then trace hash will simply put hash the entry and time ideally we will use a very basic hash function
// its called djb2 so its like this take a start hash number multiply it by 31 and add the entry time or value onto it

#include <stdio.h>
#include "../output/trace.h"
#include "../core/utils.h"
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
    sc.value = s.value;
    DYNARRAY_INSERT(trace_log, sc);
}

unsigned int trace_hash()
{
    unsigned int hash=5381; // shouldn't we also do mod or something cause otherwise it can overflow long long for big circuits
    for(int i=0; i<trace_log.size; i++)
    {
        hash = hash*31 + trace_log.data[i].time;
        hash = hash*31 + trace_log.data[i].value;
        for(int j =0;'\0' !=trace_log.data[i].signal_name[j];j++)
        {
            hash= hash *31 + trace_log.data[i].signal_name[j];
        }

        
    }
    return hash;
}