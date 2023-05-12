#pragma once

#include <libultraship/libultraship.h>

namespace GameControlEditor {
class GameControlEditorWindow : public LUS::GuiWindow {
    public:
        using LUS::GuiWindow::GuiWindow;

        void InitElement() override;
        void DrawElement() override;
        void UpdateElement() override {};
};

static int CurrentPort = 0;
static int BtnReading = -1;

} // namespace GameControlEditor
