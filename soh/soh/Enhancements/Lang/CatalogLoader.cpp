#include "Lang.h"
#include "TranslationCatalog.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include "ship/Context.h"
#include "ship/resource/File.h"
#include "ship/resource/ResourceManager.h"
#include "ship/resource/type/Json.h"
#include "spdlog/spdlog.h"

Lang::CatalogLoadResult Lang::LoadCatalogs(TranslationCatalog& destination, const std::string& folder) {
    CatalogLoadResult result;
    auto initData = std::make_shared<Ship::ResourceInitData>();
    initData->Format = RESOURCE_FORMAT_BINARY;
    initData->Type = static_cast<uint32_t>(Ship::ResourceType::Json);
    initData->ResourceVersion = 0;
    auto manager = Ship::Context::GetRawInstance()->GetResourceManager();
    const auto archives = manager->GetArchiveManager()->GetArchives();
    for (const auto& archive : *archives) {
        SPDLOG_INFO("Catalog source for {}: {}", folder, archive->GetPath());
    }
    const std::string prefix = folder + "/";
    auto files = manager->GetArchiveManager()->ListFiles(prefix + "*");
    if (files == nullptr) {
        SPDLOG_ERROR("Couldn't list translation catalogs in {}", folder);
        return result;
    }
    auto paths = *files;
    std::sort(paths.begin(), paths.end());
    for (const auto& path : paths) {
        if (!path.starts_with(prefix) || !path.ends_with(".json")) {
            continue;
        }
        const std::string language = path.substr(prefix.size(), path.size() - prefix.size() - 5);
        if (language.empty() || language.find('/') != std::string::npos) {
            continue;
        }
        try {
            // Include the path in diagnostics and keep metadata local to this resource.
            auto fileInitData = std::make_shared<Ship::ResourceInitData>(*initData);
            fileInitData->Path = path;
            auto resource = std::static_pointer_cast<Ship::Json>(manager->LoadResource(path, true, fileInitData));
            if (resource == nullptr) {
                SPDLOG_ERROR("Couldn't load translation catalog {}", path);
                continue;
            }
            destination.MergeLanguage(language, resource->Data);
            ++result.loaded;
            result.hasEnglish |= language == "en_US";
            SPDLOG_INFO("Loaded translation catalog {}", path);
        } catch (const std::exception& error) {
            SPDLOG_ERROR("Invalid translation catalog {}: {}", path, error.what());
        }
    }
    return result;
}
