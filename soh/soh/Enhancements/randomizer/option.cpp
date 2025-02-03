#include "option.h"
#include "libultraship/bridge.h"
#include <Context.h>
#include <imgui.h>
#include "soh/SohGui/UIWidgets.hpp"

namespace Rando {
Option Option::Bool(RandomizerSettingKey key_, std::string name_, std::vector<std::string> options_,
                    const OptionCategory category_, std::string cvarName_, std::string description_,
                    WidgetType widgetType_, const uint8_t defaultOption_, const bool defaultHidden_, int imFlags_) {
    return {static_cast<size_t>(key_), std::move(name_), std::move(options_), category_,
            std::move(cvarName_), std::move(description_), widgetType_, defaultOption_, defaultHidden_, imFlags_};
}

Option Option::Bool(RandomizerSettingKey key_, std::string name_, std::string cvarName_, std::string description_,
                    const int imFlags_, const WidgetType widgetType_, const bool defaultOption_) {
    return Option(key_, std::move(name_), {"Off", "On"}, OptionCategory::Setting, std::move(cvarName_),
                  std::move(description_), widgetType_, defaultOption_, false, imFlags_);
}

Option Option::U8(RandomizerSettingKey key_, std::string name_, std::vector<std::string> options_,
                  const OptionCategory category_, std::string cvarName_, std::string description_,
                  WidgetType widgetType_, const uint8_t defaultOption_, const bool defaultHidden_, int imFlags_) {
    return {static_cast<size_t>(key_), std::move(name_), std::move(options_), category_, std::move(cvarName_),
                  std::move(description_), widgetType_, defaultOption_, defaultHidden_, imFlags_};
}

Option Option::LogicTrick(RandomizerTrick rt_, std::string name_) {
    return Option(rt_, std::move(name_), { "Disabled", "Enabled" }, OptionCategory::Setting, "",
                  "", WidgetType::Checkbox, 0, false, IMFLAG_NONE);
}

OptionValue::OptionValue(uint8_t val) : mVal(val) {}

uint8_t OptionValue::Get() {
    return mVal;
}

void OptionValue::Set(uint8_t val) {
    mVal = val;
}

OptionValue::operator bool() const {
    return mVal != 0;
}

size_t Option::GetOptionCount() const {
    return options.size();
}

RandomizerSettingKey Option::GetKey() const {
    return static_cast<RandomizerSettingKey>(key);
}

const std::string& Option::GetName() const {
    return name;
}

const std::string& Option::GetDescription() const {
    return description;
}

uint8_t Option::GetMenuOptionIndex() const {
    return menuSelection;
}

const std::string& Option::GetOptionText(size_t index) const {
    return options[index];
}

const std::string& Option::GetCVarName() const {
    return cvarName;
}

void Option::SaveCVar() const {
    if (!cvarName.empty()) {
        CVarSetInteger(cvarName.c_str(), GetMenuOptionIndex());
    }
}

void Option::SetFromCVar() {
    if (!cvarName.empty()) {
        SetMenuIndex(CVarGetInteger(cvarName.c_str(), defaultOption));
    }
}

void Option::SetDelayedOption() {
    delayedSelection = contextSelection;
}

void Option::RestoreDelayedOption() {
    contextSelection = delayedSelection;
}

void Option::SetMenuIndex(size_t idx) {
    menuSelection = idx;
    if (menuSelection > options.size() - 1) {
        menuSelection = options.size() - 1;
    }
}

void Option::SetContextIndex(size_t idx) {
    // TODO: Set to Context's OptionValue array.
    contextSelection = idx;
    if (contextSelection > options.size() - 1) {
        contextSelection = options.size() - 1;
    }
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
    if (menuSelection >= opts.size()) {
        menuSelection = opts.size() - 1;
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

uint8_t Option::GetValueFromText(const std::string text) {
    if (optionsTextToVar.contains(text)) {
        return optionsTextToVar[text];
    } else {
        SPDLOG_ERROR("Option {} does not have a var named {}.", name, text);
        assert(false);
    }
    return defaultOption;
}

void Option::SetContextIndexFromText(const std::string text) {
    if (optionsTextToVar.contains(text)){
        SetContextIndex(optionsTextToVar[text]);
    } else {
        SPDLOG_ERROR("Option {} does not have a var named {}.", name, text);
        assert(false);
    }
}

Option::Option(size_t key_, std::string name_, std::vector<std::string> options_, OptionCategory category_,
               std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
               bool defaultHidden_, int imFlags_)
    : key(key_), name(std::move(name_)), options(std::move(options_)), category(category_),
      cvarName(std::move(cvarName_)), description(std::move(description_)), widgetType(widgetType_),
      defaultOption(defaultOption_), defaultHidden(defaultHidden_), imFlags(imFlags_) {
    menuSelection = contextSelection = defaultOption;
    hidden = defaultHidden;
    PopulateTextToNum();
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
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
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
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
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
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
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
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
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
    int val = GetMenuOptionIndex();
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
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    return changed;
}

void Option::PopulateTextToNum(){
    for (uint8_t count = 0; count < options.size(); count++){
        optionsTextToVar[options[count]] = count;
    }
}

LocationOption::LocationOption(RandomizerCheck key_, const std::string& name_) : 
    Option(key_, name_, {"Included", "Excluded"}, OptionCategory::Setting, "", "", WidgetType::Checkbox,
           RO_LOCATION_INCLUDE, false, IMFLAG_NONE) {}

RandomizerCheck LocationOption::GetKey() const {
    return static_cast<RandomizerCheck>(key);
}

TrickOption::TrickOption(RandomizerTrick key_, const RandomizerCheckQuest quest_, const RandomizerArea area_, std::set<Tricks::Tag> tags_, const std::string& name_, std::string description_) :
    Option(key_, name_, {"Disabled", "Enabled"}, OptionCategory::Setting, "",
        std::move(description_), WidgetType::Checkbox, 0, false, IMFLAG_NONE),
    mQuest(quest_), mArea(area_), mTags(std::move(tags_)) {}

TrickOption TrickOption::LogicTrick(RandomizerTrick key_, RandomizerCheckQuest quest_, RandomizerArea area_, std::set<Tricks::Tag> tags_, const std::string& name_, std::string description_) {
    return {key_, quest_, area_, std::move(tags_), name_, std::move(description_)};
}

RandomizerTrick TrickOption::GetKey() const {
    return static_cast<RandomizerTrick>(key);
}

RandomizerCheckQuest TrickOption::GetQuest() const {
    return mQuest;
}

RandomizerArea TrickOption::GetArea() const {
    return mArea;
}

bool TrickOption::HasTag(const Tricks::Tag tag) const {
    return mTags.contains(tag);
}

const std::set<Tricks::Tag>& TrickOption::GetTags() const {
    return mTags;
}

OptionGroup::OptionGroup(std::string name, std::vector<Option*> options, const OptionGroupType groupType,
                         const WidgetContainerType containerType, std::string description)
    : mName(std::move(name)), mOptions(std::move(options)), mGroupType(groupType),
      mContainerType(containerType), mDescription(std::move(description)) {
}

OptionGroup::OptionGroup(std::string name, std::vector<OptionGroup*> subGroups, const OptionGroupType groupType,
                         const WidgetContainerType containerType, std::string description)
    : mName(std::move(name)), mSubGroups(std::move(subGroups)), mGroupType(groupType),
      mContainsType(OptionGroupType::SUBGROUP), mContainerType(containerType), mDescription(std::move(description)) {
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<Option*> options,
                                  const WidgetContainerType containerType, std::string description) {
    return {std::move(name), std::move(options), OptionGroupType::SUBGROUP, containerType,
                       std::move(description)};
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<OptionGroup*> subGroups,
                                  const WidgetContainerType containerType, std::string description) {
    return {std::move(name), std::move(subGroups), OptionGroupType::SUBGROUP, containerType,
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
                UIWidgets::PaddedSeparator(false, true);
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