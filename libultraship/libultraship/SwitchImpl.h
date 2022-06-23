#pragma once

#include <cstdint>
#include "SwitchPerformanceProfiles.h"

namespace Ship {

enum SwitchProfiles {
    MAXIMUM,
    HIGH,
    BOOST,
    STOCK,
    POWERSAVINGM1,
    POWERSAVINGM2,
    POWERSAVINGM3
};

class Switch {
public:
    static void Init();
    static void Exit();
    static void Update();
    static bool IsRunning();
    static void GetDisplaySize(int *width, int *height);
    static void GetTouchPosition(int *touchX, int *touchY);
    static void ApplyOverclock();
    static float GetDPI();
};
};