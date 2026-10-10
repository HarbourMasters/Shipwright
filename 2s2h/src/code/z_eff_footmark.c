#include "z64eff_footmark.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#include "assets/code/eff_footmark/eff_footmark.h"

void EffFootmark_Init(PlayState* play) {
    EffFootmark* footmark;
    s32 i;

    for (footmark = play->footprintInfo, i = 0; i < ARRAY_COUNT(play->footprintInfo); i++, footmark++) {
        footmark->actor = NULL;
        footmark->pos.x = 0;
        footmark->pos.y = 0;
        footmark->pos.z = 0;
        footmark->flags = 0;
        footmark->id = 0;
        footmark->alpha = 0;
        footmark->alphaChange = 0;
    }
}

void EffFootmark_Add(PlayState* play, MtxF* mf, Actor* actor, u8 id, Vec3f* pos, u16 size, u8 red, u8 green, u8 blue,
                     u16 alpha, u16 alphaChange, u16 fadeOutDelay) {
    s32 i;
    EffFootmark* footmark;
    EffFootmark* destination = NULL;
    EffFootmark* oldest = NULL;
    s32 isNew = true;

    for (footmark = play->footprintInfo, i = 0; i < ARRAY_COUNT(play->footprintInfo); i++, footmark++) {
        if (((actor == footmark->actor) && (footmark->id == id)) && !(footmark->flags & FOOTMARK_FLAG_1)) {
            if (fabsf(footmark->pos.x - pos->x) <= 1) {
                if (fabsf(footmark->pos.z - pos->z) <= 1) {
                    isNew = false;
                    break;
                }
            }

            // This footmark is being re-added at a new pos. Let's mark this one to start fading out.
            footmark->flags = FOOTMARK_FLAG_1;
        }

        if (footmark->actor == NULL) {
            destination = footmark;
        } else if (destination == NULL) {
            if (((oldest != NULL) && (footmark->age > oldest->age)) || (oldest == NULL)) {
                oldest = footmark;
            }
        }
    }

    if (isNew && ((destination != NULL) || (oldest != NULL))) {
        if (destination == NULL) {
            destination = oldest;
        }
        Matrix_MtxFCopy(&destination->mf, mf);
        destination->actor = actor;
        destination->pos.x = pos->x;
        destination->pos.y = pos->y;
        destination->pos.z = pos->z;
        destination->flags = 0;
        destination->id = id;
        destination->red = red;
        destination->green = green;
        destination->blue = blue;
        destination->alpha = alpha;
        destination->alphaChange = alphaChange;
        destination->size = size;
        destination->fadeOutDelay = fadeOutDelay;
        destination->age = 0;
    }
}

void EffFootmark_Update(PlayState* play) {
    EffFootmark* footmark;
    s32 i;

    for (footmark = play->footprintInfo, i = 0; i < ARRAY_COUNT(play->footprintInfo); i++, footmark++) {
        if (footmark->actor == NULL) {
            continue;
        }

        if (CHECK_FLAG_ALL(footmark->flags, FOOTMARK_FLAG_1)) {
            if ((u32)footmark->age < UINT16_MAX) {
                footmark->age++;
            }

            if (footmark->fadeOutDelay == 0) {
                if (footmark->alpha >= footmark->alphaChange + 0x1000) {
                    footmark->alpha -= footmark->alphaChange;
                } else {
                    footmark->actor = NULL;
                }
            } else if (footmark->fadeOutDelay > 0) {
                footmark->fadeOutDelay--;
            }
        }
    }
}

void EffFootmark_Draw(PlayState* play) {
    EffFootmark* footmark;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL44_Xlu(play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gEffFootprintMaterialDL);

    for (footmark = play->footprintInfo, i = 0; i < ARRAY_COUNT(play->footprintInfo); i++, footmark++) {
        if (footmark->actor != NULL) {
            FrameInterpolation_RecordOpenChild(footmark, i);

            Matrix_Put(&footmark->mf);
            Matrix_Scale(footmark->size * (1.0f / 0x100) * 0.7f, 1, footmark->size * (1.0f / 0x100), MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, footmark->red, footmark->green, footmark->blue,
                            footmark->alpha >> 8);

            gSPDisplayList(POLY_XLU_DISP++, gEffFootprintModelDL);

            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
