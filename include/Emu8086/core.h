#ifndef EMU8086_CORE_H
#define EMU8086_CORE_H

extern void LoadEmuModules();

void Emu8086_Core_Init();
void Emu8086_Core_Finish();
void Emu8086_Core_Tick();

#endif