#include "SpeechLogger.h"
#include <libultraship/libultraship.h>

SpeechLogger::SpeechLogger() {
}

void SpeechLogger::Speak(const char* text, const char* language) {
    lusprintf(__FILE__, __LINE__, 2, "Spoken Text (%s): %s", language, text);
}

bool SpeechLogger::DoInit() {
    return true;
}

void SpeechLogger::DoUninitialize() {
}
