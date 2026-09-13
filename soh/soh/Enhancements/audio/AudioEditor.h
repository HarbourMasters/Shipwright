#pragma once

#ifdef __cplusplus

#include <libultraship/libultra/types.h>
#include <ship/window/gui/GuiWindow.h>

#include "AudioCollection.h"

class AudioEditor final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void DrawElement() override;
    void InitElement() override;
    void UpdateElement() override{};
    ~AudioEditor(){};
};

void AudioEditor_RandomizeAll();
void AudioEditor_AutoRandomizeAll();
void AudioEditor_RandomizeGroup(SeqType group);
void AudioEditor_ResetAll();
void AudioEditor_ResetGroup(SeqType group);
void AudioEditor_LockAll();
void AudioEditor_UnlockAll();
// Pushes the current Master Volume CVar onto the running FluidSynth's master
// gain so the synth tracks the slider. A no-op when no synth is installed; call
// from the Master Volume slider's handler.
void AudioEditor_ApplySynthMasterVolume();

extern "C" {
#endif

u16 AudioEditor_GetReplacementSeq(u16 seqId);

#ifdef __cplusplus
}
#endif
