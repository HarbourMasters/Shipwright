#ifndef Z64ACTOR_H
#define Z64ACTOR_H

#include "PR/ultratypes.h"
#include "stdbool.h"
#include "color.h"
#include "padutils.h"
#include "z64math.h"
#include "z64animation.h"
#include "z64collision_check.h"
#include "z64item.h"
#include "unk.h"
#include "code/actor/actor.h"

// This value is hardcoded to be the size of ovl_Arrow_Fire which currently is the biggest actor that uses the AM_FIELD.
#define AM_FIELD_SIZE SEGMENT_SIZE(ovl_Arrow_Fire)

#define MASS_IMMOVABLE 0xFF // Cannot be pushed by OC collisions
#define MASS_HEAVY 0xFE     // Can only be pushed by OC collisions with IMMOVABLE and HEAVY objects.

struct Actor;
struct ActorEntry;
struct CollisionContext;
struct CollisionHeader;
struct CollisionPoly;
struct GameState;
struct GraphicsContext;
struct Lights;
struct Player;
struct PlayState;

struct EnBox;
struct EnTorch2;

typedef void (*ActorFunc)(struct Actor*, struct PlayState* play); // 2S2H [Port] Removed reserved `this` keyword to fix compilation error 
typedef u16 (*NpcGetTextIdFunc)(struct PlayState*, struct Actor*);
typedef s16 (*NpcUpdateTalkStateFunc)(struct PlayState*, struct Actor*);

// #region 2S2H [Port] Reset func used for cleaning up static variables
typedef void (*ActorResetFunc)(void);
// #endregion

typedef struct PosRot {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3s rot;
} PosRot; // size = 0x14

typedef struct {
    /* 0x0 */ s16 unk_0; // frame?
    /* 0x2 */ Vec3s unk_2; // scale
} struct_80124618; // size = 0x8

typedef struct ActorProfile {
    /* 0x00 */ s16 id;
    /* 0x02 */ u8 type;
    /* 0x04 */ u32 flags;
    /* 0x08 */ s16 objectId;
    /* 0x0C */ u32 instanceSize;
    /* 0x10 */ ActorFunc init;
    /* 0x14 */ ActorFunc destroy;
    /* 0x18 */ ActorFunc update;
    /* 0x1C */ ActorFunc draw;
    // #region 2S2H [Port]
    /*      */ ActorResetFunc reset;
    // #endregion
} ActorProfile; // size = 0x20

typedef enum AllocType {
    /* 0 */ ALLOCTYPE_NORMAL,
    /* 1 */ ALLOCTYPE_ABSOLUTE,
    /* 2 */ ALLOCTYPE_PERMANENT
} AllocType;

typedef struct ActorOverlay {
    /* 0x00 */ uintptr_t vromStart;
    /* 0x04 */ uintptr_t vromEnd;
    /* 0x08 */ void* vramStart;
    /* 0x0C */ void* vramEnd;
    /* 0x10 */ void* loadedRamAddr; // original name: "allocp"
    /* 0x14 */ ActorProfile* profile;
    /* 0x18 */ char* name;
    /* 0x1C */ u16 allocType; // bit 0: don't allocate memory, use actorContext->0x250? bit 1: Always keep loaded?
    /* 0x1E */ s8 numLoaded; // original name: "clients"
} ActorOverlay; // size = 0x20

typedef void (*ActorShadowFunc)(struct Actor* actor, struct Lights* mapper, struct PlayState* play);

typedef struct {
    /* 0x00 */ Vec3s rot; // Current actor shape rotation
    /* 0x06 */ s16 face; // Used to index eyes and mouth textures. Only used by player
    /* 0x08 */ f32 yOffset; // Model y axis offset. Represents model space units
    /* 0x0C */ ActorShadowFunc shadowDraw; // Shadow draw function
    /* 0x10 */ f32 shadowScale; // Changes the size of the shadow
    /* 0x14 */ u8 shadowAlpha; // Default is 255
    /* 0x15 */ u8 feetFloorFlags; // Set if the actor's foot is clipped under the floor. & 1 is right foot, & 2 is left
    /* 0x16 */ u8 unk_16;
    /* 0x17 */ u8 unk_17;
    /* 0x18 */ Vec3f feetPos[2]; // Update by using `Actor_SetFeetPos` in PostLimbDrawOpa
} ActorShape; // size = 0x30

// Flags for bgCheckFlags
#define BGCHECKFLAG_GROUND (1 << 0) // Standing on the ground
#define BGCHECKFLAG_GROUND_TOUCH (1 << 1) // Has touched the ground (only active for 1 frame)
#define BGCHECKFLAG_GROUND_LEAVE (1 << 2) // Has left the ground (only active for 1 frame)
#define BGCHECKFLAG_WALL (1 << 3) // Touching a wall
#define BGCHECKFLAG_CEILING (1 << 4) // Touching a ceiling
#define BGCHECKFLAG_WATER (1 << 5) // In water
#define BGCHECKFLAG_WATER_TOUCH (1 << 6) // Has touched water (reset when leaving water)
#define BGCHECKFLAG_GROUND_STRICT (1 << 7) // Similar to BGCHECKFLAG_GROUND but with no velocity check and is cleared every frame
#define BGCHECKFLAG_CRUSHED (1 << 8) // Crushed between a floor and ceiling (triggers a void for player)
#define BGCHECKFLAG_PLAYER_WALL_INTERACT (1 << 9) // Only set/used by player, related to interacting with walls
#define BGCHECKFLAG_PLAYER_400 (1 << 10) // 
#define BGCHECKFLAG_PLAYER_800 (1 << 11) // 
#define BGCHECKFLAG_PLAYER_1000 (1 << 12) // 

// Flags for Actor_UpdateBgCheckInfo
#define UPDBGCHECKINFO_FLAG_1 (1 << 0) // check wall
#define UPDBGCHECKINFO_FLAG_2 (1 << 1) // check ceiling
#define UPDBGCHECKINFO_FLAG_4 (1 << 2) // check floor and water
#define UPDBGCHECKINFO_FLAG_8 (1 << 3)
#define UPDBGCHECKINFO_FLAG_10 (1 << 4)
#define UPDBGCHECKINFO_FLAG_20 (1 << 5) // unused
#define UPDBGCHECKINFO_FLAG_40 (1 << 6) // disable water ripples
#define UPDBGCHECKINFO_FLAG_80 (1 << 7)
#define UPDBGCHECKINFO_FLAG_100 (1 << 8)
#define UPDBGCHECKINFO_FLAG_200 (1 << 9)
#define UPDBGCHECKINFO_FLAG_400 (1 << 10) // check water
#define UPDBGCHECKINFO_FLAG_800 (1 << 11)

typedef struct Actor {
    /* 0x000 */ s16 id; // Actor ID
    /* 0x002 */ u8 category; // Actor category. Refer to the corresponding enum for values
    /* 0x003 */ s8 room; // Room number the actor is in. -1 denotes that the actor won't despawn on a room change
    /* 0x004 */ u32 flags; // Flags used for various purposes
    /* 0x008 */ PosRot home; // Initial position/rotation when spawned. Can be used for other purposes
    /* 0x01C */ s16 params; // Configurable variable set by the actor's spawn data; original name: "args_data"
    /* 0x01E */ s8 objectSlot; // Object slot (in ObjectContext) corresponding to the actor's object; original name: "bank"
    /* 0x01F */ s8 attentionRangeType; // Controls the attention actor range and the lock-on leash range. See `AttentionRangeType`.
    /* 0x020 */ s16 halfDaysBits; // Bitmask indicating which half-days this actor is allowed to not be killed(?) (TODO: not sure how to word this). If the current halfDayBit is not part of this mask then the actor is killed when spawning the setup actors
    /* 0x024 */ PosRot world; // Position/rotation in the world
    /* 0x038 */ s8 csId; // ActorCutscene index, see `CutsceneId`
    /* 0x039 */ u8 audioFlags; // Another set of flags? Seems related to sfx or bgm
    /* 0x03C */ PosRot focus; // Target reticle focuses on this position. For player this represents head pos and rot
    /* 0x050 */ u16 sfxId; // Id of sound effect to play. Plays when value is set, then is cleared the following update cycle
    /* 0x054 */ f32 lockOnArrowOffset; // Height offset of the lock-on arrow relative to `focus` position
    /* 0x058 */ Vec3f scale; // Scale of the actor in each axis
    /* 0x064 */ Vec3f velocity; // Velocity of the actor in each axis
    /* 0x070 */ f32 speed; // Context dependent speed value. Can be used for XZ or XYZ depending on which move function is used
    /* 0x074 */ f32 gravity; // Acceleration due to gravity. Value is added to Y velocity every frame
    /* 0x078 */ f32 terminalVelocity; // Sets the lower bounds cap on velocity along the Y axis
    /* 0x07C */ struct CollisionPoly* wallPoly; // Wall polygon the actor is touching
    /* 0x080 */ struct CollisionPoly* floorPoly; // Floor polygon directly below the actor
    /* 0x084 */ u8 wallBgId; // Bg ID of the wall polygon the actor is touching
    /* 0x085 */ u8 floorBgId; // Bg ID of the floor polygon directly below the actor
    /* 0x086 */ s16 wallYaw; // Y rotation of the wall polygon the actor is touching
    /* 0x088 */ f32 floorHeight; // Y position of the floor polygon directly below the actor
    /* 0x08C */ f32 depthInWater; // Directed distance to the surface of active waterbox. Negative value means water is below.
    /* 0x090 */ u16 bgCheckFlags; // Flags indicating how the actor is interacting with collision
    /* 0x092 */ s16 yawTowardsPlayer; // Y rotation difference between the actor and the player
    /* 0x094 */ f32 xyzDistToPlayerSq; // Squared distance between the actor and the player in the x,y,z axis
    /* 0x098 */ f32 xzDistToPlayer; // Distance between the actor and the player in the XZ plane
    /* 0x09C */ f32 playerHeightRel; // Directed distance is negative if the player is below.
    /* 0x0A0 */ CollisionCheckInfo colChkInfo; // Variables related to the Collision Check system
    /* 0x0BC */ ActorShape shape; // Variables related to the physical shape of the actor
    /* 0x0EC */ Vec3f projectedPos; // Position of the actor in projected space
    /* 0x0F8 */ f32 projectedW; // w component of the projected actor position
    /* 0x0FC */ f32 cullingVolumeDistance; // Forward distance of the culling volume (in projected space). See `Actor_CullingCheck` and `Actor_CullingVolumeTest` for more information.
    /* 0x100 */ f32 cullingVolumeScale; // Scale of the culling volume (in projected space). See `Actor_CullingCheck` and `Actor_CullingVolumeTest` for more information.
    /* 0x104 */ f32 cullingVolumeDownward; // Downward height of the culling volume (in projected space). See `Actor_CullingCheck` and `Actor_CullingVolumeTest` for more information.
    /* 0x108 */ Vec3f prevPos; // World position from the previous update cycle
    /* 0x114 */ u8 isLockedOn; // Set to true if the actor is currently being targeted by the player
    /* 0x115 */ u8 targetPriority; // Lower values have higher priority. Resets to 0 when player stops targeting
    /* 0x116 */ u16 textId; // Text ID to pass to link/display when interacting with the actor
    /* 0x118 */ u16 freezeTimer; // Actor does not update when set. Timer decrements automatically
    /* 0x11A */ u16 colorFilterParams; // Set color filter to red, blue, or white. Toggle opa or xlu
    /* 0x11C */ u8 colorFilterTimer; // A non-zero value enables the color filter. Decrements automatically
    /* 0x11D */ u8 isDrawn; // Set to true if the actor is currently being drawn. Always stays false for lens actors
    /* 0x11E */ u8 dropFlag; // Configures what item is dropped by the actor from `Item_DropCollectibleRandom`
    /* 0x11F */ u8 hintId; // Sets what 0600 dialog to display when talking to Tatl. Default 0xFF
    /* 0x120 */ struct Actor* parent; // Usage is actor specific. Set if actor is spawned via `Actor_SpawnAsChild`
    /* 0x124 */ struct Actor* child; // Usage is actor specific. Set if actor is spawned via `Actor_SpawnAsChild`
    /* 0x128 */ struct Actor* prev; // Previous actor of this category
    /* 0x12C */ struct Actor* next; // Next actor of this category
    /* 0x130 */ ActorFunc init; // Initialization Routine. Called by `Actor_InitContext` or `Actor_UpdateAll`
    /* 0x134 */ ActorFunc destroy; // Destruction Routine. Called by `Actor_Destroy`
    /* 0x138 */ ActorFunc update; // Update Routine. Called by `Actor_UpdateAll`
    /* 0x13C */ ActorFunc draw; // Draw Routine. Called by `Actor_Draw`
    /* 0x140 */ ActorOverlay* overlayEntry; // Pointer to the overlay table entry for this actor
} Actor; // size = 0x144

typedef enum {
    /* 0 */ FOOT_LEFT,
    /* 1 */ FOOT_RIGHT
} ActorFootIndex;

#define DYNA_TRANSFORM_POS (1 << 0) // Position of the actors on top follows the dynapoly actor's movement.
#define DYNA_TRANSFORM_ROT_Y (1 << 1) // The Y rotation of the actors on top follows the dynapoly actor's Y rotation.

#define DYNA_INTERACT_ACTOR_ON_TOP (1 << 0) // There is an actor standing on the collision of the dynapoly actor
#define DYNA_INTERACT_PLAYER_ON_TOP (1 << 1) // The player actor is standing on the collision of the dynapoly actor
#define DYNA_INTERACT_PLAYER_ABOVE (1 << 2) // The player is directly above the collision of the dynapoly actor (any distance above)
#define DYNA_INTERACT_ACTOR_SWITCH_PRESSED (1 << 3) // An actor that is capable of pressing switches is on top of the dynapoly actor
#define DYNA_INTERACT_ACTOR_HEAVY_SWITCH_PRESSED (1 << 4) // An actor that is capable of pressing heavy switches is on top of the dynapoly actor

typedef struct {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s32 bgId;
    /* 0x148 */ f32 pushForce;
    /* 0x14C */ f32 unk14C;
    /* 0x150 */ s16 yRotation;
    /* 0x154 */ u32 transformFlags;
    /* 0x158 */ u8 interactFlags;
} DynaPolyActor; // size = 0x15C

typedef enum Item00Type {
    /* 0x00 */ ITEM00_RUPEE_GREEN,
    /* 0x01 */ ITEM00_RUPEE_BLUE,
    /* 0x02 */ ITEM00_RUPEE_RED,
    /* 0x03 */ ITEM00_RECOVERY_HEART,
    /* 0x04 */ ITEM00_BOMBS_A,
    /* 0x05 */ ITEM00_ARROWS_10,
    /* 0x06 */ ITEM00_HEART_PIECE,
    /* 0x07 */ ITEM00_HEART_CONTAINER,
    /* 0x08 */ ITEM00_ARROWS_30,
    /* 0x09 */ ITEM00_ARROWS_40,
    /* 0x0A */ ITEM00_ARROWS_50,
    /* 0x0B */ ITEM00_BOMBS_B,
    /* 0x0C */ ITEM00_DEKU_NUTS_1,
    /* 0x0D */ ITEM00_DEKU_STICK,
    /* 0x0E */ ITEM00_MAGIC_JAR_BIG,
    /* 0x0F */ ITEM00_MAGIC_JAR_SMALL,
    /* 0x10 */ ITEM00_MASK,
    /* 0x11 */ ITEM00_SMALL_KEY,
    /* 0x12 */ ITEM00_FLEXIBLE,
    /* 0x13 */ ITEM00_RUPEE_HUGE,
    /* 0x14 */ ITEM00_RUPEE_PURPLE,
    /* 0x15 */ ITEM00_3_HEARTS,
    /* 0x16 */ ITEM00_SHIELD_HERO,
    /* 0x17 */ ITEM00_DEKU_NUTS_10,
    /* 0x18 */ ITEM00_NOTHING,
    /* 0x19 */ ITEM00_BOMBS_0,
    /* 0x1A */ ITEM00_BIG_FAIRY,
    /* 0x1B */ ITEM00_MAP,
    /* 0x1C */ ITEM00_COMPASS,
    /* 0x1D */ ITEM00_MUSHROOM_CLOUD,

    /* 0xFF */ ITEM00_NO_DROP = -1
} Item00Type;

struct EnItem00;

typedef void (*EnItem00ActionFunc)(struct EnItem00*, struct PlayState*);

#define ENITEM00_GET_8000(thisx) ((thisx)->params & 0x8000)
#define ENITEM00_GET_7F00(thisx) (((thisx)->params & 0x7F00) >> 8)

typedef struct EnItem00 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnItem00ActionFunc actionFunc;
    /* 0x148 */ s16 collectibleFlag;
    /* 0x14A */ s16 getItemId;
    /* 0x14C */ s16 unk14C;
    /* 0x14E */ s16 unk14E;
    /* 0x150 */ s16 unk150;
    /* 0x152 */ s16 unk152;
    /* 0x154 */ f32 unk154;
    /* 0x158 */ ColliderCylinder collider;
    /* 0x1A4 */ s8 unk1A4;
} EnItem00; // size = 0x1A8

struct EnAObj;

typedef void (*EnAObjActionFunc)(struct EnAObj*, struct PlayState*);

typedef struct EnAObj {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnAObjActionFunc actionFunc;
    /* 0x148 */ ColliderCylinder collision;
} EnAObj; // size = 0x194

typedef enum {
    /* 0 */ AOBJ_SIGNPOST_OBLONG,
    /* 1 */ AOBJ_SIGNPOST_ARROW
} AObjType;

#define AOBJ_GET_TEXTID(thisx) ((((thisx)->params >> 8) & 0xFF) | 0x300)
#define AOBJ_GET_TYPE(thisx) (((thisx)->params & 0xFF) - 9)

#define AOBJ_PARAMS(textId, type) ((((textId - 0x300) & 0xFF) << 8) | (type + 9))

typedef enum {
    /* 0x00 */ ACTORCAT_SWITCH,
    /* 0x01 */ ACTORCAT_BG,
    /* 0x02 */ ACTORCAT_PLAYER,
    /* 0x03 */ ACTORCAT_EXPLOSIVES,
    /* 0x04 */ ACTORCAT_NPC,
    /* 0x05 */ ACTORCAT_ENEMY,
    /* 0x06 */ ACTORCAT_PROP,
    /* 0x07 */ ACTORCAT_ITEMACTION,
    /* 0x08 */ ACTORCAT_MISC,
    /* 0x09 */ ACTORCAT_BOSS,
    /* 0x0A */ ACTORCAT_DOOR,
    /* 0x0B */ ACTORCAT_CHEST,
    /* 0x0C */ ACTORCAT_MAX
} ActorType;

#define ACTORCTX_FLAG_0 (1 << 0)
#define ACTORCTX_FLAG_TELESCOPE_ON (1 << 1)
#define ACTORCTX_FLAG_PICTO_BOX_ON (1 << 2)
#define ACTORCTX_FLAG_3 (1 << 3)
#define ACTORCTX_FLAG_4 (1 << 4)
#define ACTORCTX_FLAG_5 (1 << 5)
#define ACTORCTX_FLAG_6 (1 << 6)
#define ACTORCTX_FLAG_7 (1 << 7)

/**
 * Attention System
 *
 * The Attention System's responsibility is to bring specific actors to the player's attention.
 * This includes:
 *     - Making Tatl fly over to certain actors of interest (can be lock-on actors, but may also not be)
 *     - Displaying an arrow over an actor that can be locked onto
 *     - Displaying a reticle over the current lock-on actor
 *     - Playing enemy background music if a hostile actor is nearby
 *     - Drift the camera towards an actor (unused)
 *
 * This system does not handle the actual lock-on implementation.
 * That is the responsibility of Player via `player->focusActor` and the camera.
 */

// A set of 4 triangles which appear as a ring around an actor when locked-on
typedef struct LockOnReticle {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 radius; // distance towards the center of the locked-on actor
    /* 0x10 */ Color_RGBA8 color;
} LockOnReticle; // size = 0x14

typedef struct Attention {
    /* 0x00 */ Vec3f tatlHoverPos;  // Tatl's current hover position
    /* 0x0C */ Vec3f reticlePos; // Main reticle pos which each `LockOnReticle` instance can reference
    /* 0x18 */ Color_RGBAf tatlInnerColor; // Tatl inner color, based on actor category
    /* 0x28 */ Color_RGBAf tatlOuterColor; // Tatl outer color, based on actor category
    /* 0x38 */ Actor* tatlHoverActor; // The actor that Tatl hovers over
    /* 0x3C */ Actor* reticleActor; // Actor to draw a reticle over
    /* 0x40 */ f32 tatlMoveProgressFactor; // Controls Tatl so she can smootly transition to an actor
    /* 0x44 */ f32 reticleRadius; // Main reticle radius value which each `LockOnReticle` instance can reference
    /* 0x48 */ s16 reticleFadeAlphaControl; // Set and fade the reticle alpha; Non-zero values control if it should draw
    /* 0x4A */ u8 tatlHoverActorCategory; // Category of the actor Tatl is currently hovering over
    /* 0x4B */ u8 reticleSpinCounter; // Counts up when a reticle is active, used for the spinning animation
    /* 0x4C */ s8 curReticle; // Indexes lockOnReticles[]
    /* 0x50 */ LockOnReticle lockOnReticles[3]; // Multiple reticles are used for a motion-blur effect
    /* 0x8C */ Actor* forcedLockOnActor; // Forces lock-on to this actor when set (never used in practice)
    /* 0x90 */ Actor* bgmEnemy; // The nearest actor which can trigger enemy background music
    /* 0x94 */ Actor* arrowHoverActor; // Actor to draw an arrow over
} Attention; // size = 0x98

// It is difficult to give each type a name because it is numerically based
// and there are so many different combinations.
// Each type has a comment of the form "attention range / lock-on leash range"
typedef enum AttentionRangeType {
    /*  0 */ ATTENTION_RANGE_0,  // 70   / 140
    /*  1 */ ATTENTION_RANGE_1,  // 170  / 255
    /*  2 */ ATTENTION_RANGE_2,  // 280  / 5600
    /*  3 */ ATTENTION_RANGE_3,  // 350  / 525   (default)
    /*  4 */ ATTENTION_RANGE_4,  // 700  / 1050
    /*  5 */ ATTENTION_RANGE_5,  // 1000 / 1500
    /*  6 */ ATTENTION_RANGE_6,  // 100  / 105.36842
    /*  7 */ ATTENTION_RANGE_7,  // 140  / 163.33333
    /*  8 */ ATTENTION_RANGE_8,  // 240  / 576
    /*  9 */ ATTENTION_RANGE_9,  // 280  / 280000
    /* 10 */ ATTENTION_RANGE_10, // 2500 / 3750
    /* 11 */ ATTENTION_RANGE_MAX
} AttentionRangeType;

typedef struct AttentionRangeParams {
    /* 0x0 */ f32 attentionRangeSq;
    /* 0x4 */ f32 lockOnLeashScale;
} AttentionRangeParams; // size = 0x8

typedef struct {
    /* 0x0 */ TexturePtr texture;
    /* 0x4 */ s16 x;
    /* 0x6 */ s16 y;
    /* 0x8 */ u8 width;
    /* 0x9 */ u8 height;
    /* 0xA */ u8 durationTimer; // how long the title card appears for before fading
    /* 0xB */ u8 delayTimer; // how long the title card waits to appear
    /* 0xC */ s16 alpha;
    /* 0xE */ s16 intensity;
} TitleCardContext; // size = 0x10

typedef enum {
    /* 0 */ PLAYER_IMPACT_GORON_GROUND_POUND,
    /* 1 */ PLAYER_IMPACT_ZORA_BARRIER,
    /* 2 */ PLAYER_IMPACT_BONK // also activated by goron attack
} PlayerImpactType;

typedef struct PlayerImpact {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 timer;
    /* 0x04 */ f32 dist;
    /* 0x08 */ Vec3f pos;
} PlayerImpact; // size = 0x14

typedef struct ActorSharedMemoryEntry {
    /* 0x0 */ s16 id;
    /* 0x2 */ s8 isDynamicallyInitialised;
    /* 0x4 */ void* ptr;
} ActorSharedMemoryEntry; // size = 0x8

#define SWITCH_FLAG_NONE -1

typedef struct ActorContextSceneFlags {
    /* 0x00 */ u32 switches[4]; // First 0x40 are permanent, second 0x40 are temporary
    /* 0x10 */ u32 chest;
    /* 0x14 */ u32 clearedRoom;
    /* 0x18 */ u32 clearedRoomTemp;
    /* 0x1C */ u32 collectible[4]; // bitfield of 128 bits
} ActorContextSceneFlags; // size = 0x2C

typedef struct ActorListEntry {
    /* 0x0 */ s32 length; // number of actors loaded of this type
    /* 0x4 */ Actor* first; // pointer to first actor of this type
    /* 0x8 */ s32 categoryChanged; // at least one actor has changed categories and needs to be moved to a different list
} ActorListEntry; // size = 0xC

typedef enum {
    /* 0 */ LENS_MODE_SHOW_ACTORS, // lens actors are invisible by default, and shown by using lens (for example, invisible enemies)
    /* 1 */ LENS_MODE_HIDE_ACTORS // lens actors are visible by default, and hidden by using lens (for example, fake walls)
} LensMode;

#define LENS_ACTOR_MAX 32

// Target size when activated
#define LENS_MASK_ACTIVE_SIZE 100

#define HALFDAYBIT_DAY0_DAWN  (1 << 9)
#define HALFDAYBIT_DAY0_NIGHT (1 << 8)
#define HALFDAYBIT_DAY1_DAWN  (1 << 7)
#define HALFDAYBIT_DAY1_NIGHT (1 << 6)
#define HALFDAYBIT_DAY2_DAWN  (1 << 5)
#define HALFDAYBIT_DAY2_NIGHT (1 << 4)
#define HALFDAYBIT_DAY3_DAWN  (1 << 3)
#define HALFDAYBIT_DAY3_NIGHT (1 << 2)
#define HALFDAYBIT_DAY4_DAWN  (1 << 1)
#define HALFDAYBIT_DAY4_NIGHT (1 << 0)

#define HALFDAYBIT_DAWNS  (HALFDAYBIT_DAY0_DAWN | HALFDAYBIT_DAY1_DAWN | HALFDAYBIT_DAY2_DAWN | HALFDAYBIT_DAY3_DAWN | HALFDAYBIT_DAY4_DAWN)
#define HALFDAYBIT_NIGHTS (HALFDAYBIT_DAY0_NIGHT | HALFDAYBIT_DAY1_NIGHT | HALFDAYBIT_DAY2_NIGHT | HALFDAYBIT_DAY3_NIGHT | HALFDAYBIT_DAY4_NIGHT)
#define HALFDAYBIT_ALL    (HALFDAYBIT_DAWNS | HALFDAYBIT_NIGHTS)

typedef struct ActorContext {
    /* 0x000 */ u8 freezeFlashTimer;
    /* 0x001 */ UNK_TYPE1 pad1;
    /* 0x002 */ u8 unk2;
    /* 0x003 */ u8 lensActive;
    /* 0x004 */ s8 lensMaskSize; // The size of the circle when drawn the lens mask. Larger value leads to a smaller circle
    /* 0x005 */ u8 flags;
    /* 0x006 */ UNK_TYPE1 pad6[0x5];
    /* 0x00B */ s8 lensActorsDrawn;
    /* 0x00C */ s16 halfDaysBit; // A single bit indicating the current half-day. It is one of HALFDAYBIT_DAYX_ macro values
    /* 0x00E */ u8 totalLoadedActors;
    /* 0x00F */ u8 numLensActors;
    /* 0x010 */ ActorListEntry actorLists[ACTORCAT_MAX];
    /* 0x0A0 */ Actor* lensActors[LENS_ACTOR_MAX]; // Draws up to LENS_ACTOR_MAX number of invisible actors
    /* 0x120 */ Attention attention;
    /* 0x1B8 */ ActorContextSceneFlags sceneFlags;
    /* 0x1E4 */ TitleCardContext titleCtx;
    /* 0x1F4 */ PlayerImpact playerImpact;
    /* 0x208 */ UNK_TYPE1 unk_208[0x4];
    /* 0x20C */ ActorSharedMemoryEntry actorSharedMemory[8];
    /* 0x24C */ UNK_TYPE1 unk_24C[0x4];
    /* 0x250 */ void* absoluteSpace; // Space used to allocate actor overlays of alloc type ALLOCTYPE_ABSOLUTE
    /* 0x254 */ struct EnTorch2* elegyShells[5]; // PLAYER_FORM_MAX
    /* 0x268 */ u8 isOverrideInputOn;
    /* 0x269 */ UNK_TYPE1 pad269[0x3];
    /* 0x26C */ Input overrideInput;
} ActorContext; // size = 0x284

typedef enum {
    /*  0 */ ACTOR_DRAW_DMGEFF_FIRE,
    /*  1 */ ACTOR_DRAW_DMGEFF_BLUE_FIRE,
    /* 10 */ ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX = 10,
    /* 11 */ ACTOR_DRAW_DMGEFF_FROZEN_SFX,
    /* 20 */ ACTOR_DRAW_DMGEFF_LIGHT_ORBS = 20,
    /* 21 */ ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS,
    /* 30 */ ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL = 30,
    /* 31 */ ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM,
    /* 32 */ ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE
} ActorDrawDamageEffectType;

// 2S2H Added columns to actor table: _humanName
#define DEFINE_ACTOR(_name, enumValue, _allocType, _debugName, _humanName) enumValue,
#define DEFINE_ACTOR_INTERNAL(_name, enumValue, _allocType, _debugName, _humanName) enumValue,
#define DEFINE_ACTOR_UNSET(enumValue) enumValue,

typedef enum ActorId {
    #include "tables/actor_table.h"
    /* 0x2B2 */ ACTOR_ID_MAX // originally "ACTOR_DLF_MAX"
} ActorId;

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

typedef enum DoorLockType {
    /* 0 */ DOORLOCK_NORMAL,
    /* 1 */ DOORLOCK_BOSS,
    /* 2 */ DOORLOCK_2, // DOORLOCK_NORMAL_SPIRIT on OoT
    /* 3 */ DOORLOCK_MAX
} DoorLockType;

// Actor is discoverable by the Attention System. This enables Tatl to hover over the actor when it is in range.
// The actor can also be locked onto (as long as `ACTOR_FLAG_LOCK_ON_DISABLED` is not set).
#define ACTOR_FLAG_ATTENTION_ENABLED (1 << 0)

// Unused
#define ACTOR_FLAG_2 (1 << 1)

// Actor is hostile toward the Player. Player has specific "battle" behavior when locked onto hostile actors.
// Enemy background music will also be played when the player is close enough to a hostile actor.
// Note: This must be paired with `ACTOR_FLAG_ATTENTION_ENABLED` to have any effect.
#define ACTOR_FLAG_HOSTILE (1 << 2)

// Actor is considered "friendly"; Opposite flag of `ACTOR_FLAG_HOSTILE`.
// Note that this flag doesn't have any effect on either the actor, or Player's behavior.
// What actually matters is the presence or lack of `ACTOR_FLAG_HOSTILE`.
#define ACTOR_FLAG_FRIENDLY (1 << 3)

// Culling of the actor's update process is disabled.
// In other words, the actor will keep updating even if the actor is outside its own culling volume.
// See `Actor_CullingCheck` for more information about culling.
// See `Actor_CullingVolumeTest` for more information on the test used to determine if an actor should be culled.
#define ACTOR_FLAG_UPDATE_CULLING_DISABLED (1 << 4)

// Culling of the actor's draw process is disabled.
// In other words, the actor will keep drawing even if the actor is outside its own culling volume.
// See `Actor_CullingCheck` for more information about culling.
// See `Actor_CullingVolumeTest` for more information on the test used to determine if an actor should be culled.
// (The original name for this flag is `NO_CULL_DRAW`, known from the Majora's Mask Debug ROM)
#define ACTOR_FLAG_DRAW_CULLING_DISABLED (1 << 5)

// Set if the actor is currently within the bounds of its culling volume.
// In most cases, this flag can be used to determine whether or not an actor is currently culled.
// However this flag still updates even if `ACTOR_FLAG_UPDATE_CULLING_DISABLED` or `ACTOR_FLAG_DRAW_CULLING_DISABLED`
// are set. Meaning, the flag can still have a value of "false" even if it is not actually culled.
// (The original name for this flag is `NO_CULL_FLAG`, known from the Majora's Mask Debug ROM)
#define ACTOR_FLAG_INSIDE_CULLING_VOLUME (1 << 6)

// hidden or revealed by Lens of Truth (depending on room lensMode)
#define ACTOR_FLAG_REACT_TO_LENS (1 << 7)

// Signals that player has accepted an offer to talk to an actor
// Player will retain this flag until the player is finished talking
// Actor will retain this flag until `Actor_TalkOfferAccepted` is called or manually turned off by the actor
#define ACTOR_FLAG_TALK (1 << 8)

// When the hookshot attaches to this actor, the actor will be pulled back as the hookshot retracts.
#define ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR (1 << 9)

// When the hookshot attaches to this actor, Player will be pulled by the hookshot and fly to the actor.
#define ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER (1 << 10)

// This is likely `ACTOR_FLAG_GRASS_DESTROYED` from OoT, however this flag is unused in this game.
#define ACTOR_FLAG_800 (1 << 11)

// Actor will not shake when a quake occurs
#define ACTOR_FLAG_IGNORE_QUAKE (1 << 12)

// The hookshot is currently attached to this actor.
// The behavior that occurs after attachment is determined by `ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR` and `ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER`.
// If neither of those flags are set attachment cannot occur, and the hookshot will simply act as a damage source.
//
// This flag is also reused to indicate that an actor is attached to the Zora boomerang.
// This only has an effect for Gold Skulltula Tokens (EN_SI) which has overlapping behavior for hookshot and boomerang.
#define ACTOR_FLAG_HOOKSHOT_ATTACHED (1 << 13)

// When hit by an arrow, the actor will be able to attach to the arrow and fly with it in the air
#define ACTOR_FLAG_CAN_ATTACH_TO_ARROW (1 << 14)

// Actor is currently attached to an arrow and flying with it in the air
#define ACTOR_FLAG_ATTACHED_TO_ARROW (1 << 15)

// Player automatically accepts a Talk Offer without needing to press the A button.
// Player still has to meet all conditions to be able to receive a talk offer (for example, being in range).
#define ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED (1 << 16)

// Actor can press and hold down heavy switches.
// See usages of `DynaPolyActor_SetHeavySwitchPressed` and `DynaPolyActor_IsHeavySwitchPressed` for more context on how switches work.
#define ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES  (1 << 17)

// When locked onto an actor with this flag set, the C-Up button can be used to talk to this actor.
// A C-Up button labeled "Tatl" will appear on the HUD when locked on which indicates the actor can be checked with Tatl.
// With this flag Player talks directly to the actor with C-Up. It is expected that the resulting dialog should appear
// to be coming from Tatl, even though she is not involved at all with this interaction.
#define ACTOR_FLAG_TALK_WITH_C_UP (1 << 18)

// Allows for the attention system to refind the focus actor in the search for the next attention actor.
// Also allows for the next lock-on actor to be the focus actor again.
// When chosen as the next lock-on actor, this flag is unset.
#define ACTOR_FLAG_FOCUS_ACTOR_REFINDABLE (1 << 19)

// Actor can update even if Player is currently in one of the `sCategoryFreezeMasks` states.
// Typically an actor will halt while the player is in one of the `sCategoryFreezeMasks` states (depending on category).
// This flag allows a given actor to be an exception.
#define ACTOR_FLAG_FREEZE_EXCEPTION (1 << 20)

// Actor can update even if the Song of Soaring Cutscene or the Song of Time Cutscene is playing.
// Typically an actor will halt while the Song of Soaring Cutscene or the Song of Time Cutscene is playing.
// This flag allows a given actor to be an exception.
#define ACTOR_FLAG_UPDATE_DURING_SOARING_AND_SOT_CS (1 << 21)

// Specifies whether the actor can (not) use fake point lights, in the event that ucode point lights are not compatible with its display lists.
// In F3DZEX2 versions that predate MM, microcode point lights didn't exist so `PointLight_t` could not be used.
// Instead, fake point lights by using a directional light that constantly changes to face a reference position.
#define ACTOR_FLAG_IGNORE_LEGACY_POINT_LIGHTS (1 << 22)

// When Player is carrying this actor, it can only be thrown, not dropped/placed.
// Typically an actor can only be thrown when moving, but this allows an actor to be thrown when standing still.
#define ACTOR_FLAG_THROW_ONLY (1 << 23)

// When colliding with Player's body AC collider, a "thump" sound will play indicating his body has been hit
#define ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT (1 << 24)

// Actor can update even if Player is currently using the ocarina.
// Typically an actor will halt while the ocarina is active (depending on category).
// This flag allows a given actor to be an exception.
#define ACTOR_FLAG_UPDATE_DURING_OCARINA (1 << 25)

// Actor can press and hold down switches.
// See usages of `DynaPolyActor_SetSwitchPressed` and `DynaPolyActor_IsSwitchPressed` for more context on how switches work.
#define ACTOR_FLAG_CAN_PRESS_SWITCHES (1 << 26)

// Player is not able to lock onto the actor.
// Tatl will still be able to hover over the actor, assuming `ACTOR_FLAG_ATTENTION_ENABLED` is set.
#define ACTOR_FLAG_LOCK_ON_DISABLED (1 << 27)

// Specifies whether subsequent geometry is compatible with ucode point lights.
// The current room must also enable point lights for point lights to take effect.
#define ACTOR_FLAG_UCODE_POINT_LIGHT_ENABLED (1 << 28)

// Signals that player has accepted an offer to use the ocarina to interact with an actor
// An exception is made for EN_ZOT, see `Player_ActionHandler_13`.
// Player will retain this flag until the player is finished playing the ocarina
// Actor will retain this flag until `Actor_OcarinaInteractionAccepted` is called or manually turned off by the actor
#define ACTOR_FLAG_OCARINA_INTERACTION (1 << 29)

// Camera will slowly drift to the actor while approaching it.
// Uses the attention system but `ACTOR_FLAG_ATTENTION_ENABLED` is not required.
#define ACTOR_FLAG_CAMERA_DRIFT_ENABLED (1 << 30)

// The actor's location will be marked on the minimap.
// If the actor is a player actor, a compass icon will be drawn.
// If the actor is EN_BOX, the flag has no effect.
// Otherwise a square icon is drawn, with the color determined by it's actor category.
#define ACTOR_FLAG_MINIMAP_ICON_ENABLED (1 << 31)

#define DROPFLAG_NONE   (0)
#define DROPFLAG_1      (1 << 0)
#define DROPFLAG_2      (1 << 1)
#define DROPFLAG_20     (1 << 5)

#define COLORFILTER_GET_COLORFLAG(colorFilterParams) ((colorFilterParams) & 0xC000)
#define COLORFILTER_GET_COLORINTENSITY(colorFilterParams) (((colorFilterParams) & 0x1F00) >> 5)
#define COLORFILTER_GET_DURATION(colorFilterParams) ((colorFilterParams) & 0xFF)

#define COLORFILTER_COLORFLAG_NONE 0xC000
#define COLORFILTER_COLORFLAG_GRAY 0x8000
#define COLORFILTER_COLORFLAG_RED  0x4000
#define COLORFILTER_COLORFLAG_BLUE 0x0000

#define COLORFILTER_INTENSITY_FLAG 0x8000

#define COLORFILTER_BUFFLAG_XLU    0x2000
#define COLORFILTER_BUFFLAG_OPA    0x0000

typedef enum {
    /* 0x00 */ TATL_HINT_ID_DEFAULT,
    /* 0x01 */ TATL_HINT_ID_SNAPPER,
    /* 0x02 */ TATL_HINT_ID_MINI_BABA,
    /* 0x03 */ TATL_HINT_ID_MAD_JELLY,
    /* 0x04 */ TATL_HINT_ID_SKULLTULA,
    /* 0x05 */ TATL_HINT_ID_RED_CHUCHU,
    /* 0x06 */ TATL_HINT_ID_BLUE_CHUCHU,
    /* 0x07 */ TATL_HINT_ID_DEKU_BABA,
    /* 0x08 */ TATL_HINT_ID_BIO_DEKU_BABA,
    /* 0x09 */ TATL_HINT_ID_WILTED_DEKU_BABA,
    /* 0x0A */ TATL_HINT_ID_DEXIHAND,
    /* 0x0B */ TATL_HINT_ID_NEJIRON,
    /* 0x0C */ TATL_HINT_ID_GIANT_BEE,
    /* 0x0D */ TATL_HINT_ID_DODONGO,
    /* 0x0E */ TATL_HINT_ID_DEEP_PYTHON,
    /* 0x0F */ TATL_HINT_ID_DEATH_ARMOS,
    /* 0x10 */ TATL_HINT_ID_DINOLFOS,
    /* 0x11 */ TATL_HINT_ID_FIRE_KEESE,
    /* 0x12 */ TATL_HINT_ID_KEESE,
    /* 0x13 */ TATL_HINT_ID_ARMOS,
    /* 0x14 */ TATL_HINT_ID_EENO,
    /* 0x15 */ TATL_HINT_ID_DRAGONFLY,
    /* 0x16 */ TATL_HINT_ID_IGOS_DU_IKANA,
    /* 0x17 */ TATL_HINT_ID_GARO,
    /* 0x18 */ TATL_HINT_ID_GARO_MASTER,
    /* 0x19 */ TATL_HINT_ID_WART,
    /* 0x1A */ TATL_HINT_ID_GOMESS,
    /* 0x1B */ TATL_HINT_ID_GOHT,
    /* 0x1C */ TATL_HINT_ID_BLUE_BUBBLE,
    /* 0x1D */ TATL_HINT_ID_KINGS_LACKEYS_DARK_ROOM,
    /* 0x1E */ TATL_HINT_ID_KINGS_LACKEYS_LIGHT_ROOM,
    /* 0x1F */ TATL_HINT_ID_SKULLWALLTULA,
    /* 0x20 */ TATL_HINT_ID_GOLD_SKULLTULA, // Only in JP Ver.
    /* 0x21 */ TATL_HINT_ID_CAPTAIN_KEETA,
    /* 0x22 */ TATL_HINT_ID_YELLOW_CHUCHU,
    /* 0x23 */ TATL_HINT_ID_TAKKURI,
    /* 0x24 */ TATL_HINT_ID_RED_BUBBLE,
    /* 0x25 */ TATL_HINT_ID_HIPLOOP,
    /* 0x26 */ TATL_HINT_ID_MASKED_HIPLOOP,
    /* 0x27 */ TATL_HINT_ID_SHELLBLADE,
    /* 0x28 */ TATL_HINT_ID_GYORG, // "if you go near, you'll be eaten!"
    /* 0x29 */ TATL_HINT_ID_GYORG_STUNNED, // "Jump in and attack it!"
    /* 0x2A */ TATL_HINT_ID_REDEAD,
    /* 0x2B */ TATL_HINT_ID_2B,
    /* 0x2C */ TATL_HINT_ID_BLACK_BOE,
    /* 0x2D */ TATL_HINT_ID_GIBDO,
    /* 0x2E */ TATL_HINT_ID_TWINMOLD,
    /* 0x2F */ TATL_HINT_ID_WEARING_GIANTS_MASK, // Twinmold, JP Ver. only
    /* 0x30 */ TATL_HINT_ID_WALLMASTER,
    /* 0x31 */ TATL_HINT_ID_FLOORMASTER,
    /* 0x32 */ TATL_HINT_ID_MAJORAS_MASK,
    /* 0x33 */ TATL_HINT_ID_MAJORAS_INCARNATION,
    /* 0x34 */ TATL_HINT_ID_MAJORAS_WRATH,
    /* 0x35 */ TATL_HINT_ID_IRON_KNUCKLE,
    /* 0x36 */ TATL_HINT_ID_36, // Empty
    /* 0x37 */ TATL_HINT_ID_LIKE_LIKE,
    /* 0x38 */ TATL_HINT_ID_38, // Empty
    /* 0x39 */ TATL_HINT_ID_BEAMOS,
    /* 0x3A */ TATL_HINT_ID_3A, // Empty
    /* 0x3B */ TATL_HINT_ID_FREEZARD,
    /* 0x3C */ TATL_HINT_ID_WHITE_BOE,
    /* 0x3D */ TATL_HINT_ID_3D, // Empty
    /* 0x3E */ TATL_HINT_ID_3E, // Empty
    /* 0x3F */ TATL_HINT_ID_3F, // Empty
    /* 0x40 */ TATL_HINT_ID_40, // Empty
    /* 0x41 */ TATL_HINT_ID_41, // Empty
    /* 0x42 */ TATL_HINT_ID_OCTOROK,
    /* 0x43 */ TATL_HINT_ID_43, // Empty
    /* 0x44 */ TATL_HINT_ID_POE,
    /* 0x45 */ TATL_HINT_ID_GEKKO_SNAPPER,
    /* 0x46 */ TATL_HINT_ID_BLUE_TEKTITE,
    /* 0x47 */ TATL_HINT_ID_LEEVER,
    /* 0x48 */ TATL_HINT_ID_PEAHAT,
    /* 0x49 */ TATL_HINT_ID_PEAHAT_LARVA,
    /* 0x4A */ TATL_HINT_ID_EYEGORE,
    /* 0x4B */ TATL_HINT_ID_WIZROBE,
    /* 0x4C */ TATL_HINT_ID_WOLFOS,
    /* 0x4D */ TATL_HINT_ID_MAD_SCRUB,
    /* 0x4E */ TATL_HINT_ID_4E, // Empty
    /* 0x4F */ TATL_HINT_ID_4F, // Empty
    /* 0x50 */ TATL_HINT_ID_POE_SISTER_MEG,
    /* 0x51 */ TATL_HINT_ID_POE_SISTER_JO,
    /* 0x52 */ TATL_HINT_ID_POE_SISTER_BETH,
    /* 0x53 */ TATL_HINT_ID_POE_SISTER_AMY,
    /* 0x54 */ TATL_HINT_ID_PIRATE,
    /* 0x55 */ TATL_HINT_ID_STALCHILD,
    /* 0x56 */ TATL_HINT_ID_ICE_KEESE,
    /* 0x57 */ TATL_HINT_ID_WHITE_WOLFOS,
    /* 0x58 */ TATL_HINT_ID_GUAY,
    /* 0x59 */ TATL_HINT_ID_BIG_OCTO,
    /* 0x5A */ TATL_HINT_ID_BIG_POE,
    /* 0x5B */ TATL_HINT_ID_SKULLFISH,
    /* 0x5C */ TATL_HINT_ID_DESBREKO,
    /* 0x5D */ TATL_HINT_ID_GREEN_CHUCHU,
    /* 0x5E */ TATL_HINT_ID_ODOLWA_PHASE_ONE, // 799 or fewer frames have passed, says Odolwa is dangerous to get close to
    /* 0x5F */ TATL_HINT_ID_GEKKO_GIANT_SLIME,
    /* 0x60 */ TATL_HINT_ID_BAD_BAT,
    /* 0x61 */ TATL_HINT_ID_REAL_BOMBCHU,
    /* 0x62 */ TATL_HINT_ID_ODOLWA_CLOSE_TO_PHASE_TWO, // 800 frames have passed, warns that Odolwa will attack after dancing
    /* 0x63 */ TATL_HINT_ID_ODOLWA_PHASE_TWO, // 1000 or more frames have passed, explains that the bugs are drawn to fire
    /* 0x64 */ TATL_HINT_ID_MUSHROOM,
    /* 0xFF */ TATL_HINT_ID_NONE = 0xFF
} TatlHintId;

typedef enum NpcTalkState {
    /* 0 */ NPC_TALK_STATE_IDLE, // NPC not currently talking to player
    /* 1 */ NPC_TALK_STATE_TALKING, // NPC is currently talking to player
    /* 2 */ NPC_TALK_STATE_ACTION, // An NPC-defined action triggered in the conversation
    /* 3 */ NPC_TALK_STATE_ITEM_GIVEN // NPC finished giving an item and text box is done
} NpcTalkState;

typedef enum NpcTrackingMode {
    /* 0 */ NPC_TRACKING_PLAYER_AUTO_TURN, // Determine tracking mode based on player position, see Npc_UpdateAutoTurn
    /* 1 */ NPC_TRACKING_NONE, // Don't track the target (usually the player)
    /* 2 */ NPC_TRACKING_HEAD_AND_TORSO, // Track target by turning the head and the torso
    /* 3 */ NPC_TRACKING_HEAD, // Track target by turning the head
    /* 4 */ NPC_TRACKING_FULL_BODY // Track target by turning the body, torso and head
} NpcTrackingMode;

typedef struct NpcInteractInfo {
    /* 0x00 */ s16 talkState;
    /* 0x02 */ s16 trackingMode;
    /* 0x04 */ s16 autoTurnTimer;
    /* 0x06 */ s16 autoTurnState;
    /* 0x08 */ Vec3s headRot;
    /* 0x0E */ Vec3s torsoRot;
    /* 0x14 */ f32 yOffset; // Y position offset to add to actor position when calculating angle to target
    /* 0x18 */ Vec3f trackPos;
    /* 0x24 */ UNK_TYPE1 unk_24[0x4];
} NpcInteractInfo; // size = 0x28

extern AttentionRangeParams gAttentionRanges[ATTENTION_RANGE_MAX];
extern s16 D_801AED48[8];
// extern Gfx D_801AEF88[];
// extern Gfx D_801AEFA0[];

extern Actor* gCameraDriftActor;

void ActorShape_Init(ActorShape* actorShape, f32 yOffset, ActorShadowFunc shadowDraw, f32 shadowScale);
void ActorShadow_DrawCircle(Actor* actor, struct Lights* lights, struct PlayState* play);
void ActorShadow_DrawSquare(Actor* actor, struct Lights* lights, struct PlayState* play);
void ActorShadow_DrawWhiteCircle(Actor* actor, struct Lights* lights, struct PlayState* play);
void ActorShadow_DrawHorse(Actor* actor, struct Lights* lights, struct PlayState* play);

void ActorShadow_DrawFeet(Actor* actor, struct Lights* mapper, struct PlayState* play);
void Actor_SetFeetPos(Actor* actor, s32 limbIndex, s32 leftFootIndex, Vec3f* leftFootPos, s32 rightFootIndex,
                      Vec3f* rightFootPos);
void func_800B4AEC(struct PlayState* play, Actor* actor, f32 y);
void func_800B4B50(Actor* actor, struct Lights* mapper, struct PlayState* play);
void Actor_GetProjectedPos(struct PlayState* play, Vec3f* worldPos, Vec3f* projectedPos, f32* invW);

void Attention_Draw(Attention* attention, struct PlayState* play);

s32 Flags_GetSwitch(struct PlayState* play, s32 flag);
void Flags_SetSwitch(struct PlayState* play, s32 flag);
void Flags_UnsetSwitch(struct PlayState* play, s32 flag);
s32 Flags_GetTreasure(struct PlayState* play, s32 flag);
void Flags_SetTreasure(struct PlayState* play, s32 flag);
void Flags_SetAllTreasure(struct PlayState* play, s32 flag);
s32 Flags_GetAllTreasure(struct PlayState* play);
s32 Flags_GetClear(struct PlayState* play, s32 roomNumber);
void Flags_SetClear(struct PlayState* play, s32 roomNumber);
void Flags_UnsetClear(struct PlayState* play, s32 roomNumber);
s32 Flags_GetClearTemp(struct PlayState* play, s32 roomNumber);
void Flags_SetClearTemp(struct PlayState* play, s32 roomNumber);
void Flags_UnsetClearTemp(struct PlayState* play, s32 roomNumber);
s32 Flags_GetCollectible(struct PlayState* play, s32 flag);
void Flags_SetCollectible(struct PlayState* play, s32 flag);

void TitleCard_InitBossName(struct GameState* gameState, TitleCardContext* titleCtx, TexturePtr texture, s16 x, s16 y,
                            u8 width, u8 height);

s32 Actor_SetPlayerImpact(struct PlayState* play, PlayerImpactType type, s32 timer, f32 dist, Vec3f* pos);
f32 Actor_GetPlayerImpact(struct PlayState* play, f32 range, Vec3f* pos, PlayerImpactType* type);
void* Actor_AddSharedMemoryEntry(struct PlayState* play, s16 id, void* ptr, size_t size);
void* Actor_FreeSharedMemoryEntry(struct PlayState* play, s16 id);
void* Actor_FindSharedMemoryEntry(struct PlayState* play, s16 id);
void Actor_Kill(Actor* actor);
void Actor_SetWorldToHome(Actor* actor);
void Actor_SetFocus(Actor* actor, f32 height);
void Actor_SetWorldRotToShape(Actor* actor);
void Actor_SetShapeRotToWorld(Actor* actor);
void Actor_SetScale(Actor* actor, f32 scale);
void Actor_SetObjectDependency(struct PlayState* play, Actor* actor);
void Actor_SetMovementScale(s32 scale);
void Actor_UpdatePos(Actor* actor);
void Actor_UpdateVelocityWithGravity(Actor* actor);
void Actor_MoveWithGravity(Actor* actor);
void Actor_UpdateVelocityWithoutGravity(Actor* actor);
void Actor_MoveWithoutGravity(Actor* actor);
void Actor_UpdateVelocityWithoutGravityReverse(Actor* actor);
void Actor_MoveWithoutGravityReverse(Actor* actor);
void Actor_SetSpeeds(Actor* actor, f32 speed);
s16 Actor_WorldYawTowardActor(Actor* actorA, Actor* actorB);
s16 Actor_FocusYawTowardActor(Actor* actorA, Actor* actorB);
s16 Actor_WorldYawTowardPoint(Actor* actor, Vec3f* refPoint);
s16 Actor_WorldPitchTowardActor(Actor* actorA, Actor* actorB);
s16 Actor_FocusPitchTowardActor(Actor* actorA, Actor* actorB);
s16 Actor_WorldPitchTowardPoint(Actor* actor, Vec3f* refPoint);
f32 Actor_WorldDistXYZToActor(Actor* actorA, Actor* actorB);
f32 Actor_WorldDistXYZToPoint(Actor* actor, Vec3f* refPoint);
f32 Actor_WorldDistXZToActor(Actor* actorA, Actor* actorB);
f32 Actor_WorldDistXZToPoint(Actor* actor, Vec3f* refPoint);
void Actor_WorldToActorCoords(Actor* actor, Vec3f* offset, Vec3f* point);
f32 Actor_HeightDiff(Actor* actor1, Actor* actor2);
void Actor_SetControlStickData(struct PlayState* play, Input* input, f32 controlStickMagnitude, s16 controlStickAngle);

void func_800B72F8(DynaPolyActor* dyna, f32 extraPushForce, s16 yRotation);

s32 Actor_ActorBIsFacingActorA(Actor* actorA, Actor* actorB, s16 maxAngleDiff);
s32 Actor_IsFacingPlayer(Actor* actor, s16 angle);
s32 Actor_ActorAIsFacingActorB(Actor* actorA, Actor* actorB, s16 maxAngleDiff);
s32 Actor_IsFacingAndNearPlayer(Actor* actor, f32 range, s16 maxAngleDiff);
s32 Actor_ActorAIsFacingAndNearActorB(Actor* actorA, Actor* actorB, f32 range, s16 maxAngleDiff);

void Actor_GetSlopeDirection(struct CollisionPoly* floorPoly, Vec3f* slopeNormal, s16* downwardSlopeYaw);
void Actor_UpdateBgCheckInfo(struct PlayState* play, Actor* actor, f32 wallCheckHeight, f32 wallCheckRadius,
                             f32 ceilingCheckHeight, u32 updBgCheckInfoFlags);
Hilite* Hilite_DrawOpa(Vec3f* object, Vec3f* eye, Vec3f* lightDir, struct GraphicsContext* gfxCtx);
void func_800B8050(Actor* actor, struct PlayState* play, s32 flag);
void func_800B8118(Actor* actor, struct PlayState* play, s32 flag);
PosRot Actor_GetFocus(Actor* actor);
PosRot Actor_GetWorld(Actor* actor);
PosRot Actor_GetWorldPosShapeRot(Actor* actor);

s32 Attention_ShouldReleaseLockOn(Actor* actor, struct Player* player, s32 ignoreLeash);
s32 Actor_TalkOfferAccepted(Actor* actor, struct GameState* gameState);
s32 Actor_OfferTalk(Actor* actor, struct PlayState* play, f32 radius);
s32 Actor_OfferTalkNearColChkInfoCylinder(Actor* actor, struct PlayState* play);
s32 Actor_TextboxIsClosing(Actor* actor, struct PlayState* play);
s32 Actor_ChangeFocus(Actor* actor1, struct PlayState* play, Actor* actor2);

s32 Actor_OcarinaInteractionAccepted(Actor* actor, struct GameState* gameState);
s32 Actor_OfferOcarinaInteraction(Actor* actor, struct PlayState* play, f32 xzRange, f32 yRange);
s32 Actor_OfferOcarinaInteractionNearby(Actor* actor, struct PlayState* play, f32 xzRange);
s32 Actor_NoOcarinaInteraction(Actor* actor, struct PlayState* play);
void Actor_GetScreenPos(struct PlayState* play, Actor* actor, s16* posX, s16* posY);
bool Actor_OnScreen(struct PlayState* play, Actor* actor);
s32 Actor_HasParent(Actor* actor, struct PlayState* play);
s32 Actor_OfferGetItem(Actor* actor, struct PlayState* play, GetItemId getItemId, f32 xzRange, f32 yRange);
s32 Actor_OfferGetItemNearby(Actor* actor, struct PlayState* play, GetItemId getItemId);
s32 Actor_OfferCarry(Actor* actor, struct PlayState* play);
s32 Actor_OfferGetItemFar(Actor* actor, struct PlayState* play, GetItemId getItemId);
s32 Actor_HasNoParent(Actor* actor, struct PlayState* play);
void func_800B8C20(Actor* actorA, Actor* actorB, struct PlayState* play);
void Actor_SetClosestSecretDistance(Actor* actor, struct PlayState* play);
s32 Actor_HasRider(struct PlayState* play, Actor* horse);
s32 Actor_SetRideActor(struct PlayState* play, Actor* horse, s32 mountSide);
s32 Actor_HasNoRider(struct PlayState* play, Actor* horse);

void Actor_PlaySfx(Actor* actor, u16 sfxId);
void Actor_PlaySfx_SurfaceBomb(struct PlayState* play, Actor* actor);
void Actor_PlaySfx_Flagged2(Actor* actor, u16 sfxId);
void Actor_PlaySfx_FlaggedCentered1(Actor* actor, u16 sfxId);
void Actor_PlaySfx_FlaggedCentered2(Actor* actor, u16 sfxId);
void Actor_PlaySfx_Flagged(Actor* actor, u16 sfxId);
void Actor_PlaySfx_FlaggedTimer(Actor* actor, s32 timer);
void Actor_PlaySeq_FlaggedKamaroDance(Actor* actor);
void Actor_PlaySeq_FlaggedMusicBoxHouse(Actor* actor);
s32 func_800B90AC(struct PlayState* play, Actor* actor, struct CollisionPoly* polygon, s32 bgId, Vec3f* arg4);
void Actor_DeactivateLens(struct PlayState* play);
void Actor_InitHalfDaysBit(ActorContext* actorCtx);
void Actor_InitContext(struct PlayState* play, ActorContext* actorCtx, struct ActorEntry* actorEntry);
void Actor_UpdateAll(struct PlayState* play, ActorContext* actorCtx);
s32 Actor_AddToLensActors(struct PlayState* play, Actor* actor);
void Actor_DrawAll(struct PlayState* play, ActorContext* actorCtx);
void Actor_KillAllWithMissingObject(struct PlayState* play, ActorContext* actorCtx);
void func_800BA798(struct PlayState* play, ActorContext* actorCtx);
void Actor_CleanupContext(ActorContext* actorCtx, struct PlayState* play);
Actor* Actor_Spawn(ActorContext* actorCtx, struct PlayState* play, s16 actorId, f32 posX, f32 posY, f32 posZ, s16 rotX,
                   s16 rotY, s16 rotZ, s32 params);
Actor* Actor_SpawnAsChildAndCutscene(ActorContext* actorCtx, struct PlayState* play, s16 index, f32 x, f32 y, f32 z,
                                     s16 rotX, s16 rotY, s16 rotZ, s32 params, u32 csId, u32 halfDaysBits,
                                     Actor* parent);
Actor* Actor_SpawnAsChild(ActorContext* actorCtx, Actor* parent, struct PlayState* play, s16 actorId, f32 posX,
                          f32 posY, f32 posZ, s16 rotX, s16 rotY, s16 rotZ, s32 params);
void Actor_SpawnTransitionActors(struct PlayState* play, ActorContext* actorCtx);
void Enemy_StartFinishingBlow(struct PlayState* play, Actor* actor);
void Actor_SpawnBodyParts(Actor* actor, struct PlayState* play, s32 partParams, Gfx** dList);
void Actor_SpawnFloorDustRing(struct PlayState* play, Actor* actor, Vec3f* posXZ, f32 radius, s32 countMinusOne,
                              f32 randAccelWeight, s16 scale, s16 scaleStep, u8 useLighting);
void func_800BBFB0(struct PlayState* play, Vec3f* position, f32 arg2, s32 arg3, s16 arg4, s16 scaleStep, u8 arg6);
void Actor_ChangeCategory(struct PlayState* play, ActorContext* actorCtx, Actor* actor, u8 actorCategory);
u32 Actor_GetArrowDmgFlags(s32 params);
Actor* func_800BC270(struct PlayState* play, Actor* actor, f32 distance, u32 dmgFlags);
Actor* func_800BC444(struct PlayState* play, Actor* actor, f32 distance);
s16 Actor_TestFloorInDirection(Actor* actor, struct PlayState* play, f32 distance, s16 angle);
s32 Actor_IsTargeted(struct PlayState* play, Actor* actor);
s32 Actor_OtherIsTargeted(struct PlayState* play, Actor* actor);
void func_800BC620(Vec3f* pos, Vec3f* scale, u8 alpha, struct PlayState* play);
void Actor_RequestQuake(struct PlayState* play, s16 y, s16 duration);
void Actor_RequestQuakeWithSpeed(struct PlayState* play, s16 y, s16 duration, s16 speed);
void Actor_RequestQuakeAndRumble(Actor* actor, struct PlayState* play, s16 quakeY, s16 quakeDuration);
void Actor_DrawDoorLock(struct PlayState* play, s32 frame, s32 type);
void Actor_SetColorFilter(Actor* actor, u16 colorFlag, u16 colorIntensityMax, u16 bufFlag, u16 duration);
Hilite* func_800BCBF4(Vec3f* arg0, struct PlayState* play);
Hilite* func_800BCC68(Vec3f* arg0, struct PlayState* play);
void Actor_GetClosestPosOnPath(Vec3s* points, s32 numPoints, Vec3f* srcPos, Vec3f* dstPos, s32 isPathLoop);
s32 Npc_UpdateTalking(struct PlayState* play, Actor* actor, s16* talkState, f32 interactRange,
                      NpcGetTextIdFunc getTextId, NpcUpdateTalkStateFunc updateTalkState);
void Npc_TrackPoint(Actor* actor, NpcInteractInfo* interactInfo, s16 presetIndex, s16 trackingMode);
void func_800BD9E0(struct PlayState* play, SkelAnime* skelAnime, OverrideLimbDraw overrideLimbDraw,
                   PostLimbDraw postLimbDraw, Actor* actor, s16 alpha);
void func_800BDAA0(struct PlayState* play, SkelAnime* skelAnime, OverrideLimbDraw overrideLimbDraw,
                   PostLimbDraw postLimbDraw, Actor* actor, s16 alpha);
void Actor_ChangeAnimationByInfo(SkelAnime* skelAnime, AnimationInfo* animInfo, s32 animIndex);
void Actor_Noop(Actor* actor, struct PlayState* play);

Actor* Actor_FindNearby(struct PlayState* play, Actor* inActor, s16 actorId, u8 actorCategory, f32 distance);
s32 func_800BE184(struct PlayState* play, Actor* actor, f32 xzDist, s16 arg3, s16 arg4, s16 arg5);
u8 Actor_ApplyDamage(Actor* actor);
void Actor_SetDropFlag(Actor* actor, ColliderElement* elem);
void Actor_SetDropFlagJntSph(Actor* actor, ColliderJntSph* jntSphere);
void func_800BE33C(Vec3f* arg0, Vec3f* arg1, Vec3s* dst, s32 arg3);
void func_800BE3D0(Actor* actor, s16 angle, Vec3s* arg2);
void func_800BE504(Actor* actor, ColliderCylinder* collider);
void func_800BE568(Actor* actor, ColliderSphere* collider);
void func_800BE5CC(Actor* actor, ColliderJntSph* collider, s32 colliderIndex);
s32 Actor_IsSmallChest(struct EnBox* chest);
void Actor_DrawDamageEffects(struct PlayState* play, Actor* actor, Vec3f bodyPartsPos[], s16 bodyPartsCount,
                             f32 effectScale, f32 frozenSteamScale, f32 effectAlpha, u8 type);
void Actor_SpawnIceEffects(struct PlayState* play, Actor* actor, Vec3f bodyPartsPos[], s32 bodyPartsCount,
                           s32 effectsPerBodyPart, f32 scale, f32 scaleRange);

void DynaPolyActor_UpdateCarriedActorPos(struct CollisionContext* colCtx, s32 bgId, Actor* carriedActor);
void DynaPolyActor_UpdateCarriedActorRotY(struct CollisionContext* colCtx, s32 bgId, Actor* carriedActor);
void DynaPolyActor_AttachCarriedActor(struct CollisionContext* colCtx, Actor* carriedActor, s32 bgId);
u32 DynaPolyActor_TransformCarriedActor(struct CollisionContext* colCtx, s32 bgId, Actor* carriedActor);
void DynaPolyActor_Init(DynaPolyActor* dynaActor, s32 transformFlags);
void DynaPolyActor_LoadMesh(struct PlayState* play, DynaPolyActor* dynaActor, struct CollisionHeader* meshHeader);
void DynaPolyActor_UnsetAllInteractFlags(DynaPolyActor* dynaActor);
void DynaPolyActor_SetActorOnTop(DynaPolyActor* dynaActor);
void DynaPolyActor_SetPlayerOnTop(DynaPolyActor* dynaActor);
void DynaPoly_SetPlayerOnTop(struct CollisionContext* colCtx, s32 bgId);
void DynaPolyActor_SetPlayerAbove(DynaPolyActor* dynaActor);
void DynaPoly_SetPlayerAbove(struct CollisionContext* colCtx, s32 bgId);
void DynaPolyActor_SetSwitchPressed(DynaPolyActor* dynaActor);
void DynaPolyActor_SetHeavySwitchPressed(DynaPolyActor* dynaActor);
s32 DynaPolyActor_IsActorOnTop(DynaPolyActor* dynaActor);
s32 DynaPolyActor_IsPlayerOnTop(DynaPolyActor* dynaActor);
s32 DynaPolyActor_IsPlayerAbove(DynaPolyActor* dynaActor);
s32 DynaPolyActor_IsSwitchPressed(DynaPolyActor* dynaActor);
s32 DynaPolyActor_IsHeavySwitchPressed(DynaPolyActor* dynaActor);
s32 DynaPolyActor_ValidateMove(struct PlayState* play, DynaPolyActor* dynaActor, s16 startRadius, s16 endRadius,
                               s16 startHeight);

void Actor_ContinueText(struct PlayState* play, Actor* actor, u16 textId);
s32 Flags_GetEventChkInf(s32 flag);
void Flags_SetEventChkInf(s32 flag);
s32 Flags_GetInfTable(s32 flag);
void Flags_SetInfTable(s32 flag);
s32 Actor_TrackNone(Vec3s* headRot, Vec3s* torsoRot);
s32 Actor_TrackPoint(Actor* actor, Vec3f* target, Vec3s* headRot, Vec3s* torsoRot);
s32 Actor_TrackPlayerSetFocusHeight(struct PlayState* play, Actor* actor, Vec3s* headRot, Vec3s* torsoRot,
                                    f32 focusHeight);
s32 Actor_TrackPlayer(struct PlayState* play, Actor* actor, Vec3s* headRot, Vec3s* torsoRot, Vec3f focusPos);

void ActorOverlayTable_Init(void);
void ActorOverlayTable_Cleanup(void);

#endif
