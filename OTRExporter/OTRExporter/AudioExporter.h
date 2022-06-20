#pragma once

#include "ZResource.h"
#include "ZAudio.h"
#include "Exporter.h"
#include <Utils/BinaryWriter.h>

class OTRExporter_Audio : public OTRExporter
{
public:
	void WriteSampleEntry(SampleEntry* entry, BinaryWriter* writer);
	void WriteSampleEntryReference(ZAudio* audio, SampleEntry* entry, std::map<uint32_t, SampleEntry*> samples, BinaryWriter* writer);
	void WriteSoundFontEntry(ZAudio* audio, SoundFontEntry* entry, std::map<uint32_t, SampleEntry*> samples, BinaryWriter* writer);
	void WriteEnvData(std::vector<AdsrEnvelope*> envelopes, BinaryWriter* writer);
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};