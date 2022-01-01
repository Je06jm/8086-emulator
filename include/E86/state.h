#ifndef EMULATOR_8086_STATE_H
#define EMULATOR_8086_STATE_H

#include "types.h"
#include <stdbool.h>

void E86_StateInit();

extern bool Running;

extern E86_Byte A, X, Y;
extern E86_Byte Flags;
extern E86_Byte Stack;
extern E86_Word IP;

#endif