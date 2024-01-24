#pragma once

#include <libultraship/libultraship.h>

class InputViewer : public LUS::GuiWindow {
  public:
    using LUS::GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};

    InputViewer();
    ~InputViewer();

    void Draw();
};
