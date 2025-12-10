#pragma once

#include <libultraship/libultraship.h>
#include <ship/window/gui/GuiMenuBar.h>
#include <ship/window/gui/GuiElement.h>

class SohModalWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    void Draw() override;

    void InitElement() override{};
    void DrawElement() override;
    void UpdateElement() override{};
    void RegisterPopup(std::string title, std::string message, std::string button1 = "OK", std::string button2 = "",
                       std::function<void()> button1callback = nullptr,
                       std::function<void()> button2callback = nullptr);
    bool IsPopupOpen(std::string title);
    void DismissPopup();
};