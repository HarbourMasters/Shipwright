#pragma once

#include <libultraship/libultraship.h>

class ActorViewerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void DrawElement() override;
    void InitElement() override;
    void UpdateElement() override {};
};
