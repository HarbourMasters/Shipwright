/*
 * File: z_kaleido_prompt.c
 * Overlay: ovl_kaleido_scope
 * Description: Pause Menu - Save and Continue Prompt
 */

#include "z_kaleido_scope.h"

s16 sPromptAlphaTargets[] = { 100, 255 };

// Unused remnant of OoT
void KaleidoScope_UpdatePrompt(PlayState* play) {
    static s16 sPromptAlphaTargetIndex = 0;
    static s16 sPromptAlphaTimer = 10;
    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = CONTROLLER1(&play->state);
    s8 stickAdjX = input->rel.stick_x;
    s16 alphaStep;

    // #region 2S2H [Port] - Fix stick inputs
    stickAdjX = pauseCtx->stickAdjX;

    if (((pauseCtx->state == PAUSE_STATE_SAVEPROMPT) && (pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_1)) ||
        (pauseCtx->state == PAUSE_STATE_GAMEOVER_SAVE_PROMPT) ||
        (pauseCtx->state == PAUSE_STATE_GAMEOVER_CONTINUE_PROMPT)) {

        // Move the prompt
        if ((pauseCtx->promptChoice == PAUSE_PROMPT_YES) && (stickAdjX >= 30)) {
            // Move right to the no prompt
            Audio_PlaySfx(NA_SE_SY_CURSOR);
            pauseCtx->promptChoice = PAUSE_PROMPT_NO;
        } else if ((pauseCtx->promptChoice != PAUSE_PROMPT_YES) && (stickAdjX <= -30)) {
            // Move left to the yes prompt
            Audio_PlaySfx(NA_SE_SY_CURSOR);
            pauseCtx->promptChoice = PAUSE_PROMPT_YES;
        }

        // Update the alpha for the green glowing orb above the prompt
        alphaStep = ABS_ALT(pauseCtx->promptAlpha - sPromptAlphaTargets[sPromptAlphaTargetIndex]) / sPromptAlphaTimer;

        if (pauseCtx->promptAlpha >= sPromptAlphaTargets[sPromptAlphaTargetIndex]) {
            pauseCtx->promptAlpha -= alphaStep;
        } else {
            pauseCtx->promptAlpha += alphaStep;
        }

        sPromptAlphaTimer--;
        if (sPromptAlphaTimer == 0) {
            pauseCtx->promptAlpha = sPromptAlphaTargets[sPromptAlphaTargetIndex];
            sPromptAlphaTimer = sPromptAlphaTargetIndex + 20;
            sPromptAlphaTargetIndex ^= 1;
        }
    }
}
