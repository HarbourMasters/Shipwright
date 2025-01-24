#include "ShuffleCrates.h"
#include "soh_assets.h"
#include "static_data.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Kibako2/z_obj_kibako2.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/object_kibako2/object_kibako2.h"
extern PlayState* gPlayState;
}

extern void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play);


extern "C" void ObjKibako2_RandomizerDraw(Actor* thisx, PlayState* play) {
    static Gfx* dList = (Gfx*)gLargeCrateDL;
    auto crateActor = ((ObjKibako2*)thisx);

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gDPSetGrayscaleColor(POLY_OPA_DISP++, 175, 255, 0, 255);

    if (Flags_GetRandomizerInf(crateActor->crateIdentity.randomizerInf) == 0) {
        gSPGrayscale(POLY_OPA_DISP++, true);
    }

    Gfx_DrawDListOpa(play, dList);

    gSPGrayscale(POLY_OPA_DISP++, false);

    CLOSE_DISPS(play->state.gfxCtx);
}

uint8_t ObjKibako2_RandomizerHoldsItem(ObjKibako2* crateActor, PlayState* play) {
    RandomizerCheck rc = crateActor->crateIdentity.randomizerCheck;
    uint8_t isDungeon = Rando::StaticData::GetLocation(rc)->IsDungeon();
    uint8_t crateSetting = Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_CRATES).GetContextOptionIndex();

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

void ObjKibako2_RandomizerSpawnCollectible(ObjKibako2* crateActor, PlayState* play) {
    EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &crateActor->dyna.actor.world.pos, ITEM00_SOH_DUMMY);
    item00->randoInf = crateActor->crateIdentity.randomizerInf;
    item00->itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(crateActor->crateIdentity.randomizerCheck, true, GI_NONE);
    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
    item00->actor.velocity.y = 8.0f;
    item00->actor.speedXZ = 2.0f;
    item00->actor.world.rot.y = Rand_CenteredFloat(65536.0f);
}

void ObjKibako2_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    if (actor->id != ACTOR_OBJ_KIBAKO2 || 
        (gPlayState->sceneNum == SCENE_GERUDOS_FORTRESS && (s16)actor->world.pos.x == -4051 && (s16)actor->world.pos.z == -3429) ||
        (gPlayState->sceneNum == SCENE_GERUDOS_FORTRESS && (s16)actor->world.pos.x == -4571 && (s16)actor->world.pos.z == -3429))
        return;

    ObjKibako2* crateActor = static_cast<ObjKibako2*>(actorRef);

    crateActor->crateIdentity = OTRGlobals::Instance->gRandomizer->IdentifyCrate(gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z);
}

void ShuffleCrates_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    // Draw custom model for crates to indicate it holding a randomized item.
    if (id == VB_CRATE_SETUP_DRAW) {
        ObjKibako2* crateActor = va_arg(args, ObjKibako2*);
        if (ObjKibako2_RandomizerHoldsItem(crateActor, gPlayState)) {
            crateActor->dyna.actor.draw = (ActorFunc)ObjKibako2_RandomizerDraw;
            *should = false;
        } else {
            *should = true;
        }
    }

    // Do not spawn vanilla item from crates, instead spawn the randomized item.
    if (id == VB_CRATE_DROP_ITEM) {
        ObjKibako2* crateActor = va_arg(args, ObjKibako2*);
        if (ObjKibako2_RandomizerHoldsItem(crateActor, gPlayState)) {
            ObjKibako2_RandomizerSpawnCollectible(crateActor, gPlayState);
            *should = false;
        } else {
            *should = true;
        }
    }

    va_end(args);
}