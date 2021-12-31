#include <E86/Port.h>

#include "alloc_healper.h"

ReserveInfo read_bytes;
ReserveInfo read_words;
ReserveInfo write_bytes;
ReserveInfo write_words;

void E86_PortInit() {
    AllocSetup(&read_bytes, sizeof(E86_PortFunction_ReadByte));
    AllocSetup(&read_words, sizeof(E86_PortFunction_ReadWord));
    AllocSetup(&write_bytes, sizeof(E86_PortFunction_WriteByte));
    AllocSetup(&write_words, sizeof(E86_PortFunction_WriteWord));
}

void E86_PortFinish() {
    AllocFree(&read_bytes);
    AllocFree(&read_words);
    AllocFree(&write_bytes);
    AllocFree(&write_words);
};

void E86_PortRegisterReadByte(E86_PortFunction_ReadByte func) {
    E86_PortFunction_ReadByte* ptr = AllocObject(&read_bytes);
    *ptr = func;
}

void E86_PortRegisterReadWord(E86_PortFunction_ReadWord func) {
    E86_PortFunction_ReadWord* ptr = AllocObject(&read_words);
    *ptr = func;
}

void E86_PortRegisterWriteByte(E86_PortFunction_WriteByte func) {
    E86_PortFunction_WriteByte* ptr = AllocObject(&write_bytes);
    *ptr = func;
}

void E86_PortRegisterWriteWord(E86_PortFunction_WriteWord func) {
    E86_PortFunction_WriteWord* ptr = AllocObject(&write_words);
    *ptr = func;
}


E86_Byte E86_PortReadByte(E86_Word address) {
    E86_PortFunction_ReadByte* funcs =
        (E86_PortFunction_ReadByte*) read_bytes.mem;
    
    E86_Byte value = 0;

    for (uint32_t i = 0; i < read_bytes.count; i++) {
        value |= funcs[i](address);
    }

    return value;
}

E86_Word E86_PortReadWord(E86_Word address) {
    E86_PortFunction_ReadWord* funcs =
        (E86_PortFunction_ReadWord*)read_words.mem;
    
    E86_Word value = 0;

    for (uint32_t i = 0; i < read_words.count; i++) {
        value |= funcs[i](address);
    }

    return value;
}

void E86_PortWriteByte(E86_Word address, E86_Byte value) {
    E86_PortFunction_WriteByte* funcs =
        (E86_PortFunction_WriteByte*)write_bytes.mem;
    
    for (uint32_t i = 0; i < write_bytes.count; i++) {
        funcs[i](address, value);
    }

    return value;
}

void E86_PortWriteWord(E86_Word address, E86_Word value) {
    E86_PortFunction_WriteWord* funcs =
        (E86_PortFunction_WriteWord*)write_words.mem;
    
    for (uint32_t i = 0; i < write_words.count; i++) {
        funcs[i](address, value);
    }
}