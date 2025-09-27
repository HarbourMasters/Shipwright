#pragma once

#ifndef RANDOPTION_H
#define RANDOPTION_H

#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <type_traits>

#include "randomizerTypes.h"
#include "tricks.h"

namespace Rando {
enum ImGuiMenuFlags {
    IMFLAG_NONE = 0,
    IMFLAG_SEPARATOR_BOTTOM = 1 << 0, /** Adds a padded separator below the widget. */
    IMFLAG_SEPARATOR_TOP = 1 << 1,    /** Adds a padded separator above the widget. */
    IMFLAG_INDENT = 1 << 2,           /** Indents this widget and all proceeding widgets. */
    IMFLAG_UNINDENT = 1 << 3,         /** Unindents this widget and all proceeding widgets. */
};

/**
 * @brief Affects how options are handled when writing a spoiler/patch file
 */
enum class OptionCategory {
    Setting, /** An option that typically affects the logic/item pool/etc. of the seed. Typically gets written out to
                the spoiler file. */
    Toggle,  /** An option that typically affects other options rather than affecting the seed directly. i.e. A toggle
                for randomizing the values of other options. */
};

/**
 * @brief Controls how this option is rendered in the menu.
 */
enum class WidgetType {
    Checkbox, /** Default for Bools, not compatible if options.size() > 2. */
    Combobox, /** Default for U8s, works with U8s and Bools. */
    Slider, /** Compatible with U8s. If constructed with NumOpts, consider using this. Technically can be used for Bool
               or non-NumOpts options but it would be a bit weird semantically. */
};

class OptionValue {
  public:
    OptionValue() = default;
    OptionValue(uint8_t value_);

    /**
     * @brief Returns the value of the OptionValue's mVal
     *
     * @return uint8_t
     */
    uint8_t Get();

    /**
     * @brief Set the OptionValue's mVal to the provided val.
     *
     * @param val
     */
    void Set(uint8_t val);

    /**
     * @brief Determines if the value/selected index of this Option matches the provided value.
     *
     * @param other The value to compare.
     * @return true
     * @return false
     */
    bool Is(uint32_t other) const {
        return mVal == other;
    }

    /**
     * @brief Determines if the value/selected index of this Option does not match the provided value.
     *
     * @param other The value to compare.
     * @return true
     * @return false
     */
    bool IsNot(uint32_t other) const {
        return !Is(other);
    }

    /**
     * @brief Allows the option to be used as a boolean value directly.
     *
     * @return true
     * @return false
     */
    explicit operator bool() const;

  private:
    uint8_t mVal;
};

/**
 * @brief A class describing the state of a single option/setting, such as its name,
 * options, current value, whether or not it is interactable in the menu, or the CVar,
 * it is linked to.
 */
class Option {
  public:
    Option() = default;

    /**
     * @brief Constructs a boolean option. This overload of this function typically requires more
     * options to be specified rather than left as default.
     *
     * @param name_ The name of the option. Appears in the spoiler/patch file.
     * @param options_ A vector of value names for this Option. This vector should have a size of 2.
     * The name corresponding to the selected index for this option will be printed to the spoiler/patch file.
     * @param category_ The desired `OptionCategory` for this option.
     * @param cvarName_ The name of the CVar this option should correspond with. Set as an empty string to not
     * link to any Cvar.
     * @param description_ A description of what this option affects. Will be rendered in a tooltip in ImGui.
     * Can be left as an empty string if desired, no tooltip will be rendered.
     * @param widgetType_ What type of widget should be rendered. Should probably be `Checkbox` but technically
     * `Combobox` or `Slider` would render and function correctly.
     * @param defaultOption_ The default index that should be selected.
     * @param defaultHidden_ Whether or not to display the option (can be changed at runtime later).
     * @param imFlags_ (see ImGuiMenuFlags type) flags that can modify how this option is rendered.
     * @return Option
     */
    static Option Bool(RandomizerSettingKey key_, std::string name_,
                       std::vector<std::string> options_ = { "Off", "On" },
                       OptionCategory category_ = OptionCategory::Setting, std::string cvarName_ = "",
                       std::string description_ = "", WidgetType widgetType_ = WidgetType::Checkbox,
                       uint8_t defaultOption_ = 0, bool defaultHidden_ = false, int imFlags_ = IMFLAG_SEPARATOR_BOTTOM);

    /**
     * @brief Constructs a boolean option. This constructor was added later for convenience so that a cvarName
     * could be specified without needing to fill in options that were previously left at default in
     * existing calls to the other overload of this function. The options vector will be { "Off", "On" }
     * when using this overload. If you want your option to have different value names, use the other overload.
     *
     * @param key_ The RandomizerSettingKey of this option.
     * @param name_ The name of the option. Appears in the spoiler/patch file.
     * @param cvarName_ The name of the CVar this option should correspond with. Set as an empty string to not
     * link to any CVar.
     * @param description_ A description of what this option affects. Will be rendered in a tooltip in ImGui.
     * Can be left as an empty string if desired, no tooltip will be rendered.
     * @param imFlags_ (see ImGuiMenuFlags type) flags that can modify how this option is rendered.
     * @param widgetType_ What type of widget should be rendered. Should probably be `Checkbox` but technically
     * `Combobox` or `Slider` would render and function correctly.
     * @param defaultOption_ The defaulted selected index for this Option.
     * @return Option
     */
    static Option Bool(RandomizerSettingKey key_, std::string name_, std::string cvarName_,
                       std::string description_ = "", int imFlags_ = IMFLAG_SEPARATOR_BOTTOM,
                       WidgetType widgetType_ = WidgetType::Checkbox, bool defaultOption_ = false);

    /**
     * @brief Constructs a U8 Option.
     *
     * @param key_ The RandomizerSettingKey for this option.
     * @param name_ The name of this Option. Appears in the spoiler/patch file.
     * @param options_ A vector of value names for this Option. The name corresponding to the selected
     * index for this option will be printed to the spoiler/patch file.
     * @param category_ The desired `OptionCategory` for this option.
     * @param cvarName_ The name ofthe CVar this option should correspond with. Set as an empty string to not
     * link to any Cvar.
     * @param description_ A description of what this option affects. Will be rendered in a toolip in ImGui.
     * Can be left as an empty string if desired, no tooltip will be rendered.
     * @param widgetType_ What type of widget should be rendered. Defaults to `Combobox`, but if you use NumOpts
     * to make the `options_` vector you should probably set this to `Slider`. `Slider` will technically work for
     * any value of `options_` but may be odd/unclear semantically speaking.
     * This should not be set for `Checkbox` if options_ has more than 2 values.
     * @param defaultOption_ The default index that should be selected.
     * @param defaultHidden_ Whether or not to display the option (can be changed at runtime later).
     * @param imFlags_ (see ImGuiMenuFlags type) flags that can modify how this option is rendered.
     * @return Option
     */
    static Option U8(RandomizerSettingKey key_, std::string name_, std::vector<std::string> options_,
                     OptionCategory category_ = OptionCategory::Setting, std::string cvarName_ = "",
                     std::string description_ = "", WidgetType widgetType_ = WidgetType::Combobox,
                     uint8_t defaultOption_ = 0, bool defaultHidden_ = false, int imFlags_ = IMFLAG_SEPARATOR_BOTTOM);

    /**
     * @brief A convenience function for constructing the Option for a trick.
     *
     * @param name_ The name of the trick. Appears in the spoiler/patch file.
     * @return Option
     */
    static Option LogicTrick(RandomizerTrick rt_, std::string name_);

    /**
     * @brief Get the size of the options array.
     *
     * @return size_t
     */
    size_t GetOptionCount() const;

    /**
     * @brief Get the Key Option
     *
     * @return const RandomizerSettingKey
     */
    RandomizerSettingKey GetKey() const;

    /**
     * @brief Get the name of the Option.
     *
     * @return const std::string&
     */
    const std::string& GetName() const;
    const std::string& GetDescription() const;

    /**
     * @brief Get the text of the Option value of the selected index.
     *
     * @param index
     * @return const std::string&
     */
    const std::string& GetOptionText(size_t index) const;

    /**
     * @brief Get the CVar name for this Option.
     *
     * @return const std::string&
     */
    const std::string& GetCVarName() const;

    /**
     * @brief Get the menu index for this Option.
     *
     * @return uint8_t
     */
    uint8_t GetOptionIndex() const;

    /**
     * @brief Set the delayedOption to the currently selected index so it can be restored later.
     */
    void SetDelayedOption();

    /**
     * @brief Restores the delayedOption back to the selected index.
     */
    void RestoreDelayedOption();

    /**
     * @brief Set the rando context index for this Option.
     *
     * @param idx the index to set as the selected index.
     */
    void SetContextIndex(uint8_t idx);

    /**
     * @brief Hides this Option in the menu. (Not currently being used afaik, we prefer to
     * display all the Options and visually disable the ones that aren't applicable.)
     */
    void Hide();

    /**
     * @brief Shows this Option in the menu if it was previously hidden.
     */
    void Unhide();

    /**
     * @brief Whether or not this Option is currently hidden.
     *
     * @return true
     * @return false
     */

    bool IsHidden() const;
    /**
     * @brief Replaces the `options` vector for this Option with a new one.
     * If the new vector is smaller than the old one and the current selected
     * index is out of range, this function changes the selected index to the
     * last index of the new vector.
     *
     * @param opts The new vector of options.
     */
    void ChangeOptions(std::vector<std::string> opts);

    /**
     * @brief Enables interaction with this option.
     *
     * "Enable" in this context refers to the ability to change the option in the
     * settings menu. The actual value of the option is not decided by whether or not
     * the option is "Enabled".
     */
    void Enable();

    /**
     * @brief Disables interaction with this option.
     *
     * "Disable" in this context refers to the ability to change the option in the
     * settings menu. The actual value of the option is not decided by whether or not
     * the option is "Disabled".
     *
     * @param text The tooltip text explaining why the option is disabled.
     * @param graphic What graphic to display in a disabled checkbox. Defaults to an
     * "X" symbol.
     */
    void Disable(std::string text);
    bool IsCategory(OptionCategory category) const;

    /**
     * @brief Automatically renders a widget for this option in ImGui, based on the various
     * properties of this Option. Typically, Bool options are rendered as Checkboxes and
     * U8 options are rendered as Comboboxes, but this can be overridden during construction with
     * the `widgetType` property.
     */
    bool RenderImGui();

    bool HasFlag(int imFlag_) const;
    void AddFlag(int imFlag_);
    void SetFlag(int imFlag_);
    void RemoveFlag(int imFlag_);

    uint8_t GetValueFromText(std::string text);
    void SetContextIndexFromText(std::string text);

  protected:
    Option(size_t key_, std::string name_, std::vector<std::string> options_, OptionCategory category_,
           std::string cvarName_, std::string description_, WidgetType widgetType_, uint8_t defaultOption_,
           bool defaultHidden_, int imFlags_);
    size_t key;

  private:
    bool RenderCheckbox();
    bool RenderCombobox();
    bool RenderSlider();
    void PopulateTextToNum();
    std::string name;
    std::vector<std::string> options;
    uint8_t contextSelection = 0;
    uint8_t delayedSelection = 0;
    bool hidden = false;
    OptionCategory category = OptionCategory::Setting;
    std::string cvarName;
    std::string description;
    WidgetType widgetType = WidgetType::Checkbox;
    uint8_t defaultOption = false;
    bool defaultHidden = false;
    int imFlags = IMFLAG_NONE;
    bool disabled = false;
    std::string disabledText;
    std::unordered_map<std::string, uint8_t> optionsTextToVar = {};
};

class LocationOption : public Option {
  public:
    LocationOption() = default;
    LocationOption(RandomizerCheck key_, const std::string& name_);
    RandomizerCheck GetKey() const;
};

class TrickOption : public Option {
  public:
    TrickOption() = default;
    /**
     * @brief A convenience function for constructing the Option for a trick.
     *
     * @param key_ A RandomizerTrick key for this option.
     * @param quest_ MQ, Vanilla, or Both.
     * @param area_ The area the trick is relevant for.
     * @param tags_ The set of RandomizerTrickTags for this trick.
     * @param name_ The name of the trick. Appears in the spoiler/patch file.
     * @param description_ A brief description of the trick.
     * @return Option
     */
    static TrickOption LogicTrick(RandomizerTrick key_, RandomizerCheckQuest quest_, RandomizerArea area_,
                                  std::set<Tricks::Tag> tags_, const std::string& name_, std::string description_);

    RandomizerTrick GetKey() const;

    /**
     * @brief Retrieve the quest type this trick is relevant for.
     *
     * @return RandomizerCheckQuest
     */
    RandomizerCheckQuest GetQuest() const;

    /**
     * @brief Get the Area this trick is used in
     *
     * @return RandomizerArea
     */
    RandomizerArea GetArea() const;

    /**
     * @brief Check if this Trick has the given tag
     *
     * @param tag the RandomizerTrickTag to check for
     * @return true or false
     */
    bool HasTag(Tricks::Tag tag) const;

    const std::set<Tricks::Tag>& GetTags() const;

  private:
    TrickOption(RandomizerTrick key_, RandomizerCheckQuest quest_, RandomizerArea area_, std::set<Tricks::Tag> tags_,
                const std::string& name_, std::string description_);
    RandomizerCheckQuest mQuest;
    RandomizerArea mArea;
    std::set<Tricks::Tag> mTags;
};

enum class OptionGroupType {
    DEFAULT,
    SUBGROUP,
};

enum class WidgetContainerType {
    BASIC,   /** Barebones container, just lists the options within. */
    SECTION, /** Similar to Barebones, but has a header with the section name. */
    COLUMN,  /** Signifies the container should be the start of new column within a table. */
    TABLE,   /** Signifies the container is a table (should contain other subgroups with type column)*/
    TABBED,  /** Signifies this container's contents should be contained within a tabbed interface. */
};

class OptionGroup {
  public:
    OptionGroup() = default;

    /**
     * @brief Construct a new Option Group containing a list of `Option` pointers.
     *
     * @param name The name of this Option Group. Appears in the spoiler/patch file in front each option it contains.
     * @param options A vector of Option pointers
     * @param groupType `DEFAULT` if this group is not contained within any other groups, `SUBGROUP` if it is a
     * subgroup of another group.
     * @param containerType Specifies the type of container this widget should render as in ImGui.
     * @param description A description that can appear in a tooltip in ImGui.
     */
    OptionGroup(std::string name, std::vector<Option*> options, OptionGroupType groupType = OptionGroupType::DEFAULT,
                WidgetContainerType containerType = WidgetContainerType::BASIC, std::string description = "");

    /**
     * @brief Construct a new Option Group containing a list of `OptionGroup` pointers.
     *
     * @param name The name of this option group. Appears in the spoiler/patch file.
     * @param subGroups A vector of OptionGroup pointers that will be subgroups of this group.
     * @param groupType `DEFAULT` if this group is not contained within any other groups, `SUBGROUP` if it is a
     * subgroup of another group.
     * @param containerType Specifies the type of container this widget should render as in ImGui.
     * @param description A description that can appear in a tooltip in ImGui.
     */
    OptionGroup(std::string name, std::vector<OptionGroup*> subGroups,
                OptionGroupType groupType = OptionGroupType::DEFAULT,
                WidgetContainerType containerType = WidgetContainerType::BASIC, std::string description = "");

    /**
     * @brief Convenience function for constructing an OptionGroup of groupType `SUBGROUP` with
     * containsType of `DEFAULT` (contains `Option`s).
     *
     * @param name The name of this option group. Appears in the spoiler/patch file.
     * @param options A vector of Option pointers.
     * @param containerType Specifies the type of container this widget should render as in ImGui.
     * @param description A description that can appear in a tooltip in ImGui.
     * @return OptionGroup
     */
    static OptionGroup SubGroup(std::string name, std::vector<Option*> options,
                                WidgetContainerType containerType = WidgetContainerType::BASIC,
                                std::string description = "");

    /**
     * @brief Convenience function for constructing an OptionGroup of groupType `SUBGROUP` with
     * containsType of `SUBGROUP` (contains other `OptionGroup`s)
     *
     * @param name The name of this option group. Appears in the spoiler/patch file.
     * @param subGroups A vector of OptionGroup pointers.
     * @param containerType Specifies the type of container this widget should render as in ImGui.
     * @param description A description that can appear in a tooltip in ImGui.
     * @return OptionGroup
     */
    static OptionGroup SubGroup(std::string name, std::vector<OptionGroup*> subGroups,
                                WidgetContainerType containerType = WidgetContainerType::BASIC,
                                std::string description = "");

    /**
     * @brief Get the name of the OptionGroup.
     *
     * @return const std::string&
     */
    const std::string& GetName() const;

    /**
     * @brief Get the list of `Option`s contained within this `OptionGroup`.
     *
     * @return const std::vector<Option*>&
     */
    const std::vector<Option*>& GetOptions() const;

    /**
     * @brief Get the list of `OptionGroup`s contained within this `OptionGroup`.
     *
     * @return const std::vector<OptionGroup*>&
     */
    const std::vector<OptionGroup*>& GetSubGroups() const;

    /**
     * @brief Get the Group Type of this `OptionGroup`. `DEFAULT` means this group is not contained
     * within any other groups, while `SUBGROUP` means that it is contained within at least one other.
     * `OptionGroup`.
     *
     * @return OptionGroupType
     */
    OptionGroupType GetGroupType() const;

    /**
     * @brief Get the type of values contained in this `OptionGroup`. `DEFAULT` means this group contains
     * `Options`, and `SUBGROUP` means this group contains other `OptionGroup`s.
     *
     * @return OptionGroupType
     */
    OptionGroupType GetContainsType() const;

    WidgetContainerType GetContainerType() const;

    const std::string& GetDescription() const;

    /**
     * @brief Renders all of the options contained within this `OptionGroup` in the ImGui menu.
     */
    bool RenderImGui() const;
    void Disable();
    void Enable();

  private:
    std::string mName;
    std::vector<Option*> mOptions;
    std::vector<OptionGroup*> mSubGroups;
    OptionGroupType mGroupType = OptionGroupType::DEFAULT;
    OptionGroupType mContainsType = OptionGroupType::DEFAULT;
    WidgetContainerType mContainerType = WidgetContainerType::BASIC;
    std::string mDescription;
    bool mDisabled = false;
};
} // namespace Rando

#endif // RANDOPTION_H