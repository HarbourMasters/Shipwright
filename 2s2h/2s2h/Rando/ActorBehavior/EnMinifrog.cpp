#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Minifrog/z_en_minifrog.h"
void EnMinifrog_TurnToPlayer(EnMinifrog* enMinifrog);
void EnMinifrog_Jump(EnMinifrog* enMinifrog);
void EnMinifrog_JumpTimer(EnMinifrog* enMinifrog);
void EnMinifrog_SpawnDust(EnMinifrog* enMinifrog, PlayState* play);
}

#define SHUFFLED_FROGS (IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_FROGS])

static u16 sIsFrogReturnedFlags[] = {
    0,                  // FROG_YELLOW
    WEEKEVENTREG_32_40, // FROG_CYAN
    WEEKEVENTREG_32_80, // FROG_PINK
    WEEKEVENTREG_33_01, // FROG_BLUE
    WEEKEVENTREG_33_02, // FROG_WHITE
};

RandoCheckId GetFrogCheck(s16 index) {
    switch (index) {
        case 1:
            return RC_WOODFALL_TEMPLE_GEKKO_FROG;
        case 2:
            return RC_GREAT_BAY_TEMPLE_GEKKO_FROG;
        case 3:
            return RC_SOUTHERN_SWAMP_FROG;
        case 4:
            return RC_CLOCK_TOWN_LAUNDRY_FROG;
    }
    return RC_UNKNOWN;
}

// Custom func to make the frog give the item and vanish, without using a cutscene
void MiniFrog_TalkAndVanish(EnMinifrog* enMinifrog, PlayState* play) {
    EnMinifrog_TurnToPlayer(enMinifrog);
    EnMinifrog_Jump(enMinifrog);
    EnMinifrog_JumpTimer(enMinifrog);
    if (!play->msgCtx.currentTextId) {
        EnMinifrog_SpawnDust(enMinifrog, play);
        SoundSource_PlaySfxAtFixedWorldPos(play, &enMinifrog->actor.world.pos, 30, NA_SE_EN_NPC_FADEAWAY);
        Actor_Kill(&enMinifrog->actor);
        RandoCheckId frogCheck = GetFrogCheck(enMinifrog->frogIndex);
        RANDO_SAVE_CHECKS[frogCheck].eligible = true;
    }
}

void MiniFrog_IdleWithoutCs(EnMinifrog* enMinifrog, PlayState* play) {
    EnMinifrog_TurnToPlayer(enMinifrog);
    EnMinifrog_Jump(enMinifrog);
    EnMinifrog_JumpTimer(enMinifrog);
    if (Actor_TalkOfferAccepted(&enMinifrog->actor, &play->state)) {
        play->msgCtx.currentTextId = enMinifrog->actor.textId;
        enMinifrog->actionFunc = MiniFrog_TalkAndVanish;
    } else if ((enMinifrog->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&enMinifrog->actor, 0x3000, play) &&
               (Player_GetMask(play) == PLAYER_MASK_DON_GERO)) {
        Actor_OfferTalk(&enMinifrog->actor, play, 110.0f);
    }
}

void Rando::ActorBehavior::InitEnMinifrogBehavior() {
    COND_VB_SHOULD(VB_DESPAWN_FROG, SHUFFLED_FROGS, {
        EnMinifrog* enMinifrog = va_arg(args, EnMinifrog*);
        RandoCheckId frogCheck = GetFrogCheck(enMinifrog->frogIndex);
        if (frogCheck == RC_UNKNOWN) {
            return;
        }
        *should = RANDO_SAVE_CHECKS[frogCheck].cycleObtained;
    });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_MINIFROG, SHUFFLED_FROGS, [](Actor* actor) {
        EnMinifrog* enMinifrog = (EnMinifrog*)actor;
        // If this is not a Mountain Village frog, use the custom actionFunc
        if (enMinifrog->frogIndex != FROG_YELLOW && !EN_FROG_IS_RETURNED(actor)) {
            enMinifrog->actionFunc = MiniFrog_IdleWithoutCs;
        }
    });

    // "Ah, Don Gero"
    COND_ID_HOOK(OnOpenText, 0xD81, SHUFFLED_FROGS, [](u16* textId, bool* loadFromMessageTable) {
        Player* player = GET_PLAYER(gPlayState);
        EnMinifrog* enMinifrog =
            (EnMinifrog*)Actor_FindNearby(gPlayState, &player->actor, ACTOR_EN_MINIFROG, ACTORCAT_NPC, 100.0f);

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Why, Don Gero! I'm not joining that choir unless someone finds my other hiding spot. "
                    "Take this and don't follow me.";
        if (enMinifrog != NULL && CHECK_WEEKEVENTREG(sIsFrogReturnedFlags[enMinifrog->frogIndex])) {
            entry.msg = "Why, Don Gero! Since you found my other hiding spot, I'll join the choir. "
                        "Take this and meet me in the mountains.";
        }
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}
