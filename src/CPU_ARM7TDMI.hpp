// ARM7TDMI Implementation

#pragma once

#include "types.hpp"

// https://mgba-emu.github.io/gbatek

class CPU_ARM7TDMI {

    public:
    // Lo registers
    u32 r0;
    u32 r1;
    u32 r2;
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    // Hi registers
    u32 r8, r8_fiq; 
    u32 r9, r9_fiq;
    u32 r10, r10_fiq;
    u32 r11, r11_fiq;
    u32 r12, r12_fiq;

    // Special registers
    u32 r13, r13_svc, r13_abt, r13_irq, r13_und; // Stack pointer
    u32 r14, r14_svc, r14_abt, r14_irq, r14_und; // Link register
    u32 r15; // Program counter
    
    // Program status registers
    u32 cpsr;
    u32 spsr_fiq, spsr_svc, spsr_ab, spsr_irq, spsr_und;

    CPU_ARM7TDMI();

    void Clock();

};