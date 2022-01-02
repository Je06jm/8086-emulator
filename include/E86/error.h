#ifndef EMULATOR_8086_ERROR_H
#define EMULATOR_8086_ERROR_H

typedef void (*E86_ErrorFunction_Handler)(const char* error);

void E86_ErrorSetHandler(E86_ErrorFunction_Handler handler);

void E86_Error(const char* format, ...);

#endif