#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Rando/Logic/Logic.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Toto/z_en_toto.h"
extern void func_80BA36C0(EnToto* enToto, PlayState* play, s32 index);
}

void Rando::ActorBehavior::InitEnTotoBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* refActor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (refActor->id != ACTOR_EN_TOTO) {
            return;
        }

        RANDO_SAVE_CHECKS[RC_MILK_BAR_CIRCUS_LEADER_MASK].eligible = true;

        *should = false;
        refActor->parent = &player->actor;
        player->talkActor = refActor;
        player->talkActorDistance = refActor->xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;
        func_80BA36C0((EnToto*)refActor, gPlayState, 0); // Advance Toto to "Wanna play again?" state
    });

    /*
     * "Please take my mask"
     * Notebook events usually pop by immediately following a textbox. Since the Get Item animation is at the tail end
     * of a cutscene, and the player may have GI animations skipped, there is no subsequent textbox to trigger notebook
     * events. So, we queue the relevant notebook events manually with the final textbox of the cutscene.
     */
    COND_ID_HOOK(OnOpenText, 0x2B3B, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_RECEIVED_CIRCUS_LEADERS_MASK);
        Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_MET_TOTO);
        Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN);
    });

    COND_VB_SHOULD(VB_TOTO_START_SOUND_CHECK, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_OCARINA_BUTTONS], {
        EnToto* totoActor = va_arg(args, EnToto*);
        if (totoActor->text->textId == 0x2B24) {
            if (!(Rando::Logic::canPlaySong(OCARINA_SONG_WIND_FISH_HUMAN) &&
                  Rando::Logic::canPlaySong(OCARINA_SONG_WIND_FISH_DEKU) &&
                  Rando::Logic::canPlaySong(OCARINA_SONG_WIND_FISH_GORON) &&
                  Rando::Logic::canPlaySong(OCARINA_SONG_WIND_FISH_ZORA))) {
                Message_ContinueTextbox(gPlayState, 0x2B25);
                func_80BA36C0(totoActor, gPlayState, 0);
                Flags_UnsetSwitch(gPlayState, ENTOTO_GET_SWITCH_FLAG_1(&totoActor->actor));
                *should = false;
            }
        }
    });
}
