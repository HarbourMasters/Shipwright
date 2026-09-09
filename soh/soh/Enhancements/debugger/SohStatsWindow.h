#pragma once

#include <ship/window/gui/GuiWindow.h>

class SohStatsWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    ~SohStatsWindow(){};

  protected:
    void InitElement() override{};
    void DrawElement() override;
    void UpdateElement() override{};
};
