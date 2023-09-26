/*
 * File: z_en_js.c
 * Overlay: ovl_En_Js
 * Description: Magic Carpet Salesman
 */

#include "z_en_js.h"
#include "objects/object_js/object_js.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY)

void EnJs_Init(Actor* thisx, PlayState* play);
void EnJs_Destroy(Actor* thisx, PlayState* play);
void EnJs_Update(Actor* thisx, PlayState* play);
void EnJs_Draw(Actor* thisx, PlayState* play);

void func_80A89304(EnJs* this, PlayState* play);

const ActorInit En_Js_InitVars = {
    ACTOR_EN_JS,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_JS,
    sizeof(EnJs),
    (ActorFunc)EnJs_Init,
    (ActorFunc)EnJs_Destroy,
    (ActorFunc)EnJs_Update,
    (ActorFunc)EnJs_Draw,
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

void En_Js_SetupAction(EnJs* this, EnJsActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnJs_Init(Actor* thisx, PlayState* play) {
    EnJs* this = (EnJs*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gCarpetMerchantSkel, &gCarpetMerchantSlappingKneeAnim,
                       this->jointTable, this->morphTable, 13);
    Animation_PlayOnce(&this->skelAnime, &gCarpetMerchantSlappingKneeAnim);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    En_Js_SetupAction(this, func_80A89304);
    this->unk_284 = 0;
    this->actor.gravity = -1.0f;
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_JSJUTAN, this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
}

void EnJs_Destroy(Actor* thisx, PlayState* play) {
    EnJs* this = (EnJs*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

u8 func_80A88F64(EnJs* this, PlayState* play, u16 textId) {
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        return 1;
    } else {
        this->actor.textId = textId;
        yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

        if (ABS(yawDiff) <= 0x1800 && this->actor.xzDistToPlayer < 100.0f) {
            this->unk_284 |= 1;
            func_8002F2CC(&this->actor, play, 100.0f);
        }
        return 0;
    }
}

void func_80A89008(EnJs* this) {
    En_Js_SetupAction(this, func_80A89304);
    Animation_Change(&this->skelAnime, &gCarpetMerchantSlappingKneeAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gCarpetMerchantSlappingKneeAnim), ANIMMODE_ONCE, -4.0f);
}

void func_80A89078(EnJs* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        func_80A89008(this);
        this->actor.flags &= ~ACTOR_FLAG_WILL_TALK;
    }
}

void func_80A890C0(EnJs* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        En_Js_SetupAction(this, func_80A89078);
    } else {
        func_8002F2CC(&this->actor, play, 1000.0f);
    }
}

void func_80A8910C(EnJs* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actor.textId = 0x6078;
        En_Js_SetupAction(this, func_80A890C0);
        this->actor.flags |= ACTOR_FLAG_WILL_TALK;
    }
}

void func_80A89160(EnJs* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        En_Js_SetupAction(this, func_80A8910C);
    } else {
        if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF && 
            !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN)) {
            GetItemEntry itemEntry = Randomizer_GetItemFromKnownCheck(RC_WASTELAND_BOMBCHU_SALESMAN, GI_BOMBCHUS_10);
            gSaveContext.pendingSale = itemEntry.itemId;
            gSaveContext.pendingSaleMod = itemEntry.modIndex;
            GiveItemEntryFromActor(&this->actor, play, itemEntry, 10000.0f, 50.0f);
            Flags_SetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN);
        } else {
            GetItemEntry itemEntry = ItemTable_Retrieve(GI_BOMBCHUS_10);
            gSaveContext.pendingSale = itemEntry.itemId;
            gSaveContext.pendingSaleMod = itemEntry.modIndex;
            func_8002F434(&this->actor, play, GI_BOMBCHUS_10, 10000.0f, 50.0f);
        }
    }
}

void func_80A891C4(EnJs* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                if (gSaveContext.rupees < 200) {
                    Message_ContinueTextbox(play, 0x6075);
                    func_80A89008(this);
                } else {
                    Rupees_ChangeBy(-200);
                    En_Js_SetupAction(this, func_80A89160);
                }
                break;
            case 1: // no
                Message_ContinueTextbox(play, 0x6074);
                func_80A89008(this);
        }
    }
}

void func_80A89294(EnJs* this) {
    En_Js_SetupAction(this, func_80A891C4);
    Animation_Change(&this->skelAnime, &gCarpetMerchantIdleAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gCarpetMerchantIdleAnim), ANIMMODE_ONCE, -4.0f);
}

void func_80A89304(EnJs* this, PlayState* play) {
    if (func_80A88F64(this, play, 0x6077)) {
        func_80A89294(this);
    }
}

void EnJs_Update(Actor* thisx, PlayState* play) {
    EnJs* this = (EnJs*)thisx;
    s32 pad;
    s32 pad2;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);

    if (this->actor.bgCheckFlags & 1) {
        if (SurfaceType_GetSfx(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) == 1) {
            Math_ApproachF(&this->actor.shape.yOffset, sREG(80) + -2000.0f, 1.0f, (sREG(81) / 10.0f) + 50.0f);
        }
    } else {
        Math_ApproachZeroF(&this->actor.shape.yOffset, 1.0f, (sREG(81) / 10.0f) + 50.0f);
    }
    if (SkelAnime_Update(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }
    this->actionFunc(this, play);
    if (this->unk_284 & 1) {
        func_80038290(play, &this->actor, &this->unk_278, &this->unk_27E, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->unk_278.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->unk_278.y, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->unk_27E.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->unk_27E.y, 0, 6, 0x1838, 0x64);
    }
    this->unk_284 &= ~0x1;

    if (DECR(this->unk_288) == 0) {
        this->unk_288 = Rand_S16Offset(0x3C, 0x3C);
    }

    this->unk_286 = this->unk_288;

    if (this->unk_286 >= 3) {
        this->unk_286 = 0;
    }
}

s32 EnJs_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnJs* this = (EnJs*)thisx;

    if (limbIndex == 12) {
        rot->y -= this->unk_278.y;
    }
    return false;
}

void EnJs_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80A896DC = { 0.0f, 0.0f, 0.0f };
    EnJs* this = (EnJs*)thisx;

    if (limbIndex == 12) {
        Matrix_MultVec3f(&D_80A896DC, &this->actor.focus.pos);
    }
}
void EnJs_Draw(Actor* thisx, PlayState* play) {
    EnJs* this = (EnJs*)thisx;

    Gfx_SetupDL_37Opa(play->state.gfxCtx);
    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, EnJs_OverrideLimbDraw, EnJs_PostLimbDraw, this);
}
