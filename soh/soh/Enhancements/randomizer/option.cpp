#include "option.h"
#include "libultraship/bridge.h"
#include <Context.h>
#include <imgui.h>
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/UIWidgets.hpp"
#include <soh/cvar_prefixes.h>

namespace Rando {
Option Option::Bool(RandomizerSettingKey key_, std::string name_, std::vector<std::string> options_,
                    const OptionCategory category_, std::string cvarName_, std::string description_,
                    WidgetType widgetType_, const uint8_t defaultOption_, const bool defaultHidden_, int imFlags_) {
    return { static_cast<size_t>(key_), std::move(name_), std::move(options_), category_,      std::move(cvarName_),
             std::move(description_),   widgetType_,      defaultOption_,      defaultHidden_, imFlags_ };
}

Option Option::Bool(RandomizerSettingKey key_, std::string name_, std::string cvarName_, std::string description_,
                    const int imFlags_, const WidgetType widgetType_, const bool defaultOption_) {
    return Option(key_, std::move(name_), { "Off", "On" }, OptionCategory::Setting, std::move(cvarName_),
                  std::move(description_), widgetType_, defaultOption_, false, imFlags_);
}

Option Option::U8(RandomizerSettingKey key_, std::string name_, std::vector<std::string> options_,
                  const OptionCategory category_, std::string cvarName_, std::string description_,
                  WidgetType widgetType_, const uint8_t defaultOption_, const bool defaultHidden_, int imFlags_) {
    return { static_cast<size_t>(key_), std::move(name_), std::move(options_), category_,      std::move(cvarName_),
             std::move(description_),   widgetType_,      defaultOption_,      defaultHidden_, imFlags_ };
}

Option Option::LogicTrick(RandomizerTrick rt_, std::string name_) {
    return Option(rt_, std::move(name_), { "Disabled", "Enabled" }, OptionCategory::Setting, "", "",
                  WidgetType::Checkbox, 0, false, IMFLAG_NONE);
}

OptionValue::OptionValue(uint8_t val) : mVal(val) {
}

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

uint8_t Option::GetOptionIndex() const {
    return CVarGetInteger(cvarName.c_str(), defaultOption);
}

const std::string& Option::GetOptionText(size_t index) const {
    return options[index];
}

const std::string& Option::GetCVarName() const {
    return cvarName;
}

void Option::SetDelayedOption() {
    delayedSelection = contextSelection;
}

void Option::RestoreDelayedOption() {
    contextSelection = delayedSelection;
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
    if (GetOptionIndex() >= opts.size()) {
        CVarSetInteger(cvarName.c_str(), opts.size() - 1);
    }
    options = std::move(opts);
}

void Option::Enable() {
    disabled = false;
}

void Option::Disable(std::string text) {
    if (!disabled || disabledText != text) {
        disabled = true;
        disabledText = std::move(text);
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
        case WidgetType::Combobox:
            changed = RenderCombobox();
            break;
        case WidgetType::Slider:
            changed = RenderSlider();
            break;
    }
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
    if (optionsTextToVar.contains(text)) {
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
    contextSelection = defaultOption;
    hidden = defaultHidden;
    PopulateTextToNum();
}

bool Option::RenderCheckbox() {
    bool changed = false;
    bool val = static_cast<bool>(CVarGetInteger(cvarName.c_str(), defaultOption));
    UIWidgets::CheckboxOptions widgetOptions = static_cast<UIWidgets::CheckboxOptions>(
        UIWidgets::CheckboxOptions().Color(THEME_COLOR).Tooltip(description.c_str()));
    widgetOptions.disabled = disabled;
    if (UIWidgets::Checkbox(name.c_str(), &val, widgetOptions)) {
        CVarSetInteger(cvarName.c_str(), val);
        changed = true;
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    return changed;
}

bool Option::RenderCombobox() {
    bool changed = false;
    uint8_t selected = CVarGetInteger(cvarName.c_str(), defaultOption);
    if (selected >= options.size()) {
        selected = options.size();
        CVarSetInteger(cvarName.c_str(), selected);
        changed = true;
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    UIWidgets::ComboboxOptions widgetOptions =
        UIWidgets::ComboboxOptions().Color(THEME_COLOR).Tooltip(description.c_str());
    if (this->GetKey() == RSK_LOGIC_RULES) {
        widgetOptions = widgetOptions.LabelPosition(UIWidgets::LabelPositions::None)
                            .ComponentAlignment(UIWidgets::ComponentAlignments::Right);
    }
    widgetOptions.disabled = disabled;
    if (UIWidgets::Combobox(name.c_str(), &selected, options, widgetOptions)) {
        CVarSetInteger(cvarName.c_str(), static_cast<int>(selected));
        changed = true;
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    return changed;
}

bool Option::RenderSlider() {
    bool changed = false;
    int val = CVarGetInteger(cvarName.c_str(), defaultOption);
    if (val > options.size() - 1) {
        val = options.size() - 1;
        changed = true;
    }
    UIWidgets::IntSliderOptions widgetOptions = UIWidgets::IntSliderOptions()
                                                    .Color(THEME_COLOR)
                                                    .Min(0)
                                                    .Max(options.size() - 1)
                                                    .Tooltip(description.c_str())
                                                    .Format(options[val].c_str())
                                                    .DefaultValue(defaultOption);
    widgetOptions.disabled = disabled;
    if (UIWidgets::SliderInt(name.c_str(), &val, widgetOptions)) {
        changed = true;
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
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    return changed;
}

void Option::PopulateTextToNum() {
    for (uint8_t count = 0; count < options.size(); count++) {
        optionsTextToVar[options[count]] = count;
    }
}

LocationOption::LocationOption(RandomizerCheck key_, const std::string& name_)
    : Option(key_, name_, { "Included", "Excluded" }, OptionCategory::Setting, "", "", WidgetType::Checkbox,
             RO_LOCATION_INCLUDE, false, IMFLAG_NONE) {
}

RandomizerCheck LocationOption::GetKey() const {
    return static_cast<RandomizerCheck>(key);
}

TrickOption::TrickOption(RandomizerTrick key_, const RandomizerCheckQuest quest_, const RandomizerArea area_,
                         std::set<Tricks::Tag> tags_, const std::string& name_, std::string description_)
    : Option(key_, name_, { "Disabled", "Enabled" }, OptionCategory::Setting, "", std::move(description_),
             WidgetType::Checkbox, 0, false, IMFLAG_NONE),
      mQuest(quest_), mArea(area_), mTags(std::move(tags_)) {
}

TrickOption TrickOption::LogicTrick(RandomizerTrick key_, RandomizerCheckQuest quest_, RandomizerArea area_,
                                    std::set<Tricks::Tag> tags_, const std::string& name_, std::string description_) {
    return { key_, quest_, area_, std::move(tags_), name_, std::move(description_) };
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
    : mName(std::move(name)), mOptions(std::move(options)), mGroupType(groupType), mContainerType(containerType),
      mDescription(std::move(description)) {
}

OptionGroup::OptionGroup(std::string name, std::vector<OptionGroup*> subGroups, const OptionGroupType groupType,
                         const WidgetContainerType containerType, std::string description)
    : mName(std::move(name)), mSubGroups(std::move(subGroups)), mGroupType(groupType),
      mContainsType(OptionGroupType::SUBGROUP), mContainerType(containerType), mDescription(std::move(description)) {
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<Option*> options,
                                  const WidgetContainerType containerType, std::string description) {
    return { std::move(name), std::move(options), OptionGroupType::SUBGROUP, containerType, std::move(description) };
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<OptionGroup*> subGroups,
                                  const WidgetContainerType containerType, std::string description) {
    return { std::move(name), std::move(subGroups), OptionGroupType::SUBGROUP, containerType, std::move(description) };
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
    ImGui::BeginDisabled(mDisabled || CVarGetInteger(CVAR_SETTING("DisableChanges"), 0) ||
                         CVarGetInteger(CVAR_GENERAL("RandoGenerating"), 0) ||
                         CVarGetInteger(CVAR_GENERAL("OnFileSelectNameEntry"), 0));
    if (mContainerType == WidgetContainerType::TABLE) {
        if (ImGui::BeginTable(mName.c_str(), static_cast<int>(mSubGroups.size()),
                              ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
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
                    UIWidgets::Tooltip(mSubGroups[i]->GetDescription().c_str());
                }
            }
            ImGui::PopItemFlag();
            ImGui::TableNextRow();
        }
    }
    if (mContainerType == WidgetContainerType::SECTION && !mName.empty()) {
        ImGui::SeparatorText(mName.c_str());
        if (!mDescription.empty()) {
            UIWidgets::Tooltip(mDescription.c_str());
        }
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