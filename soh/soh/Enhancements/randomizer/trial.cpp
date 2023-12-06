#include "trial.h"

namespace Rando {
TrialInfo::TrialInfo(Text name_) : name(std::move(name_)) {}
TrialInfo::TrialInfo() = default;
TrialInfo::~TrialInfo() = default;

Text TrialInfo::GetName() const {
    return name;
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
    mTrials[FOREST_TRIAL] = TrialInfo(Text{"the Forest Trial", "l'épreuve de la Forêt", "la prueba del bosque"});
    mTrials[FIRE_TRIAL] = TrialInfo(Text{"the Fire Trial",   "l'épreuve du Feu", "la prueba del fuego"});
    mTrials[WATER_TRIAL] = TrialInfo(Text{"the Water Trial",  "l'épreuve de l'Eau", "la prueba del agua"});
    mTrials[SPIRIT_TRIAL] = TrialInfo(Text{"the Spirit Trial", "l'épreuve de l'Esprit", "la prueba del espíritu"});
    mTrials[SHADOW_TRIAL] = TrialInfo(Text{"the Shadow Trial", "l'épreuve de l'Ombre", "la prueba de las sombras"});
    mTrials[LIGHT_TRIAL] = TrialInfo(Text{"the Light Trial",  "l'épreuve de la Lumière", "la prueba de la luz"});
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

std::array<TrialInfo*, 6> Trials::GetTrialList() {
    std::array<TrialInfo*, 6> trialList{};
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