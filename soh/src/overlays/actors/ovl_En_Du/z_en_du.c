#include "z_en_du.h"
#include "objects/object_du/object_du.h"
#include "scenes/overworld/spot18/spot18_scene.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_NO_FREEZE_OCARINA)

void EnDu_Init(Actor* thisx, PlayState* play);
void EnDu_Destroy(Actor* thisx, PlayState* play);
void EnDu_Update(Actor* thisx, PlayState* play);
void EnDu_Draw(Actor* thisx, PlayState* play);

void func_809FE3B4(EnDu* this, PlayState* play);
void func_809FE3C0(EnDu* this, PlayState* play);
void func_809FE638(EnDu* this, PlayState* play);
void func_809FE890(EnDu* this, PlayState* play);
void func_809FE4A4(EnDu* this, PlayState* play);
void func_809FE6CC(EnDu* this, PlayState* play);
void func_809FE740(EnDu* this, PlayState* play);
void func_809FE798(EnDu* this, PlayState* play);
void func_809FEC14(EnDu* this, PlayState* play);
void func_809FEC70(EnDu* this, PlayState* play);
void func_809FECE4(EnDu* this, PlayState* play);
void func_809FEB08(EnDu* this, PlayState* play);

const ActorInit En_Du_InitVars = {
    ACTOR_EN_DU,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DU,
    sizeof(EnDu),
    (ActorFunc)EnDu_Init,
    (ActorFunc)EnDu_Destroy,
    (ActorFunc)EnDu_Update,
    (ActorFunc)EnDu_Draw,
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
    /*  0 */ ENDU_ANIM_0,
    /*  1 */ ENDU_ANIM_1,
    /*  2 */ ENDU_ANIM_2,
    /*  3 */ ENDU_ANIM_3,
    /*  4 */ ENDU_ANIM_4,
    /*  5 */ ENDU_ANIM_5,
    /*  6 */ ENDU_ANIM_6,
    /*  7 */ ENDU_ANIM_7,
    /*  8 */ ENDU_ANIM_8,
    /*  9 */ ENDU_ANIM_9,
    /* 10 */ ENDU_ANIM_10,
    /* 11 */ ENDU_ANIM_11,
    /* 12 */ ENDU_ANIM_12,
    /* 13 */ ENDU_ANIM_13,
    /* 14 */ ENDU_ANIM_14
} EnDuAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gDaruniaIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gDaruniaIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaItemGiveAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaItemGiveIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaHitLinkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaHitBreastAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaStandUpAfterFallingAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaDancingLoop1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -10.0f },
    { &gDaruniaDancingLoop1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaDancingLoop2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaDancingLoop3Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaWrongSongAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaWrongSongEndAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gDaruniaDancingLoop4Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaDancingEndAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -6.0f },
};

void EnDu_SetupAction(EnDu* this, EnDuActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

u16 func_809FDC38(PlayState* play, Actor* actor) {
    u16 reaction = Text_GetFaceReaction(play, 0x21);

    if (reaction != 0) {
        return reaction;
    }
    if (CUR_UPG_VALUE(UPG_STRENGTH) != 0) {
        if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
            return 0x301E;
        } else {
            return 0x301D;
        }
    }
    if (Flags_GetInfTable(INFTABLE_113)) {
        return 0x301B;
    } else {
        return 0x301A;
    }
}

s16 func_809FDCDC(PlayState* play, Actor* actor) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
            break;
        case TEXT_STATE_CLOSING:
            switch (actor->textId) {
                case 0x301A:
                    Flags_SetInfTable(INFTABLE_113);
                    break;
                case 0x301C:
                case 0x301F:
                    return NPC_TALK_STATE_ACTION;
                case 0x3020:
                    Flags_SetEventChkInf(EVENTCHKINF_22);
                    break;
            }
            return 0;
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                return NPC_TALK_STATE_ITEM_GIVEN;
            }
            break;
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return NPC_TALK_STATE_TALKING;
}

s32 func_809FDDB4(EnDu* this, PlayState* play) {
    if (play->sceneNum == SCENE_GORON_CITY && LINK_IS_CHILD) {
        return 1;
    } else if (play->sceneNum == SCENE_FIRE_TEMPLE && !Flags_GetInfTable(INFTABLE_SPOKE_TO_DARUNIA_IN_FIRE_TEMPLE) && LINK_IS_ADULT) {
        return 1;
    }
    return 0;
}

void func_809FDE24(EnDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode = NPC_TRACKING_PLAYER_AUTO_TURN;

    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        trackingMode = NPC_TRACKING_NONE;
    }
    if (this->actionFunc == func_809FE890) {
        trackingMode = NPC_TRACKING_NONE;
    }
    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = 10.0f;
    Npc_TrackPoint(&this->actor, &this->interactInfo, 3, trackingMode);
}

void func_809FDE9C(EnDu* this) {
    if (this->blinkTimer > 0) {
        this->blinkTimer--;
    } else {
        this->blinkTimer = 0;
    }
    if (this->blinkTimer < 3) {
        this->eyeTexIndex = this->blinkTimer;
    }

    switch (this->unk_1EC) {
        case 0:
            if (this->blinkTimer == 0) {
                this->blinkTimer = Rand_S16Offset(30, 30);
            }
            break;
        case 1:
            if (this->blinkTimer == 0) {
                this->eyeTexIndex = 2;
            }
            break;
        case 2:
            if (this->blinkTimer == 0) {
                this->eyeTexIndex = 2;
            }
            break;
        case 3:
            if (this->blinkTimer == 0) {
                this->eyeTexIndex = 0;
            }
            break;
    }

    switch (this->unk_1ED) {
        case 1:
            this->mouthTexIndex = 1;
            break;
        case 2:
            this->mouthTexIndex = 2;
            break;
        case 3:
            this->mouthTexIndex = 3;
            break;
        default:
            this->mouthTexIndex = 0;
            break;
    }

    if (this->unk_1EE == 1) {
        this->noseTexIndex = 1;
    } else {
        this->noseTexIndex = 0;
    }
}

void func_809FDFC0(CsCmdActorAction* csAction, Vec3f* dst) {
    dst->x = csAction->startPos.x;
    dst->y = csAction->startPos.y;
    dst->z = csAction->startPos.z;
}

void func_809FE000(CsCmdActorAction* csAction, Vec3f* dst) {
    dst->x = csAction->endPos.x;
    dst->y = csAction->endPos.y;
    dst->z = csAction->endPos.z;
}

void func_809FE040(EnDu* this) {
    s32 animationIndices[] = {
        ENDU_ANIM_8, ENDU_ANIM_8, ENDU_ANIM_8, ENDU_ANIM_8, ENDU_ANIM_9, ENDU_ANIM_10, ENDU_ANIM_10, ENDU_ANIM_13,
    };

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->unk_1E6++;
        if (this->unk_1E6 >= 8) {
            this->unk_1E6 = 0;
        }
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, animationIndices[this->unk_1E6]);
    }
}

void func_809FE104(EnDu* this) {
    s32 animationIndices[] = {
        ENDU_ANIM_8,
        ENDU_ANIM_8,
        ENDU_ANIM_11,
        ENDU_ANIM_12,
    };

    if (this->unk_1E6 < 4) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->unk_1E6++;
            if (this->unk_1E6 < 4) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, animationIndices[this->unk_1E6]);
            }
        }
    }
}

void EnDu_Init(Actor* thisx, PlayState* play) {
    EnDu* this = (EnDu*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gDaruniaSkel, NULL, 0, 0, 0);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    if (func_809FDDB4(this, play) == 0) {
        Actor_Kill(&this->actor);
        return;
    }
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENDU_ANIM_0);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.targetMode = 1;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;

    if (gSaveContext.cutsceneIndex >= 0xFFF0) {
        play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGoronCityDarunia01Cs);
        gSaveContext.cutsceneTrigger = 1;
        EnDu_SetupAction(this, func_809FE890);
    } else if (play->sceneNum == SCENE_FIRE_TEMPLE) {
        EnDu_SetupAction(this, func_809FE638);
    } else if (!LINK_IS_ADULT) {
        EnDu_SetupAction(this, func_809FE3C0);
    } else {
        EnDu_SetupAction(this, func_809FE3B4);
    }
}

void EnDu_Destroy(Actor* thisx, PlayState* play) {
    EnDu* this = (EnDu*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyCylinder(play, &this->collider);
}

void func_809FE3B4(EnDu* this, PlayState* play) {
}

void func_809FE3C0(EnDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & 0x1000000) {
        func_8010BD88(play, OCARINA_ACTION_CHECK_SARIA);
        player->stateFlags2 |= 0x2000000;
        player->unk_6A8 = &this->actor;
        EnDu_SetupAction(this, func_809FE4A4);
        return;
    }
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        func_8002DF54(play, &this->actor, 7);
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    }
    if (this->actor.xzDistToPlayer < 116.0f + this->collider.dim.radius) {
        player->stateFlags2 |= 0x800000;
    }
}

void func_809FE4A4(EnDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_00;
        EnDu_SetupAction(this, func_809FE3C0);
    } else if (play->msgCtx.ocarinaMode >= OCARINA_MODE_06) {
        if (!IS_RANDO) {
            play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGoronCityDaruniaWrongCs);
            gSaveContext.cutsceneTrigger = 1;
        }
        this->unk_1E8 = 1;
        EnDu_SetupAction(this, func_809FE890);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        if (!IS_RANDO) {
            play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGoronCityDaruniaCorrectCs);
            gSaveContext.cutsceneTrigger = 1;
        }
        this->unk_1E8 = 0;
        EnDu_SetupAction(this, func_809FE890);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else {
        player->stateFlags2 |= 0x800000;
    }
}

void func_809FE638(EnDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & 0x20000000)) {
        OnePointCutscene_Init(play, 3330, -99, &this->actor, MAIN_CAM);
        player->actor.shape.rot.y = player->actor.world.rot.y = this->actor.world.rot.y + 0x7FFF;
        Audio_PlayFanfare(NA_BGM_APPEAR);
        EnDu_SetupAction(this, func_809FE6CC);
        this->unk_1E2 = 0x32;
    }
}

void func_809FE6CC(EnDu* this, PlayState* play) {
    s16 phi_v1;

    if (this->unk_1E2 == 0) {
        phi_v1 = 0;
    } else {
        this->unk_1E2--;
        phi_v1 = this->unk_1E2;
    }
    if (phi_v1 == 0) {
        this->actor.textId = 0x3039;
        Message_StartTextbox(play, this->actor.textId, NULL);
        this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
        EnDu_SetupAction(this, func_809FE740);
    }
}

void func_809FE740(EnDu* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        func_8005B1A4(GET_ACTIVE_CAM(play));
        this->unk_1E2 = 0x5A;
        EnDu_SetupAction(this, func_809FE798);
    }
}

void func_809FE798(EnDu* this, PlayState* play) {
    s32 phi_v0;

    if (this->unk_1E2 == 0) {
        phi_v0 = 0;
    } else {
        this->unk_1E2--;
        phi_v0 = this->unk_1E2;
    }
    if (phi_v0 != 0) {
        switch (this->unk_1E2) {
            case 0x50:
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHAIN_KEY_UNLOCK_B);
                break;
            case 0x3C:
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_SLIDE_DOOR_OPEN);
                break;
            case 0xF:
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_SLIDE_DOOR_CLOSE);
                break;
            case 5:
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_STONE_BOUND);
                break;
        }
        if (this->unk_1E2 >= 0x3D) {
            this->actor.world.pos.x -= 10.0f;
        }
    } else {
        Actor_Kill(&this->actor);
        Flags_SetInfTable(INFTABLE_SPOKE_TO_DARUNIA_IN_FIRE_TEMPLE);
    }
}

void func_809FE890(EnDu* this, PlayState* play) {
    f32 frame;
    Vec3f startPos;
    Vec3f endPos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    CsCmdActorAction* csAction;

    if (play->csCtx.state == CS_STATE_IDLE || IS_RANDO) {
        if (IS_RANDO) {
            play->csCtx.state = CS_STATE_IDLE;
        }
        func_8002DF54(play, &this->actor, 1);
        EnDu_SetupAction(this, func_809FEB08);
        return;
    }
    csAction = play->csCtx.npcActions[2];

    if (csAction != NULL) {
        func_809FDFC0(csAction, &startPos);
        func_809FE000(csAction, &endPos);
        if (this->unk_1EA == 0) {
            func_809FDFC0(csAction, &startPos);
            this->actor.world.pos = startPos;
        }
        if (this->unk_1EA != csAction->action) {
            if (csAction->action == 1) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENDU_ANIM_1);
            }
            if (csAction->action == 7 || csAction->action == 8) {
                this->unk_1E6 = 0;
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENDU_ANIM_7);
            }
            this->unk_1EA = csAction->action;
            if (this->unk_1EA == 7) {
                this->blinkTimer = 11;
                this->unk_1EC = 2;
                this->unk_1ED = 2;
                this->unk_1EE = 1;
            }
            if (this->unk_1EA == 8) {
                this->blinkTimer = 11;
                this->unk_1EC = 3;
                this->unk_1ED = 3;
                this->unk_1EE = 0;
            }
        }
        if (this->unk_1EA == 7) {
            func_809FE040(this);
        }
        if (this->unk_1EA == 8) {
            func_809FE104(this);
        }
        this->actor.shape.rot.x = csAction->urot.x;
        this->actor.shape.rot.y = csAction->urot.y;
        this->actor.shape.rot.z = csAction->urot.z;
        this->actor.velocity = velocity;

        if (play->csCtx.frames < csAction->endFrame) {
            frame = csAction->endFrame - csAction->startFrame;

            this->actor.velocity.x = (endPos.x - startPos.x) / frame;
            this->actor.velocity.y = (endPos.y - startPos.y) / frame;
            this->actor.velocity.y += this->actor.gravity;
            if (this->actor.velocity.y < this->actor.minVelocityY) {
                this->actor.velocity.y = this->actor.minVelocityY;
            }
            this->actor.velocity.z = (endPos.z - startPos.z) / frame;
        }
    }
}

void func_809FEB08(EnDu* this, PlayState* play) {
    this->blinkTimer = 11;
    this->unk_1EC = 0;
    this->unk_1ED = 0;
    this->unk_1EE = 0;

    if (this->unk_1E8 == 1) {
        func_8002DF54(play, &this->actor, 7);
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENDU_ANIM_1);
        EnDu_SetupAction(this, func_809FE3C0);
        return;
    }
    if ((!IS_RANDO && CUR_UPG_VALUE(UPG_STRENGTH) <= 0) ||
         (IS_RANDO && !Flags_GetTreasure(play, 0x1E))) {
        if (IS_RANDO) {
            Flags_SetTreasure(play, 0x1E);
        }
        this->actor.textId = 0x301C;
        EnDu_SetupAction(this, func_809FEC14);
    } else {
        this->actor.textId = 0x301F;
        EnDu_SetupAction(this, func_809FE3C0);
    }
    Message_StartTextbox(play, this->actor.textId, NULL);
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENDU_ANIM_14);
    this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
}

void func_809FEC14(EnDu* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        func_8002DF54(play, &this->actor, 7);
        EnDu_SetupAction(this, func_809FEC70);
        func_809FEC70(this, play);
    }
}

void func_809FEC70(EnDu* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        EnDu_SetupAction(this, func_809FECE4);
    } else {
        f32 xzRange = this->actor.xzDistToPlayer + 1.0f;
        if (!IS_RANDO) {
            func_8002F434(&this->actor, play, GI_BRACELET, xzRange, fabsf(this->actor.yDistToPlayer) + 1.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_GC_DARUNIAS_JOY, GI_BRACELET);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, xzRange, fabsf(this->actor.yDistToPlayer) + 1.0f);
        }
    }
}

void func_809FECE4(EnDu* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ITEM_GIVEN) {
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        EnDu_SetupAction(this, func_809FE3C0);
    }
}

void EnDu_Update(Actor* thisx, PlayState* play) {
    EnDu* this = (EnDu*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    if (this->skelAnime.animation == &gDaruniaDancingEndAnim &&
        Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENDU_ANIM_1);
    }

    SkelAnime_Update(&this->skelAnime);
    func_809FDE9C(this);
    func_809FDE24(this, play);

    if (this->actionFunc == func_809FE890) {
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;
    } else {
        func_8002D7EC(&this->actor);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);

    if (this->actionFunc != func_809FE4A4) {
        Npc_UpdateTalking(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 116.0f,
                          func_809FDC38, func_809FDCDC);
    }
    this->actionFunc(this, play);
}

s32 EnDu_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnDu* this = (EnDu*)thisx;
    Vec3s sp1C;

    if (limbIndex == 16) {
        Matrix_Translate(2400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        sp1C = this->interactInfo.headRot;
        Matrix_RotateX(BINANG_TO_RAD(sp1C.y), MTXMODE_APPLY);
        Matrix_RotateZ(BINANG_TO_RAD(sp1C.x), MTXMODE_APPLY);
        Matrix_Translate(-2400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == 8) {
        sp1C = this->interactInfo.torsoRot;
        Matrix_RotateY(BINANG_TO_RAD(sp1C.y), MTXMODE_APPLY);
        Matrix_RotateX(BINANG_TO_RAD(sp1C.x), MTXMODE_APPLY);
    }
    return 0;
}

void EnDu_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnDu* this = (EnDu*)thisx;
    Vec3f D_809FF40C = { 0.0f, -1000.0f, 0.0f };

    if (limbIndex == 16) {
        Matrix_MultVec3f(&D_809FF40C, &this->actor.focus.pos);
    }
}

void EnDu_Draw(Actor* thisx, PlayState* play) {
    static void* eyeTextures[] = {
        gDaruniaEyeOpenTex,
        gDaruniaEyeOpeningTex,
        gDaruniaEyeShutTex,
        gDaruniaEyeClosingTex,
    };
    static void* mouthTextures[] = {
        gDaruniaMouthSeriousTex,
        gDaruniaMouthGrinningTex,
        gDaruniaMouthOpenTex,
        gDaruniaMouthHappyTex,
    };
    static void* noseTextures[] = {
        gDaruniaNoseSeriousTex,
        gDaruniaNoseHappyTex,
    };
    EnDu* this = (EnDu*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTextures[this->mouthTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(noseTextures[this->noseTexIndex]));

    func_80034BA0(play, &this->skelAnime, EnDu_OverrideLimbDraw, EnDu_PostLimbDraw, &this->actor, 255);

    CLOSE_DISPS(play->state.gfxCtx);
}
