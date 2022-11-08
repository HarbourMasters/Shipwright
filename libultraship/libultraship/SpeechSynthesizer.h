//
//  SpeechSynthesizer.h
//  libultraship
//
//  Created by David Chavez on 10.10.22.
//

#ifndef SpeechSynthesizer_h
#define SpeechSynthesizer_h

#include <string>

namespace Ship {
    class SpeechSynthesizer {
    public:
        SpeechSynthesizer() {};
        
        virtual bool Init(void) = 0;
        virtual void Speak(std::string text) = 0;
    };
}

#ifdef _WIN32
#include "SAPISpeechSynthesizer.h"
#elif defined(__APPLE__)
#include "DarwinSpeechSynthesizer.h"
#endif

#endif /* SpeechSynthesizer_h */
