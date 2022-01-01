#include <E86/memory.h>

#include "alloc_healper.h"

ReserveInfo read_bytes;
ReserveInfo write_bytes;

void E86_MemoryInit() {
    AllocSetup(&read_bytes, sizeof(E86_MemoryFunction_ReadByte));
    AllocSetup(&write_bytes, sizeof(E86_MemoryFunction_WriteByte));
}

void E86_MemoryFinish() {
    AllocFree(&read_bytes);
    AllocFree(&write_bytes);
};

void E86_MemoryRegisterReadByte(E86_MemoryFunction_ReadByte func) {
    E86_MemoryFunction_ReadByte* ptr = AllocObject(&read_bytes);
    *ptr = func;
}

void E86_MemoryRegisterWriteByte(E86_MemoryFunction_WriteByte func) {
    E86_MemoryFunction_WriteByte* ptr = AllocObject(&write_bytes);
    *ptr = func;
}


E86_Byte E86_MemoryReadByte(E86_Word address) {
    E86_MemoryFunction_ReadByte* funcs =
        (E86_MemoryFunction_ReadByte*) read_bytes.mem;
    
    E86_Byte value = 0;

    for (uint32_t i = 0; i < read_bytes.count; i++) {
        value |= funcs[i](address);
    }

    return value;
}

void E86_MemoryWriteByte(E86_Word address, E86_Byte value) {
    E86_MemoryFunction_WriteByte* funcs =
        (E86_MemoryFunction_WriteByte*)write_bytes.mem;
    
    for (uint32_t i = 0; i < write_bytes.count; i++) {
        funcs[i](address, value);
    }

    return value;
}