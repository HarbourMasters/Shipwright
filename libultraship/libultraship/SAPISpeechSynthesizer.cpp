//
//  SAPISpeechSynthesizer.cpp
//  libultraship
//
//  Created by David Chavez on 10.10.22.
//

#include "SAPISpeechSynthesizer.h"
#include <sapi.h>
#include <thread>

ISpVoice * pVoice = NULL;

namespace Ship {
    bool SAPISpeechSynthesizer::Init() {
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
        HRESULT CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);
        CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
        return true;
    }

    void SpeakThreadTask(const std::string &text) {
        const int w = 512;
        int* wp = const_cast <int*> (&w);
        *wp = strlen(text.c_str());

        wchar_t wtext[w];
        mbstowcs(wtext, text.c_str(), strlen(text.c_str()) + 1);

        pVoice->Speak(wtext, SPF_IS_XML | SPF_ASYNC | SPF_PURGEBEFORESPEAK, NULL);
    }

    void SAPISpeechSynthesizer::Speak(std::string text) {
        std::thread t1(SpeakThreadTask, text);
        t1.detach();
    }
}
