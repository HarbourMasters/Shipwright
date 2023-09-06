#ifdef ENABLE_REMOTE_CONTROL

#ifdef __cplusplus
#include <libultraship/libultraship.h>

#include "z64item.h"
#include "z64actor.h"
#include "z64save.h"
#include "./GameInteractor.h"

typedef struct {
    uint32_t clientId;
    std::string clientVersion;
    std::string name;
    Color_RGB8 color;
    std::string seed;
    bool gameComplete;
    uint8_t scene;
    uint8_t roomIndex;
    uint32_t entranceIndex;
    PosRot posRot;
    PlayerData playerData;
    Vec3s jointTable[23];
} AnchorClient;

class GameInteractorAnchor {
    private:
        bool isEnabled;

        void HandleRemoteJson(nlohmann::json payload);
    public:
        static GameInteractorAnchor* Instance;
        static std::map<uint32_t, AnchorClient> AnchorClients;
        static std::vector<uint32_t> FairyIndexToClientId;
        static std::string clientVersion;
        static std::string seed;

        void Enable();
        void Disable();

        void TransmitJsonToRemote(nlohmann::json payload);
};

class AnchorPlayerLocationWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};
};

struct AnchorMessage {
    uint32_t id = 0;
    const char* itemIcon = nullptr;
    std::string prefix = "";
    ImVec4 prefixColor = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);
    std::string message = "";
    ImVec4 messageColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    std::string suffix = "";
    ImVec4 suffixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
    float remainingTime = 10.0f;
};

class AnchorLogWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};
};

#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

void Anchor_RegisterHooks();
void Anchor_PushSaveStateToRemote();
void Anchor_RequestSaveStateFromRemote();
uint8_t Anchor_GetClientScene(uint32_t fairyIndex);
PosRot Anchor_GetClientPosition(uint32_t fairyIndex);
const char* Anchor_GetClientName(uint32_t fairyIndex);
uint8_t Anchor_GetClientRoomIndex(uint32_t fairyIndex);
Vec3s* Anchor_GetClientJointTable(uint32_t puppetIndex);
PlayerData Anchor_GetClientPlayerData(uint32_t puppetIndex);
Color_RGB8 Anchor_GetClientColor(uint32_t fairyIndex);
void Anchor_SpawnClientFairies();
void Anchor_SkipLocation(uint32_t locationIndex, bool skipped);
void Anchor_UpdateBeansBought(uint8_t amount);
void Anchor_UpdateBeansCount(uint8_t amount);
void Anchor_ConsumeAdultTradeItem(uint8_t itemId);
void Anchor_UpdateKeyCount(int16_t sceneNum, int8_t amount);
void Anchor_GameComplete();

#ifdef __cplusplus
}
#endif
