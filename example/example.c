#include <E65/module.h>
#include <E65/memory.h>
#include <E65/E65.h>
#include <E65/error.h>

#include <stdio.h>
#include <stdbool.h>

E65_Byte ROM[0x2000];
E65_Byte RAM[0x400];
E65_Byte OUT[0x20];
bool running = true;

E65_Byte ROM_read(E65_Word addr) {
    if (addr >= 0xe000) {
        return ROM[addr - 0xe000];
    }

    return 0;
}

void ROM_init() {
    FILE* file = fopen("ROM.bin", "rb");
    if (file == NULL) {
        E65_Error("Could not open file 'ROM.bin'");
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size < 0x2000) {
        E65_Error("ROM.bin is less than 8 KiBs");
    } else if (size > 0x2000) {
        E65_Error("ROM.bin is more than 8 KiBs");
    }

    fread(ROM, 1, size, file);
    fclose(file);

    E65_MemoryRegisterReadByte(ROM_read);
}

E65_Byte RAM_read(E65_Word addr) {
    if (addr < 0x400) {
        return RAM[addr];
    }

    return 0;
}

void RAM_write(E65_Word addr, E65_Byte value) {
    if (addr < 0x400) {
        RAM[addr] = value;
    }
}

void RAM_init() {
    E65_MemoryRegisterReadByte(RAM_read);
    E65_MemoryRegisterWriteByte(RAM_write);
}

E65_Byte OUT_read(E65_Word addr) {
    if ((addr >= 0x8000) && (addr < 0x8020)) {
        running = false;
    }
    return 0;
}

void OUT_write(E65_Word addr, E65_Byte value) {
    if ((addr >= 0x8000) && (addr < 0x8020)) {
        OUT[addr - 0x8000] = value;
    }
}

void OUT_init() {
    E65_MemoryRegisterReadByte(OUT_read);
    E65_MemoryRegisterWriteByte(OUT_write);
}

void OUT_finish() {
    FILE* file = fopen("OUT.bin", "wb");
    if (file == NULL) {
        E65_Error("Could not open 'OUT.bin' for writting");
    }

    fwrite(OUT, 1, sizeof(OUT), file);
    fclose(file);
}

int main() {
    E65_Init();

    E65_Module ROMModule = {
        .init = ROM_init
    };

    E65_Module RAMModule = {
        .init = RAM_init
    };

    E65_Module OUTModule = {
        .init = OUT_init,
        .finish = OUT_finish
    };
    
    E65_ModuleAdd(&ROMModule);
    E65_ModuleAdd(&RAMModule);
    E65_ModuleAdd(&OUTModule);

    E65_ModuleInitModules();

    while (running) {
        E65_Tick();
    }

    E65_ModuleFinishModules();
    E65_Finish();
}
