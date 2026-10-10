#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"

void EnSyatekiMan_Swamp_RunGame(EnSyatekiMan* enSyatekiMan, PlayState* play);
void EnSyatekiMan_Town_RunGame(EnSyatekiMan* enSyatekiMan, PlayState* play);
}

#define SWAMP_CVAR_NAME "gEnhancements.Minigames.SwampArcheryScore"
#define SWAMP_CVAR CVarGetInteger(SWAMP_CVAR_NAME, 2180)
#define TOWN_CVAR_NAME "gEnhancements.Minigames.TownArcheryScore"
#define TOWN_CVAR CVarGetInteger(TOWN_CVAR_NAME, 50)
#define BOAT_CVAR_NAME "gEnhancements.Minigames.BoatArcheryScore"
#define BOAT_CVAR CVarGetInteger(BOAT_CVAR_NAME, 20)
#define BOAT_HEALTH_CVAR_NAME "gEnhancements.Minigames.BoatArcheryHealth"
#define BOAT_HEALTH_CVAR CVarGetInteger(BOAT_HEALTH_CVAR_NAME, 10)
#define BOAT_NO_DAMAGE_CVAR_NAME "gEnhancements.Minigames.BoatArcheryInvincible"
#define BOAT_NO_DAMAGE_CVAR CVarGetInteger(BOAT_NO_DAMAGE_CVAR_NAME, 0)
#define SKIP_MISC_CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define SKIP_MISC_CVAR CVarGetInteger(SKIP_MISC_CVAR_NAME, 0)

static constexpr u16 TEXT_BOAT_ARCHERY_FAIL = 0x877;

static void RegisterSwampArchery() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_SYATEKI_MAN, SWAMP_CVAR != 2180, [](Actor* actor, bool* should) {
        EnSyatekiMan* enSyatekiMan = (EnSyatekiMan*)actor;

        if (enSyatekiMan->actionFunc == EnSyatekiMan_Swamp_RunGame) {
            // This checks if their current score plus the amount of bonus points they would get from the timer is
            // greater than or equal to the score required to win
            if (enSyatekiMan->score != 0 &&
                (enSyatekiMan->score + (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] / 10)) >= SWAMP_CVAR) {
                enSyatekiMan->score = 2120;
                enSyatekiMan->currentWave = 4;
                enSyatekiMan->wolfosFlags = 0;
                enSyatekiMan->bonusDekuScrubHitCounter = 2;
            }
        }
    });

    COND_VB_SHOULD(VB_ARCHERY_ADD_BONUS_POINTS, SKIP_MISC_CVAR, {
        Actor* actor = va_arg(args, Actor*);
        s32* sBonusTimer = va_arg(args, s32*);

        *sBonusTimer = 11;
        *should = true;
    });
}

static void RegisterTownArchery() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_SYATEKI_MAN, TOWN_CVAR != 50, [](Actor* actor, bool* should) {
        EnSyatekiMan* enSyatekiMan = (EnSyatekiMan*)actor;

        if (enSyatekiMan->actionFunc == EnSyatekiMan_Town_RunGame) {
            if (enSyatekiMan->score >= TOWN_CVAR) {
                enSyatekiMan->score = 50;
                gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] = 0;
            }
        }
    });
}

static void ResetBoatArcheryScore(UNUSED Actor* actor) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_25_20)) {
        HS_SET_BOAT_ARCHERY_HIGH_SCORE(BOAT_CVAR - 1);
    }
}

static void RegisterBoatArchery() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_DNH, BOAT_CVAR != 20, ResetBoatArcheryScore);
}

static bool ShouldFailBoatArchery() {
    return gSaveContext.minigameHiddenScore >= BOAT_HEALTH_CVAR;
}

static void ModifyHitCount(u16* textId, bool* loadFromMessageTable) {
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    u16 hits = gSaveContext.minigameHiddenScore;
    CustomMessage::Replace(&entry.msg, "10 times", std::to_string(hits) + " time" + (hits == 1 ? "" : "s"));
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

static void RegisterKoumeHealth() {
    bool isHealthChanged = BOAT_HEALTH_CVAR != 10;
    COND_VB_SHOULD(VB_FAIL_BOAT_ARCHERY, isHealthChanged, { *should = ShouldFailBoatArchery(); });
    COND_ID_HOOK(OnOpenText, TEXT_BOAT_ARCHERY_FAIL, isHealthChanged, ModifyHitCount);
}

static void RegisterKoumeInvincible() {
    COND_VB_SHOULD(VB_KOUME_TAKE_DAMAGE, BOAT_NO_DAMAGE_CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc_Swamp(RegisterSwampArchery, { SWAMP_CVAR_NAME });
static RegisterShipInitFunc initFunc_Town(RegisterTownArchery, { TOWN_CVAR_NAME });

static RegisterShipInitFunc initFunc_Boat(RegisterBoatArchery, { BOAT_CVAR_NAME });
static RegisterShipInitFunc initFunc_Health(RegisterKoumeHealth, { BOAT_HEALTH_CVAR_NAME });
static RegisterShipInitFunc initFunc_Invincible(RegisterKoumeInvincible, { BOAT_NO_DAMAGE_CVAR_NAME });
