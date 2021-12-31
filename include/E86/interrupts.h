#ifndef EMULATOR_8086_INTERRUPTS_H
#define EMULATOR_8086_INTERRUPTS_H

#include <stdbool.h>

void E86_InterruptsInit();

void E86_InterruptsRaiseInterrupt();
void E86_InterruptsLowerInterrupt();
bool E86_InterruptsHasInterrupt();

void E86_InterruptsRaiseNMInterrupt();
void E86_InterruptsLowerNMInterrupt();
bool E86_InterruptsHasNMInterrupt();

#endif