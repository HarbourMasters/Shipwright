#pragma once

#include <ship/window/gui/GuiWindow.h>

class ActorViewerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
};
