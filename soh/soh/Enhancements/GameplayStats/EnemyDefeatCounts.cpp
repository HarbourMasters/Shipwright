#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Bb/z_en_bb.h"
#include "src/overlays/actors/ovl_En_Dekubaba/z_en_dekubaba.h"
#include "src/overlays/actors/ovl_En_Mb/z_en_mb.h"
#include "src/overlays/actors/ovl_En_Tite/z_en_tite.h"
#include "src/overlays/actors/ovl_En_Zf/z_en_zf.h"
#include "src/overlays/actors/ovl_En_Wf/z_en_wf.h"
#include "src/overlays/actors/ovl_En_Reeba/z_en_reeba.h"
#include "src/overlays/actors/ovl_En_Peehat/z_en_peehat.h"
#include "src/overlays/actors/ovl_En_Po_Field/z_en_po_field.h"
#include "src/overlays/actors/ovl_En_Poh/z_en_poh.h"
#include "src/overlays/actors/ovl_En_Tp/z_en_tp.h"
#include "src/overlays/actors/ovl_En_Firefly/z_en_firefly.h"

extern SaveContext gSaveContext;
}

static void IncrementEnemyDefeatCount(GameplayStatCount countType) {
    gSaveContext.ship.stats.count[countType]++;
}

#define ENEMY_DEFEAT_COUNT(actorID, func) \
    COND_ID_HOOK(OnEnemyDefeat, actorID, true, [](void* refActor) { func(static_cast<Actor*>(refActor)); });

#define ENEMY_DEFEAT_COUNT_UNIQUE(actorID, countType) \
    COND_ID_HOOK(OnEnemyDefeat, actorID, true, [](void*) { IncrementEnemyDefeatCount(countType); });

static void EnemyDefeatCounts_EnBb(Actor* actor) {
    GameplayStatCount countType;
    switch (actor->params) {
        case ENBB_GREEN:
        case ENBB_GREEN_BIG:
            countType = COUNT_ENEMIES_DEFEATED_BUBBLE_GREEN;
            break;
        case ENBB_BLUE:
            countType = COUNT_ENEMIES_DEFEATED_BUBBLE_BLUE;
            break;
        case ENBB_WHITE:
            countType = COUNT_ENEMIES_DEFEATED_BUBBLE_WHITE;
            break;
        case ENBB_RED:
            countType = COUNT_ENEMIES_DEFEATED_BUBBLE_RED;
            break;
        default:
            return;
    }

    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnDekubaba(Actor* actor) {
    GameplayStatCount countType =
        (actor->params == DEKUBABA_BIG) ? COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG : COUNT_ENEMIES_DEFEATED_DEKU_BABA;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnZf(Actor* actor) {
    GameplayStatCount countType =
        (actor->params == ENZF_TYPE_DINOLFOS) ? COUNT_ENEMIES_DEFEATED_DINOLFOS : COUNT_ENEMIES_DEFEATED_LIZALFOS;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnRd(Actor* actor) {
    GameplayStatCount countType = (actor->params >= -1) ? COUNT_ENEMIES_DEFEATED_REDEAD : COUNT_ENEMIES_DEFEATED_GIBDO;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnIk(Actor* actor) {
    GameplayStatCount countType =
        (actor->params == 0) ? COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE_NABOORU : COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnFirefly(Actor* actor) {
    GameplayStatCount countType;
    switch (actor->params) {
        case KEESE_NORMAL_FLY:
        case KEESE_NORMAL_PERCH:
            countType = COUNT_ENEMIES_DEFEATED_KEESE;
            break;
        case KEESE_FIRE_FLY:
        case KEESE_FIRE_PERCH:
            countType = COUNT_ENEMIES_DEFEATED_KEESE_FIRE;
            break;
        case KEESE_ICE_FLY:
            countType = COUNT_ENEMIES_DEFEATED_KEESE_ICE;
            break;
        default:
            return;
    }

    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnTp(Actor* actor) {
    // Only count the head, otherwise each body segment will increment
    if (actor->params == TAILPASARAN_HEAD) {
        IncrementEnemyDefeatCount(COUNT_ENEMIES_DEFEATED_TAILPASARAN);
    }
}

static void EnemyDefeatCounts_EnReeba(Actor* actor) {
    EnReeba* reeba = (EnReeba*)actor;
    GameplayStatCount countType = reeba->isBig ? COUNT_ENEMIES_DEFEATED_LEEVER_BIG : COUNT_ENEMIES_DEFEATED_LEEVER;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnMb(Actor* actor) {
    GameplayStatCount countType =
        (actor->params == 0) ? COUNT_ENEMIES_DEFEATED_MOBLIN_CLUB : COUNT_ENEMIES_DEFEATED_MOBLIN;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnPeehat(Actor* actor) {
    GameplayStatCount countType =
        (actor->params == PEAHAT_TYPE_LARVA) ? COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA : COUNT_ENEMIES_DEFEATED_PEAHAT;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnPoh(Actor* actor) {
    GameplayStatCount countType = (actor->params == EN_POH_FLAT || actor->params == EN_POH_SHARP)
                                      ? COUNT_ENEMIES_DEFEATED_POE_COMPOSER
                                      : COUNT_ENEMIES_DEFEATED_POE;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnPoField(Actor* actor) {
    GameplayStatCount countType =
        (actor->params == EN_PO_FIELD_BIG) ? COUNT_ENEMIES_DEFEATED_POE_BIG : COUNT_ENEMIES_DEFEATED_POE;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnSt(Actor* actor) {
    GameplayStatCount countType =
        (actor->params == 1) ? COUNT_ENEMIES_DEFEATED_SKULLTULA_BIG : COUNT_ENEMIES_DEFEATED_SKULLTULA;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnSw(Actor* actor) {
    GameplayStatCount countType;
    if (((actor->params & 0xE000) >> 0xD) != 0) {
        countType = COUNT_ENEMIES_DEFEATED_SKULLTULA_GOLD;
    } else {
        countType = COUNT_ENEMIES_DEFEATED_SKULLWALLTULA;
    }

    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnTite(Actor* actor) {
    GameplayStatCount countType =
        (actor->params == TEKTITE_BLUE) ? COUNT_ENEMIES_DEFEATED_TEKTITE_BLUE : COUNT_ENEMIES_DEFEATED_TEKTITE_RED;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnWf(Actor* actor) {
    GameplayStatCount countType =
        (actor->params == WOLFOS_WHITE) ? COUNT_ENEMIES_DEFEATED_WOLFOS_WHITE : COUNT_ENEMIES_DEFEATED_WOLFOS;
    IncrementEnemyDefeatCount(countType);
}

static void RegisterEnemyDefeatCounts() {
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_ANUBICE, COUNT_ENEMIES_DEFEATED_ANUBIS);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_AM, COUNT_ENEMIES_DEFEATED_ARMOS);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_CLEAR_TAG, COUNT_ENEMIES_DEFEATED_ARWING);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_VALI, COUNT_ENEMIES_DEFEATED_BARI);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_VM, COUNT_ENEMIES_DEFEATED_BEAMOS);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_BIGOKUTA, COUNT_ENEMIES_DEFEATED_BIG_OCTO);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_BILI, COUNT_ENEMIES_DEFEATED_BIRI);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_DNS, COUNT_ENEMIES_DEFEATED_BUSINESS_SCRUB);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_TORCH2, COUNT_ENEMIES_DEFEATED_DARK_LINK);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_DH, COUNT_ENEMIES_DEFEATED_DEAD_HAND);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_HINTNUTS, COUNT_ENEMIES_DEFEATED_DEKU_SCRUB);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_DODONGO, COUNT_ENEMIES_DEFEATED_DODONGO);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_DODOJR, COUNT_ENEMIES_DEFEATED_DODONGO_BABY);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_DOOR_KILLER, COUNT_ENEMIES_DEFEATED_DOOR_TRAP);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_FD, COUNT_ENEMIES_DEFEATED_FLARE_DANCER);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_FLOORMAS, COUNT_ENEMIES_DEFEATED_FLOORMASTER);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_TUBO_TRAP, COUNT_ENEMIES_DEFEATED_FLYING_POT);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_YUKABYUN, COUNT_ENEMIES_DEFEATED_FLOOR_TILE);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_FZ, COUNT_ENEMIES_DEFEATED_FREEZARD);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_GELDB, COUNT_ENEMIES_DEFEATED_GERUDO_THIEF);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_GOMA, COUNT_ENEMIES_DEFEATED_GOHMA_LARVA);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_CROW, COUNT_ENEMIES_DEFEATED_GUAY);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_RR, COUNT_ENEMIES_DEFEATED_LIKE_LIKE);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_DEKUNUTS, COUNT_ENEMIES_DEFEATED_MAD_SCRUB);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_OKUTA, COUNT_ENEMIES_DEFEATED_OCTOROK);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_BA, COUNT_ENEMIES_DEFEATED_PARASITIC_TENTACLE);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_PO_SISTERS, COUNT_ENEMIES_DEFEATED_POE_SISTERS);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_BUBBLE, COUNT_ENEMIES_DEFEATED_SHABOM);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_SB, COUNT_ENEMIES_DEFEATED_SHELLBLADE);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_SKJ, COUNT_ENEMIES_DEFEATED_SKULL_KID);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_NY, COUNT_ENEMIES_DEFEATED_SPIKE);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_SKB, COUNT_ENEMIES_DEFEATED_STALCHILD);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_TEST, COUNT_ENEMIES_DEFEATED_STALFOS);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_EIYER, COUNT_ENEMIES_DEFEATED_STINGER);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_WEIYER, COUNT_ENEMIES_DEFEATED_STINGER);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_BW, COUNT_ENEMIES_DEFEATED_TORCH_SLUG);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_WALLMAS, COUNT_ENEMIES_DEFEATED_WALLMASTER);
    ENEMY_DEFEAT_COUNT_UNIQUE(ACTOR_EN_KAREBABA, COUNT_ENEMIES_DEFEATED_WITHERED_DEKU_BABA);

    ENEMY_DEFEAT_COUNT(ACTOR_EN_BB, EnemyDefeatCounts_EnBb);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_DEKUBABA, EnemyDefeatCounts_EnDekubaba);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_ZF, EnemyDefeatCounts_EnZf);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_RD, EnemyDefeatCounts_EnRd);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_IK, EnemyDefeatCounts_EnIk);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_FIREFLY, EnemyDefeatCounts_EnFirefly);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_TP, EnemyDefeatCounts_EnTp);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_REEBA, EnemyDefeatCounts_EnReeba);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_MB, EnemyDefeatCounts_EnMb);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_PEEHAT, EnemyDefeatCounts_EnPeehat);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_POH, EnemyDefeatCounts_EnPoh);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_PO_FIELD, EnemyDefeatCounts_EnPoField);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_ST, EnemyDefeatCounts_EnSt);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_SW, EnemyDefeatCounts_EnSw);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_TITE, EnemyDefeatCounts_EnTite);
    ENEMY_DEFEAT_COUNT(ACTOR_EN_WF, EnemyDefeatCounts_EnWf);
}

static RegisterShipInitFunc initFunc(RegisterEnemyDefeatCounts);
