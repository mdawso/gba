#include "Cartridge.hpp"
#include <raylib.h>
#include <print>

int main(int argc, char** argv) {

    Cartridge cartridge;

    if (argc != 2) {
        std::println(stderr, "ERROR: No ROM file provided.");
        std::println(stderr, "Usage: {} <path-to-rom>", argv[0]);
        exit(1);
    }

    try {
        cartridge = Cartridge(argv[1]);
        auto h = cartridge.GetHeader();
        std::println("Got header");
    } catch (std::exception& e) {
        std::println(stderr, "ERROR: {}", e.what());
    }

    SetTargetFPS(60);
    InitWindow(800, 600, "GBA");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

}