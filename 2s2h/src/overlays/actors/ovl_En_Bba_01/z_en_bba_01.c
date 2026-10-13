/*
 * File: z_en_bba_01.c
 * Overlay: ovl_En_Bba_01
 * Description: Unused Bomb Shop Lady NPC
 *
 * Note: This actor was probably written with OOT's object_bba's skeleton in mind, and so this actor is very bugged.
 *
 * The main offender is that gBombShopLadySkel has 18 limbs, while the system used (EnHy) expects 16 (see @bug in
 * FinishInit below).
 *
 * The draw functions also use different limbs than expected, which results in, for example, EnBba01's right arm and bag
 * following the player instead of her head and torso.
 */

#include "z_en_bba_01.h"
#include "objects/object_bba/object_bba.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnBba01_Init(Actor* thisx, PlayState* play);
void EnBba01_Destroy(Actor* thisx, PlayState* play);
void EnBba01_Update(Actor* thisx, PlayState* play);
void EnBba01_Draw(Actor* thisx, PlayState* play);

void EnBba01_Walk(EnHy* this, PlayState* play);
void EnBba01_FaceFoward(EnHy* this, PlayState* play);
void EnBba01_Talk(EnHy* this, PlayState* play);

ActorProfile En_Bba_01_Profile = {
    /**/ ACTOR_EN_BBA_01,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnBba01),
    /**/ EnBba01_Init,
    /**/ EnBba01_Destroy,
    /**/ EnBba01_Update,
    /**/ EnBba01_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 64, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0),
    /* Deku Stick     */ DMG_ENTRY(0, 0),
    /* Horse trample  */ DMG_ENTRY(0, 0),
    /* Explosives     */ DMG_ENTRY(0, 0),
    /* Zora boomerang */ DMG_ENTRY(0, 0),
    /* Normal arrow   */ DMG_ENTRY(0, 0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0),
    /* Hookshot       */ DMG_ENTRY(0, 0),
    /* Goron punch    */ DMG_ENTRY(0, 0),
    /* Sword          */ DMG_ENTRY(0, 0),
    /* Goron pound    */ DMG_ENTRY(0, 0),
    /* Fire arrow     */ DMG_ENTRY(0, 0),
    /* Ice arrow      */ DMG_ENTRY(0, 0),
    /* Light arrow    */ DMG_ENTRY(0, 0),
    /* Goron spikes   */ DMG_ENTRY(0, 0),
    /* Deku spin      */ DMG_ENTRY(0, 0),
    /* Deku bubble    */ DMG_ENTRY(0, 0),
    /* Deku launch    */ DMG_ENTRY(0, 0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0),
    /* Zora barrier   */ DMG_ENTRY(0, 0),
    /* Normal shield  */ DMG_ENTRY(0, 0),
    /* Light ray      */ DMG_ENTRY(0, 0),
    /* Thrown object  */ DMG_ENTRY(0, 0),
    /* Zora punch     */ DMG_ENTRY(0, 0),
    /* Spin attack    */ DMG_ENTRY(0, 0),
    /* Sword beam     */ DMG_ENTRY(0, 0),
    /* Normal Roll    */ DMG_ENTRY(0, 0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0),
    /* Unblockable    */ DMG_ENTRY(0, 0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0),
    /* Powder Keg     */ DMG_ENTRY(0, 0),
};

static TrackOptionsSet sTrackOptions = {
    { 0xFA0, 4, 1, 3 },
    { 0x1770, 4, 1, 6 },
    { 0xFA0, 4, 1, 3 },
    { 0x1770, 4, 1, 6 },
};

void EnBba01_UpdateModel(EnBba01* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f point;

    EnHy_UpdateSkelAnime(&this->enHy, play);
    if (SubS_AngleDiffLessEqual(this->enHy.actor.shape.rot.y, 0x36B0, this->enHy.actor.yawTowardsPlayer)) {
        point.x = player->actor.world.pos.x;
        point.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
        point.z = player->actor.world.pos.z;
        SubS_TrackPoint(&point, &this->enHy.actor.focus.pos, &this->enHy.actor.shape.rot, &this->enHy.trackTarget,
                        &this->enHy.headRot, &this->enHy.torsoRot, &sTrackOptions);
    } else {
        Math_SmoothStepToS(&this->enHy.trackTarget.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->enHy.trackTarget.y, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->enHy.headRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->enHy.headRot.y, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->enHy.torsoRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->enHy.torsoRot.y, 0, 4, 0x3E8, 1);
    }
    SubS_UpdateFidgetTables(play, this->enHy.fidgetTableY, this->enHy.fidgetTableZ, ENHY_LIMB_MAX);
    EnHy_UpdateCollider(&this->enHy, play);
}

s32 EnBba01_TestIsTalking(EnBba01* this, PlayState* play) {
    s32 isTalking = false;

    if (Actor_TalkOfferAccepted(&this->enHy.actor, &play->state)) {
        isTalking = true;
        this->enHy.textId = 0x10B9; // Invalid textId, produces empty textbox
        this->enHy.prevTrackTarget = this->enHy.trackTarget;
        this->enHy.prevHeadRot = this->enHy.headRot;
        this->enHy.prevTorsoRot = this->enHy.torsoRot;
        this->enHy.prevActionFunc = this->enHy.actionFunc;
        this->enHy.actionFunc = EnBba01_Talk;
    }
    return isTalking;
}

s32 func_809CC270(EnBba01* this, PlayState* play) {
    s16 screenPosX;
    s16 screenPosY;

    Actor_GetScreenPos(play, &this->enHy.actor, &screenPosX, &screenPosY);
    //! @bug: Both x and y conditionals are always true, || should be an &&
    if (!this->enHy.waitingOnInit && ((screenPosX >= 0) || (screenPosX < SCREEN_WIDTH)) &&
        ((screenPosY >= 0) || (screenPosY < SCREEN_HEIGHT))) {
        Actor_OfferTalkExchangeEquiCylinder(&this->enHy.actor, play, 30.0f, PLAYER_IA_MAGIC_BEANS);
    }
    return true;
}

void EnBba01_FinishInit(EnHy* this, PlayState* play) {
    //! @bug: gBombShopLadySkel does not match EnHy's skeleton assumptions.
    //! Since gBombShopLadySkel has more limbs than expected, joint and morph tables will overflow
    if (EnHy_Init(this, play, &gBombShopLadySkel, ENHY_ANIM_BBA_6)) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.draw = EnBba01_Draw;
        this->waitingOnInit = false;
        if (ENBBA01_GET_PATH_INDEX(&this->actor) == ENBBA01_PATH_INDEX_NONE) {
            this->actionFunc = EnBba01_FaceFoward;
        } else {
            this->actionFunc = EnBba01_Walk;
        }
    }
}

void EnBba01_Walk(EnHy* this, PlayState* play) {
    if (EnHy_MoveForwards(this, 1.0f)) {
        this->curPoint = 0;
    }
}

void EnBba01_FaceFoward(EnHy* this, PlayState* play) {
    this->actor.shape.rot = this->actor.world.rot;
}

void EnBba01_Talk(EnHy* this, PlayState* play) {
    s16 yaw;
    u8 talkState;

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xFA0, 1);

    talkState = Message_GetState(&play->msgCtx);
    this->msgFading = (talkState == TEXT_STATE_FADING) ? true : false;

    switch (talkState) {
        case TEXT_STATE_NONE:
            yaw = ABS_ALT(this->actor.shape.rot.y - this->actor.yawTowardsPlayer);
            if (yaw < 0x64) {
                Message_StartTextbox(play, this->textId, NULL);
            }
            break;

        case TEXT_STATE_CLOSING:
            this->actor.textId = 0;
            this->trackTarget = this->prevTrackTarget;
            this->headRot = this->prevHeadRot;
            this->torsoRot = this->prevTorsoRot;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            this->actionFunc = this->prevActionFunc;
            this->prevActionFunc = NULL;
            break;

        default:
            break;
    }
}

void EnBba01_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBba01* this = (EnBba01*)thisx;

    this->enHy.animObjectSlot = SubS_GetObjectSlot(OBJECT_BBA, play);
    this->enHy.headObjectSlot = SubS_GetObjectSlot(OBJECT_BBA, play);
    this->enHy.skelUpperObjectSlot = SubS_GetObjectSlot(OBJECT_BBA, play);
    this->enHy.skelLowerObjectSlot = SubS_GetObjectSlot(OBJECT_BBA, play);

    if ((this->enHy.animObjectSlot <= OBJECT_SLOT_NONE) || (this->enHy.headObjectSlot <= OBJECT_SLOT_NONE) ||
        (this->enHy.skelUpperObjectSlot <= OBJECT_SLOT_NONE) || (this->enHy.skelLowerObjectSlot <= OBJECT_SLOT_NONE)) {
        Actor_Kill(&this->enHy.actor);
    }
    this->enHy.actor.draw = NULL;
    Collider_InitCylinder(play, &this->enHy.collider);
    Collider_SetCylinder(play, &this->enHy.collider, &this->enHy.actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->enHy.actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->enHy.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->enHy.path = SubS_GetPathByIndex(play, ENBBA01_GET_PATH_INDEX(&this->enHy.actor), ENBBA01_PATH_INDEX_NONE);
    this->enHy.waitingOnInit = true;
    Actor_SetScale(&this->enHy.actor, 0.01f);
    this->enHy.actionFunc = EnBba01_FinishInit;
}

void EnBba01_Destroy(Actor* thisx, PlayState* play) {
    EnBba01* this = (EnBba01*)thisx;

    Collider_DestroyCylinder(play, &this->enHy.collider);
}

void EnBba01_Update(Actor* thisx, PlayState* play) {
    EnBba01* this = (EnBba01*)thisx;

    EnBba01_TestIsTalking(this, play);
    this->enHy.actionFunc(&this->enHy, play);
    Actor_UpdateBgCheckInfo(play, &this->enHy.actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    EnBba01_UpdateModel(this, play);
    func_809CC270(this, play);
}

s32 EnBba01_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBba01* this = (EnBba01*)thisx;
    s8 bodyPartIndex;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    bodyPartIndex = gEnHyLimbToBodyParts[limbIndex];
    if (bodyPartIndex > BODYPART_NONE) {
        Matrix_MultVec3f(&zeroVec, &this->enHy.bodyPartsPos[bodyPartIndex]);
    }

    if (limbIndex == BOMB_SHOP_LADY_LIMB_RIGHT_LOWER_ARM_ROOT) {
        OPEN_DISPS(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->enHy.headObjectSlot].segment);
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->enHy.headObjectSlot].segment);
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->enHy.skelLowerObjectSlot].segment);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (limbIndex == BOMB_SHOP_LADY_LIMB_RIGHT_LOWER_ARM_ROOT) {
        Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->enHy.headRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(-this->enHy.headRot.x, MTXMODE_APPLY);
        Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == BOMB_SHOP_LADY_LIMB_BAG) {
        Matrix_RotateXS(-this->enHy.torsoRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(-this->enHy.torsoRot.x, MTXMODE_APPLY);
    }

    if ((limbIndex == BOMB_SHOP_LADY_LIMB_RIGHT_LOWER_ARM_ROOT) && this->enHy.msgFading &&
        ((play->state.frames % 2) == 0)) {
        Matrix_Translate(40.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if ((limbIndex == BOMB_SHOP_LADY_LIMB_BAG) || (limbIndex == BOMB_SHOP_LADY_LIMB_TORSO) ||
        (limbIndex == BOMB_SHOP_LADY_LIMB_LEFT_FOREARM)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->enHy.fidgetTableY[limbIndex]) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(this->enHy.fidgetTableZ[limbIndex]) * 200.0f);
    }

    return false;
}

void EnBba01_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnBba01* this = (EnBba01*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == BOMB_SHOP_LADY_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->enHy.skelUpperObjectSlot].segment);
        gSegments[0x06] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->enHy.skelUpperObjectSlot].segment);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (limbIndex == BOMB_SHOP_LADY_LIMB_RIGHT_LOWER_ARM_ROOT) {
        Matrix_MultVec3f(&zeroVec, &this->enHy.actor.focus.pos);
    }
}

void EnBba01_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

void EnBba01_Draw(Actor* thisx, PlayState* play) {
    EnBba01* this = (EnBba01*)thisx;
    s32 i;
    u8* shadowTex = GRAPH_ALLOC(play->state.gfxCtx, SUBS_SHADOW_TEX_SIZE);
    u8* shadowTexIter;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_EnvColor(play->state.gfxCtx, 255, 255, 255, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_EnvColor(play->state.gfxCtx, 55, 55, 255, 0));
    gDPPipeSync(POLY_OPA_DISP++);
    SkelAnime_DrawTransformFlexOpa(play, this->enHy.skelAnime.skeleton, this->enHy.skelAnime.jointTable,
                                   this->enHy.skelAnime.dListCount, EnBba01_OverrideLimbDraw, EnBba01_PostLimbDraw,
                                   EnBba01_TransformLimbDraw, &this->enHy.actor);
    Matrix_RotateXS(0, MTXMODE_NEW);

    for (i = 0, shadowTexIter = shadowTex; i < SUBS_SHADOW_TEX_SIZE; i++) {
        *shadowTexIter++ = 0;
    }
    for (i = 0; i < 5; i++) {
        SubS_GenShadowTex(this->enHy.bodyPartsPos, &this->enHy.actor.world.pos, shadowTex, i / 5.0f, ENHY_BODYPART_MAX,
                          gEnHyShadowSizes, gEnHyParentShadowBodyParts);
    }
    SubS_DrawShadowTex(&this->enHy.actor, &play->state, shadowTex);

    CLOSE_DISPS(play->state.gfxCtx);
}
