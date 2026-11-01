#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "SpeechSynthesizer.h"

extern "C" {
#include <espeak-ng/speak_lib.h>

// C23 typeof could help here
typedef ESPEAK_API int (*speak_Initialize)(espeak_AUDIO_OUTPUT output, int buflength, const char* path, int options);
typedef ESPEAK_API espeak_ERROR (*speak_Terminate)(void);
typedef ESPEAK_API espeak_ERROR (*speak_SetVoiceByProperties)(espeak_VOICE* voice_spec);
typedef ESPEAK_API espeak_ERROR (*speak_Synth)(const void* text, size_t size, unsigned int position,
                                               espeak_POSITION_TYPE position_type, unsigned int end_position,
                                               unsigned int flags, unsigned int* unique_identifier, void* user_data);
typedef ESPEAK_API void (*speak_SetSynthCallback)(t_espeak_callback* callback);
}

class ESpeakSpeechSynthesizer : public SpeechSynthesizer {
  public:
    ESpeakSpeechSynthesizer();

    void Speak(const char* text, const char* language);

  protected:
    bool DoInit(void);
    void DoUninitialize(void);

  private:
    // espeak synthesises on the thread that asks for the samples and only returns once
    // the whole utterance is done, which is far too long to hold the game thread.
    void Work();
    // What espeak hands the samples to, on the worker thread.
    static int Collect(short* wav, int samples, espeak_EVENT* events);

    void* espeak = nullptr;
    int mSampleRate = 0;
    speak_SetVoiceByProperties SetVoiceByProperties = nullptr;
    speak_Synth Synth = nullptr;
    speak_Terminate Terminate = nullptr;

    std::thread mWorker;
    std::mutex mQueue;
    std::condition_variable mWake;
    std::string mText;
    std::string mLanguage;
    bool mHasJob = false;
    bool mQuit = false;
    // Bumped by every new utterance, so one already being synthesised can see that
    // nobody is waiting for it any more and stop.
    std::atomic<uint32_t> mGeneration{ 0 };
};
