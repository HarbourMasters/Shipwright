#include <memory>

#include <libultraship/bridge/consolevariablebridge.h>
#include "ship/Context.h"
#include "ship/resource/File.h"
#include "ship/resource/type/Json.h"
#include "ship/utils/StringHelper.h"

#include "Lang.h"
#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohMenu.h"
#include "soh/util.h"
#include "spdlog/spdlog.h"

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

static bool initialized = false;
static std::unordered_map<std::string, nlohmann::json> langs;

static std::unordered_map<std::string, std::string> cache;

#define LANGUAGE_CVAR CVAR_SETTING("Language")
#define DEFAULT_LANGUAGE "en_US"

std::variant<std::reference_wrapper<const std::string>, Lang::Error> Lang::TryTranslate(const char* path) {
    if (!initialized) {
        SPDLOG_ERROR("Tried to obtain a translation before the translation data is initialized");
        assert(false);
        return Lang::Error::LanguageDataNotInitialized;
    }

    std::string currentLang = CVarGetString(LANGUAGE_CVAR, DEFAULT_LANGUAGE);

    if (!langs.contains(currentLang)) {
        SPDLOG_WARN("Current language ({}) doesn't exist, trying to fall back to default language ({})",
                    currentLang.c_str(), DEFAULT_LANGUAGE);

        currentLang = DEFAULT_LANGUAGE;

        if (!langs.contains(currentLang)) {
            SPDLOG_ERROR("Default language ({}) doesn't exist", DEFAULT_LANGUAGE);
            assert(false);
            return Lang::Error::LanguageDataNotFound;
        }

        CVarSetString(LANGUAGE_CVAR, DEFAULT_LANGUAGE);
        SPDLOG_WARN("Fallback to default language ({}) was succesful", DEFAULT_LANGUAGE);
    }

    std::string stringPath = std::string(path);

    if (cache.contains(stringPath)) {
        return cache[stringPath];
    }

    const nlohmann::json* currentLangData = &langs.at(currentLang);

    std::vector<std::string> segments = SohUtils::StringSplit(stringPath, ".");

    std::string lastSegment = segments[segments.size() - 1];

    segments.pop_back();

    for (const auto& segment : segments) {
        if (!currentLangData->contains(segment)) {
            return Lang::Error::PathDataNotFound;
        }

        currentLangData = &currentLangData->at(segment);
    }

    if (!currentLangData->contains(lastSegment)) {
        return Lang::Error::PathDataNotFound;
    }

    if (currentLangData->at(lastSegment).is_string()) {
        const std::string& translatedString = currentLangData->at(lastSegment).get_ref<const std::string&>();
        cache[stringPath] = translatedString;
        return cache[stringPath];
    }

    if (currentLangData->at(lastSegment).is_array()) {
        std::string translatedString = "";

        for (const auto& item : currentLangData->at(lastSegment)) {
            if (!item.is_string()) {
                return Lang::Error::PathInvalidValue;
            }

            translatedString += item.get_ref<const std::string&>();
        }

        cache[stringPath] = translatedString;
        return cache[stringPath];
    }

    return Lang::Error::PathInvalidValue;
}

const std::string errorMessage =
    "[ERROR] Couldn't retrieve language data for this item, check the log for more information.";

const std::string& Lang::Translate(const char* path) {
    auto value = Lang::TryTranslate(path);

    if (std::holds_alternative<std::reference_wrapper<const std::string>>(value)) {
        return std::get<std::reference_wrapper<const std::string>>(value);
    } else if (std::holds_alternative<Lang::Error>(value)) {
        switch (std::get<Lang::Error>(value)) {
            case Lang::Error::PathDataNotFound:
                SPDLOG_WARN("Current language ({}) doesn't have data for the requested path ({})",
                            CVarGetString(LANGUAGE_CVAR, DEFAULT_LANGUAGE), path);
                break;
            case Lang::Error::PathInvalidValue:
                SPDLOG_WARN("Current language ({}) has an array with a non-string at the requested path ({})",
                            CVarGetString(LANGUAGE_CVAR, DEFAULT_LANGUAGE), path);
                break;
            default:
                break;
        }

        return errorMessage;
    } else {
        assert(false);
        return errorMessage;
    }
}

void Lang::LoadLangs() {
    auto initData = std::make_shared<Ship::ResourceInitData>();
    initData->Format = RESOURCE_FORMAT_BINARY;
    initData->Type = static_cast<uint32_t>(Ship::ResourceType::Json);
    initData->ResourceVersion = 0;
    const static std::string folder = "lang/*";
    auto langFiles = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->ListFiles(folder);
    size_t start = std::string(folder).size() - 1;
    for (size_t i = 0; i < langFiles->size(); i++) {
        std::string filePath = langFiles->at(i);
        auto json = std::static_pointer_cast<Ship::Json>(
            Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(filePath, true, initData));

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
            cache.clear();
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