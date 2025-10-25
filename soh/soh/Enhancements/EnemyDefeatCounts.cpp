#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "z64save.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

static void IncrementEnemyDefeatCount(GameplayStatCount countType) {
    gSaveContext.ship.stats.count[countType]++;
}

// this map is used for enemies that can be uniquely identified by their id
// and that are always counted
// enemies that can't be uniquely identified by their id
// or only sometimes count (like ACTOR_EN_TP)
// have to be manually handled in RegisterEnemyDefeatCounts
static std::unordered_map<ActorID, GameplayStatCount> uniqueEnemyIdToStatCount = {
    { ACTOR_EN_ANUBICE, COUNT_ENEMIES_DEFEATED_ANUBIS },
    { ACTOR_EN_AM, COUNT_ENEMIES_DEFEATED_ARMOS },
    { ACTOR_EN_CLEAR_TAG, COUNT_ENEMIES_DEFEATED_ARWING },
    { ACTOR_EN_VALI, COUNT_ENEMIES_DEFEATED_BARI },
    { ACTOR_EN_VM, COUNT_ENEMIES_DEFEATED_BEAMOS },
    { ACTOR_EN_BIGOKUTA, COUNT_ENEMIES_DEFEATED_BIG_OCTO },
    { ACTOR_EN_BILI, COUNT_ENEMIES_DEFEATED_BIRI },
    { ACTOR_EN_DNS, COUNT_ENEMIES_DEFEATED_BUSINESS_SCRUB },
    { ACTOR_EN_TORCH, COUNT_ENEMIES_DEFEATED_DARK_LINK },
    { ACTOR_EN_DH, COUNT_ENEMIES_DEFEATED_DEAD_HAND },
    { ACTOR_EN_HINTNUTS, COUNT_ENEMIES_DEFEATED_DEKU_SCRUB },
    { ACTOR_EN_DODONGO, COUNT_ENEMIES_DEFEATED_DODONGO },
    { ACTOR_EN_DODOJR, COUNT_ENEMIES_DEFEATED_DODONGO_BABY },
    { ACTOR_DOOR_KILLER, COUNT_ENEMIES_DEFEATED_DOOR_TRAP },
    { ACTOR_EN_FD, COUNT_ENEMIES_DEFEATED_FLARE_DANCER },
    { ACTOR_EN_FLOORMAS, COUNT_ENEMIES_DEFEATED_FLOORMASTER },
    { ACTOR_EN_TUBO_TRAP, COUNT_ENEMIES_DEFEATED_FLYING_POT },
    { ACTOR_EN_YUKABYUN, COUNT_ENEMIES_DEFEATED_FLOOR_TILE },
    { ACTOR_EN_FZ, COUNT_ENEMIES_DEFEATED_FREEZARD },
    { ACTOR_EN_GELDB, COUNT_ENEMIES_DEFEATED_GERUDO_THIEF },
    { ACTOR_EN_GOMA, COUNT_ENEMIES_DEFEATED_GOHMA_LARVA },
    { ACTOR_EN_CROW, COUNT_ENEMIES_DEFEATED_GUAY },
    { ACTOR_EN_RR, COUNT_ENEMIES_DEFEATED_LIKE_LIKE },
    { ACTOR_EN_DEKUNUTS, COUNT_ENEMIES_DEFEATED_MAD_SCRUB },
    { ACTOR_EN_OKUTA, COUNT_ENEMIES_DEFEATED_OCTOROK },
    { ACTOR_EN_BA, COUNT_ENEMIES_DEFEATED_PARASITIC_TENTACLE },
    { ACTOR_EN_PO_SISTERS, COUNT_ENEMIES_DEFEATED_POE_SISTERS },
    { ACTOR_EN_BUBBLE, COUNT_ENEMIES_DEFEATED_SHABOM },
    { ACTOR_EN_SB, COUNT_ENEMIES_DEFEATED_SHELLBLADE },
    { ACTOR_EN_SKJ, COUNT_ENEMIES_DEFEATED_SKULL_KID },
    { ACTOR_EN_NY, COUNT_ENEMIES_DEFEATED_SPIKE },
    { ACTOR_EN_SKB, COUNT_ENEMIES_DEFEATED_STALCHILD },
    { ACTOR_EN_TEST, COUNT_ENEMIES_DEFEATED_STALFOS },
    { ACTOR_EN_WEIYER, COUNT_ENEMIES_DEFEATED_STINGER },
    { ACTOR_EN_BW, COUNT_ENEMIES_DEFEATED_TORCH_SLUG },
    { ACTOR_EN_WALLMAS, COUNT_ENEMIES_DEFEATED_WALLMASTER },
    { ACTOR_EN_KAREBABA, COUNT_ENEMIES_DEFEATED_WITHERED_DEKU_BABA },
};

static void EnemyDefeatCounts_UniqueIds(void* refActor) {
    ActorID actorID = static_cast<Actor*>(refActor)->id;
    if (uniqueEnemyIdToStatCount.contains(actorID)) {
        IncrementEnemyDefeatCount(uniqueEnemyIdToStatCount[actorID]);
    }
}

static void EnemyDefeatCounts_EnBb(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);

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

static void EnemyDefeatCounts_EnDekubaba(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType =
        (actor->params == DEKUBABA_BIG) ? COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG : COUNT_ENEMIES_DEFEATED_DEKU_BABA;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnZf(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType =
        (actor->params == ENZF_TYPE_DINOLFOS) ? COUNT_ENEMIES_DEFEATED_DINOLFOS : COUNT_ENEMIES_DEFEATED_LIZALFOS;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnRd(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType = (actor->params >= -1) ? COUNT_ENEMIES_DEFEATED_REDEAD : COUNT_ENEMIES_DEFEATED_GIBDO;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnIk(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType =
        (actor->params == 0) ? COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE_NABOORU : COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnFirefly(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);

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

static void EnemyDefeatCounts_EnTp(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    // Only count the head, otherwise each body segment will increment
    if (actor->params == TAILPASARAN_HEAD) {
        IncrementEnemyDefeatCount(COUNT_ENEMIES_DEFEATED_TAILPASARAN);
    }
}

static void EnemyDefeatCounts_EnReeba(void* refActor) {
    EnReeba* reeba = static_cast<EnReeba*>(refActor);
    GameplayStatCount countType = reeba->isBig ? COUNT_ENEMIES_DEFEATED_LEEVER_BIG : COUNT_ENEMIES_DEFEATED_LEEVER;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnMb(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType =
        (actor->params == 0) ? COUNT_ENEMIES_DEFEATED_MOBLIN_CLUB : COUNT_ENEMIES_DEFEATED_MOBLIN;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnPeehat(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType =
        (actor->params == PEAHAT_TYPE_LARVA) ? COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA : COUNT_ENEMIES_DEFEATED_PEAHAT;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnPoh(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType = (actor->params == EN_POH_FLAT || actor->params == EN_POH_SHARP)
                                      ? COUNT_ENEMIES_DEFEATED_POE_COMPOSER
                                      : COUNT_ENEMIES_DEFEATED_POE;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnPoField(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType =
        (actor->params == EN_PO_FIELD_BIG) ? COUNT_ENEMIES_DEFEATED_POE_BIG : COUNT_ENEMIES_DEFEATED_POE;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnSt(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType =
        (actor->params == 1) ? COUNT_ENEMIES_DEFEATED_SKULLTULA_BIG : COUNT_ENEMIES_DEFEATED_SKULLTULA;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnSw(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);

    GameplayStatCount countType;
    if (((actor->params & 0xE000) >> 0xD) != 0) {
        countType = COUNT_ENEMIES_DEFEATED_SKULLTULA_GOLD;
    } else {
        countType = COUNT_ENEMIES_DEFEATED_SKULLTULA;
    }

    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnTite(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType =
        (actor->params == TEKTITE_BLUE) ? COUNT_ENEMIES_DEFEATED_TEKTITE_BLUE : COUNT_ENEMIES_DEFEATED_TEKTITE_RED;
    IncrementEnemyDefeatCount(countType);
}

static void EnemyDefeatCounts_EnWf(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    GameplayStatCount countType =
        (actor->params == WOLFOS_WHITE) ? COUNT_ENEMIES_DEFEATED_WOLFOS_WHITE : COUNT_ENEMIES_DEFEATED_WOLFOS;
    IncrementEnemyDefeatCount(countType);
}

static void RegisterEnemyDefeatCounts() {
    COND_HOOK(OnEnemyDefeat, true, EnemyDefeatCounts_UniqueIds);

    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_BB, true, EnemyDefeatCounts_EnBb);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_DEKUBABA, true, EnemyDefeatCounts_EnDekubaba);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_ZF, true, EnemyDefeatCounts_EnZf);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_RD, true, EnemyDefeatCounts_EnRd);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_IK, true, EnemyDefeatCounts_EnIk);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_FIREFLY, true, EnemyDefeatCounts_EnFirefly);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_TP, true, EnemyDefeatCounts_EnTp);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_REEBA, true, EnemyDefeatCounts_EnReeba);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_MB, true, EnemyDefeatCounts_EnMb);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_PEEHAT, true, EnemyDefeatCounts_EnPeehat);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_POH, true, EnemyDefeatCounts_EnPoh);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_PO_FIELD, true, EnemyDefeatCounts_EnPoField);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_ST, true, EnemyDefeatCounts_EnSt);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_SW, true, EnemyDefeatCounts_EnSw);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_TITE, true, EnemyDefeatCounts_EnTite);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_WF, true, EnemyDefeatCounts_EnWf);
}

static RegisterShipInitFunc initFunc(RegisterEnemyDefeatCounts);
