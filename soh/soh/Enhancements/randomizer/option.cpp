#include "option.h"
#include "libultraship/bridge.h"
#include <Context.h>
#include <ImGui/imgui.h>
#include "soh/UIWidgets.hpp"

namespace Rando {
Option Option::Bool(std::string name_, std::vector<std::string> options_, OptionCategory category_,
                    std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
                    bool defaultHidden_) {
    return Option(false, std::move(name_), std::move(options_), category_, std::move(cvarName_), std::move(description_),
                  widgetType_, defaultOption_, defaultHidden_);
}

Option Option::Bool(std::string name_, std::string cvarName_, std::string description_, WidgetType widgetType_, bool defaultOption_) {
    return Option(false, std::move(name_), {"Off", "On"}, OptionCategory::Setting, std::move(cvarName_),
                  std::move(description_), widgetType_, defaultOption_, false);
}

Option Option::U8(std::string name_, std::vector<std::string> options_, OptionCategory category_,
                  std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
                  bool defaultHidden_) {
    return Option(uint8_t(0), std::move(name_), std::move(options_), category_, std::move(cvarName_),
                  std::move(description_), widgetType_, defaultOption_, defaultHidden_);
}

Option Option::LogicTrick(std::string name_) {
    return Option(false, std::move(name_), { "Disabled", "Enabled" }, OptionCategory::Setting, std::move(""),
                  std::move(""), WidgetType::Checkbox, 0, 0);
}

Option::operator bool() const {
    if (std::holds_alternative<bool>(var)) {
        return Value<bool>();
    } else {
        return Value<uint8_t>() != 0;
    }
}

size_t Option::GetOptionCount() const {
    return options.size();
}

const std::string& Option::GetName() const {
    return name;
}

uint8_t Option::GetSelectedOptionIndex() const {
    return selectedOption;
}

const std::string& Option::GetSelectedOptionText() const {
    return options[selectedOption];
}

const std::string& Option::GetCVarName() const {
    return cvarName;
}

void Option::SetVariable() {
    if (std::holds_alternative<bool>(var)) {
        var.emplace<bool>(selectedOption != 0);
    } else {
        var.emplace<uint8_t>(selectedOption);
    }
}

void Option::SetCVar() {
    if (!cvarName.empty()) {
        CVarSetInteger(cvarName.c_str(), GetSelectedOptionIndex());
    }
}

void Option::SetFromCVar() {
    if (!cvarName.empty()) {
        SetSelectedIndex(CVarGetInteger(cvarName.c_str(), defaultOption));
    }
}

void Option::SetDelayedOption() {
    delayedOption = selectedOption;
}

void Option::RestoreDelayedOption() {
    selectedOption = delayedOption;
    SetVariable();
}

void Option::SetSelectedIndex(size_t idx) {
    selectedOption = idx;
    if (selectedOption >= options.size()) {
        selectedOption = 0;
    }
    SetVariable();
}

void Option::Hide() {
    hidden = true;
}

void Option::Unhide() {
    hidden = false;
}

bool Option::IsHidden() const {
    return hidden;
}

bool Option::IsCategory(OptionCategory category) const {
    return category == this->category;
}

// Automatically adds newlines to break up text longer than a specified number of characters
// Manually included newlines will still be respected and reset the line length
// If line is midword when it hits the limit, text should break at the last encountered space
char* WrappedText(const char* text, unsigned int charactersPerLine) {
    std::string newText(text);
    const size_t tipLength = newText.length();
    int lastSpace = -1;
    int currentLineLength = 0;
    for (unsigned int currentCharacter = 0; currentCharacter < tipLength; currentCharacter++) {
        if (newText[currentCharacter] == '\n') {
            currentLineLength = 0;
            lastSpace = -1;
            continue;
        } else if (newText[currentCharacter] == ' ') {
            lastSpace = currentCharacter;
        }

        if ((currentLineLength >= charactersPerLine) && (lastSpace >= 0)) {
            newText[lastSpace] = '\n';
            currentLineLength = currentCharacter - lastSpace - 1;
            lastSpace = -1;
        }
        currentLineLength++;
    }

    return strdup(newText.c_str());
}

void Option::RenderImGui() const {
    ImGui::BeginGroup();
    UIWidgets::Spacer(0);
    switch (widgetType) {
        case WidgetType::Checkbox:
            RenderCheckbox();
            break;
        case WidgetType::Combobox:
            RenderCombobox();
            break;
        case WidgetType::Slider:
            RenderSlider();
            break;
    }
    UIWidgets::Spacer(0);
    ImGui::EndGroup();
}

Option::Option(uint8_t var_, std::string name_, std::vector<std::string> options_, OptionCategory category_,
               std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
               bool defaultHidden_)
    : var(var_), name(std::move(name_)), options(std::move(options_)), category(category_),
      cvarName(std::move(cvarName_)), description(std::move(description_)), widgetType(widgetType_),
      defaultOption(defaultOption_), defaultHidden(defaultHidden_) {
    selectedOption = defaultOption;
    hidden = defaultHidden;
    SetVariable();
}
Option::Option(bool var_, std::string name_, std::vector<std::string> options_, OptionCategory category_,
               std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
               bool defaultHidden_)
    : var(var_), name(std::move(name_)), options(std::move(options_)), category(category_),
      cvarName(std::move(cvarName_)), description(std::move(description_)), widgetType(widgetType_),
      defaultOption(defaultOption_), defaultHidden(defaultHidden_) {
    selectedOption = defaultOption;
    hidden = defaultHidden;
    SetVariable();
}

void Option::RenderCheckbox() const {
    bool val = (bool)CVarGetInteger(cvarName.c_str(), defaultOption);
    if (UIWidgets::CustomCheckbox(name.c_str(), &val, false, UIWidgets::CheckboxGraphics::Cross)) {
        CVarSetInteger(cvarName.c_str(), val);
        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    if (!description.empty()) {
        UIWidgets::InsertHelpHoverText(description.c_str());
    }
}

void Option::RenderCombobox() const {
    ImGui::Text("%s", name.c_str());
    uint8_t selected = CVarGetInteger(cvarName.c_str(), defaultOption);
    if (!description.empty()) {
        UIWidgets::InsertHelpHoverText(description.c_str());
    }
    ImGui::BeginGroup();
    std::string comboName = std::string("##") + std::string(cvarName);
    if (ImGui::BeginCombo(comboName.c_str(), options[selected].c_str())) {
        for (uint8_t i = 0; i < options.size(); i++) {
            if (options[i].length() > 0) {
                if (ImGui::Selectable(options[i].c_str(), i == selected)) {
                    CVarSetInteger(cvarName.c_str(), i);
                    selected = i;
                    LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::EndGroup();
}

void Option::RenderSlider() const {
    bool changed = true;
    int val = CVarGetInteger(cvarName.c_str(), defaultOption);
    std::string formatName = name + ": %s";
    ImGui::Text(formatName.c_str(), options[val].c_str());
    if (!description.empty()) {
        UIWidgets::InsertHelpHoverText(description.c_str());
    }
    UIWidgets::Spacer(0);
    ImGui::BeginGroup();
    std::string MinusBTNName = " - ##" + cvarName;
    if (ImGui::Button(MinusBTNName.c_str())) {
        val--;
        changed = true;
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
    ImGui::PushItemWidth(std::min((ImGui::GetContentRegionAvail().x - 30.0f), 260.0f));
    std::string id = "##Slider" + cvarName;
    if (ImGui::SliderInt(id.c_str(), &val, 0, options.size() - 1, "", ImGuiSliderFlags_AlwaysClamp)) {
        changed = true;
    }
    ImGui::PopItemWidth();
    std::string PlusBTNName = " + ##" + cvarName;
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
    if (ImGui::Button(PlusBTNName.c_str())) {
        val++;
        changed = true;
    }
    ImGui::EndGroup();
    if (val < 0) {
        val = 0;
        changed = true;
    }
    if (val > options.size() - 1) {
        val = options.size() - 1;
        changed = true;
    }
    if (changed) {
        CVarSetInteger(cvarName.c_str(), val);
        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
}

OptionGroup::OptionGroup(std::string name, std::vector<Option*> options, OptionGroupType groupType, bool printInSpoiler,
                         OptionGroupType containsType)
    : mName(std::move(name)), mOptions(std::move(options)), mGroupType(groupType), mPrintInSpoiler(printInSpoiler),
      mContainsType(containsType) {
}

OptionGroup::OptionGroup(std::string name, std::vector<OptionGroup*> subGroups, OptionGroupType groupType,
                         bool printInSpoiler, OptionGroupType containsType)
    : mName(std::move(name)), mSubGroups(std::move(subGroups)), mGroupType(groupType), mPrintInSpoiler(printInSpoiler),
      mContainsType(containsType) {
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<Option*> options, bool printInSpoiler) {
    return OptionGroup(std::move(name), std::move(options), OptionGroupType::SUBGROUP, printInSpoiler);
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<OptionGroup*> subGroups, bool printInSpoiler) {
    return OptionGroup(std::move(name), std::move(subGroups), OptionGroupType::SUBGROUP, printInSpoiler,
                       OptionGroupType::SUBGROUP);
}

const std::string& OptionGroup::GetName() const {
    return mName;
}

const std::vector<Option*>& OptionGroup::GetOptions() const {
    return mOptions;
}

const std::vector<OptionGroup*>& OptionGroup::GetSubGroups() const {
    return mSubGroups;
}

bool OptionGroup::PrintInSpoiler() const {
    return mPrintInSpoiler;
}

OptionGroupType OptionGroup::GetGroupType() const {
    return mGroupType;
}

OptionGroupType OptionGroup::GetContainsType() const {
    return mContainsType;
}
} // namespace Rando