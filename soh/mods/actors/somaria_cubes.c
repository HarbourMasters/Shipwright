/**
 * Somaria Cubes - Colored cubes with hookshot + switchhook support
 * Uses actor hijacking on En_Lightbox
 */

#include "somaria_cubes.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_En_Lightbox/z_en_lightbox.h"
#include "overlays/actors/ovl_Bg_Bdan_Switch/z_bg_bdan_switch.h"

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

static void SomariaCube_Update(Actor* thisx, PlayState* play);
static void SomariaCube_Draw(Actor* thisx, PlayState* play);
static void SomariaCube_DestroyFunc(Actor* thisx, PlayState* play);

static ActorFunc sOriginalDestroy = NULL;
static ActorFunc sSomariaCubeUpdate = SomariaCube_Update;

// ============================================================================
// DISPLAY LIST - Cube (30x30x30 units)
// ============================================================================

static Vtx sSomariaCubeVtx[] = {
    // Front face (Z+)
    VTX(-15, 0,  15, 0, 0, 0, 0, 127, 255),
    VTX( 15, 0,  15, 0, 0, 0, 0, 127, 255),
    VTX( 15, 30, 15, 0, 0, 0, 0, 127, 255),
    VTX(-15, 30, 15, 0, 0, 0, 0, 127, 255),
    // Back face (Z-)
    VTX( 15, 0, -15, 0, 0, 0, 0, -127, 255),
    VTX(-15, 0, -15, 0, 0, 0, 0, -127, 255),
    VTX(-15, 30,-15, 0, 0, 0, 0, -127, 255),
    VTX( 15, 30,-15, 0, 0, 0, 0, -127, 255),
    // Top face (Y+)
    VTX(-15, 30, 15, 0, 0, 0, 127, 0, 255),
    VTX( 15, 30, 15, 0, 0, 0, 127, 0, 255),
    VTX( 15, 30,-15, 0, 0, 0, 127, 0, 255),
    VTX(-15, 30,-15, 0, 0, 0, 127, 0, 255),
    // Bottom face (Y-)
    VTX(-15, 0, -15, 0, 0, 0, -127, 0, 255),
    VTX( 15, 0, -15, 0, 0, 0, -127, 0, 255),
    VTX( 15, 0,  15, 0, 0, 0, -127, 0, 255),
    VTX(-15, 0,  15, 0, 0, 0, -127, 0, 255),
    // Right face (X+)
    VTX( 15, 0,  15, 0, 0, 127, 0, 0, 255),
    VTX( 15, 0, -15, 0, 0, 127, 0, 0, 255),
    VTX( 15, 30,-15, 0, 0, 127, 0, 0, 255),
    VTX( 15, 30, 15, 0, 0, 127, 0, 0, 255),
    // Left face (X-)
    VTX(-15, 0, -15, 0, 0, -127, 0, 0, 255),
    VTX(-15, 0,  15, 0, 0, -127, 0, 0, 255),
    VTX(-15, 30, 15, 0, 0, -127, 0, 0, 255),
    VTX(-15, 30,-15, 0, 0, -127, 0, 0, 255),
};

static Gfx sSomariaCubeDL[] = {
    gsSPVertex(sSomariaCubeVtx, 24, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsSP2Triangles(20, 21, 22, 0, 20, 22, 23, 0),
    gsSPEndDisplayList(),
};

// ============================================================================
// COLOR TABLE
// ============================================================================

typedef struct {
    u8 r, g, b;
} CubeColor;

static CubeColor sColorTable[SOMARIA_COLOR_MAX] = {
    { 80,  80,  255 },  // BLUE
    { 255, 80,  80  },  // RED
    { 80,  255, 80  },  // GREEN
    { 255, 255, 80  },  // YELLOW
    { 200, 80,  255 },  // PURPLE
    { 80,  255, 255 },  // CYAN
    { 255, 160, 80  },  // ORANGE
};

// ============================================================================
// COLLIDER (AC for hookshot only - NO AT, NO OC)
// ============================================================================

static ColliderCylinderInit sColliderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_NONE,
    },
    { SOMARIA_CYL_RADIUS, SOMARIA_CYL_HEIGHT, 0, { 0, 0, 0 } },
};

// ============================================================================
// STATIC COLLIDER POOL (max 3 cubes)
// Using static pool because Actor_Spawn only allocates sizeof(EnLightbox),
// so we CANNOT add extra fields to the actor struct!
// ============================================================================

typedef struct {
    ColliderCylinder collider;
    Actor* owner;           // Which cube owns this slot (NULL = free)
    u8 initialized;
} ColliderSlot;

static ColliderSlot sColliderPool[SOMARIA_MAX_CUBES] = { 0 };

static s8 SomariaCube_GetColliderSlot(Actor* actor) {
    for (s8 i = 0; i < SOMARIA_MAX_CUBES; i++) {
        if (sColliderPool[i].owner == actor) return i;
    }
    return -1;
}

static s8 SomariaCube_AllocCollider(PlayState* play, Actor* actor) {
    for (s8 i = 0; i < SOMARIA_MAX_CUBES; i++) {
        if (sColliderPool[i].owner == NULL) {
            if (!sColliderPool[i].initialized) {
                Collider_InitCylinder(play, &sColliderPool[i].collider);
                sColliderPool[i].initialized = 1;
            }
            Collider_SetCylinder(play, &sColliderPool[i].collider, actor, &sColliderInit);
            sColliderPool[i].owner = actor;
            return i;
        }
    }
    return -1;
}

static void SomariaCube_FreeCollider(PlayState* play, Actor* actor) {
    s8 slot = SomariaCube_GetColliderSlot(actor);
    if (slot >= 0) {
        sColliderPool[slot].owner = NULL;
        // Don't destroy - reuse the initialized collider
    }
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void SomariaCube_PlaySound(Actor* actor, u16 sfxId) {
    Audio_PlaySoundGeneral(sfxId, &actor->projectedPos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

// Check and activate Bg_Bdan_Switch (YELLOW_HEAVY type 0x01) when cube is on top
static void SomariaCube_TryActivateHeavySwitch(Actor* cube, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_SWITCH].head;

    while (actor != NULL) {
        if (actor->id == ACTOR_BG_BDAN_SWITCH) {
            u8 switchType = actor->params & 0xFF;

            // Only affect YELLOW_HEAVY (0x01) switches
            if (switchType == YELLOW_HEAVY) {
                // Check if cube is within horizontal range of switch
                f32 dx = cube->world.pos.x - actor->world.pos.x;
                f32 dz = cube->world.pos.z - actor->world.pos.z;
                f32 distXZ = sqrtf(dx * dx + dz * dz);

                // Check if cube is above switch (within vertical tolerance)
                f32 dy = cube->world.pos.y - actor->world.pos.y;

                // Switch is about 40 units radius, cube needs to be on top
                if (distXZ < 40.0f && dy >= 0.0f && dy < 50.0f) {
                    // Activate the switch flag
                    u8 switchFlag = (actor->params >> 8) & 0x3F;
                    if (!Flags_GetSwitch(play, switchFlag)) {
                        Flags_SetSwitch(play, switchFlag);
                        SomariaCube_PlaySound(cube, NA_SE_EV_FOOT_SWITCH);
                        // Play the chime
                        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &gSfxDefaultPos, 4,
                            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    }
                }
            }
        }
        actor = actor->next;
    }
}

u8 SomariaCube_IsSomariaCube(Actor* actor) {
    if (actor == NULL || actor->update == NULL) return 0;
    return (actor->update == sSomariaCubeUpdate);
}

u8 SomariaCube_IsSwitchable(Actor* actor) {
    // All Somaria cubes are switchhookable
    return SomariaCube_IsSomariaCube(actor);
}

// ============================================================================
// UPDATE
// ============================================================================

static void SomariaCube_Update(Actor* thisx, PlayState* play) {
    SomariaCubeState state = SOMARIA_GET_STATE(thisx);
    s16 timer = SOMARIA_GET_TIMER(thisx);
    Player* player = GET_PLAYER(play);

    if (player == NULL) return;

    if (timer > 0) {
        SOMARIA_SET_TIMER(thisx, timer - 1);
        timer--;
    }

    switch (state) {
        case SOMARIA_STATE_SPAWN:
            if (thisx->scale.x < SOMARIA_CUBE_SCALE) {
                thisx->scale.x += SOMARIA_CUBE_SCALE / SOMARIA_SPAWN_FRAMES;
                thisx->scale.y = thisx->scale.z = thisx->scale.x;
            }
            if (timer == 0) {
                Actor_SetScale(thisx, SOMARIA_CUBE_SCALE);
                SOMARIA_SET_STATE(thisx, SOMARIA_STATE_IDLE);
            }
            Actor_MoveXZGravity(thisx);
            Actor_UpdateBgCheckInfo(play, thisx, 30.0f, 15.0f, 0.0f, 0x1D);
            break;

        case SOMARIA_STATE_IDLE:
            if (Actor_HasParent(thisx, play)) {
                SOMARIA_SET_STATE(thisx, SOMARIA_STATE_HELD);
                thisx->room = -1;
                break;
            }

            if (thisx->speedXZ > 0.1f && (thisx->bgCheckFlags & BGCHECKFLAG_WALL)) {
                thisx->world.rot.y = thisx->wallYaw;
                SomariaCube_PlaySound(thisx, NA_SE_EV_BOMB_BOUND);
                thisx->speedXZ *= 0.7f;
                thisx->bgCheckFlags &= ~BGCHECKFLAG_WALL;
            }

            if (thisx->bgCheckFlags & BGCHECKFLAG_GROUND) {
                Math_StepToF(&thisx->speedXZ, 0.0f, 1.0f);
                if ((thisx->bgCheckFlags & 0x2) && thisx->velocity.y < -4.0f) {
                    SomariaCube_PlaySound(thisx, NA_SE_EV_BLOCK_BOUND);
                    thisx->velocity.y *= -0.3f;
                }
                // Allow player to pick up the cube
                Actor_OfferCarry(thisx, play);
                // Check if cube is on a heavy weight switch (Bg_Bdan_Switch type 0x01)
                SomariaCube_TryActivateHeavySwitch(thisx, play);
            } else {
                Math_StepToF(&thisx->speedXZ, 0.0f, 0.2f);
            }

            Actor_MoveXZGravity(thisx);
            Actor_UpdateBgCheckInfo(play, thisx, 30.0f, 15.0f, 0.0f, 0x1D);
            break;

        case SOMARIA_STATE_HELD:
            if (Actor_HasNoParent(thisx, play)) {
                SOMARIA_SET_STATE(thisx, SOMARIA_STATE_THROWN);
                thisx->velocity.y = SOMARIA_THROW_VEL_Y;
                thisx->speedXZ = SOMARIA_THROW_SPEED_XZ;
                thisx->world.rot.y = player->actor.shape.rot.y;
            }
            break;

        case SOMARIA_STATE_THROWN:
            if (thisx->bgCheckFlags & BGCHECKFLAG_WALL) {
                thisx->world.rot.y = thisx->wallYaw;
                SomariaCube_PlaySound(thisx, NA_SE_EV_BOMB_BOUND);
                thisx->speedXZ *= 0.5f;
                thisx->bgCheckFlags &= ~BGCHECKFLAG_WALL;
            }

            if (thisx->bgCheckFlags & BGCHECKFLAG_GROUND) {
                SOMARIA_SET_STATE(thisx, SOMARIA_STATE_IDLE);
                SomariaCube_PlaySound(thisx, NA_SE_EV_BLOCK_BOUND);
            }

            Actor_MoveXZGravity(thisx);
            Actor_UpdateBgCheckInfo(play, thisx, 30.0f, 15.0f, 0.0f, 0x1D);
            break;
    }

    thisx->focus.pos = thisx->world.pos;
    thisx->focus.pos.y += 15.0f;

    // Update collider from pool (AC only for hookshot)
    s8 slot = SomariaCube_GetColliderSlot(thisx);
    if (slot >= 0) {
        Collider_UpdateCylinder(thisx, &sColliderPool[slot].collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &sColliderPool[slot].collider.base);
    }
}

// ============================================================================
// DRAW
// ============================================================================

static void SomariaCube_Draw(Actor* thisx, PlayState* play) {
    if (thisx->scale.x <= 0.001f) return;

    SomariaCubeColor colorIdx = SOMARIA_GET_COLOR(thisx);
    if (colorIdx >= SOMARIA_COLOR_MAX) colorIdx = SOMARIA_COLOR_BLUE;

    CubeColor* color = &sColorTable[colorIdx];

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    // Position and scale the cube
    Matrix_Translate(thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, MTXMODE_NEW);
    Matrix_RotateY(BINANG_TO_RAD(thisx->shape.rot.y), MTXMODE_APPLY);
    Matrix_Scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, color->r, color->g, color->b, 180);
    gDPSetEnvColor(POLY_XLU_DISP++, color->r/2, color->g/2, color->b/2, 180);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
        G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_XLU_DISP++, sSomariaCubeDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

// ============================================================================
// DESTROY
// ============================================================================

static void SomariaCube_DestroyFunc(Actor* thisx, PlayState* play) {
    // Free collider from pool
    SomariaCube_FreeCollider(play, thisx);

    if (sOriginalDestroy != NULL) {
        sOriginalDestroy(thisx, play);
    }
}

// ============================================================================
// SPAWN
// ============================================================================

Actor* SomariaCube_Spawn(PlayState* play, Vec3f* pos, s16 yaw) {
    Actor* cube = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_LIGHTBOX,
        pos->x, pos->y, pos->z, 0, yaw, 0, 0, true);

    if (cube == NULL) return NULL;

    EnLightbox* lightbox = (EnLightbox*)cube;

    if (sOriginalDestroy == NULL) {
        sOriginalDestroy = cube->destroy;
    }

    // Hijack actor functions
    cube->update = SomariaCube_Update;
    cube->draw = SomariaCube_Draw;
    cube->destroy = SomariaCube_DestroyFunc;

    // Remove En_Lightbox's DynaPoly
    if (lightbox->dyna.bgId != BGACTOR_NEG_ONE) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, lightbox->dyna.bgId);
        lightbox->dyna.bgId = BGACTOR_NEG_ONE;
    }

    // Allocate collider from static pool (AC for hookshot)
    SomariaCube_AllocCollider(play, cube);

    // Physics
    cube->gravity = SOMARIA_GRAVITY;
    cube->minVelocityY = SOMARIA_MIN_VEL_Y;

    // Flags
    cube->flags |= ACTOR_FLAG_CAN_PRESS_SWITCHES;
    cube->flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    cube->flags |= ACTOR_FLAG_SWITCHHOOKABLE;

    // No shadow
    cube->shape.shadowDraw = NULL;
    cube->shape.shadowScale = 0.0f;

    // Random color
    SomariaCubeColor randomColor = (SomariaCubeColor)(Rand_ZeroOne() * SOMARIA_COLOR_MAX);
    if (randomColor >= SOMARIA_COLOR_MAX) randomColor = SOMARIA_COLOR_BLUE;
    SOMARIA_SET_COLOR(cube, randomColor);

    // Spawn animation
    Actor_SetScale(cube, 0.0f);
    SOMARIA_SET_STATE(cube, SOMARIA_STATE_SPAWN);
    SOMARIA_SET_TIMER(cube, SOMARIA_SPAWN_FRAMES);

    // Sound
    SomariaCube_PlaySound(cube, NA_SE_PL_MAGIC_FIRE);

    return cube;
}
