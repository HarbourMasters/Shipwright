#include "resource/importer/AudioSoundFontFactory.h"
#include "resource/type/AudioSoundFont.h"
#include "spdlog/spdlog.h"
#include "libultraship/src/core/bridge/resourcebridge.h"

namespace Ship {
std::shared_ptr<Resource> AudioSoundFontFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<AudioSoundFont>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<AudioSoundFontFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load AudioSoundFont with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::AudioSoundFontFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                    std::shared_ptr<Resource> resource)
{
	std::shared_ptr<AudioSoundFont> audioSoundFont = std::static_pointer_cast<AudioSoundFont>(resource);
	ResourceFile::ParseFileBinary(reader, audioSoundFont);

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
        for (uint32_t j = 0; j < audioSoundFont->drumEnvelopeCounts[i]; j++) {
            AdsrEnvelope env;
            
            int16_t delay = reader->ReadInt16();
            int16_t arg = reader->ReadInt16();

            env.delay = BE16SWAP(delay);
            env.arg = BE16SWAP(arg);

            drumEnvelopes.push_back(env);
        }
        audioSoundFont->drumEnvelopeArrays.push_back(drumEnvelopes);
        drum.envelope = audioSoundFont->drumEnvelopeArrays[i].data();

        bool hasSample = reader->ReadInt8();
        std::string sampleFileName = reader->ReadString();
        drum.sound.tuning = reader->ReadFloat();
        drum.sound.sample = static_cast<Sample*>(GetResourceDataByName(sampleFileName.c_str()));

        audioSoundFont->drums.push_back(drum);
        audioSoundFont->drumAddresses.push_back(&audioSoundFont->drums.back());
    }
    audioSoundFont->soundFont.drums = audioSoundFont->drumAddresses.data();

    audioSoundFont->instruments.reserve(audioSoundFont->instrumentCount);

    audioSoundFont->soundEffects.reserve(audioSoundFont->soundEffectCount);
}
} // namespace Ship

/*
    


    for (uint32_t i = 0; i < instrumentCnt; i++) {
        InstrumentEntry entry;

        entry.isValidEntry = reader->ReadUByte();
        entry.loaded = reader->ReadUByte();
        entry.normalRangeLo = reader->ReadUByte();
        entry.normalRangeHi = reader->ReadUByte();
        entry.releaseRate = reader->ReadUByte();

        entry.env = ReadEnvelopeData(reader);

        {
            bool hasSFEntry = reader->ReadInt8();

            if (hasSFEntry) {
                entry.lowNotesSound = new SoundFontEntry();
                bool hasSampleRef = reader->ReadInt8();
                entry.lowNotesSound->sampleFileName = reader->ReadString();
                entry.lowNotesSound->tuning = reader->ReadFloat();
            }
        }

        {
            bool hasSFEntry = reader->ReadInt8();

            if (hasSFEntry) {
                entry.normalNotesSound = new SoundFontEntry();
                bool hasSampleRef = reader->ReadInt8();
                entry.normalNotesSound->sampleFileName = reader->ReadString();
                entry.normalNotesSound->tuning = reader->ReadFloat();
            }
        }

        {
            bool hasSFEntry = reader->ReadInt8();

            if (hasSFEntry) {
                entry.highNotesSound = new SoundFontEntry();
                bool hasSampleRef = reader->ReadInt8();
                entry.highNotesSound->sampleFileName = reader->ReadString();
                entry.highNotesSound->tuning = reader->ReadFloat();
            }
        }

        soundFont->instruments.push_back(entry);
    }

    for (uint32_t i = 0; i < sfxCnt; i++) {
        SoundFontEntry* entry = new SoundFontEntry();

        bool hasSFEntry = reader->ReadInt8();

        if (hasSFEntry) {
            bool hasSampleRef = reader->ReadInt8();
            entry->sampleFileName = reader->ReadString();
            entry->tuning = reader->ReadFloat();
        }

        soundFont->soundEffects.push_back(entry);
    }

*/

/*

        SoundFont* soundFontC = (SoundFont*)malloc(sizeof(SoundFont));

        soundFontC->drums = (Drum**)malloc(sizeof(Drum*) * soundFont->drums.size());



        soundFontC->instruments = (Instrument**)malloc(sizeof(Instrument*) * soundFont->instruments.size());

        for (size_t i = 0; i < soundFont->instruments.size(); i++) {

            if (soundFont->instruments[i].isValidEntry)
            {
                Instrument* inst = (Instrument*)malloc(sizeof(Instrument));

                inst->loaded = 0;
                inst->releaseRate = soundFont->instruments[i].releaseRate;
                inst->normalRangeLo = soundFont->instruments[i].normalRangeLo;
                inst->normalRangeHi = soundFont->instruments[i].normalRangeHi;

                if (soundFont->instruments[i].env.size() == 0)
                    inst->envelope = NULL;
                else
                {
                    inst->envelope = (AdsrEnvelope*)malloc(sizeof(AdsrEnvelope) * soundFont->instruments[i].env.size());

                    for (int k = 0; k < soundFont->instruments[i].env.size(); k++)
                    {
                        inst->envelope[k].delay = BE16SWAP(soundFont->instruments[i].env[k]->delay);
                        inst->envelope[k].arg = BE16SWAP(soundFont->instruments[i].env[k]->arg);
                    }
                }
                if (soundFont->instruments[i].lowNotesSound != nullptr)
                {
                    inst->lowNotesSound.sample =
                        ResourceMgr_LoadAudioSample(soundFont->instruments[i].lowNotesSound->sampleFileName.c_str());
                    inst->lowNotesSound.tuning = soundFont->instruments[i].lowNotesSound->tuning;
                } else {
                    inst->lowNotesSound.sample = NULL;
                    inst->lowNotesSound.tuning = 0;
                }

                if (soundFont->instruments[i].normalNotesSound != nullptr) {
                    inst->normalNotesSound.sample =
                        ResourceMgr_LoadAudioSample(soundFont->instruments[i].normalNotesSound->sampleFileName.c_str());
                    inst->normalNotesSound.tuning = soundFont->instruments[i].normalNotesSound->tuning;

                } else {
                    inst->normalNotesSound.sample = NULL;
                    inst->normalNotesSound.tuning = 0;
                }

                if (soundFont->instruments[i].highNotesSound != nullptr) {
                    inst->highNotesSound.sample =
                        ResourceMgr_LoadAudioSample(soundFont->instruments[i].highNotesSound->sampleFileName.c_str());
                    inst->highNotesSound.tuning = soundFont->instruments[i].highNotesSound->tuning;
                } else {
                    inst->highNotesSound.sample = NULL;
                    inst->highNotesSound.tuning = 0;
                }

                soundFontC->instruments[i] = inst;
            }
            else
            {
                soundFontC->instruments[i] = nullptr;
            }
        }

        soundFontC->soundEffects = (SoundFontSound*)malloc(sizeof(SoundFontSound) * soundFont->soundEffects.size());

        for (size_t i = 0; i < soundFont->soundEffects.size(); i++)
        {
            soundFontC->soundEffects[i].sample = ResourceMgr_LoadAudioSample(soundFont->soundEffects[i]->sampleFileName.c_str());
            soundFontC->soundEffects[i].tuning = soundFont->soundEffects[i]->tuning;
        }

        soundFont->CachedGameAsset = soundFontC;
        return soundFontC;

*/