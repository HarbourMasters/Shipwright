/*
 * File: z_demo_syoten.c
 * Overlay: ovl_Demo_Syoten
 * Description: Ikana Canyon Cleansing Cutscene Effects
 */

#include "z_demo_syoten.h"
#include "objects/object_syoten/object_syoten.h"

#include "2s2h/BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DemoSyoten_Init(Actor* thisx, PlayState* play);
void DemoSyoten_Destroy(Actor* thisx, PlayState* play);
void DemoSyoten_Update(Actor* thisx, PlayState* play);
void DemoSyoten_Draw(Actor* thisx, PlayState* play);

void func_80C16A64(DemoSyoten* this, PlayState* play);
void func_80C16A74(DemoSyoten* this, PlayState* play);
void func_80C16BD4(DemoSyoten* this, PlayState* play);
void func_80C16DD4(DemoSyoten* this, PlayState* play);
void func_80C16EAC(DemoSyoten* this, PlayState* play);
void func_80C17008(DemoSyoten* this, PlayState* play);
void func_80C173B4(Actor* thisx, PlayState* play);
void func_80C17690(Actor* thisx, PlayState* play);

ActorProfile Demo_Syoten_Profile = {
    /**/ ACTOR_DEMO_SYOTEN,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_SYOTEN,
    /**/ sizeof(DemoSyoten),
    /**/ DemoSyoten_Init,
    /**/ DemoSyoten_Destroy,
    /**/ DemoSyoten_Update,
    /**/ DemoSyoten_Draw,
};

u8 D_80C177D0[] = {
    0, 1, 1, 0, 2, 2, 2, 2, 0, 0, 1, 0, 2, 2, 0, 1, 0, 2, 2, 0, 1, 0, 2, 2, 0, 1, 0,
    2, 2, 0, 1, 0, 1, 2, 2, 1, 2, 2, 0, 0, 0, 1, 1, 0, 2, 2, 2, 2, 0, 0, 1, 0, 2, 2,
    0, 1, 0, 2, 2, 0, 1, 0, 2, 2, 2, 0, 0, 2, 0, 1, 1, 0, 2, 2, 2, 0, 1, 0, 2, 2, 0,
};

Color_RGBA8 D_80C17824[] = {
    { 160, 100, 130, 255 },
    { 150, 120, 160, 255 },
    { 130, 90, 150, 255 },
    { 90, 70, 120, 255 },
};

Color_RGBA8 D_80C17834[] = {
    { 110, 50, 80, 255 },
    { 120, 100, 130, 255 },
    { 90, 40, 100, 255 },
    { 60, 50, 100, 255 },
};

void DemoSyoten_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoSyoten* this = (DemoSyoten*)thisx;

    Actor_SetScale(&this->actor, 0.3f);

    this->actionFunc = func_80C16A64;
    this->unk_3E4 = 0;
    this->unk_3E6 = 0;
    this->unk_3DC = NULL;
    this->unk_3E0 = NULL;
    this->cueId = 0;
    this->unk_3D8 = 1.0f;

    switch (DEMOSYOTEN_GET_F(&this->actor)) {
        case DEMOSYOTEN_F_0:
            Keyframe_InitFlex(&this->kfSkelAnime, gSyotenKFSkel_1328, gSyotenKFAnim_023C, this->jointTable,
                              this->morphTable, NULL);
            Keyframe_FlexPlayLoop(&this->kfSkelAnime, gSyotenKFAnim_023C);
            this->actor.draw = NULL;
            this->actionFunc = func_80C16A74;
            this->actor.child =
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EFF_DUST, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0);
            this->cueType = CS_CMD_ACTOR_CUE_533;
            Actor_SetScale(&this->actor, 0.05f);
            break;

        case DEMOSYOTEN_F_1:
            Actor_SetScale(&this->actor, 0.1f);
            this->unk_3DC = object_syoten_DL_001370;
            this->unk_3E0 = Lib_SegmentedToVirtual(&object_syoten_Matanimheader_001448);
            this->unk_3E4 |= 1;
            this->actionFunc = func_80C16BD4;
            this->cueType = CS_CMD_ACTOR_CUE_533;
            Actor_SetScale(&this->actor, 0.05f);
            break;

        case DEMOSYOTEN_F_2:
            this->unk_3DC = object_syoten_DL_001730;
            this->unk_3E0 = Lib_SegmentedToVirtual(&object_syoten_Matanimheader_0018B8);
            this->cueType = CS_CMD_ACTOR_CUE_534;
            this->actionFunc = func_80C16DD4;
            this->unk_3E4 |= 2;
            Actor_SetScale(&this->actor, 4.0f);
            break;

        case DEMOSYOTEN_F_3:
            this->unk_3DC = object_syoten_DL_001DD0;
            this->unk_3E0 = Lib_SegmentedToVirtual(&object_syoten_Matanimheader_002B98);
            this->cueType = CS_CMD_ACTOR_CUE_536;
            this->unk_3E4 |= 8;
            this->actionFunc = func_80C16EAC;
            Actor_SetScale(&this->actor, 0.5f);
            break;

        case DEMOSYOTEN_F_4:
            this->unk_3DC = object_syoten_DL_002880;
            this->unk_3E0 = Lib_SegmentedToVirtual(&object_syoten_Matanimheader_002B88);
            this->unk_3E4 |= 2;
            this->actionFunc = func_80C17008;
            this->cueType = CS_CMD_ACTOR_CUE_535;
            this->unk_3E4 |= 1;
            this->unk_3E4 |= 8;
            this->actor.draw = func_80C17690;
            Actor_SetScale(&this->actor, 5.0f);
            break;
    }
}

void DemoSyoten_Destroy(Actor* thisx, PlayState* play) {
    DemoSyoten* this = (DemoSyoten*)thisx;

    if (DEMOSYOTEN_GET_F(&this->actor) == DEMOSYOTEN_F_0) {
        Keyframe_DestroyFlex(&this->kfSkelAnime);
    }
}

void func_80C16760(DemoSyoten* this, PlayState* play) {
    s32 pad;
    Vec3s* points;
    Vec3f sp2C;

    this->unk_3EC = 0;
    if (DEMOSYOTEN_GET_PATH_INDEX(&this->actor) != DEMOSYOTEN_PATH_INDEX_NONE) {
        this->path = &play->setupPathList[DEMOSYOTEN_GET_PATH_INDEX(&this->actor)];
        if (this->path != NULL) {
            points = Lib_SegmentedToVirtual(this->path->points);
            Math_Vec3s_ToVec3f(&this->actor.world.pos, &points[0]);
            this->unk_3EC++;
            points++;
            Math_Vec3s_ToVec3f(&sp2C, points);
            this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &sp2C);
            this->actor.world.rot.x = Math_Vec3f_Pitch(&this->actor.world.pos, &sp2C);
        }
    } else {
        this->path = NULL;
    }
}

s32 func_80C16818(DemoSyoten* this) {
    s32 pad;
    Path* path = this->path;
    Vec3s* points;
    Vec3f sp28;

    if (path == NULL) {
        return true;
    }

    points = Lib_SegmentedToVirtual(this->path->points);
    points += this->unk_3EC;
    Math_Vec3s_ToVec3f(&sp28, points);
    this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &sp28);
    this->actor.world.rot.x = Math_Vec3f_Pitch(&this->actor.world.pos, &sp28);
    Actor_MoveWithoutGravityReverse(&this->actor);
    if (Math_Vec3f_DistXYZ(&this->actor.world.pos, &sp28) < 2.0f) {
        return true;
    }
    return false;
}

void func_80C168D0(DemoSyoten* this, PlayState* play) {
    s32 pad;
    Path* path = this->path;
    Vec3s* points;
    Vec3f worldPos;
    Vec3f projectedPos;
    f32 invW;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(this->path->points);
        points += this->unk_3EC;
        Math_Vec3s_ToVec3f(&worldPos, points);
        Actor_GetProjectedPos(play, &worldPos, &projectedPos, &invW);
        Math_Vec3f_Copy(&worldPos, &this->actor.projectedPos);
        worldPos.z = worldPos.y;
        projectedPos.z = projectedPos.y;
        this->actor.shape.rot.z = -Math_Vec3f_Yaw(&projectedPos, &worldPos);
    }
}

void func_80C16974(f32 arg0) {
    Vtx* vtx = ResourceMgr_LoadVtxByName(Lib_SegmentedToVirtual(&object_syotenVtx_0018C0));
    u8 sp20[3];
    s32 i;

    arg0 = CLAMP_MAX(arg0, 1.0f);

    sp20[0] = (s8)0.0f;
    sp20[1] = (s8)(152.0f * arg0);
    sp20[2] = (s8)(255.0f * arg0);

    for (i = 0; i < ARRAY_COUNT(D_80C177D0); i++, vtx++) {
        vtx->v.cn[3] = sp20[D_80C177D0[i]];
    }
}

void func_80C16A64(DemoSyoten* this, PlayState* play) {
}

void func_80C16A74(DemoSyoten* this, PlayState* play) {
    u16 cueId;

    Keyframe_UpdateFlex(&this->kfSkelAnime);
    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        if ((play->csCtx.curFrame >= 160) && (play->csCtx.curFrame < 322)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_IKANA_SOUL_LV - SFX_FLAG);
        } else if (play->csCtx.curFrame == 322) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_IKANA_SOUL_TRANSFORM);
        }

        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (cueId) {
                case 1:
                    this->actor.draw = NULL;
                    break;

                case 2:
                    this->actor.draw = func_80C173B4;
                    this->unk_3D8 = 1.0f;
                    break;

                case 3:
                    break;

                case 4:
                    this->actor.draw = NULL;
                    break;

                default:
                    break;
            }
        }

        if (cueId == 3) {
            if (this->unk_3D8 > 0.0125f) {
                this->unk_3D8 -= 0.0125f;
                if (this->actor.child != NULL) {
                    this->actor.child->home.rot.z = 0x14;
                }
            } else {
                this->actor.draw = NULL;
                if (this->actor.child != NULL) {
                    this->actor.child->home.rot.z = 0;
                }
            }
        }
    } else {
        this->actor.draw = NULL;
    }
}

void func_80C16BD4(DemoSyoten* this, PlayState* play) {
    s32 pad;
    u16 cueId;

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (cueId) {
                default:
                    this->actor.draw = NULL;
                    break;

                case 1:
                case 2:
                    this->actor.draw = NULL;
                    Actor_SetScale(&this->actor, 0.0f);
                    break;

                case 3:
                    this->actor.draw = DemoSyoten_Draw;
                    this->unk_3E4 &= ~4;
                    this->unk_3E4 |= 1;
                    this->actor.shape.rot.z = 0;
                    break;

                case 4:
                    this->actor.draw = DemoSyoten_Draw;
                    func_80C16760(this, play);
                    this->unk_3E4 |= 4;
                    this->unk_3E4 &= ~1;
                    break;
            }
        }

        switch (cueId) {
            case 3:
                if (this->actor.scale.x < 0.05f) {
                    this->actor.scale.x += 0.000625f;
                }
                Actor_SetScale(&this->actor, this->actor.scale.x);
                break;

            case 4:
                this->actor.speed =
                    play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->rot.z * 0.005493164f;
                if (this->unk_3EC < this->path->count) {
                    if (func_80C16818(this)) {
                        this->unk_3EC++;
                    }
                    func_80C168D0(this, play);
                }
                break;
        }
    } else {
        this->actor.draw = NULL;
    }
}

void func_80C16DD4(DemoSyoten* this, PlayState* play) {
    s32 pad;
    u16 cueId;

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (cueId) {
                case 1:
                    this->actor.draw = NULL;
                    break;

                case 2:
                    this->actor.draw = DemoSyoten_Draw;
                    this->unk_3E6 = -6;
                    Actor_PlaySfx(&this->actor, NA_SE_IT_DM_FLYING_GOD_DASH);
                    break;
            }
        }

        if ((cueId == 2) && (this->unk_3E6 < 40)) {
            this->unk_3E6++;
        }
    } else {
        this->actor.draw = NULL;
    }
}

void func_80C16EAC(DemoSyoten* this, PlayState* play) {
    s32 pad;
    u16 cueId;

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (cueId) {
                case 1:
                    this->actor.draw = NULL;
                    this->unk_3D8 = 0.0f;
                    Actor_SetScale(&this->actor, 0.5f);
                    break;

                case 2:
                    this->actor.draw = DemoSyoten_Draw;
                    break;
            }
        }

        if (cueId == 2) {
            if (this->actor.scale.x < 8.0f) {
                this->actor.scale.x += 0.1875f;
            }

            Actor_SetScale(&this->actor, this->actor.scale.x);

            if (this->unk_3D8 < 1.0f) {
                this->unk_3D8 += 0.025f;
            }

            if (this->unk_3D8 > 1.0f) {
                this->unk_3D8 = 1.0f;
            }
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_IKANA_PURIFICATION - SFX_FLAG);
        }
    } else {
        this->actor.draw = NULL;
    }
    func_80C16974(this->unk_3D8);
}

void func_80C17008(DemoSyoten* this, PlayState* play) {
    u16 cueId;

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (cueId) {
                case 1:
                    this->actor.draw = NULL;
                    break;

                case 2:
                    this->actor.draw = func_80C17690;
                    this->unk_3E6 = -10;
                    break;
            }
        }

        if ((cueId == 2) && (this->unk_3E6 < 15)) {
            this->unk_3E6++;
        }
    } else {
        this->actor.draw = NULL;
    }
}

void DemoSyoten_Update(Actor* thisx, PlayState* play) {
    DemoSyoten* this = (DemoSyoten*)thisx;

    this->actionFunc(this, play);
}

s32 DemoSyoten_OverrideLimbDraw(PlayState* play, KFSkelAnimeFlex* kfSkelAnime, s32 limbIndex, Gfx** dList, u8* flags,
                                void* thisx, Vec3f* scale, Vec3s* rot, Vec3f* pos) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    DemoSyoten* this = (DemoSyoten*)thisx;

    OPEN_DISPS(gfxCtx);

    switch (limbIndex) {
        case 2:
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 240, 140, (s32)(this->unk_3D8 * 150.0f));
            break;

        case 4:
        case 12:
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 240, 190, (s32)(this->unk_3D8 * 200.0f));
            break;

        case 6:
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 240, 150, (s32)(this->unk_3D8 * 150.0f));
            break;

        case 8:
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 240, 180, (s32)(this->unk_3D8 * 200.0f));
            break;

        case 10:
        case 14:
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 240, 170, (s32)(this->unk_3D8 * 150.0f));
            break;

        case 16:
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 240, 160, (s32)(this->unk_3D8 * 200.0f));
            break;
    }

    CLOSE_DISPS(gfxCtx);
    return true;
}

void func_80C173B4(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoSyoten* this = (DemoSyoten*)thisx;
    Mtx* mtxStack;

    AnimatedMat_DrawXlu(play, Lib_SegmentedToVirtual(&object_syoten_Matanimheader_001298));

    mtxStack = GRAPH_ALLOC(play->state.gfxCtx, this->kfSkelAnime.skeleton->dListCount * sizeof(Mtx));

    if (mtxStack != NULL) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
        Keyframe_DrawFlex(play, &this->kfSkelAnime, mtxStack, DemoSyoten_OverrideLimbDraw, NULL, &this->actor);
    }
}

void func_80C17468(PlayState* play) {
    Color_RGBA8 sp2C;
    Color_RGBA8 sp28;

    OPEN_DISPS(play->state.gfxCtx);

    Environment_LerpSandstormColors(D_80C17824, &sp2C);
    Environment_LerpSandstormColors(D_80C17834, &sp28);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, sp2C.r, sp2C.g, sp2C.b, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, sp28.r, sp28.g, sp28.b, 255);

    CLOSE_DISPS(play->state.gfxCtx);
}

void DemoSyoten_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoSyoten* this = (DemoSyoten*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    if (this->unk_3E4 & 4) {
        Matrix_RotateZS(-this->actor.shape.rot.z, MTXMODE_APPLY);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
        Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
        Matrix_Scale(1.0f, 5.0f, 1.0f, MTXMODE_APPLY);
    }

    if (this->unk_3E4 & 1) {
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    }

    if (this->unk_3E4 & 8) {
        func_80C17468(play);
    }

    if (this->unk_3E0 != NULL) {
        if (this->unk_3E4 & 2) {
            AnimatedMat_DrawStepXlu(play, this->unk_3E0, this->unk_3E6);
        } else {
            AnimatedMat_DrawXlu(play, this->unk_3E0);
        }
    }

    if (this->unk_3DC != NULL) {
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, this->unk_3DC);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80C17690(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoSyoten* this = (DemoSyoten*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Matrix_RotateYS(BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))), MTXMODE_APPLY);

    if (this->unk_3E4 & 8) {
        func_80C17468(play);
    }

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    AnimatedMat_DrawStepXlu(play, this->unk_3E0, this->unk_3E6);

    gSPDisplayList(POLY_XLU_DISP++, object_syoten_DL_002880);

    AnimatedMat_DrawStepXlu(play, this->unk_3E0, this->unk_3E6 + 5);

    gSPDisplayList(POLY_XLU_DISP++, object_syoten_DL_002A20);

    CLOSE_DISPS(play->state.gfxCtx);
}
