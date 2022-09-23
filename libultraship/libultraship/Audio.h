#pragma once

#include "Resource.h"
#include <vector>
#include <map>
#include <string>

namespace Ship
{
	struct AdsrEnvelope
	{
		int16_t delay;
		int16_t arg;
	};

	struct AdpcmBook
	{
		/* 0x00 */ uint32_t order;
		/* 0x04 */ uint32_t npredictors;
		/* 0x08 */ std::vector<int16_t> books;  // size 8 * order * npredictors. 8-byte aligned
	};

	struct AdpcmLoop
	{
		/* 0x00 */ uint32_t start;
		/* 0x04 */ uint32_t end;
		/* 0x08 */ uint32_t count;
		/* 0x10 */ std::vector<int16_t> states;
	};

	struct SoundFontEntry
	{
		std::string sampleFileName;
		float tuning;
	};

	struct DrumEntry
	{
		uint8_t releaseRate;
		uint8_t pan;
		uint8_t loaded;
		std::string sampleFileName;
		float tuning;
		std::vector<AdsrEnvelope*> env;
	};

	struct InstrumentEntry
	{
		bool isValidEntry;
		uint8_t loaded;
		uint8_t normalRangeLo;
		uint8_t normalRangeHi;
		uint8_t releaseRate;
		std::vector<AdsrEnvelope*> env;
		SoundFontEntry* lowNotesSound = nullptr;
		SoundFontEntry* normalNotesSound = nullptr;
		SoundFontEntry* highNotesSound = nullptr;
	};

	class AudioSequenceV2 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
		static std::vector<AdsrEnvelope*> ReadEnvelopeData(BinaryReader* reader);
	};

	class AudioSoundFontV2 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
		static std::vector<AdsrEnvelope*> ReadEnvelopeData(BinaryReader* reader);
	};

	class AudioSampleV2 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

	class AudioV2 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

	class AudioSequence : public Resource
	{
	public:
		std::vector<char> seqData;
		uint8_t seqNumber;
		uint8_t medium;
		uint8_t cachePolicy;
		std::vector<uint8_t> fonts;
	};

	class AudioSoundFont : public Resource
	{
	public:
		uint32_t ptr;
		uint32_t size;
		uint32_t id;
		uint8_t medium;
		uint8_t cachePolicy;
		uint16_t data1;
		uint16_t data2;
		uint16_t data3;

		std::vector<DrumEntry> drums;
		std::vector<SoundFontEntry*> soundEffects;
		std::vector<InstrumentEntry> instruments;
	};

	class AudioSample : public Resource
	{
	public:
		uint32_t originalOffset;
		uint8_t codec;
		uint8_t medium;
		uint8_t unk_bit26;
		uint8_t unk_bit25;

		std::vector<uint8_t> data;
		AdpcmLoop loop;
		AdpcmBook book;
	};

	class Audio : public Resource
	{
	public:
	};
}
