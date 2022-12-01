/*
 * File: z_en_tory.c
 * Overlay: ovl_En_Toryo
 * Description: Boss Carpenter
 */

#include "z_en_toryo.h"
#include "objects/object_toryo/object_toryo.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnToryo_Init(Actor* thisx, PlayState* play);
void EnToryo_Destroy(Actor* thisx, PlayState* play);
void EnToryo_Update(Actor* thisx, PlayState* play);
void EnToryo_Draw(Actor* thisx, PlayState* play);

void func_80B20914(EnToryo* this, PlayState* play);
s32 EnToryo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
void EnToryo_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx);

const ActorInit En_Toryo_InitVars = {
    ACTOR_EN_TORYO,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_TORYO,
    sizeof(EnToryo),
    (ActorFunc)EnToryo_Init,
    (ActorFunc)EnToryo_Destroy,
    (ActorFunc)EnToryo_Update,
    (ActorFunc)EnToryo_Draw,
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
    { 18, 63, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static AnimationSpeedInfo sEnToryoAnimation = { &object_toryo_Anim_000E50, 1.0f, 0, 0 };

static Vec3f sMultVec = { 800.0f, 1000.0f, 0.0f };

void EnToryo_Init(Actor* thisx, PlayState* play) {
    EnToryo* this = (EnToryo*)thisx;
    s32 pad;

    switch (play->sceneNum) {
        case SCENE_SPOT09:
            if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
                this->stateFlags |= 1;
            }
            break;
        case SCENE_SPOT01:
            if ((LINK_AGE_IN_YEARS == YEARS_CHILD) && IS_DAY) {
                this->stateFlags |= 2;
            }
            break;
        case SCENE_KAKARIKO:
            if ((LINK_AGE_IN_YEARS == YEARS_CHILD) && IS_NIGHT) {
                this->stateFlags |= 4;
            }
            break;
    }

    if ((this->stateFlags & 7) == 0) {
        Actor_Kill(&this->actor);
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 42.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_toryo_Skel_007150, NULL, this->jointTable, this->morphTable,
                       17);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    Animation_Change(&this->skelAnime, sEnToryoAnimation.animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sEnToryoAnimation.animation), sEnToryoAnimation.mode,
                     sEnToryoAnimation.morphFrames);
    this->stateFlags |= 8;
    this->actor.targetMode = 6;
    this->actionFunc = func_80B20914;
}

void EnToryo_Destroy(Actor* thisx, PlayState* play) {
    EnToryo* this = (EnToryo*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_80B203D8(EnToryo* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 ret = 1;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_EVENT:
            ret = 1;
            break;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                if (play->msgCtx.choiceIndex == 0) {
                    Message_CloseTextbox(play);
                    this->actor.parent = NULL;
                    player->exchangeItemId = EXCH_ITEM_NONE;
                    play->msgCtx.msgMode = MSGMODE_PAUSED;
                    this->actor.textId = 0x601B;
                    ret = 3;
                } else {
                    this->actor.textId = 0x606F;
                    ret = 2;
                }
            }
            break;
        case TEXT_STATE_DONE:
            switch (this->actor.textId) {
                case 0x5028:
                    ret = 1;
                    if (Message_ShouldAdvance(play)) {
                        gSaveContext.infTable[23] |= 4;
                        ret = 0;
                    }
                    break;
                case 0x601B:
                    ret = 1;
                    if (Message_ShouldAdvance(play)) {
                        ret = 4;
                    }
                    break;
                case 0x606F:
                    ret = 1;
                    if (Message_ShouldAdvance(play)) {
                        gSaveContext.infTable[23] |= 2;
                        ret = 0;
                    }
                    break;
                case 0x606A:
                    ret = 1;
                    if (Message_ShouldAdvance(play)) {
                        gSaveContext.infTable[23] |= 1;
                        ret = 0;
                    }
                    break;
                case 0x606B:
                case 0x606C:
                case 0x606D:
                case 0x606E:
                default:
                    ret = 1;
                    if (Message_ShouldAdvance(play)) {
                        ret = 0;
                    }
                    break;
            }
            break;
    }
    return ret;
}

s32 func_80B205CC(EnToryo* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 ret = 5;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            ret = 5;
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                ret = 0;
            }
            break;
    }
    return ret;
}

u32 func_80B20634(EnToryo* this, PlayState* play) {
    u32 ret;

    if (this->unk_1E0 != 0) {
        if (this->unk_1E0 == 10) {
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            if (gSaveContext.infTable[23] & 2) {
                ret = 0x606E;
            } else {
                ret = 0x606D;
            }
        } else {
            ret = 0x200F;
        }
    }
    //! @bug return value may be unitialized
    return ret;
}

s32 func_80B206A0(EnToryo* this, PlayState* play) {
    s32 textId = Text_GetFaceReaction(play, 0);
    s32 ret = textId;

    if (textId == 0) {
        if ((this->stateFlags & 1)) {
            if ((gSaveContext.eventChkInf[9] & 0xF) == 0xF) {
                ret = 0x606C;
            } else if ((gSaveContext.infTable[23] & 1)) {
                ret = 0x606B;
            } else {
                ret = 0x606A;
            }
        } else if ((this->stateFlags & 2)) {
            if ((gSaveContext.infTable[23] & 4)) {
                ret = 0x5029;
            } else {
                ret = 0x5028;
            }
        } else {
            ret = textId;
            if ((this->stateFlags & 4)) {
                ret = 0x506C;
            }
        }
    }
    return ret;
}

void func_80B20768(EnToryo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 sp32;
    s16 sp30;

    if (this->unk_1E4 == 3) {
        Actor_ProcessTalkRequest(&this->actor, play);
        Message_ContinueTextbox(play, this->actor.textId);
        this->unk_1E4 = 1;
    }

    if (this->unk_1E4 == 1) {
        this->unk_1E4 = func_80B203D8(this, play);
    }

    if (this->unk_1E4 == 5) {
        this->unk_1E4 = func_80B205CC(this, play);
        return;
    }

    if (this->unk_1E4 == 2) {
        Message_ContinueTextbox(play, this->actor.textId);
        this->unk_1E4 = 1;
    }

    if (this->unk_1E4 == 4) {
        if (Actor_HasParent(&this->actor, play)) {
            this->actor.parent = NULL;
            this->unk_1E4 = 5;
        } else {
            if (gSaveContext.n64ddFlag) {
                GetItemEntry itemEntry = Randomizer_GetItemFromKnownCheck(RC_GV_TRADE_SAW, GI_SWORD_BROKEN);
                Randomizer_ConsumeAdultTradeItem(play, ITEM_SAW);
                GiveItemEntryFromActor(&this->actor, play, itemEntry, 100.0f, 10.0f);
                Flags_SetRandomizerInf(RAND_INF_ADULT_TRADES_GV_TRADE_SAW);
            } else {
                s32 itemId = GI_SWORD_BROKEN;
                func_8002F434(&this->actor, play, itemId, 100.0f, 10.0f);
            }
        }
        return;
    }

    if (this->unk_1E4 == 0) {
        if (Actor_ProcessTalkRequest(&this->actor, play)) {
            this->unk_1E0 = func_8002F368(play);
            if (this->unk_1E0 != 0) {
                player->actor.textId = func_80B20634(this, play);
                this->actor.textId = player->actor.textId;
            }
            this->unk_1E4 = 1;
            return;
        }

        Actor_GetScreenPos(play, &this->actor, &sp32, &sp30);
        if ((sp32 >= 0) && (sp32 < 0x141) && (sp30 >= 0) && (sp30 < 0xF1)) {
            this->actor.textId = func_80B206A0(this, play);
            func_8002F298(&this->actor, play, 100.0f, 10);
        }
    }
}

void func_80B20914(EnToryo* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    func_80B20768(this, play);
    if (this->unk_1E4 != 0) {
        this->stateFlags |= 0x10;
    } else {
        this->stateFlags &= ~0x10;
    }
}

void EnToryo_Update(Actor* thisx, PlayState* play) {
    EnToryo* this = (EnToryo*)thisx;
    ColliderCylinder* collider = &this->collider;
    Player* player = GET_PLAYER(play);
    f32 rot;

    Collider_UpdateCylinder(thisx, collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, (Collider*)collider);

    this->actionFunc(this, play);

    if ((this->stateFlags & 8)) {
        this->unk_1EC.unk_18.x = player->actor.focus.pos.x;
        this->unk_1EC.unk_18.y = player->actor.focus.pos.y;
        this->unk_1EC.unk_18.z = player->actor.focus.pos.z;

        if ((this->stateFlags & 0x10)) {
            func_80034A14(thisx, &this->unk_1EC, 0, 4);
            return;
        }

        rot = thisx->yawTowardsPlayer - thisx->shape.rot.y;
        if ((rot < 14563.0f) && (rot > -14563.0f)) {
            func_80034A14(thisx, &this->unk_1EC, 0, 2);
        } else {
            func_80034A14(thisx, &this->unk_1EC, 0, 1);
        }
    }
}

void EnToryo_Draw(Actor* thisx, PlayState* play) {
    EnToryo* this = (EnToryo*)thisx;

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnToryo_OverrideLimbDraw, EnToryo_PostLimbDraw, this);
}

s32 EnToryo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                             void* thisx) {
    EnToryo* this = (EnToryo*)thisx;

    if ((this->stateFlags & 8)) {
        switch (limbIndex) {
            case 8:
                rot->x += this->unk_1EC.unk_0E.y;
                rot->y -= this->unk_1EC.unk_0E.x;
                break;
            case 15:
                rot->x += this->unk_1EC.unk_08.y;
                rot->z += this->unk_1EC.unk_08.x;
                break;
        }
    }
    return 0;
}

void EnToryo_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnToryo* this = (EnToryo*)thisx;

    switch (limbIndex) {
        case 15:
            Matrix_MultVec3f(&sMultVec, &this->actor.focus.pos);
            break;
    }
}
