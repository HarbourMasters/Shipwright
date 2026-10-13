#include "MiscBehavior.h"

extern "C" {
#include "variables.h"

void Player_StartTalking(PlayState* play, Actor* actor);
void Player_SetupTalk(PlayState* play, Player* player);
s32 Player_SetupWaitForPutAway(PlayState* play, Player* player, AfterPutAwayFunc afterPutAwayFunc);
}

// This prevents actors from giving items with Actor_OfferGetItem, along with preventing them from waiting on the
// GetItem textbox to close
void Rando::MiscBehavior::InitOfferGetItemBehavior() {
    // Scripted Actors
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);
        static std::vector<u8> skipCmds = {};

        // SPDLOG_INFO("VB_EXEC_MSG_EVENT {}", cmdId);

        if (cmdId == MSCRIPT_CMD_ID_OFFER_ITEM) {
            switch (actor->id) {
                case ACTOR_EN_PST:
                    actor->flags |= ACTOR_FLAG_TALK; // Prevent softlock
                    Player_SetupWaitForPutAway(gPlayState, player, Player_SetupTalk);
                    *should = false;
                    return;
            }
        }

        if (skipCmds.empty()) {
            return;
        }

        if (cmdId == skipCmds[0]) {
            skipCmds.erase(skipCmds.begin());
            *should = false;
        }
    });

    // Non-scripted actors
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        switch (actor->id) {
            // Otherwise, Dotour will repeat the current textbox and not display the next one
            case ACTOR_EN_DT:
                actor->textId = 0x2AD1;
                [[fallthrough]];
            case ACTOR_EN_DNO:
            case ACTOR_EN_JS:
            case ACTOR_EN_KENDO_JS:
            case ACTOR_EN_GURUGURU:
            case ACTOR_EN_HS:
            case ACTOR_EN_MINIFROG:
            case ACTOR_EN_MM3:
            case ACTOR_EN_STH:
            case ACTOR_EN_ZOS:
                *should = false;
                actor->parent = &player->actor;
                player->talkActor = actor;
                player->talkActorDistance = actor->xzDistToPlayer;
                player->exchangeItemAction = PLAYER_IA_MINUS1;
                Player_StartTalking(gPlayState, actor);
                break;
        }
    });
}
