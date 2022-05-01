#include <Archive.h>
#include "BackgroundExporter.h"
#include "TextureExporter.h"
#include "RoomExporter.h"
#include "CollisionExporter.h"
#include "DisplayListExporter.h"
#include "PlayerAnimationExporter.h"
#include "SkeletonExporter.h"
#include "SkeletonLimbExporter.h"
#include "ArrayExporter.h"
#include "VtxExporter.h"
#include "AnimationExporter.h"
#include "CutsceneExporter.h"
#include "PathExporter.h"
#include "TextExporter.h"
#include "BlobExporter.h"
#include "MtxExporter.h"
#include <Globals.h>
#include <Utils/File.h>
#include <Utils/Directory.h>
#include <Utils/MemoryStream.h>
#include <Utils/BinaryWriter.h>

std::string otrFileName = "oot.otr";
std::shared_ptr<Ship::Archive> otrArchive;
BinaryWriter* fileWriter;
std::chrono::steady_clock::time_point fileStart, resStart;
std::map<std::string, std::vector<char>> files;

void InitVersionInfo();

enum class ExporterFileMode
{
	BuildOTR = (int)ZFileMode::Custom + 1,
};

static void ExporterParseFileMode(const std::string& buildMode, ZFileMode& fileMode)
{
	if (buildMode == "botr")
	{
		fileMode = (ZFileMode)ExporterFileMode::BuildOTR;

		printf("BOTR: Generating OTR Archive...\n");

		if (File::Exists(otrFileName))
			otrArchive = std::shared_ptr<Ship::Archive>(new Ship::Archive(otrFileName, true));
		else
			otrArchive = Ship::Archive::CreateArchive(otrFileName, 65536 / 2);

		auto lst = Directory::ListFiles("Extract");

		for (auto item : lst)
		{
			auto fileData = File::ReadAllBytes(item);
			otrArchive->AddFile(StringHelper::Split(item, "Extract/")[1], (uintptr_t)fileData.data(), fileData.size());
		}
	}
}

static void ExporterProgramEnd()
{
	if (Globals::Instance->fileMode == ZFileMode::ExtractDirectory)
	{
		printf("Generating OTR Archive...\n");
		otrArchive = Ship::Archive::CreateArchive(otrFileName, 65536 / 2);

		for (auto item : files)
		{
			auto fileData = item.second;
			otrArchive->AddFile(item.first, (uintptr_t)fileData.data(), fileData.size());
		}

		// Add any additional files that need to be manually copied...
		auto lst = Directory::ListFiles("Extract");

		for (auto item : lst)
		{
			auto fileData = File::ReadAllBytes(item);
			otrArchive->AddFile(StringHelper::Split(item, "Extract/")[1], (uintptr_t)fileData.data(), fileData.size());
		}

		otrArchive->AddFile("Audiobank", (uintptr_t)Globals::Instance->GetBaseromFile("Audiobank").data(), Globals::Instance->GetBaseromFile("Audiobank").size());
		otrArchive->AddFile("Audioseq", (uintptr_t)Globals::Instance->GetBaseromFile("Audioseq").data(), Globals::Instance->GetBaseromFile("Audioseq").size());
		otrArchive->AddFile("Audiotable", (uintptr_t)Globals::Instance->GetBaseromFile("Audiotable").data(), Globals::Instance->GetBaseromFile("Audiotable").size());
	}
}


static void ExporterParseArgs(int argc, char* argv[], int& i)
{
	std::string arg = argv[i];

	if (arg == "--otrfile")
	{
		otrFileName = argv[i + 1];
		i++;
	}
}

static bool ExporterProcessFileMode(ZFileMode fileMode)
{
	// Do whatever work is associated with these custom file modes...
	// Return true to indicate one of our own file modes is being processed
	if (fileMode == (ZFileMode)ExporterFileMode::BuildOTR)
		return true;

	return false;
}

static void ExporterFileBegin(ZFile* file)
{
	fileStart = std::chrono::steady_clock::now();

	MemoryStream* stream = new MemoryStream();
	fileWriter = new BinaryWriter(stream);
}

static void ExporterFileEnd(ZFile* file)
{
	// delete fileWriter;
}

static void ExporterResourceEnd(ZResource* res, BinaryWriter& writer)
{
	auto streamShared = writer.GetStream();
	MemoryStream* strem = (MemoryStream*)streamShared.get();

	auto start = std::chrono::steady_clock::now();

	if (res->GetName() != "")
	{
		std::string oName = res->parent->GetOutName();
		std::string rName = res->GetName();
		std::string prefix = OTRExporter_DisplayList::GetPrefix(res);

		//auto xmlFilePath = res->parent->GetXmlFilePath();
		//prefix = StringHelper::Split(StringHelper::Split(xmlFilePath.string(), "xml\\")[1], ".xml")[0];

		if (StringHelper::Contains(oName, "_scene"))
		{
			auto split = StringHelper::Split(oName, "_");
			oName = "";
			for (size_t i = 0; i < split.size() - 1; i++)
				oName += split[i] + "_";

			oName += "scene";
		}
		else if (StringHelper::Contains(oName, "_room"))
		{
			oName = StringHelper::Split(oName, "_room")[0] + "_scene";
		}

		std::string fName = "";

		if (prefix != "")
			fName = StringHelper::Sprintf("%s/%s/%s", prefix.c_str(), oName.c_str(), rName.c_str());
		else
			fName = StringHelper::Sprintf("%s/%s", oName.c_str(), rName.c_str());

		if (Globals::Instance->fileMode == ZFileMode::ExtractDirectory)
			files[fName] = strem->ToVector();
		else
			File::WriteAllBytes("Extract/" + fName, strem->ToVector());
	}

	auto end = std::chrono::steady_clock::now();
	size_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	//if (diff > 10)
		//printf("Exported Resource End %s in %zums\n", res->GetName().c_str(), diff);
}

static void ExporterXMLBegin()
{
}

static void ExporterXMLEnd()
{
}

static void ImportExporters()
{
	// In this example we set up a new exporter called "EXAMPLE".
	// By running ZAPD with the argument -se EXAMPLE, we tell it that we want to use this exporter for our resources.
	ExporterSet* exporterSet = new ExporterSet();
	exporterSet->processFileModeFunc = ExporterProcessFileMode;
	exporterSet->parseFileModeFunc = ExporterParseFileMode;
	exporterSet->parseArgsFunc = ExporterParseArgs;
	exporterSet->beginFileFunc = ExporterFileBegin;
	exporterSet->endFileFunc = ExporterFileEnd;
	exporterSet->beginXMLFunc = ExporterXMLBegin;
	exporterSet->endXMLFunc = ExporterXMLEnd;
	exporterSet->resSaveFunc = ExporterResourceEnd;
	exporterSet->endProgramFunc = ExporterProgramEnd;

	exporterSet->exporters[ZResourceType::Background] = new OTRExporter_Background();
	exporterSet->exporters[ZResourceType::Texture] = new OTRExporter_Texture();
	exporterSet->exporters[ZResourceType::Room] = new OTRExporter_Room();
	exporterSet->exporters[ZResourceType::AltHeader] = new OTRExporter_Room();
	exporterSet->exporters[ZResourceType::Scene] = new OTRExporter_Room();
	exporterSet->exporters[ZResourceType::CollisionHeader] = new OTRExporter_Collision();
	exporterSet->exporters[ZResourceType::DisplayList] = new OTRExporter_DisplayList();
	exporterSet->exporters[ZResourceType::PlayerAnimationData] = new OTRExporter_PlayerAnimationExporter();
	exporterSet->exporters[ZResourceType::Skeleton] = new OTRExporter_Skeleton();
	exporterSet->exporters[ZResourceType::Limb] = new OTRExporter_SkeletonLimb();
	exporterSet->exporters[ZResourceType::Animation] = new OTRExporter_Animation();
	exporterSet->exporters[ZResourceType::Cutscene] = new OTRExporter_Cutscene();
	exporterSet->exporters[ZResourceType::Vertex] = new OTRExporter_Vtx();
	exporterSet->exporters[ZResourceType::Array] = new OTRExporter_Array();
	exporterSet->exporters[ZResourceType::Path] = new OTRExporter_Path();
	exporterSet->exporters[ZResourceType::Text] = new OTRExporter_Text();
	exporterSet->exporters[ZResourceType::Blob] = new OTRExporter_Blob();
	exporterSet->exporters[ZResourceType::Mtx] = new OTRExporter_MtxExporter();

	Globals::AddExporter("OTR", exporterSet);

	InitVersionInfo();
}

// When ZAPD starts up, it will automatically call the below function, which in turn sets up our exporters.
REGISTER_EXPORTER(ImportExporters);
