#pragma once
#include "stdint.h"

#include "libultraship/libultra/types.h"
#ifdef __cplusplus
#include <ship/window/gui/GuiWindow.h>
#include "AudioCollection.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

class AudioEditor : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void DrawElement() override;
    void InitElement() override;
    void UpdateElement() override{};
    ~AudioEditor(){};

  private:
    // void DrawPreviewButton(uint16_t sequenceId, std::string sfxKey, SeqType sequenceType);
    // void Draw_SfxTab(const std::string& tabId, SeqType type);
    uint16_t mPlayingSeq = 0;
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
u16 AudioEditor_GetOriginalSeq(u16 seqId);

#ifdef __cplusplus
}
#endif
