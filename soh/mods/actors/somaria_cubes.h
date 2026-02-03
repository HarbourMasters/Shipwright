/**
 * Somaria Cubes System
 * Colored cubes spawned by Cane of Somaria (max 3)
 * - Random color on spawn
 * - Hookshotable
 * - Switchhookable (future)
 * - Knockback on enemies when thrown
 * - Can press switches
 *
 * Uses actor hijacking on En_Lightbox
 */

#ifndef SOMARIA_CUBES_H
#define SOMARIA_CUBES_H

#include "z64.h"

// ============================================================================
// CUBE PROPERTIES
// ============================================================================

#define SOMARIA_MAX_CUBES       3
#define SOMARIA_CUBE_SCALE      1.0f
#define SOMARIA_CUBE_SIZE       30
#define SOMARIA_SPAWN_FRAMES    15

// Physics
#define SOMARIA_GRAVITY         -2.0f
#define SOMARIA_THROW_VEL_Y     8.0f
#define SOMARIA_THROW_SPEED_XZ  6.0f
#define SOMARIA_MIN_VEL_Y       -20.0f

// Collision cylinder
#define SOMARIA_CYL_RADIUS      15
#define SOMARIA_CYL_HEIGHT      30
#define SOMARIA_MASS            180

// Knockback (AT damage when thrown)
#define SOMARIA_KNOCKBACK_DAMAGE    4

// ============================================================================
// COLOR PRESETS (random on spawn)
// ============================================================================

typedef enum {
    SOMARIA_COLOR_BLUE = 0,
    SOMARIA_COLOR_RED,
    SOMARIA_COLOR_GREEN,
    SOMARIA_COLOR_YELLOW,
    SOMARIA_COLOR_PURPLE,
    SOMARIA_COLOR_CYAN,
    SOMARIA_COLOR_ORANGE,
    SOMARIA_COLOR_MAX
} SomariaCubeColor;

// ============================================================================
// STATE MACROS (using hijacked actor fields)
// ============================================================================

#define SOMARIA_GET_STATE(actor)    ((actor)->home.rot.x)
#define SOMARIA_SET_STATE(actor, s) ((actor)->home.rot.x = (s))
#define SOMARIA_GET_TIMER(actor)    ((actor)->home.rot.z)
#define SOMARIA_SET_TIMER(actor, t) ((actor)->home.rot.z = (t))
#define SOMARIA_GET_COLOR(actor)    ((actor)->home.rot.y)
#define SOMARIA_SET_COLOR(actor, c) ((actor)->home.rot.y = (c))

typedef enum {
    SOMARIA_STATE_SPAWN = 0,
    SOMARIA_STATE_IDLE = 1,
    SOMARIA_STATE_HELD = 2,
    SOMARIA_STATE_THROWN = 3,
} SomariaCubeState;

// ============================================================================
// CUSTOM FLAGS
// ============================================================================

// Flag for switchhook compatibility (future use)
#define ACTOR_FLAG_SWITCHHOOKABLE   (1 << 28)

// ============================================================================
// BGCHECK FLAGS
// ============================================================================

#ifndef BGCHECKFLAG_GROUND
#define BGCHECKFLAG_GROUND      0x0001
#define BGCHECKFLAG_WALL        0x0008
#endif

// ============================================================================
// FUNCTIONS
// ============================================================================

Actor* SomariaCube_Spawn(PlayState* play, Vec3f* pos, s16 yaw);
u8 SomariaCube_IsSomariaCube(Actor* actor);
u8 SomariaCube_IsSwitchable(Actor* actor);
void SomariaCube_PlaySound(Actor* actor, u16 sfxId);

#endif // SOMARIA_CUBES_H
