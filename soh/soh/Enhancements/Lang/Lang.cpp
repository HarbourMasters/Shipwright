#include "Lang.h"

#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/SohMenu.h"
#include "soh/util.h"

#include "ship/Context.h"
#include "ship/resource/File.h"
#include "ship/resource/ResourceManager.h"
#include "ship/resource/type/Json.h"
#include "ship/utils/StringHelper.h"

#include "spdlog/spdlog.h"

#include <memory>

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

static bool initialized = false;
static std::map<std::string, nlohmann::json> langs;

#define LANGUAGE_CVAR CVAR_SETTING("Language")
#define DEFAULT_LANGUAGE "en_US"

std::string Lang::Translate(const char* path) {
    if (!initialized) {
        SPDLOG_ERROR("Tried to obtain a translation before the translation data is initialized");
        assert(false);
        return "ERROR: Language data not initialized yet";
    }

    std::string currentLang = CVarGetString(LANGUAGE_CVAR, DEFAULT_LANGUAGE);

    if (!langs.contains(currentLang)) {
        SPDLOG_WARN("Current language ({}) doesn't exist, trying to fall back to default language ({})",
                    currentLang.c_str(), DEFAULT_LANGUAGE);

        currentLang = DEFAULT_LANGUAGE;

        if (!langs.contains(currentLang)) {
            SPDLOG_ERROR("Default language ({}) doesn't exist", DEFAULT_LANGUAGE);
            assert(false);
            return "ERROR: Language data not found";
        }

        CVarSetString(LANGUAGE_CVAR, DEFAULT_LANGUAGE);
        SPDLOG_WARN("Fallback to default language ({}) was succesful", DEFAULT_LANGUAGE);
    }

    nlohmann::json currentLangData = langs[currentLang];

    std::vector<std::string> segments = SohUtils::StringSplit(std::string(path), ".");

    std::string lastSegment = segments[segments.size() - 1];

    segments.pop_back();

    for (const auto& segment : segments) {
        if (!currentLangData.contains(segment)) {
            SPDLOG_WARN("Current language ({}) doesn't have data for the requested path ({})", currentLang.c_str(),
                        path);
            return std::string(path);
        }

        currentLangData = currentLangData[segment];
    }

    if (!currentLangData.contains(lastSegment)) {
        SPDLOG_WARN("Current language ({}) doesn't have data for the requested path ({})", currentLang.c_str(), path);
        return std::string(path);
    }

    if (currentLangData[lastSegment].is_string()) {
        return currentLangData[lastSegment].get<std::string>();
    }

    if (currentLangData[lastSegment].is_array()) {
        std::string translatedString = "";

        for (const auto& item : currentLangData[lastSegment]) {
            if (!item.is_string()) {
                SPDLOG_WARN("Current language ({}) has an array with a non-string at the requested path ({})",
                            currentLang.c_str(), path);
                return std::string(path);
            }

            translatedString += item.get<std::string>();
        }

        return translatedString;
    }

    SPDLOG_WARN("Current language ({}) doesn't have either a string or an array at the requested path ({})",
                currentLang.c_str(), path);
    return std::string(path);
}

void Lang::LoadLangs() {
    auto initData = std::make_shared<Ship::ResourceInitData>();
    initData->Format = RESOURCE_FORMAT_BINARY;
    initData->Type = static_cast<uint32_t>(Ship::ResourceType::Json);
    initData->ResourceVersion = 0;
    const static std::string folder = "lang/*";
    auto langFiles = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->ListFiles(folder);
    size_t start = std::string(folder).size() - 1;
    for (size_t i = 0; i < langFiles->size(); i++) {
        std::string filePath = langFiles->at(i);
        auto json = std::static_pointer_cast<Ship::Json>(
            Ship::Context::GetInstance()->GetResourceManager()->LoadResource(filePath, true, initData));

        std::string fileName = filePath.substr(start, filePath.size() - start - 5); // 5 for length of ".json"
        langs.insert_or_assign(fileName, json->Data);
    }
    initialized = true;
}

void LanguageCustomWidget(WidgetInfo& info) {
    ImGui::Text("Select Language:");
    for (const auto& [id, data] : langs) {
        if (ImGui::Button(StringHelper::Sprintf("%s [%s]", data["language_name"].get_ref<const std::string&>().c_str(),
                                                id.c_str())
                              .c_str())) {
            CVarSetString(LANGUAGE_CVAR, id.c_str());
        }
    }
}

void RegisterLangWidgets() {
    return;

    // TODO: Improve & enable this when everything is set up

    SohGui::mSohMenu->AddSidebarEntry("Settings", "Language", 1);
    WidgetPath path = { "Settings", "Language", SECTION_COLUMN_1 };
    SohGui::mSohMenu->AddWidget(path, "LanguageWidget", WIDGET_CUSTOM)
        .CustomFunction(LanguageCustomWidget)
        .HideInSearch(true);
}

static RegisterMenuInitFunc menuInitFunc(RegisterLangWidgets);