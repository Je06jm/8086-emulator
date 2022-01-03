#ifndef EMULATOR_6502_ALLOC_H

#include <E65/types.h>

// How many new objects to make room for when reallocating
#define ALLOC_HEALPER_NEW_ALLOC_COUNT 10

// A helper structure to aid allocations. This assumes that allocations are
// not freed
typedef struct {
    // How many objects are being used so far
    uint32_t count;
    // How many objects have been allocated
    uint32_t reserved;
    // The size of the object
    uint32_t size;
    // The memory pointer. Special care must be taken as the lifetime of the
    // memory can change without notice
    void* mem;
} ReserveInfo;

/* Sets up the ReserveInfo structure
 * @param info A pointer to the object's ReserveInfo structure
 * @param obj_size The size of the object
*/
void AllocSetup(ReserveInfo* info, uint32_t obj_size);
// Allocates a single object from the given ReserveInfo
// @param info The ReserveInfo to reserve from
void* AllocObject(ReserveInfo* info);
// Frees the memory owned by the ReserveInfo
// @param info The ReserveInfo to free memory from
void AllocFree(ReserveInfo* info);

#endif