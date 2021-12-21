#include <Emu8086/types.h>

word_t Emu8086_State_Regs[8];
word_t Emu8086_State_Segs[4];

word_t Emu8086_State_IP;
word_t Emu8086_State_Flags;

bool Emu8086_State_Running;
