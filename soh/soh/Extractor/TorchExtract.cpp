#include "TorchExtract.h"

#include <exception>
#include <filesystem>
#include <memory>

#include "spdlog/spdlog.h"

#include "Companion.h"
#include "factories/BaseFactory.h"

namespace fs = std::filesystem;

namespace SohTorch {

size_t CountAssetFiles(const std::string& ymlDir) {
    std::error_code ec;
    if (!fs::is_directory(ymlDir, ec)) {
        return 0;
    }

    size_t count = 0;
    for (fs::recursive_directory_iterator it(ymlDir, ec), end; it != end; it.increment(ec)) {
        if (ec) {
            break;
        }
        if (it->is_regular_file(ec) && it->path().extension() == ".yml") {
            count++;
        }
    }
    return count;
}

std::string Extract(const std::string& romPath, const std::string& srcDir, const std::string& destDir,
                    const std::string& portVersion, std::atomic<size_t>* progress) {
    std::string archiveName;

    try {
        // Companion::Instance is a raw global with no getter; factories dereference it.
        auto companion = std::make_unique<Companion>(fs::path(romPath), ArchiveType::O2R, false, srcDir, destDir);
        Companion::Instance = companion.get();
        companion->SetVersion(portVersion);
        companion->SetPhaseCallback([progress](int) {
            if (progress != nullptr) {
                (*progress)++;
            }
        });

        // Init is the whole run; it calls Process() internally.
        companion->Init(ExportType::Binary);

        // config.yml names the archive per rom; ask rather than guess, and ask before the
        // companion goes away.
        archiveName = fs::path(companion->GetOutputPath()).filename().string();

        // Companion holds every parsed asset, so don't leak it into the game's lifetime.
        companion.reset();
        Companion::Instance = nullptr;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Torch extraction failed: {}", e.what());
        Companion::Instance = nullptr;
        return "";
    } catch (...) {
        SPDLOG_ERROR("Torch extraction failed with an unknown exception");
        Companion::Instance = nullptr;
        return "";
    }

    // Process() returns void and several fatal paths only log and return, so confirm the
    // archive is really there rather than trusting the run.
    std::error_code ec;
    if (archiveName.empty() || !fs::exists(fs::path(destDir) / archiveName, ec)) {
        SPDLOG_ERROR("Torch produced no archive in {}", destDir);
        return "";
    }

    return archiveName;
}

} // namespace SohTorch
