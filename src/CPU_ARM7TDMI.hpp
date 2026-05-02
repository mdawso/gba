// ARM7TDMI Implementation

#pragma once

#include "Types.hpp"

// https://mgba-emu.github.io/gbatek

class CPU_ARM7TDMI {

private:

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

    enum class ProcessorMode {
        USER = 0x10, // USER
        FIQ = 0x11, // FAST INTERRUPT
        IRQ = 0x12, // INTERRUPT 
        SVC = 0x13, // SUPERVISOR
        ABT = 0x17, // ABORT
        UND = 0x1B, // UNDEFINED
        SYSTEM = 0x1F // PRIVILEGED
    };

    // Word _r0, _r1, _r2, _r3, _r4, _r5, _r6, _r7 = 0;
    // Word _r8, _r8_fiq, _r9, _r9_fiq, _r10, _r10_fiq, _r11, _r11_fiq, _r12, _r12_fiq = 0;
    // Word _r13, _r13_svc, _r13_abt, _r13_irq, _r13_und = 0;  // Stack pointer
    // Word _r14, _r14_svc, _r14_abt, _r14_irq, _r14_und = 0;  // Link register
    // Word _r15 = 0;                                          // Program counter
    // StatusReg _cpsr {};
    // StatusReg _spsr_fiq, _spsr_svc, _spsr_abt, _spsr_irq, _spsr_und {};

    Word _regs[16] {};
    Word _regs_fiq[16] {};
    Word _regs_svc[16] {};
    Word _regs_abt[16] {};
    Word _regs_irq[16] {};
    Word _regs_und[16] {};
    StatusReg _cpsr {};
    StatusReg _spsr_fiq, _spsr_svc, _spsr_abt, _spsr_irq, _spsr_und {};

    Word GetReg(int index);
    void SetReg(int index, Word value);

    StatusReg GetCPSR();
    void SetCPSR(StatusReg value);

    StatusReg GetSPSR();
    void SetSPSR(StatusReg value);

public:



    CPU_ARM7TDMI();
    
    void Reset();

    void Clock();

};