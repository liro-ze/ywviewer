#include <raylib.h>

#include "app_state.h"

int main(int argc, char** argv)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(960, 540, "YW-Viewer");
    SetTargetFPS(24);

    app_state state = { 0 };
    InitAppState(&state);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            if (!UpdateAppState(&state))
                break;
        EndDrawing();
    }

    DestroyAppState(&state);

    CloseWindow();
}
