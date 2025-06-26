#include "soh/resource/importer/AudioSoundFontFactory.h"
#include "soh/resource/type/AudioSoundFont.h"
#include "soh/resource/logging/AudioSoundFontLogger.h"
#include "spdlog/spdlog.h"
#include "libultraship/libultraship.h"
#include <tinyxml2.h>

namespace SOH {
std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryAudioSoundFontV2::ReadResource(std::shared_ptr<Ship::File> file,
                                                    std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto audioSoundFont = std::make_shared<AudioSoundFont>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    audioSoundFont->soundFont.fntIndex = reader->ReadInt32();
    audioSoundFont->medium = reader->ReadInt8();
    audioSoundFont->cachePolicy = reader->ReadInt8();

    audioSoundFont->data1 = reader->ReadUInt16();
    audioSoundFont->soundFont.sampleBankId1 = audioSoundFont->data1 >> 8;
    audioSoundFont->soundFont.sampleBankId2 = audioSoundFont->data1 & 0xFF;

    audioSoundFont->data2 = reader->ReadUInt16();
    audioSoundFont->data3 = reader->ReadUInt16();

    uint32_t drumCount = reader->ReadUInt32();
    audioSoundFont->soundFont.numDrums = drumCount;

    uint32_t instrumentCount = reader->ReadUInt32();
    audioSoundFont->soundFont.numInstruments = instrumentCount;

    uint32_t soundEffectCount = reader->ReadUInt32();
    audioSoundFont->soundFont.numSfx = soundEffectCount;

    // 🥁 DRUMS 🥁
    audioSoundFont->drums.reserve(audioSoundFont->soundFont.numDrums);
    audioSoundFont->drumAddresses.reserve(audioSoundFont->soundFont.numDrums);
    for (uint32_t i = 0; i < audioSoundFont->soundFont.numDrums; i++) {
        Drum drum;
        drum.releaseRate = reader->ReadUByte();
        drum.pan = reader->ReadUByte();
        drum.loaded = reader->ReadUByte();
        drum.loaded = 0; // this was always getting set to zero in ResourceMgr_LoadAudioSoundFont

        uint32_t envelopeCount = reader->ReadUInt32();
        audioSoundFont->drumEnvelopeCounts.push_back(envelopeCount);
        std::vector<AdsrEnvelope> drumEnvelopes;
        drumEnvelopes.reserve(audioSoundFont->drumEnvelopeCounts[i]);
        for (uint32_t j = 0; j < audioSoundFont->drumEnvelopeCounts.back(); j++) {
            AdsrEnvelope env;

            int16_t delay = reader->ReadInt16();
            int16_t arg = reader->ReadInt16();

            env.delay = BE16SWAP(delay);
            env.arg = BE16SWAP(arg);

            drumEnvelopes.push_back(env);
        }
        audioSoundFont->drumEnvelopeArrays.push_back(drumEnvelopes);
        drum.envelope = audioSoundFont->drumEnvelopeArrays.back().data();

        bool hasSample = reader->ReadInt8();
        std::string sampleFileName = reader->ReadString();
        drum.sound.tuning = reader->ReadFloat();

        if (sampleFileName.empty()) {
            drum.sound.sample = nullptr;
        } else {
            auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            drum.sound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
        }
        audioSoundFont->drumFileNames.push_back(sampleFileName);

        audioSoundFont->drums.push_back(drum);
        audioSoundFont->drumAddresses.push_back(&audioSoundFont->drums.back());
    }
    audioSoundFont->soundFont.drums = audioSoundFont->drumAddresses.data();

    // 🎺🎻🎷🎸🎹 INSTRUMENTS 🎹🎸🎷🎻🎺
    audioSoundFont->instruments.reserve(audioSoundFont->soundFont.numInstruments);
    for (uint32_t i = 0; i < audioSoundFont->soundFont.numInstruments; i++) {
        Instrument instrument;

        uint8_t isValidEntry = reader->ReadUByte();
        instrument.loaded = reader->ReadUByte();
        instrument.loaded = 0; // this was always getting set to zero in ResourceMgr_LoadAudioSoundFont

        instrument.normalRangeLo = reader->ReadUByte();
        instrument.normalRangeHi = reader->ReadUByte();
        instrument.releaseRate = reader->ReadUByte();

        uint32_t envelopeCount = reader->ReadInt32();
        audioSoundFont->instrumentEnvelopeCounts.push_back(envelopeCount);
        std::vector<AdsrEnvelope> instrumentEnvelopes;
        for (uint32_t j = 0; j < audioSoundFont->instrumentEnvelopeCounts.back(); j++) {
            AdsrEnvelope env;

            int16_t delay = reader->ReadInt16();
            int16_t arg = reader->ReadInt16();

            env.delay = BE16SWAP(delay);
            env.arg = BE16SWAP(arg);

            instrumentEnvelopes.push_back(env);
        }
        audioSoundFont->instrumentEnvelopeArrays.push_back(instrumentEnvelopes);
        instrument.envelope = audioSoundFont->instrumentEnvelopeArrays.back().data();

        bool hasLowNoteSoundFontEntry = reader->ReadInt8();
        if (hasLowNoteSoundFontEntry) {
            bool hasSampleRef = reader->ReadInt8();
            std::string sampleFileName = reader->ReadString();
            instrument.lowNotesSound.tuning = reader->ReadFloat();
            auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            instrument.lowNotesSound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
            audioSoundFont->lowInstrumentFileNames.push_back(sampleFileName);
        } else {
            instrument.lowNotesSound.sample = nullptr;
            instrument.lowNotesSound.tuning = 0;
            audioSoundFont->lowInstrumentFileNames.push_back("");
        }

        bool hasNormalNoteSoundFontEntry = reader->ReadInt8();
        if (hasNormalNoteSoundFontEntry) {
            bool hasSampleRef = reader->ReadInt8();
            std::string sampleFileName = reader->ReadString();
            instrument.normalNotesSound.tuning = reader->ReadFloat();
            auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            instrument.normalNotesSound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
            audioSoundFont->normalInstrumentFileNames.push_back(sampleFileName);
        } else {
            instrument.normalNotesSound.sample = nullptr;
            instrument.normalNotesSound.tuning = 0;
            audioSoundFont->normalInstrumentFileNames.push_back("");
        }

        bool hasHighNoteSoundFontEntry = reader->ReadInt8();
        if (hasHighNoteSoundFontEntry) {
            bool hasSampleRef = reader->ReadInt8();
            std::string sampleFileName = reader->ReadString();
            instrument.highNotesSound.tuning = reader->ReadFloat();
            auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            instrument.highNotesSound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
            audioSoundFont->highInstrumentFileNames.push_back(sampleFileName);
        } else {
            instrument.highNotesSound.sample = nullptr;
            instrument.highNotesSound.tuning = 0;
            audioSoundFont->highInstrumentFileNames.push_back("");
        }

        audioSoundFont->instruments.push_back(instrument);
        audioSoundFont->instrumentAddresses.push_back(isValidEntry ? &audioSoundFont->instruments.back() : nullptr);
    }
    audioSoundFont->soundFont.instruments = audioSoundFont->instrumentAddresses.data();

    // 🔊 SOUND EFFECTS 🔊
    audioSoundFont->soundEffects.reserve(audioSoundFont->soundFont.numSfx);
    for (uint32_t i = 0; i < audioSoundFont->soundFont.numSfx; i++) {
        SoundFontSound soundEffect;

        bool hasSFEntry = reader->ReadInt8();
        if (hasSFEntry) {
            bool hasSampleRef = reader->ReadInt8();
            std::string sampleFileName = reader->ReadString();
            soundEffect.tuning = reader->ReadFloat();
            auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            soundEffect.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
            audioSoundFont->soundEffectFileNames.push_back(sampleFileName);
        }

        audioSoundFont->soundEffects.push_back(soundEffect);
    }
    audioSoundFont->soundFont.soundEffects = audioSoundFont->soundEffects.data();

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogAudioSoundFontAsXML(initData, audioSoundFont);
    }

    return audioSoundFont;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryXMLAudioSoundFontV2::ReadResource(std::shared_ptr<Ship::File> file,
                                                 std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    std::shared_ptr<SOH::AudioSoundFont> audioSoundFont = std::make_shared<AudioSoundFont>(initData);
    std::shared_ptr<tinyxml2::XMLDocument> reader = std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader);

    tinyxml2::XMLElement* root = reader->RootElement();

    if (root->Name() != "SoundFont") {
        LUSLOG_ERROR("Tried to load malformed sound font");
        assert(false);
        return nullptr;
    }

    audioSoundFont->soundFont.fntIndex = root->IntAttribute("FntIndex");
    audioSoundFont->medium = root->IntAttribute("Medium");
    audioSoundFont->cachePolicy = root->IntAttribute("CachePolicy");

    audioSoundFont->soundFont.sampleBankId1 = root->IntAttribute("SampleBankId1");
    audioSoundFont->soundFont.sampleBankId2 = root->IntAttribute("SampleBankId2");
    audioSoundFont->data1 = (audioSoundFont->soundFont.sampleBankId1 << 8) & audioSoundFont->soundFont.sampleBankId2;

    audioSoundFont->data2 = root->IntAttribute("Data2");
    audioSoundFont->data3 = root->IntAttribute("Data3");

    tinyxml2::XMLElement* rootChild = root->FirstChildElement();

    while (rootChild != nullptr) {
        if (rootChild->Name() == "Drum") { // 🥁 DRUMS 🥁
            Drum drum;
            drum.releaseRate = rootChild->IntAttribute("ReleaseRate");
            drum.pan = rootChild->IntAttribute("Pan");

            // this was always getting set to zero in ResourceMgr_LoadAudioSoundFont
            // drum.loaded = rootChild->IntAttribute("Loaded");
            drum.loaded = 0;

            std::vector<AdsrEnvelope> drumEnvelopes;
            tinyxml2::XMLElement* drumChild = rootChild->FirstChildElement();

            while (drumChild != nullptr) {
                if (drumChild->Name() != "Envelope") {
                    LUSLOG_ERROR("Tried to load malformed sound font drum envelope");
                    assert(false);
                    return nullptr;
                }

                AdsrEnvelope env;

                int16_t delay = drumChild->IntAttribute("Delay");
                int16_t arg = drumChild->IntAttribute("Arg");

                env.delay = BE16SWAP(delay);
                env.arg = BE16SWAP(arg);

                drumEnvelopes.push_back(env);

                drumChild = drumChild->NextSiblingElement();
            }

            audioSoundFont->drumEnvelopeArrays.push_back(drumEnvelopes);
            drum.envelope = audioSoundFont->drumEnvelopeArrays.back().data();
            audioSoundFont->drumEnvelopeCounts.push_back(drumEnvelopes.size());

            std::string sampleFileName = std::string(rootChild->Attribute("SampleFileName"));
            drum.sound.tuning = rootChild->FloatAttribute("Tuning");

            if (sampleFileName.empty()) {
                drum.sound.sample = nullptr;
            } else {
                std::shared_ptr<Ship::IResource> res =
                    Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
                drum.sound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
            }
            audioSoundFont->drumFileNames.push_back(sampleFileName);

            audioSoundFont->drums.push_back(drum);
            audioSoundFont->drumAddresses.push_back(&audioSoundFont->drums.back());
        } else if (rootChild->Name() == "Instrument") { // 🎺🎻🎷🎸🎹 INSTRUMENTS 🎹🎸🎷🎻🎺
            Instrument instrument;

            uint8_t isValidEntry = rootChild->IntAttribute("IsValidEntry");
            // this was always getting set to zero in ResourceMgr_LoadAudioSoundFont
            // instrument.loaded = rootChild->IntAttribute("Loaded");
            instrument.loaded = 0;

            instrument.normalRangeLo = rootChild->IntAttribute("NormalRangeLo");
            instrument.normalRangeHi = rootChild->IntAttribute("NormalRangeHi");
            instrument.releaseRate = rootChild->IntAttribute("ReleaseRate");

            std::vector<AdsrEnvelope> instrumentEnvelopes;
            tinyxml2::XMLElement* instrumentChild = rootChild->FirstChildElement();

            while (instrumentChild != nullptr) {
                if (instrumentChild->Name() != "Envelope") {
                    LUSLOG_ERROR("Tried to load malformed sound font drum envelope");
                    assert(false);
                    return nullptr;
                }

                AdsrEnvelope env;

                int16_t delay = instrumentChild->IntAttribute("Delay");
                int16_t arg = instrumentChild->IntAttribute("Arg");

                env.delay = BE16SWAP(delay);
                env.arg = BE16SWAP(arg);

                instrumentEnvelopes.push_back(env);

                instrumentChild = instrumentChild->NextSiblingElement();
            }

            audioSoundFont->instrumentEnvelopeCounts.push_back(instrumentEnvelopes.size());
            audioSoundFont->instrumentEnvelopeArrays.push_back(instrumentEnvelopes);
            instrument.envelope = audioSoundFont->instrumentEnvelopeArrays.back().data();

            const char* lowNoteSampleFileName = rootChild->Attribute("LowNoteSampleFileName");
            if (lowNoteSampleFileName != nullptr) {
                instrument.lowNotesSound.tuning = rootChild->FloatAttribute("Tuning");
                std::string sampleFileName = std::string(lowNoteSampleFileName);
                std::shared_ptr<Ship::IResource> res =
                    Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
                instrument.lowNotesSound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
                audioSoundFont->lowInstrumentFileNames.push_back(sampleFileName);
            } else {
                instrument.lowNotesSound.tuning = 0;
                instrument.lowNotesSound.sample = nullptr;
                audioSoundFont->lowInstrumentFileNames.push_back("");
            }

            const char* normalNoteSampleFileName = rootChild->Attribute("NormalNoteSampleFileName");
            if (normalNoteSampleFileName != nullptr) {
                instrument.normalNotesSound.tuning = rootChild->FloatAttribute("Tuning");
                std::string sampleFileName = std::string(normalNoteSampleFileName);
                std::shared_ptr<Ship::IResource> res =
                    Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
                instrument.normalNotesSound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
                audioSoundFont->normalInstrumentFileNames.push_back(sampleFileName);
            } else {
                instrument.normalNotesSound.tuning = 0;
                instrument.normalNotesSound.sample = nullptr;
                audioSoundFont->normalInstrumentFileNames.push_back("");
            }

            const char* highNoteSampleFileName = rootChild->Attribute("HighNoteSampleFileName");
            if (normalNoteSampleFileName != nullptr) {
                instrument.highNotesSound.tuning = rootChild->FloatAttribute("Tuning");
                std::string sampleFileName = std::string(normalNoteSampleFileName);
                std::shared_ptr<Ship::IResource> res =
                    Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
                instrument.highNotesSound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
                audioSoundFont->highInstrumentFileNames.push_back(sampleFileName);
            } else {
                instrument.highNotesSound.tuning = 0;
                instrument.highNotesSound.sample = nullptr;
                audioSoundFont->highInstrumentFileNames.push_back("");
            }

            audioSoundFont->instruments.push_back(instrument);
            audioSoundFont->instrumentAddresses.push_back(isValidEntry ? &audioSoundFont->instruments.back() : nullptr);
        } else if (rootChild->Name() == "Sfx") { // 🔊 SOUND EFFECTS 🔊
            SoundFontSound soundEffect;

            soundEffect.tuning = rootChild->FloatAttribute("Tuning");
            std::string sampleFileName = std::string(rootChild->Attribute("SampleFileName"));
            std::shared_ptr<Ship::IResource> res =
                Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            soundEffect.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
            audioSoundFont->soundEffectFileNames.push_back(sampleFileName);

            audioSoundFont->soundEffects.push_back(soundEffect);
        } else {
            LUSLOG_ERROR("Tried to load sound font element of unknown type: %s (valid types are \"Drum\", "
                         "\"Instrument\" & \"Sfx\")",
                         rootChild->Name());
            assert(false);
            return nullptr;
        }

        rootChild = rootChild->NextSiblingElement();
    }

    audioSoundFont->soundFont.numDrums = audioSoundFont->drumAddresses.size();
    audioSoundFont->soundFont.drums = audioSoundFont->drumAddresses.data();

    audioSoundFont->soundFont.numInstruments = audioSoundFont->instrumentAddresses.size();
    audioSoundFont->soundFont.instruments = audioSoundFont->instrumentAddresses.data();

    audioSoundFont->soundFont.numSfx = audioSoundFont->soundEffects.size();
    audioSoundFont->soundFont.soundEffects = audioSoundFont->soundEffects.data();

    return audioSoundFont;
}
} // namespace SOH
