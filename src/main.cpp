#include "Bus.hpp"
#include <memory>
#include <raylib.h>
#include <print>

int main(int argc, char** argv) {

    // if (argc != 2) {
    //     std::println(stderr, "ERROR: No ROM file provided.");
    //     std::println(stderr, "Usage: {} <path-to-rom>", argv[0]);
    //     exit(1);
    // }

    // std::unique_ptr<Cartridge> cart{};

    // try {
    //     cart = std::make_unique<Cartridge>(argv[1]);
    // } catch (std::exception& e) {
    //     std::println(stderr, "ERROR: {}", e.what());
    // }

    std::unique_ptr<IBus> bus = std::make_unique<SimpleBus>();
    
    bus->WriteWord(0x00, 0xE3A0000A); // MOV R0, #10         
    bus->WriteWord(0x04, 0xE3A01014); // MOV R1, #20         
    bus->WriteWord(0x08, 0xE0802001); // ADD R2, R0, R1     
    bus->WriteWord(0x0C, 0xE0413000); // SUB R3, R1, R0     
    bus->WriteWord(0x10, 0xE0040190); // MUL R4, R0, R1      
    bus->WriteWord(0x14, 0xE1A05080); // MOV R5, R0, LSL #1  
    bus->WriteWord(0x18, 0xE0006001); // AND R6, R0, R1      
    bus->WriteWord(0x1C, 0xE1807001); // ORR R7, R0, R1      
    bus->WriteWord(0x20, 0xE1500001); // CMP R0, R1          
    bus->WriteWord(0x24, 0x03A080FF); // MOVEQ R8, #255     
    bus->WriteWord(0x28, 0x13A080FF); // MOVNE R8, #255     
    bus->WriteWord(0x2C, 0xE59F9010); // LDR R9, [PC, #16]   
    bus->WriteWord(0x30, 0xE58F9014); // STR R9, [PC, #20]   
    bus->WriteWord(0x34, 0xEAFFFFFE); // B 0x34

    // --- Data Section ---
    bus->WriteWord(0x38, 0xE1A00000); // NOP
    bus->WriteWord(0x3C, 0xE1A00000); // NOP
    bus->WriteWord(0x40, 0xE1A00000); // NOP
    bus->WriteWord(0x44, 0xDEADBEEF);
    bus->WriteWord(0x48, 0x00000000);

    for (int i = 0; i < 30; i++) 
    {
        bus->Tick();
    }
    std::println("Done.");
    

    // SetTargetFPS(60);
    // InitWindow(800, 600, "GBA");

    // while (!WindowShouldClose()) {
    //     BeginDrawing();
    //     ClearBackground(RAYWHITE);

    //     EndDrawing();
    // }

    // CloseWindow();

}