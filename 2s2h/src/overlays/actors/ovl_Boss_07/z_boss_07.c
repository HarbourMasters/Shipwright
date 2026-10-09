/*
 * File: z_boss_07.c
 * Overlay: ovl_Boss_07
 * Description: Majora
 *
 * This actor is responsible for handling everything related to the final boss battle with Majora's Mask and its various
 * forms. To be more specific, this actor handles the following things:
 * - Majora's Mask, including the dynamically-moving tentacles
 * - Majora's Incarnation
 * - Majora's Wrath, including the dynamically-moving whips
 * - The floating boss remains (Odolwa, Goht, Gyorg, and Twinmold) that fly around and attack the player
 * - The projectiles that both the boss remains and Majora's Incarnation fire at the player
 * - The spinning tops that Majora's Wrath can throw
 * - The afterimages that follow Majora's Incarnation around as it runs
 * - An invisible "battle handler" that updates the flame effects, manages the lens flare, handles the cutscene where
 *   the boss remains are activated, etc.
 * - An invisible "battle initializer" that spawns Majora's Mask and the "battle handler", resets the effects, etc.
 */

#include "prevent_bss_reordering.h"
#include "z_boss_07.h"
#include "z64shrink_window.h"
#include "attributes.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#include "2s2h/BenPort.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define MAJORA_TENTACLE_COUNT 25
#define MAJORA_WHIP_LENGTH 44
#define MAJORA_EFFECT_COUNT 50

typedef struct MajoraEffect {
    /* 0x00 */ u8 type;
    /* 0x02 */ s16 texScroll;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ UNK_TYPE1 unk28[4];
    /* 0x2C */ s16 alpha;
    /* 0x2E */ UNK_TYPE1 unk2E[2];
    /* 0x30 */ s16 isFadingAway;
    /* 0x34 */ f32 scale;
    /* 0x38 */ UNK_TYPE1 unk38[0x10];
} MajoraEffect; // size = 0x48

typedef enum MajorasWrathDamageEffect {
    // Named because everything with this effect is ignored thanks to `CollisionCheck_SetATvsAC`.
    /* 0x0 */ MAJORAS_WRATH_DMGEFF_IMMUNE,

    // Stuns and surrounds Wrath with fire.
    /* 0x2 */ MAJORAS_WRATH_DMGEFF_FIRE = 2,

    // Stuns and surrounds Wrath with ice that shatters after a short time.
    /* 0x3 */ MAJORAS_WRATH_DMGEFF_FREEZE,

    // Stuns and surrounds Wrath with yellow light orbs.
    /* 0x4 */ MAJORAS_WRATH_DMGEFF_LIGHT_ORB,

    // Deals damage and surrounds Wrath with blue light orbs.
    /* 0x9 */ MAJORAS_WRATH_DMGEFF_BLUE_LIGHT_ORB = 9,

    // Stuns and surrounds Wrath with electric sparks.
    /* 0xA */ MAJORAS_WRATH_DMGEFF_ELECTRIC_SPARKS,

    // When an attack with this effect hits Wrath while it is either stunned or currently playing its damaged animation,
    // it sets the `damagedTimer` to 15 frames, which is longer than the usual 5 frames.
    /* 0xC */ MAJORAS_WRATH_DMGEFF_EXPLOSIVE = 0xC,

    // Deals damage and has no special effect.
    /* 0xD */ MAJORAS_WRATH_DMGEFF_DAMAGE_NONE,

    // When an attack with this effect hits Wrath while it is currently playing its damaged animation, it checks to see
    // if the attack landed within the last 4 frames of the animation. If so, it will restart Wrath's damaged animation.
    // Otherwise, it will set the `disableCollisionTimer` to 30 frames.
    /* 0xE */ MAJORAS_WRATH_DMGEFF_ANIM_FRAME_CHECK,

    // Stuns and has no special effect.
    /* 0xF */ MAJORAS_WRATH_DMGEFF_STUN_NONE
} MajorasWrathDamageEffect;

typedef enum MajorasIncarnationDamageEffect {
    // Named because everything with this effect is ignored thanks to `CollisionCheck_SetATvsAC`.
    /* 0x0 */ MAJORAS_INCARNATION_DMGEFF_IMMUNE,

    // Deals damage and surrounds Incarnation with fire.
    /* 0x2 */ MAJORAS_INCARNATION_DMGEFF_FIRE = 2,

    // Deals damage and surrounds Incarnation with ice that shatters after a short time.
    /* 0x3 */ MAJORAS_INCARNATION_DMGEFF_FREEZE,

    // Deals damage and surrounds Incarnation with yellow light orbs.
    /* 0x4 */ MAJORAS_INCARNATION_DMGEFF_LIGHT_ORB,

    // Deals damage and surrounds Incarnation with blue light orbs.
    /* 0x9 */ MAJORAS_INCARNATION_DMGEFF_BLUE_LIGHT_ORB = 9,

    // Deals damage and surrounds Incarnation with electric sparks.
    /* 0xA */ MAJORAS_INCARNATION_DMGEFF_ELECTRIC_SPARKS,

    // When an attack with this effect hits Incarnation while it is either stunned or currently playing its damaged
    // animation, it sets the `damagedTimer` to 15 frames, which is longer than the usual 5 frames.
    /* 0xC */ MAJORAS_INCARNATION_DMGEFF_EXPLOSIVE = 0xC,

    // Named after the only attack that uses it. Behaves exactly like `MAJORAS_INCARNATION_DMGEFF_NONE`.
    /* 0xD */ MAJORAS_INCARNATION_DMGEFF_SPIN_ATTACK,

    // When an attack with this effect hits Incarnation while it is currently playing its damaged animation, it checks
    // to see if the attack landed within the last 4 frames of the animation. If so, it will restart Incarnation's
    // damaged animation. Otherwise, it will set the `damagedTimer` and `disableCollisionTimer` to 30 frames.
    /* 0xE */ MAJORAS_INCARNATION_DMGEFF_ANIM_FRAME_CHECK,

    // Deals damage with no special effect.
    /* 0xF */ MAJORAS_INCARNATION_DMGEFF_NONE
} MajorasIncarnationDamageEffect;

// All of these damage effects (except `MAJORAS_MASK_DMGEFF_IMMUNE`) behave in exactly the same way. Therefore, these
// are all named based on the types of attacks that use them.
typedef enum MajorasMaskDamageEffect {
    // Named because everything with this effect is ignored thanks to `CollisionCheck_SetATvsAC`.
    /* 0x0 */ MAJORAS_MASK_DMGEFF_IMMUNE,
    /* 0x2 */ MAJORAS_MASK_DMGEFF_FIRE_ARROW = 2,
    /* 0x3 */ MAJORAS_MASK_DMGEFF_ICE_ARROW,
    /* 0x4 */ MAJORAS_MASK_DMGEFF_LIGHT_ARROW,
    /* 0x9 */ MAJORAS_MASK_DMGEFF_SWORD_BEAM = 9,
    /* 0xF */ MAJORAS_MASK_DMGEFF_DAMAGE = 0xF
} MajorasMaskDamageEffect;

// All of these damage effects (except `REMAINS_DMGEFF_IMMUNE`) behave in exactly the same way. Therefore, these are all
// named based on the types of attacks that use them.
typedef enum RemainsDamageEffect {
    // Named because everything with this effect is ignored thanks to `CollisionCheck_SetATvsAC`.
    /* 0x0 */ REMAINS_DMGEFF_IMMUNE,
    /* 0x2 */ REMAINS_DMGEFF_FIRE_ARROW = 2,
    /* 0x3 */ REMAINS_DMGEFF_ICE_ARROW,
    /* 0x4 */ REMAINS_DMGEFF_LIGHT_ARROW,
    /* 0x9 */ REMAINS_DMGEFF_SWORD_BEAM = 9,
    /* 0xD */ REMAINS_DMGEFF_SPIN_ATTACK = 0xD,
    /* 0xE */ REMAINS_DMGEFF_OTHER,
    /* 0xF */ REMAINS_DMGEFF_DAMAGE
} RemainsDamageEffect;

typedef enum TopDamageEffect {
    // Named because everything with this effect is ignored thanks to `CollisionCheck_SetATvsAC`.
    /* 0x0 */ TOP_DMGEFF_IMMUNE,

    // If an attack with this effect hits the top, the top's speed is set to -15. However, if the player is currently in
    // Fierce Diety form when the attack lands, the speed will be set to -30, and the top will be bounced into the air.
    /* 0xA */ TOP_DMGEFF_REVERSE_DIRECTION = 0xA,

    // If an attack with this effect hits the top, the top is bounced into the air and knocked backwards away from the
    // source of the damage.
    /* 0xB */ TOP_DMGEFF_BOUNCE_BACK_FROM_DAMAGE,

    // If an attack with this effect hits the top, the top is knocked backwards away from the player. The speed that the
    // top is knocked backwards depends on the player's speed when the top is hit.
    /* 0xC */ TOP_DMGEFF_KNOCKED_BACK_FROM_PLAYER,

    // If an attack with this effect hits the top, the player is quickly pushed backwards.
    /* 0xD */ TOP_DMGEFF_PUSH_BACK_PLAYER,

    // If an attack with this effect hits the top, it will not react in any way.
    /* 0xE */ TOP_DMGEFF_NO_REACTION_E,

    // If an attack with this effect hits the top, it will not react in any way.
    /* 0xF */ TOP_DMGEFF_NO_REACTION_F
} TopDamageEffect;

typedef enum MajoraEffectType {
    /* 0 */ MAJORA_EFFECT_NONE,
    /* 1 */ MAJORA_EFFECT_FLAME
} MajoraEffectType;

typedef enum MajorasWrathAttackSubAction {
    /* 0 */ MAJORAS_WRATH_ATTACK_SUB_ACTION_QUICK_WHIP,
    /* 1 */ MAJORAS_WRATH_ATTACK_SUB_ACTION_FLURRY,
    /* 2 */ MAJORAS_WRATH_ATTACK_SUB_ACTION_DOUBLE_WHIP,
    /* 3 */ MAJORAS_WRATH_ATTACK_SUB_ACTION_LONG_WHIP,
    /* 4 */ MAJORAS_WRATH_ATTACK_SUB_ACTION_SPIN_ATTACK,
    /* 5 */ MAJORAS_WRATH_ATTACK_SUB_ACTION_TAUNT,
    /* 6 */ MAJORAS_WRATH_ATTACK_SUB_ACTION_THREE_HIT,
    /* 7 */ MAJORAS_WRATH_ATTACK_SUB_ACTION_WHIP_ATTACK_MAX,
    /* 7 */ MAJORAS_WRATH_ATTACK_SUB_ACTION_KICK = 7,
} MajorasWrathAttackSubAction;

typedef enum MajorasIncarnationTauntSubAction {
    /* 0 */ MAJORAS_INCARNATION_TAUNT_SUB_ACTION_DANCE_1,
    /* 1 */ MAJORAS_INCARNATION_TAUNT_SUB_ACTION_DANCE_2,
    /* 2 */ MAJORAS_INCARNATION_TAUNT_SUB_ACTION_JUMP,
    /* 3 */ MAJORAS_INCARNATION_TAUNT_SUB_ACTION_MAX
} MajorasIncarnationTauntSubAction;

typedef enum MajorasMaskSpinAttackSubAction {
    /* 0 */ MAJORAS_MASK_SPIN_ATTACK_SUB_ACTION_WIND_UP,
    /* 1 */ MAJORAS_MASK_SPIN_ATTACK_SUB_ACTION_ATTACK,
    /* 2 */ MAJORAS_MASK_SPIN_ATTACK_SUB_ACTION_END,
} MajorasMaskSpinAttackSubAction;

typedef enum MajorasMaskFireBeamSubAction {
    /* 0 */ MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_CHARGE_UP,
    /* 1 */ MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_FIRE_EYE_BEAMS,
    /* 2 */ MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_GROW_FOCUS_LIGHT_ORB,
    /* 3 */ MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_BEAM_ACTIVE,
    /* 4 */ MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_BEAM_REFLECTED,
    /* 5 */ MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_END,
    /* 6 */ MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_MAX
} MajorasMaskFireBeamSubAction;

typedef enum ProjectileSubAction {
    /* 0 */ PROJECTILE_SUB_ACTION_SPAWN,
    /* 1 */ PROJECTILE_SUB_ACTION_FLY,
} ProjectileSubAction;

typedef enum RemainsMoveSubAction {
    /*  0 */ REMAINS_MOVE_SUB_ACTION_WAIT,
    /*  1 */ REMAINS_MOVE_SUB_ACTION_FLY,
    /*  2 */ REMAINS_MOVE_SUB_ACTION_DIE,
    /*  3 */ REMAINS_MOVE_SUB_ACTION_DEAD,
    /* 10 */ REMAINS_MOVE_SUB_ACTION_DAMAGED = 10,
    /* 20 */ REMAINS_MOVE_SUB_ACTION_DETACH_FROM_WALL = 20
} RemainsMoveSubAction;

typedef enum RemainsIntroSubAction {
    /* 0 */ REMAINS_INTRO_CS_SUB_ACTION_WAIT,
    /* 1 */ REMAINS_INTRO_CS_SUB_ACTION_FLY,
    /* 2 */ REMAINS_INTRO_CS_SUB_ACTION_ATTACH_WAIT,
    /* 3 */ REMAINS_INTRO_CS_SUB_ACTION_ATTACH_TO_WALL
} RemainsIntroSubAction;

typedef enum TopMoveSubAction {
    /* 0 */ TOP_MOVE_SUB_ACTION_AIRBORNE,
    /* 1 */ TOP_MOVE_SUB_ACTION_GROUNDED,
} TopMoveSubAction;

typedef enum MajorasMaskIntroCsState {
    /* 0 */ MAJORAS_MASK_INTRO_CS_STATE_WAITING_FOR_PLAYER_OR_DONE,
    /* 1 */ MAJORAS_MASK_INTRO_CS_STATE_LOOK_AT_PLAYER,
    /* 2 */ MAJORAS_MASK_INTRO_CS_STATE_REMAINS_ATTACH_TO_WALL,
    /* 3 */ MAJORAS_MASK_INTRO_CS_STATE_WAKE_UP,
    /* 4 */ MAJORAS_MASK_INTRO_CS_STATE_DETACH_FROM_WALL
} MajorasMaskIntroCsState;

typedef enum MajorasMaskDeathCsState {
    /* 0 */ MAJORAS_MASK_DEATH_CS_STATE_STARTED,
    /* 1 */ MAJORAS_MASK_DEATH_CS_STATE_PLAYING,
    /* 2 */ MAJORAS_MASK_DEATH_CS_STATE_SPAWN_INCARNATION,
} MajorasMaskDeathCsState;

typedef enum MajorasIncarnationIntroCsState {
    /* 0 */ MAJORAS_INCARNATION_INTRO_CS_STATE_STARTING_OR_DONE,
    /* 1 */ MAJORAS_INCARNATION_INTRO_CS_STATE_UPDATE_SUBCAM,
    /* 2 */ MAJORAS_INCARNATION_INTRO_CS_STATE_GROW_LEFT_LEG,
    /* 3 */ MAJORAS_INCARNATION_INTRO_CS_STATE_GROW_OTHER_LIMBS,
    /* 4 */ MAJORAS_INCARNATION_INTRO_CS_STATE_GROW_EYESTALK,
    /* 5 */ MAJORAS_INCARNATION_INTRO_CS_STATE_DANCE
} MajorasIncarnationIntroCsState;

typedef enum MajorasIncarnationDeathCsState {
    /*  0 */ MAJORAS_INCARNATION_DEATH_CS_STATE_STARTED,
    /*  1 */ MAJORAS_INCARNATION_DEATH_CS_STATE_PLAY_FINAL_HIT_ANIM,
    /*  2 */ MAJORAS_INCARNATION_DEATH_CS_STATE_PUMP_UP_RIGHT_ARM,
    /*  3 */ MAJORAS_INCARNATION_DEATH_CS_STATE_PUMP_UP_LEFT_ARM,
    /*  4 */ MAJORAS_INCARNATION_DEATH_CS_STATE_PUMP_UP_LEGS,
    /* 10 */ MAJORAS_INCARNATION_DEATH_CS_STATE_PLAY_DAMAGED_ANIM = 10
} MajorasIncarnationDeathCsState;

typedef enum MajorasWrathIntroCsState {
    /* 0 */ MAJORAS_WRATH_INTRO_CS_STATE_STARTING_OR_DONE,
    /* 1 */ MAJORAS_WRATH_INTRO_CS_STATE_PLAYING
} MajorasWrathIntroCsState;

typedef enum MajorasWrathDeathCsState {
    /* 0 */ MAJORAS_WRATH_DEATH_CS_STATE_STARTED,
    /* 1 */ MAJORAS_WRATH_DEATH_CS_STATE_UPDATE_SUBCAM,
    /* 2 */ MAJORAS_WRATH_DEATH_CS_STATE_SPIN_AROUND,
    /* 4 */ MAJORAS_WRATH_DEATH_CS_STATE_FLOAT = 4
} MajorasWrathDeathCsState;

typedef enum MajorasWrathShadowSize {
    /* 0 */ MAJORAS_WRATH_SHADOW_SIZE_MEDIUM,
    /* 1 */ MAJORAS_WRATH_SHADOW_SIZE_LARGE,
    /* 2 */ MAJORAS_WRATH_SHADOW_SIZE_EXTRA_LARGE,
    /* 3 */ MAJORAS_WRATH_SHADOW_SIZE_SMALL
} MajorasWrathShadowSize;

typedef enum MajoraDrawDmgEffState {
    /*  0 */ MAJORA_DRAW_DMGEFF_STATE_NONE,
    /*  1 */ MAJORA_DRAW_DMGEFF_STATE_FIRE_INIT,
    /*  2 */ MAJORA_DRAW_DMGEFF_STATE_FIRE_ACTIVE,
    /* 10 */ MAJORA_DRAW_DMGEFF_STATE_FROZEN_INIT = 10,
    /* 11 */ MAJORA_DRAW_DMGEFF_STATE_FROZEN_ACTIVE,
    /* 20 */ MAJORA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT = 20,
    /* 21 */ MAJORA_DRAW_DMGEFF_STATE_LIGHT_ORB_ACTIVE,
    /* 30 */ MAJORA_DRAW_DMGEFF_STATE_BLUE_LIGHT_ORB_INIT = 30,
    /* 40 */ MAJORA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_INIT = 40,
    /* 41 */ MAJORA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_ACTIVE
} MajoraDrawDmgEffState;

typedef enum BattleHandlerRemainsCsState {
    /* 0 */ BATTLE_HANDLER_REMAINS_CS_STATE_WAITING_FOR_MAJORAS_MASK,
    /* 1 */ BATTLE_HANDLER_REMAINS_CS_STATE_STARTED,
    /* 2 */ BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_ODOLWA,
    /* 3 */ BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_GYORG,
    /* 4 */ BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_GOHT,
    /* 5 */ BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_TWINMOLD,
    /* 6 */ BATTLE_HANDLER_REMAINS_CS_STATE_DONE
} BattleHandlerRemainsCsState;

typedef enum MajorasMaskTentacleState {
    /* 0 */ MAJORAS_MASK_TENTACLE_STATE_DEFAULT,
    /* 1 */ MAJORAS_MASK_TENTACLE_STATE_FIRING_BEAM,
    /* 2 */ MAJORAS_MASK_TENTACLE_STATE_DEATH
} MajorasMaskTentacleState;

typedef enum MajorasMaskSpinAttackRetargetState {
    // The mask targets a random position at least 100 units off the ground, making it hard to hit the player.
    /* 0 */ MAJORAS_MASK_SPIN_ATTACK_RETARGET_PASSIVE,

    // The mask targets a point 10 units above the player's current position for up to 20 frames.
    /* 1 */ MAJORAS_MASK_SPIN_ATTACK_RETARGET_ACTIVE,
} MajorasMaskSpinAttackRetargetState;

typedef enum MajorasMaskEyeTexture {
    /* 0 */ MAJORAS_MASK_EYE_NORMAL,
    /* 1 */ MAJORAS_MASK_EYE_DULL
} MajorasMaskEyeTexture;

typedef enum MajorasIncarnationDustSpawnPos {
    /* 0 */ MAJORAS_INCARNATION_DUST_SPAWN_POS_FEET,
    /* 1 */ MAJORAS_INCARNATION_DUST_SPAWN_POS_FOCUS
} MajorasIncarnationDustSpawnPos;

typedef enum MajorasIncarnationLimbPumpState {
    /* 0 */ MAJORAS_INCARNATION_LIMB_PUMP_STATE_NONE,
    /* 1 */ MAJORAS_INCARNATION_LIMB_PUMP_STATE_ARMS,
    /* 2 */ MAJORAS_INCARNATION_LIMB_PUMP_STATE_LEGS
} MajorasIncarnationLimbPumpState;

typedef enum MajorasWrathHand {
    /* 0 */ MAJORAS_WRATH_HAND_RIGHT,
    /* 1 */ MAJORAS_WRATH_HAND_LEFT
} MajorasWrathHand;

void Boss07_Init(Actor* thisx, PlayState* play2);
void Boss07_Destroy(Actor* thisx, PlayState* play2);

void Boss07_Wrath_Update(Actor* thisx, PlayState* play2);
void Boss07_IncarnationAfterimage_Update(Actor* thisx, PlayState* play2);
void Boss07_Incarnation_Update(Actor* thisx, PlayState* play2);
void Boss07_Mask_Update(Actor* thisx, PlayState* play2);
void Boss07_Projectile_Update(Actor* thisx, PlayState* play2);
void Boss07_Remains_Update(Actor* thisx, PlayState* play2);
void Boss07_Top_Update(Actor* thisx, PlayState* play2);
void Boss07_BattleHandler_Update(Actor* thisx, PlayState* play2);

void Boss07_Wrath_Draw(Actor* thisx, PlayState* play2);
void Boss07_IncarnationAfterimage_Draw(Actor* thisx, PlayState* play2);
void Boss07_Incarnation_Draw(Actor* thisx, PlayState* play2);
void Boss07_Mask_Draw(Actor* thisx, PlayState* play2);
void Boss07_Projectile_Draw(Actor* thisx, PlayState* play2);
void Boss07_Remains_Draw(Actor* thisx, PlayState* play2);
void Boss07_Top_Draw(Actor* thisx, PlayState* play2);
void Boss07_BattleHandler_Draw(Actor* thisx, PlayState* play2);

void Boss07_Reset(void);

void Boss07_Wrath_SetupIntroCutscene(Boss07* this, PlayState* play);
void Boss07_Wrath_IntroCutscene(Boss07* this, PlayState* play);
void Boss07_Wrath_DeathCutscene(Boss07* this, PlayState* play);
void Boss07_Wrath_SetupIdle(Boss07* this, PlayState* play, s16 idleTimer);
void Boss07_Wrath_Idle(Boss07* this, PlayState* play);
void Boss07_Wrath_SetupJump(Boss07* this, PlayState* play);
void Boss07_Wrath_StartJump(Boss07* this, PlayState* play);
void Boss07_Wrath_Jump(Boss07* this, PlayState* play);
void Boss07_Wrath_SetupFlip(Boss07* this, PlayState* play);
void Boss07_Wrath_Flip(Boss07* this, PlayState* play);
void Boss07_Wrath_Sidestep(Boss07* this, PlayState* play);
void Boss07_Wrath_SetupAttack(Boss07* this, PlayState* play);
void Boss07_Wrath_Attack(Boss07* this, PlayState* play);
void Boss07_Wrath_SetupTryGrab(Boss07* this, PlayState* play);
void Boss07_Wrath_TryGrab(Boss07* this, PlayState* play);
void Boss07_Wrath_ThrowPlayer(Boss07* this, PlayState* play);
void Boss07_Wrath_SetupShock(Boss07* this, PlayState* play);
void Boss07_Wrath_ShockWhip(Boss07* this, PlayState* play);
void Boss07_Wrath_ShockStunned(Boss07* this, PlayState* play);
void Boss07_Wrath_SetupThrowTop(Boss07* this, PlayState* play);
void Boss07_Wrath_ThrowTop(Boss07* this, PlayState* play);
void Boss07_Wrath_Stunned(Boss07* this, PlayState* play);
void Boss07_Wrath_Damaged(Boss07* this, PlayState* play);
void Boss07_Wrath_GenShadowTex(u8* tex, Boss07* this, PlayState* play);
void Boss07_Wrath_DrawShadowTex(u8* tex, Boss07* this, PlayState* play);

void Boss07_Incarnation_SetupIntroCutscene(Boss07* this, PlayState* play);
void Boss07_Incarnation_IntroCutscene(Boss07* this, PlayState* play);
void Boss07_Incarnation_SetupTaunt(Boss07* this, PlayState* play);
void Boss07_Incarnation_Taunt(Boss07* this, PlayState* play);
void Boss07_Incarnation_Stunned(Boss07* this, PlayState* play);
void Boss07_Incarnation_Damaged(Boss07* this, PlayState* play);
void Boss07_Incarnation_SetupRun(Boss07* this, PlayState* play);
void Boss07_Incarnation_Run(Boss07* this, PlayState* play);
void Boss07_Incarnation_SetupAttack(Boss07* this, PlayState* play);
void Boss07_Incarnation_Attack(Boss07* this, PlayState* play);
void Boss07_Incarnation_SetupSquattingDance(Boss07* this, PlayState* play);
void Boss07_Incarnation_SquattingDance(Boss07* this, PlayState* play);
void Boss07_Incarnation_SetupMoonwalk(Boss07* this, PlayState* play);
void Boss07_Incarnation_Moonwalk(Boss07* this, PlayState* play);
void Boss07_Incarnation_SetupPirouette(Boss07* this, PlayState* play);
void Boss07_Incarnation_Pirouette(Boss07* this, PlayState* play);
void Boss07_Incarnation_DeathCutscene(Boss07* this, PlayState* play);

void Boss07_Mask_SetupIdle(Boss07* this, PlayState* play);
void Boss07_Mask_Idle(Boss07* this, PlayState* play);
void Boss07_Mask_SetupSpinAttack(Boss07* this, PlayState* play);
void Boss07_Mask_SpinAttack(Boss07* this, PlayState* play);
void Boss07_Mask_Stunned(Boss07* this, PlayState* play);
void Boss07_Mask_Damaged(Boss07* this, PlayState* play);
void Boss07_Mask_SetupFireBeam(Boss07* this, PlayState* play);
void Boss07_Mask_FireBeam(Boss07* this, PlayState* play);
void Boss07_Mask_SetupIntroCutscene(Boss07* this, PlayState* play);
void Boss07_Mask_IntroCutscene(Boss07* this, PlayState* play);
void Boss07_Mask_SetupDeathCutscene(Boss07* this, PlayState* play);
void Boss07_Mask_DeathCutscene(Boss07* this, PlayState* play);

void Boss07_Remains_SetupIntroCutscene(Boss07* this, PlayState* play);
void Boss07_Remains_IntroCutscene(Boss07* this, PlayState* play);
void Boss07_Remains_SetupMove(Boss07* this, PlayState* play);
void Boss07_Remains_Move(Boss07* this, PlayState* play);
void Boss07_Remains_SetupStunned(Boss07* this, PlayState* play);
void Boss07_Remains_Stunned(Boss07* this, PlayState* play);

void Boss07_Top_SetupThrown(Boss07* this, PlayState* play);
void Boss07_Top_Thrown(Boss07* this, PlayState* play);
void Boss07_Top_SetupMove(Boss07* this, PlayState* play);
void Boss07_Top_Move(Boss07* this, PlayState* play);

void Boss07_BattleHandler_UpdateEffects(PlayState* play);
void Boss07_BattleHandler_DrawEffects(PlayState* play);

static s16 sProjectileEnvColors[4][3] = {
    { 255, 255, 100 },
    { 255, 100, 100 },
    { 100, 255, 100 },
    { 100, 100, 255 },
};

static s16 sProjectilePrimColors[4][3] = {
    { 255, 255, 255 },
    { 255, 255, 255 },
    { 255, 255, 255 },
    { 255, 255, 255 },
};

static DamageTable sMajorasMaskDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* Deku Stick     */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Horse trample  */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(2, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Zora boomerang */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Normal arrow   */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* Goron punch    */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Sword          */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Goron pound    */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Fire arrow     */ DMG_ENTRY(2, MAJORAS_MASK_DMGEFF_FIRE_ARROW),
    /* Ice arrow      */ DMG_ENTRY(2, MAJORAS_MASK_DMGEFF_ICE_ARROW),
    /* Light arrow    */ DMG_ENTRY(2, MAJORAS_MASK_DMGEFF_LIGHT_ARROW),
    /* Goron spikes   */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Deku spin      */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Deku bubble    */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Deku launch    */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* Zora barrier   */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* Normal shield  */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Zora punch     */ DMG_ENTRY(1, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Spin attack    */ DMG_ENTRY(2, MAJORAS_MASK_DMGEFF_DAMAGE),
    /* Sword beam     */ DMG_ENTRY(2, MAJORAS_MASK_DMGEFF_SWORD_BEAM),
    /* Normal Roll    */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, MAJORAS_MASK_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(2, MAJORAS_MASK_DMGEFF_DAMAGE),
};

static DamageTable sMajorasIncarnationDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* Deku Stick     */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_EXPLOSIVE),
    /* Zora boomerang */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* Goron punch    */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_ANIM_FRAME_CHECK),
    /* Sword          */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_ANIM_FRAME_CHECK),
    /* Goron pound    */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, MAJORAS_INCARNATION_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, MAJORAS_INCARNATION_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, MAJORAS_INCARNATION_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* Zora barrier   */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_ELECTRIC_SPARKS),
    /* Normal shield  */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, MAJORAS_INCARNATION_DMGEFF_SPIN_ATTACK),
    /* Sword beam     */ DMG_ENTRY(2, MAJORAS_INCARNATION_DMGEFF_BLUE_LIGHT_ORB),
    /* Normal Roll    */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, MAJORAS_INCARNATION_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(4, MAJORAS_INCARNATION_DMGEFF_EXPLOSIVE),
};

static DamageTable sMajorasWrathDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* Deku Stick     */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_ANIM_FRAME_CHECK),
    /* Horse trample  */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_EXPLOSIVE),
    /* Zora boomerang */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_STUN_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_STUN_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* Goron punch    */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_ANIM_FRAME_CHECK),
    /* Sword          */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_ANIM_FRAME_CHECK),
    /* Goron pound    */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_STUN_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, MAJORAS_WRATH_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, MAJORAS_WRATH_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, MAJORAS_WRATH_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_ANIM_FRAME_CHECK),
    /* Deku spin      */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_ANIM_FRAME_CHECK),
    /* Deku bubble    */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_STUN_NONE),
    /* Deku launch    */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_ANIM_FRAME_CHECK),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* Zora barrier   */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_ELECTRIC_SPARKS),
    /* Normal shield  */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_STUN_NONE),
    /* Zora punch     */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_ANIM_FRAME_CHECK),
    /* Spin attack    */ DMG_ENTRY(1, MAJORAS_WRATH_DMGEFF_DAMAGE_NONE),
    /* Sword beam     */ DMG_ENTRY(2, MAJORAS_WRATH_DMGEFF_BLUE_LIGHT_ORB),
    /* Normal Roll    */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, MAJORAS_WRATH_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(2, MAJORAS_WRATH_DMGEFF_EXPLOSIVE),
};

static DamageTable sRemainsDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* Deku Stick     */ DMG_ENTRY(1, REMAINS_DMGEFF_DAMAGE),
    /* Horse trample  */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(2, REMAINS_DMGEFF_OTHER),
    /* Zora boomerang */ DMG_ENTRY(1, REMAINS_DMGEFF_DAMAGE),
    /* Normal arrow   */ DMG_ENTRY(1, REMAINS_DMGEFF_DAMAGE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* Goron punch    */ DMG_ENTRY(1, REMAINS_DMGEFF_OTHER),
    /* Sword          */ DMG_ENTRY(1, REMAINS_DMGEFF_OTHER),
    /* Goron pound    */ DMG_ENTRY(1, REMAINS_DMGEFF_DAMAGE),
    /* Fire arrow     */ DMG_ENTRY(2, REMAINS_DMGEFF_FIRE_ARROW),
    /* Ice arrow      */ DMG_ENTRY(2, REMAINS_DMGEFF_ICE_ARROW),
    /* Light arrow    */ DMG_ENTRY(2, REMAINS_DMGEFF_LIGHT_ARROW),
    /* Goron spikes   */ DMG_ENTRY(1, REMAINS_DMGEFF_DAMAGE),
    /* Deku spin      */ DMG_ENTRY(1, REMAINS_DMGEFF_DAMAGE),
    /* Deku bubble    */ DMG_ENTRY(1, REMAINS_DMGEFF_DAMAGE),
    /* Deku launch    */ DMG_ENTRY(1, REMAINS_DMGEFF_DAMAGE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* Zora barrier   */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* Normal shield  */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, REMAINS_DMGEFF_DAMAGE),
    /* Zora punch     */ DMG_ENTRY(1, REMAINS_DMGEFF_DAMAGE),
    /* Spin attack    */ DMG_ENTRY(1, REMAINS_DMGEFF_SPIN_ATTACK),
    /* Sword beam     */ DMG_ENTRY(2, REMAINS_DMGEFF_SWORD_BEAM),
    /* Normal Roll    */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, REMAINS_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(2, REMAINS_DMGEFF_OTHER),
};

static DamageTable sTopDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, TOP_DMGEFF_IMMUNE),
    /* Deku Stick     */ DMG_ENTRY(1, TOP_DMGEFF_PUSH_BACK_PLAYER),
    /* Horse trample  */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_E),
    /* Explosives     */ DMG_ENTRY(1, TOP_DMGEFF_BOUNCE_BACK_FROM_DAMAGE),
    /* Zora boomerang */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_F),
    /* Normal arrow   */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_F),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, TOP_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_E),
    /* Goron punch    */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_E),
    /* Sword          */ DMG_ENTRY(1, TOP_DMGEFF_PUSH_BACK_PLAYER),
    /* Goron pound    */ DMG_ENTRY(0, TOP_DMGEFF_IMMUNE),
    /* Fire arrow     */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_E),
    /* Ice arrow      */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_E),
    /* Light arrow    */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_E),
    /* Goron spikes   */ DMG_ENTRY(1, TOP_DMGEFF_KNOCKED_BACK_FROM_PLAYER),
    /* Deku spin      */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_F),
    /* Deku bubble    */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_F),
    /* Deku launch    */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_F),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, TOP_DMGEFF_IMMUNE),
    /* Zora barrier   */ DMG_ENTRY(0, TOP_DMGEFF_IMMUNE),
    /* Normal shield  */ DMG_ENTRY(1, TOP_DMGEFF_PUSH_BACK_PLAYER),
    /* Light ray      */ DMG_ENTRY(1, TOP_DMGEFF_PUSH_BACK_PLAYER),
    /* Thrown object  */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_E),
    /* Zora punch     */ DMG_ENTRY(1, TOP_DMGEFF_NO_REACTION_E),
    /* Spin attack    */ DMG_ENTRY(1, TOP_DMGEFF_REVERSE_DIRECTION),
    /* Sword beam     */ DMG_ENTRY(1, TOP_DMGEFF_REVERSE_DIRECTION),
    /* Normal Roll    */ DMG_ENTRY(0, TOP_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, TOP_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, TOP_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, TOP_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, TOP_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(2, TOP_DMGEFF_BOUNCE_BACK_FROM_DAMAGE),
};

ActorProfile Boss_07_Profile = {
    /**/ ACTOR_BOSS_07,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BOSS07,
    /**/ sizeof(Boss07),
    /**/ Boss07_Init,
    /**/ Boss07_Destroy,
    /**/ Boss07_Wrath_Update,
    /**/ Boss07_Wrath_Draw,
    /**/ Boss07_Reset,
};

// The limbs referenced here are not used. The spheres are positioned manually by Boss07_Wrath_PostLimbDraw.
static ColliderJntSphElementInit sWrathBodyColliderJntSphElementsInit[MAJORAS_WRATH_COLLIDER_BODYPART_MAX] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_NONE, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_ROOT, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_ROOT, { { 0, 0, 0 }, 25 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 150 },
    },
};

static ColliderJntSphInit sWrathBodyColliderJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sWrathBodyColliderJntSphElementsInit),
    sWrathBodyColliderJntSphElementsInit,
};

static ColliderCylinderInit sWrathCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0xF7CFFFFF, 0x00, 0x10 },
        { 0xF7CEFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 80, 200, 0, { 0, 0, 0 } },
};

// The limbs referenced here are not used. The spheres are positioned manually by Boss07_Wrath_PostLimbDraw.
static ColliderJntSphElementInit sWrathKickColliderJntSphElementsInit[MAJORAS_WARTH_KICK_COLLIDER_MAX] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_WRATH_LIMB_NONE, { { 0, 0, 0 }, 36 }, 200 },
    },
};

static ColliderJntSphInit sWrathKickColliderJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sWrathKickColliderJntSphElementsInit),
    sWrathKickColliderJntSphElementsInit,
};

static ColliderQuadInit sMaskFrontQuadInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0xF7CFFFFF, 0x04, 0x00 },
        { 0xF7CEFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderQuadInit sMaskBackQuadInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0xF7CEFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

// The limbs referenced here are not used. The spheres are positioned manually by Boss07_Incarnation_PostLimbDraw.
static ColliderJntSphElementInit sIncarnationBodyJntSphElementsInit[MAJORAS_INCARNATION_COLLIDER_BODYPART_MAX] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_NONE, { { 0, 0, 0 }, 25 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_ROOT, { { 0, 0, 0 }, 40 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_ROOT, { { 0, 0, 0 }, 0 }, 0 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { MAJORAS_INCARNATION_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 150 },
    },
};

static ColliderJntSphInit sIncarnationBodyColliderJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sIncarnationBodyJntSphElementsInit),
    sIncarnationBodyJntSphElementsInit,
};

static ColliderCylinderInit sProjectileCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0x00300000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 15, 30, -15, { 0, 0, 0 } },
};

static ColliderCylinderInit sRemainsCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 50, 100, -50, { 0, 0, 0 } },
};

static ColliderCylinderInit sTopCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0xF7FFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 40, 20, 15, { 0, 0, 0 } },
};

Vec3f sMajoraSfxPos;

static u8 sHeartbeatTimer;
static s32 sWhipLength;

Boss07* sMajorasWrath;
Boss07* sMajoraBattleHandler;
Boss07* sMajorasMask;
Boss07* sMajoraRemains[MAJORA_REMAINS_TYPE_MAX];

static u8 sKillAllProjectiles;
static u8 sMusicStartTimer;

MajoraEffect sMajoraEffects[MAJORA_EFFECT_COUNT];

s32 sMajoraRandSeed1;
s32 sMajoraRandSeed2;
s32 sMajoraRandSeed3;

void Boss07_Remains_PlayDamageSfx(Boss07* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_FOLLOWERS_DAMAGE);
}

void Boss07_InitRand(s32 seedInit1, s32 seedInit2, s32 seedInit3) {
    sMajoraRandSeed1 = seedInit1;
    sMajoraRandSeed2 = seedInit2;
    sMajoraRandSeed3 = seedInit3;
}

f32 Boss07_RandZeroOne(void) {
    // Wichmann-Hill algorithm
    f32 randFloat;

    sMajoraRandSeed1 = (sMajoraRandSeed1 * 171) % 30269;
    sMajoraRandSeed2 = (sMajoraRandSeed2 * 172) % 30307;
    sMajoraRandSeed3 = (sMajoraRandSeed3 * 170) % 30323;

    randFloat = (sMajoraRandSeed1 / 30269.0f) + (sMajoraRandSeed2 / 30307.0f) + (sMajoraRandSeed3 / 30323.0f);

    while (randFloat >= 1.0f) {
        randFloat -= 1.0f;
    }

    return fabsf(randFloat);
}

void Boss07_SpawnFlameEffect(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale) {
    s32 i;
    MajoraEffect* effect = (MajoraEffect*)play->specialEffects;

    for (i = 0; i < MAJORA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == MAJORA_EFFECT_NONE) {
            effect->type = MAJORA_EFFECT_FLAME;
            effect->pos = *pos;
            effect->velocity = *velocity;
            effect->accel = *accel;
            effect->scale = scale / 1000.0f;
            effect->isFadingAway = false;
            effect->alpha = 0;
            effect->texScroll = Rand_ZeroFloat(1000.0f);
            break;
        }
    }
}

/**
 * Manually sets the position of a sphere collider to a specific position.
 */
void Boss07_SetColliderSphere(s32 index, ColliderJntSph* collider, Vec3f* pos) {
    collider->elements[index].dim.worldSphere.center.x = pos->x;
    collider->elements[index].dim.worldSphere.center.y = pos->y;
    collider->elements[index].dim.worldSphere.center.z = pos->z;
    collider->elements[index].dim.worldSphere.radius =
        collider->elements[index].dim.modelSphere.radius * collider->elements[index].dim.scale;
}

/**
 * Returns true if this actor's model is rotated such that it is looking at the player *and* if the player's model is
 * rotated such that they are looking at this actor.
 */
s32 Boss07_ArePlayerAndActorFacing(Boss07* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y)) < 0x3000) &&
        (ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, BINANG_ROT180(player->actor.shape.rot.y))) < 0x3000)) {
        return true;
    }

    return false;
}

/**
 * Can be called repeatedly to gradually reduce the actor's speed to zero. If the actor is touching a wall or ceiling,
 * though, it will immediately set the speed to zero.
 */
void Boss07_SmoothStop(Boss07* this, f32 maxStep) {
    Math_ApproachZeroF(&this->actor.speed, 1.0f, maxStep);

    if (this->actor.bgCheckFlags & (BGCHECKFLAG_WALL | BGCHECKFLAG_CEILING)) {
        this->actor.speed = 0.0f;
    }
}

void Boss07_RandXZ(Vec3f* dst, f32 length) {
    Matrix_RotateYF(Rand_ZeroFloat(2 * M_PIf), MTXMODE_NEW);
    Matrix_MultVecZ(length, dst);
}

void Boss07_Incarnation_SpawnDust(Boss07* this, PlayState* play, u8 dustSpawnFrameMask, u8 spawnPos) {
    static Color_RGBA8 sDustPrimColor = { 60, 50, 20, 255 };
    static Color_RGBA8 sDustEnvColor = { 40, 30, 30, 255 };
    u8 i;

    if (!(this->frameCounter & dustSpawnFrameMask) && ((dustSpawnFrameMask == 0) || (this->actor.speed > 1.0f))) {
        for (i = 0; i < MAJORAS_INCARNATION_FOOT_MAX; i++) {
            Vec3f pos;
            Vec3f velocity;
            Vec3f accel;

            velocity.x = Rand_CenteredFloat(5.0f);
            velocity.y = Rand_ZeroFloat(2.0f) + 1.0f;
            velocity.z = Rand_CenteredFloat(5.0f);

            accel.x = accel.z = 0.0f;
            accel.y = -0.1f;

            pos.y = Rand_ZeroFloat(10.0f) + 3.0f;

            if ((u32)spawnPos != MAJORAS_INCARNATION_DUST_SPAWN_POS_FEET) {
                pos.x = this->actor.focus.pos.x + Rand_CenteredFloat(150.0f);
                pos.z = this->actor.focus.pos.z + Rand_CenteredFloat(150.0f);
            } else {
                pos.z = this->incarnationFeetPos[i].z + Rand_CenteredFloat(20.0f);
                pos.x = this->incarnationFeetPos[i].x + Rand_CenteredFloat(20.0f);
            }

            func_800B0EB0(play, &pos, &velocity, &accel, &sDustPrimColor, &sDustEnvColor,
                          Rand_ZeroFloat(150.0f) + 350.0f, 10, Rand_ZeroFloat(5.0f) + 14.0f);
        }
    }
}

void Boss07_MovePlayerFromCenter(PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (SQXZ(player->actor.world.pos) < SQ(80.0f)) {
        player->actor.world.pos.z = 90.0f;
    }
}

void Boss07_Wrath_SpawnDustAtPos(PlayState* play, Vec3f* spawnPos, u8 count) {
    static Color_RGBA8 sDustPrimColor = { 60, 50, 20, 255 };
    static Color_RGBA8 sDustEnvColor = { 40, 30, 30, 255 };
    u8 i;

    for (i = 0; i < count; i++) {
        Vec3f pos;
        Vec3f velocity;
        Vec3f accel;

        velocity.x = Rand_CenteredFloat(3.0f);
        velocity.y = Rand_ZeroFloat(2.0f) + 1.0f;
        velocity.z = Rand_CenteredFloat(3.0f);

        accel.x = accel.z = 0.0f;
        accel.y = -0.1f;

        pos.x = spawnPos->x + Rand_CenteredFloat(30.0f);
        pos.y = spawnPos->y + 15.0f + Rand_CenteredFloat(30.0f);
        pos.z = spawnPos->z + Rand_CenteredFloat(30.0f);

        func_800B0EB0(play, &pos, &velocity, &accel, &sDustPrimColor, &sDustEnvColor, Rand_ZeroFloat(50.0f) + 100.0f,
                      10, Rand_ZeroFloat(5.0f) + 14.0f);
    }
}

void Boss07_Wrath_ChooseJump(Boss07* this, PlayState* play, u8 canCancelCurrentJump) {
    Player* player = GET_PLAYER(play);

    if ((this->damagedTimer == 0) &&
        (((this->actionFunc != Boss07_Wrath_Flip) && (this->actionFunc != Boss07_Wrath_StartJump) &&
          (this->actionFunc != Boss07_Wrath_Jump)) ||
         canCancelCurrentJump)) {
        if (Rand_ZeroOne() < 0.5f) {
            Boss07_Wrath_SetupFlip(this, play);
        } else {
            Boss07_Wrath_SetupJump(this, play);
        }

        this->disableCollisionTimer = 10;
        this->whipWrapEndOffset = 0;

        if (&this->actor == player->actor.parent) {
            player->av2.actionVar2 = 101;
            player->actor.parent = NULL;
            player->csAction = PLAYER_CSACTION_NONE;
        }
    }
}

void Boss07_Wrath_JumpAwayFromExplosive(Boss07* this, PlayState* play) {
    Actor* explosive;

    for (explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first; explosive != NULL;
         explosive = explosive->next) {
        f32 dx = explosive->world.pos.x - this->actor.world.pos.x;
        f32 dy = explosive->world.pos.y - this->actor.world.pos.y;
        f32 dz = explosive->world.pos.z - this->actor.world.pos.z;

        if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 200.0f) {
            Boss07_Wrath_ChooseJump(this, play, false);
            break;
        }
    }
}

void Boss07_Wrath_BombWhip(Vec3f* bombPos, Vec3f* pos, Vec3f* velocity) {
    s32 i;
    f32 push;
    f32 dx;
    f32 dy;
    f32 dz;
    Vec3f impulse;

    impulse.x = 0.0f;

    for (i = 0; i < sWhipLength; i++, pos++, velocity++) {
        dx = pos->x - bombPos->x;
        dy = pos->y - bombPos->y;
        dz = pos->z - bombPos->z;

        if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 300.0f) {
            push = 300.0f - sqrtf(SQ(dx) + SQ(dy) + SQ(dz));
            push = CLAMP_MAX(push, 200.0f);
            impulse.y = push;
            impulse.z = push;

            Matrix_RotateYF(Math_Atan2F_XY(dz, dx), MTXMODE_NEW);
            Matrix_MultVec3f(&impulse, velocity);
        }
    }
}

void Boss07_Wrath_CheckBombWhips(Boss07* this, PlayState* play) {
    Actor* explosive;

    for (explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first; explosive != NULL;
         explosive = explosive->next) {
        if (explosive->params == BOMB_TYPE_EXPLOSION) {
            Boss07_Wrath_BombWhip(&explosive->world.pos, this->rightWhip.pos, this->rightWhip.velocity);
            Boss07_Wrath_BombWhip(&explosive->world.pos, this->leftWhip.pos, this->leftWhip.velocity);
        }
    }
}

static Vec3f sRemainsStartTargetOffset[MAJORA_REMAINS_TYPE_MAX] = {
    { 70.0f, 70.0f, -70.0f },  // MAJORA_REMAINS_TYPE_ODOLWA
    { 24.0f, 88.0f, -70.0f },  // MAJORA_REMAINS_TYPE_GYORG
    { -24.0f, 88.0f, -70.0f }, // MAJORA_REMAINS_TYPE_GOHT
    { -70.0f, 70.0f, -70.0f }, // MAJORA_REMAINS_TYPE_TWINMOLD
};

// y value here is y-rotation, not position
static Vec3s sRemainsEndTarget[MAJORA_REMAINS_TYPE_MAX] = {
    { 712, 0xD500, -416 },  // MAJORA_REMAINS_TYPE_ODOLWA
    { -712, 0x2B00, -420 }, // MAJORA_REMAINS_TYPE_GYORG
    { 702, 0xAB00, 415 },   // MAJORA_REMAINS_TYPE_GOHT
    { -712, 0x5500, 416 },  // MAJORA_REMAINS_TYPE_TWINMOLD
};

// #region 2S2H [Port] Masks for CPU modified texture handling
static u8 sWrathTexMask16by16[16 * 16]; // gMajorasWrathEarTex
static u8 sWrathTexMask16by32[16 * 32]; // gMajoraStripesTex
static u8 sWrathTexMask32by32[32 * 32]; // gMajorasWrathMouthTex, gMajoraBloodshotEyeTex
// gMajorasWrathEyeTex, gMajorasMaskWithNormalEyesTex, gMajoraVeinsTex, gMajoraHandTex, gMajoraBodyTex
static u8 sWrathTexMask32by64[32 * 64];
// #endregion

void Boss07_Init(Actor* thisx, PlayState* play2) {
    static s16 sMajoraRemainsParams[MAJORA_REMAINS_TYPE_MAX] = {
        MAJORA_PARAMS(MAJORA_TYPE_REMAINS + MAJORA_REMAINS_TYPE_ODOLWA),
        MAJORA_PARAMS(MAJORA_TYPE_REMAINS + MAJORA_REMAINS_TYPE_GYORG),
        MAJORA_PARAMS(MAJORA_TYPE_REMAINS + MAJORA_REMAINS_TYPE_GOHT),
        MAJORA_PARAMS(MAJORA_TYPE_REMAINS + MAJORA_REMAINS_TYPE_TWINMOLD),
    };
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;
    s32 i;

    if (MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_BATTLE_HANDLER) {
        this->actor.update = Boss07_BattleHandler_Update;
        this->actor.draw = Boss07_BattleHandler_Draw;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        sMajoraBattleHandler = this;
        sKillAllProjectiles = false;
        play->envCtx.lightSettingOverride = 0;
        play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_FULL_CONTROL;
        return;
    }

    if (MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_BATTLE_INIT) {
        this->actor.params = MAJORA_TYPE_MASK;
        Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_07, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, MAJORA_PARAMS(MAJORA_TYPE_BATTLE_HANDLER));
        play->specialEffects = (void*)sMajoraEffects;

        for (i = 0; i < MAJORA_EFFECT_COUNT; i++) {
            sMajoraEffects[i].type = MAJORA_EFFECT_NONE;
        }

        for (i = 0; i < MAJORA_REMAINS_TYPE_MAX; i++) {
            sMajoraRemains[i] = NULL;
        }
    }

    this->actor.attentionRangeType = ATTENTION_RANGE_5;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.gravity = -2.5f;

    if (MAJORA_GET_TYPE(thisx) >= MAJORA_TYPE_REMAINS) {
        this->actor.update = Boss07_Remains_Update;
        this->actor.draw = Boss07_Remains_Draw;

        sMajoraRemains[MAJORA_GET_TYPE(thisx) - MAJORA_TYPE_REMAINS] = this;

        if (CHECK_EVENTINF(EVENTINF_INTRO_CS_WATCHED_MAJORA)) {
            Actor_SetScale(&this->actor, 0.03f);
            this->actor.world.pos.x = sRemainsEndTarget[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].x;
            this->actor.world.pos.y = 370.0f;
            this->actor.world.pos.z = sRemainsEndTarget[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].z;
            this->actor.shape.rot.y = sRemainsEndTarget[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].y;
            Boss07_Remains_SetupMove(this, play);
        } else {
            Boss07_Remains_SetupIntroCutscene(this, play);
        }

        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.colChkInfo.damageTable = &sRemainsDamageTable;
        return;
    }

    if (MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_TOP) {
        static EffectTireMarkInit sTopTireMarkInit = { 0, 40, { 0, 0, 15, 200 } };

        this->actor.update = Boss07_Top_Update;
        this->actor.draw = Boss07_Top_Draw;
        Boss07_Top_SetupThrown(this, play);
        this->actor.colChkInfo.damageTable = &sTopDamageTable;
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 9.0f + KREG(55));
        this->actor.shape.shadowAlpha = 180;
        Collider_InitAndSetCylinder(play, &this->generalCollider, &this->actor, &sTopCylinderInit);
        Effect_Add(play, &this->effectIndex, EFFECT_TIRE_MARK, 0, 0, &sTopTireMarkInit);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        return;
    }

    if ((MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_PROJECTILE_REMAINS) ||
        (MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_PROJECTILE_INCARNATION)) {
        this->actor.update = Boss07_Projectile_Update;
        this->actor.draw = Boss07_Projectile_Draw;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        Collider_InitAndSetCylinder(play, &this->generalCollider, &this->actor, &sProjectileCylinderInit);
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
        this->projectileColorIndex = Rand_ZeroFloat(ARRAY_COUNT(sProjectileEnvColors) - 0.01f);
        return;
    }

    if ((MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_MASK) ||
        (MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_MASK_UNK)) {
        this->actor.colChkInfo.damageTable = &sMajorasMaskDamageTable;
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 15.0f);
        SkelAnime_Init(play, &this->skelAnime, &gMajorasMaskSkel, &gMajorasMaskFloatingAnim, this->jointTable,
                       this->morphTable, MAJORAS_MASK_LIMB_MAX);

        if (MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_MASK) {
            static EffectTireMarkInit sMaskTireMarkInit = { 0, 40, { 0, 0, 15, 200 } };

            this->actor.update = Boss07_Mask_Update;
            this->actor.draw = Boss07_Mask_Draw;
            Effect_Add(play, &this->effectIndex, EFFECT_TIRE_MARK, 0, 0, &sMaskTireMarkInit);
            sMajorasMask = this;

            if (CHECK_EVENTINF(EVENTINF_INTRO_CS_WATCHED_MAJORA)) {
                this->actor.world.pos.x = 0.0f;
                this->actor.world.pos.y = sREG(17) + 277.0f;
                this->actor.world.pos.z = -922.5f;
                Boss07_Mask_SetupIdle(this, play);
                this->targetPos.x = 0.0f;
                this->targetPos.y = 200.0f;
                this->targetPos.z = 0.0f;
                this->speedToTarget = 0.0f;
                this->timers[0] = 50;
                this->timers[2] = 200;
                this->bgCheckTimer = 50;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                sMusicStartTimer = 20;
            } else {
                Boss07_Mask_SetupIntroCutscene(this, play);
            }

            Collider_InitAndSetQuad(play, &this->maskFrontCollider, &this->actor, &sMaskFrontQuadInit);
            Collider_InitAndSetQuad(play, &this->maskBackCollider, &this->actor, &sMaskBackQuadInit);
            this->actor.colChkInfo.health = 14;

            for (i = 0; i < MAJORA_REMAINS_TYPE_MAX; i++) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_07, 0.0f, 0.0f, 0.0f, 0, 0, 0, sMajoraRemainsParams[i]);
            }
        }
        return;
    }

    if ((MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_INCARNATION) ||
        (MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_INCARNATION_AFTERIMAGE)) {
        Actor_SetScale(&this->actor, 15.0f * 0.001f);
        SkelAnime_InitFlex(play, &this->skelAnime, &gMajorasIncarnationSkel, &gMajorasIncarnationTauntDance1Anim,
                           this->jointTable, this->morphTable, MAJORAS_INCARNATION_LIMB_MAX);

        if (MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_INCARNATION_AFTERIMAGE) {
            this->timers[0] = this->actor.world.rot.z;
            this->actor.world.rot.z = 0;
            this->actor.update = Boss07_IncarnationAfterimage_Update;
            this->actor.draw = Boss07_IncarnationAfterimage_Draw;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        } else {
            this->actor.colChkInfo.damageTable = &sMajorasIncarnationDamageTable;
            this->actor.colChkInfo.health = 30;
            this->actor.update = Boss07_Incarnation_Update;
            this->actor.draw = Boss07_Incarnation_Draw;
            Collider_InitAndSetJntSph(play, &this->bodyCollider, &this->actor, &sIncarnationBodyColliderJntSphInit,
                                      this->bodyColliderElements);
            ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 80.0f);
            this->subCamId = this->actor.shape.rot.z;

            if (this->subCamId != SUB_CAM_ID_DONE) {
                Boss07_Incarnation_SetupIntroCutscene(this, play);
            } else {
                Boss07_Incarnation_SetupTaunt(this, play);
                this->lightSettingsMode = 1;

                for (i = 0; i < MAJORAS_INCARNATION_GROW_BODYPART_MAX; i++) {
                    this->incarnationIntroBodyPartsScale[i] = 1.0f;
                }

                play->envCtx.lightBlend = 0.0f;
            }

            this->incarnationArmScale = 1.0f;
            this->incarnationLegScale = 1.0f;
            this->incarnationMaskScaleY = 1.0f;
            this->incarnationMaskScaleX = 1.0f;
        }
        return;
    }

    sMajorasWrath = this;
    this->actor.colChkInfo.health = 40;
    this->actor.colChkInfo.damageTable = &sMajorasWrathDamageTable;
    Actor_SetScale(&this->actor, 0.01f);
    this->subCamId = this->actor.shape.rot.z;

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Boss07_Wrath_SetupIntroCutscene(this, play);
    } else {
        Boss07_Wrath_SetupIdle(this, play, 50);
        this->whipLengthScale = 1.0f;
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_MAJORAS_WRATH | SEQ_FLAG_ASYNC);
    }

    SkelAnime_InitFlex(play, &this->skelAnime, &gMajorasWrathSkel, &gMajorasWrathIdleAnim, this->jointTable,
                       this->morphTable, MAJORAS_WRATH_LIMB_MAX);
    Collider_InitAndSetJntSph(play, &this->bodyCollider, &this->actor, &sWrathBodyColliderJntSphInit,
                              this->bodyColliderElements);
    Collider_InitAndSetJntSph(play, &this->kickCollider, &this->actor, &sWrathKickColliderJntSphInit,
                              this->kickColliderElements);
    Collider_InitAndSetCylinder(play, &this->unusedCollider, &this->actor, &sWrathCylinderInit);

    this->leftWhip.mobility = this->rightWhip.mobility = 0.7f;
    this->leftWhip.deceleration = this->rightWhip.deceleration = 2.0f;
    this->leftWhip.gravity = this->rightWhip.gravity = -15.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;

    sWhipLength = MAJORA_WHIP_LENGTH;
    this->incarnationArmScale = 1.0f;

    // #region 2S2H [Port] Setup and register masks as blended textures to handle CPU modified
    // textures in Wrath's death cutscene
    memset(sWrathTexMask16by16, 0, sizeof(sWrathTexMask16by16));
    memset(sWrathTexMask16by32, 0, sizeof(sWrathTexMask16by32));
    memset(sWrathTexMask32by32, 0, sizeof(sWrathTexMask32by32));
    memset(sWrathTexMask32by64, 0, sizeof(sWrathTexMask32by64));

    Gfx_RegisterBlendedTexture(gMajorasWrathEarTex, sWrathTexMask16by16, NULL);
    Gfx_RegisterBlendedTexture(gMajoraStripesTex, sWrathTexMask16by32, NULL);
    Gfx_RegisterBlendedTexture(gMajorasWrathMouthTex, sWrathTexMask32by32, NULL);
    Gfx_RegisterBlendedTexture(gMajoraBloodshotEyeTex, sWrathTexMask32by32, NULL);
    Gfx_RegisterBlendedTexture(gMajorasWrathEyeTex, sWrathTexMask32by64, NULL);
    Gfx_RegisterBlendedTexture(gMajorasMaskWithNormalEyesTex, sWrathTexMask32by64, NULL);
    Gfx_RegisterBlendedTexture(gMajoraVeinsTex, sWrathTexMask32by64, NULL);
    Gfx_RegisterBlendedTexture(gMajoraHandTex, sWrathTexMask32by64, NULL);
    Gfx_RegisterBlendedTexture(gMajoraBodyTex, sWrathTexMask32by64, NULL);
    // #endregion
}

void Boss07_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;

    switch (this->actor.params) {
        //! @bug this should be MAJORAS_MASK
        case MAJORA_TYPE_WRATH:
            Collider_DestroyQuad(play, &this->maskFrontCollider);
            Collider_DestroyQuad(play, &this->maskBackCollider);
            FALLTHROUGH;
        case MAJORA_TYPE_TOP:
            Effect_Destroy(play, this->effectIndex);
            break;

        default:
            break;
    }
}

void Boss07_Wrath_SetupIntroCutscene(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Wrath_IntroCutscene;
    Animation_MorphToLoop(&this->skelAnime, &gMajorasWrathHeavyBreathingAnim, 0.0f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->cutsceneHeadRot.x = 0x7F00;
    this->damagedTimer = 20;
    this->incarnationWrathTransitionScale = 0x1400;
}

typedef struct MajorasWrathIntroCutsceneCamPoints {
    /* 0x0 */ f32 eyeY;
    /* 0x4 */ f32 eyeZ;
    /* 0x8 */ f32 atY;
} MajorasWrathIntroCutsceneCamPoints; // size = 0xC

void Boss07_Wrath_IntroCutscene(Boss07* this, PlayState* play) {
    // sCamPoints reads playerForm in a different order than the enum
    static MajorasWrathIntroCutsceneCamPoints sCamPoints[PLAYER_FORM_MAX] = {
        { 40.0f, 400.0f, 110.0f },  // PLAYER_FORM_HUMAN
        { 80.0f, 450.0f, 110.0f },  // PLAYER_FORM_GORON
        { 100.0f, 400.0f, 110.0f }, // PLAYER_FORM_FIERCE_DEITY
        { 60.0f, 390.0f, 110.0f },  // PLAYER_FORM_ZORA
        { 30.0f, 430.0f, 110.0f },  // PLAYER_FORM_DEKU
    };

    this->damagedTimer = 20;
    SkelAnime_Update(&this->skelAnime);
    this->cutsceneTimer++;
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);

    switch (this->cutsceneState) {
        case MAJORAS_WRATH_INTRO_CS_STATE_STARTING_OR_DONE:
            this->cutsceneTimer = 0;
            this->cutsceneState = MAJORAS_WRATH_INTRO_CS_STATE_PLAYING;
            this->subCamAtNext.z = 0.f;
            this->subCamEyeNext.x = 0.0;
            this->subCamEyeNext.y = KREG(17) + 100.0f - 30.0f + 80.0f;
            this->subCamEyeNext.z = KREG(18) + 270.0f - 150.0f + 30.0f - 50.0f;
            this->subCamAtNext.x = 0.0f;
            this->subCamAtNext.y = 180.0f;
            FALLTHROUGH;
        case MAJORAS_WRATH_INTRO_CS_STATE_PLAYING:
            if (this->cutsceneTimer < 40) {
                sHeartbeatTimer = 3;
            }

            if (this->cutsceneTimer > 20) {
                if (this->cutsceneTimer == 21) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_GROW_HEAD_OLD);
                }

                Math_ApproachS(&this->cutsceneHeadRot.x, 0, 5, 0x1000);
                this->cutsceneHeadRot.y =
                    Math_SinS(this->cutsceneTimer * 0x1000) * this->incarnationWrathTransitionScale;
                this->cutsceneHeadRot.z =
                    Math_SinS(this->cutsceneTimer * 0xB00) * this->incarnationWrathTransitionScale * 0.5f;

                if (this->cutsceneTimer > 40) {
                    Math_ApproachZeroF(&this->incarnationWrathTransitionScale, 1.0f, 200.0f);
                }
            }

            if (this->cutsceneTimer > 60) {
                s32 camPointIndex = 0;
                Player* player = GET_PLAYER(play);

                if (player->transformation == PLAYER_FORM_GORON) {
                    camPointIndex = 1;
                } else if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
                    camPointIndex = 2;
                } else if (player->transformation == PLAYER_FORM_ZORA) {
                    camPointIndex = 3;
                } else if (player->transformation == PLAYER_FORM_DEKU) {
                    camPointIndex = 4;
                }

                Math_ApproachF(&this->subCamEyeNext.y, sCamPoints[camPointIndex].eyeY, 0.075f,
                               this->subCamVelocity * 7.0f);
                Math_ApproachF(&this->subCamEyeNext.z, sCamPoints[camPointIndex].eyeZ, 0.075f,
                               this->subCamVelocity * 17.0f);
                Math_ApproachF(&this->subCamAtNext.y, sCamPoints[camPointIndex].atY, 0.075f,
                               this->subCamVelocity * 7.0f);
                Math_ApproachF(&this->subCamVelocity, 1.0f, 1.0f, 0.01f);

                if (this->cutsceneTimer == 70) {
                    Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathIntroAnim, -15.0f);
                    this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathIntroAnim);
                    Audio_PlaySfx_AtPosWithVolume(&this->actor.projectedPos, NA_SE_EV_ICE_PILLAR_RISING, 1.0f);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_SHOUT);
                }

                if (this->cutsceneTimer >= 110) {
                    Math_ApproachF(&this->whipLengthScale, 1.0f, 1.0f, 0.05f);
                    this->leftWhip.mobility = this->rightWhip.mobility = 0.01f * 80;
                    this->leftWhip.deceleration = this->rightWhip.deceleration = 1.0f;
                }

                if (this->cutsceneTimer == 127) {
                    this->whipLengthScale = 1.0f;
                }

                if (this->cutsceneTimer == 120) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_HOP2_OLD);
                    Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_KICK_OLD);
                    Audio_SetSfxVolumeTransition(&gSfxVolume, 0.0f, 60);
                }

                if (this->cutsceneTimer == 112) {
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_MAJORAS_WRATH | SEQ_FLAG_ASYNC);
                }

                if (this->cutsceneTimer == 137) {
                    TitleCard_InitBossName(&play->state, &play->actorCtx.titleCtx,
                                           Lib_SegmentedToVirtual(&gMajorasWrathTitleCardTex), 160, 180, 128, 40);
                }

                if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                    s32 i;
                    Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

                    this->cutsceneState = MAJORAS_WRATH_INTRO_CS_STATE_STARTING_OR_DONE;
                    Boss07_Wrath_SetupIdle(this, play, 50);
                    mainCam->eye = this->subCamEye;
                    mainCam->eyeNext = this->subCamEye;
                    mainCam->at = this->subCamAt;
                    func_80169AFC(play, this->subCamId, 0);
                    this->subCamId = SUB_CAM_ID_DONE;
                    Cutscene_StopManual(play, &play->csCtx);
                    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                    Play_DisableMotionBlur();

                    if (sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA] != NULL) {
                        for (i = 0; i < MAJORA_REMAINS_TYPE_MAX; i++) {
                            Actor_ChangeCategory(play, &play->actorCtx, &sMajoraRemains[i]->actor, ACTORCAT_BOSS);
                        }
                    }
                }
            }
            break;

        default:
            break;
    }

    Matrix_MultVec3f(&this->subCamEyeNext, &this->subCamEye);
    Matrix_MultVec3f(&this->subCamAtNext, &this->subCamAt);

    if (this->subCamId != SUB_CAM_ID_DONE) {
        ShrinkWindow_Letterbox_SetSizeTarget(27);
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void Boss07_Wrath_SetupDeathCutscene(Boss07* this, PlayState* play) {
    s32 i;

    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    Boss07_MovePlayerFromCenter(play);
    this->actionFunc = Boss07_Wrath_DeathCutscene;

    this->leftWhip.mobility = this->rightWhip.mobility = 0.7f;
    this->leftWhip.deceleration = this->rightWhip.deceleration = 2.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;
    this->leftWhip.gravity = this->rightWhip.gravity = -15.0f;

    Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathDeathAnim, 0.0f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->cutsceneState = MAJORAS_WRATH_DEATH_CS_STATE_STARTED;
    this->cutsceneTimer = 0;

    if (sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA] != NULL) {
        for (i = 0; i < MAJORA_REMAINS_TYPE_MAX; i++) {
            sMajoraRemains[i]->subAction = REMAINS_MOVE_SUB_ACTION_DIE;
        }
    }

    this->damagedTimer = 1000;

    GameInteractor_ExecuteOnBossDefeated(this->actor.id); // 2S2H Time Splits
    GameInteractor_ExecuteOnGameCompletion();
}

void Boss07_Wrath_DeathCutscene(Boss07* this, PlayState* play) {
    s32 i;
    s32 isCamCloseUp = false;
    Vec3f subCamEyeOffset;
    Vec3f subCamAtOffset = { 0.0f, 0.0f, 0.0f };
    // clang-format off
    f32 subCamMaxStepOffset = 0.0f; \
    f32 subCamScale = 0.1f;
    // clang-format on
    Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

    this->damagedTimer = 1000;
    Boss07_SmoothStop(this, 1.0f);

    switch (this->cutsceneState) {
        case MAJORAS_WRATH_DEATH_CS_STATE_STARTED:
            if (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_1);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_DEAD_ROD);
            FALLTHROUGH;
        case MAJORAS_WRATH_DEATH_CS_STATE_UPDATE_SUBCAM:
            this->cutsceneTimer = 0;
            this->cutsceneState = MAJORAS_WRATH_DEATH_CS_STATE_SPIN_AROUND;
            this->subCamEye.x = mainCam->eye.x;
            this->subCamEye.y = mainCam->eye.y;
            this->subCamEye.z = mainCam->eye.z;
            this->subCamAt.x = mainCam->at.x;
            this->subCamAt.y = mainCam->at.y;
            this->subCamAt.z = mainCam->at.z;
            this->subCamRotY = this->actor.shape.rot.y * M_PIf / 0x8000;
            this->subCamAngularVelocity = this->subCamVelocity = sMajoraBattleHandler->lensFlareScale = 0.0f;
            Boss07_InitRand(1, 0x71AC, 0x263A);

            for (i = 0; i < ARRAY_COUNT(this->deathLightScale); i++) {
                this->deathLightScale[i] = Boss07_RandZeroOne() - 1.0f;
            }

            Play_EnableMotionBlur(150);
            FALLTHROUGH;
        case MAJORAS_WRATH_DEATH_CS_STATE_SPIN_AROUND:
            if (this->cutsceneTimer == 20) {
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST2_WALK2_OLD);
            }

            if (this->cutsceneTimer == 40) {
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST2_WALK2_OLD);
            }

            if (this->cutsceneTimer == 60) {
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST2_WALK2_OLD);
            }

            if (this->cutsceneTimer == 80) {
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST2_WALK2_OLD);
            }

            subCamEyeOffset.x = 0.0f;
            subCamEyeOffset.y = -90.0f;
            subCamEyeOffset.z = 350.0f;
            this->subCamAtNext.x = this->actor.focus.pos.x;
            this->subCamAtNext.y = this->actor.focus.pos.y - 40.0f;
            this->subCamAtNext.z = this->actor.focus.pos.z;

            if ((this->cutsceneTimer >= 50) && (this->cutsceneTimer < 80)) {
                if (this->cutsceneTimer == 50) {
                    Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_DEAD_WIND1_OLD);
                }

                subCamEyeOffset.x = 30.0f;
                subCamEyeOffset.z = 120.0f;
                this->subCamAtNext.y = this->actor.focus.pos.y - 40.0f + 30.0f;
                subCamMaxStepOffset = 200.0f;
                subCamScale = 1.0f;
            } else if ((this->cutsceneTimer >= 80) && (this->cutsceneTimer < 110)) {
                if (this->cutsceneTimer == 80) {
                    this->skelAnime.curFrame -= 30.0f;
                    Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_DEAD_WIND2_OLD);
                }

                subCamEyeOffset.x = -10.0f;
                subCamEyeOffset.z = 150.0f;
                this->subCamAtNext.y = this->actor.focus.pos.y - 40.0f - 60.0f;
                subCamMaxStepOffset = 200.0f;
                subCamScale = 1.0f;
            } else if ((this->cutsceneTimer >= 110) && (this->cutsceneTimer < 140)) {
                if (this->cutsceneTimer == 110) {
                    Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_DEAD_WIND3_OLD);
                }

                subCamEyeOffset.x = -70.0f;
                subCamEyeOffset.z = 110.0f;
                this->subCamAtNext.y = this->actor.focus.pos.y - 40.0f + 30.0f;
                subCamMaxStepOffset = 200.0f;
                subCamScale = 1.0f;
            }

            if (this->cutsceneTimer < (u32)(sREG(15) + 140)) {
                break;
            }

            this->cutsceneState = MAJORAS_WRATH_DEATH_CS_STATE_FLOAT;
            this->subCamEyeModY = sREG(16) + 270.0f + 50.0f;
            Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_DEAD_FLOAT);
            FALLTHROUGH;
        case MAJORAS_WRATH_DEATH_CS_STATE_FLOAT:
            if ((this->cutsceneTimer >= (u32)(sREG(90) + 260)) && (this->cutsceneTimer < (u32)(sREG(91) + 370))) {
                isCamCloseUp = KREG(14) + 1;
                this->subCamRotY = this->actor.shape.rot.y * M_PIf / 0x8000;

                subCamEyeOffset.x = 0.0f;
                subCamEyeOffset.y = this->subCamEyeModY + -190.0f;
                subCamEyeOffset.z = sREG(17) + 390.0f - 380.0f;

                this->subCamAtNext.x = this->actor.focus.pos.x;
                this->subCamAtNext.y = this->actor.focus.pos.y - 40.0f - 60.0f + sREG(18) + 130.0f;
                this->subCamAtNext.z = this->actor.focus.pos.z;

                Math_ApproachF(&this->subCamEyeModY, sREG(19) + 240.0f, 0.05f, 1.0f + (sREG(20) * 0.1f));
                subCamMaxStepOffset = 2000.0f;
                subCamScale = 1.0f;

                this->maxDecayPixels = KREG(81) + 10;
                this->disableShadow = true;
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EV_BURN_OUT - SFX_FLAG);
            } else {
                subCamEyeOffset.x = 0.0f;
                subCamEyeOffset.y = -190.0f;
                subCamEyeOffset.z = 390.0f;
                this->subCamAtNext.x = this->actor.focus.pos.x;
                this->subCamAtNext.y = this->actor.focus.pos.y - 40.0f - 60.0f;
                this->subCamAtNext.z = this->actor.focus.pos.z;
                this->maxDecayPixels = 0;

                if (this->cutsceneTimer > 330) {
                    subCamMaxStepOffset = 2000.0f;
                    subCamScale = 1.0f;
                }

                Math_ApproachZeroF(&this->actor.world.pos.x, 0.1f, this->subCamVelocity);
                Math_ApproachZeroF(&this->actor.world.pos.z, 0.1f, this->subCamVelocity);
                Math_ApproachF(&this->subCamVelocity, 5.0f, 1.0f, 0.1f);
            }

            if (this->cutsceneTimer >= 260) {
                f32 deathLightScaleTarget;
                f32 deathLightScaleMaxStep;
                s16 screenFillAlpha;

                if (this->cutsceneTimer == 260) {
                    Audio_PlaySfx_AtPosWithVolumeTransition(&sMajoraSfxPos, NA_SE_EN_LAST3_DEAD_LIGHTS_OLD, 60);
                }

                play->envCtx.fillScreen = true;
                play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] = play->envCtx.screenFillColor[2] =
                    255;

                if (this->cutsceneTimer < 350) {
                    deathLightScaleTarget = 0.5f;
                    deathLightScaleMaxStep = 0.02f;
                    play->envCtx.screenFillColor[3] = 0;
                } else {
                    deathLightScaleTarget = 5.0f;
                    deathLightScaleMaxStep = 0.1f;
                    screenFillAlpha = (this->cutsceneTimer * 2) - 700;
                    if (screenFillAlpha > 250) {
                        screenFillAlpha = 250;
                    }

                    play->envCtx.screenFillColor[3] = screenFillAlpha;

                    if (this->cutsceneTimer == 400) {
                        Audio_SetSfxVolumeTransition(&gSfxVolume, 0.0f, 90);
                    }
                    if (this->cutsceneTimer == (u32)(KREG(94) + 440)) {
                        play->nextEntrance = ENTRANCE(TERMINA_FIELD, 0);
                        gSaveContext.nextCutsceneIndex = 0xFFF7;
                        play->transitionTrigger = TRANS_TRIGGER_START;
                    }
                }
                if (this->cutsceneTimer > 300) {
                    sMajoraBattleHandler->lensFlareOn = true;
                    Math_ApproachF(&sMajoraBattleHandler->lensFlareScale, 30.0f, 0.1f, 1.5f);
                    sMajoraBattleHandler->lensFlarePos = this->bodyPartsPos[MAJORAS_WRATH_BODYPART_PELVIS];
                    Math_ApproachF(&this->deathOrbScale, 1.0f, 0.1f, 0.05f);

                    for (i = 0; i < ARRAY_COUNT(this->deathLightScale); i++) {
                        Math_ApproachF(&this->deathLightScale[i], deathLightScaleTarget, 1.0f, deathLightScaleMaxStep);
                    }

                    Math_ApproachF(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.1f);
                }
            }

            this->subCamRotY += this->subCamAngularVelocity;
            this->subCamAngularVelocity += 0.0004f;
            if (this->subCamAngularVelocity > 0.02f) {
                this->subCamAngularVelocity = 0.02f;
            }

            if (this->cutsceneTimer >= (u32)(sREG(93) + 180)) {
                Vec3f hahenVelocity = { 0.0f, 10.0f, 0.0f };
                Vec3f hahenAccel = { 0.0f, -0.5f, 0.0f };
                Vec3f hahenPos;
                f32 cameraShakeMagnitude;

                Audio_PlaySfx(NA_SE_EV_EARTHQUAKE_LAST2 - SFX_FLAG);

                if (!isCamCloseUp) {
                    cameraShakeMagnitude = 2.0f;
                } else {
                    cameraShakeMagnitude = (KREG(53) * 0.01f) + 0.2f;
                }

                subCamAtOffset.x = Math_SinS(this->cutsceneTimer * 0x7000) * cameraShakeMagnitude;
                subCamAtOffset.y = Math_SinS(this->cutsceneTimer * 0x5000) * cameraShakeMagnitude * 2.5f;
                subCamAtOffset.z = Math_CosS(this->cutsceneTimer * 0x8000) * cameraShakeMagnitude;

                for (i = 0; i < 2; i++) {
                    hahenPos.x = Rand_CenteredFloat(500.0f) + this->actor.world.pos.x;
                    hahenPos.y = Rand_ZeroFloat(50.0f) + this->actor.world.pos.y + 200.0f;
                    hahenPos.z = Rand_CenteredFloat(500.0f) + this->actor.world.pos.z;
                    EffectSsHahen_Spawn(play, &hahenPos, &hahenVelocity, &hahenAccel, 0, Rand_ZeroFloat(5.0f) + 20.0f,
                                        HAHEN_OBJECT_DEFAULT, 10, NULL);
                }
            }

            if (this->cutsceneTimer >= (u32)(sREG(94) + 290)) {
                this->maxDecayPixels = KREG(86) + 25;
                Math_ApproachZeroF(&this->whipLengthScale, 1.0f, 0.015f);
            }
            break;

        default:
            break;
    }

    Matrix_RotateYF(this->subCamRotY, MTXMODE_NEW);
    Matrix_MultVec3f(&subCamEyeOffset, &this->subCamEyeNext);
    this->subCamEyeNext.x += this->actor.focus.pos.x;
    this->subCamEyeNext.y += this->actor.focus.pos.y;
    this->subCamEyeNext.z += this->actor.focus.pos.z;

    Math_ApproachF(&this->subCamEye.x, this->subCamEyeNext.x, subCamScale, 40.0f + subCamMaxStepOffset);
    Math_ApproachF(&this->subCamEye.y, this->subCamEyeNext.y, subCamScale, 40.0f + subCamMaxStepOffset);
    Math_ApproachF(&this->subCamEye.z, this->subCamEyeNext.z, subCamScale, 40.0f + subCamMaxStepOffset);

    Math_ApproachF(&this->subCamAt.x, this->subCamAtNext.x, subCamScale, 70.0f + subCamMaxStepOffset);
    Math_ApproachF(&this->subCamAt.y, this->subCamAtNext.y, subCamScale, 70.0f + subCamMaxStepOffset);
    Math_ApproachF(&this->subCamAt.z, this->subCamAtNext.z, subCamScale, 70.0f + subCamMaxStepOffset);

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Vec3f subCamAt;

        subCamAt.x = this->subCamAt.x + subCamAtOffset.x;
        subCamAt.y = this->subCamAt.y + subCamAtOffset.y;
        subCamAt.z = this->subCamAt.z + subCamAtOffset.z;
        Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &this->subCamEye);
    }

    SkelAnime_Update(&this->skelAnime);
    this->cutsceneTimer++;
}

void Boss07_Wrath_SetupIdle(Boss07* this, PlayState* play, s16 idleTimer) {
    this->actionFunc = Boss07_Wrath_Idle;
    Animation_MorphToLoop(&this->skelAnime, &gMajorasWrathIdleAnim, -10.0f);

    if (idleTimer != 0) {
        this->timers[0] = idleTimer;
    } else {
        this->timers[0] = Rand_ZeroFloat(30.0f);
    }

    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
}

void Boss07_Wrath_Idle(Boss07* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    Boss07_SmoothStop(this, 2.0f);

    this->rightWhip.mobility = this->leftWhip.mobility = 0.7f;
    this->rightWhip.gravity = this->leftWhip.gravity = -15.0f;
    this->rightWhip.deceleration = this->leftWhip.deceleration = 2.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;

    if ((this->actor.xzDistToPlayer <= 200.0f) && (player->actor.world.pos.y < 10.0f)) {
        if (Rand_ZeroOne() < 0.3f) {
            this->actor.xzDistToPlayer = 250.0f;
        }

        Boss07_Wrath_SetupAttack(this, play);
    } else if (this->timers[0] == 0) {
        if (KREG(78) == 1) {
            Boss07_Wrath_SetupThrowTop(this, play);
        } else if ((s8)this->actor.colChkInfo.health >= 28) {
            Boss07_Wrath_SetupAttack(this, play);
        } else if (((s8)this->actor.colChkInfo.health <= 12) && (Rand_ZeroOne() < 0.65f)) {
            Boss07_Wrath_SetupThrowTop(this, play);
        } else if (Rand_ZeroOne() < 0.3f) {
            Boss07_Wrath_SetupTryGrab(this, play);
        } else {
            Boss07_Wrath_SetupAttack(this, play);
        }
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000);
    this->canEvade = true;
}

void Boss07_Wrath_SetupJump(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Wrath_StartJump;
    Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathBackflipAnim, -5.0f);
    this->frameCounter = 0;
}

void Boss07_Wrath_StartJump(Boss07* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->leftWhip.mobility = this->rightWhip.mobility = 0.7f;
    this->leftWhip.deceleration = this->rightWhip.deceleration = 2.0f;
    this->leftWhip.gravity = this->rightWhip.gravity = -15.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;

    if (this->frameCounter == 1) {
        this->actor.velocity.y = 35.0f;
        this->actor.world.rot.y = Math_Atan2F_XY(-this->actor.world.pos.z, -this->actor.world.pos.x) * (0x8000 / M_PIf);
        this->actionFunc = Boss07_Wrath_Jump;
        this->actor.speed = 20.0f;
    }
}

void Boss07_Wrath_Jump(Boss07* this, PlayState* play) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_JUMP2 - SFX_FLAG);

    if (this->frameCounter == 13) {
        Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_LAUGH_OLD);
    }

    SkelAnime_Update(&this->skelAnime);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Boss07_Wrath_SetupIdle(this, play, 1);
        this->landSfxTimer = 5;
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x4000);
}

void Boss07_Wrath_SetupFlip(Boss07* this, PlayState* play) {
    Vec3f direction;
    Vec3f targetPos;
    s16 yawDiff;

    this->actionFunc = Boss07_Wrath_Flip;
    this->actor.velocity.y = 25.0f;
    direction.x = 0.0f - this->actor.world.pos.x;
    direction.z = 0.0f - this->actor.world.pos.z;

    yawDiff = this->actor.yawTowardsPlayer - (s16)(Math_Atan2F_XY(direction.z, direction.x) * (0x8000 / M_PIf));
    if (yawDiff < 0) {
        direction.x = 200.0f;
        Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathFlipLeftAnim, -5.0f);
    } else {
        direction.x = -200.0f;
        Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathFlipRightAnim, -5.0f);
    }

    Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
    Matrix_MultVecX(direction.x, &targetPos);
    direction.x = targetPos.x - this->actor.world.pos.x;
    direction.z = targetPos.z - this->actor.world.pos.z;
    this->actor.world.rot.y = Math_Atan2F_XY(direction.z, direction.x) * (0x8000 / M_PIf);
    this->frameCounter = 0;
    this->actor.speed = 17.0f;
}

void Boss07_Wrath_Flip(Boss07* this, PlayState* play) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_JUMP2 - SFX_FLAG);
    SkelAnime_Update(&this->skelAnime);
    this->leftWhip.mobility = this->rightWhip.mobility = 0.7f;
    this->leftWhip.deceleration = this->rightWhip.deceleration = 2.0f;
    this->leftWhip.gravity = this->rightWhip.gravity = -15.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;

    if ((this->frameCounter == 10) && (Rand_ZeroOne() < 0.5f)) {
        Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_LAUGH_OLD);
    }

    if ((this->actor.velocity.y < 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (Rand_ZeroOne() < 0.3f) {
            Boss07_Wrath_ChooseJump(this, play, true);
        } else {
            Boss07_Wrath_SetupIdle(this, play, 1);
            this->actor.speed = 5.0f;
        }

        this->landSfxTimer = 5;
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x4000);
}

void Boss07_Wrath_SetupSidestep(Boss07* this, PlayState* play) {
    Vec3f direction;
    s16 yawDiff;

    this->actionFunc = Boss07_Wrath_Sidestep;
    Animation_MorphToLoop(&this->skelAnime, &gMajorasWrathSidestepAnim, -5.0f);
    direction.x = -this->actor.world.pos.x;
    direction.z = -this->actor.world.pos.z;
    yawDiff = this->actor.yawTowardsPlayer - (s16)(Math_Atan2F_XY(direction.z, direction.x) * (0x8000 / M_PIf));
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);

    if (yawDiff < 0) {
        this->skelAnime.playSpeed = 1.0f;
        direction.x = 300.0f;
    } else {
        this->skelAnime.playSpeed = -1.0f;
        direction.x = -300.0f;
    }

    Matrix_MultVecX(direction.x, &this->targetPos);
    this->targetPos.x += this->actor.world.pos.x;
    this->targetPos.z += this->actor.world.pos.z;
    this->timers[1] = 21;
    this->disableCollisionTimer = 10;
    this->speedToTarget = 0.0f;
    this->sfxTimer = 0;
}

void Boss07_Wrath_Sidestep(Boss07* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->frameCounter == 20) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_VOICE_KICK_OLD);
    }

    this->sfxTimer++;
    if ((this->sfxTimer % 16) == 0) {
        Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST2_WALK2_OLD);
    }

    this->leftWhip.mobility = this->rightWhip.mobility = 0.7f;
    this->leftWhip.deceleration = this->rightWhip.deceleration = 2.0f;
    this->leftWhip.gravity = this->rightWhip.gravity = -15.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000);
    Math_ApproachF(&this->actor.world.pos.x, this->targetPos.x, 0.8f, this->speedToTarget);
    Math_ApproachF(&this->actor.world.pos.z, this->targetPos.z, 0.8f, this->speedToTarget);
    Math_ApproachF(&this->speedToTarget, 20.0f, 1.0f, 10.0f);

    if (this->timers[1] == 0) {
        if (Rand_ZeroOne() < 0.3f) {
            Boss07_Wrath_SetupSidestep(this, play);
        } else {
            Boss07_Wrath_SetupIdle(this, play, 1);
        }
    }
}

void Boss07_Wrath_SetupAttack(Boss07* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actionFunc = Boss07_Wrath_Attack;
    this->timers[1] = 0;

    if (player->stateFlags3 & PLAYER_STATE3_100) {
        this->subAction = MAJORAS_WRATH_ATTACK_SUB_ACTION_SPIN_ATTACK;
    } else if (this->actor.xzDistToPlayer <= 300.0f) {
        if (this->actor.xzDistToPlayer <= 200.0f) {
            this->subAction = MAJORAS_WRATH_ATTACK_SUB_ACTION_KICK;
        } else {
            this->subAction = MAJORAS_WRATH_ATTACK_SUB_ACTION_SPIN_ATTACK;
        }
    } else {
        this->subAction = Rand_ZeroFloat(MAJORAS_WRATH_ATTACK_SUB_ACTION_WHIP_ATTACK_MAX - 0.01f);

        if (((s8)this->actor.colChkInfo.health >= 28) &&
            ((this->subAction == MAJORAS_WRATH_ATTACK_SUB_ACTION_FLURRY) ||
             (this->subAction == MAJORAS_WRATH_ATTACK_SUB_ACTION_DOUBLE_WHIP))) {
            this->subAction = MAJORAS_WRATH_ATTACK_SUB_ACTION_QUICK_WHIP;
        }
    }

    switch (this->subAction) {
        case MAJORAS_WRATH_ATTACK_SUB_ACTION_QUICK_WHIP:
            Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathShortSingleWhipAttackAnim, -5.0f);
            this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathShortSingleWhipAttackAnim);
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_FLURRY:
            Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathWhipFlurryAttackAnim, -5.0f);
            this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathWhipFlurryAttackAnim);
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_DOUBLE_WHIP:
            Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathDoubleWhipAttackAnim, -5.0f);
            this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathDoubleWhipAttackAnim);
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_LONG_WHIP:
            Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathLongSingleWhipAttackAnim, -5.0f);
            this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathLongSingleWhipAttackAnim);
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_SPIN_ATTACK:
            Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathSpinAttackAnim, -5.0f);
            this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathSpinAttackAnim);
            Boss07_RandXZ(&this->targetPos, 650.0f);
            this->speedToTarget = 0.0f;
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_TAUNT:
            Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathTauntAnim, -5.0f);
            this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathTauntAnim);
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_THREE_HIT:
            Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathThreeAttackComboAnim, -5.0f);
            this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathThreeAttackComboAnim);
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_KICK:
            Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathKickAnim, -5.0f);
            this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathKickAnim);
            break;

        default:
            break;
    }

    this->frameCounter = 0;
}

void Boss07_Wrath_Attack(Boss07* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Boss07_SmoothStop(this, 2.0f);
    this->leftWhip.mobility = this->rightWhip.mobility = 0.01f * 80;
    this->leftWhip.gravity = this->rightWhip.gravity = -5.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;
    this->leftWhip.deceleration = this->rightWhip.deceleration = 1.0f;

    if (this->frameCounter > 20) {
        this->canEvade = true;
    }

    switch (this->subAction) {
        case MAJORAS_WRATH_ATTACK_SUB_ACTION_QUICK_WHIP:
            if (this->frameCounter == (s16)(KREG(92) + 1)) {
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_ROD_OLD);
            }

            if ((this->frameCounter >= 15) && (this->frameCounter < 18)) {
                this->rightWhip.tension = 500.0f;
            }

            if (this->frameCounter == 9) {
                this->whipCrackTimer = 11;
            }

            if (this->frameCounter == 1) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_HOP_OLD);
            }

            if (this->frameCounter == 10) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_WIND_OLD);
            }
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_FLURRY:
            if (this->frameCounter == (s16)(KREG(91) + 3)) {
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_THROW_OLD);
            }

            if ((this->frameCounter >= 8) && (this->frameCounter <= 55)) {
                this->leftWhip.tension = this->rightWhip.tension = 300.0f;

                if ((((this->frameCounter + 2) % 4) == 0) && (Rand_ZeroOne() < 0.5f)) {
                    Audio_PlaySfx(NA_SE_EN_LAST3_ROD_FLOOR_OLD);
                }

                if ((this->frameCounter % 4) == 0) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_WIND_OLD);
                }

                Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000);
            }
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_DOUBLE_WHIP:
            if (this->frameCounter == (s16)(KREG(84) + 5)) {
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_ROD_OLD);
            }

            if ((this->frameCounter >= 32) && (this->frameCounter <= 38)) {
                this->rightWhip.tension = 300.0f;
                this->leftWhip.tension = 300.0f;
            }

            if (this->frameCounter == 28) {
                this->whipCrackTimer = 11;
            }

            if (this->frameCounter == 10) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_HOP_OLD);
            }

            if (this->frameCounter == 32) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_WIND_OLD);
            }
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_LONG_WHIP:
            if (this->frameCounter == (s16)(KREG(84) + 5)) {
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_ROD_OLD);
            }

            if ((this->frameCounter > 30) && (this->frameCounter <= 35)) {
                this->rightWhip.tension = 1200.0f;
            }

            if (this->frameCounter == 23) {
                this->whipCrackTimer = 11;
            }

            Math_ApproachF(&this->wrathLeanRotY, -0.1f, 0.5f, 0.1f);
            Math_ApproachF(&this->wrathLeanRotX, 0.3f, 0.5f, 0.1f);

            if (this->frameCounter == 5) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_HOP_OLD);
            }

            if (this->frameCounter == 30) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_WIND_OLD);
            }
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_SPIN_ATTACK:
            if ((this->frameCounter > 16) && (this->frameCounter <= 40)) {
                this->leftWhip.tension = this->rightWhip.tension = 200.0f;

                if ((this->frameCounter % 8) == 0) {
                    Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_KOMA_OLD);
                }

                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_DANCE_OLD - SFX_FLAG);
                Math_ApproachF(&this->actor.world.pos.x, this->targetPos.x, 0.1f, this->speedToTarget);
                Math_ApproachF(&this->actor.world.pos.z, this->targetPos.z, 0.1f, this->speedToTarget);
                Math_ApproachF(&this->speedToTarget, 20.0f, 1.0f, 4.0f);
                Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x2000);
                this->canEvade = false;
            }
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_KICK:
            this->canEvade = false;
            if (this->frameCounter == 3) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_VOICE_KICK_OLD);
            }
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_TAUNT:
            this->rightWhip.mobility = this->leftWhip.mobility = 0.7f;
            this->leftWhip.gravity = this->rightWhip.gravity = -15.0f;
            Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000);
            this->canEvade = true;
            break;

        case MAJORAS_WRATH_ATTACK_SUB_ACTION_THREE_HIT:
            if (this->frameCounter == (s16)(KREG(85) + 5)) {
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_ROD_OLD);
            }

            if ((this->frameCounter >= 14) && (this->frameCounter < 19)) {
                this->rightWhip.tension = 150.0f;
            }

            if ((this->frameCounter >= 23) && (this->frameCounter < 29)) {
                this->leftWhip.tension = 200.0f;
            }

            if ((this->frameCounter >= 43) && (this->frameCounter < 49)) {
                this->rightWhip.tension = 200.0f;
            }

            Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000);

            if (this->frameCounter == 20) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_HOP_OLD);
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_ROD_OLD);
            }

            if (this->frameCounter == 5) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_HOP2_OLD);
            }

            if (this->frameCounter == 41) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_WIND_OLD);
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_ROD_OLD);
            }
            break;

        default:
            break;
    }

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame) || (this->timers[1] == 1)) {
        Boss07_Wrath_SetupIdle(this, play, 0);
    }
}

void Boss07_Wrath_SetupTryGrab(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Wrath_TryGrab;
    Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathGrabAnim, -5.0f);
    this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathGrabAnim);
    this->frameCounter = 0;
}

void Boss07_Wrath_TryGrab(Boss07* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer - 0x800, 3, 0x2000);
    Boss07_SmoothStop(this, 2.0f);
    this->leftWhip.mobility = this->rightWhip.mobility = 0.01f * 80;
    this->leftWhip.deceleration = this->rightWhip.deceleration = 1.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;
    this->rightWhip.gravity = -5.0f;
    this->leftWhip.gravity = -15.0f;

    if ((this->frameCounter >= 14) && (this->frameCounter <= 18)) {
        this->rightWhip.tension = 500.0f;
    }

    if (this->frameCounter >= 18) {
        Boss07_Wrath_SetupIdle(this, play, Rand_ZeroFloat(20.0f) + 20.0f);
        Audio_PlaySfx(NA_SE_EN_LAST3_ROD_MID_OLD);
    }
}

void Boss07_Wrath_GrabPlayer(Boss07* this, PlayState* play) {
    PlayerImpactType playerImpactType;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    Boss07_SmoothStop(this, 2.0f);
    player->actor.world.pos = this->whipGrabPos;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->whipCollisionTimer = 20;
    this->whipWrapStartIndex++;

    if (this->frameCounter > (s16)((MAJORA_WHIP_LENGTH + 2) - this->whipWrapEndOffset)) {
        Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_THROW_OLD);
        Audio_PlaySfx(NA_SE_EN_LAST3_COIL_ATTACK_OLD);
        this->actionFunc = Boss07_Wrath_ThrowPlayer;
        Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathThrowAnim, -5.0f);
        this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathThrowAnim);
        this->frameCounter = 0;
    }

    if ((Actor_GetPlayerImpact(play, 1000.0f, &this->actor.world.pos, &playerImpactType) >= 0.0f) &&
        (playerImpactType == PLAYER_IMPACT_ZORA_BARRIER)) {
        Boss07_Wrath_SetupShock(this, play);
    }
}

void Boss07_Wrath_ThrowPlayer(Boss07* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 speed;
    f32 velocityY;
    PlayerImpactType playerImpactType;

    SkelAnime_Update(&this->skelAnime);
    this->whipCollisionTimer = 20;

    if (this->frameCounter == 6) {
        this->whipWrapEndOffset = 0;

        if (&this->actor == player->actor.parent) {
            player->av2.actionVar2 = 101;
            player->actor.parent = NULL;
            player->csAction = PLAYER_CSACTION_NONE;

            if (player->transformation == PLAYER_FORM_DEKU) {
                speed = 23.0f;
                velocityY = 20.0f;
            } else if (player->transformation == PLAYER_FORM_GORON) {
                speed = 15.0f;
                velocityY = 10.0f;
            } else if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
                speed = 10.0f;
                velocityY = 3.0f;
            } else {
                speed = 20.0f;
                velocityY = 15.0f;
            }

            func_800B8D50(play, NULL, speed, this->actor.yawTowardsPlayer + 0x9000, velocityY, 0x10);
        }
    }

    if (this->frameCounter < 7) {
        player->actor.world.pos = this->whipGrabPos;

        if ((Actor_GetPlayerImpact(play, 1000.0f, &this->actor.world.pos, &playerImpactType) >= 0.0f) &&
            (playerImpactType == PLAYER_IMPACT_ZORA_BARRIER)) {
            Boss07_Wrath_SetupShock(this, play);
        }
    }

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Boss07_Wrath_SetupIdle(this, play, 0);
    }
}

void Boss07_Wrath_SetupShock(Boss07* this, PlayState* play) {
    s32 whipShockIndex;
    s32 whipShockIndexMax;

    this->actionFunc = Boss07_Wrath_ShockWhip;

    whipShockIndex = this->whipWrapEndOffset + 10;
    this->whipShockMaxIndex = this->whipShockMinIndex = whipShockIndex;

    whipShockIndexMax = sWhipLength - 1;
    if (whipShockIndexMax < whipShockIndex) {
        this->whipShockMaxIndex = this->whipShockMinIndex = whipShockIndexMax;
    }

    this->frameCounter = 0;
    Audio_PlaySfx(NA_SE_EV_ELECTRIC_EXPLOSION);
}

void Boss07_Wrath_ShockWhip(Boss07* this, PlayState* play) {
    s32 i;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    Boss07_SmoothStop(this, 2.0f);
    player->actor.world.pos = this->whipGrabPos;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->whipCollisionTimer = 20;

    if (this->frameCounter <= (s16)((MAJORA_WHIP_LENGTH + 2) - this->whipWrapEndOffset)) {
        this->whipWrapStartIndex++;
    }

    for (i = 0; i < 4; i++) {
        if (this->whipShockMinIndex != 0) {
            this->whipShockMinIndex--;
        }
    }

    if (this->whipShockMinIndex == 0) {
        Math_ApproachF(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.3f);
        Math_ApproachF(&this->shockOrbScale, 5.0f, 0.5f, 3.0f);
        Audio_PlaySfx(NA_SE_EN_BIRI_SPARK - SFX_FLAG);
    } else {
        this->shockOrbScale = 0.01f;
    }

    if (this->shockOrbScale > 4.0f) {
        this->actionFunc = Boss07_Wrath_ShockStunned;
        Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathStunAnim, -10.0f);
        this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathStunAnim);
        this->whipWrapEndOffset = 0;

        if (&this->actor == player->actor.parent) {
            player->av2.actionVar2 = 101;
            player->actor.parent = NULL;
            player->csAction = PLAYER_CSACTION_NONE;
        }

        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x, this->actor.focus.pos.y - 10.0f,
                    this->actor.focus.pos.z, 0, 0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
        this->frameCounter = 0;
    }
}

void Boss07_Wrath_ShockStunned(Boss07* this, PlayState* play) {
    s32 i;

    SkelAnime_Update(&this->skelAnime);
    this->whipCollisionTimer = 20;

    for (i = 0; i < 2; i++) {
        DECR(this->whipShockMaxIndex);
    }

    if (this->frameCounter < 70) {
        Math_ApproachF(&this->shockOrbScale, 5.0f, 0.5f, 3.0f);
        Math_ApproachF(&this->shockSparkScale, 2.5f, 0.5f, 2.0f);
        Audio_PlaySfx(NA_SE_EN_BIRI_SPARK - SFX_FLAG);
        Math_ApproachF(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.3f);
    }

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Boss07_Wrath_ChooseJump(this, play, true);
    }
}

void Boss07_Wrath_SetupStunned(Boss07* this, PlayState* play) {
    if (this->actionFunc != Boss07_Wrath_Stunned) {
        this->actionFunc = Boss07_Wrath_Stunned;
        Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathStunAnim, -10.0f);
        this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathStunAnim);
    }

    this->disableCollisionTimer = 10;
    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_VOICE_DAMAGE_OLD);
}

void Boss07_Wrath_SetupThrowTop(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Wrath_ThrowTop;
    Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathThrowTopAnim, -5.0f);
    this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathThrowTopAnim);
    this->frameCounter = 0;
}

void Boss07_Wrath_ThrowTop(Boss07* this, PlayState* play) {
    this->whipCollisionTimer = 20;

    if (this->frameCounter < (s16)(KREG(40) + 14)) {
        this->whipTopIndex += 6;

        if (this->whipTopIndex > MAJORA_WHIP_LENGTH) {
            this->whipTopIndex = MAJORA_WHIP_LENGTH;
        }
    }

    if (this->frameCounter == 8) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_07, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, MAJORA_PARAMS(MAJORA_TYPE_TOP));
    }

    //! FAKE:
    if (1) {}

    if (this->frameCounter == 10) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_ROD_WIND_OLD);
    }

    if (this->frameCounter == (s16)(KREG(40) + 18)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_VOICE_KOMA_OLD);
    }

    if ((this->frameCounter < (s16)(KREG(40) + 14)) || (this->frameCounter > (s16)(KREG(41) + 17))) {
        SkelAnime_Update(&this->skelAnime);
    }

    if ((this->frameCounter >= (s16)(KREG(40) + 14)) &&
        ((this->frameCounter <= (s16)(KREG(41) + 17)) || (this->frameCounter >= (s16)(KREG(42) + 21)))) {
        this->whipTopIndex -= KREG(39) + 5;

        if (this->whipTopIndex < 0) {
            this->whipTopIndex = 0;
        }
    }

    Math_ApproachS(&this->actor.shape.rot.y, KREG(8) * 0x100 + this->actor.yawTowardsPlayer + 0xF00, 3, 0x2000);
    Boss07_SmoothStop(this, 2.0f);

    this->leftWhip.mobility = this->rightWhip.mobility = 0.01f * 80;
    this->leftWhip.deceleration = this->rightWhip.deceleration = 1.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;
    this->rightWhip.gravity = -5.0f;
    this->leftWhip.gravity = -15.0f;

    if ((this->frameCounter >= (s16)(KREG(43) + 12)) && (this->frameCounter <= (s16)(KREG(44) + 17))) {
        this->rightWhip.tension = KREG(6) + 500.0f;
    }

    if (this->frameCounter >= (s16)(KREG(45) + 36)) {
        Boss07_Wrath_SetupIdle(this, play, Rand_ZeroFloat(20.0f) + 20.0f);
    }
}

void Boss07_Wrath_Stunned(Boss07* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_COMMON_WEAKENED - SFX_FLAG);
    Boss07_SmoothStop(this, 2.0f);

    this->leftWhip.mobility = this->rightWhip.mobility = 0.7f;
    this->leftWhip.gravity = this->rightWhip.gravity = -15.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;
    this->leftWhip.deceleration = this->rightWhip.deceleration = 2.0f;

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Boss07_Wrath_ChooseJump(this, play, true);
    }
}

void Boss07_Wrath_SetupDamaged(Boss07* this, PlayState* play, u8 damage, u8 dmgEffect) {
    if ((s8)this->actor.colChkInfo.health >= 0) {
        this->actor.colChkInfo.health -= damage;
    }

    if ((s8)this->actor.colChkInfo.health <= 0) {
        if (KREG(19) != 0) {
            Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST3_VOICE_DEAD_OLD);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_VOICE_DEAD_OLD);
        }

        this->shouldStartDeath = true;
        Enemy_StartFinishingBlow(play, &this->actor);
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_VOICE_DAMAGE2_OLD);

        if (this->actionFunc != Boss07_Wrath_Damaged) {
            Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathDamagedAnim, -10.0f);
            this->actionFunc = Boss07_Wrath_Damaged;
            this->animEndFrame = Animation_GetLastFrame(&gMajorasWrathDamagedAnim);
            this->sfxTimer = 0;
        } else if (dmgEffect == MAJORAS_WRATH_DMGEFF_ANIM_FRAME_CHECK) {
            if (this->skelAnime.curFrame <= (this->animEndFrame - 5.0f)) {
                this->disableCollisionTimer = 30;
            } else {
                Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasWrathDamagedAnim, -5.0f);
            }
        }
    }
}

void Boss07_Wrath_Damaged(Boss07* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->sfxTimer++;

    if ((this->sfxTimer % 16) == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_WALK2_OLD);
    }

    Boss07_SmoothStop(this, 2.0f);

    this->leftWhip.mobility = this->rightWhip.mobility = 0.7f;
    this->leftWhip.gravity = this->rightWhip.gravity = -15.0f;
    this->leftWhip.deceleration = this->rightWhip.deceleration = 2.0f;
    this->leftWhip.tension = this->rightWhip.tension = 0.0f;

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Boss07_Wrath_ChooseJump(this, play, true);
    }
}

void Boss07_Wrath_CheckWhipCollisions(Vec3f* whipPos, f32 tension, Boss07* this, PlayState* play) {
    s32 i;
    PlayerImpactType playerImpactType = -1;
    Player* player = GET_PLAYER(play);
    f32 dx;
    f32 dy;
    f32 dz;
    Vec3f hitPos;

    if ((tension >= 50.0f) && (this->whipCollisionTimer == 0)) {
        Actor* prop;

        for (prop = play->actorCtx.actorLists[ACTORCAT_PROP].first; prop != NULL; prop = prop->next) {
            if (prop->id == ACTOR_OBJ_TSUBO) {
                for (i = 10; i < sWhipLength; i += 3) {
                    dx = prop->world.pos.x - whipPos[i].x;
                    dy = prop->world.pos.y + 10.0f - whipPos[i].y;
                    dz = prop->world.pos.z - whipPos[i].z;

                    if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < (KREG(38) + 60.0f)) {
                        ((ObjTsubo*)prop)->unk_19B = true;
                        Boss07_Wrath_SpawnDustAtPos(play, &prop->world.pos, 10);
                    }
                }
            }
        }
    }

    if ((tension >= 50.0f) && (this->whipCollisionTimer == 0) &&
        (!(player->stateFlags3 & PLAYER_STATE3_100) ||
         (this->subAction == MAJORAS_WRATH_ATTACK_SUB_ACTION_SPIN_ATTACK))) {
        if ((Actor_GetPlayerImpact(play, 1000.0f, &this->actor.world.pos, &playerImpactType) >= 0.0f) &&
            (playerImpactType != PLAYER_IMPACT_ZORA_BARRIER)) {
            playerImpactType = -1;
        }

        for (i = 10; i < sWhipLength; i += 3) {
            dx = player->actor.world.pos.x - whipPos[i].x;
            dy = player->actor.world.pos.y + 30.0f - whipPos[i].y;
            dz = player->actor.world.pos.z - whipPos[i].z;

            dy *= 1.75f;

            if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 140.0f) {
                if ((this->actionFunc == Boss07_Wrath_TryGrab) && (playerImpactType != PLAYER_IMPACT_ZORA_BARRIER) &&
                    !(player->stateFlags3 & PLAYER_STATE3_1000) && (this->actor.xzDistToPlayer >= 520.0f) &&
                    (this->actor.xzDistToPlayer <= 900.0f)) {
                    if (play->grabPlayer(play, player)) {
                        f32 dx;
                        f32 dy;
                        f32 dz;
                        f32 distToPlayerXZ;

                        player->actor.parent = &this->actor;
                        AudioSfx_PlaySfx(NA_SE_VO_LI_DAMAGE_S, &player->actor.projectedPos, 4,
                                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                        this->whipWrapStartIndex = 0;
                        this->whipWrapEndOffset = ((this->actor.xzDistToPlayer - 300.0f) / 22.0f) + 10.0f;

                        dx = player->actor.world.pos.x - this->rightWhip.basePos.x;
                        dy = player->actor.world.pos.y - this->rightWhip.basePos.y + 50.0f;
                        dz = player->actor.world.pos.z - this->rightWhip.basePos.z;
                        distToPlayerXZ = sqrtf(SQ(dx) + SQ(dz));

                        this->whipWrapRotY = Math_Atan2F_XY(dz, dx);
                        this->whipWrapRotX = -Math_Atan2F_XY(distToPlayerXZ, dy);
                        this->actionFunc = Boss07_Wrath_GrabPlayer;
                        this->frameCounter = 0;
                        this->rightWhip.tension = 0.0f;
                        Audio_PlaySfx(NA_SE_EN_LAST3_GET_LINK_OLD);
                    }
                } else if ((player->stateFlags1 & PLAYER_STATE1_400000) && Boss07_ArePlayerAndActorFacing(this, play)) {
                    player->pushedSpeed = 10.0f;
                    player->pushedYaw = this->actor.yawTowardsPlayer;
                    Audio_PlaySfx(NA_SE_IT_SHIELD_BOUND);
                    this->whipCollisionTimer = 4;
                } else {
                    func_800B8D50(play, NULL, 5.0f, this->actor.shape.rot.y, 0.0f, 8);
                    this->whipCollisionTimer = 20;
                }

                hitPos = player->actor.world.pos;

                hitPos.x += Rand_CenteredFloat(30.0f);
                hitPos.y += Rand_CenteredFloat(30.0f) + 20.0f;
                hitPos.z += Rand_CenteredFloat(30.0f);

                EffectSsHitmark_SpawnFixedScale(play, 0, &hitPos);
                Boss07_Wrath_SpawnDustAtPos(play, &player->actor.world.pos, 7);
                break;
            }
        }
    }
}

void Boss07_Wrath_UpdateDamage(Boss07* this, PlayState* play) {
    s32 i;
    s32 j;
    u8 damage;
    Player* player = GET_PLAYER(play);

    if (this->damagedTimer != 0) {
        return;
    }

    if (this->kickCollider.elements[MAJORAS_WARTH_KICK_COLLIDER_RIGHT_FOOT].base.atElemFlags & ATELEM_HIT) {
        this->kickCollider.elements[MAJORAS_WARTH_KICK_COLLIDER_RIGHT_FOOT].base.atElemFlags &= ~ATELEM_HIT;
        player->pushedYaw = this->actor.yawTowardsPlayer;
        player->pushedSpeed = 20.0f;
        Boss07_Wrath_SpawnDustAtPos(play, &player->actor.world.pos, 12);
        Audio_PlaySfx(NA_SE_IT_HOOKSHOT_STICK_OBJ);
    }

    for (i = 0; i < ARRAY_COUNT(this->bodyColliderElements); i++) {
        if (!(this->bodyCollider.elements[i].base.acElemFlags & ACELEM_HIT)) {
            continue;
        }

        for (j = 0; j < ARRAY_COUNT(this->bodyColliderElements); j++) {
            this->bodyCollider.elements[j].base.acElemFlags &= ~ACELEM_HIT;
        }

        if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            this->drawDmgEffTimer = 0;
        }

        switch (this->actor.colChkInfo.damageEffect) {
            case MAJORAS_WRATH_DMGEFF_FREEZE:
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_FROZEN_INIT;
                break;

            case MAJORAS_WRATH_DMGEFF_FIRE:
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_FIRE_INIT;
                break;

            case MAJORAS_WRATH_DMGEFF_LIGHT_ORB:
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x, this->actor.focus.pos.y,
                            this->actor.focus.pos.z, 0, 0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                break;

            case MAJORAS_WRATH_DMGEFF_ELECTRIC_SPARKS:
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_INIT;
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                break;

            case MAJORAS_WRATH_DMGEFF_BLUE_LIGHT_ORB:
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_BLUE_LIGHT_ORB_INIT;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x, this->actor.focus.pos.y,
                            this->actor.focus.pos.z, 0, 0, 3, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                break;

            default:
                break;
        }

        damage = this->actor.colChkInfo.damage;

        if ((this->actionFunc == Boss07_Wrath_Stunned) || (this->actionFunc == Boss07_Wrath_Damaged)) {
            if ((this->actionFunc == Boss07_Wrath_Stunned) &&
                (this->actor.colChkInfo.damageEffect != MAJORAS_WRATH_DMGEFF_ANIM_FRAME_CHECK) &&
                (this->actor.colChkInfo.damageEffect != MAJORAS_WRATH_DMGEFF_DAMAGE_NONE) &&
                (this->actor.colChkInfo.damageEffect != MAJORAS_WRATH_DMGEFF_BLUE_LIGHT_ORB) &&
                (this->actor.colChkInfo.damageEffect != MAJORAS_WRATH_DMGEFF_EXPLOSIVE)) {
                Boss07_Wrath_SetupStunned(this, play);
                this->damagedTimer = 6;
            } else {
                this->damagedFlashTimer = 15;
                this->damagedTimer = (this->actor.colChkInfo.damageEffect == MAJORAS_WRATH_DMGEFF_EXPLOSIVE) ? 15 : 5;
                Boss07_Wrath_SetupDamaged(this, play, damage, this->actor.colChkInfo.damageEffect);
            }
        } else {
            this->damagedTimer = 15;
            Boss07_Wrath_SetupStunned(this, play);
            this->whipWrapEndOffset = 0;

            if (&this->actor == player->actor.parent) {
                player->av2.actionVar2 = 101;
                player->actor.parent = NULL;
                player->csAction = PLAYER_CSACTION_NONE;
            }
        }
        break;
    }
}

/**
 * Spawns two ice shards on all of Majora's Wrath's body parts that fly off in random directions.
 */
void Boss07_Wrath_Thaw(Boss07* this, PlayState* play) {
    static Color_RGBA8 sIcePrimColor = { 170, 255, 255, 255 };
    static Color_RGBA8 sIceEnvColor = { 200, 200, 255, 255 };
    static Vec3f sIceAccel = { 0.0f, -1.0f, 0.0f };
    Vec3f pos;
    Vec3f velocity;
    s32 i;

    SoundSource_PlaySfxAtFixedWorldPos(play, &this->bodyPartsPos[MAJORAS_WRATH_BODYPART_HEAD], 30, NA_SE_EV_ICE_BROKEN);

    for (i = 0; i < MAJORAS_WRATH_BODYPART_MAX * 2; i++) {
        velocity.x = Rand_CenteredFloat(7.0f);
        velocity.z = Rand_CenteredFloat(7.0f);
        velocity.y = Rand_ZeroFloat(6.0f) + 4.0f;

        pos.x = this->bodyPartsPos[i / 2].x + velocity.x;
        pos.y = this->bodyPartsPos[i / 2].y + velocity.y;
        pos.z = this->bodyPartsPos[i / 2].z + velocity.z;

        EffectSsEnIce_Spawn(play, &pos, Rand_ZeroFloat(1.0f) + 1.5f, &velocity, &sIceAccel, &sIcePrimColor,
                            &sIceEnvColor, 30);
    }
}

void Boss07_UpdateDamageEffects(Boss07* this, PlayState* play) {
    DECR(this->drawDmgEffTimer);

    switch (this->drawDmgEffState) {
        case MAJORA_DRAW_DMGEFF_STATE_NONE:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
            this->drawDmgEffTimer = 0;
            this->drawDmgEffAlpha = 0.0f;
            break;

        case MAJORA_DRAW_DMGEFF_STATE_FIRE_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
            this->drawDmgEffTimer = 40;
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffScale = 0.0f;
            this->drawDmgEffState++;
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 120, COLORFILTER_BUFFLAG_OPA, 60);
            FALLTHROUGH;
        case MAJORA_DRAW_DMGEFF_STATE_FIRE_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Math_ApproachZeroF(&this->drawDmgEffAlpha, 1.0f, 0.02f);
                if (this->drawDmgEffAlpha == 0.0f) {
                    this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_NONE;
                }
            } else {
                Math_ApproachF(&this->drawDmgEffScale, 1.0f, 0.1f, 0.5f);
            }
            break;

        case MAJORA_DRAW_DMGEFF_STATE_FROZEN_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
            this->drawDmgEffTimer = 40;
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffFrozenSteamScale = 1.0f;
            this->drawDmgEffScale = 0.0f;
            this->drawDmgEffState++;
            FALLTHROUGH;
        case MAJORA_DRAW_DMGEFF_STATE_FROZEN_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Boss07_Wrath_Thaw(this, play);
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_NONE;
                break;
            }

            if (this->drawDmgEffTimer == 50) {
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
            }

            Math_ApproachF(&this->drawDmgEffScale, 1.0f, 1.0f, 0.08f);
            Math_ApproachF(&this->drawDmgEffFrozenSteamScale, 1.0f, 0.05f, 0.05f);
            break;

        case MAJORA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
            this->drawDmgEffTimer = 40;
            this->drawDmgEffScale = 1.0f;
            goto lightOrbInitCommon;

        case MAJORA_DRAW_DMGEFF_STATE_BLUE_LIGHT_ORB_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS;
            this->drawDmgEffTimer = 40;
            this->drawDmgEffScale = 3.0f;
        lightOrbInitCommon:
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_LIGHT_ORB_ACTIVE;
            FALLTHROUGH;
        case MAJORA_DRAW_DMGEFF_STATE_LIGHT_ORB_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Math_ApproachZeroF(&this->drawDmgEffScale, 1.0f, 0.03f);
                if (this->drawDmgEffScale == 0.0f) {
                    this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_NONE;
                    this->drawDmgEffAlpha = 0.0f;
                }
            } else {
                Math_ApproachF(&this->drawDmgEffScale, 1.5f, 0.5f, 0.5f);
            }
            break;

        case MAJORA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
            this->drawDmgEffTimer = 50;
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffScale = (KREG(18) * 0.1f) + 1.0f;
            this->drawDmgEffState++;
            FALLTHROUGH;
        case MAJORA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Math_ApproachZeroF(&this->drawDmgEffScale, 1.0f, 0.05f);
                if (this->drawDmgEffScale == 0.0f) {
                    this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_NONE;
                    this->drawDmgEffAlpha = 0.0f;
                }
            }
            break;

        default:
            break;
    }
}

void Boss07_Wrath_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;
    s32 i;
    Player* player = GET_PLAYER(play);

    play->envCtx.lightSetting = 2;
    play->envCtx.prevLightSetting = 0;
    this->actor.hintId = TATL_HINT_ID_MAJORAS_WRATH;
    Math_ApproachF(&play->envCtx.lightBlend, 0.0f, 1.0f, 0.03f);
    this->shouldUpdateTentaclesOrWhips = true;
    Math_Vec3f_Copy(&sMajoraSfxPos, &this->actor.projectedPos);

    if (KREG(63) == 0) {
        this->canEvade = false;
        this->frameCounter++;
        Actor_SetScale(&this->actor, 0.015f);
        Math_ApproachZeroF(&this->wrathLeanRotY, 1.0f, 0.02f);
        Math_ApproachZeroF(&this->wrathLeanRotX, 1.0f, 0.02f);

        for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
            DECR(this->timers[i]);
        }

        DECR(this->whipCollisionTimer);
        DECR(this->disableCollisionTimer);
        DECR(this->damagedTimer);
        DECR(this->damagedFlashTimer);
        DECR(this->landSfxTimer);

        Math_ApproachZeroF(&this->shockOrbScale, 1.0f, 0.2f);
        Math_ApproachZeroF(&this->shockSparkScale, 1.0f, 0.04f);

        this->actionFunc(this, play);

        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 100.0f, 100.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

        if (this->whipCrackTimer != 0) {
            this->whipCrackTimer--;

            if ((this->actionFunc == Boss07_Wrath_Attack) && (this->whipCrackTimer == 0)) {
                Audio_PlaySfx(NA_SE_EN_LAST3_ROD_FLOOR_OLD);
            }
        }
    } else {
        sWhipLength = 45;
    }

    Boss07_Wrath_CheckWhipCollisions(this->rightWhip.pos, this->rightWhip.tension, this, play);
    Boss07_Wrath_CheckWhipCollisions(this->leftWhip.pos, this->leftWhip.tension, this, play);

    if (this->disableCollisionTimer != 0) {
        for (i = 0; i < ARRAY_COUNT(this->bodyColliderElements); i++) {
            this->bodyCollider.elements[i].base.acElemFlags &= ~ACELEM_HIT;
        }
    }

    Boss07_Wrath_UpdateDamage(this, play);

    if (this->damagedTimer != 0) {
        this->whipTopIndex = 0;
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);

    if ((this->actionFunc == Boss07_Wrath_Attack) && (this->subAction == MAJORAS_WRATH_ATTACK_SUB_ACTION_KICK) &&
        (this->frameCounter >= 6)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->kickCollider.base);
    } else {
        if (this->canEvade && Boss07_ArePlayerAndActorFacing(this, play)) {
            if ((player->unk_D57 == 4) && (player->heldItemAction != PLAYER_IA_BOW_LIGHT)) {
                if ((this->actor.xzDistToPlayer >= 400.0f) && (Rand_ZeroOne() < 0.5f)) {
                    Boss07_Wrath_SetupSidestep(this, play);
                } else {
                    Boss07_Wrath_ChooseJump(this, play, false);
                }
            }

            if ((player->unk_ADC != 0) && (this->actor.xzDistToPlayer <= 150.0f)) {
                Boss07_Wrath_ChooseJump(this, play, false);
            }
        }

        if ((this->actionFunc != Boss07_Wrath_Stunned) && (this->actionFunc != Boss07_Wrath_Damaged)) {
            if ((player->stateFlags3 & PLAYER_STATE3_1000) && !(player->stateFlags3 & PLAYER_STATE3_80000) &&
                (this->actor.xzDistToPlayer <= 250.0f)) {
                Boss07_Wrath_ChooseJump(this, play, false);
            }
        }
    }

    if (this->canEvade) {
        Boss07_Wrath_JumpAwayFromExplosive(this, play);
    }

    Boss07_Wrath_CheckBombWhips(this, play);

    if (KREG(88) || this->shouldStartDeath) {
        KREG(88) = false;
        this->shouldStartDeath = false;
        Boss07_Wrath_SetupDeathCutscene(this, play);
    }

    if (this->maxDecayPixels != 0) {
        // #region 2S2H [Port] CPU Modified textures
        // Instead of modifying the textures directly, we modify masks that match the texture sizes
        for (i = 0; i < this->maxDecayPixels; i++) {
            s32 rand32x64;
            s32 rand32x16;
            s32 rand32x32;
            s32 rand16x16;

            rand16x16 = Rand_ZeroFloat((16 * 16) - 0.01f);
            rand32x16 = Rand_ZeroFloat((32 * 16) - 0.01f);
            rand32x32 = Rand_ZeroFloat((32 * 32) - 0.01f);
            rand32x64 = Rand_ZeroFloat((32 * 64) - 0.01f);

            sWrathTexMask16by16[rand16x16] = sWrathTexMask16by32[rand32x16] = sWrathTexMask32by32[rand32x32] =
                sWrathTexMask32by64[rand32x64] = 1;
        }
        // #endregion
    }

    Boss07_UpdateDamageEffects(this, play);

    if ((this->landSfxTimer == 1) || (this->landSfxTimer == 4)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_WALK2_OLD);
    }

    if ((player->actor.world.pos.y > 100.0f) && (player->actor.world.pos.z < (KREG(82) + -850.0f))) {
        player->actor.world.pos.z = KREG(82) + -850.0f;
    }

    if (player->actor.world.pos.y < -300.0f) {
        player->actor.world.pos.x = player->actor.world.pos.y = player->actor.world.pos.z = 0.0f;
    }
}

/**
 * The whip physics model works by first assuming each point moves freely, using its current velocity and various forces
 * to find the next position. Then, starting from the base of the whip, each point is moved away from the previous point
 * until it is a fixed distance away. The new velocity of each point is found from the scaled difference between its
 * updated position and previous position. The forces applied are
 * - deceleration, which reduces the magnitude of each velocity component by a fixed amount (or to zero if smaller)
 * - gravity, which applies to the y-component of velocity
 * - tension, an outward force that makes the whip "crack"
 * - additional forces for special movements like wrapping around Link
 * The scaling factor of the velocity is the whip's mobility--how well movements at the base are transmitted to the
 * ends.
 *
 * The algorithm used to implement this physics model is as follows:
 * - Set the position of the 0th point to the position of Wrath's hand (base)
 * - Move each component of each point's velocity towards 0 by deceleration. Set that component to zero if the magnitude
 * is less than deceleration.
 * - Calculate the tension force. This points away from Wrath's hand (baserot) for whip movements and towards Link when
 * grabbed
 * - Calculate shaping forces. These are the forces from Wrath's hand on the whip base and the force that wraps the whip
 * around Link. Set tension to 0 for the latter segments.
 * - Calculate the provisional position of the point by applying the velocity, forces, and gravity. For the y-component,
 * set a minimum of 2 to not clip through the floor.
 * - Subtract the current position of the previous point along the whip from the calculated position to get the relative
 * vector between them
 * - Set this vector's rotation to the previous point's rot (used by draw so the segments connect).
 * - Calculate a new vector with the same direction and length 22*scale. Add this to the previous point's position to
 * get the final position of the point.
 * - For grab and throw movements, update the position of the grab point.
 * - The point's final velocity is the difference between its current and previous positions, scaled by mobility and
 * with each component's magnitude capped at 200.
 */

void Boss07_Wrath_UpdateWhips(Boss07* this, PlayState* play, Vec3f* base, Vec3f* pos, Vec3f* rot, Vec3f* velocity,
                              f32 gravity, f32 mobility, f32 deceleration, f32 tension, Vec3s* baseRot, s16 grabIndex,
                              f32 scale, s32 hand) {
    s32 i;
    s32 j;
    f32 segPitch;
    f32 segYaw;
    f32 tempY;
    s32 pad;
    Vec3f tempPos;
    Vec3f baseSegVec = { 0.0f, 0.0f, 0.0f };
    Vec3f offsetVec;
    Vec3f segVec;
    Vec3f handForce;
    Vec3f shapeForce;
    Vec3f tensForce;
    Vec3f* firstVelocity = velocity;

    // sets the fixed length of each whip segment
    baseSegVec.z = 22.0f * scale;

    for (i = 0; i < (s16)sWhipLength; i++, velocity++) {
        if (i == 0) {
            pos[0] = *base;
        } else {
            Math_ApproachF(&velocity->x, 0.0f, 1.0f, deceleration);
            Math_ApproachF(&velocity->y, 0.0f, 1.0f, deceleration);
            Math_ApproachF(&velocity->z, 0.0f, 1.0f, deceleration);
        }
    }

    offsetVec.z = 20.0f;

    // calculates rotation away from Wrath's hand
    Matrix_RotateYS(baseRot->y, MTXMODE_NEW);
    Matrix_RotateXS(baseRot->x, MTXMODE_APPLY);
    Matrix_RotateZS(baseRot->z, MTXMODE_APPLY);
    Matrix_RotateYS(0x4000, MTXMODE_APPLY);

    Matrix_MultVecZ(offsetVec.z, &handForce);

    if (grabIndex != 0) {
        offsetVec.z = 200.0f;
        // replaces the tension direction with the direction from Wrath's hand to Link
        Matrix_RotateYF(this->whipWrapRotY, MTXMODE_NEW);
        Matrix_RotateXFApply(this->whipWrapRotX);
    } else {
        offsetVec.z = tension;
    }

    Matrix_MultVecZ(offsetVec.z, &tensForce);

    pos++;
    rot++;
    velocity = firstVelocity + 1;
    j = 0;

    for (i = 1; i < (s16)sWhipLength; i++, velocity++, pos++, rot++) {
        if (i < 6) {
            // keeps the whip outward from Wrath's hand near the base
            shapeForce.x = (6 - i) * handForce.x * 0.2f;
            shapeForce.y = (6 - i) * handForce.y * 0.2f;
            shapeForce.z = (6 - i) * handForce.z * 0.2f;
        } else if (grabIndex != 0) {
            // wraps the whip around Link
            if ((i >= grabIndex) && (i < (s16)(grabIndex + this->whipWrapStartIndex))) {
                shapeForce.y = 0.0f;
                shapeForce.x = sinf(this->actor.yawTowardsPlayer * M_PIf / 0x8000 + (j * 1.4f)) * 100.0f;
                shapeForce.z = cosf(this->actor.yawTowardsPlayer * M_PIf / 0x8000 + (j * 1.4f)) * 100.0f;
                tensForce = gZeroVec3f;
                j++;
            }
        } else {
            shapeForce = gZeroVec3f;
        }

        // Advances the physics one frame to find the provisional position relative to the previous point
        tempPos.x = pos->x + velocity->x - (pos - 1)->x + shapeForce.x + tensForce.x;
        tempY = pos->y + velocity->y + gravity + shapeForce.y + tensForce.y;
        if (tempY < 2.0f) {
            tempY = 2.0f;
        }

        tempPos.y = tempY - (pos - 1)->y;
        tempPos.z = pos->z + velocity->z - (pos - 1)->z + shapeForce.z + tensForce.z;

        // calculates the rotation angles from the previous point
        segYaw = Math_Atan2F_XY(tempPos.z, tempPos.x);
        segPitch = -Math_Atan2F_XY(sqrtf(SQXZ(tempPos)), tempPos.y);
        (rot - 1)->y = segYaw;
        (rot - 1)->x = segPitch;

        // Sets the position to be in the same direction as the provisional position relative to the previous point, but
        // a fixed distance away
        Matrix_RotateYF(segYaw, MTXMODE_NEW);
        Matrix_RotateXFApply(segPitch);
        Matrix_MultVecZ(baseSegVec.z, &segVec);

        tempPos.x = pos->x;
        tempPos.y = pos->y;
        tempPos.z = pos->z;

        pos->x = (pos - 1)->x + segVec.x;
        pos->y = (pos - 1)->y + segVec.y;
        pos->z = (pos - 1)->z + segVec.z;

        if ((i != 0) && (i == grabIndex)) {
            // Updates the grab point for when Link is grabbed
            offsetVec.x = 15.0f;
            offsetVec.y = -30.0f;
            offsetVec.z = -12.0f;
            Matrix_MultVec3f(&offsetVec, &segVec);
            this->whipGrabPos.x = pos->x + segVec.x;
            this->whipGrabPos.y = pos->y + segVec.y;
            this->whipGrabPos.z = pos->z + segVec.z;
        } else if ((hand == MAJORAS_WRATH_HAND_RIGHT) && (this->actionFunc == Boss07_Wrath_ThrowTop)) {
            // Updates the grab point when throwing a top
            if (i == (KREG(90) + sWhipLength - this->whipTopIndex + 1)) {
                offsetVec.x = KREG(60);
                offsetVec.y = KREG(61);
                offsetVec.z = KREG(62);
                Matrix_MultVec3f(&offsetVec, &segVec);
                this->whipGrabPos.x = pos->x + segVec.x;
                this->whipGrabPos.y = pos->y + segVec.y;
                this->whipGrabPos.z = pos->z + segVec.z;
            }
        }

        // calculates the velocity for the next frame
        velocity->x = (pos->x - tempPos.x) * mobility;
        velocity->y = (pos->y - tempPos.y) * mobility;
        velocity->z = (pos->z - tempPos.z) * mobility;

        if (velocity->x > 200.0f) {
            velocity->x = 200.0f;
        }

        if (velocity->x < -200.0f) {
            velocity->x = -200.0f;
        }

        if (velocity->y > 200.0f) {
            velocity->y = 200.0f;
        }

        if (velocity->y < -200.0f) {
            velocity->y = -200.0f;
        }

        if (velocity->z > 200.0f) {
            velocity->z = 200.0f;
        }

        if (velocity->z < -200.0f) {
            velocity->z = -200.0f;
        }
    }
}

void Boss07_Wrath_DrawWhip(Boss07* this, PlayState* play, Vec3f* pos, Vec3f* rot, f32 lengthScale, s32 hand) {
    s32 topSegIndex;
    s32 i;
    f32 thicknessScale;
    s32 pad[2];
    Vec3f* prevPos = pos;
    Vec3f* prevRot = rot;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gMajorasMaskWhipShadowMaterialDL);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sREG(48) + 45, 35, 75, 255);
    topSegIndex = (hand == MAJORAS_WRATH_HAND_RIGHT) ? this->whipTopIndex : 0;

    for (i = 0; i < (sWhipLength - topSegIndex) - 1; i++, pos++, rot++) {
        Matrix_Translate(pos->x, 0.0f, pos->z, MTXMODE_NEW);
        Matrix_Scale(1.0f, 0.0f, 1.0f, MTXMODE_APPLY);
        Matrix_RotateYF(rot->y, MTXMODE_APPLY);
        Matrix_RotateXFApply(rot->x);
        thicknessScale = (i > 24) ? 0.025f : ((f32)(24 - i) * 1 * 0.001f) + 0.025f;
        Matrix_Scale(thicknessScale, thicknessScale, ((2 * lengthScale) + 0.5f) * 0.01f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gMajorasWrathWhipModelDL);
    }

    pos = prevPos;
    rot = prevRot;
    gSPDisplayList(POLY_OPA_DISP++, gMajorasWrathWhipMaterialDL);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 155, 155, 80, 255);

    for (i = 0; i < (sWhipLength - topSegIndex) - 1; i++, pos++, rot++) {
        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);
        Matrix_RotateYF(rot->y, MTXMODE_APPLY);
        Matrix_RotateXFApply(rot->x);
        thicknessScale = (i > 24) ? 0.025f : ((f32)(24 - i) * 1 * 0.001f) + 0.025f;
        Matrix_Scale(thicknessScale, thicknessScale, ((2 * lengthScale) + 0.5f) * 0.01f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gMajorasWrathWhipModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 Boss07_Wrath_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    Boss07* this = (Boss07*)thisx;

    if (limbIndex == MAJORAS_WRATH_LIMB_HEAD) {
        rot->x += this->cutsceneHeadRot.y;
        rot->y += this->cutsceneHeadRot.x;
        rot->z += this->cutsceneHeadRot.z;
    }

    return false;
}

static s8 sWrathLimbToColliderBodyParts[] = {
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_NONE
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_PELVIS,          // MAJORAS_WRATH_LIMB_PELVIS
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_LEFT_LEG_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_LEFT_THIGH,      // MAJORAS_WRATH_LIMB_LEFT_THIGH
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_LEFT_LOWER_LEG_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_LEFT_SHIN,       // MAJORAS_WRATH_LIMB_LEFT_SHIN
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_LEFT_FOOT
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_RIGHT_LEG_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_RIGHT_THIGH,     // MAJORAS_WRATH_LIMB_RIGHT_THIGH
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_RIGHT_LOWER_LEG_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_RIGHT_SHIN,      // MAJORAS_WRATH_LIMB_RIGHT_SHIN
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_RIGHT_FOOT
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_TORSO_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_TORSO,           // MAJORAS_WRATH_LIMB_TORSO
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_RIGHT_ARM_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_RIGHT_UPPER_ARM, // MAJORAS_WRATH_LIMB_RIGHT_UPPER_ARM
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_RIGHT_LOWER_ARM_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_RIGHT_FOREARM,   // MAJORAS_WRATH_LIMB_RIGHT_FOREARM
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_RIGHT_HAND
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_LEFT_ARM_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_LEFT_UPPER_ARM,  // MAJORAS_WRATH_LIMB_LEFT_UPPER_ARM
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_LEFT_LOWER_ARM_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_LEFT_FOREARM,    // MAJORAS_WRATH_LIMB_LEFT_FOREARM
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_LEFT_HAND
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_HEAD_ROOT
    MAJORAS_WRATH_COLLIDER_BODYPART_HEAD,            // MAJORAS_WRATH_LIMB_HEAD
    BODYPART_NONE,                                   // MAJORAS_WRATH_LIMB_THIRD_EYE
    BODYPART_NONE,                                   // Doesn't correspond to a real limb on Majora's Wrath
    BODYPART_NONE,                                   // Doesn't correspond to a real limb on Majora's Wrath
};

static Vec3f sWrathLimbColliderOffsets[MAJORAS_WRATH_COLLIDER_BODYPART_MAX] = {
    { 1000.0f, 0.0f, 500.0f }, // MAJORAS_WRATH_COLLIDER_BODYPART_HEAD
    { 2000.0f, 0.0f, 0.0f },   // MAJORAS_WRATH_COLLIDER_BODYPART_TORSO
    { 1000.0f, 0.0f, 500.0f }, // MAJORAS_WRATH_COLLIDER_BODYPART_PELVIS
    { 2000.0f, 0.0f, 0.0f },   // MAJORAS_WRATH_COLLIDER_BODYPART_LEFT_UPPER_ARM
    { 2000.0f, 0.0f, 0.0f },   // MAJORAS_WRATH_COLLIDER_BODYPART_LEFT_FOREARM
    { 2000.0f, 0.0f, 0.0f },   // MAJORAS_WRATH_COLLIDER_BODYPART_RIGHT_UPPER_ARM
    { 2000.0f, 0.0f, 0.0f },   // MAJORAS_WRATH_COLLIDER_BODYPART_RIGHT_FOREARM
    { 2000.0f, 0.0f, 0.0f },   // MAJORAS_WRATH_COLLIDER_BODYPART_LEFT_THIGH
    { 2000.0f, 0.0f, 0.0f },   // MAJORAS_WRATH_COLLIDER_BODYPART_LEFT_SHIN
    { 2000.0f, 0.0f, 0.0f },   // MAJORAS_WRATH_COLLIDER_BODYPART_RIGHT_THIGH
    { 2000.0f, 0.0f, 0.0f },   // MAJORAS_WRATH_COLLIDER_BODYPART_RIGHT_SHIN
};

static s8 sWrathLimbToBodyParts[] = {
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_NONE
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_ROOT
    MAJORAS_WRATH_BODYPART_PELVIS,               // MAJORAS_WRATH_LIMB_PELVIS
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_LEFT_LEG_ROOT
    MAJORAS_WRATH_BODYPART_LEFT_THIGH,           // MAJORAS_WRATH_LIMB_LEFT_THIGH
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_LEFT_LOWER_LEG_ROOT
    MAJORAS_WRATH_BODYPART_LEFT_SHIN,            // MAJORAS_WRATH_LIMB_LEFT_SHIN
    MAJORAS_WRATH_BODYPART_LEFT_FOOT,            // MAJORAS_WRATH_LIMB_LEFT_FOOT
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_RIGHT_LEG_ROOT
    MAJORAS_WRATH_BODYPART_RIGHT_THIGH,          // MAJORAS_WRATH_LIMB_RIGHT_THIGH
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_RIGHT_LOWER_LEG_ROOT
    MAJORAS_WRATH_BODYPART_RIGHT_SHIN,           // MAJORAS_WRATH_LIMB_RIGHT_SHIN
    MAJORAS_WRATH_BODYPART_RIGHT_FOOT,           // MAJORAS_WRATH_LIMB_RIGHT_FOOT
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_TORSO_ROOT
    MAJORAS_WRATH_BODYPART_TORSO,                // MAJORAS_WRATH_LIMB_TORSO
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_RIGHT_ARM_ROOT
    MAJORAS_WRATH_BODYPART_RIGHT_UPPER_ARM,      // MAJORAS_WRATH_LIMB_RIGHT_UPPER_ARM
    MAJORAS_WRATH_BODYPART_RIGHT_LOWER_ARM_ROOT, // MAJORAS_WRATH_LIMB_RIGHT_LOWER_ARM_ROOT
    MAJORAS_WRATH_BODYPART_RIGHT_FOREARM,        // MAJORAS_WRATH_LIMB_RIGHT_FOREARM
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_RIGHT_HAND
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_LEFT_ARM_ROOT
    MAJORAS_WRATH_BODYPART_LEFT_UPPER_ARM,       // MAJORAS_WRATH_LIMB_LEFT_UPPER_ARM
    MAJORAS_WRATH_BODYPART_LEFT_LOWER_ARM_ROOT,  // MAJORAS_WRATH_LIMB_LEFT_LOWER_ARM_ROOT
    MAJORAS_WRATH_BODYPART_LEFT_FOREARM,         // MAJORAS_WRATH_LIMB_LEFT_FOREARM
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_LEFT_HAND
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_HEAD_ROOT
    MAJORAS_WRATH_BODYPART_HEAD,                 // MAJORAS_WRATH_LIMB_HEAD
    BODYPART_NONE,                               // MAJORAS_WRATH_LIMB_THIRD_EYE
    BODYPART_NONE,                               // Doesn't correspond to a real limb on Majora's Wrath
    BODYPART_NONE,                               // Doesn't correspond to a real limb on Majora's Wrath
};

void Boss07_Wrath_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sWhipOffset = { 1000.0f, 100.0f, 0.0f };
    Boss07* this = (Boss07*)thisx;
    s8 bodyPartIndex;
    Vec3f colliderPos;
    MtxF curMtxF;

    bodyPartIndex = sWrathLimbToBodyParts[limbIndex];
    if (bodyPartIndex > BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[bodyPartIndex]);
    }

    bodyPartIndex = sWrathLimbToColliderBodyParts[limbIndex];
    if (bodyPartIndex > BODYPART_NONE) {
        if (this->disableCollisionTimer != 0) {
            Matrix_MultVecZ(100000.0f, &colliderPos);
        } else {
            Matrix_MultVec3f(&sWrathLimbColliderOffsets[bodyPartIndex], &colliderPos);
        }

        Boss07_SetColliderSphere(bodyPartIndex, &this->bodyCollider, &colliderPos);
    }

    if (limbIndex == MAJORAS_WRATH_LIMB_HEAD) {
        Matrix_MultZero(&this->actor.focus.pos);
    }

    if (limbIndex == MAJORAS_WRATH_LIMB_RIGHT_HAND) {
        Matrix_MultVec3f(&sWhipOffset, &this->rightWhip.basePos);
    }

    if (limbIndex == MAJORAS_WRATH_LIMB_RIGHT_LOWER_ARM_ROOT) {
        Matrix_Get(&curMtxF);
        Matrix_MtxFToYXZRot(&curMtxF, &this->rightWhip.baseRot, false);
    }

    if (limbIndex == MAJORAS_WRATH_LIMB_LEFT_HAND) {
        Matrix_MultVec3f(&sWhipOffset, &this->leftWhip.basePos);
    }

    if (limbIndex == MAJORAS_WRATH_LIMB_LEFT_LOWER_ARM_ROOT) {
        Matrix_Get(&curMtxF);
        Matrix_MtxFToYXZRot(&curMtxF, &this->leftWhip.baseRot, false);
    }

    if (limbIndex == MAJORAS_WRATH_LIMB_RIGHT_FOOT) {
        Matrix_MultZero(&colliderPos);
        Boss07_SetColliderSphere(MAJORAS_WARTH_KICK_COLLIDER_RIGHT_FOOT, &this->kickCollider, &colliderPos);
    }
}

void Boss07_Wrath_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    Boss07* this = (Boss07*)thisx;

    // This code allows the `incarnationArmScale` to scale up almost all of Wrath's limbs. However, in the final game,
    // this variable is always set to 1.0f for Wrath, meaning that this code effectively does nothing.
    if ((limbIndex == MAJORAS_WRATH_LIMB_PELVIS) || (limbIndex == MAJORAS_WRATH_LIMB_LEFT_THIGH) ||
        (limbIndex == MAJORAS_WRATH_LIMB_LEFT_SHIN) || (limbIndex == MAJORAS_WRATH_LIMB_LEFT_FOOT) ||
        (limbIndex == MAJORAS_WRATH_LIMB_RIGHT_THIGH) || (limbIndex == MAJORAS_WRATH_LIMB_RIGHT_SHIN) ||
        (limbIndex == MAJORAS_WRATH_LIMB_RIGHT_FOOT) || (limbIndex == MAJORAS_WRATH_LIMB_RIGHT_UPPER_ARM) ||
        (limbIndex == MAJORAS_WRATH_LIMB_RIGHT_FOREARM) || (limbIndex == MAJORAS_WRATH_LIMB_RIGHT_HAND) ||
        (limbIndex == MAJORAS_WRATH_LIMB_LEFT_UPPER_ARM) || (limbIndex == MAJORAS_WRATH_LIMB_LEFT_FOREARM) ||
        (limbIndex == MAJORAS_WRATH_LIMB_LEFT_HAND) || (limbIndex == MAJORAS_WRATH_LIMB_HEAD)) {
        Matrix_Scale(1.0f, this->incarnationArmScale, this->incarnationArmScale, MTXMODE_APPLY);
    }
}

void Boss07_Wrath_DrawShocks(Boss07* this, PlayState* play) {
    s32 i;
    GraphicsContext* gfxCtx;

    OPEN_DISPS(gfxCtx = play->state.gfxCtx);

    if ((this->shockOrbScale > 0.0f) || (this->shockSparkScale > 0.0f)) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);

        if (this->shockOrbScale > 0.0f) {
            gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);

            for (i = 0; i < MAJORAS_WRATH_BODYPART_MAX; i++) {
                Matrix_Translate(this->bodyPartsPos[i].x, this->bodyPartsPos[i].y, this->bodyPartsPos[i].z,
                                 MTXMODE_NEW);
                Matrix_ReplaceRotation(&play->billboardMtxF);
                Matrix_Scale(this->shockOrbScale, this->shockOrbScale, this->shockOrbScale, MTXMODE_APPLY);
                Matrix_RotateZF(Rand_ZeroFloat(2.0f * M_PIf), MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
            }

            for (i = this->whipShockMaxIndex; i >= this->whipShockMinIndex; i--) {
                Matrix_Translate(this->rightWhip.pos[i].x, this->rightWhip.pos[i].y, this->rightWhip.pos[i].z,
                                 MTXMODE_NEW);
                Matrix_ReplaceRotation(&play->billboardMtxF);
                Matrix_Scale(1.5f, 1.5f, 1.5f, MTXMODE_APPLY);
                Matrix_RotateZF(Rand_ZeroFloat(2.0f * M_PIf), MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
            }
        }

        if (this->shockSparkScale > 0.0f) {
            gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial2DL);

            for (i = 0; i < MAJORAS_WRATH_BODYPART_MAX * 2; i++) {
                Matrix_Translate(this->bodyPartsPos[i / 2].x + Rand_CenteredFloat(30.0f),
                                 this->bodyPartsPos[i / 2].y + Rand_CenteredFloat(30.0f),
                                 this->bodyPartsPos[i / 2].z + Rand_CenteredFloat(30.0f), MTXMODE_NEW);
                Matrix_Scale(this->shockSparkScale, this->shockSparkScale, this->shockSparkScale, MTXMODE_APPLY);
                Matrix_RotateXFApply(Rand_ZeroFloat(2.0f * M_PIf));
                Matrix_RotateZF(Rand_ZeroFloat(2.0f * M_PIf), MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
            }
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void Boss07_Wrath_DrawDeathLights(Boss07* this, PlayState* play, Vec3f* pos) {
    s32 i;
    f32 scale;
    f32 yOffset;
    GraphicsContext* gfxCtx;
    s16* color;

    OPEN_DISPS(gfxCtx = play->state.gfxCtx);

    if (this->deathOrbScale > 0.0f) {
        Boss07_InitRand(1, 0x71B8, 0x263A);
        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 20);
        gDPSetCombineMode(POLY_XLU_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);

        for (i = 0; i < ARRAY_COUNT(this->deathLightScale); i++) {
            color = sProjectileEnvColors[0];
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, color[0], color[1], color[2], 40);
            yOffset = (Boss07_RandZeroOne() * 40.0f) - 30.0f;
            Matrix_Translate(this->bodyPartsPos[MAJORAS_WRATH_BODYPART_PELVIS].x,
                             this->bodyPartsPos[MAJORAS_WRATH_BODYPART_PELVIS].y - 30.0f + 50.0f + yOffset + 25.0f,
                             this->bodyPartsPos[MAJORAS_WRATH_BODYPART_PELVIS].z, MTXMODE_NEW);
            Matrix_Translate(pos->x, pos->y + yOffset, pos->z, MTXMODE_NEW);
            Matrix_RotateYF(Boss07_RandZeroOne() * M_PIf * 2.0f, MTXMODE_APPLY);
            Matrix_RotateXFApply(-250.0f * 0.0001f * yOffset);
            Matrix_RotateZF(Boss07_RandZeroOne() * M_PIf * 2.0f, MTXMODE_APPLY);

            if (this->deathLightScale[i] > 0.0f) {
                Matrix_Scale(this->deathLightScale[i], 1.0f, 12.0f, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gMajorasWrathDeathLightModelDL);
            }
        }

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, (u8)(220 + sREG(18)), (u8)(170 + sREG(16)));
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, (u8)(100 + sREG(22)), 128);

        Matrix_Translate(this->bodyPartsPos[MAJORAS_WRATH_BODYPART_PELVIS].x,
                         this->bodyPartsPos[MAJORAS_WRATH_BODYPART_PELVIS].y,
                         this->bodyPartsPos[MAJORAS_WRATH_BODYPART_PELVIS].z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_RotateZS(play->gameplayFrames * 0x80, MTXMODE_APPLY);
        scale = (sREG(17) + 800) * 0.01f * this->deathOrbScale;
        Matrix_Scale(scale, scale, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
    }

    CLOSE_DISPS(gfxCtx);
}

void Boss07_BattleHandler_DrawIntroPlayerLightOrb(Boss07* this, PlayState* play) {
    s32 pad;
    GraphicsContext* gfxCtx;
    f32 yOffset;
    f32 zOffset;
    Player* player;

    OPEN_DISPS(gfxCtx = play->state.gfxCtx);

    player = GET_PLAYER(play);

    if (this->introPlayerOrbScale > 0.0f) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)(sREG(52) + 255));
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
        gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);

        if (player->transformation == PLAYER_FORM_GORON) {
            yOffset = -10.0f;
            zOffset = -20.0f;
        } else {
            yOffset = zOffset = 0.0f;
        }

        if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
            yOffset -= 43.0f;
        }

        Matrix_Translate(player->actor.world.pos.x,
                         player->actor.world.pos.y + Player_GetHeight(player) - 20.0f + yOffset + sREG(60),
                         player->actor.world.pos.z + sREG(61) - 15.0f + zOffset, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);

        Matrix_Scale(this->introPlayerOrbScale, this->introPlayerOrbScale, this->introPlayerOrbScale, MTXMODE_APPLY);
        Matrix_RotateZS(play->gameplayFrames * 0x40, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
    }

    CLOSE_DISPS(gfxCtx);
}

void Boss07_Wrath_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;
    u8* tex = GRAPH_ALLOC(play->state.gfxCtx, MAJORAS_WRATH_SHADOW_TEX_SIZE);

    OPEN_DISPS(play->state.gfxCtx);

    // #2S2H [Port] Invalidate the blend masks when they are set in the cutscene
    if (this->cutsceneState == MAJORAS_WRATH_DEATH_CS_STATE_FLOAT) {
        gSPInvalidateTexCache(POLY_OPA_DISP++, sWrathTexMask16by16);
        gSPInvalidateTexCache(POLY_OPA_DISP++, sWrathTexMask16by32);
        gSPInvalidateTexCache(POLY_OPA_DISP++, sWrathTexMask32by32);
        gSPInvalidateTexCache(POLY_OPA_DISP++, sWrathTexMask32by64);
    }
    // #enregion

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    if ((this->damagedFlashTimer % 2) != 0) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
    }

    Matrix_RotateYF(this->wrathLeanRotY, MTXMODE_APPLY);
    Matrix_RotateXFApply(this->wrathLeanRotX);
    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, Boss07_Wrath_OverrideLimbDraw, Boss07_Wrath_PostLimbDraw,
                                   Boss07_Wrath_TransformLimbDraw, &this->actor);
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    if (((KREG(63) == 0) || (KREG(63) == 2)) && this->shouldUpdateTentaclesOrWhips) {
        Boss07_Wrath_UpdateWhips(this, play, &this->rightWhip.basePos, this->rightWhip.pos, this->rightWhip.rot,
                                 this->rightWhip.velocity, this->rightWhip.gravity, this->rightWhip.mobility,
                                 this->rightWhip.deceleration, this->rightWhip.tension, &this->rightWhip.baseRot,
                                 this->whipWrapEndOffset, this->whipLengthScale, MAJORAS_WRATH_HAND_RIGHT);
        Boss07_Wrath_UpdateWhips(this, play, &this->leftWhip.basePos, this->leftWhip.pos, this->leftWhip.rot,
                                 this->leftWhip.velocity, this->leftWhip.gravity, this->leftWhip.mobility,
                                 this->leftWhip.deceleration, this->leftWhip.tension, &this->leftWhip.baseRot, 0,
                                 this->whipLengthScale, MAJORAS_WRATH_HAND_LEFT);
    }

    Boss07_Wrath_DrawWhip(this, play, this->rightWhip.pos, this->rightWhip.rot, this->whipLengthScale,
                          MAJORAS_WRATH_HAND_RIGHT);
    Boss07_Wrath_DrawWhip(this, play, this->leftWhip.pos, this->leftWhip.rot, this->whipLengthScale,
                          MAJORAS_WRATH_HAND_LEFT);

    if (!this->disableShadow) {
        Boss07_Wrath_GenShadowTex(tex, this, play);
        Boss07_Wrath_DrawShadowTex(tex, this, play);
    }

    Boss07_Wrath_DrawShocks(this, play);

    {
        Vec3f lightsPos;

        lightsPos.x = this->bodyPartsPos[MAJORAS_WRATH_BODYPART_PELVIS].x;
        lightsPos.y = this->bodyPartsPos[MAJORAS_WRATH_BODYPART_PELVIS].y - 30.0f + 50.0f;
        lightsPos.z = this->bodyPartsPos[MAJORAS_WRATH_BODYPART_PELVIS].z;
        Boss07_Wrath_DrawDeathLights(this, play, &lightsPos);
    }

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, MAJORAS_WRATH_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);
    this->shouldUpdateTentaclesOrWhips = false;

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * These four arrays encode circular shadow maps of various sizes. For an array of length N, the shadow map is N rows
 * tall, and each entry in the array describes the start and end point of the shadow within a given row (the exact
 * values of the start and end points are determined by the loops within Boss07_Wrath_FillShadowTex). To illustrate
 * using the sShadowSmallMap as an example:
 * -3 -2 -1  0  1
 *  -------------
 *  0  0  1  0  0
 *  0  1  1  1  0
 *  1  1  1  1  1
 *  1  1  1  1  1
 *  0  1  1  1  0
 *  0  0  1  0  0
 */
static s32 sShadowSmallMap[] = {
    1, 2, 3, 3, 2, 1,
};

static s32 sShadowMediumMap[] = {
    2, 3, 4, 4, 4, 3, 2,
};

static s32 sShadowLargeMap[] = {
    2, 3, 4, 4, 4, 4, 3, 2,
};

static s32 sShadowExtraLargeMap[] = {
    2, 4, 5, 5, 6, 6, 6, 6, 5, 5, 4, 2,
};

static s32 sParentShadowBodyParts[MAJORAS_WRATH_BODYPART_MAX] = {
    MAJORAS_WRATH_BODYPART_TORSO,                // MAJORAS_WRATH_BODYPART_HEAD
    BODYPART_NONE,                               // MAJORAS_WRATH_BODYPART_TORSO
    MAJORAS_WRATH_BODYPART_TORSO,                // MAJORAS_WRATH_BODYPART_PELVIS
    MAJORAS_WRATH_BODYPART_TORSO,                // MAJORAS_WRATH_BODYPART_LEFT_UPPER_ARM
    MAJORAS_WRATH_BODYPART_LEFT_UPPER_ARM,       // MAJORAS_WRATH_BODYPART_LEFT_LOWER_ARM_ROOT
    MAJORAS_WRATH_BODYPART_LEFT_LOWER_ARM_ROOT,  // MAJORAS_WRATH_BODYPART_LEFT_FOREARM
    MAJORAS_WRATH_BODYPART_TORSO,                // MAJORAS_WRATH_BODYPART_RIGHT_UPPER_ARM
    MAJORAS_WRATH_BODYPART_RIGHT_UPPER_ARM,      // MAJORAS_WRATH_BODYPART_RIGHT_LOWER_ARM_ROOT
    MAJORAS_WRATH_BODYPART_RIGHT_LOWER_ARM_ROOT, // MAJORAS_WRATH_BODYPART_RIGHT_FOREARM
    MAJORAS_WRATH_BODYPART_PELVIS,               // MAJORAS_WRATH_BODYPART_RIGHT_THIGH
    MAJORAS_WRATH_BODYPART_RIGHT_THIGH,          // MAJORAS_WRATH_BODYPART_RIGHT_SHIN
    MAJORAS_WRATH_BODYPART_RIGHT_SHIN,           // MAJORAS_WRATH_BODYPART_RIGHT_FOOT
    MAJORAS_WRATH_BODYPART_PELVIS,               // MAJORAS_WRATH_BODYPART_LEFT_THIGH
    MAJORAS_WRATH_BODYPART_LEFT_THIGH,           // MAJORAS_WRATH_BODYPART_LEFT_SHIN
    MAJORAS_WRATH_BODYPART_LEFT_SHIN             // MAJORAS_WRATH_BODYPART_LEFT_FOOT
};

static u8 sShadowSizes[MAJORAS_WRATH_BODYPART_MAX] = {
    MAJORAS_WRATH_SHADOW_SIZE_SMALL,       // MAJORAS_WRATH_BODYPART_HEAD
    MAJORAS_WRATH_SHADOW_SIZE_EXTRA_LARGE, // MAJORAS_WRATH_BODYPART_TORSO
    MAJORAS_WRATH_SHADOW_SIZE_EXTRA_LARGE, // MAJORAS_WRATH_BODYPART_PELVIS
    MAJORAS_WRATH_SHADOW_SIZE_LARGE,       // MAJORAS_WRATH_BODYPART_LEFT_UPPER_ARM
    MAJORAS_WRATH_SHADOW_SIZE_SMALL,       // MAJORAS_WRATH_BODYPART_LEFT_LOWER_ARM_ROOT
    MAJORAS_WRATH_SHADOW_SIZE_SMALL,       // MAJORAS_WRATH_BODYPART_LEFT_FOREARM
    MAJORAS_WRATH_SHADOW_SIZE_LARGE,       // MAJORAS_WRATH_BODYPART_RIGHT_UPPER_ARM
    MAJORAS_WRATH_SHADOW_SIZE_SMALL,       // MAJORAS_WRATH_BODYPART_RIGHT_LOWER_ARM_ROOT
    MAJORAS_WRATH_SHADOW_SIZE_SMALL,       // MAJORAS_WRATH_BODYPART_RIGHT_FOREARM
    MAJORAS_WRATH_SHADOW_SIZE_LARGE,       // MAJORAS_WRATH_BODYPART_RIGHT_THIGH
    MAJORAS_WRATH_SHADOW_SIZE_MEDIUM,      // MAJORAS_WRATH_BODYPART_RIGHT_SHIN
    MAJORAS_WRATH_SHADOW_SIZE_SMALL,       // MAJORAS_WRATH_BODYPART_RIGHT_FOOT
    MAJORAS_WRATH_SHADOW_SIZE_LARGE,       // MAJORAS_WRATH_BODYPART_LEFT_THIGH
    MAJORAS_WRATH_SHADOW_SIZE_MEDIUM,      // MAJORAS_WRATH_BODYPART_LEFT_SHIN
    MAJORAS_WRATH_SHADOW_SIZE_SMALL,       // MAJORAS_WRATH_BODYPART_LEFT_FOOT
};

void Boss07_Wrath_FillShadowTex(Boss07* this, u8* tex, f32 weight) {
    s32 index;
    s32 i;
    s32 baseX;
    s32 baseY;
    s32 x;
    s32 y = 0;
    s32 addY;
    Vec3f lerp;
    Vec3f pos;
    Vec3f startVec;

    for (i = 0; i < MAJORAS_WRATH_BODYPART_MAX; i++) {
        // TODO: match with a continue
        if ((weight == 0.0f) || ((y = sParentShadowBodyParts[i]) > BODYPART_NONE)) {
            if (weight > 0.0f) {
                VEC3F_LERPIMPDST(&lerp, &this->bodyPartsPos[i], &this->bodyPartsPos[y], weight);

                pos.x = lerp.x - this->actor.world.pos.x;
                pos.y = lerp.y - this->actor.world.pos.y + 76.0f + 30.0f + 30.0f + 100.0f;
                pos.z = lerp.z - this->actor.world.pos.z;
            } else {
                pos.x = this->bodyPartsPos[i].x - this->actor.world.pos.x;
                pos.y = this->bodyPartsPos[i].y - this->actor.world.pos.y + 76.0f + 30.0f + 30.0f + 100.0f;
                pos.z = this->bodyPartsPos[i].z - this->actor.world.pos.z;
            }

            Matrix_MultVec3f(&pos, &startVec);

            startVec.x *= 0.2f;
            startVec.y *= 0.2f;

            baseX = (u16)(s32)(startVec.x + 32.0f);
            baseY = (u16)((s32)startVec.y * 64);

            if (sShadowSizes[i] == MAJORAS_WRATH_SHADOW_SIZE_EXTRA_LARGE) {
                for (y = 0, addY = -384; y < ARRAY_COUNT(sShadowExtraLargeMap); y++, addY += 64) {
                    for (x = -sShadowExtraLargeMap[y]; x < sShadowExtraLargeMap[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < MAJORAS_WRATH_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else if (sShadowSizes[i] == MAJORAS_WRATH_SHADOW_SIZE_LARGE) {
                for (y = 0, addY = -256; y < ARRAY_COUNT(sShadowLargeMap); y++, addY += 64) {
                    for (x = -sShadowLargeMap[y]; x < sShadowLargeMap[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < MAJORAS_WRATH_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else if (sShadowSizes[i] == MAJORAS_WRATH_SHADOW_SIZE_MEDIUM) {
                for (y = 0, addY = -192; y < ARRAY_COUNT(sShadowMediumMap); y++, addY += 64) {
                    for (x = -sShadowMediumMap[y]; x < sShadowMediumMap[y] - 1; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < MAJORAS_WRATH_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else {
                for (y = 0, addY = -128; y < ARRAY_COUNT(sShadowSmallMap); y++, addY += 64) {
                    for (x = -sShadowSmallMap[y]; x < sShadowSmallMap[y] - 1; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < MAJORAS_WRATH_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            }
        }
    }
}

void Boss07_Wrath_GenShadowTex(u8* tex, Boss07* this, PlayState* play) {
    s32* iter = (s32*)tex;
    s32 i;

    for (i = 0; i < (s32)(MAJORAS_WRATH_SHADOW_TEX_SIZE / sizeof(s32)); i++, iter++) {
        *iter = 0;
    }

    Matrix_RotateXFNew(1.0f);

    for (i = 0; i <= 5; i++) {
        Boss07_Wrath_FillShadowTex(this, tex, i / 5.0f);
    }
}

/**
 * Draws Wrath's dynamic shadow underneath it.
 */
void Boss07_Wrath_DrawShadowTex(u8* tex, Boss07* this, PlayState* play) {
    s32 pad[2];
    f32 alpha;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    alpha = (400.0f - this->actor.world.pos.y) * (1.0f / 400.0f);
    alpha = CLAMP_MIN(alpha, 0.0f);
    alpha = CLAMP_MAX(alpha, 1.0f);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (s8)(alpha * 80.0f));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z - 20.0f, MTXMODE_NEW);
    Matrix_Scale(1.75f, 1.0f, 1.75f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gMajorasWrathShadowMaterialDL);
    gDPLoadTextureBlock(POLY_OPA_DISP++, tex, G_IM_FMT_I, G_IM_SIZ_8b, MAJORAS_WRATH_SHADOW_TEX_WIDTH,
                        MAJORAS_WRATH_SHADOW_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6,
                        6, G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_OPA_DISP++, gMajorasWrathShadowModelDL);

    CLOSE_DISPS(gfxCtx);
}

void Boss07_Incarnation_AvoidPlayer(Boss07* this) {
    s32 rotOffset = (Rand_ZeroOne() < 0.5f) ? 0x4000 : -0x4000;

    Matrix_RotateYS(this->actor.yawTowardsPlayer + rotOffset, MTXMODE_NEW);
    Matrix_MultVecZ(200.0f, &this->targetPos);
    this->timers[1] = 30;
    this->targetPos.x += this->actor.world.pos.x;
    this->targetPos.z += this->actor.world.pos.z;
    this->speedToTarget = 0x4E20;
}

void Boss07_Incarnation_SetupIntroCutscene(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Incarnation_IntroCutscene;
    Animation_MorphToLoop(&this->skelAnime, &gMajorasIncarnationJerkingAnim, 0.0f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->cutsceneHeadRot.x = 0x6E00;
}

void Boss07_Incarnation_IntroCutscene(Boss07* this, PlayState* play) {
    s32 i;
    f32 targetFactor;

    SkelAnime_Update(&this->skelAnime);

    this->cutsceneTimer++;
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    targetFactor = Math_SinS(this->frameCounter * 0x4500) * 0.3f;

    switch (this->cutsceneState) {
        case MAJORAS_INCARNATION_INTRO_CS_STATE_STARTING_OR_DONE:
            this->cutsceneTimer = 0;
            this->cutsceneState = MAJORAS_INCARNATION_INTRO_CS_STATE_UPDATE_SUBCAM;
            this->subCamAtNext.z = 0.0f;
            FALLTHROUGH;
        case MAJORAS_INCARNATION_INTRO_CS_STATE_UPDATE_SUBCAM:
            this->cutsceneState = MAJORAS_INCARNATION_INTRO_CS_STATE_GROW_LEFT_LEG;
            this->cutsceneTimer = 0;
            this->subCamEyeNext.x = 70.0f;
            this->subCamEyeNext.y = 70.0f;
            this->subCamEyeNext.z = 150.0f;
            this->subCamAtNext.x = 20.0f;
            this->subCamAtNext.y = 80.0f;
            break;

        case MAJORAS_INCARNATION_INTRO_CS_STATE_GROW_LEFT_LEG:
            if (this->cutsceneTimer >= 20) {
                if (this->cutsceneTimer == 20) {
                    this->incarnationIntroBodyPartsScaleTarget[MAJORAS_INCARNATION_GROW_BODYPART_LEFT_LEG] = 1.0f;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_TRANSFORM);
                }

                Math_ApproachF(
                    &this->incarnationIntroBodyPartsScale[MAJORAS_INCARNATION_GROW_BODYPART_LEFT_LEG],
                    1.0f + (this->incarnationIntroBodyPartsScaleTarget[MAJORAS_INCARNATION_GROW_BODYPART_LEFT_LEG] *
                            targetFactor),
                    1.0f, 0.5f);
            }

            if (this->cutsceneTimer > 40) {
                this->cutsceneState = MAJORAS_INCARNATION_INTRO_CS_STATE_GROW_OTHER_LIMBS;
                this->cutsceneTimer = 0;
            }
            break;

        case MAJORAS_INCARNATION_INTRO_CS_STATE_GROW_OTHER_LIMBS:
            Math_ApproachF(&this->subCamEyeNext.x, 0.0f, 0.05f, this->subCamVelocity * 40.0f);
            Math_ApproachF(&this->subCamEyeNext.y, 100.0f, 0.05f, this->subCamVelocity * 30.0f);
            Math_ApproachF(&this->subCamEyeNext.z, 270.0f, 0.05f, this->subCamVelocity * 150.0f);
            Math_ApproachF(&this->subCamAtNext.x, 0.0f, 0.05f, this->subCamVelocity * 20.0f);
            Math_ApproachF(&this->subCamAtNext.y, 100.0f, 0.05f, this->subCamVelocity * 20.0f);
            Math_ApproachF(&this->subCamVelocity, 0.05f, 1.0f, 0.002f);

            if (this->cutsceneTimer >= 20) {
                if (this->cutsceneTimer == 20) {
                    this->incarnationIntroBodyPartsScaleTarget[MAJORAS_INCARNATION_GROW_BODYPART_RIGHT_LEG] = 1.0f;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_TRANSFORM);
                }

                Math_ApproachF(
                    &this->incarnationIntroBodyPartsScale[MAJORAS_INCARNATION_GROW_BODYPART_RIGHT_LEG],
                    1.0f + (this->incarnationIntroBodyPartsScaleTarget[MAJORAS_INCARNATION_GROW_BODYPART_RIGHT_LEG] *
                            targetFactor),
                    1.0f, 0.5f);
            }

            if (this->cutsceneTimer >= 50) {
                if (this->cutsceneTimer == 50) {
                    this->incarnationIntroBodyPartsScaleTarget[MAJORAS_INCARNATION_GROW_BODYPART_LEFT_ARM] = 1.0f;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_TRANSFORM);
                }

                Math_ApproachF(
                    &this->incarnationIntroBodyPartsScale[MAJORAS_INCARNATION_GROW_BODYPART_LEFT_ARM],
                    1.0f + (this->incarnationIntroBodyPartsScaleTarget[MAJORAS_INCARNATION_GROW_BODYPART_LEFT_ARM] *
                            targetFactor),
                    1.0f, 0.5f);
            }

            if (this->cutsceneTimer >= 60) {
                if (this->cutsceneTimer == 60) {
                    this->incarnationIntroBodyPartsScaleTarget[MAJORAS_INCARNATION_GROW_BODYPART_RIGHT_ARM] = 1.0f;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_TRANSFORM);
                }

                Math_ApproachF(
                    &this->incarnationIntroBodyPartsScale[MAJORAS_INCARNATION_GROW_BODYPART_RIGHT_ARM],
                    1.0f + (this->incarnationIntroBodyPartsScaleTarget[MAJORAS_INCARNATION_GROW_BODYPART_RIGHT_ARM] *
                            targetFactor),
                    1.0f, 0.5f);
            }

            if (this->cutsceneTimer == 80) {
                this->cutsceneState = MAJORAS_INCARNATION_INTRO_CS_STATE_GROW_EYESTALK;
                this->cutsceneTimer = 0;
                for (i = 0; i < MAJORAS_INCARNATION_GROW_BODYPART_MAX; i++) {
                    this->incarnationIntroBodyPartsScale[i] = 1.0f;
                }
            }
            break;

        case MAJORAS_INCARNATION_INTRO_CS_STATE_GROW_EYESTALK:
            this->subCamEyeNext.x = 0.0f;
            this->subCamEyeNext.y = 120.0f;
            this->subCamEyeNext.z = 140.0f;
            this->subCamAtNext.y = 170.0f;

            if (this->cutsceneTimer >= 10) {
                // This code reuses the scale target for `MAJORAS_INCARNATION_GROW_BODYPART_LEFT_LEG` to control the
                // rotation of Incarnation's eyestalk. It doesn't matter which scale target it reused, however, because
                // none of them are currently being used by their intended limbs.
                if (this->cutsceneTimer == 10) {
                    this->incarnationIntroBodyPartsScaleTarget[MAJORAS_INCARNATION_GROW_BODYPART_LEFT_LEG] = 1.0f;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_GROW_HEAD);
                }

                Math_ApproachS(&this->cutsceneHeadRot.x,
                               this->incarnationIntroBodyPartsScaleTarget[MAJORAS_INCARNATION_GROW_BODYPART_LEFT_LEG] *
                                   targetFactor * 0x6000,
                               1, 0x4000);
            }

            if (this->cutsceneTimer == 30) {
                this->cutsceneState = MAJORAS_INCARNATION_INTRO_CS_STATE_DANCE;
                this->cutsceneTimer = 0;
                this->subCamVelocity = 0.0f;
                this->animEndFrame = 1000.0f;
                Play_DisableMotionBlur();
            }
            break;

        case MAJORAS_INCARNATION_INTRO_CS_STATE_DANCE:
            if (this->cutsceneTimer == 20) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasIncarnationIntroDanceAnim, 0.0f);
                this->animEndFrame = Animation_GetLastFrame(&gMajorasIncarnationIntroDanceAnim);
            }

            if (this->cutsceneTimer >= 20) {
                sREG(28) = this->cutsceneTimer;

                if ((this->cutsceneTimer == (u32)(KREG(16) + 28)) || (this->cutsceneTimer == (u32)(KREG(17) + 60))) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_VOICE_UAUOO2_OLD);
                }

                if ((this->cutsceneTimer == (u32)(KREG(18) + 38)) || (this->cutsceneTimer == (u32)(KREG(19) + 48)) ||
                    (this->cutsceneTimer == (u32)(KREG(20) + 68)) || (this->cutsceneTimer == (u32)(KREG(21) + 78))) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_GYM_B_OLD);
                }

                if (this->cutsceneTimer == (u32)(KREG(38) + 93)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_UAUOO_OLD);
                }
            }

            if (this->cutsceneTimer == 45) {
                TitleCard_InitBossName(&play->state, &play->actorCtx.titleCtx,
                                       Lib_SegmentedToVirtual(&gMajorasIncarnationTitleCardTex), 160, 180, 128, 40);
            }

            if (this->cutsceneTimer == 20) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_MAJORAS_INCARNATION | SEQ_FLAG_ASYNC);
                this->lightSettingsMode = 1;
                play->envCtx.lightBlend = 0.0f;
            }

            Math_ApproachF(&this->subCamEyeNext.x, -80.0f, 0.05f, this->subCamVelocity * 80.0f);
            Math_ApproachF(&this->subCamEyeNext.y, 20.0f, 0.05f, this->subCamVelocity * 60.0f);
            Math_ApproachF(&this->subCamEyeNext.z, 220.0f, 0.05f, this->subCamVelocity * 180.0f);
            Math_ApproachF(&this->subCamAtNext.y, 110.0f, 0.05f, this->subCamVelocity * 80.0f);
            Math_ApproachF(&this->subCamVelocity, 0.1f, 1.0f, 0.005f);

            if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

                this->cutsceneState = MAJORAS_INCARNATION_INTRO_CS_STATE_STARTING_OR_DONE;
                Boss07_Incarnation_SetupTaunt(this, play);
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            }
            break;
    }

    for (i = 0; i < MAJORAS_INCARNATION_GROW_BODYPART_MAX; i++) {
        Math_ApproachZeroF(&this->incarnationIntroBodyPartsScaleTarget[i], 0.5f, 0.1f);
    }

    Matrix_MultVec3f(&this->subCamEyeNext, &this->subCamEye);
    Matrix_MultVec3f(&this->subCamAtNext, &this->subCamAt);

    if (this->subCamId != SUB_CAM_ID_DONE) {
        ShrinkWindow_Letterbox_SetSizeTarget(27);
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void Boss07_Incarnation_SetupTaunt(Boss07* this, PlayState* play) {
    static AnimationHeader* sTauntAnimations[] = {
        &gMajorasIncarnationTauntDance1Anim,
        &gMajorasIncarnationTauntDance2Anim,
        &gMajorasIncarnationTauntJumpAnim,
    };

    this->actionFunc = Boss07_Incarnation_Taunt;
    this->subAction = Rand_ZeroFloat(MAJORAS_INCARNATION_TAUNT_SUB_ACTION_MAX - 0.001f);
    Animation_MorphToLoop(&this->skelAnime, sTauntAnimations[this->subAction], -10.0f);
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->timers[0] = Rand_ZeroFloat(50.0f) + 50.0f;
}

void Boss07_Incarnation_Taunt(Boss07* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->subAction < MAJORAS_INCARNATION_TAUNT_SUB_ACTION_JUMP) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_WAIT_OLD - SFX_FLAG);
    } else if ((this->subAction == MAJORAS_INCARNATION_TAUNT_SUB_ACTION_JUMP) &&
               Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_JUMP_OLD);
    }

    SkelAnime_Update(&this->skelAnime);
    Boss07_SmoothStop(this, 2.0f);
    Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 5, 0x2000);

    if ((this->timers[0] == 0) || (player->unk_D57 == 4) || (this->actor.xzDistToPlayer <= 300.0f)) {
        Boss07_Incarnation_SetupRun(this, play);
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_VOICE_SURPRISED_OLD);
    }

    Boss07_Incarnation_SpawnDust(this, play, 1, MAJORAS_INCARNATION_DUST_SPAWN_POS_FEET);
}

void Boss07_Incarnation_SetupStunned(Boss07* this, PlayState* play, s16 stunTime) {
    if (this->actionFunc != Boss07_Incarnation_Stunned) {
        this->actionFunc = Boss07_Incarnation_Stunned;
        Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasIncarnationFallOverStartAnim, -10.0f);
        this->animEndFrame = Animation_GetLastFrame(&gMajorasIncarnationFallOverStartAnim);
    }

    this->disableCollisionTimer = 10;
    this->timers[0] = stunTime;
    this->timers[1] = 12;
}

void Boss07_Incarnation_Stunned(Boss07* this, PlayState* play) {
    this->actor.shape.shadowAlpha = 0;
    SkelAnime_Update(&this->skelAnime);
    Boss07_SmoothStop(this, 1.0f);

    if (this->timers[1] == 1) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_HIPLOOP_LAND);
    }

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Animation_MorphToLoop(&this->skelAnime, &gMajorasIncarnationFallOverLoopAnim, -5.0f);
        this->animEndFrame = 1000.0f;
    }

    if (this->animEndFrame == 1000.0f) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_BIRD_OLD - SFX_FLAG);
    }

    if (this->timers[0] == 0) {
        Boss07_Incarnation_SetupTaunt(this, play);
    }

    Boss07_Incarnation_SpawnDust(this, play, 1, MAJORAS_INCARNATION_DUST_SPAWN_POS_FOCUS);
}

void Boss07_Incarnation_SetupDamaged(Boss07* this, PlayState* play, u8 damage, u8 dmgEffect) {
    if (this->actionFunc != Boss07_Incarnation_Damaged) {
        this->actionFunc = Boss07_Incarnation_Damaged;
        Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasIncarnationDamagedAnim, -2.0f);
        this->animEndFrame = Animation_GetLastFrame(&gMajorasIncarnationDamagedAnim);
    } else if (dmgEffect == MAJORAS_INCARNATION_DMGEFF_ANIM_FRAME_CHECK) {
        if (this->skelAnime.curFrame <= (this->animEndFrame - 5.0f)) {
            this->disableCollisionTimer = 30;
            this->damagedTimer = 30;
        } else {
            Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasIncarnationDamagedAnim, -2.0f);
        }
    }

    if (this->actor.colChkInfo.health != 0) {
        this->actor.colChkInfo.health -= damage;

        if ((s8)this->actor.colChkInfo.health <= 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_DEAD_OLD);
            this->shouldStartDeath = true;
            this->damagedTimer = 100;
            this->disableCollisionTimer = 100;
            Enemy_StartFinishingBlow(play, &this->actor);
            return;
        }
    }
    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_DAMAGE2_OLD);
}

void Boss07_Incarnation_Damaged(Boss07* this, PlayState* play) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_BIRD2_OLD - SFX_FLAG);
    SkelAnime_Update(&this->skelAnime);
    Boss07_SmoothStop(this, 2.0f);

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Boss07_Incarnation_SetupRun(this, play);
    }

    Boss07_Incarnation_SpawnDust(this, play, 1, MAJORAS_INCARNATION_DUST_SPAWN_POS_FEET);
}

void Boss07_Incarnation_SetupRun(Boss07* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gMajorasIncarnationRunAnim, -2.0f);
    this->actionFunc = Boss07_Incarnation_Run;
    this->timers[0] = Rand_ZeroFloat(100.0f) + 150.0f;
    Boss07_Incarnation_AvoidPlayer(this);
}

void Boss07_Incarnation_Run(Boss07* this, PlayState* play) {
    f32 dx;
    f32 dz;
    PlayerImpactType playerImpactType;

    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_WALK_OLD - SFX_FLAG);
    this->miscTimer++;

    if (this->miscTimer >= 2) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_WALK2_OLD);
        this->miscTimer = 0;
    }

    SkelAnime_Update(&this->skelAnime);

    if ((Actor_GetPlayerImpact(play, 5.0f, &this->actor.world.pos, &playerImpactType) >= 0.0f) &&
        (playerImpactType == PLAYER_IMPACT_GORON_GROUND_POUND)) {
        Boss07_Incarnation_SetupStunned(this, play, 50);
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_DAMAGE_OLD);
    } else {
        dx = this->targetPos.x - this->actor.world.pos.x;
        dz = this->targetPos.z - this->actor.world.pos.z;

        if ((this->timers[1] == 0) || (SQ(dx) + SQ(dz) < 30000.0f)) { // dist ~ 173.2
            if (Rand_ZeroOne() < 0.3f) {
                f32 rand = Rand_ZeroOne();

                if (rand < 0.25f) {
                    Boss07_Incarnation_SetupAttack(this, play);
                } else if (rand < 0.5f) {
                    Boss07_Incarnation_SetupSquattingDance(this, play);
                } else if (rand < 0.75f) {
                    Boss07_Incarnation_SetupMoonwalk(this, play);
                } else if (rand < 1.0f) {
                    Boss07_Incarnation_SetupPirouette(this, play);
                }
            } else if (Rand_ZeroOne() < 0.01f) {
                Boss07_Incarnation_SetupStunned(this, play, 50);
            } else {
                Boss07_RandXZ(&this->targetPos, 500.0f);
                this->timers[1] = Rand_ZeroFloat(50.0f) + 20.0f;
                this->speedToTarget = 0.0f;
            }
        }

        Math_ApproachS(&this->actor.world.rot.y, Math_Atan2S(dx, dz), 5, this->speedToTarget);
        Math_ApproachF(&this->speedToTarget, 0xFA0, 1.0f, 0x1F4);
        Math_ApproachF(&this->actor.speed, 25.0f, 1.0f, 20.0f);

        if (this->timers[0] == 0) {
            Boss07_Incarnation_SetupTaunt(this, play);
        }

        if ((this->actor.xzDistToPlayer < 200.0f) && (this->timers[2] == 0)) {
            Boss07_Incarnation_AvoidPlayer(this);
            this->timers[0] = 50;
            this->timers[2] = 50;
        }

        Boss07_Incarnation_SpawnDust(this, play, 3, MAJORAS_INCARNATION_DUST_SPAWN_POS_FEET);
        this->fireTimer = 5;
    }
}

void Boss07_Incarnation_SetupAttack(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Incarnation_Attack;
    Animation_MorphToLoop(&this->skelAnime, &gMajorasIncarnationEnergyBallAttackAnim, -5.0f);
    this->timers[0] = Rand_ZeroFloat(50.0f) + 30.0f;
    this->eyeBeamsLengthScale = 0.0f;
}

void Boss07_Incarnation_Attack(Boss07* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Boss07_SmoothStop(this, 3.0f);
    Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 3, 0x3000);

    if (Animation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_07, this->incarnationLeftHandPos.x,
                    this->incarnationLeftHandPos.y, this->incarnationLeftHandPos.z, 0, 0, 0,
                    MAJORA_PARAMS(MAJORA_TYPE_PROJECTILE_INCARNATION));
    }

    if (Animation_OnFrame(&this->skelAnime, 9.0f)) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_07, this->incarnationRightHandPos.x,
                    this->incarnationRightHandPos.y, this->incarnationRightHandPos.z, 0, 0, 0,
                    MAJORA_PARAMS(MAJORA_TYPE_PROJECTILE_INCARNATION));
    }

    if (this->timers[0] == 0) {
        Boss07_Incarnation_SetupRun(this, play);
    }

    Boss07_Incarnation_SpawnDust(this, play, 1, MAJORAS_INCARNATION_DUST_SPAWN_POS_FEET);
}

void Boss07_Incarnation_SetupSquattingDance(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Incarnation_SquattingDance;
    Animation_MorphToLoop(&this->skelAnime, &gMajorasIncarnationSquattingDanceAnim, -5.0f);
    this->timers[0] = Rand_ZeroFloat(100.0f) + 100.0f;
    this->timers[1] = 0;
    this->cutsceneTimer = 0;
}

void Boss07_Incarnation_SquattingDance(Boss07* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    // This uses `cutsceneTimer` to count how many loops of the dancing animation Majora's Incarnation completes.
    // Specifically, it's incremented every time Incarnation reaches the 5th frame of its animation.
    if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        if ((this->cutsceneTimer % 2) == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_VOICE_UAUOO1_OLD);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_VOICE_UAUOO2_OLD);
        }

        Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST2_JUMP_OLD);
        this->cutsceneTimer++;
    }

    Math_ApproachF(&this->actor.speed, KREG(67) + 10.0f, 1.0f, 3.0f);

    if (this->timers[1] == 0) {
        if ((this->frameCounter == 0) && (Rand_ZeroOne() < 0.7f)) {
            if (Rand_ZeroOne() < 0.75f) {
                this->topPrecessionVelocity = Rand_CenteredFloat(0x10000);
            } else {
                this->topPrecessionVelocity = this->actor.yawTowardsPlayer;
            }
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->timers[1] = 20;
            this->topPrecessionVelocity = this->actor.yawTowardsPlayer;
        }
    }

    Math_ApproachS(&this->actor.world.rot.y, this->topPrecessionVelocity, 0xA, 0x1800);

    if ((this->timers[0] == 0) || (this->actor.xzDistToPlayer <= 200.0f)) {
        Boss07_Incarnation_SetupRun(this, play);

        if (this->timers[0] != 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_VOICE_SURPRISED_OLD);
        }
    }

    Boss07_Incarnation_SpawnDust(this, play, 7, MAJORAS_INCARNATION_DUST_SPAWN_POS_FEET);
}

void Boss07_Incarnation_SetupMoonwalk(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Incarnation_Moonwalk;
    Animation_MorphToLoop(&this->skelAnime, &gMajorasIncarnationMoonwalkAnim, -10.0f);
    this->timers[0] = Rand_ZeroFloat(50.0f) + 70.0f;
    this->timers[1] = 0;
}

void Boss07_Incarnation_Moonwalk(Boss07* this, PlayState* play) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_MOONWALK_OLD - SFX_FLAG);
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachF(&this->actor.speed, -10.0f + KREG(69), 1.0f, 3.0f);

    if (this->timers[1] == 0) {
        if ((this->frameCounter == 0) && (Rand_ZeroOne() < 0.5f)) {
            if (Rand_ZeroOne() < 0.75f) {
                this->topPrecessionVelocity = Rand_CenteredFloat(0x10000);
            } else {
                this->topPrecessionVelocity = this->actor.yawTowardsPlayer + 0x8000;
            }
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->timers[1] = 20;
            this->topPrecessionVelocity = this->actor.yawTowardsPlayer + 0x8000;
        }
    }

    Math_ApproachS(&this->actor.world.rot.y, this->topPrecessionVelocity, 0xA, 0x1000);

    if (this->timers[0] == 0) {
        Boss07_Incarnation_SetupRun(this, play);
    }

    Boss07_Incarnation_SpawnDust(this, play, 7, MAJORAS_INCARNATION_DUST_SPAWN_POS_FEET);
}

void Boss07_Incarnation_SetupPirouette(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Incarnation_Pirouette;
    Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasIncarnationPirouetteAnim, -10.0f);
    this->timers[0] = Rand_ZeroFloat(70.0f) + 70.0f;
    this->timers[1] = 0;
}

void Boss07_Incarnation_Pirouette(Boss07* this, PlayState* play) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_BALLET_OLD - SFX_FLAG);
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachF(&this->actor.speed, 10.0f + KREG(69), 1.0f, 1.0f);

    if (this->timers[1] == 0) {
        if (((this->frameCounter % 64) == 0) && (Rand_ZeroOne() < 0.5f)) {
            if (Rand_ZeroOne() < 0.75f) {
                this->topPrecessionVelocity = Rand_CenteredFloat(0x10000);
            } else {
                this->topPrecessionVelocity = this->actor.yawTowardsPlayer;
            }
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->timers[1] = 20;
            this->topPrecessionVelocity = this->actor.yawTowardsPlayer;
        }
    }

    Math_ApproachS(&this->actor.world.rot.y, this->topPrecessionVelocity, 0xA, 0x1000);

    if ((this->timers[0] == 0) || (this->actor.xzDistToPlayer <= 200.0f)) {
        Boss07_Incarnation_SetupRun(this, play);

        if (this->timers[0] != 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_VOICE_SURPRISED_OLD);
        }
    }

    this->actor.shape.rot.y += 0x2000;
}

void Boss07_Incarnation_SetupDeathCutscene(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Incarnation_DeathCutscene;
    Boss07_MovePlayerFromCenter(play);
    Animation_MorphToPlayOnce(&this->skelAnime, &gMajorasIncarnationDamagedAnim, -2.0f);
    this->animEndFrame = Animation_GetLastFrame(&gMajorasIncarnationDamagedAnim);
    this->cutsceneState = MAJORAS_INCARNATION_DEATH_CS_STATE_STARTED;
    this->cutsceneTimer = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->damagedTimer = 20;
}

void Boss07_Incarnation_DeathCutscene(Boss07* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u8 limbPumpState = MAJORAS_INCARNATION_LIMB_PUMP_STATE_NONE;

    this->damagedTimer = 20;
    this->cutsceneTimer++;
    SkelAnime_Update(&this->skelAnime);
    Boss07_SmoothStop(this, 3.0f);

    switch (this->cutsceneState) {
        case MAJORAS_INCARNATION_DEATH_CS_STATE_STARTED:
            if (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            this->cutsceneTimer = 0;
            this->cutsceneState = MAJORAS_INCARNATION_DEATH_CS_STATE_PLAY_DAMAGED_ANIM;
            Play_EnableMotionBlur(150);
            FALLTHROUGH;
        case MAJORAS_INCARNATION_DEATH_CS_STATE_PLAY_DAMAGED_ANIM:
            sHeartbeatTimer = 5;
            this->subCamEyeNext.x = 0.0f;
            this->subCamEyeNext.y = 30.0f;
            this->subCamEyeNext.z = 210.0f;
            this->subCamAtNext.x = 0.0f;
            this->subCamAtNext.y = 150.0f;
            this->subCamAtNext.z = 0.0f;
            Math_ApproachF(&this->actor.world.pos.x, 0.0f, 0.1f, 5.0f);
            Math_ApproachF(&this->actor.world.pos.z, 0.0f, 0.1f, 5.0f);

            if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                this->cutsceneTimer = 0;
                this->cutsceneState = MAJORAS_INCARNATION_DEATH_CS_STATE_PLAY_FINAL_HIT_ANIM;
                Animation_MorphToLoop(&this->skelAnime, &gMajorasIncarnationFinalHitAnim, -5.0f);
            }
            break;

        case MAJORAS_INCARNATION_DEATH_CS_STATE_PLAY_FINAL_HIT_ANIM:
            sHeartbeatTimer = 5;
            this->subCamAtNext.y = (Math_SinS(this->cutsceneTimer * 0x700) * 15.0f) + 150.0f;

            if (this->cutsceneTimer == 40) {
                this->cutsceneTimer = 0;
                this->cutsceneState = MAJORAS_INCARNATION_DEATH_CS_STATE_PUMP_UP_RIGHT_ARM;

                this->subCamEyeNext.x = -30.0f;
                this->subCamEyeNext.y = 120.0f;
                this->subCamEyeNext.z = 110.0f;

                this->subCamAtNext.x = -70.0f;
                this->subCamAtNext.y = 150.0f;
                this->subCamAtNext.z = -20.0f;

                this->incarnationWrathTransitionScale = 1.0f;
                this->incarnationWrathTransitionAmplitude = 0.4f;
                Animation_MorphToLoop(&this->skelAnime, &gMajorasIncarnationPumpingUpAnim, -5.0f);
                this->actor.shape.rot.y = 0;
                this->actor.world.rot.y = this->actor.shape.rot.y;
                Math_ApproachF(&this->actor.world.pos.x, 0.0f, 1.0f, 200.0f);
                Math_ApproachF(&this->actor.world.pos.z, 0.0f, 1.0f, 200.0f);

                player->actor.world.pos.x = this->actor.world.pos.x;
                player->actor.world.pos.z = this->actor.world.pos.z + 300.0f;
                player->actor.world.rot.y = player->actor.shape.rot.y = -0x8000;

                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_PUMP_UP_OLD);
            }
            break;

        case MAJORAS_INCARNATION_DEATH_CS_STATE_PUMP_UP_RIGHT_ARM:
            sHeartbeatTimer = 5;
            limbPumpState = MAJORAS_INCARNATION_LIMB_PUMP_STATE_ARMS;

            if (this->cutsceneTimer == 40) {
                this->cutsceneTimer = 0;
                this->cutsceneState = MAJORAS_INCARNATION_DEATH_CS_STATE_PUMP_UP_LEFT_ARM;

                this->subCamEyeNext.x = 30.0f;
                this->subCamEyeNext.y = 120.0f;
                this->subCamEyeNext.z = 110.0f;

                this->subCamAtNext.x = 50.0f;
                this->subCamAtNext.y = 150.0f;
                this->subCamAtNext.z = -20.0f;

                this->incarnationWrathTransitionScale = 1.0f;
                this->incarnationWrathTransitionAmplitude = 0.4f;
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_PUMP_UP_OLD);
            }
            break;

        case MAJORAS_INCARNATION_DEATH_CS_STATE_PUMP_UP_LEFT_ARM:
            sHeartbeatTimer = 5;
            limbPumpState = MAJORAS_INCARNATION_LIMB_PUMP_STATE_ARMS;

            if (this->cutsceneTimer == 40) {
                this->cutsceneTimer = 0;
                this->cutsceneState = MAJORAS_INCARNATION_DEATH_CS_STATE_PUMP_UP_LEGS;

                this->subCamEyeNext.x = 0.0f;
                this->subCamEyeNext.y = 90.0f;
                this->subCamEyeNext.z = 110.0f;

                this->subCamAtNext.x = 0.0f;
                this->subCamAtNext.y = 90.0f;
                this->subCamAtNext.z = 0.0f;

                this->incarnationWrathTransitionScale = 1.0f;
                this->incarnationWrathTransitionAmplitude = 0.4f;
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_PUMP_UP_OLD);
            }
            break;

        case MAJORAS_INCARNATION_DEATH_CS_STATE_PUMP_UP_LEGS:
            sHeartbeatTimer = 5;
            limbPumpState = MAJORAS_INCARNATION_LIMB_PUMP_STATE_LEGS;

            if (this->cutsceneTimer == 10) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_PUMP_UP_OLD);
            }

            if (this->cutsceneTimer == 40) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_07, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, 0, this->subCamId, MAJORA_PARAMS(MAJORA_TYPE_WRATH));
                Actor_Kill(&this->actor);
            }
            break;

        default:
            break;
    }

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_MultVec3f(&this->subCamEyeNext, &this->subCamEye);
    Matrix_MultVec3f(&this->subCamAtNext, &this->subCamAt);

    if (limbPumpState == MAJORAS_INCARNATION_LIMB_PUMP_STATE_ARMS) {
        this->incarnationArmScale =
            (Math_SinS(this->cutsceneTimer * 0x3000) * this->incarnationWrathTransitionAmplitude) +
            this->incarnationWrathTransitionScale;
        Math_ApproachF(&this->incarnationWrathTransitionScale, 2.0f, 1.0f, 0.05f);
        Math_ApproachZeroF(&this->incarnationWrathTransitionAmplitude, 1.0f, 0.01f);
    } else if (limbPumpState == MAJORAS_INCARNATION_LIMB_PUMP_STATE_LEGS) {
        this->incarnationLegScale =
            (Math_SinS(this->cutsceneTimer * 0x3000) * this->incarnationWrathTransitionAmplitude) +
            this->incarnationWrathTransitionScale;
        Math_ApproachF(&this->incarnationWrathTransitionScale, 2.0f, 1.0f, 0.05f);
        Math_ApproachZeroF(&this->incarnationWrathTransitionAmplitude, 1.0f, 0.01f);
    }

    this->incarnationMaskScaleY = (Math_SinS(this->cutsceneTimer * 0x2000) * 0.1f) + 1.0f;
    this->incarnationMaskScaleX = (Math_CosS(this->cutsceneTimer * 0x2000) * 0.1f) + 1.0f;

    if (this->subCamId != SUB_CAM_ID_DONE) {
        ShrinkWindow_Letterbox_SetSizeTarget(27);
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void Boss07_Incarnation_UpdateDamage(Boss07* this, PlayState* play) {
    s32 i;
    s32 j;
    u8 damage;

    if (this->damagedTimer != 0) {
        return;
    }

    for (i = 0; i < ARRAY_COUNT(this->bodyColliderElements); i++) {
        if (!(this->bodyCollider.elements[i].base.acElemFlags & ACELEM_HIT)) {
            continue;
        }

        for (j = 0; j < ARRAY_COUNT(this->bodyColliderElements); j++) {
            this->bodyCollider.elements[j].base.acElemFlags &= ~ACELEM_HIT;
        }

        if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            this->drawDmgEffTimer = 0;
        }

        switch (this->actor.colChkInfo.damageEffect) {
            case MAJORAS_INCARNATION_DMGEFF_FREEZE:
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_FROZEN_INIT;
                break;

            case MAJORAS_INCARNATION_DMGEFF_FIRE:
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_FIRE_INIT;
                break;

            case MAJORAS_INCARNATION_DMGEFF_LIGHT_ORB:
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x, this->actor.focus.pos.y,
                            this->actor.focus.pos.z, 0, 0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                break;

            case MAJORAS_INCARNATION_DMGEFF_ELECTRIC_SPARKS:
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_INIT;
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                break;

            case MAJORAS_INCARNATION_DMGEFF_BLUE_LIGHT_ORB:
                this->drawDmgEffState = MAJORA_DRAW_DMGEFF_STATE_BLUE_LIGHT_ORB_INIT;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x, this->actor.focus.pos.y,
                            this->actor.focus.pos.z, 0, 0, 3, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                break;

            default:
                break;
        }

        if ((this->actionFunc == Boss07_Incarnation_Stunned) || (this->actionFunc == Boss07_Incarnation_Damaged)) {
            this->damagedTimer = (this->actor.colChkInfo.damageEffect == MAJORAS_INCARNATION_DMGEFF_EXPLOSIVE) ? 15 : 5;
            damage = this->actor.colChkInfo.damage;
            Boss07_Incarnation_SetupDamaged(this, play, damage, this->actor.colChkInfo.damageEffect);
            this->damagedFlashTimer = 15;
        } else {
            this->damagedTimer = 15;
            Boss07_Incarnation_SetupStunned(this, play, 150);
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_DAMAGE_OLD);
        }
        break;
    }
}

void Boss07_IncarnationAfterimage_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;

    if (DECR(this->timers[0]) == 0) {
        Actor_Kill(&this->actor);
    }
}

void Boss07_Incarnation_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;
    s32 i;
    s32 pad;

    this->actor.hintId = TATL_HINT_ID_MAJORAS_INCARNATION;
    this->frameCounter++;
    Math_Vec3f_Copy(&sMajoraSfxPos, &this->actor.projectedPos);

    if (this->lightSettingsMode == 1) {
        static u8 sPrevLightSettings[] = { 1, 0, 3, 0, 4, 0, 5, 0 };
        static u8 sLightSettings[] = { 0, 3, 0, 4, 0, 5, 0, 1 };

        Math_ApproachF(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.1f);

        if (play->envCtx.lightBlend == 1.0f) {
            play->envCtx.lightBlend = 0.0f;
            this->lightSettingsIndex++;

            if (this->lightSettingsIndex >= ARRAY_COUNT(sLightSettings)) {
                this->lightSettingsIndex = 0;
            }
        }

        play->envCtx.prevLightSetting = sPrevLightSettings[this->lightSettingsIndex];
        play->envCtx.lightSetting = sLightSettings[this->lightSettingsIndex];
    }

    Math_ApproachF(&play->envCtx.lightBlend, 0.0f, 1.0f, 0.03f);

    if (KREG(63) == 0) {
        this->canEvade = false;
        this->actor.shape.shadowAlpha = 130;

        for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
            DECR(this->timers[i]);
        }

        DECR(this->damagedTimer);
        DECR(this->damagedFlashTimer);
        DECR(this->disableCollisionTimer);
        DECR(this->fireTimer);

        Math_ApproachZeroF(&this->shockSparkScale, 1.0f, 0.04f);

        this->actionFunc(this, play);

        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 200.0f, 100.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

        if (this->shouldStartDeath || KREG(88)) {
            KREG(88) = false;
            this->shouldStartDeath = false;
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
            Boss07_Incarnation_SetupDeathCutscene(this, play);
        }
    }

    if (this->disableCollisionTimer != 0) {
        for (i = 0; i < ARRAY_COUNT(this->bodyColliderElements); i++) {
            this->bodyCollider.elements[i].base.acElemFlags &= ~ACELEM_HIT;
        }
    }

    Boss07_Incarnation_UpdateDamage(this, play);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);

    if (this->actionFunc != Boss07_Incarnation_Pirouette) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 1, 0x1000);
    }

    // This code uses `fireTimer` to determine whether or not to spawn afterimages. While Incarnation is running,
    // `Boss07_Incarnation_Run` will constantly set `fireTimer` to 5, so this will spawn afterimages every other frame.
    // Once it stops running, this will continue to spawn afterimages every other frame until `fireTimer` reaches 0.
    if ((this->fireTimer != 0) && !(this->frameCounter & 1)) {
        Boss07* afterimage =
            (Boss07*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_07, this->actor.world.pos.x,
                                        this->actor.world.pos.y, this->actor.world.pos.z, this->actor.world.rot.x,
                                        this->actor.world.rot.y, 7, MAJORA_PARAMS(MAJORA_TYPE_INCARNATION_AFTERIMAGE));

        if (afterimage != NULL) {
            for (i = 0; i < MAJORAS_INCARNATION_LIMB_MAX; i++) {
                afterimage->skelAnime.jointTable[i] = this->skelAnime.jointTable[i];
            }
        }
    }

    Boss07_UpdateDamageEffects(this, play);
}

void Boss07_IncarnationAfterimage_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;
    Boss07* parent = (Boss07*)this->actor.parent;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 0, 0, 100, 155, 900, 1099);
    SkelAnime_DrawFlexOpa(play, parent->skelAnime.skeleton, this->skelAnime.jointTable, parent->skelAnime.dListCount,
                          NULL, NULL, &this->actor);
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 Boss07_Incarnation_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        Actor* thisx) {
    Boss07* this = (Boss07*)thisx;

    if (limbIndex == MAJORAS_INCARNATION_LIMB_EYESTALK) {
        rot->y += this->cutsceneHeadRot.x;
    }

    return false;
}

static s8 sIncarnationLimbToColliderBodyParts[] = {
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_NONE
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_ROOT
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_WRAPPER
    MAJORAS_INCARNATION_COLLIDER_BODYPART_MASK,            // MAJORAS_INCARNATION_LIMB_MASK
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_RIGHT_LEG_ROOT
    MAJORAS_INCARNATION_COLLIDER_BODYPART_RIGHT_THIGH,     // MAJORAS_INCARNATION_LIMB_RIGHT_THIGH
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_RIGHT_LOWER_LEG_ROOT
    MAJORAS_INCARNATION_COLLIDER_BODYPART_RIGHT_SHIN,      // MAJORAS_INCARNATION_LIMB_RIGHT_SHIN
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_RIGHT_FOOT
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_RIGHT_ARM_ROOT
    MAJORAS_INCARNATION_COLLIDER_BODYPART_RIGHT_UPPER_ARM, // MAJORAS_INCARNATION_LIMB_RIGHT_UPPER_ARM
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_RIGHT_LOWER_ARM_ROOT
    MAJORAS_INCARNATION_COLLIDER_BODYPART_RIGHT_FOREARM,   // MAJORAS_INCARNATION_LIMB_RIGHT_FOREARM
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_RIGHT_HAND
    MAJORAS_INCARNATION_COLLIDER_BODYPART_MASK,            // MAJORAS_INCARNATION_LIMB_LEFT_ARM_ROOT
    MAJORAS_INCARNATION_COLLIDER_BODYPART_LEFT_UPPER_ARM,  // MAJORAS_INCARNATION_LIMB_LEFT_UPPER_ARM
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_LEFT_LOWER_ARM_ROOT
    MAJORAS_INCARNATION_COLLIDER_BODYPART_LEFT_FOREARM,    // MAJORAS_INCARNATION_LIMB_LEFT_FOREARM
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_LEFT_HAND
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_LEFT_LEG_ROOT
    MAJORAS_INCARNATION_COLLIDER_BODYPART_LEFT_THIGH,      // MAJORAS_INCARNATION_LIMB_LEFT_THIGH
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_LEFT_LOWER_LEG_ROOT
    MAJORAS_INCARNATION_COLLIDER_BODYPART_LEFT_SHIN,       // MAJORAS_INCARNATION_LIMB_LEFT_SHIN
    BODYPART_NONE,                                         // MAJORAS_INCARNATION_LIMB_LEFT_FOOT
    MAJORAS_INCARNATION_COLLIDER_BODYPART_EYESTALK,        // MAJORAS_INCARNATION_LIMB_EYESTALK
    BODYPART_NONE,                                         // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                                         // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                                         // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                                         // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                                         // Doesn't correspond to a real limb on Majora's Incarnation
};

static s8 sIncarnationLimbToPumpBodyParts[] = {
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_NONE
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_ROOT
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_WRAPPER
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_MASK
    MAJORAS_INCARNATION_GROW_BODYPART_RIGHT_LEG, // MAJORAS_INCARNATION_LIMB_RIGHT_LEG_ROOT
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_RIGHT_THIGH
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_RIGHT_LOWER_LEG_ROOT
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_RIGHT_SHIN
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_RIGHT_FOOT
    MAJORAS_INCARNATION_GROW_BODYPART_RIGHT_ARM, // MAJORAS_INCARNATION_LIMB_RIGHT_ARM_ROOT
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_RIGHT_UPPER_ARM
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_RIGHT_LOWER_ARM_ROOT
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_RIGHT_FOREARM
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_RIGHT_HAND
    MAJORAS_INCARNATION_GROW_BODYPART_LEFT_ARM,  // MAJORAS_INCARNATION_LIMB_LEFT_ARM_ROOT
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_LEFT_UPPER_ARM
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_LEFT_LOWER_ARM_ROOT
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_LEFT_FOREARM
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_LEFT_HAND
    MAJORAS_INCARNATION_GROW_BODYPART_LEFT_LEG,  // MAJORAS_INCARNATION_LIMB_LEFT_LEG_ROOT
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_LEFT_THIGH
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_LEFT_LOWER_LEG_ROOT
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_LEFT_SHIN
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_LEFT_FOOT
    BODYPART_NONE,                               // MAJORAS_INCARNATION_LIMB_EYESTALK
    BODYPART_NONE,                               // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                               // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                               // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                               // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                               // Doesn't correspond to a real limb on Majora's Incarnation
};

static Vec3f sIncarnationLimbColliderOffsets[MAJORAS_INCARNATION_COLLIDER_BODYPART_MAX] = {
    { 2000.0f, 0.0f, 0.0f },             // MAJORAS_INCARNATION_COLLIDER_BODYPART_EYESTALK
    { 3500.0f, -1000.0f, 0.0f },         // MAJORAS_INCARNATION_COLLIDER_BODYPART_MASK
    { 100000.0f, 100000.0f, 100000.0f }, // MAJORAS_INCARNATION_COLLIDER_BODYPART_ROOM_ORIGIN
    { 4000.0f, 0.0f, 0.0f },             // MAJORAS_INCARNATION_COLLIDER_BODYPART_LEFT_UPPER_ARM
    { 4000.0f, 0.0f, 0.0f },             // MAJORAS_INCARNATION_COLLIDER_BODYPART_LEFT_FOREARM
    { 4000.0f, 0.0f, 0.0f },             // MAJORAS_INCARNATION_COLLIDER_BODYPART_RIGHT_UPPER_ARM
    { 4000.0f, 0.0f, 0.0f },             // MAJORAS_INCARNATION_COLLIDER_BODYPART_RIGHT_FOREARM
    { 4000.0f, 0.0f, 0.0f },             // MAJORAS_INCARNATION_COLLIDER_BODYPART_LEFT_THIGH
    { 4000.0f, 0.0f, 0.0f },             // MAJORAS_INCARNATION_COLLIDER_BODYPART_LEFT_SHIN
    { 4000.0f, 0.0f, 0.0f },             // MAJORAS_INCARNATION_COLLIDER_BODYPART_RIGHT_THIGH
    { 4000.0f, 0.0f, 0.0f },             // MAJORAS_INCARNATION_COLLIDER_BODYPART_RIGHT_SHIN
};

static s8 sIncarnationLimbToBodyParts[] = {
    BODYPART_NONE,                                // MAJORAS_INCARNATION_LIMB_NONE
    BODYPART_NONE,                                // MAJORAS_INCARNATION_LIMB_ROOT
    BODYPART_NONE,                                // MAJORAS_INCARNATION_LIMB_WRAPPER
    MAJORAS_INCARNATION_BODYPART_LEFT_ARM_ROOT,   // MAJORAS_INCARNATION_LIMB_MASK
    MAJORAS_INCARNATION_BODYPART_RIGHT_LEG_ROOT,  // MAJORAS_INCARNATION_LIMB_RIGHT_LEG_ROOT
    MAJORAS_INCARNATION_BODYPART_RIGHT_THIGH,     // MAJORAS_INCARNATION_LIMB_RIGHT_THIGH
    BODYPART_NONE,                                // MAJORAS_INCARNATION_LIMB_RIGHT_LOWER_LEG_ROOT
    MAJORAS_INCARNATION_BODYPART_RIGHT_SHIN,      // MAJORAS_INCARNATION_LIMB_RIGHT_SHIN
    MAJORAS_INCARNATION_BODYPART_RIGHT_FOOT,      // MAJORAS_INCARNATION_LIMB_RIGHT_FOOT
    BODYPART_NONE,                                // MAJORAS_INCARNATION_LIMB_RIGHT_ARM_ROOT
    MAJORAS_INCARNATION_BODYPART_RIGHT_UPPER_ARM, // MAJORAS_INCARNATION_LIMB_RIGHT_UPPER_ARM
    BODYPART_NONE,                                // MAJORAS_INCARNATION_LIMB_RIGHT_LOWER_ARM_ROOT
    MAJORAS_INCARNATION_BODYPART_RIGHT_FOREARM,   // MAJORAS_INCARNATION_LIMB_RIGHT_FOREARM
    MAJORAS_INCARNATION_BODYPART_RIGHT_HAND,      // MAJORAS_INCARNATION_LIMB_RIGHT_HAND
    MAJORAS_INCARNATION_BODYPART_LEFT_ARM_ROOT,   // MAJORAS_INCARNATION_LIMB_LEFT_ARM_ROOT
    MAJORAS_INCARNATION_BODYPART_LEFT_UPPER_ARM,  // MAJORAS_INCARNATION_LIMB_LEFT_UPPER_ARM
    BODYPART_NONE,                                // MAJORAS_INCARNATION_LIMB_LEFT_LOWER_ARM_ROOT
    MAJORAS_INCARNATION_BODYPART_LEFT_FOREARM,    // MAJORAS_INCARNATION_LIMB_LEFT_FOREARM
    MAJORAS_INCARNATION_BODYPART_LEFT_HAND,       // MAJORAS_INCARNATION_LIMB_LEFT_HAND
    BODYPART_NONE,                                // MAJORAS_INCARNATION_LIMB_LEFT_LEG_ROOT
    MAJORAS_INCARNATION_BODYPART_LEFT_THIGH,      // MAJORAS_INCARNATION_LIMB_LEFT_THIGH
    BODYPART_NONE,                                // MAJORAS_INCARNATION_LIMB_LEFT_LOWER_LEG_ROOT
    MAJORAS_INCARNATION_BODYPART_LEFT_SHIN,       // MAJORAS_INCARNATION_LIMB_LEFT_SHIN
    MAJORAS_INCARNATION_BODYPART_LEFT_FOOT,       // MAJORAS_INCARNATION_LIMB_LEFT_FOOT
    MAJORAS_INCARNATION_BODYPART_EYESTALK,        // MAJORAS_INCARNATION_LIMB_EYESTALK
    BODYPART_NONE,                                // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                                // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                                // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                                // Doesn't correspond to a real limb on Majora's Incarnation
    BODYPART_NONE,                                // Doesn't correspond to a real limb on Majora's Incarnation
};

void Boss07_Incarnation_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    Boss07* this = (Boss07*)thisx;
    Vec3f colliderPos;
    s8 bodyPartIndex;

    if (limbIndex == MAJORAS_INCARNATION_LIMB_EYESTALK) {
        Matrix_MultZero(&this->actor.focus.pos);
    }

    bodyPartIndex = sIncarnationLimbToBodyParts[limbIndex];
    if (bodyPartIndex > BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[bodyPartIndex]);
    }

    bodyPartIndex = sIncarnationLimbToColliderBodyParts[limbIndex];
    if (bodyPartIndex > BODYPART_NONE) {
        if (this->disableCollisionTimer != 0) {
            Matrix_MultVecZ(100000.0f, &colliderPos);
        } else {
            Matrix_MultVec3f(&sIncarnationLimbColliderOffsets[bodyPartIndex], &colliderPos);
        }

        Boss07_SetColliderSphere(bodyPartIndex, &this->bodyCollider, &colliderPos);
    }

    if (limbIndex == MAJORAS_INCARNATION_LIMB_RIGHT_FOOT) {
        Matrix_MultZero(&this->incarnationFeetPos[MAJORAS_INCARNATION_FOOT_RIGHT]);
    }

    if (limbIndex == MAJORAS_INCARNATION_LIMB_LEFT_FOOT) {
        Matrix_MultZero(&this->incarnationFeetPos[MAJORAS_INCARNATION_FOOT_LEFT]);
    }

    if (limbIndex == MAJORAS_INCARNATION_LIMB_RIGHT_HAND) {
        Matrix_MultZero(&this->incarnationRightHandPos);
    }

    if (limbIndex == MAJORAS_INCARNATION_LIMB_LEFT_HAND) {
        Matrix_MultZero(&this->incarnationLeftHandPos);
    }

    bodyPartIndex = sIncarnationLimbToPumpBodyParts[limbIndex];
    if (bodyPartIndex > BODYPART_NONE) {
        Matrix_Scale(this->incarnationIntroBodyPartsScale[bodyPartIndex],
                     this->incarnationIntroBodyPartsScale[bodyPartIndex],
                     this->incarnationIntroBodyPartsScale[bodyPartIndex], MTXMODE_APPLY);
    }
}

void Boss07_Incarnation_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    Boss07* this = (Boss07*)thisx;

    if (limbIndex == MAJORAS_INCARNATION_LIMB_MASK) {
        Matrix_Scale(this->incarnationMaskScaleX, this->incarnationMaskScaleY, 1.0f, MTXMODE_APPLY);
    }

    if ((limbIndex == MAJORAS_INCARNATION_LIMB_RIGHT_UPPER_ARM) ||
        (limbIndex == MAJORAS_INCARNATION_LIMB_RIGHT_FOREARM) || (limbIndex == MAJORAS_INCARNATION_LIMB_RIGHT_HAND) ||
        (limbIndex == MAJORAS_INCARNATION_LIMB_LEFT_UPPER_ARM) ||
        (limbIndex == MAJORAS_INCARNATION_LIMB_LEFT_FOREARM) || (limbIndex == MAJORAS_INCARNATION_LIMB_LEFT_HAND)) {
        Matrix_Scale(1.0f, this->incarnationArmScale, this->incarnationArmScale, MTXMODE_APPLY);
    }

    if ((limbIndex == MAJORAS_INCARNATION_LIMB_RIGHT_THIGH) || (limbIndex == MAJORAS_INCARNATION_LIMB_RIGHT_SHIN) ||
        (limbIndex == MAJORAS_INCARNATION_LIMB_RIGHT_FOOT) || (limbIndex == MAJORAS_INCARNATION_LIMB_LEFT_THIGH) ||
        (limbIndex == MAJORAS_INCARNATION_LIMB_LEFT_SHIN) || (limbIndex == MAJORAS_INCARNATION_LIMB_LEFT_FOOT)) {
        Matrix_Scale(1.0f, this->incarnationLegScale, this->incarnationLegScale, MTXMODE_APPLY);
    }
}

void Boss07_Incarnation_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    if ((this->damagedFlashTimer % 2) != 0) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
    }

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, Boss07_Incarnation_OverrideLimbDraw,
                                   Boss07_Incarnation_PostLimbDraw, Boss07_Incarnation_TransformLimbDraw, &this->actor);
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, MAJORAS_INCARNATION_BODYPART_MAX,
                            this->drawDmgEffScale, this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha,
                            this->drawDmgEffType);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Boss07_Mask_SetupIdle(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Mask_Idle;
    Animation_MorphToLoop(&this->skelAnime, &gMajorasMaskFloatingAnim, -20.0f);
    this->timers[2] = Rand_ZeroFloat(150.0f) + 60.0f;
    this->tentacleLengthScale = 1.0f;
}

void Boss07_Mask_Idle(Boss07* this, PlayState* play) {
    s16 targetRotX;
    s16 targetRotY;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 distToTargetXZ;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_FLOAT_OLD - SFX_FLAG);
    if (this->timers[0] == 0) {
        if (this->timers[2] == 0) {
            if (((s8)this->actor.colChkInfo.health <= 8) && (player->transformation != PLAYER_FORM_FIERCE_DEITY) &&
                (Rand_ZeroOne() < 0.75f)) {
                Boss07_Mask_SetupFireBeam(this, play);
            } else {
                Boss07_Mask_SetupSpinAttack(this, play);
            }
        } else if (Rand_ZeroOne() < 0.15f) {
            this->flySpeedTarget = 2.0f;
            this->timers[0] = Rand_ZeroFloat(50.0f) + 30.0f;
        } else {
            Boss07_RandXZ(&this->targetPos, 500.0f);
            this->targetPos.y = Rand_ZeroFloat(350.0f) + 100.0f;
            this->timers[0] = Rand_ZeroFloat(50.0f) + 20.0f;
            this->speedToTarget = 0.0f;
            this->flySpeedTarget = Rand_ZeroFloat(12.0f) + 3.0f;
        }
    }

    dx = this->targetPos.x - this->actor.world.pos.x;
    dy = this->targetPos.y - this->actor.world.pos.y;
    dz = this->targetPos.z - this->actor.world.pos.z;
    targetRotY = Math_Atan2S(dx, dz);
    distToTargetXZ = sqrtf(SQ(dx) + SQ(dz));
    targetRotX = Math_Atan2S(dy, distToTargetXZ);
    targetRotX += (s16)(Math_SinS(this->frameCounter * 0x1388) * 0xFA0);

    Math_ApproachS(&this->actor.world.rot.y, targetRotY, 0xA, this->speedToTarget);
    Math_ApproachS(&this->actor.world.rot.x, targetRotX, 5, this->speedToTarget);
    Math_ApproachF(&this->speedToTarget, 0x7D0, 1.0f, 0x64);
    Math_ApproachF(&this->actor.speed, this->flySpeedTarget, 1.0f, 1.0f);

    if (this->timers[1] != 0) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x3000);
    } else if (this->flySpeedTarget < 7.0f) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x2000);
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 5, 0x2000);
    }

    if ((player->unk_D57 == 4) && (Rand_ZeroOne() < 0.8f)) {
        this->timers[1] = 20;
    }

    Math_ApproachS(&this->actor.shape.rot.x, 0, 0xA, 0x200);
    Math_ApproachS(&this->actor.shape.rot.z, 0, 0xA, 0x400);

    if (this->shouldStartDeath || KREG(88)) {
        KREG(88) = false;
        this->shouldStartDeath = false;
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
        Boss07_Mask_SetupDeathCutscene(this, play);
    }
}

void Boss07_Mask_SetupSpinAttack(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Mask_SpinAttack;
    this->subAction = MAJORAS_MASK_SPIN_ATTACK_SUB_ACTION_WIND_UP;
    this->cutsceneState = MAJORAS_MASK_SPIN_ATTACK_RETARGET_PASSIVE;
    this->timers[0] = 30;
    this->angularVelocity = 0;
    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_ATTACK_OLD);
}

void Boss07_Mask_SpinAttack(Boss07* this, PlayState* play) {
    s16 targetRotX;
    s16 targetRotY;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 distToTargetXZ;
    Player* player = GET_PLAYER(play);

    this->actor.shape.rot.z -= this->angularVelocity;

    switch (this->subAction) {
        case MAJORAS_MASK_SPIN_ATTACK_SUB_ACTION_WIND_UP:
            Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000);
            Boss07_SmoothStop(this, 1.0f);
            Math_ApproachS(&this->actor.shape.rot.x, -0x4000, 0xA, 0x100);
            Math_ApproachS(&this->angularVelocity, 0x2000, 1, 0x100);

            if (this->timers[0] == 0) {
                this->subAction = MAJORAS_MASK_SPIN_ATTACK_SUB_ACTION_ATTACK;
                this->actor.world.rot.x = 0;
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                this->timers[0] = 100;
                this->timers[1] = 20;
                this->speedToTarget = 0.0f;
            }
            break;

        case MAJORAS_MASK_SPIN_ATTACK_SUB_ACTION_ATTACK:
            Math_ApproachS(&this->actor.shape.rot.x, -0x4000, 0xA, 0x400);
            Math_ApproachS(&this->angularVelocity, 0x2000, 1, 0x200);

            if (this->timers[1] != 0) {
                this->targetPos.x = player->actor.world.pos.x;
                this->targetPos.y = player->actor.world.pos.y + 10.0f;
                this->targetPos.z = player->actor.world.pos.z;
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_ROLLING_OLD - SFX_FLAG);
            }

            dx = this->targetPos.x - this->actor.world.pos.x;
            dy = this->targetPos.y - this->actor.world.pos.y;
            dz = this->targetPos.z - this->actor.world.pos.z;
            targetRotY = Math_Atan2S(dx, dz);
            distToTargetXZ = sqrtf(SQ(dx) + SQ(dz));
            targetRotX = Math_Atan2S(dy, distToTargetXZ);
            Math_ApproachS(&this->actor.world.rot.y, targetRotY, 0xA, this->speedToTarget);
            Math_ApproachS(&this->actor.world.rot.x, targetRotX, 0xA, this->speedToTarget);
            Math_ApproachF(&this->speedToTarget, 0xBB8, 1.0f, 0x64);
            Math_ApproachF(&this->actor.speed, 20.0f, 1.0f, 2.0f);

            // This uses `cutsceneState` to determine if Majora's Mask should try to passively avoid the player with its
            // spin or if it should actively try to attack them. Note that the Mask will always attempt to attack the
            // player immediately after the spin attack starts; this code will only run after it has approached the
            // player at least once.
            if (((this->cutsceneState == MAJORAS_MASK_SPIN_ATTACK_RETARGET_PASSIVE) && (distToTargetXZ < 100.0f)) ||
                (this->timers[0] == 0)) {
                if (Rand_ZeroOne() < 0.25f) {
                    this->subAction = MAJORAS_MASK_SPIN_ATTACK_SUB_ACTION_END;
                    this->timers[0] = 30;
                } else {
                    Boss07_RandXZ(&this->targetPos, 500.0f);
                    this->targetPos.y = Rand_ZeroFloat(100.0f) + 100.0f;

                    if (Rand_ZeroOne() < 0.3f) {
                        this->timers[1] = 20;
                        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_ATTACK_2ND_OLD);
                        this->cutsceneState = MAJORAS_MASK_SPIN_ATTACK_RETARGET_ACTIVE;
                    } else {
                        this->timers[1] = 0;
                        this->cutsceneState = MAJORAS_MASK_SPIN_ATTACK_RETARGET_PASSIVE;
                    }

                    this->timers[0] = 50;
                    this->speedToTarget = 0.0f;
                }
            }
            break;

        case MAJORAS_MASK_SPIN_ATTACK_SUB_ACTION_END:
            Math_ApproachS(&this->angularVelocity, 0, 1, 0x100);
            Math_ApproachS(&this->actor.world.rot.x, 0x2000, 0xA, 0x7D0);
            Boss07_SmoothStop(this, 0.5f);
            if (this->timers[0] == 0) {
                Boss07_Mask_SetupIdle(this, play);
            }
            break;

        default:
            break;
    }
}

void Boss07_Mask_SetupStunned(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Mask_Stunned;
    Animation_MorphToLoop(&this->skelAnime, &gMajorasMaskJerkingAnim, -10.0f);
    this->timers[0] = 100;
    this->speedToTarget = 0.0f;
}

void Boss07_Mask_Stunned(Boss07* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_COMMON_WEAKENED - SFX_FLAG);
    Boss07_SmoothStop(this, 0.5f);
    Math_ApproachS(&this->actor.shape.rot.x, -0x1000 + 0x4B0 * Math_SinS(this->frameCounter * 0xBB8), 5, 0x800);
    Math_ApproachS(&this->actor.shape.rot.z, -0x1000 + 0x3E8 * Math_SinS(this->frameCounter * 0x9C4), 5, 0x800);

    if (this->actor.world.pos.y > 51.0f) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_FALL_OLD - SFX_FLAG);
    }

    Math_ApproachF(&this->actor.world.pos.y, 50.0f + 10.0f * Math_SinS(this->frameCounter * 0x5DC), 0.1f,
                   this->speedToTarget);
    Math_ApproachF(&this->speedToTarget, 15.0f, 1.0f, 1.0f);

    if ((this->timers[0] > 30) || ((this->timers[0] & 2) != 0)) {
        this->maskEyeTexIndex = MAJORAS_MASK_EYE_DULL;
    }

    if (this->timers[0] == 0) {
        Boss07_Mask_SetupIdle(this, play);
        this->timers[2] = 100;
    }
}

void Boss07_Mask_SetupDamaged(Boss07* this, PlayState* play, u8 damage, Actor* hitActor) {
    Player* player = GET_PLAYER(play);

    this->actionFunc = Boss07_Mask_Damaged;
    this->timers[0] = 40;

    if ((u32)damage != 0) {
        this->actor.speed = 30.0f;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
        this->angularVelocity = 0x1500;
    } else {
        if (player->stateFlags3 & PLAYER_STATE3_200) {
            this->actor.speed = 20.0f;
            this->actor.velocity.y = 20.0f;
        } else {
            this->actor.speed = 13.0f;
            this->actor.velocity.y = 10.0f;
            if (hitActor != NULL) {
                this->actor.world.rot.y = hitActor->world.rot.y;
            }
        }

        this->angularVelocity = 0x1000;
    }

    this->actor.colChkInfo.health -= damage;

    if ((s8)this->actor.colChkInfo.health <= 0) {
        this->timers[0] = 30;
    }

    this->timers[1] = 30;
}

void Boss07_Mask_Damaged(Boss07* this, PlayState* play) {
    this->damagedTimer = 20;
    SkelAnime_Update(&this->skelAnime);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Boss07_SmoothStop(this, 1.0f);
        Math_ApproachS(&this->actor.shape.rot.x, -0x4000, 1, 0x1000);
        Math_ApproachS(&this->angularVelocity, 0, 1, 0x100);
    } else {
        this->actor.shape.rot.x += 0x2000;
    }

    this->actor.shape.rot.z += this->angularVelocity;

    if ((this->timers[0] > 30) || ((this->timers[0] & 2) != 0)) {
        this->maskEyeTexIndex = MAJORAS_MASK_EYE_DULL;
    }

    if ((this->timers[0] == 15) && ((s8)this->actor.colChkInfo.health < 10)) {
        this->startRemainsCs = true;
    }

    if (this->timers[0] == 0) {
        Boss07_Mask_SetupIdle(this, play);

        if ((s8)this->actor.colChkInfo.health <= 0) {
            this->shouldStartDeath = true;
            Enemy_StartFinishingBlow(play, &this->actor);
            AudioSfx_StopByPos(&this->actor.projectedPos);
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_DEAD_OLD);
        } else {
            this->targetPos.x = this->actor.world.pos.x;
            this->targetPos.y = 200.0f;
            this->targetPos.z = this->actor.world.pos.z;
            this->actor.world.rot.x = 0x3000;
            this->timers[0] = 50;
            this->timers[2] = 100;
            this->flySpeedTarget = 6.0f;
            this->speedToTarget = 0.0f;
        }
    }
}

void Boss07_Mask_StopBeam(Boss07* this) {
    this->eyeBeamsLengthScale = 0.0f;
    this->eyeBeamsFocusOrbScale = 0.0f;
    this->beamLengthScale = 0.0f;
    this->beamBaseScale = 0.0f;
    this->reflectedBeamLengthScale = 0.0f;
}

void Boss07_Mask_SetupFireBeam(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Mask_FireBeam;
    this->subAction = MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_CHARGE_UP;
    this->timers[0] = 30;
    this->speedToTarget = 0.0f;
}

void Boss07_Mask_FireBeam(Boss07* this, PlayState* play) {
    f32 dx;
    f32 dz;
    f32 dy;
    f32 distXYZ;
    f32 yOffset;
    s16 rotScale;
    s16 i;
    Vec3f diff;
    Vec3f transformedDiff;
    Player* player = GET_PLAYER(play);
    CollisionPoly* poly;
    Vec3f beamTireMarkPos;
    u8 beamIsTouchingPoly = false;
    s32 bgId;

    this->damagedTimer = 20;
    Boss07_SmoothStop(this, 0.5f);
    Math_ApproachF(&this->actor.world.pos.y, 300.0f, 0.05f, 1.0f);
    Math_ApproachS(&this->actor.shape.rot.z, 0, 0xA, 0x400);

    if ((player->focusActor != NULL) && (player->stateFlags1 & PLAYER_STATE1_400000)) {
        yOffset = (player->transformation == PLAYER_FORM_HUMAN) ? 20 : 30.0f;
    } else {
        yOffset = (player->transformation == PLAYER_FORM_HUMAN) ? 8.0f : 15.0f;
    }

    rotScale = (player->stateFlags1 & PLAYER_STATE1_400000) ? 1 : 10;
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, rotScale, this->speedToTarget);
    dx = player->actor.world.pos.x - this->actor.world.pos.x;
    dy = player->actor.world.pos.y - this->actor.world.pos.y + yOffset;
    dz = player->actor.world.pos.z - this->actor.world.pos.z;
    Math_ApproachS(&this->actor.shape.rot.x, -Math_Atan2S(dy, sqrtf(SQ(dx) + SQ(dz))), rotScale, this->speedToTarget);
    Math_ApproachF(&this->speedToTarget, 0xFA0, 1.0f, 0xC8);
    this->tentacleState = MAJORAS_MASK_TENTACLE_STATE_FIRING_BEAM;

    switch (this->subAction) {
        case MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_CHARGE_UP:
            if (this->timers[0] == 25) {
                Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EN_LAST1_BLOW_OLD);
            }

            if (this->timers[0] == 0) {
                this->subAction = MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_FIRE_EYE_BEAMS;
                this->timers[0] = 6;
                this->beamBaseScale = 1.0f;
            }
            break;

        case MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_FIRE_EYE_BEAMS:
            Math_ApproachF(&this->eyeBeamsLengthScale, 1.0f, 1.0f, 0.2f);

            if (this->timers[0] == 0) {
                this->subAction = MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_GROW_FOCUS_LIGHT_ORB;
                this->timers[0] = 8;
            }
            break;

        case MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_GROW_FOCUS_LIGHT_ORB:
            Audio_PlaySfx(NA_SE_EN_LAST1_BEAM_OLD - SFX_FLAG);
            Math_ApproachF(&this->eyeBeamsFocusOrbScale, 1.0f, 0.2f, 0.2f);

            if (this->timers[0] == 0) {
                this->subAction = MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_BEAM_ACTIVE;
                this->timers[0] = 100;
            }
            break;

        case MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_BEAM_ACTIVE:
        case MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_BEAM_REFLECTED:
            Audio_PlaySfx(NA_SE_EN_LAST1_BEAM_OLD - SFX_FLAG);
            FALLTHROUGH;
        case MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_END:
            Math_ApproachF(&this->eyeBeamsFocusOrbScale, 1.0f, 0.2f, 0.2f);
            dx = player->actor.world.pos.x - this->beamStartPos.x;
            dy = player->actor.world.pos.y - this->beamStartPos.y + 20.0f;
            dz = player->actor.world.pos.z - this->beamStartPos.z;
            distXYZ = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));
            Math_ApproachF(&this->beamLengthScale, distXYZ * 0.2f, 1.0f, 7.0f);

            if (BgCheck_EntityLineTest1(&play->colCtx, &this->beamStartPos, &this->beamEndPos, &beamTireMarkPos, &poly,
                                        true, true, true, true, &bgId) &&
                (this->subAction != MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_END)) {
                Vec3f flamePos;
                Vec3f flameVelocity;
                Vec3f flameAccel;

                flamePos.x = Rand_CenteredFloat(20.0f) + beamTireMarkPos.x;
                flamePos.y = Rand_CenteredFloat(20.0f) + beamTireMarkPos.y;
                flamePos.z = Rand_CenteredFloat(20.0f) + beamTireMarkPos.z;

                flameVelocity.x = 0.0f;
                flameVelocity.y = 6.0f;
                flameVelocity.z = 0.0f;

                flameAccel.x = flameVelocity.x * -0.05f;
                flameAccel.y = flameVelocity.y * -0.05f;
                flameAccel.z = flameVelocity.z * -0.05f;

                Boss07_SpawnFlameEffect(play, &flamePos, &flameVelocity, &flameAccel, Rand_ZeroFloat(10.0f) + 25.0f);
                beamIsTouchingPoly = true;
            }

            diff.x = player->actor.world.pos.x - this->beamStartPos.x;
            diff.y = player->actor.world.pos.y - this->beamStartPos.y + 10.0f;
            diff.z = player->actor.world.pos.z - this->beamStartPos.z;
            Matrix_RotateXS(-this->actor.shape.rot.x, MTXMODE_NEW);
            Matrix_RotateYS(-this->actor.shape.rot.y, MTXMODE_APPLY);
            Matrix_MultVec3f(&diff, &transformedDiff);

            if ((fabsf(transformedDiff.x) < 20.0f) && (fabsf(transformedDiff.y) < 50.0f) &&
                (transformedDiff.z > 40.0f) && (transformedDiff.z <= (this->beamLengthScale * 20))) {
                if (Player_HasMirrorShieldEquipped(play) && (player->transformation == PLAYER_FORM_HUMAN) &&
                    (player->stateFlags1 & PLAYER_STATE1_400000) &&
                    (BINANG_ROT180(player->actor.shape.rot.y - this->actor.shape.rot.y) < 0x2000) &&
                    (BINANG_ROT180(player->actor.shape.rot.y - this->actor.shape.rot.y) > -0x2000)) {
                    Vec3s reflectedBeamRot;

                    this->beamLengthScale = distXYZ * 0.05f;
                    Math_ApproachF(&this->reflectedBeamLengthScale, distXYZ * 0.2f, 1.0f, 7.0f);
                    Matrix_MtxFToYXZRot(&player->shieldMf, &reflectedBeamRot, 0);
                    reflectedBeamRot.y += 0x8000;
                    reflectedBeamRot.x = -reflectedBeamRot.x;

                    if (this->subAction == MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_BEAM_ACTIVE) {
                        this->subAction = MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_BEAM_REFLECTED;
                        this->reflectedBeamPitch = reflectedBeamRot.x;
                        this->reflectedBeamYaw = reflectedBeamRot.y;
                    } else {
                        player->pushedYaw = this->actor.yawTowardsPlayer;
                        player->pushedSpeed = this->beamBaseScale * 0.5f;

                        sMajoraBattleHandler->lensFlareOn = true;
                        sMajoraBattleHandler->lensFlareScale = this->beamBaseScale * 30.0f;
                        sMajoraBattleHandler->lensFlarePos = this->beamEndPos;

                        Math_ApproachS(&this->reflectedBeamPitch, reflectedBeamRot.x, 2, 0x2000);
                        Math_ApproachS(&this->reflectedBeamYaw, reflectedBeamRot.y, 2, 0x2000);

                        diff.x = this->actor.world.pos.x - this->beamEndPos.x;
                        diff.y = this->actor.world.pos.y - this->beamEndPos.y;
                        diff.z = this->actor.world.pos.z - this->beamEndPos.z;
                        distXYZ = sqrtf(SQXYZ(diff));

                        Matrix_RotateXS(-this->reflectedBeamPitch, MTXMODE_NEW);
                        Matrix_RotateYS(-this->reflectedBeamYaw, MTXMODE_APPLY);
                        Matrix_Push();
                        Matrix_MultVec3f(&diff, &transformedDiff);

                        if ((fabsf(transformedDiff.x) < 60.0f) && (fabsf(transformedDiff.y) < 60.0f) &&
                            (transformedDiff.z > 40.0f) &&
                            (transformedDiff.z <= (this->reflectedBeamLengthScale * 16.666668f)) &&
                            (this->subAction != MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_END)) {
                            s32 j;
                            Vec3f flamePos;
                            Vec3f flameVelocity;
                            Vec3f flameAccel;

                            this->beamDamageTimer += 2;
                            this->reflectedBeamLengthScale = distXYZ * 0.062f;

                            if (this->beamDamageTimer < 10) {
                                flamePos.x = this->actor.world.pos.x + Rand_CenteredFloat(40.0f);
                                flamePos.y = this->actor.world.pos.y + Rand_CenteredFloat(40.0f);
                                flamePos.z = this->actor.world.pos.z + Rand_CenteredFloat(40.0f);

                                flameVelocity.x = 0.0f;
                                flameVelocity.y = 6.0f;
                                flameVelocity.z = 0.0f;

                                flameAccel.x = flameVelocity.x * -0.05f;
                                flameAccel.y = flameVelocity.y * -0.05f;
                                flameAccel.z = flameVelocity.z * -0.05f;

                                Boss07_SpawnFlameEffect(play, &flamePos, &flameVelocity, &flameAccel,
                                                        Rand_ZeroFloat(10.0f) + 25.0f);
                                this->damagedFlashTimer |= 10;
                            } else {
                                this->damagedTimer = 50;
                                this->damagedFlashTimer = 15;
                                AudioSfx_StopByPos(&this->actor.projectedPos);
                                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_DAMAGE2_OLD);
                                Boss07_Mask_SetupDamaged(this, play, 2, NULL);
                                Boss07_Mask_StopBeam(this);

                                for (j = 0; j < 20; j++) {
                                    flamePos.x = this->actor.world.pos.x + Rand_CenteredFloat(50.0f);
                                    flamePos.y = this->actor.world.pos.y + Rand_CenteredFloat(50.0f);
                                    flamePos.z = this->actor.world.pos.z + Rand_CenteredFloat(50.0f);

                                    flameVelocity.x = Rand_CenteredFloat(20.0f);
                                    flameVelocity.y = Rand_CenteredFloat(20.0f);
                                    flameVelocity.z = Rand_CenteredFloat(20.0f);

                                    flameAccel.x = flameVelocity.x * -0.05f;
                                    flameAccel.y = flameVelocity.y * -0.05f;
                                    flameAccel.z = flameVelocity.z * -0.05f;

                                    Boss07_SpawnFlameEffect(play, &flamePos, &flameVelocity, &flameAccel,
                                                            Rand_ZeroFloat(10.0f) + 25.0f);
                                }

                                if ((s8)this->actor.colChkInfo.health <= 0) {
                                    this->fireTimer = 200;
                                } else {
                                    this->fireTimer = 60;
                                }
                            }
                        }

                        Matrix_Pop();

                        for (i = 0; i < MAJORA_REMAINS_TYPE_MAX; i++) {
                            if (sMajoraRemains[i]->subAction >= REMAINS_MOVE_SUB_ACTION_DIE) {
                                continue;
                            }

                            diff.x = sMajoraRemains[i]->actor.world.pos.x - this->beamEndPos.x;
                            diff.y = sMajoraRemains[i]->actor.world.pos.y - this->beamEndPos.y;
                            diff.z = sMajoraRemains[i]->actor.world.pos.z - this->beamEndPos.z;
                            distXYZ = sqrtf(SQXYZ(diff));
                            Matrix_MultVec3f(&diff, &transformedDiff);

                            if ((fabsf(transformedDiff.x) < 60.0f) && (fabsf(transformedDiff.y) < 60.0f) &&
                                (transformedDiff.z > 40.0f) &&
                                (transformedDiff.z <= (this->reflectedBeamLengthScale * 16.666668f)) &&
                                (this->subAction != MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_END)) {
                                s32 j;
                                Vec3f flamePos;
                                Vec3f flameVelocity;
                                Vec3f flameAccel;

                                this->beamDamageTimer += 2;
                                this->reflectedBeamLengthScale = distXYZ * 0.062f;

                                if (this->beamDamageTimer < 5) {
                                    flamePos.x = sMajoraRemains[i]->actor.world.pos.x + Rand_CenteredFloat(40.0f);
                                    flamePos.y = sMajoraRemains[i]->actor.world.pos.y + Rand_CenteredFloat(40.0f);
                                    flamePos.z = sMajoraRemains[i]->actor.world.pos.z + Rand_CenteredFloat(40.0f);

                                    flameVelocity.x = 0.0f;
                                    flameVelocity.y = 6.0f;
                                    flameVelocity.z = 0.0f;

                                    flameAccel.x = flameVelocity.x * -0.05f;
                                    flameAccel.y = flameVelocity.y * -0.05f;
                                    flameAccel.z = flameVelocity.z * -0.05f;

                                    Boss07_SpawnFlameEffect(play, &flamePos, &flameVelocity, &flameAccel,
                                                            Rand_ZeroFloat(10.0f) + 25.0f);
                                    sMajoraRemains[i]->damagedFlashTimer |= 10;
                                } else {
                                    sMajoraRemains[i]->subAction = REMAINS_MOVE_SUB_ACTION_DIE;
                                    sMajoraRemains[i]->fireTimer = 60;
                                    Actor_PlaySfx(&this->actor, NA_SE_EN_FOLLOWERS_DEAD);

                                    for (j = 0; j < 20; j++) {
                                        flamePos.x = sMajoraRemains[i]->actor.world.pos.x + Rand_CenteredFloat(50.0f);
                                        flamePos.y = sMajoraRemains[i]->actor.world.pos.y + Rand_CenteredFloat(50.0f);
                                        flamePos.z = sMajoraRemains[i]->actor.world.pos.z + Rand_CenteredFloat(50.0f);

                                        flameVelocity.x = Rand_CenteredFloat(20.0f);
                                        flameVelocity.y = Rand_CenteredFloat(20.0f);
                                        flameVelocity.z = Rand_CenteredFloat(20.0f);

                                        flameAccel.x = flameVelocity.x * -0.05f;
                                        flameAccel.y = flameVelocity.y * -0.05f;
                                        flameAccel.z = flameVelocity.z * -0.05f;

                                        Boss07_SpawnFlameEffect(play, &flamePos, &flameVelocity, &flameAccel,
                                                                Rand_ZeroFloat(10.0f) + 25.0f);
                                    }
                                }
                            }
                        }

                        if (BgCheck_EntityLineTest1(&play->colCtx, &this->beamEndPos, &this->reflectedBeamEndPos,
                                                    &beamTireMarkPos, &poly, true, true, true, true, &bgId) &&
                            (this->subAction != MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_END)) {
                            Vec3f flamePos;
                            Vec3f flameVelocity;
                            Vec3f flameAccel;

                            beamIsTouchingPoly = true;

                            flamePos.x = Rand_CenteredFloat(20.0f) + beamTireMarkPos.x;
                            flamePos.y = Rand_CenteredFloat(20.0f) + beamTireMarkPos.y;
                            flamePos.z = Rand_CenteredFloat(20.0f) + beamTireMarkPos.z;

                            flameVelocity.x = 0.0f;
                            flameVelocity.y = 6.0f;
                            flameVelocity.z = 0.0f;

                            flameAccel.x = flameVelocity.x * -0.05f;
                            flameAccel.y = flameVelocity.y * -0.05f;
                            flameAccel.z = flameVelocity.z * -0.05f;

                            Boss07_SpawnFlameEffect(play, &flamePos, &flameVelocity, &flameAccel,
                                                    Rand_ZeroFloat(10.0f) + 25.0f);
                        }
                    }
                } else if (!player->bodyIsBurning && (this->subAction != MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_END)) {
                    s32 j;

                    func_800B8D50(play, &this->actor, 5.0f, this->actor.shape.rot.y, 0.0f, 0x10);

                    for (j = 0; j < ARRAY_COUNT(player->bodyFlameTimers); j++) {
                        player->bodyFlameTimers[j] = Rand_S16Offset(0, 200);
                    }

                    player->bodyIsBurning = true;
                    Player_PlaySfx(player, player->ageProperties->voiceSfxIdOffset + NA_SE_VO_LI_DEMO_DAMAGE);
                }
            }

            if (beamIsTouchingPoly) {
                if (beamTireMarkPos.y == 0.0f) {
                    dx = this->prevBeamTireMarkPos.x - beamTireMarkPos.x;
                    dz = this->prevBeamTireMarkPos.z - beamTireMarkPos.z;
                    func_800AE930(&play->colCtx, Effect_GetByIndex(this->effectIndex), &beamTireMarkPos, 15.0f,
                                  Math_Atan2S(dx, dz), poly, bgId);
                    this->beamTireMarkEnabled = true;
                }

                this->prevBeamTireMarkPos = beamTireMarkPos;
            }

            if (this->subAction != MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_END) {
                if (this->timers[0] == 0) {
                    this->subAction = MAJORAS_MASK_FIRE_BEAM_SUB_ACTION_END;
                    this->timers[0] = 20;
                }
            } else {
                Math_ApproachZeroF(&this->beamBaseScale, 1.0f, 0.05f);

                if (this->timers[0] == 0) {
                    Boss07_Mask_SetupIdle(this, play);
                    this->timers[2] = 100;
                    Boss07_Mask_StopBeam(this);
                }
            }
            break;

        default:
            break;
    }
}

void Boss07_Mask_SetupIntroCutscene(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Mask_IntroCutscene;
    this->actor.world.pos.x = 0.0f;
    this->actor.world.pos.y = 300.0f;
    this->actor.world.pos.z = -922.5f;
    this->motionBlurAlpha = KREG(75) + 150;
    Play_EnableMotionBlur(this->motionBlurAlpha);
}

static Vec3s sRemainsAttachSubCamEyes[MAJORA_REMAINS_TYPE_MAX] = {
    { 616, 402, -46 },  // MAJORA_REMAINS_TYPE_ODOLWA
    { -622, 380, -86 }, // MAJORA_REMAINS_TYPE_GYORG
    { 400, 300, 463 },  // MAJORA_REMAINS_TYPE_GOHT
    { -400, 470, 496 }, // MAJORA_REMAINS_TYPE_TWINMOLD
};

static Vec3s sRemainsAttachSubCamAts[MAJORA_REMAINS_TYPE_MAX] = {
    { 646, 394, -150 },  // MAJORA_REMAINS_TYPE_ODOLWA
    { -648, 380, -190 }, // MAJORA_REMAINS_TYPE_GYORG
    { 502, 321, 438 },   // MAJORA_REMAINS_TYPE_GOHT
    { -500, 445, 468 },  // MAJORA_REMAINS_TYPE_TWINMOLD
};

void Boss07_Mask_IntroCutscene(Boss07* this, PlayState* play) {
    f32 subCamAtYOffset = 0.0f;
    Player* player = GET_PLAYER(play);

    this->cutsceneTimer++;
    R_MOTION_BLUR_ALPHA = this->motionBlurAlpha;
    this->maskEyeTexIndex = MAJORAS_MASK_EYE_DULL;

    switch (this->cutsceneState) {
        case MAJORAS_MASK_INTRO_CS_STATE_WAITING_FOR_PLAYER_OR_DONE:
            this->actor.world.pos.x = 0.0f;
            this->actor.world.pos.y = 277.0f;
            this->actor.world.pos.z = -922.5f;

            if (!((player->actor.world.pos.z < 700.0f) && (CutsceneManager_GetCurrentCsId() == CS_ID_NONE))) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            this->cutsceneTimer = 0;
            this->cutsceneState = MAJORAS_MASK_INTRO_CS_STATE_LOOK_AT_PLAYER;
            this->motionBlurAlpha = KREG(76) + 150;
            FALLTHROUGH;
        case MAJORAS_MASK_INTRO_CS_STATE_LOOK_AT_PLAYER:
            if (this->cutsceneTimer >= 20) {
                Audio_PlaySfx_2(NA_SE_EV_LIGHT_GATHER - SFX_FLAG);
                Math_ApproachF(&sMajoraBattleHandler->introPlayerOrbScale, sREG(50) + 1.0f, 0.05f, sREG(51) + 0.05f);
            }

            if (this->cutsceneTimer == 35) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_15);
            }

            player->actor.world.pos.x = 0.0f;
            player->actor.world.pos.z = 700.0f;
            player->actor.shape.rot.y = -0x8000;
            player->actor.world.rot.y = player->actor.shape.rot.y;

            this->subCamEye.x = 0.0f;
            this->subCamEye.y = Player_GetHeight(player) + player->actor.world.pos.y - 24.0f;
            this->subCamEye.z = player->actor.world.pos.z - 60.0f;

            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = Player_GetHeight(player) + player->actor.world.pos.y - 20.0f;
            this->subCamAt.z = player->actor.world.pos.z;

            if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
                this->subCamAt.y -= 30.0f;
            }

            if (this->cutsceneTimer == 75) {
                this->cutsceneTimer = 0;
                this->cutsceneState = MAJORAS_MASK_INTRO_CS_STATE_REMAINS_ATTACH_TO_WALL;
                this->subCamEye.x = player->actor.world.pos.x;
                this->subCamEye.y = player->actor.world.pos.y + 20.0f;
                this->subCamEye.z = player->actor.world.pos.z - 60.0f + 120.0f;
                this->subCamAt.x = player->actor.world.pos.x;
                this->subCamAt.y = player->actor.world.pos.y + 24.0f + 5.0f;
                this->subCamAt.z = player->actor.world.pos.z;
            }

            break;

        case MAJORAS_MASK_INTRO_CS_STATE_REMAINS_ATTACH_TO_WALL:
            this->motionBlurAlpha = KREG(77) + 150;

            if (this->cutsceneTimer >= 20) {
                Math_ApproachZeroF(&sMajoraBattleHandler->introPlayerOrbScale, 1.0f, 0.05f);
            }

            if (this->cutsceneTimer == 20) {
                sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA]->subAction = REMAINS_INTRO_CS_SUB_ACTION_FLY;
                sMajoraRemains[MAJORA_REMAINS_TYPE_GYORG]->subAction = REMAINS_INTRO_CS_SUB_ACTION_FLY;
                sMajoraRemains[MAJORA_REMAINS_TYPE_GOHT]->subAction = REMAINS_INTRO_CS_SUB_ACTION_FLY;
                sMajoraRemains[MAJORA_REMAINS_TYPE_TWINMOLD]->subAction = REMAINS_INTRO_CS_SUB_ACTION_FLY;
            }

            if (this->cutsceneTimer == 0) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            }

            if (this->cutsceneTimer == 120) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_21);
            }

            if (this->cutsceneTimer > 30) {
                Math_ApproachF(&this->subCamAt.y, player->actor.world.pos.y + 24.0f + 20.0f, 0.05f,
                               this->subCamVelocity);
                Math_ApproachF(&this->subCamVelocity, 1.0f, 1.0f, 0.01f);
            }

            if (this->cutsceneTimer >= 160) {
                if (this->cutsceneTimer == 160) {
                    sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA]->subAction = REMAINS_INTRO_CS_SUB_ACTION_ATTACH_WAIT;
                    sMajoraRemains[MAJORA_REMAINS_TYPE_GYORG]->subAction = REMAINS_INTRO_CS_SUB_ACTION_ATTACH_WAIT;
                    sMajoraRemains[MAJORA_REMAINS_TYPE_GOHT]->subAction = REMAINS_INTRO_CS_SUB_ACTION_ATTACH_WAIT;
                    sMajoraRemains[MAJORA_REMAINS_TYPE_TWINMOLD]->subAction = REMAINS_INTRO_CS_SUB_ACTION_ATTACH_WAIT;
                }

                if (this->cutsceneTimer == 161) {
                    sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA]->subAction = REMAINS_INTRO_CS_SUB_ACTION_ATTACH_TO_WALL;
                }

                // This code uses `sfxTimer` as an index to track which of the remains is currently attaching itself to
                // the wall. The index is used to update the camera's position and orientation, and as it increases,
                // this code will also signal to the appropriate remains to start attaching to the wall.
                if ((this->cutsceneTimer == 180) || (this->cutsceneTimer == 200) || (this->cutsceneTimer == 220)) {
                    this->sfxTimer++;
                    sMajoraRemains[this->sfxTimer]->subAction = REMAINS_INTRO_CS_SUB_ACTION_ATTACH_TO_WALL;
                }

                this->subCamEye.x = sRemainsAttachSubCamEyes[this->sfxTimer].x;
                this->subCamEye.y = sRemainsAttachSubCamEyes[this->sfxTimer].y;
                this->subCamEye.z = sRemainsAttachSubCamEyes[this->sfxTimer].z;
                this->subCamAt.x = sRemainsAttachSubCamAts[this->sfxTimer].x;
                this->subCamAt.y = sRemainsAttachSubCamAts[this->sfxTimer].y;
                this->subCamAt.z = sRemainsAttachSubCamAts[this->sfxTimer].z;

                if (this->cutsceneTimer == 250) {
                    this->cutsceneState = MAJORAS_MASK_INTRO_CS_STATE_WAKE_UP;
                    this->cutsceneTimer = 0;

                    this->subCamEye.x = this->actor.world.pos.x;
                    this->subCamEye.y = this->actor.world.pos.y;
                    this->subCamEye.z = 300.0f;

                    this->subCamAt.x = this->actor.world.pos.x;
                    this->subCamAt.y = this->actor.world.pos.y;
                    this->subCamAt.z = this->actor.world.pos.z;

                    Boss07_Remains_SetupMove(sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA], play);
                    Boss07_Remains_SetupMove(sMajoraRemains[MAJORA_REMAINS_TYPE_GYORG], play);
                    Boss07_Remains_SetupMove(sMajoraRemains[MAJORA_REMAINS_TYPE_GOHT], play);
                    Boss07_Remains_SetupMove(sMajoraRemains[MAJORA_REMAINS_TYPE_TWINMOLD], play);
                    this->subCamVelocity = 0.0f;
                    sMajoraBattleHandler->introPlayerOrbScale = 0.0f;
                    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 10);
                }
            }
            break;

        case MAJORAS_MASK_INTRO_CS_STATE_WAKE_UP:
            this->motionBlurAlpha = KREG(78) + 150;

            if (this->cutsceneTimer >= 15) {
                Math_ApproachF(&this->subCamEye.z, -700.0f, 0.4f, 100.0f);
            }

            if (this->cutsceneTimer >= 55) {
                if (this->cutsceneTimer == 55) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_B_PAMET_BREAK);
                }

                this->maskEyeTexIndex = MAJORAS_MASK_EYE_NORMAL;
            }

            if (this->cutsceneTimer >= 75) {
                s32 i;
                Vec3f hahenVelocity = { 0.0f, 0.0f, 0.0f };
                Vec3f hahenAccel = { 0.0f, -0.5f, 0.0f };
                Vec3f hahenPos;

                if (this->cutsceneTimer == 75) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_DEMO_WALL);
                }

                subCamAtYOffset = 2.0f * (this->cutsceneTimer % 2);

                if ((this->cutsceneTimer % 2) != 0) {
                    this->actor.world.pos.x += 2.0f;
                } else {
                    this->actor.world.pos.x -= 2.0f;
                }

                for (i = 0; i < 3; i++) {
                    hahenPos.x = this->actor.world.pos.x + Rand_CenteredFloat(110.0f);
                    hahenPos.y = this->actor.world.pos.y + Rand_CenteredFloat(110.0f);
                    hahenPos.z = this->actor.world.pos.z + 10.0f;
                    EffectSsHahen_Spawn(play, &hahenPos, &hahenVelocity, &hahenAccel, 0,
                                        (Rand_ZeroOne() * 5.0f) + 10.0f, HAHEN_OBJECT_DEFAULT, 10, NULL);
                }
            }

            if (this->cutsceneTimer == 110) {
                this->cutsceneState = MAJORAS_MASK_INTRO_CS_STATE_DETACH_FROM_WALL;
                this->cutsceneTimer = 0;

                this->subCamEye.x = this->actor.world.pos.x + 200.0f;
                this->subCamEye.y = this->actor.world.pos.y;
                this->subCamEye.z = this->actor.world.pos.z + 400.0f;

                player->actor.world.pos.z = 0.0f;
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_1);
                this->motionBlurAlpha = KREG(74) + 200;
            }
            break;

        case MAJORAS_MASK_INTRO_CS_STATE_DETACH_FROM_WALL:
            this->maskEyeTexIndex = MAJORAS_MASK_EYE_NORMAL;

            if (this->cutsceneTimer >= 10) {
                Math_ApproachS(&this->motionBlurAlpha, KREG(72), 1, KREG(73) + 2);

                if (this->cutsceneTimer == 10) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_DEMO_BREAK);
                }

                SkelAnime_Update(&this->skelAnime);
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_FLOAT_OLD - SFX_FLAG);
                Math_ApproachF(&this->tentacleLengthScale, 1.0f, 1.0f, 0.02f);
                Math_ApproachF(&this->actor.world.pos.z, -642.5f, 0.05f, 30.0f);
                Math_ApproachF(&this->actor.world.pos.y, 350.0f, 0.03f, 2.0f);

                if (this->cutsceneTimer == 55) {
                    TitleCard_InitBossName(&play->state, &play->actorCtx.titleCtx,
                                           Lib_SegmentedToVirtual(&gMajorasMaskTitleCardTex), 160, 180, 128, 40);
                }

                if (this->cutsceneTimer == 30) {
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_MAJORAS_MASK | SEQ_FLAG_ASYNC);
                }

                if (this->cutsceneTimer > 100) {
                    Math_ApproachF(&this->subCamEye.x, player->actor.world.pos.x + 40.0f, 0.1f,
                                   this->subCamVelocity * 20.0f);
                    Math_ApproachF(&this->subCamEye.y, player->actor.world.pos.y + 10.0f, 0.1f,
                                   this->subCamVelocity * 20.0f);
                    Math_ApproachF(&this->subCamEye.z, player->actor.world.pos.z + 90.0f, 0.1f,
                                   this->subCamVelocity * 60.0f);
                    Math_ApproachF(&this->subCamVelocity, 1.0f, 1.0f, 0.03f);
                }

                if (this->cutsceneTimer == 175) {
                    Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

                    this->cutsceneState = MAJORAS_MASK_INTRO_CS_STATE_WAITING_FOR_PLAYER_OR_DONE;
                    Boss07_Mask_SetupIdle(this, play);
                    this->timers[0] = 50;
                    this->timers[2] = 200;
                    this->bgCheckTimer = 50;
                    this->targetPos.x = 0.0f;
                    this->targetPos.y = 200.0f;
                    this->targetPos.z = 0.0f;
                    this->speedToTarget = 0.0f;
                    mainCam->eye = this->subCamEye;
                    mainCam->eyeNext = this->subCamEye;
                    mainCam->at = this->subCamAt;
                    func_80169AFC(play, this->subCamId, 0);
                    this->subCamId = SUB_CAM_ID_DONE;
                    Cutscene_StopManual(play, &play->csCtx);
                    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                    SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_MAJORA);
                    Play_DisableMotionBlur();
                }
            } else if ((this->cutsceneTimer % 2) != 0) {
                this->actor.world.pos.x += 2.0f;
            } else {
                this->actor.world.pos.x -= 2.0f;
            }
            Math_ApproachF(&this->subCamAt.x, this->actor.world.pos.x, 0.05f, 10.0f);
            Math_ApproachF(&this->subCamAt.y, this->actor.world.pos.y, 0.05f, 10.0f);
            Math_ApproachF(&this->subCamAt.z, this->actor.world.pos.z, 0.05f, 10.0f);
            break;

        default:
            break;
    }
    if (this->subCamId != SUB_CAM_ID_DONE) {
        Vec3f subCamAt;

        ShrinkWindow_Letterbox_SetSizeTarget(27);
        subCamAt.x = this->subCamAt.x;
        subCamAt.y = this->subCamAt.y + subCamAtYOffset;
        subCamAt.z = this->subCamAt.z;
        Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &this->subCamEye);
    }
}

void Boss07_Mask_SetupDeathCutscene(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Mask_DeathCutscene;
    Animation_MorphToLoop(&this->skelAnime, &gMajorasMaskJerkingAnim, -10.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y =
        Math_Atan2F_XY(-this->actor.world.pos.z, -this->actor.world.pos.x) * (0x8000 / M_PIf);
    this->cutsceneState = MAJORAS_MASK_DEATH_CS_STATE_STARTED;
    this->cutsceneTimer = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->damagedTimer = 20;
}

void Boss07_Mask_DeathCutscene(Boss07* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f subCamAtTarget;
    Vec3f subCamEye;

    this->cutsceneTimer++;
    this->damagedTimer = 20;
    this->maskEyeTexIndex = MAJORAS_MASK_EYE_DULL;
    SkelAnime_Update(&this->skelAnime);
    Boss07_SmoothStop(this, 0.5f);

    Math_ApproachF(&this->actor.world.pos.x, 0.0f, 0.05f, 5.0f);
    Math_ApproachF(&this->actor.world.pos.z, 0.0f, 0.05f, 5.0f);
    Math_ApproachF(&this->actor.world.pos.y, 130.0f, 0.05f, 3.0f);

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);

    switch (this->cutsceneState) {
        case MAJORAS_MASK_DEATH_CS_STATE_STARTED:
            if (CutsceneManager_GetCurrentCsId() == CS_ID_NONE) {
                Cutscene_StartManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_1);
                this->subCamId = Play_CreateSubCamera(play);
                Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
                Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
                this->cutsceneTimer = 0;
                this->cutsceneState = MAJORAS_MASK_DEATH_CS_STATE_PLAYING;
                this->subCamAtNext.z = 0.0f;
                this->fireTimer = 120;
                Play_EnableMotionBlur(150);

                this->subCamEyeNext.x = 0.0f;
                this->subCamEyeNext.y = -30.0f;
                this->subCamEyeNext.z = 270.0f;

                this->subCamAtNext.x = 0.0f;
                this->subCamAtNext.y = -30.0f;

                Matrix_MultVec3f(&this->subCamEyeNext, &this->subCamEye);
                Matrix_MultVec3f(&this->subCamAtNext, &this->subCamAt);
            } else {
                break;
            }
            FALLTHROUGH;
        case MAJORAS_MASK_DEATH_CS_STATE_PLAYING:
            player->actor.world.pos.x = 0.0f;
            player->actor.world.pos.z = BREG(87) + 250.0f;
            player->actor.world.rot.y = player->actor.shape.rot.y = -0x8000;

            this->tentacleState = MAJORAS_MASK_TENTACLE_STATE_DEATH;

            if (this->cutsceneTimer > 60) {
                Math_ApproachS(&this->actor.shape.rot.x, 0, 0xA, 0x200);
                Math_ApproachS(&this->actor.shape.rot.z, 0, 0xA, 0x200);
                Math_ApproachZeroF(&this->tentacleLengthScale, 1.0f, 0.01f);
            } else {
                this->actor.shape.rot.x += 0x1000;
                this->actor.shape.rot.z += 0x1200;
                Math_ApproachZeroF(&this->tentacleLengthScale, 1.0f, 0.005f);
            }

            if (this->cutsceneTimer > 130) {
                this->cutsceneState = MAJORAS_MASK_DEATH_CS_STATE_SPAWN_INCARNATION;
            } else {
                break;
            }
            FALLTHROUGH;
        case MAJORAS_MASK_DEATH_CS_STATE_SPAWN_INCARNATION:
            Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_07, 0.0f, 0.0f, 0.0f, 0, this->actor.shape.rot.y,
                        this->subCamId, MAJORA_PARAMS(MAJORA_TYPE_INCARNATION));
            Actor_Kill(&this->actor);
            break;

        default:
            break;
    }

    Matrix_MultVec3f(&this->subCamEyeNext, &this->subCamEye);
    Matrix_MultVec3f(&this->subCamAtNext, &subCamAtTarget);

    Math_ApproachF(&this->subCamAt.x, subCamAtTarget.x, 0.1f, 10.0f);
    Math_ApproachF(&this->subCamAt.y, subCamAtTarget.y, 0.1f, 10.0f);
    Math_ApproachF(&this->subCamAt.z, subCamAtTarget.z, 0.1f, 10.0f);

    if (this->subCamId != SUB_CAM_ID_DONE) {
        ShrinkWindow_Letterbox_SetSizeTarget(27);
        subCamEye = this->subCamEye;
        if (subCamEye.y < 30.0f) {
            subCamEye.y = 30.0f;
        }

        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &subCamEye);
    }
}

void Boss07_Mask_UpdateDamage(Boss07* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    ColliderElement* acHitElem;
    u8 damage;
    Actor* hitActor;

    if (this->damagedTimer == 0) {
        if (this->maskFrontCollider.base.acFlags & AC_HIT) {
            this->maskFrontCollider.base.acFlags &= ~AC_HIT;
            this->maskShakeTimer = 7;
        }

        if (this->maskBackCollider.base.acFlags & AC_HIT) {
            this->maskBackCollider.base.acFlags &= ~AC_HIT;
            this->maskShakeTimer = 15;

            if ((this->actionFunc == Boss07_Mask_Stunned) || (player->stateFlags3 & PLAYER_STATE3_200)) {
                hitActor = this->maskBackCollider.base.ac;
                acHitElem = this->maskBackCollider.elem.acHitElem;
                damage = (acHitElem->atDmgInfo.dmgFlags & ~0x8300000) ? this->actor.colChkInfo.damage : 0;
                this->damagedTimer = 50;
                this->damagedFlashTimer = 15;
                AudioSfx_StopByPos(&this->actor.projectedPos);
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_DAMAGE2_OLD);
                Boss07_Mask_SetupDamaged(this, play, damage, hitActor);
            } else {
                this->damagedTimer = 15;
                AudioSfx_StopByPos(&this->actor.projectedPos);
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_DAMAGE1_OLD);
                Boss07_Mask_SetupStunned(this, play);
            }
        }
    }
}

void Boss07_Mask_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;
    s32 i;
    Player* player = GET_PLAYER(play);
    Vec3f topLeftVertex;
    Vec3f bottomLeftVertex;
    Vec3f topRightVertex;
    Vec3f bottomRightVertex;
    Vec3f temp;
    f32 backColliderXY;

    this->actor.hintId = TATL_HINT_ID_MAJORAS_MASK;

    if (sMusicStartTimer != 0) {
        sMusicStartTimer--;
        if (sMusicStartTimer == 0) {
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_MAJORAS_MASK | SEQ_FLAG_ASYNC);
        }
    }

    Math_Vec3f_Copy(&sMajoraSfxPos, &this->actor.projectedPos);

    if ((sMajoraBattleHandler == NULL) || (sMajoraBattleHandler->subCamId == SUB_CAM_ID_DONE)) {
        this->shouldUpdateTentaclesOrWhips = true;
        play->envCtx.lightSetting = 2;
        play->envCtx.prevLightSetting = 0;
        Math_ApproachF(&play->envCtx.lightBlend, this->beamBaseScale, 1.0f, 0.1f);
        this->frameCounter++;

        if (KREG(63) == 0) {
            this->canEvade = false;
            this->maskEyeTexIndex = MAJORAS_MASK_EYE_NORMAL;
            this->tentacleState = MAJORAS_MASK_TENTACLE_STATE_DEFAULT;
            Actor_SetScale(&this->actor, 0.1f);
            this->actor.focus.pos = this->actor.world.pos;

            for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
                DECR(this->timers[i]);
            }

            DECR(this->maskShakeTimer);
            DECR(this->damagedTimer);
            DECR(this->damagedFlashTimer);
            DECR(this->fireTimer);
            DECR(this->beamDamageTimer);

            this->beamTireMarkEnabled = false;

            this->actionFunc(this, play);

            if (!this->beamTireMarkEnabled && this->prevBeamTireMarkEnabled) {
                func_800AEF44(Effect_GetByIndex(this->effectIndex));
            }

            this->prevBeamTireMarkEnabled = this->beamTireMarkEnabled;

            if (this->actionFunc != Boss07_Mask_IntroCutscene) {
                if (this->actionFunc != Boss07_Mask_Damaged) {
                    Actor_UpdateVelocityWithoutGravity(&this->actor);
                    Actor_UpdatePos(&this->actor);
                } else {
                    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                        this->actor.velocity.y = 0.0f;
                    }

                    Actor_MoveWithGravity(&this->actor);
                }

                if (this->bgCheckTimer == 0) {
                    Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 60.0f, 100.0f,
                                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
                } else {
                    this->bgCheckTimer--;
                }
            }
        } else {
            this->actor.colChkInfo.health = 0;
        }

        Boss07_Mask_UpdateDamage(this, play);
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);

        temp.x = -55.0f;
        temp.y = 55.0f;
        temp.z = 10.0f;
        Matrix_MultVec3f(&temp, &topLeftVertex);

        temp.x = -55.0f;
        temp.y = -55.0f;
        Matrix_MultVec3f(&temp, &bottomLeftVertex);

        temp.x = 55.0f;
        temp.y = 55.0f;
        Matrix_MultVec3f(&temp, &topRightVertex);

        temp.x = 55.0f;
        temp.y = -55.0f;
        Matrix_MultVec3f(&temp, &bottomRightVertex);

        Collider_SetQuadVertices(&this->maskFrontCollider, &topLeftVertex, &bottomLeftVertex, &topRightVertex,
                                 &bottomRightVertex);

        backColliderXY = (player->stateFlags3 & PLAYER_STATE3_200) ? 70.0f : 40.0f;

        temp.x = -backColliderXY;
        temp.y = backColliderXY;
        temp.z = -10.0f;
        Matrix_MultVec3f(&temp, &topLeftVertex);

        temp.x = -backColliderXY;
        temp.y = -backColliderXY;
        Matrix_MultVec3f(&temp, &bottomLeftVertex);

        temp.x = backColliderXY;
        temp.y = backColliderXY;
        Matrix_MultVec3f(&temp, &topRightVertex);

        temp.x = backColliderXY;
        temp.y = -backColliderXY;
        Matrix_MultVec3f(&temp, &bottomRightVertex);

        Collider_SetQuadVertices(&this->maskBackCollider, &topLeftVertex, &bottomLeftVertex, &topRightVertex,
                                 &bottomRightVertex);

        if (player->stateFlags3 & PLAYER_STATE3_200) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->maskBackCollider.base);
        } else {
            if ((this->actionFunc != Boss07_Mask_Stunned) && (this->actionFunc != Boss07_Mask_Damaged)) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->maskFrontCollider.base);
            }

            CollisionCheck_SetAC(play, &play->colChkCtx, &this->maskBackCollider.base);

            if (this->actionFunc == Boss07_Mask_SpinAttack) {
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->maskFrontCollider.base);
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->maskBackCollider.base);
            }
        }

        if (this->fireTimer != 0) {
            Vec3f flamePos;
            Vec3f flameVelocity;
            Vec3f flameAccel;
            f32 flamePosRandScale = (this->actionFunc == Boss07_Mask_DeathCutscene) ? 130.0f : 80.0f;

            flamePos.x = Rand_CenteredFloat(flamePosRandScale) + this->actor.world.pos.x;
            flamePos.y = Rand_CenteredFloat(flamePosRandScale) + this->actor.world.pos.y;
            flamePos.z = Rand_CenteredFloat(flamePosRandScale) + this->actor.world.pos.z;

            flameVelocity.x = 0.0f;
            flameVelocity.y = 5.0f;
            flameVelocity.z = 0.0f;

            flameAccel.x = flameVelocity.x * -0.05f;
            flameAccel.y = flameVelocity.y * -0.05f;
            flameAccel.z = flameVelocity.z * -0.05f;

            Boss07_SpawnFlameEffect(play, &flamePos, &flameVelocity, &flameAccel, Rand_ZeroFloat(10.0f) + 25.0f);
            Audio_PlaySfx_AtPos(&sMajoraSfxPos, NA_SE_EV_BURN_OUT - SFX_FLAG);
        }
    }
}

void Boss07_Mask_UpdateTentacles(Boss07* this, PlayState* play, Vec3f* base, Vec3f* pos, Vec3f* rot, Vec3f* velocity,
                                 Vec3f* baseForce, f32 lengthScale, u8 isMovingBackwards, f32 rotZ) {
    s32 i;
    f32 gravity;
    f32 segPitch;
    f32 segYaw;
    f32 tempY;
    f32 temp;
    Vec3f tempPos;
    Vec3f baseSegVec = { 0.0f, 0.0f, 0.0f };
    Vec3f segVec;
    Vec3f shapeForce;
    Vec3f* firstVelocity = velocity;

    if (this->tentacleState != MAJORAS_MASK_TENTACLE_STATE_DEFAULT) {
        for (i = 0; i < MAJORA_TENTACLE_LENGTH; i++) {
            Matrix_Push();

            Matrix_RotateZF(rotZ, MTXMODE_APPLY);
            baseSegVec.x = Math_SinS((2 * i + this->frameCounter) * 0x1600) * 10;
            baseSegVec.y = 10.0f;
            baseSegVec.z = 0.0f;

            Matrix_MultVec3f(&baseSegVec, &segVec);
            pos[i].x += segVec.x;
            pos[i].y += segVec.y;
            pos[i].z += segVec.z;

            Matrix_Pop();
        }
    }

    for (i = 0; i < MAJORA_TENTACLE_LENGTH; i++, velocity++) {
        if (i == 0) {
            pos[0] = *base;
            continue;
        }

        Math_ApproachF(&velocity->x, 0.0f, 1.0f, 1.0f);
        Math_ApproachF(&velocity->y, 0.0f, 1.0f, 1.0f);
        Math_ApproachF(&velocity->z, 0.0f, 1.0f, 1.0f);
    }

    pos++;
    rot++;
    velocity = firstVelocity + 1;
    baseSegVec.x = baseSegVec.y = 0.0f;
    baseSegVec.z = lengthScale * 23.0f;

    for (i = 1; i < MAJORA_TENTACLE_LENGTH; i++, velocity++, pos++, rot++) {
        if (i < MAJORA_TENTACLE_LENGTH / 2) {
            shapeForce.x = baseForce->x * (MAJORA_TENTACLE_LENGTH / 2 - i) * 0.2f;
            shapeForce.y = baseForce->y * (MAJORA_TENTACLE_LENGTH / 2 - i) * 0.2f;
            shapeForce.z = baseForce->z * (MAJORA_TENTACLE_LENGTH / 2 - i) * 0.2f;
        } else {
            shapeForce = gZeroVec3f;
        }

        // Advances the physics one frame to find the provisional position relative to the previous point
        tempPos.x = pos->x + velocity->x - (pos - 1)->x + shapeForce.x;

        if (this->tentacleState != MAJORAS_MASK_TENTACLE_STATE_DEFAULT) {
            gravity = 0.0f;
        } else if (isMovingBackwards && ((this->actor.world.pos.y - 30.0f) < (pos - 1)->y)) {
            gravity = -30.0f;
        } else {
            gravity = -3.0f - ((i & 7) * 0.05f);
        }

        tempY = pos->y + velocity->y + gravity + shapeForce.y;
        if (tempY < 2.0f) {
            tempY = 2.0f;
        }

        tempPos.y = tempY - (pos - 1)->y;
        tempPos.z = pos->z + velocity->z - (pos - 1)->z + shapeForce.x;

        // calculates the rotation angles from the previous point
        segYaw = Math_Atan2F_XY(tempPos.z, tempPos.x);
        temp = sqrtf(SQXZ(tempPos));
        segPitch = -Math_Atan2F_XY(temp, tempPos.y);
        (rot - 1)->y = segYaw;
        (rot - 1)->x = segPitch;

        // Sets the position to be in the same direction as the provisional position relative to the previous point, but
        // a fixed distance away
        Matrix_RotateYF(segYaw, MTXMODE_NEW);
        Matrix_RotateXFApply(segPitch);
        Matrix_MultVecZ(baseSegVec.z, &segVec);

        tempPos.x = pos->x;
        tempPos.y = pos->y;
        tempPos.z = pos->z;

        pos->x = (pos - 1)->x + segVec.x;
        pos->y = (pos - 1)->y + segVec.y;
        pos->z = (pos - 1)->z + segVec.z;

        // calculates the velocity for the next frame
        velocity->x = (pos->x - tempPos.x) * 0.85f;
        velocity->y = (pos->y - tempPos.y) * 0.85f;
        velocity->z = (pos->z - tempPos.z) * 0.85f;
    }
}

void Boss07_Mask_DrawTentacles(Boss07* this, PlayState* play, Vec3f* pos, Vec3f* rot, f32 lengthScale, f32 rotZ) {
    s32 i;
    f32 thicknessScale;

    OPEN_DISPS(play->state.gfxCtx);

    for (i = 0; i < MAJORA_TENTACLE_LENGTH - 1; i++, pos++, rot++) {
        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);
        Matrix_RotateYF(rot->y, MTXMODE_APPLY);
        Matrix_RotateXFApply(rot->x);
        Matrix_RotateZF(rotZ, MTXMODE_APPLY);

        if (i <= (MAJORA_TENTACLE_LENGTH - 1) / 2) {
            thicknessScale = 0.035f;
        } else {
            thicknessScale = 0.035f - (i - (MAJORA_TENTACLE_LENGTH - 1) / 2) * 60.0f * 0.0001f;
        }

        Matrix_Scale(thicknessScale, thicknessScale, lengthScale * 0.01f * 2.3f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gMajorasMaskTentacleModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Boss07_Mask_DrawBeam(Boss07* this, PlayState* play) {
    s32 pad[4];

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actionFunc == Boss07_Mask_FireBeam) {
        gSPSegment(POLY_XLU_DISP++, 0x0C,
                   Gfx_TexScrollEx(play->state.gfxCtx, 0, (this->frameCounter * -15) & 0xFF, 32, 64, 0, -15));
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 60, 200);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 128);
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

        Matrix_Push();
        {
            Matrix_Push();
            {
                Matrix_Push();
                {
                    Matrix_Translate(250.0f, 0.0f, 200.0f, MTXMODE_APPLY);
                    Matrix_RotateYS(-0xA00, MTXMODE_APPLY);
                    Matrix_Scale(this->beamBaseScale * 0.05f, this->beamBaseScale * 0.05f,
                                 this->eyeBeamsLengthScale * 0.05f, MTXMODE_APPLY);
                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                    gSPDisplayList(POLY_XLU_DISP++, gMajorasMaskBeamDL);
                }
                Matrix_Pop();

                Matrix_Translate(-250.0f, 0.0f, 200.0f, MTXMODE_APPLY);
                Matrix_RotateYS(0xA00, MTXMODE_APPLY);
                Matrix_Scale(this->beamBaseScale * 0.05f, this->beamBaseScale * 0.05f,
                             this->eyeBeamsLengthScale * 0.05f, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gMajorasMaskBeamDL);
                gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);
            }
            Matrix_Pop();

            Matrix_Translate(0.0f, 0.0f, 1200.0f, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(this->eyeBeamsFocusOrbScale * 40.0f * this->beamBaseScale,
                         this->eyeBeamsFocusOrbScale * 40.0f * this->beamBaseScale, 0.0f, MTXMODE_APPLY);
            Matrix_RotateZS(this->frameCounter * 0x100, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 60, sREG(89) + 80);
            Matrix_Scale(6.0f, 6.0f, 0.0f, MTXMODE_APPLY);
            Matrix_RotateZS(0x4000, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 60, 200);
        }
        Matrix_Pop();

        Matrix_Translate(0.0f, 0.0f, 1150.0f, MTXMODE_APPLY);
        Matrix_MultZero(&this->beamStartPos);
        Matrix_Scale(this->beamBaseScale * 0.05f, this->beamBaseScale * 0.05f, (this->beamLengthScale * 0.01f) - 0.01f,
                     MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gMajorasMaskBeamDL);
        Matrix_MultVecZ(20100.0f, &this->beamEndPos);

        if (this->reflectedBeamLengthScale > 0.0f) {
            Vec3f lightOrbPos;

            Matrix_MultVecZ(20000.0f, &lightOrbPos);
            Matrix_Translate(this->beamEndPos.x, this->beamEndPos.y, this->beamEndPos.z, MTXMODE_NEW);
            Matrix_RotateYS(this->reflectedBeamYaw, MTXMODE_APPLY);
            Matrix_RotateXS(this->reflectedBeamPitch, MTXMODE_APPLY);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_Scale(this->beamBaseScale * 0.05f, this->beamBaseScale * 0.05f,
                         this->reflectedBeamLengthScale * 0.01f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gMajorasMaskBeamDL);

            Matrix_MultVecZ(20100.0f, &this->reflectedBeamEndPos);
            Matrix_Translate(lightOrbPos.x, lightOrbPos.y, lightOrbPos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(this->beamBaseScale * 5.0f, this->beamBaseScale * 5.0f, 0.0f, MTXMODE_APPLY);
            Matrix_RotateZS(this->frameCounter * 0x100, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);
            gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
        }
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void Boss07_Mask_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    Boss07* this = (Boss07*)thisx;

    if (limbIndex == MAJORAS_MASK_LIMB_FACE) {
        Matrix_MultVecX(500.0f, &this->tentacleBasePos);
    }
}

void Boss07_Mask_Draw(Actor* thisx, PlayState* play2) {
    static TexturePtr sMaskEyeTextures[] = { gMajorasMaskWithNormalEyesTex, gMajorasMaskWithDullEyesTex };
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;
    f32 shakeScale;
    f32 rotX;
    f32 rotY;
    f32 rotZ;
    f32 lengthScale;
    s32 i;
    s32 isMovingBackwards;
    Vec3f offsetVec;
    Vec3f tentacleOffset;
    Vec3f baseForce;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    shakeScale = this->maskShakeTimer * (M_PIf / 4.0f) * 0.06666667f;
    rotX = Math_SinS(this->maskShakeTimer * 0x3500) * shakeScale * 0.5f;
    rotY = Math_SinS(this->maskShakeTimer * 0x4500) * shakeScale;
    Matrix_RotateYF(rotY, MTXMODE_APPLY);
    Matrix_RotateXFApply(rotX);

    if ((this->damagedFlashTimer % 2) != 0) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
    }

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sMaskEyeTextures[this->maskEyeTexIndex]));
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, Boss07_Mask_PostLimbDraw,
                      &this->actor);
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    gSPDisplayList(POLY_OPA_DISP++, gMajorasMaskTentacleMaterialDL);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 155, 155, 80, 255);

    isMovingBackwards = ((this->actionFunc == Boss07_Mask_Idle) &&
                         (ABS_ALT((s16)(this->actor.world.rot.y - this->actor.shape.rot.y)) > 0x4000))
                            ? true
                            : false;

    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
    Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_MultVecZ(-3.0f, &baseForce);

    offsetVec.x = 0.0f;
    offsetVec.y = 20.0f;
    offsetVec.z = -2.0f;
    rotZ = 0.0f;

    for (i = 0; i < MAJORA_TENTACLE_COUNT; i++) {
        Matrix_Push();
        Matrix_Push();
        Matrix_RotateZF(rotZ, MTXMODE_APPLY);
        Matrix_MultVec3f(&offsetVec, &tentacleOffset);

        this->tentacles[i].base.x = this->tentacleBasePos.x + tentacleOffset.x;
        this->tentacles[i].base.y = this->tentacleBasePos.y + tentacleOffset.y;
        this->tentacles[i].base.z = this->tentacleBasePos.z + tentacleOffset.z;
        lengthScale = (1 - (i * 0.008f)) * this->tentacleLengthScale;

        Matrix_Pop();

        if (this->shouldUpdateTentaclesOrWhips) {
            Boss07_Mask_UpdateTentacles(this, play, &this->tentacles[i].base, this->tentacles[i].pos,
                                        this->tentacles[i].rot, this->tentacles[i].velocity, &baseForce, lengthScale,
                                        isMovingBackwards, rotZ);
        }

        Boss07_Mask_DrawTentacles(this, play, this->tentacles[i].pos, this->tentacles[i].rot, lengthScale, i * 0.9f);
        rotZ += 0.5f;
        offsetVec.y += 1.0f;

        Matrix_Pop();
    }

    Boss07_Mask_DrawBeam(this, play);
    this->shouldUpdateTentaclesOrWhips = false;

    CLOSE_DISPS(play->state.gfxCtx);
}

void Boss07_Projectile_Update(Actor* thisx, PlayState* play2) {
    s32 i;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 distToPlayerXZ;
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;
    Player* player = GET_PLAYER(play);

    this->frameCounter++;

    if (MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_PROJECTILE_REMAINS) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FOLLOWERS_BEAM - SFX_FLAG);
    }

    if (KREG(63) == 0) {
        Actor_SetScale(&this->actor, 3.5f);

        if (this->subAction == PROJECTILE_SUB_ACTION_SPAWN) {
            dx = player->actor.world.pos.x - this->actor.world.pos.x;
            dy = player->actor.world.pos.y - this->actor.world.pos.y + 20.0f;
            dz = player->actor.world.pos.z - this->actor.world.pos.z;

            this->actor.world.rot.y = Math_Atan2S(dx, dz);
            distToPlayerXZ = sqrtf(SQ(dx) + SQ(dz));
            this->actor.world.rot.x = Math_Atan2S(dy, distToPlayerXZ);
            this->subAction = PROJECTILE_SUB_ACTION_FLY;
            this->actor.speed = 30.0f;
            Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
            if (MAJORA_GET_TYPE(&this->actor) == MAJORA_TYPE_PROJECTILE_INCARNATION) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_LAST2_FIRE_OLD);
            }
        }

        for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
            DECR(this->timers[i]);
        }

        Actor_UpdateVelocityWithoutGravity(&this->actor);
        Actor_UpdatePos(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 31.0f, 100.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
        this->actor.shape.rot.z += 0x1200;

        if ((this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_WALL | BGCHECKFLAG_CEILING)) ||
            (this->generalCollider.base.atFlags & AT_HIT) || (this->generalCollider.base.atFlags & AT_HIT) ||
            sKillAllProjectiles) {
            Actor_Kill(&this->actor);
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, this->projectileColorIndex,
                        CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_LIGHT_RAYS));
        }

        Collider_UpdateCylinder(&this->actor, &this->generalCollider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->generalCollider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->generalCollider.base);
    }
}

void Boss07_Projectile_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, sProjectilePrimColors[this->projectileColorIndex][0],
                    sProjectilePrimColors[this->projectileColorIndex][1],
                    sProjectilePrimColors[this->projectileColorIndex][2], 255);
    gDPSetEnvColor(POLY_XLU_DISP++, sProjectileEnvColors[this->projectileColorIndex][0],
                   sProjectileEnvColors[this->projectileColorIndex][1],
                   sProjectileEnvColors[this->projectileColorIndex][2], 128);
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, 0.0f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);
    gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Boss07_Remains_UpdateDamage(Boss07* this, PlayState* play) {
    Vec3f knockbackMovement;
    ColliderElement* acHitElem;

    if ((this->damagedTimer == 0) && (this->generalCollider.base.acFlags & AC_HIT)) {
        this->generalCollider.base.acFlags &= ~AC_HIT;
        this->damagedTimer = 15;
        acHitElem = this->generalCollider.elem.acHitElem;

        //! @bug The hookshot has a damage effect of 0 (`REMAINS_DMGEFF_IMMUNE`) and a damage value of 0; due to
        //! `CollisionCheck_SetATvsAC`, the hookshot passes through the remains. In other words, it's impossible for the
        //! following check to be true in the final game because the hookshot can't even register a hit.
        if (acHitElem->atDmgInfo.dmgFlags & DMG_HOOKSHOT) {
            Boss07_Remains_SetupStunned(this, play);
        } else {
            u8 damage = this->actor.colChkInfo.damage;

            this->actor.colChkInfo.health -= damage;
            this->damagedFlashTimer = 15;
            this->actionFunc = Boss07_Remains_Move;

            if ((s8)this->actor.colChkInfo.health <= 0) {
                this->subAction = REMAINS_MOVE_SUB_ACTION_DIE;
                this->burnOnLanding = true;
                Enemy_StartFinishingBlow(play, &this->actor);
                Actor_PlaySfx(&this->actor, NA_SE_EN_FOLLOWERS_DEAD);
            } else {
                this->subAction = REMAINS_MOVE_SUB_ACTION_DAMAGED;
                this->timers[0] = 15;
                Boss07_Remains_PlayDamageSfx(this);
            }

            Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
            Matrix_MultVecZ(-20.0f, &knockbackMovement);
            this->knockbackMovementX = knockbackMovement.x;
            this->knockbackMovementZ = knockbackMovement.z;
        }
    }
}

void Boss07_Remains_SetupIntroCutscene(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Remains_IntroCutscene;
}

void Boss07_Remains_IntroCutscene(Boss07* this, PlayState* play) {
    f32 dx;
    f32 dy;
    f32 dz;
    f32 yOffset;
    s32 pad;
    f32 speedToTargetTarget;
    f32 speedToTargetMaxStep;
    f32 speedTarget;
    f32 speedMaxStep;
    s16 targetRotX;
    s16 targetRotY;
    Player* player = GET_PLAYER(play);

    switch (this->subAction) {
        case REMAINS_INTRO_CS_SUB_ACTION_WAIT:
            Actor_SetScale(&this->actor, 0.0f);
            this->actor.world.pos.x = player->actor.world.pos.x;
            this->actor.world.pos.y = player->actor.world.pos.y + 30.0f;
            this->actor.world.pos.z = player->actor.world.pos.z;

            if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
                this->actor.world.pos.y += 30.0f + KREG(48);
            }

            this->targetPos.x = sRemainsStartTargetOffset[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].x +
                                player->actor.world.pos.x;
            this->targetPos.y = sRemainsStartTargetOffset[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].y +
                                player->actor.world.pos.y;
            this->targetPos.z = sRemainsStartTargetOffset[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].z +
                                player->actor.world.pos.z;

            dx = this->targetPos.x - this->actor.world.pos.x;
            dy = this->targetPos.y - this->actor.world.pos.y;
            dz = this->targetPos.z - this->actor.world.pos.z;
            this->actor.world.rot.y = Math_Atan2S(dx, dz);
            this->actor.world.rot.x = Math_Atan2S(dy, sqrtf(SQ(dx) + SQ(dz)));
            this->miscTimer = Rand_ZeroFloat(100.0f);
            break;

        case REMAINS_INTRO_CS_SUB_ACTION_FLY:
            this->cutsceneTimer++;
            this->miscTimer++;
            this->introRemainsOrbRot += 0x200;
            Math_ApproachF(&this->eyeBeamsLengthScale, 1.2f, 1.0f, 0.1f);
            Math_ApproachF(&this->actor.scale.x, 0.004f, 0.5f, 0.0002f);
            this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;

            if (this->cutsceneTimer > 90) {
                this->targetPos.x = sRemainsEndTarget[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].x;
                this->targetPos.y = 370.0f;
                this->targetPos.z = sRemainsEndTarget[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].z;
                speedTarget = 20.0f;
                speedMaxStep = 0.5f;
                speedToTargetTarget = 0x1000;
                speedToTargetMaxStep = 0x190;
                yOffset = 0.0f;
            } else {
                yOffset = Math_SinS(this->miscTimer * 0x300) * 20.0f;
                speedTarget = 1.0f;
                speedMaxStep = 0.1f;
                speedToTargetTarget = 0x5DC;
                speedToTargetMaxStep = 0x64;
            }

            dx = this->targetPos.x - this->actor.world.pos.x;
            dy = this->targetPos.y - this->actor.world.pos.y + yOffset;
            dz = this->targetPos.z - this->actor.world.pos.z;
            targetRotY = Math_Atan2S(dx, dz);
            targetRotX = Math_Atan2S(dy, sqrtf(SQ(dx) + SQ(dz)));
            Math_ApproachS(&this->actor.world.rot.y, targetRotY, 5, this->speedToTarget);
            Math_ApproachS(&this->actor.world.rot.x, targetRotX, 5, this->speedToTarget);
            Math_ApproachF(&this->speedToTarget, speedToTargetTarget, 1.0f, speedToTargetMaxStep);
            Math_ApproachF(&this->actor.speed, speedTarget, 1.0f, speedMaxStep);
            Actor_UpdateVelocityWithoutGravity(&this->actor);
            Actor_UpdatePos(&this->actor);
            this->actor.shape.rot = this->actor.world.rot;
            break;

        case REMAINS_INTRO_CS_SUB_ACTION_ATTACH_WAIT:
            Actor_SetScale(&this->actor, 0.0f);
            this->eyeBeamsLengthScale = 0.0f;
            this->actor.speed = 0.0f;
            this->actor.world.pos.x = sRemainsEndTarget[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].x * 0.6f;
            this->actor.world.pos.y = 370.0f;
            this->actor.world.pos.z = sRemainsEndTarget[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].z * 0.6f;
            this->actor.shape.rot.y = sRemainsEndTarget[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].y;
            this->sfxTimer = 0;
            break;

        case REMAINS_INTRO_CS_SUB_ACTION_ATTACH_TO_WALL:
            this->sfxTimer++;
            if (this->sfxTimer == 10) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_FOLLOWERS_STAY);
            }

            Actor_SetScale(&this->actor, 0.03f);
            Math_ApproachF(&this->actor.world.pos.x,
                           sRemainsEndTarget[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].x, 0.5f, 40.0f);
            Math_ApproachF(&this->actor.world.pos.z,
                           sRemainsEndTarget[MAJORA_GET_TYPE(&this->actor) - MAJORA_TYPE_REMAINS].z, 0.5f, 22.0f);
            break;

        default:
            break;
    }
}

void Boss07_Remains_SetupMove(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Remains_Move;
    this->actor.gravity = -0.75f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
    Collider_InitAndSetCylinder(play, &this->generalCollider, &this->actor, &sRemainsCylinderInit);
    this->actor.colChkInfo.health = 5;
    this->subAction = REMAINS_MOVE_SUB_ACTION_WAIT;
}

void Boss07_Remains_Move(Boss07* this, PlayState* play) {
    s16 targetRotX;
    s16 targetRotY;
    f32 dx;
    f32 dy;
    f32 dz;
    s32 pad;

    switch (this->subAction) {
        case REMAINS_MOVE_SUB_ACTION_DETACH_FROM_WALL:
            Actor_PlaySfx(&this->actor, NA_SE_EV_MUJURA_FOLLOWERS_FLY - SFX_FLAG);
            this->timers[0] = 80;
            this->timers[2] = 100.0f + Rand_ZeroFloat(100.0f);
            this->flySpeedTarget = 5.0f;
            this->actor.speed = 5.0f;
            this->targetPos = gZeroVec3f;
            this->actor.world.rot.y = Math_Atan2S(-this->actor.world.pos.x, -this->actor.world.pos.z);
            this->subAction = REMAINS_MOVE_SUB_ACTION_FLY;
            this->bgCheckTimer = 100;
            this->generalCollider.base.colMaterial = COL_MATERIAL_HIT3;
            this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR);
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_DEMO_BREAK);
            break;

        case REMAINS_MOVE_SUB_ACTION_FLY:
            Actor_PlaySfx(&this->actor, NA_SE_EV_MUJURA_FOLLOWERS_FLY - SFX_FLAG);
            if (this->timers[2] == 0) {
                this->tryFireProjectile = true;
                this->timers[2] = Rand_ZeroFloat(200.0f) + 100.0f;
            }

            if (this->timers[0] == 0) {
                if (Rand_ZeroOne() < 0.35f) {
                    this->flySpeedTarget = 1.0f;
                    this->timers[0] = Rand_ZeroFloat(50.0f) + 30.0f;
                } else {
                    Boss07_RandXZ(&this->targetPos, 500.0f);
                    this->targetPos.y = Rand_ZeroFloat(350.0f) + 100.0f;
                    this->timers[0] = Rand_ZeroFloat(50.0f) + 20.0f;
                    this->speedToTarget = 0.0f;
                    this->flySpeedTarget = Rand_ZeroFloat(5.0f) + 5.0f;
                }
            }

            dx = this->targetPos.x - this->actor.world.pos.x;
            dy = this->targetPos.y - this->actor.world.pos.y;
            dz = this->targetPos.z - this->actor.world.pos.z;
            targetRotY = Math_Atan2S(dx, dz);
            targetRotX = Math_Atan2S(dy, sqrtf(SQ(dx) + SQ(dz)));
            targetRotX += (s16)(Math_SinS(this->frameCounter * 0x1388) * 0xFA0);

            Math_ApproachS(&this->actor.world.rot.y, targetRotY, 0xA, this->speedToTarget);
            Math_ApproachS(&this->actor.world.rot.x, targetRotX, 5, this->speedToTarget);
            Math_ApproachF(&this->speedToTarget, 0x7D0, 1.0f, 0x64);
            Math_ApproachF(&this->actor.speed, this->flySpeedTarget, 1.0f, 1.0f);

            if ((this->flySpeedTarget < 8.0f) && !Play_InCsMode(play)) {
                Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000);
            } else {
                Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 0xA, 0x1000);
            }

            Actor_UpdateVelocityWithoutGravity(&this->actor);
            Actor_UpdatePos(&this->actor);

            if (this->bgCheckTimer == 0) {
                Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 100.0f, 100,
                                        UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
            } else {
                this->bgCheckTimer--;
            }

            Boss07_Remains_UpdateDamage(this, play);
            break;

        case REMAINS_MOVE_SUB_ACTION_DIE:
            Math_ApproachS(&this->actor.shape.rot.x, -0x4000, 1, 0x500);
            Actor_MoveWithGravity(&this->actor);
            Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 100.0f, 100.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                if (this->burnOnLanding) {
                    this->fireTimer |= 4;
                }

                Math_ApproachF(&this->actor.scale.z, 0.0f, 1.0f, 0.001f);

                if (this->actor.scale.z == 0.0f) {
                    this->subAction = REMAINS_MOVE_SUB_ACTION_DEAD;
                    this->actor.draw = NULL;
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                }

                Boss07_SmoothStop(this, 2.0f);
            } else {
                this->actor.shape.rot.z += 0x200;
            }
            break;

        case REMAINS_MOVE_SUB_ACTION_DAMAGED:
            Actor_MoveWithGravity(&this->actor);
            this->actor.world.pos.y -= 50.0f;
            this->actor.prevPos.y -= 50.0f;

            Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 60.0f, 60.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

            this->actor.world.pos.y += 50.0f;
            this->actor.prevPos.y += 50.0f;

            if (this->timers[0] == 0) {
                this->subAction = REMAINS_MOVE_SUB_ACTION_FLY;
            }
            break;

        case REMAINS_MOVE_SUB_ACTION_WAIT:
        default:
            break;
    }

    if (this->subAction < REMAINS_MOVE_SUB_ACTION_DIE) {
        Collider_UpdateCylinder(&this->actor, &this->generalCollider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->generalCollider.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->generalCollider.base);
    }

    if (this->tryFireProjectile) {
        this->tryFireProjectile = false;

        if (Boss07_ArePlayerAndActorFacing(this, play) && (sMajorasMask->actionFunc != Boss07_Mask_FireBeam)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_07, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, MAJORA_PARAMS(MAJORA_TYPE_PROJECTILE_REMAINS));
        }
    }

    if (this->fireTimer != 0) {
        Vec3f flamePos;
        Vec3f flameVelocity;
        Vec3f flameAccel;

        flamePos.x = Rand_CenteredFloat(80.0f) + this->actor.world.pos.x;
        flamePos.z = Rand_CenteredFloat(80.0f) + this->actor.world.pos.z;

        if (this->burnOnLanding) {
            flameAccel.x = flameAccel.z = 0.0f;
            flameAccel.y = 0.03f;
            flamePos.y = Rand_ZeroFloat(10.0f) + this->actor.world.pos.y;
            EffectSsKFire_Spawn(play, &flamePos, &gZeroVec3f, &flameAccel, Rand_ZeroFloat(30.0f) + 30.0f, 0);
            Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
        } else {
            flamePos.y = (Rand_ZeroFloat(30.0f) + this->actor.world.pos.y) - 15.0f;

            flameVelocity.x = 0.0f;
            flameVelocity.y = 5.0f;
            flameVelocity.z = 0.0f;

            flameAccel.x = flameVelocity.x * -0.05f;
            flameAccel.y = flameVelocity.y * -0.05f;
            flameAccel.z = flameVelocity.z * -0.05f;

            Boss07_SpawnFlameEffect(play, &flamePos, &flameVelocity, &flameAccel, Rand_ZeroFloat(10.0f) + 25.0f);
            Actor_PlaySfx(&this->actor, NA_SE_EV_BURN_OUT - SFX_FLAG);
        }
    }
}

void Boss07_Remains_SetupStunned(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Remains_Stunned;
    this->timers[0] = 50;
    this->actor.speed = 0.0f;
}

void Boss07_Remains_Stunned(Boss07* this, PlayState* play) {
    Boss07_Remains_UpdateDamage(this, play);
    Collider_UpdateCylinder(&this->actor, &this->generalCollider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->generalCollider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->generalCollider.base);

    if (this->timers[0] == 0) {
        this->actionFunc = Boss07_Remains_Move;
        this->subAction = REMAINS_MOVE_SUB_ACTION_FLY;
    }
}

void Boss07_Remains_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;
    s32 i;

    this->frameCounter++;

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        DECR(this->timers[i]);
    }

    DECR(this->fireTimer);
    DECR(this->damagedTimer);
    DECR(this->damagedFlashTimer);

    this->actionFunc(this, play);

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.world.pos.x += this->knockbackMovementX;
    this->actor.world.pos.z += this->knockbackMovementZ;

    Math_ApproachZeroF(&this->knockbackMovementX, 1.0f, 1.0f);
    Math_ApproachZeroF(&this->knockbackMovementZ, 1.0f, 1.0f);
}

void Boss07_Remains_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;
    f32 shakeScale;
    f32 rotX;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if ((this->damagedFlashTimer % 2) != 0) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
    }

    shakeScale = this->damagedTimer * (M_PIf / 4.0f) * 0.06666667f;
    rotX = Math_SinS(this->damagedTimer * 0x3500) * shakeScale * 0.5f;
    Matrix_RotateYF(Math_SinS(this->damagedTimer * 0x4500) * shakeScale, MTXMODE_APPLY);
    Matrix_RotateXFApply(rotX);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    switch (MAJORA_GET_TYPE(&this->actor)) {
        case MAJORA_TYPE_REMAINS + MAJORA_REMAINS_TYPE_ODOLWA:
            gSPDisplayList(POLY_OPA_DISP++, gBossMaskOdolwaDL);
            break;

        case MAJORA_TYPE_REMAINS + MAJORA_REMAINS_TYPE_GYORG:
            gSPDisplayList(POLY_OPA_DISP++, gBossMaskGyorgDL);
            break;

        case MAJORA_TYPE_REMAINS + MAJORA_REMAINS_TYPE_GOHT:
            gSPDisplayList(POLY_OPA_DISP++, gBossMaskGohtDL);
            break;

        case MAJORA_TYPE_REMAINS + MAJORA_REMAINS_TYPE_TWINMOLD:
            gSPDisplayList(POLY_OPA_DISP++, gBossMaskTwinmoldDL);
            break;

        default:
            break;
    }

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    if (this->actionFunc == Boss07_Remains_IntroCutscene) {
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, 200);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 100, 128);

        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_Scale(this->eyeBeamsLengthScale, this->eyeBeamsLengthScale, 0.0f, MTXMODE_APPLY);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_RotateZS(this->introRemainsOrbRot, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);
        gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Boss07_Top_SetupThrown(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Top_Thrown;
    this->actor.gravity = 0.0f;
    this->topSpinAngularVelocity = (sREG(42) * 0.01f) + 0.9f;
    this->timers[0] = 200;
}

void Boss07_Top_Thrown(Boss07* this, PlayState* play) {
    if (sMajorasWrath->actionFunc == Boss07_Wrath_ThrowTop) {
        Math_Vec3f_Copy(&this->actor.world.pos, &sMajorasWrath->whipGrabPos);
        this->actor.world.pos.y -= 25.0f + sREG(78);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 50.0f, 80.0f, UPDBGCHECKINFO_FLAG_1);

    if ((sMajorasWrath->frameCounter >= (s16)(KREG(50) + 21)) || (sMajorasWrath->damagedTimer != 0)) {
        Boss07_Top_SetupMove(this, play);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        Boss07_Top_SetupMove(this, play);
        this->actor.speed = -15.0f;
        CollisionCheck_SpawnShieldParticles(play, &this->actor.focus.pos);
        Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
    }
}

void Boss07_Top_SetupMove(Boss07* this, PlayState* play) {
    this->actionFunc = Boss07_Top_Move;
    this->actor.gravity = -2.0f;

    if (sMajorasWrath->damagedTimer != 0) {
        this->timers[0] = sREG(47) + 80;
        this->timers[1] = sREG(77) + 150;
    } else {
        this->timers[0] = sREG(47) + 300;
        this->timers[1] = sREG(77) + 370;
        this->actor.velocity.y = (sREG(43) * 0.1f) + 15.0f;
        this->actor.speed = (sREG(44) * 0.1f) + -3.0f;
    }
    this->actor.world.rot.y = (sREG(45) * 0x1000) + sMajorasWrath->actor.shape.rot.y;
    this->actor.shape.rot.z = Rand_ZeroFloat((f32)(sREG(29) + 10) * 0x100);
}

void Boss07_Top_Move(Boss07* this, PlayState* play) {
    f32 dx;
    f32 dz;

    Audio_PlaySfx_AtPosWithFreq(&this->actor.projectedPos, NA_SE_EN_LAST3_KOMA_OLD - SFX_FLAG,
                                this->topSpinAngularVelocity * 1.1111112f);
    Actor_MoveWithGravity(&this->actor);
    this->actor.world.pos.x += this->knockbackMovementX;
    this->actor.world.pos.z += this->knockbackMovementZ;
    Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 40.0f, 80.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

    if ((this->disableCollisionTimer == 0) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        this->disableCollisionTimer = 10;
        if (this->topSpinAngularVelocity > 0.01f) {
            if (this->topSpinAngularVelocity < 0.45f) {
                this->actor.speed *= -1.0f;
            } else {
                this->actor.speed = -10.0f;
            }
            CollisionCheck_SpawnShieldParticles(play, &this->actor.focus.pos);
            Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
        }
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->timers[0] < (s16)(sREG(24) + 70)) {
            if (this->timers[0] >= (s16)(sREG(25) + 35)) {
                Math_ApproachZeroF(&this->actor.speed, 1.0f, (sREG(17) * 0.01f) + 0.1f);
                Math_ApproachS(&this->actor.shape.rot.z, (sREG(19) * 0x800) + 0x2000, 1, sREG(18) + 0x1A0);
                Math_ApproachZeroF(&this->topSpinAngularVelocity, 1.0f, (sREG(16) * 0.0001f) + 0.005f);
                Math_ApproachS(&this->topPrecessionVelocity, (sREG(20) * 0x800) + 0x2000, 1, (sREG(21) * 16) + 0xC0);
            } else if (this->timers[0] == 0) {
                Math_ApproachZeroF(&this->actor.speed, 1.0f, (sREG(26) * 0.01f) + 0.5f);
                Math_ApproachS(&this->actor.shape.rot.z, (sREG(19) * 0x800) + 0x2000, 1, 0x800);
                Math_ApproachZeroF(&this->topSpinAngularVelocity, 1.0f, (sREG(27) * 0.001f) + 0.024f);
                Math_ApproachS(&this->topPrecessionVelocity, 0, 2, sREG(28) + 106);
            } else {
                Math_ApproachF(&this->actor.speed, sREG(22) + 10.0f, 1.0f, (sREG(23) * 0.01f) + 0.6f);
                Math_ApproachS(&this->actor.shape.rot.z, (sREG(19) * 0x800) + 0x2000, 1, sREG(18) + 0x410);
                Math_ApproachZeroF(&this->topSpinAngularVelocity, 1.0f, (sREG(16) * 0.0001f) + 0.005f);
                Math_ApproachS(&this->topPrecessionVelocity, (sREG(29) * 0x800) - 0x800, 1, (sREG(30) * 16) + 0x340);
            }

            this->actor.world.rot.y += this->topPrecessionVelocity;
        } else {
            Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x300);
            Math_ApproachF(&this->actor.speed, (sREG(48) * 0.1f) + 2.0f + 8.0f, 1.0f,
                           (sREG(46) * 0.01f) + 0.100000024f + 0.2f);
        }

        Math_ApproachZeroF(&this->knockbackMovementX, 1.0f, 1.0f);
        Math_ApproachZeroF(&this->knockbackMovementZ, 1.0f, 1.0f);

        if (this->actor.velocity.y < (sREG(40) + -2.0f)) {
            this->actor.velocity.y *= -(0.5f + (sREG(41) * 0.01f));
            Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
            CollisionCheck_SpawnShieldParticles(play, &this->actor.world.pos);
            this->actor.speed *= 0.5f + (sREG(57) * 0.01f);
        } else {
            this->actor.velocity.y = -0.5f;
        }

        dx = this->actor.prevPos.x - this->actor.world.pos.x;
        dz = this->actor.prevPos.z - this->actor.world.pos.z;
        func_800AE930(&play->colCtx, Effect_GetByIndex(this->effectIndex), &this->actor.world.pos, 3.0f,
                      Math_Atan2S(dx, dz), this->actor.floorPoly, this->actor.floorBgId);
        this->subAction = TOP_MOVE_SUB_ACTION_GROUNDED;
    } else if (this->subAction != TOP_MOVE_SUB_ACTION_AIRBORNE) {
        this->subAction = TOP_MOVE_SUB_ACTION_AIRBORNE;
        func_800AEF44(Effect_GetByIndex(this->effectIndex));
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->timers[1] == 0) {
        EnBom* bomb =
            (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                this->actor.world.pos.y + 25.0f, this->actor.world.pos.z, 0, 0, 0, BOMB_TYPE_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        Actor_Kill(&this->actor);
    } else if (this->timers[1] == 25) {
        this->damagedFlashTimer = 25;
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_EXPLOSIVES);
    }
}

void Boss07_Top_CheckTopCollision(Boss07* this, PlayState* play) {
    s32 i;
    Boss07* top;
    f32 dx;
    f32 dy;
    f32 dz;

    if (this->disableCollisionTimer == 0) {
        for (top = (Boss07*)play->actorCtx.actorLists[ACTORCAT_BOSS].first; top != NULL;
             top = (Boss07*)top->actor.next) {
            if ((this != top) && (MAJORA_GET_TYPE(&top->actor) == MAJORA_TYPE_TOP) &&
                (top->disableCollisionTimer == 0)) {
                dx = top->actor.world.pos.x - this->actor.world.pos.x;
                dy = top->actor.world.pos.y - this->actor.world.pos.y;
                dz = top->actor.world.pos.z - this->actor.world.pos.z;

                if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < (sREG(28) + 50.0f)) {
                    top->disableCollisionTimer = this->disableCollisionTimer = 10;
                    this->actor.world.rot.y = Math_Atan2S(dx, dz);
                    top->actor.world.rot.y = this->actor.world.rot.y + 0x7FFF;

                    if (this->timers[0] > 0) {
                        this->actor.speed = -10.0f;
                    } else {
                        this->actor.speed = -5.0f;
                        this->topSpinAngularVelocity = (sREG(89) * 0.01f) + 0.2f;
                        this->topPrecessionVelocity = 0x800;
                    }

                    if (top->timers[0] > 0) {
                        top->actor.speed = -10.0f;
                    } else {
                        top->actor.speed = -5.0f;
                        top->topSpinAngularVelocity = (sREG(89) * 0.01f) + 0.2f;
                        top->topPrecessionVelocity = 0x800;
                    }

                    CollisionCheck_SpawnShieldParticles(play, &this->actor.focus.pos);
                    CollisionCheck_SpawnShieldParticles(play, &top->actor.focus.pos);

                    if (this->timers[0] > 80) {
                        this->actor.shape.rot.z = Rand_CenteredFloat((f32)(sREG(29) + 20) * 0x100);
                        for (i = 0; i < 5; i++) {
                            DECR(this->timers[0]);
                            DECR(this->timers[1]);
                        }
                    }
                    if (top->timers[0] > 80) {
                        top->actor.shape.rot.z = Rand_CenteredFloat((f32)(sREG(29) + 20) * 0x100);
                        for (i = 0; i < 5; i++) {
                            DECR(top->timers[0]);
                            DECR(top->timers[1]);
                        }
                    }
                    Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
                    break;
                }
            }
        }
    }
}

void Boss07_Top_UpdateDamage(Boss07* this, PlayState* play) {
    PlayerImpactType playerImpactType;
    s32 hit = false;
    Player* player = GET_PLAYER(play);
    s32 pad[3];

    if (this->generalCollider.base.acFlags & AC_HIT) {
        this->generalCollider.base.acFlags &= ~AC_HIT;

        if (this->damagedTimer == 0) {
            this->damagedTimer = 5;

            if ((this->actor.colChkInfo.damageEffect == TOP_DMGEFF_PUSH_BACK_PLAYER) ||
                (this->actor.colChkInfo.damageEffect == TOP_DMGEFF_REVERSE_DIRECTION)) {
                if (this->actor.colChkInfo.damageEffect == TOP_DMGEFF_PUSH_BACK_PLAYER) {
                    player->pushedYaw = this->actor.yawTowardsPlayer;
                    player->pushedSpeed = 20.0f;
                }

                if (this->timers[0] > 40) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;

                    if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
                        this->actor.speed = -30.0f;
                        this->actor.velocity.y = 10.0f;

                        if ((s16)(sREG(47) + 100) < this->timers[0]) {
                            this->timers[0] = sREG(47) + 100;
                            this->timers[1] = sREG(77) + 170;
                        }
                    } else if (this->actor.colChkInfo.damageEffect == TOP_DMGEFF_REVERSE_DIRECTION) {
                        this->actor.speed = -15.0f;
                    } else {
                        this->actor.speed = -7.0f;
                    }
                }
            } else if (this->actor.colChkInfo.damageEffect == TOP_DMGEFF_KNOCKED_BACK_FROM_PLAYER) {
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                this->actor.speed = 2.0f * -player->actor.speed;
                hit = true;
            } else if (this->actor.colChkInfo.damageEffect == TOP_DMGEFF_BOUNCE_BACK_FROM_DAMAGE) {
                this->actor.world.rot.y =
                    Math_Atan2S(this->generalCollider.base.ac->world.pos.x - this->actor.world.pos.x,
                                this->generalCollider.base.ac->world.pos.z - this->actor.world.pos.z);
                this->actor.speed = -20.0f;
                this->actor.velocity.y = sREG(55) + 15.0f;
                hit = true;
            }
        }
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if ((Actor_GetPlayerImpact(play, 5.0f, &this->actor.world.pos, &playerImpactType) >= 0.0f) &&
            (playerImpactType == PLAYER_IMPACT_GORON_GROUND_POUND)) {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            this->actor.velocity.y = sREG(55) + 25.0f;
            hit = true;
        }
    }

    if (hit && (this->timers[0] > 90)) {
        this->actor.shape.rot.z = Rand_CenteredFloat((f32)(sREG(29) + 30) * 0x100);
        this->timers[0] = Rand_ZeroFloat(10.0f) + 70.0f;
        this->timers[1] = this->timers[0] + 70;
    }
}

void Boss07_Top_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;

    this->frameCounter++;

    DECR(this->timers[0]);
    DECR(this->timers[1]);
    DECR(this->damagedFlashTimer);
    DECR(this->damagedTimer);
    DECR(this->disableCollisionTimer);

    this->actionFunc(this, play);

    Math_Vec3f_Copy(&this->actor.focus.pos, &this->actor.world.pos);
    this->actor.focus.pos.y += 25.0f;
    Boss07_Top_UpdateDamage(this, play);
    Collider_UpdateCylinder(&this->actor, &this->generalCollider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->generalCollider.base);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->generalCollider.base);
    Boss07_Top_CheckTopCollision(this, play);

    this->topSpinAngle -= this->topSpinAngularVelocity;
    if (this->topSpinAngle < -2.0f * M_PIf) {
        this->topSpinAngle += 2.0f * M_PIf;
    }

    if (sMajorasWrath->actionFunc == Boss07_Wrath_DeathCutscene) {
        Actor_Kill(&this->actor);
    }

    Math_ApproachF(&this->actor.scale.x, (sREG(77) * 0.001f) + 0.06f, 1.0f, 0.012f);
    Actor_SetScale(&this->actor, this->actor.scale.x);
}

void Boss07_Top_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if ((this->damagedFlashTimer % 2) != 0) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 780, 1099);
    }

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_RotateYF(this->topSpinAngle, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gMajorasWrathSpinningTopDL);
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Boss07_BattleHandler_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;

    Boss07_BattleHandler_UpdateEffects(play);

    if (sHeartbeatTimer != 0) {
        sHeartbeatTimer--;
        Audio_PlaySfx(NA_SE_EN_LAST2_HEARTBEAT_OLD - SFX_FLAG);
    }
    if (this->lensFlareOn) {
        gCustomLensFlare1On = true;
        gCustomLensFlare1Pos = this->lensFlarePos;
        D_801F4E44 = this->lensFlareScale;
        D_801F4E48 = 10.0f;
        D_801F4E4C = 0;
        this->lensFlareOn = false;
    } else {
        gCustomLensFlare1On = false;
    }

    this->cutsceneTimer++;

    switch (this->cutsceneState) {
        case BATTLE_HANDLER_REMAINS_CS_STATE_WAITING_FOR_MAJORAS_MASK:
            if ((sMajorasMask != NULL) && sMajorasMask->startRemainsCs) {
                this->cutsceneState = BATTLE_HANDLER_REMAINS_CS_STATE_STARTED;
                this->cutsceneTimer = 0;
            }
            break;

        case BATTLE_HANDLER_REMAINS_CS_STATE_STARTED:
            if (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            this->cutsceneTimer = 0;
            this->cutsceneState = BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_ODOLWA;
            Play_EnableMotionBlur(150);
            this->subCamEye.x = sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA]->actor.world.pos.x * 0.7f;
            this->subCamEye.y = sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA]->actor.world.pos.y * 0.7f;
            this->subCamEye.z = sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA]->actor.world.pos.z * 0.7f;
            FALLTHROUGH;
        case BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_ODOLWA:
            if (this->cutsceneTimer == 20) {
                sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA]->subAction = REMAINS_MOVE_SUB_ACTION_DETACH_FROM_WALL;
            }

            this->subCamAt.x = sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA]->actor.world.pos.x;
            this->subCamAt.y = sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA]->actor.world.pos.y;
            this->subCamAt.z = sMajoraRemains[MAJORA_REMAINS_TYPE_ODOLWA]->actor.world.pos.z;

            if (this->cutsceneTimer != 40) {
                break;
            }

            this->cutsceneState = BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_GYORG;
            this->cutsceneTimer = 0;
            this->subCamEye.x = sMajoraRemains[MAJORA_REMAINS_TYPE_GYORG]->actor.world.pos.x * 0.7f;
            this->subCamEye.y = sMajoraRemains[MAJORA_REMAINS_TYPE_GYORG]->actor.world.pos.y * 0.7f;
            this->subCamEye.z = sMajoraRemains[MAJORA_REMAINS_TYPE_GYORG]->actor.world.pos.z * 0.7f;
            FALLTHROUGH;
        case BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_GYORG:
            if (this->cutsceneTimer == 20) {
                sMajoraRemains[MAJORA_REMAINS_TYPE_GYORG]->subAction = REMAINS_MOVE_SUB_ACTION_DETACH_FROM_WALL;
            }

            this->subCamAt.x = sMajoraRemains[MAJORA_REMAINS_TYPE_GYORG]->actor.world.pos.x;
            this->subCamAt.y = sMajoraRemains[MAJORA_REMAINS_TYPE_GYORG]->actor.world.pos.y;
            this->subCamAt.z = sMajoraRemains[MAJORA_REMAINS_TYPE_GYORG]->actor.world.pos.z;

            if (this->cutsceneTimer != 40) {
                break;
            }

            this->cutsceneState = BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_GOHT;
            this->cutsceneTimer = 0;
            this->subCamEye.x = sMajoraRemains[MAJORA_REMAINS_TYPE_GOHT]->actor.world.pos.x * 0.7f;
            this->subCamEye.y = sMajoraRemains[MAJORA_REMAINS_TYPE_GOHT]->actor.world.pos.y * 0.7f;
            this->subCamEye.z = sMajoraRemains[MAJORA_REMAINS_TYPE_GOHT]->actor.world.pos.z * 0.7f;
            FALLTHROUGH;
        case BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_GOHT:
            if (this->cutsceneTimer == 20) {
                sMajoraRemains[MAJORA_REMAINS_TYPE_GOHT]->subAction = REMAINS_MOVE_SUB_ACTION_DETACH_FROM_WALL;
            }

            this->subCamAt.x = sMajoraRemains[MAJORA_REMAINS_TYPE_GOHT]->actor.world.pos.x;
            this->subCamAt.y = sMajoraRemains[MAJORA_REMAINS_TYPE_GOHT]->actor.world.pos.y;
            this->subCamAt.z = sMajoraRemains[MAJORA_REMAINS_TYPE_GOHT]->actor.world.pos.z;

            if (this->cutsceneTimer != 40) {
                break;
            }

            this->cutsceneState = BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_TWINMOLD;
            this->cutsceneTimer = 0;
            this->subCamEye.x = sMajoraRemains[MAJORA_REMAINS_TYPE_TWINMOLD]->actor.world.pos.x * 0.7f;
            this->subCamEye.y = sMajoraRemains[MAJORA_REMAINS_TYPE_TWINMOLD]->actor.world.pos.y * 0.7f;
            this->subCamEye.z = sMajoraRemains[MAJORA_REMAINS_TYPE_TWINMOLD]->actor.world.pos.z * 0.7f;
            FALLTHROUGH;
        case BATTLE_HANDLER_REMAINS_CS_STATE_ACTIVATE_TWINMOLD:
            if (this->cutsceneTimer == 20) {
                sMajoraRemains[MAJORA_REMAINS_TYPE_TWINMOLD]->subAction = REMAINS_MOVE_SUB_ACTION_DETACH_FROM_WALL;
            }

            this->subCamAt.x = sMajoraRemains[MAJORA_REMAINS_TYPE_TWINMOLD]->actor.world.pos.x;
            this->subCamAt.y = sMajoraRemains[MAJORA_REMAINS_TYPE_TWINMOLD]->actor.world.pos.y;
            this->subCamAt.z = sMajoraRemains[MAJORA_REMAINS_TYPE_TWINMOLD]->actor.world.pos.z;

            if (this->cutsceneTimer == 40) {
                Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);
                s32 i;

                this->cutsceneState = BATTLE_HANDLER_REMAINS_CS_STATE_DONE;
                this->cutsceneTimer = 0;
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                Play_DisableMotionBlur();

                for (i = 0; i < MAJORA_REMAINS_TYPE_MAX; i++) {
                    Actor_ChangeCategory(play, &play->actorCtx, &sMajoraRemains[i]->actor, ACTORCAT_ENEMY);
                }
            }
            break;

        case BATTLE_HANDLER_REMAINS_CS_STATE_DONE:
        default:
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        if (this->cutsceneTimer < 20) {
            s32 i;

            for (i = 0; i < MAJORA_REMAINS_TYPE_MAX; i++) {
                if ((this->cutsceneTimer % 2) != 0) {
                    sMajoraRemains[i]->actor.world.pos.x += 2.0f;
                    sMajoraRemains[i]->actor.world.pos.z += 2.0f;
                } else {
                    sMajoraRemains[i]->actor.world.pos.x -= 2.0f;
                    sMajoraRemains[i]->actor.world.pos.z -= 2.0f;
                }
            }
        }

        ShrinkWindow_Letterbox_SetSizeTarget(27);
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void Boss07_BattleHandler_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss07* this = (Boss07*)thisx;

    Boss07_BattleHandler_DrawEffects(play);
    Boss07_BattleHandler_DrawIntroPlayerLightOrb(this, play);
}

void Boss07_BattleHandler_UpdateEffects(PlayState* play) {
    MajoraEffect* effect = (MajoraEffect*)play->specialEffects;
    s32 i;

    for (i = 0; i < MAJORA_EFFECT_COUNT; i++, effect++) {
        if (effect->type != MAJORA_EFFECT_NONE) {
            effect->texScroll++;

            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;

            if (effect->type == MAJORA_EFFECT_FLAME) {
                if (effect->isFadingAway) {
                    effect->alpha -= (i & 7) + 13;
                    if (effect->alpha <= 0) {
                        effect->alpha = 0;
                        effect->type = MAJORA_EFFECT_NONE;
                    }
                } else {
                    effect->alpha += 300;
                    if (effect->alpha >= 255) {
                        effect->alpha = 255;
                        effect->isFadingAway++;
                    }
                }
            }
        }
    }
}

void Boss07_BattleHandler_DrawEffects(PlayState* play) {
    MajoraEffect* effect = (MajoraEffect*)play->specialEffects;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 i;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < MAJORA_EFFECT_COUNT; i++, effect++) {
        if (effect->type > MAJORA_EFFECT_NONE) {
            FrameInterpolation_RecordOpenChild(effect, effect->type);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, effect->alpha);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, (3 * effect->texScroll) & 0x7F,
                                          (15 * -effect->texScroll) & 0xFF, 32, 64, 1, 0, 0, 32, 32, 3, -15, 0, 0));
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gMajorasMaskFireDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void Boss07_Reset(void) {
    sHeartbeatTimer = 0;
    sWhipLength = 0;
    sMajorasWrath = NULL;
    sMajoraBattleHandler = NULL;
    sMajorasMask = NULL;

    for (int i = 0; i < MAJORA_REMAINS_TYPE_MAX; i++) {
        sMajoraRemains[i] = NULL;
    }

    sKillAllProjectiles = 0;
    sMusicStartTimer = 0;

    for (int i = 0; i < MAJORA_EFFECT_COUNT; i++) {
        sMajoraEffects[i].type = MAJORA_EFFECT_NONE;
    }

    sMajoraRandSeed1 = 0;
    sMajoraRandSeed2 = 0;
    sMajoraRandSeed3 = 0;

    Gfx_UnregisterBlendedTexture(gMajorasWrathEarTex);
    Gfx_UnregisterBlendedTexture(gMajoraStripesTex);
    Gfx_UnregisterBlendedTexture(gMajorasWrathMouthTex);
    Gfx_UnregisterBlendedTexture(gMajoraBloodshotEyeTex);
    Gfx_UnregisterBlendedTexture(gMajorasWrathEyeTex);
    Gfx_UnregisterBlendedTexture(gMajorasMaskWithNormalEyesTex);
    Gfx_UnregisterBlendedTexture(gMajoraVeinsTex);
    Gfx_UnregisterBlendedTexture(gMajoraHandTex);
    Gfx_UnregisterBlendedTexture(gMajoraBodyTex);

    Gfx_TextureCacheDelete(sWrathTexMask16by16);
    Gfx_TextureCacheDelete(sWrathTexMask16by32);
    Gfx_TextureCacheDelete(sWrathTexMask32by32);
    Gfx_TextureCacheDelete(sWrathTexMask32by64);
}
