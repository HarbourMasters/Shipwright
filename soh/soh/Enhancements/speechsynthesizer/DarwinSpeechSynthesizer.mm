//
//  DarwinSpeechSynthesizer.mm
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#include "DarwinSpeechSynthesizer.h"
#import <AVFoundation/AVFoundation.h>
#include <algorithm>

DarwinSpeechSynthesizer::DarwinSpeechSynthesizer() {}

bool DarwinSpeechSynthesizer::DoInit() {
    mSynthesizer = (__bridge_retained void*)[[AVSpeechSynthesizer alloc] init];
    return true;
}

void DarwinSpeechSynthesizer::DoUninitialize() {
    [(__bridge AVSpeechSynthesizer *)mSynthesizer stopSpeakingAtBoundary:AVSpeechBoundaryImmediate];
    mSynthesizer = nil;
}

void DarwinSpeechSynthesizer::Speak(const char* text, const char* language) {
    AVSpeechUtterance *utterance = [AVSpeechUtterance speechUtteranceWithString:@(text)];
    [utterance setVoice:[AVSpeechSynthesisVoice voiceWithLanguage:@(language)]];

    if (@available(macOS 11.0, *)) {
        [utterance setPrefersAssistiveTechnologySettings:YES];
    }

    // Leaving rate alone at the default lets the system's assistive technology settings pick it instead.
    if (mRate != 100) {
        // AVSpeechUtterance puts normal speech at the middle of its range, so scale each half separately -
        // a single multiplier would saturate well before the slider runs out.
        const float rate =
            mRate <= 100
                ? AVSpeechUtteranceDefaultSpeechRate * mRate / 100.0f
                : AVSpeechUtteranceDefaultSpeechRate +
                      (AVSpeechUtteranceMaximumSpeechRate - AVSpeechUtteranceDefaultSpeechRate) * (mRate - 100) /
                          (float)(SPEECH_RATE_MAX - 100);
        [utterance setRate:std::clamp(rate, AVSpeechUtteranceMinimumSpeechRate, AVSpeechUtteranceMaximumSpeechRate)];
    }
    [utterance setVolume:std::clamp(mVolume, 0, 100) / 100.0f];
    [utterance setPitchMultiplier:(mPitch <= 50 ? 0.5f + mPitch / 100.0f : mPitch / 50.0f)];

    [(__bridge AVSpeechSynthesizer *)mSynthesizer stopSpeakingAtBoundary:AVSpeechBoundaryImmediate];
    [(__bridge AVSpeechSynthesizer *)mSynthesizer speakUtterance:utterance];
}

void DarwinSpeechSynthesizer::DoApplySettings(int32_t rate, int32_t volume, int32_t pitch) {
    mRate = rate;
    mVolume = volume;
    mPitch = std::clamp(pitch, 0, 100);
}
