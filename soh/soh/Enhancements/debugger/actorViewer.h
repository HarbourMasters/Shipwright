#pragma once

#include <libultraship/libultraship.h>

class ActorViewerWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void Draw() override;
    void Init() override {};
    void Update() override {};
};