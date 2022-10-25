/*
 * File: z_en_ge3.c
 * Overlay: ovl_En_Ge3
 * Description: Gerudo giving you membership card
 */

#include "z_en_ge3.h"
#include "objects/object_geldb/object_geldb.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnGe3_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGe3_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGe3_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGe3_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnGe3_WaitLookAtPlayer(EnGe3* this, GlobalContext* globalCtx);
void EnGe3_ForceTalk(EnGe3* this, GlobalContext* globalCtx);
void EnGe3_UpdateWhenNotTalking(Actor* thisx, GlobalContext* globalCtx);

const ActorInit En_Ge3_InitVars = {
    ACTOR_EN_GE3,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GELDB,
    sizeof(EnGe3),
    (ActorFunc)EnGe3_Init,
    (ActorFunc)EnGe3_Destroy,
    (ActorFunc)EnGe3_Update,
    (ActorFunc)EnGe3_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000722, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

static EnGe3ActionFunc sActionFuncs[] = { EnGe3_WaitLookAtPlayer };
static AnimationHeader* sAnimations[] = { &gGerudoRedStandAnim }; // Idle with right hand on hip and left over mouth
static u8 sAnimationModes[] = { ANIMMODE_LOOP };

void EnGe3_ChangeAction(EnGe3* this, s32 i) {
    this->actionFunc = sActionFuncs[i];

    Animation_Change(&this->skelAnime, sAnimations[i], 1.0f, 0.0f, (f32)Animation_GetLastFrame(sAnimations[i]),
                     sAnimationModes[i], -8.0f);

    this->unk_30C &= ~2;
}

void EnGe3_Init(Actor* thisx, GlobalContext* globalCtx2) {
    EnGe3* this = (EnGe3*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gGerudoRedSkel, NULL, this->jointTable, this->morphTable,
                       GELDB_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gGerudoRedStandAnim);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.world.rot.z = 0;
    this->actor.shape.rot.z = 0;
    EnGe3_ChangeAction(this, 0);
    this->actionFunc = EnGe3_ForceTalk;
    this->unk_30C = 0;
    this->actor.targetMode = 6;
    this->actor.minVelocityY = -4.0f;
    this->actor.gravity = -1.0f;
}

void EnGe3_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnGe3* this = (EnGe3*)thisx;

    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void EnGe3_TurnToFacePlayer(EnGe3* this, GlobalContext* globalCtx) {
    s32 pad;
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(angleDiff) <= 0x4000) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, 4000, 100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        func_80038290(globalCtx, &this->actor, &this->headRot, &this->unk_306, this->actor.focus.pos);
    } else {
        if (angleDiff < 0) {
            Math_SmoothStepToS(&this->headRot.y, -0x2000, 6, 6200, 0x100);
        } else {
            Math_SmoothStepToS(&this->headRot.y, 0x2000, 6, 6200, 0x100);
        }

        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 12, 1000, 100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void EnGe3_LookAtPlayer(EnGe3* this, GlobalContext* globalCtx) {
    if ((ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x2300) &&
        (this->actor.xzDistToPlayer < 100.0f)) {
        func_80038290(globalCtx, &this->actor, &this->headRot, &this->unk_306, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_306.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_306.y, 0, 6, 6200, 100);
    }
}

void EnGe3_Wait(EnGe3* this, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        this->actionFunc = EnGe3_WaitLookAtPlayer;
        this->actor.update = EnGe3_UpdateWhenNotTalking;
        this->actor.flags &= ~ACTOR_FLAG_16;
    }
    EnGe3_TurnToFacePlayer(this, globalCtx);
}

void EnGe3_WaitLookAtPlayer(EnGe3* this, GlobalContext* globalCtx) {
    EnGe3_LookAtPlayer(this, globalCtx);
}

void EnGe3_WaitTillCardGiven(EnGe3* this, GlobalContext* globalCtx) {
    if (Actor_HasParent(&this->actor, globalCtx)) {
        this->actor.parent = NULL;
        this->actionFunc = EnGe3_Wait;
    } else {
        if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, globalCtx, GI_GERUDO_CARD, 10000.0f, 50.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_GF_GERUDO_MEMBERSHIP_CARD, GI_GERUDO_CARD);
            GiveItemEntryFromActor(&this->actor, globalCtx, getItemEntry, 10000.0f, 50.0f);
        }
    }
}

void EnGe3_GiveCard(EnGe3* this, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        this->actor.flags &= ~ACTOR_FLAG_16;
        this->actionFunc = EnGe3_WaitTillCardGiven;
        if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, globalCtx, GI_GERUDO_CARD, 10000.0f, 50.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_GF_GERUDO_MEMBERSHIP_CARD, GI_GERUDO_CARD);
            GiveItemEntryFromActor(&this->actor, globalCtx, getItemEntry, 10000.0f, 50.0f);
        }
    }
}

void EnGe3_ForceTalk(EnGe3* this, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        this->actionFunc = EnGe3_GiveCard;
    } else {
        if (!(this->unk_30C & 4)) {
            func_8002DF54(globalCtx, &this->actor, 7);
            this->unk_30C |= 4;
        }
        this->actor.textId = 0x6004;
        this->actor.flags |= ACTOR_FLAG_16;
        func_8002F1C4(&this->actor, globalCtx, 300.0f, 300.0f, 0);
    }
    EnGe3_LookAtPlayer(this, globalCtx);
}

void EnGe3_UpdateCollision(EnGe3* this, GlobalContext* globalCtx) {
    s32 pad;
    s32 pad2;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 40.0f, 25.0f, 40.0f, 5);

    if (!(this->unk_30C & 2) && SkelAnime_Update(&this->skelAnime)) {
        this->unk_30C |= 2;
    }
}

void EnGe3_MoveAndBlink(EnGe3* this, GlobalContext* globalCtx) {

    Actor_MoveForward(&this->actor);

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }

    this->eyeIndex = this->blinkTimer;

    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

void EnGe3_UpdateWhenNotTalking(Actor* thisx, GlobalContext* globalCtx) {
    EnGe3* this = (EnGe3*)thisx;

    EnGe3_UpdateCollision(this, globalCtx);
    this->actionFunc(this, globalCtx);

    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        this->actionFunc = EnGe3_Wait;
        this->actor.update = EnGe3_Update;
    } else {
        this->actor.textId = 0x6005;
        if (this->actor.xzDistToPlayer < 100.0f) {
            func_8002F2CC(&this->actor, globalCtx, 100.0f);
        }
    }

    EnGe3_MoveAndBlink(this, globalCtx);
}

void EnGe3_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnGe3* this = (EnGe3*)thisx;

    EnGe3_UpdateCollision(this, globalCtx);
    this->actionFunc(this, globalCtx);
    EnGe3_MoveAndBlink(this, globalCtx);
}

s32 EnGe3_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGe3* this = (EnGe3*)thisx;

    switch (limbIndex) {
        // Hide swords and veil from object_geldb
        case GELDB_LIMB_VEIL:
        case GELDB_LIMB_R_SWORD:
        case GELDB_LIMB_L_SWORD:
            *dList = NULL;
            return false;
        // Turn head
        case GELDB_LIMB_HEAD:
            rot->x += this->headRot.y;

        default:
            if (CVar_GetS32("gGerudoWarriorClothingFix", 0)) {
                // This is a hack to fix the color-changing clothes this Gerudo has on N64 versions
                OPEN_DISPS(globalCtx->state.gfxCtx);
                switch (limbIndex) {
                    case GELDB_LIMB_NECK:
                        break;
                    case GELDB_LIMB_HEAD:
                        gDPPipeSync(POLY_OPA_DISP++);
                        gDPSetEnvColor(POLY_OPA_DISP++, 80, 60, 10, 255);
                        break;
                    case GELDB_LIMB_R_SWORD:
                    case GELDB_LIMB_L_SWORD:
                        gDPPipeSync(POLY_OPA_DISP++);
                        gDPSetEnvColor(POLY_OPA_DISP++, 140, 170, 230, 255);
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
                        break;
                    default:
                        gDPPipeSync(POLY_OPA_DISP++);
                        gDPSetEnvColor(POLY_OPA_DISP++, 140, 0, 0, 255);
                        break;
                }
                CLOSE_DISPS(globalCtx->state.gfxCtx);
            }
            break;
    }
    return false;
}

void EnGe3_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGe3* this = (EnGe3*)thisx;
    Vec3f D_80A351C8 = { 600.0f, 700.0f, 0.0f };

    if (limbIndex == GELDB_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80A351C8, &this->actor.focus.pos);
    }
}

void EnGe3_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    static void* eyeTextures[] = {
        gGerudoRedEyeOpenTex,
        gGerudoRedEyeHalfTex,
        gGerudoRedEyeShutTex,
    };
    EnGe3* this = (EnGe3*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_800943C8(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeIndex]));
    func_8002EBCC(&this->actor, globalCtx, 0);
    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnGe3_OverrideLimbDraw, EnGe3_PostLimbDraw, this);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
