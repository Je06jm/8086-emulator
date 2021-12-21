#ifndef EMU8086_MODULE_H
#define EMU8086_MODULE_H

#include "types.h"

typedef bool (*Emu8086_Module_Init)();
typedef bool (*Emu8086_Module_Finish)();
typedef bool (*Emu8086_Module_Tick)();

typedef struct {
    Emu8086_Module_Init Init;
    Emu8086_Module_Finish Finish;
    Emu8086_Module_Tick Tick;
    word_t tick_freq_div;
} Module_t;

void Emu8086_Module_Register(const Module_t* module);

#endif