/*
 * File: z_en_tsn.c
 * Overlay: ovl_En_Tsn
 * Description: Great Bay - Fisherman
 */

#include "z_en_tsn.h"
#include "z64snap.h"
#include "objects/object_tsn/object_tsn.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnTsn_Init(Actor* thisx, PlayState* play);
void EnTsn_Destroy(Actor* thisx, PlayState* play);
void EnTsn_Update(Actor* thisx, PlayState* play);
void EnTsn_Draw(Actor* thisx, PlayState* play);

void func_80AE0010(EnTsn* this, PlayState* play);
void func_80AE0304(EnTsn* this, PlayState* play);
void func_80AE0418(EnTsn* this, PlayState* play);
void func_80AE0460(EnTsn* this, PlayState* play);
void func_80AE04C4(EnTsn* this, PlayState* play);
void func_80AE04FC(EnTsn* this, PlayState* play);
void func_80AE0704(EnTsn* this, PlayState* play);
void func_80AE0C88(EnTsn* this, PlayState* play);
void func_80AE0D10(EnTsn* this, PlayState* play);
void func_80AE0D78(EnTsn* this, PlayState* play);
void func_80AE0F84(Actor* thisx, PlayState* play);

ActorProfile En_Tsn_Profile = {
    /**/ ACTOR_EN_TSN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TSN,
    /**/ sizeof(EnTsn),
    /**/ EnTsn_Init,
    /**/ EnTsn_Destroy,
    /**/ EnTsn_Update,
    /**/ EnTsn_Draw,
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

EnTsn* func_80ADFCA0(PlayState* play) {
    Actor* npc = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (npc != NULL) {
        if ((npc->id == ACTOR_EN_TSN) && !ENTSN_GET_100(npc)) {
            return (EnTsn*)npc;
        }
        npc = npc->next;
    }

    return NULL;
}

void func_80ADFCEC(EnTsn* this, PlayState* play) {
    this->actionFunc = func_80AE0C88;
    this->actor.update = func_80AE0F84;
    this->actor.destroy = NULL;
    this->actor.draw = NULL;
    this->actor.attentionRangeType = ATTENTION_RANGE_7;

    switch (ENTSN_GET_F(&this->actor)) {
        case ENTSN_F_0:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_26_08)) {
                Actor_Kill(&this->actor);
                return;
            }
            this->actor.textId = 0x106E;
            break;

        case ENTSN_F_1:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_26_04)) {
                this->actor.textId = 0x1091;
            } else {
                this->actor.textId = 0x108A;
            }
            break;

        default:
            break;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
        if ((ENTSN_GET_F(&this->actor)) == ENTSN_F_0) {
            this->actionFunc = func_80AE0D78;
        } else {
            Actor_Kill(&this->actor);
        }
        return;
    }

    this->unk_1D8 = func_80ADFCA0(play);
    this->unk_220 = 0;

    if (this->unk_1D8 == NULL) {
        Actor_Kill(&this->actor);
        return;
    }

    if ((ENTSN_GET_F(&this->actor)) == ENTSN_F_1) {
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    }
}

void EnTsn_Init(Actor* thisx, PlayState* play) {
    EnTsn* this = (EnTsn*)thisx;

    if (ENTSN_GET_100(&this->actor)) {
        func_80ADFCEC(this, play);
        return;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_tsn_Skel_008AB8, &object_tsn_Anim_0092FC, NULL, NULL, 0);
    Animation_PlayLoop(&this->skelAnime, &object_tsn_Anim_0092FC);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_220 = 0;
    this->actionFunc = func_80AE0304;
    this->actor.textId = 0;
    this->actor.velocity.y = 0.0f;
    this->actor.terminalVelocity = -9.0f;
    this->actor.gravity = -1.0f;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
        Actor_Kill(&this->actor);
    }
}

void EnTsn_Destroy(Actor* thisx, PlayState* play) {
    EnTsn* this = (EnTsn*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80ADFF84(EnTsn* this, PlayState* play) {
    u16 textId;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_26_08)) {
        textId = 0x107E;
    } else if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_25_80)) {
            textId = 0x1083;
        } else {
            textId = 0x107F;
        }
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_26_01)) {
        textId = 0x1089;
    } else {
        textId = 0x1084;
    }

    Message_StartTextbox(play, textId, &this->actor);
}

void func_80AE0010(EnTsn* this, PlayState* play) {
    switch (play->msgCtx.currentTextId) {
        case 0x107F:
        case 0x1080:
        case 0x1081:
        case 0x1082:
        case 0x1083:
        case 0x1084:
        case 0x1085:
        case 0x1086:
        case 0x1087:
        case 0x1088:
        case 0x1089:
        case 0x1093:
            Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 6, 0x1838, 0x64);
            this->actor.shape.rot.y = this->actor.world.rot.y;
            break;
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x107F:
            case 0x1081:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x1080:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                Animation_MorphToLoop(&this->skelAnime, &object_tsn_Anim_001198, -10.0f);
                break;

            case 0x1082:
                Animation_MorphToLoop(&this->skelAnime, &object_tsn_Anim_0092FC, -10.0f);
                SET_WEEKEVENTREG(WEEKEVENTREG_25_80);
                Message_CloseTextbox(play);
                this->actionFunc = func_80AE0304;
                this->actor.textId = 0;
                break;

            case 0x1083:
                SET_WEEKEVENTREG(WEEKEVENTREG_25_80);
                Message_CloseTextbox(play);
                this->actionFunc = func_80AE0304;
                this->actor.textId = 0;
                break;

            case 0x1084:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                Animation_MorphToLoop(&this->skelAnime, &object_tsn_Anim_000964, -10.0f);
                break;

            case 0x1085:
            case 0x1086:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x1089:
            case 0x1093:
                SET_WEEKEVENTREG(WEEKEVENTREG_26_01);
                Message_CloseTextbox(play);
                Animation_MorphToLoop(&this->skelAnime, &object_tsn_Anim_0092FC, -10.0f);
                this->actionFunc = func_80AE0304;
                this->actor.textId = 0;
                break;

            case 0x1087:
                Animation_MorphToLoop(&this->skelAnime, &object_tsn_Anim_001198, -10.0f);
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x1088:
                SET_WEEKEVENTREG(WEEKEVENTREG_26_01);
                if (GameInteractor_Should(VB_CONSIDER_MIKAU_HEALED, INV_CONTENT(ITEM_MASK_ZORA) == ITEM_MASK_ZORA,
                                          true)) {
                    Message_CloseTextbox(play);
                    Animation_MorphToLoop(&this->skelAnime, &object_tsn_Anim_0092FC, -10.0f);
                    this->actionFunc = func_80AE0304;
                    this->actor.textId = 0;
                } else {
                    // "Come to think of it, there was a Zora floating lifelessly out in the bay..."
                    Message_ContinueTextbox(play, 0x1093);
                    Animation_MorphToLoop(&this->skelAnime, &object_tsn_Anim_0092FC, -10.0f);
                }
                break;

            case 0x107E:
                Message_CloseTextbox(play);
                this->actionFunc = func_80AE0304;
                this->actor.textId = 0;
                break;
        }
    }
}

void func_80AE0304(EnTsn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80AE0010;
        this->unk_220 |= 1;
        if (this->actor.textId == 0) {
            func_80ADFF84(this, play);
        }
    } else if ((this->actor.xzDistToPlayer < 150.0f) && Player_IsFacingActor(&this->actor, 0x3000, play)) {
        Actor_OfferTalk(&this->actor, play, 160.0f);
        this->unk_220 |= 1;
    } else {
        this->unk_220 &= ~1;
    }
    if (ENTSN_GET_Z(&this->actor)) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 6, 0x1838, 0x64);
    } else {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.home.rot.y, 6, 0x1838, 0x64);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void func_80AE0418(EnTsn* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        Message_StartTextbox(play, 0x107D, NULL);
        Actor_Kill(&this->actor);
    }
}

void func_80AE0460(EnTsn* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        ENTSN_SET_Z(&this->unk_1D8->actor, false);
        this->actionFunc = func_80AE0418;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_SEAHORSE_CAUGHT, 2000.0f, 1000.0f);
    }
}

void func_80AE04C4(EnTsn* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actionFunc = func_80AE0C88;
    }
}

void func_80AE04FC(EnTsn* this, PlayState* play) {
    PlayerItemAction itemAction;
    Player* player = GET_PLAYER(play);

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_PAUSE_MENU) {
        itemAction = func_80123810(play);

        if (itemAction != PLAYER_IA_NONE) {
            SET_WEEKEVENTREG(WEEKEVENTREG_26_02);
        }

        if (itemAction > PLAYER_IA_NONE) {
            Message_CloseTextbox(play);
            this->actionFunc = func_80AE0704;
            if (itemAction == PLAYER_IA_PICTOGRAPH_BOX) {
                if (CHECK_QUEST_ITEM(QUEST_PICTOGRAPH)) {
                    if (Snap_CheckFlag(PICTO_VALID_PIRATE_GOOD)) {
                        player->actor.textId = 0x107B;
                        return;
                    }

                    if (Snap_CheckFlag(PICTO_VALID_PIRATE_TOO_FAR)) {
                        player->actor.textId = 0x10A9;
                        return;
                    }

                    player->actor.textId = 0x1078;
                    this->unk_220 |= 8;
                    return;
                }

                player->actor.textId = 0x1078;
                this->unk_220 |= 8;
                return;
            }

            if (itemAction == PLAYER_IA_HOOKSHOT) {
                player->actor.textId = 0x1075;
                return;
            }

            player->actor.textId = 0x1078;
            this->unk_220 |= 8;
            return;
        }

        if (itemAction <= PLAYER_IA_MINUS1) {
            Message_ContinueTextbox(play, 0x1078);
            Animation_MorphToLoop(&this->unk_1D8->skelAnime, &object_tsn_Anim_001198, -10.0f);
            this->actionFunc = func_80AE0704;
        }
    }
}

void func_80AE0698(EnTsn* this, PlayState* play) {
    Message_CloseTextbox(play);
    this->actionFunc = func_80AE0C88;
    this->unk_220 &= ~2;
    this->actor.focus.pos = this->actor.world.pos;
    CutsceneManager_Stop(this->actor.csId);
    ENTSN_SET_Z(&this->unk_1D8->actor, false);
}

void func_80AE0704(EnTsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad[2];

    if ((this->unk_220 & 8) && (play->msgCtx.currentTextId == 0x1078)) {
        this->unk_220 &= ~8;
        Animation_MorphToLoop(&this->unk_1D8->skelAnime, &object_tsn_Anim_001198, -10.0f);
    }

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            break;

        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x106E:
                        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_25_40)) {
                            Message_ContinueTextbox(play, 0x1074);
                        } else {
                            Message_ContinueTextbox(play, 0x106F);
                        }
                        this->unk_220 |= 2;
                        SET_WEEKEVENTREG(WEEKEVENTREG_25_40);
                        ENTSN_SET_Z(&this->unk_1D8->actor, true);
                        this->unk_220 |= 4;
                        break;

                    case 0x106F:
                    case 0x1070:
                    case 0x1071:
                    case 0x1072:
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x1076:
                    case 0x1079:
                        Animation_MorphToLoop(&this->unk_1D8->skelAnime, &object_tsn_Anim_000964, -10.0f);
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x107A:
                        Message_ContinueTextbox(play, 0x10A6);
                        break;

                    case 0x1075:
                    case 0x1078:
                        player->exchangeItemAction = PLAYER_IA_NONE;
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        Animation_MorphToLoop(&this->unk_1D8->skelAnime, &object_tsn_Anim_0092FC, -10.0f);
                        break;

                    case 0x107C:
                        if (Inventory_HasEmptyBottle()) {
                            SET_WEEKEVENTREG(WEEKEVENTREG_26_08);
                            Message_CloseTextbox(play);
                            this->actionFunc = func_80AE0460;
                            func_80AE0460(this, play);
                            this->unk_220 &= ~2;
                            this->actor.focus.pos = this->actor.world.pos;
                            CutsceneManager_Stop(this->actor.csId);
                            this->actor.flags &= ~ACTOR_FLAG_TALK;
                            REMOVE_QUEST_ITEM(QUEST_PICTOGRAPH);
                        } else {
                            Message_ContinueTextbox(play, 0x10A8);
                        }
                        break;

                    case 0x1073:
                    case 0x1074:
                        Message_ContinueTextbox(play, 0xFF);
                        this->actionFunc = func_80AE04FC;
                        break;

                    case 0x107B:
                        player->exchangeItemAction = PLAYER_IA_NONE;
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        Animation_MorphToLoop(&this->unk_1D8->skelAnime, &object_tsn_Anim_0092FC, -10.0f);
                        break;

                    case 0x1077:
                    case 0x10A6:
                    case 0x10A8:
                        Animation_MorphToLoop(&this->unk_1D8->skelAnime, &object_tsn_Anim_0092FC, -10.0f);
                        func_80AE0698(this, play);
                        this->actor.flags &= ~ACTOR_FLAG_TALK;
                        this->actionFunc = func_80AE04C4;
                        break;

                    case 0x108A:
                    case 0x1091:
                        SET_WEEKEVENTREG(WEEKEVENTREG_26_04);
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        this->unk_220 |= 2;
                        this->actor.textId = 0x1091;
                        break;

                    case 0x108B:
                    case 0x108C:
                    case 0x108D:
                    case 0x108E:
                    case 0x108F:
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x1092:
                        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_26_08)) {
                            func_80AE0698(this, play);
                        } else {
                            Message_ContinueTextbox(play, 0x10A7);
                            Animation_MorphToLoop(&this->unk_1D8->skelAnime, &object_tsn_Anim_000964, -10.0f);
                        }
                        break;

                    case 0x10A7:
                        Animation_MorphToLoop(&this->unk_1D8->skelAnime, &object_tsn_Anim_0092FC, -10.0f);
                        func_80AE0698(this, play);
                        break;

                    case 0x1090:
                        func_80AE0698(this, play);
                        break;

                    case 0x10A9:
                        func_80AE0698(this, play);
                        this->actor.flags &= ~ACTOR_FLAG_TALK;
                        this->actionFunc = func_80AE04C4;
                        break;

                    default:
                        break;
                }
            }
            break;

        default:
            break;
    }

    if (this->unk_220 & 2) {
        if (this->unk_1D8 != NULL) {
            Math_SmoothStepToF(&this->actor.focus.pos.x, this->unk_1D8->actor.focus.pos.x, 0.8f, 100.0f, 5.0f);
            Math_SmoothStepToF(&this->actor.focus.pos.y, this->unk_1D8->actor.focus.pos.y, 0.8f, 100.0f, 5.0f);
            Math_SmoothStepToF(&this->actor.focus.pos.z, this->unk_1D8->actor.focus.pos.z, 0.8f, 100.0f, 5.0f);
        }
    }

    if (this->unk_220 & 4) {
        if (this->actor.csId == CS_ID_NONE) {
            this->unk_220 &= ~4;
        } else if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->actor.csId);
        } else if (CutsceneManager_IsNext(this->actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
            this->unk_220 &= ~4;
        } else {
            CutsceneManager_Queue(this->actor.csId);
        }
    }
}

void func_80AE0C88(EnTsn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80AE0704;
        if ((this->actor.textId == 0x108A) || (this->actor.textId == 0x1091)) {
            this->unk_220 |= 4;
            ENTSN_SET_Z(&this->unk_1D8->actor, true);
        }
    } else if (this->actor.isLockedOn) {
        Actor_OfferTalk(&this->actor, play, 1000.0f);
    }
}

void func_80AE0D10(EnTsn* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->actionFunc = func_80AE0D78;
        CutsceneManager_Stop(this->actor.csId);
    }
}

void func_80AE0D78(EnTsn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80AE0D10;
        this->unk_220 |= 4;
    } else if (this->actor.isLockedOn) {
        Actor_OfferTalk(&this->actor, play, 1000.0f);
    }
}

void EnTsn_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTsn* this = (EnTsn*)thisx;

    this->actionFunc(this, play);

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 25.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    SkelAnime_Update(&this->skelAnime);

    if (this->unk_220 & 1) {
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 6, 0x1838, 0x64);
    }

    if (DECR(this->unk_230) == 0) {
        this->unk_230 = Rand_S16Offset(60, 60);
    }

    if ((this->unk_230 == 1) || (this->unk_230 == 3)) {
        this->unk_22E = 1;
    } else {
        this->unk_22E = 0;
    }
}

void func_80AE0F84(Actor* thisx, PlayState* play) {
    EnTsn* this = (EnTsn*)thisx;

    this->actionFunc(this, play);
}

s32 EnTsn_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnTsn* this = (EnTsn*)thisx;
    s16 shifted = this->headRot.x >> 1;

    if (limbIndex == OBJECT_TSN_LIMB_0F) {
        rot->x += this->headRot.y;
        rot->z += shifted;
    }

    if (limbIndex == OBJECT_TSN_LIMB_08) {
        rot->x += this->torsoRot.y;
        rot->z += shifted;
    }
    return false;
}

void EnTsn_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnTsn* this = (EnTsn*)thisx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == OBJECT_TSN_LIMB_0F) {
        Matrix_MultVec3f(&zeroVec, &this->actor.focus.pos);
    }
}

void EnTsn_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr D_80AE11C8[] = { object_tsn_Tex_0073B8, object_tsn_Tex_0085B8 };
    s32 pad;
    EnTsn* this = (EnTsn*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80AE11C8[this->unk_22E]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(D_80AE11C8[this->unk_22E]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnTsn_OverrideLimbDraw, EnTsn_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
