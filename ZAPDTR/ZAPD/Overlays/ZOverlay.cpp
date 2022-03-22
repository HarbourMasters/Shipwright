#include "ZOverlay.h"

#include <cassert>
#include <unordered_set>
#include "Globals.h"
#include "Utils/Directory.h"
#include "Utils/File.h"
#include "Utils/Path.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"

using namespace ELFIO;

const char* RelocationEntry::GetSectionName() const
{
	switch (sectionType)
	{
	case SectionType::Text:
		return ".text";
	case SectionType::Data:
		return ".data";
	case SectionType::RoData:
		return ".rodata";
	case SectionType::Bss:
		return ".bss";
	case SectionType::ERROR:
		return ".ERROR";
	}
	assert(!"Oh no :c");
}

const char* RelocationEntry::GetRelocTypeName() const
{
	switch (relocationType)
	{
	case RelocationType::R_MIPS_32:
		return "R_MIPS_32";
	case RelocationType::R_MIPS_26:
		return "R_MIPS_26";
	case RelocationType::R_MIPS_HI16:
		return "R_MIPS_HI16";
	case RelocationType::R_MIPS_LO16:
		return "R_MIPS_LO16";
	}
	assert(!"Oh no :c");
}

ZOverlay::ZOverlay()
{
	name = "";
	entries = std::vector<RelocationEntry*>();
}

ZOverlay::ZOverlay(const std::string& nName) : ZOverlay()
{
	name = nName;
}

ZOverlay::~ZOverlay()
{
	for (auto entry : entries)
		if (entry)
			delete entry;
	entries.clear();
}

static const std::unordered_set<std::string> sRelSections = {
	".rel.text",
	".rel.data",
	".rel.rodata",
};
static const std::unordered_set<std::string> sSections = {
	".text", ".data", ".symtab", ".rodata", ".rodata.str1.4", ".rodata.cst4", ".rodata.cst8",
};

ZOverlay* ZOverlay::FromBuild(fs::path buildPath, fs::path cfgFolderPath)
{
	std::string cfgText = File::ReadAllText(cfgFolderPath / "overlay.cfg");
	std::vector<std::string> cfgLines = StringHelper::Split(cfgText, "\n");

	ZOverlay* ovl = new ZOverlay(StringHelper::Strip(cfgLines[0], "\r"));
	ovl->cfgLines = cfgLines;

	int32_t sectionOffs[5] = {0};
	std::vector<RelocationEntry*> textRelocs;
	std::vector<RelocationEntry*> dataRelocs;
	std::vector<RelocationEntry*> rodataRelocs;

	// get the elf files
	std::vector<elfio*> readers;
	for (size_t i = 1; i < cfgLines.size(); i++)
	{
		std::string elfPath =
			(buildPath / (cfgLines[i].substr(0, cfgLines[i].size() - 2) + ".o")).string();
		elfio* reader = new elfio();

		if (!reader->load(elfPath))
		{
			// not all files were compiled
			for (auto r : readers)
				delete r;
			readers.clear();

			delete ovl;
			return nullptr;
		}

		readers.push_back(reader);
	}

	for (size_t curReaderId = 0; curReaderId < readers.size(); curReaderId++)
	{
		auto& curReader = readers[curReaderId];

		Elf_Half sec_num = curReader->sections.size();
		for (int32_t i = 0; i < sec_num; i++)
		{
			section* pSec = curReader->sections[i];

			if (pSec->get_type() != SHT_REL ||
			    sRelSections.find(pSec->get_name()) == sRelSections.end())
			{
				continue;
			}

			symbol_section_accessor currentSymbols(*curReader,
			                                       curReader->sections[(Elf_Half)pSec->get_link()]);

			SectionType sectionType = GetSectionTypeFromStr(pSec->get_name());

			if (sectionType == SectionType::ERROR)
			{
				HANDLE_WARNING(WarningType::Always, "one of the section types returned ERROR", "");
			}

			relocation_section_accessor relocs(*curReader, pSec);
			for (Elf_Xword j = 0; j < relocs.get_entries_num(); j++)
			{
				Elf64_Addr offset = 0;
				Elf_Word symbol = 0;
				Elf_Word type = 0;
				{
					Elf_Sxword addend = 0;
					relocs.get_entry(j, offset, symbol, type, addend);
				}

				std::string curSymName;
				Elf_Half curSymShndx = SHN_UNDEF;
				{
					Elf64_Addr value;
					Elf_Xword size;
					unsigned char bind;
					unsigned char type;
					unsigned char other;
					currentSymbols.get_symbol(symbol, curSymName, value, size, bind, type,
					                          curSymShndx, other);
				}

				// check symbols outside the elf but within the overlay
				if (curSymShndx == SHN_UNDEF)
				{
					if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
					{
						printf("Symbol '%s' doesn't exist in current .o file (%s). Searching...\n",
						       curSymName.c_str(), cfgLines[curReaderId + 1].c_str());
					}

					for (size_t readerId = 0; readerId < readers.size(); readerId++)
					{
						auto& reader = readers[readerId];

						if (curSymShndx != SHN_UNDEF)
							break;

						if (reader == curReader)
							continue;

						auto sectionData = reader->sections[(Elf_Half)pSec->get_link()];
						curSymShndx =
							ovl->FindSymbolInSection(curSymName, sectionData, *reader, readerId);
						if (curSymShndx != SHN_UNDEF)
							break;

						if (Globals::Instance->gccCompat)
						{
							// Symbol wasn't found, try checking every section
							Elf_Half sec_num = reader->sections.size();
							for (int32_t otherSectionIdx = 0; otherSectionIdx < sec_num;
							     otherSectionIdx++)
							{
								if (curSymShndx != SHN_UNDEF)
								{
									break;
								}

								auto sectionDataIter = reader->sections[otherSectionIdx];

								curSymShndx = ovl->FindSymbolInSection(curSymName, sectionDataIter,
								                                       *reader, readerId);
							}
						}
					}
				}

				if (curSymShndx != SHN_UNDEF)
				{
					RelocationType typeConverted = (RelocationType)type;
					offset += sectionOffs[static_cast<size_t>(sectionType)];

					RelocationEntry* reloc =
						new RelocationEntry(sectionType, typeConverted, offset);

					if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
					{
						printf(".word 0x%08X  # %s %s 0x%04X\n", reloc->CalcRelocationWord(),
						       reloc->GetSectionName(), reloc->GetRelocTypeName(), reloc->offset);
					}

					// this is to keep the correct reloc entry order
					if (sectionType == SectionType::Text)
						textRelocs.push_back(reloc);
					if (sectionType == SectionType::Data)
						dataRelocs.push_back(reloc);
					if (sectionType == SectionType::RoData)
						rodataRelocs.push_back(reloc);
				}
			}
		}

		// increase section offsets
		for (int32_t i = 0; i < sec_num; i++)
		{
			section* pSec = curReader->sections[i];
			if (pSec->get_type() == SHT_PROGBITS &&
			    sSections.find(pSec->get_name()) != sSections.end())
			{
				SectionType sectionType = GetSectionTypeFromStr(pSec->get_name());
				sectionOffs[static_cast<size_t>(sectionType)] += pSec->get_size();
			}
		}
	}

	ovl->entries.reserve(textRelocs.size() + dataRelocs.size() + rodataRelocs.size());
	ovl->entries.insert(ovl->entries.end(), textRelocs.begin(), textRelocs.end());
	ovl->entries.insert(ovl->entries.end(), dataRelocs.begin(), dataRelocs.end());
	ovl->entries.insert(ovl->entries.end(), rodataRelocs.begin(), rodataRelocs.end());

	for (auto r : readers)
		delete r;
	readers.clear();

	return ovl;
}

std::string ZOverlay::GetSourceOutputCode([[maybe_unused]] const std::string& prefix)
{
	std::string output;

	output += ".section .ovl\n";

	output += StringHelper::Sprintf("# %sOverlayInfo\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentTextSize\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentDataSize\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentRoDataSize\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentBssSize\n", name.c_str());
	output += "\n";

	output += StringHelper::Sprintf(".word %i # reloc_count\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		RelocationEntry* reloc = entries[i];
		output += StringHelper::Sprintf(".word 0x%08X  # %s %s 0x%04X\n",
		                                reloc->CalcRelocationWord(), reloc->GetSectionName(),
		                                reloc->GetRelocTypeName(), reloc->offset);
	}

	size_t offset = (entries.size() * 4) + 20;

	while (offset % 16 != 12)
	{
		output += ".word 0\n";
		offset += 4;
	}

	output += "\n";
	output +=
		StringHelper::Sprintf(".word 0x%08X # %sOverlayInfoOffset\n", offset + 4, name.c_str());
	return output;
}

SectionType ZOverlay::GetSectionTypeFromStr(const std::string& sectionName)
{
	if (sectionName == ".rel.text" || sectionName == ".text")
		return SectionType::Text;
	else if (sectionName == ".rel.data" || sectionName == ".data")
		return SectionType::Data;
	else if (sectionName == ".rel.rodata" || sectionName == ".rodata" ||
	         sectionName == ".rodata.str1.4" || sectionName == ".rodata.cst4")
		return SectionType::RoData;
	else if (sectionName == ".rel.bss" || sectionName == ".bss")
		return SectionType::Bss;

	return SectionType::ERROR;
}

ELFIO::Elf_Half ZOverlay::FindSymbolInSection(const std::string& curSymName,
                                              ELFIO::section* sectionData, ELFIO::elfio& reader,
                                              size_t readerId)
{
	if (sectionData == nullptr)
		return SHN_UNDEF;

	auto sectionDataName = sectionData->get_name();
	if (sSections.find(sectionDataName) == sSections.end())
		return SHN_UNDEF;

#ifdef DEVELOPMENT
	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
	{
		printf("\t File '%s' section: %s \n", cfgLines[readerId + 1].c_str(),
		       sectionDataName.c_str());
	}
#endif

	symbol_section_accessor symbols(reader, sectionData);

	Elf_Xword symbolNum = symbols.get_symbols_num();
	for (Elf_Xword symIdx = 0; symIdx < symbolNum; symIdx++)
	{
		Elf_Half shndx = SHN_UNDEF;
		Elf64_Addr value;
		std::string name;
		Elf_Xword size;
		unsigned char bind;
		unsigned char type;
		unsigned char other;

		symbols.get_symbol(symIdx, name, value, size, bind, type, shndx, other);

		if (name == curSymName)
		{
			if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
			{
				printf("\t Symbol '%s' found in '%s' '%s' \n", curSymName.c_str(),
				       cfgLines[readerId + 1].c_str(), sectionDataName.c_str());
			}
			return shndx;
		}
	}
	return SHN_UNDEF;
}
