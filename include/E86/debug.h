#ifndef EMULATOR_8086_DEBUG_H
#define EMULATOR_8086_DEBUG_H

void E86_Debug(const char* format, ...);

#ifdef DEBUG
#define DEBUG_PRINT(...) E86_Debug(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#endif