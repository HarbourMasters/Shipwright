#include "soh/Enhancements/mm/forms/FormsCommon.h"

#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "soh/Enhancements/mm/MMPlayerAnims.h"
}

// Zora form: fast swimming, free diving, no drown timer and lakebed walking, adapted from
// MM's Zora swim actions (2Ship2Harkinian Player_Action_56 / func_80850BF8 / func_80850BA8)
// and grafted onto OOT's swim system:
//
// - A with stick input while in any vanilla swim action starts the MM fast swim (pz_fishswim,
//   full 3D movement steered by pitch/yaw), replacing OOT's depth-limited A-dive.
// - Dive (A) with the stick neutral — or B while swimming, MM's actual control (mm
//   func_8083A04C toggles PLAYER_BOOTS_ZORA_UNDERWATER on B) — sinks to the lakebed for
//   underwater walking. MM implements the sink with PLAYER_BOOTS_ZORA_UNDERWATER; here the
//   same result comes from temporarily forcing OOT's iron boots: the vanilla
//   currentBoots != prevBoots block in Player_UpdateCommon does the whole swim → sink →
//   land transition, and restoring the boots floats back up through func_8083D36C the same
//   way. While sunk (mid-descent or on the lakebed), A with the stick neutral floats back
//   to the surface (MM's A control); A with stick input kicks into the fast swim.
// - Releasing A winds down through pz_swimtowait back to the vanilla paddle.
// - MM zeroes underwaterTimer for the Zora every frame (mm z_player.c func_8083BB4C); doing
//   the same here disables OOT's underwater env-hazard timer and drowning damage.
// - Regular paddling gets MM's Zora speed edge via VB_PLAYER_MODIFY_SWIM_SPEED.
//
// VB_PLAYER_RESET_SWIM_STATE keeps func_8083D53C from stomping the custom swim actions:
// deep water would reset them into the vanilla tread-water action every frame, and nearing
// the surface would pop the player out of the water (turn-in-place + water exit) — the
// fast swim handles breaching itself instead (ballistic arc, splash back in).

typedef struct ZoraSwimState {
    bool fastSwim;      // MM PLAYER_STATE3_8000: in the fish-swim actions
    bool bootsForced;   // iron boots override active (MM PLAYER_BOOTS_ZORA_UNDERWATER)
    s16 pitch;          // MM unk_AAA: dive angle, positive = downward
    s16 floorBumpTimer; // MM unk_B8C: cooldown after bouncing off the lakebed
    f32 speed3D;        // MM unk_B48: speed along the pitched swim direction
} ZoraSwimState;

static ZoraSwimState sSwim;

void ZoraForm_FastSwimAction(Player* player, PlayState* play);
void ZoraForm_SwimToWaitAction(Player* player, PlayState* play);

static bool IsZoraSwimActionFunc(PlayerActionFunc func) {
    return func == ZoraForm_FastSwimAction || func == ZoraForm_SwimToWaitAction;
}

static bool IsVanillaSwimAction(PlayerActionFunc func) {
    return func == Player_Action_8084D610 || func == Player_Action_8084D84C || func == Player_Action_8084DAB4 ||
           func == Player_Action_8084DC48;
}

static bool IsNeutralGroundAction(Player* player) {
    return player->actionFunc == Player_Action_Idle || player->actionFunc == Player_Action_80840450 ||
           player->actionFunc == Player_Action_808407CC || player->actionFunc == Player_Action_80840DE4 ||
           player->actionFunc == Player_Action_808414F8 || player->actionFunc == Player_Action_8084170C;
}

static void RestoreBoots(PlayState* play, Player* player) {
    if (sSwim.bootsForced) {
        sSwim.bootsForced = false;
        player->currentBoots = CUR_EQUIP_VALUE(EQUIP_TYPE_BOOTS) - 1;
        // Sync prevBoots so Player_UpdateCommon's boots-diff block doesn't run
        // func_8083D36C and stomp an action we install this same frame
        player->prevBoots = player->currentBoots;
        Player_SetBootData(play, player);
    }
}

static void StartFastSwim(PlayState* play, Player* player) {
    RestoreBoots(play, player);
    Player_SetupAction(play, player, ZoraForm_FastSwimAction, 1);
    Player_AnimPlayLoopAdjusted(play, player, (LinkAnimationHeader*)gPlayerAnim_pz_fishswim);
    sSwim.fastSwim = true;
    sSwim.floorBumpTimer = 0;
    // Carry the current motion into the pitched swim direction (MM Action_28)
    sSwim.pitch = Math_Atan2S(player->linearVelocity, -player->actor.velocity.y);
    sSwim.pitch = CLAMP(sSwim.pitch, -0x3000, 0x3000);
    // MM's swim kick (Action_56 entry phase) bursts the swim speed to 16.0, which the
    // AsymStep in the steering then bleeds down to the 9.0 sustained target
    sSwim.speed3D = 16.0f;
    Player_PlaySfx(&player->actor, NA_SE_PL_SWIM);
}

static void ExitFastSwim(PlayState* play, Player* player) {
    sSwim.fastSwim = false;
    player->actor.shape.rot.x = 0;
}

static void StartSink(PlayState* play, Player* player) {
    sSwim.bootsForced = true;
    // prevBoots is deliberately left stale: Player_UpdateCommon's boots-diff block sees the
    // change and performs the whole swim -> sink transition (and gates off the vanilla
    // A-dive, func_8083D12C's currentBoots != PLAYER_BOOTS_IRON check)
    player->currentBoots = PLAYER_BOOTS_IRON;
    Player_SetBootData(play, player);
    if (sSwim.fastSwim) {
        ExitFastSwim(play, player);
        if (IsZoraSwimActionFunc(player->actionFunc)) {
            func_80838F18(play, player);
        }
    }
    Player_PlaySfx(&player->actor, NA_SE_PL_DIVE_BUBBLE);
}

static void StartFloat(PlayState* play, Player* player) {
    // MM func_8083A04C: A while on the underwater boots switches back to the land boots and
    // buoyancy floats the Zora to the surface. prevBoots is deliberately left stale (still
    // iron) so Player_UpdateCommon's boots-diff block runs func_8083D36C, the vanilla
    // boots-off-underwater transition that puts the player into the rising swim.
    sSwim.bootsForced = false;
    player->currentBoots = CUR_EQUIP_VALUE(EQUIP_TYPE_BOOTS) - 1;
    Player_SetBootData(play, player);
    Player_PlaySfx(&player->actor, NA_SE_PL_DIVE_BUBBLE);
}

void ZoraForm_FastSwimAction(Player* player, PlayState* play) {
    Input* input = &play->state.input[0];

    player->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    // Without this, func_8083AA10 stomps the action into vanilla freefall during the breach arc
    player->stateFlags3 |= PLAYER_STATE3_MIDAIR;

    // Anim speed follows the swim speed (MM func_808477D0)
    player->skelAnime.playSpeed = CLAMP(sSwim.speed3D * 0.15f, 1.0f, 2.5f) * PLAYER_ANIM_ADJUSTED_SPEED;
    LinkAnimation_Update(play, &player->skelAnime);

    if ((player->stateFlags1 & PLAYER_STATE1_IN_WATER) && !(player->actor.bgCheckFlags & 0x20)) {
        // Breached the surface: vanilla water-exit housekeeping, but keep this action
        // (the vanilla near-surface pop-out in func_8083D53C is suppressed via
        // VB_PLAYER_RESET_SWIM_STATE, so this is the only exit path while fast swimming)
        func_8083D0A8(play, player, player->actor.velocity.y);
    }

    if (!(player->stateFlags1 & PLAYER_STATE1_IN_WATER)) {
        if ((player->actor.bgCheckFlags & 0x20) && (player->actor.yDistToWater > player->ageProperties->unk_2C)) {
            // Splashed back in: redo func_8083D36C's water-entry housekeeping and carry the
            // arc velocity back into the swim direction
            if (func_8083CFA8(play, player, player->actor.velocity.y, 500)) {
                Player_PlaySfx(&player->actor, NA_SE_EV_DIVE_INTO_WATER);
            }
            player->stateFlags1 |= PLAYER_STATE1_IN_WATER;
            player->stateFlags1 &= ~(PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALL);
            player->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
            Player_SetBootData(play, player);
            sSwim.pitch = Math_Atan2S(player->linearVelocity, -player->actor.velocity.y);
            sSwim.pitch = CLAMP(sSwim.pitch, -0x3000, 0x3000);
            sSwim.speed3D = CLAMP_MIN(sqrtf(SQ(player->linearVelocity) + SQ(player->actor.velocity.y)), 3.0f);
        } else {
            // Ballistic arc until splashing back in or landing (shallow water counts as land)
            player->actor.gravity = -1.0f;
            sSwim.pitch = Math_Atan2S(player->linearVelocity, -player->actor.velocity.y);
            player->actor.shape.rot.x = sSwim.pitch;

            if (player->actor.bgCheckFlags & 1) {
                ExitFastSwim(play, player);
                func_80839F90(player, play);
            }
            return;
        }
    }

    player->actor.gravity = 0.0f;

    if (!CHECK_BTN_ALL(input->cur.button, BTN_A)) {
        // A released: wind down through pz_swimtowait
        Player_SetupAction(play, player, ZoraForm_SwimToWaitAction, 1);
        Player_AnimPlayOnceAdjusted(play, player, (LinkAnimationHeader*)gPlayerAnim_pz_swimtowait);
        return;
    }

    // --- steering (MM func_80850BF8 + Action_56 pitch control) ---
    s8 stickX = input->rel.stick_x;
    s8 stickY = input->rel.stick_y;

    Math_AsymStepToF(&sSwim.speed3D, 9.0f, 1.0f, (fabsf(sSwim.speed3D) * 0.01f) + 0.4f);

    f32 turnCurve = Math_CosS(stickX * 0x10E);
    s16 yawStep = (s16)(((stickX >= 0) ? 1 : -1) * (1.0f - turnCurve) * -1100.0f);
    yawStep = CLAMP(yawStep, -0x1F40, 0x1F40);
    player->yaw += yawStep;

    s16 pitchTarget = stickY * 0xC8;
    if (sSwim.floorBumpTimer != 0) {
        sSwim.floorBumpTimer--;
        pitchTarget = CLAMP_MAX(pitchTarget, (s16)(player->floorPitch - 0xFA0));
    }
    // Near the surface, keep a slight downward pitch unless deliberately pitching up to breach
    if ((sSwim.pitch >= -0x1555) && (player->actor.yDistToWater < (player->ageProperties->unk_24 + 10.0f))) {
        pitchTarget = CLAMP_MIN(pitchTarget, 0x7D0);
    }
    Math_SmoothStepToS(&sSwim.pitch, pitchTarget, 4, 0xFA0, 0x190);

    // Bounce off the lakebed (MM Action_56)
    if ((player->actor.bgCheckFlags & 1) && (sSwim.floorBumpTimer < 8)) {
        sSwim.pitch += (s16)((-player->floorPitch - sSwim.pitch) * 2);
        sSwim.floorBumpTimer = 15;
        Player_PlaySfx(&player->actor, NA_SE_PL_BOUND); // MM NA_SE_PL_BODY_BOUND
    }

    // Head-on wall bonk
    if ((player->actor.bgCheckFlags & 8) && (sSwim.speed3D > 4.0f)) {
        s16 facingWall = player->yaw - (s16)(player->actor.wallYaw + 0x8000);

        if (ABS(facingWall) < 0x1C00) {
            sSwim.speed3D = 2.0f;
            Player_RequestQuake(play, 33267, 3, 12);
            Player_RequestRumble(player, 255, 20, 150, 0);
            Player_PlaySfx(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }

    // Apply the pitched swim velocity (MM func_80850BA8). Positive shape.rot.x pitches the
    // model nose-DOWN — same sign as the dive pitch (MM rotates the root limb by +unk_AAA,
    // mm z_player_lib.c:2442); note this is opposite to Actor_UpdateVelocityXYZ's
    // world.rot.x, where positive means upward velocity
    player->linearVelocity = Math_CosS(sSwim.pitch) * sSwim.speed3D;
    player->actor.velocity.y = -Math_SinS(sSwim.pitch) * sSwim.speed3D;
    player->actor.shape.rot.y = player->yaw;
    player->actor.shape.rot.x = sSwim.pitch;

    if ((play->gameplayFrames % 16) == 0) {
        Player_PlaySfx(&player->actor, NA_SE_PL_SWIM); // MM NA_SE_PL_ZORA_SWIM_LV
    }
}

void ZoraForm_SwimToWaitAction(Player* player, PlayState* play) {
    player->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    player->actor.gravity = 0.0f;

    Math_StepToF(&player->linearVelocity, 0.0f, 1.0f);
    Math_StepToF(&player->actor.velocity.y, 0.0f, 1.0f);
    Math_ScaledStepToS(&player->actor.shape.rot.x, 0, 0x400);

    if (LinkAnimation_Update(play, &player->skelAnime)) {
        bool deep = player->actor.yDistToWater > (player->ageProperties->unk_2C + 40.0f);

        ExitFastSwim(play, player);
        if (deep) {
            // Sink and walk the lakebed: MM's Zora underwater boots, via OOT's iron boots
            sSwim.bootsForced = true;
        }
        func_80838F18(play, player);
    }
}

static void CleanupZoraSwim(PlayState* play, Player* player) {
    RestoreBoots(play, player);
    if (sSwim.fastSwim) {
        ExitFastSwim(play, player);
        if (IsZoraSwimActionFunc(player->actionFunc)) {
            if (player->stateFlags1 & PLAYER_STATE1_IN_WATER) {
                func_80838F18(play, player);
            } else {
                func_80839F90(player, play);
            }
        }
    }
}

static void RegisterZoraForm() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::ShouldActorUpdate>([](void* actorRef, bool*) {
        Actor* actor = (Actor*)actorRef;
        PlayState* play = gPlayState;

        if (play == nullptr || actor != &GET_PLAYER(play)->actor) {
            return;
        }
        Player* player = GET_PLAYER(play);

        if (PlayerForm_GetApplied() != PLAYER_SHIPFORM_ZORA) {
            if (sSwim.fastSwim || sSwim.bootsForced) {
                CleanupZoraSwim(play, player);
            }
            return;
        }

        // Our action was replaced externally (damage, cutscene, grab)
        if (sSwim.fastSwim && !IsZoraSwimActionFunc(player->actionFunc)) {
            ExitFastSwim(play, player);
        }

        // Iron-boots override while sunk. Keep the boots for as long as the engine says we
        // are in water: vanilla clears IN_WATER itself once the player walks up into the
        // shallows (func_8083D53C's near-surface exit). A depth test here would release the
        // boots the moment a sink starts from the surface, where yDistToWater is still small
        if (sSwim.bootsForced) {
            bool stillUnder = (player->stateFlags1 & PLAYER_STATE1_IN_WATER) != 0;

            if (!stillUnder || sSwim.fastSwim) {
                RestoreBoots(play, player);
            } else if (player->currentBoots != PLAYER_BOOTS_IRON) {
                // Reassert after pause-menu equipment changes
                player->currentBoots = PLAYER_BOOTS_IRON;
                Player_SetBootData(play, player);
            }
        }

        if (Player_InBlockingCsMode(play, player) ||
            (player->stateFlags1 &
             (PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_CARRYING_ACTOR))) {
            return;
        }

        Input* input = &play->state.input[0];

        if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
            bool stickNeutral = (ABS(input->rel.stick_x) < 10) && (ABS(input->rel.stick_y) < 10);

            if (sSwim.bootsForced) {
                bool grounded = (player->actor.bgCheckFlags & 1) != 0;

                if (!grounded || IsNeutralGroundAction(player)) {
                    if (stickNeutral) {
                        // A while sunk — standing or mid-descent — floats back up to the
                        // surface (MM func_8083A04C: A switches the underwater boots back
                        // to the land boots)
                        StartFloat(play, player);
                    } else {
                        // With stick input, kick straight into the fast swim instead
                        StartFastSwim(play, player);
                        if (grounded) {
                            sSwim.pitch = -0x1800; // kick off the lakebed slightly upward
                        }
                    }
                }
            } else if (IsVanillaSwimAction(player->actionFunc) || player->actionFunc == ZoraForm_SwimToWaitAction) {
                // Dive with the stick neutral sinks to the lakebed; with stick input it
                // starts the fast swim (replacing the vanilla depth-limited dive)
                if (stickNeutral) {
                    StartSink(play, player);
                } else {
                    StartFastSwim(play, player);
                }
            }
        } else if (CHECK_BTN_ALL(input->press.button, BTN_B) && !sSwim.bootsForced &&
                   (IsVanillaSwimAction(player->actionFunc) || IsZoraSwimActionFunc(player->actionFunc))) {
            // MM's actual sink control: B while swimming (mm func_8083A04C)
            StartSink(play, player);
        }
    });

    // MM zeroes the Zora's underwater timer every frame; this kills both the env-hazard
    // (drowning) timer and the deep-underwater face textures
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (PlayerForm_GetApplied() == PLAYER_SHIPFORM_ZORA && gPlayState != nullptr) {
            GET_PLAYER(gPlayState)->underwaterTimer = 0;
        }
    });

    // Keep func_8083D53C from stomping the custom swim actions (deep-water tread reset
    // and the near-surface pop-out; the fast swim handles breaching itself)
    REGISTER_VB_SHOULD(VB_PLAYER_RESET_SWIM_STATE, {
        Player* player = va_arg(args, Player*);

        if (IsZoraSwimActionFunc(player->actionFunc)) {
            *should = false;
        }
    });

    // MM's Zora paddles noticeably faster than the other forms even outside the fast swim
    REGISTER_VB_SHOULD(VB_PLAYER_MODIFY_SWIM_SPEED, {
        va_arg(args, Player*);
        f32* swimSpeed = va_arg(args, f32*);

        if (PlayerForm_GetApplied() == PLAYER_SHIPFORM_ZORA) {
            *swimSpeed *= 1.5f;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterZoraForm);
