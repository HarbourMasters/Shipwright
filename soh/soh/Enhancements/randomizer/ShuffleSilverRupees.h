#ifndef SHUFFLESILVERRUPEES_H
#define SHUFFLESILVERRUPEES_H

#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "randomizerTypes.h"

typedef struct SilverRupeeIdentity {
    RandomizerInf randomizerInf;
    union {
        RandomizerCheck randomizerCheck;
        RandomizerGet randomizerGet;
    } index;
    GetItemEntry itemEntry;
} SilverRupeeIdentity;

#ifdef __cplusplus
#include <soh/Enhancements/custom-message/CustomMessageManager.h>
CustomMessage GetSilverRupeeItemMessage(uint16_t rgid);
namespace Rando {
class SilverRupeeCounter {
  public:
    SilverRupeeCounter();
    SilverRupeeCounter(uint8_t total, RandomizerGet randoGet, uint8_t dungeonId, RandomizerCheckQuest quest);

    uint8_t GetCollected() const;
    uint8_t GetTotal() const;
    void IncrementCollected(uint8_t amount = 1);
    bool AllCollected();
    void SetCollected(uint8_t newCollected);
    uint8_t DungeonID();
    RandomizerCheckQuest Quest();

  private:
    uint8_t mCollected = 0;
    uint8_t mTotal;
    RandomizerGet mRandoGet;
    uint8_t mDungeonId;
    RandomizerCheckQuest mQuest;
};
} // namespace Rando
#endif

#endif // SHUFLESILVERRUPEES_H