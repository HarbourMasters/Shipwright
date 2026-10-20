//
//  SpeechSynthesizer.cpp
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#include <libultraship/bridge/consolevariablebridge.h>

#include "SpeechSynthesizer.h"
#include "soh/cvar_prefixes.h"

SpeechSynthesizer::SpeechSynthesizer() : mInitialized(false){};

bool SpeechSynthesizer::Init(void) {
    if (mInitialized) {
        return true;
    }

    mInitialized = DoInit();
    if (mInitialized) {
        ApplySettings();
    }

    return mInitialized;
}

void SpeechSynthesizer::Uninitialize(void) {
    if (!mInitialized) {
        return;
    }

    DoUninitialize();
    mInitialized = false;
}

void SpeechSynthesizer::ApplySettings(void) {
    if (!mInitialized) {
        return;
    }

    DoApplySettings(CVarGetInteger(CVAR_SETTING("A11yTTSRate"), 100),
                    CVarGetInteger(CVAR_SETTING("A11yTTSVolume"), 100),
                    CVarGetInteger(CVAR_SETTING("A11yTTSPitch"), 50));
}

void SpeechSynthesizer::DoApplySettings(int32_t, int32_t, int32_t) {
}

bool SpeechSynthesizer::IsInitialized(void) {
    return mInitialized;
}
