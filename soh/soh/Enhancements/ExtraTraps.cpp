#include "libultraship/bridge.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
GetItemEntry ItemTable_RetrieveEntry(s16 modIndex, s16 getItemID);
}

#define CVAR_EXTRA_TRAPS_NAME CVAR_ENHANCEMENT("ExtraTraps.Enabled")
#define CVAR_EXTRA_TRAPS_DEFAULT 0
#define CVAR_EXTRA_TRAPS_VALUE CVarGetInteger(CVAR_EXTRA_TRAPS_NAME, CVAR_EXTRA_TRAPS_DEFAULT)

typedef enum {
    ADD_ICE_TRAP,
    ADD_BURN_TRAP,
    ADD_SHOCK_TRAP,
    ADD_KNOCK_TRAP,
    ADD_SPEED_TRAP,
    ADD_BOMB_TRAP,
    ADD_VOID_TRAP,
    ADD_AMMO_TRAP,
    ADD_KILL_TRAP,
    ADD_TELEPORT_TRAP,
    ADD_TRAP_MAX
} AltTrapType;

static AltTrapType roll = ADD_TRAP_MAX;
static int statusTimer = -1;
static int eventTimer = -1;

const char* altTrapTypeCvars[] = {
    CVAR_ENHANCEMENT("ExtraTraps.Ice"),   CVAR_ENHANCEMENT("ExtraTraps.Burn"),
    CVAR_ENHANCEMENT("ExtraTraps.Shock"), CVAR_ENHANCEMENT("ExtraTraps.Knockback"),
    CVAR_ENHANCEMENT("ExtraTraps.Speed"), CVAR_ENHANCEMENT("ExtraTraps.Bomb"),
    CVAR_ENHANCEMENT("ExtraTraps.Void"),  CVAR_ENHANCEMENT("ExtraTraps.Ammo"),
    CVAR_ENHANCEMENT("ExtraTraps.Kill"),  CVAR_ENHANCEMENT("ExtraTraps.Teleport"),
};

std::vector<AltTrapType> getEnabledAddTraps() {
    std::vector<AltTrapType> enabledAddTraps;
    for (int i = 0; i < ADD_TRAP_MAX; i++) {
        if (CVarGetInteger(altTrapTypeCvars[i], 0)) {
            if (gSaveContext.equips.buttonItems[0] == ITEM_FISHING_POLE &&
                (i == ADD_VOID_TRAP || i == ADD_TELEPORT_TRAP)) {
                continue; // don't add void or teleport if you're holding the fishing pole, as this causes issues
            }
            enabledAddTraps.push_back(static_cast<AltTrapType>(i));
        }
    }
    if (enabledAddTraps.size() == 0) {
        enabledAddTraps.push_back(ADD_ICE_TRAP);
    }
    return enabledAddTraps;
};

static void RollRandomTrap(uint32_t seed) {
    uint32_t finalSeed = seed + (IS_RANDO ? Rando::Context::GetInstance()->GetSeed()
                                          : static_cast<uint32_t>(gSaveContext.ship.stats.fileCreatedAt));
    Random_Init(finalSeed);

    roll = RandomElement(getEnabledAddTraps());
    switch (roll) {
        case ADD_ICE_TRAP:
            GameInteractor::RawAction::FreezePlayer();
            break;
        case ADD_BURN_TRAP:
            GameInteractor::RawAction::BurnPlayer();
            break;
        case ADD_SHOCK_TRAP:
            GameInteractor::RawAction::ElectrocutePlayer();
            break;
        case ADD_KNOCK_TRAP:
            eventTimer = 3;
            break;
        case ADD_SPEED_TRAP:
            Audio_PlaySoundGeneral(NA_SE_VO_KZ_MOVE, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            GameInteractor::State::RunSpeedModifier = -2;
            statusTimer = 200;
            Notification::Emit({ .message = "Speed Decreased!" });
            break;
        case ADD_BOMB_TRAP:
            eventTimer = 3;
            break;
        case ADD_VOID_TRAP:
            Audio_PlaySoundGeneral(NA_SE_EN_GANON_LAUGH, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            eventTimer = 3;
            break;
        case ADD_AMMO_TRAP:
            eventTimer = 3;
            Notification::Emit({ .message = "Ammo Halved!" });
            break;
        case ADD_KILL_TRAP:
            GameInteractor::RawAction::SetPlayerHealth(0);
            break;
        case ADD_TELEPORT_TRAP:
            eventTimer = 3;
            break;
        default:
            break;
    }
}

static void OnPlayerUpdate() {
    Player* player = GET_PLAYER(gPlayState);
    if (statusTimer == 0) {
        GameInteractor::State::RunSpeedModifier = 0;
    }
    if (eventTimer == 0) {
        switch (roll) {
            case ADD_KNOCK_TRAP:
                GameInteractor::RawAction::KnockbackPlayer(1);
                break;
            case ADD_BOMB_TRAP:
                GameInteractor::RawAction::SpawnActor(ACTOR_EN_BOM, 1);
                break;
            case ADD_VOID_TRAP:
                Play_TriggerRespawn(gPlayState);
                break;
            case ADD_AMMO_TRAP:
                AMMO(ITEM_STICK) = static_cast<int8_t>(floor(AMMO(ITEM_STICK) * 0.5f));
                AMMO(ITEM_NUT) = static_cast<int8_t>(floor(AMMO(ITEM_NUT) * 0.5f));
                AMMO(ITEM_SLINGSHOT) = static_cast<int8_t>(floor(AMMO(ITEM_SLINGSHOT) * 0.5f));
                AMMO(ITEM_BOW) = static_cast<int8_t>(floor(AMMO(ITEM_BOW) * 0.5f));
                AMMO(ITEM_BOMB) = static_cast<int8_t>(floor(AMMO(ITEM_BOMB) * 0.5f));
                AMMO(ITEM_BOMBCHU) = static_cast<int8_t>(floor(AMMO(ITEM_BOMBCHU) * 0.5f));
                Audio_PlaySoundGeneral(NA_SE_VO_FR_SMILE_0, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                break;
            case ADD_TELEPORT_TRAP: {
                int entrance;
                int index = Random(0, 7);
                switch (index) {
                    case 0:
                        entrance = GI_TP_DEST_SERENADE;
                        break;
                    case 1:
                        entrance = GI_TP_DEST_REQUIEM;
                        break;
                    case 2:
                        entrance = GI_TP_DEST_BOLERO;
                        break;
                    case 3:
                        entrance = GI_TP_DEST_MINUET;
                        break;
                    case 4:
                        entrance = GI_TP_DEST_NOCTURNE;
                        break;
                    case 5:
                        entrance = GI_TP_DEST_PRELUDE;
                        break;
                    default:
                        entrance = GI_TP_DEST_LINKSHOUSE;
                        break;
                }
                GameInteractor::RawAction::TeleportPlayer(entrance);
                break;
            }
            default:
                break;
        }
    }
    if (statusTimer >= 0) {
        statusTimer--;
    }
    if (eventTimer >= 0) {
        eventTimer--;
    }
}

void RegisterExtraTraps() {
    COND_HOOK(OnPlayerUpdate, CVAR_EXTRA_TRAPS_VALUE, OnPlayerUpdate);

    COND_VB_SHOULD(VB_SHORT_CIRCUIT_GIVE_ITEM_PROCESS, true, {
        if (!gSaveContext.ship.pendingIceTrapCount) {
            return;
        }

        Player* player = GET_PLAYER(gPlayState);

        *should = true;
        gSaveContext.ship.pendingIceTrapCount--;
        gSaveContext.ship.stats.count[COUNT_ICE_TRAPS]++;
        GameInteractor_ExecuteOnItemReceiveHooks(ItemTable_RetrieveEntry(MOD_RANDOMIZER, RG_ICE_TRAP));
        if (CVAR_EXTRA_TRAPS_VALUE) {
            RollRandomTrap(gPlayState->sceneNum + player->getItemEntry.drawItemId);
        } else {
            GameInteractor::RawAction::FreezePlayer();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterExtraTraps, { CVAR_EXTRA_TRAPS_NAME });
