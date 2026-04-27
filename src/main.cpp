#include "Bus.hpp"
#include <raylib.h>
#include <print>

int main() {

    IBus* bus = new Bus();
    bus->WriteByte(0, 0XAA);
    bus->WriteByte(1, 0XBB);
    bus->WriteByte(2, 0XCC);
    bus->WriteByte(3, 0XDD);
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