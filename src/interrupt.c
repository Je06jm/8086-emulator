#include <Emu8086/interrupt.h>

#include <Emu8086/types.h>

#ifdef EMU8086_DEBUG
#include <assert.h>
#endif

address_t intr = 0;
address_t nmi = 0;

void Emu8086_Interrupt_Raise() {
    intr++;
}

void Emu8086_Interrupt_Finish() {
#ifdef EMU8086_DEBUG
    address_t pre = intr;
    intr--;
    assert(intr < pre);
#else
    intr--;
#endif
}

void Emu8086_Interrupt_RaiseNonMaskable() {
    nmi++;
}

void Emu8086_Interrupt_FinishNonMaskable() {
#ifdef EMU8086_DEBUG
    address_t pre = nmi;
    nmi--;
    assert(nmi < pre);
#else
    nmi--;
#endif
}


bool Emu8086_Interrupt_Pending() {
    return intr != 0;
}

bool Emu8086_Interrupt_PendingNonMaskable() {
    return nmi != 0;
}
