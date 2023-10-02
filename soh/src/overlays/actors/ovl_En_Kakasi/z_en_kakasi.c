/*
 * File: z_en_kakasi.c
 * Overlay: ovl_En_Kakasi
 * Description: Pierre the Scarecrow
 */

#include "z_en_kakasi.h"
#include "vt.h"
#include "objects/object_ka/object_ka.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_NO_FREEZE_OCARINA)

void EnKakasi_Init(Actor* thisx, PlayState* play);
void EnKakasi_Destroy(Actor* thisx, PlayState* play);
void EnKakasi_Update(Actor* thisx, PlayState* play);
void EnKakasi_Draw(Actor* thisx, PlayState* play);

void func_80A8F660(EnKakasi* this, PlayState* play);
void func_80A8F75C(EnKakasi* this, PlayState* play);
void func_80A8F8D0(EnKakasi* this, PlayState* play);
void func_80A8F9C8(EnKakasi* this, PlayState* play);
void func_80A8FBB8(EnKakasi* this, PlayState* play);
void func_80A8FAA4(EnKakasi* this, PlayState* play);

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
        BUMP_NONE | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

const ActorInit En_Kakasi_InitVars = {
    ACTOR_EN_KAKASI,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_KA,
    sizeof(EnKakasi),
    (ActorFunc)EnKakasi_Init,
    (ActorFunc)EnKakasi_Destroy,
    (ActorFunc)EnKakasi_Update,
    (ActorFunc)EnKakasi_Draw,
    NULL,
};

void EnKakasi_Destroy(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    SkelAnime_Free(&this->skelanime, play);
}

void EnKakasi_Init(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;

    osSyncPrintf("\n\n");
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ Ｌｅｔ’ｓ ＤＡＮＣＥ！ ☆☆☆☆☆ %f\n" VT_RST, this->actor.world.pos.y);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.targetMode = 6;
    SkelAnime_InitFlex(play, &this->skelanime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);

    this->rot = this->actor.world.rot;
    this->actor.flags |= ACTOR_FLAG_DRAGGED_BY_HOOKSHOT;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = func_80A8F660;
}

void func_80A8F28C(EnKakasi* this) {
    this->unk_1A4 = 0;
    this->skelanime.playSpeed = 0.0f;
    this->unk_1A8 = this->unk_1AC = 0;

    Math_ApproachZeroF(&this->skelanime.curFrame, 0.5f, 1.0f);
    Math_SmoothStepToS(&this->actor.shape.rot.x, this->rot.x, 5, 0x2710, 0);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->rot.y, 5, 0x2710, 0);
    Math_SmoothStepToS(&this->actor.shape.rot.z, this->rot.z, 5, 0x2710, 0);
}

void func_80A8F320(EnKakasi* this, PlayState* play, s16 arg) {
    s16 ocarinaNote = play->msgCtx.lastOcaNoteIdx;
    s16 currentFrame;

    if (arg != 0) {
        if (this->unk_19C[3] == 0) {
            this->unk_19C[3] = (s16)Rand_ZeroFloat(10.99f) + 30;
            this->unk_1A6 = (s16)Rand_ZeroFloat(4.99f);
        }

        this->unk_19A = (s16)Rand_ZeroFloat(2.99f) + 5;
        ocarinaNote = this->unk_1A6;
    }
    switch (ocarinaNote) {
        case OCARINA_NOTE_D4:
            this->unk_19A++;
            if (this->unk_1A4 == 0) {
                this->unk_1A4 = 1;
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_KAKASHI_ROLL);
            }
            break;
        case OCARINA_NOTE_F4:
            this->unk_19A++;
            this->unk_1B8 = 1.0f;
            break;
        case OCARINA_NOTE_A4:
            this->unk_19A++;
            if (this->unk_1AC == 0) {
                this->unk_1AC = 0x1388;
            }
            break;
        case OCARINA_NOTE_B4:
            this->unk_19A++;
            if (this->unk_1A8 == 0) {
                this->unk_1A8 = 0x1388;
            }
            break;
        case OCARINA_NOTE_D5:
            this->unk_19A++;
            this->unk_1B8 = 2.0f;
            break;
    }

    if (this->unk_19A > 8) {
        this->unk_19A = 8;
    }

    if (this->unk_19A != 0) {
        this->actor.gravity = -1.0f;
        if (this->unk_19A == 8 && (this->actor.bgCheckFlags & 1)) {
            this->actor.velocity.y = 3.0f;
            Audio_PlayActorSound2(&this->actor, NA_SE_IT_KAKASHI_JUMP);
        }
        Math_ApproachF(&this->skelanime.playSpeed, this->unk_1B8, 0.1f, 0.2f);
        Math_SmoothStepToS(&this->actor.shape.rot.x, this->unk_1A8, 5, 0x3E8, 0);
        Math_SmoothStepToS(&this->actor.shape.rot.z, this->unk_1AC, 5, 0x3E8, 0);

        if (this->unk_1A8 != 0 && fabsf(this->actor.shape.rot.x - this->unk_1A8) < 50.0f) {
            this->unk_1A8 *= -1.0f;
        }
        if (this->unk_1AC != 0 && fabsf(this->actor.shape.rot.z - this->unk_1AC) < 50.0f) {
            this->unk_1AC *= -1.0f;
        }

        if (this->unk_1A4 != 0) {
            this->actor.shape.rot.y += 0x1000;
            if (this->actor.shape.rot.y == 0) {
                this->unk_1A4 = 0;
            }
        }
        currentFrame = this->skelanime.curFrame;
        if (currentFrame == 11 || currentFrame == 17) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_KAKASHI_SWING);
        }
        SkelAnime_Update(&this->skelanime);
    }
}

void func_80A8F660(EnKakasi* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&object_ka_Anim_000214);

    Animation_Change(&this->skelanime, &object_ka_Anim_000214, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);

    this->actor.textId = 0x4076;
    this->unk_196 = TEXT_STATE_DONE;
    if (!LINK_IS_ADULT) {
        this->unk_194 = false;
        if (gSaveContext.scarecrowLongSongSet) {
            this->actor.textId = 0x407A;
            this->unk_196 = TEXT_STATE_EVENT;
        }
    } else {
        this->unk_194 = true;
        if (gSaveContext.scarecrowLongSongSet) {
            this->actor.textId = 0x4079;
            this->unk_196 = TEXT_STATE_EVENT;
        }
    }
    this->actionFunc = func_80A8F75C;
}

void func_80A8F75C(EnKakasi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    func_80A8F28C(this);
    SkelAnime_Update(&this->skelanime);
    this->camId = SUBCAM_NONE;
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if (this->unk_196 == TEXT_STATE_EVENT) {
            this->actionFunc = func_80A8F9C8;
        } else {
            this->actionFunc = func_80A8F660;
        }
    } else {
        s16 yawTowardsPlayer = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

        if (!(this->actor.xzDistToPlayer > 120.0f)) {
            s16 absyawTowardsPlayer = ABS(yawTowardsPlayer);

            if (absyawTowardsPlayer < 0x4300) {
                if (!this->unk_194) {
                    if (player->stateFlags2 & 0x1000000) {
                        this->camId = OnePointCutscene_Init(play, 2260, -99, &this->actor, MAIN_CAM);

                        func_8010BD58(play, OCARINA_ACTION_SCARECROW_LONG_RECORDING);
                        this->unk_19A = 0;
                        this->unk_1B8 = 0.0;
                        player->stateFlags2 |= 0x800000;
                        this->actionFunc = func_80A8F8D0;
                        return;
                    }
                    if (this->actor.xzDistToPlayer < 80.0f) {
                        player->stateFlags2 |= 0x800000;
                    }
                }
                func_8002F2CC(&this->actor, play, 100.0f);
            }
        }
    }
}

void func_80A8F8D0(EnKakasi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04 && play->msgCtx.msgMode == MSGMODE_NONE) {
        // "end?"
        osSyncPrintf(VT_FGCOL(BLUE) "☆☆☆☆☆ 終り？ ☆☆☆☆☆ \n" VT_RST);

        if (this->unk_19A != 0) {
            Message_CloseTextbox(play);
            this->actor.textId = 0x4077;
            this->unk_196 = TEXT_STATE_EVENT;
            Message_StartTextbox(play, this->actor.textId, NULL);
            this->actionFunc = func_80A8F9C8;
        } else {
            OnePointCutscene_EndCutscene(play, this->camId);
            this->camId = SUBCAM_NONE;
            this->actionFunc = func_80A8F660;
        }
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        func_80A8F320(this, play, 0);
        player->stateFlags2 |= 0x800000;
    }
}

void func_80A8F9C8(EnKakasi* this, PlayState* play) {
    func_80A8F28C(this);
    SkelAnime_Update(&this->skelanime);
    func_8002DF54(play, NULL, 8);

    if (this->unk_196 == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play)) {

        if (this->camId != SUBCAM_NONE) {
            func_8005B1A4(play->cameraPtrs[this->camId]);
        }
        this->camId = OnePointCutscene_Init(play, 2270, -99, &this->actor, MAIN_CAM);
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        func_8002DF54(play, NULL, 8);
        func_8010BD58(play, OCARINA_ACTION_SCARECROW_LONG_PLAYBACK);
        this->actionFunc = func_80A8FAA4;
    }
}

void func_80A8FAA4(EnKakasi* this, PlayState* play) {
    if (play->msgCtx.ocarinaMode != OCARINA_MODE_0F) {
        func_80A8F320(this, play, 1);
        return;
    }

    osSyncPrintf("game_play->message.msg_mode=%d\n", play->msgCtx.msgMode);

    if (play->msgCtx.msgMode == MSGMODE_NONE) {
        if (this->unk_194) {
            this->actor.textId = 0x4077;
            this->unk_196 = TEXT_STATE_EVENT;
            Message_StartTextbox(play, this->actor.textId, NULL);
        } else {
            this->actor.textId = 0x4078;
            this->unk_196 = TEXT_STATE_EVENT;
            Message_StartTextbox(play, this->actor.textId, NULL);
        }
        this->actionFunc = func_80A8FBB8;
        OnePointCutscene_EndCutscene(play, this->camId);
        this->camId = SUBCAM_NONE;
        this->camId = OnePointCutscene_Init(play, 2260, -99, &this->actor, MAIN_CAM);
        func_8005B1A4(play->cameraPtrs[this->camId]);
    }
}

void func_80A8FBB8(EnKakasi* this, PlayState* play) {
    func_80A8F28C(this);
    SkelAnime_Update(&this->skelanime);

    if (this->unk_196 == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play)) {
        func_8005B1A4(play->cameraPtrs[this->camId]);
        Message_CloseTextbox(play);
        func_8002DF54(play, NULL, 7);
        this->actionFunc = func_80A8F660;
    }
}

void EnKakasi_Update(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;
    s32 pad;
    s32 i;

    this->unk_198++;
    this->actor.world.rot = this->actor.shape.rot;
    for (i = 0; i < ARRAY_COUNT(this->unk_19C); i++) {
        if (this->unk_19C[i] != 0) {
            this->unk_19C[i]--;
        }
    }

    this->height = 60.0f;
    Actor_SetFocus(&this->actor, this->height);
    this->actionFunc(this, play);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 50.0f, 100.0f, 28);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnKakasi_Draw(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;

    if (BREG(3) != 0) {
        osSyncPrintf("\n\n");
        // "flag!"
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ フラグ！ ☆☆☆☆☆ %d\n" VT_RST, gSaveContext.scarecrowLongSongSet);
    }
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawSkeletonOpa(play, &this->skelanime, NULL, NULL, this);
}
