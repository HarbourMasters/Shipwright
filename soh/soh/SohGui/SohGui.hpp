//
//  SohGui.hpp
//  soh
//
//  Created by David Chavez on 24.08.22.
//

#ifndef SohGui_hpp
#define SohGui_hpp

#include <stdio.h>
#include "SohMenuBar.h"
#include "soh/Enhancements/audio/AudioEditor.h"
#include "soh/Enhancements/controls/InputViewer.h"
#include "soh/Enhancements/cosmetics/CosmeticsEditor.h"
#include "soh/Enhancements/debugger/actorViewer.h"
#include "soh/Enhancements/debugger/colViewer.h"
#include "soh/Enhancements/debugger/debugSaveEditor.h"
#include "soh/Enhancements/debugger/hookDebugger.h"
#include "soh/Enhancements/debugger/dlViewer.h"
#include "soh/Enhancements/debugger/SohConsoleWindow.h"
#include "soh/Enhancements/debugger/SohGfxDebuggerWindow.h"
#include "soh/Enhancements/debugger/SohStatsWindow.h"
#include "soh/Enhancements/debugger/valueViewer.h"
#include "soh/Enhancements/gameplaystatswindow.h"
#include "soh/Enhancements/randomizer/randomizer_check_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_item_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_settings_window.h"
#include "soh/Enhancements/timesplits/TimeSplits.h"
#include "soh/Enhancements/randomizer/Plandomizer.h"
#include "SohModals.h"

namespace SohGui {
    void SetupHooks();
    void SetupGuiElements();
    void Draw();
    void Destroy();
    void RegisterPopup(std::string title, std::string message, std::string button1 = "OK", std::string button2 = "", std::function<void()> button1callback = nullptr, std::function<void()> button2callback = nullptr);
    void ShowRandomizerSettingsMenu();
    UIWidgets::Colors GetMenuThemeColor();
}

#define THEME_COLOR SohGui::GetMenuThemeColor()

#endif /* SohGui_hpp */
