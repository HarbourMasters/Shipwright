#pragma once

#include <libultraship/libultraship.h>
#include "window/gui/GuiMenuBar.h"
#include "window/gui/GuiElement.h"

class SohModalWindow : public LUS::GuiWindow {
  public:
    using LUS::GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};
    void RegisterPopup(std::string title, std::string message, std::string button1 = "OK", std::string button2 = "", std::function<void()> button1callback = nullptr, std::function<void()> button2callback = nullptr);
};