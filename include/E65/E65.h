#ifndef EMULATOR_6502_E65_H
#define EMULATOR_6502_E65_H

// Setups up the machine and allocates memory
void E65_Init();
// Deallocates memory
void E65_Finish();

// Advances the machine one step forward
void E65_Tick();

#endif