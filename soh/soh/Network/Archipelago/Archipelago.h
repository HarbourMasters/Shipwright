#pragma once
#ifdef __cplusplus
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include <vector>
#include <nlohmann/json.hpp>
#include <queue>
#include <map>
#include <atomic>
#include "ArchipelagoTypes.h"

// Forward declaration
class APClient;

namespace AP_Client_consts {
static constexpr int MAX_ADDRESS_LENGTH = 64;
static constexpr int MAX_PLAYER_NAME_LENGHT = 17;
static constexpr int MAX_PASSWORD_LENGTH = 32;

static constexpr char const* AP_GAME_NAME = "Ship of Harkinian";
static constexpr char const* AP_WORLD_VERSION_MAJOR = "1";
static constexpr char const* AP_WORLD_VERSION_MINOR = "3";
static constexpr int MAX_RETRIES = 3;
} // namespace AP_Client_consts

class ArchipelagoClient {
  public:
    struct ApItem {
        std::string itemName;
        std::string hintName;
        std::string locationName;
        std::string playerName;
        int playerNumber;
        unsigned int flags;
        uint64_t index;
    };

    struct ApForeignHint {
        std::string locationName;
        std::string groupName;
        std::string playerName;
        int64_t locationId;
        int64_t playerId;
    };

    static ArchipelagoClient& GetInstance();

    bool StartClient();
    bool StopClient();

    void RequestInitData();
    void GameLoaded();
    void StartLocationScouts();
    void SynchItems();
    void SynchSentLocations();
    void SynchReceivedLocations();
    void InitForeignHints();

    // getters
    int GetSlot() const;
    const std::string GetSlotName() const;
    const std::string GetAlias() const;
    int GetHintCost() const;
    int GetHintPoints() const;

    uint8_t GetConnectionStatus();
    void OnItemGiven(uint32_t rc, GetItemEntry gi, uint8_t isGiSkipped);
    void SendDeathLink();
    void SendDamageLink(int16_t amount);
    void SendTrapLink();
    void SetTags();
    RandomizerGet GetIceTrapItem();
    std::string GetApItemName(int64_t ApItemId);
    std::string GetApItemHint(RandomizerCheck rc, RandomizerGet rg);
    std::string GetApLocationHint(RandomizerHint rh, uint8_t index);
    const nlohmann::json GetSlotData();
    const std::vector<ApItem>& GetScoutedItems();
    void UpdateHintStatus(int player, int location, AP_Hint::HintStatus status);

    bool IsConnected() const;
    void CheckLocation(RandomizerCheck sohCheckId);

    void OnItemReceived(const ApItem apItem);
    void QueueItem(const ApItem item);
    void QueueExternalCheck(int64_t apLocation);

    void SendGameWon();
    void SendMessageToConsole(const std::string message);
    void UpdateHints(const std::vector<nlohmann::json>& hints_json);
    void UpdateItemGroup(const std::string game, const nlohmann::json& item_group_json);
    void UpdateLocationGroup(const std::string game, const nlohmann::json& location_group_json);
    void Poll();
    void ResetQueue();

    void OnSceneInit(uint16_t sceneNum);
    void SetDataStorage(const std::string& key, const nlohmann::json& value) const;

    void OpenLocalHint(RandomizerCheck sohCheckId);
    void OpenForeignHint(RandomizerHint randomizerHintId);
    void OnDialogHook();
    void OnDialogCloseHook();
    void OnShopSlotChangeHook(uint8_t cursorIndex);

    bool slotMatch(const std::string& slotName, const std::string& roomHash);
    void newInitDataRecieved();

    std::unique_ptr<APClient> apClient;
    bool itemQueued;
    bool disconnecting;
    uint64_t lastDeathLink = 0;
    uint64_t lastDamageLink = 0;
    uint8_t trapLinkCount = 0;
    int retries;
    std::string uri;
    std::string password;
    std::unordered_map<RandomizerHint, std::vector<ApForeignHint>> foreignHints;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> item_groups;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> location_groups;

  protected:
    ArchipelagoClient();

  private:
    ArchipelagoClient(ArchipelagoClient&) = delete;
    void operator=(const ArchipelagoClient&) = delete;

    bool isRightSaveLoaded() const;
    std::string get_random_group_from_item(const std::string& item_name, unsigned int item_flags,
                                           const std::string& game);
    std::string get_random_group_from_location(const std::string& item_name, const std::string& game);

    std::string uuid;

    static std::shared_ptr<ArchipelagoClient> instance;
    static bool initialized;

    nlohmann::json slotData;
    std::set<int64_t> locations;
    std::vector<ApItem> scoutedItems;
    std::queue<ApItem> receiveQueue;

    bool locationsScouted;
    bool hintsInitialized;
};

void LoadArchipelagoData();
void SaveArchipelagoData(SaveContext* saveContext, int sectionID, bool fullSave);
void InitArchipelagoData(bool isDebug);
extern "C" {
#endif // END __cplusplus
void Archipelago_InitSaveFile();
void Archipelago_InitConnection();
void Archipelago_RequestInitData();
void SetArchipelagoParsing(uint8_t state);
uint8_t IsArchipelagoParsing();
#ifdef __cplusplus
}
#endif
