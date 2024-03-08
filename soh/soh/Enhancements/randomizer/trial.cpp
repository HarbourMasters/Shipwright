#include "trial.h"
#include "3drando/hint_list.hpp"

namespace Rando {
TrialInfo::TrialInfo(CustomMessage name_, TrialKey key_) : name(std::move(name_)), key(std::move(key_)) {}
TrialInfo::TrialInfo() = default;
TrialInfo::~TrialInfo() = default;

CustomMessage TrialInfo::GetName() const {
    return name;
}

TrialKey TrialInfo::GetKey() const {
    return key;
}

bool TrialInfo::IsSkipped() const {
    return skipped;
}

bool TrialInfo::IsRequired() const {
    return !skipped;
}

void TrialInfo::SetAsRequired() {
    skipped = false;
}

void TrialInfo::SetAsSkipped() {
    skipped = true;
}

Trials::Trials() {
    mTrials[TK_LIGHT_TRIAL] = TrialInfo(GetHintText(RHT_LIGHT_TRIAL).GetText(), TK_LIGHT_TRIAL);
    mTrials[TK_FOREST_TRIAL] = TrialInfo(GetHintText(RHT_FOREST_TRIAL).GetText(), TK_FOREST_TRIAL);
    mTrials[TK_FIRE_TRIAL] = TrialInfo(GetHintText(RHT_FIRE_TRIAL).GetText(), TK_FIRE_TRIAL);
    mTrials[TK_WATER_TRIAL] = TrialInfo(GetHintText(RHT_WATER_TRIAL).GetText(), TK_WATER_TRIAL);
    mTrials[TK_SPIRIT_TRIAL] = TrialInfo(GetHintText(RHT_SPIRIT_TRIAL).GetText(), TK_SPIRIT_TRIAL);
    mTrials[TK_SHADOW_TRIAL] = TrialInfo(GetHintText(RHT_SHADOW_TRIAL).GetText(), TK_SHADOW_TRIAL);
}
Trials::~Trials() = default;

TrialInfo* Trials::GetTrial(const TrialKey key) {
    return &mTrials[key];
}

void Trials::SkipAll() {
    for (TrialInfo& trial : mTrials) {
        trial.SetAsSkipped();
    }
}

void Trials::RequireAll() {
    for (TrialInfo& trial : mTrials) {
        trial.SetAsRequired();
    }
}

std::vector<TrialInfo*> Trials::GetTrialList() {
    std::vector<TrialInfo*> trialList{};
    for (size_t i = 0; i < mTrials.size(); i++) {
        trialList[i] = &mTrials[i];
    }
    return trialList;
}

size_t Trials::GetTrialListSize() const {
    return mTrials.size();
}
void Trials::ParseJson(nlohmann::json spoilerFileJson) {
    nlohmann::json trialsJson = spoilerFileJson["requiredTrials"];
    for (auto it = trialsJson.begin(); it != trialsJson.end(); ++it) {
        std::string trialName = it.value().get<std::string>();
        for (auto& trial : mTrials) {
            if (trial.GetName() == trialName) {
                trial.SetAsRequired();
            } else {
                trial.SetAsSkipped();
            }
        }
    }
}
} // namespace Rando