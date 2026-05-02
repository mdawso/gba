#pragma once

#include <filesystem>
#include <vector>

#include "Types.hpp"

/*
  Address Bytes Expl.
  000h    4     ROM Entry Point  (32bit ARM branch opcode, eg. "B rom_start")
  004h    156   Nintendo Logo    (compressed bitmap, required!)
  0A0h    12    Game Title       (uppercase ascii, max 12 characters)
  0ACh    4     Game Code        (uppercase ascii, 4 characters)
  0B0h    2     Maker Code       (uppercase ascii, 2 characters)
  0B2h    1     Fixed value      (must be 96h, required!)
  0B3h    1     Main unit code   (00h for current GBA models)
  0B4h    1     Device type      (usually 00h) (bit7=DACS/debug related)
  0B5h    7     Reserved Area    (should be zero filled)
  0BCh    1     Software version (usually 00h)
  0BDh    1     Complement check (header checksum, required!)
  0BEh    2     Reserved Area    (should be zero filled)
  --- Additional Multiboot Header Entries ---
  0C0h    4     RAM Entry Point  (32bit ARM branch opcode, eg. "B ram_start")
  0C4h    1     Boot mode        (init as 00h - BIOS overwrites this value!)
  0C5h    1     Slave ID Number  (init as 00h - BIOS overwrites this value!)
  0C6h    26    Not used         (seems to be unused)
  0E0h    4     JOYBUS Entry Pt. (32bit ARM branch opcode, eg. "B joy_start")
*/

static_assert(true); // Shut up clangd!
#pragma pack(push)
#pragma pack(1)
struct CartridgeHeader {
    Byte rom_entry_point[4];
    Byte nintendo_logo[156];
    Byte game_title[12];
    Byte game_code[4];
    Byte maker_code[2];
    Byte fixed;
    Byte main_unit_code;
    Byte device_type;
    Byte reserved[7];
    Byte software_version;
    Byte complement_check;
    Byte reserved2[2];
    Byte ram_entry_point[4];
    Byte boot_mode;
    Byte slave_id_number;
    Byte reserved3[26];
    Byte joybus_entry_pt[4];
};
#pragma pack(pop)

class Cartridge {
private:
    std::filesystem::path _file_path = "";
    std::vector<Byte> _rom;
public:
    Cartridge();
    Cartridge(std::filesystem::path const &path);
    void LoadFile(std::filesystem::path const &path);
    CartridgeHeader GetHeader();
};