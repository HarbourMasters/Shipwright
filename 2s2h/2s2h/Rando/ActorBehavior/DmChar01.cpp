#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "overlays/actors/ovl_Dm_Char01/z_dm_char01.h"
}

void Rando::ActorBehavior::InitDmChar01Behavior() {

    bool shouldRegisterOnSceneInit = IS_RANDO && RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS] == RO_ACCESS_DUNGEONS_OPEN;

    bool shouldRegisterVB = IS_RANDO && RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS] >= RO_ACCESS_DUNGEONS_FORM_OR_SONG &&
                            RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS] <= RO_ACCESS_DUNGEONS_SONG_ONLY;

    COND_VB_SHOULD(VB_OPEN_WOODFALL_FROM_SONG, shouldRegisterVB, {
        DmChar01* dmChar01 = va_arg(args, DmChar01*);
        Player* player = GET_PLAYER(gPlayState);

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_20_01)) {
            return;
        }

        bool hasSongAccess = (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_EVENT &&
                              gPlayState->msgCtx.lastPlayedSong == OCARINA_SONG_SONATA);

        bool hasFormAccess =
            (player->transformation == PLAYER_FORM_DEKU && (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_EVENT ||
                                                            gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE));
        switch (RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS]) {
            case RO_ACCESS_DUNGEONS_FORM_OR_SONG:
                *should = hasSongAccess || hasFormAccess;
                break;
            case RO_ACCESS_DUNGEONS_FORM_ONLY:
                *should = hasFormAccess;
                break;
            case RO_ACCESS_DUNGEONS_SONG_ONLY:
                *should = hasSongAccess;
                break;
            default:
                break;
        }
    });

    COND_HOOK(OnFlagSet, shouldRegisterVB, [](FlagType flagType, u32 flag) {
        if (flagType == FLAG_WEEK_EVENT_REG && flag == WEEKEVENTREG_20_01) {
            AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
            gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_END;
            Message_CloseTextbox(gPlayState);
        }
    });

    COND_ID_HOOK(OnSceneInit, SCENE_21MITURINMAE, shouldRegisterOnSceneInit,
                 [](s16 sceneId, s8 spawnNum) { SET_WEEKEVENTREG(WEEKEVENTREG_20_01); });
}
