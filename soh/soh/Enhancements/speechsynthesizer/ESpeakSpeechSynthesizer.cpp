#include "ESpeakSpeechSynthesizer.h"
#include <dlfcn.h>
#include <libultraship/libultraship.h>

ESpeakSpeechSynthesizer::ESpeakSpeechSynthesizer() {
}

bool ESpeakSpeechSynthesizer::DoInit() {
    void* espeak = dlopen("libespeak-ng.so", RTLD_LAZY | RTLD_LOCAL);
    if (espeak != NULL) {
        this->Initialize = (speak_Initialize)dlsym(espeak, "espeak_Initialize");
        this->SetVoiceByProperties = (speak_SetVoiceByProperties)dlsym(espeak, "espeak_SetVoiceByProperties");
        this->Synth = (speak_Synth)dlsym(espeak, "espeak_Synth");
        this->Terminate = (speak_Terminate)dlsym(espeak, "espeak_Terminate");
        if (this->Initialize == NULL || this->SetVoiceByProperties == NULL || this->Synth == NULL ||
            this->Terminate == NULL) {
            lusprintf(__FILE__, __LINE__, 2, "Failed to load espeak-ng");
            dlclose(espeak);
            return false;
        } else {
            this->espeak = espeak;
            return this->Initialize(AUDIO_OUTPUT_PLAYBACK, 100, NULL, 0) != -1;
        }
    }
    return true;
}

void ESpeakSpeechSynthesizer::DoUninitialize() {
    if (this->espeak != NULL) {
        this->Terminate();
        dlclose(this->espeak);
        this->espeak = NULL;
    }
}

void ESpeakSpeechSynthesizer::Speak(const char* text, const char* language) {
    if (this->espeak == NULL) {
        lusprintf(__FILE__, __LINE__, 2, "Spoken Text (%s): %s", language, text);
    } else {
        if (language != this->mLanguage) {
            espeak_VOICE voice = { .languages = language };
            if (this->SetVoiceByProperties(&voice)) {
                return;
            }
            this->mLanguage = language;
        }
        this->Synth(text, 100, 0, POS_CHARACTER, 0, espeakCHARS_UTF8, NULL, NULL);
    }
}
