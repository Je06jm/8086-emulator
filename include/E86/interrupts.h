#ifndef EMULATOR_8086_INTERRUPTS_H
#define EMULATOR_8086_INTERRUPTS_H

#include <stdbool.h>
#include "types.h"

void E86_InterruptsInit();

void E86_InterruptSetInterrupt(bool interrupt);
bool E86_InterruptsHasInterrupt();
void E86_InterruptTriggerNMInterrupt();
bool E86_InterruptHasNMInterrupt();

#endif