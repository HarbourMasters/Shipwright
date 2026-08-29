#include <libultraship/bridge/consolevariablebridge.h>

#include "option.h"
#include "soh/SohGui/SohMenu.h"
#include "soh/Enhancements/Lang/Lang.h"
#include <soh/cvar_prefixes.h>

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

namespace Rando {
Option Option::Bool(RandomizerSettingKey key_, std::vector<std::string> options_, const OptionCategory category_,
                    std::string cvarName_, WidgetType widgetType_, const uint8_t defaultOption_,
                    const bool defaultHidden_, WidgetFunc callback_, int imFlags_) {
    return { static_cast<size_t>(key_),
             std::move(options_),
             category_,
             std::move(cvarName_),
             widgetType_,
             defaultOption_,
             defaultHidden_,
             callback_,
             imFlags_ };
}

Option Option::Bool(RandomizerSettingKey key_, std::string cvarName_, const int imFlags_, const WidgetType widgetType_,
                    const bool defaultOption_, WidgetFunc callback_) {
    return Option(key_, { "Off", "On" }, OptionCategory::Setting, std::move(cvarName_), widgetType_, defaultOption_,
                  false, callback_, imFlags_);
}

Option Option::U8(RandomizerSettingKey key_, std::vector<std::string> options_, const OptionCategory category_,
                  std::string cvarName_, WidgetType widgetType_, const uint8_t defaultOption_,
                  const bool defaultHidden_, WidgetFunc callback_, int imFlags_) {
    return { static_cast<size_t>(key_),
             std::move(options_),
             category_,
             std::move(cvarName_),
             widgetType_,
             defaultOption_,
             defaultHidden_,
             callback_,
             imFlags_ };
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

#pragma region Lang

#define RANDO_ENUM_ITEM(enum) { enum, #enum },

std::unordered_map<RandomizerSettingKey, std::string> settingNames = {
#include "randomizerEnums/RandomizerSettingKey.h"
};

std::unordered_map<RandomizerTrick, std::string> trickNames = {
#include "randomizerEnums/RandomizerTrick.h"
};

#undef RANDO_ENUM_ITEM

const static std::string namePostfix = ".name";
const static std::string descriptionPostfix = ".description";

const static std::string settingPrefix = "randomizer.settings.";
const static std::string trickPrefix = "randomizer.tricks.";

const static std::string empty = "";
const static std::string error = "[ERROR]";

static const std::string& MakeSettingName(RandomizerSettingKey key) {
    std::string settingNamePart = settingNames[key].substr(4);
    std::transform(settingNamePart.begin(), settingNamePart.end(), settingNamePart.begin(), ::tolower);
    return Lang::Translate((settingPrefix + settingNamePart + namePostfix).c_str());
}

static const std::string& MakeSettingDescription(RandomizerSettingKey key) {
    std::string settingNamePart = settingNames[key].substr(4);
    std::transform(settingNamePart.begin(), settingNamePart.end(), settingNamePart.begin(), ::tolower);
    auto result = Lang::TryTranslate((settingPrefix + settingNamePart + descriptionPostfix).c_str());
    if (std::holds_alternative<std::reference_wrapper<const std::string>>(result)) {
        return std::get<std::reference_wrapper<const std::string>>(result);
    } else if (std::holds_alternative<Lang::Error>(result)) {
        return empty;
    } else {
        assert(false);
        return error;
    }
}

static const std::string& MakeTrickName(RandomizerTrick key) {
    std::string trickNamePart = trickNames[key].substr(3);
    std::transform(trickNamePart.begin(), trickNamePart.end(), trickNamePart.begin(), ::tolower);
    return Lang::Translate((trickPrefix + trickNamePart + namePostfix).c_str());
}

static const std::string& MakeTrickDescription(RandomizerTrick key) {
    std::string trickNamePart = trickNames[key].substr(3);
    std::transform(trickNamePart.begin(), trickNamePart.end(), trickNamePart.begin(), ::tolower);
    return Lang::Translate((trickPrefix + trickNamePart + descriptionPostfix).c_str());
}

#pragma endregion

const static std::string todo = "TODO";

const std::string& Option::GetName() const {
    switch (this->GetCategory()) {
        case OptionCategory::Setting:
        case OptionCategory::Toggle:
            return MakeSettingName(static_cast<RandomizerSettingKey>(this->key));
        case OptionCategory::Trick:
            return MakeTrickName(static_cast<RandomizerTrick>(this->key));
        case OptionCategory::LocationExclusion:
            return todo;
        default:
            assert(false);
            return error;
    }
}

const std::string& Option::GetDescription() const {
    switch (this->GetCategory()) {
        case OptionCategory::Setting:
        case OptionCategory::Toggle:
            return MakeSettingDescription(static_cast<RandomizerSettingKey>(this->key));
        case OptionCategory::Trick:
            return MakeTrickDescription(static_cast<RandomizerTrick>(this->key));
        case OptionCategory::LocationExclusion:
            return todo;
        default:
            assert(false);
            return error;
    }
}

uint8_t Option::GetOptionIndex() const {
    return CVarGetInteger(cvarName.c_str(), defaultOption);
}

uint8_t Option::GetMenuOptionDefault() const {
    return defaultOption;
}

const std::string& Option::GetOptionText(size_t index) const {
    if (index >= options.size()) {
        index = options.size() - 1;
    }
    return options[index];
}

const std::string& Option::GetCVarName() const {
    return cvarName;
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

OptionCategory Option::GetCategory() const {
    return this->category;
}

void Option::AddFlag(const int imFlag_) {
    imFlags |= imFlag_;
}

void Option::RemoveFlag(const int imFlag_) {
    imFlags &= ~imFlag_;
}

uint8_t Option::GetValueFromText(const std::string text) {
    if (optionsTextToVar.contains(text)) {
        return optionsTextToVar[text];
    } else {
        SPDLOG_ERROR("Option {} does not have a var named {}.", this->GetName(), text);
        assert(false);
    }
    return defaultOption;
}

Option::Option(size_t key_, std::vector<std::string> options_, OptionCategory category_, std::string cvarName_,
               WidgetType widgetType_, uint8_t defaultOption_, bool defaultHidden_, WidgetFunc callback_, int imFlags_)
    : key(key_), options(std::move(options_)), category(category_), cvarName(std::move(cvarName_)),
      widgetType(widgetType_), defaultOption(defaultOption_), defaultHidden(defaultHidden_), imFlags(imFlags_),
      callback(callback_) {
    contextSelection = defaultOption;
    hidden = defaultHidden;
    for (size_t i = 0; i < options.size(); i++) {
        optionsMap.emplace(static_cast<int32_t>(i), options[i].c_str());
    }
    UIWidgets::LabelPositions labelPosition;
    switch (widgetType) {
        case WIDGET_CVAR_CHECKBOX:
            // labelPosition = UIWidgets::LabelPositions::Near;
            // if (imFlags_ & IMFLAG_LABEL_INLINE) {
            //     labelPosition = UIWidgets::LabelPositions::Near;
            // }
            widgetOptions = std::make_shared<UIWidgets::CheckboxOptions>(
                UIWidgets::CheckboxOptions().DefaultValue(defaultOption).Tooltip(this->GetDescription()));
            break;
        case WIDGET_CVAR_COMBOBOX:
            labelPosition = UIWidgets::LabelPositions::Above;
            if (imFlags_ & IMFLAG_LABEL_INLINE) {
                labelPosition = UIWidgets::LabelPositions::Near;
            }
            widgetOptions = std::make_shared<UIWidgets::ComboboxOptions>(UIWidgets::ComboboxOptions()
                                                                             .DefaultIndex(defaultOption)
                                                                             .ComboMap(optionsMap)
                                                                             .Tooltip(this->GetDescription())
                                                                             .LabelPosition(labelPosition));
            break;
        case WIDGET_CVAR_SLIDER_INT:
            labelPosition = UIWidgets::LabelPositions::Above;
            if (imFlags_ & IMFLAG_LABEL_INLINE) {
                labelPosition = UIWidgets::LabelPositions::Near;
            }
            widgetOptions =
                std::make_shared<UIWidgets::IntSliderOptions>(UIWidgets::IntSliderOptions()
                                                                  .DefaultValue(defaultOption)
                                                                  .Tooltip(this->GetDescription())
                                                                  .Min(0)
                                                                  .Max(static_cast<int32_t>(options.size() - 1))
                                                                  .Format(options[defaultOption].c_str())
                                                                  .LabelPosition(labelPosition));
            break;
        default:
            break;
    }
    PopulateTextToNum();
}

void Option::AddWidget(WidgetPath& path) {
    auto widget = SohGui::mSohMenu->AddWidget(path, this->GetName() + "##Randomizer", widgetType)
                      .Callback(callback)
                      .PreFunc([this](WidgetInfo& info) {
                          info.isHidden = this->IsHidden();
                          info.options->disabled = this->disabled;
                          info.options->disabledTooltip = this->disabledText.c_str();
                          info.options->tooltip = this->GetDescription();
                          if (info.type == WIDGET_CVAR_SLIDER_INT) {
                              UIWidgets::IntSliderOptions* sliderOpts =
                                  (UIWidgets::IntSliderOptions*)info.options.get();
                              size_t maxIndex = this->options.size() - 1;
                              if (this->GetKey() == RSK_SHOPSANITY_COUNT && maxIndex > 7 &&
                                  CVarGetInteger(CVAR_RANDOMIZER_SETTING("LogicRules"), RO_LOGIC_GLITCHLESS) !=
                                      RO_LOGIC_NO_LOGIC) {
                                  maxIndex = 7;
                              }
                              sliderOpts->Max(static_cast<int32_t>(maxIndex));
                              if (this->GetOptionIndex() > maxIndex) {
                                  CVarSetInteger(cvarName.c_str(), static_cast<int32_t>(maxIndex));
                              }
                              sliderOpts->Format(this->GetOptionText(this->GetOptionIndex()).c_str());
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

LocationOption::LocationOption(RandomizerCheck key_)
    : Option(key_, { "Included", "Excluded" }, OptionCategory::LocationExclusion, "", WIDGET_CVAR_CHECKBOX,
             RO_LOCATION_INCLUDE, false, nullptr, IMFLAG_NONE) {
}

RandomizerCheck LocationOption::GetKey() const {
    return static_cast<RandomizerCheck>(key);
}

TrickSetting::TrickSetting(RandomizerTrick key_, const RandomizerCheckQuest quest_, const RandomizerArea area_,
                           std::set<Tricks::Tag> tags_, const std::string nameTag_)
    : Option(key_, { "Disabled", "Enabled" }, OptionCategory::Trick, "", WIDGET_CVAR_CHECKBOX, 0, false, nullptr,
             IMFLAG_NONE),
      mQuest(quest_), mArea(area_), mNameTag(nameTag_), mTags(std::move(tags_)) {
}

TrickSetting TrickSetting::LogicTrick(RandomizerTrick key_, RandomizerCheckQuest quest_, RandomizerArea area_,
                                      std::set<Tricks::Tag> tags_, const std::string nameTag_) {
    return { key_, quest_, area_, std::move(tags_), nameTag_ };
}

RandomizerTrick TrickSetting::GetKey() const {
    return static_cast<RandomizerTrick>(key);
}

RandomizerCheckQuest TrickSetting::GetQuest() const {
    return mQuest;
}

RandomizerArea TrickSetting::GetArea() const {
    return mArea;
}

std::string TrickSetting::GetNameTag() const {
    return mNameTag;
}

bool TrickSetting::HasTag(const Tricks::Tag tag) const {
    return mTags.contains(tag);
}

const std::set<Tricks::Tag>& TrickSetting::GetTags() const {
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
        SohGui::mSohMenu->AddSidebarEntry("Randomizer", path.sidebarName, static_cast<uint32_t>(mSubGroups.size()));
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