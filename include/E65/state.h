#ifndef EMULATOR_6502_STATE_H
#define EMULATOR_6502_STATE_H

#include "types.h"

// Sets up registers to their initial state
void E65_StateInit();

// 6502 registers
extern E65_Byte A, X, Y;
extern E65_Byte Flags;
extern E65_Byte Stack;
extern E65_Word IP;

#endif