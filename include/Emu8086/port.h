#ifndef EMU8086_PORT_H
#define EMU8086_PORT_H

#include "types.h"

typedef byte_t (*Emu8086_Port_FuncReadByte)(byte_t port);
typedef void (*Emu8086_Port_FuncWriteByte)(byte_t port, byte_t value);
typedef word_t (*Emu8086_Port_FuncReadWord)(word_t port);
typedef void (*Emu8086_Port_FuncWriteWord)(word_t port, word_t value);

void Emu8086_Port_RegisterReadByte(Emu8086_Port_FuncReadByte* device);
void Emu8086_Port_RegisterWriteByte(Emu8086_Port_FuncWriteByte* device);
void Emu8086_Port_RegisterReadWord(Emu8086_Port_FuncReadWord* device);
void Emu8086_Port_RegisterWriteWord(Emu8086_Port_FuncWriteWord* device);

byte_t Emu8086_Port_ReadByte(byte_t port);
void Emu8086_Port_WriteByte(byte_t port, byte_t value);
word_t Emu8086_Port_ReadWord(word_t port);
void Emu8086_Port_WriteWord(word_t port, word_t value);

#endif