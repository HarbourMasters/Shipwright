#include <spdlog/spdlog.h>

#include "SpeechLogger.h"

SpeechLogger::SpeechLogger() {
}

void SpeechLogger::Speak(const char* text, const char* language) {
    SPDLOG_INFO("Spoken Text ({}): {}", language, text);
}

bool SpeechLogger::DoInit() {
    return true;
}

void SpeechLogger::DoUninitialize() {
}
