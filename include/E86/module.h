#ifndef EMULATOR_8086_MODULE_H
#define EMULATOR_8086_MODULE_H

#include <stdint.h>

typedef void (*EM86_ModuleFunction_Init)();
typedef void (*EM86_ModuleFunction_Finish)();
typedef void (*EM86_ModuleFunciton_Tick)();

typedef struct {
    EM86_ModuleFunction_Init init;
    EM86_ModuleFunction_Finish finish;
    EM86_ModuleFunciton_Tick tick;

    uint32_t divider;
    uint32_t counter;
} EM86_Module;

void E86_ModuleInit();
void E86_ModuleFinish();
void E86_ModuleAdd(EM86_Module* module);

void E86_ModuleInitModules();
void E86_ModuleFinishModules();
void E86_ModuleTickModules();

#endif