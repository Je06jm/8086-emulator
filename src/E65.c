#include <E65/E65.h>

#include <E65/debug.h>
#include <E65/error.h>
#include <E65/flags.h>
#include <E65/interrupts.h>
#include <E65/memory.h>
#include <E65/module.h>
#include <E65/opcodes.h>
#include <E65/state.h>
#include <E65/types.h>

// This is uses the principles of a fast adder to calculate a the carry.
// This is only used when calculating the overflow flag
#define FAST_CARRY(x, y, c) (((x&1) & (y&1)) + (((x&1) ^ (y&1)) & (c&1)))

// Helper debug macros
#define DEBUG_OP(name) DEBUG_PRINT("PC %x: %s\n", IP, name)
#define DEBUG_OP_OPERAND(name, op) DEBUG_PRINT("PC %x: %s 0x%x\n", IP, name, op)

// This is set true until the first tick after init is called
bool first_tick = false;

void E65_Init() {
    // Initializes the system
    E65_InterruptsInit();
    E65_MemoryInit();
    E65_ModuleInit();

    // At this point, no memory devices are available so IP can't be set yet.
    // Instead, we set first_tick
    first_tick = true;
}

void E65_Finish() {
    // Cleanup the system
    E65_ModuleFinish();
    E65_MemoryFinish();
}

/* The 6502 supports a decimal mode. This functions converts decimal numbers to
 * binary numbers to allow for easy math
 * @param value The decimal value
 * @returns The binary value
*/
E65_Byte FromDecimal(E65_Byte value) {
    E65_Byte result = value & 0xf;
    result += value / 0x10 * 10;
    return result;
}

/* The 6502 supports a decimal mode. This functions converts binary numbers to
 * decimal numbers
 * @param value The binary value
 * @returns The decimal value
*/
E65_Byte ToDecimal(E65_Byte value) {
    E65_Byte result = value % 10;
    result += value / 10 * 0x10;
    return result;
}

/* A helper function to help handle the stack. This function pushes a value 
 * onto the stack
 * @param value The value to push
*/
void PushStack(E65_Byte value) {
    E65_Word sp = 0x100 | Stack;
    Stack--;

    E65_MemoryWriteByte(sp, value);
}

/* A helper function to help handle the stack. This function pops a value off 
 * the stack
 * @returns The value pulled from the stack
*/
E65_Byte PopStack() {
    Stack++;
    E65_Word sp = 0x100 | Stack;

    return E65_MemoryReadByte(sp);
}

void E65_Tick() {
    if (first_tick) {
        // If this is the first tick, set all the registers to their initial
        // value
        first_tick = false;
        IP = E65_MemoryReadByte(0Xfffc);
        IP |= E65_MemoryReadByte(0xfffd) << 8;

        Flags = 0;
        Stack = 0;
        A = 0;
        X = 0;
        Y = 0;
    }

    // Tick all the modules
    E65_ModuleTickModules();

    // Check for NM interrupts and interrupts
    if (E65_InterruptHasNMInterrupt()) {
        // There is an NM interrupt. Push IP and flags then jump to the address
        // stored in the NM interrupt pointer (0xfffa)
        PushStack(IP >> 8);
        PushStack(IP);
        PushStack(Flags);
        Flags &= ~FLAG_INTERRUPT;
        IP = E65_MemoryReadByte(0Xfffa);
        IP |= E65_MemoryReadByte(0xfffb) << 8;
        return;
    } else if (E65_InterruptsHasInterrupt() && (Flags & FLAG_INTERRUPT)) {
        // There is an interrupt. Push IP and flags then jump to the address
        // stored in the interrupt pointer (0xfffe)
        PushStack(IP >> 8);
        PushStack(IP);
        PushStack(Flags);
        Flags &= ~FLAG_INTERRUPT;
        IP = E65_MemoryReadByte(0xfffe);
        IP |= E65_MemoryReadByte(0xffff);
        return;
    }

    E65_Byte mop;
    E65_Byte opcode;
    E65_Byte address;
    E65_Byte cc;
    E65_Byte oper = 0;
    E65_Word woper = 0;
    E65_Byte nextIP = 0;

    // Read the next opcode and extact opcode, address, and c for decoding
    mop = E65_MemoryReadByte(IP);
    address = mop & MASK_BBB;
    opcode = mop & MASK_AAA;
    cc = mop & MASK_CC;

    // Test to see if the instruction is a branch
    if ((mop & 0b11111) == OPCODE_BRA) {
        E65_Byte conditions = mop >> 6;
        bool set = ((mop >> 5) & 1) == 1;
        E65_Byte bra_flag = 0;
        
        // Determin which flag to test
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
            // The test passed so we jump to a new relative position
            oper = E65_MemoryReadByte(IP+1);
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

    // Test instruction for misc opcodes
    switch (mop) {
        case OPCODE_BRK:
            // This instructions triggers an interrupt, but it also set's the
            // break flag when pushing Flags onto the stack
            PushStack((IP+2)>>8);
            PushStack((IP+2));
            PushStack(Flags | FLAG_BREAK);
            Flags &= ~FLAG_INTERRUPT;
            DEBUG_OP("BRK");
            IP = E65_MemoryReadByte(0xfffe);
            IP |= E65_MemoryReadByte(0xffff);
            break;
        case OPCODE_JSR:
            // Pushes IP+2 onto stack and then jump to the address specified by
            // the operand
            PushStack((IP+2)>>8);
            PushStack(IP+2);
            woper = E65_MemoryReadByte(IP+1);
            woper |= E65_MemoryReadByte(IP+2) << 8;
            DEBUG_OP_OPERAND("JSR", woper);
            IP = woper;
            break;
        case OPCODE_RTI:
            // Pops the flags and IP off of the stack
            Flags = PopStack();
            Flags &= ~(1<<5) & ~FLAG_BREAK;;
            DEBUG_OP("RTI");
            IP = PopStack();
            IP |= PopStack() << 8;
            break;
        case OPCODE_RTS:
            // Pops the IP off of the stack
            DEBUG_OP("RTS");
            IP = PopStack();
            IP |= PopStack() << 8;
            IP++;
            break;
        case OPCODE_PHP:
            // Pushes the flags onto the stack
            PushStack(Flags | (1<<5));
            DEBUG_OP("PHP");
            nextIP = 1;
            break;
        case OPCODE_PLP:
            // Pops the flags off of the stack
            Flags = PopStack();
            Flags &= ~(1<<5) & ~FLAG_BREAK;
            DEBUG_OP("PLP");
            nextIP = 1;
            break;
        case OPCODE_PHA:
            // Pushes the A register onto the stack
            PushStack(A);
            DEBUG_OP("PHA");
            nextIP = 1;
            break;
        case OPCODE_PLA:
            // Pops value from stack into the A register
            A = PopStack();
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= A == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("PLA");
            nextIP = 1;
            break;
        case OPCODE_DEY:
            // Decrements the Y register
            Y--;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (Y & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= Y == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("DEY");
            nextIP = 1;
            break;
        case OPCODE_TAY:
            // Copies the value from the A register to the Y register
            Y = A;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (Y & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= Y == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TAY");
            nextIP = 1;
            break;
        case OPCODE_INY:
            // Increments the Y register
            Y++;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (Y & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= Y == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("INY");
            nextIP = 1;
            break;
        case OPCODE_INX:
            // Increments the X register
            X++;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (X & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= X == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("INX");
            nextIP = 1;
            break;
        case OPCODE_CLC:
            // Clear the carry flag
            Flags &= ~FLAG_CARRY;
            DEBUG_OP("CLC");
            nextIP = 1;
            break;
        case OPCODE_SEC:
            // Sets the carry flag
            Flags |= FLAG_CARRY;
            DEBUG_OP("SEC");
            nextIP = 1;
            break;
        case OPCODE_CLI:
            // Clear the interrupt flag
            Flags &= ~FLAG_INTERRUPT;
            DEBUG_OP("CLI");
            nextIP = 1;
            break;
        case OPCODE_SEI:
            // Set the interrupt flag
            Flags |= FLAG_INTERRUPT;
            DEBUG_OP("SEI");
            nextIP = 1;
            break;
        case OPCODE_TYA:
            // Copies the value from the Y register to the A register
            A = Y;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= A == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TYA");
            nextIP = 1;
            break;
        case OPCODE_CLV:
            // Clear the overflow flag
            Flags &= ~FLAG_OVERFLOW;
            DEBUG_OP("CLV");
            nextIP = 1;
            break;
        case OPCODE_CLD:
            // Clear the decimal flag
            Flags &= ~FLAG_DECIMAL;
            DEBUG_OP("CLD");
            nextIP = 1;
            break;
        case OPCODE_SED:
            // Set the decimal flag
            Flags |= FLAG_DECIMAL;
            DEBUG_OP("SED");
            nextIP = 1;
            break;
        case OPCODE_TXA:
            // Copies the value from the X register to the A register
            A = X;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= A == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TXA");
            nextIP = 1;
            break;
        case OPCODE_TXS:
            // Copies the value from the X register to the Stack register
            Stack = X;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (Stack & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= Stack == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TXS");
            nextIP = 1;
            break;
        case OPCODE_TAX:
            // Copies the value from the A register to the X register
            X = A;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (X & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= X == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TAX");
            nextIP = 1;
            break;
        case OPCODE_TSX:
            // Copies the value from the Stack register to the X register
            X = Stack;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (X & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= X == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("TSX");
            nextIP = 1;
            break;
        case OPCODE_DEX:
            // Decrements the X register
            X--;
            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
            Flags |= (X & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
            Flags |= X == 0 ? FLAG_ZERO : 0;
            DEBUG_OP("DEX");
            nextIP = 1;
            break;
        case OPCODE_NOP:
            // Does nothing
            nextIP = 1;
            break;
        default:
            // Decode instruction
            switch (cc) {
                case 0b00:
                    // Decode the address of the operand
                    switch (address) {
                        case ADDRESS_00_IMM:
                            // Gets the address of the bytes after the current
                            // IP
                            woper = IP + 1;
                            nextIP = 2;
                            break;
                        case ADDRESS_00_ZERO:
                            // Gets the address of a byte in the zero page
                            woper = E65_MemoryReadByte(IP+1);
                            nextIP = 2;
                            break;
                        case ADDRESS_00_ABS:
                            // Gets the address of a byte anywhere in memory
                            woper = E65_MemoryReadByte(IP+1);
                            woper |= E65_MemoryReadByte(IP+2) << 8;
                            nextIP = 3;
                            break;
                        case ADDRESS_00_ZERO_X:
                            // Gets the indexed address of a byte in the zero
                            // page
                            oper = E65_MemoryReadByte(IP+1);
                            woper = (oper+X) & 0xff;
                            nextIP = 2;
                            break;
                        case ADDRESS_00_ABS_X:
                            // Gets the indexed address of a byte anywhere in
                            // memory
                            woper = E65_MemoryReadByte(IP+1);
                            woper |= E65_MemoryReadByte(IP+2);
                            woper += X;
                            nextIP = 3;
                            break;
                        default: goto unknown_address;
                    }
                    switch (opcode) {
                        case OPCODE_00_BIT:
                            /* Ands the operand and the A register and then
                             * sets the flags accordingly. The A register is not
                             * modified
                            */
                            oper = E65_MemoryReadByte(woper);
                            Flags &= ~FLAG_ZERO;
                            Flags |= (A & oper) == 0 ? FLAG_ZERO : 0;
                            Flags &= ~FLAG_NEGATIVE & ~FLAG_OVERFLOW;
                            Flags |= oper & (FLAG_NEGATIVE | FLAG_OVERFLOW);
                            DEBUG_OP_OPERAND("BIT", oper);
                            break;
                        case OPCODE_00_JMP:
                            // Sets IP to operand
                            woper = E65_MemoryReadByte(IP+1);
                            woper |= E65_MemoryReadByte(IP+2) << 8;
                            DEBUG_OP_OPERAND("JMP", woper);
                            IP = woper;
                            nextIP = 0;
                            break;
                        case OPCODE_00_JMP_IND:
                            // Sets IP to the address stored in memory pointed
                            // to by operand
                            woper = E65_MemoryReadByte(IP+1);
                            woper |= E65_MemoryReadByte(IP+2) << 8;
                            DEBUG_OP_OPERAND("JMP ()", woper);
                            IP = E65_MemoryReadByte(woper);
                            IP |= E65_MemoryReadByte(woper + 1);
                            nextIP = 0;
                            break;
                        case OPCODE_00_STY:
                            // Stores the value of the Y register
                            E65_MemoryWriteByte(woper, Y);
                            DEBUG_OP("STY");
                            break;
                        case OPCODE_00_LDY:
                            // Loads a value into the Y register
                            Y = E65_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (Y & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= Y == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP("LDY");
                            break;
                        case OPCODE_00_CPY:
                            // Compares the Y register with the operand
                            woper = Y - E65_MemoryReadByte(woper);
                            Flags &= ~(FLAG_CARRY | FLAG_ZERO | FLAG_NEGATIVE);
                            Flags |= (woper & 0xff) == 0 ? FLAG_ZERO : 0;
                            Flags |= (woper & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= (woper >> 8) != 0 ? FLAG_OVERFLOW : 0;
                            DEBUG_OP_OPERAND("CPY", woper & 0xff);
                            break;
                        case OPCODE_00_CPX:
                            // Compares the X register with the operand
                            woper = X - E65_MemoryReadByte(woper);
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
                    // Decode the address of the operand
                    switch (address) {
                        case ADDRESS_01_IND_ZERO_X:
                            // Gets the address from the zero page, index by
                            // the next byte
                            oper = E65_MemoryReadByte(IP+1);
                            woper = E65_MemoryReadByte((oper+X) & 0xff);
                            woper = E65_MemoryReadByte((oper+X+1) & 0xff);
                            nextIP = 2;
                            break;
                        case ADDRESS_01_ZERO:
                            // Gets the address into zero page indexed by the
                            // next byte
                            woper = E65_MemoryReadByte(IP+1);
                            nextIP = 2;
                            break;
                        case ADDRESS_01_IMM:
                            // Sets address as the next byte
                            woper = IP+1;
                            nextIP = 2;
                            break;
                        case ADDRESS_01_ABS:
                            // Gets the address of the next byte
                            woper = E65_MemoryReadByte(IP+1);
                            woper |= E65_MemoryReadByte(IP+2) << 8;
                            nextIP = 3;
                            break;
                        case ADDRESS_01_IND_ZERO_Y:
                            // Gets the indexed address from the zero paged
                            // index by the next byte
                            oper = E65_MemoryReadByte(IP+1);
                            woper = E65_MemoryReadByte(oper);
                            woper |= E65_MemoryReadByte((oper+1) & 0xff);
                            woper += Y;
                            nextIP = 2;
                            break;
                        case ADDRESS_01_ABS_Y:
                            // Gets the indexed address from the next bytes
                            woper = E65_MemoryReadByte(IP+1);
                            woper |= E65_MemoryReadByte(IP+2) << 8;
                            woper += Y;
                            nextIP = 3;
                            break;
                        case ADDRESS_01_ABS_X:
                            // Gets the indexed address from the next bytes
                            woper = E65_MemoryReadByte(IP+1);
                            woper |= E65_MemoryReadByte(IP+2) << 8;
                            woper += X;
                            nextIP = 3;
                            break;
                        default: goto unknown_address;
                    }
                    switch (opcode) {
                        case OPCODE_01_ORA:
                            // ORs the A register with the operand
                            A |= E65_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= A == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP_OPERAND("ORA", E65_MemoryReadByte(woper));
                            break;
                        case OPCODE_01_AND:
                            // ANDs the A register with the operand
                            A &= E65_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= A == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP_OPERAND("ANDA", E65_MemoryReadByte(woper));
                            break;
                        case OPCODE_01_EOR:
                            // XORs the A register with the operand
                            A ^= E65_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= A == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP_OPERAND("EOR", E65_MemoryReadByte(woper));
                            break;
                        case OPCODE_01_ADC:
                            // Adds the A register with the operand and carry
                            woper = E65_MemoryReadByte(woper);
                            if (Flags & FLAG_DECIMAL) {
                                woper = FromDecimal(woper);
                                A = FromDecimal(A);
                            }
                            woper = A + woper + (Flags & FLAG_CARRY);
                            Flags &= ~FLAG_OVERFLOW;
                            {
                                // Everything in this scope calculates the
                                // overflow flag
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
                            // Stores the value of the A register
                            E65_MemoryWriteByte(woper, A);
                            DEBUG_OP_OPERAND("STA", woper);
                            break;
                        case OPCODE_01_LDA:
                            // Loads a value into the A register
                            A = E65_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= A == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP_OPERAND("LDA", woper);
                            break;
                        case OPCODE_01_CMP:
                            // Compares the A register with the operand
                            oper = E65_MemoryReadByte(woper);
                            woper = A - oper;
                            Flags &= ~(FLAG_CARRY | FLAG_ZERO | FLAG_NEGATIVE);
                            Flags |= (woper & 0xff) == 0 ? FLAG_ZERO : 0;
                            Flags |= (woper & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= (woper >> 8) != 0 ? FLAG_OVERFLOW : 0;
                            DEBUG_OP_OPERAND("CMP", oper);
                            break;
                        case OPCODE_01_SBC:
                            // Subtracts the A register with the operand and
                            // carry
                            oper = E65_MemoryReadByte(woper);
                            if (Flags & FLAG_DECIMAL) {
                                oper = FromDecimal(oper);
                                A = FromDecimal(A);
                            }
                            oper = ~oper;
                            {
                                // Everything in this scope calculates the
                                // overflow flag
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
                    // Decode the address of the operand
                    switch (address) {
                        case ADDRESS_10_IMM:
                            // Gets the address of the next byte
                            woper = IP+1;
                            nextIP = 2;
                            break;
                        case ADDRESS_10_ZERO:
                            // Gets the address of a byte in the zero page
                            woper = E65_MemoryReadByte(IP+1);
                            nextIP = 2;
                            break;
                        case ADDRESS_10_ACCU:
                            // This is to point to the A register
                            nextIP = 1;
                            break;
                        case ADDRESS_10_ABS:
                            // Gets the address pointed to by the next bytes
                            woper = E65_MemoryReadByte(IP+1);
                            woper |= E65_MemoryReadByte(IP+2) << 8;
                            nextIP = 3;
                            break;
                        case ADDRESS_10_ZERO_X:
                            // Gets the indexed address of a byte in the zero
                            // page
                            oper = E65_MemoryReadByte(IP+1);
                            woper = (oper + X) & 0xff;
                            nextIP = 2;
                            break;
                        case ADDRESS_10_ABS_X:
                            // Gets the indexed address pointed to by the next
                            // bytes
                            woper = E65_MemoryReadByte(IP+1);
                            woper |= E65_MemoryReadByte(IP+2);
                            woper += X;
                            nextIP = 3;
                            break;
                        default: goto unknown_address;
                    }
                    switch (opcode) {
                        case OPCODE_10_ASL:
                            // The A register is shifted left and the MSB is
                            // shifted into carry
                            Flags &= ~FLAG_CARRY;
                            if (address == ADDRESS_10_ACCU) {
                                woper = A << 1;
                                Flags |= (woper >> 8) & FLAG_CARRY;
                                A = woper;
                                Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                                Flags |= (A & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                                Flags |= A == 0 ? FLAG_ZERO : 0;
                            } else {
                                E65_Word temp = E65_MemoryReadByte(woper);
                                temp = temp << 1;
                                Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                                Flags |= (temp & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                                Flags |= temp == 0 ? FLAG_ZERO : 0;
                                Flags |= (temp >> 8) & FLAG_CARRY;
                                E65_MemoryWriteByte(woper, temp);
                            }
                            DEBUG_OP("ASL");
                            break;
                        case OPCODE_10_ROL: {
                            // The A register is rolled left with the carry bit
                            // acts as an 9th bit
                            E65_Byte old_flags = Flags;
                            E65_Word temp;
                            Flags &= ~FLAG_CARRY;
                            if (address == ADDRESS_10_ACCU) {
                                temp = A << 1;
                                temp |= old_flags & FLAG_CARRY;
                                Flags |= (temp >> 8) & FLAG_CARRY;
                                A = temp;
                            } else {
                                temp = E65_MemoryReadByte(woper) << 1;
                                temp |= old_flags & FLAG_CARRY;
                                Flags |= (temp >> 8) & FLAG_CARRY;
                                E65_MemoryWriteByte(woper, temp);                            
                            }
                            DEBUG_OP("ROL");
                            break; }
                        case OPCODE_10_LSR:
                            // The A register is shifted right and the LSB is
                            // shifted into carry
                            Flags &= ~FLAG_CARRY;
                            if (address == ADDRESS_10_ACCU) {
                                Flags |= A & FLAG_CARRY;
                                A >>= 1;
                                Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                                Flags |= A == 0 ? FLAG_ZERO : 0;
                            } else {
                                oper = E65_MemoryReadByte(woper);
                                Flags |= oper & FLAG_CARRY;
                                oper >>= 1;
                                Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                                Flags |= oper == 0 ? FLAG_ZERO : 0;
                                E65_MemoryWriteByte(woper, oper);
                            }
                            DEBUG_OP("LSR");
                            break;
                        case OPCODE_10_ROR: {
                            // The A register is rolled right with the carry bit
                            // acts as an 0th bit
                            E65_Byte old_flags = Flags;
                            E65_Word temp;
                            Flags &= ~FLAG_CARRY;
                            if (address == ADDRESS_10_ACCU) {
                                Flags |= A & FLAG_CARRY;
                                temp = A >> 1;
                                temp |= (old_flags & FLAG_CARRY) << 7;
                                A = temp;
                            } else {
                                temp = E65_MemoryReadByte(woper);
                                Flags |= temp & FLAG_CARRY;
                                temp = temp >> 1;
                                temp |= (old_flags & FLAG_CARRY) << 7;
                                E65_MemoryWriteByte(woper, temp);                            
                            }
                            DEBUG_OP("ROR");
                            break; }
                        case OPCODE_10_STX:
                            // Stores the value of the X register
                            E65_MemoryWriteByte(woper, X);
                            DEBUG_OP_OPERAND("STX", woper);
                            break;
                        case OPCODE_10_LDX:
                            // Loads a value into the X register
                            X = E65_MemoryReadByte(woper);
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (X & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= X == 0 ? FLAG_ZERO : 0;
                            DEBUG_OP_OPERAND("LDX", woper);
                            break;
                        case OPCODE_10_DEC:
                            // Decrements a value in memory
                            oper = E65_MemoryReadByte(woper);
                            oper--;
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (oper & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= oper == 0 ? FLAG_ZERO : 0;
                            E65_MemoryWriteByte(woper, oper);
                            DEBUG_OP_OPERAND("DEC", woper);
                            break;
                        case OPCODE_10_INC:
                            // Increments a value in memory
                            oper = E65_MemoryReadByte(woper);
                            oper++;
                            Flags &= ~(FLAG_NEGATIVE | FLAG_ZERO);
                            Flags |= (oper & (1<<7)) != 0 ? FLAG_NEGATIVE : 0;
                            Flags |= oper == 0 ? FLAG_ZERO : 0;
                            E65_MemoryWriteByte(woper, oper);
                            DEBUG_OP_OPERAND("INC", woper);
                            break;
                        default: goto unknown_opcode;
                    }
                break;
                default: goto unknown_opcode;
            }
    }
    // Precedes to the next instruction
    IP += nextIP;

    return;

unknown_opcode:;
    E65_Error("Unknown opcode: 0x%x", mop);
    return;

unknown_address:;
    E65_Error("Unknown addressing mode for opcode: 0x%x", mop);
    return;
}