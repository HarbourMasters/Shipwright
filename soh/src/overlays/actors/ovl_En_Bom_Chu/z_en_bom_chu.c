#include "z_en_bom_chu.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_4

#define BOMBCHU_SCALE 0.01f

void EnBomChu_Init(Actor* thisx, PlayState* play);
void EnBomChu_Destroy(Actor* thisx, PlayState* play);
void EnBomChu_Update(Actor* thisx, PlayState* play);
void EnBomChu_Draw(Actor* thisx, PlayState* play);

void EnBomChu_WaitForRelease(EnBomChu* this, PlayState* play);
void EnBomChu_Move(EnBomChu* this, PlayState* play);
void EnBomChu_WaitForKill(EnBomChu* this, PlayState* play);

const ActorInit En_Bom_Chu_InitVars = {
    ACTOR_EN_BOM_CHU,
    ACTORCAT_EXPLOSIVE,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnBomChu),
    (ActorFunc)EnBomChu_Init,
    (ActorFunc)EnBomChu_Destroy,
    (ActorFunc)EnBomChu_Update,
    (ActorFunc)EnBomChu_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElemInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 12 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_1 | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElemInit),
    sJntSphElemInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000 * BOMBCHU_SCALE, ICHAIN_STOP),
};

void EnBomChu_Init(Actor* thisx, PlayState* play) {
    static u8 p1StartColor[] = { 250, 0, 0, 250 };
    static u8 p2StartColor[] = { 200, 0, 0, 130 };
    static u8 p1EndColor[] = { 150, 0, 0, 100 };
    static u8 p2EndColor[] = { 100, 0, 0, 50 };
    EnBomChu* this = (EnBomChu*)thisx;
    EffectBlureInit1 blureInit;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);

    this->collider.elements[0].dim.worldSphere.radius = this->collider.elements[0].dim.modelSphere.radius;

    for (i = 0; i < 4; i++) {
        blureInit.p1StartColor[i] = p1StartColor[i];
        blureInit.p2StartColor[i] = p2StartColor[i];
        blureInit.p1EndColor[i] = p1EndColor[i];
        blureInit.p2EndColor[i] = p2EndColor[i];
    }

    blureInit.elemDuration = 16;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 0;
    blureInit.trailType = 3;

    Effect_Add(play, &this->blure1Index, EFFECT_BLURE1, 0, 0, &blureInit);
    Effect_Add(play, &this->blure2Index, EFFECT_BLURE1, 0, 0, &blureInit);

    this->actor.room = -1;
    this->timer = 120;
    this->actionFunc = EnBomChu_WaitForRelease;
}

void EnBomChu_Destroy(Actor* thisx, PlayState* play) {
    EnBomChu* this = (EnBomChu*)thisx;

    Effect_Delete(play, this->blure1Index);
    Effect_Delete(play, this->blure2Index);
    Collider_DestroyJntSph(play, &this->collider);
}

void EnBomChu_Explode(EnBomChu* this, PlayState* play) {
    EnBom* bomb;
    s32 i;

    bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, BOMB_BODY);
    if (bomb != NULL) {
        bomb->timer = 0;
    }

    this->timer = 1;
    this->actor.speedXZ = 0.0f;

    if (this->actor.yDistToWater > 0.0f) {
        for (i = 0; i < 40; i++) {
            EffectSsBubble_Spawn(play, &this->actor.world.pos, 1.0f, 5.0f, 30.0f, 0.25f);
        }
    }

    this->actionFunc = EnBomChu_WaitForKill;
}

void EnBomChu_CrossProduct(Vec3f* a, Vec3f* b, Vec3f* dest) {
    dest->x = (a->y * b->z) - (a->z * b->y);
    dest->y = (a->z * b->x) - (a->x * b->z);
    dest->z = (a->x * b->y) - (a->y * b->x);
}

void EnBomChu_UpdateFloorPoly(EnBomChu* this, CollisionPoly* floorPoly, PlayState* play) {
    Vec3f normal;
    Vec3f vec;
    f32 angle;
    f32 magnitude;
    f32 normDotUp;
    MtxF mf;

    if (CVar_GetS32("gBombchusOOB", 0) && floorPoly == NULL) {
        EnBomChu_Explode(this, play);
        return;
    }

    this->actor.floorPoly = floorPoly;

    normal.x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
    normal.y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
    normal.z = COLPOLY_GET_NORMAL(floorPoly->normal.z);

    normDotUp = DOTXYZ(normal, this->axisUp);

    if (!(fabsf(normDotUp) >= 1.0f)) {
        angle = Math_FAcosF(normDotUp);

        if (!(angle < 0.001f)) {
            EnBomChu_CrossProduct(&this->axisUp, &normal, &vec);
            //! @bug this function expects a unit vector but `vec` is not normalized
            Matrix_RotateAxis(angle, &vec, MTXMODE_NEW);

            Matrix_MultVec3f(&this->axisLeft, &vec);
            this->axisLeft = vec;

            EnBomChu_CrossProduct(&this->axisLeft, &normal, &this->axisForwards);

            magnitude = Math3D_Vec3fMagnitude(&this->axisForwards);

            if (magnitude < 0.001f) {
                EnBomChu_Explode(this, play);
                return;
            }

            this->axisForwards.x *= 1.0f / magnitude;
            this->axisForwards.y *= 1.0f / magnitude;
            this->axisForwards.z *= 1.0f / magnitude;

            this->axisUp = normal;


            // mf = (axisLeft | axisUp | axisForwards)

            mf.xx = this->axisLeft.x;
            mf.yx = this->axisLeft.y;
            mf.zx = this->axisLeft.z;

            mf.xy = normal.x;
            mf.yy = normal.y;
            mf.zy = normal.z;

            mf.xz = this->axisForwards.x;
            mf.yz = this->axisForwards.y;
            mf.zz = this->axisForwards.z;

            Matrix_MtxFToYXZRotS(&mf, &this->actor.world.rot, 0);

            // A hack for preventing bombchus from sticking to ledges.
            // The visual rotation reverts the sign inversion (shape.rot.x = -world.rot.x).
            // The better fix would be making func_8002D908 compute XYZ velocity better,
            // or not using it and make the bombchu compute its own velocity.
            this->actor.world.rot.x = -this->actor.world.rot.x;
        }
    }
}

void EnBomChu_WaitForRelease(EnBomChu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        EnBomChu_Explode(this, play);
        return;
    }

    if (Actor_HasNoParent(&this->actor, play)) {
        this->actor.world.pos = player->actor.world.pos;
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
        this->actor.shape.rot.y = player->actor.shape.rot.y;

        // rot.y = 0 -> +z (forwards in model space)
        this->axisForwards.x = Math_SinS(this->actor.shape.rot.y);
        this->axisForwards.y = 0.0f;
        this->axisForwards.z = Math_CosS(this->actor.shape.rot.y);

        // +y (up in model space)
        this->axisUp.x = 0.0f;
        this->axisUp.y = 1.0f;
        this->axisUp.z = 0.0f;

        // rot.y = 0 -> +x (left in model space)
        this->axisLeft.x = Math_SinS(this->actor.shape.rot.y + 0x4000);
        this->axisLeft.y = 0;
        this->axisLeft.z = Math_CosS(this->actor.shape.rot.y + 0x4000);

        this->actor.speedXZ = 8.0f;
        //! @bug there is no NULL check on the floor poly.  If the player is out of bounds the floor poly will be NULL
        //! and will cause a crash inside this function.
        EnBomChu_UpdateFloorPoly(this, this->actor.floorPoly, play);
        this->actor.flags |= ACTOR_FLAG_0; // make chu targetable
        func_8002F850(play, &this->actor);
        this->actionFunc = EnBomChu_Move;
    }
}

void EnBomChu_Move(EnBomChu* this, PlayState* play) {
    CollisionPoly* polySide;
    CollisionPoly* polyUpDown;
    s32 bgIdSide;
    s32 bgIdUpDown;
    s32 i;
    f32 lineLength;
    Vec3f posA;
    Vec3f posB;
    Vec3f posSide;
    Vec3f posUpDown;

    this->actor.speedXZ = 8.0f;
    lineLength = this->actor.speedXZ * 2.0f;

    if (this->timer != 0) {
        this->timer--;
    }

    if ((this->timer == 0) || (this->collider.base.acFlags & AC_HIT) ||
        ((this->collider.base.ocFlags1 & OC1_HIT) && (this->collider.base.oc->category != ACTORCAT_PLAYER))) {
        EnBomChu_Explode(this, play);
        return;
    }

    posA.x = this->actor.world.pos.x + (this->axisUp.x * 2.0f);
    posA.y = this->actor.world.pos.y + (this->axisUp.y * 2.0f);
    posA.z = this->actor.world.pos.z + (this->axisUp.z * 2.0f);

    posB.x = this->actor.world.pos.x - (this->axisUp.x * 4.0f);
    posB.y = this->actor.world.pos.y - (this->axisUp.y * 4.0f);
    posB.z = this->actor.world.pos.z - (this->axisUp.z * 4.0f);

    if (BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, &posUpDown, &polyUpDown, true, true, true, true,
                                &bgIdUpDown) &&
        !(func_80041DB8(&play->colCtx, polyUpDown, bgIdUpDown) & 0x30) && // && not crawl space?
        !SurfaceType_IsIgnoredByProjectiles(&play->colCtx, polyUpDown, bgIdUpDown)) {
        // forwards
        posB.x = (this->axisForwards.x * lineLength) + posA.x;
        posB.y = (this->axisForwards.y * lineLength) + posA.y;
        posB.z = (this->axisForwards.z * lineLength) + posA.z;

        if (BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, &posSide, &polySide, true, true, true, true,
                                    &bgIdSide) &&
            !(func_80041DB8(&play->colCtx, polySide, bgIdSide) & 0x30) &&
            !SurfaceType_IsIgnoredByProjectiles(&play->colCtx, polySide, bgIdSide)) {
            EnBomChu_UpdateFloorPoly(this, polySide, play);
            this->actor.world.pos = posSide;
            this->actor.floorBgId = bgIdSide;
            this->actor.speedXZ = 0.0f;
        } else {
            if (this->actor.floorPoly != polyUpDown) {
                EnBomChu_UpdateFloorPoly(this, polyUpDown, play);
            }

            this->actor.world.pos = posUpDown;
            this->actor.floorBgId = bgIdUpDown;
        }
    } else {
        this->actor.speedXZ = 0.0f;
        lineLength *= 3.0f;
        posA = posB;

        for (i = 0; i < 3; i++) {
            if (i == 0) {
                // backwards
                posB.x = posA.x - (this->axisForwards.x * lineLength);
                posB.y = posA.y - (this->axisForwards.y * lineLength);
                posB.z = posA.z - (this->axisForwards.z * lineLength);
            } else if (i == 1) {
                // left
                posB.x = posA.x + (this->axisLeft.x * lineLength);
                posB.y = posA.y + (this->axisLeft.y * lineLength);
                posB.z = posA.z + (this->axisLeft.z * lineLength);
            } else {
                // right
                posB.x = posA.x - (this->axisLeft.x * lineLength);
                posB.y = posA.y - (this->axisLeft.y * lineLength);
                posB.z = posA.z - (this->axisLeft.z * lineLength);
            }

            if (BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, &posSide, &polySide, true, true, true, true,
                                        &bgIdSide) &&
                !(func_80041DB8(&play->colCtx, polySide, bgIdSide) & 0x30) &&
                !SurfaceType_IsIgnoredByProjectiles(&play->colCtx, polySide, bgIdSide)) {
                EnBomChu_UpdateFloorPoly(this, polySide, play);
                this->actor.world.pos = posSide;
                this->actor.floorBgId = bgIdSide;
                break;
            }
        }

        if (i == 3) {
            // no collision nearby
            EnBomChu_Explode(this, play);
        }
    }

    Math_ScaledStepToS(&this->actor.shape.rot.x, -this->actor.world.rot.x, 0x800);
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 0x800);
    Math_ScaledStepToS(&this->actor.shape.rot.z, this->actor.world.rot.z, 0x800);

    func_8002F8F0(&this->actor, NA_SE_IT_BOMBCHU_MOVE - SFX_FLAG);
}

void EnBomChu_WaitForKill(EnBomChu* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        Actor_Kill(&this->actor);
    }
}

/**
 * Transform coordinates from model space to world space, according to current orientation.
 * `posModel` is expected to already be at world scale (1/100 compared to model scale)
 */
void EnBomChu_ModelToWorld(EnBomChu* this, Vec3f* posModel, Vec3f* dest) {
    f32 x = posModel->x + this->visualJitter;

    dest->x = this->actor.world.pos.x + (this->axisLeft.x * x) + (this->axisUp.x * posModel->y) +
              (this->axisForwards.x * posModel->z);
    dest->y = this->actor.world.pos.y + (this->axisLeft.y * x) + (this->axisUp.y * posModel->y) +
              (this->axisForwards.y * posModel->z);
    dest->z = this->actor.world.pos.z + (this->axisLeft.z * x) + (this->axisUp.z * posModel->y) +
              (this->axisForwards.z * posModel->z);
}

void EnBomChu_SpawnRipples(EnBomChu* this, PlayState* play, f32 y) {
    Vec3f pos;

    pos.x = this->actor.world.pos.x;
    pos.y = y;
    pos.z = this->actor.world.pos.z;

    EffectSsGRipple_Spawn(play, &pos, 70, 500, 0);
    EffectSsGRipple_Spawn(play, &pos, 70, 500, 4);
    EffectSsGRipple_Spawn(play, &pos, 70, 500, 8);
}

void EnBomChu_Update(Actor* thisx, PlayState* play2) {
    static Vec3f blureP1Model = { 0.0f, 7.0f, -6.0f };
    static Vec3f blureP2LeftModel = { 12.0f, 0.0f, -5.0f };
    static Vec3f blureP2RightModel = { -12.0f, 0.0f, -5.0f };
    PlayState* play = play2;
    EnBomChu* this = (EnBomChu*)thisx;
    s16 yaw;
    f32 sin;
    f32 cos;
    f32 tempX;
    Vec3f blureP1;
    Vec3f blureP2;
    WaterBox* waterBox;
    f32 waterY;

    if (this->actor.floorBgId != BGCHECK_SCENE) {
        yaw = this->actor.shape.rot.y;
        func_800433A4(&play->colCtx, this->actor.floorBgId, &this->actor);

        if (yaw != this->actor.shape.rot.y) {
            yaw = this->actor.shape.rot.y - yaw;

            sin = Math_SinS(yaw);
            cos = Math_CosS(yaw);

            tempX = this->axisForwards.x;
            this->axisForwards.x = (this->axisForwards.z * sin) + (cos * tempX);
            this->axisForwards.z = (this->axisForwards.z * cos) - (sin * tempX);

            tempX = this->axisUp.x;
            this->axisUp.x = (this->axisUp.z * sin) + (cos * tempX);
            this->axisUp.z = (this->axisUp.z * cos) - (sin * tempX);

            tempX = this->axisLeft.x;
            this->axisLeft.x = (this->axisLeft.z * sin) + (cos * tempX);
            this->axisLeft.z = (this->axisLeft.z * cos) - (sin * tempX);
        }
    }

    this->actionFunc(this, play);
    func_8002D97C(&this->actor);

    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

    if (this->actionFunc != EnBomChu_WaitForRelease) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    Actor_SetFocus(&this->actor, 0.0f);

    if (this->actionFunc == EnBomChu_Move) {
        this->visualJitter =
            (5.0f + (Rand_ZeroOne() * 3.0f)) * Math_SinS(((Rand_ZeroOne() * (f32)0x200) + (f32)0x3000) * this->timer);

        EnBomChu_ModelToWorld(this, &blureP1Model, &blureP1);

        EnBomChu_ModelToWorld(this, &blureP2LeftModel, &blureP2);
        EffectBlure_AddVertex(Effect_GetByIndex(this->blure1Index), &blureP1, &blureP2);

        EnBomChu_ModelToWorld(this, &blureP2RightModel, &blureP2);
        EffectBlure_AddVertex(Effect_GetByIndex(this->blure2Index), &blureP1, &blureP2);

        waterY = this->actor.world.pos.y;

        if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z,
                                 &waterY, &waterBox)) {
            this->actor.yDistToWater = waterY - this->actor.world.pos.y;

            if (this->actor.yDistToWater < 0.0f) {
                if (this->actor.bgCheckFlags & 0x20) {
                    EnBomChu_SpawnRipples(this, play, waterY);
                }

                this->actor.bgCheckFlags &= ~0x20;
            } else {
                if (!(this->actor.bgCheckFlags & 0x20) && (this->timer != 120)) {
                    EnBomChu_SpawnRipples(this, play, waterY);
                } else {
                    EffectSsBubble_Spawn(play, &this->actor.world.pos, 0.0f, 3.0f, 15.0f, 0.25f);
                }

                this->actor.bgCheckFlags |= 0x20;
            }
        } else {
            this->actor.bgCheckFlags &= ~0x20;
            this->actor.yDistToWater = BGCHECK_Y_MIN;
        }
    }
}


const Color_RGB8 BombchuColorOriginal = { 209, 34, -35 };

void EnBomChu_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBomChu* this = (EnBomChu*)thisx;
    f32 colorIntensity;
    s32 blinkHalfPeriod;
    s32 blinkTime;
    Color_RGB8 BombchuCol = CVar_GetRGB("gBombTrailCol", BombchuColorOriginal);

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);
    func_8002EBCC(&this->actor, play, 0);

    if (this->timer >= 40) {
        blinkTime = this->timer % 20;
        blinkHalfPeriod = 10;
    } else if (this->timer >= 10) {
        blinkTime = this->timer % 10;
        blinkHalfPeriod = 5;
    } else {
        blinkTime = this->timer & 1;
        blinkHalfPeriod = 1;
    }

    if (blinkTime > blinkHalfPeriod) {
        blinkTime = 2 * blinkHalfPeriod - blinkTime;
    }

    colorIntensity = blinkTime / (f32)blinkHalfPeriod;

    if (CVar_GetS32("gUseTrailsCol", 0) != 0)
        gDPSetEnvColor(POLY_OPA_DISP++, (colorIntensity * BombchuCol.r), (colorIntensity * BombchuCol.g),
                       (colorIntensity * BombchuCol.b), 255);
    else gDPSetEnvColor(POLY_OPA_DISP++, 9.0f + (colorIntensity * 209.0f), 9.0f + (colorIntensity * 34.0f),
                        35.0f + (colorIntensity * -35.0f), 255);
    Matrix_Translate(this->visualJitter * (1.0f / BOMBCHU_SCALE), 0.0f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gBombchuDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
