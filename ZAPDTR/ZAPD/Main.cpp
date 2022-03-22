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

#if !defined(_MSC_VER) && !defined(__CYGWIN__)
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

//extern const char gBuildHash[];
const char gBuildHash[] = "";

bool Parse(const fs::path& xmlFilePath, const fs::path& basePath, const fs::path& outPath,
           ZFileMode fileMode);

void BuildAssetTexture(const fs::path& pngFilePath, TextureType texType, const fs::path& outPath);
void BuildAssetBackground(const fs::path& imageFilePath, const fs::path& outPath);
void BuildAssetBlob(const fs::path& blobFilePath, const fs::path& outPath);

#if !defined(_MSC_VER) && !defined(__CYGWIN__)
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
		else if (arg == "-rconf")  // Read Config File
		{
			Globals::Instance->cfg.ReadConfigFile(argv[++i]);
		}
		else if (arg == "-eh")  // Enable Error Handler
		{
#if !defined(_MSC_VER) && !defined(__CYGWIN__)
			signal(SIGSEGV, ErrorHandler);
			signal(SIGABRT, ErrorHandler);
#else
			HANDLE_WARNING(WarningType::Always,
			               "tried to set error handler, but this ZAPD build lacks support for one",
			               "");
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
	else if (exporterSet != nullptr && exporterSet->parseFileModeFunc != nullptr)
		exporterSet->parseFileModeFunc(buildMode, fileMode);

	if (fileMode == ZFileMode::Invalid)
	{
		printf("Error: Invalid file mode '%s'\n", buildMode.c_str());
		return 1;
	}

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
	if (fileMode == ZFileMode::Extract || fileMode == ZFileMode::BuildSourceFile)
	{
		bool procFileModeSuccess = false;

		if (exporterSet != nullptr && exporterSet->processFileModeFunc != nullptr)
			procFileModeSuccess = exporterSet->processFileModeFunc(fileMode);

		if (!procFileModeSuccess)
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
				                        extFile.outPath, ZFileMode::ExternalFile);

				if (!parseSuccessful)
					return 1;
			}

			parseSuccessful = Parse(Globals::Instance->inputPath, Globals::Instance->baseRomPath,
			                        Globals::Instance->outputPath, fileMode);
			if (!parseSuccessful)
				return 1;
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
	else if (fileMode == ZFileMode::BuildOverlay)
	{
		ZOverlay* overlay =
			ZOverlay::FromBuild(Path::GetDirectoryName(Globals::Instance->inputPath),
		                        Path::GetDirectoryName(Globals::Instance->cfgPath));

		if (overlay != nullptr)
			File::WriteAllText(Globals::Instance->outputPath.string(),
			                   overlay->GetSourceOutputCode(""));
	}

	delete g;
	return 0;
}

bool Parse(const fs::path& xmlFilePath, const fs::path& basePath, const fs::path& outPath,
           ZFileMode fileMode)
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
			ZFile* file = new ZFile(fileMode, child, basePath, outPath, "", xmlFilePath);
			Globals::Instance->files.push_back(file);
			if (fileMode == ZFileMode::ExternalFile)
			{
				Globals::Instance->externalFiles.push_back(file);
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
			Parse(externalXmlFilePath, basePath, externalOutFilePath, ZFileMode::ExternalFile);
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

		for (ZFile* file : Globals::Instance->files)
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
	std::string name = outPath.stem().string();

	ZTexture tex(nullptr);

	if (name.find("u32") != std::string::npos)
		tex.dWordAligned = false;

	tex.FromPNG(pngFilePath.string(), texType);
	std::string cfgPath = StringHelper::Split(pngFilePath.string(), ".")[0] + ".cfg";

	if (File::Exists(cfgPath))
		name = File::ReadAllText(cfgPath);

	std::string src = tex.GetBodySourceCode();

	File::WriteAllText(outPath.string(), src);
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
