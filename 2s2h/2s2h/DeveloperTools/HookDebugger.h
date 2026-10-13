#ifndef HOOK_DEBUGGER_H
#define HOOK_DEBUGGER_H

#include <ship/window/gui/GuiWindow.h>

class HookDebuggerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

#endif // HOOK_DEBUGGER_H
