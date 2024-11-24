#include "ShufflePots.h"
#include "soh_assets.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"

u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);
GetItemEntry Randomizer_GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId);
PotIdentity Randomizer_IdentifyPot(s32 sceneNum, s32 posX, s32 posZ);
extern PlayState* gPlayState;
}


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
    uint8_t isDungeon =
        play->sceneNum < SCENE_GANONS_TOWER_COLLAPSE_INTERIOR ||
        (play->sceneNum > SCENE_TREASURE_BOX_SHOP && play->sceneNum < SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR);
    uint8_t potSetting = Randomizer_GetSettingValue(RSK_SHUFFLE_POTS);

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
    item00->itemEntry =
        OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(potActor->potIdentity.randomizerCheck, GI_NONE);
    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
    item00->actor.velocity.y = 8.0f;
    item00->actor.speedXZ = 2.0f;
    item00->actor.world.rot.y = Rand_CenteredFloat(65536.0f);
}

void ObjTsubo_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    if (actor->id != ACTOR_OBJ_TSUBO) return;

    ObjTsubo* potActor = static_cast<ObjTsubo*>(actorRef);

    potActor->potIdentity =
        Randomizer_IdentifyPot(gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z);
}

void ShufflePots_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    // Draw custom model for pot to indicate it holding a randomized item.
    if (id == VB_POT_DRAW) {
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
    if (id == VB_LOCK_DOOR) {
        DoorShutter* doorActor = va_arg(args, DoorShutter*);
        uint8_t shufflePotSetting = Randomizer_GetSettingValue(RSK_SHUFFLE_POTS);
        if (gPlayState->sceneNum == SCENE_GANONS_TOWER && doorActor->dyna.actor.world.pos.y == 800 &&
            (shufflePotSetting == RO_SHUFFLE_POTS_DUNGEONS || shufflePotSetting == RO_SHUFFLE_POTS_ALL)) {
            *should = false;
        }
    }

    va_end(args);
}

void RegisterShufflePots() {
    static uint32_t onActorInitHook = 0;
    static uint32_t onVanillaBehaviorHook = 0;

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {

        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(onActorInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(onVanillaBehaviorHook);

        onActorInitHook = 0;
        onVanillaBehaviorHook = 0;

        if (!IS_RANDO) return;
        if (!Randomizer_GetSettingValue(RSK_SHUFFLE_POTS)) return;

        onActorInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(ObjTsubo_RandomizerInit);
        onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(ShufflePots_OnVanillaBehaviorHandler);

    });
}
