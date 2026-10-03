#include <algorithm>
#include <dlfcn.h>

#include <spdlog/spdlog.h>

#include "ESpeakSpeechSynthesizer.h"

ESpeakSpeechSynthesizer::ESpeakSpeechSynthesizer() {
}

bool ESpeakSpeechSynthesizer::DoInit() {
    void* espeak = dlopen("libespeak-ng.so", RTLD_LAZY | RTLD_LOCAL);
    if (espeak != NULL) {
        this->Initialize = (speak_Initialize)dlsym(espeak, "espeak_Initialize");
        this->SetVoiceByProperties = (speak_SetVoiceByProperties)dlsym(espeak, "espeak_SetVoiceByProperties");
        this->Synth = (speak_Synth)dlsym(espeak, "espeak_Synth");
        this->Cancel = (speak_Cancel)dlsym(espeak, "espeak_Cancel");
        this->SetParameter = (speak_SetParameter)dlsym(espeak, "espeak_SetParameter");
        this->Terminate = (speak_Terminate)dlsym(espeak, "espeak_Terminate");
        if (this->Initialize == NULL || this->SetVoiceByProperties == NULL || this->Synth == NULL ||
            this->Cancel == NULL || this->SetParameter == NULL || this->Terminate == NULL) {
            SPDLOG_INFO("Failed to load espeak-ng");
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
        SPDLOG_INFO("Spoken Text ({}): {}", language, text);
    } else {
        // Match the SAPI and AVSpeechSynthesizer backends, which both drop what they are saying when a new
        // utterance arrives. Without this, scrolling a menu queues one utterance per item with no way to skip.
        this->Cancel();

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

void ESpeakSpeechSynthesizer::DoApplySettings(int32_t rate, int32_t volume, int32_t pitch) {
    if (this->espeak == NULL) {
        return;
    }

    this->SetParameter(espeakRATE, std::clamp(espeakRATE_NORMAL * rate / 100, espeakRATE_MINIMUM, espeakRATE_MAXIMUM),
                       0);
    this->SetParameter(espeakVOLUME, volume, 0);
    this->SetParameter(espeakPITCH, pitch, 0);
}
