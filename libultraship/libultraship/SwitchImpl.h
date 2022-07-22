#pragma once

#include <cstdint>
#include <string>

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

    enum SwitchPhase {
        PreInitPhase,
        PostInitPhase
    };

    class Switch {
        public:
            static void Init(SwitchPhase phase);
            static void Exit();
            static void SetupFont(ImFontAtlas* fonts);
            static bool IsRunning();
            static void GetDisplaySize(int *width, int *height);
            static void ApplyOverclock();
            static void ThrowMissingOTR(std::string OTRPath);
            static void PrintErrorMessageToScreen(const char *str, ...);
    };
};