#ifndef Z64COLLISION_CHECK_H
#define Z64COLLISION_CHECK_H

#include "PR/ultratypes.h"
#include "z64math.h"
#include "unk.h"

struct Actor;
struct PlayState;

typedef struct {
    /* 0x00 */ struct Actor* actor; // Attached actor
    /* 0x04 */ struct Actor* at; // Actor attached to what it collided with as an AT collider.
    /* 0x08 */ struct Actor* ac; // Actor attached to what it collided with as an AC collider.
    /* 0x0C */ struct Actor* oc; // Actor attached to what it collided with as an OC collider.
    /* 0x10 */ u8 atFlags; // Information flags for AT collisions. 
    /* 0x11 */ u8 acFlags; // Information flags for AC collisions.
    /* 0x12 */ u8 ocFlags1; // Information flags for OC collisions.
    /* 0x13 */ u8 ocFlags2; // Flags related to which colliders it can OC collide with.
    /* 0x14 */ u8 colMaterial; // Determines hitmarks and sound effects during AC collisions. See `ColliderMaterial` enum
    /* 0x15 */ u8 shape; // See `ColliderShape` enum
} Collider; // size = 0x18

typedef struct {
    /* 0x0 */ u8 colMaterial; // Determines hitmarks and sound effects during AC collisions. See `ColliderMaterial` enum
    /* 0x1 */ u8 atFlags; // Information flags for AT collisions. 
    /* 0x2 */ u8 acFlags; // Information flags for AC collisions.
    /* 0x3 */ u8 ocFlags1; // Information flags for OC collisions.
    /* 0x4 */ u8 ocFlags2; // Flags related to which colliders it can OC collide with.
    /* 0x5 */ u8 shape; // See `ColliderShape` enum
} ColliderInit; // size = 0x6

typedef struct {
    /* 0x0 */ u8 colMaterial; // Determines hitmarks and sound effects during AC collisions. See `ColliderMaterial` enum
    /* 0x1 */ u8 atFlags; // Information flags for AT collisions. 
    /* 0x2 */ u8 acFlags; // Information flags for AC collisions.
    /* 0x3 */ u8 ocFlags1; // Information flags for OC collisions.
    /* 0x4 */ u8 shape; // See `ColliderShape` enum
} ColliderInitType1; // size = 0x5

typedef struct {
    /* 0x0 */ struct Actor* actor; // Attached actor
    /* 0x4 */ u8 atFlags; // Information flags for AT collisions.
    /* 0x5 */ u8 acFlags; // Information flags for AC collisions.
    /* 0x6 */ u8 ocFlags1; // Information flags for OC collisions.
    /* 0x7 */ u8 shape; // See `ColliderShape` enum
} ColliderInitToActor; // size = 0x8

typedef struct {
    /* 0x0 */ u32 dmgFlags; // Damage types dealt by this collider element as AT.
    /* 0x4 */ u8 effect; // Damage Effect (Knockback, Fire, etc.)
    /* 0x5 */ u8 damage; // Damage or Stun Timer
} ColliderElementDamageInfoAT; // size = 0x8

typedef struct {
    /* 0x0 */ u32 dmgFlags; // Damage types dealt by this collider element as AT.
    /* 0x4 */ u8 effect; // Damage Effect (Knockback, Fire, etc.)
    /* 0x5 */ u8 damage; // Damage or Stun Timer
} ColliderElementDamageInfoATInit; // size = 0x8

typedef struct {
    /* 0x0 */ u32 dmgFlags; // Bumper damage type flags.
    /* 0x4 */ u8 effect; // Damage Effect (Knockback, Fire, etc.)
    /* 0x5 */ u8 defense; // Damage Resistance
    /* 0x6 */ Vec3s hitPos; // Point of contact
} ColliderElementDamageInfoAC; // size = 0xC

typedef struct {
    /* 0x0 */ u32 dmgFlags; // Bumper exclusion mask
    /* 0x4 */ u8 effect; // Damage Effect (Knockback, Fire, etc.)
    /* 0x5 */ u8 defense; // Damage Resistance
} ColliderElementDamageInfoACInit; // size = 0x8

typedef struct ColliderElement {
    /* 0x00 */ ColliderElementDamageInfoAT atDmgInfo; // Damage properties when acting as an AT collider
    /* 0x08 */ ColliderElementDamageInfoAC acDmgInfo; // Damage properties when acting as an AC collider
    /* 0x14 */ u8 elemMaterial; // Affects sfx when attacked by Player, and interaction with hookshot and arrows. See `ElementMaterial` enum
    /* 0x15 */ u8 atElemFlags; // Information flags for AT collisions
    /* 0x16 */ u8 acElemFlags; // Information flags for AC collisions
    /* 0x17 */ u8 ocElemFlags; // Information flags for OC collisions
    /* 0x18 */ Collider* atHit; // object touching this element's AT collider
    /* 0x1C */ Collider* acHit; // object touching this element's AC collider
    /* 0x20 */ struct ColliderElement* atHitElem; // element that hit the AT collider
    /* 0x24 */ struct ColliderElement* acHitElem; // element that hit the AC collider
} ColliderElement; // size = 0x28

typedef struct {
    /* 0x00 */ u8 elemMaterial; // Affects sfx when attacked by Player, and interaction with hookshot and arrows. See `ElementMaterial` enum
    /* 0x04 */ ColliderElementDamageInfoATInit atDmgInfo; // Damage properties when acting as an AT collider
    /* 0x0C */ ColliderElementDamageInfoACInit acDmgInfo; // Damage properties when acting as an AC collider
    /* 0x14 */ u8 atElemFlags; // Information flags for AT collisions
    /* 0x15 */ u8 acElemFlags; // Information flags for AC collisions
    /* 0x16 */ u8 ocElemFlags; // Information flags for OC collisions
} ColliderElementInit; // size = 0x18

/*
 * JntSph - A collider made of sphere shaped elements. Each sphere can attach to a skeleton joint (limb).
 */

// collider structs

typedef struct {
    /* 0x00 */ Sphere16 modelSphere; // model space sphere
    /* 0x08 */ Sphere16 worldSphere; // world space sphere
    /* 0x10 */ f32 scale; // worldsphere = modelsphere * scale * 0.01
    /* 0x14 */ u8 limb; // attached limb
} ColliderJntSphElementDim; // size = 0x18

typedef struct {
    /* 0x00 */ ColliderElement base;
    /* 0x28 */ ColliderJntSphElementDim dim;
} ColliderJntSphElement; // size = 0x40

typedef struct {
    /* 0x00 */ Collider base;
    /* 0x18 */ s32 count;
    /* 0x1C */ ColliderJntSphElement* elements;
} ColliderJntSph; // size = 0x20

// init data structs

typedef struct {
    /* 0x0 */ u8 limb; // attached limb
    /* 0x2 */ Sphere16 modelSphere; // model space sphere
    /* 0xA */ s16 scale; // world space sphere = model * scale * 0.01
} ColliderJntSphElementDimInit; // size = 0xC

typedef struct {
    /* 0x00 */ ColliderElementInit base;
    /* 0x18 */ ColliderJntSphElementDimInit dim;
} ColliderJntSphElementInit; // size = 0x24

typedef struct {
    /* 0x0 */ ColliderInit base;
    /* 0x8 */ s32 count;
    /* 0xC */ ColliderJntSphElementInit* elements;
} ColliderJntSphInit; // size = 0x10

typedef struct {
    /* 0x0 */ ColliderInitType1 base;
    /* 0x8 */ s32 count;
    /* 0xC */ ColliderJntSphElementInit* elements;
} ColliderJntSphInitType1; // size = 0x10

typedef struct {
    /* 0x0 */ ColliderInitToActor base;
    /* 0x8 */ s32 count;
    /* 0xC */ ColliderJntSphElementInit* elements;
} ColliderJntSphInitToActor; // size = 0x10

/*
 * Cylinder - A single cylinder shaped collider
 */

// collider structs

typedef struct {
    /* 0x00 */ Collider base;
    /* 0x18 */ ColliderElement elem;
    /* 0x40 */ Cylinder16 dim;
} ColliderCylinder; // size = 0x4C

// init data structs

typedef struct {
    /* 0x00 */ ColliderInit base;
    /* 0x08 */ ColliderElementInit elem;
    /* 0x20 */ Cylinder16 dim;
} ColliderCylinderInit; // size = 0x2C

typedef struct {
    /* 0x00 */ ColliderInitType1 base;
    /* 0x08 */ ColliderElementInit elem;
    /* 0x20 */ Cylinder16 dim;
} ColliderCylinderInitType1; // size = 0x2C

typedef struct {
    /* 0x00 */ ColliderInitToActor base;
    /* 0x08 */ ColliderElementInit elem;
    /* 0x20 */ Cylinder16 dim;
} ColliderCylinderInitToActor; // size = 0x2C

/*
 * Tris - A collider made of triangle shaped elements
 */

// collider structs

typedef struct {
    /* 0x00 */ ColliderElement base;
    /* 0x28 */ TriNorm dim;
} ColliderTrisElement; // size = 0x5C

typedef struct {
    /* 0x00 */ Collider base;
    /* 0x18 */ s32 count;
    /* 0x1C */ ColliderTrisElement* elements;
} ColliderTris; // size = 0x20

// init data structs

typedef struct {
    /* 0x00 */ Vec3f vtx[3];
} ColliderTrisElementDimInit; // size = 0x24

typedef struct {
    /* 0x00 */ ColliderElementInit base;
    /* 0x18 */ ColliderTrisElementDimInit dim;
} ColliderTrisElementInit; // size = 0x3C

typedef struct {
    /* 0x0 */ ColliderInit base;
    /* 0x8 */ s32 count;
    /* 0xC */ ColliderTrisElementInit* elements;
} ColliderTrisInit; // size = 0x10

typedef struct {
    /* 0x0 */ ColliderInitType1 base;
    /* 0x8 */ s32 count;
    /* 0xC */ ColliderTrisElementInit* elements;
} ColliderTrisInitType1; // size = 0x10

/*
 * Quad - A single quad shaped collider
 */

// collider structs

typedef struct {
    /* 0x00 */ Vec3f quad[4];
    /* 0x30 */ Vec3s dcMid; // midpoint of vectors d, c
    /* 0x36 */ Vec3s baMid; // midpoint of vectors b, a
    /* 0x3C */ f32 acDist; // distance to nearest AC collision this frame.
} ColliderQuadDim; // size = 0x40

typedef struct {
    /* 0x00 */ Collider base;
    /* 0x18 */ ColliderElement elem;
    /* 0x40 */ ColliderQuadDim dim;
} ColliderQuad; // size = 0x80

// init data structs

typedef struct {
    /* 0x00 */ Vec3f quad[4];
} ColliderQuadDimInit; // size = 0x30

typedef struct {
    /* 0x00 */ ColliderInit base;
    /* 0x08 */ ColliderElementInit elem;
    /* 0x20 */ ColliderQuadDimInit dim;
} ColliderQuadInit; // size = 0x50

typedef struct {
    /* 0x00 */ ColliderInitType1 base;
    /* 0x08 */ ColliderElementInit elem;
    /* 0x20 */ ColliderQuadDimInit dim;
} ColliderQuadInitType1; // size = 0x50

/*
 * Sphere - A single sphere shaped collider
 */

// collider structs

typedef struct {
    /* 0x00 */ Collider base;
    /* 0x18 */ ColliderElement elem;
    /* 0x40 */ ColliderJntSphElementDim dim;
} ColliderSphere; // size = 0x58

// init data structs

typedef struct {
    /* 0x00 */ ColliderInit base;
    /* 0x08 */ ColliderElementInit elem;
    /* 0x20 */ ColliderJntSphElementDimInit dim;
} ColliderSphereInit; // size = 0x2C

/*
 * Line collider
 */

typedef struct {
    /* 0x00 */ Linef line;
    /* 0x18 */ u16 ocFlags;
} OcLine; // size = 0x1C

typedef struct {
    /* 0x000 */ s16 colATCount;
    /* 0x002 */ u16 sacFlags; // Controls whether new collidors can be added or removed, or only swapped
    /* 0x004 */ Collider* colAT[50];
    /* 0x0CC */ s32 colACCount;
    /* 0x0D0 */ Collider* colAC[60];
    /* 0x1C0 */ s32 colOCCount;
    /* 0x1C4 */ Collider* colOC[50];
    /* 0x28C */ s32 colLineCount;
    /* 0x290 */ OcLine* colLine[3];
} CollisionCheckContext; // size = 0x29C

typedef struct {
    /* 0x0 */ u8 blood;
    /* 0x1 */ u8 effect;
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

typedef enum ColliderMaterial {
    /*  0 */ COL_MATERIAL_HIT0, // Blue blood, white hitmark
    /*  1 */ COL_MATERIAL_HIT1, // No blood, dust hitmark
    /*  2 */ COL_MATERIAL_HIT2, // Green blood, dust hitmark
    /*  3 */ COL_MATERIAL_HIT3, // No blood, white hitmark
    /*  4 */ COL_MATERIAL_HIT4, // Water burst, no hitmark
    /*  5 */ COL_MATERIAL_HIT5, // No blood, red hitmark
    /*  6 */ COL_MATERIAL_HIT6, // Green blood, white hitmark
    /*  7 */ COL_MATERIAL_HIT7, // Red blood, white hitmark
    /*  8 */ COL_MATERIAL_HIT8, // Blue blood, red hitmark
    /*  9 */ COL_MATERIAL_METAL,
    /* 10 */ COL_MATERIAL_NONE,
    /* 11 */ COL_MATERIAL_WOOD,
    /* 12 */ COL_MATERIAL_HARD,
    /* 13 */ COL_MATERIAL_TREE
} ColliderMaterial;

typedef enum {
    /* 0 */ COLSHAPE_JNTSPH,
    /* 1 */ COLSHAPE_CYLINDER,
    /* 2 */ COLSHAPE_TRIS,
    /* 3 */ COLSHAPE_QUAD,
    /* 4 */ COLSHAPE_SPHERE,
    /* 5 */ COLSHAPE_MAX
} ColliderShape;

typedef enum ElementMaterial {
    /* 0 */ ELEM_MATERIAL_UNK0,
    /* 1 */ ELEM_MATERIAL_UNK1,
    /* 2 */ ELEM_MATERIAL_UNK2,
    /* 3 */ ELEM_MATERIAL_UNK3,
    /* 4 */ ELEM_MATERIAL_UNK4,
    /* 5 */ ELEM_MATERIAL_UNK5,
    /* 6 */ ELEM_MATERIAL_UNK6,
    /* 7 */ ELEM_MATERIAL_UNK7
} ElementMaterial;

#define SAC_ON (1 << 0) // CollisionContext SAC Flag

#define AT_NONE 0 // No flags set. Cannot have AT collisions when set as AT
#define AT_ON (1 << 0) // Can have AT collisions when set as AT
#define AT_HIT (1 << 1) // Had an AT collision
#define AT_BOUNCED (1 << 2) // Had an AT collision with an AC_HARD collider
#define AT_TYPE_PLAYER (1 << 3) // Has player-aligned damage
#define AT_TYPE_ENEMY (1 << 4) // Has enemy-aligned damage
#define AT_TYPE_OTHER (1 << 5) // Has non-aligned damage
#define AT_SELF (1 << 6) // Can have AT collisions with colliders attached to the same actor
#define AT_TYPE_ALL (AT_TYPE_PLAYER | AT_TYPE_ENEMY | AT_TYPE_OTHER) // Has all three damage alignments

#define AC_NONE 0 // No flags set. Cannot have AC collisions when set as AC
#define AC_ON (1 << 0) // Can have AC collisions when set as AC
#define AC_HIT (1 << 1) // Had an AC collision
#define AC_HARD (1 << 2) // Causes AT colliders to bounce off it
#define AC_TYPE_PLAYER AT_TYPE_PLAYER // Takes player-aligned damage
#define AC_TYPE_ENEMY AT_TYPE_ENEMY // Takes enemy-aligned damage
#define AC_TYPE_OTHER AT_TYPE_OTHER // Takes non-aligned damage
#define AC_NO_DAMAGE (1 << 6) // Collider does not take damage
#define AC_BOUNCED (1 << 7) // Caused an AT collider to bounce off it
#define AC_TYPE_ALL (AC_TYPE_PLAYER | AC_TYPE_ENEMY | AC_TYPE_OTHER) // Takes damage from all three alignments

#define OC1_NONE 0 // No flags set. Cannot have OC collisions when set as OC
#define OC1_ON (1 << 0) // Can have OC collisions when set as OC
#define OC1_HIT (1 << 1) // Had an OC collision
#define OC1_NO_PUSH (1 << 2) // Does not push other colliders away during OC collisions
#define OC1_TYPE_PLAYER (1 << 3) // Can have OC collisions with OC type player
#define OC1_TYPE_1 (1 << 4) // Can have OC collisions with OC type 1
#define OC1_TYPE_2 (1 << 5) // Can have OC collisions with OC type 2
#define OC1_TYPE_ALL (OC1_TYPE_PLAYER | OC1_TYPE_1 | OC1_TYPE_2) // Can have collisions with all three OC types

#define OC2_NONE 0 // No flags set. Has no OC type
#define OC2_HIT_PLAYER (1 << 0) // Had an OC collision with OC type player
#define OC2_UNK1 (1 << 1) // Prevents OC collisions with OC2_UNK2. Some horses and toki_sword have it.
#define OC2_UNK2 (1 << 2) // Prevents OC collisions with OC2_UNK1. Nothing has it.
#define OC2_TYPE_PLAYER OC1_TYPE_PLAYER // Has OC type player
#define OC2_TYPE_1 OC1_TYPE_1 // Has OC type 1
#define OC2_TYPE_2 OC1_TYPE_2 // Has OC type 2
#define OC2_FIRST_ONLY (1 << 6) // Skips AC checks on elements after the first collision. Only used by Ganon

#define ATELEM_NONE 0 // No flags set. Cannot have AT collisions
#define ATELEM_ON (1 << 0) // Can have AT collisions
#define ATELEM_HIT (1 << 1) // Had an AT collision
#define ATELEM_NEAREST (1 << 2) // If a Quad, only collides with the closest AC element
#define ATELEM_SFX_NORMAL (0 << 3) // Hit sound effect based on AC collider's type
#define ATELEM_SFX_HARD (1 << 3) // Always uses hard deflection sound
#define ATELEM_SFX_WOOD (2 << 3) // Always uses wood deflection sound
#define ATELEM_SFX_NONE (3 << 3) // No hit sound effect
#define ATELEM_AT_HITMARK (1 << 5) // Draw hitmarks for every AT collision
#define ATELEM_DREW_HITMARK (1 << 6) // Already drew hitmark for this frame
#define ATELEM_UNK7 (1 << 7) // Unknown purpose. Used by some enemy quads

#define ACELEM_NONE 0 // No flags set. Cannot have AC collisions
#define ACELEM_ON (1 << 0) // Can have AC collisions
#define ACELEM_HIT (1 << 1) // Had an AC collision
#define ACELEM_HOOKABLE (1 << 2) // Can be hooked if actor has hookability flags set.
#define ACELEM_NO_AT_INFO (1 << 3) // Does not give its info to the AT collider that hit it.
#define ACELEM_NO_DAMAGE (1 << 4) // Does not take damage.
#define ACELEM_NO_SWORD_SFX (1 << 5) // Does not have a sound when hit by player-attached AT colliders.
#define ACELEM_NO_HITMARK (1 << 6) // Skips hit effects.
#define ACELEM_DRAW_HITMARK (1 << 7) // Draw hitmark for AC collision this frame.

#define OCELEM_NONE 0 // No flags set. Cannot have OC collisions
#define OCELEM_ON (1 << 0) // Can have OC collisions
#define OCELEM_HIT (1 << 1) // Had an OC collision
#define OCELEM_UNK2 (1 << 2) // Unknown purpose.
#define OCELEM_UNK3 (1 << 3) // Unknown purpose. Used by Dead Hand element 0 and Dodongo element 5

#define OCLINE_NONE 0 // Did not have an OcLine collision
#define OCLINE_HIT (1 << 0) // Had an OcLine collision

#define DMG_ENTRY(damage, effect) ((damage) | ((effect) << 4))

// Don't use combinations of these flags in code until we figure out how we want to format them.
// It's okay to use these flags if the code is only checking a single flag, though.
#define DMG_DEKU_NUT       (1 << 0x00)
#define DMG_DEKU_STICK     (1 << 0x01)
#define DMG_HORSE_TRAMPLE  (1 << 0x02)
#define DMG_EXPLOSIVES     (1 << 0x03)
#define DMG_ZORA_BOOMERANG (1 << 0x04)
#define DMG_NORMAL_ARROW   (1 << 0x05)
#define DMG_UNK_0x06       (1 << 0x06)
#define DMG_HOOKSHOT       (1 << 0x07)
#define DMG_GORON_PUNCH    (1 << 0x08)
#define DMG_SWORD          (1 << 0x09)
#define DMG_GORON_POUND    (1 << 0x0A)
#define DMG_FIRE_ARROW     (1 << 0x0B)
#define DMG_ICE_ARROW      (1 << 0x0C)
#define DMG_LIGHT_ARROW    (1 << 0x0D)
#define DMG_GORON_SPIKES   (1 << 0x0E)
#define DMG_DEKU_SPIN      (1 << 0x0F)
#define DMG_DEKU_BUBBLE    (1 << 0x10)
#define DMG_DEKU_LAUNCH    (1 << 0x11)
#define DMG_UNK_0x12       (1 << 0x12)
#define DMG_ZORA_BARRIER   (1 << 0x13)
#define DMG_NORMAL_SHIELD  (1 << 0x14)
#define DMG_LIGHT_RAY      (1 << 0x15)
#define DMG_THROWN_OBJECT  (1 << 0x16)
#define DMG_ZORA_PUNCH     (1 << 0x17)
#define DMG_SPIN_ATTACK    (1 << 0x18)
#define DMG_SWORD_BEAM     (1 << 0x19)
#define DMG_NORMAL_ROLL    (1 << 0x1A)
#define DMG_UNK_0x1B       (1 << 0x1B)
#define DMG_UNK_0x1C       (1 << 0x1C)
#define DMG_UNBLOCKABLE    (1 << 0x1D)
#define DMG_UNK_0x1E       (1 << 0x1E)
#define DMG_POWDER_KEG     (1 << 0x1F)

typedef struct DamageTable {
    /* 0x00 */ u8 attack[32];
} DamageTable; // size = 0x20

typedef struct CollisionCheckInfoInit {
    /* 0x0 */ u8 health;
    /* 0x2 */ s16 cylRadius;
    /* 0x4 */ s16 cylHeight;
    /* 0x6 */ u8 mass;
} CollisionCheckInfoInit; // size = 0x8

typedef struct CollisionCheckInfoInit2 {
    /* 0x0 */ u8 health;
    /* 0x2 */ s16 cylRadius;
    /* 0x4 */ s16 cylHeight;
    /* 0x6 */ s16 cylYShift;
    /* 0x8 */ u8 mass;
} CollisionCheckInfoInit2; // size = 0xC

typedef struct CollisionCheckInfo {
    /* 0x00 */ DamageTable* damageTable;
    /* 0x04 */ Vec3f displacement;
    /* 0x10 */ s16 cylRadius;
    /* 0x12 */ s16 cylHeight;
    /* 0x14 */ s16 cylYShift;
    /* 0x16 */ u8 mass;
    /* 0x17 */ u8 health;
    /* 0x18 */ u8 damage;
    /* 0x19 */ u8 damageEffect;
    /* 0x1A */ u8 atHitEffect;
    /* 0x1B */ u8 acHitEffect;
} CollisionCheckInfo; // size = 0x1C


DamageTable* DamageTable_Get(s32 index);
void DamageTable_Clear(DamageTable* damageTable);
f32 CollisionCheck_GetDamageAndEffectOnElementAC(Collider* atCol, ColliderElement* atElem, Collider* acCol, ColliderElement* acElem, u32* effect);
f32 CollisionCheck_ApplyElementATDefense(f32 damage, ColliderElement* acElem);
s32 CollisionCheck_GetElementATDamage(Collider* atCol, ColliderElement* atElem, Collider* acCol, ColliderElement* acElem);
s32 Collider_InitBase(struct PlayState* play, Collider* col);
s32 Collider_DestroyBase(struct PlayState* play, Collider* collider);
s32 Collider_SetBaseToActor(struct PlayState* play, Collider* col, ColliderInitToActor* src);
s32 Collider_SetBaseType1(struct PlayState* play, Collider* col, struct Actor* actor, ColliderInitType1* src);
s32 Collider_SetBase(struct PlayState* play, Collider* col, struct Actor* actor, ColliderInit* src);
void Collider_ResetATBase(struct PlayState* play, Collider* col);
void Collider_ResetACBase(struct PlayState* play, Collider* col);
void Collider_ResetOCBase(struct PlayState* play, Collider* col);
s32 Collider_InitElementDamageInfoAT(struct PlayState* play, ColliderElementDamageInfoAT* atDmgInfo);
s32 Collider_DestroyElementDamageInfoAT(struct PlayState* play, ColliderElementDamageInfoAT* atDmgInfo);
s32 Collider_SetElementDamageInfoAT(struct PlayState* play, ColliderElementDamageInfoAT* dest, ColliderElementDamageInfoATInit* src);
void Collider_ResetATElementUnk(struct PlayState* play, ColliderElement* elem);
s32 Collider_InitElementDamageInfoAC(struct PlayState* play, ColliderElementDamageInfoAC* acDmgInfo);
s32 Collider_DestroyElementDamageInfoAC(struct PlayState* play, ColliderElementDamageInfoAC* acDmgInfo);
s32 Collider_SetElementDamageInfoAC(struct PlayState* play, ColliderElementDamageInfoAC* dest, ColliderElementDamageInfoACInit* src);
s32 Collider_InitElement(struct PlayState* play, ColliderElement* elem);
s32 Collider_DestroyElement(struct PlayState* play, ColliderElement* elem);
s32 Collider_SetElement(struct PlayState* play, ColliderElement* elem, ColliderElementInit* elemInit);
void Collider_ResetATElement(struct PlayState* play, ColliderElement* elem);
void Collider_ResetACElement(struct PlayState* play, ColliderElement* elem);
void Collider_ResetOCElement(struct PlayState* play, ColliderElement* elem);
s32 Collider_InitJntSphElementDim(struct PlayState* play, ColliderJntSphElementDim* dim);
s32 Collider_DestroyJntSphElementDim(struct PlayState* play, ColliderJntSphElementDim* dim);
s32 Collider_SetJntSphElementDim(struct PlayState* play, ColliderJntSphElementDim* dest, ColliderJntSphElementDimInit* src);
s32 Collider_InitJntSphElement(struct PlayState* play, ColliderJntSphElement* jntSphElem);
s32 Collider_DestroyJntSphElement(struct PlayState* play, ColliderJntSphElement* jntSphElem);
s32 Collider_SetJntSphElement(struct PlayState* play, ColliderJntSphElement* dest, ColliderJntSphElementInit* src);
s32 Collider_ResetJntSphElementAT(struct PlayState* play, ColliderJntSphElement* jntSphElem);
s32 Collider_ResetJntSphElementAC(struct PlayState* play, ColliderJntSphElement* jntSphElem);
s32 Collider_ResetJntSphElementOC(struct PlayState* play, ColliderJntSphElement* jntSphElem);
s32 Collider_InitJntSph(struct PlayState* play, ColliderJntSph* jntSph);
s32 Collider_FreeJntSph(struct PlayState* play, ColliderJntSph* jntSph);
s32 Collider_DestroyJntSph(struct PlayState* play, ColliderJntSph* jntSph);
s32 Collider_SetJntSphToActor(struct PlayState* play, ColliderJntSph* dest, ColliderJntSphInitToActor* src);
s32 Collider_SetJntSphAllocType1(struct PlayState* play, ColliderJntSph* dest, struct Actor* actor, ColliderJntSphInitType1* src);
s32 Collider_SetJntSph(struct PlayState* play, ColliderJntSph* dest, struct Actor* actor, ColliderJntSphInit* src, ColliderJntSphElement* jntSphElements);
s32 Collider_InitAndSetJntSph(struct PlayState* play, ColliderJntSph* dest, struct Actor* actor, ColliderJntSphInit* src, ColliderJntSphElement* jntSphElements);
s32 Collider_ResetJntSphAT(struct PlayState* play, Collider* col);
s32 Collider_ResetJntSphAC(struct PlayState* play, Collider* col);
s32 Collider_ResetJntSphOC(struct PlayState* play, Collider* col);
s32 Collider_InitCylinderDim(struct PlayState* play, Cylinder16* dim);
s32 Collider_DestroyCylinderDim(struct PlayState* play, Cylinder16* dim);
s32 Collider_SetCylinderDim(struct PlayState* play, Cylinder16* dim, Cylinder16* src);
s32 Collider_InitCylinder(struct PlayState* play, ColliderCylinder* cyl);
s32 Collider_DestroyCylinder(struct PlayState* play, ColliderCylinder* cyl);
s32 Collider_SetCylinderToActor(struct PlayState* play, ColliderCylinder* dest, ColliderCylinderInitToActor* src);
s32 Collider_SetCylinderType1(struct PlayState* play, ColliderCylinder* dest, struct Actor* actor, ColliderCylinderInitType1* src);
s32 Collider_SetCylinder(struct PlayState* play, ColliderCylinder* dest, struct Actor* actor, ColliderCylinderInit* src);
s32 Collider_InitAndSetCylinder(struct PlayState* play, ColliderCylinder* dest, struct Actor* actor, ColliderCylinderInit* src);
s32 Collider_ResetCylinderAT(struct PlayState* play, Collider* col);
s32 Collider_ResetCylinderAC(struct PlayState* play, Collider* col);
s32 Collider_ResetCylinderOC(struct PlayState* play, Collider* col);
s32 Collider_InitTrisElementDim(struct PlayState* play, TriNorm* dim);
s32 Collider_DestroyTrisElementDim(struct PlayState* play, TriNorm* dim);
s32 Collider_SetTrisElementDim(struct PlayState* play, TriNorm* dest, ColliderTrisElementDimInit* src);
s32 Collider_InitTrisElement(struct PlayState* play, ColliderTrisElement* trisElem);
s32 Collider_DestroyTrisElement(struct PlayState* play, ColliderTrisElement* trisElem);
s32 Collider_SetTrisElement(struct PlayState* play, ColliderTrisElement* dest, ColliderTrisElementInit* src);
s32 Collider_ResetTrisElementAT(struct PlayState* play, ColliderTrisElement* trisElem);
s32 Collider_ResetTrisElementAC(struct PlayState* play, ColliderTrisElement* trisElem);
s32 Collider_ResetTrisElementOC(struct PlayState* play, ColliderTrisElement* trisElem);
s32 Collider_InitTris(struct PlayState* play, ColliderTris* tris);
s32 Collider_FreeTris(struct PlayState* play, ColliderTris* tris);
s32 Collider_DestroyTris(struct PlayState* play, ColliderTris* tris);
s32 Collider_SetTrisAllocType1(struct PlayState* play, ColliderTris* dest, struct Actor* actor, ColliderTrisInitType1* src);
s32 Collider_SetTris(struct PlayState* play, ColliderTris* dest, struct Actor* actor, ColliderTrisInit* src, ColliderTrisElement* trisElements);
s32 Collider_InitAndSetTris(struct PlayState* play, ColliderTris* dest, struct Actor* actor, ColliderTrisInit* src, ColliderTrisElement* trisElements);
s32 Collider_ResetTrisAT(struct PlayState* play, Collider* col);
s32 Collider_ResetTrisAC(struct PlayState* play, Collider* col);
s32 Collider_ResetTrisOC(struct PlayState* play, Collider* col);
s32 Collider_InitQuadDim(struct PlayState* play, ColliderQuadDim* dim);
s32 Collider_DestroyQuadDim(struct PlayState* play, ColliderQuadDim* dim);
s32 Collider_ResetQuadACDist(struct PlayState* play, ColliderQuadDim* dim);
void Collider_SetQuadMidpoints(ColliderQuadDim* dim);
s32 Collider_SetQuadDim(struct PlayState* play, ColliderQuadDim* dest, ColliderQuadDimInit* src);
s32 Collider_InitQuad(struct PlayState* play, ColliderQuad* quad);
s32 Collider_DestroyQuad(struct PlayState* play, ColliderQuad* quad);
s32 Collider_SetQuadType1(struct PlayState* play, ColliderQuad* dest, struct Actor* actor, ColliderQuadInitType1* src);
s32 Collider_SetQuad(struct PlayState* play, ColliderQuad* dest, struct Actor* actor, ColliderQuadInit* src);
s32 Collider_InitAndSetQuad(struct PlayState* play, ColliderQuad* dest, struct Actor* actor, ColliderQuadInit* src);
s32 Collider_ResetQuadAT(struct PlayState* play, Collider* col);
s32 Collider_ResetQuadAC(struct PlayState* play, Collider* col);
s32 Collider_ResetQuadOC(struct PlayState* play, Collider* col);
s32 Collider_QuadSetNearestAC(struct PlayState* play, ColliderQuad* quad, Vec3f* hitPos);
s32 Collider_InitSphere(struct PlayState* play, ColliderSphere* sph);
s32 Collider_DestroySphere(struct PlayState* play, ColliderSphere* sph);
s32 Collider_SetSphere(struct PlayState* play, ColliderSphere* dest, struct Actor* actor, ColliderSphereInit* src);
s32 Collider_InitAndSetSphere(struct PlayState* play, ColliderSphere* dest, struct Actor* actor, ColliderSphereInit* src);
s32 Collider_ResetSphereAT(struct PlayState* play, Collider* col);
s32 Collider_ResetSphereAC(struct PlayState* play, Collider* col);
s32 Collider_ResetSphereOC(struct PlayState* play, Collider* col);
s32 Collider_InitLine(struct PlayState* play, OcLine* line);
s32 Collider_DestroyLine(struct PlayState* play, OcLine* line);
s32 Collider_SetLinePoints(struct PlayState* play, OcLine* line, Vec3f* a, Vec3f* b);
s32 Collider_SetLine(struct PlayState* play, OcLine* dst, OcLine* src);
s32 Collider_ResetLineOC(struct PlayState* play, OcLine* line);
void CollisionCheck_InitContext(struct PlayState* play, CollisionCheckContext* colChkCtx);
void CollisionCheck_DestroyContext(struct PlayState* play, CollisionCheckContext* colCtxt);
void CollisionCheck_ClearContext(struct PlayState* play, CollisionCheckContext* colChkCtx);
void CollisionCheck_EnableSAC(struct PlayState* play, CollisionCheckContext* colChkCtx);
void CollisionCheck_DisableSAC(struct PlayState* play, CollisionCheckContext* colChkCtx);
s32 CollisionCheck_SetAT(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col);
s32 CollisionCheck_SetAT_SAC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, s32 index);
s32 CollisionCheck_SetAC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col);
s32 CollisionCheck_SetAC_SAC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, s32 index);
s32 CollisionCheck_SetOC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col);
s32 CollisionCheck_SetOC_SAC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, s32 index);
s32 CollisionCheck_SetOCLine(struct PlayState* play, CollisionCheckContext* colChkCtx, OcLine* line);
s32 CollisionCheck_IsElementNotAT(ColliderElement* elem);
s32 CollisionCheck_IsElementNotAC(ColliderElement* elem);
s32 CollisionCheck_NoSharedFlags(ColliderElement* atElem, ColliderElement* acElem);
void CollisionCheck_NoBlood(struct PlayState* play, Collider* collider, Vec3f* v);
void CollisionCheck_BlueBlood(struct PlayState* play, Collider* collider, Vec3f* v);
void CollisionCheck_GreenBlood(struct PlayState* play, Collider* collider, Vec3f* v);
void CollisionCheck_WaterBurst(struct PlayState* play, Collider* collider, Vec3f* v);
void CollisionCheck_RedBlood(struct PlayState* play, Collider* collider, Vec3f* v);
void CollisionCheck_RedBloodUnused(struct PlayState* play, Collider* collider, Vec3f* v);
void CollisionCheck_HitSolid(struct PlayState* play, ColliderElement* elem, Collider* col, Vec3f* hitPos);
s32 CollisionCheck_SwordHitAudio(Collider* at, ColliderElement* acElem);
void CollisionCheck_HitEffects(struct PlayState* play, Collider* at, ColliderElement* atElem, Collider* ac, ColliderElement* acElem, Vec3f* hitPos);
void CollisionCheck_SetBounce(Collider* at, Collider* ac);
s32 CollisionCheck_SetATvsAC(struct PlayState* play, Collider* atCol, ColliderElement* atElem, Vec3f* atPos, Collider* acCol, ColliderElement* acElem, Vec3f* acPos, Vec3f* hitPos);
void CollisionCheck_TrisAvgPoint(ColliderTrisElement* trisElem, Vec3f* avg);
void CollisionCheck_QuadAvgPoint(ColliderQuad* quad, Vec3f* avg);
void CollisionCheck_AC_JntSphVsJntSph(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_JntSphVsCyl(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_JntSphVsTris(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_JntSphVsQuad(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_JntSphVsSphere(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_CylVsJntSph(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_CylVsCyl(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_CylVsTris(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_CylVsQuad(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_CylVsSphere(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_TrisVsJntSph(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_TrisVsCyl(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_TrisVsTris(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_TrisVsQuad(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_TrisVsSphere(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_QuadVsJntSph(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_QuadVsCyl(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_QuadVsTris(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_QuadVsQuad(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_QuadVsSphere(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_SphereVsJntSph(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_SphereVsCylinder(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_SphereVsTris(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_SphereVsQuad(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_AC_SphereVsSphere(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* atCol, Collider* acCol);
void CollisionCheck_SetJntSphHitFX(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* col);
void CollisionCheck_SetCylHitFX(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* col);
void CollisionCheck_SetTrisHitFX(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* col);
void CollisionCheck_SetQuadHitFX(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* col);
void CollisionCheck_SetSphereHitFX(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* col);
void CollisionCheck_SetHitEffects(struct PlayState* play, CollisionCheckContext* colChkCtx);
void CollisionCheck_AC(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol);
void CollisionCheck_AT(struct PlayState* play, CollisionCheckContext* colChkCtx);
s32 CollisionCheck_GetMassType(u8 mass);
void CollisionCheck_SetOCvsOC(struct PlayState* play, Collider* leftCol, ColliderElement* leftElem, Vec3f* leftPos, Collider* rightCol, ColliderElement* rightElem, Vec3f* rightPos, f32 overlapSize);
void CollisionCheck_OC_JntSphVsJntSph(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* leftCol, Collider* rightCol);
void CollisionCheck_OC_JntSphVsCyl(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* leftCol, Collider* rightCol);
void CollisionCheck_OC_JntSphVsSphere(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* leftCol, Collider* rightCol);
void CollisionCheck_OC_CylVsJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol, Collider* rightCol);
void CollisionCheck_OC_CylVsCyl(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* leftCol, Collider* rightCol);
void CollisionCheck_OC_CylVsSphere(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* leftCol, Collider* rightCol);
void CollisionCheck_OC_SphereVsJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol, Collider* rightCol);
void CollisionCheck_OC_SphereVsCyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol, Collider* rightCol);
void CollisionCheck_OC_SphereVsSphere(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* leftCol, Collider* rightCol);
s32 CollisionCheck_SkipOC(Collider* col);
s32 CollisionCheck_Incompatible(Collider* leftCol, Collider* rightCol);
void CollisionCheck_OC(struct PlayState* play, CollisionCheckContext* colChkCtx);
void CollisionCheck_InitInfo(CollisionCheckInfo* info);
void CollisionCheck_ResetDamage(CollisionCheckInfo* info);
void CollisionCheck_SetInfoNoDamageTable(CollisionCheckInfo* info, CollisionCheckInfoInit* init);
void CollisionCheck_SetInfo(CollisionCheckInfo* info, DamageTable* damageTable, CollisionCheckInfoInit* init);
void CollisionCheck_SetInfo2(CollisionCheckInfo* info, DamageTable* damageTable, CollisionCheckInfoInit2* init);
void CollisionCheck_SetInfoGetDamageTable(CollisionCheckInfo* info, s32 index, CollisionCheckInfoInit2* init);
void CollisionCheck_ApplyDamage(struct PlayState* play, CollisionCheckContext* colCtxt, Collider* col, ColliderElement* elem);
void CollisionCheck_ApplyDamageJntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col);
void CollisionCheck_ApplyDamageCyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col);
void CollisionCheck_ApplyDamageTris(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col);
void CollisionCheck_ApplyDamageQuad(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col);
void CollisionCheck_ApplyDamageSphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col);
void CollisionCheck_Damage(struct PlayState* play, CollisionCheckContext* colChkCtx);
s32 CollisionCheck_LineOC_JntSph(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, Vec3f* a, Vec3f* b);
s32 CollisionCheck_LineOC_Cyl(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, Vec3f* a, Vec3f* b);
s32 CollisionCheck_LineOC_Sphere(struct PlayState* play, CollisionCheckContext* colChkCtx, Collider* col, Vec3f* a, Vec3f* b);
s32 CollisionCheck_LineOC(struct PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b, struct Actor** exclusions, s32 numExclusions);
s32 CollisionCheck_LineOCCheckAll(struct PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b);
s32 CollisionCheck_LineOCCheck(struct PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b, struct Actor** exclusions, s32 numExclusions);
void Collider_UpdateCylinder(struct Actor* actor, ColliderCylinder* cyl);
void Collider_SetCylinderPosition(ColliderCylinder* cyl, Vec3s* pos);
void Collider_SetQuadVertices(ColliderQuad* quad, Vec3f* a, Vec3f* b, Vec3f* c, Vec3f* d);
void Collider_SetTrisVertices(ColliderTris* tris, s32 elemIndex, Vec3f* a, Vec3f* b, Vec3f* c);
void Collider_SetTrisDim(struct PlayState* play, ColliderTris* dest, s32 elemIndex, ColliderTrisElementDimInit* src);
void Collider_UpdateSpheres(s32 limb, ColliderJntSph* jntSph);
void Collider_UpdateSpheresElement(ColliderJntSph* jntSph, s32 elemIndex, struct Actor* actor);
void Collider_UpdateSphere(s32 limb, ColliderSphere* sph);
void CollisionCheck_SpawnRedBlood(struct PlayState* play, Vec3f* v);
void CollisionCheck_SpawnWaterDroplets(struct PlayState* play, Vec3f* v);
void CollisionCheck_SpawnShieldParticles(struct PlayState* play, Vec3f* v);
void CollisionCheck_SpawnShieldParticlesMetal(struct PlayState* play, Vec3f* v);
void CollisionCheck_SpawnShieldParticlesMetalSound(struct PlayState* play, Vec3f* v, Vec3f* pos);
void CollisionCheck_SpawnShieldParticlesMetal2(struct PlayState* play, Vec3f* v);
void CollisionCheck_SpawnShieldParticlesWood(struct PlayState* play, Vec3f* v, Vec3f* pos);
s32 CollisionCheck_CylSideVsLineSeg(f32 radius, f32 height, f32 offset, Vec3f* actorPos, Vec3f* itemPos, Vec3f* itemProjPos, Vec3f* out1, Vec3f* out2);

#endif
