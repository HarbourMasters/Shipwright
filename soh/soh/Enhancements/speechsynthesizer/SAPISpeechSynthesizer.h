//
//  SAPISpeechSynthesizer.h
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#ifndef SOHSAPISpeechSynthesizer_h
#define SOHSAPISpeechSynthesizer_h

#include "SpeechSynthesizer.h"
#include <stdio.h>

class SAPISpeechSynthesizer : public SpeechSynthesizer {
  public:
    SAPISpeechSynthesizer();

    void Speak(const char* text, const char* language);

  protected:
    bool DoInit(void);
    void DoUninitialize(void);
};

#endif /* SAPISpeechSynthesizer_h */
