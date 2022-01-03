#include <E65/debug.h>

#include <stdarg.h>
#include <stdio.h>

void E65_Debug(const char* format, ...) {
    va_list va;
    va_start(va, format);

    vprintf(format, va);

    va_end(va);
}