#pragma once

#include "randomizerTypes.h"
#include "../custom-message/CustomMessageManager.h"
#include <nlohmann/json.hpp>
#include "static_data.h"
#include <unordered_map>

namespace Rando {
class TrialInfo {
  public:
    explicit TrialInfo(RandomizerHintTextKey nameKey_, TrialKey key_);
    TrialInfo();
    ~TrialInfo();

    CustomMessage GetName() const;
    RandomizerHintTextKey GetNameKey() const;
    TrialKey GetTrialKey() const;
    bool IsSkipped() const;
    bool IsRequired() const;
    void SetAsRequired();
    void SetAsSkipped();
  private:
    RandomizerHintTextKey nameKey;
    TrialKey trialKey;
    bool skipped = true;
};

class Trials {
  public:
    Trials();
    ~Trials();
    TrialInfo* GetTrial(TrialKey key);
    void SkipAll();
    void RequireAll();
    std::vector<TrialInfo*> GetTrialList();
    size_t GetTrialListSize() const;
    void ParseJson(nlohmann::json spoilerFileJson);
    std::unordered_map<uint32_t, RandomizerHintTextKey> GetAllTrialHintHeys() const;
  private:
    std::array<TrialInfo, TK_MAX> mTrials;
};
}