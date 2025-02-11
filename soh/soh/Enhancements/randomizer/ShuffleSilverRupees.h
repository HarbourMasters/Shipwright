#ifndef SHUFFLESILVERRUPEES_H
#define SHUFFLESILVERRUPEES_H

#ifdef __cplusplus
extern "C" {
#endif
void EnGSwitch_RandomizerInit(void* actorRef);
#ifdef __cplusplus
};
#endif

namespace Rando {
    class SilverRupeeCounter {
    public:
        SilverRupeeCounter();
        SilverRupeeCounter(uint8_t total, RandomizerGet randoGet);

        uint8_t GetCollected() const;
        uint8_t GetTotal() const;
        void IncrementCollected(uint8_t amount = 1)
    private:
        uint8_t mCollected = 0;
        uint8_t mTotal;
        RandomizerGet mRandoGet;
    }
}

#endif //SHUFLESILVERRUPEES_H