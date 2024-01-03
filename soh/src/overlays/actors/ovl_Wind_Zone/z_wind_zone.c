#include "z_wind_zone.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_En_Glider/z_en_glider.h"

#define FLAGS (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_ALWAYS_THROWN)

void WindZone_Init(Actor* thisx, PlayState* play);
void WindZone_Destroy(Actor* thisx, PlayState* play);
void WindZone_Update(Actor* thisx, PlayState* play);
void WindZone_Draw(Actor* thisx, PlayState* play);
void WindZone_Disappear(WindZone* this, PlayState* play);
bool GliderIsInZone(WindZone* this, ColliderCylinder cylinder, PlayState* play);

const ActorInit Wind_Zone_InitVars = {
    ACTOR_WIND_ZONE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(WindZone),
    (ActorFunc)WindZone_Init,
    (ActorFunc)WindZone_Destroy,
    (ActorFunc)WindZone_Update,
    (ActorFunc)WindZone_Draw,
    NULL,
};


static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HARD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00100000, 0x00, 0x00 }, //only collides with gliders damage type 0x00100000
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 50, 200, 0, { 0, 0, 0 } }, //Radius, Height
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 30, MASS_IMMOVABLE };


void WindZone_UpdatePos(WindZone* this, Vec3f* targetPos) {
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

s32 WindZone_CheckCollider(WindZone* this, PlayState* play) {
    if (this->actionFunc == WindZone_Disappear) {
        return false;
    }
    else if (this->collider.base.acFlags & AC_HIT && gPlayState->csCtx.state == 0) {
        this->collider.base.acFlags &= ~AC_HIT;
        return true;
    }
    else if (!(this->collider.base.ocFlags1 & OC1_TYPE_PLAYER) && (this->actor.xzDistToPlayer > 12.0f)) {
        this->collider.base.ocFlags1 |= OC1_TYPE_PLAYER;
        return true;
    }
    return false;
}

void WindZone_InitCollider(Actor* thisx, PlayState* play) {
    WindZone* this = (WindZone*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);
}

void WindZone_Init(Actor* thisx, PlayState* play) {
    WindZone* this = (WindZone*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    WindZone_InitCollider(thisx, play);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);

    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    this->scale = 3.0f;
    this->tile2Y = (s16)Rand_ZeroFloat(5.0f) - 25;
    this->deathTimer = 1000;
    if (this->actor.params == 1) this->deathTimer = 120; //if Rivali's Gale then last 6 seconds
}

void WindZone_Destroy(Actor* thisx, PlayState* play) {
    WindZone* this = (WindZone*)thisx;
    Collider_DestroyCylinder(play, &this->collider);
}



void WindZone_Disappear(WindZone* this, PlayState* play) {
    this->scale -= 0.6f;
    this->actor.shape.shadowScale = 20.0f;
    this->actor.shape.shadowScale *= (this->scale / 3.0f);
    if (!(this->scale > 0.01f)) {
        Actor_Kill(&this->actor);
    }
}

void WindZone_Update(Actor* thisx, PlayState* play) {
    WindZone* this = (WindZone*)thisx;
    s32 pad;
    s32 atTouched;
    Actor* hitActor;
    float targetScale = 20.0f;
   
    this->deathTimer--;
    if (this->deathTimer <= 0) WindZone_Disappear(this, play);
    else {
        //if (GliderIsInZone(this, this->collider, play)) {

        //}
        //else {
            WindZone_CheckCollider(this, play);
            if (this->scale <= targetScale) this->scale += 0.3f;
      //  }
    }
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}



void WindZone_Draw(Actor* thisx, PlayState* play) {
    int r1 = 0, r2 = 0, g1 = 0, g2 = 0, b1 = 0, b2 = 0, a1 = 0;
    if (thisx->params >= 0) { //White / Updraft
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
    WindZone* this = (WindZone*)thisx;
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
    
    if(thisx->params >= 0) gSPDisplayList(POLY_XLU_DISP++, gEffFire2DL);
    else gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

bool GliderIsInZone(WindZone* this, ColliderCylinder cylinder, PlayState* play) {
    if (cylinder.base.acFlags & AC_HIT) {
        cylinder.base.acFlags &= ~AC_HIT;
        if ((cylinder.base.ac != NULL) && (cylinder.base.ac->id == ACTOR_EN_GLIDER)) {
            EnGlider* glider = (EnGlider*)cylinder.base.ac;
            glider->inWindZone = true;
            
            return true;
        }
    }
    return false;
}
