/*
 * File: z_obj_tree.c
 * Overlay: ovl_Obj_Tree
 * Description: Single branching tree (e.g. North Clock Town)
 */

#include "z_obj_tree.h"
#include "objects/object_tree/object_tree.h"

#define FLAGS (ACTOR_FLAG_UPDATE_DURING_OCARINA)

void ObjTree_Init(Actor* thisx, PlayState* play);
void ObjTree_Destroy(Actor* thisx, PlayState* play);
void ObjTree_Update(Actor* thisx, PlayState* play);
void ObjTree_Draw(Actor* thisx, PlayState* play);

void ObjTree_DoNothing(ObjTree* this, PlayState* play);
void ObjTree_SetupDoNothing(ObjTree* this);
void ObjTree_Sway(ObjTree* this, PlayState* play);

ActorProfile Obj_Tree_Profile = {
    /**/ ACTOR_OBJ_TREE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_TREE,
    /**/ sizeof(ObjTree),
    /**/ ObjTree_Init,
    /**/ ObjTree_Destroy,
    /**/ ObjTree_Update,
    /**/ ObjTree_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_TREE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x0100020A, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 28, 120, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(2, 0xF),
    /* Hookshot       */ DMG_ENTRY(0, 0x1),
    /* Goron punch    */ DMG_ENTRY(2, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(1, 0xF),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0xF),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

static CollisionCheckInfoInit2 sColchkInfoInit = { 8, 0, 0, 0, MASS_HEAVY };

void ObjTree_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjTree* this = (ObjTree*)thisx;
    CollisionHeader* colHeader = NULL;

    if (OBJTREE_ISLARGE(&this->dyna.actor)) {
        Actor_SetScale(&this->dyna.actor, 0.15f);
        this->dyna.actor.cullingVolumeDistance = 4000.0f;
    } else {
        Actor_SetScale(&this->dyna.actor, 0.1f);
        DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
        CollisionHeader_GetVirtual(&gTreeTopCol, &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    }

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->dyna.actor.colChkInfo, &sDamageTable, &sColchkInfoInit);

    if (OBJTREE_ISLARGE(&this->dyna.actor)) {
        this->collider.dim.height = 220;
    }

    this->swayAmplitude = 0.0f;
    this->swayAngle = 0;
    this->swayVelocity = 0;
    ObjTree_SetupDoNothing(this);
}

void ObjTree_Destroy(Actor* thisx, PlayState* play) {
    ObjTree* this = (ObjTree*)thisx;
    s32 bgId;

    if (!OBJTREE_ISLARGE(&this->dyna.actor)) {
        bgId = this->dyna.bgId;
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, bgId);
    }

    Collider_DestroyCylinder(play, &this->collider);
}

void ObjTree_SetupDoNothing(ObjTree* this) {
    this->actionFunc = ObjTree_DoNothing;
}

void ObjTree_DoNothing(ObjTree* this, PlayState* play) {
}

void ObjTree_SetupSway(ObjTree* this) {
    this->timer = 0;
    this->swayAmplitude = 546.0f;
    this->swayVelocity = 35 * 0x10000 / 360;
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_TREE_SWING);
    this->actionFunc = ObjTree_Sway;
}

void ObjTree_Sway(ObjTree* this, PlayState* play) {
    if (this->timer > 80) {
        ObjTree_SetupDoNothing(this);
        return;
    }

    Math_SmoothStepToF(&this->swayAmplitude, 0.0f, 0.1f, 91.0f, 18.0f);
    this->swayVelocity += 1 * 0x10000 / 360;
    this->swayAngle += this->swayVelocity;
    this->dyna.actor.shape.rot.x = Math_SinS(this->swayAngle) * this->swayAmplitude;
    this->dyna.actor.shape.rot.z = Math_CosS(this->swayAngle) * this->swayAmplitude;
    this->timer++;
}

void ObjTree_UpdateCollision(ObjTree* this, PlayState* play) {
    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    if (this->dyna.actor.xzDistToPlayer < 600.0f) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        if (this->dyna.actor.home.rot.y == 1) {
            this->dyna.actor.home.rot.y = 0;
            ObjTree_SetupSway(this);
        }
    }
}

void ObjTree_Update(Actor* thisx, PlayState* play) {
    ObjTree* this = (ObjTree*)thisx;

    this->actionFunc(this, play);
    ObjTree_UpdateCollision(this, play);
}

void ObjTree_Draw(Actor* thisx, PlayState* play) {
    s16 xRot = (f32)thisx->shape.rot.x;
    s16 zRot = (f32)thisx->shape.rot.z;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gTreeBodyDL);

    Matrix_RotateZYX(xRot, 0, zRot, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gTreeLeavesDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
