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

std::string Lang::Translate(const char* path) {
    std::string currentLang = CVarGetString(CVAR_SETTING("Language"), "en_US");

    if (!langs.contains(currentLang)) {
        currentLang = "en_US";
        CVarSetString(CVAR_SETTING("Language"), "en_US");

        if (!langs.contains(currentLang)) {
            return "ERROR: Language data not found";
        }
    }

    nlohmann::json currentLangData = langs[currentLang];

    std::string path2 = path;

    std::replace(path2.begin(), path2.end(), '.', '/');

    path2 = "/" + path2;

    if (!currentLangData.contains(path2)) {
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
        ImGui::Text("    %s", data["/language_name"].get_ref<const std::string&>().c_str());
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