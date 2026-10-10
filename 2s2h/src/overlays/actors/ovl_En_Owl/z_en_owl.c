/*
 * File: z_en_owl.c
 * Overlay: ovl_En_Owl
 * Description: Kaepora Gaebora (Owl)
 */

#include "z_en_owl.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnOwl_Init(Actor* thisx, PlayState* play);
void EnOwl_Destroy(Actor* thisx, PlayState* play);
void EnOwl_Update(Actor* thisx, PlayState* play);
void EnOwl_Draw(Actor* thisx, PlayState* play);

void func_8095AB1C(EnOwl* this, PlayState* play);
void func_8095ABF0(EnOwl* this, PlayState* play);
void func_8095AD54(EnOwl* this, PlayState* play);
void func_8095AE00(EnOwl* this, PlayState* play);
void func_8095AE60(EnOwl* this, PlayState* play);
void func_8095AEC0(EnOwl* this, PlayState* play);
void func_8095AF2C(EnOwl* this, PlayState* play);
void func_8095AFEC(EnOwl* this, PlayState* play);
void func_8095B254(EnOwl* this, PlayState* play);
void func_8095B2F8(EnOwl* this, PlayState* play);
void func_8095B650(EnOwl* this, PlayState* play);
void func_8095B6C8(EnOwl* this, PlayState* play);
void func_8095B76C(EnOwl* this, PlayState* play);
void func_8095B960(EnOwl* this, PlayState* play);
void func_8095BA84(EnOwl* this, PlayState* play);
void func_8095BE0C(EnOwl* this, PlayState* play);
void func_8095BF20(EnOwl* this, PlayState* play);
void func_8095BF58(EnOwl* this, PlayState* play);
void func_8095C1C8(EnOwl* this, PlayState* play);
void func_8095C328(EnOwl* this);
void func_8095C408(EnOwl* this);
void func_8095C484(EnOwl* this);
void func_8095CCF4(Actor* thisx, PlayState* play);
void func_8095D074(Actor* thisx, PlayState* play);
void EnOwl_ChangeMode(EnOwl* this, EnOwlActionFunc actionFunc, EnOwlFunc unkFunc, SkelAnime* skelAnime,
                      AnimationHeader* animation, f32 morphFrames);

typedef enum {
    /* 0 */ OWL_REPEAT,
    /* 1 */ OWL_OK
} EnOwlMessageChoice;

ActorProfile En_Owl_Profile = {
    /**/ ACTOR_EN_OWL,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OWL,
    /**/ sizeof(EnOwl),
    /**/ EnOwl_Init,
    /**/ EnOwl_Destroy,
    /**/ EnOwl_Update,
    /**/ EnOwl_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
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
    { 30, 40, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 25, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2400, ICHAIN_STOP),
};

void func_8095A510(EnOwl* this, PlayState* play) {
    this->pathIndex = ENOWL_GET_PATH_INDEX(&this->actor);
    if (this->pathIndex == ENOWL_PATH_INDEX_NONE) {
        this->pathIndex = PATH_INDEX_NONE;
        this->path = NULL;
    } else {
        this->unk_3F8 = 0;
        this->path = &play->setupPathList[this->pathIndex];
    }
}

void EnOwl_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOwl* this = (EnOwl*)thisx;
    s32 i;
    s16 csId = this->actor.csId;
    s32 owlType;
    s32 switchFlag;

    for (i = 0; i < ARRAY_COUNT(this->csIdList); i++) {
        this->csIdList[i] = csId;
        if (csId != CS_ID_NONE) {
            this->actor.csId = csId;
            csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
        }
    }

    this->actor.csId = this->csIdList[0];
    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (ENOWL_GET_TYPE(&this->actor) == ENOWL_GET_TYPE_30) {
        Actor_SetScale(&this->actor, 0.1f);
        this->actor.update = func_8095CCF4;
        this->actor.draw = func_8095D074;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->unk_3D8 = 0;
        this->unk_3DA = 0x320;
        this->unk_3DC = 0x12C;
        return;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnimeFlying, &gOwlFlyingSkel, &gOwlFlyAnim, this->jointTableFlying,
                       this->morphTableFlying, OWL_FLYING_LIMB_MAX);
    SkelAnime_InitFlex(play, &this->skelAnimePerching, &gOwlPerchingSkel, &gOwlPerchAnim, this->jointTablePerching,
                       this->morphTablePerching, OWL_PERCHING_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.terminalVelocity = -10.0f;
    this->actor.lockOnArrowOffset = 500.0f;
    EnOwl_ChangeMode(this, func_8095BF58, func_8095C484, &this->skelAnimePerching, &gOwlPerchAnim, 0.0f);

    this->actionFlags = 0;
    this->unk_40D = 0;
    this->unk_40A = 0;
    this->unk_409 = 4;
    this->unk_40B = this->unk_408 = 0;
    this->unk_40C = 4;
    this->csIdIndex = -1;
    owlType = ENOWL_GET_TYPE(&this->actor);
    switchFlag = ENOWL_GET_SWITCH_FLAG(&this->actor);

    switch (owlType) {
        case ENOWL_GET_TYPE_1:
            if ((switchFlag < ENOWL_SWITCH_FLAG_NONE) && Flags_GetSwitch(play, switchFlag)) {
                Actor_Kill(&this->actor);
                return;
            }
            break;

        case ENOWL_GET_TYPE_2:
            if (GameInteractor_Should(VB_KILL_GORON_VILLAGE_OWL,
                                      gSaveContext.save.saveInfo.inventory.items[ITEM_LENS_OF_TRUTH] ==
                                          ITEM_LENS_OF_TRUTH)) {
                Actor_Kill(&this->actor);
                return;
            }
            break;

        case ENOWL_GET_TYPE_3:
            if (CHECK_QUEST_ITEM(QUEST_SONG_SOARING)) {
                Actor_Kill(&this->actor);
                return;
            }
            break;

        default:
            break;
    }

    this->unk_3DA = 0;
    this->unk_3F0 = this->actor.home.pos.y;
    this->unk_3EC = this->actor.home.rot.y;
    func_8095A510(this, play);

    switch (owlType) {
        case ENOWL_GET_TYPE_1:
            this->actionFunc = func_8095AEC0;
            break;

        case ENOWL_GET_TYPE_2:
            this->actionFunc = func_8095BE0C;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_09_20)) {
                this->actor.textId = 0xBF0;
            } else {
                this->actor.textId = 0xBEA;
            }
            break;

        case ENOWL_GET_TYPE_3:
            this->actionFunc = func_8095AFEC;
            break;

        case ENOWL_GET_TYPE_1000:
            this->actionFunc = func_8095BF20;
            break;

        default:
            this->actionFlags |= 2;
            this->unk_3DA = 0x20;
            this->actionFunc = func_8095BF58;
            break;
    }
}

void EnOwl_Destroy(Actor* thisx, PlayState* play) {
    EnOwl* this = (EnOwl*)thisx;

    if (ENOWL_GET_TYPE(&this->actor) != ENOWL_GET_TYPE_30) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void func_8095A920(EnOwl* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &player->actor.world.pos);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 6, 0x3E8, 0xC8);
}

s32 func_8095A978(EnOwl* this, PlayState* play, u16 textId, f32 targetDist, f32 arg4) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        return true;
    }

    this->actor.textId = textId;
    if (this->actor.xzDistToPlayer < targetDist) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, targetDist, arg4, PLAYER_IA_NONE);
    }

    return false;
}

s32 func_8095A9FC(EnOwl* this, PlayState* play, u16 textId) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        return true;
    }

    this->actor.textId = textId;
    if (this->actor.xzDistToPlayer < 120.0f) {
        Actor_OfferTalkExchange(&this->actor, play, 350.0f, 1000.0f, PLAYER_IA_NONE);
    }

    return false;
}

void func_8095AA70(EnOwl* this) {
    EnOwl_ChangeMode(this, func_8095C1C8, func_8095C484, &this->skelAnimeFlying, &gOwlUnfoldWingsAnim, 0.0f);
    this->eyeTexIndex = 0;
    this->blinkTimer = Rand_S16Offset(60, 60);
}

void func_8095AAD0(EnOwl* this, PlayState* play) {
    s32 switchFlag = ENOWL_GET_SWITCH_FLAG(&this->actor);

    if (switchFlag < ENOWL_SWITCH_FLAG_NONE) {
        Flags_SetSwitch(play, switchFlag);
    }

    func_8095AA70(this);
}

void func_8095AB1C(EnOwl* this, PlayState* play) {
    if ((this->unk_3DA % 64) == 0) {
        func_8095AAD0(this, play);
    }
}

void func_8095AB4C(EnOwl* this) {
    if (Rand_CenteredFloat(1.0f) < 0.0f) {
        this->actionFlags |= 0x20;
    } else {
        this->actionFlags &= ~0x20;
    }
}

// Unused?
void func_8095ABA8(EnOwl* this) {
    func_8095AB4C(this);
    this->unk_40B = 0;
    this->unk_3DE = 0;
    this->actionFlags |= 0x10;
    this->unk_40C = 4;
    this->unk_408 = 0;
    this->unk_40A = 0;
    this->unk_409 = 4;
}

void func_8095ABF0(EnOwl* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
        func_8095AAD0(this, play);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
}

// Unused?
void func_8095AC50(EnOwl* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
        if ((this->unk_3DA % 64) == 0) {
            func_8095AAD0(this, play);
        } else {
            this->actionFlags &= ~2;
            func_8095ABA8(this);
            this->actionFunc = func_8095AB1C;
        }
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
}

void func_8095ACEC(EnOwl* this) {
    this->actionFlags &= ~0x40;
    if (this->csIdIndex >= 0) {
        if (CutsceneManager_GetCurrentCsId() == this->csIdList[this->csIdIndex]) {
            CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
        }
        this->csIdIndex = -1;
    }
}

void func_8095AD54(EnOwl* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case OWL_REPEAT:
                Message_ContinueTextbox(play, 0x7D1);
                this->actionFunc = func_8095AE00;
                break;

            case OWL_OK:
                Message_ContinueTextbox(play, 0x7D3);
                this->actionFunc = func_8095ABF0;
                break;

            default:
                break;
        }
    }
}

void func_8095AE00(EnOwl* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_ContinueTextbox(play, 0x7D2);
        this->actionFunc = func_8095AD54;
    }
}

void func_8095AE60(EnOwl* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_ContinueTextbox(play, 0x7D1);
        this->actionFunc = func_8095AE00;
    }
}

void func_8095AEC0(EnOwl* this, PlayState* play) {
    func_8095A920(this, play);
    if (func_8095A978(this, play, 0x7D0, 360.0f, 200.0f)) {
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_8095AE60;
    }
}

void func_8095AF2C(EnOwl* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                if (play->msgCtx.currentTextId == 0xBFE) {
                    func_8095ACEC(this);
                    Message_CloseTextbox(play);
                    this->actionFunc = func_8095ABF0;
                } else {
                    Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                }
            }
            break;

        case TEXT_STATE_DONE:
            func_8095ACEC(this);
            this->actionFunc = func_8095ABF0;
            break;

        default:
            break;
    }
}

void func_8095AFEC(EnOwl* this, PlayState* play) {
    func_8095A920(this, play);
    if (func_8095A978(this, play, 0xBF6, 200.0f, 100.0f)) {
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_8095AF2C;
        this->csIdIndex = 0;
        this->actionFlags |= 0x40;
    }
}

s32 func_8095B06C(EnOwl* this) {
    if ((this->actor.world.pos.y + 2.0f) < this->unk_3F0) {
        this->actor.world.pos.y += 2.0f;
        return false;
    }

    if ((this->actor.world.pos.y - 2.0f) > this->unk_3F0) {
        this->actor.world.pos.y -= 2.0f;
        return false;
    }

    return true;
}

void func_8095B0C8(EnOwl* this) {
    s32 pad;
    Vec3s* points = Lib_SegmentedToVirtual(this->path->points);

    points += this->unk_3F8;
    this->unk_3EC = Math_Atan2S_XY(points->z - this->actor.world.pos.z, points->x - this->actor.world.pos.x);
    this->unk_3F0 = points->y;
}

void func_8095B158(EnOwl* this) {
    if (Animation_OnFrame(&this->skelAnimeFlying, 2.0f) || Animation_OnFrame(&this->skelAnimeFlying, 9.0f) ||
        Animation_OnFrame(&this->skelAnimeFlying, 23.0f) || Animation_OnFrame(&this->skelAnimeFlying, 40.0f) ||
        Animation_OnFrame(&this->skelAnimeFlying, 58.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_OWL_FLUTTER);
    }
}

void func_8095B1E4(EnOwl* this, PlayState* play) {
    if (this->actor.speed < 6.0f) {
        this->actor.speed += 1.0f;
    }

    if (this->actor.xzDistToPlayer > 6000.0f) {
        Actor_Kill(&this->actor);
    }
}

void func_8095B254(EnOwl* this, PlayState* play) {
    if (this->actor.speed < 6.0f) {
        this->actor.speed += 1.0f;
    }

    if (this->actionFlags & 1) {
        EnOwl_ChangeMode(this, func_8095B1E4, func_8095C328, &this->skelAnimeFlying, &gOwlFlyAnim, 0.0f);
        this->unk_3EA = 6;
        this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
    }

    func_8095B158(this);
}

void func_8095B2F8(EnOwl* this, PlayState* play) {
    func_8095B06C(this);
    if (this->skelAnimeFlying.curFrame >= 18.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_3EC, 2, 0x200, 0x80);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if ((this->actor.shape.rot.y == this->unk_3EC) && (this->actionFlags & 1)) {
        this->actionFunc = func_8095B254;
        Animation_Change(this->curSkelAnime, this->curSkelAnime->animation, 1.0f, 19.0f,
                         Animation_GetLastFrame(&gOwlTakeoffAnim), ANIMMODE_ONCE, 0.0f);
        this->unk_414 = func_8095C484;
    }

    func_8095B158(this);
}

void func_8095B3DC(EnOwl* this, PlayState* play) {
    if (this->actionFlags & 1) {
        this->actionFunc = func_8095B2F8;
        Animation_Change(this->curSkelAnime, &gOwlTakeoffAnim, 1.0f, 0.0f, 35.0f, ANIMMODE_ONCE, 0.0f);
        this->unk_414 = func_8095C408;
        this->unk_3EC = 0x5500;
        this->actor.world.pos.y += 100.0f;
        this->unk_3F0 = this->actor.world.pos.y;
    }
}

void func_8095B480(EnOwl* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags3 & PLAYER_STATE3_10000000) {
        this->actor.textId = 0xBF1;
        EnOwl_ChangeMode(this, func_8095BF58, func_8095C484, &this->skelAnimePerching, &gOwlPerchAnim, 0.0f);
        this->eyeTexIndex = 0;
        this->actionFlags &= ~8;
        this->blinkTimer = Rand_S16Offset(60, 60);
        this->actor.home.rot.x = 0;
        this->actionFlags |= 8;
        func_8095ACEC(this);
        this->unk_3F0 = this->actor.home.pos.y;
        this->unk_3EC = this->actor.home.rot.y;
        this->actor.world.pos = this->actor.home.pos;
        func_8095A510(this, play);
        this->actor.speed = 0.0f;
        this->actionFunc = func_8095BE0C;
    }
}

void func_8095B574(EnOwl* this, PlayState* play) {
    func_8095A920(this, play);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_8095BA84;
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFlags |= 0x40;
        this->csIdIndex = 2;
    } else if (this->actor.xzDistToPlayer < 200.0f) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, 200.0f, 400.0f, PLAYER_IA_NONE);
    } else {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
    func_8095B480(this, play);
}

void func_8095B650(EnOwl* this, PlayState* play) {
    if (this->actionFlags & 1) {
        if (GameInteractor_Should(VB_OWL_TELL_ABOUT_SHRINE, true, this)) {
            EnOwl_ChangeMode(this, func_8095B574, func_8095C484, &this->skelAnimePerching, &gOwlPerchAnim, 0.0f);
            this->actor.textId = 0xBF5;
        }
        this->actionFlags &= ~8;
    }
    func_8095B480(this, play);
}

void func_8095B6C8(EnOwl* this, PlayState* play) {
    if (this->actionFlags & 1) {
        Animation_Change(this->curSkelAnime, &gOwlUnfoldWingsAnim, -1.0f, Animation_GetLastFrame(&gOwlUnfoldWingsAnim),
                         0.0f, ANIMMODE_ONCE, 0.0f);
        this->unk_414 = func_8095C484;
        this->actionFunc = func_8095B650;
    }
    func_8095B158(this);
    func_8095B480(this, play);
}

void func_8095B76C(EnOwl* this, PlayState* play) {
    Actor* thisx = &this->actor;
    s16 sp4A;
    f32 sp44 = Path_OrientAndGetDistSq(thisx, this->path, this->unk_3F8, &sp4A);
    Vec3s* point;

    Math_SmoothStepToS(&thisx->world.rot.y, sp4A, 6, 0x800, 0x200);
    thisx->shape.rot.y = thisx->world.rot.y;
    if (sp44 < SQ(thisx->speed)) {
        thisx->speed = 0.0f;
        point = &((Vec3s*)Lib_SegmentedToVirtual(this->path->points))[this->unk_3F8];

        thisx->world.pos.x = point->x;
        thisx->world.pos.z = point->z;

        this->unk_3F8++;
        if (this->unk_3F8 >= this->path->count) {
            this->actionFunc = func_8095B6C8;
            return;
        }

        if (GameInteractor_Should(VB_OWL_SPAWN_FEATHER, true, this)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_OWL, thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z,
                        0, 0, 0, 0xF00);
        }
        thisx->home.rot.x++;
        if (thisx->home.rot.x >= 3) {
            func_8095ACEC(this);
        }
        func_8095B0C8(this);
    } else if (sp44 < SQ(21.0f)) {
        if (thisx->speed > 1.0f) {
            thisx->speed -= 1.0f;
        } else {
            thisx->speed = 1.0f;
        }
    } else if (thisx->speed < 6.0f) {
        thisx->speed += 1.0f;
    }

    func_8095B06C(this);
    func_8095B158(this);
    func_8095B480(this, play);
}

void func_8095B960(EnOwl* this, PlayState* play) {
    if (this->skelAnimeFlying.curFrame >= 18.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_3EC, 2, 0x200, 0x80);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    func_8095B06C(this);
    if (this->actor.shape.rot.y == this->unk_3EC) {
        this->actionFunc = func_8095B76C;
    }

    func_8095B158(this);
    func_8095B480(this, play);
}

void func_8095B9FC(EnOwl* this, PlayState* play) {
    if (this->actionFlags & 1) {
        this->actionFunc = func_8095B960;
        Animation_Change(this->curSkelAnime, &gOwlTakeoffAnim, 1.0f, 0.0f, 35.0f, ANIMMODE_ONCE, 0.0f);
        this->unk_414 = func_8095C408;
        func_8095B0C8(this);
    }
}

void func_8095BA84(EnOwl* this, PlayState* play) {
    func_8095A920(this, play);

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0xBEC:
                        switch (play->msgCtx.choiceIndex) {
                            case 0:
                                Audio_PlaySfx_MessageDecide();
                                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_09_40)) {
                                    Message_ContinueTextbox(play, 0xBF4);
                                } else {
                                    SET_WEEKEVENTREG(WEEKEVENTREG_09_40);
                                    Message_ContinueTextbox(play, 0xBED);
                                }
                                break;

                            case 1:
                                Audio_PlaySfx_MessageCancel();
                                Message_ContinueTextbox(play, 0xBEF);
                                break;

                            default:
                                break;
                        }
                        break;

                    case 0xBF2:
                        switch (play->msgCtx.choiceIndex) {
                            case 0:
                                Audio_PlaySfx_MessageDecide();
                                Message_ContinueTextbox(play, 0xBF4);
                                break;

                            case 1:
                                Audio_PlaySfx_MessageCancel();
                                Message_ContinueTextbox(play, 0xBF3);
                                break;

                            default:
                                break;
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0xBEA:
                        SET_WEEKEVENTREG(WEEKEVENTREG_09_20);
                        Message_ContinueTextbox(play, 0xBEB);
                        break;

                    case 0xBEB:
                    case 0xBF0:
                        Message_ContinueTextbox(play, 0xBEC);
                        break;

                    case 0xBED:
                    case 0xBF4:
                        Message_ContinueTextbox(play, 0xBEE);
                        break;

                    case 0xBEE:
                        Message_CloseTextbox(play);
                        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
                        EnOwl_ChangeMode(this, func_8095B9FC, func_8095C484, &this->skelAnimeFlying,
                                         &gOwlUnfoldWingsAnim, 0.0f);
                        this->eyeTexIndex = 0;
                        this->blinkTimer = Rand_S16Offset(60, 60);
                        this->actionFlags |= 8;
                        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                        this->actor.home.rot.x = 0;
                        func_8095ACEC(this);
                        this->csIdIndex = 0;
                        this->actionFlags |= 0x40;
                        break;

                    case 0xBEF:
                    case 0xBF3:
                        Message_CloseTextbox(play);
                        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
                        func_8095ACEC(this);
                        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                        this->actor.textId = 0xBF0;
                        this->actionFunc = func_8095BE0C;
                        break;

                    case 0xBF1:
                        Message_ContinueTextbox(play, 0xBF2);
                        break;

                    case 0xBF5:
                        Message_CloseTextbox(play);
                        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
                        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                        EnOwl_ChangeMode(this, func_8095B3DC, func_8095C484, &this->skelAnimeFlying,
                                         &gOwlUnfoldWingsAnim, 0.0f);
                        this->eyeTexIndex = 0;
                        this->blinkTimer = Rand_S16Offset(60, 60);
                        this->actionFlags |= 8;
                        func_8095ACEC(this);
                        break;

                    default:
                        break;
                }
            }
            break;

        default:
            break;
    }
}

void func_8095BE0C(EnOwl* this, PlayState* play) {
    func_8095A920(this, play);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_8095BA84;
        Audio_PlayFanfare(NA_BGM_OWL);
        this->csIdIndex = 1;
        this->actionFlags |= 0x40;
    } else if (this->actor.textId == 0xBF0) {
        if (this->actor.isLockedOn) {
            Actor_OfferTalkExchange(&this->actor, play, 200.0f, 200.0f, PLAYER_IA_NONE);
        }
    } else if (this->actor.xzDistToPlayer < 200.0f) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, 200.0f, 200.0f, PLAYER_IA_NONE);
    } else {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
}

void func_8095BF20(EnOwl* this, PlayState* play) {
    func_8095A9FC(this, play, 0x7D0);
    this->actionFunc = func_8095ABF0;
}

void func_8095BF58(EnOwl* this, PlayState* play) {
    func_8095A920(this, play);
}

void func_8095BF78(EnOwl* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
    if (this->actor.xzDistToPlayer > 6000.0f) {
        Actor_Kill(&this->actor);
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_3EC, 2, 0x80, 0x40);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->actor.speed < 16.0f) {
        this->actor.speed += 0.5f;
    }

    if ((this->unk_3E4 + 1000.0f) < this->actor.world.pos.y) {
        if (this->actor.velocity.y > 0.0f) {
            this->actor.velocity.y -= 0.4f;
        }
    } else if (this->actor.velocity.y < 4.0f) {
        this->actor.velocity.y += 0.2f;
    }
}

void func_8095C09C(EnOwl* this, PlayState* play) {
    if (this->skelAnimeFlying.curFrame > 10.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_3EC, 2, 0x400, 0x40);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (this->skelAnimeFlying.curFrame > 45.0f) {
        this->actor.velocity.y = 2.0f;
        this->actor.gravity = 0.0f;
        this->actor.speed = 8.0f;
    } else if (this->skelAnimeFlying.curFrame > 17.0f) {
        this->actor.velocity.y = 6.0f;
        this->actor.gravity = 0.0f;
        this->actor.speed = 4.0f;
    }

    if (this->actionFlags & 1) {
        EnOwl_ChangeMode(this, func_8095BF78, func_8095C328, &this->skelAnimeFlying, &gOwlFlyAnim, 0.0f);
        this->unk_3EA = 6;
        this->unk_3EC += 0x2000;
    }

    func_8095B158(this);
}

void func_8095C1C8(EnOwl* this, PlayState* play) {
    if (this->actionFlags & 1) {
        this->unk_3EA = 3;
        EnOwl_ChangeMode(this, func_8095C09C, func_8095C484, &this->skelAnimeFlying, &gOwlTakeoffAnim, 0.0f);
        this->unk_3EC = BINANG_ADD(this->actor.world.rot.y, 0x4000);
        this->unk_3E4 = this->actor.world.pos.y;
        this->actor.velocity.y = 2.0f;
    }
    this->actionFlags |= 8;
}

void func_8095C258(EnOwl* this) {
    SkelAnime_Update(this->curSkelAnime);
    if (this->unk_3EA > 0) {
        this->unk_3EA--;
        this->actor.shape.rot.z = Math_SinS(this->unk_3EA * 0x333) * 1000.0f;
    } else {
        this->unk_414 = func_8095C328;
        this->unk_3EA = 6;
        Animation_Change(this->curSkelAnime, &gOwlFlyAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gOwlFlyAnim),
                         ANIMMODE_ONCE, 5.0f);
    }
}

void func_8095C328(EnOwl* this) {
    if (SkelAnime_Update(this->curSkelAnime)) {
        if (this->unk_3EA > 0) {
            this->unk_3EA--;
            Animation_Change(this->curSkelAnime, this->curSkelAnime->animation, 1.0f, 0.0f,
                             Animation_GetLastFrame(this->curSkelAnime->animation), ANIMMODE_ONCE, 0.0f);
        } else {
            this->unk_3EA = 160;
            this->unk_414 = func_8095C258;
            Animation_Change(this->curSkelAnime, &gOwlGlideAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gOwlGlideAnim),
                             ANIMMODE_LOOP, 5.0f);
        }
    }
}

void func_8095C408(EnOwl* this) {
    if (SkelAnime_Update(this->curSkelAnime)) {
        Animation_Change(this->curSkelAnime, this->curSkelAnime->animation, 1.0f, 18.0f, 35.0f, ANIMMODE_ONCE, 0.0f);
        this->actionFlags |= 1;
    } else {
        this->actionFlags &= ~1;
    }
}

void func_8095C484(EnOwl* this) {
    if (SkelAnime_Update(this->curSkelAnime)) {
        Animation_Change(this->curSkelAnime, this->curSkelAnime->animation, 1.0f, 0.0f,
                         Animation_GetLastFrame(this->curSkelAnime->animation), ANIMMODE_ONCE, 0.0f);
        this->actionFlags |= 1;
    } else {
        this->actionFlags &= ~1;
    }
}

s32 func_8095C510(EnOwl* this) {
    s32 phi_v1;

    if (this->actionFlags & 2) {
        phi_v1 = 0x20;
    } else {
        phi_v1 = 0;
    }

    if (phi_v1 == (this->unk_3DA & 0x3F)) {
        return true;
    }

    if (this->actionFlags & 0x20) {
        this->unk_3DA += 4;
    } else {
        this->unk_3DA -= 4;
    }

    return false;
}

void func_8095C568(EnOwl* this) {
    if (this->actionFlags & 0x40) {
        if ((this->csIdIndex < 0) || (this->csIdList[this->csIdIndex] <= CS_ID_NONE)) {
            this->actionFlags &= ~0x40;
        } else if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        } else if (CutsceneManager_IsNext(this->csIdList[this->csIdIndex])) {
            CutsceneManager_StartWithPlayerCs(this->csIdList[this->csIdIndex], &this->actor);
            this->actionFlags &= ~0x40;
        } else {
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        }
    }
}

void EnOwl_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOwl* this = (EnOwl*)thisx;
    s16 sp36;

    if (this->actor.draw != NULL) {
        Actor_MoveWithGravity(&this->actor);
    }

    if (this->unk_414 != NULL) {
        this->unk_414(this);
    }

    this->actionFunc(this, play);
    func_8095C568(this);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 10.0f, UPDBGCHECKINFO_FLAG_4);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    if (this->actor.update == NULL) {
        return;
    }

    if ((this->skelAnimeFlying.animation == &gOwlFlyAnim) && Animation_OnFrame(&this->skelAnimeFlying, 4.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_OWL_FLUTTER);
    }

    if (this->actionFlags & 2) {
        this->eyeTexIndex = 2;
    } else {
        if (DECR(this->blinkTimer) == 0) {
            this->blinkTimer = Rand_S16Offset(60, 60);
        }
        this->eyeTexIndex = this->blinkTimer;
        if (this->eyeTexIndex >= 3) {
            this->eyeTexIndex = 0;
        }
    }

    if (!(this->actionFlags & 8)) {
        sp36 = 0;

        if (this->actionFlags & 0x10) {
            switch (this->unk_408) {
                case 0:
                    this->unk_408 = 1;
                    this->unk_409 = 6;
                    break;

                case 1:
                    this->unk_409--;
                    if (this->unk_409 != 0) {
                        sp36 = Math_CosS(this->unk_409 * 0x2000) * 0x1000;
                    } else {
                        if (this->actionFlags & 2) {
                            this->unk_3DA = 0;
                        } else {
                            this->unk_3DA = 0x20;
                        }

                        if (this->actionFlags & 0x20) {
                            this->unk_3DA -= 4;
                        } else {
                            this->unk_3DA += 4;
                        }
                        this->unk_408++;
                    }

                    if (this->actionFlags & 0x20) {
                        sp36 = -sp36;
                    }
                    break;

                case 2:
                    if (func_8095C510(this)) {
                        this->actionFlags &= ~0x10;
                        this->unk_40A = (s32)Rand_ZeroFloat(20.0f) + 60;
                        this->unk_408 = 0;
                        func_8095AB4C(this);
                    }
                    break;

                default:
                    break;
            }
        } else {
            if (this->unk_40A > 0) {
                this->unk_40A--;
            } else {
                if (this->unk_408 == 0) {
                    if (Rand_ZeroOne() < 0.3f) {
                        this->unk_408 = 4;
                        this->unk_409 = 12;
                    } else {
                        this->unk_408 = 1;
                        this->unk_409 = 4;
                    }
                }
                this->unk_409--;

                switch (this->unk_408) {
                    case 1:
                        sp36 = Math_SinS((-this->unk_409 * 0x1000) + 0x4000) * 5000.0f;
                        if (this->unk_409 <= 0) {
                            this->unk_409 = (s32)(Rand_ZeroFloat(15.0f) + 5.0f);
                            this->unk_408 = 2;
                        }
                        break;

                    case 2:
                        sp36 = 0x1388;
                        if (this->unk_409 <= 0) {
                            this->unk_408 = 3;
                            this->unk_409 = 4;
                        }
                        break;

                    case 3:
                        sp36 = Math_SinS(this->unk_409 * 0x1000) * 5000.0f;
                        if (this->unk_409 <= 0) {
                            this->unk_40A = (s32)Rand_ZeroFloat(20.0f) + 60;
                            this->unk_408 = 0;
                            func_8095AB4C(this);
                        }
                        break;

                    case 4:
                        sp36 = Math_SinS(this->unk_409 * 0x2000) * 5000.0f;
                        if (this->unk_409 <= 0) {
                            this->unk_40A = (s32)Rand_ZeroFloat(20.0f) + 60;
                            this->unk_408 = 0;
                            func_8095AB4C(this);
                        }
                        break;

                    default:
                        break;
                }

                if (this->actionFlags & 0x20) {
                    sp36 = -sp36;
                }
            }

            if (this->unk_40D > 0) {
                this->unk_40D--;
            } else {
                this->unk_40C--;
                switch (this->unk_40B) {
                    case 0:
                        this->unk_3DE = (-this->unk_40C * 0x5DC) + 0x1770;
                        if (this->unk_40C <= 0) {
                            this->unk_40B = 1;
                            this->unk_40C = (s8)(Rand_ZeroFloat(15.0f) + 5.0f);
                        }
                        break;

                    case 1:
                        this->unk_3DE = 0x1770;
                        if (this->unk_40C <= 0) {
                            this->unk_40B = 2;
                            this->unk_40C = 4;
                        }
                        break;

                    case 2:
                        this->unk_3DE = this->unk_40C * 0x5DC;
                        if (this->unk_40C <= 0) {
                            this->unk_40B = 0;
                            this->unk_40C = 4;
                            this->unk_40D = (s32)Rand_ZeroFloat(40.0f) + 160;
                        }
                        break;

                    default:
                        break;
                }
            }
        }

        //! FAKE:
        if (sp36) {}

        this->unk_3DC = (u16)((this->unk_3DA << 2) << 8) + sp36;
        this->unk_3D8 = ABS(this->unk_3DC) >> 3;
    } else {
        this->unk_3DE = 0;
        if (this->actionFlags & 2) {
            this->unk_3DC = -0x8000;
        } else {
            this->unk_3DC = 0;
        }

        this->unk_3D8 = ABS(this->unk_3DC) >> 3;
    }
}

void func_8095CCF4(Actor* thisx, PlayState* play) {
    EnOwl* this = (EnOwl*)thisx;
    Player* player = GET_PLAYER(play);

    if (GameInteractor_Should(VB_OWL_DESPAWN_FEATHER, player->stateFlags3 & PLAYER_STATE3_10000000, this)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (this->actor.world.pos.y < (this->unk_3F0 - 1000.0f)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.world.pos.y -= 1.0f;
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 10.0f, UPDBGCHECKINFO_FLAG_4);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->unk_3DA = (this->unk_3DA >> 3) * 7;
        if (this->unk_3DC > 0) {
            this->unk_3DC--;
        } else {
            Actor_Kill(&this->actor);
            return;
        }
    }

    this->unk_3D8 += this->unk_3DA;
    this->unk_3DA -= (s16)(this->unk_3D8 >> 6);
}

s32 EnOwl_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnOwl* this = (EnOwl*)thisx;

    switch (limbIndex) {
        case OWL_FLYING_LIMB_HEAD: // OWL_PERCHING_LIMB_HEAD
            rot->x += this->unk_3DC;
            rot->z += this->unk_3D8;
            rot->z -= this->unk_3DE;
            break;

        case OWL_FLYING_LIMB_UPPER_BODY: // OWL_PERCHING_LIMB_UPPER_BODY
            rot->z += this->unk_3DE;
            break;

        case OWL_FLYING_LIMB_RIGHT_EYEBROW: // OWL_PERCHING_LIMB_RIGHT_EYEBROW
            if (!(this->actionFlags & 8)) {
                rot->y -= TRUNCF_BINANG(this->unk_3D8 * 1.5f);
            }
            break;

        case OWL_FLYING_LIMB_LEFT_EYEBROW: // OWL_PERCHING_LIMB_LEFT_EYEBROW
            if (!(this->actionFlags & 8)) {
                rot->y += TRUNCF_BINANG(this->unk_3D8 * 1.5f);
            }
            break;

        default:
            break;
    }

    return false;
}

void EnOwl_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnOwl* this = (EnOwl*)thisx;
    Vec3f sp18;

    sp18.z = 0.0f;
    if (this->actionFlags & 2) {
        sp18.x = 700.0f;
        sp18.y = 400.0f;
    } else {
        sp18.y = 0.0f;
        sp18.x = 1400.0f;
    }

    if (limbIndex == OWL_FLYING_LIMB_HEAD) { // OWL_PERCHING_LIMB_HEAD
        Matrix_MultVec3f(&sp18, &this->actor.focus.pos);
    }
}

void EnOwl_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = {
        gOwlEyeOpenTex,
        gOwlEyeHalfTex,
        gOwlEyeClosedTex,
    };
    s32 pad;
    EnOwl* this = (EnOwl*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));

    SkelAnime_DrawFlexOpa(play, this->curSkelAnime->skeleton, this->curSkelAnime->jointTable,
                          this->curSkelAnime->dListCount, EnOwl_OverrideLimbDraw, EnOwl_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_8095D074(Actor* thisx, PlayState* play) {
    EnOwl* this = (EnOwl*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(0.0f, 500.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateXS(this->unk_3D8 - 0x4000, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, -500.0f, MTXMODE_APPLY);
    if (this->unk_3DC >= 0x20) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPDisplayList(POLY_OPA_DISP++, gOwlFeatherDL);
    } else {
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_XLU_SURF2);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, (u8)(this->unk_3DC * 8));
        gSPDisplayList(POLY_XLU_DISP++, gOwlFeatherDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnOwl_ChangeMode(EnOwl* this, EnOwlActionFunc actionFunc, EnOwlFunc unkFunc, SkelAnime* skelAnime,
                      AnimationHeader* animation, f32 morphFrames) {
    this->curSkelAnime = skelAnime;
    Animation_Change(this->curSkelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE,
                     morphFrames);
    this->actionFunc = actionFunc;
    this->unk_414 = unkFunc;
}
