#include <Emu8086/port.h>

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

void Emu8086_Port_Init() {
    List_Init(&read_byte_funcs, sizeof(void*));
    List_Init(&write_byte_funcs, sizeof(void*));
    List_Init(&read_word_funcs, sizeof(void*));
    List_Init(&write_word_funcs, sizeof(void*));
}

void Emu8086_Port_Finish() {
    read_byte_funcs.Finish(&read_byte_funcs);
    write_byte_funcs.Finish(&write_byte_funcs);
    read_word_funcs.Finish(&read_word_funcs);
    write_word_funcs.Finish(&write_word_funcs);
}

void Emu8086_Port_RegisterReadByte(Emu8086_Port_FuncReadByte device) {
    TestForNull(device);
    read_byte_funcs.PushBack(&read_byte_funcs, device);
}

void Emu8086_Port_RegisterWriteByte(Emu8086_Port_FuncWriteByte device) {
    TestForNull(device);
    write_byte_funcs.PushBack(&write_byte_funcs, device);
}

void Emu8086_Port_RegisterReadWord(Emu8086_Port_FuncReadWord device) {
    TestForNull(device);
    read_word_funcs.PushBack(&read_word_funcs, device);
}

void Emu8086_Port_RegisterWriteWord(Emu8086_Port_FuncWriteWord device) {
    TestForNull(device);
    write_word_funcs.PushBack(&write_word_funcs, device);
}


byte_t Emu8086_Port_ReadByte(byte_t port) {
    Emu8086_Port_FuncReadByte func;
    byte_t value = 0;

    for (uintptr_t i = 0; i < read_byte_funcs.count; i++) {
        func = read_byte_funcs.Index(&read_byte_funcs, i);
        value |= func(port);
    }

    return value;
}

void Emu8086_Port_WriteByte(byte_t port, byte_t value) {
    Emu8086_Port_FuncWriteByte func;

    for (uintptr_t i = 0; i < write_byte_funcs.count; i++) {
        func = write_byte_funcs.Index(&write_byte_funcs, i);
        func(port, value);
    }
}

word_t Emu8086_Port_ReadWord(word_t port) {
    Emu8086_Port_FuncReadWord func;
    word_t value = 0;

    for (uintptr_t i = 0; i < read_word_funcs.count; i++) {
        func = read_word_funcs.Index(&read_word_funcs, i);
        value |= func(port);
    }

    return value;
}

void Emu8086_Port_WriteWord(word_t port, word_t value) {
    Emu8086_Port_FuncWriteWord func;

    for (uintptr_t i = 0; i < write_word_funcs.count; i++) {
        func = write_word_funcs.Index(&write_word_funcs, i);
        func(port, value);
    }
}

