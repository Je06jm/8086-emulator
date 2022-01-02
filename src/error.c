#include <E86/error.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void E86_DefaultErrorHandler(const char* error) {
    printf("%s\n", error);
    exit(EXIT_FAILURE);
}

E86_ErrorFunction_Handler err_handler = E86_DefaultErrorHandler;

void E86_ErrorSetHandler(E86_ErrorFunction_Handler handler) {
    err_handler = handler;
}

void E86_Error(const char* format, ...) {
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