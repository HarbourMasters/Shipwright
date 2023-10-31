#include "z_en_sa.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "objects/object_sa/object_sa.h"
#include "scenes/overworld/spot04/spot04_scene.h"
#include "scenes/overworld/spot05/spot05_scene.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_NO_FREEZE_OCARINA)

void EnSa_Init(Actor* thisx, PlayState* play);
void EnSa_Destroy(Actor* thisx, PlayState* play);
void EnSa_Update(Actor* thisx, PlayState* play);
void EnSa_Draw(Actor* thisx, PlayState* play);

void func_80AF6448(EnSa* this, PlayState* play);
void func_80AF67D0(EnSa* this, PlayState* play);
void func_80AF683C(EnSa* this, PlayState* play);
void func_80AF68E4(EnSa* this, PlayState* play);
void func_80AF6B20(EnSa* this, PlayState* play);

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

const ActorInit En_Sa_InitVars = {
    ACTOR_EN_SA,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SA,
    sizeof(EnSa),
    (ActorFunc)EnSa_Init,
    (ActorFunc)EnSa_Destroy,
    (ActorFunc)EnSa_Update,
    (ActorFunc)EnSa_Draw,
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
    { 20, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = {
    0, 0, 0, 0, MASS_IMMOVABLE,
};

typedef enum {
    /*  0 */ ENSA_ANIM1_0,
    /*  1 */ ENSA_ANIM1_1,
    /*  2 */ ENSA_ANIM1_2,
    /*  3 */ ENSA_ANIM1_3,
    /*  4 */ ENSA_ANIM1_4,
    /*  5 */ ENSA_ANIM1_5,
    /*  6 */ ENSA_ANIM1_6,
    /*  7 */ ENSA_ANIM1_7,
    /*  8 */ ENSA_ANIM1_8,
    /*  9 */ ENSA_ANIM1_9,
    /* 10 */ ENSA_ANIM1_10,
    /* 11 */ ENSA_ANIM1_11
} EnSaAnimation1;

static AnimationFrameCountInfo sAnimationInfo1[] = {
    { &gSariaWaitArmsToSideAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gSariaLookUpArmExtendedAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaWaveAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaRunAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaWaitArmsToSideAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaLookOverShoulderAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaPlayingOcarinaAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaStopPlayingOcarinaAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaOcarinaToMouthAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaLinkLearnedSariasSongAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaReturnToOcarinaAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaPlayingOcarinaAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
};

typedef enum {
    /* 0 */ ENSA_ANIM2_0,
    /* 1 */ ENSA_ANIM2_1,
    /* 2 */ ENSA_ANIM2_2,
    /* 3 */ ENSA_ANIM2_3,
    /* 4 */ ENSA_ANIM2_4,
    /* 5 */ ENSA_ANIM2_5,
    /* 6 */ ENSA_ANIM2_6,
    /* 7 */ ENSA_ANIM2_7,
    /* 8 */ ENSA_ANIM2_8,
    /* 9 */ ENSA_ANIM2_9
} EnSaAnimation2;

static AnimationInfo sAnimationInfo2[] = {
    { &gSariaTransitionHandsSideToChestToSideAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaTransitionHandsSideToBackAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -4.0f },
    { &gSariaRightArmExtendedWaitAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaHandsOutAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaStandHandsOnHipsAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaExtendRightArmAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaTransitionHandsSideToHipsAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaHandsBehindBackWaitAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaHandsOnFaceAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaWaitArmsToSideAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
};

s16 func_80AF5560(EnSa* this, PlayState* play) {
    s16 textState = Message_GetState(&play->msgCtx);

    if (this->unk_209 == TEXT_STATE_AWAITING_NEXT || this->unk_209 == TEXT_STATE_EVENT ||
        this->unk_209 == TEXT_STATE_CLOSING || this->unk_209 == TEXT_STATE_DONE_HAS_NEXT) {
        if (textState != this->unk_209) {
            this->unk_208++;
        }
    }
    this->unk_209 = textState;
    return textState;
}

u16 func_80AF55E0(PlayState* play, Actor* thisx) {
    EnSa* this = (EnSa*)thisx;
    u16 reaction = Text_GetFaceReaction(play, 0x10);

    if (reaction != 0) {
        return reaction;
    }
    if (IS_RANDO && Randomizer_GetSettingValue(RSK_SARIA_HINT)) {
        return 0x10AD;
    }
    if (CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
        return 0x10AD;
    }
    if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        this->unk_208 = 0;
        this->unk_209 = TEXT_STATE_NONE;
        if (Flags_GetInfTable(INFTABLE_05)) {
            return 0x1048;
        } else {
            return 0x1047;
        }
    }
    if (Flags_GetEventChkInf(EVENTCHKINF_FIRST_SPOKE_TO_MIDO)) {
        this->unk_208 = 0;
        this->unk_209 = TEXT_STATE_NONE;
        if (Flags_GetInfTable(INFTABLE_03)) {
            return 0x1032;
        } else {
            return 0x1031;
        }
    }
    if (Flags_GetInfTable(INFTABLE_GREETED_BY_SARIA)) {
        this->unk_208 = 0;
        this->unk_209 = TEXT_STATE_NONE;
        if (Flags_GetInfTable(INFTABLE_01)) {
            return 0x1003;
        } else {
            return 0x1002;
        }
    }
    return 0x1001;
}

s16 func_80AF56F4(PlayState* play, Actor* thisx) {
    s16 ret = NPC_TALK_STATE_TALKING;
    EnSa* this = (EnSa*)thisx;

    switch (func_80AF5560(this, play)) {
        case TEXT_STATE_CLOSING:
            switch (this->actor.textId) {
                case 0x1002:
                    Flags_SetInfTable(INFTABLE_01);
                    ret = NPC_TALK_STATE_IDLE;
                    break;
                case 0x1031:
                    Flags_SetEventChkInf(EVENTCHKINF_COMPLAINED_ABOUT_MIDO);
                    Flags_SetInfTable(INFTABLE_03);
                    ret = NPC_TALK_STATE_IDLE;
                    break;
                case 0x1047:
                    Flags_SetInfTable(INFTABLE_05);
                    ret = NPC_TALK_STATE_IDLE;
                    break;
                default:
                    ret = NPC_TALK_STATE_IDLE;
                    break;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return ret;
}

void func_80AF57D8(EnSa* this, PlayState* play) {
    if (play->sceneNum != SCENE_SACRED_FOREST_MEADOW ||
        ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) < 0x1555 || this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        Npc_UpdateTalking(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 30.0f,
                          func_80AF55E0, func_80AF56F4);
    }
}

f32 func_80AF5894(EnSa* this) {
    f32 endFrame = this->skelAnime.endFrame;
    f32 startFrame = this->skelAnime.startFrame;

    this->skelAnime.startFrame = endFrame;
    this->skelAnime.curFrame = endFrame;
    this->skelAnime.endFrame = startFrame;
    this->skelAnime.playSpeed = -1.0f;
    return startFrame;
}

void func_80AF58B8(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_3);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_2);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF594C(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_8);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_9);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF59E0(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_1);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_7);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF5A74(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_1);
            func_80AF5894(this);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_9);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF5B10(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_6);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_4);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF5BA4(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_6);
            func_80AF5894(this);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_9);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF5C40(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_5);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_0);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF5CD4(EnSa* this, u8 arg1) {
    this->unk_20B = arg1;
    this->unk_20A = 0;
}

void func_80AF5CE4(EnSa* this) {
    switch (this->unk_20B) {
        case 1:
            func_80AF58B8(this);
            break;
        case 2:
            func_80AF594C(this);
            break;
        case 3:
            func_80AF59E0(this);
            break;
        case 4:
            func_80AF5A74(this);
            break;
        case 5:
            func_80AF5B10(this);
            break;
        case 6:
            func_80AF5BA4(this);
            break;
        case 7:
            func_80AF5C40(this);
            break;
    }
}

void EnSa_ChangeAnim(EnSa* this, s32 index) {
    Animation_Change(&this->skelAnime, sAnimationInfo1[index].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo1[index].animation), sAnimationInfo1[index].mode,
                     sAnimationInfo1[index].morphFrames);
}

s32 func_80AF5DFC(EnSa* this, PlayState* play) {
    if (gSaveContext.cutsceneIndex >= 0xFFF0 && gSaveContext.cutsceneIndex != 0xFFFD) {
        if (play->sceneNum == SCENE_KOKIRI_FOREST) {
            return 4;
        }
        if (play->sceneNum == SCENE_SACRED_FOREST_MEADOW) {
            return 5;
        }
    }
    if (play->sceneNum == SCENE_SARIAS_HOUSE && !LINK_IS_ADULT &&
        INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY && !Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_ZELDAS_LETTER)) {
        return 1;
    }
    if (play->sceneNum == SCENE_SACRED_FOREST_MEADOW && (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_ZELDAS_LETTER))) {
        if (IS_RANDO) {
            return 5;
        }
        return CHECK_QUEST_ITEM(QUEST_SONG_SARIA) ? 2 : 5;
    }
    if (play->sceneNum == SCENE_KOKIRI_FOREST && !CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        if (Flags_GetInfTable(INFTABLE_GREETED_BY_SARIA)) {
            return 1;
        }
        return 4;
    }
    return 0;
}

void func_80AF5F34(EnSa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode = NPC_TRACKING_PLAYER_AUTO_TURN;

    if (play->sceneNum == SCENE_KOKIRI_FOREST) {
        trackingMode = (this->actionFunc == func_80AF68E4) ? NPC_TRACKING_NONE : NPC_TRACKING_FULL_BODY;
    }
    if (play->sceneNum == SCENE_SACRED_FOREST_MEADOW) {
        trackingMode = (this->skelAnime.animation == &gSariaPlayingOcarinaAnim) ? NPC_TRACKING_NONE : NPC_TRACKING_HEAD;
    }
    if (play->sceneNum == SCENE_SACRED_FOREST_MEADOW && this->actionFunc == func_80AF6448 &&
        this->skelAnime.animation == &gSariaStopPlayingOcarinaAnim) {
        trackingMode = NPC_TRACKING_NONE;
    }
    if (play->sceneNum == SCENE_SACRED_FOREST_MEADOW && this->actionFunc == func_80AF68E4 &&
        this->skelAnime.animation == &gSariaOcarinaToMouthAnim) {
        trackingMode = NPC_TRACKING_NONE;
    }
    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = 4.0f;
    Npc_TrackPoint(&this->actor, &this->interactInfo, 2, trackingMode);
}

s32 func_80AF603C(EnSa* this) {
    if (this->skelAnime.animation != &gSariaPlayingOcarinaAnim &&
        this->skelAnime.animation != &gSariaOcarinaToMouthAnim) {
        return 0;
    }
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        return 0;
    }
    this->unk_20E = 0;
    if (this->rightEyeIndex != SARIA_EYE_CLOSED) {
        return 0;
    }
    return 1;
}

void func_80AF609C(EnSa* this) {
    s16 phi_v1;

    if (func_80AF603C(this) == 0) {
        if (this->unk_20E == 0) {
            phi_v1 = 0;
        } else {
            this->unk_20E--;
            phi_v1 = this->unk_20E;
        }
        if (phi_v1 == 0) {
            this->rightEyeIndex++;
            if (this->rightEyeIndex < SARIA_EYE_SUPRISED) {
                this->leftEyeIndex = this->rightEyeIndex;
            } else {
                this->unk_20E = Rand_S16Offset(30, 30);
                this->leftEyeIndex = SARIA_EYE_OPEN;
                this->rightEyeIndex = this->leftEyeIndex;
            }
        }
    }
}

void func_80AF6130(CsCmdActorAction* csAction, Vec3f* dst) {
    dst->x = csAction->startPos.x;
    dst->y = csAction->startPos.y;
    dst->z = csAction->startPos.z;
}

void func_80AF6170(CsCmdActorAction* csAction, Vec3f* dst) {
    dst->x = csAction->endPos.x;
    dst->y = csAction->endPos.y;
    dst->z = csAction->endPos.z;
}

void EnSa_Init(Actor* thisx, PlayState* play) {
    EnSa* this = (EnSa*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 12.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gSariaSkel, NULL, this->jointTable, this->morphTable, 17);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &sColChkInfoInit);

    switch (func_80AF5DFC(this, play)) {
        case 2:
            EnSa_ChangeAnim(this, ENSA_ANIM1_11);
            this->actionFunc = func_80AF6448;
            break;
        case 5:
            EnSa_ChangeAnim(this, ENSA_ANIM1_11);
            this->actionFunc = func_80AF683C;
            break;
        case 1:
            this->actor.gravity = -1.0f;
            EnSa_ChangeAnim(this, ENSA_ANIM1_0);
            this->actionFunc = func_80AF6448;
            break;
        case 4:
            this->unk_210 = 0;
            this->actor.gravity = -1.0f;
            play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gSpot04Cs_10E20);
            gSaveContext.cutsceneTrigger = 1;
            EnSa_ChangeAnim(this, ENSA_ANIM1_4);
            this->actionFunc = func_80AF68E4;
            break;
        case 3:
            this->unk_210 = 0;
            this->actor.gravity = -1.0f;
            EnSa_ChangeAnim(this, ENSA_ANIM1_0);
            this->actionFunc = func_80AF68E4;
            break;
        case 0:
            Actor_Kill(&this->actor);
            return;
    }

    Actor_SetScale(&this->actor, 0.01f);

    this->actor.targetMode = 6;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    this->alpha = 255;
    this->unk_21A = this->actor.shape.rot;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, FAIRY_KOKIRI);
}

void EnSa_Destroy(Actor* thisx, PlayState* play) {
    EnSa* this = (EnSa*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void func_80AF6448(EnSa* this, PlayState* play) {
    if (play->sceneNum == SCENE_KOKIRI_FOREST) {
        if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
            switch (this->actor.textId) {
                case 0x1002:
                    if (this->unk_208 == 0 && this->unk_20B != 1) {
                        func_80AF5CD4(this, 1);
                        this->mouthIndex = 1;
                    }
                    if (this->unk_208 == 2 && this->unk_20B != 2) {
                        func_80AF5CD4(this, 2);
                        this->mouthIndex = 1;
                    }
                    if (this->unk_208 == 5) {
                        this->mouthIndex = 0;
                    }
                    break;
                case 0x1003:
                    if (this->unk_208 == 0 && this->unk_20B != 4) {
                        func_80AF5CD4(this, 4);
                    }
                    break;
                case 0x1031:
                    if (this->unk_208 == 0 && this->unk_20B != 4 &&
                        this->skelAnime.animation == &gSariaHandsBehindBackWaitAnim) {
                        func_80AF5CD4(this, 4);
                        this->mouthIndex = 3;
                    }
                    if (this->unk_208 == 2 && this->unk_20B != 5) {
                        func_80AF5CD4(this, 5);
                        this->mouthIndex = 2;
                    }
                    if (this->unk_208 == 4 && this->unk_20B != 6) {
                        func_80AF5CD4(this, 6);
                        this->mouthIndex = 0;
                    }
                    break;
                case 0x1032:
                    if (this->unk_208 == 0 && this->unk_20B != 4 &&
                        this->skelAnime.animation == &gSariaHandsBehindBackWaitAnim) {
                        func_80AF5CD4(this, 4);
                    }
                    break;
                case 0x1047:
                    if (this->unk_208 == 1 && this->unk_20B != 7) {
                        func_80AF5CD4(this, 7);
                    }
                    break;
                case 0x1048:
                    if (this->unk_208 == 0 && this->unk_20B != 7) {
                        func_80AF5CD4(this, 7);
                    }
                    break;
            }
        } else if (!CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) &&
                   ((Flags_GetInfTable(INFTABLE_01)) || (Flags_GetInfTable(INFTABLE_03)))) {
            if (this->unk_20B != 3) {
                func_80AF5CD4(this, 3);
            }
        } else {
            func_80AF5CD4(this, 0);
        }
        func_80AF5CE4(this);
    }
    if (this->skelAnime.animation == &gSariaStopPlayingOcarinaAnim) {
        this->skelAnime.playSpeed = -1.0f;
        if ((s32)this->skelAnime.curFrame == 0) {
            EnSa_ChangeAnim(this, ENSA_ANIM1_6);
        }
    }
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE && play->sceneNum == SCENE_SACRED_FOREST_MEADOW) {
        Animation_Change(&this->skelAnime, &gSariaStopPlayingOcarinaAnim, 1.0f, 0.0f, 10.0f, ANIMMODE_ONCE, -10.0f);
        this->actionFunc = func_80AF67D0;
    }
}

void func_80AF67D0(EnSa* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        Animation_Change(&this->skelAnime, &gSariaStopPlayingOcarinaAnim, 0.0f, 10.0f, 0.0f, ANIMMODE_ONCE, -10.0f);
        this->actionFunc = func_80AF6448;
    }
}

void GivePlayerRandoRewardSaria(EnSa* saria, PlayState* play, RandomizerCheck check) {
    GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(check, RG_SARIAS_SONG);
    if (saria->actor.parent != NULL && saria->actor.parent->id == GET_PLAYER(play)->actor.id &&
        !Flags_GetTreasure(play, 0x1F)) {
        Flags_SetTreasure(play, 0x1F);
    } else if (!Flags_GetTreasure(play, 0x1F)) {
        GiveItemEntryFromActor(&saria->actor, play, getItemEntry, 10000.0f, 100.0f);
    }
}

void func_80AF683C(EnSa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!(player->actor.world.pos.z >= -2220.0f) && !Play_InCsMode(play)) {
        if (IS_RANDO) {
            GivePlayerRandoRewardSaria(this, play, RC_SONG_FROM_SARIA);
            return;
        }

        play->csCtx.segment = SEGMENTED_TO_VIRTUAL(spot05_scene_Cs_005730);
        gSaveContext.cutsceneTrigger = 1;
        this->actionFunc = func_80AF68E4;
    }
}

void func_80AF68E4(EnSa* this, PlayState* play) {
    s16 phi_v0;
    Vec3f startPos;
    Vec3f endPos;
    Vec3f D_80AF7448 = { 0.0f, 0.0f, 0.0f };
    CsCmdActorAction* csAction;
    f32 temp_f0;
    f32 gravity;

    if ((gSaveContext.cutsceneTrigger != 1) && (play->csCtx.state == CS_STATE_IDLE)) {
        this->actionFunc = func_80AF6B20;
        return;
    }
    csAction = play->csCtx.npcActions[1];
    if (csAction != NULL) {
        func_80AF6130(csAction, &startPos);
        func_80AF6170(csAction, &endPos);

        if (this->unk_210 == 0) {
            this->actor.world.pos = startPos;
        }
        if (this->unk_210 != csAction->action) {
            switch (csAction->action) {
                case 2:
                    this->mouthIndex = 1;
                    break;
                case 9:
                    this->mouthIndex = 1;
                    break;
                default:
                    this->mouthIndex = 0;
                    break;
            }
            EnSa_ChangeAnim(this, csAction->action);
            this->unk_210 = csAction->action;
        }
        //if (phi_v0) {}
        if (csAction->action == 3) {
            if (this->unk_20C == 0) {
                phi_v0 = 0;
            } else {
                this->unk_20C--;
                phi_v0 = this->unk_20C;
            }
            if (phi_v0 == 0) {
                Audio_PlaySoundGeneral(NA_SE_PL_WALK_GROUND, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
                this->unk_20C = 8;
            }
        }
        this->actor.shape.rot.x = csAction->urot.x;
        this->actor.shape.rot.y = csAction->urot.y;
        this->actor.shape.rot.z = csAction->urot.z;
        this->actor.velocity = D_80AF7448;

        if (play->csCtx.frames < csAction->endFrame) {
            temp_f0 = csAction->endFrame - csAction->startFrame;
            this->actor.velocity.x = (endPos.x - startPos.x) / temp_f0;
            this->actor.velocity.y = (endPos.y - startPos.y) / temp_f0;
            gravity = this->actor.gravity;
            if (play->sceneNum == SCENE_SACRED_FOREST_MEADOW) {
                gravity = 0.0f;
            }
            this->actor.velocity.y += gravity;
            if (this->actor.velocity.y < this->actor.minVelocityY) {
                this->actor.velocity.y = this->actor.minVelocityY;
            }
            this->actor.velocity.z = (endPos.z - startPos.z) / temp_f0;
        }
    }
}

void func_80AF6B20(EnSa* this, PlayState* play) {
    if (play->sceneNum == SCENE_SACRED_FOREST_MEADOW) {
        Item_Give(play, ITEM_SONG_SARIA);
        EnSa_ChangeAnim(this, ENSA_ANIM1_6);
    }

    if (play->sceneNum == SCENE_KOKIRI_FOREST) {
        EnSa_ChangeAnim(this, ENSA_ANIM1_4);
        this->actor.world.pos = this->actor.home.pos;
        this->actor.world.rot = this->unk_21A;
        this->mouthIndex = 0;
        Flags_SetInfTable(INFTABLE_GREETED_BY_SARIA);
    }

    this->actionFunc = func_80AF6448;
}

void EnSa_Update(Actor* thisx, PlayState* play) {
    EnSa* this = (EnSa*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    SkelAnime_Update(&this->skelAnime);

    if (this->skelAnime.animation == &gSariaOcarinaToMouthAnim &&
        this->skelAnime.curFrame >= Animation_GetLastFrame(&gSariaOcarinaToMouthAnim)) {
        EnSa_ChangeAnim(this, ENSA_ANIM1_6);
    }

    if (this->actionFunc != func_80AF68E4) {
        if (CVarGetInteger("gDisableKokiriDrawDistance", 0) != 0) {
            this->alpha = func_80034DD4(&this->actor, play, this->alpha, 32767);
        }
        else {
            this->alpha = func_80034DD4(&this->actor, play, this->alpha, 400.0f);
        }
    } else {
        this->alpha = 255;
    }

    this->actor.shape.shadowAlpha = this->alpha;

    if (this->actionFunc == func_80AF68E4) {
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;
    } else {
        func_8002D7EC(&this->actor);
    }

    if (play->sceneNum != SCENE_SACRED_FOREST_MEADOW) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    }

    func_80AF609C(this);
    this->actionFunc(this, play);
    func_80AF57D8(this, play);
    func_80AF5F34(this, play);
}

s32 EnSa_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnSa* this = (EnSa*)thisx;
    s32 pad;
    Vec3s sp18;

    if (limbIndex == 16) {
        Matrix_Translate(900.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        sp18 = this->interactInfo.headRot;
        Matrix_RotateX(BINANG_TO_RAD(sp18.y), MTXMODE_APPLY);
        Matrix_RotateZ(BINANG_TO_RAD(sp18.x), MTXMODE_APPLY);
        Matrix_Translate(-900.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == 9) {
        sp18 = this->interactInfo.torsoRot;
        Matrix_RotateY(BINANG_TO_RAD(sp18.y), MTXMODE_APPLY);
        Matrix_RotateX(BINANG_TO_RAD(sp18.x), MTXMODE_APPLY);
    }

    if (play->sceneNum == SCENE_SACRED_FOREST_MEADOW && limbIndex == 15) {
        *dList = gSariaRightHandAndOcarinaDL;
    }

    return 0;
}

void EnSa_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnSa* this = (EnSa*)thisx;
    Vec3f D_80AF7454 = { 400.0, 0.0f, 0.0f };

    if (limbIndex == 16) {
        Matrix_MultVec3f(&D_80AF7454, &this->actor.focus.pos);
    }
}

void EnSa_Draw(Actor* thisx, PlayState* play) {
    static void* mouthTextures[] = {
        gSariaMouthClosed2Tex,  gSariaMouthSmilingOpenTex, gSariaMouthFrowningTex,
        gSariaMouthSuprisedTex, gSariaMouthClosedTex,
    };
    static void* eyeTextures[] = {
        gSariaEyeOpenTex, gSariaEyeHalfTex, gSariaEyeClosedTex, gSariaEyeSuprisedTex, gSariaEyeSadTex,
    };
    EnSa* this = (EnSa*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->alpha == 255) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->rightEyeIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTextures[this->leftEyeIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTextures[this->mouthIndex]));
        func_80034BA0(play, &this->skelAnime, EnSa_OverrideLimbDraw, EnSa_PostLimbDraw, &this->actor, this->alpha);
    } else if (this->alpha != 0) {
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->rightEyeIndex]));
        gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTextures[this->leftEyeIndex]));
        gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTextures[this->mouthIndex]));
        func_80034CC4(play, &this->skelAnime, EnSa_OverrideLimbDraw, EnSa_PostLimbDraw, &this->actor, this->alpha);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
