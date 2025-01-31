#include "ShuffleCrates.h"
#include "soh_assets.h"
#include "static_data.h"
#include <libultraship/libultra.h>
#include "global.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Kibako2/z_obj_kibako2.h"
#include "objects/object_kibako2/object_kibako2.h"
#include "overlays/actors/ovl_Obj_Kibako/z_obj_kibako.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/ResourceManagerHelpers.h"
extern PlayState* gPlayState;
}

#define RAND_GET_OPTION(option) Rando::Context::GetInstance()->GetOption(option).Get()

extern void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play);

extern "C" void ObjKibako2_RandomizerDraw(Actor* thisx, PlayState* play) {
    GetItemCategory getItemCategory;
    auto crateActor = ((ObjKibako2*)thisx);
    u8 hasCustomCrateDLs = 0;
    int csmc = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), CSMC_DISABLED);
    int requiresStoneAgony = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"), 0);

    if (ResourceMgr_FileIsCustomByName(gLargeCrateDL)) {
        hasCustomCrateDLs = 1;
        return;
    }

    int isVanilla = csmc == CSMC_DISABLED || (requiresStoneAgony && !CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY));

    GetItemEntry crateItem =
        Rando::Context::GetInstance()->GetFinalGIEntry(crateActor->crateIdentity.randomizerCheck, true, GI_NONE);
    getItemCategory = crateItem.getItemCategory;
   
    // If they have bombchus, don't consider the bombchu item major
    if (INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU &&
            ((crateItem.modIndex == MOD_RANDOMIZER && crateItem.getItemId == RG_PROGRESSIVE_BOMBCHUS) ||
             (crateItem.modIndex == MOD_NONE && 
                 (crateItem.getItemId == GI_BOMBCHUS_5 || crateItem.getItemId == GI_BOMBCHUS_10 ||
           crateItem.getItemId == GI_BOMBCHUS_20)))) {
        getItemCategory = ITEM_CATEGORY_JUNK;
        // If it's a bottle and they already have one, consider the item lesser
    } else if ((crateItem.modIndex == MOD_RANDOMIZER && crateItem.getItemId >= RG_BOTTLE_WITH_RED_POTION && crateItem.getItemId <= RG_BOTTLE_WITH_POE) ||
             (crateItem.modIndex == MOD_NONE && (crateItem.getItemId == GI_BOTTLE || crateItem.getItemId == GI_MILK_BOTTLE))) {
        if (gSaveContext.inventory.items[SLOT_BOTTLE_1] != ITEM_NONE) {
            getItemCategory = ITEM_CATEGORY_LESSER;
        }
    }

        // Change texture
        // TODO: add heart texture when supported gLargeHeartCrateDL
    if (!isVanilla && !hasCustomCrateDLs &&
        (csmc == CSMC_BOTH || csmc == CSMC_TEXTURE)) {
        switch (getItemCategory) {
            case ITEM_CATEGORY_MAJOR:
                Gfx_DrawDListOpa(play, (Gfx*)gLargeMajorCrateDL);
                break;
            case ITEM_CATEGORY_SKULLTULA_TOKEN:
                Gfx_DrawDListOpa(play, (Gfx*)gLargeTokenCrateDL);
                break;
            case ITEM_CATEGORY_SMALL_KEY:
                Gfx_DrawDListOpa(play, (Gfx*)gLargeSmallKeyCrateDL);
                break;
            case ITEM_CATEGORY_BOSS_KEY:
                Gfx_DrawDListOpa(play, (Gfx*)gLargeBossKeyCrateDL);
                break;
            case ITEM_CATEGORY_LESSER:
                Gfx_DrawDListOpa(play, (Gfx*)gLargeMinorCrateDL);
                break;
            case ITEM_CATEGORY_JUNK:
            default:
                Gfx_DrawDListOpa(play, (Gfx*)gLargeJunkCrateDL);
                break;
        }
    } else {
        Gfx_DrawDListOpa(play, (Gfx*)gLargeRandoCrateDL);
    }
}

extern "C" void ObjKibako_RandomizerDraw(Actor* thisx, PlayState* play) {
    GetItemCategory getItemCategory;
    auto smallCrateActor = ((ObjKibako*)thisx);
    u8 hasCustomSmallCrateDLs = 0;
    int csmc = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), CSMC_DISABLED);
    int requiresStoneAgony = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"), 0);

    if (ResourceMgr_FileIsCustomByName(gSmallWoodenBoxDL)) {
        hasCustomSmallCrateDLs = 1;
        return;
    }

    int isVanilla = csmc == CSMC_DISABLED || (requiresStoneAgony && !CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY));

    GetItemEntry smallCrateItem = Rando::Context::GetInstance()->GetFinalGIEntry(smallCrateActor->smallCrateIdentity.randomizerCheck, true, GI_NONE);
    getItemCategory = smallCrateItem.getItemCategory;

    if (INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU &&
        ((smallCrateItem.modIndex == MOD_RANDOMIZER && smallCrateItem.getItemId == RG_PROGRESSIVE_BOMBCHUS) ||
         (smallCrateItem.modIndex == MOD_NONE &&
          (smallCrateItem.getItemId == GI_BOMBCHUS_5 || smallCrateItem.getItemId == GI_BOMBCHUS_10 || smallCrateItem.getItemId == GI_BOMBCHUS_20)))) {
        getItemCategory = ITEM_CATEGORY_JUNK;
        // If it's a bottle and they already have one, consider the item lesser
    } else if ((smallCrateItem.modIndex == MOD_RANDOMIZER && smallCrateItem.getItemId >= RG_BOTTLE_WITH_RED_POTION &&
                smallCrateItem.getItemId <= RG_BOTTLE_WITH_POE) ||
               (smallCrateItem.modIndex == MOD_NONE && (smallCrateItem.getItemId == GI_BOTTLE || smallCrateItem.getItemId == GI_MILK_BOTTLE))) {
        if (gSaveContext.inventory.items[SLOT_BOTTLE_1] != ITEM_NONE) {
            getItemCategory = ITEM_CATEGORY_LESSER;
        }
    }

    // Change texture
    // TODO: add heart texture when supported gSmallHeartCrateDL
    if (!isVanilla && !hasCustomSmallCrateDLs && (csmc == CSMC_BOTH || csmc == CSMC_TEXTURE)) {
        switch (getItemCategory) {
            case ITEM_CATEGORY_MAJOR:
                Gfx_DrawDListOpa(play, (Gfx*)gSmallMajorCrateDL);
                break;
            case ITEM_CATEGORY_SKULLTULA_TOKEN:
                Gfx_DrawDListOpa(play, (Gfx*)gSmallTokenCrateDL);
                break;
            case ITEM_CATEGORY_SMALL_KEY:
                Gfx_DrawDListOpa(play, (Gfx*)gSmallSmallKeyCrateDL);
                break;
            case ITEM_CATEGORY_BOSS_KEY:
                Gfx_DrawDListOpa(play, (Gfx*)gSmallBossKeyCrateDL);
                break;
            case ITEM_CATEGORY_LESSER:
                Gfx_DrawDListOpa(play, (Gfx*)gSmallMinorCrateDL);
                break;
            case ITEM_CATEGORY_JUNK:
            default:
                Gfx_DrawDListOpa(play, (Gfx*)gSmallJunkCrateDL);
                break;
        }
    } else {
        Gfx_DrawDListOpa(play, (Gfx*)gSmallRandoCrateDL);
    }
}

bool GetOverworldCratesIncluded(Rando::Location* loc) {
    uint8_t logicSetting = Rando::Context::GetInstance()->GetOption(RSK_LOGIC_RULES).Get();
    uint8_t crateSetting = Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_CRATES).Get();

    if (loc->GetRCType() != RCTYPE_CRATE || crateSetting == RO_SHUFFLE_CRATES_OFF) {
        return false;
    }

    RandomizerCheck rc = loc->GetRandomizerCheck();
    
    // Remove child GV/GF crates from check tracker when not shuffled
    if (logicSetting != RO_LOGIC_NO_LOGIC && (rc == RC_GV_CRATE_BRIDGE_1 || rc == RC_GV_CRATE_BRIDGE_2 || rc == RC_GV_CRATE_BRIDGE_3 ||
        rc == RC_GV_CRATE_BRIDGE_4 || rc == RC_GF_ARCHERY_LEFT_END_CHILD_CRATE)) {
        return false;
    }
    return true;
}

uint8_t ObjKibako2_RandomizerHoldsItem(ObjKibako2* crateActor, PlayState* play) {
    RandomizerCheck rc = crateActor->crateIdentity.randomizerCheck;
    uint8_t isDungeon = Rando::StaticData::GetLocation(rc)->IsDungeon();
    uint8_t crateSetting = Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_CRATES).Get();

    // Don't pull randomized item if crate isn't randomized or is already checked
    if (!IS_RANDO || (crateSetting == RO_SHUFFLE_CRATES_OVERWORLD && isDungeon) ||
        (crateSetting == RO_SHUFFLE_CRATES_DUNGEONS && !isDungeon) ||
        Flags_GetRandomizerInf(crateActor->crateIdentity.randomizerInf) ||
        crateActor->crateIdentity.randomizerCheck == RC_UNKNOWN_CHECK) {
        return false;
    } else {
        return true;
    }
}

uint8_t ObjKibako_RandomizerHoldsItem(ObjKibako* smallCrateActor, PlayState* play) {
    RandomizerCheck rc = smallCrateActor->smallCrateIdentity.randomizerCheck;
    uint8_t isDungeon = Rando::StaticData::GetLocation(rc)->IsDungeon();
    uint8_t crateSetting = Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_CRATES).Get();

    // Don't pull randomized item if crate isn't randomized or is already checked
    if (!IS_RANDO || (crateSetting == RO_SHUFFLE_CRATES_OVERWORLD && isDungeon) ||
        (crateSetting == RO_SHUFFLE_CRATES_DUNGEONS && !isDungeon) ||
        Flags_GetRandomizerInf(smallCrateActor->smallCrateIdentity.randomizerInf) ||
        smallCrateActor->smallCrateIdentity.randomizerCheck == RC_UNKNOWN_CHECK) {
        return false;
    } else {
        return true;
    }
}

void ObjKibako2_RandomizerSpawnCollectible(ObjKibako2* crateActor, PlayState* play) {
    EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &crateActor->dyna.actor.world.pos, ITEM00_SOH_DUMMY);
    item00->randoInf = crateActor->crateIdentity.randomizerInf;
    item00->itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(crateActor->crateIdentity.randomizerCheck, true, GI_NONE);
    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
    item00->actor.velocity.y = 8.0f;
    item00->actor.speedXZ = 2.0f;
    item00->actor.world.rot.y = Rand_CenteredFloat(65536.0f);
}

void ObjKibako_RandomizerSpawnCollectible(ObjKibako* smallCrateActor, PlayState* play) {
    EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &smallCrateActor->actor.world.pos, ITEM00_SOH_DUMMY);
    item00->randoInf = smallCrateActor->smallCrateIdentity.randomizerInf;
    item00->itemEntry =
        Rando::Context::GetInstance()->GetFinalGIEntry(smallCrateActor->smallCrateIdentity.randomizerCheck, true, GI_NONE);
    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
    item00->actor.velocity.y = 8.0f;
    item00->actor.speedXZ = 2.0f;
    item00->actor.world.rot.y = Rand_CenteredFloat(65536.0f);
}

void ObjKibako2_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);
    uint8_t logicSetting = Rando::Context::GetInstance()->GetOption(RSK_LOGIC_RULES).Get();

    // don't shuffle two OOB crates in GF and don't shuffle child GV/GF crates when not in no logic
    if (actor->id != ACTOR_OBJ_KIBAKO2 ||
        (gPlayState->sceneNum == SCENE_GERUDOS_FORTRESS && (s16)actor->world.pos.x == -4051 && (s16)actor->world.pos.z == -3429) ||
        (gPlayState->sceneNum == SCENE_GERUDOS_FORTRESS && (s16)actor->world.pos.x == -4571 && (s16)actor->world.pos.z == -3429) ||
        (logicSetting != RO_LOGIC_NO_LOGIC &&
        ((gPlayState->sceneNum == SCENE_GERUDOS_FORTRESS && (s16)actor->world.pos.x == 3443 && (s16)actor->world.pos.z == -4876) ||
        (gPlayState->sceneNum == SCENE_GERUDO_VALLEY && (s16)actor->world.pos.x == -764 && (s16)actor->world.pos.z == 148) || 
        (gPlayState->sceneNum == SCENE_GERUDO_VALLEY && (s16)actor->world.pos.x == -860 && (s16)actor->world.pos.z == -125) ||
        (gPlayState->sceneNum == SCENE_GERUDO_VALLEY && (s16)actor->world.pos.x == -860 && (s16)actor->world.pos.z == -150) || 
        (gPlayState->sceneNum == SCENE_GERUDO_VALLEY && (s16)actor->world.pos.x == -860 && (s16)actor->world.pos.z == -90))))
        return;

    ObjKibako2* crateActor = static_cast<ObjKibako2*>(actorRef);

    crateActor->crateIdentity = OTRGlobals::Instance->gRandomizer->IdentifyCrate(gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z);
}

void ObjKibako_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    if (actor->id != ACTOR_OBJ_KIBAKO) return;

    ObjKibako* smallCrateActor = static_cast<ObjKibako*>(actorRef);

    smallCrateActor->smallCrateIdentity = OTRGlobals::Instance->gRandomizer->IdentifySmallCrate(gPlayState->sceneNum, (s16)actor->home.pos.x, (s16)actor->home.pos.z);
}

void RegisterShuffleCrates() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_CRATES);

    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_KIBAKO2, shouldRegister, ObjKibako2_RandomizerInit);

    COND_VB_SHOULD(VB_CRATE_SETUP_DRAW, shouldRegister, {
        ObjKibako2* crateActor = va_arg(args, ObjKibako2*);
        if (ObjKibako2_RandomizerHoldsItem(crateActor, gPlayState)) {
            crateActor->dyna.actor.draw = (ActorFunc)ObjKibako2_RandomizerDraw;
            *should = false;
        } else {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_CRATE_DROP_ITEM, shouldRegister, {
        ObjKibako2* crateActor = va_arg(args, ObjKibako2*);
        if (ObjKibako2_RandomizerHoldsItem(crateActor, gPlayState)) {
            ObjKibako2_RandomizerSpawnCollectible(crateActor, gPlayState);
            *should = false;
        } else {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_KIBAKO, shouldRegister, ObjKibako_RandomizerInit);

    COND_VB_SHOULD(VB_SMALL_CRATE_SETUP_DRAW, shouldRegister, {
        ObjKibako* smallCrateActor = va_arg(args, ObjKibako*);
        if (ObjKibako_RandomizerHoldsItem(smallCrateActor, gPlayState)) {
            smallCrateActor->actor.draw = (ActorFunc)ObjKibako_RandomizerDraw;
            *should = false;
        } else {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_SMALL_CRATE_DROP_ITEM, shouldRegister, {
        ObjKibako* smallCrateActor = va_arg(args, ObjKibako*);
        if (ObjKibako_RandomizerHoldsItem(smallCrateActor, gPlayState)) {
            ObjKibako_RandomizerSpawnCollectible(smallCrateActor, gPlayState);
            *should = false;
        } else {
            *should = true;
        }
    });

}

static RegisterShipInitFunc initFunc(RegisterShuffleCrates, { "IS_RANDO" });