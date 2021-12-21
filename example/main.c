#include <Emu8086/core.h>
#include <Emu8086/memory.h>
#include <Emu8086/module.h>
#include <Emu8086/state.h>

#include <stdio.h>
#include <stdlib.h>

#define RAM_START_ADDRESS 0x80000
#define ROM_START_ADDRESS 0xF0000
#define RES_START_ADDRESS 0x20000

byte_t RAM[1024];
byte_t ROM[8096];
byte_t RES[512];

bool RAMInRange(address_t* address) {
    if (*address >= RAM_START_ADDRESS) {
        address -= RAM_START_ADDRESS;
        return address < sizeof(RAM);
    }

    return false;
}

byte_t RAMReadByte(address_t address) {
    if (RAMInRange(&address)) {
        return RAM[address];
    }
    return 0;
}

void RAMWriteByte(address_t address, byte_t value) {
    if (RAMInRange(&address)) {
        RAM[address] = value;
    }
}

word_t RAMReadWord(address_t address) {
    word_t value = 0;
    address_t address2 = address + 1;
    if (RAMInRange(&address)) {
        value = RAM[address];
    }
    if (RAMInRange(&address2)) {
        value = RAM[address2] << 8;
    }

    return value;
}

void RAMWriteWord(address_t address, word_t value) {
    address_t address2 = address + 1;
    if (RAMInRange(&address)) {
        RAM[address] = (byte_t)value;
    }
    if (RAMInRange(&address2)) {
        RAM[address2] = (byte_t)(value >> 8);
    }
}

void RAMInit() {
    Emu8086_Memory_RegisterReadByte(RAMReadByte);
    Emu8086_Memory_RegisterWriteByte(RAMWriteByte);
    Emu8086_Memory_RegisterReadWord(RAMReadWord);
    Emu8086_Memory_RegisterWriteWord(RAMWriteWord);
}

bool ROMInRange(address_t* address) {
    if (*address >= ROM_START_ADDRESS) {
        address -= ROM_START_ADDRESS;
        return address < sizeof(ROM);
    }

    return false;
}

byte_t ROMReadByte(address_t address) {
    if (ROMInRange(&address)) {
        return ROM[address];
    }
    return 0;
}

word_t ROMReadWord(address_t address) {
    word_t value = 0;
    address_t address2 = address + 1;
    if (ROMInRange(&address)) {
        value = ROM[address];
    }
    if (ROMInRange(&address2)) {
        value = ROM[address2] << 8;
    }

    return value;
}

void ROMInit() {
    Emu8086_Memory_RegisterReadByte(ROMReadByte);
    Emu8086_Memory_RegisterReadWord(ROMReadWord);
}

bool RESInRange(address_t* address) {
    if (*address >= RES_START_ADDRESS) {
        address -= RES_START_ADDRESS;
        return address < sizeof(ROM);
    }

    return false;
}

void RESWriteByte(address_t address, byte_t value) {
    if (RESInRange(&address)) {
        RES[address] = value;
    }
}

void RESWriteWord(address_t address, word_t value) {
    address_t address2 = address + 1;
    if (RESInRange(&address)) {
        RES[address] = (byte_t)value;
    }
    if (RESInRange(&address2)) {
        RES[address2] = (byte_t)(value >> 8);
    }
}

void RESInit() {
    Emu8086_Memory_RegisterWriteByte(RESWriteByte);
    Emu8086_Memory_RegisterWriteWord(RESWriteWord);
}

void LoadEmuModules() {
    Module_t RAMModule = {
        .Init = RAMInit
    };

    Module_t ROMModule = {
        .Init = ROMInit
    };

    Module_t RESModule = {
        .Init = RESInit
    };

    Emu8086_Module_Register(&RAMModule);
    Emu8086_Module_Register(&ROMModule);
    Emu8086_Module_Register(&RESModule);
}

int main() {
    // Setup machine
    Emu8086_Core_Init();

    Emu8086_State_Running = true;
    Emu8086_State_IP = 0;
    Emu8086_State_Segs[REGISTER_CS] = 0xf0;

    // Load ROM
    FILE* file = fopen("ROM.bin", "rb");
    if (file == NULL) {
        printf("Could not open ROM.bin");
        Emu8086_Core_Finish();
        exit(EXIT_FAILURE);
    }

    fread(ROM, sizeof(byte_t), sizeof(ROM), file);

    fclose(file);

    // Run machine
    while (Emu8086_State_Running) {
        Emu8086_Core_Tick();
    }
    
    // Save RES
    file = fopen("RESULT.bin", "wb");
    if (file == NULL) {
        printf("Could not open RESULT.bin");
    } else {
        fwrite(ROM, sizeof(byte_t), sizeof(RES), file);
        fclose(file);
    }

    Emu8086_Core_Finish();
}
