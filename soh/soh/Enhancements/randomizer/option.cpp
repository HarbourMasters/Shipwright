#include "option.h"
#include "libultraship/bridge.h"
#include <Context.h>
#include <ImGui/imgui.h>
#include "soh/UIWidgets.hpp"

namespace Rando {
Option Option::Bool(std::string name_, std::vector<std::string> options_, OptionCategory category_,
                    std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
                    bool defaultHidden_, int imFlags_) {
    return Option(false, std::move(name_), std::move(options_), category_, std::move(cvarName_), std::move(description_),
                  widgetType_, defaultOption_, defaultHidden_, imFlags_);
}

Option Option::Bool(std::string name_, std::string cvarName_, std::string description_, int imFlags_, WidgetType widgetType_, bool defaultOption_) {
    return Option(false, std::move(name_), {"Off", "On"}, OptionCategory::Setting, std::move(cvarName_),
                  std::move(description_), widgetType_, defaultOption_, false, imFlags_);
}

Option Option::U8(std::string name_, std::vector<std::string> options_, OptionCategory category_,
                  std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
                  bool defaultHidden_, int imFlags_) {
    return Option(uint8_t(0), std::move(name_), std::move(options_), category_, std::move(cvarName_),
                  std::move(description_), widgetType_, defaultOption_, defaultHidden_, imFlags_);
}

Option Option::LogicTrick(std::string name_) {
    return Option(false, std::move(name_), { "Disabled", "Enabled" }, OptionCategory::Setting, std::move(""),
                  std::move(""), WidgetType::Checkbox, 0, 0, IMFLAG_NONE);
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

void Option::ChangeOptions(std::vector<std::string> opts) {
    if (selectedOption >= opts.size()) {
        selectedOption = opts.size() - 1;
    }
    options = std::move(opts);
}

void Option::Enable() {
    disabled = false;
}

void Option::Disable(std::string text, UIWidgets::CheckboxGraphics graphic) {
    if (!disabled || disabledText != text || disabledGraphic != graphic) {
        disabled = true;
        disabledText = std::move(text);
        disabledGraphic = graphic;
    }
}

bool Option::IsCategory(OptionCategory category) const {
    return category == this->category;
}

bool Option::RenderImGui() const {
    bool changed = false;
    ImGui::BeginGroup();
    switch (widgetType) {
        case WidgetType::Checkbox:
            changed = RenderCheckbox();
            break;
        case WidgetType::Combobox:
            changed = RenderCombobox();
            break;
        case WidgetType::Slider:
            changed = RenderSlider();
            break;
    }
    UIWidgets::Spacer(0);
    ImGui::EndGroup();
    return changed;
}

bool Option::HasFlag(int imFlag_) const {
    return imFlag_ & imFlags;
}

void Option::AddFlag(int imFlag_) {
    imFlags |= imFlag_;
}

void Option::SetFlag(int imFlag_) {
    imFlags = imFlag_;
}

void Option::RemoveFlag(int imFlag_) {
    imFlags &= ~(imFlag_);
}

Option::Option(uint8_t var_, std::string name_, std::vector<std::string> options_, OptionCategory category_,
               std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
               bool defaultHidden_, int imFlags_)
    : var(var_), name(std::move(name_)), options(std::move(options_)), category(category_),
      cvarName(std::move(cvarName_)), description(std::move(description_)), widgetType(widgetType_),
      defaultOption(defaultOption_), defaultHidden(defaultHidden_), imFlags(imFlags_) {
    selectedOption = defaultOption;
    hidden = defaultHidden;
    SetVariable();
}
Option::Option(bool var_, std::string name_, std::vector<std::string> options_, OptionCategory category_,
               std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
               bool defaultHidden_, int imFlags_)
    : var(var_), name(std::move(name_)), options(std::move(options_)), category(category_),
      cvarName(std::move(cvarName_)), description(std::move(description_)), widgetType(widgetType_),
      defaultOption(defaultOption_), defaultHidden(defaultHidden_), imFlags(imFlags_) {
    selectedOption = defaultOption;
    hidden = defaultHidden;
    SetVariable();
}

bool Option::RenderCheckbox() const {
    bool changed = false;
    if (disabled) {
        UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
    }
    bool val = (bool)CVarGetInteger(cvarName.c_str(), defaultOption);
    if (UIWidgets::CustomCheckbox(name.c_str(), &val, disabled, disabledGraphic)) {
        CVarSetInteger(cvarName.c_str(), val);
        changed = true;
        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    if (!description.empty()) {
        UIWidgets::InsertHelpHoverText(description.c_str());
    }
    if (disabled) {
        UIWidgets::ReEnableComponent(disabledText.c_str());
    }
    return changed;
}

bool Option::RenderCombobox() const {
    bool changed = false;
    if (disabled) {
        UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
    }
    ImGui::Text("%s", name.c_str());
    uint8_t selected = CVarGetInteger(cvarName.c_str(), defaultOption);
    if (selected >= options.size()) {
        selected--;
        CVarSetInteger(cvarName.c_str(), selected);
        changed = true;
        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
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
                    changed = true;
                    selected = i;
                    LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::EndGroup();
    if (disabled) {
        UIWidgets::ReEnableComponent(disabledText.c_str());
    }
    return changed;
}

bool Option::RenderSlider() const {
    bool changed = false;
    int val = CVarGetInteger(cvarName.c_str(), defaultOption);
    if (val >= options.size()) {
        val--;
        changed = true;
    }
    if (disabled) {
        UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
    }
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
    if (disabled) {
        UIWidgets::ReEnableComponent(disabledText.c_str());
    }
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
    return changed;
}

OptionGroup::OptionGroup(std::string name, std::vector<Option*> options, OptionGroupType groupType, bool printInSpoiler,
                         WidgetContainerType containerType, std::string description)
    : mName(std::move(name)), mOptions(std::move(options)), mGroupType(groupType), mPrintInSpoiler(printInSpoiler),
      mContainsType(OptionGroupType::DEFAULT), mContainerType(containerType), mDescription(std::move(description)) {
}

OptionGroup::OptionGroup(std::string name, std::vector<OptionGroup*> subGroups, OptionGroupType groupType,
                         bool printInSpoiler, WidgetContainerType containerType, std::string description)
    : mName(std::move(name)), mSubGroups(std::move(subGroups)), mGroupType(groupType), mPrintInSpoiler(printInSpoiler),
      mContainsType(OptionGroupType::SUBGROUP), mContainerType(containerType), mDescription(std::move(description)) {
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<Option*> options, bool printInSpoiler,
                                  WidgetContainerType containerType, std::string description) {
    return OptionGroup(std::move(name), std::move(options), OptionGroupType::SUBGROUP, printInSpoiler, containerType,
                       std::move(description));
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<OptionGroup*> subGroups, bool printInSpoiler,
                                  WidgetContainerType containerType, std::string description) {
    return OptionGroup(std::move(name), std::move(subGroups), OptionGroupType::SUBGROUP, printInSpoiler, containerType,
                       std::move(description));
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

WidgetContainerType OptionGroup::GetContainerType() const {
    return mContainerType;
}

const std::string& OptionGroup::GetDescription() const {
    return mDescription;
}

bool OptionGroup::RenderImGui() const {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    bool changed = false;
    if (mContainerType == WidgetContainerType::TABLE) {
        if (ImGui::BeginTable(mName.c_str(), mSubGroups.size(), ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            for (auto column : mSubGroups) {
                if (column->GetContainerType() == WidgetContainerType::COLUMN) {
                    ImGui::TableSetupColumn(column->GetName().c_str(), ImGuiTableColumnFlags_WidthStretch, 200.0f);
                }
            }
            ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            for (int i = 0; i < mSubGroups.size(); i++) {
                ImGui::TableSetColumnIndex(i);
                ImGui::TableHeader(mSubGroups[i]->GetName().c_str());
                if (!mSubGroups[i]->GetDescription().empty()) {
                    UIWidgets::SetLastItemHoverText(mSubGroups[i]->GetDescription().c_str());
                }
            }
            ImGui::PopItemFlag();
            ImGui::TableNextRow();
        }
    }
    if (mContainerType == WidgetContainerType::SECTION && !mName.empty()) {
        UIWidgets::PaddedSeparator();
        ImGui::Text("%s", mName.c_str());
        if (!mDescription.empty()) {
            UIWidgets::InsertHelpHoverText(mDescription.c_str());
        }
        UIWidgets::PaddedSeparator();
    }
    if (mContainerType == WidgetContainerType::COLUMN) {
        ImGui::TableNextColumn();
        window->DC.CurrLineTextBaseOffset = 0.0f;
        ImGui::BeginChild(mName.c_str(), ImVec2(0, -8));
        ImGui::PushItemWidth(-FLT_MIN);
    }
    if (mContainsType == OptionGroupType::SUBGROUP) {
        for (auto optionGroup : mSubGroups) {
            if (optionGroup->RenderImGui()) {
                changed = true;
            }
        }
    } else {
        for (auto option : mOptions) {
            if (option->IsHidden()) {
                continue;
            }
            if (option->HasFlag(IMFLAG_INDENT)) {
                ImGui::Indent();
            }
            // If any options changed, changed will end up being true
            if (option->RenderImGui()) {
                changed = true;
            }
            if (option->HasFlag(IMFLAG_UNINDENT)) {
                ImGui::Unindent();
            }
            if (option->HasFlag(IMFLAG_SEPARATOR_BOTTOM)) {
                UIWidgets::PaddedSeparator();
            }
        }
    }
    if (mContainerType == WidgetContainerType::COLUMN) {
        ImGui::EndChild();
    }
    if (mContainerType == WidgetContainerType::TABLE) {
        ImGui::EndTable();
    }
    return changed;
}
} // namespace Rando