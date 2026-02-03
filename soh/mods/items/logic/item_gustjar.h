/**
 * Gust Jar Item Header
 * Suction/projectile item with ammo types
 */

#ifndef ITEM_GUSTJAR_H
#define ITEM_GUSTJAR_H

#include "z64.h"
#include "../custom_items.h"

// Range
#define GUST_RANGE_MAX      220.0f
#define GUST_RANGE_CAPTURE  50.0f
#define GUST_RANGE_SHRINK   120.0f
#define LINK_HEIGHT_HITBOX  80.0f

// Ammo types
#define GUST_AMMO_NONE      0
#define GUST_AMMO_PHYSICAL  1
#define GUST_AMMO_FIRE      2
#define GUST_AMMO_ICE       3
#define GUST_AMMO_SHOCK     4
#define GUST_AMMO_BREAK     5

// Damage flags
#define DMG_GU_PHYSICAL (1 << 0x09)
#define DMG_GU_FIRE     (1 << 0x0B)
#define DMG_GU_ICE      (1 << 0x0C)
#define DMG_GU_SHOCK    (1 << 0x0D)

// Actor IDs
#ifndef ACTOR_EN_SW
#define ACTOR_EN_SW 0x0095
#endif
#ifndef ACTOR_EN_SI
#define ACTOR_EN_SI 0x0096
#endif

// State aliases
#define gjEquipped       gCustomItemState.gustJarEquipped
#define gjMode           gCustomItemState.gustJarMode
#define gjAmmoType       gCustomItemState.gustJarAmmoType
#define gjProjectileActive gCustomItemState.gustJarProjectileActive
#define gjProjPos        gCustomItemState.gustJarProjPos
#define gjProjYaw        gCustomItemState.gustJarProjYaw
#define gjProjPitch      gCustomItemState.gustJarProjPitch
#define gjTimer          gCustomItemState.gustJarTimer
#define gjCollider       gCustomItemState.gustJarCollider
#define gjFirstPerson    gCustomItemState.gustJarFirstPersonActive
#define gjAimMode        gCustomItemState.gustJarAimMode
#define gjButtonMask     gCustomItemState.gustJarButtonMask
#define gjScaleCache     gCustomItemState.gustJarScaleCache
#define gjScaleCacheCount gCustomItemState.gustJarScaleCacheCount

// Collider init
static ColliderCylinderInit sGustJarColliderInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_TYPE_PLAYER, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK0, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, TOUCH_ON | TOUCH_SFX_NONE, BUMP_NONE, OCELEM_NONE },
    { 30, 30, 0, { 0, 0, 0 } }
};

// Ammo definition
typedef struct {
    s16 actorId;
    s16 params;
    u8 ammoType;
} GustJarDefinition;

/**
 * Gust Jar Ammo Table
 * Defines which actors can be sucked up and what ammo type they provide.
 * Format: { actorId, params (-1 = any), ammoType }
 *
 * GUST_AMMO_BREAK    - Actor is destroyed on suction (pots, bushes)
 * GUST_AMMO_PHYSICAL - Normal projectile ammo
 * GUST_AMMO_FIRE     - Fire element projectile
 * GUST_AMMO_ICE      - Ice element projectile
 * GUST_AMMO_SHOCK    - Electric element projectile
 */
static const GustJarDefinition sGustJarTable[] = {
    // Breakable props
    { ACTOR_EN_KUSA, -1, GUST_AMMO_BREAK },       // Grass/bushes
    { ACTOR_OBJ_TSUBO, -1, GUST_AMMO_BREAK },     // Pots
    { ACTOR_OBJ_KIBAKO, -1, GUST_AMMO_BREAK },    // Crates
    { 0x01A0, -1, GUST_AMMO_BREAK },              // Other breakable

    // Physical ammo enemies
    { ACTOR_EN_FIREFLY, 0, GUST_AMMO_PHYSICAL },  // Keese (normal)
    { ACTOR_EN_TITE, -1, GUST_AMMO_PHYSICAL },    // Tektite
    { ACTOR_EN_GOMA, -1, GUST_AMMO_PHYSICAL },    // Gohma Larva
    { ACTOR_EN_BUBBLE, 2, GUST_AMMO_PHYSICAL },   // Bubble (green)
    { ACTOR_EN_BUBBLE, 3, GUST_AMMO_PHYSICAL },   // Bubble (white)
    { ACTOR_EN_DEKUNUTS, -1, GUST_AMMO_PHYSICAL }, // Deku Scrub
    { 0x001D, -1, GUST_AMMO_PHYSICAL },           // Skulltula
    { 0x002D, -1, GUST_AMMO_PHYSICAL },           // Gold Skulltula

    // Fire ammo enemies
    { ACTOR_EN_FIREFLY, 2, GUST_AMMO_FIRE },      // Fire Keese
    { ACTOR_EN_BUBBLE, 0, GUST_AMMO_FIRE },       // Red Bubble
    { ACTOR_EN_TORCH2, -1, GUST_AMMO_FIRE },      // Dark Link (fire)
    { 0x002F, -1, GUST_AMMO_FIRE },               // Fire enemy

    // Ice ammo enemies
    { ACTOR_EN_FIREFLY, 4, GUST_AMMO_ICE },       // Ice Keese
    { ACTOR_EN_BUBBLE, 1, GUST_AMMO_ICE },        // Blue Bubble
    { ACTOR_EN_FZ, 1, GUST_AMMO_ICE },            // Freezard

    // Shock ammo enemies
    { ACTOR_EN_BILI, -1, GUST_AMMO_SHOCK },       // Biri (jellyfish)
    { 0x0043, -1, GUST_AMMO_SHOCK },              // Electric enemy
    { ACTOR_EN_SW, -1, GUST_AMMO_SHOCK },         // Walltula
};
#define GUSTJAR_TABLE_COUNT (sizeof(sGustJarTable) / sizeof(sGustJarTable[0]))

#endif // ITEM_GUSTJAR_H
