#include "ExternalModManager.h"

#include <fstream>
#include <sstream>
#include <unordered_set>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <zip.h>

#include <ship/Context.h>

namespace SOH {

namespace {
constexpr int32_t kExternalModApiVersion = 1;

bool ValidateRequiredString(const nlohmann::json& json, const char* key, std::string& value, std::string& outError) {
    if (!json.contains(key) || !json[key].is_string()) {
        outError = std::string("Missing or invalid manifest field: ") + key;
        return false;
    }

    value = json[key].get<std::string>();
    if (value.empty()) {
        outError = std::string("Manifest field must not be empty: ") + key;
        return false;
    }

    return true;
}
} // namespace

ExternalModManager& ExternalModManager::Instance() {
    static ExternalModManager instance;
    return instance;
}

const std::vector<ExternalModPackage>& ExternalModManager::GetPackages() const {
    return mPackages;
}

void ExternalModManager::DiscoverPackages() {
    mPackages.clear();

    const auto modsPath = std::filesystem::path(Ship::Context::LocateFileAcrossAppDirs("mods", "soh"));
    if (!std::filesystem::exists(modsPath)) {
        SPDLOG_INFO("[ExternalMods] Mods path does not exist: {}", modsPath.string());
        return;
    }

    std::unordered_set<std::string> seenIds;

    for (const auto& entry : std::filesystem::directory_iterator(modsPath)) {
        if (!entry.is_directory() && !entry.is_regular_file()) {
            continue;
        }

        ExternalModPackage package;
        package.sourcePath = entry.path();
        package.isZip = entry.is_regular_file() && entry.path().extension() == ".zip";

        std::string manifestContent;
        std::string manifestError;
        bool manifestRead = false;

        if (entry.is_directory()) {
            manifestRead = ReadManifestFromDirectory(entry.path(), manifestContent, manifestError);
        } else if (package.isZip) {
            manifestRead = ReadManifestFromZip(entry.path(), manifestContent, manifestError);
        } else {
            continue;
        }

        if (!manifestRead) {
            package.valid = false;
            package.error = manifestError;
            SPDLOG_WARN("[ExternalMods] Skipping package {}: {}", package.sourcePath.string(), package.error);
            mPackages.push_back(package);
            continue;
        }

        std::string parseError;
        if (!TryParseManifest(manifestContent, package.manifest, parseError)) {
            package.valid = false;
            package.error = parseError;
            SPDLOG_WARN("[ExternalMods] Invalid manifest in {}: {}", package.sourcePath.string(), package.error);
            mPackages.push_back(package);
            continue;
        }

        if (seenIds.contains(package.manifest.id)) {
            package.valid = false;
            package.error = "Duplicate mod id detected: " + package.manifest.id;
            SPDLOG_WARN("[ExternalMods] Skipping package {}: {}", package.sourcePath.string(), package.error);
            mPackages.push_back(package);
            continue;
        }

        seenIds.insert(package.manifest.id);
        package.valid = true;
        SPDLOG_INFO("[ExternalMods] Loaded manifest {} ({}) from {}", package.manifest.name, package.manifest.id,
                    package.sourcePath.string());
        mPackages.push_back(package);
    }

    SPDLOG_INFO("[ExternalMods] Discovery complete: {} package(s)", mPackages.size());
}

bool ExternalModManager::TryParseManifest(const std::string& content, ExternalModManifest& outManifest,
                                          std::string& outError) {
    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("JSON parse error: ") + ex.what();
        return false;
    }

    if (!ValidateRequiredString(json, "id", outManifest.id, outError) ||
        !ValidateRequiredString(json, "name", outManifest.name, outError) ||
        !ValidateRequiredString(json, "version", outManifest.version, outError) ||
        !ValidateRequiredString(json, "entryScript", outManifest.entryScript, outError)) {
        return false;
    }

    if (!json.contains("apiVersion") || !json["apiVersion"].is_number_integer()) {
        outError = "Missing or invalid manifest field: apiVersion";
        return false;
    }

    outManifest.apiVersion = json["apiVersion"].get<int32_t>();
    if (outManifest.apiVersion != kExternalModApiVersion) {
        outError = "Unsupported apiVersion: " + std::to_string(outManifest.apiVersion);
        return false;
    }

    if (json.contains("loadOrder") && json["loadOrder"].is_number_integer()) {
        outManifest.loadOrder = json["loadOrder"].get<int32_t>();
    }

    if (json.contains("assets")) {
        if (!json["assets"].is_array()) {
            outError = "Invalid manifest field: assets must be an array";
            return false;
        }

        for (const auto& asset : json["assets"]) {
            if (!asset.is_string()) {
                outError = "Invalid manifest field: assets entries must be strings";
                return false;
            }
            outManifest.assets.push_back(asset.get<std::string>());
        }
    }

    return true;
}

bool ExternalModManager::ReadManifestFromDirectory(const std::filesystem::path& dirPath, std::string& outContent,
                                                   std::string& outError) {
    const auto manifestPath = dirPath / "mod.json";
    if (!std::filesystem::exists(manifestPath)) {
        outError = "mod.json not found in directory package";
        return false;
    }

    std::ifstream file(manifestPath);
    if (!file.is_open()) {
        outError = "Unable to open mod.json";
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    outContent = ss.str();
    return true;
}

bool ExternalModManager::ReadManifestFromZip(const std::filesystem::path& zipPath, std::string& outContent,
                                             std::string& outError) {
    int err = 0;
    zip_t* archive = zip_open(zipPath.string().c_str(), ZIP_RDONLY, &err);
    if (archive == nullptr) {
        outError = "Unable to open zip archive";
        return false;
    }

    zip_stat_t stat;
    zip_stat_init(&stat);
    if (zip_stat(archive, "mod.json", ZIP_FL_ENC_GUESS, &stat) != 0) {
        outError = "mod.json not found in zip package";
        zip_close(archive);
        return false;
    }

    zip_file_t* file = zip_fopen(archive, "mod.json", ZIP_FL_ENC_GUESS);
    if (file == nullptr) {
        outError = "Unable to read mod.json from zip package";
        zip_close(archive);
        return false;
    }

    std::string content(stat.size, '\0');
    const auto readCount = zip_fread(file, content.data(), stat.size);
    zip_fclose(file);
    zip_close(archive);

    if (readCount < 0) {
        outError = "Failed reading mod.json from zip package";
        return false;
    }

    content.resize(static_cast<size_t>(readCount));
    outContent = content;
    return true;
}

} // namespace SOH
