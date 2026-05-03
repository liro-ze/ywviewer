#include <raylib.h>

#include "app_state.h"

int main(int argc, char** argv)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(960, 540, "YW-Viewer");
    SetTargetFPS(24);

    app_state state = { 0 };
    InitAppState(&state);

    const char* vertShader =
        "   #version 330                                        \n"
        "                                                       \n"
        "   in vec3 vertexPosition;                             \n"
        "   in vec2 vertexTexCoord;                             \n"
        "   in vec3 vertexNormal;                               \n"
        "   in vec4 vertexColor;                                \n"
        "                                                       \n"
        "   uniform mat4 mvp;                                   \n"
        "                                                       \n"
        "   out vec2 fragTexCoord;                              \n"
        "   out vec4 fragColor;                                 \n"
        "                                                       \n"
        "                                                       \n"
        "   void main()                                         \n"
        "   {                                                   \n"
        "       fragTexCoord = vertexTexCoord;                  \n"
        "       fragColor = vertexColor;                        \n"
        "                                                       \n"
        "       gl_Position = mvp*vec4(vertexPosition, 1.0);    \n"
        "   }                                                   \n";

    const char* fragShader =
        "    #version 330                                               \n"
        "                                                               \n"
        "    in vec2 fragTexCoord;                                      \n"
        "    in vec4 fragColor;                                         \n"
        "                                                               \n"
        "    uniform sampler2D texture0;                                \n"
        "    uniform int channel0;                                      \n"
        "                                                               \n"
        "    out vec4 finalColor;                                       \n"
        "                                                               \n"
        "    void main()                                                \n"
        "    {                                                          \n"
        "        vec4 texel = texture(texture0, fragTexCoord);          \n"
        "        if (channel0 != -1)                                    \n"
        "        {                                                      \n"
        "            float v = texel[channel0];                         \n"
        "            finalColor = vec4(1.0, 1.0, 1.0, v);               \n"
        "        }                                                      \n"
        "        else                                                   \n"
        "        {                                                      \n"
        "            finalColor = texel;                                \n"
        "        }                                                      \n"
        "        finalColor = finalColor * fragColor;                   \n"
        "    }                                                          \n";

    state.shader = LoadShaderFromMemory(vertShader, fragShader);
    state.channel0Loc = GetShaderLocation(state.shader, "channel0");
    
    int value = -1;
    SetShaderValue(state.shader, state.channel0Loc, &value, SHADER_UNIFORM_INT);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            if (!UpdateAppState(&state))
                break;
        }
        EndDrawing();
    }

    DestroyAppState(&state);

    CloseWindow();
}
