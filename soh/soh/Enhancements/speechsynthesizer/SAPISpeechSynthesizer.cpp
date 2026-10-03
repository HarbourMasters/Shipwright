//
//  SAPISpeechSynthesizer.cpp
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#include "SAPISpeechSynthesizer.h"
#include <sapi.h>
#include <algorithm>
#include <cmath>
#include <thread>
#include <string>
#include <spdlog/common.h>
#include <spdlog/fmt/xchar.h>
#include <atomic>

ISpVoice* ispVoice = NULL;
// Read by the detached speaking thread; SAPI carries pitch as markup rather than a voice property.
std::atomic<int32_t> sPitchAbsMiddle = 0;

SAPISpeechSynthesizer::SAPISpeechSynthesizer() {
}

bool SAPISpeechSynthesizer::DoInit() {
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    HRESULT CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);
    CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&ispVoice);
    return true;
}

void SAPISpeechSynthesizer::DoUninitialize() {
    ispVoice->Release();
    ispVoice = NULL;
    CoUninitialize();
}

std::wstring CharToWideString(std::string text) {
    int textSize = MultiByteToWideChar(CP_UTF8, 0, &text[0], (int)text.size(), NULL, 0);
    std::wstring wstrTo(textSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, &text[0], (int)text.size(), &wstrTo[0], textSize);
    return wstrTo;
}

void SpeakThreadTask(std::string text, std::string language) {
    auto wText = CharToWideString(text);
    auto wLanguage = CharToWideString(language);

    auto speakText = spdlog::fmt_lib::format(
        L"<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xml:lang='{}'><pitch absmiddle='{}'/>{}</"
        L"speak>",
        wLanguage, sPitchAbsMiddle.load(), wText);
    ispVoice->Speak(speakText.c_str(), SPF_IS_XML | SPF_ASYNC | SPF_PURGEBEFORESPEAK, NULL);
}

void SAPISpeechSynthesizer::Speak(const char* text, const char* language) {
    // convert to string so char buffers don't have to be kept alive by caller
    std::string textStr(text);
    std::string languageStr(language);

    std::thread t1(SpeakThreadTask, textStr, languageStr);
    t1.detach();
}

void SAPISpeechSynthesizer::DoApplySettings(int32_t rate, int32_t volume, int32_t pitch) {
    if (ispVoice == NULL) {
        return;
    }

    // SAPI's rate is a -10..10 log scale where each end is a third of / triple the normal speed.
    const double ratio = std::clamp(rate, 1, 1000) / 100.0;
    ispVoice->SetRate(std::clamp((int32_t)std::lround(10.0 * std::log(ratio) / std::log(3.0)), -10, 10));
    ispVoice->SetVolume((USHORT)std::clamp(volume, 0, 100));
    sPitchAbsMiddle = std::clamp((pitch - 50) / 5, -10, 10);
}
