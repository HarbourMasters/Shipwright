#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "functions.h"

#include "overlays/actors/ovl_En_Mkk/z_en_mkk.h"
#include "overlays/actors/ovl_En_Slime/z_en_slime.h"
#include "overlays/actors/ovl_En_Sw/z_en_sw.h"
}

typedef enum {
    DROP_TYPE_NORMAL,
    DROP_TYPE_KILL,
} EnemyDropType;

// clang-format off
std::unordered_map<int16_t, std::tuple<RandoCheckId, ActorType, EnemyDropType>> enemyDropProfiles = {
    { ACTOR_EN_INVADEPOH,   { RC_ENEMY_DROP_ALIEN, ACTORCAT_PROP, DROP_TYPE_NORMAL } },
    { ACTOR_EN_AM,          { RC_ENEMY_DROP_ARMOS, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_BAT,         { RC_ENEMY_DROP_BAD_BAT, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    { ACTOR_EN_VM,          { RC_ENEMY_DROP_BEAMOS, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_BOSS_05,        { RC_ENEMY_DROP_BIO_DEKU_BABA, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_BB,          { RC_ENEMY_DROP_BLUE_BUBBLE, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_MKK,         { RC_ENEMY_DROP_BOE, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_SLIME,       { RC_ENEMY_DROP_CHUCHU, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    // Captain Keeta dies in a cutscene, so that drop is handled specially below.
    { ACTOR_EN_FAMOS,       { RC_ENEMY_DROP_DEATH_ARMOS, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_DRAGON,      { RC_ENEMY_DROP_DEEP_PYTHON, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    { ACTOR_EN_DEKUBABA,    { RC_ENEMY_DROP_DEKU_BABA, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_PR,          { RC_ENEMY_DROP_DESBREKO, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    { ACTOR_EN_WDHAND,      { RC_ENEMY_DROP_DEXIHAND, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    { ACTOR_EN_DINOFOS,     { RC_ENEMY_DROP_DINOLFOS, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    { ACTOR_EN_DODONGO,     { RC_ENEMY_DROP_DODONGO, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_GRASSHOPPER, { RC_ENEMY_DROP_DRAGONFLY, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_SNOWMAN,     { RC_ENEMY_DROP_EENO, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_EGOL,        { RC_ENEMY_DROP_EYEGORE, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    { ACTOR_EN_TUBO_TRAP,   { RC_ENEMY_DROP_FLYING_POT, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    { ACTOR_EN_FLOORMAS,    { RC_ENEMY_DROP_FLOORMASTER, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_FZ,          { RC_ENEMY_DROP_FREEZARD, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_JSO,         { RC_ENEMY_DROP_GARO, ACTORCAT_NPC, DROP_TYPE_KILL } },
    { ACTOR_EN_JSO2,        { RC_ENEMY_DROP_GARO_MASTER, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    { ACTOR_EN_BIGSLIME,    { RC_ENEMY_DROP_GEKKO, ACTORCAT_BOSS, DROP_TYPE_KILL } },
    { ACTOR_EN_PAMETFROG,   { RC_ENEMY_DROP_GEKKO, ACTORCAT_BOSS, DROP_TYPE_KILL } },
    { ACTOR_EN_BEE,         { RC_ENEMY_DROP_GIANT_BEE, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    // Gomess dies in a cutscene, so that drop is handled specially below.
    { ACTOR_EN_CROW,        { RC_ENEMY_DROP_GUAY, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_PP,          { RC_ENEMY_DROP_HIPLOOP, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    // Igos du Ikana dies in a cutscene, so that drop is handled specially below.
    { ACTOR_EN_IK,          { RC_ENEMY_DROP_IRON_KNUCKLE, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_FIREFLY,     { RC_ENEMY_DROP_KEESE, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_NEO_REEBA,   { RC_ENEMY_DROP_LEEVER, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_RR,          { RC_ENEMY_DROP_LIKE_LIKE, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_DEKUNUTS,    { RC_ENEMY_DROP_MAD_SCRUB, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_KAREBABA,    { RC_ENEMY_DROP_MINI_BABA, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_BAGUO,       { RC_ENEMY_DROP_NEJIRON, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_OKUTA,       { RC_ENEMY_DROP_OCTOROK, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_PEEHAT,      { RC_ENEMY_DROP_PEAHAT, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_KAIZOKU,     { RC_ENEMY_DROP_PIRATE, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    // Poes and Big Poes are excluded because they drop a bottleable item, which may make more sense for bottle shuffle.
    { ACTOR_EN_PO_SISTERS,  { RC_ENEMY_DROP_POE_SISTER, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_RAT,         { RC_ENEMY_DROP_REAL_BOMBCHU, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_BBFALL,      { RC_ENEMY_DROP_RED_BUBBLE, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    // Gibdos are excluded. Well Gibdos only get "killed" when receiving their requested item, which may not make sense
    // for enemy drops. Patrolling Gibdos take forever to die, and one of them doesn't call Actor_Kill at all, but also
    // does not handle normal drops.
    { ACTOR_EN_RD,          { RC_ENEMY_DROP_REDEAD, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_SB,          { RC_ENEMY_DROP_SHELLBLADE, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_PR2,         { RC_ENEMY_DROP_SKULLFISH, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_ST,          { RC_ENEMY_DROP_SKULLTULA, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_SW,          { RC_ENEMY_DROP_SKULLWALLTULA, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    { ACTOR_EN_BIGPAMET,    { RC_ENEMY_DROP_SNAPPER, ACTORCAT_BOSS, DROP_TYPE_KILL } },
    { ACTOR_EN_KAME,        { RC_ENEMY_DROP_SNAPPER, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_HINT_SKB,    { RC_ENEMY_DROP_STALCHILD, ACTORCAT_NPC, DROP_TYPE_NORMAL } },
    // ACTOR_EN_RAIL_SKB is excluded. It neither calls a drop function nor dies when attacked. It respawns. It's
    // logically gated no differently from the regular ACTOR_EN_SKB in the same region, so leave it be for now.
    { ACTOR_EN_SKB,         { RC_ENEMY_DROP_STALCHILD, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_THIEFBIRD,   { RC_ENEMY_DROP_TAKKURI, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_TITE,        { RC_ENEMY_DROP_TEKTITE, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_EN_WALLMAS,     { RC_ENEMY_DROP_WALLMASTER, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
    { ACTOR_BOSS_04,        { RC_ENEMY_DROP_WART, ACTORCAT_BOSS, DROP_TYPE_KILL } },
    { ACTOR_EN_WIZ,         { RC_ENEMY_DROP_WIZROBE, ACTORCAT_ENEMY, DROP_TYPE_KILL } },
    { ACTOR_EN_WF,          { RC_ENEMY_DROP_WOLFOS, ACTORCAT_ENEMY, DROP_TYPE_NORMAL } },
};
// clang-format on

void SpawnDropItem(Vec3f position, RandoCheckId randoCheckId) {
    CustomItem::Spawn(
        position.x, position.y, position.z, 0,
        CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN | CustomItem::ABLE_TO_ZORA_RANG, randoCheckId,
        [](Actor* actor, PlayState* play) {
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            randoSaveCheck.eligible = true;
        },
        [](Actor* actor, PlayState* play) {
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM),
                            (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
        });
}

bool ProcessDropProfile(Vec3f position, std::tuple<RandoCheckId, ActorType, EnemyDropType> dropProfile,
                        EnemyDropType dropType) {
    EnemyDropType profileDropType = std::get<EnemyDropType>(dropProfile);
    RandoCheckId randoCheckId = std::get<RandoCheckId>(dropProfile);
    if (profileDropType != dropType || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
        return false;
    }
    SpawnDropItem(position, randoCheckId);
    return true;
}

int isDropActorAtPosition(PlayState* play, Actor* callingActor_, Actor* actor, void* verifyData) {
    Vec3f* position = (Vec3f*)verifyData;
    return actor->world.pos.x == position->x && actor->world.pos.y == position->y && actor->world.pos.z == position->z;
}

bool SpawnNormalEnemyDrop(Vec3f position, u32 params) {
    Actor* dropActor;
    std::tuple<RandoCheckId, ActorType, EnemyDropType> dropProfile;
    for (auto it = enemyDropProfiles.begin(); it != enemyDropProfiles.end(); it++) {
        dropProfile = it->second;
        dropActor = SubS_FindActorCustom(gPlayState, NULL, NULL, std::get<ActorType>(dropProfile), it->first, &position,
                                         isDropActorAtPosition);
        if (dropActor != nullptr) {
            break;
        }
    }

    if (dropActor == nullptr) {
        return false;
    }

    // Only shuffle the Huge Rupee drop from Takkuri
    if (dropActor->id == ACTOR_EN_THIEFBIRD && params != ITEM00_RUPEE_HUGE) {
        return false;
    }

    // Do not shuffle the eaten Hero's Shield from a Like-Like
    if (dropActor->id == ACTOR_EN_RR && params == ITEM00_SHIELD_HERO) {
        return false;
    }

    return ProcessDropProfile(position, dropProfile, DROP_TYPE_NORMAL);
}

void Rando::ActorBehavior::InitEnemyDropBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENEMY_DROPS];

    COND_VB_SHOULD(VB_DROP_COLLECTIBLE, shouldRegister, {
        Vec3f position = va_arg(args, Vec3f);
        u32 params = va_arg(args, u32);
        if (SpawnNormalEnemyDrop(position, params)) {
            *should = false;
        }
    });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_MKK, shouldRegister, [](Actor* actor) {
        if (!RANDO_SAVE_CHECKS[RC_ENEMY_DROP_BOE].cycleObtained) {
            EnMkk* enMkk = (EnMkk*)actor;
            enMkk->unk_14C = 1; // Enable drop code only normally reachable with JP grotto Boes
        }
    });

    COND_HOOK(OnActorKill, shouldRegister, [](Actor* actor) {
        // Ignore Gold Skulltulas
        if (actor->id == ACTOR_EN_SW && ENSW_GET_3(actor)) {
            return;
        }

        if (actor->room == gPlayState->roomCtx.curRoom.num) { // Ignore room change actor kills
            for (auto& map : enemyDropProfiles) {
                if (map.first == actor->id) {
                    Vec3f position = actor->world.pos;
                    if (actor->id == ACTOR_EN_DRAGON) {
                        // The Deep Python's base is out of bounds. Mimic what it does when spawning the Seahorse.
                        position = actor->parent->world.pos;
                        position.x += Math_SinS(actor->world.rot.y + 0x8000) * (500.0f + BREG(38));
                        position.y += -100.0f + BREG(33);
                        position.z += Math_CosS(actor->world.rot.y + 0x8000) * (500.0f + BREG(38));
                    }
                    ProcessDropProfile(position, map.second, DROP_TYPE_KILL);
                    break;
                }
            }
        }
    });

    // Captain Keeta dies in a cutscene, so spawn that drop once the weekeventreg flag is set
    COND_HOOK(OnFlagSet, shouldRegister, [](FlagType flagType, u32 flag) {
        if (flagType == FLAG_WEEK_EVENT_REG && flag == WEEKEVENTREG_23_04) {
            // Spawn near where Link ends up after the fight
            SpawnDropItem({ -100.0f, 525.0f, -2330.0f }, RC_ENEMY_DROP_CAPTAIN_KEETA);
        }
    });

    // Igos dies in a cutscene, so spawn that drop once the scene clear flag is set
    COND_HOOK(OnSceneFlagSet, shouldRegister, [](s16 sceneId, FlagType flagType, u32 flag) {
        if (sceneId == SCENE_IKNINSIDE && flagType == FLAG_CYCL_SCENE_CLEARED_ROOM && flag == 1) {
            // Spawn on the throne
            SpawnDropItem({ 1408.25f, 76.0f, 2863.5f }, RC_ENEMY_DROP_IGOS_DU_IKANA);
        }
    });

    // Gomess dies in a cutscene, so spawn that drop once the scene clear flag is set
    COND_HOOK(OnSceneFlagSet, shouldRegister, [](s16 sceneId, FlagType flagType, u32 flag) {
        if (sceneId == SCENE_INISIE_R && flagType == FLAG_CYCL_SCENE_CLEARED_ROOM && flag == 0xB) {
            // Spawn at Gomess's spawn point
            SpawnDropItem({ 2460.0f, -300.0f, -1260.f }, RC_ENEMY_DROP_GOMESS);
        }
    });

    COND_VB_SHOULD(VB_DRAW_SLIME_RANDO_ITEM, shouldRegister, {
        if (!RANDO_SAVE_CHECKS[RC_ENEMY_DROP_CHUCHU].cycleObtained) {
            RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[RC_ENEMY_DROP_CHUCHU].randoItemId);

            EnSlime* slime = va_arg(args, EnSlime*);
            Matrix_RotateYS(slime->actor.shape.rot.y, MTXMODE_APPLY);
            Matrix_Scale(0.25f, 0.25f, 0.25f, MTXMODE_APPLY);
            Rando::DrawItem(randoItemId, RC_ENEMY_DROP_CHUCHU, (Actor*)&slime->actor);

            *should = false;
        }
    });
}
