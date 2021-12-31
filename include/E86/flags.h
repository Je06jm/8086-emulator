#ifndef EMULATOR_8086_FLAGS_H
#define EMULATOR_8086_FLAGS_H

#define FLAG_CARRY (1<<0)
#define FLAG_PARITY (1<<2)
#define FLAG_AUXILIARY_CARRY (1<<4)
#define FLAG_ZERO (1<<6)
#define FLAG_SIGN (1<<7)
#define FLAG_TRAP (1<<8)
#define FLAG_INTERRUPT (1<<9)
#define FLAG_DIRECTION (1<<10)
#define FLAG_OVERFLOW (1<<11)

#endif