/*
 * File: z_en_zot.c
 * Overlay: ovl_En_Zot
 * Description: Great Bay - Zora With Directions to Zora Hall / Pot Game Zora
 */

#include "z_en_zot.h"
#include "z64snap.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnZot_Init(Actor* thisx, PlayState* play2);
void EnZot_Destroy(Actor* thisx, PlayState* play);
void EnZot_Update(Actor* thisx, PlayState* play);
void EnZot_Draw(Actor* thisx, PlayState* play);

void EnZot_ChangeAnim(EnZot* this, s16 animIndex, u8 animMode);
void func_80B97100(EnZot* this, PlayState* play);
void func_80B97240(EnZot* this, PlayState* play);
void func_80B97708(EnZot* this, PlayState* play);
void func_80B97B5C(EnZot* this, PlayState* play);
void func_80B97CC8(EnZot* this, PlayState* play);
void func_80B97FD0(EnZot* this, PlayState* play);
void func_80B980FC(EnZot* this, PlayState* play);
void func_80B98728(EnZot* this, PlayState* play);
void func_80B98998(EnZot* this, PlayState* play);
void func_80B98CA8(EnZot* this, PlayState* play);
void func_80B990A4(EnZot* this, PlayState* play);
void func_80B992C0(EnZot* this, PlayState* play);
void func_80B99384(EnZot* this, PlayState* play);

ActorProfile En_Zot_Profile = {
    /**/ ACTOR_EN_ZOT,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZO,
    /**/ sizeof(EnZot),
    /**/ EnZot_Init,
    /**/ EnZot_Destroy,
    /**/ EnZot_Update,
    /**/ EnZot_Draw,
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

typedef enum EnZotAnimation {
    /* -1 */ ENZOT_ANIM_NONE = -1,
    /*  0 */ ENZOT_ANIM_0,
    /*  1 */ ENZOT_ANIM_1,
    /*  2 */ ENZOT_ANIM_2,
    /*  3 */ ENZOT_ANIM_3,
    /*  4 */ ENZOT_ANIM_4,
    /*  5 */ ENZOT_ANIM_5,
    /*  6 */ ENZOT_ANIM_6,
    /*  7 */ ENZOT_ANIM_7,
    /*  8 */ ENZOT_ANIM_8,
    /*  9 */ ENZOT_ANIM_9,
    /* 10 */ ENZOT_ANIM_MAX,
} EnZotAnimation;

static AnimationHeader* sAnimations[ENZOT_ANIM_MAX] = {
    &gZoraStandAnim,           // ENZOT_ANIM_0
    &gZoraWalkAnim,            // ENZOT_ANIM_1
    &gZoraSitAnim,             // ENZOT_ANIM_2
    &gZoraRunAnim,             // ENZOT_ANIM_3
    &gZoraFixSpeakerStartAnim, // ENZOT_ANIM_4
    &gZoraFixSpeakerLoopAnim,  // ENZOT_ANIM_5
    &gZoraFixSpeakerEndAnim,   // ENZOT_ANIM_6
    &gZoraBobHandAnim,         // ENZOT_ANIM_7
    &gZoraListenAnim,          // ENZOT_ANIM_8
    &gZoraListenAnim,          // ENZOT_ANIM_9
};

void func_80B965D0(EnZot* this, PlayState* play) {
    s32 i;

    if ((this->path != NULL) && (this->path->count >= 5)) {
        Vec3s* points = Lib_SegmentedToVirtual(this->path->points);

        for (i = 0; i < ARRAY_COUNT(this->unk_2D8); i++, points++) {
            if (this->unk_2D8[i] == NULL) {
                this->unk_2D8[i] = Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_TSUBO, points->x, points->y, points->z,
                                               0, 0, 0, 0x13F);
            }
        }
    }
}

void EnZot_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnZot* this = (EnZot*)thisx;
    s32 i;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = func_80B97100;
    SkelAnime_InitFlex(play, &this->skelAnime, &gZoraSkel, &gZoraIdleAnim, this->jointTable, this->morphTable,
                       ZORA_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gZoraStandAnim);
    this->animIndex = ENZOT_ANIM_0;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->unk_2F2 = 0;
    this->unk_2F4 = 0;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.world.rot.z = this->actor.shape.rot.z = 0;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.terminalVelocity = -4.0f;
    this->actor.gravity = -4.0f;

    switch (ENZOT_GET_1F(thisx)) {
        case 0:
            this->actionFunc = func_80B97240;
            this->actor.textId = 0;
            break;

        case 1:
            this->actionFunc = func_80B97708;
            for (i = 0; i < ARRAY_COUNT(this->unk_2D8); i++) {
                this->unk_2D8[i] = NULL;
            }
            if (ENZOT_GET_PATH_INDEX(&this->actor) != ENZOT_PATH_INDEX_NONE) {
                this->path = &play->setupPathList[ENZOT_GET_PATH_INDEX(&this->actor)];
                this->unk_2D4 = 0;
                func_80B965D0(this, play);
            } else {
                this->path = NULL;
            }
            break;

        case 2:
        case 3:
        case 4:
            this->actionFunc = func_80B97B5C;
            if (ENZOT_GET_PATH_INDEX(&this->actor) != ENZOT_PATH_INDEX_NONE) {
                this->path = &play->setupPathList[ENZOT_GET_PATH_INDEX(&this->actor)];
            } else {
                this->path = NULL;
            }
            break;

        case 5:
            this->unk_2F2 |= 4;
            this->actionFunc = func_80B97FD0;
            if (ENZOT_GET_PATH_INDEX(&this->actor) != ENZOT_PATH_INDEX_NONE) {
                this->path = &play->setupPathList[ENZOT_GET_PATH_INDEX(&this->actor)];
            } else {
                this->path = NULL;
            }
            EnZot_ChangeAnim(this, ENZOT_ANIM_5, ANIMMODE_LOOP);
            break;

        case 6:
            this->actionFunc = func_80B98998;
            EnZot_ChangeAnim(this, ENZOT_ANIM_2, ANIMMODE_LOOP);
            this->actor.colChkInfo.cylRadius = 0;
            this->actor.shape.yOffset = -1400.0f;
            break;

        case 7:
            this->actionFunc = func_80B98998;
            EnZot_ChangeAnim(this, ENZOT_ANIM_0, ANIMMODE_LOOP);
            break;

        case 8:
            this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
            this->actionFunc = func_80B98CA8;
            EnZot_ChangeAnim(this, ENZOT_ANIM_5, ANIMMODE_LOOP);
            break;

        case 9:
            this->actionFunc = func_80B990A4;
            break;

        case 10:
            this->actionFunc = func_80B992C0;
            EnZot_ChangeAnim(this, ENZOT_ANIM_1, ANIMMODE_LOOP);
            if (ENZOT_GET_PATH_INDEX(&this->actor) != ENZOT_PATH_INDEX_NONE) {
                this->path = &play->setupPathList[ENZOT_GET_PATH_INDEX(&this->actor)];
            } else {
                this->path = NULL;
            }
            break;

        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
            this->actionFunc = func_80B98998;
            EnZot_ChangeAnim(this, ENZOT_ANIM_2, ANIMMODE_LOOP);
            this->actor.colChkInfo.cylRadius = 0;
            this->actor.shape.yOffset = -1400.0f;
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
                Actor_Kill(&this->actor);
            }
            break;

        case 18:
            this->actionFunc = func_80B99384;
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
                Actor_Kill(&this->actor);
            }
            break;

        case 19:
            EnZot_ChangeAnim(this, ENZOT_ANIM_7, ANIMMODE_LOOP);
            break;

        case 20:
            EnZot_ChangeAnim(this, ENZOT_ANIM_8, ANIMMODE_ONCE);
            break;

        case 21:
            EnZot_ChangeAnim(this, ENZOT_ANIM_9, ANIMMODE_ONCE);
            break;

        case 22:
            this->actionFunc = func_80B980FC;
            if (ENZOT_GET_PATH_INDEX(&this->actor) != ENZOT_PATH_INDEX_NONE) {
                this->path = &play->setupPathList[ENZOT_GET_PATH_INDEX(&this->actor)];
            } else {
                this->path = NULL;
            }
            this->actor.home.rot.x = this->actor.home.rot.z;
            break;
    }

    if ((ENZOT_GET_1F(thisx) >= 2) && (ENZOT_GET_1F(thisx) < 11) &&
        CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
        Actor_Kill(&this->actor);
    }
}

void EnZot_Destroy(Actor* thisx, PlayState* play) {
    EnZot* this = (EnZot*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    if (ENZOT_GET_1F(&this->actor) == 8) {
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_41_20);
    }
}

void EnZot_ChangeAnim(EnZot* this, s16 animIndex, u8 animMode) {

    if ((animIndex > ENZOT_ANIM_NONE) && (animIndex < ENZOT_ANIM_MAX)) {
        if (animIndex >= ENZOT_ANIM_8) {
            Animation_Change(&this->skelAnime, sAnimations[animIndex], 0.0f, animIndex - 8, animIndex - 8, animMode,
                             0.0f);
        } else {
            Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimations[animIndex]), animMode, -5.0f);
        }
        this->animIndex = animIndex;
    }
}

s32 func_80B96CE4(EnZot* this) {
    s32 i;
    s32 count = 0;

    for (i = 0; i < ARRAY_COUNT(this->unk_2D8); i++) {
        if (this->unk_2D8[i] == NULL) {
            count++;
        }
    }

    return count;
}

void func_80B96D4C(EnZot* this) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_2D8); i++) {
        if ((this->unk_2D8[i] != NULL) && (this->unk_2D8[i]->update == NULL)) {
            this->unk_2D8[i] = NULL;
        }
    }
}

s32 func_80B96DF0(EnZot* this, PlayState* play) {
    if (Player_IsFacingActor(&this->actor, 0x3000, play) && Actor_IsFacingPlayer(&this->actor, 0x3000) &&
        (this->actor.xzDistToPlayer < 100.0f)) {
        return true;
    }
    return false;
}

s32 func_80B96E5C(EnZot* this) {
    Path* path = this->path;
    Vec3s* points;
    f32 temp_f12;
    f32 temp_f14;
    f32 phi_f2;

    if (path == NULL) {
        return true;
    }

    points = &((Vec3s*)Lib_SegmentedToVirtual(path->points))[this->unk_2D4];
    temp_f12 = points->x - this->actor.world.pos.x;
    temp_f14 = points->z - this->actor.world.pos.z;
    this->actor.world.rot.y = Math_Atan2S(temp_f12, temp_f14);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 0x7D0, 0xC8);
    phi_f2 = SQ(this->actor.speed) * SQ(3.0f);

    if (this->unk_2D4 == 0) {
        phi_f2 = SQ(20.0f);
    }

    if ((SQ(temp_f12) + SQ(temp_f14)) < phi_f2) {
        this->unk_2D4++;
        if (this->unk_2D4 >= path->count) {
            this->actor.world.pos.x = points->x;
            this->actor.world.pos.z = points->z;
            return true;
        }
    }

    return false;
}

s32 func_80B96FB0(EnZot* this) {
    Path* path = this->path;
    Vec3s* points;
    f32 temp_f12;
    f32 temp_f14;

    if (path == NULL) {
        return true;
    }

    points = &((Vec3s*)Lib_SegmentedToVirtual(path->points))[this->unk_2D4];
    temp_f12 = points->x - this->actor.world.pos.x;
    temp_f14 = points->z - this->actor.world.pos.z;
    this->actor.world.rot.y = Math_Atan2S(temp_f12, temp_f14);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 0x7D0, 0xC8);

    if ((SQ(temp_f12) + SQ(temp_f14)) < SQ(10.0f)) {
        if (this->unk_2F2 & 0x20) {
            this->unk_2D4--;
            if (this->unk_2D4 < 0) {
                this->unk_2D4 = 1;
                this->unk_2F2 &= ~0x20;
                return true;
            }
        } else {
            this->unk_2D4++;
            if (this->unk_2D4 >= path->count) {
                this->unk_2D4 = path->count - 1;
                this->unk_2F2 |= 0x20;
                return true;
            }
        }
    }

    return false;
}

void func_80B97100(EnZot* this, PlayState* play) {
}

void func_80B97110(EnZot* this, PlayState* play) {
    u16 textId;

    if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        textId = 0x125F;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_28_80)) {
            textId = 0x1261;
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_28_80);
        }
    } else {
        textId = 0x125C;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_28_40)) {
            textId = 0x125E;
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_28_40);
        }
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80B97194(EnZot* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x125C:
            case 0x125F:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x125D:
            case 0x125E:
            case 0x1260:
            case 0x1261:
                Message_CloseTextbox(play);
                this->actionFunc = func_80B97240;
                break;
        }
    }
}

void func_80B97240(EnZot* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B97194;
        func_80B97110(this, play);
    } else if ((this->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->actor, 0x3000, play) &&
               Actor_IsFacingPlayer(&this->actor, 0x3000)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80B972E8(EnZot* this, PlayState* play) {
    u16 textId;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_29_10)) {
        if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
            textId = 0x126A;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_29_01)) {
                textId = 0x126D;
            } else {
                SET_WEEKEVENTREG(WEEKEVENTREG_29_01);
            }
        } else {
            textId = 0x1267;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_29_02)) {
                textId = 0x1269;
            } else {
                SET_WEEKEVENTREG(WEEKEVENTREG_29_02);
            }
        }
    } else if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        textId = 0x1265;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_29_04)) {
            textId = 0x1266;
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_29_04);
        }
    } else {
        textId = 0x1262;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_29_08)) {
            textId = 0x1264;
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_29_08);
        }
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80B973BC(EnZot* this, PlayState* play) {
    func_80B96D4C(this);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x800, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x126E:
            case 0x1270:
            case 0x1273:
            case 0x1274:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x1272:
                Message_ContinueTextbox(play, 0x126F);
                break;

            case 0x126F:
                Rupees_ChangeBy(90);
                Message_ContinueTextbox(play, 0x1270);
                break;

            case 0x1275:
                if (gSaveContext.save.saveInfo.playerData.rupees < 10) {
                    Message_ContinueTextbox(play, 0x1277);
                } else {
                    Message_ContinueTextbox(play, 0x1278);
                    Rupees_ChangeBy(-10);
                }
                break;

            case 0x1276:
                Message_ContinueTextbox(play, 0x1275);
                break;

            case 0x1271:
            case 0x1277:
            case 0x1278:
            case 0x1279:
                Message_CloseTextbox(play);
                func_80B965D0(this, play);
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                this->actor.textId = 0;
                this->actionFunc = func_80B97708;
                if ((this->actor.csId != CS_ID_NONE) && !(this->unk_2F2 & 1)) {
                    CutsceneManager_Stop(this->actor.csId);
                }
                this->unk_2F2 &= ~1;
                break;
        }
    }

    if (this->unk_2F2 & 1) {
        if (this->actor.csId == CS_ID_NONE) {
            this->unk_2F2 &= ~1;
        } else if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->actor.csId);
        } else if (CutsceneManager_IsNext(this->actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
            this->unk_2F2 &= ~1;
        } else {
            CutsceneManager_Queue(this->actor.csId);
        }
    }
}

void func_80B975F8(EnZot* this, PlayState* play) {
    func_80B96D4C(this);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B973BC;
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 10000.0f, 1000.0f, PLAYER_IA_NONE);
    }
}

void func_80B9765C(EnZot* this, PlayState* play) {
    func_80B96D4C(this);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        s32 requiredScopeTemp;

        switch (play->msgCtx.currentTextId) {
            case 0x1262:
            case 0x1267:
            case 0x126A:
            case 0x126B:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            default:
                Message_CloseTextbox(play);
                this->actionFunc = func_80B97708;
                break;
        }
    }
}

void func_80B97708(EnZot* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 phi_v1;

    func_80B96D4C(this);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 2, 0x400, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B9765C;
        func_80B972E8(this, play);
        return;
    }

    if (!(player->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN)) {
        phi_v1 = func_80B96CE4(this);
    } else {
        phi_v1 = 0;
    }

    if (phi_v1 != 0) {
        SET_WEEKEVENTREG(WEEKEVENTREG_29_10);
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        if (phi_v1 == 5) {
            if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
                this->actor.textId = 0x126E;
            } else {
                this->actor.textId = 0x1272;
            }
        } else if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
            this->actor.textId = 0x1273;
        } else {
            this->actor.textId = 0x1276;
        }
        this->unk_2F2 |= 1;
        func_80B975F8(this, play);
        this->actionFunc = func_80B975F8;
    } else if (func_80B96DF0(this, play)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80B9787C(EnZot* this, PlayState* play) {
    u16 textId;

    if (this->actor.textId == 0) {
        if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
            switch (ENZOT_GET_1F(&this->actor)) {
                case 2:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_37_40)) {
                        textId = 0x127E;
                    } else {
                        textId = 0x127C;
                        SET_WEEKEVENTREG(WEEKEVENTREG_37_40);
                    }
                    break;

                case 3:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_38_01)) {
                        textId = 0x1284;
                    } else {
                        textId = 0x1282;
                        SET_WEEKEVENTREG(WEEKEVENTREG_38_01);
                    }
                    break;

                default:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_38_04)) {
                        textId = 0x128A;
                    } else {
                        textId = 0x1288;
                        SET_WEEKEVENTREG(WEEKEVENTREG_38_04);
                    }
                    break;
            }
        } else {
            switch (ENZOT_GET_1F(&this->actor)) {
                case 2:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_37_20)) {
                        textId = 0x127B;
                    } else {
                        textId = 0x1279;
                        SET_WEEKEVENTREG(WEEKEVENTREG_37_20);
                    }
                    break;

                case 3:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_37_80)) {
                        textId = 0x1281;
                    } else {
                        textId = 0x127F;
                        SET_WEEKEVENTREG(WEEKEVENTREG_37_80);
                    }
                    break;

                default:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_38_02)) {
                        textId = 0x1287;
                    } else {
                        textId = 0x1285;
                        SET_WEEKEVENTREG(WEEKEVENTREG_38_02);
                    }
                    break;
            }
        }
        Message_StartTextbox(play, textId, &this->actor);
    }
}

void func_80B979DC(EnZot* this, PlayState* play) {
    if (func_80B96E5C(this)) {
        this->actionFunc = func_80B97B5C;
        EnZot_ChangeAnim(this, ENZOT_ANIM_0, ANIMMODE_LOOP);
        this->actor.speed = 0.0f;
    } else {
        this->actor.speed = 1.5f;
    }
}

void func_80B97A44(EnZot* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x1279:
            case 0x127C:
            case 0x127F:
            case 0x1282:
            case 0x1285:
            case 0x1288:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x127D:
            case 0x127E:
            case 0x1283:
            case 0x1284:
            case 0x1289:
            case 0x128A:
                Message_CloseTextbox(play);
                if (!(this->unk_2F2 & 2)) {
                    this->unk_2F2 |= 2;
                    this->actionFunc = func_80B979DC;
                    EnZot_ChangeAnim(this, ENZOT_ANIM_1, ANIMMODE_LOOP);
                } else {
                    this->actionFunc = func_80B97B5C;
                }
                break;

            default:
                Message_CloseTextbox(play);
                this->actionFunc = func_80B97B5C;
                break;
        }
    }
}

void func_80B97B5C(EnZot* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 2, 0x400, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B97A44;
        func_80B9787C(this, play);
    } else if (func_80B96DF0(this, play)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80B97BF8(EnZot* this, PlayState* play) {
    u16 textId;

    if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        textId = 0x128C;
    } else {
        textId = 0x128B;
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80B97C40(EnZot* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x800, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->actionFunc = func_80B97CC8;
    }
}

void func_80B97CC8(EnZot* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B97C40;
        Message_StartTextbox(play, 0x128B, &this->actor);
    } else if (Player_IsFacingActor(&this->actor, 0x3000, play) && (this->actor.xzDistToPlayer < 100.0f)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80B97D6C(EnZot* this, PlayState* play) {
    if (func_80B96E5C(this)) {
        this->actionFunc = func_80B97CC8;
        this->actor.speed = 0.0f;
        EnZot_ChangeAnim(this, ENZOT_ANIM_0, ANIMMODE_LOOP);
    } else {
        this->actor.speed = 8.0f;
    }

    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_ZORA_WALK);
    }
}

void func_80B97E0C(EnZot* this, PlayState* play) {
    if (this->unk_2F2 & 0x40) {
        this->actionFunc = func_80B97D6C;
        EnZot_ChangeAnim(this, ENZOT_ANIM_3, ANIMMODE_LOOP);
    }
}

void func_80B97E4C(EnZot* this, PlayState* play) {
    if (this->unk_2F2 & 0x40) {
        EnZot_ChangeAnim(this, ENZOT_ANIM_0, ANIMMODE_LOOP);
    }

    if (!(this->unk_2F2 & 4)) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x800, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }

    if ((Message_GetState(&play->msgCtx) != TEXT_STATE_EVENT) || !Message_ShouldAdvance(play)) {
        return;
    }

    switch (play->msgCtx.currentTextId) {
        case 0x128C:
            this->unk_2F2 &= ~4;
            EnZot_ChangeAnim(this, ENZOT_ANIM_6, ANIMMODE_ONCE);
            Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
            break;

        case 0x128D:
        case 0x128E:
        case 0x128F:
            Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
            break;

        case 0x1290:
            Message_CloseTextbox(play);
            this->actionFunc = func_80B97D6C;
            this->unk_2F2 |= 4;
            EnZot_ChangeAnim(this, ENZOT_ANIM_3, ANIMMODE_LOOP);
            SET_WEEKEVENTREG(WEEKEVENTREG_38_08);
            break;

        case 0x128B:
            Message_CloseTextbox(play);
            this->actionFunc = func_80B97FD0;
            break;

        default:
            break;
    }
}

void func_80B97FD0(EnZot* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 2, 0x800, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B97E4C;
        func_80B97BF8(this, play);
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_38_08)) {
        if ((this->actor.xzDistToPlayer < 120.0f) && (GET_PLAYER_FORM == PLAYER_FORM_ZORA)) {
            this->unk_2F2 |= 4;
            this->actionFunc = func_80B97E0C;
            EnZot_ChangeAnim(this, ENZOT_ANIM_6, ANIMMODE_ONCE);
        }
    } else if (Player_IsFacingActor(&this->actor, 0x3000, play) && (this->actor.xzDistToPlayer < 100.0f)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80B980FC(EnZot* this, PlayState* play) {
    if (this->actor.home.rot.x > 0) {
        this->actor.home.rot.x--;
    } else if (func_80B96E5C(this)) {
        this->actionFunc = func_80B97100;
        this->actor.speed = 0.0f;
    } else {
        this->actor.speed = 8.0f;
        if (this->animIndex != ENZOT_ANIM_3) {
            EnZot_ChangeAnim(this, ENZOT_ANIM_3, ANIMMODE_LOOP);
        }
    }
}

void func_80B98178(EnZot* this, PlayState* play) {
    u16 textId;
    s32 phi_v0;

    switch (ENZOT_GET_1F(&this->actor)) {
        case 6:
            if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
                if (CURRENT_DAY == 3) {
                    textId = 0x129D;
                } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_39_01)) {
                    textId = 0x129C;
                } else {
                    textId = 0x129B;
                    SET_WEEKEVENTREG(WEEKEVENTREG_39_01);
                }
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_38_80)) {
                textId = 0x1293;
            } else {
                textId = 0x1291;
                SET_WEEKEVENTREG(WEEKEVENTREG_38_80);
            }
            break;

        case 7:
            if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_39_04)) {
                    textId = 0x12AA;
                } else {
                    textId = 0x12A6;
                    SET_WEEKEVENTREG(WEEKEVENTREG_39_04);
                }
            } else if (Flags_GetSwitch(play, ENZOT_GET_SWITCH_FLAG(&this->actor))) {
                textId = 0x12A0;
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_39_02)) {
                textId = 0x12A5;
            } else {
                textId = 0x12A2;
                SET_WEEKEVENTREG(WEEKEVENTREG_39_02);
            }
            break;

        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
            phi_v0 = (ENZOT_GET_1F(&this->actor) * 4) - 44;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_79_01)) {
                phi_v0 += 2;
            }

            if (GET_PLAYER_FORM != PLAYER_FORM_ZORA) {
                phi_v0++;
            }
            textId = phi_v0 + 0x1302;
            break;

        default:
            if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
                textId = 0x129F;
            } else {
                textId = 0x1295;
            }
            break;
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80B98348(EnZot* this, PlayState* play) {
    s16 y;

    if (ENZOT_GET_1F(&this->actor) == 7) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x800, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else if (Actor_IsFacingPlayer(&this->actor, 0x3000)) {
        this->unk_2F2 &= ~8;
    } else {
        this->unk_2F2 |= 8;
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 6, 0x1838, 0x64);
        y = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        if (y > 0) {
            Math_SmoothStepToS(&this->headRot.y, 0x3000, 6, 0x1838, 0x64);
            Math_SmoothStepToS(&this->torsoRot.y, 0x1000, 6, 0x1838, 0x64);
        } else {
            Math_SmoothStepToS(&this->headRot.y, -0x3000, 6, 0x1838, 0x64);
            Math_SmoothStepToS(&this->torsoRot.y, -0x1000, 6, 0x1838, 0x64);
        }
    }
}

void func_80B9849C(EnZot* this, PlayState* play) {
    func_80B98348(this, play);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (this->unk_2D4 == 2) {
            Message_StartTextbox(play, 0x12AD, &this->actor);
        } else {
            Message_StartTextbox(play, 0x12B0, &this->actor);
        }
        this->actionFunc = func_80B98728;
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    }
}

void func_80B9854C(EnZot* this, PlayState* play) {
    func_80B98348(this, play);
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_80B9849C;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    } else {
        Actor_OfferGetItem(&this->actor, play, this->unk_2D4, 10000.0f, 50.0f);
    }
}

void func_80B985EC(EnZot* this, PlayState* play) {
    PlayerItemAction itemAction;
    Player* player = GET_PLAYER(play);

    func_80B98348(this, play);
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_PAUSE_MENU) {
        itemAction = func_80123810(play);

        if (itemAction > PLAYER_IA_NONE) {
            Message_CloseTextbox(play);
            if ((itemAction == PLAYER_IA_PICTOGRAPH_BOX) && CHECK_QUEST_ITEM(QUEST_PICTOGRAPH) &&
                Snap_CheckFlag(PICTO_VALID_LULU_HEAD)) {
                if (Snap_CheckFlag(PICTO_VALID_LULU_RIGHT_ARM) && Snap_CheckFlag(PICTO_VALID_LULU_LEFT_ARM)) {
                    player->actor.textId = 0x12AE;
                } else {
                    player->actor.textId = 0x12AC;
                }
                REMOVE_QUEST_ITEM(QUEST_PICTOGRAPH);
            } else {
                player->actor.textId = 0x12AB;
            }
            this->actionFunc = func_80B98728;
        } else if (itemAction <= PLAYER_IA_MINUS1) {
            Message_ContinueTextbox(play, 0x12AB);
            this->actionFunc = func_80B98728;
        }
    }
}

void func_80B98728(EnZot* this, PlayState* play) {
    func_80B98348(this, play);

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play) && (play->msgCtx.currentTextId == 0x1293)) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        Audio_PlaySfx_MessageDecide();
                        Message_ContinueTextbox(play, 0x1294);
                        break;

                    case 1:
                        Audio_PlaySfx_MessageCancel();
                        Message_ContinueTextbox(play, 0x1298);
                        break;
                }
            }
            break;

        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x1291:
                    case 0x1292:
                    case 0x1296:
                    case 0x1298:
                    case 0x1299:
                    case 0x129B:
                    case 0x129D:
                    case 0x129E:
                    case 0x12A0:
                    case 0x12A2:
                    case 0x12A3:
                    case 0x12A4:
                    case 0x12A6:
                    case 0x12A7:
                    case 0x12A8:
                    case 0x12AE:
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x12A9:
                    case 0x12AA:
                        Message_ContinueTextbox(play, 0xFF);
                        this->actionFunc = func_80B985EC;
                        break;

                    case 0x1294:
                        if (CURRENT_DAY == 3) {
                            Message_ContinueTextbox(play, 0x1296);
                        } else {
                            Message_ContinueTextbox(play, 0x1295);
                        }
                        break;

                    case 0x12AB:
                        Message_CloseTextbox(play);
                        this->actionFunc = func_80B98998;
                        break;

                    case 0x12AC:
                        Message_CloseTextbox(play);
                        this->unk_2D4 = 2;
                        this->actionFunc = func_80B9854C;
                        func_80B9854C(this, play);
                        break;

                    case 0x12AF:
                        Message_CloseTextbox(play);
                        this->unk_2D4 = 4;
                        this->actionFunc = func_80B9854C;
                        func_80B9854C(this, play);
                        break;

                    default:
                        Message_CloseTextbox(play);
                        this->actionFunc = func_80B98998;
                        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                        break;
                }
            }
            break;
    }
}

void func_80B98998(EnZot* this, PlayState* play) {
    this->unk_2F2 &= ~8;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B98728;
        func_80B98178(this, play);
    } else if ((this->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->actor, 0x3000, play) &&
               Actor_IsFacingPlayer(&this->actor, 0x7000)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80B98A4C(EnZot* this, PlayState* play) {
    u16 textId;

    if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_39_80)) {
            textId = 0x12B6;
        } else {
            textId = 0x12B4;
            SET_WEEKEVENTREG(WEEKEVENTREG_39_80);
        }
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_39_40)) {
        textId = 0x12B3;
    } else {
        textId = 0x12B1;
        SET_WEEKEVENTREG(WEEKEVENTREG_39_40);
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80B98AD0(EnZot* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x12B1:
            case 0x12B4:
            case 0x12B7:
            case 0x12B9:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x12B8:
                Message_CloseTextbox(play);
                this->actionFunc = func_80B98CA8;
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_41_20);
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                break;

            case 0x12BA:
                Message_CloseTextbox(play);
                this->actionFunc = func_80B98CA8;
                SET_WEEKEVENTREG(WEEKEVENTREG_41_20);
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                break;

            default:
                Message_CloseTextbox(play);
                this->actionFunc = func_80B98CA8;
                break;
        }
    }
}

void func_80B98BF4(EnZot* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_41_20)) {
            Message_StartTextbox(play, 0x12B7, &this->actor);
            this->actionFunc = func_80B98AD0;
        } else {
            Message_StartTextbox(play, 0x12B9, &this->actor);
            this->actionFunc = func_80B98AD0;
        }
    } else {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80B98CA8(EnZot* this, PlayState* play) {
    if (Actor_OcarinaInteractionAccepted(&this->actor, &play->state)) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        AudioOcarina_StartDefault(0xFFFF);
        this->actionFunc = func_80B98BF4;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalk(&this->actor, play, 120.0f);
    } else if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B98AD0;
        func_80B98A4C(this, play);
    } else {
        if ((this->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->actor, 0x3000, play)) {
            Actor_OfferTalk(&this->actor, play, 120.0f);
        }

        if ((GET_PLAYER_FORM == PLAYER_FORM_ZORA) || (this->actor.xzDistToPlayer < 100.0f)) {
            Actor_OfferOcarinaInteraction(&this->actor, play, 120.0f, 100.0f);
        }
    }

    if (this->actor.xzDistToPlayer > 100.0f) {
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_41_20);
    }
}

void func_80B98E10(EnZot* this, PlayState* play) {
    u16 textId;

    if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        if (Flags_GetSwitch(play, ENZOT_GET_SWITCH_FLAG(&this->actor))) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_40_04)) {
                textId = 0x12C5;
            } else {
                textId = 0x12C3;
                SET_WEEKEVENTREG(WEEKEVENTREG_40_04);
            }
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_40_02)) {
            textId = 0x12C2;
        } else {
            textId = 0x12C0;
            SET_WEEKEVENTREG(WEEKEVENTREG_40_02);
        }
    } else {
        if (Flags_GetSwitch(play, ENZOT_GET_SWITCH_FLAG(&this->actor))) {
            if (this->unk_2F2 & 0x10) {
                textId = 0x12BF;
            } else {
                textId = 0x12BE;
                this->unk_2F2 |= 0x10;
            }
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_40_01)) {
            textId = 0x12BC;
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_40_01);
            this->unk_2F2 |= 4;
            textId = 0x12BB;
        }
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80B98F30(EnZot* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_80B990A4;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_BLUE, 10000.0f, 50.0f);
    }
}

void func_80B98F94(EnZot* this, PlayState* play) {
    if (!(this->unk_2F2 & 4)) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x800, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x12BB:
                this->unk_2F2 &= ~4;

            case 0x12BC:
            case 0x12C0:
            case 0x12C3:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x12BE:
                Message_CloseTextbox(play);
                this->actionFunc = func_80B98F30;
                func_80B98F30(this, play);
                break;

            default:
                Message_CloseTextbox(play);
                this->actionFunc = func_80B990A4;
                break;
        }
    }
}

void func_80B990A4(EnZot* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 2, 0x400, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B98F94;
        func_80B98E10(this, play);
    } else if (Player_IsFacingActor(&this->actor, 0x3000, play) && (this->actor.xzDistToPlayer < 100.0f)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80B99160(EnZot* this, PlayState* play) {
    u16 textId;

    if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_40_10)) {
            textId = 0x12CD;
        } else {
            textId = 0x12CA;
            SET_WEEKEVENTREG(WEEKEVENTREG_40_10);
        }
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_40_08)) {
        textId = 0x12C9;
    } else {
        textId = 0x12C6;
        SET_WEEKEVENTREG(WEEKEVENTREG_40_08);
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80B991E4(EnZot* this, PlayState* play) {
    if (1) {}

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x800, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        u16 temp = play->msgCtx.currentTextId;
        u32 temp2;

        if ((temp == 0x12C6) || (temp == 0x12C7) || (temp == 0x12CA) || (temp == 0x12CB)) {
            temp2 = temp;
            Message_ContinueTextbox(play, temp2 + 1);
        } else {
            Message_CloseTextbox(play);
            this->actionFunc = func_80B992C0;
            EnZot_ChangeAnim(this, ENZOT_ANIM_1, ANIMMODE_LOOP);
        }
    }
}

void func_80B992C0(EnZot* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B991E4;
        func_80B99160(this, play);
        this->actor.speed = 0.0f;
        EnZot_ChangeAnim(this, ENZOT_ANIM_0, ANIMMODE_LOOP);
    } else {
        if (Player_IsFacingActor(&this->actor, 0x3000, play) && (this->actor.xzDistToPlayer < 100.0f)) {
            Actor_OfferTalk(&this->actor, play, 120.0f);
        }
        this->actor.speed = 1.5f;
        func_80B96FB0(this);
    }
}

void func_80B99384(EnZot* this, PlayState* play) {
}

void EnZot_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZot* this = (EnZot*)thisx;

    Actor_MoveWithGravity(&this->actor);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 15.0f, 30.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

    this->unk_2F2 &= ~0x40;
    if (SkelAnime_Update(&this->skelAnime) && (this->animIndex != ENZOT_ANIM_0)) {
        this->unk_2F2 |= 0x40;
    }

    this->actionFunc(this, play);
    if (!(this->unk_2F2 & 8)) {
        if (!(this->unk_2F2 & 4) && func_80B96DF0(this, play)) {
            Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
        } else {
            Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
            Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
            Math_SmoothStepToS(&this->torsoRot.x, 0, 6, 0x1838, 0x64);
            Math_SmoothStepToS(&this->torsoRot.y, 0, 6, 0x1838, 0x64);
        }
    }

    if (DECR(this->unk_2EE) == 0) {
        this->unk_2EE = Rand_S16Offset(60, 60);
    }

    this->unk_2EC = this->unk_2EE;
    if (this->unk_2EC > 2) {
        this->unk_2EC = 0;
    }
}

Gfx* func_80B99580(GraphicsContext* gfxCtx) {
    Gfx* gfxHead = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));
    Gfx* gfx = gfxHead;

    gSPEndDisplayList(gfx++);

    return gfxHead;
}

s32 EnZot_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnZot* this = (EnZot*)thisx;
    s32 pad;

    if (limbIndex == ZORA_LIMB_HEAD) {
        Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(this->headRot.x, MTXMODE_APPLY);
        Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == ZORA_LIMB_TORSO) {
        Matrix_RotateXS(this->torsoRot.y * -1, MTXMODE_APPLY);
        Matrix_RotateZS(this->torsoRot.x * -1, MTXMODE_APPLY);
    }

    if (((this->animIndex == ENZOT_ANIM_8) || (this->animIndex == ENZOT_ANIM_9)) &&
        ((limbIndex == ZORA_LIMB_TORSO) || (limbIndex == ZORA_LIMB_LEFT_FOREARM) ||
         (limbIndex == ZORA_LIMB_RIGHT_FOREARM))) {
        rot->y += TRUNCF_BINANG(Math_SinS(play->state.frames * ((limbIndex * 50) + 0x814)) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(play->state.frames * ((limbIndex * 50) + 0x940)) * 200.0f);
    }
    return false;
}

void EnZot_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80B99934 = { 400.0f, 0.0f, 0.0f };
    EnZot* this = (EnZot*)thisx;

    if (limbIndex == ZORA_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80B99934, &this->actor.focus.pos);
    }
}

void EnZot_Draw(Actor* thisx, PlayState* play) {
    TexturePtr sp4C[] = {
        gZoraEyeOpenTex,
        gZoraEyeHalfTex,
        gZoraEyeClosedTex,
    };
    EnZot* this = (EnZot*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sp4C[this->unk_2EC]));
    gSPSegment(POLY_OPA_DISP++, 0x0C, func_80B99580(play->state.gfxCtx));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnZot_OverrideLimbDraw, EnZot_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
