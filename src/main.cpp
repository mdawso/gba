#include <raylib.h>

int main() {

    SetTargetFPS(60);
    InitWindow(800, 600, "GBA");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

}