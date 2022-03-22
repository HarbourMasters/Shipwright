#pragma once

#include <map>
#include <string>
#include <vector>
#include "GameConfig.h"
#include "ZFile.h"

class ZRoom;

enum class VerbosityLevel
{
	VERBOSITY_SILENT,
	VERBOSITY_INFO,
	VERBOSITY_DEBUG
};

typedef void (*ExporterSetFunc)(ZFile*);
typedef bool (*ExporterSetFuncBool)(ZFileMode fileMode);
typedef void (*ExporterSetFuncVoid)(int argc, char* argv[], int& i);
typedef void (*ExporterSetFuncVoid2)(const std::string& buildMode, ZFileMode& fileMode);
typedef void (*ExporterSetFuncVoid3)();
typedef void (*ExporterSetResSave)(ZResource* res, BinaryWriter& writer);

class ExporterSet
{
public:
	~ExporterSet();

	std::map<ZResourceType, ZResourceExporter*> exporters;
	ExporterSetFuncVoid parseArgsFunc = nullptr;
	ExporterSetFuncVoid2 parseFileModeFunc = nullptr;
	ExporterSetFuncBool processFileModeFunc = nullptr;
	ExporterSetFunc beginFileFunc = nullptr;
	ExporterSetFunc endFileFunc = nullptr;
	ExporterSetFuncVoid3 beginXMLFunc = nullptr;
	ExporterSetFuncVoid3 endXMLFunc = nullptr;
	ExporterSetResSave resSaveFunc = nullptr;
};

class Globals
{
public:
	static Globals* Instance;

	bool genSourceFile;  // Used for extraction
	bool useExternalResources;
	bool testMode;  // Enables certain experimental features
	bool outputCrc = false;
	bool profile;  // Measure performance of certain operations
	bool useLegacyZDList;
	VerbosityLevel verbosity;  // ZAPD outputs additional information
	ZFileMode fileMode;
	fs::path baseRomPath, inputPath, outputPath, sourceOutputPath, cfgPath;
	TextureType texType;
	ZGame game;
	GameConfig cfg;
	bool verboseUnaccounted = false;
	bool gccCompat = false;
	bool forceStatic = false;
	bool forceUnaccountedStatic = false;
	bool otrMode = true;

	std::vector<ZFile*> files;
	std::vector<ZFile*> externalFiles;
	std::vector<int32_t> segments;

	std::string currentExporter;
	static std::map<std::string, ExporterSet*>& GetExporterMap();
	static void AddExporter(std::string exporterName, ExporterSet* exporterSet);

	Globals();
	~Globals();

	void AddSegment(int32_t segment, ZFile* file);
	bool HasSegment(int32_t segment);
	ZFile* GetSegment(int32_t segment);

	ZResourceExporter* GetExporter(ZResourceType resType);
	ExporterSet* GetExporterSet();

	/**
	 * Search in every file (and the symbol map) for the `segAddress` passed as parameter.
	 * If the segment of `currentFile` is the same segment of `segAddress`, then that file will be
	 * used only, otherwise, the search will be performed in every other file.
	 * The name of that variable will be stored in the `declName` parameter.
	 * Returns `true` if the address is found. `false` otherwise,
	 * in which case `declName` will be set to the address formatted as a pointer.
	 */
	bool GetSegmentedPtrName(segptr_t segAddress, ZFile* currentFile,
	                         const std::string& expectedType, std::string& declName);

	bool GetSegmentedArrayIndexedName(segptr_t segAddress, size_t elementSize, ZFile* currentFile,
	                                  const std::string& expectedType, std::string& declName);
};
