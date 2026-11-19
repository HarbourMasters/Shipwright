#include "Lang.h"

#include "soh/OTRGlobals.h"
#include "soh/SohGui/SohMenu.h"

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

namespace {

void DrawUILanguage(WidgetInfo&) {
    ImGui::PushFont(OTRGlobals::Instance->fontJapaneseLarger);
    ImGui::TextUnformatted(Lang::Translate("ui.language.title").c_str());
    const std::string selected = Lang::GetUILanguage();
    const auto& languages = Lang::GetAvailableUILanguages();
    const auto current = languages.find(selected);
    const char* preview = current == languages.end() ? selected.c_str() : current->second.c_str();
    // Native language names must also be readable before Japanese is selected.
    if (ImGui::BeginCombo("###UILanguage", preview)) {
        for (const auto& [locale, name] : languages) {
            const bool active = locale == selected;
            if (ImGui::Selectable((name + "###UILanguage." + locale).c_str(), active)) {
                Lang::SetUILanguage(locale);
            }
            if (active) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::TextWrapped("%s", Lang::Translate("ui.language.description").c_str());
    ImGui::Spacing();
    ImGui::TextWrapped("%s", Lang::Translate("ui.language.translation_status").c_str());
    if (Lang::IsUsingBuiltinUICatalog()) {
        ImGui::Spacing();
        ImGui::TextWrapped("%s", Lang::Translate("ui.language.catalog_error").c_str());
    }
    ImGui::PopFont();
}

void RegisterUILanguageMenu() {
    SohGui::mSohMenu->AddSidebarEntry("Settings", "UI Language", 1);
    WidgetPath path = { "Settings", "UI Language", SECTION_COLUMN_1 };
    SohGui::mSohMenu->AddWidget(path, "UI Language", WIDGET_CUSTOM)
        .Translation("ui.panels.uilanguagemenu.ui_language")
        .CustomFunction(DrawUILanguage)
        .RaceDisable(false)
        .HideInSearch(true);
}

RegisterMenuInitFunc registerUILanguageMenu(RegisterUILanguageMenu);

} // namespace
