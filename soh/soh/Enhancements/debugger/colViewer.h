#pragma once

#include <libultraship/libultraship.h>

void DrawColViewer();

class ColViewerWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};