#ifndef EMULATOR_6502_MODULE_H
#define EMULATOR_6502_MODULE_H

#include <stdint.h>

// Defines the module init callback
typedef void (*E65_ModuleFunction_Init)();
// Defines the module finish callback
typedef void (*E65_ModuleFunction_Finish)();
// Defines the module tick callback
typedef void (*E65_ModuleFunciton_Tick)();

// Modules allows for custom devices to be registered in the system. For 
// example, a keyboard contoller
typedef struct {
    // This is called to init this module. If this is NULL, then initialization
    // is skipped
    E65_ModuleFunction_Init init;
    // This is called to cleanup this module. If this is NULL, then cleanup is
    // skipped
    E65_ModuleFunction_Finish finish;
    // This is called every time the counter is zero. If this is NULL, then this
    // module won't be ticked;
    E65_ModuleFunciton_Tick tick;

    // When the counter reaches zero, it is set to this value
    uint32_t reset;
    // Every system tick, this is decremented
    uint32_t counter;
} E65_Module;

// Allocates module structures
void E65_ModuleInit();
// Cleanup module structures
void E65_ModuleFinish();
/* Adds a module to the system
 * @param module A pointer to the module to be added. The module structure will 
 * be copied into an internal structure
*/
void E65_ModuleAdd(E65_Module* module);

// Calls all registered module's init function
void E65_ModuleInitModules();
// Calls all registered module's finish function
void E65_ModuleFinishModules();
// Calls all registered module's tick function
void E65_ModuleTickModules();

#endif