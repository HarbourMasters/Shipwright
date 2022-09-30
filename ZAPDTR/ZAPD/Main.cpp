#include "Globals.h"
#include "Overlays/ZOverlay.h"
#include "Utils/Directory.h"
#include "Utils/File.h"
#include "Utils/Path.h"
#include "WarningHandler.h"
#include "ZAnimation.h"
#include "ZBackground.h"
#include "ZBlob.h"
#include "ZFile.h"
#include "ZTexture.h"

#ifdef __linux__
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <cxxabi.h>  // for __cxa_demangle
#include <dlfcn.h>   // for dladdr
#include <execinfo.h>
#include <unistd.h>
#endif

#include <string>
#include <string_view>
#include "tinyxml2.h"
#include <ctpl_stl.h>

//extern const char gBuildHash[];
const char gBuildHash[] = "";

// LINUX_TODO: remove, those are because of soh <-> lus dependency problems
float divisor_num = 0.0f;

extern "C" void Audio_SetGameVolume(int player_id, float volume)
{

}


extern "C" int ResourceMgr_OTRSigCheck(char* imgData)
{
	return 0;
}

void DebugConsole_SaveCVars()
{

}

void DebugConsole_LoadCVars()
{

}


bool Parse(const fs::path& xmlFilePath, const fs::path& basePath, const fs::path& outPath,
           ZFileMode fileMode, int workerID);

void BuildAssetTexture(const fs::path& pngFilePath, TextureType texType, const fs::path& outPath);
void BuildAssetBackground(const fs::path& imageFilePath, const fs::path& outPath);
void BuildAssetBlob(const fs::path& blobFilePath, const fs::path& outPath);
int ExtractFunc(int workerID, int fileListSize, std::string fileListItem, ZFileMode fileMode);

volatile int numWorkersLeft = 0;

#ifdef __linux__
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof(arr[0]))
void ErrorHandler(int sig)
{
	void* array[4096];
	const size_t nMaxFrames = sizeof(array) / sizeof(array[0]);
	size_t size = backtrace(array, nMaxFrames);
	char** symbols = backtrace_symbols(array, nMaxFrames);

	fprintf(stderr, "\nZAPD crashed. (Signal: %i)\n", sig);

	// Feel free to add more crash messages.
	const char* crashEasterEgg[] = {
		"\tYou've met with a terrible fate, haven't you?",
		"\tSEA BEARS FOAM. SLEEP BEARS DREAMS. \n\tBOTH END IN THE SAME WAY: CRASSSH!",
		"\tZAPD has fallen and cannot get up.",
	};

	srand(time(nullptr));
	auto easterIndex = rand() % ARRAY_COUNT(crashEasterEgg);

	fprintf(stderr, "\n%s\n\n", crashEasterEgg[easterIndex]);

	fprintf(stderr, "Traceback:\n");
	for (size_t i = 1; i < size; i++)
	{
		Dl_info info;
		uint32_t gotAddress = dladdr(array[i], &info);
		std::string functionName(symbols[i]);

		if (gotAddress != 0 && info.dli_sname != nullptr)
		{
			int32_t status;
			char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
			const char* nameFound = info.dli_sname;

			if (status == 0)
			{
				nameFound = demangled;
			}

			functionName = StringHelper::Sprintf("%s (+0x%X)", nameFound,
			                                     (char*)array[i] - (char*)info.dli_saddr);
			free(demangled);
		}

		fprintf(stderr, "%-3zd %s\n", i, functionName.c_str());
	}

	fprintf(stderr, "\n");

	free(symbols);
	exit(1);
}
#endif

int main(int argc, char* argv[])
{
	// Syntax: ZAPD.out [mode (btex/bovl/e)] (Arbritrary Number of Arguments)

	if (argc < 2)
	{
		printf("ZAPD.out (%s) [mode (btex/bovl/bsf/bblb/bmdlintr/bamnintr/e)] ...\n", gBuildHash);
		return 1;
	}

	Globals* g = new Globals();
	WarningHandler::Init(argc, argv);

	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--version"))
		{
			printf("ZAPD.out %s\n", gBuildHash);
			return 0;
		}
		else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
		{
			printf("Congratulations!\n");
			printf("You just found the (unimplemented and undocumented) ZAPD's help message.\n");
			printf("Feel free to implement it if you want :D\n");

			WarningHandler::PrintHelp();
			return 0;
		}
	}

	// Parse other "commands"
	for (int32_t i = 2; i < argc; i++)
	{
		std::string arg = argv[i];

		if (arg == "-o" || arg == "--outputpath")  // Set output path
		{
			Globals::Instance->outputPath = argv[++i];

			if (Globals::Instance->sourceOutputPath == "")
				Globals::Instance->sourceOutputPath = Globals::Instance->outputPath;
		}
		else if (arg == "-i" || arg == "--inputpath")  // Set input path
		{
			Globals::Instance->inputPath = argv[++i];
		}
		else if (arg == "-b" || arg == "--baserompath")  // Set baserom path
		{
			Globals::Instance->baseRomPath = argv[++i];
		}
		else if (arg == "-osf")  // Set source output path
		{
			Globals::Instance->sourceOutputPath = argv[++i];
		}
		else if (arg == "-gsf")  // Generate source file during extraction
		{
			Globals::Instance->genSourceFile = std::string_view(argv[++i]) == "1";
		}
		else if (arg == "-tm")  // Test Mode (enables certain experimental features)
		{
			Globals::Instance->testMode = std::string_view(argv[++i]) == "1";
		}
		else if (arg == "-crc" ||
		         arg == "--output-crc")  // Outputs a CRC file for each extracted texture.
		{
			Globals::Instance->testMode = std::string_view(argv[++i]) == "1";
		}
		else if (arg == "-ulzdl")  // Use Legacy ZDisplay List
		{
			Globals::Instance->useLegacyZDList = std::string_view(argv[++i]) == "1";
		}
		else if (arg == "-profile")  // Enable profiling
		{
			Globals::Instance->profile = std::string_view(argv[++i]) == "1";
		}
		else if (arg ==
		         "-uer")  // Split resources into their individual components (enabled by default)
		                  // TODO: We may wish to make this a part of the config file...
		{
			Globals::Instance->useExternalResources = std::string_view(argv[++i]) == "1";
		}
		else if (arg == "-tt")  // Set texture type
		{
			Globals::Instance->texType = ZTexture::GetTextureTypeFromString(argv[++i]);
		}
		else if (arg == "-cfg")  // Set cfg path (for overlays)
		                         // TODO: Change the name of this to something else so it doesn't
		                         // get confused with XML config files.
		{
			Globals::Instance->cfgPath = argv[++i];
		}
		else if (arg == "-fl")  // Set baserom filelist path
		{
			Globals::Instance->fileListPath = argv[++i];
		}
		else if (arg == "-rconf")  // Read Config File
		{
			Globals::Instance->cfg.ReadConfigFile(argv[++i]);
		}
		else if (arg == "-eh")  // Enable Error Handler
		{
#ifdef __linux__
			signal(SIGSEGV, ErrorHandler);
			signal(SIGABRT, ErrorHandler);
#else
			// HANDLE_WARNING(WarningType::Always,
			//                "tried to set error handler, but this ZAPD build lacks support for one",
			//                "");
#endif


		}
		else if (arg == "-v")  // Verbose
		{
			Globals::Instance->verbosity = static_cast<VerbosityLevel>(strtol(argv[++i], NULL, 16));
		}
		else if (arg == "-vu" || arg == "--verbose-unaccounted")  // Verbose unaccounted
		{
			Globals::Instance->verboseUnaccounted = true;
		}
		else if (arg == "-se" || arg == "--set-exporter")  // Set Current Exporter
		{
			Globals::Instance->currentExporter = argv[++i];
		}
		else if (arg == "--gcc-compat")  // GCC compatibility
		{
			Globals::Instance->gccCompat = true;
		}
		else if (arg == "-s" || arg == "--static")
		{
			Globals::Instance->forceStatic = true;
		}
		else if (arg == "-us" || arg == "--unaccounted-static")
		{
			Globals::Instance->forceUnaccountedStatic = true;
		}
		else if (arg == "-brt" || arg == "--build-raw-texture")
		{
			Globals::Instance->buildRawTexture = true;
		}
	}

	// Parse File Mode
	ExporterSet* exporterSet = Globals::Instance->GetExporterSet();
	std::string buildMode = argv[1];
	ZFileMode fileMode = ZFileMode::Invalid;

	if (buildMode == "btex")
		fileMode = ZFileMode::BuildTexture;
	else if (buildMode == "bren")
		fileMode = ZFileMode::BuildBackground;
	else if (buildMode == "bovl")
		fileMode = ZFileMode::BuildOverlay;
	else if (buildMode == "bsf")
		fileMode = ZFileMode::BuildSourceFile;
	else if (buildMode == "bblb")
		fileMode = ZFileMode::BuildBlob;
	else if (buildMode == "e")
		fileMode = ZFileMode::Extract;
	else if (buildMode == "ed")
		fileMode = ZFileMode::ExtractDirectory;
	else if (exporterSet != nullptr && exporterSet->parseFileModeFunc != nullptr)
		exporterSet->parseFileModeFunc(buildMode, fileMode);

	if (fileMode == ZFileMode::Invalid)
	{
		printf("Error: Invalid file mode '%s'\n", buildMode.c_str());
		return 1;
	}

	Globals::Instance->fileMode = fileMode;

	if (fileMode == ZFileMode::ExtractDirectory)
		Globals::Instance->rom = new ZRom(Globals::Instance->baseRomPath.string());

	// We've parsed through our commands once. If an exporter exists, it's been set by now.
	// Now we'll parse through them again but pass them on to our exporter if one is available.

	if (exporterSet != nullptr && exporterSet->parseArgsFunc != nullptr)
	{
		for (int32_t i = 2; i < argc; i++)
			exporterSet->parseArgsFunc(argc, argv, i);
	}

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
		printf("ZAPD: Zelda Asset Processor For Decomp: %s\n", gBuildHash);

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
	{
		WarningHandler::PrintWarningsDebugInfo();
	}

	// TODO: switch
	if (fileMode == ZFileMode::Extract || fileMode == ZFileMode::BuildSourceFile || fileMode == ZFileMode::ExtractDirectory)
	{
		bool procFileModeSuccess = false;

		if (exporterSet != nullptr && exporterSet->processFileModeFunc != nullptr)
			procFileModeSuccess = exporterSet->processFileModeFunc(fileMode);

		if (!procFileModeSuccess)
		{
			if (fileMode == ZFileMode::ExtractDirectory)
			{
				std::vector<std::string> fileList =
					Directory::ListFiles(Globals::Instance->inputPath.string());

				const int num_threads = std::thread::hardware_concurrency();
				ctpl::thread_pool pool(num_threads / 2);

				bool parseSuccessful;

				auto start = std::chrono::steady_clock::now();
				int fileListSize = fileList.size();
				Globals::Instance->singleThreaded = false;

				for (int i = 0; i < fileListSize; i++)
					Globals::Instance->workerData[i] = new FileWorker();

				numWorkersLeft = fileListSize;

				for (int i = 0; i < fileListSize; i++)
				{
					if (Globals::Instance->singleThreaded)
					{
						ExtractFunc(i, fileList.size(), fileList[i], fileMode);
					}
					else
					{
						std::string fileListItem = fileList[i];
						pool.push([i, fileListSize, fileListItem, fileMode](int) {
							ExtractFunc(i, fileListSize, fileListItem, fileMode);
						});
					}
				}

				if (!Globals::Instance->singleThreaded)
				{
					while (true)
					{
						if (numWorkersLeft <= 0)
							break;

						std::this_thread::sleep_for(std::chrono::milliseconds(250));
					}
				}

				auto end = std::chrono::steady_clock::now();
				auto diff =
					std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

				printf("Generated OTR File Data in %i seconds\n", diff);
 			}
			else
			{
				bool parseSuccessful;

				for (auto& extFile : Globals::Instance->cfg.externalFiles)
				{
					fs::path externalXmlFilePath =
						Globals::Instance->cfg.externalXmlFolder / extFile.xmlPath;

					if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
					{
						printf("Parsing external file from config: '%s'\n",
						       externalXmlFilePath.c_str());
					}

					parseSuccessful = Parse(externalXmlFilePath, Globals::Instance->baseRomPath,
					                        extFile.outPath, ZFileMode::ExternalFile, 0);

					if (!parseSuccessful)
						return 1;
				}

				parseSuccessful =
					Parse(Globals::Instance->inputPath, Globals::Instance->baseRomPath,
				          Globals::Instance->outputPath, fileMode, 0);
				if (!parseSuccessful)
					return 1;
			}
		}
	}
	else if (fileMode == ZFileMode::BuildTexture)
	{
		TextureType texType = Globals::Instance->texType;
		BuildAssetTexture(Globals::Instance->inputPath, texType, Globals::Instance->outputPath);
	}
	else if (fileMode == ZFileMode::BuildBackground)
	{
		BuildAssetBackground(Globals::Instance->inputPath, Globals::Instance->outputPath);
	}
	else if (fileMode == ZFileMode::BuildBlob)
	{
		BuildAssetBlob(Globals::Instance->inputPath, Globals::Instance->outputPath);
	}

	if (exporterSet != nullptr && exporterSet->endProgramFunc != nullptr)
		exporterSet->endProgramFunc();

	delete g;
	return 0;
}

int ExtractFunc(int workerID, int fileListSize, std::string fileListItem, ZFileMode fileMode)
{
	bool parseSuccessful;

	printf("(%i / %i): %s\n", (workerID + 1), fileListSize, fileListItem.c_str());

	for (auto& extFile : Globals::Instance->cfg.externalFiles)
	{
		fs::path externalXmlFilePath = Globals::Instance->cfg.externalXmlFolder / extFile.xmlPath;

		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
		{
			printf("Parsing external file from config: '%s'\n", externalXmlFilePath.c_str());
		}

		parseSuccessful = Parse(externalXmlFilePath, Globals::Instance->baseRomPath,
		                        extFile.outPath, ZFileMode::ExternalFile, workerID);

		if (!parseSuccessful)
			return 1;
	}

	parseSuccessful = Parse(fileListItem, Globals::Instance->baseRomPath,
	                        Globals::Instance->outputPath, fileMode, workerID);

	if (!parseSuccessful)
		return 1;

	if (Globals::Instance->singleThreaded)
	{
		for (int i = 0; i < Globals::Instance->files.size(); i++)
		{
			delete Globals::Instance->files[i];
			Globals::Instance->files.erase(Globals::Instance->files.begin() + i);
			i--;
		}

		Globals::Instance->externalFiles.clear();
		Globals::Instance->segments.clear();
		Globals::Instance->cfg.segmentRefFiles.clear();
	}
	else
	{
		for (int i = 0; i < Globals::Instance->workerData[workerID]->files.size(); i++)
		{
			delete Globals::Instance->workerData[workerID]->files[i];
			Globals::Instance->workerData[workerID]->files.erase(
				Globals::Instance->workerData[workerID]->files.begin() +
			                               i);
			i--;
		}

		Globals::Instance->workerData[workerID]->externalFiles.clear();
		Globals::Instance->workerData[workerID]->segments.clear();
		Globals::Instance->workerData[workerID]->segmentRefFiles.clear();

		numWorkersLeft--;
	}
	return 0;
}

bool Parse(const fs::path& xmlFilePath, const fs::path& basePath, const fs::path& outPath,
           ZFileMode fileMode, int workerID)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError eResult = doc.LoadFile(xmlFilePath.string().c_str());

	if (eResult != tinyxml2::XML_SUCCESS)
	{
		// TODO: use XMLDocument::ErrorIDToName to get more specific error messages here
		HANDLE_ERROR(WarningType::InvalidXML,
		             StringHelper::Sprintf("invalid XML file: '%s'", xmlFilePath.c_str()), "");
		return false;
	}

	tinyxml2::XMLNode* root = doc.FirstChild();

	if (root == nullptr)
	{
		HANDLE_WARNING(
			WarningType::InvalidXML,
			StringHelper::Sprintf("missing Root tag in xml file: '%s'", xmlFilePath.c_str()), "");
		return false;
	}

	for (tinyxml2::XMLElement* child = root->FirstChildElement(); child != NULL;
	     child = child->NextSiblingElement())
	{
		if (std::string_view(child->Name()) == "File")
		{
			ZFile* file = new ZFile(fileMode, child, basePath, outPath, "", xmlFilePath, workerID);
			Globals::Instance->AddFile(file, workerID);
			if (fileMode == ZFileMode::ExternalFile)
			{
				Globals::Instance->AddExternalFile(file, workerID);
				file->isExternalFile = true;
			}
		}
		else if (std::string(child->Name()) == "ExternalFile")
		{
			const char* xmlPathValue = child->Attribute("XmlPath");
			if (xmlPathValue == nullptr)
			{
				throw std::runtime_error(StringHelper::Sprintf(
					"Parse: Fatal error in '%s'.\n"
					"\t Missing 'XmlPath' attribute in `ExternalFile` element.\n",
					xmlFilePath.c_str()));
			}
			const char* outPathValue = child->Attribute("OutPath");
			if (outPathValue == nullptr)
			{
				throw std::runtime_error(StringHelper::Sprintf(
					"Parse: Fatal error in '%s'.\n"
					"\t Missing 'OutPath' attribute in `ExternalFile` element.\n",
					xmlFilePath.c_str()));
			}

			fs::path externalXmlFilePath =
				Globals::Instance->cfg.externalXmlFolder / fs::path(xmlPathValue);
			fs::path externalOutFilePath = fs::path(outPathValue);

			if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
			{
				printf("Parsing external file: '%s'\n", externalXmlFilePath.c_str());
			}

			// Recursion. What can go wrong?
			Parse(externalXmlFilePath, basePath, externalOutFilePath, ZFileMode::ExternalFile, workerID);
		}
		else
		{
			std::string errorHeader =
				StringHelper::Sprintf("when parsing file '%s'", xmlFilePath.c_str());
			std::string errorBody = StringHelper::Sprintf(
				"Found a resource outside a File element: '%s'", child->Name());
			HANDLE_ERROR(WarningType::InvalidXML, errorHeader, errorBody);
		}
	}

	if (fileMode != ZFileMode::ExternalFile)
	{
		ExporterSet* exporterSet = Globals::Instance->GetExporterSet();

		if (exporterSet != nullptr && exporterSet->beginXMLFunc != nullptr)
			exporterSet->beginXMLFunc();

		std::vector<ZFile*> files;

		if (Globals::Instance->singleThreaded)
			files = Globals::Instance->files;
		else
			files = Globals::Instance->workerData[workerID]->files;

		for (ZFile* file : files)
		{
			if (fileMode == ZFileMode::BuildSourceFile)
				file->BuildSourceFile();
			else
				file->ExtractResources();
		}

		if (exporterSet != nullptr && exporterSet->endXMLFunc != nullptr)
			exporterSet->endXMLFunc();
	}

	return true;
}

void BuildAssetTexture(const fs::path& pngFilePath, TextureType texType, const fs::path& outPath)
{
	return Globals::Instance->BuildAssetTexture(pngFilePath, texType, outPath);
}

void BuildAssetBackground(const fs::path& imageFilePath, const fs::path& outPath)
{
	ZBackground background(nullptr);
	background.ParseBinaryFile(imageFilePath.string(), false);

	File::WriteAllText(outPath.string(), background.GetBodySourceCode());
}

void BuildAssetBlob(const fs::path& blobFilePath, const fs::path& outPath)
{
	ZBlob* blob = ZBlob::FromFile(blobFilePath.string());
	std::string name = outPath.stem().string();  // filename without extension

	std::string src = blob->GetBodySourceCode();

	File::WriteAllText(outPath.string(), src);

	delete blob;
}
