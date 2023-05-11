#pragma once
#include "stdint.h"

#include "GuiWindow.h"
#include <ImGui/imgui.h>

class AudioEditorWindow : public GuiWindow, public std::enable_shared_from_this<AudioEditorWindow> {

}

// void InitAudioEditor();
void AudioEditor_RandomizeAll();
void AudioEditor_ResetAll();
