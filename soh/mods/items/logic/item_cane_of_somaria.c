/**
 * item_cane_of_somaria.c - Cane of Somaria from A Link to the Past
 *
 * Controls:
 *   C Button: Swing cane and spawn a pushable block
 *   C Button (on block): Destroy block to spawn 4 projectiles
 *
 * Features:
 *   - Creates magical blocks that can be pushed onto switches
 *   - Maximum 3 blocks at a time (oldest destroyed when limit reached)
 *   - Destroying blocks spawns 4-way projectile attack
 *   - Uses cane swing animation
 */

#include "z64.h"
#include "item_cane_of_somaria.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

// Include the cube system
#include "../../actors/somaria_cubes.h"
#include "../../actors/somaria_cubes.c"

// Include object (for draw function) - original staff model for Link's hand
#include "../objects/object_cane_of_somaria.c"

// Include animation
#include "../anim/somaria_cane/somaria_anim.c"
#include "../anim/somaria_cane/somaria_anim_data.c"

// Static state like Fire Rod
static ItemEquipState sSomariaEquipState = { 0 };
static s8 sSomariaPrevInvinc = 0;

// ============================================================================
// BLOCK MANAGEMENT
// ============================================================================

static void Somaria_CleanupInvalidBlocks(void) {
    for (u8 i = 0; i < SOMARIA_MAX_CUBES; i++) {
        if (somariaBlocks[i] != NULL && somariaBlocks[i]->update == NULL) {
            somariaBlocks[i] = NULL;
            if (somariaBlockCount > 0) somariaBlockCount--;
        }
    }
}

static u8 Somaria_FindEmptySlot(void) {
    for (u8 i = 0; i < SOMARIA_MAX_CUBES; i++) {
        if (somariaBlocks[i] == NULL) return i;
    }
    return 0xFF;
}

static u8 Somaria_FindOldestBlock(void) {
    for (u8 i = 0; i < SOMARIA_MAX_CUBES; i++) {
        u8 slot = (somariaOldestSlot + i) % SOMARIA_MAX_CUBES;
        if (somariaBlocks[slot] != NULL && somariaBlocks[slot]->update != NULL) {
            return slot;
        }
    }
    return 0xFF;
}

static void Somaria_DestroyOldestBlock(PlayState* play) {
    u8 slot = Somaria_FindOldestBlock();
    if (slot == 0xFF) return;

    Actor* cube = somariaBlocks[slot];
    if (cube != NULL && cube->update != NULL) {
        SomariaCube_PlaySound(cube, NA_SE_EV_BLOCK_BOUND);
        Actor_Kill(cube);
    }

    somariaBlocks[slot] = NULL;
    if (somariaBlockCount > 0) somariaBlockCount--;
    somariaOldestSlot = (slot + 1) % SOMARIA_MAX_CUBES;
}

// Spawn radial sparkles around position (like chest spawn circle)
static void Somaria_SpawnRadialSparkles(PlayState* play, Vec3f* center, f32 radius, u8 count) {
    Vec3f pos, vel, accel;
    Color_RGBA8 primColor = { 80, 150, 255, 255 };   // Blue primary
    Color_RGBA8 envColor = { 40, 80, 200, 255 };     // Darker blue env

    accel.x = 0.0f;
    accel.y = 0.0f;
    accel.z = 0.0f;

    for (u8 i = 0; i < count; i++) {
        f32 angle = (f32)i * (65536.0f / (f32)count);
        s16 angleS = (s16)angle;

        pos.x = center->x + Math_SinS(angleS) * radius;
        pos.y = center->y + 20.0f + Rand_ZeroFloat(15.0f);
        pos.z = center->z + Math_CosS(angleS) * radius;

        // Velocity moving outward and upward
        vel.x = Math_SinS(angleS) * 1.5f;
        vel.y = Rand_ZeroFloat(2.0f) + 1.0f;
        vel.z = Math_CosS(angleS) * 1.5f;

        EffectSsKiraKira_SpawnFocused(play, &pos, &vel, &accel, &primColor, &envColor, 800, 30);
    }
}

static void Somaria_SpawnBlock(Player* p, PlayState* play) {
    if (somariaBlockCount >= SOMARIA_MAX_CUBES) {
        Somaria_DestroyOldestBlock(play);
    }

    u8 slot = Somaria_FindEmptySlot();
    if (slot == 0xFF) return;

    Vec3f spawnPos;
    s16 yaw = p->actor.shape.rot.y;
    spawnPos.x = p->actor.world.pos.x + Math_SinS(yaw) * 50.0f;
    spawnPos.y = p->actor.world.pos.y;
    spawnPos.z = p->actor.world.pos.z + Math_CosS(yaw) * 50.0f;

    CollisionPoly* outPoly = NULL;
    s32 bgId = BGCHECK_SCENE;
    f32 floorHeight = BgCheck_EntityRaycastFloor5(play, &play->colCtx, &outPoly, &bgId, &p->actor, &spawnPos);
    if (floorHeight > BGCHECK_Y_MIN) spawnPos.y = floorHeight;

    Actor* cube = SomariaCube_Spawn(play, &spawnPos, yaw);
    if (cube != NULL) {
        somariaBlocks[slot] = cube;
        somariaBlockCount++;

        // VFX 1: White shockwave flash (like deku nut peak)
        Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
        Vec3f flashPos = spawnPos;
        flashPos.y += 20.0f;
        EffectSsBlast_SpawnWhiteShockwave(play, &flashPos, &zeroVec, &zeroVec);

        // VFX 2: Radial sparkle circle (like chest spawn)
        Somaria_SpawnRadialSparkles(play, &spawnPos, 30.0f, 12);
    }
}

// ============================================================================
// ANIMATION
// ============================================================================

// Animation uses upperSkelAnime with gSomariaAnim (60 frames)
// Spawn block at frame 30 (midpoint of the casting animation)
#define SOMARIA_SPAWN_FRAME 30

static void Somaria_StartCastAnim(Player* p, PlayState* play) {
    // Play the somaria casting animation
    LinkAnimation_PlayOnce(play, &p->upperSkelAnime, &gSomariaAnim);
    shSomariaAnimating = 1;
    shSomariaAnimTimer = 0;
}

// ============================================================================
// EQUIP/UNEQUIP CALLBACKS (like Fire Rod)
// ============================================================================

static void Somaria_OnEquip(PlayState* play, Player* p) {
    shSomariaActive = 1;
    somariaState = SOMARIA_STATE_EQUIPPED;
    shSomariaAnimating = 0;
    shSomariaAnimTimer = 0;
    ItemEquip_PlayEquipSFX(play, p);
}

static void Somaria_OnUnequip(PlayState* play, Player* p) {
    shSomariaActive = 0;
    somariaState = SOMARIA_STATE_INACTIVE;
    shSomariaAnimating = 0;
    shSomariaAnimTimer = 0;
    somariaButtonMask = 0;
    ItemEquip_PlayUnequipSFX(play, p);
}

// ============================================================================
// MAIN HANDLER (like Fire Rod pattern)
// ============================================================================

void Handle_CaneOfSomaria(Player* p, PlayState* play) {
    ItemInputState in;
    ItemInput_Update(&in, ITEM_CANE_OF_SOMARIA, p, play);
    somariaButtonMask = in.equippedButton;

    // Not equipped - cleanup
    if (!in.wasEquipped) {
        if (shSomariaActive) Somaria_OnUnequip(play, p);
        sSomariaEquipState.isEquipped = 0;
        return;
    }

    // Blocking check (like Fire Rod)
    if (!shSomariaActive) {
        if (ItemInput_IsBlockedEx(p, play, 1)) return;
    } else {
        u32 criticalBlocks = (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_LOADING |
            PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_TALKING | PLAYER_STATE1_GETTING_ITEM |
            PLAYER_STATE1_DAMAGED | PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE |
            PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_HOOKSHOT_FALLING);
        if (p->stateFlags1 & criticalBlocks) {
            return;
        }
    }

    // Damage check (like Fire Rod)
    if (ItemInput_CheckDamage(p, &sSomariaPrevInvinc)) {
        if (shSomariaActive) Somaria_OnUnequip(play, p);
        sSomariaEquipState.isEquipped = 0;
        return;
    }

    // Use ItemEquip_Update with callbacks (like Fire Rod)
    ItemEquip_Update(&sSomariaEquipState, &in, Somaria_OnEquip, Somaria_OnUnequip, p, play);

    if (!shSomariaActive) return;

    // Always cleanup invalid blocks
    Somaria_CleanupInvalidBlocks();

    // If animating, just wait for animation to complete (handled by upper action)
    if (shSomariaAnimating) {
        // Stop movement during animation
        p->actor.speedXZ = 0.0f;
        p->linearVelocity = 0.0f;
        return;
    }

    // Check for activation
    if (p->stateFlags1 & PLAYER_STATE1_IN_WATER) return;
    if (p->meleeWeaponState != 0) return;

    if (in.isPressed && (p->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Somaria_StartCastAnim(p, play);
        somariaState = SOMARIA_STATE_CASTING;
        ItemEquip_PlayEquipSFX(play, p);
    }
}

// ============================================================================
// INIT (like Fire Rod)
// ============================================================================

void Player_InitCaneOfSomariaIA(PlayState* play, Player* p) {
    shSomariaActive = 1;
    somariaState = SOMARIA_STATE_EQUIPPED;
    shSomariaAnimating = 0;
    shSomariaAnimTimer = 0;
    somariaButtonMask = 0;
}

// ============================================================================
// UPPER ACTION
// ============================================================================

s32 Player_UpperAction_CaneOfSomaria(Player* player, PlayState* play) {
    // Only active when somaria is active and animating
    if (!shSomariaActive) return 0;
    if (!shSomariaAnimating) return 0;

    // Update animation
    if (LinkAnimation_Update(play, &player->upperSkelAnime)) {
        // Animation finished
        shSomariaAnimating = 0;
        shSomariaAnimTimer = 0;
        somariaState = SOMARIA_STATE_EQUIPPED;
    } else {
        // Animation in progress - track frame
        shSomariaAnimTimer++;

        // Spawn block at the spawn frame
        if (shSomariaAnimTimer == SOMARIA_SPAWN_FRAME) {
            Somaria_SpawnBlock(player, play);
            Audio_PlayActorSound2(&player->actor, NA_SE_PL_MAGIC_FIRE);
        }
    }

    // Return 1 to indicate upper body is busy (use upperSkelAnime)
    return 1;
}
