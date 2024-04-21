#ifndef SOHSpeechLogger_h
#define SOHSpeechLogger_h

#include "SpeechSynthesizer.h"

class SpeechLogger : public SpeechSynthesizer {
  public:
    SpeechLogger();

    void Speak(const char* text, const char* language);

  protected:
    bool DoInit(void);
    void DoUninitialize(void);
};

#endif
