/*
 * File: z_en_scopecoin.c
 * Overlay: ovl_En_Scopecoin
 * Description: Termina Field rupees visible from the telescope
 */

#include "z_en_scopecoin.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnScopecoin_Init(Actor* thisx, PlayState* play);
void EnScopecoin_Destroy(Actor* thisx, PlayState* play);
void EnScopecoin_Update(Actor* thisx, PlayState* play);
void EnScopecoin_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Scopecoin_Profile = {
    /**/ ACTOR_EN_SCOPECOIN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnScopecoin),
    /**/ EnScopecoin_Init,
    /**/ EnScopecoin_Destroy,
    /**/ EnScopecoin_Update,
    /**/ EnScopecoin_Draw,
};

void EnScopecoin_Spin(EnScopecoin* this, PlayState* play) {
    this->actor.shape.rot.y += 0x1F4;
}

void EnScopecoin_CheckCollectible(EnScopecoin* this, PlayState* play) {
    if (Flags_GetCollectible(play, OBJMUPICT_GET_RUPEE_FLAG(&this->actor))) {
        Item_DropCollectible(play, &this->actor.world.pos, ITEM00_RUPEE_RED);
        Actor_Kill(&this->actor);
    }
}

void EnScopecoin_Init(Actor* thisx, PlayState* play) {
    EnScopecoin* this = (EnScopecoin*)thisx;

    Actor_SetScale(&this->actor, 0.01f);
    ActorShape_Init(&this->actor.shape, 0, ActorShadow_DrawCircle, 10.0f);

    this->rupeeIndex = OBJMUPICT_GET_RUPEE_INDEX(&this->actor);
    if ((this->rupeeIndex < 0) || (this->rupeeIndex > 7)) {
        this->rupeeIndex = 0;
    }

    if (play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) {
        if ((this->rupeeIndex == 2) || (this->rupeeIndex == 6)) {
            if (Flags_GetCollectible(play, OBJMUPICT_GET_RUPEE_FLAG(&this->actor))) {
                Actor_Kill(&this->actor);
                return;
            }
        }
        this->actor.shape.yOffset = 700.0f;
        this->actionFunc = EnScopecoin_Spin;
    } else {
        if ((this->rupeeIndex == 2) || (this->rupeeIndex == 6)) {
            if (Flags_GetCollectible(play, OBJMUPICT_GET_RUPEE_FLAG(&this->actor))) {
                Actor_Kill(&this->actor);
                return;
            }
            this->actor.draw = NULL;
            this->actionFunc = EnScopecoin_CheckCollectible;
        } else {
            Actor_Kill(&this->actor);
        }
    }
}

void EnScopecoin_Destroy(Actor* thisx, PlayState* play) {
}

void EnScopecoin_Update(Actor* thisx, PlayState* play) {
    EnScopecoin* this = (EnScopecoin*)thisx;

    this->actionFunc(this, play);
}

static TexturePtr sRupeeTextures[] = {
    gRupeeGreenTex, gRupeeBlueTex, gRupeeRedTex, gRupeeOrangeTex, gRupeePurpleTex, gRupeeSilverTex, gRupeeRedTex,
};

void EnScopecoin_Draw(Actor* thisx, PlayState* play) {
    EnScopecoin* this = (EnScopecoin*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    func_800B8050(&this->actor, play, 0);

    OPEN_DISPS(gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sRupeeTextures[this->rupeeIndex]));
    gSPDisplayList(POLY_OPA_DISP++, gRupeeDL);

    CLOSE_DISPS(gfxCtx);
}
