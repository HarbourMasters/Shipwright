/*
 * File: z_en_hs.c
 * Overlay: ovl_En_Hs
 * Description: Carpenter's Son
 */

#include "z_en_hs.h"
#include "vt.h"
#include "objects/object_hs/object_hs.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnHs_Init(Actor* thisx, PlayState* play);
void EnHs_Destroy(Actor* thisx, PlayState* play);
void EnHs_Update(Actor* thisx, PlayState* play);
void EnHs_Draw(Actor* thisx, PlayState* play);

void func_80A6E9AC(EnHs* this, PlayState* play);
void func_80A6E6B0(EnHs* this, PlayState* play);

const ActorInit En_Hs_InitVars = {
    ACTOR_EN_HS,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_HS,
    sizeof(EnHs),
    (ActorFunc)EnHs_Init,
    (ActorFunc)EnHs_Destroy,
    (ActorFunc)EnHs_Update,
    (ActorFunc)EnHs_Draw,
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
    { 40, 40, 0, { 0, 0, 0 } },
};

void func_80A6E3A0(EnHs* this, EnHsActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnHs_Init(Actor* thisx, PlayState* play) {
    EnHs* this = (EnHs*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_hs_Skel_006260, &object_hs_Anim_0005C0, this->jointTable,
                       this->morphTable, 16);
    Animation_PlayLoop(&this->skelAnime, &object_hs_Anim_0005C0);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);

    if (!LINK_IS_ADULT) {
        this->actor.params = 0;
    } else {
        this->actor.params = 1;
    }

    if (this->actor.params == 1) {
        // "chicken shop (adult era)"
        osSyncPrintf(VT_FGCOL(CYAN) " ヒヨコの店(大人の時) \n" VT_RST);
        func_80A6E3A0(this, func_80A6E9AC);
        bool shouldSpawn;
        bool tradedMushroom = gSaveContext.itemGetInf[3] & 1;
        if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_ADULT_TRADE)) {
            // To explain the logic because Fado and Grog are linked:
            // - If you have Cojiro, then spawn Grog and not Fado.
            // - If you don't have Cojiro but do have Odd Potion, spawn Fado and not Grog.
            // - If you don't have either, spawn Grog if you haven't traded the Odd Mushroom.
            // - If you don't have either but have traded the mushroom, don't spawn either.
            if (PLAYER_HAS_SHUFFLED_ADULT_TRADE_ITEM(ITEM_COJIRO)) {
                shouldSpawn = true;
            } else if (PLAYER_HAS_SHUFFLED_ADULT_TRADE_ITEM(ITEM_ODD_POTION)) {
                shouldSpawn = false;
            } else {
                shouldSpawn = !tradedMushroom;
            }
        } else {
            shouldSpawn = !tradedMushroom;
        }
        if (!shouldSpawn) {
            // "chicken shop closed"
            osSyncPrintf(VT_FGCOL(CYAN) " ヒヨコ屋閉店 \n" VT_RST);
            Actor_Kill(&this->actor);
        }
    } else {
        // "chicken shop (child era)"
        osSyncPrintf(VT_FGCOL(CYAN) " ヒヨコの店(子人の時) \n" VT_RST);
        func_80A6E3A0(this, func_80A6E9AC);
    }

    this->unk_2A8 = 0;
    this->actor.targetMode = 6;
}

void EnHs_Destroy(Actor* thisx, PlayState* play) {
    EnHs* this = (EnHs*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_80A6E53C(EnHs* this, PlayState* play, u16 textId, EnHsActionFunc actionFunc) {
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        func_80A6E3A0(this, actionFunc);
        return 1;
    }

    this->actor.textId = textId;
    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if ((ABS(yawDiff) <= 0x2150) && (this->actor.xzDistToPlayer < 100.0f)) {
        this->unk_2A8 |= 1;
        func_8002F2CC(&this->actor, play, 100.0f);
    }

    return 0;
}

void func_80A6E5EC(EnHs* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        func_80A6E3A0(this, func_80A6E6B0);
    }

    this->unk_2A8 |= 1;
}

void func_80A6E630(EnHs* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        if (!gSaveContext.n64ddFlag) {
            func_80088AA0(180);
        }
        func_80A6E3A0(this, func_80A6E6B0);
        gSaveContext.eventInf[1] &= ~1;
    }

    this->unk_2A8 |= 1;
}

void func_80A6E6B0(EnHs* this, PlayState* play) {
    func_80A6E53C(this, play, 0x10B6, func_80A6E5EC);
}

void func_80A6E6D8(EnHs* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        func_80A6E3A0(this, func_80A6E9AC);
    }
}

void func_80A6E70C(EnHs* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        func_80A6E3A0(this, func_80A6E9AC);
    }
}

void func_80A6E740(EnHs* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        func_80A6E3A0(this, func_80A6E630);
    } else {
        if (gSaveContext.n64ddFlag) {
            GetItemEntry itemEntry = Randomizer_GetItemFromKnownCheck(RC_LW_TRADE_COJIRO, GI_ODD_MUSHROOM);
            Randomizer_ConsumeAdultTradeItem(play, ITEM_COJIRO);
            GiveItemEntryFromActor(&this->actor, play, itemEntry, 10000.0f, 50.0f);
            Flags_SetRandomizerInf(RAND_INF_ADULT_TRADES_LW_TRADE_COJIRO);
        } else {
            s32 itemId = GI_ODD_MUSHROOM;
            func_8002F434(&this->actor, play, itemId, 10000.0f, 50.0f);
        }
    }

    this->unk_2A8 |= 1;
}

void func_80A6E7BC(EnHs* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                func_80A6E3A0(this, func_80A6E740);
                if (gSaveContext.n64ddFlag) {
                    GetItemEntry itemEntry = Randomizer_GetItemFromKnownCheck(RC_LW_TRADE_COJIRO, GI_ODD_MUSHROOM);
                    Randomizer_ConsumeAdultTradeItem(play, ITEM_COJIRO);
                    GiveItemEntryFromActor(&this->actor, play, itemEntry, 10000.0f, 50.0f);
                    Flags_SetRandomizerInf(RAND_INF_ADULT_TRADES_LW_TRADE_COJIRO);
                } else {
                    s32 itemId = GI_ODD_MUSHROOM;
                    func_8002F434(&this->actor, play, itemId, 10000.0f, 50.0f);
                }
                break;
            case 1:
                Message_ContinueTextbox(play, 0x10B4);
                func_80A6E3A0(this, func_80A6E70C);
                break;
        }

        Animation_Change(&this->skelAnime, &object_hs_Anim_0005C0, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_hs_Anim_0005C0), ANIMMODE_LOOP, 8.0f);
    }

    this->unk_2A8 |= 1;
}

void func_80A6E8CC(EnHs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_ContinueTextbox(play, 0x10B3);
        func_80A6E3A0(this, func_80A6E7BC);
        Animation_Change(&this->skelAnime, &object_hs_Anim_000528, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_hs_Anim_000528), ANIMMODE_LOOP, 8.0f);
    }

    if (this->unk_2AA > 0) {
        this->unk_2AA--;
        if (this->unk_2AA == 0) {
            func_8002F7DC(&player->actor, NA_SE_EV_CHICKEN_CRY_M);
        }
    }

    this->unk_2A8 |= 1;
}

void func_80A6E9AC(EnHs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if (func_8002F368(play) == 7) {
            player->actor.textId = 0x10B2;
            func_80A6E3A0(this, func_80A6E8CC);
            Animation_Change(&this->skelAnime, &object_hs_Anim_000304, 1.0f, 0.0f,
                             Animation_GetLastFrame(&object_hs_Anim_000304), ANIMMODE_LOOP, 8.0f);
            this->unk_2AA = 40;
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        } else {
            player->actor.textId = 0x10B1;
            func_80A6E3A0(this, func_80A6E6D8);
        }
    } else {
        yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        this->actor.textId = 0x10B1;
        if ((ABS(yawDiff) <= 0x2150) && (this->actor.xzDistToPlayer < 100.0f)) {
            func_8002F298(&this->actor, play, 100.0f, 7);
        }
    }
}

void EnHs_Update(Actor* thisx, PlayState* play) {
    EnHs* this = (EnHs*)thisx;
    s32 pad;

    Collider_UpdateCylinder(thisx, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    if (SkelAnime_Update(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }

    this->actionFunc(this, play);

    if (this->unk_2A8 & 1) {
        func_80038290(play, &this->actor, &this->unk_29C, &this->unk_2A2, this->actor.focus.pos);
        this->unk_2A8 &= ~1;
    } else {
        Math_SmoothStepToS(&this->unk_29C.x, 12800, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_29C.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_2A2.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_2A2.y, 0, 6, 6200, 100);
    }
}

s32 EnHs_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHs* this = (EnHs*)thisx;

    switch (limbIndex) {
        case 9:
            rot->x += this->unk_29C.y;
            rot->z += this->unk_29C.x;
            break;
        case 10:
            *dList = NULL;
            return false;
        case 11:
            *dList = NULL;
            return false;
        case 12:
            if (this->actor.params == 1) {
                *dList = NULL;
                return false;
            }
            break;
        case 13:
            if (this->actor.params == 1) {
                *dList = NULL;
                return false;
            }
            break;
    }
    return false;
}

void EnHs_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80A6EDFC = { 300.0f, 1000.0f, 0.0f };
    EnHs* this = (EnHs*)thisx;

    if (limbIndex == 9) {
        Matrix_MultVec3f(&D_80A6EDFC, &this->actor.focus.pos);
    }
}

void EnHs_Draw(Actor* thisx, PlayState* play) {
    EnHs* this = (EnHs*)thisx;

    Gfx_SetupDL_37Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnHs_OverrideLimbDraw, EnHs_PostLimbDraw, this);
}
