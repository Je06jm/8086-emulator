#include "error.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char buffer[512];

void print_error(const char* format, ...) {
    va_list list;
    va_start(list, format);
    
    vsprintf_s(buffer, sizeof(buffer), format, list);
    
    va_end(list);

    // TODO: Implement a better error handler
    printf(buffer);

    exit(EXIT_FAILURE);
}