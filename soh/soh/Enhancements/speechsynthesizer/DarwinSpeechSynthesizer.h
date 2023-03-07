//
//  DarwinSpeechSynthesizer.h
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#ifndef SOHDarwinSpeechSynthesizer_h
#define SOHDarwinSpeechSynthesizer_h

#include "SpeechSynthesizer.h"

class DarwinSpeechSynthesizer : public SpeechSynthesizer {
  public:
    DarwinSpeechSynthesizer();

    void Speak(const char* text, const char* language);

  protected:
    bool DoInit(void);
    void DoUninitialize(void);

  private:
    void* mSynthesizer;
};

#endif /* DarwinSpeechSynthesizer_h */
