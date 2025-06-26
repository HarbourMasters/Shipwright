#include "soh/resource/type/AudioSoundFont.h"
#include "spdlog/spdlog.h"
#include <tinyxml2.h>

namespace SOH {
void LogAudioSoundFontAsXML(std::shared_ptr<Ship::ResourceInitData> initData,
                            std::shared_ptr<SOH::AudioSoundFont> audioSoundFont) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SoundFont");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < audioSoundFont->soundFont.numDrums; i += 1) {
        tinyxml2::XMLElement* drumElement = doc.NewElement("Drum");
        Drum* drum = audioSoundFont->soundFont.drums[i];

        drumElement->SetAttribute("ReleaseRate", drum->releaseRate);
        drumElement->SetAttribute("Pan", drum->pan);
        drumElement->SetAttribute("Loaded", drum->loaded);
        drumElement->SetAttribute("Tuning", drum->sound.tuning);

        if (drum->sound.sample != nullptr) {
            drumElement->SetAttribute("SampleFileName", audioSoundFont->drumFileNames[i].c_str());
        }

        std::vector<AdsrEnvelope> envelopes = audioSoundFont->drumEnvelopeArrays[i];

        for (AdsrEnvelope envelope : envelopes) {
            tinyxml2::XMLElement* drumEnvelopeElement = doc.NewElement("Envelope");

            drumEnvelopeElement->SetAttribute("Delay", BE16SWAP(envelope.delay));
            drumEnvelopeElement->SetAttribute("Arg", BE16SWAP(envelope.arg));

            drumElement->InsertEndChild(drumEnvelopeElement);
        }

        root->InsertEndChild(drumElement);
    }

    for (size_t i = 0; i < audioSoundFont->soundFont.numInstruments; i += 1) {
        tinyxml2::XMLElement* instrumentElement = doc.NewElement("Instrument");
        Instrument* instrument = audioSoundFont->soundFont.instruments[i];

        if (instrument == nullptr) {
            SPDLOG_INFO("[LogAudioSoundFontAsXML]: Instrument was nullptr (i={})", i);
            instrumentElement->SetAttribute("IsValidEntry", 0);
            root->InsertEndChild(instrumentElement);
            continue;
        }

        instrumentElement->SetAttribute("IsValidEntry", "1");
        instrumentElement->SetAttribute("Loaded", instrument->loaded);
        instrumentElement->SetAttribute("NormalRangeLo", instrument->normalRangeLo);
        instrumentElement->SetAttribute("NormalRangeHi", instrument->normalRangeHi);
        instrumentElement->SetAttribute("ReleaseRate", instrument->releaseRate);

        std::vector<AdsrEnvelope> envelopes = audioSoundFont->instrumentEnvelopeArrays[i];

        for (AdsrEnvelope envelope : envelopes) {
            tinyxml2::XMLElement* instrumentEnvelopeElement = doc.NewElement("Envelope");

            instrumentEnvelopeElement->SetAttribute("Delay", BE16SWAP(envelope.delay));
            instrumentEnvelopeElement->SetAttribute("Arg", BE16SWAP(envelope.arg));

            instrumentElement->InsertEndChild(instrumentEnvelopeElement);
        }

        if (instrument->lowNotesSound.sample != nullptr) {
            instrumentElement->SetAttribute("LowNoteSampleFileName", audioSoundFont->lowInstrumentFileNames[i].c_str());
        }

        if (instrument->lowNotesSound.sample != nullptr) {
            instrumentElement->SetAttribute("NormalNoteSampleFileName",
                                            audioSoundFont->normalInstrumentFileNames[i].c_str());
        }

        if (instrument->lowNotesSound.sample != nullptr) {
            instrumentElement->SetAttribute("HighNoteSampleFileName",
                                            audioSoundFont->highInstrumentFileNames[i].c_str());
        }

        root->InsertEndChild(instrumentElement);
    }

    for (size_t i = 0; i < audioSoundFont->soundFont.numSfx; i += 1) {
        tinyxml2::XMLElement* sfxElement = doc.NewElement("Sfx");
        SoundFontSound sfx = audioSoundFont->soundFont.soundEffects[i];

        sfxElement->SetAttribute("Tuning", sfx.tuning);

        sfxElement->SetAttribute("SampleFileName", audioSoundFont->soundEffectFileNames[i].c_str());

        root->InsertEndChild(sfxElement);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", initData->Path, printer.CStr());
}
} // namespace SOH