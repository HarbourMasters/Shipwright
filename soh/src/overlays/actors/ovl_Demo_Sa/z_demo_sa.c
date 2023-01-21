/*
 * File: z_demo_sa.c
 * Overlay: Demo_Sa
 * Description: Saria (Cutscene)
 */

#include "z_demo_sa.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "objects/object_sa/object_sa.h"

#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void DemoSa_Init(Actor* thisx, PlayState* play);
void DemoSa_Destroy(Actor* thisx, PlayState* play);
void DemoSa_Update(Actor* thisx, PlayState* play);
void DemoSa_Draw(Actor* thisx, PlayState* play);

void func_8098EBB8(DemoSa* this, PlayState* play);
void func_8098EBD8(DemoSa* this, PlayState* play);
void func_8098EBF8(DemoSa* this, PlayState* play);
void func_8098EC28(DemoSa* this, PlayState* play);
void func_8098EC60(DemoSa* this, PlayState* play);
void func_8098EC94(DemoSa* this, PlayState* play);
void func_8098ECCC(DemoSa* this, PlayState* play);
void func_8098F0E8(DemoSa* this, PlayState* play);
void func_8098F118(DemoSa* this, PlayState* play);
void func_8098F16C(DemoSa* this, PlayState* play);
void func_8098F3F0(DemoSa* this, PlayState* play);
void func_8098F714(DemoSa* this, PlayState* play);
void func_8098F734(DemoSa* this, PlayState* play);
void func_8098F77C(DemoSa* this, PlayState* play);
void func_8098F7BC(DemoSa* this, PlayState* play);
void func_8098F7FC(DemoSa* this, PlayState* play);
void func_8098FC44(DemoSa* this, PlayState* play);
void func_8098FC64(DemoSa* this, PlayState* play);
void func_8098FC9C(DemoSa* this, PlayState* play);
void func_8098FCD4(DemoSa* this, PlayState* play);
void func_8098FD0C(DemoSa* this, PlayState* play);

void DemoSa_DrawNothing(DemoSa* this, PlayState* play);
void DemoSa_DrawOpa(DemoSa* this, PlayState* play);
void DemoSa_DrawXlu(DemoSa* this, PlayState* play);

typedef enum {
    /* 0 */ SARIA_EYE_OPEN,
    /* 1 */ SARIA_EYE_HALF,
    /* 2 */ SARIA_EYE_CLOSED,
    /* 3 */ SARIA_EYE_SUPRISED,
    /* 4 */ SARIA_EYE_SAD
} SariaEyeState;

typedef enum {
    /* 0 */ SARIA_MOUTH_CLOSED2,
    /* 1 */ SARIA_MOUTH_SUPRISED,
    /* 2 */ SARIA_MOUTH_CLOSED,
    /* 3 */ SARIA_MOUTH_SMILING_OPEN,
    /* 4 */ SARIA_MOUTH_FROWNING
} SariaMouthState;

static void* sEyeTextures[] = {
    gSariaEyeOpenTex, gSariaEyeHalfTex, gSariaEyeClosedTex, gSariaEyeSuprisedTex, gSariaEyeSadTex,
};

static void* sMouthTextures[] = {
    gSariaMouthClosed2Tex,     gSariaMouthSuprisedTex, gSariaMouthClosedTex,
    gSariaMouthSmilingOpenTex, gSariaMouthFrowningTex,
};

static u32 D_80990108 = 0;

#include "z_demo_sa_cutscene_data.c" EARLY

static DemoSaActionFunc sActionFuncs[] = {
    func_8098EBB8, func_8098EBD8, func_8098EBF8, func_8098EC28, func_8098EC60, func_8098EC94, func_8098ECCC,
    func_8098F0E8, func_8098F118, func_8098F16C, func_8098F3F0, func_8098F714, func_8098F734, func_8098F77C,
    func_8098F7BC, func_8098F7FC, func_8098FC44, func_8098FC64, func_8098FC9C, func_8098FCD4, func_8098FD0C,
};

static DemoSaDrawFunc sDrawFuncs[] = {
    DemoSa_DrawNothing,
    DemoSa_DrawOpa,
    DemoSa_DrawXlu,
};

const ActorInit Demo_Sa_InitVars = {
    ACTOR_DEMO_SA,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SA,
    sizeof(DemoSa),
    (ActorFunc)DemoSa_Init,
    (ActorFunc)DemoSa_Destroy,
    (ActorFunc)DemoSa_Update,
    (ActorFunc)DemoSa_Draw,
    NULL,
};

void DemoSa_Destroy(Actor* thisx, PlayState* play) {
    DemoSa* this = (DemoSa*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
}

void func_8098E480(DemoSa* this) {
    s32 pad[2];
    s16* eyeIndex = &this->eyeIndex;
    s16* blinkTimer = &this->blinkTimer;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(0x3C, 0x3C);
    }

    *eyeIndex = *blinkTimer;
    if (*eyeIndex >= 3) {
        *eyeIndex = 0;
    }
}

void DemoSa_SetEyeIndex(DemoSa* this, s16 eyeIndex) {
    this->eyeIndex = eyeIndex;
}

void DemoSa_SetMouthIndex(DemoSa* this, s16 mouthIndex) {
    this->mouthIndex = mouthIndex;
}

void func_8098E530(DemoSa* this) {
    this->action = 7;
    this->drawConfig = 0;
    this->alpha = 0;
    this->unk_1A8 = 0;
    this->actor.shape.shadowAlpha = 0;
    this->unk_1A0 = 0.0f;
}

void func_8098E554(DemoSa* this, PlayState* play) {
    u32* something = &D_80990108;

    if (play->csCtx.state == CS_STATE_IDLE) {
        if (*something != 0) {
            if (this->actor.params == 2) {
                func_8098E530(this);
            }
            *something = 0;
        }
    } else if (*something == 0) {
        *something = 1;
    }
}

void func_8098E5C8(DemoSa* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, 5);
}

s32 DemoSa_UpdateSkelAnime(DemoSa* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorAction* DemoSa_GetNpcAction(PlayState* play, s32 idx) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        return play->csCtx.npcActions[idx];
    }
    return NULL;
}

s32 func_8098E654(DemoSa* this, PlayState* play, u16 arg2, s32 arg3) {
    CsCmdActorAction* npcAction = DemoSa_GetNpcAction(play, arg3);

    if ((npcAction != NULL) && (npcAction->action == arg2)) {
        return 1;
    }
    return 0;
}

s32 func_8098E6A0(DemoSa* this, PlayState* play, u16 arg2, s32 arg3) {
    CsCmdActorAction* npcAction = DemoSa_GetNpcAction(play, arg3);

    if ((npcAction != NULL) && (npcAction->action != arg2)) {
        return 1;
    }
    return 0;
}

void func_8098E6EC(DemoSa* this, PlayState* play, s32 actionIdx) {
    CsCmdActorAction* npcAction = DemoSa_GetNpcAction(play, actionIdx);

    if (npcAction != NULL) {
        this->actor.world.pos.x = npcAction->startPos.x;
        this->actor.world.pos.y = npcAction->startPos.y;
        this->actor.world.pos.z = npcAction->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = npcAction->rot.y;
    }
}

void func_8098E76C(DemoSa* this, AnimationHeader* animHeaderSeg, u8 arg2, f32 transitionRate, s32 arg4) {
    s32 pad[2];
    f32 frameCount = Animation_GetLastFrame(animHeaderSeg);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (arg4 == 0) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        fc = 0.0f;
        unk0 = frameCount;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&this->skelAnime, animHeaderSeg, playbackSpeed, unk0, fc, arg2, transitionRate);
}

void func_8098E7FC(DemoSa* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gSariaSkel, &gSariaWaitArmsToSideAnim, NULL, NULL, 0);
    this->actor.shape.yOffset = -10000.0f;
    DemoSa_SetEyeIndex(this, SARIA_EYE_HALF);
    DemoSa_SetMouthIndex(this, SARIA_MOUTH_CLOSED2);
}

void func_8098E86C(DemoSa* this, PlayState* play) {
    Vec3f* world = &this->actor.world.pos;
    f32 posX = world->x;
    f32 posY = world->y;
    f32 posZ = world->z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0,
                       WARP_SAGES);
}

void func_8098E8C8(DemoSa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 80.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0,
                       0xB);
    Item_Give(play, ITEM_MEDALLION_FOREST);
}

void func_8098E944(DemoSa* this, PlayState* play) {
    this->actor.shape.yOffset += (250.0f / 3.0f);
}

void func_8098E960(DemoSa* this, PlayState* play) {
    s32 pad[2];
    Player* player;

    if ((gSaveContext.chamberCutsceneNum == 0) && (gSaveContext.sceneSetupIndex < 4)) {
        player = GET_PLAYER(play);
        this->action = 1;
        play->csCtx.segment = D_8099010C;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(play, ITEM_MEDALLION_FOREST);
        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void func_8098E9EC(DemoSa* this, PlayState* play) {
    CsCmdActorAction* npcAction;

    if (play->csCtx.state != CS_STATE_IDLE) {
        npcAction = play->csCtx.npcActions[4];
        if ((npcAction != NULL) && (npcAction->action == 2)) {
            this->action = 2;
            this->drawConfig = 1;
            func_8098E86C(this, play);
        }
    }
}

void func_8098EA3C(DemoSa* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = 3;
        this->actor.shape.yOffset = 0.0f;
    }
}

void func_8098EA68(DemoSa* this, PlayState* play) {
    CsCmdActorAction* npcAction;

    if (play->csCtx.state != CS_STATE_IDLE) {
        npcAction = play->csCtx.npcActions[4];
        if ((npcAction != NULL) && (npcAction->action == 3)) {
            Animation_Change(&this->skelAnime, &gSariaGiveForestMedallionAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gSariaGiveForestMedallionAnim), ANIMMODE_ONCE, -4.0f);
            this->action = 4;
        }
    }
}

void func_8098EB00(DemoSa* this, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&this->skelAnime, &gSariaGiveForestMedallionStandAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gSariaGiveForestMedallionStandAnim), ANIMMODE_LOOP, 0.0f);
        this->action = 5;
    }
}

void func_8098EB6C(DemoSa* this, PlayState* play) {
    CsCmdActorAction* npcAction;

    if (play->csCtx.state != CS_STATE_IDLE) {
        npcAction = play->csCtx.npcActions[6];
        if ((npcAction != NULL) && (npcAction->action == 2)) {
            this->action = 6;
            func_8098E8C8(this, play);
        }
    }
}

void func_8098EBB8(DemoSa* this, PlayState* play) {
    func_8098E960(this, play);
}

void func_8098EBD8(DemoSa* this, PlayState* play) {
    func_8098E9EC(this, play);
}

void func_8098EBF8(DemoSa* this, PlayState* play) {
    func_8098E944(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098EA3C(this);
}

void func_8098EC28(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098EA68(this, play);
}

void func_8098EC60(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    func_8098EB00(this, DemoSa_UpdateSkelAnime(this));
}

void func_8098EC94(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098EB6C(this, play);
}

void func_8098ECCC(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
}

void func_8098ECF4(DemoSa* this, PlayState* play) {
    s32 pad[2];
    SkelAnime* skelAnime = &this->skelAnime;
    f32 frameCount = Animation_GetLastFrame(&gSariaSealGanonAnim);

    SkelAnime_InitFlex(play, skelAnime, &gSariaSkel, NULL, NULL, NULL, 0);
    Animation_Change(skelAnime, &gSariaSealGanonAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
    this->action = 7;
    this->actor.shape.shadowAlpha = 0;
    DemoSa_SetEyeIndex(this, SARIA_EYE_CLOSED);
    DemoSa_SetMouthIndex(this, SARIA_MOUTH_CLOSED);
}

void func_8098EDB0(DemoSa* this) {
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->skelAnime.mode == 2) && (curFrame >= 32.0f)) {
        DemoSa_SetEyeIndex(this, SARIA_EYE_HALF);
        DemoSa_SetMouthIndex(this, SARIA_MOUTH_CLOSED2);
    }
}

void func_8098EE08(void) {
    func_800788CC(NA_SE_SY_WHITE_OUT_T);
}

void func_8098EE28(DemoSa* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       (kREG(23) + 25.0f) + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 4);
}

void func_8098EEA8(DemoSa* this, PlayState* play) {
    if (func_8098E654(this, play, 4, 4)) {
        this->action = 8;
        this->drawConfig = 2;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_1A0 = 0.0f;
        func_8098EE08();
    }
}

void func_8098EEFC(DemoSa* this, PlayState* play) {
    s32 alpha = 255;
    f32* unk_1A0 = &this->unk_1A0;

    if (func_8098E654(this, play, 4, 4)) {
        *unk_1A0 += 1.0f;
        if ((kREG(5) + 10.0f) <= *unk_1A0) {
            this->action = 9;
            this->drawConfig = 1;
            *unk_1A0 = kREG(5) + 10.0f;
            this->alpha = alpha;
            this->actor.shape.shadowAlpha = alpha;
            return;
        }
    } else {
        *unk_1A0 -= 1.0f;
        if (*unk_1A0 <= 0.0f) {
            this->action = 7;
            this->drawConfig = 0;
            *unk_1A0 = 0.0f;
            this->alpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    this->actor.shape.shadowAlpha = this->alpha = (*unk_1A0 / (kREG(5) + 10.0f)) * 255.0f;
}

void func_8098F050(DemoSa* this, PlayState* play) {
    if (func_8098E6A0(this, play, 4, 4)) {
        this->action = 8;
        this->drawConfig = 2;
        this->unk_1A0 = kREG(5) + 10.0f;
        this->alpha = 255;
        if (this->unk_1A8 == 0) {
            func_8098EE28(this, play);
            this->unk_1A8 = 1;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_8098F0E8(DemoSa* this, PlayState* play) {
    func_8098EEA8(this, play);
    func_8098E554(this, play);
}

void func_8098F118(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
    func_8098EEFC(this, play);
    func_8098E554(this, play);
}

void func_8098F16C(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098EDB0(this);
    func_8098F050(this, play);
    func_8098E554(this, play);
}

void DemoSa_DrawXlu(DemoSa* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* sp78 = sEyeTextures[eyeIndex];
    s16 mouthIndex = this->mouthIndex;
    s32 pad2;
    void* sp6C = sMouthTextures[mouthIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sp78));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sp78));
    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(sp6C));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       NULL, NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_8098F390(DemoSa* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gSariaSkel, &gSariaWaitArmsToSideAnim, NULL, NULL, 0);
    this->action = 10;
    this->drawConfig = 1;
}

void func_8098F3F0(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
}

void func_8098F420(DemoSa* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gSariaSkel, &gSariaSitting3Anim, NULL, NULL, 0);
    this->action = 11;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_8098F480(DemoSa* this) {
    s32 alpha = 255;
    f32* unk_1A0 = &this->unk_1A0;
    f32 temp_f0;

    *unk_1A0 += 1.0f;
    temp_f0 = kREG(17) + 10.0f;

    if (temp_f0 <= *unk_1A0) {
        this->actor.shape.shadowAlpha = this->alpha = alpha;
    } else {
        this->actor.shape.shadowAlpha = this->alpha = (*unk_1A0 / temp_f0) * 255.0f;
    }
}

void func_8098F50C(DemoSa* this, PlayState* play) {
    func_8098E6EC(this, play, 4);
    this->action = 12;
    this->drawConfig = 2;
}

void func_8098F544(DemoSa* this) {
    if (this->unk_1A0 >= kREG(17) + 10.0f) {
        this->action = 13;
        this->drawConfig = 1;
    }
}

void func_8098F590(DemoSa* this) {
    func_8098E76C(this, &gSariaSitting1Anim, 2, -8.0f, 0);
    this->action = 14;
}

void func_8098F5D0(DemoSa* this) {
    func_8098E76C(this, &gSariaSitting2Anim, 2, 0.0f, 0);
    this->action = 15;
}

void func_8098F610(DemoSa* this, s32 arg1) {
    if (arg1 != 0) {
        func_8098E76C(this, &gSariaSitting3Anim, 0, 0.0f, 0);
        this->action = 13;
    }
}

void func_8098F654(DemoSa* this, PlayState* play) {
    s32 unk_1AC;
    s32 action;
    CsCmdActorAction* npcAction = DemoSa_GetNpcAction(play, 4);

    if (npcAction != NULL) {
        action = npcAction->action;
        unk_1AC = this->unk_1AC;
        if (action != unk_1AC) {
            switch (action) {
                case 7:
                    func_8098F50C(this, play);
                    break;
                case 8:
                    func_8098F590(this);
                    break;
                case 9:
                    func_8098F5D0(this);
                    break;
                default:
                    osSyncPrintf("Demo_Sa_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->unk_1AC = action;
        }
    }
}

void func_8098F714(DemoSa* this, PlayState* play) {
    func_8098F654(this, play);
}

void func_8098F734(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
    func_8098F480(this);
    func_8098F544(this);
}

void func_8098F77C(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
    func_8098F654(this, play);
}

void func_8098F7BC(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
    func_8098F654(this, play);
}

void func_8098F7FC(DemoSa* this, PlayState* play) {
    s32 sp1C;

    func_8098E5C8(this, play);
    sp1C = DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
    func_8098F610(this, sp1C);
}

void func_8098F83C(DemoSa* this, PlayState* play) {
    Vec3f* thisPos = &this->actor.world.pos;

    SkelAnime_InitFlex(play, &this->skelAnime, &gSariaSkel, &gSariaWaitOnBridgeAnim, NULL, NULL, 0);
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_ELF, thisPos->x, thisPos->y, thisPos->z,
                       0, 0, 0, FAIRY_KOKIRI);
    this->action = 16;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
    DemoSa_SetEyeIndex(this, SARIA_EYE_SAD);
    DemoSa_SetMouthIndex(this, SARIA_MOUTH_CLOSED);
}

void func_8098F8F8(DemoSa* this) {
    s32 alpha = 255;
    f32* unk_1A0 = &this->unk_1A0;
    f32 temp_f0;

    *unk_1A0 += 1.0f;
    temp_f0 = kREG(17) + 10.0f;

    if (temp_f0 <= *unk_1A0) {
        this->actor.shape.shadowAlpha = this->alpha = alpha;
    } else {
        this->actor.shape.shadowAlpha = this->alpha = (*unk_1A0 / temp_f0) * 255.0f;
    }
}

void func_8098F984(DemoSa* this) {
    this->action = 16;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_8098F998(DemoSa* this, PlayState* play) {
    if (this->unk_1AC == 4) {
        func_8098E6EC(this, play, 1);
        this->action = 17;
        this->drawConfig = 2;
        this->unk_1B0 = 0;
        this->actor.shape.shadowAlpha = 0;
    } else {
        func_8098E76C(this, &gSariaWaitOnBridgeAnim, 0, 0.0f, 0);
        this->action = 18;
        this->drawConfig = 1;
        this->unk_1B0 = 0;
        this->actor.shape.shadowAlpha = 0xFF;
    }
    DemoSa_SetEyeIndex(this, SARIA_EYE_SAD);
}

void func_8098FA2C(DemoSa* this) {
    if (this->unk_1A0 >= kREG(17) + 10.0f) {
        this->action = 18;
        this->drawConfig = 1;
        this->unk_1B0 = 0;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_8098FA84(DemoSa* this) {
    func_8098E76C(this, &gSariaHoldOcarinaAnim, 0, 0.0f, 0);
    this->action = 19;
    this->drawConfig = 1;
    this->unk_1B0 = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    DemoSa_SetEyeIndex(this, SARIA_EYE_CLOSED);
}

void func_8098FAE0(DemoSa* this) {
    func_8098E76C(this, &gSariaGiveLinkOcarinaAnim, 2, -8.0f, 0);
    this->action = 20;
    this->drawConfig = 1;
    this->unk_1B0 = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_8098FB34(DemoSa* this, s32 arg1) {
    if (arg1 != 0) {
        func_8098E76C(this, &gSariaHoldOutOcarinaAnim, 0, 0, 0);
    }
}

void func_8098FB68(DemoSa* this, PlayState* play) {
    s32 unk_1AC;
    s32 action;
    CsCmdActorAction* npcAction = DemoSa_GetNpcAction(play, 1);

    if (npcAction != NULL) {
        action = npcAction->action;
        unk_1AC = this->unk_1AC;
        if (action != unk_1AC) {
            switch (action) {
                case 4:
                    func_8098F984(this);
                    break;
                case 12:
                    func_8098F998(this, play);
                    break;
                case 13:
                    func_8098FA84(this);
                    break;
                case 14:
                    func_8098FAE0(this);
                    break;
                default:
                    osSyncPrintf("Demo_Sa_inPresent_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->unk_1AC = action;
        }
    }
}

void func_8098FC44(DemoSa* this, PlayState* play) {
    func_8098FB68(this, play);
}

void func_8098FC64(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098F8F8(this);
    func_8098FA2C(this);
}

void func_8098FC9C(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098FB68(this, play);
}

void func_8098FCD4(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098FB68(this, play);
}

void func_8098FD0C(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    func_8098FB34(this, DemoSa_UpdateSkelAnime(this));
    func_8098FB68(this, play);
}

void DemoSa_Update(Actor* thisx, PlayState* play) {
    DemoSa* this = (DemoSa*)thisx;

    if (this->action < 0 || this->action >= 21 || sActionFuncs[this->action] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void DemoSa_Init(Actor* thisx, PlayState* play) {
    DemoSa* this = (DemoSa*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);

    switch (this->actor.params) {
        case 2:
            func_8098ECF4(this, play);
            break;
        case 3:
            func_8098F390(this, play);
            break;
        case 4:
            func_8098F420(this, play);
            break;
        case 5:
            func_8098F83C(this, play);
            break;
        default:
            func_8098E7FC(this, play);
    }
}

s32 DemoSa_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    DemoSa* this = (DemoSa*)thisx;

    if ((limbIndex == 15) && (this->unk_1B0 != 0)) {
        *dList = gSariaRightHandAndOcarinaDL;
    }
    return false;
}

void DemoSa_DrawNothing(DemoSa* this, PlayState* play) {
}

void DemoSa_DrawOpa(DemoSa* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTex = sEyeTextures[eyeIndex];
    s32 pad2;
    s16 mouthIndex = this->mouthIndex;
    void* mouthTex = sMouthTextures[mouthIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          DemoSa_OverrideLimbDraw, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void DemoSa_Draw(Actor* thisx, PlayState* play) {
    DemoSa* this = (DemoSa*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= 3 || sDrawFuncs[this->drawConfig] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, play);
}
