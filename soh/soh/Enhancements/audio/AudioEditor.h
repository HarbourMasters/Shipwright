#pragma once
#include "stdint.h"

#include <libultraship/libultraship.h>
#include <ImGui/imgui.h>

class AudioEditor : public LUS::GuiWindow {
    public:
        using LUS::GuiWindow::GuiWindow;

        void DrawElement() override;
        void InitElement() override {};
        void UpdateElement() override {};
        ~AudioEditor() {};
};

void AudioEditor_RandomizeAll();
void AudioEditor_ResetAll();
