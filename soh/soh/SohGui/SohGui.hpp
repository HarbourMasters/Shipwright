//
//  SohGui.hpp
//  soh
//
//  Created by David Chavez on 24.08.22.
//

#pragma once

#include "SohMenu.h"

namespace SohGui {
void SetupHooks();
void SetupMenu();
void SetupMenuElements();
void SetupGuiElements();
void Draw();
void Destroy();
void RegisterPopup(std::string title, std::string message, std::string button1 = "OK", std::string button2 = "",
                   std::function<void()> button1callback = nullptr, std::function<void()> button2callback = nullptr);
size_t PopupsQueued();
bool DismissPopup(std::string title);
void ShowRandomizerSettingsMenu();
void ShowEscMenu();
UIWidgets::Colors GetMenuThemeColor();
std::shared_ptr<SohMenu> GetSohMenu();
void DrawStartingItemsMenu(WidgetInfo& info);
} // namespace SohGui

#define THEME_COLOR SohGui::GetMenuThemeColor()
