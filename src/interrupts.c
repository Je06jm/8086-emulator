#include <E86/interrupts.h>

#include <stdint.h>

bool has_interrupts;
bool has_NMInterrupts;

void E86_InterruptsInit() {
    has_interrupts = false;
    has_NMInterrupts = false;
}

void E86_InterruptSetInterrupt(bool interrupt) {
    has_interrupts = interrupt;
}

bool E86_InterruptsHasInterrupt() {
    return has_interrupts;
}

void E86_InterruptTriggerNMInterrupt() {
    has_NMInterrupts = true;
}
bool E86_InterruptHasNMInterrupt(){
    if (has_NMInterrupts) {
        has_NMInterrupts = false;
        return true;
    }

    return false;
}

