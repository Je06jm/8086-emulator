#include <Emu8086/memory.h>

#include "list.h"

#include <assert.h>

#ifdef EMU8086_DEBUG
#define TestForNull(ptr) assert(ptr != NULL);
#else
#define TestForNull(ptr)
#endif

List read_byte_funcs;
List write_byte_funcs;
List read_word_funcs;
List write_word_funcs;

void Emu8086_Memory_Init() {
    List_Init(&read_byte_funcs, sizeof(void*));
    List_Init(&write_byte_funcs, sizeof(void*));
    List_Init(&read_word_funcs, sizeof(void*));
    List_Init(&write_word_funcs, sizeof(void*));
}

void Emu8086_Memory_Finish() {
    read_byte_funcs.Finish(&read_byte_funcs);
    write_byte_funcs.Finish(&write_byte_funcs);
    read_word_funcs.Finish(&read_word_funcs);
    write_word_funcs.Finish(&write_word_funcs);
}

void Emu8086_Memory_RegisterReadByte(Emu8086_Memory_FuncReadByte device) {
    TestForNull(device);
    read_byte_funcs.PushBack(&read_byte_funcs, device);
}

void Emu8086_Memory_RegisterWriteByte(Emu8086_Memory_FuncWriteByte device) {
    TestForNull(device);
    write_byte_funcs.PushBack(&write_byte_funcs, device);
}

void Emu8086_Memory_RegisterReadWord(Emu8086_Memory_FuncReadWord device) {
    TestForNull(device);
    read_word_funcs.PushBack(&read_word_funcs, device);
}

void Emu8086_Memory_RegisterWriteWord(Emu8086_Memory_FuncWriteWord device) {
    TestForNull(device);
    write_word_funcs.PushBack(&write_word_funcs, device);
}


byte_t Emu8086_Memory_ReadByte(address_t address) {
    byte_t value = 0;
    Emu8086_Memory_FuncReadByte func;
    address &= 0xfffff;

    for (uintptr_t i = 0; i < read_byte_funcs.count; i++) {
        func = read_byte_funcs.Index(&read_byte_funcs, i);
        value |= func(address);
    }
    
    return value;
}

void Emu8086_Memory_WriteByte(address_t address, byte_t value) {
    Emu8086_Memory_FuncWriteByte func;
    address &= 0xfffff;

    for (uintptr_t i = 0; i < write_byte_funcs.count; i++) {
        func = write_byte_funcs.Index(&write_byte_funcs, i);
        func(address, value);
    }
}

word_t Emu8086_Memory_ReadWord(address_t address) {
    word_t value = 0;
    Emu8086_Memory_FuncReadWord func;
    address &= 0xfffff;

    for (uintptr_t i = 0; i < read_word_funcs.count; i++) {
        func = read_word_funcs.Index(&read_word_funcs, i);
        value |= func(address);
    }

    return value;
}

void Emu8086_Memory_WriteWord(address_t address, word_t value) {
    Emu8086_Memory_FuncWriteWord func;
    address &= 0xfffff;

    for (uintptr_t i = 0; i < write_word_funcs.count; i++) {
        func = write_word_funcs.Index(&write_word_funcs, i);
        func(address, value);
    }
}
