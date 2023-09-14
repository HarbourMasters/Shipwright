#include "z_demo_ik.h"
#include "vt.h"
#include "objects/object_ik/object_ik.h"

#define FLAGS ACTOR_FLAG_UPDATE_WHILE_CULLED

void DemoIk_Init(Actor* thisx, PlayState* play);
void DemoIk_Destroy(Actor* thisx, PlayState* play);
void DemoIk_Update(Actor* thisx, PlayState* play);
void DemoIk_Draw(Actor* thisx, PlayState* play);

void DemoIk_Type1Init(DemoIk* this, PlayState* play);
void DemoIk_Type2Init(DemoIk* this, PlayState* play);

void DemoIk_Type1Action0(DemoIk* this, PlayState* play);
void DemoIk_Type1Action1(DemoIk* this, PlayState* play);
void DemoIk_Type1Action2(DemoIk* this, PlayState* play);
void DemoIk_Type2Action0(DemoIk* this, PlayState* play);
void DemoIk_Type2Action1(DemoIk* this, PlayState* play);
void DemoIk_Type2Action2(DemoIk* this, PlayState* play);

void DemoIk_DrawNothing(DemoIk* this, PlayState* play);
void DemoIk_Type1Draw(DemoIk* this, PlayState* play);
void DemoIk_Type2Draw(DemoIk* this, PlayState* play);

void DemoIk_Destroy(Actor* thisx, PlayState* play) {
    DemoIk* this = (DemoIk*)thisx;

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void DemoIk_BgCheck(DemoIk* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, 5);
}

s32 DemoIk_UpdateSkelAnime(DemoIk* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorAction* DemoIk_GetCue(PlayState* play, s32 index) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        return play->csCtx.npcActions[index];
    }
    return NULL;
}

s32 DemoIk_CheckCue(PlayState* play, u16 action, s32 index) {
    CsCmdActorAction* cue = DemoIk_GetCue(play, index);

    if ((cue != NULL) && (cue->action == action)) {
        return 1;
    }
    return 0;
}

void DemoIk_SetMove(DemoIk* this, PlayState* play) {
    this->skelAnime.moveFlags |= 1;
    AnimationContext_SetMoveActor(play, &this->actor, &this->skelAnime, 1.0f);
}

void DemoIk_EndMove(DemoIk* this) {
    this->skelAnime.moveFlags &= ~1;
}

f32 DemoIk_GetCurFrame(DemoIk* this) {
    return this->skelAnime.curFrame;
}

Gfx* DemoIk_SetColors(GraphicsContext* gfxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB) {
    Gfx* head = Graph_Alloc(gfxCtx, 3 * sizeof(Gfx));
    Gfx* entry = head;

    gDPSetPrimColor(entry++, 0x00, 0x00, primR, primG, primB, 255);
    gDPSetEnvColor(entry++, envR, envG, envB, 255);
    gSPEndDisplayList(entry++);
    return head;
}

s32 DemoIk_GetIndexFromParams(s32 params) {
    s32 ret;

    if (params == 0) {
        ret = 5;
    } else if (params == 1) {
        ret = 6;
    } else {
        ret = 7;
    }
    return ret;
}

void DemoIk_Type1PlaySound(DemoIk* this) {
    switch (this->actor.params) {
        case 0:
            if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
                Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_ARMOR_LAND1_DEMO, &this->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
            }
            break;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, 10.0f)) {
                Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_ARMOR_LAND3_DEMO, &this->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
            }
            break;
        case 2:
            if (Animation_OnFrame(&this->skelAnime, 9.0f)) {
                Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_ARMOR_LAND2_DEMO, &this->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
            }
            break;
    }
}

void DemoIk_SpawnDeadDb(DemoIk* this, PlayState* play) {
    static Vec3f deadDbOffsets[] = {
        { -14.0f, 5.0f, 5.0f },  { -20.0f, 12.0f, 0.0f }, { -5.0f, 10.0f, -1.0f }, { -10.0f, 8.0f, 14.0f },
        { -3.0f, 10.0f, 7.0f },  { -10.0f, 11.0f, 0.0f }, { 9.0f, 10.0f, -8.0f },  { 4.0f, 10.0f, 3.0f },
        { -6.0f, 13.0f, -5.0f }, { 1.0f, 9.0f, 3.0f },    { -10.0f, 9.0f, 1.0f },
    };
    s32 i;
    s32 index = DemoIk_GetIndexFromParams(this->actor.params);

    if (DemoIk_CheckCue(play, 5, index)) {
        Vec3f pos;
        Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
        s32 startIndex;
        s32 endIndex;

        if (index == 5) {
            startIndex = 0;
            endIndex = 4;
        } else if (index == 7) {
            startIndex = 4;
            endIndex = 8;
        } else {
            startIndex = 8;
            endIndex = 11;
        }
        for (i = startIndex; i < endIndex; i++) {
            pos.x = deadDbOffsets[i].x + this->actor.world.pos.x;
            pos.y = deadDbOffsets[i].y + this->actor.world.pos.y;
            pos.z = deadDbOffsets[i].z + this->actor.world.pos.z;
            EffectSsDeadDb_Spawn(play, &pos, &zeroVec, &zeroVec, 10, 7, 255, 255, 255, 255, 0, 0, 255, 1, 9, true);
        }
    }
}

void DemoIk_MoveToStartPos(DemoIk* this, PlayState* play, s32 index) {
    CsCmdActorAction* cue = DemoIk_GetCue(play, index);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void DemoIk_Type1Init(DemoIk* this, PlayState* play) {
    s32 pad[3];
    SkeletonHeader* skeleton;
    AnimationHeader* animation;
    f32 phi_f0;

    switch (this->actor.params) {
        case 0:
            skeleton = &object_ik_Skel_000C90;
            animation = &object_ik_Anim_000C6C;
            phi_f0 = 30.0f;
            break;
        case 1:
            skeleton = &object_ik_Skel_000660;
            animation = &object_ik_Anim_000634;
            phi_f0 = 10.0f;
            break;
        default:
            skeleton = &object_ik_Skel_000380;
            animation = &object_ik_Anim_00035C;
            phi_f0 = 20.0f;
            // No break is required for matching
    }
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, phi_f0);
    SkelAnime_Init(play, &this->skelAnime, skeleton, NULL, this->jointTable, this->morphTable, 2);
    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE, 0.0f);
}

void func_8098393C(DemoIk* this) {
    this->actionMode = 0;
    this->drawMode = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_8098394C(DemoIk* this, PlayState* play) {
    DemoIk_EndMove(this);
    DemoIk_MoveToStartPos(this, play, DemoIk_GetIndexFromParams(this->actor.params));
    this->actionMode = 1;
    this->drawMode = 1;
    this->actor.shape.shadowAlpha = 255;
    this->skelAnime.curFrame = 0.0f;
}

void func_809839AC(DemoIk* this) {
    this->actionMode = 2;
    this->drawMode = 1;
    this->actor.shape.shadowAlpha = 255;
    this->skelAnime.curFrame = 0.0f;
}

void func_809839D0(DemoIk* this, PlayState* play) {
    CsCmdActorAction* cue = DemoIk_GetCue(play, DemoIk_GetIndexFromParams(this->actor.params));

    if (cue != NULL) {
        s32 nextCsAction = cue->action;
        s32 csAction = this->csAction;

        if (nextCsAction != csAction) {
            switch (nextCsAction) {
                case 1:
                    func_8098393C(this);
                    break;
                case 2:
                    func_8098394C(this, play);
                    break;
                case 3:
                    func_809839AC(this);
                    break;
                case 4:
                    Actor_Kill(&this->actor);
                    break;
                case 5:
                case 6:
                    break;
                default:
                    // "there is no such action"
                    osSyncPrintf("Demo_Ik_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->csAction = nextCsAction;
        }
    }
}

void DemoIk_Type1Action0(DemoIk* this, PlayState* play) {
    func_809839D0(this, play);
}

void DemoIk_Type1Action1(DemoIk* this, PlayState* play) {
    DemoIk_BgCheck(this, play);
    func_809839D0(this, play);
}

void DemoIk_Type1Action2(DemoIk* this, PlayState* play) {
    DemoIk_UpdateSkelAnime(this);
    DemoIk_Type1PlaySound(this);
    DemoIk_SetMove(this, play);
    DemoIk_BgCheck(this, play);
    DemoIk_SpawnDeadDb(this, play);
    func_809839D0(this, play);
}

void DemoIk_Type1PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    DemoIk* this = (DemoIk*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);
    if (limbIndex == 1) {
        switch (this->actor.params) {
            case 0:
                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, gIronKnuckleArmorRivetAndSymbolDL);
                break;
            case 2:
                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016F88);
                break;
        }
    }
    CLOSE_DISPS(gfxCtx);
}

void DemoIk_Type1Draw(DemoIk* this, PlayState* play) {
    s32 pad[2];
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(gfxCtx);
    func_8002EBCC(&this->actor, play, 0);
    Gfx_SetupDL_25Opa(gfxCtx);
    Gfx_SetupDL_25Xlu(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, DemoIk_SetColors(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, DemoIk_SetColors(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, DemoIk_SetColors(gfxCtx, 255, 255, 255, 20, 40, 30));
    SkelAnime_DrawOpa(play, skelAnime->skeleton, skelAnime->jointTable, NULL, DemoIk_Type1PostLimbDraw, this);
    CLOSE_DISPS(gfxCtx);
}

void DemoIk_Type2Init(DemoIk* this, PlayState* play) {
    s32 pad[2];
    FlexSkeletonHeader* skeleton;
    AnimationHeader* animation;

    switch (this->actor.params) {
        case 3:
            skeleton = &object_ik_Skel_01EB40;
            animation = &object_ik_Anim_01EB14;
            break;
        case 4:
            skeleton = &object_ik_Skel_01EE60;
            animation = &object_ik_Anim_01EE34;
            break;
        case 5:
            skeleton = &object_ik_Skel_000F30;
            animation = &object_ik_Anim_000F0C;
            break;
        default:
            skeleton = &object_ik_Skel_000900;
            animation = &object_ik_Anim_0008DC;
    }

    SkelAnime_InitFlex(play, &this->skelAnime, skeleton, NULL, this->jointTable, this->morphTable, 2);
    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE, 0.0f);
    this->actionMode = 3;
    this->drawMode = 0;
}

void DemoIk_Type2PlaySoundOnFrame(DemoIk* this, f32 frame) {
    if (Animation_OnFrame(&this->skelAnime, frame)) {
        Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_ARMOR_OFF_DEMO, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void DemoIk_Type2PlaySound(DemoIk* this) {
    switch (this->actor.params) {
        case 3:
            DemoIk_Type2PlaySoundOnFrame(this, 33.0f);
            break;
        case 5:
            DemoIk_Type2PlaySoundOnFrame(this, 44.0f);
            break;
    }
}

void func_80983FDC(DemoIk* this) {
    this->actionMode = 3;
    this->drawMode = 0;
}

void func_80983FEC(DemoIk* this, PlayState* play) {
    DemoIk_MoveToStartPos(this, play, 4);
    this->actionMode = 4;
    this->drawMode = 2;
    this->skelAnime.curFrame = 0.0f;
}

void func_8098402C(DemoIk* this) {
    this->actionMode = 5;
    this->drawMode = 2;
    this->skelAnime.curFrame = 0.0f;
}

void func_80984048(DemoIk* this, PlayState* play) {
    CsCmdActorAction* cue = DemoIk_GetCue(play, 4);

    if (cue != NULL) {
        s32 nextCsAction = cue->action;
        s32 csAction = this->csAction;

        if (nextCsAction != csAction) {
            switch (nextCsAction) {
                case 1:
                    func_80983FDC(this);
                    break;
                case 5:
                    func_80983FEC(this, play);
                    break;
                case 6:
                    func_8098402C(this);
                    break;
                case 7:
                    Actor_Kill(&this->actor);
                    break;
                default:
                    // "there is no such action"
                    osSyncPrintf("Demo_Ik_inFace_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->csAction = nextCsAction;
        }
    }
}

void DemoIk_Type2Action0(DemoIk* this, PlayState* play) {
    func_80984048(this, play);
}

void DemoIk_Type2Action1(DemoIk* this, PlayState* play) {
    func_80984048(this, play);
}

void DemoIk_Type2Action2(DemoIk* this, PlayState* play) {
    DemoIk_UpdateSkelAnime(this);
    DemoIk_Type2PlaySound(this);
    func_80984048(this, play);
}

s32 DemoIk_Type2OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                 void* thisx) {
    DemoIk* this = (DemoIk*)thisx;

    if ((limbIndex == 1) && (DemoIk_GetCurFrame(this) < 30.0f)) {
        *dList = NULL;
    }
    return 0;
}

void DemoIk_Type2PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    DemoIk* this = (DemoIk*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 frame = DemoIk_GetCurFrame(this);

    OPEN_DISPS(gfxCtx);
    if (limbIndex == 1 && (frame >= 30.0f)) {
        switch (this->actor.params) {
            case 3:
                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_017028);
                break;
            case 4:
                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_017170);
                break;
            case 5:
                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, gIronKnuckleArmorRivetAndSymbolDL);
                break;
            default:
                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016CD8);
                break;
        }
    }
    CLOSE_DISPS(gfxCtx);
}

void DemoIk_Type2Draw(DemoIk* this, PlayState* play) {
    s32 pad[2];
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(gfxCtx);
    func_8002EBCC(&this->actor, play, 0);
    Gfx_SetupDL_25Opa(gfxCtx);
    Gfx_SetupDL_25Xlu(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, DemoIk_SetColors(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, DemoIk_SetColors(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, DemoIk_SetColors(gfxCtx, 255, 255, 255, 20, 40, 30));
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          DemoIk_Type2OverrideLimbDraw, DemoIk_Type2PostLimbDraw, this);
    CLOSE_DISPS(gfxCtx);
}

static DemoIkActionFunc sActionFuncs[] = {
    DemoIk_Type1Action0, DemoIk_Type1Action1, DemoIk_Type1Action2,
    DemoIk_Type2Action0, DemoIk_Type2Action1, DemoIk_Type2Action2,
};

void DemoIk_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoIk* this = (DemoIk*)thisx;

    if (this->actionMode < 0 || this->actionMode >= ARRAY_COUNT(sActionFuncs) ||
        sActionFuncs[this->actionMode] == NULL) {
        // "The main mode is strange"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->actionMode](this, play);
}

void DemoIk_DrawNothing(DemoIk* this, PlayState* play) {
}

static DemoIkDrawFunc sDrawFuncs[] = {
    DemoIk_DrawNothing,
    DemoIk_Type1Draw,
    DemoIk_Type2Draw,
};

void DemoIk_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoIk* this = (DemoIk*)thisx;

    if (this->drawMode < 0 || this->drawMode >= ARRAY_COUNT(sDrawFuncs) || sDrawFuncs[this->drawMode] == NULL) {
        // "The draw mode is strange"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawMode](this, play);
}

const ActorInit Demo_Ik_InitVars = {
    ACTOR_DEMO_IK,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_IK,
    sizeof(DemoIk),
    (ActorFunc)DemoIk_Init,
    (ActorFunc)DemoIk_Destroy,
    (ActorFunc)DemoIk_Update,
    (ActorFunc)DemoIk_Draw,
    NULL,
};

void DemoIk_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoIk* this = (DemoIk*)thisx;

    if (this->actor.params == 0 || this->actor.params == 1 || this->actor.params == 2) {
        DemoIk_Type1Init(this, play);
    } else {
        DemoIk_Type2Init(this, play);
    }
}
