/*
 * File: z_En_Glider.c
 * Overlay: ovl_En_Glider
 * Description: Glider
 */

#include "z_en_glider.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
//#include "vt.h"
//#include "soh/frame_interpolation.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_WHILE_CULLED)

void EnGlider_Init(Actor* thisx, PlayState* play);
void EnGlider_Destroy(Actor* thisx, PlayState* play);
void EnGlider_Update(Actor* thisx, PlayState* play);
void EnGlider_Draw(Actor* thisx, PlayState* play);
void EnGlider_ResetAction(EnGlider* this, PlayState* play);

const ActorInit En_Glider_InitVars = {
    ACTOR_EN_GLIDER,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GLIDER,
    sizeof(EnGlider),
    (ActorFunc)EnGlider_Init,
    (ActorFunc)EnGlider_Destroy,
    (ActorFunc)EnGlider_Update,
    (ActorFunc)EnGlider_Draw,
};

static ColliderCylinderInit sCylinderInit = { //damages enemies
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00100000, 0x00, 0x00 }, //Glider damage type - only collides with wind zones 0x00100000
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 23, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 6, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

s32 EnGlider_CheckCollider(EnGlider* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        return true;
    }

    if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        return true;
    }
    return false;
}

void EnGlider_Init(Actor* thisx, PlayState* play) {
    EnGlider* this = (EnGlider*)thisx;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    Actor_SetScale(&this->actor, 1.0f);
    this->actionFunc = EnGlider_ResetAction;
}

void EnGlider_Destroy(Actor* thisx, PlayState* play) {
    EnGlider* this = (EnGlider*)thisx;
    Collider_DestroyCylinder(play, &this->collider);
    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void EnGlider_ResetAction(EnGlider* this, PlayState* play) {
}

void EnGlider_Update(Actor* thisx, PlayState* play) {

    EnGlider* this = (EnGlider*)thisx;
    Player* player = GET_PLAYER(play);
    s32 atTouched;
    Actor* hitActor;

    EnGlider_CheckCollider(this, play);

    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, this->unk_304);
    Actor_MoveForward(&this->actor);



    atTouched = (this->collider.base.atFlags & AT_HIT);
    if (atTouched && (this->collider.info.atHitInfo->elemType != ELEMTYPE_UNK4)) {
        hitActor = this->collider.base.at;
        if (hitActor->id == ACTOR_WIND_ZONE) {
            this->wz = hitActor;
            this->inWindZone = true;
        }
        else {
            this->wz = NULL;
            this->inWindZone = false;
        }
    }
    else {
        this->wz = NULL;
        this->inWindZone = false;
    }
    if (this->inWindZone) {
        this->wzDistY = this->actor.world.pos.y - this->wz->actor.world.pos.y;
    }



    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}



void EnGlider_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gGliderDL);
}