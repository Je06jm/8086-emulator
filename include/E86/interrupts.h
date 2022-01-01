#ifndef EMULATOR_8086_INTERRUPTS_H
#define EMULATOR_8086_INTERRUPTS_H

#include <stdbool.h>
#include "types.h"

void E86_InterruptsInit();

void E86_InterruptsRaiseInterrupt(E86_Byte interrupt);
void E86_InterruptsLowerInterrupt();
bool E86_InterruptsHasInterrupt();
E86_Byte E86_InterruptsGetInterrupt();

#endif