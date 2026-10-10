#include "z64pause_menu.h"

#include "z64.h"
#include "z64shrink_window.h"

void (*sKaleidoScopeUpdateFunc)(PlayState* play);
void (*sKaleidoScopeDrawFunc)(PlayState* play);

extern void KaleidoScope_Update(PlayState* play);
extern void KaleidoScope_Draw(PlayState* play);

void KaleidoScopeCall_LoadPlayer(void) {
    KaleidoMgrOverlay* playerActorOvl = &gKaleidoMgrOverlayTable[KALEIDO_OVL_PLAYER_ACTOR];

    if (gKaleidoMgrCurOvl != playerActorOvl) {
        if (gKaleidoMgrCurOvl != NULL) {
            KaleidoManager_ClearOvl(gKaleidoMgrCurOvl);
        }

        KaleidoManager_LoadOvl(playerActorOvl);
    }
}

void KaleidoScopeCall_Init(PlayState* play) {
    sKaleidoScopeUpdateFunc = KaleidoManager_GetRamAddr(KaleidoScope_Update);
    sKaleidoScopeDrawFunc = KaleidoManager_GetRamAddr(KaleidoScope_Draw);
    KaleidoSetup_Init(play);
}

void KaleidoScopeCall_Destroy(PlayState* play) {
    KaleidoSetup_Destroy(play);
}

void KaleidoScopeCall_Update(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    KaleidoMgrOverlay* kaleidoScopeOvl = &gKaleidoMgrOverlayTable[KALEIDO_OVL_KALEIDO_SCOPE];

    if (!IS_PAUSED(&play->pauseCtx)) {
        return;
    }

    if ((pauseCtx->state == PAUSE_STATE_OPENING_0) || (pauseCtx->state == PAUSE_STATE_OWL_WARP_0)) {
        if (ShrinkWindow_Letterbox_GetSize() == 0) {
            R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_SETUP;
            pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
            pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_APPEARING;
            pauseCtx->state = (pauseCtx->state & 0xFFFF) + 1;
        }
    } else if (pauseCtx->state == PAUSE_STATE_GAMEOVER_0) {
        R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_SETUP;
        pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
        pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_APPEARING;
        pauseCtx->state = (pauseCtx->state & 0xFFFF) + 1;
    } else if ((pauseCtx->state == PAUSE_STATE_OPENING_1) || (pauseCtx->state == PAUSE_STATE_GAMEOVER_1) ||
               (pauseCtx->state == PAUSE_STATE_OWL_WARP_1)) {
        if (R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_READY) {
            pauseCtx->state++;
        }
    } else if (pauseCtx->state != PAUSE_STATE_OFF) {
        if (gKaleidoMgrCurOvl != kaleidoScopeOvl) {
            if (gKaleidoMgrCurOvl != NULL) {
                KaleidoManager_ClearOvl(gKaleidoMgrCurOvl);
            }

            KaleidoManager_LoadOvl(kaleidoScopeOvl);
        }

        if (gKaleidoMgrCurOvl == kaleidoScopeOvl) {
            sKaleidoScopeUpdateFunc(play);

            if (!IS_PAUSED(&play->pauseCtx)) {
                KaleidoManager_ClearOvl(kaleidoScopeOvl);
                KaleidoScopeCall_LoadPlayer();
            }
        }
    }
}

void KaleidoScopeCall_Draw(PlayState* play) {
    KaleidoMgrOverlay* kaleidoScopeOvl = &gKaleidoMgrOverlayTable[KALEIDO_OVL_KALEIDO_SCOPE];

    if (R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_READY) {
        if (((play->pauseCtx.state >= PAUSE_STATE_OPENING_3) && (play->pauseCtx.state <= PAUSE_STATE_SAVEPROMPT)) ||
            ((play->pauseCtx.state >= PAUSE_STATE_GAMEOVER_3) && (play->pauseCtx.state <= PAUSE_STATE_UNPAUSE_SETUP))) {
            if (gKaleidoMgrCurOvl == kaleidoScopeOvl) {
                sKaleidoScopeDrawFunc(play);
            }
        }
    }
}
