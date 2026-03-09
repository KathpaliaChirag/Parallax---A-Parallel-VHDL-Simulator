//CHIRAG : 26/2/26
//something that holds events is event quueue which is defined here

#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H
#include "event.h"
//CHIRAG -09-03-26 : replacing the manual type of struct event queue to an automated marco  
#include "utils.h"

// typedef struct{
//     int size;
//     int capacity;
//     Event *data;
// } EventQueue;

DYNARRAY_TYPE(Event) //semicol not needed here as it litrally expands to ; in end 
typedef DynArray_Event EventQueue; //this line helps in aliasing
#endif