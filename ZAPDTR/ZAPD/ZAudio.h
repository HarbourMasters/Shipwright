#pragma once

#include "ZResource.h"
#include "tinyxml2.h"

struct AdsrEnvelope
{
	int16_t delay;
	int16_t arg;
};

struct AdpcmBook
{
	/* 0x00 */ int32_t order;
	/* 0x04 */ int32_t npredictors;
	/* 0x08 */ std::vector<int16_t> books;  // size 8 * order * npredictors. 8-byte aligned
};

struct AdpcmLoop
{
	/* 0x00 */ uint32_t start;
	/* 0x04 */ uint32_t end;
	/* 0x08 */ uint32_t count;
	/* 0x10 */ std::vector<int16_t> states;
};

struct SampleEntry
{
	std::string fileName;
	uint8_t bankId;
	uint32_t sampleDataOffset;
	uint32_t sampleLoopOffset = 0xFFFFFFFF;
	uint8_t codec;
	uint8_t medium;
	uint8_t unk_bit26;
	uint8_t unk_bit25;

	std::vector<uint8_t> data;
	AdpcmLoop loop;
	AdpcmBook book;
};

struct SoundFontEntry
{
	SampleEntry* sampleEntry = nullptr;
	float tuning;
};

struct DrumEntry
{
	uint8_t releaseRate;
	uint8_t pan;
	uint8_t loaded;
	uint32_t offset;
	float tuning;
	std::vector<AdsrEnvelope*> env;
	SampleEntry* sample = nullptr;
};

struct InstrumentEntry
{
	bool isValidInstrument;
	uint8_t loaded;
	uint8_t normalRangeLo;
	uint8_t normalRangeHi;
	uint8_t releaseRate;
	std::vector<AdsrEnvelope*> env;
	SoundFontEntry* lowNotesSound = nullptr;
	SoundFontEntry* normalNotesSound = nullptr;
	SoundFontEntry* highNotesSound = nullptr;
};

struct AudioTableEntry
{
	uint32_t ptr;
	uint32_t size;
	uint8_t medium;
	uint8_t cachePolicy;
	uint16_t data1;
	uint16_t data2;
	uint16_t data3;

	std::vector<DrumEntry> drums;
	std::vector<SoundFontEntry*> soundEffects;
	std::vector<InstrumentEntry> instruments;
};

class ZAudio : public ZResource
{
public:
	std::vector<AudioTableEntry> soundFontTable;
	std::vector<AudioTableEntry> sequenceTable;
	std::vector<AudioTableEntry> sampleBankTable;
	std::vector<std::vector<char>> sequences;
	std::map<uint32_t, SampleEntry*> samples;
	std::vector<std::vector<uint32_t>> fontIndices;
	std::vector<std::string> seqNames;
	std::map<uint32_t, std::string> soundFontNames;

	// First Key = Bank ID, Sec Key = LoopDataOffset, Third Key = Sample Data Offset
	std::map<uint32_t, std::map<uint32_t, std::map<uint32_t, std::string>>> sampleOffsets;

	// Key = Loop Offset, Value = Sample Offset
	std::map<uint32_t, uint32_t> specialLoopSamples;

	ZAudio(ZFile* nParent);

	void ParseXML(tinyxml2::XMLElement* reader) override;

	void DecodeADPCMSample(SampleEntry* sample);
	std::vector<AdsrEnvelope*> ParseEnvelopeData(std::vector<uint8_t> audioBank, std::vector<uint8_t> audioTable,
	                               int envelopeOffset, int baseOffset);

	SoundFontEntry* ParseSoundFontEntry(std::vector<uint8_t> audioBank,
	                                    std::vector<uint8_t> audioTable,
	                                    AudioTableEntry audioSampleBankEntry, int bankIndex,
	                                    int soundFontOffset,
	                                    int baseOffset);

	SampleEntry* ParseSampleEntry(std::vector<uint8_t> audioBank, std::vector<uint8_t> audioTable,
	                              AudioTableEntry audioSampleBankEntry, int bankIndex,
	                              int sampleOffset, int baseOffset);

	std::vector<AudioTableEntry> ParseAudioTable(std::vector<uint8_t> codeData, int baseOffset);
	void ParseSoundFont(std::vector<uint8_t> codeData, std::vector<uint8_t> audioTable,
	                    std::vector<AudioTableEntry> audioSampleBank, AudioTableEntry& entry);

	void ParseRawData() override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
};
