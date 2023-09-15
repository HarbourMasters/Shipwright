#include "ZFile.h"

#include <algorithm>
#include <cassert>
#include <string_view>
#include <unordered_set>

#include "Globals.h"
#include "OutputFormatter.h"
#include "Utils/BinaryWriter.h"
#include "Utils/BitConverter.h"
#include "Utils/Directory.h"
#include <Utils/DiskFile.h>
#include "Utils/MemoryStream.h"
#include "Utils/Path.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZAnimation.h"
#include "ZArray.h"
#include "ZBackground.h"
#include "ZBlob.h"
#include "ZCollision.h"
#include "ZCutscene.h"
#include "ZDisplayList.h"
#include "ZLimb.h"
#include "ZMtx.h"
#include "ZRoom/ZRoom.h"
#include "ZScalar.h"
#include "ZSkeleton.h"
#include "ZSymbol.h"
#include "ZTexture.h"
#include "ZVector.h"
#include "ZVtx.h"

ZFile::ZFile()
{
	resources = std::vector<ZResource*>();
	basePath = "";
	declarations = std::map<uint32_t, Declaration*>();
	defines = "";
	baseAddress = 0;
	rangeStart = 0x000000000;
	rangeEnd = 0xFFFFFFFF;
	workerID = 0;
}

ZFile::ZFile(const fs::path& nOutPath, const std::string& nName) : ZFile()
{
	name = nName;
	outName = nName;
	outputPath = nOutPath;
}

ZFile::ZFile(ZFileMode nMode, tinyxml2::XMLElement* reader, const fs::path& nBasePath,
             const fs::path& nOutPath, const std::string& filename, const fs::path& nXmlFilePath, int nWorkerID)
	: ZFile()
{
	xmlFilePath = nXmlFilePath;
	if (nBasePath == "")
		basePath = Directory::GetCurrentDirectory();
	else
		basePath = nBasePath;

	if (nOutPath == "")
		outputPath = Directory::GetCurrentDirectory();
	else
		outputPath = nOutPath;

	mode = nMode;
	workerID = nWorkerID;

	ParseXML(reader, filename);
	if (mode != ZFileMode::ExternalFile)
		DeclareResourceSubReferences();
}

ZFile::~ZFile()
{
	for (ZResource* res : resources)
		delete res;

	for (auto d : declarations)
		delete d.second;

	for (auto sym : symbolResources)
		delete sym.second;
}

void ZFile::ParseXML(tinyxml2::XMLElement* reader, const std::string& filename)
{
	assert(mode != ZFileMode::Invalid);

	if (filename == "")
		name = reader->Attribute("Name");
	else
		name = filename;

	outName = name;
	const char* outNameXml = reader->Attribute("OutName");
	if (outNameXml != nullptr)
		outName = outNameXml;

	// TODO: This should be a variable on the ZFile, but it is a large change in order to force all
	// ZResource types to have a parent ZFile.
	const char* gameStr = reader->Attribute("Game");
	if (reader->Attribute("Game") != nullptr)
	{
		if (std::string_view(gameStr) == "MM")
			Globals::Instance->game = ZGame::MM_RETAIL;
		else if (std::string_view(gameStr) == "SW97" || std::string_view(gameStr) == "OOTSW97")
			Globals::Instance->game = ZGame::OOT_SW97;
		else if (std::string_view(gameStr) == "OOT")
			Globals::Instance->game = ZGame::OOT_RETAIL;
		else
		{
			std::string errorHeader =
				StringHelper::Sprintf("'Game' type '%s' is not supported.", gameStr);
			HANDLE_ERROR_PROCESS(WarningType::InvalidAttributeValue, errorHeader, "");
		}
	}

	if (reader->Attribute("BaseAddress") != nullptr)
		baseAddress = StringHelper::StrToL(reader->Attribute("BaseAddress"), 16);

	if (reader->Attribute("RangeStart") != nullptr)
		rangeStart = StringHelper::StrToL(reader->Attribute("RangeStart"), 16);

	if (reader->Attribute("RangeEnd") != nullptr)
		rangeEnd = StringHelper::StrToL(reader->Attribute("RangeEnd"), 16);

	if (reader->Attribute("Compilable") != nullptr)
		isCompilable = true;

	if (rangeStart > rangeEnd)
		HANDLE_ERROR_PROCESS(
			WarningType::Always,
			StringHelper::Sprintf("'RangeStart' 0x%06X must be before 'RangeEnd' 0x%06X",
		                          rangeStart, rangeEnd),
			"");

	const char* segmentXml = reader->Attribute("Segment");
	if (segmentXml != nullptr)
	{
		if (!StringHelper::HasOnlyDigits(segmentXml))
		{
			HANDLE_ERROR_PROCESS(WarningType::Always,
			                     StringHelper::Sprintf("error: Invalid segment value '%s': must be "
			                                           "a decimal between 0 and 15 inclusive",
			                                           segmentXml),
			                     "");
		}

		segment = StringHelper::StrToL(segmentXml, 10);
		if (segment > 15)
		{
			if (segment == 128)
			{
#ifdef DEPRECATION_ON
				HANDLE_WARNING_PROCESS(
					WarningType::Always, "warning: segment 128 is deprecated.",
					"Remove 'Segment=\"128\"' from the xml to use virtual addresses\n");
#endif
			}
			else
			{
				HANDLE_ERROR_PROCESS(
					WarningType::Always,
					StringHelper::Sprintf("error: invalid segment value '%s': must be a decimal "
				                          "number between 0 and 15 inclusive",
				                          segmentXml),
					"");
			}
		}
	}
	Globals::Instance->AddSegment(segment, this, workerID);

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
	{
		if (segment == 0x80)
		{
			printf("File '%s' using virtual addresses.\n", GetName().c_str());
		}
		else
		{
			printf("File '%s' using segment %X.\n", GetName().c_str(), segment);
		}
	}

	if (mode == ZFileMode::Extract || mode == ZFileMode::ExternalFile || mode == ZFileMode::ExtractDirectory)
	{
		if (Globals::Instance->fileMode != ZFileMode::ExtractDirectory)
		{
			if (!DiskFile::Exists((basePath / name).string()))
			{
				std::string errorHeader = StringHelper::Sprintf("binary file '%s' does not exist.",
				                                                (basePath / name).c_str());
				HANDLE_ERROR_PROCESS(WarningType::Always, errorHeader, "");
			}
		}

		if (Globals::Instance->fileMode == ZFileMode::ExtractDirectory)
			rawData = Globals::Instance->GetBaseromFile(name);
		else
			rawData = Globals::Instance->GetBaseromFile((basePath / name).string());

		if (reader->Attribute("RangeEnd") == nullptr)
			rangeEnd = rawData.size();
	}

	std::unordered_set<std::string> nameSet;
	std::unordered_set<std::string> outNameSet;
	std::unordered_set<std::string> offsetSet;

	auto nodeMap = *GetNodeMap();
	uint32_t rawDataIndex = 0;

	for (tinyxml2::XMLElement* child = reader->FirstChildElement(); child != nullptr;
	     child = child->NextSiblingElement())
	{
		const char* nameXml = child->Attribute("Name");
		const char* outNameXml = child->Attribute("OutName");
		const char* offsetXml = child->Attribute("Offset");

		// Check for repeated attributes.
		if (offsetXml != nullptr)
		{
			if (!StringHelper::IsValidOffset(std::string_view(offsetXml)))
			{
				HANDLE_ERROR(WarningType::InvalidXML,
				             StringHelper::Sprintf("Invalid offset %s entered", offsetXml), "");
			}
			rawDataIndex = strtol(offsetXml, NULL, 16);

			if (offsetSet.find(offsetXml) != offsetSet.end())
			{
				std::string errorHeader =
					StringHelper::Sprintf("repeated 'Offset' attribute: %s", offsetXml);
				HANDLE_ERROR_PROCESS(WarningType::InvalidXML, errorHeader, "");
			}
			offsetSet.insert(offsetXml);
		}
		else
		{
			HANDLE_WARNING_RESOURCE(WarningType::MissingOffsets, this, nullptr, rawDataIndex,
			                        StringHelper::Sprintf("no offset specified for %s.", nameXml),
			                        "");
		}

		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
			printf("%s: 0x%06X\n", nameXml, rawDataIndex);

		if (outNameXml != nullptr)
		{
			if (outNameSet.find(outNameXml) != outNameSet.end())
			{
				std::string errorHeader =
					StringHelper::Sprintf("repeated 'OutName' attribute: %s", outNameXml);
				HANDLE_ERROR_PROCESS(WarningType::InvalidXML, errorHeader, "");
			}
			outNameSet.insert(outNameXml);
		}
		if (nameXml != nullptr)
		{
			if (nameSet.find(nameXml) != nameSet.end())
			{
				std::string errorHeader =
					StringHelper::Sprintf("repeated 'Name' attribute: %s", nameXml);
				HANDLE_ERROR_PROCESS(WarningType::InvalidXML, errorHeader, "");
			}
			nameSet.insert(nameXml);
		}
		
		std::string nodeName = std::string(child->Name());

		if (nodeMap.find(nodeName) != nodeMap.end())
		{
			ZResource* nRes = nodeMap[nodeName](this);

			if (mode == ZFileMode::Extract || mode == ZFileMode::ExternalFile ||
			    mode == ZFileMode::ExtractDirectory)
			{
				if (!isCompilable)
					nRes->ExtractFromXML(child, rawDataIndex);
			}
			switch (nRes->GetResourceType())
			{
			case ZResourceType::Texture:
				AddTextureResource(rawDataIndex, static_cast<ZTexture*>(nRes));
				break;

			case ZResourceType::Symbol:
				AddSymbolResource(rawDataIndex, static_cast<ZSymbol*>(nRes));
				break;

			default:
				AddResource(nRes);
				break;
			}

			rawDataIndex += nRes->GetRawDataSize();
		}
		else if (std::string_view(child->Name()) == "File")
		{
			std::string errorHeader = "Can't declare a <File> inside a <File>";
			HANDLE_ERROR_PROCESS(WarningType::InvalidXML, errorHeader, "");
		}
		else
		{
			std::string errorHeader = StringHelper::Sprintf(
				"Unknown element found inside a <File> element: %s", nodeName.c_str());
			HANDLE_ERROR_PROCESS(WarningType::InvalidXML, errorHeader, "");
		}
	}
}

void ZFile::DeclareResourceSubReferences()
{
	for (size_t i = 0; i < resources.size(); i++)
	{
		resources.at(i)->DeclareReferences(name);
	}
}

void ZFile::BuildSourceFile()
{
	if (mode == ZFileMode::ExternalFile)
		return;

	if (!Directory::Exists(outputPath))
		Directory::CreateDirectory(outputPath.string());

	GenerateSourceFiles();
}

std::string ZFile::GetName() const
{
	return name;
}

std::string ZFile::GetOutName() const
{
	return outName.string();
}

ZFileMode ZFile::GetMode() const
{
	return mode;
}

const fs::path& ZFile::GetXmlFilePath() const
{
	return xmlFilePath;
}

const std::vector<uint8_t>& ZFile::GetRawData() const
{
	return rawData;
}

void ZFile::ExtractResources()
{
	if (mode == ZFileMode::ExternalFile)
		return;

	if (!Directory::Exists(outputPath))
		Directory::CreateDirectory(outputPath.string());

	if (!Directory::Exists(GetSourceOutputFolderPath()))
		Directory::CreateDirectory(GetSourceOutputFolderPath().string());

	for (size_t i = 0; i < resources.size(); i++)
		resources[i]->ParseRawDataLate();
	for (size_t i = 0; i < resources.size(); i++)
		resources[i]->DeclareReferencesLate(name);

	if (Globals::Instance->genSourceFile)
		GenerateSourceFiles();

	auto memStreamFile = std::shared_ptr<MemoryStream>(new MemoryStream());
	BinaryWriter writerFile = BinaryWriter(memStreamFile);

	ExporterSet* exporterSet = Globals::Instance->GetExporterSet();

	if (exporterSet != nullptr && exporterSet->beginFileFunc != nullptr)
		exporterSet->beginFileFunc(this);

	int totalMs = 0;

	for (ZResource* res : resources)
	{
		auto start = std::chrono::steady_clock::now();

		auto memStreamRes = std::shared_ptr<MemoryStream>(new MemoryStream());
		BinaryWriter writerRes = BinaryWriter(memStreamRes);

		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
			printf("Saving resource %s\n", res->GetName().c_str());

		res->Save(outputPath);

		// Check if we have an exporter "registered" for this resource type
		ZResourceExporter* exporter = Globals::Instance->GetExporter(res->GetResourceType());
		if (exporter != nullptr)
		{
			// exporter->Save(res, Globals::Instance->outputPath.string(), &writerFile);
			exporter->Save(res, Globals::Instance->outputPath.string(), &writerRes);
		}

		if (exporterSet != nullptr && exporterSet->resSaveFunc != nullptr)
			exporterSet->resSaveFunc(res, writerRes);

		auto end = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		totalMs += diff;

		//printf("Res %s in %lims\n", res->GetName().c_str(), diff);
	}

	//printf("File %s in %lims\n", GetName().c_str(), totalMs);

	if (memStreamFile->GetLength() > 0)
	{
		DiskFile::WriteAllBytes(StringHelper::Sprintf("%s%s.bin",
		                                          Globals::Instance->outputPath.string().c_str(),
		                                          GetName().c_str()),
		                    memStreamFile->ToVector());
	}

	writerFile.Close();

	if (exporterSet != nullptr && exporterSet->endFileFunc != nullptr)
		exporterSet->endFileFunc(this);
}

void ZFile::AddResource(ZResource* res)
{
	resources.push_back(res);
}

ZResource* ZFile::FindResource(offset_t rawDataIndex)
{
	for (ZResource* res : resources)
	{
		if (res->GetRawDataIndex() == rawDataIndex)
			return res;
	}

	return nullptr;
}

std::vector<ZResource*> ZFile::GetResourcesOfType(ZResourceType resType)
{
	std::vector<ZResource*> resList;
	resList.reserve(resources.size());

	for (ZResource* res : resources)
	{
		if (res->GetResourceType() == resType)
			resList.push_back(res);
	}

	return resList;
}

Declaration* ZFile::AddDeclaration(offset_t address, DeclarationAlignment alignment, size_t size,
                                   const std::string& varType, const std::string& varName,
                                   const std::string& body)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	Declaration* decl = GetDeclaration(address);
	if (decl == nullptr)
	{
		decl = new Declaration(address, alignment, size, varType, varName, false, body);
		declarations[address] = decl;
	}
	else
	{
		decl->alignment = alignment;
		decl->size = size;
		decl->varType = varType;
		decl->varName = varName;
		decl->text = body;
	}
	return decl;
}

Declaration* ZFile::AddDeclarationArray(offset_t address, DeclarationAlignment alignment,
                                        size_t size, const std::string& varType,
                                        const std::string& varName, size_t arrayItemCnt,
                                        const std::string& body)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	Declaration* decl = GetDeclaration(address);
	if (decl == nullptr)
	{
		decl =
			new Declaration(address, alignment, size, varType, varName, true, arrayItemCnt, body);
		declarations[address] = decl;
	}
	else
	{
		if (decl->isPlaceholder)
			decl->varName = varName;
		decl->alignment = alignment;
		decl->size = size;
		decl->varType = varType;
		decl->isArray = true;
		decl->arrayItemCnt = arrayItemCnt;
		decl->text = body;
	}

	return decl;
}

Declaration* ZFile::AddDeclarationArray(offset_t address, DeclarationAlignment alignment,
                                        size_t size, const std::string& varType,
                                        const std::string& varName,
                                        const std::string& arrayItemCntStr, const std::string& body)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	Declaration* decl = GetDeclaration(address);
	if (decl == nullptr)
	{
		decl = new Declaration(address, alignment, size, varType, varName, true, arrayItemCntStr,
		                       body);
		declarations[address] = decl;
	}
	else
	{
		decl->alignment = alignment;
		decl->size = size;
		decl->varType = varType;
		decl->varName = varName;
		decl->isArray = true;
		decl->arrayItemCntStr = arrayItemCntStr;
		decl->text = body;
	}
	return decl;
}

Declaration* ZFile::AddDeclarationPlaceholder(offset_t address, const std::string& varName)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	Declaration* decl;
	if (declarations.find(address) == declarations.end())
	{
		decl = new Declaration(address, DeclarationAlignment::Align4, 0, "", varName, false, "");
		decl->isPlaceholder = true;
		declarations[address] = decl;
	}
	else
		decl = declarations[address];

	return decl;
}

Declaration* ZFile::AddDeclarationInclude(offset_t address, const std::string& includePath,
                                          size_t size, const std::string& varType,
                                          const std::string& varName)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	Declaration* decl = GetDeclaration(address);
	if (decl == nullptr)
	{
		decl = new Declaration(address, includePath, size, varType, varName);
		declarations[address] = decl;
	}
	else
	{
		decl->includePath = includePath;
		decl->size = size;
		decl->varType = varType;
		decl->varName = varName;
	}
	return decl;
}

Declaration* ZFile::AddDeclarationIncludeArray(offset_t address, std::string& includePath,
                                               size_t size, const std::string& varType,
                                               const std::string& varName, size_t arrayItemCnt)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	if (StringHelper::StartsWith(includePath, "assets/extracted/"))
		includePath = "assets/" + StringHelper::Split(includePath, "assets/extracted/")[1];
	if (StringHelper::StartsWith(includePath, "assets/custom/"))
		includePath = "assets/" + StringHelper::Split(includePath, "assets/custom/")[1];

	Declaration* decl = GetDeclaration(address);
	if (decl == nullptr)
	{
		decl = new Declaration(address, includePath, size, varType, varName);

		decl->isArray = true;
		decl->arrayItemCnt = arrayItemCnt;

		declarations[address] = decl;
	}
	else
	{
		decl->includePath = includePath;
		decl->varType = varType;
		decl->varName = varName;
		decl->size = size;
		decl->isArray = true;
		decl->arrayItemCnt = arrayItemCnt;
	}
	return decl;
}

bool ZFile::AddDeclarationChecks(uint32_t address, const std::string& varName)
{
	assert(GETSEGNUM(address) == 0);
	assert(varName != "");
#ifdef DEVELOPMENT
	if (address == 0x0000)
	{
		[[maybe_unused]] int32_t bp = 0;
	}
#endif

	if (!IsOffsetInFileRange(address))
	{
		fprintf(stderr,
		        "%s: Warning in %s\n"
		        "\t Tried to declare a variable outside of this file's range. Ignoring...\n"
		        "\t\t Variable's name: %s\n"
		        "\t\t Variable's offset: 0x%06X\n",
		        __PRETTY_FUNCTION__, name.c_str(), varName.c_str(), address);
		return false;
	}

	return true;
}

bool ZFile::GetDeclarationPtrName(segptr_t segAddress, const std::string& expectedType,
                                  std::string& declName) const
{
	if (segAddress == 0)
	{
		declName = "NULL";
		return true;
	}

	uint32_t offset = Seg2Filespace(segAddress, baseAddress);
	Declaration* decl = GetDeclaration(offset);
	if (GETSEGNUM(segAddress) != segment || decl == nullptr)
	{
		declName = StringHelper::Sprintf("0x%08X", segAddress);
		return false;
	}

	if (expectedType != "" && expectedType != "void*")
	{
		if (expectedType != decl->varType && "static " + expectedType != decl->varType)
		{
			declName = StringHelper::Sprintf("0x%08X", segAddress);
			return false;
		}
	}

	if (!decl->isArray)
		declName = "&" + decl->varName;
	else
		declName = decl->varName;
	return true;
}

bool ZFile::GetDeclarationArrayIndexedName(segptr_t segAddress, size_t elementSize,
                                           const std::string& expectedType,
                                           std::string& declName) const
{
	if (segAddress == 0)
	{
		declName = "NULL";
		return true;
	}

	uint32_t address = Seg2Filespace(segAddress, baseAddress);
	Declaration* decl = GetDeclarationRanged(address);
	if (GETSEGNUM(segAddress) != segment || decl == nullptr || !decl->isArray)
	{
		declName = StringHelper::Sprintf("0x%08X", segAddress);
		return false;
	}

	if (expectedType != "" && expectedType != "void*")
	{
		if (expectedType != decl->varType && "static " + expectedType != decl->varType)
		{
			declName = StringHelper::Sprintf("0x%08X", segAddress);
			return false;
		}
	}

	if (decl->address == address)
	{
		declName = decl->varName;
		return true;
	}

	if ((address - decl->address) % elementSize != 0 || !(address < decl->address + decl->size))
	{
		declName = StringHelper::Sprintf("0x%08X", segAddress);
		return false;
	}

	uint32_t index = (address - decl->address) / elementSize;
	declName = StringHelper::Sprintf("&%s[%u]", decl->varName.c_str(), index);
	return true;
}

Declaration* ZFile::GetDeclaration(offset_t address) const
{
	if (declarations.find(address) != declarations.end())
		return declarations.at(address);

	return nullptr;
}

Declaration* ZFile::GetDeclarationRanged(offset_t address) const
{
	for (const auto decl : declarations)
	{
		if (address >= decl.first && address < decl.first + decl.second->size)
			return decl.second;
	}

	return nullptr;
}

bool ZFile::HasDeclaration(offset_t address)
{
	assert(GETSEGNUM(address) == 0);
	return declarations.find(address) != declarations.end();
}

void ZFile::GenerateSourceFiles()
{
	std::string sourceOutput;

	sourceOutput += "#include \"ultra64.h\"\n";
	sourceOutput += "#include \"z64.h\"\n";
	sourceOutput += "#include \"macros.h\"\n";
	sourceOutput += GetHeaderInclude();

	bool hasZRoom = false;
	for (const auto& res : resources)
	{
		ZResourceType resType = res->GetResourceType();
		if (resType == ZResourceType::Room || resType == ZResourceType::Scene ||
		    resType == ZResourceType::AltHeader)
		{
			hasZRoom = true;
			break;
		}
	}

	if (hasZRoom)
	{
		sourceOutput += GetZRoomHeaderInclude();
	}

	sourceOutput += GetExternalFileHeaderInclude();

	GeneratePlaceholderDeclarations();

	// Generate Code
	for (size_t i = 0; i < resources.size(); i++)
	{
		ZResource* res = resources.at(i);
		res->GetSourceOutputCode(name);
	}

	sourceOutput += ProcessDeclarations();

	fs::path outPath = GetSourceOutputFolderPath() / outName.stem().concat(".c");

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
		printf("Writing C file: %s\n", outPath.c_str());

	if (!Globals::Instance->otrMode)
	{
		OutputFormatter formatter;
		formatter.Write(sourceOutput);

		DiskFile::WriteAllText(outPath, formatter.GetOutput());
	}

	GenerateSourceHeaderFiles();
}

void ZFile::GenerateSourceHeaderFiles()
{
	OutputFormatter formatter;
	// Use parent folder and output name as guard as some headers have the same output name
	std::string guard = xmlFilePath.parent_path().stem().string() + "_" + outName.stem().string();

	std::transform(guard.begin(), guard.end(), guard.begin(), ::toupper);
	formatter.Write(
		StringHelper::Sprintf("#ifndef %s_H\n#define %s_H 1\n\n", guard.c_str(), guard.c_str()));
	formatter.Write("#include \"align_asset_macro.h\"\n");

	std::set<std::string> nameSet;
	for (ZResource* res : resources)
	{
		std::string resSrc = res->GetSourceOutputHeader("", &nameSet);
		if (!resSrc.empty()) 
		{
			formatter.Write(resSrc.front() == '\n' ? resSrc : "\n" + resSrc);
			formatter.Write(res == resources.back() ? "" : "\n");
		}
	}

	for (auto& sym : symbolResources)
	{
		formatter.Write("\n\n");
		formatter.Write(sym.second->GetSourceOutputHeader("", &nameSet));
	}

	formatter.Write(ProcessExterns());

	formatter.Write(StringHelper::Sprintf("\n#endif // %s_H\n", guard.c_str()));

	fs::path headerFilename = GetSourceOutputFolderPath() / outName.stem().concat(".h");

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
		printf("Writing H file: %s\n", headerFilename.c_str());

	std::string output = formatter.GetOutput();

	if (Globals::Instance->fileMode != ZFileMode::ExtractDirectory)
		DiskFile::WriteAllText(headerFilename, output);
	else if (Globals::Instance->sourceOutputPath != "")
	{
		std::string xmlPath = xmlFilePath.string();
		xmlPath = StringHelper::Replace(xmlPath, "\\", "/");
		auto pathList = StringHelper::Split(xmlPath, "/");
		std::string outPath = "";

		for (int i = 0; i < 3; i++)
			outPath += pathList[i] + "/";

		for (int i = 5; i < pathList.size(); i++)
		{
			if (i == pathList.size() - 1)
			{
				outPath += Path::GetFileNameWithoutExtension(pathList[i]) + "/";
				outPath += outName.string() + ".h";
			}
			else
				outPath += pathList[i];

			if (i < pathList.size() - 1)
				outPath += "/";
		}

		DiskFile::WriteAllText(outPath, output);
	}
}

std::string ZFile::GetHeaderInclude() const
{
	std::string headers = StringHelper::Sprintf("#include \"%s.h\"\n",
	                                            (outName.parent_path() / outName.stem()).string().c_str());

	return headers;
}

std::string ZFile::GetZRoomHeaderInclude() const
{
	std::string headers;
	headers += "#include \"segment_symbols.h\"\n";
	headers += "#include \"command_macros_base.h\"\n";
	headers += "#include \"z64cutscene_commands.h\"\n";
	headers += "#include \"variables.h\"\n";
	return headers;
}

std::string ZFile::GetExternalFileHeaderInclude() const
{
	std::string externalFilesIncludes = "";

	for (ZFile* externalFile : Globals::Instance->files)
	{
		if (externalFile != this)
		{
			fs::path outputFolderPath = externalFile->GetSourceOutputFolderPath();
			if (outputFolderPath == this->GetSourceOutputFolderPath())
			{
				outputFolderPath = externalFile->outName.stem();
			}
			else
			{
				outputFolderPath /= externalFile->outName.stem();
			}

			externalFilesIncludes +=
				StringHelper::Sprintf("#include \"%s.h\"\n", outputFolderPath.string().c_str());
		}
	}

	return externalFilesIncludes;
}

void ZFile::GeneratePlaceholderDeclarations()
{
	// Generate placeholder declarations
	for (ZResource* res : resources)
	{
		if (GetDeclaration(res->GetRawDataIndex()) != nullptr)
		{
			continue;
		}

		Declaration* decl = res->DeclareVar(GetName(), "");

		if (decl != nullptr)
		{
			decl->staticConf = res->GetStaticConf();
			if (res->GetResourceType() == ZResourceType::Symbol)
			{
				decl->staticConf = StaticConfig::Off;
			}
		}
	}
}

void ZFile::AddTextureResource(uint32_t offset, ZTexture* tex)
{
	for (auto res : resources)
		assert(res->GetRawDataIndex() != offset);

	resources.push_back(tex);
	texturesResources[offset] = tex;
}

ZTexture* ZFile::GetTextureResource(uint32_t offset) const
{
	auto tex = texturesResources.find(offset);
	if (tex != texturesResources.end())
		return tex->second;

	return nullptr;
}

void ZFile::AddSymbolResource(uint32_t offset, ZSymbol* sym)
{
	symbolResources[offset] = sym;
}

ZSymbol* ZFile::GetSymbolResource(uint32_t offset) const
{
	auto sym = symbolResources.find(offset);
	if (sym != symbolResources.end())
		return sym->second;

	return nullptr;
}

ZSymbol* ZFile::GetSymbolResourceRanged(uint32_t offset) const
{
	for (const auto decl : symbolResources)
	{
		if (offset >= decl.first && offset < decl.first + decl.second->GetRawDataSize())
			return decl.second;
	}

	return nullptr;
}

fs::path ZFile::GetSourceOutputFolderPath() const
{
	return outputPath / outName.parent_path();
}

bool ZFile::IsOffsetInFileRange(uint32_t offset) const
{
	if (!(offset < rawData.size()))
		return false;

	return rangeStart <= offset && offset < rangeEnd;
}
bool ZFile::IsSegmentedInFilespaceRange(segptr_t segAddress) const
{
	uint8_t segnum = GETSEGNUM(segAddress);
	uint32_t offset = Seg2Filespace(segAddress, baseAddress);

	if (segment != segnum)
		return false;

	return IsOffsetInFileRange(offset);
}

std::map<std::string, ZResourceFactoryFunc*>* ZFile::GetNodeMap()
{
	static std::map<std::string, ZResourceFactoryFunc*> nodeMap;
	return &nodeMap;
}

void ZFile::RegisterNode(std::string nodeName, ZResourceFactoryFunc* nodeFunc)
{
	std::map<std::string, ZResourceFactoryFunc*>* nodeMap = GetNodeMap();
	(*nodeMap)[nodeName] = nodeFunc;
}

std::string ZFile::ProcessDeclarations()
{
	std::string output;

	if (declarations.size() == 0)
		return output;

	defines += ProcessTextureIntersections(name);

	// printf("RANGE START: 0x%06X - RANGE END: 0x%06X\n", rangeStart, rangeEnd);

	// Optimization: See if there are any arrays side by side that can be merged...
	std::vector<std::pair<int32_t, Declaration*>> declarationKeys(declarations.begin(),
	                                                              declarations.end());

	std::pair<int32_t, Declaration*> lastItem = declarationKeys.at(0);

	for (size_t i = 1; i < declarationKeys.size(); i++)
	{
		std::pair<int32_t, Declaration*> curItem = declarationKeys[i];

		if (curItem.second->isArray && lastItem.second->isArray)
		{
			if (curItem.second->varType == lastItem.second->varType)
			{
				if (!curItem.second->declaredInXml && !lastItem.second->declaredInXml)
				{
					// TEST: For now just do Vtx declarations...
					if (lastItem.second->varType == "Vtx")
					{
						int32_t sizeDiff = curItem.first - (lastItem.first + lastItem.second->size);

						// Make sure there isn't an unaccounted inbetween these two
						if (sizeDiff == 0)
						{
							lastItem.second->size += curItem.second->size;
							lastItem.second->arrayItemCnt += curItem.second->arrayItemCnt;
							lastItem.second->text += "\n" + curItem.second->text;

							for (auto vtx : curItem.second->vertexHack)
								lastItem.second->vertexHack.push_back(vtx);

							declarations.erase(curItem.first);
							declarationKeys.erase(declarationKeys.begin() + i);
							delete curItem.second;
							i--;
							continue;
						}
					}
				}
			}
		}

		lastItem = curItem;
	}

	for (std::pair<uint32_t, Declaration*> item : declarations)
		ProcessDeclarationText(item.second);

	for (std::pair<uint32_t, Declaration*> item : declarations)
	{
		while (item.second->size % 4 != 0)
			item.second->size++;
	}

	HandleUnaccountedData();

	// Go through include declarations
	// First, handle the prototypes (static only for now)
	for (std::pair<uint32_t, Declaration*> item : declarations)
	{
		output += item.second->GetStaticForwardDeclarationStr();
	}

	output += "\n";

	// Next, output the actual declarations
	for (const auto& item : declarations)
	{
		if (!IsOffsetInFileRange(item.first))
			continue;

		if (item.second->includePath != "")
		{
			if (item.second->isExternal)
			{
				if (!Globals::Instance->otrMode)
				{
					// HACK
					std::string extType;

					if (item.second->varType == "Gfx")
						extType = "dlist";
					else if (item.second->varType == "Vtx")
						extType = "vtx";

					auto filepath = outputPath / item.second->varName;
					DiskFile::WriteAllText(
						StringHelper::Sprintf("%s.%s.inc", filepath.string().c_str(), extType.c_str()),
						item.second->text);
				}
			}

			output += item.second->GetExternalDeclarationStr();
		}
		else if (item.second->varType != "")
		{
			output += item.second->GetNormalDeclarationStr();
		}
	}

	return output;
}

void ZFile::ProcessDeclarationText(Declaration* decl)
{
	size_t refIndex = 0;

	if (!(decl->references.size() > 0))
		return;

	for (size_t i = 0; i < decl->text.size() - 1; i++)
	{
		char c = decl->text[i];
		char c2 = decl->text[i + 1];

		if (c == '@' && c2 == 'r')
		{
			std::string vtxName;
			Globals::Instance->GetSegmentedArrayIndexedName(decl->references[refIndex], 0x10, this,
			                                                "Vtx", vtxName, workerID);
			decl->text.replace(i, 2, vtxName);

			refIndex++;

			if (refIndex >= decl->references.size())
				break;
		}
	}
}

std::string ZFile::ProcessExterns()
{
	std::string output;

	for (const auto& item : declarations)
	{
		if (!IsOffsetInFileRange(item.first))
		{
			continue;
		}

		output += item.second->GetExternStr();
	}

	output += "\n";

	output += defines;

	return output;
}

std::string ZFile::ProcessTextureIntersections([[maybe_unused]] const std::string& prefix)
{
	if (texturesResources.empty())
		return "";

	std::string defines;
	std::vector<std::pair<uint32_t, ZTexture*>> texturesSorted(texturesResources.begin(),
	                                                           texturesResources.end());

	for (size_t i = 0; i < texturesSorted.size() - 1; i++)
	{
		uint32_t currentOffset = texturesSorted[i].first;
		uint32_t nextOffset = texturesSorted[i + 1].first;
		auto& currentTex = texturesResources.at(currentOffset);
		int texSize = currentTex->GetRawDataSize();

		if (currentTex->WasDeclaredInXml())
		{
			// We believe the user is right.
			continue;
		}

		if ((currentOffset + texSize) > nextOffset)
		{
			uint32_t offsetDiff = nextOffset - currentOffset;
			if (currentTex->isPalette)
			{
				// Shrink palette so it doesn't overlap
				currentTex->SetDimensions(offsetDiff / currentTex->GetPixelMultiplyer(), 1);

				if (declarations.find(currentOffset) != declarations.end())
					declarations.at(currentOffset)->size = currentTex->GetRawDataSize();

				currentTex->DeclareVar(GetName(), "");
			}
			else
			{
				std::string texName;
				std::string texNextName;
				GetDeclarationPtrName(currentOffset, "", texName);

				Declaration* nextDecl = GetDeclaration(nextOffset);
				if (nextDecl == nullptr)
					texNextName = texturesResources.at(nextOffset)->GetName();
				else
					texNextName = nextDecl->varName;

#if 0
				defines += StringHelper::Sprintf("#define %s ((u32)%s + 0x%06X)\n",
				                                 texNextName.c_str(), texName.c_str(), offsetDiff);
#endif

				delete declarations[nextOffset];
				declarations.erase(nextOffset);
				texturesResources.erase(nextOffset);
				texturesSorted.erase(texturesSorted.begin() + i + 1);

				i--;
			}
		}
	}

	return defines;
}

void ZFile::HandleUnaccountedData()
{
	uint32_t lastAddr = 0;
	uint32_t lastSize = 0;
	std::vector<offset_t> declsAddresses;

	if (Globals::Instance->otrMode)
		return;

	declsAddresses.reserve(declarations.size());
	if (Globals::Instance->otrMode)
		return;

	for (const auto& item : declarations)
	{
		declsAddresses.push_back(item.first);
	}

	bool breakLoop = false;
	for (offset_t currentAddress : declsAddresses)
	{
		if (currentAddress >= rangeEnd)
		{
			breakLoop = true;
			break;
		}

		if (currentAddress < rangeStart)
		{
			lastAddr = currentAddress;
			continue;
		}

		breakLoop = HandleUnaccountedAddress(currentAddress, lastAddr, lastSize);
		if (breakLoop)
			break;

		lastAddr = currentAddress;
	}

	if (!breakLoop)
	{
		// TODO: change rawData.size() to rangeEnd
		// HandleUnaccountedAddress(rangeEnd, lastAddr, lastSize);
		HandleUnaccountedAddress(rawData.size(), lastAddr, lastSize);
	}
}

bool ZFile::HandleUnaccountedAddress(offset_t currentAddress, offset_t lastAddr, uint32_t& lastSize)
{
	if (currentAddress != lastAddr && declarations.find(lastAddr) != declarations.end())
	{
		Declaration* lastDecl = declarations.at(lastAddr);
		lastSize = lastDecl->size;

		if (lastAddr + lastSize > currentAddress)
		{
			Declaration* currentDecl = declarations.at(currentAddress);

			std::string intersectionInfo = StringHelper::Sprintf(
				"Resource from 0x%06X:0x%06X (%s) conflicts with 0x%06X (%s).", lastAddr,
				lastAddr + lastSize, lastDecl->varName.c_str(), currentAddress,
				currentDecl->varName.c_str());
			HANDLE_WARNING_RESOURCE(WarningType::Intersection, this, nullptr, currentAddress,
			                        "intersection detected", intersectionInfo);
		}
	}

	uint32_t unaccountedAddress = lastAddr + lastSize;

	if (unaccountedAddress != currentAddress && lastAddr >= rangeStart &&
	    unaccountedAddress < rangeEnd)
	{
		int diff = currentAddress - unaccountedAddress;
		bool nonZeroUnaccounted = false;

		std::string src = "    ";

		if (currentAddress > rawData.size())
		{
			throw std::runtime_error(StringHelper::Sprintf(
				"ZFile::ProcessDeclarations(): Fatal error while processing XML '%s'.\n"
				"\t Offset '0x%X' is outside of the limits of file '%s', which has a size of "
				"'0x%X'.\n"
				"\t Aborting...",
				xmlFilePath.c_str(), currentAddress, name.c_str(), rawData.size()));
		}

		// Handle Align8
		if (currentAddress % 8 == 0 && diff % 8 != 0)
		{
			Declaration* currentDecl = GetDeclaration(currentAddress);

			if (currentDecl != nullptr)
			{
				if (currentDecl->alignment == DeclarationAlignment::Align8)
				{
					// Check removed bytes are zeroes
					if (BitConverter::ToUInt32BE(rawData, unaccountedAddress + diff - 4) == 0)
					{
						diff -= 4;
					}
				}

				if (diff == 0)
				{
					return false;
				}
			}
		}

		for (int i = 0; i < diff; i++)
		{
			uint8_t val = rawData.at(unaccountedAddress + i);
			src += StringHelper::Sprintf("0x%02X, ", val);
			if (val != 0x00)
			{
				nonZeroUnaccounted = true;
			}

			if (Globals::Instance->verboseUnaccounted)
			{
				if ((i % 4 == 3))
				{
					src += StringHelper::Sprintf(" // 0x%06X", unaccountedAddress + i - 3);
					if (i != (diff - 1))
					{
						src += "\n\t";
					}
				}
			}
			else
			{
				if ((i % 16 == 15) && (i != (diff - 1)))
					src += "\n    ";
			}
		}

		if (declarations.find(unaccountedAddress) == declarations.end() && diff > 0)
		{
			std::string unaccountedPrefix = "unaccounted";

			if (diff < 16 && !nonZeroUnaccounted)
			{
				unaccountedPrefix = "possiblePadding";

				// Strip unnecessary padding at the end of the file.
				if (unaccountedAddress + diff >= rawData.size())
					return true;
			}

			Declaration* decl = AddDeclarationArray(
				unaccountedAddress, DeclarationAlignment::Align4, diff, "u8",
				StringHelper::Sprintf("%s_%s_%06X", name.c_str(), unaccountedPrefix.c_str(),
			                          unaccountedAddress),
				diff, src);

			decl->isUnaccounted = true;
			if (Globals::Instance->forceUnaccountedStatic)
				decl->staticConf = StaticConfig::On;

			if (nonZeroUnaccounted)
			{
				HANDLE_WARNING_RESOURCE(WarningType::Unaccounted, this, nullptr, unaccountedAddress,
				                        "a non-zero unaccounted block was found",
				                        StringHelper::Sprintf("Block size: '0x%X'", diff));
			}
			else if (diff >= 16)
			{
				HANDLE_WARNING_RESOURCE(WarningType::Unaccounted, this, nullptr, unaccountedAddress,
				                        "a big (size>=0x10) zero-only unaccounted block was found",
				                        StringHelper::Sprintf("Block size: '0x%X'", diff));
			}
		}
	}

	return false;
}
