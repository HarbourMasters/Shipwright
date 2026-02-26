#pragma once

#include "ExternalModTypes.h"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <filesystem>
#include <memory>

namespace Ship {
class Archive;
}

namespace SOH {

struct Fast64ConversionOutput {
    std::map<std::string, std::vector<uint8_t>> resources;
    std::string rootDisplayListPath;
    size_t vertexArrayCount = 0;
    size_t displayListArrayCount = 0;
    size_t textureCount = 0;
};

struct Fast64ArchiveInspection {
    bool isZipArchive = false;
    bool hasDisplayListResources = false;
    bool hasFast64Source = false;
    std::string rootDisplayListPath;
    std::string modelIncContent;
    std::string headerContent;
    std::string objectNameHint;
};

struct Fast64TextureMeta {
    std::string format;
    std::string size;
    int32_t widthArg = 0;
    int32_t line = -1;
    int32_t loadBlockLrs = -1;
    bool hasTileSize = false;
    int32_t tileLrs = 0;
    int32_t tileLrt = 0;
    int32_t tileWidthHint = 0;
    int32_t tileHeightHint = 0;
    int32_t maxObservedLrs = -1;
    int32_t maxObservedLrt = -1;
    bool hasDecodedDimensions = false;
    int32_t decodedWidth = 0;
    int32_t decodedHeight = 0;
};

using Fast64TextureIncludeResolver =
    std::function<bool(const std::string&, const Fast64TextureMeta&, std::vector<uint8_t>&, int32_t&, int32_t&,
                       std::string&)>;

class ExternalModFast64 {
public:
    static bool ConvertFast64SourceToResources(const std::string& modelContent, const std::string& objectName,
                                               const Fast64TextureIncludeResolver& includeResolver,
                                               ExternalModModelTextureFilter configuredTextureFilter,
                                               const std::string& debugLabel, Fast64ConversionOutput& outConversion,
                                               std::string& outError);

    static bool InspectFast64ArchiveBytes(const std::vector<uint8_t>& archiveBytes, Fast64ArchiveInspection& outInspection,
                                          std::string& outError);

    static std::shared_ptr<Ship::Archive> CreateGeneratedFast64Archive(const std::filesystem::path& virtualArchivePath,
                                                                       std::map<std::string, std::vector<uint8_t>> resources,
                                                                       std::string& outError);

    static std::filesystem::path BuildGeneratedFast64ArchivePath(const std::string& modId, const std::string& itemId,
                                                                 const std::string& objectName);
};

} // namespace SOH
