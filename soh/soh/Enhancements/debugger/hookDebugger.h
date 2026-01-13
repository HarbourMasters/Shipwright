#ifndef hookDebugger_h
#define hookDebugger_h

#include <libultraship/libultraship.h>

class HookDebuggerWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

#endif // hookDebugger_h
