#include <unordered_map>
#include "randomizerTypes.h"
#include "item.h"
/**
 * @brief Singleton for storing and accessing static Randomizer-related data
 * 
 * The StaticData class is a class for storing and accessing static Randomizer-related data.
 * Static Data, in this context, refers to data that is the same for every single randomizer seed,
 * i.e. the item and location tables. This class is intended to be used as a Singleton.
 */
class StaticData {
    private:
    std::unordered_map<RandomizerGet, RandoItem> itemTable;
    void InitItemTable();

    public:
    StaticData();
    ~StaticData();

};