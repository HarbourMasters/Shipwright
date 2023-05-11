#pragma once

#include <libultraship/libultraship.h>

void DrawColViewer();

class ColViewerWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void Init() override;
    void Draw() override;
};