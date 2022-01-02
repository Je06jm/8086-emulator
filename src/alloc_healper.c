#include "alloc_healper.h"

#include <stdlib.h>
#include <memory.h>

void AllocSetup(ReserveInfo* info, uint32_t obj_size) {
    info->count = 0;
    info->reserved = 0;
    info->size = obj_size;
    info->mem = NULL;
}

void* AllocObject(ReserveInfo* info) {
    if (info->count == info->reserved) {
        uint32_t new_mem_size =
            (info->reserved + ALLOC_HEALPER_NEW_ALLOC_COUNT) * info->size;

        void* new_mem = malloc(new_mem_size);
        
        if (info->mem != NULL) {
            memcpy_s(new_mem, new_mem_size, info->mem,
                info->reserved * info->size);
            
            free(info->mem);
        }

        info->reserved += ALLOC_HEALPER_NEW_ALLOC_COUNT;
        info->mem = new_mem;
    }

    uintptr_t addr = (uintptr_t)(info->mem);
    addr += info->count * info->size;
    info->count++;

    return (void*)addr;
}

void AllocFree(ReserveInfo* info) {
    if (info->mem != NULL) {
        free(info->mem);
    }
}