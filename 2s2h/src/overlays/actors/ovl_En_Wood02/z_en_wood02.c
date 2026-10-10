/*
 * File: z_en_wood02.c
 * Overlay: ovl_En_Wood02
 * Description: Trees, shrubs
 */

#include "z_en_wood02.h"
#include "objects/object_wood02/object_wood02.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS 0x00000000

void EnWood02_Init(Actor* thisx, PlayState* play);
void EnWood02_Destroy(Actor* thisx, PlayState* play);
void EnWood02_Update(Actor* thisx, PlayState* play2);
void EnWood02_Draw(Actor* thisx, PlayState* play);

/**
 * WOOD_SPAWN_SPAWNER is also used by some individual trees: EnWood02_Update also checks for parent before running any
 * despawning code.
 *  */
typedef enum {
    /* 0 */ WOOD_SPAWN_NORMAL,
    /* 1 */ WOOD_SPAWN_SPAWNED,
    /* 2 */ WOOD_SPAWN_SPAWNER
} WoodSpawnType;

typedef enum {
    /* 0 */ WOOD_DRAW_TREE_CONICAL,
    /* 1 */ WOOD_DRAW_TREE_OVAL,
    /* 2 */ WOOD_DRAW_TREE_KAKARIKO_ADULT,
    /* 3 */ WOOD_DRAW_BUSH_GREEN,
    /* 4 */ WOOD_DRAW_4, // Used for black bushes and green leaves
    /* 5 */ WOOD_DRAW_LEAF_YELLOW
} WoodDrawType;

f32 sWood02SpawnCos;
f32 sWood02SpawnSin;

ActorProfile En_Wood02_Profile = {
    /**/ ACTOR_EN_WOOD02,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_WOOD02,
    /**/ sizeof(EnWood02),
    /**/ EnWood02_Init,
    /**/ EnWood02_Destroy,
    /**/ EnWood02_Update,
    /**/ EnWood02_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_TREE,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK5,
        { 0x00000000, 0x00, 0x00 },
        { 0x0100020A, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 60, 0, { 0, 0, 0 } },
};

f32 sWood02SpawnDistance[] = { 707.0f, 525.0f, 510.0f, 500.0f, 566.0f, 141.0f };

s16 sWood02SpawnAngle[] = { 0x1FFF, 0x4C9E, 0x77F5, 0xA5C9, -0x293D, 0xA000 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 5600, ICHAIN_STOP),
};

Gfx* D_808C4D54[] = {
    object_wood02_DL_0078D0, object_wood02_DL_007CA0, object_wood02_DL_008160, object_wood02_DL_000090,
    object_wood02_DL_000340, object_wood02_DL_000340, object_wood02_DL_000700,
};

Gfx* D_808C4D70[] = {
    object_wood02_DL_007968,
    object_wood02_DL_007D38,
    object_wood02_DL_0080D0,
    NULL,
    NULL,
    NULL,
    object_wood02_DL_007AD0,
    object_wood02_DL_007E20,
    object_wood02_DL_009340,
    object_wood02_DL_000160,
    object_wood02_DL_000440,
    object_wood02_DL_000700,
};

s32 EnWood02_SpawnZoneCheck(EnWood02* this, PlayState* play, Vec3f* arg2) {
    f32 phi_f12;

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, arg2, &this->actor.projectedPos, &this->actor.projectedW);

    // 2S2H [Enhancement] Use the extended culling calculation
    if (CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1) > 1 ||
        CVarGetInteger("gEnhancements.Graphics.ActorCullingAccountsForWidescreen", 0)) {
        bool shipShouldDraw = false;
        bool shipShouldUpdate = false;
        return Ship_CalcShouldDrawAndUpdate(play, &this->actor, &this->actor.projectedPos, this->actor.projectedW,
                                            &shipShouldDraw, &shipShouldUpdate);
    }

    if (this->actor.projectedW == 0.0f) {
        phi_f12 = 1000.0f;
    } else {
        phi_f12 = fabsf(1.0f / this->actor.projectedW);
    }

    if (((-this->actor.cullingVolumeScale < this->actor.projectedPos.z) &&
         (this->actor.projectedPos.z < (this->actor.cullingVolumeDistance + this->actor.cullingVolumeScale)) &&
         (((fabsf(this->actor.projectedPos.x) - this->actor.cullingVolumeScale) * phi_f12) < 1.0f)) &&
        (((this->actor.projectedPos.y + this->actor.cullingVolumeDownward) * phi_f12) > -1.0f) &&
        (((this->actor.projectedPos.y - this->actor.cullingVolumeScale) * phi_f12) < 1.0f)) {
        return true;
    }
    return false;
}

/** Spawns similar-looking trees or bushes only when the player is sufficiently close. Presumably done this way to keep
 * memory usage down in Hyrule Field. */
void EnWood02_SpawnOffspring(EnWood02* this, PlayState* play) {
    EnWood02* child;
    s32 extraRot;
    Vec3f childPos;
    s32 unk;
    s32 i;
    s16 childParams;

    for (i = 4; i >= 0; i--) {
        extraRot = 0;
        if (!(this->unk_14A[i] & 0x7F)) {
            if (this->actor.params == WOOD_BUSH_GREEN_LARGE_SPAWNER) {
                extraRot = 0x4000;
            }

            sWood02SpawnCos = Math_CosS(sWood02SpawnAngle[i] + this->actor.world.rot.y + extraRot);
            sWood02SpawnSin = Math_SinS(sWood02SpawnAngle[i] + this->actor.world.rot.y + extraRot);

            childPos.x = (sWood02SpawnDistance[i] * sWood02SpawnSin) + this->actor.home.pos.x;
            childPos.y = this->actor.home.pos.y;
            childPos.z = (sWood02SpawnDistance[i] * sWood02SpawnCos) + this->actor.home.pos.z;

            if (EnWood02_SpawnZoneCheck(this, play, &childPos)) {
                if (this->unk_14A[i] & 0x80) {
                    unk = 0;
                } else {
                    unk = this->actor.home.rot.z;
                }

                childParams = ((this->unk_144 << 8) & 0xFF00) | (this->actor.params + 1);
                child = (EnWood02*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_WOOD02, childPos.x,
                                                      childPos.y, childPos.z, this->actor.world.rot.x,
                                                      sWood02SpawnAngle[i], unk, childParams);
                if (child != NULL) {
                    child->unk_14A[0] = i;
                    this->unk_14A[i] |= 1;
                    child->actor.projectedPos = this->actor.projectedPos;
                } else {
                    this->unk_14A[i] &= 0x80;
                }
            }
        }
    }
}

void EnWood02_Init(Actor* thisx, PlayState* play) {
    s16 spawnType = 0;
    f32 actorScale = 1.0f;
    EnWood02* this = (EnWood02*)thisx;
    s32 pad;
    CollisionPoly* outPoly;
    s32 bgId;
    f32 floorY;
    s16 extraRot;

    this->actor.world.rot.z = 0;
    this->unk_144 = ENWOOD02_GET_FF00(&this->actor);
    this->unk_146 = -1;
    this->actor.shape.rot.z = this->actor.world.rot.z;

    if (this->unk_144 & 0x80) {
        this->unk_144 = -1;
    }

    this->actor.params &= 0xFF;
    this->unk_148 = this->actor.home.rot.z;

    Actor_ProcessInitChain(&this->actor, sInitChain);

    if ((this->actor.params < WOOD_BUSH_GREEN_SMALL) || (this->actor.params == WOOD_TREE_SPECIAL)) {
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    }

    if ((this->actor.params == WOOD_TREE_SPECIAL) && !gSaveContext.save.isNight && (this->unk_144 != -1) &&
        !Flags_GetCollectible(play, this->unk_144)) {
        this->actor.child =
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_ANI, this->actor.world.pos.x,
                               this->actor.world.pos.y + 120.0f, this->actor.world.pos.z - 15.0f, 0, 0, 0, 1);
        if (this->actor.child != NULL) {
            this->actor.child->csId = this->actor.csId;
        }
        this->unk_151 = 1;
    } else {
        this->actor.child = NULL;
        this->unk_151 = 0;
    }

    switch (this->actor.params) {
        case WOOD_BUSH_GREEN_LARGE_SPAWNER:
        case WOOD_BUSH_BLACK_LARGE_SPAWNER:
            spawnType = 1;
            // fallthrough
        case WOOD_BUSH_GREEN_LARGE_SPAWNED:
        case WOOD_BUSH_BLACK_LARGE_SPAWNED:
            spawnType++;
            // fallthrough
        case WOOD_TREE_CONICAL_LARGE:
        case WOOD_BUSH_GREEN_LARGE:
        case WOOD_BUSH_BLACK_LARGE:
        case WOOD_TREE_SPECIAL:
            actorScale = 1.5f;
            this->actor.cullingVolumeDistance = 4000.0f;
            this->actor.cullingVolumeScale = 2000.0f;
            this->actor.cullingVolumeDownward = 2400.0f;
            break;

        case WOOD_TREE_CONICAL_SPAWNER:
        case WOOD_TREE_OVAL_YELLOW_SPAWNER:
        case WOOD_TREE_OVAL_GREEN_SPAWNER:
        case WOOD_BUSH_GREEN_SMALL_SPAWNER:
        case WOOD_BUSH_BLACK_SMALL_SPAWNER:
            spawnType = 1;
            // fallthrough
        case WOOD_TREE_CONICAL_SPAWNED:
        case WOOD_TREE_OVAL_YELLOW_SPAWNED:
        case WOOD_TREE_OVAL_GREEN_SPAWNED:
        case WOOD_BUSH_GREEN_SMALL_SPAWNED:
        case WOOD_BUSH_BLACK_SMALL_SPAWNED:
            spawnType++;
            // fallthrough
        case WOOD_TREE_CONICAL_MEDIUM:
        case WOOD_TREE_OVAL_GREEN:
        case WOOD_TREE_KAKARIKO_ADULT:
        case WOOD_BUSH_GREEN_SMALL:
        case WOOD_BUSH_BLACK_SMALL:
            this->actor.cullingVolumeDistance = 4000.0f;
            this->actor.cullingVolumeScale = 800.0f;
            this->actor.cullingVolumeDownward = 1800.0f;
            break;

        case WOOD_TREE_CONICAL_SMALL:
            actorScale = 0.6f;
            this->actor.cullingVolumeDistance = 4000.0f;
            this->actor.cullingVolumeScale = 400.0f;
            this->actor.cullingVolumeDownward = 1000.0f;
            break;

        case WOOD_LEAF_GREEN:
        case WOOD_LEAF_YELLOW:
            this->unk_14A[0] = 75;
            actorScale = 0.02f;
            this->actor.velocity.x = Rand_CenteredFloat(6.0f);
            this->actor.velocity.z = Rand_CenteredFloat(6.0f);
            this->actor.velocity.y = (Rand_ZeroOne() * 1.25f) + -3.1f;
            break;

        default:
            break;
    }

    if (this->actor.params < WOOD_TREE_OVAL_GREEN) {
        this->drawType = WOOD_DRAW_TREE_CONICAL;
    } else if (this->actor.params < WOOD_TREE_KAKARIKO_ADULT) {
        this->drawType = WOOD_DRAW_TREE_OVAL;
    } else if (this->actor.params < WOOD_BUSH_GREEN_SMALL) {
        this->drawType = WOOD_DRAW_TREE_KAKARIKO_ADULT;
    } else if (this->actor.params < WOOD_BUSH_BLACK_SMALL) {
        this->drawType = WOOD_DRAW_BUSH_GREEN;
    } else if (this->actor.params < WOOD_LEAF_YELLOW) {
        this->drawType = WOOD_DRAW_4;
    } else if (this->actor.params == WOOD_TREE_SPECIAL) {
        this->drawType = WOOD_DRAW_TREE_CONICAL;
    } else {
        this->drawType = WOOD_DRAW_LEAF_YELLOW;
    }

    Actor_SetScale(&this->actor, actorScale);
    this->spawnType = spawnType;

    if (spawnType != WOOD_SPAWN_NORMAL) {
        extraRot = 0;
        if (this->actor.params == WOOD_BUSH_GREEN_LARGE_SPAWNER) {
            extraRot = 0x4000;
        }

        if (spawnType == WOOD_SPAWN_SPAWNER) {
            EnWood02_SpawnOffspring(this, play);
            sWood02SpawnCos = Math_CosS(sWood02SpawnAngle[5] + this->actor.world.rot.y + extraRot);
            sWood02SpawnSin = Math_SinS(sWood02SpawnAngle[5] + this->actor.world.rot.y + extraRot);
            this->actor.world.pos.x += sWood02SpawnSin * sWood02SpawnDistance[5];
            this->actor.world.pos.z += sWood02SpawnCos * sWood02SpawnDistance[5];
        } else {
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->unk_151 = 2;
        }

        // Snap to floor, or remove if over void
        this->actor.world.pos.y += 200.0f;
        floorY = BgCheck_EntityRaycastFloor5(&play->colCtx, &outPoly, &bgId, &this->actor, &this->actor.world.pos);

        if (floorY > BGCHECK_Y_MIN) {
            this->actor.world.pos.y = floorY;
        } else {
            Actor_Kill(&this->actor);
            return;
        }
    }

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    this->actor.home.rot.y = 0;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
}

void EnWood02_Destroy(Actor* thisx, PlayState* play) {
    EnWood02* this = (EnWood02*)thisx;

    if ((this->actor.params < WOOD_BUSH_GREEN_SMALL) || (this->actor.params == WOOD_TREE_SPECIAL)) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void func_808C4458(EnWood02* this, PlayState* play, Vec3f* arg2, u16 arg3) {
    s32 pad[2];
    s32 sp24;

    if (this->unk_148 != 0) {
        if ((this->unk_144 == -1) || !Flags_GetCollectible(play, this->unk_144)) {
            if ((this->unk_148 < 0) && (this->unk_148 >= -0x10)) {
                if (arg3 & 1) {
                    Item_DropCollectibleRandom(play, &this->actor, arg2, ((-1 - this->unk_148) * 0x10) | 0x8000);
                } else {
                    Item_DropCollectibleRandom(play, &this->actor, arg2, (-1 - this->unk_148) * 0x10);
                }
            } else if (this->unk_148 > 0) {
                sp24 = func_800A8150(this->unk_148 - 1);
                if (sp24 >= 0) {
                    if (this->unk_144 == -1) {
                        Item_DropCollectible(play, arg2, sp24);
                    } else {
                        Item_DropCollectible(play, arg2, ((this->unk_144 & 0x7F) << 8) | sp24);
                    }
                }
            }
        }
        this->unk_148 = 0;
    }
}

void EnWood02_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnWood02* this = (EnWood02*)thisx;
    f32 wobbleAmplitude;

    // Despawn extra trees in a group if out of range
    if ((this->spawnType == WOOD_SPAWN_SPAWNED) && (thisx->parent != NULL)) {
        if (!(thisx->flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) && (this->unk_151 != 2)) {
            s32 index = this->unk_14A[0];
            s32 phi_v0 = 0;

            if (this->unk_148 == 0) {
                phi_v0 = 0x80;
            }

            ((EnWood02*)thisx->parent)->unk_14A[index] = phi_v0;

            Actor_Kill(thisx);
            return;
        }
        this->unk_151 = 0;
    } else if (this->spawnType == WOOD_SPAWN_SPAWNER) {
        EnWood02_SpawnOffspring(this, play);
    }

    if ((thisx->params < WOOD_BUSH_GREEN_SMALL) || (thisx->params == WOOD_TREE_SPECIAL)) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            Actor_PlaySfx(thisx, NA_SE_IT_REFLECTION_WOOD);
        }

        if (thisx->home.rot.y != 0) {
            Vec3f dropsSpawnPt = thisx->world.pos;

            dropsSpawnPt.y += 200.0f;

            func_808C4458(this, play, &dropsSpawnPt, 0);

            // Spawn falling leaves
            if (this->unk_146 >= -1) {
                s32 i;
                s32 leavesParams = WOOD_LEAF_GREEN;

                if ((thisx->params == WOOD_TREE_OVAL_YELLOW_SPAWNER) ||
                    (thisx->params == WOOD_TREE_OVAL_YELLOW_SPAWNED)) {
                    leavesParams = WOOD_LEAF_YELLOW;
                }
                Actor_PlaySfx(thisx, NA_SE_EV_TREE_SWING);

                for (i = 3; i >= 0; i--) {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_WOOD02, dropsSpawnPt.x, dropsSpawnPt.y, dropsSpawnPt.z,
                                0, Rand_CenteredFloat(0xFFFF), 0, leavesParams);
                }
            }

            if ((thisx->child != NULL) && (this->unk_151 == 1)) {
                thisx->child->home.rot.x = 1;
                this->unk_151 = 0;
            }

            if ((thisx->params == WOOD_TREE_SPECIAL) && (this->unk_144 != -1)) {
                Flags_SetCollectible(play, this->unk_144);
                this->unk_144 = -1;
            }
            this->unk_146 = -0x15;
            thisx->home.rot.y = 0;
        }

        if (thisx->xzDistToPlayer < 600.0f) {
            Collider_UpdateCylinder(thisx, &this->collider);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
    } else if ((thisx->params < WOOD_LEAF_GREEN) || (thisx->params == WOOD_TREE_SPECIAL)) { // not leaves
        Player* player = GET_PLAYER(play);

        if ((this->unk_146 >= -1) &&
            (((player->rideActor == NULL) && (sqrtf(thisx->xyzDistToPlayerSq) < 20.0f) && (player->speedXZ != 0.0f)) ||
             ((player->rideActor != NULL) && (sqrtf(thisx->xyzDistToPlayerSq) < 60.0f) &&
              (player->rideActor->speed != 0.0f)))) {
            func_808C4458(this, play, &thisx->world.pos, 1);
            this->unk_146 = -0x15;
            Actor_PlaySfx(thisx, NA_SE_EV_TREE_SWING);
        }
    } else { // Leaves
        this->unk_146++;
        Math_ApproachF(&thisx->velocity.x, 0.0f, 1.0f, 5.0f * 0.01f);
        Math_ApproachF(&thisx->velocity.z, 0.0f, 1.0f, 5.0f * 0.01f);
        Actor_UpdatePos(thisx);
        thisx->shape.rot.z = Math_SinS(this->unk_146 * 0xBB8) * 16384.0f;
        this->unk_14A[0]--;
        if (this->unk_14A[0] == 0) {
            Actor_Kill(thisx);
        }
    }

    // Wobble from impact
    if (this->unk_146 < -1) {
        this->unk_146++;
        wobbleAmplitude = Math_SinS((this->unk_146 ^ 0xFFFF) * 0x3332) * 250.0f;
        thisx->shape.rot.x = Math_CosS(thisx->yawTowardsPlayer - thisx->shape.rot.y) * wobbleAmplitude;
        thisx->shape.rot.z = Math_SinS(thisx->yawTowardsPlayer - thisx->shape.rot.y) * wobbleAmplitude;
    }
}

void EnWood02_Draw(Actor* thisx, PlayState* play) {
    EnWood02* this = (EnWood02*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u8 red;
    u8 green;
    u8 blue;

    OPEN_DISPS(gfxCtx);

    if ((thisx->params == WOOD_TREE_OVAL_GREEN_SPAWNER) || (thisx->params == WOOD_TREE_OVAL_GREEN_SPAWNED) ||
        (thisx->params == WOOD_TREE_OVAL_GREEN) || (thisx->params == WOOD_LEAF_GREEN)) {
        red = 50;
        green = 170;
        blue = 70;
    } else if ((thisx->params == WOOD_TREE_OVAL_YELLOW_SPAWNER) || (thisx->params == WOOD_TREE_OVAL_YELLOW_SPAWNED) ||
               (thisx->params == WOOD_LEAF_YELLOW)) {
        red = 180;
        green = 155;
        blue = 0;
    } else {
        red = green = blue = 255;
    }

    Gfx_SetupDL25_Xlu(gfxCtx);

    if ((thisx->params == WOOD_LEAF_GREEN) || (thisx->params == WOOD_LEAF_YELLOW)) {
        Gfx_SetupDL25_Opa(gfxCtx);

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, red, green, blue, 127);

        Gfx_DrawDListOpa(play, object_wood02_DL_000700);
    } else if (D_808C4D70[this->drawType & 0xF] != NULL) {
        Gfx_DrawDListOpa(play, D_808C4D54[this->drawType & 0xF]);

        gDPSetEnvColor(POLY_XLU_DISP++, red, green, blue, 0);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, D_808C4D70[this->drawType & 0xF]);
    } else {
        Gfx_SetupDL25_Xlu(gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, D_808C4D54[this->drawType & 0xF]);
    }

    CLOSE_DISPS(gfxCtx);
}
