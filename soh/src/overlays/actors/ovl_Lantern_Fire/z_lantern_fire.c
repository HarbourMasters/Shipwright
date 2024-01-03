#include "z_lantern_fire.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Wind_Zone/z_wind_zone.h"

#define FLAGS ( ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_WHILE_CULLED)

void LanternFire_Init(Actor* thisx, PlayState* play);
void LanternFire_Destroy(Actor* thisx, PlayState* play);
void LanternFire_Update(Actor* thisx, PlayState* play);
void LanternFire_Draw(Actor* thisx, PlayState* play);
void LanternFire_Disappear(LanternFire* this, PlayState* play);
void LanternFire_WaitToDie(LanternFire* this, PlayState* play);

const ActorInit Lantern_Fire_InitVars = {
    ACTOR_LANTERN_FIRE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(LanternFire),
    (ActorFunc)LanternFire_Init,
    (ActorFunc)LanternFire_Destroy,
    (ActorFunc)LanternFire_Update,
    (ActorFunc)LanternFire_Draw,
    NULL,
};


static ColliderCylinderInit sCylinderInitRedFire = { //damages enemies
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
        { 0x00000020, 0x00, 0x00 }, // normal arrow damage type 
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 23, 46, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInitBlueFire = { //damages enemies
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_OTHER,
        AC_NONE,
        OC1_ON | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000020, 0x00, 0x00 }, // normal arrow damage type 
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 23, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInit = { 1, 0, 0, 0, MASS_IMMOVABLE };


void LanternFire_UpdatePos(LanternFire* this, Vec3f* targetPos) {
    f32 dist;
    f32 xDiff = targetPos->x - this->actor.world.pos.x;
    f32 yDiff = targetPos->y - this->actor.world.pos.y;
    f32 zDiff = targetPos->z - this->actor.world.pos.z;

    dist = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
    if (fabsf(dist) > fabsf(this->actor.speedXZ)) {
        this->actor.velocity.x = (xDiff / dist) * this->actor.speedXZ;
        this->actor.velocity.z = (zDiff / dist) * this->actor.speedXZ;
    } else {
        this->actor.velocity.x = 0.0f;
        this->actor.velocity.z = 0.0f;
    }

    this->actor.velocity.y += this->actor.gravity;
    if (!(this->actor.minVelocityY <= this->actor.velocity.y)) {
        this->actor.velocity.y = this->actor.minVelocityY;
    }
}

s32 LanternFire_CheckCollider(LanternFire* this, PlayState* play) {
    if (this->actionFunc == LanternFire_Disappear) {
        return false;
    }

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

void LanternFire_Init(Actor* thisx, PlayState* play) {
    LanternFire* this = (LanternFire*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    thisx->room = -1;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);

    Collider_InitCylinder(play, &this->collider);

    if (this->actor.params == 0) { // regular fire
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInitRedFire);
        this->collider.info.toucherFlags &= ~0x18;
        this->collider.info.toucherFlags |= 0;
        this->collider.info.toucher.dmgFlags = 0x00000800;
    }
    else if (this->actor.params == 1) //Blue Fire
    {
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInitBlueFire);
    }
    else { //Green or Purple 
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInitRedFire);
    }
    // params == 1 Blue Fire
    //Taken care of in "z_bg_ice_shelter.c"  Red Ice
    //Taken care of in "z_en_kusa.c"  Bushes
    this->actor.gravity = -0.6f;
    this->actor.speedXZ = 5.0f;
    this->actor.velocity.y = 2.0f;
    this->deathTimer = 100;
    this->actor.parent = NULL;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    this->scale = 3.0f;
    this->tile2Y = (s16)Rand_ZeroFloat(5.0f) - 25;
}

void LanternFire_Destroy(Actor* thisx, PlayState* play) {
    LanternFire* this = (LanternFire*)thisx;
    if (this->wz != NULL) this->wz->deathTimer = 0;
    Collider_DestroyCylinder(play, &this->collider);
}


void LanternFire_WaitToDie(LanternFire* this, PlayState* play) {
    if (DECR(this->deathTimer) == 0) {
        this->actionFunc = LanternFire_Disappear;
    }
}


void LanternFire_Disappear(LanternFire* this, PlayState* play) {
    Math_SmoothStepToF(&this->scale, 0.0f, 0.3f, 0.2f, 0.0f);
    this->actor.shape.shadowScale = 20.0f;
    this->actor.shape.shadowScale *= (this->scale / 3.0f);
    if (!(this->scale > 0.02f)) {
        if (this->wz != NULL) this->wz->deathTimer = 0;
        Actor_Kill(&this->actor);
    }
}

void LanternFire_Update(Actor* thisx, PlayState* play) {
    LanternFire* this = (LanternFire*)thisx;
    s32 pad;
    s32 atTouched;
    Actor* hitActor;

    if (this->actionFunc != LanternFire_Disappear) {
        if (LanternFire_CheckCollider(this, play)) {
            this->actionFunc = LanternFire_Disappear;
        }
        else {
            this->actionFunc = LanternFire_WaitToDie;
        }
    }
    if (this->actor.world.pos.y - this->actor.floorHeight > 0.2f) {
        this->actor.speedXZ = 5.0f;
        Actor_MoveForward(&this->actor);
    }
    else {
        this->actor.speedXZ = 0.0f;
        Actor_MoveForward(&this->actor);
    }
    if (this->actor.parent == NULL) {
        atTouched = (this->collider.base.atFlags & AT_HIT);
        if (atTouched && (this->collider.info.atHitInfo->elemType != ELEMTYPE_UNK4)) {
            hitActor = this->collider.base.at;
            this->hitFlags |= 1;
            this->hitFlags |= 2;

            if (this->collider.info.atHitInfo->bumperFlags & 2) {
                this->actor.world.pos.x = this->collider.info.atHitInfo->bumper.hitPos.x;
                this->actor.world.pos.y = this->collider.info.atHitInfo->bumper.hitPos.y;
                this->actor.world.pos.z = this->collider.info.atHitInfo->bumper.hitPos.z;
                if (hitActor->id == ACTOR_EN_KUSA) {
                    if (hitActor->child == NULL) {
                        this->actor.parent = hitActor;
                        this->scale = 5.0f;
                        this->actor.world.pos = this->actor.parent->world.pos;
                        this->wz = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_WIND_ZONE,
                            this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0,
                            this->actor.shape.rot.y, 1, 0);
                    }
                }
                else {
                    this->actionFunc = LanternFire_Disappear;
                }
            }
        }
        else {
            //if (this->actor.bgCheckFlags & 0x1) this->collider.dim.radius *= 1.05f; //grow slightly if touching the ground
        }
    }
    else {
        this->actor.world.pos = this->actor.parent->world.pos;
    }

    this->actionFunc(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 12.0f, 10.0f, 0.0f, 5);

    if (this->actionFunc != LanternFire_Disappear) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void LanternFire_Draw(Actor* thisx, PlayState* play) {
    int r1 = 0, r2 = 0, g1 = 0, g2 = 0, b1 = 0, b2 = 0, a1 = 0;
    if (thisx->params == 0) { //Red
        r1 = 255;
        g1 = 200;
        b1 = 0;
        a1 = 255;
        r2 = 255;
        g2 = 0;
        b2 = 0;
    }
    if (thisx->params == 1) { //Blue
        r1 = 0;
        g1 = 170;
        b1 = 255;
        a1 = 255;
        r2 = 0;
        g2 = 0;
        b2 = 255;
    }
    if (thisx->params == 2) { //Green
        r1 = 170;
        g1 = 255;
        b1 = 0;
        a1 = 255;
        r2 = 0;
        g2 = 150;
        b2 = 0;
    }
    if (thisx->params == 3) { //Purple
        r1 = 255;
        g1 = 170;
        b1 = 255;
        a1 = 255;
        r2 = 100;
        g2 = 0;
        b2 = 255;
    }
    if (thisx->params == 4) { //White / Updraft
        r1 = 170;
        g1 = 170;
        b1 = 170;
        a1 = 96;
        r2 = 255;
        g2 = 255;
        b2 = 255;
    }

    Color_RGBA8 primColors[] = { //yellow inside
        { r1, g1, b1, 128 },
        { 0, 0, 0, 255 }, //nothing
    };
    Color_RGBA8 envColors[] = { //red outside
        { r2, g2, b2, 255 }, //Alpha Does Nothing
        { 0, 0, 0, 255 }, //nothing
    };
    s32 pad;
    LanternFire* this = (LanternFire*)thisx;
    Vec3f scale = { 0.0f, 0.0f, 0.0f };
    Vec3f sp90 = { 0.0f, 0.0f, 0.0f };
    s16 sp8E;
    f32 sp88;
    f32 sp84;
    f32 sp80;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    sp8E = Math_Vec3f_Yaw(&scale, &this->actor.velocity) - Camera_GetCamDirYaw(GET_ACTIVE_CAM(play));
    sp84 = fabsf(Math_CosS(sp8E));
    sp88 = Math_SinS(sp8E);
    sp80 = Math_Vec3f_DistXZ(&scale, &this->actor.velocity) / 1.5f;
    Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000) * (M_PI / 0x8000), MTXMODE_APPLY);
    Matrix_RotateZ(((sp88 * -10.0f) * sp80) * (M_PI / 180.0f), MTXMODE_APPLY);
    scale.x = scale.y = scale.z = this->scale * 0.001f;
    Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
    sp84 = sp80 * ((0.01f * -15.0f) * sp84) + 1.0f;
    if (sp84 < 0.1f) {
        sp84 = 0.1f;
    }
    Matrix_Scale(1.0f, sp84, 1.0f / sp84, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x8,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                play->state.frames * this->tile2Y, 0x20, 0x80));
    gDPSetPrimColor(POLY_XLU_DISP++, 128, 128, primColors[((this->actor.params & 0x8000) >> 0xF)].r,
        primColors[((this->actor.params & 0x8000) >> 0xF)].g,
        primColors[((this->actor.params & 0x8000) >> 0xF)].b,
        primColors[((this->actor.params & 0x8000) >> 0xF)].a);
    gDPSetEnvColor(POLY_XLU_DISP++, envColors[((this->actor.params & 0x8000) >> 0xF)].r,
        envColors[((this->actor.params & 0x8000) >> 0xF)].g,
        envColors[((this->actor.params & 0x8000) >> 0xF)].b,
        envColors[((this->actor.params & 0x8000) >> 0xF)].a);
    gDPPipeSync(POLY_XLU_DISP++);
    
    if(thisx->params == 4) gSPDisplayList(POLY_XLU_DISP++, gEffFire2DL);
    else gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}
