//
//  DarwinSpeechSynthesizer.h
//  libultraship
//
//  Created by David Chavez on 10.10.22.
//

#ifndef DarwinSpeechSynthesizer_h
#define DarwinSpeechSynthesizer_h

#include "SpeechSynthesizer.h"
#include <string>

namespace Ship {
    class DarwinSpeechSynthesizer: public SpeechSynthesizer {
    public:
        DarwinSpeechSynthesizer() {};
        
        bool Init(void);
        void Speak(std::string text);
    private:
        void *synthesizer;
    };
}

#endif /* DarwinSpeechSynthesizer_h */
