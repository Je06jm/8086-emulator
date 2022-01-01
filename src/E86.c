#include <E86/E86.h>

#include <E86/flags.h>
#include <E86/interrupts.h>
#include <E86/memory.h>
#include <E86/module.h>
#include <E86/opcodes.h>
#include <E86/port.h>
#include <E86/state.h>
#include <E86/types.h>

void E86_Init() {
    E86_InterruptsInit();
    E86_MemoryInit();
    E86_ModuleInit();
    E86_PortInit();

    E86_ModuleInitModules();
}

void E86_Finish() {
    E86_ModuleFinishModules();

    E86_PortFinish();
    E86_ModuleFinish();
    E86_MemoryFinish();
}

void PushStack(E86_Byte value) {
    E86_Word sp = 0x100 | Stack;
    Stack--;

    E86_MemoryWriteByte(sp, value);
}

E86_Byte PopStack() {
    Stack++;
    E86_Word sp = 0x100 | Stack;

    return E86_MemoryReadByte(sp);
}

void E86_Tick() {
    E86_ModuleTickModules();

    if (E86_InterruptsHasNMInterrupt()) {
        // TODO: Finish this
    } else if (E86_InterruptsHasInterrupt()) {
        // TODO: Finish this
    }

    E86_Byte opcode;
    E86_Byte address;
    E86_Byte cc;

    if (Running) {
        cc = E86_MemoryReadByte(IP);
        address = cc & MASK_BBB;
        opcode = cc & MASK_AAA;
        cc &= MASK_CC;

        switch (cc) {
            case 0b00:
                switch (opcode) {
                    case OPCODE_00_BIT: break;
                    case OPCODE_00_JMP: break;
                    case OPCODE_00_JMPABS: break;
                    case OPCODE_00_STY: break;
                    case OPCODE_00_LDY: break;
                    case OPCODE_00_CPY: break;
                    case OPCODE_00_CPX: break;
                    default: goto unknown_opcode;
                }
            break;
            case 0b01:
                switch (opcode) {
                    case OPCODE_01_ORA: break;
                    case OPCODE_01_AND: break;
                    case OPCODE_01_EOR: break;
                    case OPCODE_01_ADC: break;
                    case OPCODE_01_STA: break;
                    case OPCODE_01_LBA: break;
                    case OPCODE_01_CMP: break;
                    case OPCODE_01_SBC: break;
                    default: goto unknown_opcode;
                }
            break;
            case 0b10:
                switch (opcode) {
                    case OPCODE_10_ASL: break;
                    case OPCODE_10_ROL: break;
                    case OPCODE_10_LSR: break;
                    case OPCODE_10_ROR: break;
                    case OPCODE_10_STX: break;
                    case OPCODE_10_LDX: break;
                    case OPCODE_10_DEC: break;
                    case OPCODE_10_INC: break;
                    default: goto unknown_opcode;
                }
            break;
            default: goto unknown_opcode;
        }
    }

unknown_opcode:;
    return;
}
