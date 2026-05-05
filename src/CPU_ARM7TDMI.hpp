#pragma once

#include "Types.hpp"

class IBus;

#include <array>
#include <string>

// https://mgba-emu.github.io/gbatek

class CPU_ARM7TDMI {

private:

    IBus* _bus{};

    /*
    struct StatusReg {
        union {
            Word value;
            struct {
                uint32_t mode     : 5;  // Bits 0-4: Mode bits
                uint32_t T        : 1;  // Bit 5: State bit (ARM/THUMB)
                uint32_t F        : 1;  // Bit 6: FIQ disable
                uint32_t I        : 1;  // Bit 7: IRQ disable
                uint32_t reserved : 19; // Bits 8-26: Reserved
                uint32_t Q        : 1;  // Bit 27: Sticky Overflow
                uint32_t V        : 1;  // Bit 28: Overflow Flag
                uint32_t C        : 1;  // Bit 29: Carry Flag
                uint32_t Z        : 1;  // Bit 30: Zero Flag
                uint32_t N        : 1;  // Bit 31: Sign Flag
            };
        };
    };
    */

    struct StatusReg {
        Word value{};

        Word GetMode() const { return value & 0x1F; }
        void SetMode(Word mode) { value = (value & ~0x1F) | (mode & 0x1F); }

        bool GetT() const { return (value >> 5) & 1; }
        void SetT(bool t) { value = (value & ~(1 << 5)) | (t << 5); }

        bool GetF() const { return (value >> 6) & 1; }
        void SetF(bool f) { value = (value & ~(1 << 6)) | (f << 6); }

        bool GetI() const { return (value >> 7) & 1; }
        void SetI(bool i) { value = (value & ~(1 << 7)) | (i << 7); }

        bool GetQ() const { return (value >> 27) & 1; }
        void SetQ(bool q) { value = (value & ~(1 << 27)) | (q << 27); }

        bool GetV() const { return (value >> 28) & 1; }
        void SetV(bool v) { value = (value & ~(1 << 28)) | (v << 28); }

        bool GetC() const { return (value >> 29) & 1; }
        void SetC(bool c) { value = (value & ~(1 << 29)) | (c << 29); }

        bool GetZ() const { return (value >> 30) & 1; }
        void SetZ(bool z) { value = (value & ~(1 << 30)) | (z << 30); }

        bool GetN() const { return (value >> 31) & 1; }
        void SetN(bool n) { value = (value & ~(1 << 31)) | (n << 31); }
    };

    enum class ProcessorMode {
        USER = 0x10, // USER
        FIQ = 0x11, // FAST INTERRUPT
        IRQ = 0x12, // INTERRUPT 
        SVC = 0x13, // SUPERVISOR
        ABT = 0x17, // ABORT
        UND = 0x1B, // UNDEFINED
        SYSTEM = 0x1F // PRIVILEGED
    };

    std::array<Word, 16> _regs{};
    std::array<Word, 16> _regs_fiq{};
    std::array<Word, 16> _regs_svc{};
    std::array<Word, 16> _regs_abt{};
    std::array<Word, 16> _regs_irq{};
    std::array<Word, 16> _regs_und{};
    StatusReg _cpsr{};
    StatusReg _spsr_fiq, _spsr_svc, _spsr_abt, _spsr_irq, _spsr_und{};

    Word* Reg(int index);
    StatusReg* CPSR();
    StatusReg* SPSR();

    // Instruction will act as NOP if cond is not met.
    bool CheckCondition(Byte cond);

    Word FetchWord();
    HalfWord FetchHalfword();

    // [Cond:4][00][I:1][OpCode:4][S:1][Rn:4][Rd:4][Operand2:12]
    void ExecuteARM(Word opcode);
    void ExecuteTHUMB(HalfWord opcode);

    // Barrel shifter helper
    Word GetOperand2(Word opcode, bool immediate, bool& carryOut);
    
public:

    CPU_ARM7TDMI(IBus* bus);

    std::string DEBUG_GetDebugString();

    void Reset();

    void Clock();

};