#include "Bus.hpp"
#include "Helpers.hpp"

Halfword IBus::ReadHalfword(Word addr) {
    return MakeHalfword(ReadByte(addr), ReadByte(addr + 1));
}

void IBus::WriteHalfword(Word addr, Halfword value) {
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
    Halfword low, high;
    SplitWord(value, low, high);
    WriteHalfword(addr, low); WriteHalfword(addr + 2, high);
}

Bus::Bus() :
_bios({}),
_board_wram({}),
_chip_wram({})
{
    
}

Byte Bus::ReadByte(Word addr) {
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

void Bus::WriteByte(Word addr, Byte value)
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
