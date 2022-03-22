#pragma once

#include "Utils/Directory.h"
#include "ZResource.h"
#include "elfio/elfio.hpp"
#include "tinyxml2.h"

enum class SectionType
{
	Text = 1,
	Data = 2,
	RoData = 3,
	Bss = 4,
	ERROR = 255
};

enum class RelocationType
{
	R_MIPS_32 = 2,
	R_MIPS_26 = 4,
	R_MIPS_HI16 = 5,
	R_MIPS_LO16 = 6,
};

class RelocationEntry
{
public:
	SectionType sectionType;
	RelocationType relocationType;
	int32_t offset;

	RelocationEntry(SectionType nSecType, RelocationType nRelType, int32_t nOffset)
	{
		sectionType = nSecType;
		relocationType = nRelType;
		offset = nOffset;
	}

	uint32_t CalcRelocationWord()
	{
		uint32_t relocationWord = 0;

		relocationWord |= static_cast<uint32_t>(sectionType) << 30;
		relocationWord |= static_cast<uint32_t>(relocationType) << 24;
		relocationWord |= offset;

		return relocationWord;
	}

	const char* GetSectionName() const;
	const char* GetRelocTypeName() const;
};

class ZOverlay
{
public:
	std::string name;

	ZOverlay(const std::string& nName);
	~ZOverlay();
	static ZOverlay* FromBuild(fs::path buildPath, fs::path cfgFolderPath);
	std::string GetSourceOutputCode(const std::string& prefix);

private:
	std::vector<RelocationEntry*> entries;
	std::vector<std::string> cfgLines;

	ZOverlay();

	static SectionType GetSectionTypeFromStr(const std::string& sectionName);
	// static std::string GetOverlayNameFromElf(ELFIO::elfio& reader);

	ELFIO::Elf_Half FindSymbolInSection(const std::string& curSymName, ELFIO::section* sectionData,
	                                    ELFIO::elfio& reader, size_t readerId);
};
