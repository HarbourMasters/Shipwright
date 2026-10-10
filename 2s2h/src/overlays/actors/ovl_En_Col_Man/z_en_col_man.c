/*
 * File: z_en_col_man.c
 * Overlay: ovl_En_Col_Man
 * Description: Lab HP, Garo Master falling rocks, Garo Master bomb
 */

#include "z_en_col_man.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_FREEZE_EXCEPTION)

void EnColMan_Init(Actor* thisx, PlayState* play);
void EnColMan_Destroy(Actor* thisx, PlayState* play);
void EnColMan_Update(Actor* thisx, PlayState* play);
void func_80AFDD60(EnColMan* this);
void func_80AFDE00(EnColMan* this, PlayState* play);
void EnColMan_SetHeartPieceCollectedAndKill(EnColMan* this, PlayState* play);
void func_80AFDF60(EnColMan* this);
void func_80AFDFB4(EnColMan* this, PlayState* play);
void func_80AFE234(EnColMan* this);
void func_80AFE25C(EnColMan* this, PlayState* play);
void func_80AFE414(Actor* thisx, PlayState* play);
void func_80AFE4AC(Actor* thisx, PlayState* play);
void func_80AFE584(Actor* thisx, PlayState* play);
void func_80AFE650(Actor* thisx, PlayState* play);

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 10, 11, 1, { 0, 0, 0 } },
};

ActorProfile En_Col_Man_Profile = {
    /**/ ACTOR_EN_COL_MAN,
    /**/ ACTORCAT_MISC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnColMan),
    /**/ EnColMan_Init,
    /**/ EnColMan_Destroy,
    /**/ EnColMan_Update,
    /**/ NULL,
};

void EnColMan_Init(Actor* thisx, PlayState* play) {
    EnColMan* this = (EnColMan*)thisx;

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    this->scale = (BREG(55) / 1000.0f) + 0.01f;

    switch (this->actor.params) {
        case EN_COL_MAN_HEART_PIECE:
        case EN_COL_MAN_RECOVERY_HEART:
        default:
            ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 10.0f);
            func_80AFDD60(this);
            break;

        case EN_COL_MAN_FALLING_ROCK:
            ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 10.0f);
            func_80AFDF60(this);
            break;

        case EN_COL_MAN_CUTSCENE_BOMB:
        case EN_COL_MAN_GAMEPLAY_BOMB:
            func_80AFE234(this);
            break;
    }
}

void EnColMan_Destroy(Actor* thisx, PlayState* play) {
    EnColMan* this = (EnColMan*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80AFDD60(EnColMan* this) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MARINE_RESEARCH_LAB_FISH_HEART_PIECE)) {
        this->actor.draw = func_80AFE414;
        this->actor.shape.yOffset = 700.0f;
        if (this->actor.params == EN_COL_MAN_HEART_PIECE) {
            this->actor.gravity = -2.0f;
        }
        this->type = EN_COL_MAN_HEART_PIECE;
    } else {
        this->actor.draw = func_80AFE4AC;
        this->actor.shape.yOffset = 300.0f;
        this->actor.shape.shadowScale = 5.0f;
        if (this->actor.params == EN_COL_MAN_HEART_PIECE) {
            this->actor.gravity = -2.0f;
        }
        this->type = EN_COL_MAN_RECOVERY_HEART;
    }
    this->actionFunc = func_80AFDE00;
}

void func_80AFDE00(EnColMan* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.params == EN_COL_MAN_HEART_PIECE) {
            this->actor.params = EN_COL_MAN_RECOVERY_HEART;
            this->actor.speed = 2.0f;
            this->actor.velocity.y = 8.0f;
        } else {
            this->actor.speed = 0.0f;
        }
    }
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MARINE_RESEARCH_LAB_FISH_HEART_PIECE)) {
        this->actor.shape.rot.y += 0x3E8;
    }
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actor.draw = NULL;
        this->actionFunc = EnColMan_SetHeartPieceCollectedAndKill;
    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MARINE_RESEARCH_LAB_FISH_HEART_PIECE)) {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 40.0f, 40.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_RECOVERY_HEART, 40.0f, 40.0f);
    }
}

void EnColMan_SetHeartPieceCollectedAndKill(EnColMan* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MARINE_RESEARCH_LAB_FISH_HEART_PIECE);
        Actor_Kill(&this->actor);
    }
}

void func_80AFDF60(EnColMan* this) {
    this->actor.draw = func_80AFE584;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->type = EN_COL_MAN_FALLING_ROCK;
    this->actionFunc = func_80AFDFB4;
    this->actor.shape.shadowScale = 5.0f;
    this->actor.gravity = -3.0f;
}

void func_80AFDFB4(EnColMan* this, PlayState* play) {
    static Color_RGBA8 sPrimColor = { 60, 50, 20, 255 };
    static Color_RGBA8 sEnvColor = { 40, 30, 30, 255 };
    s32 i;
    Vec3f velocity;
    Vec3f accel;

    this->scale = (BREG(55) / 10000.0f) + 0.0015f;

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.velocity.y < 0.0f)) {
        if (!this->hasSetRandomValues) {
            this->actor.world.rot.y = Rand_CenteredFloat(30000.0f);
            this->actor.speed = 2.0f + BREG(56) + Rand_ZeroFloat(2.0f);
            this->actor.velocity.y = 12.0f + BREG(57) + Rand_ZeroFloat(5.0f);
            this->hasSetRandomValues = true;
            Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_ROCK);
            return;
        }

        for (i = 0; i < 2; i++) {
            velocity.x = Rand_CenteredFloat(2.0f);
            velocity.y = Rand_ZeroFloat(2.0f) + 1.0f;
            velocity.z = Rand_CenteredFloat(2.0f);
            accel.y = -0.1f;
            accel.z = 0.0f;
            accel.x = 0.0f;

            func_800B0EB0(play, &this->actor.world.pos, &velocity, &accel, &sPrimColor, &sEnvColor,
                          Rand_ZeroFloat(50.0f) + 60.0f, 30, Rand_ZeroFloat(5.0f) + 20.0f);
        }

        Actor_Kill(&this->actor);
    }
}

void func_80AFE234(EnColMan* this) {
    this->actor.draw = func_80AFE650;
    this->type = EN_COL_MAN_CUTSCENE_BOMB;
    this->actionFunc = func_80AFE25C;
}

void func_80AFE25C(EnColMan* this, PlayState* play) {
    this->scale = BREG(55) * 0.01f + 0.05f;

    if (BREG(60) || (this->actor.world.rot.z != 0)) {
        if (this->actor.params == EN_COL_MAN_CUTSCENE_BOMB) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.parent->world.pos.x,
                        this->actor.parent->world.pos.y, this->actor.parent->world.pos.z, 0, 0, 0,
                        CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_EXPLOSION));
        } else {
            EnBom* bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                              this->actor.world.pos.y, this->actor.world.pos.z,
                                              BOMB_EXPLOSIVE_TYPE_BOMB, 0, 0, BOMB_TYPE_BODY);

            if (bomb != NULL) {
                bomb->timer = 0;
            }
        }

        Actor_PlaySfx(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
        Actor_Kill(&this->actor);
    }
}

void EnColMan_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnColMan* this = (EnColMan*)thisx;

    Actor_SetScale(&this->actor, this->scale);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 30.0f, 30.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_80AFE414(Actor* thisx, PlayState* play) {
    EnColMan* this = (EnColMan*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    func_800B8118(&this->actor, play, 0);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gHeartPieceInteriorDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80AFE4AC(Actor* thisx, PlayState* play) {
    EnColMan* this = (EnColMan*)thisx;

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gDropRecoveryHeartTex));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gItemDropDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80AFE584(Actor* thisx, PlayState* play) {
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
    gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_06AB30);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80AFE650(Actor* thisx, PlayState* play) {
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gDropBombTex));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gItemDropDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
