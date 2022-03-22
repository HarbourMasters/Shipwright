/*
 * File: z_en_js.c
 * Overlay: ovl_En_Js
 * Description: Magic Carpet Salesman
 */

#include "z_en_js.h"
#include "objects/object_js/object_js.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnJs_Init(Actor* thisx, GlobalContext* globalCtx);
void EnJs_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnJs_Update(Actor* thisx, GlobalContext* globalCtx);
void EnJs_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A89304(EnJs* this, GlobalContext* globalCtx);

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

void EnJs_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnJs* this = (EnJs*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 36.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gCarpetMerchantSkel, &gCarpetMerchantSlappingKneeAnim,
                       this->jointTable, this->morphTable, 13);
    Animation_PlayOnce(&this->skelAnime, &gCarpetMerchantSlappingKneeAnim);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    En_Js_SetupAction(this, func_80A89304);
    this->unk_284 = 0;
    this->actor.gravity = -1.0f;
    Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_EN_JSJUTAN, this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
}

void EnJs_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnJs* this = (EnJs*)thisx;

    Collider_DestroyCylinder(globalCtx, &this->collider);
}

u8 func_80A88F64(EnJs* this, GlobalContext* globalCtx, u16 textId) {
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        return 1;
    } else {
        this->actor.textId = textId;
        yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

        if (ABS(yawDiff) <= 0x1800 && this->actor.xzDistToPlayer < 100.0f) {
            this->unk_284 |= 1;
            func_8002F2CC(&this->actor, globalCtx, 100.0f);
        }
        return 0;
    }
}

void func_80A89008(EnJs* this) {
    En_Js_SetupAction(this, func_80A89304);
    Animation_Change(&this->skelAnime, &gCarpetMerchantSlappingKneeAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gCarpetMerchantSlappingKneeAnim), ANIMMODE_ONCE, -4.0f);
}

void func_80A89078(EnJs* this, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        func_80A89008(this);
        this->actor.flags &= ~ACTOR_FLAG_16;
    }
}

void func_80A890C0(EnJs* this, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        En_Js_SetupAction(this, func_80A89078);
    } else {
        func_8002F2CC(&this->actor, globalCtx, 1000.0f);
    }
}

void func_80A8910C(EnJs* this, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        this->actor.textId = 0x6078;
        En_Js_SetupAction(this, func_80A890C0);
        this->actor.flags |= ACTOR_FLAG_16;
    }
}

void func_80A89160(EnJs* this, GlobalContext* globalCtx) {
    if (Actor_HasParent(&this->actor, globalCtx)) {
        this->actor.parent = NULL;
        En_Js_SetupAction(this, func_80A8910C);
    } else {
        func_8002F434(&this->actor, globalCtx, GI_BOMBCHUS_10, 10000.0f, 50.0f);
    }
}

void func_80A891C4(EnJs* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // yes
                if (gSaveContext.rupees < 200) {
                    Message_ContinueTextbox(globalCtx, 0x6075);
                    func_80A89008(this);
                } else {
                    Rupees_ChangeBy(-200);
                    En_Js_SetupAction(this, func_80A89160);
                }
                break;
            case 1: // no
                Message_ContinueTextbox(globalCtx, 0x6074);
                func_80A89008(this);
        }
    }
}

void func_80A89294(EnJs* this) {
    En_Js_SetupAction(this, func_80A891C4);
    Animation_Change(&this->skelAnime, &gCarpetMerchantIdleAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gCarpetMerchantIdleAnim), ANIMMODE_ONCE, -4.0f);
}

void func_80A89304(EnJs* this, GlobalContext* globalCtx) {
    if (func_80A88F64(this, globalCtx, 0x6077)) {
        func_80A89294(this);
    }
}

void EnJs_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnJs* this = (EnJs*)thisx;
    s32 pad;
    s32 pad2;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 0.0f, 0.0f, 0.0f, 4);

    if (this->actor.bgCheckFlags & 1) {
        if (SurfaceType_GetSfx(&globalCtx->colCtx, this->actor.floorPoly, this->actor.floorBgId) == 1) {
            Math_ApproachF(&this->actor.shape.yOffset, sREG(80) + -2000.0f, 1.0f, (sREG(81) / 10.0f) + 50.0f);
        }
    } else {
        Math_ApproachZeroF(&this->actor.shape.yOffset, 1.0f, (sREG(81) / 10.0f) + 50.0f);
    }
    if (SkelAnime_Update(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }
    this->actionFunc(this, globalCtx);
    if (this->unk_284 & 1) {
        func_80038290(globalCtx, &this->actor, &this->unk_278, &this->unk_27E, this->actor.focus.pos);
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

s32 EnJs_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnJs* this = (EnJs*)thisx;

    if (limbIndex == 12) {
        rot->y -= this->unk_278.y;
    }
    return false;
}

void EnJs_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80A896DC = { 0.0f, 0.0f, 0.0f };
    EnJs* this = (EnJs*)thisx;

    if (limbIndex == 12) {
        Matrix_MultVec3f(&D_80A896DC, &this->actor.focus.pos);
    }
}
void EnJs_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnJs* this = (EnJs*)thisx;

    func_800943C8(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnJs_OverrideLimbDraw, EnJs_PostLimbDraw, this);
}
