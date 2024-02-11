#ifndef SHIP_SILVER_RUPEE_H
#define SHIP_SILVER_RUPEE_H

#include <cstdint>
#include <array>
#include "randomizerTypes.h"

namespace Rando {

    class SilverRupeesInfo {
    public:
        SilverRupeesInfo();
        SilverRupeesInfo(uint8_t total, RandomizerGet randoGet);

        uint8_t GetCollected() const;
        uint8_t GetTotal() const;
        void IncrementCollected(uint8_t amount = 1);
    private:
        uint8_t mCollected = 0;
        uint8_t mTotal;
        RandomizerGet mRandoGet;

    };

    class SilverRupees {
    public:
        SilverRupees();

        SilverRupeesInfo& GetInfo(RandomizerGet rgid);
    private:
        std::array<SilverRupeesInfo, RG_SILVER_RUPEE_LAST - RG_SILVER_RUPEE_FIRST + 1> mSilverRupees;
    };

} // Rando

#endif //SHIP_SILVER_RUPEE_H
