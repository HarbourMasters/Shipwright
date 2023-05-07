#include "soh/resource/importer/AudioSoundFontFactory.h"
#include "soh/resource/type/AudioSoundFont.h"
#include "spdlog/spdlog.h"
#include "libultraship/bridge.h"

namespace Ship {
std::shared_ptr<Resource> AudioSoundFontFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                              std::shared_ptr<ResourceInitData> initData,
                                                              std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<AudioSoundFont>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 2:
	factory = std::make_shared<AudioSoundFontFactoryV0>();
	break;
    }

    if (factory == nullptr)
    {
        SPDLOG_ERROR("Failed to load AudioSoundFont with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::AudioSoundFontFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                    std::shared_ptr<Resource> resource) {
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
            drum.sound.sample = static_cast<Sample*>(GetResourceDataByName(sampleFileName.c_str(), true));
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
            instrument.lowNotesSound.sample = static_cast<Sample*>(GetResourceDataByName(sampleFileName.c_str(), true));
        } else {
            instrument.lowNotesSound.sample = nullptr;
            instrument.lowNotesSound.tuning = 0;
        }

        bool hasNormalNoteSoundFontEntry = reader->ReadInt8();
        if (hasNormalNoteSoundFontEntry) {
            bool hasSampleRef = reader->ReadInt8();
            std::string sampleFileName = reader->ReadString();
            instrument.normalNotesSound.tuning = reader->ReadFloat();
            instrument.normalNotesSound.sample = static_cast<Sample*>(GetResourceDataByName(sampleFileName.c_str(), true));
        } else {
            instrument.normalNotesSound.sample = nullptr;
            instrument.normalNotesSound.tuning = 0;
        }

        bool hasHighNoteSoundFontEntry = reader->ReadInt8();
        if (hasHighNoteSoundFontEntry) {
            bool hasSampleRef = reader->ReadInt8();
            std::string sampleFileName = reader->ReadString();
            instrument.highNotesSound.tuning = reader->ReadFloat();
            instrument.highNotesSound.sample = static_cast<Sample*>(GetResourceDataByName(sampleFileName.c_str(), true));
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
            soundEffect.tuning = reader->ReadFloat();
            soundEffect.sample = static_cast<Sample*>(GetResourceDataByName(sampleFileName.c_str(), true));
        }
        
        audioSoundFont->soundEffects.push_back(soundEffect);
    }
    audioSoundFont->soundFont.soundEffects = audioSoundFont->soundEffects.data();
}
} // namespace Ship
