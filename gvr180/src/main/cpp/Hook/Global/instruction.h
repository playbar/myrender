#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <stdio.h>

void relocateInstruction( uint32_t target_addr, void *orig_instructions, int length, void *trampoline_instructions, int *orig_boundaries, int *trampoline_boundaries, int *count );

#endif //_INSTRUCTION_H_