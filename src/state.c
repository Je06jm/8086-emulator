#include <E86/state.h>

void E86_StateInit() {
    Running = true;
    A = 0;
    X = 0;
    Y = 0;
    Flags = 0;
}

bool Running = true;

E86_Byte A = 0, X = 0, Y = 0;
E86_Byte Flags = 0;
E86_Byte Stack = 0;
E86_Word IP = 0;
