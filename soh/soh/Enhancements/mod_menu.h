#pragma once

#include <ship/window/gui/GuiWindow.h>

#ifdef __cplusplus
class ModMenuWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};
#endif