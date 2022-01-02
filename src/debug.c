#include <E86/debug.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void E86_Debug(const char* format, ...) {
#ifdef DEBUG
    va_list va;
    va_start(va, format);

    vprintf(format, va);

    va_end(va);
#endif
}