// CHIRAG : 10-03-26 : 12:35 : Okay so main goal around this file is to create a process init and other functions
//as for process init idea is that it takes 4 things id, name, function pointer and senstivity list
//now i have thought well and decided to use a static type of counter variable 
// maybe we can have process id counter even as a global....
// rest name can be passed by the user creating process, same for funtion pointer for now will keep it void with void arguments
// as for senstivity list its much easier to start with an empty one and then make a function to add variables to it
// last but not the least... the type will be process 

#include<stdio.h>
#include "process.h"
#include "signal.h"
static int id_counter =0;
Process process_init(char *name, void (*run)(void))
{
Process p;
p.id = id_counter;
id_counter++;
p.name = name;
p.run = run;
DYNARRAY_INIT(p.senstivity_list)
return p;
}
// next up i think i need a way to add signal to process senstivity list for that i will put to use this mechanism of marco that i built
void process_add_signal(Process* p, Signal s)
{
    DYNARRAY_INSERT((*p).senstivity_list, s)
}


// was just looking at design goals had to add this process print thing which i almost missed so adding now
void process_print_signals(Process *p)
{
    printf("the process id %d and name %s watches following :\n", p->id, p->name);
    for(int i =0; i<p->senstivity_list.size; i++)
    {
        printf("for the number %d, the data item name is %s, and its value is %d \n",i, p->senstivity_list.data[i].name, p->senstivity_list.data[i].value);
    }
}