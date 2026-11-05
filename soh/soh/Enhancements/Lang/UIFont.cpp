#include "UIFont.h"
#include "Lang.h"
#include "soh/Enhancements/custom-message/GameText.h"
#include "TranslationCatalog.h"

const ImWchar* Lang::GetUIFontGlyphRanges() {
    static ImVector<ImWchar> ranges;
    if (ranges.empty()) {
        ImFontGlyphRangesBuilder builder;
        builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
        // The Japanese preset omits some punctuation and arrows used by the UI.
        static const ImWchar extraRanges[] = { 0x2000, 0x206F, 0x2190, 0x21FF, 0 };
        builder.AddRanges(extraRanges);
        // Include catalog kanji outside the preset without adding all of CJK.
        const std::string uiText = GetUIFontText();
        builder.AddText(uiText.c_str());
        const std::string itemNames = CustomMessages::GetGameTextCatalog().GetFontText();
        builder.AddText(itemNames.c_str());
        builder.BuildRanges(&ranges);
    }
    return ranges.Data;
}
