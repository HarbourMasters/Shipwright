#pragma once

#include <cstdint>

namespace Ship {
class Switch {
public:
    static void Init();
    static void Exit();
    static void Update();
    static bool IsRunning();
    static void GetDisplaySize(uint32_t *width, uint32_t *height);
    static void GetTouchPosition(int *touchX, int *touchY);
    static float GetDPI();
};
};