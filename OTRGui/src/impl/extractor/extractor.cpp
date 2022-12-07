#include "extractor.h"
#include <string>

#include "impl.h"
#include "utils/mutils.h"
#include "ctpl/ctpl_stl.h"
#include <thread>
#include <impl/baserom_extractor/baserom_extractor.h>

#ifdef _WIN32
#define PLATFORM Platforms::WINDOWS
#else
#define PLATFORM Platforms::LINUX
#endif
namespace Util = MoonUtils;

bool oldExtractMode = false;
static int maxResources = 0;
static int extractedResources = 0;
bool buildingOtr = false;
int skipFrames = 0;

bool isWindows() {
	return (PLATFORM == Platforms::WINDOWS);
}

std::string GetXMLVersion(RomVersion version)
{
	switch (version.crc)
	{
	case OOT_PAL_GC_DBG1: return "GC_NMQ_D";
	case OOT_PAL_GC_MQ_DBG: return "GC_MQ_D";
	case OOT_PAL_GC:	  return "GC_NMQ_PAL_F";
	}

	return "ERROR";
}

void BuildOTR(const std::string output, RomVersion version) {
	if (oldExtractMode)
	{
		std::string execStr = Util::format("assets/extractor/%s", isWindows() ? "ZAPD.exe" : "ZAPD.out") + " botr -se OTR";
		ProcessResult result = NativeFS->LaunchProcess(execStr);
		if (result.exitCode != 0) {
			std::cout << "\nError when building the OTR file with error code: " << result.exitCode << " !" << std::endl;
			std::cout << "Aborting...\n" << std::endl;
		}
	}

	setCurrentStep("Done!");

	// If a custom SoH folder was not selected, or the custom SOH folder is the same location as OTRGui,
	// then the otr file should already be where it is expected
	if (output == "." || output == MoonUtils::absolute(".")) {
		return;
	}

	// Otherwise an outside SoH folder was selected so we need to copy the exported otr to the correct folder
	const std::string otrName = version.isMQ ? "oot-mq.otr" : "oot.otr";
	const std::string outputPath = MoonUtils::join(output, otrName);
	if (MoonUtils::exists(outputPath)) MoonUtils::rm(outputPath);

	MoonUtils::copy(otrName, outputPath);
}

void ExtractFile(std::string xmlPath, std::string outPath, std::string outSrcPath, RomVersion version) {
	std::string otrExporterArgs = Util::format("--otrfile %s", version.isMQ ? "oot-mq.otr" : "oot.otr");
	if (xmlPath.find("overlays") != std::string::npos) {
		otrExporterArgs += " --static";
	}

	std::string execStr = Util::format("assets/extractor/%s", isWindows() ? "ZAPD.exe" : "ZAPD.out");
	std::string args = Util::format(" e -eh -i %s -b tmp/baserom/ -o %s -osf %s -gsf 1 -rconf assets/extractor/Config_%s.xml -se OTR %s", xmlPath.c_str(), outPath.c_str(), outSrcPath.c_str(), GetXMLVersion(version).c_str(), otrExporterArgs);
	ProcessResult result = NativeFS->LaunchProcess(execStr + args);

	if (result.exitCode != 0) {
		std::cout << "\nError when extracting the ROM with error code: " << result.exitCode << " !" << std::endl;
		std::cout << "Aborting...\n" << std::endl;
	}
}

void ExtractFunc(std::string fullPath, RomVersion version) {
	std::vector<std::string> path = Util::split(fullPath, Util::pathSeparator());
	std::string outPath = Util::join(Util::join("assets/extractor/xmls/output", path[4]), Util::basename(fullPath));
	Util::mkdir(outPath);
	ExtractFile(fullPath, outPath, outPath, version);
	setCurrentStep("Extracting: " + Util::basename(fullPath));
	extractedResources++;
}

void startWorker(RomVersion version) {
	std::string path = "assets/extractor/xmls/";

	path += GetXMLVersion(version);

	// Util::write("tmp/baserom/version", (char*)&version.crc, sizeof(version.crc));

	if (oldExtractMode)
	{
		std::vector<std::string> files;
		Util::dirscan(path, files);
		std::vector<std::string> xmlFiles;

		const int num_threads = std::thread::hardware_concurrency();
		ctpl::thread_pool pool(num_threads / 2);
		for (auto& file : files) {
			if (file.find(".xml") != std::string::npos) xmlFiles.push_back(file);
		}

		for (auto& file : xmlFiles) {
			if (single_thread) {
				ExtractFunc(file, version);
			}
			else {
				pool.push([file, version](int) {
					ExtractFunc(file, version);
					});
			}
		}

		maxResources = xmlFiles.size();
	}
	else
	{
		std::string otrExporterArgs = Util::format("--otrfile %s", version.isMQ ? "oot-mq.otr" : "oot.otr");
		std::string execStr = Util::format("assets/extractor/%s", isWindows() ? "ZAPD.exe" : "ZAPD.out");
		std::string args = Util::format(" ed -eh -i %s -b tmp/rom.z64 -fl assets/extractor/filelists -o %s -osf %s -gsf 1 -rconf assets/extractor/Config_%s.xml -se OTR %s", path.c_str(), (path + "/../").c_str(), (path + "/../").c_str(), GetXMLVersion(version).c_str(), otrExporterArgs.c_str());
		ProcessResult result = NativeFS->LaunchProcess(execStr + args);

		if (result.exitCode != 0) {
			std::cout << "\nError when extracting the ROM with error code: " << result.exitCode << " !" << std::endl;
			std::cout << "Aborting...\n" << std::endl;
		}
		else
		{
			printf("All done?\n");
		}

		maxResources = 1;
	}
}

void updateWorker(const std::string& output, RomVersion version) {
	if (maxResources > 0 && !buildingOtr && (extractedResources >= maxResources || !oldExtractMode)) 
	{
		setCurrentStep("Building OTR...");
		if (skipFrames < 3) {
			skipFrames++;
			return;
		}
		buildingOtr = true;
		
		if (single_thread || !oldExtractMode){
			BuildOTR(output, version);
			return;
		}
		std::thread otr(BuildOTR, output, version);
		otr.detach();
	}
}
