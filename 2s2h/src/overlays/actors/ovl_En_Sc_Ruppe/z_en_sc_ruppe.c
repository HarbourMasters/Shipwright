/*
 * File: z_en_sc_ruppe.c
 * Overlay: ovl_En_Sc_Ruppe
 * Description: Giant Rupee
 */

#include "z_en_sc_ruppe.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnScRuppe_Init(Actor* thisx, PlayState* play);
void EnScRuppe_Destroy(Actor* thisx, PlayState* play);
void EnScRuppe_Update(Actor* thisx, PlayState* play);
void EnScRuppe_Draw(Actor* thisx, PlayState* play);

void func_80BD6B18(EnScRuppe* this, PlayState* play);

typedef struct {
    /* 0x0 */ TexturePtr tex;
    /* 0x4 */ s16 amount;
} RuppeInfo; // size = 0x8

ActorProfile En_Sc_Ruppe_Profile = {
    /**/ ACTOR_EN_SC_RUPPE,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnScRuppe),
    /**/ EnScRuppe_Init,
    /**/ EnScRuppe_Destroy,
    /**/ EnScRuppe_Update,
    /**/ EnScRuppe_Draw,
};

RuppeInfo sRupeeInfo[] = {
    { gRupeeGreenTex, 1 },    // Green rupee
    { gRupeeBlueTex, 5 },     // Blue rupee
    { gRupeeRedTex, 20 },     // Red rupee
    { gRupeeOrangeTex, 200 }, // Orange rupee
    { gRupeePurpleTex, 50 },  // Purple rupee
    { gRupeeSilverTex, 10 },  // (unused)
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK4,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

void EnScRuppe_UpdateCollision(EnScRuppe* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 32.0f, 30.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
}

s32 func_80BD697C(s16 ruppeIndex) {
    switch (ruppeIndex) {
        case RUPEE_GREEN:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_53_04)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_53_04);
                return true;
            }
            break;
        case RUPEE_BLUE:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_53_80)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_53_80);
                return true;
            }
            break;
        case RUPEE_RED:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_54_01)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_54_01);
                return true;
            }
            break;
        case RUPEE_ORANGE:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_54_02)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_54_02);
                return true;
            }
            break;
        case RUPEE_PURPLE:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_54_04)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_54_04);
                return true;
            }
            break;
        case RUPEE_UNUSED:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_54_08)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_54_08);
                return true;
            }
            break;
    }
    return false;
}

void func_80BD6A8C(EnScRuppe* this, PlayState* play) {
    if (this->collider.base.ocFlags1 & OC1_HIT) {
        this->ruppeDisplayTime = 0;
        this->actor.gravity = 0.0f;
        Actor_PlaySfx(&this->actor, NA_SE_SY_GET_RUPY);
        Rupees_ChangeBy(sRupeeInfo[this->ruppeIndex].amount);
        this->actionFunc = func_80BD6B18;
    }
    this->actor.shape.rot.y += 0x1F4;
    Actor_MoveWithGravity(&this->actor);
}

void func_80BD6B18(EnScRuppe* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->ruppeDisplayTime > 30) {
        if (func_80BD697C(this->ruppeIndex)) {
            Actor_Kill(&this->actor);
        }
    } else {
        f32 scale;

        this->ruppeDisplayTime++;
        this->actor.world.pos = player->actor.world.pos;
        this->actor.world.pos.y += 40.0f;
        scale = (30.0f - this->ruppeDisplayTime) * 0.001f;
        Actor_SetScale(&this->actor, scale);
    }
    this->actor.shape.rot.y += 0x3E8;
    Actor_MoveWithGravity(&this->actor);
}

void EnScRuppe_Init(Actor* thisx, PlayState* play) {
    EnScRuppe* this = (EnScRuppe*)thisx;
    ColliderCylinder* collider = &this->collider;

    Collider_InitCylinder(play, collider);
    Collider_InitAndSetCylinder(play, collider, &this->actor, &sCylinderInit);
    Actor_SetScale(&this->actor, 0.03f);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 10.0f);
    this->ruppeIndex = SCRUPPE_GET_TYPE(thisx);
    if ((this->ruppeIndex < RUPEE_GREEN) || (this->ruppeIndex >= RUPEE_UNUSED)) {
        this->ruppeIndex = RUPEE_GREEN;
    }
    this->actor.speed = 0.0f;
    this->actionFunc = func_80BD6A8C;
    this->actor.gravity = -0.5f;
    this->actor.shape.yOffset = 700.0f;
}

void EnScRuppe_Destroy(Actor* thisx, PlayState* play) {
    EnScRuppe* this = (EnScRuppe*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnScRuppe_Update(Actor* thisx, PlayState* play) {
    EnScRuppe* this = (EnScRuppe*)thisx;

    this->actionFunc(this, play);
    EnScRuppe_UpdateCollision(this, play);
}

void EnScRuppe_Draw(Actor* thisx, PlayState* play) {
    s32* pad;
    EnScRuppe* this = (EnScRuppe*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    func_800B8050(&this->actor, play, 0);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sRupeeInfo[this->ruppeIndex].tex));
    gSPDisplayList(POLY_OPA_DISP++, gRupeeDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
