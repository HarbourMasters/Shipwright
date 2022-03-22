#include <iostream>
#include "raylib.h"
#include "game/game.h"
#include "impl/fs-bridge/fs-bridge.h"
#include "impl/fs-bridge/windows/fs-windows.h"
#include "impl/fs-bridge/linux/fs-linux.h"
#include "impl.h"

using namespace std;

FSBridge* NativeFS = nullptr;
OTRGame* Game = nullptr;

void UpdateDrawFrame(void) {
    Game->update();
    Game->draw();
}

int main() {
	constexpr Vector2 windowSize = Vector2(400, 200);
    SetTargetFPS(144);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(windowSize.x, windowSize.y, "");
#ifdef _WIN32
    NativeFS = new WindowsBridge;
#else
    NativeFS = new LinuxBridge;
#endif
    Game = new OTRGame();
    Game->preload();
    Game->init();
    while(!WindowShouldClose()) {
        UpdateDrawFrame();
    }
    CloseWindow();
    return 0;
}