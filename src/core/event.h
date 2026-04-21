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
        int delta; // CHIRAG 12-03-26 00:00 :: added this line...okay so concept of deltas is interesting one descibed below
        int type;

        char* signal_name; // CHIRAG 16-03-26 :: added this ans below line while making schduler and run simthough of running signal by event
        int new_value;// could not think what to do so connecting event and adding signal here to trigger 
    } Event;

#endif
// CHIRAG 12-03-26 00:01 :
// so delta is set of updates that happen at same time unit like in a scheduler we ca have A=b and C=a happen at same time 
// so to avoid the race conditions and stopping C from getting old value of A what we have ?
// well thats where delta's come in whenever we have such cases we want our delta's to increment till all changes at a particular time
// so deltas increment as long as any change is seen at the time k when no change in system delta reset to 0 and time increase by 1
// NORMAL CASE: sorted by time changes like 1ns delta 0 A=B, 2ns delta 0 C=A... simple C gets new value of A

// DELTA CASE: same time different delta then changes happen by deltas... each new delta looks at changes made till old delta
// SAME TIME AND SAME DELTA : IN THIS CASE ORDER DOES NOT MATTER AND EACH VALUE IS TAKEN OF OLD DELTA
// SO LETS SAY C=A AND A=B.... THEN BY END OF DELTA C OLD VALUE OF A

// CASES IN DELTA : NO CHANGE MEANS WE ARE STABLE AND REAL TIME ADVANCES
// SOMETHING ALWAYS CHANGES => INFINTITE LOOP => BAD CIRCUT LIKE A<= not(A)
// FIRST EVENT ALWAYS OCCURE AT TIME T's DELTA 0