#include "MenuLocalization.h"

#include <algorithm>
#include "soh/Enhancements/Lang/Lang.h"

namespace {

void CopyOptions(WidgetInfo& target, const WidgetInfo& source) {
    using namespace UIWidgets;
    switch (source.type) {
        case WIDGET_CHECKBOX:
        case WIDGET_CVAR_CHECKBOX:
            target.Options(*std::static_pointer_cast<CheckboxOptions>(source.options));
            break;
        case WIDGET_COMBOBOX:
        case WIDGET_CVAR_COMBOBOX:
        case WIDGET_AUDIO_BACKEND:
        case WIDGET_VIDEO_BACKEND:
            target.Options(*std::static_pointer_cast<ComboboxOptions>(source.options));
            break;
        case WIDGET_SLIDER_INT:
        case WIDGET_CVAR_SLIDER_INT:
            target.Options(*std::static_pointer_cast<IntSliderOptions>(source.options));
            break;
        case WIDGET_SLIDER_FLOAT:
        case WIDGET_CVAR_SLIDER_FLOAT:
            target.Options(*std::static_pointer_cast<FloatSliderOptions>(source.options));
            break;
        case WIDGET_CVAR_BTN_SELECTOR:
            target.Options(*std::static_pointer_cast<BtnSelectorOptions>(source.options));
            break;
        case WIDGET_BUTTON:
            target.Options(*std::static_pointer_cast<ButtonOptions>(source.options));
            break;
        case WIDGET_WINDOW_BUTTON:
            target.Options(*std::static_pointer_cast<WindowButtonOptions>(source.options));
            break;
        case WIDGET_COLOR_PICKER:
        case WIDGET_CVAR_COLOR_PICKER:
            target.Options(*std::static_pointer_cast<ColorPickerOptions>(source.options));
            break;
        case WIDGET_TEXT:
        case WIDGET_SEPARATOR_TEXT:
            target.Options(*std::static_pointer_cast<TextOptions>(source.options));
            break;
        default:
            target.options = std::make_shared<WidgetOptions>(*source.options);
            break;
    }
}

std::string WidgetLabel(const WidgetInfo& source) {
    if (source.translationKey.empty() || source.options == nullptr || source.dynamicName)
        return source.name;
    const auto label = source.name.substr(0, source.name.find("##"));
    const auto stableId = source.name.find("###");
    const auto suffix = stableId == std::string::npos
                            ? "###" + (source.cVar ? std::string(source.cVar) : source.translationKey)
                            : source.name.substr(stableId);
    const auto fallback = Lang::UIString(source.translationKey.c_str(), label.c_str());
    const bool formatted = source.type == WIDGET_SLIDER_INT || source.type == WIDGET_CVAR_SLIDER_INT ||
                           source.type == WIDGET_SLIDER_FLOAT || source.type == WIDGET_CVAR_SLIDER_FLOAT;
    std::string translated = formatted ? Lang::UIFormat((source.translationKey + ".name").c_str(), fallback)
                                       : Lang::UIString((source.translationKey + ".name").c_str(), fallback);
    // Plain text and custom renderers display the caption directly, rather than
    // passing it through ImGui's label parser.
    if (source.type == WIDGET_TEXT || source.type == WIDGET_SEPARATOR_TEXT || source.type == WIDGET_CUSTOM)
        return translated;
    return translated + suffix;
}

} // namespace

namespace SohGui {

struct WidgetSearchCache {
    uint64_t revision;
    std::string locale, name, tooltip, key, cvar, extraTerms, sidebar;
    std::string text, normalized;
};

std::string MenuLabel(const std::string& menu) {
    return Lang::TranslateOrFallback("ui.menu." + menu, menu);
}

std::string SidebarLabel(const std::string& menu, const std::string& sidebar) {
    return Lang::TranslateOrFallback("ui.sidebars." + menu + "." + sidebar, sidebar);
}

std::string NormalizeMenuSearch(std::string text) {
    // Fold ASCII only. Calling ::tolower on signed UTF-8 bytes is undefined.
    for (auto& character : text) {
        if (character >= 'A' && character <= 'Z') {
            character += 'a' - 'A';
        }
    }
    std::erase(text, ' ');
    return text;
}

namespace {
const WidgetSearchCache& SearchText(const WidgetInfo& source, std::string_view extraTerms, std::string_view sidebar) {
    const std::string_view locale = Lang::GetUILanguage();
    const auto revision = Lang::GetUICatalogRevision();
    const std::string_view tooltip = source.options ? source.options->tooltip : std::string_view{};
    const std::string_view cvar = source.cVar ? source.cVar : "";
    const auto& previous = source.localizedSearch;
    if (previous && previous->revision == revision && previous->locale == locale && previous->name == source.name &&
        previous->tooltip == tooltip && previous->key == source.translationKey && previous->cvar == cvar &&
        previous->extraTerms == extraTerms && previous->sidebar == sidebar) {
        return *previous;
    }
    auto cached = std::make_shared<WidgetSearchCache>();
    cached->revision = revision;
    cached->locale = locale;
    cached->name = source.name;
    cached->tooltip = tooltip;
    cached->key = source.translationKey;
    cached->cvar = cvar;
    cached->extraTerms = extraTerms;
    cached->sidebar = sidebar;
    // Search needs text only: do not clone widget options, callbacks, combo
    // maps or their translated choices for every invisible widget each frame.
    cached->text = source.name + std::string(tooltip);
    cached->text += WidgetLabel(source);
    if (!source.translationKey.empty()) {
        cached->text += Lang::TranslateOrFallback(source.translationKey + ".description", std::string(tooltip));
        const auto english = Lang::TryTranslateForLanguage((source.translationKey + ".name").c_str(), "en_US");
        if (const auto* value = std::get_if<std::reference_wrapper<const std::string>>(&english))
            cached->text += value->get();
    } else {
        cached->text += tooltip;
    }
    cached->text += extraTerms;
    cached->text += sidebar;
    cached->normalized = NormalizeMenuSearch(cached->text);
    source.localizedSearch = std::move(cached);
    return *source.localizedSearch;
}
} // namespace

const std::string& WidgetSearchText(const WidgetInfo& source) {
    return SearchText(source, {}, {}).text;
}

const std::string& NormalizedWidgetSearchText(const WidgetInfo& source, std::string_view extraTerms,
                                              std::string_view sidebar) {
    return SearchText(source, extraTerms, sidebar).normalized;
}

LocalizedWidget::LocalizedWidget(const WidgetInfo& source) : view(source) {
    if (source.translationKey.empty() || source.options == nullptr) {
        return;
    }
    CopyOptions(view, source);
    view.name = WidgetLabel(source);
    view.options->tooltip = Lang::TranslateOrFallback(source.translationKey + ".description", source.options->tooltip);
    view.options->disabledTooltip = Lang::UIVariant((source.translationKey + ".disabled_messages").c_str(),
                                                    source.options->disabledTooltip.c_str());
    if (source.type == WIDGET_SLIDER_INT || source.type == WIDGET_CVAR_SLIDER_INT) {
        auto options = std::static_pointer_cast<UIWidgets::IntSliderOptions>(view.options);
        options->format = Lang::UIFormatVariant(source.translationKey, options->format);
    } else if (source.type == WIDGET_SLIDER_FLOAT || source.type == WIDGET_CVAR_SLIDER_FLOAT) {
        auto options = std::static_pointer_cast<UIWidgets::FloatSliderOptions>(view.options);
        options->format = Lang::UIFormatVariant(source.translationKey, options->format);
    }
    if (source.type == WIDGET_COMBOBOX || source.type == WIDGET_CVAR_COMBOBOX) {
        const auto original = std::static_pointer_cast<UIWidgets::ComboboxOptions>(source.options);
        const auto translated = std::static_pointer_cast<UIWidgets::ComboboxOptions>(view.options);
        for (const auto& [value, label] : original->comboMap) {
            auto& owned = choices[value];
            owned = Lang::TranslateOrFallback(source.translationKey + ".options." + std::to_string(value),
                                              label ? label : "");
            translated->comboMap[value] = owned.c_str();
        }
    }
}

} // namespace SohGui
