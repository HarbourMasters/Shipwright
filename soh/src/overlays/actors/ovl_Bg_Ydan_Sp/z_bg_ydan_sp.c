/*
 * File: z_bg_ydan_sp.c
 * Overlay: ovl_Bg_Ydan_Sp
 * Description: Webs
 */

#include "z_bg_ydan_sp.h"
#include "objects/object_ydan_objects/object_ydan_objects.h"

#define FLAGS 0

void BgYdanSp_Init(Actor* thisx, PlayState* play);
void BgYdanSp_Destroy(Actor* thisx, PlayState* play);
void BgYdanSp_Update(Actor* thisx, PlayState* play);
void BgYdanSp_Draw(Actor* thisx, PlayState* play);

void BgYdanSp_BurnFloorWeb(BgYdanSp* this, PlayState* play);
void BgYdanSp_FloorWebIdle(BgYdanSp* this, PlayState* play);
void BgYdanSp_BurnWallWeb(BgYdanSp* this, PlayState* play);
void BgYdanSp_WallWebIdle(BgYdanSp* this, PlayState* play);

//extern CollisionHeader gDTWebWallCol;

typedef enum {
    /* 0 */ WEB_FLOOR,
    /* 1 */ WEB_WALL
} BgYdanSpType;

const ActorInit Bg_Ydan_Sp_InitVars = {
    ACTOR_BG_YDAN_SP,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_YDAN_OBJECTS,
    sizeof(BgYdanSp),
    (ActorFunc)BgYdanSp_Init,
    (ActorFunc)BgYdanSp_Destroy,
    (ActorFunc)BgYdanSp_Update,
    (ActorFunc)BgYdanSp_Draw,
    NULL,
};

static ColliderTrisElementInit sTrisItemsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x00 },
            { 0x00020800, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 75.0f, -8.0f, 75.0f }, { -75.0f, -8.0f, 75.0f }, { -75.0f, -8.0f, -75.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x00 },
            { 0x00020800, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 140.0f, 288.8f, 0.0f }, { -140.0f, 288.0f, 0.0f }, { -140.0f, 0.0f, 0.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    sTrisItemsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgYdanSp_Init(Actor* thisx, PlayState* play) {
    BgYdanSp* this = (BgYdanSp*)thisx;
    ColliderTrisElementInit* ti0 = &sTrisItemsInit[0];
    Vec3f tri[3];
    s32 i;
    CollisionHeader* colHeader = NULL;
    ColliderTrisElementInit* ti1 = &sTrisItemsInit[1];
    f32 cossY;
    f32 sinsY;
    f32 cossX;
    f32 nSinsX;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->isDestroyedSwitchFlag = thisx->params & 0x3F;
    this->burnSwitchFlag = (thisx->params >> 6) & 0x3F;
    this->dyna.actor.params = (thisx->params >> 0xC) & 0xF;
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
    Collider_InitTris(play, &this->trisCollider);
    Collider_SetTris(play, &this->trisCollider, &this->dyna.actor, &sTrisInit, this->trisColliderItems);
    if (this->dyna.actor.params == WEB_FLOOR) {
        CollisionHeader_GetVirtual(&gDTWebFloorCol, &colHeader);
        this->actionFunc = BgYdanSp_FloorWebIdle;

        for (i = 0; i < 3; i++) {
            tri[i].x = ti0->dim.vtx[i].x + this->dyna.actor.world.pos.x;
            tri[i].y = ti0->dim.vtx[i].y + this->dyna.actor.world.pos.y;
            tri[i].z = ti0->dim.vtx[i].z + this->dyna.actor.world.pos.z;
        }

        Collider_SetTrisVertices(&this->trisCollider, 0, &tri[0], &tri[1], &tri[2]);
        tri[1].x = tri[0].x;
        tri[1].z = tri[2].z;
        Collider_SetTrisVertices(&this->trisCollider, 1, &tri[0], &tri[2], &tri[1]);
        this->unk_16C = 0.0f;
    } else {
        CollisionHeader_GetVirtual(&gDTWebWallCol, &colHeader);
        this->actionFunc = BgYdanSp_WallWebIdle;
        Actor_SetFocus(&this->dyna.actor, 30.0f);
        sinsY = Math_SinS(this->dyna.actor.shape.rot.y);
        cossY = Math_CosS(this->dyna.actor.shape.rot.y);
        nSinsX = -Math_SinS(this->dyna.actor.shape.rot.x);
        cossX = Math_CosS(this->dyna.actor.shape.rot.x);

        for (i = 0; i < 3; i++) {
            tri[i].x =
                this->dyna.actor.world.pos.x + (cossY * ti1->dim.vtx[i].x) - (sinsY * ti1->dim.vtx[i].y * nSinsX);
            tri[i].y = this->dyna.actor.world.pos.y + (ti1->dim.vtx[i].y * cossX);
            tri[i].z =
                this->dyna.actor.world.pos.z - (sinsY * ti1->dim.vtx[i].x) + (ti1->dim.vtx[i].y * cossY * nSinsX);
        }

        Collider_SetTrisVertices(&this->trisCollider, 0, &tri[0], &tri[1], &tri[2]);

        tri[1].x = this->dyna.actor.world.pos.x + (cossY * ti1->dim.vtx[0].x) - (ti1->dim.vtx[2].y * sinsY * nSinsX);
        tri[1].y = this->dyna.actor.world.pos.y + (ti1->dim.vtx[2].y * cossX);
        tri[1].z = this->dyna.actor.world.pos.z - (sinsY * ti1->dim.vtx[0].x) + (ti1->dim.vtx[2].y * cossY * nSinsX);
        Collider_SetTrisVertices(&this->trisCollider, 1, &tri[0], &tri[2], &tri[1]);
    }
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->timer = 0;
    if (Flags_GetSwitch(play, this->isDestroyedSwitchFlag)) {
        Actor_Kill(&this->dyna.actor);
    }
}

void BgYdanSp_Destroy(Actor* thisx, PlayState* play) {
    BgYdanSp* this = (BgYdanSp*)thisx;
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyTris(play, &this->trisCollider);
}

void BgYdanSp_UpdateFloorWebCollision(BgYdanSp* this) {
    s16 newY;
    CollisionHeader* colHeader;

    colHeader = ResourceMgr_LoadColByName(SEGMENTED_TO_VIRTUAL(&gDTWebFloorCol));
    colHeader->vtxList = SEGMENTED_TO_VIRTUAL(colHeader->vtxList);
    newY = (this->dyna.actor.home.pos.y - this->dyna.actor.world.pos.y) * 10;
    colHeader->vtxList[14].y = newY;
    colHeader->vtxList[12].y = newY;
    colHeader->vtxList[10].y = newY;
    colHeader->vtxList[9].y = newY;
    colHeader->vtxList[6].y = newY;
    colHeader->vtxList[5].y = newY;
    colHeader->vtxList[1].y = newY;
    colHeader->vtxList[0].y = newY;
}

void BgYdanSp_BurnWeb(BgYdanSp* this, PlayState* play) {
    this->timer = 30;
    this = this;
    func_80078884(NA_SE_SY_CORRECT_CHIME);
    Flags_SetSwitch(play, this->isDestroyedSwitchFlag);
    if (this->dyna.actor.params == WEB_FLOOR) {
        this->actionFunc = BgYdanSp_BurnFloorWeb;
    } else {
        this->actionFunc = BgYdanSp_BurnWallWeb;
    }
}

void BgYdanSp_BurnFloorWeb(BgYdanSp* this, PlayState* play) {
    static Vec3f accel = { 0 };
    Vec3f velocity;
    Vec3f pos2;
    f32 distXZ;
    f32 sins;
    f32 coss;
    s16 rot;
    s16 rot2;
    s32 i;

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        Actor_Kill(&this->dyna.actor);
        return;
    }
    if ((this->timer % 3) == 0) {
        rot2 = Rand_ZeroOne() * 0x2AAA;
        velocity.y = 0.0f;
        pos2.y = this->dyna.actor.world.pos.y;

        for (i = 0; i < 6; i++) {
            rot = Rand_CenteredFloat(0x2800) + rot2;
            sins = Math_SinS(rot);
            coss = Math_CosS(rot);
            pos2.x = this->dyna.actor.world.pos.x + (120.0f * sins);
            pos2.z = this->dyna.actor.world.pos.z + (120.0f * coss);
            distXZ = Math_Vec3f_DistXZ(&this->dyna.actor.home.pos, &pos2) * (1.0f / 120.0f);
            if (distXZ < 0.7f) {
                sins = Math_SinS(rot + 0x8000);
                coss = Math_CosS(rot + 0x8000);
                pos2.x = this->dyna.actor.world.pos.x + (120.0f * sins);
                pos2.z = this->dyna.actor.world.pos.z + (120.0f * coss);
                distXZ = Math_Vec3f_DistXZ(&this->dyna.actor.home.pos, &pos2) * (1.0f / 120.0f);
            }
            velocity.x = (7.0f * sins) * distXZ;
            velocity.y = 0.0f;
            velocity.z = (7.0f * coss) * distXZ;
            EffectSsDeadDb_Spawn(play, &this->dyna.actor.home.pos, &velocity, &accel, 60, 6, 255, 255, 150, 170,
                                 255, 0, 0, 1, 0xE, 1);
            rot2 += 0x2AAA;
        }
    }
}

void BgYdanSp_FloorWebBroken(BgYdanSp* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        Actor_Kill(&this->dyna.actor);
    }
}

void BgYdanSp_FloorWebBreaking(BgYdanSp* this, PlayState* play) {
    static Color_RGBA8 primColor = { 250, 250, 250, 255 };
    static Color_RGBA8 envColor = { 180, 180, 180, 255 };
    static Vec3f zeroVec = { 0 };
    s32 i;
    Vec3f pos;
    s16 rot;

    if (this->timer != 0) {
        this->timer--;
    }

    this->dyna.actor.world.pos.y = (sinf((f32)this->timer * (M_PI / 20)) * this->unk_16C) + this->dyna.actor.home.pos.y;
    if (this->dyna.actor.home.pos.y - this->dyna.actor.world.pos.y > 190.0f) {
        func_8003EBF8(play, &play->colCtx.dyna, this->dyna.bgId);
        this->timer = 40;
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        Flags_SetSwitch(play, this->isDestroyedSwitchFlag);
        this->actionFunc = BgYdanSp_FloorWebBroken;
        pos.y = this->dyna.actor.world.pos.y - 60.0f;
        rot = 0;
        for (i = 0; i < 6; i++) {
            pos.x = Math_SinS(rot) * 60.0f + this->dyna.actor.world.pos.x;
            pos.z = Math_CosS(rot) * 60.0f + this->dyna.actor.world.pos.z;
            func_8002829C(play, &pos, &zeroVec, &zeroVec, &primColor, &envColor, 1000, 10);

            rot += 0x2AAA;
        }
    }
    BgYdanSp_UpdateFloorWebCollision(this);
}

void BgYdanSp_FloorWebIdle(BgYdanSp* this, PlayState* play) {
    Player* player;
    Vec3f webPos;
    f32 sqrtFallDistance;
    f32 unk;

    player = GET_PLAYER(play);
    webPos.x = this->dyna.actor.world.pos.x;
    webPos.y = this->dyna.actor.world.pos.y - 50.0f;
    webPos.z = this->dyna.actor.world.pos.z;
    if (Player_IsBurningStickInRange(play, &webPos, 70.0f, 50.0f) != 0) {
        this->dyna.actor.home.pos.x = player->meleeWeaponInfo[0].tip.x;
        this->dyna.actor.home.pos.z = player->meleeWeaponInfo[0].tip.z;
        BgYdanSp_BurnWeb(this, play);
        return;
    }
    if ((this->trisCollider.base.acFlags & 2) != 0) {
        BgYdanSp_BurnWeb(this, play);
        return;
    }
    if (func_8004356C(&this->dyna)) {
        sqrtFallDistance = sqrtf(CLAMP_MIN(player->fallDistance, 0.0f));
        if (player->fallDistance > 750.0f) {
            if (this->dyna.actor.xzDistToPlayer < 80.0f) {
                this->unk_16C = 200.0f;
                this->dyna.actor.room = -1;
                this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED;
                this->timer = 40;
                Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_WEB_BROKEN);
                this->actionFunc = BgYdanSp_FloorWebBreaking;
                return;
            }
        }
        unk = sqrtFallDistance + sqrtFallDistance;
        if (this->unk_16C < unk) {
            if (unk > 2.0f) {
                this->unk_16C = unk;
                this->timer = 14;
            }
        }
        if (player->actor.speedXZ != 0.0f) {
            if (this->unk_16C < 0.1f) {
                this->timer = 14;
            }
            if (this->unk_16C < 2.0f) {
                this->unk_16C = 2.0f;
            } else {
                this->unk_16C = this->unk_16C;
            }
        }
    }
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->timer = 14;
    }
    this->dyna.actor.world.pos.y = sinf((f32)this->timer * (M_PI / 7)) * this->unk_16C + this->dyna.actor.home.pos.y;
    Math_ApproachZeroF(&this->unk_16C, 1.0f, 0.8f);
    if (this->timer == 13) {
        if (this->unk_16C > 3.0f) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_WEB_VIBRATION);
        } else {
            Audio_StopSfxById(NA_SE_EV_WEB_VIBRATION);
        }
    }
    BgYdanSp_UpdateFloorWebCollision(this);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->trisCollider.base);
}

void BgYdanSp_BurnWallWeb(BgYdanSp* this, PlayState* play) {
    static Vec3f accel = { 0 };
    Vec3f velocity;
    Vec3f spC8;
    f32 distXYZ;
    f32 sins;
    f32 coss;
    f32 coss2;
    s16 rot;
    s16 rot2;
    s32 i;

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        Actor_Kill(&this->dyna.actor);
        return;
    }
    if ((this->timer % 3) == 0) {
        rot2 = Rand_ZeroOne() * 0x2AAA;

        for (i = 0; i < 6; i++) {
            rot = Rand_CenteredFloat(0x2800) + rot2;
            sins = Math_SinS(rot);
            coss = Math_CosS(rot);
            coss2 = Math_CosS(this->dyna.actor.shape.rot.y) * sins;
            sins *= Math_SinS(this->dyna.actor.shape.rot.y);

            spC8.x = this->dyna.actor.world.pos.x + (140.0f * coss2);
            spC8.y = this->dyna.actor.world.pos.y + (140.0f * (1.0f + coss));
            spC8.z = this->dyna.actor.world.pos.z - (140.0f * sins);
            distXYZ = Math_Vec3f_DistXYZ(&this->dyna.actor.home.pos, &spC8) * (1.0f / 140.0f);
            if (distXYZ < 0.65f) {
                sins = Math_SinS(rot + 0x8000);
                coss = Math_CosS(rot + 0x8000);
                coss2 = Math_CosS(this->dyna.actor.shape.rot.y) * sins;
                sins *= Math_SinS(this->dyna.actor.shape.rot.y);
                spC8.x = this->dyna.actor.world.pos.x + (140.0f * coss2);
                spC8.y = this->dyna.actor.world.pos.y + (140.0f * (1.0f + coss));
                spC8.z = this->dyna.actor.world.pos.z - (140.0f * sins);
                distXYZ = Math_Vec3f_DistXYZ(&this->dyna.actor.home.pos, &spC8) * (1.0f / 140.0f);
            }
            velocity.x = 6.5f * coss2 * distXYZ;
            velocity.y = 6.5f * coss * distXYZ;
            velocity.z = -6.5f * sins * distXYZ;
            EffectSsDeadDb_Spawn(play, &this->dyna.actor.home.pos, &velocity, &accel, 80, 6, 255, 255, 150, 170,
                                 255, 0, 0, 1, 0xE, 1);
            rot2 += 0x2AAA;
        }
    }
}

void BgYdanSp_WallWebIdle(BgYdanSp* this, PlayState* play) {
    Player* player;
    Vec3f sp30;

    player = GET_PLAYER(play);
    if (Flags_GetSwitch(play, this->burnSwitchFlag) || (this->trisCollider.base.acFlags & 2)) {
        this->dyna.actor.home.pos.y = this->dyna.actor.world.pos.y + 80.0f;
        BgYdanSp_BurnWeb(this, play);
    } else if (player->heldItemAction == PLAYER_IA_DEKU_STICK && player->unk_860 != 0) {
        func_8002DBD0(&this->dyna.actor, &sp30, &player->meleeWeaponInfo[0].tip);
        if (fabsf(sp30.x) < 100.0f && sp30.z < 1.0f && sp30.y < 200.0f) {
            OnePointCutscene_Init(play, 3020, 40, &this->dyna.actor, MAIN_CAM);
            Math_Vec3f_Copy(&this->dyna.actor.home.pos, &player->meleeWeaponInfo[0].tip);
            BgYdanSp_BurnWeb(this, play);
        }
    }
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->trisCollider.base);
}

void BgYdanSp_Update(Actor* thisx, PlayState* play) {
    BgYdanSp* this = (BgYdanSp*)thisx;

    this->actionFunc(this, play);
}

void BgYdanSp_Draw(Actor* thisx, PlayState* play) {
    BgYdanSp* this = (BgYdanSp*)thisx;
    s32 i;
    MtxF mtxF;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    if (thisx->params == WEB_WALL) {
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gDTWebWallDL);
    } else if (this->actionFunc == BgYdanSp_FloorWebBroken) {
        Matrix_Get(&mtxF);
        if (this->timer == 40) {
            Matrix_Translate(0.0f, (thisx->home.pos.y - thisx->world.pos.y) * 10.0f, 0.0f, MTXMODE_APPLY);
            Matrix_Scale(1.0f, ((thisx->home.pos.y - thisx->world.pos.y) + 10.0f) * 0.1f, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gDTWebFloorDL);
        }
        for (i = 0; i < 8; i++) {
            Matrix_Put(&mtxF);
            Matrix_RotateZYX(-0x5A0, i * 0x2000, 0, MTXMODE_APPLY);
            Matrix_Translate(0.0f, 700.0f, -900.0f, MTXMODE_APPLY);
            Matrix_Scale(3.5f, 5.0f, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gDTUnknownWebDL);
        }
    } else {
        Matrix_Translate(0.0f, (thisx->home.pos.y - thisx->world.pos.y) * 10.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(1.0f, ((thisx->home.pos.y - thisx->world.pos.y) + 10.0f) * 0.1f, 1.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gDTWebFloorDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
