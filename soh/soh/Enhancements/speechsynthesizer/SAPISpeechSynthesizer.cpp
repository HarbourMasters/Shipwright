//
//  SAPISpeechSynthesizer.cpp
//  libultraship
//
//  Created by David Chavez on 22.11.22.
//

#include "SAPISpeechSynthesizer.h"
#include <sapi.h>
#include <thread>
#include <string>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/xchar.h>

ISpVoice* ispVoice = NULL;

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

    auto speakText = fmt::format(
        L"<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xml:lang='{}'>{}</speak>", wLanguage, wText);
    ispVoice->Speak(speakText.c_str(), SPF_IS_XML | SPF_ASYNC | SPF_PURGEBEFORESPEAK, NULL);
}

void SAPISpeechSynthesizer::Speak(const char* text, const char* language) {
    // convert to string so char buffers don't have to be kept alive by caller
    std::string textStr(text);
    std::string languageStr(language);

    std::thread t1(SpeakThreadTask, textStr, languageStr);
    t1.detach();
}
