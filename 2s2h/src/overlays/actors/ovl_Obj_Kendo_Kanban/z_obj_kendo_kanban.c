/*
 * File: z_obj_kendo_kanban.c
 * Overlay: ovl_Obj_Kendo_Kanban
 * Description: Swordsman's School - Cuttable Board at back of school
 */

#include "z_obj_kendo_kanban.h"
#include "objects/object_dora/object_dora.h"

#define FLAGS 0x00000000

// Board Fragment Flags: Identify which of the 4 quadrants of the board are present.
#define OBJKENDOKANBAN_PART_FULL 0
#define OBJKENDOKANBAN_PART_TOP_RIGHT (1 << 0)
#define OBJKENDOKANBAN_PART_TOP_LEFT (1 << 1)
#define OBJKENDOKANBAN_PART_BOTTOM_RIGHT (1 << 2)
#define OBJKENDOKANBAN_PART_BOTTOM_LEFT (1 << 3)
#define OBJKENDOKANBAN_RIGHT_HALF (OBJKENDOKANBAN_PART_TOP_RIGHT | OBJKENDOKANBAN_PART_BOTTOM_RIGHT)
#define OBJKENDOKANBAN_LEFT_HALF (OBJKENDOKANBAN_PART_TOP_LEFT | OBJKENDOKANBAN_PART_BOTTOM_LEFT)
#define OBJKENDOKANBAN_TOP_HALF (OBJKENDOKANBAN_PART_TOP_RIGHT | OBJKENDOKANBAN_PART_TOP_LEFT)
#define OBJKENDOKANBAN_BOTTOM_HALF (OBJKENDOKANBAN_PART_BOTTOM_RIGHT | OBJKENDOKANBAN_PART_BOTTOM_LEFT)

// Number of bounces the board takes before settling.
#define MAX_BOUNCE_COUNT (7)

typedef enum {
    /* -1 */ OBJKENDOKANBAN_DIR_DOWN = -1,
    /*  0 */ OBJKENDOKANBAN_DIR_UNDETERMINED,
    /*  1 */ OBJKENDOKANBAN_DIR_UP
} ObjKendoKanbanDirection;

void ObjKendoKanban_Init(Actor* thisx, PlayState* play);
void ObjKendoKanban_Destroy(Actor* thisx, PlayState* play);
void ObjKendoKanban_Update(Actor* thisx, PlayState* play);
void ObjKendoKanban_Draw(Actor* thisx, PlayState* play);

void ObjKendoKanban_SetupDoNothing(ObjKendoKanban* this);
void ObjKendoKanban_DoNothing(ObjKendoKanban* this, PlayState* play);
void ObjKendoKanban_SetupTumble(ObjKendoKanban* this, PlayState* play);
void ObjKendoKanban_Tumble(ObjKendoKanban* this, PlayState* play);
void ObjKendoKanban_Settled(ObjKendoKanban* this, PlayState* play);
void ObjKendoKanban_HandlePhysics(ObjKendoKanban* this, PlayState* play);
s32 ObjKendoKanban_IsPlayerOnTop(ObjKendoKanban* this, PlayState* play);

ActorProfile Obj_Kendo_Kanban_Profile = {
    /**/ ACTOR_OBJ_KENDO_KANBAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DORA,
    /**/ sizeof(ObjKendoKanban),
    /**/ ObjKendoKanban_Init,
    /**/ ObjKendoKanban_Destroy,
    /**/ ObjKendoKanban_Update,
    /**/ ObjKendoKanban_Draw,
};

// Directly applied to the velocity of the actor upon object creation.
static Vec3f sVelocityLeftHalf = { -1.5f, 10.0f, 0.5f };  // Push Left/Up/Forward  (Left side Breakaway)
static Vec3f sVelocityRightHalf = { 1.5f, 10.0f, 0.5f };  // Push Right/Up/Forward (Right side Breakaway)
static Vec3f sVelocityBottomHalf = { 0.0f, 4.0f, -1.0f }; // Push _/Up/Back  (Bottom Breakaway)
static Vec3f sVelocityTopHalf = { 1.0f, 7.0f, 4.0f };     // Push Right/Up/Forward (Top Breakaway)

// Centerpoint of the piece
static Vec3f sCenterPointLeftHalf = { -150.0f, 425.0f, 40.0f };
static Vec3f sCenterPointRightHalf = { 150.0f, 425.0f, 40.0f };
static Vec3f sCenterPointBottomHalf = { 0.0f, 140.0f, 40.0f };
static Vec3f sCenterPointTopHalf = { 0.0f, 565.0f, 40.0f };

// Displaylists for the 4 pieces.
static Gfx* sDisplayLists[] = {
    gKendoKanbanTopRightDL,
    gKendoKanbanTopLeftDL,
    gKendoKanbanBottomRightDL,
    gKendoKanbanBottomLeftDL,
};

// Coordinates on the object face upon which the board can break
static Vec3f sPointTL = { -300.0f, 850.0f, 40.0f };
static Vec3f sPointTC = { 10.0f, 850.0f, 40.0f };
static Vec3f sPointTR = { 300.0f, 850.0f, 40.0f };
static Vec3f sPointCL = { -300.0f, 310.0f, 40.0f };
static Vec3f sPointCC = { 0.0f, 280.0f, 40.0f }; // Unused
static Vec3f sPointCR = { 300.0f, 250.0f, 40.0f };
static Vec3f sPointBL = { -300.0f, 10.0f, 40.0f };
static Vec3f sPointBC = { 0.0f, 10.0f, 40.0f };
static Vec3f sPointBR = { 300.0f, 10.0f, 40.0f };

static ColliderTrisElementInit sTrisElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK5,
            { 0x00000000, 0x00, 0x00 },
            { 0x01000202, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -300.0f, 850.0f, 40.0f }, { -300.0f, 10.0f, 40.0f }, { 300.0f, 850.0f, 40.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK5,
            { 0x00000000, 0x00, 0x00 },
            { 0x01000202, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 300.0f, 850.0f, 40.0f }, { 300.0f, 10.0f, 40.0f }, { -300.0f, 10.0f, 40.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COL_MATERIAL_TREE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit),
    sTrisElementsInit,
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
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 33, 80, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(2, 0xF),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
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

static CollisionCheckInfoInit2 sColChkInfoInit = { 8, 0, 0, 0, MASS_HEAVY };

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };
static Vec3f sUnitVecX = { 1.0f, 0.0f, 0.0f };

void ObjKendoKanban_Init(Actor* thisx, PlayState* play) {
    s32 pad[2];
    ObjKendoKanban* this = (ObjKendoKanban*)thisx;
    Vec3f vertices[3];
    s32 i;
    s32 j;

    Actor_SetScale(&this->actor, 0.1f);

    Collider_InitCylinder(play, &this->colliderCylinder);
    Collider_SetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Collider_InitTris(play, &this->colliderTris);
    Collider_SetTris(play, &this->colliderTris, &this->actor, &sTrisInit, this->colliderTrisElements);

    Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                 &this->actor.shape.rot);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

    for (i = 0; i < ARRAY_COUNT(this->colliderTrisElements); i++) {
        for (j = 0; j < ARRAY_COUNT(vertices); j++) {
            Matrix_MultVec3f(&sTrisElementsInit[i].dim.vtx[j], &vertices[j]);
        }
        Collider_SetTrisVertices(&this->colliderTris, i, &vertices[0], &vertices[1], &vertices[2]);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    this->boardFragments = OBJKENDOKANBAN_GET_BOARD_FRAGMENTS(&this->actor);
    this->actor.gravity = -2.0f;
    this->centerPoint = sZeroVec;
    this->centerPos = sZeroVec;
    this->rootCornerPos = sZeroVec;
    this->rotAxis = sUnitVecX;
    this->rotAngle = 0;
    this->angularVelocity = 0;
    this->indexLastRootCornerPos = -1;
    this->hasNewRootCornerPos = false;
    this->numBounces = 0;

    for (i = 0; i < ARRAY_COUNT(this->cornerPos); i++) {
        this->cornerPos[i] = this->cornerPoints[i] = sZeroVec;
    }

    this->unk_30A = 0;
    if (this->boardFragments == OBJKENDOKANBAN_PART_FULL) {
        ObjKendoKanban_SetupDoNothing(this);
    } else {
        ObjKendoKanban_SetupTumble(this, play);
    }
}

void ObjKendoKanban_Destroy(Actor* thisx, PlayState* play) {
    ObjKendoKanban* this = (ObjKendoKanban*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
    Collider_DestroyTris(play, &this->colliderTris);
}

void ObjKendoKanban_SetupDoNothing(ObjKendoKanban* this) {
    this->actionFunc = ObjKendoKanban_DoNothing;
}

void ObjKendoKanban_DoNothing(ObjKendoKanban* this, PlayState* play) {
}

void ObjKendoKanban_SetupTumble(ObjKendoKanban* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->boardFragments == OBJKENDOKANBAN_PART_FULL) {
        if ((player->meleeWeaponAnimation == PLAYER_MWA_FORWARD_SLASH_1H) ||
            (player->meleeWeaponAnimation == PLAYER_MWA_FORWARD_SLASH_2H) ||
            (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH)) {

            // Vertical cuts initialize the right half, spawn the left half.
            this->boardFragments = OBJKENDOKANBAN_RIGHT_HALF;
            this->angularVelocity = 0x71C; // 10 degrees
            this->actor.velocity = sVelocityRightHalf;
            this->centerPoint = sCenterPointRightHalf;

            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_OBJ_KENDO_KANBAN,
                               this->actor.home.pos.x - 5.0f, this->actor.home.pos.y, this->actor.home.pos.z, 0, 0, 0,
                               OBJKENDOKANBAN_LEFT_HALF);

            this->cornerPoints[0] = sPointTC;
            this->cornerPoints[1] = sPointTR;
            this->cornerPoints[2] = sPointBR;
            this->cornerPoints[3] = sPointBC;
        } else {
            // Horizontal cuts initialize the bottom half, spawn the top half.
            this->boardFragments = OBJKENDOKANBAN_BOTTOM_HALF;
            this->angularVelocity = -0x71C; // -10 degrees
            this->actor.velocity = sVelocityBottomHalf;
            this->centerPoint = sCenterPointBottomHalf;

            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_OBJ_KENDO_KANBAN, this->actor.home.pos.x,
                               this->actor.home.pos.y + 5.0f, this->actor.home.pos.z, 0, 0, 0, OBJKENDOKANBAN_TOP_HALF);

            this->cornerPoints[0] = sPointCL;
            this->cornerPoints[1] = sPointCR;
            this->cornerPoints[2] = sPointBR;
            this->cornerPoints[3] = sPointBL;
        }
    } else if (this->boardFragments == OBJKENDOKANBAN_LEFT_HALF) {
        // Initialize the newly spawned left half
        this->angularVelocity = 0x71C; // 10 degrees
        this->actor.velocity = sVelocityLeftHalf;
        this->centerPoint = sCenterPointLeftHalf;

        this->cornerPoints[0] = sPointTL;
        this->cornerPoints[1] = sPointTC;
        this->cornerPoints[2] = sPointBC;
        this->cornerPoints[3] = sPointBL;
    } else if (this->boardFragments == OBJKENDOKANBAN_TOP_HALF) {
        // Initialize the newly spawned top half
        this->angularVelocity = 0x71C; // 10 degrees
        this->actor.velocity = sVelocityTopHalf;
        this->centerPoint = sCenterPointTopHalf;

        this->cornerPoints[0] = sPointTL;
        this->cornerPoints[1] = sPointTR;
        this->cornerPoints[2] = sPointCR;
        this->cornerPoints[3] = sPointCL;
    }

    this->unk_30A = 0;
    this->actionFunc = ObjKendoKanban_Tumble;
}

void ObjKendoKanban_Tumble(ObjKendoKanban* this, PlayState* play) {
    this->actor.velocity.y += this->actor.gravity;
    Actor_UpdatePos(&this->actor);
    this->rotAngle += this->angularVelocity;
    ObjKendoKanban_HandlePhysics(this, play);
    if (this->actor.world.pos.y < -200.0f) {
        this->actor.world.pos.y = -200.0f;
    }
}

void ObjKendoKanban_SetupSettled(ObjKendoKanban* this) {
    this->actionFunc = ObjKendoKanban_Settled;
}

void ObjKendoKanban_Settled(ObjKendoKanban* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (ObjKendoKanban_IsPlayerOnTop(this, play) == true) {
        player->unk_AC0 = 700.0f;
    }
}

void ObjKendoKanban_HandlePhysics(ObjKendoKanban* this, PlayState* play) {
    Vec3f rootCornerPos = this->cornerPos[0];
    s32 pad[2];
    Vec3f vecCenterOut = this->actor.world.pos;
    s32 pad2;
    s32 indexRootCornerPos = 0;
    s32 i;
    f32 verticalScalar;

    // Calculate an affect on the rotation from gravity.
    vecCenterOut.x -= this->centerPos.x;
    vecCenterOut.y -= this->centerPos.y;
    vecCenterOut.z -= this->centerPos.z;
    verticalScalar = (this->rotAxis.x * vecCenterOut.z) + (this->rotAxis.z * -vecCenterOut.x);
    if (verticalScalar < 0.0f) {
        this->angularVelocity += 0x64;
    } else {
        this->angularVelocity -= 0x64;
    }

    // Find the lowest point
    for (i = 0; i < ARRAY_COUNT(this->cornerPos); i++) {
        if (this->cornerPos[i].y < rootCornerPos.y) {
            rootCornerPos = this->cornerPos[i];
            indexRootCornerPos = i;
        }
    }

    // When the lowest point changes, re-initialize the actor position to that point
    if (indexRootCornerPos != this->indexLastRootCornerPos) {
        this->hasNewRootCornerPos = true;
        this->indexLastRootCornerPos = indexRootCornerPos;
        this->rootCornerPos = this->cornerPoints[indexRootCornerPos];

        Matrix_Push();
        Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                     &this->actor.shape.rot);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_MultVec3f(&this->rootCornerPos, &this->actor.world.pos);
        this->actor.world.pos = rootCornerPos;
        this->actor.prevPos = this->actor.world.pos;
        Matrix_Pop();
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        // When on the ground, apply some friction.
        this->actor.velocity.x *= 0.8f;
        this->actor.velocity.z *= 0.8f;
    }

    if (this->hasNewRootCornerPos == true) {
        if (this->numBounces >= MAX_BOUNCE_COUNT) {
            s16 deltaRotAngle = this->rotAngle & 0x3FFF;

            if (deltaRotAngle >= 0x2000) { // 45 degrees
                deltaRotAngle -= 0x4000;   // 90 degrees
            }
            this->rotAngle -= deltaRotAngle;
            this->angularVelocity = 0;
            ObjKendoKanban_SetupSettled(this);
            return;
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            // Upon touching the ground...
            Actor_PlaySfx(&this->actor, NA_SE_EV_WOODPLATE_BOUND);
            this->hasNewRootCornerPos = false;
            this->actor.velocity.y *= 0.5f;
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            // When on the ground...
            this->numBounces++;
            this->hasNewRootCornerPos = false;
            this->actor.velocity.x *= 0.3f;
            this->actor.velocity.z *= 0.3f;
            Actor_PlaySfx(&this->actor, NA_SE_EV_WOODPLATE_BOUND);

            // Adjust and (potentially) reverse rotation depending on the current
            // facing of the board and the direction in which it is rotating.
            if (verticalScalar > 0.0f) {
                if (this->angularVelocity > 0) {
                    this->angularVelocity *= 1.2f;
                } else {
                    this->angularVelocity *= -0.6f;
                }
            } else {
                if (this->angularVelocity < 0) {
                    this->angularVelocity *= 1.2f;
                } else {
                    this->angularVelocity *= -0.6f;
                }
            }
        }
    }
}

s32 ObjKendoKanban_IsPlayerOnTop(ObjKendoKanban* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;
    s32 j;
    Vec2f playerToCornerA;
    Vec2f playerToCornerB;
    s32 priorDir = OBJKENDOKANBAN_DIR_UNDETERMINED;

    for (i = 0; i < ARRAY_COUNT(this->cornerPos); i++) {
        j = (i != 3) ? (i + 1) : 0;

        // For each pair of points (moving clockwise around the shape), verify that the normal
        // vector's magnitute is in the same direction. This condition being true means
        // the player is within the bounds of the four cornerPoints.
        playerToCornerA.z = this->cornerPos[i].z - player->actor.world.pos.z;
        playerToCornerA.x = this->cornerPos[i].x - player->actor.world.pos.x;
        playerToCornerB.z = this->cornerPos[j].z - player->actor.world.pos.z;
        playerToCornerB.x = this->cornerPos[j].x - player->actor.world.pos.x;
        if ((playerToCornerA.x * playerToCornerB.z) < (playerToCornerA.z * playerToCornerB.x)) {
            if (priorDir == OBJKENDOKANBAN_DIR_UNDETERMINED) {
                priorDir = OBJKENDOKANBAN_DIR_UP;
            } else if (priorDir != OBJKENDOKANBAN_DIR_UP) {
                return false;
            }
        } else {
            if (priorDir == OBJKENDOKANBAN_DIR_UNDETERMINED) {
                priorDir = OBJKENDOKANBAN_DIR_DOWN;
            } else if (priorDir != OBJKENDOKANBAN_DIR_DOWN) {
                return false;
            }
        }
    }
    return true;
}

void ObjKendoKanban_UpdateCollision(ObjKendoKanban* this, PlayState* play) {
    if ((this->actionFunc != ObjKendoKanban_Tumble) && (this->actionFunc != ObjKendoKanban_Settled)) {
        if (this->colliderTris.base.acFlags & AC_HIT) {
            this->colliderTris.base.acFlags &= ~AC_HIT;
            ObjKendoKanban_SetupTumble(this, play);
        }

        Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
        this->colliderCylinder.dim.pos.z -= TRUNCF_BINANG(20.0f * Math_CosS(this->actor.shape.rot.y));
        this->colliderCylinder.dim.pos.x -= TRUNCF_BINANG(20.0f * Math_SinS(this->actor.shape.rot.y));

        if (this->actionFunc == ObjKendoKanban_DoNothing) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderTris.base);
        }
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
    }
}

void ObjKendoKanban_Update(Actor* thisx, PlayState* play) {
    ObjKendoKanban* this = (ObjKendoKanban*)thisx;

    this->actionFunc(this, play);
    ObjKendoKanban_UpdateCollision(this, play);
}

void ObjKendoKanban_Draw(Actor* thisx, PlayState* play) {
    ObjKendoKanban* this = (ObjKendoKanban*)thisx;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->boardFragments == OBJKENDOKANBAN_PART_FULL) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gKendoKanbanDL);
    } else {
        Matrix_RotateAxisS(this->rotAngle, &this->rotAxis, MTXMODE_APPLY);
        Matrix_Translate(-this->rootCornerPos.x, -this->rootCornerPos.y, -this->rootCornerPos.z, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        // Display only the fragments of the board which are present
        for (i = 0; i < ARRAY_COUNT(sDisplayLists); i++) {
            if ((1 << i) & this->boardFragments) {
                gSPDisplayList(POLY_OPA_DISP++, sDisplayLists[i]);
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);

    // Update the alternate position trackers (Corners and Center of the board)
    for (i = 0; i < ARRAY_COUNT(this->cornerPos); i++) {
        Matrix_MultVec3f(&this->cornerPoints[i], &this->cornerPos[i]);
    }
    Matrix_MultVec3f(&this->centerPoint, &this->centerPos);
}
