#include "AudioExporter.h"
#include "Main.h"
#include <Animation.h>
#include <Utils/MemoryStream.h>
#include <Globals.h>
#include <Utils/File.h>
#include "DisplayListExporter.h"

void OTRExporter_Audio::WriteSampleEntryReference(ZAudio* audio, SampleEntry* entry, std::map<uint32_t, SampleEntry*> samples, BinaryWriter* writer)
{
	writer->Write((uint8_t)(entry != nullptr ? 1 : 0));

	if (entry != nullptr)
	{
		if (audio->sampleOffsets[entry->bankId].find(entry->sampleLoopOffset) != audio->sampleOffsets[entry->bankId].end())
		{
			if (audio->sampleOffsets[entry->bankId][entry->sampleLoopOffset].find(entry->sampleDataOffset) != audio->sampleOffsets[entry->bankId][entry->sampleLoopOffset].end())
			{
				writer->Write(StringHelper::Sprintf("audio/samples/%s", audio->sampleOffsets[entry->bankId][entry->sampleLoopOffset][entry->sampleDataOffset].c_str()));
			}
			else
				writer->Write(entry->fileName);
		}
		else
			writer->Write(entry->fileName);
	}
	else
		writer->Write("");
}

void OTRExporter_Audio::WriteSampleEntry(SampleEntry* entry, BinaryWriter* writer)
{
	WriteHeader(nullptr, "", writer, Ship::ResourceType::AudioSample, Ship::Version::Rachael);

	writer->Write(entry->codec);
	writer->Write(entry->medium);
	writer->Write(entry->unk_bit26);
	writer->Write(entry->unk_bit25);

	writer->Write((uint32_t)entry->data.size());
	writer->Write((char*)entry->data.data(), entry->data.size());

	writer->Write((uint32_t)(entry->loop.start));
	writer->Write((uint32_t)(entry->loop.end));
	writer->Write((uint32_t)(entry->loop.count));
	writer->Write((uint32_t)entry->loop.states.size());

	for (size_t i = 0; i < entry->loop.states.size(); i++)
		writer->Write((entry->loop.states[i]));

	writer->Write((uint32_t)(entry->book.order));
	writer->Write((uint32_t)(entry->book.npredictors));
	writer->Write((uint32_t)entry->book.books.size());

	for (size_t i = 0; i < entry->book.books.size(); i++)
		writer->Write((entry->book.books[i]));
}

void OTRExporter_Audio::WriteSoundFontEntry(ZAudio* audio, SoundFontEntry* entry, std::map<uint32_t, SampleEntry*> samples, BinaryWriter* writer)
{
	writer->Write((uint8_t)(entry != nullptr ? 1 : 0));

	if (entry != nullptr)
	{
		WriteSampleEntryReference(audio, entry->sampleEntry, samples, writer);
		writer->Write(entry->tuning);
	}
}

void OTRExporter_Audio::WriteEnvData(std::vector<AdsrEnvelope*> envelopes, BinaryWriter* writer)
{
	writer->Write((uint32_t)envelopes.size());

	for (auto env : envelopes)
	{
		writer->Write(env->delay);
		writer->Write(env->arg);
	}
}

void OTRExporter_Audio::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer)
{
	ZAudio* audio = (ZAudio*)res;

	WriteHeader(res, outPath, writer, Ship::ResourceType::Audio, Ship::Version::Rachael);

	// Write Samples as individual files
	for (auto pair : audio->samples)
	{
		MemoryStream* sampleStream = new MemoryStream();
		BinaryWriter sampleWriter = BinaryWriter(sampleStream);

		writer->Write((uint32_t)pair.first);
		WriteSampleEntry(pair.second, &sampleWriter);

		std::string basePath = "";

		if (audio->sampleOffsets[pair.second->bankId].find(pair.second->sampleLoopOffset) != audio->sampleOffsets[pair.second->bankId].end())
		{
			if (audio->sampleOffsets[pair.second->bankId][pair.second->sampleLoopOffset].find(pair.second->sampleDataOffset) != audio->sampleOffsets[pair.second->bankId][pair.second->sampleLoopOffset].end())
				basePath = StringHelper::Sprintf("samples/%s", audio->sampleOffsets[pair.second->bankId][pair.second->sampleLoopOffset][pair.second->sampleDataOffset].c_str());
			else
				basePath = StringHelper::Sprintf("samples/sample_%08X", pair.first);
		}
		else
			basePath = StringHelper::Sprintf("samples/sample_%08X", pair.first);

		std::string fName = OTRExporter_DisplayList::GetPathToRes(res, basePath);
		AddFile(fName, sampleStream->ToVector());
	}

	// Write the soundfont table
	for (size_t i = 0; i < audio->soundFontTable.size(); i++)
	{
		MemoryStream* fntStream = new MemoryStream();
		BinaryWriter fntWriter = BinaryWriter(fntStream);

		WriteHeader(nullptr, "", &fntWriter, Ship::ResourceType::AudioSoundFont, Ship::Version::Rachael);

		fntWriter.Write((uint32_t)i);
		fntWriter.Write(audio->soundFontTable[i].medium);
		fntWriter.Write(audio->soundFontTable[i].cachePolicy);
		fntWriter.Write(audio->soundFontTable[i].data1);
		fntWriter.Write(audio->soundFontTable[i].data2);
		fntWriter.Write(audio->soundFontTable[i].data3);

		fntWriter.Write((uint32_t)audio->soundFontTable[i].drums.size());
		fntWriter.Write((uint32_t)audio->soundFontTable[i].instruments.size());
		fntWriter.Write((uint32_t)audio->soundFontTable[i].soundEffects.size());

		for (size_t k = 0; k < audio->soundFontTable[i].drums.size(); k++)
		{
			fntWriter.Write(audio->soundFontTable[i].drums[k].releaseRate);
			fntWriter.Write(audio->soundFontTable[i].drums[k].pan);
			fntWriter.Write(audio->soundFontTable[i].drums[k].loaded);

			WriteEnvData(audio->soundFontTable[i].drums[k].env, &fntWriter);

			WriteSampleEntryReference(audio, audio->soundFontTable[i].drums[k].sample, audio->samples, &fntWriter);
			fntWriter.Write(audio->soundFontTable[i].drums[k].tuning);
		}

		for (size_t k = 0; k < audio->soundFontTable[i].instruments.size(); k++)
		{
			fntWriter.Write((uint8_t)audio->soundFontTable[i].instruments[k].isValidInstrument);

			fntWriter.Write(audio->soundFontTable[i].instruments[k].loaded);
			fntWriter.Write(audio->soundFontTable[i].instruments[k].normalRangeLo);
			fntWriter.Write(audio->soundFontTable[i].instruments[k].normalRangeHi);
			fntWriter.Write(audio->soundFontTable[i].instruments[k].releaseRate);

			WriteEnvData(audio->soundFontTable[i].instruments[k].env, &fntWriter);

			WriteSoundFontEntry(audio, audio->soundFontTable[i].instruments[k].lowNotesSound, audio->samples, &fntWriter);
			WriteSoundFontEntry(audio, audio->soundFontTable[i].instruments[k].normalNotesSound, audio->samples, &fntWriter);
			WriteSoundFontEntry(audio, audio->soundFontTable[i].instruments[k].highNotesSound, audio->samples, &fntWriter);
		}

		for (size_t k = 0; k < audio->soundFontTable[i].soundEffects.size(); k++)
		{
			WriteSoundFontEntry(audio, audio->soundFontTable[i].soundEffects[k], audio->samples, &fntWriter);
		}

		//std::string fName = OTRExporter_DisplayList::GetPathToRes(res, StringHelper::Sprintf("fonts/font_%02X", i));
		std::string fName = OTRExporter_DisplayList::GetPathToRes(res, StringHelper::Sprintf("fonts/%s", audio->soundFontNames[i].c_str()));
		AddFile(fName, fntStream->ToVector());
	}

	// Write Sequences
	for (size_t i = 0; i < audio->sequences.size(); i++)
	{
		auto seq = audio->sequences[i];

		MemoryStream* seqStream = new MemoryStream();
		BinaryWriter seqWriter = BinaryWriter(seqStream);

		WriteHeader(nullptr, "", &seqWriter, Ship::ResourceType::AudioSequence, Ship::Version::Rachael);

		seqWriter.Write((uint32_t)seq.size());
		seqWriter.Write(seq.data(), seq.size());
		seqWriter.Write((uint8_t)i);
		seqWriter.Write((uint8_t)audio->sequenceTable[i].medium);
		seqWriter.Write((uint8_t)audio->sequenceTable[i].cachePolicy);
		seqWriter.Write((uint32_t)audio->fontIndices[i].size());

		for (size_t k = 0; k < audio->fontIndices[i].size(); k++)
			seqWriter.Write((uint8_t)audio->fontIndices[i][k]);

		std::string fName = OTRExporter_DisplayList::GetPathToRes(res, StringHelper::Sprintf("sequences/%s", audio->seqNames[i].c_str()));
		AddFile(fName, seqStream->ToVector());
	}
}
