#ifndef EMULATOR_6502_FLAGS_H
#define EMULATOR_6502_FLAGS_H

// Different flags used in the 6502
// https://www.atarimagazines.com/compute/issue53/047_1_All_About_The_Status_Register.php
#define FLAG_CARRY (1<<0)
#define FLAG_ZERO (1<<1)
#define FLAG_INTERRUPT (1<<2)
#define FLAG_DECIMAL (1<<3)
#define FLAG_BREAK (1<<4)
// The overflow flag is a bit confusing, so here is a link to more info on it
// http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
#define FLAG_OVERFLOW (1<<6)
#define FLAG_NEGATIVE (1<<7)

#endif