#ifndef EMULATOR_6502_ERROR_H
#define EMULATOR_6502_ERROR_H

// The error handler's callback function
typedef void (*E65_ErrorFunction_Handler)(const char* error);

// Sets the error handler. The default handler will output to stdout and then
// call exit(EXIT_FAILURE)
void E65_ErrorSetHandler(E65_ErrorFunction_Handler handler);

/* This function is called whenever the machine runs into a problem where it
 * cannot continue. For example, this is called when an invalid opcode is read.
 * Like E65_Debug, the parameters are used exactly like printf
 * @param format The string to use to format all other params using printf 
 * syntax
 * @param ... All other optional parameters
*/
void E65_Error(const char* format, ...);

#endif