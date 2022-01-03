#include <E65/error.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Default error handler
void E65_DefaultErrorHandler(const char* error) {
    printf("%s\n", error);
    exit(EXIT_FAILURE);
}

// The pointer to the current error handler
E65_ErrorFunction_Handler err_handler = E65_DefaultErrorHandler;

void E65_ErrorSetHandler(E65_ErrorFunction_Handler handler) {
    err_handler = handler;
}

void E65_Error(const char* format, ...) {
    // Formats the error then call the error handler. If no error handler is
    // available, then print an error and then exit(EXIT_FAILURE)
    va_list va;
    va_start(va, format);

    char error[512];

    vsprintf_s(error, 512, format, va);

    if (err_handler != NULL) {
        err_handler(error);
    } else {
        printf("Could not find a error handler, shutting down\n");
        exit(EXIT_FAILURE);
    }

    va_end(va);
}