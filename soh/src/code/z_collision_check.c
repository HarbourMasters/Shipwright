#include "global.h"
#include "vt.h"
#include "overlays/effects/ovl_Effect_Ss_HitMark/z_eff_ss_hitmark.h"

typedef s32 (*ColChkResetFunc)(PlayState*, Collider*);
typedef void (*ColChkBloodFunc)(PlayState*, Collider*, Vec3f*);
typedef void (*ColChkApplyFunc)(PlayState*, CollisionCheckContext*, Collider*);
typedef void (*ColChkVsFunc)(PlayState*, CollisionCheckContext*, Collider*, Collider*);
typedef s32 (*ColChkLineFunc)(PlayState*, CollisionCheckContext*, Collider*, Vec3f*, Vec3f*);

typedef struct {
    /* 0 */ u8 blood;
    /* 1 */ u8 effect;
} HitInfo; // size = 0x2

typedef enum {
    /* 0 */ BLOOD_NONE,
    /* 1 */ BLOOD_BLUE,
    /* 2 */ BLOOD_GREEN,
    /* 3 */ BLOOD_WATER,
    /* 4 */ BLOOD_RED,
    /* 5 */ BLOOD_RED2
} ColChkBloodType;

typedef enum {
    /* 0 */ HIT_WHITE,
    /* 1 */ HIT_DUST,
    /* 2 */ HIT_RED,
    /* 3 */ HIT_SOLID,
    /* 4 */ HIT_WOOD,
    /* 5 */ HIT_NONE
} ColChkHitType;

typedef enum {
    /* 0 */ MASSTYPE_IMMOVABLE,
    /* 1 */ MASSTYPE_HEAVY,
    /* 2 */ MASSTYPE_NORMAL
} ColChkMassType;

/**
 * Draws a red triangle with vertices vA, vB, and vC.
 */
void Collider_DrawRedPoly(GraphicsContext* gfxCtx, Vec3f* vA, Vec3f* vB, Vec3f* vC) {
    Collider_DrawPoly(gfxCtx, vA, vB, vC, 255, 0, 0);
}

/**
 * Draws the triangle with vertices vA, vB, and vC and with the specified color.
 */
void Collider_DrawPoly(GraphicsContext* gfxCtx, Vec3f* vA, Vec3f* vB, Vec3f* vC, u8 r, u8 g, u8 b) {
    Vtx* vtxTbl;
    Vtx* vtx;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 originDist;

    OPEN_DISPS(gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0xFF, r, g, b, 50);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
    gSPTexture(POLY_OPA_DISP++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                      COMBINED);
    gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BOTH);
    gSPSetGeometryMode(POLY_OPA_DISP++, G_LIGHTING);
    gDPPipeSync(POLY_OPA_DISP++);

    vtxTbl = Graph_Alloc(gfxCtx, 3 * sizeof(Vtx));
    ASSERT(vtxTbl != NULL);

    vtxTbl[0].n.ob[0] = vA->x;
    vtxTbl[0].n.ob[1] = vA->y;
    vtxTbl[0].n.ob[2] = vA->z;
    vtxTbl[1].n.ob[0] = vB->x;
    vtxTbl[1].n.ob[1] = vB->y;
    vtxTbl[1].n.ob[2] = vB->z;
    vtxTbl[2].n.ob[0] = vC->x;
    vtxTbl[2].n.ob[1] = vC->y;
    vtxTbl[2].n.ob[2] = vC->z;

    Math3D_DefPlane(vA, vB, vC, &nx, &ny, &nz, &originDist);

    for (vtx = vtxTbl; vtx < vtxTbl + 3; vtx++) {
        vtx->n.flag = 0;
        vtx->n.tc[0] = 0;
        vtx->n.tc[1] = 0;
        vtx->n.n[0] = (u8)(s32)nx & 0xFF;
        vtx->n.n[1] = (u8)(s32)ny & 0xFF;
        vtx->n.n[2] = (u8)(s32)nz & 0xFF;
        vtx->n.a = 255;
    }

    gSPVertex(POLY_OPA_DISP++, vtxTbl, 3, 0);
    gSP1Triangle(POLY_OPA_DISP++, 0, 1, 2, 0);

    CLOSE_DISPS(gfxCtx);
}

s32 Collider_InitBase(PlayState* play, Collider* collider) {
    static Collider init = {
        NULL, NULL, NULL, NULL, AT_NONE, AC_NONE, OC1_NONE, OC2_NONE, COLTYPE_HIT3, COLSHAPE_INVALID,
    };

    *collider = init;
    return 1;
}

s32 Collider_DestroyBase(PlayState* play, Collider* collider) {
    return 1;
}

/**
 * Uses default OC2_TYPE_1 and COLTYPE_HIT0
 */
s32 Collider_SetBaseToActor(PlayState* play, Collider* collider, ColliderInitToActor* src) {
    collider->actor = src->actor;
    collider->atFlags = src->atFlags;
    collider->acFlags = src->acFlags;
    collider->ocFlags1 = src->ocFlags1;
    collider->ocFlags2 = OC2_TYPE_1;
    collider->shape = src->shape;
    return 1;
}

/**
 * Uses default OC2_TYPE_1
 */
s32 Collider_SetBaseType1(PlayState* play, Collider* collider, Actor* actor, ColliderInitType1* src) {
    collider->actor = actor;
    collider->colType = src->colType;
    collider->atFlags = src->atFlags;
    collider->acFlags = src->acFlags;
    collider->ocFlags1 = src->ocFlags1;
    collider->ocFlags2 = OC2_TYPE_1;
    collider->shape = src->shape;
    return 1;
}

s32 Collider_SetBase(PlayState* play, Collider* collider, Actor* actor, ColliderInit* src) {
    collider->actor = actor;
    collider->colType = src->colType;
    collider->atFlags = src->atFlags;
    collider->acFlags = src->acFlags;
    collider->ocFlags1 = src->ocFlags1;
    collider->ocFlags2 = src->ocFlags2;
    collider->shape = src->shape;
    return 1;
}

void Collider_ResetATBase(PlayState* play, Collider* collider) {
    collider->at = NULL;
    collider->atFlags &= ~(AT_HIT | AT_BOUNCED);
}

void Collider_ResetACBase(PlayState* play, Collider* collider) {
    collider->ac = NULL;
    collider->acFlags &= ~(AC_HIT | AC_BOUNCED);
}

void Collider_ResetOCBase(PlayState* play, Collider* collider) {
    collider->oc = NULL;
    collider->ocFlags1 &= ~OC1_HIT;
    collider->ocFlags2 &= ~OC2_HIT_PLAYER;
}

s32 Collider_InitTouch(PlayState* play, ColliderTouch* touch) {
    static ColliderTouch init = { 0x00000000, 0, 0 };

    *touch = init;
    return 1;
}

s32 Collider_DestroyTouch(PlayState* play, ColliderTouch* touch) {
    return 1;
}

s32 Collider_SetTouch(PlayState* play, ColliderTouch* dest, ColliderTouch* src) {
    dest->dmgFlags = src->dmgFlags;
    dest->effect = src->effect;
    dest->damage = src->damage;
    return 1;
}

void Collider_ResetATInfo_Unk(PlayState* play, ColliderInfo* info) {
}

s32 Collider_InitBump(PlayState* play, ColliderBump* bump) {
    static ColliderBump init = { 0xFFCFFFFF, 0, 0, { 0, 0, 0 } };

    *bump = init;
    return 1;
}

s32 Collider_DestroyBump(PlayState* play, ColliderBump* bump) {
    return 1;
}

s32 Collider_SetBump(PlayState* play, ColliderBump* bump, ColliderBumpInit* init) {
    bump->dmgFlags = init->dmgFlags;
    bump->effect = init->effect;
    bump->defense = init->defense;
    return 1;
}

s32 Collider_InitInfo(PlayState* play, ColliderInfo* info) {
    static ColliderInfo init = {
        { 0, 0, 0 },   { 0xFFCFFFFF, 0, 0, { 0, 0, 0 } },
        ELEMTYPE_UNK0, TOUCH_NONE,
        BUMP_NONE,     OCELEM_NONE,
        NULL,          NULL,
        NULL,          NULL,
    };

    *info = init;
    Collider_InitTouch(play, &info->toucher);
    Collider_InitBump(play, &info->bumper);
    return 1;
}

s32 Collider_DestroyInfo(PlayState* play, ColliderInfo* info) {
    Collider_DestroyTouch(play, &info->toucher);
    Collider_DestroyBump(play, &info->bumper);
    return 1;
}

s32 Collider_SetInfo(PlayState* play, ColliderInfo* info, ColliderInfoInit* infoInit) {
    info->elemType = infoInit->elemType;
    Collider_SetTouch(play, &info->toucher, &infoInit->toucher);
    Collider_SetBump(play, &info->bumper, &infoInit->bumper);
    info->toucherFlags = infoInit->toucherFlags;
    info->bumperFlags = infoInit->bumperFlags;
    info->ocElemFlags = infoInit->ocElemFlags;
    return 1;
}

void Collider_ResetATInfo(PlayState* play, ColliderInfo* info) {
    info->atHit = NULL;
    info->atHitInfo = NULL;
    info->toucherFlags &= ~TOUCH_HIT;
    info->toucherFlags &= ~TOUCH_DREW_HITMARK;
    Collider_ResetATInfo_Unk(play, info);
}

void Collider_ResetACInfo(PlayState* play, ColliderInfo* info) {
    info->bumper.hitPos.x = info->bumper.hitPos.y = info->bumper.hitPos.z = 0;
    info->bumperFlags &= ~BUMP_HIT;
    info->bumperFlags &= ~BUMP_DRAW_HITMARK;
    info->acHit = NULL;
    info->acHitInfo = NULL;
}

void Collider_ResetOCInfo(PlayState* play, ColliderInfo* info) {
    info->ocElemFlags &= ~OCELEM_HIT;
}

s32 Collider_InitJntSphElementDim(PlayState* play, ColliderJntSphElementDim* dim) {
    static ColliderJntSphElementDim init = {
        { { 0, 0, 0 }, 0 },
        { { 0, 0, 0 }, 0 },
        0.0f,
        0,
    };
    *dim = init;
    return 1;
}

s32 Collider_DestroyJntSphElementDim(PlayState* play, ColliderJntSphElementDim* element) {
    return 1;
}

s32 Collider_SetJntSphElementDim(PlayState* play, ColliderJntSphElementDim* dest,
                                 ColliderJntSphElementDimInit* src) {
    dest->limb = src->limb;
    dest->modelSphere = src->modelSphere;
    dest->scale = src->scale * 0.01f;
    return 1;
}

s32 Collider_InitJntSphElement(PlayState* play, ColliderJntSphElement* element) {
    Collider_InitInfo(play, &element->info);
    Collider_InitJntSphElementDim(play, &element->dim);
    return 1;
}

s32 Collider_DestroyJntSphElement(PlayState* play, ColliderJntSphElement* element) {
    Collider_DestroyInfo(play, &element->info);
    Collider_DestroyJntSphElementDim(play, &element->dim);
    return 1;
}

s32 Collider_SetJntSphElement(PlayState* play, ColliderJntSphElement* dest, ColliderJntSphElementInit* src) {
    Collider_SetInfo(play, &dest->info, &src->info);
    Collider_SetJntSphElementDim(play, &dest->dim, &src->dim);
    return 1;
}

s32 Collider_ResetJntSphElementAT(PlayState* play, ColliderJntSphElement* collider) {
    Collider_ResetATInfo(play, &collider->info);
    return 1;
}

s32 Collider_ResetJntSphElementAC(PlayState* play, ColliderJntSphElement* collider) {
    Collider_ResetACInfo(play, &collider->info);
    return 1;
}

s32 Collider_ResetJntSphElementOC(PlayState* play, ColliderJntSphElement* collider) {
    Collider_ResetOCInfo(play, &collider->info);
    return 1;
}

/**
 * Initializes a ColliderJntSph to default values
 */
s32 Collider_InitJntSph(PlayState* play, ColliderJntSph* collider) {
    Collider_InitBase(play, &collider->base);
    collider->count = 0;
    collider->elements = NULL;
    return 1;
}

/**
 * Destroys a dynamically allocated ColliderJntSph
 */
s32 Collider_FreeJntSph(PlayState* play, ColliderJntSph* collider) {
    ColliderJntSphElement* element;

    Collider_DestroyBase(play, &collider->base);
    for (element = collider->elements; element < collider->elements + collider->count; element++) {
        Collider_DestroyJntSphElement(play, element);
    }

    collider->count = 0;
    if (collider->elements != NULL) {
        ZELDA_ARENA_FREE_DEBUG(collider->elements);
    }
    collider->elements = NULL;
    return 1;
}

/**
 * Destroys a preallocated ColliderJntSph
 */
s32 Collider_DestroyJntSph(PlayState* play, ColliderJntSph* collider) {
    ColliderJntSphElement* element;

    Collider_DestroyBase(play, &collider->base);
    for (element = collider->elements; element < collider->elements + collider->count; element++) {
        Collider_DestroyJntSphElement(play, element);
    }
    collider->count = 0;
    collider->elements = NULL;
    return 1;
}

/**
 * Sets up the ColliderJntSph using the values in src, sets it to the actor specified in src, and dynamically allocates
 * the element array. Uses default OC2_TYPE_1 and COLTYPE_HIT0. Unused.
 */
s32 Collider_SetJntSphToActor(PlayState* play, ColliderJntSph* dest, ColliderJntSphInitToActor* src) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    Collider_SetBaseToActor(play, &dest->base, &src->base);
    dest->count = src->count;
    dest->elements = ZELDA_ARENA_MALLOC_DEBUG(src->count * sizeof(ColliderJntSphElement));

    if (dest->elements == NULL) {
        dest->count = 0;
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("ClObjJntSph_set():zelda_malloc()出来ません。\n"); // "Can not."
        osSyncPrintf(VT_RST);
        return 0;
    }

    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        Collider_InitJntSphElement(play, destElem);
        Collider_SetJntSphElement(play, destElem, srcElem);
    }
    return 1;
}

/**
 * Sets up the ColliderJntSph using the values in src and dynamically allocates the element array. Uses default
 * OC2_TYPE_1. Only used by En_Nwc, an unused and unfinished actor.
 */
s32 Collider_SetJntSphAllocType1(PlayState* play, ColliderJntSph* dest, Actor* actor,
                                 ColliderJntSphInitType1* src) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    Collider_SetBaseType1(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = ZELDA_ARENA_MALLOC_DEBUG(src->count * sizeof(ColliderJntSphElement));

    if (dest->elements == NULL) {
        dest->count = 0;
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("ClObjJntSph_set3():zelda_malloc_出来ません。\n"); // "Can not."
        osSyncPrintf(VT_RST);
        return 0;
    }

    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        Collider_InitJntSphElement(play, destElem);
        Collider_SetJntSphElement(play, destElem, srcElem);
    }
    return 1;
}

/**
 * Sets up the ColliderJntSph using the values in src and dynamically allocates the element array.
 * Unused.
 */
s32 Collider_SetJntSphAlloc(PlayState* play, ColliderJntSph* dest, Actor* actor, ColliderJntSphInit* src) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    Collider_SetBase(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = ZELDA_ARENA_MALLOC_DEBUG(src->count * sizeof(ColliderJntSphElement));

    if (dest->elements == NULL) {
        dest->count = 0;
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("ClObjJntSph_set5():zelda_malloc出来ません\n"); // "Can not."
        osSyncPrintf(VT_RST);
        return 0;
    }
    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        Collider_InitJntSphElement(play, destElem);
        Collider_SetJntSphElement(play, destElem, srcElem);
    }
    return 1;
}

/**
 * Sets up the ColliderJntSph using the values in src, placing the element array in elements.
 */
s32 Collider_SetJntSph(PlayState* play, ColliderJntSph* dest, Actor* actor, ColliderJntSphInit* src,
                       ColliderJntSphElement* elements) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    Collider_SetBase(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = elements;
    ASSERT(dest->elements != NULL);

    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        Collider_InitJntSphElement(play, destElem);
        Collider_SetJntSphElement(play, destElem, srcElem);
    }
    return 1;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 Collider_ResetJntSphAT(PlayState* play, Collider* collider) {
    ColliderJntSphElement* element;
    ColliderJntSph* jntSph = (ColliderJntSph*)collider;

    Collider_ResetATBase(play, &jntSph->base);

    for (element = jntSph->elements; element < jntSph->elements + jntSph->count; element++) {
        Collider_ResetJntSphElementAT(play, element);
    }
    return 1;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 Collider_ResetJntSphAC(PlayState* play, Collider* collider) {
    ColliderJntSphElement* element;
    ColliderJntSph* jntSph = (ColliderJntSph*)collider;

    Collider_ResetACBase(play, &jntSph->base);

    for (element = jntSph->elements; element < jntSph->elements + jntSph->count; element++) {
        Collider_ResetJntSphElementAC(play, element);
    }
    return 1;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 Collider_ResetJntSphOC(PlayState* play, Collider* collider) {
    ColliderJntSphElement* element;
    ColliderJntSph* jntSph = (ColliderJntSph*)collider;

    Collider_ResetOCBase(play, &jntSph->base);

    for (element = jntSph->elements; element < jntSph->elements + jntSph->count; element++) {
        Collider_ResetJntSphElementOC(play, element);
    }
    return 1;
}

s32 Collider_InitCylinderDim(PlayState* play, Cylinder16* dim) {
    Cylinder16 init = { 0, 0, 0, { 0, 0, 0 } };

    *dim = init;
    return 1;
}

s32 Collider_DestroyCylinderDim(PlayState* play, Cylinder16* dim) {
    return 1;
}

s32 Collider_SetCylinderDim(PlayState* play, Cylinder16* dest, Cylinder16* src) {
    *dest = *src;
    return 1;
}

/**
 * Initializes a ColliderCylinder to default values
 */
s32 Collider_InitCylinder(PlayState* play, ColliderCylinder* collider) {
    Collider_InitBase(play, &collider->base);
    Collider_InitInfo(play, &collider->info);
    Collider_InitCylinderDim(play, &collider->dim);
    return 1;
}

/**
 * Destroys a ColliderCylinder
 */
s32 Collider_DestroyCylinder(PlayState* play, ColliderCylinder* collider) {
    Collider_DestroyBase(play, &collider->base);
    Collider_DestroyInfo(play, &collider->info);
    Collider_DestroyCylinderDim(play, &collider->dim);
    return 1;
}

/**
 * Sets up the ColliderCylinder using the values in src and sets it to the actor specified in src. Uses default
 * OC2_TYPE_1 and COLTYPE_0. Used only by DekuJr, who sets it to himself anyways.
 */
s32 Collider_SetCylinderToActor(PlayState* play, ColliderCylinder* collider,
                                ColliderCylinderInitToActor* src) {
    Collider_SetBaseToActor(play, &collider->base, &src->base);
    Collider_SetInfo(play, &collider->info, &src->info);
    Collider_SetCylinderDim(play, &collider->dim, &src->dim);
    return 1;
}

/**
 * Sets up the ColliderCylinder using the values in src. Uses default OC2_TYPE_1
 */
s32 Collider_SetCylinderType1(PlayState* play, ColliderCylinder* collider, Actor* actor,
                              ColliderCylinderInitType1* src) {
    Collider_SetBaseType1(play, &collider->base, actor, &src->base);
    Collider_SetInfo(play, &collider->info, &src->info);
    Collider_SetCylinderDim(play, &collider->dim, &src->dim);
    return 1;
}

/**
 * Sets up the ColliderCylinder using the values in src.
 */
s32 Collider_SetCylinder(PlayState* play, ColliderCylinder* collider, Actor* actor,
                         ColliderCylinderInit* src) {
    Collider_SetBase(play, &collider->base, actor, &src->base);
    Collider_SetInfo(play, &collider->info, &src->info);
    Collider_SetCylinderDim(play, &collider->dim, &src->dim);
    return 1;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 Collider_ResetCylinderAT(PlayState* play, Collider* collider) {
    ColliderCylinder* cylinder = (ColliderCylinder*)collider;

    Collider_ResetATBase(play, &cylinder->base);
    Collider_ResetATInfo(play, &cylinder->info);
    return 1;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 Collider_ResetCylinderAC(PlayState* play, Collider* collider) {
    ColliderCylinder* cylinder = (ColliderCylinder*)collider;

    Collider_ResetACBase(play, &cylinder->base);
    Collider_ResetACInfo(play, &cylinder->info);
    return 1;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 Collider_ResetCylinderOC(PlayState* play, Collider* collider) {
    ColliderCylinder* cylinder = (ColliderCylinder*)collider;

    Collider_ResetOCBase(play, &cylinder->base);
    Collider_ResetOCInfo(play, &cylinder->info);
    return 1;
}

s32 Collider_InitTrisElementDim(PlayState* play, TriNorm* dim) {
    static TriNorm init = {
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.0f, 0.0f, 0.0f }, 0.0f },
    };

    *dim = init;
    return 1;
}

s32 Collider_DestroyTrisElementDim(PlayState* play, TriNorm* dim) {
    return 1;
}

s32 Collider_SetTrisElementDim(PlayState* play, TriNorm* dest, ColliderTrisElementDimInit* src) {
    Vec3f* destVtx;
    Vec3f* srcVtx;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 originDist;

    for (destVtx = dest->vtx, srcVtx = src->vtx; destVtx < dest->vtx + 3; destVtx++, srcVtx++) {
        *destVtx = *srcVtx;
    }

    Math3D_DefPlane(&src->vtx[0], &src->vtx[1], &src->vtx[2], &nx, &ny, &nz, &originDist);

    dest->plane.normal.x = nx;
    dest->plane.normal.y = ny;
    dest->plane.normal.z = nz;
    dest->plane.originDist = originDist;
    return 1;
}

s32 Collider_InitTrisElement(PlayState* play, ColliderTrisElement* collider) {
    Collider_InitInfo(play, &collider->info);
    Collider_InitTrisElementDim(play, &collider->dim);
    return 1;
}

s32 Collider_DestroyTrisElement(PlayState* play, ColliderTrisElement* collider) {
    Collider_DestroyInfo(play, &collider->info);
    Collider_DestroyTrisElementDim(play, &collider->dim);
    return 1;
}

s32 Collider_SetTrisElement(PlayState* play, ColliderTrisElement* dest, ColliderTrisElementInit* src) {
    Collider_SetInfo(play, &dest->info, &src->info);
    Collider_SetTrisElementDim(play, &dest->dim, &src->dim);
    return 1;
}

s32 Collider_ResetTrisElementAT(PlayState* play, ColliderTrisElement* element) {
    Collider_ResetATInfo(play, &element->info);
    return 1;
}

s32 Collider_ResetTrisElementAC(PlayState* play, ColliderTrisElement* element) {
    Collider_ResetACInfo(play, &element->info);
    return 1;
}

s32 Collider_ResetTrisElementOC(PlayState* play, ColliderTrisElement* element) {
    Collider_ResetOCInfo(play, &element->info);
    return 1;
}

/**
 * Initializes a ColliderTris to default values
 */
s32 Collider_InitTris(PlayState* play, ColliderTris* tris) {
    Collider_InitBase(play, &tris->base);
    tris->count = 0;
    tris->elements = 0;
    return 1;
}

/**
 * Destroys a dynamically allocated ColliderTris
 * Unused
 */
s32 Collider_FreeTris(PlayState* play, ColliderTris* tris) {
    ColliderTrisElement* element;

    Collider_DestroyBase(play, &tris->base);
    for (element = tris->elements; element < tris->elements + tris->count; element++) {
        Collider_DestroyTrisElement(play, element);
    }

    tris->count = 0;
    if (tris->elements != NULL) {
        ZELDA_ARENA_FREE_DEBUG(tris->elements);
    }
    tris->elements = NULL;
    return 1;
}

/**
 * Destroys a preallocated ColliderTris
 */
s32 Collider_DestroyTris(PlayState* play, ColliderTris* tris) {
    ColliderTrisElement* element;

    Collider_DestroyBase(play, &tris->base);
    for (element = tris->elements; element < tris->elements + tris->count; element++) {
        Collider_DestroyTrisElement(play, element);
    }

    tris->count = 0;
    tris->elements = NULL;
    return 1;
}

/**
 * Sets up the ColliderTris using the values in src and dynamically allocates the element array. Uses default OC2_TYPE_1
 * Unused.
 */
s32 Collider_SetTrisAllocType1(PlayState* play, ColliderTris* dest, Actor* actor, ColliderTrisInitType1* src) {
    ColliderTrisElement* destElem;
    ColliderTrisElementInit* srcElem;

    Collider_SetBaseType1(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = ZELDA_ARENA_MALLOC_DEBUG(dest->count * sizeof(ColliderTrisElement));
    if (dest->elements == NULL) {
        dest->count = 0;
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("ClObjTris_set3():zelda_malloc()出来ません\n"); // "Can not."
        osSyncPrintf(VT_RST);
        return 0;
    }
    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        Collider_InitTrisElement(play, destElem);
        Collider_SetTrisElement(play, destElem, srcElem);
    }
    return 1;
}

/**
 * Sets up the ColliderTris using the values in src and dynamically allocates the element array.
 * Unused
 */
s32 Collider_SetTrisAlloc(PlayState* play, ColliderTris* dest, Actor* actor, ColliderTrisInit* src) {
    ColliderTrisElement* destElem;
    ColliderTrisElementInit* srcElem;

    Collider_SetBase(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = ZELDA_ARENA_MALLOC_DEBUG(dest->count * sizeof(ColliderTrisElement));

    if (dest->elements == NULL) {
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("ClObjTris_set5():zelda_malloc出来ません\n"); // "Can not."
        osSyncPrintf(VT_RST);
        dest->count = 0;
        return 0;
    }

    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        Collider_InitTrisElement(play, destElem);
        Collider_SetTrisElement(play, destElem, srcElem);
    }
    return 1;
}

/**
 * Sets up the ColliderTris using the values in src, placing the element array in elements.
 */
s32 Collider_SetTris(PlayState* play, ColliderTris* dest, Actor* actor, ColliderTrisInit* src,
                     ColliderTrisElement* elements) {
    ColliderTrisElement* destElem;
    ColliderTrisElementInit* srcElem;

    Collider_SetBase(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = elements;
    ASSERT(dest->elements != NULL);

    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        Collider_InitTrisElement(play, destElem);
        Collider_SetTrisElement(play, destElem, srcElem);
    }
    return 1;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 Collider_ResetTrisAT(PlayState* play, Collider* collider) {
    ColliderTrisElement* element;
    ColliderTris* tris = (ColliderTris*)collider;

    Collider_ResetATBase(play, &tris->base);
    for (element = tris->elements; element < tris->elements + tris->count; element++) {
        Collider_ResetTrisElementAT(play, element);
    }
    return 1;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 Collider_ResetTrisAC(PlayState* play, Collider* collider) {
    ColliderTrisElement* element;
    ColliderTris* tris = (ColliderTris*)collider;

    Collider_ResetACBase(play, &tris->base);
    for (element = tris->elements; element < tris->elements + tris->count; element++) {
        Collider_ResetTrisElementAC(play, element);
    }
    return 1;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 Collider_ResetTrisOC(PlayState* play, Collider* collider) {
    ColliderTrisElement* element;
    ColliderTris* tris = (ColliderTris*)collider;

    Collider_ResetOCBase(play, &tris->base);
    for (element = tris->elements; element < tris->elements + tris->count; element++) {
        Collider_ResetTrisElementOC(play, element);
    }
    return 1;
}

s32 Collider_InitQuadDim(PlayState* play, ColliderQuadDim* dim) {
    static ColliderQuadDim init = {
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
        { 0, 0, 0 },
        { 0, 0, 0 },
        1.0E38f,
    };

    *dim = init;
    return 1;
}

s32 Collider_DestroyQuadDim(PlayState* play, ColliderQuadDim* dim) {
    return 1;
}

s32 Collider_ResetQuadACDist(PlayState* play, ColliderQuadDim* dim) {
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

s32 Collider_SetQuadDim(PlayState* play, ColliderQuadDim* dest, ColliderQuadDimInit* src) {
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
s32 Collider_InitQuad(PlayState* play, ColliderQuad* collider) {
    Collider_InitBase(play, &collider->base);
    Collider_InitInfo(play, &collider->info);
    Collider_InitQuadDim(play, &collider->dim);
    return 1;
}

/**
 * Destroys a ColliderQuad.
 */
s32 Collider_DestroyQuad(PlayState* play, ColliderQuad* collider) {
    Collider_DestroyBase(play, &collider->base);
    Collider_DestroyInfo(play, &collider->info);
    Collider_DestroyQuadDim(play, &collider->dim);
    return 1;
}

/**
 * Sets up the ColliderQuad using the values in src. Uses the default OC2_TYPE_1
 */
s32 Collider_SetQuadType1(PlayState* play, ColliderQuad* collider, Actor* actor, ColliderQuadInitType1* src) {
    Collider_SetBaseType1(play, &collider->base, actor, &src->base);
    Collider_SetInfo(play, &collider->info, &src->info);
    Collider_SetQuadDim(play, &collider->dim, &src->dim);
    return 1;
}

/**
 * Sets up the ColliderQuad using the values in src.
 */
s32 Collider_SetQuad(PlayState* play, ColliderQuad* collider, Actor* actor, ColliderQuadInit* src) {
    Collider_SetBase(play, &collider->base, actor, &src->base);
    Collider_SetInfo(play, &collider->info, &src->info);
    Collider_SetQuadDim(play, &collider->dim, &src->dim);
    return 1;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 Collider_ResetQuadAT(PlayState* play, Collider* collider) {
    ColliderQuad* quad = (ColliderQuad*)collider;

    Collider_ResetATBase(play, &quad->base);
    Collider_ResetATInfo(play, &quad->info);
    Collider_ResetQuadACDist(play, &quad->dim);
    return 1;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 Collider_ResetQuadAC(PlayState* play, Collider* collider) {
    ColliderQuad* quad = (ColliderQuad*)collider;

    Collider_ResetACBase(play, &quad->base);
    Collider_ResetACInfo(play, &quad->info);
    return 1;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 Collider_ResetQuadOC(PlayState* play, Collider* collider) {
    ColliderQuad* quad = (ColliderQuad*)collider;

    Collider_ResetOCBase(play, &quad->base);
    Collider_ResetOCInfo(play, &quad->info);
    return 1;
}

/**
 * For quad colliders with AT_NEAREST, resets the previous AC collider it hit if the current element is closer,
 * otherwise returns false. Used on player AT colliders to prevent multiple collisions from registering.
 */
s32 Collider_QuadSetNearestAC(PlayState* play, ColliderQuad* quad, Vec3f* hitPos) {
    f32 acDist;
    Vec3f dcMid;

    if (!(quad->info.toucherFlags & TOUCH_NEAREST)) {
        return true;
    }
    Math_Vec3s_ToVec3f(&dcMid, &quad->dim.dcMid);
    acDist = Math3D_Vec3fDistSq(&dcMid, hitPos);
    if (acDist < quad->dim.acDist) {
        quad->dim.acDist = acDist;
        if (quad->info.atHit != NULL) {
            Collider_ResetACBase(play, quad->info.atHit);
        }
        if (quad->info.atHitInfo != NULL) {
            Collider_ResetACInfo(play, quad->info.atHitInfo);
        }
        return true;
    }
    return false;
}

/**
 * Initializes an OcLine to default values
 * OcLines are entirely unused.
 */
s32 Collider_InitLine(PlayState* play, OcLine* line) {
    Vec3f init = { 0.0f, 0.0f, 0.0f };

    Math_Vec3f_Copy(&line->line.a, &init);
    Math_Vec3f_Copy(&line->line.b, &init);
    return 1;
}

/**
 * Destroys an OcLine
 * OcLines are entirely unused.
 */
s32 Collider_DestroyLine(PlayState* play, OcLine* line) {
    return 1;
}

/**
 * Sets up an OcLine with endpoints a and b.
 * OcLines are entirely unused.
 */
s32 Collider_SetLinePoints(PlayState* PlayState, OcLine* ocLine, Vec3f* a, Vec3f* b) {
    Math_Vec3f_Copy(&ocLine->line.a, a);
    Math_Vec3f_Copy(&ocLine->line.b, b);
    return 1;
}

/**
 * Sets up an OcLine using the values in src.
 * OcLines are entirely unused.
 */
s32 Collider_SetLine(PlayState* play, OcLine* dest, OcLine* src) {
    dest->ocFlags = src->ocFlags;
    Collider_SetLinePoints(play, dest, &src->line.a, &src->line.b);
    return 1;
}

/**
 * Resets the OcLine's collision flags.
 * OcLines are entirely unused.
 */
s32 Collider_ResetLineOC(PlayState* play, OcLine* line) {
    line->ocFlags &= ~OCLINE_HIT;
    return 1;
}

/**
 * Initializes CollisionCheckContext. Clears all collider arrays, disables SAC, and sets flags for drawing colliders.
 */
void CollisionCheck_InitContext(PlayState* play, CollisionCheckContext* colChkCtx) {
    colChkCtx->sacFlags = 0;
    CollisionCheck_ClearContext(play, colChkCtx);
    AREG(21) = true;
    AREG(22) = true;
    AREG(23) = true;
}

void CollisionCheck_DestroyContext(PlayState* play, CollisionCheckContext* colChkCtx) {
}

/**
 * Clears all collider lists in CollisionCheckContext when not in SAC mode.
 */
void CollisionCheck_ClearContext(PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** col;
    OcLine** line;

    if (!(colChkCtx->sacFlags & 1)) {
        colChkCtx->colATCount = 0;
        colChkCtx->colACCount = 0;
        colChkCtx->colOCCount = 0;
        colChkCtx->colLineCount = 0;
        for (col = colChkCtx->colAT; col < colChkCtx->colAT + COLLISION_CHECK_AT_MAX; col++) {
            *col = NULL;
        }

        for (col = colChkCtx->colAC; col < colChkCtx->colAC + COLLISION_CHECK_AC_MAX; col++) {
            *col = NULL;
        }

        for (col = colChkCtx->colOC; col < colChkCtx->colOC + COLLISION_CHECK_OC_MAX; col++) {
            *col = NULL;
        }

        for (line = colChkCtx->colLine; line < colChkCtx->colLine + COLLISION_CHECK_OC_LINE_MAX; line++) {
            *line = NULL;
        }
    }
}

/**
 * Enables SAC, an alternate collision check mode that allows direct management of collider lists. Unused.
 */
void CollisionCheck_EnableSAC(PlayState* play, CollisionCheckContext* colChkCtx) {
    colChkCtx->sacFlags |= 1;
}

/**
 * Disables SAC, an alternate collision check mode that allows direct management of collider lists. Unused.
 */
void CollisionCheck_DisableSAC(PlayState* play, CollisionCheckContext* colChkCtx) {
    colChkCtx->sacFlags &= ~1;
}

/**
 * Draws a collider of any shape.
 * Math3D_DrawSphere and Math3D_DrawCylinder are noops, so JntSph and Cylinder are not drawn.
 */
void Collider_Draw(PlayState* play, Collider* collider) {
    ColliderJntSph* jntSph;
    ColliderCylinder* cylinder;
    ColliderTris* tris;
    ColliderQuad* quad;
    s32 i;

    if (collider == NULL) {
        return;
    }
    switch (collider->shape) {
        case COLSHAPE_JNTSPH:
            jntSph = (ColliderJntSph*)collider;
            for (i = 0; i < jntSph->count; i++) {
                Math3D_DrawSphere(play, &jntSph->elements[i].dim.worldSphere);
            }
            break;
        case COLSHAPE_CYLINDER:
            cylinder = (ColliderCylinder*)collider;
            Math3D_DrawCylinder(play, &cylinder->dim);
            break;
        case COLSHAPE_TRIS:
            tris = (ColliderTris*)collider;
            for (i = 0; i < tris->count; i++) {
                Collider_DrawRedPoly(play->state.gfxCtx, &tris->elements[i].dim.vtx[0],
                                     &tris->elements[i].dim.vtx[1], &tris->elements[i].dim.vtx[2]);
            }
            break;
        case COLSHAPE_QUAD:
            quad = (ColliderQuad*)collider;
            Collider_DrawRedPoly(play->state.gfxCtx, &quad->dim.quad[2], &quad->dim.quad[3], &quad->dim.quad[1]);
            Collider_DrawRedPoly(play->state.gfxCtx, &quad->dim.quad[1], &quad->dim.quad[0], &quad->dim.quad[2]);
            break;
    }
}

/**
 * Draws collision if AREG(15) and other AREGs are set. AREG(21) draws AT colliders, AREG(22) draws AC colliders,
 * AREG(23) draws OC colliders, AREG(24) draws dynapolys, and AREG(25) draws bg polys
 */
void CollisionCheck_DrawCollision(PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider* collider;
    s32 i;

    if (AREG(15)) {
        if (AREG(21)) {
            for (i = 0; i < colChkCtx->colATCount; i++) {
                Collider_Draw(play, colChkCtx->colAT[i]);
            }
        }
        if (AREG(22)) {
            for (i = 0; i < colChkCtx->colACCount; i++) {
                Collider_Draw(play, colChkCtx->colAC[i]);
            }
        }
        if (AREG(23)) {
            for (i = 0; i < colChkCtx->colOCCount; i++) {
                collider = colChkCtx->colOC[i];
                if (collider->ocFlags1 & OC1_ON) {
                    Collider_Draw(play, collider);
                }
            }
        }
        if (AREG(24)) {
            BgCheck_DrawDynaCollision(play, &play->colCtx);
        }
        if (AREG(25)) {
            BgCheck_DrawStaticCollision(play, &play->colCtx);
        }
    }
}

static ColChkResetFunc sATResetFuncs[] = {
    Collider_ResetJntSphAT,
    Collider_ResetCylinderAT,
    Collider_ResetTrisAT,
    Collider_ResetQuadAT,
};

/**
 * Sets collider as an AT (attack) for the current frame, which will be checked against ACs (attack colliders)
 * The last argument takes a Collider, so pass collider.base rather than the raw collider.
 */
s32 CollisionCheck_SetAT(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    s32 index;

    if (FrameAdvance_IsEnabled(play) == true) {
        return -1;
    }
    ASSERT(collider->shape <= COLSHAPE_QUAD);
    sATResetFuncs[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->colATCount >= COLLISION_CHECK_AT_MAX) {
        // "Index exceeded and cannot add more"
        osSyncPrintf("CollisionCheck_setAT():インデックスがオーバーして追加不能\n");
        return -1;
    }
    if (colChkCtx->sacFlags & 1) {
        return -1;
    }
    index = colChkCtx->colATCount;
    colChkCtx->colAT[colChkCtx->colATCount++] = collider;
    return index;
}

/**
 * Unused. Sets collider as an AT (attack) for the current frame, which will be checked against ACs (attack colliders).
 * If CollisionCheck_SAC is enabled, the collider will be inserted into the list at the specified index, otherwise it
 * will be inserted into the next slot
 */
s32 CollisionCheck_SetAT_SAC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider,
                             s32 index) {
    ASSERT(collider->shape <= COLSHAPE_QUAD);
    if (FrameAdvance_IsEnabled(play) == true) {
        return -1;
    }
    sATResetFuncs[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->sacFlags & 1) {
        if (!(index < colChkCtx->colATCount)) {
            // "You are trying to register a location that is larger than the total number of data."
            osSyncPrintf("CollisionCheck_setAT_SAC():全データ数より大きいところに登録しようとしている。\n");
            return -1;
        }
        colChkCtx->colAT[index] = collider;
    } else {
        if (!(colChkCtx->colATCount < COLLISION_CHECK_AT_MAX)) {
            // "Index exceeded and cannot add more"
            osSyncPrintf("CollisionCheck_setAT():インデックスがオーバーして追加不能\n");
            return -1;
        }
        index = colChkCtx->colATCount;
        colChkCtx->colAT[colChkCtx->colATCount++] = collider;
    }
    return index;
}

static ColChkResetFunc sACResetFuncs[] = {
    Collider_ResetJntSphAC,
    Collider_ResetCylinderAC,
    Collider_ResetTrisAC,
    Collider_ResetQuadAC,
};

/**
 * Sets collider as an AC (attack collider) for the current frame, allowing it to detect ATs (attacks)
 * The last argument takes a Collider, so pass collider.base rather than the raw collider.
 */
s32 CollisionCheck_SetAC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    s32 index;

    if (FrameAdvance_IsEnabled(play) == true) {
        return -1;
    }
    ASSERT(collider->shape <= COLSHAPE_QUAD);
    sACResetFuncs[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->colACCount >= COLLISION_CHECK_AC_MAX) {
        // "Index exceeded and cannot add more"
        osSyncPrintf("CollisionCheck_setAC():インデックスがオーバして追加不能\n");
        return -1;
    }
    if (colChkCtx->sacFlags & 1) {
        return -1;
    }
    index = colChkCtx->colACCount;
    colChkCtx->colAC[colChkCtx->colACCount++] = collider;
    return index;
}

/**
 * Unused. Sets collider as an AC (attack collider) for the current frame, allowing it to detect ATs (attacks).
 * If CollisionCheck_SAC is enabled, the collider will be inserted into the list at the specified index, otherwise it
 * will be inserted into the next slot
 */
s32 CollisionCheck_SetAC_SAC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider,
                             s32 index) {
    ASSERT(collider->shape <= COLSHAPE_QUAD);
    if (FrameAdvance_IsEnabled(play) == true) {
        return -1;
    }
    sACResetFuncs[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->sacFlags & 1) {
        if (!(index < colChkCtx->colACCount)) {
            // "You are trying to register a location that is larger than the total number of data."
            osSyncPrintf("CollisionCheck_setAC_SAC():全データ数より大きいところに登録しようとしている。\n");
            return -1;
        }
        colChkCtx->colAC[index] = collider;
    } else {
        if (!(colChkCtx->colACCount < COLLISION_CHECK_AC_MAX)) {
            // "Index exceeded and cannot add more"
            osSyncPrintf("CollisionCheck_setAC():インデックスがオーバして追加不能\n");
            return -1;
        }
        index = colChkCtx->colACCount;
        colChkCtx->colAC[colChkCtx->colACCount++] = collider;
    }
    return index;
}

static ColChkResetFunc sOCResetFuncs[] = {
    Collider_ResetJntSphOC,
    Collider_ResetCylinderOC,
    Collider_ResetTrisOC,
    Collider_ResetQuadOC,
};

/**
 * Sets collider as an OC (object collider) for the current frame, allowing it to detect other OCs
 * The last argument takes a Collider, so pass collider.base rather than the raw collider.
 */
s32 CollisionCheck_SetOC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    s32 index;

    if (FrameAdvance_IsEnabled(play) == true) {
        return -1;
    }

    ASSERT(collider->shape <= COLSHAPE_QUAD);

    sOCResetFuncs[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->colOCCount >= COLLISION_CHECK_OC_MAX) {
        // "Index exceeded and cannot add more"
        osSyncPrintf("CollisionCheck_setOC():インデックスがオーバして追加不能\n");
        return -1;
    }
    if (colChkCtx->sacFlags & 1) {
        return -1;
    }
    index = colChkCtx->colOCCount;
    colChkCtx->colOC[colChkCtx->colOCCount++] = collider;
    return index;
}

/**
 * Unused. Sets collider as an OC (object collider) for the current frame, allowing it to detect other OCs
 * If CollisionCheck_SAC is enabled, the collider will be inserted into the list at the specified index, otherwise it
 * will be inserted into the next slot
 */
s32 CollisionCheck_SetOC_SAC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider,
                             s32 index) {
    if (FrameAdvance_IsEnabled(play) == true) {
        return -1;
    }
    ASSERT(collider->shape <= COLSHAPE_QUAD);
    sOCResetFuncs[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->sacFlags & 1) {
        if (!(index < colChkCtx->colOCCount)) {
            // "You are trying to register a location that is larger than the total number of data."
            osSyncPrintf("CollisionCheck_setOC_SAC():全データ数より大きいところに登録しようとしている。\n");
            return -1;
        }
        //! @bug Should be colOC
        colChkCtx->colAT[index] = collider;
    } else {
        if (!(colChkCtx->colOCCount < COLLISION_CHECK_OC_MAX)) {
            // "Index exceeded and cannot add more"
            osSyncPrintf("CollisionCheck_setOC():インデックスがオーバして追加不能\n");
            return -1;
        }
        index = colChkCtx->colOCCount;
        colChkCtx->colOC[colChkCtx->colOCCount++] = collider;
    }
    return index;
}

/**
 * Sets a line as an OC collider for this frame.
 * OC lines are entirely unused, and do not even have collision check functions.
 */
s32 CollisionCheck_SetOCLine(PlayState* play, CollisionCheckContext* colChkCtx, OcLine* collider) {
    s32 index;

    if (FrameAdvance_IsEnabled(play) == true) {
        return -1;
    }
    Collider_ResetLineOC(play, collider);
    if (!(colChkCtx->colLineCount < COLLISION_CHECK_OC_LINE_MAX)) {
        // "Index exceeded and cannot add more"
        osSyncPrintf("CollisionCheck_setOCLine():インデックスがオーバして追加不能\n");
        return -1;
    }
    index = colChkCtx->colLineCount;
    colChkCtx->colLine[colChkCtx->colLineCount++] = collider;
    return index;
}

/**
 * Skips AT elements that are off.
 */
s32 CollisionCheck_SkipTouch(ColliderInfo* info) {
    if (!(info->toucherFlags & TOUCH_ON)) {
        return 1;
    }
    return 0;
}

/**
 * Skips AC elements that are off.
 */
s32 CollisionCheck_SkipBump(ColliderInfo* info) {
    if (!(info->bumperFlags & BUMP_ON)) {
        return 1;
    }
    return 0;
}

/**
 * If the AT element has no dmgFlags in common with the AC element, no collision happens.
 */
s32 CollisionCheck_NoSharedFlags(ColliderInfo* atInfo, ColliderInfo* acInfo) {
    if (!(atInfo->toucher.dmgFlags & acInfo->bumper.dmgFlags)) {
        return 1;
    }
    return 0;
}

/**
 * Spawns no blood drops.
 * Used by collider types HIT1, HIT3, HIT5, METAL, NONE, WOOD, HARD, and TREE
 */
void CollisionCheck_NoBlood(PlayState* play, Collider* collider, Vec3f* v) {
}

/**
 * Spawns blue blood drops.
 * Used by collider types HIT0 and HIT8.
 */
void CollisionCheck_BlueBlood(PlayState* play, Collider* collider, Vec3f* v) {
    static EffectSparkInit D_8015D8A0;
    s32 effectIndex;

    D_8015D8A0.position.x = v->x;
    D_8015D8A0.position.y = v->y;
    D_8015D8A0.position.z = v->z;
    D_8015D8A0.uDiv = 5;
    D_8015D8A0.vDiv = 5;
    D_8015D8A0.colorStart[0].r = 10;
    D_8015D8A0.colorStart[0].g = 10;
    D_8015D8A0.colorStart[0].b = 200;
    D_8015D8A0.colorStart[0].a = 255;
    D_8015D8A0.colorStart[1].r = 0;
    D_8015D8A0.colorStart[1].g = 0;
    D_8015D8A0.colorStart[1].b = 128;
    D_8015D8A0.colorStart[1].a = 255;
    D_8015D8A0.colorStart[2].r = 0;
    D_8015D8A0.colorStart[2].g = 0;
    D_8015D8A0.colorStart[2].b = 128;
    D_8015D8A0.colorStart[2].a = 255;
    D_8015D8A0.colorStart[3].r = 0;
    D_8015D8A0.colorStart[3].g = 0;
    D_8015D8A0.colorStart[3].b = 128;
    D_8015D8A0.colorStart[3].a = 255;
    D_8015D8A0.colorEnd[0].r = 0;
    D_8015D8A0.colorEnd[0].g = 0;
    D_8015D8A0.colorEnd[0].b = 32;
    D_8015D8A0.colorEnd[0].a = 0;
    D_8015D8A0.colorEnd[1].r = 0;
    D_8015D8A0.colorEnd[1].g = 0;
    D_8015D8A0.colorEnd[1].b = 32;
    D_8015D8A0.colorEnd[1].a = 0;
    D_8015D8A0.colorEnd[2].r = 0;
    D_8015D8A0.colorEnd[2].g = 0;
    D_8015D8A0.colorEnd[2].b = 64;
    D_8015D8A0.colorEnd[2].a = 0;
    D_8015D8A0.colorEnd[3].r = 0;
    D_8015D8A0.colorEnd[3].g = 0;
    D_8015D8A0.colorEnd[3].b = 64;
    D_8015D8A0.colorEnd[3].a = 0;
    D_8015D8A0.timer = 0;
    D_8015D8A0.duration = 16;
    D_8015D8A0.speed = 8.0f;
    D_8015D8A0.gravity = -1.0f;

    Effect_Add(play, &effectIndex, EFFECT_SPARK, 0, 1, &D_8015D8A0);
}

/**
 * Spawns green blood drops.
 * Used by collider types HIT2 and HIT6. No actor has type HIT2.
 */
void CollisionCheck_GreenBlood(PlayState* play, Collider* collider, Vec3f* v) {
    static EffectSparkInit D_8015DD68;
    s32 effectIndex;

    D_8015DD68.position.x = v->x;
    D_8015DD68.position.y = v->y;
    D_8015DD68.position.z = v->z;
    D_8015DD68.uDiv = 5;
    D_8015DD68.vDiv = 5;
    D_8015DD68.colorStart[0].r = 10;
    D_8015DD68.colorStart[0].g = 200;
    D_8015DD68.colorStart[0].b = 10;
    D_8015DD68.colorStart[0].a = 255;
    D_8015DD68.colorStart[1].r = 0;
    D_8015DD68.colorStart[1].g = 128;
    D_8015DD68.colorStart[1].b = 0;
    D_8015DD68.colorStart[1].a = 255;
    D_8015DD68.colorStart[2].r = 0;
    D_8015DD68.colorStart[2].g = 128;
    D_8015DD68.colorStart[2].b = 0;
    D_8015DD68.colorStart[2].a = 255;
    D_8015DD68.colorStart[3].r = 0;
    D_8015DD68.colorStart[3].g = 128;
    D_8015DD68.colorStart[3].b = 0;
    D_8015DD68.colorStart[3].a = 255;
    D_8015DD68.colorEnd[0].r = 0;
    D_8015DD68.colorEnd[0].g = 32;
    D_8015DD68.colorEnd[0].b = 0;
    D_8015DD68.colorEnd[0].a = 0;
    D_8015DD68.colorEnd[1].r = 0;
    D_8015DD68.colorEnd[1].g = 32;
    D_8015DD68.colorEnd[1].b = 0;
    D_8015DD68.colorEnd[1].a = 0;
    D_8015DD68.colorEnd[2].r = 0;
    D_8015DD68.colorEnd[2].g = 64;
    D_8015DD68.colorEnd[2].b = 0;
    D_8015DD68.colorEnd[2].a = 0;
    D_8015DD68.colorEnd[3].r = 0;
    D_8015DD68.colorEnd[3].g = 64;
    D_8015DD68.colorEnd[3].b = 0;
    D_8015DD68.colorEnd[3].a = 0;
    D_8015DD68.timer = 0;
    D_8015DD68.duration = 16;
    D_8015DD68.speed = 8.0f;
    D_8015DD68.gravity = -1.0f;

    Effect_Add(play, &effectIndex, EFFECT_SPARK, 0, 1, &D_8015DD68);
}

/**
 * Spawns a burst of water.
 * Used by collider type HIT4, which no actor has.
 */
void CollisionCheck_WaterBurst(PlayState* play, Collider* collider, Vec3f* pos) {
    EffectSsSibuki_SpawnBurst(play, pos);
    CollisionCheck_SpawnWaterDroplets(play, pos);
}

/**
 * Spawns red blood drops.
 * Used by collider type HIT7, which no actor has.
 */
void CollisionCheck_RedBlood(PlayState* play, Collider* collider, Vec3f* v) {
    CollisionCheck_SpawnRedBlood(play, v);
}

/**
 * Spawns red blood drops.
 * Unused.
 */
void CollisionCheck_RedBloodUnused(PlayState* play, Collider* collider, Vec3f* v) {
    CollisionCheck_SpawnRedBlood(play, v);
}

/**
 * Plays sound effects and displays hitmarks for solid-type AC colliders (METAL, WOOD, HARD, and TREE)
 */
void CollisionCheck_HitSolid(PlayState* play, ColliderInfo* info, Collider* collider, Vec3f* hitPos) {
    s32 flags = info->toucherFlags & TOUCH_SFX_NONE;

    if (flags == TOUCH_SFX_NORMAL && collider->colType != COLTYPE_METAL) {
        EffectSsHitMark_SpawnFixedScale(play, EFFECT_HITMARK_WHITE, hitPos);
        if (collider->actor == NULL) {
            Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_BOUND, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_BOUND, &collider->actor->projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    } else if (flags == TOUCH_SFX_NORMAL) { // collider->colType == COLTYPE_METAL
        EffectSsHitMark_SpawnFixedScale(play, EFFECT_HITMARK_METAL, hitPos);
        if (collider->actor == NULL) {
            CollisionCheck_SpawnShieldParticlesMetal(play, hitPos);
        } else {
            CollisionCheck_SpawnShieldParticlesMetalSound(play, hitPos, &collider->actor->projectedPos);
        }
    } else if (flags == TOUCH_SFX_HARD) {
        EffectSsHitMark_SpawnFixedScale(play, EFFECT_HITMARK_WHITE, hitPos);
        if (collider->actor == NULL) {
            Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_BOUND, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_BOUND, &collider->actor->projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    } else if (flags == TOUCH_SFX_WOOD) {
        EffectSsHitMark_SpawnFixedScale(play, EFFECT_HITMARK_DUST, hitPos);
        if (collider->actor == NULL) {
            Audio_PlaySoundGeneral(NA_SE_IT_REFLECTION_WOOD, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            Audio_PlaySoundGeneral(NA_SE_IT_REFLECTION_WOOD, &collider->actor->projectedPos, 4, &D_801333E0,
                                   &D_801333E0, &D_801333E8);
        }
    }
}

/**
 * Plays a hit sound effect for AT colliders attached to Player based on the AC element's elemType.
 */
s32 CollisionCheck_SwordHitAudio(Collider* at, ColliderInfo* acInfo) {
    if (at->actor != NULL && at->actor->category == ACTORCAT_PLAYER) {
        if (acInfo->elemType == ELEMTYPE_UNK0) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_STRIKE, &at->actor->projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        } else if (acInfo->elemType == ELEMTYPE_UNK1) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_STRIKE_HARD, &at->actor->projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        } else if (acInfo->elemType == ELEMTYPE_UNK2) {
            Audio_PlaySoundGeneral(NA_SE_PL_WALK_GROUND - SFX_FLAG, &at->actor->projectedPos, 4, &D_801333E0,
                                   &D_801333E0, &D_801333E8);
        } else if (acInfo->elemType == ELEMTYPE_UNK3) {
            Audio_PlaySoundGeneral(NA_SE_PL_WALK_GROUND - SFX_FLAG, &at->actor->projectedPos, 4, &D_801333E0,
                                   &D_801333E0, &D_801333E8);
        }
    }
    return 1;
}

static ColChkBloodFunc sBloodFuncs[] = {
    CollisionCheck_NoBlood,    CollisionCheck_BlueBlood, CollisionCheck_GreenBlood,
    CollisionCheck_WaterBurst, CollisionCheck_RedBlood,  CollisionCheck_RedBloodUnused,
};
static HitInfo sHitInfo[] = {
    { BLOOD_BLUE, HIT_WHITE }, { BLOOD_NONE, HIT_DUST },  { BLOOD_GREEN, HIT_DUST },  { BLOOD_NONE, HIT_WHITE },
    { BLOOD_WATER, HIT_NONE }, { BLOOD_NONE, HIT_RED },   { BLOOD_GREEN, HIT_WHITE }, { BLOOD_RED, HIT_WHITE },
    { BLOOD_BLUE, HIT_RED },   { BLOOD_NONE, HIT_SOLID }, { BLOOD_NONE, HIT_NONE },   { BLOOD_NONE, HIT_SOLID },
    { BLOOD_NONE, HIT_SOLID }, { BLOOD_NONE, HIT_WOOD },
};

/**
 * Handles hitmarks, blood, and sound effects for each AC collision, determined by the AC collider's colType
 */
void CollisionCheck_HitEffects(PlayState* play, Collider* at, ColliderInfo* atInfo, Collider* ac,
                               ColliderInfo* acInfo, Vec3f* hitPos) {
    if (acInfo->bumperFlags & BUMP_NO_HITMARK) {
        return;
    }
    if (!(atInfo->toucherFlags & TOUCH_AT_HITMARK) && atInfo->toucherFlags & TOUCH_DREW_HITMARK) {
        return;
    }
    if (ac->actor != NULL) {
        sBloodFuncs[sHitInfo[ac->colType].blood](play, ac, hitPos);
    }
    if (ac->actor != NULL) {
        if (sHitInfo[ac->colType].effect == HIT_SOLID) {
            CollisionCheck_HitSolid(play, atInfo, ac, hitPos);
        } else if (sHitInfo[ac->colType].effect == HIT_WOOD) {
            if (at->actor == NULL) {
                CollisionCheck_SpawnShieldParticles(play, hitPos);
                Audio_PlaySoundGeneral(NA_SE_IT_REFLECTION_WOOD, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                CollisionCheck_SpawnShieldParticlesWood(play, hitPos, &at->actor->projectedPos);
            }
        } else if (sHitInfo[ac->colType].effect != HIT_NONE) {
            EffectSsHitMark_SpawnFixedScale(play, sHitInfo[ac->colType].effect, hitPos);
            if (!(acInfo->bumperFlags & BUMP_NO_SWORD_SFX)) {
                CollisionCheck_SwordHitAudio(at, acInfo);
            }
        }
    } else {
        EffectSsHitMark_SpawnFixedScale(play, EFFECT_HITMARK_WHITE, hitPos);
        if (ac->actor == NULL) {
            Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_BOUND, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_BOUND, &ac->actor->projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
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
s32 CollisionCheck_SetATvsAC(PlayState* play, Collider* at, ColliderInfo* atInfo, Vec3f* atPos, Collider* ac,
                             ColliderInfo* acInfo, Vec3f* acPos, Vec3f* hitPos) {
    if (ac->acFlags & AC_HARD && at->actor != NULL && ac->actor != NULL) {
        CollisionCheck_SetBounce(at, ac);
    }
    if (!(acInfo->bumperFlags & BUMP_NO_AT_INFO)) {
        at->atFlags |= AT_HIT;
        at->at = ac->actor;
        atInfo->atHit = ac;
        atInfo->atHitInfo = acInfo;
        atInfo->toucherFlags |= TOUCH_HIT;
        if (at->actor != NULL) {
            at->actor->colChkInfo.atHitEffect = acInfo->bumper.effect;
        }
    }
    ac->acFlags |= AC_HIT;
    ac->ac = at->actor;
    acInfo->acHit = at;
    acInfo->acHitInfo = atInfo;
    acInfo->bumperFlags |= BUMP_HIT;
    if (ac->actor != NULL) {
        ac->actor->colChkInfo.acHitEffect = atInfo->toucher.effect;
    }
    acInfo->bumper.hitPos.x = hitPos->x;
    acInfo->bumper.hitPos.y = hitPos->y;
    acInfo->bumper.hitPos.z = hitPos->z;
    if (!(atInfo->toucherFlags & TOUCH_AT_HITMARK) && ac->colType != COLTYPE_METAL && ac->colType != COLTYPE_WOOD &&
        ac->colType != COLTYPE_HARD) {
        acInfo->bumperFlags |= BUMP_DRAW_HITMARK;
    } else {
        CollisionCheck_HitEffects(play, at, atInfo, ac, acInfo, hitPos);
        atInfo->toucherFlags |= TOUCH_DREW_HITMARK;
    }
    return 1;
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_JntSphVsJntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                      Collider* colAC) {
    ColliderJntSph* at = (ColliderJntSph*)colAT;
    ColliderJntSphElement* atItem;
    ColliderJntSph* ac = (ColliderJntSph*)colAC;
    ColliderJntSphElement* acElem;
    f32 overlapSize;
    f32 centerDist;

    if (at->count > 0 && at->elements != NULL && ac->count > 0 && ac->elements != NULL) {
        for (atItem = at->elements; atItem < at->elements + at->count; atItem++) {
            if (CollisionCheck_SkipTouch(&atItem->info) == 1) {
                continue;
            }
            for (acElem = ac->elements; acElem < ac->elements + ac->count; acElem++) {
                if (CollisionCheck_SkipBump(&acElem->info) == 1) {
                    continue;
                }
                if (CollisionCheck_NoSharedFlags(&atItem->info, &acElem->info) == 1) {
                    continue;
                }
                if (Math3D_SphVsSphOverlapCenter(&atItem->dim.worldSphere, &acElem->dim.worldSphere, &overlapSize,
                                                 &centerDist) == 1) {
                    f32 acToHit;
                    Vec3f hitPos;
                    Vec3f atPos;
                    Vec3f acPos;

                    atPos.x = atItem->dim.worldSphere.center.x;
                    atPos.y = atItem->dim.worldSphere.center.y;
                    atPos.z = atItem->dim.worldSphere.center.z;
                    acPos.x = acElem->dim.worldSphere.center.x;
                    acPos.y = acElem->dim.worldSphere.center.y;
                    acPos.z = acElem->dim.worldSphere.center.z;
                    if (!IS_ZERO(centerDist)) {
                        acToHit = acElem->dim.worldSphere.radius / centerDist;
                        hitPos.x = (((atPos.x - acPos.x) * acToHit) + acPos.x);
                        hitPos.y = (((atPos.y - acPos.y) * acToHit) + acPos.y);
                        hitPos.z = (((atPos.z - acPos.z) * acToHit) + acPos.z);
                    } else {
                        Math_Vec3f_Copy(&hitPos, &atPos);
                    }
                    CollisionCheck_SetATvsAC(play, &at->base, &atItem->info, &atPos, &ac->base, &acElem->info,
                                             &acPos, &hitPos);
                    if (!(ac->base.ocFlags2 & OC2_FIRST_ONLY)) {
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
void CollisionCheck_AC_JntSphVsCyl(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                   Collider* colAC) {
    ColliderJntSph* at = (ColliderJntSph*)colAT;
    ColliderJntSphElement* atItem;
    ColliderCylinder* ac = (ColliderCylinder*)colAC;
    f32 overlapSize;
    f32 centerDist;

    if (at->count > 0 && at->elements != NULL && ac->dim.radius > 0 && ac->dim.height > 0) {
        if (CollisionCheck_SkipBump(&ac->info) == 1) {
            return;
        }
        for (atItem = at->elements; atItem < at->elements + at->count; atItem++) {
            if (CollisionCheck_SkipTouch(&atItem->info) == 1) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atItem->info, &ac->info) == 1) {
                continue;
            }
            if (Math3D_SphVsCylOverlapCenterDist(&atItem->dim.worldSphere, &ac->dim, &overlapSize, &centerDist)) {
                Vec3f hitPos;
                Vec3f atPos;
                Vec3f acPos;
                f32 acToHit;

                atPos.x = atItem->dim.worldSphere.center.x;
                atPos.y = atItem->dim.worldSphere.center.y;
                atPos.z = atItem->dim.worldSphere.center.z;
                acPos.x = ac->dim.pos.x;
                acPos.y = ac->dim.pos.y;
                acPos.z = ac->dim.pos.z;
                if (!IS_ZERO(centerDist)) {
                    acToHit = ac->dim.radius / centerDist;
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
                CollisionCheck_SetATvsAC(play, &at->base, &atItem->info, &atPos, &ac->base, &ac->info, &acPos,
                                         &hitPos);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_CylVsJntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                   Collider* colAC) {
    ColliderCylinder* at = (ColliderCylinder*)colAT;
    ColliderJntSph* ac = (ColliderJntSph*)colAC;
    f32 overlapSize;
    f32 centerDist;
    ColliderJntSphElement* acElem;

    if (ac->count > 0 && ac->elements != NULL && at->dim.radius > 0 && at->dim.height > 0) {
        if (CollisionCheck_SkipTouch(&at->info) == 1) {
            return;
        }
        for (acElem = ac->elements; acElem < ac->elements + ac->count; acElem++) {
            if (CollisionCheck_SkipBump(&acElem->info) == 1) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&at->info, &acElem->info) == 1) {
                continue;
            }
            if (Math3D_SphVsCylOverlapCenterDist(&acElem->dim.worldSphere, &at->dim, &overlapSize, &centerDist)) {
                Vec3f hitPos;
                Vec3f atPos;
                Vec3f acPos;
                f32 acToHit;

                atPos.x = at->dim.pos.x;
                atPos.y = at->dim.pos.y;
                atPos.z = at->dim.pos.z;
                acPos.x = acElem->dim.worldSphere.center.x;
                acPos.y = acElem->dim.worldSphere.center.y;
                acPos.z = acElem->dim.worldSphere.center.z;
                if (!IS_ZERO(centerDist)) {
                    acToHit = acElem->dim.worldSphere.radius / centerDist;
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
                CollisionCheck_SetATvsAC(play, &at->base, &at->info, &atPos, &ac->base, &acElem->info, &acPos,
                                         &hitPos);
                if (!(ac->base.ocFlags2 & OC2_FIRST_ONLY)) {
                    break;
                }
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_JntSphVsTris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                    Collider* colAC) {
    ColliderJntSph* at = (ColliderJntSph*)colAT;
    ColliderJntSphElement* atSph;
    ColliderTris* ac = (ColliderTris*)colAC;
    ColliderTrisElement* acTri;
    Vec3f hitPos;

    if (at->count > 0 && at->elements != NULL && ac->count > 0 && ac->elements != NULL) {
        for (atSph = at->elements; atSph < at->elements + at->count; atSph++) {
            if (CollisionCheck_SkipTouch(&atSph->info) == 1) {
                continue;
            }
            for (acTri = ac->elements; acTri < ac->elements + ac->count; acTri++) {
                if (CollisionCheck_SkipBump(&acTri->info) == 1) {
                    continue;
                }
                if (CollisionCheck_NoSharedFlags(&atSph->info, &acTri->info) == 1) {
                    continue;
                }
                if (Math3D_TriVsSphIntersect(&atSph->dim.worldSphere, &acTri->dim, &hitPos) == 1) {
                    Vec3f atPos;
                    Vec3f acPos;

                    atPos.x = atSph->dim.worldSphere.center.x;
                    atPos.y = atSph->dim.worldSphere.center.y;
                    atPos.z = atSph->dim.worldSphere.center.z;
                    acPos.x = (acTri->dim.vtx[0].x + acTri->dim.vtx[1].x + acTri->dim.vtx[2].x) * (1.0f / 3);
                    acPos.y = (acTri->dim.vtx[0].y + acTri->dim.vtx[1].y + acTri->dim.vtx[2].y) * (1.0f / 3);
                    acPos.z = (acTri->dim.vtx[0].z + acTri->dim.vtx[1].z + acTri->dim.vtx[2].z) * (1.0f / 3);
                    CollisionCheck_SetATvsAC(play, &at->base, &atSph->info, &atPos, &ac->base, &acTri->info,
                                             &acPos, &hitPos);
                    return;
                }
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_TrisVsJntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                    Collider* colAC) {
    ColliderTris* at = (ColliderTris*)colAT;
    ColliderTrisElement* atItem;
    ColliderJntSph* ac = (ColliderJntSph*)colAC;
    ColliderJntSphElement* acElem;
    Vec3f hitPos;

    if (ac->count > 0 && ac->elements != NULL && at->count > 0 && at->elements != NULL) {
        for (acElem = ac->elements; acElem < ac->elements + ac->count; acElem++) {
            if (CollisionCheck_SkipBump(&acElem->info) == 1) {
                continue;
            }
            for (atItem = at->elements; atItem < at->elements + at->count; atItem++) {
                if (CollisionCheck_SkipTouch(&atItem->info) == 1) {
                    continue;
                }
                if (CollisionCheck_NoSharedFlags(&atItem->info, &acElem->info) == 1) {
                    continue;
                }
                if (Math3D_TriVsSphIntersect(&acElem->dim.worldSphere, &atItem->dim, &hitPos) == 1) {
                    Vec3f atPos;
                    Vec3f acPos;

                    Math_Vec3s_ToVec3f(&acPos, &acElem->dim.worldSphere.center);
                    atPos.x = (atItem->dim.vtx[0].x + atItem->dim.vtx[1].x + atItem->dim.vtx[2].x) * (1.0f / 3);
                    atPos.y = (atItem->dim.vtx[0].y + atItem->dim.vtx[1].y + atItem->dim.vtx[2].y) * (1.0f / 3);
                    atPos.z = (atItem->dim.vtx[0].z + atItem->dim.vtx[1].z + atItem->dim.vtx[2].z) * (1.0f / 3);
                    CollisionCheck_SetATvsAC(play, &at->base, &atItem->info, &atPos, &ac->base, &acElem->info,
                                             &acPos, &hitPos);
                    if (!(ac->base.ocFlags2 & OC2_FIRST_ONLY)) {
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
void CollisionCheck_AC_JntSphVsQuad(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                    Collider* colAC) {
    static TriNorm D_8015E230;
    static TriNorm D_8015E268;
    ColliderJntSph* at = (ColliderJntSph*)colAT;
    ColliderQuad* ac = (ColliderQuad*)colAC;
    Vec3f hitPos;
    ColliderJntSphElement* atItem;

    if (at->count > 0 && at->elements != NULL) {
        if (CollisionCheck_SkipBump(&ac->info) == 1) {
            return;
        }
        Math3D_TriNorm(&D_8015E230, &ac->dim.quad[2], &ac->dim.quad[3], &ac->dim.quad[1]);
        Math3D_TriNorm(&D_8015E268, &ac->dim.quad[1], &ac->dim.quad[0], &ac->dim.quad[2]);
        for (atItem = at->elements; atItem < at->elements + at->count; atItem++) {
            if (CollisionCheck_SkipTouch(&atItem->info) == 1) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atItem->info, &ac->info) == 1) {
                continue;
            }
            if (Math3D_TriVsSphIntersect(&atItem->dim.worldSphere, &D_8015E230, &hitPos) == 1 ||
                Math3D_TriVsSphIntersect(&atItem->dim.worldSphere, &D_8015E268, &hitPos) == 1) {
                Vec3f atPos;
                Vec3f acPos;

                Math_Vec3s_ToVec3f(&atPos, &atItem->dim.worldSphere.center);

                acPos.x = (ac->dim.quad[0].x + (ac->dim.quad[1].x + (ac->dim.quad[3].x + ac->dim.quad[2].x))) / 4.0f;
                acPos.y = (ac->dim.quad[0].y + (ac->dim.quad[1].y + (ac->dim.quad[3].y + ac->dim.quad[2].y))) / 4.0f;
                acPos.z = (ac->dim.quad[0].z + (ac->dim.quad[1].z + (ac->dim.quad[3].z + ac->dim.quad[2].z))) / 4.0f;

                CollisionCheck_SetATvsAC(play, &at->base, &atItem->info, &atPos, &ac->base, &ac->info, &acPos,
                                         &hitPos);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_QuadVsJntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                    Collider* colAC) {
    static TriNorm D_8015E2A0;
    static TriNorm D_8015E2D8;
    ColliderJntSph* ac = (ColliderJntSph*)colAC;
    Vec3f hitPos;
    ColliderQuad* at = (ColliderQuad*)colAT;
    ColliderJntSphElement* acElem;

    if (ac->count > 0 && ac->elements != NULL) {
        if (CollisionCheck_SkipTouch(&at->info) == 1) {
            return;
        }
        Math3D_TriNorm(&D_8015E2A0, &at->dim.quad[2], &at->dim.quad[3], &at->dim.quad[1]);
        Math3D_TriNorm(&D_8015E2D8, &at->dim.quad[2], &at->dim.quad[1], &at->dim.quad[0]);
        for (acElem = ac->elements; acElem < ac->elements + ac->count; acElem++) {
            if (CollisionCheck_SkipBump(&acElem->info) == 1) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&at->info, &acElem->info) == 1) {
                continue;
            }
            if (Math3D_TriVsSphIntersect(&acElem->dim.worldSphere, &D_8015E2A0, &hitPos) == 1 ||
                Math3D_TriVsSphIntersect(&acElem->dim.worldSphere, &D_8015E2D8, &hitPos) == 1) {
                if (Collider_QuadSetNearestAC(play, at, &hitPos)) {
                    Vec3f atPos;
                    Vec3f acPos;

                    acPos.x = acElem->dim.worldSphere.center.x;
                    acPos.y = acElem->dim.worldSphere.center.y;
                    acPos.z = acElem->dim.worldSphere.center.z;

                    atPos.x =
                        (at->dim.quad[0].x + (at->dim.quad[1].x + (at->dim.quad[3].x + at->dim.quad[2].x))) / 4.0f;
                    atPos.y =
                        (at->dim.quad[0].y + (at->dim.quad[1].y + (at->dim.quad[3].y + at->dim.quad[2].y))) / 4.0f;
                    atPos.z =
                        (at->dim.quad[0].z + (at->dim.quad[1].z + (at->dim.quad[3].z + at->dim.quad[2].z))) / 4.0f;
                    CollisionCheck_SetATvsAC(play, &at->base, &at->info, &atPos, &ac->base, &acElem->info, &acPos,
                                             &hitPos);
                    if (!(ac->base.ocFlags2 & OC2_FIRST_ONLY)) {
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
void CollisionCheck_AC_CylVsCyl(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                Collider* colAC) {
    ColliderCylinder* at = (ColliderCylinder*)colAT;
    ColliderCylinder* ac = (ColliderCylinder*)colAC;
    f32 deadSpace;
    f32 centerDistXZ;
    Vec3f hitPos;

    if (at->dim.radius > 0 && at->dim.height > 0 && ac->dim.radius > 0 && ac->dim.height > 0) {
        if (CollisionCheck_SkipBump(&ac->info) == 1) {
            return;
        }
        if (CollisionCheck_SkipTouch(&at->info) == 1) {
            return;
        }
        if (CollisionCheck_NoSharedFlags(&at->info, &ac->info) == 1) {
            return;
        }
        if (Math3D_CylOutsideCylDist(&at->dim, &ac->dim, &deadSpace, &centerDistXZ) == 1) {
            Vec3f atPos;
            Vec3f acPos;
            f32 acToHit;

            Math_Vec3s_ToVec3f(&atPos, &at->dim.pos);
            Math_Vec3s_ToVec3f(&acPos, &ac->dim.pos);
            if (!IS_ZERO(centerDistXZ)) {
                acToHit = ac->dim.radius / centerDistXZ;
                hitPos.y = (f32)ac->dim.pos.y + ac->dim.yShift + ac->dim.height * 0.5f;
                hitPos.x = ((f32)at->dim.pos.x - ac->dim.pos.x) * acToHit + ac->dim.pos.x;
                hitPos.z = ((f32)at->dim.pos.z - ac->dim.pos.z) * acToHit + ac->dim.pos.z;
            } else {
                Math_Vec3s_ToVec3f(&hitPos, &ac->dim.pos);
            }
            CollisionCheck_SetATvsAC(play, &at->base, &at->info, &atPos, &ac->base, &ac->info, &acPos, &hitPos);
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_CylVsTris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                 Collider* colAC) {
    ColliderCylinder* at = (ColliderCylinder*)colAT;
    ColliderTris* ac = (ColliderTris*)colAC;
    ColliderTrisElement* acElem;
    Vec3f hitPos;

    if (at->dim.radius > 0 && at->dim.height > 0 && ac->count > 0 && ac->elements != NULL) {
        if (CollisionCheck_SkipTouch(&at->info) == 1) {
            return;
        }
        for (acElem = ac->elements; acElem < ac->elements + ac->count; acElem++) {
            if (CollisionCheck_SkipBump(&acElem->info) == 1) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&at->info, &acElem->info) == 1) {
                continue;
            }
            if (Math3D_CylTriVsIntersect(&at->dim, &acElem->dim, &hitPos) == 1) {
                Vec3f atpos;
                Vec3f acPos;

                Math_Vec3s_ToVec3f(&atpos, &at->dim.pos);

                acPos.x = (acElem->dim.vtx[0].x + acElem->dim.vtx[1].x + acElem->dim.vtx[2].x) * (1.0f / 3);
                acPos.y = (acElem->dim.vtx[0].y + acElem->dim.vtx[1].y + acElem->dim.vtx[2].y) * (1.0f / 3);
                acPos.z = (acElem->dim.vtx[0].z + acElem->dim.vtx[1].z + acElem->dim.vtx[2].z) * (1.0f / 3);
                CollisionCheck_SetATvsAC(play, &at->base, &at->info, &atpos, &ac->base, &acElem->info, &acPos,
                                         &hitPos);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_TrisVsCyl(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                 Collider* colAC) {
    static Vec3f D_8015E310;
    ColliderTris* at = (ColliderTris*)colAT;
    ColliderTrisElement* atItem;
    ColliderCylinder* ac = (ColliderCylinder*)colAC;
    Vec3f atPos;
    Vec3f acPos;

    if (ac->dim.radius > 0 && ac->dim.height > 0 && at->count > 0 && at->elements != NULL) {
        if (CollisionCheck_SkipBump(&ac->info) == 1) {
            return;
        }
        for (atItem = at->elements; atItem < at->elements + at->count; atItem++) {
            if (CollisionCheck_SkipTouch(&atItem->info) == 1) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atItem->info, &ac->info) == 1) {
                continue;
            }

            if (Math3D_CylTriVsIntersect(&ac->dim, &atItem->dim, &D_8015E310) == 1) {
                atPos.x = (atItem->dim.vtx[0].x + atItem->dim.vtx[1].x + atItem->dim.vtx[2].x) * (1.0f / 3);
                atPos.y = (atItem->dim.vtx[0].y + atItem->dim.vtx[1].y + atItem->dim.vtx[2].y) * (1.0f / 3);
                atPos.z = (atItem->dim.vtx[0].z + atItem->dim.vtx[1].z + atItem->dim.vtx[2].z) * (1.0f / 3);
                Math_Vec3s_ToVec3f(&acPos, &ac->dim.pos);
                CollisionCheck_SetATvsAC(play, &at->base, &atItem->info, &atPos, &ac->base, &ac->info, &acPos,
                                         &D_8015E310);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_CylVsQuad(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                 Collider* colAC) {
    static TriNorm D_8015E320;
    static TriNorm D_8015E358;
    static Vec3f D_8015E390;
    ColliderCylinder* at = (ColliderCylinder*)colAT;
    ColliderQuad* ac = (ColliderQuad*)colAC;

    if (at->dim.height > 0 && at->dim.radius > 0) {
        if (CollisionCheck_SkipTouch(&at->info) == 1 || CollisionCheck_SkipBump(&ac->info) == 1) {
            return;
        }
        if (CollisionCheck_NoSharedFlags(&at->info, &ac->info) == 1) {
            return;
        }
        Math3D_TriNorm(&D_8015E320, &ac->dim.quad[2], &ac->dim.quad[3], &ac->dim.quad[1]);
        Math3D_TriNorm(&D_8015E358, &ac->dim.quad[1], &ac->dim.quad[0], &ac->dim.quad[2]);
        if (Math3D_CylTriVsIntersect(&at->dim, &D_8015E320, &D_8015E390) == 1) {
            Vec3f atPos1;
            Vec3f acPos1;

            Math_Vec3s_ToVec3f(&atPos1, &at->dim.pos);
            acPos1.x = (ac->dim.quad[0].x + (ac->dim.quad[1].x + (ac->dim.quad[3].x + ac->dim.quad[2].x))) / 4.0f;
            acPos1.y = (ac->dim.quad[0].y + (ac->dim.quad[1].y + (ac->dim.quad[3].y + ac->dim.quad[2].y))) / 4.0f;
            acPos1.z = (ac->dim.quad[0].z + (ac->dim.quad[1].z + (ac->dim.quad[3].z + ac->dim.quad[2].z))) / 4.0f;
            CollisionCheck_SetATvsAC(play, &at->base, &at->info, &atPos1, &ac->base, &ac->info, &acPos1,
                                     &D_8015E390);
        } else if (Math3D_CylTriVsIntersect(&at->dim, &D_8015E358, &D_8015E390) == 1) {
            Vec3f atPos2;
            Vec3f acPos2;

            Math_Vec3s_ToVec3f(&atPos2, &at->dim.pos);
            acPos2.x = (ac->dim.quad[0].x + (ac->dim.quad[1].x + (ac->dim.quad[3].x + ac->dim.quad[2].x))) / 4.0f;
            acPos2.y = (ac->dim.quad[0].y + (ac->dim.quad[1].y + (ac->dim.quad[3].y + ac->dim.quad[2].y))) / 4.0f;
            acPos2.z = (ac->dim.quad[0].z + (ac->dim.quad[1].z + (ac->dim.quad[3].z + ac->dim.quad[2].z))) / 4.0f;
            CollisionCheck_SetATvsAC(play, &at->base, &at->info, &atPos2, &ac->base, &ac->info, &acPos2,
                                     &D_8015E390);
        }
    }
}

static s8 sBssDummy0;
static s8 sBssDummy1;

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_QuadVsCyl(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                 Collider* colAC) {
    static TriNorm D_8015E3A0;
    static TriNorm D_8015E3D8;
    static Vec3f D_8015E410;
    ColliderQuad* at = (ColliderQuad*)colAT;
    ColliderCylinder* ac = (ColliderCylinder*)colAC;

    if (ac->dim.height > 0 && ac->dim.radius > 0) {
        if (CollisionCheck_SkipBump(&ac->info) == 1 || CollisionCheck_SkipTouch(&at->info) == 1) {
            return;
        }
        if (CollisionCheck_NoSharedFlags(&at->info, &ac->info) == 1) {
            return;
        }
        Math3D_TriNorm(&D_8015E3A0, &at->dim.quad[2], &at->dim.quad[3], &at->dim.quad[1]);
        Math3D_TriNorm(&D_8015E3D8, &at->dim.quad[2], &at->dim.quad[1], &at->dim.quad[0]);
        if (Math3D_CylTriVsIntersect(&ac->dim, &D_8015E3A0, &D_8015E410) == 1) {
            if (Collider_QuadSetNearestAC(play, at, &D_8015E410)) {
                Vec3f atPos1;
                Vec3f acPos1;

                atPos1.x = (at->dim.quad[0].x + (at->dim.quad[1].x + (at->dim.quad[3].x + at->dim.quad[2].x))) / 4.0f;
                atPos1.y = (at->dim.quad[0].y + (at->dim.quad[1].y + (at->dim.quad[3].y + at->dim.quad[2].y))) / 4.0f;
                atPos1.z = (at->dim.quad[0].z + (at->dim.quad[1].z + (at->dim.quad[3].z + at->dim.quad[2].z))) / 4.0f;
                Math_Vec3s_ToVec3f(&acPos1, &ac->dim.pos);
                CollisionCheck_SetATvsAC(play, &at->base, &at->info, &atPos1, &ac->base, &ac->info, &acPos1,
                                         &D_8015E410);
                return;
            }
        }
        if (Math3D_CylTriVsIntersect(&ac->dim, &D_8015E3D8, &D_8015E410) == 1) {
            if (Collider_QuadSetNearestAC(play, at, &D_8015E410)) {
                Vec3f atPos2;
                Vec3f acPos2;

                atPos2.x = (at->dim.quad[0].x + (at->dim.quad[1].x + (at->dim.quad[3].x + at->dim.quad[2].x))) / 4.0f;
                atPos2.y = (at->dim.quad[0].y + (at->dim.quad[1].y + (at->dim.quad[3].y + at->dim.quad[2].y))) / 4.0f;
                atPos2.z = (at->dim.quad[0].z + (at->dim.quad[1].z + (at->dim.quad[3].z + at->dim.quad[2].z))) / 4.0f;
                Math_Vec3s_ToVec3f(&acPos2, &ac->dim.pos);
                CollisionCheck_SetATvsAC(play, &at->base, &at->info, &atPos2, &ac->base, &ac->info, &acPos2,
                                         &D_8015E410);
            }
        }
    }
}

static s8 sBssDummy3;
static s8 sBssDummy4;
static s8 sBssDummy5;
static s8 sBssDummy6;

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_TrisVsTris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                  Collider* colAC) {
    static Vec3f D_8015E420;
    ColliderTris* at = (ColliderTris*)colAT;
    ColliderTrisElement* atItem;
    ColliderTris* ac = (ColliderTris*)colAC;
    ColliderTrisElement* acElem;

    if (ac->count > 0 && ac->elements != NULL && at->count > 0 && at->elements != NULL) {
        for (acElem = ac->elements; acElem < ac->elements + ac->count; acElem++) {
            if (CollisionCheck_SkipBump(&acElem->info) == 1) {
                continue;
            }
            for (atItem = at->elements; atItem < at->elements + at->count; atItem++) {
                if (CollisionCheck_SkipTouch(&atItem->info) == 1) {
                    continue;
                }
                if (CollisionCheck_NoSharedFlags(&atItem->info, &acElem->info) == 1) {
                    continue;
                }
                if (Math3D_TriVsTriIntersect(&atItem->dim, &acElem->dim, &D_8015E420) == 1) {
                    Vec3f atPos;
                    Vec3f acPos;

                    atPos.x = (atItem->dim.vtx[0].x + atItem->dim.vtx[1].x + atItem->dim.vtx[2].x) * (1.0f / 3);
                    atPos.y = (atItem->dim.vtx[0].y + atItem->dim.vtx[1].y + atItem->dim.vtx[2].y) * (1.0f / 3);
                    atPos.z = (atItem->dim.vtx[0].z + atItem->dim.vtx[1].z + atItem->dim.vtx[2].z) * (1.0f / 3);
                    acPos.x = (acElem->dim.vtx[0].x + acElem->dim.vtx[1].x + acElem->dim.vtx[2].x) * (1.0f / 3);
                    acPos.y = (acElem->dim.vtx[0].y + acElem->dim.vtx[1].y + acElem->dim.vtx[2].y) * (1.0f / 3);
                    acPos.z = (acElem->dim.vtx[0].z + acElem->dim.vtx[1].z + acElem->dim.vtx[2].z) * (1.0f / 3);
                    CollisionCheck_SetATvsAC(play, &at->base, &atItem->info, &atPos, &ac->base, &acElem->info,
                                             &acPos, &D_8015E420);
                    return;
                }
            }
        }
    }
}

static s8 sBssDummy7;
static s8 sBssDummy8;
static s8 sBssDummy9;
static s8 sBssDummy10;

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_TrisVsQuad(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                  Collider* colAC) {
    static Vec3f D_8015E430;
    static TriNorm D_8015E440;
    static TriNorm D_8015E478;
    ColliderTris* at = (ColliderTris*)colAT;
    ColliderTrisElement* atItem;
    ColliderQuad* ac = (ColliderQuad*)colAC;

    if (at->count > 0 && at->elements != NULL) {
        if (CollisionCheck_SkipBump(&ac->info) == 1) {
            return;
        }
        Math3D_TriNorm(&D_8015E440, &ac->dim.quad[2], &ac->dim.quad[3], &ac->dim.quad[1]);
        Math3D_TriNorm(&D_8015E478, &ac->dim.quad[1], &ac->dim.quad[0], &ac->dim.quad[2]);
        for (atItem = at->elements; atItem < at->elements + at->count; atItem++) {
            if (CollisionCheck_SkipTouch(&atItem->info) == 1) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&atItem->info, &ac->info) == 1) {
                continue;
            }
            if (Math3D_TriVsTriIntersect(&D_8015E440, &atItem->dim, &D_8015E430) == 1 ||
                Math3D_TriVsTriIntersect(&D_8015E478, &atItem->dim, &D_8015E430) == 1) {
                Vec3f atPos;
                Vec3f acPos;

                atPos.x = (atItem->dim.vtx[0].x + atItem->dim.vtx[1].x + atItem->dim.vtx[2].x) * (1.0f / 3);
                atPos.y = (atItem->dim.vtx[0].y + atItem->dim.vtx[1].y + atItem->dim.vtx[2].y) * (1.0f / 3);
                atPos.z = (atItem->dim.vtx[0].z + atItem->dim.vtx[1].z + atItem->dim.vtx[2].z) * (1.0f / 3);
                acPos.x = (ac->dim.quad[0].x + (ac->dim.quad[1].x + (ac->dim.quad[3].x + ac->dim.quad[2].x))) / 4.0f;
                acPos.y = (ac->dim.quad[0].y + (ac->dim.quad[1].y + (ac->dim.quad[3].y + ac->dim.quad[2].y))) / 4.0f;
                acPos.z = (ac->dim.quad[0].z + (ac->dim.quad[1].z + (ac->dim.quad[3].z + ac->dim.quad[2].z))) / 4.0f;
                CollisionCheck_SetATvsAC(play, &at->base, &atItem->info, &atPos, &ac->base, &ac->info, &acPos,
                                         &D_8015E430);
                return;
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_QuadVsTris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                  Collider* colAC) {
    static Vec3f D_8015E4B0;
    static TriNorm D_8015E4C0;
    static TriNorm D_8015E4F8;
    ColliderQuad* at = (ColliderQuad*)colAT;
    ColliderTris* ac = (ColliderTris*)colAC;
    ColliderTrisElement* acElem;

    if (ac->count > 0 && ac->elements != NULL) {
        if (CollisionCheck_SkipTouch(&at->info) == 1) {
            return;
        }
        Math3D_TriNorm(&D_8015E4C0, &at->dim.quad[2], &at->dim.quad[3], &at->dim.quad[1]);
        Math3D_TriNorm(&D_8015E4F8, &at->dim.quad[1], &at->dim.quad[0], &at->dim.quad[2]);
        for (acElem = ac->elements; acElem < ac->elements + ac->count; acElem++) {
            if (CollisionCheck_SkipBump(&acElem->info) == 1) {
                continue;
            }
            if (CollisionCheck_NoSharedFlags(&at->info, &acElem->info) == 1) {
                continue;
            }
            if (Math3D_TriVsTriIntersect(&D_8015E4C0, &acElem->dim, &D_8015E4B0) == 1 ||
                Math3D_TriVsTriIntersect(&D_8015E4F8, &acElem->dim, &D_8015E4B0) == 1) {
                if (Collider_QuadSetNearestAC(play, at, &D_8015E4B0)) {
                    Vec3f atPos;
                    Vec3f acPos;

                    acPos.x = (acElem->dim.vtx[0].x + acElem->dim.vtx[1].x + acElem->dim.vtx[2].x) * (1.0f / 3);
                    acPos.y = (acElem->dim.vtx[0].y + acElem->dim.vtx[1].y + acElem->dim.vtx[2].y) * (1.0f / 3);
                    acPos.z = (acElem->dim.vtx[0].z + acElem->dim.vtx[1].z + acElem->dim.vtx[2].z) * (1.0f / 3);
                    atPos.x =
                        (at->dim.quad[0].x + (at->dim.quad[1].x + (at->dim.quad[3].x + at->dim.quad[2].x))) / 4.0f;
                    atPos.y =
                        (at->dim.quad[0].y + (at->dim.quad[1].y + (at->dim.quad[3].y + at->dim.quad[2].y))) / 4.0f;
                    atPos.z =
                        (at->dim.quad[0].z + (at->dim.quad[1].z + (at->dim.quad[3].z + at->dim.quad[2].z))) / 4.0f;
                    CollisionCheck_SetATvsAC(play, &at->base, &at->info, &atPos, &ac->base, &acElem->info, &acPos,
                                             &D_8015E4B0);
                    return;
                }
            }
        }
    }
}

/**
 * AC overlap check. Calculates the center of each collider element and the point of contact.
 */
void CollisionCheck_AC_QuadVsQuad(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT,
                                  Collider* colAC) {
    static TriNorm D_8015E530[2];
    static Vec3f D_8015E598;
    static TriNorm D_8015E5A8[2];
    ColliderQuad* at = (ColliderQuad*)colAT;
    ColliderQuad* ac = (ColliderQuad*)colAC;
    s32 i;
    s32 j;

    if (CollisionCheck_SkipTouch(&at->info) == 1) {
        return;
    }
    if (CollisionCheck_SkipBump(&ac->info) == 1) {
        return;
    }
    if (CollisionCheck_NoSharedFlags(&at->info, &ac->info) == 1) {
        return;
    }

    Math3D_TriNorm(&D_8015E5A8[0], &at->dim.quad[2], &at->dim.quad[3], &at->dim.quad[1]);
    Math3D_TriNorm(&D_8015E5A8[1], &at->dim.quad[2], &at->dim.quad[1], &at->dim.quad[0]);
    Math3D_TriNorm(&D_8015E530[0], &ac->dim.quad[2], &ac->dim.quad[3], &ac->dim.quad[1]);
    Math3D_TriNorm(&D_8015E530[1], &ac->dim.quad[2], &ac->dim.quad[1], &ac->dim.quad[0]);

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (Math3D_TriVsTriIntersect(&D_8015E5A8[j], &D_8015E530[i], &D_8015E598) == 1) {
                if (Collider_QuadSetNearestAC(play, at, &D_8015E598)) {
                    Vec3f atPos;
                    Vec3f acPos;

                    atPos.x =
                        (at->dim.quad[0].x + (at->dim.quad[1].x + (at->dim.quad[3].x + at->dim.quad[2].x))) / 4.0f;
                    atPos.y =
                        (at->dim.quad[0].y + (at->dim.quad[1].y + (at->dim.quad[3].y + at->dim.quad[2].y))) / 4.0f;
                    atPos.z =
                        (at->dim.quad[0].z + (at->dim.quad[1].z + (at->dim.quad[3].z + at->dim.quad[2].z))) / 4.0f;
                    acPos.x =
                        (ac->dim.quad[0].x + (ac->dim.quad[1].x + (ac->dim.quad[3].x + ac->dim.quad[2].x))) / 4.0f;
                    acPos.y =
                        (ac->dim.quad[0].y + (ac->dim.quad[1].y + (ac->dim.quad[3].y + ac->dim.quad[2].y))) / 4.0f;
                    acPos.z =
                        (ac->dim.quad[0].z + (ac->dim.quad[1].z + (ac->dim.quad[3].z + ac->dim.quad[2].z))) / 4.0f;
                    CollisionCheck_SetATvsAC(play, &at->base, &at->info, &atPos, &ac->base, &ac->info, &acPos,
                                             &D_8015E598);
                    return;
                }
            }
        }
    }
}

/**
 * Sets a ColliderJntSph's hit effects
 */
void CollisionCheck_SetJntSphHitFX(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    ColliderJntSph* jntSph = (ColliderJntSph*)collider;
    ColliderJntSphElement* element;
    Vec3f hitPos;

    for (element = jntSph->elements; element < jntSph->elements + jntSph->count; element++) {
        if ((element->info.bumperFlags & BUMP_DRAW_HITMARK) && (element->info.acHitInfo != NULL) &&
            !(element->info.acHitInfo->toucherFlags & TOUCH_DREW_HITMARK)) {
            Math_Vec3s_ToVec3f(&hitPos, &element->info.bumper.hitPos);
            CollisionCheck_HitEffects(play, element->info.acHit, element->info.acHitInfo, &jntSph->base,
                                      &element->info, &hitPos);
            element->info.acHitInfo->toucherFlags |= TOUCH_DREW_HITMARK;
            return;
        }
    }
}

/**
 * Sets a ColliderCylinder's hit effects
 */
void CollisionCheck_SetCylHitFX(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    ColliderCylinder* cylinder = (ColliderCylinder*)collider;
    Vec3f hitPos;

    if ((cylinder->info.bumperFlags & BUMP_DRAW_HITMARK) && (cylinder->info.acHitInfo != NULL) &&
        !(cylinder->info.acHitInfo->toucherFlags & TOUCH_DREW_HITMARK)) {
        Math_Vec3s_ToVec3f(&hitPos, &cylinder->info.bumper.hitPos);
        CollisionCheck_HitEffects(play, cylinder->info.acHit, cylinder->info.acHitInfo, &cylinder->base,
                                  &cylinder->info, &hitPos);
        cylinder->info.acHitInfo->toucherFlags |= TOUCH_DREW_HITMARK;
    }
}

/**
 * Sets a ColliderTris's hit effects
 */
void CollisionCheck_SetTrisHitFX(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    ColliderTris* tris = (ColliderTris*)collider;
    ColliderTrisElement* element;
    Vec3f hitPos;

    for (element = tris->elements; element < tris->elements + tris->count; element++) {
        if ((element->info.bumperFlags & BUMP_DRAW_HITMARK) && (element->info.acHitInfo != NULL) &&
            !(element->info.acHitInfo->toucherFlags & TOUCH_DREW_HITMARK)) {
            Math_Vec3s_ToVec3f(&hitPos, &element->info.bumper.hitPos);
            CollisionCheck_HitEffects(play, element->info.acHit, element->info.acHitInfo, &tris->base,
                                      &element->info, &hitPos);
            element->info.acHitInfo->toucherFlags |= TOUCH_DREW_HITMARK;
            return;
        }
    }
}

/**
 * Sets a ColliderQuad's hit effects
 */
void CollisionCheck_SetQuadHitFX(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    ColliderQuad* quad = (ColliderQuad*)collider;
    Vec3f hitPos;

    if ((quad->info.bumperFlags & BUMP_DRAW_HITMARK) && (quad->info.acHitInfo != NULL) &&
        !(quad->info.acHitInfo->toucherFlags & TOUCH_DREW_HITMARK)) {
        Math_Vec3s_ToVec3f(&hitPos, &quad->info.bumper.hitPos);
        CollisionCheck_HitEffects(play, quad->info.acHit, quad->info.acHitInfo, &quad->base, &quad->info, &hitPos);
        quad->info.acHitInfo->toucherFlags |= TOUCH_DREW_HITMARK;
    }
}

static ColChkApplyFunc sColChkApplyFuncs[] = {
    CollisionCheck_SetJntSphHitFX,
    CollisionCheck_SetCylHitFX,
    CollisionCheck_SetTrisHitFX,
    CollisionCheck_SetQuadHitFX,
};

/**
 * Handles hit effects for each AC collider that had an AC collision. Spawns hitmarks and plays sound effects.
 */
void CollisionCheck_SetHitEffects(PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** col;

    for (col = colChkCtx->colAC; col < colChkCtx->colAC + colChkCtx->colACCount; col++) {
        Collider* colAC = *col;

        if (colAC != NULL && colAC->acFlags & AC_ON) {
            if (colAC->actor != NULL && colAC->actor->update == NULL) {
                continue;
            }
            sColChkApplyFuncs[colAC->shape](play, colChkCtx, colAC);
        }
    }
}

static ColChkVsFunc sACVsFuncs[4][4] = {
    { CollisionCheck_AC_JntSphVsJntSph, CollisionCheck_AC_JntSphVsCyl, CollisionCheck_AC_JntSphVsTris,
      CollisionCheck_AC_JntSphVsQuad },
    { CollisionCheck_AC_CylVsJntSph, CollisionCheck_AC_CylVsCyl, CollisionCheck_AC_CylVsTris,
      CollisionCheck_AC_CylVsQuad },
    { CollisionCheck_AC_TrisVsJntSph, CollisionCheck_AC_TrisVsCyl, CollisionCheck_AC_TrisVsTris,
      CollisionCheck_AC_TrisVsQuad },
    { CollisionCheck_AC_QuadVsJntSph, CollisionCheck_AC_QuadVsCyl, CollisionCheck_AC_QuadVsTris,
      CollisionCheck_AC_QuadVsQuad },
};

/**
 * Iterates through all AC colliders, performing AC collisions with the AT collider.
 */
void CollisionCheck_AC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* colAT) {
    Collider** col;

    for (col = colChkCtx->colAC; col < colChkCtx->colAC + colChkCtx->colACCount; col++) {
        Collider* colAC = *col;

        if (colAC != NULL && colAC->acFlags & AC_ON) {
            if (colAC->actor != NULL && colAC->actor->update == NULL) {
                continue;
            }
            if ((colAC->acFlags & colAT->atFlags & AC_TYPE_ALL) && (colAT != colAC)) {
                if (!(colAT->atFlags & AT_SELF) && colAT->actor != NULL && colAC->actor == colAT->actor) {
                    continue;
                }
                sACVsFuncs[colAT->shape][colAC->shape](play, colChkCtx, colAT, colAC);
            }
        }
    }
}

/**
 * Iterates through all AT colliders, testing them for AC collisions with each AC collider, setting the info regarding
 * the collision for each AC and AT collider that collided. Then spawns hitmarks and plays sound effects for each
 * successful collision. To collide, an AT collider must share a type (AC_TYPE_PLAYER, AC_TYPE_ENEMY, or AC_TYPE_OTHER)
 * with the AC collider and the toucher and bumper elements that overlapped must share a dmgFlag.
 */
void CollisionCheck_AT(PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** col;

    if (colChkCtx->colATCount == 0 || colChkCtx->colACCount == 0) {
        return;
    }
    for (col = colChkCtx->colAT; col < colChkCtx->colAT + colChkCtx->colATCount; col++) {
        Collider* colAT = *col;

        if (colAT != NULL && colAT->atFlags & AT_ON) {
            if (colAT->actor != NULL && colAT->actor->update == NULL) {
                continue;
            }
            CollisionCheck_AC(play, colChkCtx, colAT);
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
void CollisionCheck_SetOCvsOC(Collider* left, ColliderInfo* leftInfo, Vec3f* leftPos, Collider* right,
                              ColliderInfo* rightInfo, Vec3f* rightPos, f32 overlap) {
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
    Actor* leftActor = left->actor;
    Actor* rightActor = right->actor;
    s32 leftMassType;
    s32 rightMassType;

    left->ocFlags1 |= OC1_HIT;
    left->oc = rightActor;
    leftInfo->ocElemFlags |= OCELEM_HIT;
    if (right->ocFlags2 & OC2_TYPE_PLAYER) {
        left->ocFlags2 |= OC2_HIT_PLAYER;
    }
    right->oc = leftActor;
    right->ocFlags1 |= OC1_HIT;
    rightInfo->ocElemFlags |= OCELEM_HIT;
    if (left->ocFlags2 & OC2_TYPE_PLAYER) {
        right->ocFlags2 |= OC2_HIT_PLAYER;
    }
    if (leftActor == NULL || rightActor == NULL || left->ocFlags1 & OC1_NO_PUSH || right->ocFlags1 & OC1_NO_PUSH) {
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
        } else { // leftMassType == MASS_HEAVY | MASS_NORMAL
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
        } else { // leftMassType == MASS_NORMAL
            leftDispRatio = 0;
            rightDispRatio = 1;
        }
    } else { // rightMassType == MASS_NORMAL
        if (leftMassType == MASSTYPE_NORMAL) {
            inverseTotalMass = 1 / totalMass;
            leftDispRatio = rightMass * inverseTotalMass;
            rightDispRatio = leftMass * inverseTotalMass;
        } else { // leftMassType == MASS_HEAVY | MASS_IMMOVABLE
            leftDispRatio = 1;
            rightDispRatio = 0;
        }
    }

    if (!IS_ZERO(xzDist)) {
        xDelta *= overlap / xzDist;
        zDelta *= overlap / xzDist;
        leftActor->colChkInfo.displacement.x += -xDelta * leftDispRatio;
        leftActor->colChkInfo.displacement.z += -zDelta * leftDispRatio;
        rightActor->colChkInfo.displacement.x += xDelta * rightDispRatio;
        rightActor->colChkInfo.displacement.z += zDelta * rightDispRatio;
    } else if (!(overlap == 0.0f)) {
        leftActor->colChkInfo.displacement.x += -overlap * leftDispRatio;
        rightActor->colChkInfo.displacement.x += overlap * rightDispRatio;
    } else {
        leftActor->colChkInfo.displacement.x -= leftDispRatio;
        rightActor->colChkInfo.displacement.x += rightDispRatio;
    }
}

/**
 * OC overlap check for two JntSphs
 */
void CollisionCheck_OC_JntSphVsJntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* l,
                                      Collider* r) {
    ColliderJntSph* left = (ColliderJntSph*)l;
    ColliderJntSphElement* leftElem;
    ColliderJntSph* right = (ColliderJntSph*)r;
    ColliderJntSphElement* rightElem;
    f32 overlap;

    if (left->count > 0 && left->elements != NULL && right->count > 0 && right->elements != NULL) {
        for (leftElem = left->elements; leftElem < left->elements + left->count; leftElem++) {
            if (!(leftElem->info.ocElemFlags & OCELEM_ON)) {
                continue;
            }
            for (rightElem = right->elements; rightElem < right->elements + right->count; rightElem++) {
                if (!(rightElem->info.ocElemFlags & OCELEM_ON)) {
                    continue;
                }
                if (Math3D_SphVsSphOverlap(&leftElem->dim.worldSphere, &rightElem->dim.worldSphere, &overlap) == 1) {
                    Vec3f leftPos;
                    Vec3f rightPos;

                    Math_Vec3s_ToVec3f(&leftPos, &leftElem->dim.worldSphere.center);
                    Math_Vec3s_ToVec3f(&rightPos, &rightElem->dim.worldSphere.center);
                    CollisionCheck_SetOCvsOC(&left->base, &leftElem->info, &leftPos, &right->base, &rightElem->info,
                                             &rightPos, overlap);
                }
            }
        }
    }
}

/**
 * OC overlap check for a JntSph and Cylinder
 */
void CollisionCheck_OC_JntSphVsCyl(PlayState* play, CollisionCheckContext* colChkCtx, Collider* l,
                                   Collider* r) {
    ColliderJntSph* left = (ColliderJntSph*)l;
    ColliderJntSphElement* leftElem;
    ColliderCylinder* right = (ColliderCylinder*)r;
    f32 overlap;

    if (left->count > 0 && left->elements != NULL) {
        if ((right->base.ocFlags1 & OC1_ON) && (right->info.ocElemFlags & OCELEM_ON)) {
            for (leftElem = left->elements; leftElem < left->elements + left->count; leftElem++) {
                if (!(leftElem->info.ocElemFlags & OCELEM_ON)) {
                    continue;
                }
                if (Math3D_SphVsCylOverlapDist(&leftElem->dim.worldSphere, &right->dim, &overlap) == 1) {
                    Vec3f leftPos;
                    Vec3f rightPos;

                    Math_Vec3s_ToVec3f(&leftPos, &leftElem->dim.worldSphere.center);
                    Math_Vec3s_ToVec3f(&rightPos, &right->dim.pos);
                    CollisionCheck_SetOCvsOC(&left->base, &leftElem->info, &leftPos, &right->base, &right->info,
                                             &rightPos, overlap);
                }
            }
        }
    }
}

/**
 * OC overlap check for a Cylinder and JntSph
 */
void CollisionCheck_OC_CylVsJntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* l,
                                   Collider* r) {
    CollisionCheck_OC_JntSphVsCyl(play, colChkCtx, r, l);
}

/**
 * OC overlap check for two Cylinders
 */
void CollisionCheck_OC_CylVsCyl(PlayState* play, CollisionCheckContext* colChkCtx, Collider* l, Collider* r) {
    ColliderCylinder* left = (ColliderCylinder*)l;
    ColliderCylinder* right = (ColliderCylinder*)r;
    f32 deadSpace;

    if ((left->base.ocFlags1 & OC1_ON) && (right->base.ocFlags1 & OC1_ON)) {
        if ((left->info.ocElemFlags & OCELEM_ON) && (right->info.ocElemFlags & OCELEM_ON)) {
            if (Math3D_CylOutsideCyl(&left->dim, &right->dim, &deadSpace) == 1) {
                Vec3f leftPos;
                Vec3f rightPos;

                Math_Vec3s_ToVec3f(&leftPos, &left->dim.pos);
                Math_Vec3s_ToVec3f(&rightPos, &right->dim.pos);
                CollisionCheck_SetOCvsOC(&left->base, &left->info, &leftPos, &right->base, &right->info, &rightPos,
                                         deadSpace);
            }
        }
    }
}

/**
 *  Skip any OC colliders that are off
 */
s32 CollisionCheck_SkipOC(Collider* collider) {
    if (!(collider->ocFlags1 & OC1_ON)) {
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
s32 CollisionCheck_Incompatible(Collider* left, Collider* right) {
    if (!(left->ocFlags1 & right->ocFlags2 & OC1_TYPE_ALL) || !(left->ocFlags2 & right->ocFlags1 & OC1_TYPE_ALL) ||
        ((left->ocFlags2 & OC2_UNK1) && (right->ocFlags2 & OC2_UNK2)) ||
        ((right->ocFlags2 & OC2_UNK1) && (left->ocFlags2 & OC2_UNK2))) {
        return 1;
    }
    if (left->actor == right->actor) {
        return 1;
    }
    return 0;
}

static ColChkVsFunc sOCVsFuncs[4][4] = {
    { CollisionCheck_OC_JntSphVsJntSph, CollisionCheck_OC_JntSphVsCyl, NULL, NULL },
    { CollisionCheck_OC_CylVsJntSph, CollisionCheck_OC_CylVsCyl, NULL, NULL },
    { NULL, NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL },
};

/**
 * Iterates through all OC colliders and collides them with all subsequent OC colliders on the list. During an OC
 * collision, colliders with overlapping elements move away from each other so that their elements no longer overlap.
 * The relative amount each collider is pushed is determined by the collider's mass. Only JntSph and Cylinder colliders
 * can collide, and each collider must have the OC flag corresponding to the other's OC type. Additionally, OC2_UNK1
 * cannot collide with OC2_UNK2, nor can two colliders that share an actor.
 */
void CollisionCheck_OC(PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** left;
    Collider** right;
    ColChkVsFunc vsFunc;

    for (left = colChkCtx->colOC; left < colChkCtx->colOC + colChkCtx->colOCCount; left++) {
        if (*left == NULL || CollisionCheck_SkipOC(*left) == 1) {
            continue;
        }
        for (right = left + 1; right < colChkCtx->colOC + colChkCtx->colOCCount; right++) {
            if (*right == NULL || CollisionCheck_SkipOC(*right) == 1 ||
                CollisionCheck_Incompatible(*left, *right) == 1) {
                continue;
            }
            vsFunc = sOCVsFuncs[(*left)->shape][(*right)->shape];
            if (vsFunc == NULL) {
                // "Not compatible"
                osSyncPrintf("CollisionCheck_OC():未対応 %d, %d\n", (*left)->shape, (*right)->shape);
                continue;
            }
            vsFunc(play, colChkCtx, *left, *right);
        }
    }
}

/**
 * Initializes CollisionCheckInfo to default values
 */
void CollisionCheck_InitInfo(CollisionCheckInfo* info) {
    static CollisionCheckInfo init = {
        NULL, { 0.0f, 0.0f, 0.0f }, 10, 10, 0, 50, 8, 0, 0, 0, 0,
    };

    *info = init;
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
 * Unused, as all actors that don't set a damage table set their CollisionCheckInfo manually
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
 * Unused, as all actors that use a preset damage table set their CollisionCheckInfo manually.
 */
void CollisionCheck_SetInfoGetDamageTable(CollisionCheckInfo* info, s32 index, CollisionCheckInfoInit2* init) {
    CollisionCheck_SetInfo2(info, DamageTable_Get(index), init);
}

/**
 * Apply AC damage effect
 */
void CollisionCheck_ApplyDamage(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider,
                                ColliderInfo* info) {
    DamageTable* tbl;
    f32 damage;

    if (collider->actor == NULL || !(collider->acFlags & AC_HIT)) {
        return;
    }
    if (!(info->bumperFlags & BUMP_HIT) || info->bumperFlags & BUMP_NO_DAMAGE) {
        return;
    }

    ASSERT(info->acHitInfo != NULL);
    tbl = collider->actor->colChkInfo.damageTable;
    if (tbl == NULL) {
        damage = (f32)info->acHitInfo->toucher.damage - info->bumper.defense;
        if (damage < 0) {
            damage = 0;
        }
    } else {
        s32 i;
        u32 flags = info->acHitInfo->toucher.dmgFlags;

        for (i = 0; i < 0x20; i++, flags >>= 1) {
            if (flags == 1) {
                break;
            }
        }

        damage = tbl->table[i] & 0xF;
        collider->actor->colChkInfo.damageEffect = tbl->table[i] >> 4 & 0xF;
    }
    if (!(collider->acFlags & AC_HARD)) {
        collider->actor->colChkInfo.damage += damage;
    }
}

/**
 * Apply ColliderJntSph AC damage effect
 */
void CollisionCheck_ApplyDamageJntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    ColliderJntSph* jntSph = (ColliderJntSph*)collider;
    s32 i;

    if (jntSph->count > 0 && jntSph->elements != NULL) {
        for (i = 0; i < jntSph->count; i++) {
            CollisionCheck_ApplyDamage(play, colChkCtx, &jntSph->base, &jntSph->elements[i].info);
        }
    }
}

/**
 * Apply ColliderCylinder AC damage effect
 */
void CollisionCheck_ApplyDamageCyl(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    ColliderCylinder* cylinder = (ColliderCylinder*)collider;
    CollisionCheck_ApplyDamage(play, colChkCtx, &cylinder->base, &cylinder->info);
}

/**
 * Apply ColliderTris AC damage effect
 */
void CollisionCheck_ApplyDamageTris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    ColliderTris* tris = (ColliderTris*)collider;
    s32 i;

    for (i = 0; i < tris->count; i++) {
        CollisionCheck_ApplyDamage(play, colChkCtx, collider, &tris->elements[i].info);
    }
}

/**
 *  Apply ColliderQuad AC damage effect
 */
void CollisionCheck_ApplyDamageQuad(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    ColliderQuad* quad = (ColliderQuad*)collider;
    CollisionCheck_ApplyDamage(play, colChkCtx, &quad->base, &quad->info);
}

static ColChkApplyFunc sApplyDamageFuncs[4] = {
    CollisionCheck_ApplyDamageJntSph,
    CollisionCheck_ApplyDamageCyl,
    CollisionCheck_ApplyDamageTris,
    CollisionCheck_ApplyDamageQuad,
};

/**
 * For all AC colliders, sets any damage effects from collisions with AT colliders to their corresponding actor's
 * CollisionCheckInfo.
 */
void CollisionCheck_Damage(PlayState* play, CollisionCheckContext* colChkCtx) {
    s32 i;

    for (i = 0; i < colChkCtx->colACCount; i++) {
        Collider* collider = colChkCtx->colAC[i];

        if (collider == NULL) {
            continue;
        }
        if (collider->acFlags & AC_NO_DAMAGE) {
            continue;
        }
        sApplyDamageFuncs[collider->shape](play, colChkCtx, collider);
    }
}

/**
 * Checks if the line ab intersects any of the ColliderJntSph's elements
 */
s32 CollisionCheck_LineOC_JntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider,
                                 Vec3f* a, Vec3f* b) {
    static Linef D_8015E610;
    ColliderJntSph* jntSph = (ColliderJntSph*)collider;
    s32 i;

    for (i = 0; i < jntSph->count; i++) {
        ColliderJntSphElement* element = &jntSph->elements[i];

        if (!(element->info.ocElemFlags & OCELEM_ON)) {
            continue;
        }
        D_8015E610.a = *a;
        D_8015E610.b = *b;
        if (Math3D_LineVsSph(&element->dim.worldSphere, &D_8015E610) == 1) {
            return true;
        }
    }
    return false;
}

/**
 * Checks if the line segment ab intersects the ColliderCylinder
 */
s32 CollisionCheck_LineOC_Cyl(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider, Vec3f* a,
                              Vec3f* b) {
    static Vec3f D_8015E628;
    static Vec3f D_8015E638;
    ColliderCylinder* cylinder = (ColliderCylinder*)collider;

    if (!(cylinder->info.ocElemFlags & OCELEM_ON)) {
        return false;
    }
    if (Math3D_CylVsLineSeg(&cylinder->dim, a, b, &D_8015E628, &D_8015E638) != 0) {
        return true;
    }
    return false;
}

static ColChkLineFunc sOCLineCheckFuncs[4] = {
    CollisionCheck_LineOC_JntSph,
    CollisionCheck_LineOC_Cyl,
    NULL,
    NULL,
};

/**
 * Checks if the line segment ab intersects any OC colliders, excluding those attached to actors
 * on the exclusion list. Returns true if there are any intersections and false otherwise.
 */
s32 CollisionCheck_LineOC(PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b,
                          Actor** exclusions, s32 numExclusions) {
    ColChkLineFunc lineCheck;
    Collider** col;
    s32 i;
    s32 exclude;
    s32 result = 0;

    for (col = colChkCtx->colOC; col < colChkCtx->colOC + colChkCtx->colOCCount; col++) {
        if (CollisionCheck_SkipOC(*col) == 1) {
            continue;
        }
        exclude = 0;
        for (i = 0; i < numExclusions; i++) {
            if ((*col)->actor == exclusions[i]) {
                exclude = 1;
                break;
            }
        }
        if (exclude == 1) {
            continue;
        }
        lineCheck = sOCLineCheckFuncs[(*col)->shape];
        if (lineCheck == NULL) {
            // "type %d not supported"
            osSyncPrintf("CollisionCheck_generalLineOcCheck():未対応 %dタイプ\n", (*col)->shape);
        } else {
            result = lineCheck(play, colChkCtx, (*col), a, b);
            if (result) {
                break;
            }
        }
    }
    return result;
}

/**
 * Checks if the line segment ab intersects any OC colliders. Returns true if there are any intersections and false
 * otherwise. Unused.
 */
s32 CollisionCheck_LineOCCheckAll(PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b) {
    return CollisionCheck_LineOC(play, colChkCtx, a, b, NULL, 0);
}

/**
 * Checks if the line segment ab intersects any OC colliders, excluding those attached to actors on the exclusion list.
 * Returns true if there are any intersections and false otherwise.
 */
s32 CollisionCheck_LineOCCheck(PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b,
                               Actor** exclusions, s32 numExclusions) {
    return CollisionCheck_LineOC(play, colChkCtx, a, b, exclusions, numExclusions);
}

/**
 * Moves the ColliderCylinder's position to the actor's position
 */
void Collider_UpdateCylinder(Actor* actor, ColliderCylinder* collider) {
    collider->dim.pos.x = actor->world.pos.x;
    collider->dim.pos.y = actor->world.pos.y;
    collider->dim.pos.z = actor->world.pos.z;
}

/**
 * Sets the ColliderCylinder's position
 */
void Collider_SetCylinderPosition(ColliderCylinder* collider, Vec3s* pos) {
    collider->dim.pos.x = pos->x;
    collider->dim.pos.y = pos->y;
    collider->dim.pos.z = pos->z;
}

/**
 * Sets the ColliderQuad's vertices
 */
void Collider_SetQuadVertices(ColliderQuad* collider, Vec3f* a, Vec3f* b, Vec3f* c, Vec3f* d) {
    Math_Vec3f_Copy(&collider->dim.quad[2], c);
    Math_Vec3f_Copy(&collider->dim.quad[3], d);
    Math_Vec3f_Copy(&collider->dim.quad[0], a);
    Math_Vec3f_Copy(&collider->dim.quad[1], b);
    Collider_SetQuadMidpoints(&collider->dim);
}

/**
 * Sets the specified ColliderTrisElement's vertices
 */
void Collider_SetTrisVertices(ColliderTris* collider, s32 index, Vec3f* a, Vec3f* b, Vec3f* c) {
    ColliderTrisElement* element = &collider->elements[index];
    f32 nx;
    f32 ny;
    f32 nz;
    f32 originDist;

    Math_Vec3f_Copy(&element->dim.vtx[0], a);
    Math_Vec3f_Copy(&element->dim.vtx[1], b);
    Math_Vec3f_Copy(&element->dim.vtx[2], c);
    Math3D_DefPlane(a, b, c, &nx, &ny, &nz, &originDist);
    element->dim.plane.normal.x = nx;
    element->dim.plane.normal.y = ny;
    element->dim.plane.normal.z = nz;
    element->dim.plane.originDist = originDist;
}

/**
 * Sets the specified ColliderTrisElement's dim using the values in src
 */
void Collider_SetTrisDim(PlayState* play, ColliderTris* collider, s32 index, ColliderTrisElementDimInit* src) {
    ColliderTrisElement* element = &collider->elements[index];

    Collider_SetTrisElementDim(play, &element->dim, src);
}

// Due to an unknown reason, bss ordering changed between the 2 static Vec3f variables in the function below.
// In order to reproduce this behavior, we need a specific number of bss variables in the file before that point.
// For this, we introduce a certain amount of dummy variables throughout the file, which we fit inside padding added
// by the compiler between structs like TriNorm and/or Vec3f, so they don't take space in bss.
static s8 sBssDummy11;
static s8 sBssDummy12;
static s8 sBssDummy13;
static s8 sBssDummy14;

/**
 * Updates the world spheres for all of the collider's JntSph elements attached to the specified limb
 */
void Collider_UpdateSpheres(s32 limb, ColliderJntSph* collider) {
    static Vec3f D_8015E648;
    static Vec3f D_8015CF00; // bss ordering changes here
    s32 i;

    for (i = 0; i < collider->count; i++) {
        if (limb == collider->elements[i].dim.limb) {
            D_8015E648.x = collider->elements[i].dim.modelSphere.center.x;
            D_8015E648.y = collider->elements[i].dim.modelSphere.center.y;
            D_8015E648.z = collider->elements[i].dim.modelSphere.center.z;
            Matrix_MultVec3f(&D_8015E648, &D_8015CF00);
            collider->elements[i].dim.worldSphere.center.x = D_8015CF00.x;
            collider->elements[i].dim.worldSphere.center.y = D_8015CF00.y;
            collider->elements[i].dim.worldSphere.center.z = D_8015CF00.z;
            collider->elements[i].dim.worldSphere.radius =
                collider->elements[i].dim.modelSphere.radius * collider->elements[i].dim.scale;
        }
    }
}

/**
 * Spawns red blood droplets.
 * No actor has a collision type that spawns red blood.
 */
void CollisionCheck_SpawnRedBlood(PlayState* play, Vec3f* v) {
    static EffectSparkInit D_8015CF10;
    s32 effectIndex;

    D_8015CF10.position.x = v->x;
    D_8015CF10.position.y = v->y;
    D_8015CF10.position.z = v->z;
    D_8015CF10.uDiv = 5;
    D_8015CF10.vDiv = 5;
    D_8015CF10.colorStart[0].r = 128;
    D_8015CF10.colorStart[0].g = 0;
    D_8015CF10.colorStart[0].b = 64;
    D_8015CF10.colorStart[0].a = 255;
    D_8015CF10.colorStart[1].r = 128;
    D_8015CF10.colorStart[1].g = 0;
    D_8015CF10.colorStart[1].b = 64;
    D_8015CF10.colorStart[1].a = 255;
    D_8015CF10.colorStart[2].r = 255;
    D_8015CF10.colorStart[2].g = 128;
    D_8015CF10.colorStart[2].b = 0;
    D_8015CF10.colorStart[2].a = 255;
    D_8015CF10.colorStart[3].r = 255;
    D_8015CF10.colorStart[3].g = 128;
    D_8015CF10.colorStart[3].b = 0;
    D_8015CF10.colorStart[3].a = 255;
    D_8015CF10.colorEnd[0].r = 64;
    D_8015CF10.colorEnd[0].g = 0;
    D_8015CF10.colorEnd[0].b = 32;
    D_8015CF10.colorEnd[0].a = 0;
    D_8015CF10.colorEnd[1].r = 64;
    D_8015CF10.colorEnd[1].g = 0;
    D_8015CF10.colorEnd[1].b = 32;
    D_8015CF10.colorEnd[1].a = 0;
    D_8015CF10.colorEnd[2].r = 128;
    D_8015CF10.colorEnd[2].g = 0;
    D_8015CF10.colorEnd[2].b = 64;
    D_8015CF10.colorEnd[2].a = 0;
    D_8015CF10.colorEnd[3].r = 128;
    D_8015CF10.colorEnd[3].g = 0;
    D_8015CF10.colorEnd[3].b = 64;
    D_8015CF10.colorEnd[3].a = 0;
    D_8015CF10.timer = 0;
    D_8015CF10.duration = 16;
    D_8015CF10.speed = 8.0f;
    D_8015CF10.gravity = -1.0f;

    Effect_Add(play, &effectIndex, EFFECT_SPARK, 0, 1, &D_8015CF10);
}

/**
 * Spawns water droplets.
 * No actor has a collision type that spawns water droplets.
 */
void CollisionCheck_SpawnWaterDroplets(PlayState* play, Vec3f* v) {
    static EffectSparkInit D_8015D3D8;
    s32 effectIndex;

    D_8015D3D8.position.x = v->x;
    D_8015D3D8.position.y = v->y;
    D_8015D3D8.position.z = v->z;
    D_8015D3D8.uDiv = 5;
    D_8015D3D8.vDiv = 5;
    D_8015D3D8.colorStart[0].r = 255;
    D_8015D3D8.colorStart[0].g = 255;
    D_8015D3D8.colorStart[0].b = 255;
    D_8015D3D8.colorStart[0].a = 255;
    D_8015D3D8.colorStart[1].r = 100;
    D_8015D3D8.colorStart[1].g = 100;
    D_8015D3D8.colorStart[1].b = 100;
    D_8015D3D8.colorStart[1].a = 100;
    D_8015D3D8.colorStart[2].r = 100;
    D_8015D3D8.colorStart[2].g = 100;
    D_8015D3D8.colorStart[2].b = 100;
    D_8015D3D8.colorStart[2].a = 100;
    D_8015D3D8.colorStart[3].r = 100;
    D_8015D3D8.colorStart[3].g = 100;
    D_8015D3D8.colorStart[3].b = 100;
    D_8015D3D8.colorStart[3].a = 100;
    D_8015D3D8.colorEnd[0].r = 50;
    D_8015D3D8.colorEnd[0].g = 50;
    D_8015D3D8.colorEnd[0].b = 50;
    D_8015D3D8.colorEnd[0].a = 50;
    D_8015D3D8.colorEnd[1].r = 50;
    D_8015D3D8.colorEnd[1].g = 50;
    D_8015D3D8.colorEnd[1].b = 50;
    D_8015D3D8.colorEnd[1].a = 50;
    D_8015D3D8.colorEnd[2].r = 50;
    D_8015D3D8.colorEnd[2].g = 50;
    D_8015D3D8.colorEnd[2].b = 50;
    D_8015D3D8.colorEnd[2].a = 50;
    D_8015D3D8.colorEnd[3].r = 0;
    D_8015D3D8.colorEnd[3].g = 0;
    D_8015D3D8.colorEnd[3].b = 0;
    D_8015D3D8.colorEnd[3].a = 0;
    D_8015D3D8.timer = 0;
    D_8015D3D8.duration = 16;
    D_8015D3D8.speed = 8.0f;
    D_8015D3D8.gravity = -1.0f;

    Effect_Add(play, &effectIndex, EFFECT_SPARK, 0, 1, &D_8015D3D8);
}

/**
 * Spawns streaks of light from hits against solid objects
 */
void CollisionCheck_SpawnShieldParticles(PlayState* play, Vec3f* v) {
    static EffectShieldParticleInit initMetal = {
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
        { 0, 0, 0, { 0, 128, 255 }, false, 300 },
        true,
    };
    s32 effectIndex;

    initMetal.position.x = v->x;
    initMetal.position.y = v->y;
    initMetal.position.z = v->z;
    initMetal.lightPoint.x = initMetal.position.x;
    initMetal.lightPoint.y = initMetal.position.y;
    initMetal.lightPoint.z = initMetal.position.z;

    Effect_Add(play, &effectIndex, EFFECT_SHIELD_PARTICLE, 0, 1, &initMetal);
}

/**
 * Spawns streaks of light and makes a metallic sound
 */
void CollisionCheck_SpawnShieldParticlesMetal(PlayState* play, Vec3f* v) {
    CollisionCheck_SpawnShieldParticles(play, v);
    Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_REFLECT_SW, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

/**
 * Spawns streaks of light and makes a metallic sound at the specified position
 */
void CollisionCheck_SpawnShieldParticlesMetalSound(PlayState* play, Vec3f* v, Vec3f* pos) {
    CollisionCheck_SpawnShieldParticles(play, v);
    Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_REFLECT_SW, pos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

/**
 * Spawns streaks of light and makes a metallic sound
 */
void CollisionCheck_SpawnShieldParticlesMetal2(PlayState* play, Vec3f* v) {
    CollisionCheck_SpawnShieldParticlesMetal(play, v);
}

/**
 * Spawns streaks of light and makes a wooden sound
 */
void CollisionCheck_SpawnShieldParticlesWood(PlayState* play, Vec3f* v, Vec3f* actorPos) {
    static EffectShieldParticleInit initWood = {
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
        { 0, 0, 0, { 0, 128, 255 }, false, 300 },
        false,
    };
    s32 effectIndex;

    initWood.position.x = v->x;
    initWood.position.y = v->y;
    initWood.position.z = v->z;
    initWood.lightPoint.x = initWood.position.x;
    initWood.lightPoint.y = initWood.position.y;
    initWood.lightPoint.z = initWood.position.z;

    Effect_Add(play, &effectIndex, EFFECT_SHIELD_PARTICLE, 0, 1, &initWood);
    Audio_PlaySoundGeneral(NA_SE_IT_REFLECTION_WOOD, actorPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

/**
 * Determines if the line segment connecting itemPos and itemProjPos intersects the side of a cylinder with the given
 * radius, height, and offset at actorPos. Returns 3 if either endpoint is inside the cylinder, otherwise returns the
 * number of points of intersection with the side of the cylinder. The locations of those points are put in out1 and
 * out2, with out1 being closer to itemPos. Line segments that pass through both bases of the cylinder are not detected.
 */
s32 CollisionCheck_CylSideVsLineSeg(f32 radius, f32 height, f32 offset, Vec3f* actorPos, Vec3f* itemPos,
                                    Vec3f* itemProjPos, Vec3f* out1, Vec3f* out2) {
    Vec3f actorToItem;
    Vec3f actorToItemProj;
    Vec3f itemStep;
    f32 frac1;
    f32 frac2;
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
        if (SQ(actorDotItemXZ) - (4.0f * SQXZ(itemStep) * radSqDiff) > zero) {
            intersect1 = intersect2 = 1;
        } else {
            intersect1 = 1;
            intersect2 = 0;
        }
        closeDist = sqrtf(SQ(actorDotItemXZ) - (4.0f * SQXZ(itemStep) * radSqDiff));
        if (intersect1 == 1) {
            frac1 = (closeDist - actorDotItemXZ) / (2.0f * SQXZ(itemStep));
        }
        if (intersect2 == 1) {
            frac2 = (-actorDotItemXZ - closeDist) / (2.0f * SQXZ(itemStep));
        }
    } else if (!IS_ZERO(DOTXZ(2.0f * itemStep, actorToItem))) {
        intersect1 = 1;
        intersect2 = 0;
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

    if (intersect2 == 0) {
        if (frac1 < 0.0f || 1.0f < frac1) {
            return 0;
        }
    } else {
        test1 = (frac1 < 0.0f || 1.0f < frac1);
        test2 = (frac2 < 0.0f || 1.0f < frac2);

        if (test1 && test2) {
            return 0;
        }
        if (test1) {
            intersect1 = 0;
        }
        if (test2) {
            intersect2 = 0;
        }
    }

    if ((intersect1 == 1) &&
        ((frac1 * itemStep.y + actorToItem.y < 0.0f) || (height < frac1 * itemStep.y + actorToItem.y))) {
        intersect1 = 0;
    }
    if ((intersect2 == 1) &&
        ((frac2 * itemStep.y + actorToItem.y < 0.0f) || (height < frac2 * itemStep.y + actorToItem.y))) {
        intersect2 = 0;
    }
    if (intersect1 == 0 && intersect2 == 0) {
        return 0;
    } else if ((intersect1 == 1) && (intersect2 == 1)) {
        out1->x = frac1 * itemStep.x + actorToItem.x + actorPos->x;
        out1->y = frac1 * itemStep.y + actorToItem.y + actorPos->y;
        out1->z = frac1 * itemStep.z + actorToItem.z + actorPos->z;
        out2->x = frac2 * itemStep.x + actorToItem.x + actorPos->x;
        out2->y = frac2 * itemStep.y + actorToItem.y + actorPos->y;
        out2->z = frac2 * itemStep.z + actorToItem.z + actorPos->z;
        return 2;
    } else if (intersect1 == 1) {
        out1->x = frac1 * itemStep.x + actorToItem.x + actorPos->x;
        out1->y = frac1 * itemStep.y + actorToItem.y + actorPos->y;
        out1->z = frac1 * itemStep.z + actorToItem.z + actorPos->z;
        return 1;
    } else if (intersect2 == 1) {
        out1->x = frac2 * itemStep.x + actorToItem.x + actorPos->x;
        out1->y = frac2 * itemStep.y + actorToItem.y + actorPos->y;
        out1->z = frac2 * itemStep.z + actorToItem.z + actorPos->z;
        return 1;
    }
    return 1;
}

/**
 * Gets damage from a sword strike using generic values, and returns 0 if the attack is
 * not sword-type. Used by bosses to require that a sword attack deal the killing blow.
 */
u8 CollisionCheck_GetSwordDamage(s32 dmgFlags) {
    u8 damage = 0;

    if (dmgFlags & 0x00400100) {
        damage = 1;
    } else if (dmgFlags & 0x03000242) {
        damage = 2;
    } else if (dmgFlags & 0x48800400) {
        damage = 4;
    } else if (dmgFlags & 0x04000000) {
        damage = 8;
    }

    KREG(7) = damage;
    return damage;
}
