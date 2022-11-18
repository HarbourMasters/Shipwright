/*
 * File: z_en_bx.c
 * Overlay: ovl_En_Bx
 * Description: Jabu-Jabu Electrified Tentacle
 */

#include "z_en_bx.h"
#include "objects/object_bxa/object_bxa.h"
#include "soh/frame_interpolation.h"

#define FLAGS ACTOR_FLAG_4

void EnBx_Init(Actor* thisx, PlayState* play);
void EnBx_Destroy(Actor* thisx, PlayState* play);
void EnBx_Update(Actor* thisx, PlayState* play);
void EnBx_Draw(Actor* thisx, PlayState* play);

const ActorInit En_Bx_InitVars = {
    ACTOR_EN_BX,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BXA,
    sizeof(EnBx),
    (ActorFunc)EnBx_Init,
    (ActorFunc)EnBx_Destroy,
    (ActorFunc)EnBx_Update,
    (ActorFunc)EnBx_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0xFFCFFFFF, 0x03, 0x04 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 60, 100, 100, { 0, 0, 0 } },
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x03, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

void EnBx_Init(Actor* thisx, PlayState* play) {
    EnBx* this = (EnBx*)thisx;
    Vec3f sp48 = { 0.015f, 0.015f, 0.015f };
    Vec3f sp3C = { 0.0f, 0.0f, 0.0f };
    static InitChainEntry sInitChain[] = {
        ICHAIN_F32(targetArrowOffset, 5300, ICHAIN_STOP),
    };
    s32 i;
    s32 pad;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    thisx->scale.x = thisx->scale.z = 0.01f;
    thisx->scale.y = 0.03f;

    thisx->world.pos.y = thisx->world.pos.y - 100.0f;
    for (i = 0; i < 4; i++) {
        this->unk_184[i] = sp48;
        if (i == 0) {
            this->unk_1B4[i].x = thisx->shape.rot.x - 0x4000;
        }
        this->unk_154[i] = thisx->world.pos;
        this->unk_154[i].y = thisx->world.pos.y + ((i + 1) * 140.0f);
    }

    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 48.0f);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_InitQuad(play, &this->colliderQuad);
    Collider_SetQuad(play, &this->colliderQuad, &this->actor, &sQuadInit);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_14C = 0;
    thisx->uncullZoneDownward = 2000.0f;
    if (Flags_GetSwitch(play, (thisx->params >> 8) & 0xFF)) {
        Actor_Kill(&this->actor);
    }
    thisx->params &= 0xFF;
    this->epoch++;
}

void EnBx_Destroy(Actor* thisx, PlayState* play) {
    EnBx* this = (EnBx*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_809D1D0C(Actor* thisx, PlayState* play) {
    Vec3f sp5C = { 8000.0f, 15000.0f, 2500.0f };
    Vec3f sp50 = { 8000.0f, 10000.0f, 2500.0f };
    static Vec3f D_809D2540 = { -8000.0f, 15000.0f, 2500.0f };
    static Vec3f D_809D254C = { -8000.0f, 10000.0f, 2500.0f };
    Vec3f sp44;
    Vec3f sp38;
    EnBx* this = (EnBx*)thisx;

    Matrix_MultVec3f(&D_809D2540, &sp44);
    Matrix_MultVec3f(&D_809D254C, &sp38);
    Matrix_MultVec3f(&sp5C, &this->colliderQuad.dim.quad[1]);
    Matrix_MultVec3f(&sp50, &this->colliderQuad.dim.quad[0]);
    Collider_SetQuadVertices(&this->colliderQuad, &sp38, &sp44, &this->colliderQuad.dim.quad[0],
                             &this->colliderQuad.dim.quad[1]);
}

void EnBx_Update(Actor* thisx, PlayState* play) {
    EnBx* this = (EnBx*)thisx;
    Player* player = GET_PLAYER(play);
    s32 i;
    s16 tmp32;
    s32 tmp33;

    if ((thisx->xzDistToPlayer <= 70.0f) || (this->collider.base.atFlags & AT_HIT) ||
        (this->collider.base.acFlags & AC_HIT) || (this->colliderQuad.base.atFlags & AT_HIT)) {
        if ((thisx->xzDistToPlayer <= 70.0f) || (&player->actor == this->collider.base.at) ||
            (&player->actor == this->collider.base.ac) || (&player->actor == this->colliderQuad.base.at)) {
            tmp33 = player->invincibilityTimer & 0xFF;
            tmp32 = thisx->world.rot.y;
            if (!(thisx->params & 0x80)) {
                tmp32 = thisx->yawTowardsPlayer;
            }
            if ((&player->actor != this->collider.base.at) && (&player->actor != this->collider.base.ac) &&
                (&player->actor != this->colliderQuad.base.at) && (player->invincibilityTimer <= 0)) {
                if (player->invincibilityTimer < -39) {
                    player->invincibilityTimer = 0;
                } else {
                    player->invincibilityTimer = 0;
                    play->damagePlayer(play, -4);
                }
            }
            func_8002F71C(play, &this->actor, 6.0f, tmp32, 6.0f);
            player->invincibilityTimer = tmp33;
        }

        this->collider.base.atFlags &= ~AT_HIT;
        this->collider.base.acFlags &= ~AC_HIT;
        this->colliderQuad.base.atFlags &= ~AT_HIT;
        this->colliderQuad.base.at = NULL;
        this->collider.base.ac = NULL;
        this->collider.base.at = NULL;
        this->unk_14C = 0x14;
    }

    if (this->unk_14C != 0) {
        this->unk_14C--;
        for (i = 0; i < 4; i++) {
            if (!((this->unk_14C + (i << 1)) % 4)) {
                static Color_RGBA8 primColor = { 255, 255, 255, 255 };
                static Color_RGBA8 envColor = { 200, 255, 255, 255 };
                Vec3f pos;
                s16 yaw;

                yaw = (s32)Rand_CenteredFloat(12288.0f);
                yaw = (yaw + (i * 0x4000)) + 0x2000;
                pos.x = Rand_CenteredFloat(5.0f) + thisx->world.pos.x;
                pos.y = Rand_CenteredFloat(30.0f) + thisx->world.pos.y + 170.0f;
                pos.z = Rand_CenteredFloat(5.0f) + thisx->world.pos.z;
                EffectSsLightning_Spawn(play, &pos, &primColor, &envColor, 230, yaw, 6, 0);
            }
        }

        Audio_PlayActorSound2(thisx, NA_SE_EN_BIRI_SPARK - SFX_FLAG);
    }
    thisx->focus.pos = thisx->world.pos;
    Collider_UpdateCylinder(thisx, &this->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    if (thisx->params & 0x80) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderQuad.base);
    }
}

void EnBx_Draw(Actor* thisx, PlayState* play) {
    static void* D_809D2560[] = {
        object_bxa_Tex_0024F0,
        object_bxa_Tex_0027F0,
        object_bxa_Tex_0029F0,
    };
    EnBx* this = (EnBx*)thisx;
    s32 pad;
    Mtx* mtx = Graph_Alloc(play->state.gfxCtx, 4 * sizeof(Mtx));
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0C, mtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_809D2560[this->actor.params & 0x7F]));
    gSPSegment(POLY_OPA_DISP++, 0x09,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 16, 16, 1, 0, (play->gameplayFrames * -10) % 128,
                                32, 32));
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (this->actor.params & 0x80) {
        func_809D1D0C(&this->actor, play);
    }

    this->unk_14E -= 0xBB8;
    thisx->scale.z = thisx->scale.x = (Math_CosS(this->unk_14E) * 0.0075f) + 0.015f;

    for (i = 3; i >= 0; i--) {
        s16 off = (0x2000 * i);

        this->unk_184[i].z = this->unk_184[i].x = (Math_CosS(this->unk_14E + off) * 0.0075f) + 0.015f;
        this->unk_1B4[i].x = thisx->shape.rot.x;
        this->unk_1B4[i].y = thisx->shape.rot.y;
        this->unk_1B4[i].z = thisx->shape.rot.z;
    }

    for (i = 0; i < 4; i++, mtx++) {
        FrameInterpolation_RecordOpenChild(this, this->epoch + i * 25);

        Matrix_Translate(this->unk_154[i].x, this->unk_154[i].y, this->unk_154[i].z, MTXMODE_NEW);
        Matrix_RotateZYX(this->unk_1B4[i].x, this->unk_1B4[i].y, this->unk_1B4[i].z, MTXMODE_APPLY);
        Matrix_Scale(this->unk_184[i].x, this->unk_184[i].y, this->unk_184[i].z, MTXMODE_APPLY);
        MATRIX_TOMTX(mtx);

        FrameInterpolation_RecordCloseChild();
    }

    gSPDisplayList(POLY_OPA_DISP++, object_bxa_DL_0022F0);

    CLOSE_DISPS(play->state.gfxCtx);
}
