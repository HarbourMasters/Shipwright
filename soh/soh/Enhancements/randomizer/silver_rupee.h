#ifndef SHIP_SILVER_RUPEE_H
#define SHIP_SILVER_RUPEE_H

#include <cstdint>
#include <array>
#include "randomizerTypes.h"
#include "soh/Enhancements/randomizer/3drando/text.hpp"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"

namespace Rando {

    class SilverRupeesInfo {
    public:
        SilverRupeesInfo();
        SilverRupeesInfo(uint8_t total, RandomizerGet randoGet);

        uint8_t GetCollected() const;
        uint8_t GetTotal() const;
        void IncrementCollected(uint8_t amount = 1);
        void Reset();
    private:
        uint8_t mCollected = 0;
        uint8_t mTotal;
        RandomizerGet mRandoGet;

    };

    class SilverRupees {
    public:
        SilverRupees();

        SilverRupeesInfo& GetInfo(RandomizerGet rgid);
        size_t GetInfoListSize();
        void ResetAll();
        static CustomMessage GetSilverRupeeMessage(uint16_t rgid);
    private:
        std::array<SilverRupeesInfo, RG_SILVER_RUPEE_LAST - RG_SILVER_RUPEE_FIRST + 1> mSilverRupees;
        static std::unordered_map<RandomizerGet, Text> mSilverRupeeRoomNames;
    };

} // Rando

#endif //SHIP_SILVER_RUPEE_H
