#pragma once

#include <atomic>
#include <cstddef>
#include <string>

// No torch types here; TorchExtract.cpp is the only TU that includes Companion.h.
namespace SohTorch {

// Count of .yml files under a version directory. Torch's phase callback fires once per file,
// so this is the progress denominator.
size_t CountAssetFiles(const std::string& ymlDir);

// Extracts romPath into destDir. Torch picks both the version directory under srcDir and the
// archive name (oot.o2r, oot-mq.o2r) from config.yml by hashing the ROM, so the name it chose
// is returned rather than assumed. Empty if extraction threw or produced no archive.
// Increments progress once per asset file.
std::string Extract(const std::string& romPath, const std::string& srcDir, const std::string& destDir,
                    const std::string& portVersion, std::atomic<size_t>* progress);

} // namespace SohTorch
