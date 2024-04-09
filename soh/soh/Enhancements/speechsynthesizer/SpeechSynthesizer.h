//
//  SpeechSynthesizer.h
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#ifndef SOHSpeechSynthesizer_h
#define SOHSpeechSynthesizer_h

#include <stdio.h>

class SpeechSynthesizer {
  public:
    static SpeechSynthesizer* Instance;
    SpeechSynthesizer();

    bool Init(void);
    void Uninitialize(void);
    virtual void Speak(const char* text, const char* language) = 0;

    bool IsInitialized(void);

  protected:
    virtual bool DoInit(void) = 0;
    virtual void DoUninitialize(void) = 0;

  private:
    bool mInitialized;
};

#endif /* SpeechSynthesizer_h */

#ifdef _WIN32
#include "SAPISpeechSynthesizer.h"
#elif defined(__APPLE__)
#include "DarwinSpeechSynthesizer.h"
#endif

#include "SpeechLogger.h"
