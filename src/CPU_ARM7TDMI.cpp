#include "CPU_ARM7TDMI.hpp"
#include "Types.hpp"

CPU_ARM7TDMI::CPU_ARM7TDMI() 
{
    
}

Word CPU_ARM7TDMI::GetReg(int index)
{
    switch (index) {
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 15: 
        {
            return _regs[index];
        }
        case 8: case 9: case 10: case 11: case 12:
        {
            if (static_cast<ProcessorMode>(GetCPSR().mode) == ProcessorMode::FIQ) 
            { 
                return _regs_fiq[index];
            } 
            else {
                return _regs[index];
            }
        }
        case 13: case 14: 
        {
            switch (static_cast<ProcessorMode>(GetCPSR().mode)) 
            {
                case ProcessorMode::SVC:
                {
                    return _regs_svc[index];
                }
                case ProcessorMode::ABT:
                {
                    return _regs_abt[index];
                }
                case ProcessorMode::IRQ:
                {
                    return _regs_irq[index];
                }
                case ProcessorMode::UND:
                {
                    return _regs_und[index];
                }
                default:
                {
                    return _regs[index];
                }
            }
        }
        default:
        {
            return 0;
        }
    }
}

void CPU_ARM7TDMI::SetReg(int index, Word value)
{
    switch (index) {
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 15: 
        {
            _regs[index] = value; break;
        }
        case 8: case 9: case 10: case 11: case 12:
        {
            if (static_cast<ProcessorMode>(GetCPSR().mode) == ProcessorMode::FIQ) 
            { 
                _regs_fiq[index] = value; break;
            } 
            else {
                _regs[index] = value; break;
            }
        }
        case 13: case 14: 
        {
            switch (static_cast<ProcessorMode>(GetCPSR().mode)) 
            {
                case ProcessorMode::SVC:
                {
                    _regs_svc[index] = value; break;
                }
                case ProcessorMode::ABT:
                {
                    _regs_abt[index] = value; break;
                }
                case ProcessorMode::IRQ:
                {
                    _regs_irq[index] = value; break;
                }
                case ProcessorMode::UND:
                {
                    _regs_und[index] = value; break;
                }
                default:
                {
                    _regs[index] = value; break;
                }
            }
        }
    }
}

CPU_ARM7TDMI::StatusReg CPU_ARM7TDMI::GetCPSR()
{
    return _cpsr;
}

void CPU_ARM7TDMI::SetCPSR(StatusReg value)
{
    _cpsr = value;
}

CPU_ARM7TDMI::StatusReg CPU_ARM7TDMI::GetSPSR()
{
    switch (static_cast<ProcessorMode>(GetCPSR().mode))
    {
        case ProcessorMode::FIQ: 
        {
            return _spsr_fiq;
        }
        case ProcessorMode::SVC:
        {
            return _spsr_svc;
        }
        case ProcessorMode::ABT:
        {
            return _spsr_abt;
        }
        case ProcessorMode::IRQ:
        {
            return _spsr_irq;
        }
        case ProcessorMode::UND:
        {
            return _spsr_und;
        }
        default:
        {
            return _cpsr; // SPSR does not exist in USER or SYSTEM mode. Usually returns value of CPSR. This is undefined behaviour.
        }
    }
}

void CPU_ARM7TDMI::SetSPSR(StatusReg value)
{
    switch (static_cast<ProcessorMode>(GetCPSR().mode))
    {
        case ProcessorMode::FIQ: 
        {
            _spsr_fiq = value; break;
        }
        case ProcessorMode::SVC:
        {
            _spsr_svc = value; break;
        }
        case ProcessorMode::ABT:
        {
            _spsr_abt = value; break;
        }
        case ProcessorMode::IRQ:
        {
            _spsr_irq = value; break;
        }
        case ProcessorMode::UND:
        {
            _spsr_und = value; break;
        }
        default:
        {
            break; // SPSR does not exist in USER or SYSTEM mode. Usually treated as a NOP. This is undefined behaviour.
        }
    }
}

void CPU_ARM7TDMI::Reset() 
{

}

void CPU_ARM7TDMI::Clock() 
{

}