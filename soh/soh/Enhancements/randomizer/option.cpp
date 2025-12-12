#include "option.h"
#include "libultraship/bridge.h"
#include <ship/Context.h>
#include <imgui.h>
#include "soh/Enhancements/randomizer/settings.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/SohMenu.h"
#include "soh/SohGui/UIWidgets.hpp"
#include <soh/cvar_prefixes.h>

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

namespace Rando {
Option Option::Bool(RandomizerSettingKey key_, std::string name_, std::vector<std::string> options_,
                    const OptionCategory category_, std::string cvarName_, std::string description_,
                    WidgetType widgetType_, const uint8_t defaultOption_, const bool defaultHidden_,
                    WidgetFunc callback_, int imFlags_) {
    return { static_cast<size_t>(key_),
             std::move(name_),
             std::move(options_),
             category_,
             std::move(cvarName_),
             std::move(description_),
             widgetType_,
             defaultOption_,
             defaultHidden_,
             callback_,
             imFlags_ };
}

Option Option::Bool(RandomizerSettingKey key_, std::string name_, std::string cvarName_, std::string description_,
                    const int imFlags_, const WidgetType widgetType_, const bool defaultOption_, WidgetFunc callback_) {
    return Option(key_, std::move(name_), { "Off", "On" }, OptionCategory::Setting, std::move(cvarName_),
                  std::move(description_), widgetType_, defaultOption_, false, callback_, imFlags_);
}

Option Option::U8(RandomizerSettingKey key_, std::string name_, std::vector<std::string> options_,
                  const OptionCategory category_, std::string cvarName_, std::string description_,
                  WidgetType widgetType_, const uint8_t defaultOption_, const bool defaultHidden_, WidgetFunc callback_,
                  int imFlags_) {
    return { static_cast<size_t>(key_),
             std::move(name_),
             std::move(options_),
             category_,
             std::move(cvarName_),
             std::move(description_),
             widgetType_,
             defaultOption_,
             defaultHidden_,
             callback_,
             imFlags_ };
}

Option Option::LogicTrick(RandomizerTrick rt_, std::string name_) {
    return Option(rt_, std::move(name_), { "Disabled", "Enabled" }, OptionCategory::Setting, "", "",
                  WIDGET_CVAR_CHECKBOX, 0, false, nullptr, IMFLAG_NONE);
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

void Option::SetContextIndex(uint8_t idx) {
    // TODO: Set to Context's OptionValue array.
    contextSelection = idx;
    if (contextSelection > static_cast<uint8_t>(options.size() - 1)) {
        contextSelection = static_cast<uint8_t>(options.size() - 1);
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
        CVarSetInteger(cvarName.c_str(), static_cast<uint8_t>(opts.size() - 1));
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
               bool defaultHidden_, WidgetFunc callback_, int imFlags_)
    : key(key_), name(std::move(name_)), options(std::move(options_)), category(category_),
      cvarName(std::move(cvarName_)), description(std::move(description_)), widgetType(widgetType_),
      defaultOption(defaultOption_), defaultHidden(defaultHidden_), callback(callback_), imFlags(imFlags_) {
    contextSelection = defaultOption;
    hidden = defaultHidden;
    for (int i = 0; i < options.size(); i++) {
        optionsMap.emplace(i, options[i].c_str());
    }
    UIWidgets::LabelPositions labelPosition;
    switch (widgetType) {
        case WIDGET_CVAR_CHECKBOX:
            // labelPosition = UIWidgets::LabelPositions::Near;
            // if (imFlags_ & IMFLAG_LABEL_INLINE) {
            //     labelPosition = UIWidgets::LabelPositions::Near;
            // }
            widgetOptions = std::make_shared<UIWidgets::CheckboxOptions>(
                UIWidgets::CheckboxOptions().DefaultValue(defaultOption).Tooltip(description.c_str()));
            break;
        case WIDGET_CVAR_COMBOBOX:
            labelPosition = UIWidgets::LabelPositions::Above;
            if (imFlags_ & IMFLAG_LABEL_INLINE) {
                labelPosition = UIWidgets::LabelPositions::Near;
            }
            widgetOptions = std::make_shared<UIWidgets::ComboboxOptions>(UIWidgets::ComboboxOptions()
                                                                             .DefaultIndex(defaultOption)
                                                                             .ComboMap(optionsMap)
                                                                             .Tooltip(description.c_str())
                                                                             .LabelPosition(labelPosition));
            break;
        case WIDGET_CVAR_SLIDER_INT:
            labelPosition = UIWidgets::LabelPositions::Above;
            if (imFlags_ & IMFLAG_LABEL_INLINE) {
                labelPosition = UIWidgets::LabelPositions::Near;
            }
            widgetOptions = std::make_shared<UIWidgets::IntSliderOptions>(UIWidgets::IntSliderOptions()
                                                                              .DefaultValue(defaultOption)
                                                                              .Tooltip(description.c_str())
                                                                              .Min(0)
                                                                              .Max(options.size() - 1)
                                                                              .Format(options[defaultOption].c_str())
                                                                              .LabelPosition(labelPosition));
            break;
        default:
            break;
    }
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
    if (selected >= static_cast<uint8_t>(options.size())) {
        selected = static_cast<uint8_t>(options.size());
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
        val = static_cast<int>(options.size()) - 1;
        changed = true;
    }
    UIWidgets::IntSliderOptions widgetOptions = UIWidgets::IntSliderOptions()
                                                    .Color(THEME_COLOR)
                                                    .Min(0)
                                                    .Max(static_cast<uint8_t>(options.size() - 1))
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

void Option::AddWidget(WidgetPath& path) {
    auto widget = SohGui::mSohMenu->AddWidget(path, name, widgetType)
                      .Callback(callback)
                      .PreFunc([this](WidgetInfo& info) {
                          info.isHidden = this->IsHidden();
                          info.options->disabled = this->disabled;
                          info.options->disabledTooltip = this->disabledText.c_str();
                          info.options->tooltip = this->description.c_str();
                          if (info.type == WIDGET_CVAR_SLIDER_INT) {
                              UIWidgets::IntSliderOptions* sliderOpts =
                                  (UIWidgets::IntSliderOptions*)info.options.get();
                              sliderOpts->Format(this->GetOptionText(this->GetOptionIndex()).c_str());
                              sliderOpts->Max(this->options.size() - 1);
                          }
                      })
                      .CVar(cvarName.c_str())
                      .Options(widgetOptions)
                      .SameLine(imFlags & IMFLAG_SAME_LINE);
    widgetInfo = widget;
    if (callback != nullptr) {
        callback(widgetInfo);
    }
}

void Option::PopulateTextToNum() {
    for (uint8_t count = 0; count < options.size(); count++) {
        optionsTextToVar[options[count]] = count;
    }
}

void Option::SetCallback(WidgetFunc callback) {
    this->callback = callback;
}

void Option::RunCallback() {
    if (callback != nullptr) {
        this->callback(widgetInfo);
    }
}

LocationOption::LocationOption(RandomizerCheck key_, const std::string& name_)
    : Option(key_, name_, { "Included", "Excluded" }, OptionCategory::Setting, "", "", WIDGET_CVAR_CHECKBOX,
             RO_LOCATION_INCLUDE, false, nullptr, IMFLAG_NONE) {
}

RandomizerCheck LocationOption::GetKey() const {
    return static_cast<RandomizerCheck>(key);
}

TrickOption::TrickOption(RandomizerTrick key_, const RandomizerCheckQuest quest_, const RandomizerArea area_,
                         std::set<Tricks::Tag> tags_, const std::string& name_, std::string description_)
    : Option(key_, name_, { "Disabled", "Enabled" }, OptionCategory::Setting, "", std::move(description_),
             WIDGET_CVAR_CHECKBOX, 0, false, nullptr, IMFLAG_NONE),
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

void OptionGroup::AddWidgets(WidgetPath& path) const {
    if (mContainerType == WidgetContainerType::TABLE) {
        path.column = SECTION_COLUMN_1;
        path.sidebarName = mName;
        SohGui::mSohMenu->AddSidebarEntry("Randomizer", path.sidebarName, mSubGroups.size());
    }
    if (mContainerType == WidgetContainerType::SECTION || mContainerType == WidgetContainerType::COLUMN) {
        if (!mName.empty()) {
            SohGui::mSohMenu->AddWidget(path, mName.c_str(), WIDGET_SEPARATOR_TEXT);
        }
    }
    if (mContainsType == OptionGroupType::SUBGROUP) {
        for (const auto optionGroup : mSubGroups) {
            optionGroup->AddWidgets(path);
        }
    } else {
        for (const auto option : mOptions) {
            option->AddWidget(path);
        }
    }
    if (mContainerType == WidgetContainerType::COLUMN) {
        assert(path.column < 3);
        path.column = static_cast<SectionColumns>(path.column + 1);
    }
}
} // namespace Rando