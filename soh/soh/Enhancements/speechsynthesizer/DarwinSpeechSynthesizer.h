//
//  DarwinSpeechSynthesizer.h
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#pragma once

#include <stdint.h>

#include "SpeechSynthesizer.h"

class DarwinSpeechSynthesizer : public SpeechSynthesizer {
  public:
    DarwinSpeechSynthesizer();

    void Speak(const char* text, const char* language);

  protected:
    bool DoInit(void);
    void DoUninitialize(void);
    void DoApplySettings(int32_t rate, int32_t volume, int32_t pitch);

  private:
    void* mSynthesizer;
    int32_t mRate = 100;
    int32_t mVolume = 100;
    int32_t mPitch = 50;
};
