#ifndef EMULATOR_6502_INTERRUPTS_H
#define EMULATOR_6502_INTERRUPTS_H

#include <stdbool.h>
#include "types.h"

// Sets the initial interrupt states to false
void E65_InterruptsInit();

// The interrupt pin on the 6502 is level triggered so here we can set the state
void E65_InterruptSetInterrupt(bool interrupt);
// Returns true when there is an interrupt
bool E65_InterruptsHasInterrupt();
// The NM interrupt pin on the 6502 is edge triggered so here it is a trigger
// function
void E65_InterruptTriggerNMInterrupt();
// Returns true when there is an NM interrupt
bool E65_InterruptHasNMInterrupt();

#endif