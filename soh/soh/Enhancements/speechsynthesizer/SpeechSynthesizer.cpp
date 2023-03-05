//
//  SpeechSynthesizer.cpp
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#include "SpeechSynthesizer.h"

SpeechSynthesizer::SpeechSynthesizer() : mInitialized(false){};

bool SpeechSynthesizer::Init(void) {
    if (mInitialized) {
        return true;
    }

    mInitialized = DoInit();
    return mInitialized;
}

void SpeechSynthesizer::Uninitialize(void) {
    if (!mInitialized) {
        return;
    }

    DoUninitialize();
    mInitialized = false;
}

bool SpeechSynthesizer::IsInitialized(void) {
    return mInitialized;
}
