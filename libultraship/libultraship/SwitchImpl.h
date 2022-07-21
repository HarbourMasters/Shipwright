#pragma once

#include <cstdint>

#include "Lib/ImGui/imgui.h"
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
            static void SetupFont(ImFontAtlas* fonts);
            static bool IsRunning();
            static void GetDisplaySize(int *width, int *height);
            static void ApplyOverclock();
            static void PrintErrorMessageToScreen(const char *str, ...);
    };
};