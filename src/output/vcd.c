// CHIRAG 17-03-26 : 13:06 :: okay so VCD and trace.... let me explain why we need these
//
// VCD (Value Change Dump) :
// so basically whenever a signal changes in my Parallax I want to record it
// the record goes into a .vcd file which is a standard format that tools like GTKWave can read
// GTKWave then shows a visual waveform.... like an oscilloscope basically
// without this I just have printf outputs which is not great for demo or verification
// the format is simple.... header defines signals, then for each timestep you write what changed
// example : #1 means time=1, then 1! means signal with id ! became 1
// its literally just text.... not complex at all
//
// WHY VCD : demo + industry standard + visual verification that simulation is correct
//
// TRACE HASHING :
// so when we later make parallel simulator we need to verify it gives same answer as sequential
// idea is simple.... record every signal change in order during simulation
// then hash that sequence into one number.... like a checksum
// sequential hash == parallel hash means results are identical
// different hash means bug somewhere in parallel implementation
// its basically a fingerprint of the entire simulation run
//
// ORDER OF THINGS :
// 1. VCD .... write signal changes to file as sim runs
// 2. TRACE .... record signal changes in memory, hash at end  
// 3. LATER .... compare seq vs parallel hashes to verify correctness

// CHIRAG 17-03-26 :: VCD output implementation
// I do not completely understand this vcd format yet so I took help of AI here 
// code is written by me though the understanding and explanation and format, where to put error 
// all was told by GPT
// VCD is just a text file.... we use fprintf to write to it same as printf but to a file
#include <stdio.h>
#include "vcd.h"

FILE* vcd_file; // the actual file we write to

// opens the vcd file for writing
void vcd_init(char* filename)
{
    // fopen opens a file.... "w" means write mode, creates if not exists
    vcd_file = fopen(filename, "w");
    if(vcd_file == NULL)
        printf("ERROR: could not open vcd file\n");
}

// writes the header.... done once at start before simulation
void vcd_write_header(DynArray_Signal* signals)
{
    // timescale tells GTKWave what 1 unit of time means
    fprintf(vcd_file, "$timescale 1ns $end\n");
    
    // declare each signal.... assign symbol starting from '!'
    // format: $var wire 1 SYMBOL NAME $end
    for(int i = 0; i < signals->size; i++)
    {
        signals->data[i].vcd_symbol = '!' + i;
        fprintf(vcd_file, "$var wire 1 %c %s $end\n",  signals->data[i].vcd_symbol,  signals->data[i].name);
    }
    
    // dumpvars section.... initial values of all signals
    fprintf(vcd_file, "$dumpvars\n");
    for(int i = 0; i < signals->size; i++)
        fprintf(vcd_file, "0%c\n", signals->data[i].vcd_symbol);
    fprintf(vcd_file, "$end\n");
}

// called every time a signal changes during simulation
void vcd_write_change(Signal s, double time)
{
    // # means timestamp
    fprintf(vcd_file, "#%d\n", (int)time);
    // write new value followed by symbol
    fprintf(vcd_file, "%d%c\n", s.value, s.vcd_symbol);
}

// close the file when simulation ends
void vcd_close()
{
    if(vcd_file != NULL)
        fclose(vcd_file);
}