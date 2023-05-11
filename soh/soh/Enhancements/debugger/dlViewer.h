#pragma once

#include <libultraship/libultraship.h>

class DLViewerWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void Init() override;
    void Draw() override;
    void Update() override {};
};
