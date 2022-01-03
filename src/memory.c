#include <E65/memory.h>

#include "alloc_healper.h"

ReserveInfo read_bytes;
ReserveInfo write_bytes;

void E65_MemoryInit() {
    // Setup structures
    AllocSetup(&read_bytes, sizeof(E65_MemoryFunction_ReadByte));
    AllocSetup(&write_bytes, sizeof(E65_MemoryFunction_WriteByte));
}

void E65_MemoryFinish() {
    // Free structures
    AllocFree(&read_bytes);
    AllocFree(&write_bytes);
};

void E65_MemoryRegisterReadByte(E65_MemoryFunction_ReadByte func) {
    // Register read function
    E65_MemoryFunction_ReadByte* ptr = AllocObject(&read_bytes);
    *ptr = func;
}

void E65_MemoryRegisterWriteByte(E65_MemoryFunction_WriteByte func) {
    // Register write function
    E65_MemoryFunction_WriteByte* ptr = AllocObject(&write_bytes);
    *ptr = func;
}


E65_Byte E65_MemoryReadByte(E65_Word address) {
    // Iterates through all registered read functions and call them. All the
    // results are then ORed together to emulate how real hardware would react
    // when multiple components respond at the same time
    E65_MemoryFunction_ReadByte* funcs =
        (E65_MemoryFunction_ReadByte*) read_bytes.mem;
    
    E65_Byte value = 0;

    for (uint32_t i = 0; i < read_bytes.count; i++) {
        value |= funcs[i](address);
    }

    return value;
}

void E65_MemoryWriteByte(E65_Word address, E65_Byte value) {
    // Iterates through all registered write functions and call them
    E65_MemoryFunction_WriteByte* funcs =
        (E65_MemoryFunction_WriteByte*)write_bytes.mem;
    
    for (uint32_t i = 0; i < write_bytes.count; i++) {
        funcs[i](address, value);
    }
}