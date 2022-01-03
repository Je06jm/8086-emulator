#include <E65/module.h>

#include "alloc_healper.h"

ReserveInfo modules;

void E65_ModuleInit() {
    // Setup structures
    AllocSetup(&modules, sizeof(E65_Module));
}

void E65_ModuleFinish() {
    // Free structures
    AllocFree(&modules);
}

void E65_ModuleAdd(E65_Module* module) {
    // Adds a module
    E65_Module* obj_module = AllocObject(&modules);

    *obj_module = *module;
}


void E65_ModuleInitModules() {
    // Iterates through all registered modules and call their init functions
    E65_Module* module_list = (E65_Module*)modules.mem;
    
    for (uint32_t i = 0; i < modules.count; i++) {
        if (module_list[i].init != NULL) {
            module_list[i].init();
        }
    }
}

void E65_ModuleFinishModules() {
    // Iterates through all registered modules and call their finish functions
    E65_Module* module_list = (E65_Module*)modules.mem;
    
    for (uint32_t i = 0; i < modules.count; i++) {
        if (module_list[i].finish != NULL) {
            module_list[i].finish();
        }
    }
}

void E65_ModuleTickModules() {
    // Iterates through all registered modules and decrement their counter.
    // If the counter reaches zero then it's tick function is called
    E65_Module* module_list = (E65_Module*)modules.mem;
    
    for (uint32_t i = 0; i < modules.count; i++) {
        if ((module_list[i].counter == 0) && (module_list[i].tick != NULL)) {
            module_list[i].tick();
            module_list[i].counter = module_list[i].reset;
        } else {
            module_list[i].counter--;
        }
    }
}