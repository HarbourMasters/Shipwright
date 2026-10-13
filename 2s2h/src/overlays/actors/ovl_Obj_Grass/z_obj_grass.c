/*
 * File: z_obj_grass.c
 * Overlay: ovl_Obj_Grass
 * Description: Optimized manager for processing grass "spawned" by ObjGrassUnit
 *
 * Instances of ObjGrass must be spawned by ObjGrassUnit to function correctly.
 */

#include "prevent_bss_reordering.h"
#include "z_obj_grass.h"
#include "overlays/actors/ovl_Obj_Grass_Carry/z_obj_grass_carry.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "2s2h/ShipUtils.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjGrass_Init(Actor* thisx, PlayState* play);
void ObjGrass_Destroy(Actor* thisx, PlayState* play);
void ObjGrass_Update(Actor* thisx, PlayState* play);
void ObjGrass_Draw(Actor* thisx, PlayState* play);

ObjGrassGroup* sNearestGrassGroups[OBJ_GRASS_NEAREST_GROUP_MAX];
f32 sNearestGrassGroupsDist[OBJ_GRASS_NEAREST_GROUP_MAX];
ObjGrassElement* sNearestGrassElements[OBJ_GRASS_NEAREST_ELEM_MAX];
f32 sNearestGrassElementsDistSq[OBJ_GRASS_NEAREST_ELEM_MAX];

#include "overlays/ovl_Obj_Grass/ovl_Obj_Grass.h"

ActorProfile Obj_Grass_Profile = {
    /**/ ACTOR_OBJ_GRASS,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(ObjGrass),
    /**/ ObjGrass_Init,
    /**/ ObjGrass_Destroy,
    /**/ ObjGrass_Update,
    /**/ ObjGrass_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x0580C71C, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 6, 44, 0, { 0, 0, 0 } },
};

static Vec3f sUnitDirections[] = {
    { 0.0f, 0.7071f, 0.7071f },
    { 0.7071f, 0.7071f, 0.0f },
    { 0.0f, 0.7071f, -0.7071f },
    { -0.7071f, 0.7071f, 0.0f },
};

static s16 sFragmentScales[] = { 108, 102, 96, 84, 66, 55, 42, 38 };

s32 func_809A9110(PlayState* play, Vec3f* pos) {
    f32 w;
    Vec3f projectedPos;

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, pos, &projectedPos, &w);

    // #region 2S2H [Enhancement] Extended Culling handling for grass unit drawing
    s32 distMultiplier = CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1);
    distMultiplier = MAX(distMultiplier, 1);

    f32 aspectMultiplier = 1.0f;
    if (CVarGetInteger("gEnhancements.Graphics.ActorCullingAccountsForWidescreen", 0)) {
        aspectMultiplier = Ship_GetExtendedAspectRatioMultiplier();
    }

    if ((play->projectionMtxFDiagonal.z * -130.13191f * distMultiplier) < projectedPos.z) {
        if (w < 1.0f) {
            w = 1.0f;
        }

        if (((fabsf(projectedPos.x) - (130.13191f * play->projectionMtxFDiagonal.x)) < (w * aspectMultiplier)) &&
            ((fabsf(projectedPos.y) - (130.13191f * play->projectionMtxFDiagonal.y)) < w)) {
            return true;
        }
    }
    // #endregion
    return false;
}

void ObjGrass_OverrideMatrixCurrent(MtxF* matrix) {
    s32 i;
    MtxF* temp = Matrix_GetCurrent();
    f32* tmp = &temp->xx;
    f32* tmp2 = &matrix->xx;

    for (i = 0; i < 16; i++) {
        *tmp++ += *tmp2++;
    }
}

void ObjGrass_DropCollectible(ObjGrassElement* grassElem, PlayState* play) {
    if (GameInteractor_Should(VB_GRASS_DROP_COLLECTIBLE, true, ACTOR_OBJ_GRASS, grassElem)) {
        if (!(grassElem->dropTable & 0x10)) {
            Item_DropCollectibleRandom(play, NULL, &grassElem->pos, grassElem->dropTable * 0x10);
        }
    }
}

void ObjGrass_SpawnFragments(Vec3f* basePos, PlayState* play) {
    Vec3f velocity;
    Vec3f pos;
    s32 i;
    Vec3f* dir;

    for (i = 0; i < ARRAY_COUNT(sUnitDirections); i++) {
        dir = &sUnitDirections[i];

        pos.x = basePos->x + (dir->x * 8.0f);
        pos.y = basePos->y + (dir->y * 8.0f) + 10.0f;
        pos.z = basePos->z + (dir->z * 8.0f);

        velocity.x = (Rand_ZeroOne() - 0.5f) * 8.0f;
        velocity.y = Rand_ZeroOne() * 10.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 8.0f;

        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -100, 64, 40, 3, 0,
                             sFragmentScales[(s32)(Rand_ZeroOne() * 111.1f) & 7], 0, 0, 80, -1, GAMEPLAY_KEEP,
                             gKakeraLeafMiddleDL);

        pos.x = basePos->x + (dir->x * 16.0f);
        pos.y = basePos->y + (dir->y * 16.0f) + 10.0f;
        pos.z = basePos->z + (dir->z * 16.0f);

        velocity.x = (Rand_ZeroOne() - 0.5f) * 6.0f;
        velocity.y = Rand_ZeroOne() * 10.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 6.0f;

        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -100, 64, 40, 3, 0,
                             sFragmentScales[(s32)(Rand_ZeroOne() * 111.1f) % 7], 0, 0, 80, -1, GAMEPLAY_KEEP,
                             gKakeraLeafTipDL);
    }
}

void ObjGrass_Init(Actor* thisx, PlayState* play) {
    ObjGrass* this = (ObjGrass*)thisx;
    s32 i;

    Actor_SetScale(&this->actor, 0.4f);

    for (i = 0; i < ARRAY_COUNT(this->grassElemColliders); i++) {
        Collider_InitCylinder(play, &this->grassElemColliders[i].collider);
        Collider_SetCylinder(play, &this->grassElemColliders[i].collider, &this->actor, &sCylinderInit);
    }

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_3288 = Rand_Next() >> 0x10;
    this->unk_328A = Rand_Next() >> 0x10;
    this->unk_328C = Rand_Next() >> 0x10;
    this->unk_328E = Rand_Next() >> 0x10;
    this->unk_3290 = Rand_Next() >> 0x10;
}

void ObjGrass_Destroy(Actor* thisx, PlayState* play) {
    ObjGrass* this = (ObjGrass*)thisx;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->grassElemColliders); i++) {
        Collider_DestroyCylinder(play, &this->grassElemColliders[i].collider);
    }

    for (i = 0; i < ARRAY_COUNT(this->grassCarry); i++) {
        ObjGrassCarry** grassCarry = &this->grassCarry[i];

        if (*grassCarry != NULL) {
            (*grassCarry)->grassManager = NULL;
            *grassCarry = NULL;
        }
    }
}

void ObjGrass_ProcessColliders(ObjGrass* this, PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->grassElemColliders); i++) {
        ObjGrassElement* grassElem = this->grassElemColliders[i].entity;

        if ((grassElem != NULL) && (this->grassElemColliders[i].collider.base.acFlags & AC_HIT)) {
            ObjGrass_SpawnFragments(&grassElem->pos, play);
            ObjGrass_DropCollectible(grassElem, play);
            grassElem->flags |= OBJ_GRASS_ELEM_REMOVED;
            SoundSource_PlaySfxAtFixedWorldPos(play, &grassElem->pos, 20, NA_SE_EV_PLANT_BROKEN);
        }
    }
}

void ObjGrass_UpdateGrass(ObjGrass* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    f32 distSq;
    ObjGrassGroup* grassGroup;
    s16 yaw;

    for (i = 0; i < OBJ_GRASS_NEAREST_ELEM_MAX; i++) {
        sNearestGrassElements[i] = NULL;
        sNearestGrassElementsDistSq[i] = SQ(650.0f);
    }

    for (i = 0; i < OBJ_GRASS_NEAREST_GROUP_MAX; i++) {
        sNearestGrassGroups[i] = NULL;
        sNearestGrassGroupsDist[i] = SQ(650.0f);
    }

    for (i = 0; i < this->activeGrassGroups; i++) {
        grassGroup = &this->grassGroups[i];

        for (j = 0; j < grassGroup->count; j++) {
            grassGroup->elements[j].flags &= (u16)~OBJ_GRASS_ELEM_ANIM;
        }
    }

    for (i = 0; i < this->activeGrassGroups; i++) {
        grassGroup = &this->grassGroups[i];
        distSq = Math3D_Vec3fDistSq(&grassGroup->homePos, &player->actor.world.pos);

        for (j = 0; j < OBJ_GRASS_NEAREST_GROUP_MAX; j++) {
            if (distSq < sNearestGrassGroupsDist[j]) {
                break;
            }
        }

        if (j < OBJ_GRASS_NEAREST_GROUP_MAX) {
            // Insert the grassGroup into the list

            // Shift each element under j that remains in the list down 1
            for (x = OBJ_GRASS_NEAREST_GROUP_MAX - 2; x >= j; x--) {
                sNearestGrassGroupsDist[x + 1] = sNearestGrassGroupsDist[x];
                sNearestGrassGroups[x + 1] = sNearestGrassGroups[x];
            }

            sNearestGrassGroupsDist[j] = distSq;
            sNearestGrassGroups[j] = grassGroup;
        }
    }

    this->carryGrassElem = NULL;

    for (i = 0; i < OBJ_GRASS_NEAREST_GROUP_MAX; i++) {
        grassGroup = sNearestGrassGroups[i];

        if (grassGroup != NULL) {
            for (j = 0; j < grassGroup->count; j++) {
                if (!(grassGroup->elements[j].flags & OBJ_GRASS_ELEM_REMOVED)) {
                    distSq = Math3D_Vec3fDistSq(&grassGroup->elements[j].pos, &player->actor.world.pos);

                    for (x = 0; x < OBJ_GRASS_NEAREST_ELEM_MAX; x++) {
                        if (distSq < sNearestGrassElementsDistSq[x]) {
                            break;
                        }
                    }

                    if (x < OBJ_GRASS_NEAREST_ELEM_MAX) {
                        // Insert the GrassElement into the list

                        // Shift each element under j that remains in the list down 1
                        for (y = OBJ_GRASS_NEAREST_ELEM_MAX - 2; y >= x; y--) {
                            sNearestGrassElementsDistSq[y + 1] = sNearestGrassElementsDistSq[y];
                            sNearestGrassElements[y + 1] = sNearestGrassElements[y];
                        }

                        sNearestGrassElementsDistSq[x] = distSq;
                        sNearestGrassElements[x] = &grassGroup->elements[j];

                        if (distSq < SQ(50.0f)) {
                            yaw = player->actor.shape.rot.y -
                                  Math_Vec3f_Yaw(&player->actor.world.pos, &grassGroup->elements[j].pos);

                            if (ABS_ALT(yaw) < 0x2000) {
                                this->carryGrassElem = &grassGroup->elements[j];
                            }
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < ARRAY_COUNT(this->grassElemColliders); i++) {
        ObjGrassCollider* grassCol = &this->grassElemColliders[i];
        ObjGrassElement* grassElem;

        grassCol->collider.base.acFlags &= (u16)~AC_HIT;
        grassCol->collider.base.ocFlags1 &= (u16)~OC1_HIT;

        grassElem = sNearestGrassElements[i];
        if (grassElem != NULL) {
            grassCol->collider.dim.pos.x = grassElem->pos.x;
            grassCol->collider.dim.pos.y = grassElem->pos.y;
            grassCol->collider.dim.pos.z = grassElem->pos.z;
            CollisionCheck_SetAC(play, &play->colChkCtx, &grassCol->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &grassCol->collider.base);
            grassElem->flags |= OBJ_GRASS_ELEM_ANIM;
            grassCol->entity = grassElem;
        } else {
            grassCol->entity = NULL;
        }
    }
}

void ObjGrass_CalcAnimationMatrices(ObjGrass* this) {
    s32 i;
    s32 pad;
    f32* ptr;
    f32 temp_f28;
    f32 spA4;
    f32 spA0;
    f32 temp_f22;
    f32 temp_f24;
    f32 sp94;
    f32 sp90;
    f32 temp_f26;
    f32 sp6C[8];
    f32 tempf1;
    f32 tempf2;
    f32 tempf3;
    f32 tempf4;
    f32 tempf5;
    f32 temp_f16;
    f32 temp_f20;

    this->unk_3288 += 70;
    this->unk_328A += 300;
    this->unk_328C += 700;
    this->unk_328E += 1300;
    this->unk_3290 += 8900;

    temp_f24 = Math_SinS(this->unk_3288);
    temp_f28 = Math_SinS(this->unk_328A);
    temp_f26 = Math_SinS(this->unk_328C);
    spA4 = Math_SinS(this->unk_328E) * 1.2f;
    spA0 = Math_SinS(this->unk_3290) * 1.5f;
    temp_f22 = Math_CosS(this->unk_3288);
    temp_f20 = Math_CosS(this->unk_328A);
    sp94 = Math_CosS(this->unk_328C);
    sp90 = Math_CosS(this->unk_328E) * 1.3f;
    temp_f16 = Math_CosS(this->unk_3290) * 1.7f;

    sp6C[0] = (temp_f24 - temp_f20) * temp_f26 * temp_f22 * temp_f24 * 0.0015f;
    sp6C[1] = (temp_f28 - sp94) * spA4 * temp_f20 * temp_f24 * 0.0015f;
    sp6C[2] = (temp_f26 - sp90) * sp94 * temp_f24 * temp_f22 * 0.0015f;
    sp6C[3] = (spA4 - temp_f20) * sp90 * temp_f28 * temp_f22 * 0.0015f;
    sp6C[4] = (temp_f24 - sp94) * temp_f24 * temp_f28 * spA0 * 0.0015f;
    sp6C[5] = (temp_f28 - sp90) * temp_f26 * spA4 * spA0 * 0.0015f;
    sp6C[6] = (temp_f26 - temp_f22) * temp_f22 * temp_f20 * temp_f16 * 0.0015f;
    sp6C[7] = (spA4 - temp_f20) * sp94 * sp90 * temp_f16 * 0.0015f;

    for (i = 0; i < ARRAY_COUNT(this->distortionMtx); i++) {
        ptr = &this->distortionMtx[i].mf[0][0];

        tempf1 = sp6C[(i + 0) & 7];
        tempf2 = sp6C[(i + 1) & 7];
        tempf3 = sp6C[(i + 2) & 7];
        tempf4 = sp6C[(i + 3) & 7];
        tempf5 = sp6C[(i + 4) & 7];

        ptr[0] = sp6C[1] * 0.2f;
        ptr[1] = tempf1;
        ptr[2] = tempf2;
        ptr[3] = 0.0f;

        ptr[4] = tempf3;
        ptr[5] = sp6C[0];
        ptr[6] = tempf3;
        ptr[7] = 0.0f;

        ptr[8] = tempf4;
        ptr[9] = tempf5;
        ptr[10] = sp6C[3] * 0.2f;
        ptr[11] = 0.0f;

        ptr[12] = 0.0f;
        ptr[13] = 0.0f;
        ptr[14] = 0.0f;
        ptr[15] = 0.0f;
    }
}

void ObjGrass_Update(Actor* thisx, PlayState* play) {
    ObjGrass* this = (ObjGrass*)thisx;

    ObjGrass_ProcessColliders(this, play);
    ObjGrass_UpdateGrass(this, play);
    ObjGrass_CalcAnimationMatrices(this);
}

void ObjGrass_InitDraw(ObjGrass* this, PlayState* play) {
    ObjGrassGroup* grassGroup;
    ObjGrassElement* grassElem;
    s32 i;
    s32 j;
    f32 distSq;
    f32 eyeDist;

    // 2S2H [Enhancement] Extended Culling handling
    // Recompute the eyeDist and distSq values below with the inverse distance multiplier to trick the checks
    s32 multiplier = CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1);
    multiplier = MAX(multiplier, 1);

    for (i = 0; i < this->activeGrassGroups; i++) {
        grassGroup = &this->grassGroups[i];

        eyeDist = Math3D_Vec3fDistSq(&grassGroup->homePos, &GET_ACTIVE_CAM(play)->eye);
        eyeDist = SQ(sqrtf(eyeDist) / multiplier); // Hijacked

        if ((eyeDist < SQ(1280.0f)) && func_809A9110(play, &grassGroup->homePos)) {
            grassGroup->flags |= OBJ_GRASS_GROUP_DRAW;

            for (j = 0; j < grassGroup->count; j++) {
                grassElem = &grassGroup->elements[j];

                if (grassElem->flags & OBJ_GRASS_ELEM_REMOVED) {
                    grassElem->alpha = 255;
                    grassElem->flags &= ~OBJ_GRASS_ELEM_DRAW;
                } else {
                    grassElem->flags |= OBJ_GRASS_ELEM_DRAW;
                    if (eyeDist < SQ(980.0f)) {
                        grassElem->alpha = 255;
                    } else {
                        distSq = Math3D_Vec3fDistSq(&grassElem->pos, &GET_ACTIVE_CAM(play)->eye);
                        distSq = SQ(sqrtf(distSq) / multiplier); // Hijacked
                        if ((distSq <= SQ(1080.0f)) ||
                            ((grassElem->flags & OBJ_GRASS_ELEM_UNDERWATER) && (distSq < SQ(1180.0f)))) {
                            grassElem->alpha = 255;
                        } else if (distSq >= SQ(1180.0f)) {
                            grassElem->alpha = 0;
                        } else {
                            grassElem->alpha = (1180.0f - sqrtf(distSq)) * 2.55f;
                        }
                    }
                }
            }
        } else {
            grassGroup->flags &= ~OBJ_GRASS_GROUP_DRAW;
        }
    }
}

void ObjGrass_DrawOpa(Actor* thisx, PlayState* play2) {
    ObjGrass* this = (ObjGrass*)thisx;
    PlayState* play = play2;
    Lights* lights;
    ObjGrassGroup* grassGroup;
    s32 i;
    s32 j;
    Vec3s rot = { 0, 0, 0 };
    ObjGrassElement* grassElem;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gSPDisplayList(POLY_OPA_DISP++, gObjGrass_D_809AA9F0);

    for (i = 0; i < this->activeGrassGroups; i++) {
        grassGroup = &this->grassGroups[i];

        if (grassGroup->flags & OBJ_GRASS_GROUP_DRAW) {
            lights = LightContext_NewLights(&play->lightCtx, play->state.gfxCtx);
            Lights_BindAll(lights, play->lightCtx.listHead, &grassGroup->homePos, play);
            Lights_Draw(lights, play->state.gfxCtx);

            for (j = 0; j < grassGroup->count; j++) {
                grassElem = &grassGroup->elements[j];

                if ((grassElem->flags & OBJ_GRASS_ELEM_DRAW) && (grassElem->alpha == 255)) {
                    FrameInterpolation_RecordOpenChild(grassElem, 0);
                    if (GameInteractor_Should(VB_OBJGRASS_OPA_DRAW_BE_OVERRIDDEN, true, this, grassElem, j)) {
                        rot.y = grassElem->rotY;
                        Matrix_SetTranslateRotateYXZ(grassElem->pos.x, grassElem->pos.y, grassElem->pos.z, &rot);
                        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
                        if (grassElem->flags & OBJ_GRASS_ELEM_ANIM) {
                            ObjGrass_OverrideMatrixCurrent(&this->distortionMtx[j]);
                        }

                        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                        gSPDisplayList(POLY_OPA_DISP++, gObjGrass_D_809AAAE0);
                    }
                    FrameInterpolation_RecordCloseChild();
                }
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjGrass_DrawXlu(Actor* thisx, PlayState* play) {
    ObjGrass* this = (ObjGrass*)thisx;
    ObjGrassGroup* grassGroup;
    ObjGrassElement* grassElem;
    s32 i;
    s32 j;
    Vec3s rot = { 0, 0, 0 };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gObjGrass_D_809AAA68);

    for (i = 0; i < this->activeGrassGroups; i++) {
        grassGroup = &this->grassGroups[i];

        if (grassGroup->flags & OBJ_GRASS_GROUP_DRAW) {
            for (j = 0; j < grassGroup->count; j++) {
                grassElem = &grassGroup->elements[j];

                if ((grassElem->flags & OBJ_GRASS_ELEM_DRAW) && (grassElem->alpha > 0) && (grassElem->alpha < 255)) {
                    FrameInterpolation_RecordOpenChild(grassElem, 0);
                    if (GameInteractor_Should(VB_OBJGRASS_XLU_DRAW_BE_OVERRIDDEN, true, this, grassElem)) {
                        rot.y = grassElem->rotY;
                        Matrix_SetTranslateRotateYXZ(grassElem->pos.x, grassElem->pos.y, grassElem->pos.z, &rot);
                        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

                        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, grassElem->alpha);
                        gSPDisplayList(POLY_XLU_DISP++, gObjGrass_D_809AAAE0);
                    }
                    FrameInterpolation_RecordCloseChild();
                }
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjGrass_Draw(Actor* thisx, PlayState* play) {
    ObjGrass* this = (ObjGrass*)thisx;

    ObjGrass_InitDraw(this, play);
    ObjGrass_DrawOpa(thisx, play);
    ObjGrass_DrawXlu(thisx, play);
}
