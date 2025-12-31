#include <soh/OTRGlobals.h>
#include "soh_assets.h"
#include "static_data.h"
#include "soh/ObjectExtension/ObjectExtension.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Wood02/z_en_wood02.h"
#include "objects/object_wood02/object_wood02.h"
#include "soh/Enhancements/enhancementTypes.h"
extern PlayState* gPlayState;
void EnWood02_Draw(Actor*, PlayState*);
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

uint8_t EnWood02_RandomizerHoldsItem(EnWood02* treeActor, PlayState* play) {
    const auto treeIdentity = ObjectExtension::GetInstance().Get<TreeIdentity>(&treeActor->actor);
    if (treeIdentity == nullptr) {
        return false;
    }

    // Don't pull randomized item if tree isn't randomized or is already checked
    return IS_RANDO && Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_TREES).Get() &&
           !Flags_GetRandomizerInf(treeIdentity->randomizerInf) && treeIdentity->randomizerCheck != RC_UNKNOWN_CHECK;
}

extern "C" void EnWood02_RandomizerDraw(Actor* thisx, PlayState* play) {
    GetItemCategory getItemCategory;
    GetItemEntry smallCrateItem;
    auto treeActor = (EnWood02*)thisx;
    int csmc = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), CSMC_DISABLED);
    int requiresStoneAgony = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"), 0);

    int isVanilla =
        csmc == CSMC_DISABLED || csmc == CSMC_SIZE || (requiresStoneAgony && !CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY));

    const auto treeIdentity = ObjectExtension::GetInstance().Get<TreeIdentity>(&treeActor->actor);
    if (treeIdentity == nullptr) {
        return;
    }

    if (isVanilla || treeIdentity == nullptr || treeIdentity->randomizerCheck == RC_UNKNOWN_CHECK) {
        getItemCategory = ITEM_CATEGORY_JUNK;
    } else {
        smallCrateItem = Rando::Context::GetInstance()->GetFinalGIEntry(treeIdentity->randomizerCheck, true, GI_NONE);
        getItemCategory = smallCrateItem.getItemCategory;

        // If they have bombchus, don't consider the bombchu item major
        if (INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU &&
            ((smallCrateItem.modIndex == MOD_RANDOMIZER && smallCrateItem.getItemId == RG_PROGRESSIVE_BOMBCHU_BAG) ||
             (smallCrateItem.modIndex == MOD_NONE &&
              (smallCrateItem.getItemId == GI_BOMBCHUS_5 || smallCrateItem.getItemId == GI_BOMBCHUS_10 ||
               smallCrateItem.getItemId == GI_BOMBCHUS_20)))) {
            getItemCategory = ITEM_CATEGORY_JUNK;
            // If it's a bottle and they already have one, consider the item lesser
        } else if ((smallCrateItem.modIndex == MOD_RANDOMIZER &&
                    smallCrateItem.getItemId >= RG_BOTTLE_WITH_RED_POTION &&
                    smallCrateItem.getItemId <= RG_BOTTLE_WITH_POE) ||
                   (smallCrateItem.modIndex == MOD_NONE &&
                    (smallCrateItem.getItemId == GI_BOTTLE || smallCrateItem.getItemId == GI_MILK_BOTTLE))) {
            if (gSaveContext.inventory.items[SLOT_BOTTLE_1] != ITEM_NONE) {
                getItemCategory = ITEM_CATEGORY_LESSER;
            }
        }
    }

    GraphicsContext* gfxCtx = play->state.gfxCtx;
    OPEN_DISPS(gfxCtx);
    Matrix_Push();

    // Change texture
    switch (getItemCategory) {
        case ITEM_CATEGORY_MAJOR:
            Matrix_Scale(0.1, 0.05, 0.1, MTXMODE_APPLY);
            Gfx_DrawDListOpa(play, (Gfx*)gSmallMajorCrateDL);
            break;
        case ITEM_CATEGORY_SKULLTULA_TOKEN:
            Matrix_Scale(0.1, 0.05, 0.1, MTXMODE_APPLY);
            Gfx_DrawDListOpa(play, (Gfx*)gSmallTokenCrateDL);
            break;
        case ITEM_CATEGORY_SMALL_KEY:
            Matrix_Scale(0.1, 0.05, 0.1, MTXMODE_APPLY);
            Gfx_DrawDListOpa(play, (Gfx*)gSmallSmallKeyCrateDL);
            break;
        case ITEM_CATEGORY_BOSS_KEY:
            Matrix_Scale(0.1, 0.05, 0.1, MTXMODE_APPLY);
            Gfx_DrawDListOpa(play, (Gfx*)gSmallBossKeyCrateDL);
            break;
        case ITEM_CATEGORY_LESSER:
            Matrix_Scale(0.1, 0.05, 0.1, MTXMODE_APPLY);
            switch (smallCrateItem.itemId) {
                case ITEM_HEART_PIECE:
                case ITEM_HEART_PIECE_2:
                case ITEM_HEART_CONTAINER:
                    Gfx_DrawDListOpa(play, (Gfx*)gSmallHeartCrateDL);
                    break;
                default:
                    Gfx_DrawDListOpa(play, (Gfx*)gSmallMinorCrateDL);
                    break;
            }
        case ITEM_CATEGORY_JUNK:
        default:
            Matrix_Scale(0.04, 0.02, 0.04, MTXMODE_APPLY);
            Gfx_DrawDListOpa(play, (Gfx*)gLargeJunkCrateDL);
            break;
    }

    Matrix_Pop();
    CLOSE_DISPS(gfxCtx);
}

void EnWood02_RandomizerSpawnCollectible(EnWood02* treeActor, PlayState* play) {
    const auto treeIdentity = ObjectExtension::GetInstance().Get<TreeIdentity>(&treeActor->actor);
    if (treeIdentity == nullptr) {
        return;
    }

    EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &treeActor->actor.world.pos, ITEM00_SOH_DUMMY);
    item00->randoInf = treeIdentity->randomizerInf;
    item00->itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(treeIdentity->randomizerCheck, true, GI_NONE);
    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
    item00->actor.velocity.y = 0.0f;
    item00->actor.world.pos.y += 120.0f;
    item00->actor.speedXZ = 2.0f;
    item00->actor.world.rot.y = Rand_CenteredFloat(65536.0f);
    // clear randomizerCheck to prevent multiple bonks,
    // reloading area without collecting drop won't persist this
    treeIdentity->randomizerCheck = RC_UNKNOWN_CHECK;
}

void EnWood02_RandomizerInit(void* actorRef) {
    EnWood02* treeActor = static_cast<EnWood02*>(actorRef);
    if (treeActor->actor.params <= WOOD_TREE_KAKARIKO_ADULT) {
        auto treeIdentity = OTRGlobals::Instance->gRandomizer->IdentifyTree(
            gPlayState->sceneNum, (s16)treeActor->actor.world.pos.x, (s16)treeActor->actor.world.pos.z);
        ObjectExtension::GetInstance().Set<TreeIdentity>(actorRef, std::move(treeIdentity));
    }
}

void RegisterShuffleTrees() {
    bool shouldRegister = IS_RANDO && Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_TREES).Get();

    COND_ID_HOOK(OnActorInit, ACTOR_EN_WOOD02, shouldRegister, EnWood02_RandomizerInit);

    COND_VB_SHOULD(VB_TREE_SETUP_DRAW, shouldRegister, {
        EnWood02* treeActor = va_arg(args, EnWood02*);
        if (EnWood02_RandomizerHoldsItem(treeActor, gPlayState)) {
            EnWood02_RandomizerDraw(&treeActor->actor, gPlayState);
        }
    });

    COND_VB_SHOULD(VB_TREE_DROP_ITEM, shouldRegister, {
        EnWood02* treeActor = va_arg(args, EnWood02*);
        if (EnWood02_RandomizerHoldsItem(treeActor, gPlayState)) {
            EnWood02_RandomizerSpawnCollectible(treeActor, gPlayState);
            // QoL, drop golden skulltula alongside item
            if ((treeActor->unk_14C < 0 || treeActor->unk_14C >= 0x64) && treeActor->actor.home.rot.z != 0) {
                Vec3f dropsSpawnPt = treeActor->actor.world.pos;
                dropsSpawnPt.y += 200.0f;
                treeActor->actor.home.rot.z &= 0x1FFF;
                treeActor->actor.home.rot.z |= 0xE000;
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_SW, dropsSpawnPt.x, dropsSpawnPt.y,
                            dropsSpawnPt.z, 0, treeActor->actor.world.rot.y, 0, treeActor->actor.home.rot.z, true);
                treeActor->actor.home.rot.z = 0;
            }
            *should = false;
        }
    });
}

void Rando::StaticData::RegisterTreeLocations() {
    // clang-format off
    // Trees
    //            Randomizer Check                Randomizer Check                                       Quest         Area                          Scene ID                           Params                          Short Name                           Hint Text Key             Vanilla         Spoiler Collection Check
    locationTable[RC_MARKET_TREE]                 = Location::Tree(RC_MARKET_TREE,                       RCQUEST_BOTH, RCAREA_MARKET,                SCENE_MARKET_DAY,                  TWO_ACTOR_PARAMS(-100, 240),    "Tree in Hyrule Market",             RHT_TREE_MARKET,          RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_MARKET_TREE));
    locationTable[RC_HC_NEAR_GUARDS_TREE_1]       = Location::Tree(RC_HC_NEAR_GUARDS_TREE_1,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(1209, 2242),   "Tree Near Guards 1",                RHT_TREE_HYRULE_CASTLE,   RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_1));
    locationTable[RC_HC_NEAR_GUARDS_TREE_2]       = Location::Tree(RC_HC_NEAR_GUARDS_TREE_2,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(943, 2051),    "Tree Near Guards 2",                RHT_TREE_HYRULE_CASTLE,   RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_2));
    locationTable[RC_HC_NEAR_GUARDS_TREE_3]       = Location::Tree(RC_HC_NEAR_GUARDS_TREE_3,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(827, 1428),    "Tree Near Guards 3",                RHT_TREE_HYRULE_CASTLE,   RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_3));
    locationTable[RC_HC_NEAR_GUARDS_TREE_4]       = Location::Tree(RC_HC_NEAR_GUARDS_TREE_4,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(421, 1397),    "Tree Near Guards 4",                RHT_TREE_HYRULE_CASTLE,   RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_4));
    locationTable[RC_HC_NEAR_GUARDS_TREE_5]       = Location::Tree(RC_HC_NEAR_GUARDS_TREE_5,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(-73, 1459),    "Tree Near Guards 5",                RHT_TREE_HYRULE_CASTLE,   RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_5));
    locationTable[RC_HC_NEAR_GUARDS_TREE_6]       = Location::Tree(RC_HC_NEAR_GUARDS_TREE_6,             RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(1494, 2108),   "Tree Near Guards 6",                RHT_TREE_HYRULE_CASTLE,   RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NEAR_GUARDS_TREE_6));
    locationTable[RC_HC_SKULLTULA_TREE]           = Location::Tree(RC_HC_SKULLTULA_TREE,                 RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(-145, 2961),   "HC GS Tree",                        RHT_TREE_HYRULE_CASTLE,   RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_SKULLTULA_TREE));
    locationTable[RC_HC_GROTTO_TREE]              = Location::Tree(RC_HC_GROTTO_TREE,                    RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(924, 872),     "Tree Near Storms Grotto",           RHT_TREE_HYRULE_CASTLE,   RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_GROTTO_TREE));
    locationTable[RC_HC_NL_TREE_1]                = Location::NLTree(RC_HC_NL_TREE_1,                    RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(-331, 1438),   "NL Tree Near Guards 1",             RHT_TREE_HYRULE_CASTLE,   RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NL_TREE_1));
    locationTable[RC_HC_NL_TREE_2]                = Location::NLTree(RC_HC_NL_TREE_2,                    RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(1022, 1444),   "NL Tree Near Guards 2",             RHT_TREE_HYRULE_CASTLE,   RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_NL_TREE_2));
    locationTable[RC_HF_NEAR_KAK_TREE]            = Location::Tree(RC_HF_NEAR_KAK_TREE,                  RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(3276, 971),    "Tree Outside Kakariko",             RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NEAR_KAK_TREE));
    locationTable[RC_HF_NEAR_KAK_SMALL_TREE]      = Location::Tree(RC_HF_NEAR_KAK_SMALL_TREE,            RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(2076, -91),    "Small Tree Outside Kakariko",       RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NEAR_KAK_SMALL_TREE));
    locationTable[RC_HF_NEAR_MARKET_TREE_1]       = Location::Tree(RC_HF_NEAR_MARKET_TREE_1,             RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-1041, 1022),  "Tree Near HC Entrance Grotto 1",    RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NEAR_MARKET_TREE_1));
    locationTable[RC_HF_NEAR_MARKET_TREE_2]       = Location::Tree(RC_HF_NEAR_MARKET_TREE_2,             RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-1244, 819),   "Tree Near HC Entrance Grotto 2",    RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NEAR_MARKET_TREE_2));
    locationTable[RC_HF_NEAR_MARKET_TREE_3]       = Location::Tree(RC_HF_NEAR_MARKET_TREE_3,             RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-1448, 620),   "Tree Near HC Entrance Grotto 3",    RHT_TREE_HYRULE_FIELD,    RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NEAR_MARKET_TREE_3));
    locationTable[RC_HF_NEAR_LLR_TREE]            = Location::Tree(RC_HF_NEAR_LLR_TREE,                  RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-1907, 5409),  "Tree Outside Lon Lon Ranch",        RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NEAR_LLR_TREE));
    locationTable[RC_HF_NEAR_LH_TREE]             = Location::Tree(RC_HF_NEAR_LH_TREE,                   RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4377, 13662), "Tree Outside Lake Hylia",           RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NEAR_LH_TREE));
    locationTable[RC_HF_CHILD_NEAR_GV_TREE]       = Location::Tree(RC_HF_CHILD_NEAR_GV_TREE,             RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-6270, 8579),  "Child Near Gerudo Valley Tree",     RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_CHILD_NEAR_GV_TREE));
    locationTable[RC_HF_ADULT_NEAR_GV_TREE]       = Location::Tree(RC_HF_ADULT_NEAR_GV_TREE,             RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-6241, 7097),  "Adult Near Gerudo Valley Tree",     RHT_TREE_HYRULE_FIELD,    RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ADULT_NEAR_GV_TREE));
    locationTable[RC_HF_NEAR_ZR_TREE]             = Location::Tree(RC_HF_NEAR_ZR_TREE,                   RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(3117, 4239),   "Tree Outside Zora's River",         RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NEAR_ZR_TREE));
    locationTable[RC_HF_NORTHWEST_TREE_1]         = Location::Tree(RC_HF_NORTHWEST_TREE_1,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4777, 136),   "Tree in Northwest 1",               RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NORTHWEST_TREE_1));
    locationTable[RC_HF_NORTHWEST_TREE_2]         = Location::Tree(RC_HF_NORTHWEST_TREE_2,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4188, 263),   "Tree in Northwest 2",               RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NORTHWEST_TREE_2));
    locationTable[RC_HF_NORTHWEST_TREE_3]         = Location::Tree(RC_HF_NORTHWEST_TREE_3,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-5000, -147),  "Tree in Northwest 3",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NORTHWEST_TREE_3));
    locationTable[RC_HF_NORTHWEST_TREE_4]         = Location::Tree(RC_HF_NORTHWEST_TREE_4,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4463, -182),  "Tree in Northwest 4",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NORTHWEST_TREE_4));
    locationTable[RC_HF_NORTHWEST_TREE_5]         = Location::Tree(RC_HF_NORTHWEST_TREE_5,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-5262, 398),   "Tree in Northwest 5",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NORTHWEST_TREE_5));
    locationTable[RC_HF_NORTHWEST_TREE_6]         = Location::Tree(RC_HF_NORTHWEST_TREE_6,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4391, 891),   "Tree in Northwest 6",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_NORTHWEST_TREE_6));
    locationTable[RC_HF_EAST_TREE_1]              = Location::Tree(RC_HF_EAST_TREE_1,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(3817, 7119),   "Tree in East 1",                    RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_EAST_TREE_1));
    locationTable[RC_HF_EAST_TREE_2]              = Location::Tree(RC_HF_EAST_TREE_2,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(4365, 7182),   "Tree in East 2",                    RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_EAST_TREE_2));
    locationTable[RC_HF_EAST_TREE_3]              = Location::Tree(RC_HF_EAST_TREE_3,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(3837, 7479),   "Tree in East 3",                    RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_EAST_TREE_3));
    locationTable[RC_HF_EAST_TREE_4]              = Location::Tree(RC_HF_EAST_TREE_4,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(3377, 7201),   "Tree in East 4",                    RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_EAST_TREE_4));
    locationTable[RC_HF_EAST_TREE_5]              = Location::Tree(RC_HF_EAST_TREE_5,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(3408, 6676),   "Tree in East 5",                    RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_EAST_TREE_5));
    locationTable[RC_HF_EAST_TREE_6]              = Location::Tree(RC_HF_EAST_TREE_6,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(3935, 6279),   "Tree in East 6",                    RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_EAST_TREE_6));
    locationTable[RC_HF_SOUTHEAST_TREE_1]         = Location::Tree(RC_HF_SOUTHEAST_TREE_1,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(915, 12557),   "Tree in Southeast 1",               RHT_TREE_HYRULE_FIELD,    RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_1));
    locationTable[RC_HF_SOUTHEAST_TREE_2]         = Location::Tree(RC_HF_SOUTHEAST_TREE_2,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(350, 11605),   "Tree in Southeast 2",               RHT_TREE_HYRULE_FIELD,    RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_2));
    locationTable[RC_HF_SOUTHEAST_TREE_3]         = Location::Tree(RC_HF_SOUTHEAST_TREE_3,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(470, 12494),   "Tree in Southeast 3",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_3));
    locationTable[RC_HF_SOUTHEAST_TREE_4]         = Location::Tree(RC_HF_SOUTHEAST_TREE_4,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(614, 12357),   "Tree in Southeast 4",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_4));
    locationTable[RC_HF_SOUTHEAST_TREE_5]         = Location::Tree(RC_HF_SOUTHEAST_TREE_5,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(1114, 12156),  "Tree in Southeast 5",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_5));
    locationTable[RC_HF_SOUTHEAST_TREE_6]         = Location::Tree(RC_HF_SOUTHEAST_TREE_6,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(549, 11204),   "Tree in Southeast 6",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_6));
    locationTable[RC_HF_SOUTHEAST_TREE_7]         = Location::Tree(RC_HF_SOUTHEAST_TREE_7,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(49, 11405),    "Tree in Southeast 7",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_7));
    locationTable[RC_HF_SOUTHEAST_TREE_8]         = Location::Tree(RC_HF_SOUTHEAST_TREE_8,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-29, 12005),   "Tree in Southeast 8",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_8));
    locationTable[RC_HF_SOUTHEAST_TREE_9]         = Location::Tree(RC_HF_SOUTHEAST_TREE_9,               RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(1514,13157),   "Tree in Southeast 9",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_9));
    locationTable[RC_HF_SOUTHEAST_TREE_10]        = Location::Tree(RC_HF_SOUTHEAST_TREE_10,              RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-509,12954),   "Tree in Southeast 10",              RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_10));
    locationTable[RC_HF_SOUTHEAST_TREE_11]        = Location::Tree(RC_HF_SOUTHEAST_TREE_11,              RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-430,12354),   "Tree in Southeast 11",              RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_11));
    locationTable[RC_HF_SOUTHEAST_TREE_12]        = Location::Tree(RC_HF_SOUTHEAST_TREE_12,              RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(69,12153),     "Tree in Southeast 12",              RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_12));
    locationTable[RC_HF_SOUTHEAST_TREE_13]        = Location::Tree(RC_HF_SOUTHEAST_TREE_13,              RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-129,12554),   "Tree in Southeast 13",              RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_13));
    locationTable[RC_HF_SOUTHEAST_TREE_14]        = Location::Tree(RC_HF_SOUTHEAST_TREE_14,              RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(950,11545),    "Tree in Southeast 14",              RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_14));
    locationTable[RC_HF_SOUTHEAST_TREE_15]        = Location::Tree(RC_HF_SOUTHEAST_TREE_15,              RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(949,12205),    "Tree in Southeast 15",              RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_15));
    locationTable[RC_HF_SOUTHEAST_TREE_16]        = Location::Tree(RC_HF_SOUTHEAST_TREE_16,              RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(469,13154),    "Tree in Southeast 16",              RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_16));
    locationTable[RC_HF_SOUTHEAST_TREE_17]        = Location::Tree(RC_HF_SOUTHEAST_TREE_17,              RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(535,12957),    "Tree in Southeast 17",              RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_17));
    locationTable[RC_HF_SOUTHEAST_TREE_18]        = Location::Tree(RC_HF_SOUTHEAST_TREE_18,              RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(1515,12497),   "Tree in Southeast 18",              RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_18));
    locationTable[RC_HF_SOUTHEAST_TREE_19]        = Location::Tree(RC_HF_SOUTHEAST_TREE_19,              RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-786,11293),   "Tree in Southeast 19",              RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SOUTHEAST_TREE_19));
    locationTable[RC_HF_CHILD_SOUTHEAST_TREE_1]   = Location::Tree(RC_HF_CHILD_SOUTHEAST_TREE_1,         RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(1535, 11943),  "Child Tree in Southeast Corner 1",  RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_CHILD_SOUTHEAST_TREE_1));
    locationTable[RC_HF_CHILD_SOUTHEAST_TREE_2]   = Location::Tree(RC_HF_CHILD_SOUTHEAST_TREE_2,         RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(2135,11883),   "Child Tree in Southeast Corner 2",  RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_CHILD_SOUTHEAST_TREE_2));
    locationTable[RC_HF_CHILD_SOUTHEAST_TREE_3]   = Location::Tree(RC_HF_CHILD_SOUTHEAST_TREE_3,         RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(2134,12543),   "Child Tree in Southeast Corner 3",  RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_CHILD_SOUTHEAST_TREE_3));
    locationTable[RC_HF_CHILD_SOUTHEAST_TREE_4]   = Location::Tree(RC_HF_CHILD_SOUTHEAST_TREE_4,         RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(1734,11542),   "Child Tree in Southeast Corner 4",  RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_CHILD_SOUTHEAST_TREE_4));
    locationTable[RC_HF_CHILD_SOUTHEAST_TREE_5]   = Location::Tree(RC_HF_CHILD_SOUTHEAST_TREE_5,         RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(1234,11743),   "Child Tree in Southeast Corner 5",  RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_CHILD_SOUTHEAST_TREE_5));
    locationTable[RC_HF_CHILD_SOUTHEAST_TREE_6]   = Location::Tree(RC_HF_CHILD_SOUTHEAST_TREE_6,         RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(1155,12343),   "Child Tree in Southeast Corner 6",  RHT_TREE_HYRULE_FIELD,    RG_BOMBS_5,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_CHILD_SOUTHEAST_TREE_6));
    locationTable[RC_HF_TEKTITE_GROTTO_TREE]      = Location::Tree(RC_HF_TEKTITE_GROTTO_TREE,            RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4976, 2812),  "Tektite Grotto Tree",               RHT_TREE_HYRULE_FIELD,    RG_BLUE_RUPEE,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_TEKTITE_GROTTO_TREE));
    locationTable[RC_ZF_TREE]                     = Location::Tree(RC_ZF_TREE,                           RCQUEST_BOTH, RCAREA_ZORAS_FOUNTAIN,        SCENE_ZORAS_FOUNTAIN,              TWO_ACTOR_PARAMS(186, 2222),    "Tree in Zora's Fountain",           RHT_TREE_ZORAS_FOUNTAIN,  RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZF_TREE));
    locationTable[RC_ZR_TREE]                     = Location::Tree(RC_ZR_TREE,                           RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1690, 554),   "Tree in Zoras River",               RHT_TREE_ZORAS_RIVER,     RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_TREE));
    locationTable[RC_KAK_TREE]                    = Location::Tree(RC_KAK_TREE,                          RCQUEST_BOTH, RCAREA_KAKARIKO_VILLAGE,      SCENE_KAKARIKO_VILLAGE,            TWO_ACTOR_PARAMS(-860, 522),    "Kakariko GS Tree",                  RHT_TREE_KAKARIKO,        RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_TREE));
    locationTable[RC_LLR_TREE]                    = Location::Tree(RC_LLR_TREE,                          RCQUEST_BOTH, RCAREA_LON_LON_RANCH,         SCENE_LON_LON_RANCH,               TWO_ACTOR_PARAMS(1309, -2241),  "Lon Lon Ranch GS Tree",             RHT_TREE_LON_LON_RANCH,   RG_DEKU_NUTS_5, SpoilerCollectionCheck::RandomizerInf(RAND_INF_LLR_TREE));
    // clang-format on
}

static ObjectExtension::Register<TreeIdentity> RegisterPotIdentity;
static RegisterShipInitFunc registerShuffleTrees(RegisterShuffleTrees, { "IS_RANDO" });
static RegisterShipInitFunc registerTreeLocations(Rando::StaticData::RegisterTreeLocations);
