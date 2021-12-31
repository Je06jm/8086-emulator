#ifndef EMULATOR_8086_ALLOC_H

#include <E86/types.h>

#define ALLOC_HEALPER_NEW_ALLOC_COUNT 10

typedef struct {
    uint32_t count;
    uint32_t reserved;
    uint32_t size;
    void* mem;
} ReserveInfo;

void AllocSetup(ReserveInfo* info, uint32_t obj_size);
void* AllocObject(ReserveInfo* info);
void AllocFree(ReserveInfo* info);

#endif