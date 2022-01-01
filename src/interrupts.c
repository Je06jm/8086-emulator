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
    static bool last_NMIs = false;
    bool has_NMIs = NMInterrupts != 0;
    bool ret = false;

    ret = ((last_NMIs != has_NMIs) && has_NMIs);
    has_NMIs = NMInterrupts;
    
    return ret;
}

