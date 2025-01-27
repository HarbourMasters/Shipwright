#include "ShufflePots.h"
#include "soh_assets.h"
#include "static_data.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
extern PlayState* gPlayState;
}

extern void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play);


extern "C" void ObjTsubo_RandomizerDraw(Actor* thisx, PlayState* play) {
    float potSize = 1.0f;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_Scale(potSize, potSize, potSize, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                G_MTX_MODELVIEW | G_MTX_LOAD);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gRandoPotDL);
    CLOSE_DISPS(play->state.gfxCtx);
}

uint8_t ObjTsubo_RandomizerHoldsItem(ObjTsubo* potActor, PlayState* play) {
    RandomizerCheck rc = potActor->potIdentity.randomizerCheck;
    uint8_t isDungeon = Rando::StaticData::GetLocation(rc)->IsDungeon();
    uint8_t potSetting = Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_POTS).Get();

    // Don't pull randomized item if pot isn't randomized or is already checked
    if (!IS_RANDO || (potSetting == RO_SHUFFLE_POTS_OVERWORLD && isDungeon) ||
        (potSetting == RO_SHUFFLE_POTS_DUNGEONS && !isDungeon) ||
        Flags_GetRandomizerInf(potActor->potIdentity.randomizerInf) ||
        potActor->potIdentity.randomizerCheck == RC_UNKNOWN_CHECK) {
        return false;
    } else {
        return true;
    }
}

void ObjTsubo_RandomizerSpawnCollectible(ObjTsubo* potActor, PlayState* play) {
    EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &potActor->actor.world.pos, ITEM00_SOH_DUMMY);
    item00->randoInf = potActor->potIdentity.randomizerInf;
    item00->itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(potActor->potIdentity.randomizerCheck, true, GI_NONE);
    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
    item00->actor.velocity.y = 8.0f;
    item00->actor.speedXZ = 2.0f;
    item00->actor.world.rot.y = Rand_CenteredFloat(65536.0f);
}

void ObjTsubo_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    // Check for Lake Hylia specifically because the game spawns 2 pots out of bounds there for some reason.
    if (actor->id != ACTOR_OBJ_TSUBO || gPlayState->sceneNum == SCENE_LAKE_HYLIA || gPlayState->sceneNum == SCENE_HYRULE_CASTLE) return;

    ObjTsubo* potActor = static_cast<ObjTsubo*>(actorRef);

    potActor->potIdentity = OTRGlobals::Instance->gRandomizer->IdentifyPot(gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z);
}

void ShufflePots_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    // Draw custom model for pot to indicate it holding a randomized item.
    if (id == VB_POT_SETUP_DRAW) {
        ObjTsubo* potActor = va_arg(args, ObjTsubo*);
        if (ObjTsubo_RandomizerHoldsItem(potActor, gPlayState)) {
            potActor->actor.draw = (ActorFunc)ObjTsubo_RandomizerDraw;
            *should = false;
        }
    }

    // Do not spawn vanilla item from pot, instead spawn the ranomized item.
    if (id == VB_POT_DROP_ITEM) {
        ObjTsubo* potActor = va_arg(args, ObjTsubo*);
        if (ObjTsubo_RandomizerHoldsItem(potActor, gPlayState)) {
            ObjTsubo_RandomizerSpawnCollectible(potActor, gPlayState);
            *should = false;
        }
    }

    // Unlock early Ganon's Boss Key doors to allow access to the pots there when pots are shuffled in dungeon
    if (id == VB_LOCK_BOSS_DOOR) {
        DoorShutter* doorActor = va_arg(args, DoorShutter*);
        uint8_t shufflePotSetting = Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_POTS).Get();
        if (gPlayState->sceneNum == SCENE_GANONS_TOWER && doorActor->dyna.actor.world.pos.y == 800 &&
            (shufflePotSetting == RO_SHUFFLE_POTS_DUNGEONS || shufflePotSetting == RO_SHUFFLE_POTS_ALL)) {
            *should = false;
        }
    }

    va_end(args);
}
