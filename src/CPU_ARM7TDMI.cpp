#include "CPU_ARM7TDMI.hpp"
#include "Bus.hpp"
#include "Types.hpp"
#include <memory>

CPU_ARM7TDMI::CPU_ARM7TDMI(std::shared_ptr<IBus> bus) :
_bus(bus)
{
    
}

Word* CPU_ARM7TDMI::Reg(int index) {
    if (index < 8 || index == 15) return &_regs[index];

    ProcessorMode mode = static_cast<ProcessorMode>(CPSR()->mode);

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
            return &_regs[index]; // USER / SYSTEM mode
    }
}

CPU_ARM7TDMI::StatusReg* CPU_ARM7TDMI::CPSR()
{
    return &_cpsr;
}

CPU_ARM7TDMI::StatusReg* CPU_ARM7TDMI::SPSR()
{
    switch (static_cast<ProcessorMode>(CPSR()->mode))
    {
        case ProcessorMode::FIQ: 
        {
            return &_spsr_fiq;
        }
        case ProcessorMode::SVC:
        {
            return &_spsr_svc;
        }
        case ProcessorMode::ABT:
        {
            return &_spsr_abt;
        }
        case ProcessorMode::IRQ:
        {
            return &_spsr_irq;
        }
        case ProcessorMode::UND:
        {
            return &_spsr_und;
        }
        default:
        {
            return &_cpsr; // SPSR does not exist in USER or SYSTEM mode. Usually returns value of CPSR. This is undefined behaviour.
        }
    }
}

Word CPU_ARM7TDMI::FetchWord() 
{
    Word pc = *Reg(15);
    Word opcode = _bus->ReadWord(pc);
    *Reg(15) += 4; // Increment PC
    return opcode;
}

bool CPU_ARM7TDMI::CheckCondition(Byte cond)
{
    bool n = CPSR()->N;
    bool z = CPSR()->Z;
    bool c = CPSR()->C;
    bool v = CPSR()->V;

    switch (cond) {
        case 0x0: return z;               // EQ
        case 0x1: return !z;              // NE
        case 0x2: return c;               // CS/HS
        case 0x3: return !c;              // CC/LO
        case 0x4: return n;               // MI
        case 0x5: return !n;              // PL
        case 0x6: return v;               // VS
        case 0x7: return !v;              // VC
        case 0x8: return c && !z;         // HI
        case 0x9: return !c || z;         // LS
        case 0xA: return n == v;          // GE
        case 0xB: return n != v;          // LT
        case 0xC: return !z && (n == v);  // GT
        case 0xD: return z || (n != v);   // LE
        case 0xE: return true;            // AL (Always)
        default: return false;
    }
}

void CPU_ARM7TDMI::ExecuteARM(Word opcode) {

}

void CPU_ARM7TDMI::Reset() 
{

}

void CPU_ARM7TDMI::Clock() {

    Word opcode = FetchWord();

    if (CPSR()->T == 0) {
        ExecuteARM(opcode);
    } else {
        // TODO: ExecuteTHUMB
    }
}