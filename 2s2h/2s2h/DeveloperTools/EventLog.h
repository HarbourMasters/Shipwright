#pragma once

#include <ship/window/gui/GuiWindow.h>

class EventLogWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};
