#include "Audio.h"

namespace Ship
{
	void AudioSampleV1::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		AudioSample* entry = (AudioSample*)res;

		ResourceFile::ParseFileBinary(reader, res);

		entry->codec = reader->ReadByte();
		entry->medium = reader->ReadByte();
		entry->unk_bit26 = reader->ReadByte();
		entry->unk_bit25 = reader->ReadByte();

		int dataSize = reader->ReadInt32();
		
		for (uint32_t i = 0; i < dataSize; i++)
			entry->data.push_back(reader->ReadUByte());

		entry->loop.start = reader->ReadUInt32();
		entry->loop.end = reader->ReadUInt32();
		entry->loop.count = reader->ReadUInt32();

		int loopStateCnt = reader->ReadUInt32();

		for (uint32_t i = 0; i < loopStateCnt; i++)
			entry->loop.states.push_back(reader->ReadInt16());

		entry->book.order = reader->ReadInt32();
		entry->book.npredictors = reader->ReadInt32();

		int bookSize = reader->ReadInt32();

		for (uint32_t i = 0; i < bookSize; i++)
			entry->book.books.push_back(reader->ReadInt16());
	}

	void AudioSoundFontV1::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		AudioSoundFont* soundFont = (AudioSoundFont*)res;

		ResourceFile::ParseFileBinary(reader, res);

		soundFont->id = reader->ReadInt32();
		soundFont->medium = reader->ReadByte();
		soundFont->cachePolicy = reader->ReadByte();
		soundFont->data1 = reader->ReadInt16();
		soundFont->data2 = reader->ReadInt16();
		soundFont->data3 = reader->ReadInt16();

		int drumCnt = reader->ReadInt32();
		int instrumentCnt = reader->ReadInt32();
		int sfxCnt = reader->ReadInt32();

		for (int i = 0; i < drumCnt; i++)
		{
			DrumEntry drum;
			drum.releaseRate = reader->ReadUByte();
			drum.pan = reader->ReadUByte();
			drum.loaded = reader->ReadUByte();

			drum.env = ReadEnvelopeData(reader);
			
			bool hasSample = reader->ReadByte();
			drum.sampleFileName = reader->ReadString();
			drum.tuning = reader->ReadSingle();

			soundFont->drums.push_back(drum);
		}

		for (int i = 0; i < instrumentCnt; i++)
		{
			InstrumentEntry entry;

			entry.isValidEntry = reader->ReadByte();
			entry.loaded = reader->ReadByte();
			entry.normalRangeLo = reader->ReadByte();
			entry.normalRangeHi = reader->ReadByte();
			entry.releaseRate = reader->ReadByte();

			entry.env = ReadEnvelopeData(reader);
			
			{
				bool hasSFEntry = reader->ReadByte();

				if (hasSFEntry)
				{
					entry.lowNotesSound = new SoundFontEntry();
					bool hasSampleRef = reader->ReadByte();
					entry.lowNotesSound->sampleFileName = reader->ReadString();
					entry.lowNotesSound->tuning = reader->ReadSingle();
				}
			}

			{
				bool hasSFEntry = reader->ReadByte();

				if (hasSFEntry)
				{
					entry.normalNotesSound = new SoundFontEntry();
					bool hasSampleRef = reader->ReadByte();
					entry.normalNotesSound->sampleFileName = reader->ReadString();
					entry.normalNotesSound->tuning = reader->ReadSingle();
				}
			}

			{
				bool hasSFEntry = reader->ReadByte();

				if (hasSFEntry)
				{
					entry.highNotesSound = new SoundFontEntry();
					bool hasSampleRef = reader->ReadByte();
					entry.highNotesSound->sampleFileName = reader->ReadString();
					entry.highNotesSound->tuning = reader->ReadSingle();
				}
			}

			soundFont->instruments.push_back(entry);
		}

		for (int i = 0; i < sfxCnt; i++)
		{
			SoundFontEntry* entry = new SoundFontEntry();

			bool hasSFEntry = reader->ReadByte();

			if (hasSFEntry)
			{
				bool hasSampleRef = reader->ReadByte();
				entry->sampleFileName = reader->ReadString();
				entry->tuning = reader->ReadSingle();
			}

			soundFont->soundEffects.push_back(entry);
		}
	}

	std::vector<AdsrEnvelope*> AudioSoundFontV1::ReadEnvelopeData(BinaryReader* reader)
	{
		std::vector<AdsrEnvelope*> envelopes;

		int envelopeCnt = reader->ReadInt32();

		for (int i = 0; i < envelopeCnt; i++)
		{
			AdsrEnvelope* env = new AdsrEnvelope();
			env->delay = reader->ReadInt16();
			env->arg = reader->ReadInt16();

			envelopes.push_back(env);
		}

		return envelopes;
	}

	void AudioV1::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		Audio* audio = (Audio*)res;

		ResourceFile::ParseFileBinary(reader, res);
	}
}