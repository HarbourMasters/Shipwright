#include "option.h"

namespace Rando {
Option Option::Bool(std::string name_, std::vector<std::string> options_, const OptionCategory category_,
                    const uint8_t defaultOption_, const bool defaultHidden_) {
    return {false, std::move(name_), std::move(options_), category_, defaultOption_, defaultHidden_};
}

Option Option::U8(std::string name_, std::vector<std::string> options_, const OptionCategory category_,
                  const uint8_t defaultOption_, const bool defaultHidden_) {
    return {static_cast<uint8_t>(0), std::move(name_), std::move(options_), category_, defaultOption_, defaultHidden_};
}

Option Option::LogicTrick(std::string name_) {
    return Option(false, std::move(name_), { "Disabled", "Enabled" }, OptionCategory::Setting, 0, false);
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

uint8_t Option::GetSelectedOptionIndex() const {
    return selectedOption;
}

const std::string& Option::GetSelectedOptionText() const {
    return options[selectedOption];
}

void Option::SetVariable() {
    if (std::holds_alternative<bool>(var)) {
        var.emplace<bool>(selectedOption != 0);
    } else {
        var.emplace<uint8_t>(selectedOption);
    }
}

void Option::SetDelayedOption() {
    delayedOption = selectedOption;
}

void Option::RestoreDelayedOption() {
    selectedOption = delayedOption;
    SetVariable();
}

void Option::SetSelectedIndex(const size_t idx) {
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

bool Option::IsCategory(const OptionCategory category) const {
    return category == this->category;
}

Option::Option(uint8_t var_, std::string name_, std::vector<std::string> options_, const OptionCategory category_,
               const uint8_t defaultOption_, const bool defaultHidden_)
    : var(var_), name(std::move(name_)), options(std::move(options_)), category(category_),
      defaultOption(defaultOption_), defaultHidden(defaultHidden_) {
    selectedOption = defaultOption;
    hidden = defaultHidden;
    SetVariable();
}
Option::Option(bool var_, std::string name_, std::vector<std::string> options_, const OptionCategory category_,
               const uint8_t defaultOption_, const bool defaultHidden_)
    : var(var_), name(std::move(name_)), options(std::move(options_)), category(category_),
      defaultOption(defaultOption_), defaultHidden(defaultHidden_) {
    selectedOption = defaultOption;
    hidden = defaultHidden;
    SetVariable();
}

OptionGroup::OptionGroup(std::string name, std::vector<Option*> options, const OptionGroupType groupType,
                         const bool printInSpoiler, const OptionGroupType containsType)
    : mName(std::move(name)), mOptions(std::move(options)), mGroupType(groupType), mPrintInSpoiler(printInSpoiler),
      mContainsType(containsType) {
}

OptionGroup::OptionGroup(std::string name, std::vector<OptionGroup*> subGroups, const OptionGroupType groupType,
                         const bool printInSpoiler, const OptionGroupType containsType)
    : mName(std::move(name)), mSubGroups(std::move(subGroups)), mGroupType(groupType), mPrintInSpoiler(printInSpoiler),
      mContainsType(containsType) {
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<Option*> options, const bool printInSpoiler) {
    return {std::move(name), std::move(options), OptionGroupType::SUBGROUP, printInSpoiler};
}

OptionGroup OptionGroup::SubGroup(std::string name, std::vector<OptionGroup*> subGroups, const bool printInSpoiler) {
    return {std::move(name), std::move(subGroups), OptionGroupType::SUBGROUP, printInSpoiler, OptionGroupType::SUBGROUP};
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