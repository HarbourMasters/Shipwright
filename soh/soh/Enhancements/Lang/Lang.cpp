#include "Lang.h"

#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/SohMenu.h"

#include "ship/Context.h"
#include "ship/resource/File.h"
#include "ship/resource/ResourceManager.h"
#include "ship/resource/type/Json.h"

#include <memory>

namespace SohGui {
    extern std::shared_ptr<SohMenu> mSohMenu;
}

static std::map<std::string, nlohmann::json> langs;

#define DEFAULT_LANGUAGE "en_US"

std::string Lang::Translate(const char* path) {
    std::string currentLang = CVarGetString(CVAR_SETTING("Language"), DEFAULT_LANGUAGE);

    if (!langs.contains(currentLang)) {
        LUSLOG_WARN("Current language (%s) doesn't exist, trying to fall back to default language (%s)", currentLang.c_str(), DEFAULT_LANGUAGE);

        currentLang = DEFAULT_LANGUAGE;
        CVarSetString(CVAR_SETTING("Language"), DEFAULT_LANGUAGE);

        if (!langs.contains(currentLang)) {
            LUSLOG_ERROR("Default language (%s) doesn't exist", DEFAULT_LANGUAGE);
            return "ERROR: Language data not found";
        }

        LUSLOG_WARN("Fall back to default language (%s) was succesful", DEFAULT_LANGUAGE);
    }

    nlohmann::json currentLangData = langs[currentLang];

    std::string path2 = std::string(path);

    std::replace(path2.begin(), path2.end(), '.', '/');

    path2 = "/" + path2;

    if (!currentLangData.contains(path2)) {
        LUSLOG_WARN("Current language (%s) doesn't have data for the requested path (%s)", currentLang.c_str(), path2.c_str());
        return path;
    }

    std::string translatedString = currentLangData[path2].get<std::string>();

    return translatedString;
}

void Lang::LoadLangs() {
    auto initData = std::make_shared<Ship::ResourceInitData>();
    initData->Format = RESOURCE_FORMAT_BINARY;
    initData->Type = static_cast<uint32_t>(Ship::ResourceType::Json);
    initData->ResourceVersion = 0;
    std::string folder = "lang/*";
    auto langFiles = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->ListFiles(folder);
    size_t start = std::string(folder).size() - 1;
    for (size_t i = 0; i < langFiles->size(); i++) {
        std::string filePath = langFiles->at(i);
        auto json = std::static_pointer_cast<Ship::Json>(
            Ship::Context::GetInstance()->GetResourceManager()->LoadResource(filePath, true, initData));

        std::string fileName = filePath.substr(start, filePath.size() - start - 5); // 5 for length of ".json"
        langs.insert_or_assign(fileName, json->Data.flatten());
    }
}

void LanguageCustomWidget(WidgetInfo& info) {
    ImGui::Text("Avaliable Langs (%d):", langs.size());
    for (const auto& [name, data] : langs) {
        ImGui::Text("    %s [%s]", data["/language_name"].get_ref<const std::string&>().c_str(), name.c_str());
    }
}

void RegisterLangWidgets() {
    SohGui::mSohMenu->AddSidebarEntry("Settings", "Language", 1);
    WidgetPath path = { "Settings", "Language", SECTION_COLUMN_1 };
    SohGui::mSohMenu->AddWidget(path, "LanguageWidget", WIDGET_CUSTOM)
        .CustomFunction(LanguageCustomWidget)
        .HideInSearch(true);
}

static RegisterMenuInitFunc menuInitFunc(RegisterLangWidgets);