#pragma once

#ifdef __cplusplus

#include <libultraship/libultra/types.h>
#include "soh/SohGui/LocalizedWindow.h"

#include "AudioCollection.h"

class AudioEditor final : public SohGui::LocalizedWindow {
  public:
    using LocalizedWindow::LocalizedWindow;

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

extern "C" {
#endif

u16 AudioEditor_GetReplacementSeq(u16 seqId);

#ifdef __cplusplus
}
#endif
