/*
 * File: z_en_bombers2.c
 * Overlay: ovl_En_Bombers2
 * Description: Bombers - Hideout Guard
 */

#include "z_en_bombers2.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnBombers2_Init(Actor* thisx, PlayState* play);
void EnBombers2_Destroy(Actor* thisx, PlayState* play);
void EnBombers2_Update(Actor* thisx, PlayState* play);
void EnBombers2_Draw(Actor* thisx, PlayState* play);

void func_80C04BA0(EnBombers2* this, PlayState* play);
void func_80C04D8C(EnBombers2* this, PlayState* play);
void func_80C0520C(EnBombers2* this, PlayState* play);
void func_80C04B40(EnBombers2* this);
void func_80C04D00(EnBombers2* this);
void func_80C050B8(EnBombers2* this, PlayState* play);

ActorProfile En_Bombers2_Profile = {
    /**/ ACTOR_EN_BOMBERS2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_CS,
    /**/ sizeof(EnBombers2),
    /**/ EnBombers2_Init,
    /**/ EnBombers2_Destroy,
    /**/ EnBombers2_Update,
    /**/ EnBombers2_Draw,
};

static u16 sTextIds[] = {
    0x0727, 0x0728, 0x0726, 0x0729, 0x072B, 0x0725, 0x072A, 0x073C,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 35, 30, 0, { 0, 0, 0 } },
};

typedef enum {
    /* -1 */ ENBOMBERS_ANIM_NONE = -1,
    /*  0 */ ENBOMBERS_ANIM_0,
    /*  1 */ ENBOMBERS_ANIM_1,
    /*  2 */ ENBOMBERS_ANIM_2,
    /*  3 */ ENBOMBERS_ANIM_3,
    /*  4 */ ENBOMBERS_ANIM_4,
    /*  5 */ ENBOMBERS_ANIM_5,
    /*  6 */ ENBOMBERS_ANIM_6,
    /*  7 */ ENBOMBERS_ANIM_MAX
} EnBombersAnimation;

static AnimationHeader* sAnimations[ENBOMBERS_ANIM_MAX] = {
    &gBomberIdleAnim,       // ENBOMBERS_ANIM_0
    &object_cs_Anim_0053F4, // ENBOMBERS_ANIM_1
    &object_cs_Anim_01007C, // ENBOMBERS_ANIM_2
    &object_cs_Anim_00349C, // ENBOMBERS_ANIM_3
    &object_cs_Anim_0060E8, // ENBOMBERS_ANIM_4
    &object_cs_Anim_005DC4, // ENBOMBERS_ANIM_5
    &object_cs_Anim_0026B0, // ENBOMBERS_ANIM_6
};

static u8 sAnimationModes[ENBOMBERS_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_0
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_1
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_2
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_3
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_4
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_5
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_6
};

static Gfx sSetPrimColorDL[] = {
    gsDPSetPrimColor(0, 0xFF, 198, 140, 0, 255),
    gsSPEndDisplayList(),
};

static Gfx* sSetPrimColorDlPtr = sSetPrimColorDL;

static TexturePtr sEyeTextures[] = {
    object_cs_Tex_00C520,
    object_cs_Tex_00CD20,
    object_cs_Tex_00D520,
};

static TexturePtr D_80C05920 = object_cs_Tex_00E220;

void EnBombers2_Init(Actor* thisx, PlayState* play) {
    f32 cos;
    EnBombers2* this = (EnBombers2*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 19.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_cs_Skel_00F82C, &gBomberIdleAnim, this->jointTable,
                       this->morphTable, OBJECT_CS_LIMB_MAX);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_80) || (gSaveContext.save.entrance == ENTRANCE(EAST_CLOCK_TOWN, 2))) {
        this->actor.world.pos.x += Math_SinS(this->actor.home.rot.y + 0xC100) * 50.0f;
        cos = Math_CosS(this->actor.home.rot.y + 0xC100) * 50.0f;
        this->unk_2AC = true;
        this->actor.world.pos.z += cos;
    }
    this->csId = this->actor.csId;
    if (this->csId == 0) {
        Actor_Kill(&this->actor);
    }
    func_80C04B40(this);
}

void EnBombers2_Destroy(Actor* thisx, PlayState* play) {
    EnBombers2* this = (EnBombers2*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnBombers2_ChangeAnim(EnBombers2* this, s32 animIndex, f32 playSpeed) {
    f32 endFrame;

    this->animIndex = animIndex;
    endFrame = Animation_GetLastFrame(sAnimations[this->animIndex]);
    this->lastAnimFrame = endFrame;
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], playSpeed, 0.0f, endFrame,
                     sAnimationModes[this->animIndex], -10.0f);
}

void func_80C04B40(EnBombers2* this) {
    if (!this->unk_2AC) {
        EnBombers2_ChangeAnim(this, ENBOMBERS_ANIM_4, 1.0f);
    } else {
        EnBombers2_ChangeAnim(this, ENBOMBERS_ANIM_0, 1.0f);
    }
    this->unk_2C0 = 0;
    this->actionFunc = func_80C04BA0;
}

void func_80C04BA0(EnBombers2* this, PlayState* play) {
    s32 yawDiffAbs;
    s16 yawDiff;
    Player* player;

    yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
    yawDiffAbs = ABS_ALT(yawDiff);
    player = GET_PLAYER(play);

    switch (player->transformation) {
        // #region 2S2H - Enhancements. Fierce Deity gets same dialog as Goron and Zora
        case PLAYER_FORM_FIERCE_DEITY:
        // #endregion
        case PLAYER_FORM_GORON:
        case PLAYER_FORM_ZORA:
            this->textIdIndex = 1;
            this->talkState = TEXT_STATE_EVENT;
            break;

        case PLAYER_FORM_DEKU:
        case PLAYER_FORM_HUMAN:
            this->textIdIndex = 2;
            this->talkState = TEXT_STATE_EVENT;
            break;

        default:
            break;
    }
    if (this->unk_2AC) {
        this->textIdIndex = 7;
    }
    this->actor.textId = sTextIds[this->textIdIndex];

    if (Text_GetFaceReaction(play, FACE_REACTION_SET_BOMBERS_HIDEOUT_GUARD) != 0) {
        this->actor.textId = Text_GetFaceReaction(play, FACE_REACTION_SET_BOMBERS_HIDEOUT_GUARD);
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_2B6 = this->actor.world.rot.y;
        SET_WEEKEVENTREG(WEEKEVENTREG_86_02);
        func_80C04D00(this);
        return;
    }
    if (yawDiffAbs < 0x3BB5) {
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void func_80C04D00(EnBombers2* this) {
    if ((this->textIdIndex == 0) || (this->textIdIndex == 1)) {
        EnBombers2_ChangeAnim(this, ENBOMBERS_ANIM_5, 1.0f);
    } else {
        EnBombers2_ChangeAnim(this, ENBOMBERS_ANIM_1, 1.0f);
    }

    this->unk_2A8 = 0;
    if (this->unk_2AC) {
        EnBombers2_ChangeAnim(this, ENBOMBERS_ANIM_6, 1.0f);
    }
    this->unk_2C0 = 1;
    this->actionFunc = func_80C04D8C;
}

void func_80C04D8C(EnBombers2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;

    if (!this->unk_2AC && (curFrame >= this->lastAnimFrame)) {
        if (!(this->unk_2A8 & 1)) {
            EnBombers2_ChangeAnim(this, ENBOMBERS_ANIM_1, 1.0f);
        } else {
            EnBombers2_ChangeAnim(this, ENBOMBERS_ANIM_5, 1.0f);
        }
        this->unk_2A8++;
        this->unk_2A8 &= 1;
    }
    switch (player->transformation) {
        case PLAYER_FORM_HUMAN:
            this->unk_28E = -0xFA0;
            break;

        // #region 2S2H - Enhancements. Fierce Deity.
        case PLAYER_FORM_FIERCE_DEITY:
        // #endregion
        case PLAYER_FORM_GORON:
        case PLAYER_FORM_ZORA:
            this->unk_28E = -0x1770;
            break;

        default:
            break;
    }
    if ((this->talkState == Message_GetState(&play->msgCtx)) && Message_ShouldAdvance(play)) {
        if (this->talkState == TEXT_STATE_INPUT_BOMBER_CODE) {
            s32 i;
            s32 correctDigits;

            for (i = 0; i < ARRAY_COUNT(this->correctDigitSlots); i++) {
                if (!(this->correctDigitSlots[i]) &&
                    (play->msgCtx.unk12054[i] == gSaveContext.save.saveInfo.bomberCode[i])) {
                    this->correctDigitSlots[i] = true;
                }
            }
            for (i = 0, correctDigits = 0; i < ARRAY_COUNT(this->correctDigitSlots); i++) {
                if (this->correctDigitSlots[i]) {
                    correctDigits++;
                }
            }

            if (correctDigits >= 5) {
                this->textIdIndex = 6;
                this->actor.textId = sTextIds[this->textIdIndex];
                Message_ContinueTextbox(play, this->actor.textId);
                Actor_PlaySfx(&this->actor, NA_SE_SY_PIECE_OF_HEART);
                this->talkState = TEXT_STATE_EVENT;
                return;
            }
        }
        if (Text_GetFaceReaction(play, FACE_REACTION_SET_BOMBERS_HIDEOUT_GUARD) != 0) {
            this->unk_28E = 0;
            Message_CloseTextbox(play);
            func_80C04B40(this);
        } else {
            s32 j;

            switch (this->textIdIndex) {
                case 0:
                case 1:
                case 5:
                case 7:
                    this->unk_28E = 0;
                    Message_CloseTextbox(play);
                    func_80C04B40(this);
                    break;

                case 2:
                    this->textIdIndex = 3;
                    this->actor.textId = sTextIds[this->textIdIndex];
                    Message_ContinueTextbox(play, this->actor.textId);
                    this->talkState = TEXT_STATE_INPUT_BOMBER_CODE;
                    break;

                case 3:
                    for (j = 0; j < ARRAY_COUNT(this->correctDigitSlots); j++) {
                        this->correctDigitSlots[j] = false;
                    }
                    this->textIdIndex = 4;
                    Actor_PlaySfx(&this->actor, NA_SE_SY_ERROR);
                    this->actor.textId = sTextIds[this->textIdIndex];
                    Message_ContinueTextbox(play, this->actor.textId);
                    this->talkState = TEXT_STATE_EVENT;
                    break;

                case 4:
                    this->textIdIndex = 5;
                    this->actor.textId = sTextIds[this->textIdIndex];
                    Message_ContinueTextbox(play, this->actor.textId);
                    this->talkState = TEXT_STATE_EVENT;
                    break;

                case 6:
                    play->msgCtx.msgLength = 0;
                    func_80C050B8(this, play);
                    break;

                default:
                    break;
            }
        }
    }
}

void func_80C050B8(EnBombers2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 homeYawToPlayer;

    EnBombers2_ChangeAnim(this, ENBOMBERS_ANIM_2, 1.0f);
    this->unk_2A8 = 0;
    homeYawToPlayer = Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos);
    Math_Vec3f_Copy(&this->unk_29C, &this->actor.world.pos);
    if (this->actor.home.rot.y < homeYawToPlayer) {
        this->unk_29C.x += Math_SinS(this->actor.home.rot.y + 0xC100) * 50.0f;
        this->unk_29C.z += Math_CosS(this->actor.home.rot.y + 0xC100) * 50.0f;
    } else {
        this->unk_29C.x += Math_SinS(this->actor.home.rot.y + 0x3F00) * 50.0f;
        this->unk_29C.z += Math_CosS(this->actor.home.rot.y + 0x3F00) * 50.0f;
    }
    this->unk_2B6 = Math_Vec3f_Yaw(&this->actor.home.pos, &this->unk_29C);
    this->unk_2C0 = 2;
    this->actionFunc = func_80C0520C;
}

void func_80C0520C(EnBombers2* this, PlayState* play) {
    if (this->unk_2A8 == 0) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csId);
        } else if (!CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_Queue(this->csId);
        } else {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
            this->unk_2A8 = 1;
        }
    } else {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2B6, 1, 0xBB8, 0);
        if ((fabsf(this->unk_29C.x - this->actor.world.pos.x) < 3.0f) &&
            (fabsf(this->unk_29C.z - this->actor.world.pos.z) < 3.0f)) {
            this->unk_2B6 = this->actor.yawTowardsPlayer;
            this->actor.speed = 0.0f;
            if (fabsf(this->actor.world.rot.y - this->actor.yawTowardsPlayer) < 100.0f) {
                Message_CloseTextbox(play);
                this->talkState = TEXT_STATE_EVENT;
                this->textIdIndex = 7;
                EnBombers2_ChangeAnim(this, ENBOMBERS_ANIM_6, 1.0f);
                this->unk_2A8 = 0;
                this->unk_2C0 = 1;
                SET_WEEKEVENTREG(WEEKEVENTREG_73_80);
                CutsceneManager_Stop(this->csId);
                this->unk_2AC = true;
                this->actor.textId = sTextIds[this->textIdIndex];
                Message_StartTextbox(play, this->actor.textId, &this->actor);
                this->actionFunc = func_80C04D8C;
            }
        } else {
            Math_ApproachF(&this->actor.world.pos.x, this->unk_29C.x, 0.3f, 1.0f);
            Math_ApproachF(&this->actor.world.pos.z, this->unk_29C.z, 0.3f, 1.0f);
        }
    }
}

void EnBombers2_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBombers2* this = (EnBombers2*)thisx;
    Vec3f sp34;
    s32 yawDiffAbs;

    SkelAnime_Update(&this->skelAnime);
    if (this->unk_2B2 != 0) {
        this->unk_2B2--;
    }
    if ((this->animIndex == ENBOMBERS_ANIM_2) &&
        (Animation_OnFrame(&this->skelAnime, 9.0f) || Animation_OnFrame(&this->skelAnime, 10.0f) ||
         Animation_OnFrame(&this->skelAnime, 17.0f) || Animation_OnFrame(&this->skelAnime, 18.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->unk_2C0 != 2) {

        yawDiffAbs = ABS_ALT((s16)(this->actor.yawTowardsPlayer - this->actor.world.rot.y));
        this->unk_290 = 0;
        if (yawDiffAbs < 0x3A98) {
            this->unk_290 = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
            if (this->unk_290 >= 0x2711) {
                this->unk_290 = 0x2710;
            } else if (this->unk_290 < -0x2710) {
                this->unk_290 = -0x2710;
            }
        }
    }
    Actor_SetFocus(&this->actor, 20.0f);
    Actor_SetScale(&this->actor, 0.008f);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Math_SmoothStepToS(&this->unk_28A, this->unk_290, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->unk_288, this->unk_28E, 1, 0xBB8, 0);
    if (this->unk_2BE == 0) {
        this->eyeIndex += 1;
        if (this->eyeIndex >= 3) {
            this->eyeIndex = 0;
            this->unk_2BE = TRUNCF_BINANG(Rand_ZeroFloat(60.0f)) + 0x14;
        }
    }
    Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
    if (!this->unk_2AC) {
        this->actor.world.pos.x = sp34.x + Math_SinS(this->actor.world.rot.y + 0x8000) * 26.0f;
        this->actor.world.pos.z = sp34.z + Math_CosS(this->actor.world.rot.y + 0x8000) * 26.0f;
        this->collider.dim.radius = 35;
        this->collider.dim.height = 30;
    } else {
        this->collider.dim.radius = 10;
        this->collider.dim.height = 25;
    }
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    if (!this->unk_2AC) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                    UPDBGCHECKINFO_FLAG_10);
    }
    Math_Vec3f_Copy(&this->actor.world.pos, &sp34);
}

s32 EnBombers2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBombers2* this = (EnBombers2*)thisx;

    if (limbIndex == OBJECT_CS_LIMB_08) {
        rot->x += this->unk_296;
    }
    if (limbIndex == OBJECT_CS_LIMB_11) {
        rot->x += this->unk_28A;
        rot->z += this->unk_288;
    }
    if ((limbIndex == OBJECT_CS_LIMB_0F) || (limbIndex == OBJECT_CS_LIMB_13) || (limbIndex == OBJECT_CS_LIMB_14)) {
        *dList = NULL;
    }
    return false;
}

void EnBombers2_Draw(Actor* thisx, PlayState* play) {
    EnBombers2* this = (EnBombers2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(D_80C05920));
    gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(sSetPrimColorDlPtr));
    Scene_SetRenderModeXlu(play, 0, 1);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnBombers2_OverrideLimbDraw, NULL, &this->actor);
    CLOSE_DISPS(play->state.gfxCtx);
}
