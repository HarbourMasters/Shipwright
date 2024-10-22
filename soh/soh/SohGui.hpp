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
#include "Enhancements/audio/AudioEditor.h"
#include "Enhancements/controls/InputViewer.h"
#include "Enhancements/cosmetics/CosmeticsEditor.h"
#include "Enhancements/debugger/actorViewer.h"
#include "Enhancements/debugger/colViewer.h"
#include "Enhancements/debugger/debugSaveEditor.h"
#include "Enhancements/debugger/hookDebugger.h"
#include "Enhancements/debugger/dlViewer.h"
#include "Enhancements/debugger/valueViewer.h"
#include "Enhancements/gameplaystatswindow.h"
#include "Enhancements/randomizer/randomizer_check_tracker.h"
#include "Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "Enhancements/randomizer/randomizer_item_tracker.h"
#include "Enhancements/randomizer/randomizer_settings_window.h"
#include "Enhancements/timesplits/TimeSplits.h"
#include "SohModals.h"

#ifdef __cplusplus
extern "C" {
#endif
    void enableBetaQuest();
    void disableBetaQuest();
#ifdef __cplusplus
}
#endif

namespace SohGui {
    void SetupHooks();
    void SetupGuiElements();
    void Draw();
    void Destroy();
    void RegisterPopup(std::string title, std::string message, std::string button1 = "OK", std::string button2 = "", std::function<void()> button1callback = nullptr, std::function<void()> button2callback = nullptr);
}

#endif /* SohGui_hpp */
