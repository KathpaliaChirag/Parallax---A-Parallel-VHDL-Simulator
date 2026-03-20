#ifndef VCD_H
#define VCD_H
#include <stdio.h>
#include "signal.h"
#include "../core/signal.h"
#include "../core/utils.h"

extern FILE* vcd_file; 
void vcd_init(char* filename); // make a file with some name which we can open and write into
void vcd_write_header(DynArray_Signal* signals); // adds a header based on signals in file
void vcd_write_change(Signal S, double time); // adds new changes/updates as they happen and its time
void vcd_write_close();// close file


#endif