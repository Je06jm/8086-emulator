#ifndef EMU8086_MEMORY_H
#define EMU8086_MEMORY_H

#include "types.h"

typedef byte_t (*Emu8086_Memory_FuncReadByte)(address_t address);
typedef void (*Emu8086_Memory_FuncWriteByte)(address_t address, byte_t value);
typedef word_t (*Emu8086_Memory_FuncReadWord)(address_t address);
typedef void (*Emu8086_Memory_FuncWriteWord)(address_t address, word_t value);

void Emu8086_Memory_RegisterReadByte(Emu8086_Memory_FuncReadByte* device);
void Emu8086_Memory_RegisterWriteByte(Emu8086_Memory_FuncWriteByte* device);
void Emu8086_Memory_RegisterReadWord(Emu8086_Memory_FuncReadWord* device);
void Emu8086_Memory_RegisterWriteWord(Emu8086_Memory_FuncWriteWord* device);

byte_t Emu8086_Memory_ReadByte(address_t address);
void Emu8086_Memory_WriteByte(address_t address, byte_t value);
word_t Emu8086_Memory_ReadWord(address_t address);
void Emu8086_Memory_WriteWord(address_t address, word_t value);

// Memory helper macros

#define CALC_MEMORY(seg, off) ((seg << 4) | off)

#endif