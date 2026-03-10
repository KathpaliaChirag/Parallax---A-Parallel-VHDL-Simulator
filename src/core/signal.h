//CHIRAG : 28-02-2026 (15:50) 
//so basically idea os to define a signal thats also the 1st goal of day
//we need 4 things in signal for now may add more later
// ideally we are storing name, value, type, lastchange time

#ifndef SIGNAL_H
#define SIGNAL_H
#include "utils.h"
typedef struct
{
int value;
char* name;
double last_change_on;
int type;
} Signal;
DYNARRAY_TYPE(Signal) //added here as a part of design choice CHIRAG-10-03-26 12:26
#endif
//interestingly I also got to know that there is this unsaid convention in C to name your struct starting with a capital letter
//it does not affect the code but helps to understand in a good code that 'Signal' is a type and not a name

