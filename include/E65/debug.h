#ifndef EMULATOR_6502_DEBUG_H
#define EMULATOR_6502_DEBUG_H

/* A utility function to help debug the CPU. It's parameters are exactly like 
 * printf
 * @param format The string to use to format all other params using printf 
 * syntax
 * @param ... All other optional parameters
*/
void E65_Debug(const char* format, ...);

#ifdef DEBUG
// A macro that is used to enable debug printing when DEBUG is defined
#define DEBUG_PRINT(...) E65_Debug(__VA_ARGS__)
#else
// A macro that is used to enable debug printing when DEBUG is defined
#define DEBUG_PRINT(...)
#endif

#endif