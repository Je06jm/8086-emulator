#include <E86/module.h>
#include <E86/memory.h>
#include <E86/E86.h>
#include <E86/error.h>

#include <stdio.h>
#include <stdbool.h>

E86_Byte ROM[0x2000];
E86_Byte RAM[0x400];
E86_Byte OUT[0x20];
bool running = true;

E86_Byte ROM_read(E86_Word addr) {
    if (addr >= 0xe000) {
        return ROM[addr - 0xe000];
    }

    return 0;
}

void ROM_init() {
    FILE* file = fopen("ROM.bin", "rb");
    if (file == NULL) {
        E86_Error("Could not open file 'ROM.bin'");
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size < 0x2000) {
        E86_Error("ROM.bin is less than 8 KiBs");
    } else if (size > 0x2000) {
        E86_Error("ROM.bin is more than 8 KiBs");
    }

    fread(ROM, 1, size, file);
    fclose(file);

    E86_MemoryRegisterReadByte(ROM_read);
}

E86_Byte RAM_read(E86_Word addr) {
    if (addr < 0x400) {
        return RAM[addr];
    }

    return 0;
}

void RAM_write(E86_Word addr, E86_Byte value) {
    if (addr < 0x400) {
        RAM[addr] = value;
    }
}

void RAM_init() {
    E86_MemoryRegisterReadByte(RAM_read);
    E86_MemoryRegisterWriteByte(RAM_write);
}

E86_Byte OUT_read(E86_Word addr) {
    if ((addr >= 0x8000) && (addr < 0x8020)) {
        running = false;
    }
    return 0;
}

void OUT_write(E86_Word addr, E86_Byte value) {
    if ((addr >= 0x8000) && (addr < 0x8020)) {
        OUT[addr - 0x8000] = value;
    }
}

void OUT_init() {
    E86_MemoryRegisterReadByte(OUT_read);
    E86_MemoryRegisterWriteByte(OUT_write);
}

void OUT_finish() {
    FILE* file = fopen("OUT.bin", "wb");
    if (file == NULL) {
        E86_Error("Could not open 'OUT.bin' for writting");
    }

    fwrite(OUT, 1, sizeof(OUT), file);
    fclose(file);
}

int main() {
    E86_Init();

    E86_Module ROMModule = {
        .init = ROM_init
    };

    E86_Module RAMModule = {
        .init = RAM_init
    };

    E86_Module OUTModule = {
        .init = OUT_init,
        .finish = OUT_finish
    };
    
    E86_ModuleAdd(&ROMModule);
    E86_ModuleAdd(&RAMModule);
    E86_ModuleAdd(&OUTModule);

    E86_ModuleInitModules();

    while (running) {
        E86_Tick();
    }

    E86_ModuleFinishModules();
    E86_Finish();
}
