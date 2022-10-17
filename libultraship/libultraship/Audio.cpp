#include "Audio.h"

namespace Ship
{
	void AudioSequenceV2::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		AudioSequence* seq = (AudioSequence*)res;

		ResourceFile::ParseFileBinary(reader, res);

		uint32_t seqDataSize = reader->ReadInt32();

		seq->seqData.reserve(seqDataSize);

		for (uint32_t i = 0; i < seqDataSize; i++)
			seq->seqData.push_back(reader->ReadUByte());

		seq->seqNumber = reader->ReadUByte();
		seq->medium = reader->ReadUByte();
		seq->cachePolicy = reader->ReadUByte();

		uint32_t numFonts = reader->ReadInt32();

		for (uint32_t i = 0; i < numFonts; i++)
			seq->fonts.push_back(reader->ReadUByte());
	}

	void AudioSampleV2::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		AudioSample* entry = (AudioSample*)res;

		ResourceFile::ParseFileBinary(reader, res);

		entry->codec = reader->ReadInt8();
		entry->medium = reader->ReadInt8();
		entry->unk_bit26 = reader->ReadInt8();
		entry->unk_bit25 = reader->ReadInt8();

		uint32_t dataSize = reader->ReadInt32();

		for (uint32_t i = 0; i < dataSize; i++)
			entry->data.push_back(reader->ReadUByte());

		entry->loop.start = reader->ReadUInt32();
		entry->loop.end = reader->ReadUInt32();
		entry->loop.count = reader->ReadUInt32();

		uint32_t loopStateCnt = reader->ReadUInt32();

		for (uint32_t i = 0; i < loopStateCnt; i++)
			entry->loop.states.push_back(reader->ReadInt16());

		entry->book.order = reader->ReadInt32();
		entry->book.npredictors = reader->ReadInt32();

		uint32_t bookSize = reader->ReadInt32();

		for (uint32_t i = 0; i < bookSize; i++)
			entry->book.books.push_back(reader->ReadInt16());
	}

	void AudioSoundFontV2::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		AudioSoundFont* soundFont = (AudioSoundFont*)res;

		ResourceFile::ParseFileBinary(reader, res);

		soundFont->id = reader->ReadInt32();
		soundFont->medium = reader->ReadInt8();
		soundFont->cachePolicy = reader->ReadInt8();
		soundFont->data1 = reader->ReadInt16();
		soundFont->data2 = reader->ReadInt16();
		soundFont->data3 = reader->ReadInt16();

		uint32_t drumCnt = reader->ReadInt32();
		uint32_t instrumentCnt = reader->ReadInt32();
		uint32_t sfxCnt = reader->ReadInt32();

		for (uint32_t i = 0; i < drumCnt; i++)
		{
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

		for (uint32_t i = 0; i < instrumentCnt; i++)
		{
			InstrumentEntry entry;

			entry.isValidEntry = reader->ReadUByte();
			entry.loaded = reader->ReadUByte();
			entry.normalRangeLo = reader->ReadUByte();
			entry.normalRangeHi = reader->ReadUByte();
			entry.releaseRate = reader->ReadUByte();

			entry.env = ReadEnvelopeData(reader);

			{
				bool hasSFEntry = reader->ReadInt8();

				if (hasSFEntry)
				{
					entry.lowNotesSound = new SoundFontEntry();
					bool hasSampleRef = reader->ReadInt8();
					entry.lowNotesSound->sampleFileName = reader->ReadString();
					entry.lowNotesSound->tuning = reader->ReadFloat();
				}
			}

			{
				bool hasSFEntry = reader->ReadInt8();

				if (hasSFEntry)
				{
					entry.normalNotesSound = new SoundFontEntry();
					bool hasSampleRef = reader->ReadInt8();
					entry.normalNotesSound->sampleFileName = reader->ReadString();
					entry.normalNotesSound->tuning = reader->ReadFloat();
				}
			}

			{
				bool hasSFEntry = reader->ReadInt8();

				if (hasSFEntry)
				{
					entry.highNotesSound = new SoundFontEntry();
					bool hasSampleRef = reader->ReadInt8();
					entry.highNotesSound->sampleFileName = reader->ReadString();
					entry.highNotesSound->tuning = reader->ReadFloat();
				}
			}

			soundFont->instruments.push_back(entry);
		}

		for (uint32_t i = 0; i < sfxCnt; i++)
		{
			SoundFontEntry* entry = new SoundFontEntry();

			bool hasSFEntry = reader->ReadInt8();

			if (hasSFEntry)
			{
				bool hasSampleRef = reader->ReadInt8();
				entry->sampleFileName = reader->ReadString();
				entry->tuning = reader->ReadFloat();
			}

			soundFont->soundEffects.push_back(entry);
		}
	}

	std::vector<AdsrEnvelope*> AudioSoundFontV2::ReadEnvelopeData(BinaryReader* reader)
	{
		std::vector<AdsrEnvelope*> envelopes;

		uint32_t envelopeCnt = reader->ReadInt32();

		for (uint32_t i = 0; i < envelopeCnt; i++)
		{
			AdsrEnvelope* env = new AdsrEnvelope();
			env->delay = reader->ReadInt16();
			env->arg = reader->ReadInt16();

			envelopes.push_back(env);
		}

		return envelopes;
	}

	void AudioV2::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		Audio* audio = (Audio*)res;

		ResourceFile::ParseFileBinary(reader, res);
	}
}
