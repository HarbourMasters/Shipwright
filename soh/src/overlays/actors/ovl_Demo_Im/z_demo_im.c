/*
 * File: z_demo_im.c
 * Overlay: Demo_Im
 * Description: Impa
 */

#include "z_demo_im.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "scenes/indoors/nakaniwa/nakaniwa_scene.h"
#include "objects/object_im/object_im.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4)

void DemoIm_Init(Actor* thisx, GlobalContext* globalCtx);
void DemoIm_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DemoIm_Update(Actor* thisx, GlobalContext* globalCtx);
void DemoIm_Draw(Actor* thisx, GlobalContext* globalCtx);
void func_809856F8(DemoIm* this, GlobalContext* globalCtx);
void func_80985718(DemoIm* this, GlobalContext* globalCtx);
void func_80985738(DemoIm* this, GlobalContext* globalCtx);
void func_80985770(DemoIm* this, GlobalContext* globalCtx);
void func_809857B0(DemoIm* this, GlobalContext* globalCtx);
void func_809857F0(DemoIm* this, GlobalContext* globalCtx);
void func_80985830(DemoIm* this, GlobalContext* globalCtx);
void func_80985C10(DemoIm* this, GlobalContext* globalCtx);
void func_80985C40(DemoIm* this, GlobalContext* globalCtx);
void func_80985C94(DemoIm* this, GlobalContext* globalCtx);
void DemoIm_DrawTranslucent(DemoIm* this, GlobalContext* globalCtx);
void func_809863BC(DemoIm* this, GlobalContext* globalCtx);
void func_809863DC(DemoIm* this, GlobalContext* globalCtx);
void func_80986430(DemoIm* this, GlobalContext* globalCtx);
void func_80986494(DemoIm* this, GlobalContext* globalCtx);
void func_809864D4(DemoIm* this, GlobalContext* globalCtx);
void func_809868E8(DemoIm* this, GlobalContext* globalCtx);
void func_80986908(DemoIm* this, GlobalContext* globalCtx);
void func_80986948(DemoIm* this, GlobalContext* globalCtx);
void func_80986D40(DemoIm* this, GlobalContext* globalCtx);
void func_80986DC8(DemoIm* this, GlobalContext* globalCtx);
void func_80986E20(DemoIm* this, GlobalContext* globalCtx);
void func_80986E40(DemoIm* this, GlobalContext* globalCtx);
void func_80986EAC(DemoIm* this, GlobalContext* globalCtx);
void func_80986F08(DemoIm* this, GlobalContext* globalCtx);
void func_80986F28(DemoIm* this, GlobalContext* globalCtx);
void func_80986F88(DemoIm* this, GlobalContext* globalCtx);
void func_80986FA8(DemoIm* this, GlobalContext* globalCtx);
void func_80987288(DemoIm* this, GlobalContext* globalCtx);
void func_809872A8(DemoIm* this, GlobalContext* globalCtx);
void func_809872F0(DemoIm* this, GlobalContext* globalCtx);
void func_80987330(DemoIm* this, GlobalContext* globalCtx);
void DemoIm_DrawNothing(DemoIm* this, GlobalContext* globalCtx);
void DemoIm_DrawSolid(DemoIm* this, GlobalContext* globalCtx);

static void* sEyeTextures[] = {
    gImpaEyeOpenTex,
    gImpaEyeHalfTex,
    gImpaEyeClosedTex,
};

static u32 D_8098783C = 0;

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, 0x00, 0x00, 0x01 },
    { 25, 80, 0, { 0, 0, 0 } },
};

#include "z_demo_im_cutscene_data.c" EARLY

static DemoImActionFunc sActionFuncs[] = {
    func_809856F8, func_80985718, func_80985738, func_80985770, func_809857B0, func_809857F0, func_80985830,
    func_80985C10, func_80985C40, func_80985C94, func_809863BC, func_809863DC, func_80986430, func_80986494,
    func_809864D4, func_809868E8, func_80986908, func_80986948, func_80986D40, func_80986DC8, func_80986E20,
    func_80986E40, func_80986EAC, func_80986F08, func_80986F28, func_80986F88, func_80986FA8, func_80987288,
    func_809872A8, func_809872F0, func_80987330,
};

static Vec3f D_809887D8 = { 0.0f, 10.0f, 0.0f };

static DemoImDrawFunc sDrawFuncs[] = {
    DemoIm_DrawNothing,
    DemoIm_DrawSolid,
    DemoIm_DrawTranslucent,
};

const ActorInit Demo_Im_InitVars = {
    ACTOR_DEMO_IM,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_IM,
    sizeof(DemoIm),
    (ActorFunc)DemoIm_Init,
    (ActorFunc)DemoIm_Destroy,
    (ActorFunc)DemoIm_Update,
    (ActorFunc)DemoIm_Draw,
    NULL,
};

void func_80984BE0(DemoIm* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeIndex = &this->eyeIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }

    *eyeIndex = *blinkTimer;
    if (*eyeIndex >= 3) {
        *eyeIndex = 0;
    }
}

void func_80984C68(DemoIm* this) {
    this->action = 7;
    this->drawConfig = 0;
    this->alpha = 0;
    this->unk_270 = 0;
    this->actor.shape.shadowAlpha = 0;
    this->unk_268 = 0.0f;
}

void func_80984C8C(DemoIm* this, GlobalContext* globalCtx) {
    u32* something = &D_8098783C;

    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        if (*something != 0) {
            if (this->actor.params == 2) {
                func_80984C68(this);
            }
            *something = 0;
        }
    } else {
        if (*something == 0) {
            *something = 1;
        }
    }
}

void DemoIm_InitCollider(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm* this = (DemoIm*)thisx;

    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinderType1(globalCtx, &this->collider, &this->actor, &sCylinderInit);
}

void DemoIm_DestroyCollider(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm* this = (DemoIm*)thisx;

    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void DemoIm_UpdateCollider(DemoIm* this, GlobalContext* globalCtx) {
    s32 pad[5];

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
}

void func_80984DB8(DemoIm* this) {
    s32 pad[2];
    Vec3s* vec1 = &this->unk_2D4.unk_08;
    Vec3s* vec2 = &this->unk_2D4.unk_0E;

    Math_SmoothStepToS(&vec1->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&vec1->y, 0, 20, 6200, 100);

    Math_SmoothStepToS(&vec2->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&vec2->y, 0, 20, 6200, 100);
}

void func_80984E58(DemoIm* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 yawDiff;
    s16 phi_a3;

    this->unk_2D4.unk_18 = player->actor.world.pos;
    this->unk_2D4.unk_14 = kREG(16) + 4.0f;

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    phi_a3 = (ABS(yawDiff) < 0x18E3) ? 2 : 1;
    func_80034A14(&this->actor, &this->unk_2D4, kREG(17) + 0xC, phi_a3);
}

void func_80984F10(DemoIm* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    this->unk_2D4.unk_18 = player->actor.world.pos;
    this->unk_2D4.unk_14 = kREG(16) + 12.0f;

    func_80034A14(&this->actor, &this->unk_2D4, kREG(17) + 0xC, 2);
}

void func_80984F94(DemoIm* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    this->unk_2D4.unk_18 = player->actor.world.pos;
    this->unk_2D4.unk_14 = kREG(16) + 4.0f;
    func_80034A14(&this->actor, &this->unk_2D4, kREG(17) + 0xC, 4);
}

void DemoIm_UpdateBgCheckInfo(DemoIm* this, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 75.0f, 30.0f, 30.0f, 5);
}

s32 DemoIm_UpdateSkelAnime(DemoIm* this) {
    return SkelAnime_Update(&this->skelAnime);
}

s32 DemoIm_IsCsStateIdle(GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        return true;
    } else {
        return false;
    }
}

CsCmdActorAction* DemoIm_GetNpcAction(GlobalContext* globalCtx, s32 actionIdx) {
    s32 pad[2];
    CsCmdActorAction* ret = NULL;

    if (!DemoIm_IsCsStateIdle(globalCtx)) {
        ret = globalCtx->csCtx.npcActions[actionIdx];
    }
    return ret;
}

s32 func_809850E8(DemoIm* this, GlobalContext* globalCtx, u16 action, s32 actionIdx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, actionIdx);

    if (npcAction != NULL) {
        if (npcAction->action == action) {
            return true;
        }
    }
    return false;
}

s32 func_80985134(DemoIm* this, GlobalContext* globalCtx, u16 action, s32 actionIdx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, actionIdx);

    if (npcAction != NULL) {
        if (npcAction->action != action) {
            return true;
        }
    }
    return false;
}

void func_80985180(DemoIm* this, GlobalContext* globalCtx, s32 actionIdx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, actionIdx);

    if (npcAction != NULL) {
        this->actor.world.pos.x = npcAction->startPos.x;
        this->actor.world.pos.y = npcAction->startPos.y;
        this->actor.world.pos.z = npcAction->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = npcAction->rot.y;
    }
}

void func_80985200(DemoIm* this, GlobalContext* globalCtx, s32 actionIdx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, actionIdx);

    if (npcAction != NULL) {
        this->actor.world.pos.x = npcAction->startPos.x;
        this->actor.world.pos.y = npcAction->startPos.y;
        this->actor.world.pos.z = npcAction->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = npcAction->rot.y;
    }
}

void DemoIm_ChangeAnim(DemoIm* this, AnimationHeader* animHeaderSeg, u8 animMode, f32 transitionRate,
                       s32 playBackwards) {
    f32 frameCount = Animation_GetLastFrame(animHeaderSeg);
    f32 playbackSpeed;
    f32 startFrame;
    f32 endFrame;

    if (!playBackwards) {
        startFrame = 0.0f;
        endFrame = frameCount;
        playbackSpeed = 1.0f;
    } else {
        endFrame = 0.0f;
        startFrame = frameCount;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&this->skelAnime, animHeaderSeg, playbackSpeed, startFrame, endFrame, animMode, transitionRate);
}

void func_80985310(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->actor.shape.yOffset = -10000.0f;
}

void func_80985358(DemoIm* this, GlobalContext* globalCtx) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0,
                       WARP_SAGES);
}

void func_809853B4(DemoIm* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 playerX = player->actor.world.pos.x;
    f32 playerY = player->actor.world.pos.y + 80.0f;
    f32 playerZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_DEMO_EFFECT, playerX, playerY, playerZ, 0,
                       0, 0, 0xD);
    Item_Give(globalCtx, ITEM_MEDALLION_SHADOW);
}

void func_80985430(DemoIm* this, GlobalContext* globalCtx) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

void func_8098544C(DemoIm* this, GlobalContext* globalCtx) {
    s32 pad[2];

    if ((gSaveContext.chamberCutsceneNum == 4) && (gSaveContext.sceneSetupIndex < 4)) {
        Player* player = GET_PLAYER(globalCtx);

        this->action = 1;
        globalCtx->csCtx.segment = D_8098786C;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(globalCtx, ITEM_MEDALLION_SHADOW);
        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void func_809854DC(DemoIm* this, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[5] != NULL) &&
        (globalCtx->csCtx.npcActions[5]->action == 2)) {
        Animation_Change(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->action = 2;
        this->drawConfig = 1;
        func_80985358(this, globalCtx);
    }
}

void func_8098557C(DemoIm* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = 3;
        this->actor.shape.yOffset = 0.0f;
    }
}

void func_809855A8(DemoIm* this, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[5] != NULL) &&
        (globalCtx->csCtx.npcActions[5]->action == 3)) {
        Animation_Change(&this->skelAnime, &gImpaRaiseArmsAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaRaiseArmsAnim),
                         ANIMMODE_ONCE, 4.0f);
        this->action = 4;
    }
}

void func_80985640(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&this->skelAnime, &gImpaPresentShadowMedallionAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gImpaPresentShadowMedallionAnim), ANIMMODE_LOOP, 0.0f);
        this->action = 5;
    }
}

void func_809856AC(DemoIm* this, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[6] != NULL) &&
        (globalCtx->csCtx.npcActions[6]->action == 2)) {
        this->action = 6;
        func_809853B4(this, globalCtx);
    }
}

void func_809856F8(DemoIm* this, GlobalContext* globalCtx) {
    func_8098544C(this, globalCtx);
}

void func_80985718(DemoIm* this, GlobalContext* globalCtx) {
    func_809854DC(this, globalCtx);
}

void func_80985738(DemoIm* this, GlobalContext* globalCtx) {
    func_80985430(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_8098557C(this);
}

void func_80985770(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809855A8(this, globalCtx);
}

void func_809857B0(DemoIm* this, GlobalContext* globalCtx) {
    s32 sp1C;

    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    sp1C = DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80985640(this, sp1C);
}

void func_809857F0(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809856AC(this, globalCtx);
}

void func_80985830(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
}

void func_80985860(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 7;
    this->actor.shape.shadowAlpha = 0;
}

void func_809858A8(void) {
    func_800788CC(NA_SE_SY_WHITE_OUT_T);
}

void DemoIm_SpawnLightBall(DemoIm* this, GlobalContext* globalCtx) {
    Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       (kREG(17) + 24.0f) + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 6);
}

void func_80985948(DemoIm* this, GlobalContext* globalCtx) {
    if (func_809850E8(this, globalCtx, 4, 5)) {
        Animation_Change(&this->skelAnime, &gImpaPrepareSealGanonAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gImpaPrepareSealGanonAnim), ANIMMODE_ONCE, 0.0f);
        this->action = 8;
        this->drawConfig = 2;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_268 = 0.0f;
        func_809858A8();
    }
}

void func_809859E0(DemoIm* this, GlobalContext* globalCtx) {
    f32* unk_268 = &this->unk_268;
    s32 alpha = 255;

    if (func_809850E8(this, globalCtx, 4, 5)) {
        *unk_268 += 1.0f;
        if (*unk_268 >= kREG(5) + 10.0f) {
            this->action = 9;
            this->drawConfig = 1;
            *unk_268 = kREG(5) + 10.0f;
            this->alpha = this->actor.shape.shadowAlpha = alpha;
            return;
        }
    } else {
        *unk_268 -= 1.0f;
        if (*unk_268 <= 0.0f) {
            this->action = 7;
            this->drawConfig = 0;
            *unk_268 = 0.0f;
            this->alpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    this->actor.shape.shadowAlpha = this->alpha = (*unk_268 / (kREG(5) + 10.0f)) * 255.0f;
}

void func_80985B34(DemoIm* this, GlobalContext* globalCtx) {
    if (func_80985134(this, globalCtx, 4, 5)) {
        Animation_Change(&this->skelAnime, &gImpaSealGanonAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaSealGanonAnim),
                         ANIMMODE_ONCE, -8.0f);
        this->action = 8;
        this->drawConfig = 2;
        this->unk_268 = kREG(5) + 10.0f;
        this->alpha = 255;
        if (this->unk_270 == 0) {
            DemoIm_SpawnLightBall(this, globalCtx);
            this->unk_270 = 1;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80985C10(DemoIm* this, GlobalContext* globalCtx) {
    func_80985948(this, globalCtx);
    func_80984C8C(this, globalCtx);
}

void func_80985C40(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809859E0(this, globalCtx);
    func_80984C8C(this, globalCtx);
}

void func_80985C94(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80985B34(this, globalCtx);
    func_80984C8C(this, globalCtx);
}

void DemoIm_DrawTranslucent(DemoIm* this, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTex = sEyeTextures[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       NULL, NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void func_80985E60(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 10;
    this->unk_280 = 1;
}

void func_80985EAC(DemoIm* this, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.frames >= 80) && (globalCtx->csCtx.frames < 243)) {
        func_80984F10(this, globalCtx);
    } else {
        func_80984DB8(this);
    }
}

void func_80985EF4(DemoIm* this) {
    if (!Animation_OnFrame(&this->skelAnime, Animation_GetLastFrame(&gImpaWhistlingAnim) - 1.0f)) {
        DemoIm_UpdateSkelAnime(this);
    }
}

void func_80985F54(DemoIm* this) {
    this->action = 10;
    this->drawConfig = 0;
}

void func_80985F64(DemoIm* this, GlobalContext* globalCtx) {
    Animation_Change(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                     ANIMMODE_LOOP, 0.0f);
    func_80985180(this, globalCtx, 5);
    this->action = 11;
    this->drawConfig = 1;
}

void func_80985FE8(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&this->skelAnime, &gImpaWhistlingAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaWhistlingAnim),
                         ANIMMODE_LOOP, -8.0f);
    }
}

void func_8098604C(DemoIm* this) {
    f32 frameCount = Animation_GetLastFrame(&gImpaStartWhistlingAnim);

    Animation_Change(&this->skelAnime, &gImpaStartWhistlingAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -8.0f);
    this->action = 12;
    this->drawConfig = 1;
    this->unk_2D0 = 1;
}

void func_809860C8(DemoIm* this) {
    this->action = 13;
    this->drawConfig = 1;
}

void func_809860DC(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                         ANIMMODE_LOOP, -8.0f);
        this->unk_2D0 = 0;
    }
}

void func_80986148(DemoIm* this) {
    Animation_Change(&this->skelAnime, &gImpaStartWhistlingAnim, -1.0f,
                     Animation_GetLastFrame(&gImpaStartWhistlingAnim), 0.0f, ANIMMODE_ONCE, -8.0f);
    this->action = 14;
    this->drawConfig = 1;
}

void func_809861C4(DemoIm* this, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, 5);

    if (npcAction != NULL) {
        u32 action = npcAction->action;
        u32 unk_274 = this->unk_274;

        if (action != unk_274) {
            switch (action) {
                case 9:
                    func_80986148(this);
                    break;
                case 7:
                    Animation_Change(&this->skelAnime, &gImpaWhistlingAnim, 1.0f, 0.0f,
                                     Animation_GetLastFrame(&gImpaWhistlingAnim), ANIMMODE_LOOP, -8.0f);
                    this->action = 12;
                    break;
                default:
                    osSyncPrintf("Demo_Im_Ocarina_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->unk_274 = action;
        }
    }
}

void func_8098629C(DemoIm* this, GlobalContext* globalCtx) {
    if (DemoIm_IsCsStateIdle(globalCtx)) {
        this->action = 21;
        this->drawConfig = 1;
        this->unk_280 = 1;
    }
}

void func_809862E0(DemoIm* this, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, 5);

    if (npcAction != NULL) {
        u32 action = npcAction->action;
        u32 unk_274 = this->unk_274;

        if (action != unk_274) {
            switch (action) {
                case 1:
                    func_80985F54(this);
                    break;
                case 2:
                    func_80985F64(this, globalCtx);
                    break;
                case 7:
                    func_8098604C(this);
                    break;
                case 8:
                    func_809860C8(this);
                    break;
                case 9:
                    func_80986148(this);
                    break;
                default:
                    osSyncPrintf("Demo_Im_Ocarina_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->unk_274 = action;
        }
    }
}

void func_809863BC(DemoIm* this, GlobalContext* globalCtx) {
    func_809862E0(this, globalCtx);
}

void func_809863DC(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80985EAC(this, globalCtx);
    func_80984BE0(this);
    func_809862E0(this, globalCtx);
}

void func_80986430(DemoIm* this, GlobalContext* globalCtx) {
    s32 sp24;

    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    sp24 = DemoIm_UpdateSkelAnime(this);
    func_80985EAC(this, globalCtx);
    func_80984BE0(this);
    func_80985FE8(this, sp24);
    func_809862E0(this, globalCtx);
}

void func_80986494(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    func_80985EF4(this);
    func_80984BE0(this);
    func_809861C4(this, globalCtx);
}

void func_809864D4(DemoIm* this, GlobalContext* globalCtx) {
    s32 sp24;

    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    sp24 = DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809860DC(this, sp24);
    func_8098629C(this, globalCtx);
}

void func_8098652C(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 15;
}

void func_80986570(DemoIm* this, GlobalContext* globalCtx) {
    if (Animation_OnFrame(&this->skelAnime, 7.0f) && (this->actor.bgCheckFlags & 1)) {
        u32 sfxId = SFX_FLAG;

        sfxId += SurfaceType_GetSfx(&globalCtx->colCtx, this->actor.floorPoly, this->actor.floorBgId);
        Audio_PlaySoundGeneral(sfxId, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void func_809865F8(DemoIm* this, GlobalContext* globalCtx, s32 arg2) {
    s32 pad[2];

    if (arg2 != 0) {
        f32* unk_278 = &this->unk_278;

        if (*unk_278 >= 0.0f) {
            if (this->unk_27C == 0) {
                Vec3f* thisPos = &this->actor.world.pos;
                s16 shapeRotY = this->actor.shape.rot.y;
                f32 spawnPosX = thisPos->x + (Math_SinS(shapeRotY) * 30.0f);
                f32 spawnPosY = thisPos->y;
                f32 spawnPosZ = thisPos->z + (Math_CosS(shapeRotY) * 30.0f);

                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ARROW, spawnPosX, spawnPosY, spawnPosZ, 0xFA0,
                            this->actor.shape.rot.y, 0, ARROW_CS_NUT);
                this->unk_27C = 1;
            }
        } else {
            *unk_278 += 1.0f;
        }
    }
}

void func_80986700(DemoIm* this) {
    this->action = 15;
    this->drawConfig = 0;
}

void func_80986710(DemoIm* this, GlobalContext* globalCtx) {
    Animation_Change(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                     ANIMMODE_LOOP, 0.0f);
    func_80985180(this, globalCtx, 5);
    this->action = 16;
    this->drawConfig = 1;
}

void func_80986794(DemoIm* this) {
    Animation_Change(&this->skelAnime, &gImpaThrowDekuNutAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gImpaThrowDekuNutAnim), ANIMMODE_ONCE, -8.0f);
    this->action = 17;
    this->drawConfig = 1;
}

void func_8098680C(DemoIm* this, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, 5);

    if (npcAction != NULL) {
        u32 action = npcAction->action;
        u32 unk_274 = this->unk_274;

        if (action != unk_274) {
            switch (action) {
                case 1:
                    func_80986700(this);
                    break;
                case 2:
                    func_80986710(this, globalCtx);
                    break;
                case 10:
                    func_80986794(this);
                    break;
                case 11:
                    Actor_Kill(&this->actor);
                    break;
                default:
                    osSyncPrintf("Demo_Im_Spot00_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->unk_274 = action;
        }
    }
}

void func_809868E8(DemoIm* this, GlobalContext* globalCtx) {
    func_8098680C(this, globalCtx);
}

void func_80986908(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_8098680C(this, globalCtx);
}

void func_80986948(DemoIm* this, GlobalContext* globalCtx) {
    s32 sp24;

    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    sp24 = DemoIm_UpdateSkelAnime(this);
    func_80986570(this, globalCtx);
    func_80984BE0(this);
    func_809865F8(this, globalCtx, sp24);
    func_8098680C(this, globalCtx);
}

void func_809869B0(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 18;
    this->actor.shape.shadowAlpha = 0;
}

s32 func_809869F8(DemoIm* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 playerPosX = player->actor.world.pos.x;
    f32 thisPosX = this->actor.world.pos.x;

    if ((thisPosX - (kREG(16) + 30.0f) > playerPosX) && !(this->actor.flags & ACTOR_FLAG_6)) {
        return true;
    } else {
        return false;
    }
}

s32 func_80986A5C(DemoIm* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 playerPosX = player->actor.world.pos.x;
    f32 thisPosX = this->actor.world.pos.x;

    if ((thisPosX - (kREG(17) + 130.0f) < playerPosX) && (!Gameplay_InCsMode(globalCtx))) {
        return true;
    } else {
        return false;
    }
}

s32 func_80986AD0(DemoIm* this, GlobalContext* globalCtx) {
    this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
    if (!Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        this->actor.textId = 0x708E;
        func_8002F2F4(&this->actor, globalCtx);
    } else {
        return true;
    }
    return false;
}

void func_80986B2C(GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        Player* player = GET_PLAYER(globalCtx);

        globalCtx->nextEntranceIndex = 0xCD;
        globalCtx->fadeTransition = 38;
        globalCtx->sceneLoadFlag = 0x14;
        func_8002DF54(globalCtx, &player->actor, 8);
    }
}

void func_80986BA0(DemoIm* this, GlobalContext* globalCtx) {
    if (func_809869F8(this, globalCtx)) {
        this->action = 21;
        this->drawConfig = 1;
        this->unk_280 = 1;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80986BE4(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        this->action = 22;
    }
}

void func_80986BF8(DemoIm* this, GlobalContext* globalCtx) {
    if (gSaveContext.eventChkInf[4] & 1) {
        this->action = 24;
        this->drawConfig = 1;
        this->unk_280 = 1;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void GivePlayerRandoRewardImpa(Actor* impa, GlobalContext* globalCtx, RandomizerCheck check) {
    GetItemID getItemId = GetRandomizedItemIdFromKnownCheck(check, GI_ZELDAS_LULLABY);

    if (impa->parent != NULL && impa->parent->id == GET_PLAYER(globalCtx)->actor.id &&
        !Flags_GetTreasure(globalCtx, 0x1F)) {
        Flags_SetTreasure(globalCtx, 0x1F);
    } else if (!Flags_GetTreasure(globalCtx, 0x1F) && !GetRandoSettingValue(RSK_SKIP_CHILD_ZELDA)) {
        func_8002F434(impa, globalCtx, getItemId, 75.0f, 50.0f);
    } else if (!Player_InBlockingCsMode(globalCtx, GET_PLAYER(globalCtx))) {
        gSaveContext.eventChkInf[5] |= 0x200;
        globalCtx->sceneLoadFlag = 0x14;
        globalCtx->fadeTransition = 3;
        gSaveContext.nextTransition = 3;
        globalCtx->nextEntranceIndex = 0x0594;
        gSaveContext.nextCutsceneIndex = 0;
    }
}

void func_80986C30(DemoIm* this, GlobalContext* globalCtx) {
    if (func_80986A5C(this, globalCtx)) {
        if (gSaveContext.n64ddFlag) {
            GivePlayerRandoRewardImpa(this, globalCtx, RC_SONG_FROM_IMPA);
        } else {
            globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gZeldasCourtyardLullabyCs);
            gSaveContext.cutsceneTrigger = 1;
            gSaveContext.eventChkInf[5] |= 0x200;
            Item_Give(globalCtx, ITEM_SONG_LULLABY);
            func_80985F54(this);
        }
    }
}

void func_80986CC8(DemoIm* this) {
    if (gSaveContext.eventChkInf[4] & 1) {
        this->action = 26;
        this->drawConfig = 1;
        this->unk_280 = 1;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80986CFC(DemoIm* this, GlobalContext* globalCtx) {
    if (func_80986A5C(this, globalCtx)) {
        gSaveContext.eventChkInf[4] |= 0x1000;
        this->action = 19;
    }
}

void func_80986D40(DemoIm* this, GlobalContext* globalCtx) {
    if (gSaveContext.sceneSetupIndex == 6) {
        this->action = 19;
        this->drawConfig = 1;
    } else if ((gSaveContext.eventChkInf[8] & 1) && !gSaveContext.n64ddFlag) {
        Actor_Kill(&this->actor);
    } else if (!(gSaveContext.eventChkInf[5] & 0x200)) {
        this->action = 23;
    } else {
        this->action = 20;
    }
}

void func_80986DC8(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80984E58(this, globalCtx);
    this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
}

void func_80986E20(DemoIm* this, GlobalContext* globalCtx) {
    func_80986BA0(this, globalCtx);
}

void func_80986E40(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80984E58(this, globalCtx);
    DemoIm_UpdateCollider(this, globalCtx);
    func_80986BE4(this, func_80986AD0(this, globalCtx));
}

void func_80986EAC(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80984F94(this, globalCtx);
    DemoIm_UpdateCollider(this, globalCtx);
    func_80986B2C(globalCtx);
}

void func_80986F08(DemoIm* this, GlobalContext* globalCtx) {
    func_80986BF8(this, globalCtx);
}

void func_80986F28(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80984E58(this, globalCtx);
    DemoIm_UpdateCollider(this, globalCtx);
    func_80986C30(this, globalCtx);
}

void func_80986F88(DemoIm* this, GlobalContext* globalCtx) {
    func_80986CC8(this);
}

void func_80986FA8(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80984E58(this, globalCtx);
    this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    DemoIm_UpdateCollider(this, globalCtx);
    func_80986CFC(this, globalCtx);
}

void func_80987018(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 27;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80987064(DemoIm* this) {
    f32* unk_268 = &this->unk_268;
    f32 temp;
    s32 alpha = 255;

    *unk_268 += 1.0f;
    temp = kREG(17) + 10.0f;

    if (*unk_268 >= temp) {
        this->actor.shape.shadowAlpha = this->alpha = alpha;
    } else {
        this->actor.shape.shadowAlpha = this->alpha = (*unk_268 / temp) * 255.0f;
    }
}

void func_809870F0(DemoIm* this, GlobalContext* globalCtx) {
    func_80985200(this, globalCtx, 5);
    this->action = 28;
    this->drawConfig = 2;
}

void func_80987128(DemoIm* this) {
    if (this->unk_268 >= kREG(17) + 10.0f) {
        this->action = 29;
        this->drawConfig = 1;
    }
}

void func_80987174(DemoIm* this) {
    DemoIm_ChangeAnim(this, &object_im_Anim_0101C8, ANIMMODE_ONCE, -8.0f, false);
    this->action = 30;
}

void func_809871B4(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        DemoIm_ChangeAnim(this, &object_im_Anim_00FB10, ANIMMODE_LOOP, 0.0f, false);
    }
}

void func_809871E8(DemoIm* this, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, 5);

    if (npcAction != NULL) {
        u32 action = npcAction->action;
        u32 unk_274 = this->unk_274;

        if (action != unk_274) {
            switch (action) {
                case 12:
                    func_809870F0(this, globalCtx);
                    break;
                case 13:
                    func_80987174(this);
                    break;
                default:
                    osSyncPrintf("Demo_Im_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->unk_274 = action;
        }
    }
}

void func_80987288(DemoIm* this, GlobalContext* globalCtx) {
    func_809871E8(this, globalCtx);
}

void func_809872A8(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80987064(this);
    func_80987128(this);
}

void func_809872F0(DemoIm* this, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809871E8(this, globalCtx);
}

void func_80987330(DemoIm* this, GlobalContext* globalCtx) {
    s32 sp1C;

    DemoIm_UpdateBgCheckInfo(this, globalCtx);
    sp1C = DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809871B4(this, sp1C);
}

void DemoIm_Update(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm* this = (DemoIm*)thisx;

    if ((this->action < 0) || (this->action >= 31) || (sActionFuncs[this->action] == NULL)) {
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, globalCtx);
}

void DemoIm_Init(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm* this = (DemoIm*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    DemoIm_InitCollider(thisx, globalCtx);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gImpaSkel, NULL, this->jointTable, this->morphTable, 17);
    thisx->flags &= ~ACTOR_FLAG_0;

    switch (this->actor.params) {
        case 2:
            func_80985860(this, globalCtx);
            break;
        case 3:
            func_80985E60(this, globalCtx);
            break;
        case 4:
            func_8098652C(this, globalCtx);
            break;
        case 5:
            func_809869B0(this, globalCtx);
            break;
        case 6:
            func_80987018(this, globalCtx);
            break;
        default:
            func_80985310(this, globalCtx);
    }
}

void DemoIm_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm_DestroyCollider(thisx, globalCtx);
}

s32 DemoIm_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    DemoIm* this = (DemoIm*)thisx;
    s32* unk_2D0 = &this->unk_2D0;

    if (this->unk_280 != 0) {
        Vec3s* unk_2D4_unk_0E = &this->unk_2D4.unk_0E;
        Vec3s* unk_2D4_unk_08 = &this->unk_2D4.unk_08;

        switch (limbIndex) {
            case IMPA_LIMB_CHEST:
                rot->x += unk_2D4_unk_0E->y;
                rot->y -= unk_2D4_unk_0E->x;
                break;
            case IMPA_LIMB_HEAD:
                rot->x += unk_2D4_unk_08->y;
                rot->z += unk_2D4_unk_08->x;
                break;
        }
    }

    if ((*unk_2D0 != 0) && (limbIndex == IMPA_LIMB_RIGHT_HAND)) {
        *dList = gImpaHandPointingDL;
    }

    return false;
}

void DemoIm_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    DemoIm* this = (DemoIm*)thisx;

    if (limbIndex == IMPA_LIMB_HEAD) {
        Vec3f sp28 = D_809887D8;
        Vec3f dest;

        Matrix_MultVec3f(&sp28, &dest);
        this->actor.focus.pos.x = dest.x;
        this->actor.focus.pos.y = dest.y;
        this->actor.focus.pos.z = dest.z;
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
}

void DemoIm_DrawNothing(DemoIm* this, GlobalContext* globalCtx) {
}

void DemoIm_DrawSolid(DemoIm* this, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTexture = sEyeTextures[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          DemoIm_OverrideLimbDraw, DemoIm_PostLimbDraw, this);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void DemoIm_Draw(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm* this = (DemoIm*)thisx;

    if ((this->drawConfig < 0) || (this->drawConfig >= 3) || (sDrawFuncs[this->drawConfig] == NULL)) {
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, globalCtx);
}
