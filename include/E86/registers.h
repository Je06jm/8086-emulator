#ifndef EMULATOR_8086_REGISTERS_H
#define EMULATOR_8086_REGISTERS_H

#include "state.h"

#define REGISTER_AL 0
#define REGISTER_CL 1
#define REGISTER_DL 2
#define REGISTER_BL 3
#define REGISTER_AH 4
#define REGISTER_CH 5
#define REGISTER_DH 6
#define REGISTER_BH 7

#define REGISTER_AX 0
#define REGSITER_CX 1
#define REGISTER_DX 2
#define REGISTER_BX 3

#define REGISTER_SEG_ES 0
#define REGISTER_SEG_CS 1
#define REGISTER_SEG_SS 2
#define REGISTER_SEG_DS 3

#define REGISTER_READ_WORD(reg) (regs[reg] | regs[reg+4])
#define REGISTER_WRITE_WORD(reg, value) (regs[reg] = value & 0xff;\
                                        regs[reg+4] = (value >> 8) & 0xff)

#endif