/*
 * File: z_demo_effect.c
 * Overlay: ovl_Demo_Effect
 * Description: Various cutscene effects (blue warp in, Great Fairy vanish, etc.)
 */

#include "z_demo_effect.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_efc_tw/object_efc_tw.h"

#include "2s2h/BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DemoEffect_Init(Actor* thisx, PlayState* play);
void DemoEffect_Destroy(Actor* thisx, PlayState* play);
void DemoEffect_Update(Actor* thisx, PlayState* play);

void DemoEffect_WaitForObject(DemoEffect* this, PlayState* play);
void DemoEffect_SetupTimewarp(DemoEffect* this, PlayState* play);
void DemoEffect_StartTimewarp(DemoEffect* this, PlayState* play);
void DemoEffect_ShrinkLight(DemoEffect* this, PlayState* play);
void DemoEffect_ExpandLight(DemoEffect* this, PlayState* play);
void DemoEffect_DrawTimewarp(Actor* thisx, PlayState* play);
void DemoEffect_DrawLight(Actor* thisx, PlayState* play2);

ActorProfile Demo_Effect_Profile = {
    /**/ ACTOR_DEMO_EFFECT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(DemoEffect),
    /**/ DemoEffect_Init,
    /**/ DemoEffect_Destroy,
    /**/ DemoEffect_Update,
    /**/ NULL,
};

void DemoEffect_Init(Actor* thisx, PlayState* play) {
    static s16 sEffectTypeObjectIds[] = {
        OBJECT_EFC_TW, OBJECT_EFC_TW, OBJECT_EFC_TW, OBJECT_EFC_TW, GAMEPLAY_KEEP,
        GAMEPLAY_KEEP, GAMEPLAY_KEEP, GAMEPLAY_KEEP, GAMEPLAY_KEEP,
    };
    s32 pad;
    DemoEffect* this = (DemoEffect*)thisx;
    s32 type = DEMO_EFFECT_GET_TYPE(&this->actor);
    s32 objectSlot;
    s32 pad2;
    Color_RGB8 lightColors[] = {
        { 200, 200, 0 },  // Yellow
        { 255, 40, 100 }, // Pink
        { 50, 255, 0 },   // Light green
        { 0, 0, 255 },    // Blue
        { 255, 255, 80 }, // Light Yellow
    };

    if (sEffectTypeObjectIds[type] == GAMEPLAY_KEEP) {
        objectSlot = 0;
    } else {
        objectSlot = Object_GetSlot(&play->objectCtx, sEffectTypeObjectIds[type]);
    }

    if (objectSlot <= OBJECT_SLOT_NONE) {
        // assert on debug
    } else {
        this->initObjectSlot = objectSlot;
    }

    Actor_SetScale(&this->actor, 0.2f);

    switch (type) {
        case DEMO_EFFECT_TIMEWARP_TIMEBLOCK_LARGE:
        case DEMO_EFFECT_TIMEWARP_TIMEBLOCK_SMALL:
            this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
        // FALLTHROUGH
        case DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_LARGE:
        case DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_VERY_LARGE:
            this->initDrawFunc = DemoEffect_DrawTimewarp;
            this->initActionFunc = DemoEffect_SetupTimewarp;
            this->envXluColor[0] = 0;
            this->envXluColor[1] = 100;
            this->envXluColor[2] = 255;
            SkelCurve_Clear(&this->skelCurve);
            this->timer = 0;
            break;

        case DEMO_EFFECT_TYPE_LIGHT_DARK_YELLOW:
        case DEMO_EFFECT_TYPE_LIGHT_PINK:
        case DEMO_EFFECT_TYPE_LIGHT_GREEN:
        case DEMO_EFFECT_TYPE_LIGHT_BLUE:
        case DEMO_EFFECT_TYPE_LIGHT_YELLOW:
            this->envXluColor[0] = lightColors[type - DEMO_EFFECT_TYPE_LIGHT_BASE].r;
            this->envXluColor[1] = lightColors[type - DEMO_EFFECT_TYPE_LIGHT_BASE].g;
            this->envXluColor[2] = lightColors[type - DEMO_EFFECT_TYPE_LIGHT_BASE].b;
            Actor_SetScale(&this->actor, 0.0f);
            this->initDrawFunc = DemoEffect_DrawLight;
            this->initActionFunc = DemoEffect_ExpandLight;
            this->timer = 0;
            break;

        default:
            break;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    this->actionFunc = DemoEffect_WaitForObject;
}

void DemoEffect_Destroy(Actor* thisx, PlayState* play) {
    DemoEffect* this = (DemoEffect*)thisx;

    switch (DEMO_EFFECT_GET_TYPE(&this->actor)) {
        case DEMO_EFFECT_TIMEWARP_TIMEBLOCK_LARGE:
        case DEMO_EFFECT_TIMEWARP_TIMEBLOCK_SMALL:
        case DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_LARGE:
        case DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_VERY_LARGE:
            SkelCurve_Destroy(play, &this->skelCurve);
            break;

        default:
            break;
    }
}

void DemoEffect_WaitForObject(DemoEffect* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->initObjectSlot)) {
        this->actor.objectSlot = this->initObjectSlot;
        this->actor.draw = this->initDrawFunc;
        this->actionFunc = this->initActionFunc;
    }
}

void DemoEffect_SetupTimewarp(DemoEffect* this, PlayState* play) {
    s32 type = DEMO_EFFECT_GET_TYPE(&this->actor);

    if (SkelCurve_Init(play, &this->skelCurve, &gTimewarpSkel, &gTimewarpAnim)) {}

    SkelCurve_SetAnim(&this->skelCurve, &gTimewarpAnim, 1.0f, 59.0f, 1.0f, 1.7f);
    SkelCurve_Update(play, &this->skelCurve);
    this->actionFunc = DemoEffect_StartTimewarp;

    switch (type) {
        case DEMO_EFFECT_TIMEWARP_TIMEBLOCK_LARGE:
            Actor_SetScale(&this->actor, 168.0f * 0.001f);
            break;

        case DEMO_EFFECT_TIMEWARP_TIMEBLOCK_SMALL:
            Actor_SetScale(&this->actor, 84.0f * 0.001f);
            break;

        case DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_LARGE:
            Actor_SetScale(&this->actor, 168.0f * 0.001f);
            break;

        case DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_VERY_LARGE:
            Actor_SetScale(&this->actor, 280.0f * 0.001f);
            break;

        default:
            Actor_SetScale(&this->actor, 14.0f * 0.001f);
            break;
    }
}

void DemoEffect_SetPerVertexAlpha(f32 alphaScale) {
    static u8 sAlphaTypes[] = { 1, 1, 2, 0, 1, 1, 2, 0, 1, 2, 0, 2, 1, 0, 1, 0, 2, 0, 2, 2, 0 };
    Vtx* vtx = ResourceMgr_LoadVtxByName(Lib_SegmentedToVirtual(gTimewarpVtx));
    s32 i;
    u8 alphas[3];

    alphas[0] = 0;
    alphas[1] = (s32)(202.0f * alphaScale);
    alphas[2] = (s32)(255.0f * alphaScale);

    for (i = 0; i < ARRAY_COUNT(sAlphaTypes); i++) {
        if (sAlphaTypes[i] != 0) {
            vtx[i].v.cn[3] = alphas[sAlphaTypes[i]];
        }
    }
}

/**
 * Shrink and fade linearly for 100 frames then remove.
 */
void DemoEffect_FinishTimewarp(DemoEffect* this, PlayState* play) {
    s32 type = DEMO_EFFECT_GET_TYPE(&this->actor);
    f32 scale;
    f32 alphaScale;

    this->timer++;
    if (this->timer <= 100) {
        alphaScale = (100 - this->timer) * 0.01f;
        scale = alphaScale * 0.14f;

        switch (type) {
            case DEMO_EFFECT_TIMEWARP_TIMEBLOCK_LARGE:
                scale *= 1.2f;
                break;

            case DEMO_EFFECT_TIMEWARP_TIMEBLOCK_SMALL:
                scale *= 0.6f;
                break;

            case DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_LARGE:
                scale *= 1.2f;
                break;

            case DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_VERY_LARGE:
                scale *= 2.0f;
                break;

            default:
                break;
        }

        this->actor.scale.x = scale;
        this->actor.scale.z = scale;
        DemoEffect_SetPerVertexAlpha(alphaScale);
        Actor_PlaySfx_FlaggedCentered2(&this->actor, NA_SE_EV_TIMETRIP_LIGHT - SFX_FLAG);
    } else {
        DemoEffect_SetPerVertexAlpha(1.0f);
        Actor_Kill(&this->actor);
    }
}

/**
 * Runs until animation plays to frame 59 and pauses it on frame 59.
 */
void DemoEffect_StartTimewarp(DemoEffect* this, PlayState* play) {
    Actor_PlaySfx_FlaggedCentered2(&this->actor, NA_SE_EV_TIMETRIP_LIGHT - SFX_FLAG);

    if (SkelCurve_Update(play, &this->skelCurve)) {
        SkelCurve_SetAnim(&this->skelCurve, &gTimewarpAnim, 1.0f, 60.0f, 59.0f, 0.0f);
        this->actionFunc = DemoEffect_FinishTimewarp;
        this->timer = 0;
    }
}

/**
 * Take scale to 0 linearly, when scale is small enough, remove.
 */
void DemoEffect_ShrinkLight(DemoEffect* this, PlayState* play) {
    Actor_SetScale(&this->actor, this->actor.scale.x - 0.02f);

    this->timer++;
    if (this->actor.scale.x < 0.02f) {
        Actor_Kill(&this->actor);
    }
}

/**
 * Changes scale for 3 frames, scale is successively 0.0f, 0.2f, 0.3f, 0.35f (would converge to 0.4 exponentially if run
 * for a long time).
 */
void DemoEffect_ExpandLight(DemoEffect* this, PlayState* play) {
    Actor_SetScale(&this->actor, (this->actor.scale.x * 0.5f) + 0.2f);

    this->timer++;
    if (this->timer >= 3) {
        this->actionFunc = DemoEffect_ShrinkLight;
    }
}

void DemoEffect_Update(Actor* thisx, PlayState* play) {
    DemoEffect* this = (DemoEffect*)thisx;

    this->actionFunc(this, play);
}

s32 DemoEffect_OverrideLimbDrawTimewarp(PlayState* play, SkelCurve* skelCurve, s32 limbIndex, Actor* thisx) {
    s32 pad;
    DemoEffect* this = (DemoEffect*)thisx;
    u32 frames = play->gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, this->envXluColor[0], this->envXluColor[1], this->envXluColor[2], 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, (frames * 6) % 1024, 255 - ((frames * 16) % 256), 0x100, 0x40,
                                  1, (frames * 4) % 512, 127 - ((frames * 12) % 128), 0x80, 0x20, 6, -16, 4, -12));

    CLOSE_DISPS(play->state.gfxCtx);

    if (limbIndex == TIMEWARP_LIMB_NONE) {
        s16* transform = skelCurve->jointTable[LIMB_ROOT_POS];

        transform[2] = transform[0] = 1024;
        transform[1] = 1024;
    }

    return true;
}

void DemoEffect_DrawTimewarp(Actor* thisx, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    DemoEffect* this = (DemoEffect*)thisx;

    OPEN_DISPS(gfxCtx);

    POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_25);

    Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);
    SkelCurve_Draw(&this->actor, play, &this->skelCurve, DemoEffect_OverrideLimbDrawTimewarp, NULL, 1, &this->actor);

    CLOSE_DISPS(gfxCtx);
}

void DemoEffect_DrawLight(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DemoEffect* this = (DemoEffect*)thisx;
    s16 zRot = (this->timer * 0x400) & 0xFFFF;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, this->envXluColor[0], this->envXluColor[1], this->envXluColor[2], 255);

    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_Push();
    Matrix_RotateZS(zRot, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gEffFlash2DL);

    Matrix_Pop();
    Matrix_RotateZS(-zRot, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gEffFlash2DL);

    CLOSE_DISPS(play->state.gfxCtx);

    gCustomLensFlare1On = true;
    gCustomLensFlare1Pos.x = thisx->world.pos.x;
    gCustomLensFlare1Pos.y = thisx->world.pos.y;
    gCustomLensFlare1Pos.z = thisx->world.pos.z;

    D_801F4E44 = thisx->scale.x * 60.0f;
    D_801F4E48 = thisx->scale.x * 50.0f;
    D_801F4E4C = thisx->scale.x * 250.0f;
}
