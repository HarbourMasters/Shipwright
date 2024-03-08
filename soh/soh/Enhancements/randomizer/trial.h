#pragma once

#include "3drando/text.hpp"
#include "randomizerTypes.h"
#include "../custom-message/CustomMessageManager.h"
#include <array>
#include <nlohmann/json.hpp>

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
  private:
    std::vector<TrialInfo> mTrials;
};
}