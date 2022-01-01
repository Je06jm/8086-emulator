#ifndef EMULATOR_8086_MEMORY_H
#define EMULATOR_8086_MEMORY_H

#include "types.h"

void E86_MemoryInit();
void E86_MemoryFinish();

typedef E86_Byte (*E86_MemoryFunction_ReadByte)(E86_Word);
typedef void (*E86_MemoryFunction_WriteByte)(E86_Word, E86_Byte);

void E86_MemoryRegisterReadByte(E86_MemoryFunction_ReadByte func);
void E86_MemoryRegisterWriteByte(E86_MemoryFunction_WriteByte func);

E86_Byte E86_MemoryReadByte(E86_Word address);
void E86_MemoryWriteByte(E86_Word address, E86_Byte value);

#define E86_CALC_ADDRESS(segment, address) ((segment << 4) | address)

#endif