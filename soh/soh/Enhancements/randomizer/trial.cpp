#include "trial.h"
#include "static_data.h"

namespace Rando {
TrialInfo::TrialInfo(RandomizerHintTextKey nameKey_, TrialKey trialKey_) : nameKey(std::move(nameKey_)), trialKey(std::move(trialKey_)) {}
TrialInfo::TrialInfo() = default;
TrialInfo::~TrialInfo() = default;

CustomMessage TrialInfo::GetName() const {
    return StaticData::hintTextTable[nameKey].GetHintMessage();
}

RandomizerHintTextKey TrialInfo::GetNameKey() const {
    return nameKey;
}

TrialKey TrialInfo::GetTrialKey() const {
    return trialKey;
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
    for (const auto trial : StaticData::trialData){
        mTrials[trial.first] = TrialInfo(trial.second, static_cast<TrialKey>(trial.first));
    }
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
        trialList.push_back(&mTrials[i]);
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

std::unordered_map<uint32_t, RandomizerHintTextKey> Trials::GetAllTrialHintHeys() const {
    std::unordered_map<uint32_t, RandomizerHintTextKey> output = {};
    for (auto trial: mTrials){
        output[(uint32_t)trial.GetTrialKey()] = trial.GetNameKey();
    }
    return output;
}

} // namespace Rando