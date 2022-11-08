//
//  DarwinSpeechSynthesizer.mm
//  libultraship
//
//  Created by David Chavez on 10.10.22.
//

#include "DarwinSpeechSynthesizer.h"
#import <AVFoundation/AVFoundation.h>

namespace Ship {
    bool DarwinSpeechSynthesizer::Init() {
        synthesizer = [[AVSpeechSynthesizer alloc] init];
        
        return true;
    }

    void DarwinSpeechSynthesizer::Speak(std::string text) {
        AVSpeechUtterance *utterance = [AVSpeechUtterance speechUtteranceWithString:@(text.c_str())];
        [utterance setVoice:[AVSpeechSynthesisVoice voiceWithIdentifier:@"com.apple.voice.compact.en-US.Samantha"]];

        if (@available(macOS 11.0, *)) {
            [utterance setPrefersAssistiveTechnologySettings:YES];
        }

        [(AVSpeechSynthesizer *)synthesizer stopSpeakingAtBoundary:AVSpeechBoundaryImmediate];
        [(AVSpeechSynthesizer *)synthesizer speakUtterance:utterance];
    }
}
