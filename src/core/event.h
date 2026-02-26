// CHIRAG : 26-02=26
//ifndef is called a header guard
/*
why its needed lets say 2 files have event.h included in them what would happen
compiler may see them and get confused and may throw a redefinition error
ifndef = if not defined yet
then okay define now (2nd line)
*/
#ifndef EVENT_H
#define EVENT_H

typedef struct 
    {
        double time;
        int type;
    } Event;

#endif