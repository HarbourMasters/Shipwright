#include "SaveEditor.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/Rando/MiscBehavior/ClockShuffle.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/BenGui/Notification.h"
#include "2s2h/Rando/Spoiler/Spoiler.h"
#include "2s2h/ShipUtils.h"

#include "interface/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "archives/icon_item_24_static/icon_item_24_static_yar.h"

#include <fast/Fast3dGui.h>
#include <fast/Fast3dWindow.h>

extern "C" {
#include <z64.h>
#include <z64save.h>
#include <macros.h>
#include <variables.h>
#include <functions.h>
#include "overlays/actors/ovl_En_Test4/z_en_test4.h"
#include "overlays/actors/ovl_Obj_Tokei_Step/z_obj_tokei_step.h"

extern PlayState* gPlayState;
extern SaveContext gSaveContext;
extern TexturePtr gItemIcons[131];
extern s16 D_801CFF94[250];
extern u8 gItemSlots[77];
void Interface_LoadItemIconImpl(PlayState* play, u8 btn);
void Interface_NewDay(PlayState* play, s32 day);
extern RegEditor* gRegEditor;
extern ActorOverlay gActorOverlayTable[ACTOR_ID_MAX];
extern s16 gPlayerFormObjectIds[PLAYER_FORM_MAX];
void Player_PlaySfx(Player* player, u16 sfxId);
void PlayerCall_Init(Actor* thisx, PlayState* play);
void PlayerCall_Update(Actor* thisx, PlayState* play);
void PlayerCall_Draw(Actor* thisx, PlayState* play);
void TransitionFade_SetColor(void* thisx, u32 color);

void ObjTokeiStep_SetupOpen(ObjTokeiStep* objTokeiStep);
void ObjTokeiStep_DrawOpen(Actor* actor, PlayState* play);
void ObjTokeiStep_DoNothing(ObjTokeiStep* objTokeiStep, PlayState* play);
void EnTest4_GetBellTimeOnDay3(EnTest4* thisx);
void EnTest4_GetBellTimeAndShrinkScreenBeforeDay3(EnTest4* thisx, PlayState* play);
}

bool safeMode = true;
constexpr float INV_GRID_WIDTH = 46.0f;
constexpr float INV_GRID_HEIGHT = 58.0f;
constexpr float INV_GRID_ICON_SIZE = 40.0f;
constexpr float INV_GRID_PADDING = 10.0f;
constexpr float INV_GRID_TOP_MARGIN = 20.0f;
constexpr uint16_t HEART_COUNT_MIN = 3;
constexpr uint16_t HEART_COUNT_MAX = 20;
constexpr int16_t S16_ZERO = 0;
constexpr int8_t S8_ZERO = 0;
constexpr u8 U8_ZERO = 0;
constexpr u8 REG_PAGES_MAX = REG_PAGES;
constexpr u8 REG_GROUPS_MAX = REG_GROUPS - 1;
const char* MAGIC_LEVEL_NAMES[3] = { "No Magic", "Single Magic", "Double Magic" };
constexpr int8_t MAGIC_LEVEL_MAX = 2;
const char* WALLET_LEVEL_NAMES[4] = { "Child Wallet", "Adult Wallet", "Giant Wallet", "Tycoon Wallet" };
constexpr u8 WALLET_LEVEL_MAX = 2;
ImVec4 colorTint;
const char* songTooltip;
const char* curForm;
UIWidgets::Colors formColor;
uint32_t formObject;
static std::unordered_map<RandoItemId, const char*> randoItemIdComboboxMap;

InventorySlot selectedInventorySlot = SLOT_NONE;
std::vector<ItemId> safeItemsForInventorySlot[SLOT_MASK_FIERCE_DEITY + 1] = {};

using namespace UIWidgets;

void initSafeItemsForInventorySlot() {
    for (int i = 0; i < sizeof(gItemSlots); i++) {
        InventorySlot slot = static_cast<InventorySlot>(gItemSlots[i]);
        switch (slot) {
            case SLOT_BOTTLE_1:
                if (i != ITEM_LONGSHOT) { // No longshot in bottles
                    safeItemsForInventorySlot[SLOT_BOTTLE_1].push_back(static_cast<ItemId>(i));
                    safeItemsForInventorySlot[SLOT_BOTTLE_2].push_back(static_cast<ItemId>(i));
                    safeItemsForInventorySlot[SLOT_BOTTLE_3].push_back(static_cast<ItemId>(i));
                    safeItemsForInventorySlot[SLOT_BOTTLE_4].push_back(static_cast<ItemId>(i));
                    safeItemsForInventorySlot[SLOT_BOTTLE_5].push_back(static_cast<ItemId>(i));
                    safeItemsForInventorySlot[SLOT_BOTTLE_6].push_back(static_cast<ItemId>(i));
                }
                break;
            case SLOT_BOW:
                if (i == ITEM_BOW) { // No elemental bows here
                    safeItemsForInventorySlot[slot].push_back(static_cast<ItemId>(i));
                }
                break;
            case SLOT_TRADE_KEY_MAMA:
                if (i != ITEM_SLINGSHOT) { // No slingshot in trade items
                    safeItemsForInventorySlot[slot].push_back(static_cast<ItemId>(i));
                }
                break;
            case SLOT_TRADE_DEED:
                if (i != ITEM_OCARINA_FAIRY) { // No fairy ocarina in trade items
                    safeItemsForInventorySlot[slot].push_back(static_cast<ItemId>(i));
                }
                break;
            default:
                safeItemsForInventorySlot[slot].push_back(static_cast<ItemId>(i));
                break;
        }
    }
}

char z2ASCII(int code) {
    int ret;
    if (code < 10) { // Digits
        ret = code + 0x30;
    } else if (code >= 10 && code < 36) { // Uppercase letters
        ret = code + 0x37;
    } else if (code >= 36 && code < 62) { // Lowercase letters
        ret = code + 0x3D;
    } else if (code == 62) { // Space
        ret = code - 0x1E;
    } else if (code == 63 || code == 64) { // _ and .
        ret = code - 0x12;
    } else {
        ret = code;
    }
    return char(ret);
}

char ASCII2z(int code) {
    int ret;
    if (code >= 0x30 && code < 0x3A) { // Digits
        ret = code - 0x30;
    } else if (code >= 0x41 && code < 0x5B) { // Uppercase letters
        ret = code - 0x37;
    } else if (code >= 0x61 && code < 0x7B) { // Lowercase letters
        ret = code - 0x3D;
    } else if (code == 0x20 || code == 0x0) { // Space
        ret = 62;
    } else if (code == 0x5F || code == 0x2E) { // _ and .
        ret = code + 0x12;
    } else {
        ret = code;
    }
    return char(ret);
}

char* getPlayerName(char* playerNameBuf) {
    for (int i = 0; i < 8; i++) {
        playerNameBuf[i] = z2ASCII(gSaveContext.save.saveInfo.playerData.playerName[i]);
    }
    playerNameBuf[8] = '\0';
    return playerNameBuf;
}

int setPlayerName(ImGuiInputTextCallbackData* data) {
    for (int i = 0; i < 8; i++) {
        gSaveContext.save.saveInfo.playerData.playerName[i] = ASCII2z(data->Buf[i]);
    }
    return 0;
};

EnTest4* FindEnTest4Actor() {
    if (gPlayState == NULL) {
        return NULL;
    }

    Actor* enTest4Search = gPlayState->actorCtx.actorLists[ACTORCAT_SWITCH].first;

    while (enTest4Search != NULL) {
        if (enTest4Search->id == ACTOR_EN_TEST4) {
            return (EnTest4*)enTest4Search;
        }
        enTest4Search = enTest4Search->next;
    }

    return NULL;
}

void UpdateGameTime(u16 gameTime) {
    bool newTimeIsNight = (gameTime > CLOCK_TIME(18, 0)) || (gameTime < CLOCK_TIME(6, 0));
    bool prevTimeIsNight = (CURRENT_TIME > CLOCK_TIME(18, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 0));

    gSaveContext.save.time = gameTime;

    if (gPlayState == NULL) {
        return;
    }

    // Clear weather from day 2
    gWeatherMode = WEATHER_MODE_CLEAR;
    gPlayState->envCtx.lightningState = LIGHTNING_OFF;

    // When transitioning over night boundaries, stop the sequences and ask to replay, then respawn actors
    if (newTimeIsNight != prevTimeIsNight) {
        // AMBIENCE_ID_13 is used to persist a scenes sequence through night, so we shouldn't
        // change anything if thats active
        if (gPlayState->sceneSequences.ambienceId != AMBIENCE_ID_13) {
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_AMBIENCE, 0);
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 240);
            gSaveContext.seqId = NA_BGM_DISABLED;
            gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
            Environment_PlaySceneSequence(gPlayState);
        }

        // Kills/Spawns half-day actors
        gPlayState->numSetupActors = -gPlayState->numSetupActors;
    }

    EnTest4* enTest4 = FindEnTest4Actor();

    // Update EnTest4 actor to be in sync with the new time
    // This ensures that day transitions are not triggered with the change
    if (enTest4 != NULL) {
        enTest4->prevTime = gameTime;
        enTest4->prevBellTime = gameTime;
        enTest4->daytimeIndex = newTimeIsNight ? 0 : 1;

        // Sets the nextBellTime based on the new current time
        if (CURRENT_DAY == 3) {
            EnTest4_GetBellTimeOnDay3(enTest4);
        } else {
            EnTest4_GetBellTimeAndShrinkScreenBeforeDay3(enTest4, gPlayState);
        }

        // Unset any screen scaling from the above funcs
        gSaveContext.screenScale = 1000.0f;
        gSaveContext.screenScaleFlag = false;
    }

    // Open the Clock Tower rooftop
    if (((CURRENT_DAY == 3) && (CURRENT_TIME < CLOCK_TIME(6, 0)))) {
        ObjTokeiStep* objTokeiStep = (ObjTokeiStep*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor,
                                                                     ACTOR_OBJ_TOKEI_STEP, ACTORCAT_BG, 99999.9f);
        if (objTokeiStep != NULL && objTokeiStep->actionFunc == ObjTokeiStep_DoNothing) {
            objTokeiStep->dyna.actor.draw = ObjTokeiStep_DrawOpen;
            ObjTokeiStep_SetupOpen(objTokeiStep);
        }
    }
}

void DrawTempleClears() {
    bool cleared;
    bool open;
    bool inverted = false;
    bool inStoneTower = false;

    if (gPlayState != NULL) {
        inStoneTower = Play_GetOriginalSceneId(gPlayState->sceneId) == SCENE_F40;
        inverted = Flags_GetSwitch(gPlayState, 20);
    }

    ImGui::Columns(2, nullptr, false);

    // Left column: Cleared checkboxes
    // Woodfall
    cleared = CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE);
    if (UIWidgets::Checkbox("Woodfall cleared", &cleared)) {
        if (cleared) {
            SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE);
        } else {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE);
        }
    }

    // Snowhead
    cleared = CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE);
    if (UIWidgets::Checkbox("Snowhead cleared", &cleared)) {
        if (cleared) {
            SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE);
        } else {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE);
        }
    }

    // Great Bay
    cleared = CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE);
    if (UIWidgets::Checkbox("Great Bay cleared", &cleared)) {
        if (cleared) {
            SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE);
        } else {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE);
        }
    }

    // Stone Tower
    cleared = CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE);
    if (UIWidgets::Checkbox("Stone Tower cleared", &cleared)) {
        if (cleared) {
            SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE);
        } else {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE);
        }
    }

    ImGui::NextColumn();

    // Right column: Open/Inverted checkboxes
    // Woodfall
    cleared = CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE);
    if (cleared) {
        open = true;
        SET_WEEKEVENTREG(WEEKEVENTREG_20_01);
    } else {
        open = CHECK_WEEKEVENTREG(WEEKEVENTREG_20_01);
    }
    if (UIWidgets::Checkbox("Woodfall Open", &open, { { .disabled = cleared } })) {
        if (open) {
            SET_WEEKEVENTREG(WEEKEVENTREG_20_01);
        } else {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_01);
        }
    }

    // Snowhead
    cleared = CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE);
    if (cleared) {
        open = true;
        SET_WEEKEVENTREG(WEEKEVENTREG_30_01);
    } else {
        open = CHECK_WEEKEVENTREG(WEEKEVENTREG_30_01);
    }
    if (UIWidgets::Checkbox("Snowhead Open", &open, { { .disabled = cleared } })) {
        if (open) {
            SET_WEEKEVENTREG(WEEKEVENTREG_30_01);
        } else {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_30_01);
        }
    }

    // Great Bay
    cleared = CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE);
    if (cleared) {
        open = true;
        // 53_20 is if the turtle is raised
        // 93_08 determines if a long or short cutscene should play
        SET_WEEKEVENTREG(WEEKEVENTREG_53_20);
        SET_WEEKEVENTREG(WEEKEVENTREG_93_08);
    } else {
        open = CHECK_WEEKEVENTREG(WEEKEVENTREG_53_20);
    }
    if (UIWidgets::Checkbox("Great Bay Open", &open, { { .disabled = cleared } })) {
        if (open) {
            SET_WEEKEVENTREG(WEEKEVENTREG_53_20);
            SET_WEEKEVENTREG(WEEKEVENTREG_93_08);
        } else {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_53_20);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_93_08);
        }
    }

    // Stone Tower Inverted
    if (UIWidgets::Checkbox(
            "Stone Tower Inverted", &inverted,
            { { .disabled = !inStoneTower, .disabledTooltip = "Can only invert while in Stone Tower" } })) {
        if (inverted) {
            Flags_SetSwitch(gPlayState, 20);
        } else {
            Flags_UnsetSwitch(gPlayState, 20);
        }
    }

    ImGui::Columns(1);
}

void DrawGeneralTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("generalTab", ImVec2(0, 0), true);

    UIWidgets::BeginCardLayout({ .columnsPerRow = 2, .minColumnWidth = 420.0f });

    // Card 1: Player Identity
    UIWidgets::BeginCard("identityCard");
    ImGui::Text("Player Name");
    ImGui::PushStyleColor(ImGuiCol_FrameBg, UIWidgets::ColorValues.at(UIWidgets::Colors::Gray));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
    static char playerNameBuf[9];
    ImGui::InputText("##playerNameInput", getPlayerName(playerNameBuf), 9,
                     ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_AutoSelectAll, setPlayerName);
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);

    ImGui::Spacing();
    ImGui::Text("Bombers Code: %d %d %d %d %d", gSaveContext.save.saveInfo.bomberCode[0],
                gSaveContext.save.saveInfo.bomberCode[1], gSaveContext.save.saveInfo.bomberCode[2],
                gSaveContext.save.saveInfo.bomberCode[3], gSaveContext.save.saveInfo.bomberCode[4]);

    ImGui::Spacing();
    UIWidgets::Checkbox("Has Tatl", (bool*)&gSaveContext.save.hasTatl, { .color = UIWidgets::Colors::Gray });
    UIWidgets::Checkbox("Is Owl Save", (bool*)&gSaveContext.save.isOwlSave, { .color = UIWidgets::Colors::Gray });
    UIWidgets::Checkbox("Finished Intro Sequence", (bool*)&gSaveContext.save.isFirstCycle,
                        { .color = UIWidgets::Colors::Gray });
    UIWidgets::EndCard();

    // Card 2: Time & Day
    UIWidgets::BeginCard("timeCard");
    ImGui::Text("Time of Day");
    UIWidgets::PushStyleSlider();
    static u16 minTime = 0;
    static u16 maxTime = 0xFFFF;
    // Get current time and format as digital string
    u16 curMinutes = (s32)TIME_TO_MINUTES_F(CURRENT_TIME) % 60;
    u16 curHours = (s32)TIME_TO_MINUTES_F(CURRENT_TIME) / 60;
    std::string minutes = (curMinutes < 10 ? "0" : "") + std::to_string(curMinutes);
    std::string hours = "";
    std::string ampm = "";
    // Handle 24 or 12 hour time
    if (CVarGetInteger("gEnhancements.Graphics.24HoursClock", 0)) {
        if (curHours < 10) {
            hours += "0";
        }
    } else {
        ampm = curHours >= 12 ? " pm" : " am";
        curHours = curHours % 12 ? curHours % 12 : 12;
    }
    hours += std::to_string(curHours);
    std::string timeString = hours + ":" + minutes + ampm + " (0x%x)";

    u16 gameTime = CURRENT_TIME;
    if (ImGui::SliderScalar("##timeInput", ImGuiDataType_U16, &gameTime, &minTime, &maxTime, timeString.c_str())) {
        UpdateGameTime(gameTime);
    }
    UIWidgets::PopStyleSlider();

    ImGui::Spacing();
    static const std::array<std::pair<int8_t, const char*>, 4> timeSkipAmounts = {
        { { -60, "-1hr" }, { -15, "-15m" }, { 15, "+15m" }, { 60, "+1hr" } }
    };
    float availWidth = ImGui::GetContentRegionAvail().x;
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    float buttonWidth = (availWidth - (3 * spacing)) / 4;
    for (size_t i = 0; i < timeSkipAmounts.size(); i++) {
        const auto& skip = timeSkipAmounts.at(i);
        if (UIWidgets::Button(skip.second, { .size = ImVec2(buttonWidth, 0), .color = UIWidgets::Colors::LightBlue })) {
            UpdateGameTime(CURRENT_TIME + CLOCK_TIME(0, skip.first));
        }
        if (i < timeSkipAmounts.size() - 1) {
            ImGui::SameLine();
        }
    }

    UIWidgets::PushStyleSlider();
    static s32 minDay = 0;
    static s32 maxDay = 4;
    if (ImGui::SliderScalar("##dayInput", ImGuiDataType_S32, &gSaveContext.save.day, &minDay, &maxDay, "Day: %d")) {
        gSaveContext.save.eventDayCount = CURRENT_DAY;

        if (gPlayState != nullptr) {
            Interface_NewDay(gPlayState, CURRENT_DAY);
            // Inverting setup actors forces half-day actors to kill/respawn for new day
            gPlayState->numSetupActors = -gPlayState->numSetupActors;
            // Load environment values for new day
            Environment_NewDay(&gPlayState->envCtx);
            // Clear weather from day 2
            gWeatherMode = WEATHER_MODE_CLEAR;
            gPlayState->envCtx.lightningState = LIGHTNING_OFF;
        }
    }

    // Time speed slider
    // Values are added to R_TIME_SPEED which is generally 3 in normal play state
    static s32 minSpeed = -3; // Time frozen
    static s32 maxSpeed = 7;
    std::string extraText = "";
    if (gSaveContext.save.timeSpeedOffset == 0) {
        extraText = " (default)";
    } else if (gSaveContext.save.timeSpeedOffset == -2) {
        extraText = " (inverted)";
    }
    std::string timeSpeedString = "Time Speed: " + std::to_string(gSaveContext.save.timeSpeedOffset + 3) + extraText;
    ImGui::SliderScalar("##timeSpeedInput", ImGuiDataType_S32, &gSaveContext.save.timeSpeedOffset, &minSpeed, &maxSpeed,
                        timeSpeedString.c_str());
    UIWidgets::PopStyleSlider();
    UIWidgets::EndCard();

    // Card 3: Currency
    UIWidgets::BeginCard("currencyCard");
    u8 walletLevelMax = (IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_TYCOON_WALLET] == RO_GENERIC_YES)
                            ? WALLET_LEVEL_MAX + 1
                            : WALLET_LEVEL_MAX;
    if (UIWidgets::Button("Max Rupees", { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Green })) {
        Inventory_ChangeUpgrade(UPG_WALLET, walletLevelMax);
        gSaveContext.save.saveInfo.playerData.rupees = CUR_CAPACITY(UPG_WALLET);
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Reset##resetRupeesButton",
                          { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
        gSaveContext.save.saveInfo.playerData.rupees = 0;
        Inventory_ChangeUpgrade(UPG_WALLET, 0);
    }
    UIWidgets::PushStyleSlider(UIWidgets::Colors::Green);
    u8 currentWalletLevel = CUR_UPG_VALUE(UPG_WALLET);
    if (ImGui::SliderScalar("##walletLevelSlider", ImGuiDataType_U8, &currentWalletLevel, &U8_ZERO, &walletLevelMax,
                            WALLET_LEVEL_NAMES[currentWalletLevel])) {
        Inventory_ChangeUpgrade(UPG_WALLET, currentWalletLevel);
        gSaveContext.save.saveInfo.playerData.rupees =
            MIN(gSaveContext.save.saveInfo.playerData.rupees, CUR_CAPACITY(UPG_WALLET));
    }
    s16 walletCapacity = CUR_CAPACITY(UPG_WALLET);
    ImGui::SliderScalar("##rupeesSlider", ImGuiDataType_S16, &gSaveContext.save.saveInfo.playerData.rupees, &S16_ZERO,
                        &walletCapacity, "Rupees: %d");

    int bankedRupees = HS_GET_BANK_RUPEES();
    if (ImGui::SliderInt("##setBank", &bankedRupees, 0, 5000, "Banked Rupees: %d")) {
        HS_SET_BANK_RUPEES(bankedRupees);
    }
    UIWidgets::Tooltip("To receive the rewards, set the bank to 199, 999, or 4,999 then deposit a single rupee");
    UIWidgets::PopStyleSlider();
    UIWidgets::EndCard();

    // Card 4: Health
    UIWidgets::BeginCard("healthCard");
    if (UIWidgets::Button("Max Health", { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
        gSaveContext.save.saveInfo.playerData.doubleDefense = 1;
        gSaveContext.save.saveInfo.inventory.defenseHearts = 20;
        gSaveContext.save.saveInfo.playerData.healthCapacity = gSaveContext.save.saveInfo.playerData.health = 20 * 16;
        gSaveContext.save.saveInfo.inventory.questItems &= ~0xF0000000;
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Reset##resetHealthButton",
                          { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
        gSaveContext.save.saveInfo.playerData.doubleDefense = 0;
        gSaveContext.save.saveInfo.inventory.defenseHearts = 0;
        gSaveContext.save.saveInfo.playerData.healthCapacity = gSaveContext.save.saveInfo.playerData.health = 3 * 16;
        gSaveContext.save.saveInfo.inventory.questItems &= ~0xF0000000;
    }
    ImGui::SameLine();
    if (UIWidgets::Checkbox("DD", (bool*)&gSaveContext.save.saveInfo.playerData.doubleDefense,
                            { .color = UIWidgets::Colors::Red })) {
        if (gSaveContext.save.saveInfo.playerData.doubleDefense) {
            gSaveContext.save.saveInfo.inventory.defenseHearts = 20;
        } else {
            gSaveContext.save.saveInfo.inventory.defenseHearts = 0;
        }
    }
    UIWidgets::PushStyleSlider(UIWidgets::Colors::DarkRed);
    int16_t heartCount = (int16_t)gSaveContext.save.saveInfo.playerData.healthCapacity / 16;
    if (ImGui::SliderScalar("##heartCountSlider", ImGuiDataType_U16, &heartCount, &HEART_COUNT_MIN, &HEART_COUNT_MAX,
                            "Max Hearts: %d")) {
        gSaveContext.save.saveInfo.playerData.healthCapacity = heartCount * 16;
        gSaveContext.save.saveInfo.playerData.health =
            MIN(gSaveContext.save.saveInfo.playerData.health, gSaveContext.save.saveInfo.playerData.healthCapacity);
    }
    ImGui::SliderScalar("##healthSlider", ImGuiDataType_S16, &gSaveContext.save.saveInfo.playerData.health, &S16_ZERO,
                        &gSaveContext.save.saveInfo.playerData.healthCapacity, "Health: %d");
    UIWidgets::PopStyleSlider();
    UIWidgets::EndCard();

    // Card 5: Magic
    UIWidgets::BeginCard("magicCard");
    if (UIWidgets::Button("Max Magic", { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::DarkGreen })) {
        gSaveContext.magicCapacity = gSaveContext.save.saveInfo.playerData.magic = MAGIC_DOUBLE_METER;
        gSaveContext.save.saveInfo.playerData.magicLevel = 2;
        gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
        gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = true;
        BUTTON_ITEM_EQUIP(PLAYER_FORM_DEKU, EQUIP_SLOT_B) = ITEM_DEKU_NUT;
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Reset##resetMagicButton",
                          { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
        gSaveContext.magicCapacity = gSaveContext.save.saveInfo.playerData.magic = 0;
        gSaveContext.save.saveInfo.playerData.magicLevel = 0;
        gSaveContext.save.saveInfo.playerData.isMagicAcquired = false;
        gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = false;
    }
    bool spinAttack = CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK);
    if (UIWidgets::Checkbox("Spin Lv2", &spinAttack, { .color = UIWidgets::Colors::DarkGreen })) {
        if (spinAttack) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK);
        } else {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK);
        }
    }
    ImGui::SameLine();
    bool drankChateau = CHECK_WEEKEVENTREG(WEEKEVENTREG_DRANK_CHATEAU_ROMANI);
    if (UIWidgets::Checkbox("Drank Chateau", &drankChateau, { .color = UIWidgets::Colors::DarkGreen })) {
        if (drankChateau) {
            SET_WEEKEVENTREG(WEEKEVENTREG_DRANK_CHATEAU_ROMANI);
        } else {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_DRANK_CHATEAU_ROMANI);
        }
    }
    UIWidgets::PushStyleSlider(UIWidgets::Colors::DarkGreen);
    if (ImGui::SliderScalar("##magicLevelSlider", ImGuiDataType_S8, &gSaveContext.save.saveInfo.playerData.magicLevel,
                            &S8_ZERO, &MAGIC_LEVEL_MAX,
                            MAGIC_LEVEL_NAMES[gSaveContext.save.saveInfo.playerData.magicLevel])) {
        gSaveContext.magicCapacity = gSaveContext.save.saveInfo.playerData.magicLevel * MAGIC_NORMAL_METER;
        gSaveContext.save.saveInfo.playerData.magic =
            MIN(gSaveContext.save.saveInfo.playerData.magic, gSaveContext.magicCapacity);
        switch (gSaveContext.save.saveInfo.playerData.magicLevel) {
            case 0:
                gSaveContext.save.saveInfo.playerData.isMagicAcquired = false;
                gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = false;
                break;
            case 1:
                gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
                gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = false;
                BUTTON_ITEM_EQUIP(PLAYER_FORM_DEKU, EQUIP_SLOT_B) = ITEM_DEKU_NUT;
                break;
            case 2:
                gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
                gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = true;
                BUTTON_ITEM_EQUIP(PLAYER_FORM_DEKU, EQUIP_SLOT_B) = ITEM_DEKU_NUT;
                break;
        }
    }
    ImGui::SliderScalar("##magicSlider", ImGuiDataType_S8, &gSaveContext.save.saveInfo.playerData.magic, &S8_ZERO,
                        &gSaveContext.magicCapacity, "Magic: %d");
    UIWidgets::PopStyleSlider();
    UIWidgets::EndCard();

    // Card 6: Temple Progress
    UIWidgets::BeginCard("progressCard");
    DrawTempleClears();
    UIWidgets::EndCard();

    UIWidgets::EndCardLayout();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

template <typename C, typename T> bool contains(C&& c, T e) {
    return std::find(std::begin(c), std::end(c), e) != std::end(c);
};

void DrawAmmoInput(InventorySlot slot) {
    int x = slot % 6;
    int y = ((int)floor(slot / 6) % 4);
    ItemId currentItemId = static_cast<ItemId>(gSaveContext.save.saveInfo.inventory.items[slot]);

    ImGui::SetCursorPos(
        ImVec2(x * INV_GRID_WIDTH + INV_GRID_PADDING + 7.0f,
               y * INV_GRID_HEIGHT + INV_GRID_TOP_MARGIN + INV_GRID_PADDING + (INV_GRID_ICON_SIZE - 2.0f)));
    ImGui::PushItemWidth(24.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, UIWidgets::ColorValues.at(UIWidgets::Colors::Gray));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
    if (ImGui::InputScalar("##ammoInput", ImGuiDataType_S8, &AMMO(currentItemId))) {
        // Crashes when < 0 and graphical issues when > 99
        AMMO(currentItemId) = MAX(0, MIN(AMMO(currentItemId), 99));
    }
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
    ImGui::PopItemWidth();
}

void DrawEquipItemMenu(InventorySlot slot) {
    ItemId currentItemId = static_cast<ItemId>(gSaveContext.save.saveInfo.inventory.items[slot]);

    if (ImGui::BeginMenu("Equip")) {
        if (ImGui::MenuItem("C-Left")) {
            SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_LEFT, currentItemId);
            SET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_C_LEFT, slot);
            Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_C_LEFT);
        }
        if (ImGui::MenuItem("C-Down")) {
            SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_DOWN, currentItemId);
            SET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_C_DOWN, slot);
            Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_C_DOWN);
        }
        if (ImGui::MenuItem("C-Right")) {
            SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_RIGHT, currentItemId);
            SET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_C_RIGHT, slot);
            Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_C_RIGHT);
        }
        if (ImGui::MenuItem("D-Right")) {
            DPAD_SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_RIGHT, currentItemId);
            DPAD_SET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_D_RIGHT, slot);
            Interface_Dpad_LoadItemIconImpl(gPlayState, EQUIP_SLOT_D_RIGHT);
        }
        if (ImGui::MenuItem("D-Left")) {
            DPAD_SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_LEFT, currentItemId);
            DPAD_SET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_D_LEFT, slot);
            Interface_Dpad_LoadItemIconImpl(gPlayState, EQUIP_SLOT_D_LEFT);
        }
        if (ImGui::MenuItem("D-Down")) {
            DPAD_SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_DOWN, currentItemId);
            DPAD_SET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_D_DOWN, slot);
            Interface_Dpad_LoadItemIconImpl(gPlayState, EQUIP_SLOT_D_DOWN);
        }
        if (ImGui::MenuItem("D-Up")) {
            DPAD_SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_UP, currentItemId);
            DPAD_SET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_D_UP, slot);
            Interface_Dpad_LoadItemIconImpl(gPlayState, EQUIP_SLOT_D_UP);
        }
        ImGui::EndMenu();
    }
}

void NextItemInSlot(InventorySlot slot) {
    ItemId currentItemId = static_cast<ItemId>(gSaveContext.save.saveInfo.inventory.items[slot]);
    size_t currentItemIndex =
        find(safeItemsForInventorySlot[slot].begin(), safeItemsForInventorySlot[slot].end(), currentItemId) -
        safeItemsForInventorySlot[slot].begin();

    if (currentItemId == ITEM_NONE) {
        gSaveContext.save.saveInfo.inventory.items[slot] = safeItemsForInventorySlot[slot][0];

        if (gSaveContext.save.saveInfo.inventory.items[slot] == ITEM_BOW) {
            Inventory_ChangeUpgrade(UPG_QUIVER, 1);
            AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
        } else if (gSaveContext.save.saveInfo.inventory.items[slot] == ITEM_BOMB ||
                   gSaveContext.save.saveInfo.inventory.items[slot] == ITEM_BOMBCHU) {
            if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
                Inventory_ChangeUpgrade(UPG_BOMB_BAG, 1);
            }
            AMMO(gSaveContext.save.saveInfo.inventory.items[slot]) = CUR_CAPACITY(UPG_BOMB_BAG);
        }
    } else if (currentItemId == ITEM_BOW) {
        if (CUR_UPG_VALUE(UPG_QUIVER) < 3) {
            Inventory_ChangeUpgrade(UPG_QUIVER, CUR_UPG_VALUE(UPG_QUIVER) + 1);
            AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
        } else {
            Inventory_DeleteItem(gSaveContext.save.saveInfo.inventory.items[slot], slot);
            Inventory_ChangeUpgrade(UPG_QUIVER, 0);
            AMMO(ITEM_BOW) = 0;
        }
    } else if (currentItemId == ITEM_BOMB || currentItemId == ITEM_BOMBCHU) {
        if (CUR_UPG_VALUE(UPG_BOMB_BAG) < 3) {
            Inventory_ChangeUpgrade(UPG_BOMB_BAG, CUR_UPG_VALUE(UPG_BOMB_BAG) + 1);
            AMMO(ITEM_BOMB) = AMMO(ITEM_BOMBCHU) = CUR_CAPACITY(UPG_BOMB_BAG);
        } else {
            Inventory_DeleteItem(ITEM_BOMB, SLOT_BOMB);
            Inventory_DeleteItem(ITEM_BOMBCHU, SLOT_BOMBCHU);
            Inventory_ChangeUpgrade(UPG_BOMB_BAG, 0);
            AMMO(ITEM_BOMB) = AMMO(ITEM_BOMBCHU) = 0;
        }
    } else if (currentItemIndex < safeItemsForInventorySlot[slot].size() - 1) {
        Inventory_ReplaceItem(gPlayState, currentItemId, safeItemsForInventorySlot[slot][currentItemIndex + 1]);
    } else {
        Inventory_DeleteItem(gSaveContext.save.saveInfo.inventory.items[slot], slot);
    }
}

void DrawSlot(InventorySlot slot) {
    int x = slot % 6;
    int y = ((int)floor(slot / 6) % 4);
    ItemId currentItemId = static_cast<ItemId>(gSaveContext.save.saveInfo.inventory.items[slot]);

    ImGui::PushID(slot);

    if (currentItemId != ITEM_NONE &&
        currentItemId <= ITEM_BOW_LIGHT &&            // gItemSlots only has entries till 77 (ITEM_BOW_LIGHT)
        gItemSlots[currentItemId] <= SLOT_BOTTLE_6 && // There is only ammo data for the first page
        ((safeMode && gAmmoItems[gItemSlots[currentItemId]] != ITEM_NONE) || !safeMode)) {
        DrawAmmoInput(slot);
    }

    ImGui::SetCursorPos(
        ImVec2(x * INV_GRID_WIDTH + INV_GRID_PADDING, y * INV_GRID_HEIGHT + INV_GRID_TOP_MARGIN + INV_GRID_PADDING));

    // isEquipped border
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    if (GET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_C_LEFT) == slot || GET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_C_DOWN) == slot ||
        GET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_C_RIGHT) == slot ||
        (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0) &&
         (DPAD_GET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_D_RIGHT) == slot ||
          DPAD_GET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_D_LEFT) == slot ||
          DPAD_GET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_D_DOWN) == slot ||
          DPAD_GET_CUR_FORM_BTN_SLOT(EQUIP_SLOT_D_UP) == slot))) {
        ImGui::PushStyleColor(ImGuiCol_Border, UIWidgets::ColorValues.at(UIWidgets::Colors::White));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

    ImTextureID textureId =
        std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->GetTextureByName((const char*)gItemIcons[safeItemsForInventorySlot[slot][0]]);

    if (currentItemId != ITEM_NONE) {
        textureId = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                        ->GetTextureByName((const char*)gItemIcons[currentItemId]);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    bool buttonPressed =
        ImGui::ImageButton((const char*)gItemIcons[safeItemsForInventorySlot[slot][0]], textureId,
                           ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1),
                           ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, currentItemId == ITEM_NONE ? 0.4f : 1.0f));
    ImGui::PopStyleVar();
    if (buttonPressed) {
        if (safeMode && safeItemsForInventorySlot[slot].size() < 2) {
            NextItemInSlot(slot);
        } else {
            selectedInventorySlot = slot;
            ImGui::OpenPopup("InventorySlotPopup");
        }
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);

    if (ImGui::BeginPopup("InventorySlotPopup")) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
        if (ImGui::Button("##invNonePicker", ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE))) {
            Inventory_DeleteItem(gSaveContext.save.saveInfo.inventory.items[selectedInventorySlot],
                                 selectedInventorySlot);
            ImGui::CloseCurrentPopup();
        }
        UIWidgets::Tooltip("None");

        size_t availableItems = safeMode ? safeItemsForInventorySlot[selectedInventorySlot].size() : ITEM_FISHING_ROD;
        for (int32_t pickerIndex = 0; pickerIndex < availableItems; pickerIndex++) {
            if (((pickerIndex + 1) % 8) != 0) {
                ImGui::SameLine();
            }
            ItemId id = safeMode ? safeItemsForInventorySlot[selectedInventorySlot][pickerIndex]
                                 : static_cast<ItemId>(pickerIndex);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            bool buttonPressed = ImGui::ImageButton(
                (const char*)gItemIcons[id],
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName((const char*)gItemIcons[id]),
                ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE));
            ImGui::PopStyleVar();
            if (buttonPressed) {
                gSaveContext.save.saveInfo.inventory.items[selectedInventorySlot] = id;
                ImGui::CloseCurrentPopup();
            }
            // TODO: Add names for items
            // UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(id));
        }
        ImGui::PopStyleColor(3);
        ImGui::EndPopup();
    }

    ImGui::PopID();

    if (currentItemId != ITEM_NONE && ImGui::BeginPopupContextItem()) {
        DrawEquipItemMenu(slot);
        ImGui::EndPopup();
    }
}

void DrawItemsAndMasksTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    // Group left side in a child for a scroll bar
    ImGui::BeginChild("leftSide##items", ImVec2(0, 0), ImGuiChildFlags_AutoResizeX);
    ImGui::BeginGroup();
    ImGui::BeginChild("itemsBox",
                      ImVec2(INV_GRID_WIDTH * 6 + INV_GRID_PADDING * 2,
                             INV_GRID_HEIGHT * 4 + INV_GRID_PADDING * 2 + INV_GRID_TOP_MARGIN),
                      ImGuiChildFlags_Border);
    ImGui::Text("Items");
    for (uint32_t i = SLOT_OCARINA; i <= SLOT_BOTTLE_6; i++) {
        InventorySlot slot = static_cast<InventorySlot>(i);

        DrawSlot(slot);
    }
    ImGui::EndChild();
    ImGui::BeginChild("masksBox",
                      ImVec2(INV_GRID_WIDTH * 6 + INV_GRID_PADDING * 2,
                             INV_GRID_HEIGHT * 4 + INV_GRID_PADDING * 2 + INV_GRID_TOP_MARGIN),
                      ImGuiChildFlags_Border);
    ImGui::Text("Masks");
    for (uint32_t i = SLOT_MASK_POSTMAN; i <= SLOT_MASK_FIERCE_DEITY; i++) {
        InventorySlot slot = static_cast<InventorySlot>(i);

        DrawSlot(slot);
    }
    ImGui::EndChild();
    ImGui::EndGroup();
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("equipsBox", ImVec2(0, 0), true);
    if (UIWidgets::Button("Give All##items")) {
        for (int32_t i = 0; i <= SLOT_MASK_FIERCE_DEITY; i++) {
            if (i >= SLOT_BOTTLE_1 && i <= SLOT_BOTTLE_6) {
                gSaveContext.save.saveInfo.inventory.items[i] = ITEM_BOTTLE;
            } else {
                gSaveContext.save.saveInfo.inventory.items[i] = safeItemsForInventorySlot[i].back();
            }
        }
        Inventory_ChangeUpgrade(UPG_BOMB_BAG, 3);
        Inventory_ChangeUpgrade(UPG_QUIVER, 3);
        AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
        AMMO(ITEM_BOMB) = AMMO(ITEM_BOMBCHU) = CUR_CAPACITY(UPG_BOMB_BAG);
        AMMO(ITEM_DEKU_STICK) = CUR_CAPACITY(UPG_DEKU_STICKS);
        AMMO(ITEM_DEKU_NUT) = CUR_CAPACITY(UPG_DEKU_NUTS);
        AMMO(ITEM_MAGIC_BEANS) = 20;
        AMMO(ITEM_POWDER_KEG) = 1;
    }
    if (UIWidgets::Button("Reset##items")) {
        for (int32_t i = 0; i <= SLOT_MASK_FIERCE_DEITY; i++) {
            Inventory_DeleteItem(gSaveContext.save.saveInfo.inventory.items[i], i);
        }
        AMMO(ITEM_BOW) = AMMO(ITEM_BOMB) = AMMO(ITEM_BOMBCHU) = AMMO(ITEM_DEKU_STICK) = AMMO(ITEM_DEKU_NUT) =
            AMMO(ITEM_MAGIC_BEANS) = AMMO(ITEM_POWDER_KEG) = 0;
        Inventory_ChangeUpgrade(UPG_BOMB_BAG, 0);
        Inventory_ChangeUpgrade(UPG_QUIVER, 0);
    }
    UIWidgets::Checkbox("Safe Mode", &safeMode);

    if (gSaveContext.save.shipSaveInfo.saveType == SAVETYPE_RANDO) {
        if (RANDO_SAVE_OPTIONS[RO_CLOCK_SHUFFLE]) {
            // Time Items Management Section
            ImGui::SeparatorText("Time Items");

            // Individual time items in 3x2 grid with static positioning
            RandoItemId clockItems[] = { RI_TIME_DAY_1,   RI_TIME_DAY_2,   RI_TIME_DAY_3,
                                         RI_TIME_NIGHT_1, RI_TIME_NIGHT_2, RI_TIME_NIGHT_3 };

            const char* clockNames[] = { "Day 1", "Day 2", "Day 3", "Night 1", "Night 2", "Night 3" };

            // Use table for static positioning - 3 columns, 2 rows
            if (ImGui::BeginTable("ClockItemsTable", 3, ImGuiTableFlags_None)) {
                ImGui::TableSetupColumn("Day 1", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Day 2", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Day 3", ImGuiTableColumnFlags_WidthStretch);

                // First row - Day items
                ImGui::TableNextRow();
                for (int i = 0; i < 3; i++) {
                    ImGui::TableNextColumn();
                    RandoItemId clockItem = clockItems[i];
                    int halfIndex = Rando::ClockItems::GetHalfDayIndexFromClockItem(clockItem);
                    bool isOwned = Flags_GetRandoInf(static_cast<RandoInf>(RANDO_INF_OBTAINED_CLOCK_DAY_1 + halfIndex));

                    std::string buttonText =
                        isOwned ? ("Remove " + std::string(clockNames[i])) : ("No Item##" + std::to_string(i));
                    std::string tooltipText = "";
                    if (!isOwned) {
                        tooltipText = "You don't own " + std::string(clockNames[i]);
                    }
                    UIWidgets::ButtonOptions buttonOpts;
                    buttonOpts.disabled = !isOwned;
                    buttonOpts.disabledTooltip = !isOwned ? tooltipText.c_str() : "";
                    if (UIWidgets::Button(buttonText.c_str(), buttonOpts)) {
                        Rando::RemoveItem(clockItem);
                    }
                }

                // Second row - Night items
                ImGui::TableNextRow();
                for (int i = 3; i < 6; i++) {
                    ImGui::TableNextColumn();
                    RandoItemId clockItem = clockItems[i];
                    int halfIndex = Rando::ClockItems::GetHalfDayIndexFromClockItem(clockItem);
                    bool isOwned = Flags_GetRandoInf(static_cast<RandoInf>(RANDO_INF_OBTAINED_CLOCK_DAY_1 + halfIndex));

                    std::string buttonText =
                        isOwned ? ("Remove " + std::string(clockNames[i])) : ("No Item##" + std::to_string(i));
                    std::string tooltipText = "";
                    if (!isOwned) {
                        tooltipText = "You don't own " + std::string(clockNames[i]);
                    }
                    UIWidgets::ButtonOptions buttonOpts;
                    buttonOpts.disabled = !isOwned;
                    buttonOpts.disabledTooltip = !isOwned ? tooltipText.c_str() : "";
                    if (UIWidgets::Button(buttonText.c_str(), buttonOpts)) {
                        Rando::RemoveItem(clockItem);
                    }
                }

                ImGui::EndTable();
            }
        }

        // Queue Randomizer Item Gives section
        ImGui::Spacing();
        ImGui::SeparatorText("Queue Randomizer Item Gives");

        static ImGuiTextFilter riFilter;
        UIWidgets::PushStyleCombobox();
        riFilter.Draw("##filter", ImGui::GetContentRegionAvail().x);
        UIWidgets::PopStyleCombobox();
        if (!riFilter.IsActive()) {
            ImGui::SameLine(18.0f);
            ImGui::Text("Search");
        }
        std::string riFilterString(riFilter.InputBuf);

        for (auto& [randoItemId, randoStaticItem] : Rando::StaticData::Items) {
            if (!riFilter.PassFilter(randoStaticItem.name)) {
                continue;
            }
            if (randoItemId == RI_TRIFORCE_PIECE_PREVIOUS) {
                continue;
            }

            std::string buttonLabel = "Give ";
            buttonLabel += randoStaticItem.name;
            if (UIWidgets::Button(buttonLabel.c_str())) {
                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                    .showGetItemCutscene =
                        Rando::StaticData::ShouldShowGetItemCutscene(Rando::ConvertItem(randoItemId)),
                    .param = (int16_t)randoItemId,
                    .giveItem =
                        [](Actor* actor, PlayState* play) {
                            RandoItemId randoItemId = Rando::ConvertItem((RandoItemId)CUSTOM_ITEM_PARAM);
                            std::string prefix = "You found";
                            std::string message = Rando::StaticData::GetItemName(randoItemId);

                            CustomMessage::Entry entry = {
                                .textboxType = 2,
                                .icon = Rando::StaticData::GetIconForZMessage(randoItemId),
                                .msg = prefix + " " + message + "!",
                            };

                            if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                CustomMessage::SetActiveCustomMessage(entry.msg, entry);
                            } else if (Rando::StaticData::ShouldShowGetItemCutscene(
                                           Rando::ConvertItem((RandoItemId)CUSTOM_ITEM_PARAM))) {
                                CustomMessage::StartTextbox(entry.msg + "\x1C\x02\x10", entry);
                            } else {
                                Notification::Emit({
                                    .itemIcon = Rando::StaticData::GetIconTexturePath(randoItemId),
                                    .message = prefix,
                                    .suffix = message,
                                });
                            }
                            Rando::GiveItem(randoItemId);
                            CUSTOM_ITEM_PARAM = randoItemId;
                        },
                    .drawItem =
                        [](Actor* actor, PlayState* play) {
                            RandoItemId randoItemId;

                            // If the item has been given, the CUSTOM_ITEM_PARAM is set to the RI, prior to that it's
                            // the RC
                            if (CUSTOM_ITEM_FLAGS & CustomItem::CALLED_ACTION) {
                                randoItemId = (RandoItemId)CUSTOM_ITEM_PARAM;
                            } else {
                                randoItemId = Rando::ConvertItem((RandoItemId)CUSTOM_ITEM_PARAM);
                            }

                            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                            Rando::DrawItem(randoItemId, RC_UNKNOWN, actor);
                        } });
            }
        }
    }

    // Expose inputs to edit raw number values of equips
    // ImGui::Text("Equips");
    // ImGui::Text("C-Buttons");
    // ImGui::Text("D-Pad");
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

void SongInfo(int32_t itemID) {
    switch (itemID) {
        case QUEST_SONG_SONATA:
            colorTint = ImVec4(0.588f, 1.0f, 0.392f, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "Sonata of Awakening";
            break;
        case QUEST_SONG_LULLABY:
            colorTint = ImVec4(1.0f, 0.313f, 0.156f,
                               (CHECK_QUEST_ITEM(itemID) || CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)) ? 1.0f : 0.4f);
            songTooltip = (!CHECK_QUEST_ITEM(itemID) && CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO))
                              ? "Goron Lullaby Intro"
                              : "Goron Lullaby";
            break;
        case QUEST_SONG_BOSSA_NOVA:
            colorTint = ImVec4(0.392f, 0.588f, 1.0f, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "New Wave Bossa Nova";
            break;
        case QUEST_SONG_ELEGY:
            colorTint = ImVec4(1.0f, 0.627f, 0.0f, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "Elegy of Emptiness";
            break;
        case QUEST_SONG_OATH:
            colorTint = ImVec4(1.0f, 0.392f, 1.0f, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "Oath to Order";
            break;
        case QUEST_SONG_TIME:
            colorTint = ImVec4(1, 1, 1, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "Song of Time";
            break;
        case QUEST_SONG_HEALING:
            colorTint = ImVec4(1, 1, 1, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "Song of Healing";
            break;
        case QUEST_SONG_EPONA:
            colorTint = ImVec4(1, 1, 1, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "Eponas Song";
            break;
        case QUEST_SONG_SOARING:
            colorTint = ImVec4(1, 1, 1, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "Song of Soaring";
            break;
        case QUEST_SONG_STORMS:
            colorTint = ImVec4(1, 1, 1, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "Song of Storms";
            break;
        case QUEST_SONG_SARIA:
            colorTint = ImVec4(1, 1, 1, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "Saria's Song (Unused?)";
            break;
        case QUEST_SONG_SUN:
            colorTint = ImVec4(1, 1, 1, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = "Sun's Song (Not Obtainable)";
            break;
        default:
            colorTint = ImVec4(1, 1, 1, CHECK_QUEST_ITEM(itemID) ? 1.0f : 0.4f);
            songTooltip = " ";
            break;
    }
}

std::map<QuestItem, ItemId> questToItemMap = {
    { QUEST_REMAINS_ODOLWA, ITEM_REMAINS_ODOLWA }, { QUEST_REMAINS_GOHT, ITEM_REMAINS_GOHT },
    { QUEST_REMAINS_GYORG, ITEM_REMAINS_GYORG },   { QUEST_REMAINS_TWINMOLD, ITEM_REMAINS_TWINMOLD },
    { QUEST_SONG_SONATA, ITEM_SONG_SONATA },       { QUEST_SONG_LULLABY, ITEM_SONG_LULLABY },
    { QUEST_SONG_BOSSA_NOVA, ITEM_SONG_NOVA },     { QUEST_SONG_ELEGY, ITEM_SONG_ELEGY },
    { QUEST_SONG_OATH, ITEM_SONG_OATH },           { QUEST_SONG_SARIA, ITEM_SONG_SARIA },
    { QUEST_SONG_TIME, ITEM_SONG_TIME },           { QUEST_SONG_HEALING, ITEM_SONG_HEALING },
    { QUEST_SONG_EPONA, ITEM_SONG_EPONA },         { QUEST_SONG_SOARING, ITEM_SONG_SOARING },
    { QUEST_SONG_STORMS, ITEM_SONG_STORMS },       { QUEST_SONG_SUN, ITEM_SONG_SUN }
};

void NextQuestInSlot(QuestItem slot) {
    if (!gPlayState) {
        return;
    }
    Player* player = GET_PLAYER(gPlayState);
    if (slot == QUEST_SONG_LULLABY || slot == QUEST_SONG_LULLABY_INTRO) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
            REMOVE_QUEST_ITEM(QUEST_SONG_LULLABY);
            REMOVE_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO);
        } else if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)) {
            SET_QUEST_ITEM(QUEST_SONG_LULLABY);
        } else {
            SET_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO);
        }
    } else if (slot == QUEST_SWORD) {
        uint32_t currentSword = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD);
        if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_GILDED) {
            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_NONE);
        } else {
            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, currentSword + 1);
        }
        if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_NONE) {
            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_NONE;
        } else {
            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) =
                ITEM_SWORD_KOKIRI + GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) - EQUIP_VALUE_SWORD_KOKIRI;
        }
        if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_RAZOR) {
            gSaveContext.save.saveInfo.playerData.swordHealth = 100;
        }
        Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_B);
    } else if (slot == QUEST_SHIELD) {
        uint32_t currentShield = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD);
        if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) == EQUIP_VALUE_SHIELD_NONE) {
            SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_HERO);
        } else if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) == EQUIP_VALUE_SHIELD_HERO) {
            SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_MIRROR);
        } else {
            SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_NONE);
        }
        Player_SetEquipmentData(gPlayState, player);
    } else {
        if (CHECK_QUEST_ITEM(slot)) {
            REMOVE_QUEST_ITEM(slot);
        } else {
            SET_QUEST_ITEM(slot);
        }
    }
}

void DrawQuestSlot(QuestItem slot) {
    int x = slot % 6;
    int y = ((int)floor(slot / 6) % 4);
    ImGui::PushID(slot);

    ImGui::SetCursorPos(
        ImVec2(x * INV_GRID_WIDTH + INV_GRID_PADDING, y * INV_GRID_HEIGHT + INV_GRID_TOP_MARGIN + INV_GRID_PADDING));

    ImTextureID textureId =
        std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->GetTextureByName((const char*)gItemIcons[questToItemMap[slot]]);
    if (ImGui::ImageButton(std::to_string(slot).c_str(), textureId, ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE),
                           ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                           ImVec4(1, 1, 1, CHECK_QUEST_ITEM(slot) ? 1.0f : 0.4f))) {
        NextQuestInSlot(slot);
    }

    ImGui::PopID();
}

ImVec2 DrawSong(QuestItem slot) {
    SongInfo(slot);
    if (ImGui::ImageButton(
            std::to_string(slot).c_str(),
            std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                ->GetTextureByName((const char*)gItemIcons[questToItemMap[(QuestItem)slot]]),
            ImVec2(INV_GRID_ICON_SIZE / 1.5f, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
            colorTint)) {
        NextQuestInSlot(slot);
    }
    ImVec2 itemSize = ImGui::GetItemRectSize();
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", songTooltip);
        ImGui::EndTooltip();
    }
    return itemSize;
}

void DrawQuestStatusTab() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("questTab", ImVec2(0, 0), true);

    if (UIWidgets::Button("Give All##items", { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Green })) {
        for (int32_t i = QUEST_REMAINS_ODOLWA; i <= QUEST_BOMBERS_NOTEBOOK; i++) {
            if (i != QUEST_SHIELD && i != QUEST_SWORD) {
                SET_QUEST_ITEM(i);
            }
        }
        SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_MIRROR);
        if (GET_PLAYER_FORM != PLAYER_FORM_FIERCE_DEITY) {
            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_GILDED);
            if (gPlayState) {
                Player_SetEquipmentData(gPlayState, GET_PLAYER(gPlayState));
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) =
                    ITEM_SWORD_KOKIRI + GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) - EQUIP_VALUE_SWORD_KOKIRI;
                Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_B);
            }
        }
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Reset##items", { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
        for (int32_t i = QUEST_REMAINS_ODOLWA; i <= QUEST_BOMBERS_NOTEBOOK; i++) {
            if (i != QUEST_SHIELD && i != QUEST_SWORD) {
                REMOVE_QUEST_ITEM(i);
            }
        }
        REMOVE_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO);
        SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_HERO);
        if (GET_PLAYER_FORM != PLAYER_FORM_FIERCE_DEITY) {
            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_KOKIRI);
            if (gPlayState) {
                Player_SetEquipmentData(gPlayState, GET_PLAYER(gPlayState));
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) =
                    ITEM_SWORD_KOKIRI + GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) - EQUIP_VALUE_SWORD_KOKIRI;
                Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_B);
            }
        }
    }
    ImGui::BeginChild("leftColumn", ImVec2(ImGui::GetWindowWidth() / 2, 0));
    ImGui::SeparatorText("Boss Remains");
    for (int32_t i = QUEST_REMAINS_ODOLWA; i <= QUEST_REMAINS_TWINMOLD; i++) {
        QuestItem slot = static_cast<QuestItem>(i);

        DrawQuestSlot(slot);
    }
    ImGui::SeparatorText("Songs");
    auto drawSongRange = [](int32_t start, int32_t end) {
        for (int32_t i = start; i <= end; i++) {
            ImVec2 itemSize = DrawSong(static_cast<QuestItem>(i));
            if (i != end) {
                float currentCursorX = ImGui::GetCursorPosX();
                float nextItemMaxX = currentCursorX + ImGui::GetStyle().ItemSpacing.x + itemSize.x;
                float regionMaxX = ImGui::GetWindowContentRegionMax().x;
                if (nextItemMaxX <= regionMaxX) {
                    ImGui::SameLine();
                }
            }
        }
    };
    drawSongRange(QUEST_SONG_TIME, QUEST_SONG_SUN);
    drawSongRange(QUEST_SONG_SONATA, QUEST_SONG_SARIA);
    ImGui::SeparatorText("Equipment");
    if (GET_PLAYER_FORM == PLAYER_FORM_FIERCE_DEITY) {
        ImTextureID swordTextureId =
            std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                ->GetTextureByName((const char*)gItemIcons[ITEM_SWORD_DEITY]);
        ImGui::ImageButton(std::to_string(ITEM_SWORD_DEITY).c_str(), swordTextureId,
                           ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1),
                           ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
    } else {
        int swordValue = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD);
        if (swordValue == EQUIP_VALUE_SWORD_NONE) {
            swordValue = EQUIP_VALUE_SWORD_KOKIRI;
        }
        ImTextureID swordTextureId =
            std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                ->GetTextureByName((const char*)gItemIcons[ITEM_SWORD_KOKIRI + swordValue - EQUIP_VALUE_SWORD_KOKIRI]);

        if (ImGui::ImageButton(std::to_string(ITEM_SWORD_KOKIRI).c_str(), swordTextureId,
                               ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1),
                               ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) ? 1 : 0.4f))) {
            NextQuestInSlot(QUEST_SWORD);
        }
    }
    ImGui::SameLine();
    int shieldValue = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD);
    if (shieldValue == EQUIP_VALUE_SHIELD_NONE) {
        shieldValue = EQUIP_VALUE_SHIELD_HERO;
    }
    ImTextureID shieldTextureId =
        std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->GetTextureByName((const char*)gItemIcons[ITEM_SHIELD_HERO + shieldValue - EQUIP_VALUE_SHIELD_HERO]);

    if (ImGui::ImageButton(std::to_string(ITEM_SHIELD_HERO).c_str(), shieldTextureId,
                           ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1),
                           ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) ? 1 : 0.4f))) {
        NextQuestInSlot(QUEST_SHIELD);
    }
    ImGui::SameLine();
    ImTextureID textureId =
        std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->GetTextureByName((const char*)gItemIcons[ITEM_BOMBERS_NOTEBOOK]);
    if (ImGui::ImageButton(std::to_string(ITEM_BOMBERS_NOTEBOOK).c_str(), textureId,
                           ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1),
                           ImVec4(0, 0, 0, 0),
                           ImVec4(1, 1, 1, CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK) ? 1.0f : 0.4f))) {
        NextQuestInSlot(QUEST_BOMBERS_NOTEBOOK);
    }
    ImGui::SeparatorText("Heart Pieces");
    int32_t pohCount = (gSaveContext.save.saveInfo.inventory.questItems & 0xF0000000) >> 28;
    UIWidgets::PushStyleCombobox(UIWidgets::Colors::Red);
    if (ImGui::BeginCombo("##PoHcount", std::to_string(pohCount).c_str())) {
        for (int32_t i = 0; i < 4; i++) {
            if (ImGui::Selectable(std::to_string(i).c_str(), pohCount == i)) {
                gSaveContext.save.saveInfo.inventory.questItems &= ~0xF0000000;
                gSaveContext.save.saveInfo.inventory.questItems |= (i << 28);
            }
        }
        ImGui::EndCombo();
    }
    UIWidgets::PopStyleCombobox();
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("rightColumn", ImVec2(0, 0));
    ImGui::SeparatorText("Gold Skulltula Tokens");
    int OceanSkullTokens = Inventory_GetSkullTokenCount(SCENE_KINDAN2);
    int SwampSkullTokens = Inventory_GetSkullTokenCount(SCENE_KINSTA1);
    UIWidgets::PushStyleSlider();
    ImGui::PushItemWidth(ImGui::GetWindowWidth());
    if (ImGui::SliderInt("##swampSkulltulas", &SwampSkullTokens, 0, 30, "Swamp Tokens: %d")) {
        gSaveContext.save.saveInfo.skullTokenCount =
            ((int)(SwampSkullTokens & 0xFFFF) << 0x10) | (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF);
    }
    if (ImGui::SliderInt("##oceanSkulltulas", &OceanSkullTokens, 0, 30, "Ocean Tokens: %d")) {
        gSaveContext.save.saveInfo.skullTokenCount =
            (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF0000) | (OceanSkullTokens & 0xFFFF);
    }
    ImGui::PopItemWidth();
    UIWidgets::PopStyleSlider();

    ImGui::EndChild();
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

void SetDungeonItems(uint32_t dungeonItem, uint32_t dungeonId) {
    if (CHECK_DUNGEON_ITEM(dungeonItem, dungeonId)) {
        REMOVE_DUNGEON_ITEM(dungeonItem, dungeonId);
    } else {
        SET_DUNGEON_ITEM(dungeonItem, dungeonId);
    }
}

const char* dungeonNames[4] = { "Woodfall Temple", "Snowhead Temple", "Great Bay Temple", "Stone Tower Temple" };
uint32_t smallKeyCounts[4] = { 1, 3, 1, 4 };
const char* fairyIcons[] = { gDungeonStrayFairyWoodfallIconTex, gDungeonStrayFairySnowheadIconTex,
                             gDungeonStrayFairyGreatBayIconTex, gDungeonStrayFairyStoneTowerIconTex };
void DrawDungeonItemTab() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::BeginChild("dungeonTab", ImVec2(0, 0), true);

    for (int i = DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE; i <= DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE; i++) {
        std::string stray_id = "Stray" + std::to_string(i);
        std::string map_id = "Map" + std::to_string(i);
        std::string comp_id = "Compass" + std::to_string(i);
        std::string sKey_id = "Small Key" + std::to_string(i);
        std::string bKey_id = "Boss Key" + std::to_string(i);
        int dungeonId = i;
        ImGui::BeginChild(std::to_string(i).c_str(),
                          ImVec2(INV_GRID_WIDTH * 6 + INV_GRID_PADDING * 2, INV_GRID_HEIGHT * 1.5 + INV_GRID_PADDING),
                          ImGuiChildFlags_Border);
        ImGui::Text("%s", dungeonNames[i]);
        if (ImGui::ImageButton(
                stray_id.c_str(),
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName(fairyIcons[dungeonId]),
                ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                ImVec4(1, 1, 1, gSaveContext.save.saveInfo.inventory.strayFairies[dungeonId] ? 1.0f : 0.4f))) {
            ImGui::OpenPopup("strayFairies");
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(
                map_id.c_str(),
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName(gQuestIconDungeonMapTex),
                ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                ImVec4(1, 1, 1, CHECK_DUNGEON_ITEM(DUNGEON_MAP, i) ? 1.0f : 0.4f))) {
            SetDungeonItems(DUNGEON_MAP, i);
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(
                comp_id.c_str(),
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName(gQuestIconCompassTex),
                ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                ImVec4(1, 1, 1, CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, i) ? 1.0f : 0.4f))) {
            SetDungeonItems(DUNGEON_COMPASS, i);
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(
                sKey_id.c_str(),
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName(gQuestIconSmallKeyTex),
                ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                ImVec4(1, 1, 1, DUNGEON_KEY_COUNT(i) + 1 ? 1.0f : 0.4f))) {
            ImGui::OpenPopup("smallKeys");
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(
                bKey_id.c_str(),
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName(gQuestIconBossKeyTex),
                ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                ImVec4(1, 1, 1, CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, i) ? 1.0f : 0.4f))) {
            SetDungeonItems(DUNGEON_BOSS_KEY, i);
        }
        if (ImGui::BeginPopup("strayFairies")) {
            UIWidgets::PushStyleSlider(UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)));
            s32 minStray = 0;
            s32 maxStray = 15;
            int currentStrays = gSaveContext.save.saveInfo.inventory.strayFairies[dungeonId];
            ImGui::PushItemWidth(ImGui::GetFontSize() * 12.0f);
            if (ImGui::SliderScalar("##strayCount", ImGuiDataType_S32, &currentStrays, &minStray, &maxStray,
                                    "Strays: %d")) {
                gSaveContext.save.saveInfo.inventory.strayFairies[dungeonId] = currentStrays;
            }
            ImGui::PopItemWidth();
            UIWidgets::PopStyleSlider();
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("smallKeys")) {
            UIWidgets::PushStyleSlider(UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)));
            s32 minKey = -1;
            s32 maxKey = smallKeyCounts[dungeonId];
            int currentKeys = gSaveContext.save.saveInfo.inventory.dungeonKeys[dungeonId];
            ImGui::PushItemWidth(ImGui::GetFontSize() * 12.0f);
            if (ImGui::SliderScalar("##sKeyCount", ImGuiDataType_S32, &currentKeys, &minKey, &maxKey,
                                    "Small Keys: %d")) {
                gSaveContext.save.saveInfo.inventory.dungeonKeys[dungeonId] = currentKeys;
            }
            ImGui::PopItemWidth();
            UIWidgets::PopStyleSlider();
            ImGui::EndPopup();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(1);
}

void GetPlayerForm(uint32_t form) {
    switch (form) {
        case PLAYER_FORM_FIERCE_DEITY:
            curForm = "Fierce Deity";
            formColor = UIWidgets::Colors::Red;
            formObject = OBJECT_LINK_BOY;
            break;
        case PLAYER_FORM_GORON:
            curForm = "Goron";
            formColor = UIWidgets::Colors::Brown;
            formObject = OBJECT_LINK_GORON;
            break;
        case PLAYER_FORM_ZORA:
            curForm = "Zora";
            formColor = UIWidgets::Colors::LightBlue;
            formObject = OBJECT_LINK_ZORA;
            break;
        case PLAYER_FORM_DEKU:
            curForm = "Deku";
            formColor = UIWidgets::Colors::DarkGreen;
            formObject = OBJECT_LINK_NUTS;
            break;
        case PLAYER_FORM_HUMAN:
            curForm = "Human";
            formColor = UIWidgets::Colors::Green;
            formObject = OBJECT_LINK_CHILD;
            break;
        default:
            break;
    }
}

void ClearAllEquippedItems() {
    auto& buttonItems = gSaveContext.save.saveInfo.equips.buttonItems;
    auto& cButtonSlots = gSaveContext.save.saveInfo.equips.cButtonSlots;

    for (size_t form = 0; form < ARRAY_COUNT(buttonItems); form++) {
        for (size_t slot = 0; slot < ARRAY_COUNT(buttonItems[form]); slot++) {
            if (slot == EQUIP_SLOT_B) {
                // Goron/Deku/Zora/FD expect their innate B actions; leave untouched.
                // Form 0 is shared by FD and Human via CUR_FORM
                if (form != 0 || GET_PLAYER_FORM == PLAYER_FORM_FIERCE_DEITY) {
                    continue;
                }

                int swordValue = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD);
                if (swordValue == EQUIP_VALUE_SWORD_NONE) {
                    buttonItems[form][slot] = ITEM_NONE;
                } else {
                    buttonItems[form][slot] = ITEM_SWORD_KOKIRI + swordValue - EQUIP_VALUE_SWORD_KOKIRI;
                }
                continue;
            }

            // B slot stores item directly (sword); cButtonSlots only maps C-buttons to inventory slots
            buttonItems[form][slot] = ITEM_NONE;
            cButtonSlots[form][slot] = SLOT_NONE;
        }
    }

    auto& dpadItems = gSaveContext.save.shipSaveInfo.dpadEquips.dpadItems;
    auto& dpadSlots = gSaveContext.save.shipSaveInfo.dpadEquips.dpadSlots;

    for (size_t form = 0; form < ARRAY_COUNT(dpadItems); form++) {
        for (size_t slot = 0; slot < ARRAY_COUNT(dpadItems[form]); slot++) {
            dpadItems[form][slot] = ITEM_NONE;
            dpadSlots[form][slot] = SLOT_NONE;
        }
    }
}

void DrawPlayerTab() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("playerTab", ImVec2(0, 0), true);

    if (gPlayState) {
        Player* player = GET_PLAYER(gPlayState);

        UIWidgets::BeginCardLayout({ .columnsPerRow = 2, .minColumnWidth = 420.0f });

        UIWidgets::BeginCard("playerSpeed");
        ImGui::Text("Link's Speed");
        UIWidgets::PushStyleCombobox(formColor);
        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        ImGui::InputScalar("XZ Speed", ImGuiDataType_Float, &player->speedXZ);
        ImGui::InputScalar("Y Velocity", ImGuiDataType_Float, &player->actor.velocity.y);
        ImGui::InputScalar("Ledge Height", ImGuiDataType_Float, &player->yDistToLedge);
        ImGui::InputScalar("Invincibility Timer", ImGuiDataType_S16, &player->invincibilityTimer);
        ImGui::InputScalar("Gravity", ImGuiDataType_Float, &player->actor.gravity);
        ImGui::PopItemWidth();
        UIWidgets::PopStyleCombobox();
        UIWidgets::EndCard();

        UIWidgets::BeginCard("playerLocation");
        GetPlayerForm(GET_PLAYER_FORM);
        ImGui::Text("%s Link", curForm);
        ImGui::Text("Position:");
        UIWidgets::PushStyleCombobox(formColor);
        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        ImGui::InputScalar("X Pos", ImGuiDataType_Float, &player->actor.world.pos.x);
        ImGui::SameLine();
        ImGui::InputScalar("Y Pos", ImGuiDataType_Float, &player->actor.world.pos.y);
        ImGui::SameLine();
        ImGui::InputScalar("Z Pos", ImGuiDataType_Float, &player->actor.world.pos.z);
        ImGui::Text("Rotation:");
        ImGui::InputScalar("X Rot", ImGuiDataType_S16, &player->actor.world.rot.x);
        ImGui::SameLine();
        ImGui::InputScalar("Y Rot", ImGuiDataType_S16, &player->actor.world.rot.y);
        ImGui::SameLine();
        ImGui::InputScalar("Z Rot", ImGuiDataType_S16, &player->actor.world.rot.z);
        ImGui::PopItemWidth();
        UIWidgets::PopStyleCombobox();
        UIWidgets::EndCard();

        UIWidgets::BeginCard("playerForm");
        ImGui::Text("Change Link's Current Form");
        for (int i = PLAYER_FORM_FIERCE_DEITY; i <= PLAYER_FORM_HUMAN; i++) {
            GetPlayerForm(i);
            UIWidgets::PushStyleButton(formColor);
            if (ImGui::Button(curForm)) {
                s16 objectId = formObject;
                if (i != PLAYER_FORM_HUMAN) {
                    gSaveContext.save.equippedMask = PLAYER_MASK_FIERCE_DEITY + i;
                }
                gActorOverlayTable[ACTOR_PLAYER].profile->objectId = objectId;
                func_8012F73C(&gPlayState->objectCtx, player->actor.objectSlot, objectId);
                player->actor.objectSlot = Object_GetSlot(&gPlayState->objectCtx, GAMEPLAY_KEEP);
                gSaveContext.save.playerForm = i;
                s32 objectSlot =
                    Object_GetSlot(&gPlayState->objectCtx, gActorOverlayTable[ACTOR_PLAYER].profile->objectId);
                player->actor.objectSlot = objectSlot;
                player->actor.shape.rot.z = GET_PLAYER_FORM + 1;
                player->actor.init = PlayerCall_Init;
                player->actor.update = PlayerCall_Update;
                player->actor.draw = PlayerCall_Draw;
                gSaveContext.save.equippedMask = PLAYER_MASK_NONE;

                TransitionFade_SetColor(&gPlayState->unk_18E48, 0x000000);
                R_TRANS_FADE_FLASH_ALPHA_STEP = -1;
                Player_PlaySfx(GET_PLAYER(gPlayState), NA_SE_SY_TRANSFORM_MASK_FLASH);
            }
            UIWidgets::PopStyleButton();
            if (i != PLAYER_FORM_HUMAN) {
                ImGui::SameLine();
            }
        }

        ImGui::Separator();
        ImGui::Text("Link's Current Equipment");
        if (GET_PLAYER_FORM == PLAYER_FORM_HUMAN) {
            static const char* weaponCombo[] = { "Kokiri Sword", "Razor Sword", "Gilded Sword" };
            static const char* shieldCombo[] = { "Hero's Shield", "Mirror Shield" };
            static int currentWeapon = 0;
            static int currentShield = 0;

            UIWidgets::PushStyleCombobox(UIWidgets::Colors::Green);
            ImGui::PushItemWidth(ImGui::GetFontSize() * 15);
            if (ImGui::BeginCombo("Equipped Sword", weaponCombo[currentWeapon])) {
                for (int i = EQUIP_VALUE_SWORD_KOKIRI; i <= EQUIP_VALUE_SWORD_GILDED; i++) {
                    const bool isSelected = (currentWeapon == i);
                    if (ImGui::Selectable(weaponCombo[i - 1], isSelected)) {
                        currentWeapon = i - 1;
                        SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, i);
                        if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_RAZOR) {
                            gSaveContext.save.saveInfo.playerData.swordHealth = 100;
                        }
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) =
                            ITEM_SWORD_KOKIRI + GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) - EQUIP_VALUE_SWORD_KOKIRI;
                        Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_B);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            if (ImGui::BeginCombo("Equipped Shield", shieldCombo[currentShield])) {
                for (int i = EQUIP_VALUE_SHIELD_HERO; i <= EQUIP_VALUE_SHIELD_MIRROR; i++) {
                    const bool isSelected = (currentShield == i);
                    if (ImGui::Selectable(shieldCombo[i - 1], isSelected)) {
                        currentShield = i - 1;
                        SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, i);
                        Player_SetEquipmentData(gPlayState, player);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::PopItemWidth();
            UIWidgets::PopStyleCombobox();

        } else {
            ImGui::Separator();
            ImGui::Text("              No Equipment in this Form");
            ImGui::Separator();
        }
        UIWidgets::EndCard();

        UIWidgets::BeginCard("currentItems");
        ImGui::Text("Current Items");

        // Two-column layout
        ImGui::Columns(2, nullptr, false);

        // Left column: C-buttons
        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        UIWidgets::PushStyleInput();
        static u8 one = 1;
        ImGui::InputScalar("B Button", ImGuiDataType_U8, &CUR_FORM_EQUIP(0), &one, NULL);
        ImGui::InputScalar("C Left", ImGuiDataType_U8, &CUR_FORM_EQUIP(1), &one, NULL);
        ImGui::InputScalar("C Down", ImGuiDataType_U8, &CUR_FORM_EQUIP(2), &one, NULL);
        ImGui::InputScalar("C Right", ImGuiDataType_U8, &CUR_FORM_EQUIP(3), &one, NULL);
        UIWidgets::PopStyleInput();
        ImGui::PopItemWidth();

        // Right column: D-pad
        ImGui::NextColumn();
        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        UIWidgets::PushStyleInput();
        ImGui::InputScalar("D-Pad Right", ImGuiDataType_U8, &DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_RIGHT), &one,
                           NULL);
        ImGui::InputScalar("D-Pad Left", ImGuiDataType_U8, &DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_LEFT), &one, NULL);
        ImGui::InputScalar("D-Pad Down", ImGuiDataType_U8, &DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_DOWN), &one, NULL);
        ImGui::InputScalar("D-Pad Up", ImGuiDataType_U8, &DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_UP), &one, NULL);
        UIWidgets::PopStyleInput();
        ImGui::PopItemWidth();

        ImGui::Columns(1);

        // Clear All button
        ImGui::Spacing();
        if (UIWidgets::Button("Clear All", { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
            ClearAllEquippedItems();
        }

        UIWidgets::EndCard();

        UIWidgets::BeginCard("playerStates");
        ImGui::Text("Player States");
        uint32_t states[4] = { player->stateFlags1, player->stateFlags2, player->stateFlags3,
                               player->meleeWeaponState };

        if (ImGui::BeginTable("stateTable", 4)) {
            GetPlayerForm(GET_PLAYER_FORM);
            ImGui::PushStyleVar(ImGuiTableColumnFlags_WidthFixed, 15.0f);
            ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, formColor);
            ImGui::TableSetupColumn("State 1");
            ImGui::TableSetupColumn("State 2");
            ImGui::TableSetupColumn("State 3");
            ImGui::TableSetupColumn("Sword State");
            ImGui::TableHeadersRow();
            ImGui::TableNextColumn();

            for (int i = 0; i <= 3; i++) {
                ImGui::Text("%s", std::to_string(states[i]).c_str());
                ImGui::TableNextColumn();
            }

            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(1);
            ImGui::EndTable();
        }
        UIWidgets::EndCard();

        UIWidgets::EndCardLayout();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

const char* regGroupNames[] = {
    "REG  (0)",  "SREG (1)",  "OREG (2)",  "PREG (3)",  "QREG (4)",  "MREG (5)",  "YREG (6)",  "DREG (7)",
    "UREG (8)",  "IREG (9)",  "ZREG (10)", "CREG (11)", "NREG (12)", "KREG (13)", "XREG (14)", "cREG (15)",
    "sREG (16)", "iREG (17)", "WREG (18)", "AREG (19)", "VREG (20)", "HREG (21)", "GREG (22)", "mREG (23)",
    "nREG (24)", "BREG (25)", "dREG (26)", "kREG (27)", "bREG (28)",
};

// 2S2H Added columns to scene table: entranceSceneId, betterMapSelectIndex, humanName
#define DEFINE_SCENE(_name, enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, \
                     _entranceSceneId, _betterMapSelectIndex, humanName)                               \
    { enumValue, humanName },
#define DEFINE_SCENE_UNSET(_enumValue)

std::unordered_map<s16, const char*> sceneList = {
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

void DrawRegEditorTab() {
    UIWidgets::PushStyleSlider();
    ImGui::SliderScalar("Reg Group", ImGuiDataType_U8, &gRegEditor->regGroup, &S8_ZERO, &REG_GROUPS_MAX,
                        regGroupNames[gRegEditor->regGroup]);
    ImGui::SliderScalar("Reg Page", ImGuiDataType_U8, &gRegEditor->regPage, &S8_ZERO, &REG_PAGES_MAX, "%d");
    UIWidgets::PopStyleSlider();

    ImGui::BeginChild("regSliders", ImVec2(0, 0), ImGuiChildFlags_Border);

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnWidth(0, 100.0f);

    for (int i = 0; i < REG_PER_PAGE; i++) {
        ImGui::PushID(i);
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 4.0f);
        ImGui::Text("%02X (%d)", i + gRegEditor->regPage * REG_PER_PAGE, i + gRegEditor->regPage * REG_PER_PAGE);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UIWidgets::ColorValues.at(UIWidgets::Colors::Gray));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
        ImGui::InputScalar(
            "##regInput", ImGuiDataType_S16,
            &gRegEditor->data[i + gRegEditor->regPage * REG_PER_PAGE + gRegEditor->regGroup * REG_PER_PAGE * REG_PAGES],
            NULL, NULL, "%d");
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);
        ImGui::NextColumn();
        ImGui::PopItemWidth();
        ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::EndChild();
}

const char* flagEditorSections[] = { "currentSceneFlags", "weekEventReg",  "eventInf",
                                     "scenesVisible",     "owlActivation", "permanentSceneFlags",
                                     "cycleSceneFlags",   "randoInf",      "collectedHearts" };

void DrawFlagsTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("flagsBox", ImVec2(0, 0), true);
    static int selectedFlagSection = 0;
    UIWidgets::Combobox("Flag Type", &selectedFlagSection, flagEditorSections,
                        {
                            .alignment = UIWidgets::ComponentAlignment::Left,
                            .labelPosition = UIWidgets::LabelPosition::None,
                        });

    if (selectedFlagSection == WEEK_EVENT_REG) { // Draw color legend
        ImGui::SameLine();
        ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::Gray), ICON_FA_SQUARE "Persistent");
        ImGui::SameLine();
        ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::LightBlue), ICON_FA_SQUARE "Cycle");
        ImGui::SameLine();
        ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::Orange), ICON_FA_SQUARE "Scene/Other");
    }

    static int16_t selectedScene = 0;
    if (selectedFlagSection == PERMANENT_SCENE_FLAGS || selectedFlagSection == CYCLE_SCENE_FLAGS) {
        ImGui::SameLine();
        UIWidgets::ComboboxWithSearch("Scene", &selectedScene, &sceneList,
                                      {
                                          .alignment = UIWidgets::ComponentAlignment::Left,
                                          .labelPosition = UIWidgets::LabelPosition::None,
                                      });
        if (gPlayState != NULL) {
            ImGui::SameLine();
            if (UIWidgets::Button("Current", UIWidgets::ButtonOptions{ { .color = UIWidgets::Colors::Gray } }.Size(
                                                 UIWidgets::Sizes::Inline))) {
                selectedScene = gPlayState->sceneId;
            }
        }
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.0f, 1.0f));
    switch (selectedFlagSection) {
        case CURRENT_SCENE_FLAGS:
            if (gPlayState == NULL) {
                ImGui::Text("Play state is NULL, cannot display scene flags");
                break;
            }

            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("switches[0]");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##switches0",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.switches[0] = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##switches0",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.switches[0] = 0;
            }
            UIWidgets::DrawFlagArray32("##switches0", gPlayState->actorCtx.sceneFlags.switches[0]);
            ImGui::EndGroup();
            ImGui::SameLine(0, 10.0f);
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("switches[1]");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##switches1",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.switches[1] = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##switches1",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.switches[1] = 0;
            }
            UIWidgets::DrawFlagArray32("##switches1", gPlayState->actorCtx.sceneFlags.switches[1]);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("switches[2]");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##switches2",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.switches[2] = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##switches2",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.switches[2] = 0;
            }
            UIWidgets::DrawFlagArray32("##switches2", gPlayState->actorCtx.sceneFlags.switches[2]);
            ImGui::EndGroup();
            ImGui::SameLine(0, 10.0f);
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("switches[3]");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##switches3",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.switches[3] = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##switches3",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.switches[3] = 0;
            }
            UIWidgets::DrawFlagArray32("##switches3", gPlayState->actorCtx.sceneFlags.switches[3]);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("chest");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##chest",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.chest = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##chest",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.chest = 0;
            }
            UIWidgets::DrawFlagArray32("##chest", gPlayState->actorCtx.sceneFlags.chest);
            ImGui::EndGroup();
            ImGui::SameLine(0, 10.0f);
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("clearedRoom");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##clearedRoom",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.clearedRoom = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##clearedRoom",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.clearedRoom = 0;
            }
            UIWidgets::DrawFlagArray32("##clearedRoom", gPlayState->actorCtx.sceneFlags.clearedRoom);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("clearedRoomTemp");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##clearedRoomTemp",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.clearedRoomTemp = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##clearedRoomTemp",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.clearedRoomTemp = 0;
            }
            UIWidgets::DrawFlagArray32("##clearedRoomTemp", gPlayState->actorCtx.sceneFlags.clearedRoomTemp);
            ImGui::EndGroup();
            ImGui::SameLine(0, 10.0f);
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("collectible[0]");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##collectible0",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.collectible[0] = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##collectible0",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.collectible[0] = 0;
            }
            UIWidgets::DrawFlagArray32("##collectible0", gPlayState->actorCtx.sceneFlags.collectible[0]);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("collectible[1]");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##collectible1",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.collectible[1] = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##collectible1",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.collectible[1] = 0;
            }
            UIWidgets::DrawFlagArray32("##collectible1", gPlayState->actorCtx.sceneFlags.collectible[1]);
            ImGui::EndGroup();
            ImGui::SameLine(0, 10.0f);
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("collectible[2]");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##collectible2",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.collectible[2] = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##collectible2",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.collectible[2] = 0;
            }
            UIWidgets::DrawFlagArray32("##collectible2", gPlayState->actorCtx.sceneFlags.collectible[2]);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("collectible[3]");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##collectible3",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gPlayState->actorCtx.sceneFlags.collectible[3] = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##collectible3",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gPlayState->actorCtx.sceneFlags.collectible[3] = 0;
            }
            UIWidgets::DrawFlagArray32("##collectible3", gPlayState->actorCtx.sceneFlags.collectible[3]);
            ImGui::EndGroup();
            break;
        case WEEK_EVENT_REG:
            for (int i = 0; i < 100; i++) {
                ImGui::PushID(i);
                ImGui::Text("%02d", i);
                ImGui::SameLine();
                UIWidgets::DrawFlagTableArray8Mask(flagTables.at(WEEK_EVENT_REG), i,
                                                   gSaveContext.save.saveInfo.weekEventReg[i]);
                ImGui::PopID();
            }
            break;
        case EVENT_INF:
            for (int i = 0; i < 8; i++) {
                ImGui::PushID(i);
                ImGui::Text("%02d", i);
                ImGui::SameLine();
                UIWidgets::DrawFlagTableArray8(flagTables.at(EVENT_INF), i, gSaveContext.eventInf[i]);
                ImGui::PopID();
            }
            break;
        case SCENES_VISIBLE:
            if (UIWidgets::Button("All##scenesVisible",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                for (int i = 0; i < 7; i++) {
                    gSaveContext.save.saveInfo.scenesVisible[i] = UINT32_MAX;
                }
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##scenesVisible",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                for (int i = 0; i < 7; i++) {
                    gSaveContext.save.saveInfo.scenesVisible[i] = 0;
                }
            }
            for (int i = 0; i < 7; i++) {
                ImGui::PushID(i);
                UIWidgets::DrawFlagArray32("##", gSaveContext.save.saveInfo.scenesVisible[i]);
                ImGui::PopID();
            }
            break;
        case OWL_ACTIVATION:
            if (UIWidgets::Button("All##owlActivationFlags",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.save.saveInfo.playerData.owlActivationFlags = UINT16_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##owlActivationFlags",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.save.saveInfo.playerData.owlActivationFlags = 0;
            }
            UIWidgets::DrawFlagTableArray16(flagTables.at(OWL_ACTIVATION),
                                            gSaveContext.save.saveInfo.playerData.owlActivationFlags);
            break;
        case PERMANENT_SCENE_FLAGS:
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("chest");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##chest",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].chest = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##chest",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].chest = 0;
            }
            UIWidgets::DrawFlagArray32("##chest", gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].chest);
            ImGui::EndGroup();
            ImGui::SameLine(0, 10.0f);
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("switch0");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##switch0",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].switch0 = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##switch0",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].switch0 = 0;
            }
            UIWidgets::DrawFlagArray32("##switch0",
                                       gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].switch0);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("collectible");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##collectible",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].collectible = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##collectible",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].collectible = 0;
            }
            UIWidgets::DrawFlagArray32("##collectible",
                                       gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].collectible);
            ImGui::EndGroup();
            ImGui::SameLine(0, 10.0f);
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("switch1");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##switch1",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].switch1 = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##switch1",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].switch1 = 0;
            }
            UIWidgets::DrawFlagArray32("##switch1",
                                       gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].switch1);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("clearedRoom");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##clearedRoom",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].clearedRoom = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##clearedRoom",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].clearedRoom = 0;
            }
            UIWidgets::DrawFlagArray32("##clearedRoom",
                                       gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].clearedRoom);
            ImGui::EndGroup();
            ImGui::SameLine(0, 10.0f);
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("unk_14");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##unk_14",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].unk_14 = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##unk_14",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].unk_14 = 0;
            }
            UIWidgets::DrawFlagArray32("##unk_14",
                                       gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].unk_14);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("rooms");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##rooms",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].rooms = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##rooms",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].rooms = 0;
            }
            UIWidgets::DrawFlagArray32("##rooms", gSaveContext.save.saveInfo.permanentSceneFlags[selectedScene].rooms);
            ImGui::EndGroup();
            break;
        case CYCLE_SCENE_FLAGS:
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("chest");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##chest",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.cycleSceneFlags[selectedScene].chest = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##chest",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.cycleSceneFlags[selectedScene].chest = 0;
            }
            UIWidgets::DrawFlagArray32("##chest", gSaveContext.cycleSceneFlags[selectedScene].chest);
            ImGui::EndGroup();
            ImGui::SameLine(0, 10.0f);
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("switch0");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##switch0",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.cycleSceneFlags[selectedScene].switch0 = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##switch0",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.cycleSceneFlags[selectedScene].switch0 = 0;
            }
            UIWidgets::DrawFlagArray32("##switch0", gSaveContext.cycleSceneFlags[selectedScene].switch0);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("collectible");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##collectible",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.cycleSceneFlags[selectedScene].collectible = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##collectible",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.cycleSceneFlags[selectedScene].collectible = 0;
            }
            UIWidgets::DrawFlagArray32("##collectible", gSaveContext.cycleSceneFlags[selectedScene].collectible);
            ImGui::EndGroup();
            ImGui::SameLine(0, 10.0f);
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("switch1");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##switch1",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.cycleSceneFlags[selectedScene].switch1 = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##switch1",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.cycleSceneFlags[selectedScene].switch1 = 0;
            }
            UIWidgets::DrawFlagArray32("##switch1", gSaveContext.cycleSceneFlags[selectedScene].switch1);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("clearedRoom");
            ImGui::SameLine(110);
            if (UIWidgets::Button("All##clearedRoom",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Gray })) {
                gSaveContext.cycleSceneFlags[selectedScene].clearedRoom = UINT32_MAX;
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Clear##clearedRoom",
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
                gSaveContext.cycleSceneFlags[selectedScene].clearedRoom = 0;
            }
            UIWidgets::DrawFlagArray32("##clearedRoom", gSaveContext.cycleSceneFlags[selectedScene].clearedRoom);
            ImGui::EndGroup();
            break;
        case RANDO_INF:
            for (int i = 0; i < 5; i++) {
                ImGui::PushID(i);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%02d", i);
                ImGui::SameLine(ImGui::CalcTextSize("000").x + ImGui::GetStyle().ItemSpacing.x);
                UIWidgets::DrawFlagTableArray16(flagTables.at(RANDO_INF), i,
                                                gSaveContext.save.shipSaveInfo.rando.randoInf[i]);
                ImGui::PopID();
            }
            break;
        case HEART_FLAGS:
            if (gPlayState == NULL) {
                ImGui::Text("Play state is NULL, cannot display flags");
                break;
            }
            if (IS_RANDO) {
                ImGui::Text("Flags are not relevant to rando");
                break;
            }
            for (size_t i = 0; i < heartFlags.size(); ++i) {
                const auto& heartFlag = heartFlags[i];
                bool collected;
                uint16_t flag = heartFlag.flag;
                std::string label;

                if (gPlayState->sceneId == heartFlag.scene) {
                    switch (heartFlag.flagType) {
                        case WEEK_EVENT_REG:
                            collected = CHECK_WEEKEVENTREG(flag);
                            label = fmt::format("weekEventReg | {:02}_{:02X}", flag >> 8, flag & 0xFF);
                            break;
                        case FLAG_CYCL_SCENE_COLLECTIBLE:
                            collected = Flags_GetCollectible(gPlayState, flag);
                            label = fmt::format("currentSceneFlags | Collectible[0] | 0x{:02X}", flag);
                            break;
                        case FLAG_CYCL_SCENE_SWITCH:
                            collected = Flags_GetSwitch(gPlayState, flag);
                            label = fmt::format("currentSceneFlags | Switch[0] | 0x{:02X}", flag);
                            break;
                    }
                } else {
                    switch (heartFlag.flagType) {
                        case WEEK_EVENT_REG:
                            collected = CHECK_WEEKEVENTREG(flag);
                            label = fmt::format("weekEventReg | {:02}_{:02X}", flag >> 8, flag & 0xFF);
                            break;
                        case FLAG_CYCL_SCENE_COLLECTIBLE:
                            collected = gSaveContext.cycleSceneFlags[heartFlag.scene].collectible & (1 << flag);
                            label = fmt::format("cycleSceneFlags | {} | Collectible | 0x{:02X}",
                                                sceneList.at(heartFlag.scene), flag);
                            break;
                        case FLAG_CYCL_SCENE_SWITCH:
                            collected = gSaveContext.cycleSceneFlags[heartFlag.scene].switch0 & (1 << flag);
                            label = fmt::format("cycleSceneFlags | {} | Switch0 | 0x{:02X}",
                                                sceneList.at(heartFlag.scene), flag);
                            break;
                    }
                }

                ImGui::PushID(i);
                ImGui::BeginDisabled();
                ImGui::Checkbox(heartFlag.description.c_str(), &collected);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip("%s", label.c_str());
                }
                ImGui::EndDisabled();
                ImGui::PopID();
            }
            break;
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

void DrawRandoTab() {
    if (UIWidgets::Button("Generate Spoiler from Save", { .size = UIWidgets::Sizes::Inline })) {
        nlohmann::json spoiler = Rando::Spoiler::GenerateFromSaveContext();
        std::string inputSeed = std::to_string(Ship_Random(0, 1000000));
        spoiler["inputSeed"] = inputSeed;

        std::string fileName = inputSeed + ".json";
        Rando::Spoiler::SaveToFile(fileName, spoiler);
    }

    static ImGuiTextFilter rcFilter;
    UIWidgets::PushStyleCombobox();
    rcFilter.Draw("##filter", ImGui::GetContentRegionAvail().x);
    UIWidgets::PopStyleCombobox();
    if (!rcFilter.IsActive()) {
        ImGui::SameLine(18.0f);
        ImGui::Text("Search");
    }

    ImGui::BeginChild("RandoChild");
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

    ImGui::BeginTable("Check List", 5);
    ImGui::TableSetupColumn("Shuffled", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed, 30.0f);
    ImGui::TableSetupColumn("Eligible", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed, 30.0f);
    ImGui::TableSetupColumn("Obtained", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed, 30.0f);
    ImGui::TableSetupColumn("Check Name");
    ImGui::TableSetupColumn("Reward");
    ImGui::TableSetupScrollFreeze(5, 1);
    ImGui::TableHeadersRow();

    for (auto& [_, randoStaticCheck] : Rando::StaticData::Checks) {
        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

        if (!rcFilter.PassFilter(randoStaticCheck.name) &&
            !rcFilter.PassFilter(Rando::StaticData::Items[randoSaveCheck.randoItemId].spoilerName)) {
            continue;
        }

        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            continue;
        }
        std::string hiddenName = "##";
        hiddenName += randoStaticCheck.name;
        ImGui::TableNextColumn();
        UIWidgets::Checkbox((hiddenName + "shuffled").c_str(), &randoSaveCheck.shuffled);
        UIWidgets::Tooltip("Shuffled");
        ImGui::TableNextColumn();
        UIWidgets::Checkbox((hiddenName + "eligible").c_str(), &randoSaveCheck.eligible);
        UIWidgets::Tooltip("Eligible");
        ImGui::TableNextColumn();
        UIWidgets::Checkbox((hiddenName + "obtained").c_str(), &randoSaveCheck.obtained);
        UIWidgets::Tooltip("Obtained");
        ImGui::TableNextColumn();
        ImGui::TextColored(randoSaveCheck.obtained ? UIWidgets::ColorValues.at(UIWidgets::Colors::Green)
                                                   : UIWidgets::ColorValues.at(UIWidgets::Colors::White),
                           "%s", randoStaticCheck.name);
        ImGui::TableNextColumn();
        UIWidgets::ComboboxWithSearch((hiddenName + "reward").c_str(), &randoSaveCheck.randoItemId,
                                      &randoItemIdComboboxMap, { .labelPosition = UIWidgets::LabelPosition::None });
    }

    ImGui::EndTable();
    ImGui::PopStyleColor(3);
    ImGui::EndChild();
}

void SaveEditorWindow::DrawElement() {
    UIWidgets::PushStyleTabs(UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)));
    if (ImGui::BeginTabBar("SaveContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("General")) {
            DrawGeneralTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Items & Masks")) {
            DrawItemsAndMasksTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Quest Status")) {
            DrawQuestStatusTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Dungeon Items")) {
            DrawDungeonItemTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Reg Editor")) {
            DrawRegEditorTab();
            ImGui::EndTabItem();
        }

        // if (ImGui::BeginTabItem("Equipment")) {
        //     DrawEquipmentTab();
        //     ImGui::EndTabItem();
        // }

        if (ImGui::BeginTabItem("Flags")) {
            DrawFlagsTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Player")) {
            DrawPlayerTab();
            ImGui::EndTabItem();
        }

        if (IS_RANDO) {
            if (ImGui::BeginTabItem("Rando")) {
                DrawRandoTab();
                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }
    UIWidgets::PopStyleTabs();
}

void SaveEditorWindow::InitElement() {
    initSafeItemsForInventorySlot();
    randoItemIdComboboxMap.clear();
    for (auto& [_, randoItem] : Rando::StaticData::Items) {
        randoItemIdComboboxMap[randoItem.randoItemId] = randoItem.spoilerName;
    }
}
