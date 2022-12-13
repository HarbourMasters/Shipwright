/*
 * File: z_en_mk.c
 * Overlay: ovl_En_Mk
 * Description: Lakeside Professor
 */

#include "z_en_mk.h"
#include "objects/object_mk/object_mk.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnMk_Init(Actor* thisx, PlayState* play);
void EnMk_Destroy(Actor* thisx, PlayState* play);
void EnMk_Update(Actor* thisx, PlayState* play);
void EnMk_Draw(Actor* thisx, PlayState* play);

void EnMk_Wait(EnMk* this, PlayState* play);

const ActorInit En_Mk_InitVars = {
    ACTOR_EN_MK,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MK,
    sizeof(EnMk),
    (ActorFunc)EnMk_Init,
    (ActorFunc)EnMk_Destroy,
    (ActorFunc)EnMk_Update,
    (ActorFunc)EnMk_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

void EnMk_Init(Actor* thisx, PlayState* play) {
    EnMk* this = (EnMk*)thisx;
    s32 swimFlag;

    this->actor.minVelocityY = -4.0f;
    this->actor.gravity = -1.0f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_mk_Skel_005DF0, &object_mk_Anim_000D88, this->jointTable,
                       this->morphTable, 13);
    Animation_PlayLoop(&this->skelAnime, &object_mk_Anim_000D88);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = 0xFF;
    Actor_SetScale(&this->actor, 0.01f);

    this->actionFunc = EnMk_Wait;
    this->flags = 0;
    this->swimFlag = 0;
    this->actor.targetMode = 6;

    if (gSaveContext.itemGetInf[1] & 1) {
        this->flags |= 4;
    }
}

void EnMk_Destroy(Actor* thisx, PlayState* play) {
    EnMk* this = (EnMk*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80AACA40(EnMk* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actor.flags &= ~ACTOR_FLAG_16;
        this->actionFunc = EnMk_Wait;
    }

    this->flags |= 1;
}

void func_80AACA94(EnMk* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play) != 0) {
        this->actor.parent = NULL;
        this->actionFunc = func_80AACA40;
        if (!gSaveContext.n64ddFlag) {
            func_80088AA0(240);
            gSaveContext.eventInf[1] &= ~1;
        }
    } else {
        if (gSaveContext.n64ddFlag) {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LH_TRADE_FROG, GI_EYEDROPS);
            Randomizer_ConsumeAdultTradeItem(play, ITEM_FROG);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 10000.0f, 50.0f);
            Flags_SetRandomizerInf(RAND_INF_ADULT_TRADES_LH_TRADE_FROG);
        } else {
            s32 getItemID = GI_EYEDROPS;
            func_8002F434(&this->actor, play, getItemID, 10000.0f, 50.0f);
        }
    }
}

void func_80AACB14(EnMk* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actionFunc = func_80AACA94;
        if (gSaveContext.n64ddFlag) {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LH_TRADE_FROG, GI_EYEDROPS);
            Randomizer_ConsumeAdultTradeItem(play, ITEM_FROG);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 10000.0f, 50.0f);
            Flags_SetRandomizerInf(RAND_INF_ADULT_TRADES_LH_TRADE_FROG);
        } else {
            s32 getItemID = GI_EYEDROPS;
            func_8002F434(&this->actor, play, getItemID, 10000.0f, 50.0f);
        }
    }
}

void func_80AACB6C(EnMk* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = func_80AACB14;
    }

    this->flags |= 1;
}

void func_80AACBAC(EnMk* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
        this->actor.shape.rot.y -= 0x800;
    } else {
        this->actionFunc = func_80AACB6C;
        Message_ContinueTextbox(play, 0x4030);
    }
}

void func_80AACC04(EnMk* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        this->timer = gSaveContext.n64ddFlag ? 0 : 16;
        this->actionFunc = func_80AACBAC;
        Animation_Change(&this->skelAnime, &object_mk_Anim_000D88, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_mk_Anim_000D88), ANIMMODE_LOOP, -4.0f);
        this->flags &= ~2;
    }
}

void func_80AACCA0(EnMk* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
        this->actor.shape.rot.y += 0x800;
    } else {
        this->timer = gSaveContext.n64ddFlag ? 0 : 120;
        this->actionFunc = func_80AACC04;
        Animation_Change(&this->skelAnime, &object_mk_Anim_000724, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_mk_Anim_000724), ANIMMODE_LOOP, -4.0f);
        this->flags &= ~2;
    }
}

void func_80AACD48(EnMk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->actionFunc = func_80AACCA0;
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        player->exchangeItemId = EXCH_ITEM_NONE;
        this->timer = gSaveContext.n64ddFlag ? 0 : 16;
        Animation_Change(&this->skelAnime, &object_mk_Anim_000D88, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_mk_Anim_000D88), ANIMMODE_LOOP, -4.0f);
        this->flags &= ~2;
    }

    this->flags |= 1;
}

void func_80AACE2C(EnMk* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_ContinueTextbox(play, 0x4001);
        Animation_Change(&this->skelAnime, &object_mk_Anim_000AC0, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_mk_Anim_000AC0), ANIMMODE_ONCE, -4.0f);
        this->flags &= ~2;
        this->actionFunc = func_80AACD48;
    }

    this->flags |= 1;
}

void func_80AACEE8(EnMk* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_ContinueTextbox(play, 0x4000);
        Animation_Change(&this->skelAnime, &object_mk_Anim_000AC0, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_mk_Anim_000AC0), ANIMMODE_LOOP, -4.0f);
        this->flags &= ~2;
        this->actionFunc = func_80AACE2C;
    }

    this->flags |= 1;
}

void func_80AACFA0(EnMk* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_80AACA40;
        gSaveContext.itemGetInf[1] |= 1;
    } else {
        // not sure when/how/if this is getting called
        if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, play, GI_HEART_PIECE, 10000.0f, 50.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LH_LAB_DIVE, GI_HEART_PIECE);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 10000.0f, 50.0f);
        }
    }
}

void func_80AAD014(EnMk* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actionFunc = func_80AACFA0;
        if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, play, GI_HEART_PIECE, 10000.0f, 50.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LH_LAB_DIVE, GI_HEART_PIECE);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 10000.0f, 50.0f);
        }
    }

    this->flags |= 1;
}

void EnMk_Wait(EnMk* this, PlayState* play) {
    s16 angle;
    s32 swimFlag;
    Player* player = GET_PLAYER(play);
    s32 playerExchangeItem;

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        playerExchangeItem = func_8002F368(play);

        if (this->actor.textId != 0x4018) {
            player->actor.textId = this->actor.textId;
            this->actionFunc = func_80AACA40;
        } else {
            // Skip eye drop text on rando if Link went in the water, so you can still receive the dive check
            if (INV_CONTENT(ITEM_ODD_MUSHROOM) == ITEM_EYEDROPS &&
                (!gSaveContext.n64ddFlag || this->swimFlag == 0)) {
                player->actor.textId = 0x4032;
                this->actionFunc = func_80AACA40;
            } else {
                switch (playerExchangeItem) {
                    case EXCH_ITEM_NONE:
                        if (this->swimFlag >= 8) {
                            if (gSaveContext.itemGetInf[1] & 1) {
                                player->actor.textId = 0x4075;
                                this->actionFunc = func_80AACA40;
                            } else {
                                player->actor.textId = 0x4074;
                                this->actionFunc = func_80AAD014;
                                this->swimFlag = 0;
                            }
                        } else {
                            if (this->swimFlag == 0) {
                                player->actor.textId = 0x4018;
                                this->actionFunc = func_80AACA40;
                            } else {
                                player->actor.textId = 0x406C + this->swimFlag;
                                this->actionFunc = func_80AACA40;
                            }
                        }
                        break;
                    case EXCH_ITEM_FROG:
                        player->actor.textId = 0x4019;
                        this->actionFunc = func_80AACEE8;
                        Animation_Change(&this->skelAnime, &object_mk_Anim_000368, 1.0f, 0.0f,
                                         Animation_GetLastFrame(&object_mk_Anim_000368), ANIMMODE_ONCE, -4.0f);
                        this->flags &= ~2;
                        gSaveContext.timer2State = 0;
                        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                        break;
                    default:
                        player->actor.textId = 0x4018;
                        this->actionFunc = func_80AACA40;
                        break;
                }
            }
        }
    } else {
        this->actor.textId = Text_GetFaceReaction(play, 0x1A);

        if (this->actor.textId == 0) {
            this->actor.textId = 0x4018;
        }

        angle = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

        if ((ABS(angle) < 0x2151) && (this->actor.xzDistToPlayer < 100.0f)) {
            func_8002F298(&this->actor, play, 100.0f, EXCH_ITEM_FROG);
            this->flags |= 1;
        }
    }
}

void EnMk_Update(Actor* thisx, PlayState* play) {
    EnMk* this = (EnMk*)thisx;
    s32 pad;
    Vec3s vec;
    Player* player;
    s16 swimFlag;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);

    if ((!(this->flags & 2)) && (SkelAnime_Update(&this->skelAnime))) {
        this->flags |= 2;
    }

    this->actionFunc(this, play);

    if (this->flags & 1) {
        func_80038290(play, &this->actor, &this->headRotation, &vec, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRotation.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->headRotation.y, 0, 6, 6200, 100);
    }

    player = GET_PLAYER(play);

    if (this->flags & 8) {
        if (!(player->stateFlags2 & 0x400)) {
            this->flags &= ~8;
        }
    } else {
        if (player->currentBoots == PLAYER_BOOTS_IRON) {
            this->flags |= 8;
        } else if (player->stateFlags2 & 0x400) {
            swimFlag = player->actor.yDistToWater;

            if (swimFlag > 0) {
                if (swimFlag >= 320) {
                    if (swimFlag >= 355) {
                        swimFlag = 8;
                    } else {
                        swimFlag = 7;
                    }
                } else if (swimFlag < 80) {
                    swimFlag = 1;
                } else {
                    swimFlag *= 0.025f;
                }

                if (this->swimFlag < swimFlag) {
                    this->swimFlag = swimFlag;

                    if ((!(this->flags & 4)) && (this->swimFlag >= 8)) {
                        this->flags |= 4;
                        func_80078884(NA_SE_SY_CORRECT_CHIME);
                    }
                }
            }
        }
    }
}

s32 EnMk_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMk* this = (EnMk*)thisx;

    if (limbIndex == 11) {
        rot->y -= this->headRotation.y;
        rot->z += this->headRotation.x;
    }

    return false;
}

void EnMk_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80AAD64C = { 1000.0f, -100.0f, 0.0f };
    EnMk* this = (EnMk*)thisx;

    if (limbIndex == 11) {
        Matrix_MultVec3f(&D_80AAD64C, &this->actor.focus.pos);
    }
}

void EnMk_Draw(Actor* thisx, PlayState* play) {
    EnMk* this = (EnMk*)thisx;

    Gfx_SetupDL_37Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMk_OverrideLimbDraw, EnMk_PostLimbDraw, &this->actor);
}
