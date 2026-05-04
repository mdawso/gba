#include "CPU_ARM7TDMI.hpp"
#include "Bus.hpp"
#include "Types.hpp"
#include <format>

CPU_ARM7TDMI::CPU_ARM7TDMI(IBus* bus)
{
    _bus = bus;
}

Word* CPU_ARM7TDMI::Reg(int index) {
    if (index < 8 || index == 15) return &_regs[index];

    ProcessorMode mode = static_cast<ProcessorMode>(CPSR()->GetMode());

    switch (mode) {
        case ProcessorMode::FIQ:
            return (index >= 8 && index <= 14) ? &_regs_fiq[index] : &_regs[index];
        case ProcessorMode::SVC:
            return (index == 13 || index == 14) ? &_regs_svc[index] : &_regs[index];
        case ProcessorMode::ABT:
            return (index == 13 || index == 14) ? &_regs_abt[index] : &_regs[index];
        case ProcessorMode::IRQ:
            return (index == 13 || index == 14) ? &_regs_irq[index] : &_regs[index];
        case ProcessorMode::UND:
            return (index == 13 || index == 14) ? &_regs_und[index] : &_regs[index];
        default:
            return &_regs[index]; // USER and SYSTEM mode share the standard registers
    }
}

CPU_ARM7TDMI::StatusReg* CPU_ARM7TDMI::CPSR()
{
    return &_cpsr;
}

CPU_ARM7TDMI::StatusReg* CPU_ARM7TDMI::SPSR()
{
    switch (static_cast<ProcessorMode>(CPSR()->GetMode()))
    {
        case ProcessorMode::FIQ: return &_spsr_fiq;
        case ProcessorMode::SVC: return &_spsr_svc;
        case ProcessorMode::ABT: return &_spsr_abt;
        case ProcessorMode::IRQ: return &_spsr_irq;
        case ProcessorMode::UND: return &_spsr_und;
        default: return &_cpsr; // USER and SYSTEM have no SPSR
    }
}

Word CPU_ARM7TDMI::FetchWord() 
{
    Word pc = *Reg(15);
    Word opcode = _bus->ReadWord(pc);
    *Reg(15) += 4; // Increment PC by 4 for ARM instructions
    return opcode;
}

HalfWord CPU_ARM7TDMI::FetchHalfword() 
{
    Word pc = *Reg(15);
    HalfWord opcode = _bus->ReadHalfword(pc);
    *Reg(15) += 2; // Increment PC by 2 for THUMB instructions
    return opcode;
}

bool CPU_ARM7TDMI::CheckCondition(Byte cond)
{
    bool n = CPSR()->GetN();
    bool z = CPSR()->GetZ();
    bool c = CPSR()->GetC();
    bool v = CPSR()->GetV();

    // Evaluate the cond field (bits 28-31)
    switch (cond) {
        case 0x0: return z;               // EQ:    Equal
        case 0x1: return !z;              // NE:    Not equal
        case 0x2: return c;               // CS/HS: Carry set / Unsigned higher or same
        case 0x3: return !c;              // CC/LO: Carry clear / Unsigned lower
        case 0x4: return n;               // MI:    Minus / Negative
        case 0x5: return !n;              // PL:    Plus / Positive or zero
        case 0x6: return v;               // VS:    Overflow set
        case 0x7: return !v;              // VC:    Overflow clear
        case 0x8: return c && !z;         // HI:    Unsigned higher
        case 0x9: return !c || z;         // LS:    Unsigned lower or same
        case 0xA: return n == v;          // GE:    Signed greater than or equal
        case 0xB: return n != v;          // LT:    Signed less than
        case 0xC: return !z && (n == v);  // GT:    Signed greater than
        case 0xD: return z || (n != v);   // LE:    Signed less than or equal
        case 0xE: return true;            // AL:    Always execute
        default: return false;            // NV:    Never execute (Reserved)
    }
}

std::string CPU_ARM7TDMI::DEBUG_GetDebugString() {
    bool t = CPSR()->GetT();
    Word pcVal = *Reg(15) -4;
    
    std::string opcodeHex;
    if (t) {
        HalfWord opcode = _bus->ReadHalfword(pcVal);
        opcodeHex = std::format("{:#06x}", opcode);
    } else {
        Word opcode = _bus->ReadWord(pcVal);
        opcodeHex = std::format("{:#010x}", opcode);
    }

    std::string modeStr;
    switch (CPSR()->GetMode()) {
        case 0x10: modeStr = "USR"; break;
        case 0x11: modeStr = "FIQ"; break;
        case 0x12: modeStr = "IRQ"; break;
        case 0x13: modeStr = "SVC"; break;
        case 0x17: modeStr = "ABT"; break;
        case 0x1B: modeStr = "UND"; break;
        case 0x1F: modeStr = "SYS"; break;
        default:   modeStr = "UNK"; break;
    }

    std::string flags = std::format("{}{}{}{}",
        CPSR()->GetN() ? 'N' : '-',
        CPSR()->GetZ() ? 'Z' : '-',
        CPSR()->GetC() ? 'C' : '-',
        CPSR()->GetV() ? 'V' : '-'
    );
    
    return std::format(
        "PC: {:#010x} | Instr: {:<10} | Mode: {:<3} | State: {:<5} | Flags: [{}]\n"
        "R0:  {:>10} | R1:  {:>10} | R2:  {:>10} | R3:  {:>10}\n"
        "R4:  {:>10} | R5:  {:>10} | R6:  {:>10} | R7:  {:>10}\n"
        "R8:  {:>10} | R9:  {:>10} | R10: {:>10} | R11: {:>10}\n"
        "R12: {:>10} | SP:  {:>10} | LR:  {:>10} | CPSR: {:#010x}",
        pcVal, opcodeHex, modeStr, (t ? "THUMB" : "ARM"), flags,
        *Reg(0),  *Reg(1),  *Reg(2),  *Reg(3),
        *Reg(4),  *Reg(5),  *Reg(6),  *Reg(7),
        *Reg(8),  *Reg(9),  *Reg(10), *Reg(11),
        *Reg(12), *Reg(13), *Reg(14), CPSR()->value
    );
}

static bool _SignBitPresent(Word word) { return (word >> 31) & 0x1; }
static bool _SignBitPresent(DoubleWord dword) { return (dword >> 63) & 0x1;}

static bool _UndefinedInst(Byte op) { return (op & 0x0FFFFFF) == 0x0FFFFF; }

// Checkers
static Byte _GetCond(Word opcode) { return (opcode >> 28) & 0xF; }
// bits 27-25 provide the primary classification of the instruction
static Byte _GetType(Word opcode) { return (opcode >> 25) & 0x7; }

// [Cond:4][00][I:1][OpCode:4][S:1][Rn:4][Rd:4][Operand2:12]
void CPU_ARM7TDMI::ExecuteARM(Word opcode) 
{
    Byte cond = _GetCond(opcode);
    
    if (!CheckCondition(cond)) {
        return;
    }
    
    if (_UndefinedInst(opcode)) {
        return;
    }

    Byte type = _GetType(opcode);

    // type 0b000 signifies MULs, Extra LDAs + STAs, ALU ops
    if (type == 0b000) {
        if ((opcode & 0x00000090) == 0x00000090) { // bits 4 and 7
            if ((opcode & 0x0F800000) == 0x00000000) { // 32 bit MULs
                // Multiply (MUL) and Multiply-Accumulate (MLA)
                bool a = (opcode >> 21) & 0x1; // accumulate?
                bool s = (opcode >> 20) & 0x1; // update flags?
                Byte rd = (opcode >> 16) & 0xF;
                Byte rn = (opcode >> 12) & 0xF;
                Byte rs = (opcode >> 8) & 0xF;
                Byte rm = opcode & 0xF;

                Word res = *Reg(rm) * *Reg(rs);
                if (a) {
                    res += *Reg(rn); // if accumulate bit (MLA), add rn to the result
                }
                *Reg(rd) = res;

                if (s) { // if update flags bit, update flags
                    CPSR()->SetN(_SignBitPresent(res));
                    CPSR()->SetZ(res == 0);
                }
                return;
            
            } else if ((opcode & 0x0F800000) == 0x00800000) { // 64 bit MULs
                // Multiply Long (SMULL, UMULL, SMLAL, UMLAL)
                bool u = (opcode >> 22) & 0x1; // unsigned?
                bool a = (opcode >> 21) & 0x1; // accumulate?
                bool s = (opcode >> 20) & 0x1; // update flags?
                Byte rdhi = (opcode >> 16) & 0xF;
                Byte rdlo = (opcode >> 12) & 0xF;
                Byte rs = (opcode >> 8) & 0xF;
                Byte rm = opcode & 0xF;

                DoubleWord res = 0;
                if (u) { // if unsigned sign-extend before multiplying
                    SignedDoubleWord lhs = static_cast<SignedWord>(*Reg(rm));
                    SignedDoubleWord rhs = static_cast<SignedWord>(*Reg(rs));
                    res = static_cast<DoubleWord>(lhs * rhs);
                } else {
                    DoubleWord lhs = *Reg(rm);
                    DoubleWord rhs = *Reg(rs);
                    res = lhs * rhs;
                }

                if (a) { // if accumulate add existing 64 bit val from rdhi, rdlo
                    DoubleWord acc = (static_cast<DoubleWord>(*Reg(rdhi)) << 32) | *Reg(rdlo);
                    res += acc;
                }

                *Reg(rdhi) = (res >> 32) & 0xFFFFFFFF;
                *Reg(rdlo) = res & 0xFFFFFFFF;

                if (s) { // set flags
                    CPSR()->SetN(_SignBitPresent(res));
                    CPSR()->SetZ(res == 0);
                }
                return;
            
            } else if ((opcode & 0x0FB00000) == 0x01000000) {
                // Single Data Swap (SWP, SWPB)
                bool b = (opcode >> 22) & 0x1; // byte or word swap?
                Byte rn = (opcode >> 16) & 0xF;
                Byte rd = (opcode >> 12) & 0xF;
                Byte rm = opcode & 0xF;

                Word addr = *Reg(rn);
                if (b) { // byte
                    Word temp = _bus->ReadByte(addr);
                    _bus->WriteByte(addr, *Reg(rm) & 0xFF);
                    *Reg(rd) = temp;
                } else { // word
                    Word temp = _bus->ReadWord(addr);
                    _bus->WriteWord(addr, *Reg(rm));
                    *Reg(rd) = temp;
                }
                return;
            
            } else if ((opcode & 0x0E400000) == 0x00000000) {
                // Halfword, Signed Byte, and Signed Halfword Data Transfer (LDRH, STRH, LDRSB, LDRSH)
                bool p = (opcode >> 24) & 0x1; // pre or post index?
                bool u = (opcode >> 23) & 0x1; // up or down?
                bool i = (opcode >> 22) & 0x1; // immediate or reg offset?
                bool w = (opcode >> 21) & 0x1; // writeback?
                bool l = (opcode >> 20) & 0x1; // load or store?
                Byte rn = (opcode >> 16) & 0xF;
                Byte rd = (opcode >> 12) & 0xF;
                Byte s = (opcode >> 6) & 0x1; // signed?
                Byte h = (opcode >> 5) & 0x1; // halfword?

                Word offset = 0;
                if (i) { // imm
                    offset = ((opcode >> 4) & 0xF0) | (opcode & 0xF);
                } else { // rel
                    offset = *Reg(opcode & 0xF);
                }

                Word addr = *Reg(rn);
                Word effectiveAddr = addr;

                if (p) { // if pre-indexing adjust address before access
                    effectiveAddr = u ? (addr + offset) : (addr - offset);
                }

                if (l) { // load
                    if (s && h) {
                        HalfWord val = _bus->ReadHalfword(effectiveAddr);
                        *Reg(rd) = (val & 0x8000) ? (val | 0xFFFF0000) : val;
                    } else if (s && !h) {
                        Byte val = _bus->ReadByte(effectiveAddr);
                        *Reg(rd) = (val & 0x80) ? (val | 0xFFFFFF00) : val;
                    } else {
                        *Reg(rd) = _bus->ReadHalfword(effectiveAddr);
                    }
                } else { // store
                    if (h) {
                        _bus->WriteHalfword(effectiveAddr, *Reg(rd) & 0xFFFF);
                    }
                }

                if (!p || w) { // if post-indexing or writeback update base register
                    *Reg(rn) = u ? (addr + offset) : (addr - offset);
                }
                return;
            }
        } else if ((opcode & 0x0FBF0FFF) == 0x010F0000) {
            // Move PSR to Register (MRS)
            bool psr = (opcode >> 22) & 0x1; // cpsr or spsr?
            Byte rd = (opcode >> 12) & 0xF;
            *Reg(rd) = psr ? SPSR()->value : CPSR()->value;
            return;
        
        } else if ((opcode & 0x0DB0F000) == 0x0120F000) {
            // MSR - move register/immediate to psr
            bool psr = (opcode >> 22) & 0x1; // cpsr or spsr?
            bool f = (opcode >> 19) & 0x1; // control field mask
            bool c = (opcode >> 16) & 0x1; // flags field mask
            Word mask = 0;
            
            if (f) mask |= 0xFF000000;
            if (c) mask |= 0x000000FF;

            Word op = 0;
            if (opcode & (1 << 25)) {
                bool carry = false;
                op = GetOperand2(opcode, true, carry);
            } else {
                op = *Reg(opcode & 0xF);
            }

            if (psr) {
                SPSR()->value = (SPSR()->value & ~mask) | (op & mask);
            } else {
                CPSR()->value = (CPSR()->value & ~mask) | (op & mask);
            }
            return;
        }
    }

    // ALU operations
    if (type == 0b000 || type == 0b001) {
        bool i = (opcode >> 25) & 0x1; // immediate?
        Byte opCode = (opcode >> 21) & 0xF;
        bool s = (opcode >> 20) & 0x1; // set condition codes flag?
        Byte rn = (opcode >> 16) & 0xF;
        Byte rd = (opcode >> 12) & 0xF;
        
        bool carryOut = CPSR()->GetC();
        Word operand2 = GetOperand2(opcode, i, carryOut); // Op2 can be a shifted register or a barrel shifted immediate
        Word op1 = (rn == 15) ? (*Reg(rn) + 4) : *Reg(rn);
        Word result = 0;

        // Perform the requested ALU instruction
        switch (opCode) {
            case 0x0: result = op1 & operand2;                      break; // AND: Logical AND
            case 0x1: result = op1 ^ operand2;                      break; // EOR: Logical Exclusive OR
            case 0x2: result = op1 - operand2;                      break; // SUB: Subtract
            case 0x3: result = operand2 - op1;                      break; // RSB: Reverse Subtract
            case 0x4: result = op1 + operand2;                      break; // ADD: Add
            case 0x5: result = op1 + operand2 + CPSR()->GetC();     break; // ADC: Add with Carry
            case 0x6: result = op1 - operand2 + CPSR()->GetC() - 1; break; // SBC: Subtract with Carry
            case 0x7: result = operand2 - op1 + CPSR()->GetC() - 1; break; // RSC: Reverse Subtract with Carry
            case 0x8: result = op1 & operand2;                      break; // TST: Test (AND, but do not write result)
            case 0x9: result = op1 ^ operand2;                      break; // TEQ: Test Equivalence (EOR, do not write)
            case 0xA: result = op1 - operand2;                      break; // CMP: Compare (SUB, do not write)
            case 0xB: result = op1 + operand2;                      break; // CMN: Compare Negated (ADD, do not write)
            case 0xC: result = op1 | operand2;                      break; // ORR: Logical OR
            case 0xD: result = operand2;                            break; // MOV: Move
            case 0xE: result = op1 & ~operand2;                     break; // BIC: Bit Clear (AND NOT)
            case 0xF: result = ~operand2;                           break; // MVN: Move Not (Bitwise inverted)
        }

        // Do not write to Rd if it is a test/compare instruction
        if (opCode != 0x8 && opCode != 0x9 && opCode != 0xA && opCode != 0xB) {
            *Reg(rd) = result;
        }

        if (s) { // flag updates
            
            if (rd == 15) { // MOVS PC, LR restores cpsr from spsr
                
                if (CPSR()->GetMode() != 0x10 && CPSR()->GetMode() != 0x1F) {
                    CPSR()->value = SPSR()->value;
                }
            
            } else {
                
                CPSR()->SetN(_SignBitPresent(result));
                CPSR()->SetZ(result == 0);
                
                if (opCode != 0x2 && opCode != 0x3 && opCode != 0x4 && opCode != 0x5 
                    && opCode != 0x6 && opCode != 0x7 && opCode != 0xA && opCode != 0xB) {
                    // Logical ops update carry from the shifter
                    CPSR()->SetC(carryOut);
                
                } else { // arithmetic ops update carry and overflow based on result
                   
                    if (opCode == 0x4 || opCode == 0x5 || opCode == 0xB) { // ADD, ADC, CMN (addition)
                        
                        DoubleWord r64 = 
                            static_cast<DoubleWord>(op1) + operand2 + (opCode == 0x5 ? CPSR()->GetC() : 0);
                        
                        CPSR()->SetC(r64 > 0xFFFFFFFF);
                        CPSR()->SetV((~(op1 ^ operand2) & (op1 ^ result)) >> 31);
                    
                    } else if (opCode == 0x2 || opCode == 0x6 || opCode == 0xA) { // SUB, SBC, CMP (subtraction)
                        
                        DoubleWord r64 = 
                            static_cast<DoubleWord>(op1) - operand2 - (opCode == 0x6 ? !CPSR()->GetC() : 0); // op1 - op2 - 1 if not carry
                                                                                                             // since carry is "not borrow" if there is a carry then it means there is no borrow
                        CPSR()->SetC(!(r64 > 0xFFFFFFFF));                                                   // if there is not a carry there must be a borrow so we subtract 1.
                        CPSR()->SetV(((op1 ^ operand2) & (op1 ^ result)) >> 31);
                    
                    } else if (opCode == 0x3 || opCode == 0x7) { // RSB, RSC (reverse subtraction)
                        
                        DoubleWord r64 = 
                            static_cast<DoubleWord>(operand2) - op1 - (opCode == 0x7 ? !CPSR()->GetC() : 0); // op2 - op1 - 1 if not carry
                        
                        CPSR()->SetC(!(r64 > 0xFFFFFFFF));
                        CPSR()->SetV(((operand2 ^ op1) & (operand2 ^ result)) >> 31);
                    }
                }
            }
        }
    } else if (type == 0b010 || type == 0b011) {
        // Single Data Transfer (LDR, STR)
        bool i = (opcode >> 25) & 0x1; // immediate or reg offset?
        bool p = (opcode >> 24) & 0x1; // pre or post index?
        bool u = (opcode >> 23) & 0x1; // up or down?
        bool b = (opcode >> 22) & 0x1; // byte or word?
        bool w = (opcode >> 21) & 0x1; // writeback?
        bool l = (opcode >> 20) & 0x1; // load or store?
        Byte rn = (opcode >> 16) & 0xF;
        Byte rd = (opcode >> 12) & 0xF;

        Word offset = 0;
        if (!i) {
            offset = opcode & 0xFFF;
        } else {
            bool dummyCarry;
            offset = GetOperand2(opcode, false, dummyCarry);
        }

        Word addr = (rn == 15) ? (*Reg(rn) + 4) : *Reg(rn);
        Word effectiveAddr = addr;

        if (p) {
            effectiveAddr = u ? (addr + offset) : (addr - offset);
        }

        if (l) { // LDA
            if (b) {
                *Reg(rd) = _bus->ReadByte(effectiveAddr);
            } else {
                // unaligned word reads result in a rotated value
                Word val = _bus->ReadWord(effectiveAddr); 
                Word rot = (effectiveAddr & 3) * 8;
                if (rot > 0) {
                    val = (val >> rot) | (val << (32 - rot));
                }
                *Reg(rd) = val;
            }
        } else { // STR
            
            Word val = (rd == 15) ? (*Reg(rd) + 8) : *Reg(rd);
            
            if (b) {
                _bus->WriteByte(effectiveAddr, val & 0xFF);
            } else {
                _bus->WriteWord(effectiveAddr, val);
            }
        }

        if (!p || w) {
            *Reg(rn) = u ? (addr + offset) : (addr - offset);
        }

    } else if (type == 0b100) {
        // Block Data Transfer (LDM, STM)
        // Load or Store multiple registers at once.
        bool p = (opcode >> 24) & 0x1; // pre or post?
        bool u = (opcode >> 23) & 0x1; // up or down?
        bool s = (opcode >> 22) & 0x1; // user mode?
        bool w = (opcode >> 21) & 0x1; // writeback?
        bool l = (opcode >> 20) & 0x1; // load or store?
        Byte rn = (opcode >> 16) & 0xF;
        HalfWord regList = opcode & 0xFFFF; // bitmask of registers to transfer

        Word addr = *Reg(rn);
        int numRegs = 0;
        for (int j = 0; j < 16; ++j) {
            if ((regList >> j) & 1) numRegs++;
        }

        // calculate starting address based on direction and indexing
        Word startAddr = addr;
        if (!u) {
            startAddr = addr - (numRegs * 4);
            if (!p) startAddr += 4;
        } else {
            if (p) startAddr += 4;
        }

        // perform the transfer
        Word currentAddr = startAddr;
        for (int j = 0; j < 16; ++j) {
            if ((regList >> j) & 1) {
                if (l) {
                    *Reg(j) = _bus->ReadWord(currentAddr);
                } else {
                    Word val = (j == 15) ? (*Reg(j) + 8) : *Reg(j);
                    _bus->WriteWord(currentAddr, val);
                }
                currentAddr += 4;
            }
        }

        if (w) {
            *Reg(rn) = u ? (addr + (numRegs * 4)) : (addr - (numRegs * 4));
        }

        if (s && l && ((regList >> 15) & 1)) { // restore spsr to cpsr if PC is in the load list and s bit is set
            if (CPSR()->GetMode() != 0x10 && CPSR()->GetMode() != 0x1F) {
                CPSR()->value = SPSR()->value;
            }
        }

    } else if (type == 0b101) {
        // Branch (B) and Branch with Link (BL)
        bool l = (opcode >> 24) & 0x1; // save return address?
        Word offset = opcode & 0xFFFFFF;
        
        // sign extend 24 bit offset to 32 bit
        if (offset & 0x800000) {
            offset |= 0xFF000000;
        }
        offset <<= 2; // word align

        if (l) {
            *Reg(14) = *Reg(15) - 4; // save return address into LR
        }
        
        *Reg(15) += offset;
    } else if (type == 0b111) {
        // Software Interrupt (SWI)
        if ((opcode & 0x0F000000) == 0x0F000000) {
            // backup flags, switch to supervisor mode, disable IRQs, save pc to lr, jump to swi vector
            CPSR()->SetMode(static_cast<Word>(ProcessorMode::SVC));
            *SPSR() = *CPSR();
            CPSR()->SetI(true);
            *Reg(14) = *Reg(15) - 4;
            *Reg(15) = 0x08;
        }
    }
}

void CPU_ARM7TDMI::ExecuteTHUMB(HalfWord opcode) 
{
    if ((opcode & 0xF800) == 0x1800) {
        // ADD, SUB
        Byte rm = (opcode >> 6) & 0x7;     // source 2 (achieved with sooooooourse)
        Byte rn = (opcode >> 3) & 0x7;     // source 1
        Byte rd = opcode & 0x7;            // destination
        bool s = (opcode >> 9) & 0x1;      // sub or add?
        bool i = (opcode >> 10) & 0x1;
        
        Word op1 = *Reg(rn);
        Word op2 = i ? rm : *Reg(rm);
        Word result = s ? (op1 - op2) : (op1 + op2);
        
        *Reg(rd) = result;
        
        CPSR()->SetN(_SignBitPresent(result));
        CPSR()->SetZ(result == 0);
        
        if (s) {
            DoubleWord r64 = static_cast<DoubleWord>(op1) - op2;
            CPSR()->SetC(!(r64 > 0xFFFFFFFF));
            CPSR()->SetV(((op1 ^ op2) & (op1 ^ result)) >> 31);
        } else {
            DoubleWord r64 = static_cast<DoubleWord>(op1) + op2;
            CPSR()->SetC(r64 > 0xFFFFFFFF);
            CPSR()->SetV((~(op1 ^ op2) & (op1 ^ result)) >> 31);
        }
    
    } else if ((opcode & 0xE000) == 0x0000) {
        // LSL, LSR, ASR (shifted reg movs)
        Byte type = (opcode >> 11) & 0x3;
        Byte amt = (opcode >> 6) & 0x1F;
        Byte rm = (opcode >> 3) & 0x7;
        Byte rd = opcode & 0x7;
        
        Word val = *Reg(rm);
        bool carryOut = CPSR()->GetC();
        
        switch (type) {
            case 0: // LSL - logical shift left
                if (amt != 0) {
                    carryOut = (val >> (32 - amt)) & 1;
                    val <<= amt;
                }
                break;
            case 1: // LSR - logical shift right
                if (amt == 0) amt = 32;
                carryOut = (val >> (amt - 1)) & 1;
                val >>= amt;
                break;
            case 2: // ASR - arithmetic shift right
                if (amt == 0) amt = 32;
                carryOut = (val >> (amt - 1)) & 1;
                val = static_cast<SignedWord>(val) >> amt;
                break;
        }
        
        *Reg(rd) = val;
        CPSR()->SetN((val >> 31) & 0x1);
        CPSR()->SetZ(val == 0);
        CPSR()->SetC(carryOut);
    
    } else if ((opcode & 0xE000) == 0x2000) {
        // MOV, CMP, ADD, SUB - immediate
        Byte op = (opcode >> 11) & 0x3;
        Byte rd = (opcode >> 8) & 0x7;
        Word imm = opcode & 0xFF;
        Word op1 = *Reg(rd);
        Word result = 0;
        
        switch (op) {
            case 0: // MOV
                result = imm;
                *Reg(rd) = result;
                break;
            case 1: // CMP
                result = op1 - imm;
                break;
            case 2: // ADD
                result = op1 + imm;
                *Reg(rd) = result;
                break;
            case 3: // SUB
                result = op1 - imm;
                *Reg(rd) = result;
                break;
        }
        
        CPSR()->SetN(_SignBitPresent(result));
        CPSR()->SetZ(result == 0);
        if (op == 1 || op == 3) {
            DoubleWord r64 = static_cast<DoubleWord>(op1) - imm;
            CPSR()->SetC(!(r64 > 0xFFFFFFFF));
            CPSR()->SetV(((op1 ^ imm) & (op1 ^ result)) >> 31);
        } else if (op == 2) {
            DoubleWord r64 = static_cast<DoubleWord>(op1) + imm;
            CPSR()->SetC(r64 > 0xFFFFFFFF);
            CPSR()->SetV((~(op1 ^ imm) & (op1 ^ result)) >> 31);
        }
    
    } else if ((opcode & 0xFC00) == 0x4000) {
        // ALU ops
        Byte op = (opcode >> 6) & 0xF;
        Byte rm = (opcode >> 3) & 0x7;
        Byte rd = opcode & 0x7;
        
        Word op1 = *Reg(rd);
        Word op2 = *Reg(rm);
        Word result = 0;
        bool write = true;
        bool carryOut = CPSR()->GetC();
        bool overflow = CPSR()->GetV();
        
        switch (op) {
            case 0x0: result = op1 & op2; break; // AND - and
            case 0x1: result = op1 ^ op2; break; // EOR - xor
            case 0x2: {                          // LSL - logical shift left
                DoubleWord r64 = static_cast<DoubleWord>(op1) << (op2 & 0xFF);
                result = static_cast<Word>(r64);
                if ((op2 & 0xFF) > 0) carryOut = (op1 >> (32 - (op2 & 0xFF))) & 1;
                break;
            }
            case 0x3: { // LSR - logical shift right
                Byte shift = op2 & 0xFF;
                if (shift > 0) {
                    carryOut = (op1 >> (shift - 1)) & 1;
                    result = shift >= 32 ? 0 : op1 >> shift;
                } else {
                    result = op1;
                }
                break;
            }
            case 0x4: { // ASR - arithmetic shift right
                Byte shift = op2 & 0xFF;
                if (shift > 0) {
                    carryOut = (op1 >> (shift - 1)) & 1;
                    result = shift >= 32 ? (op1 >> 31 ? 0xFFFFFFFF : 0) : static_cast<SignedWord>(op1) >> shift;
                } else {
                    result = op1;
                }
                break;
            }
            case 0x5: { // ADC - add with carry
                DoubleWord r64 = static_cast<DoubleWord>(op1) + op2 + CPSR()->GetC();
                result = static_cast<Word>(r64);
                carryOut = (r64 > 0xFFFFFFFF);
                overflow = (~(op1 ^ op2) & (op1 ^ result)) >> 31;
                break;
            }
            case 0x6: { // SBC - subtract with carry 
                DoubleWord r64 = static_cast<DoubleWord>(op1) - op2 - !CPSR()->GetC();
                result = static_cast<Word>(r64);
                carryOut = !(r64 > 0xFFFFFFFF);
                overflow = ((op1 ^ op2) & (op1 ^ result)) >> 31;
                break;
            }
            case 0x7: { // ROR - rotate right (not risk of rain)
                        // rotates are wrapping shifts eg: 0x1001 ROR 1 = 0x1100
                Byte shift = op2 & 0x1F;
                if (shift > 0) {
                    carryOut = (op1 >> (shift - 1)) & 1;
                    result = (op1 >> shift) | (op1 << (32 - shift));
                } else if ((op2 & 0xFF) == 0) {
                    result = op1;
                }
                break;
            }
            case 0x8: write = false; result = op1 & op2; break; // TST (do not write)
            case 0x9: write = false; result = 0 - op2;   break; // NEG (RSB from 0)
            case 0xA: write = false; result = op1 - op2; break; // CMP (do not write)
            case 0xB: write = false; result = op1 + op2; break; // CMN (do not write)
            case 0xC: result = op1 | op2;                break; // ORR
            case 0xD: result = op1 * op2;                break; // MUL
            case 0xE: result = op1 & ~op2;               break; // BIC
            case 0xF: result = ~op2;                     break; // MVN
        }
        
        if (write) *Reg(rd) = result;
        
        CPSR()->SetN(_SignBitPresent(result));
        CPSR()->SetZ(result == 0);
        CPSR()->SetC(carryOut);
        CPSR()->SetV(overflow);
        
        if (!write) {
            
            if (op == 0x9 || op == 0xA) {
                DoubleWord r64 = static_cast<DoubleWord>(op1) - op2;
                CPSR()->SetC(!(r64 > 0xFFFFFFFF));
                CPSR()->SetV(((op1 ^ op2) & (op1 ^ result)) >> 31);
            } else if (op == 0xB) {
                DoubleWord r64 = static_cast<DoubleWord>(op1) + op2;
                CPSR()->SetC(r64 > 0xFFFFFFFF);
                CPSR()->SetV((~(op1 ^ op2) & (op1 ^ result)) >> 31);
            }
        }
    
    } else if ((opcode & 0xFC00) == 0x4400) {
        // hi reg ops, branch exchange
        Byte op = (opcode >> 8) & 0x3;
        bool h1 = (opcode >> 7) & 0x1;
        bool h2 = (opcode >> 6) & 0x1;
        Byte rm = (opcode >> 3) & 0x7;
        Byte rd = opcode & 0x7;
        
        Byte dstReg = (h1 << 3) | rd;
        Byte srcReg = (h2 << 3) | rm;
        
        Word op1 = (dstReg == 15) ? (*Reg(dstReg) + 2) : *Reg(dstReg);
        Word op2 = (srcReg == 15) ? (*Reg(srcReg) + 2) : *Reg(srcReg);
        
        if (op == 0) {
            // ADD
            *Reg(dstReg) = op1 + op2;
            if (dstReg == 15) *Reg(15) &= ~1;
        } else if (op == 1) {
            // CMP
            Word result = op1 - op2;
            CPSR()->SetN(_SignBitPresent(result));
            CPSR()->SetZ(result == 0);
            DoubleWord r64 = static_cast<DoubleWord>(op1) - op2;
            CPSR()->SetC(!(r64 > 0xFFFFFFFF));
            CPSR()->SetV(((op1 ^ op2) & (op1 ^ result)) >> 31);
        } else if (op == 2) {
            // MOV
            *Reg(dstReg) = op2;
            if (dstReg == 15) *Reg(15) &= ~1;
        } else if (op == 3) {
            // BX - branch exchange
            if (op2 & 1) {
                *Reg(15) = op2 & ~1; // stay in THUMB
            } else {
                CPSR()->SetT(false); // switch to ARM
                *Reg(15) = op2 & ~3; // align to word boundary
            }
        }
    
    } else if ((opcode & 0xF800) == 0x4800) {
        // pc relative load
        Byte rd = (opcode >> 8) & 0x7;
        Word imm = (opcode & 0xFF) << 2;
        Word addr = (*Reg(15) & ~2) + imm;
        *Reg(rd) = _bus->ReadWord(addr);
    
    } else if ((opcode & 0xF000) == 0x5000) {
        // lda / sta with register offset
        bool l = (opcode >> 11) & 0x1;
        bool b = (opcode >> 10) & 0x1;
        Byte rm = (opcode >> 6) & 0x7;
        Byte rn = (opcode >> 3) & 0x7;
        Byte rd = opcode & 0x7;
        
        Word addr = *Reg(rn) + *Reg(rm);
        
        if (l) {
            *Reg(rd) = b ? _bus->ReadByte(addr) : _bus->ReadWord(addr);
        } else {
            if (b) _bus->WriteByte(addr, *Reg(rd) & 0xFF);
            else _bus->WriteWord(addr, *Reg(rd));
        }
    
    } else if ((opcode & 0xF000) == 0x9000) {
        // lda / sta with sp offset
        bool l = (opcode >> 11) & 0x1;
        Byte rd = (opcode >> 8) & 0x7;
        Word imm = (opcode & 0xFF) << 2;
        
        Word addr = *Reg(13) + imm;
        
        if (l) *Reg(rd) = _bus->ReadWord(addr);
        else _bus->WriteWord(addr, *Reg(rd));
    
    } else if ((opcode & 0xF000) == 0xA000) {
        // load address (PC/SP + offset)
        bool sp = (opcode >> 11) & 0x1;
        Byte rd = (opcode >> 8) & 0x7;
        Word imm = (opcode & 0xFF) << 2;
        
        if (sp) *Reg(rd) = *Reg(13) + imm;
        else *Reg(rd) = (*Reg(15) & ~2) + imm;
    
    } else if ((opcode & 0xFF00) == 0xB000) {
        // add offset to sp
        bool isSub = (opcode >> 7) & 0x1;
        Word imm = (opcode & 0x7F) << 2;
        
        if (isSub) *Reg(13) -= imm;
        else *Reg(13) += imm;
    
    } else if ((opcode & 0xF600) == 0xB400) {
        // push / pop registers
        bool l = (opcode >> 11) & 0x1;
        bool r = (opcode >> 8) & 0x1;
        Byte rlist = opcode & 0xFF;
        
        if (l) {
            Word addr = *Reg(13);
            for (int i = 0; i < 8; i++) {
                if ((rlist >> i) & 1) {
                    *Reg(i) = _bus->ReadWord(addr);
                    addr += 4;
                }
            }
            if (r) {
                Word pc = _bus->ReadWord(addr);
                addr += 4;
                if (pc & 1) {
                    *Reg(15) = pc & ~1;
                } else {
                    CPSR()->SetT(false);
                    *Reg(15) = pc & ~3;
                }
            }
            *Reg(13) = addr;
        
        } else {
            Word addr = *Reg(13);
            int count = 0;
            for (int i = 0; i < 8; i++) if ((rlist >> i) & 1) count++;
            if (r) count++;
            
            addr -= count * 4;
            *Reg(13) = addr;
            
            for (int i = 0; i < 8; i++) {
                if ((rlist >> i) & 1) {
                    _bus->WriteWord(addr, *Reg(i));
                    addr += 4;
                }
            }
            if (r) {
                _bus->WriteWord(addr, *Reg(14));
            }
        }
    
    } else if ((opcode & 0xF000) == 0xC000) {
        // multi lda / sta
        bool l = (opcode >> 11) & 0x1;
        Byte rn = (opcode >> 8) & 0x7;
        Byte rlist = opcode & 0xFF;
        
        Word addr = *Reg(rn);
        
        if (l) {
            for (int i = 0; i < 8; i++) {
                if ((rlist >> i) & 1) {
                    *Reg(i) = _bus->ReadWord(addr);
                    addr += 4;
                }
            }
        } else {
            for (int i = 0; i < 8; i++) {
                if ((rlist >> i) & 1) {
                    _bus->WriteWord(addr, *Reg(i));
                    addr += 4;
                }
            }
        }
        *Reg(rn) = addr;
    
    } else if ((opcode & 0xF000) == 0xD000) {
        // conditional branch
        Byte cond = (opcode >> 8) & 0xF;
        if (cond != 0xF) { // 0xF is reserved / used for SWI
            if (CheckCondition(cond)) {
                SignedByte offset = opcode & 0xFF;
                *Reg(15) += (offset << 1);
            }
        }
    
    } else if ((opcode & 0xF800) == 0xE000) {
        // unconditional branch
        SignedHalfWord offset = (opcode & 0x7FF) << 5;
        offset >>= 4;
        *Reg(15) += offset;
    
    } else if ((opcode & 0xF000) == 0xF000) {
        // long branch with LR
        bool high = (opcode >> 11) & 0x1;
        Word offset = opcode & 0x7FF;
        
        if (!high) {
            offset = (offset << 21) >> 9;
            *Reg(14) = *Reg(15) + offset;
        } else {
            Word pc = *Reg(15);
            *Reg(15) = (*Reg(14) + (offset << 1)) & ~1;
            *Reg(14) = (pc - 2) | 1;
        }
    }
}

Word CPU_ARM7TDMI::GetOperand2(Word opcode, bool immediate, bool& carryOut) {
    if (immediate) {
        Word imm = opcode & 0xFF;
        Byte rotate = ((opcode >> 8) & 0xF) * 2;
        
        if (rotate == 0) {
            return imm;
        }
        
        // immediate rotates always update carry-out if shift != 0
        carryOut = (imm >> (rotate - 1)) & 0x1;
        return (imm >> rotate) | (imm << (32 - rotate));
    }
    
    Byte rm = opcode & 0xF;
    Byte shiftType = (opcode >> 5) & 0x3;
    Byte shiftAmt = 0;
    
    if ((opcode >> 4) & 1) {
        Byte rs = (opcode >> 8) & 0xF;
        shiftAmt = *Reg(rs) & 0xFF;
    } else {
        shiftAmt = (opcode >> 7) & 0x1F;
    }

    Word val = *Reg(rm);

    switch (shiftType) {
        
        case 0: // LSL
            if (shiftAmt != 0) {
                if (shiftAmt >= 32) {
                    carryOut = (shiftAmt == 32) ? (val & 1) : 0;
                    val = 0;
                } else {
                    carryOut = (val >> (32 - shiftAmt)) & 1;
                    val <<= shiftAmt;
                }
            }
            break;
        
        case 1: // LSR
            if (shiftAmt == 0 && !((opcode >> 4) & 1)) shiftAmt = 32;
            if (shiftAmt >= 32) {
                carryOut = (shiftAmt == 32) ? ((val >> 31) & 1) : 0;
                val = 0;
            } else if (shiftAmt > 0) {
                carryOut = (val >> (shiftAmt - 1)) & 1;
                val >>= shiftAmt;
            }
            break;
        
        case 2: // ASR
            if (shiftAmt == 0 && !((opcode >> 4) & 1)) shiftAmt = 32;
            if (shiftAmt >= 32) {
                carryOut = (val >> 31) & 1;
                val = carryOut ? 0xFFFFFFFF : 0;
            } else if (shiftAmt > 0) {
                carryOut = (val >> (shiftAmt - 1)) & 1;
                val = static_cast<SignedWord>(val) >> shiftAmt;
            }
            break;
        
            case 3: // ROR / RRX
            if (shiftAmt == 0 && !((opcode >> 4) & 1)) {
                // RRX - rotate right extending through carry flag
                carryOut = val & 1;
                val = (CPSR()->GetC() << 31) | (val >> 1);
            } else {
                shiftAmt &= 0x1F;
                if (shiftAmt > 0) {
                    carryOut = (val >> (shiftAmt - 1)) & 1;
                    val = (val >> shiftAmt) | (val << (32 - shiftAmt));
                }
            }
            break;
    }
    
    return val;
}

void CPU_ARM7TDMI::Reset() 
{
    // Switch to Supervisor Mode (0x13), Disable IRQ (0x80) and FIQ (0x40), THUMB mode off
    CPSR()->SetMode(0x13 + 0x80 + 0x40); 
    CPSR()->SetT(false);
    
    // Jump to reset vector
    *Reg(15) = 0x00000000;
}

void CPU_ARM7TDMI::Clock() {
    if (CPSR()->GetT() == 0) {
        Word opcode = FetchWord();
        ExecuteARM(opcode);
    } else {
        HalfWord opcode = FetchHalfword();
        ExecuteTHUMB(opcode);
    }
}