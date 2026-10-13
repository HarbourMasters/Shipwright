/*
 * File: z_en_gg.c
 * Overlay: ovl_En_Gg
 * Description: Darmani's Ghost
 */

#include "z_en_gg.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_REACT_TO_LENS)

void EnGg_Init(Actor* thisx, PlayState* play);
void EnGg_Destroy(Actor* thisx, PlayState* play);
void EnGg_Update(Actor* thisx, PlayState* play);
void EnGg_Draw(Actor* thisx, PlayState* play);

void func_80B352A4(EnGg* this, PlayState* play);
void func_80B35450(EnGg* this, PlayState* play);
void func_80B3556C(EnGg* this, PlayState* play);
u16 func_80B357F0(EnGg* this);
void func_80B358D8(EnGg* this, PlayState* play);
void func_80B359DC(EnGg* this, PlayState* play);
void func_80B363E8(EnGgStruct* ptr, PlayState* play, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3);
void func_80B364D4(EnGgStruct* ptr, PlayState* play);

ActorProfile En_Gg_Profile = {
    /**/ ACTOR_EN_GG,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GG,
    /**/ sizeof(EnGg),
    /**/ EnGg_Init,
    /**/ EnGg_Destroy,
    /**/ EnGg_Update,
    /**/ EnGg_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 24, 72, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 24, 72, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(0, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

typedef enum {
    /*   -1 */ ENGG_ANIM_NONE = -1,
    /* 0x00 */ ENGG_ANIM_0,
    /* 0x01 */ ENGG_ANIM_1,
    /* 0x02 */ ENGG_ANIM_2,
    /* 0x03 */ ENGG_ANIM_3,
    /* 0x04 */ ENGG_ANIM_4,
    /* 0x05 */ ENGG_ANIM_5,
    /* 0x06 */ ENGG_ANIM_6,
    /* 0x07 */ ENGG_ANIM_7,
    /* 0x08 */ ENGG_ANIM_8,
    /* 0x09 */ ENGG_ANIM_9,
    /* 0x0A */ ENGG_ANIM_10,
    /* 0x0B */ ENGG_ANIM_11,
    /* 0x0C */ ENGG_ANIM_12,
    /* 0x0D */ ENGG_ANIM_13,
    /* 0x0E */ ENGG_ANIM_14,
    /* 0x0F */ ENGG_ANIM_15,
    /* 0x10 */ ENGG_ANIM_MAX
} EnGgAnimation;

static AnimationInfo sAnimationInfo[ENGG_ANIM_MAX] = {
    { &object_gg_Anim_00F578, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG_ANIM_0
    { &object_gg_Anim_00D528, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -10.0f }, // ENGG_ANIM_1
    { &object_gg_Anim_00D174, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -10.0f }, // ENGG_ANIM_2
    { &object_gg_Anim_00ECC0, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -10.0f }, // ENGG_ANIM_3
    { &object_gg_Anim_00BAF0, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG_ANIM_4
    { &object_gg_Anim_00AF40, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG_ANIM_5
    { &object_gg_Anim_00F578, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG_ANIM_6
    { &object_gg_Anim_00AF40, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG_ANIM_7
    { &object_gg_Anim_00F578, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG_ANIM_8
    { &object_gg_Anim_0100C8, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },   // ENGG_ANIM_9
    { &object_gg_Anim_00CDA0, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },   // ENGG_ANIM_10
    { &object_gg_Anim_00B560, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },   // ENGG_ANIM_11
    { &object_gg_Anim_00A4B4, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },   // ENGG_ANIM_12
    { &object_gg_Anim_00E86C, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },   // ENGG_ANIM_13
    { &object_gg_Anim_00D99C, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },   // ENGG_ANIM_14
    { &object_gg_Anim_00E2A4, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },   // ENGG_ANIM_15
};

void func_80B34F70(EnGg* this) {
    this->actor.focus.pos.x = this->actor.world.pos.x;
    this->actor.focus.pos.y = this->actor.world.pos.y + 80.0f;
    this->actor.focus.pos.z = this->actor.world.pos.z;

    this->actor.focus.rot.x = this->actor.world.rot.x;
    this->actor.focus.rot.y = this->actor.world.rot.y;
    this->actor.focus.rot.z = this->actor.world.rot.z;
}

s32 func_80B34FB4(EnGg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp40;
    Vec3f sp34;
    s16 pitch;

    sp40 = player->actor.world.pos;
    sp40.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    sp34 = this->actor.world.pos;
    sp34.y += 70.0f;
    pitch = Math_Vec3f_Pitch(&sp34, &sp40);

    if ((this->actor.xzDistToPlayer < 250.0f) && (this->actor.xzDistToPlayer > 50.0f) &&
        (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS) || CHECK_WEEKEVENTREG(WEEKEVENTREG_19_80))) {
        Math_SmoothStepToS(&this->unk_2E8, pitch, 4, 0x2AA8, 1);
    } else {
        Math_SmoothStepToS(&this->unk_2E8, 0, 4, 0x2AA8, 1);
    }
    this->unk_2E8 = CLAMP(this->unk_2E8, 0, 0x1C70);
    return true;
}

void func_80B35108(EnGg* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 30.0f, 30.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
}

void func_80B351A4(EnGg* this) {
    if ((this->animIndex == ENGG_ANIM_2) || (this->animIndex == ENGG_ANIM_3)) {
        this->unk_2E2 = 3;
    } else {
        s16 temp = this->unk_2E4 - 1;

        if (temp >= 3) {
            this->unk_2E2 = 0;
            this->unk_2E4 = temp;
        } else if (temp == 0) {
            this->unk_2E2 = 2;
            this->unk_2E4 = (s32)(Rand_ZeroOne() * 60.0f) + 20;
        } else {
            this->unk_2E2 = 1;
            this->unk_2E4 = temp;
        }
    }
}

void func_80B35250(EnGg* this) {
    this->unk_2E4 = 20;
    this->unk_2E2 = 0;
    this->animIndex = ENGG_ANIM_0;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG_ANIM_0);
    this->actionFunc = func_80B35450;
}

void func_80B352A4(EnGg* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (curFrame == endFrame) {
        switch (this->actor.textId) {
            case 0xCE5:
                this->animIndex = ENGG_ANIM_1;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG_ANIM_1);
                break;

            case 0xCE6:
            case 0xCEC:
                this->animIndex = ENGG_ANIM_0;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG_ANIM_0);
                break;

            case 0xCE8:
                this->animIndex = ENGG_ANIM_2;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG_ANIM_2);
                break;

            case 0xCE9:
                this->animIndex = ENGG_ANIM_3;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG_ANIM_3);
                break;

            case 0xCED:
            case 0xCEE:
                this->animIndex = ENGG_ANIM_4;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG_ANIM_4);
                break;

            default:
                this->animIndex = ENGG_ANIM_0;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG_ANIM_0);
                break;
        }
        SET_WEEKEVENTREG(WEEKEVENTREG_19_80);
        this->actionFunc = func_80B3556C;
    } else if ((this->animIndex == ENGG_ANIM_0) && ((this->actor.textId == 0xCED) || (this->actor.textId == 0xCEE))) {
        if (curFrame < (endFrame - 1)) {
            this->skelAnime.playSpeed = 2.0f;
        } else {
            this->animIndex = ENGG_ANIM_4;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG_ANIM_4);
        }
    }
}

void func_80B35450(EnGg* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_91_10) && (play->csCtx.state == CS_STATE_IDLE)) {
        func_80B359DC(this, play);
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
            Actor_DeactivateLens(play);
        }
        this->unk_308 = 1;
        this->actionFunc = func_80B352A4;
    } else if ((this->actor.xzDistToPlayer < 200.0f) && (this->actor.xzDistToPlayer > 50.0f)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_19_80)) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
            this->actor.textId = 0xCEE;
        } else if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
            this->actor.textId = 0xCE5;
        }
    }
}

void func_80B3556C(EnGg* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->animIndex == ENGG_ANIM_4) {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->unk_308 = 0;
            this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
            func_80B35250(this);
        } else {
            this->actor.textId = func_80B357F0(this);
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            this->actionFunc = func_80B352A4;
        }
    }
}

void func_80B35634(EnGg* this, PlayState* play) {
    s32 pad;
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_119)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_119);

        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            this->cueId = play->csCtx.actorCues[cueChannel]->id;

            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    this->csAnimIndex = ENGG_ANIM_0;
                    this->animIndex = ENGG_ANIM_0;
                    break;

                case 2:
                    this->csAnimIndex = ENGG_ANIM_9;
                    this->animIndex = ENGG_ANIM_9;
                    break;

                case 3:
                    this->csAnimIndex = ENGG_ANIM_10;
                    this->animIndex = ENGG_ANIM_10;
                    break;

                case 4:
                    this->unk_344.animIndex = this->csAnimIndex = ENGG_ANIM_11;
                    this->animIndex = ENGG_ANIM_11;
                    func_80B364D4(&this->unk_344, play);
                    break;

                case 5:
                    this->unk_344.animIndex = this->csAnimIndex = ENGG_ANIM_12;
                    //! FAKE:
                    if (1) {}
                    this->animIndex = ENGG_ANIM_12;
                    break;

                case 6:
                    this->unk_344.animIndex = this->csAnimIndex = ENGG_ANIM_13;
                    //! FAKE:
                    if (1) {}
                    this->animIndex = ENGG_ANIM_13;
                    break;

                case 7:
                    this->unk_344.animIndex = this->csAnimIndex = ENGG_ANIM_14;
                    this->animIndex = ENGG_ANIM_14;
                    func_80B364D4(&this->unk_344, play);
                    break;

                case 8:
                    Actor_Kill(&this->actor);
                    return;

                default:
                    this->csAnimIndex = ENGG_ANIM_0;
                    break;
            }
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->csAnimIndex);
        }

        if (this->csAnimIndex == ENGG_ANIM_14) {
            func_80B358D8(this, play);
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        this->actor.shape.yOffset = 0.0f;
    } else {
        this->cueId = 99;
    }
}

u16 func_80B357F0(EnGg* this) {
    switch (this->actor.textId) {
        case 0xCE5:
            return 0xCE6;

        case 0xCE6:
            return 0xCE8;

        case 0xCE8:
            return 0xCE9;

        case 0xCE9:
            return 0xCEC;

        case 0xCEC:
            return 0xCED;

        default:
            return 0;
    }
}

void func_80B3584C(EnGg* this) {
    this->unk_30A += 0x72C;
    this->actor.shape.yOffset = Math_SinS(this->unk_30A) * 100.0f;
    this->actor.shape.shadowScale = 30.0f - (Math_SinS(this->unk_30A) * 5.0f);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void func_80B358D8(EnGg* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->csAnimIndex].animation);

    if ((this->animIndex == ENGG_ANIM_14) && (curFrame == endFrame)) {
        this->animIndex = ENGG_ANIM_15;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG_ANIM_15);
    }
}

void func_80B35968(EnGg* this, PlayState* play) {
    if (this->unk_344.unk_34 != NULL) {
        this->unk_344.unk_34(&this->unk_344, play);
    } else {
        if ((this->csAnimIndex == ENGG_ANIM_11) || (this->csAnimIndex == ENGG_ANIM_14)) {
            this->unk_344.animIndex = this->csAnimIndex;
        }
        func_80B363E8(&this->unk_344, play, &this->unk_320, &this->unk_32C, &this->unk_338);
    }
}

void func_80B359DC(EnGg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.xzDistToPlayer < 200.0f) {
        if (this->unk_306 == 0) {
            if (player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) {
                this->unk_306 = 1;
                Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
            }
        } else if (!(player->stateFlags2 & PLAYER_STATE2_USING_OCARINA)) {
            this->unk_306 = 0;
        }

        if ((player->transformation == PLAYER_FORM_HUMAN) && (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
            (play->msgCtx.lastPlayedSong == OCARINA_SONG_HEALING)) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_19_80)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_19_80);
            }
            this->unk_307 = true;
        }

        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_Start(this->csId, &this->actor);
            this->unk_307 = false;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }

            if (this->unk_307) {
                CutsceneManager_Queue(this->csId);
            }
        }
    } else {
        this->unk_307 = false;
    }
}

void func_80B35B24(EnGgStruct* ptr, PlayState* play) {
    ptr->unk_34 = NULL;
    ptr->unk_38 = NULL;
    ptr->unk_3C = 0;
    ptr->unk_40 = 0;
    ptr->unk_44 = 0;
}

void func_80B35B44(EnGgStruct* ptr, PlayState* play) {
    if (ptr->animIndex == ENGG_ANIM_14) {
        ptr->unk_40++;
        if (ptr->unk_40 > 0x46) {
            ptr->unk_48 = 1;
            ptr->unk_40 = 0;
        }

        if (ptr->unk_40 != 0x46) {
            if (ptr->unk_48 != 0) {
                ptr->unk_44 = 0x37;
            } else {
                ptr->unk_44 = ptr->unk_40 - 15;
                if (ptr->unk_44 < 0) {
                    ptr->unk_44 = 0;
                }
            }
        } else {
            ptr->unk_44++;
            if (ptr->unk_44 > 0x37) {
                ptr->unk_44 = 0x37;
            }
        }
    } else if ((ptr->animIndex == ENGG_ANIM_11) || (ptr->animIndex == ENGG_ANIM_12)) {
        ptr->unk_40++;
        if (ptr->unk_40 > 0x46) {
            ptr->unk_48 = 1;
            ptr->unk_40 = 0;
        }
    } else if (ptr->animIndex == ENGG_ANIM_13) {
        ptr->unk_48 = 0;
        ptr->unk_40++;
        if (ptr->unk_40 > 0x46) {
            ptr->unk_40 = 0x46;
        }

        if (ptr->unk_40 != 0x46) {
            ptr->unk_44 = ptr->unk_40 - 15;
            if (ptr->unk_44 < 0) {
                ptr->unk_44 = 0;
            }
        } else {
            ptr->unk_44++;
            if (ptr->unk_44 > 0x37) {
                func_80B364D4(ptr, play);
            }
        }
    }
}

void func_80B35C84(EnGgStruct* ptr, PlayState* play) {
    s32 sp74;
    s32 phi_s7;
    s32 i;
    s32 ten = 10;

    if (ptr->unk_48 != 0) {
        sp74 = ptr->unk_40 % ten;
        ptr->unk_40 = sp74;
        phi_s7 = 70;
    } else if (ptr->animIndex == ENGG_ANIM_13) {
        sp74 = ptr->unk_40;
        phi_s7 = 70;
    } else {
        phi_s7 = ptr->unk_40 - ptr->unk_44;
        sp74 = ptr->unk_40 % ten;
    }

    if (phi_s7 <= 0) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();

    for (i = sp74; i < phi_s7; i += ten) {
        f32 temp = i * 0.14f;
        f32 temp_f22 = ptr->unk_00.x + (ptr->unk_18.x * temp) + (0.5f * ptr->unk_24.x * temp * temp);
        f32 temp_f24 = ptr->unk_00.y - Math_SinS((i * 0x27FFB) / 70);
        f32 temp_f26 = ptr->unk_00.z + (ptr->unk_18.z * temp) + (0.5f * ptr->unk_24.z * temp * temp);
        f32 temp_f20 = Rand_ZeroOne() * 0.003f;

        Matrix_Translate(temp_f22, temp_f24, temp_f26, MTXMODE_NEW);
        Matrix_Scale(temp_f20, temp_f20, temp_f20, MTXMODE_APPLY);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 150, 0, 255);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
    }

    Matrix_Pop();

    Matrix_Push();

    for (i = sp74; i < phi_s7; i += ten) {
        f32 temp = i * 0.14f;
        f32 temp_f22 = ptr->unk_0C.x + ((ptr->unk_18.x * temp) + (0.5f * ptr->unk_24.x * temp * temp));
        f32 temp_f24 = ptr->unk_0C.y - Math_SinS((i * 0x27FFB) / 70);
        f32 temp_f26 = ptr->unk_0C.z + (ptr->unk_18.z * temp) + (0.5f * ptr->unk_24.z * temp * temp);
        f32 temp_f20 = Rand_ZeroOne() * 0.003f;

        Matrix_Translate(temp_f22, temp_f24, temp_f26, MTXMODE_NEW);
        Matrix_Scale(temp_f20, temp_f20, temp_f20, MTXMODE_APPLY);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 150, 0, 255);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
    }

    Matrix_Pop();

    gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80B3610C(EnGgStruct* ptr, PlayState* play) {
    f32 temp_f20;
    f32 temp_f24;
    f32 temp_f26;
    f32 temp_f28;
    f32 temp_f30;
    s32 phi_s4;
    f32 phi_f22;
    s32 i;

    if (ptr->unk_48 != 0) {
        phi_s4 = 0x46;
    } else {
        phi_s4 = ptr->unk_40;
    }

    if (phi_s4 > 0) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();

        for (i = 0; i < phi_s4; i++) {
            if (ptr->unk_48 != 0) {
                phi_f22 = phi_s4 / 70.0f;
            } else {
                phi_f22 = i / 70.0f;
            }
            temp_f24 = ptr->unk_0C.x + (ptr->unk_18.x * i) + (0.5f * ptr->unk_24.x * i * i);
            temp_f26 = ptr->unk_0C.y + (ptr->unk_18.y * i) + (0.5f * ptr->unk_24.y * i * i);
            temp_f28 = ptr->unk_0C.z + (ptr->unk_18.z * i) + (0.5f * ptr->unk_24.z * i * i);
            temp_f20 = Rand_ZeroOne() * 0.003f;

            Matrix_Translate((Rand_Centered() * (100.0f * phi_f22)) + temp_f24, temp_f26,
                             ((30.0f * phi_f22) * Rand_Centered()) + temp_f28, MTXMODE_NEW);
            Matrix_Scale(temp_f20, temp_f20, temp_f20, MTXMODE_APPLY);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 150, 0, 255);

            Gfx_SetupDL25_Xlu(play->state.gfxCtx);
            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
        }

        Matrix_Pop();

        gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void func_80B363E8(EnGgStruct* ptr, PlayState* play, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3) {
    ptr->unk_34 = func_80B35B44;
    ptr->unk_00 = *arg1;
    ptr->unk_0C = *arg1;
    ptr->unk_18 = *arg2;
    ptr->unk_24 = *arg3;
    ptr->unk_48 = 0;

    ptr->unk_00.x += -4.0f;
    ptr->unk_0C.x += 4.0f;

    if ((ptr->animIndex == ENGG_ANIM_11) || (ptr->animIndex == ENGG_ANIM_12) || (ptr->animIndex == ENGG_ANIM_13)) {
        ptr->unk_38 = func_80B35C84;
    } else if (ptr->animIndex == ENGG_ANIM_14) {
        ptr->unk_38 = func_80B3610C;
    }
}

void func_80B364D4(EnGgStruct* ptr, PlayState* play) {
    ptr->unk_34 = NULL;
    ptr->unk_38 = NULL;
    func_80B35B24(ptr, play);
}

void EnGg_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGg* this = (EnGg*)thisx;

    if (GameInteractor_Should(VB_CONSIDER_DARMANI_HEALED, INV_CONTENT(ITEM_MASK_GORON) == ITEM_MASK_GORON)) {
        Actor_Kill(&this->actor);
        return;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->actor.bgCheckFlags |= BGCHECKFLAG_PLAYER_400;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_gg_Skel_00F6C0, &object_gg_Anim_00F578, this->jointTable,
                       this->morphTable, OBJECT_GG_LIMB_MAX);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_04);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_08);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_10);
    this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
    this->unk_310 = this->actor.home.pos.y;
    this->csId = this->actor.csId;
    this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
    this->unk_308 = 0;
    this->unk_309 = 0;
    this->unk_304 = 0;
    this->unk_30A = 0;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    func_80B35B24(&this->unk_344, play);
    func_80B35250(this);
}

void EnGg_Destroy(Actor* thisx, PlayState* play) {
}

void EnGg_Update(Actor* thisx, PlayState* play) {
    EnGg* this = (EnGg*)thisx;

    if (play->actorCtx.lensMaskSize == LENS_MASK_ACTIVE_SIZE) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_19_80)) {
        if (play->csCtx.state == CS_STATE_IDLE) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        } else {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        }
    }

    if ((play->csCtx.state == CS_STATE_IDLE) &&
        ((this->csAnimIndex != ENGG_ANIM_14) && (this->csAnimIndex != ENGG_ANIM_11) &&
         (this->csAnimIndex != ENGG_ANIM_12) && (this->csAnimIndex != ENGG_ANIM_13))) {
        func_80B364D4(&this->unk_344, play);
    }

    if (((this->csAnimIndex == ENGG_ANIM_14) || (this->csAnimIndex == ENGG_ANIM_11) ||
         (this->csAnimIndex == ENGG_ANIM_12) || (this->csAnimIndex == ENGG_ANIM_13)) &&
        (this->unk_309 == 1)) {
        func_80B35968(this, play);
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_91_10) &&
        (CHECK_WEEKEVENTREG(WEEKEVENTREG_19_80) || CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS) ||
         (this->unk_308 == 1))) {
        SET_WEEKEVENTREG(WEEKEVENTREG_91_10);
    }

    this->actionFunc(this, play);

    func_80B34F70(this);
    func_80B35108(this, play);
    func_80B34FB4(this, play);
    Actor_MoveWithoutGravity(&this->actor);
    SkelAnime_Update(&this->skelAnime);

    if (play->csCtx.state == CS_STATE_IDLE) {
        func_80B3584C(this);
    } else {
        this->unk_2E8 = 0;
    }

    func_80B35634(this, play);
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    func_80B351A4(this);
}

s32 EnGg_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                          Gfx** gfx) {
    EnGg* this = (EnGg*)thisx;

    if (limbIndex == OBJECT_GG_LIMB_02) {
        Matrix_RotateZS(this->unk_2E8, MTXMODE_APPLY);
    }
    return false;
}

void EnGg_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    static Vec3f D_80B36DF0 = { 1800.0f, 300.0f, 200.0f };
    EnGg* this = (EnGg*)thisx;
    Vec3f sp30 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp24 = { 0.0f, 0.0f, 0.0f };

    if (this->csAnimIndex == ENGG_ANIM_14) {
        sp30.y = 3.0f;
        sp30.z = -1.0f;
        sp24.y = -0.07f;
        this->unk_309 = 1;
    } else if ((this->csAnimIndex == ENGG_ANIM_11) || (this->csAnimIndex == ENGG_ANIM_12) ||
               (this->csAnimIndex == ENGG_ANIM_13)) {
        sp30.x = 3.0f;
        sp24.x = 0.5f;
        this->unk_309 = 1;
    } else {
        this->unk_309 = 0;
    }

    if (limbIndex == OBJECT_GG_LIMB_04) {
        Matrix_MultVec3f(&D_80B36DF0, &this->unk_320);
        Matrix_Push();
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
        Matrix_MultVec3f(&sp30, &this->unk_32C);
        Matrix_MultVec3f(&sp24, &this->unk_338);
        Matrix_Pop();
    }
}

TexturePtr D_80B36DFC[] = {
    object_gg_Tex_009260,
    object_gg_Tex_009660,
    object_gg_Tex_009A60,
    object_gg_Tex_009E60,
};

void EnGg_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGg* this = (EnGg*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->unk_344.unk_38 != 0) {
        this->unk_344.unk_38(&this->unk_344, play);
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_19_80)) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80B36DFC[this->unk_2E2]));

        POLY_OPA_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnGg_OverrideLimbDraw, EnGg_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    } else if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS) || (this->unk_308 == 1)) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(D_80B36DFC[this->unk_2E2]));

        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnGg_OverrideLimbDraw, EnGg_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    }
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    CLOSE_DISPS(play->state.gfxCtx);
}
