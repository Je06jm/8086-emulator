#include "alloc_healper.h"

#include <stdlib.h>
#include <memory.h>

void AllocSetup(ReserveInfo* info, uint32_t obj_size) {
    // Nothing is allocated yet, so set everything, except size, to zero
    info->count = 0;
    info->reserved = 0;
    info->size = obj_size;
    info->mem = NULL;
}

void* AllocObject(ReserveInfo* info) {
    // Check to see if there is a free object
    if (info->count == info->reserved) {
        // At this point, there is no free object so a reallocation must happen
        uint32_t new_mem_size =
            (info->reserved + ALLOC_HEALPER_NEW_ALLOC_COUNT) * info->size;

        void* new_mem = malloc(new_mem_size);
        
        if (info->mem != NULL) {
            // Copy the existing objects if they exists
            memcpy_s(new_mem, new_mem_size, info->mem,
                info->reserved * info->size);
            
            free(info->mem);
        }

        info->reserved += ALLOC_HEALPER_NEW_ALLOC_COUNT;
        info->mem = new_mem;
    }

    // Get the address of the memory
    uintptr_t addr = (uintptr_t)(info->mem);
    // Add the offset to the first free object
    addr += info->count * info->size;
    info->count++;

    return (void*)addr;
}

void AllocFree(ReserveInfo* info) {
    // Free the objects
    if (info->mem != NULL) {
        free(info->mem);
    }
}