#pragma once

#include "menu/GuiWindow.h"

namespace GameControlEditor {
class GameControlEditorWindow : public LUS::GuiWindow {
    public:
        using LUS::GuiWindow::GuiWindow;

        void Init() override;
        void Draw() override;
};

static int CurrentPort = 0;
static int BtnReading = -1;

} // namespace GameControlEditor
