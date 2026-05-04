#include "Bus.hpp"
#include "CPU_ARM7TDMI.hpp"
#include "Helpers.hpp"
#include <print>

HalfWord IBus::ReadHalfword(Word addr) {
    return MakeHalfword(ReadByte(addr), ReadByte(addr + 1));
}

void IBus::WriteHalfword(Word addr, HalfWord value) {
    Byte low, high;
    SplitHalfword(value, low, high);
    WriteByte(addr, low); WriteByte(addr + 1, high);
}

Word IBus::ReadWord(Word addr) {
    return MakeWord (
        MakeHalfword(ReadByte(addr), ReadByte(addr + 1)), 
        MakeHalfword(ReadByte(addr + 2), ReadByte(addr + 3)
        )
    );
}

void IBus::WriteWord(Word addr, Word value) {
    HalfWord low, high;
    SplitWord(value, low, high);
    WriteHalfword(addr, low); WriteHalfword(addr + 2, high);
}

SimpleBus::SimpleBus() :
_cpu(CPU_ARM7TDMI(this))
{
    _cpu.Reset();
}

Byte SimpleBus::ReadByte(Word addr)
{
    return _ram[addr];
}

void SimpleBus::WriteByte(Word addr, Byte value)
{
    _ram[addr] = value;
}

void SimpleBus::Tick()
{
    _cpu.Clock();
    std::println("{}", _cpu.DEBUG_GetDebugString());
} 

SystemBus::SystemBus() :
_cpu(CPU_ARM7TDMI(this))
{
    _cpu.Reset();
}

/*
General Internal Memory

  00000000-00003FFF   BIOS - System ROM         (16 KBytes)
  00004000-01FFFFFF   Not used
  02000000-0203FFFF   WRAM - On-board Work RAM  (256 KBytes) 2 Wait
  02040000-02FFFFFF   Not used
  03000000-03007FFF   WRAM - On-chip Work RAM   (32 KBytes)
  03008000-03FFFFFF   Not used
  04000000-040003FE   I/O Registers
  04000400-04FFFFFF   Not used

Internal Display Memory

  05000000-050003FF   BG/OBJ Palette RAM        (1 Kbyte)
  05000400-05FFFFFF   Not used
  06000000-06017FFF   VRAM - Video RAM          (96 KBytes)
  06018000-06FFFFFF   Not used
  07000000-070003FF   OAM - OBJ Attributes      (1 Kbyte)
  07000400-07FFFFFF   Not used

External Memory (Game Pak)

  08000000-09FFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 0
  0A000000-0BFFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 1
  0C000000-0DFFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 2
  0E000000-0E00FFFF   Game Pak SRAM    (max 64 KBytes) - 8bit Bus width
  0E010000-0FFFFFFF   Not used

Unused Memory Area

  10000000-FFFFFFFF   Not used (upper 4bits of address bus unused)
*/

Byte SystemBus::ReadByte(Word addr) {
    if (addr >= 0 && addr <= 0x3FFF) {
        return _bios[addr];
    } else if (addr >= 0x02000000 && addr <= 0x0203FFFF) {
        return _board_wram[addr & 0x3FFFF];
    } else if (addr >= 0x03000000 && addr <= 0x03007FFF) {
        return _chip_wram[addr & 0x7FFF];
    } else if (addr >= 0x4000000 && addr <= 0x040003FE) {
        // IO Registers
    } else if (addr >= 0x05000000 && addr <= 0x050003FF) {
        // BG/OBJ Palette RAM
    } else if (addr >= 0x06000000 && addr <= 0x06017FFF) {
        // VRAM
    } else if (addr >= 0x07000000 && addr <= 0x070003FF) {
        // OAM-OBJ Attrs
    } else if (addr >= 0x08000000 && addr <= 0x09FFFFFF) {
        // GamePak ROM - Wait State 0
    } else if (addr >= 0x0A000000 && addr <= 0x0BFFFFFF) {
        // GamePak ROM - Wait State 1
    } else if (addr >= 0x0C000000 && addr <= 0x0DFFFFFF) {
        // GamePak ROM - Wait State 2
    } else if (addr >= 0x0E000000 && addr <= 0x0E00FFFF) {
        // GamePak SRAM
    } else {
        // Read in unused memory
    }
    return 0;
}

void SystemBus::WriteByte(Word addr, Byte value)
{
    if (addr >= 0 && addr <= 0x3FFF) {
        // BIOS ROM
    } else if (addr >= 0x02000000 && addr <= 0x0203FFFF) {
        _board_wram[addr & 0x3FFFF] = value;
    } else if (addr >= 0x03000000 && addr <= 0x03007FFF) {
        _chip_wram[addr & 0x7FFF] = value;
    } else if (addr >= 0x4000000 && addr <= 0x040003FE) {
        // IO Registers
    } else if (addr >= 0x05000000 && addr <= 0x050003FF) {
        // BG/OBJ Palette RAM
    } else if (addr >= 0x06000000 && addr <= 0x06017FFF) {
        // VRAM
    } else if (addr >= 0x07000000 && addr <= 0x070003FF) {
        // OAM-OBJ Attrs
    } else if (addr >= 0x08000000 && addr <= 0x09FFFFFF) {
        // GamePak ROM - Wait State 0
    } else if (addr >= 0x0A000000 && addr <= 0x0BFFFFFF) {
        // GamePak ROM - Wait State 1
    } else if (addr >= 0x0C000000 && addr <= 0x0DFFFFFF) {
        // GamePak ROM - Wait State 2
    } else if (addr >= 0x0E000000 && addr <= 0x0E00FFFF) {
        // GamePak SRAM
    } else {
        // Write in unused memory.
    }
}

void SystemBus::Tick()
{
    _cpu.Clock();
}
