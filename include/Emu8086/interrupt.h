#ifndef EMU8086_INTERRUPT_H
#define EMU8086_INTERRUPT_H

void Emu8086_Interrupt_Raise();
void Emu8086_Interrupt_Finish();
void Emu8086_Interrupt_RaiseNonMaskable();
void Emu8086_Interrupt_FinishNonMaskable();

bool Emu8086_Interrupt_Pending();
bool Emu8086_Interrupt_PendingNonMaskable();

#endif