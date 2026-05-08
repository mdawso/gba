#pragma once

#include "Types.hpp"

#include <array>

class IBus;

#include "CPU_ARM7TDMI.hpp"
#include "PPU.hpp"

class IBus
{
public:
    virtual ~IBus() = default;

    virtual Byte ReadByte(Word addr) = 0;
    virtual void WriteByte(Word addr, Byte value) = 0;
    
    HalfWord ReadHalfword(Word addr);
    void WriteHalfword(Word addr, HalfWord value);
    Word ReadWord(Word addr);
    void WriteWord(Word addr, Word value);

    virtual void Tick() = 0;
};

class SystemBus : public IBus
{
private:
std::array<Byte, 1024 * 16> _bios{};
std::array<Byte, 1024 * 256> _board_wram{};
std::array<Byte, 1024 * 32> _chip_wram{};
CPU_ARM7TDMI _cpu;
PPU _ppu;

public:
    SystemBus();

    Byte ReadByte(Word addr) override;
    void WriteByte(Word addr, Byte value) override;
    void Tick() override;
};

class SimpleBus : public IBus
{
private:
std::array<Byte, 1024 * 1024> _ram;
CPU_ARM7TDMI _cpu;

public:
    SimpleBus();

    Byte ReadByte(Word addr) override;
    void WriteByte(Word addr, Byte value) override;
    void Tick() override;
};