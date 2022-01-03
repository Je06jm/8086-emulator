#include <E65/state.h>

void E65_StateInit() {
    A = 0;
    X = 0;
    Y = 0;
    Flags = 0;
}

E65_Byte A = 0, X = 0, Y = 0;
E65_Byte Flags = 0;
E65_Byte Stack = 0;
E65_Word IP = 0;
