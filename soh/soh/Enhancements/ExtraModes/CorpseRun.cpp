#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/draw.h"
#include "soh/SaveManager.h"
#include "soh/ShipInit.hpp"
#include "z64item.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"

extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_CORPSE_RUN_DEFAULT = 0;
#define CVAR_CORPSE_RUN_NAME CVAR_ENHANCEMENT("CorpseRun")
#define CVAR_CORPSE_RUN_VALUE CVarGetInteger(CVAR_CORPSE_RUN_NAME, CVAR_CORPSE_RUN_DEFAULT)

namespace CorpseRun {

static constexpr int32_t INVALID_SAVE_SECTION_ID = -1;
static constexpr int32_t VERSION = 1;

struct Remnant {
    // State
    bool dropped = false;
    bool active = false;

    int16_t sceneNum = -1;
    int8_t roomNum = -1;

    Vec3f pos = {};
    int16_t yaw = 0;

    // Resources
    int32_t rupees = 0;
    int32_t magic = 0;

    // Consumables
    int32_t bombs = 0;
    int32_t arrows = 0;
    int32_t sticks = 0;
    int32_t nuts = 0;
    int32_t seeds = 0;
    int32_t bombchus = 0;

    // TODO(jperos): Progression items and/or Equipment modes?

    void Clear() {
        *this = {};
    }
    bool HasContents() const;
    bool IsInRoom() const;
    bool IsRecoverable() const {
        return dropped && active;
    }
    bool IsPendingActivation() const {
        return dropped && !active;
    }
};

static constexpr struct ConsumableDesc {
    const char* name;
    int32_t Remnant::*field;
    int16_t itemId;
} CONSUMABLE_TABLE[] = {
    // clang-format off
    { "bombs",    &Remnant::bombs,    ITEM_BOMB      },
    { "arrows",   &Remnant::arrows,   ITEM_BOW       },
    { "sticks",   &Remnant::sticks,   ITEM_STICK     },
    { "nuts",     &Remnant::nuts,     ITEM_NUT       },
    { "seeds",    &Remnant::seeds,    ITEM_SLINGSHOT },
    { "bombchus", &Remnant::bombchus, ITEM_BOMBCHU   }
    // clang-format on
};

static Remnant sRemnant;
static GetItemEntry sMysteryItem = GET_ITEM_MYSTERY;

// Remnant member helpers
bool Remnant::HasContents() const {
    if (rupees > 0)
        return true;
    if (magic > 0)
        return true;
    for (const ConsumableDesc& desc : CONSUMABLE_TABLE) {
        if (this->*desc.field > 0)
            return true;
    }
    return false;
}

bool Remnant::IsInRoom() const {
    return gPlayState != nullptr && gPlayState->sceneNum == sceneNum && gPlayState->roomCtx.curRoom.num == roomNum;
}

// Save/Load
static void InitSave(bool isDebug) {
    sRemnant.Clear();
}

static void SaveFile() {
    SaveManager::Instance->SaveFile(gSaveContext.fileNum);
}

static void LoadSave() {
    sRemnant.Clear();

    SaveManager::Instance->LoadData("dropped", sRemnant.dropped, false);

    if (!sRemnant.dropped) {
        return;
    }

    // Loaded remnants should always be recoverable.
    sRemnant.active = true;

    SaveManager::Instance->LoadData("sceneNum", sRemnant.sceneNum, (int16_t)-1);
    SaveManager::Instance->LoadData("roomNum", sRemnant.roomNum, (int8_t)-1);

    SaveManager::Instance->LoadStruct("pos", []() {
        SaveManager::Instance->LoadData("x", sRemnant.pos.x, 0.0f);
        SaveManager::Instance->LoadData("y", sRemnant.pos.y, 0.0f);
        SaveManager::Instance->LoadData("z", sRemnant.pos.z, 0.0f);
    });

    SaveManager::Instance->LoadData("yaw", sRemnant.yaw, (int16_t)0);

    SaveManager::Instance->LoadStruct("contents", []() {
        SaveManager::Instance->LoadData("rupees", sRemnant.rupees, 0);
        SaveManager::Instance->LoadData("magic", sRemnant.magic, 0);
        for (const ConsumableDesc& desc : CONSUMABLE_TABLE) {
            SaveManager::Instance->LoadData(desc.name, sRemnant.*desc.field, 0);
        }
    });
}

static void Save(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveManager::Instance->SaveData("dropped", sRemnant.dropped);

    if (!sRemnant.dropped) {
        return;
    }

    SaveManager::Instance->SaveData("sceneNum", sRemnant.sceneNum);
    SaveManager::Instance->SaveData("roomNum", sRemnant.roomNum);

    SaveManager::Instance->SaveStruct("pos", []() {
        SaveManager::Instance->SaveData("x", sRemnant.pos.x);
        SaveManager::Instance->SaveData("y", sRemnant.pos.y);
        SaveManager::Instance->SaveData("z", sRemnant.pos.z);
    });

    SaveManager::Instance->SaveData("yaw", sRemnant.yaw);

    SaveManager::Instance->SaveStruct("contents", []() {
        SaveManager::Instance->SaveData("rupees", sRemnant.rupees);
        SaveManager::Instance->SaveData("magic", sRemnant.magic);
        for (const ConsumableDesc& desc : CONSUMABLE_TABLE) {
            SaveManager::Instance->SaveData(desc.name, sRemnant.*desc.field);
        }
    });
}

static void RegisterSave() {
    static int32_t sSaveSectionId = CorpseRun::INVALID_SAVE_SECTION_ID;

    // Don't register twice
    if (sSaveSectionId != CorpseRun::INVALID_SAVE_SECTION_ID) {
        return;
    }

    SaveManager::Instance->AddInitFunction(InitSave);
    SaveManager::Instance->AddLoadFunction("corpseRun", CorpseRun::VERSION, LoadSave);
    sSaveSectionId =
        SaveManager::Instance->AddSaveFunction("corpseRun", CorpseRun::VERSION, Save, true, SECTION_PARENT_NONE);
}

// Corpse Run Logic
static void Activate(Player* player, PlayState* playState, int32_t respawnFlag) {
    if (!sRemnant.dropped) {
        return;
    }

    sRemnant.active = true;
}

static void Drop() {
    // Safety in case death hook fires more than once during the same death flow
    // If a remnant has been dropped but not activated by Player_Init yet, this is probably the same death
    if (sRemnant.IsPendingActivation()) {
        return;
    }

    if (gPlayState == nullptr) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return;
    }

    // Destroy previous remnant
    sRemnant.Clear();

    sRemnant.dropped = true;
    sRemnant.active = false;
    sRemnant.sceneNum = gPlayState->sceneNum;
    sRemnant.roomNum = gPlayState->roomCtx.curRoom.num;
    sRemnant.pos = player->actor.world.pos;
    sRemnant.yaw = player->actor.shape.rot.y;

    // v1 - consumables only
    sRemnant.rupees = gSaveContext.rupees;
    sRemnant.magic = gSaveContext.magic;
    for (const ConsumableDesc& desc : CONSUMABLE_TABLE) {
        sRemnant.*desc.field = AMMO(desc.itemId);
    }

    if (!sRemnant.HasContents()) {
        // Dark Souls behavior: dying again still destroys the previous remnant,
        // even if the new death has nothing worth dropping
        sRemnant.Clear();
        SaveFile();
        return;
    }

    gSaveContext.rupees = 0;
    Magic_Reset(gPlayState);
    for (const ConsumableDesc& desc : CONSUMABLE_TABLE) {
        AMMO(desc.itemId) = 0;
    }

    SaveFile();
}

static void Recover() {
    Rupees_ChangeBy(sRemnant.rupees);
    Magic_RequestChange(gPlayState, sRemnant.magic, MAGIC_ADD);
    for (const ConsumableDesc& desc : CONSUMABLE_TABLE) {
        Inventory_ChangeAmmo(desc.itemId, sRemnant.*desc.field);
    }

    sRemnant.Clear();

    SaveFile();
}

static void Update() {
    if (!sRemnant.IsInRoom() || !sRemnant.IsRecoverable()) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return;
    }

    const float dx = player->actor.world.pos.x - sRemnant.pos.x;
    const float dy = player->actor.world.pos.y - sRemnant.pos.y;
    const float dz = player->actor.world.pos.z - sRemnant.pos.z;
    const float distSq = dx * dx + dy * dy + dz * dz;

    if (distSq < SQ(80.0f)) {
        Recover();
    }
}

static void Draw() {
    if (!sRemnant.IsInRoom() || !sRemnant.IsRecoverable()) {
        return;
    }

    Matrix_Push();

    // Cheap bob/spin so it reads as collectible-ish.
    const s16 bobAngle = static_cast<s16>(gPlayState->gameplayFrames * 0x400);
    const s16 spinAngle = static_cast<s16>(gPlayState->gameplayFrames * 0x200);

    const f32 bob = Math_SinS(bobAngle) * 8.0f;

    Matrix_Translate(sRemnant.pos.x, sRemnant.pos.y + 40.0f + bob, sRemnant.pos.z, MTXMODE_NEW);
    Matrix_RotateY(static_cast<f32>(BINANG_TO_RAD(spinAngle)), MTXMODE_APPLY);
    Matrix_Scale(0.35f, 0.35f, 0.35f, MTXMODE_APPLY);

    Randomizer_DrawMysteryItem(gPlayState, &sMysteryItem);

    Matrix_Pop();
}

static void Register() {
    RegisterSave();

    COND_HOOK(OnPlayerInit, CVAR_CORPSE_RUN_VALUE, Activate);
    COND_HOOK(OnPlayerDeath, CVAR_CORPSE_RUN_VALUE, Drop);
    COND_HOOK(OnGameFrameUpdate, CVAR_CORPSE_RUN_VALUE, Update);
    COND_HOOK(OnPlayDrawEnd, CVAR_CORPSE_RUN_VALUE, Draw);
}
} // namespace CorpseRun

static RegisterShipInitFunc initFunc(CorpseRun::Register, { CVAR_CORPSE_RUN_NAME });
