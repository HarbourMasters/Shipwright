#ifndef SHUFFLESILVERRUPEES_H
#define SHUFFLESILVERRUPEES_H

#ifdef __cplusplus
extern "C" {
#endif
void EnGSwitch_RandomizerInit(void* actorRef);
#ifdef __cplusplus
};
#endif

#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "randomizer_inf.h"
#include "randomizerTypes.h"

typedef struct SilverRupeeIdentity {
    RandomizerInf randomizerInf;
    union {
        RandomizerCheck randomizerCheck;
        RandomizerGet randomizerGet;
    } index;
    GetItemEntry itemEntry;
} SilverRupeeIdentity;

void ShuffleSilverRupees_RegisterHooks();
void ShuffleSilverRupees_UnregisterHooks();

#ifdef __cplusplus
namespace Rando {
class SilverRupeeCounter {
public:
    SilverRupeeCounter();
    SilverRupeeCounter(uint8_t total, RandomizerGet randoGet);

    uint8_t GetCollected() const;
    uint8_t GetTotal() const;
    void IncrementCollected(uint8_t amount = 1);
    bool AllCollected();
private:
    uint8_t mCollected = 0;
    uint8_t mTotal;
    RandomizerGet mRandoGet;
};
} // namespace Rando
#endif

#endif //SHUFLESILVERRUPEES_H