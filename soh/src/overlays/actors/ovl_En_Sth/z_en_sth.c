/*
 * File: z_en_sth.c
 * Overlay: ovl_En_Sth
 * Description: Uncursed House of Skulltula People
 */

#include "vt.h"
#include "z_en_sth.h"
#include "objects/object_ahg/object_ahg.h"
#include "objects/object_boj/object_boj.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnSth_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSth_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSth_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSth_Update2(Actor* thisx, GlobalContext* globalCtx);
void EnSth_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnSth_WaitForObjectLoaded(EnSth* this, GlobalContext* globalCtx);
void EnSth_ParentRewardObtainedWait(EnSth* this, GlobalContext* globalCtx);
void EnSth_RewardUnobtainedWait(EnSth* this, GlobalContext* globalCtx);
void EnSth_ChildRewardObtainedWait(EnSth* this, GlobalContext* globalCtx);

const ActorInit En_Sth_InitVars = {
    ACTOR_EN_STH,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnSth),
    (ActorFunc)EnSth_Init,
    (ActorFunc)EnSth_Destroy,
    (ActorFunc)EnSth_Update,
    NULL,
    NULL,
};

#include "overlays/ovl_En_Sth/ovl_En_Sth.h"

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

static s16 sObjectIds[6] = {
    OBJECT_AHG, OBJECT_BOJ, OBJECT_BOJ, OBJECT_BOJ, OBJECT_BOJ, OBJECT_BOJ,
};

static FlexSkeletonHeader* sSkeletons[6] = {
    &object_ahg_Skel_0000F0,
    &object_boj_Skel_0000F0,
    &object_boj_Skel_0000F0,
    &object_boj_Skel_0000F0,
    &object_boj_Skel_0000F0,
    &object_boj_Skel_0000F0,
};

static AnimationHeader* sAnimations[6] = {
    &sParentDanceAnim, &sChildDanceAnim, &sChildDanceAnim, &sChildDanceAnim, &sChildDanceAnim, &sChildDanceAnim,
};

static EnSthActionFunc sRewardObtainedWaitActions[6] = {
    EnSth_ParentRewardObtainedWait, EnSth_ChildRewardObtainedWait, EnSth_ChildRewardObtainedWait,
    EnSth_ChildRewardObtainedWait,  EnSth_ChildRewardObtainedWait, EnSth_ChildRewardObtainedWait,
};

static u16 sEventFlags[6] = {
    0x0000, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000,
};

static s16 sGetItemIds[6] = {
    GI_RUPEE_GOLD, GI_WALLET_ADULT, GI_STONE_OF_AGONY, GI_WALLET_GIANT, GI_BOMBCHUS_10, GI_HEART_PIECE,
};

static Vec3f D_80B0B49C = { 700.0f, 400.0f, 0.0f };

static Color_RGB8 sTunicColors[6] = {
    { 190, 110, 0 }, { 0, 180, 110 }, { 0, 255, 80 }, { 255, 160, 60 }, { 190, 230, 250 }, { 240, 230, 120 },
};

void EnSth_SetupAction(EnSth* this, EnSthActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnSth_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSth* this = (EnSth*)thisx;

    s16 objectId;
    s32 params = this->actor.params;
    s32 objectBankIdx;

    osSyncPrintf(VT_FGCOL(BLUE) "金スタル屋 no = %d\n" VT_RST, params); // "Gold Skulltula Shop"
    if (this->actor.params == 0) {
        if (gSaveContext.inventory.gsTokens < 100) {
            Actor_Kill(&this->actor);
            // "Gold Skulltula Shop I still can't be a human"
            osSyncPrintf("金スタル屋 まだ 人間に戻れない \n");
            return;
        }
    } else if (gSaveContext.inventory.gsTokens < (this->actor.params * 10)) {
        Actor_Kill(&this->actor);
        // "Gold Skulltula Shop I still can't be a human"
        osSyncPrintf(VT_FGCOL(BLUE) "金スタル屋 まだ 人間に戻れない \n" VT_RST);
        return;
    }

    objectId = sObjectIds[params];
    if (objectId != 1) {
        objectBankIdx = Object_GetIndex(&globalCtx->objectCtx, objectId);
    } else {
        objectBankIdx = 0;
    }

    osSyncPrintf("bank_ID = %d\n", objectBankIdx);
    if (objectBankIdx < 0) {
        ASSERT(objectBankIdx < 0);
    }
    this->objectBankIdx = objectBankIdx;
    this->drawFunc = EnSth_Draw;
    Actor_SetScale(&this->actor, 0.01f);
    EnSth_SetupAction(this, EnSth_WaitForObjectLoaded);
    this->actor.draw = NULL;
    this->unk_2B2 = 0;
    this->actor.targetMode = 6;
}

void EnSth_SetupShapeColliderUpdate2AndDraw(EnSth* this, GlobalContext* globalCtx) {
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.update = EnSth_Update2;
    this->actor.draw = this->drawFunc;
}

void EnSth_SetupAfterObjectLoaded(EnSth* this, GlobalContext* globalCtx) {
    s32 pad;
    s16* params;

    EnSth_SetupShapeColliderUpdate2AndDraw(this, globalCtx);
    gSegments[6] = PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[this->objectBankIdx].segment);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, sSkeletons[this->actor.params], NULL, this->jointTable,
                       this->morphTable, 16);
    Animation_PlayLoop(&this->skelAnime, sAnimations[this->actor.params]);

    this->eventFlag = sEventFlags[this->actor.params];
    params = &this->actor.params;
    if (gSaveContext.eventChkInf[13] & this->eventFlag) {
        EnSth_SetupAction(this, sRewardObtainedWaitActions[*params]);
    } else {
        EnSth_SetupAction(this, EnSth_RewardUnobtainedWait);
    }
}

void EnSth_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSth* this = (EnSth*)thisx;

    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void EnSth_WaitForObjectLoaded(EnSth* this, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, this->objectBankIdx)) {
        this->actor.objBankIndex = this->objectBankIdx;
        this->actionFunc = EnSth_SetupAfterObjectLoaded;
    }
}

void EnSth_FacePlayer(EnSth* this, GlobalContext* globalCtx) {
    s32 pad;
    s16 diffRot = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(diffRot) <= 0x4000) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, 0xFA0, 0x64);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        func_80038290(globalCtx, &this->actor, &this->headRot, &this->unk_2AC, this->actor.focus.pos);
    } else {
        if (diffRot < 0) {
            Math_SmoothStepToS(&this->headRot.y, -0x2000, 6, 0x1838, 0x100);
        } else {
            Math_SmoothStepToS(&this->headRot.y, 0x2000, 6, 0x1838, 0x100);
        }
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xC, 0x3E8, 0x64);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void EnSth_LookAtPlayer(EnSth* this, GlobalContext* globalCtx) {
    s16 diffRot = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((ABS(diffRot) <= 0x4300) && (this->actor.xzDistToPlayer < 100.0f)) {
        func_80038290(globalCtx, &this->actor, &this->headRot, &this->unk_2AC, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->unk_2AC.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->unk_2AC.y, 0, 6, 0x1838, 0x64);
    }
}

void EnSth_RewardObtainedTalk(EnSth* this, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        if (this->actor.params == 0) {
            EnSth_SetupAction(this, EnSth_ParentRewardObtainedWait);
        } else {
            EnSth_SetupAction(this, EnSth_ChildRewardObtainedWait);
        }
    }
    EnSth_FacePlayer(this, globalCtx);
}

void EnSth_ParentRewardObtainedWait(EnSth* this, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        EnSth_SetupAction(this, EnSth_RewardObtainedTalk);
    } else {
        this->actor.textId = 0x23;
        if (this->actor.xzDistToPlayer < 100.0f) {
            func_8002F2CC(&this->actor, globalCtx, 100.0f);
        }
    }
    EnSth_LookAtPlayer(this, globalCtx);
}

void EnSth_GivePlayerItem(EnSth* this, GlobalContext* globalCtx) {
    u16 getItemId = sGetItemIds[this->actor.params];
    
    if (gSaveContext.n64ddFlag) {
        switch (getItemId) {
            case GI_RUPEE_GOLD:
                break;
            case GI_WALLET_ADULT:
                getItemId = Randomizer_GetItemIdFromKnownCheck(RC_KAK_10_GOLD_SKULLTULA_REWARD, GI_WALLET_ADULT);
                break;
            case GI_STONE_OF_AGONY:
                getItemId = Randomizer_GetItemIdFromKnownCheck(RC_KAK_20_GOLD_SKULLTULA_REWARD, GI_STONE_OF_AGONY);
                break;
            case GI_WALLET_GIANT:
                getItemId = Randomizer_GetItemIdFromKnownCheck(RC_KAK_30_GOLD_SKULLTULA_REWARD, GI_WALLET_GIANT);
                break;
            case GI_BOMBCHUS_10:
                getItemId = Randomizer_GetItemIdFromKnownCheck(RC_KAK_40_GOLD_SKULLTULA_REWARD, GI_BOMBCHUS_10);
                break;
            case GI_HEART_PIECE:
                getItemId = Randomizer_GetItemIdFromKnownCheck(RC_KAK_50_GOLD_SKULLTULA_REWARD, GI_HEART_PIECE);
                break;
        }
    } else {
        switch (this->actor.params) {
            case 1:
            case 3:
                switch (CUR_UPG_VALUE(UPG_WALLET)) {
                    case 0:
                        getItemId = GI_WALLET_ADULT;
                        break;

                    case 1:
                        getItemId = GI_WALLET_GIANT;
                        break;
                }
                break;
        }
    }

    func_8002F434(&this->actor, globalCtx, getItemId, 10000.0f, 50.0f);
}

void EnSth_GiveReward(EnSth* this, GlobalContext* globalCtx) {
    if (Actor_HasParent(&this->actor, globalCtx)) {
        this->actor.parent = NULL;
        EnSth_SetupAction(this, EnSth_RewardObtainedTalk);
        gSaveContext.eventChkInf[13] |= this->eventFlag;
    } else {
        EnSth_GivePlayerItem(this, globalCtx);
    }
    EnSth_FacePlayer(this, globalCtx);
}

void EnSth_RewardUnobtainedTalk(EnSth* this, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        EnSth_SetupAction(this, EnSth_GiveReward);
        EnSth_GivePlayerItem(this, globalCtx);
    }
    EnSth_FacePlayer(this, globalCtx);
}

void EnSth_RewardUnobtainedWait(EnSth* this, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        EnSth_SetupAction(this, EnSth_RewardUnobtainedTalk);
    } else {
        if (this->actor.params == 0) {
            this->actor.textId = 0x28;
        } else {
            this->actor.textId = 0x21;
        }
        if (this->actor.xzDistToPlayer < 100.0f) {
            func_8002F2CC(&this->actor, globalCtx, 100.0f);
        }
    }
    EnSth_LookAtPlayer(this, globalCtx);
}

void EnSth_ChildRewardObtainedWait(EnSth* this, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        EnSth_SetupAction(this, EnSth_RewardObtainedTalk);
    } else {
        if (gSaveContext.inventory.gsTokens < 50) {
            this->actor.textId = 0x20;
        } else {
            this->actor.textId = 0x1F;
        }
        if (this->actor.xzDistToPlayer < 100.0f) {
            func_8002F2CC(&this->actor, globalCtx, 100.0f);
        }
    }
    EnSth_LookAtPlayer(this, globalCtx);
}

void EnSth_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSth* this = (EnSth*)thisx;

    this->actionFunc(this, globalCtx);
}

void EnSth_Update2(Actor* thisx, GlobalContext* globalCtx) {
    EnSth* this = (EnSth*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    if (SkelAnime_Update(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }
    this->actionFunc(this, globalCtx);

    // Likely an unused blink timer and eye index
    if (DECR(this->unk_2B6) == 0) {
        this->unk_2B6 = Rand_S16Offset(0x3C, 0x3C);
    }
    this->unk_2B4 = this->unk_2B6;
    if (this->unk_2B4 >= 3) {
        this->unk_2B4 = 0;
    }
}

s32 EnSth_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSth* this = (EnSth*)thisx;

    s32 temp_v1;

    if (limbIndex == 15) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
        *dList = D_80B0A050;
    }

    if (this->unk_2B2 & 2) {
        this->unk_2B2 &= ~2;
        return 0;
    }

    if ((limbIndex == 8) || (limbIndex == 10) || (limbIndex == 13)) {
        temp_v1 = limbIndex * 0x32;
        rot->y += (Math_SinS(globalCtx->state.frames * (temp_v1 + 0x814)) * 200.0f);
        rot->z += (Math_CosS(globalCtx->state.frames * (temp_v1 + 0x940)) * 200.0f);
    }
    return 0;
}

void EnSth_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnSth* this = (EnSth*)thisx;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&D_80B0B49C, &this->actor.focus.pos);
        if (this->actor.params != 0) { // Children
            OPEN_DISPS(globalCtx->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, D_80B0A3C0);

            CLOSE_DISPS(globalCtx->state.gfxCtx);
        }
    }
}

Gfx* EnSth_AllocColorDList(GraphicsContext* globalCtx, u8 envR, u8 envG, u8 envB, u8 envA) {
    Gfx* dList;

    dList = Graph_Alloc(globalCtx, 2 * sizeof(Gfx));
    gDPSetEnvColor(dList, envR, envG, envB, envA);
    gSPEndDisplayList(dList + 1);

    return dList;
}

void EnSth_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnSth* this = (EnSth*)thisx;
    Color_RGB8* envColor1;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    gSegments[6] = PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[this->objectBankIdx].segment);
    func_800943C8(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               EnSth_AllocColorDList(globalCtx->state.gfxCtx, sTunicColors[this->actor.params].r,
                                     sTunicColors[this->actor.params].g, sTunicColors[this->actor.params].b, 255));

    if (this->actor.params == 0) {
        gSPSegment(POLY_OPA_DISP++, 0x09, EnSth_AllocColorDList(globalCtx->state.gfxCtx, 190, 110, 0, 255));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x09, EnSth_AllocColorDList(globalCtx->state.gfxCtx, 90, 110, 130, 255));
    }
    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnSth_OverrideLimbDraw, EnSth_PostLimbDraw, &this->actor);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
