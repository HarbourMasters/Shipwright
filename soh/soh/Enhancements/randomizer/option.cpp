#include "option.h"
#include "libultraship/bridge.h"
#include <Context.h>
#include <imgui.h>
#include "soh/UIWidgets.hpp"

namespace Rando {
Option Option::Bool(std::string name_, std::vector<std::string> options_, const OptionCategory category_,
                    std::string cvarName_, std::string description_, WidgetType widgetType_, const uint8_t defaultOption_,
                    const bool defaultHidden_, int imFlags_) {
    return {false, std::move(name_), std::move(options_), category_, std::move(cvarName_), std::move(description_),
                  widgetType_, defaultOption_, defaultHidden_, imFlags_};
}

Option Option::Bool(std::string name_, std::string cvarName_, std::string description_, const int imFlags_,
    const WidgetType widgetType_, const bool defaultOption_) {
    return Option(false, std::move(name_), {"Off", "On"}, OptionCategory::Setting, std::move(cvarName_),
                  std::move(description_), widgetType_, defaultOption_, false, imFlags_);
}

Option Option::U8(std::string name_, std::vector<std::string> options_, const OptionCategory category_,
                  std::string cvarName_, std::string description_, WidgetType widgetType_, const uint8_t defaultOption_,
                  const bool defaultHidden_, int imFlags_) {
    return {static_cast<uint8_t>(0), std::move(name_), std::move(options_), category_, std::move(cvarName_),
                  std::move(description_), widgetType_, defaultOption_, defaultHidden_, imFlags_};
}

Option Option::LogicTrick(std::string name_) {
    return Option(false, std::move(name_), { "Disabled", "Enabled" }, OptionCategory::Setting, "",
                  "", WidgetType::Checkbox, 0, false, IMFLAG_NONE);
}

Option::operator bool() const {
    if (std::holds_alternative<bool>(var)) {
        return Value<bool>();
    }
    return Value<uint8_t>() != 0;
}

size_t Option::GetOptionCount() const {
    return options.size();
}

const std::string& Option::GetName() const {
    return name;
}

const std::string& Option::GetDescription() const {
    return description;
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

void Option::SetCVar() const {
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
    if (selectedOption > options.size() - 1) {
        selectedOption = options.size() - 1;
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

void Option::Disable(std::string text, const UIWidgets::CheckboxGraphics graphic) {
    if (!disabled || disabledText != text || disabledGraphic != graphic) {
        disabled = true;
        disabledText = std::move(text);
        disabledGraphic = graphic;
    }
}

bool Option::IsCategory(const OptionCategory category) const {
    return category == this->category;
}

bool Option::RenderImGui() {
    bool changed = false;
    ImGui::BeginGroup();
    switch (widgetType) {
        case WidgetType::Checkbox:
            changed = RenderCheckbox();
            break;
        case WidgetType::TristateCheckbox:
            changed = RenderTristateCheckbox();
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

bool Option::HasFlag(const int imFlag_) const {
    return imFlag_ & imFlags;
}

void Option::AddFlag(const int imFlag_) {
    imFlags |= imFlag_;
}

void Option::SetFlag(const int imFlag_) {
    imFlags = imFlag_;
}

void Option::RemoveFlag(const int imFlag_) {
    imFlags &= ~imFlag_;
}

Option::Option(uint8_t var_, std::string name_, std::vector<std::string> options_, OptionCategory category_,
               std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
               bool defaultHidden_, int imFlags_)
    : var(var_), name(std::move(name_)), options(std::move(options_)), category(category_),
      cvarName(std::move(cvarName_)), description(std::move(description_)), widgetType(widgetType_),
      defaultOption(defaultOption_), defaultHidden(defaultHidden_), imFlags(imFlags_) {
    selectedOption = defaultOption;
    hidden = defaultHidden;
    SetFromCVar();
}
Option::Option(bool var_, std::string name_, std::vector<std::string> options_, const OptionCategory category_,
               std::string cvarName_, std::string description_, WidgetType widgetType_, const uint8_t defaultOption_,
               const bool defaultHidden_, int imFlags_)
    : var(var_), name(std::move(name_)), options(std::move(options_)), category(category_),
      cvarName(std::move(cvarName_)), description(std::move(description_)), widgetType(widgetType_),
      defaultOption(defaultOption_), defaultHidden(defaultHidden_), imFlags(imFlags_) {
    selectedOption = defaultOption;
    hidden = defaultHidden;
    SetFromCVar();
}

bool Option::RenderCheckbox() {
    bool changed = false;
    if (disabled) {
        UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
    }
    bool val = static_cast<bool>(CVarGetInteger(cvarName.c_str(), defaultOption));
    if (CustomCheckbox(name.c_str(), &val, disabled, disabledGraphic)) {
        CVarSetInteger(cvarName.c_str(), val);
        changed = true;
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    if (!description.empty()) {
        UIWidgets::InsertHelpHoverText(description.c_str());
    }
    if (disabled) {
        UIWidgets::ReEnableComponent(disabledText.c_str());
    }
    return changed;
}

bool Option::RenderTristateCheckbox() {
    bool changed = false;
    if (disabled) {
        UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
    }
    int val = CVarGetInteger(cvarName.c_str(), defaultOption);
    if (CustomCheckboxTristate(name.c_str(), &val, disabled, disabledGraphic)) {
        CVarSetInteger(cvarName.c_str(), val);
        changed = true;
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    if (!description.empty()) {
        UIWidgets::InsertHelpHoverText(description.c_str());
    }
    if (disabled) {
        UIWidgets::ReEnableComponent(disabledText.c_str());
    }
    return changed;
}

bool Option::RenderCombobox() {
    bool changed = false;
    if (disabled) {
        UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
    }
    ImGui::Text("%s", name.c_str());
    uint8_t selected = CVarGetInteger(cvarName.c_str(), defaultOption);
    if (selected >= options.size()) {
        selected = options.size();
        CVarSetInteger(cvarName.c_str(), selected);
        changed = true;
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    if (!description.empty()) {
        UIWidgets::InsertHelpHoverText(description.c_str());
    }
    const std::string comboName = std::string("##") + std::string(cvarName);
    if (ImGui::BeginCombo(comboName.c_str(), options[selected].c_str())) {
        for (size_t i = 0; i < options.size(); i++) {
            if (!options[i].empty()) {
                if (ImGui::Selectable(options[i].c_str(), i == selected)) {
                    CVarSetInteger(cvarName.c_str(), static_cast<int>(i));
                    changed = true;
                    selected = i;
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                }
            }
        }
        ImGui::EndCombo();
    }
    if (disabled) {
        UIWidgets::ReEnableComponent(disabledText.c_str());
    }
    return changed;
}

bool Option::RenderSlider() {
    bool changed = false;
    int val = GetSelectedOptionIndex();
    if (val > options.size() - 1) {
        val = options.size() - 1;
        CVarSetInteger(cvarName.c_str(), val);
        changed = true;
    }
    if (disabled) {
        UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
    }
    const std::string formatName = name + ": %s";
    ImGui::Text(formatName.c_str(), options[val].c_str());
    if (!description.empty()) {
        UIWidgets::InsertHelpHoverText(description.c_str());
    }
    UIWidgets::Spacer(0);
    ImGui::BeginGroup();
    const std::string MinusBTNName = " - ##" + cvarName;
    if (ImGui::Button(MinusBTNName.c_str())) {
        val--;
        changed = true;
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
    ImGui::PushItemWidth(std::min(ImGui::GetContentRegionAvail().x - 30.0f, 260.0f));
    const std::string id = "##Slider" + cvarName;
    if (ImGui::SliderInt(id.c_str(), &val, 0, static_cast<int>(options.size()) - 1, "", ImGuiSliderFlags_AlwaysClamp)) {
        changed = true;
    }
    ImGui::PopItemWidth();
    const std::string PlusBTNName = " + ##" + cvarName;
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
        val = static_cast<int>(options.size() - 1);
        changed = true;
    }
    if (changed) {
        CVarSetInteger(cvarName.c_str(), val);
        SetFromCVar();
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    return changed;
}

TrickOption::TrickOption(const RandomizerCheckQuest quest_, const RandomizerArea area_, std::set<Tricks::Tag> tags_, const bool glitch_, const std::string& name_, std::string description_) :
    Option(false, name_, {"Disabled", "Enabled"}, OptionCategory::Setting, "",
        std::move(description_), WidgetType::Checkbox, 0, false, IMFLAG_NONE),
    mQuest(quest_), mArea(area_), mTags(std::move(tags_)), mGlitch(glitch_) {}

TrickOption TrickOption::LogicTrick(RandomizerCheckQuest quest_, RandomizerArea area_, std::set<Tricks::Tag> tags_, bool glitch_, const std::string& name_, std::string description_) {
    return {quest_, area_, std::move(tags_), glitch_, name_, std::move(description_)};
}

RandomizerCheckQuest TrickOption::GetQuest() const {
    return mQuest;
}

RandomizerArea TrickOption::GetArea() const {
    return mArea;
}

bool TrickOption::IsGlitch() const {
    return mGlitch;
}

bool TrickOption::HasTag(const Tricks::Tag tag) const {
    return mTags.contains(tag);
}

const std::set<Tricks::Tag>& TrickOption::GetTags() const {
    return mTags;
}

OptionGroup::OptionGroup(std::string name, std::vector<Option*> options, const OptionGroupType groupType,
                         const bool printInSpoiler, const WidgetContainerType containerType, std::string description)
    : mName(std::move(name)), mOptions(std::move(options)), mGroupType(groupType), mPrintInSpoiler(printInSpoiler),
      mContainerType(containerType), mDescription(std::move(description)) {
}

OptionGroup::OptionGroup(std::string name, std::vector<OptionGroup*> subGroups, const OptionGroupType groupType,
                         const bool printInSpoiler, const WidgetContainerType containerType, std::string description)
    : mName(std::move(name)), mSubGroups(std::move(subGroups)), mGroupType(groupType), mPrintInSpoiler(printInSpoiler),
      mContainsType(OptionGroupType::SUBGROUP), mContainerType(containerType), mDescription(std::move(description)) {
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<Option*> options, const bool printInSpoiler,
                                  const WidgetContainerType containerType, std::string description) {
    return {std::move(name), std::move(options), OptionGroupType::SUBGROUP, printInSpoiler, containerType,
                       std::move(description)};
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<OptionGroup*> subGroups, const bool printInSpoiler,
                                  const WidgetContainerType containerType, std::string description) {
    return {std::move(name), std::move(subGroups), OptionGroupType::SUBGROUP, printInSpoiler, containerType,
                       std::move(description)};
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

void OptionGroup::Enable() {
    mDisabled = false;
}

void OptionGroup::Disable() {
    mDisabled = true;
}

bool OptionGroup::RenderImGui() const { // NOLINT(*-no-recursion)
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    bool changed = false;
    ImGui::BeginDisabled(mDisabled);
    if (mContainerType == WidgetContainerType::TABLE) {
        if (ImGui::BeginTable(mName.c_str(), static_cast<int>(mSubGroups.size()), ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            for (const auto column : mSubGroups) {
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
        for (const auto optionGroup : mSubGroups) {
            if (optionGroup->RenderImGui()) {
                changed = true;
            }
        }
    } else {
        for (const auto option : mOptions) {
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
    ImGui::EndDisabled();
    return changed;
}
} // namespace Rando