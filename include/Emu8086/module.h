#ifndef EMU8086_MODULE_H
#define EMU8086_MODULE_H

#include "types.h"

typedef void (*Emu8086_Module_InitFunc)();
typedef void (*Emu8086_Module_FinishFunc)();
typedef void (*Emu8086_Module_TickFunc)();

typedef struct {
    Emu8086_Module_InitFunc Init;
    Emu8086_Module_FinishFunc Finish;
    Emu8086_Module_TickFunc Tick;
    word_t tick_freq_count;
    word_t _ticks;
} Module_t;

void Emu8086_Module_Init();
void Emu8086_Module_Finish();

void Emu8086_Module_Register(const Module_t* module);

bool Emu8086_Module_InitAll();
void Emu8086_Module_FinishAll();
void Emu8086_Module_TickAll();

#endif