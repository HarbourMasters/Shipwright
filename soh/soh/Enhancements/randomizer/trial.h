#pragma once

#include "randomizerTypes.h"
#include "../custom-message/CustomMessageManager.h"
#include <nlohmann/json.hpp>
#include "3drando/hint_list.hpp"
#include "3drando/hints.hpp"
#include <unordered_map>

namespace Rando {
class TrialInfo {
  public:
    explicit TrialInfo(CustomMessage name_, TrialKey key_);
    TrialInfo();
    ~TrialInfo();

    CustomMessage GetName() const;
    TrialKey GetKey() const;
    bool IsSkipped() const;
    bool IsRequired() const;
    void SetAsRequired();
    void SetAsSkipped();
  private:
    CustomMessage name;
    TrialKey key;
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
    std::unordered_map<TrialKey, HintText> GetAllTrialHintHeys() const;
  private:
    std::vector<TrialInfo> mTrials;
};
}