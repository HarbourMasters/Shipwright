//
//  SpeechSynthesizer.h
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#pragma once

#include <stdint.h>

// Speech rate is carried as a percentage of the engine's normal speed; backends map the ends onto their own range.
constexpr int32_t SPEECH_RATE_MIN = 50;
constexpr int32_t SPEECH_RATE_MAX = 250;

class SpeechSynthesizer {
  public:
    static SpeechSynthesizer* Instance;
    SpeechSynthesizer();

    bool Init(void);
    void Uninitialize(void);
    virtual void Speak(const char* text, const char* language) = 0;

    // Reads the speech CVars and pushes them to the backend. Call after changing any of them.
    void ApplySettings(void);

    bool IsInitialized(void);

  protected:
    virtual bool DoInit(void) = 0;
    virtual void DoUninitialize(void) = 0;
    // rate and volume are percentages of the backend's normal speech; pitch is 0-100 with 50 neutral.
    virtual void DoApplySettings(int32_t rate, int32_t volume, int32_t pitch);

  private:
    bool mInitialized;
};

#ifdef _WIN32
#include "SAPISpeechSynthesizer.h"
#elif defined(__APPLE__)
#include "DarwinSpeechSynthesizer.h"
#elif ESPEAK
#include "ESpeakSpeechSynthesizer.h"
#endif

#include "SpeechLogger.h"
