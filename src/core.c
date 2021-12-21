#include <Emu8086/core.h>
#include <Emu8086/interrupt.h>
#include <Emu8086/memory.h>
#include <Emu8086/module.h>
#include <Emu8086/port.h>
#include <Emu8086/state.h>

#include "error.h"
#include "list.h"

void Emu8086_Core_Init() {
    Emu8086_Memory_Init();
    Emu8086_Module_Init();
    Emu8086_Port_Init();

    LoadEmuModules();

    Emu8086_Module_InitAll();
}

void Emu8086_Core_Finish() {
    Emu8086_Module_FinishAll();
    
    Emu8086_Port_Finish();
    Emu8086_Module_Finish();
    Emu8086_Memory_Finish();
}

void Emu8086_Core_Tick() {
    Emu8086_Module_TickAll();
}
