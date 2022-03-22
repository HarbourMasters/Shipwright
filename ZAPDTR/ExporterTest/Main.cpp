#include "CollisionExporter.h"
#include "Globals.h"
#include "RoomExporter.h"
#include "TextureExporter.h"

enum class ExporterFileMode
{
	ModeExample1 = (int)ZFileMode::Custom + 1,
	ModeExample2 = (int)ZFileMode::Custom + 2,
	ModeExample3 = (int)ZFileMode::Custom + 3,
};

static void ExporterParseFileMode(const std::string& buildMode, ZFileMode& fileMode)
{
	if (buildMode == "me1")
		fileMode = (ZFileMode)ExporterFileMode::ModeExample1;
	else if (buildMode == "me2")
		fileMode = (ZFileMode)ExporterFileMode::ModeExample2;
	else if (buildMode == "me3")
		fileMode = (ZFileMode)ExporterFileMode::ModeExample3;
}

static void ExporterParseArgs([[maybe_unused]] int argc, char* argv[], int& i)
{
	std::string arg = argv[i];

	if (arg == "--do-x")
	{
	}
	else if (arg == "--do-y")
	{
	}
}

static bool ExporterProcessFileMode(ZFileMode fileMode)
{
	// Do whatever work is associated with these custom file modes...
	// Return true to indicate one of our own file modes is being processed
	if (fileMode == (ZFileMode)ExporterFileMode::ModeExample1)
		return true;
	else if (fileMode == (ZFileMode)ExporterFileMode::ModeExample2)
		return true;
	else if (fileMode == (ZFileMode)ExporterFileMode::ModeExample3)
		return true;

	return false;
}

static void ExporterFileBegin(ZFile* file)
{
	printf("ExporterFileBegin() called on ZFile %s.\n", file->GetName().c_str());
}

static void ExporterFileEnd(ZFile* file)
{
	printf("ExporterFileEnd() called on ZFile %s.\n", file->GetName().c_str());
}

static void ImportExporters()
{
	// In this example we set up a new exporter called "EXAMPLE".
	// By running ZAPD with the argument -se EXAMPLE, we tell it that we want to use this exporter
	// for our resources.
	ExporterSet* exporterSet = new ExporterSet();
	exporterSet->processFileModeFunc = ExporterProcessFileMode;
	exporterSet->parseFileModeFunc = ExporterParseFileMode;
	exporterSet->parseArgsFunc = ExporterParseArgs;
	exporterSet->beginFileFunc = ExporterFileBegin;
	exporterSet->endFileFunc = ExporterFileEnd;
	exporterSet->exporters[ZResourceType::Texture] = new ExporterExample_Texture();
	exporterSet->exporters[ZResourceType::Room] = new ExporterExample_Room();
	exporterSet->exporters[ZResourceType::CollisionHeader] = new ExporterExample_Collision();

	Globals::AddExporter("EXAMPLE", exporterSet);
}

// When ZAPD starts up, it will automatically call the below function, which in turn sets up our
// exporters.
REGISTER_EXPORTER(ImportExporters);
