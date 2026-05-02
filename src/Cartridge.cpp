#include "Cartridge.hpp"
#include <fstream>
#include <print>

Cartridge::Cartridge()
{}

Cartridge::Cartridge(std::filesystem::path const& path)
{
    LoadFile(path);
}

void Cartridge::LoadFile(std::filesystem::path const& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::println("INFO: Successfully opened {} ({} bytes)", path.string(), size);
    _rom.resize(size);
    if (!file.read(reinterpret_cast<char*>(_rom.data()), size)) {
        throw std::runtime_error("Failed to read file content: " + path.string());
    }
    std::println("INFO: Successfully read {} bytes.", size);
    if(GetHeader().fixed != 0x96) { // fixed should always be 0x96 in a valid gba rom
        throw std::runtime_error("Invalid file header: " + path.string());
    }
    _file_path = path;
}

CartridgeHeader Cartridge::GetHeader()
{
    return *reinterpret_cast<CartridgeHeader*>(_rom.data());
}
