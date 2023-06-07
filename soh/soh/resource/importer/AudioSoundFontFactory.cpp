#include "soh/resource/importer/AudioSoundFontFactory.h"
#include "soh/resource/type/AudioSoundFont.h"
#include "spdlog/spdlog.h"
#include "libultraship/libultraship.h"
#include "soh/OTRGlobals.h"

std::string ResourceMgr_BaseName(std::string path) {
    std::string result = path;
    if (path.empty()) {
        return path;
    }
    while (path[result.length() - 1] == '/') { //clear trailing slashes
        result = result.substr(result.length() - 1);
        if(result.length() <= 0) {
            return result;
        }
    }
    u32 len = result.length();

    for (u32 i = 0; i < len; i++) {
        if (path[i] == '/') {
            result = path.substr(i + 1);
        }
    }

    return result;
}

namespace LUS {
std::shared_ptr<IResource>
AudioSoundFontFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<AudioSoundFont>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 2:
	factory = std::make_shared<AudioSoundFontFactoryV0>();
	break;
    }

    if (factory == nullptr)
    {
        SPDLOG_ERROR("Failed to load AudioSoundFont with version {}", resource->GetInitData()->ResourceVersion);
        return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::AudioSoundFontFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                    std::shared_ptr<IResource> resource) {
    std::shared_ptr<AudioSoundFont> audioSoundFont = std::static_pointer_cast<AudioSoundFont>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, audioSoundFont);

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

    auto customSampleList = *LUS::Context::GetInstance()->GetResourceManager()->GetArchive()->ListFiles("custom/samples/*").get();

    std::string sampleFileNameToLoad;

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
        std::string sampleBaseName = ResourceMgr_BaseName(sampleFileName);
        drum.sound.tuning = reader->ReadFloat();

        if (sampleFileName.empty()) {
            drum.sound.sample = nullptr;
        } else {
            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/wav/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/mp3/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/z64/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            drum.sound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
        }

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

            std::string sampleBaseName = ResourceMgr_BaseName(sampleFileName);

            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/wav/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/mp3/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/z64/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            instrument.lowNotesSound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
        } else {
            instrument.lowNotesSound.sample = nullptr;
            instrument.lowNotesSound.tuning = 0;
        }

        bool hasNormalNoteSoundFontEntry = reader->ReadInt8();
        if (hasNormalNoteSoundFontEntry) {
            bool hasSampleRef = reader->ReadInt8();
            std::string sampleFileName = reader->ReadString();
            instrument.normalNotesSound.tuning = reader->ReadFloat();

            std::string sampleBaseName = ResourceMgr_BaseName(sampleFileName);

            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/wav/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/mp3/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/z64/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            instrument.normalNotesSound.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
        } else {
            instrument.normalNotesSound.sample = nullptr;
            instrument.normalNotesSound.tuning = 0;
        }

        bool hasHighNoteSoundFontEntry = reader->ReadInt8();
        if (hasHighNoteSoundFontEntry) {
            bool hasSampleRef = reader->ReadInt8();
            std::string sampleFileName = reader->ReadString();
            instrument.highNotesSound.tuning = reader->ReadFloat();

            std::string sampleBaseName = ResourceMgr_BaseName(sampleFileName);

            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/wav/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/mp3/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/z64/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            instrument.highNotesSound.sample = static_cast<Sample*>(res ? res->GetPointer() : nullptr);
        } else {
            instrument.highNotesSound.sample = nullptr;
            instrument.highNotesSound.tuning = 0;
        }

        
        audioSoundFont->instruments.push_back(instrument);
        audioSoundFont->instrumentAddresses.push_back(isValidEntry ? 
                                                      &audioSoundFont->instruments.back() :
                                                      nullptr);
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
            std::string sampleBaseName = ResourceMgr_BaseName(sampleFileName);
            soundEffect.tuning = reader->ReadFloat();
            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/wav/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/mp3/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess("custom/samples/z64/" + sampleBaseName);
            res = res ? res : LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(sampleFileName.c_str());
            soundEffect.sample = static_cast<Sample*>(res ? res->GetRawPointer() : nullptr);
        }
        
        audioSoundFont->soundEffects.push_back(soundEffect);
    }
    audioSoundFont->soundFont.soundEffects = audioSoundFont->soundEffects.data();
}
} // namespace LUS
