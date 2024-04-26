#pragma once
#include "stdint.h"

#ifdef __cplusplus

#include <libultraship/libultraship.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <ImGui/imgui.h>
#include "AudioCollection.h"

class AudioEditor : public ShipDK::GuiWindow {
    public:
        using GuiWindow::GuiWindow;

        void DrawElement() override;
        void InitElement() override;
        void UpdateElement() override {};
        ~AudioEditor() {};
};

void AudioEditor_RandomizeAll();
void AudioEditor_RandomizeGroup(SeqType group);
void AudioEditor_ResetAll();
void AudioEditor_ResetGroup(SeqType group);
void AudioEditor_LockAll();
void AudioEditor_UnlockAll();

extern "C" {
#endif

u16 AudioEditor_GetReplacementSeq(u16 seqId);


#ifdef __cplusplus
}
#endif
