#ifndef EMULATOR_8086_STATE_H
#define EMULATOR_8086_STATE_H

#include "types.h"
#include <stdbool.h>

extern bool running;

extern E86_Byte regs[8];
extern E86_Word segs[4];
extern E86_Word IP;
extern E86_Word Flags;

#endif