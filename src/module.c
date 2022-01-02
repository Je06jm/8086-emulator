#include <E86/module.h>

#include "alloc_healper.h"

ReserveInfo modules;

void E86_ModuleInit() {
    AllocSetup(&modules, sizeof(E86_Module));
}

void E86_ModuleFinish() {
    AllocFree(&modules);
}

void E86_ModuleAdd(E86_Module* module) {
    E86_Module* obj_module = AllocObject(&modules);

    *obj_module = *module;
}


void E86_ModuleInitModules() {
    E86_Module* module_list = (E86_Module*)modules.mem;
    
    for (uint32_t i = 0; i < modules.count; i++) {
        if (module_list[i].init != NULL) {
            module_list[i].init();
        }
    }
}

void E86_ModuleFinishModules() {
    E86_Module* module_list = (E86_Module*)modules.mem;
    
    for (uint32_t i = 0; i < modules.count; i++) {
        if (module_list[i].finish != NULL) {
            module_list[i].finish();
        }
    }
}

void E86_ModuleTickModules() {
    E86_Module* module_list = (E86_Module*)modules.mem;
    
    for (uint32_t i = 0; i < modules.count; i++) {
        if ((module_list[i].counter == 0) && (module_list[i].tick != NULL)) {
            module_list[i].tick();
            module_list[i].counter = module_list[i].divider;
        } else {
            module_list[i].counter--;
        }
    }
}