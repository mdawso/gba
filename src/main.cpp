#include "Bus.hpp"
#include <raylib.h>
#include <print>

int main() {

    IBus* bus = new Bus();
    bus->WriteWord(0, 0xAABBCCDD);
    std::println("Reading byte = {}", bus->ReadByte(0));
    std::println("Reading WORD = {}", bus->ReadWord(0));

    SetTargetFPS(60);
    InitWindow(800, 600, "GBA");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

}