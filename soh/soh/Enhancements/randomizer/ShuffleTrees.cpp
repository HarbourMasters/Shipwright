#include "ShuffleTrees.h"
#include "soh_assets.h"
#include "assets/objects/object_wood02/object_wood02.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_en_Wood02/z_en_wood02.h"
#include "static_data.h"
extern PlayState* gPlayState;
extern void EnWood02_Draw(Actor* thisx, PlayState* play);
}

static Gfx* D_80B3BF54[] = {
    (Gfx*)object_wood02_DL_0078D0, (Gfx*)object_wood02_DL_007CA0, (Gfx*)object_wood02_DL_0080D0,
    (Gfx*)object_wood02_DL_000090, (Gfx*)object_wood02_DL_000340, (Gfx*)object_wood02_DL_000340,
    (Gfx*)object_wood02_DL_000700,
};

static Gfx* D_80B3BF70[] = {
    (Gfx*)object_wood02_DL_007968,
    (Gfx*)object_wood02_DL_007D38,
    (Gfx*)object_wood02_DL_0081A8,
    NULL,
    NULL,
    NULL,
    (Gfx*)object_wood02_DL_007AD0,
    (Gfx*)object_wood02_DL_007E20,
    (Gfx*)object_wood02_DL_008350,
    (Gfx*)object_wood02_DL_000160,
    (Gfx*)object_wood02_DL_000440,
    (Gfx*)object_wood02_DL_000700,
};

extern void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play);

void Rando::StaticData::RegisterTreeLocations() {
    // Trees
    //            Randomizer Check                              Randomizer Check                                         Quest         Area                          Scene ID                           Params                          Short Name                    	   Spoiler Name                                Hint Text Key                     Vanilla             Spoiler Collection Check
    locationTable[RC_HF_NEAR_KAK_TREE]                    	    =	Location::Tree(RC_HF_NEAR_KAK_TREE,                  RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(3276, 971),    "Tree Outside Kakariko",          "HF Tree Near Kakariko",                   RHT_TREE_HYRULE_FIELD,	             RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_NEAR_KAK_TREE));
    locationTable[RC_HF_SOUTH_TREE]                    	        =	Location::Tree(RC_HF_SOUTH_TREE,                     RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-786, 11293),  "Tree in south Hyrule Field",     "HF Tree South",                           RHT_TREE_HYRULE_FIELD,	             RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_SOUTH_HF_TREE));
    locationTable[RC_HF_NEAR_LLR_TREE]                    	    =	Location::Tree(RC_HF_NEAR_LLR_TREE,                  RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-1907, 5409),  "Tree outside Lon Lon Ranch",     "HF Tree Outside LLR",                     RHT_TREE_HYRULE_FIELD,	             RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_NEAR_LLR_TREE));
    locationTable[RC_HF_NEAR_LH_TREE]                    	    =	Location::Tree(RC_HF_NEAR_LH_TREE,                   RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4377, 13662), "Tree outside Lake Hylia",        "HF Tree Outside LH",                      RHT_TREE_HYRULE_FIELD,	             RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_NEAR_LH_TREE));
    locationTable[RC_HF_NEAR_GV_TREE]                    	    =	Location::Tree(RC_HF_NEAR_GV_TREE,                   RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-6270, 8579),  "Tree outside Gerudo Valley",     "HF Tree Outside GV",                      RHT_TREE_HYRULE_FIELD,	             RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_NEAR_GV_TREE));
    locationTable[RC_HF_NEAR_ZR_TREE]                    	    =	Location::Tree(RC_HF_NEAR_ZR_TREE,                   RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(3117, 4239),   "Tree outside Zora's River",      "HF Tree Outside ZR",                      RHT_TREE_HYRULE_FIELD,	             RG_BOMBS_5,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_NEAR_ZR_TREE));
    locationTable[RC_HF_NEAR_KAK_S_TREE]                    	=	Location::Tree(RC_HF_NEAR_KAK_S_TREE,                RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(2076, -91),    "Small Tree Outside Kakariko",    "Small Tree Near Kakariko",             RHT_TREE_HYRULE_FIELD,	             RG_BOMBS_5,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_NEAR_KAK_S_TREE));
    locationTable[RC_MARKET_DAY_TREE]                    	    =	Location::Tree(RC_MARKET_DAY_TREE,                   RCQUEST_BOTH, RCAREA_MARKET,                SCENE_MARKET_DAY,                  TWO_ACTOR_PARAMS(-100, 240),    "Tree in Hyrule Market (Day)",    "Market Tree (Day)",                    RHT_TREE_MARKET,	                     RG_BOMBS_5,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_MARKET_DAY_TREE));
    locationTable[RC_ZF_TREE]                                   =   Location::Tree(RC_ZF_TREE,                           RCQUEST_BOTH, RCAREA_ZORAS_FOUNTAIN,        SCENE_ZORAS_FOUNTAIN,              TWO_ACTOR_PARAMS(186, 2222),    "Tree in Zora's Fountain",                      "ZF Tree with GS",           RHT_TREE_ZORAS_FOUNTAIN,                RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZF_TREE));
    locationTable[RC_ZR_TREE]                                   =   Location::Tree(RC_ZR_TREE,                           RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1690, 554),   "Tree in Zoras River",                          "ZR Tree with GS",           RHT_TREE_ZORAS_RIVER,                   RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_TREE));
    locationTable[RC_KAK_TREE]                                  =   Location::Tree(RC_KAK_TREE,                          RCQUEST_BOTH, RCAREA_KAKARIKO_VILLAGE,      SCENE_KAKARIKO_VILLAGE,            TWO_ACTOR_PARAMS(-860, 522),    "Kakariko GS Tree",                             "Kak Tree with GS",          RHT_TREE_KAKARIKO,                   RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_TREE));
    locationTable[RC_LLR_TREE]                                  =   Location::Tree(RC_LLR_TREE,                          RCQUEST_BOTH, RCAREA_LON_LON_RANCH,         SCENE_LON_LON_RANCH,               TWO_ACTOR_PARAMS(1309, -2241),  "Lon Lon Ranch GS Tree",                        "LLR Tree with GS",          RHT_TREE_LON_LON_RANCH,                        RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_LLR_TREE));
    locationTable[RC_HF_ADULT_NEAR_GV_TREE]                     =   Location::Tree(RC_HF_ADULT_NEAR_GV_TREE,             RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-6241, 7097),  "Hyrule Field Adult Near Gerudo Valley Tree",   "HF Tree near GV",           RHT_TREE_HYRULE_FIELD,                  RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ADULT_NEAR_GV_TREE));
    locationTable[RC_HC_SKULLTULA_TREE]                         =   Location::Tree(RC_HC_SKULLTULA_TREE,                 RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(-145, 2961),   "Tree (GS) in Hyrule Castle",                   "HC Tree with GS",           RHT_TREE_HYRULE_CASTLE,                      RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_SKULLTULA_TREE));
    locationTable[RC_HC_NEAR_GUARDS_TREE]                       =   Location::Tree(RC_HC_NEAR_GUARDS_TREE,               RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(1494, 2108),   "Tree Near HC Guards",                          "HC Tree Near Guards",       RHT_TREE_HYRULE_CASTLE,                      RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE));
    locationTable[RC_HC_GROTTO_TREE]                            =   Location::Tree(RC_HC_GROTTO_TREE,                    RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(924, 872),     "Tree Near HC Storms Grotto",                   "SoS Grotto Tree",        RHT_TREE_HYRULE_CASTLE,                      RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_GROTTO_TREE));
    locationTable[RC_HC_NEAR_GUARDS_TREE_5]                     =   Location::Tree(RC_HC_NEAR_GUARDS_TREE_5,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(-73, 1459),    "Tree Near HC Guards 5", "HC Tree Near Guards 5", RHT_TREE_HYRULE_CASTLE, RG_BLUE_RUPEE, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_5));
    locationTable[RC_HC_NEAR_GUARDS_TREE_4]                     =   Location::Tree(RC_HC_NEAR_GUARDS_TREE_4,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(421, 1397),    "Tree Near HC Guards 4", "HC Tree Near Guards 4", RHT_TREE_HYRULE_CASTLE, RG_BLUE_RUPEE, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_4));
    locationTable[RC_HC_NEAR_GUARDS_TREE_3]                     =   Location::Tree(RC_HC_NEAR_GUARDS_TREE_3,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(827, 1428),    "Tree Near HC Guards 3", "HC Tree Near Guards 3", RHT_TREE_HYRULE_CASTLE, RG_BLUE_RUPEE, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_3));
    locationTable[RC_HC_NEAR_GUARDS_TREE_2]                     =   Location::Tree(RC_HC_NEAR_GUARDS_TREE_2,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(943, 2051),    "Tree Near HC Guards 2", "HC Tree Near Guards 2", RHT_TREE_HYRULE_CASTLE, RG_BLUE_RUPEE, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_2));
    locationTable[RC_HC_NEAR_GUARDS_TREE_1]                     =   Location::Tree(RC_HC_NEAR_GUARDS_TREE_1,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(1209, 2242),   "Tree Near HC Guards 1", "HC Tree Near Guards 1", RHT_TREE_HYRULE_CASTLE, RG_BLUE_RUPEE, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_1));
    locationTable[RC_HF_SOUTH_TREE_1]                           =   Location::Tree(RC_HF_SOUTH_TREE_1,                   RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-129, 12554),  "Tree in South HF 1", "HF South Tree 1", RHT_TREE_HYRULE_FIELD, RG_BOMBS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTH_TREE_1));
    locationTable[RC_HF_NORTHWEST_TREE_2]                       =   Location::Tree(RC_HF_NORTHWEST_TREE_2,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4188, 264),   "Tree in Northwest HF 2", "HF NW Tree 2", RHT_TREE_HYRULE_FIELD, RG_BOMBS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NORTHWEST_TREE_2));
    locationTable[RC_HF_SOUTHEAST_TREE_4]                       =   Location::Tree(RC_HF_SOUTHEAST_TREE_4,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-130, 12554),  "Tree in South East HF 4", "HF SE Tree 4", RHT_TREE_HYRULE_FIELD, RG_BOMBS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_4));
    locationTable[RC_HF_NORTHWEST_TREE_1]                       =   Location::Tree(RC_HF_NORTHWEST_TREE_1,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4777, 136),   "Tree in NW HF 1", "HF NW Tree 1", RHT_TREE_HYRULE_FIELD, RG_BOMBS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NORTHWEST_TREE_1));
    locationTable[RC_HF_EAST_TREE_1]                            =   Location::Tree(RC_HF_EAST_TREE_1,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(3817, 7119),   "Tree in East HF 1", "HF East Tree 1", RHT_TREE_HYRULE_FIELD, RG_BOMBS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_EAST_TREE_1));
    locationTable[RC_HF_SOUTHEAST_TREE_3]                       =   Location::Tree(RC_HF_SOUTHEAST_TREE_3,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(915, 12557),   "Tree in South East HF 3", "HF SE Tree 3", RHT_TREE_HYRULE_FIELD, RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_3));
    locationTable[RC_HF_SOUTHEAST_TREE_2]                       =   Location::Tree(RC_HF_SOUTHEAST_TREE_2,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(350, 11605),   "Tree in South East HF 2", "HF SE Tree 2", RHT_TREE_HYRULE_FIELD, RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_2));
    locationTable[RC_HF_NEAR_HC_GROTTO_TREE_3]                  =   Location::Tree(RC_HF_NEAR_HC_GROTTO_TREE_3,          RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-1448, 620),   "Tree Near HC Entrance Grotto 3", "HF Tree Near HC Grotto 3", RHT_TREE_HYRULE_FIELD, RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NEAR_HC_GROTTO_TREE_3));
    locationTable[RC_HF_SOUTHEAST_TREE_1]                       =   Location::Tree(RC_HF_SOUTHEAST_TREE_1,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(1535, 11943),  "Tree in South East HF 1", "HF Tree in SE HF 1", RHT_TREE_HYRULE_FIELD, RG_BLUE_RUPEE, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_1));
    locationTable[RC_HF_TREE_YELLOW_GROTTO_TREE]                =   Location::Tree(RC_HF_TREE_YELLOW_GROTTO_TREE,        RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4976, 2812),  "Tree in HF near Grotto", "HF Tree near Grotto 2", RHT_TREE_HYRULE_FIELD, RG_BLUE_RUPEE, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_TREE_YELLOW_GROTTO_TREE));
    locationTable[RC_HF_TREE_NEAR_HC_GROTTO_2]                  =   Location::Tree(RC_HF_TREE_NEAR_HC_GROTTO_2,          RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-1244, 819),   "Tree Near HC Entrance Grotto 2", "HF Tree Near HC Grotto 2", RHT_TREE_HYRULE_FIELD, RG_BLUE_RUPEE, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_TREE_NEAR_HC_GROTTO_2));
    locationTable[RC_HF_TREE_NEAR_HC_GROTTO_1]                  =   Location::Tree(RC_HF_TREE_NEAR_HC_GROTTO_1,          RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-1041, 1022),  "Tree Near HC Entrance Grotto 1", "HF Tree Near HC Grotto 1", RHT_TREE_HYRULE_FIELD, RG_BLUE_RUPEE, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_TREE_NEAR_HC_GROTTO_1));
}

static RegisterShipInitFunc initFunc(Rando::StaticData::RegisterTreeLocations);

uint8_t EnWood02_RandomizerHoldsItem(EnWood02* treeActor, PlayState* play) {
    uint8_t treeSetting = Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_TREES).Get();

    // Don't pull randomized item if tree isn't randomized or is already checked
    if (!IS_RANDO || Flags_GetRandomizerInf(treeActor->treeId.randomizerInf) ||
        treeActor->treeId.randomizerCheck == RC_UNKNOWN_CHECK) {
        return false;
    } else {
        return true;
    }
}

extern "C" void EnWood02_RandomizerDraw(Actor* thisx, PlayState* play) {
    EnWood02* thisy = (EnWood02*)thisx;
    float treeSize = 1.0f;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    if ((thisy->actor.params == WOOD_LEAF_GREEN) || (thisy->actor.params == WOOD_LEAF_YELLOW)) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 127);
        Gfx_DrawDListOpa(play, (Gfx*)gRandoTreeDL);
    } else if (D_80B3BF70[thisy->drawType & 0xF] != NULL) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 0);
        Gfx_DrawDListOpa(play, (Gfx*)gRandoTreeDL);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, D_80B3BF70[thisy->drawType & 0xF]);
    }
    CLOSE_DISPS(play->state.gfxCtx);
    if (!EnWood02_RandomizerHoldsItem(thisy, gPlayState)) {
        thisy->actor.draw = (ActorFunc)EnWood02_Draw;
    }
}



void EnWood02_RandomizerSpawnCollectible(EnWood02* treeActor, PlayState* play) {
    EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &treeActor->actor.world.pos, ITEM00_SOH_DUMMY);
    item00->randoInf = treeActor->treeId.randomizerInf;
    item00->itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(treeActor->treeId.randomizerCheck, true, GI_NONE);
    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
    item00->actor.velocity.y = 8.0f;
    item00->actor.speedXZ = 2.0f;
    item00->actor.world.rot.y = Rand_CenteredFloat(65536.0f);
}

void EnWood02_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    // 0x00 - Large trees
    // 0x01 - Medium trees
    // 0x02 - Small trees
    if (actor->id == ACTOR_EN_WOOD02 && actor->params >= 0x03 && gPlayState->sceneNum == SCENE_HYRULE_CASTLE)
        return;
    if (actor->id != ACTOR_EN_WOOD02 || (gPlayState->sceneNum == SCENE_HYRULE_FIELD && gSaveContext.cutsceneIndex == 0xFFF3))
        return;
    if (actor->id == ACTOR_EN_WOOD02 && ((actor->params >= WOOD_BUSH_GREEN_SMALL) || (actor->params == WOOD_TREE_OVAL_GREEN_SPAWNED || actor->params == WOOD_TREE_OVAL_YELLOW_SPAWNED || actor->params == WOOD_TREE_CONICAL_SPAWNED)))
        return;

    EnWood02* treeActor = static_cast<EnWood02*>(actorRef);

    treeActor->treeId = OTRGlobals::Instance->gRandomizer->IdentifyTree(gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z);
}

void ShuffleTrees_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);
    // Hook drawing function, draw randomized tree.
    if (id == VB_TREE_SETUP_DRAW) {
        EnWood02* treeActor = va_arg(args, EnWood02*);
        if (EnWood02_RandomizerHoldsItem(treeActor, gPlayState)) {
            treeActor->actor.draw = (ActorFunc)EnWood02_RandomizerDraw;
            *should = false;
        }
    }

    // Hook dropping function, spawn randomized item.
    if (id == VB_TREE_DROP_ITEM) {
        EnWood02* treeActor = va_arg(args, EnWood02*);
        if (EnWood02_RandomizerHoldsItem(treeActor, gPlayState)) {
            EnWood02_RandomizerSpawnCollectible(treeActor, gPlayState);
            *should = false;
        }
    }

    va_end(args);
}
