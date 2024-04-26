#pragma once

#include <string>
#include <vector>
#include <variant>

#include "text.hpp"
#include "random.hpp"
#include <functional>
#include "../randomizerTypes.h"
#include "../../custom-message/CustomMessageManager.h"

struct HintDistributionSetting {
  std::string name;
  HintType type;
  uint32_t weight;
  uint8_t fixed;
  uint8_t copies;
  std::function<bool(RandomizerCheck)> filter;
  uint8_t dungeonLimit;

  HintDistributionSetting(std::string _name, HintType _type, uint32_t _weight, uint8_t _fixed, uint8_t _copies, 
                          std::function<bool(RandomizerCheck)> _filter, uint8_t _dungeonLimit = 40);
};

struct HintSetting {
  uint8_t alwaysCopies;
  uint8_t trialCopies;
  uint8_t junkWeight;
  std::vector<HintDistributionSetting> distTable;
};

class HintText {
public:
    HintText() = default;
    HintText(CustomMessage clearText_, std::vector<CustomMessage> ambiguousText_ = {}, std::vector<CustomMessage> obscureText_ = {});
    const CustomMessage& GetClear() const;
    const CustomMessage& GetObscure() const;
    const CustomMessage& GetObscure(uint8_t selection) const;
    const CustomMessage& GetAmbiguous() const;
    const CustomMessage& GetAmbiguous(uint8_t selection) const;
    uint8_t GetAmbiguousSize() const;
    uint8_t GetObscureSize() const;
    const CustomMessage& GetHintMessage(uint8_t selection = 0) const;
    const CustomMessage GetMessageCopy() const;
    bool operator==(const HintText& right) const;
    bool operator!=(const HintText& right) const;

private:
    CustomMessage clearText;
    std::vector<CustomMessage> ambiguousText = {};
    std::vector<CustomMessage> obscureText = {};
};

struct StaticHintInfo{
  HintType type;
  std::vector<RandomizerHintTextKey> hintKeys;
  RandomizerSettingKey setting;
  std::variant<bool, uint8_t> condition;
  std::vector<RandomizerCheck> targetChecks;
  std::vector<RandomizerGet> targetItems;
  std::vector<RandomizerCheck> hintChecks;
  bool yourPocket;
  int num;

  StaticHintInfo() = default;
  StaticHintInfo(HintType _type, std::vector<RandomizerHintTextKey> _hintKeys, RandomizerSettingKey _setting, std::variant<bool, uint8_t> _condition,
                 std::vector<RandomizerCheck> _targetChecks, std::vector<RandomizerGet> _targetItems = {},
                 std::vector<RandomizerCheck> _hintChecks = {}, bool _yourPocket = false, int _num = 0);
};

extern void CreateAllHints();
extern void CreateWarpSongTexts();
void CreateStaticHints();