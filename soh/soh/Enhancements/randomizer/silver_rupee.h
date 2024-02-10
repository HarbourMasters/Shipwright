#ifndef SHIP_SILVER_RUPEE_H
#define SHIP_SILVER_RUPEE_H

#include <cstdint>
#include <array>
#include "randomizerTypes.h"

namespace Rando {

    class SilverRupeesInfo {
    public:
        SilverRupeesInfo(uint8_t total, RandomizerGet randoGet);

        const uint8_t GetCollected() const;
        const uint8_t GetTotal() const;
        void IncrementCollected();
    private:
        uint8_t mCollected;
        uint8_t mTotal;
        RandomizerGet mRandoGet;

    };

    class SilverRupees {
    public:
        SilverRupees();
    private:
        std::array<SilverRupeesInfo, 10> mSilverRupees;
    };

} // Rando

#endif //SHIP_SILVER_RUPEE_H
