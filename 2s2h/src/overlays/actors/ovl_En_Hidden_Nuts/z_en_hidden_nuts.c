/*
 * File: z_en_hidden_nuts.c
 * Overlay: ovl_En_Hidden_Nuts
 * Description: Swamp Spider House - Sleeping Deku Scrub
 */

#include "z_en_hidden_nuts.h"
#include "z64voice.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnHiddenNuts_Init(Actor* thisx, PlayState* play);
void EnHiddenNuts_Destroy(Actor* thisx, PlayState* play);
void EnHiddenNuts_Update(Actor* thisx, PlayState* play);
void EnHiddenNuts_Draw(Actor* thisx, PlayState* play);

void func_80BDB268(EnHiddenNuts* this);
void func_80BDB2B8(EnHiddenNuts* this, PlayState* play);
void func_80BDB580(EnHiddenNuts* this);
void func_80BDB59C(EnHiddenNuts* this, PlayState* play);
void func_80BDB788(EnHiddenNuts* this);
void func_80BDB7E8(EnHiddenNuts* this, PlayState* play);
void func_80BDB8F4(EnHiddenNuts* this, PlayState* play);
void func_80BDB930(EnHiddenNuts* this);
void func_80BDB978(EnHiddenNuts* this, PlayState* play);
void func_80BDBA28(EnHiddenNuts* this, PlayState* play);
void func_80BDBB48(EnHiddenNuts* this, PlayState* play);
void func_80BDBE70(EnHiddenNuts* this, PlayState* play);
void func_80BDBED4(EnHiddenNuts* this, PlayState* play);

ActorProfile En_Hidden_Nuts_Profile = {
    /**/ ACTOR_EN_HIDDEN_NUTS,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_HINTNUTS,
    /**/ sizeof(EnHiddenNuts),
    /**/ EnHiddenNuts_Init,
    /**/ EnHiddenNuts_Destroy,
    /**/ EnHiddenNuts_Update,
    /**/ EnHiddenNuts_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 30, 20, 0, { 0, 0, 0 } },
};

typedef enum EnHiddenNutsAnimation {
    /* 0 */ ENHIDDENNUTS_ANIM_0,
    /* 1 */ ENHIDDENNUTS_ANIM_1,
    /* 2 */ ENHIDDENNUTS_ANIM_2,
    /* 3 */ ENHIDDENNUTS_ANIM_3,
    /* 4 */ ENHIDDENNUTS_ANIM_4,
    /* 5 */ ENHIDDENNUTS_ANIM_5,
    /* 6 */ ENHIDDENNUTS_ANIM_6,
    /* 7 */ ENHIDDENNUTS_ANIM_7,
    /* 8 */ ENHIDDENNUTS_ANIM_8,
    /* 9 */ ENHIDDENNUTS_ANIM_MAX
} EnHiddenNutsAnimation;

static AnimationHeader* sAnimations[ENHIDDENNUTS_ANIM_MAX] = {
    &object_hintnuts_Anim_000168, // ENHIDDENNUTS_ANIM_0
    &object_hintnuts_Anim_0024CC, // ENHIDDENNUTS_ANIM_1
    &object_hintnuts_Anim_0026C4, // ENHIDDENNUTS_ANIM_2
    &object_hintnuts_Anim_002894, // ENHIDDENNUTS_ANIM_3
    &object_hintnuts_Anim_002B90, // ENHIDDENNUTS_ANIM_4
    &object_hintnuts_Anim_002F7C, // ENHIDDENNUTS_ANIM_5
    &object_hintnuts_Anim_003128, // ENHIDDENNUTS_ANIM_6
    &object_hintnuts_Anim_0029BC, // ENHIDDENNUTS_ANIM_7
    &object_hintnuts_Anim_0024CC, // ENHIDDENNUTS_ANIM_8
};

static u8 sAnimationModes[ENHIDDENNUTS_ANIM_MAX] = {
    ANIMMODE_ONCE, // ENHIDDENNUTS_ANIM_0
    ANIMMODE_ONCE, // ENHIDDENNUTS_ANIM_1
    ANIMMODE_ONCE, // ENHIDDENNUTS_ANIM_2
    ANIMMODE_LOOP, // ENHIDDENNUTS_ANIM_3
    ANIMMODE_ONCE, // ENHIDDENNUTS_ANIM_4
    ANIMMODE_LOOP, // ENHIDDENNUTS_ANIM_5
    ANIMMODE_LOOP, // ENHIDDENNUTS_ANIM_6
    ANIMMODE_ONCE, // ENHIDDENNUTS_ANIM_7
    ANIMMODE_ONCE, // ENHIDDENNUTS_ANIM_8
};

void EnHiddenNuts_Init(Actor* thisx, PlayState* play) {
    EnHiddenNuts* this = (EnHiddenNuts*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_Init(play, &this->skelAnime, &object_hintnuts_Skel_0023B8, &object_hintnuts_Anim_0024CC, this->jointTable,
                   this->morphTable, OBJECT_HINTNUTS_LIMB_MAX);
    Actor_SetScale(&this->actor, 0.01f);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.attentionRangeType = ATTENTION_RANGE_0;

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->pathIndex = ENHIDDENNUTS_GET_PATH_INDEX(&this->actor);
    this->switchFlag = ENHIDDENNUTS_GET_SWITCH_FLAG(&this->actor);

    if (this->switchFlag == ENHIDDENNUTS_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }

    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (this->pathIndex == ENHIDDENNUTS_PATH_INDEX_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    this->path = SubS_GetPathByIndex(play, this->pathIndex, ENHIDDENNUTS_PATH_INDEX_NONE_ALT);
    this->csId = this->actor.csId;
    AudioVoice_InitWord(VOICE_WORD_ID_WAKE_UP);
    func_80BDB268(this);
}

void EnHiddenNuts_Destroy(Actor* thisx, PlayState* play) {
    EnHiddenNuts* this = (EnHiddenNuts*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnHiddenNuts_ChangeAnim(EnHiddenNuts* this, s32 animIndex) {
    f32 startFrame;
    f32 morphFrames;

    this->animIndex = animIndex;
    startFrame = 0.0f;
    morphFrames = -4.0f;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);

    if (this->animIndex == ENHIDDENNUTS_ANIM_1) {
        startFrame = this->animEndFrame;
        morphFrames = 0.0f;
    }

    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, startFrame, this->animEndFrame,
                     sAnimationModes[this->animIndex], morphFrames);
}

void func_80BDB268(EnHiddenNuts* this) {
    this->actor.textId = 0x234;
    this->unk_228 = 1600.0f;
    this->actor.shape.yOffset = 1600.0f;
    EnHiddenNuts_ChangeAnim(this, ENHIDDENNUTS_ANIM_1);
    this->unk_21A = 0;
    this->actionFunc = func_80BDB2B8;
}

void func_80BDB2B8(EnHiddenNuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;

    Actor_PlaySfx(&this->actor, NA_SE_EN_NEMURI_SLEEP - SFX_FLAG);

    if (player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) {
        if (!this->unk_20A) {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
            this->unk_20A = true;
        }
    } else {
        this->unk_20A = false;
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80BDB580(this);
        return;
    }

    if (this->unk_218 == 0) {
        Vec3f sp94 = { 0.0f, 0.0f, 0.0f };
        Vec3f sp88 = { 0.0f, 0.0f, 0.0f };
        Color_RGBA8 sp84 = { 255, 255, 255, 255 };
        Color_RGBA8 sp80 = { 150, 150, 150, 0 };
        Vec3f sp74;

        for (i = 0; i < 3; i++) {
            sp94.y = Rand_ZeroFloat(1.0f) + 1.0f;

            Math_Vec3f_Copy(&sp74, &this->actor.world.pos);

            sp74.x += Rand_CenteredFloat(15.0f);
            sp74.z += Rand_CenteredFloat(15.0f);

            EffectSsDtBubble_SpawnCustomColor(play, &sp74, &sp94, &sp88, &sp84, &sp80, Rand_S16Offset(120, 90), 30,
                                              true);
        }
        this->unk_218 = 30;
    }

    if (!(this->actor.xzDistToPlayer > 120.0f)) {
        if ((play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) && (play->msgCtx.lastPlayedSong == OCARINA_SONG_SONATA)) {
            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
            func_80BDB788(this);
        } else if (AudioVoice_GetWord() == VOICE_WORD_ID_WAKE_UP) {
            func_80BDB788(this);
        } else {
            Actor_OfferTalk(&this->actor, play, BREG(13) + 100.0f);
        }
    }
}

void func_80BDB580(EnHiddenNuts* this) {
    this->unk_21A = 1;
    this->actionFunc = func_80BDB59C;
}

void func_80BDB59C(EnHiddenNuts* this, PlayState* play) {
    s32 i;

    Actor_PlaySfx(&this->actor, NA_SE_EN_NEMURI_SLEEP - SFX_FLAG);

    if (this->unk_218 == 0) {
        Vec3f sp98 = { 0.0f, 0.0f, 0.0f };
        Vec3f sp8C = { 0.0f, 0.0f, 0.0f };
        Color_RGBA8 sp88 = { 255, 255, 255, 255 };
        Color_RGBA8 sp84 = { 150, 150, 150, 0 };
        Vec3f sp78;

        for (i = 0; i < 3; i++) {
            sp98.y = Rand_ZeroFloat(1.0f) + 1.0f;

            Math_Vec3f_Copy(&sp78, &this->actor.world.pos);

            sp78.x += Rand_CenteredFloat(15.0f);
            sp78.z += Rand_CenteredFloat(15.0f);

            EffectSsDtBubble_SpawnCustomColor(play, &sp78, &sp98, &sp8C, &sp88, &sp84, Rand_S16Offset(120, 90), 30,
                                              true);
        }
        this->unk_218 = 30;
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        func_80BDB268(this);
    }
}

void func_80BDB788(EnHiddenNuts* this) {
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_UP);
    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DEAD);
    this->unk_21A = 2;
    this->actionFunc = func_80BDB7E8;
}

void func_80BDB7E8(EnHiddenNuts* this, PlayState* play) {
    Vec3f sp3C;

    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->csId);
        return;
    }

    if (!CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_Queue(this->csId);
        return;
    }

    CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
    this->unk_228 = -1200.0f;

    Math_Vec3f_Copy(&sp3C, &this->actor.world.pos);
    sp3C.y = this->actor.floorHeight + 30.0f;

    EffectSsHahen_SpawnBurst(play, &sp3C, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);

    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_UP);
    EnHiddenNuts_ChangeAnim(this, ENHIDDENNUTS_ANIM_7);
    this->actionFunc = func_80BDB8F4;
}

void func_80BDB8F4(EnHiddenNuts* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        func_80BDB930(this);
    }
}

void func_80BDB930(EnHiddenNuts* this) {
    EnHiddenNuts_ChangeAnim(this, ENHIDDENNUTS_ANIM_3);
    this->unk_21A = 3;
    this->unk_228 = 500.0f;
    this->actionFunc = func_80BDB978;
}

void func_80BDB978(EnHiddenNuts* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 8.0f) ||
        Animation_OnFrame(&this->skelAnime, 18.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_NUT_FAINT);
    }

    if (curFrame >= this->animEndFrame) {
        this->unk_224++;
        if (this->unk_224 >= 2) {
            func_80BDBA28(this, play);
        }
    }
}

void func_80BDBA28(EnHiddenNuts* this, PlayState* play) {
    Vec3f sp44;

    EnHiddenNuts_ChangeAnim(this, ENHIDDENNUTS_ANIM_2);
    this->unk_228 = 0.0f;

    Math_Vec3f_Copy(&sp44, &this->actor.world.pos);
    sp44.y = this->actor.floorHeight + 30.0f;

    EffectSsHahen_SpawnBurst(play, &sp44, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);

    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_UP);
    this->unk_208 = 1;

    if ((this->path != NULL) && !SubS_CopyPointFromPath(this->path, this->unk_208, &this->unk_20C)) {
        Actor_Kill(&this->actor);
    }

    this->actor.speed = 2.0f;
    this->actor.gravity = -2.0f;
    this->actor.velocity.y = 4.0f;
    this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_20C);
    this->unk_21A = 4;
    this->actionFunc = func_80BDBB48;
}

void func_80BDBB48(EnHiddenNuts* this, PlayState* play) {
    s32 pad[3];
    f32 curFrame = this->skelAnime.curFrame;
    WaterBox* waterBox;
    f32 sp50;
    s16 sp4E = false;
    Vec3f sp40;

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp50, &waterBox) &&
        (this->actor.world.pos.y < sp50)) {
        this->actor.velocity.y = 0.0f;
        Math_Vec3f_Copy(&sp40, &this->actor.world.pos);
        sp40.y = sp50;
        this->actor.gravity = 0.0f;

        if ((play->gameplayFrames % 8) == 0) {
            EffectSsGRipple_Spawn(play, &sp40, 650, 3150, 0);
        }

        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 5.0f) ||
            Animation_OnFrame(&this->skelAnime, 10.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_WALK_WATER);
            sp4E = true;
        }
    }

    if (sp4E && (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 5.0f) ||
                 Animation_OnFrame(&this->skelAnime, 10.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_WALK);
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_20C), 1, 0x1388, 0);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 1, 0x3E8, 0);

    if (this->animIndex == ENHIDDENNUTS_ANIM_2) {
        if (curFrame >= this->animEndFrame) {
            this->actor.speed = 0.0f;
            this->actor.velocity.y = 0.0f;
            EnHiddenNuts_ChangeAnim(this, ENHIDDENNUTS_ANIM_6);
        }
    } else {
        Math_ApproachF(&this->actor.world.pos.x, this->unk_20C.x, 0.5f,
                       fabsf(Math_SinS(this->actor.world.rot.y) * (BREG(33) + 8.0f)));
        Math_ApproachF(&this->actor.world.pos.z, this->unk_20C.z, 0.5f,
                       fabsf(Math_CosS(this->actor.world.rot.y) * (BREG(33) + 8.0f)));

        if ((sqrtf(SQ(this->actor.world.pos.x - this->unk_20C.x) + SQ(this->actor.world.pos.z - this->unk_20C.z)) <
             4.0f) &&
            (this->path != NULL)) {
            this->unk_208++;
            if (this->unk_208 >= this->path->count) {
                func_80BDBE70(this, play);
            } else if ((this->path != NULL) && !SubS_CopyPointFromPath(this->path, this->unk_208, &this->unk_20C)) {
                Actor_Kill(&this->actor);
            }
        }
    }
}

void func_80BDBE70(EnHiddenNuts* this, PlayState* play) {
    if (this->switchFlag > SWITCH_FLAG_NONE) {
        Flags_SetSwitch(play, this->switchFlag);
    }
    EnHiddenNuts_ChangeAnim(this, ENHIDDENNUTS_ANIM_8);
    this->unk_21A = 5;
    this->actionFunc = func_80BDBED4;
}

void func_80BDBED4(EnHiddenNuts* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f sp38;

    if (curFrame >= this->animEndFrame) {
        Math_Vec3f_Copy(&sp38, &this->actor.world.pos);
        sp38.y = this->actor.floorHeight;

        Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DOWN);
        EffectSsHahen_SpawnBurst(play, &sp38, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Actor_Kill(&this->actor);
    }
}

void EnHiddenNuts_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHiddenNuts* this = (EnHiddenNuts*)thisx;

    if (this->unk_218 != 0) {
        this->unk_218--;
    }

    Actor_SetFocus(&this->actor, 20.0f);
    SkelAnime_Update(&this->skelAnime);

    this->actionFunc(this, play);

    Math_ApproachF(&this->actor.shape.yOffset, this->unk_228, 0.5f, 200.0f);

    if (this->unk_21A >= 4) {
        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 40.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                    UPDBGCHECKINFO_FLAG_10);
    }

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnHiddenNuts_Draw(Actor* thisx, PlayState* play) {
    EnHiddenNuts* this = (EnHiddenNuts*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, &this->actor);
}
