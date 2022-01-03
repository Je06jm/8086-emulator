#ifndef EMULATOR_6502_MEMORY_H
#define EMULATOR_6502_MEMORY_H

#include "types.h"

// Allocates structures for memory
void E65_MemoryInit();
// Frees structures from memory
void E65_MemoryFinish();

/* Since no memory is provided out of the box, this function allowes for custom
 * memory devices to receive any CPU read requests.
 * @param address The address that the CPU is reading from
 * @returns The value that the memory device returns. Return 0 if the address is
 * not in the custom memory device's memory range
*/
typedef E65_Byte (*E65_MemoryFunction_ReadByte)(E65_Word address);
/* Since no memory is provided out of the box, this function allowes for custom
 * memory devices to receive any CPU write requests.
 * @param address The address that the CPU is writting to
 * @param value The value that the CPU is writting
*/
typedef void (*E65_MemoryFunction_WriteByte)(E65_Word address, E65_Byte value);

// Registers a function that receives CPU read requests
void E65_MemoryRegisterReadByte(E65_MemoryFunction_ReadByte func);
// Registers a function that receives CPU write requests
void E65_MemoryRegisterWriteByte(E65_MemoryFunction_WriteByte func);

/* Reads a byte from memory. Every registered read function will get this
 * request
 * @param address The address of the byte to read
 * @returns The value read from memory
*/
E65_Byte E65_MemoryReadByte(E65_Word address);
/* Writes a byte to memory. Every registered write function will get this
 * request
 * @param address The address of the byte to write
 * @param value The value to write to memory
*/
void E65_MemoryWriteByte(E65_Word address, E65_Byte value);

#endif