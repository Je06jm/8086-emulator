#include <E86/memory.h>

#include "alloc_healper.h"

ReserveInfo read_bytes;
ReserveInfo read_words;
ReserveInfo write_bytes;
ReserveInfo write_words;

void E86_MemoryInit() {
    AllocSetup(&read_bytes, sizeof(E86_MemoryFunction_ReadByte));
    AllocSetup(&read_words, sizeof(E86_MemoryFunction_ReadWord));
    AllocSetup(&write_bytes, sizeof(E86_MemoryFunction_WriteByte));
    AllocSetup(&write_words, sizeof(E86_MemoryFunction_WriteWord));
}

void E86_MemoryFinish() {
    AllocFree(&read_bytes);
    AllocFree(&read_words);
    AllocFree(&write_bytes);
    AllocFree(&write_words);
};

void E86_MemoryRegisterReadByte(E86_MemoryFunction_ReadByte func) {
    E86_MemoryFunction_ReadByte* ptr = AllocObject(&read_bytes);
    *ptr = func;
}

void E86_MemoryRegisterReadWord(E86_MemoryFunction_ReadWord func) {
    E86_MemoryFunction_ReadWord* ptr = AllocObject(&read_words);
    *ptr = func;
}

void E86_MemoryRegisterWriteByte(E86_MemoryFunction_WriteByte func) {
    E86_MemoryFunction_WriteByte* ptr = AllocObject(&write_bytes);
    *ptr = func;
}

void E86_MemoryRegisterWriteWord(E86_MemoryFunction_WriteWord func) {
    E86_MemoryFunction_WriteWord* ptr = AllocObject(&write_words);
    *ptr = func;
}


E86_Byte E86_MemoryReadByte(E86_Address address) {
    E86_MemoryFunction_ReadByte* funcs =
        (E86_MemoryFunction_ReadByte*) read_bytes.mem;
    
    E86_Byte value = 0;

    for (uint32_t i = 0; i < read_bytes.count; i++) {
        value |= funcs[i](address);
    }

    return value;
}

E86_Word E86_MemoryReadWord(E86_Address address) {
    E86_MemoryFunction_ReadWord* funcs =
        (E86_MemoryFunction_ReadWord*)read_words.mem;
    
    E86_Word value = 0;

    for (uint32_t i = 0; i < read_words.count; i++) {
        value |= funcs[i](address);
    }

    return value;
}

void E86_MemoryWriteByte(E86_Address address, E86_Byte value) {
    E86_MemoryFunction_WriteByte* funcs =
        (E86_MemoryFunction_WriteByte*)write_bytes.mem;
    
    for (uint32_t i = 0; i < write_bytes.count; i++) {
        funcs[i](address, value);
    }

    return value;
}

void E86_MemoryWriteWord(E86_Address address, E86_Word value) {
    E86_MemoryFunction_WriteWord* funcs =
        (E86_MemoryFunction_WriteWord*)write_words.mem;
    
    for (uint32_t i = 0; i < write_words.count; i++) {
        funcs[i](address, value);
    }
}