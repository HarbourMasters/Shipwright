#pragma once

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
}

class ESpeakSpeechSynthesizer : public SpeechSynthesizer {
  public:
    ESpeakSpeechSynthesizer();

    void Speak(const char* text, const char* language);

  protected:
    bool DoInit(void);
    void DoUninitialize(void);

  private:
    const char* mLanguage = NULL;
    void* espeak = NULL;
    speak_Initialize Initialize = NULL;
    speak_SetVoiceByProperties SetVoiceByProperties = NULL;
    speak_Synth Synth = NULL;
    speak_Terminate Terminate = NULL;
};
