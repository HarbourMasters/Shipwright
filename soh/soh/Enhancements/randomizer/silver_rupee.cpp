#include "silver_rupee.h"

namespace Rando {
    SilverRupees::SilverRupees() {
        mSilverRupees = {
                // Vanilla
                SilverRupeesInfo(5, RG_ICE_CAVERN_SPINNING_BLADES_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_ICE_CAVERN_SLIDING_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_BOTTOM_OF_THE_WELL_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_GERUDO_TRAINING_GROUNDS_BOULDER_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_GERUDO_TRAINING_GROUNDS_LAVA_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_GERUDO_TRAINING_GROUNDS_TOILET_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SPIRIT_CHILD_BLOCK_PUSH_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SPIRIT_BOULDER_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_SCYTHE_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_INVISIBLE_SPIKES_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_FOREST_TRIAL_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_FIRE_TRIAL_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SPIRIT_TRIAL_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_LIGHT_TRIAL_SILVER_RUPEE),
                // MQ
                SilverRupeesInfo(5, RG_GERUDO_TRAINING_GROUNDS_MQ_BOULDER_SILVER_RUPEE),
                SilverRupeesInfo(6, RG_GERUDO_TRAINING_GROUNDS_MQ_LAVA_SILVER_RUPEE),
                SilverRupeesInfo(3, RG_GERUDO_TRAINING_GROUNDS_MQ_TOILET_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_DODONGOS_CAVERN_MQ_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SPIRIT_MQ_LOBBY_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_MQ_SCYTHE_SILVER_RUPEE),
                SilverRupeesInfo(10, RG_SHADOW_MQ_INVISIBLE_SCYTHE_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_MQ_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE),
                SilverRupeesInfo(10, RG_SHADOW_MQ_INVISIBLE_SPIKES_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_FIRE_TRIAL_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_WATER_TRIAL_MQ_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_TRIAL_MQ_SILVER_RUPEE),
        };
    }

    SilverRupeesInfo &SilverRupees::GetInfo(RandomizerGet rgid) {
        return mSilverRupees[rgid - RG_SILVER_RUPEE_FIRST];
    }

    SilverRupeesInfo::SilverRupeesInfo(uint8_t total, RandomizerGet randoGet) : mCollected(0), mTotal(total), mRandoGet(randoGet) {

    }

    uint8_t SilverRupeesInfo::GetCollected() const {
        return mCollected;
    }

    uint8_t SilverRupeesInfo::GetTotal() const {
        return mTotal;
    }

    SilverRupeesInfo::SilverRupeesInfo() : mCollected(0), mTotal(0), mRandoGet(RG_NONE) {

    }

    void SilverRupeesInfo::IncrementCollected(uint8_t amount) {
        mCollected += amount;
    }
} // Rando