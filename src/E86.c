#include <E86/E86.h>

#include <E86/debug.h>
#include <E86/error.h>
#include <E86/flags.h>
#include <E86/interrupts.h>
#include <E86/memory.h>
#include <E86/module.h>
#include <E86/opcodes.h>
#include <E86/state.h>
#include <E86/types.h>

#define FAST_CARRY(x, y, c) (((x&1) & (y&1)) + (((x&1) ^ (y&1)) & (c&1)))

#define DEBUG_OP(name) DEBUG_PRINT("PC %x: %s\n", IP, name)
#define DEBUG_OP_OPERAND(name, op) E86_Debug("PC %x: %s 0x%x\n", IP, name, op)

bool first_tick = false;

void E86_Init() {
    E86_InterruptsInit();
    E86_MemoryInit();
    E86_ModuleInit();

    first_tick = true;
}

void E86_Finish() {
    E86_ModuleFinishModules();

    E86_ModuleFinish();
    E86_MemoryFinish();
}

E86_Byte FromDecimal(E86_Byte value) {
    E86_Byte result = value & 0xf;
    result += value / 0x10 * 10;
    return result;
}

E86_Byte ToDecimal(E86_Byte value) {
    E86_Byte result = value % 10;
    result += value / 10 * 0x10;
    return result;
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
    if (first_tick) {
        first_tick = false;
        IP = E86_MemoryReadByte(0Xfffc);
        IP |= E86_MemoryReadByte(0xfffd) << 8;

        Flags = 0;
        Stack = 0;
        A = 0;
        X = 0;
        Y = 0;
    }

    E86_ModuleTickModules();

    if (E86_InterruptHasNMInterrupt()) {
        PushStack(IP >> 8);
        PushStack(IP);
        PushStack(Flags);
        Flags &= ~FLAG_INTERRUPT;
        IP = E86_MemoryReadByte(0Xfffa);
        IP |= E86_MemoryReadByte(0xfffb) << 8;
        return;
    } else if (E86_InterruptsHasInterrupt() && (Flags & FLAG_INTERRUPT)) {
        PushStack(IP >> 8);
        PushStack(IP);
        PushStack(Flags);
        Flags &= ~FLAG_INTERRUPT;
        IP = E86_MemoryReadByte(0xfffe);
        IP |= E86_MemoryReadByte(0xffff);
        return;
    }

    E86_Byte mop;
    E86_Byte opcode;
    E86_Byte address;
    E86_Byte cc;
    E86_Byte oper = 0;
    E86_Word woper = 0;
    E86_Byte nextIP = 0;

    mop = E86_MemoryReadByte(IP);
    address = mop & MASK_BBB;
    opcode = mop & MASK_AAA;
    cc = mop & MASK_CC;

    if ((mop & 0b11111) == OPCODE_BRA) {
        E86_Byte conditions = mop >> 6;
        bool set = ((mop >> 5) & 1) == 1;
        E86_Byte bra_flag = 0;
        
        switch (conditions) {
            case FLAG_BRA_NEG:
                bra_flag = FLAG_NEGATIVE;
                break;
            case FLAG_BRA_CARRY:
                bra_flag = FLAG_CARRY;
                break;
            case FLAG_BRA_OVERFLOW:
                bra_flag = FLAG_OVERFLOW;
                break;
            case FLAG_BRA_ZERO:
                bra_flag = FLAG_ZERO;
                break;
        }

        nextIP = 2;
        if (((bool)(Flags & bra_flag)) == set) {
            oper = E86_MemoryReadByte(IP+1);
            if (oper & (1<<7)) {
                oper = ~oper;
                IP -= oper + 1;
            } else {
                IP += oper;
            }
        }
        IP += nextIP;
        return;
    }

    switch (mop) {
        case OPCODE_BRK:
            PushStack((IP+2)>>8);
            PushStack((IP+2));
            PushStack(Flags | FLAG_BREAK);
            Flags &= ~FLAG_INTERRUPT;
            DEBUG_OP("BRK");
            IP = E86_MemoryReadByte(0xfffe);
            IP |= E86_MemoryReadByte(0xffff);
            break;
        case OPCODE_JSR:
            PushStack((IP+2)>>8);
            PushStack(IP+2);
            woper = E86_MemoryReadByte(IP+1);
            woper |= E86_MemoryReadByte(IP+2) << 8;
            DEBUG_OP_OPERAND("JSR", woper);
            IP = woper;
            break;
        case OPCODE_RTI:
            Flags = PopStack();
            Flags &= ~(1<<5) & ~FLAG_BREAK;;
            DEBUG_OP("RTI");
            IP = PopStack();
            IP |= PopStack() << 8;
            break;
        case OPCODE_RTS:
            DEBUG_OP("RTS");
            IP = PopStack();
            IP |= PopStack() << 8;
            IP++;
            break;
        case OPCODE_PHP:
            PushStack(Flags | (1<<5));
            DEBUG_OP("PHP");
            nextIP = 1;
            break;
        case OPCODE_PLP:
            Flags = PopStack();
            Flags &= ~(1<<5) & ~FLAG_BREAK;
            DEBUG_OP("PLP");
            nextIP = 1;
            break;
        case OPCODE_PHA:
            PushStack(A);
            DEBUG_OP("PHA");
            nextIP = 1;
            break;
        case OPCODE_PLA:
            A = PopStack();
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= A == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("PLA");
            nextIP = 1;
            break;
        case OPCODE_DEY:
            Y--;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (Y & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= Y == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("DEY");
            nextIP = 1;
            break;
        case OPCODE_TAY:
            Y = A;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (Y & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= Y == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TAY");
            nextIP = 1;
            break;
        case OPCODE_INY:
            Y++;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (Y & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= Y == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("INY");
            nextIP = 1;
            break;
        case OPCODE_INX:
            X++;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (X & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= X == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("INX");
            nextIP = 1;
            break;
        case OPCODE_CLC:
            Flags &= ~FLAG_CARRY;
            DEBUG_OP("CLC");
            nextIP = 1;
            break;
        case OPCODE_SEC:
            Flags |= FLAG_CARRY;
            DEBUG_OP("SEC");
            nextIP = 1;
            break;
        case OPCODE_CLI:
            Flags &= ~FLAG_INTERRUPT;
            DEBUG_OP("CLI");
            nextIP = 1;
            break;
        case OPCODE_SEI:
            Flags |= FLAG_INTERRUPT;
            DEBUG_OP("SEI");
            nextIP = 1;
            break;
        case OPCODE_TYA:
            A = Y;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= A == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TYA");
            nextIP = 1;
            break;
        case OPCODE_CLV:
            Flags &= ~FLAG_OVERFLOW;
            DEBUG_OP("CLV");
            nextIP = 1;
            break;
        case OPCODE_CLD:
            Flags &= ~FLAG_DECIMAL;
            DEBUG_OP("CLD");
            nextIP = 1;
            break;
        case OPCODE_SED:
            Flags |= FLAG_DECIMAL;
            DEBUG_OP("SED");
            nextIP = 1;
            break;
        case OPCODE_TXA:
            A = X;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= A == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TXA");
            nextIP = 1;
            break;
        case OPCODE_TXS:
            Stack = X;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (Stack & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= Stack == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TXS");
            nextIP = 1;
            break;
        case OPCODE_TAX:
            X = A;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (X & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= X == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TAX");
            nextIP = 1;
            break;
        case OPCODE_TSX:
            X = Stack;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (X & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= X == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TSX");
            nextIP = 1;
            break;
        case OPCODE_DEX:
            X--;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (X & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= X == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("DEX");
            nextIP = 1;
            break;
        case OPCODE_NOP:
            nextIP = 1;
            break;
        default:
            switch (cc) {
                case 0b00:
                    switch (address) {
                        case ADDRESS_00_IMM:
                            woper = IP + 1;
                            nextIP = 2;
                            break;
                        case ADDRESS_00_ZERO:
                            woper = E86_MemoryReadByte(IP+1);
                            nextIP = 2;
                            break;
                        case ADDRESS_00_ABS:
                            woper = E86_MemoryReadByte(IP+1);
                            woper |= E86_MemoryReadByte(IP+2) << 8;
                            nextIP = 3;
                            break;
                        case ADDRESS_00_ZERO_X:
                            oper = E86_MemoryReadByte(IP+1);
                            woper = (oper+X) & 0xff;
                            nextIP = 2;
                            break;
                        case ADDRESS_00_ABS_X:
                            woper = E86_MemoryReadByte(IP+1);
                            woper |= E86_MemoryReadByte(IP+2);
                            woper += X;
                            nextIP = 3;
                            break;
                        default: goto unknown_address;
                    }
                    switch (opcode) {
                        case OPCODE_00_BIT:
                            oper = E86_MemoryReadByte(woper);
                            Flags &= ~FLAG_ZERO;
                            Flags |= (A & oper) == 0 ? FLAG_ZERO : 0;
                            Flags &= ~FLAG_NEGATIVE & ~FLAG_OVERFLOW;
                            Flags |= oper & (FLAG_NEGATIVE | FLAG_OVERFLOW);
                            DEBUG_OP_OPERAND("BIT", oper);
                            break;
                        case OPCODE_00_JMP:
                            woper = E86_MemoryReadByte(IP+1);
                            woper |= E86_MemoryReadByte(IP+2) << 8;
                            DEBUG_OP_OPERAND("JMP", woper);
                            IP = woper;
                            nextIP = 0;
                            break;
                        case OPCODE_00_JMP_IND:
                            woper = E86_MemoryReadByte(IP+1);
                            woper |= E86_MemoryReadByte(IP+2) << 8;
                            DEBUG_OP_OPERAND("JMP ()", woper);
                            IP = E86_MemoryReadByte(woper);
                            IP |= E86_MemoryReadByte(woper + 1);
                            nextIP = 0;
                            break;
                        case OPCODE_00_STY:
                            E86_MemoryWriteByte(woper, Y);
                            DEBUG_OP("STY");
                            break;
                        case OPCODE_00_LDY:
                            Y = E86_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (Y & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= Y == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP("LDY");
                            break;
                        case OPCODE_00_CPY:
                            woper = Y - E86_MemoryReadByte(woper);
                            Flags &= ~(FLAG_CARRY | FLAG_ZERO | FLAG_NEGATIVE);
                            Flags |= (woper & 0xff) == 0 ? FLAG_ZERO : 0;
                            Flags |= (woper & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= (woper >> 8) != 0 ? FLAG_OVERFLOW : 0;
                            DEBUG_OP_OPERAND("CPY", woper & 0xff);
                            break;
                        case OPCODE_00_CPX:
                            woper = X - E86_MemoryReadByte(woper);
                            Flags &= ~(FLAG_CARRY | FLAG_ZERO | FLAG_NEGATIVE);
                            Flags |= (woper & 0xff) == 0 ? FLAG_ZERO : 0;
                            Flags |= (woper & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= (woper >> 8) != 0 ? FLAG_OVERFLOW : 0;
                            DEBUG_OP_OPERAND("CPX", woper & 0xff);
                            break;
                        default: goto unknown_opcode;
                    }
                break;
                case 0b01:
                    switch (address) {
                        case ADDRESS_01_IND_ZERO_X:
                            oper = E86_MemoryReadByte(IP+1);
                            woper = E86_MemoryReadByte((oper+X) & 0xff);
                            woper = E86_MemoryReadByte((oper+X+1) & 0xff);
                            nextIP = 2;
                            break;
                        case ADDRESS_01_ZERO:
                            woper = E86_MemoryReadByte(IP+1);
                            nextIP = 2;
                            break;
                        case ADDRESS_01_IMM:
                            woper = IP+1;
                            nextIP = 2;
                            break;
                        case ADDRESS_01_ABS:
                            woper = E86_MemoryReadByte(IP+1);
                            woper |= E86_MemoryReadByte(IP+2) << 8;
                            nextIP = 3;
                            break;
                        case ADDRESS_01_IND_ZERO_Y:
                            oper = E86_MemoryReadByte(IP+1);
                            woper = E86_MemoryReadByte(oper);
                            woper |= E86_MemoryReadByte((oper+1) & 0xff);
                            woper += Y;
                            nextIP = 2;
                            break;
                        case ADDRESS_01_ABS_Y:
                            woper = E86_MemoryReadByte(IP+1);
                            woper |= E86_MemoryReadByte(IP+2) << 8;
                            woper += Y;
                            nextIP = 3;
                            break;
                        case ADDRESS_01_ABS_X:
                            woper = E86_MemoryReadByte(IP+1);
                            woper |= E86_MemoryReadByte(IP+2) << 8;
                            woper += X;
                            nextIP = 3;
                            break;
                        default: goto unknown_address;
                    }
                    switch (opcode) {
                        case OPCODE_01_ORA:
                            A |= E86_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= A == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP_OPERAND("ORA", E86_MemoryReadByte(woper));
                            break;
                        case OPCODE_01_AND:
                            A &= E86_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= A == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP_OPERAND("ANDA", E86_MemoryReadByte(woper));
                            break;
                        case OPCODE_01_EOR:
                            A ^= E86_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= A == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP_OPERAND("EOR", E86_MemoryReadByte(woper));
                            break;
                        case OPCODE_01_ADC:
                            woper = E86_MemoryReadByte(woper);
                            if (Flags & FLAG_DECIMAL) {
                                woper = FromDecimal(woper);
                                A = FromDecimal(A);
                            }
                            woper = A + woper + (Flags & FLAG_CARRY);
                            Flags &= ~FLAG_OVERFLOW;
                            {
                                bool C6 = FAST_CARRY(A>>6, woper>>6,
                                    FAST_CARRY(A>>5, woper>>5,
                                    FAST_CARRY(A>>4, woper>>4,
                                    FAST_CARRY(A>>3, woper>>3,
                                    FAST_CARRY(A>>2, woper>>2,
                                    FAST_CARRY(A>>1, woper>>1,
                                    FAST_CARRY(A, woper, 0)))))));
                                bool NOR_A7_B7 = (A & (1<<7)) | (woper & (1<<7));
                                NOR_A7_B7 = !NOR_A7_B7;

                                bool NAND_A7_B7 = (A & (1<<7)) & (woper & (1<<7));
                                NAND_A7_B7 = !NAND_A7_B7;

                                bool NOR_A7_B7_AND_C6 = NOR_A7_B7 & C6;
                                bool NAND_A7_B7_NOR_C6 = !(NAND_A7_B7 | C6);
                                
                                bool V = (NOR_A7_B7_AND_C6 | NAND_A7_B7_NOR_C6);

                                if (V) {
                                    Flags |= FLAG_OVERFLOW;
                                }
                            }
                            if (Flags & FLAG_DECIMAL) {
                                A = ToDecimal(woper);
                                DEBUG_OP_OPERAND("ADC (DECIMAL)", A);
                            } else {
                                A = woper;
                                DEBUG_OP_OPERAND("ADC", A);
                            }
                            Flags &= ~(FLAG_CARRY | FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (woper >> 8) & FLAG_CARRY;
                            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= A == 0 ? FLAG_ZERO : 0;
                            break;
                        case OPCODE_01_STA:
                            E86_MemoryWriteByte(woper, A);
                            DEBUG_OP_OPERAND("STA", woper);
                            break;
                        case OPCODE_01_LDA:
                            A = E86_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= A == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP_OPERAND("LDA", woper);
                            break;
                        case OPCODE_01_CMP:
                            oper = E86_MemoryReadByte(woper);
                            woper = A - oper;
                            Flags &= ~(FLAG_CARRY | FLAG_ZERO | FLAG_NEGATIVE);
                            Flags |= (woper & 0xff) == 0 ? FLAG_ZERO : 0;
                            Flags |= (woper & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= (woper >> 8) != 0 ? FLAG_OVERFLOW : 0;
                            DEBUG_OP_OPERAND("CMP", oper);
                            break;
                        case OPCODE_01_SBC:
                            oper = E86_MemoryReadByte(woper);
                            if (Flags & FLAG_DECIMAL) {
                                oper = FromDecimal(oper);
                                A = FromDecimal(A);
                            }
                            oper = ~oper;
                            {
                                bool C6 = FAST_CARRY(A>>6, oper>>6,
                                    FAST_CARRY(A>>5, oper>>5,
                                    FAST_CARRY(A>>4, oper>>4,
                                    FAST_CARRY(A>>3, oper>>3,
                                    FAST_CARRY(A>>2, oper>>2,
                                    FAST_CARRY(A>>1, oper>>1,
                                    FAST_CARRY(A, oper, 0)))))));
                                bool NOR_A7_B7 = (A & (1<<7)) | (oper & (1<<7));
                                NOR_A7_B7 = !NOR_A7_B7;

                                bool NAND_A7_B7 = (A & (1<<7)) & (oper & (1<<7));
                                NAND_A7_B7 = !NAND_A7_B7;

                                bool NOR_A7_B7_AND_C6 = NOR_A7_B7 & C6;
                                bool NAND_A7_B7_NOR_C6 = !(NAND_A7_B7 | C6);
                                
                                bool V = (NOR_A7_B7_AND_C6 | NAND_A7_B7_NOR_C6);

                                if (V) {
                                    Flags |= FLAG_OVERFLOW;
                                }
                            }
                            woper = A + oper + (Flags & FLAG_CARRY);
                            if (Flags & FLAG_DECIMAL) {
                                A = ToDecimal(woper);
                                DEBUG_OP_OPERAND("SBC (DECIMAL)", A);
                            } else {
                                A = woper;
                                DEBUG_OP_OPERAND("SBC", A);
                            }
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= A == 0 ? FLAG_ZERO : 0;
                            break;
                        default: goto unknown_opcode;
                    }
                break;
                case 0b10:
                    switch (address) {
                        case ADDRESS_10_IMM:
                            woper = IP+1;
                            nextIP = 2;
                            break;
                        case ADDRESS_10_ZERO:
                            woper = E86_MemoryReadByte(IP+1);
                            nextIP = 2;
                            break;
                        case ADDRESS_10_ACCU:
                            nextIP = 1;
                            break;
                        case ADDRESS_10_ABS:
                            woper = E86_MemoryReadByte(IP+1);
                            woper |= E86_MemoryReadByte(IP+2) << 8;
                            nextIP = 3;
                            break;
                        case ADDRESS_10_ZERO_X:
                            oper = E86_MemoryReadByte(IP+1);
                            woper = (oper + X) & 0xff;
                            nextIP = 2;
                            break;
                        case ADDRESS_10_ABS_X:
                            woper = E86_MemoryReadByte(IP+1);
                            woper |= E86_MemoryReadByte(IP+2);
                            woper += X;
                            nextIP = 3;
                            break;
                        default: goto unknown_address;
                    }
                    switch (opcode) {
                        case OPCODE_10_ASL:
                            Flags &= ~FLAG_CARRY;
                            if (address == ADDRESS_10_ACCU) {
                                woper = A << 1;
                                Flags |= (woper >> 8) & FLAG_CARRY;
                                A = woper;
                                Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                                Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                                Flags |= A == 0 ? FLAG_ZERO : 0;
                            } else {
                                E86_Word temp = E86_MemoryReadByte(woper);
                                temp = temp << 1;
                                Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                                Flags |= (temp & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                                Flags |= temp == 0 ? FLAG_ZERO : 0;
                                Flags |= (temp >> 8) & FLAG_CARRY;
                                E86_MemoryWriteByte(woper, temp);
                            }
                            DEBUG_OP("ASL");
                            break;
                        case OPCODE_10_ROL: {
                            E86_Byte old_flags = Flags;
                            E86_Word temp;
                            Flags &= ~FLAG_CARRY;
                            if (address == ADDRESS_10_ACCU) {
                                temp = A << 1;
                                temp |= old_flags & FLAG_CARRY;
                                Flags |= (temp >> 8) & FLAG_CARRY;
                                A = temp;
                            } else {
                                temp = E86_MemoryReadByte(woper) << 1;
                                temp |= old_flags & FLAG_CARRY;
                                Flags |= (temp >> 8) & FLAG_CARRY;
                                E86_MemoryWriteByte(woper, temp);                            
                            }
                            DEBUG_OP("ROL");
                            break; }
                        case OPCODE_10_LSR:
                            Flags &= ~FLAG_CARRY;
                            if (address == ADDRESS_10_ACCU) {
                                Flags |= A & FLAG_CARRY;
                                A >>= 1;
                                Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                                Flags |= A == 0 ? FLAG_ZERO : 0;
                            } else {
                                oper = E86_MemoryReadByte(woper);
                                Flags |= oper & FLAG_CARRY;
                                oper >>= 1;
                                Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                                Flags |= oper == 0 ? FLAG_ZERO : 0;
                                E86_MemoryWriteByte(woper, oper);
                            }
                            DEBUG_OP("LSR");
                            break;
                        case OPCODE_10_ROR: {
                            E86_Byte old_flags = Flags;
                            E86_Word temp;
                            Flags &= ~FLAG_CARRY;
                            if (address == ADDRESS_10_ACCU) {
                                Flags |= A & FLAG_CARRY;
                                temp = A >> 1;
                                temp |= (old_flags & FLAG_CARRY) << 7;
                                A = temp;
                            } else {
                                temp = E86_MemoryReadByte(woper);
                                Flags |= temp & FLAG_CARRY;
                                temp = temp >> 1;
                                temp |= (old_flags & FLAG_CARRY) << 7;
                                E86_MemoryWriteByte(woper, temp);                            
                            }
                            DEBUG_OP("ROR");
                            break; }
                        case OPCODE_10_STX:
                            E86_MemoryWriteByte(woper, X);
                            DEBUG_OP_OPERAND("STX", woper);
                            break;
                        case OPCODE_10_LDX:
                            X = E86_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (X & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= X == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP_OPERAND("LDX", woper);
                            break;
                        case OPCODE_10_DEC:
                            oper = E86_MemoryReadByte(woper);
                            oper--;
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (oper & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= oper == 0 ? FLAG_ZERO : 0;
                            E86_MemoryWriteByte(woper, oper);
                            DEBUG_OP_OPERAND("DEC", woper);
                            break;
                        case OPCODE_10_INC:
                            oper = E86_MemoryReadByte(woper);
                            oper++;
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (oper & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= oper == 0 ? FLAG_ZERO : 0;
                            E86_MemoryWriteByte(woper, oper);
                            DEBUG_OP_OPERAND("INC", woper);
                            break;
                        default: goto unknown_opcode;
                    }
                break;
                default: goto unknown_opcode;
            }
    }
    IP += nextIP;

    return;

unknown_opcode:;
    E86_Error("Unknown opcode: 0x%x", mop);

unknown_address:;
    E86_Error("Unknown addressing mode for opcode: 0x%x", mop);
}