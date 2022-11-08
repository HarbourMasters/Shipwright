//
//  SAPISpeechSynthesizer.h
//  libultraship
//
//  Created by David Chavez on 10.10.22.
//

#ifndef SAPISpeechSynthesizer_h
#define SAPISpeechSynthesizer_h

#include "SpeechSynthesizer.h"
#include <string>

namespace Ship {
    class SAPISpeechSynthesizer: public SpeechSynthesizer {
    public:
        SAPISpeechSynthesizer() {};

        bool Init(void);
        void Speak(std::string text);
    };
}

#endif /* SAPISpeechSynthesizer_h */