#include <Emu8086/module.h>

#include "list.h"

#ifdef EMU8086_DEBUG
#include <assert.h>
#endif

List modules;

void Emu8086_Module_Init() {
    List_Init(&modules, sizeof(Module_t));
}

void Emu8086_Module_Finish() {
    modules.Finish(&modules);
}

void Emu8086_Module_Register(const Module_t* module) {
#ifdef EMU8086_DEBUG
    assert(module != NULL);
#endif

    modules.PushBack(&modules, module);
}

void Emu8086_Module_InitAll() {
    Module_t* module;

    for (uintptr_t i = 0; i < modules.count; i++) {
        module = modules.Index(&modules, i);
        module->_ticks = 0;
        module->Init();
    }
}

void Emu8086_Module_FinishAll() {
    Module_t* module;

    for (uintptr_t i = 0; i < modules.count; i++) {
        module = modules.Index(&modules, i);
        module->Finish();
    }
}

void Emu8086_Module_TickAll() {
    Module_t* module;

    for (uintptr_t i = 0; i < modules.count; i++) {
        module = modules.Index(&modules, i);

        if (module->_ticks == 0) {
            module->Tick();
        }
        
        module->_ticks++;
        module->_ticks = module->_ticks % module->tick_freq_count;
    }
}