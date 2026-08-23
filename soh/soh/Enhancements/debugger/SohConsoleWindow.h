#pragma once

#include <ship/window/gui/ConsoleWindow.h>

class SohConsoleWindow : public Ship::ConsoleWindow {
  public:
    using ConsoleWindow::ConsoleWindow;

  protected:
    void DrawElement() override;
};
