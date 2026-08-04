#ifndef SOH_CONSOLE_H
#define SOH_CONSOLE_H

#include <ship/window/gui/ConsoleWindow.h>

class SohConsoleWindow : public Ship::ConsoleWindow {
  public:
    using ConsoleWindow::ConsoleWindow;

  protected:
    void DrawElement() override;
};

#endif // SOH_CONSOLE_H
