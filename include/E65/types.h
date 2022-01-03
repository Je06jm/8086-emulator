#ifndef EMULATOR_6502_TYPE_H
#define EMULATOR_6502_TYPE_H

#include <stdint.h>

// Define our byte
typedef uint8_t E65_Byte;
// Define our word
typedef uint16_t E65_Word;

#ifndef NULL
// If NULL is not defined, then define it here
#define NULL ((void*)0)
#endif

#endif