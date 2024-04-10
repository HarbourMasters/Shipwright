#pragma once

#include <array>
#include <string>
#include <vector>

#include "text.hpp"
#include "random.hpp"
#include <functional>
#include "../randomizerTypes.h"
#include "../context.h"
#include "../../custom-message/CustomMessageManager.h"
#include "../trial.h"

struct HintDistributionSetting {
  std::string name;
  HintType type;
  uint32_t weight;
  uint8_t fixed;
  uint8_t copies;
  std::function<bool(RandomizerCheck)> filter;
  uint8_t dungeonLimit;

  HintDistributionSetting(std::string _name, HintType _type, uint32_t _weight, uint8_t _fixed, uint8_t _copies, 
                          std::function<bool(RandomizerCheck)> _filter, uint8_t _dungeonLimit = 40){}
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
    const CustomMessage& GetMessage() const;
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

  StaticHintInfo(HintType _type, std::vector<RandomizerHintTextKey> _textKeys, RandomizerSettingKey _setting, std::variant<bool, uint8_t> _condition,
                 std::vector<RandomizerCheck> _targetChecks, std::vector<RandomizerGet> _targetItems = {},
                 std::vector<RandomizerCheck> _hintChecks = {}, bool _yourPocket = false, int _num = 0);
};

using ConditionalAlwaysHint = std::pair<RandomizerCheck, std::function<bool()>>;

extern std::array<ConditionalAlwaysHint, 12> conditionalAlwaysHints;

extern void CreateAllHints();
extern void CreateWarpSongTexts();

std::unordered_map<u32, RandomizerHint> stoneFlagToHint{
    {0x0, RH_NONE},
    {0x1, RH_ZF_FAIRY_GOSSIP_STONE},
    {0x2, RH_ZF_JABU_GOSSIP_STONE},
    {0x3, RH_LH_LAB_GOSSIP_STONE},
    {0x4, RH_DMT_GOSSIP_STONE},
    {0x5, RH_DMC_GOSSIP_STONE},
    {0x6, RH_TOT_LEFTMOST_GOSSIP_STONE},
    {0x7, RH_TOT_RIGHTMOST_GOSSIP_STONE},
    {0x8, RH_LH_SOUTHWEST_GOSSIP_STONE},
    {0x9, RH_ZD_GOSSIP_STONE},
    {0xA, RH_GRAVEYARD_GOSSIP_STONE},
    {0xB, RH_HC_ROCK_WALL_GOSSIP_STONE},
    {0xC, RH_ZR_NEAR_DOMAIN_GOSSIP_STONE},
    {0xD, RH_ZR_NEAR_GROTTOS_GOSSIP_STONE},
    {0xE, RH_TOT_LEFT_CENTER_GOSSIP_STONE},
    {0xF, RH_LH_SOUTHEAST_GOSSIP_STONE},
    {0x10, RH_TOT_RIGHT_CENTER_GOSSIP_STONE},
    {0x11, RH_GV_GOSSIP_STONE},
    {0x12, RH_HC_MALON_GOSSIP_STONE},
    {0x13, RH_HC_STORMS_GROTTO_GOSSIP_STONE},
    {0x15, RH_GC_MAZE_GOSSIP_STONE},
    {0x16, RH_SFM_MAZE_NEAR_LW_GOSSIP_STONE},
    {0x17, RH_SFM_MAZE_CENTER_GOSSIP_STONE},
    {0x18, RH_GC_MEDIGORON_GOSSIP_STONE},
    {0x19, RH_COLOSSUS_GOSSIP_STONE},
    {0x1A, RH_HF_COW_GROTTO_GOSSIP_STONE},
    {0x1B, RH_SFM_SARIA_GOSSIP_STONE},
    {0x1C, RH_LW_GOSSIP_STONE},
    {0x1D, RH_KF_GOSSIP_STONE},
    {0x1E, RH_KF_DEKU_TREE_LEFT_GOSSIP_STONE},
    {0x1F, RH_KF_DEKU_TREE_RIGHT_GOSSIP_STONE},
    {0x30, RH_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE},
    {0x32, RH_HF_SOUTHEAST_GROTTO_GOSSIP_STONE},
    {0x33, RH_HF_OPEN_GROTTO_GOSSIP_STONE},
    {0x34, RH_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE},
    {0x37, RH_DMT_STORMS_GROTTO_GOSSIP_STONE},
    {0x38, RH_KAK_OPEN_GROTTO_GOSSIP_STONE},
    {0x39, RH_ZR_OPEN_GROTTO_GOSSIP_STONE},
    {0x3A, RH_DMC_UPPER_GROTTO_GOSSIP_STONE},
    {0x3C, RH_KF_STORMS_GROTTO_GOSSIP_STONE}
};

