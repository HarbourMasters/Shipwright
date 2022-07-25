#pragma once

#include <array>

#include "text.hpp"

//This is probably overkill for a small amount of information, but we can add
//stuff later if we want

namespace Trial {
class TrialInfo {
public:
    TrialInfo(const Text name_);
    ~TrialInfo();

    Text GetName() const {
        return name;
    }

    bool IsSkipped() const {
        return skipped;
    }

    bool IsRequired() const {
        return !skipped;
    }

    void SetAsRequired() {
        skipped = false;
    }

    void SetAsSkipped() {
        skipped = true;
    }

private:
      Text name;
      bool skipped = true;
};

extern TrialInfo ForestTrial;
extern TrialInfo FireTrial;
extern TrialInfo WaterTrial;
extern TrialInfo SpiritTrial;
extern TrialInfo ShadowTrial;
extern TrialInfo LightTrial;

using TrialArray = std::array<TrialInfo*, 6>;

extern const TrialArray trialList;
} //namespace Trial
