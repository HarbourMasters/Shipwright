#include "extractor.h"
#include <string>

#include "impl.h"
#include "utils/mutils.h"
#include "ctpl/ctpl_stl.h"
#include <thread>

#ifdef _WIN32
#define PLATFORM Platforms::WINDOWS
#else
#define PLATFORM Platforms::LINUX
#endif
namespace Util = MoonUtils;

static int maxResources = 0;
static int extractedResources = 0;
bool buildingOtr = false;
int skipFrames = 0;

bool isWindows() {
	return (PLATFORM == Platforms::WINDOWS);
}

void BuildOTR(const std::string output) {
	Util::copy("tmp/baserom/Audiobank", "Extract/Audiobank");
	Util::copy("tmp/baserom/Audioseq", "Extract/Audioseq");
	Util::copy("tmp/baserom/Audiotable", "Extract/Audiotable");

	Util::copy("assets/game/", "Extract/assets/");

	std::string execStr = Util::format("assets/extractor/%s", isWindows() ? "ZAPD.exe" : "ZAPD.out") + " botr -se OTR";
	ProcessResult result = NativeFS->LaunchProcess(execStr);
	if(result.exitCode != 0) {
		std::cout << "\nError when building the OTR file with error code: " << result.exitCode << " !" << std::endl;
		std::cout << "Aborting...\n" << std::endl;
	}
	setCurrentStep("Done!");

	if (output == ".") return;
	const std::string outputPath = MoonUtils::join(output, "oot.otr");
	if(MoonUtils::exists(outputPath)) MoonUtils::rm(outputPath);

	MoonUtils::copy("oot.otr", outputPath);
}

void ExtractFile(std::string xmlPath, std::string outPath, std::string outSrcPath) {
	std::string execStr = Util::format("assets/extractor/%s", isWindows() ? "ZAPD.exe" : "ZAPD.out");
	std::string args = Util::format(" e -eh -i %s -b tmp/baserom/ -o %s -osf %s -gsf 1 -rconf assets/extractor/Config.xml -se OTR %s", xmlPath.c_str(), outPath.c_str(), outSrcPath.c_str(), xmlPath.find("overlays") != std::string::npos ? "--static" : "");
	ProcessResult result = NativeFS->LaunchProcess(execStr + args);

	if (result.exitCode != 0) {
		std::cout << "\nError when extracting the ROM with error code: " << result.exitCode << " !" << std::endl;
		std::cout << "Aborting...\n" << std::endl;
	}
}

void ExtractFunc(std::string fullPath) {
	std::vector<std::string> path = Util::split(fullPath, Util::pathSeparator());
	std::string outPath = Util::join(Util::join("assets/extractor/xmls/output", path[4]), Util::basename(fullPath));
	Util::mkdir(outPath);
	ExtractFile(fullPath, outPath, outPath);
	setCurrentStep("Extracting: " + Util::basename(fullPath));
	extractedResources++;
}

void startWorker() {
	std::string path = "assets/extractor/xmls";
	std::vector<std::string> files;
	Util::dirscan(path, files);
	std::vector<std::string> xmlFiles;

	const int num_threads = std::thread::hardware_concurrency();
	ctpl::thread_pool pool(num_threads / 2);
	for(auto &file : files) {
		if (file.find(".xml") != std::string::npos) xmlFiles.push_back(file);
	}

	for (auto& file : xmlFiles) {
		if(single_thread) {
			ExtractFunc(file);
		} else {
			pool.push([file](int) {
				ExtractFunc(file);
			});
		}
	}

	maxResources = xmlFiles.size();
}

void updateWorker(const std::string& output) {
	if (maxResources > 0 && !buildingOtr && extractedResources >= maxResources) {
		setCurrentStep("Building OTR...");
		if (skipFrames < 3) {
			skipFrames++;
			return;
		}
		buildingOtr = true;
		if (single_thread){
			BuildOTR(output);
			return;
		}
		std::thread otr(BuildOTR, output);
		otr.detach();
	}
}