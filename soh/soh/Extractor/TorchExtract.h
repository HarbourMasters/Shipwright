#ifndef TORCHEXTRACT_H
#define TORCHEXTRACT_H

#include <atomic>
#include <cstddef>
#include <string>

// No torch types here; TorchExtract.cpp is the only TU that includes Companion.h.
namespace SohTorch {

// Count of .yml files under a version directory. Torch's phase callback fires once per file,
// so this is the progress denominator.
size_t CountAssetFiles(const std::string& ymlDir);

// Extracts romPath into destDir/archiveName, where archiveName is what config.yml names the
// output for that ROM. Torch picks the version directory under srcDir by hashing the ROM.
// Increments progress once per asset file. False if it threw or produced no archive.
bool Extract(const std::string& romPath, const std::string& srcDir, const std::string& destDir,
             const std::string& portVersion, const std::string& archiveName,
             std::atomic<size_t>* progress);

} // namespace SohTorch

#endif
