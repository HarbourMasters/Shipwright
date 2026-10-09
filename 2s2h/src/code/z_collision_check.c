#include "z64collision_check.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#include "stdbool.h"

#include "macros.h"
#include "sfx.h"
#include "sys_matrix.h"

#include "z64actor.h"
#include "z64effect.h"
#include "z64frameadvance.h"
#include "z64malloc.h"
#include "z64math.h"

typedef s32 (*ColChkResetFunc)(struct PlayState*, Collider*);
typedef void (*ColChkBloodFunc)(struct PlayState*, Collider*, Vec3f*);
typedef void (*ColChkApplyFunc)(struct PlayState*, CollisionCheckContext*, Collider*);
typedef void (*ColChkVsFunc)(struct PlayState*, CollisionCheckContext*, Collider*, Collider*);
typedef s32 (*ColChkLineFunc)(struct PlayState*, CollisionCheckContext*, Collider*, Vec3f*, Vec3f*);

Vec3f D_801EDE00;
Vec3f D_801EDE10;
Vec3f D_801EDE20;
Vec3f D_801EDE30;
TriNorm D_801EDE40;
TriNorm D_801EDE78;
Linef D_801EDEB0;
TriNorm D_801EDEC8;
TriNorm D_801EDF00;
Vec3f D_801EDF38;
Vec3f D_801EDF48;
TriNorm D_801EDF58;
TriNorm D_801EDF90;
Linef D_801EDFC8;
Vec3f D_801EDFE0;
Vec3f D_801EDFF0;
TriNorm D_801EE000;
TriNorm D_801EE038;
TriNorm D_801EE070[2];
Vec3f D_801EE0D8;
TriNorm D_801EE0E8[2];
TriNorm D_801EE150;
TriNorm D_801EE188;

#include "prevent_bss_reordering.h"

/**
 * Gets the damage and effect that should be applied for the collision between
 * `at` and `ac`, referring to the ac actor's damage chart if applicable.
 */
f32 CollisionCheck_GetDamageAndEffectOnElementAC(Collider* atCol, ColliderElement* atElem, Collider* acCol,
                                                 ColliderElement* acElem, u32* effect) {
    static f32 sDamageMultipliers[] = {
        0.0f, 1.0f, 2.0f, 0.5f, 0.25f, 3.0f, 4.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    };
    u32 dmgFlags;
    s32 i;
    f32 damage;

    *effect = 0;
    damage = CollisionCheck_GetElementATDamage(atCol, atElem, acCol, acElem);

    if (acCol->actor->colChkInfo.damageTable != NULL) {
        dmgFlags = atElem->atDmgInfo.dmgFlags;

        for (i = 0; i < ARRAY_COUNT(acCol->actor->colChkInfo.damageTable->attack); i++) {
            if (dmgFlags == 1) {
                break;
            }
            dmgFlags >>= 1;
        }

        // #region 2S2H - Enhancements - Damage Multiplier and Effect
        if ((GameInteractor_Should(VB_DAMAGE_MULTIPLIER, true, i, acCol->actor->colChkInfo.damageTable, &damage,
                                   sDamageMultipliers))) {
            damage *= sDamageMultipliers[acCol->actor->colChkInfo.damageTable->attack[i] & 0xF];
        }

        if ((GameInteractor_Should(VB_DAMAGE_EFFECT, true, i, acCol->actor->colChkInfo.damageTable, effect,
                                   acCol->actor))) {
            *effect = (acCol->actor->colChkInfo.damageTable->attack[i] >> 4) & 0xF;
        }
        // #endregion
    }
    return damage;
}

/**
 * Calculates damage after factoring in the ac collider's defense
 */
f32 CollisionCheck_ApplyElementATDefense(f32 damage, ColliderElement* acElem) {
    f32 finalDamage = damage - acElem->acDmgInfo.defense;

    return finalDamage;
}

/**
 * Gets the damage to be inflicted by `at` on `ac`, before applying other
 * factors such as the ac collider's defense.
 */
s32 CollisionCheck_GetElementATDamage(Collider* atCol, ColliderElement* atElem, Collider* acCol,
                                      ColliderElement* acElem) {
    if ((atCol->actor != NULL) && (atCol->actor->id == ACTOR_EN_BOM) && (acCol->actor != NULL) &&
        (acCol->actor->id == ACTOR_PLAYER)) {
        return 8;
    }
    return atElem->atDmgInfo.damage;
}

s32 Collider_InitBase(struct PlayState* play, Collider* col) {
    static Collider sDefaultCollider = {
        NULL, NULL, NULL, NULL, AT_NONE, AC_NONE, OC1_NONE, OC2_NONE, COL_MATERIAL_HIT3, COLSHAPE_MAX,
    };

    *col = sDefaultCollider;
    return 1;
}

s32 Collider_DestroyBase(struct PlayState* play, Collider* col) {
    return 1;
}

/**
 * Uses default OC2_TYPE_1 and COL_MATERIAL_HIT0
 */
s32 Collider_SetBaseToActor(struct PlayState* play, Collider* col, ColliderInitToActor* src) {
    col->actor = src->actor;
    col->atFlags = src->atFlags;
    col->acFlags = src->acFlags;
    col->ocFlags1 = src->ocFlags1;
    col->ocFlags2 = OC2_TYPE_1;
    col->shape = src->shape;
    return 1;
}

/**
 * Uses default OC2_TYPE_1
 */
s32 Collider_SetBaseType1(struct PlayState* play, Collider* col, Actor* actor, ColliderInitType1* src) {
    col->actor = actor;
    col->colMaterial = src->colMaterial;
    col->atFlags = src->atFlags;
    col->acFlags = src->acFlags;
    col->ocFlags1 = src->ocFlags1;
    col->ocFlags2 = OC2_TYPE_1;
    col->shape = src->shape;
    return 1;
}

s32 Collider_SetBase(struct PlayState* play, Collider* col, Actor* actor, ColliderInit* src) {
    col->actor = actor;
    col->colMaterial = src->colMaterial;
    col->atFlags = src->atFlags;
    col->acFlags = src->acFlags;
    col->ocFlags1 = src->ocFlags1;
    col->ocFlags2 = src->ocFlags2;
    col->shape = src->shape;
    return 1;
}

void Collider_ResetATBase(struct PlayState* play, Collider* col) {
    col->at = NULL;
    col->atFlags &= ~(AT_HIT | AT_BOUNCED);
}

void Collider_ResetACBase(struct PlayState* play, Collider* col) {
    col->ac = NULL;
    col->acFlags &= ~(AC_HIT | AC_BOUNCED);
}

void Collider_ResetOCBase(struct PlayState* play, Collider* col) {
    col->oc = NULL;
    col->ocFlags1 &= ~OC1_HIT;
    col->ocFlags2 &= ~OC2_HIT_PLAYER;
}

s32 Collider_InitElementDamageInfoAT(struct PlayState* play, ColliderElementDamageInfoAT* atDmgInfo) {
    static ColliderElementDamageInfoAT sDefaultDamageInfoAT = { 0x00000000, 0, 0 };

    *atDmgInfo = sDefaultDamageInfoAT;
    return 1;
}

s32 Collider_DestroyElementDamageInfoAT(struct PlayState* play, ColliderElementDamageInfoAT* atDmgInfo) {
    return 1;
}

s32 Collider_SetElementDamageInfoAT(struct PlayState* play, ColliderElementDamageInfoAT* dest,
                                    ColliderElementDamageInfoATInit* src) {
    dest->dmgFlags = src->dmgFlags;
    dest->effect = src->effect;
    dest->damage = src->damage;
    return 1;
}

void Collider_ResetATElementUnk(struct PlayState* play, ColliderElement* elem) {
}

s32 Collider_InitElementDamageInfoAC(struct PlayState* play, ColliderElementDamageInfoAC* acDmgInfo) {
    static ColliderElementDamageInfoAC sDefaultDamageInfoAC = { 0xF7CFFFFF, 0, 0, { 0, 0, 0 } };

    *acDmgInfo = sDefaultDamageInfoAC;
    return 1;
}

s32 Collider_DestroyElementDamageInfoAC(struct PlayState* play, ColliderElementDamageInfoAC* acDmgInfo) {
    return 1;
}

s32 Collider_SetElementDamageInfoAC(struct PlayState* play, ColliderElementDamageInfoAC* dest,
                                    ColliderElementDamageInfoACInit* src) {
    dest->dmgFlags = src->dmgFlags;
    dest->effect = src->effect;
    dest->defense = src->defense;
    return 1;
}

s32 Collider_InitElement(struct PlayState* play, ColliderElement* elem) {
    static ColliderElement sDefaultColliderElement = {
        { 0, 0, 0 },
        { 0xF7CFFFFF, 0, 0, { 0, 0, 0 } },
        ELEM_MATERIAL_UNK0,
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_NONE,
        NULL,
        NULL,
        NULL,
        NULL,
    };

    *elem = sDefaultColliderElement;
    Collider_InitElementDamageInfoAT(play, &elem->atDmgInfo);
    Collider_InitElementDamageInfoAC(play, &elem->acDmgInfo);
    return 1;
}

s32 Collider_DestroyElement(struct PlayState* play, ColliderElement* elem) {
    Collider_DestroyElementDamageInfoAT(play, &elem->atDmgInfo);
    Collider_DestroyElementDamageInfoAC(play, &elem->acDmgInfo);
    return 1;
}

s32 Collider_SetElement(struct PlayState* play, ColliderElement* elem, ColliderElementInit* elemInit) {
    elem->elemMaterial = elemInit->elemMaterial;
    Collider_SetElementDamageInfoAT(play, &elem->atDmgInfo, &elemInit->atDmgInfo);
    Collider_SetElementDamageInfoAC(play, &elem->acDmgInfo, &elemInit->acDmgInfo);
    elem->atElemFlags = elemInit->atElemFlags;
    elem->acElemFlags = elemInit->acElemFlags;
    elem->ocElemFlags = elemInit->ocElemFlags;
    return 1;
}

void Collider_ResetATElement(struct PlayState* play, ColliderElement* elem) {
    elem->atHit = NULL;
    elem->atHitElem = NULL;
    elem->atElemFlags &= ~ATELEM_HIT;
    elem->atElemFlags &= ~ATELEM_DREW_HITMARK;
    Collider_ResetATElementUnk(play, elem);
}

void Collider_ResetACElement(struct PlayState* play, ColliderElement* elem) {
    elem->acDmgInfo.hitPos.x = elem->acDmgInfo.hitPos.y = elem->acDmgInfo.hitPos.z = 0;
    elem->acElemFlags &= ~ACELEM_HIT;
    elem->acElemFlags &= ~ACELEM_DRAW_HITMARK;
    elem->acHit = NULL;
    elem->acHitElem = NULL;
}

void Collider_ResetOCElement(struct PlayState* play, ColliderElement* elem) {
    elem->ocElemFlags &= ~OCELEM_HIT;
}

s32 Collider_InitJntSphElementDim(struct PlayState* play, ColliderJntSphElementDim* dim) {
    static ColliderJntSphElementDim sDefaultColliderJntSphElementDim = {
        { { 0, 0, 0 }, 0 },
        { { 0, 0, 0 }, 0 },
        0.0f,
        0,
    };

    *dim = sDefaultColliderJntSphElementDim;
    return 1;
}

s32 Collider_DestroyJntSphElementDim(struct PlayState* play, ColliderJntSphElementDim* dim) {
    return 1;
}

s32 Collider_SetJntSphElementDim(struct PlayState* play, ColliderJntSphElementDim* dest,
                                 ColliderJntSphElementDimInit* src) {
    dest->limb = src->limb;
    dest->modelSphere = src->modelSphere;
    dest->scale = src->scale * 0.01f;
    return 1;
}

s32 Collider_InitJntSphElement(struct PlayState* play, ColliderJntSphElement* jntSphElem) {
    Collider_InitElement(play, &jntSphElem->base);
    Collider_InitJntSphElementDim(play, &jntSphElem->dim);
    return 1;
}

s32 Collider_DestroyJntSphElement(struct PlayState* play, ColliderJntSphElement* jntSphElem) {
    Collider_DestroyElement(play, &jntSphElem->base);
    Collider_DestroyJntSphElementDim(play, &jntSphElem->dim);
    return 1;
}

s32 Collider_SetJntSphElement(struct PlayState* play, ColliderJntSphElement* dest, ColliderJntSphElementInit* src) {
    Collider_SetElement(play, &dest->base, &src->base);
    Collider_SetJntSphElementDim(play, &dest->dim, &src->dim);
    return 1;
}

s32 Collider_ResetJntSphElementAT(struct PlayState* play, ColliderJntSphElement* jntSphElem) {
    Collider_ResetATElement(play, &jntSphElem->base);
    return 1;
}

s32 Collider_ResetJntSphElementAC(struct PlayState* play, ColliderJntSphElement* jntSphElem) {
    Collider_ResetACElement(play, &jntSphElem->base);
    return 1;
}

s32 Collider_ResetJntSphElementOC(struct PlayState* play, ColliderJntSphElement* jntSphElem) {
    Collider_ResetOCElement(play, &jntSphElem->base);
    return 1;
}

/**
 * Initializes a ColliderJntSph to default values
 */
s32 Collider_InitJntSph(struct PlayState* play, ColliderJntSph* jntSph) {
    Collider_InitBase(play, &jntSph->base);
    jntSph->count = 0;
    jntSph->elements = NULL;
    return 1;
}

/**
 * Destroys a dynamically allocated ColliderJntSph
 */
s32 Collider_FreeJntSph(struct PlayState* play, ColliderJntSph* jntSph) {
    ColliderJntSphElement* jntSphElem;

    Collider_DestroyBase(play, &jntSph->base);
    for (jntSphElem = jntSph->elements; jntSphElem < &jntSph->elements[jntSph->count]; jntSphElem++) {
        Collider_DestroyJntSphElement(play, jntSphElem);
    }

    jntSph->count = 0;
    if (jntSph->elements != NULL) {
        ZeldaArena_Free(jntSph->elements);
    }
    jntSph->elements = NULL;
    return 1;
}

/**
 * Destroys a preallocated ColliderJntSph
 */
s32 Collider_DestroyJntSph(struct PlayState* play, ColliderJntSph* jntSph) {
    ColliderJntSphElement* jntSphElem;

    Collider_DestroyBase(play, &jntSph->base);

    for (jntSphElem = jntSph->elements; jntSphElem < &jntSph->elements[jntSph->count]; jntSphElem++) {
        Collider_DestroyJntSphElement(play, jntSphElem);
    }
    jntSph->count = 0;
    jntSph->elements = NULL;
    return 1;
}

/**
 * Sets up the ColliderJntSph using the values in src, sets it to the actor specified in src, and dynamically allocates
 * the element array. Uses default OC2_TYPE_1 and COL_MATERIAL_HIT0.
 */
s32 Collider_SetJntSphToActor(struct PlayState* play, ColliderJntSph* dest, ColliderJntSphInitToActor* src) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    Collider_SetBaseToActor(play, &dest->base, &src->base);
    dest->count = src->count;
    dest->elements = ZeldaArena_Malloc(src->count * sizeof(ColliderJntSphElement));

    if (dest->elements == NULL) {
        dest->count = 0;
        return 0;
    }

    for (destElem = dest->elements, srcElem = src->elements; destElem < &dest->elements[dest->count];
         destElem++, srcElem++) {
        Collider_InitJntSphElement(play, destElem);
        Collider_SetJntSphElement(play, destElem, srcElem);
    }
    return 1;
}

/**
 * Sets up the ColliderJntSph using the values in src and dynamically allocates the element array. Uses default
 * OC2_TYPE_1.
 */
s32 Collider_SetJntSphAllocType1(struct PlayState* play, ColliderJntSph* dest, Actor* actor,
                                 ColliderJntSphInitType1* src) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    Collider_SetBaseType1(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = ZeldaArena_Malloc(src->count * sizeof(ColliderJntSphElement));

    if (dest->elements == NULL) {
        dest->count = 0;
        return 0;
    }

    for (destElem = dest->elements, srcElem = src->elements; destElem < &dest->elements[dest->count];
         destElem++, srcElem++) {
        Collider_InitJntSphElement(play, destElem);
        Collider_SetJntSphElement(play, destElem, srcElem);
    }
    return 1;
}

/**
 * Sets up the ColliderJntSph using the values in src, placing the element array in elements.
 */
s32 Collider_SetJntSph(struct PlayState* play, ColliderJntSph* dest, Actor* actor, ColliderJntSphInit* src,
                       ColliderJntSphElement* jntSphElements) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    Collider_SetBase(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = jntSphElements;

    for (destElem = dest->elements, srcElem = src->elements; destElem < &dest->elements[dest->count];
         destElem++, srcElem++) {
        Collider_InitJntSphElement(play, destElem);
        Collider_SetJntSphElement(play, destElem, srcElem);
    }

    return 1;
}

/**
 * Fully initializes a ColliderJntSph using the values in `src`, placing the element array in elements.
 */
s32 Collider_InitAndSetJntSph(struct PlayState* play, ColliderJntSph* dest, Actor* actor, ColliderJntSphInit* src,
                              ColliderJntSphElement* jntSphElements) {
    Collider_InitJntSph(play, dest);
    Collider_SetJntSph(play, dest, actor, src, jntSphElements);
    return 0;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 Collider_ResetJntSphAT(struct PlayState* play, Collider* col) {
    ColliderJntSphElement* jntSphElem;
    ColliderJntSph* jntSph = (ColliderJntSph*)col;

    Collider_ResetATBase(play, &jntSph->base);

    for (jntSphElem = jntSph->elements; jntSphElem < &jntSph->elements[jntSph->count]; jntSphElem++) {
        Collider_ResetJntSphElementAT(play, jntSphElem);
    }
    return 1;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 Collider_ResetJntSphAC(struct PlayState* play, Collider* col) {
    ColliderJntSphElement* jntSphElem;
    ColliderJntSph* jntSph = (ColliderJntSph*)col;

    Collider_ResetACBase(play, &jntSph->base);

    for (jntSphElem = jntSph->elements; jntSphElem < &jntSph->elements[jntSph->count]; jntSphElem++) {
        Collider_ResetJntSphElementAC(play, jntSphElem);
    }
    return 1;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 Collider_ResetJntSphOC(struct PlayState* play, Collider* col) {
    ColliderJntSphElement* jntSphElem;
    ColliderJntSph* jntSph = (ColliderJntSph*)col;

    Collider_ResetOCBase(play, &jntSph->base);

    for (jntSphElem = jntSph->elements; jntSphElem < &jntSph->elements[jntSph->count]; jntSphElem++) {
        Collider_ResetJntSphElementOC(play, jntSphElem);
    }
    return 1;
}

s32 Collider_InitCylinderDim(struct PlayState* play, Cylinder16* dim) {
    static Cylinder16 sDefaultColliderCylinderDim = { 0, 0, 0, { 0, 0, 0 } };

    *dim = sDefaultColliderCylinderDim;
    return 1;
}

s32 Collider_DestroyCylinderDim(struct PlayState* play, Cylinder16* dim) {
    return 1;
}

s32 Collider_SetCylinderDim(struct PlayState* play, Cylinder16* dim, Cylinder16* src) {
    *dim = *src;
    return 1;
}

/**
 * Initializes a ColliderCylinder to default values
 */
s32 Collider_InitCylinder(struct PlayState* play, ColliderCylinder* cyl) {
    Collider_InitBase(play, &cyl->base);
    Collider_InitElement(play, &cyl->elem);
    Collider_InitCylinderDim(play, &cyl->dim);
    return 1;
}

/**
 * Destroys a ColliderCylinder
 */
s32 Collider_DestroyCylinder(struct PlayState* play, ColliderCylinder* cyl) {
    Collider_DestroyBase(play, &cyl->base);
    Collider_DestroyElement(play, &cyl->elem);
    Collider_DestroyCylinderDim(play, &cyl->dim);
    return 1;
}

/**
 * Sets up the ColliderCylinder using the values in src and sets it to the actor specified in src. Uses default
 * OC2_TYPE_1 and COL_MATERIAL_0.
 */
s32 Collider_SetCylinderToActor(struct PlayState* play, ColliderCylinder* dest, ColliderCylinderInitToActor* src) {
    Collider_SetBaseToActor(play, &dest->base, &src->base);
    Collider_SetElement(play, &dest->elem, &src->elem);
    Collider_SetCylinderDim(play, &dest->dim, &src->dim);
    return 1;
}

/**
 * Sets up the ColliderCylinder using the values in src. Uses default OC2_TYPE_1
 */
s32 Collider_SetCylinderType1(struct PlayState* play, ColliderCylinder* dest, Actor* actor,
                              ColliderCylinderInitType1* src) {
    Collider_SetBaseType1(play, &dest->base, actor, &src->base);
    Collider_SetElement(play, &dest->elem, &src->elem);
    Collider_SetCylinderDim(play, &dest->dim, &src->dim);
    return 1;
}

/**
 * Sets up the ColliderCylinder using the values in src.
 */
s32 Collider_SetCylinder(struct PlayState* play, ColliderCylinder* dest, Actor* actor, ColliderCylinderInit* src) {
    Collider_SetBase(play, &dest->base, actor, &src->base);
    Collider_SetElement(play, &dest->elem, &src->elem);
    Collider_SetCylinderDim(play, &dest->dim, &src->dim);
    return 1;
}

/**
 * Fully initializes a ColliderCylinder using the values in src.
 */
s32 Collider_InitAndSetCylinder(struct PlayState* play, ColliderCylinder* dest, Actor* actor,
                                ColliderCylinderInit* src) {
    Collider_InitCylinder(play, dest);
    Collider_SetCylinder(play, dest, actor, src);
    return 1;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 Collider_ResetCylinderAT(struct PlayState* play, Collider* col) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    Collider_ResetATBase(play, &cyl->base);
    Collider_ResetATElement(play, &cyl->elem);
    return 1;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 Collider_ResetCylinderAC(struct PlayState* play, Collider* col) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    Collider_ResetACBase(play, &cyl->base);
    Collider_ResetACElement(play, &cyl->elem);
    return 1;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 Collider_ResetCylinderOC(struct PlayState* play, Collider* col) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    Collider_ResetOCBase(play, &cyl->base);
    Collider_ResetOCElement(play, &cyl->elem);
    return 1;
}

s32 Collider_InitTrisElementDim(struct PlayState* play, TriNorm* dim) {
    static TriNorm sDefaultColliderTrisElementDim = {
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.0f, 0.0f, 0.0f }, 0.0f },
    };

    *dim = sDefaultColliderTrisElementDim;
    return 1;
}

s32 Collider_DestroyTrisElementDim(struct PlayState* play, TriNorm* dim) {
    return 1;
}

s32 Collider_SetTrisElementDim(struct PlayState* play, TriNorm* dest, ColliderTrisElementDimInit* src) {
    Vec3f* destVtx;
    Vec3f* srcVtx;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 originDist;

    for (destVtx = dest->vtx, srcVtx = &src->vtx[0]; destVtx < &dest->vtx[3]; destVtx++, srcVtx++) {
        *destVtx = *srcVtx;
    }

    Math3D_DefPlane(&src->vtx[0], &src->vtx[1], &src->vtx[2], &nx, &ny, &nz, &originDist);

    dest->plane.normal.x = nx;
    dest->plane.normal.y = ny;
    dest->plane.normal.z = nz;
    dest->plane.originDist = originDist;
    return 1;
}

s32 Collider_InitTrisElement(struct PlayState* play, ColliderTrisElement* trisElem) {
    Collider_InitElement(play, &trisElem->base);
    Collider_InitTrisElementDim(play, &trisElem->dim);
    return 1;
}

s32 Collider_DestroyTrisElement(struct PlayState* play, ColliderTrisElement* trisElem) {
    Collider_DestroyElement(play, &trisElem->base);
    Collider_DestroyTrisElementDim(play, &trisElem->dim);
    return 1;
}

s32 Collider_SetTrisElement(struct PlayState* play, ColliderTrisElement* dest, ColliderTrisElementInit* src) {
    Collider_SetElement(play, &dest->base, &src->base);
    Collider_SetTrisElementDim(play, &dest->dim, &src->dim);
    return 1;
}

s32 Collider_ResetTrisElementAT(struct PlayState* play, ColliderTrisElement* trisElem) {
    Collider_ResetATElement(play, &trisElem->base);
    return 1;
}

s32 Collider_ResetTrisElementAC(struct PlayState* play, ColliderTrisElement* trisElem) {
    Collider_ResetACElement(play, &trisElem->base);
    return 1;
}

s32 Collider_ResetTrisElementOC(struct PlayState* play, ColliderTrisElement* trisElem) {
    Collider_ResetOCElement(play, &trisElem->base);
    return 1;
}

/**
 * Initializes a ColliderTris to default values
 */
s32 Collider_InitTris(struct PlayState* play, ColliderTris* tris) {
    Collider_InitBase(play, &tris->base);
    tris->count = 0;
    tris->elements = NULL;
    return 1;
}

/**
 * Destroys a dynamically allocated ColliderTris
 */
s32 Collider_FreeTris(struct PlayState* play, ColliderTris* tris) {
    ColliderTrisElement* trisElem;

    Collider_DestroyBase(play, &tris->base);

    for (trisElem = tris->elements; trisElem < &tris->elements[tris->count]; trisElem++) {
        Collider_DestroyTrisElement(play, trisElem);
    }

    tris->count = 0;
    if (tris->elements != NULL) {
        ZeldaArena_Free(tris->elements);
    }
    tris->elements = NULL;

    return 1;
}

/**
 * Destroys a preallocated ColliderTris
 */
s32 Collider_DestroyTris(struct PlayState* play, ColliderTris* tris) {
    ColliderTrisElement* trisElem;

    Collider_DestroyBase(play, &tris->base);

    for (trisElem = tris->elements; trisElem < &tris->elements[tris->count]; trisElem++) {
        Collider_DestroyTrisElement(play, trisElem);
    }
    tris->count = 0;
    tris->elements = NULL;
    return 1;
}

/**
 * Sets up the ColliderTris using the values in src and dynamically allocates the element array. Uses default
 * OC2_TYPE_1.
 */
s32 Collider_SetTrisAllocType1(struct PlayState* play, ColliderTris* dest, Actor* actor, ColliderTrisInitType1* src) {
    ColliderTrisElement* destElem;
    ColliderTrisElementInit* srcElem;

    Collider_SetBaseType1(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = ZeldaArena_Malloc(dest->count * sizeof(ColliderTrisElement));

    if (dest->elements == NULL) {
        dest->count = 0;
        return 0;
    }

    for (destElem = dest->elements, srcElem = src->elements; destElem < &dest->elements[dest->count];
         destElem++, srcElem++) {
        Collider_InitTrisElement(play, destElem);
        Collider_SetTrisElement(play, destElem, srcElem);
    }

    return 1;
}

/**
 * Sets up the ColliderTris using the values in src, placing the element array in elements.
 */
s32 Collider_SetTris(struct PlayState* play, ColliderTris* dest, Actor* actor, ColliderTrisInit* src,
                     ColliderTrisElement* trisElements) {
    ColliderTrisElement* destElem;
    ColliderTrisElementInit* srcElem;

    Collider_SetBase(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = trisElements;

    for (destElem = dest->elements, srcElem = src->elements; destElem < &dest->elements[dest->count];
         destElem++, srcElem++) {
        Collider_InitTrisElement(play, destElem);
        Collider_SetTrisElement(play, destElem, srcElem);
    }

    return 1;
}

/**
 * Fully initializes a ColliderTris using the values in src, placing the element array in elements.
 */
s32 Collider_InitAndSetTris(struct PlayState* play, ColliderTris* dest, Actor* actor, ColliderTrisInit* src,
                            ColliderTrisElement* trisElements) {
    Collider_InitTris(play, dest);
    Collider_SetTris(play, dest, actor, src, trisElements);
    return 0;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 Collider_ResetTrisAT(struct PlayState* play, Collider* col) {
    ColliderTrisElement* trisElem;
    ColliderTris* tris = (ColliderTris*)col;

    Collider_ResetATBase(play, &tris->base);

    for (trisElem = tris->elements; trisElem < &tris->elements[tris->count]; trisElem++) {
        Collider_ResetTrisElementAT(play, trisElem);
    }
    return 1;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 Collider_ResetTrisAC(struct PlayState* play, Collider* col) {
    ColliderTrisElement* trisElem;
    ColliderTris* tris = (ColliderTris*)col;

    Collider_ResetACBase(play, &tris->base);

    for (trisElem = tris->elements; trisElem < &tris->elements[tris->count]; trisElem++) {
        Collider_ResetTrisElementAC(play, trisElem);
    }
    return 1;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 Collider_ResetTrisOC(struct PlayState* play, Collider* col) {
    ColliderTrisElement* trisElem;
    ColliderTris* tris = (ColliderTris*)col;

    Collider_ResetOCBase(play, &tris->base);

    for (trisElem = tris->elements; trisElem < &tris->elements[tris->count]; trisElem++) {
        Collider_ResetTrisElementOC(play, trisElem);
    }
    return 1;
}

s32 Collider_InitQuadDim(struct PlayState* play, ColliderQuadDim* dim) {
    static ColliderQuadDim sDefaultColliderQuadDim = {
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
        { 0, 0, 0 },
        { 0, 0, 0 },
        1.0E38f,
    };

    *dim = sDefaultColliderQuadDim;
    return 1;
}

s32 Collider_DestroyQuadDim(struct PlayState* play, ColliderQuadDim* dim) {
    return 1;
}

s32 Collider_ResetQuadACDist(struct PlayState* play, ColliderQuadDim* dim) {
    dim->acDist = 1.0E38f;
    return 1;
}

void Collider_SetQuadMidpoints(ColliderQuadDim* dim) {
    dim->dcMid.x = (dim->quad[3].x + dim->quad[2].x) * 0.5f;
    dim->dcMid.y = (dim->quad[3].y + dim->quad[2].y) * 0.5f;
    dim->dcMid.z = (dim->quad[3].z + dim->quad[2].z) * 0.5f;
    dim->baMid.x = (dim->quad[1].x + dim->quad[0].x) * 0.5f;
    dim->baMid.y = (dim->quad[1].y + dim->quad[0].y) * 0.5f;
    dim->baMid.z = (dim->quad[1].z + dim->quad[0].z) * 0.5f;
}

s32 Collider_SetQuadDim(struct PlayState* play, ColliderQuadDim* dest, ColliderQuadDimInit* src) {
    dest->quad[0] = src->quad[0];
    dest->quad[1] = src->quad[1];
    dest->quad[2] = src->quad[2];
    dest->quad[3] = src->quad[3];

    Collider_SetQuadMidpoints(dest);
    return 1;
}

/**
 * Initializes a ColliderQuad to default values.
 */
s32 Collider_InitQuad(struct PlayState* play, ColliderQuad* quad) {
    Collider_InitBase(play, &quad->base);
    Collider_InitElement(play, &quad->elem);
    Collider_InitQuadDim(play, &quad->dim);
    return 1;
}

/**
 * Destroys a ColliderQuad.
 */
s32 Collider_DestroyQuad(struct PlayState* play, ColliderQuad* quad) {
    Collider_DestroyBase(play, &quad->base);
    Collider_DestroyElement(play, &quad->elem);
    Collider_DestroyQuadDim(play, &quad->dim);
    return 1;
}

/**
 * Sets up the ColliderQuad using the values in src. Uses the default OC2_TYPE_1
 */
s32 Collider_SetQuadType1(struct PlayState* play, ColliderQuad* dest, Actor* actor, ColliderQuadInitType1* src) {
    Collider_SetBaseType1(play, &dest->base, actor, &src->base);
    Collider_SetElement(play, &dest->elem, &src->elem);
    Collider_SetQuadDim(play, &dest->dim, &src->dim);
    return 1;
}

/**
 * Sets up the ColliderQuad using the values in src.
 */
s32 Collider_SetQuad(struct PlayState* play, ColliderQuad* dest, Actor* actor, ColliderQuadInit* src) {
    Collider_SetBase(play, &dest->base, actor, &src->base);
    Collider_SetElement(play, &dest->elem, &src->elem);
    Collider_SetQuadDim(play, &dest->dim, &src->dim);
    return 1;
}

/**
 * Fully initializes a ColliderQuad using the values in src.
 */
s32 Collider_InitAndSetQuad(struct PlayState* play, ColliderQuad* dest, Actor* actor, ColliderQuadInit* src) {
    Collider_InitQuad(play, dest);
    Collider_SetQuad(play, dest, actor, src);
    return 0;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 Collider_ResetQuadAT(struct PlayState* play, Collider* col) {
    ColliderQuad* quad = (ColliderQuad*)col;

    Collider_ResetATBase(play, &quad->base);
    Collider_ResetATElement(play, &quad->elem);
    Collider_ResetQuadACDist(play, &quad->dim);
    return 1;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 Collider_ResetQuadAC(struct PlayState* play, Collider* col) {
    ColliderQuad* quad = (ColliderQuad*)col;

    Collider_ResetACBase(play, &quad->base);
    Collider_ResetACElement(play, &quad->elem);
    return 1;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 Collider_ResetQuadOC(struct PlayState* play, Collider* col) {
    ColliderQuad* quad = (ColliderQuad*)col;

    Collider_ResetOCBase(play, &quad->base);
    Collider_ResetOCElement(play, &quad->elem);
    return 1;
}

/**
 * For quad colliders with ATELEM_NEAREST, resets the previous AC collider it hit if the current element is closer,
 * otherwise returns false. Used on player AT colliders to prevent multiple collisions from registering.
 */
s32 Collider_QuadSetNearestAC(struct PlayState* play, ColliderQuad* quad, Vec3f* hitPos) {
    f32 acDist;
    Vec3f dcMid;

    if (!(quad->elem.atElemFlags & ATELEM_NEAREST)) {
        return 1;
    }
    Math_Vec3s_ToVec3f(&dcMid, &quad->dim.dcMid);
    acDist = Math3D_Vec3fDistSq(&dcMid, hitPos);

    if (acDist < quad->dim.acDist) {
        quad->dim.acDist = acDist;

        if (quad->elem.atHit != NULL) {
            Collider_ResetACBase(play, quad->elem.atHit);
        }
        if (quad->elem.atHitElem != NULL) {
            Collider_ResetACElement(play, quad->elem.atHitElem);
        }
        return 1;
    } else {
        return 0;
    }
}

/**
 * Initializes a ColliderSphere to default values.
 */
s32 Collider_InitSphere(struct PlayState* play, ColliderSphere* sph) {
    Collider_InitBase(play, &sph->base);
    Collider_InitElement(play, &sph->elem);
    Collider_InitJntSphElementDim(play, &sph->dim);
    return 1;
}

/**
 * Destroys a ColliderSphere.
 */
s32 Collider_DestroySphere(struct PlayState* play, ColliderSphere* sph) {
    Collider_DestroyBase(play, &sph->base);
    Collider_DestroyElement(play, &sph->elem);
    Collider_DestroyJntSphElementDim(play, &sph->dim);
    return 1;
}

/**
 * Sets up the ColliderSphere using the values in src.
 */
s32 Collider_SetSphere(struct PlayState* play, ColliderSphere* dest, Actor* actor, ColliderSphereInit* src) {
    Collider_SetBase(play, &dest->base, actor, &src->base);
    Collider_SetElement(play, &dest->elem, &src->elem);
    Collider_SetJntSphElementDim(play, &dest->dim, &src->dim);
    return 1;
}

/**
 * Fully initializes a ColliderSphere using the values in src.
 */
s32 Collider_InitAndSetSphere(struct PlayState* play, ColliderSphere* dest, Actor* actor, ColliderSphereInit* src) {
    Collider_InitSphere(play, dest);
    Collider_SetSphere(play, dest, actor, src);
    return 0;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 Collider_ResetSphereAT(struct PlayState* play, Collider* col) {
    ColliderSphere* sph = (ColliderSphere*)col;

    Collider_ResetATBase(play, &sph->base);
    Collider_ResetATElement(play, &sph->elem);
    return 1;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 Collider_ResetSphereAC(struct PlayState* play, Collider* col) {
    ColliderSphere* sph = (ColliderSphere*)col;

    Collider_ResetACBase(play, &sph->base);
    Collider_ResetACElement(play, &sph->elem);
    return 1;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 Collider_ResetSphereOC(struct PlayState* play, Collider* col) {
    ColliderSphere* sph = (ColliderSphere*)col;

    Collider_ResetOCBase(play, &sph->base);
    Collider_ResetOCElement(play, &sph->elem);
    return 1;
}

/**
 * Initializes an OcLine to default values
 */
s32 Collider_InitLine(struct PlayState* play, OcLine* line) {
    static Vec3f sDefaultLinePoint = { 0.0f, 0.0f, 0.0f };

    Math_Vec3f_Copy(&line->line.a, &sDefaultLinePoint);
    Math_Vec3f_Copy(&line->line.b, &sDefaultLinePoint);
    return 1;
}

/**
 * Destroys an OcLine
 */
s32 Collider_DestroyLine(struct PlayState* play, OcLine* line) {
    return 1;
}

/**
 * Sets up an OcLine with endpoints a and b.
 */
s32 Collider_SetLinePoints(struct PlayState* play, OcLine* line, Vec3f* a, Vec3f* b) {
    Math_Vec3f_Copy(&line->line.a, a);
    Math_Vec3f_Copy(&line->line.b, b);
    return 1;
}

/**
 * Sets up an OcLine using the values in src.
 */
s32 Collider_SetLine(struct PlayState* play, OcLine* dst, OcLine* src) {
    dst->ocFlags = src->ocFlags;
    Collider_SetLinePoints(play, dst, &src->line.a, &src->line.b);
    return 1;
}

/**
 * Resets the OcLine's collision flags.
 */
s32 Collider_ResetLineOC(struct PlayState* play, OcLine* line) {
    line->ocFlags &= ~OCLINE_HIT;
    return 1;
}

/**
 * Initializes CollisionCheckContext.
 * Clears all collider arrays, disables SAC, and sets flags for drawing colliders.
 */
void CollisionCheck_InitContext(struct PlayState* play, CollisionCheckContext* colChkCtx) {
    colChkCtx->sacFlags = 0;
    CollisionCheck_ClearContext(play, colChkCtx);
}

void CollisionCheck_DestroyContext(struct PlayState* play, CollisionCheckContext* colChkCtx) {
}

/**
 * Clears all collider lists in CollisionCheckContext when not in SAC mode.
 */
void CollisionCheck_ClearContext(struct PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** colP;
    OcLine** lineP;

    if (colChkCtx->sacFlags & SAC_ON) {
        return;
    }

    colChkCtx->colATCount = 0;
    colChkCtx->colACCount = 0;
    colChkCtx->colOCCount = 0;
    colChkCtx->colLineCount = 0;

    for (colP = &colChkCtx->colAT[0]; colP < &colChkCtx->colAT[ARRAY_COUNT(colChkCtx->colAT)]; colP++) {
        *colP = NULL;
    }

    for (colP = &colChkCtx->colAC[0]; colP < &colChkCtx->colAC[ARRAY_COUNT(colChkCtx->colAC)]; colP++) {
        *colP = NULL;
    }

    for (colP = &colChkCtx->colOC[0]; colP < &colChkCtx->colOC[ARRAY_COUNT(colChkCtx->colOC)]; colP++) {
        *colP = NULL;
    }

    for (lineP = &colChkCtx->colLine[0]; lineP < &colChkCtx->colLine[ARRAY_COUNT(colChkCtx->colLine)]; lineP++) {
        *lineP = NULL;
    }
}

/**
 * Enables SAC, an alternate collision check mode that allows direct management of collider lists.
 */
void CollisionCheck_EnableSAC(struct PlayState* play, CollisionCheckContext* colChkCtx) {
    colChkCtx->sacFlags |= SAC_ON;
}

/**
 * Disables SAC, an alternate collision check mode that allows direct management of collider lists.
 */
void CollisionCheck_DisableSAC(struct PlayState* play, CollisionCheckContext* colChkCtx) {
    colChkCtx->sacFlags &= ~SAC_ON;
}

ColChkResetFunc sATResetFuncs[COLSHAPE_MAX] = {
    Collider_ResetJntSphAT,   // COLSHAPE_JNTSPH
    Collider_ResetCylinderAT, // COLSHAPE_CYLINDER
    Collider_ResetTrisAT,     // COLSHAPE_TRIS
    Collider_ResetQuadAT,     // COLSHAPE_QUAD
    Collider_ResetSphereAT,   // COLSHAPE_SPHERE
};

/**
 * Sets collider as an AT (attack) for the current frame, which will be checked against ACs (attack colliders)
 */
s32 CollisionCheck_SetAT(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    s32 index;

    if (FrameAdvance_IsEnabled(play)) {
        return -1;
    }
    sATResetFuncs[col->shape](play, col);

    if ((col->actor != NULL) && (col->actor->update == NULL)) {
        return -1;
    }
    if (colChkCtx->colATCount >= ARRAY_COUNT(colChkCtx->colAT)) {
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ON) {
        return -1;
    }
    index = colChkCtx->colATCount;
    colChkCtx->colAT[colChkCtx->colATCount++] = col;

    return index;
}

/**
 * Sets collider as an AT (attack) for the current frame, which will be checked against ACs (attack colliders).
 * If CollisionCheck_SAC is enabled, the collider will be inserted into the list at the specified index, otherwise it
 * will be inserted into the next slot.
 */
s32 CollisionCheck_SetAT_SAC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, s32 index) {
    if (FrameAdvance_IsEnabled(play)) {
        return -1;
    }
    sATResetFuncs[col->shape](play, col);

    if ((col->actor != NULL) && (col->actor->update == NULL)) {
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ON) {
        if (index >= colChkCtx->colATCount) {
            return -1;
        }
        colChkCtx->colAT[index] = col;
    } else {
        if (colChkCtx->colATCount >= ARRAY_COUNT(colChkCtx->colAT)) {
            return -1;
        }
        index = colChkCtx->colATCount;
        colChkCtx->colAT[colChkCtx->colATCount++] = col;
    }
    return index;
}

ColChkResetFunc sACResetFuncs[COLSHAPE_MAX] = {
    Collider_ResetJntSphAC,   // COLSHAPE_JNTSPH
    Collider_ResetCylinderAC, // COLSHAPE_CYLINDER
    Collider_ResetTrisAC,     // COLSHAPE_TRIS
    Collider_ResetQuadAC,     // COLSHAPE_QUAD
    Collider_ResetSphereAC,   // COLSHAPE_SPHERE
};

/**
 * Sets collider as an AC (attack collider) for the current frame, allowing it to detect ATs (attacks)
 */
s32 CollisionCheck_SetAC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    s32 index;

    if (FrameAdvance_IsEnabled(play)) {
        return -1;
    }
    sACResetFuncs[col->shape](play, col);

    if ((col->actor != NULL) && (col->actor->update == NULL)) {
        return -1;
    }
    if (colChkCtx->colACCount >= ARRAY_COUNT(colChkCtx->colAC)) {
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ON) {
        return -1;
    }
    index = colChkCtx->colACCount;
    colChkCtx->colAC[colChkCtx->colACCount++] = col;

    return index;
}

/**
 * Sets collider as an AC (attack collider) for the current frame, allowing it to detect ATs (attacks).
 * If CollisionCheck_SAC is enabled, the collider will be inserted into the list at the specified index, otherwise it
 * will be inserted into the next slot
 */
s32 CollisionCheck_SetAC_SAC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, s32 index) {
    if (FrameAdvance_IsEnabled(play)) {
        return -1;
    }
    sACResetFuncs[col->shape](play, col);

    if ((col->actor != NULL) && (col->actor->update == NULL)) {
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ON) {
        if (index >= colChkCtx->colACCount) {
            return -1;
        }
        colChkCtx->colAC[index] = col;
    } else {
        if (colChkCtx->colACCount >= ARRAY_COUNT(colChkCtx->colAC)) {
            return -1;
        }
        index = colChkCtx->colACCount;
        colChkCtx->colAC[colChkCtx->colACCount++] = col;
    }
    return index;
}

static ColChkResetFunc sOCResetFuncs[COLSHAPE_MAX] = {
    Collider_ResetJntSphOC,   // COLSHAPE_JNTSPH
    Collider_ResetCylinderOC, // COLSHAPE_CYLINDER
    Collider_ResetTrisOC,     // COLSHAPE_TRIS
    Collider_ResetQuadOC,     // COLSHAPE_QUAD
    Collider_ResetSphereOC,   // COLSHAPE_SPHERE
};

struct TriNorm2 {
    TriNorm norm1;
    u8 pad[4];
    TriNorm norm2;
};

struct TriNorm2 D_801EE6C8;

/**
 * Sets collider as an OC (object collider) for the current frame, allowing it to detect other OCs.
 */
s32 CollisionCheck_SetOC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    s32 index;

    if (FrameAdvance_IsEnabled(play)) {
        return -1;
    }
    sOCResetFuncs[col->shape](play, col);

    if ((col->actor != NULL) && (col->actor->update == NULL)) {
        return -1;
    }
    if (colChkCtx->colOCCount >= ARRAY_COUNT(colChkCtx->colOC)) {
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ON) {
        return -1;
    }
    index = colChkCtx->colOCCount;
    colChkCtx->colOC[colChkCtx->colOCCount++] = col;

    return index;
}

/**
 * Sets collider as an OC (object collider) for the current frame, allowing it to detect other OCs.
 * If CollisionCheck_SAC is enabled, the collider will be inserted into the list at the specified index, otherwise it
 * will be inserted into the next slot.
 */
s32 CollisionCheck_SetOC_SAC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, s32 index) {
    if (FrameAdvance_IsEnabled(play)) {
        return -1;
    }
    sOCResetFuncs[col->shape](play, col);

    if ((col->actor != NULL) && (col->actor->update == NULL)) {
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ON) {
        if (index >= colChkCtx->colOCCount) {
            return -1;
        }
        //! @bug should be colOC
        colChkCtx->colAT[index] = col;
    } else {
        if (colChkCtx->colOCCount >= ARRAY_COUNT(colChkCtx->colOC)) {
            return -1;
        }
        index = colChkCtx->colOCCount;

        colChkCtx->colOC[colChkCtx->colOCCount] = col;
        colChkCtx->colOCCount++;
    }
    return index;
}

/**
 * Sets a line as an OC collider for this frame.
 */
s32 CollisionCheck_SetOCLine(struct PlayState* play, CollisionCheckContext* colChkCtx, OcLine* line) {
    s32 index;

    if (FrameAdvance_IsEnabled(play)) {
        return -1;
    }

    Collider_ResetLineOC(play, line);

    if (colChkCtx->colLineCount >= ARRAY_COUNT(colChkCtx->colLine)) {
        return -1;
    }
    index = colChkCtx->colLineCount;
    colChkCtx->colLine[colChkCtx->colLineCount++] = line;

    return index;
}

/**
 * Skips AT elements that are off.
 */
s32 CollisionCheck_IsElementNotAT(ColliderElement* elem) {
    if (!(elem->atElemFlags & ATELEM_ON)) {
        return true;
    }
    return false;
}

/**
 * Skips AC elements that are off.
 */
s32 CollisionCheck_IsElementNotAC(ColliderElement* elem) {
    if (!(elem->acElemFlags & ACELEM_ON)) {
        return true;
    }
    return false;
}

/**
 * If the AT element has no dmgFlags in common with the AC element, no collision happens.
 */
s32 CollisionCheck_NoSharedFlags(ColliderElement* atElem, ColliderElement* acElem) {
    if (GameInteractor_Should(VB_CHECK_BUMPER_COLLISION, !(atElem->atDmgInfo.dmgFlags & acElem->acDmgInfo.dmgFlags),
                              atElem, acElem)) {
        return true;
    }
    return false;
}

/**
 * Spawns no blood drops.
 * Used by collider types HIT1, HIT3, HIT5, METAL, NONE, WOOD, HARD, and TREE
 */
void CollisionCheck_NoBlood(struct PlayState* play, Collider* col, Vec3f* v) {
}

/**
 * Spawns blue blood drops.
 * Used by collider types HIT0 and HIT8.
 */
void CollisionCheck_BlueBlood(struct PlayState* play, Collider* col, Vec3f* v) {
    static EffectSparkInit D_801EEC00;
    s32 effectIndex;

    D_801EEC00.position.x = v->x;
    D_801EEC00.position.y = v->y;
    D_801EEC00.position.z = v->z;
    D_801EEC00.uDiv = 5;
    D_801EEC00.vDiv = 5;
    D_801EEC00.colorStart[0].r = 10;
    D_801EEC00.colorStart[0].g = 10;
    D_801EEC00.colorStart[0].b = 200;
    D_801EEC00.colorStart[0].a = 255;
    D_801EEC00.colorStart[1].r = 0;
    D_801EEC00.colorStart[1].g = 0;
    D_801EEC00.colorStart[1].b = 128;
    D_801EEC00.colorStart[1].a = 255;
    D_801EEC00.colorStart[2].r = 0;
    D_801EEC00.colorStart[2].g = 0;
    D_801EEC00.colorStart[2].b = 128;
    D_801EEC00.colorStart[2].a = 255;
    D_801EEC00.colorStart[3].r = 0;
    D_801EEC00.colorStart[3].g = 0;
    D_801EEC00.colorStart[3].b = 128;
    D_801EEC00.colorStart[3].a = 255;
    D_801EEC00.colorEnd[0].r = 0;
    D_801EEC00.colorEnd[0].g = 0;
    D_801EEC00.colorEnd[0].b = 32;
    D_801EEC00.colorEnd[0].a = 0;
    D_801EEC00.colorEnd[1].r = 0;
    D_801EEC00.colorEnd[1].g = 0;
    D_801EEC00.colorEnd[1].b = 32;
    D_801EEC00.colorEnd[1].a = 0;
    D_801EEC00.colorEnd[2].r = 0;
    D_801EEC00.colorEnd[2].g = 0;
    D_801EEC00.colorEnd[2].b = 64;
    D_801EEC00.colorEnd[2].a = 0;
    D_801EEC00.colorEnd[3].r = 0;
    D_801EEC00.colorEnd[3].g = 0;
    D_801EEC00.colorEnd[3].b = 64;
    D_801EEC00.colorEnd[3].a = 0;
    D_801EEC00.timer = 0;
    D_801EEC00.duration = 16;
    D_801EEC00.speed = 8.0f;
    D_801EEC00.gravity = -1.0f;

    Effect_Add(play, &effectIndex, EFFECT_SPARK, 0, 1, &D_801EEC00);
}

/**
 * Spawns green blood drops.
 * Used by collider types HIT2 and HIT6. No actor has type HIT2.
 */
void CollisionCheck_GreenBlood(struct PlayState* play, Collider* col, Vec3f* v) {
    static EffectSparkInit D_801EF0C8;
    s32 effectIndex;

    D_801EF0C8.position.x = v->x;
    D_801EF0C8.position.y = v->y;
    D_801EF0C8.position.z = v->z;
    D_801EF0C8.uDiv = 5;
    D_801EF0C8.vDiv = 5;
    D_801EF0C8.colorStart[0].r = 10;
    D_801EF0C8.colorStart[0].g = 200;
    D_801EF0C8.colorStart[0].b = 10;
    D_801EF0C8.colorStart[0].a = 255;
    D_801EF0C8.colorStart[1].r = 0;
    D_801EF0C8.colorStart[1].g = 128;
    D_801EF0C8.colorStart[1].b = 0;
    D_801EF0C8.colorStart[1].a = 255;
    D_801EF0C8.colorStart[2].r = 0;
    D_801EF0C8.colorStart[2].g = 128;
    D_801EF0C8.colorStart[2].b = 0;
    D_801EF0C8.colorStart[2].a = 255;
    D_801EF0C8.colorStart[3].r = 0;
    D_801EF0C8.colorStart[3].g = 128;
    D_801EF0C8.colorStart[3].b = 0;
    D_801EF0C8.colorStart[3].a = 255;
    D_801EF0C8.colorEnd[0].r = 0;
    D_801EF0C8.colorEnd[0].g = 32;
    D_801EF0C8.colorEnd[0].b = 0;
    D_801EF0C8.colorEnd[0].a = 0;
    D_801EF0C8.colorEnd[1].r = 0;
    D_801EF0C8.colorEnd[1].g = 32;
    D_801EF0C8.colorEnd[1].b = 0;
    D_801EF0C8.colorEnd[1].a = 0;
    D_801EF0C8.colorEnd[2].r = 0;
    D_801EF0C8.colorEnd[2].g = 64;
    D_801EF0C8.colorEnd[2].b = 0;
    D_801EF0C8.colorEnd[2].a = 0;
    D_801EF0C8.colorEnd[3].r = 0;
    D_801EF0C8.colorEnd[3].g = 64;
    D_801EF0C8.colorEnd[3].b = 0;
    D_801EF0C8.colorEnd[3].a = 0;
    D_801EF0C8.timer = 0;
    D_801EF0C8.duration = 16;
    D_801EF0C8.speed = 8.0f;
    D_801EF0C8.gravity = -1.0f;
    Effect_Add(play, &effectIndex, EFFECT_SPARK, 0, 1, &D_801EF0C8);
}

TriNorm D_801EF590;
TriNorm D_801EF5C8;
u8 pad4[4];

/**
 * Spawns a burst of water.
 * Used by collider type HIT4.
 */
void CollisionCheck_WaterBurst(struct PlayState* play, Collider* col, Vec3f* v) {
    EffectSsSibuki_SpawnBurst(play, v);
    CollisionCheck_SpawnWaterDroplets(play, v);
}

/**
 * Spawns red blood drops.
 * Used by collider type HIT7.
 */
void CollisionCheck_RedBlood(struct PlayState* play, Collider* col, Vec3f* v) {
    CollisionCheck_SpawnRedBlood(play, v);
}

/**
 * Spawns red blood drops.
 */
void CollisionCheck_RedBloodUnused(struct PlayState* play, Collider* col, Vec3f* v) {
    CollisionCheck_SpawnRedBlood(play, v);
}

/**
 * Plays sound effects and displays hitmarks for solid-type AC colliders (METAL, WOOD, HARD, and TREE)
 */
void CollisionCheck_HitSolid(struct PlayState* play, ColliderElement* elem, Collider* col, Vec3f* hitPos) {
    s32 flags = elem->atElemFlags & ATELEM_SFX_NONE;

    if ((flags == ATELEM_SFX_NORMAL) && (col->colMaterial != COL_MATERIAL_METAL)) {
        EffectSsHitmark_SpawnFixedScale(play, 0, hitPos);
        if (col->actor == NULL) {
            Audio_PlaySfx(NA_SE_IT_SHIELD_BOUND);
        } else {
            Audio_PlaySfx_AtPos(&col->actor->projectedPos, NA_SE_IT_SHIELD_BOUND);
        }
    } else if (flags == ATELEM_SFX_NORMAL) {
        EffectSsHitmark_SpawnFixedScale(play, 3, hitPos);
        if (col->actor == NULL) {
            CollisionCheck_SpawnShieldParticlesMetal(play, hitPos);
        } else {
            CollisionCheck_SpawnShieldParticlesMetalSound(play, hitPos, &col->actor->projectedPos);
        }
    } else if (flags == ATELEM_SFX_HARD) {
        EffectSsHitmark_SpawnFixedScale(play, 0, hitPos);
        if (col->actor == NULL) {
            Audio_PlaySfx(NA_SE_IT_SHIELD_BOUND);
        } else {
            Audio_PlaySfx_AtPos(&col->actor->projectedPos, NA_SE_IT_SHIELD_BOUND);
        }
    } else if (flags == ATELEM_SFX_WOOD) {
        EffectSsHitmark_SpawnFixedScale(play, 1, hitPos);
        if (col->actor == NULL) {
            Audio_PlaySfx(NA_SE_IT_REFLECTION_WOOD);
        } else {
            Audio_PlaySfx_AtPos(&col->actor->projectedPos, NA_SE_IT_REFLECTION_WOOD);
        }
    }
}

/**
 * Plays a hit sound effect for AT colliders attached to Player based on the AC element's elemMaterial.
 */
s32 CollisionCheck_SwordHitAudio(Collider* at, ColliderElement* acElem) {
    if ((at->actor != NULL) && (at->actor->category == ACTORCAT_PLAYER)) {
        if (acElem->elemMaterial == ELEM_MATERIAL_UNK0) {
            Audio_PlaySfx_AtPos(&at->actor->projectedPos, NA_SE_IT_SWORD_STRIKE);
        } else if (acElem->elemMaterial == ELEM_MATERIAL_UNK1) {
            Audio_PlaySfx_AtPos(&at->actor->projectedPos, NA_SE_IT_SWORD_STRIKE_HARD);
        } else if (acElem->elemMaterial == ELEM_MATERIAL_UNK2) {
            Audio_PlaySfx_AtPos(&at->actor->projectedPos, 0);
        } else if (acElem->elemMaterial == ELEM_MATERIAL_UNK3) {
            Audio_PlaySfx_AtPos(&at->actor->projectedPos, 0);
        }
    }
    return 1;
}

ColChkBloodFunc sBloodFuncs[] = {
    CollisionCheck_NoBlood,    CollisionCheck_BlueBlood, CollisionCheck_GreenBlood,
    CollisionCheck_WaterBurst, CollisionCheck_RedBlood,  CollisionCheck_RedBloodUnused,
};

HitInfo sHitInfo[] = {
    { BLOOD_BLUE, HIT_WHITE }, { BLOOD_NONE, HIT_DUST },  { BLOOD_GREEN, HIT_DUST },  { BLOOD_NONE, HIT_WHITE },
    { BLOOD_WATER, HIT_NONE }, { BLOOD_NONE, HIT_RED },   { BLOOD_GREEN, HIT_WHITE }, { BLOOD_RED, HIT_WHITE },
    { BLOOD_BLUE, HIT_RED },   { BLOOD_NONE, HIT_SOLID }, { BLOOD_NONE, HIT_NONE },   { BLOOD_NONE, HIT_SOLID },
    { BLOOD_NONE, HIT_SOLID }, { BLOOD_NONE, HIT_WOOD },
};

/**
 * Handles hitmarks, blood, and sound effects for each AC collision, determined by the AC collider's colMaterial
 */
void CollisionCheck_HitEffects(struct PlayState* play, Collider* at, ColliderElement* atElem, Collider* ac,
                               ColliderElement* acElem, Vec3f* hitPos) {
    if (acElem->acElemFlags & ACELEM_NO_HITMARK) {
        return;
    }

    if (!(atElem->atElemFlags & ATELEM_AT_HITMARK) && (atElem->atElemFlags & ATELEM_DREW_HITMARK)) {
        return;
    }

    if (ac->actor != NULL) {
        sBloodFuncs[sHitInfo[ac->colMaterial].blood](play, ac, hitPos);
    }
    if (ac->actor != NULL) {
        if (sHitInfo[ac->colMaterial].effect == HIT_SOLID) {
            CollisionCheck_HitSolid(play, atElem, ac, hitPos);
        } else if (sHitInfo[ac->colMaterial].effect == HIT_WOOD) {
            if (at->actor == NULL) {
                CollisionCheck_SpawnShieldParticles(play, hitPos);
                Audio_PlaySfx(NA_SE_IT_REFLECTION_WOOD);
            } else {
                CollisionCheck_SpawnShieldParticlesWood(play, hitPos, &at->actor->projectedPos);
            }
        } else if (sHitInfo[ac->colMaterial].effect != HIT_NONE) {
            EffectSsHitmark_SpawnFixedScale(play, sHitInfo[ac->colMaterial].effect, hitPos);
            if (!(acElem->acElemFlags & ACELEM_NO_SWORD_SFX)) {
                CollisionCheck_SwordHitAudio(at, acElem);
            }
        }
    } else {
        EffectSsHitmark_SpawnFixedScale(play, 0, hitPos);
        if (ac->actor == NULL) {
            Audio_PlaySfx(NA_SE_IT_SHIELD_BOUND);
        } else {
            Audio_PlaySfx_AtPos(&ac->actor->projectedPos, NA_SE_IT_SHIELD_BOUND);
        }
    }
}

/**
 * Sets the flags to indicate an attack bounced off an AC_HARD collider.
 */
void CollisionCheck_SetBounce(Collider* at, Collider* ac) {
    at->atFlags |= AT_BOUNCED;
    ac->acFlags |= AC_BOUNCED;
}

/**
 * Performs the AC collision between the AT element and AC element that collided.
 */
s32 CollisionCheck_SetATvsAC(struct PlayState* play, Collider* atCol, ColliderElement* atElem, Vec3f* atPos,
                             Collider* acCol, ColliderElement* acElem, Vec3f* acPos, Vec3f* hitPos) {
    if (!GameInteractor_Should(VB_PERFORM_AC_COLLISION, true, atCol, acCol, atElem, acElem)) {
        return 0;
    }

    f32 damage;
    u32 effect;

    if (CollisionCheck_GetElementATDamage(atCol, atElem, acCol, acElem) != 0) {
        damage = CollisionCheck_GetDamageAndEffectOnElementAC(atCol, atElem, acCol, acElem, &effect);
        if (damage < 1.0f) {
            if (effect == 0) {
                return 0;
            }
        } else if ((CollisionCheck_ApplyElementATDefense(damage, acElem) < 1.0f) && (effect == 0)) {
            return 0;
        }
    }
    if ((acCol->acFlags & AC_HARD) && (atCol->actor != NULL) && (acCol->actor != NULL)) {
        CollisionCheck_SetBounce(atCol, acCol);
    }
    if (!(acElem->acElemFlags & ACELEM_NO_AT_INFO)) {
        atCol->atFlags |= AT_HIT;
        atCol->at = acCol->actor;
        atElem->atHit = acCol;
        atElem->atElemFlags |= ATELEM_HIT;
        atElem->atHitElem = acElem;
        if (!(atElem->acElemFlags & ACELEM_HIT)) {
            atElem->acDmgInfo.hitPos.x = hitPos->x;
            atElem->acDmgInfo.hitPos.y = hitPos->y;
            atElem->acDmgInfo.hitPos.z = hitPos->z;
        }
        if (atCol->actor != NULL) {
            atCol->actor->colChkInfo.atHitEffect = acElem->acDmgInfo.effect;
        }
    }
    if (!(atElem->ocElemFlags & OCELEM_UNK2)) {
        acCol->acFlags |= AC_HIT;
        acCol->ac = atCol->actor;
        acElem->acHit = atCol;
        acElem->acHitElem = atElem;
        acElem->acElemFlags |= ACELEM_HIT;
        if (acCol->actor != NULL) {
            acCol->actor->colChkInfo.acHitEffect = atElem->atDmgInfo.effect;
        }
        acElem->acDmgInfo.hitPos.x = hitPos->x;
        acElem->acDmgInfo.hitPos.y = hitPos->y;
        acElem->acDmgInfo.hitPos.z = hitPos->z;
    }
    if (!(atElem->atElemFlags & ATELEM_AT_HITMARK) && (acCol->colMaterial != COL_MATERIAL_METAL) &&
        (acCol->colMaterial != COL_MATERIAL_WOOD) && (acCol->colMaterial != COL_MATERIAL_HARD)) {
        acElem->acElemFlags |= ACELEM_DRAW_HITMARK;
    } else {
        CollisionCheck_HitEffects(play, atCol, atElem, acCol, acElem, hitPos);
        atElem->atElemFlags |= ATELEM_DREW_HITMARK;
    }
    return 1;
}

void CollisionCheck_TrisAvgPoint(ColliderTrisElement* trisElem, Vec3f* avg) {
    f32 oneThird = 1.0f / 3;

    avg->x = (trisElem->dim.vtx[0].x + trisElem->dim.vtx[1].x + trisElem->dim.vtx[2].x) * oneThird;
    avg->y = (trisElem->dim.vtx[0].y + trisElem->dim.vtx[1].y + trisElem->dim.vtx[2].y) * oneThird;
    avg->z = (trisElem->dim.vtx[0].z + trisElem->dim.vtx[1].z + trisElem->dim.vtx[2].z) * oneThird;
}

void CollisionCheck_QuadAvgPoint(ColliderQuad* quad, Vec3f* avg) {
    avg->x = (quad->dim.quad[0].x + (quad->dim.quad[1].x + (quad->dim.quad[3].x + quad->dim.quad[2].x))) / 4.0f;
    avg->y = (quad->dim.quad[0].y + (quad->dim.quad[1].y + (quad->dim.quad[3].y + quad->dim.quad[2].y))) / 4.0f;
    avg->z = (quad->dim.quad[0].z + (quad->dim.quad[1].z + (quad->dim.quad[3].z + quad->dim.quad[2].z))) / 4.0f;
}

TriNorm D_801EF600;
TriNorm D_801EF638;

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_JntSphVsJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                      Collider* acCol) {
    ColliderJntSph* atJntSph = (ColliderJntSph*)atCol;
    ColliderJntSphElement* atJntSphElem;
    ColliderJntSph* acJntSph = (ColliderJntSph*)acCol;
    ColliderJntSphElement* acJntSphElem;
    f32 overlapSize;
    f32 centerDist;

    if ((atJntSph->count > 0) && (atJntSph->elements != NULL) && (acJntSph->count > 0) &&
        (acJntSph->elements != NULL)) {
        for (atJntSphElem = atJntSph->elements; atJntSphElem < &atJntSph->elements[atJntSph->count]; atJntSphElem++) {
            if (CollisionCheck_IsElementNotAT(&atJntSphElem->base)) {
                continue;
            }

            for (acJntSphElem = acJntSph->elements; acJntSphElem < &acJntSph->elements[acJntSph->count];
                 acJntSphElem++) {
                if (CollisionCheck_IsElementNotAC(&acJntSphElem->base)) {
                    continue;
                }
                if (CollisionCheck_NoSharedFlags(&atJntSphElem->base, &acJntSphElem->base)) {
                    continue;
                }

                if (Math3D_SphVsSphOverlapCenterDist(&atJntSphElem->dim.worldSphere, &acJntSphElem->dim.worldSphere,
                                                     &overlapSize, &centerDist)) {
                    f32 acToHit;
                    Vec3f hitPos;
                    Vec3f atPos;
                    Vec3f acPos;

                    Math_Vec3s_ToVec3f(&atPos, &atJntSphElem->dim.worldSphere.center);
                    Math_Vec3s_ToVec3f(&acPos, &acJntSphElem->dim.worldSphere.center);

                    if (!IS_ZERO(centerDist)) {
                        acToHit = acJntSphElem->dim.worldSphere.radius / centerDist;
                        hitPos.x = ((atPos.x - acPos.x) * acToHit) + acPos.x;
                        hitPos.y = ((atPos.y - acPos.y) * acToHit) + acPos.y;
                        hitPos.z = ((atPos.z - acPos.z) * acToHit) + acPos.z;
                    } else {
                        Math_Vec3f_Copy(&hitPos, &atPos);
                    }
                    CollisionCheck_SetATvsAC(play, &atJntSph->base, &atJntSphElem->base, &atPos, &acJntSph->base,
                                             &acJntSphElem->base, &acPos, &hitPos);

                    if (!(acJntSph->base.ocFlags2 & OC2_FIRST_ONLY)) {
                        return;
                    }
                }
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_JntSphVsCyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                   Collider* acCol) {
    ColliderJntSph* atJntSph = (ColliderJntSph*)atCol;
    ColliderJntSphElement* atJntSphElem;
    ColliderCylinder* acCyl = (ColliderCylinder*)acCol;
    f32 overlapSize;
    f32 centerDist;

    if ((atJntSph->count > 0) && (atJntSph->elements != NULL) && (acCyl->dim.radius > 0) && (acCyl->dim.height > 0)) {
        if (CollisionCheck_IsElementNotAC(&acCyl->elem)) {
            return;
        }
        for (atJntSphElem = atJntSph->elements; atJntSphElem < &atJntSph->elements[atJntSph->count]; atJntSphElem++) {
            if (CollisionCheck_IsElementNotAT(&atJntSphElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atJntSphElem->base, &acCyl->elem)) {
                continue;
            }
            if (Math3D_SphVsCylOverlapCenterDist(&atJntSphElem->dim.worldSphere, &acCyl->dim, &overlapSize,
                                                 &centerDist)) {
                Vec3f hitPos;
                Vec3f atPos;
                Vec3f acPos;
                f32 acToHit;

                Math_Vec3s_ToVec3f(&atPos, &atJntSphElem->dim.worldSphere.center);
                Math_Vec3s_ToVec3f(&acPos, &acCyl->dim.pos);
                if (!IS_ZERO(centerDist)) {
                    acToHit = acCyl->dim.radius / centerDist;
                    if (acToHit <= 1.0f) {
                        hitPos.x = ((atPos.x - acPos.x) * acToHit) + acPos.x;
                        hitPos.y = ((atPos.y - acPos.y) * acToHit) + acPos.y;
                        hitPos.z = ((atPos.z - acPos.z) * acToHit) + acPos.z;
                    } else {
                        Math_Vec3f_Copy(&hitPos, &atPos);
                    }
                } else {
                    Math_Vec3f_Copy(&hitPos, &atPos);
                }
                CollisionCheck_SetATvsAC(play, &atJntSph->base, &atJntSphElem->base, &atPos, &acCyl->base, &acCyl->elem,
                                         &acPos, &hitPos);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_JntSphVsTris(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                    Collider* acCol) {
    ColliderJntSph* atJntSph = (ColliderJntSph*)atCol;
    ColliderJntSphElement* atJntSphElem;
    ColliderTris* acTris = (ColliderTris*)acCol;
    ColliderTrisElement* acTrisElem;
    Vec3f hitPos;

    if ((atJntSph->count > 0) && (atJntSph->elements != NULL) && (acTris->count > 0) && (acTris->elements != NULL)) {
        for (atJntSphElem = atJntSph->elements; atJntSphElem < &atJntSph->elements[atJntSph->count]; atJntSphElem++) {
            if (CollisionCheck_IsElementNotAT(&atJntSphElem->base)) {
                continue;
            }
            for (acTrisElem = acTris->elements; acTrisElem < &acTris->elements[acTris->count]; acTrisElem++) {
                if (CollisionCheck_IsElementNotAC(&acTrisElem->base)) {
                    continue;
                }
                if (CollisionCheck_NoSharedFlags(&atJntSphElem->base, &acTrisElem->base)) {
                    continue;
                }
                if (Math3D_TriVsSphIntersect(&atJntSphElem->dim.worldSphere, &acTrisElem->dim, &hitPos)) {
                    Vec3f atPos;
                    Vec3f acPos;

                    Math_Vec3s_ToVec3f(&atPos, &atJntSphElem->dim.worldSphere.center);
                    CollisionCheck_TrisAvgPoint(acTrisElem, &acPos);
                    CollisionCheck_SetATvsAC(play, &atJntSph->base, &atJntSphElem->base, &atPos, &acTris->base,
                                             &acTrisElem->base, &acPos, &hitPos);
                    return;
                }
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_JntSphVsQuad(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                    Collider* acCol) {
    ColliderJntSph* atJntSph = (ColliderJntSph*)atCol;
    ColliderQuad* acQuad = (ColliderQuad*)acCol;
    Vec3f hitPos;
    ColliderJntSphElement* atJntSphElem;

    if ((atJntSph->count > 0) && (atJntSph->elements != NULL)) {
        if (CollisionCheck_IsElementNotAC(&acQuad->elem)) {
            return;
        }
        Math3D_TriNorm(&D_801EF590, &acQuad->dim.quad[2], &acQuad->dim.quad[3], &acQuad->dim.quad[1]);
        Math3D_TriNorm(&D_801EF5C8, &acQuad->dim.quad[1], &acQuad->dim.quad[0], &acQuad->dim.quad[2]);

        for (atJntSphElem = atJntSph->elements; atJntSphElem < &atJntSph->elements[atJntSph->count]; atJntSphElem++) {
            if (CollisionCheck_IsElementNotAT(&atJntSphElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atJntSphElem->base, &acQuad->elem)) {
                continue;
            }
            if (Math3D_TriVsSphIntersect(&atJntSphElem->dim.worldSphere, &D_801EF590, &hitPos) ||
                Math3D_TriVsSphIntersect(&atJntSphElem->dim.worldSphere, &D_801EF5C8, &hitPos)) {
                Vec3f atPos;
                Vec3f acPos;

                Math_Vec3s_ToVec3f(&atPos, &atJntSphElem->dim.worldSphere.center);
                CollisionCheck_QuadAvgPoint(acQuad, &acPos);
                CollisionCheck_SetATvsAC(play, &atJntSph->base, &atJntSphElem->base, &atPos, &acQuad->base,
                                         &acQuad->elem, &acPos, &hitPos);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_JntSphVsSphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                      Collider* acCol) {
    ColliderJntSph* atJntSph = (ColliderJntSph*)atCol;
    ColliderSphere* acSph = (ColliderSphere*)acCol;
    ColliderJntSphElement* atJntSphElem;
    f32 overlapSize;
    f32 centerDist;

    if ((atJntSph->count > 0) && (atJntSph->elements != NULL)) {
        if (CollisionCheck_IsElementNotAC(&acSph->elem)) {
            return;
        }
        for (atJntSphElem = atJntSph->elements; atJntSphElem < &atJntSph->elements[atJntSph->count]; atJntSphElem++) {
            if (CollisionCheck_IsElementNotAT(&atJntSphElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atJntSphElem->base, &acSph->elem)) {
                continue;
            }
            if (Math3D_SphVsSphOverlapCenterDist(&atJntSphElem->dim.worldSphere, &acSph->dim.worldSphere, &overlapSize,
                                                 &centerDist)) {
                f32 acToHit;
                Vec3f hitPos;
                Vec3f atPos;
                Vec3f acPos;

                Math_Vec3s_ToVec3f(&atPos, &atJntSphElem->dim.worldSphere.center);
                Math_Vec3s_ToVec3f(&acPos, &acSph->dim.worldSphere.center);
                if (!IS_ZERO(centerDist)) {
                    acToHit = acSph->dim.worldSphere.radius / centerDist;
                    hitPos.x = ((atPos.x - acPos.x) * acToHit) + acPos.x;
                    hitPos.y = ((atPos.y - acPos.y) * acToHit) + acPos.y;
                    hitPos.z = ((atPos.z - acPos.z) * acToHit) + acPos.z;
                } else {
                    Math_Vec3f_Copy(&hitPos, &atPos);
                }
                CollisionCheck_SetATvsAC(play, &atJntSph->base, &atJntSphElem->base, &atPos, &acSph->base, &acSph->elem,
                                         &acPos, &hitPos);
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_CylVsJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                   Collider* acCol) {
    ColliderCylinder* atCyl = (ColliderCylinder*)atCol;
    ColliderJntSph* acJntSph = (ColliderJntSph*)acCol;
    f32 overlapSize;
    f32 centerDist;
    ColliderJntSphElement* acJntSphElem;

    if ((acJntSph->count > 0) && (acJntSph->elements != NULL) && (atCyl->dim.radius > 0) && (atCyl->dim.height > 0)) {
        if (CollisionCheck_IsElementNotAT(&atCyl->elem)) {
            return;
        }
        for (acJntSphElem = acJntSph->elements; acJntSphElem < &acJntSph->elements[acJntSph->count]; acJntSphElem++) {
            if (CollisionCheck_IsElementNotAC(&acJntSphElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atCyl->elem, &acJntSphElem->base)) {
                continue;
            }
            if (Math3D_SphVsCylOverlapCenterDist(&acJntSphElem->dim.worldSphere, &atCyl->dim, &overlapSize,
                                                 &centerDist)) {
                Vec3f hitPos;
                Vec3f atPos;
                Vec3f acPos;
                f32 acToHit;

                Math_Vec3s_ToVec3f(&atPos, &atCyl->dim.pos);
                Math_Vec3s_ToVec3f(&acPos, &acJntSphElem->dim.worldSphere.center);
                if (!IS_ZERO(centerDist)) {
                    acToHit = acJntSphElem->dim.worldSphere.radius / centerDist;
                    if (acToHit <= 1.0f) {
                        hitPos.x = ((atPos.x - acPos.x) * acToHit) + acPos.x;
                        hitPos.y = ((atPos.y - acPos.y) * acToHit) + acPos.y;
                        hitPos.z = ((atPos.z - acPos.z) * acToHit) + acPos.z;
                    } else {
                        Math_Vec3f_Copy(&hitPos, &atPos);
                    }
                } else {
                    Math_Vec3f_Copy(&hitPos, &atPos);
                }
                CollisionCheck_SetATvsAC(play, &atCyl->base, &atCyl->elem, &atPos, &acJntSph->base, &acJntSphElem->base,
                                         &acPos, &hitPos);
                if (!(acJntSph->base.ocFlags2 & OC2_FIRST_ONLY)) {
                    break;
                }
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_CylVsCyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                Collider* acCol) {
    ColliderCylinder* atCyl = (ColliderCylinder*)atCol;
    ColliderCylinder* acCyl = (ColliderCylinder*)acCol;
    f32 overlapSize;
    f32 centerDist;

    if ((atCyl->dim.radius > 0) && (atCyl->dim.height > 0) && (acCyl->dim.radius > 0) && (acCyl->dim.height > 0)) {
        if (CollisionCheck_IsElementNotAC(&acCyl->elem)) {
            return;
        }
        if (CollisionCheck_IsElementNotAT(&atCyl->elem)) {
            return;
        }
        if (CollisionCheck_NoSharedFlags(&atCyl->elem, &acCyl->elem)) {
            return;
        }

        if (Math3D_CylVsCylOverlapCenterDist(&atCyl->dim, &acCyl->dim, &overlapSize, &centerDist)) {
            Vec3f hitPos;
            Vec3f atPos;
            Vec3f acPos;

            Math_Vec3s_ToVec3f(&atPos, &atCyl->dim.pos);
            Math_Vec3s_ToVec3f(&acPos, &acCyl->dim.pos);
            if (!IS_ZERO(centerDist)) {
                f32 acToHit = acCyl->dim.radius / centerDist;
                f32 height;

                hitPos.y = (f32)atCyl->dim.pos.y + atCyl->dim.yShift + atCyl->dim.height * 0.5f;
                height = (f32)acCyl->dim.pos.y + acCyl->dim.yShift;

                if (hitPos.y < height) {
                    hitPos.y = height;
                } else {
                    height += acCyl->dim.height;
                    if (hitPos.y > height) {
                        hitPos.y = height;
                    }
                }
                hitPos.x = ((f32)atCyl->dim.pos.x - acCyl->dim.pos.x) * acToHit + acCyl->dim.pos.x;
                hitPos.z = ((f32)atCyl->dim.pos.z - acCyl->dim.pos.z) * acToHit + acCyl->dim.pos.z;
            } else {
                Math_Vec3s_ToVec3f(&hitPos, &acCyl->dim.pos);
            }
            CollisionCheck_SetATvsAC(play, &atCyl->base, &atCyl->elem, &atPos, &acCyl->base, &acCyl->elem, &acPos,
                                     &hitPos);
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_CylVsTris(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                 Collider* acCol) {
    ColliderCylinder* atCyl = (ColliderCylinder*)atCol;
    ColliderTris* acTris = (ColliderTris*)acCol;
    ColliderTrisElement* acTrisElem;
    Vec3f hitPos;

    if ((atCyl->dim.radius > 0) && (atCyl->dim.height > 0) && (acTris->count > 0) && (acTris->elements != NULL)) {
        if (CollisionCheck_IsElementNotAT(&atCyl->elem)) {
            return;
        }

        for (acTrisElem = acTris->elements; acTrisElem < &acTris->elements[acTris->count]; acTrisElem++) {
            if (CollisionCheck_IsElementNotAC(&acTrisElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atCyl->elem, &acTrisElem->base)) {
                continue;
            }

            if (Math3D_CylTriVsIntersect(&atCyl->dim, &acTrisElem->dim, &hitPos)) {
                Vec3f atPos;
                Vec3f acPos;

                Math_Vec3s_ToVec3f(&atPos, &atCyl->dim.pos);
                CollisionCheck_TrisAvgPoint(acTrisElem, &acPos);
                CollisionCheck_SetATvsAC(play, &atCyl->base, &atCyl->elem, &atPos, &acTris->base, &acTrisElem->base,
                                         &acPos, &hitPos);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_CylVsQuad(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                 Collider* acCol) {
    ColliderCylinder* atCyl = (ColliderCylinder*)atCol;
    ColliderQuad* acQuad = (ColliderQuad*)acCol;

    if ((atCyl->dim.height > 0) && (atCyl->dim.radius > 0)) {
        if (CollisionCheck_IsElementNotAT(&atCyl->elem)) {
            return;
        }
        if (CollisionCheck_IsElementNotAC(&acQuad->elem)) {
            return;
        }
        if (CollisionCheck_NoSharedFlags(&atCyl->elem, &acQuad->elem)) {
            return;
        }
        Math3D_TriNorm(&D_801EF600, &acQuad->dim.quad[2], &acQuad->dim.quad[3], &acQuad->dim.quad[1]);
        Math3D_TriNorm(&D_801EF638, &acQuad->dim.quad[1], &acQuad->dim.quad[0], &acQuad->dim.quad[2]);

        if (Math3D_CylTriVsIntersect(&atCyl->dim, &D_801EF600, &D_801EDE00)) {
            Vec3f atPos;
            Vec3f acPos;

            Math_Vec3s_ToVec3f(&atPos, &atCyl->dim.pos);
            CollisionCheck_QuadAvgPoint(acQuad, &acPos);
            CollisionCheck_SetATvsAC(play, &atCyl->base, &atCyl->elem, &atPos, &acQuad->base, &acQuad->elem, &acPos,
                                     &D_801EDE00);
        } else if (Math3D_CylTriVsIntersect(&atCyl->dim, &D_801EF638, &D_801EDE00)) {
            Vec3f atPos;
            Vec3f acPos;

            Math_Vec3s_ToVec3f(&atPos, &atCyl->dim.pos);
            CollisionCheck_QuadAvgPoint(acQuad, &acPos);
            CollisionCheck_SetATvsAC(play, &atCyl->base, &atCyl->elem, &atPos, &acQuad->base, &acQuad->elem, &acPos,
                                     &D_801EDE00);
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_CylVsSphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                   Collider* acCol) {
    ColliderCylinder* atCyl = (ColliderCylinder*)atCol;
    ColliderSphere* acSph = (ColliderSphere*)acCol;
    f32 overlapSize;
    f32 centerDist;

    if ((atCyl->dim.radius > 0) && (atCyl->dim.height > 0)) {
        if (CollisionCheck_IsElementNotAT(&atCyl->elem)) {
            return;
        }
        if (CollisionCheck_IsElementNotAC(&acSph->elem)) {
            return;
        }
        if (CollisionCheck_NoSharedFlags(&atCyl->elem, &acSph->elem)) {
            return;
        }

        if (Math3D_SphVsCylOverlapCenterDist(&acSph->dim.worldSphere, &atCyl->dim, &overlapSize, &centerDist)) {
            Vec3f hitPos;
            Vec3f atPos;
            Vec3f acPos;
            f32 acToHit;

            Math_Vec3s_ToVec3f(&atPos, &atCyl->dim.pos);
            Math_Vec3s_ToVec3f(&acPos, &acSph->dim.worldSphere.center);

            if (!IS_ZERO(centerDist)) {
                acToHit = acSph->dim.worldSphere.radius / centerDist;
                if (acToHit <= 1.0f) {
                    hitPos.x = ((atPos.x - acPos.x) * acToHit) + acPos.x;
                    hitPos.y = ((atPos.y - acPos.y) * acToHit) + acPos.y;
                    hitPos.z = ((atPos.z - acPos.z) * acToHit) + acPos.z;
                } else {
                    Math_Vec3f_Copy(&hitPos, &atPos);
                }
            } else {
                Math_Vec3f_Copy(&hitPos, &atPos);
            }
            CollisionCheck_SetATvsAC(play, &atCyl->base, &atCyl->elem, &atPos, &acSph->base, &acSph->elem, &acPos,
                                     &hitPos);
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_TrisVsJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                    Collider* acCol) {
    ColliderTris* atTris = (ColliderTris*)atCol;
    ColliderJntSphElement* acJntSphElem;
    ColliderJntSph* acJntSph = (ColliderJntSph*)acCol;
    ColliderTrisElement* atTrisElem;
    Vec3f hitPos;

    if ((acJntSph->count > 0) && (acJntSph->elements != NULL) && (atTris->count > 0) && (atTris->elements != NULL)) {
        for (acJntSphElem = acJntSph->elements; acJntSphElem < &acJntSph->elements[acJntSph->count]; acJntSphElem++) {
            if (CollisionCheck_IsElementNotAC(&acJntSphElem->base)) {
                continue;
            }
            for (atTrisElem = atTris->elements; atTrisElem < &atTris->elements[atTris->count]; atTrisElem++) {
                if (CollisionCheck_IsElementNotAT(&atTrisElem->base)) {
                    continue;
                }
                if (CollisionCheck_NoSharedFlags(&atTrisElem->base, &acJntSphElem->base)) {
                    continue;
                }
                if (Math3D_TriVsSphIntersect(&acJntSphElem->dim.worldSphere, &atTrisElem->dim, &hitPos)) {
                    Vec3f atPos;
                    Vec3f acPos;

                    Math_Vec3s_ToVec3f(&acPos, &acJntSphElem->dim.worldSphere.center);
                    CollisionCheck_TrisAvgPoint(atTrisElem, &atPos);
                    CollisionCheck_SetATvsAC(play, &atTris->base, &atTrisElem->base, &atPos, &acJntSph->base,
                                             &acJntSphElem->base, &acPos, &hitPos);

                    if (!(acJntSph->base.ocFlags2 & OC2_FIRST_ONLY)) {
                        return;
                    }
                }
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_TrisVsCyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                 Collider* acCol) {
    ColliderTris* atTris = (ColliderTris*)atCol;
    ColliderCylinder* acCyl = (ColliderCylinder*)acCol;
    ColliderTrisElement* atTrisElem;

    if ((acCyl->dim.radius > 0) && (acCyl->dim.height > 0) && (atTris->count > 0) && (atTris->elements != NULL)) {
        if (CollisionCheck_IsElementNotAC(&acCyl->elem)) {
            return;
        }
        for (atTrisElem = atTris->elements; atTrisElem < &atTris->elements[atTris->count]; atTrisElem++) {
            if (CollisionCheck_IsElementNotAT(&atTrisElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atTrisElem->base, &acCyl->elem)) {
                continue;
            }
            if (Math3D_CylTriVsIntersect(&acCyl->dim, &atTrisElem->dim, &D_801EDE10)) {
                Vec3f atPos;
                Vec3f acPos;

                CollisionCheck_TrisAvgPoint(atTrisElem, &atPos);
                Math_Vec3s_ToVec3f(&acPos, &acCyl->dim.pos);
                CollisionCheck_SetATvsAC(play, &atTris->base, &atTrisElem->base, &atPos, &acCyl->base, &acCyl->elem,
                                         &acPos, &D_801EDE10);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_TrisVsTris(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                  Collider* acCol) {
    ColliderTris* atTris = (ColliderTris*)atCol;
    ColliderTrisElement* atTrisElem;
    ColliderTris* acTris = (ColliderTris*)acCol;
    ColliderTrisElement* acTrisElem;

    if ((acTris->count > 0) && (acTris->elements != NULL) && (atTris->count > 0) && (atTris->elements != NULL)) {
        for (acTrisElem = acTris->elements; acTrisElem < &acTris->elements[acTris->count]; acTrisElem++) {
            if (CollisionCheck_IsElementNotAC(&acTrisElem->base)) {
                continue;
            }
            for (atTrisElem = atTris->elements; atTrisElem < &atTris->elements[atTris->count]; atTrisElem++) {
                if (CollisionCheck_IsElementNotAT(&atTrisElem->base)) {
                    continue;
                }
                if (CollisionCheck_NoSharedFlags(&atTrisElem->base, &acTrisElem->base)) {
                    continue;
                }
                if (Math3D_TriVsTriIntersect(&atTrisElem->dim, &acTrisElem->dim, &D_801EDE20)) {
                    Vec3f atPos;
                    Vec3f acPos;

                    CollisionCheck_TrisAvgPoint(atTrisElem, &atPos);
                    CollisionCheck_TrisAvgPoint(acTrisElem, &acPos);
                    CollisionCheck_SetATvsAC(play, &atTris->base, &atTrisElem->base, &atPos, &acTris->base,
                                             &acTrisElem->base, &acPos, &D_801EDE20);
                    return;
                }
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_TrisVsQuad(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                  Collider* acCol) {
    ColliderTris* atTris = (ColliderTris*)atCol;
    ColliderQuad* acQuad = (ColliderQuad*)acCol;
    ColliderTrisElement* atTrisElem;

    if ((atTris->count > 0) && (atTris->elements != NULL)) {
        if (CollisionCheck_IsElementNotAC(&acQuad->elem)) {
            return;
        }

        Math3D_TriNorm(&D_801EDE40, &acQuad->dim.quad[2], &acQuad->dim.quad[3], &acQuad->dim.quad[1]);
        Math3D_TriNorm(&D_801EDE78, &acQuad->dim.quad[1], &acQuad->dim.quad[0], &acQuad->dim.quad[2]);

        for (atTrisElem = atTris->elements; atTrisElem < &atTris->elements[atTris->count]; atTrisElem++) {
            if (CollisionCheck_IsElementNotAT(&atTrisElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atTrisElem->base, &acQuad->elem)) {
                continue;
            }
            if (Math3D_TriVsTriIntersect(&D_801EDE40, &atTrisElem->dim, &D_801EDE30) ||
                Math3D_TriVsTriIntersect(&D_801EDE78, &atTrisElem->dim, &D_801EDE30)) {
                Vec3f atPos;
                Vec3f acPos;

                CollisionCheck_TrisAvgPoint(atTrisElem, &atPos);
                CollisionCheck_QuadAvgPoint(acQuad, &acPos);
                CollisionCheck_SetATvsAC(play, &atTris->base, &atTrisElem->base, &atPos, &acQuad->base, &acQuad->elem,
                                         &acPos, &D_801EDE30);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_TrisVsSphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                    Collider* acCol) {
    ColliderTris* atTris = (ColliderTris*)atCol;
    ColliderTrisElement* atTrisElem;
    ColliderSphere* acSph = (ColliderSphere*)acCol;
    Vec3f hitPos;

    if ((atTris->count > 0) && (atTris->elements != NULL)) {
        if (CollisionCheck_IsElementNotAC(&acSph->elem)) {
            return;
        }
        for (atTrisElem = atTris->elements; atTrisElem < &atTris->elements[atTris->count]; atTrisElem++) {
            if (CollisionCheck_IsElementNotAT(&atTrisElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atTrisElem->base, &acSph->elem)) {
                continue;
            }
            if (Math3D_TriVsSphIntersect(&acSph->dim.worldSphere, &atTrisElem->dim, &hitPos)) {
                Vec3f atPos;
                Vec3f acPos;

                Math_Vec3s_ToVec3f(&acPos, &acSph->dim.worldSphere.center);
                CollisionCheck_TrisAvgPoint(atTrisElem, &atPos);
                CollisionCheck_SetATvsAC(play, &atTris->base, &atTrisElem->base, &atPos, &acSph->base, &acSph->elem,
                                         &acPos, &hitPos);
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_QuadVsJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                    Collider* acCol) {
    ColliderJntSphElement* acJntSphElem;
    Vec3f hitPos;
    ColliderQuad* atQuad = (ColliderQuad*)atCol;
    ColliderJntSph* acJntSph = (ColliderJntSph*)acCol;

    if ((acJntSph->count > 0) && (acJntSph->elements != NULL)) {
        if (CollisionCheck_IsElementNotAT(&atQuad->elem)) {
            return;
        }
        Math3D_TriNorm(&D_801EDEC8, &atQuad->dim.quad[2], &atQuad->dim.quad[3], &atQuad->dim.quad[1]);
        Math3D_TriNorm(&D_801EDF00, &atQuad->dim.quad[2], &atQuad->dim.quad[1], &atQuad->dim.quad[0]);

        for (acJntSphElem = acJntSph->elements; acJntSphElem < &acJntSph->elements[acJntSph->count]; acJntSphElem++) {
            if (CollisionCheck_IsElementNotAC(&acJntSphElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atQuad->elem, &acJntSphElem->base)) {
                continue;
            }
            if (Math3D_TriVsSphIntersect(&acJntSphElem->dim.worldSphere, &D_801EDEC8, &hitPos) ||
                Math3D_TriVsSphIntersect(&acJntSphElem->dim.worldSphere, &D_801EDF00, &hitPos)) {
                Vec3f atPos;
                Vec3f acPos;

                if (!Collider_QuadSetNearestAC(play, atQuad, &hitPos)) {
                    continue;
                }
                Math_Vec3s_ToVec3f(&acPos, &acJntSphElem->dim.worldSphere.center);
                CollisionCheck_QuadAvgPoint(atQuad, &atPos);
                CollisionCheck_SetATvsAC(play, &atQuad->base, &atQuad->elem, &atPos, &acJntSph->base,
                                         &acJntSphElem->base, &acPos, &hitPos);

                if (!(acJntSph->base.ocFlags2 & OC2_FIRST_ONLY)) {
                    break;
                }
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_QuadVsCyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                 Collider* acCol) {
    ColliderQuad* atQuad = (ColliderQuad*)atCol;
    ColliderCylinder* acCyl = (ColliderCylinder*)acCol;

    if ((acCyl->dim.height > 0) && (acCyl->dim.radius > 0)) {
        if (CollisionCheck_IsElementNotAC(&acCyl->elem)) {
            return;
        }
        if (CollisionCheck_IsElementNotAT(&atQuad->elem)) {
            return;
        }
        if (CollisionCheck_NoSharedFlags(&atQuad->elem, &acCyl->elem)) {
            return;
        }

        Math3D_TriNorm(&D_801EDF58, &atQuad->dim.quad[2], &atQuad->dim.quad[3], &atQuad->dim.quad[1]);
        Math3D_TriNorm(&D_801EDF90, &atQuad->dim.quad[2], &atQuad->dim.quad[1], &atQuad->dim.quad[0]);

        if (Math3D_CylTriVsIntersect(&acCyl->dim, &D_801EDF58, &D_801EDFE0)) {
            if (Collider_QuadSetNearestAC(play, atQuad, &D_801EDFE0)) {
                Vec3f atPos;
                Vec3f acPos;

                CollisionCheck_QuadAvgPoint(atQuad, &atPos);
                Math_Vec3s_ToVec3f(&acPos, &acCyl->dim.pos);
                CollisionCheck_SetATvsAC(play, &atQuad->base, &atQuad->elem, &atPos, &acCyl->base, &acCyl->elem, &acPos,
                                         &D_801EDFE0);
                return;
            }
        }
        if (Math3D_CylTriVsIntersect(&acCyl->dim, &D_801EDF90, &D_801EDFE0)) {
            if (Collider_QuadSetNearestAC(play, atQuad, &D_801EDFE0)) {
                Vec3f atPos;
                Vec3f acPos;

                CollisionCheck_QuadAvgPoint(atQuad, &atPos);
                Math_Vec3s_ToVec3f(&acPos, &acCyl->dim.pos);
                CollisionCheck_SetATvsAC(play, &atQuad->base, &atQuad->elem, &atPos, &acCyl->base, &acCyl->elem, &acPos,
                                         &D_801EDFE0);
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_QuadVsTris(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                  Collider* acCol) {
    ColliderQuad* atQuad = (ColliderQuad*)atCol;
    ColliderTris* acTris = (ColliderTris*)acCol;
    ColliderTrisElement* acTrisElem;

    if ((acTris->count > 0) && (acTris->elements != NULL)) {
        if (CollisionCheck_IsElementNotAT(&atQuad->elem)) {
            return;
        }

        Math3D_TriNorm(&D_801EE000, &atQuad->dim.quad[2], &atQuad->dim.quad[3], &atQuad->dim.quad[1]);
        Math3D_TriNorm(&D_801EE038, &atQuad->dim.quad[1], &atQuad->dim.quad[0], &atQuad->dim.quad[2]);

        for (acTrisElem = acTris->elements; acTrisElem < &acTris->elements[acTris->count]; acTrisElem++) {
            if (CollisionCheck_IsElementNotAC(&acTrisElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atQuad->elem, &acTrisElem->base)) {
                continue;
            }

            if ((Math3D_TriVsTriIntersect(&D_801EE000, &acTrisElem->dim, &D_801EDFF0)) ||
                (Math3D_TriVsTriIntersect(&D_801EE038, &acTrisElem->dim, &D_801EDFF0))) {
                if (Collider_QuadSetNearestAC(play, atQuad, &D_801EDFF0)) {
                    Vec3f atPos;
                    Vec3f acPos;

                    CollisionCheck_TrisAvgPoint(acTrisElem, &acPos);
                    CollisionCheck_QuadAvgPoint(atQuad, &atPos);
                    CollisionCheck_SetATvsAC(play, &atQuad->base, &atQuad->elem, &atPos, &acTris->base,
                                             &acTrisElem->base, &acPos, &D_801EDFF0);
                    return;
                }
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_QuadVsQuad(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                  Collider* acCol) {
    ColliderQuad* atQuad = (ColliderQuad*)atCol;
    ColliderQuad* acQuad = (ColliderQuad*)acCol;
    s32 i;
    s32 j;

    if (CollisionCheck_IsElementNotAT(&atQuad->elem)) {
        return;
    }
    if (CollisionCheck_IsElementNotAC(&acQuad->elem)) {
        return;
    }
    if (CollisionCheck_NoSharedFlags(&atQuad->elem, &acQuad->elem)) {
        return;
    }

    Math3D_TriNorm(&D_801EE0E8[0], &atQuad->dim.quad[2], &atQuad->dim.quad[3], &atQuad->dim.quad[1]);
    Math3D_TriNorm(&D_801EE0E8[1], &atQuad->dim.quad[2], &atQuad->dim.quad[1], &atQuad->dim.quad[0]);
    Math3D_TriNorm(&D_801EE070[0], &acQuad->dim.quad[2], &acQuad->dim.quad[3], &acQuad->dim.quad[1]);
    Math3D_TriNorm(&D_801EE070[1], &acQuad->dim.quad[2], &acQuad->dim.quad[1], &acQuad->dim.quad[0]);

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (Math3D_TriVsTriIntersect(&D_801EE0E8[j], &D_801EE070[i], &D_801EE0D8) &&
                Collider_QuadSetNearestAC(play, atQuad, &D_801EE0D8)) {
                Vec3f atPos;
                Vec3f acPos;

                CollisionCheck_QuadAvgPoint(atQuad, &atPos);
                CollisionCheck_QuadAvgPoint(acQuad, &acPos);
                CollisionCheck_SetATvsAC(play, &atQuad->base, &atQuad->elem, &atPos, &acQuad->base, &acQuad->elem,
                                         &acPos, &D_801EE0D8);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_QuadVsSphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                    Collider* acCol) {
    ColliderQuad* atQuad = (ColliderQuad*)atCol;
    Vec3f hitPos;
    ColliderSphere* acSph = (ColliderSphere*)acCol;

    if (CollisionCheck_IsElementNotAT(&atQuad->elem)) {
        return;
    }
    if (CollisionCheck_IsElementNotAC(&acSph->elem) || CollisionCheck_NoSharedFlags(&atQuad->elem, &acSph->elem)) {
        return;
    }

    Math3D_TriNorm(&D_801EE150, &atQuad->dim.quad[2], &atQuad->dim.quad[3], &atQuad->dim.quad[1]);
    Math3D_TriNorm(&D_801EE188, &atQuad->dim.quad[2], &atQuad->dim.quad[1], &atQuad->dim.quad[0]);

    if ((Math3D_TriVsSphIntersect(&acSph->dim.worldSphere, &D_801EE150, &hitPos)) ||
        (Math3D_TriVsSphIntersect(&acSph->dim.worldSphere, &D_801EE188, &hitPos))) {
        if (Collider_QuadSetNearestAC(play, atQuad, &hitPos)) {
            Vec3f atPos;
            Vec3f acPos;

            Math_Vec3s_ToVec3f(&acPos, &acSph->dim.worldSphere.center);
            CollisionCheck_QuadAvgPoint(atQuad, &atPos);
            CollisionCheck_SetATvsAC(play, &atQuad->base, &atQuad->elem, &atPos, &acSph->base, &acSph->elem, &acPos,
                                     &hitPos);
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_SphereVsJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                      Collider* acCol) {
    ColliderSphere* atSph = (ColliderSphere*)atCol;
    ColliderJntSph* acJntSph = (ColliderJntSph*)acCol;
    ColliderJntSphElement* acJntSphElem;
    f32 overlapSize;
    f32 centerDist;

    if ((acJntSph->count > 0) && (acJntSph->elements != NULL)) {
        if (CollisionCheck_IsElementNotAT(&atSph->elem)) {
            return;
        }

        for (acJntSphElem = acJntSph->elements; acJntSphElem < &acJntSph->elements[acJntSph->count]; acJntSphElem++) {
            if (CollisionCheck_IsElementNotAC(&acJntSphElem->base)) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atSph->elem, &acJntSphElem->base)) {
                continue;
            }

            if (Math3D_SphVsSphOverlapCenterDist(&atSph->dim.worldSphere, &acJntSphElem->dim.worldSphere, &overlapSize,
                                                 &centerDist)) {
                f32 acToHit;
                Vec3f hitPos;
                Vec3f atPos;
                Vec3f acPos;

                Math_Vec3s_ToVec3f(&atPos, &atSph->dim.worldSphere.center);
                Math_Vec3s_ToVec3f(&acPos, &acJntSphElem->dim.worldSphere.center);
                if (!IS_ZERO(centerDist)) {
                    acToHit = acJntSphElem->dim.worldSphere.radius / centerDist;
                    hitPos.x = (atPos.x - acPos.x) * acToHit + acPos.x;
                    hitPos.y = (atPos.y - acPos.y) * acToHit + acPos.y;
                    hitPos.z = (atPos.z - acPos.z) * acToHit + acPos.z;
                } else {
                    Math_Vec3f_Copy(&hitPos, &atPos);
                }
                CollisionCheck_SetATvsAC(play, &atSph->base, &atSph->elem, &atPos, &acJntSph->base, &acJntSphElem->base,
                                         &acPos, &hitPos);
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_SphereVsCylinder(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                        Collider* acCol) {
    ColliderCylinder* acCyl = (ColliderCylinder*)acCol;
    ColliderSphere* atSph = (ColliderSphere*)atCol;
    f32 overlapSize;
    f32 centerDist;

    if (CollisionCheck_IsElementNotAT(&atSph->elem)) {
        return;
    }
    if (CollisionCheck_IsElementNotAC(&acCyl->elem)) {
        return;
    }
    if (CollisionCheck_NoSharedFlags(&atSph->elem, &acCyl->elem)) {
        return;
    }

    if (Math3D_SphVsCylOverlapCenterDist(&atSph->dim.worldSphere, &acCyl->dim, &overlapSize, &centerDist)) {
        Vec3f hitPos;
        Vec3f atPos;
        Vec3f acPos;

        Math_Vec3s_ToVec3f(&atPos, &atSph->dim.worldSphere.center);
        Math_Vec3s_ToVec3f(&acPos, &acCyl->dim.pos);

        if (!IS_ZERO(centerDist)) {
            f32 acToHit = acCyl->dim.radius / centerDist;

            if (acToHit <= 1.0f) {
                hitPos.x = (atPos.x - acPos.x) * acToHit + acPos.x;
                hitPos.y = (atPos.y - acPos.y) * acToHit + acPos.y;
                hitPos.z = (atPos.z - acPos.z) * acToHit + acPos.z;
            } else {
                Math_Vec3f_Copy(&hitPos, &atPos);
            }
        } else {
            Math_Vec3f_Copy(&hitPos, &atPos);
        }
        CollisionCheck_SetATvsAC(play, &atSph->base, &atSph->elem, &atPos, &acCyl->base, &acCyl->elem, &acPos, &hitPos);
    }

    if (atSph) {}
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_SphereVsTris(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                    Collider* acCol) {
    ColliderSphere* atSph = (ColliderSphere*)atCol;
    ColliderTris* acTris = (ColliderTris*)acCol;
    ColliderTrisElement* acTrisElem;
    Vec3f hitPos;

    if (CollisionCheck_IsElementNotAT(&atSph->elem)) {
        return;
    }

    for (acTrisElem = acTris->elements; acTrisElem < &acTris->elements[acTris->count]; acTrisElem++) {
        if (CollisionCheck_IsElementNotAC(&acTrisElem->base)) {
            continue;
        }
        if (CollisionCheck_NoSharedFlags(&atSph->elem, &acTrisElem->base)) {
            continue;
        }
        if (Math3D_TriVsSphIntersect(&atSph->dim.worldSphere, &acTrisElem->dim, &hitPos)) {
            Vec3f atPos;
            Vec3f acPos;

            Math_Vec3s_ToVec3f(&atPos, &atSph->dim.worldSphere.center);
            CollisionCheck_TrisAvgPoint(acTrisElem, &acPos);
            CollisionCheck_SetATvsAC(play, &atSph->base, &atSph->elem, &atPos, &acTris->base, &acTrisElem->base, &acPos,
                                     &hitPos);
            return;
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_SphereVsQuad(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                    Collider* acCol) {
    ColliderSphere* atSph = (ColliderSphere*)atCol;
    Vec3f hitPos;
    ColliderQuad* acQuad = (ColliderQuad*)acCol;

    if (CollisionCheck_IsElementNotAT(&atSph->elem)) {
        return;
    }
    if (CollisionCheck_IsElementNotAC(&acQuad->elem) || CollisionCheck_NoSharedFlags(&atSph->elem, &acQuad->elem)) {
        return;
    }

    Math3D_TriNorm(&D_801EE6C8.norm1, &acQuad->dim.quad[2], &acQuad->dim.quad[3], &acQuad->dim.quad[1]);
    Math3D_TriNorm(&D_801EE6C8.norm2, &acQuad->dim.quad[1], &acQuad->dim.quad[0], &acQuad->dim.quad[2]);

    if (Math3D_TriVsSphIntersect(&atSph->dim.worldSphere, &D_801EE6C8.norm1, &hitPos) ||
        Math3D_TriVsSphIntersect(&atSph->dim.worldSphere, &D_801EE6C8.norm2, &hitPos)) {
        Vec3f atPos;
        Vec3f acPos;

        Math_Vec3s_ToVec3f(&atPos, &atSph->dim.worldSphere.center);
        CollisionCheck_QuadAvgPoint(acQuad, &acPos);
        CollisionCheck_SetATvsAC(play, &atSph->base, &atSph->elem, &atPos, &acQuad->base, &acQuad->elem, &acPos,
                                 &hitPos);
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_SphereVsSphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                      Collider* acCol) {
    ColliderSphere* atSph = (ColliderSphere*)atCol;
    ColliderSphere* acSph = (ColliderSphere*)acCol;
    f32 overlapSize;
    f32 centerDist;

    if (CollisionCheck_IsElementNotAT(&atSph->elem)) {
        return;
    }
    if (CollisionCheck_IsElementNotAC(&acSph->elem)) {
        return;
    }
    if (CollisionCheck_NoSharedFlags(&atSph->elem, &acSph->elem)) {
        return;
    }

    if (Math3D_SphVsSphOverlapCenterDist(&atSph->dim.worldSphere, &acSph->dim.worldSphere, &overlapSize, &centerDist)) {
        f32 acToHit;
        Vec3f hitPos;
        Vec3f atPos;
        Vec3f acPos;

        Math_Vec3s_ToVec3f(&atPos, &atSph->dim.worldSphere.center);
        Math_Vec3s_ToVec3f(&acPos, &acSph->dim.worldSphere.center);
        if (!IS_ZERO(centerDist)) {
            acToHit = acSph->dim.worldSphere.radius / centerDist;
            hitPos.x = (atPos.x - acPos.x) * acToHit + acPos.x;
            hitPos.y = (atPos.y - acPos.y) * acToHit + acPos.y;
            hitPos.z = (atPos.z - acPos.z) * acToHit + acPos.z;
        } else {
            Math_Vec3f_Copy(&hitPos, &atPos);
        }
        CollisionCheck_SetATvsAC(play, &atSph->base, &atSph->elem, &atPos, &acSph->base, &acSph->elem, &acPos, &hitPos);
    }
}

/**
 * Sets a ColliderJntSph's hit effects
 */
void CollisionCheck_SetJntSphHitFX(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderJntSph* jntSph = (ColliderJntSph*)col;
    ColliderJntSphElement* jntSphElem;

    for (jntSphElem = jntSph->elements; jntSphElem < &jntSph->elements[jntSph->count]; jntSphElem++) {
        if ((jntSphElem->base.acElemFlags & ACELEM_DRAW_HITMARK) && (jntSphElem->base.acHitElem != NULL) &&
            !(jntSphElem->base.acHitElem->atElemFlags & ATELEM_DREW_HITMARK)) {
            Vec3f hitPos;

            Math_Vec3s_ToVec3f(&hitPos, &jntSphElem->base.acDmgInfo.hitPos);
            CollisionCheck_HitEffects(play, jntSphElem->base.acHit, jntSphElem->base.acHitElem, &jntSph->base,
                                      &jntSphElem->base, &hitPos);
            jntSphElem->base.acHitElem->atElemFlags |= ATELEM_DREW_HITMARK;
            return;
        }
    }
}

/**
 * Sets a ColliderCylinder's hit effects
 */
void CollisionCheck_SetCylHitFX(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    if ((cyl->elem.acElemFlags & ACELEM_DRAW_HITMARK) && (cyl->elem.acHitElem != NULL) &&
        !(cyl->elem.acHitElem->atElemFlags & ATELEM_DREW_HITMARK)) {
        Vec3f hitPos;

        Math_Vec3s_ToVec3f(&hitPos, &cyl->elem.acDmgInfo.hitPos);
        CollisionCheck_HitEffects(play, cyl->elem.acHit, cyl->elem.acHitElem, &cyl->base, &cyl->elem, &hitPos);
        cyl->elem.acHitElem->atElemFlags |= ATELEM_DREW_HITMARK;
    }
}

/**
 * Sets a ColliderTris's hit effects
 */
void CollisionCheck_SetTrisHitFX(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderTris* tris = (ColliderTris*)col;
    ColliderTrisElement* trisElem;

    for (trisElem = tris->elements; trisElem < &tris->elements[tris->count]; trisElem++) {
        if ((trisElem->base.acElemFlags & ACELEM_DRAW_HITMARK) && (trisElem->base.acHitElem != NULL) &&
            !(trisElem->base.acHitElem->atElemFlags & ATELEM_DREW_HITMARK)) {
            Vec3f hitPos;

            Math_Vec3s_ToVec3f(&hitPos, &trisElem->base.acDmgInfo.hitPos);
            CollisionCheck_HitEffects(play, trisElem->base.acHit, trisElem->base.acHitElem, &tris->base,
                                      &trisElem->base, &hitPos);
            trisElem->base.acHitElem->atElemFlags |= ATELEM_DREW_HITMARK;
            return;
        }
    }
}

/**
 * Sets a ColliderQuad's hit effects
 */
void CollisionCheck_SetQuadHitFX(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderQuad* quad = (ColliderQuad*)col;

    if ((quad->elem.acElemFlags & ACELEM_DRAW_HITMARK) && (quad->elem.acHitElem != NULL) &&
        !(quad->elem.acHitElem->atElemFlags & ATELEM_DREW_HITMARK)) {
        Vec3f hitPos;

        Math_Vec3s_ToVec3f(&hitPos, &quad->elem.acDmgInfo.hitPos);
        CollisionCheck_HitEffects(play, quad->elem.acHit, quad->elem.acHitElem, &quad->base, &quad->elem, &hitPos);
        quad->elem.acHitElem->atElemFlags |= ATELEM_DREW_HITMARK;
    }
}

/**
 * Sets a ColliderSphere's hit effects
 */
void CollisionCheck_SetSphereHitFX(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderSphere* sph = (ColliderSphere*)col;

    if ((sph->elem.acElemFlags & ACELEM_DRAW_HITMARK) && (sph->elem.acHitElem != NULL) &&
        !(sph->elem.acHitElem->atElemFlags & ATELEM_DREW_HITMARK)) {
        Vec3f hitPos;

        Math_Vec3s_ToVec3f(&hitPos, &sph->elem.acDmgInfo.hitPos);
        CollisionCheck_HitEffects(play, sph->elem.acHit, sph->elem.acHitElem, &sph->base, &sph->elem, &hitPos);
        sph->elem.acHitElem->atElemFlags |= ATELEM_DREW_HITMARK;
    }
}

ColChkApplyFunc sColChkApplyFuncs[COLSHAPE_MAX] = {
    CollisionCheck_SetJntSphHitFX, // COLSHAPE_JNTSPH
    CollisionCheck_SetCylHitFX,    // COLSHAPE_CYLINDER
    CollisionCheck_SetTrisHitFX,   // COLSHAPE_TRIS
    CollisionCheck_SetQuadHitFX,   // COLSHAPE_QUAD
    CollisionCheck_SetSphereHitFX, // COLSHAPE_SPHERE
};

/**
 * Handles hit effects for each AC collider that had an AC collision. Spawns hitmarks and plays sound effects.
 */
void CollisionCheck_SetHitEffects(struct PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** acColP;

    for (acColP = &colChkCtx->colAC[0]; acColP < &colChkCtx->colAC[colChkCtx->colACCount]; acColP++) {
        Collider* acCol = *acColP;

        if ((acCol != NULL) && (acCol->acFlags & AC_ON)) {
            if ((acCol->actor != NULL) && (acCol->actor->update == NULL)) {
                continue;
            }
            sColChkApplyFuncs[acCol->shape](play, colChkCtx, acCol);
        }
    }
}

ColChkVsFunc sACVsFuncs[COLSHAPE_MAX][COLSHAPE_MAX] = {
    // COLSHAPE_JNTSPH
    {
        CollisionCheck_AC_JntSphVsJntSph, // COLSHAPE_JNTSPH
        CollisionCheck_AC_JntSphVsCyl,    // COLSHAPE_CYLINDER
        CollisionCheck_AC_JntSphVsTris,   // COLSHAPE_TRIS
        CollisionCheck_AC_JntSphVsQuad,   // COLSHAPE_QUAD
        CollisionCheck_AC_JntSphVsSphere  // COLSHAPE_SPHERE
    },
    // COLSHAPE_CYLINDER
    {
        CollisionCheck_AC_CylVsJntSph, // COLSHAPE_JNTSPH
        CollisionCheck_AC_CylVsCyl,    // COLSHAPE_CYLINDER
        CollisionCheck_AC_CylVsTris,   // COLSHAPE_TRIS
        CollisionCheck_AC_CylVsQuad,   // COLSHAPE_QUAD
        CollisionCheck_AC_CylVsSphere  // COLSHAPE_SPHERE
    },
    // COLSHAPE_TRIS
    {
        CollisionCheck_AC_TrisVsJntSph, // COLSHAPE_JNTSPH
        CollisionCheck_AC_TrisVsCyl,    // COLSHAPE_CYLINDER
        CollisionCheck_AC_TrisVsTris,   // COLSHAPE_TRIS
        CollisionCheck_AC_TrisVsQuad,   // COLSHAPE_QUAD
        CollisionCheck_AC_TrisVsSphere  // COLSHAPE_SPHERE
    },
    // COLSHAPE_QUAD
    {
        CollisionCheck_AC_QuadVsJntSph, // COLSHAPE_JNTSPH
        CollisionCheck_AC_QuadVsCyl,    // COLSHAPE_CYLINDER
        CollisionCheck_AC_QuadVsTris,   // COLSHAPE_TRIS
        CollisionCheck_AC_QuadVsQuad,   // COLSHAPE_QUAD
        CollisionCheck_AC_QuadVsSphere  // COLSHAPE_SPHERE
    },
    // COLSHAPE_SPHERE
    {
        CollisionCheck_AC_SphereVsJntSph,   // COLSHAPE_JNTSPH
        CollisionCheck_AC_SphereVsCylinder, // COLSHAPE_CYLINDER
        CollisionCheck_AC_SphereVsTris,     // COLSHAPE_TRIS
        CollisionCheck_AC_SphereVsQuad,     // COLSHAPE_QUAD
        CollisionCheck_AC_SphereVsSphere    // COLSHAPE_SPHERE
    },
};

/**
 * Iterates through all AC colliders, performing AC collisions with the AT collider.
 */
void CollisionCheck_AC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol) {
    Collider** acColP;

    for (acColP = &colChkCtx->colAC[0]; acColP < &colChkCtx->colAC[colChkCtx->colACCount]; acColP++) {
        Collider* acCol = *acColP;

        if ((acCol != NULL) && (acCol->acFlags & AC_ON)) {
            if ((acCol->actor != NULL) && (acCol->actor->update == NULL)) {
                continue;
            }
            if ((acCol->acFlags & atCol->atFlags & AC_TYPE_ALL) && (atCol != acCol)) {
                if (!(atCol->atFlags & AT_SELF) && (atCol->actor != NULL) && (acCol->actor == atCol->actor)) {
                    continue;
                }
                sACVsFuncs[atCol->shape][acCol->shape](play, colChkCtx, atCol, acCol);
            }
        }
    }
}

/**
 * Iterates through all AT colliders, testing them for AC collisions with each AC collider, setting the info regarding
 * the collision for each AC and AT collider that collided. Then spawns hitmarks and plays sound effects for each
 * successful collision. To collide, an AT collider must share a type (AC_TYPE_PLAYER, AC_TYPE_ENEMY, or AC_TYPE_OTHER)
 * with the AC collider and the atDmgInfo and acDmgInfo elements that overlapped must share a dmgFlag.
 */
void CollisionCheck_AT(struct PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** acColP;

    if ((colChkCtx->colATCount == 0) || (colChkCtx->colACCount == 0)) {
        return;
    }

    for (acColP = &colChkCtx->colAT[0]; acColP < &colChkCtx->colAT[colChkCtx->colATCount]; acColP++) {
        Collider* acCol = *acColP;

        if ((acCol != NULL) && (acCol->atFlags & AT_ON)) {
            if ((acCol->actor != NULL) && (acCol->actor->update == NULL)) {
                continue;
            }
            CollisionCheck_AC(play, colChkCtx, acCol);
        }
    }

    CollisionCheck_SetHitEffects(play, colChkCtx);
}

/**
 * Get mass type. Immobile colliders cannot be pushed, while heavy colliders can only be pushed by heavy and immobile
 * colliders.
 */
s32 CollisionCheck_GetMassType(u8 mass) {
    if (mass == MASS_IMMOVABLE) {
        return MASSTYPE_IMMOVABLE;
    }
    if (mass == MASS_HEAVY) {
        return MASSTYPE_HEAVY;
    }
    return MASSTYPE_NORMAL;
}

/**
 * Sets OC collision flags for OC collider overlaps. If both colliders are attached to actors and can push,
 * also performs an elastic collision where both colliders are moved apart in proportion to their masses.
 */
void CollisionCheck_SetOCvsOC(struct PlayState* play, Collider* leftCol, ColliderElement* leftElem, Vec3f* leftPos,
                              Collider* rightCol, ColliderElement* rightElem, Vec3f* rightPos, f32 overlapSize) {
    f32 pad;
    f32 leftDispRatio;
    f32 rightDispRatio;
    f32 xzDist;
    f32 leftMass;
    f32 rightMass;
    f32 totalMass;
    f32 inverseTotalMass;
    f32 xDelta;
    f32 zDelta;
    Actor* leftActor = leftCol->actor;
    Actor* rightActor = rightCol->actor;
    s32 leftMassType;
    s32 rightMassType;

    leftCol->ocFlags1 |= OC1_HIT;
    leftCol->oc = rightActor;
    leftElem->ocElemFlags |= OCELEM_HIT;
    if (rightCol->ocFlags2 & OC2_TYPE_PLAYER) {
        leftCol->ocFlags2 |= OC2_HIT_PLAYER;
    }

    rightCol->ocFlags1 |= OC1_HIT;
    rightCol->oc = leftActor;
    rightElem->ocElemFlags |= OCELEM_HIT;
    if (leftCol->ocFlags2 & OC2_TYPE_PLAYER) {
        rightCol->ocFlags2 |= OC2_HIT_PLAYER;
    }

    if ((leftActor == NULL) || (rightActor == NULL) || (leftCol->ocFlags1 & OC1_NO_PUSH) ||
        (rightCol->ocFlags1 & OC1_NO_PUSH)) {
        return;
    }

    rightMassType = CollisionCheck_GetMassType(leftActor->colChkInfo.mass);
    leftMassType = CollisionCheck_GetMassType(rightActor->colChkInfo.mass);
    leftMass = leftActor->colChkInfo.mass;
    rightMass = rightActor->colChkInfo.mass;
    totalMass = leftMass + rightMass;

    if (IS_ZERO(totalMass)) {
        leftMass = rightMass = 1.0f;
        totalMass = 2.0f;
    }
    xDelta = rightPos->x - leftPos->x;
    zDelta = rightPos->z - leftPos->z;

    xzDist = sqrtf(SQ(xDelta) + SQ(zDelta));

    if (rightMassType == MASSTYPE_IMMOVABLE) {
        if (leftMassType == MASSTYPE_IMMOVABLE) {
            return;
        } else {
            leftDispRatio = 0;
            rightDispRatio = 1;
        }
    } else if (rightMassType == MASSTYPE_HEAVY) {
        if (leftMassType == MASSTYPE_IMMOVABLE) {
            leftDispRatio = 1;
            rightDispRatio = 0;
        } else if (leftMassType == MASSTYPE_HEAVY) {
            leftDispRatio = 0.5f;
            rightDispRatio = 0.5f;
        } else {
            leftDispRatio = 0;
            rightDispRatio = 1;
        }
    } else {
        if (leftMassType == MASSTYPE_NORMAL) {
            inverseTotalMass = 1.0f / totalMass;
            leftDispRatio = rightMass * inverseTotalMass;
            rightDispRatio = leftMass * inverseTotalMass;
        } else {
            leftDispRatio = 1;
            rightDispRatio = 0;
        }
    }

    if (!IS_ZERO(xzDist)) {
        xDelta *= overlapSize / xzDist;
        zDelta *= overlapSize / xzDist;
        leftActor->colChkInfo.displacement.x += -xDelta * leftDispRatio;
        leftActor->colChkInfo.displacement.z += -zDelta * leftDispRatio;
        rightActor->colChkInfo.displacement.x += xDelta * rightDispRatio;
        rightActor->colChkInfo.displacement.z += zDelta * rightDispRatio;
    } else if (overlapSize != 0.0f) {
        leftActor->colChkInfo.displacement.x += -overlapSize * leftDispRatio;
        rightActor->colChkInfo.displacement.x += overlapSize * rightDispRatio;
    } else {
        leftActor->colChkInfo.displacement.x += -leftDispRatio;
        rightActor->colChkInfo.displacement.x += rightDispRatio;
    }
}

/**
 * OC overlap check for two JntSphs
 */
void CollisionCheck_OC_JntSphVsJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                      Collider* rightCol) {
    ColliderJntSph* leftJntSph = (ColliderJntSph*)leftCol;
    ColliderJntSph* rightJntSph = (ColliderJntSph*)rightCol;
    ColliderJntSphElement* leftJntSphElem;
    ColliderJntSphElement* rightJntSphElem;
    f32 overlapSize;

    if ((leftJntSph->count > 0) && (leftJntSph->elements != NULL) && (rightJntSph->count > 0) &&
        (rightJntSph->elements != NULL) && (leftJntSph->base.ocFlags1 & OCELEM_ON) &&
        (rightJntSph->base.ocFlags1 & OCELEM_ON)) {

        for (leftJntSphElem = leftJntSph->elements; leftJntSphElem < &leftJntSph->elements[leftJntSph->count];
             leftJntSphElem++) {
            if (!(leftJntSphElem->base.ocElemFlags & OCELEM_ON)) {
                continue;
            }
            for (rightJntSphElem = rightJntSph->elements; rightJntSphElem < &rightJntSph->elements[rightJntSph->count];
                 rightJntSphElem++) {
                if (!(rightJntSphElem->base.ocElemFlags & OCELEM_ON)) {
                    continue;
                }
                if (Math3D_SphVsSphOverlap(&leftJntSphElem->dim.worldSphere, &rightJntSphElem->dim.worldSphere,
                                           &overlapSize)) {
                    Vec3f leftPos;
                    Vec3f rightPos;

                    Math_Vec3s_ToVec3f(&leftPos, &leftJntSphElem->dim.worldSphere.center);
                    Math_Vec3s_ToVec3f(&rightPos, &rightJntSphElem->dim.worldSphere.center);
                    CollisionCheck_SetOCvsOC(play, &leftJntSph->base, &leftJntSphElem->base, &leftPos,
                                             &rightJntSph->base, &rightJntSphElem->base, &rightPos, overlapSize);
                }
            }
        }
    }
}

/**
 * OC overlap check for a JntSph and Cylinder
 */
void CollisionCheck_OC_JntSphVsCyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                   Collider* rightCol) {
    ColliderJntSph* leftJntSph = (ColliderJntSph*)leftCol;
    ColliderCylinder* rightCyl = (ColliderCylinder*)rightCol;
    ColliderJntSphElement* leftJntSphElem;
    f32 overlapSize;

    if ((leftJntSph->count > 0) && (leftJntSph->elements != NULL) && (leftJntSph->base.ocFlags1 & OCELEM_ON) &&
        (rightCyl->base.ocFlags1 & OCELEM_ON) && (rightCyl->elem.ocElemFlags & OCELEM_ON)) {

        for (leftJntSphElem = leftJntSph->elements; leftJntSphElem < &leftJntSph->elements[leftJntSph->count];
             leftJntSphElem++) {
            if (!(leftJntSphElem->base.ocElemFlags & OCELEM_ON)) {
                continue;
            }
            if (Math3D_SphVsCylOverlap(&leftJntSphElem->dim.worldSphere, &rightCyl->dim, &overlapSize)) {
                Vec3f leftPos;
                Vec3f rightPos;

                Math_Vec3s_ToVec3f(&leftPos, &leftJntSphElem->dim.worldSphere.center);
                Math_Vec3s_ToVec3f(&rightPos, &rightCyl->dim.pos);
                CollisionCheck_SetOCvsOC(play, &leftJntSph->base, &leftJntSphElem->base, &leftPos, &rightCyl->base,
                                         &rightCyl->elem, &rightPos, overlapSize);
            }
        }
    }
}

/**
 * OC overlap check for a JntSph and Sphere
 */
void CollisionCheck_OC_JntSphVsSphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                      Collider* rightCol) {
    ColliderJntSph* leftJntSph = (ColliderJntSph*)leftCol;
    ColliderSphere* rightSph = (ColliderSphere*)rightCol;
    ColliderJntSphElement* leftJntSphElem;
    f32 overlapSize;

    if ((leftJntSph->count > 0) && (leftJntSph->elements != NULL) && (leftJntSph->base.ocFlags1 & OCELEM_ON) &&
        (rightSph->base.ocFlags1 & OCELEM_ON) && (rightSph->elem.ocElemFlags & OCELEM_ON)) {

        for (leftJntSphElem = leftJntSph->elements; leftJntSphElem < &leftJntSph->elements[leftJntSph->count];
             leftJntSphElem++) {
            if (!(leftJntSphElem->base.ocElemFlags & OCELEM_ON)) {
                continue;
            }
            if (Math3D_SphVsSphOverlap(&leftJntSphElem->dim.worldSphere, &rightSph->dim.worldSphere, &overlapSize)) {
                Vec3f leftPos;
                Vec3f rightPos;

                Math_Vec3s_ToVec3f(&leftPos, &leftJntSphElem->dim.worldSphere.center);
                Math_Vec3s_ToVec3f(&rightPos, &rightSph->dim.worldSphere.center);
                CollisionCheck_SetOCvsOC(play, &leftJntSph->base, &leftJntSphElem->base, &leftPos, &rightSph->base,
                                         &rightSph->elem, &rightPos, overlapSize);
            }
        }
    }
}

/**
 * OC overlap check for a Cylinder and JntSph
 */
void CollisionCheck_OC_CylVsJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                   Collider* rightCol) {
    CollisionCheck_OC_JntSphVsCyl(play, colChkCtx, rightCol, leftCol);
}

/**
 * OC overlap check for two Cylinders
 */
void CollisionCheck_OC_CylVsCyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                Collider* rightCol) {
    ColliderCylinder* leftCyl = (ColliderCylinder*)leftCol;
    ColliderCylinder* rightCyl = (ColliderCylinder*)rightCol;
    f32 overlapSize;

    if ((leftCyl->base.ocFlags1 & OCELEM_ON) && (rightCyl->base.ocFlags1 & OCELEM_ON) &&
        (leftCyl->elem.ocElemFlags & OCELEM_ON) && (rightCyl->elem.ocElemFlags & OCELEM_ON)) {
        if (Math3D_CylVsCylOverlap(&leftCyl->dim, &rightCyl->dim, &overlapSize)) {
            Vec3f leftPos;
            Vec3f rightPos;

            Math_Vec3s_ToVec3f(&leftPos, &leftCyl->dim.pos);
            Math_Vec3s_ToVec3f(&rightPos, &rightCyl->dim.pos);
            CollisionCheck_SetOCvsOC(play, &leftCyl->base, &leftCyl->elem, &leftPos, &rightCyl->base, &rightCyl->elem,
                                     &rightPos, overlapSize);
        }
    }
}

/**
 * OC overlap check for a Cylinder and Sphere
 */
void CollisionCheck_OC_CylVsSphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                   Collider* rightCol) {
    ColliderCylinder* leftCyl = (ColliderCylinder*)leftCol;
    ColliderSphere* rightSph = (ColliderSphere*)rightCol;
    f32 overlapSize;

    if ((leftCyl->base.ocFlags1 & OCELEM_ON) && (leftCyl->elem.ocElemFlags & OCELEM_ON) &&
        (rightSph->base.ocFlags1 & OCELEM_ON) && (rightSph->elem.ocElemFlags & OCELEM_ON)) {
        if (Math3D_SphVsCylOverlap(&rightSph->dim.worldSphere, &leftCyl->dim, &overlapSize)) {
            Vec3f leftPos;
            Vec3f rightPos;

            Math_Vec3s_ToVec3f(&leftPos, &leftCyl->dim.pos);
            Math_Vec3s_ToVec3f(&rightPos, &rightSph->dim.worldSphere.center);
            CollisionCheck_SetOCvsOC(play, &leftCyl->base, &leftCyl->elem, &leftPos, &rightSph->base, &rightSph->elem,
                                     &rightPos, overlapSize);
        }
    }
}

/**
 * OC overlap check for a Sphere and JntSph
 */
void CollisionCheck_OC_SphereVsJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                      Collider* rightCol) {
    CollisionCheck_OC_JntSphVsSphere(play, colChkCtx, rightCol, leftCol);
}

/**
 * OC overlap check for a Sphere and Cylinder
 */
void CollisionCheck_OC_SphereVsCyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                   Collider* rightCol) {
    CollisionCheck_OC_CylVsSphere(play, colChkCtx, rightCol, leftCol);
}

/**
 * OC overlap check for two Spheres
 */
void CollisionCheck_OC_SphereVsSphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                      Collider* rightCol) {
    ColliderSphere* leftSph = (ColliderSphere*)leftCol;
    ColliderSphere* rightSph = (ColliderSphere*)rightCol;
    f32 overlapSize;

    if ((leftSph->base.ocFlags1 & OCELEM_ON) && (leftSph->elem.ocElemFlags & OCELEM_ON) &&
        (rightSph->base.ocFlags1 & OCELEM_ON) && (rightSph->elem.ocElemFlags & OCELEM_ON)) {
        if (Math3D_SphVsSphOverlap(&leftSph->dim.worldSphere, &rightSph->dim.worldSphere, &overlapSize)) {
            Vec3f leftPos;
            Vec3f rightPos;

            Math_Vec3s_ToVec3f(&leftPos, &leftSph->dim.worldSphere.center);
            Math_Vec3s_ToVec3f(&rightPos, &rightSph->dim.worldSphere.center);
            CollisionCheck_SetOCvsOC(play, &leftSph->base, &leftSph->elem, &leftPos, &rightSph->base, &rightSph->elem,
                                     &rightPos, overlapSize);
        }
    }
}

/**
 *  Skip any OC colliders that are off
 */
s32 CollisionCheck_SkipOC(Collider* col) {
    if (!(col->ocFlags1 & OCELEM_ON)) {
        return 1;
    }
    return 0;
}

/**
 * Checks for OC compatibility. There are three conditions:
 * First, each collider must have an OC flag corresponding to the other's OC type.
 * Second, OC2_UNK1 and OC2_UNK2 can't collide with each other (has something to do with horses?)
 * Third, the colliders can't collide if they belong to the same actor
 */
s32 CollisionCheck_Incompatible(Collider* leftCol, Collider* rightCol) {
    if (!(leftCol->ocFlags1 & rightCol->ocFlags2 & OC1_TYPE_ALL) ||
        !(leftCol->ocFlags2 & rightCol->ocFlags1 & OC1_TYPE_ALL) ||
        ((leftCol->ocFlags2 & OC2_UNK1) && (rightCol->ocFlags2 & OC2_UNK2)) ||
        ((rightCol->ocFlags2 & OC2_UNK1) && (leftCol->ocFlags2 & OC2_UNK2))) {
        return 1;
    }
    if (leftCol->actor == rightCol->actor) {
        return 1;
    }
    return 0;
}

ColChkVsFunc sOCVsFuncs[COLSHAPE_MAX][COLSHAPE_MAX] = {
    // COLSHAPE_JNTSPH
    {
        CollisionCheck_OC_JntSphVsJntSph, // COLSHAPE_JNTSPH
        CollisionCheck_OC_JntSphVsCyl,    // COLSHAPE_CYLINDER
        NULL,                             // COLSHAPE_TRIS
        NULL,                             // COLSHAPE_QUAD
        CollisionCheck_OC_JntSphVsSphere  // COLSHAPE_SPHERE
    },
    // COLSHAPE_CYLINDER
    {
        CollisionCheck_OC_CylVsJntSph, // COLSHAPE_JNTSPH
        CollisionCheck_OC_CylVsCyl,    // COLSHAPE_CYLINDER
        NULL,                          // COLSHAPE_TRIS
        NULL,                          // COLSHAPE_QUAD
        CollisionCheck_OC_CylVsSphere  // COLSHAPE_SPHERE
    },
    // COLSHAPE_TRIS
    {
        NULL, // COLSHAPE_JNTSPH
        NULL, // COLSHAPE_CYLINDER
        NULL, // COLSHAPE_TRIS
        NULL, // COLSHAPE_QUAD
        NULL  // COLSHAPE_SPHERE
    },
    // COLSHAPE_QUAD
    {
        NULL, // COLSHAPE_JNTSPH
        NULL, // COLSHAPE_CYLINDER
        NULL, // COLSHAPE_TRIS
        NULL, // COLSHAPE_QUAD
        NULL  // COLSHAPE_SPHERE
    },
    // COLSHAPE_SPHERE
    {
        CollisionCheck_OC_SphereVsJntSph, // COLSHAPE_JNTSPH
        CollisionCheck_OC_SphereVsCyl,    // COLSHAPE_CYLINDER
        NULL,                             // COLSHAPE_TRIS
        NULL,                             // COLSHAPE_QUAD
        CollisionCheck_OC_SphereVsSphere  // COLSHAPE_SPHERE
    },
};

/**
 * Iterates through all OC colliders and collides them with all subsequent OC colliders on the list. During an OC
 * collision, colliders with overlapping elements move away from each other so that their elements no longer overlap.
 * The relative amount each collider is pushed is determined by the collider's mass. Only JntSph, Cylinder and Sphere
 * colliders can collide, and each collider must have the OC flag corresponding to the other's OC type. Additionally,
 * OC2_UNK1 cannot collide with OC2_UNK2, nor can two colliders that share an actor.
 */
void CollisionCheck_OC(struct PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** leftColP;
    Collider** rightColP;
    ColChkVsFunc vsFunc;

    for (leftColP = colChkCtx->colOC; leftColP < colChkCtx->colOC + colChkCtx->colOCCount; leftColP++) {
        if ((*leftColP == NULL) || CollisionCheck_SkipOC(*leftColP)) {
            continue;
        }
        for (rightColP = leftColP + 1; rightColP < colChkCtx->colOC + colChkCtx->colOCCount; rightColP++) {
            if ((*rightColP == NULL) || CollisionCheck_SkipOC(*rightColP) ||
                CollisionCheck_Incompatible(*leftColP, *rightColP)) {
                continue;
            }
            vsFunc = sOCVsFuncs[(*leftColP)->shape][(*rightColP)->shape];
            if (vsFunc == NULL) {
                continue;
            }
            vsFunc(play, colChkCtx, *leftColP, *rightColP);
        }
    }
}

/**
 * Initializes CollisionCheckInfo to default values
 */
void CollisionCheck_InitInfo(CollisionCheckInfo* info) {
    static CollisionCheckInfo sDefaultColChkInfo = {
        NULL, { 0.0f, 0.0f, 0.0f }, 10, 10, 0, MASS_IMMOVABLE, 8, 0, 0, 0, 0,
    };

    *info = sDefaultColChkInfo;
}

/**
 * Resets ColisionCheckInfo fields other than DamageTable, mass, and dim.
 */
void CollisionCheck_ResetDamage(CollisionCheckInfo* info) {
    info->damage = 0;
    info->damageEffect = 0;
    info->atHitEffect = 0;
    info->acHitEffect = 0;
    info->displacement.x = info->displacement.y = info->displacement.z = 0.0f;
}

/**
 * Sets up CollisionCheckInfo using the values in init. Does not set a damage table or the unused unk_14.
 */
void CollisionCheck_SetInfoNoDamageTable(CollisionCheckInfo* info, CollisionCheckInfoInit* init) {
    info->health = init->health;
    info->cylRadius = init->cylRadius;
    info->cylHeight = init->cylHeight;
    info->mass = init->mass;
}

/**
 * Sets up CollisionCheckInfo using the values in init. Does not set the unused unk_14
 */
void CollisionCheck_SetInfo(CollisionCheckInfo* info, DamageTable* damageTable, CollisionCheckInfoInit* init) {
    info->health = init->health;
    info->damageTable = damageTable;
    info->cylRadius = init->cylRadius;
    info->cylHeight = init->cylHeight;
    info->mass = init->mass;
}

/**
 * Sets up CollisionCheckInfo using the values in init. Sets the unused unk_14
 */
void CollisionCheck_SetInfo2(CollisionCheckInfo* info, DamageTable* damageTable, CollisionCheckInfoInit2* init) {
    info->health = init->health;
    info->damageTable = damageTable;
    info->cylRadius = init->cylRadius;
    info->cylHeight = init->cylHeight;
    info->cylYShift = init->cylYShift;
    info->mass = init->mass;
}

/**
 * Sets up CollisionCheckInfo using the values in Init and a preset damage table. Sets the unused unk_14.
 */
void CollisionCheck_SetInfoGetDamageTable(CollisionCheckInfo* info, s32 index, CollisionCheckInfoInit2* init) {
    CollisionCheck_SetInfo2(info, DamageTable_Get(index), init);
}

/**
 * Apply AC damage effect
 */
void CollisionCheck_ApplyDamage(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col,
                                ColliderElement* elem) {
    f32 damage;
    f32 finalDamage = 0.0f;
    s32 pad;
    Collider* atCol;
    ColliderElement* atElem;
    s32 pad1;
    u32 effect;

    if ((col->actor == NULL) || !(col->acFlags & AC_HIT)) {
        return;
    }
    if (!(elem->acElemFlags & ACELEM_HIT) || (elem->acElemFlags & ACELEM_NO_DAMAGE)) {
        return;
    }

    atCol = elem->acHit;
    atElem = elem->acHitElem;

    if ((atCol != NULL) && (atElem != NULL) && (col != NULL) && (elem != NULL)) {
        damage = CollisionCheck_GetDamageAndEffectOnElementAC(atCol, atElem, col, elem, &effect);

        if (CollisionCheck_GetElementATDamage(atCol, atElem, col, elem) != 0) {
            if (damage < 1.0f) {
                finalDamage = 0.0f;
                if (effect == 0) {
                    return;
                }
            } else {
                finalDamage = CollisionCheck_ApplyElementATDefense(damage, elem);
                if ((finalDamage < 1.0f) && (effect == 0)) {
                    return;
                }
            }
        }
        if (col->actor->colChkInfo.damageTable != NULL) {
            col->actor->colChkInfo.damageEffect = effect;
        }
        if (!(col->acFlags & AC_HARD) || ((col->acFlags & AC_HARD) && (atElem->atDmgInfo.dmgFlags == 0x20000000))) {
            if (col->actor->colChkInfo.damage < finalDamage) {
                col->actor->colChkInfo.damage = finalDamage;
            }
        }
    }
}

/**
 * Apply ColliderJntSph AC damage effect
 */
void CollisionCheck_ApplyDamageJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderJntSph* jntSph = (ColliderJntSph*)col;
    s32 i;

    if ((jntSph->count > 0) && (jntSph->elements != NULL)) {
        for (i = 0; i < jntSph->count; i++) {
            CollisionCheck_ApplyDamage(play, colChkCtx, &jntSph->base, &jntSph->elements[i].base);
        }
    }
}

/**
 * Apply ColliderCylinder AC damage effect
 */
void CollisionCheck_ApplyDamageCyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    CollisionCheck_ApplyDamage(play, colChkCtx, &cyl->base, &cyl->elem);
}

/**
 * Apply ColliderTris AC damage effect
 */
void CollisionCheck_ApplyDamageTris(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderTris* tris = (ColliderTris*)col;
    s32 i;

    // unlike sphere groups above, tri groups are not guarded against
    //  tris->elements being NULL
    for (i = 0; i < tris->count; i++) {
        CollisionCheck_ApplyDamage(play, colChkCtx, &tris->base, &tris->elements[i].base);
    }
}

/**
 * Apply ColliderQuad AC damage effect
 */
void CollisionCheck_ApplyDamageQuad(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderQuad* quad = (ColliderQuad*)col;

    CollisionCheck_ApplyDamage(play, colChkCtx, &quad->base, &quad->elem);
}

/**
 * Apply ColliderSphere AC damage effect
 */
void CollisionCheck_ApplyDamageSphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderSphere* sph = (ColliderSphere*)col;

    CollisionCheck_ApplyDamage(play, colChkCtx, &sph->base, &sph->elem);
}

ColChkApplyFunc sApplyDamageFuncs[COLSHAPE_MAX] = {
    CollisionCheck_ApplyDamageJntSph, // COLSHAPE_JNTSPH
    CollisionCheck_ApplyDamageCyl,    // COLSHAPE_CYLINDER
    CollisionCheck_ApplyDamageTris,   // COLSHAPE_TRIS
    CollisionCheck_ApplyDamageQuad,   // COLSHAPE_QUAD
    CollisionCheck_ApplyDamageSphere, // COLSHAPE_SPHERE
};

/**
 * For all AC colliders, sets any damage effects from collisions with AT colliders to their corresponding actor's
 * CollisionCheckInfo.
 */
void CollisionCheck_Damage(struct PlayState* play, CollisionCheckContext* colChkCtx) {
    s32 i;

    for (i = 0; i < colChkCtx->colACCount; i++) {
        Collider* col = colChkCtx->colAC[i];

        if (col == NULL) {
            continue;
        }
        if (col->acFlags & AC_NO_DAMAGE) {
            continue;
        }
        sApplyDamageFuncs[col->shape](play, colChkCtx, col);
    }
}

/**
 * Checks if the line segment ab intersects any of the ColliderJntSph's elements
 */
s32 CollisionCheck_LineOC_JntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, Vec3f* a,
                                 Vec3f* b) {
    ColliderJntSph* jntSph = (ColliderJntSph*)col;
    s32 i;

    for (i = 0; i < jntSph->count; i++) {
        ColliderJntSphElement* jntSphElem = &jntSph->elements[i];

        if (!(jntSphElem->base.ocElemFlags & OCELEM_ON)) {
            continue;
        }

        D_801EDEB0.a = *a;
        D_801EDEB0.b = *b;
        if (Math3D_LineVsSph(&jntSphElem->dim.worldSphere, &D_801EDEB0)) {
            return true;
        }
    }
    return false;
}

/**
 * Checks if the line segment ab intersects the ColliderCylinder
 */
s32 CollisionCheck_LineOC_Cyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, Vec3f* a,
                              Vec3f* b) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    if (!(cyl->elem.ocElemFlags & OCELEM_ON)) {
        return false;
    }

    if (Math3D_CylVsLineSeg(&cyl->dim, a, b, &D_801EDF38, &D_801EDF48)) {
        return true;
    }

    return false;
}

/**
 * Checks if the line segment ab intersects the ColliderSphere
 */
s32 CollisionCheck_LineOC_Sphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, Vec3f* a,
                                 Vec3f* b) {
    ColliderSphere* sph = (ColliderSphere*)col;

    if (!(sph->elem.ocElemFlags & OCELEM_ON)) {
        return false;
    }

    D_801EDFC8.a = *a;
    D_801EDFC8.b = *b;
    if (Math3D_LineVsSph(&sph->dim.worldSphere, &D_801EDFC8)) {
        return true;
    }

    return false;
}

static ColChkLineFunc sOCLineCheckFuncs[COLSHAPE_MAX] = {
    CollisionCheck_LineOC_JntSph, // COLSHAPE_JNTSPH
    CollisionCheck_LineOC_Cyl,    // COLSHAPE_CYLINDER
    NULL,                         // COLSHAPE_TRIS
    NULL,                         // COLSHAPE_QUAD
    CollisionCheck_LineOC_Sphere, // COLSHAPE_SPHERE
};

/**
 * Checks if the line segment ab intersects any OC colliders, excluding those attached to actors
 * on the exclusion list. Returns true if there are any intersections and false otherwise.
 */
s32 CollisionCheck_LineOC(struct PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b,
                          Actor** exclusions, s32 numExclusions) {
    ColChkLineFunc lineCheck;
    Collider** colP;
    s32 i;
    s32 exclude;
    s32 result = false;

    for (colP = colChkCtx->colOC; colP < &colChkCtx->colOC[colChkCtx->colOCCount]; colP++) {
        if (CollisionCheck_SkipOC(*colP)) {
            continue;
        }

        exclude = false;
        for (i = 0; i < numExclusions; i++) {
            if ((*colP)->actor == exclusions[i]) {
                exclude = 1;
                break;
            }
        }
        if (exclude) {
            continue;
        }

        lineCheck = sOCLineCheckFuncs[(*colP)->shape];
        if (lineCheck == NULL) {
            continue;
        }

        result = lineCheck(play, colChkCtx, *colP, a, b);
        if (result) {
            break;
        }
    }
    return result;
}

/**
 * Checks if the line segment ab intersects any OC colliders. Returns true if there are any intersections and false
 * otherwise.
 */
s32 CollisionCheck_LineOCCheckAll(struct PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b) {
    return CollisionCheck_LineOC(play, colChkCtx, a, b, NULL, 0);
}

/**
 * Checks if the line segment ab intersects any OC colliders, excluding those attached to actors on the exclusion list.
 * Returns true if there are any intersections and false otherwise.
 */
s32 CollisionCheck_LineOCCheck(struct PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b,
                               Actor** exclusions, s32 numExclusions) {
    return CollisionCheck_LineOC(play, colChkCtx, a, b, exclusions, numExclusions);
}

/**
 * Moves the ColliderCylinder's position to the actor's position
 */
void Collider_UpdateCylinder(Actor* actor, ColliderCylinder* cyl) {
    cyl->dim.pos.x = actor->world.pos.x;
    cyl->dim.pos.y = actor->world.pos.y;
    cyl->dim.pos.z = actor->world.pos.z;
}

/**
 * Sets the ColliderCylinder's position
 */
void Collider_SetCylinderPosition(ColliderCylinder* cyl, Vec3s* pos) {
    cyl->dim.pos.x = pos->x;
    cyl->dim.pos.y = pos->y;
    cyl->dim.pos.z = pos->z;
}

/**
 * Sets the ColliderQuad's vertices
 */
void Collider_SetQuadVertices(ColliderQuad* quad, Vec3f* a, Vec3f* b, Vec3f* c, Vec3f* d) {
    Math_Vec3f_Copy(&quad->dim.quad[2], c);
    Math_Vec3f_Copy(&quad->dim.quad[3], d);
    Math_Vec3f_Copy(&quad->dim.quad[0], a);
    Math_Vec3f_Copy(&quad->dim.quad[1], b);
    Collider_SetQuadMidpoints(&quad->dim);
}

/**
 * Sets the specified ColliderTrisElement's vertices
 */
void Collider_SetTrisVertices(ColliderTris* tris, s32 elemIndex, Vec3f* a, Vec3f* b, Vec3f* c) {
    ColliderTrisElement* trisElem = &tris->elements[elemIndex];
    f32 nx;
    f32 ny;
    f32 nz;
    f32 originDist;

    Math_Vec3f_Copy(&trisElem->dim.vtx[0], a);
    Math_Vec3f_Copy(&trisElem->dim.vtx[1], b);
    Math_Vec3f_Copy(&trisElem->dim.vtx[2], c);
    Math3D_DefPlane(a, b, c, &nx, &ny, &nz, &originDist);
    trisElem->dim.plane.normal.x = nx;
    trisElem->dim.plane.normal.y = ny;
    trisElem->dim.plane.normal.z = nz;
    trisElem->dim.plane.originDist = originDist;
}

/**
 * Sets the specified ColliderTrisElement's dim using the values in src
 */
void Collider_SetTrisDim(struct PlayState* play, ColliderTris* dest, s32 elemIndex, ColliderTrisElementDimInit* src) {
    ColliderTrisElement* trisElem = &dest->elements[elemIndex];

    Collider_SetTrisElementDim(play, &trisElem->dim, src);
}

/**
 * Updates the world spheres for all of the collider's JntSph elements attached to the specified limb
 */
void Collider_UpdateSpheres(s32 limb, ColliderJntSph* jntSph) {
    static Vec3f D_801EE1C0;
    static Vec3f D_801EE1D0;
    s32 i;

    for (i = 0; i < jntSph->count; i++) {
        if (limb == jntSph->elements[i].dim.limb) {
            D_801EE1C0.x = jntSph->elements[i].dim.modelSphere.center.x;
            D_801EE1C0.y = jntSph->elements[i].dim.modelSphere.center.y;
            D_801EE1C0.z = jntSph->elements[i].dim.modelSphere.center.z;
            Matrix_MultVec3f(&D_801EE1C0, &D_801EE1D0);
            jntSph->elements[i].dim.worldSphere.center.x = D_801EE1D0.x;
            jntSph->elements[i].dim.worldSphere.center.y = D_801EE1D0.y;
            jntSph->elements[i].dim.worldSphere.center.z = D_801EE1D0.z;
            jntSph->elements[i].dim.worldSphere.radius =
                jntSph->elements[i].dim.modelSphere.radius * jntSph->elements[i].dim.scale;
        }
    }
}

/**
 * Updates the world spheres for the specified ColliderJntSph element
 */
void Collider_UpdateSpheresElement(ColliderJntSph* jntSph, s32 elemIndex, Actor* actor) {
    if (elemIndex < jntSph->count) {
        jntSph->elements[elemIndex].dim.worldSphere.center.x =
            jntSph->elements[elemIndex].dim.modelSphere.center.x + actor->world.pos.x;
        jntSph->elements[elemIndex].dim.worldSphere.center.y =
            jntSph->elements[elemIndex].dim.modelSphere.center.y + actor->world.pos.y;
        jntSph->elements[elemIndex].dim.worldSphere.center.z =
            jntSph->elements[elemIndex].dim.modelSphere.center.z + actor->world.pos.z;
        jntSph->elements[elemIndex].dim.worldSphere.radius =
            jntSph->elements[elemIndex].dim.modelSphere.radius * jntSph->elements[elemIndex].dim.scale;
    }
}

/**
 * Updates the world sphere for the ColliderSphere if it is attached to the specified limb
 */
void Collider_UpdateSphere(s32 limb, ColliderSphere* sph) {
    static Vec3f D_801EE1E0;
    static Vec3f D_801EE1F0;

    if (limb == sph->dim.limb) {
        D_801EE1E0.x = sph->dim.modelSphere.center.x;
        D_801EE1E0.y = sph->dim.modelSphere.center.y;
        D_801EE1E0.z = sph->dim.modelSphere.center.z;
        Matrix_MultVec3f(&D_801EE1E0, &D_801EE1F0);
        sph->dim.worldSphere.center.x = D_801EE1F0.x;
        sph->dim.worldSphere.center.y = D_801EE1F0.y;
        sph->dim.worldSphere.center.z = D_801EE1F0.z;
        sph->dim.worldSphere.radius = sph->dim.modelSphere.radius * sph->dim.scale;
    }
}

/**
 * Spawns red blood droplets.
 * No actor has a collision type that spawns red blood.
 */
void CollisionCheck_SpawnRedBlood(struct PlayState* play, Vec3f* v) {
    static EffectSparkInit D_801EE200;
    s32 effectIndex;

    D_801EE200.position.x = v->x;
    D_801EE200.position.y = v->y;
    D_801EE200.position.z = v->z;
    D_801EE200.uDiv = 5;
    D_801EE200.vDiv = 5;
    D_801EE200.colorStart[0].r = 128;
    D_801EE200.colorStart[0].g = 0;
    D_801EE200.colorStart[0].b = 64;
    D_801EE200.colorStart[0].a = 255;
    D_801EE200.colorStart[1].r = 128;
    D_801EE200.colorStart[1].g = 0;
    D_801EE200.colorStart[1].b = 64;
    D_801EE200.colorStart[1].a = 255;
    D_801EE200.colorStart[2].r = 255;
    D_801EE200.colorStart[2].g = 128;
    D_801EE200.colorStart[2].b = 0;
    D_801EE200.colorStart[2].a = 255;
    D_801EE200.colorStart[3].r = 255;
    D_801EE200.colorStart[3].g = 128;
    D_801EE200.colorStart[3].b = 0;
    D_801EE200.colorStart[3].a = 255;
    D_801EE200.colorEnd[0].r = 64;
    D_801EE200.colorEnd[0].g = 0;
    D_801EE200.colorEnd[0].b = 32;
    D_801EE200.colorEnd[0].a = 0;
    D_801EE200.colorEnd[1].r = 64;
    D_801EE200.colorEnd[1].g = 0;
    D_801EE200.colorEnd[1].b = 32;
    D_801EE200.colorEnd[1].a = 0;
    D_801EE200.colorEnd[2].r = 128;
    D_801EE200.colorEnd[2].g = 0;
    D_801EE200.colorEnd[2].b = 64;
    D_801EE200.colorEnd[2].a = 0;
    D_801EE200.colorEnd[3].r = 128;
    D_801EE200.colorEnd[3].g = 0;
    D_801EE200.colorEnd[3].b = 64;
    D_801EE200.colorEnd[3].a = 0;
    D_801EE200.timer = 0;
    D_801EE200.duration = 16;
    D_801EE200.speed = 8.0f;
    D_801EE200.gravity = -1.0f;

    Effect_Add(play, &effectIndex, EFFECT_SPARK, 0, 1, &D_801EE200);
}

/**
 * Spawns water droplets.
 * No actor has a collision type that spawns water droplets.
 */
void CollisionCheck_SpawnWaterDroplets(struct PlayState* play, Vec3f* v) {
    static EffectSparkInit D_801EE738;
    s32 effectIndex;

    D_801EE738.position.x = v->x;
    D_801EE738.position.y = v->y;
    D_801EE738.position.z = v->z;
    D_801EE738.uDiv = 5;
    D_801EE738.vDiv = 5;
    D_801EE738.colorStart[0].r = 255;
    D_801EE738.colorStart[0].g = 255;
    D_801EE738.colorStart[0].b = 255;
    D_801EE738.colorStart[0].a = 255;
    D_801EE738.colorStart[1].r = 100;
    D_801EE738.colorStart[1].g = 100;
    D_801EE738.colorStart[1].b = 100;
    D_801EE738.colorStart[1].a = 100;
    D_801EE738.colorStart[2].r = 100;
    D_801EE738.colorStart[2].g = 100;
    D_801EE738.colorStart[2].b = 100;
    D_801EE738.colorStart[2].a = 100;
    D_801EE738.colorStart[3].r = 100;
    D_801EE738.colorStart[3].g = 100;
    D_801EE738.colorStart[3].b = 100;
    D_801EE738.colorStart[3].a = 100;
    D_801EE738.colorEnd[0].r = 50;
    D_801EE738.colorEnd[0].g = 50;
    D_801EE738.colorEnd[0].b = 50;
    D_801EE738.colorEnd[0].a = 50;
    D_801EE738.colorEnd[1].r = 50;
    D_801EE738.colorEnd[1].g = 50;
    D_801EE738.colorEnd[1].b = 50;
    D_801EE738.colorEnd[1].a = 50;
    D_801EE738.colorEnd[2].r = 50;
    D_801EE738.colorEnd[2].g = 50;
    D_801EE738.colorEnd[2].b = 50;
    D_801EE738.colorEnd[2].a = 50;
    D_801EE738.colorEnd[3].r = 0;
    D_801EE738.colorEnd[3].g = 0;
    D_801EE738.colorEnd[3].b = 0;
    D_801EE738.colorEnd[3].a = 0;
    D_801EE738.timer = 0;
    D_801EE738.duration = 16;
    D_801EE738.speed = 8.0f;
    D_801EE738.gravity = -1.0f;

    Effect_Add(play, &effectIndex, EFFECT_SPARK, 0, 1, &D_801EE738);
}

/**
 * Spawns streaks of light from hits against solid objects
 */
void CollisionCheck_SpawnShieldParticles(struct PlayState* play, Vec3f* v) {
    static EffectShieldParticleInit sShieldParticleInitMetal = {
        16,
        { 0, 0, 0 },
        { 0, 200, 255, 255 },
        { 255, 255, 255, 255 },
        { 255, 255, 128, 255 },
        { 255, 255, 0, 255 },
        { 255, 64, 0, 200 },
        { 255, 0, 0, 255 },
        2.1f,
        35.0f,
        30.0f,
        8,
        { 0, 0, 0, { 0, 128, 255 }, 0, 300 },
        1,
    };
    s32 effectIndex;

    sShieldParticleInitMetal.position.x = v->x;
    sShieldParticleInitMetal.position.y = v->y;
    sShieldParticleInitMetal.position.z = v->z;
    sShieldParticleInitMetal.lightPoint.x = sShieldParticleInitMetal.position.x;
    sShieldParticleInitMetal.lightPoint.y = sShieldParticleInitMetal.position.y;
    sShieldParticleInitMetal.lightPoint.z = sShieldParticleInitMetal.position.z;

    Effect_Add(play, &effectIndex, EFFECT_SHIELD_PARTICLE, 0, 1, &sShieldParticleInitMetal);
}

/**
 * Spawns streaks of light and makes a metallic sound
 */
void CollisionCheck_SpawnShieldParticlesMetal(struct PlayState* play, Vec3f* v) {
    CollisionCheck_SpawnShieldParticles(play, v);
    Audio_PlaySfx(NA_SE_IT_SHIELD_REFLECT_SW);
}

/**
 * Spawns streaks of light and makes a metallic sound at the specified position
 */
void CollisionCheck_SpawnShieldParticlesMetalSound(struct PlayState* play, Vec3f* v, Vec3f* pos) {
    CollisionCheck_SpawnShieldParticles(play, v);
    Audio_PlaySfx_AtPos(pos, NA_SE_IT_SHIELD_REFLECT_SW);
}

/**
 * Spawns streaks of light and makes a metallic sound
 */
void CollisionCheck_SpawnShieldParticlesMetal2(struct PlayState* play, Vec3f* v) {
    CollisionCheck_SpawnShieldParticlesMetal(play, v);
}

/**
 * Spawns streaks of light and makes a wooden sound
 */
void CollisionCheck_SpawnShieldParticlesWood(struct PlayState* play, Vec3f* v, Vec3f* pos) {
    static EffectShieldParticleInit sShieldParticleInitWood = {
        16,
        { 0, 0, 0 },
        { 0, 200, 255, 255 },
        { 255, 255, 255, 255 },
        { 255, 255, 128, 255 },
        { 255, 255, 0, 255 },
        { 255, 64, 0, 200 },
        { 255, 0, 0, 255 },
        2.1f,
        35.0f,
        30.0f,
        8,
        { 0, 0, 0, { 0, 128, 255 }, 0, 300 },
        0,
    };
    s32 effectIndex;

    sShieldParticleInitWood.position.x = v->x;
    sShieldParticleInitWood.position.y = v->y;
    sShieldParticleInitWood.position.z = v->z;
    sShieldParticleInitWood.lightPoint.x = sShieldParticleInitWood.position.x;
    sShieldParticleInitWood.lightPoint.y = sShieldParticleInitWood.position.y;
    sShieldParticleInitWood.lightPoint.z = sShieldParticleInitWood.position.z;

    Effect_Add(play, &effectIndex, EFFECT_SHIELD_PARTICLE, 0, 1, &sShieldParticleInitWood);
    Audio_PlaySfx_AtPos(pos, NA_SE_IT_REFLECTION_WOOD);
}

/**
 * Determines if the line segment connecting `itemPos` and `itemProjPos` intersects the side of a cylinder with the
 * given `radius`, `height`, and `offset` at `actorPos`. Returns 3 if either endpoint is inside the cylinder, otherwise
 * returns the number of points of intersection with the side of the cylinder. The locations of those points are put in
 * `out1` and `out2`, with `out1` being closer to `itemPos`. Line segments that pass through both bases of the cylinder
 * are not detected.
 */
s32 CollisionCheck_CylSideVsLineSeg(f32 radius, f32 height, f32 offset, Vec3f* actorPos, Vec3f* itemPos,
                                    Vec3f* itemProjPos, Vec3f* out1, Vec3f* out2) {
    Vec3f actorToItem;
    Vec3f actorToItemProj;
    Vec3f itemStep;
    f32 frac1 = 0.0f;
    f32 frac2 = 0.0f;
    u32 intersect2;
    u32 intersect1;
    u32 test1;
    u32 test2;
    f32 radSqDiff;
    f32 actorDotItemXZ;
    f32 zero = 0.0f;
    f32 closeDist;
    s32 pad1;
    s32 pad2;

    actorToItem.x = itemPos->x - actorPos->x;
    actorToItem.y = itemPos->y - actorPos->y - offset;
    actorToItem.z = itemPos->z - actorPos->z;

    actorToItemProj.x = itemProjPos->x - actorPos->x;
    actorToItemProj.y = itemProjPos->y - actorPos->y - offset;
    actorToItemProj.z = itemProjPos->z - actorPos->z;

    itemStep.x = actorToItemProj.x - actorToItem.x;
    itemStep.y = actorToItemProj.y - actorToItem.y;
    itemStep.z = actorToItemProj.z - actorToItem.z;

    if ((actorToItem.y > 0.0f) && (actorToItem.y < height) && (sqrtf(SQXZ(actorToItem)) < radius)) {
        return 3;
    }

    if ((actorToItemProj.y > 0.0f) && (actorToItemProj.y < height) && (sqrtf(SQXZ(actorToItemProj)) < radius)) {
        return 3;
    }
    radSqDiff = SQXZ(actorToItem) - SQ(radius);
    if (!IS_ZERO(SQXZ(itemStep))) {
        actorDotItemXZ = DOTXZ(2.0f * itemStep, actorToItem);
        if (SQ(actorDotItemXZ) < (4.0f * SQXZ(itemStep) * radSqDiff)) {
            return 0;
        }
        if ((SQ(actorDotItemXZ) - (4.0f * SQXZ(itemStep) * radSqDiff)) > zero) {
            intersect1 = intersect2 = true;
        } else {
            intersect1 = true;
            intersect2 = false;
        }
        closeDist = sqrtf(SQ(actorDotItemXZ) - (4.0f * SQXZ(itemStep) * radSqDiff));
        if (intersect1) {
            frac1 = (closeDist - actorDotItemXZ) / (2.0f * SQXZ(itemStep));
        }
        if (intersect2) {
            frac2 = (-actorDotItemXZ - closeDist) / (2.0f * SQXZ(itemStep));
        }
    } else if (!IS_ZERO(DOTXZ(2.0f * itemStep, actorToItem))) {
        intersect1 = true;
        intersect2 = false;
        frac1 = -radSqDiff / DOTXZ(2.0f * itemStep, actorToItem);
    } else {
        if (radSqDiff <= 0.0f) {
            test1 = (0.0f < actorToItem.y) && (actorToItem.y < height);
            test2 = (0.0f < actorToItemProj.y) && (actorToItemProj.y < height);

            if (test1 && test2) {
                *out1 = actorToItem;
                *out2 = actorToItemProj;
                return 2;
            }
            if (test1) {
                *out1 = actorToItem;
                return 1;
            }
            if (test2) {
                *out1 = actorToItemProj;
                return 1;
            }
        }
        return 0;
    }

    if (!intersect2) {
        if ((frac1 < 0.0f) || (1.0f < frac1)) {
            return 0;
        }
    } else {
        test1 = (frac1 < 0.0f) || (1.0f < frac1);
        test2 = (frac2 < 0.0f) || (1.0f < frac2);

        if (test1 && test2) {
            return 0;
        }
        if (test1) {
            intersect1 = false;
        }
        if (test2) {
            intersect2 = false;
        }
    }

    if (intersect1 &&
        (((frac1 * itemStep.y + actorToItem.y) < 0.0f) || (height < (frac1 * itemStep.y + actorToItem.y)))) {
        intersect1 = false;
    }
    if (intersect2 &&
        (((frac2 * itemStep.y + actorToItem.y) < 0.0f) || (height < (frac2 * itemStep.y + actorToItem.y)))) {
        intersect2 = false;
    }
    if (!intersect1 && !intersect2) {
        return 0;
    } else if (intersect1 && intersect2) {
        out1->x = frac1 * itemStep.x + actorToItem.x + actorPos->x;
        out1->y = frac1 * itemStep.y + actorToItem.y + actorPos->y;
        out1->z = frac1 * itemStep.z + actorToItem.z + actorPos->z;
        out2->x = frac2 * itemStep.x + actorToItem.x + actorPos->x;
        out2->y = frac2 * itemStep.y + actorToItem.y + actorPos->y;
        out2->z = frac2 * itemStep.z + actorToItem.z + actorPos->z;
        return 2;
    } else if (intersect1) {
        out1->x = frac1 * itemStep.x + actorToItem.x + actorPos->x;
        out1->y = frac1 * itemStep.y + actorToItem.y + actorPos->y;
        out1->z = frac1 * itemStep.z + actorToItem.z + actorPos->z;
        return 1;
    } else if (intersect2) {
        out1->x = frac2 * itemStep.x + actorToItem.x + actorPos->x;
        out1->y = frac2 * itemStep.y + actorToItem.y + actorPos->y;
        out1->z = frac2 * itemStep.z + actorToItem.z + actorPos->z;
        return 1;
    }
    return 1;
}
