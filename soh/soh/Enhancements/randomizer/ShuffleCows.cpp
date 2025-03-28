#include <soh/OTRGlobals.h>
#include "static_data.h"

extern "C" {
#include "src/overlays/actors/ovl_En_Cow/z_en_cow.h"
extern PlayState* gPlayState;
}

void EnCow_MoveForRandomizer(EnCow* enCow, PlayState* play) {
    bool moved = false;

    // Don't reposition the tail
    if (enCow->actor.params != 0) {
        return;
    }

    if (play->sceneNum == SCENE_LON_LON_BUILDINGS && enCow->actor.world.pos.x == -108 &&
        enCow->actor.world.pos.z == -65) {
        // Move left cow in lon lon tower
        enCow->actor.world.pos.x = -229.0f;
        enCow->actor.world.pos.z = 157.0f;
        enCow->actor.shape.rot.y = 15783.0f;
        moved = true;
    } else if (play->sceneNum == SCENE_STABLE && enCow->actor.world.pos.x == -3 && enCow->actor.world.pos.z == -254) {
        // Move right cow in lon lon stable
        enCow->actor.world.pos.x += 119.0f;
        moved = true;
    }

    if (moved) {
        // Reposition collider
        func_809DEE9C(enCow);
    }
}

void RegisterShuffleCows() {
    bool shouldRegister = IS_RANDO && Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_COWS).Get();

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_COW, shouldRegister, {
        EnCow* enCow = va_arg(args, EnCow*);
        CowIdentity cowIdentity = OTRGlobals::Instance->gRandomizer->IdentifyCow(gPlayState->sceneNum, enCow->actor.world.pos.x, enCow->actor.world.pos.z);
        // Has this cow already rewarded an item?
        if (!Flags_GetRandomizerInf(cowIdentity.randomizerInf)) {
            Flags_SetRandomizerInf(cowIdentity.randomizerInf);
            // setting the ocarina mode here prevents intermittent issues
            // with the item get not triggering until walking away
            gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_00;
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_DESPAWN_HORSE_RACE_COW, shouldRegister, {
        EnCow* enCow = va_arg(args, EnCow*);
        // If this is a cow we have to move, then move it now.
        EnCow_MoveForRandomizer(enCow, gPlayState);
    });
}

static RegisterShipInitFunc initFunc(RegisterShuffleCows, { "IS_RANDO" });

void Rando::StaticData::RegisterCowLocations() {
    static bool registered = false;
    if (registered) return;
    registered = true;
    // clang-format-off
    locationTable[RC_KF_LINKS_HOUSE_COW] =      Location::Base(RC_KF_LINKS_HOUSE_COW,      RCQUEST_BOTH, RCTYPE_COW,                              ACTOR_EN_COW, SCENE_LINKS_HOUSE,       0x00,                               "Links House Cow",   RHT_KF_LINKS_HOUSE_COW,      RG_MILK, SpoilerCollectionCheck::RandomizerInf(RAND_INF_COWS_MILKED_KF_LINKS_HOUSE_COW));
    locationTable[RC_HF_COW_GROTTO_COW] =       Location::Base(RC_HF_COW_GROTTO_COW,       RCQUEST_BOTH, RCTYPE_COW, RCAREA_HYRULE_FIELD,         ACTOR_EN_COW, SCENE_GROTTOS,           TWO_ACTOR_PARAMS(3485, -291),       "Cow Grotto Cow",    RHT_HF_COW_GROTTO_COW,       RG_MILK, SpoilerCollectionCheck::RandomizerInf(RAND_INF_COWS_MILKED_HF_COW_GROTTO_COW));
    locationTable[RC_LLR_STABLES_LEFT_COW] =    Location::Base(RC_LLR_STABLES_LEFT_COW,    RCQUEST_BOTH, RCTYPE_COW,                              ACTOR_EN_COW, SCENE_STABLE,            TWO_ACTOR_PARAMS(-122, -254),       "Stables Left Cow",  RHT_LLR_STABLES_LEFT_COW,    RG_MILK, SpoilerCollectionCheck::RandomizerInf(RAND_INF_COWS_MILKED_LLR_STABLES_LEFT_COW));
    locationTable[RC_LLR_STABLES_RIGHT_COW] =   Location::Base(RC_LLR_STABLES_RIGHT_COW,   RCQUEST_BOTH, RCTYPE_COW,                              ACTOR_EN_COW, SCENE_STABLE,            TWO_ACTOR_PARAMS(116, -254),        "Stables Right Cow", RHT_LLR_STABLES_RIGHT_COW,   RG_MILK, SpoilerCollectionCheck::RandomizerInf(RAND_INF_COWS_MILKED_LLR_STABLES_RIGHT_COW));
    locationTable[RC_LLR_TOWER_LEFT_COW] =      Location::Base(RC_LLR_TOWER_LEFT_COW,      RCQUEST_BOTH, RCTYPE_COW,                              ACTOR_EN_COW, SCENE_LON_LON_BUILDINGS, TWO_ACTOR_PARAMS(-229, 157),        "Tower Left Cow",    RHT_LLR_TOWER_LEFT_COW,      RG_MILK, SpoilerCollectionCheck::RandomizerInf(RAND_INF_COWS_MILKED_LLR_TOWER_LEFT_COW));
    locationTable[RC_LLR_TOWER_RIGHT_COW] =     Location::Base(RC_LLR_TOWER_RIGHT_COW,     RCQUEST_BOTH, RCTYPE_COW,                              ACTOR_EN_COW, SCENE_LON_LON_BUILDINGS, TWO_ACTOR_PARAMS(-142, -140),       "Tower Right Cow",   RHT_LLR_TOWER_RIGHT_COW,     RG_MILK, SpoilerCollectionCheck::RandomizerInf(RAND_INF_COWS_MILKED_LLR_TOWER_RIGHT_COW));
    locationTable[RC_KAK_IMPAS_HOUSE_COW] =     Location::Base(RC_KAK_IMPAS_HOUSE_COW,     RCQUEST_BOTH, RCTYPE_COW,                              ACTOR_EN_COW, SCENE_IMPAS_HOUSE,       0x00,                               "Impas House Cow",   RHT_KAK_IMPAS_HOUSE_COW,     RG_MILK, SpoilerCollectionCheck::RandomizerInf(RAND_INF_COWS_MILKED_KAK_IMPAS_HOUSE_COW));
    locationTable[RC_DMT_COW_GROTTO_COW] =      Location::Base(RC_DMT_COW_GROTTO_COW,      RCQUEST_BOTH, RCTYPE_COW, RCAREA_DEATH_MOUNTAIN_TRAIL, ACTOR_EN_COW, SCENE_GROTTOS,           TWO_ACTOR_PARAMS(2444, -471),       "Cow Grotto Cow",    RHT_DMT_COW_GROTTO_COW,      RG_MILK, SpoilerCollectionCheck::RandomizerInf(RAND_INF_COWS_MILKED_DMT_COW_GROTTO_COW));
    locationTable[RC_GV_COW] =                  Location::Base(RC_GV_COW,                  RCQUEST_BOTH, RCTYPE_COW,                              ACTOR_EN_COW, SCENE_GERUDO_VALLEY,     0x00,                               "Cow",               RHT_GV_COW,                  RG_MILK, SpoilerCollectionCheck::RandomizerInf(RAND_INF_COWS_MILKED_GV_COW));
    locationTable[RC_JABU_JABUS_BELLY_MQ_COW] = Location::Base(RC_JABU_JABUS_BELLY_MQ_COW, RCQUEST_MQ,   RCTYPE_COW,                              ACTOR_EN_COW, SCENE_JABU_JABU,         0x00,                               "MQ Cow",            RHT_JABU_JABUS_BELLY_MQ_COW, RG_MILK, SpoilerCollectionCheck::RandomizerInf(RAND_INF_COWS_MILKED_JABU_JABUS_BELLY_MQ_COW));

    // clang-format-on
}

static RegisterShipInitFunc registerFunc(Rando::StaticData::RegisterCowLocations);
