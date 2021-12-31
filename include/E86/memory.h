#ifndef EMULATOR_8086_MEMORY_H
#define EMULATOR_8086_MEMORY_H

#include "types.h"

void E86_MemoryInit();
void E86_MemoryFinish();

typedef E86_Byte (*E86_MemoryFunction_ReadByte)(E86_Address);
typedef E86_Word (*E86_MemoryFunction_ReadWord)(E86_Address);
typedef void (*E86_MemoryFunction_WriteByte)(E86_Address, E86_Byte);
typedef void (*E86_MemoryFunction_WriteWord)(E86_Address, E86_Word);

void E86_MemoryRegisterReadByte(E86_MemoryFunction_ReadByte func);
void E86_MemoryRegisterReadWord(E86_MemoryFunction_ReadWord func);
void E86_MemoryRegisterWriteByte(E86_MemoryFunction_WriteByte func);
void E86_MemoryRegisterWriteWord(E86_MemoryFunction_WriteWord func);

E86_Byte E86_MemoryReadByte(E86_Address address);
E86_Word E86_MemoryReadWord(E86_Address address);
void E86_MemoryWriteByte(E86_Address address, E86_Byte value);
void E86_MemoryWriteWord(E86_Address address, E86_Word value);

#define E86_CALC_ADDRESS(segment, address) ((segment << 4) | address)

#endif