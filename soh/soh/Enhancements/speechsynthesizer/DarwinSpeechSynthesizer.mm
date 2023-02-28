//
//  DarwinSpeechSynthesizer.mm
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#include "DarwinSpeechSynthesizer.h"
#import <AVFoundation/AVFoundation.h>

DarwinSpeechSynthesizer::DarwinSpeechSynthesizer() {}

bool DarwinSpeechSynthesizer::DoInit() {
    mSynthesizer = [[AVSpeechSynthesizer alloc] init];
    return true;
}

void DarwinSpeechSynthesizer::DoUninitialize() {
    [(AVSpeechSynthesizer *)mSynthesizer stopSpeakingAtBoundary:AVSpeechBoundaryImmediate];
    mSynthesizer = nil;
}

void DarwinSpeechSynthesizer::Speak(const char* text, const char* language) {
    AVSpeechUtterance *utterance = [AVSpeechUtterance speechUtteranceWithString:@(text)];
    [utterance setVoice:[AVSpeechSynthesisVoice voiceWithLanguage:@(language)]];

    if (@available(macOS 11.0, *)) {
        [utterance setPrefersAssistiveTechnologySettings:YES];
    }

    [(AVSpeechSynthesizer *)mSynthesizer stopSpeakingAtBoundary:AVSpeechBoundaryImmediate];
    [(AVSpeechSynthesizer *)mSynthesizer speakUtterance:utterance];
}
