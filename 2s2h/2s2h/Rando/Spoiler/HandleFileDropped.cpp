#include "Spoiler.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <spdlog/spdlog.h>
#include "BenPort.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ShipInit.hpp"
#include <ship/window/Window.h>
#include "2s2h/BenGui/Notification.h"

extern "C" {
#include "sfx.h"
}

bool Rando::Spoiler::HandleFileDropped(char* filePath) {
    try {
        std::ifstream fileStream(filePath);

        if (!fileStream.is_open()) {
            return false;
        }

        // Check if first byte is "{"
        if (fileStream.peek() != '{') {
            return false;
        }

        nlohmann::json j;

        // Attempt to parse the file
        try {
            fileStream >> j;
        } catch (nlohmann::json::exception& e) { return false; }

        // Check if the file is a spoiler file
        if (!j.contains("type") || j["type"] != "2S2H_RANDO_SPOILER") {
            return false;
        }

        // Save the spoiler file to the randomizer folder
        std::string spoilerFile = std::filesystem::path(filePath).filename().string();
        std::string spoilerFilePath =
            Ship::Context::GetPathRelativeToAppDirectory("randomizer/" + spoilerFile, appShortName);
        std::filesystem::copy_file(filePath, spoilerFilePath, std::filesystem::copy_options::overwrite_existing);

        // Set the spoiler file to the new file
        CVarSetString("gRando.SpoilerFile", spoilerFile.c_str());
        // Update the spoiler file options
        Rando::Spoiler::RefreshOptions();
        ShipInit::Init("gRando.SpoilerFile");

        Audio_PlaySfx(NA_SE_SY_QUIZ_CORRECT);
        Notification::Emit({ .message = "Spoiler file loaded" });
        return true;
    } catch (std::exception& e) {
        SPDLOG_ERROR("Failed to load file: {}", e.what());
        Notification::Emit({ .message = "Failed to load file" });
        return false;
    } catch (...) {
        SPDLOG_ERROR("Failed to load file");
        Notification::Emit({ .message = "Failed to load file" });
        return false;
    }
}
