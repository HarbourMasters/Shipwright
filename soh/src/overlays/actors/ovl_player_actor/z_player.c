/*
 * File: z_player.c
 * Overlay: ovl_player_actor
 * Description: Link
 */

#include <libultraship/libultra.h>
#include "global.h"

#include "overlays/actors/ovl_Bg_Heavy_Block/z_bg_heavy_block.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "overlays/actors/ovl_En_Boom/z_en_boom.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/actors/ovl_En_Fish/z_en_fish.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
#include "overlays/misc/ovl_kaleido_scope/z_kaleido_scope.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_link_child/object_link_child.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include <soh/Enhancements/custom-message/CustomMessageTypes.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include <overlays/actors/ovl_En_Partner/z_en_partner.h>
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/randomizer_grotto.h"
#include "soh/frame_interpolation.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

// Some player animations are played at this reduced speed, for reasons yet unclear.
// This is called "adjusted" for now.
#define PLAYER_ANIM_ADJUSTED_SPEED (2.0f / 3.0f)

typedef enum {
    /* 0x00 */ KNOB_ANIM_ADULT_L,
    /* 0x01 */ KNOB_ANIM_CHILD_L,
    /* 0x02 */ KNOB_ANIM_ADULT_R,
    /* 0x03 */ KNOB_ANIM_CHILD_R
} KnobDoorAnim;

typedef struct ExplosiveInfo {
    /* 0x00 */ u8 itemId;
    /* 0x02 */ s16 actorId;
} ExplosiveInfo; // size = 0x04

typedef struct BottleDropInfo {
    /* 0x00 */ s16 actorId;
    /* 0x02 */ s16 actorParams;
} BottleDropInfo; // size = 0x04

typedef struct FallImpactInfo {
    /* 0x00 */ s8 damage;
    /* 0x01 */ u8 rumbleStrength;
    /* 0x02 */ u8 rumbleDuration;
    /* 0x03 */ u8 rumbleDecreaseRate;
    /* 0x04 */ u16 sfxId;
} FallImpactInfo; // size = 0x06

typedef struct SpecialRespawnInfo {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yaw;
} SpecialRespawnInfo; // size = 0x10

typedef enum AnimSfxType {
    /* 1 */ ANIMSFX_TYPE_GENERAL = 1,
    /* 2 */ ANIMSFX_TYPE_FLOOR,
    /* 3 */ ANIMSFX_TYPE_FLOOR_BY_AGE,
    /* 4 */ ANIMSFX_TYPE_VOICE,
    /* 5 */ ANIMSFX_TYPE_LANDING, // `AnimSfxEntry.sfxId` is ignored. Adjusted for Iron Boots if needed.
    /* 6 */ ANIMSFX_TYPE_RUNNING, // `AnimSfxEntry.sfxId` is ignored. Adjusted for Iron Boots if needed.
    /* 7 */ ANIMSFX_TYPE_JUMPING, // `AnimSfxEntry.sfxId` is ignored. Adjusted for Iron Boots if needed.
    /* 8 */ ANIMSFX_TYPE_WALKING, // `AnimSfxEntry.sfxId` is ignored. Adjusted for Iron Boots if needed.
    /* 9 */ ANIMSFX_TYPE_UNKNOWN  // `AnimSfxEntry.sfxId` is ignored. Only used in the intro cutscene.
} AnimSfxType;

#define ANIMSFX_SHIFT_TYPE(type) ((type) << 11)

#define ANIMSFX_DATA(type, frame) ((ANIMSFX_SHIFT_TYPE(type) | ((frame)&0x7FF)))

#define ANIMSFX_GET_TYPE(data) ((data)&0x7800)
#define ANIMSFX_GET_FRAME(data) ((data)&0x7FF)

typedef struct AnimSfxEntry {
    /* 0x00 */ u16 sfxId;
    /* 0x02 */ s16 data;
} AnimSfxEntry; // size = 0x04

typedef struct struct_808551A4 {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ s16 unk_02;
} struct_808551A4; // size = 0x04

typedef struct ItemChangeInfo {
    /* 0x00 */ LinkAnimationHeader* anim;
    /* 0x04 */ u8 changeFrame;
} ItemChangeInfo; // size = 0x08

typedef struct struct_80854190 {
    /* 0x00 */ LinkAnimationHeader* unk_00;
    /* 0x04 */ LinkAnimationHeader* unk_04;
    /* 0x08 */ LinkAnimationHeader* unk_08;
    /* 0x0C */ u8 unk_0C;
    /* 0x0D */ u8 unk_0D;
} struct_80854190; // size = 0x10

typedef struct struct_80854578 {
    /* 0x00 */ LinkAnimationHeader* anim;
    /* 0x04 */ f32 unk_04;
    /* 0x04 */ f32 unk_08;
} struct_80854578; // size = 0x0C

typedef struct struct_80854B18 {
    /* 0x00 */ s8 type;
    /* 0x04 */ union {
        void* ptr;
        void (*func)(PlayState*, Player*, CsCmdActorCue*);
    };
} struct_80854B18; // size = 0x08

void Player_InitItemAction(PlayState* play, Player* this, s8 itemAction);

void Player_InitDefaultIA(PlayState* play, Player* this);
void Player_InitHammerIA(PlayState* play, Player* this);
void Player_InitBowOrSlingshotIA(PlayState* play, Player* this);
void Player_InitDekuStickIA(PlayState* play, Player* this);
void Player_InitExplosiveIA(PlayState* play, Player* this);
void Player_InitHookshotIA(PlayState* play, Player* this);
void Player_InitBoomerangIA(PlayState* play, Player* this);

s32 Player_UpperAction_ChangeHeldItem(Player* this, PlayState* play);
s32 func_8083485C(Player* this, PlayState* play);
s32 Player_UpperAction_Sword(Player* this, PlayState* play);
s32 func_80834B5C(Player* this, PlayState* play);
s32 func_80834C74(Player* this, PlayState* play);
s32 func_8083501C(Player* this, PlayState* play);
s32 func_808351D4(Player* this, PlayState* play); // Arrow nocked
s32 func_808353D8(Player* this, PlayState* play); // Aiming in first person
s32 func_80835588(Player* this, PlayState* play);
s32 Player_UpperAction_CarryActor(Player* this, PlayState* play);
s32 func_80835800(Player* this, PlayState* play);
s32 func_80835884(Player* this, PlayState* play); // Start aiming boomerang
s32 func_808358F0(Player* this, PlayState* play); // Aim boomerang
s32 func_808359FC(Player* this, PlayState* play); // Throw boomerang
s32 func_80835B60(Player* this, PlayState* play); // Boomerang active
s32 func_80835C08(Player* this, PlayState* play);

void Player_UseItem(PlayState* play, Player* this, s32 item);
void func_80839F90(Player* this, PlayState* play);
s32 func_8083C61C(PlayState* play, Player* this);
void func_8083CA20(PlayState* play, Player* this);
void func_8083CA54(PlayState* play, Player* this);
void func_8083CA9C(PlayState* play, Player* this);
void func_80846648(PlayState* play, Player* this);
void func_80846660(PlayState* play, Player* this);
void func_808467D4(PlayState* play, Player* this);
void func_808468A8(PlayState* play, Player* this);
void func_808468E8(PlayState* play, Player* this);
void func_80846978(PlayState* play, Player* this);
void func_808469BC(PlayState* play, Player* this);
void func_80846A68(PlayState* play, Player* this);
void Player_UpdateCommon(Player* this, PlayState* play, Input* input);
void func_8084FF7C(Player* this);
void Player_UpdateBunnyEars(Player* this);
void func_80851008(PlayState* play, Player* this, void* anim);
void func_80851030(PlayState* play, Player* this, void* anim);
void func_80851050(PlayState* play, Player* this, void* anim);
void func_80851094(PlayState* play, Player* this, void* anim);
void func_808510B4(PlayState* play, Player* this, void* anim);
void func_808510D4(PlayState* play, Player* this, void* anim);
void func_808510F4(PlayState* play, Player* this, void* anim);
void func_80851114(PlayState* play, Player* this, void* anim);
void func_80851134(PlayState* play, Player* this, void* anim);
void func_80851154(PlayState* play, Player* this, void* anim);
void func_80851174(PlayState* play, Player* this, void* anim);
void func_80851194(PlayState* play, Player* this, void* anim);
void func_808511B4(PlayState* play, Player* this, void* anim);
void func_808511D4(PlayState* play, Player* this, void* anim);
void func_808511FC(PlayState* play, Player* this, void* anim);
void func_80851248(PlayState* play, Player* this, void* anim);
void func_80851294(PlayState* play, Player* this, void* anim);
void func_808512E0(PlayState* play, Player* this, void* arg2);
void func_80851368(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808513BC(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808514C0(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_8085157C(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808515A4(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851688(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851750(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851788(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851828(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808518DC(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_8085190C(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851998(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808519C0(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808519EC(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851A50(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851B90(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851BE8(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851CA4(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851D2C(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851D80(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851DEC(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851E28(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851E64(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851E90(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851ECC(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851F84(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80851FB0(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852048(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852080(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852174(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808521B8(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808521F4(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852234(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_8085225C(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852280(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852358(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852388(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852298(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852328(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852480(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852450(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808524B0(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808524D0(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852514(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852544(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852554(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852564(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808525C0(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852608(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852648(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808526EC(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_8085283C(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808528C8(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852944(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_808529D0(PlayState* play, Player* this, CsCmdActorCue* cue);
void func_80852C50(PlayState* play, Player* this, CsCmdActorCue* cue);
int Player_IsDroppingFish(PlayState* play);
s32 Player_StartFishing(PlayState* play);
s32 func_80852F38(PlayState* play, Player* this);
s32 Player_TryCsAction(PlayState* play, Actor* actor, s32 csAction);
void func_80853080(Player* this, PlayState* play);
s32 Player_InflictDamage(PlayState* play, s32 damage);
s32 Player_InflictDamageModified(PlayState* play, s32 damage, u8 modified);
void Player_StartTalking(PlayState* play, Actor* actor);

void Player_Action_80840450(Player* this, PlayState* play);
void Player_Action_808407CC(Player* this, PlayState* play);
void Player_Action_Idle(Player* this, PlayState* play);
void Player_Action_80840DE4(Player* this, PlayState* play);
void Player_Action_808414F8(Player* this, PlayState* play);
void Player_Action_8084170C(Player* this, PlayState* play);
void Player_Action_808417FC(Player* this, PlayState* play);
void Player_Action_8084193C(Player* this, PlayState* play);
void Player_Action_TurnInPlace(Player* this, PlayState* play);
void Player_Action_80842180(Player* this, PlayState* play);
void Player_Action_8084227C(Player* this, PlayState* play);
void Player_Action_8084279C(Player* this, PlayState* play);
void Player_Action_808423EC(Player* this, PlayState* play);
void Player_Action_8084251C(Player* this, PlayState* play);
void Player_Action_80843188(Player* this, PlayState* play);
void Player_Action_808435C4(Player* this, PlayState* play);
void Player_Action_8084370C(Player* this, PlayState* play);
void Player_Action_8084377C(Player* this, PlayState* play);
void Player_Action_80843954(Player* this, PlayState* play);
void Player_Action_80843A38(Player* this, PlayState* play);
void Player_Action_80843CEC(Player* this, PlayState* play);
void Player_Action_8084411C(Player* this, PlayState* play);
void Player_Action_Roll(Player* this, PlayState* play);
void Player_Action_80844A44(Player* this, PlayState* play);
void Player_Action_80844AF4(Player* this, PlayState* play);
void Player_Action_80844E68(Player* this, PlayState* play);
void Player_Action_80845000(Player* this, PlayState* play);
void Player_Action_80845308(Player* this, PlayState* play);
void Player_Action_80845668(Player* this, PlayState* play);
void Player_Action_WaitForPutAway(Player* this, PlayState* play);
void Player_Action_80845CA4(Player* this, PlayState* play);
void Player_Action_80845EF8(Player* this, PlayState* play);
void Player_Action_80846050(Player* this, PlayState* play);
void Player_Action_80846120(Player* this, PlayState* play);
void Player_Action_80846260(Player* this, PlayState* play);
void Player_Action_80846358(Player* this, PlayState* play);
void Player_Action_80846408(Player* this, PlayState* play);
void Player_Action_808464B0(Player* this, PlayState* play);
void Player_Action_80846578(Player* this, PlayState* play);
void Player_Action_8084B1D8(Player* this, PlayState* play);
void Player_Action_Talk(Player* this, PlayState* play);
void Player_Action_8084B78C(Player* this, PlayState* play);
void Player_Action_8084B898(Player* this, PlayState* play);
void Player_Action_8084B9E4(Player* this, PlayState* play);
void Player_Action_8084BBE4(Player* this, PlayState* play);
void Player_Action_8084BDFC(Player* this, PlayState* play);
void Player_Action_8084BF1C(Player* this, PlayState* play);
void Player_Action_8084C5F8(Player* this, PlayState* play);
void Player_Action_8084C760(Player* this, PlayState* play);
void Player_Action_8084C81C(Player* this, PlayState* play);
void Player_Action_8084CC98(Player* this, PlayState* play);
void Player_Action_8084D3E4(Player* this, PlayState* play);
void Player_Action_8084D610(Player* this, PlayState* play);
void Player_Action_8084D7C4(Player* this, PlayState* play);
void Player_Action_8084D84C(Player* this, PlayState* play);
void Player_Action_8084DAB4(Player* this, PlayState* play);
void Player_Action_8084DC48(Player* this, PlayState* play);
void Player_Action_8084E1EC(Player* this, PlayState* play);
void Player_Action_8084E30C(Player* this, PlayState* play);
void Player_Action_8084E368(Player* this, PlayState* play);
void Player_Action_8084E3C4(Player* this, PlayState* play);
void Player_Action_8084E604(Player* this, PlayState* play);
void Player_Action_8084E6D4(Player* this, PlayState* play);
void Player_Action_8084E9AC(Player* this, PlayState* play);
void Player_Action_8084EAC0(Player* this, PlayState* play);
void Player_Action_SwingBottle(Player* this, PlayState* play);
void Player_Action_8084EED8(Player* this, PlayState* play);
void Player_Action_8084EFC0(Player* this, PlayState* play);
void Player_Action_ExchangeItem(Player* this, PlayState* play);
void Player_Action_SlideOnSlope(Player* this, PlayState* play);
void Player_Action_8084F608(Player* this, PlayState* play);
void Player_Action_8084F698(Player* this, PlayState* play);
void Player_Action_8084F710(Player* this, PlayState* play);
void Player_Action_8084F88C(Player* this, PlayState* play);
void Player_Action_8084F9A0(Player* this, PlayState* play);
void Player_Action_8084F9C0(Player* this, PlayState* play);
void Player_Action_8084FA54(Player* this, PlayState* play);
void Player_Action_8084FB10(Player* this, PlayState* play);
void Player_Action_8084FBF4(Player* this, PlayState* play);
void Player_Action_808502D0(Player* this, PlayState* play);
void Player_Action_808505DC(Player* this, PlayState* play);
void Player_Action_8085063C(Player* this, PlayState* play);
void Player_Action_8085076C(Player* this, PlayState* play);
void Player_Action_808507F4(Player* this, PlayState* play);
void Player_Action_80850AEC(Player* this, PlayState* play);
void Player_Action_80850C68(Player* this, PlayState* play);
void Player_Action_80850E84(Player* this, PlayState* play);
void Player_Action_CsAction(Player* this, PlayState* play);

#pragma region [SoH]
u8 gWalkSpeedToggle1;
u8 gWalkSpeedToggle2;

s32 spawn_boomerang_ivan(EnPartner* this, PlayState* play) {
    if (!CVarGetInteger(CVAR_ENHANCEMENT("IvanCoopModeEnabled"), 0)) {
        return 0;
    }

    f32 posX = (Math_SinS(this->actor.shape.rot.y) * 1.0f) + this->actor.world.pos.x;
    f32 posZ = (Math_CosS(this->actor.shape.rot.y) * 1.0f) + this->actor.world.pos.z;
    s32 yaw = this->actor.shape.rot.y;
    EnBoom* boomerang =
        (EnBoom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOOM, posX, this->actor.world.pos.y + 7.0f, posZ,
                             this->actor.focus.rot.x, yaw, 0, 0, true);

    this->boomerangActor = &boomerang->actor;
    if (boomerang != NULL) {
        boomerang->returnTimer = 20;
        Audio_PlayActorSound2(&this->actor, NA_SE_IT_BOOMERANG_THROW);
    }

    return 1;
}

// Sets a flag according to which type of flag is specified in player->pendingFlag.flagType
// and which flag is specified in player->pendingFlag.flagID.
void Player_SetPendingFlag(Player* this, PlayState* play) {
    switch (this->pendingFlag.flagType) {
        case FLAG_SCENE_SWITCH:
            Flags_SetSwitch(play, this->pendingFlag.flagID);
            break;
        case FLAG_SCENE_TREASURE:
            Flags_SetTreasure(play, this->pendingFlag.flagID);
            break;
        case FLAG_SCENE_CLEAR:
            Flags_SetClear(play, this->pendingFlag.flagID);
            break;
        case FLAG_SCENE_COLLECTIBLE:
            Flags_SetCollectible(play, this->pendingFlag.flagID);
            break;
        case FLAG_EVENT_CHECK_INF:
            Flags_SetEventChkInf(this->pendingFlag.flagID);
            break;
        case FLAG_ITEM_GET_INF:
            Flags_SetItemGetInf(this->pendingFlag.flagID);
            break;
        case FLAG_INF_TABLE:
            Flags_SetInfTable(this->pendingFlag.flagID);
            break;
        case FLAG_EVENT_INF:
            Flags_SetEventInf(this->pendingFlag.flagID);
            break;
        case FLAG_RANDOMIZER_INF:
            Flags_SetRandomizerInf(this->pendingFlag.flagID);
            break;
        case FLAG_NONE:
        default:
            break;
    }
    this->pendingFlag.flagType = FLAG_NONE;
    this->pendingFlag.flagID = 0;
}
#pragma endregion

// .bss part 1
static s32 D_80858AA0;
static s32 sSavedCurrentMask;
static Vec3f sInteractWallCheckResult;
static Input* sControlInput;

// .data

static u8 sUpperBodyLimbCopyMap[PLAYER_LIMB_MAX] = {
    false, // PLAYER_LIMB_NONE
    false, // PLAYER_LIMB_ROOT
    false, // PLAYER_LIMB_WAIST
    false, // PLAYER_LIMB_LOWER
    false, // PLAYER_LIMB_R_THIGH
    false, // PLAYER_LIMB_R_SHIN
    false, // PLAYER_LIMB_R_FOOT
    false, // PLAYER_LIMB_L_THIGH
    false, // PLAYER_LIMB_L_SHIN
    false, // PLAYER_LIMB_L_FOOT
    true,  // PLAYER_LIMB_UPPER
    true,  // PLAYER_LIMB_HEAD
    true,  // PLAYER_LIMB_HAT
    true,  // PLAYER_LIMB_COLLAR
    true,  // PLAYER_LIMB_L_SHOULDER
    true,  // PLAYER_LIMB_L_FOREARM
    true,  // PLAYER_LIMB_L_HAND
    true,  // PLAYER_LIMB_R_SHOULDER
    true,  // PLAYER_LIMB_R_FOREARM
    true,  // PLAYER_LIMB_R_HAND
    true,  // PLAYER_LIMB_SHEATH
    true   // PLAYER_LIMB_TORSO
};

static PlayerAgeProperties sAgeProperties[] = {
    {
        56.0f,            // ceilingCheckHeight
        90.0f,            // unk_04
        1.0f,             // unk_08
        111.0f,           // unk_0C
        70.0f,            // unk_10
        79.4f,            // unk_14
        59.0f,            // unk_18
        41.0f,            // unk_1C
        19.0f,            // unk_20
        36.0f,            // unk_24
        44.8f,            // unk_28
        56.0f,            // unk_2C
        68.0f,            // unk_30
        70.0f,            // unk_34
        18.0f,            // wallCheckRadius
        15.0f,            // unk_3C
        70.0f,            // unk_40
        { 9, 4671, 359 }, // unk_44
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // unk_4A
        {
            { 9, 6122, 359 },
            { 9, 7693, 380 },
            { 9, 6122, 359 },
            { 9, 7693, 380 },
        }, // unk_62
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // unk_7A
        {
            { -1592, 4694, 380 },
            { -1591, 6122, 359 },
        },                                     // unk_86
        0,                                     // unk_92
        0x80,                                  // unk_94
        &gPlayerAnim_link_demo_Tbox_open,      // unk_98
        &gPlayerAnim_link_demo_back_to_past,   // unk_9C
        &gPlayerAnim_link_demo_return_to_past, // unk_A0
        &gPlayerAnim_link_normal_climb_startA, // unk_A4
        &gPlayerAnim_link_normal_climb_startB, // unk_A8
        { &gPlayerAnim_link_normal_climb_upL, &gPlayerAnim_link_normal_climb_upR, &gPlayerAnim_link_normal_Fclimb_upL,
          &gPlayerAnim_link_normal_Fclimb_upR },                                          // unk_AC
        { &gPlayerAnim_link_normal_Fclimb_sideL, &gPlayerAnim_link_normal_Fclimb_sideR }, // unk_BC
        { &gPlayerAnim_link_normal_climb_endAL, &gPlayerAnim_link_normal_climb_endAR },   // unk_C4
        { &gPlayerAnim_link_normal_climb_endBR, &gPlayerAnim_link_normal_climb_endBL },   // unk_CC
    },
    {
        40.0f,                   // ceilingCheckHeight
        60.0f,                   // unk_04
        11.0f / 17.0f,           // unk_08
        71.0f,                   // unk_0C
        50.0f,                   // unk_10
        47.0f,                   // unk_14
        39.0f,                   // unk_18
        27.0f,                   // unk_1C
        19.0f,                   // unk_20
        22.0f,                   // unk_24
        29.6f,                   // unk_28
        32.0f,                   // unk_2C
        48.0f,                   // unk_30
        70.0f * (11.0f / 17.0f), // unk_34
        14.0f,                   // wallCheckRadius
        12.0f,                   // unk_3C
        55.0f,                   // unk_40
        { -24, 3565, 876 },      // unk_44
        {
            { -24, 3474, 862 },
            { -24, 4977, 937 },
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // unk_4A
        {
            { -24, 4977, 937 },
            { -24, 6495, 937 },
            { 9, 6122, 359 },
            { 9, 7693, 380 },
        }, // unk_62
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // unk_7A
        {
            { -1592, 4694, 380 },
            { -1591, 6122, 359 },
        },                                        // unk_86
        0x20,                                     // unk_92
        0,                                        // unk_94
        &gPlayerAnim_clink_demo_Tbox_open,        // unk_98
        &gPlayerAnim_clink_demo_goto_future,      // unk_9C
        &gPlayerAnim_clink_demo_return_to_future, // unk_A0
        &gPlayerAnim_clink_normal_climb_startA,   // unk_A4
        &gPlayerAnim_clink_normal_climb_startB,   // unk_A8
        { &gPlayerAnim_clink_normal_climb_upL, &gPlayerAnim_clink_normal_climb_upR, &gPlayerAnim_link_normal_Fclimb_upL,
          &gPlayerAnim_link_normal_Fclimb_upR },                                          // unk_AC
        { &gPlayerAnim_link_normal_Fclimb_sideL, &gPlayerAnim_link_normal_Fclimb_sideR }, // unk_BC
        { &gPlayerAnim_clink_normal_climb_endAL, &gPlayerAnim_clink_normal_climb_endAR }, // unk_C4
        { &gPlayerAnim_clink_normal_climb_endBR, &gPlayerAnim_clink_normal_climb_endBL }, // unk_CC
    },
};

static u32 sNoclipEnabled = false;
static f32 sControlStickMagnitude = 0.0f;
static s16 sControlStickAngle = 0;
static s16 sControlStickWorldYaw = 0;
static s32 sUpperBodyIsBusy = false; // see `Player_UpdateUpperBody`
static s32 sFloorType = 0;
static f32 sWaterSpeedFactor = 1.0f;    // Set to 0.5f in water, 1.0f otherwise. Influences different speed values.
static f32 sInvWaterSpeedFactor = 1.0f; // Inverse of `sWaterSpeedFactor` (1.0f / sWaterSpeedFactor)
static u32 sTouchedWallFlags = 0;
static u32 sConveyorSpeed = 0;
static s16 sIsFloorConveyor = false;
static s16 sConveyorYaw = 0;
static f32 sYDistToFloor = 0.0f;
static s32 sPrevFloorProperty = 0; // floor property from the previous frame
static s32 sShapeYawToTouchedWall = 0;
static s32 sWorldYawToTouchedWall = 0;
static s16 sFloorShapePitch = 0;
static s32 sUseHeldItem = false; // When true, the current held item is used. Is reset to false every frame.
static s32 sHeldItemButtonIsHeldDown = false; // Indicates if the button for the current held item is held down.

static u16 D_8085361C[] = {
    NA_SE_VO_LI_SWEAT,
    NA_SE_VO_LI_SNEEZE,
    NA_SE_VO_LI_RELAX,
    NA_SE_VO_LI_FALL_L,
};

#define GET_PLAYER_ANIM(group, type) D_80853914[group][type]

static LinkAnimationHeader* D_80853914[PLAYER_ANIMGROUP_MAX][PLAYER_ANIMTYPE_MAX] = {
    /* PLAYER_ANIMGROUP_wait */
    {
        &gPlayerAnim_link_normal_wait_free,
        &gPlayerAnim_link_normal_wait,
        &gPlayerAnim_link_normal_wait,
        &gPlayerAnim_link_fighter_wait_long,
        &gPlayerAnim_link_normal_wait_free,
        &gPlayerAnim_link_normal_wait_free,
    },
    /* PLAYER_ANIMGROUP_walk */
    {
        &gPlayerAnim_link_normal_walk_free,
        &gPlayerAnim_link_normal_walk,
        &gPlayerAnim_link_normal_walk,
        &gPlayerAnim_link_fighter_walk_long,
        &gPlayerAnim_link_normal_walk_free,
        &gPlayerAnim_link_normal_walk_free,
    },
    /* PLAYER_ANIMGROUP_run */
    {
        &gPlayerAnim_link_normal_run_free,
        &gPlayerAnim_link_fighter_run,
        &gPlayerAnim_link_normal_run,
        &gPlayerAnim_link_fighter_run_long,
        &gPlayerAnim_link_normal_run_free,
        &gPlayerAnim_link_normal_run_free,
    },
    /* PLAYER_ANIMGROUP_damage_run */
    {
        &gPlayerAnim_link_normal_damage_run_free,
        &gPlayerAnim_link_fighter_damage_run,
        &gPlayerAnim_link_normal_damage_run_free,
        &gPlayerAnim_link_fighter_damage_run_long,
        &gPlayerAnim_link_normal_damage_run_free,
        &gPlayerAnim_link_normal_damage_run_free,
    },
    /* PLAYER_ANIMGROUP_heavy_run */
    {
        &gPlayerAnim_link_normal_heavy_run_free,
        &gPlayerAnim_link_normal_heavy_run,
        &gPlayerAnim_link_normal_heavy_run_free,
        &gPlayerAnim_link_fighter_heavy_run_long,
        &gPlayerAnim_link_normal_heavy_run_free,
        &gPlayerAnim_link_normal_heavy_run_free,
    },
    /* PLAYER_ANIMGROUP_waitL */
    {
        &gPlayerAnim_link_normal_waitL_free,
        &gPlayerAnim_link_anchor_waitL,
        &gPlayerAnim_link_anchor_waitL,
        &gPlayerAnim_link_fighter_waitL_long,
        &gPlayerAnim_link_normal_waitL_free,
        &gPlayerAnim_link_normal_waitL_free,
    },
    /* PLAYER_ANIMGROUP_waitR */
    {
        &gPlayerAnim_link_normal_waitR_free,
        &gPlayerAnim_link_anchor_waitR,
        &gPlayerAnim_link_anchor_waitR,
        &gPlayerAnim_link_fighter_waitR_long,
        &gPlayerAnim_link_normal_waitR_free,
        &gPlayerAnim_link_normal_waitR_free,
    },
    /* PLAYER_ANIMGROUP_wait2waitR */
    {
        &gPlayerAnim_link_fighter_wait2waitR_long,
        &gPlayerAnim_link_normal_wait2waitR,
        &gPlayerAnim_link_normal_wait2waitR,
        &gPlayerAnim_link_fighter_wait2waitR_long,
        &gPlayerAnim_link_fighter_wait2waitR_long,
        &gPlayerAnim_link_fighter_wait2waitR_long,
    },
    /* PLAYER_ANIMGROUP_normal2fighter */
    {
        &gPlayerAnim_link_normal_normal2fighter_free,
        &gPlayerAnim_link_fighter_normal2fighter,
        &gPlayerAnim_link_fighter_normal2fighter,
        &gPlayerAnim_link_normal_normal2fighter_free,
        &gPlayerAnim_link_normal_normal2fighter_free,
        &gPlayerAnim_link_normal_normal2fighter_free,
    },
    /* PLAYER_ANIMGROUP_doorA_free */
    {
        &gPlayerAnim_link_demo_doorA_link_free,
        &gPlayerAnim_link_demo_doorA_link,
        &gPlayerAnim_link_demo_doorA_link,
        &gPlayerAnim_link_demo_doorA_link_free,
        &gPlayerAnim_link_demo_doorA_link_free,
        &gPlayerAnim_link_demo_doorA_link_free,
    },
    /* PLAYER_ANIMGROUP_doorA */
    {
        &gPlayerAnim_clink_demo_doorA_link,
        &gPlayerAnim_clink_demo_doorA_link,
        &gPlayerAnim_clink_demo_doorA_link,
        &gPlayerAnim_clink_demo_doorA_link,
        &gPlayerAnim_clink_demo_doorA_link,
        &gPlayerAnim_clink_demo_doorA_link,
    },
    /* PLAYER_ANIMGROUP_doorB_free */
    {
        &gPlayerAnim_link_demo_doorB_link_free,
        &gPlayerAnim_link_demo_doorB_link,
        &gPlayerAnim_link_demo_doorB_link,
        &gPlayerAnim_link_demo_doorB_link_free,
        &gPlayerAnim_link_demo_doorB_link_free,
        &gPlayerAnim_link_demo_doorB_link_free,
    },
    /* PLAYER_ANIMGROUP_doorB */
    {
        &gPlayerAnim_clink_demo_doorB_link,
        &gPlayerAnim_clink_demo_doorB_link,
        &gPlayerAnim_clink_demo_doorB_link,
        &gPlayerAnim_clink_demo_doorB_link,
        &gPlayerAnim_clink_demo_doorB_link,
        &gPlayerAnim_clink_demo_doorB_link,
    },
    /* PLAYER_ANIMGROUP_carryB */
    {
        &gPlayerAnim_link_normal_carryB_free,
        &gPlayerAnim_link_normal_carryB,
        &gPlayerAnim_link_normal_carryB,
        &gPlayerAnim_link_normal_carryB_free,
        &gPlayerAnim_link_normal_carryB_free,
        &gPlayerAnim_link_normal_carryB_free,
    },
    /* PLAYER_ANIMGROUP_landing */
    {
        &gPlayerAnim_link_normal_landing_free,
        &gPlayerAnim_link_normal_landing,
        &gPlayerAnim_link_normal_landing,
        &gPlayerAnim_link_normal_landing_free,
        &gPlayerAnim_link_normal_landing_free,
        &gPlayerAnim_link_normal_landing_free,
    },
    /* PLAYER_ANIMGROUP_short_landing */
    {
        &gPlayerAnim_link_normal_short_landing_free,
        &gPlayerAnim_link_normal_short_landing,
        &gPlayerAnim_link_normal_short_landing,
        &gPlayerAnim_link_normal_short_landing_free,
        &gPlayerAnim_link_normal_short_landing_free,
        &gPlayerAnim_link_normal_short_landing_free,
    },
    /* PLAYER_ANIMGROUP_landing_roll */
    {
        &gPlayerAnim_link_normal_landing_roll_free,
        &gPlayerAnim_link_normal_landing_roll,
        &gPlayerAnim_link_normal_landing_roll,
        &gPlayerAnim_link_fighter_landing_roll_long,
        &gPlayerAnim_link_normal_landing_roll_free,
        &gPlayerAnim_link_normal_landing_roll_free,
    },
    /* PLAYER_ANIMGROUP_hip_down */
    {
        &gPlayerAnim_link_normal_hip_down_free,
        &gPlayerAnim_link_normal_hip_down,
        &gPlayerAnim_link_normal_hip_down,
        &gPlayerAnim_link_normal_hip_down_long,
        &gPlayerAnim_link_normal_hip_down_free,
        &gPlayerAnim_link_normal_hip_down_free,
    },
    /* PLAYER_ANIMGROUP_walk_endL */
    {
        &gPlayerAnim_link_normal_walk_endL_free,
        &gPlayerAnim_link_normal_walk_endL,
        &gPlayerAnim_link_normal_walk_endL,
        &gPlayerAnim_link_fighter_walk_endL_long,
        &gPlayerAnim_link_normal_walk_endL_free,
        &gPlayerAnim_link_normal_walk_endL_free,
    },
    /* PLAYER_ANIMGROUP_walk_endR */
    {
        &gPlayerAnim_link_normal_walk_endR_free,
        &gPlayerAnim_link_normal_walk_endR,
        &gPlayerAnim_link_normal_walk_endR,
        &gPlayerAnim_link_fighter_walk_endR_long,
        &gPlayerAnim_link_normal_walk_endR_free,
        &gPlayerAnim_link_normal_walk_endR_free,
    },
    /* PLAYER_ANIMGROUP_defense */
    {
        &gPlayerAnim_link_normal_defense_free,
        &gPlayerAnim_link_normal_defense,
        &gPlayerAnim_link_normal_defense,
        &gPlayerAnim_link_normal_defense_free,
        &gPlayerAnim_link_bow_defense,
        &gPlayerAnim_link_normal_defense_free,
    },
    /* PLAYER_ANIMGROUP_defense_wait */
    {
        &gPlayerAnim_link_normal_defense_wait_free,
        &gPlayerAnim_link_normal_defense_wait,
        &gPlayerAnim_link_normal_defense_wait,
        &gPlayerAnim_link_normal_defense_wait_free,
        &gPlayerAnim_link_bow_defense_wait,
        &gPlayerAnim_link_normal_defense_wait_free,
    },
    /* PLAYER_ANIMGROUP_defense_end */
    {
        &gPlayerAnim_link_normal_defense_end_free,
        &gPlayerAnim_link_normal_defense_end,
        &gPlayerAnim_link_normal_defense_end,
        &gPlayerAnim_link_normal_defense_end_free,
        &gPlayerAnim_link_normal_defense_end_free,
        &gPlayerAnim_link_normal_defense_end_free,
    },
    /* PLAYER_ANIMGROUP_side_walk */
    {
        &gPlayerAnim_link_normal_side_walk_free,
        &gPlayerAnim_link_normal_side_walk,
        &gPlayerAnim_link_normal_side_walk,
        &gPlayerAnim_link_fighter_side_walk_long,
        &gPlayerAnim_link_normal_side_walk_free,
        &gPlayerAnim_link_normal_side_walk_free,
    },
    /* PLAYER_ANIMGROUP_side_walkL */
    {
        &gPlayerAnim_link_normal_side_walkL_free,
        &gPlayerAnim_link_anchor_side_walkL,
        &gPlayerAnim_link_anchor_side_walkL,
        &gPlayerAnim_link_fighter_side_walkL_long,
        &gPlayerAnim_link_normal_side_walkL_free,
        &gPlayerAnim_link_normal_side_walkL_free,
    },
    /* PLAYER_ANIMGROUP_side_walkR */
    {
        &gPlayerAnim_link_normal_side_walkR_free,
        &gPlayerAnim_link_anchor_side_walkR,
        &gPlayerAnim_link_anchor_side_walkR,
        &gPlayerAnim_link_fighter_side_walkR_long,
        &gPlayerAnim_link_normal_side_walkR_free,
        &gPlayerAnim_link_normal_side_walkR_free,
    },
    /* PLAYER_ANIMGROUP_45_turn */
    {
        &gPlayerAnim_link_normal_45_turn_free,
        &gPlayerAnim_link_normal_45_turn,
        &gPlayerAnim_link_normal_45_turn,
        &gPlayerAnim_link_normal_45_turn_free,
        &gPlayerAnim_link_normal_45_turn_free,
        &gPlayerAnim_link_normal_45_turn_free,
    },
    /* PLAYER_ANIMGROUP_waitL2wait */
    {
        &gPlayerAnim_link_fighter_waitL2wait_long,
        &gPlayerAnim_link_normal_waitL2wait,
        &gPlayerAnim_link_normal_waitL2wait,
        &gPlayerAnim_link_fighter_waitL2wait_long,
        &gPlayerAnim_link_fighter_waitL2wait_long,
        &gPlayerAnim_link_fighter_waitL2wait_long,
    },
    /* PLAYER_ANIMGROUP_waitR2wait */
    {
        &gPlayerAnim_link_fighter_waitR2wait_long,
        &gPlayerAnim_link_normal_waitR2wait,
        &gPlayerAnim_link_normal_waitR2wait,
        &gPlayerAnim_link_fighter_waitR2wait_long,
        &gPlayerAnim_link_fighter_waitR2wait_long,
        &gPlayerAnim_link_fighter_waitR2wait_long,
    },
    /* PLAYER_ANIMGROUP_throw */
    {
        &gPlayerAnim_link_normal_throw_free,
        &gPlayerAnim_link_normal_throw,
        &gPlayerAnim_link_normal_throw,
        &gPlayerAnim_link_normal_throw_free,
        &gPlayerAnim_link_normal_throw_free,
        &gPlayerAnim_link_normal_throw_free,
    },
    /* PLAYER_ANIMGROUP_put */
    {
        &gPlayerAnim_link_normal_put_free,
        &gPlayerAnim_link_normal_put,
        &gPlayerAnim_link_normal_put,
        &gPlayerAnim_link_normal_put_free,
        &gPlayerAnim_link_normal_put_free,
        &gPlayerAnim_link_normal_put_free,
    },
    /* PLAYER_ANIMGROUP_back_walk */
    {
        &gPlayerAnim_link_normal_back_walk,
        &gPlayerAnim_link_normal_back_walk,
        &gPlayerAnim_link_normal_back_walk,
        &gPlayerAnim_link_normal_back_walk,
        &gPlayerAnim_link_normal_back_walk,
        &gPlayerAnim_link_normal_back_walk,
    },
    /* PLAYER_ANIMGROUP_check */
    {
        &gPlayerAnim_link_normal_check_free,
        &gPlayerAnim_link_normal_check,
        &gPlayerAnim_link_normal_check,
        &gPlayerAnim_link_normal_check_free,
        &gPlayerAnim_link_normal_check_free,
        &gPlayerAnim_link_normal_check_free,
    },
    /* PLAYER_ANIMGROUP_check_wait */
    {
        &gPlayerAnim_link_normal_check_wait_free,
        &gPlayerAnim_link_normal_check_wait,
        &gPlayerAnim_link_normal_check_wait,
        &gPlayerAnim_link_normal_check_wait_free,
        &gPlayerAnim_link_normal_check_wait_free,
        &gPlayerAnim_link_normal_check_wait_free,
    },
    /* PLAYER_ANIMGROUP_check_end */
    {
        &gPlayerAnim_link_normal_check_end_free,
        &gPlayerAnim_link_normal_check_end,
        &gPlayerAnim_link_normal_check_end,
        &gPlayerAnim_link_normal_check_end_free,
        &gPlayerAnim_link_normal_check_end_free,
        &gPlayerAnim_link_normal_check_end_free,
    },
    /* PLAYER_ANIMGROUP_pull_start */
    {
        &gPlayerAnim_link_normal_pull_start_free,
        &gPlayerAnim_link_normal_pull_start,
        &gPlayerAnim_link_normal_pull_start,
        &gPlayerAnim_link_normal_pull_start_free,
        &gPlayerAnim_link_normal_pull_start_free,
        &gPlayerAnim_link_normal_pull_start_free,
    },
    /* PLAYER_ANIMGROUP_pulling */
    {
        &gPlayerAnim_link_normal_pulling_free,
        &gPlayerAnim_link_normal_pulling,
        &gPlayerAnim_link_normal_pulling,
        &gPlayerAnim_link_normal_pulling_free,
        &gPlayerAnim_link_normal_pulling_free,
        &gPlayerAnim_link_normal_pulling_free,
    },
    /* PLAYER_ANIMGROUP_pull_end */
    {
        &gPlayerAnim_link_normal_pull_end_free,
        &gPlayerAnim_link_normal_pull_end,
        &gPlayerAnim_link_normal_pull_end,
        &gPlayerAnim_link_normal_pull_end_free,
        &gPlayerAnim_link_normal_pull_end_free,
        &gPlayerAnim_link_normal_pull_end_free,
    },
    /* PLAYER_ANIMGROUP_fall_up */
    {
        &gPlayerAnim_link_normal_fall_up_free,
        &gPlayerAnim_link_normal_fall_up,
        &gPlayerAnim_link_normal_fall_up,
        &gPlayerAnim_link_normal_fall_up_free,
        &gPlayerAnim_link_normal_fall_up_free,
        &gPlayerAnim_link_normal_fall_up_free,
    },
    /* PLAYER_ANIMGROUP_jump_climb_hold */
    {
        &gPlayerAnim_link_normal_jump_climb_hold_free,
        &gPlayerAnim_link_normal_jump_climb_hold,
        &gPlayerAnim_link_normal_jump_climb_hold,
        &gPlayerAnim_link_normal_jump_climb_hold_free,
        &gPlayerAnim_link_normal_jump_climb_hold_free,
        &gPlayerAnim_link_normal_jump_climb_hold_free,
    },
    /* PLAYER_ANIMGROUP_jump_climb_wait */
    {
        &gPlayerAnim_link_normal_jump_climb_wait_free,
        &gPlayerAnim_link_normal_jump_climb_wait,
        &gPlayerAnim_link_normal_jump_climb_wait,
        &gPlayerAnim_link_normal_jump_climb_wait_free,
        &gPlayerAnim_link_normal_jump_climb_wait_free,
        &gPlayerAnim_link_normal_jump_climb_wait_free,
    },
    /* PLAYER_ANIMGROUP_jump_climb_up */
    {
        &gPlayerAnim_link_normal_jump_climb_up_free,
        &gPlayerAnim_link_normal_jump_climb_up,
        &gPlayerAnim_link_normal_jump_climb_up,
        &gPlayerAnim_link_normal_jump_climb_up_free,
        &gPlayerAnim_link_normal_jump_climb_up_free,
        &gPlayerAnim_link_normal_jump_climb_up_free,
    },
    /* PLAYER_ANIMGROUP_down_slope_slip_end */
    {
        &gPlayerAnim_link_normal_down_slope_slip_end_free,
        &gPlayerAnim_link_normal_down_slope_slip_end,
        &gPlayerAnim_link_normal_down_slope_slip_end,
        &gPlayerAnim_link_normal_down_slope_slip_end_long,
        &gPlayerAnim_link_normal_down_slope_slip_end_free,
        &gPlayerAnim_link_normal_down_slope_slip_end_free,
    },
    /* PLAYER_ANIMGROUP_up_slope_slip_end */
    {
        &gPlayerAnim_link_normal_up_slope_slip_end_free,
        &gPlayerAnim_link_normal_up_slope_slip_end,
        &gPlayerAnim_link_normal_up_slope_slip_end,
        &gPlayerAnim_link_normal_up_slope_slip_end_long,
        &gPlayerAnim_link_normal_up_slope_slip_end_free,
        &gPlayerAnim_link_normal_up_slope_slip_end_free,
    },
    /* PLAYER_ANIMGROUP_nwait */
    {
        &gPlayerAnim_sude_nwait,
        &gPlayerAnim_lkt_nwait,
        &gPlayerAnim_lkt_nwait,
        &gPlayerAnim_sude_nwait,
        &gPlayerAnim_sude_nwait,
        &gPlayerAnim_sude_nwait,
    }
};

static LinkAnimationHeader* D_80853D4C[][3] = {
    { &gPlayerAnim_link_fighter_front_jump, &gPlayerAnim_link_fighter_front_jump_end,
      &gPlayerAnim_link_fighter_front_jump_endR },
    { &gPlayerAnim_link_fighter_Lside_jump, &gPlayerAnim_link_fighter_Lside_jump_end,
      &gPlayerAnim_link_fighter_Lside_jump_endL },
    { &gPlayerAnim_link_fighter_backturn_jump, &gPlayerAnim_link_fighter_backturn_jump_end,
      &gPlayerAnim_link_fighter_backturn_jump_endR },
    { &gPlayerAnim_link_fighter_Rside_jump, &gPlayerAnim_link_fighter_Rside_jump_end,
      &gPlayerAnim_link_fighter_Rside_jump_endR },
};

typedef enum FidgetType {
    /* 0x00 */ FIDGET_LOOK_AROUND, // ROOM_ENV_DEFAULT
    /* 0x01 */ FIDGET_COLD,        // ROOM_ENV_COLD
    /* 0x02 */ FIDGET_WARM,        // ROOM_ENV_WARM
    /* 0x03 */ FIDGET_HOT,         // ROOM_ENV_HOT (same animations as FIDGET_WARM)
    /* 0x04 */ FIDGET_STRETCH_1,   // ROOM_ENV_UNK_STRETCH_1
    /* 0x05 */ FIDGET_STRETCH_2,   // ROOM_ENV_UNK_STRETCH_1 (same animations as FIDGET_STRETCH_1)
    /* 0x06 */ FIDGET_STRETCH_3,   // ROOM_ENV_UNK_STRETCH_1 (same animations as FIDGET_STRETCH_1)
    /* 0x07 */ FIDGET_CRIT_HEALTH_START,
    /* 0x08 */ FIDGET_CRIT_HEALTH_LOOP,
    /* 0x09 */ FIDGET_SWORD_SWING,
    /* 0x0A */ FIDGET_ADJUST_TUNIC,
    /* 0x0B */ FIDGET_TAP_FEET,
    /* 0x0C */ FIDGET_ADJUST_SHIELD,
    /* 0x0D */ FIDGET_SWORD_SWING_TWO_HAND
} FidgetType;

static LinkAnimationHeader* sFidgetAnimations[][2] = {
    // FIDGET_LOOK_AROUND
    { &gPlayerAnim_link_normal_wait_typeA_20f, &gPlayerAnim_link_normal_waitF_typeA_20f },

    // FIDGET_COLD
    { &gPlayerAnim_link_normal_wait_typeC_20f, &gPlayerAnim_link_normal_waitF_typeC_20f },

    // FIDGET_WARM
    { &gPlayerAnim_link_normal_wait_typeB_20f, &gPlayerAnim_link_normal_waitF_typeB_20f },

    // FIDGET_HOT
    { &gPlayerAnim_link_normal_wait_typeB_20f, &gPlayerAnim_link_normal_waitF_typeB_20f },

    // FIDGET_STRETCH_1
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },

    // FIDGET_STRETCH_2
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },

    // FIDGET_STRETCH_3
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },

    // FIDGET_CRIT_HEALTH_START
    { &gPlayerAnim_link_wait_heat1_20f, &gPlayerAnim_link_waitF_heat1_20f },

    // FIDGET_CRIT_HEALTH_LOOP
    { &gPlayerAnim_link_wait_heat2_20f, &gPlayerAnim_link_waitF_heat2_20f },

    // FIDGET_SWORD_SWING
    { &gPlayerAnim_link_wait_itemD1_20f, &gPlayerAnim_link_wait_itemD1_20f },

    // FIDGET_ADJUST_TUNIC
    { &gPlayerAnim_link_wait_itemA_20f, &gPlayerAnim_link_waitF_itemA_20f },

    // FIDGET_TAP_FEET
    { &gPlayerAnim_link_wait_itemB_20f, &gPlayerAnim_link_waitF_itemB_20f },

    // FIDGET_ADJUST_SHIELD
    { &gPlayerAnim_link_wait_itemC_20f, &gPlayerAnim_link_wait_itemC_20f },

    // FIDGET_SWORD_SWING_TWO_HAND
    { &gPlayerAnim_link_wait_itemD2_20f, &gPlayerAnim_link_wait_itemD2_20f }
};

static AnimSfxEntry sFidgetAnimSfxSneeze[] = {
    { NA_SE_VO_LI_SNEEZE, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 8) },
};

static AnimSfxEntry sFidgetAnimSfxSweat[] = {
    { NA_SE_VO_LI_SWEAT, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 18) },
};

static AnimSfxEntry sFidgetAnimSfxCritHealthStart[] = {
    { NA_SE_VO_LI_BREATH_REST, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 13) },
};

static AnimSfxEntry sFidgetAnimSfxCritHealthLoop[] = {
    { NA_SE_VO_LI_BREATH_REST, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 10) },
};

static AnimSfxEntry sFidgetAnimSfxTunic[] = {
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 44) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 48) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 52) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 56) },
    { NA_SE_PL_CALM_HIT, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 60) },
};

static AnimSfxEntry sFidgetAnimSfxTapFeet[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 25) }, { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 30) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 44) }, { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 48) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 52) }, { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 56) },
};

static AnimSfxEntry sFidgetAnimSfxShield[] = {
    { NA_SE_IT_SHIELD_POSTURE, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 16) },
    { NA_SE_IT_SHIELD_POSTURE, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 20) },
    { NA_SE_IT_SHIELD_POSTURE, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 70) },
};

static AnimSfxEntry sFidgetAnimSfxSword[] = {
    { NA_SE_IT_HAMMER_SWING, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 10) },
    { NA_SE_VO_LI_AUTO_JUMP, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 10) },
    { NA_SE_IT_SWORD_SWING, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 22) },
    { NA_SE_VO_LI_SWORD_N, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 22) },
};

static AnimSfxEntry sFidgetAnimSfxSwordTwoHand[] = {
    { NA_SE_IT_SWORD_SWING, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 39) },
    { NA_SE_VO_LI_SWORD_N, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 39) },
};

static AnimSfxEntry sFidgetAnimSfxStretch[] = {
    { NA_SE_VO_LI_RELAX, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 20) },
};

typedef enum FidgetAnimSfxType {
    /* 0x0 */ FIDGET_ANIMSFX_NONE,
    /* 0x1 */ FIDGET_ANIMSFX_SNEEZE,
    /* 0x2 */ FIDGET_ANIMSFX_SWEAT,
    /* 0x3 */ FIDGET_ANIMSFX_CRIT_HEALTH_START,
    /* 0x4 */ FIDGET_ANIMSFX_CRIT_HEALTH_LOOP,
    /* 0x5 */ FIDGET_ANIMSFX_TUNIC,
    /* 0x6 */ FIDGET_ANIMSFX_TAP_FEET,
    /* 0x7 */ FIDGET_ANIMSFX_SHIELD,
    /* 0x8 */ FIDGET_ANIMSFX_SWORD,
    /* 0x9 */ FIDGET_ANIMSFX_SWORD_TWO_HAND,
    /* 0xA */ FIDGET_ANIMSFX_STRETCH
} FidgetAnimSfxType;

static AnimSfxEntry* sFidgetAnimSfxLists[] = {
    sFidgetAnimSfxSneeze,          // FIDGET_ANIMSFX_SNEEZE
    sFidgetAnimSfxSweat,           // FIDGET_ANIMSFX_SWEAT
    sFidgetAnimSfxCritHealthStart, // FIDGET_ANIMSFX_CRIT_HEALTH_START
    sFidgetAnimSfxCritHealthLoop,  // FIDGET_ANIMSFX_CRIT_HEALTH_LOOP
    sFidgetAnimSfxTunic,           // FIDGET_ANIMSFX_TUNIC
    sFidgetAnimSfxTapFeet,         // FIDGET_ANIMSFX_TAP_FEET
    sFidgetAnimSfxShield,          // FIDGET_ANIMSFX_SHIELD
    sFidgetAnimSfxSword,           // FIDGET_ANIMSFX_SWORD
    sFidgetAnimSfxSwordTwoHand,    // FIDGET_ANIMSFX_SWORD_TWO_HAND
    sFidgetAnimSfxStretch,         // FIDGET_ANIMSFX_STRETCH
    NULL,                          // unused entry
};

/**
 * The indices in this array correspond 1 to 1 with the entries of sFidgetAnimations.
 * There is also an extra FIDGET_ANIMSFX_NONE at the end that doesn't correspond to any animation.
 */
static u8 sFidgetAnimSfxTypes[] = {
    FIDGET_ANIMSFX_NONE,              // FIDGET_LOOK_AROUND
    FIDGET_ANIMSFX_NONE,              // FIDGET_LOOK_AROUND (sword/shield in hand)
    FIDGET_ANIMSFX_SNEEZE,            // FIDGET_COLD
    FIDGET_ANIMSFX_SNEEZE,            // FIDGET_COLD (sword/shield in hand)
    FIDGET_ANIMSFX_SWEAT,             // FIDGET_WARM
    FIDGET_ANIMSFX_SWEAT,             // FIDGET_WARM (sword/shield in hand)
    FIDGET_ANIMSFX_SWEAT,             // FIDGET_HOT
    FIDGET_ANIMSFX_SWEAT,             // FIDGET_HOT (sword/shield in hand)
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_1
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_1 (sword/shield in hand)
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_2
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_2 (sword/shield in hand)
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_3
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_3 (sword/shield in hand)
    FIDGET_ANIMSFX_CRIT_HEALTH_START, // FIDGET_CRIT_HEALTH_START
    FIDGET_ANIMSFX_CRIT_HEALTH_START, // FIDGET_CRIT_HEALTH_START (sword/shield in hand)
    FIDGET_ANIMSFX_CRIT_HEALTH_LOOP,  // FIDGET_CRIT_HEALTH_LOOP
    FIDGET_ANIMSFX_CRIT_HEALTH_LOOP,  // FIDGET_CRIT_HEALTH_LOOP (sword/shield in hand)
    FIDGET_ANIMSFX_SWORD,             // FIDGET_SWORD_SWING
    FIDGET_ANIMSFX_SWORD,             // FIDGET_SWORD_SWING (sword/shield in hand)
    FIDGET_ANIMSFX_TUNIC,             // FIDGET_ADJUST_TUNIC
    FIDGET_ANIMSFX_TUNIC,             // FIDGET_ADJUST_TUNIC (sword/shield in hand)
    FIDGET_ANIMSFX_TAP_FEET,          // FIDGET_TAP_FEET
    FIDGET_ANIMSFX_TAP_FEET,          // FIDGET_TAP_FEET (sword/shield in hand)
    FIDGET_ANIMSFX_SHIELD,            // FIDGET_ADJUST_SHIELD
    FIDGET_ANIMSFX_SHIELD,            // FIDGET_ADJUST_SHIELD (sword/shield in hand)
    FIDGET_ANIMSFX_SWORD_TWO_HAND,    // FIDGET_SWORD_SWING_TWO_HAND
    FIDGET_ANIMSFX_SWORD_TWO_HAND,    // FIDGET_SWORD_SWING_TWO_HAND (sword/shield in hand)
    FIDGET_ANIMSFX_NONE,              // unused, doesnt correspond to any animation
};

// Used to map item IDs to item actions
static s8 sItemActions[] = {
    PLAYER_IA_DEKU_STICK,          // ITEM_DEKU_STICK
    PLAYER_IA_DEKU_NUT,            // ITEM_DEKU_NUT
    PLAYER_IA_BOMB,                // ITEM_BOMB
    PLAYER_IA_BOW,                 // ITEM_BOW
    PLAYER_IA_BOW_FIRE,            // ITEM_ARROW_FIRE
    PLAYER_IA_DINS_FIRE,           // ITEM_DINS_FIRE
    PLAYER_IA_SLINGSHOT,           // ITEM_SLINGSHOT
    PLAYER_IA_OCARINA_FAIRY,       // ITEM_OCARINA_FAIRY
    PLAYER_IA_OCARINA_OF_TIME,     // ITEM_OCARINA_OF_TIME
    PLAYER_IA_BOMBCHU,             // ITEM_BOMBCHU
    PLAYER_IA_HOOKSHOT,            // ITEM_HOOKSHOT
    PLAYER_IA_LONGSHOT,            // ITEM_LONGSHOT
    PLAYER_IA_BOW_ICE,             // ITEM_ARROW_ICE
    PLAYER_IA_FARORES_WIND,        // ITEM_FARORES_WIND
    PLAYER_IA_BOOMERANG,           // ITEM_BOOMERANG
    PLAYER_IA_LENS_OF_TRUTH,       // ITEM_LENS_OF_TRUTH
    PLAYER_IA_MAGIC_BEAN,          // ITEM_MAGIC_BEAN
    PLAYER_IA_HAMMER,              // ITEM_HAMMER
    PLAYER_IA_BOW_LIGHT,           // ITEM_ARROW_LIGHT
    PLAYER_IA_NAYRUS_LOVE,         // ITEM_NAYRUS_LOVE
    PLAYER_IA_BOTTLE,              // ITEM_BOTTLE_EMPTY
    PLAYER_IA_BOTTLE_POTION_RED,   // ITEM_BOTTLE_POTION_RED
    PLAYER_IA_BOTTLE_POTION_GREEN, // ITEM_BOTTLE_POTION_GREEN
    PLAYER_IA_BOTTLE_POTION_BLUE,  // ITEM_BOTTLE_POTION_BLUE
    PLAYER_IA_BOTTLE_FAIRY,        // ITEM_BOTTLE_FAIRY
    PLAYER_IA_BOTTLE_FISH,         // ITEM_BOTTLE_FISH
    PLAYER_IA_BOTTLE_MILK_FULL,    // ITEM_BOTTLE_MILK_FULL
    PLAYER_IA_BOTTLE_RUTOS_LETTER, // ITEM_BOTTLE_RUTOS_LETTER
    PLAYER_IA_BOTTLE_FIRE,         // ITEM_BOTTLE_BLUE_FIRE
    PLAYER_IA_BOTTLE_BUG,          // ITEM_BOTTLE_BUG
    PLAYER_IA_BOTTLE_BIG_POE,      // ITEM_BOTTLE_BIG_POE
    PLAYER_IA_BOTTLE_MILK_HALF,    // ITEM_BOTTLE_MILK_HALF
    PLAYER_IA_BOTTLE_POE,          // ITEM_BOTTLE_POE
    PLAYER_IA_WEIRD_EGG,           // ITEM_WEIRD_EGG
    PLAYER_IA_CHICKEN,             // ITEM_CHICKEN
    PLAYER_IA_ZELDAS_LETTER,       // ITEM_ZELDAS_LETTER
    PLAYER_IA_MASK_KEATON,         // ITEM_MASK_KEATON
    PLAYER_IA_MASK_SKULL,          // ITEM_MASK_SKULL
    PLAYER_IA_MASK_SPOOKY,         // ITEM_MASK_SPOOKY
    PLAYER_IA_MASK_BUNNY_HOOD,     // ITEM_MASK_BUNNY_HOOD
    PLAYER_IA_MASK_GORON,          // ITEM_MASK_GORON
    PLAYER_IA_MASK_ZORA,           // ITEM_MASK_ZORA
    PLAYER_IA_MASK_GERUDO,         // ITEM_MASK_GERUDO
    PLAYER_IA_MASK_TRUTH,          // ITEM_MASK_TRUTH
    PLAYER_IA_SWORD_MASTER,        // ITEM_SOLD_OUT
    PLAYER_IA_POCKET_EGG,          // ITEM_POCKET_EGG
    PLAYER_IA_POCKET_CUCCO,        // ITEM_POCKET_CUCCO
    PLAYER_IA_COJIRO,              // ITEM_COJIRO
    PLAYER_IA_ODD_MUSHROOM,        // ITEM_ODD_MUSHROOM
    PLAYER_IA_ODD_POTION,          // ITEM_ODD_POTION
    PLAYER_IA_POACHERS_SAW,        // ITEM_POACHERS_SAW
    PLAYER_IA_BROKEN_GORONS_SWORD, // ITEM_BROKEN_GORONS_SWORD
    PLAYER_IA_PRESCRIPTION,        // ITEM_PRESCRIPTION
    PLAYER_IA_FROG,                // ITEM_EYEBALL_FROG
    PLAYER_IA_EYEDROPS,            // ITEM_EYE_DROPS
    PLAYER_IA_CLAIM_CHECK,         // ITEM_CLAIM_CHECK
    PLAYER_IA_BOW_FIRE,            // ITEM_BOW_FIRE
    PLAYER_IA_BOW_ICE,             // ITEM_BOW_ICE
    PLAYER_IA_BOW_LIGHT,           // ITEM_BOW_LIGHT
    PLAYER_IA_SWORD_KOKIRI,        // ITEM_SWORD_KOKIRI
    PLAYER_IA_SWORD_MASTER,        // ITEM_SWORD_MASTER
    PLAYER_IA_SWORD_BIGGORON,      // ITEM_SWORD_BIGGORON
};

static s32 (*sItemActionUpdateFuncs[])(Player* this, PlayState* play) = {
    func_8083485C,                 // PLAYER_IA_NONE
    func_8083485C,                 // PLAYER_IA_SWORD_CS
    func_8083485C,                 // PLAYER_IA_FISHING_POLE
    Player_UpperAction_Sword,      // PLAYER_IA_SWORD_MASTER
    Player_UpperAction_Sword,      // PLAYER_IA_SWORD_KOKIRI
    Player_UpperAction_Sword,      // PLAYER_IA_SWORD_BIGGORON
    func_8083485C,                 // PLAYER_IA_DEKU_STICK
    func_8083485C,                 // PLAYER_IA_HAMMER
    func_8083501C,                 // PLAYER_IA_BOW
    func_8083501C,                 // PLAYER_IA_BOW_FIRE
    func_8083501C,                 // PLAYER_IA_BOW_ICE
    func_8083501C,                 // PLAYER_IA_BOW_LIGHT
    func_8083501C,                 // PLAYER_IA_BOW_0C
    func_8083501C,                 // PLAYER_IA_BOW_0D
    func_8083501C,                 // PLAYER_IA_BOW_0E
    func_8083501C,                 // PLAYER_IA_SLINGSHOT
    func_8083501C,                 // PLAYER_IA_HOOKSHOT
    func_8083501C,                 // PLAYER_IA_LONGSHOT
    Player_UpperAction_CarryActor, // PLAYER_IA_BOMB
    Player_UpperAction_CarryActor, // PLAYER_IA_BOMBCHU
    func_80835800,                 // PLAYER_IA_BOOMERANG
    func_8083485C,                 // PLAYER_IA_MAGIC_SPELL_15
    func_8083485C,                 // PLAYER_IA_MAGIC_SPELL_16
    func_8083485C,                 // PLAYER_IA_MAGIC_SPELL_17
    func_8083485C,                 // PLAYER_IA_FARORES_WIND
    func_8083485C,                 // PLAYER_IA_NAYRUS_LOVE
    func_8083485C,                 // PLAYER_IA_DINS_FIRE
    func_8083485C,                 // PLAYER_IA_DEKU_NUT
    func_8083485C,                 // PLAYER_IA_OCARINA_FAIRY
    func_8083485C,                 // PLAYER_IA_OCARINA_OF_TIME
    func_8083485C,                 // PLAYER_IA_BOTTLE
    func_8083485C,                 // PLAYER_IA_BOTTLE_FISH
    func_8083485C,                 // PLAYER_IA_BOTTLE_FIRE
    func_8083485C,                 // PLAYER_IA_BOTTLE_BUG
    func_8083485C,                 // PLAYER_IA_BOTTLE_POE
    func_8083485C,                 // PLAYER_IA_BOTTLE_BIG_POE
    func_8083485C,                 // PLAYER_IA_BOTTLE_RUTOS_LETTER
    func_8083485C,                 // PLAYER_IA_BOTTLE_POTION_RED
    func_8083485C,                 // PLAYER_IA_BOTTLE_POTION_BLUE
    func_8083485C,                 // PLAYER_IA_BOTTLE_POTION_GREEN
    func_8083485C,                 // PLAYER_IA_BOTTLE_MILK_FULL
    func_8083485C,                 // PLAYER_IA_BOTTLE_MILK_HALF
    func_8083485C,                 // PLAYER_IA_BOTTLE_FAIRY
    func_8083485C,                 // PLAYER_IA_ZELDAS_LETTER
    func_8083485C,                 // PLAYER_IA_WEIRD_EGG
    func_8083485C,                 // PLAYER_IA_CHICKEN
    func_8083485C,                 // PLAYER_IA_MAGIC_BEAN
    func_8083485C,                 // PLAYER_IA_POCKET_EGG
    func_8083485C,                 // PLAYER_IA_POCKET_CUCCO
    func_8083485C,                 // PLAYER_IA_COJIRO
    func_8083485C,                 // PLAYER_IA_ODD_MUSHROOM
    func_8083485C,                 // PLAYER_IA_ODD_POTION
    func_8083485C,                 // PLAYER_IA_POACHERS_SAW
    func_8083485C,                 // PLAYER_IA_BROKEN_GORONS_SWORD
    func_8083485C,                 // PLAYER_IA_PRESCRIPTION
    func_8083485C,                 // PLAYER_IA_FROG
    func_8083485C,                 // PLAYER_IA_EYEDROPS
    func_8083485C,                 // PLAYER_IA_CLAIM_CHECK
    func_8083485C,                 // PLAYER_IA_MASK_KEATON
    func_8083485C,                 // PLAYER_IA_MASK_SKULL
    func_8083485C,                 // PLAYER_IA_MASK_SPOOKY
    func_8083485C,                 // PLAYER_IA_MASK_BUNNY_HOOD
    func_8083485C,                 // PLAYER_IA_MASK_GORON
    func_8083485C,                 // PLAYER_IA_MASK_ZORA
    func_8083485C,                 // PLAYER_IA_MASK_GERUDO
    func_8083485C,                 // PLAYER_IA_MASK_TRUTH
    func_8083485C,                 // PLAYER_IA_LENS_OF_TRUTH
};

static void (*sItemActionInitFuncs[])(PlayState* play, Player* this) = {
    Player_InitDefaultIA,        // PLAYER_IA_NONE
    Player_InitDefaultIA,        // PLAYER_IA_SWORD_CS
    Player_InitDefaultIA,        // PLAYER_IA_FISHING_POLE
    Player_InitDefaultIA,        // PLAYER_IA_SWORD_MASTER
    Player_InitDefaultIA,        // PLAYER_IA_SWORD_KOKIRI
    Player_InitDefaultIA,        // PLAYER_IA_SWORD_BIGGORON
    Player_InitDekuStickIA,      // PLAYER_IA_DEKU_STICK
    Player_InitHammerIA,         // PLAYER_IA_HAMMER
    Player_InitBowOrSlingshotIA, // PLAYER_IA_BOW
    Player_InitBowOrSlingshotIA, // PLAYER_IA_BOW_FIRE
    Player_InitBowOrSlingshotIA, // PLAYER_IA_BOW_ICE
    Player_InitBowOrSlingshotIA, // PLAYER_IA_BOW_LIGHT
    Player_InitBowOrSlingshotIA, // PLAYER_IA_BOW_0C
    Player_InitBowOrSlingshotIA, // PLAYER_IA_BOW_0D
    Player_InitBowOrSlingshotIA, // PLAYER_IA_BOW_0E
    Player_InitBowOrSlingshotIA, // PLAYER_IA_SLINGSHOT
    Player_InitHookshotIA,       // PLAYER_IA_HOOKSHOT
    Player_InitHookshotIA,       // PLAYER_IA_LONGSHOT
    Player_InitExplosiveIA,      // PLAYER_IA_BOMB
    Player_InitExplosiveIA,      // PLAYER_IA_BOMBCHU
    Player_InitBoomerangIA,      // PLAYER_IA_BOOMERANG
    Player_InitDefaultIA,        // PLAYER_IA_MAGIC_SPELL_15
    Player_InitDefaultIA,        // PLAYER_IA_MAGIC_SPELL_16
    Player_InitDefaultIA,        // PLAYER_IA_MAGIC_SPELL_17
    Player_InitDefaultIA,        // PLAYER_IA_FARORES_WIND
    Player_InitDefaultIA,        // PLAYER_IA_NAYRUS_LOVE
    Player_InitDefaultIA,        // PLAYER_IA_DINS_FIRE
    Player_InitDefaultIA,        // PLAYER_IA_DEKU_NUT
    Player_InitDefaultIA,        // PLAYER_IA_OCARINA_FAIRY
    Player_InitDefaultIA,        // PLAYER_IA_OCARINA_OF_TIME
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_FISH
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_FIRE
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_BUG
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_POE
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_BIG_POE
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_RUTOS_LETTER
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_POTION_RED
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_POTION_BLUE
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_POTION_GREEN
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_MILK_FULL
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_MILK_HALF
    Player_InitDefaultIA,        // PLAYER_IA_BOTTLE_FAIRY
    Player_InitDefaultIA,        // PLAYER_IA_ZELDAS_LETTER
    Player_InitDefaultIA,        // PLAYER_IA_WEIRD_EGG
    Player_InitDefaultIA,        // PLAYER_IA_CHICKEN
    Player_InitDefaultIA,        // PLAYER_IA_MAGIC_BEAN
    Player_InitDefaultIA,        // PLAYER_IA_POCKET_EGG
    Player_InitDefaultIA,        // PLAYER_IA_POCKET_CUCCO
    Player_InitDefaultIA,        // PLAYER_IA_COJIRO
    Player_InitDefaultIA,        // PLAYER_IA_ODD_MUSHROOM
    Player_InitDefaultIA,        // PLAYER_IA_ODD_POTION
    Player_InitDefaultIA,        // PLAYER_IA_POACHERS_SAW
    Player_InitDefaultIA,        // PLAYER_IA_BROKEN_GORONS_SWORD
    Player_InitDefaultIA,        // PLAYER_IA_PRESCRIPTION
    Player_InitDefaultIA,        // PLAYER_IA_FROG
    Player_InitDefaultIA,        // PLAYER_IA_EYEDROPS
    Player_InitDefaultIA,        // PLAYER_IA_CLAIM_CHECK
    Player_InitDefaultIA,        // PLAYER_IA_MASK_KEATON
    Player_InitDefaultIA,        // PLAYER_IA_MASK_SKULL
    Player_InitDefaultIA,        // PLAYER_IA_MASK_SPOOKY
    Player_InitDefaultIA,        // PLAYER_IA_MASK_BUNNY_HOOD
    Player_InitDefaultIA,        // PLAYER_IA_MASK_GORON
    Player_InitDefaultIA,        // PLAYER_IA_MASK_ZORA
    Player_InitDefaultIA,        // PLAYER_IA_MASK_GERUDO
    Player_InitDefaultIA,        // PLAYER_IA_MASK_TRUTH
    Player_InitDefaultIA,        // PLAYER_IA_LENS_OF_TRUTH
};

typedef enum ItemChangeType {
    /*  0 */ PLAYER_ITEM_CHG_0,
    /*  1 */ PLAYER_ITEM_CHG_1,
    /*  2 */ PLAYER_ITEM_CHG_2,
    /*  3 */ PLAYER_ITEM_CHG_3,
    /*  4 */ PLAYER_ITEM_CHG_4,
    /*  5 */ PLAYER_ITEM_CHG_5,
    /*  6 */ PLAYER_ITEM_CHG_6,
    /*  7 */ PLAYER_ITEM_CHG_7,
    /*  8 */ PLAYER_ITEM_CHG_8,
    /*  9 */ PLAYER_ITEM_CHG_9,
    /* 10 */ PLAYER_ITEM_CHG_10,
    /* 11 */ PLAYER_ITEM_CHG_11,
    /* 12 */ PLAYER_ITEM_CHG_12,
    /* 13 */ PLAYER_ITEM_CHG_13,
    /* 14 */ PLAYER_ITEM_CHG_MAX
} ItemChangeType;

static ItemChangeInfo sItemChangeInfo[PLAYER_ITEM_CHG_MAX] = {
    /* PLAYER_ITEM_CHG_0 */ { &gPlayerAnim_link_normal_free2free, 12 },
    /* PLAYER_ITEM_CHG_1 */ { &gPlayerAnim_link_normal_normal2fighter, 6 },
    /* PLAYER_ITEM_CHG_2 */ { &gPlayerAnim_link_hammer_normal2long, 8 },
    /* PLAYER_ITEM_CHG_3 */ { &gPlayerAnim_link_normal_normal2free, 8 },
    /* PLAYER_ITEM_CHG_4 */ { &gPlayerAnim_link_fighter_fighter2long, 8 },
    /* PLAYER_ITEM_CHG_5 */ { &gPlayerAnim_link_normal_fighter2free, 10 },
    /* PLAYER_ITEM_CHG_6 */ { &gPlayerAnim_link_hammer_long2free, 7 },
    /* PLAYER_ITEM_CHG_7 */ { &gPlayerAnim_link_hammer_long2long, 11 },
    /* PLAYER_ITEM_CHG_8 */ { &gPlayerAnim_link_normal_free2free, 12 },
    /* PLAYER_ITEM_CHG_9 */ { &gPlayerAnim_link_normal_normal2bom, 4 },
    /* PLAYER_ITEM_CHG_10 */ { &gPlayerAnim_link_normal_long2bom, 4 },
    /* PLAYER_ITEM_CHG_11 */ { &gPlayerAnim_link_normal_free2bom, 4 },
    /* PLAYER_ITEM_CHG_12 */ { &gPlayerAnim_link_anchor_anchor2fighter, 5 },
    /* PLAYER_ITEM_CHG_13 */ { &gPlayerAnim_link_normal_free2freeB, 13 },
};

// Maps the appropriate ItemChangeType based on current and next animtype.
// A negative type value means the corresponding animation should be played in reverse.
static s8 sItemChangeTypes[PLAYER_ANIMTYPE_MAX][PLAYER_ANIMTYPE_MAX] = {
    { PLAYER_ITEM_CHG_8, -PLAYER_ITEM_CHG_5, -PLAYER_ITEM_CHG_3, -PLAYER_ITEM_CHG_6, PLAYER_ITEM_CHG_8,
      PLAYER_ITEM_CHG_11 },
    { PLAYER_ITEM_CHG_5, PLAYER_ITEM_CHG_0, -PLAYER_ITEM_CHG_1, PLAYER_ITEM_CHG_4, PLAYER_ITEM_CHG_5,
      PLAYER_ITEM_CHG_9 },
    { PLAYER_ITEM_CHG_3, PLAYER_ITEM_CHG_1, PLAYER_ITEM_CHG_0, PLAYER_ITEM_CHG_2, PLAYER_ITEM_CHG_3,
      PLAYER_ITEM_CHG_9 },
    { PLAYER_ITEM_CHG_6, -PLAYER_ITEM_CHG_4, -PLAYER_ITEM_CHG_2, PLAYER_ITEM_CHG_7, PLAYER_ITEM_CHG_6,
      PLAYER_ITEM_CHG_10 },
    { PLAYER_ITEM_CHG_8, -PLAYER_ITEM_CHG_5, -PLAYER_ITEM_CHG_3, -PLAYER_ITEM_CHG_6, PLAYER_ITEM_CHG_8,
      PLAYER_ITEM_CHG_11 },
    { PLAYER_ITEM_CHG_8, -PLAYER_ITEM_CHG_5, -PLAYER_ITEM_CHG_3, -PLAYER_ITEM_CHG_6, PLAYER_ITEM_CHG_8,
      PLAYER_ITEM_CHG_11 },
};

static ExplosiveInfo sExplosiveInfos[] = {
    { ITEM_BOMB, ACTOR_EN_BOM },
    { ITEM_BOMBCHU, ACTOR_EN_BOM_CHU },
};

static struct_80854190 D_80854190[PLAYER_MWA_MAX] = {
    /* PLAYER_MWA_FORWARD_SLASH_1H */
    { &gPlayerAnim_link_fighter_normal_kiru, &gPlayerAnim_link_fighter_normal_kiru_end,
      &gPlayerAnim_link_fighter_normal_kiru_endR, 1, 4 },
    /* PLAYER_MWA_FORWARD_SLASH_2H */
    { &gPlayerAnim_link_fighter_Lnormal_kiru, &gPlayerAnim_link_fighter_Lnormal_kiru_end,
      &gPlayerAnim_link_anchor_Lnormal_kiru_endR, 1, 4 },
    /* PLAYER_MWA_FORWARD_COMBO_1H */
    { &gPlayerAnim_link_fighter_normal_kiru_finsh, &gPlayerAnim_link_fighter_normal_kiru_finsh_end,
      &gPlayerAnim_link_anchor_normal_kiru_finsh_endR, 0, 5 },
    /* PLAYER_MWA_FORWARD_COMBO_2H */
    { &gPlayerAnim_link_fighter_Lnormal_kiru_finsh, &gPlayerAnim_link_fighter_Lnormal_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lnormal_kiru_finsh_endR, 1, 7 },
    /* PLAYER_MWA_RIGHT_SLASH_1H */
    { &gPlayerAnim_link_fighter_Lside_kiru, &gPlayerAnim_link_fighter_Lside_kiru_end,
      &gPlayerAnim_link_anchor_Lside_kiru_endR, 1, 4 },
    /* PLAYER_MWA_RIGHT_SLASH_2H */
    { &gPlayerAnim_link_fighter_LLside_kiru, &gPlayerAnim_link_fighter_LLside_kiru_end,
      &gPlayerAnim_link_anchor_LLside_kiru_endL, 0, 5 },
    /* PLAYER_MWA_RIGHT_COMBO_1H */
    { &gPlayerAnim_link_fighter_Lside_kiru_finsh, &gPlayerAnim_link_fighter_Lside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lside_kiru_finsh_endR, 2, 8 },
    /* PLAYER_MWA_RIGHT_COMBO_2H */
    { &gPlayerAnim_link_fighter_LLside_kiru_finsh, &gPlayerAnim_link_fighter_LLside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_LLside_kiru_finsh_endR, 3, 8 },
    /* PLAYER_MWA_LEFT_SLASH_1H */
    { &gPlayerAnim_link_fighter_Rside_kiru, &gPlayerAnim_link_fighter_Rside_kiru_end,
      &gPlayerAnim_link_anchor_Rside_kiru_endR, 0, 4 },
    /* PLAYER_MWA_LEFT_SLASH_2H */
    { &gPlayerAnim_link_fighter_LRside_kiru, &gPlayerAnim_link_fighter_LRside_kiru_end,
      &gPlayerAnim_link_anchor_LRside_kiru_endR, 0, 5 },
    /* PLAYER_MWA_LEFT_COMBO_1H */
    { &gPlayerAnim_link_fighter_Rside_kiru_finsh, &gPlayerAnim_link_fighter_Rside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Rside_kiru_finsh_endR, 0, 6 },
    /* PLAYER_MWA_LEFT_COMBO_2H */
    { &gPlayerAnim_link_fighter_LRside_kiru_finsh, &gPlayerAnim_link_fighter_LRside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_LRside_kiru_finsh_endL, 1, 5 },
    /* PLAYER_MWA_STAB_1H */
    { &gPlayerAnim_link_fighter_pierce_kiru, &gPlayerAnim_link_fighter_pierce_kiru_end,
      &gPlayerAnim_link_anchor_pierce_kiru_endR, 0, 3 },
    /* PLAYER_MWA_STAB_2H */
    { &gPlayerAnim_link_fighter_Lpierce_kiru, &gPlayerAnim_link_fighter_Lpierce_kiru_end,
      &gPlayerAnim_link_anchor_Lpierce_kiru_endL, 0, 3 },
    /* PLAYER_MWA_STAB_COMBO_1H */
    { &gPlayerAnim_link_fighter_pierce_kiru_finsh, &gPlayerAnim_link_fighter_pierce_kiru_finsh_end,
      &gPlayerAnim_link_anchor_pierce_kiru_finsh_endR, 1, 9 },
    /* PLAYER_MWA_STAB_COMBO_2H */
    { &gPlayerAnim_link_fighter_Lpierce_kiru_finsh, &gPlayerAnim_link_fighter_Lpierce_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lpierce_kiru_finsh_endR, 1, 8 },
    /* PLAYER_MWA_FLIPSLASH_START */
    { &gPlayerAnim_link_fighter_jump_rollkiru, &gPlayerAnim_link_fighter_jump_kiru_finsh,
      &gPlayerAnim_link_fighter_jump_kiru_finsh, 1, 10 },
    /* PLAYER_MWA_JUMPSLASH_START */
    { &gPlayerAnim_link_fighter_Lpower_jump_kiru, &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit,
      &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit, 1, 11 },
    /* PLAYER_MWA_FLIPSLASH_FINISH */
    { &gPlayerAnim_link_fighter_jump_kiru_finsh, &gPlayerAnim_link_fighter_jump_kiru_finsh_end,
      &gPlayerAnim_link_fighter_jump_kiru_finsh_end, 1, 2 },
    /* PLAYER_MWA_JUMPSLASH_FINISH */
    { &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit, &gPlayerAnim_link_fighter_Lpower_jump_kiru_end,
      &gPlayerAnim_link_fighter_Lpower_jump_kiru_end, 1, 2 },
    /* PLAYER_MWA_BACKSLASH_RIGHT */
    { &gPlayerAnim_link_fighter_turn_kiruR, &gPlayerAnim_link_fighter_turn_kiruR_end,
      &gPlayerAnim_link_fighter_turn_kiruR_end, 1, 5 },
    /* PLAYER_MWA_BACKSLASH_LEFT */
    { &gPlayerAnim_link_fighter_turn_kiruL, &gPlayerAnim_link_fighter_turn_kiruL_end,
      &gPlayerAnim_link_fighter_turn_kiruL_end, 1, 4 },
    /* PLAYER_MWA_HAMMER_FORWARD */
    { &gPlayerAnim_link_hammer_hit, &gPlayerAnim_link_hammer_hit_end, &gPlayerAnim_link_hammer_hit_endR, 3, 10 },
    /* PLAYER_MWA_HAMMER_SIDE */
    { &gPlayerAnim_link_hammer_side_hit, &gPlayerAnim_link_hammer_side_hit_end, &gPlayerAnim_link_hammer_side_hit_endR,
      2, 11 },
    /* PLAYER_MWA_SPIN_ATTACK_1H */
    { &gPlayerAnim_link_fighter_rolling_kiru, &gPlayerAnim_link_fighter_rolling_kiru_end,
      &gPlayerAnim_link_anchor_rolling_kiru_endR, 0, 12 },
    /* PLAYER_MWA_SPIN_ATTACK_2H */
    { &gPlayerAnim_link_fighter_Lrolling_kiru, &gPlayerAnim_link_fighter_Lrolling_kiru_end,
      &gPlayerAnim_link_anchor_Lrolling_kiru_endR, 0, 15 },
    /* PLAYER_MWA_BIG_SPIN_1H */
    { &gPlayerAnim_link_fighter_Wrolling_kiru, &gPlayerAnim_link_fighter_Wrolling_kiru_end,
      &gPlayerAnim_link_anchor_rolling_kiru_endR, 0, 16 },
    /* PLAYER_MWA_BIG_SPIN_2H */
    { &gPlayerAnim_link_fighter_Wrolling_kiru, &gPlayerAnim_link_fighter_Wrolling_kiru_end,
      &gPlayerAnim_link_anchor_Lrolling_kiru_endR, 0, 16 },
};

static LinkAnimationHeader* D_80854350[] = {
    &gPlayerAnim_link_fighter_power_kiru_start,
    &gPlayerAnim_link_fighter_Lpower_kiru_start,
};

static LinkAnimationHeader* D_80854358[] = {
    &gPlayerAnim_link_fighter_power_kiru_startL,
    &gPlayerAnim_link_fighter_Lpower_kiru_start,
};

static LinkAnimationHeader* D_80854360[] = {
    &gPlayerAnim_link_fighter_power_kiru_wait,
    &gPlayerAnim_link_fighter_Lpower_kiru_wait,
};

static LinkAnimationHeader* D_80854368[] = {
    &gPlayerAnim_link_fighter_power_kiru_wait_end,
    &gPlayerAnim_link_fighter_Lpower_kiru_wait_end,
};

static LinkAnimationHeader* D_80854370[] = {
    &gPlayerAnim_link_fighter_power_kiru_walk,
    &gPlayerAnim_link_fighter_Lpower_kiru_walk,
};

static LinkAnimationHeader* D_80854378[] = {
    &gPlayerAnim_link_fighter_power_kiru_side_walk,
    &gPlayerAnim_link_fighter_Lpower_kiru_side_walk,
};

static u8 D_80854380[2] = { PLAYER_MWA_SPIN_ATTACK_1H, PLAYER_MWA_SPIN_ATTACK_2H };
static u8 D_80854384[2] = { PLAYER_MWA_BIG_SPIN_1H, PLAYER_MWA_BIG_SPIN_2H };

static u16 sItemButtons[] = { BTN_B, BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT, BTN_DUP, BTN_DDOWN, BTN_DLEFT, BTN_DRIGHT };

static u8 sMagicSpellCosts[] = { 12, 24, 24, 12, 24, 12 };

static u16 D_80854398[] = { NA_SE_IT_BOW_DRAW, NA_SE_IT_SLING_DRAW, NA_SE_IT_HOOKSHOT_READY };

static u8 sMagicArrowCosts[] = { 4, 4, 8 };

static LinkAnimationHeader* D_808543A4[] = {
    &gPlayerAnim_link_anchor_waitR2defense,
    &gPlayerAnim_link_anchor_waitR2defense_long,
};

static LinkAnimationHeader* D_808543AC[] = {
    &gPlayerAnim_link_anchor_waitL2defense,
    &gPlayerAnim_link_anchor_waitL2defense_long,
};

static LinkAnimationHeader* D_808543B4[] = {
    &gPlayerAnim_link_anchor_defense_hit,
    &gPlayerAnim_link_anchor_defense_long_hitL,
};

static LinkAnimationHeader* D_808543BC[] = {
    &gPlayerAnim_link_anchor_defense_hit,
    &gPlayerAnim_link_anchor_defense_long_hitR,
};

static LinkAnimationHeader* D_808543C4[] = {
    &gPlayerAnim_link_normal_defense_hit,
    &gPlayerAnim_link_fighter_defense_long_hit,
};

static LinkAnimationHeader* D_808543CC[] = {
    &gPlayerAnim_link_bow_walk2ready,
    &gPlayerAnim_link_hook_walk2ready,
};

static LinkAnimationHeader* D_808543D4[] = {
    &gPlayerAnim_link_bow_bow_wait,
    &gPlayerAnim_link_hook_wait,
};

BAD_RETURN(s32) Player_ZeroSpeedXZ(Player* this) {
    this->actor.speedXZ = 0.0f;
    this->linearVelocity = 0.0f;
}

// return type can't be void due to regalloc in func_8083F72C
BAD_RETURN(s32) func_80832224(Player* this) {
    Player_ZeroSpeedXZ(this);
    this->unk_6AD = 0;
}

s32 Player_IsTalking(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_PLAYER_TALKED_TO);
}

void Player_AnimPlayOnce(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayOnce(play, &this->skelAnime, anim);
}

void Player_AnimPlayLoop(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayLoop(play, &this->skelAnime, anim);
}

void Player_AnimPlayLoopAdjusted(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayLoopSetSpeed(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_AnimPlayOnceAdjusted(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_ApplyYawFromAnim(Player* this) {
    this->actor.shape.rot.y += this->skelAnime.jointTable[1].y;
    this->skelAnime.jointTable[1].y = 0;
}

void func_80832318(Player* this) {
    this->stateFlags2 &= ~PLAYER_STATE2_SPIN_ATTACKING;
    this->meleeWeaponState = 0;
    this->meleeWeaponInfo[0].active = this->meleeWeaponInfo[1].active = this->meleeWeaponInfo[2].active = 0;
}

void func_80832340(PlayState* play, Player* this) {
    Camera* subCam;

    if (this->subCamId != SUBCAM_NONE) {
        subCam = play->cameraPtrs[this->subCamId];
        if ((subCam != NULL) && (subCam->csId == 1100)) {
            OnePointCutscene_EndCutscene(play, this->subCamId);
            this->subCamId = SUBCAM_NONE;
        }
    }

    this->stateFlags2 &= ~(PLAYER_STATE2_UNDERWATER | PLAYER_STATE2_DIVING);
}

void Player_DetachHeldActor(PlayState* play, Player* this) {
    Actor* heldActor = this->heldActor;

    if ((heldActor != NULL) && !Player_HoldsHookshot(this)) {
        this->actor.child = NULL;
        this->heldActor = NULL;
        this->interactRangeActor = NULL;
        heldActor->parent = NULL;
        this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
    }

    if (Player_GetExplosiveHeld(this) >= 0) {
        Player_InitItemAction(play, this, PLAYER_IA_NONE);
        this->heldItemId = ITEM_NONE_FE;
    }
}

void func_80832440(PlayState* play, Player* this) {
    if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->heldActor == NULL)) {
        if (this->interactRangeActor != NULL) {
            if (this->getItemId == GI_NONE) {
                this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
                this->interactRangeActor = NULL;
            }
        } else {
            this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
        }
    }

    func_80832318(this);
    this->unk_6AD = 0;

    func_80832340(play, this);
    func_8005B1A4(Play_GetCamera(play, 0));

    this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_FIRST_PERSON | PLAYER_STATE1_CLIMBING_LADDER);
    this->stateFlags2 &= ~(PLAYER_STATE2_MOVING_DYNAPOLY | PLAYER_STATE2_GRABBED_BY_ENEMY | PLAYER_STATE2_CRAWLING);

    this->actor.shape.rot.x = 0;
    this->actor.shape.yOffset = 0.0f;

    this->unk_845 = this->unk_844 = 0;
}

/**
 * Puts away item currently in hand, if holding any.
 * @return  true if an item needs to be put away, false if not.
 */
s32 Player_PutAwayHeldItem(PlayState* play, Player* this) {
    if (this->heldItemAction >= PLAYER_IA_FISHING_POLE) {
        Player_UseItem(play, this, ITEM_NONE);
        return true;
    } else {
        return false;
    }
}

void func_80832564(PlayState* play, Player* this) {
    func_80832440(play, this);
    Player_DetachHeldActor(play, this);
}

s32 func_80832594(Player* this, s32 arg1, s32 arg2) {
    s16 controlStickAngleDiff = this->prevControlStickAngle - sControlStickAngle;

    this->av2.actionVar2 +=
        arg1 + (s16)(ABS(controlStickAngleDiff) * fabsf(sControlStickMagnitude) * 2.5415802156203426e-06f);

    if (CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B)) {
        this->av2.actionVar2 += 5;
    }

    return this->av2.actionVar2 > arg2;
}

void func_80832630(PlayState* play) {
    if (play->actorCtx.freezeFlashTimer == 0) {
        play->actorCtx.freezeFlashTimer = 1;
    }
}

void Player_RequestRumble(Player* this, s32 sourceStrength, s32 duration, s32 decreaseRate, s32 distSq) {
    if (this->actor.category == ACTORCAT_PLAYER) {
        func_800AA000(distSq, sourceStrength, duration, decreaseRate);
    }
}

void Player_PlayVoiceSfx(Player* this, u16 sfxId) {
    if (this->actor.category == ACTORCAT_PLAYER) {
        Player_PlaySfx(this, sfxId + this->ageProperties->unk_92);
    } else {
        func_800F4190(&this->actor.projectedPos, sfxId);
    }
}

void func_808326F0(Player* this) {
    u16* entry = &D_8085361C[0];
    s32 i;

    for (i = 0; i < 4; i++) {
        Audio_StopSfxById((u16)(*entry + this->ageProperties->unk_92));
        entry++;
    }
}

u16 Player_ApplyFloorSfxOffset(Player* this, u16 sfxId) {
    return sfxId + this->floorSfxOffset;
}

void Player_PlayFloorSfx(Player* this, u16 sfxId) {
    Player_PlaySfx(this, Player_ApplyFloorSfxOffset(this, sfxId));
}

u16 Player_ApplyFloorAndAgeSfxOffsets(Player* this, u16 sfxId) {
    return sfxId + this->floorSfxOffset + this->ageProperties->unk_94;
}

void Player_PlayFloorSfxByAge(Player* this, u16 sfxId) {
    Player_PlaySfx(this, Player_ApplyFloorAndAgeSfxOffsets(this, sfxId));
}

void Player_PlaySteppingSfx(Player* this, f32 pitchAdjustment) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_WALK_HEAVYBOOTS;
    } else {
        sfxId = Player_ApplyFloorAndAgeSfxOffsets(this, NA_SE_PL_WALK_GROUND);
    }

    func_800F4010(&this->actor.projectedPos, sfxId, pitchAdjustment);
    // Gameplay stats: Count footsteps
    // Only count while game isn't complete and don't count Link's idle animations or crawling in crawlspaces
    if (!gSaveContext.sohStats.gameComplete && !(this->stateFlags2 & PLAYER_STATE2_IDLE_FIDGET) &&
        !(this->stateFlags2 & PLAYER_STATE2_CRAWLING)) {
        gSaveContext.sohStats.count[COUNT_STEPS]++;
    }
}

void Player_PlayJumpingSfx(Player* this) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_JUMP_HEAVYBOOTS;
    } else {
        sfxId = Player_ApplyFloorAndAgeSfxOffsets(this, NA_SE_PL_JUMP);
    }

    Player_PlaySfx(this, sfxId);
}

void Player_PlayLandingSfx(Player* this) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_LAND_HEAVYBOOTS;
    } else {
        sfxId = Player_ApplyFloorAndAgeSfxOffsets(this, NA_SE_PL_LAND);
    }

    Player_PlaySfx(this, sfxId);
}

void func_808328EC(Player* this, u16 sfxId) {
    Player_PlaySfx(this, sfxId);
    this->stateFlags2 |= PLAYER_STATE2_FOOTSTEP;
}

/**
 * Process a list of `AnimSfx` entries.
 * An `AnimSfx` entry contains a sound effect to play, a frame number that indicates
 * when during an animation it should play, and a type value that indicates how it should be played back.
 *
 * The list will stop being processed after an entry that has a negative value for the `data` field.
 *
 * Some types do not make use of `sfxId`, the SFX function called will pick a sound on its own.
 * The `sfxId` field is not used in this case and can be any value, but 0 is typically used.
 *
 * @param entry  A pointer to the first entry of an `AnimSfx` list.
 */
void Player_ProcessAnimSfxList(Player* this, AnimSfxEntry* entry) {
    s32 cont;
    s32 pad;

    do {
        s32 absData = ABS(entry->data);
        s32 type = ANIMSFX_GET_TYPE(absData);

        if (LinkAnimation_OnFrame(&this->skelAnime, fabsf(ANIMSFX_GET_FRAME(absData)))) {
            if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_GENERAL)) {
                Player_PlaySfx(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR)) {
                Player_PlayFloorSfx(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR_BY_AGE)) {
                Player_PlayFloorSfxByAge(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_VOICE)) {
                Player_PlayVoiceSfx(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_LANDING)) {
                Player_PlayLandingSfx(this);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_RUNNING)) {
                Player_PlaySteppingSfx(this, 6.0f);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_JUMPING)) {
                Player_PlayJumpingSfx(this);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_WALKING)) {
                Player_PlaySteppingSfx(this, 0.0f);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_UNKNOWN)) {
                func_800F4010(&this->actor.projectedPos,
                              NA_SE_PL_WALK_LADDER + this->ageProperties->unk_94, 0.0f);
            }
        }

        cont = (entry->data >= 0); // stop processing if `data` is negative
        entry++;
    } while (cont);
}

void Player_AnimChangeOnceMorph(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 0.0f, Animation_GetLastFrame(anim), ANIMMODE_ONCE, -6.0f);
}

void Player_AnimChangeOnceMorphAdjusted(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, Animation_GetLastFrame(anim),
                         ANIMMODE_ONCE, -6.0f);
}

void Player_AnimChangeLoopMorph(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void Player_AnimChangeFreeze(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
}

void Player_AnimChangeLoopSlowMorph(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -16.0f);
}

s32 func_80832CB0(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimPlayLoop(play, this, anim);
        return 1;
    } else {
        return 0;
    }
}

void Player_ResetAnimMovement(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;
    this->skelAnime.prevRot = this->actor.shape.rot.y;
}

void Player_ResetAnimMovementScaledByAge(Player* this) {
    Player_ResetAnimMovement(this);

    this->skelAnime.prevTransl.x *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.y *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.z *= this->ageProperties->unk_08;
}

void Player_ZeroRootLimbYaw(Player* this) {
    this->skelAnime.jointTable[1].y = 0;
}

/**
 * Finishes "AnimMovement" by resetting various aspects of Player's SkelAnime structure.
 *
 * This function is called in Player_SetupAction so it will run on every action change, but
 * it can also be called within action functions to change animations in the middle of an action.
 */
void Player_FinishAnimMovement(Player* this) {
    if (this->skelAnime.moveFlags != 0) {
        Player_ApplyYawFromAnim(this);

        this->skelAnime.jointTable[0].x = this->skelAnime.baseTransl.x;
        this->skelAnime.jointTable[0].z = this->skelAnime.baseTransl.z;

        if (this->skelAnime.moveFlags & 8) {
            if (this->skelAnime.moveFlags & 2) {
                this->skelAnime.jointTable[0].y = this->skelAnime.prevTransl.y;
            }
        } else {
            this->skelAnime.jointTable[0].y = this->skelAnime.baseTransl.y;
        }

        Player_ResetAnimMovement(this);

        this->skelAnime.moveFlags = 0;
    }
}

/**
 * This is a reimplementation of `AnimTask_ActorMovement`.
 *
 * This achieves the same goal as `AnimTask_ActorMovement`but it adds
 * the ability to scale the resulting movement according to age.
 *
 * When using the AnimTask variant, age specific scaling can only be applied visually
 * to the root bone position and does not affect world position.
 */
void Player_ApplyAnimMovementScaledByAge(Player* this, s32 movementFlags) {
    Vec3f diff;

    this->skelAnime.moveFlags = movementFlags;
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;

    SkelAnime_UpdateTranslation(&this->skelAnime, &diff, this->actor.shape.rot.y);

    if (movementFlags & 1) {
        if (!LINK_IS_ADULT) {
            diff.x *= 0.64f;
            diff.z *= 0.64f;
        }

        this->actor.world.pos.x += diff.x * this->actor.scale.x;
        this->actor.world.pos.z += diff.z * this->actor.scale.z;
    }

    if (movementFlags & 2) {
        if (!(movementFlags & 4)) {
            diff.y *= this->ageProperties->unk_08;
        }

        this->actor.world.pos.y += diff.y * this->actor.scale.y;
    }

    Player_ApplyYawFromAnim(this);
}

#define PLAYER_ANIM_MOVEMENT_RESET (1 << 8)
#define PLAYER_ANIM_MOVEMENT_RESET_BY_AGE (1 << 9)

/**
 * Starts "AnimMovement" so that Player will move according to the translation and rotation specified
 * by the animation that is playing.
 *
 * The `flags` field can be any of the SkelAnime system's `ANIM_FLAG_` flags, as well as Player-specific
 * `PLAYER_ANIM_MOVEMENT_` flags.
 *
 * For AnimMovement features to be enabled, it is usually required to pass `ANIM_FLAG_ENABLE_MOVEMENT`
 * as one of the flags, but there are a few niche cases where it can be desirable to omit it
 * (for example to use `ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT` without any actual AnimMovement).
 *
 * Note: AnimMovement is always disabled during every action change.
 *       This means the order that functions are called matters.
 *       `Player_StartAnimMovement` must be called *after* a call to `Player_SetupAction`.
 */
void Player_StartAnimMovement(PlayState* play, Player* this, s32 flags) {
    if (flags & PLAYER_ANIM_MOVEMENT_RESET_BY_AGE) {
        Player_ResetAnimMovementScaledByAge(this);
    } else if ((flags & PLAYER_ANIM_MOVEMENT_RESET) || (this->skelAnime.moveFlags != 0)) {
        // If AnimMovement is already in use when this function is called and
        // `PLAYER_ANIM_MOVEMENT_RESET_BY_AGE` is not set, then this case will be used.
        Player_ResetAnimMovement(this);
    } else {
        // Default case used when AnimMovement was not enabled previously.
        // This sets prevTransl and prevRot to Players current translation and yaw.
        this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
        this->skelAnime.prevRot = this->actor.shape.rot.y;
    }

    // Remove Player specific flags by masking the lower byte before setting to `skelAnime.movementFlags`
    this->skelAnime.moveFlags = flags /*&& 0xFF*/;

    Player_ZeroSpeedXZ(this);
    AnimationContext_DisableQueue(play);
}

// TODO: Change all of these wrapper functions below to use "AnimMovement" instead of "AnimReplace"
void Player_AnimReplacePlayOnceSetSpeed(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags,
                                        f32 playbackSpeed) {
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, playbackSpeed);
    Player_StartAnimMovement(play, this, flags);
}

void Player_AnimReplacePlayOnce(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags) {
    Player_AnimReplacePlayOnceSetSpeed(play, this, anim, flags, 1.0f);
}

void Player_AnimReplacePlayOnceAdjusted(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags) {
    Player_AnimReplacePlayOnceSetSpeed(play, this, anim, flags, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_AnimReplaceNormalPlayOnceAdjusted(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Player_AnimReplacePlayOnceAdjusted(play, this, anim, 0x1C);
}

void Player_AnimReplacePlayLoopSetSpeed(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags,
                                        f32 playbackSpeed) {
    LinkAnimation_PlayLoopSetSpeed(play, &this->skelAnime, anim, playbackSpeed);
    Player_StartAnimMovement(play, this, flags);
}

void Player_AnimReplacePlayLoop(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags) {
    Player_AnimReplacePlayLoopSetSpeed(play, this, anim, flags, 1.0f);
}

void Player_AnimReplacePlayLoopAdjusted(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags) {
    Player_AnimReplacePlayLoopSetSpeed(play, this, anim, flags, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_AnimReplaceNormalPlayLoopAdjusted(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Player_AnimReplacePlayLoopAdjusted(play, this, anim, 0x1C);
}

void Player_ProcessControlStick(PlayState* play, Player* this) {
    s8 spinAngle;
    s8 direction;

    this->prevControlStickMagnitude = sControlStickMagnitude;
    this->prevControlStickAngle = sControlStickAngle;

    func_80077D10(&sControlStickMagnitude, &sControlStickAngle, sControlInput);

    sControlStickWorldYaw = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play)) + sControlStickAngle;

    this->controlStickDataIndex = (this->controlStickDataIndex + 1) % 4;

    if (sControlStickMagnitude < 55.0f) {
        direction = PLAYER_STICK_DIR_NONE;
        spinAngle = -1;
    } else {
        spinAngle = (u16)(sControlStickAngle + 0x2000) >> 9;
        direction = (u16)((s16)(sControlStickWorldYaw - this->actor.shape.rot.y) + 0x2000) >> 14;
    }

    this->controlStickSpinAngles[this->controlStickDataIndex] = spinAngle;
    this->controlStickDirections[this->controlStickDataIndex] = direction;
}

void func_8083328C(PlayState* play, Player* this, LinkAnimationHeader* linkAnim) {
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, linkAnim, sWaterSpeedFactor);
}

int func_808332B8(Player* this) {
    return (this->stateFlags1 & PLAYER_STATE1_IN_WATER) && (this->currentBoots != PLAYER_BOOTS_IRON);
}

s32 func_808332E4(Player* this) {
    return (this->stateFlags1 & PLAYER_STATE1_USING_BOOMERANG);
}

void func_808332F4(Player* this, PlayState* play) {
    GetItemEntry giEntry;
    if (this->getItemEntry.objectId == OBJECT_INVALID || (this->getItemId != this->getItemEntry.getItemId)) {
        giEntry = ItemTable_Retrieve(this->getItemId);
    } else {
        giEntry = this->getItemEntry;
    }

    this->unk_862 = ABS(giEntry.gi);
}

/**
 * Get the appropriate Idle animation based on current `modelAnimType`.
 * This is the default idle animation.
 *
 * For fidget idle animations (which can for example, change based on environment)
 * see `sFidgetAnimations`.
 */
LinkAnimationHeader* Player_GetIdleAnim(Player* this) {
    return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType);
}

/**
 * Return values for `Player_CheckForIdleAnim`
 */
#define IDLE_ANIM_DEFAULT -1
#define IDLE_ANIM_NONE 0
// Fidget idle anims are returned by index. See `sFidgetAnimations` and `FidgetType`.

/**
 * Checks if the current animation is an idle animation.
 * If the current animation is a fidget animation, the index into
 * `sFidgetAnimations` is returned (plus one).
 * If the current animation is a default idle animation, -1 is returned.
 * Lastly if the current animation is neither of these, 0 is returned.
 */
s32 Player_CheckForIdleAnim(Player* this) {
    if (Player_GetIdleAnim(this) != this->skelAnime.animation) {
        LinkAnimationHeader** fidgetAnim;
        s32 i;

        for (i = 0, fidgetAnim = &sFidgetAnimations[0][0]; i < ARRAY_COUNT_2D(sFidgetAnimations); i++, fidgetAnim++) {
            if (this->skelAnime.animation == *fidgetAnim) {
                return i + 1;
            }
        }

        return IDLE_ANIM_NONE;
    }

    return IDLE_ANIM_DEFAULT;
}

void Player_ProcessFidgetAnimSfxList(Player* this, s32 fidgetAnimIndex) {
    if (sFidgetAnimSfxTypes[fidgetAnimIndex] != FIDGET_ANIMSFX_NONE) {
        Player_ProcessAnimSfxList(this, sFidgetAnimSfxLists[sFidgetAnimSfxTypes[fidgetAnimIndex] - 1]);
    }
}

LinkAnimationHeader* func_80833438(Player* this) {
    if (this->unk_890 != 0) {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_damage_run, this->modelAnimType);
    } else if (!(this->stateFlags1 & (PLAYER_STATE1_IN_WATER | PLAYER_STATE1_IN_CUTSCENE)) &&
               (this->currentBoots == PLAYER_BOOTS_IRON)) {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_heavy_run, this->modelAnimType);
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_run, this->modelAnimType);
    }
}

int func_808334B4(Player* this) {
    return func_808332E4(this) && (this->unk_834 != 0);
}

LinkAnimationHeader* func_808334E4(Player* this) {
    if (func_808334B4(this)) {
        return &gPlayerAnim_link_boom_throw_waitR;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_waitR, this->modelAnimType);
    }
}

LinkAnimationHeader* func_80833528(Player* this) {
    if (func_808334B4(this)) {
        return &gPlayerAnim_link_boom_throw_waitL;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_waitL, this->modelAnimType);
    }
}

LinkAnimationHeader* func_8083356C(Player* this) {
    if (func_8002DD78(this)) {
        return &gPlayerAnim_link_bow_side_walk;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walk, this->modelAnimType);
    }
}

LinkAnimationHeader* func_808335B0(Player* this) {
    if (func_808334B4(this)) {
        return &gPlayerAnim_link_boom_throw_side_walkR;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkR, this->modelAnimType);
    }
}

LinkAnimationHeader* func_808335F4(Player* this) {
    if (func_808334B4(this)) {
        return &gPlayerAnim_link_boom_throw_side_walkL;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkL, this->modelAnimType);
    }
}

void Player_SetUpperActionFunc(Player* this, UpperActionFunc upperActionFunc) {
    this->upperActionFunc = upperActionFunc;
    this->unk_836 = 0;
    this->upperAnimInterpWeight = 0.0f;
    func_808326F0(this);
}

#define Player_GetMeleeWeaponHeld2 Player_GetMeleeWeaponHeld

void Player_InitItemActionWithAnim(PlayState* play, Player* this, s8 itemAction) {
    LinkAnimationHeader* current = this->skelAnime.animation;
    LinkAnimationHeader** iter = &D_80853914[0][this->modelAnimType];
    u32 animGroup;

    // This is redundant, the same two flags get unset in
    // `Player_InitItemAction` called below.
    this->stateFlags1 &= ~(PLAYER_STATE1_ITEM_IN_HAND | PLAYER_STATE1_USING_BOOMERANG);

    for (animGroup = 0; animGroup < PLAYER_ANIMGROUP_MAX; animGroup++) {
        if (current == *iter) {
            break;
        }
        iter += PLAYER_ANIMTYPE_MAX;
    }

    Player_InitItemAction(play, this, itemAction);

    if (animGroup < PLAYER_ANIMGROUP_MAX) {
        this->skelAnime.animation = GET_PLAYER_ANIM(animGroup, this->modelAnimType);
    }
}

s8 Player_ItemToItemAction(s32 item) {
    if (item >= ITEM_NONE_FE) {
        return PLAYER_IA_NONE;
    } else if (item == ITEM_LAST_USED) {
        return PLAYER_IA_SWORD_CS;
    } else if (item == ITEM_FISHING_POLE) {
        return PLAYER_IA_FISHING_POLE;
    // #region SOH [Enhancement] Added to prevent crashes with assignable equipment
    } else if (item >= ITEM_TUNIC_KOKIRI && item <= ITEM_BOOTS_HOVER) {
        return PLAYER_IA_NONE;
    // #endregion
    } else {
        return sItemActions[item];
    }
}

void Player_InitDefaultIA(PlayState* play, Player* this) {
}

#define Player_HoldsTwoHandedWeapon2 Player_HoldsTwoHandedWeapon

void Player_InitDekuStickIA(PlayState* play, Player* this) {
    this->unk_85C = 1.0f;
}

void Player_InitHammerIA(PlayState* play, Player* this) {
}

void Player_InitBowOrSlingshotIA(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_ITEM_IN_HAND;

    if (this->heldItemAction != PLAYER_IA_SLINGSHOT) {
        this->unk_860 = -1;
    } else {
        this->unk_860 = -2;
    }
}

void Player_InitExplosiveIA(PlayState* play, Player* this) {
    s32 explosiveType;
    ExplosiveInfo* explosiveInfo;
    Actor* spawnedActor;

    if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        Player_PutAwayHeldItem(play, this);
        return;
    }

    explosiveType = Player_GetExplosiveHeld(this);
    explosiveInfo = &sExplosiveInfos[explosiveType];

    spawnedActor =
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, explosiveInfo->actorId, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0);
    if (spawnedActor != NULL) {
        if ((explosiveType != 0) && (play->bombchuBowlingStatus != 0)) {
            if (!CVarGetInteger(CVAR_CHEAT("InfiniteAmmo"), 0)) {
                play->bombchuBowlingStatus--;
            }
            if (play->bombchuBowlingStatus == 0) {
                play->bombchuBowlingStatus = -1;
            }
        } else {
            Inventory_ChangeAmmo(explosiveInfo->itemId, -1);
        }

        this->interactRangeActor = spawnedActor;
        this->heldActor = spawnedActor;
        this->getItemId = GI_NONE;
        this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
        this->unk_3BC.y = spawnedActor->shape.rot.y - this->actor.shape.rot.y;
        this->stateFlags1 |= PLAYER_STATE1_CARRYING_ACTOR;
    }
}

void Player_InitHookshotIA(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_ITEM_IN_HAND;
    this->unk_860 = -3;

    this->heldActor =
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_ARMS_HOOK, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0);
}

void Player_InitBoomerangIA(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_USING_BOOMERANG;
}

void Player_InitItemAction(PlayState* play, Player* this, s8 itemAction) {
    this->unk_85C = 0.0f;
    this->unk_858 = 0.0f;
    this->unk_860 = 0;

    this->heldItemAction = this->itemAction = itemAction;
    this->modelGroup = this->nextModelGroup;

    this->stateFlags1 &= ~(PLAYER_STATE1_ITEM_IN_HAND | PLAYER_STATE1_USING_BOOMERANG);

    sItemActionInitFuncs[itemAction](play, this);

    Player_SetModelGroup(this, this->modelGroup);
}

void func_80833A20(Player* this, s32 newMeleeWeaponState) {
    u16 itemSfx;
    u16 voiceSfx;

    if (this->meleeWeaponState == 0) {
        if ((this->heldItemAction == PLAYER_IA_SWORD_BIGGORON) &&
            (gSaveContext.swordHealth > 0.0f)) {
            itemSfx = NA_SE_IT_HAMMER_SWING;
        } else {
            itemSfx = NA_SE_IT_SWORD_SWING;
        }

        voiceSfx = NA_SE_VO_LI_SWORD_N;
        if (this->heldItemAction == PLAYER_IA_HAMMER) {
            itemSfx = NA_SE_IT_HAMMER_SWING;
        } else if (this->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) {
            itemSfx = 0;
            voiceSfx = NA_SE_VO_LI_SWORD_L;
        } else if (this->unk_845 >= 3) {
            itemSfx = NA_SE_IT_SWORD_SWING_HARD;
            voiceSfx = NA_SE_VO_LI_SWORD_L;
        }

        if (itemSfx != 0) {
            func_808328EC(this, itemSfx);
        }

        if (!((this->meleeWeaponAnimation >= PLAYER_MWA_FLIPSLASH_START) &&
              (this->meleeWeaponAnimation <= PLAYER_MWA_JUMPSLASH_FINISH))) {
            Player_PlayVoiceSfx(this, voiceSfx);
        }

        if (this->heldItemAction >= PLAYER_IA_SWORD_MASTER && this->heldItemAction <= PLAYER_IA_SWORD_BIGGORON) {
            gSaveContext.sohStats.count[COUNT_SWORD_SWINGS]++;
        }
    }

    this->meleeWeaponState = newMeleeWeaponState;
}

/**
 * This function checks for friendly (non-hostile) Z-Target related states.
 * For hostile related lock-on states, see `Player_UpdateHostileLockOn` and `Player_CheckHostileLockOn`.
 *
 * Note that `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS` will include all `focusActor` use cases that relate to
 * friendly actors. This function can return true when talking to an actor, for example.
 * Despite that, this function is only relevant in the context of actor lock-on, which is a subset of actor focus.
 * This is why the function name states `FriendlyLockOn` instead of `FriendlyActorFocus`.
 *
 * There is a special case that allows hostile actors to be treated as "friendly" if Player is carrying another actor
 * See relevant code in `Player_UpdateZTargeting` for more details.
 *
 * Additionally, `PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE` will be set very briefly in some conditions when
 * a lock-on is forced to release. In these niche cases, this function will apply to both friendly and hostile actors.
 * Overall, it is safe to assume that this specific state flag is not very relevant for this function's use cases.
 */
s32 Player_FriendlyLockOnOrParallel(Player* this) {
    if (this->stateFlags1 &
        (PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS | PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE)) {
        return true;
    } else {
        return false;
    }
}

/**
 * Checks the current state of `focusActor` and if it is a hostile actor (if applicable).
 * If so, sets `PLAYER_STATE1_HOSTILE_LOCK_ON` which will control Player's "battle" response to
 * hostile actors. This includes affecting how movement is handled, and enabling a "fighting" set
 * of animations.
 *
 * Note that `Player_CheckHostileLockOn` also exists to check if there is currently a hostile lock-on actor.
 * This function differs in that it first updates the flag if appropriate, then returns the same information.
 *
 * @return  true if there is currently a hostile lock-on actor, false otherwise
 */
s32 Player_UpdateHostileLockOn(Player* this) {
    if ((this->focusActor != NULL) &&
        CHECK_FLAG_ALL(this->focusActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE)) {
        this->stateFlags1 |= PLAYER_STATE1_HOSTILE_LOCK_ON;
        return true;
    } else {
        if (this->stateFlags1 & PLAYER_STATE1_HOSTILE_LOCK_ON) {
            this->stateFlags1 &= ~PLAYER_STATE1_HOSTILE_LOCK_ON;

            // sync world and shape yaw when not moving
            if (this->linearVelocity == 0.0f) {
                this->yaw = this->actor.shape.rot.y;
            }
        }

        return false;
    }
}

/**
 * Returns true if currently Z-Targeting, false if not.
 * Z-Targeting here is a blanket term that covers both the "actor lock-on" and "parallel" states.
 *
 * This variant of the function calls `Player_CheckHostileLockOn`, which does not update the hostile
 * lock-on actor state.
 */
int Player_IsZTargeting(Player* this) {
    return Player_CheckHostileLockOn(this) || Player_FriendlyLockOnOrParallel(this);
}

/**
 * Returns true if currently Z-Targeting, false if not.
 * Z-Targeting here is a blanket term that covers both the "actor lock-on" and "parallel" states.
 *
 * This variant of the function calls `Player_UpdateHostileLockOn`, which updates the hostile
 * lock-on actor state before checking its state.
 */
int Player_IsZTargetingWithHostileUpdate(Player* this) {
    return Player_UpdateHostileLockOn(this) || Player_FriendlyLockOnOrParallel(this);
}

void func_80833C3C(Player* this) {
    this->unk_870 = this->unk_874 = 0.0f;
}

s32 Player_ItemIsInUse(Player* this, s32 item) {
    if ((item < ITEM_NONE_FE) && (Player_ItemToItemAction(item) == this->itemAction)) {
        return true;
    } else {
        return false;
    }
}

s32 Player_ItemIsItemAction(s32 item1, s32 itemAction) {
    if ((item1 < ITEM_NONE_FE) && (Player_ItemToItemAction(item1) == itemAction)) {
        return true;
    } else {
        return false;
    }
}

s32 Player_GetItemOnButton(PlayState* play, s32 index) {
    if (index >= ((CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) != 0) ? 8 : 4)) {
        return ITEM_NONE;
    } else if (play->bombchuBowlingStatus != 0) {
        return (play->bombchuBowlingStatus > 0) ? ITEM_BOMBCHU : ITEM_NONE;
    } else if (index == 0) {
        return B_BTN_ITEM;
    } else if (index == 1) {
        return C_BTN_ITEM(0);
    } else if (index == 2) {
        return C_BTN_ITEM(1);
    } else if (index == 3) {
        return C_BTN_ITEM(2);
    } else if (index == 4) {
        return DPAD_ITEM(0);
    } else if (index == 5) {
        return DPAD_ITEM(1);
    } else if (index == 6) {
        return DPAD_ITEM(2);
    } else if (index == 7) {
        return DPAD_ITEM(3);
    }
}

/**
 * Handles the high level item usage and changing process based on the B and C buttons.
 *
 * Tasks include:
 *    - Put away a mask if it is not present on any C button
 *    - Put away an item if it is not present on the B button or any C button
 *    - Use an item on the B button or any C button if the corresponding button is pressed
 *    - Keep track of the current item button being held down
 */
void Player_ProcessItemButtons(Player* this, PlayState* play) {
    s32 maskItemAction;
    s32 item;
    s32 i;

    if (this->currentMask != PLAYER_MASK_NONE && !CVarGetInteger(CVAR_ENHANCEMENT("PersistentMasks"), 0)) {
        maskItemAction = this->currentMask - 1 + PLAYER_IA_MASK_KEATON;

        bool hasOnDpad = false;
        if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) != 0) {
            for (int buttonIndex = 0; buttonIndex < 4; buttonIndex++) {
                hasOnDpad |= Player_ItemIsItemAction(DPAD_ITEM(buttonIndex), maskItemAction);
            }
        }
        if (!Player_ItemIsItemAction(C_BTN_ITEM(0), maskItemAction) &&
            !Player_ItemIsItemAction(C_BTN_ITEM(1), maskItemAction) &&
            !Player_ItemIsItemAction(C_BTN_ITEM(2), maskItemAction) && !hasOnDpad) {
            this->currentMask = PLAYER_MASK_NONE;
        }
    }

    if (!(this->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_IN_CUTSCENE)) && !func_8008F128(this)) {
        if (this->itemAction >= PLAYER_IA_FISHING_POLE) {
            bool hasOnDpad = false;
            if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) != 0) {
                for (int buttonIndex = 0; buttonIndex < 4; buttonIndex++) {
                    hasOnDpad |= Player_ItemIsInUse(this, DPAD_ITEM(buttonIndex));
                }
            }
            if (!Player_ItemIsInUse(this, B_BTN_ITEM) && !Player_ItemIsInUse(this, C_BTN_ITEM(0)) &&
                !Player_ItemIsInUse(this, C_BTN_ITEM(1)) && !Player_ItemIsInUse(this, C_BTN_ITEM(2)) && !hasOnDpad) {
                Player_UseItem(play, this, ITEM_NONE);
                return;
            }
        }

        for (i = 0; i < ARRAY_COUNT(sItemButtons); i++) {
            if (CHECK_BTN_ALL(sControlInput->press.button, sItemButtons[i])) {
                break;
            }
        }

        item = Player_GetItemOnButton(play, i);

        if (item >= ITEM_NONE_FE) {
            for (i = 0; i < ARRAY_COUNT(sItemButtons); i++) {
                if (CHECK_BTN_ALL(sControlInput->cur.button, sItemButtons[i])) {
                    break;
                }
            }

            item = Player_GetItemOnButton(play, i);

            if ((item < ITEM_NONE_FE) && (Player_ItemToItemAction(item) == this->heldItemAction)) {
                sHeldItemButtonIsHeldDown = true;
            }
        } else {
            this->heldItemButton = i;
            Player_UseItem(play, this, item);
        }
    }
}

void Player_StartChangingHeldItem(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 endFrameTemp;
    f32 startFrame;
    f32 endFrame;
    f32 playSpeed;
    s32 itemChangeType;
    s8 heldItemAction;
    s32 nextAnimType;

    heldItemAction = Player_ItemToItemAction(this->heldItemId);

    Player_SetUpperActionFunc(this, Player_UpperAction_ChangeHeldItem);

    nextAnimType = gPlayerModelTypes[this->nextModelGroup][PLAYER_MODELGROUPENTRY_ANIM];
    itemChangeType = sItemChangeTypes[gPlayerModelTypes[this->modelGroup][PLAYER_MODELGROUPENTRY_ANIM]][nextAnimType];
    
    if ((heldItemAction == PLAYER_IA_BOTTLE) || (heldItemAction == PLAYER_IA_BOOMERANG) ||
        ((heldItemAction == PLAYER_IA_NONE) &&
         ((this->heldItemAction == PLAYER_IA_BOTTLE) || (this->heldItemAction == PLAYER_IA_BOOMERANG)))) {
        itemChangeType = (heldItemAction == PLAYER_IA_NONE) ? -PLAYER_ITEM_CHG_13 : PLAYER_ITEM_CHG_13;
    }

    this->itemChangeType = ABS(itemChangeType);
    anim = sItemChangeInfo[this->itemChangeType].anim;

    if ((anim == &gPlayerAnim_link_normal_fighter2free) && (this->currentShield == PLAYER_SHIELD_NONE)) {
        anim = &gPlayerAnim_link_normal_free2fighter_free;
    }

    endFrameTemp = Animation_GetLastFrame(anim);
    endFrame = endFrameTemp;

    if (itemChangeType >= 0) {
        playSpeed = 1.2f;
        startFrame = 0.0f;
    } else {
        endFrame = 0.0f;
        playSpeed = -1.2f;
        startFrame = endFrameTemp;
    }

    if (heldItemAction != PLAYER_IA_NONE) {
        playSpeed *= 2.0f;
    }

    LinkAnimation_Change(play, &this->upperSkelAnime, anim, playSpeed, startFrame, endFrame, ANIMMODE_ONCE, 0.0f);

    this->stateFlags1 &= ~PLAYER_STATE1_START_CHANGING_HELD_ITEM;
}

void Player_UpdateItems(Player* this, PlayState* play) {
    if ((this->actor.category == ACTORCAT_PLAYER) && (CVarGetInteger(CVAR_ENHANCEMENT("QuickPutaway"), 0) || !(this->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM)) &&
        ((this->heldItemAction == this->itemAction) || (this->stateFlags1 & PLAYER_STATE1_SHIELDING)) &&
        (gSaveContext.health != 0) && (play->csCtx.state == CS_STATE_IDLE) && (this->csAction == 0) &&
        (play->shootingGalleryStatus == 0) && (play->activeCamera == MAIN_CAM) &&
        (play->transitionTrigger != TRANS_TRIGGER_START) && (gSaveContext.timer1State != 10)) {
        Player_ProcessItemButtons(this, play);
    }

    if (this->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM) {
        Player_StartChangingHeldItem(this, play);
    }
}

// Determine projectile type for bow or slingshot
s32 func_80834380(PlayState* play, Player* this, s32* itemPtr, s32* typePtr) {
    bool useBow = LINK_IS_ADULT;
    if(CVarGetInteger(CVAR_ENHANCEMENT("BowSlingshotAmmoFix"), 0)){
        useBow = this->heldItemAction != PLAYER_IA_SLINGSHOT;
    }
    if (useBow) {
        *itemPtr = ITEM_BOW;
        if (this->stateFlags1 & PLAYER_STATE1_ON_HORSE) {
            *typePtr = ARROW_NORMAL_HORSE;
        } else {
            *typePtr = this->heldItemAction - 6;
        }
    } else {
        *itemPtr = ITEM_SLINGSHOT;
        *typePtr = ARROW_SEED;
    }

    if (gSaveContext.minigameState == 1) {
        return play->interfaceCtx.hbaAmmo;
    } else if (play->shootingGalleryStatus != 0) {
        return play->shootingGalleryStatus;
    } else {
        return AMMO(*itemPtr);
    }
}

// The player has pressed the bow or hookshot button
s32 func_8083442C(Player* this, PlayState* play) {
    s32 item;
    s32 arrowType;
    s32 magicArrowType;

    if ((this->heldItemAction >= PLAYER_IA_BOW_FIRE) && (this->heldItemAction <= PLAYER_IA_BOW_0E) &&
        (gSaveContext.magicState != MAGIC_STATE_IDLE)) {
        func_80078884(NA_SE_SY_ERROR);
    } else {
        Player_SetUpperActionFunc(this, func_808351D4);

        this->stateFlags1 |= PLAYER_STATE1_READY_TO_FIRE;
        this->unk_834 = 14;

        if (this->unk_860 >= 0) {
            Player_PlaySfx(this, D_80854398[ABS(this->unk_860) - 1]);

            if (!Player_HoldsHookshot(this) && (func_80834380(play, this, &item, &arrowType) > 0)) {
                magicArrowType = arrowType - ARROW_FIRE;

                if (this->unk_860 >= 0) {
                    if ((magicArrowType >= 0) && (magicArrowType <= 2) &&
                        !Magic_RequestChange(play, sMagicArrowCosts[magicArrowType], MAGIC_CONSUME_NOW)) {
                        arrowType = ARROW_NORMAL;
                    }

                    this->heldActor = Actor_SpawnAsChild(
                        &play->actorCtx, &this->actor, play, ACTOR_EN_ARROW, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, arrowType);
                }
            }
        }

        return 1;
    }

    return 0;
}

void Player_FinishItemChange(PlayState* play, Player* this) {
    if (this->heldItemAction != PLAYER_IA_NONE) {
        if (func_8008F2BC(this, this->heldItemAction) >= 0) {
            func_808328EC(this, NA_SE_IT_SWORD_PUTAWAY);
        } else {
            func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
        }
    }

    Player_UseItem(play, this, this->heldItemId);

    if (func_8008F2BC(this, this->heldItemAction) >= 0) {
        func_808328EC(this, NA_SE_IT_SWORD_PICKOUT);
    } else if (this->heldItemAction != PLAYER_IA_NONE) {
        func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
    }
}

void func_80834644(PlayState* play, Player* this) {
    if (Player_UpperAction_ChangeHeldItem == this->upperActionFunc) {
        Player_FinishItemChange(play, this);
    }

    Player_SetUpperActionFunc(this, sItemActionUpdateFuncs[this->heldItemAction]);
    this->unk_834 = 0;
    this->idleType = PLAYER_IDLE_DEFAULT;
    Player_DetachHeldActor(play, this);
    this->stateFlags1 &= ~PLAYER_STATE1_START_CHANGING_HELD_ITEM;
}

LinkAnimationHeader* func_808346C4(PlayState* play, Player* this) {
    Player_SetUpperActionFunc(this, func_80834B5C);
    Player_DetachHeldActor(play, this);

    if (this->unk_870 < 0.5f) {
        return D_808543A4[Player_HoldsTwoHandedWeapon(this) &&
                          !(CVarGetInteger(CVAR_CHEAT("ShieldTwoHanded"), 0) && (this->heldItemAction != PLAYER_IA_DEKU_STICK))];
    } else {
        return D_808543AC[Player_HoldsTwoHandedWeapon(this) &&
                          !(CVarGetInteger(CVAR_CHEAT("ShieldTwoHanded"), 0) && (this->heldItemAction != PLAYER_IA_DEKU_STICK))];
    }
}

s32 func_80834758(PlayState* play, Player* this) {
    LinkAnimationHeader* anim;
    f32 frame;

    if (!(this->stateFlags1 & (PLAYER_STATE1_SHIELDING | PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_IN_CUTSCENE)) &&
        (play->shootingGalleryStatus == 0) && (this->heldItemAction == this->itemAction) &&
        (this->currentShield != PLAYER_SHIELD_NONE) && !Player_IsChildWithHylianShield(this) && Player_IsZTargeting(this) &&
        CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {

        anim = func_808346C4(play, this);
        frame = Animation_GetLastFrame(anim);
        LinkAnimation_Change(play, &this->upperSkelAnime, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);
        Player_PlaySfx(this, NA_SE_IT_SHIELD_POSTURE);

        return 1;
    } else {
        return 0;
    }
}

s32 func_8083485C(Player* this, PlayState* play) {
    if (func_80834758(play, this)) {
        return true;
    } else {
        return false;
    }
}

void func_80834894(Player* this) {
    Player_SetUpperActionFunc(this, func_80834C74);

    if (this->itemAction < 0) {
        func_8008EC70(this);
    }

    Animation_Reverse(&this->upperSkelAnime);
    Player_PlaySfx(this, NA_SE_IT_SHIELD_REMOVE);
}

void Player_WaitToFinishItemChange(PlayState* play, Player* this) {
    ItemChangeInfo* itemChangeEntry = &sItemChangeInfo[this->itemChangeType];
    f32 changeFrame;

    changeFrame = itemChangeEntry->changeFrame;
    changeFrame = (this->upperSkelAnime.playSpeed < 0.0f) ? changeFrame - 1.0f : changeFrame;

    if (LinkAnimation_OnFrame(&this->upperSkelAnime, changeFrame)) {
        Player_FinishItemChange(play, this);
    }

    Player_UpdateHostileLockOn(this);
}

s32 func_8083499C(Player* this, PlayState* play) {
    if (this->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM) {
        Player_StartChangingHeldItem(this, play);
    } else {
        return 0;
    }

    return 1;
}

/**
 * The actual sword weapon is not handled here. See `Player_ActionHandler_7` for melee weapon usage.
 * This upper body action allows for shielding or changing held items while a sword is in hand.
 */
s32 Player_UpperAction_Sword(Player* this, PlayState* play) {
    if (func_80834758(play, this) || func_8083499C(this, play)) {
        return true;
    } else {
        return false;
    }
}

s32 Player_UpperAction_ChangeHeldItem(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->upperSkelAnime) ||
        ((Player_ItemToItemAction(this->heldItemId) == this->heldItemAction) &&
         (sUseHeldItem =
              (sUseHeldItem || ((this->modelAnimType != PLAYER_ANIMTYPE_3) && (play->shootingGalleryStatus == 0)))))) {
        Player_SetUpperActionFunc(this, sItemActionUpdateFuncs[this->heldItemAction]);
        this->unk_834 = 0;
        this->idleType = PLAYER_IDLE_DEFAULT;
        sHeldItemButtonIsHeldDown = sUseHeldItem;

        return this->upperActionFunc(this, play);
    }

    if (Player_CheckForIdleAnim(this) != IDLE_ANIM_NONE) {
        Player_WaitToFinishItemChange(play, this);
        Player_AnimPlayOnce(play, this, Player_GetIdleAnim(this));
        this->idleType = PLAYER_IDLE_DEFAULT;
    } else {
        Player_WaitToFinishItemChange(play, this);
    }

    return true;
}

s32 func_80834B5C(Player* this, PlayState* play) {
    LinkAnimation_Update(play, &this->upperSkelAnime);

    if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {
        func_80834894(this);
        return true;
    } else {
        this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
        Player_SetModelsForHoldingShield(this);
        return true;
    }
}

s32 func_80834BD4(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 frame;

    if (LinkAnimation_Update(play, &this->upperSkelAnime)) {
        anim = func_808346C4(play, this);
        frame = Animation_GetLastFrame(anim);
        LinkAnimation_Change(play, &this->upperSkelAnime, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);
    }

    this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
    Player_SetModelsForHoldingShield(this);

    return true;
}

s32 func_80834C74(Player* this, PlayState* play) {
    sUseHeldItem = sHeldItemButtonIsHeldDown;

    if (sUseHeldItem || LinkAnimation_Update(play, &this->upperSkelAnime)) {
        Player_SetUpperActionFunc(this, sItemActionUpdateFuncs[this->heldItemAction]);
        LinkAnimation_PlayLoop(play, &this->upperSkelAnime,
                               GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType));
        this->idleType = PLAYER_IDLE_DEFAULT;
        this->upperActionFunc(this, play);

        return false;
    }

    return true;
}

s32 func_80834D2C(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;

    if (this->heldItemAction != PLAYER_IA_BOOMERANG) {
        if (!func_8083442C(this, play)) {
            return 0;
        }

        if (!Player_HoldsHookshot(this)) {
            anim = &gPlayerAnim_link_bow_bow_ready;
        } else {
            anim = &gPlayerAnim_link_hook_shot_ready;
        }
        LinkAnimation_PlayOnce(play, &this->upperSkelAnime, anim);
    } else {
        Player_SetUpperActionFunc(this, func_80835884);
        this->unk_834 = 10;
        LinkAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_boom_throw_wait2waitR);
    }

    if (this->stateFlags1 & PLAYER_STATE1_ON_HORSE) {
        Player_AnimPlayLoop(play, this, &gPlayerAnim_link_uma_anim_walk);
    } else if ((this->actor.bgCheckFlags & 1) && !Player_UpdateHostileLockOn(this)) {
        Player_AnimPlayLoop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType));
    }

    return 1;
}

int func_80834E44(PlayState* play) {
    return (play->shootingGalleryStatus > 0) && CHECK_BTN_ALL(sControlInput->press.button, BTN_B);
}

int func_80834E7C(PlayState* play) {
    u16 buttonsToCheck = BTN_A | BTN_B | BTN_CUP | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) != 0) {
        buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
    }
    return (play->shootingGalleryStatus != 0) &&
           ((play->shootingGalleryStatus < 0) ||
            CHECK_BTN_ANY(sControlInput->cur.button, buttonsToCheck));
}

s32 func_80834EB8(Player* this, PlayState* play) {
    if ((this->unk_6AD == 0) || (this->unk_6AD == 2)) {
        if (Player_IsZTargeting(this) ||
            (Camera_CheckValidMode(Play_GetCamera(play, 0), 7) == 0)) {
            return 1;
        }
        this->unk_6AD = 2;
    }

    return 0;
}

s32 func_80834F2C(Player* this, PlayState* play) {
    if ((this->doorType == PLAYER_DOORTYPE_NONE) && !(this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN)) {
        if (sUseHeldItem || func_80834E44(play)) {
            if (func_80834D2C(this, play)) {
                return func_80834EB8(this, play);
            }
        }
    }

    return 0;
}

s32 func_80834FBC(Player* this) {
    if (this->actor.child != NULL) {
        if (this->heldActor == NULL) {
            this->heldActor = this->actor.child;
            Player_RequestRumble(this, 255, 10, 250, 0);
            Player_PlaySfx(this, NA_SE_IT_HOOKSHOT_RECEIVE);
        }

        return 1;
    }

    return 0;
}

s32 func_8083501C(Player* this, PlayState* play) {
    if (this->unk_860 >= 0) {
        this->unk_860 = -this->unk_860;
    }

    if ((!Player_HoldsHookshot(this) || func_80834FBC(this)) && !func_80834758(play, this) &&
        !func_80834F2C(this, play)) {
        return false;
    } else if (this->rideActor != NULL) {
        this->unk_6AD = 2; // OTRTODO: THIS IS A BAD IDEA BUT IT FIXES THE HORSE FIRST PERSON?
    }

    return true;
}

// Fire the projectile
s32 func_808350A4(PlayState* play, Player* this) {
    s32 item;
    s32 arrowType;

    if (this->heldActor != NULL) {
        if (!Player_HoldsHookshot(this)) {
            func_80834380(play, this, &item, &arrowType);

            if (gSaveContext.minigameState == 1) {
                if (!CVarGetInteger(CVAR_CHEAT("InfiniteAmmo"), 0)) {
                    play->interfaceCtx.hbaAmmo--;
                }
            } else if (play->shootingGalleryStatus != 0) {
                if (!CVarGetInteger(CVAR_CHEAT("InfiniteAmmo"), 0)) {
                    play->shootingGalleryStatus--;
                }
            } else {
                Inventory_ChangeAmmo(item, -1);
            }

            if (play->shootingGalleryStatus == 1) {
                play->shootingGalleryStatus = -10;
            }

            Player_RequestRumble(this, 150, 10, 150, 0);
        } else {
            Player_RequestRumble(this, 255, 20, 150, 0);
        }

        this->unk_A73 = 4;
        this->heldActor->parent = NULL;
        this->actor.child = NULL;
        this->heldActor = NULL;

        return 1;
    }

    return 0;
}

static u16 D_808543DC[] = { NA_SE_IT_BOW_FLICK, NA_SE_IT_SLING_FLICK };

s32 func_808351D4(Player* this, PlayState* play) {
    s32 sp2C;

    if (!Player_HoldsHookshot(this)) {
        sp2C = 0;
    } else {
        sp2C = 1;
    }

    Math_ScaledStepToS(&this->upperLimbRot.z, 1200, 400);
    this->unk_6AE_rotFlags |= UNK6AE_ROT_UPPER_Z;

    if ((this->unk_836 == 0) && (Player_CheckForIdleAnim(this) == IDLE_ANIM_NONE) &&
        (this->skelAnime.animation == &gPlayerAnim_link_bow_side_walk)) {
        LinkAnimation_PlayOnce(play, &this->upperSkelAnime, D_808543CC[sp2C]);
        this->unk_836 = -1;
    } else if (LinkAnimation_Update(play, &this->upperSkelAnime)) {
        LinkAnimation_PlayLoop(play, &this->upperSkelAnime, D_808543D4[sp2C]);
        this->unk_836 = 1;
    } else if (this->unk_836 == 1) {
        this->unk_836 = 2;
    }

    if (this->unk_834 > 10) {
        this->unk_834--;
    }

    func_80834EB8(this, play);

    if ((this->unk_836 > 0) && ((this->unk_860 < 0) || (!sHeldItemButtonIsHeldDown && !func_80834E7C(play)))) {
        Player_SetUpperActionFunc(this, func_808353D8);
        if (this->unk_860 >= 0) {
            if (sp2C == 0) {
                if (!func_808350A4(play, this)) {
                    Player_PlaySfx(this, D_808543DC[ABS(this->unk_860) - 1]);
                }
            } else if (this->actor.bgCheckFlags & 1) {
                func_808350A4(play, this);
            }
        }
        this->unk_834 = 10;
        Player_ZeroSpeedXZ(this);
    } else {
        this->stateFlags1 |= PLAYER_STATE1_READY_TO_FIRE;
    }

    return true;
}

s32 func_808353D8(Player* this, PlayState* play) {
    LinkAnimation_Update(play, &this->upperSkelAnime);

    if (Player_HoldsHookshot(this) && !func_80834FBC(this)) {
        return true;
    }

    if (!func_80834758(play, this) &&
        (sUseHeldItem || ((this->unk_860 < 0) && sHeldItemButtonIsHeldDown) || func_80834E44(play))) {
        this->unk_860 = ABS(this->unk_860);

        if (func_8083442C(this, play)) {
            if (Player_HoldsHookshot(this)) {
                this->unk_836 = 1;
            } else {
                LinkAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_bow_bow_shoot_next);
            }
        }
    } else {
        if (this->unk_834 != 0) {
            this->unk_834--;
        }

        if (Player_IsZTargeting(this) || (this->unk_6AD != 0) || (this->stateFlags1 & PLAYER_STATE1_FIRST_PERSON)) {
            if (this->unk_834 == 0) {
                this->unk_834++;
            }

            return true;
        }

        if (Player_HoldsHookshot(this)) {
            Player_SetUpperActionFunc(this, func_8083501C);
        } else {
            Player_SetUpperActionFunc(this, func_80835588);
            LinkAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_bow_bow_shoot_end);
        }

        this->unk_834 = 0;
    }

    return true;
}

s32 func_80835588(Player* this, PlayState* play) {
    if (!(this->actor.bgCheckFlags & 1) || LinkAnimation_Update(play, &this->upperSkelAnime)) {
        Player_SetUpperActionFunc(this, func_8083501C);
    }

    return true;
}

void Player_SetParallel(Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_PARALLEL;

    if (!(this->skelAnime.moveFlags & 0x80) &&
        (this->actor.bgCheckFlags & 0x200) && (sShapeYawToTouchedWall < 0x2000)) {
        // snap to the wall
        this->yaw = this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
    }

    this->parallelYaw = this->actor.shape.rot.y;
}

s32 func_80835644(PlayState* play, Player* this, Actor* arg2) {
    if (arg2 == NULL) {
        func_80832564(play, this);
        func_80839F90(this, play);
        return 1;
    }

    return 0;
}

void func_80835688(Player* this, PlayState* play) {
    if (!func_80835644(play, this, this->heldActor)) {
        Player_SetUpperActionFunc(this, Player_UpperAction_CarryActor);
        LinkAnimation_PlayLoop(play, &this->upperSkelAnime, &gPlayerAnim_link_normal_carryB_wait);
    }
}

s32 Player_UpperAction_CarryActor(Player* this, PlayState* play) {
    Actor* heldActor = this->heldActor;

    if (heldActor == NULL) {
        func_80834644(play, this);
    }

    if (func_80834758(play, this)) {
        return true;
    }

    if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        if (LinkAnimation_Update(play, &this->upperSkelAnime)) {
            LinkAnimation_PlayLoop(play, &this->upperSkelAnime, &gPlayerAnim_link_normal_carryB_wait);
        }

        if ((heldActor->id == ACTOR_EN_NIW) && (this->actor.velocity.y <= 0.0f)) {
            this->actor.minVelocityY = -2.0f;
            this->actor.gravity = -0.5f;
            this->fallStartHeight = this->actor.world.pos.y;
        }

        return true;
    }

    return func_8083485C(this, play);
}

void func_808357E8(Player* this, Gfx** dLists) {
    if (LINK_IS_ADULT && (CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0))) {
        this->leftHandDLists = &dLists[1];
    } else {
        this->leftHandDLists = &dLists[gSaveContext.linkAge];
    }
}

s32 func_80835800(Player* this, PlayState* play) {
    if (func_80834758(play, this)) {
        return true;
    }

    if (this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN) {
        Player_SetUpperActionFunc(this, func_80835B60);
    } else if (func_80834F2C(this, play)) {
        return true;
    }

    return false;
}

s32 func_80835884(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->upperSkelAnime)) {
        Player_SetUpperActionFunc(this, func_808358F0);
        LinkAnimation_PlayLoop(play, &this->upperSkelAnime, &gPlayerAnim_link_boom_throw_waitR);
    }

    func_80834EB8(this, play);

    return true;
}

s32 func_808358F0(Player* this, PlayState* play) {
    LinkAnimationHeader* animSeg = this->skelAnime.animation;

    if ((func_808334E4(this) == animSeg) || (func_80833528(this) == animSeg) || (func_808335B0(this) == animSeg) ||
        (func_808335F4(this) == animSeg)) {
        AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->upperSkelAnime.jointTable,
                                    this->skelAnime.jointTable);
    } else {
        LinkAnimation_Update(play, &this->upperSkelAnime);
    }

    func_80834EB8(this, play);

    if (!sHeldItemButtonIsHeldDown) {
        Player_SetUpperActionFunc(this, func_808359FC);
        LinkAnimation_PlayOnce(play, &this->upperSkelAnime,
                               (this->unk_870 < 0.5f) ? &gPlayerAnim_link_boom_throwR : &gPlayerAnim_link_boom_throwL);
    }

    return true;
}

s32 func_808359FC(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->upperSkelAnime)) {
        Player_SetUpperActionFunc(this, func_80835B60);
        this->unk_834 = 0;
    } else if (LinkAnimation_OnFrame(&this->upperSkelAnime, 6.0f)) {
        f32 posX = (Math_SinS(this->actor.shape.rot.y) * 10.0f) + this->actor.world.pos.x;
        f32 posZ = (Math_CosS(this->actor.shape.rot.y) * 10.0f) + this->actor.world.pos.z;
        s32 yaw = (this->focusActor != NULL) ? this->actor.shape.rot.y + 14000 : this->actor.shape.rot.y;
        EnBoom* boomerang =
            (EnBoom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOOM, posX, this->actor.world.pos.y + 30.0f, posZ,
                                 this->actor.focus.rot.x, yaw, 0, 0, true);

        this->boomerangActor = &boomerang->actor;

        if (boomerang != NULL) {
            boomerang->moveTo = this->focusActor;
            boomerang->returnTimer = 20;
            this->stateFlags1 |= PLAYER_STATE1_BOOMERANG_THROWN;

            if (!Player_CheckHostileLockOn(this)) {
                Player_SetParallel(this);
            }

            this->unk_A73 = 4;
            Player_PlaySfx(this, NA_SE_IT_BOOMERANG_THROW);
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_SWORD_N);
        }
    }

    return true;
}

s32 func_80835B60(Player* this, PlayState* play) {
    if (func_80834758(play, this)) {
        return true;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN)) {
        Player_SetUpperActionFunc(this, func_80835C08);
        LinkAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_boom_catch);
        func_808357E8(this, gPlayerLeftHandBoomerangDLs);
        Player_PlaySfx(this, NA_SE_PL_CATCH_BOOMERANG);
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_SWORD_N);

        return true;
    }

    if (sUseHeldItem && CVarGetInteger(CVAR_ENHANCEMENT("FastBoomerang"), 0)) {
        this->boomerangQuickRecall = true;
    }

    return false;
}

s32 func_80835C08(Player* this, PlayState* play) {
    if (!func_80835800(this, play) && LinkAnimation_Update(play, &this->upperSkelAnime)) {
        Player_SetUpperActionFunc(this, func_80835800);
    }

    return true;
}

s32 Player_SetupAction(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 flags) {
    if (actionFunc == this->actionFunc) {
        return 0;
    }

    if (Player_Action_8084E3C4 == this->actionFunc) {
        Audio_OcaSetInstrument(0);
        this->stateFlags2 &= ~(PLAYER_STATE2_ATTEMPT_PLAY_FOR_ACTOR | PLAYER_STATE2_PLAY_FOR_ACTOR);
    } else if (Player_Action_808507F4 == this->actionFunc) {
        func_80832340(play, this);
    }

    this->actionFunc = actionFunc;

    if ((this->itemAction != this->heldItemAction) && (!(flags & 1) || !(this->stateFlags1 & PLAYER_STATE1_SHIELDING))) {
        func_8008EC70(this);
    }

    if (!(flags & 1) && !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
        func_80834644(play, this);
        this->stateFlags1 &= ~PLAYER_STATE1_SHIELDING;
    }

    Player_FinishAnimMovement(this);

    this->stateFlags1 &= ~(PLAYER_STATE1_HOOKSHOT_FALLING | PLAYER_STATE1_TALKING | PLAYER_STATE1_DAMAGED | PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE |
                           PLAYER_STATE1_FLOOR_DISABLED);
    this->stateFlags2 &= ~(PLAYER_STATE2_HOPPING | PLAYER_STATE2_OCARINA_PLAYING | PLAYER_STATE2_IDLE_FIDGET);
    this->stateFlags3 &= ~(PLAYER_STATE3_MIDAIR | PLAYER_STATE3_FINISHED_ATTACKING | PLAYER_STATE3_FLYING_WITH_HOOKSHOT);

    this->av1.actionVar1 = 0;
    this->av2.actionVar2 = 0;

    this->idleType = PLAYER_IDLE_DEFAULT;

    func_808326F0(this);

    return 1;
}

/**
 * Calls `Player_SetupAction` to setup a new action, but takes extra measures to
 * preserve AnimMovement while doing so.
 */
void Player_SetupActionPreserveAnimMovement(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 flags) {
    s32 savedMovementFlags;

    savedMovementFlags = this->skelAnime.moveFlags;

    // Setting `skelAnime.movementFlags` to 0 will prevent `Player_FinishAnimMovement` from ending
    // AnimMovement when `Player_SetupAction` is called.
    this->skelAnime.moveFlags = 0;

    Player_SetupAction(play, this, actionFunc, flags);
    this->skelAnime.moveFlags = savedMovementFlags;
}

/**
 * Calls `Player_SetupAction` to setup a new action, but takes extra measures to
 * preserve the current itemAction while doing so.
 *
 * Note that `itemAction` must be PLAYER_IA_NONE or higher for the action change to take place.
 */
void Player_SetupActionPreserveItemAction(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 flags) {
    s32 savedItemAction;

    if (this->itemAction >= PLAYER_IA_NONE) {
        savedItemAction = this->itemAction;

        // Setting `itemAction` to `heldItemAction` will prevent `func_8008EC70` from running when
        // `Player_SetupAction` is called.
        this->itemAction = this->heldItemAction;

        Player_SetupAction(play, this, actionFunc, flags);
        this->itemAction = savedItemAction;
        Player_SetModels(this, Player_ActionToModelGroup(this, this->itemAction));
    }
}

void func_80835E44(PlayState* play, s16 camSetting) {
    if (!func_800C0CB8(play)) {
        if (camSetting == CAM_SET_SCENE_TRANSITION) {
            Interface_ChangeAlpha(2);
        }
    } else {
        Camera_ChangeSetting(Play_GetCamera(play, 0), camSetting);
    }
}

void func_80835EA4(PlayState* play, s32 arg1) {
    func_80835E44(play, CAM_SET_TURN_AROUND);
    Camera_SetCameraData(Play_GetCamera(play, 0), 4, NULL, NULL, arg1, 0, 0);
}

void Player_DestroyHookshot(Player* this) {
    if (Player_HoldsHookshot(this)) {
        Actor* heldActor = this->heldActor;

        if (heldActor != NULL) {
            Actor_Kill(heldActor);
            this->actor.child = NULL;
            this->heldActor = NULL;
        }
    }
}

void Player_UseItem(PlayState* play, Player* this, s32 item) {
    s8 itemAction;
    s32 temp;
    s32 nextAnimType;

    itemAction = Player_ItemToItemAction(item);

    if (((this->heldItemAction == this->itemAction) &&
         (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) || (Player_ActionToMeleeWeapon(itemAction) != 0) ||
          (itemAction == PLAYER_IA_NONE))) ||
        ((this->itemAction < 0) && ((Player_ActionToMeleeWeapon(itemAction) != 0) || (itemAction == PLAYER_IA_NONE)))) {

        if ((itemAction == PLAYER_IA_NONE) || !(this->stateFlags1 & PLAYER_STATE1_IN_WATER) ||
            ((this->actor.bgCheckFlags & 1) &&
             ((itemAction == PLAYER_IA_HOOKSHOT) || (itemAction == PLAYER_IA_LONGSHOT)))) {

            if ((play->bombchuBowlingStatus == 0) &&
                (((itemAction == PLAYER_IA_DEKU_STICK) && (AMMO(ITEM_STICK) == 0)) ||
                 ((itemAction == PLAYER_IA_MAGIC_BEAN) && (AMMO(ITEM_BEAN) == 0)) ||
                 (temp = Player_ActionToExplosive(this, itemAction),
                  ((temp >= 0) && ((AMMO(sExplosiveInfos[temp].itemId) == 0) ||
                                   (play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].length >= 3 && !CVarGetInteger(CVAR_ENHANCEMENT("RemoveExplosiveLimit"), 0))))))) {
                // Prevent some items from being used if player is out of ammo.
                // Also prevent explosives from being used if there are 3 or more active (outside of bombchu bowling)
                func_80078884(NA_SE_SY_ERROR);
            } else if (itemAction == PLAYER_IA_LENS_OF_TRUTH) {
                // Handle Lens of Truth
                if (Magic_RequestChange(play, 0, MAGIC_CONSUME_LENS)) {
                    if (play->actorCtx.lensActive) {
                        Actor_DisableLens(play);
                    } else {
                        play->actorCtx.lensActive = true;
                    }
                    func_80078884((play->actorCtx.lensActive) ? NA_SE_SY_GLASSMODE_ON : NA_SE_SY_GLASSMODE_OFF);
                } else {
                    func_80078884(NA_SE_SY_ERROR);
                }
            } else if (itemAction == PLAYER_IA_DEKU_NUT) {
                // Handle Deku Nuts
                if (AMMO(ITEM_NUT) != 0) {
                    func_8083C61C(play, this);
                } else {
                    func_80078884(NA_SE_SY_ERROR);
                }
            } else if ((temp = Player_ActionToMagicSpell(this, itemAction)) >= 0) {
                // Handle magic spells
                if (((itemAction == PLAYER_IA_FARORES_WIND) && (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 0)) ||
                    ((gSaveContext.magicCapacity != 0) && (gSaveContext.magicState == MAGIC_STATE_IDLE) &&
                     (gSaveContext.magic >= sMagicSpellCosts[temp]))) {
                    this->itemAction = itemAction;
                    this->unk_6AD = 4;
                } else {
                    func_80078884(NA_SE_SY_ERROR);
                }
            } else if (itemAction >= PLAYER_IA_MASK_KEATON) {
                // Handle wearable masks
                if (this->currentMask != PLAYER_MASK_NONE) {
                    this->currentMask = PLAYER_MASK_NONE;
                } else {
                    this->currentMask = itemAction - PLAYER_IA_MASK_KEATON + 1;
                }

                gSaveContext.maskMemory = this->currentMask;

                func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
            } else if (((itemAction >= PLAYER_IA_OCARINA_FAIRY) && (itemAction <= PLAYER_IA_OCARINA_OF_TIME)) ||
                (itemAction >= PLAYER_IA_BOTTLE_FISH)) {
                // Handle "cutscene items"
                if (!Player_CheckHostileLockOn(this) ||
                    ((itemAction >= PLAYER_IA_BOTTLE_POTION_RED) && (itemAction <= PLAYER_IA_BOTTLE_FAIRY))) {
                    func_8002D53C(play, &play->actorCtx.titleCtx);
                    this->unk_6AD = 4;
                    this->itemAction = itemAction;
                }
            } else if ((itemAction != this->heldItemAction) ||
                ((this->heldActor == NULL) && (Player_ActionToExplosive(this, itemAction) >= 0))) {
                // Handle using a new held item
                this->nextModelGroup = Player_ActionToModelGroup(this, itemAction);
                nextAnimType = gPlayerModelTypes[this->nextModelGroup][PLAYER_MODELGROUPENTRY_ANIM];

                if ((this->heldItemAction >= 0) && (Player_ActionToMagicSpell(this, itemAction) < 0) &&
                    (item != this->heldItemId) &&
                    (sItemChangeTypes[gPlayerModelTypes[this->modelGroup][PLAYER_MODELGROUPENTRY_ANIM]][nextAnimType] !=
                     PLAYER_ITEM_CHG_0) &&
                    (!CVarGetInteger(CVAR_ENHANCEMENT("SeparateArrows"), 0) ||
                     itemAction < PLAYER_IA_BOW || itemAction > PLAYER_IA_BOW_0E ||
                     this->heldItemAction < PLAYER_IA_BOW || this->heldItemAction > PLAYER_IA_BOW_0E)) {
                    // Start the held item change process
                    this->heldItemId = item;
                    this->stateFlags1 |= PLAYER_STATE1_START_CHANGING_HELD_ITEM;
                } else {
                    // Init new held item for use
                    Player_DestroyHookshot(this);
                    Player_DetachHeldActor(play, this);
                    Player_InitItemActionWithAnim(play, this, itemAction);
                }
            } else {
                // Handle using the held item already in hand
                sUseHeldItem = sHeldItemButtonIsHeldDown = true;
            }
        }
    }
}

void func_80836448(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    s32 cond = func_808332B8(this);

    func_80832564(play, this);

    Player_SetupAction(play, this, cond ? Player_Action_8084E368 : Player_Action_80843CEC, 0);

    this->stateFlags1 |= PLAYER_STATE1_DEAD;

    Player_AnimPlayOnce(play, this, anim);
    if (anim == &gPlayerAnim_link_derth_rebirth) {
        this->skelAnime.endFrame = 84.0f;
    }

    func_80832224(this);
    Player_PlayVoiceSfx(this, NA_SE_VO_LI_DOWN);

    if (this->actor.category == ACTORCAT_PLAYER) {
        func_800F47BC();

        if (Inventory_ConsumeFairy(play)) {
            play->gameOverCtx.state = GAMEOVER_REVIVE_START;
            this->av1.actionVar1 = 1;
        } else {
            play->gameOverCtx.state = GAMEOVER_DEATH_START;
            func_800F6AB0(0);
            Audio_PlayFanfare(NA_BGM_GAME_OVER);
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
        }

        OnePointCutscene_Init(play, 9806, cond ? 120 : 60, &this->actor, MAIN_CAM);
        ShrinkWindow_SetVal(0x20);
    }
}

int Player_CanUpdateItems(Player* this) {
    return (!(Player_Action_WaitForPutAway == this->actionFunc) ||
            ((this->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM) &&
             ((this->heldItemId == ITEM_LAST_USED) || (this->heldItemId == ITEM_NONE)))) &&
           (!(Player_UpperAction_ChangeHeldItem == this->upperActionFunc) ||
            (Player_ItemToItemAction(this->heldItemId) == this->heldItemAction));
}

/**
 * Updates the Upper Body system.
 * The Upper Body system is composed of an upper action function and
 * a separate skelanime that can play an animation which is different
 * from the main skelanime.
 *
 * @return true if the upper body is "busy", false otherwise.
 *
 * The upper body being "busy" can mean a few things:
 * - Hookshot has just connected with something that Player can fly to
 * - A deku nut is currently being thrown
 * - The current upper action function has indicated that it is busy
 *
 * If an upper action indicates being busy by returning true, the
 * animation playing in the upper body skeleton will be used.
 * This animation may be used for all limbs or only the upper body limbs
 * depending on some conditions. See details below.
 */
s32 Player_UpdateUpperBody(Player* this, PlayState* play) {
    if (!(this->stateFlags1 & PLAYER_STATE1_ON_HORSE) && (this->actor.parent != NULL) && Player_HoldsHookshot(this)) {
        Player_SetupAction(play, this, Player_Action_80850AEC, 1);
        this->stateFlags3 |= PLAYER_STATE3_FLYING_WITH_HOOKSHOT;
        Player_AnimPlayOnce(play, this, &gPlayerAnim_link_hook_fly_start);
        Player_StartAnimMovement(play, this, 0x9B);
        func_80832224(this);
        this->yaw = this->actor.shape.rot.y;
        this->actor.bgCheckFlags &= ~1;
        this->hoverBootsTimer = 0;
        this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_FOCUS_Y | UNK6AE_ROT_UPPER_X;
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_LASH);
        return true;
    }

    if (Player_CanUpdateItems(this)) {
        Player_UpdateItems(this, play);
        if (Player_Action_8084E604 == this->actionFunc) {
            return true;
        }
    }

    if (!this->upperActionFunc(this, play)) {
        return false;
    }

    if (this->upperAnimInterpWeight != 0.0f) {
        // The functionality contained within this block of code is never used in practice
        // because `upperAnimInterpWeight` is always 0.
        if ((Player_CheckForIdleAnim(this) == IDLE_ANIM_NONE) || (this->linearVelocity != 0.0f)) {
            AnimationContext_SetCopyFalse(play, this->skelAnime.limbCount, this->upperSkelAnime.jointTable,
                                          this->skelAnime.jointTable, sUpperBodyLimbCopyMap);
        }
        Math_StepToF(&this->upperAnimInterpWeight, 0.0f, 0.25f);
        AnimationContext_SetInterp(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                   this->upperSkelAnime.jointTable, 1.0f - this->upperAnimInterpWeight);
    } else if ((Player_CheckForIdleAnim(this) == IDLE_ANIM_NONE) || (this->linearVelocity != 0.0f)) {
        // Only copy the upper body animation to the upper body limbs in the main skeleton.
        // Doing so allows the main skeleton to play its own animation for the lower body limbs.
        AnimationContext_SetCopyTrue(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                     this->upperSkelAnime.jointTable, sUpperBodyLimbCopyMap);
    } else {
        // Copy all of the upper body animation into the whole main skeleton.
        // The upper body has full control of all limbs.
        AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                    this->upperSkelAnime.jointTable);
    }

    return true;
}

/**
 * Sets up `Player_Action_WaitForPutAway`, which will allow the held item put away process
 * to complete before moving on to a new action.
 *
 * The function provided by the `afterPutAwayFunc` argument will run after the put away is complete.
 * This function is expected to set a new action and move execution away from `Player_Action_WaitForPutAway`.
 *
 * @return  From `Player_PutAwayHeldItem`: true if an item needs to be put away, false if not.
 */
s32 Player_SetupWaitForPutAway(PlayState* play, Player* this, AfterPutAwayFunc afterPutAwayFunc) {
    this->afterPutAwayFunc = afterPutAwayFunc;
    Player_SetupAction(play, this, Player_Action_WaitForPutAway, 0);
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;

    return Player_PutAwayHeldItem(play, this);
}

/**
 * Updates Shape Yaw (`shape.rot.y`). In other words, the Y rotation of Player's model.
 * This does not affect the direction Player will move in.
 *
 * There are 3 modes shape yaw can be updated with, based on player state:
 *     - Lock on:  Rotates Player to face the current lock on target.
 *     - Parallel: Rotates Player to face the current Parallel angle, set when Z-Targeting without an actor lock-on
 *     - Normal:   Rotates Player to face `this->yaw`, the direction he is currently moving
 */
void Player_UpdateShapeYaw(Player* this, PlayState* play) {
    s16 previousYaw = this->actor.shape.rot.y;

    if (!(this->stateFlags2 & (PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS))) {
        if ((this->focusActor != NULL) &&
            ((play->actorCtx.targetCtx.unk_4B != 0) || (this->actor.category != ACTORCAT_PLAYER))) {
            Math_ScaledStepToS(&this->actor.shape.rot.y,
                               Math_Vec3f_Yaw(&this->actor.world.pos, &this->focusActor->focus.pos), 4000);
        } else if ((this->stateFlags1 & PLAYER_STATE1_PARALLEL) &&
                   !(this->stateFlags2 & (PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS))) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, this->parallelYaw, 4000);
        }
    } else if (!(this->stateFlags2 & PLAYER_STATE2_DISABLE_ROTATION_ALWAYS)) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->yaw, 2000);
    }

    this->unk_87C = this->actor.shape.rot.y - previousYaw;
}

/**
 * Step a value by `step` to a `target` value.
 * Constrains the value to be no further than `constraintRange` from `constraintMid` (accounting for wrapping).
 * Constrains the value to be no further than `overflowRange` from 0.
 * If this second constraint is enforced, return how much the value was past by the range, or return 0.
 *
 * @return The amount by which the value overflowed the absolute range defined by `overflowRange`
 */
s32 Player_ScaledStepBinangClamped(s16* pValue, s16 target, s16 step, s16 overflowRange, s16 constraintMid,
                                   s16 constraintRange) {
    s16 diff;
    s16 clampedDiff;
    s16 valueBeforeOverflowClamp;

    // Clamp value to [constraintMid - constraintRange , constraintMid + constraintRange]
    // This is more involved than a simple `CLAMP`, to account for binang wrapping
    diff = clampedDiff = constraintMid - *pValue;
    clampedDiff = CLAMP(clampedDiff, -constraintRange, constraintRange);
    *pValue += (s16)(diff - clampedDiff);

    Math_ScaledStepToS(pValue, target, step);

    valueBeforeOverflowClamp = *pValue;
    if (*pValue < -overflowRange) {
        *pValue = -overflowRange;
    } else if (*pValue > overflowRange) {
        *pValue = overflowRange;
    }
    return valueBeforeOverflowClamp - *pValue;
}

s32 func_80836AB8(Player* this, s32 arg1) {
    s16 targetUpperBodyYaw;
    s16 yaw;

    yaw = this->actor.shape.rot.y;
    if (arg1) {
        yaw = this->actor.focus.rot.y;
        this->upperLimbRot.x = this->actor.focus.rot.x;
        this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_UPPER_X;
    } else {
        // Step the head pitch to the focus pitch.
        // If the head cannot be pitched enough, pitch the upper body.
        Player_ScaledStepBinangClamped(&this->upperLimbRot.x,
                                       Player_ScaledStepBinangClamped(&this->headLimbRot.x, this->actor.focus.rot.x,
                                                                      600, 10000, this->actor.focus.rot.x, 0),
                                       200, 4000, this->headLimbRot.x, 10000);

        // Step the upper body and head yaw to the focus yaw.
        // Eventually prefers turning the upper body rather than the head.
        targetUpperBodyYaw = this->actor.focus.rot.y - yaw;
        Player_ScaledStepBinangClamped(&targetUpperBodyYaw, 0, 200, 24000, this->upperLimbRot.y, 8000);
        yaw = this->actor.focus.rot.y - targetUpperBodyYaw;
        Player_ScaledStepBinangClamped(&this->headLimbRot.y, targetUpperBodyYaw - this->upperLimbRot.y, 200, 8000,
                                       targetUpperBodyYaw, 8000);
        Player_ScaledStepBinangClamped(&this->upperLimbRot.y, targetUpperBodyYaw, 200, 8000, this->headLimbRot.y, 8000);

        this->unk_6AE_rotFlags |=
            UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_HEAD_X | UNK6AE_ROT_HEAD_Y | UNK6AE_ROT_UPPER_X | UNK6AE_ROT_UPPER_Y;
    }

    return yaw;
}

/**
 * Updates state related to Z-Targeting.
 *
 * Z-Targeting is an umbrella term for two main states:
 * - Actor Lock-on: Player has locked onto an actor, a reticle appears, both Player and the camera focus on the actor.
 * - Parallel: Player and the camera keep facing the same angle from when Z was pressed. Can snap to walls.
 *             This state occurs when there are no actors available to lock onto.
 *
 * First this function updates `zTargetActiveTimer`. For most Z-Target related states to update, this
 * timer has to have a non-zero value. Additionally, the timer must have a value of 5 or greater
 * for the Attention system to recognize that an actor lock-on is active.
 *
 * Following this, a next lock-on actor is chosen. If there is currently no actor lock-on active, the actor
 * Navi is hovering over will be chosen. If there is an active lock-on, the next available
 * lock-on will be the actor with an arrow hovering above it.
 *
 * If the above regarding actor lock-on does not occur, then Z-Parallel can begin.
 *
 * Lastly, the function handles updating general "actor focus" state. This applies to non Z-Target states
 * like talking to an actor. If the current focus actor is not considered "hostile", then
 * `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS` can be set. This flag being set will trigger `Player_UpdateCamAndSeqModes`
 * to make the camera focus on the current focus actor.
 */
void Player_UpdateZTargeting(Player* this, PlayState* play) {
    s32 ignoreLeash = false;
    s32 zButtonHeld = CHECK_BTN_ALL(sControlInput->cur.button, BTN_Z);
    Actor* nextLockOnActor;
    s32 pad;
    s32 usingHoldTargeting;
    s32 isTalking;

    if (!zButtonHeld) {
        this->stateFlags1 &= ~PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) || (this->csAction != 0) ||
        (this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE)) ||
        (this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT)) {
        // Don't allow Z-Targeting in various states
        this->zTargetActiveTimer = 0;
    } else if (zButtonHeld || (this->stateFlags2 & PLAYER_STATE2_LOCK_ON_WITH_SWITCH) ||
               (this->autoLockOnActor != NULL)) {
        // While a lock-on is active, decrement the timer and hold it at 5.
        // Values under 5 indicate a lock-on has ended and will make the reticle release.
        // See usage toward the end of `Actor_UpdateAll`.
        //
        // `zButtonHeld` will also be true for Parallel. This is necessary because the timer
        // needs to be non-zero for `Player_SetParallel` to be able to run below.
        if (this->zTargetActiveTimer <= 5) {
            this->zTargetActiveTimer = 5;
        } else {
            this->zTargetActiveTimer--;
        }
    } else if (this->stateFlags1 & PLAYER_STATE1_PARALLEL) {
        // If the above code block which checks `zButtonHeld` is not taken, that means Z has been released.
        // In that case, setting `zTargetActiveTimer` to 0 will stop Parallel if it is currently active.
        this->zTargetActiveTimer = 0;
    } else if (this->zTargetActiveTimer != 0) {
        this->zTargetActiveTimer--;
    }

    if (this->zTargetActiveTimer >= 6) {
        // When a lock-on is started, `zTargetActiveTimer` will be set to 15 and then immediately start decrementing
        // down to 5. During this 10 frame period, set `ignoreLeash` so that the lock-on will temporarily
        // have an infinite leash distance.
        // This gives time for the reticle to settle while it locks on, even if the player leaves the leash range.
        ignoreLeash = true;
    }

    isTalking = Player_IsTalking(play);

    if (isTalking || (this->zTargetActiveTimer != 0) ||
        (this->stateFlags1 & (PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_BOOMERANG_THROWN))) {
        if (!isTalking) {
            if (!(this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN) &&
                ((this->heldItemAction != PLAYER_IA_FISHING_POLE) || (this->unk_860 == 0)) &&
                CHECK_BTN_ALL(sControlInput->press.button, BTN_Z)) {

                if (this->actor.category == ACTORCAT_PLAYER) {
                    // The next lock-on actor defaults to the actor Navi is hovering over.
                    // This may change to the arrow hover actor below.
                    nextLockOnActor = play->actorCtx.targetCtx.arrowPointedActor;
                } else {
                    // Dark Link will always lock onto the player.
                    nextLockOnActor = &GET_PLAYER(play)->actor;
                }

                // Get saved Z Target setting.
                // Dark Link uses Hold Targeting.
                usingHoldTargeting = (gSaveContext.zTargetSetting != 0) || (this->actor.category != ACTORCAT_PLAYER);

                this->stateFlags1 |= PLAYER_STATE1_Z_TARGETING;

                if ((nextLockOnActor != NULL) && !(nextLockOnActor->flags & ACTOR_FLAG_NO_LOCKON)) {

                    // Navi hovers over the current lock-on actor, so `nextLockOnActor` and `focusActor`
                    // will be the same if already locked on.
                    // In this case, `nextLockOnActor` will be the arrow hover actor instead.
                    if ((nextLockOnActor == this->focusActor) && (this->actor.category == ACTORCAT_PLAYER)) {
                        nextLockOnActor = play->actorCtx.targetCtx.unk_94;
                    }

                    if (nextLockOnActor != this->focusActor) {
                        // Set new lock-on

                        if (!usingHoldTargeting) {
                            this->stateFlags2 |= PLAYER_STATE2_LOCK_ON_WITH_SWITCH;
                        }

                        this->focusActor = nextLockOnActor;
                        this->zTargetActiveTimer = 15;
                        this->stateFlags2 &= ~(PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER | PLAYER_STATE2_NAVI_ALERT);
                    } else {
                        if (!usingHoldTargeting) {
                            Player_ReleaseLockOn(this);
                        }
                    }

                    this->stateFlags1 &= ~PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE;
                } else {
                    // Lock-on was not started above. Set Parallel Mode.
                    if (!(this->stateFlags1 & (PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE))) {
                        Player_SetParallel(this);
                    }
                }
            }

            if (this->focusActor != NULL) {
                if ((this->actor.category == ACTORCAT_PLAYER) && (this->focusActor != this->autoLockOnActor) &&
                    func_8002F0C8(this->focusActor, this, ignoreLeash)) {
                    Player_ReleaseLockOn(this);
                    this->stateFlags1 |= PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE;
                } else if (this->focusActor != NULL) {
                    this->focusActor->targetPriority = 40;
                }
            } else if (this->autoLockOnActor != NULL) {
                // Because of the previous if condition above, `autoLockOnActor` does not take precedence
                // over `focusActor` if it already exists.
                // However, `autoLockOnActor` is expected to be set with `Player_SetAutoLockOnActor`
                // which will release any existing lock-on before setting the new one.
                this->focusActor = this->autoLockOnActor;
            }
        }

        if (this->focusActor != NULL) {
            this->stateFlags1 &= ~(PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS | PLAYER_STATE1_PARALLEL);

            // Check if an actor is not hostile, aka "friendly", to set `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS`.
            //
            // When carrying another actor, `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS` will be set even if the actor
            // is hostile. This is a special case to allow Player to have more freedom of movement and be able
            // to throw a carried actor at the lock-on actor, even if it is hostile.
            if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) ||
                !CHECK_FLAG_ALL(this->focusActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE)) {
                this->stateFlags1 |= PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS;
            }
        } else {
            if (this->stateFlags1 & PLAYER_STATE1_PARALLEL) {
                this->stateFlags2 &= ~PLAYER_STATE2_LOCK_ON_WITH_SWITCH;
            } else {
                Player_ClearZTargeting(this);
            }
        }
    } else {
        Player_ClearZTargeting(this);
    }
}

/**
 * These defines exist to simplify the variable used to toggle the different speed modes.
 * While the `speedMode` variable is a float and can contain a non-boolean value,
 * `Player_CalcSpeedAndYawFromControlStick` never actually uses the value for anything.
 * It simply checks if the value is non-zero to toggle the "curved" mode.
 * In practice, 0.0f or 0.018f are the only values passed to this function.
 *
 * It's clear that this value was intended to mean something in the curved mode calculation at
 * some point in development, but was either never implemented or removed.
 *
 * To see the difference between linear and curved mode, with interactive toggles for
 * speed cap and floor pitch, see the following desmos graph: https://www.desmos.com/calculator/hri7dcws4c
 */

// Linear mode is a straight line, increasing target speed at a steady rate relative to the control stick magnitude
#define SPEED_MODE_LINEAR 0.0f

// Curved mode drops any input below 20 units of magnitude, resulting in zero for target speed.
// Beyond 20 units, a gradual curve slowly moves up until around the 40 unit mark
// when target speed ramps up very quickly.
#define SPEED_MODE_CURVED 0.018f

/**
 * Calculates target speed and yaw based on input from the control stick.
 * See `Player_GetMovementSpeedAndYaw` for detailed argument descriptions.
 *
 * @return true if the control stick has any magnitude, false otherwise.
 */
s32 Player_CalcSpeedAndYawFromControlStick(PlayState* play, Player* this, f32* outSpeedTarget, s16* outYawTarget,
                                           f32 speedMode) {
    f32 temp;
    f32 sinFloorPitch;
    f32 floorPitchInfluence;
    f32 speedCap;

    if ((this->unk_6AD != 0) || (play->transitionTrigger == TRANS_TRIGGER_START) ||
        (this->stateFlags1 & PLAYER_STATE1_LOADING)) {
        *outSpeedTarget = 0.0f;
        *outYawTarget = this->actor.shape.rot.y;
    } else {
        *outSpeedTarget = sControlStickMagnitude;
        *outYawTarget = sControlStickAngle;

        // The value of `speedMode` is never actually used. It only toggles this condition.
        // See the definition of `SPEED_MODE_LINEAR` and `SPEED_MODE_CURVED` for more information.
        if (speedMode != SPEED_MODE_LINEAR) {
            *outSpeedTarget -= 20.0f;

            if (*outSpeedTarget < 0.0f) {
                // If control stick magnitude is below 20, return zero speed.
                *outSpeedTarget = 0.0f;
            } else {
                // Cosine of the control stick magnitude isn't exactly meaningful, but
                // it happens to give a desirable curve for grounded movement speed relative
                // to control stick magnitude.
                temp = 1.0f - Math_CosS(*outSpeedTarget * 450.0f);
                *outSpeedTarget = (SQ(temp) * 30.0f) + 7.0f;
            }
        } else {
            // Speed increases linearly relative to control stick magnitude
            *outSpeedTarget *= 0.8f;
        }

        if (sControlStickMagnitude != 0.0f) {
            sinFloorPitch = Math_SinS(this->floorPitch);
            speedCap = this->unk_880;
            floorPitchInfluence = CLAMP(sinFloorPitch, 0.0f, 0.6f);

            if (this->unk_6C4 != 0.0f) {
                speedCap -= this->unk_6C4 * 0.008f;
                speedCap = CLAMP_MIN(speedCap, 2.0f);
            }

            *outSpeedTarget = (*outSpeedTarget * 0.14f) - (8.0f * floorPitchInfluence * floorPitchInfluence);
            *outSpeedTarget = CLAMP(*outSpeedTarget, 0.0f, speedCap);

            return true;
        }
    }

    return false;
}

/**
 * Steps speed toward zero to at a rate defined by current boot data.
 * After zero is reached, speed will be held at zero.
 *
 * @return true if speed is 0, false otherwise
 */
s32 Player_DecelerateToZero(Player* this) {
    return Math_StepToF(&this->linearVelocity, 0.0f, REG(43) / 100.0f);
}

/**
 * Gets target speed and yaw values for movement based on control stick input.
 * Control stick magnitude and angle are processed in `Player_CalcSpeedAndYawFromControlStick` to get target values.
 * Additionally, this function does extra processing on the target yaw value if the control stick is neutral.
 *
 * @param outSpeedTarget  a pointer to the variable that will hold the resulting target speed value
 * @param outYawTarget    a pointer to the variable that will hold the resulting target yaw value
 * @param speedMode       toggles between a linear and curved mode for the speed value
 *
 * @see Player_CalcSpeedAndYawFromControlStick for more information on the linear vs curved speed mode.
 *
 * @return true if the control stick has any magnitude, false otherwise.
 */
s32 Player_GetMovementSpeedAndYaw(Player* this, f32* outSpeedTarget, s16* outYawTarget, f32 speedMode,
                                  PlayState* play) {
    if (!Player_CalcSpeedAndYawFromControlStick(play, this, outSpeedTarget, outYawTarget, speedMode)) {
        *outYawTarget = this->actor.shape.rot.y;

        if (this->focusActor != NULL) {
            if ((play->actorCtx.targetCtx.unk_4B != 0) && !(this->stateFlags2 & PLAYER_STATE2_DISABLE_ROTATION_ALWAYS)) {
                *outYawTarget = Math_Vec3f_Yaw(&this->actor.world.pos, &this->focusActor->focus.pos);
                return false;
            }
        } else if (Player_FriendlyLockOnOrParallel(this)) {
            *outYawTarget = this->parallelYaw;
        }

        return false;
    } else {
        *outYawTarget += Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
        return true;
    }
}

typedef enum ActionHandlerIndex {
    /*  0 */ PLAYER_ACTION_HANDLER_0,
    /*  1 */ PLAYER_ACTION_HANDLER_1,
    /*  2 */ PLAYER_ACTION_HANDLER_2,
    /*  3 */ PLAYER_ACTION_HANDLER_3,
    /*  4 */ PLAYER_ACTION_HANDLER_TALK,
    /*  5 */ PLAYER_ACTION_HANDLER_5,
    /*  6 */ PLAYER_ACTION_HANDLER_ROLL,
    /*  7 */ PLAYER_ACTION_HANDLER_7,
    /*  8 */ PLAYER_ACTION_HANDLER_8,
    /*  9 */ PLAYER_ACTION_HANDLER_9,
    /* 10 */ PLAYER_ACTION_HANDLER_10,
    /* 11 */ PLAYER_ACTION_HANDLER_11,
    /* 12 */ PLAYER_ACTION_HANDLER_12,
    /* 13 */ PLAYER_ACTION_HANDLER_13
} ActionHandlerIndex;

static s8 sActionHandlerList1[] = {
    PLAYER_ACTION_HANDLER_13, PLAYER_ACTION_HANDLER_2,  PLAYER_ACTION_HANDLER_TALK, PLAYER_ACTION_HANDLER_9,
    PLAYER_ACTION_HANDLER_10, PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_8,    -PLAYER_ACTION_HANDLER_7,
};

static s8 sActionHandlerList2[] = {
    PLAYER_ACTION_HANDLER_13, PLAYER_ACTION_HANDLER_1,    PLAYER_ACTION_HANDLER_2, PLAYER_ACTION_HANDLER_5,
    PLAYER_ACTION_HANDLER_3,  PLAYER_ACTION_HANDLER_TALK, PLAYER_ACTION_HANDLER_9, PLAYER_ACTION_HANDLER_10,
    PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_7,    PLAYER_ACTION_HANDLER_8, -PLAYER_ACTION_HANDLER_ROLL,
};

static s8 sActionHandlerList3[] = {
    PLAYER_ACTION_HANDLER_13,   PLAYER_ACTION_HANDLER_1, PLAYER_ACTION_HANDLER_2,     PLAYER_ACTION_HANDLER_3,
    PLAYER_ACTION_HANDLER_TALK, PLAYER_ACTION_HANDLER_9, PLAYER_ACTION_HANDLER_10,    PLAYER_ACTION_HANDLER_11,
    PLAYER_ACTION_HANDLER_8,    PLAYER_ACTION_HANDLER_7, -PLAYER_ACTION_HANDLER_ROLL,
};

static s8 sActionHandlerList4[] = {
    PLAYER_ACTION_HANDLER_13, PLAYER_ACTION_HANDLER_2,  PLAYER_ACTION_HANDLER_TALK, PLAYER_ACTION_HANDLER_9,
    PLAYER_ACTION_HANDLER_10, PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_8,    -PLAYER_ACTION_HANDLER_7,
};

static s8 sActionHandlerList5[] = {
    PLAYER_ACTION_HANDLER_13, PLAYER_ACTION_HANDLER_2,  PLAYER_ACTION_HANDLER_TALK,
    PLAYER_ACTION_HANDLER_9,  PLAYER_ACTION_HANDLER_10, PLAYER_ACTION_HANDLER_11,
    PLAYER_ACTION_HANDLER_12, PLAYER_ACTION_HANDLER_8,  -PLAYER_ACTION_HANDLER_7,
};

static s8 sActionHandlerListTurnInPlace[] = {
    -PLAYER_ACTION_HANDLER_7,
};

static s8 sActionHandlerListIdle[] = {
    PLAYER_ACTION_HANDLER_0, PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_1,     PLAYER_ACTION_HANDLER_2,
    PLAYER_ACTION_HANDLER_3, PLAYER_ACTION_HANDLER_5,  PLAYER_ACTION_HANDLER_TALK,  PLAYER_ACTION_HANDLER_9,
    PLAYER_ACTION_HANDLER_8, PLAYER_ACTION_HANDLER_7,  -PLAYER_ACTION_HANDLER_ROLL,
};

static s8 sActionHandlerList8[] = {
    PLAYER_ACTION_HANDLER_0, PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_1, PLAYER_ACTION_HANDLER_2,
    PLAYER_ACTION_HANDLER_3, PLAYER_ACTION_HANDLER_12, PLAYER_ACTION_HANDLER_5, PLAYER_ACTION_HANDLER_TALK,
    PLAYER_ACTION_HANDLER_9, PLAYER_ACTION_HANDLER_8,  PLAYER_ACTION_HANDLER_7, -PLAYER_ACTION_HANDLER_ROLL,
};

static s8 sActionHandlerList9[] = {
    PLAYER_ACTION_HANDLER_13,    PLAYER_ACTION_HANDLER_1,  PLAYER_ACTION_HANDLER_2,    PLAYER_ACTION_HANDLER_3,
    PLAYER_ACTION_HANDLER_12,    PLAYER_ACTION_HANDLER_5,  PLAYER_ACTION_HANDLER_TALK, PLAYER_ACTION_HANDLER_9,
    PLAYER_ACTION_HANDLER_10,    PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_8,    PLAYER_ACTION_HANDLER_7,
    -PLAYER_ACTION_HANDLER_ROLL,
};

static s8 sActionHandlerList10[] = {
    PLAYER_ACTION_HANDLER_10,
    PLAYER_ACTION_HANDLER_8,
    -PLAYER_ACTION_HANDLER_7,
};

static s8 sActionHandlerList11[] = {
    PLAYER_ACTION_HANDLER_0,
    PLAYER_ACTION_HANDLER_12,
    PLAYER_ACTION_HANDLER_5,
    -PLAYER_ACTION_HANDLER_TALK,
};

s32 Player_ActionHandler_0(Player* this, PlayState* play);
s32 Player_ActionHandler_1(Player* this, PlayState* play);
s32 Player_ActionHandler_2(Player* this, PlayState* play);
s32 Player_ActionHandler_3(Player* this, PlayState* play);
s32 Player_ActionHandler_Talk(Player* this, PlayState* play);
s32 Player_ActionHandler_5(Player* this, PlayState* play);
s32 Player_ActionHandler_Roll(Player* this, PlayState* play);
s32 Player_ActionHandler_7(Player* this, PlayState* play);
s32 Player_ActionHandler_8(Player* this, PlayState* play);
s32 Player_ActionHandler_9(Player* this, PlayState* play);
s32 Player_ActionHandler_10(Player* this, PlayState* play);
s32 Player_ActionHandler_11(Player* this, PlayState* play);
s32 Player_ActionHandler_12(Player* this, PlayState* play);
s32 Player_ActionHandler_13(Player* this, PlayState* play);

static s32 (*sActionHandlerFuncs[])(Player* this, PlayState* play) = {
    Player_ActionHandler_0,    // PLAYER_ACTION_HANDLER_0
    Player_ActionHandler_1,    // PLAYER_ACTION_HANDLER_1
    Player_ActionHandler_2,    // PLAYER_ACTION_HANDLER_2
    Player_ActionHandler_3,    // PLAYER_ACTION_HANDLER_3
    Player_ActionHandler_Talk, // PLAYER_ACTION_HANDLER_TALK
    Player_ActionHandler_5,    // PLAYER_ACTION_HANDLER_5
    Player_ActionHandler_Roll, // PLAYER_ACTION_HANDLER_ROLL
    Player_ActionHandler_7,    // PLAYER_ACTION_HANDLER_7
    Player_ActionHandler_8,    // PLAYER_ACTION_HANDLER_8
    Player_ActionHandler_9,    // PLAYER_ACTION_HANDLER_9
    Player_ActionHandler_10,   // PLAYER_ACTION_HANDLER_10
    Player_ActionHandler_11,   // PLAYER_ACTION_HANDLER_11
    Player_ActionHandler_12,   // PLAYER_ACTION_HANDLER_12
    Player_ActionHandler_13,   // PLAYER_ACTION_HANDLER_13
};

/**
 * This function processes "Action Handler Lists".
 *
 * An Action Handler is a function that "listens" for certain conditions or the right time
 * to change to a certain action. These can include actions triggered manually by the player
 * or actions that happen automatically, given some other condition(s).
 *
 * Action Handler Lists are a list of indices for the `sActionHandlerFuncs` array.
 * The Action Handlers are ran in order until one of them returns true, or the end of the list is reached.
 * An Action Handler index having a negative value indicates that it is the last member in the list.
 *
 * Because these lists are processed sequentially, the order of the indices in the list
 * determines an Action Handler's priority.
 *
 * If the `updateUpperBody` argument is true, Player's upper body will update before the Action Handler List
 * is processed. This allows for Item Action functions to run, for example.
 *
 * @return true if a new action has been chosen
 *
 */
s32 Player_TryActionHandlerList(PlayState* play, Player* this, s8* actionHandlerList, s32 updateUpperBody) {
    s32 i;

    if (!(this->stateFlags1 & (PLAYER_STATE1_LOADING | PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE))) {
        if (updateUpperBody) {
            sUpperBodyIsBusy = Player_UpdateUpperBody(this, play);

            if (Player_Action_8084E604 == this->actionFunc) {
                return true;
            }
        }

        if (func_8008F128(this)) {
            this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_UPPER_X;
            return true;
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM) &&
            (Player_UpperAction_ChangeHeldItem != this->upperActionFunc)) {
            // Process all entries in the Action Handler List with a positive index
            while (*actionHandlerList >= 0) {
                if (sActionHandlerFuncs[*actionHandlerList](this, play)) {
                    return true;
                }
                actionHandlerList++;
            }

            // Try the last entry in the list. Negate the index to make it positive again.
            if (sActionHandlerFuncs[-(*actionHandlerList)](this, play)) {
                return true;
            }
        }
    }

    return false;
}

typedef enum PlayerActionInterruptResult {
    /* -1 */ PLAYER_INTERRUPT_NONE = -1,
    /*  0 */ PLAYER_INTERRUPT_NEW_ACTION,
    /*  1 */ PLAYER_INTERRUPT_MOVE
} PlayerActionInterruptResult;

/**
 * An Action Interrupt allows for ending an action early, toward the end of an animation.
 *
 * First, `sActionHandlerListIdle` will be checked to see if any of those actions should be used.
 * It should be noted that the `updateUpperBody` argument passed to `Player_TryActionHandlerList`
 * is `true`. This means that an item can be used during the interrupt window.
 *
 * If no actions from the Action Handler List are used, then the control stick is checked to see if
 * any movement should occur.
 *
 * Note that while this function can set up a new action with `sActionHandlerListIdle`, this function
 * will not set up an appropriate action for moving.
 * It is the callers responsibility to react accordingly to `PLAYER_INTERRUPT_MOVE`.
 *
 * @param frameRange  The number of frames, from the end of the current animation, where an interrupt can occur.
 * @return The interrupt result. See `PlayerActionInterruptResult`.
 */
s32 Player_TryActionInterrupt(PlayState* play, Player* this, SkelAnime* skelAnime, f32 frameRange) {
    f32 speedTarget;
    s16 yawTarget;

    if ((skelAnime->endFrame - frameRange) <= skelAnime->curFrame) {
        if (Player_TryActionHandlerList(play, this, sActionHandlerListIdle, true)) {
            return PLAYER_INTERRUPT_NEW_ACTION;
        }

        if (Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play)) {
            return PLAYER_INTERRUPT_MOVE;
        }
    }

    return PLAYER_INTERRUPT_NONE;
}

void func_80837530(PlayState* play, Player* this, s32 arg2) {
    if (arg2 != 0) {
        this->unk_858 = 0.0f;
    } else {
        this->unk_858 = 0.5f;
    }

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (this->actor.category == ACTORCAT_PLAYER) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_M_THUNDER, this->bodyPartsPos[PLAYER_BODYPART_WAIST].x,
                    this->bodyPartsPos[PLAYER_BODYPART_WAIST].y, this->bodyPartsPos[PLAYER_BODYPART_WAIST].z, 0, 0, 0,
                    Player_GetMeleeWeaponHeld(this) | arg2, true);
    }
}

s32 Player_CanSpinAttack(Player* this) {
    s8 sp3C[4];
    s8* iter;
    s8* iter2;
    s8 temp1;
    s8 temp2;
    s32 i;

    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) || Player_HoldsBrokenKnife(this)) {
        return false;
    }

    iter = &this->controlStickSpinAngles[0];
    iter2 = &sp3C[0];

    for (i = 0; i < 4; i++, iter++, iter2++) {
        if ((*iter2 = *iter) < 0) {
            return false;
        }

        *iter2 *= 2;
    }

    temp1 = sp3C[0] - sp3C[1];

    if (ABS(temp1) < 10) {
        return false;
    }

    iter2 = &sp3C[1];

    for (i = 1; i < 3; i++, iter2++) {
        temp2 = *iter2 - *(iter2 + 1);

        if ((ABS(temp2) < 10) || (temp2 * temp1 < 0)) {
            return false;
        }
    }

    return true;
}

void func_80837704(PlayState* play, Player* this) {
    LinkAnimationHeader* anim;

    if ((this->meleeWeaponAnimation >= PLAYER_MWA_RIGHT_SLASH_1H) &&
        (this->meleeWeaponAnimation <= PLAYER_MWA_RIGHT_COMBO_2H)) {
        anim = D_80854358[Player_HoldsTwoHandedWeapon(this)];
    } else {
        anim = D_80854350[Player_HoldsTwoHandedWeapon(this)];
    }

    func_80832318(this);
    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 8.0f, Animation_GetLastFrame(anim), ANIMMODE_ONCE, -9.0f);
    func_80837530(play, this, 0x200);
}

void func_808377DC(PlayState* play, Player* this) {
    Player_SetupAction(play, this, Player_Action_80844E68, 1);
    func_80837704(play, this);
}

static s8 D_80854480[] = {
    PLAYER_MWA_STAB_1H,
    PLAYER_MWA_RIGHT_SLASH_1H,
    PLAYER_MWA_RIGHT_SLASH_1H,
    PLAYER_MWA_LEFT_SLASH_1H,
};

static s8 D_80854484[] = {
    PLAYER_MWA_HAMMER_FORWARD,
    PLAYER_MWA_HAMMER_SIDE,
    PLAYER_MWA_HAMMER_FORWARD,
    PLAYER_MWA_HAMMER_SIDE,
};

s32 func_80837818(Player* this) {
    s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];
    s32 sp18;

    if (this->heldItemAction == PLAYER_IA_HAMMER) {
        if (controlStickDirection <= PLAYER_STICK_DIR_NONE) {
            controlStickDirection = PLAYER_STICK_DIR_FORWARD;
        }

        sp18 = D_80854484[controlStickDirection];
        this->unk_845 = 0;
    } else {
        if (Player_CanSpinAttack(this)) {
            sp18 = PLAYER_MWA_SPIN_ATTACK_1H;
        } else {
            if (controlStickDirection <= PLAYER_STICK_DIR_NONE) {
                if (Player_IsZTargeting(this)) {
                    sp18 = PLAYER_MWA_FORWARD_SLASH_1H;
                } else {
                    sp18 = PLAYER_MWA_RIGHT_SLASH_1H;
                }
            } else {
                sp18 = D_80854480[controlStickDirection];

                if (sp18 == PLAYER_MWA_STAB_1H) {
                    this->stateFlags2 |= PLAYER_STATE2_SWORD_LUNGE;

                    if (!Player_IsZTargeting(this)) {
                        sp18 = PLAYER_MWA_FORWARD_SLASH_1H;
                    }
                }
            }

            if (this->heldItemAction == PLAYER_IA_DEKU_STICK) {
                sp18 = PLAYER_MWA_FORWARD_SLASH_1H;
            }
        }

        if (Player_HoldsTwoHandedWeapon(this)) {
            sp18++;
        }
    }

    return sp18;
}

void func_80837918(Player* this, s32 quadIndex, u32 dmgFlags) {
    this->meleeWeaponQuads[quadIndex].info.toucher.dmgFlags = dmgFlags;

    if (dmgFlags == 2) {
        this->meleeWeaponQuads[quadIndex].info.toucherFlags = TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_WOOD;
    } else {
        this->meleeWeaponQuads[quadIndex].info.toucherFlags = TOUCH_ON | TOUCH_NEAREST;
    }
}

static u32 D_80854488[][2] = {
    { 0x00000200, 0x08000000 }, { 0x00000100, 0x02000000 }, { 0x00000400, 0x04000000 },
    { 0x00000002, 0x08000000 }, { 0x00000040, 0x40000000 },
};

void func_80837948(PlayState* play, Player* this, s32 arg2) {
    s32 pad;
    u32 dmgFlags;
    s32 temp;

    Player_SetupAction(play, this, Player_Action_808502D0, 0);
    this->unk_844 = 8;
    if (!((arg2 >= PLAYER_MWA_FLIPSLASH_FINISH) && (arg2 <= PLAYER_MWA_JUMPSLASH_FINISH))) {
        func_80832318(this);
    }

    if ((arg2 != this->meleeWeaponAnimation) || !(this->unk_845 < 3)) {
        this->unk_845 = 0;
    }

    this->unk_845++;
    if (this->unk_845 >= 3) {
        arg2 += 2;
    }

    this->meleeWeaponAnimation = arg2;

    Player_AnimPlayOnceAdjusted(play, this, D_80854190[arg2].unk_00);
    if ((arg2 != PLAYER_MWA_FLIPSLASH_START) && (arg2 != PLAYER_MWA_JUMPSLASH_START)) {
        Player_StartAnimMovement(play, this, 0x209);
    }

    this->yaw = this->actor.shape.rot.y;

    if (Player_HoldsBrokenKnife(this)) {
        temp = 1;
    } else {
        temp = Player_GetMeleeWeaponHeld(this) - 1;
    }

    if ((arg2 >= PLAYER_MWA_FLIPSLASH_START) && (arg2 <= PLAYER_MWA_JUMPSLASH_FINISH)) {
        if (CVarGetInteger(CVAR_GENERAL("RestoreQPA"), 1) && temp == -1) {
            dmgFlags = 0x16171617;
        }
        else {
            dmgFlags = D_80854488[temp][1];
        }
    } else {
        dmgFlags = D_80854488[temp][0];
    }

    func_80837918(this, 0, dmgFlags);
    func_80837918(this, 1, dmgFlags);
}

/**
 * Gives the player intangibility frames. Used for when the player takes damage.
 *
 * If the player is already intangible, it will be overridden by the new intangibility duration.
 * If the player is already invunerable, no intangibility will be applied.
 *
 * @param timer must be a positive value representing the number of intangibility frames.
 * @note Intangibility prevents taking damage and responses to damage like knockback, while invulnerability only
 * prevents taking damage.
 */
void Player_SetIntangibility(Player* this, s32 timer) {
    if (this->invincibilityTimer >= 0) {
        this->invincibilityTimer = timer;
        this->damageFlickerAnimCounter = 0;
    }
}

/**
 * Gives the player invulnerability frames. Used for when the player performs a dodging maneuver like a roll.
 *
 * If the player is already intangible, they will become invulnerable instead.
 * If the player is already invulnerable, the longer of the two invulnerability periods is kept.
 *
 * @param timer must be a negative value representing the number of invulnerability frames.
 * @note Intangibility prevents taking damage and responses to damage like knockback, while invulnerability only
 * prevents taking damage.
 */
void Player_SetInvulnerability(Player* this, s32 timer) {
    if (this->invincibilityTimer > timer) {
        this->invincibilityTimer = timer;
    }
    this->damageFlickerAnimCounter = 0;
}

s32 func_80837B18_modified(PlayState* play, Player* this, s32 damage, u8 modified) {
    if ((this->invincibilityTimer != 0) || (this->actor.category != ACTORCAT_PLAYER)) {
        return 1;
    }

    s32 modifiedDamage = damage;
    if (modified) {
        modifiedDamage *= (1 << CVarGetInteger(CVAR_ENHANCEMENT("DamageMult"), 0));
    }

    return Health_ChangeBy(play, modifiedDamage);
}

s32 func_80837B18(PlayState* play, Player* this, s32 damage) {
    return func_80837B18_modified(play, this, damage, true);
}

void func_80837B60(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
    Player_ApplyAnimMovementScaledByAge(this, 3);
}

void func_80837B9C(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_8084411C, 0);
    Player_AnimPlayLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
    this->av2.actionVar2 = 1;
    if (this->unk_6AD != 3) {
        this->unk_6AD = 0;
    }
}

static LinkAnimationHeader* D_808544B0[] = {
    &gPlayerAnim_link_normal_front_shit, &gPlayerAnim_link_normal_front_shitR, &gPlayerAnim_link_normal_back_shit,
    &gPlayerAnim_link_normal_back_shitR, &gPlayerAnim_link_normal_front_hit,   &gPlayerAnim_link_anchor_front_hitR,
    &gPlayerAnim_link_normal_back_hit,   &gPlayerAnim_link_anchor_back_hitR,
};

void func_80837C0C(PlayState* play, Player* this, s32 damageResponseType, f32 speed, f32 yVelocity, s16 yRot,
                   s32 invincibilityTimer) {
    LinkAnimationHeader* anim = NULL;
    LinkAnimationHeader** sp28;

    if (this->stateFlags1 & PLAYER_STATE1_HANGING_OFF_LEDGE) {
        func_80837B60(this);
    }

    this->unk_890 = 0;

    Player_PlaySfx(this, NA_SE_PL_DAMAGE);

    if (!func_80837B18(play, this, 0 - this->actor.colChkInfo.damage)) {
        this->stateFlags2 &= ~PLAYER_STATE2_GRABBED_BY_ENEMY;
        if (!(this->actor.bgCheckFlags & 1) && !(this->stateFlags1 & PLAYER_STATE1_IN_WATER)) {
            func_80837B9C(this, play);
        }
        return;
    }

    Player_SetIntangibility(this, invincibilityTimer);

    if (damageResponseType == PLAYER_HIT_RESPONSE_ICE_TRAP) {
        Player_SetupAction(play, this, Player_Action_8084FB10, 0);

        anim = &gPlayerAnim_link_normal_ice_down;

        func_80832224(this);
        Player_RequestRumble(this, 255, 10, 40, 0);

        Player_PlaySfx(this, NA_SE_PL_FREEZE_S);
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_FREEZE);
    } else if (damageResponseType == PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK) {
        Player_SetupAction(play, this, Player_Action_8084FBF4, 0);

        Player_RequestRumble(this, 255, 80, 150, 0);

        Player_AnimPlayLoopAdjusted(play, this, &gPlayerAnim_link_normal_electric_shock);
        func_80832224(this);

        this->av2.actionVar2 = 20;
    } else {
        yRot -= this->actor.shape.rot.y;
        if (this->stateFlags1 & PLAYER_STATE1_IN_WATER) {
            Player_SetupAction(play, this, Player_Action_8084E30C, 0);
            Player_RequestRumble(this, 180, 20, 50, 0);

            this->linearVelocity = 4.0f;
            this->actor.velocity.y = 0.0f;

            anim = &gPlayerAnim_link_swimer_swim_hit;

            Player_PlayVoiceSfx(this, NA_SE_VO_LI_DAMAGE_S);
        } else if ((damageResponseType == PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE) ||
                   (damageResponseType == PLAYER_HIT_RESPONSE_KNOCKBACK_SMALL) ||
                   !(this->actor.bgCheckFlags & 1) ||
                   (this->stateFlags1 & (PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_CLIMBING_LADDER))) {
            Player_SetupAction(play, this, Player_Action_8084377C, 0);

            this->stateFlags3 |= PLAYER_STATE3_MIDAIR;

            Player_RequestRumble(this, 255, 20, 150, 0);
            func_80832224(this);

            if (damageResponseType == PLAYER_HIT_RESPONSE_KNOCKBACK_SMALL) {
                this->av2.actionVar2 = 4;

                this->actor.speedXZ = 3.0f;
                this->linearVelocity = 3.0f;
                this->actor.velocity.y = 6.0f;

                Player_AnimChangeFreeze(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_damage_run, this->modelAnimType));
                Player_PlayVoiceSfx(this, NA_SE_VO_LI_DAMAGE_S);
            } else {
                this->actor.speedXZ = speed;
                this->linearVelocity = speed;
                this->actor.velocity.y = yVelocity;

                if (ABS(yRot) > 0x4000) {
                    anim = &gPlayerAnim_link_normal_front_downA;
                } else {
                    anim = &gPlayerAnim_link_normal_back_downA;
                }

                if ((this->actor.category != ACTORCAT_PLAYER) && (this->actor.colChkInfo.health == 0)) {
                    Player_PlayVoiceSfx(this, NA_SE_VO_BL_DOWN);
                } else {
                    Player_PlayVoiceSfx(this, NA_SE_VO_LI_FALL_L);
                }
            }

            this->hoverBootsTimer = 0;
            this->actor.bgCheckFlags &= ~1;
        } else {
            if ((this->linearVelocity > 4.0f) && !Player_CheckHostileLockOn(this)) {
                this->unk_890 = 20;
                Player_RequestRumble(this, 120, 20, 10, 0);
                Player_PlayVoiceSfx(this, NA_SE_VO_LI_DAMAGE_S);
                return;
            }

            sp28 = D_808544B0;

            Player_SetupAction(play, this, Player_Action_8084370C, 0);
            func_80833C3C(this);

            if (this->actor.colChkInfo.damage < 5) {
                Player_RequestRumble(this, 120, 20, 10, 0);
            } else {
                Player_RequestRumble(this, 180, 20, 100, 0);
                this->linearVelocity = 23.0f;
                sp28 += 4;
            }

            if (ABS(yRot) <= 0x4000) {
                sp28 += 2;
            }

            if (Player_CheckHostileLockOn(this)) {
                sp28 += 1;
            }

            anim = *sp28;

            Player_PlayVoiceSfx(this, NA_SE_VO_LI_DAMAGE_S);
        }

        this->actor.shape.rot.y += yRot;
        this->yaw = this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (ABS(yRot) > 0x4000) {
            this->actor.shape.rot.y += 0x8000;
        }
    }

    func_80832564(play, this);

    this->stateFlags1 |= PLAYER_STATE1_DAMAGED;

    if (anim != NULL) {
        Player_AnimPlayOnceAdjusted(play, this, anim);
    }
}

s32 func_80838144(s32 arg0) {
    s32 temp = arg0 - 2;

    if ((temp >= 0) && (temp < 2)) {
        return temp;
    } else {
        return -1;
    }
}

int func_8083816C(s32 arg0) {
    return (arg0 == 4) || (arg0 == 7) || (arg0 == 12);
}

void func_8083819C(Player* this, PlayState* play) {
    if (this->currentShield == PLAYER_SHIELD_DEKU && (CVarGetInteger(CVAR_CHEAT("FireproofDekuShield"), 0) == 0)) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_SHIELD, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, 1, true);
        Inventory_DeleteEquipment(play, EQUIP_TYPE_SHIELD);
        Message_StartTextbox(play, 0x305F, NULL);
    }
}

void func_8083821C(Player* this) {
    s32 i;

    // clang-format off
    for (i = 0; i < PLAYER_BODYPART_MAX; i++) { this->bodyFlameTimers[i] = Rand_S16Offset(0, 200); }
    // clang-format on

    this->bodyIsBurning = true;
}

void func_80838280(Player* this) {
    if (this->actor.colChkInfo.acHitEffect == 1) {
        func_8083821C(this);
    }
    Player_PlayVoiceSfx(this, NA_SE_VO_LI_FALL_L);
}

void func_808382BC(Player* this) {
    if ((this->invincibilityTimer >= 0) && (this->invincibilityTimer < 20)) {
        this->invincibilityTimer = 20;
    }
}

s32 func_808382DC(Player* this, PlayState* play) {
    s32 pad;
    s32 sp68 = false;
    s32 sp64;

    if (this->unk_A86 != 0) {
        if (!Player_InBlockingCsMode(play, this)) {
            Player_InflictDamageModified(play, -16 * (1 << CVarGetInteger(CVAR_ENHANCEMENT("VoidDamageMult"), 0)), false);
            this->unk_A86 = 0;
        }
    } else {
        sp68 = ((Player_GetHeight(this) - 8.0f) < (this->unk_6C4 * this->actor.scale.y));

        if (sp68 || (this->actor.bgCheckFlags & 0x100) || (sFloorType == 9) || (this->stateFlags2 & PLAYER_STATE2_FORCED_VOID_OUT)) {
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_DAMAGE_S);

            if (sp68) {
                Play_TriggerRespawn(play);
                Scene_SetTransitionForNextEntrance(play);
            } else {
                // Special case for getting crushed in Forest Temple's Checkboard Ceiling Hall or Shadow Temple's
                // Falling Spike Trap Room, to respawn the player in a specific place
                if (((play->sceneNum == SCENE_FOREST_TEMPLE) && (play->roomCtx.curRoom.num == 15)) ||
                    ((play->sceneNum == SCENE_SHADOW_TEMPLE) && (play->roomCtx.curRoom.num == 10))) {
                    static SpecialRespawnInfo checkboardCeilingRespawn = { { 1992.0f, 403.0f, -3432.0f }, 0 };
                    static SpecialRespawnInfo fallingSpikeTrapRespawn = { { 1200.0f, -1343.0f, 3850.0f }, 0 };
                    SpecialRespawnInfo* respawnInfo;

                    if (play->sceneNum == SCENE_FOREST_TEMPLE) {
                        respawnInfo = &checkboardCeilingRespawn;
                    } else {
                        respawnInfo = &fallingSpikeTrapRespawn;
                    }

                    Play_SetupRespawnPoint(play, RESPAWN_MODE_DOWN, 0xDFF);
                    gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = respawnInfo->pos;
                    gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = respawnInfo->yaw;
                }

                Play_TriggerVoidOut(play);
            }

            Player_PlayVoiceSfx(this, NA_SE_VO_LI_TAKEN_AWAY);
            play->unk_11DE9 = 1;
            func_80078884(NA_SE_OC_ABYSS);
        } else if ((this->knockbackType != PLAYER_KNOCKBACK_NONE) &&
                   ((this->knockbackType >= PLAYER_KNOCKBACK_LARGE) || (this->invincibilityTimer == 0))) {
            u8 knockbackResponse[] = {
                PLAYER_HIT_RESPONSE_KNOCKBACK_SMALL,
                PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE,
                PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE,
            };

            func_80838280(this);

            if (this->knockbackType == PLAYER_KNOCKBACK_LARGE_SHOCK) {
                this->bodyShockTimer = 40;
            }

            this->actor.colChkInfo.damage += this->knockbackDamage;
            func_80837C0C(play, this, knockbackResponse[this->knockbackType - 1], this->knockbackSpeed,
                          this->knockbackYVelocity, this->knockbackRot, 20);
        } else {
            sp64 = (this->shieldQuad.base.acFlags & AC_BOUNCED) != 0;

            //! @bug The second set of conditions here seems intended as a way for Link to "block" hits by rolling.
            // However, `Collider.atFlags` is a byte so the flag check at the end is incorrect and cannot work.
            // Additionally, `Collider.atHit` can never be set while already colliding as AC, so it's also bugged.
            // This behavior was later fixed in MM, most likely by removing both the `atHit` and `atFlags` checks.
            if (sp64 || ((this->invincibilityTimer < 0) && (this->cylinder.base.acFlags & AC_HIT) &&
                         (this->cylinder.info.atHit != NULL) && (this->cylinder.info.atHit->atFlags & 0x20000000))) {

                Player_RequestRumble(this, 180, 20, 100, 0);

                if (!Player_IsChildWithHylianShield(this)) {
                    if (this->invincibilityTimer >= 0) {
                        LinkAnimationHeader* anim;
                        s32 sp54 = Player_Action_80843188 == this->actionFunc;

                        if (!func_808332B8(this)) {
                            Player_SetupAction(play, this, Player_Action_808435C4, 0);
                        }

                        if (!(this->av1.actionVar1 = sp54)) {
                            Player_SetUpperActionFunc(this, func_80834BD4);

                            if (this->unk_870 < 0.5f) {
                                anim = D_808543BC[Player_HoldsTwoHandedWeapon(this) &&
                                                  !(CVarGetInteger(CVAR_CHEAT("ShieldTwoHanded"), 0) &&
                                                    (this->heldItemAction != PLAYER_IA_DEKU_STICK))];
                            } else {
                                anim = D_808543B4[Player_HoldsTwoHandedWeapon(this) &&
                                                  !(CVarGetInteger(CVAR_CHEAT("ShieldTwoHanded"), 0) &&
                                                    (this->heldItemAction != PLAYER_IA_DEKU_STICK))];
                            }
                            LinkAnimation_PlayOnce(play, &this->upperSkelAnime, anim);
                        } else {
                            Player_AnimPlayOnce(play, this,
                                          D_808543C4[Player_HoldsTwoHandedWeapon(this) &&
                                                     !(CVarGetInteger(CVAR_CHEAT("ShieldTwoHanded"), 0) &&
                                                       (this->heldItemAction != PLAYER_IA_DEKU_STICK))]);
                        }
                    }

                    if (!(this->stateFlags1 & (PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_CLIMBING_LADDER))) {
                        this->linearVelocity = -18.0f;
                        this->yaw = this->actor.shape.rot.y;
                    }
                }

                if (sp64 && (this->shieldQuad.info.acHitInfo->toucher.effect == 1)) {
                    func_8083819C(this, play);
                }

                return 0;
            }

            if ((this->unk_A87 != 0) || (this->invincibilityTimer > 0) || (this->stateFlags1 & PLAYER_STATE1_DAMAGED) ||
                (this->csAction != 0) || (this->meleeWeaponQuads[0].base.atFlags & AT_HIT) ||
                (this->meleeWeaponQuads[1].base.atFlags & AT_HIT)) {
                return 0;
            }

            if (this->cylinder.base.acFlags & AC_HIT) {
                Actor* ac = this->cylinder.base.ac;
                s32 sp4C;

                if (ac->flags & ACTOR_FLAG_PLAY_HIT_SFX) {
                    Player_PlaySfx(this, NA_SE_PL_BODY_HIT);
                }

                if (this->stateFlags1 & PLAYER_STATE1_IN_WATER) {
                    sp4C = PLAYER_HIT_RESPONSE_NONE;
                } else if (this->actor.colChkInfo.acHitEffect == 2) {
                    sp4C = PLAYER_HIT_RESPONSE_ICE_TRAP;
                } else if (this->actor.colChkInfo.acHitEffect == 3) {
                    sp4C = PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK;
                } else if (this->actor.colChkInfo.acHitEffect == 4) {
                    sp4C = PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE;
                } else {
                    func_80838280(this);
                    sp4C = PLAYER_HIT_RESPONSE_NONE;
                }

                func_80837C0C(play, this, sp4C, 4.0f, 5.0f, Actor_WorldYawTowardActor(ac, &this->actor), 20);
            } else if (this->invincibilityTimer != 0) {
                return 0;
            } else {
                static u8 D_808544F4[] = { 120, 60 };
                s32 sp48 = func_80838144(sFloorType);

                if (((this->actor.wallPoly != NULL) &&
                     SurfaceType_IsWallDamage(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) ||
                    ((sp48 >= 0) &&
                     SurfaceType_IsWallDamage(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) &&
                     (this->floorTypeTimer >= D_808544F4[sp48])) ||
                    ((sp48 >= 0) && ((this->currentTunic != PLAYER_TUNIC_GORON && CVarGetInteger(CVAR_CHEAT("SuperTunic"), 0) == 0) ||
                                     (this->floorTypeTimer >= D_808544F4[sp48])))) {
                    this->floorTypeTimer = 0;
                    this->actor.colChkInfo.damage = 4;
                    func_80837C0C(play, this, PLAYER_HIT_RESPONSE_NONE, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
                } else {
                    return 0;
                }
            }
        }
    }

    return 1;
}

void func_80838940(Player* this, LinkAnimationHeader* anim, f32 arg2, PlayState* play, u16 sfxId) {
    Player_SetupAction(play, this, Player_Action_8084411C, 1);

    if (anim != NULL) {
        Player_AnimPlayOnceAdjusted(play, this, anim);
    }

    this->actor.velocity.y = arg2 * sWaterSpeedFactor;
    this->hoverBootsTimer = 0;
    this->actor.bgCheckFlags &= ~1;

    Player_PlayJumpingSfx(this);
    Player_PlayVoiceSfx(this, sfxId);

    this->stateFlags1 |= PLAYER_STATE1_JUMPING;
}

void func_808389E8(Player* this, LinkAnimationHeader* anim, f32 arg2, PlayState* play) {
    func_80838940(this, anim, arg2, play, NA_SE_VO_LI_SWORD_N);
}

s32 Player_ActionHandler_12(Player* this, PlayState* play) {
    s32 sp3C;
    LinkAnimationHeader* anim;
    f32 sp34;
    f32 temp;
    f32 wallPolyNormalX;
    f32 wallPolyNormalZ;
    f32 sp24;

    if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->ledgeClimbType >= 2) &&
        (!(this->stateFlags1 & PLAYER_STATE1_IN_WATER) || (this->ageProperties->unk_14 > this->yDistToLedge))) {
        sp3C = 0;

        if (func_808332B8(this)) {
            if (this->actor.yDistToWater < 50.0f) {
                if ((this->ledgeClimbType < 2) || (this->yDistToLedge > this->ageProperties->unk_10)) {
                    return 0;
                }
            } else if ((this->currentBoots != PLAYER_BOOTS_IRON) || (this->ledgeClimbType > 2)) {
                return 0;
            }
        } else if (!(this->actor.bgCheckFlags & 1) ||
                   ((this->ageProperties->unk_14 <= this->yDistToLedge) && (this->stateFlags1 & PLAYER_STATE1_IN_WATER))) {
            return 0;
        }

        if ((this->actor.wallBgId != BGCHECK_SCENE) && (sTouchedWallFlags & 0x40)) {
            if (this->ledgeClimbDelayTimer >= 6) {
                this->stateFlags2 |= PLAYER_STATE2_DO_ACTION_CLIMB;
                if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
                    sp3C = 1;
                }
            }
        } else if ((this->ledgeClimbDelayTimer >= 6) || CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
            sp3C = 1;
        }

        if (sp3C != 0) {
            Player_SetupAction(play, this, Player_Action_80845668, 0);

            this->stateFlags1 |= PLAYER_STATE1_JUMPING;

            sp34 = this->yDistToLedge;

            if (this->ageProperties->unk_14 <= sp34) {
                anim = &gPlayerAnim_link_normal_250jump_start;
                this->linearVelocity = 1.0f;
            } else {
                wallPolyNormalX = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.x);
                wallPolyNormalZ = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.z);
                sp24 = this->distToInteractWall + 0.5f;

                this->stateFlags1 |= PLAYER_STATE1_CLIMBING_LEDGE;

                if (func_808332B8(this)) {
                    anim = &gPlayerAnim_link_swimer_swim_15step_up;
                    sp34 -= (60.0f * this->ageProperties->unk_08);
                    this->stateFlags1 &= ~PLAYER_STATE1_IN_WATER;
                } else if (this->ageProperties->unk_18 <= sp34) {
                    anim = &gPlayerAnim_link_normal_150step_up;
                    sp34 -= (59.0f * this->ageProperties->unk_08);
                } else {
                    anim = &gPlayerAnim_link_normal_100step_up;
                    sp34 -= (41.0f * this->ageProperties->unk_08);
                }

                this->actor.shape.yOffset -= sp34 * 100.0f;

                this->actor.world.pos.x -= sp24 * wallPolyNormalX;
                this->actor.world.pos.y += this->yDistToLedge;
                this->actor.world.pos.z -= sp24 * wallPolyNormalZ;

                func_80832224(this);
            }

            this->actor.bgCheckFlags |= 1;

            LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, 1.3f);
            AnimationContext_DisableQueue(play);

            this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;

            return 1;
        }
    } else if ((this->actor.bgCheckFlags & 1) && (this->ledgeClimbType == 1) && (this->ledgeClimbDelayTimer >= 3)) {
        temp = (this->yDistToLedge * 0.08f) + 5.5f;
        func_808389E8(this, &gPlayerAnim_link_normal_jump, temp, play);
        this->linearVelocity = 2.5f;

        return 1;
    }

    return 0;
}

void func_80838E70(PlayState* play, Player* this, f32 arg2, s16 arg3) {
    Player_SetupAction(play, this, Player_Action_80845CA4, 0);
    func_80832440(play, this);

    this->av1.actionVar1 = 1;
    this->av2.actionVar2 = 1;

    this->unk_450.x = (Math_SinS(arg3) * arg2) + this->actor.world.pos.x;
    this->unk_450.z = (Math_CosS(arg3) * arg2) + this->actor.world.pos.z;

    Player_AnimPlayOnce(play, this, Player_GetIdleAnim(this));
}

void func_80838F18(PlayState* play, Player* this) {
    Player_SetupAction(play, this, Player_Action_8084D610, 0);
    Player_AnimChangeLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
}

void func_80838F5C(PlayState* play, Player* this) {
    Player_SetupAction(play, this, Player_Action_8084F88C, 0);

    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_FLOOR_DISABLED;

    Camera_ChangeSetting(Play_GetCamera(play, 0), CAM_SET_FREE0);
}

s32 func_80838FB8(PlayState* play, Player* this) {
    if ((play->transitionTrigger == TRANS_TRIGGER_OFF) && (this->stateFlags1 & PLAYER_STATE1_FLOOR_DISABLED)) {
        func_80838F5C(play, this);
        Player_AnimPlayLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_FALL_S);
        func_800788CC(NA_SE_OC_SECRET_WARP_IN);
        return 1;
    }

    return 0;
}

/**
 * The actual entrances each "return entrance" value can map to.
 * This is used by scenes that are shared between locations, like child/adult Shooting Gallery or Great Fairy Fountains.
 *
 * This 1D array is split into groups of entrances.
 * The start of each group is indexed by `sReturnEntranceGroupIndices` values.
 * The resulting groups are then indexed by the spawn value.
 *
 * The spawn value (`PlayState.spawn`) is set to a different value depending on the entrance used to enter the
 * scene, which allows these dynamic "return entrances" to link back to the previous scene.
 *
 * Note: grottos and normal fairy fountains use `ENTR_RETURN_GROTTO`
 */
static s16 sReturnEntranceGroupData[] = {
    // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_MAGIC
    /*  0 */ ENTR_DEATH_MOUNTAIN_TRAIL_4,  // from Magic Fairy Fountain
    /*  1 */ ENTR_DEATH_MOUNTAIN_CRATER_3, // from Double Magic Fairy Fountain
    /*  2 */ ENTR_HYRULE_CASTLE_2,         // from Double Defense Fairy Fountain (as adult)

    // ENTR_RETURN_2
    /*  3 */ ENTR_KAKARIKO_VILLAGE_9, // from Potion Shop in Kakariko
    /*  4 */ ENTR_MARKET_DAY_5,       // from Potion Shop in Market

    // ENTR_RETURN_BAZAAR
    /*  5 */ ENTR_KAKARIKO_VILLAGE_3,
    /*  6 */ ENTR_MARKET_DAY_6,

    // ENTR_RETURN_4
    /*  7 */ ENTR_KAKARIKO_VILLAGE_11, // from House of Skulltulas
    /*  8 */ ENTR_BACK_ALLEY_DAY_2,    // from Bombchu Shop

    // ENTR_RETURN_SHOOTING_GALLERY
    /*  9 */ ENTR_KAKARIKO_VILLAGE_10,
    /* 10 */ ENTR_MARKET_DAY_8,

    // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_SPELLS
    /* 11 */ ENTR_ZORAS_FOUNTAIN_5,  // from Farores Wind Fairy Fountain
    /* 12 */ ENTR_HYRULE_CASTLE_2,   // from Dins Fire Fairy Fountain (as child)
    /* 13 */ ENTR_DESERT_COLOSSUS_7, // from Nayrus Love Fairy Fountain
};

/**
 * The values are indices into `sReturnEntranceGroupData` marking the start of each group
 */
static u8 sReturnEntranceGroupIndices[] = {
    11, // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_SPELLS
    9,  // ENTR_RETURN_SHOOTING_GALLERY
    3,  // ENTR_RETURN_2
    5,  // ENTR_RETURN_BAZAAR
    7,  // ENTR_RETURN_4
    0,  // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_MAGIC
};

s32 Player_HandleExitsAndVoids(PlayState* play, Player* this, CollisionPoly* poly, u32 bgId) {
    s32 exitIndex;
    s32 temp;
    s32 sp34;
    f32 speedXZ;
    s32 yaw;

    if (this->actor.category == ACTORCAT_PLAYER) {
        exitIndex = 0;

        if (!(this->stateFlags1 & PLAYER_STATE1_DEAD) && (play->transitionTrigger == TRANS_TRIGGER_OFF) && (this->csAction == 0) &&
            !(this->stateFlags1 & PLAYER_STATE1_LOADING) &&
            (((poly != NULL) && (exitIndex = SurfaceType_GetSceneExitIndex(&play->colCtx, poly, bgId), exitIndex != 0)) ||
             (func_8083816C(sFloorType) && (this->floorProperty == 12)))) {

            sp34 = this->unk_A84 - (s32)this->actor.world.pos.y;

            if (!(this->stateFlags1 & (PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_IN_WATER | PLAYER_STATE1_IN_CUTSCENE)) &&
                !(this->actor.bgCheckFlags & 1) && (sp34 < 100) && (sYDistToFloor > 100.0f)) {
                return 0;
            }

            if (exitIndex == 0) {
                Play_TriggerVoidOut(play);
                Scene_SetTransitionForNextEntrance(play);
            } else {
                play->nextEntranceIndex = play->setupExitList[exitIndex - 1];

                // Main override for entrance rando and entrance skips
                if (IS_RANDO) {
                    play->nextEntranceIndex = Entrance_OverrideNextIndex(play->nextEntranceIndex);
                }

                if (play->nextEntranceIndex == ENTR_RETURN_GROTTO) {
                    gSaveContext.respawnFlag = 2;
                    play->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex;
                    play->transitionType = TRANS_TYPE_FADE_WHITE;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                } else if (play->nextEntranceIndex >= ENTR_RETURN_YOUSEI_IZUMI_YOKO) {
                    // handle dynamic exits
                    if (IS_RANDO) {
                        play->nextEntranceIndex = Entrance_OverrideDynamicExit(sReturnEntranceGroupIndices[play->nextEntranceIndex - ENTR_RETURN_YOUSEI_IZUMI_YOKO] + play->curSpawn);
                    } else {
                        play->nextEntranceIndex =
                            sReturnEntranceGroupData[sReturnEntranceGroupIndices[play->nextEntranceIndex - ENTR_RETURN_YOUSEI_IZUMI_YOKO] + play->curSpawn];
                    }

                    Scene_SetTransitionForNextEntrance(play);
                } else {
                    // In Entrance rando, if our respawnFlag is set for a grotto return, we don't want the void out to happen
                    if (SurfaceType_GetSlope(&play->colCtx, poly, bgId) == 2 &&
                        (!IS_RANDO || (Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES) && gSaveContext.respawnFlag != 2))) {
                        gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = play->nextEntranceIndex;
                        Play_TriggerVoidOut(play);
                        gSaveContext.respawnFlag = -2;
                    }
                    gSaveContext.retainWeatherMode = 1;
                    Scene_SetTransitionForNextEntrance(play);
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_IN_CUTSCENE)) &&
                !(this->stateFlags2 & PLAYER_STATE2_CRAWLING) && !func_808332B8(this) &&
                (temp = func_80041D4C(&play->colCtx, poly, bgId), (temp != 10)) &&
                ((sp34 < 100) || (this->actor.bgCheckFlags & 1))) {

                if (temp == 11) {
                    func_800788CC(NA_SE_OC_SECRET_HOLE_OUT);
                    func_800F6964(5);
                    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
                    gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
                } else {
                    speedXZ = this->linearVelocity;

                    if (speedXZ < 0.0f) {
                        this->actor.world.rot.y += 0x8000;
                        speedXZ = -speedXZ;
                    }

                    if (speedXZ > R_RUN_SPEED_LIMIT / 100.0f) {
                        gSaveContext.entranceSpeed = R_RUN_SPEED_LIMIT / 100.0f;
                    } else {
                        gSaveContext.entranceSpeed = speedXZ;
                    }

                    if (sConveyorSpeed != 0) {
                        yaw = sConveyorYaw;
                    } else {
                        yaw = this->actor.world.rot.y;
                    }
                    func_80838E70(play, this, 400.0f, yaw);
                }
            } else {
                if (!(this->actor.bgCheckFlags & 1)) {
                    Player_ZeroSpeedXZ(this);
                }
            }

            this->stateFlags1 |= PLAYER_STATE1_LOADING | PLAYER_STATE1_IN_CUTSCENE;

            func_80835E44(play, 0x2F);

            return 1;
        } else {
            if (play->transitionTrigger == TRANS_TRIGGER_OFF) {

                if ((this->actor.world.pos.y < -4000.0f) ||
                    (((this->floorProperty == 5) || (this->floorProperty == 12)) &&
                     ((sYDistToFloor < 100.0f) || (this->fallDistance > 400.0f) ||
                      ((play->sceneNum != SCENE_SHADOW_TEMPLE) && (this->fallDistance > 200.0f)))) ||
                    ((play->sceneNum == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) && (this->fallDistance > 320.0f))) {

                    if (this->actor.bgCheckFlags & 1) {
                        if (this->floorProperty == 5) {
                            Play_TriggerRespawn(play);
                        } else {
                            Play_TriggerVoidOut(play);
                        }
                        play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
                        func_80078884(NA_SE_OC_ABYSS);
                    } else {
                        func_80838F5C(play, this);
                        this->av2.actionVar2 = 9999;
                        if (this->floorProperty == 5) {
                            this->av1.actionVar1 = -1;
                        } else {
                            this->av1.actionVar1 = 1;
                        }
                    }
                }

                this->unk_A84 = this->actor.world.pos.y;
            }
        }
    }

    return 0;
}

/**
 * Gets a position relative to player's yaw.
 * An offset is applied to the provided base position in the direction of shape y rotation.
 * The resulting position is stored in `dest`
 */
void Player_GetRelativePosition(Player* this, Vec3f* base, Vec3f* offset, Vec3f* dest) {
    f32 cos = Math_CosS(this->actor.shape.rot.y);
    f32 sin = Math_SinS(this->actor.shape.rot.y);

    dest->x = base->x + ((offset->x * cos) + (offset->z * sin));
    dest->y = base->y + offset->y;
    dest->z = base->z + ((offset->z * cos) - (offset->x * sin));
}

Actor* Player_SpawnFairy(PlayState* play, Player* this, Vec3f* arg2, Vec3f* arg3, s32 type) {
    Vec3f pos;

    Player_GetRelativePosition(this, arg2, arg3, &pos);

    return Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, pos.x, pos.y, pos.z, 0, 0, 0, type, true);
}

f32 func_808396F4(PlayState* play, Player* this, Vec3f* arg2, Vec3f* arg3, CollisionPoly** arg4, s32* arg5) {
    Player_GetRelativePosition(this, &this->actor.world.pos, arg2, arg3);

    return BgCheck_EntityRaycastFloor3(&play->colCtx, arg4, arg5, arg3);
}

f32 func_8083973C(PlayState* play, Player* this, Vec3f* arg2, Vec3f* arg3) {
    CollisionPoly* sp24;
    s32 sp20;

    return func_808396F4(play, this, arg2, arg3, &sp24, &sp20);
}

/**
 * Checks if a line between the player's position and the provided `offset` intersect a wall.
 *
 * Point A of the line is at player's world position offset by the height provided in `offset`.
 * Point B of the line is at player's world position offset by the entire `offset` vector.
 * Point A and B are always at the same height, meaning this is a horizontal line test.
 */
s32 Player_PosVsWallLineTest(PlayState* play, Player* this, Vec3f* offset, CollisionPoly** wallPoly, s32* bgId,
                             Vec3f* posResult) {
    Vec3f posA;
    Vec3f posB;

    posA.x = this->actor.world.pos.x;
    posA.y = this->actor.world.pos.y + offset->y;
    posA.z = this->actor.world.pos.z;

    Player_GetRelativePosition(this, &this->actor.world.pos, offset, &posB);

    return BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, posResult, wallPoly, true, false, false, true, bgId);
}

s32 Player_ActionHandler_1(Player* this, PlayState* play) {
    DoorShutter* doorShutter;
    EnDoor* door; // Can also be DoorKiller*
    s32 doorDirection;
    f32 sp78;
    f32 sp74;
    Actor* doorActor;
    f32 sp6C;
    s32 pad3;
    s32 frontRoom;
    Actor* attachedActor;
    LinkAnimationHeader* sp5C;
    CollisionPoly* groundPoly;
    Vec3f checkPos;

    if ((this->doorType != PLAYER_DOORTYPE_NONE) &&
        (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) ||
         ((this->heldActor != NULL) && (this->heldActor->id == ACTOR_EN_RU1)))) {
        if ((CHECK_BTN_ALL(sControlInput->press.button, BTN_A) || (Player_Action_8084F9A0 == this->actionFunc)) && GameInteractor_Should(VB_BE_ABLE_TO_OPEN_DOORS, true)) {
            doorActor = this->doorActor;

            if (this->doorType <= PLAYER_DOORTYPE_AJAR) {
                doorActor->textId = 0xD0;
                Player_StartTalking(play, doorActor);
                return 0;
            }

            doorDirection = this->doorDirection;
            sp78 = Math_CosS(doorActor->shape.rot.y);
            sp74 = Math_SinS(doorActor->shape.rot.y);

            if (this->doorType == PLAYER_DOORTYPE_SLIDING) {
                doorShutter = (DoorShutter*)doorActor;

                this->yaw = doorShutter->dyna.actor.home.rot.y;
                if (doorDirection > 0) {
                    this->yaw -= 0x8000;
                }
                this->actor.shape.rot.y = this->yaw;

                if (this->linearVelocity <= 0.0f) {
                    this->linearVelocity = 0.1f;
                }

                func_80838E70(play, this, 50.0f, this->actor.shape.rot.y);

                this->av1.actionVar1 = 0;
                this->unk_447 = this->doorType;
                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;

                this->unk_450.x = this->actor.world.pos.x + ((doorDirection * 20.0f) * sp74);
                this->unk_450.z = this->actor.world.pos.z + ((doorDirection * 20.0f) * sp78);
                this->unk_45C.x = this->actor.world.pos.x + ((doorDirection * -120.0f) * sp74);
                this->unk_45C.z = this->actor.world.pos.z + ((doorDirection * -120.0f) * sp78);

                doorShutter->unk_164 = 1;
                func_80832224(this);

                if (this->doorTimer != 0) {
                    this->av2.actionVar2 = 0;
                    Player_AnimChangeOnceMorph(play, this, Player_GetIdleAnim(this));
                    this->skelAnime.endFrame = 0.0f;
                } else {
                    this->linearVelocity = 0.1f;
                }

                if (doorShutter->dyna.actor.category == ACTORCAT_DOOR) {
                    this->cv.slidingDoorBgCamIndex = play->transiActorCtx.list[(u16)doorShutter->dyna.actor.params >> 10]
                                        .sides[(doorDirection > 0) ? 0 : 1]
                                        .effects;

                    Actor_DisableLens(play);
                }
            } else {
                // This actor can be either EnDoor or DoorKiller.
                // Don't try to access any struct vars other than `animStyle` and `playerIsOpening`! These two variables
                // are common across the two actors' structs however most other variables are not!
                door = (EnDoor*)doorActor;

                door->animStyle = (doorDirection < 0.0f) ? (LINK_IS_ADULT ? KNOB_ANIM_ADULT_L : KNOB_ANIM_CHILD_L)
                                                         : (LINK_IS_ADULT ? KNOB_ANIM_ADULT_R : KNOB_ANIM_CHILD_R);

                if (door->animStyle == KNOB_ANIM_ADULT_L) {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorA_free, this->modelAnimType);
                } else if (door->animStyle == KNOB_ANIM_CHILD_L) {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorA, this->modelAnimType);
                } else if (door->animStyle == KNOB_ANIM_ADULT_R) {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorB_free, this->modelAnimType);
                } else {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorB, this->modelAnimType);
                }

                Player_SetupAction(play, this, Player_Action_80845EF8, 0);
                Player_PutAwayHeldItem(play, this);

                if (doorDirection < 0) {
                    this->actor.shape.rot.y = doorActor->shape.rot.y;
                } else {
                    this->actor.shape.rot.y = doorActor->shape.rot.y - 0x8000;
                }

                this->yaw = this->actor.shape.rot.y;

                sp6C = (doorDirection * 22.0f);
                this->actor.world.pos.x = doorActor->world.pos.x + sp6C * sp74;
                this->actor.world.pos.z = doorActor->world.pos.z + sp6C * sp78;

                func_8083328C(play, this, sp5C);

                if (this->doorTimer != 0) {
                    this->skelAnime.endFrame = 0.0f;
                }

                func_80832224(this);
                Player_StartAnimMovement(play, this, 0x28F);

                if (doorActor->parent != NULL) {
                    doorDirection = -doorDirection;
                }

                door->playerIsOpening = 1;

                if (this->doorType != PLAYER_DOORTYPE_FAKE) {
                    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                    Actor_DisableLens(play);

                    if (((doorActor->params >> 7) & 7) == 3) {
                        checkPos.x = doorActor->world.pos.x - (sp6C * sp74);
                        checkPos.y = doorActor->world.pos.y + 10.0f;
                        checkPos.z = doorActor->world.pos.z - (sp6C * sp78);

                        BgCheck_EntityRaycastFloor1(&play->colCtx, &groundPoly, &checkPos);

                        //! @bug groundPoly's bgId is not guaranteed to be BGCHECK_SCENE
                        if (Player_HandleExitsAndVoids(play, this, groundPoly, BGCHECK_SCENE)) {
                            gSaveContext.entranceSpeed = 2.0f;
                            gSaveContext.entranceSound = NA_SE_OC_DOOR_OPEN;
                        }
                    } else {
                        Camera_ChangeDoorCam(Play_GetCamera(play, 0), doorActor,
                                             play->transiActorCtx.list[(u16)doorActor->params >> 10]
                                                 .sides[(doorDirection > 0) ? 0 : 1]
                                                 .effects,
                                             0, 38.0f * sInvWaterSpeedFactor, 26.0f * sInvWaterSpeedFactor, 10.0f * sInvWaterSpeedFactor);
                    }
                }
            }

            if ((this->doorType != PLAYER_DOORTYPE_FAKE) && (doorActor->category == ACTORCAT_DOOR)) {
                frontRoom = play->transiActorCtx.list[(u16)doorActor->params >> 10]
                                .sides[(doorDirection > 0) ? 0 : 1]
                                .room;

                if ((frontRoom >= 0) && (frontRoom != play->roomCtx.curRoom.num)) {
                    func_8009728C(play, &play->roomCtx, frontRoom);
                }
            }

            doorActor->room = play->roomCtx.curRoom.num;

            if (((attachedActor = doorActor->child) != NULL) || ((attachedActor = doorActor->parent) != NULL)) {
                attachedActor->room = play->roomCtx.curRoom.num;
            }

            return 1;
        }
    }

    return 0;
}

void func_80839E88(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;

    Player_SetupAction(play, this, Player_Action_80840450, 1);

    if (this->unk_870 < 0.5f) {
        anim = func_808334E4(this);
        this->unk_870 = 0.0f;
    } else {
        anim = func_80833528(this);
        this->unk_870 = 1.0f;
    }

    this->unk_874 = this->unk_870;
    Player_AnimPlayLoop(play, this, anim);
    this->yaw = this->actor.shape.rot.y;
}

void func_80839F30(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_808407CC, 1);
    Player_AnimChangeOnceMorph(play, this, Player_GetIdleAnim(this));
    this->yaw = this->actor.shape.rot.y;
}

void func_80839F90(Player* this, PlayState* play) {
    if (Player_CheckHostileLockOn(this)) {
        func_80839E88(this, play);
    } else if (Player_FriendlyLockOnOrParallel(this)) {
        func_80839F30(this, play);
    } else {
        func_80853080(this, play);
    }
}

void func_80839FFC(Player* this, PlayState* play) {
    PlayerActionFunc actionFunc;

    if (Player_CheckHostileLockOn(this)) {
        actionFunc = Player_Action_80840450;
    } else if (Player_FriendlyLockOnOrParallel(this)) {
        actionFunc = Player_Action_808407CC;
    } else {
        actionFunc = Player_Action_Idle;
    }

    Player_SetupAction(play, this, actionFunc, 1);
}

void func_8083A060(Player* this, PlayState* play) {
    func_80839FFC(this, play);

    if (Player_CheckHostileLockOn(this)) {
        this->av2.actionVar2 = 1;
    }
}

void func_8083A098(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    func_8083A060(this, play);
    func_8083328C(play, this, anim);
}

int func_8083A0D4(Player* this) {
    return (this->interactRangeActor != NULL) && (this->heldActor == NULL);
}

void func_8083A0F4(PlayState* play, Player* this) {
    if (func_8083A0D4(this)) {
        Actor* interactRangeActor = this->interactRangeActor;
        s32 interactActorId = interactRangeActor->id;

        if (interactActorId == ACTOR_BG_TOKI_SWD) {
            this->interactRangeActor->parent = &this->actor;
            Player_SetupAction(play, this, Player_Action_8084F608, 0);
            this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
            if (!CVarGetInteger(CVAR_ENHANCEMENT("PersistentMasks"), 0) || !CVarGetInteger(CVAR_ENHANCEMENT("AdultMasks"), 0)) {
                gSaveContext.maskMemory = PLAYER_MASK_NONE;
            }
        } else {
            LinkAnimationHeader* anim;

            if (interactActorId == ACTOR_BG_HEAVY_BLOCK) {
                Player_SetupAction(play, this, Player_Action_80846120, 0);
                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                anim = &gPlayerAnim_link_normal_heavy_carry;
            } else if ((interactActorId == ACTOR_EN_ISHI) && ((interactRangeActor->params & 0xF) == 1)) {
                Player_SetupAction(play, this, Player_Action_80846260, 0);
                anim = &gPlayerAnim_link_silver_carry;
            } else if (((interactActorId == ACTOR_EN_BOMBF) || (interactActorId == ACTOR_EN_KUSA)) &&
                       (Player_GetStrength() <= PLAYER_STR_NONE)) {
                Player_SetupAction(play, this, Player_Action_80846408, 0);
                this->actor.world.pos.x =
                    (Math_SinS(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.x;
                this->actor.world.pos.z =
                    (Math_CosS(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.z;
                this->yaw = this->actor.shape.rot.y = interactRangeActor->yawTowardsPlayer + 0x8000;
                anim = &gPlayerAnim_link_normal_nocarry_free;
            } else {
                Player_SetupAction(play, this, Player_Action_80846050, 0);
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_carryB, this->modelAnimType);
            }

            if (GameInteractor_Should(VB_PLAY_THROW_ANIMATION, true, anim)) {
                LinkAnimation_PlayOnce(play, &this->skelAnime, anim);
            }
        }
    } else {
        func_80839F90(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
    }
}

void Player_SetupTalk(PlayState* play, Player* this) {
    Player_SetupActionPreserveAnimMovement(play, this, Player_Action_Talk, 0);

    this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_IN_CUTSCENE;

    if (this->actor.textId != 0) {
        Message_StartTextbox(play, this->actor.textId, this->talkActor);
        this->focusActor = this->talkActor;
    }
}

void func_8083A360(PlayState* play, Player* this) {
    Player_SetupActionPreserveAnimMovement(play, this, Player_Action_8084CC98, 0);
}

void func_8083A388(PlayState* play, Player* this) {
    Player_SetupAction(play, this, Player_Action_8084B78C, 0);
}

void func_8083A3B0(PlayState* play, Player* this) {
    s32 sp1C = this->av2.actionVar2;
    s32 sp18 = this->av1.actionVar1;

    Player_SetupActionPreserveAnimMovement(play, this, Player_Action_8084BF1C, 0);
    this->actor.velocity.y = 0.0f;

    this->av2.actionVar2 = sp1C;
    this->av1.actionVar1 = sp18;
}

void func_8083A40C(PlayState* play, Player* this) {
    Player_SetupActionPreserveAnimMovement(play, this, Player_Action_8084C760, 0);
}

void func_8083A434(PlayState* play, Player* this) {
    Player_SetupActionPreserveAnimMovement(play, this, Player_Action_8084E6D4, 0);

    this->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_IN_CUTSCENE;

    if (this->getItemId == GI_HEART_CONTAINER_2) {
        this->av2.actionVar2 = 20;
    } else if (this->getItemId >= 0 || (this->getItemEntry.objectId != OBJECT_INVALID && this->getItemEntry.getItemId >= 0)) {
        this->av2.actionVar2 = 1;
    } else {
        this->getItemId = -this->getItemId;
        this->getItemEntry.getItemId = -this->getItemEntry.getItemId;
    }
}

s32 func_8083A4A8(Player* this, PlayState* play) {
    s16 yawDiff;
    LinkAnimationHeader* anim;
    f32 temp;

    yawDiff = this->yaw - this->actor.shape.rot.y;

    if ((ABS(yawDiff) < 0x1000) && (this->linearVelocity > 4.0f)) {
        anim = &gPlayerAnim_link_normal_run_jump;
    } else {
        anim = &gPlayerAnim_link_normal_jump;
    }

    if (this->linearVelocity > (IREG(66) / 100.0f)) {
        temp = IREG(67) / 100.0f;
    } else {
        temp = (IREG(68) / 100.0f) + ((IREG(69) * this->linearVelocity) / 1000.0f);
    }

    func_80838940(this, anim, temp, play, NA_SE_VO_LI_AUTO_JUMP);
    this->av2.actionVar2 = 1;

    return 1;
}

void func_8083A5C4(PlayState* play, Player* this, CollisionPoly* arg2, f32 arg3, LinkAnimationHeader* anim) {
    f32 nx = COLPOLY_GET_NORMAL(arg2->normal.x);
    f32 nz = COLPOLY_GET_NORMAL(arg2->normal.z);

    Player_SetupAction(play, this, Player_Action_8084BBE4, 0);
    func_80832564(play, this);
    Player_AnimPlayOnce(play, this, anim);

    this->actor.world.pos.x -= (arg3 + 1.0f) * nx;
    this->actor.world.pos.z -= (arg3 + 1.0f) * nz;
    this->actor.shape.rot.y = this->yaw = Math_Atan2S(nz, nx);

    func_80832224(this);
    Player_ResetAnimMovement(this);
}

s32 func_8083A6AC(Player* this, PlayState* play) {
    //! @bug `floorPitch` and `floorPitchAlt` are cleared to 0 before this function is called, because the player
    //! left the ground. The angles will always be zero and therefore will always pass these checks.
    //! The intention seems to be to prevent ledge hanging or vine grabbing when walking off of a steep enough slope.
    if ((this->actor.yDistToWater < -80.0f) && (ABS(this->floorPitch) < 2730) && (ABS(this->floorPitchAlt) < 2730)) {
        CollisionPoly* sp84;
        s32 sp80;
        Vec3f sp74;
        Vec3f sp68;
        f32 temp1;

        sp74.x = this->actor.prevPos.x - this->actor.world.pos.x;
        sp74.z = this->actor.prevPos.z - this->actor.world.pos.z;

        temp1 = sqrtf(SQ(sp74.x) + SQ(sp74.z));
        if (temp1 != 0.0f) {
            temp1 = 5.0f / temp1;
        } else {
            temp1 = 0.0f;
        }

        sp74.x = this->actor.prevPos.x + (sp74.x * temp1);
        sp74.y = this->actor.world.pos.y;
        sp74.z = this->actor.prevPos.z + (sp74.z * temp1);

        if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &sp74, &sp68, &sp84, true, false, false,
                                    true, &sp80) &&
            ((ABS(sp84->normal.y) < 600) || (CVarGetInteger(CVAR_CHEAT("ClimbEverything"), 0) != 0))) {
            f32 nx = COLPOLY_GET_NORMAL(sp84->normal.x);
            f32 ny = COLPOLY_GET_NORMAL(sp84->normal.y);
            f32 nz = COLPOLY_GET_NORMAL(sp84->normal.z);
            f32 sp54;
            s32 sp50;

            sp54 = Math3D_UDistPlaneToPos(nx, ny, nz, sp84->dist, &this->actor.world.pos);

            sp50 = (sPrevFloorProperty == 6);
            if (!sp50 && (func_80041DB8(&play->colCtx, sp84, sp80) & 8)) {
                sp50 = 1;
            }

            func_8083A5C4(play, this, sp84, sp54,
                          sp50 ? &gPlayerAnim_link_normal_Fclimb_startB : &gPlayerAnim_link_normal_fall);

            if (sp50) {
                Player_SetupWaitForPutAway(play, this, func_8083A3B0);

                this->yaw += 0x8000;
                this->actor.shape.rot.y = this->yaw;

                this->stateFlags1 |= PLAYER_STATE1_CLIMBING_LADDER;
                Player_StartAnimMovement(play, this, 0x9F);

                this->av2.actionVar2 = -1;
                this->av1.actionVar1 = sp50;
            } else {
                this->stateFlags1 |= PLAYER_STATE1_HANGING_OFF_LEDGE;
                this->stateFlags1 &= ~PLAYER_STATE1_PARALLEL;
            }

            Player_PlaySfx(this, NA_SE_PL_SLIPDOWN);
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_HANG);
            return 1;
        }
    }

    return 0;
}

void func_8083A9B8(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_8084BDFC, 0);
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, 1.3f);
}

static Vec3f D_8085451C = { 0.0f, 0.0f, 100.0f };

void func_8083AA10(Player* this, PlayState* play) {
    s32 sp5C;
    CollisionPoly* sp58;
    s32 sp54;
    WaterBox* sp50;
    Vec3f sp44;
    f32 sp40;
    f32 sp3C;

    this->fallDistance = this->fallStartHeight - (s32)this->actor.world.pos.y;

    if (!(this->stateFlags1 & (PLAYER_STATE1_IN_WATER | PLAYER_STATE1_IN_CUTSCENE)) && !(this->actor.bgCheckFlags & 1)) {
        if (!func_80838FB8(play, this)) {
            if (sPrevFloorProperty == 8) {
                this->actor.world.pos.x = this->actor.prevPos.x;
                this->actor.world.pos.z = this->actor.prevPos.z;
                return;
            }

            if (!(this->stateFlags3 & PLAYER_STATE3_MIDAIR) && !(this->skelAnime.moveFlags & 0x80) &&
                (Player_Action_8084411C != this->actionFunc) && (Player_Action_80844A44 != this->actionFunc)) {

                if ((sPrevFloorProperty == 7) || (this->meleeWeaponState != 0)) {
                    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
                    Player_ZeroSpeedXZ(this);
                    return;
                }

                if (this->hoverBootsTimer != 0) {
                    this->actor.velocity.y = 1.0f;
                    sPrevFloorProperty = 9;
                    return;
                }

                sp5C = (s16)(this->yaw - this->actor.shape.rot.y);

                Player_SetupAction(play, this, Player_Action_8084411C, 1);
                func_80832440(play, this);

                this->floorSfxOffset = this->prevFloorSfxOffset;

                if ((this->actor.bgCheckFlags & 4) && !(this->stateFlags1 & PLAYER_STATE1_IN_WATER) && (sPrevFloorProperty != 6) &&
                    (sPrevFloorProperty != 9) && (sYDistToFloor > 20.0f) && (this->meleeWeaponState == 0) && (ABS(sp5C) < 0x2000) &&
                    (this->linearVelocity > 3.0f)) {

                    if ((sPrevFloorProperty == 11) && !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {

                        sp40 = func_808396F4(play, this, &D_8085451C, &sp44, &sp58, &sp54);
                        sp3C = this->actor.world.pos.y;

                        if (WaterBox_GetSurface1(play, &play->colCtx, sp44.x, sp44.z, &sp3C, &sp50) &&
                            ((sp3C - sp40) > 50.0f)) {
                            func_808389E8(this, &gPlayerAnim_link_normal_run_jump_water_fall, 6.0f, play);
                            Player_SetupAction(play, this, Player_Action_80844A44, 0);
                            return;
                        }
                    }

                    func_8083A4A8(this, play);
                    return;
                }

                if ((sPrevFloorProperty == 9) || (sYDistToFloor <= this->ageProperties->unk_34) ||
                    !func_8083A6AC(this, play)) {
                    Player_AnimPlayLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
                    return;
                }
            }
        }
    } else {
        this->fallStartHeight = this->actor.world.pos.y;
    }
}

s32 func_8083AD4C(PlayState* play, Player* this) {
    s32 camMode;

    if (this->unk_6AD == 2) {
        if (func_8002DD6C(this)) {
            bool shouldUseBowCamera = LINK_IS_ADULT;

            if(CVarGetInteger(CVAR_ENHANCEMENT("BowSlingshotAmmoFix"), 0) || CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0)) {
                shouldUseBowCamera = this->heldItemAction != PLAYER_IA_SLINGSHOT;
            }

            camMode = shouldUseBowCamera ? CAM_MODE_BOWARROW : CAM_MODE_SLINGSHOT;
        } else {
            camMode = CAM_MODE_BOOMERANG;
        }
    } else {
        camMode = CAM_MODE_FIRSTPERSON;
    }

    return Camera_ChangeMode(Play_GetCamera(play, 0), camMode);
}

/**
 * If appropriate, setup action for performing a `csAction`
 *
 * @return  true if a `csAction` is started, false if not
 */
s32 Player_StartCsAction(PlayState* play, Player* this) {
    // unk_6AD will get set to 3 in `Player_UpdateCommon` if `this->csAction` is non-zero
    // (with a special case for `PLAYER_CSACTION_7`)
    if (this->unk_6AD == 3) {
        Player_SetupAction(play, this, Player_Action_CsAction, 0);

        if (this->cv.haltActorsDuringCsAction) {
            this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
        }

        func_80832318(this);
        return true;
    } else {
        return false;
    }
}

void func_8083AE40(Player* this, s16 objectId) {
    s32 pad;
    size_t size;

    if (objectId != OBJECT_INVALID) {
        this->giObjectLoading = true;
        osCreateMesgQueue(&this->giObjectLoadQueue, &this->giObjectLoadMsg, 1);

        size = gObjectTable[objectId].vromEnd - gObjectTable[objectId].vromStart;

        LOG_HEX("size", size);
        assert(size <= 1024 * 8);

        DmaMgr_SendRequest2(&this->giObjectDmaRequest, (uintptr_t)this->giObjectSegment,
                            gObjectTable[objectId].vromStart, size, 0, &this->giObjectLoadQueue, OS_MESG_PTR(NULL),
                            __FILE__, __LINE__);
    }
}

void func_8083AF44(PlayState* play, Player* this, s32 magicSpell) {
    Player_SetupActionPreserveItemAction(play, this, Player_Action_808507F4, 0);

    this->av1.actionVar1 = magicSpell - 3;

    //! @bug `MAGIC_CONSUME_WAIT_PREVIEW` is not guaranteed to succeed.
    //! Ideally, the return value of `Magic_RequestChange` should be checked before allowing the process of
    //! using a spell to continue. If the magic state change request fails, `gSaveContext.magicTarget` will
    //! never be set correctly.
    //! When `MAGIC_STATE_CONSUME_SETUP` is set in `Player_Action_808507F4`, magic will eventually be
    //! consumed to a stale target value. If that stale target value is higher than the current
    //! magic value, it will be consumed to zero.
    Magic_RequestChange(play, sMagicSpellCosts[magicSpell], MAGIC_CONSUME_WAIT_PREVIEW);

    u8 isFastFarores = CVarGetInteger(CVAR_ENHANCEMENT("FastFarores"), 0) && this->itemAction == PLAYER_IA_FARORES_WIND;

    if (isFastFarores) {
        LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, &gPlayerAnim_link_magic_tame, 0.83f * 2);
        return;
    } else {
        LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, &gPlayerAnim_link_magic_tame, 0.83f);
    }

    if (magicSpell == 5) {
        this->subCamId = OnePointCutscene_Init(play, 1100, -101, NULL, MAIN_CAM);
    } else {
        func_80835EA4(play, 10);
    }
}

void func_8083B010(Player* this) {
    this->actor.focus.rot.x = this->actor.focus.rot.z = this->headLimbRot.x = this->headLimbRot.y = this->headLimbRot.z = this->upperLimbRot.x =
        this->upperLimbRot.y = this->upperLimbRot.z = 0;

    this->actor.focus.rot.y = this->actor.shape.rot.y;
}

static u8 D_80854528[] = {
    GI_LETTER_ZELDA, // EXCH_ITEM_ZELDAS_LETTER
    GI_WEIRD_EGG,    // EXCH_ITEM_WEIRD_EGG
    GI_CHICKEN,      // EXCH_ITEM_CHICKEN
    GI_BEAN,         // EXCH_ITEM_MAGIC_BEAN
    GI_POCKET_EGG,   // EXCH_ITEM_POCKET_EGG
    GI_POCKET_CUCCO, // EXCH_ITEM_POCKET_CUCCO
    GI_COJIRO,       // EXCH_ITEM_COJIRO
    GI_ODD_MUSHROOM, // EXCH_ITEM_ODD_MUSHROOM
    GI_ODD_POTION,   // EXCH_ITEM_ODD_POTION
    GI_SAW,          // EXCH_ITEM_POACHERS_SAW
    GI_SWORD_BROKEN, // EXCH_ITEM_BROKEN_GORONS_SWORD
    GI_PRESCRIPTION, // EXCH_ITEM_PRESCRIPTION
    GI_FROG,         // EXCH_ITEM_EYEBALL_FROG
    GI_EYEDROPS,     // EXCH_ITEM_EYE_DROPS
    GI_CLAIM_CHECK,  // EXCH_ITEM_CLAIM_CHECK
    GI_MASK_SKULL,   // EXCH_ITEM_MASK_SKULL
    GI_MASK_SPOOKY,  // EXCH_ITEM_MASK_SPOOKY
    GI_MASK_KEATON,  // EXCH_ITEM_MASK_KEATON
    GI_MASK_BUNNY,   // EXCH_ITEM_MASK_BUNNY_HOOD
    GI_MASK_TRUTH,   // EXCH_ITEM_MASK_TRUTH
    GI_MASK_GORON,   // EXCH_ITEM_MASK_GORON
    GI_MASK_ZORA,    // EXCH_ITEM_MASK_ZORA
    GI_MASK_GERUDO,  // EXCH_ITEM_MASK_GERUDO
    GI_LETTER_RUTO,  // EXCH_ITEM_BOTTLE_FISH
    GI_LETTER_RUTO,  // EXCH_ITEM_BOTTLE_BLUE_FIRE
    GI_LETTER_RUTO,  // EXCH_ITEM_BOTTLE_BUG
    GI_LETTER_RUTO,  // EXCH_ITEM_BOTTLE_POE
    GI_LETTER_RUTO,  // EXCH_ITEM_BOTTLE_BIG_POE
    GI_LETTER_RUTO,  // EXCH_ITEM_BOTTLE_RUTOS_LETTER
};

static LinkAnimationHeader* D_80854548[] = {
    &gPlayerAnim_link_normal_give_other,
    &gPlayerAnim_link_bottle_read,
    &gPlayerAnim_link_normal_take_out,
};

s32 Player_ActionHandler_13(Player* this, PlayState* play) {
    s32 sp2C;
    s32 sp28;
    GetItemEntry giEntry;
    Actor* talkActor;

    if ((this->unk_6AD != 0) &&
        (func_808332B8(this) || (this->actor.bgCheckFlags & 1) || (this->stateFlags1 & PLAYER_STATE1_ON_HORSE))) {

        if (!Player_StartCsAction(play, this)) {
            if (this->unk_6AD == 4) {
                sp2C = Player_ActionToMagicSpell(this, this->itemAction);
                if (sp2C >= 0) {
                    if ((sp2C != 3) || (gSaveContext.respawn[RESPAWN_MODE_TOP].data <= 0)) {
                        func_8083AF44(play, this, sp2C);
                    } else {
                        Player_SetupAction(play, this, Player_Action_8085063C, 1);
                        this->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE;
                        Player_AnimPlayOnce(play, this, Player_GetIdleAnim(this));
                        func_80835EA4(play, 4);
                    }

                    func_80832224(this);
                    return 1;
                }

                sp2C = this->itemAction - PLAYER_IA_ZELDAS_LETTER;
                if ((sp2C >= 0) ||
                    (sp28 = Player_ActionToBottle(this, this->itemAction) - 1,
                     ((sp28 >= 0) && (sp28 < 6) &&
                      ((this->itemAction > PLAYER_IA_BOTTLE_POE) ||
                       ((this->talkActor != NULL) &&
                        (((this->itemAction == PLAYER_IA_BOTTLE_POE) && (this->exchangeItemId == EXCH_ITEM_POE)) ||
                         (this->exchangeItemId == EXCH_ITEM_BLUE_FIRE))))))) {

                    if ((play->actorCtx.titleCtx.delayTimer == 0) && (play->actorCtx.titleCtx.alpha == 0)) {
                        Player_SetupActionPreserveItemAction(play, this, Player_Action_ExchangeItem, 0);

                        if (sp2C >= 0) {
                            if (this->getItemEntry.objectId == OBJECT_INVALID) {
                                giEntry = ItemTable_Retrieve(D_80854528[sp2C]);
                            } else {
                                giEntry = this->getItemEntry;
                            }
                            func_8083AE40(this, giEntry.objectId);
                        }

                        this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE;

                        if (sp2C >= 0) {
                            sp2C = sp2C + 1;
                        } else {
                            sp2C = sp28 + 0x18;
                        }

                        talkActor = this->talkActor;

                        if ((talkActor != NULL) &&
                            ((this->exchangeItemId == sp2C) || (this->exchangeItemId == EXCH_ITEM_BLUE_FIRE) ||
                             ((this->exchangeItemId == EXCH_ITEM_POE) &&
                              (this->itemAction == PLAYER_IA_BOTTLE_BIG_POE)) ||
                             ((this->exchangeItemId == EXCH_ITEM_BEAN) &&
                              (this->itemAction == PLAYER_IA_BOTTLE_BUG))) &&
                            ((this->exchangeItemId != EXCH_ITEM_BEAN) || (this->itemAction == PLAYER_IA_MAGIC_BEAN))) {
                            if (this->exchangeItemId == EXCH_ITEM_BEAN) {
                                Inventory_ChangeAmmo(ITEM_BEAN, -1);
                                Player_SetupActionPreserveItemAction(play, this, Player_Action_8084279C, 0);
                                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                                this->av2.actionVar2 = 0x50;
                                this->av1.actionVar1 = -1;
                            }
                            talkActor->flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
                            this->focusActor = this->talkActor;
                        } else if (sp2C == EXCH_ITEM_LETTER_RUTO) {
                            this->av1.actionVar1 = 1;
                            this->actor.textId = 0x4005;
                            func_80835EA4(play, 1);
                        } else {
                            this->av1.actionVar1 = 2;
                            this->actor.textId = 0xCF;
                            func_80835EA4(play, 4);
                        }

                        this->actor.flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
                        this->exchangeItemId = sp2C;

                        if (this->av1.actionVar1 < 0) {
                            Player_AnimChangeOnceMorph(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_check, this->modelAnimType));
                        } else {
                            Player_AnimPlayOnce(play, this, D_80854548[this->av1.actionVar1]);
                        }

                        func_80832224(this);
                    }
                    return 1;
                }

                sp2C = Player_ActionToBottle(this, this->itemAction);
                if (sp2C >= 0) {
                    if (sp2C == 0xC) {
                        Player_SetupActionPreserveItemAction(play, this, Player_Action_8084EED8, 0);
                        Player_AnimPlayOnceAdjusted(play, this, &gPlayerAnim_link_bottle_bug_out);
                        func_80835EA4(play, 3);
                    } else if ((sp2C > 0) && (sp2C < 4)) {
                        Player_SetupActionPreserveItemAction(play, this, Player_Action_8084EFC0, 0);
                        Player_AnimPlayOnceAdjusted(play, this, &gPlayerAnim_link_bottle_fish_out);
                        func_80835EA4(play, (sp2C == 1) ? 1 : 5);
                    } else {
                        Player_SetupActionPreserveItemAction(play, this, Player_Action_8084EAC0, 0);
                        Player_AnimChangeOnceMorphAdjusted(play, this, &gPlayerAnim_link_bottle_drink_demo_start);
                        func_80835EA4(play, 2);
                    }
                } else {
                    Player_SetupActionPreserveItemAction(play, this, Player_Action_8084E3C4, 0);
                    Player_AnimPlayOnceAdjusted(play, this, &gPlayerAnim_link_normal_okarina_start);
                    this->stateFlags2 |= PLAYER_STATE2_OCARINA_PLAYING;
                    func_80835EA4(play, (this->unk_6A8 != NULL) ? 0x5B : 0x5A);
                    if (this->unk_6A8 != NULL) {
                        this->stateFlags2 |= PLAYER_STATE2_PLAY_FOR_ACTOR;
                        Camera_SetParam(Play_GetCamera(play, 0), 8, this->unk_6A8);
                    }
                }
            } else if (func_8083AD4C(play, this)) {
                if (!(this->stateFlags1 & PLAYER_STATE1_ON_HORSE)) {
                    Player_SetupAction(play, this, Player_Action_8084B1D8, 1);
                    this->av2.actionVar2 = 13;
                    func_8083B010(this);
                }
                this->stateFlags1 |= PLAYER_STATE1_FIRST_PERSON;
                func_80078884(NA_SE_SY_CAMERA_ZOOM_UP);
                Player_ZeroSpeedXZ(this);
                return 1;
            } else {
                this->unk_6AD = 0;
                func_80078884(NA_SE_SY_ERROR);
                return 0;
            }

            this->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE;
        }

        func_80832224(this);
        return 1;
    }

    return 0;
}

s32 Player_ActionHandler_Talk(Player* this, PlayState* play) {
    Actor* talkOfferActor = this->talkActor;
    Actor* lockOnActor = this->focusActor;
    Actor* cUpTalkActor = NULL;
    s32 forceTalkToNavi = false;
    s32 canTalkToLockOnWithCUp;

    canTalkToLockOnWithCUp =
        (lockOnActor != NULL) &&
        (CHECK_FLAG_ALL(lockOnActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_NAVI_HAS_INFO) ||
        (lockOnActor->naviEnemyId != 0xFF));

    if (canTalkToLockOnWithCUp || (this->naviTextId != 0)) {
        // If `naviTextId` is negative and outside the 0x2XX range, talk to Navi instantly
        forceTalkToNavi = (this->naviTextId < 0) && ((ABS(this->naviTextId) & 0xFF00) != 0x200);

        if (forceTalkToNavi || !canTalkToLockOnWithCUp) {
            // If `lockOnActor` can't be talked to with c-up, the only option left is Navi
            cUpTalkActor = this->naviActor;

            if (forceTalkToNavi) {
                // Clearing these pointers guarantees that `cUpTalkActor` will take priority
                lockOnActor = NULL;
                talkOfferActor = NULL;
            }
        } else {
            // Navi is not the talk actor, so the only option left for talking with c-up is `lockOnActor`
            // (though, `lockOnActor` may be NULL at this point).
            cUpTalkActor = lockOnActor;
        }
    }

    if ((talkOfferActor != NULL) || (cUpTalkActor != NULL)) {
        if ((lockOnActor == NULL) || (lockOnActor == talkOfferActor) || (lockOnActor == cUpTalkActor)) {
            if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) ||
                ((this->heldActor != NULL) && (forceTalkToNavi || (talkOfferActor == this->heldActor) || (cUpTalkActor == this->heldActor) ||
                                               ((talkOfferActor != NULL) && (talkOfferActor->flags & ACTOR_FLAG_WILL_TALK))))) {
                if ((this->actor.bgCheckFlags & 1) || (this->stateFlags1 & PLAYER_STATE1_ON_HORSE) ||
                    (func_808332B8(this) && !(this->stateFlags2 & PLAYER_STATE2_UNDERWATER))) {

                    if (talkOfferActor != NULL) {
                        this->stateFlags2 |= PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER;
                        if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) || (talkOfferActor->flags & ACTOR_FLAG_WILL_TALK)) {
                            cUpTalkActor = NULL;
                        } else if (cUpTalkActor == NULL) {
                            return 0;
                        }
                    }

                    if (cUpTalkActor != NULL) {
                        if (!forceTalkToNavi) {
                            this->stateFlags2 |= PLAYER_STATE2_NAVI_ALERT;
                        }

                        if (!CHECK_BTN_ALL(sControlInput->press.button, CVarGetInteger(CVAR_ENHANCEMENT("NaviOnL"), 0) ? BTN_L : BTN_CUP) && !forceTalkToNavi) {
                            return 0;
                        }

                        talkOfferActor = cUpTalkActor;
                        this->talkActor = NULL;

                        if (forceTalkToNavi || !canTalkToLockOnWithCUp) {
                            cUpTalkActor->textId = ABS(this->naviTextId);
                        } else {
                            if (cUpTalkActor->naviEnemyId != 0xFF) {
                                cUpTalkActor->textId = cUpTalkActor->naviEnemyId + 0x600;
                            }
                        }
                    }

                    // `sSavedCurrentMask` saves the current mask just before the current action runs on this frame.
                    // This saved mask value is then restored just before starting a conversation.
                    //
                    // This handles an edge case where a conversation is started on the same frame that a mask was taken on or off.
                    // Because Player updates early before most actors, the text ID being offered comes from the previous frame.
                    // If a mask was taken on or off the same frame this function runs, the wrong text will be used.
                    // This is especially important to prevent unwanted behavior with regards to mask trading.
                    this->currentMask = sSavedCurrentMask;
                    Player_StartTalking(play, talkOfferActor);
                    return true;
                }
            }
        }
    }

    return false;
}

s32 func_8083B8F4(Player* this, PlayState* play) {
    if (!(this->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_ON_HORSE)) &&
        Camera_CheckValidMode(Play_GetCamera(play, 0), 6)) {
        if ((this->actor.bgCheckFlags & 1) ||
            (func_808332B8(this) && (this->actor.yDistToWater < this->ageProperties->unk_2C))) {
            this->unk_6AD = 1;
            return 1;
        }
    }

    return 0;
}

s32 Player_ActionHandler_0(Player* this, PlayState* play) {
    if (this->unk_6AD != 0) {
        Player_ActionHandler_13(this, play);
        return 1;
    }

    if ((this->focusActor != NULL) && (CHECK_FLAG_ALL(this->focusActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_NAVI_HAS_INFO) ||
                                    (this->focusActor->naviEnemyId != 0xFF))) {
        this->stateFlags2 |= PLAYER_STATE2_NAVI_ALERT;
    } else if ((this->naviTextId == 0 || CVarGetInteger(CVAR_ENHANCEMENT("NaviOnL"), 0)) && !Player_CheckHostileLockOn(this) && CHECK_BTN_ALL(sControlInput->press.button, BTN_CUP) &&
               (YREG(15) != 0x10) &&
               (YREG(15) != 0x20) && !func_8083B8F4(this, play)) {
        func_80078884(NA_SE_SY_ERROR);
    }

    return 0;
}

void func_8083BA90(PlayState* play, Player* this, s32 arg2, f32 xzVelocity, f32 yVelocity) {
    func_80837948(play, this, arg2);
    Player_SetupAction(play, this, Player_Action_80844AF4, 0);

    this->stateFlags3 |= PLAYER_STATE3_MIDAIR;

    this->yaw = this->actor.shape.rot.y;
    this->linearVelocity = xzVelocity;
    this->actor.velocity.y = yVelocity;

    this->actor.bgCheckFlags &= ~1;
    this->hoverBootsTimer = 0;

    Player_PlayJumpingSfx(this);
    Player_PlayVoiceSfx(this, NA_SE_VO_LI_SWORD_L);
}

s32 func_8083BB20(Player* this) {
    if (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) && (Player_GetMeleeWeaponHeld(this) != 0)) {
        if (sUseHeldItem ||
            ((this->actor.category != ACTORCAT_PLAYER) && CHECK_BTN_ALL(sControlInput->press.button, BTN_B))) {
            return 1;
        }
    }

    return 0;
}

s32 func_8083BBA0(Player* this, PlayState* play) {
    if (func_8083BB20(this) && (sFloorType != 7)) {
        func_8083BA90(play, this, PLAYER_MWA_JUMPSLASH_START, 3.0f, 4.5f);
        return 1;
    }

    return 0;
}

void Player_SetupRoll(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_Roll, 0);
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime,
                                   GET_PLAYER_ANIM(PLAYER_ANIMGROUP_landing_roll, this->modelAnimType),
                                   1.25f * sWaterSpeedFactor);
    gSaveContext.sohStats.count[COUNT_ROLLS]++;
}

s32 Player_TryRoll(Player* this, PlayState* play) {
    if ((this->controlStickDirections[this->controlStickDataIndex] == 0) && (sFloorType != 7)) {
        Player_SetupRoll(this, play);

        return true;
    }

    return false;
}

void func_8083BCD0(Player* this, PlayState* play, s32 controlStickDirection) {
    func_80838940(this, D_80853D4C[controlStickDirection][0], !(controlStickDirection & 1) ? 5.8f : 3.5f, play, NA_SE_VO_LI_SWORD_N);

    if (controlStickDirection) {}

    this->av2.actionVar2 = 1;
    this->av1.actionVar1 = controlStickDirection;

    this->yaw = this->actor.shape.rot.y + (controlStickDirection << 0xE);
    this->linearVelocity = !(controlStickDirection & 1) ? 6.0f : 8.5f;

    this->stateFlags2 |= PLAYER_STATE2_HOPPING;

    Player_PlaySfx(this, ((controlStickDirection << 0xE) == 0x8000) ? NA_SE_PL_ROLL : NA_SE_PL_SKIP);
}

s32 Player_ActionHandler_10(Player* this, PlayState* play) {
    s32 controlStickDirection;

    if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) &&
        (play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) && (sFloorType != 7) &&
        (SurfaceType_GetSlope(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) != 1)) {
        controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];

        if (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) {
            if (Player_IsZTargeting(this)) {
                if (this->actor.category != ACTORCAT_PLAYER) {
                    if (controlStickDirection <= PLAYER_STICK_DIR_NONE) {
                        func_808389E8(this, &gPlayerAnim_link_normal_jump, REG(69) / 100.0f, play);
                    } else {
                        Player_SetupRoll(this, play);
                    }
                } else {
                    if ((Player_GetMeleeWeaponHeld(this) != 0) && Player_CanUpdateItems(this)) {
                        func_8083BA90(play, this, PLAYER_MWA_JUMPSLASH_START, 5.0f, 5.0f);
                    } else {
                        Player_SetupRoll(this, play);
                    }
                }

                return 1;
            }
        } else {
            func_8083BCD0(this, play, controlStickDirection);

            if (controlStickDirection == 1 || controlStickDirection == 3) {
                gSaveContext.sohStats.count[COUNT_SIDEHOPS]++;
            }
            if (controlStickDirection == 2) {
                gSaveContext.sohStats.count[COUNT_BACKFLIPS]++;
            }

            return 1;
        }
    }

    return 0;
}

void func_8083BF50(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 sp30;

    sp30 = this->unk_868 - 3.0f;
    if (sp30 < 0.0f) {
        sp30 += 29.0f;
    }

    if (sp30 < 14.0f) {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk_endL, this->modelAnimType);
        sp30 = 11.0f - sp30;
        if (sp30 < 0.0f) {
            sp30 = 1.375f * -sp30;
        }
        sp30 /= 11.0f;
    } else {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk_endR, this->modelAnimType);
        sp30 = 26.0f - sp30;
        if (sp30 < 0.0f) {
            sp30 = 2 * -sp30;
        }
        sp30 /= 12.0f;
    }

    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 0.0f, Animation_GetLastFrame(anim), ANIMMODE_ONCE,
                         4.0f * sp30);
    this->yaw = this->actor.shape.rot.y;
}

void func_8083C0B8(Player* this, PlayState* play) {
    func_80839FFC(this, play);
    func_8083BF50(this, play);
}

void func_8083C0E8(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_Idle, 1);
    Player_AnimPlayOnce(play, this, Player_GetIdleAnim(this));
    this->yaw = this->actor.shape.rot.y;
}

void func_8083C148(Player* this, PlayState* play) {
    if (!(this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT)) {
        func_8083B010(this);
        if (this->stateFlags1 & PLAYER_STATE1_IN_WATER) {
            func_80838F18(play, this);
        } else {
            func_80839F90(this, play);
        }
        if (this->unk_6AD < 4) {
            this->unk_6AD = 0;
        }
    }

    this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_FIRST_PERSON);
}

/**
 * Handles setting up a roll if it is appropriate.
 *
 * If a roll is not applicable, there are two extra behaviors that could occur:
 * - If an item is currently held in hand, it can be put away.
 * - Navi can be toggled on and off.
 *   She will either appear and fly around Link's head, or disappear by flying back into his hat
 *
 * These extra behaviors are not new actions themselves, so they will result in `false` being returned
 * even if they occur.
 */
s32 Player_ActionHandler_Roll(Player* this, PlayState* play) {
    if (!Player_UpdateHostileLockOn(this) && !sUpperBodyIsBusy && !(this->stateFlags1 & PLAYER_STATE1_ON_HORSE) &&
        CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
        if (Player_TryRoll(this, play)) {
            return true;
        } else if ((this->putAwayCooldownTimer == 0) && (this->heldItemAction >= PLAYER_IA_SWORD_MASTER)) {
            Player_UseItem(play, this, ITEM_NONE);
        } else {
            this->stateFlags2 ^= PLAYER_STATE2_NAVI_ACTIVE;
        }
    }

    return false;
}

s32 Player_ActionHandler_11(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 frame;

    if ((play->shootingGalleryStatus == 0) && (this->currentShield != PLAYER_SHIELD_NONE) &&
        CHECK_BTN_ALL(sControlInput->cur.button, BTN_R) &&
        (Player_IsChildWithHylianShield(this) ||
         (!Player_FriendlyLockOnOrParallel(this) && (this->focusActor == NULL)))) {

        func_80832318(this);
        Player_DetachHeldActor(play, this);

        if (Player_SetupAction(play, this, Player_Action_80843188, 0)) {
            this->stateFlags1 |= PLAYER_STATE1_SHIELDING;

            if (!Player_IsChildWithHylianShield(this)) {
                Player_SetModelsForHoldingShield(this);
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense, this->modelAnimType);
            } else {
                anim = &gPlayerAnim_clink_normal_defense_ALL;
            }

            if (anim != this->skelAnime.animation) {
                if (Player_CheckHostileLockOn(this)) {
                    this->unk_86C = 1.0f;
                } else {
                    this->unk_86C = 0.0f;
                    func_80833C3C(this);
                }
                this->upperLimbRot.x = this->upperLimbRot.y = this->upperLimbRot.z = 0;
            }

            frame = Animation_GetLastFrame(anim);
            LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);

            if (Player_IsChildWithHylianShield(this)) {
                Player_StartAnimMovement(play, this, 4);
            }

            Player_PlaySfx(this, NA_SE_IT_SHIELD_POSTURE);
        }

        return 1;
    }

    return 0;
}

s32 func_8083C484(Player* this, f32* arg1, s16* arg2) {
    s16 yaw = this->yaw - *arg2;

    if (ABS(yaw) > 0x6000) {
        if (Player_DecelerateToZero(this)) {
            *arg1 = 0.0f;
            *arg2 = this->yaw;
        } else {
            return 1;
        }
    }

    return 0;
}

void func_8083C50C(Player* this) {
    if ((this->unk_844 > 0) && !CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
        this->unk_844 = -this->unk_844;
    }
}

s32 Player_ActionHandler_8(Player* this, PlayState* play) {
    if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) && (Player_GetMeleeWeaponHeld(this) != 0) && (this->unk_844 == 1) &&
            (this->heldItemAction != PLAYER_IA_DEKU_STICK)) {
            if ((this->heldItemAction != PLAYER_IA_SWORD_BIGGORON) || (gSaveContext.swordHealth > 0.0f)) {
                func_808377DC(play, this);
                return 1;
            }
        }
    } else {
        func_8083C50C(this);
    }

    return 0;
}

s32 func_8083C61C(PlayState* play, Player* this) {
    if ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) && (this->actor.bgCheckFlags & 1) &&
        (AMMO(ITEM_NUT) != 0)) {
        Player_SetupAction(play, this, Player_Action_8084E604, 0);
        Player_AnimPlayOnce(play, this, &gPlayerAnim_link_normal_light_bom);
        this->unk_6AD = 0;
        return 1;
    }

    return 0;
}

typedef struct BottleSwingInfo {
    /* 0x00 */ LinkAnimationHeader* missAnimation;
    /* 0x04 */ LinkAnimationHeader* catchAnimation;
    /* 0x08 */ u8 firstActiveFrame;
    /* 0x09 */ u8 numActiveFrames;
} BottleSwingInfo; // size = 0x0C

static BottleSwingInfo sBottleSwingInfo[] = {
    { &gPlayerAnim_link_bottle_bug_miss, &gPlayerAnim_link_bottle_bug_in, 2, 3 },
    { &gPlayerAnim_link_bottle_fish_miss, &gPlayerAnim_link_bottle_fish_in, 5, 3 },
};

s32 func_8083C6B8(PlayState* play, Player* this) {
    if (sUseHeldItem) {
        if (Player_GetBottleHeld(this) >= 0) {
            Player_SetupAction(play, this, Player_Action_SwingBottle, 0);

            if (this->actor.yDistToWater > 12.0f) {
                this->av2.inWater = true;
            }

            Player_AnimPlayOnceAdjusted(play, this, sBottleSwingInfo[this->av2.inWater].missAnimation);

            Player_PlaySfx(this, NA_SE_IT_SWORD_SWING);
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_AUTO_JUMP);
            return 1;
        }

        if (this->heldItemAction == PLAYER_IA_FISHING_POLE) {
            Vec3f rodCheckPos = this->actor.world.pos;

            rodCheckPos.y += 50.0f;

            if (CVarGetInteger(CVAR_ENHANCEMENT("HoverFishing"), 0)
                    ? 0
                    : !(this->actor.bgCheckFlags & 1) || (this->actor.world.pos.z > 1300.0f) ||
                          BgCheck_SphVsFirstPoly(&play->colCtx, &rodCheckPos, 20.0f)) {
                func_80078884(NA_SE_SY_ERROR);
                return 0;
            }

            Player_SetupAction(play, this, Player_Action_80850C68, 0);
            this->unk_860 = 1;
            Player_ZeroSpeedXZ(this);
            Player_AnimPlayOnce(play, this, &gPlayerAnim_link_fishing_throw);
            return 1;
        } else {
            return 0;
        }
    }

    return 0;
}

void func_8083C858(Player* this, PlayState* play) {
    PlayerActionFunc actionFunc;

    if (Player_IsZTargeting(this)) {
        actionFunc = Player_Action_8084227C;
    } else {
        actionFunc = Player_Action_80842180;
    }

    Player_SetupAction(play, this, actionFunc, 1);
    Player_AnimChangeLoopMorph(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_run, this->modelAnimType));

    this->unk_89C = 0;
    this->unk_864 = this->unk_868 = 0.0f;
}

void func_8083C8DC(Player* this, PlayState* play, s16 arg2) {
    this->actor.shape.rot.y = this->yaw = arg2;
    func_8083C858(this, play);
}

s32 func_8083C910(PlayState* play, Player* this, f32 arg2) {
    WaterBox* sp2C;
    f32 sp28;

    sp28 = this->actor.world.pos.y;
    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp28,
                             &sp2C) != 0) {
        sp28 -= this->actor.world.pos.y;
        if (this->ageProperties->unk_24 <= sp28) {
            Player_SetupAction(play, this, Player_Action_8084D7C4, 0);
            Player_AnimChangeLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim);
            this->stateFlags1 |= PLAYER_STATE1_IN_WATER | PLAYER_STATE1_IN_CUTSCENE;
            this->av2.actionVar2 = 20;
            this->linearVelocity = 2.0f;
            Player_SetBootData(play, this);
            return 0;
        }
    }

    func_80838E70(play, this, arg2, this->actor.shape.rot.y);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    return 1;
}

void func_8083CA20(PlayState* play, Player* this) {
    if (func_8083C910(play, this, 180.0f)) {
        this->av2.actionVar2 = -20;
    }
}

void func_8083CA54(PlayState* play, Player* this) {
    this->linearVelocity = 2.0f;
    gSaveContext.entranceSpeed = 2.0f;
    if (func_8083C910(play, this, 120.0f)) {
        this->av2.actionVar2 = -15;
    }
}

void func_8083CA9C(PlayState* play, Player* this) {
    if (gSaveContext.entranceSpeed < 0.1f) {
        gSaveContext.entranceSpeed = 0.1f;
    }

    this->linearVelocity = gSaveContext.entranceSpeed;

    if (func_8083C910(play, this, 800.0f)) {
        this->av2.actionVar2 = -80 / this->linearVelocity;
        if (this->av2.actionVar2 < -20) {
            this->av2.actionVar2 = -20;
        }
    }
}

void func_8083CB2C(Player* this, s16 yaw, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_808414F8, 1);
    LinkAnimation_CopyJointToMorph(play, &this->skelAnime);
    this->unk_864 = this->unk_868 = 0.0f;
    this->yaw = yaw;
}

void func_8083CB94(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_80840DE4, 1);
    Player_AnimChangeLoopMorph(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType));
}

void func_8083CBF0(Player* this, s16 yaw, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_808423EC, 1);
    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_anchor_back_walk, 2.2f, 0.0f,
                         Animation_GetLastFrame(&gPlayerAnim_link_anchor_back_walk), ANIMMODE_ONCE, -6.0f);
    this->linearVelocity = 8.0f;
    this->yaw = yaw;
}

void func_8083CC9C(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_8084193C, 1);
    Player_AnimChangeLoopMorph(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkR, this->modelAnimType));
    this->unk_868 = 0.0f;
}

void func_8083CD00(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_8084251C, 1);
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, &gPlayerAnim_link_anchor_back_brake, 2.0f);
}

void Player_SetupTurnInPlace(PlayState* play, Player* this, s16 yaw) {
    this->yaw = yaw;

    Player_SetupAction(play, this, Player_Action_TurnInPlace, 1);

    this->turnRate = 1200;
    this->turnRate *= sWaterSpeedFactor; // slow turn rate by half when in water

    LinkAnimation_Change(play, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_45_turn][this->modelAnimType], 1.0f,
                         0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void func_8083CE0C(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;

    Player_SetupAction(play, this, Player_Action_Idle, 1);

    if (this->unk_870 < 0.5f) {
        anim = D_80853914[PLAYER_ANIMGROUP_waitR2wait][this->modelAnimType];
    } else {
        anim = D_80853914[PLAYER_ANIMGROUP_waitL2wait][this->modelAnimType];
    }
    Player_AnimPlayOnce(play, this, anim);

    this->yaw = this->actor.shape.rot.y;
}

void func_8083CEAC(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_80840450, 1);
    Player_AnimChangeOnceMorph(play, this, D_80853914[PLAYER_ANIMGROUP_wait2waitR][this->modelAnimType]);
    this->av2.actionVar2 = 1;
}

void func_8083CF10(Player* this, PlayState* play) {
    if (this->linearVelocity != 0.0f) {
        func_8083C858(this, play);
    } else {
        func_8083CE0C(this, play);
    }
}

void func_8083CF5C(Player* this, PlayState* play) {
    if (this->linearVelocity != 0.0f) {
        func_8083C858(this, play);
    } else {
        func_80839F90(this, play);
    }
}

s32 func_8083CFA8(PlayState* play, Player* this, f32 arg2, s32 splashScale) {
    f32 sp3C = fabsf(arg2);
    WaterBox* sp38;
    f32 sp34;
    Vec3f splashPos;
    s32 splashType;

    if (sp3C > 2.0f) {
        splashPos.x = this->bodyPartsPos[PLAYER_BODYPART_WAIST].x;
        splashPos.z = this->bodyPartsPos[PLAYER_BODYPART_WAIST].z;
        sp34 = this->actor.world.pos.y;
        if (WaterBox_GetSurface1(play, &play->colCtx, splashPos.x, splashPos.z, &sp34, &sp38)) {
            if ((sp34 - this->actor.world.pos.y) < 100.0f) {
                splashType = (sp3C <= 10.0f) ? 0 : 1;
                splashPos.y = sp34;
                EffectSsGSplash_Spawn(play, &splashPos, NULL, NULL, splashType, splashScale);
                return 1;
            }
        }
    }

    return 0;
}

void func_8083D0A8(PlayState* play, Player* this, f32 arg2) {
    this->stateFlags1 |= PLAYER_STATE1_JUMPING;
    this->stateFlags1 &= ~PLAYER_STATE1_IN_WATER;

    func_80832340(play, this);
    if (func_8083CFA8(play, this, arg2, 500)) {
        Player_PlaySfx(this, NA_SE_EV_JUMP_OUT_WATER);
    }

    Player_SetBootData(play, this);
}

s32 func_8083D12C(PlayState* play, Player* this, Input* arg2) {
    if (!(this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) && !(this->stateFlags2 & PLAYER_STATE2_UNDERWATER)) {
        if ((arg2 == NULL) || (CHECK_BTN_ALL(arg2->press.button, BTN_A) && (ABS(this->unk_6C2) < 12000) &&
                               (this->currentBoots != PLAYER_BOOTS_IRON))) {

            Player_SetupAction(play, this, Player_Action_8084DC48, 0);
            Player_AnimPlayOnce(play, this, &gPlayerAnim_link_swimer_swim_deep_start);

            this->unk_6C2 = 0;
            this->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
            this->actor.velocity.y = 0.0f;

            if (arg2 != NULL) {
                this->stateFlags2 |= PLAYER_STATE2_DIVING;
                Player_PlaySfx(this, NA_SE_PL_DIVE_BUBBLE);
            }

            return 1;
        }
    }

    if ((this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) || (this->stateFlags2 & PLAYER_STATE2_UNDERWATER)) {
        if (this->actor.velocity.y > 0.0f) {
            if (this->actor.yDistToWater < this->ageProperties->unk_30) {

                this->stateFlags2 &= ~PLAYER_STATE2_UNDERWATER;

                if (arg2 != NULL) {
                    Player_SetupAction(play, this, Player_Action_8084E1EC, 1);

                    if (this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) {
                        this->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_IN_CUTSCENE;
                    }

                    this->av2.actionVar2 = 2;
                }

                func_80832340(play, this);
                // Skip take breath animation on surface if Link didn't grab an item while underwater and the setting is enabled
                if (CVarGetInteger(CVAR_ENHANCEMENT("SkipSwimDeepEndAnim"), 0) && !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
                    int lastAnimFrame = Animation_GetLastFrame(&gPlayerAnim_link_swimer_swim_deep_end);
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_swimer_swim_deep_end, 1.0f,
                        lastAnimFrame, lastAnimFrame, ANIMMODE_ONCE, -6.0f);
                } else {
                    Player_AnimChangeOnceMorph(play, this,
                                (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) ? &gPlayerAnim_link_swimer_swim_get
                                                                        : &gPlayerAnim_link_swimer_swim_deep_end);
                }

                if (func_8083CFA8(play, this, this->actor.velocity.y, 500)) {
                    Player_PlaySfx(this, NA_SE_PL_FACE_UP);
                }

                return 1;
            }
        }
    }

    return 0;
}

void func_8083D330(PlayState* play, Player* this) {
    Player_AnimPlayLoop(play, this, &gPlayerAnim_link_swimer_swim);
    this->unk_6C2 = 16000;
    this->av2.actionVar2 = 1;
}

void func_8083D36C(PlayState* play, Player* this) {
    if ((this->currentBoots != PLAYER_BOOTS_IRON) || !(this->actor.bgCheckFlags & 1)) {
        func_80832564(play, this);

        if ((this->currentBoots != PLAYER_BOOTS_IRON) && (this->stateFlags2 & PLAYER_STATE2_UNDERWATER)) {
            this->stateFlags2 &= ~PLAYER_STATE2_UNDERWATER;
            func_8083D12C(play, this, 0);
            this->av1.actionVar1 = 1;
        } else if (Player_Action_80844A44 == this->actionFunc) {
            Player_SetupAction(play, this, Player_Action_8084DC48, 0);
            func_8083D330(play, this);
        } else {
            Player_SetupAction(play, this, Player_Action_8084D610, 1);
            Player_AnimChangeOnceMorph(play, this,
                          (this->actor.bgCheckFlags & 1) ? &gPlayerAnim_link_swimer_wait2swim_wait
                                                         : &gPlayerAnim_link_swimer_land2swim_wait);
        }
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_IN_WATER) || (this->actor.yDistToWater < this->ageProperties->unk_2C)) {
        if (func_8083CFA8(play, this, this->actor.velocity.y, 500)) {
            Player_PlaySfx(this, NA_SE_EV_DIVE_INTO_WATER);

            if (this->fallDistance > 800.0f) {
                Player_PlayVoiceSfx(this, NA_SE_VO_LI_CLIMB_END);
            }
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_IN_WATER;
    this->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
    this->stateFlags1 &= ~(PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALL);
    this->unk_854 = 0.0f;

    Player_SetBootData(play, this);
}

void func_8083D53C(PlayState* play, Player* this) {
    if (this->actor.yDistToWater < this->ageProperties->unk_2C) {
        Audio_SetBaseFilter(0);
        this->underwaterTimer = 0;
    } else {
        Audio_SetBaseFilter(0x20);
        if (this->underwaterTimer < 300) {
            this->underwaterTimer++;
        }
    }

    if ((Player_Action_80845668 != this->actionFunc) && (Player_Action_8084BDFC != this->actionFunc)) {
        if (this->ageProperties->unk_2C < this->actor.yDistToWater) {
            if (!(this->stateFlags1 & PLAYER_STATE1_IN_WATER) ||
                (!((this->currentBoots == PLAYER_BOOTS_IRON) && (this->actor.bgCheckFlags & 1)) &&
                 (Player_Action_8084E30C != this->actionFunc) && (Player_Action_8084E368 != this->actionFunc) &&
                 (Player_Action_8084D610 != this->actionFunc) && (Player_Action_8084D84C != this->actionFunc) &&
                 (Player_Action_8084DAB4 != this->actionFunc) && (Player_Action_8084DC48 != this->actionFunc) &&
                 (Player_Action_8084E1EC != this->actionFunc) && (Player_Action_8084D7C4 != this->actionFunc))) {
                func_8083D36C(play, this);
                return;
            }
        } else if ((this->stateFlags1 & PLAYER_STATE1_IN_WATER) && (this->actor.yDistToWater < this->ageProperties->unk_24)) {
            if ((this->skelAnime.moveFlags == 0) && (this->currentBoots != PLAYER_BOOTS_IRON)) {
                Player_SetupTurnInPlace(play, this, this->actor.shape.rot.y);
            }
            func_8083D0A8(play, this, this->actor.velocity.y);
        }
    }
}

void func_8083D6EC(PlayState* play, Player* this) {
    Vec3f ripplePos;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;

    this->actor.minVelocityY = -20.0f;
    this->actor.gravity = REG(68) / 100.0f;

    if (func_8083816C(sFloorType)) {
        temp1 = fabsf(this->linearVelocity) * 20.0f;
        temp3 = 0.0f;

        if (sFloorType == 4) {
            if (this->unk_6C4 > 1300.0f) {
                temp2 = this->unk_6C4;
            } else {
                temp2 = 1300.0f;
            }
            if (this->currentBoots == PLAYER_BOOTS_HOVER) {
                temp1 += temp1;
            } else if (this->currentBoots == PLAYER_BOOTS_IRON) {
                temp1 *= 0.3f;
            }
        } else {
            temp2 = 20000.0f;
            if (this->currentBoots != PLAYER_BOOTS_HOVER) {
                temp1 += temp1;
            } else if ((sFloorType == 7) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
                temp1 = 0;
            }
        }

        if (this->currentBoots != PLAYER_BOOTS_HOVER) {
            temp3 = (temp2 - this->unk_6C4) * 0.02f;
            temp3 = CLAMP(temp3, 0.0f, 300.0f);
            if (this->currentBoots == PLAYER_BOOTS_IRON) {
                temp3 += temp3;
            }
        }

        this->unk_6C4 += temp3 - temp1;
        this->unk_6C4 = CLAMP(this->unk_6C4, 0.0f, temp2);

        this->actor.gravity -= this->unk_6C4 * 0.004f;
    } else {
        this->unk_6C4 = 0.0f;
    }

    if (this->actor.bgCheckFlags & 0x20) {
        if (this->actor.yDistToWater < 50.0f) {
            temp4 = fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].x - this->unk_A88.x) +
                    fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].y - this->unk_A88.y) +
                    fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].z - this->unk_A88.z);
            if (temp4 > 4.0f) {
                temp4 = 4.0f;
            }
            this->unk_854 += temp4;

            if (this->unk_854 > 15.0f) {
                this->unk_854 = 0.0f;

                ripplePos.x = (Rand_ZeroOne() * 10.0f) + this->actor.world.pos.x;
                ripplePos.y = this->actor.world.pos.y + this->actor.yDistToWater;
                ripplePos.z = (Rand_ZeroOne() * 10.0f) + this->actor.world.pos.z;
                EffectSsGRipple_Spawn(play, &ripplePos, 100, 500, 0);

                if ((this->linearVelocity > 4.0f) && !func_808332B8(this) &&
                    ((this->actor.world.pos.y + this->actor.yDistToWater) <
                     this->bodyPartsPos[PLAYER_BODYPART_WAIST].y)) {
                    func_8083CFA8(play, this, 20.0f,
                                  (fabsf(this->linearVelocity) * 50.0f) + (this->actor.yDistToWater * 5.0f));
                }
            }
        }

        if (this->actor.yDistToWater > 40.0f) {
            s32 numBubbles = 0;
            s32 i;

            if ((this->actor.velocity.y > -1.0f) || (this->actor.bgCheckFlags & 1)) {
                if (Rand_ZeroOne() < 0.2f) {
                    numBubbles = 1;
                }
            } else {
                numBubbles = this->actor.velocity.y * -2.0f;
            }

            for (i = 0; i < numBubbles; i++) {
                EffectSsBubble_Spawn(play, &this->actor.world.pos, 20.0f, 10.0f, 20.0f, 0.13f);
            }
        }
    }
}

s32 func_8083DB98(Player* this, s32 arg1) {
    Actor* focusActor = this->focusActor;
    Vec3f playerHeadPos;
    s16 targetFocusRotX;
    s16 targetFocusRotY;

    playerHeadPos.x = this->actor.world.pos.x;
    playerHeadPos.y = this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    playerHeadPos.z = this->actor.world.pos.z;

    targetFocusRotX = Math_Vec3f_Pitch(&playerHeadPos, &focusActor->focus.pos);
    targetFocusRotY = Math_Vec3f_Yaw(&playerHeadPos, &focusActor->focus.pos);

    Math_SmoothStepToS(&this->actor.focus.rot.y, targetFocusRotY, 4, 10000, 0);
    Math_SmoothStepToS(&this->actor.focus.rot.x, targetFocusRotX, 4, 10000, 0);
    this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_Y;

    return func_80836AB8(this, arg1);
}

static Vec3f D_8085456C = { 0.0f, 100.0f, 40.0f };

void func_8083DC54(Player* this, PlayState* play) {
    s16 sp46;
    s16 temp2;
    f32 temp1;
    Vec3f sp34;

    if (this->focusActor != NULL) {
        if (func_8002DD78(this) || func_808334B4(this)) {
            func_8083DB98(this, true);
        } else {
            func_8083DB98(this, false);
        }
        return;
    }

    if (sFloorType == 11) {
        Math_SmoothStepToS(&this->actor.focus.rot.x, -20000, 10, 4000, 800);
    } else {
        sp46 = 0;
        temp1 = func_8083973C(play, this, &D_8085456C, &sp34);
        if (temp1 > BGCHECK_Y_MIN) {
            temp2 = Math_Atan2S(40.0f, this->actor.world.pos.y - temp1);
            sp46 = CLAMP(temp2, -4000, 4000);
        }
        this->actor.focus.rot.y = this->actor.shape.rot.y;
        Math_SmoothStepToS(&this->actor.focus.rot.x, sp46, 14, 4000, 30);
    }

    func_80836AB8(this, func_8002DD78(this) || func_808334B4(this));
}

void func_8083DDC8(Player* this, PlayState* play) {
    if (!func_8002DD78(this) && !func_808334B4(this) && (this->linearVelocity > 5.0f)) {
        s16 targetPitch;
        s16 targetRoll;

        targetPitch = this->linearVelocity * 200.0f;
        targetRoll = (s16)(this->yaw - this->actor.shape.rot.y) * this->linearVelocity * 0.1f;

        targetPitch = CLAMP(targetPitch, -4000, 4000);
        targetRoll = CLAMP(-targetRoll, -4000, 4000);

        Math_ScaledStepToS(&this->upperLimbRot.x, targetPitch, 900);
        this->headLimbRot.x = -(f32)this->upperLimbRot.x * 0.5f;

        Math_ScaledStepToS(&this->headLimbRot.z, targetRoll, 300);
        Math_ScaledStepToS(&this->upperLimbRot.z, targetRoll, 200);

        this->unk_6AE_rotFlags |= UNK6AE_ROT_HEAD_X | UNK6AE_ROT_HEAD_Z | UNK6AE_ROT_UPPER_X | UNK6AE_ROT_UPPER_Z;
    } else {
        func_8083DC54(this, play);
    }
}

void func_8083DF68(Player* this, f32 arg1, s16 arg2) {
    Math_AsymStepToF(&this->linearVelocity, arg1, REG(19) / 100.0f, 1.5f);
    Math_ScaledStepToS(&this->yaw, arg2, REG(27));
}

void func_8083DFE0(Player* this, f32* arg1, s16* arg2) {
    s16 yawDiff = this->yaw - *arg2;

    if (this->meleeWeaponState == 0) {
        float maxSpeed = R_RUN_SPEED_LIMIT / 100.0f;

        int32_t giSpeedModifier = GameInteractor_RunSpeedModifier();
        if (giSpeedModifier != 0) {
            if (giSpeedModifier > 0) {
                maxSpeed *= giSpeedModifier;
            } else {
                maxSpeed /= abs(giSpeedModifier);
            }
        }

        if (CVarGetInteger(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_VANILLA) == BUNNY_HOOD_FAST_AND_JUMP && this->currentMask == PLAYER_MASK_BUNNY) {
            maxSpeed *= 1.5f;
        } 
        
        if (CVarGetInteger(CVAR_SETTING("WalkModifier.Enabled"), 0) && !CVarGetInteger(CVAR_SETTING("WalkModifier.DoesntChangeJump"), 0)) {
            if (CVarGetInteger(CVAR_SETTING("WalkModifier.SpeedToggle"), 0)) {
                if (gWalkSpeedToggle1) {
                    maxSpeed *= CVarGetFloat(CVAR_SETTING("WalkModifier.Mapping1"), 1.0f);
                } else if (gWalkSpeedToggle2) {
                    maxSpeed *= CVarGetFloat(CVAR_SETTING("WalkModifier.Mapping2"), 1.0f);
                }
            } else {
                if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_CUSTOM_MODIFIER1)) {
                    maxSpeed *= CVarGetFloat(CVAR_SETTING("WalkModifier.Mapping1"), 1.0f);
                } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_CUSTOM_MODIFIER2)) {
                    maxSpeed *= CVarGetFloat(CVAR_SETTING("WalkModifier.Mapping2"), 1.0f);
                }
            }
        }

        this->linearVelocity = CLAMP(this->linearVelocity, -maxSpeed, maxSpeed);
    }

    if (ABS(yawDiff) > 0x6000) {
        if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
            this->yaw = *arg2;
        }
    } else {
        Math_AsymStepToF(&this->linearVelocity, *arg1, 0.05f, 0.1f);
        Math_ScaledStepToS(&this->yaw, *arg2, 200);
    }
}

static struct_80854578 D_80854578[] = {
    { &gPlayerAnim_link_uma_left_up, 35.17f, 6.6099997f },
    { &gPlayerAnim_link_uma_right_up, -34.16f, 7.91f },
};

s32 Player_ActionHandler_3(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    f32 unk_04;
    f32 unk_08;
    f32 sp38;
    f32 sp34;
    s32 temp;

    if ((rideActor != NULL) && CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
        sp38 = Math_CosS(rideActor->actor.shape.rot.y);
        sp34 = Math_SinS(rideActor->actor.shape.rot.y);

        Player_SetupWaitForPutAway(play, this, func_8083A360);

        this->stateFlags1 |= PLAYER_STATE1_ON_HORSE;
        this->actor.bgCheckFlags &= ~0x20;

        if (this->mountSide < 0) {
            temp = 0;
        } else {
            temp = 1;
        }

        unk_04 = D_80854578[temp].unk_04;
        unk_08 = D_80854578[temp].unk_08;
        this->actor.world.pos.x =
            rideActor->actor.world.pos.x + rideActor->riderPos.x + ((unk_04 * sp38) + (unk_08 * sp34));
        this->actor.world.pos.z =
            rideActor->actor.world.pos.z + rideActor->riderPos.z + ((unk_08 * sp38) - (unk_04 * sp34));

        this->unk_878 = rideActor->actor.world.pos.y - this->actor.world.pos.y;
        this->yaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

        Actor_MountHorse(play, this, &rideActor->actor);
        Player_AnimPlayOnce(play, this, D_80854578[temp].anim);
        Player_StartAnimMovement(play, this, 0x9B);
        this->actor.parent = this->rideActor;
        func_80832224(this);
        Actor_DisableLens(play);
        return 1;
    }

    return 0;
}

void Player_GetSlopeDirection(CollisionPoly* floorPoly, Vec3f* slopeNormal, s16* downwardSlopeYaw) {
    slopeNormal->x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
    slopeNormal->y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
    slopeNormal->z = COLPOLY_GET_NORMAL(floorPoly->normal.z);

    *downwardSlopeYaw = Math_Atan2S(slopeNormal->z, slopeNormal->x);
}

s32 Player_HandleSlopes(PlayState* play, Player* this, CollisionPoly* floorPoly) {
    static LinkAnimationHeader* sSlopeSlideAnims[] = {
        &gPlayerAnim_link_normal_down_slope_slip,
        &gPlayerAnim_link_normal_up_slope_slip,
    };
    s32 pad;
    s16 playerVelYaw;
    Vec3f slopeNormal;
    s16 downwardSlopeYaw;
    f32 slopeSlowdownSpeed;
    f32 slopeSlowdownSpeedStep;
    s16 velYawToDownwardSlope;

    if (!Player_InBlockingCsMode(play, this) && (Player_Action_SlideOnSlope != this->actionFunc) &&
        (SurfaceType_GetSlope(&play->colCtx, floorPoly, this->actor.floorBgId) == 1)) {
        // Get direction of movement relative to the downward direction of the slope
        playerVelYaw = Math_Atan2S(this->actor.velocity.z, this->actor.velocity.x);
        Player_GetSlopeDirection(floorPoly, &slopeNormal, &downwardSlopeYaw);
        velYawToDownwardSlope = downwardSlopeYaw - playerVelYaw;

        if (ABS(velYawToDownwardSlope) > 0x3E80) { // 87.9 degrees
            // moving parallel or upwards on the slope, player does not slip but does slow down
            slopeSlowdownSpeed = (1.0f - slopeNormal.y) * 40.0f;
            slopeSlowdownSpeedStep = (slopeSlowdownSpeed * slopeSlowdownSpeed) * 0.015f;

            if (slopeSlowdownSpeedStep < 1.2f) {
                slopeSlowdownSpeedStep = 1.2f;
            }

            // slows down speed as player is climbing a slope
            this->pushedYaw = downwardSlopeYaw;
            Math_StepToF(&this->pushedSpeed, slopeSlowdownSpeed, slopeSlowdownSpeedStep);
        } else {
            // moving downward on the slope, causing player to slip
            Player_SetupAction(play, this, Player_Action_SlideOnSlope, 0);
            func_80832564(play, this);

            if (sFloorShapePitch >= 0) {
                this->av1.actionVar1 = 1;
            }
            Player_AnimChangeLoopMorph(play, this, sSlopeSlideAnims[this->av1.actionVar1]);
            this->linearVelocity = sqrtf(SQ(this->actor.velocity.x) + SQ(this->actor.velocity.z));
            this->yaw = playerVelYaw;
            return true;
        }
    }

    return false;
}

// unknown data (unused)
static s32 D_80854598[] = {
    0xFFDB0871, 0xF8310000, 0x00940470, 0xF3980000, 0xFFB504A9, 0x0C9F0000, 0x08010402,
};

void func_8083E4C4(PlayState* play, Player* this, GetItemEntry* giEntry) {
    s32 dropType = giEntry->field & 0x1F;

    if (!(giEntry->field & 0x80)) {
        Item_DropCollectible(play, &this->actor.world.pos, dropType | 0x8000);
        if ((dropType != 4) && (dropType != 8) && (dropType != 9) && (dropType != 0xA) && (dropType != 0) && (dropType != 1) && (dropType != 2) &&
            (dropType != 0x14) && (dropType != 0x13)) {
            Item_Give(play, giEntry->itemId);
        }
    } else {
        Item_Give(play, giEntry->itemId);
    }
    func_80078884((this->getItemId < 0 || this->getItemEntry.getItemId < 0) ? NA_SE_SY_GET_BOXITEM : NA_SE_SY_GET_ITEM);
}

s32 Player_ActionHandler_2(Player* this, PlayState* play) {
    Actor* interactedActor;

    if(gSaveContext.pendingIceTrapCount) {
        gSaveContext.pendingIceTrapCount--;
        GameInteractor_ExecuteOnItemReceiveHooks(ItemTable_RetrieveEntry(MOD_RANDOMIZER, RG_ICE_TRAP));
        if (CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0)) {
            return 1;
        }
        this->stateFlags1 &= ~(PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_CARRYING_ACTOR);
        this->actor.colChkInfo.damage = 0;
        func_80837C0C(play, this, 3, 0.0f, 0.0f, 0, 20);
        this->getItemId = GI_NONE;
        this->getItemEntry = (GetItemEntry) GET_ITEM_NONE;
        // Gameplay stats: Increment Ice Trap count
        gSaveContext.sohStats.count[COUNT_ICE_TRAPS]++;
        return 1;
    }

    if (iREG(67) || (((interactedActor = this->interactRangeActor) != NULL) &&
                     func_8002D53C(play, &play->actorCtx.titleCtx))) {
        if (iREG(67) || (this->getItemId > GI_NONE)) {
            if (iREG(67)) {
                this->getItemId = iREG(68);
            }

            GetItemEntry giEntry;
            if (this->getItemEntry.objectId == OBJECT_INVALID || (this->getItemId != this->getItemEntry.getItemId)) {
                giEntry = ItemTable_Retrieve(this->getItemId);
            } else {
                giEntry = this->getItemEntry;
            }
            if (giEntry.collectable) {
                if ((interactedActor != &this->actor) && !iREG(67)) {
                    interactedActor->parent = &this->actor;
                }

                iREG(67) = false;

                if (IS_RANDO && giEntry.getItemId == RG_ICE_TRAP && giEntry.getItemFrom == ITEM_FROM_FREESTANDING) {
                    this->actor.freezeTimer = 30;
                    Player_SetPendingFlag(this, play);
                    Message_StartTextbox(play, 0xF8, NULL);
                    Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
                    gSaveContext.pendingIceTrapCount++;
                    return 1;
                }

                // Show the cutscene for picking up an item. In vanilla, this happens in bombchu bowling alley (because getting bombchus need to show the cutscene)
                // and whenever the player doesn't have the item yet. In rando, we're overruling this because we need to keep showing the cutscene
                // because those items can be randomized and thus it's important to keep showing the cutscene.
                uint8_t showItemCutscene = play->sceneNum == SCENE_BOMBCHU_BOWLING_ALLEY || Item_CheckObtainability(giEntry.itemId) == ITEM_NONE || IS_RANDO;

                // Only skip cutscenes for drops when they're items/consumables from bushes/rocks/enemies.
                uint8_t isDropToSkip = 
                                        (
                                            interactedActor->id == ACTOR_EN_ITEM00 &&
                                            interactedActor->params != ITEM00_HEART_PIECE &&
                                            interactedActor->params != ITEM00_SMALL_KEY &&
                                            interactedActor->params != ITEM00_SOH_GIVE_ITEM_ENTRY &&
                                            interactedActor->params != ITEM00_SOH_GIVE_ITEM_ENTRY_GI
                                        ) ||
                                        interactedActor->id == ACTOR_EN_KAREBABA || 
                                        interactedActor->id == ACTOR_EN_DEKUBABA;

                // Skip cutscenes from picking up consumables with "Fast Pickup Text" enabled, even when the player never picked it up before.
                // But only for bushes/rocks/enemies because otherwise it can lead to softlocks in deku mask theatre and potentially other places.
                uint8_t skipItemCutscene = CVarGetInteger(CVAR_ENHANCEMENT("FastDrops"), 0) && isDropToSkip;

                // Same as above but for rando. Rando is different because we want to enable cutscenes for items that the player already has because
                // those items could be a randomized item coming from scrubs, freestanding PoH's and keys. So we need to once again overrule
                // this specifically for items coming from bushes/rocks/enemies when the player has already picked that item up.
                uint8_t skipItemCutsceneRando = IS_RANDO && Item_CheckObtainability(giEntry.itemId) != ITEM_NONE && isDropToSkip;

                // Show cutscene when picking up a item.
                if (showItemCutscene && !skipItemCutscene && !skipItemCutsceneRando) {

                    Player_DetachHeldActor(play, this);
                    func_8083AE40(this, giEntry.objectId);

                    if (!(this->stateFlags2 & PLAYER_STATE2_UNDERWATER) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
                        Player_SetupWaitForPutAway(play, this, func_8083A434);
                        Player_AnimPlayOnceAdjusted(play, this, &gPlayerAnim_link_demo_get_itemB);
                        func_80835EA4(play, 9);
                    }

                    this->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_IN_CUTSCENE;
                    func_80832224(this);
                    return 1;
                }

                // Don't show cutscene when picking up an item.
                func_8083E4C4(play, this, &giEntry);
                this->getItemId = GI_NONE;
                this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
            }
        } else if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) && !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) &&
                   !(this->stateFlags2 & PLAYER_STATE2_UNDERWATER)) {
            if (this->getItemId != GI_NONE) {
                GetItemEntry giEntry;
                if (this->getItemEntry.objectId == OBJECT_INVALID) {
                    giEntry = ItemTable_Retrieve(-this->getItemId);
                } else {
                    giEntry = this->getItemEntry;
                }
                EnBox* chest = (EnBox*)interactedActor;
                if (giEntry.itemId != ITEM_NONE) {
                    if (((Item_CheckObtainability(giEntry.itemId) == ITEM_NONE) && (giEntry.field & 0x40)) ||
                        ((Item_CheckObtainability(giEntry.itemId) != ITEM_NONE) && (giEntry.field & 0x20))) {
                        this->getItemId = -GI_RUPEE_BLUE;
                        giEntry = ItemTable_Retrieve(GI_RUPEE_BLUE);
                    }
                }

                if (GameInteractor_Should(VB_GIVE_ITEM_FROM_CHEST, true, chest)) {
                    Player_SetupWaitForPutAway(play, this, func_8083A434);
                }
                this->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_IN_CUTSCENE;
                func_8083AE40(this, giEntry.objectId);

                this->actor.world.pos.x =
                    chest->dyna.actor.world.pos.x - (Math_SinS(chest->dyna.actor.shape.rot.y) * 29.4343f);
                this->actor.world.pos.z =
                    chest->dyna.actor.world.pos.z - (Math_CosS(chest->dyna.actor.shape.rot.y) * 29.4343f);
                this->yaw = this->actor.shape.rot.y = chest->dyna.actor.shape.rot.y;
                func_80832224(this);

                bool vanillaPlaySlowChestCS = (giEntry.itemId != ITEM_NONE) && (giEntry.gi >= 0) &&
                    (Item_CheckObtainability(giEntry.itemId) == ITEM_NONE);

                if (GameInteractor_Should(VB_PLAY_SLOW_CHEST_CS, vanillaPlaySlowChestCS, chest)) {
                    Player_AnimPlayOnceAdjusted(play, this, this->ageProperties->unk_98);
                    Player_StartAnimMovement(play, this, 0x28F);
                    chest->unk_1F4 = 1;
                    Camera_ChangeSetting(Play_GetCamera(play, 0), CAM_SET_SLOW_CHEST_CS);
                } else {
                    Player_AnimPlayOnce(play, this, &gPlayerAnim_link_normal_box_kick);
                    chest->unk_1F4 = -1;
                }

                return 1;
            }

            if ((this->heldActor == NULL) || Player_HoldsHookshot(this)) {
                if ((interactedActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT) {
                    s32 sp24 = this->itemAction;

                    this->itemAction = PLAYER_IA_NONE;
                    this->modelAnimType = PLAYER_ANIMTYPE_0;
                    this->heldItemAction = this->itemAction;
                    Player_SetupWaitForPutAway(play, this, func_8083A0F4);

                    if (sp24 == PLAYER_IA_SWORD_MASTER) {
                        this->nextModelGroup = Player_ActionToModelGroup(this, PLAYER_IA_SWORD_CS);
                        Player_InitItemAction(play, this, PLAYER_IA_SWORD_CS);
                    } else {
                        Player_UseItem(play, this, ITEM_LAST_USED);
                    }
                } else {
                    s32 strength = Player_GetStrength();

                    if ((interactedActor->id == ACTOR_EN_ISHI) && ((interactedActor->params & 0xF) == 1) &&
                        (strength < PLAYER_STR_SILVER_G)) {
                        return 0;
                    }

                    Player_SetupWaitForPutAway(play, this, func_8083A0F4);
                }

                func_80832224(this);
                this->stateFlags1 |= PLAYER_STATE1_CARRYING_ACTOR;
                return 1;
            }
        }
    }

    return 0;
}

void func_8083EA94(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_80846578, 1);
    Player_AnimPlayOnce(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_throw, this->modelAnimType));
}

s32 func_8083EAF0(Player* this, Actor* actor) {
    if ((actor != NULL) && !(actor->flags & ACTOR_FLAG_ALWAYS_THROWN) &&
        ((this->linearVelocity < 1.1f) || (actor->id == ACTOR_EN_BOM_CHU))) {
        return 0;
    }

    return 1;
}

s32 Player_ActionHandler_9(Player* this, PlayState* play) {
    u16 buttonsToCheck = BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) != 0) {
        buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
    }
    if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->heldActor != NULL) &&
        CHECK_BTN_ANY(sControlInput->press.button, buttonsToCheck)) {
        if (!func_80835644(play, this, this->heldActor)) {
            if (!func_8083EAF0(this, this->heldActor)) {
                Player_SetupAction(play, this, Player_Action_808464B0, 1);
                Player_AnimPlayOnce(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_put, this->modelAnimType));
            } else {
                func_8083EA94(this, play);
            }
        }
        return 1;
    }

    return 0;
}

s32 func_8083EC18(Player* this, PlayState* play, u32 wallFlags) {
    if (this->yDistToLedge >= 79.0f) {
        if (!(this->stateFlags1 & PLAYER_STATE1_IN_WATER) || (this->currentBoots == PLAYER_BOOTS_IRON) ||
            (this->actor.yDistToWater < this->ageProperties->unk_2C)) {
            s32 sp8C = (wallFlags & 8) ? 2 : 0;

            if ((sp8C != 0) || (wallFlags & 2) ||
                func_80041E4C(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) {
                f32 phi_f20;
                CollisionPoly* wallPoly = this->actor.wallPoly;
                f32 sp80;
                f32 sp7C;
                f32 phi_f12;
                f32 phi_f14;

                phi_f20 = phi_f12 = 0.0f;

                if (sp8C != 0) {
                    sp80 = this->actor.world.pos.x;
                    sp7C = this->actor.world.pos.z;
                } else {
                    Vec3f sp50[3];
                    s32 i;
                    f32 sp48;
                    Vec3f* sp44 = &sp50[0];
                    s32 pad;

                    CollisionPoly_GetVerticesByBgId(wallPoly, this->actor.wallBgId, &play->colCtx, sp50);

                    sp80 = phi_f12 = sp44->x;
                    sp7C = phi_f14 = sp44->z;
                    phi_f20 = sp44->y;
                    for (i = 1; i < 3; i++) {
                        sp44++;
                        if (sp80 > sp44->x) {
                            sp80 = sp44->x;
                        } else if (phi_f12 < sp44->x) {
                            phi_f12 = sp44->x;
                        }

                        if (sp7C > sp44->z) {
                            sp7C = sp44->z;
                        } else if (phi_f14 < sp44->z) {
                            phi_f14 = sp44->z;
                        }

                        if (phi_f20 > sp44->y) {
                            phi_f20 = sp44->y;
                        }
                    }

                    sp80 = (sp80 + phi_f12) * 0.5f;
                    sp7C = (sp7C + phi_f14) * 0.5f;

                    phi_f12 = ((this->actor.world.pos.x - sp80) * COLPOLY_GET_NORMAL(wallPoly->normal.z)) -
                              ((this->actor.world.pos.z - sp7C) * COLPOLY_GET_NORMAL(wallPoly->normal.x));
                    sp48 = this->actor.world.pos.y - phi_f20;

                    phi_f20 = ((f32)(s32)((sp48 / 15.000000223517418) + 0.5) * 15.000000223517418) - sp48;
                    phi_f12 = fabsf(phi_f12);
                }

                if (phi_f12 < 8.0f) {
                    f32 wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                    f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                    f32 sp34 = this->distToInteractWall;
                    LinkAnimationHeader* anim;

                    Player_SetupWaitForPutAway(play, this, func_8083A3B0);
                    this->stateFlags1 |= PLAYER_STATE1_CLIMBING_LADDER;
                    this->stateFlags1 &= ~PLAYER_STATE1_IN_WATER;

                    if ((sp8C != 0) || (wallFlags & 2)) {
                        if ((this->av1.actionVar1 = sp8C) != 0) {
                            if (this->actor.bgCheckFlags & 1) {
                                anim = &gPlayerAnim_link_normal_Fclimb_startA;
                            } else {
                                anim = &gPlayerAnim_link_normal_Fclimb_hold2upL;
                            }
                            sp34 = (this->ageProperties->wallCheckRadius - 1.0f) - sp34;
                        } else {
                            anim = this->ageProperties->unk_A4;
                            sp34 = sp34 - 1.0f;
                        }
                        this->av2.actionVar2 = -2;
                        this->actor.world.pos.y += phi_f20;
                        this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
                    } else {
                        anim = this->ageProperties->unk_A8;
                        this->av2.actionVar2 = -4;
                        this->actor.shape.rot.y = this->yaw = this->actor.wallYaw;
                    }

                    this->actor.world.pos.x = (sp34 * wallPolyNormalX) + sp80;
                    this->actor.world.pos.z = (sp34 * wallPolyNormalZ) + sp7C;
                    func_80832224(this);
                    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
                    Player_AnimPlayOnce(play, this, anim);
                    Player_StartAnimMovement(play, this, 0x9F);

                    return true;
                }
            }
        }
    }

    return false;
}

void func_8083F070(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    Player_SetupActionPreserveAnimMovement(play, this, Player_Action_8084C5F8, 0);
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, (4.0f / 3.0f));
}

/**
 * @return true if Player chooses to enter crawlspace
 */
s32 Player_TryEnteringCrawlspace(Player* this, PlayState* play, u32 interactWallFlags) {
    CollisionPoly* wallPoly;
    Vec3f wallVertices[3];
    f32 xVertex1;
    f32 xVertex2;
    f32 zVertex1;
    f32 zVertex2;
    s32 i;

    if (!LINK_IS_ADULT && !(this->stateFlags1 & PLAYER_STATE1_IN_WATER) && (interactWallFlags & 0x30)) {
        wallPoly = this->actor.wallPoly;
        CollisionPoly_GetVerticesByBgId(wallPoly, this->actor.wallBgId, &play->colCtx, wallVertices);

        // Determines min and max vertices for x & z (edges of the crawlspace hole)
        xVertex1 = xVertex2 = wallVertices[0].x;
        zVertex1 = zVertex2 = wallVertices[0].z;
        for (i = 1; i < 3; i++) {
            if (xVertex1 > wallVertices[i].x) {
                // Update x min
                xVertex1 = wallVertices[i].x;
            } else if (xVertex2 < wallVertices[i].x) {
                // Update x max
                xVertex2 = wallVertices[i].x;
            }

            if (zVertex1 > wallVertices[i].z) {
                // Update z min
                zVertex1 = wallVertices[i].z;
            } else if (zVertex2 < wallVertices[i].z) {
                // Update z max
                zVertex2 = wallVertices[i].z;
            }
        }

        // XZ Center of the crawlspace hole
        xVertex1 = (xVertex1 + xVertex2) * 0.5f;
        zVertex1 = (zVertex1 + zVertex2) * 0.5f;

        // Perpendicular (sideways) XZ-Distance from player pos to crawlspace line
        // Uses y-component of crossproduct formula for the distance from a point to a line
        xVertex2 = ((this->actor.world.pos.x - xVertex1) * COLPOLY_GET_NORMAL(wallPoly->normal.z)) -
               ((this->actor.world.pos.z - zVertex1) * COLPOLY_GET_NORMAL(wallPoly->normal.x));

        if (fabsf(xVertex2) < 8.0f) {
            // Give do-action prompt to "Enter on A" for the crawlspace
            this->stateFlags2 |= PLAYER_STATE2_DO_ACTION_ENTER;

            if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
                // Enter Crawlspace
                f32 wallPolyNormX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 wallPolyNormZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 distToInteractWall = this->distToInteractWall;

                Player_SetupWaitForPutAway(play, this, func_8083A40C);
                this->stateFlags2 |= PLAYER_STATE2_CRAWLING;
                this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
                this->actor.world.pos.x = xVertex1 + (distToInteractWall * wallPolyNormX);
                this->actor.world.pos.z = zVertex1 + (distToInteractWall * wallPolyNormZ);
                func_80832224(this);
                this->actor.prevPos = this->actor.world.pos;
                // #region SOH [Enhancement]
                if (CVarGetInteger(CVAR_ENHANCEMENT("CrawlSpeed"), 1) > 1) {
                    // increase animation speed when entering a tunnel
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_child_tunnel_start,
                                         ((CVarGetInteger(CVAR_ENHANCEMENT("CrawlSpeed"), 1) + 1.0f) / 2.0f), 0.0f,
                                         Animation_GetLastFrame(&gPlayerAnim_link_child_tunnel_start), ANIMMODE_ONCE,
                                         0.0f);
                    Player_StartAnimMovement(play, this, 0x9D);
                // #endregion
                } else {
                    Player_AnimPlayOnce(play, this, &gPlayerAnim_link_child_tunnel_start);
                    Player_StartAnimMovement(play, this, 0x9D);
                }
                return true;
            }
        }
    }

    return false;
}

s32 func_8083F360(PlayState* play, Player* this, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    CollisionPoly* wallPoly;
    s32 wallBgId;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    f32 yawCos;
    f32 yawSin;
    s32 temp;
    f32 wallPolyNormalX;
    f32 wallPolyNormalZ;

    yawCos = Math_CosS(this->actor.shape.rot.y);
    yawSin = Math_SinS(this->actor.shape.rot.y);

    sp6C.x = this->actor.world.pos.x + (arg4 * yawSin);
    sp6C.z = this->actor.world.pos.z + (arg4 * yawCos);
    sp60.x = this->actor.world.pos.x + (arg3 * yawSin);
    sp60.z = this->actor.world.pos.z + (arg3 * yawCos);
    sp60.y = sp6C.y = this->actor.world.pos.y + arg1;

    if (BgCheck_EntityLineTest1(&play->colCtx, &sp6C, &sp60, &sp54, &this->actor.wallPoly, true, false, false,
                                true, &wallBgId)) {
        wallPoly = this->actor.wallPoly;

        this->actor.bgCheckFlags |= 0x200;
        this->actor.wallBgId = wallBgId;

        sTouchedWallFlags = func_80041DB8(&play->colCtx, wallPoly, wallBgId);

        wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
        wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
        temp = Math_Atan2S(-wallPolyNormalZ, -wallPolyNormalX);
        Math_ScaledStepToS(&this->actor.shape.rot.y, temp, 800);

        this->yaw = this->actor.shape.rot.y;
        this->actor.world.pos.x = sp54.x - (Math_SinS(this->actor.shape.rot.y) * arg2);
        this->actor.world.pos.z = sp54.z - (Math_CosS(this->actor.shape.rot.y) * arg2);

        return 1;
    }

    this->actor.bgCheckFlags &= ~0x200;

    return 0;
}

s32 func_8083F524(PlayState* play, Player* this) {
    return func_8083F360(play, this, 26.0f, this->ageProperties->wallCheckRadius + 5.0f, 30.0f, 0.0f);
}

/**
 * Two exit walls are placed at each end of the crawlspace, separate to the two entrance walls used to enter the
 * crawlspace. These front and back exit walls are further into the crawlspace than the front and
 * back entrance walls. When player interacts with either of these two interior exit walls, start the leaving-crawlspace
 * cutscene and return true. Else, return false
 */
s32 Player_TryLeavingCrawlspace(Player* this, PlayState* play) {
    s16 yawToWall;

    if ((this->linearVelocity != 0.0f) && (this->actor.bgCheckFlags & 8) && (sTouchedWallFlags & 0x30)) {

        // The exit wallYaws will always point inward on the crawlline
        // Interacting with the exit wall in front will have a yaw diff of 0x8000
        // Interacting with the exit wall behind will have a yaw diff of 0
        yawToWall = this->actor.shape.rot.y - this->actor.wallYaw;
        if (this->linearVelocity < 0.0f) {
            yawToWall += 0x8000;
        }

        if (ABS(yawToWall) > 0x4000) {
            Player_SetupAction(play, this, Player_Action_8084C81C, 0);

            if (this->linearVelocity > 0.0f) {
                // Leaving a crawlspace forwards
                this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
                // #region SOH [Enhancement]
                if (CVarGetInteger(CVAR_ENHANCEMENT("CrawlSpeed"), 1) > 1) {
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_child_tunnel_end,
                                         ((CVarGetInteger(CVAR_ENHANCEMENT("CrawlSpeed"), 1) + 1.0f) / 2.0f), 0.0f,
                                         Animation_GetLastFrame(&gPlayerAnim_link_child_tunnel_end), ANIMMODE_ONCE,
                                         0.0f);
                    Player_StartAnimMovement(play, this, 0x9D);
                    OnePointCutscene_Init(play, 9601, 999, NULL, MAIN_CAM);
                // #endregion
                } else {
                    Player_AnimPlayOnce(play, this, &gPlayerAnim_link_child_tunnel_end);
                    Player_StartAnimMovement(play, this, 0x9D);
                    OnePointCutscene_Init(play, 9601, 999, NULL, MAIN_CAM);
                }
            } else {
                // Leaving a crawlspace backwards
                this->actor.shape.rot.y = this->actor.wallYaw;
                // #region SOH [Enhancement]
                if (CVarGetInteger(CVAR_ENHANCEMENT("CrawlSpeed"), 1) > 1) {
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_child_tunnel_start,
                                         -1.0f * ((CVarGetInteger(CVAR_ENHANCEMENT("CrawlSpeed"), 1) + 1.0f) / 2.0f),
                                         Animation_GetLastFrame(&gPlayerAnim_link_child_tunnel_start), 0.0f, ANIMMODE_ONCE, 0.0f);
                    Player_StartAnimMovement(play, this, 0x9D);
                    OnePointCutscene_Init(play, 9602, 999, NULL, MAIN_CAM);
                // #endregion
                }
                else {
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_child_tunnel_start, -1.0f,
                                         Animation_GetLastFrame(&gPlayerAnim_link_child_tunnel_start), 0.0f, ANIMMODE_ONCE, 0.0f);
                    Player_StartAnimMovement(play, this, 0x9D);
                    OnePointCutscene_Init(play, 9602, 999, NULL, MAIN_CAM);
                }
            }

            this->yaw = this->actor.shape.rot.y;
            Player_ZeroSpeedXZ(this);

            return true;
        }
    }

    return false;
}

void func_8083F72C(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    if (!Player_SetupWaitForPutAway(play, this, func_8083A388)) {
        Player_SetupAction(play, this, Player_Action_8084B78C, 0);
    }

    Player_AnimPlayOnce(play, this, anim);
    func_80832224(this);

    this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
}

s32 Player_ActionHandler_5(Player* this, PlayState* play) {
    DynaPolyActor* wallPolyActor;

    if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->actor.bgCheckFlags & 0x200) && (sShapeYawToTouchedWall < 0x3000)) {

        if (((this->linearVelocity > 0.0f) && func_8083EC18(this, play, sTouchedWallFlags)) ||
            Player_TryEnteringCrawlspace(this, play, sTouchedWallFlags)) {
            return 1;
        }

        if (!func_808332B8(this) && ((this->linearVelocity == 0.0f) || !(this->stateFlags2 & PLAYER_STATE2_DO_ACTION_CLIMB)) &&
            (sTouchedWallFlags & 0x40) && (this->actor.bgCheckFlags & 1) && (this->yDistToLedge >= 39.0f)) {

            this->stateFlags2 |= PLAYER_STATE2_DO_ACTION_GRAB;

            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A)) {

                if ((this->actor.wallBgId != BGCHECK_SCENE) &&
                    ((wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId)) != NULL)) {

                    if (wallPolyActor->actor.id == ACTOR_BG_HEAVY_BLOCK) {
                        if (Player_GetStrength() < PLAYER_STR_GOLD_G) {
                            return 0;
                        }

                        Player_SetupWaitForPutAway(play, this, func_8083A0F4);
                        this->stateFlags1 |= PLAYER_STATE1_CARRYING_ACTOR;
                        this->interactRangeActor = &wallPolyActor->actor;
                        this->getItemId = GI_NONE;
                        this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
                        this->yaw = this->actor.wallYaw + 0x8000;
                        func_80832224(this);

                        return 1;
                    }

                    this->unk_3C4 = &wallPolyActor->actor;
                } else {
                    this->unk_3C4 = NULL;
                }

                func_8083F72C(this, &gPlayerAnim_link_normal_push_wait, play);

                return 1;
            }
        }
    }

    return 0;
}

s32 func_8083F9D0(PlayState* play, Player* this) {
    if ((this->actor.bgCheckFlags & 0x200) &&
        ((this->stateFlags2 & PLAYER_STATE2_MOVING_DYNAPOLY) || CHECK_BTN_ALL(sControlInput->cur.button, BTN_A))) {
        DynaPolyActor* wallPolyActor = NULL;

        if (this->actor.wallBgId != BGCHECK_SCENE) {
            wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);
        }

        if (&wallPolyActor->actor == this->unk_3C4) {
            if (this->stateFlags2 & PLAYER_STATE2_MOVING_DYNAPOLY) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    func_80839FFC(this, play);
    Player_AnimPlayOnce(play, this, &gPlayerAnim_link_normal_push_wait_end);
    this->stateFlags2 &= ~PLAYER_STATE2_MOVING_DYNAPOLY;
    return 1;
}

void func_8083FAB8(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_8084B898, 0);
    this->stateFlags2 |= PLAYER_STATE2_MOVING_DYNAPOLY;
    Player_AnimPlayOnce(play, this, &gPlayerAnim_link_normal_push_start);
}

void func_8083FB14(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_8084B9E4, 0);
    this->stateFlags2 |= PLAYER_STATE2_MOVING_DYNAPOLY;
    Player_AnimPlayOnce(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_pull_start, this->modelAnimType));
}

void func_8083FB7C(Player* this, PlayState* play) {
    this->stateFlags1 &= ~(PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_IN_WATER);
    func_80837B9C(this, play);
    this->linearVelocity = -0.4f;
}

s32 func_8083FBC0(Player* this, PlayState* play) {
    if (!CHECK_BTN_ALL(sControlInput->press.button, BTN_A) && (this->actor.bgCheckFlags & 0x200) &&
        ((sTouchedWallFlags & 8) || (sTouchedWallFlags & 2) ||
         func_80041E4C(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId))) {
        return false;
    }

    func_8083FB7C(this, play);
    Player_PlayVoiceSfx(this, NA_SE_VO_LI_AUTO_JUMP);
    return true;
}

s32 func_8083FC68(Player* this, f32 arg1, s16 arg2) {
    f32 sp1C = (s16)(arg2 - this->actor.shape.rot.y);
    f32 temp;

    if (this->focusActor != NULL) {
        func_8083DB98(this, func_8002DD78(this) || func_808334B4(this));
    }

    temp = fabsf(sp1C) / 32768.0f;

    if (arg1 > (((temp * temp) * 50.0f) + 6.0f)) {
        return 1;
    } else if (arg1 > (((1.0f - temp) * 10.0f) + 6.8f)) {
        return -1;
    }

    return 0;
}

s32 func_8083FD78(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    s16 sp2E = *arg2 - this->parallelYaw;
    u16 sp2C = ABS(sp2E);

    if ((func_8002DD78(this) || func_808334B4(this)) && (this->focusActor == NULL)) {
        *arg1 *= Math_SinS(sp2C);

        if (*arg1 != 0.0f) {
            *arg2 = (((sp2E >= 0) ? 1 : -1) << 0xE) + this->actor.shape.rot.y;
        } else {
            *arg2 = this->actor.shape.rot.y;
        }

        // #region SOH [Enhancement]
        if (CVarGetInteger(CVAR_SETTING("Controls.RightStickAim"), 0) || !CVarGetInteger(CVAR_SETTING("Controls.InvertZAimingYAxis"), 1)) {
            
            if (this->focusActor != NULL) {
                func_8083DB98(this, 1);
            } else {
                int8_t relStickY;

                // preserves simultaneous left/right-stick aiming
                if (CVarGetInteger(CVAR_SETTING("Controls.RightStickAim"), 0)) {
                    if ((sControlInput->rel.stick_y + sControlInput->rel.right_stick_y) >= 0) {
                        relStickY = (((sControlInput->rel.stick_y) > (sControlInput->rel.right_stick_y))
                                         ? (sControlInput->rel.stick_y)
                                         : (sControlInput->rel.right_stick_y));
                    } else {
                        relStickY = (((sControlInput->rel.stick_y) < (sControlInput->rel.right_stick_y))
                                         ? (sControlInput->rel.stick_y)
                                         : (sControlInput->rel.right_stick_y));
                    }
                } else {
                    relStickY = sControlInput->rel.stick_y;
                }

                Math_SmoothStepToS(&this->actor.focus.rot.x,
                                   relStickY * (CVarGetInteger(CVAR_SETTING("Controls.InvertZAimingYAxis"), 1) ? 1 : -1) * 240.0f, 14, 4000, 30);
                func_80836AB8(this, 1);
            }
        // #endregion
        } else {
            if (this->focusActor != NULL) {
                func_8083DB98(this, 1);
            } else {
                Math_SmoothStepToS(&this->actor.focus.rot.x, sControlInput->rel.stick_y * 240.0f, 14, 4000, 30);
                func_80836AB8(this, 1);
            }
        }
    } else {
        if (this->focusActor != NULL) {
            return func_8083FC68(this, *arg1, *arg2);
        } else {
            func_8083DC54(this, play);
            if ((*arg1 != 0.0f) && (sp2C < 6000)) {
                return 1;
            } else if (*arg1 > Math_SinS((0x4000 - (sp2C >> 1))) * 200.0f) {
                return -1;
            }
        }
    }

    return 0;
}

s32 func_8083FFB8(Player* this, f32* arg1, s16* arg2) {
    s16 temp1 = *arg2 - this->actor.shape.rot.y;
    u16 temp2 = ABS(temp1);
    f32 temp3 = Math_CosS(temp2);

    *arg1 *= temp3;

    if (*arg1 != 0.0f) {
        if (temp3 > 0) {
            return 1;
        } else {
            return -1;
        }
    }

    return 0;
}

s32 func_80840058(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    func_8083DC54(this, play);

    if ((*arg1 != 0.0f) || (ABS(this->unk_87C) > 400)) {
        s16 temp1 = *arg2 - (u16)Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
        u16 temp2 = (ABS(temp1) - 0x2000);

        if ((temp2 < 0x4000) || (this->unk_87C != 0)) {
            return -1;
        }

        return 1;
    }

    return 0;
}

void func_80840138(Player* this, f32 arg1, s16 arg2) {
    s16 temp = arg2 - this->actor.shape.rot.y;

    if (arg1 > 0.0f) {
        if (temp < 0) {
            this->unk_874 = 0.0f;
        } else {
            this->unk_874 = 1.0f;
        }
    }

    Math_StepToF(&this->unk_870, this->unk_874, 0.3f);
}

void func_808401B0(PlayState* play, Player* this) {
    LinkAnimation_BlendToJoint(play, &this->skelAnime, func_808334E4(this), this->unk_868, func_80833528(this),
                               this->unk_868, this->unk_870, this->blendTable);
}

s32 func_8084021C(f32 arg0, f32 arg1, f32 arg2, f32 arg3) {
    f32 temp;

    if ((arg3 == 0.0f) && (arg1 > 0.0f)) {
        arg3 = arg2;
    }

    temp = (arg0 + arg1) - arg3;

    if (((temp * arg1) >= 0.0f) && (((temp - arg1) * arg1) < 0.0f)) {
        return 1;
    }

    return 0;
}

void func_8084029C(Player* this, f32 arg1) {
    f32 updateScale = R_UPDATE_RATE * 0.5f;

    arg1 *= updateScale;
    if (arg1 < -7.25) {
        arg1 = -7.25;
    } else if (arg1 > 7.25f) {
        arg1 = 7.25f;
    }

    if ((this->currentBoots == PLAYER_BOOTS_HOVER ||
         (CVarGetInteger(CVAR_ENHANCEMENT("IvanCoopModeEnabled"), 0) && this->ivanFloating)) &&
        !(this->actor.bgCheckFlags & 1) &&
        (this->hoverBootsTimer != 0 || (CVarGetInteger(CVAR_ENHANCEMENT("IvanCoopModeEnabled"), 0) && this->ivanFloating))) {
        func_8002F8F0(&this->actor, NA_SE_PL_HOBBERBOOTS_LV - SFX_FLAG);
    } else if (func_8084021C(this->unk_868, arg1, 29.0f, 10.0f) || func_8084021C(this->unk_868, arg1, 29.0f, 24.0f)) {
        Player_PlaySteppingSfx(this, this->linearVelocity);
        if (this->linearVelocity > 4.0f) {
            this->stateFlags2 |= PLAYER_STATE2_FOOTSTEP;
        }
    }

    this->unk_868 += arg1;

    if (this->unk_868 < 0.0f) {
        this->unk_868 += 29.0f;
    } else if (this->unk_868 >= 29.0f) {
        this->unk_868 -= 29.0f;
    }
}

void Player_Action_80840450(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp1;
    u32 temp2;
    s16 temp3;
    s32 temp4;

    if (this->stateFlags3 & PLAYER_STATE3_FINISHED_ATTACKING) {
        if (Player_GetMeleeWeaponHeld(this)) {
            this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;
        } else {
            this->stateFlags3 &= ~PLAYER_STATE3_FINISHED_ATTACKING;
        }
    }

    if (this->av2.actionVar2 != 0) {
        if (LinkAnimation_Update(play, &this->skelAnime)) {
            Player_FinishAnimMovement(this);
            Player_AnimPlayLoop(play, this, func_808334E4(this));
            this->av2.actionVar2 = 0;
            this->stateFlags3 &= ~PLAYER_STATE3_FINISHED_ATTACKING;
        }
        func_80833C3C(this);
    } else {
        func_808401B0(play, this);
    }

    Player_DecelerateToZero(this);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList1, true)) {
        if (!Player_UpdateHostileLockOn(this) && (!Player_FriendlyLockOnOrParallel(this) || (func_80834B5C != this->upperActionFunc))) {
            func_8083CF10(this, play);
            return;
        }

        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        temp1 = func_8083FC68(this, speedTarget, yawTarget);

        if (temp1 > 0) {
            func_8083C8DC(this, play, yawTarget);
            return;
        }

        if (temp1 < 0) {
            func_8083CBF0(this, yawTarget, play);
            return;
        }

        if (speedTarget > 4.0f) {
            func_8083CC9C(this, play);
            return;
        }

        func_8084029C(this, (this->linearVelocity * 0.3f) + 1.0f);
        func_80840138(this, speedTarget, yawTarget);

        temp2 = this->unk_868;
        if ((temp2 < 6) || ((temp2 - 0xE) < 6)) {
            Math_StepToF(&this->linearVelocity, 0.0f, 1.5f);
            return;
        }

        temp3 = yawTarget - this->yaw;
        temp4 = ABS(temp3);

        if (temp4 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.5f)) {
                this->yaw = yawTarget;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, speedTarget * 0.3f, 2.0f, 1.5f);

        if (!(this->stateFlags3 & PLAYER_STATE3_FINISHED_ATTACKING)) {
            Math_ScaledStepToS(&this->yaw, yawTarget, temp4 * 0.1f);
        }
    }
}

void Player_Action_808407CC(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp1;
    s16 temp2;
    s32 temp3;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_FinishAnimMovement(this);
        Player_AnimPlayOnce(play, this, Player_GetIdleAnim(this));
    }

    Player_DecelerateToZero(this);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList2, true)) {
        if (Player_UpdateHostileLockOn(this)) {
            func_8083CEAC(this, play);
            return;
        }

        if (!Player_FriendlyLockOnOrParallel(this)) {
            Player_SetupActionPreserveAnimMovement(play, this, Player_Action_Idle, 1);
            this->yaw = this->actor.shape.rot.y;
            return;
        }

        if (func_80834B5C == this->upperActionFunc) {
            func_8083CEAC(this, play);
            return;
        }

        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        temp1 = func_8083FD78(this, &speedTarget, &yawTarget, play);

        if (temp1 > 0) {
            func_8083C8DC(this, play, yawTarget);
            return;
        }

        if (temp1 < 0) {
            func_8083CB2C(this, yawTarget, play);
            return;
        }

        if (speedTarget > 4.9f) {
            func_8083CC9C(this, play);
            func_80833C3C(this);
            return;
        }
        if (speedTarget != 0.0f) {
            func_8083CB94(this, play);
            return;
        }

        temp2 = yawTarget - this->actor.shape.rot.y;
        temp3 = ABS(temp2);

        if (temp3 > 800) {
            Player_SetupTurnInPlace(play, this, yawTarget);
        }
    }
}

void Player_ChooseNextIdleAnim(PlayState* play, Player* this) {
    LinkAnimationHeader* anim;
    LinkAnimationHeader** fidgetAnimPtr;
    s32 heathIsCritical;
    s32 fidgetType;
    s32 commonType;

    if ((this->focusActor != NULL) ||
        (!(heathIsCritical = HealthMeter_IsCritical()) && ((this->idleType = (this->idleType + 1) & 1) != 0))) {
        this->stateFlags2 &= ~PLAYER_STATE2_IDLE_FIDGET;
        anim = Player_GetIdleAnim(this);
    } else {
        this->stateFlags2 |= PLAYER_STATE2_IDLE_FIDGET;

        if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
            // Default idle animation will play if carrying an actor.
            // Note that in this case, `PLAYER_STATE2_IDLE_FIDGET` is still set even though the
            // animation that plays isn't a fidget animation.
            anim = Player_GetIdleAnim(this);
        } else {
            // Pick fidget type based on room behavior.
            // This may be changed below.
            fidgetType = play->roomCtx.curRoom.behaviorType2;

            if (heathIsCritical) {
                if (this->idleType >= PLAYER_IDLE_DEFAULT) {
                    fidgetType = FIDGET_CRIT_HEALTH_START;

                    // When health is critical, `idleType` will not be updated.
                    // It will stay as `PLAYER_IDLE_CRIT_HEALTH` until health is no longer critical.
                    this->idleType = PLAYER_IDLE_CRIT_HEALTH;
                } else {
                    // Keep looping the critical health animation until critical health ends
                    fidgetType = FIDGET_CRIT_HEALTH_LOOP;
                }
            } else {
                commonType = Rand_ZeroOne() * 5;

                // There is a 4/5 chance that a common fidget type will be considered.
                // However it may get rejected by the conditions below.
                // The type determined by `curRoom.behaviorType2` will be used if a common type is rejected.
                if (commonType < 4) {
                    // `FIDGET_ADJUST_TUNIC` and `FIDGET_TAP_FEET` are accepted unconditionally.
                    // The sword and shield related common types have extra restrictions.
                    //
                    // Note that `FIDGET_SWORD_SWING` is the first common fidget type, which is why
                    // all operations are done relative to this type.
                    if (((commonType + FIDGET_SWORD_SWING != FIDGET_SWORD_SWING) &&
                         (commonType + FIDGET_SWORD_SWING != FIDGET_ADJUST_SHIELD)) ||
                        ((this->rightHandType == PLAYER_MODELTYPE_RH_SHIELD) &&
                         ((commonType + FIDGET_SWORD_SWING == FIDGET_ADJUST_SHIELD) ||
                          (Player_GetMeleeWeaponHeld2(this) != 0)))) {
                        //! @bug It is possible for `FIDGET_ADJUST_SHIELD` to be used even if
                        //! a shield is not currently equipped. This is because of how being shieldless
                        //! is implemented. There is no sword-only model type, only
                        //! `PLAYER_MODELGROUP_SWORD_AND_SHIELD` exists. Therefore, the right hand type will be
                        //! `PLAYER_MODELTYPE_RH_SHIELD` if sword is in hand, even if no shield is equipped.
                        if ((commonType + FIDGET_SWORD_SWING == FIDGET_SWORD_SWING) &&
                            Player_HoldsTwoHandedWeapon(this) &&
                            CVarGetInteger(CVAR_ENHANCEMENT("TwoHandedIdle"), 0) == 1) {
                            //! @bug This code is unreachable.
                            //! The check above groups the `Player_GetMeleeWeaponHeld2` check and
                            //! `PLAYER_MODELTYPE_RH_SHIELD` conditions together, meaning sword and shield must be
                            //! in hand. However shield is not in hand when using a two handed melee weapon.
                            commonType = FIDGET_SWORD_SWING_TWO_HAND - FIDGET_SWORD_SWING;
                        }

                        fidgetType = FIDGET_SWORD_SWING + commonType;
                    }
                }
            }

            fidgetAnimPtr = &sFidgetAnimations[fidgetType][0];

            if (this->modelAnimType != PLAYER_ANIMTYPE_1) {
                fidgetAnimPtr = &sFidgetAnimations[fidgetType][1];
            }

            anim = *fidgetAnimPtr;
        }
    }

    LinkAnimation_Change(play, &this->skelAnime, anim, (2.0f / 3.0f) * sWaterSpeedFactor, 0.0f,
                         Animation_GetLastFrame(anim), ANIMMODE_ONCE, -6.0f);
}

void Player_Action_Idle(Player* this, PlayState* play) {
    s32 idleAnimResult = Player_CheckForIdleAnim(this);
    s32 animDone = LinkAnimation_Update(play, &this->skelAnime);
    f32 speedTarget;
    s16 yawTarget;
    s16 yawDiff;

    if (idleAnimResult > IDLE_ANIM_NONE) {
        Player_ProcessFidgetAnimSfxList(this, idleAnimResult - 1);
    }

    if (animDone) {
        if (this->av2.fallDamageStunTimer != 0) {
            if (DECR(this->av2.fallDamageStunTimer) == 0) {
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }

            // Offset model y position.
            // Depending on if the timer is even or odd, the offset will be 40 or -40 model space units.
            this->skelAnime.jointTable[0].y =
                (this->skelAnime.jointTable[0].y + ((this->av2.fallDamageStunTimer & 1) * 0x50)) - 0x28;
        } else {
            Player_FinishAnimMovement(this);
            Player_ChooseNextIdleAnim(play, this);
        }
    }

    Player_DecelerateToZero(this);

    if (this->av2.fallDamageStunTimer == 0) {
        if (!Player_TryActionHandlerList(play, this, sActionHandlerListIdle, true)) {
            if (Player_UpdateHostileLockOn(this)) {
                func_8083CEAC(this, play);
                return;
            }

            if (Player_FriendlyLockOnOrParallel(this)) {
                func_80839F30(this, play);
                return;
            }

            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

            if (speedTarget != 0.0f) {
                func_8083C8DC(this, play, yawTarget);
                return;
            }

            yawDiff = yawTarget - this->actor.shape.rot.y;

            if (ABS(yawDiff) > 800) {
                Player_SetupTurnInPlace(play, this, yawTarget);
                return;
            }

            Math_ScaledStepToS(&this->actor.shape.rot.y, yawTarget, 1200);
            this->yaw = this->actor.shape.rot.y;

            if (Player_GetIdleAnim(this) == this->skelAnime.animation) {
                func_8083DC54(this, play);
            }
        }
    }
}

void Player_Action_80840DE4(Player* this, PlayState* play) {
    f32 frames;
    f32 coeff;
    f32 speedTarget;
    s16 yawTarget;
    s32 temp1;
    s16 temp2;
    s32 temp3;
    s32 direction;

    this->skelAnime.mode = 0;
    LinkAnimation_SetUpdateFunction(&this->skelAnime);

    this->skelAnime.animation = func_8083356C(this);

    if (this->skelAnime.animation == &gPlayerAnim_link_bow_side_walk) {
        frames = 24.0f;
        coeff = -(MREG(95) / 100.0f);
    } else {
        frames = 29.0f;
        coeff = MREG(95) / 100.0f;
    }

    this->skelAnime.animLength = frames;
    this->skelAnime.endFrame = frames - 1.0f;

    if ((s16)(this->yaw - this->actor.shape.rot.y) >= 0) {
        direction = 1;
    } else {
        direction = -1;
    }

    this->skelAnime.playSpeed = direction * (this->linearVelocity * coeff);

    LinkAnimation_Update(play, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 0.0f) || LinkAnimation_OnFrame(&this->skelAnime, frames * 0.5f)) {
        Player_PlaySteppingSfx(this, this->linearVelocity);
    }

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList3, true)) {
        if (Player_UpdateHostileLockOn(this)) {
            func_8083CEAC(this, play);
            return;
        }

        if (!Player_FriendlyLockOnOrParallel(this)) {
            func_80853080(this, play);
            return;
        }

        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        temp1 = func_8083FD78(this, &speedTarget, &yawTarget, play);

        if (temp1 > 0) {
            func_8083C8DC(this, play, yawTarget);
            return;
        }

        if (temp1 < 0) {
            func_8083CB2C(this, yawTarget, play);
            return;
        }

        if (speedTarget > 4.9f) {
            func_8083CC9C(this, play);
            func_80833C3C(this);
            return;
        }

        if ((speedTarget == 0.0f) && (this->linearVelocity == 0.0f)) {
            func_80839F30(this, play);
            return;
        }

        temp2 = yawTarget - this->yaw;
        temp3 = ABS(temp2);

        if (temp3 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.5f)) {
                this->yaw = yawTarget;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, speedTarget * 0.4f, 1.5f, 1.5f);
        Math_ScaledStepToS(&this->yaw, yawTarget, temp3 * 0.1f);
    }
}

void func_80841138(Player* this, PlayState* play) {
    f32 temp1;
    f32 temp2;

    if (this->unk_864 < 1.0f) {
        s32 pad;

        temp1 = R_UPDATE_RATE * 0.5f;
        func_8084029C(this, REG(35) / 1000.0f);
        LinkAnimation_LoadToJoint(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_back_walk, this->modelAnimType),
                                  this->unk_868);
        this->unk_864 += 1 * temp1;
        if (this->unk_864 >= 1.0f) {
            this->unk_864 = 1.0f;
        }
        temp1 = this->unk_864;
    } else {
        temp2 = this->linearVelocity - (REG(48) / 100.0f);
        if (temp2 < 0.0f) {
            temp1 = 1.0f;
            func_8084029C(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));
            LinkAnimation_LoadToJoint(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_back_walk, this->modelAnimType),
                                      this->unk_868);
        } else {
            temp1 = (REG(37) / 1000.0f) * temp2;
            if (temp1 < 1.0f) {
                func_8084029C(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));
            } else {
                temp1 = 1.0f;
                func_8084029C(this, 1.2f + ((REG(38) / 1000.0f) * temp2));
            }
            LinkAnimation_LoadToMorph(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_back_walk, this->modelAnimType),
                                      this->unk_868);
            LinkAnimation_LoadToJoint(play, &this->skelAnime, &gPlayerAnim_link_normal_back_run,
                                      this->unk_868 * (16.0f / 29.0f));
        }
    }

    if (temp1 < 1.0f) {
        LinkAnimation_InterpJointMorph(play, &this->skelAnime, 1.0f - temp1);
    }
}

void func_8084140C(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_8084170C, 1);
    Player_AnimChangeOnceMorph(play, this, &gPlayerAnim_link_normal_back_brake);
}

s32 func_80841458(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    if (this->linearVelocity > 6.0f) {
        func_8084140C(this, play);
        return 1;
    }

    if (*arg1 != 0.0f) {
        if (Player_DecelerateToZero(this)) {
            *arg1 = 0.0f;
            *arg2 = this->yaw;
        } else {
            return 1;
        }
    }

    return 0;
}

void Player_Action_808414F8(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 sp2C;

    func_80841138(this, play);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList4, true)) {
        if (!Player_IsZTargetingWithHostileUpdate(this)) {
            func_8083C8DC(this, play, this->yaw);
            return;
        }

        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        sp2C = func_8083FD78(this, &speedTarget, &yawTarget, play);

        if (sp2C >= 0) {
            if (!func_80841458(this, &speedTarget, &yawTarget, play)) {
                if (sp2C != 0) {
                    func_8083C858(this, play);
                } else if (speedTarget > 4.9f) {
                    func_8083CC9C(this, play);
                } else {
                    func_8083CB94(this, play);
                }
            }
        } else {
            s16 sp2A = yawTarget - this->yaw;

            Math_AsymStepToF(&this->linearVelocity, speedTarget * 1.5f, 1.5f, 2.0f);
            Math_ScaledStepToS(&this->yaw, yawTarget, sp2A * 0.1f);

            if ((speedTarget == 0.0f) && (this->linearVelocity == 0.0f)) {
                func_80839F30(this, play);
            }
        }
    }
}

void func_808416C0(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_808417FC, 1);
    Player_AnimPlayOnce(play, this, &gPlayerAnim_link_normal_back_brake_end);
}

void Player_Action_8084170C(Player* this, PlayState* play) {
    s32 sp34;
    f32 speedTarget;
    s16 yawTarget;

    sp34 = LinkAnimation_Update(play, &this->skelAnime);
    Player_DecelerateToZero(this);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList4, true)) {
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (this->linearVelocity == 0.0f) {
            this->yaw = this->actor.shape.rot.y;

            if (func_8083FD78(this, &speedTarget, &yawTarget, play) > 0) {
                func_8083C858(this, play);
            } else if ((speedTarget != 0.0f) || (sp34 != 0)) {
                func_808416C0(this, play);
            }
        }
    }
}

void Player_Action_808417FC(Player* this, PlayState* play) {
    s32 sp1C;

    sp1C = LinkAnimation_Update(play, &this->skelAnime);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList4, true)) {
        if (sp1C != 0) {
            func_80839F30(this, play);
        }
    }
}

void func_80841860(PlayState* play, Player* this) {
    f32 frame;
    // fake match? see Player_InitItemActionWithAnim
    LinkAnimationHeader* sp38 = D_80853914[0][this->modelAnimType + PLAYER_ANIMGROUP_side_walkL * ARRAY_COUNT(D_80853914[0])];
    LinkAnimationHeader* sp34 = D_80853914[0][this->modelAnimType + PLAYER_ANIMGROUP_side_walkR * ARRAY_COUNT(D_80853914[0])];

    this->skelAnime.animation = sp38;

    func_8084029C(this, (REG(30) / 1000.0f) + ((REG(32) / 1000.0f) * this->linearVelocity));

    frame = this->unk_868 * (16.0f / 29.0f);
    LinkAnimation_BlendToJoint(play, &this->skelAnime, sp34, frame, sp38, frame, this->unk_870, this->blendTable);
}

void Player_Action_8084193C(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp1;
    s16 temp2;
    s32 temp3;

    func_80841860(play, this);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList5, true)) {
        if (!Player_IsZTargetingWithHostileUpdate(this)) {
            func_8083C858(this, play);
            return;
        }

        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (Player_FriendlyLockOnOrParallel(this)) {
            temp1 = func_8083FD78(this, &speedTarget, &yawTarget, play);
        } else {
            temp1 = func_8083FC68(this, speedTarget, yawTarget);
        }

        if (temp1 > 0) {
            func_8083C858(this, play);
            return;
        }

        if (temp1 < 0) {
            if (Player_FriendlyLockOnOrParallel(this)) {
                func_8083CB2C(this, yawTarget, play);
            } else {
                func_8083CBF0(this, yawTarget, play);
            }
            return;
        }

        if ((this->linearVelocity < 3.6f) && (speedTarget < 4.0f)) {
            if (!Player_CheckHostileLockOn(this) && Player_FriendlyLockOnOrParallel(this)) {
                func_8083CB94(this, play);
            } else {
                func_80839F90(this, play);
            }
            return;
        }

        func_80840138(this, speedTarget, yawTarget);

        temp2 = yawTarget - this->yaw;
        temp3 = ABS(temp2);

        if (temp3 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 3.0f) != 0) {
                this->yaw = yawTarget;
            }
            return;
        }

        speedTarget *= 0.9f;
        Math_AsymStepToF(&this->linearVelocity, speedTarget, 2.0f, 3.0f);
        Math_ScaledStepToS(&this->yaw, yawTarget, temp3 * 0.1f);
    }
}

/**
 * Turn in place until the angle pointed to by the control stick is reached.
 *
 * This is the state that the speedrunning community refers to as "ESS" or "ESS Position".
 * See the bug comment below and https://www.zeldaspeedruns.com/oot/tech/extended-superslide
 * for more information.
 */
void Player_Action_TurnInPlace(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    LinkAnimation_Update(play, &this->skelAnime);

    if (Player_HoldsTwoHandedWeapon(this)) {
        AnimationContext_SetLoadFrame(play, Player_GetIdleAnim(this), 0, this->skelAnime.limbCount,
                                      this->skelAnime.morphTable);
        AnimationContext_SetCopyTrue(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                     this->skelAnime.morphTable, sUpperBodyLimbCopyMap);
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

    //! @bug This action does not handle xzSpeed in any capacity.
    //! Player's current speed value will be maintained the entire time this action is running.
    //! This is the core bug that allows many different glitches to manifest.
    //!
    //! One possible fix is to kill all speed instantly in `Player_SetupTurnInPlace`.
    //! Another possible fix is to gradually kill speed by calling `Player_DecelerateToZero`
    //! here, which plenty of other "standing" actions do.

    if (!Player_TryActionHandlerList(play, this, sActionHandlerListTurnInPlace, true)) {
        if (speedTarget != 0.0f) {
            this->actor.shape.rot.y = yawTarget;
            func_8083C858(this, play);
        } else if (Math_ScaledStepToS(&this->actor.shape.rot.y, yawTarget, this->turnRate)) {
            func_8083C0E8(this, play);
        }

        this->yaw = this->actor.shape.rot.y;
    }
}

void func_80841CC4(Player* this, s32 arg1, PlayState* play) {
    LinkAnimationHeader* anim;
    s16 target;
    f32 rate;

    if (ABS(sFloorShapePitch) < 3640) {
        target = 0;
    } else {
        target = CLAMP(sFloorShapePitch, -10922, 10922);
    }

    Math_ScaledStepToS(&this->unk_89C, target, 400);

    if ((this->modelAnimType == PLAYER_ANIMTYPE_3) || ((this->unk_89C == 0) && (this->unk_6C4 <= 0.0f))) {
        if (arg1 == 0) {
            LinkAnimation_LoadToJoint(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                      this->unk_868);
        } else {
            LinkAnimation_LoadToMorph(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                      this->unk_868);
        }
        return;
    }

    if (this->unk_89C != 0) {
        rate = this->unk_89C / 10922.0f;
    } else {
        rate = this->unk_6C4 * 0.0006f;
    }

    rate *= fabsf(this->linearVelocity) * 0.5f;

    if (rate > 1.0f) {
        rate = 1.0f;
    }

    if (rate < 0.0f) {
        anim = &gPlayerAnim_link_normal_climb_down;
        rate = -rate;
    } else {
        anim = &gPlayerAnim_link_normal_climb_up;
    }

    if (arg1 == 0) {
        LinkAnimation_BlendToJoint(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                   this->unk_868, anim, this->unk_868, rate, this->blendTable);
    } else {
        LinkAnimation_BlendToMorph(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                   this->unk_868, anim, this->unk_868, rate, this->blendTable);
    }
}

void func_80841EE4(Player* this, PlayState* play) {
    f32 temp1;
    f32 temp2;

    if (this->unk_864 < 1.0f) {
        temp1 = R_UPDATE_RATE * 0.5f;

        func_8084029C(this, REG(35) / 1000.0f);
        LinkAnimation_LoadToJoint(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                  this->unk_868);

        this->unk_864 += 1 * temp1;
        if (this->unk_864 >= 1.0f) {
            this->unk_864 = 1.0f;
        }

        temp1 = this->unk_864;
    } else {
        temp2 = this->linearVelocity - (REG(48) / 100.0f);

        if (temp2 < 0.0f) {
            temp1 = 1.0f;
            func_8084029C(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));

            func_80841CC4(this, 0, play);
        } else {
            temp1 = (REG(37) / 1000.0f) * temp2;
            if (temp1 < 1.0f) {
                func_8084029C(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));
            } else {
                temp1 = 1.0f;
                func_8084029C(this, 1.2f + ((REG(38) / 1000.0f) * temp2));
            }

            func_80841CC4(this, 1, play);

            LinkAnimation_LoadToJoint(play, &this->skelAnime, func_80833438(this),
                                      this->unk_868 * (20.0f / 29.0f));
        }
    }

    if (temp1 < 1.0f) {
        LinkAnimation_InterpJointMorph(play, &this->skelAnime, 1.0f - temp1);
    }
}

void Player_Action_80842180(Player* this, PlayState* play) {
    f32 sp2C;
    s16 sp2A;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    func_80841EE4(this, play);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList8, true)) {
        if (Player_IsZTargetingWithHostileUpdate(this)) {
            func_8083C858(this, play);
            return;
        }

        Player_GetMovementSpeedAndYaw(this, &sp2C, &sp2A, SPEED_MODE_CURVED, play);

        if (!func_8083C484(this, &sp2C, &sp2A)) {
            int32_t giSpeedModifier = GameInteractor_RunSpeedModifier();
            if (giSpeedModifier != 0) {
                if (giSpeedModifier > 0) {
                    sp2C *= giSpeedModifier;
                } else {
                    sp2C /= abs(giSpeedModifier);
                }
            }

            if (CVarGetInteger(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_VANILLA) != BUNNY_HOOD_VANILLA && this->currentMask == PLAYER_MASK_BUNNY) {
                sp2C *= 1.5f;
            }
            
            if (CVarGetInteger(CVAR_SETTING("WalkModifier.Enabled"), 0)) {
                if (CVarGetInteger(CVAR_SETTING("WalkModifier.SpeedToggle"), 0)) {
                    if (gWalkSpeedToggle1) {
                        sp2C *= CVarGetFloat(CVAR_SETTING("WalkModifier.Mapping1"), 1.0f);
                    } else if (gWalkSpeedToggle2) {
                        sp2C *= CVarGetFloat(CVAR_SETTING("WalkModifier.Mapping2"), 1.0f);
                    }
                } else {
                    if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_CUSTOM_MODIFIER1)) {
                        sp2C *= CVarGetFloat(CVAR_SETTING("WalkModifier.Mapping1"), 1.0f);
                    } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_CUSTOM_MODIFIER2)) {
                        sp2C *= CVarGetFloat(CVAR_SETTING("WalkModifier.Mapping2"), 1.0f);
                    }
                }
            }

            func_8083DF68(this, sp2C, sp2A);
            func_8083DDC8(this, play);

            if ((this->linearVelocity == 0.0f) && (sp2C == 0.0f)) {
                func_8083C0B8(this, play);
            }
        }
    }
}

void Player_Action_8084227C(Player* this, PlayState* play) {
    f32 sp2C;
    s16 sp2A;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    func_80841EE4(this, play);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList9, true)) {
        if (!Player_IsZTargetingWithHostileUpdate(this)) {
            func_8083C858(this, play);
            return;
        }

        Player_GetMovementSpeedAndYaw(this, &sp2C, &sp2A, SPEED_MODE_LINEAR, play);

        if (!func_8083C484(this, &sp2C, &sp2A)) {
            if ((Player_FriendlyLockOnOrParallel(this) && (sp2C != 0.0f) && (func_8083FD78(this, &sp2C, &sp2A, play) <= 0)) ||
                (!Player_FriendlyLockOnOrParallel(this) && (func_8083FC68(this, sp2C, sp2A) <= 0))) {
                func_80839F90(this, play);
                return;
            }

            func_8083DF68(this, sp2C, sp2A);
            func_8083DDC8(this, play);

            if ((this->linearVelocity == 0) && (sp2C == 0)) {
                func_80839F90(this, play);
            }
        }
    }
}

void Player_Action_808423EC(Player* this, PlayState* play) {
    s32 sp34;
    f32 sp30;
    s16 sp2E;

    sp34 = LinkAnimation_Update(play, &this->skelAnime);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList5, true)) {
        if (!Player_IsZTargetingWithHostileUpdate(this)) {
            func_8083C858(this, play);
            return;
        }

        Player_GetMovementSpeedAndYaw(this, &sp30, &sp2E, SPEED_MODE_LINEAR, play);

        if ((this->skelAnime.morphWeight == 0.0f) && (this->skelAnime.curFrame > 5.0f)) {
            Player_DecelerateToZero(this);

            if ((this->skelAnime.curFrame > 10.0f) && (func_8083FC68(this, sp30, sp2E) < 0)) {
                func_8083CBF0(this, sp2E, play);
                return;
            }

            if (sp34 != 0) {
                func_8083CD00(this, play);
            }
        }
    }
}

void Player_Action_8084251C(Player* this, PlayState* play) {
    s32 sp34;
    f32 speedTarget;
    s16 yawTarget;

    sp34 = LinkAnimation_Update(play, &this->skelAnime);

    Player_DecelerateToZero(this);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList10, true)) {
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (this->linearVelocity == 0.0f) {
            this->yaw = this->actor.shape.rot.y;

            if (func_8083FC68(this, speedTarget, yawTarget) > 0) {
                func_8083C858(this, play);
                return;
            }

            if ((speedTarget != 0.0f) || (sp34 != 0)) {
                func_80839F90(this, play);
            }
        }
    }
}

void func_8084260C(Vec3f* src, Vec3f* dest, f32 arg2, f32 arg3, f32 arg4) {
    dest->x = (Rand_ZeroOne() * arg3) + src->x;
    dest->y = (Rand_ZeroOne() * arg4) + (src->y + arg2);
    dest->z = (Rand_ZeroOne() * arg3) + src->z;
}

static Vec3f D_808545B4 = { 0.0f, 0.0f, 0.0f };
static Vec3f D_808545C0 = { 0.0f, 0.0f, 0.0f };

s32 func_8084269C(PlayState* play, Player* this) {
    Vec3f sp2C;

    if ((this->floorSfxOffset == 0) || (this->floorSfxOffset == 1)) {
        func_8084260C(&this->actor.shape.feetPos[FOOT_LEFT], &sp2C,
                      this->actor.floorHeight - this->actor.shape.feetPos[FOOT_LEFT].y, 7.0f, 5.0f);
        func_800286CC(play, &sp2C, &D_808545B4, &D_808545C0, 50, 30);
        func_8084260C(&this->actor.shape.feetPos[FOOT_RIGHT], &sp2C,
                      this->actor.floorHeight - this->actor.shape.feetPos[FOOT_RIGHT].y, 7.0f, 5.0f);
        func_800286CC(play, &this->actor.shape.feetPos[FOOT_RIGHT], &D_808545B4, &D_808545C0, 50, 30);
        return 1;
    }

    return 0;
}

void Player_Action_8084279C(Player* this, PlayState* play) {
    func_80832CB0(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_check_wait, this->modelAnimType));

    if (DECR(this->av2.actionVar2) == 0) {
        if (!Player_ActionHandler_13(this, play)) {
            func_8083A098(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_check_end, this->modelAnimType), play);
        }

        this->actor.flags &= ~ACTOR_FLAG_PLAYER_TALKED_TO;
        func_8005B1A4(Play_GetCamera(play, 0));
    }
}

s32 func_8084285C(Player* this, f32 arg1, f32 arg2, f32 arg3) {
    if ((arg1 <= this->skelAnime.curFrame) && (this->skelAnime.curFrame <= arg3)) {
        func_80833A20(this, (arg2 <= this->skelAnime.curFrame) ? 1 : -1);
        return 1;
    }

    func_80832318(this);
    return 0;
}

s32 func_808428D8(Player* this, PlayState* play) {
    if (Player_IsChildWithHylianShield(this) || !Player_GetMeleeWeaponHeld(this) || !sUseHeldItem) {
        return 0;
    }

    Player_AnimPlayOnce(play, this, &gPlayerAnim_link_normal_defense_kiru);
    this->av1.actionVar1 = 1;
    this->meleeWeaponAnimation = PLAYER_MWA_STAB_1H;
    this->yaw = this->actor.shape.rot.y + this->upperLimbRot.y;

    if (!CVarGetInteger(CVAR_ENHANCEMENT("CrouchStabHammerFix"), 0)) {
        return 1;
    }

    u32 swordId;
    if (Player_HoldsBrokenKnife(this)) {
        swordId = 1;
    } else {
        swordId = Player_GetMeleeWeaponHeld(this) - 1;
    }

    if (swordId != 4 && !CVarGetInteger(CVAR_ENHANCEMENT("CrouchStabFix"), 0)) { // 4 = Megaton Hammer
        return 1;
    }

    u32 flags = D_80854488[swordId][0];
    func_80837918(this, 0, flags);
    func_80837918(this, 1, flags);

    return 1;
}

s32 func_80842964(Player* this, PlayState* play) {
    return Player_ActionHandler_13(this, play) || Player_ActionHandler_Talk(this, play) || Player_ActionHandler_2(this, play);
}

void Player_RequestQuake(PlayState* play, s32 speed, s32 y, s32 countdown) {
    s32 quakeIndex = Quake_Add(Play_GetCamera(play, 0), 3);

    Quake_SetSpeed(quakeIndex, speed);
    Quake_SetQuakeValues(quakeIndex, y, 0, 0, 0);
    Quake_SetCountdown(quakeIndex, countdown);
}

void func_80842A28(PlayState* play, Player* this) {
    Player_RequestQuake(play, 27767, 7, 20);
    play->actorCtx.unk_02 = 4;
    Player_RequestRumble(this, 255, 20, 150, 0);
    Player_PlaySfx(this, NA_SE_IT_HAMMER_HIT);
}

void func_80842A88(PlayState* play, Player* this) {
    Inventory_ChangeAmmo(ITEM_STICK, -1);
    Player_UseItem(play, this, ITEM_NONE);
}

s32 func_80842AC4(PlayState* play, Player* this) {
    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) && (this->unk_85C > 0.5f)) {

        if (GameInteractor_Should(VB_DEKU_STICK_BREAK, AMMO(ITEM_STICK) != 0)) {
            EffectSsStick_Spawn(play, &this->bodyPartsPos[PLAYER_BODYPART_R_HAND],
                                this->actor.shape.rot.y + 0x8000);
            this->unk_85C = 0.5f;
            func_80842A88(play, this);
            Player_PlaySfx(this, NA_SE_IT_WOODSTICK_BROKEN);
        }

        return 1;
    }

    return 0;
}

s32 func_80842B7C(PlayState* play, Player* this) {
    if (this->heldItemAction == PLAYER_IA_SWORD_BIGGORON) {
        if (!gSaveContext.bgsFlag && (gSaveContext.swordHealth > 0.0f)) {
            if ((gSaveContext.swordHealth -= 1.0f) <= 0.0f) {
                EffectSsStick_Spawn(play, &this->bodyPartsPos[PLAYER_BODYPART_R_HAND],
                                    this->actor.shape.rot.y + 0x8000);
                func_800849EC(play);
                Player_PlaySfx(this, NA_SE_IT_MAJIN_SWORD_BROKEN);
            }
        }

        return 1;
    }

    return 0;
}

void func_80842CF0(PlayState* play, Player* this) {
    func_80842AC4(play, this);
    func_80842B7C(play, this);
}

static LinkAnimationHeader* D_808545CC[] = {
    &gPlayerAnim_link_fighter_rebound,
    &gPlayerAnim_link_fighter_rebound_long,
    &gPlayerAnim_link_fighter_reboundR,
    &gPlayerAnim_link_fighter_rebound_longR,
};

void func_80842D20(PlayState* play, Player* this) {
    s32 pad;
    s32 sp28;

    if (Player_Action_80843188 != this->actionFunc) {
        func_80832440(play, this);
        Player_SetupAction(play, this, Player_Action_808505DC, 0);

        if (Player_CheckHostileLockOn(this)) {
            sp28 = 2;
        } else {
            sp28 = 0;
        }

        Player_AnimPlayOnceAdjusted(play, this, D_808545CC[Player_HoldsTwoHandedWeapon(this) + sp28]);
    }

    Player_RequestRumble(this, 180, 20, 100, 0);
    this->linearVelocity = -18.0f;
    func_80842CF0(play, this);
}

s32 func_80842DF4(PlayState* play, Player* this) {
    f32 phi_f2;
    CollisionPoly* sp78;
    s32 sp74;
    Vec3f sp68;
    Vec3f sp5C;
    Vec3f sp50;
    s32 temp1;
    s32 sp48;

    if (this->meleeWeaponState > 0) {
        if (this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) {
            if (!(this->meleeWeaponQuads[0].base.atFlags & AT_BOUNCED) && !(this->meleeWeaponQuads[1].base.atFlags & AT_BOUNCED)) {
                if (this->skelAnime.curFrame >= 2.0f) {

                    phi_f2 = Math_Vec3f_DistXYZAndStoreDiff(&this->meleeWeaponInfo[0].tip, &this->meleeWeaponInfo[0].base, &sp50);
                    if (phi_f2 != 0.0f) {
                        phi_f2 = (phi_f2 + 10.0f) / phi_f2;
                    }

                    sp68.x = this->meleeWeaponInfo[0].tip.x + (sp50.x * phi_f2);
                    sp68.y = this->meleeWeaponInfo[0].tip.y + (sp50.y * phi_f2);
                    sp68.z = this->meleeWeaponInfo[0].tip.z + (sp50.z * phi_f2);

                    if (BgCheck_EntityLineTest1(&play->colCtx, &sp68, &this->meleeWeaponInfo[0].tip, &sp5C, &sp78, true,
                                                false, false, true, &sp74) &&
                        !SurfaceType_IsIgnoredByEntities(&play->colCtx, sp78, sp74) &&
                        (func_80041D4C(&play->colCtx, sp78, sp74) != 6) &&
                        (func_8002F9EC(play, &this->actor, sp78, sp74, &sp5C) == 0)) {

                        if (this->heldItemAction == PLAYER_IA_HAMMER) {
                            func_80832630(play);
                            func_80842A28(play, this);
                            func_80842D20(play, this);
                            return 1;
                        }

                        if (this->linearVelocity >= 0.0f) {
                            sp48 = func_80041F10(&play->colCtx, sp78, sp74);

                            if (sp48 == 0xA) {
                                CollisionCheck_SpawnShieldParticlesWood(play, &sp5C, &this->actor.projectedPos);
                            } else {
                                CollisionCheck_SpawnShieldParticles(play, &sp5C);
                                if (sp48 == 0xB) {
                                    Player_PlaySfx(this, NA_SE_IT_WALL_HIT_SOFT);
                                } else {
                                    Player_PlaySfx(this, NA_SE_IT_WALL_HIT_HARD);
                                }
                            }

                            func_80842CF0(play, this);
                            this->linearVelocity = -14.0f;
                            Player_RequestRumble(this, 180, 20, 100, 0);
                        }
                    }
                }
            } else {
                func_80842D20(play, this);
                func_80832630(play);
                return 1;
            }
        }

        temp1 = (this->meleeWeaponQuads[0].base.atFlags & AT_HIT) || (this->meleeWeaponQuads[1].base.atFlags & AT_HIT);

        if (temp1) {
            if (this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) {
                Actor* at = this->meleeWeaponQuads[temp1 ? 1 : 0].base.at;

                if ((at != NULL) && (at->id != ACTOR_EN_KANBAN)) {
                    func_80832630(play);
                }
            }

            if ((func_80842AC4(play, this) == 0) && (this->heldItemAction != PLAYER_IA_HAMMER)) {
                func_80842B7C(play, this);

                if (this->actor.colChkInfo.atHitEffect == 1) {
                    this->actor.colChkInfo.damage = 8;
                    func_80837C0C(play, this, PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK, 0.0f, 0.0f, this->actor.shape.rot.y,
                                  20);
                    return 1;
                }
            }
        }
    }

    return 0;
}

void Player_Action_80843188(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (!Player_IsChildWithHylianShield(this)) {
            Player_AnimPlayLoop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense_wait, this->modelAnimType));
        }
        this->av2.actionVar2 = 1;
        this->av1.actionVar1 = 0;
    }

    if (!Player_IsChildWithHylianShield(this)) {
        this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
        Player_UpdateUpperBody(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_SHIELDING;
    }

    Player_DecelerateToZero(this);

    if (this->av2.actionVar2 != 0) {
        f32 sp54;
        f32 sp50;
        s16 sp4E;
        s16 sp4C;
        s16 sp4A;
        s16 sp48;
        s16 sp46;
        f32 sp40;

        sp54 = sControlInput->rel.stick_y * 100 * (CVarGetInteger(CVAR_SETTING("Controls.InvertShieldAimingYAxis"), 1) ? 1 : -1);
        sp50 = sControlInput->rel.stick_x * (CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0) ? 120 : -120) * (CVarGetInteger(CVAR_SETTING("Controls.InvertShieldAimingYAxis"), 0) ? -1 : 1);
        sp4E = this->actor.shape.rot.y - Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));

        sp40 = Math_CosS(sp4E);
        sp4C = (Math_SinS(sp4E) * sp50) + (sp54 * sp40);
        sp40 = Math_CosS(sp4E);
        sp4A = (sp50 * sp40) - (Math_SinS(sp4E) * sp54);

        if (sp4C > 3500) {
            sp4C = 3500;
        }

        sp48 = ABS(sp4C - this->actor.focus.rot.x) * 0.25f;
        if (sp48 < 100) {
            sp48 = 100;
        }

        sp46 = ABS(sp4A - this->upperLimbRot.y) * 0.25f;
        if (sp46 < 50) {
            sp46 = 50;
        }

        Math_ScaledStepToS(&this->actor.focus.rot.x, sp4C, sp48);
        this->upperLimbRot.x = this->actor.focus.rot.x;
        Math_ScaledStepToS(&this->upperLimbRot.y, sp4A, sp46);

        if (this->av1.actionVar1 != 0) {
            if (!func_80842DF4(play, this)) {
                if (this->skelAnime.curFrame < 2.0f) {
                    func_80833A20(this, 1);
                }
            } else {
                this->av2.actionVar2 = 1;
                this->av1.actionVar1 = 0;
            }
        } else if (!func_80842964(this, play)) {
            if (Player_ActionHandler_11(this, play)) {
                func_808428D8(this, play);
            } else {
                this->stateFlags1 &= ~PLAYER_STATE1_SHIELDING;
                func_80832318(this);

                if (Player_IsChildWithHylianShield(this)) {
                    func_8083A060(this, play);
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_clink_normal_defense_ALL, 1.0f,
                                         Animation_GetLastFrame(&gPlayerAnim_clink_normal_defense_ALL), 0.0f,
                                         ANIMMODE_ONCE, 0.0f);
                    Player_StartAnimMovement(play, this, 4);
                } else {
                    if (this->itemAction < 0) {
                        func_8008EC70(this);
                    }
                    func_8083A098(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense_end, this->modelAnimType), play);
                }

                Player_PlaySfx(this, NA_SE_IT_SHIELD_REMOVE);
                return;
            }
        } else {
            return;
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
    Player_SetModelsForHoldingShield(this);

    this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_UPPER_X | UNK6AE_ROT_UPPER_Y;
}

void Player_Action_808435C4(Player* this, PlayState* play) {
    s32 interruptResult;
    LinkAnimationHeader* anim;
    f32 frames;

    Player_DecelerateToZero(this);

    if (this->av1.actionVar1 == 0) {
        sUpperBodyIsBusy = Player_UpdateUpperBody(this, play);
        if ((func_80834B5C == this->upperActionFunc) || (Player_TryActionInterrupt(play, this, &this->upperSkelAnime, 4.0f) > 0)) {
            Player_SetupAction(play, this, Player_Action_80840450, 1);
        }
    } else {
        interruptResult = Player_TryActionInterrupt(play, this, &this->skelAnime, 4.0f);
        if ((interruptResult != 0) && ((interruptResult > 0) || LinkAnimation_Update(play, &this->skelAnime))) {
            Player_SetupAction(play, this, Player_Action_80843188, 1);
            this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
            Player_SetModelsForHoldingShield(this);
            anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense, this->modelAnimType);
            frames = Animation_GetLastFrame(anim);
            LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, frames, frames, ANIMMODE_ONCE, 0.0f);
        }
    }
}

void Player_Action_8084370C(Player* this, PlayState* play) {
    s32 interruptResult;

    Player_DecelerateToZero(this);

    interruptResult = Player_TryActionInterrupt(play, this, &this->skelAnime, 16.0f);
    if ((interruptResult != 0) && (LinkAnimation_Update(play, &this->skelAnime) || (interruptResult > 0))) {
        func_80839F90(this, play);
    }
}

void Player_Action_8084377C(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;

    func_808382BC(this);

    if (!(this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) && (this->av2.actionVar2 == 0) &&
        (this->knockbackType != PLAYER_KNOCKBACK_NONE)) {
        s16 temp = this->actor.shape.rot.y - this->knockbackRot;

        this->yaw = this->actor.shape.rot.y = this->knockbackRot;
        this->linearVelocity = this->knockbackSpeed;

        if (ABS(temp) > 0x4000) {
            this->actor.shape.rot.y = this->knockbackRot + 0x8000;
        }

        if (this->actor.velocity.y < 0.0f) {
            this->actor.gravity = 0.0f;
            this->actor.velocity.y = 0.0f;
        }
    }

    if (LinkAnimation_Update(play, &this->skelAnime) && (this->actor.bgCheckFlags & 1)) {
        if (this->av2.actionVar2 != 0) {
            this->av2.actionVar2--;
            if (this->av2.actionVar2 == 0) {
                func_80853080(this, play);
            }
        } else if ((this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) ||
                   (!(this->cylinder.base.acFlags & AC_HIT) && (this->knockbackType == 0))) {
            if (this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
                this->av2.actionVar2++;
            } else {
                Player_SetupAction(play, this, Player_Action_80843954, 0);
                this->stateFlags1 |= PLAYER_STATE1_DAMAGED;
            }

            Player_AnimPlayOnce(play, this,
                          (this->yaw != this->actor.shape.rot.y) ? &gPlayerAnim_link_normal_front_downB
                                                                        : &gPlayerAnim_link_normal_back_downB);
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_FREEZE);
        }
    }

    if (this->actor.bgCheckFlags & 2) {
        Player_PlayFloorSfx(this, NA_SE_PL_BOUND);
    }
}

void Player_Action_80843954(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;
    func_808382BC(this);

    Player_DecelerateToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime) && (this->linearVelocity == 0.0f)) {
        if (this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
            this->av2.actionVar2++;
        } else {
            Player_SetupAction(play, this, Player_Action_80843A38, 0);
            this->stateFlags1 |= PLAYER_STATE1_DAMAGED;
        }

        Player_AnimPlayOnceAdjusted(play, this,
                      (this->yaw != this->actor.shape.rot.y) ? &gPlayerAnim_link_normal_front_down_wake
                                                                    : &gPlayerAnim_link_normal_back_down_wake);
        this->yaw = this->actor.shape.rot.y;
    }
}

static AnimSfxEntry D_808545DC[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 20) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 30) },
};

void Player_Action_80843A38(Player* this, PlayState* play) {
    s32 sp24;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    func_808382BC(this);

    if (this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
        LinkAnimation_Update(play, &this->skelAnime);
    } else {
        sp24 = Player_TryActionInterrupt(play, this, &this->skelAnime, 16.0f);
        if ((sp24 != 0) && (LinkAnimation_Update(play, &this->skelAnime) || (sp24 > 0))) {
            func_80839F90(this, play);
        }
    }

    Player_ProcessAnimSfxList(this, D_808545DC);
}

static Vec3f D_808545E4 = { 0.0f, 0.0f, 5.0f };

void func_80843AE8(PlayState* play, Player* this) {
    if (this->av2.actionVar2 != 0) {
        if (this->av2.actionVar2 > 0) {
            this->av2.actionVar2--;
            if (this->av2.actionVar2 == 0) {
                if (this->stateFlags1 & PLAYER_STATE1_IN_WATER) {
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_swimer_swim_wait, 1.0f, 0.0f,
                                         Animation_GetLastFrame(&gPlayerAnim_link_swimer_swim_wait), ANIMMODE_ONCE,
                                         -16.0f);
                } else {
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_derth_rebirth, 1.0f, 99.0f,
                                         Animation_GetLastFrame(&gPlayerAnim_link_derth_rebirth), ANIMMODE_ONCE, 0.0f);
                }
                if (CVarGetInteger(CVAR_ENHANCEMENT("FairyReviveEffect"), 0)) {
                    if (CVarGetInteger(CVAR_ENHANCEMENT("FairyRevivePercentRestore"), 0)) {
                        gSaveContext.healthAccumulator =
                            (gSaveContext.healthCapacity * CVarGetInteger(CVAR_ENHANCEMENT("FairyReviveHealth"), 100) / 100 + 15) / 16 * 16;
                    } else {
                        gSaveContext.healthAccumulator = CVarGetInteger(CVAR_ENHANCEMENT("FairyReviveHealth"), 20) * 16;
                    }
                } else {
                    gSaveContext.healthAccumulator = 0x140;
                }
                this->av2.actionVar2 = -1;
            }
        } else if (gSaveContext.healthAccumulator == 0) {
            this->stateFlags1 &= ~PLAYER_STATE1_DEAD;
            if (this->stateFlags1 & PLAYER_STATE1_IN_WATER) {
                func_80838F18(play, this);
            } else {
                func_80853080(this, play);
            }
            this->unk_A87 = 20;
            Player_SetInvulnerability(this, -20);
            func_800F47FC();
        }
    } else if (this->av1.actionVar1 != 0) {
        this->av2.actionVar2 = 60;
        Player_SpawnFairy(play, this, &this->actor.world.pos, &D_808545E4, FAIRY_REVIVE_DEATH);
        Player_PlaySfx(this, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
        OnePointCutscene_Init(play, 9908, 125, &this->actor, MAIN_CAM);
    } else if (play->gameOverCtx.state == GAMEOVER_DEATH_WAIT_GROUND) {
        play->gameOverCtx.state = GAMEOVER_DEATH_DELAY_MENU;
        if (!CVarGetInteger(CVAR_ENHANCEMENT("PersistentMasks"), 0)) {
            gSaveContext.maskMemory = PLAYER_MASK_NONE;
        }
    }
}

static AnimSfxEntry D_808545F0[] = {
    { NA_SE_PL_BOUND, ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 60) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 140) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 164) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 170) },
};

void Player_Action_80843CEC(Player* this, PlayState* play) {
    if (this->currentTunic != PLAYER_TUNIC_GORON && CVarGetInteger(CVAR_CHEAT("SuperTunic"), 0) == 0) {
        if ((play->roomCtx.curRoom.behaviorType2 == ROOM_BEHAVIOR_TYPE2_3) || (sFloorType == 9) ||
            ((func_80838144(sFloorType) >= 0) &&
             !SurfaceType_IsWallDamage(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            func_8083821C(this);
        }
    }

    Player_DecelerateToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->actor.category == ACTORCAT_PLAYER) {
            func_80843AE8(play, this);
        }
        return;
    }

    if (this->skelAnime.animation == &gPlayerAnim_link_derth_rebirth) {
        Player_ProcessAnimSfxList(this, D_808545F0);
    } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_electric_shock_end) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 88.0f)) {
            Player_PlayFloorSfx(this, NA_SE_PL_BOUND);
        }
    }
}

void func_80843E14(Player* this, u16 sfxId) {
    Player_PlayVoiceSfx(this, sfxId);

    if ((this->heldActor != NULL) && (this->heldActor->id == ACTOR_EN_RU1)) {
        Audio_PlayActorSound2(this->heldActor, NA_SE_VO_RT_FALL);
    }
}

static FallImpactInfo D_80854600[] = {
    { -8, 180, 40, 100, NA_SE_VO_LI_LAND_DAMAGE_S },
    { -16, 255, 140, 150, NA_SE_VO_LI_LAND_DAMAGE_S },
};

s32 func_80843E64(PlayState* play, Player* this) {
    s32 sp34;

    if ((sFloorType == 6) || (sFloorType == 9)) {
        sp34 = 0;
    } else {
        sp34 = this->fallDistance;
    }

    Math_StepToF(&this->linearVelocity, 0.0f, 1.0f);

    this->stateFlags1 &= ~(PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALL);

    if (sp34 >= 400) {
        s32 impactIndex;
        FallImpactInfo* impactInfo;

        if (this->fallDistance < 800) {
            impactIndex = 0;
        } else {
            impactIndex = 1;
        }

        impactInfo = &D_80854600[impactIndex];

        if (Player_InflictDamageModified(play, impactInfo->damage * (1 << CVarGetInteger(CVAR_ENHANCEMENT("FallDamageMult"), 0)),
                                         false)) {
            return -1;
        }

        Player_SetIntangibility(this, 40);
        Player_RequestQuake(play, 32967, 2, 30);
        Player_RequestRumble(this, impactInfo->rumbleStrength, impactInfo->rumbleDuration, impactInfo->rumbleDecreaseRate, 0);
        Player_PlaySfx(this, NA_SE_PL_BODY_HIT);
        Player_PlayVoiceSfx(this, impactInfo->sfxId);

        return impactIndex + 1;
    }

    if (sp34 > 200) {
        sp34 *= 2;

        if (sp34 > 255) {
            sp34 = 255;
        }

        Player_RequestRumble(this, (u8)sp34, (u8)(sp34 * 0.1f), (u8)sp34, 0);

        if (sFloorType == 6) {
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_CLIMB_END);
        }
    }

    Player_PlayLandingSfx(this);

    return 0;
}

void func_8084409C(PlayState* play, Player* this, f32 speedXZ, f32 velocityY) {
    Actor* heldActor = this->heldActor;

    if (!func_80835644(play, this, heldActor)) {
        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speedXZ = speedXZ;
        heldActor->velocity.y = velocityY;
        func_80834644(play, this);
        Player_PlaySfx(this, NA_SE_PL_THROW);
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_SWORD_N);
    }
}

void Player_Action_8084411C(Player* this, PlayState* play) {
    f32 sp4C;
    s16 sp4A;

    if (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 40) {
        this->actor.gravity = 0.0f;
    } else if (Player_CheckHostileLockOn(this)) {
        this->actor.gravity = -1.2f;
    }

    Player_GetMovementSpeedAndYaw(this, &sp4C, &sp4A, SPEED_MODE_LINEAR, play);

    if (!(this->actor.bgCheckFlags & 1)) {
        if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
            Actor* heldActor = this->heldActor;

            u16 buttonsToCheck = BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
            if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) != 0) {
                buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
            }
            if (!func_80835644(play, this, heldActor) && (heldActor->id == ACTOR_EN_NIW) &&
                CHECK_BTN_ANY(sControlInput->press.button, buttonsToCheck)) {
                func_8084409C(play, this, this->linearVelocity + 2.0f, this->actor.velocity.y + 2.0f);
            }
        }

        LinkAnimation_Update(play, &this->skelAnime);

        if (!(this->stateFlags2 & PLAYER_STATE2_HOPPING)) {
            func_8083DFE0(this, &sp4C, &sp4A);
        }

        Player_UpdateUpperBody(this, play);

        if (((this->stateFlags2 & PLAYER_STATE2_HOPPING) && (this->av1.actionVar1 == 2)) || !func_8083BBA0(this, play)) {
            if (this->actor.velocity.y < 0.0f) {
                if (this->av2.actionVar2 >= 0) {
                    if ((this->actor.bgCheckFlags & 8) || (this->av2.actionVar2 == 0) || (this->fallDistance > 0)) {
                        if ((sYDistToFloor > 800.0f) || (this->stateFlags1 & PLAYER_STATE1_HOOKSHOT_FALLING)) {
                            func_80843E14(this, NA_SE_VO_LI_FALL_S);
                            this->stateFlags1 &= ~PLAYER_STATE1_HOOKSHOT_FALLING;
                        }

                        LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_normal_landing, 1.0f, 0.0f,
                                             0.0f, ANIMMODE_ONCE, 8.0f);
                        this->av2.actionVar2 = -1;
                    }
                } else {
                    if ((this->av2.actionVar2 == -1) && (this->fallDistance > 120.0f) && (sYDistToFloor > 280.0f)) {
                        this->av2.actionVar2 = -2;
                        func_80843E14(this, NA_SE_VO_LI_FALL_L);
                    }

                    if (!GameInteractor_GetDisableLedgeGrabsActive() && (this->actor.bgCheckFlags & 0x200) &&
                        !(this->stateFlags2 & PLAYER_STATE2_HOPPING) &&
                        !(this->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_IN_WATER)) && (this->linearVelocity > 0.0f)) {
                        if ((this->yDistToLedge >= 150.0f) && (this->controlStickDirections[this->controlStickDataIndex] == 0)) {
                            func_8083EC18(this, play, sTouchedWallFlags);
                        } else if ((this->ledgeClimbType >= 2) && (this->yDistToLedge < 150.0f) &&
                                   (((this->actor.world.pos.y - this->actor.floorHeight) + this->yDistToLedge) >
                                    (70.0f * this->ageProperties->unk_08))) {
                            AnimationContext_DisableQueue(play);
                            if (this->stateFlags1 & PLAYER_STATE1_HOOKSHOT_FALLING) {
                                Player_PlayVoiceSfx(this, NA_SE_VO_LI_HOOKSHOT_HANG);
                            } else {
                                Player_PlayVoiceSfx(this, NA_SE_VO_LI_HANG);
                            }
                            this->actor.world.pos.y += this->yDistToLedge;
                            func_8083A5C4(play, this, this->actor.wallPoly, this->distToInteractWall,
                                          GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_hold, this->modelAnimType));
                            this->actor.shape.rot.y = this->yaw += 0x8000;
                            this->stateFlags1 |= PLAYER_STATE1_HANGING_OFF_LEDGE;
                        }
                    }
                }
            }
        }
    } else {
        LinkAnimationHeader* anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_landing, this->modelAnimType);
        s32 sp3C;

        if (this->stateFlags2 & PLAYER_STATE2_HOPPING) {
            if (Player_CheckHostileLockOn(this)) {
                anim = D_80853D4C[this->av1.actionVar1][2];
            } else {
                anim = D_80853D4C[this->av1.actionVar1][1];
            }
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_run_jump) {
            anim = &gPlayerAnim_link_normal_run_jump_end;
        } else if (Player_CheckHostileLockOn(this)) {
            anim = &gPlayerAnim_link_anchor_landingR;
            func_80833C3C(this);
        } else if (this->fallDistance <= 80) {
            anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_short_landing, this->modelAnimType);
        } else if ((this->fallDistance < 800) && (this->controlStickDirections[this->controlStickDataIndex] == 0) &&
                   !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            Player_SetupRoll(this, play);
            return;
        }

        sp3C = func_80843E64(play, this);

        if (sp3C > 0) {
            func_8083A098(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_landing, this->modelAnimType), play);
            this->skelAnime.endFrame = 8.0f;
            if (sp3C == 1) {
                this->av2.actionVar2 = 10;
            } else {
                this->av2.actionVar2 = 20;
            }
        } else if (sp3C == 0) {
            func_8083A098(this, anim, play);
        }
    }
}

static AnimSfxEntry sRollAnimSfxList[] = {
    { NA_SE_VO_LI_SWORD_N, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 1) },
    { NA_SE_PL_WALK_GROUND, ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR_BY_AGE, 6) },
    { NA_SE_PL_ROLL, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 6) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 18) },
};

void Player_Action_Roll(Player* this, PlayState* play) {
    Actor* ocCollidedActor;
    s32 interruptResult;
    s32 animDone;
    DynaPolyActor* wallPolyActor;
    s32 pad;
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    ocCollidedActor = NULL;
    animDone = LinkAnimation_Update(play, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 8.0f)) {
        Player_SetInvulnerability(this, -10);
    }

    if (!func_80842964(this, play)) {
        if (this->av2.bonked) {
            Math_StepToF(&this->linearVelocity, 0.0f, 2.0f);

            interruptResult = Player_TryActionInterrupt(play, this, &this->skelAnime, 5.0f);
            if ((interruptResult != PLAYER_INTERRUPT_NEW_ACTION) &&
                ((interruptResult >= PLAYER_INTERRUPT_MOVE) || animDone)) {
                func_8083A060(this, play);
            }
        } else {
            // Must have a speed of 7 or above to be able to bonk into something
            if (this->linearVelocity >= 7.0f) {
                bool randomBonk = GameInteractor_GetRandomBonksActive() && (Rand_ZeroOne() <= .05);
                if (randomBonk || ((this->actor.bgCheckFlags & 0x200) && (sWorldYawToTouchedWall < 0x2000)) ||
                    ((this->cylinder.base.ocFlags1 & OC1_HIT) &&
                     (ocCollidedActor = this->cylinder.base.oc,
                      ((ocCollidedActor->id == ACTOR_EN_WOOD02) &&
                       (ABS((s16)(this->actor.world.rot.y - ocCollidedActor->yawTowardsPlayer)) > 0x6000))))) {

                    if (ocCollidedActor != NULL) {
                        // The EN_WOOD02 actor uses home y rotation as a flag to signal that it has been
                        // bonked into and should try to spawn a drop.
                        ocCollidedActor->home.rot.y = 1;
                    } else if (this->actor.wallBgId != BGCHECK_SCENE) {
                        wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);

                        if ((wallPolyActor != NULL) && (wallPolyActor->actor.id == ACTOR_OBJ_KIBAKO2)) {
                            // The OBJ_KIBAKO2 actor uses home z rotation as a flag to signal that it has been
                            // bonked into and should break.
                            wallPolyActor->actor.home.rot.z = 1;
                        }
                    }

                    Player_AnimPlayOnce(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_hip_down, this->modelAnimType));
                    this->linearVelocity = -this->linearVelocity;
                    Player_RequestQuake(play, 33267, 3, 12);
                    Player_RequestRumble(this, 255, 20, 150, 0);
                    Player_PlaySfx(this, NA_SE_PL_BODY_HIT);
                    Player_PlayVoiceSfx(this, NA_SE_VO_LI_CLIMB_END);
                    this->av2.bonked = 1;

                    gSaveContext.sohStats.count[COUNT_BONKS]++;
                    GameInteractor_ExecuteOnPlayerBonk();

                    return;
                }
            }

            if ((this->skelAnime.curFrame < 15.0f) || !Player_ActionHandler_7(this, play)) {
                if (this->skelAnime.curFrame >= 20.0f) {
                    func_8083A060(this, play);

                    return;
                }

                Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

                // `speedTarget` at this point is the speed that would be used for regular walking.
                // Rolling speed is 1.5 times faster than what the walking speed would be for the current control stick
                // input.
                speedTarget *= 1.5f;

                if ((speedTarget < 3.0f) || (this->controlStickDirections[this->controlStickDataIndex] != 0)) {
                    speedTarget = 3.0f;
                }

                func_8083DF68(this, speedTarget, this->actor.shape.rot.y);

                if (func_8084269C(play, this)) {
                    func_8002F8F0(&this->actor, NA_SE_PL_ROLL_DUST - SFX_FLAG);
                }

                Player_ProcessAnimSfxList(this, sRollAnimSfxList);
            }
        }
    }
}

void Player_Action_80844A44(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimPlayLoop(play, this, &gPlayerAnim_link_normal_run_jump_water_fall_wait);
    }

    Math_StepToF(&this->linearVelocity, 0.0f, 0.05f);

    if (this->actor.bgCheckFlags & 1) {
        this->actor.colChkInfo.damage = 0x10;
        func_80837C0C(play, this, PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
    }
}

void Player_Action_80844AF4(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    this->actor.gravity = -1.2f;
    LinkAnimation_Update(play, &this->skelAnime);

    if (!func_80842DF4(play, this)) {
        func_8084285C(this, 6.0f, 7.0f, 99.0f);

        if (!(this->actor.bgCheckFlags & 1)) {
            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
            func_8083DFE0(this, &speedTarget, &this->yaw);
            return;
        }

        if (func_80843E64(play, this) >= 0) {
            this->meleeWeaponAnimation += 2;
            func_80837948(play, this, this->meleeWeaponAnimation);
            this->unk_845 = 3;
            Player_PlayLandingSfx(this);
        }
    }
}

s32 func_80844BE4(Player* this, PlayState* play) {
    s32 temp;

    if (Player_StartCsAction(play, this)) {
        this->stateFlags2 |= PLAYER_STATE2_SPIN_ATTACKING;
    } else {
        if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
            if ((this->unk_858 >= 0.85f) || Player_CanSpinAttack(this)) {
                temp = D_80854384[Player_HoldsTwoHandedWeapon(this)];
            } else {
                temp = D_80854380[Player_HoldsTwoHandedWeapon(this)];
            }

            func_80837948(play, this, temp);
            Player_SetInvulnerability(this, -8);

            this->stateFlags2 |= PLAYER_STATE2_SPIN_ATTACKING;
            if (this->controlStickDirections[this->controlStickDataIndex] == 0) {
                this->stateFlags2 |= PLAYER_STATE2_SWORD_LUNGE;
            }
        } else {
            return 0;
        }
    }

    return 1;
}

void func_80844CF8(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_80845000, 1);
}

void func_80844D30(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_80845308, 1);
}

void func_80844D68(Player* this, PlayState* play) {
    func_80839FFC(this, play);
    func_80832318(this);
    Player_AnimChangeOnceMorph(play, this, D_80854368[Player_HoldsTwoHandedWeapon(this)]);
    this->yaw = this->actor.shape.rot.y;
}

void func_80844DC8(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_80844E68, 1);
    this->unk_868 = 0.0f;
    Player_AnimPlayLoop(play, this, D_80854360[Player_HoldsTwoHandedWeapon(this)]);
    this->av2.actionVar2 = 1;
}

void func_80844E3C(Player* this) {
    Math_StepToF(&this->unk_858, 1.0f, 0.02f);
}

void Player_Action_80844E68(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp;

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_FinishAnimMovement(this);
        Player_SetParallel(this);
        this->stateFlags1 &= ~PLAYER_STATE1_PARALLEL;
        Player_AnimPlayLoop(play, this, D_80854360[Player_HoldsTwoHandedWeapon(this)]);
        this->av2.actionVar2 = -1;
    }

    Player_DecelerateToZero(this);

    if (!func_80842964(this, play) && (this->av2.actionVar2 != 0)) {
        func_80844E3C(this);

        if (this->av2.actionVar2 < 0) {
            if (this->unk_858 >= 0.1f) {
                this->unk_845 = 0;
                this->av2.actionVar2 = 1;
            } else if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
                func_80844D68(this, play);
            }
        } else if (!func_80844BE4(this, play)) {
            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

            temp = func_80840058(this, &speedTarget, &yawTarget, play);
            if (temp > 0) {
                func_80844CF8(this, play);
            } else if (temp < 0) {
                func_80844D30(this, play);
            }
        }
    }
}

void Player_Action_80845000(Player* this, PlayState* play) {
    s16 temp1;
    s32 temp2;
    f32 sp5C;
    f32 sp58;
    f32 sp54;
    s16 sp52;
    s32 temp4;
    s16 temp5;
    s32 sp44;

    temp1 = this->yaw - this->actor.shape.rot.y;
    temp2 = ABS(temp1);

    sp5C = fabsf(this->linearVelocity);
    sp58 = sp5C * 1.5f;

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (sp58 < 1.5f) {
        sp58 = 1.5f;
    }

    sp58 = ((temp2 < 0x4000) ? -1.0f : 1.0f) * sp58;

    func_8084029C(this, sp58);

    sp58 = CLAMP(sp5C * 0.5f, 0.5f, 1.0f);

    LinkAnimation_BlendToJoint(play, &this->skelAnime, D_80854360[Player_HoldsTwoHandedWeapon(this)], 0.0f,
                               D_80854370[Player_HoldsTwoHandedWeapon(this)], this->unk_868 * (21.0f / 29.0f), sp58,
                               this->blendTable);

    if (!func_80842964(this, play) && !func_80844BE4(this, play)) {
        func_80844E3C(this);
        Player_GetMovementSpeedAndYaw(this, &sp54, &sp52, SPEED_MODE_LINEAR, play);

        temp4 = func_80840058(this, &sp54, &sp52, play);

        if (temp4 < 0) {
            func_80844D30(this, play);
            return;
        }

        if (temp4 == 0) {
            sp54 = 0.0f;
            sp52 = this->yaw;
        }

        temp5 = sp52 - this->yaw;
        sp44 = ABS(temp5);

        if (sp44 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
                this->yaw = sp52;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, sp54 * 0.2f, 1.0f, 0.5f);
        Math_ScaledStepToS(&this->yaw, sp52, sp44 * 0.1f);

        if ((sp54 == 0.0f) && (this->linearVelocity == 0.0f)) {
            func_80844DC8(this, play);
        }
    }
}

void Player_Action_80845308(Player* this, PlayState* play) {
    f32 sp5C;
    f32 sp58;
    f32 sp54;
    s16 sp52;
    s32 temp4;
    s16 temp5;
    s32 sp44;

    sp5C = fabsf(this->linearVelocity);

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (sp5C == 0.0f) {
        sp5C = ABS(this->unk_87C) * 0.0015f;
        if (sp5C < 400.0f) {
            sp5C = 0.0f;
        }
        func_8084029C(this, ((this->unk_87C >= 0) ? 1 : -1) * sp5C);
    } else {
        sp58 = sp5C * 1.5f;
        if (sp58 < 1.5f) {
            sp58 = 1.5f;
        }
        func_8084029C(this, sp58);
    }

    sp58 = CLAMP(sp5C * 0.5f, 0.5f, 1.0f);

    LinkAnimation_BlendToJoint(play, &this->skelAnime, D_80854360[Player_HoldsTwoHandedWeapon(this)], 0.0f,
                               D_80854378[Player_HoldsTwoHandedWeapon(this)], this->unk_868 * (21.0f / 29.0f), sp58,
                               this->blendTable);

    if (!func_80842964(this, play) && !func_80844BE4(this, play)) {
        func_80844E3C(this);
        Player_GetMovementSpeedAndYaw(this, &sp54, &sp52, SPEED_MODE_LINEAR, play);

        temp4 = func_80840058(this, &sp54, &sp52, play);

        if (temp4 > 0) {
            func_80844CF8(this, play);
            return;
        }

        if (temp4 == 0) {
            sp54 = 0.0f;
            sp52 = this->yaw;
        }

        temp5 = sp52 - this->yaw;
        sp44 = ABS(temp5);

        if (sp44 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
                this->yaw = sp52;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, sp54 * 0.2f, 1.0f, 0.5f);
        Math_ScaledStepToS(&this->yaw, sp52, sp44 * 0.1f);

        if ((sp54 == 0.0f) && (this->linearVelocity == 0.0f) && (sp5C == 0.0f)) {
            func_80844DC8(this, play);
        }
    }
}

void Player_Action_80845668(Player* this, PlayState* play) {
    s32 sp3C;
    f32 temp1;
    s32 temp2;
    f32 temp3;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    sp3C = LinkAnimation_Update(play, &this->skelAnime);

    if (this->skelAnime.animation == &gPlayerAnim_link_normal_250jump_start) {
        this->linearVelocity = 1.0f;

        if (LinkAnimation_OnFrame(&this->skelAnime, 8.0f)) {
            temp1 = this->yDistToLedge;

            if (temp1 > this->ageProperties->unk_0C) {
                temp1 = this->ageProperties->unk_0C;
            }

            if (this->stateFlags1 & PLAYER_STATE1_IN_WATER) {
                temp1 *= 0.085f;
            } else {
                temp1 *= 0.072f;
            }

            if (!LINK_IS_ADULT) {
                temp1 += 1.0f;
            }

            func_80838940(this, NULL, temp1, play, NA_SE_VO_LI_AUTO_JUMP);
            this->av2.actionVar2 = -1;
            return;
        }
    } else {
        temp2 = Player_TryActionInterrupt(play, this, &this->skelAnime, 4.0f);

        if (temp2 == 0) {
            this->stateFlags1 &= ~(PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_JUMPING);
            return;
        }

        if ((sp3C != 0) || (temp2 > 0)) {
            func_8083C0E8(this, play);
            this->stateFlags1 &= ~(PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_JUMPING);
            return;
        }

        temp3 = 0.0f;

        if (this->skelAnime.animation == &gPlayerAnim_link_swimer_swim_15step_up) {
            if (LinkAnimation_OnFrame(&this->skelAnime, 30.0f)) {
                func_8083D0A8(play, this, 10.0f);
            }
            temp3 = 50.0f;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_150step_up) {
            temp3 = 30.0f;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_100step_up) {
            temp3 = 16.0f;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, temp3)) {
            Player_PlayLandingSfx(this);
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_CLIMB_END);
        }

        if ((this->skelAnime.animation == &gPlayerAnim_link_normal_100step_up) || (this->skelAnime.curFrame > 5.0f)) {
            if (this->av2.actionVar2 == 0) {
                Player_PlayJumpingSfx(this);
                this->av2.actionVar2 = 1;
            }
            Math_StepToF(&this->actor.shape.yOffset, 0.0f, 150.0f);
        }
    }
}

/**
 * Allow the held item put away process to complete before running `afterPutAwayFunc`
 */
void Player_Action_WaitForPutAway(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;
    LinkAnimation_Update(play, &this->skelAnime);

    // Wait for the held item put away process to complete.
    // Determining if the put away process is complete is a bit complicated:
    // `Player_UpdateUpperBody` will only return false if the current UpperAction returns false.
    // The UpperAction responsible for putting away items, `Player_UpperAction_ChangeHeldItem`, constantly
    // returns true until the item change is done. False won't be returned until the item change is done, and a new
    // UpperAction is running and can return false itself.
    // Note that this implementation allows for delaying indefinitely by, for example, holding shield
    // during the item put away. The shield UpperAction will return true while shielding and targeting.
    // Meaning, `afterPutAwayFunc` will be delayed until the player decides to let go of shield.
    // This quirk can contribute to the possibility of other bugs manifesting.
    //
    // The other conditions listed will force the put away delay function to run instantly if carrying an actor.
    // This is necessary because the UpperAction for carrying actors will always return true while holding
    // the actor, so `!Player_UpdateUpperBody` could never pass.
    if (((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->heldActor != NULL) && (this->getItemId == GI_NONE)) ||
        !Player_UpdateUpperBody(this, play)) {
        this->afterPutAwayFunc(play, this);
    }
}

s32 func_80845964(PlayState* play, Player* this, CsCmdActorCue* cue, f32 arg3, s16 arg4, s32 arg5) {
    if ((arg5 != 0) && (this->linearVelocity == 0.0f)) {
        return LinkAnimation_Update(play, &this->skelAnime);
    }

    if (arg5 != 2) {
        f32 sp34 = R_UPDATE_RATE * 0.5f;
        f32 selfDistX = cue->endPos.x - this->actor.world.pos.x;
        f32 selfDistZ = cue->endPos.z - this->actor.world.pos.z;
        f32 sp28 = sqrtf(SQ(selfDistX) + SQ(selfDistZ)) / sp34;
        s32 sp24 = (cue->endFrame - play->csCtx.frames) + 1;

        arg4 = Math_Atan2S(selfDistZ, selfDistX);

        if (arg5 == 1) {
            f32 distX = cue->endPos.x - cue->startPos.x;
            f32 distZ = cue->endPos.z - cue->startPos.z;
            s32 temp = (((sqrtf(SQ(distX) + SQ(distZ)) / sp34) / (cue->endFrame - cue->startFrame)) / 1.5f) * 4.0f;

            if (temp >= sp24) {
                arg4 = this->actor.shape.rot.y;
                arg3 = 0.0f;
            } else {
                arg3 = sp28 / ((sp24 - temp) + 1);
            }
        } else {
            arg3 = sp28 / sp24;
        }
    }

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    func_80841EE4(this, play);
    func_8083DF68(this, arg3, arg4);

    if ((arg3 == 0.0f) && (this->linearVelocity == 0.0f)) {
        func_8083BF50(this, play);
    }

    return 0;
}

s32 func_80845BA0(PlayState* play, Player* this, f32* arg2, s32 arg3) {
    f32 dx = this->unk_450.x - this->actor.world.pos.x;
    f32 dz = this->unk_450.z - this->actor.world.pos.z;
    s32 sp2C = sqrtf(SQ(dx) + SQ(dz));
    s16 yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_450);

    if (sp2C < arg3) {
        *arg2 = 0.0f;
        yaw = this->actor.shape.rot.y;
    }

    if (func_80845964(play, this, NULL, *arg2, yaw, 2)) {
        return 0;
    }

    return sp2C;
}

s32 func_80845C68(PlayState* play, s32 arg1) {
    if (arg1 == 0) {
        Play_SetupRespawnPoint(play, RESPAWN_MODE_DOWN, 0xDFF);
    }
    gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 0;
    return arg1;
}

void Player_Action_80845CA4(Player* this, PlayState* play) {
    f32 sp3C;
    s32 temp;
    f32 sp34;
    s32 sp30;
    s32 pad;

    if (!Player_ActionHandler_13(this, play)) {
        if (this->av2.actionVar2 == 0) {
            LinkAnimation_Update(play, &this->skelAnime);

            if (DECR(this->doorTimer) == 0) {
                this->linearVelocity = 0.1f;
                this->av2.actionVar2 = 1;
            }
        } else if (this->av1.actionVar1 == 0) {
            sp3C = 5.0f * sWaterSpeedFactor;

            if (func_80845BA0(play, this, &sp3C, -1) < 30) {
                this->av1.actionVar1 = 1;
                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;

                this->unk_450.x = this->unk_45C.x;
                this->unk_450.z = this->unk_45C.z;
            }
        } else {
            sp34 = 5.0f;
            sp30 = 20;

            if (this->stateFlags1 & PLAYER_STATE1_LOADING) {
                sp34 = gSaveContext.entranceSpeed;

                if (sConveyorSpeed != 0) {
                    this->unk_450.x = (Math_SinS(sConveyorYaw) * 400.0f) + this->actor.world.pos.x;
                    this->unk_450.z = (Math_CosS(sConveyorYaw) * 400.0f) + this->actor.world.pos.z;
                }
            } else if (this->av2.actionVar2 < 0) {
                this->av2.actionVar2++;

                sp34 = gSaveContext.entranceSpeed;
                sp30 = -1;
            }

            temp = func_80845BA0(play, this, &sp34, sp30);

            if ((this->av2.actionVar2 == 0) ||
                ((temp == 0) && (this->linearVelocity == 0.0f) && (Play_GetCamera(play, 0)->unk_14C & 0x10))) {

                func_8005B1A4(Play_GetCamera(play, 0));
                func_80845C68(play, gSaveContext.respawn[RESPAWN_MODE_DOWN].data);

                if (!Player_ActionHandler_Talk(this, play)) {
                    func_8083CF5C(this, play);
                }
            }
        }
    }

    if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        Player_UpdateUpperBody(this, play);
    }
}

void Player_Action_80845EF8(Player* this, PlayState* play) {
    s32 sp2C;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    sp2C = LinkAnimation_Update(play, &this->skelAnime);

    Player_UpdateUpperBody(this, play);

    if (sp2C) {
        if (this->av2.actionVar2 == 0) {
            if (DECR(this->doorTimer) == 0) {
                this->av2.actionVar2 = 1;
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        } else {
            func_8083C0E8(this, play);
            if (play->roomCtx.prevRoom.num >= 0) {
                func_80097534(play, &play->roomCtx);
            }
            func_8005B1A4(Play_GetCamera(play, 0));
            Play_SetupRespawnPoint(play, 0, 0xDFF);
        }
        return;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) && LinkAnimation_OnFrame(&this->skelAnime, 15.0f)) {
        play->func_11D54(this, play);
    }
}

void Player_Action_80846050(Player* this, PlayState* play) {
    Player_DecelerateToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_80839F90(this, play);
        func_80835688(this, play);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor* interactRangeActor = this->interactRangeActor;

        if (!func_80835644(play, this, interactRangeActor)) {
            this->heldActor = interactRangeActor;
            this->actor.child = interactRangeActor;
            interactRangeActor->parent = &this->actor;
            interactRangeActor->bgCheckFlags &= 0xFF00;
            this->unk_3BC.y = interactRangeActor->shape.rot.y - this->actor.shape.rot.y;
        }
        return;
    }

    Math_ScaledStepToS(&this->unk_3BC.y, 0, 4000);
}

static AnimSfxEntry D_8085461C[] = {
    { NA_SE_VO_LI_SWORD_L, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 49) },
    { NA_SE_VO_LI_SWORD_N, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 230) },
};

void Player_Action_80846120(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime) && (this->av2.actionVar2++ > 20)) {
        if (!Player_ActionHandler_13(this, play)) {
            func_8083A098(this, &gPlayerAnim_link_normal_heavy_carry_end, play);
        }
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 41.0f)) {
        BgHeavyBlock* heavyBlock = (BgHeavyBlock*)this->interactRangeActor;

        this->heldActor = &heavyBlock->dyna.actor;
        this->actor.child = &heavyBlock->dyna.actor;
        heavyBlock->dyna.actor.parent = &this->actor;
        func_8002DBD0(&heavyBlock->dyna.actor, &heavyBlock->unk_164, &this->leftHandPos);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 229.0f)) {
        Actor* heldActor = this->heldActor;

        if (GameInteractor_Should(VB_MOVE_THROWN_ACTOR, true, heldActor)) {
            heldActor->speedXZ = Math_SinS(heldActor->shape.rot.x) * 40.0f;
            heldActor->velocity.y = Math_CosS(heldActor->shape.rot.x) * 40.0f;
            heldActor->gravity = -2.0f;
            heldActor->minVelocityY = -30.0f;
        }
        Player_DetachHeldActor(play, this);
        return;
    }

    Player_ProcessAnimSfxList(this, D_8085461C);
}

void Player_Action_80846260(Player* this, PlayState* play) {
    Player_DecelerateToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimPlayLoop(play, this, &gPlayerAnim_link_silver_wait);
        this->av2.actionVar2 = 1;
        return;
    }

    u16 buttonsToCheck = BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) != 0) {
        buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
    }
    if (this->av2.actionVar2 == 0) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 27.0f)) {
            Actor* interactRangeActor = this->interactRangeActor;

            this->heldActor = interactRangeActor;
            this->actor.child = interactRangeActor;
            interactRangeActor->parent = &this->actor;
            return;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, 25.0f)) {
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_SWORD_L);
            return;
        }

    } else if (CHECK_BTN_ANY(sControlInput->press.button, buttonsToCheck)) {
        Player_SetupAction(play, this, Player_Action_80846358, 1);
        Player_AnimPlayOnce(play, this, &gPlayerAnim_link_silver_throw);
    }
}

void Player_Action_80846358(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_80839F90(this, play);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 6.0f)) {
        Actor* heldActor = this->heldActor;

        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speedXZ = 10.0f;
        heldActor->velocity.y = 20.0f;
        func_80834644(play, this);
        Player_PlaySfx(this, NA_SE_PL_THROW);
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_SWORD_N);
    }
}

void Player_Action_80846408(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimPlayLoop(play, this, &gPlayerAnim_link_normal_nocarry_free_wait);
        this->av2.actionVar2 = 15;
        return;
    }

    if (this->av2.actionVar2 != 0) {
        this->av2.actionVar2--;
        if (this->av2.actionVar2 == 0) {
            func_8083A098(this, &gPlayerAnim_link_normal_nocarry_free_end, play);
            this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_DAMAGE_S);
        }
    }
}

void Player_Action_808464B0(Player* this, PlayState* play) {
    Player_DecelerateToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_80839F90(this, play);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor* heldActor = this->heldActor;

        if (!func_80835644(play, this, heldActor)) {
            heldActor->velocity.y = 0.0f;
            heldActor->speedXZ = 0.0f;
            func_80834644(play, this);
            if (heldActor->id == ACTOR_EN_BOM_CHU && !CVarGetInteger(CVAR_ENHANCEMENT("DisableFirstPersonChus"), 0)) {
                func_8083B8F4(this, play);
            }
        }
    }
}

void Player_Action_80846578(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;

    Player_DecelerateToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime) ||
        ((this->skelAnime.curFrame >= 8.0f) && Player_GetMovementSpeedAndYaw(this, &sp34, &sp32, SPEED_MODE_CURVED, play))) {
        func_80839F90(this, play);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 3.0f)) {
        func_8084409C(play, this, this->linearVelocity + 8.0f, 12.0f);
    }
}

static ColliderCylinderInit D_80854624 = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 12, 60, 0, { 0, 0, 0 } },
};

static ColliderQuadInit D_80854650 = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000100, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderQuadInit D_808546A0 = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_HARD | AC_TYPE_ENEMY,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0xDFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

void func_8084663C(Actor* thisx, PlayState* play) {
}

void func_80846648(PlayState* play, Player* this) {
    this->actor.update = func_8084663C;
    this->actor.draw = NULL;
}

void func_80846660(PlayState* play, Player* this) {
    Player_SetupAction(play, this, Player_Action_8084F710, 0);
    if ((play->sceneNum == SCENE_LAKE_HYLIA) && (gSaveContext.sceneSetupIndex >= 4)) {
        this->av1.actionVar1 = 1;
    }
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_okarina_warp_goal, 2.0f / 3.0f, 0.0f, 24.0f,
                         ANIMMODE_ONCE, 0.0f);
    this->actor.world.pos.y += 800.0f;
}

static u8 D_808546F0[] = { ITEM_SWORD_MASTER, ITEM_SWORD_KOKIRI };

void func_80846720(PlayState* play, Player* this, s32 arg2) {
    s32 item = D_808546F0[(void)0, gSaveContext.linkAge];
    s32 itemAction = sItemActions[item];

    Player_DestroyHookshot(this);
    Player_DetachHeldActor(play, this);

    this->heldItemId = item;
    this->nextModelGroup = Player_ActionToModelGroup(this, itemAction);

    Player_InitItemAction(play, this, itemAction);
    func_80834644(play, this);

    if (arg2 != 0) {
        Player_PlaySfx(this, NA_SE_IT_SWORD_PICKOUT);
    }
}

static Vec3f D_808546F4 = { -1.0f, 69.0f, 20.0f };

void func_808467D4(PlayState* play, Player* this) {
    Player_SetupAction(play, this, Player_Action_8084E9AC, 0);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    Math_Vec3f_Copy(&this->actor.world.pos, &D_808546F4);
    this->yaw = this->actor.shape.rot.y = -0x8000;
    LinkAnimation_Change(play, &this->skelAnime, this->ageProperties->unk_A0, 2.0f / 3.0f, 0.0f, 0.0f,
                         ANIMMODE_ONCE, 0.0f);
    Player_StartAnimMovement(play, this, 0x28F);
    if (LINK_IS_ADULT) {
        func_80846720(play, this, 0);
    }
    this->av2.actionVar2 = 20;
}

void func_808468A8(PlayState* play, Player* this) {
    Player_SetupAction(play, this, Player_Action_8084F9A0, 0);
    Player_StartAnimMovement(play, this, 0x9B);
}

void func_808468E8(PlayState* play, Player* this) {
    func_808389E8(this, &gPlayerAnim_link_normal_jump, 12.0f, play);
    Player_SetupAction(play, this, Player_Action_8084F9C0, 0);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    this->fallStartHeight = this->actor.world.pos.y;
    OnePointCutscene_Init(play, 5110, 40, &this->actor, MAIN_CAM);
}

void func_80846978(PlayState* play, Player* this) {
    func_80837C0C(play, this, PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE, 2.0f, 2.0f, this->actor.shape.rot.y + 0x8000, 0);
}

void func_808469BC(PlayState* play, Player* this) {
    Player_SetupAction(play, this, Player_Action_8084F698, 0);
    this->actor.draw = NULL;
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
}

Actor* Player_SpawnMagicSpell(PlayState* play, Player* this, s32 spell) {
    static s16 sMagicSpellActorIds[] = { ACTOR_MAGIC_WIND, ACTOR_MAGIC_DARK, ACTOR_MAGIC_FIRE };

    return Actor_Spawn(&play->actorCtx, play, sMagicSpellActorIds[spell], this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0, true);
}

void func_80846A68(PlayState* play, Player* this) {
    this->actor.draw = NULL;
    Player_SetupAction(play, this, Player_Action_8085076C, 0);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 500, ICHAIN_STOP),
};

static EffectBlureInit2 blureSword = {
    0, 8, 0, { 255, 255, 255, 255 }, { 255, 255, 255, 64 }, { 255, 255, 255, 0 }, { 255, 255, 255, 0 }, 4,
    0, 2, 0, { 255, 255, 255, 255 }, { 255, 255, 255, 64 }, 1,
};

static Vec3s sSkeletonBaseTransl = { -57, 3377, 0 };

void Player_InitCommon(Player* this, PlayState* play, FlexSkeletonHeader* skelHeader) {
    this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    this->ageProperties = &sAgeProperties[gSaveContext.linkAge];
    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->meleeWeaponEffectIndex = TOTAL_EFFECT_COUNT;
    this->yaw = this->actor.world.rot.y;
    func_80834644(play, this);

    SkelAnime_InitLink(play, &this->skelAnime, skelHeader, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType), 9,
                       this->jointTable, this->morphTable, PLAYER_LIMB_MAX);
    this->skelAnime.baseTransl = sSkeletonBaseTransl;
    SkelAnime_InitLink(play, &this->upperSkelAnime, skelHeader, Player_GetIdleAnim(this), 9, this->upperJointTable,
                       this->upperMorphTable, PLAYER_LIMB_MAX);
    this->upperSkelAnime.baseTransl = sSkeletonBaseTransl;

    Effect_Add(play, &this->meleeWeaponEffectIndex, EFFECT_BLURE2, 0, 0, &blureSword);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFeet, this->ageProperties->unk_04);
    this->subCamId = SUBCAM_NONE;
    Collider_InitCylinder(play, &this->cylinder);
    Collider_SetCylinder(play, &this->cylinder, &this->actor, &D_80854624);
    Collider_InitQuad(play, &this->meleeWeaponQuads[0]);
    Collider_SetQuad(play, &this->meleeWeaponQuads[0], &this->actor, &D_80854650);
    Collider_InitQuad(play, &this->meleeWeaponQuads[1]);
    Collider_SetQuad(play, &this->meleeWeaponQuads[1], &this->actor, &D_80854650);
    Collider_InitQuad(play, &this->shieldQuad);
    Collider_SetQuad(play, &this->shieldQuad, &this->actor, &D_808546A0);

    this->ivanDamageMultiplier = 1;
}

static void (*D_80854738[])(PlayState* play, Player* this) = {
    /* 0x0 */ func_80846648,
    /* 0x1 */ func_808467D4, // From time travel
    /* 0x2 */ func_80846660,
    /* 0x3 */ func_808468A8,
    /* 0x4 */ func_808468E8,
    /* 0x5 */ func_808469BC,
    /* 0x6 */ func_80846A68,
    /* 0x7 */ func_80846978,
    /* 0x8 */ func_8083CA54,
    /* 0x9 */ func_8083CA54,
    /* 0xA */ func_8083CA54,
    /* 0xB */ func_8083CA54,
    /* 0xC */ func_8083CA54,
    /* 0xD */ func_8083CA20,
    /* 0xE */ func_8083CA54,
    /* 0xF */ func_8083CA9C,
};

static Vec3f D_80854778 = { 0.0f, 50.0f, 0.0f };

void Player_Init(Actor* thisx, PlayState* play2) {
    Player* this = (Player*)thisx;
    PlayState* play = play2;
    SceneTableEntry* scene = play->loadedScene;
    u32 titleFileSize;
    s32 initMode;
    s32 respawnFlag;
    s32 respawnMode;

    // In ER, once Link has spawned we know the scene has loaded, so we can sanitize the last known entrance type
    if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
        Grotto_SanitizeEntranceType();
    }

    play->shootingGalleryStatus = play->bombchuBowlingStatus = 0;

    play->playerInit = Player_InitCommon;
    play->playerUpdate = Player_UpdateCommon;
    play->isPlayerDroppingFish = Player_IsDroppingFish;
    play->startPlayerFishing = Player_StartFishing;
    play->grabPlayer = func_80852F38;
    play->startPlayerCutscene = Player_TryCsAction;
    play->func_11D54 = func_80853080;
    play->damagePlayer = Player_InflictDamage;
    play->talkWithPlayer = Player_StartTalking;

    thisx->room = -1;
    this->ageProperties = &sAgeProperties[gSaveContext.linkAge];
    this->itemAction = this->heldItemAction = -1;
    this->heldItemId = ITEM_NONE;

    Player_UseItem(play, this, ITEM_NONE);
    Player_SetEquipmentData(play, this);
    this->prevBoots = this->currentBoots;
    //keep masks thru loading zones
    if (CVarGetInteger(CVAR_ENHANCEMENT("PersistentMasks"), 0)) {
        if (INV_CONTENT(ITEM_TRADE_CHILD) == ITEM_SOLD_OUT) {
            gSaveContext.maskMemory = PLAYER_MASK_NONE;
        }
        this->currentMask = gSaveContext.maskMemory;
    }
    Player_InitCommon(this, play, gPlayerSkelHeaders[((void)0, gSaveContext.linkAge)]);
    // `giObjectSegment` is used for both "get item" objects and title cards. The maximum size for
    // get item objects is 0x2000 (see the assert in func_8083AE40), and the maximum size for
    // title cards is 0x1000 * LANGUAGE_MAX since each title card image includes all languages.
    this->giObjectSegment = (void*)(((uintptr_t)ZELDA_ARENA_MALLOC_DEBUG(0x3008) + 8) & ~0xF);

    respawnFlag = gSaveContext.respawnFlag;

    if (respawnFlag != 0) {
        if (respawnFlag == -3) {
            thisx->params = gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams;
        } else {
            if ((respawnFlag == 1) || (respawnFlag == -1)) {
                this->unk_A86 = -2;
            }

            if (respawnFlag < 0) {
                respawnMode = 0;
            } else {
                respawnMode = respawnFlag - 1;
                Math_Vec3f_Copy(&thisx->world.pos, &gSaveContext.respawn[respawnFlag - 1].pos);
                Math_Vec3f_Copy(&thisx->home.pos, &thisx->world.pos);
                Math_Vec3f_Copy(&thisx->prevPos, &thisx->world.pos);
                this->fallStartHeight = thisx->world.pos.y;
                this->yaw = thisx->shape.rot.y = gSaveContext.respawn[respawnMode].yaw;
                thisx->params = gSaveContext.respawn[respawnMode].playerParams;
            }

            play->actorCtx.flags.tempSwch = gSaveContext.respawn[respawnMode].tempSwchFlags & 0xFFFFFF;
            play->actorCtx.flags.tempCollect = gSaveContext.respawn[respawnMode].tempCollectFlags;
        }
    }

    if ((respawnFlag == 0) || (respawnFlag < -1)) {
        titleFileSize = scene->titleFile.vromEnd - scene->titleFile.vromStart;
        if (GameInteractor_Should(VB_SHOW_TITLE_CARD, gSaveContext.showTitleCard)) {
            if ((gSaveContext.sceneSetupIndex < 4) &&
                (gEntranceTable[((void)0, gSaveContext.entranceIndex) + ((void)0, gSaveContext.sceneSetupIndex)].field &
                 ENTRANCE_INFO_DISPLAY_TITLE_CARD_FLAG) &&
                ((play->sceneNum != SCENE_DODONGOS_CAVERN) || (Flags_GetEventChkInf(EVENTCHKINF_ENTERED_DODONGOS_CAVERN))) &&
                ((play->sceneNum != SCENE_BOMBCHU_SHOP) || (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)))) {
                TitleCard_InitPlaceName(play, &play->actorCtx.titleCtx, this->giObjectSegment, 160, 120, 144,
                                        24, 20);
            }
        }
        gSaveContext.showTitleCard = true;
    }

    if (func_80845C68(play, (respawnFlag == 2) ? 1 : 0) == 0) {
        gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = (thisx->params & 0xFF) | 0xD00;
    }

    gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 1;

    if (play->sceneNum <= SCENE_INSIDE_GANONS_CASTLE_COLLAPSE) {
        gSaveContext.infTable[26] |= gBitFlags[play->sceneNum];
    }

    initMode = (thisx->params & 0xF00) >> 8;
    if ((initMode == 5) || (initMode == 6)) {
        if (gSaveContext.cutsceneIndex >= 0xFFF0) {
            initMode = 13;
        }
    }

    D_80854738[initMode](play, this);

    if (initMode != 0) {
        if ((gSaveContext.gameMode == 0) || (gSaveContext.gameMode == 3)) {
            this->naviActor = Player_SpawnFairy(play, this, &thisx->world.pos, &D_80854778, FAIRY_NAVI);
            if (gSaveContext.dogParams != 0) {
                gSaveContext.dogParams |= 0x8000;
            }
        }
    }

    if (gSaveContext.nayrusLoveTimer != 0) {
        gSaveContext.magicState = MAGIC_STATE_METER_FLASH_1;
        Player_SpawnMagicSpell(play, this, 1);
        this->stateFlags3 &= ~PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }

    if (gSaveContext.entranceSound != 0) {
        Audio_PlayActorSound2(&this->actor, ((void)0, gSaveContext.entranceSound));
        gSaveContext.entranceSound = 0;
    }

    Map_SavePlayerInitialInfo(play);
    MREG(64) = 0;
}

void Player_ApproachZeroBinang(s16* pValue) {
    s16 step;

    step = ABS(*pValue) * 100.0f / 1000.0f;
    step = CLAMP(step, 400, 4000);

    Math_ScaledStepToS(pValue, 0, step);
}

void func_80847298(Player* this) {
    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_FOCUS_Y)) {
        s16 diff = this->actor.focus.rot.y - this->actor.shape.rot.y;

        Player_ApproachZeroBinang(&diff);
        this->actor.focus.rot.y = this->actor.shape.rot.y + diff;
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_FOCUS_X)) {
        Player_ApproachZeroBinang(&this->actor.focus.rot.x);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_HEAD_X)) {
        Player_ApproachZeroBinang(&this->headLimbRot.x);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_UPPER_X)) {
        Player_ApproachZeroBinang(&this->upperLimbRot.x);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_FOCUS_Z)) {
        Player_ApproachZeroBinang(&this->actor.focus.rot.z);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_HEAD_Y)) {
        Player_ApproachZeroBinang(&this->headLimbRot.y);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_HEAD_Z)) {
        Player_ApproachZeroBinang(&this->headLimbRot.z);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_UPPER_Y)) {
        if (this->upperLimbYawSecondary != 0) {
            Player_ApproachZeroBinang(&this->upperLimbYawSecondary);
        } else {
            Player_ApproachZeroBinang(&this->upperLimbRot.y);
        }
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_UPPER_Z)) {
        Player_ApproachZeroBinang(&this->upperLimbRot.z);
    }

    this->unk_6AE_rotFlags = 0;
}

static f32 D_80854784[] = { 120.0f, 240.0f, 360.0f };

/**
 * Updates the two main interface elements that player is responsible for:
 *     - Do Action label on the A button
 *     - Navi C-up icon for hints
 */
void Player_UpdateInterface(PlayState* play, Player* this) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) && (this->actor.category == ACTORCAT_PLAYER)) {
        Actor* heldActor = this->heldActor;
        Actor* interactRangeActor = this->interactRangeActor;
        s32 sp24;
        s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];
        s32 sp1C = func_808332B8(this);
        s32 doAction = DO_ACTION_NONE;

        if (!Player_InBlockingCsMode(play, this)) {
            if (this->stateFlags1 & PLAYER_STATE1_FIRST_PERSON) {
                doAction = DO_ACTION_RETURN;
            } else if ((this->heldItemAction == PLAYER_IA_FISHING_POLE) && (this->unk_860 != 0)) {
                if (this->unk_860 == 2) {
                    doAction = DO_ACTION_REEL;
                }
            } else if ((Player_Action_8084E3C4 != this->actionFunc) && !(this->stateFlags2 & PLAYER_STATE2_CRAWLING)) {
                if ((this->doorType != PLAYER_DOORTYPE_NONE) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) ||
                     ((heldActor != NULL) && (heldActor->id == ACTOR_EN_RU1)))) {
                    doAction = DO_ACTION_OPEN;
                } else if ((!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) || (heldActor == NULL)) &&
                           (interactRangeActor != NULL) &&
                           ((!sp1C && (this->getItemId == GI_NONE)) ||
                            (this->getItemId < 0 && !(this->stateFlags1 & PLAYER_STATE1_IN_WATER)))) {
                    if (this->getItemId < 0) {
                        doAction = DO_ACTION_OPEN;
                    } else if ((interactRangeActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT) {
                        doAction = DO_ACTION_DROP;
                    } else {
                        doAction = DO_ACTION_GRAB;
                    }
                } else if (!sp1C && (this->stateFlags2 & PLAYER_STATE2_DO_ACTION_GRAB)) {
                    doAction = DO_ACTION_GRAB;
                } else if ((this->stateFlags2 & PLAYER_STATE2_DO_ACTION_CLIMB) ||
                           (!(this->stateFlags1 & PLAYER_STATE1_ON_HORSE) && (this->rideActor != NULL))) {
                    doAction = DO_ACTION_CLIMB;
                } else if ((this->stateFlags1 & PLAYER_STATE1_ON_HORSE) && !EN_HORSE_CHECK_4((EnHorse*)this->rideActor) &&
                           (Player_Action_8084D3E4 != this->actionFunc)) {
                    if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
                        if (this->talkActor->category == ACTORCAT_NPC) {
                            doAction = DO_ACTION_SPEAK;
                        } else {
                            doAction = DO_ACTION_CHECK;
                        }
                    } else if (!func_8002DD78(this) && !(this->stateFlags1 & PLAYER_STATE1_FIRST_PERSON)) {
                        doAction = DO_ACTION_FASTER;
                    }
                } else if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
                    if (this->talkActor->category == ACTORCAT_NPC) {
                        doAction = DO_ACTION_SPEAK;
                    } else {
                        doAction = DO_ACTION_CHECK;
                    }
                } else if ((this->stateFlags1 & (PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LADDER)) ||
                           ((this->stateFlags1 & PLAYER_STATE1_ON_HORSE) && (this->stateFlags2 & PLAYER_STATE2_DO_ACTION_DOWN))) {
                    doAction = DO_ACTION_DOWN;
                } else if (this->stateFlags2 & PLAYER_STATE2_DO_ACTION_ENTER) {
                    doAction = DO_ACTION_ENTER;
                } else if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->getItemId == GI_NONE) &&
                           (heldActor != NULL)) {
                    if ((this->actor.bgCheckFlags & 1) || (heldActor->id == ACTOR_EN_NIW)) {
                        if (func_8083EAF0(this, heldActor) == 0) {
                            doAction = DO_ACTION_DROP;
                        } else {
                            doAction = DO_ACTION_THROW;
                        }
                    }
                } else if (!(this->stateFlags1 & PLAYER_STATE1_IN_WATER) && func_8083A0D4(this) &&
                           (this->getItemId < GI_MAX)) {
                    doAction = DO_ACTION_GRAB;
                } else if (this->stateFlags2 & PLAYER_STATE2_DIVING) {
                    static u8 sDiveNumberDoActions[] = { DO_ACTION_1, DO_ACTION_2, DO_ACTION_3, DO_ACTION_4,
                                                         DO_ACTION_5, DO_ACTION_6, DO_ACTION_7, DO_ACTION_8 };

                    sp24 = (D_80854784[CUR_UPG_VALUE(UPG_SCALE)] - this->actor.yDistToWater) / 40.0f;
                    sp24 = CLAMP(sp24, 0, 7);
                    doAction = sDiveNumberDoActions[sp24];
                } else if (sp1C && !(this->stateFlags2 & PLAYER_STATE2_UNDERWATER)) {
                    doAction = DO_ACTION_DIVE;
                } else if (!sp1C && (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) || Player_IsZTargeting(this) ||
                                     !Player_IsChildWithHylianShield(this))) {
                    if ((!(this->stateFlags1 & PLAYER_STATE1_CLIMBING_LEDGE) &&
                         (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) &&
                         (Player_CheckHostileLockOn(this) ||
                          ((sFloorType != 7) && (Player_FriendlyLockOnOrParallel(this) ||
                                                 ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) &&
                                                  !(this->stateFlags1 & PLAYER_STATE1_SHIELDING) &&
                                                  (controlStickDirection == PLAYER_STICK_DIR_FORWARD))))))) {
                        doAction = DO_ACTION_ATTACK;
                    } else if ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) &&
                               Player_IsZTargeting(this) && (controlStickDirection >= PLAYER_STICK_DIR_LEFT)) {
                        doAction = DO_ACTION_JUMP;
                    } else if ((this->heldItemAction >= PLAYER_IA_SWORD_MASTER) ||
                               ((this->stateFlags2 & PLAYER_STATE2_NAVI_ACTIVE) &&
                                (play->actorCtx.targetCtx.arrowPointedActor == NULL))) {
                        doAction = DO_ACTION_PUTAWAY;
                    }
                }
            }
        }

        if (doAction != DO_ACTION_PUTAWAY) {
            this->putAwayCooldownTimer = 20;
        } else if (this->putAwayCooldownTimer != 0) {
            if (CVarGetInteger(CVAR_ENHANCEMENT("InstantPutaway"), 0) != 0) {
                this->putAwayCooldownTimer = 0;
            } else {
                // Replace the "Put Away" Do Action label with a blank label while
                // the cooldown timer is counting down
                doAction = DO_ACTION_NONE;
                this->putAwayCooldownTimer--;
            }
        }

        Interface_SetDoAction(play, doAction);

        if (this->stateFlags2 & PLAYER_STATE2_NAVI_ALERT) {
            if (this->focusActor != NULL) {
                Interface_SetNaviCall(play, 0x1E);
            } else {
                Interface_SetNaviCall(play, 0x1D);
            }
            Interface_SetNaviCall(play, 0x1E);
        } else {
            Interface_SetNaviCall(play, 0x1F);
        }
    }
}

/**
 * Updates state related to the Hover Boots.
 * Handles a special case where the Hover Boots are able to activate when standing on certain floor types even if the
 * player is standing on the ground.
 *
 * If the player is not on the ground, regardless of the usage of the Hover Boots, various floor related variables are
 * reset.
 *
 * @return true if not on the ground, false otherwise. Note this is independent of the Hover Boots state.
 */
s32 Player_UpdateHoverBoots(Player* this) {
    s32 canHoverOnGround;

    if ((this->currentBoots == PLAYER_BOOTS_HOVER ||
         (CVarGetInteger(CVAR_ENHANCEMENT("IvanCoopModeEnabled"), 0) && this->ivanFloating)) &&
        (this->hoverBootsTimer != 0)) {
        this->hoverBootsTimer--;
    } else {
        this->hoverBootsTimer = 0;
    }

    canHoverOnGround =
        (this->currentBoots == PLAYER_BOOTS_HOVER || (CVarGetInteger(CVAR_ENHANCEMENT("IvanCoopModeEnabled"), 0) && this->ivanFloating)) &&
        ((this->actor.yDistToWater >= 0.0f) || (func_80838144(sFloorType) >= 0) || func_8083816C(sFloorType));

    if (canHoverOnGround && (this->actor.bgCheckFlags & 1) && (this->hoverBootsTimer != 0)) {
        this->actor.bgCheckFlags &= ~1;
    }

    if (this->actor.bgCheckFlags & 1) {
        if (!canHoverOnGround) {
            this->hoverBootsTimer = 19;
        }
        return false;
    } else {
        sFloorType = 0;
        this->floorPitch = this->floorPitchAlt = sFloorShapePitch = 0;

        return true;
    }
}

/**
 * Performs various tasks related to scene collision.
 *
 * This includes:
 * - Update BgCheckInfo, parameters adjusted due to various state flags
 * - Update floor type, floor property and floor sfx offset
 * - Update conveyor, reverb and light settings according to the current floor poly
 * - Handle exits and voids
 * - Update information relating to the "interact wall"
 * - Update information for ledge climbing
 * - Update hover boots
 * - Calculate floor poly angles
 *
 */
void Player_ProcessSceneCollision(PlayState* play, Player* this) {
    static Vec3f sInteractWallCheckOffset = { 0.0f, 18.0f, 0.0f };
    u8 nextLedgeClimbType = PLAYER_LEDGE_CLIMB_NONE;
    CollisionPoly* floorPoly;
    Vec3f unusedWorldPos;
    f32 float0; // multi-purpose variable, see define names (fake match?)
    f32 float1; // multi-purpose variable, see define names (fake match?)
    f32 ceilingCheckHeight;
    u32 flags;

    sPrevFloorProperty = this->floorProperty;

#define vWallCheckRadius float0
#define vWallCheckHeight float1

    if (this->stateFlags2 & PLAYER_STATE2_CRAWLING) {
        vWallCheckRadius = 10.0f;
        vWallCheckHeight = 15.0f;
        ceilingCheckHeight = 30.0f;
    } else {
        vWallCheckRadius = this->ageProperties->wallCheckRadius;
        vWallCheckHeight = 26.0f;
        ceilingCheckHeight = this->ageProperties->ceilingCheckHeight;
    }

    if (this->stateFlags1 & (PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_FLOOR_DISABLED)) {
        if (this->stateFlags1 & PLAYER_STATE1_FLOOR_DISABLED) {
            this->actor.bgCheckFlags &= ~1;
            flags = 0x38;
        } else if ((this->stateFlags1 & PLAYER_STATE1_LOADING) && ((this->unk_A84 - (s32)this->actor.world.pos.y) >= 100)) {
            flags = 0x39;
        } else if (!(this->stateFlags1 & PLAYER_STATE1_LOADING) &&
                   ((Player_Action_80845EF8 == this->actionFunc) || (Player_Action_80845CA4 == this->actionFunc))) {
            this->actor.bgCheckFlags &= ~0x208;
            flags = 0x3C;
        } else {
            flags = 0x3F;
        }
    } else {
        flags = 0x3F;
    }

    if (this->stateFlags3 & PLAYER_STATE3_IGNORE_CEILING_FLOOR_WATER) {
        flags &= ~6;
    }

    if (flags & 4) {
        this->stateFlags3 |= PLAYER_STATE3_CHECK_FLOOR_WATER_COLLISION;
    }

    Math_Vec3f_Copy(&unusedWorldPos, &this->actor.world.pos);
    Actor_UpdateBgCheckInfo(play, &this->actor, vWallCheckHeight, vWallCheckRadius, ceilingCheckHeight, flags);

    if (this->actor.bgCheckFlags & 0x10) {
        this->actor.velocity.y = 0.0f;
    }

    sYDistToFloor = this->actor.world.pos.y - this->actor.floorHeight;
    sConveyorSpeed = 0;
    floorPoly = this->actor.floorPoly;

    if (floorPoly != NULL) {
        this->floorProperty = func_80041EA4(&play->colCtx, floorPoly, this->actor.floorBgId);
        this->prevFloorSfxOffset = this->floorSfxOffset;

        if (this->actor.bgCheckFlags & 0x20) {
            if (this->actor.yDistToWater < 20.0f) {
                this->floorSfxOffset = 4;
            } else {
                this->floorSfxOffset = 5;
            }
        } else {
            if (this->stateFlags2 & PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND) {
                this->floorSfxOffset = 1;
            } else {
                this->floorSfxOffset = SurfaceType_GetSfx(&play->colCtx, floorPoly, this->actor.floorBgId);
            }
        }

        if (this->actor.category == ACTORCAT_PLAYER) {
            Audio_SetCodeReverb(SurfaceType_GetEcho(&play->colCtx, floorPoly, this->actor.floorBgId));

            if (this->actor.floorBgId == BGCHECK_SCENE) {
                func_80074CE8(play,
                              SurfaceType_GetLightSettingIndex(&play->colCtx, floorPoly, this->actor.floorBgId));
            } else {
                func_80043508(&play->colCtx, this->actor.floorBgId);
            }
        }

        sConveyorSpeed = SurfaceType_GetConveyorSpeed(&play->colCtx, floorPoly, this->actor.floorBgId);
        if (sConveyorSpeed != 0) {
            sIsFloorConveyor = SurfaceType_IsConveyor(&play->colCtx, floorPoly, this->actor.floorBgId);
            if (((sIsFloorConveyor == 0) && (this->actor.yDistToWater > 20.0f) &&
                 (this->currentBoots != PLAYER_BOOTS_IRON)) ||
                ((sIsFloorConveyor != 0) && (this->actor.bgCheckFlags & 1))) {
                sConveyorYaw = SurfaceType_GetConveyorDirection(&play->colCtx, floorPoly, this->actor.floorBgId) << 10;
            } else {
                sConveyorSpeed = 0;
            }
        }
    }

    Player_HandleExitsAndVoids(play, this, floorPoly, this->actor.floorBgId);

    this->actor.bgCheckFlags &= ~0x200;

    if (this->actor.bgCheckFlags & 8) {
        CollisionPoly* wallPoly;
        s32 wallBgId;
        s16 yawDiff;
        s32 pad;

        sInteractWallCheckOffset.y = 18.0f;
        sInteractWallCheckOffset.z = this->ageProperties->wallCheckRadius + 10.0f;

        if (!(this->stateFlags2 & PLAYER_STATE2_CRAWLING) &&
            Player_PosVsWallLineTest(play, this, &sInteractWallCheckOffset, &wallPoly, &wallBgId, &sInteractWallCheckResult)) {
            this->actor.bgCheckFlags |= 0x200;
            if (this->actor.wallPoly != wallPoly) {
                this->actor.wallPoly = wallPoly;
                this->actor.wallBgId = wallBgId;
                this->actor.wallYaw = Math_Atan2S(wallPoly->normal.z, wallPoly->normal.x);
            }
        }

        yawDiff = this->actor.shape.rot.y - (s16)(this->actor.wallYaw + 0x8000);

        sTouchedWallFlags = func_80041DB8(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId);

        // conflicts arise from these two being enabled at once, and with ClimbEverything on, FixVineFall is redundant anyway
        if (CVarGetInteger(CVAR_ENHANCEMENT("FixVineFall"), 0) && !CVarGetInteger(CVAR_CHEAT("ClimbEverything"), 0)) {
            /* This fixes the "started climbing a wall and then immediately fell off" bug.
            * The main idea is if a climbing wall is detected, double-check that it will
            * still be valid once climbing begins by doing a second raycast with a small
            * margin to make sure it still hits a climbable poly. Then update the flags
            * in sTouchedWallFlags again and proceed as normal.
            */
            if (sTouchedWallFlags & 8) {
                Vec3f checkPosA;
                Vec3f checkPosB;
                f32 yawCos;
                f32 yawSin;
                s32 hitWall;

                /* Angle the raycast slightly out towards the side based on the angle of
                * attack the player takes coming at the climb wall. This is necessary because
                * the player's XZ position actually wobbles very slightly while climbing
                * due to small rounding errors in the sin/cos lookup tables. This wobble
                * can cause wall checks while climbing to be slightly left or right of
                * the wall check to start the climb. By adding this buffer it accounts for
                * any possible wobble. The end result is the player has to be further than
                * some epsilon distance from the edge of the climbing poly to actually
                * start the climb. I divide it by 2 to make that epsilon slightly smaller,
                * mainly for visuals. Using the full yawDiff leaves a noticeable gap on
                * the edges that can't be climbed. But with the half distance it looks like
                * the player is climbing right on the edge, and still works.
                */
                yawCos = Math_CosS(this->actor.wallYaw - (yawDiff / 2) + 0x8000);
                yawSin = Math_SinS(this->actor.wallYaw - (yawDiff / 2) + 0x8000);
                checkPosA.x = this->actor.world.pos.x + (-20.0f * yawSin);
                checkPosA.z = this->actor.world.pos.z + (-20.0f * yawCos);
                checkPosB.x = this->actor.world.pos.x + (50.0f * yawSin);
                checkPosB.z = this->actor.world.pos.z + (50.0f * yawCos);
                checkPosB.y = checkPosA.y = this->actor.world.pos.y + 26.0f;

                hitWall = BgCheck_EntityLineTest1(&play->colCtx, &checkPosA, &checkPosB,
                    &sInteractWallCheckResult, &wallPoly, true, false, false, true, &wallBgId);

                if (hitWall) {
                    this->actor.wallPoly = wallPoly;
                    this->actor.wallBgId = wallBgId;
                    this->actor.wallYaw = Math_Atan2S(wallPoly->normal.z, wallPoly->normal.x);
                    yawDiff = this->actor.shape.rot.y - (s16)(this->actor.wallYaw + 0x8000);

                    sTouchedWallFlags = func_80041DB8(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId);
                }
            }
        }

        sShapeYawToTouchedWall = ABS(yawDiff);

        yawDiff = this->yaw - (s16)(this->actor.wallYaw + 0x8000);
        sWorldYawToTouchedWall = ABS(yawDiff);

#define vSpeedScale float0
#define vSpeedLimit float1

        vSpeedScale = sWorldYawToTouchedWall * 0.00008f;

        if (!(this->actor.bgCheckFlags & 1) || vSpeedScale >= 1.0f) {
            this->unk_880 = R_RUN_SPEED_LIMIT / 100.0f;
        } else {
            vSpeedLimit = (R_RUN_SPEED_LIMIT / 100.0f * vSpeedScale);
            this->unk_880 = vSpeedLimit;

            if (vSpeedLimit < 0.1f) {
                this->unk_880 = 0.1f;
            }
        }

        if ((this->actor.bgCheckFlags & 0x200) && (sShapeYawToTouchedWall < 0x3000)) {
            CollisionPoly* wallPoly = this->actor.wallPoly;

            if (ABS(wallPoly->normal.y) < 600 || (CVarGetInteger(CVAR_CHEAT("ClimbEverything"), 0) != 0)) {
                f32 wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 wallPolyNormalY = COLPOLY_GET_NORMAL(wallPoly->normal.y);
                f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 yDistToLedge;
                CollisionPoly* ledgeFloorPoly;
                CollisionPoly* poly;
                s32 bgId;
                Vec3f ledgeCheckPos;
                f32 ledgePosY;
                f32 ceillingPosY;
                s32 wallYawDiff;

                this->distToInteractWall = Math3D_UDistPlaneToPos(wallPolyNormalX, wallPolyNormalY, wallPolyNormalZ, wallPoly->dist, &this->actor.world.pos);

#define vLedgeCheckOffsetXZ float0

                vLedgeCheckOffsetXZ = this->distToInteractWall + 10.0f;
                ledgeCheckPos.x = this->actor.world.pos.x - (vLedgeCheckOffsetXZ * wallPolyNormalX);
                ledgeCheckPos.z = this->actor.world.pos.z - (vLedgeCheckOffsetXZ * wallPolyNormalZ);
                ledgeCheckPos.y = this->actor.world.pos.y + this->ageProperties->unk_0C;

                ledgePosY = BgCheck_EntityRaycastFloor1(&play->colCtx, &ledgeFloorPoly, &ledgeCheckPos);
                yDistToLedge = ledgePosY - this->actor.world.pos.y;
                this->yDistToLedge = yDistToLedge;

                if ((this->yDistToLedge < 18.0f) ||
                    BgCheck_EntityCheckCeiling(&play->colCtx, &ceillingPosY, &this->actor.world.pos,
                                               (ledgePosY - this->actor.world.pos.y) + 20.0f, &poly, &bgId, &this->actor)) {
                    this->yDistToLedge = 399.96002f;
                } else {
                    sInteractWallCheckOffset.y = (ledgePosY + 5.0f) - this->actor.world.pos.y;

                    if (Player_PosVsWallLineTest(play, this, &sInteractWallCheckOffset, &poly, &bgId, &sInteractWallCheckResult) &&
                        (wallYawDiff = this->actor.wallYaw - Math_Atan2S(poly->normal.z, poly->normal.x),
                         ABS(wallYawDiff) < 0x4000) &&
                        !func_80041E18(&play->colCtx, poly, bgId)) {
                        this->yDistToLedge = 399.96002f;
                    } else if (func_80041DE4(&play->colCtx, wallPoly, this->actor.wallBgId) == 0) {
                        if (this->ageProperties->unk_1C <= this->yDistToLedge) {
                            if (ABS(ledgeFloorPoly->normal.y) > 28000) {
                                if (this->ageProperties->unk_14 <= this->yDistToLedge) {
                                    nextLedgeClimbType = 4;
                                } else if (this->ageProperties->unk_18 <= this->yDistToLedge) {
                                    nextLedgeClimbType = 3;
                                } else {
                                    nextLedgeClimbType = 2;
                                }
                            }
                        } else {
                            nextLedgeClimbType = 1;
                        }
                    }
                }
            }
        }
    } else {
        this->unk_880 = R_RUN_SPEED_LIMIT / 100.0f;
        this->ledgeClimbDelayTimer = 0;
        this->yDistToLedge = 0.0f;
    }

    if (nextLedgeClimbType == this->ledgeClimbType) {
        if ((this->linearVelocity != 0.0f) && (this->ledgeClimbDelayTimer < 100)) {
            this->ledgeClimbDelayTimer++;
        }
    } else {
        this->ledgeClimbType = nextLedgeClimbType;
        this->ledgeClimbDelayTimer = 0;
    }

    if (this->actor.bgCheckFlags & 1) {
        sFloorType = func_80041D4C(&play->colCtx, floorPoly, this->actor.floorBgId);

        if (!Player_UpdateHoverBoots(this)) {
            f32 floorPolyNormalX;
            f32 invFloorPolyNormalY;
            f32 floorPolyNormalZ;
            f32 sin;
            s32 pad2;
            f32 cos;
            s32 pad3;

            if (this->actor.floorBgId != BGCHECK_SCENE) {
                func_800434C8(&play->colCtx, this->actor.floorBgId);
            }

            floorPolyNormalX = COLPOLY_GET_NORMAL(floorPoly->normal.x);
            invFloorPolyNormalY = 1.0f / COLPOLY_GET_NORMAL(floorPoly->normal.y);
            floorPolyNormalZ = COLPOLY_GET_NORMAL(floorPoly->normal.z);

            sin = Math_SinS(this->yaw);
            cos = Math_CosS(this->yaw);

            this->floorPitch =
                Math_Atan2S(1.0f, (-(floorPolyNormalX * sin) - (floorPolyNormalZ * cos)) * invFloorPolyNormalY);
            this->floorPitchAlt =
                Math_Atan2S(1.0f, (-(floorPolyNormalX * cos) - (floorPolyNormalZ * sin)) * invFloorPolyNormalY);

            sin = Math_SinS(this->actor.shape.rot.y);
            cos = Math_CosS(this->actor.shape.rot.y);

            sFloorShapePitch = Math_Atan2S(1.0f, (-(floorPolyNormalX * sin) - (floorPolyNormalZ * cos)) * invFloorPolyNormalY);

            Player_HandleSlopes(play, this, floorPoly);
        }
    } else {
        Player_UpdateHoverBoots(this);
    }

    if (this->prevFloorType == sFloorType) {
        this->floorTypeTimer++;
    } else {
        this->prevFloorType = sFloorType;
        this->floorTypeTimer = 0;
    }
}

void Player_UpdateCamAndSeqModes(PlayState* play, Player* this) {
    u8 seqMode;
    s32 pad;
    Actor* focusActor;
    s32 camMode;

    if (this->actor.category == ACTORCAT_PLAYER) {
        seqMode = SEQ_MODE_DEFAULT;

        if (this->csAction != 0) {
            Camera_ChangeMode(Play_GetCamera(play, 0), CAM_MODE_NORMAL);
        } else if (!(this->stateFlags1 & PLAYER_STATE1_FIRST_PERSON)) {
            if ((this->actor.parent != NULL) && (this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT)) {
                camMode = CAM_MODE_HOOKSHOT;
                Camera_SetParam(Play_GetCamera(play, 0), 8, this->actor.parent);
            } else if (Player_Action_8084377C == this->actionFunc) {
                camMode = CAM_MODE_STILL;
            } else if (this->stateFlags2 & PLAYER_STATE2_GRABBING_DYNAPOLY) {
                camMode = CAM_MODE_PUSHPULL;
            } else if ((focusActor = this->focusActor) != NULL) {
                if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_PLAYER_TALKED_TO)) {
                    camMode = CAM_MODE_TALK;
                } else if (this->stateFlags1 & PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS) {
                    if (this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN) {
                        camMode = CAM_MODE_FOLLOWBOOMERANG;
                    } else {
                        camMode = CAM_MODE_FOLLOWTARGET;
                    }
                } else {
                    camMode = CAM_MODE_BATTLE;
                }
                Camera_SetParam(Play_GetCamera(play, 0), 8, focusActor);
            } else if (this->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) {
                camMode = CAM_MODE_CHARGE;
            } else if (this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN) {
                camMode = CAM_MODE_FOLLOWBOOMERANG;
                Camera_SetParam(Play_GetCamera(play, 0), 8, this->boomerangActor);
            } else if (this->stateFlags1 & (PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE)) {
                if (Player_FriendlyLockOnOrParallel(this)) {
                    camMode = CAM_MODE_HANGZ;
                } else {
                    camMode = CAM_MODE_HANG;
                }
            } else if (this->stateFlags1 & (PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE)) {
                if (func_8002DD78(this) || func_808334B4(this)) {
                    camMode = CAM_MODE_BOWARROWZ;
                } else if (this->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER) {
                    camMode = CAM_MODE_CLIMBZ;
                } else {
                    camMode = CAM_MODE_TARGET;
                }
            } else if (this->stateFlags1 & (PLAYER_STATE1_JUMPING | PLAYER_STATE1_CLIMBING_LADDER)) {
                if ((Player_Action_80845668 == this->actionFunc) || (this->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER)) {
                    camMode = CAM_MODE_CLIMB;
                } else {
                    camMode = CAM_MODE_JUMP;
                }
            } else if (this->stateFlags1 & PLAYER_STATE1_FREEFALL) {
                camMode = CAM_MODE_FREEFALL;
            } else if ((this->meleeWeaponState != 0) && (this->meleeWeaponAnimation >= PLAYER_MWA_FORWARD_SLASH_1H) &&
                       (this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H)) {
                camMode = CAM_MODE_STILL;
            } else {
                camMode = CAM_MODE_NORMAL;
                if ((this->linearVelocity == 0.0f) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_ON_HORSE) || (this->rideActor->speedXZ == 0.0f))) {
                    // not moving
                    seqMode = SEQ_MODE_STILL;
                }
            }

            Camera_ChangeMode(Play_GetCamera(play, 0), camMode);
        } else {
            // First person mode
            seqMode = SEQ_MODE_STILL;
        }

        if (play->actorCtx.targetCtx.bgmEnemy != NULL && !CVarGetInteger(CVAR_AUDIO("EnemyBGMDisable"), 0)) {
            seqMode = SEQ_MODE_ENEMY;
            Audio_SetBgmEnemyVolume(sqrtf(play->actorCtx.targetCtx.bgmEnemy->xyzDistToPlayerSq));
        }

        if (play->sceneNum != SCENE_FISHING_POND) {
            Audio_SetSequenceMode(seqMode);
        }
    }
}

static Vec3f D_808547A4 = { 0.0f, 0.5f, 0.0f };
static Vec3f D_808547B0 = { 0.0f, 0.5f, 0.0f };

static Color_RGBA8 D_808547BC = { 255, 255, 100, 255 };
static Color_RGBA8 D_808547C0 = { 255, 50, 0, 0 };

void Player_UpdateBurningDekuStick(PlayState* play, Player* this) {
    f32 temp;

    if (GameInteractor_Should(VB_DEKU_STICK_BURN_OUT, this->unk_85C == 0.0f)) {
        Player_UseItem(play, this, ITEM_NONE);
        return;
    }

    temp = 1.0f;
    uint8_t vanillaShouldBurnOutCondition = DECR(this->unk_860) == 0;
    if (GameInteractor_Should(VB_DEKU_STICK_BURN_OUT, vanillaShouldBurnOutCondition)) {
        Inventory_ChangeAmmo(ITEM_STICK, -1);
        this->unk_860 = 1;
        temp = 0.0f;
        this->unk_85C = temp;
    } else if (this->unk_860 > 200) {
        temp = (210 - this->unk_860) / 10.0f;
    } else if (GameInteractor_Should(VB_DEKU_STICK_BURN_DOWN, this->unk_860 < 20)) {
        temp = this->unk_860 / 20.0f;
        this->unk_85C = temp;
    }

    func_8002836C(play, &this->meleeWeaponInfo[0].tip, &D_808547A4, &D_808547B0, &D_808547BC, &D_808547C0,
                  temp * 200.0f, 0, 8);
}

void Player_UpdateBodyShock(PlayState* play, Player* this) {
    Vec3f shockPos;
    Vec3f* randBodyPart;
    s32 shockScale;

    this->bodyShockTimer--;
    this->unk_892 += this->bodyShockTimer;

    if (this->unk_892 > 20) {
        shockScale = this->bodyShockTimer * 2;
        this->unk_892 -= 20;

        if (shockScale > 40) {
            shockScale = 40;
        }

        randBodyPart = this->bodyPartsPos + (s32)Rand_ZeroFloat(ARRAY_COUNT(this->bodyPartsPos) - 0.1f);
        shockPos.x = (Rand_CenteredFloat(5.0f) + randBodyPart->x) - this->actor.world.pos.x;
        shockPos.y = (Rand_CenteredFloat(5.0f) + randBodyPart->y) - this->actor.world.pos.y;
        shockPos.z = (Rand_CenteredFloat(5.0f) + randBodyPart->z) - this->actor.world.pos.z;

        EffectSsFhgFlash_SpawnShock(play, &this->actor, &shockPos, shockScale, FHGFLASH_SHOCK_PLAYER);
        func_8002F8F0(&this->actor, NA_SE_PL_SPARK - SFX_FLAG);
    }
}

void Player_UpdateBodyBurn(PlayState* play, Player* this) {
    s32 spawnedFlame;
    u8* timerPtr;
    s32 timerStep;
    f32 flameScale;
    f32 flameIntensity;
    s32 dmgCooldown;
    s32 i;
    s32 sp58;
    s32 sp54;

    if (this->currentTunic == PLAYER_TUNIC_GORON || CVarGetInteger(CVAR_CHEAT("SuperTunic"), 0) != 0) {
        sp54 = 20;
    } else {
        sp54 = (s32)(this->linearVelocity * 0.4f) + 1;
    }

    if (this->stateFlags2 & PLAYER_STATE2_FOOTSTEP) {
        sp58 = 100;
    } else {
        sp58 = 0;
    }

    spawnedFlame = false;
    timerPtr = this->bodyFlameTimers;

    func_8083819C(this, play);

    for (i = 0; i < PLAYER_BODYPART_MAX; i++, timerPtr++) {
        timerStep = sp58 + sp54;

        if (*timerPtr <= timerStep) {
            *timerPtr = 0;
        } else {
            spawnedFlame = true;
            *timerPtr -= timerStep;

            if (*timerPtr > 20.0f) {
                flameIntensity = (*timerPtr - 20.0f) * 0.01f;
                flameScale = CLAMP(flameIntensity, 0.19999999f, 0.2f);
            } else {
                flameScale = *timerPtr * 0.01f;
            }

            flameIntensity = (*timerPtr - 25.0f) * 0.02f;
            flameIntensity = CLAMP(flameIntensity, 0.0f, 1.0f);
            EffectSsFireTail_SpawnFlameOnPlayer(play, flameScale, i, flameIntensity);
        }
    }

    if (spawnedFlame) {
        Player_PlaySfx(this, NA_SE_EV_TORCH - SFX_FLAG);

        if (play->sceneNum == SCENE_SPIRIT_TEMPLE_BOSS) {
            dmgCooldown = 0;
        } else {
            dmgCooldown = 7;
        }

        if ((dmgCooldown & play->gameplayFrames) == 0) {
            Player_InflictDamage(play, -1);
        }
    } else {
        this->bodyIsBurning = false;
    }
}

void Player_DetectRumbleSecrets(Player* this) {
    if (CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY)) {
        f32 temp = 200000.0f - (this->closestSecretDistSq * 5.0f);

        if (temp < 0.0f) {
            temp = 0.0f;
        }

        this->unk_6A0 += temp;

        /*Prevent it on horse, while jumping and on title screen.
        If you fly around no stone of agony for you! */
        Color_RGB8 stoneOfAgonyColor = { 255, 255, 255 };
        if (CVarGetInteger(CVAR_COSMETIC("HUD.StoneOfAgony.Changed"), 0)) {
            stoneOfAgonyColor = CVarGetColor24(CVAR_COSMETIC("HUD.StoneOfAgony.Value"), stoneOfAgonyColor);
        }
        if (CVarGetInteger(CVAR_ENHANCEMENT("VisualAgony"), 0) && !this->stateFlags1 && !GameInteractor_NoUIActive()) {
            s16 Top_Margins = (CVarGetInteger(CVAR_COSMETIC("HUD.Margin.T"), 0) * -1);
            s16 Left_Margins = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.L"), 0);
            s16 Right_Margins = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.R"), 0);
            s16 X_Margins_VSOA;
            s16 Y_Margins_VSOA;
            if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.UseMargins"), 0) != 0) {
                if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) == 0) {
                    X_Margins_VSOA = Left_Margins;
                };
                Y_Margins_VSOA = Top_Margins;
            } else {
                X_Margins_VSOA = 0;
                Y_Margins_VSOA = 0;
            }
            s16 PosX_VSOA_ori = OTRGetRectDimensionFromLeftEdge(26) + X_Margins_VSOA;
            s16 PosY_VSOA_ori = 60 + Y_Margins_VSOA;
            s16 PosX_VSOA;
            s16 PosY_VSOA;
            if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) != 0) {
                PosY_VSOA = CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosY"), 0) + Y_Margins_VSOA;
                if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) == 1) { // Anchor Left
                    if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.UseMargins"), 0) != 0) {
                        X_Margins_VSOA = Left_Margins;
                    };
                    PosX_VSOA = OTRGetDimensionFromLeftEdge(CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosX"), 0) + X_Margins_VSOA);
                } else if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) == 2) { // Anchor Right
                    if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.UseMargins"), 0) != 0) {
                        X_Margins_VSOA = Right_Margins;
                    };
                    PosX_VSOA = OTRGetDimensionFromRightEdge(CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosX"), 0) + X_Margins_VSOA);
                } else if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) == 3) { // Anchor None
                    PosX_VSOA = CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosX"), 0);
                } else if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) == 4) { // Hidden
                    PosX_VSOA = -9999;
                }
            } else {
                PosY_VSOA = PosY_VSOA_ori;
                PosX_VSOA = PosX_VSOA_ori;
            }

            int rectLeft = PosX_VSOA; // Left X Pos
            int rectTop = PosY_VSOA;  // Top Y Pos
            int rectWidth = 24;       // Texture Width
            int rectHeight = 24;      // Texture Heigh
            int DefaultIconA = 50;    // Default icon alpha (55 on 255)

            OPEN_DISPS(gPlayState->state.gfxCtx);
            gDPPipeSync(OVERLAY_DISP++);

            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b, DefaultIconA);

            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            if (this->unk_6A0 > 4000000.0f) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b, 255);
            } else {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b, DefaultIconA);
            }
            if (temp == 0 || temp <= 0.1f) {
                /*Fail check, it is used to draw off the icon when
                link is standing out range but do not refresh unk_6A0.
                Also used to make a default value in my case.*/
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b, DefaultIconA);
            }
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
            gDPSetOtherMode(OVERLAY_DISP++,
                            G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_IA16 | G_TL_TILE |
                                G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                            G_AC_NONE | G_ZS_PRIM | G_RM_XLU_SURF | G_RM_XLU_SURF2);
            gDPLoadTextureBlock(OVERLAY_DISP++, gQuestIconStoneOfAgonyTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 24, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gDPSetOtherMode(OVERLAY_DISP++,
                            G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_IA16 | G_TL_TILE |
                                G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                            G_AC_NONE | G_ZS_PRIM | G_RM_XLU_SURF | G_RM_XLU_SURF2);
            gSPWideTextureRectangle(OVERLAY_DISP++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2,
                                    (rectTop + rectHeight) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
            CLOSE_DISPS(gPlayState->state.gfxCtx);
        }

        if (this->unk_6A0 > 4000000.0f) {
            this->unk_6A0 = 0.0f;
            if (CVarGetInteger(CVAR_ENHANCEMENT("VisualAgony"), 0) && !this->stateFlags1 && !GameInteractor_NoUIActive()) {
                // This audio is placed here and not in previous CVar check to prevent ears ra.. :)
                Audio_PlaySoundGeneral(NA_SE_SY_MESSAGE_WOMAN, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E0);
            }
            Player_RequestRumble(this, 120, 20, 10, 0);
        }
    }
}

static s8 sCueToCsActionMap[PLAYER_CUEID_MAX] = {
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_NONE
    PLAYER_CSACTION_3,    // PLAYER_CUEID_1
    PLAYER_CSACTION_3,    // PLAYER_CUEID_2
    PLAYER_CSACTION_5,    // PLAYER_CUEID_3
    PLAYER_CSACTION_4,    // PLAYER_CUEID_4
    PLAYER_CSACTION_8,    // PLAYER_CUEID_5
    PLAYER_CSACTION_9,    // PLAYER_CUEID_6
    PLAYER_CSACTION_13,   // PLAYER_CUEID_7
    PLAYER_CSACTION_14,   // PLAYER_CUEID_8
    PLAYER_CSACTION_15,   // PLAYER_CUEID_9
    PLAYER_CSACTION_16,   // PLAYER_CUEID_10
    PLAYER_CSACTION_17,   // PLAYER_CUEID_11
    PLAYER_CSACTION_18,   // PLAYER_CUEID_12
    -PLAYER_CSACTION_22,  // PLAYER_CUEID_13
    PLAYER_CSACTION_23,   // PLAYER_CUEID_14
    PLAYER_CSACTION_24,   // PLAYER_CUEID_15
    PLAYER_CSACTION_25,   // PLAYER_CUEID_16
    PLAYER_CSACTION_26,   // PLAYER_CUEID_17
    PLAYER_CSACTION_27,   // PLAYER_CUEID_18
    PLAYER_CSACTION_28,   // PLAYER_CUEID_19
    PLAYER_CSACTION_29,   // PLAYER_CUEID_20
    PLAYER_CSACTION_31,   // PLAYER_CUEID_21
    PLAYER_CSACTION_32,   // PLAYER_CUEID_22
    PLAYER_CSACTION_33,   // PLAYER_CUEID_23
    PLAYER_CSACTION_34,   // PLAYER_CUEID_24
    -PLAYER_CSACTION_35,  // PLAYER_CUEID_25
    PLAYER_CSACTION_30,   // PLAYER_CUEID_26
    PLAYER_CSACTION_36,   // PLAYER_CUEID_27
    PLAYER_CSACTION_38,   // PLAYER_CUEID_28
    -PLAYER_CSACTION_39,  // PLAYER_CUEID_29
    -PLAYER_CSACTION_40,  // PLAYER_CUEID_30
    -PLAYER_CSACTION_41,  // PLAYER_CUEID_31
    PLAYER_CSACTION_42,   // PLAYER_CUEID_32
    PLAYER_CSACTION_43,   // PLAYER_CUEID_33
    PLAYER_CSACTION_45,   // PLAYER_CUEID_34
    PLAYER_CSACTION_46,   // PLAYER_CUEID_35
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_36
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_37
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_38
    PLAYER_CSACTION_67,   // PLAYER_CUEID_39
    PLAYER_CSACTION_48,   // PLAYER_CUEID_40
    PLAYER_CSACTION_47,   // PLAYER_CUEID_41
    -PLAYER_CSACTION_50,  // PLAYER_CUEID_42
    PLAYER_CSACTION_51,   // PLAYER_CUEID_43
    -PLAYER_CSACTION_52,  // PLAYER_CUEID_44
    -PLAYER_CSACTION_53,  // PLAYER_CUEID_45
    PLAYER_CSACTION_54,   // PLAYER_CUEID_46
    PLAYER_CSACTION_55,   // PLAYER_CUEID_47
    PLAYER_CSACTION_56,   // PLAYER_CUEID_48
    PLAYER_CSACTION_57,   // PLAYER_CUEID_49
    PLAYER_CSACTION_58,   // PLAYER_CUEID_50
    PLAYER_CSACTION_59,   // PLAYER_CUEID_51
    PLAYER_CSACTION_60,   // PLAYER_CUEID_52
    PLAYER_CSACTION_61,   // PLAYER_CUEID_53
    PLAYER_CSACTION_62,   // PLAYER_CUEID_54
    PLAYER_CSACTION_63,   // PLAYER_CUEID_55
    PLAYER_CSACTION_64,   // PLAYER_CUEID_56
    -PLAYER_CSACTION_65,  // PLAYER_CUEID_57
    -PLAYER_CSACTION_66,  // PLAYER_CUEID_58
    PLAYER_CSACTION_68,   // PLAYER_CUEID_59
    PLAYER_CSACTION_11,   // PLAYER_CUEID_60
    PLAYER_CSACTION_69,   // PLAYER_CUEID_61
    PLAYER_CSACTION_70,   // PLAYER_CUEID_62
    PLAYER_CSACTION_71,   // PLAYER_CUEID_63
    PLAYER_CSACTION_8,    // PLAYER_CUEID_64
    PLAYER_CSACTION_8,    // PLAYER_CUEID_65
    PLAYER_CSACTION_72,   // PLAYER_CUEID_66
    PLAYER_CSACTION_73,   // PLAYER_CUEID_67
    PLAYER_CSACTION_78,   // PLAYER_CUEID_68
    PLAYER_CSACTION_79,   // PLAYER_CUEID_69
    PLAYER_CSACTION_80,   // PLAYER_CUEID_70
    PLAYER_CSACTION_89,   // PLAYER_CUEID_71
    PLAYER_CSACTION_90,   // PLAYER_CUEID_72
    PLAYER_CSACTION_91,   // PLAYER_CUEID_73
    PLAYER_CSACTION_92,   // PLAYER_CUEID_74
    PLAYER_CSACTION_77,   // PLAYER_CUEID_75
    PLAYER_CSACTION_19,   // PLAYER_CUEID_76
    PLAYER_CSACTION_94,   // PLAYER_CUEID_77
};

static Vec3f D_80854814 = { 0.0f, 0.0f, 200.0f };

static f32 sWaterConveyorSpeeds[] = { 2.0f, 4.0f, 7.0f };
static f32 sFloorConveyorSpeeds[] = { 0.5f, 1.0f, 3.0f };

void Player_UseTunicBoots(Player* this, PlayState* play) {
    // Boots and tunics equip despite state
    if (
        this->stateFlags1 & (PLAYER_STATE1_INPUT_DISABLED | PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD) ||
        this->stateFlags2 & PLAYER_STATE2_OCARINA_PLAYING
    ) {
        return;
    }

    s32 i;
    for (i = 0; i < ARRAY_COUNT(sItemButtons); i++) {
        if (CHECK_BTN_ALL(sControlInput->press.button, sItemButtons[i])) {
            break;
        }
    }
    s32 item = Player_GetItemOnButton(play, i);
    if (item >= ITEM_TUNIC_KOKIRI && item <= ITEM_BOOTS_HOVER) {
        if (item >= ITEM_BOOTS_KOKIRI) {
            u16 bootsValue = item - ITEM_BOOTS_KOKIRI + 1;
            if (CUR_EQUIP_VALUE(EQUIP_TYPE_BOOTS) == bootsValue) {
                Inventory_ChangeEquipment(EQUIP_TYPE_BOOTS, EQUIP_VALUE_BOOTS_KOKIRI);
            } else {
                Inventory_ChangeEquipment(EQUIP_TYPE_BOOTS, bootsValue);
            }
            Player_SetEquipmentData(play, this);
            func_808328EC(this, CUR_EQUIP_VALUE(EQUIP_TYPE_BOOTS) == EQUIP_VALUE_BOOTS_IRON ? NA_SE_PL_WALK_HEAVYBOOTS : NA_SE_PL_CHANGE_ARMS);
        } else {
            u16 tunicValue = item - ITEM_TUNIC_KOKIRI + 1;
            if (CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC) == tunicValue) {
                Inventory_ChangeEquipment(EQUIP_TYPE_TUNIC, EQUIP_VALUE_TUNIC_KOKIRI);
            } else {
                Inventory_ChangeEquipment(EQUIP_TYPE_TUNIC, tunicValue);
            }
            Player_SetEquipmentData(play, this);
            func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
        }
    }
}

void Player_UpdateCommon(Player* this, PlayState* play, Input* input) {
    s32 pad;

    sControlInput = input;

    if (this->unk_A86 < 0) {
        this->unk_A86++;
        if (this->unk_A86 == 0) {
            this->unk_A86 = 1;
            func_80078884(NA_SE_OC_REVENGE);
        }
    }

    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.home.pos);

    if (this->unk_A73 != 0) {
        this->unk_A73--;
    }

    if (this->textboxBtnCooldownTimer != 0) {
        this->textboxBtnCooldownTimer--;
    }

    if (this->unk_A87 != 0) {
        this->unk_A87--;
    }

    if (this->invincibilityTimer < 0) {
        this->invincibilityTimer++;
    } else if (this->invincibilityTimer > 0) {
        this->invincibilityTimer--;
    }

    if (this->unk_890 != 0) {
        this->unk_890--;
    }

    Player_UpdateInterface(play, this);
    Player_UpdateZTargeting(this, play);

    if (this->heldItemAction == PLAYER_IA_DEKU_STICK &&
        GameInteractor_Should(VB_DEKU_STICK_BE_ON_FIRE, this->unk_860 != 0)) {
        Player_UpdateBurningDekuStick(play, this);
    } else if ((this->heldItemAction == PLAYER_IA_FISHING_POLE) && (this->unk_860 < 0)) {
        this->unk_860++;
    }

    if (this->bodyShockTimer != 0) {
        Player_UpdateBodyShock(play, this);
    }

    if (this->bodyIsBurning) {
        Player_UpdateBodyBurn(play, this);
    }

    if ((this->stateFlags3 & PLAYER_STATE3_RESTORE_NAYRUS_LOVE) && (gSaveContext.nayrusLoveTimer != 0) && (gSaveContext.magicState == MAGIC_STATE_IDLE)) {
        gSaveContext.magicState = MAGIC_STATE_METER_FLASH_1;
        Player_SpawnMagicSpell(play, this, 1);
        this->stateFlags3 &= ~PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }

    if (this->stateFlags2 & PLAYER_STATE2_PAUSE_MOST_UPDATING) {
        if (!(this->actor.bgCheckFlags & 1)) {
            Player_ZeroSpeedXZ(this);
            Actor_MoveForward(&this->actor);
        }

        Player_ProcessSceneCollision(play, this);
    } else {
        f32 temp_f0;
        f32 phi_f12;

        if (this->currentBoots != this->prevBoots) {
            if (this->currentBoots == PLAYER_BOOTS_IRON) {
                if (this->stateFlags1 & PLAYER_STATE1_IN_WATER) {
                    func_80832340(play, this);
                    if (this->ageProperties->unk_2C < this->actor.yDistToWater) {
                        this->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
                    }
                }
            } else {
                if (this->stateFlags1 & PLAYER_STATE1_IN_WATER) {
                    if ((this->prevBoots == PLAYER_BOOTS_IRON) || (this->actor.bgCheckFlags & 1)) {
                        func_8083D36C(play, this);
                        this->stateFlags2 &= ~PLAYER_STATE2_UNDERWATER;
                    }
                }
            }

            this->prevBoots = this->currentBoots;
        }

        if ((this->actor.parent == NULL) && (this->stateFlags1 & PLAYER_STATE1_ON_HORSE)) {
            this->actor.parent = this->rideActor;
            func_8083A360(play, this);
            this->stateFlags1 |= PLAYER_STATE1_ON_HORSE;
            Player_AnimPlayOnce(play, this, &gPlayerAnim_link_uma_wait_1);
            Player_StartAnimMovement(play, this, 0x9B);
            this->av2.actionVar2 = 99;
        }

        if (this->unk_844 == 0) {
            this->unk_845 = 0;
        } else if (this->unk_844 < 0) {
            this->unk_844++;
        } else {
            this->unk_844--;
        }

        Math_ScaledStepToS(&this->unk_6C2, 0, 400);
        func_80032CB4(this->unk_3A8, 20, 80, 6);

        this->actor.shape.face = this->unk_3A8[0] + ((play->gameplayFrames & 32) ? 0 : 3);

        if (this->currentMask == PLAYER_MASK_BUNNY) {
            Player_UpdateBunnyEars(this);
        }

        if (func_8002DD6C(this) != 0) {
            func_8084FF7C(this);
        }

        if (!(this->skelAnime.moveFlags & 0x80)) {
            if (((this->actor.bgCheckFlags & 1) && (sFloorType == 5) && (this->currentBoots != PLAYER_BOOTS_IRON)) ||
                ((this->currentBoots == PLAYER_BOOTS_HOVER || GameInteractor_GetSlipperyFloorActive()) &&
                 !(this->stateFlags1 & (PLAYER_STATE1_IN_WATER | PLAYER_STATE1_IN_CUTSCENE)))) {
                f32 sp70 = this->linearVelocity;
                s16 sp6E = this->yaw;
                s16 yawDiff = this->actor.world.rot.y - sp6E;
                s32 pad;

                if ((ABS(yawDiff) > 0x6000) && (this->actor.speedXZ != 0.0f)) {
                    sp70 = 0.0f;
                    sp6E += 0x8000;
                }

                if (Math_StepToF(&this->actor.speedXZ, sp70, 0.35f) && (sp70 == 0.0f)) {
                    this->actor.world.rot.y = this->yaw;
                }

                if (this->linearVelocity != 0.0f) {
                    s32 phi_v0;

                    phi_v0 = (fabsf(this->linearVelocity) * 700.0f) - (fabsf(this->actor.speedXZ) * 100.0f);
                    phi_v0 = CLAMP(phi_v0, 0, 1350);

                    Math_ScaledStepToS(&this->actor.world.rot.y, sp6E, phi_v0);
                }

                if ((this->linearVelocity == 0.0f) && (this->actor.speedXZ != 0.0f)) {
                    func_800F4138(&this->actor.projectedPos, 0xD0, this->actor.speedXZ);
                }
            } else {
                this->actor.speedXZ = this->linearVelocity;
                this->actor.world.rot.y = this->yaw;
            }

            func_8002D868(&this->actor);

            if ((this->pushedSpeed != 0.0f) && !Player_InCsMode(play) &&
                !(this->stateFlags1 & (PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_CLIMBING_LADDER)) &&
                (Player_Action_80845668 != this->actionFunc) && (Player_Action_808507F4 != this->actionFunc)) {
                this->actor.velocity.x += this->pushedSpeed * Math_SinS(this->pushedYaw);
                this->actor.velocity.z += this->pushedSpeed * Math_CosS(this->pushedYaw);
            }

            func_8002D7EC(&this->actor);
            Player_ProcessSceneCollision(play, this);
        } else {
            sFloorType = 0;
            this->floorProperty = 0;

            if (!(this->stateFlags1 & PLAYER_STATE1_LOADING) && (this->stateFlags1 & PLAYER_STATE1_ON_HORSE)) {
                EnHorse* rideActor = (EnHorse*)this->rideActor;
                CollisionPoly* sp5C;
                s32 sp58;
                Vec3f sp4C;

                if (!(rideActor->actor.bgCheckFlags & 1)) {
                    func_808396F4(play, this, &D_80854814, &sp4C, &sp5C, &sp58);
                } else {
                    sp5C = rideActor->actor.floorPoly;
                    sp58 = rideActor->actor.floorBgId;
                }

                if ((sp5C != NULL) && Player_HandleExitsAndVoids(play, this, sp5C, sp58)) {
                    if (DREG(25) != 0) {
                        DREG(25) = 0;
                    } else {
                        AREG(6) = 1;
                    }
                }
            }

            sConveyorSpeed = 0;
            this->pushedSpeed = 0.0f;
        }

        if ((sConveyorSpeed != 0) && (this->currentBoots != PLAYER_BOOTS_IRON)) {
            f32 sp48;

            sConveyorSpeed--;

            if (sIsFloorConveyor == 0) {
                sp48 = sWaterConveyorSpeeds[sConveyorSpeed];

                if (!(this->stateFlags1 & PLAYER_STATE1_IN_WATER)) {
                    sp48 *= 0.25f;
                }
            } else {
                sp48 = sFloorConveyorSpeeds[sConveyorSpeed];
            }

            Math_StepToF(&this->pushedSpeed, sp48, sp48 * 0.1f);

            Math_ScaledStepToS(&this->pushedYaw, sConveyorYaw,
                               ((this->stateFlags1 & PLAYER_STATE1_IN_WATER) ? 400.0f : 800.0f) * sp48);
        } else if (this->pushedSpeed != 0.0f) {
            Math_StepToF(&this->pushedSpeed, 0.0f, (this->stateFlags1 & PLAYER_STATE1_IN_WATER) ? 0.5f : 1.0f);
        }

        if (!Player_InBlockingCsMode(play, this) && !(this->stateFlags2 & PLAYER_STATE2_CRAWLING)) {
            func_8083D53C(play, this);

            if ((this->actor.category == ACTORCAT_PLAYER) && (gSaveContext.health == 0)) {
                if (this->stateFlags1 & (PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_CLIMBING_LADDER)) {
                    func_80832440(play, this);
                    func_80837B9C(this, play);
                } else if ((this->actor.bgCheckFlags & 1) || (this->stateFlags1 & PLAYER_STATE1_IN_WATER)) {
                    func_80836448(play, this,
                                  func_808332B8(this)       ? &gPlayerAnim_link_swimer_swim_down
                                  : (this->bodyShockTimer != 0) ? &gPlayerAnim_link_normal_electric_shock_end
                                                            : &gPlayerAnim_link_derth_rebirth);
                }
            } else {
                if ((this->actor.parent == NULL) &&
                    ((play->transitionTrigger == TRANS_TRIGGER_START) || (this->unk_A87 != 0) || !func_808382DC(this, play))) {
                    func_8083AA10(this, play);
                } else {
                    this->fallStartHeight = this->actor.world.pos.y;
                }
                Player_DetectRumbleSecrets(this);
            }
        }

        if ((play->csCtx.state != CS_STATE_IDLE) && (this->csAction != 6) &&
            !(this->stateFlags1 & PLAYER_STATE1_ON_HORSE) && !(this->stateFlags2 & PLAYER_STATE2_GRABBED_BY_ENEMY) &&
            (this->actor.category == ACTORCAT_PLAYER)) {
            CsCmdActorCue* linkActionCsCmd = play->csCtx.linkAction;

            if ((linkActionCsCmd != NULL) && (sCueToCsActionMap[linkActionCsCmd->action] != 0)) {
                Player_SetCsActionWithHaltedActors(play, NULL, 6);
                Player_ZeroSpeedXZ(this);
            } else if ((this->csAction == 0) && !(this->stateFlags2 & PLAYER_STATE2_UNDERWATER) &&
                       (play->csCtx.state != CS_STATE_UNSKIPPABLE_INIT)) {
                Player_SetCsActionWithHaltedActors(play, NULL, 0x31);
                Player_ZeroSpeedXZ(this);
            }
        }

        if (this->csAction != 0) {
            if ((this->csAction != 7) ||
                !(this->stateFlags1 & (PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_DAMAGED))) {
                this->unk_6AD = 3;
            } else if (Player_Action_CsAction != this->actionFunc) {
                func_80852944(play, this, NULL);
            }
        } else {
            this->prevCsAction = 0;
        }

        func_8083D6EC(play, this);

        if ((this->focusActor == NULL) && (this->naviTextId == 0)) {
            this->stateFlags2 &= ~(PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER | PLAYER_STATE2_NAVI_ALERT);
        }

        this->stateFlags1 &= ~(PLAYER_STATE1_SWINGING_BOTTLE | PLAYER_STATE1_READY_TO_FIRE | PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_SHIELDING);
        this->stateFlags2 &= ~(PLAYER_STATE2_DO_ACTION_GRAB | PLAYER_STATE2_DO_ACTION_CLIMB | PLAYER_STATE2_FOOTSTEP | PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS |
                               PLAYER_STATE2_GRABBING_DYNAPOLY | PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND | PLAYER_STATE2_STATIONARY_LADDER | PLAYER_STATE2_FROZEN |
                               PLAYER_STATE2_DO_ACTION_ENTER | PLAYER_STATE2_DO_ACTION_DOWN | PLAYER_STATE2_REFLECTION);
        this->stateFlags3 &= ~PLAYER_STATE3_CHECK_FLOOR_WATER_COLLISION;

        func_80847298(this);
        Player_ProcessControlStick(play, this);

        if (this->stateFlags1 & PLAYER_STATE1_IN_WATER) {
            sWaterSpeedFactor = 0.5f;
        } else {
            sWaterSpeedFactor = 1.0f;
        }

        sInvWaterSpeedFactor = 1.0f / sWaterSpeedFactor;
        sUseHeldItem = sHeldItemButtonIsHeldDown = 0;
        sSavedCurrentMask = this->currentMask;

        if (!(this->stateFlags3 & PLAYER_STATE3_PAUSE_ACTION_FUNC)) {
            this->actionFunc(this, play);
            Player_UseTunicBoots(this, play);
        }

        Player_UpdateCamAndSeqModes(play, this);

        if (this->skelAnime.moveFlags & 8) {
            AnimationContext_SetMoveActor(play, &this->actor, &this->skelAnime,
                                          (this->skelAnime.moveFlags & 4) ? 1.0f : this->ageProperties->unk_08);
        }

        Player_UpdateShapeYaw(this, play);

        if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_PLAYER_TALKED_TO)) {
            this->talkActorDistance = 0.0f;
        } else {
            this->talkActor = NULL;
            this->talkActorDistance = FLT_MAX;
            this->exchangeItemId = EXCH_ITEM_NONE;
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            this->interactRangeActor = NULL;
            this->getItemDirection = 0x6000;
        }

        if (this->actor.parent == NULL) {
            this->rideActor = NULL;
        }

        this->naviTextId = 0;

        if (!(this->stateFlags2 & PLAYER_STATE2_PLAY_FOR_ACTOR)) {
            this->unk_6A8 = NULL;
        }

        this->stateFlags2 &= ~PLAYER_STATE2_NEAR_OCARINA_ACTOR;
        this->closestSecretDistSq = FLT_MAX;

        temp_f0 = this->actor.world.pos.y - this->actor.prevPos.y;

        this->doorType = PLAYER_DOORTYPE_NONE;
        this->knockbackType = 0;
        this->autoLockOnActor = NULL;

        phi_f12 =
            ((this->bodyPartsPos[PLAYER_BODYPART_L_FOOT].y + this->bodyPartsPos[PLAYER_BODYPART_R_FOOT].y) * 0.5f) +
            temp_f0;
        temp_f0 += this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 10.0f;

        this->cylinder.dim.height = temp_f0 - phi_f12;

        if (this->cylinder.dim.height < 0) {
            phi_f12 = temp_f0;
            this->cylinder.dim.height = -this->cylinder.dim.height;
        }

        this->cylinder.dim.yShift = phi_f12 - this->actor.world.pos.y;

        if (this->stateFlags1 & PLAYER_STATE1_SHIELDING) {
            this->cylinder.dim.height = this->cylinder.dim.height * 0.8f;
        }

        Collider_UpdateCylinder(&this->actor, &this->cylinder);

        if (!(this->stateFlags2 & PLAYER_STATE2_FROZEN)) {
            if (!(this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_ON_HORSE))) {
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->cylinder.base);
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_DAMAGED)) && (this->invincibilityTimer <= 0)) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->cylinder.base);

                if (this->invincibilityTimer < 0) {
                    CollisionCheck_SetAT(play, &play->colChkCtx, &this->cylinder.base);
                }
            }
        }

        AnimationContext_SetNextQueue(play);
    }

    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
    Math_Vec3f_Copy(&this->unk_A88, &this->bodyPartsPos[PLAYER_BODYPART_WAIST]);

    if (this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE)) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        this->actor.colChkInfo.mass = 50;
    }

    this->stateFlags3 &= ~PLAYER_STATE3_PAUSE_ACTION_FUNC;

    Collider_ResetCylinderAC(play, &this->cylinder.base);

    Collider_ResetQuadAT(play, &this->meleeWeaponQuads[0].base);
    Collider_ResetQuadAT(play, &this->meleeWeaponQuads[1].base);

    Collider_ResetQuadAC(play, &this->shieldQuad.base);
    Collider_ResetQuadAT(play, &this->shieldQuad.base);
}

static Vec3f D_80854838 = { 0.0f, 0.0f, -30.0f };

s32 Player_UpdateNoclip(Player* this, PlayState* play);

void Player_Update(Actor* thisx, PlayState* play) {
    static Vec3f sDogSpawnPos;
    Player* this = (Player*)thisx;
    s32 dogParams;
    s32 pad;
    Input sp44;
    Actor* dog;

    if (Player_UpdateNoclip(this, play)) {
        if (gSaveContext.dogParams < 0) {
            // Disable object dependency to prevent losing dog in scenes other than market
            if (Object_GetIndex(&play->objectCtx, OBJECT_DOG) < 0 && !CVarGetInteger(CVAR_ENHANCEMENT("DogFollowsEverywhere"), 0)) {
                gSaveContext.dogParams = 0;
            } else {
                gSaveContext.dogParams &= 0x7FFF;
                Player_GetRelativePosition(this, &this->actor.world.pos, &D_80854838, &sDogSpawnPos);
                dogParams = gSaveContext.dogParams;

                dog = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_DOG, sDogSpawnPos.x, sDogSpawnPos.y,
                                  sDogSpawnPos.z, 0, this->actor.shape.rot.y, 0, dogParams | 0x8000, true);
                if (dog != NULL) {
                    // Room -1 allows actor to cross between rooms, similar to Navi
                    dog->room = CVarGetInteger(CVAR_ENHANCEMENT("DogFollowsEverywhere"), 0) ? -1 : 0;
                }
            }
        }

        if ((this->interactRangeActor != NULL) && (this->interactRangeActor->update == NULL)) {
            this->interactRangeActor = NULL;
        }

        if ((this->heldActor != NULL) && (this->heldActor->update == NULL)) {
            Player_DetachHeldActor(play, this);
        }

        if (this->stateFlags1 & (PLAYER_STATE1_INPUT_DISABLED | PLAYER_STATE1_IN_CUTSCENE)) {
            memset(&sp44, 0, sizeof(sp44));
        } else {
            sp44 = play->state.input[0];
            if (this->textboxBtnCooldownTimer != 0) {
                sp44.cur.button &= ~(BTN_A | BTN_B | BTN_CUP);
                sp44.press.button &= ~(BTN_A | BTN_B | BTN_CUP);
            }
        }

        if (CVarGetInteger(CVAR_SETTING("WalkModifier.Enabled"), 0) && CVarGetInteger(CVAR_SETTING("WalkModifier.SpeedToggle"), 0)) {
            if (CHECK_BTN_ALL(sControlInput->press.button, BTN_CUSTOM_MODIFIER1)) {
                gWalkSpeedToggle1 = !gWalkSpeedToggle1;
            }
            if (CHECK_BTN_ALL(sControlInput->press.button, BTN_CUSTOM_MODIFIER2)) {
                gWalkSpeedToggle2 = !gWalkSpeedToggle2;
            }
        }

        Player_UpdateCommon(this, play, &sp44);
    }

    MREG(52) = this->actor.world.pos.x;
    MREG(53) = this->actor.world.pos.y;
    MREG(54) = this->actor.world.pos.z;
    MREG(55) = this->actor.world.rot.y;

    // Make Link normal size when going through doors and crawlspaces and when climbing ladders.
    // Otherwise Link can glitch out, being in unloaded rooms or falling OoB.
    if (this->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER || this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE ||
        this->stateFlags2 & PLAYER_STATE2_CRAWLING) {
        this->actor.scale.x = 0.01f;
        this->actor.scale.y = 0.01f;
        this->actor.scale.z = 0.01f;
    } else {
        switch (GameInteractor_GetLinkSize()) {
            case GI_LINK_SIZE_RESET:
                this->actor.scale.x = 0.01f;
                this->actor.scale.y = 0.01f;
                this->actor.scale.z = 0.01f;
                GameInteractor_SetLinkSize(GI_LINK_SIZE_NORMAL);
                break;
            case GI_LINK_SIZE_GIANT:
                this->actor.scale.x = 0.02f;
                this->actor.scale.y = 0.02f;
                this->actor.scale.z = 0.02f;
                break;
            case GI_LINK_SIZE_MINISH:
                this->actor.scale.x = 0.001f;
                this->actor.scale.y = 0.001f;
                this->actor.scale.z = 0.001f;
                break;
            case GI_LINK_SIZE_PAPER:
                this->actor.scale.x = 0.001f;
                this->actor.scale.y = 0.01f;
                this->actor.scale.z = 0.01f;
                break;
            case GI_LINK_SIZE_SQUISHED:
                this->actor.scale.x = 0.015f;
                this->actor.scale.y = 0.001f;
                this->actor.scale.z = 0.015f;
                break;
            case GI_LINK_SIZE_NORMAL:
            default:
                break;
        }
    }

    // Don't apply gravity when Link is in water, otherwise
    // it makes him sink instead of float.
    if (!(this->stateFlags1 & PLAYER_STATE1_IN_WATER)) {
        switch (GameInteractor_GravityLevel()) {
            case GI_GRAVITY_LEVEL_HEAVY:
                this->actor.gravity = -4.0f;
                break;
            case GI_GRAVITY_LEVEL_LIGHT:
                this->actor.gravity = -0.3f;
                break;
            default:
                break;
        }
    }

    if (GameInteractor_GetRandomWindActive()) {
        Player* player = GET_PLAYER(play);
        player->pushedSpeed = 3.0f;
        // Play fan sound (too annoying)
        //func_8002F974(&player->actor, NA_SE_EV_WIND_TRAP - SFX_FLAG);
    }
    
    GameInteractor_ExecuteOnPlayerUpdate();
}

typedef struct BunnyEarKinematics {
    /* 0x0 */ Vec3s rot;
    /* 0x6 */ Vec3s angVel;
} BunnyEarKinematics; // size = 0xC

static BunnyEarKinematics sBunnyEarKinematics;

static Gfx* sMaskDlists[PLAYER_MASK_MAX - 1] = {
    gLinkChildKeatonMaskDL, gLinkChildSkullMaskDL, gLinkChildSpookyMaskDL, gLinkChildBunnyHoodDL,
    gLinkChildGoronMaskDL,  gLinkChildZoraMaskDL,  gLinkChildGerudoMaskDL, gLinkChildMaskOfTruthDL,
};

static Vec3s D_80854864 = { 0, 0, 0 };

void Player_DrawGameplay(PlayState* play, Player* this, s32 lod, Gfx* cullDList, OverrideLimbDrawOpa overrideLimbDraw) {
    static s32 D_8085486C = 255;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0C, cullDList);
    gSPSegment(POLY_XLU_DISP++, 0x0C, cullDList);

    Player_DrawImpl(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, lod,
                  this->currentTunic, this->currentBoots, this->actor.shape.face, overrideLimbDraw, Player_PostLimbDrawGameplay,
                  this);

    if ((overrideLimbDraw == Player_OverrideLimbDrawGameplayDefault) && (this->currentMask != PLAYER_MASK_NONE)) {
        // Fixes a bug in vanilla where ice traps are rendered extremely large while wearing a bunny hood
        if (CVarGetInteger(CVAR_GENERAL("FixIceTrapWithBunnyHood"), 1)) Matrix_Push();
        Mtx* bunnyEarMtx = Graph_Alloc(play->state.gfxCtx, 2 * sizeof(Mtx));

        if (this->currentMask == PLAYER_MASK_BUNNY) {
            Vec3s earRot;

            FrameInterpolation_RecordActorPosRotMatrix();
            gSPSegment(POLY_OPA_DISP++, 0x0B, bunnyEarMtx);

            // Right ear
            earRot.x = sBunnyEarKinematics.rot.y + 0x3E2;
            earRot.y = sBunnyEarKinematics.rot.z + 0xDBE;
            earRot.z = sBunnyEarKinematics.rot.x - 0x348A;
            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f - CVarGetFloat(CVAR_COSMETIC("BunnyHood.EarLength"), 0.0f), -240.0f - CVarGetFloat(CVAR_COSMETIC("BunnyHood.EarSpread"), 0.0f), &earRot);
            MATRIX_TOMTX(bunnyEarMtx++);

            // Left ear
            earRot.x = sBunnyEarKinematics.rot.y - 0x3E2;
            earRot.y = -0xDBE - sBunnyEarKinematics.rot.z;
            earRot.z = sBunnyEarKinematics.rot.x - 0x348A;
            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f - CVarGetFloat(CVAR_COSMETIC("BunnyHood.EarLength"), 0.0f), 240.0f + CVarGetFloat(CVAR_COSMETIC("BunnyHood.EarSpread"), 0.0f), &earRot);
            MATRIX_TOMTX(bunnyEarMtx);
        }

       
        if (this->currentMask != PLAYER_MASK_BUNNY || !CVarGetInteger(CVAR_ENHANCEMENT("HideBunnyHood"), 0)) {
            gSPDisplayList(POLY_OPA_DISP++, sMaskDlists[this->currentMask - 1]);
        }

        if (CVarGetInteger(CVAR_GENERAL("FixIceTrapWithBunnyHood"), 1)) Matrix_Pop();
    }

    if ((this->currentBoots == PLAYER_BOOTS_HOVER ||
         (CVarGetInteger(CVAR_ENHANCEMENT("IvanCoopModeEnabled"), 0) && this->ivanFloating)) &&
        !(this->actor.bgCheckFlags & 1) &&
        !(this->stateFlags1 & PLAYER_STATE1_ON_HORSE) && (this->hoverBootsTimer != 0)) {
        s32 sp5C;
        s32 hoverBootsTimer = this->hoverBootsTimer;

        if (this->hoverBootsTimer < 19) {
            if (hoverBootsTimer >= 15) {
                D_8085486C = (19 - hoverBootsTimer) * 51.0f;
            } else if (hoverBootsTimer < 19) {
                sp5C = hoverBootsTimer;

                if (sp5C > 9) {
                    sp5C = 9;
                }

                D_8085486C = (-sp5C * 4) + 36;
                D_8085486C = D_8085486C * D_8085486C;
                D_8085486C = (s32)((Math_CosS(D_8085486C) * 100.0f) + 100.0f) + 55.0f;
                D_8085486C = D_8085486C * (sp5C * (1.0f / 9.0f));
            }

            FrameInterpolation_RecordActorPosRotMatrix();
            Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y + 2.0f,
                                         this->actor.world.pos.z, &D_80854864);
            Matrix_Scale(4.0f, 4.0f, 4.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 16, 32, 1, 0,
                                        (play->gameplayFrames * -15) % 128, 16, 32));
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, D_8085486C);
            gDPSetEnvColor(POLY_XLU_DISP++, 120, 90, 30, 128);
            gSPDisplayList(POLY_XLU_DISP++, gHoverBootsCircleDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Player_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Player* this = (Player*)thisx;

    Vec3f pos;
    Vec3s rot;
    f32 scale;

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        pos.x = 2.0f;
        pos.y = -130.0f;
        pos.z = -150.0f;
        scale = 0.046f;
    } else if (CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) != EQUIP_VALUE_SWORD_MASTER) {
        pos.x = 25.0f;
        pos.y = -228.0f;
        pos.z = 60.0f;
        scale = 0.056f;
    } else {
        pos.x = 20.0f;
        pos.y = -180.0f;
        pos.z = -40.0f;
        scale = 0.047f;
    }

    rot.y = 32300;
    rot.x = rot.z = 0;

    OPEN_DISPS(play->state.gfxCtx);

    if (!(this->stateFlags2 & PLAYER_STATE2_DISABLE_DRAW)) {
        OverrideLimbDrawOpa overrideLimbDraw = Player_OverrideLimbDrawGameplayDefault;
        s32 lod;
        s32 pad;

        if ((this->csAction != 0) || (Player_CheckHostileLockOn(this) && 0) || (this->actor.projectedPos.z < 160.0f)) {
            lod = 0;
        } else {
            lod = 1;
        }

        if (CVarGetInteger(CVAR_ENHANCEMENT("DisableLOD"), 0)) {
            lod = 0;
        }

        func_80093C80(play);
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);

        if (this->invincibilityTimer > 0) {
            this->damageFlickerAnimCounter += CLAMP(50 - this->invincibilityTimer, 8, 40);
            POLY_OPA_DISP =
                Gfx_SetFog2(POLY_OPA_DISP, 255, 0, 0, 0, 0, 4000 - (s32)(Math_CosS(this->damageFlickerAnimCounter * 256) * 2000.0f));
        }

        func_8002EBCC(&this->actor, play, 0);
        func_8002ED80(&this->actor, play, 0);

        if (this->unk_6AD != 0) {
            Vec3f projectedHeadPos;

            SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &this->actor.focus.pos, &projectedHeadPos);
            if (projectedHeadPos.z < -4.0f) {
                overrideLimbDraw = Player_OverrideLimbDrawGameplayFirstPerson;
            }
        } else if (this->stateFlags2 & PLAYER_STATE2_CRAWLING) {
            if (this->actor.projectedPos.z < 0.0f) {
                overrideLimbDraw = Player_OverrideLimbDrawGameplayCrawling;
            }
        }

        if (this->stateFlags2 & PLAYER_STATE2_REFLECTION) {
            f32 sp78 = ((u16)(play->gameplayFrames * 600) * M_PI) / 0x8000;
            f32 sp74 = ((u16)(play->gameplayFrames * 1000) * M_PI) / 0x8000;

            Matrix_Push();
            this->actor.scale.y = -this->actor.scale.y;
            Matrix_SetTranslateRotateYXZ(
                this->actor.world.pos.x,
                (this->actor.floorHeight + (this->actor.floorHeight - this->actor.world.pos.y)) +
                    (this->actor.shape.yOffset * this->actor.scale.y),
                this->actor.world.pos.z, &this->actor.shape.rot);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateX(sp78, MTXMODE_APPLY);
            Matrix_RotateY(sp74, MTXMODE_APPLY);
            Matrix_Scale(1.1f, 0.95f, 1.05f, MTXMODE_APPLY);
            Matrix_RotateY(-sp74, MTXMODE_APPLY);
            Matrix_RotateX(-sp78, MTXMODE_APPLY);
            Player_DrawGameplay(play, this, lod, gCullFrontDList, overrideLimbDraw);
            this->actor.scale.y = -this->actor.scale.y;
            Matrix_Pop();
        }

        gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BOTH);
        gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BOTH);

        Player_DrawGameplay(play, this, lod, gCullBackDList, overrideLimbDraw);

        if (this->invincibilityTimer > 0) {
            POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
        }

        if (this->stateFlags2 & PLAYER_STATE2_FROZEN) {
            f32 scale = (this->av1.actionVar1 >> 1) * 22.0f;

            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, (0 - play->gameplayFrames) % 128, 32, 32, 1,
                                        0, (play->gameplayFrames * -2) % 128, 32, 32));

            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, 255);
            gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);
        }

        if (this->unk_862 > 0) {
            Player_DrawGetItem(play, this);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Player_Destroy(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;

    Effect_Delete(play, this->meleeWeaponEffectIndex);

    Collider_DestroyCylinder(play, &this->cylinder);
    Collider_DestroyQuad(play, &this->meleeWeaponQuads[0]);
    Collider_DestroyQuad(play, &this->meleeWeaponQuads[1]);
    Collider_DestroyQuad(play, &this->shieldQuad);

    Magic_Reset(play);

    gSaveContext.linkAge = play->linkAgeOnLoad;

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
    ResourceMgr_UnregisterSkeleton(&this->upperSkelAnime);
}

//first person manipulate player actor
s16 func_8084ABD8(PlayState* play, Player* this, s32 arg2, s16 arg3) {
    s32 temp1 = 0;
    s16 temp2 = 0;
    s16 temp3 = 0;
    s8 invertXAxisMulti = ((CVarGetInteger(CVAR_SETTING("Controls.InvertAimingXAxis"), 0) && !CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0)) || (!CVarGetInteger(CVAR_SETTING("Controls.InvertAimingXAxis"), 0) && CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0))) ? -1 : 1;
    s8 invertYAxisMulti = CVarGetInteger(CVAR_SETTING("Controls.InvertAimingYAxis"), 1) ? 1 : -1;
    f32 xAxisMulti = CVarGetFloat(CVAR_SETTING("FirstPersonCameraSensitivity.X"), 1.0f);
    f32 yAxisMulti = CVarGetFloat(CVAR_SETTING("FirstPersonCameraSensitivity.Y"), 1.0f);

    if (!func_8002DD78(this) && !func_808334B4(this) && (arg2 == 0)) { // First person without weapon
        // Y Axis
        if (!CVarGetInteger(CVAR_SETTING("MoveInFirstPerson"), 0)) {
            temp2 += sControlInput->rel.stick_y * 240.0f * invertYAxisMulti * yAxisMulti;
        }
        if (CVarGetInteger(CVAR_SETTING("Controls.RightStickAim"), 0) && fabsf(sControlInput->cur.right_stick_y) > 15.0f) {
            temp2 += sControlInput->cur.right_stick_y * 240.0f * invertYAxisMulti * yAxisMulti;
        }
        if (fabsf(sControlInput->cur.gyro_x) > 0.01f) {
            temp2 += (-sControlInput->cur.gyro_x) * 750.0f;
        }
        if (CVarGetInteger(CVAR_SETTING("DisableFirstPersonAutoCenterView"), 0)) {
            this->actor.focus.rot.x += temp2 * 0.1f;
            this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -14000, 14000);
        } else {
            Math_SmoothStepToS(&this->actor.focus.rot.x, temp2, 14, 4000, 30);
        }

        // X Axis
        temp2 = 0;
        if (!CVarGetInteger(CVAR_SETTING("MoveInFirstPerson"), 0)) {
            temp2 += sControlInput->rel.stick_x * -16.0f * invertXAxisMulti * xAxisMulti;
        }
        if (CVarGetInteger(CVAR_SETTING("Controls.RightStickAim"), 0) && fabsf(sControlInput->cur.right_stick_x) > 15.0f) {
            temp2 += sControlInput->cur.right_stick_x * -16.0f * invertXAxisMulti * xAxisMulti;
        }
        if (fabsf(sControlInput->cur.gyro_y) > 0.01f) {
            temp2 += (sControlInput->cur.gyro_y) * 750.0f * invertXAxisMulti;
        }
        temp2 = CLAMP(temp2, -3000, 3000);
        this->actor.focus.rot.y += temp2;
    } else { // First person with weapon
        // Y Axis
        temp1 = (this->stateFlags1 & PLAYER_STATE1_ON_HORSE) ? 3500 : 14000;
        
        if (!CVarGetInteger(CVAR_SETTING("MoveInFirstPerson"), 0)) {
            temp3 += ((sControlInput->rel.stick_y >= 0) ? 1 : -1) *
                    (s32)((1.0f - Math_CosS(sControlInput->rel.stick_y * 200)) * 1500.0f) * invertYAxisMulti * yAxisMulti;
        }
        if (CVarGetInteger(CVAR_SETTING("Controls.RightStickAim"), 0) && fabsf(sControlInput->cur.right_stick_y) > 15.0f) {
            temp3 += ((sControlInput->cur.right_stick_y >= 0) ? 1 : -1) *
                    (s32)((1.0f - Math_CosS(sControlInput->cur.right_stick_y * 200)) * 1500.0f) * invertYAxisMulti * yAxisMulti;
        }
        if (fabsf(sControlInput->cur.gyro_x) > 0.01f) {
            temp3 += (-sControlInput->cur.gyro_x) * 750.0f;
        }
        this->actor.focus.rot.x += temp3;
        this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -temp1, temp1);

        // X Axis
        temp1 = 19114;
        temp2 = this->actor.focus.rot.y - this->actor.shape.rot.y;
        temp3 = 0;
        if (!CVarGetInteger(CVAR_SETTING("MoveInFirstPerson"), 0)) {
            temp3 = ((sControlInput->rel.stick_x >= 0) ? 1 : -1) *
                    (s32)((1.0f - Math_CosS(sControlInput->rel.stick_x * 200)) * -1500.0f) * invertXAxisMulti * xAxisMulti;
        }
        if (CVarGetInteger(CVAR_SETTING("Controls.RightStickAim"), 0) && fabsf(sControlInput->cur.right_stick_x) > 15.0f) {
            temp3 += ((sControlInput->cur.right_stick_x >= 0) ? 1 : -1) *
                    (s32)((1.0f - Math_CosS(sControlInput->cur.right_stick_x * 200)) * -1500.0f) * invertXAxisMulti * xAxisMulti;
        }
        if (fabsf(sControlInput->cur.gyro_y) > 0.01f) {
            temp3 += (sControlInput->cur.gyro_y) * 750.0f * invertXAxisMulti;
        }
        temp2 += temp3;
        this->actor.focus.rot.y = CLAMP(temp2, -temp1, temp1) + this->actor.shape.rot.y;
    }

    if (CVarGetInteger(CVAR_SETTING("MoveInFirstPerson"), 0)) {
        f32 movementSpeed = LINK_IS_ADULT ? 9.0f : 8.25f;
        if (CVarGetInteger(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_VANILLA) != BUNNY_HOOD_VANILLA && this->currentMask == PLAYER_MASK_BUNNY) {
            movementSpeed *= 1.5f;
        }

        f32 relX = (sControlInput->rel.stick_x / 10 * -invertXAxisMulti);
        f32 relY = (sControlInput->rel.stick_y / 10);

        // Normalize so that diagonal movement isn't faster
        f32 relMag = sqrtf((relX * relX) + (relY * relY));
        if (relMag > 1.0f) {
            relX /= relMag;
            relY /= relMag;
        }

        // Determine what left and right mean based on camera angle
        f32 relX2 = relX * Math_CosS(this->actor.focus.rot.y) + relY * Math_SinS(this->actor.focus.rot.y);
        f32 relY2 = relY * Math_CosS(this->actor.focus.rot.y) - relX * Math_SinS(this->actor.focus.rot.y);

        // Calculate distance for footstep sound
        f32 distance = sqrtf((relX2 * relX2) + (relY2 * relY2)) * movementSpeed;
        func_8084029C(this, distance / 4.5f);

        this->actor.world.pos.x += (relX2 * movementSpeed) + this->actor.colChkInfo.displacement.x;
        this->actor.world.pos.z += (relY2 * movementSpeed) + this->actor.colChkInfo.displacement.z;
    }

    this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_Y;
    return func_80836AB8(this, (play->shootingGalleryStatus != 0) || func_8002DD78(this) || func_808334B4(this)) - arg3;
}

void func_8084AEEC(Player* this, f32* arg1, f32 arg2, s16 arg3) {
    f32 temp1;
    f32 temp2;
    
    // #region SOH [Enhancement]
    f32 swimMod = 1.0f;

    if (CVarGetInteger(CVAR_SETTING("WalkModifier.Enabled"), 0) == 1) {
        if (CVarGetInteger(CVAR_SETTING("WalkModifier.SpeedToggle"), 0) == 1) {
            if (gWalkSpeedToggle1) {
                swimMod *= CVarGetFloat(CVAR_SETTING("WalkModifier.SwimMapping1"), 1.0f);
            } else if (gWalkSpeedToggle2) {
                swimMod *= CVarGetFloat(CVAR_SETTING("WalkModifier.SwimMapping2"), 1.0f);
            }
        // sControlInput is NULL to prevent inputs while surfacing after obtaining an underwater item so we want to ignore it for that case
        } else if (sControlInput != NULL) {
            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_CUSTOM_MODIFIER1)) {
                swimMod *= CVarGetFloat(CVAR_SETTING("WalkModifier.SwimMapping1"), 1.0f);
            } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_CUSTOM_MODIFIER2)) {
                swimMod *= CVarGetFloat(CVAR_SETTING("WalkModifier.SwimMapping2"), 1.0f);
            }
        }
        temp1 = this->skelAnime.curFrame - 10.0f;

        temp2 = (R_RUN_SPEED_LIMIT / 100.0f) * 0.8f * swimMod;
        if (*arg1 > temp2) {
            *arg1 = temp2;
        }
    
        if ((0.0f < temp1) && (temp1 < 10.0f)) {
            temp1 *= 6.0f;
        } else {
            temp1 = 0.0f;
            arg2 = 0.0f;
        }

        Math_AsymStepToF(arg1, arg2 * 0.8f * swimMod, temp1, (fabsf(*arg1) * 0.02f) + 0.05f);
        Math_ScaledStepToS(&this->yaw, arg3, 1600);
    // #endregion
    } else {

        temp1 = this->skelAnime.curFrame - 10.0f;

        temp2 = (R_RUN_SPEED_LIMIT / 100.0f) * 0.8f;
        if (*arg1 > temp2) {
            *arg1 = temp2;
        }

        if ((0.0f < temp1) && (temp1 < 10.0f)) {
            temp1 *= 6.0f;
        } else {
            temp1 = 0.0f;
            arg2 = 0.0f;
        }

        Math_AsymStepToF(arg1, arg2 * 0.8f, temp1, (fabsf(*arg1) * 0.02f) + 0.05f);
        Math_ScaledStepToS(&this->yaw, arg3, 1600);
    }
}

// #region SOH [Enhancement]
//Diving uses function func_8084AEEC to calculate changes both xz and y velocity (via func_8084DBC4)
//Provide original calculation for y velocity when swim speed mod is active
void SurfaceWithoutSwimMod(Player* this, f32* arg1, f32 arg2, s16 arg3) {
    f32 temp1;
    f32 temp2;

    temp1 = this->skelAnime.curFrame - 10.0f;

    temp2 = (R_RUN_SPEED_LIMIT / 100.0f) * 0.8f;
    if (*arg1 > temp2) {
        *arg1 = temp2;
    }

    if ((0.0f < temp1) && (temp1 < 10.0f)) {
        temp1 *= 6.0f;
    } else {
        temp1 = 0.0f;
        arg2 = 0.0f;
    }

    Math_AsymStepToF(arg1, arg2 * 0.8f, temp1, (fabsf(*arg1) * 0.02f) + 0.05f);
    Math_ScaledStepToS(&this->yaw, arg3, 1600);
}
// #endregion

void func_8084B000(Player* this) {
    f32 phi_f18;
    f32 phi_f16;
    f32 phi_f14;
    f32 yDistToWater;

    phi_f14 = -5.0f;

    phi_f16 = this->ageProperties->unk_28;
    if (this->actor.velocity.y < 0.0f) {
        phi_f16 += 1.0f;
    }

    if (this->actor.yDistToWater < phi_f16) {
        if (this->actor.velocity.y <= 0.0f) {
            phi_f16 = 0.0f;
        } else {
            phi_f16 = this->actor.velocity.y * 0.5f;
        }
        phi_f18 = -0.1f - phi_f16;
    } else {
        if (!(this->stateFlags1 & PLAYER_STATE1_DEAD) && (this->currentBoots == PLAYER_BOOTS_IRON) &&
            (this->actor.velocity.y >= -3.0f)) {
            phi_f18 = -0.2f;
        } else {
            phi_f14 = 2.0f;
            if (this->actor.velocity.y >= 0.0f) {
                phi_f16 = 0.0f;
            } else {
                phi_f16 = this->actor.velocity.y * -0.3f;
            }
            phi_f18 = phi_f16 + 0.1f;
        }

        yDistToWater = this->actor.yDistToWater;
        if (yDistToWater > 100.0f) {
            this->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
        }
    }

    this->actor.velocity.y += phi_f18;

    if (((this->actor.velocity.y - phi_f14) * phi_f18) > 0) {
        this->actor.velocity.y = phi_f14;
    }

    this->actor.gravity = 0.0f;
}

void func_8084B158(PlayState* play, Player* this, Input* input, f32 arg3) {
    f32 temp;

    if ((input != NULL) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B)) {
        temp = 1.0f;
    } else {
        temp = 0.5f;
    }

    temp *= arg3;

    if (temp < 1.0f) {
        temp = 1.0f;
    }

    this->skelAnime.playSpeed = temp;
    LinkAnimation_Update(play, &this->skelAnime);
}

void Player_Action_8084B1D8(Player* this, PlayState* play) {
    if (this->stateFlags1 & PLAYER_STATE1_IN_WATER) {
        func_8084B000(this);
        func_8084AEEC(this, &this->linearVelocity, 0, this->actor.shape.rot.y);
    } else {
        Player_DecelerateToZero(this);
    }

    if ((this->unk_6AD == 2) && (func_8002DD6C(this) || func_808332E4(this))) {
        Player_UpdateUpperBody(this, play);
    }

    u16 buttonsToCheck = BTN_A | BTN_B | BTN_R | BTN_CUP | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) != 0) {
        buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
    }
    if ((this->csAction != 0) || (this->unk_6AD == 0) || (this->unk_6AD >= 4) || Player_UpdateHostileLockOn(this) ||
        (this->focusActor != NULL) || !func_8083AD4C(play, this) ||
        (((this->unk_6AD == 2) && (CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B | BTN_R) ||
                                   Player_FriendlyLockOnOrParallel(this) || (!func_8002DD78(this) && !func_808334B4(this)))) ||
         ((this->unk_6AD == 1) && CHECK_BTN_ANY(sControlInput->press.button, buttonsToCheck)))) {
        func_8083C148(this, play);
        func_80078884(NA_SE_SY_CAMERA_ZOOM_UP);
    } else if ((DECR(this->av2.actionVar2) == 0) || (this->unk_6AD != 2)) {
        if (func_8008F128(this)) {
            this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_FOCUS_Y | UNK6AE_ROT_UPPER_X;
        } else {
            this->actor.shape.rot.y = func_8084ABD8(play, this, 0, 0);
        }
    }

    this->yaw = this->actor.shape.rot.y;
}

s32 func_8084B3CC(PlayState* play, Player* this) {
    if (play->shootingGalleryStatus != 0) {
        func_80832564(play, this);
        Player_SetupAction(play, this, Player_Action_8084FA54, 0);

        if (!func_8002DD6C(this) || Player_HoldsHookshot(this)) {
            s32 projectileItemToUse = ITEM_BOW;
            if(CVarGetInteger(CVAR_ENHANCEMENT("BowSlingshotAmmoFix"), 0) || CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0)) {
                projectileItemToUse = LINK_IS_ADULT ? ITEM_BOW : ITEM_SLINGSHOT;
            }

            Player_UseItem(play, this, projectileItemToUse);
        }

        this->stateFlags1 |= PLAYER_STATE1_FIRST_PERSON;
        Player_AnimPlayOnce(play, this, Player_GetIdleAnim(this));
        Player_ZeroSpeedXZ(this);
        func_8083B010(this);
        return 1;
    }

    return 0;
}

void func_8084B498(Player* this) {
    this->itemAction =
        (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY) ? PLAYER_IA_OCARINA_FAIRY : PLAYER_IA_OCARINA_OF_TIME;
}

s32 func_8084B4D4(PlayState* play, Player* this) {
    if (this->stateFlags3 & PLAYER_STATE3_FORCE_PULL_OCARINA) {
        this->stateFlags3 &= ~PLAYER_STATE3_FORCE_PULL_OCARINA;
        func_8084B498(this);
        this->unk_6AD = 4;
        Player_ActionHandler_13(this, play);
        return 1;
    }

    return 0;
}

void Player_Action_Talk(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    Player_UpdateUpperBody(this, play);

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~ACTOR_FLAG_PLAYER_TALKED_TO;

        if (!CHECK_FLAG_ALL(this->talkActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE)) {
            this->stateFlags2 &= ~PLAYER_STATE2_LOCK_ON_WITH_SWITCH;
        }

        func_8005B1A4(Play_GetCamera(play, 0));

        if (!func_8084B4D4(play, this) && !func_8084B3CC(play, this) && !Player_StartCsAction(play, this)) {
            if ((this->talkActor != this->interactRangeActor) || !Player_ActionHandler_2(this, play)) {
                if (this->stateFlags1 & PLAYER_STATE1_ON_HORSE) {
                    s32 sp24 = this->av2.actionVar2;
                    func_8083A360(play, this);
                    this->av2.actionVar2 = sp24;
                } else if (func_808332B8(this)) {
                    func_80838F18(play, this);
                } else {
                    func_80853080(this, play);
                }
            }
        }

        this->textboxBtnCooldownTimer = 10;
        return;
    }

    if (this->stateFlags1 & PLAYER_STATE1_ON_HORSE) {
        Player_Action_8084CC98(this, play);
    } else if (func_808332B8(this)) {
        Player_Action_8084D610(this, play);
    } else if (!Player_CheckHostileLockOn(this) && LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->skelAnime.moveFlags != 0) {
            Player_FinishAnimMovement(this);

            if ((this->talkActor->category == ACTORCAT_NPC) && (this->heldItemAction != PLAYER_IA_FISHING_POLE)) {
                Player_AnimPlayOnceAdjusted(play, this, &gPlayerAnim_link_normal_talk_free);
            } else {
                Player_AnimPlayLoop(play, this, Player_GetIdleAnim(this));
            }
        } else {
            Player_AnimPlayLoopAdjusted(play, this, &gPlayerAnim_link_normal_talk_free_wait);
        }
    }

    if (this->focusActor != NULL) {
        this->yaw = this->actor.shape.rot.y = func_8083DB98(this, false);
    }
}

void Player_Action_8084B78C(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;
    s32 temp;

    this->stateFlags2 |= PLAYER_STATE2_DO_ACTION_GRAB | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS | PLAYER_STATE2_GRABBING_DYNAPOLY;
    func_8083F524(play, this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (!func_8083F9D0(play, this)) {
            Player_GetMovementSpeedAndYaw(this, &sp34, &sp32, SPEED_MODE_LINEAR, play);
            temp = func_8083FFB8(this, &sp34, &sp32);
            if (temp > 0) {
                func_8083FAB8(this, play);
            } else if (temp < 0) {
                func_8083FB14(this, play);
            }
        }
    }
}

void func_8084B840(PlayState* play, Player* this, f32 arg2) {
    if (this->actor.wallBgId != BGCHECK_SCENE) {
        DynaPolyActor* dynaPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);

        if (dynaPolyActor != NULL) {
            func_8002DFA4(dynaPolyActor, arg2, this->actor.world.rot.y);
        }
    }
}

static AnimSfxEntry D_80854870[] = {
    { NA_SE_PL_SLIP, ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 3) },
    { NA_SE_PL_SLIP, -ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 21) },
};

void Player_Action_8084B898(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;
    s32 temp;

    this->stateFlags2 |= PLAYER_STATE2_DO_ACTION_GRAB | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS | PLAYER_STATE2_GRABBING_DYNAPOLY;

    if (func_80832CB0(play, this, &gPlayerAnim_link_normal_pushing)) {
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 11.0f)) {
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_PUSH);
        }
    }

    Player_ProcessAnimSfxList(this, D_80854870);
    func_8083F524(play, this);

    if (!func_8083F9D0(play, this)) {
        Player_GetMovementSpeedAndYaw(this, &sp34, &sp32, SPEED_MODE_LINEAR, play);
        temp = func_8083FFB8(this, &sp34, &sp32);
        if (temp < 0) {
            func_8083FB14(this, play);
        } else if (temp == 0) {
            func_8083F72C(this, &gPlayerAnim_link_normal_push_end, play);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_MOVING_DYNAPOLY;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_MOVING_DYNAPOLY) {
        func_8084B840(play, this, 2.0f);
        this->linearVelocity = 2.0f;
    }
}

static AnimSfxEntry D_80854878[] = {
    { NA_SE_PL_SLIP, ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 4) },
    { NA_SE_PL_SLIP, -ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 24) },
};

static Vec3f D_80854880 = { 0.0f, 26.0f, -40.0f };

void Player_Action_8084B9E4(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 sp70;
    s16 sp6E;
    s32 temp1;
    Vec3f sp5C;
    f32 temp2;
    CollisionPoly* sp54;
    s32 sp50;
    Vec3f sp44;
    Vec3f sp38;

    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_pulling, this->modelAnimType);
    this->stateFlags2 |= PLAYER_STATE2_DO_ACTION_GRAB | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS | PLAYER_STATE2_GRABBING_DYNAPOLY;

    if (func_80832CB0(play, this, anim)) {
        this->av2.actionVar2 = 1;
    } else {
        if (this->av2.actionVar2 == 0) {
            if (LinkAnimation_OnFrame(&this->skelAnime, 11.0f)) {
                Player_PlayVoiceSfx(this, NA_SE_VO_LI_PUSH);
            }
        } else {
            Player_ProcessAnimSfxList(this, D_80854878);
        }
    }

    func_8083F524(play, this);

    if (!func_8083F9D0(play, this)) {
        Player_GetMovementSpeedAndYaw(this, &sp70, &sp6E, SPEED_MODE_LINEAR, play);
        temp1 = func_8083FFB8(this, &sp70, &sp6E);
        if (temp1 > 0) {
            func_8083FAB8(this, play);
        } else if (temp1 == 0) {
            func_8083F72C(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_pull_end, this->modelAnimType), play);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_MOVING_DYNAPOLY;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_MOVING_DYNAPOLY) {
        temp2 = func_8083973C(play, this, &D_80854880, &sp5C) - this->actor.world.pos.y;
        if (fabsf(temp2) < 20.0f) {
            sp44.x = this->actor.world.pos.x;
            sp44.z = this->actor.world.pos.z;
            sp44.y = sp5C.y;
            if (!BgCheck_EntityLineTest1(&play->colCtx, &sp44, &sp5C, &sp38, &sp54, true, false, false, true,
                                         &sp50)) {
                func_8084B840(play, this, -2.0f);
                return;
            }
        }
        this->stateFlags2 &= ~PLAYER_STATE2_MOVING_DYNAPOLY;
    }
}

void Player_Action_8084BBE4(Player* this, PlayState* play) {
    f32 sp3C;
    s16 sp3A;
    LinkAnimationHeader* anim;
    f32 temp;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        // clang-format off
        anim = (this->av1.actionVar1 > 0) ? &gPlayerAnim_link_normal_fall_wait : GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_wait, this->modelAnimType); Player_AnimPlayLoop(play, this, anim);
        // clang-format on
    } else if (this->av1.actionVar1 == 0) {
        if (this->skelAnime.animation == &gPlayerAnim_link_normal_fall) {
            temp = 11.0f;
        } else {
            temp = 1.0f;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, temp)) {
            Player_PlayFloorSfx(this, NA_SE_PL_WALK_GROUND);
            if (this->skelAnime.animation == &gPlayerAnim_link_normal_fall) {
                this->av1.actionVar1 = 1;
            } else {
                this->av1.actionVar1 = -1;
            }
        }
    }

    Math_ScaledStepToS(&this->actor.shape.rot.y, this->yaw, 0x800);

    if (this->av1.actionVar1 != 0) {
        Player_GetMovementSpeedAndYaw(this, &sp3C, &sp3A, SPEED_MODE_LINEAR, play);
        if (this->controlStickSpinAngles[this->controlStickDataIndex] >= 0) {
            if (this->av1.actionVar1 > 0) {
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_fall_up, this->modelAnimType);
            } else {
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_up, this->modelAnimType);
            }
            func_8083A9B8(this, anim, play);
            return;
        }

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A) || (this->actor.shape.feetFloorFlags != 0)) {
            func_80837B60(this);
            if (this->av1.actionVar1 < 0) {
                this->linearVelocity = -0.8f;
            } else {
                this->linearVelocity = 0.8f;
            }
            func_80837B9C(this, play);
            this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE);
        }
    }
}

void Player_Action_8084BDFC(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_ApplyAnimMovementScaledByAge(this, 1);
        func_8083C0E8(this, play);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, this->skelAnime.endFrame - 6.0f)) {
        Player_PlayLandingSfx(this);
    } else if (LinkAnimation_OnFrame(&this->skelAnime, this->skelAnime.endFrame - 34.0f)) {
        this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE);
        Player_PlaySfx(this, NA_SE_PL_CLIMB_CLIFF);
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_CLIMB_END);
    }
}

void func_8084BEE4(Player* this) {
    Player_PlaySfx(this, (this->av1.actionVar1 != 0) ? NA_SE_PL_WALK_WALL : NA_SE_PL_WALK_LADDER);
}

void Player_Action_8084BF1C(Player* this, PlayState* play) {
    static Vec3f D_8085488C = { 0.0f, 0.0f, 26.0f };
    s32 sp84;
    s32 sp80;
    f32 phi_f0;
    f32 phi_f2;
    Vec3f sp6C;
    s32 sp68;
    Vec3f sp5C;
    f32 temp_f0;
    LinkAnimationHeader* anim1;
    LinkAnimationHeader* anim2;

    sp84 = sControlInput->rel.stick_y;
    sp80 = sControlInput->rel.stick_x;

    this->fallStartHeight = this->actor.world.pos.y;
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;

    if ((this->av1.actionVar1 != 0) && (ABS(sp84) < ABS(sp80))) {
        phi_f0 = ABS(sp80) * 0.0325f;
        sp84 = 0;
    } else {
        phi_f0 = ABS(sp84) * 0.05f;
        sp80 = 0;
    }

    if (phi_f0 < 1.0f) {
        phi_f0 = 1.0f;
    } else if (phi_f0 > 3.35f) {
        phi_f0 = 3.35f;
    }

    if (this->skelAnime.playSpeed >= 0.0f) {
        phi_f2 = 1.0f;
    } else {
        phi_f2 = -1.0f;
    }

    this->skelAnime.playSpeed = phi_f2 * phi_f0 + phi_f2 * CVarGetInteger(CVAR_ENHANCEMENT("ClimbSpeed"), 0);

    if (this->av2.actionVar2 >= 0) {
        if ((this->actor.wallPoly != NULL) && (this->actor.wallBgId != BGCHECK_SCENE)) {
            DynaPolyActor* wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);
            if (wallPolyActor != NULL) {
                Math_Vec3f_Diff(&wallPolyActor->actor.world.pos, &wallPolyActor->actor.prevPos, &sp6C);
                Math_Vec3f_Sum(&this->actor.world.pos, &sp6C, &this->actor.world.pos);
            }
        }

        Actor_UpdateBgCheckInfo(play, &this->actor, 26.0f, 6.0f, this->ageProperties->ceilingCheckHeight, 7);
        func_8083F360(play, this, 26.0f, this->ageProperties->unk_3C, 50.0f, -20.0f);
    }

    if ((this->av2.actionVar2 < 0) || !func_8083FBC0(this, play)) {
        if (LinkAnimation_Update(play, &this->skelAnime) != 0) {
            if (this->av2.actionVar2 < 0) {
                this->av2.actionVar2 = ABS(this->av2.actionVar2) & 1;
                return;
            }

            if (sp84 != 0) {
                sp68 = this->av1.actionVar1 + this->av2.actionVar2;

                if (sp84 > 0) {
                    D_8085488C.y = this->ageProperties->unk_40;
                    temp_f0 = func_8083973C(play, this, &D_8085488C, &sp5C);

                    if (this->actor.world.pos.y < temp_f0) {
                        if (this->av1.actionVar1 != 0) {
                            this->actor.world.pos.y = temp_f0;
                            this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING_LADDER;
                            func_8083A5C4(play, this, this->actor.wallPoly, this->ageProperties->unk_3C,
                                          &gPlayerAnim_link_normal_jump_climb_up_free);
                            this->yaw += 0x8000;
                            this->actor.shape.rot.y = this->yaw;
                            func_8083A9B8(this, &gPlayerAnim_link_normal_jump_climb_up_free, play);
                            this->stateFlags1 |= PLAYER_STATE1_CLIMBING_LEDGE;
                        } else {
                            func_8083F070(this, this->ageProperties->unk_CC[this->av2.actionVar2], play);
                        }
                    } else {
                        this->skelAnime.prevTransl = this->ageProperties->unk_4A[sp68];
                        Player_AnimPlayOnce(play, this, this->ageProperties->unk_AC[sp68]);
                    }
                } else {
                    if ((this->actor.world.pos.y - this->actor.floorHeight) < 15.0f) {
                        if (this->av1.actionVar1 != 0) {
                            func_8083FB7C(this, play);
                        } else {
                            if (this->av2.actionVar2 != 0) {
                                this->skelAnime.prevTransl = this->ageProperties->unk_44;
                            }
                            func_8083F070(this, this->ageProperties->unk_C4[this->av2.actionVar2], play);
                            this->av2.actionVar2 = 1;
                        }
                    } else {
                        sp68 ^= 1;
                        this->skelAnime.prevTransl = this->ageProperties->unk_62[sp68];
                        anim1 = this->ageProperties->unk_AC[sp68];
                        LinkAnimation_Change(play, &this->skelAnime, anim1, -1.0f, Animation_GetLastFrame(anim1),
                                             0.0f, ANIMMODE_ONCE, 0.0f);
                    }
                }
                this->av2.actionVar2 ^= 1;
            } else {
                if ((this->av1.actionVar1 != 0) && (sp80 != 0)) {
                    anim2 = this->ageProperties->unk_BC[this->av2.actionVar2];

                    if (CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0) ? (sp80 < 0) : (sp80 > 0)) {
                        this->skelAnime.prevTransl = this->ageProperties->unk_7A[this->av2.actionVar2];
                        Player_AnimPlayOnce(play, this, anim2);
                    } else {
                        this->skelAnime.prevTransl = this->ageProperties->unk_86[this->av2.actionVar2];
                        LinkAnimation_Change(play, &this->skelAnime, anim2, -1.0f, Animation_GetLastFrame(anim2),
                                             0.0f, ANIMMODE_ONCE, 0.0f);
                    }
                } else {
                    this->stateFlags2 |= PLAYER_STATE2_STATIONARY_LADDER;
                }
            }

            return;
        }
    }

    if (this->av2.actionVar2 < 0) {
        if (((this->av2.actionVar2 == -2) &&
             (LinkAnimation_OnFrame(&this->skelAnime, 14.0f) || LinkAnimation_OnFrame(&this->skelAnime, 29.0f))) ||
            ((this->av2.actionVar2 == -4) &&
             (LinkAnimation_OnFrame(&this->skelAnime, 22.0f) || LinkAnimation_OnFrame(&this->skelAnime, 35.0f) ||
              LinkAnimation_OnFrame(&this->skelAnime, 49.0f) || LinkAnimation_OnFrame(&this->skelAnime, 55.0f)))) {
            func_8084BEE4(this);
        }
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, (this->skelAnime.playSpeed > 0.0f) ? 20.0f : 0.0f)) {
        func_8084BEE4(this);
    }
}

static f32 D_80854898[] = { 10.0f, 20.0f };
static f32 D_808548A0[] = { 40.0f, 50.0f };

static AnimSfxEntry D_808548A8[] = {
    { NA_SE_PL_WALK_LADDER, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 10) },
    { NA_SE_PL_WALK_LADDER, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 20) },
    { NA_SE_PL_WALK_LADDER, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 30) },
};

void Player_Action_8084C5F8(Player* this, PlayState* play) {
    s32 temp;
    f32* sp38;
    CollisionPoly* sp34;
    s32 sp30;
    Vec3f sp24;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;

    temp = Player_TryActionInterrupt(play, this, &this->skelAnime, 4.0f);

    if (temp == 0) {
        this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING_LADDER;
        return;
    }

    if ((temp > 0) || LinkAnimation_Update(play, &this->skelAnime)) {
        func_8083C0E8(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING_LADDER;
        return;
    }

    sp38 = D_80854898;

    if (this->av2.actionVar2 != 0) {
        Player_ProcessAnimSfxList(this, D_808548A8);
        sp38 = D_808548A0;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, sp38[0]) || LinkAnimation_OnFrame(&this->skelAnime, sp38[1])) {
        sp24.x = this->actor.world.pos.x;
        sp24.y = this->actor.world.pos.y + 20.0f;
        sp24.z = this->actor.world.pos.z;
        if (BgCheck_EntityRaycastFloor3(&play->colCtx, &sp34, &sp30, &sp24) != 0.0f) {
            this->floorSfxOffset = func_80041F10(&play->colCtx, sp34, sp30);
            Player_PlayLandingSfx(this);
        }
    }
}

static AnimSfxEntry D_808548B4[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 40) },   { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 48) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 56) },   { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 64) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 72) },   { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 80) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 88) },   { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 96) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 104) },
};

void Player_Action_8084C760(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_LOADING)) {
            if (this->skelAnime.moveFlags != 0) {
                this->skelAnime.moveFlags = 0;
                return;
            }

            // player speed in a tunnel
            if (!Player_TryLeavingCrawlspace(this, play)) {
                // #region SOH [Enhancement]
                if (CVarGetInteger(CVAR_ENHANCEMENT("CrawlSpeed"), 1) > 1) {
                    this->linearVelocity = sControlInput->rel.stick_y * 0.03f * CVarGetInteger(CVAR_ENHANCEMENT("CrawlSpeed"), 1);
                // #endregion
                } else {
                    this->linearVelocity = sControlInput->rel.stick_y * 0.03f;
                }
            }
        }
        return;
    }

    Player_ProcessAnimSfxList(this, D_808548B4);
}

static AnimSfxEntry D_808548D8[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 10) },  { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 18) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 26) },  { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 34) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 52) },  { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 60) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 68) },  { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 76) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 84) },
};

void Player_Action_8084C81C(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_8083C0E8(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_CRAWLING;
        return;
    }

    Player_ProcessAnimSfxList(this, D_808548D8);
}

static Vec3f D_808548FC[] = {
    { 40.0f, 0.0f, 0.0f },
    { -40.0f, 0.0f, 0.0f },
};

static Vec3f D_80854914[] = {
    { 60.0f, 20.0f, 0.0f },
    { -60.0f, 20.0f, 0.0f },
};

static Vec3f D_8085492C[] = {
    { 60.0f, -20.0f, 0.0f },
    { -60.0f, -20.0f, 0.0f },
};

s32 func_8084C89C(PlayState* play, Player* this, s32 arg2, f32* arg3) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    f32 sp50;
    f32 sp4C;
    Vec3f sp40;
    Vec3f sp34;
    CollisionPoly* sp30;
    s32 sp2C;

    sp50 = rideActor->actor.world.pos.y + 20.0f;
    sp4C = rideActor->actor.world.pos.y - 20.0f;

    *arg3 = func_8083973C(play, this, &D_808548FC[arg2], &sp40);

    return (sp4C < *arg3) && (*arg3 < sp50) &&
           !Player_PosVsWallLineTest(play, this, &D_80854914[arg2], &sp30, &sp2C, &sp34) &&
           !Player_PosVsWallLineTest(play, this, &D_8085492C[arg2], &sp30, &sp2C, &sp34);
}

s32 func_8084C9BC(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    s32 sp38;
    f32 sp34;

    if (this->av2.actionVar2 < 0) {
        this->av2.actionVar2 = 99;
    } else {
        sp38 = (this->mountSide < 0) ? 0 : 1;
        if (!func_8084C89C(play, this, sp38, &sp34)) {
            sp38 ^= 1;
            if (!func_8084C89C(play, this, sp38, &sp34)) {
                return 0;
            } else {
                this->mountSide = -this->mountSide;
            }
        }

        if ((play->csCtx.state == CS_STATE_IDLE) && (play->transitionMode == TRANS_MODE_OFF) &&
            (EN_HORSE_CHECK_1(rideActor) || EN_HORSE_CHECK_4(rideActor))) {
            this->stateFlags2 |= PLAYER_STATE2_DO_ACTION_DOWN;

            if (EN_HORSE_CHECK_1(rideActor) ||
                (EN_HORSE_CHECK_4(rideActor) && CHECK_BTN_ALL(sControlInput->press.button, BTN_A))) {
                rideActor->actor.child = NULL;
                Player_SetupActionPreserveAnimMovement(play, this, Player_Action_8084D3E4, 0);
                this->unk_878 = sp34 - rideActor->actor.world.pos.y;
                Player_AnimPlayOnce(play, this,
                              (this->mountSide < 0) ? &gPlayerAnim_link_uma_left_down
                                                    : &gPlayerAnim_link_uma_right_down);
                return 1;
            }
        }
    }

    return 0;
}

void func_8084CBF4(Player* this, f32 arg1, f32 arg2) {
    f32 temp;
    f32 dir;

    if ((this->unk_878 != 0.0f) && (arg2 <= this->skelAnime.curFrame)) {
        if (arg1 < fabsf(this->unk_878)) {
            if (this->unk_878 >= 0.0f) {
                dir = 1;
            } else {
                dir = -1;
            }
            temp = dir * arg1;
        } else {
            temp = this->unk_878;
        }
        this->actor.world.pos.y += temp;
        this->unk_878 -= temp;
    }
}

static LinkAnimationHeader* D_80854944[] = {
    &gPlayerAnim_link_uma_anim_stop,
    &gPlayerAnim_link_uma_anim_stand,
    &gPlayerAnim_link_uma_anim_walk,
    &gPlayerAnim_link_uma_anim_slowrun,
    &gPlayerAnim_link_uma_anim_fastrun,
    &gPlayerAnim_link_uma_anim_jump100,
    &gPlayerAnim_link_uma_anim_jump200,
    NULL,
    NULL,
};

static LinkAnimationHeader* D_80854968[] = {
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_slowrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    NULL,
    NULL,
};

static LinkAnimationHeader* D_8085498C[] = {
    &gPlayerAnim_link_uma_wait_3,
    &gPlayerAnim_link_uma_wait_1,
    &gPlayerAnim_link_uma_wait_2,
};

static u8 D_80854998[2][2] = {
    { 32, 58 },
    { 25, 42 },
};

static Vec3s D_8085499C = { -69, 7146, -266 };

static AnimSfxEntry D_808549A4[] = {
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 48) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 58) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 68) },
    { NA_SE_PL_CALM_PAT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 92) },
    { NA_SE_PL_CALM_PAT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 110) },
    { NA_SE_PL_CALM_PAT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 126) },
    { NA_SE_PL_CALM_PAT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 132) },
    { NA_SE_PL_CALM_PAT, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 136) },
};

void Player_Action_8084CC98(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    u8* arr;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;

    func_8084CBF4(this, 1.0f, 10.0f);

    if (this->av2.actionVar2 == 0) {
        if (LinkAnimation_Update(play, &this->skelAnime)) {
            this->skelAnime.animation = &gPlayerAnim_link_uma_wait_1;
            this->av2.actionVar2 = 99;
            return;
        }

        arr = D_80854998[(this->mountSide < 0) ? 0 : 1];

        if (LinkAnimation_OnFrame(&this->skelAnime, arr[0])) {
            Player_PlaySfx(this, NA_SE_PL_CLIMB_CLIFF);
            return;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, arr[1])) {
            func_8002DE74(play, this);
            Player_PlaySfx(this, NA_SE_PL_SIT_ON_HORSE);
            return;
        }

        return;
    }

    func_8002DE74(play, this);
    this->skelAnime.prevTransl = D_8085499C;

    if ((rideActor->animationIdx != this->av2.actionVar2) &&
        ((rideActor->animationIdx >= 2) || (this->av2.actionVar2 >= 2))) {
        if ((this->av2.actionVar2 = rideActor->animationIdx) < 2) {
            f32 rand = Rand_ZeroOne();
            s32 temp = 0;

            this->av2.actionVar2 = 1;

            if (rand < 0.1f) {
                temp = 2;
            } else if (rand < 0.2f) {
                temp = 1;
            }
            Player_AnimPlayOnce(play, this, D_8085498C[temp]);
        } else {
            this->skelAnime.animation = D_80854944[this->av2.actionVar2 - 2];
            Animation_SetMorph(play, &this->skelAnime, 8.0f);
            if (this->av2.actionVar2 < 4) {
                func_80834644(play, this);
                this->av1.actionVar1 = 0;
            }
        }
    }

    if (this->av2.actionVar2 == 1) {
        if (sUpperBodyIsBusy || Player_IsTalking(play)) {
            Player_AnimPlayOnce(play, this, &gPlayerAnim_link_uma_wait_3);
        } else if (LinkAnimation_Update(play, &this->skelAnime)) {
            this->av2.actionVar2 = 99;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_uma_wait_1) {
            Player_ProcessAnimSfxList(this, D_808549A4);
        }
    } else {
        this->skelAnime.curFrame = rideActor->curFrame;
        LinkAnimation_AnimateFrame(play, &this->skelAnime);
    }

    AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->skelAnime.morphTable,
                                this->skelAnime.jointTable);

    if ((play->csCtx.state != CS_STATE_IDLE) || (this->csAction != 0)) {
        if (this->csAction == 7) {
            this->csAction = 0;
        }
        this->unk_6AD = 0;
        this->av1.actionVar1 = 0;
    } else if ((this->av2.actionVar2 < 2) || (this->av2.actionVar2 >= 4)) {
        sUpperBodyIsBusy = Player_UpdateUpperBody(this, play);
        if (sUpperBodyIsBusy) {
            this->av1.actionVar1 = 0;
        }
    }

    this->actor.world.pos.x = rideActor->actor.world.pos.x + rideActor->riderPos.x;
    this->actor.world.pos.y = (rideActor->actor.world.pos.y + rideActor->riderPos.y) - 27.0f;
    this->actor.world.pos.z = rideActor->actor.world.pos.z + rideActor->riderPos.z;

    this->yaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

    if ((this->csAction != 0) ||
        (!Player_IsTalking(play) && ((rideActor->actor.speedXZ != 0.0f) || !Player_ActionHandler_Talk(this, play)) &&
         !Player_ActionHandler_Roll(this, play))) {
        if (!sUpperBodyIsBusy) {
            if (this->av1.actionVar1 != 0) {
                if (LinkAnimation_Update(play, &this->upperSkelAnime)) {
                    rideActor->stateFlags &= ~ENHORSE_FLAG_8;
                    this->av1.actionVar1 = 0;
                }

                if (this->upperSkelAnime.animation == &gPlayerAnim_link_uma_stop_muti) {
                    if (LinkAnimation_OnFrame(&this->upperSkelAnime, 23.0f)) {
                        Player_PlaySfx(this, NA_SE_IT_LASH);
                        Player_PlayVoiceSfx(this, NA_SE_VO_LI_LASH);
                    }

                    AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                                this->upperSkelAnime.jointTable);
                } else {
                    if (LinkAnimation_OnFrame(&this->upperSkelAnime, 10.0f)) {
                        Player_PlaySfx(this, NA_SE_IT_LASH);
                        Player_PlayVoiceSfx(this, NA_SE_VO_LI_LASH);
                    }

                    AnimationContext_SetCopyTrue(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                                 this->upperSkelAnime.jointTable, sUpperBodyLimbCopyMap);
                }
            } else {
                LinkAnimationHeader* anim = NULL;

                if (EN_HORSE_CHECK_3(rideActor)) {
                    anim = &gPlayerAnim_link_uma_stop_muti;
                } else if (EN_HORSE_CHECK_2(rideActor)) {
                    if ((this->av2.actionVar2 >= 2) && (this->av2.actionVar2 != 99)) {
                        anim = D_80854968[this->av2.actionVar2 - 2];
                    }
                }

                if (anim != NULL) {
                    LinkAnimation_PlayOnce(play, &this->upperSkelAnime, anim);
                    this->av1.actionVar1 = 1;
                }
            }
        }

        if (this->stateFlags1 & PLAYER_STATE1_FIRST_PERSON) {
            if (!func_8083AD4C(play, this) || CHECK_BTN_ANY(sControlInput->press.button, BTN_A) ||
                Player_IsZTargeting(this)) {
                this->unk_6AD = 0;
                this->stateFlags1 &= ~PLAYER_STATE1_FIRST_PERSON;
            } else {
                this->upperLimbRot.y = func_8084ABD8(play, this, 1, -5000) - this->actor.shape.rot.y;
                this->upperLimbRot.y += 5000;
                this->upperLimbYawSecondary = -5000;
            }
            return;
        }

        if ((this->csAction != 0) || (!func_8084C9BC(this, play) && !Player_ActionHandler_13(this, play))) {
            if (this->focusActor != NULL) {
                if (func_8002DD78(this) != 0) {
                    this->upperLimbRot.y = func_8083DB98(this, 1) - this->actor.shape.rot.y;
                    this->upperLimbRot.y = CLAMP(this->upperLimbRot.y, -0x4AAA, 0x4AAA);
                    this->actor.focus.rot.y = this->actor.shape.rot.y + this->upperLimbRot.y;
                    this->upperLimbRot.y += 5000;
                    this->unk_6AE_rotFlags |= UNK6AE_ROT_UPPER_Y;
                } else {
                    func_8083DB98(this, 0);
                }
            } else {
                if (func_8002DD78(this) != 0) {
                    this->upperLimbRot.y = func_8084ABD8(play, this, 1, -5000) - this->actor.shape.rot.y;
                    this->upperLimbRot.y += 5000;
                    this->upperLimbYawSecondary = -5000;
                }
            }
        }
    }
}

static AnimSfxEntry D_808549C4[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 0) },
    { NA_SE_PL_GET_OFF_HORSE, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 10) },
    { NA_SE_PL_SLIPDOWN, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 25) },
};

void Player_Action_8084D3E4(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;
    func_8084CBF4(this, 1.0f, 10.0f);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        EnHorse* rideActor = (EnHorse*)this->rideActor;

        func_8083C0E8(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_ON_HORSE;
        this->actor.parent = NULL;
        AREG(6) = 0;

        if (Flags_GetEventChkInf(EVENTCHKINF_EPONA_OBTAINED) || (DREG(1) != 0)) {
            gSaveContext.horseData.pos.x = rideActor->actor.world.pos.x;
            gSaveContext.horseData.pos.y = rideActor->actor.world.pos.y;
            gSaveContext.horseData.pos.z = rideActor->actor.world.pos.z;
            gSaveContext.horseData.angle = rideActor->actor.shape.rot.y;
        }
    } else {
        Camera_ChangeSetting(Play_GetCamera(play, 0), CAM_SET_NORMAL0);

        if (this->mountSide < 0) {
            D_808549C4[0].data = ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 40);
        } else {
            D_808549C4[0].data = ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 29);
        }
        Player_ProcessAnimSfxList(this, D_808549C4);
    }
}

static AnimSfxEntry D_808549D0[] = {
    { NA_SE_PL_SWIM, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 0) },
};

void func_8084D530(Player* this, f32* arg1, f32 arg2, s16 arg3) {
    func_8084AEEC(this, arg1, arg2, arg3);
    Player_ProcessAnimSfxList(this, D_808549D0);
}

void func_8084D574(PlayState* play, Player* this, s16 arg2) {
    Player_SetupAction(play, this, Player_Action_8084D84C, 0);
    this->actor.shape.rot.y = this->yaw = arg2;
    Player_AnimChangeLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim);
}

void func_8084D5CC(PlayState* play, Player* this) {
    Player_SetupAction(play, this, Player_Action_8084DAB4, 0);
    Player_AnimChangeLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim);
}

void Player_Action_8084D610(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;

    func_80832CB0(play, this, &gPlayerAnim_link_swimer_swim_wait);
    func_8084B000(this);

    if (!Player_IsTalking(play) && !Player_TryActionHandlerList(play, this, sActionHandlerList11, true) &&
        !func_8083D12C(play, this, sControlInput)) {
        if (this->unk_6AD != 1) {
            this->unk_6AD = 0;
        }

        if (this->currentBoots == PLAYER_BOOTS_IRON) {
            sp34 = 0.0f;
            sp32 = this->actor.shape.rot.y;

            if (this->actor.bgCheckFlags & 1) {
                func_8083A098(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_short_landing, this->modelAnimType), play);
                Player_PlayLandingSfx(this);
            }
        } else {
            Player_GetMovementSpeedAndYaw(this, &sp34, &sp32, SPEED_MODE_LINEAR, play);

            if (sp34 != 0.0f) {
                s16 temp = this->actor.shape.rot.y - sp32;

                if ((ABS(temp) > 0x6000) && !Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
                    return;
                }

                if (Player_IsZTargetingWithHostileUpdate(this)) {
                    func_8084D5CC(play, this);
                } else {
                    func_8084D574(play, this, sp32);
                }
            }
        }

        func_8084AEEC(this, &this->linearVelocity, sp34, sp32);
    }
}

void Player_Action_8084D7C4(Player* this, PlayState* play) {
    if (!Player_ActionHandler_13(this, play)) {
        this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

        func_8084B158(play, this, NULL, this->linearVelocity);
        func_8084B000(this);

        if (DECR(this->av2.actionVar2) == 0) {
            func_80838F18(play, this);
        }
    }
}

void Player_Action_8084D84C(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;
    s16 temp;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    func_8084B158(play, this, sControlInput, this->linearVelocity);
    func_8084B000(this);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList11, true) &&
        !func_8083D12C(play, this, sControlInput)) {
        Player_GetMovementSpeedAndYaw(this, &sp34, &sp32, SPEED_MODE_LINEAR, play);

        temp = this->actor.shape.rot.y - sp32;
        if ((sp34 == 0.0f) || (ABS(temp) > 0x6000) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
            func_80838F18(play, this);
        } else if (Player_IsZTargetingWithHostileUpdate(this)) {
            func_8084D5CC(play, this);
        }

        func_8084D530(this, &this->linearVelocity, sp34, sp32);
    }
}

s32 func_8084D980(PlayState* play, Player* this, f32* arg2, s16* arg3) {
    LinkAnimationHeader* anim;
    s16 temp1;
    s32 temp2;

    temp1 = this->yaw - *arg3;

    if (ABS(temp1) > 0x6000) {
        anim = &gPlayerAnim_link_swimer_swim_wait;

        if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
            this->yaw = *arg3;
        } else {
            *arg2 = 0.0f;
            *arg3 = this->yaw;
        }
    } else {
        temp2 = func_8083FD78(this, arg2, arg3, play);

        if (temp2 > 0) {
            anim = &gPlayerAnim_link_swimer_swim;
        } else if (temp2 < 0) {
            anim = &gPlayerAnim_link_swimer_back_swim;
        } else if ((temp1 = this->actor.shape.rot.y - *arg3) > 0) {
            anim = &gPlayerAnim_link_swimer_Rside_swim;
        } else {
            anim = &gPlayerAnim_link_swimer_Lside_swim;
        }
    }

    if (anim != this->skelAnime.animation) {
        Player_AnimChangeLoopSlowMorph(play, this, anim);
        return 1;
    }

    return 0;
}

void Player_Action_8084DAB4(Player* this, PlayState* play) {
    f32 sp2C;
    s16 sp2A;

    func_8084B158(play, this, sControlInput, this->linearVelocity);
    func_8084B000(this);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList11, true) &&
        !func_8083D12C(play, this, sControlInput)) {
        Player_GetMovementSpeedAndYaw(this, &sp2C, &sp2A, SPEED_MODE_LINEAR, play);

        if (sp2C == 0.0f) {
            func_80838F18(play, this);
        } else if (!Player_IsZTargetingWithHostileUpdate(this)) {
            func_8084D574(play, this, sp2A);
        } else {
            func_8084D980(play, this, &sp2C, &sp2A);
        }

        func_8084D530(this, &this->linearVelocity, sp2C, sp2A);
    }
}

void func_8084DBC4(PlayState* play, Player* this, f32 arg2) {
    f32 sp2C;
    s16 sp2A;

    Player_GetMovementSpeedAndYaw(this, &sp2C, &sp2A, SPEED_MODE_LINEAR, play);
    func_8084AEEC(this, &this->linearVelocity, sp2C * 0.5f, sp2A);
    // Original implementation of func_8084AEEC (SurfaceWithoutSwimMod) to prevent velocity increases via swim mod which push Link into the air
    // #region SOH [Enhancement]
    if (CVarGetInteger(CVAR_SETTING("WalkModifier.Enabled"), 0)) {
        SurfaceWithoutSwimMod(this, &this->actor.velocity.y, arg2, this->yaw);
    // #endregion
    } else {
        func_8084AEEC(this, &this->actor.velocity.y, arg2, this->yaw);
    }
}

void Player_Action_8084DC48(Player* this, PlayState* play) {
    f32 sp2C;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    this->actor.gravity = 0.0f;
    Player_UpdateUpperBody(this, play);

    if (!Player_ActionHandler_13(this, play)) {
        if (this->currentBoots == PLAYER_BOOTS_IRON) {
            func_80838F18(play, this);
            return;
        }

        if (this->av1.actionVar1 == 0) {
            if (this->av2.actionVar2 == 0) {
                if (LinkAnimation_Update(play, &this->skelAnime) ||
                    ((this->skelAnime.curFrame >= 22.0f) && !CHECK_BTN_ALL(sControlInput->cur.button, BTN_A))) {
                    func_8083D330(play, this);
                } else if (LinkAnimation_OnFrame(&this->skelAnime, 20.0f) != 0) {
                    this->actor.velocity.y = -2.0f;
                }

                Player_DecelerateToZero(this);
                return;
            }

            func_8084B158(play, this, sControlInput, this->actor.velocity.y);
            this->unk_6C2 = 16000;

            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A) && !Player_ActionHandler_2(this, play) &&
                !(this->actor.bgCheckFlags & 1) && (this->actor.yDistToWater < D_80854784[CUR_UPG_VALUE(UPG_SCALE)])) {
                func_8084DBC4(play, this, -2.0f);
            } else {
                this->av1.actionVar1++;
                Player_AnimChangeLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
            }
        } else if (this->av1.actionVar1 == 1) {
            LinkAnimation_Update(play, &this->skelAnime);
            func_8084B000(this);

            if (this->unk_6C2 < 10000) {
                this->av1.actionVar1++;
                this->av2.actionVar2 = this->actor.yDistToWater;
                Player_AnimChangeLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim);
            }
        } else if (!func_8083D12C(play, this, sControlInput)) {
            sp2C = (this->av2.actionVar2 * 0.018f) + 4.0f;

            if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
                sControlInput = NULL;
            }

            func_8084B158(play, this, sControlInput, fabsf(this->actor.velocity.y));
            Math_ScaledStepToS(&this->unk_6C2, -10000, 800);

            if (sp2C > 8.0f) {
                sp2C = 8.0f;
            }

            func_8084DBC4(play, this, sp2C);
        }
    }
}

void func_8084DF6C(PlayState* play, Player* this) {
    this->unk_862 = 0;
    this->stateFlags1 &= ~(PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_CARRYING_ACTOR);
    this->getItemId = GI_NONE;
    this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    func_8005B1A4(Play_GetCamera(play, 0));
}

void func_8084DFAC(PlayState* play, Player* this) {
    func_8084DF6C(play, this);
    Player_ApplyYawFromAnim(this);
    func_8083C0E8(this, play);
    this->yaw = this->actor.shape.rot.y;
}

s32 func_8084DFF4(PlayState* play, Player* this) {
    GetItemEntry giEntry;
    s32 temp1;
    s32 temp2;
    static s32 equipItem;
    static bool equipNow;

    if (this->getItemId == GI_NONE && this->getItemEntry.objectId == OBJECT_INVALID) {
        return 1;
    }

    if (this->av1.actionVar1 == 0) {
        if (this->getItemEntry.objectId == OBJECT_INVALID || (this->getItemId != this->getItemEntry.getItemId)) {
            giEntry = ItemTable_Retrieve(this->getItemId);
        } else {
            giEntry = this->getItemEntry;
        }
        this->av1.actionVar1 = 1;
        equipItem = giEntry.itemId;
        equipNow = CVarGetInteger(CVAR_ENHANCEMENT("AskToEquip"), 0) && giEntry.modIndex == MOD_NONE &&
                    equipItem >= ITEM_SWORD_KOKIRI && equipItem <= ITEM_TUNIC_ZORA &&
                    CHECK_AGE_REQ_ITEM(equipItem);

        Message_StartTextbox(play, giEntry.textId, &this->actor);
        // RANDOTODO: Macro this boolean check.
        if (!(giEntry.modIndex == MOD_RANDOMIZER && giEntry.itemId == RG_ICE_TRAP)) {
            if (giEntry.modIndex == MOD_NONE) {
                // RANDOTOD: Move this into Item_Give() or some other more central location
                if (giEntry.getItemId == GI_SWORD_BGS) {
                    gSaveContext.bgsFlag = true;
                    gSaveContext.swordHealth = 8;
                }
                Item_Give(play, giEntry.itemId);
            } else {
                Randomizer_Item_Give(play, giEntry);
            }
            Player_SetPendingFlag(this, play);
        }

        // Use this if we do have a getItemEntry
        if (giEntry.modIndex == MOD_NONE) {
            if (IS_RANDO) {
                Audio_PlayFanfare_Rando(giEntry);
            } else if (((giEntry.itemId >= ITEM_RUPEE_GREEN) && (giEntry.itemId <= ITEM_RUPEE_RED)) ||
                        ((giEntry.itemId >= ITEM_RUPEE_PURPLE) && (giEntry.itemId <= ITEM_RUPEE_GOLD)) ||
                        (giEntry.itemId == ITEM_HEART)) {
                Audio_PlaySoundGeneral(NA_SE_SY_GET_BOXITEM, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                if ((giEntry.itemId == ITEM_HEART_CONTAINER) ||
                    ((giEntry.itemId == ITEM_HEART_PIECE_2) &&
                        ((gSaveContext.inventory.questItems & 0xF0000000) == 0x40000000))) {
                    temp1 = NA_BGM_HEART_GET | 0x900;
                } else {
                    temp1 = temp2 =
                        (giEntry.itemId == ITEM_HEART_PIECE_2) ? NA_BGM_SMALL_ITEM_GET : NA_BGM_ITEM_GET | 0x900;
                }
                Audio_PlayFanfare(temp1);
            }
        } else if (giEntry.modIndex == MOD_RANDOMIZER) {
            if (IS_RANDO) {
                Audio_PlayFanfare_Rando(giEntry);
            } else if (giEntry.itemId == RG_DOUBLE_DEFENSE || giEntry.itemId == RG_MAGIC_SINGLE ||
                        giEntry.itemId == RG_MAGIC_DOUBLE) {
                Audio_PlayFanfare(NA_BGM_HEART_GET | 0x900);
            } else {
                // Just in case something weird happens with MOD_INDEX
                Audio_PlayFanfare(NA_BGM_ITEM_GET | 0x900);
            }
        } else {
            // Just in case something weird happens with modIndex.
            Audio_PlayFanfare(NA_BGM_ITEM_GET | 0x900);
        }
    }
    else if (equipNow && Message_ShouldAdvanceSilent(play) &&
             Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) {
        if (play->msgCtx.choiceIndex == 0) { // Equip now? Yes

            if (equipItem >= ITEM_SWORD_KOKIRI && equipItem <= ITEM_SWORD_BGS) {
                gSaveContext.equips.buttonItems[0] = equipItem;
                Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, equipItem - ITEM_SWORD_KOKIRI + 1);
                func_808328EC(this, NA_SE_IT_SWORD_PUTAWAY);

            } else if (equipItem >= ITEM_SHIELD_DEKU && equipItem <= ITEM_SHIELD_MIRROR) {
                Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, equipItem - ITEM_SHIELD_DEKU + 1);
                func_808328EC(&this->actor, NA_SE_IT_SHIELD_REMOVE);
                Player_SetEquipmentData(play, this);

            } else if (equipItem == ITEM_TUNIC_GORON || equipItem == ITEM_TUNIC_ZORA) {
                Inventory_ChangeEquipment(EQUIP_TYPE_TUNIC, equipItem - ITEM_TUNIC_KOKIRI + 1);
                func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
                Player_SetEquipmentData(play, this);
            }
        }
        equipNow = false;
        Message_CloseTextbox(play);
        play->msgCtx.msgMode = MSGMODE_TEXT_DONE;
    } else {
        if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
            if (GameInteractor_Should(VB_PLAY_NABOORU_CAPTURED_CS, this->getItemId == GI_GAUNTLETS_SILVER)) {
                play->nextEntranceIndex = ENTR_DESERT_COLOSSUS_0;
                play->transitionTrigger = TRANS_TRIGGER_START;
                gSaveContext.nextCutsceneIndex = 0xFFF1;
                play->transitionType = TRANS_TYPE_SANDSTORM_END;
                this->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
                Player_TryCsAction(play, NULL, 8);
            }

            // Set unk_862 to 0 early to not have the game draw non-custom colored models for a split second.
            // This unk is what the game normally uses to decide what item to draw when holding up an item above Link's head.
            // Only do this when the item actually has a custom draw function.
            if (this->getItemEntry.drawFunc != NULL) {
                this->unk_862 = 0;
            }

            // #region SOH [Randomizer] TODO Better Ice trap handling?
            if (this->getItemEntry.itemId == RG_ICE_TRAP && this->getItemEntry.modIndex == MOD_RANDOMIZER) {
                this->unk_862 = 0;
                gSaveContext.pendingIceTrapCount++;
                Player_SetPendingFlag(this, play);
            }
            // #endregion

            this->getItemId = GI_NONE;
            this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
        }
    }

    return 0;
}

void Player_Action_8084E1EC(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) || func_8084DFF4(play, this)) {
            func_8084DF6C(play, this);
            func_80838F18(play, this);
            func_80832340(play, this);
        }
    } else {
        if ((this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) && LinkAnimation_OnFrame(&this->skelAnime, 10.0f)) {
            func_808332F4(this, play);
            func_80832340(play, this);
            func_80835EA4(play, 8);
        } else if (LinkAnimation_OnFrame(&this->skelAnime, 5.0f)) {
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_BREATH_DRINK);
        }
    }

    func_8084B000(this);
    func_8084AEEC(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

void Player_Action_8084E30C(Player* this, PlayState* play) {
    func_8084B000(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_80838F18(play, this);
    }

    func_8084AEEC(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

void Player_Action_8084E368(Player* this, PlayState* play) {
    func_8084B000(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_80843AE8(play, this);
    }

    func_8084AEEC(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

static s16 sWarpSongEntrances[] = {
    ENTR_SACRED_FOREST_MEADOW_2,
    ENTR_DEATH_MOUNTAIN_CRATER_4,
    ENTR_LAKE_HYLIA_8,
    ENTR_DESERT_COLOSSUS_5,
    ENTR_GRAVEYARD_7,
    ENTR_TEMPLE_OF_TIME_7,
};

void Player_Action_8084E3C4(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimPlayLoopAdjusted(play, this, &gPlayerAnim_link_normal_okarina_swing);
        this->av2.actionVar2 = 1;
        if (this->stateFlags2 & (PLAYER_STATE2_NEAR_OCARINA_ACTOR | PLAYER_STATE2_PLAY_FOR_ACTOR)) {
            this->stateFlags2 |= PLAYER_STATE2_ATTEMPT_PLAY_FOR_ACTOR;
        } else {
            func_8010BD58(play, OCARINA_ACTION_FREE_PLAY);
        }
        return;
    }

    if (this->av2.actionVar2 == 0) {
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        func_8005B1A4(Play_GetCamera(play, 0));

        if ((this->talkActor != NULL) && (this->talkActor == this->unk_6A8)) {
            Player_StartTalking(play, this->talkActor);
        } else if (this->naviTextId < 0) {
            this->talkActor = this->naviActor;
            this->naviActor->textId = -this->naviTextId;
            Player_StartTalking(play, this->talkActor);
        } else if (!Player_ActionHandler_13(this, play)) {
            func_8083A098(this, &gPlayerAnim_link_normal_okarina_end, play);
        }

        this->stateFlags2 &= ~(PLAYER_STATE2_NEAR_OCARINA_ACTOR | PLAYER_STATE2_ATTEMPT_PLAY_FOR_ACTOR | PLAYER_STATE2_PLAY_FOR_ACTOR);
        this->unk_6A8 = NULL;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_02) {
        gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex = sWarpSongEntrances[play->msgCtx.lastPlayedSong];
        gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams = 0x5FF;
        gSaveContext.respawn[RESPAWN_MODE_RETURN].data = play->msgCtx.lastPlayedSong;

        this->csAction = 0;
        this->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;

        Player_TryCsAction(play, NULL, 8);
        play->mainCamera.unk_14C &= ~8;

        this->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE;
        this->stateFlags2 |= PLAYER_STATE2_OCARINA_PLAYING;

        if (Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0xF, true) == NULL) {
            Environment_WarpSongLeave(play);
        }

        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
    }
}

void Player_Action_8084E604(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_8083A098(this, &gPlayerAnim_link_normal_light_bom_end, play);
    } else if (LinkAnimation_OnFrame(&this->skelAnime, 3.0f)) {
        Inventory_ChangeAmmo(ITEM_NUT, -1);
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ARROW, this->bodyPartsPos[PLAYER_BODYPART_R_HAND].x,
                    this->bodyPartsPos[PLAYER_BODYPART_R_HAND].y, this->bodyPartsPos[PLAYER_BODYPART_R_HAND].z, 4000,
                    this->actor.shape.rot.y, 0, ARROW_NUT, true);
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_SWORD_N);
    }

    Player_DecelerateToZero(this);
}

static AnimSfxEntry D_808549E0[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_JUMPING, 87) },
    { NA_SE_VO_LI_CLIMB_END, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 87) },
    { NA_SE_VO_LI_AUTO_JUMP, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 69) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 123) },
};

void Player_Action_8084E6D4(Player* this, PlayState* play) {
    s32 cond;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->av2.actionVar2 != 0) {
            if (this->av2.actionVar2 >= 2) {
                this->av2.actionVar2--;
            }

            if (func_8084DFF4(play, this) && (this->av2.actionVar2 == 1)) {
                cond = ((this->talkActor != NULL) && (this->exchangeItemId < 0)) ||
                       (this->stateFlags3 & PLAYER_STATE3_FORCE_PULL_OCARINA);

                if (cond || (gSaveContext.healthAccumulator == 0)) {
                    if (cond) {
                        func_8084DF6C(play, this);
                        this->exchangeItemId = EXCH_ITEM_NONE;

                        if (func_8084B4D4(play, this) == 0) {
                            Player_StartTalking(play, this->talkActor);
                        }
                    } else {
                        func_8084DFAC(play, this);
                    }
                }
            }
        } else {
            Player_FinishAnimMovement(this);
            if ((this->getItemId == GI_ICE_TRAP && !IS_RANDO) ||
                (IS_RANDO && (this->getItemId == RG_ICE_TRAP || this->getItemEntry.getItemId == RG_ICE_TRAP))) {
                this->stateFlags1 &= ~(PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_CARRYING_ACTOR);

                if ((this->getItemId != GI_ICE_TRAP && !IS_RANDO) ||
                    (IS_RANDO && (this->getItemId != RG_ICE_TRAP || this->getItemEntry.getItemId != RG_ICE_TRAP))) {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x,
                                this->actor.world.pos.y + 100.0f, this->actor.world.pos.z, 0, 0, 0, 0, true);
                    func_8083C0E8(this, play);
                } else if (IS_RANDO) {
                    gSaveContext.pendingIceTrapCount++;
                    Player_SetPendingFlag(this, play);
                    func_8083C0E8(this, play);
                } else {
                    this->actor.colChkInfo.damage = 0;
                    func_80837C0C(play, this, PLAYER_HIT_RESPONSE_ICE_TRAP, 0.0f, 0.0f, 0, 20);
                }
                return;
            }

            if (this->skelAnime.animation == &gPlayerAnim_link_normal_box_kick) {
                Player_AnimPlayOnceAdjusted(play, this, &gPlayerAnim_link_demo_get_itemB);
            } else {
                Player_AnimPlayOnceAdjusted(play, this, &gPlayerAnim_link_demo_get_itemA);
            }

            this->av2.actionVar2 = 2;
            func_80835EA4(play, 9);
        }
    } else {
        if (this->av2.actionVar2 == 0) {
            if (!LINK_IS_ADULT) {
                Player_ProcessAnimSfxList(this, D_808549E0);
            }
            return;
        }

        if (this->skelAnime.animation == &gPlayerAnim_link_demo_get_itemB) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000, 4000);
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, 21.0f)) {
            func_808332F4(this, play);
        }
    }
}

static AnimSfxEntry D_808549F0[] = {
    { NA_SE_IT_MASTER_SWORD_SWING, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 60) },
};

void func_8084E988(Player* this) {
    Player_ProcessAnimSfxList(this, D_808549F0);
}

static AnimSfxEntry D_808549F4[] = {
    { NA_SE_VO_LI_AUTO_JUMP, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 5) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 15) },
};

void Player_Action_8084E9AC(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->av1.actionVar1 == 0) {
            if (DECR(this->av2.actionVar2) == 0) {
                this->av1.actionVar1 = 1;
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        } else {
            func_8083C0E8(this, play);
        }
    } else {
        if (LINK_IS_ADULT && LinkAnimation_OnFrame(&this->skelAnime, 158.0f)) {
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_SWORD_N);
            return;
        }

        if (!LINK_IS_ADULT) {
            Player_ProcessAnimSfxList(this, D_808549F4);
        } else {
            func_8084E988(this);
        }
    }
}

static u8 D_808549FC[] = {
    0x01, 0x03, 0x02, 0x04, 0x04,
};

void Player_Action_8084EAC0(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->av2.actionVar2 == 0) {
            if (this->itemAction == PLAYER_IA_BOTTLE_POE) {
                s32 rand = Rand_S16Offset(-1, 3);

                if (rand == 0) {
                    rand = 3;
                }

                if ((rand < 0) && (gSaveContext.health <= 0x10)) {
                    rand = 3;
                }

                if (rand < 0) {
                    Health_ChangeBy(play, -0x10);
                } else {
                    gSaveContext.healthAccumulator = rand * 0x10;
                }
            } else {
                s32 sp28 = D_808549FC[this->itemAction - PLAYER_IA_BOTTLE_POTION_RED];

                if (CVarGetInteger(CVAR_ENHANCEMENT("RedPotionEffect"), 0) && this->itemAction == PLAYER_IA_BOTTLE_POTION_RED) {
                    if (CVarGetInteger(CVAR_ENHANCEMENT("RedPercentRestore"), 0)) {
                        gSaveContext.healthAccumulator =
                            (gSaveContext.healthCapacity * CVarGetInteger(CVAR_ENHANCEMENT("RedPotionHealth"), 100) / 100 + 15) / 16 * 16;
                    } else {
                        gSaveContext.healthAccumulator = CVarGetInteger(CVAR_ENHANCEMENT("RedPotionHealth"), 20) * 16;
                    }
                } else if (CVarGetInteger(CVAR_ENHANCEMENT("BluePotionEffects"), 0) &&
                           this->itemAction == PLAYER_IA_BOTTLE_POTION_BLUE) {
                    if (CVarGetInteger(CVAR_ENHANCEMENT("BlueHealthPercentRestore"), 0)) {
                        gSaveContext.healthAccumulator =
                            (gSaveContext.healthCapacity * CVarGetInteger(CVAR_ENHANCEMENT("BluePotionHealth"), 100) / 100 + 15) / 16 * 16;
                    } else {
                        gSaveContext.healthAccumulator = CVarGetInteger(CVAR_ENHANCEMENT("BluePotionHealth"), 20) * 16;
                    }

                    if (CVarGetInteger(CVAR_ENHANCEMENT("BlueManaPercentRestore"), 0)) {
                        if (gSaveContext.magicState != MAGIC_STATE_ADD) {
                            Magic_Fill(play);
                        }

                        Magic_RequestChange(play,
                                      (gSaveContext.magicLevel * 48 * CVarGetInteger(CVAR_ENHANCEMENT("BluePotionMana"), 100) / 100 + 15) /
                                          16 * 16,
                                      MAGIC_ADD);
                    } else {
                        if (gSaveContext.magicState != MAGIC_STATE_ADD) {
                            Magic_Fill(play);
                        }

                        Magic_RequestChange(play, CVarGetInteger(CVAR_ENHANCEMENT("BluePotionMana"), 100), MAGIC_ADD);
                        ;
                    }
                } else if (CVarGetInteger(CVAR_ENHANCEMENT("GreenPotionEffect"), 0) &&
                           this->itemAction == PLAYER_IA_BOTTLE_POTION_GREEN) {
                    if (CVarGetInteger(CVAR_ENHANCEMENT("GreenPercentRestore"), 0)) {
                        if (gSaveContext.magicState != MAGIC_STATE_ADD) {
                            Magic_Fill(play);
                        }

                        Magic_RequestChange(play,
                                      (gSaveContext.magicLevel * 48 * CVarGetInteger(CVAR_ENHANCEMENT("GreenPotionMana"), 100) / 100 + 15) /
                                          16 * 16,
                                      MAGIC_ADD);
                    } else {
                        if (gSaveContext.magicState != MAGIC_STATE_ADD) {
                            Magic_Fill(play);
                        }

                        Magic_RequestChange(play, CVarGetInteger(CVAR_ENHANCEMENT("GreenPotionMana"), 100), MAGIC_ADD);
                        ;
                    }
                } else if (CVarGetInteger(CVAR_ENHANCEMENT("MilkEffect"), 0) && (this->itemAction == PLAYER_IA_BOTTLE_MILK_FULL ||
                                                             this->itemAction == PLAYER_IA_BOTTLE_MILK_HALF)) {
                    if (CVarGetInteger(CVAR_ENHANCEMENT("MilkPercentRestore"), 0)) {
                        gSaveContext.healthAccumulator =
                            (gSaveContext.healthCapacity * CVarGetInteger(CVAR_ENHANCEMENT("MilkHealth"), 100) / 100 + 15) / 16 * 16;
                    } else {
                        gSaveContext.healthAccumulator = CVarGetInteger(CVAR_ENHANCEMENT("MilkHealth"), 5) * 16;
                    }
                    if (CVarGetInteger(CVAR_ENHANCEMENT("SeparateHalfMilkEffect"), 0) &&
                        this->itemAction == PLAYER_IA_BOTTLE_MILK_HALF) {
                        if (CVarGetInteger(CVAR_ENHANCEMENT("HalfMilkPercentRestore"), 0)) {
                            gSaveContext.healthAccumulator =
                                (gSaveContext.healthCapacity * CVarGetInteger(CVAR_ENHANCEMENT("HalfMilkHealth"), 100) / 100 + 15) / 16 *
                                16;
                        } else {
                            gSaveContext.healthAccumulator = CVarGetInteger(CVAR_ENHANCEMENT("HalfMilkHealth"), 5) * 16;
                        }
                    }
                } else {
                    if (sp28 & 1) {
                        gSaveContext.healthAccumulator = 0x140;
                    }

                    if (sp28 & 2) {
                        Magic_Fill(play);
                    }

                    if (sp28 & 4) {
                        gSaveContext.healthAccumulator = 0x50;
                    }
                }
            }

            Player_AnimPlayLoopAdjusted(play, this, &gPlayerAnim_link_bottle_drink_demo_wait);
            this->av2.actionVar2 = 1;
            return;
        }

        func_8083C0E8(this, play);
        func_8005B1A4(Play_GetCamera(play, 0));
    } else if (this->av2.actionVar2 == 1) {
        if ((gSaveContext.healthAccumulator == 0) && (gSaveContext.magicState != MAGIC_STATE_FILL)) {
            Player_AnimChangeOnceMorphAdjusted(play, this, &gPlayerAnim_link_bottle_drink_demo_end);
            this->av2.actionVar2 = 2;
            Player_UpdateBottleHeld(play, this, ITEM_BOTTLE, PLAYER_IA_BOTTLE);
        }
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_DRINK - SFX_FLAG);
    } else if ((this->av2.actionVar2 == 2) && LinkAnimation_OnFrame(&this->skelAnime, 29.0f)) {
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_BREATH_DRINK);
    }
}

typedef enum BottleCatchType {
    BOTTLE_CATCH_NONE, // This type does not have an associated entry in `sBottleCatchInfo`
    BOTTLE_CATCH_FAIRY,
    BOTTLE_CATCH_FISH,
    BOTTLE_CATCH_BLUE_FIRE,
    BOTTLE_CATCH_BUGS
} BottleCatchType;

typedef struct BottleCatchInfo {
    /* 0x00 */ s16 actorId;
    /* 0x02 */ u8 itemId;
    /* 0x03 */ u8 itemAction;
    /* 0x04 */ u8 textId;
} BottleCatchInfo; // size = 0x06

static BottleCatchInfo sBottleCatchInfo[] = {
    { ACTOR_EN_ELF, ITEM_FAIRY, PLAYER_IA_BOTTLE_FAIRY, 0x46 },         // BOTTLE_CATCH_FAIRY
    { ACTOR_EN_FISH, ITEM_FISH, PLAYER_IA_BOTTLE_FISH, 0x47 },          // BOTTLE_CATCH_FISH
    { ACTOR_EN_ICE_HONO, ITEM_BLUE_FIRE, PLAYER_IA_BOTTLE_FIRE, 0x5D }, // BOTTLE_CATCH_BLUE_FIRE
    { ACTOR_EN_INSECT, ITEM_BUG, PLAYER_IA_BOTTLE_BUG, 0x7A },          // BOTTLE_CATCH_BUGS
};

void Player_Action_SwingBottle(Player* this, PlayState* play) {
    // Action Variable 2 has two separate uses within the same action.
    // After it is used as `inWater` here, it will be used for `startedTextbox` below.
    // The two usages will never overlap, so this won't cause any issues.
    BottleSwingInfo* swingEntry = &sBottleSwingInfo[this->av2.inWater];

    Player_DecelerateToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->av1.bottleCatchType != BOTTLE_CATCH_NONE) {
            if (!this->av2.startedTextbox) {
                if (CVarGetInteger(CVAR_ENHANCEMENT("FastDrops"), 0)) {
                    this->av1.bottleCatchType = BOTTLE_CATCH_NONE;
                } else {
                    // 1 is subtracted because `sBottleCatchInfo` does not have an entry for `BOTTLE_CATCH_NONE`
                    Message_StartTextbox(play, sBottleCatchInfo[this->av1.bottleCatchType - 1].textId, &this->actor);
                }
                Audio_PlayFanfare(NA_BGM_ITEM_GET | 0x900);
                GameInteractor_ExecuteOnPlayerBottleUpdate((sBottleCatchInfo[this->av1.bottleCatchType - 1].itemId));
                this->av2.startedTextbox = true;
            } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                this->av1.bottleCatchType = BOTTLE_CATCH_NONE;
                func_8005B1A4(Play_GetCamera(play, 0));
            }
        } else {
            func_8083C0E8(this, play);
        }
    } else if (this->av1.bottleCatchType == BOTTLE_CATCH_NONE) {
        s32 activeFrame = this->skelAnime.curFrame - swingEntry->firstActiveFrame;

        if (activeFrame >= 0 && activeFrame <= swingEntry->numActiveFrames) {
            if (this->av2.inWater && activeFrame == 0) {
                // Play water scoop sound on the first active frame, if applicable
                Player_PlaySfx(this, NA_SE_IT_SCOOP_UP_WATER);
            }

            // `interactRangeActor` will be set by the Get Item system. See `Actor_OfferGetItem`.
            if (this->interactRangeActor != NULL) {
                BottleCatchInfo* catchInfo = &sBottleCatchInfo[0];
                s32 i;

                // Try to find an `interactRangeActor` with the same ID as an entry in `sBottleCatchInfo`
                for (i = 0; i < ARRAY_COUNT(sBottleCatchInfo); i++, catchInfo++) {
                    if (this->interactRangeActor->id == catchInfo->actorId) {
                        break;
                    }
                }

                if (GameInteractor_Should(VB_BOTTLE_ACTOR, i < ARRAY_COUNT(sBottleCatchInfo), this->interactRangeActor)) {
                    // 1 is added because `sBottleCatchInfo` does not have an entry for `BOTTLE_CATCH_NONE`
                    this->av1.bottleCatchType = i + 1;

                    this->av2.startedTextbox = false;
                    if (!CVarGetInteger(CVAR_ENHANCEMENT("FastDrops"), 0)) {
                        this->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE;
                    }
                    this->interactRangeActor->parent = &this->actor;

                    Player_UpdateBottleHeld(play, this, catchInfo->itemId, ABS(catchInfo->itemAction));
                    if (!CVarGetInteger(CVAR_ENHANCEMENT("FastDrops"), 0)) {
                        Player_AnimPlayOnceAdjusted(play, this, swingEntry->catchAnimation);
                        func_80835EA4(play, 4);
                    }
                }
            }
        }
    }

    //! @bug If the animation is changed at any point above (such as by func_8083C0E8() or
    //! Player_AnimPlayOnceAdjusted()), it will change the curFrame to 0. This causes this flag to be set for one frame,
    //! at a time when it does not look like Player is swinging the bottle.
    if (this->skelAnime.curFrame <= 7.0f) {
        this->stateFlags1 |= PLAYER_STATE1_SWINGING_BOTTLE;
    }
}

static Vec3f D_80854A1C = { 0.0f, 0.0f, 5.0f };

void Player_Action_8084EED8(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_8083C0E8(this, play);
        func_8005B1A4(Play_GetCamera(play, 0));
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 37.0f)) {
        Player_SpawnFairy(play, this, &this->leftHandPos, &D_80854A1C, FAIRY_REVIVE_BOTTLE);
        Player_UpdateBottleHeld(play, this, ITEM_BOTTLE, PLAYER_IA_BOTTLE);
        Player_PlaySfx(this, NA_SE_EV_BOTTLE_CAP_OPEN);
        Player_PlaySfx(this, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
    } else if (LinkAnimation_OnFrame(&this->skelAnime, 47.0f)) {
        if (CVarGetInteger(CVAR_ENHANCEMENT("FairyEffect"), 0)) {
            if (CVarGetInteger(CVAR_ENHANCEMENT("FairyPercentRestore"), 0)) {
                gSaveContext.healthAccumulator =
                    (gSaveContext.healthCapacity * CVarGetInteger(CVAR_ENHANCEMENT("FairyHealth"), 100) / 100 + 15) / 16 * 16;
            } else {
                gSaveContext.healthAccumulator = CVarGetInteger(CVAR_ENHANCEMENT("FairyHealth"), 8) * 16;
            }
        } else {
            gSaveContext.healthAccumulator = 0x140;
        }
    }
}

static BottleDropInfo D_80854A28[] = {
    { ACTOR_EN_FISH, FISH_DROPPED },
    { ACTOR_EN_ICE_HONO, 0 },
    { ACTOR_EN_INSECT, 2 },
};

static AnimSfxEntry D_80854A34[] = {
    { NA_SE_VO_LI_AUTO_JUMP, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 38) },
    { NA_SE_EV_BOTTLE_CAP_OPEN, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 40) },
};

void Player_Action_8084EFC0(Player* this, PlayState* play) {
    Player_DecelerateToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_8083C0E8(this, play);
        func_8005B1A4(Play_GetCamera(play, 0));
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 76.0f)) {
        BottleDropInfo* dropInfo = &D_80854A28[this->itemAction - PLAYER_IA_BOTTLE_FISH];

        Actor_Spawn(&play->actorCtx, play, dropInfo->actorId,
                    (Math_SinS(this->actor.shape.rot.y) * 5.0f) + this->leftHandPos.x, this->leftHandPos.y,
                    (Math_CosS(this->actor.shape.rot.y) * 5.0f) + this->leftHandPos.z, 0x4000, this->actor.shape.rot.y,
                    0, dropInfo->actorParams, true);

        Player_UpdateBottleHeld(play, this, ITEM_BOTTLE, PLAYER_IA_BOTTLE);
        return;
    }

    Player_ProcessAnimSfxList(this, D_80854A34);
}

static AnimSfxEntry D_80854A3C[] = {
    { NA_SE_PL_PUT_OUT_ITEM, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 30) },
};

void Player_Action_ExchangeItem(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->av2.actionVar2 < 0) {
            func_8083C0E8(this, play);
        } else if (this->exchangeItemId == EXCH_ITEM_NONE) {
            Actor* talkActor = this->talkActor;

            this->unk_862 = 0;
            if (talkActor->textId != 0xFFFF) {
                this->actor.flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
            }

            Player_StartTalking(play, talkActor);
        } else {
            GetItemEntry giEntry = ItemTable_Retrieve(D_80854528[this->exchangeItemId - 1]);

            if (this->itemAction >= PLAYER_IA_ZELDAS_LETTER) {
                if (giEntry.gi >= 0) {
                    this->unk_862 = giEntry.gi;
                } else {
                    this->unk_862 = -giEntry.gi;
                }
            }

            if (this->av2.actionVar2 == 0) {
                s32 pad;

                Message_StartTextbox(play, this->actor.textId, &this->actor);

                if ((this->itemAction == PLAYER_IA_CHICKEN) || (this->itemAction == PLAYER_IA_POCKET_CUCCO)) {
                    Player_PlaySfx(this, NA_SE_EV_CHICKEN_CRY_M);
                }

                this->av2.actionVar2 = 1;
            } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                this->actor.flags &= ~ACTOR_FLAG_PLAYER_TALKED_TO;
                this->unk_862 = 0;

                if (this->av1.actionVar1 == 1) {
                    Player_AnimPlayOnce(play, this, &gPlayerAnim_link_bottle_read_end);
                    this->av2.actionVar2 = -1;
                } else {
                    func_8083C0E8(this, play);
                }

                func_8005B1A4(Play_GetCamera(play, 0));
            }
        }
    } else if (this->av2.actionVar2 >= 0) {
        Player_ProcessAnimSfxList(this, D_80854A3C);
    }

    if ((this->av1.actionVar1 == 0) && (this->focusActor != NULL)) {
        this->yaw = this->actor.shape.rot.y = func_8083DB98(this, 0);
    }
}

void Player_Action_8084F308(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimPlayLoop(play, this, &gPlayerAnim_link_normal_re_dead_attack_wait);
    }

    if (func_80832594(this, 0, 100)) {
        func_80839F90(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_GRABBED_BY_ENEMY;
    }
}

void Player_Action_SlideOnSlope(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET | PLAYER_STATE2_DISABLE_ROTATION_ALWAYS;
    LinkAnimation_Update(play, &this->skelAnime);
    func_8084269C(play, this);
    func_800F4138(&this->actor.projectedPos, NA_SE_PL_SLIP_LEVEL - SFX_FLAG, this->actor.speedXZ);

    if (Player_ActionHandler_13(this, play) == 0) {
        CollisionPoly* floorPoly = this->actor.floorPoly;
        f32 xzSpeedTarget;
        f32 xzSpeedIncrStep;
        f32 xzSpeedDecrStep;
        s16 downwardSlopeYaw;
        s16 shapeYawTarget;
        Vec3f slopeNormal;

        if (floorPoly == NULL) {
            func_80837B9C(this, play);
            return;
        }

        Player_GetSlopeDirection(floorPoly, &slopeNormal, &downwardSlopeYaw);

        shapeYawTarget = downwardSlopeYaw;
        if (this->av1.facingUpSlope) {
            shapeYawTarget = downwardSlopeYaw + 0x8000;
        }

        if (this->linearVelocity < 0) {
            downwardSlopeYaw += 0x8000;
        }

        xzSpeedTarget = (1.0f - slopeNormal.y) * 40.0f;
        xzSpeedTarget = CLAMP(xzSpeedTarget, 0, 10.0f);
        xzSpeedIncrStep = SQ(xzSpeedTarget) * 0.015f;
        xzSpeedDecrStep = slopeNormal.y * 0.01f;

        if (SurfaceType_GetSlope(&play->colCtx, floorPoly, this->actor.floorBgId) != 1) {
            xzSpeedTarget = 0;
            xzSpeedDecrStep = slopeNormal.y * 10.0f;
        }

        if (xzSpeedIncrStep < 1.0f) {
            xzSpeedIncrStep = 1.0f;
        }

        if (Math_AsymStepToF(&this->linearVelocity, xzSpeedTarget, xzSpeedIncrStep, xzSpeedDecrStep) && (xzSpeedTarget == 0)) {
            LinkAnimationHeader* slideAnimation;

            if (!this->av1.facingUpSlope) {
                slideAnimation = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_down_slope_slip_end, this->modelAnimType);
            } else {
                slideAnimation = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_up_slope_slip_end, this->modelAnimType);
            }
            func_8083A098(this, slideAnimation, play);
        }

        Math_SmoothStepToS(&this->yaw, downwardSlopeYaw, 10, 4000, 800);
        Math_ScaledStepToS(&this->actor.shape.rot.y, shapeYawTarget, 2000);
    }
}

void Player_Action_8084F608(Player* this, PlayState* play) {
    if ((DECR(this->av2.actionVar2) == 0) && Player_StartCsAction(play, this)) {
        func_80852280(play, this, NULL);
        Player_SetupAction(play, this, Player_Action_CsAction, 0);
        Player_Action_CsAction(this, play);
    }
}

void Player_Action_8084F698(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_8084F608, 0);
    this->av2.actionVar2 = 40;
    Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0x10, true);
}

void Player_Action_8084F710(Player* this, PlayState* play) {
    s32 pad;

    if ((this->av1.actionVar1 != 0) && (play->csCtx.frames < 0x131)) {
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
    } else if (sYDistToFloor < 150.0f) {
        if (LinkAnimation_Update(play, &this->skelAnime)) {
            if (this->av2.actionVar2 == 0) {
                if (this->actor.bgCheckFlags & 1) {
                    this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
                    Player_PlayLandingSfx(this);
                    this->av2.actionVar2 = 1;
                }
            } else {
                if ((play->sceneNum == SCENE_KOKIRI_FOREST) && Player_StartCsAction(play, this)) {
                    return;
                }
                func_80853080(this, play);
            }
        }
        Math_SmoothStepToF(&this->actor.velocity.y, 2.0f, 0.3f, 8.0f, 0.5f);
    }

    if ((play->sceneNum == SCENE_CHAMBER_OF_THE_SAGES) && Player_StartCsAction(play, this)) {
        return;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.linkAction != NULL)) {
        f32 sp28 = this->actor.world.pos.y;
        func_808529D0(play, this, play->csCtx.linkAction);
        this->actor.world.pos.y = sp28;
    }
}

void Player_Action_8084F88C(Player* this, PlayState* play) {
    LinkAnimation_Update(play, &this->skelAnime);

    if ((this->av2.actionVar2++ > 8) && (play->transitionTrigger == TRANS_TRIGGER_OFF)) {

        if (this->av1.actionVar1 != 0) {
            if (play->sceneNum == SCENE_ICE_CAVERN) {
                Play_TriggerRespawn(play);
                play->nextEntranceIndex = ENTR_ICE_CAVERN_0;
            } else if (this->av1.actionVar1 < 0) {
                Play_TriggerRespawn(play);
                // In ER, handle DMT and other special void outs to respawn from last entrance from grotto 
                if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
                    Grotto_ForceRegularVoidOut();
                }
            } else {
                Play_TriggerVoidOut(play);
            }

            play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
            func_80078884(NA_SE_OC_ABYSS);
        } else {
            play->transitionType = TRANS_TYPE_FADE_BLACK;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = 0xFF;
        }

        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

void Player_Action_8084F9A0(Player* this, PlayState* play) {
    Player_ActionHandler_1(this, play);
}

void Player_Action_8084F9C0(Player* this, PlayState* play) {
    this->actor.gravity = -1.0f;

    LinkAnimation_Update(play, &this->skelAnime);

    if (this->actor.velocity.y < 0.0f) {
        func_80837B9C(this, play);
    } else if (this->actor.velocity.y < 6.0f) {
        Math_StepToF(&this->linearVelocity, 3.0f, 0.5f);
    }
}

void Player_Action_8084FA54(Player* this, PlayState* play) {
    this->unk_6AD = 2;

    func_8083AD4C(play, this);
    LinkAnimation_Update(play, &this->skelAnime);
    Player_UpdateUpperBody(this, play);

    this->upperLimbRot.y = func_8084ABD8(play, this, 1, 0) - this->actor.shape.rot.y;
    this->unk_6AE_rotFlags |= UNK6AE_ROT_UPPER_Y;

    if (play->shootingGalleryStatus < 0) {
        play->shootingGalleryStatus++;
        if (play->shootingGalleryStatus == 0) {
            func_8083C148(this, play);
        }
    }
}

void Player_Action_8084FB10(Player* this, PlayState* play) {
    if (this->av1.actionVar1 >= 0) {
        if (this->av1.actionVar1 < 6) {
            this->av1.actionVar1++;
        }

        if (func_80832594(this, 1, 100)) {
            this->av1.actionVar1 = -1;
            EffectSsIcePiece_SpawnBurst(play, &this->actor.world.pos, this->actor.scale.x);
            Player_PlaySfx(this, NA_SE_PL_ICE_BROKEN);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_FROZEN;
        }

        if ((play->gameplayFrames % 4) == 0) {
            Player_InflictDamage(play, -1);
        }
    } else {
        if (LinkAnimation_Update(play, &this->skelAnime)) {
            func_80839F90(this, play);
            Player_SetInvulnerability(this, -20);
        }
    }
}

void Player_Action_8084FBF4(Player* this, PlayState* play) {
    LinkAnimation_Update(play, &this->skelAnime);
    func_808382BC(this);

    if (((this->av2.actionVar2 % 25) != 0) || func_80837B18(play, this, -1)) {
        if (DECR(this->av2.actionVar2) == 0) {
            func_80839F90(this, play);
        }
    }

    this->bodyShockTimer = 40;
    func_8002F8F0(&this->actor, NA_SE_VO_LI_TAKEN_AWAY - SFX_FLAG + this->ageProperties->unk_92);
}

/**
 * Updates the "Noclip" debug feature, which allows the player to fly around anywhere
 * in the world and clip through any collision.
 *
 * Noclip can be toggled on and off with two different button combos:
 * Hold L + R + A and press B
 * or
 * Hold L and press D-pad right
 *
 * To control Noclip mode:
 * - Move horizontally with the 4 D-pad directions
 * - Move up with B
 * - Move down with A
 * - Hold R to move faster
 *
 * With Noclip enabled, another button combination can be pressed to set all "temp clear" flags
 * in the current room. To do so hold L and press D-pad left.
 *
 * @return  true if Noclip is disabled, false if enabled
 */
s32 Player_UpdateNoclip(Player* this, PlayState* play) {
    sControlInput = &play->state.input[0];

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugEnabled"), 0) &&
        ((CHECK_BTN_ALL(sControlInput->cur.button, BTN_A | BTN_L | BTN_R) &&
          CHECK_BTN_ALL(sControlInput->press.button, BTN_B)) ||
         (CHECK_BTN_ALL(sControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sControlInput->press.button, BTN_DRIGHT)))) {

        sNoclipEnabled ^= 1;

        if (sNoclipEnabled) {
            Camera_ChangeMode(Play_GetCamera(play, 0), CAM_MODE_BOWARROWZ);
        }
    }

    if (sNoclipEnabled) {
        f32 speed;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {
            speed = 100.0f;
        } else {
            speed = 20.0f;
        }

        func_8006375C(3, 2, "DEBUG MODE");

        if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_L)) {
            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
                this->actor.world.pos.y += speed;
            } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A)) {
                this->actor.world.pos.y -= speed;
            }

            if (CHECK_BTN_ANY(sControlInput->cur.button, BTN_DUP | BTN_DLEFT | BTN_DDOWN | BTN_DRIGHT)) {
                s16 angle;
                s16 temp;

                angle = temp = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));

                if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DDOWN)) {
                    angle = temp + 0x8000;
                } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DLEFT)) {
                    angle = temp + (0x4000 * (CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0) ? -1 : 1));
                } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DRIGHT)) {
                    angle = temp - (0x4000 * (CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0) ? -1 : 1));
                }

                this->actor.world.pos.x += speed * Math_SinS(angle);
                this->actor.world.pos.z += speed * Math_CosS(angle);
            }
        }

        Player_ZeroSpeedXZ(this);

        this->actor.gravity = 0.0f;
        this->actor.velocity.x = this->actor.velocity.z = this->actor.velocity.y = 0.0f;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sControlInput->press.button, BTN_DLEFT)) {
            Flags_SetTempClear(play, play->roomCtx.curRoom.num);
        }

        Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);

        return false;
    }

    return true;
}

void func_8084FF7C(Player* this) {
    this->unk_858 += this->unk_85C;
    this->unk_85C -= this->unk_858 * 5.0f;
    this->unk_85C *= 0.3f;

    if (ABS(this->unk_85C) < 0.00001f) {
        this->unk_85C = 0.0f;
        if (ABS(this->unk_858) < 0.00001f) {
            this->unk_858 = 0.0f;
        }
    }
}

/**
 * Updates the Bunny Hood's floppy ears' rotation and velocity.
 */
void Player_UpdateBunnyEars(Player* this) {
    Vec3s force;
    s16 angle;

    // Damping: decay by 1/8 the previous value each frame
    sBunnyEarKinematics.angVel.x -= sBunnyEarKinematics.angVel.x >> 3;
    sBunnyEarKinematics.angVel.y -= sBunnyEarKinematics.angVel.y >> 3;

    // Elastic restorative force
    sBunnyEarKinematics.angVel.x += -sBunnyEarKinematics.rot.x >> 2;
    sBunnyEarKinematics.angVel.y += -sBunnyEarKinematics.rot.y >> 2;

    // Forcing from motion relative to shape frame
    angle = this->actor.world.rot.y - this->actor.shape.rot.y;
    force.x = (s32)(this->actor.speedXZ * -200.0f * Math_CosS(angle) * (Rand_CenteredFloat(2.0f) + 10.0f)) & 0xFFFF;
    force.y = (s32)(this->actor.speedXZ * 100.0f * Math_SinS(angle) * (Rand_CenteredFloat(2.0f) + 10.0f)) & 0xFFFF;

    sBunnyEarKinematics.angVel.x += force.x >> 2;
    sBunnyEarKinematics.angVel.y += force.y >> 2;

    // Clamp both angular velocities to [-6000, 6000]
    if (sBunnyEarKinematics.angVel.x > 6000) {
        sBunnyEarKinematics.angVel.x = 6000;
    } else if (sBunnyEarKinematics.angVel.x < -6000) {
        sBunnyEarKinematics.angVel.x = -6000;
    }
    if (sBunnyEarKinematics.angVel.y > 6000) {
        sBunnyEarKinematics.angVel.y = 6000;
    } else if (sBunnyEarKinematics.angVel.y < -6000) {
        sBunnyEarKinematics.angVel.y = -6000;
    }

    // Add angular velocity to rotations
    sBunnyEarKinematics.rot.x += sBunnyEarKinematics.angVel.x;
    sBunnyEarKinematics.rot.y += sBunnyEarKinematics.angVel.y;

    // swivel ears outwards if bending backwards
    if (sBunnyEarKinematics.rot.x < 0) {
        sBunnyEarKinematics.rot.z = sBunnyEarKinematics.rot.x >> 1;
    } else {
        sBunnyEarKinematics.rot.z = 0;
    }
}

s32 Player_ActionHandler_7(Player* this, PlayState* play) {
    if (func_8083C6B8(play, this) == 0) {
        if (func_8083BB20(this) != 0) {
            s32 sp24 = func_80837818(this);

            func_80837948(play, this, sp24);

            if (sp24 >= PLAYER_MWA_SPIN_ATTACK_1H) {
                this->stateFlags2 |= PLAYER_STATE2_SPIN_ATTACKING;
                func_80837530(play, this, 0);
                return 1;
            }
        } else {
            return 0;
        }
    }

    return 1;
}

static Vec3f D_80854A40 = { 0.0f, 40.0f, 45.0f };

void Player_Action_808502D0(Player* this, PlayState* play) {
    struct_80854190* sp44 = &D_80854190[this->meleeWeaponAnimation];

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    if (!func_80842DF4(play, this)) {
        func_8084285C(this, 0.0f, sp44->unk_0C, sp44->unk_0D);

        if ((this->stateFlags2 & PLAYER_STATE2_SWORD_LUNGE) && (this->heldItemAction != PLAYER_IA_HAMMER) &&
            LinkAnimation_OnFrame(&this->skelAnime, 0.0f)) {
            this->linearVelocity = 15.0f;
            this->stateFlags2 &= ~PLAYER_STATE2_SWORD_LUNGE;
        }

        if (this->linearVelocity > 12.0f) {
            func_8084269C(play, this);
        }

        Math_StepToF(&this->linearVelocity, 0.0f, 5.0f);
        func_8083C50C(this);

        if (LinkAnimation_Update(play, &this->skelAnime)) {
            if (!Player_ActionHandler_7(this, play)) {
                u8 sp43 = this->skelAnime.moveFlags;
                LinkAnimationHeader* sp3C;

                if (Player_CheckHostileLockOn(this)) {
                    sp3C = sp44->unk_08;
                } else {
                    sp3C = sp44->unk_04;
                }

                func_80832318(this);
                this->skelAnime.moveFlags = 0;

                if ((sp3C == &gPlayerAnim_link_fighter_Lpower_jump_kiru_end) &&
                    (this->modelAnimType != PLAYER_ANIMTYPE_3)) {
                    sp3C = &gPlayerAnim_link_fighter_power_jump_kiru_end;
                }

                func_8083A098(this, sp3C, play);

                this->skelAnime.moveFlags = sp43;
                this->stateFlags3 |= PLAYER_STATE3_FINISHED_ATTACKING;
            }
        } else if (this->heldItemAction == PLAYER_IA_HAMMER) {
            if ((this->meleeWeaponAnimation == PLAYER_MWA_HAMMER_FORWARD) ||
                (this->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH)) {
                static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
                Vec3f shockwavePos;
                f32 sp2C;

                shockwavePos.y = func_8083973C(play, this, &D_80854A40, &shockwavePos);
                sp2C = this->actor.world.pos.y - shockwavePos.y;

                Math_ScaledStepToS(&this->actor.focus.rot.x, Math_Atan2S(45.0f, sp2C), 800);
                func_80836AB8(this, true);

                if ((((this->meleeWeaponAnimation == PLAYER_MWA_HAMMER_FORWARD) && LinkAnimation_OnFrame(&this->skelAnime, 7.0f)) ||
                     ((this->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH) && LinkAnimation_OnFrame(&this->skelAnime, 2.0f))) &&
                    (sp2C > -40.0f) && (sp2C < 40.0f)) {
                    func_80842A28(play, this);
                    EffectSsBlast_SpawnWhiteShockwave(play, &shockwavePos, &zeroVec, &zeroVec);
                }
            }
        }
    }
}

void Player_Action_808505DC(Player* this, PlayState* play) {
    LinkAnimation_Update(play, &this->skelAnime);
    Player_DecelerateToZero(this);

    if (this->skelAnime.curFrame >= 6.0f) {
        func_80839FFC(this, play);
    }
}

void Player_Action_8085063C(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    LinkAnimation_Update(play, &this->skelAnime);
    Player_UpdateUpperBody(this, play);

    if (this->av2.actionVar2 == 0) {
        Message_StartTextbox(play, 0x3B, &this->actor);
        this->av2.actionVar2 = 1;
        return;
    }

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        s32 respawnData = gSaveContext.respawn[RESPAWN_MODE_TOP].data;

        if (play->msgCtx.choiceIndex == 0) { //Returns to FW
            gSaveContext.respawnFlag = 3;
            play->transitionTrigger = TRANS_TRIGGER_START;
            play->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_TOP].entranceIndex;
            play->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
            func_80088AF0(play);
            return;
        }

        if (play->msgCtx.choiceIndex == 1) { //Unsets FW
            gSaveContext.respawn[RESPAWN_MODE_TOP].data = -respawnData;
            gSaveContext.fw.set = 0;
            func_80078914(&gSaveContext.respawn[RESPAWN_MODE_TOP].pos, NA_SE_PL_MAGIC_WIND_VANISH);
        }

        func_80853080(this, play);
        func_8005B1A4(Play_GetCamera(play, 0));
    }
}

void Player_Action_8085076C(Player* this, PlayState* play) {
    s32 respawnData = gSaveContext.respawn[RESPAWN_MODE_TOP].data;

    if (this->av2.actionVar2 > 20) {
        this->actor.draw = Player_Draw;
        this->actor.world.pos.y += 60.0f;
        func_80837B9C(this, play);
        return;
    }

    if (this->av2.actionVar2++ == 20) {
        gSaveContext.respawn[RESPAWN_MODE_TOP].data = respawnData + 1;
        func_80078914(&gSaveContext.respawn[RESPAWN_MODE_TOP].pos, NA_SE_PL_MAGIC_WIND_WARP);
    }
}

static LinkAnimationHeader* D_80854A58[] = {
    &gPlayerAnim_link_magic_kaze1,
    &gPlayerAnim_link_magic_honoo1,
    &gPlayerAnim_link_magic_tamashii1,
};

static LinkAnimationHeader* D_80854A64[] = {
    &gPlayerAnim_link_magic_kaze2,
    &gPlayerAnim_link_magic_honoo2,
    &gPlayerAnim_link_magic_tamashii2,
};

static LinkAnimationHeader* D_80854A70[] = {
    &gPlayerAnim_link_magic_kaze3,
    &gPlayerAnim_link_magic_honoo3,
    &gPlayerAnim_link_magic_tamashii3,
};

static u8 D_80854A7C[] = { 70, 10, 10 };

static AnimSfxEntry D_80854A80[] = {
    { NA_SE_PL_SKIP, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 20) },
    { NA_SE_VO_LI_SWORD_N, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 20) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 26) },
};

static AnimSfxEntry D_80854A8C[][2] = {
    {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 20) },
        { NA_SE_VO_LI_MAGIC_FROL, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 30) },
    },
    {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 20) },
        { NA_SE_VO_LI_MAGIC_NALE, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 44) },
    },
    {
        { NA_SE_VO_LI_MAGIC_ATTACK, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 20) },
        { NA_SE_IT_SWORD_SWING_HARD, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 20) },
    },
};

void Player_Action_808507F4(Player* this, PlayState* play) {
    u8 isFastFarores = CVarGetInteger(CVAR_ENHANCEMENT("FastFarores"), 0) && this->itemAction == PLAYER_IA_FARORES_WIND;
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->av1.actionVar1 < 0) {
            if ((this->itemAction == PLAYER_IA_NAYRUS_LOVE) || isFastFarores || (gSaveContext.magicState == MAGIC_STATE_IDLE)) {
                func_80839FFC(this, play);
                func_8005B1A4(Play_GetCamera(play, 0));
            }
        } else {
            if (this->av2.actionVar2 == 0) {
                LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, D_80854A58[this->av1.actionVar1], 0.83f * (isFastFarores ? 2 : 1));

                if (Player_SpawnMagicSpell(play, this, this->av1.actionVar1) != NULL) {
                    this->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE;
                    if ((this->av1.actionVar1 != 0) || (gSaveContext.respawn[RESPAWN_MODE_TOP].data <= 0)) {
                        gSaveContext.magicState = MAGIC_STATE_CONSUME_SETUP;
                    }
                } else {
                    Magic_Reset(play);
                }
            } else {
                LinkAnimation_PlayLoopSetSpeed(play, &this->skelAnime, D_80854A64[this->av1.actionVar1], 0.83f * (isFastFarores ? 2 : 1));

                if (this->av1.actionVar1 == 0) {
                    this->av2.actionVar2 = -10;
                }
            }

            this->av2.actionVar2++;
        }
    } else {
        if (this->av2.actionVar2 < 0) {
            this->av2.actionVar2++;

            if (this->av2.actionVar2 == 0) {
                gSaveContext.respawn[RESPAWN_MODE_TOP].data = 1;
                Play_SetupRespawnPoint(play, RESPAWN_MODE_TOP, 0x6FF);
                gSaveContext.fw.set = 1;
                gSaveContext.fw.pos.x = gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.x;
                gSaveContext.fw.pos.y = gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.y;
                gSaveContext.fw.pos.z = gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.z;
                gSaveContext.fw.yaw = gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw;
                gSaveContext.fw.playerParams = 0x6FF;
                gSaveContext.fw.entranceIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex;
                gSaveContext.fw.roomIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex;
                gSaveContext.fw.tempSwchFlags = gSaveContext.respawn[RESPAWN_MODE_DOWN].tempSwchFlags;
                gSaveContext.fw.tempCollectFlags = gSaveContext.respawn[RESPAWN_MODE_DOWN].tempCollectFlags;
                this->av2.actionVar2 = 2;
            }
        } else if (this->av1.actionVar1 >= 0) {
            if (this->av2.actionVar2 == 0) {
                Player_ProcessAnimSfxList(this, D_80854A80);
            } else if (this->av2.actionVar2 == 1) {
                Player_ProcessAnimSfxList(this, D_80854A8C[this->av1.actionVar1]);
                if ((this->av1.actionVar1 == 2) && LinkAnimation_OnFrame(&this->skelAnime, 30.0f)) {
                    this->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE);
                }
            } else if ((isFastFarores ? 10 : D_80854A7C[this->av1.actionVar1]) < this->av2.actionVar2++) {
                LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, D_80854A70[this->av1.actionVar1], 0.83f * (isFastFarores ? 2 : 1));
                this->yaw = this->actor.shape.rot.y;
                this->av1.actionVar1 = -1;
            }
        }
    }

    Player_DecelerateToZero(this);
}

void Player_Action_80850AEC(Player* this, PlayState* play) {
    f32 temp;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimPlayLoop(play, this, &gPlayerAnim_link_hook_fly_wait);
    }

    Math_Vec3f_Sum(&this->actor.world.pos, &this->actor.velocity, &this->actor.world.pos);

    if (func_80834FBC(this)) {
        Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
        Player_ProcessSceneCollision(play, this);

        temp = this->actor.world.pos.y - this->actor.floorHeight;
        if (temp > 20.0f) {
            temp = 20.0f;
        }

        this->actor.world.rot.x = this->actor.shape.rot.x = 0;
        this->actor.world.pos.y -= temp;
        this->linearVelocity = 1.0f;
        this->actor.velocity.y = 0.0f;
        func_80837B9C(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_UNDERWATER;
        this->actor.bgCheckFlags |= 1;
        this->stateFlags1 |= PLAYER_STATE1_HOOKSHOT_FALLING;
        return;
    }

    if ((this->skelAnime.animation != &gPlayerAnim_link_hook_fly_start) || (4.0f <= this->skelAnime.curFrame)) {
        this->actor.gravity = 0.0f;
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->actor.world.rot.x, 0x800);
        Player_RequestRumble(this, 100, 2, 100, 0);
    }
}

void Player_Action_80850C68(Player* this, PlayState* play) {
    if ((this->av2.actionVar2 != 0) && ((this->unk_858 != 0.0f) || (this->unk_85C != 0.0f))) {
        // 144-byte buffer, declared as a u64 array for 8-byte alignment. LinkAnimation_BlendToMorph will round up
        // the buffer address to the nearest 16-byte alignment before passing it to AnimTaskQueue_AddLoadPlayerFrame,
        // and AnimTaskQueue_AddLoadPlayerFrame requires space for `sizeof(Vec3s) * limbCount + 2` bytes. Link's
        // skeleton has 22 limbs (including the root limb) so we need 134 bytes of space, plus 8 bytes of margin for
        // the 16-byte alignment operation.
        static u64 D_80858AD8[18];
        f32 updateScale = R_UPDATE_RATE * 0.5f;

        this->skelAnime.curFrame += this->skelAnime.playSpeed * updateScale;
        if (this->skelAnime.curFrame >= this->skelAnime.animLength) {
            this->skelAnime.curFrame -= this->skelAnime.animLength;
        }

        LinkAnimation_BlendToJoint(
            play, &this->skelAnime, &gPlayerAnim_link_fishing_wait, this->skelAnime.curFrame,
            (this->unk_858 < 0.0f) ? &gPlayerAnim_link_fishing_reel_left : &gPlayerAnim_link_fishing_reel_right, 5.0f,
            fabsf(this->unk_858), this->blendTable);
        LinkAnimation_BlendToMorph(
            play, &this->skelAnime, &gPlayerAnim_link_fishing_wait, this->skelAnime.curFrame,
            (this->unk_85C < 0.0f) ? &gPlayerAnim_link_fishing_reel_up : &gPlayerAnim_link_fishing_reel_down, 5.0f,
            fabsf(this->unk_85C), (Vec3s*)D_80858AD8);
        LinkAnimation_InterpJointMorph(play, &this->skelAnime, 0.5f);
    } else if (LinkAnimation_Update(play, &this->skelAnime)) {
        this->unk_860 = 2;
        Player_AnimPlayLoop(play, this, &gPlayerAnim_link_fishing_wait);
        this->av2.actionVar2 = 1;
    }

    Player_DecelerateToZero(this);

    if (this->unk_860 == 0) {
        func_80853080(this, play);
    } else if (this->unk_860 == 3) {
        Player_SetupAction(play, this, Player_Action_80850E84, 0);
        Player_AnimChangeOnceMorph(play, this, &gPlayerAnim_link_fishing_fish_catch);
    }
}

void Player_Action_80850E84(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime) && (this->unk_860 == 0)) {
        func_8083A098(this, &gPlayerAnim_link_fishing_fish_catch_end, play);
    }
}

static void (*D_80854AA4[])(PlayState*, Player*, void*) = {
    NULL,          func_80851008, func_80851030, func_80851094, func_808510B4, func_808510D4, func_808510F4,
    func_80851114, func_80851134, func_80851154, func_80851174, func_808511D4, func_808511FC, func_80851294,
    func_80851050, func_80851194, func_808511B4, func_80851248, func_808512E0,
};

static AnimSfxEntry D_80854AF0[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 34) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 45) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 51) },
    { NA_SE_PL_CALM_HIT, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 64) },
};

static AnimSfxEntry D_80854B00[] = {
    { NA_SE_VO_LI_SURPRISE, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 3) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 15) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 24) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 30) },
    { NA_SE_VO_LI_FALL_L, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 31) },
};

static AnimSfxEntry D_80854B14[] = {
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 10) },
};

static struct_80854B18 D_80854B18[PLAYER_CSACTION_MAX] = {
    { 0, NULL },                                         // PLAYER_CSACTION_NONE
    { -1, func_808515A4 },                               // PLAYER_CSACTION_1
    { 2, &gPlayerAnim_link_demo_goma_furimuki },         // PLAYER_CSACTION_2
    { 0, NULL },                                         // PLAYER_CSACTION_3
    { 0, NULL },                                         // PLAYER_CSACTION_4
    { 3, &gPlayerAnim_link_demo_bikkuri },               // PLAYER_CSACTION_5
    { 0, NULL },                                         // PLAYER_CSACTION_6
    { 0, NULL },                                         // PLAYER_CSACTION_7
    { -1, func_808515A4 },                               // PLAYER_CSACTION_8
    { 2, &gPlayerAnim_link_demo_furimuki },              // PLAYER_CSACTION_9
    { -1, func_80851788 },                               // PLAYER_CSACTION_10
    { 3, &gPlayerAnim_link_demo_warp },                  // PLAYER_CSACTION_11
    { -1, func_808518DC },                               // PLAYER_CSACTION_12
    { 7, &gPlayerAnim_clink_demo_get1 },                 // PLAYER_CSACTION_13
    { 5, &gPlayerAnim_clink_demo_get2 },                 // PLAYER_CSACTION_14
    { 5, &gPlayerAnim_clink_demo_get3 },                 // PLAYER_CSACTION_15
    { 5, &gPlayerAnim_clink_demo_standup },              // PLAYER_CSACTION_16
    { 7, &gPlayerAnim_clink_demo_standup_wait },         // PLAYER_CSACTION_17
    { -1, func_808519EC },                               // PLAYER_CSACTION_18
    { 2, &gPlayerAnim_link_demo_baru_op1 },              // PLAYER_CSACTION_19
    { 2, &gPlayerAnim_link_demo_baru_op3 },              // PLAYER_CSACTION_20
    { 0, NULL },                                         // PLAYER_CSACTION_21
    { -1, func_80851B90 },                               // PLAYER_CSACTION_22
    { 3, &gPlayerAnim_link_demo_jibunmiru },             // PLAYER_CSACTION_23
    { 9, &gPlayerAnim_link_normal_back_downA },          // PLAYER_CSACTION_24
    { 2, &gPlayerAnim_link_normal_back_down_wake },      // PLAYER_CSACTION_25
    { -1, func_80851D2C },                               // PLAYER_CSACTION_26
    { 2, &gPlayerAnim_link_normal_okarina_end },         // PLAYER_CSACTION_27
    { 3, &gPlayerAnim_link_demo_get_itemA },             // PLAYER_CSACTION_28
    { -1, func_808515A4 },                               // PLAYER_CSACTION_29
    { 2, &gPlayerAnim_link_normal_normal2fighter_free }, // PLAYER_CSACTION_30
    { 0, NULL },                                         // PLAYER_CSACTION_31
    { 0, NULL },                                         // PLAYER_CSACTION_32
    { 5, &gPlayerAnim_clink_demo_atozusari },            // PLAYER_CSACTION_33
    { -1, func_80851368 },                               // PLAYER_CSACTION_34
    { -1, func_80851E64 },                               // PLAYER_CSACTION_35
    { 5, &gPlayerAnim_clink_demo_bashi },                // PLAYER_CSACTION_36
    { 16, &gPlayerAnim_link_normal_hang_up_down },       // PLAYER_CSACTION_37
    { -1, func_80851F84 },                               // PLAYER_CSACTION_38
    { -1, func_80851E90 },                               // PLAYER_CSACTION_39
    { 6, &gPlayerAnim_clink_op3_okiagari },              // PLAYER_CSACTION_40
    { 6, &gPlayerAnim_clink_op3_tatiagari },             // PLAYER_CSACTION_41
    { -1, func_80852080 },                               // PLAYER_CSACTION_42
    { 5, &gPlayerAnim_clink_demo_miokuri },              // PLAYER_CSACTION_43
    { -1, func_808521F4 },                               // PLAYER_CSACTION_44
    { -1, func_8085225C },                               // PLAYER_CSACTION_45
    { -1, func_80852280 },                               // PLAYER_CSACTION_46
    { 5, &gPlayerAnim_clink_demo_nozoki },               // PLAYER_CSACTION_47
    { 5, &gPlayerAnim_clink_demo_koutai },               // PLAYER_CSACTION_48
    { -1, func_808515A4 },                               // PLAYER_CSACTION_49
    { 5, &gPlayerAnim_clink_demo_koutai_kennuki },       // PLAYER_CSACTION_50
    { 5, &gPlayerAnim_link_demo_kakeyori },              // PLAYER_CSACTION_51
    { 5, &gPlayerAnim_link_demo_kakeyori_mimawasi },     // PLAYER_CSACTION_52
    { 5, &gPlayerAnim_link_demo_kakeyori_miokuri },      // PLAYER_CSACTION_53
    { 3, &gPlayerAnim_link_demo_furimuki2 },             // PLAYER_CSACTION_54
    { 3, &gPlayerAnim_link_demo_kaoage },                // PLAYER_CSACTION_55
    { 4, &gPlayerAnim_link_demo_kaoage_wait },           // PLAYER_CSACTION_56
    { 3, &gPlayerAnim_clink_demo_mimawasi },             // PLAYER_CSACTION_57
    { 3, &gPlayerAnim_link_demo_nozokikomi },            // PLAYER_CSACTION_58
    { 6, &gPlayerAnim_kolink_odoroki_demo },             // PLAYER_CSACTION_59
    { 6, &gPlayerAnim_link_shagamu_demo },               // PLAYER_CSACTION_60
    { 14, &gPlayerAnim_link_okiru_demo },                // PLAYER_CSACTION_61
    { 3, &gPlayerAnim_link_okiru_demo },                 // PLAYER_CSACTION_62
    { 5, &gPlayerAnim_link_fighter_power_kiru_start },   // PLAYER_CSACTION_63
    { 16, &gPlayerAnim_demo_link_nwait },                // PLAYER_CSACTION_64
    { 15, &gPlayerAnim_demo_link_tewatashi },            // PLAYER_CSACTION_65
    { 15, &gPlayerAnim_demo_link_orosuu },               // PLAYER_CSACTION_66
    { 3, &gPlayerAnim_d_link_orooro },                   // PLAYER_CSACTION_67
    { 3, &gPlayerAnim_d_link_imanodare },                // PLAYER_CSACTION_68
    { 3, &gPlayerAnim_link_hatto_demo },                 // PLAYER_CSACTION_69
    { 6, &gPlayerAnim_o_get_mae },                       // PLAYER_CSACTION_70
    { 6, &gPlayerAnim_o_get_ato },                       // PLAYER_CSACTION_71
    { 6, &gPlayerAnim_om_get_mae },                      // PLAYER_CSACTION_72
    { 6, &gPlayerAnim_nw_modoru },                       // PLAYER_CSACTION_73
    { 3, &gPlayerAnim_link_demo_gurad },                 // PLAYER_CSACTION_74
    { 3, &gPlayerAnim_link_demo_look_hand },             // PLAYER_CSACTION_75
    { 4, &gPlayerAnim_link_demo_sita_wait },             // PLAYER_CSACTION_76
    { 3, &gPlayerAnim_link_demo_ue },                    // PLAYER_CSACTION_77
    { 3, &gPlayerAnim_Link_muku },                       // PLAYER_CSACTION_78
    { 3, &gPlayerAnim_Link_miageru },                    // PLAYER_CSACTION_79
    { 6, &gPlayerAnim_Link_ha },                         // PLAYER_CSACTION_80
    { 3, &gPlayerAnim_L_1kyoro },                        // PLAYER_CSACTION_81
    { 3, &gPlayerAnim_L_2kyoro },                        // PLAYER_CSACTION_82
    { 3, &gPlayerAnim_L_sagaru },                        // PLAYER_CSACTION_83
    { 3, &gPlayerAnim_L_bouzen },                        // PLAYER_CSACTION_84
    { 3, &gPlayerAnim_L_kamaeru },                       // PLAYER_CSACTION_85
    { 3, &gPlayerAnim_L_hajikareru },                    // PLAYER_CSACTION_86
    { 3, &gPlayerAnim_L_ken_miru },                      // PLAYER_CSACTION_87
    { 3, &gPlayerAnim_L_mukinaoru },                     // PLAYER_CSACTION_88
    { -1, func_808524B0 },                               // PLAYER_CSACTION_89
    { 3, &gPlayerAnim_link_wait_itemD1_20f },            // PLAYER_CSACTION_90
    { -1, func_80852544 },                               // PLAYER_CSACTION_91
    { -1, func_80852564 },                               // PLAYER_CSACTION_92
    { 3, &gPlayerAnim_link_normal_wait_typeB_20f },      // PLAYER_CSACTION_93
    { -1, func_80852608 },                               // PLAYER_CSACTION_94
    { 3, &gPlayerAnim_link_demo_kousan },                // PLAYER_CSACTION_95
    { 3, &gPlayerAnim_link_demo_return_to_past },        // PLAYER_CSACTION_96
    { 3, &gPlayerAnim_link_last_hit_motion1 },           // PLAYER_CSACTION_97
    { 3, &gPlayerAnim_link_last_hit_motion2 },           // PLAYER_CSACTION_98
    { 3, &gPlayerAnim_link_demo_zeldamiru },             // PLAYER_CSACTION_99
    { 3, &gPlayerAnim_link_demo_kenmiru1 },              // PLAYER_CSACTION_100
    { 3, &gPlayerAnim_link_demo_kenmiru2 },              // PLAYER_CSACTION_101
    { 3, &gPlayerAnim_link_demo_kenmiru2_modori },       // PLAYER_CSACTION_102
};

static struct_80854B18 D_80854E50[PLAYER_CSACTION_MAX] = {
    { 0, NULL },                                          // PLAYER_CSACTION_NONE
    { -1, func_808514C0 },                                // PLAYER_CSACTION_1
    { -1, func_8085157C },                                // PLAYER_CSACTION_2
    { -1, func_80851998 },                                // PLAYER_CSACTION_3
    { -1, func_808519C0 },                                // PLAYER_CSACTION_4
    { 11, NULL },                                         // PLAYER_CSACTION_5
    { -1, func_80852C50 },                                // PLAYER_CSACTION_6
    { -1, func_80852944 },                                // PLAYER_CSACTION_7
    { -1, func_80851688 },                                // PLAYER_CSACTION_8
    { -1, func_80851750 },                                // PLAYER_CSACTION_9
    { -1, func_80851828 },                                // PLAYER_CSACTION_10
    { -1, func_808521B8 },                                // PLAYER_CSACTION_11
    { -1, func_8085190C },                                // PLAYER_CSACTION_12
    { 11, NULL },                                         // PLAYER_CSACTION_13
    { 11, NULL },                                         // PLAYER_CSACTION_14
    { 11, NULL },                                         // PLAYER_CSACTION_15
    { 18, D_80854AF0 },                                   // PLAYER_CSACTION_16
    { 11, NULL },                                         // PLAYER_CSACTION_17
    { -1, func_80851A50 },                                // PLAYER_CSACTION_18
    { 12, &gPlayerAnim_link_demo_baru_op2 },              // PLAYER_CSACTION_19
    { 11, NULL },                                         // PLAYER_CSACTION_20
    { 0, NULL },                                          // PLAYER_CSACTION_21
    { -1, func_80851BE8 },                                // PLAYER_CSACTION_22
    { 11, NULL },                                         // PLAYER_CSACTION_23
    { -1, func_80851CA4 },                                // PLAYER_CSACTION_24
    { 11, NULL },                                         // PLAYER_CSACTION_25
    { 17, &gPlayerAnim_link_normal_okarina_swing },       // PLAYER_CSACTION_26
    { 11, NULL },                                         // PLAYER_CSACTION_27
    { 11, NULL },                                         // PLAYER_CSACTION_28
    { 11, NULL },                                         // PLAYER_CSACTION_29
    { -1, func_80851D80 },                                // PLAYER_CSACTION_30
    { -1, func_80851DEC },                                // PLAYER_CSACTION_31
    { -1, func_80851E28 },                                // PLAYER_CSACTION_32
    { 18, D_80854B00 },                                   // PLAYER_CSACTION_33
    { -1, func_808513BC },                                // PLAYER_CSACTION_34
    { 11, NULL },                                         // PLAYER_CSACTION_35
    { 11, NULL },                                         // PLAYER_CSACTION_36
    { 11, NULL },                                         // PLAYER_CSACTION_37
    { 11, NULL },                                         // PLAYER_CSACTION_38
    { -1, func_80851ECC },                                // PLAYER_CSACTION_39
    { -1, func_80851FB0 },                                // PLAYER_CSACTION_40
    { -1, func_80852048 },                                // PLAYER_CSACTION_41
    { -1, func_80852174 },                                // PLAYER_CSACTION_42
    { 13, &gPlayerAnim_clink_demo_miokuri_wait },         // PLAYER_CSACTION_43
    { -1, func_80852234 },                                // PLAYER_CSACTION_44
    { 0, NULL },                                          // PLAYER_CSACTION_45
    { 0, NULL },                                          // PLAYER_CSACTION_46
    { 11, NULL },                                         // PLAYER_CSACTION_47
    { -1, func_80852450 },                                // PLAYER_CSACTION_48
    { -1, func_80851688 },                                // PLAYER_CSACTION_49
    { -1, func_80852298 },                                // PLAYER_CSACTION_50
    { 13, &gPlayerAnim_link_demo_kakeyori_wait },         // PLAYER_CSACTION_51
    { -1, func_80852480 },                                // PLAYER_CSACTION_52
    { 13, &gPlayerAnim_link_demo_kakeyori_miokuri_wait }, // PLAYER_CSACTION_53
    { -1, func_80852328 },                                // PLAYER_CSACTION_54
    { 11, NULL },                                         // PLAYER_CSACTION_55
    { 11, NULL },                                         // PLAYER_CSACTION_56
    { 12, &gPlayerAnim_clink_demo_mimawasi_wait },        // PLAYER_CSACTION_57
    { -1, func_80852358 },                         // PLAYER_CSACTION_58
    { 11, NULL },                                  // PLAYER_CSACTION_59
    { 18, D_80854B14 },                            // PLAYER_CSACTION_60
    { 11, NULL },                                  // PLAYER_CSACTION_61
    { 11, NULL },                                  // PLAYER_CSACTION_62
    { 11, NULL },                                  // PLAYER_CSACTION_63
    { 11, NULL },                                  // PLAYER_CSACTION_64
    { -1, func_80852388 },                         // PLAYER_CSACTION_65
    { 17, &gPlayerAnim_demo_link_nwait },          // PLAYER_CSACTION_66
    { 12, &gPlayerAnim_d_link_orowait },           // PLAYER_CSACTION_67
    { 12, &gPlayerAnim_demo_link_nwait },          // PLAYER_CSACTION_68
    { 11, NULL },                                  // PLAYER_CSACTION_69
    { -1, func_808526EC },                         // PLAYER_CSACTION_70
    { 17, &gPlayerAnim_sude_nwait },               // PLAYER_CSACTION_71
    { -1, func_808526EC },                         // PLAYER_CSACTION_72
    { 17, &gPlayerAnim_sude_nwait },               // PLAYER_CSACTION_73
    { 12, &gPlayerAnim_link_demo_gurad_wait },     // PLAYER_CSACTION_74
    { 12, &gPlayerAnim_link_demo_look_hand_wait }, // PLAYER_CSACTION_75
    { 11, NULL },                                  // PLAYER_CSACTION_76
    { 12, &gPlayerAnim_link_demo_ue_wait },        // PLAYER_CSACTION_77
    { 12, &gPlayerAnim_Link_m_wait },              // PLAYER_CSACTION_78
    { 13, &gPlayerAnim_Link_ue_wait },             // PLAYER_CSACTION_79
    { 12, &gPlayerAnim_Link_otituku_w },           // PLAYER_CSACTION_80
    { 12, &gPlayerAnim_L_kw },                     // PLAYER_CSACTION_81
    { 11, NULL },                                  // PLAYER_CSACTION_82
    { 11, NULL },                                  // PLAYER_CSACTION_83
    { 11, NULL },                                  // PLAYER_CSACTION_84
    { 11, NULL },                                  // PLAYER_CSACTION_85
    { -1, func_80852648 },                         // PLAYER_CSACTION_86
    { 11, NULL },                                  // PLAYER_CSACTION_87
    { 12, &gPlayerAnim_L_kennasi_w },              // PLAYER_CSACTION_88
    { -1, func_808524D0 },                         // PLAYER_CSACTION_89
    { -1, func_80852514 },                         // PLAYER_CSACTION_90
    { -1, func_80852554 },                         // PLAYER_CSACTION_91
    { -1, func_808525C0 },                         // PLAYER_CSACTION_92
    { 11, NULL },                                  // PLAYER_CSACTION_93
    { 11, NULL },                                  // PLAYER_CSACTION_94
    { 11, NULL },                                  // PLAYER_CSACTION_95
    { -1, func_8085283C },                         // PLAYER_CSACTION_96
    { -1, func_808528C8 },                         // PLAYER_CSACTION_97
    { -1, func_808528C8 },                         // PLAYER_CSACTION_98
    { 12, &gPlayerAnim_link_demo_zeldamiru_wait }, // PLAYER_CSACTION_99
    { 12, &gPlayerAnim_link_demo_kenmiru1_wait },  // PLAYER_CSACTION_100
    { 12, &gPlayerAnim_link_demo_kenmiru2_wait },  // PLAYER_CSACTION_101
    { 12, &gPlayerAnim_demo_link_nwait },          // PLAYER_CSACTION_102
};

void Player_AnimChangeOnceMorphZeroRootYawSpeed(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Player_ZeroRootLimbYaw(this);
    Player_AnimChangeOnceMorph(play, this, anim);
    Player_ZeroSpeedXZ(this);
}

void Player_AnimChangeOnceMorphAdjustedZeroRootYawSpeed(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Player_ZeroRootLimbYaw(this);
    LinkAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, Animation_GetLastFrame(anim),
                         ANIMMODE_ONCE, -8.0f);
    Player_ZeroSpeedXZ(this);
}

void Player_AnimChangeLoopMorphAdjustedZeroRootYawSpeed(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Player_ZeroRootLimbYaw(this);
    LinkAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f);
    Player_ZeroSpeedXZ(this);
}

void func_80851008(PlayState* play, Player* this, void* anim) {
    Player_ZeroSpeedXZ(this);
}

void func_80851030(PlayState* play, Player* this, void* anim) {
    Player_AnimChangeOnceMorphZeroRootYawSpeed(play, this, anim);
}

void func_80851050(PlayState* play, Player* this, void* anim) {
    Player_ZeroRootLimbYaw(this);
    Player_AnimChangeFreeze(play, this, anim);
    Player_ZeroSpeedXZ(this);
}

void func_80851094(PlayState* play, Player* this, void* anim) {
    Player_AnimChangeOnceMorphAdjustedZeroRootYawSpeed(play, this, anim);
}

void func_808510B4(PlayState* play, Player* this, void* anim) {
    Player_AnimChangeLoopMorphAdjustedZeroRootYawSpeed(play, this, anim);
}

void func_808510D4(PlayState* play, Player* this, void* anim) {
    Player_AnimReplaceNormalPlayOnceAdjusted(play, this, anim);
}

void func_808510F4(PlayState* play, Player* this, void* anim) {
    Player_AnimReplacePlayOnce(play, this, anim, 0x9C);
}

void func_80851114(PlayState* play, Player* this, void* anim) {
    Player_AnimReplaceNormalPlayLoopAdjusted(play, this, anim);
}

void func_80851134(PlayState* play, Player* this, void* anim) {
    Player_AnimReplacePlayLoop(play, this, anim, 0x9C);
}

void func_80851154(PlayState* play, Player* this, void* anim) {
    Player_AnimPlayOnce(play, this, anim);
}

void func_80851174(PlayState* play, Player* this, void* anim) {
    Player_AnimPlayLoop(play, this, anim);
}

void func_80851194(PlayState* play, Player* this, void* anim) {
    Player_AnimPlayOnceAdjusted(play, this, anim);
}

void func_808511B4(PlayState* play, Player* this, void* anim) {
    Player_AnimPlayLoopAdjusted(play, this, anim);
}

void func_808511D4(PlayState* play, Player* this, void* anim) {
    LinkAnimation_Update(play, &this->skelAnime);
}

void func_808511FC(PlayState* play, Player* this, void* anim) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimChangeLoopMorphAdjustedZeroRootYawSpeed(play, this, anim);
        this->av2.actionVar2 = 1;
    }
}

void func_80851248(PlayState* play, Player* this, void* anim) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_FinishAnimMovement(this);
        Player_AnimPlayLoopAdjusted(play, this, anim);
    }
}

void func_80851294(PlayState* play, Player* this, void* anim) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplaceNormalPlayLoopAdjusted(play, this, anim);
        this->av2.actionVar2 = 1;
    }
}

void func_808512E0(PlayState* play, Player* this, void* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);
    Player_ProcessAnimSfxList(this, arg2);
}

void func_80851314(Player* this) {
    if ((this->csActor == NULL) || (this->csActor->update == NULL)) {
        this->csActor = NULL;
    }

    this->focusActor = this->csActor;

    if (this->focusActor != NULL) {
        this->actor.shape.rot.y = func_8083DB98(this, 0);
    }
}

void func_80851368(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->stateFlags1 |= PLAYER_STATE1_IN_WATER;
    this->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
    this->stateFlags1 &= ~(PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALL);

    Player_AnimPlayLoop(play, this, &gPlayerAnim_link_swimer_swim);
}

void func_808513BC(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.gravity = 0.0f;

    if (this->av1.actionVar1 == 0) {
        if (func_8083D12C(play, this, NULL)) {
            this->av1.actionVar1 = 1;
        } else {
            func_8084B158(play, this, NULL, fabsf(this->actor.velocity.y));
            Math_ScaledStepToS(&this->unk_6C2, -10000, 800);
            func_8084AEEC(this, &this->actor.velocity.y, 4.0f, this->yaw);
        }
        return;
    }

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->av1.actionVar1 == 1) {
            Player_AnimChangeLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
        } else {
            Player_AnimPlayLoop(play, this, &gPlayerAnim_link_swimer_swim_wait);
        }
    }

    func_8084B000(this);
    func_8084AEEC(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

void func_808514C0(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80851314(this);

    if (func_808332B8(this)) {
        func_808513BC(play, this, 0);
        return;
    }

    LinkAnimation_Update(play, &this->skelAnime);

    if (func_8008F128(this) || (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
        Player_UpdateUpperBody(this, play);
        return;
    }

    if ((this->interactRangeActor != NULL) && (this->interactRangeActor->textId == 0xFFFF)) {
        Player_ActionHandler_2(this, play);
    }
}

void func_8085157C(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimation_Update(play, &this->skelAnime);
}

void func_808515A4(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimationHeader* anim;

    if (func_808332B8(this)) {
        func_80851368(play, this, 0);
        return;
    }

    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_nwait, this->modelAnimType);

    if ((this->cueId == 6) || (this->cueId == 0x2E)) {
        Player_AnimPlayOnce(play, this, anim);
    } else {
        Player_ZeroRootLimbYaw(this);
        LinkAnimation_Change(play, &this->skelAnime, anim, (2.0f / 3.0f), 0.0f, Animation_GetLastFrame(anim),
                             ANIMMODE_LOOP, -4.0f);
    }

    Player_ZeroSpeedXZ(this);
}

void func_80851688(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (func_8084B3CC(play, this) == 0) {
        if ((this->csAction == 0x31) && (play->csCtx.state == CS_STATE_IDLE)) {
            Player_SetCsActionWithHaltedActors(play, NULL, 7);
            return;
        }

        if (func_808332B8(this) != 0) {
            func_808513BC(play, this, 0);
            return;
        }

        LinkAnimation_Update(play, &this->skelAnime);

        if (func_8008F128(this) || (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            Player_UpdateUpperBody(this, play);
        }
    }
}

static AnimSfxEntry D_80855188[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 42) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 48) },
};

void func_80851750(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimation_Update(play, &this->skelAnime);
    Player_ProcessAnimSfxList(this, D_80855188);
}

void func_80851788(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->stateFlags1 &= ~PLAYER_STATE1_BOOMERANG_THROWN;

    this->yaw = this->actor.shape.rot.y = this->actor.world.rot.y =
        Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_450);

    if (this->linearVelocity <= 0.0f) {
        this->linearVelocity = 0.1f;
    } else if (this->linearVelocity > 2.5f) {
        this->linearVelocity = 2.5f;
    }
}

void func_80851828(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 sp1C = 2.5f;

    func_80845BA0(play, this, &sp1C, 10);

    if (play->sceneNum == SCENE_JABU_JABU_BOSS) {
        if (this->av2.actionVar2 == 0) {
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                return;
            }
        } else {
            if (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE) {
                return;
            }
        }
    }

    this->av2.actionVar2++;
    if (this->av2.actionVar2 > 20) {
        this->csAction = 0xB;
    }
}

void func_808518DC(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_8083CEAC(this, play);
}

void func_8085190C(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80851314(this);

    if (this->av2.actionVar2 != 0) {
        if (LinkAnimation_Update(play, &this->skelAnime)) {
            Player_AnimPlayLoop(play, this, func_808334E4(this));
            this->av2.actionVar2 = 0;
        }

        func_80833C3C(this);
    } else {
        func_808401B0(play, this);
    }
}

void func_80851998(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80845964(play, this, cue, 0.0f, 0, 0);
}

void func_808519C0(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80845964(play, this, cue, 0.0f, 0, 1);
}

// unused
static LinkAnimationHeader* D_80855190[] = {
    &gPlayerAnim_link_demo_back_to_past,
    &gPlayerAnim_clink_demo_goto_future,
};

static Vec3f D_80855198 = { -1.0f, 70.0f, 20.0f };

void func_808519EC(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Math_Vec3f_Copy(&this->actor.world.pos, &D_80855198);
    this->actor.shape.rot.y = -0x8000;
    Player_AnimPlayOnceAdjusted(play, this, this->ageProperties->unk_9C);
    Player_StartAnimMovement(play, this, 0x28F);
}

static struct_808551A4 D_808551A4[] = {
    { NA_SE_IT_SWORD_PUTAWAY_STN, 0 },
    { NA_SE_IT_SWORD_STICK_STN, NA_SE_VO_LI_SWORD_N },
};

static AnimSfxEntry D_808551AC[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 29) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 39) },
};

void func_80851A50(PlayState* play, Player* this, CsCmdActorCue* cue) {
    struct_808551A4* sp2C;
    Gfx** dLists;

    LinkAnimation_Update(play, &this->skelAnime);

    if ((LINK_IS_ADULT && LinkAnimation_OnFrame(&this->skelAnime, 70.0f)) ||
        (!LINK_IS_ADULT && LinkAnimation_OnFrame(&this->skelAnime, 87.0f))) {
        sp2C = &D_808551A4[gSaveContext.linkAge];
        this->interactRangeActor->parent = &this->actor;

        if (!LINK_IS_ADULT) {
            dLists = gPlayerLeftHandBgsDLs;
        } else {
            dLists = gPlayerLeftHandClosedDLs;
        }
        this->leftHandDLists = &dLists[gSaveContext.linkAge];

        Player_PlaySfx(this, sp2C->unk_00);
        if (!LINK_IS_ADULT) {
            Player_PlayVoiceSfx(this, sp2C->unk_02);
        }
    } else if (LINK_IS_ADULT) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 66.0f)) {
            Player_PlayVoiceSfx(this, NA_SE_VO_LI_SWORD_L);
        }
    } else {
        Player_ProcessAnimSfxList(this, D_808551AC);
    }
}

void func_80851B90(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_demo_warp, -(2.0f / 3.0f), 12.0f, 12.0f,
                         ANIMMODE_ONCE, 0.0f);
}

static AnimSfxEntry D_808551B4[] = {
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 30) },
};

void func_80851BE8(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimation_Update(play, &this->skelAnime);

    this->av2.actionVar2++;

    if (this->av2.actionVar2 >= 180) {
        if (this->av2.actionVar2 == 180) {
            LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_okarina_warp_goal, (2.0f / 3.0f), 10.0f,
                                 Animation_GetLastFrame(&gPlayerAnim_link_okarina_warp_goal), ANIMMODE_ONCE, -8.0f);
        }
        Player_ProcessAnimSfxList(this, D_808551B4);
    }
}

void func_80851CA4(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (LinkAnimation_Update(play, &this->skelAnime) && (this->av2.actionVar2 == 0) && (this->actor.bgCheckFlags & 1)) {
        Player_AnimPlayOnce(play, this, &gPlayerAnim_link_normal_back_downB);
        this->av2.actionVar2 = 1;
    }

    if (this->av2.actionVar2 != 0) {
        Player_DecelerateToZero(this);
    }
}

void func_80851D2C(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_AnimChangeOnceMorphAdjustedZeroRootYawSpeed(play, this, &gPlayerAnim_link_normal_okarina_start);
    func_8084B498(this);
    Player_SetModels(this, Player_ActionToModelGroup(this, this->itemAction));
}

static AnimSfxEntry D_808551B8[] = {
    { NA_SE_IT_SWORD_PICKOUT, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 12) },
};

void func_80851D80(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimation_Update(play, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 6.0f)) {
        func_80846720(play, this, 0);
    } else {
        Player_ProcessAnimSfxList(this, D_808551B8);
    }
}

void func_80851DEC(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimation_Update(play, &this->skelAnime);
    Math_StepToS(&this->actor.shape.face, 0, 1);
}

void func_80851E28(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimation_Update(play, &this->skelAnime);
    Math_StepToS(&this->actor.shape.face, 2, 1);
}

void func_80851E64(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_AnimReplacePlayOnceAdjusted(play, this, &gPlayerAnim_link_swimer_swim_get, 0x98);
}

void func_80851E90(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_AnimReplacePlayOnce(play, this, &gPlayerAnim_clink_op3_negaeri, 0x9C);
    Player_PlayVoiceSfx(this, NA_SE_VO_LI_GROAN);
}

void func_80851ECC(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplacePlayLoop(play, this, &gPlayerAnim_clink_op3_wait2, 0x9C);
    }
}

void func_80851F14(PlayState* play, Player* this, LinkAnimationHeader* anim, AnimSfxEntry* arg3) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimPlayLoopAdjusted(play, this, anim);
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        Player_ProcessAnimSfxList(this, arg3);
    }
}

void func_80851F84(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.shape.shadowDraw = NULL;
    func_80851134(play, this, &gPlayerAnim_clink_op3_wait1);
}

static AnimSfxEntry D_808551BC[] = {
    { NA_SE_VO_LI_RELAX, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 35) },
    { NA_SE_PL_SLIPDOWN, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 236) },
    { NA_SE_PL_SLIPDOWN, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 256) },
};

void func_80851FB0(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplacePlayLoop(play, this, &gPlayerAnim_clink_op3_wait3, 0x9C);
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        Player_ProcessAnimSfxList(this, D_808551BC);
        if (LinkAnimation_OnFrame(&this->skelAnime, 240.0f)) {
            this->actor.shape.shadowDraw = ActorShadow_DrawFeet;
        }
    }
}

static AnimSfxEntry D_808551C8[] = {
    { NA_SE_PL_LAND_LADDER, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 67) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_UNKNOWN, 84) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_UNKNOWN, 90) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_UNKNOWN, 96) },
};

void func_80852048(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimation_Update(play, &this->skelAnime);
    Player_ProcessAnimSfxList(this, D_808551C8);
}

void func_80852080(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_AnimReplacePlayOnceAdjusted(play, this, &gPlayerAnim_clink_demo_futtobi, 0x9D);
    Player_PlayVoiceSfx(this, NA_SE_VO_LI_FALL_L);
}

void func_808520BC(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 startX = cue->startPos.x;
    f32 startY = cue->startPos.y;
    f32 startZ = cue->startPos.z;
    f32 distX = (cue->endPos.x - startX);
    f32 distY = (cue->endPos.y - startY);
    f32 distZ = (cue->endPos.z - startZ);
    f32 sp4 = (f32)(play->csCtx.frames - cue->startFrame) / (f32)(cue->endFrame - cue->startFrame);

    this->actor.world.pos.x = distX * sp4 + startX;
    this->actor.world.pos.y = distY * sp4 + startY;
    this->actor.world.pos.z = distZ * sp4 + startZ;
}

static AnimSfxEntry D_808551D8[] = {
    { NA_SE_PL_BOUND, ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 20) },
    { NA_SE_PL_BOUND, -ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 30) },
};

void func_80852174(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_808520BC(play, this, cue);
    LinkAnimation_Update(play, &this->skelAnime);
    Player_ProcessAnimSfxList(this, D_808551D8);
}

void func_808521B8(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (cue != NULL) {
        func_808520BC(play, this, cue);
    }
    LinkAnimation_Update(play, &this->skelAnime);
}

void func_808521F4(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_AnimChangeOnceMorph(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_nwait, this->modelAnimType));
    Player_ZeroSpeedXZ(this);
}

void func_80852234(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimation_Update(play, &this->skelAnime);
}

void func_8085225C(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_StartAnimMovement(play, this, 0x98);
}

void func_80852280(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.draw = Player_Draw;
}

void func_80852298(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplaceNormalPlayLoopAdjusted(play, this, &gPlayerAnim_clink_demo_koutai_wait);
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 10.0f)) {
            func_80846720(play, this, 1);
        }
    }
}

static AnimSfxEntry D_808551E0[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 10) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 24) },
};

void func_80852328(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80851F14(play, this, &gPlayerAnim_link_demo_furimuki2_wait, D_808551E0);
}

static AnimSfxEntry D_808551E8[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 15) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 35) },
};

void func_80852358(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80851F14(play, this, &gPlayerAnim_link_demo_nozokikomi_wait, D_808551E8);
}

void func_80852388(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_AnimPlayLoopAdjusted(play, this, &gPlayerAnim_demo_link_twait);
        this->av2.actionVar2 = 1;
    }

    if ((this->av2.actionVar2 != 0) && (play->csCtx.frames >= 900)) {
        this->rightHandType = PLAYER_MODELTYPE_LH_OPEN;
    } else {
        this->rightHandType = PLAYER_MODELTYPE_RH_FF;
    }
}

void func_80852414(PlayState* play, Player* this, LinkAnimationHeader* anim, AnimSfxEntry* arg3) {
    func_80851294(play, this, anim);
    if (this->av2.actionVar2 == 0) {
        Player_ProcessAnimSfxList(this, arg3);
    }
}

static AnimSfxEntry D_808551F0[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 15) },
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 33) },
};

void func_80852450(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80852414(play, this, &gPlayerAnim_clink_demo_koutai_wait, D_808551F0);
}

static AnimSfxEntry D_808551F8[] = {
    { NA_SE_PL_KNOCK, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 78) },
};

void func_80852480(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80852414(play, this, &gPlayerAnim_link_demo_kakeyori_wait, D_808551F8);
}

void func_808524B0(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80837704(play, this);
}

void func_808524D0(PlayState* play, Player* this, CsCmdActorCue* cue) {
    sControlInput->press.button |= BTN_B;

    Player_Action_80844E68(this, play);
}

void func_80852514(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_Action_80844E68(this, play);
}

void func_80852544(PlayState* play, Player* this, CsCmdActorCue* cue) {
}

void func_80852554(PlayState* play, Player* this, CsCmdActorCue* cue) {
}

void func_80852564(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->stateFlags3 |= PLAYER_STATE3_MIDAIR;
    this->linearVelocity = 2.0f;
    this->actor.velocity.y = -1.0f;

    Player_AnimPlayOnce(play, this, &gPlayerAnim_link_normal_back_downA);
    Player_PlayVoiceSfx(this, NA_SE_VO_LI_FALL_L);
}

static void (*D_808551FC[])(Player* this, PlayState* play) = {
    Player_Action_8084377C,
    Player_Action_80843954,
    Player_Action_80843A38,
};

void func_808525C0(PlayState* play, Player* this, CsCmdActorCue* cue) {
    D_808551FC[this->av2.actionVar2](this, play);
}

void func_80852608(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80846720(play, this, 0);
    Player_AnimPlayOnceAdjusted(play, this, &gPlayerAnim_link_demo_return_to_past);
}

void func_80852648(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimation_Update(play, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 10.0f)) {
        this->heldItemAction = this->itemAction = PLAYER_IA_NONE;
        this->heldItemId = ITEM_NONE;
        this->modelGroup = this->nextModelGroup = Player_ActionToModelGroup(this, PLAYER_IA_NONE);
        this->leftHandDLists = gPlayerLeftHandOpenDLs;
        
        // If MS sword is shuffled and not in the players inventory, then we need to unequip the current sword
        // and set swordless flag to mimic Link having his weapon knocked out of his hand in the Ganon fight
        if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_MASTER_SWORD) && !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER)) {
            Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_NONE);
            gSaveContext.equips.buttonItems[0] = ITEM_NONE;
            Flags_SetInfTable(INFTABLE_SWORDLESS);
            return;
        }
        
        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_MASTER);
        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_MASTER;
        Inventory_DeleteEquipment(play, EQUIP_TYPE_SWORD);
    }
}

static LinkAnimationHeader* D_80855208[] = {
    &gPlayerAnim_L_okarina_get,
    &gPlayerAnim_om_get,
};

static Vec3s D_80855210[2][2] = {
    { { -200, 700, 100 }, { 800, 600, 800 } },
    { { -200, 500, 0 }, { 600, 400, 600 } },
};

void func_808526EC(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 0 };
    static Color_RGBA8 envColor = { 0, 128, 128, 0 };
    s32 age = gSaveContext.linkAge;
    Vec3f sparklePos;
    Vec3f sp34;
    Vec3s* ptr;

    func_80851294(play, this, D_80855208[age]);

    if (this->rightHandType != PLAYER_MODELTYPE_RH_FF) {
        this->rightHandType = PLAYER_MODELTYPE_RH_FF;
        return;
    }

    ptr = D_80855210[gSaveContext.linkAge];

    sp34.x = ptr[0].x + Rand_CenteredFloat(ptr[1].x);
    sp34.y = ptr[0].y + Rand_CenteredFloat(ptr[1].y);
    sp34.z = ptr[0].z + Rand_CenteredFloat(ptr[1].z);

    SkinMatrix_Vec3fMtxFMultXYZ(&this->shieldMf, &sp34, &sparklePos);

    EffectSsKiraKira_SpawnDispersed(play, &sparklePos, &zeroVec, &zeroVec, &primColor, &envColor, 600, -10);
}

void func_8085283C(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_80852944(play, this, cue);
    // This is when link picks up the sword in the Ganon fight
    } else if (this->av2.actionVar2 == 0) {
        Item_Give(play, ITEM_SWORD_MASTER);
        func_80846720(play, this, 0);
    } else {
        func_8084E988(this);
    }
}

void func_808528C8(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_8084285C(this, 0.0f, 99.0f, this->skelAnime.endFrame - 8.0f);
    }

    if (this->heldItemAction != PLAYER_IA_SWORD_MASTER) {
        func_80846720(play, this, 1);
    }
}

void func_80852944(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (func_808332B8(this)) {
        func_80838F18(play, this);
        func_80832340(play, this);
    } else {
        func_8083C148(this, play);
        if (!Player_ActionHandler_Talk(this, play)) {
            Player_ActionHandler_2(this, play);
        }
    }

    this->csAction = 0;
    this->unk_6AD = 0;
}

void func_808529D0(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;
    if ((play->sceneNum == SCENE_KOKIRI_FOREST) && !LINK_IS_ADULT) {
        this->actor.world.pos.y -= 1.0f;
    }
    this->actor.world.pos.z = cue->startPos.z;
    this->yaw = this->actor.shape.rot.y = cue->rot.y;
}

void func_80852A54(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 dx = cue->startPos.x - (s32)this->actor.world.pos.x;
    f32 dy = cue->startPos.y - (s32)this->actor.world.pos.y;
    f32 dz = cue->startPos.z - (s32)this->actor.world.pos.z;
    f32 dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));
    s16 yawDiff = cue->rot.y - this->actor.shape.rot.y;

    if ((this->linearVelocity == 0.0f) && ((dist > 50.0f) || (ABS(yawDiff) > 0x4000))) {
        func_808529D0(play, this, cue);
    }

    this->skelAnime.moveFlags = 0;
    Player_ZeroRootLimbYaw(this);
}

void func_80852B4C(PlayState* play, Player* this, CsCmdActorCue* cue, struct_80854B18* arg3) {
    if (arg3->type > 0) {
        D_80854AA4[arg3->type](play, this, arg3->ptr);
    } else if (arg3->type < 0) {
        arg3->func(play, this, cue);
    }

    if ((D_80858AA0 & 4) && !(this->skelAnime.moveFlags & 4)) {
        this->skelAnime.morphTable[0].y /= this->ageProperties->unk_08;
        D_80858AA0 = 0;
    }
}

void func_80852C0C(PlayState* play, Player* this, s32 csAction) {
    if ((csAction != 1) && (csAction != 8) && (csAction != 0x31) && (csAction != 7)) {
        Player_DetachHeldActor(play, this);
    }
}

void func_80852C50(PlayState* play, Player* this, CsCmdActorCue* cue) {
    CsCmdActorCue* linkCsAction = play->csCtx.linkAction;
    s32 pad;
    s32 sp24;

    if (play->csCtx.state == CS_STATE_UNSKIPPABLE_INIT) {
        Player_SetCsActionWithHaltedActors(play, NULL, 7);
        this->cueId = 0;
        Player_ZeroSpeedXZ(this);
        return;
    }

    if (linkCsAction == NULL) {
        this->actor.flags &= ~ACTOR_FLAG_ACTIVE;
        return;
    }

    if (this->cueId != linkCsAction->action) {
        sp24 = sCueToCsActionMap[linkCsAction->action];
        if (sp24 >= 0) {
            if ((sp24 == 3) || (sp24 == 4)) {
                func_80852A54(play, this, linkCsAction);
            } else {
                func_808529D0(play, this, linkCsAction);
            }
        }

        D_80858AA0 = this->skelAnime.moveFlags;

        Player_FinishAnimMovement(this);
        osSyncPrintf("TOOL MODE=%d\n", sp24);
        func_80852C0C(play, this, ABS(sp24));
        func_80852B4C(play, this, linkCsAction, &D_80854B18[ABS(sp24)]);

        this->av2.actionVar2 = 0;
        this->av1.actionVar1 = 0;
        this->cueId = linkCsAction->action;
    }

    sp24 = sCueToCsActionMap[this->cueId];
    func_80852B4C(play, this, linkCsAction, &D_80854E50[ABS(sp24)]);

    if (CVarGetInteger(CVAR_ENHANCEMENT("FixEyesOpenWhileSleeping"), 0) && (play->csCtx.linkAction->action == 28 || play->csCtx.linkAction->action == 29)) {
        this->skelAnime.jointTable[22].x = 8;
    }
}

void Player_Action_CsAction(Player* this, PlayState* play) {
    if (this->csAction != this->prevCsAction) {
        D_80858AA0 = this->skelAnime.moveFlags;

        Player_FinishAnimMovement(this);
        this->prevCsAction = this->csAction;
        osSyncPrintf("DEMO MODE=%d\n", this->csAction);
        func_80852C0C(play, this, this->csAction);
        func_80852B4C(play, this, NULL, &D_80854B18[this->csAction]);
    }

    func_80852B4C(play, this, NULL, &D_80854E50[this->csAction]);
}

int Player_IsDroppingFish(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return (Player_Action_8084EFC0 == this->actionFunc) && (this->itemAction == PLAYER_IA_BOTTLE_FISH);
}

s32 Player_StartFishing(PlayState* play) {
    Player* this = GET_PLAYER(play);

    func_80832564(play, this);
    Player_UseItem(play, this, ITEM_FISHING_POLE);
    return 1;
}

s32 func_80852F38(PlayState* play, Player* this) {
    if (!Player_InBlockingCsMode(play, this) && (this->invincibilityTimer >= 0) && !func_8008F128(this) &&
        !(this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT)) {
        func_80832564(play, this);
        Player_SetupAction(play, this, Player_Action_8084F308, 0);
        Player_AnimPlayOnce(play, this, &gPlayerAnim_link_normal_re_dead_attack);
        this->stateFlags2 |= PLAYER_STATE2_GRABBED_BY_ENEMY;
        func_80832224(this);
        Player_PlayVoiceSfx(this, NA_SE_VO_LI_HELD);
        return true;
    }

    return false;
}

/**
 * Tries to starts a cutscene action specified by `csAction`.
 * A cutscene action will only start if player is not already in another form of cutscene.
 *
 * No actors will be halted over the duration of the cutscene action.
 *
 * @return  true if successful starting a `csAction`, false if not
 */
s32 Player_TryCsAction(PlayState* play, Actor* actor, s32 csAction) {
    Player* this = GET_PLAYER(play);

    if (!Player_InBlockingCsMode(play, this)) {
        func_80832564(play, this);
        Player_SetupAction(play, this, Player_Action_CsAction, 0);
        this->csAction = csAction;
        this->csActor = actor;
        func_80832224(this);
        return true;
    }

    return false;
}

void func_80853080(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_Idle, 1);
    Player_AnimChangeOnceMorph(play, this, Player_GetIdleAnim(this));
    this->yaw = this->actor.shape.rot.y;
}

s32 Player_InflictDamage(PlayState* play, s32 damage) {
    return Player_InflictDamageModified(play, damage, true);
}

s32 Player_InflictDamageModified(PlayState* play, s32 damage, u8 modified) {
    Player* this = GET_PLAYER(play);

    if (!Player_InBlockingCsMode(play, this) && !func_80837B18_modified(play, this, damage, modified)) {
        this->stateFlags2 &= ~PLAYER_STATE2_GRABBED_BY_ENEMY;
        return 1;
    }

    return 0;
}

/**
 * Start talking to the specified actor.
 *
 * This function does not concern trading exchange items.
 * For item exchanges see relevant code in `Player_ActionHandler_13` and `Player_Action_ExchangeItem`.
 */
void Player_StartTalking(PlayState* play, Actor* actor) {
    Player* this = GET_PLAYER(play);
    s32 pad;

    if ((this->talkActor != NULL) || (actor == this->naviActor) ||
        CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_NAVI_HAS_INFO)) {
        actor->flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
    }

    this->talkActor = actor;
    this->exchangeItemId = EXCH_ITEM_NONE;

    if (actor->textId == 0xFFFF) {
        // Player will stand and look at the actor with no text appearing.
        // This can be used to delay text from appearing, for example.
        Player_SetCsActionWithHaltedActors(play, actor, 1);
        actor->flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
        Player_PutAwayHeldItem(play, this);
    } else {
        if (this->actor.flags & ACTOR_FLAG_PLAYER_TALKED_TO) {
            this->actor.textId = 0;
        } else {
            this->actor.flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
            this->actor.textId = actor->textId;
        }

        if (this->stateFlags1 & PLAYER_STATE1_ON_HORSE) {
            s32 sp24 = this->av2.actionVar2;

            Player_PutAwayHeldItem(play, this);
            Player_SetupTalk(play, this);

            this->av2.actionVar2 = sp24;
        } else {
            if (func_808332B8(this)) {
                Player_SetupWaitForPutAway(play, this, Player_SetupTalk);
                Player_AnimChangeLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
            } else if ((actor->category != ACTORCAT_NPC) || (this->heldItemAction == PLAYER_IA_FISHING_POLE)) {
                Player_SetupTalk(play, this);

                if (!Player_CheckHostileLockOn(this)) {
                    if ((actor != this->naviActor) && (actor->xzDistToPlayer < 40.0f)) {
                        Player_AnimPlayOnceAdjusted(play, this, &gPlayerAnim_link_normal_backspace);
                    } else {
                        Player_AnimPlayLoop(play, this, Player_GetIdleAnim(this));
                    }
                }
            } else {
                Player_SetupWaitForPutAway(play, this, Player_SetupTalk);
                Player_AnimPlayOnceAdjusted(play, this,
                              (actor->xzDistToPlayer < 40.0f) ? &gPlayerAnim_link_normal_backspace
                                                              : &gPlayerAnim_link_normal_talk_free);
            }

            if (this->skelAnime.animation == &gPlayerAnim_link_normal_backspace) {
                Player_StartAnimMovement(play, this, 0x19);
            }

            func_80832224(this);
        }

        this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_IN_CUTSCENE;
    }

    if ((this->naviActor == this->talkActor) && ((this->talkActor->textId & 0xFF00) != 0x200)) {
        this->naviActor->flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
        func_80835EA4(play, 0xB);
    }
}
