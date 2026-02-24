#pragma once

#include "ExternalModTypes.h"
#include <string>
#include <vector>
#include <filesystem>

namespace SOH {

class ExternalModPackageIO {
public:
    static bool ReadManifestFromDirectory(const std::filesystem::path& dirPath, std::string& outContent, std::string& outError);
    static bool ReadManifestFromZip(const std::filesystem::path& zipPath, std::string& outContent, std::string& outError);
    static bool ReadFileFromDirectory(const std::filesystem::path& filePath, uint64_t maxBytes, std::string& outContent, std::string& outError);
    static bool ReadFileFromZip(const std::filesystem::path& zipPath, const std::filesystem::path& packageRelativePath, uint64_t maxBytes, std::vector<char>& outBytes, std::string& outError);
    static bool ReadFileFromPackage(const ExternalModPackage& package, const std::filesystem::path& packageRelativePath, uint64_t maxBytes, std::string& outContent, std::string& outError);
    static bool ReadBinaryFromPackage(const ExternalModPackage& package, const std::filesystem::path& packageRelativePath, uint64_t maxBytes, std::vector<uint8_t>& outBytes, std::string& outError);
    static bool IsSafePackageRelativePath(const std::string& pathValue, std::filesystem::path& outNormalizedPath, std::string& outError);
};

} // namespace SOH
