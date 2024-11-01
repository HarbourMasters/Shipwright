#ifndef HOLIDAY_HPP
#define HOLIDAY_HPP

#include <vector>
#include <functional>
#include <libultraship/libultraship.h>
#include "soh/UIWidgets.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/cosmetics/CosmeticsEditor.h"

inline std::vector<std::function<void()>> holidayDrawFuncs = {};
inline std::vector<std::function<void()>> holidayRegisterFuncs = {};

inline void DrawHolidayMenu() {
    if (ImGui::BeginMenu("Holiday")) {
        for (auto& drawFunc : holidayDrawFuncs) {
            ImGui::PushID(&drawFunc);
            drawFunc();
            ImGui::PopID();
        }
        ImGui::EndMenu();
    }
}

inline void RegisterHoliday() {
    for (auto& regFunc : holidayRegisterFuncs) {
        regFunc();
    }
}

struct Holiday {
    Holiday(std::function<void()> drawFunc, std::function<void()> registerFunc) {
        holidayDrawFuncs.push_back(drawFunc);
        holidayRegisterFuncs.push_back(registerFunc);
    }
};

#endif //HOLIDAY_HPP
