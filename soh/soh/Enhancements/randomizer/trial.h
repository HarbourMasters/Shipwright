#pragma once

#include "3drando/text.hpp"

#include <array>
#include <nlohmann/json.hpp>

namespace Rando {
class TrialInfo {
  public:
    explicit TrialInfo(Text name_);
    TrialInfo();
    ~TrialInfo();

    Text GetName() const;
    bool IsSkipped() const;
    bool IsRequired() const;
    void SetAsRequired();
    void SetAsSkipped();
  private:
    Text name;
    bool skipped = true;
};

typedef enum {
    LIGHT_TRIAL,
    FOREST_TRIAL,
    FIRE_TRIAL,
    WATER_TRIAL,
    SPIRIT_TRIAL,
    SHADOW_TRIAL,
} TrialKey;

class Trials {
  public:
    Trials();
    ~Trials();
    TrialInfo* GetTrial(TrialKey key);
    void SkipAll();
    void RequireAll();
    std::array<TrialInfo*, 6> GetTrialList();
    size_t GetTrialListSize() const;
    void ParseJson(nlohmann::json spoilerFileJson);
  private:
    std::array<TrialInfo, 6> mTrials;
};
}