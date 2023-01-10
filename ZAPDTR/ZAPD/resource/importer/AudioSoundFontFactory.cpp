#include "resource/importer/AudioSoundFontFactory.h"
#include "resource/type/AudioSoundFont.h"
#include "spdlog/spdlog.h"

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



}
} // namespace Ship

/*

    soundFont->id = reader->ReadInt32();
    soundFont->medium = reader->ReadInt8();
    soundFont->cachePolicy = reader->ReadInt8();
    soundFont->data1 = reader->ReadInt16();
    soundFont->data2 = reader->ReadInt16();
    soundFont->data3 = reader->ReadInt16();

    uint32_t drumCnt = reader->ReadInt32();
    uint32_t instrumentCnt = reader->ReadInt32();
    uint32_t sfxCnt = reader->ReadInt32();

    for (uint32_t i = 0; i < drumCnt; i++) {
        DrumEntry drum;
        drum.releaseRate = reader->ReadUByte();
        drum.pan = reader->ReadUByte();
        drum.loaded = reader->ReadUByte();

        drum.env = ReadEnvelopeData(reader);

        bool hasSample = reader->ReadInt8();
        drum.sampleFileName = reader->ReadString();
        drum.tuning = reader->ReadFloat();

        soundFont->drums.push_back(drum);
    }

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