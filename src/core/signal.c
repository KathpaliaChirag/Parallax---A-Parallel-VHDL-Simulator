//CHIRAG : 28-02-26 (16:54)
//Okay so this file will have the implementation of what fuctions we will be applyong on a signal...for now i can think of 3
// namely initilise it, update it and maybe read it....this should cover all
// idea was to sorta have name passed as a parameter to the function and it create dunction of that name
//the other firlds will be value, last_change_time, type
// by default type would be 0 and will mean int
// value by def would be 0 same for last time change

#include<stdio.h>
#include "signal.h"
#include<stdlib.h> //for malloc
Signal* init_signal(char *n)
{

    Signal* s = (Signal*) malloc(sizeof(Signal));
    s->name = n;
    s->value = 0;
    s->last_change_on = 0;
    s->type =0;
    return s;
}
//as for update signla initial thought was time will come from the system
//later realised its a simulator so actual tome should come from the sim itself not us
void update_signal(Signal* s,int val, double time)
{
s->value = val;
s->last_change_on = time;
}
int get_signal_value(Signal *s)
{
return s->value;
}
//added later same day thought that migh need to see when was a function last written so added it 
double get_signal_last_change(Signal *s)
{
return s->last_change_on;
}

//somehow i always confuse between when too declare pointer (struct*) and when to add normal structs in here
// so ideally i can follow this rule and for every function as myself
//1. do i wish to modify the orignal using the later?
//2. is the orignal large enough that coping feels like a waste?

// 1st is what i knew but 2nd i didn't as in 
// Pass by value (struct) → C copies the whole thing. Fine for tiny structs, wasteful for big ones. 
// Also changes inside the function don't affect the original.
// Pass by pointer (struct*) → no copy, just address. Changes inside affect the original.
