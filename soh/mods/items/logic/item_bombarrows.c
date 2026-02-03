/**
 * item_bombarrows.c - Bomb Arrows from Twilight Princess
 *
 * Controls:
 *   Hold C Button:  Aim in first-person mode (bomb visible on arrow)
 *   Release C:      Fire explosive arrow
 *
 * Features:
 *   - Consumes 1 arrow + 1 bomb per shot
 *   - Bomb follows Link while aiming (can explode if fuse expires)
 *   - Authentic explosion using real bomb actor
 *   - First-person aiming with reticle
 */

#include "z64.h"
#include "item_bombarrows.h"
#include "../custom_items.h"
#include "../helpers/camera_helper.h"
#include "../helpers/equip_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

static void BombArrows_Stop(Player* p, PlayState* play);
static void BombArrows_FireArrow(Player* p, PlayState* play);
static s8 sBombArrowPrevInvinc = 0;
static s32 sBaAnimState = -1; // Tracks which animation state the upper action last initiated

// Check if player can use bomb arrows
static u8 BombArrows_CanUse(Player* p, PlayState* play) {
    if (AMMO(ITEM_BOW) <= 0 || AMMO(ITEM_BOMB) <= 0) return 0;
    if (!LINK_IS_ADULT && !CVarGetInteger(CVAR_ENHANCEMENT("BowSlingshotAmmoFix"), 0)) return 0;
    return 1;
}

// Consume 1 arrow + 1 bomb
static void BombArrows_ConsumeAmmo(void) {
    Inventory_ChangeAmmo(ITEM_BOW, -1);
    Inventory_ChangeAmmo(ITEM_BOMB, -1);
}

// Get bomb position offset for Link (near his hands/bow)
static void BombArrows_GetBombOffset(Player* p, Vec3f* out) {
    out->x = p->actor.world.pos.x;
    out->y = p->actor.world.pos.y + 45.0f;  // Above hands
    out->z = p->actor.world.pos.z;
}

// Spawn a real bomb that explodes instantly at impact position
// Combines both arrow and bomb damage types for full damage coverage
static void BombArrows_SpawnInstantBomb(PlayState* play, Vec3f* pos) {
    EnBom* bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM,
        pos->x, pos->y, pos->z, 0, 0, 0, BOMB_BODY, true);

    if (bomb != NULL) {
        // Timer=1 will decrement to 0 on first update and trigger explosion
        bomb->timer = 1;

        // Scale must be set (init chain sets to 0, normally set at timer=67)
        Actor_SetScale(&bomb->actor, 0.01f);

        // CRITICAL: Initialize explosion collider position manually.
        // Normally Collider_UpdateSpheres is called in EnBom_Draw while params==BOMB_BODY,
        // but since we explode on frame 1, Update runs first (params becomes BOMB_EXPLOSION)
        // and Draw never updates the collider position. Set it here so the explosion
        // collider is at the correct position when EnBom_Explode registers it.
        bomb->explosionCollider.elements[0].dim.worldSphere.center.x = (s16)pos->x;
        bomb->explosionCollider.elements[0].dim.worldSphere.center.y = (s16)pos->y;
        bomb->explosionCollider.elements[0].dim.worldSphere.center.z = (s16)pos->z;

        // Add arrow damage type (0x00000800) to bomb damage type (0x00000008)
        // This makes the explosion hit enemies vulnerable to either damage type
        bomb->explosionCollider.elements[0].info.toucher.dmgFlags |= 0x00000800;
    }
}

// Stop bomb arrows - clean up all state
static void BombArrows_Stop(Player* p, PlayState* play) {
    // Exit first-person mode
    if (baFirstPerson) {
        FirstPerson_Exit(p, play);
        baFirstPerson = 0;
    }

    // Kill bomb actor if exists (without explosion - just remove it)
    if (baBombActor != NULL && baBombActor->update != NULL) {
        Actor_Kill(baBombActor);
    }
    baBombActor = NULL;

    // Clean up arrow actor reference
    if (baArrowActor != NULL) {
        if (p->heldActor == baArrowActor) p->heldActor = NULL;
        if (p->actor.child == baArrowActor) p->actor.child = NULL;
    }
    baArrowActor = NULL;

    // Reset state
    baActive = 0;
    baState = BOMBARROW_STATE_IDLE;

    ItemEquip_PlayUnequipSFX(play, p);
}

// Start charging - spawn real bomb on Link
static void BombArrows_StartCharge(Player* p, PlayState* play) {
    if (!BombArrows_CanUse(p, play)) {
        Sfx_PlaySfxCentered(NA_SE_SY_ERROR);
        return;
    }

    Vec3f bombPos;
    BombArrows_GetBombOffset(p, &bombPos);

    // Spawn real bomb actor with normal fuse (timer=70)
    EnBom* bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM,
        bombPos.x, bombPos.y, bombPos.z, 0, 0, 0, BOMB_BODY, true);

    if (bomb == NULL) {
        Sfx_PlaySfxCentered(NA_SE_SY_ERROR);
        return;
    }

    baBombActor = &bomb->actor;
    baActive = 1;
    baState = BOMBARROW_STATE_CHARGING;
    baArrowActor = NULL;

    // Animation is handled by Player_UpperAction_BombArrows (detects state transitions)

    // Enter first-person mode if not Z-targeting
    if (!Player_IsZTargeting(p)) {
        FirstPerson_Init(p, play);
        baFirstPerson = 1;
    } else {
        baFirstPerson = 0;
    }

    ItemEquip_PlayEquipSFX(play, p);
    Player_PlaySfx(p, NA_SE_IT_BOW_DRAW);
}

// Get aim yaw based on camera mode
static s16 BombArrows_GetAimYaw(Player* p, PlayState* play) {
    if (baFirstPerson) return FirstPerson_GetAimYaw(p);
    if (Player_IsZTargeting(p) && p->focusActor != NULL)
        return Math_Vec3f_Yaw(&p->actor.world.pos, &p->focusActor->focus.pos);
    return p->actor.shape.rot.y;
}

// Get aim pitch
static s16 BombArrows_GetAimPitch(Player* p) {
    return baFirstPerson ? FirstPerson_GetAimPitch(p) : 0;
}

// Update charging state - bomb follows Link
static void BombArrows_UpdateCharging(Player* p, PlayState* play, ItemInputState* in) {
    // Animation update is handled by Player_UpperAction_BombArrows

    // Handle Z-targeting transitions
    u8 isZTargeting = Player_IsZTargeting(p);
    if (baFirstPerson && isZTargeting) {
        FirstPerson_Exit(p, play);
        baFirstPerson = 0;
    } else if (!baFirstPerson && !isZTargeting) {
        FirstPerson_Init(p, play);
        baFirstPerson = 1;
    }

    // Keep first-person updated
    if (baFirstPerson) {
        FirstPerson_Update(p, play);
    }

    // Update bomb position to follow Link
    if (baBombActor != NULL && baBombActor->update != NULL) {
        Vec3f bombPos;
        BombArrows_GetBombOffset(p, &bombPos);
        baBombActor->world.pos = bombPos;

        // Check if bomb exploded (timer reached 0 and params changed to BOMB_EXPLOSION)
        EnBom* bomb = (EnBom*)baBombActor;
        if (baBombActor->params == BOMB_EXPLOSION || bomb->timer <= 0) {
            // Bomb exploded on Link! Consume ammo and stop
            BombArrows_ConsumeAmmo();
            baBombActor = NULL;  // Don't kill it, let it finish its explosion
            baActive = 0;
            baState = BOMBARROW_STATE_IDLE;
            if (baFirstPerson) {
                FirstPerson_Exit(p, play);
                baFirstPerson = 0;
            }
            return;
        }
    } else {
        // Bomb was destroyed somehow
        BombArrows_Stop(p, play);
        return;
    }

    // Bow charging sound
    func_8002F974(&p->actor, NA_SE_PL_ARROW_CHARGE_FIRE - SFX_FLAG);

    // Fire arrow when button released
    if (!in->isHeld) {
        BombArrows_FireArrow(p, play);
        return;
    }

    // Cancel with B or other button
    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B) || in->otherButtonPressed) {
        BombArrows_Stop(p, play);
        return;
    }
}

// Fire the arrow
static void BombArrows_FireArrow(Player* p, PlayState* play) {
    s16 aimYaw = BombArrows_GetAimYaw(p, play);
    s16 aimPitch = BombArrows_GetAimPitch(p);

    // Animation is handled by Player_UpperAction_BombArrows (detects state transitions)

    // Kill the bomb actor (no explosion - we fire the arrow instead)
    if (baBombActor != NULL && baBombActor->update != NULL) {
        Actor_Kill(baBombActor);
    }
    baBombActor = NULL;

    // Consume ammo
    BombArrows_ConsumeAmmo();

    // Spawn arrow
    baArrowActor = Actor_SpawnAsChild(&play->actorCtx, &p->actor, play, ACTOR_EN_ARROW,
        p->actor.world.pos.x, p->actor.world.pos.y + 40.0f, p->actor.world.pos.z,
        aimPitch, aimYaw, 0, ARROW_NORMAL);

    if (baArrowActor != NULL) {
        baArrowActor->world.rot.x = aimPitch;
        baArrowActor->world.rot.y = aimYaw;
        baArrowActor->shape.rot.x = aimPitch;
        baArrowActor->shape.rot.y = aimYaw;

        // Trigger arrow to fly
        p->heldActor = baArrowActor;
        p->unk_A73 = 4;
        baArrowActor->parent = NULL;
        p->actor.child = NULL;
        p->heldActor = NULL;
    }

    Player_PlaySfx(p, NA_SE_IT_ARROW_SHOT);
    Player_PlaySfx(p, NA_SE_IT_BOW_FLICK);

    baState = BOMBARROW_STATE_FLYING;

    // Exit first-person after firing
    if (baFirstPerson) {
        FirstPerson_Exit(p, play);
        baFirstPerson = 0;
    }
}

// Update flying state - watch for arrow impact
static void BombArrows_UpdateFlying(Player* p, PlayState* play) {
    // Animation update is handled by Player_UpperAction_BombArrows

    // Check if arrow is gone
    if (baArrowActor == NULL || baArrowActor->update == NULL) {
        baActive = 0;
        baState = BOMBARROW_STATE_IDLE;
        baArrowActor = NULL;
        return;
    }

    EnArrow* arrow = (EnArrow*)baArrowActor;

    // Check for any hit (wall, floor, or actor via AT_HIT)
    // hitFlags & 1 = hit something (wall or actor that bounced)
    // hitFlags & 2 = hit actor specifically
    // Also check collider AT_HIT for actor collision
    u8 hitSomething = (arrow->hitFlags & 1) || (arrow->collider.base.atFlags & AT_HIT);

    if (hitSomething) {
        BombArrows_SpawnInstantBomb(play, &baArrowActor->world.pos);
        // Kill the arrow immediately
        Actor_Kill(baArrowActor);
        baActive = 0;
        baState = BOMBARROW_STATE_IDLE;
        baArrowActor = NULL;
        return;
    }

    // Check if arrow timer expired
    if (arrow->timer <= 0) {
        baActive = 0;
        baState = BOMBARROW_STATE_IDLE;
        baArrowActor = NULL;
        return;
    }

    // Small fuse sparks on flying arrow (scale=40, smaller than vanilla scale=100)
    if ((play->gameplayFrames % 3) == 0) {
        Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
        Vec3f effAccel = { 0.0f, 0.0f, 0.0f };
        Color_RGBA8 primColor = { 255, 255, 150, 255 };
        Color_RGBA8 envColor = { 255, 0, 0, 0 };
        EffectSsGSpk_SpawnAccel(play, baArrowActor, &baArrowActor->world.pos,
            &effVelocity, &effAccel, &primColor, &envColor, 40, 2);
    }

    // Fuse sound while flying
    func_8002F974(&p->actor, NA_SE_IT_BOMB_IGNIT - SFX_FLAG);
}

// Main handler
void Handle_BombArrows(Player* p, PlayState* play) {
    ItemInputState in;
    ItemInput_Update(&in, ITEM_BOMB_ARROWS, p, play);
    baButtonMask = in.equippedButton;

    if (!in.wasEquipped) {
        if (baActive) BombArrows_Stop(p, play);
        return;
    }

    if (ItemInput_IsBlocked(p, play)) {
        if (baActive) BombArrows_Stop(p, play);
        return;
    }

    if (ItemInput_CheckDamage(p, &sBombArrowPrevInvinc)) {
        if (baActive) BombArrows_Stop(p, play);
        return;
    }

    switch (baState) {
        case BOMBARROW_STATE_IDLE:
            if (in.isPressed) BombArrows_StartCharge(p, play);
            break;
        case BOMBARROW_STATE_CHARGING:
            BombArrows_UpdateCharging(p, play, &in);
            break;
        case BOMBARROW_STATE_FLYING:
            BombArrows_UpdateFlying(p, play);
            break;
        default:
            baState = BOMBARROW_STATE_IDLE;
            break;
    }
}

void Player_InitBombArrowsIA(PlayState* play, Player* p) {
    baActive = 0;
    baState = BOMBARROW_STATE_IDLE;
    baBombActor = NULL;
    baArrowActor = NULL;
    baFirstPerson = 0;
    baButtonMask = 0;
    sBaAnimState = -1;
    p->stateFlags1 |= PLAYER_STATE1_ITEM_IN_HAND;
}

s32 Player_UpperAction_BombArrows(Player* this, PlayState* play) {
    // Visual: force bow model in right hand (model group is DEFAULT for animation compatibility)
    this->rightHandType = PLAYER_MODELTYPE_RH_BOW_SLINGSHOT;

    // Idle: let lower body control everything (same pattern as Beetle/Cane/DekuLeaf)
    if (!baActive) {
        sBaAnimState = -1;
        return 0;
    }

    // Detect state transitions and start appropriate animation
    // All animation init happens HERE (inside the upper action) so it's in the same
    // execution context as LinkAnimation_Update and AnimationContext_SetCopyTrue.
    if ((s32)baState != sBaAnimState) {
        sBaAnimState = baState;
        switch (baState) {
            case BOMBARROW_STATE_CHARGING:
                LinkAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_bow_bow_ready);
                break;
            case BOMBARROW_STATE_FLYING:
                LinkAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_bow_bow_shoot);
                break;
        }
    }

    // Advance animation and handle transitions when finished
    if (LinkAnimation_Update(play, &this->upperSkelAnime)) {
        switch (baState) {
            case BOMBARROW_STATE_CHARGING:
                LinkAnimation_PlayLoop(play, &this->upperSkelAnime, &gPlayerAnim_link_bow_bow_wait);
                break;
            case BOMBARROW_STATE_FLYING:
                LinkAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_bow_bow_shoot_end);
                break;
            default:
                break;
        }
    }

    return 1;
}

// Draw reticle when aiming bomb arrows
void CustomItems_DrawBombArrowsReticle(Player* p, PlayState* play) {
    if (!baFirstPerson || baState != BOMBARROW_STATE_CHARGING) return;

    // Use shared reticle function - red color (255, 0, 0)
    FirstPerson_DrawReticle(p, play, 0.0f, 255, 0, 0);
}
