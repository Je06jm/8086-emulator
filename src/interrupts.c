#include <E86/interrupts.h>

#include <stdint.h>

uint32_t interrupts = 0;
uint32_t NMInterrupts = 0;

void E86_InterruptsInit() {
    interrupts = 0;
    NMInterrupts = 0;
}

void E86_InterruptsRaiseInterrupt() {
    interrupts++;
}

void E86_InterruptsLowerInterrupt() {
    interrupts--;
}

bool E86_InterruptsHasInterrupt() {
    return interrupts != 0;
}

void E86_InterruptsRaiseNMInterrupt() {
    NMInterrupts++;
}

void E86_InterruptsLowerNMInterrupt() {
    NMInterrupts--;
}

bool E86_InterruptsHasNMInterrupt() {
    return NMInterrupts != 0;
}

