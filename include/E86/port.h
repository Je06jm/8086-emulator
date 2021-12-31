#ifndef EMULATOR_8086_PORT_H
#define EMULATOR_8086_PORT_H

#include "types.h"

void E86_PortInit();
void E86_PortFinish();

typedef E86_Byte (*E86_PortFunction_ReadByte)(E86_Word);
typedef E86_Word (*E86_PortFunction_ReadWord)(E86_Word);
typedef void (*E86_PortFunction_WriteByte)(E86_Word, E86_Byte);
typedef void (*E86_PortFunction_WriteWord)(E86_Word, E86_Word);

void E86_PortRegisterReadByte(E86_PortFunction_ReadByte func);
void E86_PortRegisterReadWord(E86_PortFunction_ReadWord func);
void E86_PortRegisterWriteByte(E86_PortFunction_WriteByte func);
void E86_PortRegisterWriteWord(E86_PortFunction_WriteWord func);

E86_Byte E86_PortReadByte(E86_Word address);
E86_Word E86_PortReadWord(E86_Word address);
void E86_PortWriteByte(E86_Word address, E86_Byte value);
void E86_PortWriteWord(E86_Word address, E86_Word value);

#endif