#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace SOH {

struct ExternalModManifest {
    std::string id;
    std::string name;
    std::string version;
    int32_t apiVersion = 0;
    std::string entryScript;
    std::vector<std::string> assets;
    int32_t loadOrder = 0;
};

struct ExternalModPackage {
    std::filesystem::path sourcePath;
    bool isZip = false;
    bool valid = false;
    std::string error;
    ExternalModManifest manifest;
};

class ExternalModManager {
  public:
    static ExternalModManager& Instance();

    void DiscoverPackages();
    const std::vector<ExternalModPackage>& GetPackages() const;

  private:
    std::vector<ExternalModPackage> mPackages;

    static bool TryParseManifest(const std::string& content, ExternalModManifest& outManifest, std::string& outError);
    static bool ReadManifestFromDirectory(const std::filesystem::path& dirPath, std::string& outContent,
                                          std::string& outError);
    static bool ReadManifestFromZip(const std::filesystem::path& zipPath, std::string& outContent, std::string& outError);
};

} // namespace SOH
