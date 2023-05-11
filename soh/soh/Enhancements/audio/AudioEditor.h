#pragma once
#include "stdint.h"

#include "libultraship/libultraship.h"
#include <ImGui/imgui.h>

class AudioEditor : public LUS::GuiWindow {
    public:
        // using LUS::GuiWindow::GuiWindow;

        void Draw() override;
        void Init() override {};
        void Update() override {};
};

void AudioEditor_RandomizeAll();
void AudioEditor_ResetAll();
