#include <E65/interrupts.h>

#include <stdint.h>

// True when there is an interrupt
bool has_interrupts;
// True when there is an NM interrupt
bool has_NMInterrupts;

void E65_InterruptsInit() {
    has_interrupts = false;
    has_NMInterrupts = false;
}

void E65_InterruptSetInterrupt(bool interrupt) {
    has_interrupts = interrupt;
}

bool E65_InterruptsHasInterrupt() {
    return has_interrupts;
}

void E65_InterruptTriggerNMInterrupt() {
    has_NMInterrupts = true;
}
bool E65_InterruptHasNMInterrupt(){
    // If there is a NM interrupt, set has_NMInterrupts to false and the return
    // true
    if (has_NMInterrupts) {
        has_NMInterrupts = false;
        return true;
    }

    return false;
}

