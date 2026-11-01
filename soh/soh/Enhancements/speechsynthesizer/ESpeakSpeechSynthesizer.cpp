#include <dlfcn.h>

#include <spdlog/spdlog.h>

#include "ESpeakSpeechSynthesizer.h"
#include "soh/Enhancements/audio/SpeechPlayer.h"

namespace {
// espeak's synth callback takes no user data, so what it fills has to be reachable
// without an instance. Only the worker thread touches these.
std::vector<short> gUtterance;
std::atomic<uint32_t>* gGeneration = nullptr;
uint32_t gSpeaking = 0;
} // namespace

ESpeakSpeechSynthesizer::ESpeakSpeechSynthesizer() {
}

int ESpeakSpeechSynthesizer::Collect(short* wav, int samples, espeak_EVENT* events) {
    // Non-zero abandons the rest of the utterance, which is how a new one cuts in.
    if (gGeneration != nullptr && gGeneration->load(std::memory_order_relaxed) != gSpeaking) {
        return 1;
    }
    if (wav != nullptr && samples > 0) {
        gUtterance.insert(gUtterance.end(), wav, wav + samples);
    }
    return 0;
}

bool ESpeakSpeechSynthesizer::DoInit() {
    void* espeak = dlopen("libespeak-ng.so", RTLD_LAZY | RTLD_LOCAL);
    if (espeak == nullptr) {
        return true;
    }

    const auto initialize = (speak_Initialize)dlsym(espeak, "espeak_Initialize");
    const auto setCallback = (speak_SetSynthCallback)dlsym(espeak, "espeak_SetSynthCallback");
    this->SetVoiceByProperties = (speak_SetVoiceByProperties)dlsym(espeak, "espeak_SetVoiceByProperties");
    this->Synth = (speak_Synth)dlsym(espeak, "espeak_Synth");
    this->Terminate = (speak_Terminate)dlsym(espeak, "espeak_Terminate");
    if (initialize == nullptr || setCallback == nullptr || this->SetVoiceByProperties == nullptr ||
        this->Synth == nullptr || this->Terminate == nullptr) {
        SPDLOG_INFO("Failed to load espeak-ng");
        dlclose(espeak);
        return false;
    }

    // Synchronous rather than playback: espeak hands the samples back instead of
    // opening an audio device of its own, which is what lets the game decide where
    // speech goes and at what volume.
    this->mSampleRate = initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, nullptr, 0);
    if (this->mSampleRate < 0) {
        SPDLOG_INFO("Failed to initialize espeak-ng");
        dlclose(espeak);
        return false;
    }

    this->espeak = espeak;
    gGeneration = &this->mGeneration;
    setCallback(&ESpeakSpeechSynthesizer::Collect);
    this->mWorker = std::thread(&ESpeakSpeechSynthesizer::Work, this);
    return true;
}

void ESpeakSpeechSynthesizer::DoUninitialize() {
    if (this->espeak == nullptr) {
        return;
    }

    {
        std::lock_guard<std::mutex> guard(this->mQueue);
        this->mQuit = true;
    }
    this->mGeneration.fetch_add(1, std::memory_order_relaxed);
    this->mWake.notify_one();
    this->mWorker.join();

    SOH::SpeechPlayer::Instance().Stop();
    SOH::SpeechPlayer::Instance().SetSeparateOutput(false, "");
    this->Terminate();
    dlclose(this->espeak);
    this->espeak = nullptr;
    gGeneration = nullptr;
}

void ESpeakSpeechSynthesizer::Speak(const char* text, const char* language) {
    if (this->espeak == nullptr) {
        SPDLOG_INFO("Spoken Text ({}): {}", language, text);
        return;
    }

    // Match the SAPI and AVSpeechSynthesizer backends, which both drop what they are saying when a new
    // utterance arrives. Without this, scrolling a menu queues one utterance per item with no way to skip.
    this->mGeneration.fetch_add(1, std::memory_order_relaxed);
    SOH::SpeechPlayer::Instance().Stop();

    {
        std::lock_guard<std::mutex> guard(this->mQueue);
        this->mText = text;
        this->mLanguage = language;
        this->mHasJob = true;
    }
    this->mWake.notify_one();
}

void ESpeakSpeechSynthesizer::Work() {
    for (;;) {
        std::string text;
        std::string language;
        {
            std::unique_lock<std::mutex> guard(this->mQueue);
            this->mWake.wait(guard, [this]() { return this->mHasJob || this->mQuit; });
            if (this->mQuit) {
                return;
            }
            text = std::move(this->mText);
            language = std::move(this->mLanguage);
            this->mHasJob = false;
        }

        gUtterance.clear();
        gSpeaking = this->mGeneration.load(std::memory_order_relaxed);

        espeak_VOICE voice = { .languages = language.c_str() };
        if (this->SetVoiceByProperties(&voice) != EE_OK) {
            continue;
        }
        this->Synth(text.c_str(), text.size() + 1, 0, POS_CHARACTER, 0, espeakCHARS_UTF8, nullptr, nullptr);

        if (gSpeaking == this->mGeneration.load(std::memory_order_relaxed)) {
            SOH::SpeechPlayer::Instance().Play(std::move(gUtterance), this->mSampleRate);
        }
    }
}
