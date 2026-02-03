/**
 * item_demise_destruction.c - Demise's Destruction (super attack)
 *
 * Controls:
 *   C Button: Perform devastating magic attack (high MP cost)
 *
 * Features:
 *   - Large area explosion with lightning effects
 *   - Heavy damage to all enemies in radius
 *   - Ground only (cannot use in water or air)
 *   - Custom superhero landing animation
 */

#include "z64.h"
#include "item_demise_destruction.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "../anim/superhero/demise_anim.h"
#include "../anim/superhero/demise_anim_data.c"
#include "../anim/superhero/demise_anim.c"

static s8 sDemisePrevInvinc = 0;
static FX_Color sDemiseDustColor = { 60, 0, 0, 255 };

static void Demise_Stop(Player* p, PlayState* play) {
    if (!ddActive) return;
    ddCollider.base.atFlags &= ~(AT_ON | AT_HIT);
    ddActive = 0; ddState = DEMISE_STATE_IDLE; ddTimer = 0;
}

static void Demise_Start(Player* p, PlayState* play) {
    if (ddActive) return;
    if (!ItemMagic_HasEnough(play, DEMISE_MAGIC_COST)) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        return;
    }
    if (!(p->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) return;

    ddActive = 1; ddState = DEMISE_STATE_WINDUP; ddTimer = -2;
    ItemMagic_Consume(play, DEMISE_MAGIC_COST);
}

static void Demise_FinalExplosion(PlayState* play, Player* p) {
    // Ring of explosions + center explosion
    FX_SpawnRadialExplosion(play, &p->actor.world.pos, 400.0f, 12, 1.0f);
    FX_SpawnExplosion(play, &p->actor.world.pos, 1.0f);

    Audio_PlaySoundGeneral(NA_SE_IT_HAMMER_HIT, &p->actor.world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    Audio_PlaySoundGeneral(NA_SE_IT_BOMB_EXPLOSION, &p->actor.world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    func_800AA000(800.0f, 0xFF, 0x28, 0xC8);
}

static void Demise_StateWindup(Player* p, PlayState* play) {
    ddTimer++;

    if (ddTimer == -1) {
        Camera_ChangeSetting(Play_GetCamera(play, 0), CAM_SET_TURN_AROUND);
        Camera_SetCameraData(Play_GetCamera(play, 0), 4, NULL, NULL, 10, 0, 0);
        p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    }

    p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    p->linearVelocity = 0.0f; p->actor.speedXZ = 0.0f;
    p->actor.velocity.x = p->actor.velocity.y = p->actor.velocity.z = 0.0f;

    if (ddTimer == 0) {
        LinkAnimation_Change(play, &p->skelAnime, &gDemiseDestructionAnim, 0.65f, 0.0f,
            Animation_GetLastFrame(&gDemiseDestructionAnim), ANIMMODE_ONCE, -8.0f);
    }

    if (ddTimer >= 0) {
        LinkAnimation_Update(play, &p->skelAnime);
    }

    if (ddTimer == 20) {
        Audio_PlaySoundGeneral(NA_SE_EN_GANON_AT_RETURN, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    if (ddTimer > 20) {
        if ((ddTimer - 20) % 8 == 0) FX_SpawnLightningRing(play, &p->actor.world.pos, 200.0f, 560.0f, 1, 1, 80);
        if (play->gameplayFrames % 4 == 0) FX_SpawnRadialDust(play, &p->actor.world.pos, 80.0f, 320.0f, 4, &sDemiseDustColor);
    }

    if (ddTimer > 50) {
        if (ddTimer % 4 == 0) {
            FX_SpawnLightningRing(play, &p->actor.world.pos, 200.0f, 560.0f, 2, 1, 80);
            Audio_PlaySoundGeneral(NA_SE_EV_LIGHTNING, &p->actor.world.pos, 4,
                &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
        if (play->gameplayFrames % 3 == 0) FX_SpawnRadialDust(play, &p->actor.world.pos, 80.0f, 320.0f, 8, &sDemiseDustColor);
        if (ddTimer % 8 == 0) {
            Audio_PlaySoundGeneral(NA_SE_EV_EARTHQUAKE, &p->actor.world.pos, 4,
                &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
        if (ddTimer % 2 == 0) func_800AA000(200.0f, 180, 20, 10);
    }

    if (ddTimer == DEMISE_WINDUP_DURATION) {
        Demise_FinalExplosion(play, p);
        p->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED);
        func_8005B1A4(Play_GetCamera(play, 0));
        ddState = DEMISE_STATE_FINISH; ddTimer = 0;
    }
}

static void Demise_StateFinish(Player* p, PlayState* play) {
    ddCollider.dim.pos.x = (s16)p->actor.world.pos.x;
    ddCollider.dim.pos.y = (s16)p->actor.world.pos.y;
    ddCollider.dim.pos.z = (s16)p->actor.world.pos.z;
    ddCollider.dim.radius = (s16)DEMISE_COLLISION_RADIUS;
    ddCollider.dim.height = DEMISE_COLLISION_HEIGHT;
    ddCollider.info.toucher.dmgFlags = DEMISE_DAMAGE_FLAGS;
    ddCollider.info.toucher.damage = DEMISE_DAMAGE;
    ddCollider.info.toucher.effect = 1;
    ddCollider.base.atFlags |= AT_ON | AT_TYPE_PLAYER;

    CollisionCheck_SetAT(play, &play->colChkCtx, &ddCollider.base);

    if (ddCollider.base.atFlags & AT_HIT) {
        Audio_PlaySoundGeneral(NA_SE_IT_HAMMER_HIT, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    ddTimer++;
    if (ddTimer >= DEMISE_FINISH_DURATION) Demise_Stop(p, play);
}

void Handle_DemiseDestruction(Player* p, PlayState* play) {
    if (ddCollider.base.shape != COLSHAPE_CYLINDER) {
        Player_InitDemiseDestructionIA(play, p);
    }

    ItemInputState in;
    ItemInput_Update(&in, ITEM_DEMISE_DESTRUCTION, p, play);

    if (!in.wasEquipped) { if (ddActive) Demise_Stop(p, play); return; }
    if (ItemInput_CheckDamage(p, &sDemisePrevInvinc)) { Demise_Stop(p, play); return; }
    if (in.otherButtonPressed) { Demise_Stop(p, play); return; }

    // Cannot use in water
    if (p->stateFlags1 & PLAYER_STATE1_IN_WATER) return;

    if (!ddActive) {
        if (ItemInput_IsBlocked(p, play)) return;
        if (in.isPressed) Demise_Start(p, play);
        return;
    }

    switch (ddState) {
        case DEMISE_STATE_WINDUP: Demise_StateWindup(p, play); break;
        case DEMISE_STATE_FINISH: Demise_StateFinish(p, play); break;
        default: Demise_Stop(p, play); break;
    }
}

void CustomItems_DrawDemiseDestruction(Player* p, PlayState* play) {}

s32 Player_UpperAction_DemiseDestruction(Player* p, PlayState* play) { return 0; }

void Player_InitDemiseDestructionIA(PlayState* play, Player* p) {
    // Only initialize if collider not already set up (prevents resetting active state)
    if (ddCollider.base.shape != COLSHAPE_CYLINDER) {
        ddActive = 0; ddTimer = 0; ddState = DEMISE_STATE_IDLE;
        Collider_InitCylinder(play, &ddCollider);
        Collider_SetCylinder(play, &ddCollider, &p->actor, &sDemiseColInit);
    }
}
