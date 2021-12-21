#ifndef EMU8086_STATE_H
#define EMU8086_STATE_H

#include <types.h>

#define REGISTER_AL 0
#define REGISTER_CL 1
#define REGISTER_DL 2
#define REGISTER_BL 3

#define REGISTER_AH 4
#define REGISTER_CH 5
#define REGISTER_DH 6
#define REGISTER_BH 7

#define REGISTER_AX 0
#define REGISTER_CX 1
#define REGISTER_DX 2
#define REGISTER_BX 3
#define REGISTER_SP 4
#define REGISTER_BP 5
#define REGISTER_SI 6
#define REGISTER_DI 7

#define REGISTER_ES 0
#define REGISTER_CS 1
#define REGISTER_SS 2
#define REGISTER_DS 3

#define FLAG_CF (1<<0)
#define FLAG_PF (1<<2)
#define FLAG_AF (1<<4)
#define FLAG_ZF (1<<6)
#define FLAG_SF (1<<7)
#define FLAG_TF (1<<8)
#define FLAG_IF (1<<9)
#define FLAG_DF (1<<10)
#define FLAG_OF (1<<11)

extern word_t Emu8086_State_Regs[8];
extern word_t Emu8086_State_Segs[4];

extern word_t Emu8086_State_IP;
extern word_t Emu8086_State_Flags;

extern bool Emu8086_State_Running;

// State helper macros

#define FLAG_SET(flag)\
    Emu8086_State_Flags = Emu8086_State_Flags | flag

#define FLAG_RESET(flag)\
    Emu8086_State_Flags = Emu8086_State_Flags & ~flag;

#define GET_LOWER(reg)\
    ((byte_t)Emu8086_State_Regs[reg])

#define GET_UPPER(reg)\
    ((byte_t)(Emu8086_State_Regs[reg] >> 8))

#define SET_LOWER(reg, value)\
    Emu8086_State_Regs[reg] = Emu8086_State_Regs[reg] & 0xff00 |\
    ((byte_t)value);

#define SET_UPPER(reg, value)\
    Emu8086_State_Regs[reg] = Emu8086_State_Regs[reg] & 0xff |\
    (((byte_t)value) << 8);

#endif