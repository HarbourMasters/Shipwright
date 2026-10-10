#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "overlays/actors/ovl_En_Dai/z_en_dai.h"
}

void Rando::ActorBehavior::InitEnDaiBehavior() {

    bool shouldRegisterOnSceneInit = IS_RANDO && RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS] == RO_ACCESS_DUNGEONS_OPEN;

    bool shouldRegisterVB = IS_RANDO && RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS] >= RO_ACCESS_DUNGEONS_FORM_OR_SONG &&
                            RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS] <= RO_ACCESS_DUNGEONS_SONG_ONLY;

    COND_VB_SHOULD(VB_OPEN_SNOWHEAD_FROM_SONG, shouldRegisterVB, {
        EnDai* enDai = va_arg(args, EnDai*);
        Player* player = GET_PLAYER(gPlayState);

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_30_01)) {
            return;
        }

        bool hasSongAccess = (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_EVENT &&
                              gPlayState->msgCtx.lastPlayedSong == OCARINA_SONG_GORON_LULLABY);

        bool hasFormAccess =
            (player->transformation == PLAYER_FORM_GORON && (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_EVENT ||
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

        if (*should) {
            AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
            gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_END;
            Message_CloseTextbox(gPlayState);
        }
    });

    COND_ID_HOOK(OnSceneInit, SCENE_12HAKUGINMAE, shouldRegisterOnSceneInit,
                 [](s16 sceneId, s8 spawnNum) { SET_WEEKEVENTREG(WEEKEVENTREG_30_01); });
}
