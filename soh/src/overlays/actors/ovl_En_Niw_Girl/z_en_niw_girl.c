/*
 * File: z_en_niw_girl.c
 * Overlay: ovl_En_Niw_Girl
 * Description: Girl that chases a cuckoo around in Hyrule Market and Kakariko Village
 */

#include "z_en_niw_girl.h"
#include "objects/object_gr/object_gr.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnNiwGirl_Init(Actor* thisx, PlayState* play);
void EnNiwGirl_Destroy(Actor* thisx, PlayState* play);
void EnNiwGirl_Update(Actor* thisx, PlayState* play);
void EnNiwGirl_Draw(Actor* thisx, PlayState* play);

void EnNiwGirl_Talk(EnNiwGirl* this, PlayState* play);
void func_80AB94D0(EnNiwGirl* this, PlayState* play);
void func_80AB9210(EnNiwGirl* this, PlayState* play);

const ActorInit En_Niw_Girl_InitVars = {
    ACTOR_EN_NIW_GIRL,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GR,
    sizeof(EnNiwGirl),
    (ActorFunc)EnNiwGirl_Init,
    (ActorFunc)EnNiwGirl_Destroy,
    (ActorFunc)EnNiwGirl_Update,
    (ActorFunc)EnNiwGirl_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

void EnNiwGirl_Init(Actor* thisx, PlayState* play) {
    EnNiwGirl* this = (EnNiwGirl*)thisx;
    s32 pad;
    Vec3f vec1;
    Vec3f vec2;
    s32 pad2;

    SkelAnime_InitFlex(play, &this->skelAnime, &gNiwGirlSkel, &gNiwGirlRunAnim, this->jointTable, this->morphTable,
                       17);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.targetMode = 6;
    if (this->actor.params < 0) {
        this->actor.params = 0;
    }
    this->path = ((this->actor.params >> 8) & 0xFF);
    this->actor.gravity = -3.0f;
    Matrix_RotateY((this->actor.shape.rot.y / 32768.0f) * M_PI, MTXMODE_NEW);
    vec2.x = vec2.y = vec2.z = 0.0f;
    vec1.x = vec1.y = 0.0f;
    vec1.z = 50.0;
    Matrix_MultVec3f(&vec1, &vec2);
    this->chasedEnNiw = (EnNiw*)Actor_SpawnAsChild(
        &play->actorCtx, &this->actor, play, ACTOR_EN_NIW, this->actor.world.pos.x + vec2.x,
        this->actor.world.pos.y + vec2.y, this->actor.world.pos.z + vec2.z, 0, this->actor.world.rot.y, 0, 0xA);
    if (this->chasedEnNiw != NULL) {
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ シツレイしちゃうわね！プンプン ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ きゃははははは、まてー ☆☆☆☆☆ %d\n" VT_RST, this->path);
        osSyncPrintf("\n\n");
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actionFunc = EnNiwGirl_Talk;
    } else {
        osSyncPrintf("\n\n");
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ なぜか、セットできむぅあせん ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ んんがくく ☆☆☆☆☆ %d\n" VT_RST, this->path);
        osSyncPrintf("\n\n");
        Actor_Kill(&this->actor);
    }
}

void EnNiwGirl_Destroy(Actor* thisx, PlayState* play) {
}

void EnNiwGirl_Jump(EnNiwGirl* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gNiwGirlRunAnim);
    Animation_Change(&this->skelAnime, &gNiwGirlRunAnim, 1.0f, 0.0f, frameCount, 0, -10.0f);
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actionFunc = func_80AB9210;
}

void func_80AB9210(EnNiwGirl* this, PlayState* play) {
    Path* path = &play->setupPathList[this->path];
    f32 xDistBetween;
    f32 zDistBetween;

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachF(&this->actor.speedXZ, 3.0f, 0.2f, 0.4f);

    // Find the X and Z distance between the girl and the cuckoo she is chasing
    xDistBetween = this->chasedEnNiw->actor.world.pos.x - this->actor.world.pos.x;
    zDistBetween = this->chasedEnNiw->actor.world.pos.z - this->actor.world.pos.z;
    if (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE) {
        this->chasedEnNiw->path = 0;
    }
    if (sqrtf(SQ(xDistBetween) + SQ(zDistBetween)) < 70.0f) {
        this->chasedEnNiw->path = (this->path + 1);
        this->chasedEnNiw->unk_2EC = path->count;
    } else if (sqrtf(SQ(xDistBetween) + SQ(zDistBetween)) > 150.0f) {
        this->chasedEnNiw->path = 0;
    }

    // Change her angle so that she is always facing the cuckoo
    Math_SmoothStepToS(&this->actor.shape.rot.y, Math_FAtan2F(xDistBetween, zDistBetween) * (0x8000 / M_PI), 3,
                       this->unk_27C, 0);
    Math_ApproachF(&this->unk_27C, 5000.0f, 30.0f, 150.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    // Only allow Link to talk to her when she is playing the jumping animation
    if ((this->jumpTimer == 0) || (Player_GetMask(play) != PLAYER_MASK_NONE)) {
        this->jumpTimer = 60;
        this->actionFunc = EnNiwGirl_Talk;
    }
}

void EnNiwGirl_Talk(EnNiwGirl* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gNiwGirlJumpAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gNiwGirlJumpAnim), 0,
                     -10.0f);
    this->actor.flags |= ACTOR_FLAG_0;
    this->actor.textId = 0x7000;
    if ((gSaveContext.eventChkInf[8] & 1) && (this->unk_27A == 0)) {
        this->actor.textId = 0x70EA;
    }
    switch (Player_GetMask(play)) {
        case PLAYER_MASK_KEATON:
            this->actor.textId = 0x7118;
            break;
        case PLAYER_MASK_SPOOKY:
            this->actor.textId = 0x7119;
            break;
        case PLAYER_MASK_BUNNY:
        case PLAYER_MASK_ZORA:
        case PLAYER_MASK_GERUDO:
            this->actor.textId = 0x711A;
            break;
        case PLAYER_MASK_SKULL:
        case PLAYER_MASK_GORON:
        case PLAYER_MASK_TRUTH:
            this->actor.textId = 0x711B;
            break;
    }
    this->unk_270 = 6;
    this->actionFunc = func_80AB94D0;
}

void func_80AB94D0(EnNiwGirl* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE) {
        this->chasedEnNiw->path = 0;
    }
    Math_ApproachZeroF(&this->actor.speedXZ, 0.8f, 0.2f);
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if (this->actor.textId == 0x70EA) {
            this->unk_27A = 1;
        }
    } else {
        if ((this->jumpTimer == 0) && Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
            this->jumpTimer = Rand_ZeroFloat(100.0f) + 250.0f;
            this->actionFunc = EnNiwGirl_Jump;
        } else {
            func_8002F2CC(&this->actor, play, 100.0f);
        }
    }
}

void EnNiwGirl_Update(Actor* thisx, PlayState* play) {
    EnNiwGirl* this = (EnNiwGirl*)thisx;
    EnNiwGirlActionFunc tempActionFunc;
    Player* player = GET_PLAYER(play);

    Actor_SetScale(&this->actor, 0.013f);
    this->unkUpTimer++;
    tempActionFunc = func_80AB94D0;
    if (this->blinkTimer == 0) {
        this->eyeIndex++;
        if (this->eyeIndex >= 3) {
            this->eyeIndex = 0;
            this->blinkTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
        }
    }
    this->unk_280 = 30.0f;
    Actor_SetFocus(&this->actor, 30.0f);
    if (tempActionFunc == this->actionFunc) {
        this->unk_2D4.unk_18 = player->actor.world.pos;
        if (!LINK_IS_ADULT) {
            this->unk_2D4.unk_18.y = player->actor.world.pos.y - 10.0f;
        }
        func_80034A14(&this->actor, &this->unk_2D4, 2, 4);
        this->unk_260 = this->unk_2D4.unk_08;
        this->unk_266 = this->unk_2D4.unk_0E;
    } else {
        Math_SmoothStepToS(&this->unk_266.y, 0, 5, 3000, 0);
        Math_SmoothStepToS(&this->unk_260.y, 0, 5, 3000, 0);
        Math_SmoothStepToS(&this->unk_260.z, 0, 5, 3000, 0);
    }
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    }
    if (this->jumpTimer != 0) {
        this->jumpTimer--;
    }
    this->actionFunc(this, play);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 100.0f, 100.0f, 200.0f, 0x1C);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnNiwGirlOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                              void* thisx) {
    EnNiwGirl* this = (EnNiwGirl*)thisx;

    if (limbIndex == 3) {
        rot->x += this->unk_266.y;
    }
    if (limbIndex == 4) {
        rot->x += this->unk_260.y;
        rot->z += this->unk_260.z;
    }
    return false;
}

static Vec3f sConstVec3f = { 0.2f, 0.2f, 0.2f };

void EnNiwGirl_Draw(Actor* thisx, PlayState* play) {
    static void* eyeTextures[] = { gNiwGirlEyeOpenTex, gNiwGirlEyeHalfTex, gNiwGirlEyeClosedTex };
    EnNiwGirl* this = (EnNiwGirl*)thisx;
    s32 pad;
    Vec3f sp4C = sConstVec3f;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnNiwGirlOverrideLimbDraw, NULL, this);
    func_80033C30(&this->actor.world.pos, &sp4C, 255, play);

    CLOSE_DISPS(play->state.gfxCtx);
}
