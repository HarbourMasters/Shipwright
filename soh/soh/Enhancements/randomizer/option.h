#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <variant>
#include <type_traits>

namespace Rando {
enum class OptionCategory {
    Setting,
    Toggle,
};

class Option {
  public:
    Option() = default;
    static Option Bool(std::string name_, std::vector<std::string> options_ = { "Off", "On" },
                       OptionCategory category_ = OptionCategory::Setting, uint8_t defaultOption_ = 0,
                       bool defaultHidden_ = false);
    static Option U8(std::string name_, std::vector<std::string> options_,
                     OptionCategory category_ = OptionCategory::Setting, uint8_t defaultOption = 0,
                     bool defaultHidden = false);
    static Option LogicTrick(std::string name_);

    template <typename T> T Value() const {
        return std::get<T>(var);
    }

    template <typename T> bool Is(T other) const {
        static_assert(std::is_integral_v<T> || std::is_enum_v<T>, "T must be an integral type or an enum.");
        if constexpr ((std::is_integral_v<T> && !std::is_same_v<bool, T>) || std::is_enum_v<T>) {
            return Value<uint8_t>() == static_cast<uint8_t>(other);
        } else {
            return Value<bool>() == static_cast<bool>(other);
        }
    }

    template <typename T> bool IsNot(T other) const {
        return !Is(other);
    }

    explicit operator bool() const;
    size_t GetOptionCount() const;
    const std::string& GetName() const;
    const std::string& GetSelectedOptionText() const;
    uint8_t GetSelectedOptionIndex() const;
    void SetVariable();
    void SetDelayedOption();
    void RestoreDelayedOption();
    void SetSelectedIndex(size_t idx);
    void Hide();
    void Unhide();
    bool IsHidden() const;
    bool IsCategory(OptionCategory category) const;

  private:
    Option(uint8_t var_, std::string name_, std::vector<std::string> options_, OptionCategory category_,
           uint8_t defaultOption_, bool defaultHidden_);
    Option(bool var_, std::string name_, std::vector<std::string> options_, OptionCategory category_,
           uint8_t defaultOption_, bool defaultHidden_);
    std::variant<bool, uint8_t> var;
    std::string name;
    std::vector<std::string> options;
    uint8_t selectedOption = 0;
    uint8_t delayedOption = 0;
    bool hidden = false;
    OptionCategory category;
    uint8_t defaultOption = false;
    bool defaultHidden = false;
};

enum class OptionGroupType {
    DEFAULT,
    SUBGROUP,
};

class OptionGroup {
  public:
    OptionGroup() = default;
    OptionGroup(std::string name, std::vector<Option*> options, OptionGroupType groupType = OptionGroupType::DEFAULT, bool printInSpoiler = true, OptionGroupType containsType = OptionGroupType::DEFAULT);
    OptionGroup(std::string name, std::vector<OptionGroup*> subGroups, OptionGroupType groupType = OptionGroupType::DEFAULT, bool printInSpoiler = true, OptionGroupType containsType = OptionGroupType::SUBGROUP);
    static OptionGroup SubGroup(std::string name, std::vector<Option*> options, bool printInSpoiler = true);
    static OptionGroup SubGroup(std::string name, std::vector<OptionGroup*> subGroups, bool printInSpoiler = true);
    const std::string& GetName() const;
    const std::vector<Option*>& GetOptions() const;
    const std::vector<OptionGroup*>& GetSubGroups() const;
    bool PrintInSpoiler() const;
    OptionGroupType GetGroupType() const;
    OptionGroupType GetContainsType() const;

  private:
    std::string mName;
    std::vector<Option*> mOptions;
    std::vector<OptionGroup*> mSubGroups;
    OptionGroupType mGroupType = OptionGroupType::DEFAULT;
    bool mPrintInSpoiler;
    OptionGroupType mContainsType = OptionGroupType::DEFAULT;
};
} // namespace Rando