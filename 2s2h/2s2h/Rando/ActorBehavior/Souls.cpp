#include "ActorBehavior.h"
#include "Souls.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "Rando/DrawFuncs.h"
#include "Rando/Logic/Logic.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "variables.h"
#include "functions.h"

#include "overlays/actors/ovl_Boss_Hakugin/z_boss_hakugin.h"
#include "overlays/actors/ovl_En_Invadepoh/z_en_invadepoh.h"

void BossHakugin_DrawIce(BossHakugin*, PlayState*);
extern PlayState* gPlayState;
}

bool shouldMajoraRegister() {
    bool registerStatus = false;
    if (IS_RANDO) {
        if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_SOULS] == RO_GENERIC_YES ||
            RANDO_SAVE_OPTIONS[RO_SHUFFLE_TRIFORCE_PIECES] == RO_GENERIC_YES) {
            registerStatus = true;
        }
    }
    return registerStatus;
}

// clang-format off
std::unordered_map<int16_t, RandoItemId> enemySoulMap = {
    // Real Bombchu and Flying Pot souls are excluded, as those actors are programmed to die to any collision, not just
    // the damaging type. We don't have a good answer for what behavior those should follow if the hit something.
    { ACTOR_EN_INVADEPOH,   RI_SOUL_ENEMY_ALIEN },
    { ACTOR_EN_AM,          RI_SOUL_ENEMY_ARMOS },
    { ACTOR_EN_BAT,         RI_SOUL_ENEMY_BAD_BAT },
    { ACTOR_EN_VM,          RI_SOUL_ENEMY_BEAMOS },
    { ACTOR_EN_BB,          RI_SOUL_ENEMY_BUBBLE },
    { ACTOR_EN_BBFALL,      RI_SOUL_ENEMY_BUBBLE },
    { ACTOR_EN_MKK,         RI_SOUL_ENEMY_BOE },
    { ACTOR_EN_BSB,         RI_SOUL_ENEMY_CAPTAIN_KEETA },
    { ACTOR_EN_SLIME,       RI_SOUL_ENEMY_CHUCHU },
    { ACTOR_EN_FAMOS,       RI_SOUL_ENEMY_DEATH_ARMOS },
    { ACTOR_EN_DRAGON,      RI_SOUL_ENEMY_DEEP_PYTHON },
    { ACTOR_EN_DEKUBABA,    RI_SOUL_ENEMY_DEKU_BABA },
    { ACTOR_EN_KAREBABA,    RI_SOUL_ENEMY_DEKU_BABA },
    { ACTOR_BOSS_05,        RI_SOUL_ENEMY_DEKU_BABA },
    { ACTOR_EN_WDHAND,      RI_SOUL_ENEMY_DEXIHAND },
    { ACTOR_EN_DINOFOS,     RI_SOUL_ENEMY_DINOLFOS },
    { ACTOR_EN_DODONGO,     RI_SOUL_ENEMY_DODONGO },
    { ACTOR_EN_GRASSHOPPER, RI_SOUL_ENEMY_DRAGONFLY },
    { ACTOR_EN_SNOWMAN,     RI_SOUL_ENEMY_EENO },
    { ACTOR_EN_EGOL,        RI_SOUL_ENEMY_EYEGORE },
    { ACTOR_EN_FZ,          RI_SOUL_ENEMY_FREEZARD },
    { ACTOR_EN_JSO,         RI_SOUL_ENEMY_GARO },
    { ACTOR_EN_JSO2,        RI_SOUL_ENEMY_GARO },
    { ACTOR_EN_BIGSLIME,    RI_SOUL_ENEMY_GEKKO },
    { ACTOR_EN_PAMETFROG,   RI_SOUL_ENEMY_GEKKO },
    { ACTOR_EN_BEE,         RI_SOUL_ENEMY_GIANT_BEE },
    { ACTOR_EN_DEATH,       RI_SOUL_ENEMY_GOMESS },
    { ACTOR_EN_MINIDEATH,   RI_SOUL_ENEMY_GOMESS },
    { ACTOR_EN_CROW,        RI_SOUL_ENEMY_GUAY },
    { ACTOR_EN_RUPPECROW,   RI_SOUL_ENEMY_GUAY },
    { ACTOR_EN_PP,          RI_SOUL_ENEMY_HIPLOOP },
    { ACTOR_EN_KNIGHT,      RI_SOUL_ENEMY_IGOS_DU_IKANA },
    { ACTOR_EN_IK,          RI_SOUL_ENEMY_IRON_KNUCKLE },
    { ACTOR_EN_FIREFLY,     RI_SOUL_ENEMY_KEESE },
    { ACTOR_EN_NEO_REEBA,   RI_SOUL_ENEMY_LEEVER },
    { ACTOR_EN_RR,          RI_SOUL_ENEMY_LIKE_LIKE },
    { ACTOR_EN_DEKUNUTS,    RI_SOUL_ENEMY_MAD_SCRUB },
    { ACTOR_EN_BAGUO,       RI_SOUL_ENEMY_NEJIRON },
    { ACTOR_EN_OKUTA,       RI_SOUL_ENEMY_OCTOROK },
    { ACTOR_EN_PEEHAT,      RI_SOUL_ENEMY_PEAHAT },
    { ACTOR_EN_KAIZOKU,     RI_SOUL_ENEMY_PIRATE },
    { ACTOR_EN_BIGPO,       RI_SOUL_ENEMY_POE },
    { ACTOR_EN_PO_SISTERS,  RI_SOUL_ENEMY_POE },
    { ACTOR_EN_POH,         RI_SOUL_ENEMY_POE },
    { ACTOR_EN_RD,          RI_SOUL_ENEMY_REDEAD },
    { ACTOR_EN_SB,          RI_SOUL_ENEMY_SHELLBLADE },
    { ACTOR_EN_PR,          RI_SOUL_ENEMY_SKULLFISH },
    { ACTOR_EN_PR2,         RI_SOUL_ENEMY_SKULLFISH },
    { ACTOR_EN_PRZ,         RI_SOUL_ENEMY_SKULLFISH },
    { ACTOR_EN_ST,          RI_SOUL_ENEMY_SKULLTULA },
    { ACTOR_EN_SW,          RI_SOUL_ENEMY_SKULLTULA },
    { ACTOR_EN_BIGPAMET,    RI_SOUL_ENEMY_SNAPPER },
    { ACTOR_EN_KAME,        RI_SOUL_ENEMY_SNAPPER },
    { ACTOR_EN_HINT_SKB,    RI_SOUL_ENEMY_STALCHILD },
    { ACTOR_EN_RAIL_SKB,    RI_SOUL_ENEMY_STALCHILD },
    { ACTOR_EN_SKB,         RI_SOUL_ENEMY_STALCHILD },
    { ACTOR_EN_THIEFBIRD,   RI_SOUL_ENEMY_TAKKURI },
    { ACTOR_EN_TITE,        RI_SOUL_ENEMY_TEKTITE },
    { ACTOR_EN_FLOORMAS,    RI_SOUL_ENEMY_WALLMASTER },
    { ACTOR_EN_WALLMAS,     RI_SOUL_ENEMY_WALLMASTER },
    { ACTOR_BOSS_04,        RI_SOUL_ENEMY_WART },
    { ACTOR_EN_TANRON2,     RI_SOUL_ENEMY_WART },
    { ACTOR_EN_WIZ,         RI_SOUL_ENEMY_WIZROBE },
    { ACTOR_EN_WF,          RI_SOUL_ENEMY_WOLFOS },
};
// clang-format on

bool HaveEnemySoul(ActorId enemyId) {
    auto findSoulFlag = enemySoulMap.find(enemyId);
    if (findSoulFlag != enemySoulMap.end()) {
        RandoItemId randoItemId = findSoulFlag->second;
        if (randoItemId != RI_UNKNOWN) {
            return Flags_GetRandoInf(SOUL_RI_TO_RANDO_INF(randoItemId));
        }
    }
    // Enemy soul does not exist, so act as if it is obtained
    return true;
}

bool IsEnemySoulMissing(Actor* actor) {
    // ACTOR_EN_INVADEPOH is a shared actor so filter out the non aliens
    if (actor->id == ACTOR_EN_INVADEPOH && EN_INVADEPOH_GET_TYPE(actor) != EN_INVADEPOH_TYPE_ALIEN) {
        return false;
    }
    return !HaveEnemySoul((ActorId)actor->id);
}

void ShouldActorUpdate(Actor* actor, bool* should, RandoInf randoInf) {
    if (!Flags_GetRandoInf(randoInf)) {
        *should = false;
        actor->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else if (!actor->flags & ACTOR_FLAG_ATTENTION_ENABLED) {
        actor->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void ShouldActorDraw(Actor* actor, bool* should, RandoInf randoInf) {
    if (!Flags_GetRandoInf(randoInf)) {
        *should = false;
    }
}

void SetEnemyInjureGrayscale(bool enable) {
    OPEN_DISPS(gPlayState->state.gfxCtx);
    if (enable) {
        gDPSetGrayscaleColor(POLY_OPA_DISP++, 70, 70, 70, 255);
        gDPSetGrayscaleColor(POLY_XLU_DISP++, 70, 70, 70, 255);
    }
    gSPGrayscale(POLY_OPA_DISP++, enable);
    gSPGrayscale(POLY_XLU_DISP++, enable);
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void Rando::ActorBehavior::InitSoulsBehavior() {
    bool shouldBossRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_SOULS] == RO_GENERIC_YES;
    bool shouldEnemyInjure = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENEMY_SOULS] == RO_GENERIC_YES;

    COND_VB_SHOULD(VB_PERFORM_AC_COLLISION, shouldEnemyInjure, {
        Collider* at = va_arg(args, Collider*);
        Collider* ac = va_arg(args, Collider*);
        *should = HaveEnemySoul((ActorId)ac->actor->id);
    });

    // ShouldActorDraw & ShouldActorUpdate for Boss Souls
    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_HAKUGIN, shouldBossRegister, [](Actor* actor, bool* should) {
        if (!Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_BOSS_GOHT)) {
            BossHakugin_DrawIce((BossHakugin*)actor, gPlayState);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_GOHT_UNFREEZE, shouldBossRegister, {
        if (!Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_BOSS_GOHT)) {
            *should = false;
        }
    });

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_03, shouldBossRegister, [](Actor* actor, bool* should) {
        ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_BOSS_GYORG);
    });

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_07, shouldMajoraRegister(), [](Actor* actor, bool* should) {
        ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_BOSS_MAJORA);
    });

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_01, shouldBossRegister, [](Actor* actor, bool* should) {
        ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_BOSS_ODOLWA);
    });

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_02, shouldBossRegister, [](Actor* actor, bool* should) {
        ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_BOSS_TWINMOLD);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_03, shouldBossRegister, [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_BOSS_GYORG);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_07, shouldMajoraRegister(), [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_BOSS_MAJORA);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_01, shouldBossRegister, [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_BOSS_ODOLWA);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_02, shouldBossRegister, [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_BOSS_TWINMOLD);
    });

    /*
     * Giant's Mask functionality is handled by two pieces. The scene (Twinmold's Lair) determines whether the mask can
     * be used, while the Twinmold actor itself handles the transformation. Boss Souls prevent Twinmold from updating
     * unless its soul has been obtained, which results in a softlock. In this case, disable the item.
     */
    COND_VB_SHOULD(VB_ITEM_BE_RESTRICTED, shouldBossRegister, {
        ItemId itemId = *va_arg(args, ItemId*);
        if (itemId == ITEM_MASK_GIANT && gPlayState->sceneId == SCENE_INISIE_BS &&
            !Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_BOSS_TWINMOLD)) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_DRAW_LOCK_ON_ARROW, shouldEnemyInjure, {
        Actor* refActor = va_arg(args, Actor*);
        if (IsEnemySoulMissing(refActor)) {
            DrawEnLight({ 155, 0, 0 }, { 1.0f, 1.0f, 1.0f });
            *should = false;
        }
    });

    COND_HOOK(ShouldActorDraw, shouldEnemyInjure, [](Actor* actor, bool* should) {
        if (IsEnemySoulMissing(actor)) {
            SetEnemyInjureGrayscale(true);
        }
    });

    COND_HOOK(OnActorDraw, shouldEnemyInjure, [](Actor* actor) {
        if (IsEnemySoulMissing(actor)) {
            SetEnemyInjureGrayscale(false);
        }
    });
}
