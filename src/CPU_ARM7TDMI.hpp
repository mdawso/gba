// ARM7TDMI Implementation

#pragma once

#include "Types.hpp"

// https://mgba-emu.github.io/gbatek

class CPU_ARM7TDMI {

    public:
    // Lo registers
    Word r0;
    Word r1;
    Word r2;
    Word r3;
    Word r4;
    Word r5;
    Word r6;
    Word r7;

    // Hi registers
    Word r8, r8_fiq; 
    Word r9, r9_fiq;
    Word r10, r10_fiq;
    Word r11, r11_fiq;
    Word r12, r12_fiq;

    // Special registers
    Word r13, r13_svc, r13_abt, r13_irq, r13_und; // Stack pointer
    Word r14, r14_svc, r14_abt, r14_irq, r14_und; // Link register
    Word r15; // Program counter
    
    // Program status registers
    struct cpsr {
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
    } cpsr;

    Word spsr_fiq, spsr_svc, spsr_abt, spsr_irq, spsr_und;

    CPU_ARM7TDMI();
    
    void Reset();

    void Clock();

};