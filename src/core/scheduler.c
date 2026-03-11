//CHIRAG 10-03-26 (13:50) : COMING TO PLAN OF SCHEDULER.C FILE
// WE WILL NEED TO INITIALIZE SCEDULER AN EMPTY ONE... AND THEN WILL HAVE TO WORK ON FURTHER THINGS 
// MAYBE A SCHEDULER NOTIFY FUNCTION THAT DOES P.RUN() 
// MAYBE A MAPPING DISPLAY TABLE WHAT ALL SIGNALS DOES A PROCESS TAKE FOR ALL PROCESS?

#include <stdio.h>
#include "scheduler.h"
#include <string.h>
Scheduler scheduler_init()
{
    Scheduler p;
    DYNARRAY_INIT(p.process_ARRAY)
    return p;
}

//CHIRAG 11-03-26 21:26 : so now that we have it the scheduler ininitialiser even thopugh empty its there so now 
// we will wake up porocess on change in signal ... for that idea is simple
// for each process i , for each signal j in process i senstivity list we wake up process by calling run 
void scheduler_notify(Scheduler* sch, Signal s)
{
    for(int i =0; i<sch->process_ARRAY.size; i++)
    {
        for(int j=0; j<sch->process_ARRAY.data[i].senstivity_list.size; j++)
        if(strcmp(sch->process_ARRAY.data[i].senstivity_list.data[j].name, s.name) ==0)
        {
            sch->process_ARRAY.data[i].run();
            printf("process woke up %s", sch->process_ARRAY.data[i].name);
        }
    }
}

// next function on the go is scheduler add process just ike process add signals

void scheduler_add_process(Scheduler* sch, Process p)
{
    DYNARRAY_INSERT(sch->process_ARRAY, p)
}
