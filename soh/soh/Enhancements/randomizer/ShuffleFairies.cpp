#include "ShuffleFairies.h"
#include "randomizer_grotto.h"
#include "draw.h"
#include "src/overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "src/overlays/actors/ovl_Obj_Bean/z_obj_bean.h"
#include "src/overlays/actors/ovl_En_Gs/z_en_gs.h"
#include "../../OTRGlobals.h"
#include "../../cvar_prefixes.h"

#define FAIRY_FLAG_TIMED (1 << 8)

void ShuffleFairies_DrawRandomizedItem(EnElf* enElf, PlayState* play) {
    GetItemEntry randoGetItem = enElf->sohFairyIdentity.itemEntry;
    if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
        randoGetItem = GET_ITEM_MYSTERY;
    }
    Matrix_Push();
    Matrix_Scale(37.5, 37.5, 37.5, MTXMODE_APPLY);
    EnItem00_CustomItemsParticles(&enElf->actor, play, randoGetItem);
    GetItemEntry_Draw(play, randoGetItem);
    Matrix_Pop();
}

bool ShuffleFairies_FairyExists(FairyIdentity fairyIdentity) {
    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;

    while (actor != NULL) {
        if (actor->id != ACTOR_EN_ELF) {
            actor = actor->next;
        } else {
            EnElf* enElf = (EnElf*)(actor);
            if (fairyIdentity.randomizerInf == enElf->sohFairyIdentity.randomizerInf) {
                return true;
            }
            actor = actor->next;
        }
    }

    return false;
}

FairyIdentity ShuffleFairies_GetFairyIdentity(int32_t params) {
    FairyIdentity fairyIdentity;
    s16 sceneNum = gPlayState->sceneNum;
    fairyIdentity.randomizerInf = RAND_INF_MAX;

    if (sceneNum == SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT || sceneNum == SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS) {
        sceneNum = SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY;
    }

    Rando::Location* location = OTRGlobals::Instance->gRandomizer->GetCheckObjectFromActor(ACTOR_EN_ELF, sceneNum, params);

    if (location->GetRandomizerCheck() == RC_UNKNOWN_CHECK) {
        LUSLOG_WARN("FairyGetIdentity did not receive a valid RC value (%d).", location->GetRandomizerCheck());
        assert(false);
    } else {
        fairyIdentity.randomizerInf = static_cast<RandomizerInf>(location->GetCollectionCheck().flag);
        fairyIdentity.itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(location->GetRandomizerCheck(), true, GI_FAIRY);
    }

    return fairyIdentity;
}

bool ShuffleFairies_SpawnFairy(f32 posX, f32 posY, f32 posZ, int32_t params) {
    FairyIdentity fairyIdentity = ShuffleFairies_GetFairyIdentity(params);
    if (!Flags_GetRandomizerInf(fairyIdentity.randomizerInf)) {
        EnElf* fairy = (EnElf*)Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_ELF, posX, posY - 30.0f, posZ, 0,
                                           0, 0, FAIRY_HEAL, true);
        fairy->sohFairyIdentity = fairyIdentity;
        fairy->actor.draw = (ActorFunc)ShuffleFairies_DrawRandomizedItem;
        return true;
    }
    return false;
}

void ShuffleFairies_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    Actor* actor = va_arg(args, Actor*);

    va_end(args);

    // Grant item when picking up fairy. If randomized, disable healing effect.
    if (id == VB_FAIRY_HEAL) {
        EnElf* enElf = (EnElf*)(actor);
        if (enElf->sohFairyIdentity.randomizerInf && enElf->sohFairyIdentity.randomizerInf != RAND_INF_MAX) {
            Flags_SetRandomizerInf(enElf->sohFairyIdentity.randomizerInf);
        }
    // Spawn fairies in fairy fountains
    } else if (id == VB_SPAWN_FOUNTAIN_FAIRIES) {
        bool fairySpawned = false;
        s16 grottoId = (gPlayState->sceneNum == SCENE_FAIRYS_FOUNTAIN) ? Grotto_CurrentGrotto() : 0;
        for (s16 index = 0; index < 8; index++) {
            int32_t params = (grottoId << 8) | index;
            if (ShuffleFairies_SpawnFairy(actor->world.pos.x, actor->world.pos.y, actor->world.pos.z,
                params)) {
                fairySpawned = true;
            }
        }
        if (fairySpawned) {
            *should = false;
        }
    // Spawn 3 fairies when playing Song of Storms next to a planted bean
    } else if (id == VB_SPAWN_BEAN_STALK_FAIRIES) {
        ObjBean* objBean = (ObjBean*)(actor);
        bool fairySpawned = false;
        for (s16 index = 0; index < 3; index++) {
            int32_t params = ((objBean->dyna.actor.params & 0x3F) << 8) | index;
            if (ShuffleFairies_SpawnFairy(objBean->dyna.actor.world.pos.x, objBean->dyna.actor.world.pos.y,
                objBean->dyna.actor.world.pos.z,
                params)) {
                fairySpawned = true;
            }
        }
        if (fairySpawned) {
            *should = false;
        }
    // Handle playing both misc songs and song of storms in front of a gossip stone.
    } else if (id == VB_SPAWN_GOSSIP_STONE_FAIRY) {
        EnGs* gossipStone = (EnGs*)(actor);

        // If not any of the songs that normally spawn a fairy, mimic vanilla behaviour.
        if (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_01) {
            Player* player = GET_PLAYER(gPlayState);
            player->stateFlags2 |= PLAYER_STATE2_NEAR_OCARINA_ACTOR;
            return;
        } else if (gPlayState->msgCtx.unk_E3F2 != OCARINA_SONG_LULLABY &&
                   gPlayState->msgCtx.unk_E3F2 != OCARINA_SONG_SARIAS &&
                   gPlayState->msgCtx.unk_E3F2 != OCARINA_SONG_EPONAS &&
                   gPlayState->msgCtx.unk_E3F2 != OCARINA_SONG_SUNS &&
                   gPlayState->msgCtx.unk_E3F2 != OCARINA_SONG_TIME &&
                   gPlayState->msgCtx.unk_E3F2 != OCARINA_SONG_STORMS &&
                   gPlayState->msgCtx.ocarinaMode != OCARINA_MODE_04) {
            return;
        }

        int32_t params = (gPlayState->sceneNum == SCENE_GROTTOS) ? Grotto_CurrentGrotto() : 0;
        // Distinguish storms fairies from the normal song fairies
        if (gPlayState->msgCtx.unk_E3F2 == OCARINA_SONG_STORMS) {
            params |= 0x1000;
        }

        // Combine actor + song params with position to get the right randomizer check
        params = TWO_ACTOR_PARAMS(params, (int32_t)gossipStone->actor.world.pos.z);

        // Check if a fairy already exists with the same identity as the stone is trying to spawn.
        // Because the gossip stone code runs several times after playing the song, we need to
        // stop spawning the vanilla fairy as well when these fairies exist, otherwise both
        // the randomized and the vanilla fairy will spawn. When the randomized fairy is already
        // collected, the vanilla code will handle that part automatically.
        FairyIdentity fairyIdentity = ShuffleFairies_GetFairyIdentity(params);
        if (!ShuffleFairies_FairyExists(fairyIdentity)) {
            if (ShuffleFairies_SpawnFairy(gossipStone->actor.world.pos.x, gossipStone->actor.world.pos.y,
                gossipStone->actor.world.pos.z, params)) {
                Audio_PlayActorSound2(&gossipStone->actor, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                // Set vanilla check for fairy spawned so it doesn't spawn the vanilla fairy afterwards as well.
                gossipStone->unk_19D = 0;
                *should = false;
            }
        } else {
            *should = false;
        }
    }
}

uint32_t onVanillaBehaviorHook = 0;

void ShuffleFairies_RegisterHooks() {
    onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(ShuffleFairies_OnVanillaBehaviorHandler);
}

void ShuffleFairies_UnregisterHooks() {
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(onVanillaBehaviorHook);

    onVanillaBehaviorHook = 0;
}