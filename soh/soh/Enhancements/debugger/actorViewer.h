#pragma once

#include <libultraship/libultraship.h>

class ActorViewerWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void DrawElement() override;
    void InitElement() override;
    void UpdateElement() override{};
};
