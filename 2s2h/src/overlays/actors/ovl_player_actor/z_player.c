/*
 * File: z_player.c
 * Overlay: ovl_player_actor
 * Description: Player
 */

#include "global.h"
#include "z64horse.h"
#include "z64malloc.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "z64shrink_window.h"
#include <string.h>

#include "overlays/actors/ovl_Arms_Hook/z_arms_hook.h"
#include "overlays/actors/ovl_Door_Spiral/z_door_spiral.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Boom/z_en_boom.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"
#include "overlays/actors/ovl_En_Dnp/z_en_dnp.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/actors/ovl_En_Fish/z_en_fish.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"
#include "overlays/actors/ovl_En_Ishi/z_en_ishi.h"
#include "overlays/actors/ovl_En_Mushi2/z_en_mushi2.h"
#include "overlays/actors/ovl_En_Ot/z_en_ot.h"
#include "overlays/actors/ovl_En_Test3/z_en_test3.h"
#include "overlays/actors/ovl_En_Test5/z_en_test5.h"
#include "overlays/actors/ovl_En_Test7/z_en_test7.h"
#include "overlays/actors/ovl_En_Torch2/z_en_torch2.h"
#include "overlays/actors/ovl_En_Zoraegg/z_en_zoraegg.h"
#include "overlays/actors/ovl_Obj_Aqua/z_obj_aqua.h"

#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
#include "overlays/effects/ovl_Effect_Ss_G_Splash/z_eff_ss_g_splash.h"

#include "objects/gameplay_keep/gameplay_keep.h"

#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_goron/object_link_goron.h"
#include "objects/object_link_zora/object_link_zora.h"
#include "objects/object_link_nuts/object_link_nuts.h"
#include "objects/object_link_child/object_link_child.h"

#include "2s2h/BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include <libultraship/bridge/consolevariablebridge.h>

void Player_Init(Actor* thisx, PlayState* play);
void Player_Destroy(Actor* thisx, PlayState* play);
void Player_Update(Actor* thisx, PlayState* play);
void Player_Draw(Actor* thisx, PlayState* play);

s32 Player_GrabPlayer(PlayState* play, Player* this);
s32 Player_TryCsAction(PlayState* play, Player* this, PlayerCsAction csAction);
void func_8085B384(Player* this, PlayState* play);
s32 Player_InflictDamage(PlayState* play, s32 damage);
void Player_StartTalking(PlayState* play, Actor* actor);
void func_8085B74C(PlayState* play);
void func_8085B820(PlayState* play, s16 arg1);
PlayerItemAction func_8085B854(PlayState* play, Player* this, ItemId itemId);
s32 func_8085B930(PlayState* play, PlayerAnimationHeader* talkAnim, AnimationMode animMode);

void Player_UpdateCommon(Player* this, PlayState* play, Input* input);
s32 Player_StartFishing(PlayState* play);
void func_8085B170(PlayState* play, Player* this);
s32 func_8083A658(PlayState* play, Player* this);
void Player_InitItemAction(PlayState* play, Player* this, PlayerItemAction itemAction);

void Player_UseItem(PlayState* play, Player* this, ItemId item);

void func_80836988(Player* this, PlayState* play);

void func_808484F0(Player* this);

void func_80838A20(PlayState* play, Player* this);
void func_80839978(PlayState* play, Player* this);
void func_80839A10(PlayState* play, Player* this);

void func_80859CE0(PlayState* play, Player* this, s32 arg2);

void Player_Cutscene_SetPosAndYawToStart(Player* this, CsCmdActorCue* cue);

typedef enum AnimSfxType {
    /*  1 */ ANIMSFX_TYPE_GENERAL = 1,
    /*  2 */ ANIMSFX_TYPE_FLOOR,
    /*  3 */ ANIMSFX_TYPE_FLOOR_BY_AGE,
    /*  4 */ ANIMSFX_TYPE_VOICE,
    /*  5 */ ANIMSFX_TYPE_FLOOR_LAND, // does not use sfxId
    /*  6 */ ANIMSFX_TYPE_6,          // FLOOR_WALK_Something // does not use sfxId
    /*  7 */ ANIMSFX_TYPE_FLOOR_JUMP, // does not use sfxId
    /*  8 */ ANIMSFX_TYPE_8,          // FLOOR_WALK_Something2 // does not use sfxId
    /*  9 */ ANIMSFX_TYPE_9,          // Uses NA_SE_PL_WALK_LADDER // does not use sfxId, unused
    /* 10 */ ANIMSFX_TYPE_SURFACE
} AnimSfxType;

#define ANIMSFX_SHIFT_TYPE(type) ((type) << 11)

#define ANIMSFX_CONTINUE (1)
#define ANIMSFX_STOP (0)

#define ANIMSFX_FLAGS(type, frame, cont) \
    (((ANIMSFX_##cont) == ANIMSFX_STOP ? -1 : 1) * (ANIMSFX_SHIFT_TYPE(type) | ((frame)&0x7FF)))

#define ANIMSFX(type, frame, sfxId, cont) \
    { (sfxId), ANIMSFX_FLAGS(type, frame, cont) }

#define ANIMSFX_GET_TYPE(data) ((data)&0x7800)
#define ANIMSFX_GET_FRAME(data) ((data)&0x7FF)

typedef struct AnimSfxEntry {
    /* 0x0 */ u16 sfxId;
    /* 0x2 */ s16 flags; // negative marks the end
} AnimSfxEntry;          // size = 0x4

/* action funcs */
void Player_Action_OwlSaveArrive(Player* this, PlayState* play);
void Player_Action_1(Player* this, PlayState* play);
void Player_Action_2(Player* this, PlayState* play);
void Player_Action_3(Player* this, PlayState* play);
void Player_Action_Idle(Player* this, PlayState* play);
void Player_Action_5(Player* this, PlayState* play);
void Player_Action_6(Player* this, PlayState* play);
void Player_Action_7(Player* this, PlayState* play);
void Player_Action_8(Player* this, PlayState* play);
void Player_Action_9(Player* this, PlayState* play);
void Player_Action_TurnInPlace(Player* this, PlayState* play);
void Player_Action_11(Player* this, PlayState* play);
void Player_Action_12(Player* this, PlayState* play);
void Player_Action_13(Player* this, PlayState* play);
void Player_Action_14(Player* this, PlayState* play);
void Player_Action_15(Player* this, PlayState* play);
void Player_Action_16(Player* this, PlayState* play);
void Player_Action_17(Player* this, PlayState* play);
void Player_Action_18(Player* this, PlayState* play);
void Player_Action_19(Player* this, PlayState* play);
void Player_Action_20(Player* this, PlayState* play);
void Player_Action_21(Player* this, PlayState* play);
void Player_Action_22(Player* this, PlayState* play);
void Player_Action_23(Player* this, PlayState* play);
void Player_Action_24(Player* this, PlayState* play);
void Player_Action_25(Player* this, PlayState* play);
void Player_Action_26(Player* this, PlayState* play);
void Player_Action_27(Player* this, PlayState* play);
void Player_Action_28(Player* this, PlayState* play);
void Player_Action_29(Player* this, PlayState* play);
void Player_Action_30(Player* this, PlayState* play);
void Player_Action_31(Player* this, PlayState* play);
void Player_Action_32(Player* this, PlayState* play);
void Player_Action_33(Player* this, PlayState* play);
void Player_Action_WaitForPutAway(Player* this, PlayState* play);
void Player_Action_35(Player* this, PlayState* play);
void Player_Action_36(Player* this, PlayState* play);
void Player_Action_37(Player* this, PlayState* play);
void Player_Action_38(Player* this, PlayState* play);
void Player_Action_39(Player* this, PlayState* play);
void Player_Action_40(Player* this, PlayState* play);
void Player_Action_41(Player* this, PlayState* play);
void Player_Action_42(Player* this, PlayState* play);
void Player_Action_43(Player* this, PlayState* play);
void Player_Action_Talk(Player* this, PlayState* play);
void Player_Action_45(Player* this, PlayState* play);
void Player_Action_46(Player* this, PlayState* play);
void Player_Action_47(Player* this, PlayState* play);
void Player_Action_48(Player* this, PlayState* play);
void Player_Action_49(Player* this, PlayState* play);
void Player_Action_50(Player* this, PlayState* play);
void Player_Action_51(Player* this, PlayState* play);
void Player_Action_52(Player* this, PlayState* play);
void Player_Action_53(Player* this, PlayState* play);
void Player_Action_54(Player* this, PlayState* play);
void Player_Action_55(Player* this, PlayState* play);
void Player_Action_56(Player* this, PlayState* play);
void Player_Action_57(Player* this, PlayState* play);
void Player_Action_58(Player* this, PlayState* play);
void Player_Action_59(Player* this, PlayState* play);
void Player_Action_60(Player* this, PlayState* play);
void Player_Action_61(Player* this, PlayState* play);
void Player_Action_62(Player* this, PlayState* play);
void Player_Action_63(Player* this, PlayState* play);
void Player_Action_64(Player* this, PlayState* play);
void Player_Action_65(Player* this, PlayState* play);
void Player_Action_TimeTravelEnd(Player* this, PlayState* play);
void Player_Action_67(Player* this, PlayState* play);
void Player_Action_68(Player* this, PlayState* play);
void Player_Action_69(Player* this, PlayState* play);
void Player_Action_70(Player* this, PlayState* play);
void Player_Action_ExchangeItem(Player* this, PlayState* play);
void Player_Action_72(Player* this, PlayState* play);
void Player_Action_SlideOnSlope(Player* this, PlayState* play);
void Player_Action_WaitForCutscene(Player* this, PlayState* play);
void Player_Action_StartWarpSongArrive(Player* this, PlayState* play);
void Player_Action_BlueWarpArrive(Player* this, PlayState* play);
void Player_Action_77(Player* this, PlayState* play);
void Player_Action_TryOpeningDoor(Player* this, PlayState* play);
void Player_Action_ExitGrotto(Player* this, PlayState* play);
void Player_Action_80(Player* this, PlayState* play);
void Player_Action_81(Player* this, PlayState* play);
void Player_Action_82(Player* this, PlayState* play);
void Player_Action_83(Player* this, PlayState* play);
void Player_Action_84(Player* this, PlayState* play);
void Player_Action_85(Player* this, PlayState* play);
void Player_Action_86(Player* this, PlayState* play);
void Player_Action_87(Player* this, PlayState* play);
void Player_Action_88(Player* this, PlayState* play);
void Player_Action_89(Player* this, PlayState* play);
void Player_Action_90(Player* this, PlayState* play);
void Player_Action_91(Player* this, PlayState* play);
void Player_Action_HookshotFly(Player* this, PlayState* play);
void Player_Action_93(Player* this, PlayState* play);
void Player_Action_94(Player* this, PlayState* play);
void Player_Action_95(Player* this, PlayState* play);
void Player_Action_96(Player* this, PlayState* play);
void Player_Action_CsAction(Player* this, PlayState* play);

s32 Player_UpperAction_0(Player* this, PlayState* play);
s32 Player_UpperAction_1(Player* this, PlayState* play);
s32 Player_UpperAction_ChangeHeldItem(Player* this, PlayState* play);
s32 Player_UpperAction_3(Player* this, PlayState* play);
s32 Player_UpperAction_4(Player* this, PlayState* play);
s32 Player_UpperAction_5(Player* this, PlayState* play);
s32 Player_UpperAction_6(Player* this, PlayState* play);
s32 Player_UpperAction_7(Player* this, PlayState* play);
s32 Player_UpperAction_8(Player* this, PlayState* play);
s32 Player_UpperAction_9(Player* this, PlayState* play);
s32 Player_UpperAction_CarryActor(Player* this, PlayState* play);
s32 Player_UpperAction_11(Player* this, PlayState* play);
s32 Player_UpperAction_12(Player* this, PlayState* play);
s32 Player_UpperAction_13(Player* this, PlayState* play);
s32 Player_UpperAction_14(Player* this, PlayState* play);
s32 Player_UpperAction_15(Player* this, PlayState* play);
s32 Player_UpperAction_16(Player* this, PlayState* play);

void Player_InitDefaultIA(PlayState* play, Player* this);
void Player_InitDekuStickIA(PlayState* play, Player* this);
void Player_InitBowOrDekuNutIA(PlayState* play, Player* this);
void Player_InitExplosiveIA(PlayState* play, Player* this);
void Player_InitHookshotIA(PlayState* play, Player* this);
void Player_InitZoraBoomerangIA(PlayState* play, Player* this);

s32 Player_ActionHandler_0(Player* this, PlayState* play);
s32 Player_ActionHandler_1(Player* this, PlayState* play);
s32 Player_ActionHandler_2(Player* this, PlayState* play);
s32 Player_ActionHandler_3(Player* this, PlayState* play);
s32 Player_ActionHandler_Talk(Player* this, PlayState* play);
s32 Player_ActionHandler_5(Player* this, PlayState* play);
s32 Player_ActionHandler_6(Player* this, PlayState* play);
s32 Player_ActionHandler_7(Player* this, PlayState* play);
s32 Player_ActionHandler_8(Player* this, PlayState* play);
s32 Player_ActionHandler_9(Player* this, PlayState* play);
s32 Player_ActionHandler_10(Player* this, PlayState* play);
s32 Player_ActionHandler_11(Player* this, PlayState* play);
s32 Player_ActionHandler_12(Player* this, PlayState* play);
s32 Player_ActionHandler_13(Player* this, PlayState* play);
s32 Player_ActionHandler_14(Player* this, PlayState* play);

/* Cutscene functions */
void Player_CsAction_0(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_1(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_2(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_3(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_4(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_5(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_6(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_7(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_8(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_9(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_10(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_11(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_12(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_13(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_14(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_15(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_16(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_17(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_18(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_19(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_20(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_21(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_22(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_23(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_TranslateReverse(PlayState* play, Player* this, CsCmdActorCue* cue2);
void Player_CsAction_25(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_26(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_27(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_28(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_29(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_30(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_31(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_32(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_33(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_34(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_35(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_36(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_37(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_38(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_39(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_40(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_41(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_42(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_43(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_44(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_45(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_46(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_End(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_48(PlayState* play, Player* this, CsCmdActorCue* cue);

// Mostly PlayerAnimationHeader* anim

void Player_CsAnim_StopHorizontalMovement(PlayState* play, Player* this, void* arg2); // void* arg2
void Player_CsAnim_PlayOnceMorphReset(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayOnceSlowMorphAdjustedReset(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoopSlowMorphAdjustedReset(PlayState* play, Player* this, void* anim);
void Player_CsAnim_ReplacePlayOnceNormalAdjusted(PlayState* play, Player* this, void* anim);
void Player_CsAnim_ReplacePlayOnce(PlayState* play, Player* this, void* anim);
void Player_CsAnim_ReplacePlayLoopNormalAdjusted(PlayState* play, Player* this, void* anim);
void Player_CsAnim_ReplacePlayLoop(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayOnce(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoop(PlayState* play, Player* this, void* anim);
void Player_CsAnim_Update(PlayState* play, Player* this, void* cue); // CsCmdActorCue* cue
void Player_CsAnim_PlayLoopAdjustedSlowMorphAnimSfxReset(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoopNormalAdjustedOnceFinished(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayOnceFreezeReset(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayOnceAdjusted(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoopAdjusted(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoopAdjustedOnceFinished(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayAnimSfx(PlayState* play, Player* this, void* entry); // AnimSfxEntry* entry
void Player_CsAnim_ReplacePlayOnceAdjustedReverse(PlayState* play, Player* this, void* anim);

typedef struct struct_8085C2A4 {
    /* 0x0 */ PlayerAnimationHeader* unk_0;
    /* 0x4 */ PlayerAnimationHeader* unk_4;
    /* 0x8 */ PlayerAnimationHeader* unk_8;
} struct_8085C2A4; // size = 0xC

typedef struct BlureColors {
    /* 0x0 */ u8 p1StartColor[4];
    /* 0x4 */ u8 p2StartColor[4];
    /* 0x8 */ u8 p1EndColor[4];
    /* 0xC */ u8 p2EndColor[4];
} BlureColors; // size = 0x10

typedef void (*PlayerCsAnim)(PlayState*, Player*, void*);
typedef void (*PlayerCsActionFunc)(PlayState*, Player*, CsCmdActorCue*);

typedef enum {
    /*   -1 */ PLAYER_CSTYPE_ACTION = -1,
    /* 0x00 */ PLAYER_CSTYPE_NONE,
    /* 0x01 */ PLAYER_CSTYPE_ANIM_1,
    /* 0x02 */ PLAYER_CSTYPE_ANIM_2,
    /* 0x03 */ PLAYER_CSTYPE_ANIM_3,
    /* 0x04 */ PLAYER_CSTYPE_ANIM_4,
    /* 0x05 */ PLAYER_CSTYPE_ANIM_5,
    /* 0x06 */ PLAYER_CSTYPE_ANIM_6,
    /* 0x07 */ PLAYER_CSTYPE_ANIM_7,
    /* 0x08 */ PLAYER_CSTYPE_ANIM_8,
    /* 0x09 */ PLAYER_CSTYPE_ANIM_9,
    /* 0x0A */ PLAYER_CSTYPE_ANIM_10,
    /* 0x0B */ PLAYER_CSTYPE_ANIM_11,
    /* 0x0C */ PLAYER_CSTYPE_ANIM_12,
    /* 0x0D */ PLAYER_CSTYPE_ANIM_13,
    /* 0x0E */ PLAYER_CSTYPE_ANIM_14,
    /* 0x0F */ PLAYER_CSTYPE_ANIM_15,
    /* 0x10 */ PLAYER_CSTYPE_ANIM_16,
    /* 0x11 */ PLAYER_CSTYPE_ANIM_17,
    /* 0x12 */ PLAYER_CSTYPE_ANIM_18,
    /* 0x13 */ PLAYER_CSTYPE_ANIM_19
} PlayerCsType;

typedef struct PlayerCsActionEntry {
    /* 0x0 */ s8 type; // PlayerCsType enum
    /* 0x4 */ union {
        void* ptr; // Do not use, required in the absence of designated initialisors
        PlayerCsActionFunc csActionFunc;
        void* csAnimArg2; // Can point to any of the below in the union
        PlayerAnimationHeader* anim;
        AnimSfxEntry* entry;
        CsCmdActorCue* cue;
    };
} PlayerCsActionEntry; // size = 0x8

typedef struct struct_8085E368 {
    /* 0x0 */ Vec3s base;
    /* 0x6 */ Vec3s range;
} struct_8085E368; // size = 0xC

typedef struct struct_8085D910 {
    /* 0x0 */ u8 unk_0;
    /* 0x1 */ u8 unk_1;
    /* 0x2 */ u8 unk_2;
    /* 0x3 */ u8 unk_3;
} struct_8085D910; // size = 0x4

typedef struct struct_8085D848_unk_00 {
    /* 0x0 */ s16 fogNear;
    /* 0x2 */ u8 fogColor[3];
    /* 0x5 */ u8 ambientColor[3];
} struct_8085D848_unk_00; // size = 0x8

typedef struct struct_8085D848_unk_18 {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ u8 color[3];
    /* 0x10 */ s16 radius;
} struct_8085D848_unk_18; // size = 0x14

typedef struct struct_8085D848 {
    /* 0x00 */ struct_8085D848_unk_00 unk_00[3];
    /* 0x18 */ struct_8085D848_unk_18 light[3];
} struct_8085D848; // size = 0x54

typedef struct struct_8085D80C {
    /* 0x0 */ s16 actorId;
    /* 0x2 */ s16 params;
} struct_8085D80C; // size = 0x4

typedef struct struct_8085D798 {
    /* 0x0 */ s16 actorId;
    /* 0x2 */ s8 actorParams;
    /* 0x3 */ u8 itemId;
    /* 0x4 */ u8 itemAction;
    /* 0x5 */ u8 textId;
} struct_8085D798; // size = 0x6

typedef struct struct_8085D714 {
    /* 0x0 */ u8 unk_0;
    /* 0x4 */ PlayerAnimationHeader* unk_4;
} struct_8085D714; // size = 0x8

typedef struct struct_8085D224 {
    /* 0x0 */ PlayerAnimationHeader* anim;
    /* 0x4 */ f32 unk_4;
    /* 0x8 */ f32 unk_8;
} struct_8085D224; // size = 0xC

typedef struct FallImpactInfo {
    /* 0x0 */ s8 damage;
    /* 0x1 */ u8 sourceIntensity;
    /* 0x2 */ u8 decayTimer;
    /* 0x3 */ u8 decayStep;
    /* 0x4 */ u16 sfxId;
} FallImpactInfo; // size = 0x6

typedef struct AttackAnimInfo {
    /* 0x0 */ PlayerAnimationHeader* unk_0;
    /* 0x4 */ PlayerAnimationHeader* unk_4;
    /* 0x8 */ PlayerAnimationHeader* unk_8;
    /* 0xC */ u8 unk_C;
    /* 0xD */ u8 unk_D;
} AttackAnimInfo; // size = 0x10

typedef struct MeleeWeaponDamageInfo {
    /* 0x0 */ s32 dmgFlags;
    // Presumably these two fields are intended for Fierce Deity, but will also work for Deku if it can equip a sword
    /* 0x4 */ u8 dmgTransformedNormal;
    /* 0x5 */ u8 dmgTransformedStrong;
    /* 0x6 */ u8 dmgHumanNormal;
    /* 0x7 */ u8 dmgHumanStrong;
} MeleeWeaponDamageInfo; // size = 0x8

typedef struct ItemChangeInfo {
    /* 0x0 */ PlayerAnimationHeader* anim;
    /* 0x4 */ u8 changeFrame;
} ItemChangeInfo; // size = 0x8

typedef struct {
    /* 0x0 */ u8 itemId;
    /* 0x2 */ s16 actorId;
} ExplosiveInfo; // size = 0x4

typedef struct {
    /* 0x0 */ Color_RGB8 ambientColor;
    /* 0x3 */ Color_RGB8 diffuseColor;
    /* 0x6 */ Color_RGB8 fogColor;
    /* 0xA */ s16 fogNear;
    /* 0xC */ s16 zFar;
} PlayerEnvLighting; // size = 0xE

typedef struct GetItemEntry {
    /* 0x0 */ u8 itemId;
    /* 0x1 */ u8 field; // various bit-packed data
    /* 0x2 */ s8 gid;   // defines the draw id and chest opening animation
    /* 0x3 */ u8 textId;
    /* 0x4 */ u16 objectId;
} GetItemEntry; // size = 0x6

typedef struct struct_8085D200 {
    /* 0x0 */ PlayerAnimationHeader* unk_0;
    /* 0x4 */ PlayerAnimationHeader* unk_4;
    /* 0x8 */ u8 unk_8;
    /* 0x9 */ u8 unk_9;
} struct_8085D200; // size = 0xC

f32 sControlStickMagnitude;
s16 sControlStickAngle;
s16 sControlStickWorldYaw;
s32 sUpperBodyIsBusy; // see `Player_UpdateUpperBody`
FloorType sPlayerFloorType;
u32 sPlayerTouchedWallFlags;
ConveyorSpeed sPlayerConveyorSpeedIndex;
s16 sPlayerIsOnFloorConveyor;
s16 sPlayerConveyorYaw;
f32 sPlayerYDistToFloor;
FloorProperty sPrevFloorProperty;
s32 sShapeYawToTouchedWall;
s32 sWorldYawToTouchedWall;
s16 sFloorPitchShape;
s32 sSavedCurrentMask;
Vec3f sInteractWallCheckResult;
f32 D_80862B3C;
FloorEffect sPlayerFloorEffect;
Input* sPlayerControlInput;
s32 sPlayerUseHeldItem;              // When true, the current held item is used. Is reset to false every frame.
s32 sPlayerHeldItemButtonIsHeldDown; // Indicates if the button for the current held item is held down.
AdjLightSettings D_80862B50;         // backup of lay->envCtx.adjLightSettings
s32 D_80862B6C;                      // this->skelAnime.movementFlags // sPlayerSkelMoveFlags?

bool func_8082DA90(PlayState* play) {
    return (play->transitionTrigger != TRANS_TRIGGER_OFF) || (play->transitionMode != TRANS_MODE_OFF);
}

void Player_StopHorizontalMovement(Player* this) {
    this->speedXZ = 0.0f;
    this->actor.speed = 0.0f;
}

void func_8082DAD4(Player* this) {
    Player_StopHorizontalMovement(this);
    this->unk_AA5 = PLAYER_UNKAA5_0;
}

s32 Player_IsTalking(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return CHECK_FLAG_ALL(player->actor.flags, ACTOR_FLAG_TALK);
}

void Player_Anim_PlayOnce(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_PlayOnce(play, &this->skelAnime, anim);
}

void Player_Anim_PlayLoop(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_PlayLoop(play, &this->skelAnime, anim);
}

void Player_Anim_PlayLoopAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_PlayLoopSetSpeed(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_Anim_PlayOnceAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_Anim_PlayOnceAdjustedReverse(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, -PLAYER_ANIM_ADJUSTED_SPEED, Animation_GetLastFrame(anim),
                           0.0f, ANIMMODE_ONCE, 0.0f);
}

void Player_Anim_ResetModelRotY(Player* this) {
    this->skelAnime.jointTable[LIMB_ROOT_ROT].y = 0;
}

void func_8082DC38(Player* this) {
    this->stateFlags2 &= ~PLAYER_STATE2_20000;
    this->meleeWeaponState = PLAYER_MELEE_WEAPON_STATE_0;
    this->meleeWeaponInfo[2].active = false;
    this->meleeWeaponInfo[1].active = false;
    this->meleeWeaponInfo[0].active = false;
}

void func_8082DC64(PlayState* play, Player* this) {
    if ((this->subCamId != CAM_ID_NONE) && (play->cameraPtrs[this->subCamId] != NULL)) {
        this->subCamId = CAM_ID_NONE;
    }

    this->stateFlags2 &= ~(PLAYER_STATE2_400 | PLAYER_STATE2_800);
}

void Player_DetachHeldActor(PlayState* play, Player* this) {
    Actor* heldActor = this->heldActor;

    if ((heldActor != NULL) && !Player_IsHoldingHookshot(this)) {
        this->actor.child = NULL;
        this->heldActor = NULL;
        this->interactRangeActor = NULL;
        heldActor->parent = NULL;
        this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
    }

    if (Player_GetExplosiveHeld(this) > PLAYER_EXPLOSIVE_NONE) {
        Player_InitItemAction(play, this, PLAYER_IA_NONE);
        this->heldItemId = ITEM_FE;
    }
}

void func_8082DD2C(PlayState* play, Player* this) {
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

    func_8082DC38(this);
    this->unk_AA5 = PLAYER_UNKAA5_0;
    func_8082DC64(play, this);
    Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
    this->stateFlags1 &=
        ~(PLAYER_STATE1_4 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 | PLAYER_STATE1_100000 | PLAYER_STATE1_200000);
    this->stateFlags2 &= ~(PLAYER_STATE2_10 | PLAYER_STATE2_80);
    this->unk_ADD = 0;
    this->unk_ADC = 0;
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->unk_ABC = 0.0f;
    this->unk_AC0 = 0.0f;
}

/**
 * Puts away item currently in hand, if holding any.
 * @return  true if an item needs to be put away, false if not.
 */
s32 Player_PutAwayHeldItem(PlayState* play, Player* this) {
    if (this->heldItemAction > PLAYER_IA_LAST_USED) {
        Player_UseItem(play, this, ITEM_NONE);
        return true;
    } else {
        return false;
    }
}

void func_8082DE50(PlayState* play, Player* this) {
    func_8082DD2C(play, this);
    Player_DetachHeldActor(play, this);
}

s32 func_8082DE88(Player* this, s32 arg1, s32 arg2) {
    s16 controlStickAngleDiff = this->prevControlStickAngle - sControlStickAngle;

    this->av2.actionVar2 +=
        arg1 + TRUNCF_BINANG(ABS_ALT(controlStickAngleDiff) * fabsf(sControlStickMagnitude) * (1.0f / 0x600F0));

    if (CHECK_BTN_ANY(sPlayerControlInput->press.button, BTN_B | BTN_A)) {
        this->av2.actionVar2 += 5;
    }

    return this->av2.actionVar2 >= arg2;
}

void func_8082DF2C(PlayState* play) {
    if (play->actorCtx.freezeFlashTimer == 0) {
        play->actorCtx.freezeFlashTimer = 1;
    }
}

u8 sPlayerUpperBodyLimbCopyMap[PLAYER_LIMB_MAX] = {
    false, // PLAYER_LIMB_NONE
    false, // PLAYER_LIMB_ROOT
    false, // PLAYER_LIMB_WAIST
    false, // PLAYER_LIMB_LOWER_ROOT
    false, // PLAYER_LIMB_R_THIGH
    false, // PLAYER_LIMB_R_SHIN
    false, // PLAYER_LIMB_R_FOOT
    false, // PLAYER_LIMB_L_THIGH
    false, // PLAYER_LIMB_L_SHIN
    false, // PLAYER_LIMB_L_FOOT
    true,  // PLAYER_LIMB_UPPER_ROOT
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
    true,  // PLAYER_LIMB_TORSO
};
u8 D_8085BA08[PLAYER_LIMB_MAX] = {
    false, // PLAYER_LIMB_NONE
    false, // PLAYER_LIMB_ROOT
    false, // PLAYER_LIMB_WAIST
    false, // PLAYER_LIMB_LOWER_ROOT
    false, // PLAYER_LIMB_R_THIGH
    false, // PLAYER_LIMB_R_SHIN
    false, // PLAYER_LIMB_R_FOOT
    false, // PLAYER_LIMB_L_THIGH
    false, // PLAYER_LIMB_L_SHIN
    false, // PLAYER_LIMB_L_FOOT
    false, // PLAYER_LIMB_UPPER_ROOT
    false, // PLAYER_LIMB_HEAD
    false, // PLAYER_LIMB_HAT
    false, // PLAYER_LIMB_COLLAR
    true,  // PLAYER_LIMB_L_SHOULDER
    true,  // PLAYER_LIMB_L_FOREARM
    true,  // PLAYER_LIMB_L_HAND
    false, // PLAYER_LIMB_R_SHOULDER
    false, // PLAYER_LIMB_R_FOREARM
    false, // PLAYER_LIMB_R_HAND
    false, // PLAYER_LIMB_SHEATH
    false, // PLAYER_LIMB_TORSO
};
u8 D_8085BA20[PLAYER_LIMB_MAX] = {
    false, // PLAYER_LIMB_NONE
    false, // PLAYER_LIMB_ROOT
    false, // PLAYER_LIMB_WAIST
    false, // PLAYER_LIMB_LOWER_ROOT
    false, // PLAYER_LIMB_R_THIGH
    false, // PLAYER_LIMB_R_SHIN
    false, // PLAYER_LIMB_R_FOOT
    false, // PLAYER_LIMB_L_THIGH
    false, // PLAYER_LIMB_L_SHIN
    false, // PLAYER_LIMB_L_FOOT
    false, // PLAYER_LIMB_UPPER_ROOT
    false, // PLAYER_LIMB_HEAD
    false, // PLAYER_LIMB_HAT
    false, // PLAYER_LIMB_COLLAR
    false, // PLAYER_LIMB_L_SHOULDER
    false, // PLAYER_LIMB_L_FOREARM
    false, // PLAYER_LIMB_L_HAND
    true,  // PLAYER_LIMB_R_SHOULDER
    true,  // PLAYER_LIMB_R_FOREARM
    true,  // PLAYER_LIMB_R_HAND
    false, // PLAYER_LIMB_SHEATH
    false, // PLAYER_LIMB_TORSO
};

void Player_RequestRumble(PlayState* play, Player* this, s32 sourceIntensity, s32 decayTimer, s32 decayStep,
                          s32 distSq) {
    if (this == GET_PLAYER(play)) {
        Rumble_Request(distSq, sourceIntensity, decayTimer, decayStep);
    }
}

PlayerAgeProperties sPlayerAgeProperties[PLAYER_FORM_MAX] = {
    {
        // ceilingCheckHeight
        84.0f,
        // shadowScale
        90.0f,
        // unk_08
        1.5f,
        // unk_0C
        166.5f,
        // unk_10
        105.0f,
        // unk_14
        119.100006f,
        // unk_18
        88.5f,
        // unk_1C
        61.5f,
        // unk_20
        28.5f,
        // unk_24
        54.0f,
        // unk_28
        75.0f,
        // unk_2C
        84.0f,
        // unk_30
        102.0f,
        // unk_34
        70.0f,
        // wallCheckRadius
        27.0f,
        // unk_3C
        24.75f,
        // unk_40
        105.0f,
        // unk_44
        { 9, 0x123F, 0x167 },
        {
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
        },
        {
            { 9, 0x17EA, 0x167 },
            { 9, 0x1E0D, 0x17C },
            { 9, 0x17EA, 0x167 },
            { 9, 0x1E0D, 0x17C },
        },
        {
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
            { -0x638, 0x1256, 0x17C },
            { -0x637, 0x17EA, 0x167 },
        },
        // voiceSfxIdOffset
        SFX_VOICE_BANK_SIZE * 0,
        // surfaceSfxIdOffset
        0x80,
        // unk_98
        33.0f,
        // unk_9C
        44.15145f,
        // openChestAnim
        &gPlayerAnim_link_demo_Tbox_open,
        // timeTravelStartAnim
        &gPlayerAnim_link_demo_back_to_past,
        // timeTravelEndAnim
        &gPlayerAnim_link_demo_return_to_past,
        // unk_AC
        &gPlayerAnim_link_normal_climb_startA,
        // unk_B0
        &gPlayerAnim_link_normal_climb_startB,
        // unk_B4
        {
            &gPlayerAnim_link_normal_climb_upL,
            &gPlayerAnim_link_normal_climb_upR,
            &gPlayerAnim_link_normal_Fclimb_upL,
            &gPlayerAnim_link_normal_Fclimb_upR,
        },
        // unk_C4
        {
            &gPlayerAnim_link_normal_Fclimb_sideL,
            &gPlayerAnim_link_normal_Fclimb_sideR,
        },
        // unk_CC
        {
            &gPlayerAnim_link_normal_climb_endAL,
            &gPlayerAnim_link_normal_climb_endAR,
        },
        // unk_D4
        {
            &gPlayerAnim_link_normal_climb_endBR,
            &gPlayerAnim_link_normal_climb_endBL,
        },
    },
    {
        // ceilingCheckHeight
        70.0f,
        // shadowScale
        90.0f,
        // unk_08
        0.74f,
        // unk_0C
        111.0f,
        // unk_10
        70.0f,
        // unk_14
        79.4f,
        // unk_18
        59.0f,
        // unk_1C
        41.0f,
        // unk_20
        19.0f,
        // unk_24
        36.0f,
        // unk_28
        50.0f,
        // unk_2C
        56.0f,
        // unk_30
        68.0f,
        // unk_34
        70.0f,
        // wallCheckRadius
        19.5f,
        // unk_3C
        18.2f,
        // unk_40
        80.0f,
        // unk_44
        { 0x17, 0xF3B, 0xDF },
        {
            { 0x18, 0xF3B, 0xDF },
            { 0x17, 0x14CF, 0xDF },
            { 0x18, 0xF3B, 0xDF },
            { 0x17, 0x14CF, 0xDF },
        },
        {
            { 0x17, 0x14CF, 0xDF },
            { 0x18, 0x1AF2, 0xDF },
            { 0x17, 0x14CF, 0xDF },
            { 0x18, 0x1AF2, 0xDF },
        },
        {
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
            { -0x638, 0x1256, 0x17C },
            { -0x637, 0x17EA, 0x167 },
        },
        // voiceSfxIdOffset
        SFX_VOICE_BANK_SIZE * 6,
        // surfaceSfxIdOffset
        0x150,
        // unk_98
        -25.0f,
        // unk_9C
        42.0f,
        // openChestAnim
        &gPlayerAnim_pg_Tbox_open,
        // timeTravelStartAnim
        &gPlayerAnim_link_demo_back_to_past,
        // timeTravelEndAnim
        &gPlayerAnim_link_demo_return_to_past,
        // unk_AC
        &gPlayerAnim_pg_climb_startA,
        // unk_B0
        &gPlayerAnim_pg_climb_startB,
        // unk_B4
        {
            &gPlayerAnim_pg_climb_upL,
            &gPlayerAnim_pg_climb_upR,
            &gPlayerAnim_pg_climb_upL,
            &gPlayerAnim_pg_climb_upR,
        },
        // unk_C4
        {
            &gPlayerAnim_link_normal_Fclimb_sideL,
            &gPlayerAnim_link_normal_Fclimb_sideR,
        },
        // unk_CC
        {
            &gPlayerAnim_pg_climb_endAL,
            &gPlayerAnim_pg_climb_endAR,
        },
        // unk_D4
        {
            &gPlayerAnim_pg_climb_endBR,
            &gPlayerAnim_pg_climb_endBL,
        },
    },
    {
        // ceilingCheckHeight
        56.0f,
        // shadowScale
        90.0f,
        // unk_08
        1.0f,
        // unk_0C
        111.0f,
        // unk_10
        70.0f,
        // unk_14
        79.4f,
        // unk_18
        59.0f,
        // unk_1C
        41.0f,
        // unk_20
        19.0f,
        // unk_24
        36.0f,
        // unk_28
        50.0f,
        // unk_2C
        56.0f,
        // unk_30
        68.0f,
        // unk_34
        70.0f,
        // wallCheckRadius
        18.0f,
        // unk_3C
        23.0f,
        // unk_40
        70.0f,
        // unk_44
        { 0x17, 0x1323, -0x6D },
        {
            { 0x17, 0x1323, -0x58 },
            { 0x17, 0x18B7, -0x6D },
            { 0x17, 0x1323, -0x58 },
            { 0x17, 0x18B7, -0x6D },
        },
        {
            { 0x17, 0x18B7, -0x6D },
            { 0x18, 0x1EDA, -0x58 },
            { 0x17, 0x18B7, -0x6D },
            { 0x18, 0x1EDA, -0x58 },
        },
        {
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
            { -0x638, 0x1256, 0x17C },
            { -0x637, 0x17EA, 0x167 },
        },
        // voiceSfxIdOffset
        SFX_VOICE_BANK_SIZE * 5,
        // surfaceSfxIdOffset
        0x120,
        // unk_98
        22.0f,
        // unk_9C
        36.0f,
        // openChestAnim
        &gPlayerAnim_pz_Tbox_open,
        // timeTravelStartAnim
        &gPlayerAnim_link_demo_back_to_past,
        // timeTravelEndAnim
        &gPlayerAnim_link_demo_return_to_past,
        // unk_AC
        &gPlayerAnim_pz_climb_startA,
        // unk_B0
        &gPlayerAnim_pz_climb_startB,
        // unk_B4
        {
            &gPlayerAnim_pz_climb_upL,
            &gPlayerAnim_pz_climb_upR,
            &gPlayerAnim_link_normal_Fclimb_upL,
            &gPlayerAnim_link_normal_Fclimb_upR,
        },
        // unk_C4
        {
            &gPlayerAnim_link_normal_Fclimb_sideL,
            &gPlayerAnim_link_normal_Fclimb_sideR,
        },
        // unk_CC
        {
            &gPlayerAnim_pz_climb_endAL,
            &gPlayerAnim_pz_climb_endAR,
        },
        // unk_D4
        {
            &gPlayerAnim_pz_climb_endBR,
            &gPlayerAnim_pz_climb_endBL,
        },
    },
    {
        // ceilingCheckHeight
        35.0f,
        // shadowScale
        50.0f,
        // unk_08
        0.3f,
        // unk_0C
        71.0f,
        // unk_10
        50.0f,
        // unk_14
        49.0f,
        // unk_18
        39.0f,
        // unk_1C
        27.0f,
        // unk_20
        19.0f,
        // unk_24
        8.0f,
        // unk_28
        13.6f,
        // unk_2C
        24.0f,
        // unk_30
        24.0f,
        // unk_34
        70.0f,
        // wallCheckRadius
        14.0f,
        // unk_3C
        12.0f,
        // unk_40
        55.0f,
        // unk_44
        { -0x18, 0xDED, 0x36C },
        {
            { -0x18, 0xD92, 0x35E },
            { -0x18, 0x1371, 0x3A9 },
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
        },
        {
            { -0x18, 0x1371, 0x3A9 },
            { -0x18, 0x195F, 0x3A9 },
            { 9, 0x17EA, 0x167 },
            { 9, 0x1E0D, 0x17C },
        },
        {
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
            { -0x638, 0x1256, 0x17C },
            { -0x637, 0x17EA, 0x167 },
        },
        // voiceSfxIdOffset
        SFX_VOICE_BANK_SIZE * 4,
        // surfaceSfxIdOffset
        0xF0,
        // unk_98
        -21.0f,
        // unk_9C
        33.0f,
        // openChestAnim
        &gPlayerAnim_pn_Tbox_open,
        // timeTravelStartAnim
        &gPlayerAnim_link_demo_back_to_past,
        // timeTravelEndAnim
        &gPlayerAnim_link_demo_return_to_past,
        // unk_AC
        &gPlayerAnim_clink_normal_climb_startA,
        // unk_B0
        &gPlayerAnim_clink_normal_climb_startB,
        // unk_B4
        {
            &gPlayerAnim_clink_normal_climb_upL,
            &gPlayerAnim_clink_normal_climb_upR,
            &gPlayerAnim_link_normal_Fclimb_upL,
            &gPlayerAnim_link_normal_Fclimb_upR,
        },
        // unk_C4
        {
            &gPlayerAnim_link_normal_Fclimb_sideL,
            &gPlayerAnim_link_normal_Fclimb_sideR,
        },
        // unk_CC
        {
            &gPlayerAnim_clink_normal_climb_endAL,
            &gPlayerAnim_clink_normal_climb_endAR,
        },
        // unk_D4
        {
            &gPlayerAnim_clink_normal_climb_endBR,
            &gPlayerAnim_clink_normal_climb_endBL,
        },
    },
    {
        // ceilingCheckHeight
        40.0f,
        // shadowScale
        60.0f,
        // unk_08
        11.0f / 17.0f,
        // unk_0C
        71.0f,
        // unk_10
        50.0f,
        // unk_14
        49.0f,
        // unk_18
        39.0f,
        // unk_1C
        27.0f,
        // unk_20
        19.0f,
        // unk_24
        22.0f,
        // unk_28
        32.4f,
        // unk_2C
        32.0f,
        // unk_30
        48.0f,
        // unk_34
        11.0f / 17.0f * 70.0f,
        // wallCheckRadius
        14.0f,
        // unk_3C
        12.0f,
        // unk_40
        55.0f,
        // unk_44
        { -0x18, 0xDED, 0x36C },
        {
            { -0x18, 0xD92, 0x35E },
            { -0x18, 0x1371, 0x3A9 },
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
        },
        {
            { -0x18, 0x1371, 0x3A9 },
            { -0x18, 0x195F, 0x3A9 },
            { 9, 0x17EA, 0x167 },
            { 9, 0x1E0D, 0x17C },
        },
        {
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
            { -0x638, 0x1256, 0x17C },
            { -0x637, 0x17EA, 0x167 },
        },
        // voiceSfxIdOffset
        SFX_VOICE_BANK_SIZE * 1,
        // surfaceSfxIdOffset
        0,
        // unk_98
        22.0f,
        // unk_9C
        29.4343f,
        // openChestAnim
        &gPlayerAnim_clink_demo_Tbox_open,
        // timeTravelStartAnim
        &gPlayerAnim_clink_demo_goto_future,
        // timeTravelEndAnim
        &gPlayerAnim_clink_demo_return_to_future,
        // unk_AC
        &gPlayerAnim_clink_normal_climb_startA,
        // unk_B0
        &gPlayerAnim_clink_normal_climb_startB,
        // unk_B4
        {
            &gPlayerAnim_clink_normal_climb_upL,
            &gPlayerAnim_clink_normal_climb_upR,
            &gPlayerAnim_link_normal_Fclimb_upL,
            &gPlayerAnim_link_normal_Fclimb_upR,
        },
        // unk_C4
        {
            &gPlayerAnim_link_normal_Fclimb_sideL,
            &gPlayerAnim_link_normal_Fclimb_sideR,
        },
        // unk_CC
        {
            &gPlayerAnim_clink_normal_climb_endAL,
            &gPlayerAnim_clink_normal_climb_endAR,
        },
        // unk_D4
        {
            &gPlayerAnim_clink_normal_climb_endBR,
            &gPlayerAnim_clink_normal_climb_endBL,
        },
    },
};

PlayerAnimationHeader* D_8085BE84[PLAYER_ANIMGROUP_MAX][PLAYER_ANIMTYPE_MAX] = {
    // PLAYER_ANIMGROUP_wait
    {
        &gPlayerAnim_link_normal_wait_free,  // Default idle standing, looking forward
        &gPlayerAnim_link_normal_wait,       // Default idle standing, looking forward, sword and shield
        &gPlayerAnim_link_normal_wait,       // Default idle standing, looking forward, sword and shield
        &gPlayerAnim_link_fighter_wait_long, // Default idle standing, looking forward, two hand weapon
        &gPlayerAnim_link_normal_wait_free,  // Default idle standing, looking forward
        &gPlayerAnim_link_normal_wait_free,  // Default idle standing, looking forward
    },
    // PLAYER_ANIMGROUP_walk
    {
        &gPlayerAnim_link_normal_walk_free,
        &gPlayerAnim_link_normal_walk,
        &gPlayerAnim_link_normal_walk,
        &gPlayerAnim_link_fighter_walk_long,
        &gPlayerAnim_link_normal_walk_free,
        &gPlayerAnim_link_normal_walk_free,
    },
    // PLAYER_ANIMGROUP_run
    {
        &gPlayerAnim_link_normal_run_free, // Running with empty hands
        &gPlayerAnim_link_fighter_run,     // Running with Sword and Shield in hands
        &gPlayerAnim_link_normal_run,
        &gPlayerAnim_link_fighter_run_long, // Running with Two handed weapon
        &gPlayerAnim_link_normal_run_free,  // Running with empty hands
        &gPlayerAnim_link_normal_run_free,  // Running with empty hands
    },
    // PLAYER_ANIMGROUP_damage_run
    {
        &gPlayerAnim_link_normal_damage_run_free,
        &gPlayerAnim_link_fighter_damage_run,
        &gPlayerAnim_link_normal_damage_run_free,
        &gPlayerAnim_link_fighter_damage_run_long,
        &gPlayerAnim_link_normal_damage_run_free,
        &gPlayerAnim_link_normal_damage_run_free,
    },
    // PLAYER_ANIMGROUP_waitL
    {
        &gPlayerAnim_link_normal_waitL_free,
        &gPlayerAnim_link_anchor_waitL,
        &gPlayerAnim_link_anchor_waitL,
        &gPlayerAnim_link_fighter_waitL_long,
        &gPlayerAnim_link_normal_waitL_free,
        &gPlayerAnim_link_normal_waitL_free,
    },
    // PLAYER_ANIMGROUP_waitR
    {
        &gPlayerAnim_link_normal_waitR_free,
        &gPlayerAnim_link_anchor_waitR,
        &gPlayerAnim_link_anchor_waitR,
        &gPlayerAnim_link_fighter_waitR_long,
        &gPlayerAnim_link_normal_waitR_free,
        &gPlayerAnim_link_normal_waitR_free,
    },
    // PLAYER_ANIMGROUP_wait2waitR
    {
        &gPlayerAnim_link_fighter_wait2waitR_long,
        &gPlayerAnim_link_normal_wait2waitR,
        &gPlayerAnim_link_normal_wait2waitR,
        &gPlayerAnim_link_fighter_wait2waitR_long,
        &gPlayerAnim_link_fighter_wait2waitR_long,
        &gPlayerAnim_link_fighter_wait2waitR_long,
    },
    // PLAYER_ANIMGROUP_normal2fighter
    {
        &gPlayerAnim_link_normal_normal2fighter_free,
        &gPlayerAnim_link_fighter_normal2fighter,
        &gPlayerAnim_link_fighter_normal2fighter,
        &gPlayerAnim_link_normal_normal2fighter_free,
        &gPlayerAnim_link_normal_normal2fighter_free,
        &gPlayerAnim_link_normal_normal2fighter_free,
    },
    // PLAYER_ANIMGROUP_doorA_free
    {
        &gPlayerAnim_link_demo_doorA_link_free,
        &gPlayerAnim_link_demo_doorA_link,
        &gPlayerAnim_link_demo_doorA_link,
        &gPlayerAnim_link_demo_doorA_link_free,
        &gPlayerAnim_link_demo_doorA_link_free,
        &gPlayerAnim_link_demo_doorA_link_free,
    },
    // PLAYER_ANIMGROUP_doorA
    {
        &gPlayerAnim_clink_demo_doorA_link,
        &gPlayerAnim_clink_demo_doorA_link,
        &gPlayerAnim_clink_demo_doorA_link,
        &gPlayerAnim_clink_demo_doorA_link,
        &gPlayerAnim_clink_demo_doorA_link,
        &gPlayerAnim_clink_demo_doorA_link,
    },
    // PLAYER_ANIMGROUP_doorB_free
    {
        &gPlayerAnim_link_demo_doorB_link_free,
        &gPlayerAnim_link_demo_doorB_link,
        &gPlayerAnim_link_demo_doorB_link,
        &gPlayerAnim_link_demo_doorB_link_free,
        &gPlayerAnim_link_demo_doorB_link_free,
        &gPlayerAnim_link_demo_doorB_link_free,
    },
    // PLAYER_ANIMGROUP_doorB
    {
        &gPlayerAnim_clink_demo_doorB_link,
        &gPlayerAnim_clink_demo_doorB_link,
        &gPlayerAnim_clink_demo_doorB_link,
        &gPlayerAnim_clink_demo_doorB_link,
        &gPlayerAnim_clink_demo_doorB_link,
        &gPlayerAnim_clink_demo_doorB_link,
    },
    // PLAYER_ANIMGROUP_carryB
    {
        &gPlayerAnim_link_normal_carryB_free, // Grabbing something from the floor
        &gPlayerAnim_link_normal_carryB,      //
        &gPlayerAnim_link_normal_carryB,      //
        &gPlayerAnim_link_normal_carryB_free, // Grabbing something from the floor
        &gPlayerAnim_link_normal_carryB_free, // Grabbing something from the floor
        &gPlayerAnim_link_normal_carryB_free, // Grabbing something from the floor
    },
    // PLAYER_ANIMGROUP_landing
    {
        &gPlayerAnim_link_normal_landing_free,
        &gPlayerAnim_link_normal_landing,
        &gPlayerAnim_link_normal_landing,
        &gPlayerAnim_link_normal_landing_free,
        &gPlayerAnim_link_normal_landing_free,
        &gPlayerAnim_link_normal_landing_free,
    },
    // PLAYER_ANIMGROUP_short_landing
    {
        &gPlayerAnim_link_normal_short_landing_free,
        &gPlayerAnim_link_normal_short_landing,
        &gPlayerAnim_link_normal_short_landing,
        &gPlayerAnim_link_normal_short_landing_free,
        &gPlayerAnim_link_normal_short_landing_free,
        &gPlayerAnim_link_normal_short_landing_free,
    },
    // PLAYER_ANIMGROUP_landing_roll
    {
        &gPlayerAnim_link_normal_landing_roll_free,  // Rolling with nothing in hands
        &gPlayerAnim_link_normal_landing_roll,       // Rolling with sword and shield
        &gPlayerAnim_link_normal_landing_roll,       // Rolling with sword and shield
        &gPlayerAnim_link_fighter_landing_roll_long, // Rolling with two hand weapon
        &gPlayerAnim_link_normal_landing_roll_free,  // Rolling with nothing in hands
        &gPlayerAnim_link_normal_landing_roll_free,  // Rolling with nothing in hands
    },
    // PLAYER_ANIMGROUP_hip_down
    {
        &gPlayerAnim_link_normal_hip_down_free, // Rolling bonk
        &gPlayerAnim_link_normal_hip_down,      // Rolling bonk swrod and shield
        &gPlayerAnim_link_normal_hip_down,      // Rolling bonk swrod and shield
        &gPlayerAnim_link_normal_hip_down_long, // Rolling bonk two hand weapon
        &gPlayerAnim_link_normal_hip_down_free, // Rolling bonk
        &gPlayerAnim_link_normal_hip_down_free, // Rolling bonk
    },
    // PLAYER_ANIMGROUP_walk_endL
    {
        &gPlayerAnim_link_normal_walk_endL_free,
        &gPlayerAnim_link_normal_walk_endL,
        &gPlayerAnim_link_normal_walk_endL,
        &gPlayerAnim_link_fighter_walk_endL_long,
        &gPlayerAnim_link_normal_walk_endL_free,
        &gPlayerAnim_link_normal_walk_endL_free,
    },
    // PLAYER_ANIMGROUP_walk_endR
    {
        &gPlayerAnim_link_normal_walk_endR_free,
        &gPlayerAnim_link_normal_walk_endR,
        &gPlayerAnim_link_normal_walk_endR,
        &gPlayerAnim_link_fighter_walk_endR_long,
        &gPlayerAnim_link_normal_walk_endR_free,
        &gPlayerAnim_link_normal_walk_endR_free,
    },
    // PLAYER_ANIMGROUP_defense
    {
        &gPlayerAnim_link_normal_defense_free,
        &gPlayerAnim_link_normal_defense,
        &gPlayerAnim_link_normal_defense,
        &gPlayerAnim_link_normal_defense_free,
        &gPlayerAnim_link_bow_defense,
        &gPlayerAnim_link_normal_defense_free,
    },
    // PLAYER_ANIMGROUP_defense_wait
    {
        &gPlayerAnim_link_normal_defense_wait_free,
        &gPlayerAnim_link_normal_defense_wait,
        &gPlayerAnim_link_normal_defense_wait,
        &gPlayerAnim_link_normal_defense_wait_free,
        &gPlayerAnim_link_bow_defense_wait,
        &gPlayerAnim_link_normal_defense_wait_free,
    },
    // PLAYER_ANIMGROUP_defense_end
    {
        &gPlayerAnim_link_normal_defense_end_free,
        &gPlayerAnim_link_normal_defense_end,
        &gPlayerAnim_link_normal_defense_end,
        &gPlayerAnim_link_normal_defense_end_free,
        &gPlayerAnim_link_normal_defense_end_free,
        &gPlayerAnim_link_normal_defense_end_free,
    },
    // PLAYER_ANIMGROUP_side_walk
    {
        &gPlayerAnim_link_normal_side_walk_free,
        &gPlayerAnim_link_normal_side_walk,
        &gPlayerAnim_link_normal_side_walk,
        &gPlayerAnim_link_fighter_side_walk_long,
        &gPlayerAnim_link_normal_side_walk_free,
        &gPlayerAnim_link_normal_side_walk_free,
    },
    // PLAYER_ANIMGROUP_side_walkL
    {
        &gPlayerAnim_link_normal_side_walkL_free, // Side walking
        &gPlayerAnim_link_anchor_side_walkL,      // Side walking with sword and shield in hands
        &gPlayerAnim_link_anchor_side_walkL,      // Side walking with sword and shield in hands
        &gPlayerAnim_link_fighter_side_walkL_long,
        &gPlayerAnim_link_normal_side_walkL_free, // Side walking
        &gPlayerAnim_link_normal_side_walkL_free, // Side walking
    },
    // PLAYER_ANIMGROUP_side_walkR
    {
        &gPlayerAnim_link_normal_side_walkR_free,
        &gPlayerAnim_link_anchor_side_walkR,
        &gPlayerAnim_link_anchor_side_walkR,
        &gPlayerAnim_link_fighter_side_walkR_long,
        &gPlayerAnim_link_normal_side_walkR_free,
        &gPlayerAnim_link_normal_side_walkR_free,
    },
    // PLAYER_ANIMGROUP_45_turn
    {
        &gPlayerAnim_link_normal_45_turn_free,
        &gPlayerAnim_link_normal_45_turn,
        &gPlayerAnim_link_normal_45_turn,
        &gPlayerAnim_link_normal_45_turn_free,
        &gPlayerAnim_link_normal_45_turn_free,
        &gPlayerAnim_link_normal_45_turn_free,
    },
    // PLAYER_ANIMGROUP_waitL2wait
    {
        &gPlayerAnim_link_normal_waitL2wait,
        &gPlayerAnim_link_normal_waitL2wait,
        &gPlayerAnim_link_normal_waitL2wait,
        &gPlayerAnim_link_fighter_waitL2wait_long,
        &gPlayerAnim_link_fighter_waitL2wait_long,
        &gPlayerAnim_link_fighter_waitL2wait_long,
    },
    // PLAYER_ANIMGROUP_waitR2wait
    {
        &gPlayerAnim_link_normal_waitR2wait,
        &gPlayerAnim_link_normal_waitR2wait,
        &gPlayerAnim_link_normal_waitR2wait,
        &gPlayerAnim_link_fighter_waitR2wait_long,
        &gPlayerAnim_link_fighter_waitR2wait_long,
        &gPlayerAnim_link_fighter_waitR2wait_long,
    },
    // PLAYER_ANIMGROUP_throw
    {
        &gPlayerAnim_link_normal_throw_free,
        &gPlayerAnim_link_normal_throw,
        &gPlayerAnim_link_normal_throw,
        &gPlayerAnim_link_normal_throw_free,
        &gPlayerAnim_link_normal_throw_free,
        &gPlayerAnim_link_normal_throw_free,
    },
    // PLAYER_ANIMGROUP_put
    {
        &gPlayerAnim_link_normal_put_free,
        &gPlayerAnim_link_normal_put,
        &gPlayerAnim_link_normal_put,
        &gPlayerAnim_link_normal_put_free,
        &gPlayerAnim_link_normal_put_free,
        &gPlayerAnim_link_normal_put_free,
    },
    // PLAYER_ANIMGROUP_back_walk
    {
        &gPlayerAnim_link_normal_back_walk,
        &gPlayerAnim_link_normal_back_walk,
        &gPlayerAnim_link_normal_back_walk,
        &gPlayerAnim_link_normal_back_walk,
        &gPlayerAnim_link_normal_back_walk,
        &gPlayerAnim_link_normal_back_walk,
    },
    // PLAYER_ANIMGROUP_check
    {
        &gPlayerAnim_link_normal_check_free,
        &gPlayerAnim_link_normal_check,
        &gPlayerAnim_link_normal_check,
        &gPlayerAnim_link_normal_check_free,
        &gPlayerAnim_link_normal_check_free,
        &gPlayerAnim_link_normal_check_free,
    },
    // PLAYER_ANIMGROUP_check_wait
    {
        &gPlayerAnim_link_normal_check_wait_free,
        &gPlayerAnim_link_normal_check_wait,
        &gPlayerAnim_link_normal_check_wait,
        &gPlayerAnim_link_normal_check_wait_free,
        &gPlayerAnim_link_normal_check_wait_free,
        &gPlayerAnim_link_normal_check_wait_free,
    },
    // PLAYER_ANIMGROUP_check_end
    {
        &gPlayerAnim_link_normal_check_end_free,
        &gPlayerAnim_link_normal_check_end,
        &gPlayerAnim_link_normal_check_end,
        &gPlayerAnim_link_normal_check_end_free,
        &gPlayerAnim_link_normal_check_end_free,
        &gPlayerAnim_link_normal_check_end_free,
    },
    // PLAYER_ANIMGROUP_pull_start
    {
        &gPlayerAnim_link_normal_pull_start_free,
        &gPlayerAnim_link_normal_pull_start,
        &gPlayerAnim_link_normal_pull_start,
        &gPlayerAnim_link_normal_pull_start_free,
        &gPlayerAnim_link_normal_pull_start_free,
        &gPlayerAnim_link_normal_pull_start_free,
    },
    // PLAYER_ANIMGROUP_pulling
    {
        &gPlayerAnim_link_normal_pulling_free,
        &gPlayerAnim_link_normal_pulling,
        &gPlayerAnim_link_normal_pulling,
        &gPlayerAnim_link_normal_pulling_free,
        &gPlayerAnim_link_normal_pulling_free,
        &gPlayerAnim_link_normal_pulling_free,
    },
    // PLAYER_ANIMGROUP_pull_end
    {
        &gPlayerAnim_link_normal_pull_end_free,
        &gPlayerAnim_link_normal_pull_end,
        &gPlayerAnim_link_normal_pull_end,
        &gPlayerAnim_link_normal_pull_end_free,
        &gPlayerAnim_link_normal_pull_end_free,
        &gPlayerAnim_link_normal_pull_end_free,
    },
    // PLAYER_ANIMGROUP_fall_up
    {
        &gPlayerAnim_link_normal_fall_up_free,
        &gPlayerAnim_link_normal_fall_up,
        &gPlayerAnim_link_normal_fall_up,
        &gPlayerAnim_link_normal_fall_up_free,
        &gPlayerAnim_link_normal_fall_up_free,
        &gPlayerAnim_link_normal_fall_up_free,
    },
    // PLAYER_ANIMGROUP_jump_climb_hold
    {
        &gPlayerAnim_link_normal_jump_climb_hold_free,
        &gPlayerAnim_link_normal_jump_climb_hold,
        &gPlayerAnim_link_normal_jump_climb_hold,
        &gPlayerAnim_link_normal_jump_climb_hold_free,
        &gPlayerAnim_link_normal_jump_climb_hold_free,
        &gPlayerAnim_link_normal_jump_climb_hold_free,
    },
    // PLAYER_ANIMGROUP_jump_climb_wait
    {
        &gPlayerAnim_link_normal_jump_climb_wait_free,
        &gPlayerAnim_link_normal_jump_climb_wait,
        &gPlayerAnim_link_normal_jump_climb_wait,
        &gPlayerAnim_link_normal_jump_climb_wait_free,
        &gPlayerAnim_link_normal_jump_climb_wait_free,
        &gPlayerAnim_link_normal_jump_climb_wait_free,
    },
    // PLAYER_ANIMGROUP_jump_climb_up
    {
        &gPlayerAnim_link_normal_jump_climb_up_free,
        &gPlayerAnim_link_normal_jump_climb_up,
        &gPlayerAnim_link_normal_jump_climb_up,
        &gPlayerAnim_link_normal_jump_climb_up_free,
        &gPlayerAnim_link_normal_jump_climb_up_free,
        &gPlayerAnim_link_normal_jump_climb_up_free,
    },
    // PLAYER_ANIMGROUP_down_slope_slip_end
    {
        &gPlayerAnim_link_normal_down_slope_slip_end_free,
        &gPlayerAnim_link_normal_down_slope_slip_end,
        &gPlayerAnim_link_normal_down_slope_slip_end,
        &gPlayerAnim_link_normal_down_slope_slip_end_long,
        &gPlayerAnim_link_normal_down_slope_slip_end_free,
        &gPlayerAnim_link_normal_down_slope_slip_end_free,
    },
    // PLAYER_ANIMGROUP_up_slope_slip_end
    {
        &gPlayerAnim_link_normal_up_slope_slip_end_free,
        &gPlayerAnim_link_normal_up_slope_slip_end,
        &gPlayerAnim_link_normal_up_slope_slip_end,
        &gPlayerAnim_link_normal_up_slope_slip_end_long,
        &gPlayerAnim_link_normal_up_slope_slip_end_free,
        &gPlayerAnim_link_normal_up_slope_slip_end_free,
    },
    // PLAYER_ANIMGROUP_nwait
    {
        &gPlayerAnim_sude_nwait,
        &gPlayerAnim_lkt_nwait,
        &gPlayerAnim_lkt_nwait,
        &gPlayerAnim_sude_nwait,
        &gPlayerAnim_sude_nwait,
        &gPlayerAnim_sude_nwait,
    }
};

struct_8085C2A4 D_8085C2A4[] = {
    /* 0 / Forward */
    {
        &gPlayerAnim_link_fighter_front_jump,
        &gPlayerAnim_link_fighter_front_jump_end,
        &gPlayerAnim_link_fighter_front_jump_endR,
    },
    /* 1 / Left */
    {
        &gPlayerAnim_link_fighter_Lside_jump,
        &gPlayerAnim_link_fighter_Lside_jump_end,
        &gPlayerAnim_link_fighter_Lside_jump_endL,
    },
    /* 2 / Back */
    {
        &gPlayerAnim_link_fighter_backturn_jump,
        &gPlayerAnim_link_fighter_backturn_jump_end,
        &gPlayerAnim_link_fighter_backturn_jump_endR,
    },
    /* 3 / Right */
    {
        &gPlayerAnim_link_fighter_Rside_jump,
        &gPlayerAnim_link_fighter_Rside_jump_end,
        &gPlayerAnim_link_fighter_Rside_jump_endR,
    },
    /* 4 /  */
    {
        &gPlayerAnim_link_normal_newroll_jump_20f,
        &gPlayerAnim_link_normal_newroll_jump_end_20f,
        &gPlayerAnim_link_normal_newroll_jump_end_20f,
    },
    /* 5 /  */
    {
        &gPlayerAnim_link_normal_newside_jump_20f,
        &gPlayerAnim_link_normal_newside_jump_end_20f,
        &gPlayerAnim_link_normal_newside_jump_end_20f,
    },
};

// sCylinderInit
ColliderCylinderInit D_8085C2EC = {
    {
        COL_MATERIAL_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 12, 60, 0, { 0, 0, 0 } },
};

// sShieldCylinderInit
ColliderCylinderInit D_8085C318 = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_HARD | AC_TYPE_ENEMY,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00100000, 0x00, 0x02 },
        { 0xD7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 25, 60, 0, { 0, 0, 0 } },
};

// sMeleeWeaponQuadInit
ColliderQuadInit D_8085C344 = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x01 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

// sShieldQuadInit
ColliderQuadInit D_8085C394 = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_HARD | AC_TYPE_ENEMY,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0xD7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

f32 sWaterSpeedFactor = 1.0f;    // Set to 0.5f in water, 1.0f otherwise. Influences different speed values.
f32 sInvWaterSpeedFactor = 1.0f; // Inverse of `sWaterSpeedFactor` (1.0f / sWaterSpeedFactor)

// ANIMSFX_TYPE_VOICE
void Player_AnimSfx_PlayVoice(Player* this, u16 sfxId) {
    u16 sfxOffset;

    if (this->currentMask == PLAYER_MASK_GIANT) {
        Audio_PlaySfx_GiantsMask(&this->actor.projectedPos, sfxId);
    } else if (this->actor.id == ACTOR_PLAYER) {
        if (this->currentMask == PLAYER_MASK_SCENTS) {
            sfxOffset = SFX_VOICE_BANK_SIZE * 7;
        } else {
            sfxOffset = this->ageProperties->voiceSfxIdOffset;
        }

        Player_PlaySfx(this, sfxOffset + sfxId);
    }
}

u16 D_8085C3EC[] = {
    NA_SE_VO_LI_SWEAT,
    NA_SE_VO_LI_SNEEZE,
    NA_SE_VO_LI_RELAX,
    NA_SE_VO_LI_FALL_L,
};

void func_8082E00C(Player* this) {
    s32 i;
    u16* sfxIdPtr = D_8085C3EC;

    for (i = 0; i < ARRAY_COUNT(D_8085C3EC); i++) {
        AudioSfx_StopById((u16)(*sfxIdPtr + this->ageProperties->voiceSfxIdOffset));
        sfxIdPtr++;
    }
}

u16 Player_GetFloorSfx(Player* this, u16 sfxId) {
    return sfxId + this->floorSfxOffset;
}

// ANIMSFX_TYPE_FLOOR
void Player_AnimSfx_PlayFloor(Player* this, u16 sfxId) {
    Player_PlaySfx(this, Player_GetFloorSfx(this, sfxId));
}

u16 Player_GetFloorSfxByAge(Player* this, u16 sfxId) {
    return sfxId + this->floorSfxOffset + this->ageProperties->surfaceSfxIdOffset;
}

// ANIMSFX_TYPE_FLOOR_BY_AGE
void Player_AnimSfx_PlayFloorByAge(Player* this, u16 sfxId) {
    Player_PlaySfx(this, Player_GetFloorSfxByAge(this, sfxId));
}

// ANIMSFX_TYPE_6 and ANIMSFX_TYPE_8
void Player_AnimSfx_PlayFloorWalk(Player* this, f32 freqVolumeLerp) {
    s32 sfxId;

    if (this->currentMask == PLAYER_MASK_GIANT) {
        sfxId = NA_SE_PL_GIANT_WALK;
    } else {
        sfxId = Player_GetFloorSfxByAge(this, NA_SE_PL_WALK_GROUND);
    }

    // Audio_PlaySfx_AtPosForMetalEffectsWithSyncedFreqAndVolume
    Audio_PlaySfx_AtPosForMetalEffectsWithSyncedFreqAndVolume(&this->actor.projectedPos, sfxId, freqVolumeLerp);
}

// ANIMSFX_TYPE_FLOOR_JUMP
void Player_AnimSfx_PlayFloorJump(Player* this) {
    Player_PlaySfx(this, Player_GetFloorSfxByAge(this, NA_SE_PL_JUMP_GROUND));
}

// ANIMSFX_TYPE_FLOOR_LAND
void Player_AnimSfx_PlayFloorLand(Player* this) {
    Player_PlaySfx(this, Player_GetFloorSfxByAge(this, NA_SE_PL_LAND_GROUND));
}

void func_8082E1F0(Player* this, u16 sfxId) {
    Player_PlaySfx(this, sfxId);
    this->stateFlags2 |= PLAYER_STATE2_8;
}

void Player_PlayAnimSfx(Player* this, AnimSfxEntry* entry) {
    s32 cond;

    do {
        s32 data = ABS_ALT(entry->flags);
        s32 type = ANIMSFX_GET_TYPE(data);

        if (PlayerAnimation_OnFrame(&this->skelAnime, fabsf(ANIMSFX_GET_FRAME(data)))) {
            if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_GENERAL)) {
                Player_PlaySfx(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR)) {
                Player_AnimSfx_PlayFloor(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR_BY_AGE)) {
                Player_AnimSfx_PlayFloorByAge(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_VOICE)) {
                Player_AnimSfx_PlayVoice(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR_LAND)) {
                Player_AnimSfx_PlayFloorLand(this);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_6)) {
                Player_AnimSfx_PlayFloorWalk(this, 6.0f);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR_JUMP)) {
                Player_AnimSfx_PlayFloorJump(this);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_8)) {
                Player_AnimSfx_PlayFloorWalk(this, 0.0f);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_9)) {
                // Audio_PlaySfx_AtPosForMetalEffectsWithSyncedFreqAndVolume
                Audio_PlaySfx_AtPosForMetalEffectsWithSyncedFreqAndVolume(
                    &this->actor.projectedPos, this->ageProperties->surfaceSfxIdOffset + NA_SE_PL_WALK_LADDER, 0.0f);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_SURFACE)) {
                Player_PlaySfx(this, entry->sfxId + this->ageProperties->surfaceSfxIdOffset);
            }
        }

        cond = entry->flags >= 0;
        entry++;
    } while (cond);
}

void Player_Anim_PlayOnceMorph(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, Animation_GetLastFrame(anim),
                           ANIMMODE_ONCE, -6.0f);
}

void Player_Anim_PlayOnceMorphAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, Animation_GetLastFrame(anim),
                           ANIMMODE_ONCE, -6.0f);
}

void Player_Anim_PlayLoopMorph(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void Player_Anim_PlayLoopMorphAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void Player_Anim_PlayOnceFreeze(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
}

void Player_Anim_PlayOnceFreezeAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
}

void Player_Anim_PlayLoopSlowMorph(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f, ANIMMODE_LOOP, -16.0f);
}

s32 Player_Anim_PlayLoopOnceFinished(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, anim);
        return true;
    } else {
        return false;
    }
}

void Player_Anim_ResetPrevTranslRot(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;
    this->skelAnime.prevYaw = this->actor.shape.rot.y;
}

void Player_Anim_ResetPrevTranslRotFormScale(Player* this) {
    Player_Anim_ResetPrevTranslRot(this);
    this->skelAnime.prevTransl.x *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.y *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.z *= this->ageProperties->unk_08;
}

void Player_Anim_ZeroModelYaw(Player* this) {
    this->skelAnime.jointTable[LIMB_ROOT_ROT].y = 0;
}

void Player_Anim_ResetMove(Player* this) {
    if (this->skelAnime.movementFlags) {
        Player_Anim_ResetModelRotY(this);
        this->skelAnime.jointTable[LIMB_ROOT_POS].x = this->skelAnime.baseTransl.x;
        this->skelAnime.jointTable[LIMB_ROOT_POS].z = this->skelAnime.baseTransl.z;

        if (this->skelAnime.movementFlags & ANIM_FLAG_ENABLE_MOVEMENT) {
            if (this->skelAnime.movementFlags & ANIM_FLAG_UPDATE_Y) {
                this->skelAnime.jointTable[LIMB_ROOT_POS].y = this->skelAnime.prevTransl.y;
            }
        } else {
            this->skelAnime.jointTable[LIMB_ROOT_POS].y = this->skelAnime.baseTransl.y;
        }
        Player_Anim_ResetPrevTranslRot(this);
        this->skelAnime.movementFlags = 0;
    }
}

/**
 * Only used for ledge climbing
 */
void Player_AnimReplace_SetupLedgeClimb(Player* this, s32 movementFlags) {
    Vec3f pos;

    this->skelAnime.movementFlags = movementFlags;
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;
    SkelAnime_UpdateTranslation(&this->skelAnime, &pos, this->actor.shape.rot.y);

    if (movementFlags & ANIM_FLAG_1) {
        pos.x *= this->ageProperties->unk_08;
        pos.z *= this->ageProperties->unk_08;
        this->actor.world.pos.x += pos.x * this->actor.scale.x;
        this->actor.world.pos.z += pos.z * this->actor.scale.z;
    }

    if (movementFlags & ANIM_FLAG_UPDATE_Y) {
        if (!(movementFlags & ANIM_FLAG_4)) {
            pos.y *= this->ageProperties->unk_08;
        }
        this->actor.world.pos.y += pos.y * this->actor.scale.y;
    }

    Player_Anim_ResetModelRotY(this);
}

void Player_AnimReplace_Setup(PlayState* play, Player* this, s32 movementFlags) {
    if (movementFlags & ANIM_FLAG_200) {
        Player_Anim_ResetPrevTranslRotFormScale(this);
    } else if ((movementFlags & ANIM_FLAG_100) || this->skelAnime.movementFlags) {
        Player_Anim_ResetPrevTranslRot(this);
    } else {
        this->skelAnime.prevTransl = this->skelAnime.jointTable[LIMB_ROOT_POS];
        this->skelAnime.prevYaw = this->actor.shape.rot.y;
    }

    this->skelAnime.movementFlags = movementFlags;
    Player_StopHorizontalMovement(this);
    AnimTaskQueue_DisableTransformTasksForGroup(play);
}

void Player_AnimReplace_PlayOnceSetSpeed(PlayState* play, Player* this, PlayerAnimationHeader* anim, s32 movementFlags,
                                         f32 playSpeed) {
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, playSpeed);
    Player_AnimReplace_Setup(play, this, movementFlags);
}

void Player_AnimReplace_PlayOnce(PlayState* play, Player* this, PlayerAnimationHeader* anim, s32 movementFlags) {
    Player_AnimReplace_PlayOnceSetSpeed(play, this, anim, movementFlags, PLAYER_ANIM_NORMAL_SPEED);
}

void Player_AnimReplace_PlayOnceAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim,
                                         s32 movementFlags) {
    Player_AnimReplace_PlayOnceSetSpeed(play, this, anim, movementFlags, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_AnimReplace_PlayOnceNormalAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    Player_AnimReplace_PlayOnceAdjusted(play, this, anim, ANIM_FLAG_4 | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_200);
}

void Player_AnimReplace_PlayLoopSetSpeed(PlayState* play, Player* this, PlayerAnimationHeader* anim, s32 movementFlags,
                                         f32 playSpeed) {
    PlayerAnimation_PlayLoopSetSpeed(play, &this->skelAnime, anim, playSpeed);
    Player_AnimReplace_Setup(play, this, movementFlags);
}

void Player_AnimReplace_PlayLoop(PlayState* play, Player* this, PlayerAnimationHeader* anim, s32 movementFlags) {
    Player_AnimReplace_PlayLoopSetSpeed(play, this, anim, movementFlags, PLAYER_ANIM_NORMAL_SPEED);
}

void Player_AnimReplace_PlayLoopAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim,
                                         s32 movementFlags) {
    Player_AnimReplace_PlayLoopSetSpeed(play, this, anim, movementFlags, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_AnimReplace_PlayLoopNormalAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    Player_AnimReplace_PlayLoopAdjusted(play, this, anim, ANIM_FLAG_4 | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE);
}

void Player_ProcessControlStick(PlayState* play, Player* this) {
    s8 spinAngle;
    s8 direction;

    this->prevControlStickMagnitude = sControlStickMagnitude;
    this->prevControlStickAngle = sControlStickAngle;

    Lib_GetControlStickData(&sControlStickMagnitude, &sControlStickAngle, sPlayerControlInput);

    if (sControlStickMagnitude < 8.0f) {
        sControlStickMagnitude = 0.0f;
    }

    sControlStickWorldYaw = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play)) + sControlStickAngle;

    this->controlStickDataIndex = (this->controlStickDataIndex + 1) % ARRAY_COUNT(this->controlStickSpinAngles);

    if (sControlStickMagnitude < 55.0f) {
        direction = PLAYER_STICK_DIR_NONE;
        spinAngle = -1;
    } else {
        spinAngle = ((u16)(sControlStickAngle + 0x2000)) >> 9;
        direction = ((u16)(BINANG_SUB(sControlStickWorldYaw, this->actor.shape.rot.y) + 0x2000)) >> 14;
    }

    this->controlStickSpinAngles[this->controlStickDataIndex] = spinAngle;
    this->controlStickDirections[this->controlStickDataIndex] = direction;
}

void Player_Anim_PlayOnceWaterAdjustment(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, sWaterSpeedFactor);
}

s32 Player_IsUsingZoraBoomerang(Player* this) {
    return this->stateFlags1 & PLAYER_STATE1_USING_ZORA_BOOMERANG;
}

#define CHEST_ANIM_SHORT 0
#define CHEST_ANIM_LONG 1

// TODO: consider what to do with the NONEs: cannot use a zero-argument macro like OoT since the text id is involved.
#define GET_ITEM(itemId, objectId, drawId, textId, field, chestAnim) \
    { itemId, field, (chestAnim != CHEST_ANIM_SHORT ? 1 : -1) * (drawId + 1), textId, objectId }

#define GIFIELD_GET_DROP_TYPE(field) ((field)&0x1F)
#define GIFIELD_20 (1 << 5)
#define GIFIELD_40 (1 << 6)
#define GIFIELD_NO_COLLECTIBLE (1 << 7)
/**
 * `flags` must be 0, GIFIELD_20, GIFIELD_40 or GIFIELD_NO_COLLECTIBLE (which can be or'ed together)
 * `dropType` must be either a value from the `Item00Type` enum or 0 if the `GIFIELD_NO_COLLECTIBLE` flag was used
 */
#define GIFIELD(flags, dropType) ((flags) | (dropType))

GetItemEntry sGetItemTable[GI_MAX - 1] = {
    // GI_RUPEE_GREEN
    GET_ITEM(ITEM_RUPEE_GREEN, OBJECT_GI_RUPY, GID_RUPEE_GREEN, 0xC4, GIFIELD(0, ITEM00_RUPEE_GREEN), CHEST_ANIM_SHORT),
    // GI_RUPEE_BLUE
    GET_ITEM(ITEM_RUPEE_BLUE, OBJECT_GI_RUPY, GID_RUPEE_BLUE, 0x2, GIFIELD(0, ITEM00_RUPEE_BLUE), CHEST_ANIM_SHORT),
    // GI_RUPEE_10
    GET_ITEM(ITEM_RUPEE_10, OBJECT_GI_RUPY, GID_RUPEE_RED, 0x3, GIFIELD(0, ITEM00_RUPEE_RED), CHEST_ANIM_SHORT),
    // GI_RUPEE_RED
    GET_ITEM(ITEM_RUPEE_RED, OBJECT_GI_RUPY, GID_RUPEE_RED, 0x4, GIFIELD(0, ITEM00_RUPEE_RED), CHEST_ANIM_SHORT),
    // GI_RUPEE_PURPLE
    GET_ITEM(ITEM_RUPEE_PURPLE, OBJECT_GI_RUPY, GID_RUPEE_PURPLE, 0x5, GIFIELD(0, ITEM00_RUPEE_PURPLE),
             CHEST_ANIM_SHORT),
    // GI_RUPEE_SILVER
    GET_ITEM(ITEM_RUPEE_SILVER, OBJECT_GI_RUPY, GID_RUPEE_SILVER, 0x6, GIFIELD(0, ITEM00_RUPEE_PURPLE),
             CHEST_ANIM_SHORT),
    // GI_RUPEE_HUGE
    GET_ITEM(ITEM_RUPEE_HUGE, OBJECT_GI_RUPY, GID_RUPEE_HUGE, 0x7, GIFIELD(0, ITEM00_RUPEE_HUGE), CHEST_ANIM_SHORT),
    // GI_WALLET_ADULT
    GET_ITEM(ITEM_WALLET_ADULT, OBJECT_GI_PURSE, GID_WALLET_ADULT, 0x8, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_WALLET_GIANT
    GET_ITEM(ITEM_WALLET_GIANT, OBJECT_GI_PURSE, GID_WALLET_GIANT, 0x9, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_RECOVERY_HEART
    GET_ITEM(ITEM_RECOVERY_HEART, OBJECT_GI_HEART, GID_RECOVERY_HEART, 0xA, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_0B
    GET_ITEM(ITEM_RECOVERY_HEART, OBJECT_GI_HEART, GID_RECOVERY_HEART, 0xB,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_HEART_PIECE
    GET_ITEM(ITEM_HEART_PIECE_2, OBJECT_GI_HEARTS, GID_HEART_PIECE, 0xC,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_HEART_CONTAINER
    GET_ITEM(ITEM_HEART_CONTAINER, OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xD,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MAGIC_JAR_SMALL
    GET_ITEM(ITEM_MAGIC_JAR_SMALL, OBJECT_GI_MAGICPOT, GID_MAGIC_JAR_SMALL, 0xE,
             GIFIELD(GIFIELD_20 | GIFIELD_40, ITEM00_MAGIC_JAR_SMALL), CHEST_ANIM_SHORT),
    // GI_MAGIC_JAR_BIG
    GET_ITEM(ITEM_MAGIC_JAR_BIG, OBJECT_GI_MAGICPOT, GID_MAGIC_JAR_BIG, 0xF,
             GIFIELD(GIFIELD_20 | GIFIELD_40, ITEM00_MAGIC_JAR_BIG), CHEST_ANIM_SHORT),
    // GI_10
    GET_ITEM(ITEM_RECOVERY_HEART, OBJECT_GI_HEART, GID_RECOVERY_HEART, 0x10, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_STRAY_FAIRY
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x11, 0, 0),
    // GI_12
    GET_ITEM(ITEM_RECOVERY_HEART, OBJECT_GI_HEART, GID_RECOVERY_HEART, 0x12, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_13
    GET_ITEM(ITEM_RECOVERY_HEART, OBJECT_GI_HEART, GID_RECOVERY_HEART, 0x13, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_BOMBS_1
    GET_ITEM(ITEM_BOMB, OBJECT_GI_BOMB_1, GID_BOMB, 0x14, GIFIELD(GIFIELD_40, ITEM00_BOMBS_0), CHEST_ANIM_SHORT),
    // GI_BOMBS_5
    GET_ITEM(ITEM_BOMBS_5, OBJECT_GI_BOMB_1, GID_BOMB, 0x15, GIFIELD(GIFIELD_40, ITEM00_BOMBS_0), CHEST_ANIM_SHORT),
    // GI_BOMBS_10
    GET_ITEM(ITEM_BOMBS_10, OBJECT_GI_BOMB_1, GID_BOMB, 0x16, GIFIELD(GIFIELD_40, ITEM00_BOMBS_0), CHEST_ANIM_SHORT),
    // GI_BOMBS_20
    GET_ITEM(ITEM_BOMBS_20, OBJECT_GI_BOMB_1, GID_BOMB, 0x17, GIFIELD(GIFIELD_40, ITEM00_BOMBS_0), CHEST_ANIM_SHORT),
    // GI_BOMBS_30
    GET_ITEM(ITEM_BOMBS_30, OBJECT_GI_BOMB_1, GID_BOMB, 0x18, GIFIELD(GIFIELD_40, ITEM00_BOMBS_0), CHEST_ANIM_SHORT),
    // GI_DEKU_STICKS_1
    GET_ITEM(ITEM_DEKU_STICK, OBJECT_GI_STICK, GID_DEKU_STICK, 0x19, GIFIELD(0, ITEM00_DEKU_STICK), CHEST_ANIM_SHORT),
    // GI_BOMBCHUS_10
    GET_ITEM(ITEM_BOMBCHUS_10, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x1A, GIFIELD(GIFIELD_40 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_SHORT),
    // GI_BOMB_BAG_20
    GET_ITEM(ITEM_BOMB_BAG_20, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_20, 0x1B,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_BOMB_BAG_30
    GET_ITEM(ITEM_BOMB_BAG_30, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_30, 0x1C,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_BOMB_BAG_40
    GET_ITEM(ITEM_BOMB_BAG_40, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_40, 0x1D,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_ARROWS_10
    GET_ITEM(ITEM_ARROWS_10, OBJECT_GI_ARROW, GID_ARROWS_SMALL, 0x1E, GIFIELD(GIFIELD_40, ITEM00_ARROWS_30),
             CHEST_ANIM_SHORT),
    // GI_ARROWS_30
    GET_ITEM(ITEM_ARROWS_30, OBJECT_GI_ARROW, GID_ARROWS_MEDIUM, 0x1F, GIFIELD(GIFIELD_40, ITEM00_ARROWS_40),
             CHEST_ANIM_SHORT),
    // GI_ARROWS_40
    GET_ITEM(ITEM_ARROWS_40, OBJECT_GI_ARROW, GID_ARROWS_LARGE, 0x20, GIFIELD(GIFIELD_40, ITEM00_ARROWS_50),
             CHEST_ANIM_SHORT),
    // GI_ARROWS_50
    GET_ITEM(ITEM_ARROWS_40, OBJECT_GI_ARROW, GID_ARROWS_LARGE, 0x21, GIFIELD(GIFIELD_40, ITEM00_ARROWS_50),
             CHEST_ANIM_SHORT),
    // GI_QUIVER_30
    GET_ITEM(ITEM_BOW, OBJECT_GI_BOW, GID_BOW, 0x22, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_QUIVER_40
    GET_ITEM(ITEM_QUIVER_40, OBJECT_GI_ARROWCASE, GID_QUIVER_40, 0x23, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_QUIVER_50
    GET_ITEM(ITEM_QUIVER_50, OBJECT_GI_ARROWCASE, GID_QUIVER_50, 0x24, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_ARROW_FIRE
    GET_ITEM(ITEM_ARROW_FIRE, OBJECT_GI_M_ARROW, GID_ARROW_FIRE, 0x25, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_ARROW_ICE
    GET_ITEM(ITEM_ARROW_ICE, OBJECT_GI_M_ARROW, GID_ARROW_ICE, 0x26, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_ARROW_LIGHT
    GET_ITEM(ITEM_ARROW_LIGHT, OBJECT_GI_M_ARROW, GID_ARROW_LIGHT, 0x27,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_DEKU_NUTS_1
    GET_ITEM(ITEM_DEKU_NUT, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0x28, GIFIELD(0, ITEM00_DEKU_NUTS_1), CHEST_ANIM_SHORT),
    // GI_DEKU_NUTS_5
    GET_ITEM(ITEM_DEKU_NUTS_5, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0x29, GIFIELD(0, ITEM00_DEKU_NUTS_1), CHEST_ANIM_SHORT),
    // GI_DEKU_NUTS_10
    GET_ITEM(ITEM_DEKU_NUTS_10, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0x2A, GIFIELD(0, ITEM00_DEKU_NUTS_1), CHEST_ANIM_SHORT),
    // GI_2B
    GET_ITEM(ITEM_DEKU_NUT_UPGRADE_30, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0x2B,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_SHORT),
    // GI_2C
    GET_ITEM(ITEM_DEKU_NUT_UPGRADE_30, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0x2C,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_SHORT),
    // GI_2D
    GET_ITEM(ITEM_DEKU_NUT_UPGRADE_40, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0x2D,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_SHORT),
    // GI_BOMBCHUS_20
    GET_ITEM(ITEM_BOMBCHUS_20, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x2E, GIFIELD(GIFIELD_40 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_SHORT),
    // GI_2F
    GET_ITEM(ITEM_DEKU_STICK_UPGRADE_20, OBJECT_GI_STICK, GID_DEKU_STICK, 0x2F,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_SHORT),
    // GI_30
    GET_ITEM(ITEM_DEKU_STICK_UPGRADE_20, OBJECT_GI_STICK, GID_DEKU_STICK, 0x30,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_SHORT),
    // GI_31
    GET_ITEM(ITEM_DEKU_STICK_UPGRADE_30, OBJECT_GI_STICK, GID_DEKU_STICK, 0x31,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_SHORT),
    // GI_SHIELD_HERO
    GET_ITEM(ITEM_SHIELD_HERO, OBJECT_GI_SHIELD_2, GID_SHIELD_HERO, 0x32,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_SHORT),
    // GI_SHIELD_MIRROR
    GET_ITEM(ITEM_SHIELD_MIRROR, OBJECT_GI_SHIELD_3, GID_SHIELD_MIRROR, 0x33,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_POWDER_KEG
    GET_ITEM(ITEM_POWDER_KEG, OBJECT_GI_BIGBOMB, GID_POWDER_KEG, 0x34, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MAGIC_BEANS
    GET_ITEM(ITEM_MAGIC_BEANS, OBJECT_GI_BEAN, GID_MAGIC_BEANS, 0x35, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_SHORT),
    // GI_BOMBCHUS_1
    GET_ITEM(ITEM_BOMBCHUS_1, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x36, GIFIELD(GIFIELD_40 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_SHORT),
    // GI_SWORD_KOKIRI
    GET_ITEM(ITEM_SWORD_KOKIRI, OBJECT_GI_SWORD_1, GID_SWORD_KOKIRI, 0x37,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_SWORD_RAZOR
    GET_ITEM(ITEM_SWORD_RAZOR, OBJECT_GI_SWORD_2, GID_SWORD_RAZOR, 0x38,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_SWORD_GILDED
    GET_ITEM(ITEM_SWORD_GILDED, OBJECT_GI_SWORD_3, GID_SWORD_GILDED, 0x39,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_BOMBCHUS_5
    GET_ITEM(ITEM_BOMBCHUS_5, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x3A, GIFIELD(GIFIELD_40 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_SHORT),
    // GI_SWORD_GREAT_FAIRY
    GET_ITEM(ITEM_SWORD_GREAT_FAIRY, OBJECT_GI_SWORD_4, GID_SWORD_GREAT_FAIRY, 0x3B,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_KEY_SMALL
    GET_ITEM(ITEM_KEY_SMALL, OBJECT_GI_KEY, GID_KEY_SMALL, 0x3C, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_SHORT),
    // GI_KEY_BOSS
    GET_ITEM(ITEM_KEY_BOSS, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, 0x3D, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MAP
    GET_ITEM(ITEM_DUNGEON_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, 0x3E, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_COMPASS
    GET_ITEM(ITEM_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, 0x3F, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_40
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x40, 0, 0),
    // GI_HOOKSHOT
    GET_ITEM(ITEM_HOOKSHOT, OBJECT_GI_HOOKSHOT, GID_HOOKSHOT, 0x41, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_LENS_OF_TRUTH
    GET_ITEM(ITEM_LENS_OF_TRUTH, OBJECT_GI_GLASSES, GID_LENS, 0x42, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_PICTOGRAPH_BOX
    GET_ITEM(ITEM_PICTOGRAPH_BOX, OBJECT_GI_CAMERA, GID_PICTOGRAPH_BOX, 0x43,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_44
    GET_ITEM(ITEM_PICTOGRAPH_BOX, OBJECT_UNSET_0, GID_NONE, 0x44, GIFIELD(0, ITEM00_RUPEE_GREEN), 0),
    // GI_45
    GET_ITEM(ITEM_RECOVERY_HEART, OBJECT_GI_HEART, GID_RECOVERY_HEART, 0x45, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_46
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x46, 0, 0),
    // GI_47
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x47, 0, 0),
    // GI_48
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x48, 0, 0),
    // GI_49
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x49, 0, 0),
    // GI_4A
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x4A, 0, 0),
    // GI_4B
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x4B, 0, 0),
    // GI_OCARINA_OF_TIME
    GET_ITEM(ITEM_OCARINA_OF_TIME, OBJECT_GI_OCARINA, GID_OCARINA, 0x4C,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_4D
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x4D, 0, 0),
    // GI_4E
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x4E, 0, 0),
    // GI_4F
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x4F, 0, 0),
    // GI_BOMBERS_NOTEBOOK
    GET_ITEM(ITEM_BOMBERS_NOTEBOOK, OBJECT_GI_SCHEDULE, GID_BOMBERS_NOTEBOOK, 0x50, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_51
    GET_ITEM(ITEM_NONE, OBJECT_GI_MAP, GID_STONE_OF_AGONY, 0x51, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_SKULL_TOKEN
    GET_ITEM(ITEM_SKULL_TOKEN, OBJECT_GI_SUTARU, GID_SKULL_TOKEN, 0x52, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_SHORT),
    // GI_53
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x53, 0, 0),
    // GI_54
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x54, 0, 0),
    // GI_REMAINS_ODOLWA
    GET_ITEM(ITEM_REMAINS_ODOLWA, OBJECT_UNSET_0, GID_REMAINS_ODOLWA, 0x55, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_REMAINS_GOHT
    GET_ITEM(ITEM_REMAINS_GOHT, OBJECT_UNSET_0, GID_REMAINS_GOHT, 0x56, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_REMAINS_GYORG
    GET_ITEM(ITEM_REMAINS_GYORG, OBJECT_UNSET_0, GID_REMAINS_GYORG, 0x57, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_REMAINS_TWINMOLD
    GET_ITEM(ITEM_REMAINS_TWINMOLD, OBJECT_UNSET_0, GID_REMAINS_TWINMOLD, 0x58, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_POTION_RED_BOTTLE
    GET_ITEM(ITEM_LONGSHOT, OBJECT_GI_BOTTLE_RED, GID_57, GIFIELD(GIFIELD_40, ITEM00_BOMBS_0),
             GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_BOTTLE
    GET_ITEM(ITEM_BOTTLE, OBJECT_GI_BOTTLE, GID_BOTTLE, 0x5A, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_POTION_RED
    GET_ITEM(ITEM_POTION_RED, OBJECT_GI_LIQUID, GID_POTION_RED, 0x5B, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_POTION_GREEN
    GET_ITEM(ITEM_POTION_GREEN, OBJECT_GI_LIQUID, GID_POTION_GREEN, 0x5C, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_POTION_BLUE
    GET_ITEM(ITEM_POTION_BLUE, OBJECT_GI_LIQUID, GID_POTION_BLUE, 0x5D, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_FAIRY
    GET_ITEM(ITEM_FAIRY, OBJECT_GI_BOTTLE_04, GID_FAIRY, 0x5E, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_DEKU_PRINCESS
    GET_ITEM(ITEM_FAIRY, OBJECT_GI_BOTTLE, GID_BOTTLE, 0x5F, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MILK_BOTTLE
    GET_ITEM(ITEM_MILK_BOTTLE, OBJECT_GI_MILK, GID_MILK, 0x60, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MILK_HALF
    GET_ITEM(ITEM_MILK_HALF, OBJECT_GI_MILK, GID_MILK, 0x61, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_FISH
    GET_ITEM(ITEM_FISH, OBJECT_GI_FISH, GID_FISH, 0x62, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_BUG
    GET_ITEM(ITEM_BUG, OBJECT_GI_INSECT, GID_BUG, 0x63, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_BLUE_FIRE
    GET_ITEM(ITEM_BLUE_FIRE, OBJECT_UNSET_0, GID_NONE, 0x64, GIFIELD(0, ITEM00_RUPEE_GREEN), 0),
    // GI_POE
    GET_ITEM(ITEM_BOTTLE, OBJECT_GI_BOTTLE, GID_BOTTLE, 0x65, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_BIG_POE
    GET_ITEM(ITEM_BIG_POE, OBJECT_GI_GHOST, GID_BIG_POE, 0x66, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_SPRING_WATER
    GET_ITEM(ITEM_SPRING_WATER, OBJECT_UNSET_0, GID_NONE, 0x67, GIFIELD(0, ITEM00_RUPEE_GREEN), 0),
    // GI_HOT_SPRING_WATER
    GET_ITEM(ITEM_HOT_SPRING_WATER, OBJECT_UNSET_0, GID_NONE, 0x68, GIFIELD(0, ITEM00_RUPEE_GREEN), 0),
    // GI_ZORA_EGG
    GET_ITEM(ITEM_ZORA_EGG, OBJECT_GI_BOTTLE_15, GID_ZORA_EGG, 0x69, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_GOLD_DUST
    GET_ITEM(ITEM_GOLD_DUST, OBJECT_GI_BOTTLE_16, GID_SEAHORSE, 0x6A, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MUSHROOM
    GET_ITEM(ITEM_MUSHROOM, OBJECT_GI_MAGICMUSHROOM, GID_MUSHROOM, 0x6B, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_6C
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x6C, GIFIELD(0, ITEM00_RUPEE_GREEN), 0),
    // GI_6D
    GET_ITEM(ITEM_BOTTLE, OBJECT_GI_BOTTLE, GID_BOTTLE, 0x6D, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_SEAHORSE
    GET_ITEM(ITEM_SEAHORSE, OBJECT_GI_BOTTLE_16, GID_SEAHORSE, 0x6E, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_CHATEAU_BOTTLE
    GET_ITEM(ITEM_CHATEAU, OBJECT_GI_BOTTLE_21, GID_CHATEAU, 0x6F, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_HYLIAN_LOACH
    GET_ITEM(ITEM_BOTTLE, OBJECT_GI_BOTTLE, GID_BOTTLE, 0x70, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_71
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x71, 0, 0),
    // GI_72
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x72, 0, 0),
    // GI_73
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x73, 0, 0),
    // GI_74
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x74, 0, 0),
    // GI_75
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x75, 0, 0),
    // GI_ICE_TRAP
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x76, 0, 0),
    // GI_77
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x77, 0, 0),
    // GI_MASK_DEKU
    GET_ITEM(ITEM_MASK_DEKU, OBJECT_GI_NUTSMASK, GID_MASK_DEKU, 0x78, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_GORON
    GET_ITEM(ITEM_MASK_GORON, OBJECT_GI_GOLONMASK, GID_MASK_GORON, 0x79,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_ZORA
    GET_ITEM(ITEM_MASK_ZORA, OBJECT_GI_ZORAMASK, GID_MASK_ZORA, 0x7A, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_FIERCE_DEITY
    GET_ITEM(ITEM_MASK_FIERCE_DEITY, OBJECT_GI_MASK03, GID_MASK_FIERCE_DEITY, 0x7B,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_CAPTAIN
    GET_ITEM(ITEM_MASK_CAPTAIN, OBJECT_GI_MASK18, GID_MASK_CAPTAIN, 0x7C,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_GIANT
    GET_ITEM(ITEM_MASK_GIANT, OBJECT_GI_MASK23, GID_MASK_GIANT, 0x7D, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_ALL_NIGHT
    GET_ITEM(ITEM_MASK_ALL_NIGHT, OBJECT_GI_MASK06, GID_MASK_ALL_NIGHT, 0x7E,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_BUNNY
    GET_ITEM(ITEM_MASK_BUNNY, OBJECT_GI_RABIT_MASK, GID_MASK_BUNNY, 0x7F,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_KEATON
    GET_ITEM(ITEM_MASK_KEATON, OBJECT_GI_KI_TAN_MASK, GID_MASK_KEATON, 0x80,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_GARO
    GET_ITEM(ITEM_MASK_GARO, OBJECT_GI_MASK09, GID_MASK_GARO, 0x81, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_ROMANI
    GET_ITEM(ITEM_MASK_ROMANI, OBJECT_GI_MASK10, GID_MASK_ROMANI, 0x82, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_CIRCUS_LEADER
    GET_ITEM(ITEM_MASK_CIRCUS_LEADER, OBJECT_GI_MASK11, GID_MASK_CIRCUS_LEADER, 0x83,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_POSTMAN
    GET_ITEM(ITEM_MASK_POSTMAN, OBJECT_GI_MASK12, GID_MASK_POSTMAN, 0x84,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_COUPLE
    GET_ITEM(ITEM_MASK_COUPLE, OBJECT_GI_MASK13, GID_MASK_COUPLE, 0x85, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_GREAT_FAIRY
    GET_ITEM(ITEM_MASK_GREAT_FAIRY, OBJECT_GI_MASK14, GID_MASK_GREAT_FAIRY, 0x86,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_GIBDO
    GET_ITEM(ITEM_MASK_GIBDO, OBJECT_GI_MASK15, GID_MASK_GIBDO, 0x87, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_DON_GERO
    GET_ITEM(ITEM_MASK_DON_GERO, OBJECT_GI_MASK16, GID_MASK_DON_GERO, 0x88,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_KAMARO
    GET_ITEM(ITEM_MASK_KAMARO, OBJECT_GI_MASK17, GID_MASK_KAMARO, 0x89, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_TRUTH
    GET_ITEM(ITEM_MASK_TRUTH, OBJECT_GI_TRUTH_MASK, GID_MASK_TRUTH, 0x8A,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_STONE
    GET_ITEM(ITEM_MASK_STONE, OBJECT_GI_STONEMASK, GID_MASK_STONE, 0x8B,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_MASK_BREMEN
    GET_ITEM(ITEM_MASK_BREMEN, OBJECT_GI_MASK20, GID_MASK_BREMEN, 0x8C, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_BLAST
    GET_ITEM(ITEM_MASK_BLAST, OBJECT_GI_MASK21, GID_MASK_BLAST, 0x8D, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_SCENTS
    GET_ITEM(ITEM_MASK_SCENTS, OBJECT_GI_MASK22, GID_MASK_SCENTS, 0x8E, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MASK_KAFEIS_MASK
    GET_ITEM(ITEM_MASK_KAFEIS_MASK, OBJECT_GI_MASK05, GID_MASK_KAFEIS_MASK, 0x8F,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_90
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0x90, 0, 0),
    // GI_CHATEAU
    GET_ITEM(ITEM_CHATEAU_2, OBJECT_GI_BOTTLE_21, GID_CHATEAU, 0x91, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MILK
    GET_ITEM(ITEM_MILK, OBJECT_GI_MILK, GID_MILK, 0x92, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_GOLD_DUST_2
    GET_ITEM(ITEM_GOLD_DUST_2, OBJECT_GI_GOLD_DUST, GID_GOLD_DUST, 0x93, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_HYLIAN_LOACH_2
    GET_ITEM(ITEM_HYLIAN_LOACH_2, OBJECT_GI_LOACH, GID_HYLIAN_LOACH, 0x94, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_SEAHORSE_CAUGHT
    GET_ITEM(ITEM_SEAHORSE_CAUGHT, OBJECT_GI_SEAHORSE, GID_SEAHORSE_CAUGHT, 0x95, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_MOONS_TEAR
    GET_ITEM(ITEM_MOONS_TEAR, OBJECT_GI_RESERVE00, GID_MOONS_TEAR, 0x96, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_DEED_LAND
    GET_ITEM(ITEM_DEED_LAND, OBJECT_GI_RESERVE01, GID_DEED_LAND, 0x97, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_DEED_SWAMP
    GET_ITEM(ITEM_DEED_SWAMP, OBJECT_GI_RESERVE01, GID_DEED_SWAMP, 0x98, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_DEED_MOUNTAIN
    GET_ITEM(ITEM_DEED_MOUNTAIN, OBJECT_GI_RESERVE01, GID_DEED_MOUNTAIN, 0x99, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_DEED_OCEAN
    GET_ITEM(ITEM_DEED_OCEAN, OBJECT_GI_RESERVE01, GID_DEED_OCEAN, 0x9A, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_SWORD_GREAT_FAIRY_STOLEN
    GET_ITEM(ITEM_SWORD_GREAT_FAIRY, OBJECT_GI_SWORD_4, GID_SWORD_GREAT_FAIRY, 0x9B,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_SWORD_KOKIRI_STOLEN
    GET_ITEM(ITEM_SWORD_KOKIRI, OBJECT_GI_SWORD_1, GID_SWORD_KOKIRI, 0x9C,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_SWORD_RAZOR_STOLEN
    GET_ITEM(ITEM_SWORD_RAZOR, OBJECT_GI_SWORD_2, GID_SWORD_RAZOR, 0x9D,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_SWORD_GILDED_STOLEN
    GET_ITEM(ITEM_SWORD_GILDED, OBJECT_GI_SWORD_3, GID_SWORD_GILDED, 0x9E,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_SHIELD_HERO_STOLEN
    GET_ITEM(ITEM_SHIELD_HERO, OBJECT_GI_SHIELD_2, GID_SHIELD_HERO, 0x9F,
             GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_SHORT),
    // GI_ROOM_KEY
    GET_ITEM(ITEM_ROOM_KEY, OBJECT_GI_RESERVE_B_00, GID_ROOM_KEY, 0xA0, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_LETTER_TO_MAMA
    GET_ITEM(ITEM_LETTER_MAMA, OBJECT_GI_RESERVE_B_01, GID_LETTER_MAMA, 0xA1, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_A2
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xA2, 0, 0),
    // GI_A3
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xA3, 0, 0),
    // GI_A4
    GET_ITEM(ITEM_NONE, OBJECT_GI_KI_TAN_MASK, GID_MASK_KEATON, 0xA4, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_A5
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xA5, 0, 0),
    // GI_A6
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xA6, 0, 0),
    // GI_A7
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xA7, 0, 0),
    // GI_A8
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xA8, 0, 0),
    // GI_BOTTLE_STOLEN
    GET_ITEM(ITEM_BOTTLE, OBJECT_GI_BOTTLE, GID_BOTTLE, 0xA9, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_LETTER_TO_KAFEI
    GET_ITEM(ITEM_LETTER_TO_KAFEI, OBJECT_GI_RESERVE_C_00, GID_LETTER_TO_KAFEI, 0xAA,
             GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_PENDANT_OF_MEMORIES
    GET_ITEM(ITEM_PENDANT_OF_MEMORIES, OBJECT_GI_RESERVE_C_01, GID_PENDANT_OF_MEMORIES, 0xAB,
             GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_AC
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xAC, 0, 0),
    // GI_AD
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xAD, 0, 0),
    // GI_AE
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xAE, 0, 0),
    // GI_AF
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xAF, 0, 0),
    // GI_B0
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xB0, 0, 0),
    // GI_B1
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xB1, 0, 0),
    // GI_B2
    GET_ITEM(ITEM_NONE, OBJECT_UNSET_0, GID_NONE, 0xB2, 0, 0),
    // GI_B3
    GET_ITEM(ITEM_NONE, OBJECT_GI_MSSA, GID_MASK_SUN, 0xB3, GIFIELD(GIFIELD_NO_COLLECTIBLE, 0), CHEST_ANIM_LONG),
    // GI_TINGLE_MAP_CLOCK_TOWN
    GET_ITEM(ITEM_TINGLE_MAP, OBJECT_GI_FIELDMAP, GID_TINGLE_MAP, 0xB4, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_TINGLE_MAP_WOODFALL
    GET_ITEM(ITEM_TINGLE_MAP, OBJECT_GI_FIELDMAP, GID_TINGLE_MAP, 0xB5, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_TINGLE_MAP_SNOWHEAD
    GET_ITEM(ITEM_TINGLE_MAP, OBJECT_GI_FIELDMAP, GID_TINGLE_MAP, 0xB6, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_TINGLE_MAP_ROMANI_RANCH
    GET_ITEM(ITEM_TINGLE_MAP, OBJECT_GI_FIELDMAP, GID_TINGLE_MAP, 0xB7, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_TINGLE_MAP_GREAT_BAY
    GET_ITEM(ITEM_TINGLE_MAP, OBJECT_GI_FIELDMAP, GID_TINGLE_MAP, 0xB8, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // GI_TINGLE_MAP_STONE_TOWER
    GET_ITEM(ITEM_TINGLE_MAP, OBJECT_GI_FIELDMAP, GID_TINGLE_MAP, 0xB9, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             CHEST_ANIM_LONG),
    // #region 2S2H [Enhancement] Added to enable custom item gives
    // GI_SHIP
    GET_ITEM(ITEM_SHIP, OBJECT_UNSET_0, GID_NONE, CUSTOM_MESSAGE_ID, GIFIELD(GIFIELD_20 | GIFIELD_NO_COLLECTIBLE, 0),
             0),
    // #endregion
};

// Player_UpdateCurrentGetItemDrawId?
void func_8082ECE0(Player* this) {
    GetItemEntry* giEntry = &sGetItemTable[this->getItemId - 1];

    this->getItemDrawIdPlusOne = ABS_ALT(giEntry->gid);
}

typedef enum FidgetType {
    /* 0x0 */ FIDGET_LOOK_AROUND,
    /* 0x1 */ FIDGET_COLD,
    /* 0x2 */ FIDGET_WARM,
    /* 0x3 */ FIDGET_HOT, // same animations as FIDGET_WARM
    /* 0x4 */ FIDGET_STRETCH_1,
    /* 0x5 */ FIDGET_STRETCH_2, // same animations as FIDGET_STRETCH_1
    /* 0x6 */ FIDGET_STRETCH_3, // same animations as FIDGET_STRETCH_1
    /* 0x7 */ FIDGET_CRIT_HEALTH_START,
    /* 0x8 */ FIDGET_CRIT_HEALTH_LOOP,
    /* 0x9 */ FIDGET_SWORD_SWING,
    /* 0xA */ FIDGET_ADJUST_TUNIC,
    /* 0xB */ FIDGET_TAP_FEET,
    /* 0xC */ FIDGET_ADJUST_SHIELD,
    /* 0xD */ FIDGET_SWORD_SWING_TWO_HAND,
    /* 0xE */ FIDGET_SNIFF // for mask of scents. Only used for animSfx
} FidgetType;

PlayerAnimationHeader* sFidgetAnimations[][2] = {
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
    { &gPlayerAnim_link_wait_itemD2_20f, &gPlayerAnim_link_wait_itemD2_20f },

    // FIDGET_SNIFF
    { &gPlayerAnim_cl_msbowait, &gPlayerAnim_cl_msbowait },
};

AnimSfxEntry sFidgetAnimSfxSneeze[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 8, NA_SE_VO_LI_SNEEZE, STOP),
};
AnimSfxEntry sFidgetAnimSfxSweat[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 18, NA_SE_VO_LI_SWEAT, STOP),
};
AnimSfxEntry sFidgetAnimSfxCritHealthStart[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 13, NA_SE_VO_LI_BREATH_REST, STOP),
};
AnimSfxEntry sFidgetAnimSfxCritHealthLoop[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 10, NA_SE_VO_LI_BREATH_REST, STOP),
};

AnimSfxEntry sFidgetAnimSfxTunic[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 44, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 48, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 52, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 56, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 60, NA_SE_PL_CALM_HIT, STOP),
};

AnimSfxEntry sFidgetAnimSfxTapFeet[] = {
    ANIMSFX(ANIMSFX_TYPE_8, 25, NA_SE_NONE, CONTINUE), ANIMSFX(ANIMSFX_TYPE_8, 30, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_8, 44, NA_SE_NONE, CONTINUE), ANIMSFX(ANIMSFX_TYPE_8, 48, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_8, 52, NA_SE_NONE, CONTINUE), ANIMSFX(ANIMSFX_TYPE_8, 56, NA_SE_NONE, STOP),
};

AnimSfxEntry sFidgetAnimSfxShield[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 16, NA_SE_IT_SHIELD_SWING, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 20, NA_SE_IT_SHIELD_SWING, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 70, NA_SE_IT_SHIELD_SWING, STOP),
};

AnimSfxEntry sFidgetAnimSfxSword[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 10, NA_SE_IT_HAMMER_SWING, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 10, NA_SE_VO_LI_AUTO_JUMP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 22, NA_SE_IT_SWORD_SWING, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 22, NA_SE_VO_LI_SWORD_N, STOP),
};

AnimSfxEntry sFidgetAnimSfxSwordTwoHand[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 39, NA_SE_IT_SWORD_SWING, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 39, NA_SE_VO_LI_SWORD_N, STOP),
};
AnimSfxEntry sFidgetAnimSfxStretch[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 20, NA_SE_VO_LI_RELAX, STOP),
};

AnimSfxEntry sFidgetAnimSfxPigGrunt[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 4, NA_SE_VO_LI_POO_WAIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 12, NA_SE_VO_LI_POO_WAIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 30, NA_SE_VO_LI_POO_WAIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 61, NA_SE_VO_LI_POO_WAIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 68, NA_SE_VO_LI_POO_WAIT, STOP),
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
    /* 0xA */ FIDGET_ANIMSFX_STRETCH,
    /* 0xB */ FIDGET_ANIMSFX_PIG_GRUNT
} FidgetAnimSfxType;

AnimSfxEntry* sFidgetAnimSfxLists[] = {
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
    sFidgetAnimSfxPigGrunt,        // FIDGET_ANIMSFX_PIG_GRUNT
    NULL,                          // unused entry
};

/**
 * The indices in this array correspond 1 to 1 with the entries of sFidgetAnimations.
 */
u8 sFidgetAnimSfxTypes[] = {
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
    FIDGET_ANIMSFX_PIG_GRUNT,         // FIDGET_SNIFF
    FIDGET_ANIMSFX_PIG_GRUNT,         // FIDGET_SNIFF (sword/shield in hand)
};

/**
 * Get the appropriate Idle animation based on either current `modelAnimType`,
 * or special cases for zora, non-player (kafei), goron, or while wearing mask of scents.
 * This is the default idle animation.
 *
 * For fidget idle animations (which can for example, change based on environment)
 * see `sFidgetAnimations`.
 */
PlayerAnimationHeader* Player_GetIdleAnim(Player* this) {
    if ((this->transformation == PLAYER_FORM_ZORA) || (this->actor.id != ACTOR_PLAYER)) {
        return &gPlayerAnim_pz_wait;
    }
    if (this->transformation == PLAYER_FORM_GORON) {
        return &gPlayerAnim_pg_wait;
    }
    if (this->currentMask == PLAYER_MASK_SCENTS) {
        return &gPlayerAnim_cl_msbowait;
    }
    return D_8085BE84[PLAYER_ANIMGROUP_wait][this->modelAnimType];
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
    if (!BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_newroll_jump_end_20f) &&
        !BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_newside_jump_end_20f)) {
        if (!BEN_ANIM_EQUAL(this->skelAnime.animation, Player_GetIdleAnim(this)) ||
            BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_cl_msbowait)) {
            PlayerAnimationHeader** fidgetAnim;
            s32 i;

            for (i = 0, fidgetAnim = &sFidgetAnimations[0][0]; i < ARRAY_COUNT_2D(sFidgetAnimations); i++) {
                if (this->skelAnime.animation == *fidgetAnim) {
                    return i + 1;
                }
                fidgetAnim++;
            }

            return IDLE_ANIM_NONE;
        }
    }

    return IDLE_ANIM_DEFAULT;
}

void Player_ProcessFidgetAnimSfxList(Player* this, s32 fidgetAnimIndex) {
    if (sFidgetAnimSfxTypes[fidgetAnimIndex] != FIDGET_ANIMSFX_NONE) {
        Player_PlayAnimSfx(this, sFidgetAnimSfxLists[sFidgetAnimSfxTypes[fidgetAnimIndex] - 1]);
    }
}

PlayerAnimationHeader* func_8082EEE0(Player* this) {
    if (this->unk_B64 != 0) {
        return D_8085BE84[PLAYER_ANIMGROUP_damage_run][this->modelAnimType];
    } else {
        return D_8085BE84[PLAYER_ANIMGROUP_run][this->modelAnimType];
    }
}

bool func_8082EF20(Player* this) {
    return Player_IsUsingZoraBoomerang(this) && (this->unk_ACC != 0);
}

PlayerAnimationHeader* func_8082EF54(Player* this) {
    if (func_8082EF20(this)) {
        return &gPlayerAnim_link_boom_throw_waitR;
    } else {
        return D_8085BE84[PLAYER_ANIMGROUP_waitR][this->modelAnimType];
    }
}

PlayerAnimationHeader* func_8082EF9C(Player* this) {
    if (func_8082EF20(this)) {
        return &gPlayerAnim_link_boom_throw_waitL;
    } else {
        return D_8085BE84[PLAYER_ANIMGROUP_waitL][this->modelAnimType];
    }
}

PlayerAnimationHeader* func_8082EFE4(Player* this) {
    if (func_800B7128(this)) {
        return &gPlayerAnim_link_bow_side_walk;
    } else {
        return D_8085BE84[PLAYER_ANIMGROUP_side_walk][this->modelAnimType];
    }
}

void Player_LerpEnvLighting(PlayState* play, PlayerEnvLighting* lighting, f32 lerp) {
    Environment_LerpAmbientColor(play, &lighting->ambientColor, lerp);
    Environment_LerpDiffuseColor(play, &lighting->diffuseColor, lerp);
    Environment_LerpFogColor(play, &lighting->fogColor, lerp);
    Environment_LerpFog(play, lighting->fogNear, lighting->zFar, lerp);
}

/**
 * Revert cylinder to normal properties
 */
void Player_ResetCylinder(Player* this) {
    this->cylinder.base.colMaterial = COL_MATERIAL_HIT5;
    this->cylinder.base.atFlags = AT_NONE;
    this->cylinder.base.acFlags = AC_ON | AC_TYPE_ENEMY;
    this->cylinder.base.ocFlags1 = OC1_ON | OC1_TYPE_ALL;
    this->cylinder.elem.elemMaterial = ELEM_MATERIAL_UNK1;
    this->cylinder.elem.atDmgInfo.dmgFlags = 0;
    this->cylinder.elem.acDmgInfo.dmgFlags = 0xF7CFFFFF;
    this->cylinder.elem.atElemFlags = ATELEM_NONE | ATELEM_SFX_NORMAL;
    this->cylinder.dim.radius = 12;
}

/**
 * Give cylinder special properties for attacks, uses include
 * - Normal roll
 * - Deku spin
 * - Deku launch
 * - Goron pound
 * - Goron spike roll
 * - Zora barrier
 *
 * and possibly more.
 *
 * @param dmgFlags Damage flags (DMGFLAG defines)
 * @param damage to do
 * @param radius of cylinder
 */
void Player_SetCylinderForAttack(Player* this, u32 dmgFlags, s32 damage, s32 radius) {
    this->cylinder.base.atFlags = AT_ON | AT_TYPE_PLAYER;
    if (radius > 30) {
        this->cylinder.base.ocFlags1 = OC1_NONE;
    } else {
        if (GameInteractor_Should(VB_SET_PLAYER_CYLINDER_OC_FLAGS, true, this, dmgFlags)) {
            this->cylinder.base.ocFlags1 = OC1_ON | OC1_TYPE_ALL;
        }
    }

    this->cylinder.elem.elemMaterial = ELEM_MATERIAL_UNK2;
    this->cylinder.elem.atElemFlags = ATELEM_ON | ATELEM_NEAREST | ATELEM_SFX_NORMAL;
    this->cylinder.dim.radius = radius;
    this->cylinder.elem.atDmgInfo.dmgFlags = dmgFlags;
    this->cylinder.elem.atDmgInfo.damage = damage;

    if (dmgFlags & DMG_GORON_POUND) {
        this->cylinder.base.acFlags = AC_NONE;
    } else {
        this->cylinder.base.colMaterial = COL_MATERIAL_NONE;
        this->cylinder.elem.acDmgInfo.dmgFlags = 0xF7CFFFFF;

        if (dmgFlags & DMG_ZORA_BARRIER) {
            this->cylinder.base.acFlags = AC_NONE;
        } else {
            this->cylinder.base.acFlags = AC_ON | AC_TYPE_ENEMY;
        }
    }
}

// Check for starting Zora barrier
void func_8082F164(Player* this, u16 button) {
    if ((this->transformation == PLAYER_FORM_ZORA) && CHECK_BTN_ALL(sPlayerControlInput->cur.button, button)) {
        this->stateFlags1 |= PLAYER_STATE1_10;
    }
}

PlayerEnvLighting sZoraBarrierEnvLighting = {
    { 0, 0, 0 },       // ambientColor
    { 255, 255, 155 }, // diffuseColor
    { 20, 20, 50 },    // fogColor
    940,               // fogNear
    5000,              // zFar
};

// Run Zora Barrier
void func_8082F1AC(PlayState* play, Player* this) {
    s32 sp4C = this->unk_B62;
    f32 temp;
    s16 sp46;
    s16 sp44;
    f32 sp40;
    f32 sp3C;
    s32 var_v0;

    if ((gSaveContext.save.saveInfo.playerData.magic != 0) && (this->stateFlags1 & PLAYER_STATE1_10)) {
        if (gSaveContext.magicState == MAGIC_STATE_IDLE) {
            Magic_Consume(play, 0, MAGIC_CONSUME_GORON_ZORA);
        }

        temp = 16.0f;
        if (gSaveContext.save.saveInfo.playerData.magic >= 16) {
            var_v0 = 255;
        } else {
            var_v0 = (gSaveContext.save.saveInfo.playerData.magic / temp) * 255.0f;
        }
        Math_StepToS(&this->unk_B62, var_v0, 50);
    } else if (Math_StepToS(&this->unk_B62, 0, 50) && (gSaveContext.magicState != MAGIC_STATE_IDLE)) {
        Magic_Reset(play);
    }

    if ((this->unk_B62 != 0) || (sp4C != 0)) {
        f32 sp34;
        f32 new_var;

        sp46 = play->gameplayFrames * 7000;
        sp44 = play->gameplayFrames * 14000;
        Player_LerpEnvLighting(play, &sZoraBarrierEnvLighting, this->unk_B62 / 255.0f);

        sp34 = Math_SinS(sp44) * 40.0f;
        sp40 = Math_CosS(sp44) * 40.0f;
        sp3C = Math_SinS(sp46) * sp34;
        new_var = Math_CosS(sp46) * sp34;

        Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x + sp40, this->actor.world.pos.y + sp3C,
                                  this->actor.world.pos.z + new_var, 100, 200, 255, 600);

        Player_PlaySfx(this, NA_SE_PL_ZORA_SPARK_BARRIER - SFX_FLAG);
        Actor_SetPlayerImpact(play, PLAYER_IMPACT_ZORA_BARRIER, 2, 100.0f, &this->actor.world.pos);
    }
}

void Player_SetUpperAction(PlayState* play, Player* this, PlayerUpperActionFunc upperActionFunc) {
    this->upperActionFunc = upperActionFunc;
    this->unk_ACE = 0;
    this->skelAnimeUpperBlendWeight = 0.0f;
    func_8082E00C(this);
}

#define GET_PLAYER_ANIM(group, type) ((PlayerAnimationHeader**)D_8085BE84)[group * PLAYER_ANIMTYPE_MAX + type]

void Player_InitItemActionWithAnim(PlayState* play, Player* this, PlayerItemAction itemAction) {
    PlayerAnimationHeader* curAnim = this->skelAnime.animation;
    PlayerAnimationHeader*(*iter)[PLAYER_ANIMTYPE_MAX] = (void*)&D_8085BE84[0][this->modelAnimType];
    s32 animGroup;

    this->stateFlags1 &= ~(PLAYER_STATE1_8 | PLAYER_STATE1_USING_ZORA_BOOMERANG);

    for (animGroup = 0; animGroup < PLAYER_ANIMGROUP_MAX; animGroup++) {
        if (BEN_ANIM_EQUAL(curAnim, **iter)) {
            break;
        }
        iter++;
    }

    Player_InitItemAction(play, this, itemAction);

    if (animGroup < PLAYER_ANIMGROUP_MAX) {
        this->skelAnime.animation = GET_PLAYER_ANIM(animGroup, this->modelAnimType);
    }
}

s8 sItemItemActions[] = {
    PLAYER_IA_OCARINA,                 // ITEM_OCARINA_OF_TIME,
    PLAYER_IA_BOW,                     // ITEM_BOW,
    PLAYER_IA_BOW_FIRE,                // ITEM_ARROW_FIRE,
    PLAYER_IA_BOW_ICE,                 // ITEM_ARROW_ICE,
    PLAYER_IA_BOW_LIGHT,               // ITEM_ARROW_LIGHT,
    PLAYER_IA_PICTOGRAPH_BOX,          // ITEM_OCARINA_FAIRY,
    PLAYER_IA_BOMB,                    // ITEM_BOMB,
    PLAYER_IA_BOMBCHU,                 // ITEM_BOMBCHU,
    PLAYER_IA_DEKU_STICK,              // ITEM_DEKU_STICK,
    PLAYER_IA_DEKU_NUT,                // ITEM_DEKU_NUT,
    PLAYER_IA_MAGIC_BEANS,             // ITEM_MAGIC_BEANS,
    PLAYER_IA_PICTOGRAPH_BOX,          // ITEM_SLINGSHOT,
    PLAYER_IA_POWDER_KEG,              // ITEM_POWDER_KEG,
    PLAYER_IA_PICTOGRAPH_BOX,          // ITEM_PICTOGRAPH_BOX,
    PLAYER_IA_LENS_OF_TRUTH,           // ITEM_LENS_OF_TRUTH,
    PLAYER_IA_HOOKSHOT,                // ITEM_HOOKSHOT,
    PLAYER_IA_SWORD_TWO_HANDED,        // ITEM_SWORD_GREAT_FAIRY,
    PLAYER_IA_PICTOGRAPH_BOX,          // ITEM_LONGSHOT, // OoT Leftover
    PLAYER_IA_BOTTLE_EMPTY,            // ITEM_BOTTLE,
    PLAYER_IA_BOTTLE_POTION_RED,       // ITEM_POTION_RED,
    PLAYER_IA_BOTTLE_POTION_GREEN,     // ITEM_POTION_GREEN,
    PLAYER_IA_BOTTLE_POTION_BLUE,      // ITEM_POTION_BLUE,
    PLAYER_IA_BOTTLE_FAIRY,            // ITEM_FAIRY,
    PLAYER_IA_BOTTLE_DEKU_PRINCESS,    // ITEM_DEKU_PRINCESS,
    PLAYER_IA_BOTTLE_MILK,             // ITEM_MILK_BOTTLE,
    PLAYER_IA_BOTTLE_MILK_HALF,        // ITEM_MILK_HALF,
    PLAYER_IA_BOTTLE_FISH,             // ITEM_FISH,
    PLAYER_IA_BOTTLE_BUG,              // ITEM_BUG,
    PLAYER_IA_BOTTLE_BUG,              // ITEM_BLUE_FIRE, // !
    PLAYER_IA_BOTTLE_POE,              // ITEM_POE,
    PLAYER_IA_BOTTLE_BIG_POE,          // ITEM_BIG_POE,
    PLAYER_IA_BOTTLE_SPRING_WATER,     // ITEM_SPRING_WATER,
    PLAYER_IA_BOTTLE_HOT_SPRING_WATER, // ITEM_HOT_SPRING_WATER,
    PLAYER_IA_BOTTLE_ZORA_EGG,         // ITEM_ZORA_EGG,
    PLAYER_IA_BOTTLE_GOLD_DUST,        // ITEM_GOLD_DUST,
    PLAYER_IA_BOTTLE_MUSHROOM,         // ITEM_MUSHROOM,
    PLAYER_IA_BOTTLE_SEAHORSE,         // ITEM_SEA_HORSE,
    PLAYER_IA_BOTTLE_CHATEAU,          // ITEM_CHATEAU,
    PLAYER_IA_BOTTLE_HYLIAN_LOACH,     // ITEM_HYLIAN_LOACH,
    PLAYER_IA_BOTTLE_POE,              // ITEM_OBABA_DRINK, // !
    PLAYER_IA_MOONS_TEAR,              // ITEM_MOONS_TEAR,
    PLAYER_IA_DEED_LAND,               // ITEM_DEED_LAND,
    PLAYER_IA_DEED_SWAMP,              // ITEM_DEED_SWAMP,
    PLAYER_IA_DEED_MOUNTAIN,           // ITEM_DEED_MOUNTAIN,
    PLAYER_IA_DEED_OCEAN,              // ITEM_DEED_OCEAN,
    PLAYER_IA_ROOM_KEY,                // ITEM_ROOM_KEY,
    PLAYER_IA_LETTER_MAMA,             // ITEM_LETTER_MAMA,
    PLAYER_IA_LETTER_TO_KAFEI,         // ITEM_LETTER_TO_KAFEI,
    PLAYER_IA_PENDANT_OF_MEMORIES,     // ITEM_PENDANT_MEMORIES,
    PLAYER_IA_38,                      // ITEM_TINGLE_MAP, // !
    PLAYER_IA_MASK_DEKU,               // ITEM_MASK_DEKU,
    PLAYER_IA_MASK_GORON,              // ITEM_MASK_GORON,
    PLAYER_IA_MASK_ZORA,               // ITEM_MASK_ZORA,
    PLAYER_IA_MASK_FIERCE_DEITY,       // ITEM_MASK_FIERCE_DEITY,
    PLAYER_IA_MASK_TRUTH,              // ITEM_MASK_TRUTH,
    PLAYER_IA_MASK_KAFEIS_MASK,        // ITEM_MASK_KAFEIS_MASK,
    PLAYER_IA_MASK_ALL_NIGHT,          // ITEM_MASK_ALL_NIGHT,
    PLAYER_IA_MASK_BUNNY,              // ITEM_MASK_BUNNY,
    PLAYER_IA_MASK_KEATON,             // ITEM_MASK_KEATON,
    PLAYER_IA_MASK_GARO,               // ITEM_MASK_GARO,
    PLAYER_IA_MASK_ROMANI,             // ITEM_MASK_ROMANI,
    PLAYER_IA_MASK_CIRCUS_LEADER,      // ITEM_MASK_CIRCUS_LEADER,
    PLAYER_IA_MASK_POSTMAN,            // ITEM_MASK_POSTMAN,
    PLAYER_IA_MASK_COUPLE,             // ITEM_MASK_COUPLE,
    PLAYER_IA_MASK_GREAT_FAIRY,        // ITEM_MASK_GREAT_FAIRY,
    PLAYER_IA_MASK_GIBDO,              // ITEM_MASK_GIBDO,
    PLAYER_IA_MASK_DON_GERO,           // ITEM_MASK_DON_GERO,
    PLAYER_IA_MASK_KAMARO,             // ITEM_MASK_KAMARO,
    PLAYER_IA_MASK_CAPTAIN,            // ITEM_MASK_CAPTAIN,
    PLAYER_IA_MASK_STONE,              // ITEM_MASK_STONE,
    PLAYER_IA_MASK_BREMEN,             // ITEM_MASK_BREMEN,
    PLAYER_IA_MASK_BLAST,              // ITEM_MASK_BLAST,
    PLAYER_IA_MASK_SCENTS,             // ITEM_MASK_SCENTS,
    PLAYER_IA_MASK_GIANT,              // ITEM_MASK_GIANT,
    PLAYER_IA_BOW_FIRE,                // ITEM_BOW_FIRE,
    PLAYER_IA_BOW_ICE,                 // ITEM_BOW_ICE,
    PLAYER_IA_BOW_LIGHT,               // ITEM_BOW_LIGHT,
    PLAYER_IA_SWORD_KOKIRI,            // ITEM_SWORD_KOKIRI,
    PLAYER_IA_SWORD_RAZOR,             // ITEM_SWORD_RAZOR,
    PLAYER_IA_SWORD_GILDED,            // ITEM_SWORD_GILDED,
    PLAYER_IA_SWORD_TWO_HANDED,        // ITEM_SWORD_DEITY,
};

PlayerItemAction Player_ItemToItemAction(Player* this, ItemId item) {
    if (item >= ITEM_FD) {
        return PLAYER_IA_NONE;
    } else if (item == ITEM_FC) {
        return PLAYER_IA_LAST_USED;
    } else if (item == ITEM_FISHING_ROD) {
        return PLAYER_IA_FISHING_ROD;
    } else if ((item == ITEM_SWORD_KOKIRI) && (this->transformation == PLAYER_FORM_ZORA)) {
        return PLAYER_IA_ZORA_BOOMERANG;
    } else {
        return sItemItemActions[item];
    }
}

PlayerUpperActionFunc sItemActionUpdateFuncs[PLAYER_IA_MAX] = {
    Player_UpperAction_0,          // PLAYER_IA_NONE
    Player_UpperAction_0,          // PLAYER_IA_LAST_USED
    Player_UpperAction_0,          // PLAYER_IA_FISHING_ROD
    Player_UpperAction_1,          // PLAYER_IA_SWORD_KOKIRI
    Player_UpperAction_1,          // PLAYER_IA_SWORD_RAZOR
    Player_UpperAction_1,          // PLAYER_IA_SWORD_GILDED
    Player_UpperAction_1,          // PLAYER_IA_SWORD_TWO_HANDED
    Player_UpperAction_0,          // PLAYER_IA_DEKU_STICK
    Player_UpperAction_0,          // PLAYER_IA_ZORA_BOOMERANG
    Player_UpperAction_6,          // PLAYER_IA_BOW
    Player_UpperAction_6,          // PLAYER_IA_BOW_FIRE
    Player_UpperAction_6,          // PLAYER_IA_BOW_ICE
    Player_UpperAction_6,          // PLAYER_IA_BOW_LIGHT
    Player_UpperAction_6,          // PLAYER_IA_HOOKSHOT
    Player_UpperAction_CarryActor, // PLAYER_IA_BOMB
    Player_UpperAction_CarryActor, // PLAYER_IA_POWDER_KEG
    Player_UpperAction_CarryActor, // PLAYER_IA_BOMBCHU
    Player_UpperAction_11,         // PLAYER_IA_11
    Player_UpperAction_6,          // PLAYER_IA_DEKU_NUT
    Player_UpperAction_0,          // PLAYER_IA_PICTOGRAPH_BOX
    Player_UpperAction_0,          // PLAYER_IA_OCARINA
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_EMPTY
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_FISH
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_SPRING_WATER
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_HOT_SPRING_WATER
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_ZORA_EGG
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_DEKU_PRINCESS
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_GOLD_DUST
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_1C
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_SEA_HORSE
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_MUSHROOM
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_HYLIAN_LOACH
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_BUG
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_POE
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_BIG_POE
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_POTION_RED
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_POTION_BLUE
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_POTION_GREEN
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_MILK
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_MILK_HALF
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_CHATEAU
    Player_UpperAction_0,          // PLAYER_IA_BOTTLE_FAIRY
    Player_UpperAction_0,          // PLAYER_IA_MOONS_TEAR
    Player_UpperAction_0,          // PLAYER_IA_DEED_LAND
    Player_UpperAction_0,          // PLAYER_IA_ROOM_KEY
    Player_UpperAction_0,          // PLAYER_IA_LETTER_TO_KAFEI
    Player_UpperAction_0,          // PLAYER_IA_MAGIC_BEANS
    Player_UpperAction_0,          // PLAYER_IA_DEED_SWAMP
    Player_UpperAction_0,          // PLAYER_IA_DEED_MOUNTAIN
    Player_UpperAction_0,          // PLAYER_IA_DEED_OCEAN
    Player_UpperAction_0,          // PLAYER_IA_32
    Player_UpperAction_0,          // PLAYER_IA_LETTER_MAMA
    Player_UpperAction_0,          // PLAYER_IA_34
    Player_UpperAction_0,          // PLAYER_IA_35
    Player_UpperAction_0,          // PLAYER_IA_PENDANT_MEMORIES
    Player_UpperAction_0,          // PLAYER_IA_37
    Player_UpperAction_0,          // PLAYER_IA_38
    Player_UpperAction_0,          // PLAYER_IA_39
    Player_UpperAction_0,          // PLAYER_IA_MASK_TRUTH
    Player_UpperAction_0,          // PLAYER_IA_MASK_KAFEIS_MASK
    Player_UpperAction_0,          // PLAYER_IA_MASK_ALL_NIGHT
    Player_UpperAction_0,          // PLAYER_IA_MASK_BUNNY
    Player_UpperAction_0,          // PLAYER_IA_MASK_KEATON
    Player_UpperAction_0,          // PLAYER_IA_MASK_GARO
    Player_UpperAction_0,          // PLAYER_IA_MASK_ROMANI
    Player_UpperAction_0,          // PLAYER_IA_MASK_CIRCUS_LEADER
    Player_UpperAction_0,          // PLAYER_IA_MASK_POSTMAN
    Player_UpperAction_0,          // PLAYER_IA_MASK_COUPLE
    Player_UpperAction_0,          // PLAYER_IA_MASK_GREAT_FAIRY
    Player_UpperAction_0,          // PLAYER_IA_MASK_GIBDO
    Player_UpperAction_0,          // PLAYER_IA_MASK_DON_GERO
    Player_UpperAction_0,          // PLAYER_IA_MASK_KAMARO
    Player_UpperAction_0,          // PLAYER_IA_MASK_CAPTAIN
    Player_UpperAction_0,          // PLAYER_IA_MASK_STONE
    Player_UpperAction_0,          // PLAYER_IA_MASK_BREMEN
    Player_UpperAction_0,          // PLAYER_IA_MASK_BLAST
    Player_UpperAction_0,          // PLAYER_IA_MASK_SCENTS
    Player_UpperAction_0,          // PLAYER_IA_MASK_GIANT
    Player_UpperAction_0,          // PLAYER_IA_MASK_FIERCE_DEITY
    Player_UpperAction_0,          // PLAYER_IA_MASK_GORON
    Player_UpperAction_0,          // PLAYER_IA_MASK_ZORA
    Player_UpperAction_0,          // PLAYER_IA_MASK_DEKU
    Player_UpperAction_0,          // PLAYER_IA_LENS_OF_TRUTH
};

typedef void (*PlayerItemActionInitFunc)(PlayState*, Player*);

PlayerItemActionInitFunc sItemActionInitFuncs[PLAYER_IA_MAX] = {
    Player_InitDefaultIA,       // PLAYER_IA_NONE
    Player_InitDefaultIA,       // PLAYER_IA_LAST_USED
    Player_InitDefaultIA,       // PLAYER_IA_FISHING_ROD
    Player_InitDefaultIA,       // PLAYER_IA_SWORD_KOKIRI
    Player_InitDefaultIA,       // PLAYER_IA_SWORD_RAZOR
    Player_InitDefaultIA,       // PLAYER_IA_SWORD_GILDED
    Player_InitDefaultIA,       // PLAYER_IA_SWORD_TWO_HANDED
    Player_InitDekuStickIA,     // PLAYER_IA_DEKU_STICK
    Player_InitZoraBoomerangIA, // PLAYER_IA_ZORA_BOOMERANG
    Player_InitBowOrDekuNutIA,  // PLAYER_IA_BOW
    Player_InitBowOrDekuNutIA,  // PLAYER_IA_BOW_FIRE
    Player_InitBowOrDekuNutIA,  // PLAYER_IA_BOW_ICE
    Player_InitBowOrDekuNutIA,  // PLAYER_IA_BOW_LIGHT
    Player_InitHookshotIA,      // PLAYER_IA_HOOKSHOT
    Player_InitExplosiveIA,     // PLAYER_IA_BOMB
    Player_InitExplosiveIA,     // PLAYER_IA_POWDER_KEG
    Player_InitExplosiveIA,     // PLAYER_IA_BOMBCHU
    Player_InitZoraBoomerangIA, // PLAYER_IA_11
    Player_InitBowOrDekuNutIA,  // PLAYER_IA_DEKU_NUT
    Player_InitDefaultIA,       // PLAYER_IA_PICTOGRAPH_BOX
    Player_InitDefaultIA,       // PLAYER_IA_OCARINA
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_EMPTY
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_FISH
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_SPRING_WATER
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_HOT_SPRING_WATER
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_ZORA_EGG
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_DEKU_PRINCESS
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_GOLD_DUST
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_1C
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_SEA_HORSE
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_MUSHROOM
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_HYLIAN_LOACH
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_BUG
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_POE
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_BIG_POE
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_POTION_RED
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_POTION_BLUE
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_POTION_GREEN
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_MILK
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_MILK_HALF
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_CHATEAU
    Player_InitDefaultIA,       // PLAYER_IA_BOTTLE_FAIRY
    Player_InitDefaultIA,       // PLAYER_IA_MOONS_TEAR
    Player_InitDefaultIA,       // PLAYER_IA_DEED_LAND
    Player_InitDefaultIA,       // PLAYER_IA_ROOM_KEY
    Player_InitDefaultIA,       // PLAYER_IA_LETTER_TO_KAFEI
    Player_InitDefaultIA,       // PLAYER_IA_MAGIC_BEANS
    Player_InitDefaultIA,       // PLAYER_IA_DEED_SWAMP
    Player_InitDefaultIA,       // PLAYER_IA_DEED_MOUNTAIN
    Player_InitDefaultIA,       // PLAYER_IA_DEED_OCEAN
    Player_InitDefaultIA,       // PLAYER_IA_32
    Player_InitDefaultIA,       // PLAYER_IA_LETTER_MAMA
    Player_InitDefaultIA,       // PLAYER_IA_34
    Player_InitDefaultIA,       // PLAYER_IA_35
    Player_InitDefaultIA,       // PLAYER_IA_PENDANT_MEMORIES
    Player_InitDefaultIA,       // PLAYER_IA_37
    Player_InitDefaultIA,       // PLAYER_IA_38
    Player_InitDefaultIA,       // PLAYER_IA_39
    Player_InitDefaultIA,       // PLAYER_IA_MASK_TRUTH
    Player_InitDefaultIA,       // PLAYER_IA_MASK_KAFEIS_MASK
    Player_InitDefaultIA,       // PLAYER_IA_MASK_ALL_NIGHT
    Player_InitDefaultIA,       // PLAYER_IA_MASK_BUNNY
    Player_InitDefaultIA,       // PLAYER_IA_MASK_KEATON
    Player_InitDefaultIA,       // PLAYER_IA_MASK_GARO
    Player_InitDefaultIA,       // PLAYER_IA_MASK_ROMANI
    Player_InitDefaultIA,       // PLAYER_IA_MASK_CIRCUS_LEADER
    Player_InitDefaultIA,       // PLAYER_IA_MASK_POSTMAN
    Player_InitDefaultIA,       // PLAYER_IA_MASK_COUPLE
    Player_InitDefaultIA,       // PLAYER_IA_MASK_GREAT_FAIRY
    Player_InitDefaultIA,       // PLAYER_IA_MASK_GIBDO
    Player_InitDefaultIA,       // PLAYER_IA_MASK_DON_GERO
    Player_InitDefaultIA,       // PLAYER_IA_MASK_KAMARO
    Player_InitDefaultIA,       // PLAYER_IA_MASK_CAPTAIN
    Player_InitDefaultIA,       // PLAYER_IA_MASK_STONE
    Player_InitDefaultIA,       // PLAYER_IA_MASK_BREMEN
    Player_InitDefaultIA,       // PLAYER_IA_MASK_BLAST
    Player_InitDefaultIA,       // PLAYER_IA_MASK_SCENTS
    Player_InitDefaultIA,       // PLAYER_IA_MASK_GIANT
    Player_InitDefaultIA,       // PLAYER_IA_MASK_FIERCE_DEITY
    Player_InitDefaultIA,       // PLAYER_IA_MASK_GORON
    Player_InitDefaultIA,       // PLAYER_IA_MASK_ZORA
    Player_InitDefaultIA,       // PLAYER_IA_MASK_DEKU
    Player_InitDefaultIA,       // PLAYER_IA_LENS_OF_TRUTH
};

void Player_InitDefaultIA(PlayState* play, Player* this) {
}

void Player_InitDekuStickIA(PlayState* play, Player* this) {
    this->unk_B28 = 0;
    this->unk_B0C = 1.0f;
}

void Player_InitBowOrDekuNutIA(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_8;

    if (this->heldItemAction == PLAYER_IA_DEKU_NUT) {
        this->unk_B28 = -2;
    } else {
        this->unk_B28 = -1;
    }
    this->unk_ACC = 0;
}

void func_8082F5FC(Player* this, Actor* actor) {
    this->heldActor = actor;
    this->interactRangeActor = actor;
    this->getItemId = GI_NONE;
    this->leftHandWorld.rot.y = actor->shape.rot.y - this->actor.shape.rot.y;
    this->stateFlags1 |= PLAYER_STATE1_CARRYING_ACTOR;
}

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
    /* 14 */ PLAYER_ITEM_CHG_14,
    /* 15 */ PLAYER_ITEM_CHG_MAX
} ItemChangeType;

ItemChangeInfo sPlayerItemChangeInfo[PLAYER_ITEM_CHG_MAX] = {
    { &gPlayerAnim_link_normal_free2free, 12 },     // PLAYER_ITEM_CHG_0
    { &gPlayerAnim_link_normal_normal2fighter, 6 }, // PLAYER_ITEM_CHG_1
    { &gPlayerAnim_link_hammer_normal2long, 8 },    // PLAYER_ITEM_CHG_2
    { &gPlayerAnim_link_normal_normal2free, 8 },    // PLAYER_ITEM_CHG_3
    { &gPlayerAnim_link_fighter_fighter2long, 8 },  // PLAYER_ITEM_CHG_4
    { &gPlayerAnim_link_normal_fighter2free, 10 },  // PLAYER_ITEM_CHG_5
    { &gPlayerAnim_link_hammer_long2free, 7 },      // PLAYER_ITEM_CHG_6
    { &gPlayerAnim_link_hammer_long2long, 11 },     // PLAYER_ITEM_CHG_7
    { &gPlayerAnim_link_normal_free2free, 12 },     // PLAYER_ITEM_CHG_8
    { &gPlayerAnim_link_normal_normal2bom, 4 },     // PLAYER_ITEM_CHG_9
    { &gPlayerAnim_link_normal_long2bom, 4 },       // PLAYER_ITEM_CHG_10
    { &gPlayerAnim_link_normal_free2bom, 4 },       // PLAYER_ITEM_CHG_11
    { &gPlayerAnim_link_anchor_anchor2fighter, 5 }, // PLAYER_ITEM_CHG_12
    { &gPlayerAnim_link_normal_free2freeB, 13 },    // PLAYER_ITEM_CHG_13
    { &gPlayerAnim_pz_bladeon, 4 },                 // PLAYER_ITEM_CHG_14
};

// Maps the appropriate ItemChangeType based on current and next animtype.
// A negative type value means the corresponding animation should be played in reverse.
s8 sPlayerItemChangeTypes[PLAYER_ANIMTYPE_MAX][PLAYER_ANIMTYPE_MAX] = {
    {
        PLAYER_ITEM_CHG_8,  // PLAYER_ANIMTYPE_DEFAULT -> PLAYER_ANIMTYPE_DEFAULT
        -PLAYER_ITEM_CHG_5, // PLAYER_ANIMTYPE_DEFAULT -> PLAYER_ANIMTYPE_1
        -PLAYER_ITEM_CHG_3, // PLAYER_ANIMTYPE_DEFAULT -> PLAYER_ANIMTYPE_2
        -PLAYER_ITEM_CHG_6, // PLAYER_ANIMTYPE_DEFAULT -> PLAYER_ANIMTYPE_3
        PLAYER_ITEM_CHG_8,  // PLAYER_ANIMTYPE_DEFAULT -> PLAYER_ANIMTYPE_4
        PLAYER_ITEM_CHG_11, // PLAYER_ANIMTYPE_DEFAULT -> PLAYER_ANIMTYPE_5
    },
    {
        PLAYER_ITEM_CHG_5,  // PLAYER_ANIMTYPE_1 -> PLAYER_ANIMTYPE_DEFAULT
        PLAYER_ITEM_CHG_0,  // PLAYER_ANIMTYPE_1 -> PLAYER_ANIMTYPE_1
        -PLAYER_ITEM_CHG_1, // PLAYER_ANIMTYPE_1 -> PLAYER_ANIMTYPE_2
        PLAYER_ITEM_CHG_4,  // PLAYER_ANIMTYPE_1 -> PLAYER_ANIMTYPE_3
        PLAYER_ITEM_CHG_5,  // PLAYER_ANIMTYPE_1 -> PLAYER_ANIMTYPE_4
        PLAYER_ITEM_CHG_9,  // PLAYER_ANIMTYPE_1 -> PLAYER_ANIMTYPE_5
    },
    {
        PLAYER_ITEM_CHG_3, // PLAYER_ANIMTYPE_2 -> PLAYER_ANIMTYPE_DEFAULT
        PLAYER_ITEM_CHG_1, // PLAYER_ANIMTYPE_2 -> PLAYER_ANIMTYPE_1
        PLAYER_ITEM_CHG_0, // PLAYER_ANIMTYPE_2 -> PLAYER_ANIMTYPE_2
        PLAYER_ITEM_CHG_2, // PLAYER_ANIMTYPE_2 -> PLAYER_ANIMTYPE_3
        PLAYER_ITEM_CHG_3, // PLAYER_ANIMTYPE_2 -> PLAYER_ANIMTYPE_4
        PLAYER_ITEM_CHG_9, // PLAYER_ANIMTYPE_2 -> PLAYER_ANIMTYPE_5
    },
    {
        PLAYER_ITEM_CHG_6,  // PLAYER_ANIMTYPE_3 -> PLAYER_ANIMTYPE_DEFAULT
        -PLAYER_ITEM_CHG_4, // PLAYER_ANIMTYPE_3 -> PLAYER_ANIMTYPE_1
        -PLAYER_ITEM_CHG_2, // PLAYER_ANIMTYPE_3 -> PLAYER_ANIMTYPE_2
        PLAYER_ITEM_CHG_7,  // PLAYER_ANIMTYPE_3 -> PLAYER_ANIMTYPE_3
        PLAYER_ITEM_CHG_6,  // PLAYER_ANIMTYPE_3 -> PLAYER_ANIMTYPE_4
        PLAYER_ITEM_CHG_10, // PLAYER_ANIMTYPE_3 -> PLAYER_ANIMTYPE_5
    },
    {
        PLAYER_ITEM_CHG_8,  // PLAYER_ANIMTYPE_4 -> PLAYER_ANIMTYPE_DEFAULT
        -PLAYER_ITEM_CHG_5, // PLAYER_ANIMTYPE_4 -> PLAYER_ANIMTYPE_1
        -PLAYER_ITEM_CHG_3, // PLAYER_ANIMTYPE_4 -> PLAYER_ANIMTYPE_2
        -PLAYER_ITEM_CHG_6, // PLAYER_ANIMTYPE_4 -> PLAYER_ANIMTYPE_3
        PLAYER_ITEM_CHG_8,  // PLAYER_ANIMTYPE_4 -> PLAYER_ANIMTYPE_4
        PLAYER_ITEM_CHG_11, // PLAYER_ANIMTYPE_4 -> PLAYER_ANIMTYPE_5
    },
    {
        PLAYER_ITEM_CHG_8,  // PLAYER_ANIMTYPE_5 -> PLAYER_ANIMTYPE_DEFAULT
        -PLAYER_ITEM_CHG_5, // PLAYER_ANIMTYPE_5 -> PLAYER_ANIMTYPE_1
        -PLAYER_ITEM_CHG_3, // PLAYER_ANIMTYPE_5 -> PLAYER_ANIMTYPE_2
        -PLAYER_ITEM_CHG_6, // PLAYER_ANIMTYPE_5 -> PLAYER_ANIMTYPE_3
        PLAYER_ITEM_CHG_8,  // PLAYER_ANIMTYPE_5 -> PLAYER_ANIMTYPE_4
        PLAYER_ITEM_CHG_11, // PLAYER_ANIMTYPE_5 -> PLAYER_ANIMTYPE_5
    },
};

ExplosiveInfo sPlayerExplosiveInfo[PLAYER_EXPLOSIVE_MAX] = {
    { ITEM_BOMB, ACTOR_EN_BOM },        // PLAYER_EXPLOSIVE_BOMB
    { ITEM_POWDER_KEG, ACTOR_EN_BOM },  // PLAYER_EXPLOSIVE_POWDER_KEG
    { ITEM_BOMBCHU, ACTOR_EN_BOM_CHU }, // PLAYER_EXPLOSIVE_BOMBCHU
};

void Player_InitExplosiveIA(PlayState* play, Player* this) {
    PlayerExplosive explosiveType;
    ExplosiveInfo* explosiveInfo;
    Actor* explosiveActor;

    if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        Player_PutAwayHeldItem(play, this);
        return;
    }

    explosiveType = Player_GetExplosiveHeld(this);
    explosiveInfo = &sPlayerExplosiveInfo[explosiveType];
    if ((explosiveType == PLAYER_EXPLOSIVE_POWDER_KEG) && (gSaveContext.powderKegTimer == 0)) {
        gSaveContext.powderKegTimer = 200;
    }

    explosiveActor = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, explosiveInfo->actorId,
                                        this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                        (explosiveType == PLAYER_EXPLOSIVE_POWDER_KEG) ? BOMB_EXPLOSIVE_TYPE_POWDER_KEG
                                                                                       : BOMB_EXPLOSIVE_TYPE_BOMB,
                                        this->actor.shape.rot.y, 0, BOMB_TYPE_BODY);
    if (explosiveActor != NULL) {
        if ((explosiveType == PLAYER_EXPLOSIVE_BOMB) && (play->unk_1887E != 0)) {
            play->unk_1887E--;
            if (play->unk_1887E == 0) {
                play->unk_1887E = -1;
            }
        } else if ((explosiveType == PLAYER_EXPLOSIVE_BOMBCHU) && (play->unk_1887D != 0)) {
            play->unk_1887D--;
            if (play->unk_1887D == 0) {
                play->unk_1887D = -1;
            }
        } else {
            Inventory_ChangeAmmo(explosiveInfo->itemId, -1);
        }
        func_8082F5FC(this, explosiveActor);
    } else if (explosiveType == PLAYER_EXPLOSIVE_POWDER_KEG) {
        gSaveContext.powderKegTimer = 0;
    }
}

void Player_InitHookshotIA(PlayState* play, Player* this) {
    ArmsHook* armsHook;

    this->stateFlags1 |= PLAYER_STATE1_8;
    this->unk_B28 = -3;
    this->unk_B48 = 0.0f;

    this->heldActor =
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_ARMS_HOOK, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0);

    if (this->heldActor == NULL) {
        Player_UseItem(play, this, ITEM_NONE);
        return;
    }
    armsHook = (ArmsHook*)this->heldActor;
    armsHook->actor.objectSlot = this->actor.objectSlot;
    armsHook->unk_208 = this->transformation;
}

void Player_InitZoraBoomerangIA(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_USING_ZORA_BOOMERANG;
}

void Player_InitItemAction(PlayState* play, Player* this, PlayerItemAction itemAction) {
    this->itemAction = this->heldItemAction = itemAction;
    this->modelGroup = this->nextModelGroup;

    this->stateFlags1 &= ~(PLAYER_STATE1_USING_ZORA_BOOMERANG | PLAYER_STATE1_8);

    this->unk_B08 = 0.0f;
    this->unk_B0C = 0.0f;
    this->unk_B28 = 0;

    sItemActionInitFuncs[itemAction](play, this);
    Player_SetModelGroup(this, this->modelGroup);
}

// AttackAnimInfo sMeleeAttackAnimInfo
AttackAnimInfo sMeleeAttackAnimInfo[PLAYER_MWA_MAX] = {
    // PLAYER_MWA_FORWARD_SLASH_1H
    { &gPlayerAnim_link_fighter_normal_kiru, &gPlayerAnim_link_fighter_normal_kiru_end,
      &gPlayerAnim_link_fighter_normal_kiru_endR, 1, 4 },
    // PLAYER_MWA_FORWARD_SLASH_2H
    { &gPlayerAnim_link_fighter_Lnormal_kiru, &gPlayerAnim_link_fighter_Lnormal_kiru_end,
      &gPlayerAnim_link_anchor_Lnormal_kiru_endR, 1, 4 },
    // PLAYER_MWA_FORWARD_COMBO_1H
    { &gPlayerAnim_link_fighter_normal_kiru_finsh, &gPlayerAnim_link_fighter_normal_kiru_finsh_end,
      &gPlayerAnim_link_anchor_normal_kiru_finsh_endR, 0, 5 },
    // PLAYER_MWA_FORWARD_COMBO_2H
    { &gPlayerAnim_link_fighter_Lnormal_kiru_finsh, &gPlayerAnim_link_fighter_Lnormal_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lnormal_kiru_finsh_endR, 1, 7 },
    // PLAYER_MWA_RIGHT_SLASH_1H
    { &gPlayerAnim_link_fighter_Lside_kiru, &gPlayerAnim_link_fighter_Lside_kiru_end,
      &gPlayerAnim_link_anchor_Lside_kiru_endR, 1, 4 },
    // PLAYER_MWA_RIGHT_SLASH_2H
    { &gPlayerAnim_link_fighter_LLside_kiru, &gPlayerAnim_link_fighter_LLside_kiru_end,
      &gPlayerAnim_link_anchor_LLside_kiru_endL, 0, 5 },
    // PLAYER_MWA_RIGHT_COMBO_1H
    { &gPlayerAnim_link_fighter_Lside_kiru_finsh, &gPlayerAnim_link_fighter_Lside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lside_kiru_finsh_endR, 2, 8 },
    // PLAYER_MWA_RIGHT_COMBO_2H
    { &gPlayerAnim_link_fighter_LLside_kiru_finsh, &gPlayerAnim_link_fighter_LLside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_LLside_kiru_finsh_endR, 3, 8 },
    // PLAYER_MWA_LEFT_SLASH_1H
    { &gPlayerAnim_link_fighter_Rside_kiru, &gPlayerAnim_link_fighter_Rside_kiru_end,
      &gPlayerAnim_link_anchor_Rside_kiru_endR, 0, 4 },
    // PLAYER_MWA_LEFT_SLASH_2H
    { &gPlayerAnim_link_fighter_LRside_kiru, &gPlayerAnim_link_fighter_LRside_kiru_end,
      &gPlayerAnim_link_anchor_LRside_kiru_endR, 0, 5 },
    // PLAYER_MWA_LEFT_COMBO_1H
    { &gPlayerAnim_link_fighter_Rside_kiru_finsh, &gPlayerAnim_link_fighter_Rside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Rside_kiru_finsh_endR, 0, 6 },
    // PLAYER_MWA_LEFT_COMBO_2H
    { &gPlayerAnim_link_fighter_LRside_kiru_finsh, &gPlayerAnim_link_fighter_LRside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_LRside_kiru_finsh_endL, 1, 5 },
    // PLAYER_MWA_STAB_1H
    { &gPlayerAnim_link_fighter_pierce_kiru, &gPlayerAnim_link_fighter_pierce_kiru_end,
      &gPlayerAnim_link_anchor_pierce_kiru_endR, 0, 3 },
    // PLAYER_MWA_STAB_2H
    { &gPlayerAnim_link_fighter_Lpierce_kiru, &gPlayerAnim_link_fighter_Lpierce_kiru_end,
      &gPlayerAnim_link_anchor_Lpierce_kiru_endL, 0, 3 },
    // PLAYER_MWA_STAB_COMBO_1H
    { &gPlayerAnim_link_fighter_pierce_kiru_finsh, &gPlayerAnim_link_fighter_pierce_kiru_finsh_end,
      &gPlayerAnim_link_anchor_pierce_kiru_finsh_endR, 1, 9 },
    // PLAYER_MWA_STAB_COMBO_2H
    { &gPlayerAnim_link_fighter_Lpierce_kiru_finsh, &gPlayerAnim_link_fighter_Lpierce_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lpierce_kiru_finsh_endR, 1, 8 },
    // PLAYER_MWA_FLIPSLASH_START
    { &gPlayerAnim_link_fighter_jump_rollkiru, &gPlayerAnim_link_fighter_jump_kiru_finsh,
      &gPlayerAnim_link_fighter_jump_kiru_finsh, 7, 99 },
    // PLAYER_MWA_JUMPSLASH_START
    { &gPlayerAnim_link_fighter_Lpower_jump_kiru, &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit,
      &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit, 7, 99 },
    // PLAYER_MWA_ZORA_JUMPKICK_START
    { &gPlayerAnim_pz_jumpAT, &gPlayerAnim_pz_jumpATend, &gPlayerAnim_pz_jumpATend, 8, 99 },
    // PLAYER_MWA_FLIPSLASH_FINISH
    { &gPlayerAnim_link_fighter_jump_kiru_finsh, &gPlayerAnim_link_fighter_jump_kiru_finsh_end,
      &gPlayerAnim_link_fighter_jump_kiru_finsh_end, 1, 2 },
    // PLAYER_MWA_JUMPSLASH_FINISH
    { &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit, &gPlayerAnim_link_fighter_Lpower_jump_kiru_end,
      &gPlayerAnim_link_fighter_Lpower_jump_kiru_end, 1, 2 },
    // PLAYER_MWA_ZORA_JUMPKICK_FINISH
    { &gPlayerAnim_pz_jumpATend, &gPlayerAnim_pz_wait, &gPlayerAnim_link_normal_waitR_free, 1, 2 },
    // PLAYER_MWA_BACKSLASH_RIGHT
    { &gPlayerAnim_link_fighter_turn_kiruR, &gPlayerAnim_link_fighter_turn_kiruR_end,
      &gPlayerAnim_link_fighter_turn_kiruR_end, 1, 5 },
    // PLAYER_MWA_BACKSLASH_LEFT
    { &gPlayerAnim_link_fighter_turn_kiruL, &gPlayerAnim_link_fighter_turn_kiruL_end,
      &gPlayerAnim_link_fighter_turn_kiruL_end, 1, 4 },
    // PLAYER_MWA_GORON_PUNCH_LEFT
    { &gPlayerAnim_pg_punchA, &gPlayerAnim_pg_punchAend, &gPlayerAnim_pg_punchAendR, 6, 8 },
    // PLAYER_MWA_GORON_PUNCH_RIGHT
    { &gPlayerAnim_pg_punchB, &gPlayerAnim_pg_punchBend, &gPlayerAnim_pg_punchBendR, 12, 18 },
    // PLAYER_MWA_GORON_PUNCH_BUTT
    { &gPlayerAnim_pg_punchC, &gPlayerAnim_pg_punchCend, &gPlayerAnim_pg_punchCendR, 8, 14 },
    // PLAYER_MWA_ZORA_PUNCH_LEFT
    { &gPlayerAnim_pz_attackA, &gPlayerAnim_pz_attackAend, &gPlayerAnim_pz_attackAendR, 2, 5 },
    // PLAYER_MWA_ZORA_PUNCH_COMBO
    { &gPlayerAnim_pz_attackB, &gPlayerAnim_pz_attackBend, &gPlayerAnim_pz_attackBendR, 3, 8 },
    // PLAYER_MWA_ZORA_PUNCH_KICK
    { &gPlayerAnim_pz_attackC, &gPlayerAnim_pz_attackCend, &gPlayerAnim_pz_attackCendR, 3, 10 },
    // PLAYER_MWA_SPIN_ATTACK_1H
    { &gPlayerAnim_link_fighter_rolling_kiru, &gPlayerAnim_link_fighter_rolling_kiru_end,
      &gPlayerAnim_link_anchor_rolling_kiru_endR, 0, 12 },
    // PLAYER_MWA_SPIN_ATTACK_2H
    { &gPlayerAnim_link_fighter_Lrolling_kiru, &gPlayerAnim_link_fighter_Lrolling_kiru_end,
      &gPlayerAnim_link_anchor_Lrolling_kiru_endR, 0, 15 },
    // PLAYER_MWA_BIG_SPIN_1H
    { &gPlayerAnim_link_fighter_Wrolling_kiru, &gPlayerAnim_link_fighter_Wrolling_kiru_end,
      &gPlayerAnim_link_anchor_rolling_kiru_endR, 0, 16 },
    // PLAYER_MWA_BIG_SPIN_2H
    { &gPlayerAnim_link_fighter_Wrolling_kiru, &gPlayerAnim_link_fighter_Wrolling_kiru_end,
      &gPlayerAnim_link_anchor_Lrolling_kiru_endR, 0, 16 },
};

PlayerAnimationHeader* D_8085CF50[] = {
    &gPlayerAnim_link_fighter_power_kiru_start,
    &gPlayerAnim_link_fighter_Lpower_kiru_start,
};
PlayerAnimationHeader* D_8085CF58[] = {
    &gPlayerAnim_link_fighter_power_kiru_startL,
    &gPlayerAnim_link_fighter_Lpower_kiru_start,
};
PlayerAnimationHeader* D_8085CF60[] = {
    &gPlayerAnim_link_fighter_power_kiru_wait,
    &gPlayerAnim_link_fighter_Lpower_kiru_wait,
};
PlayerAnimationHeader* D_8085CF68[] = {
    &gPlayerAnim_link_fighter_power_kiru_wait_end,
    &gPlayerAnim_link_fighter_Lpower_kiru_wait_end,
};
PlayerAnimationHeader* D_8085CF70[] = {
    &gPlayerAnim_link_fighter_power_kiru_walk,
    &gPlayerAnim_link_fighter_Lpower_kiru_walk,
};
PlayerAnimationHeader* D_8085CF78[] = {
    &gPlayerAnim_link_fighter_power_kiru_side_walk,
    &gPlayerAnim_link_fighter_Lpower_kiru_side_walk,
};

u8 D_8085CF80[] = {
    PLAYER_MWA_SPIN_ATTACK_1H,
    PLAYER_MWA_SPIN_ATTACK_2H,
};
u8 D_8085CF84[] = {
    PLAYER_MWA_BIG_SPIN_1H,
    PLAYER_MWA_BIG_SPIN_2H,
};

// sBlureColors
BlureColors D_8085CF88[] = {
    { { 255, 255, 255, 255 }, { 255, 255, 255, 64 }, { 255, 255, 255, 0 }, { 255, 255, 255, 0 } },
    { { 165, 185, 255, 185 }, { 205, 225, 255, 50 }, { 255, 255, 255, 0 }, { 255, 255, 255, 0 } },
};

void Player_OverrideBlureColors(PlayState* play, Player* this, s32 colorType, s32 elemDuration) {
    EffectBlure* blure0 = Effect_GetByIndex(this->meleeWeaponEffectIndex[0]);
    EffectBlure* blure1 = Effect_GetByIndex(this->meleeWeaponEffectIndex[1]);
    s32 i;

    for (i = 0; i < 4; i++) {
        blure0->p1StartColor[i] = D_8085CF88[colorType].p1StartColor[i];
        blure0->p2StartColor[i] = D_8085CF88[colorType].p2StartColor[i];
        blure0->p1EndColor[i] = D_8085CF88[colorType].p1EndColor[i];
        blure0->p2EndColor[i] = D_8085CF88[colorType].p2EndColor[i];
        blure1->p1StartColor[i] = D_8085CF88[colorType].p1StartColor[i];
        blure1->p2StartColor[i] = D_8085CF88[colorType].p2StartColor[i];
        blure1->p1EndColor[i] = D_8085CF88[colorType].p1EndColor[i];
        blure1->p2EndColor[i] = D_8085CF88[colorType].p2EndColor[i];
    }

    if (this->transformation == PLAYER_FORM_DEKU) {
        elemDuration = 8;
    }
    blure0->elemDuration = elemDuration;
    blure1->elemDuration = elemDuration;
}

void func_8082FA5C(PlayState* play, Player* this, PlayerMeleeWeaponState meleeWeaponState) {
    u16 voiceSfxId;
    u16 itemSfxId;

    if (this->meleeWeaponState == PLAYER_MELEE_WEAPON_STATE_0) {
        voiceSfxId = NA_SE_VO_LI_SWORD_N;
        if (this->transformation == PLAYER_FORM_GORON) {
            itemSfxId = NA_SE_IT_GORON_PUNCH_SWING;
        } else {
            itemSfxId = NA_SE_NONE;
            if (this->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) {
                voiceSfxId = NA_SE_VO_LI_SWORD_L;
            } else if (this->meleeWeaponAnimation == PLAYER_MWA_ZORA_PUNCH_KICK) {
                itemSfxId = NA_SE_IT_GORON_PUNCH_SWING;
            } else {
                itemSfxId = NA_SE_IT_SWORD_SWING_HARD;
                if (this->unk_ADD >= 3) {
                    voiceSfxId = NA_SE_VO_LI_SWORD_L;
                } else {
                    itemSfxId = (this->heldItemAction == PLAYER_IA_SWORD_TWO_HANDED) ? NA_SE_IT_HAMMER_SWING
                                                                                     : NA_SE_IT_SWORD_SWING;
                }
            }
        }

        if (itemSfxId != NA_SE_NONE) {
            func_8082E1F0(this, itemSfxId);
        }

        if (!((this->meleeWeaponAnimation >= PLAYER_MWA_FLIPSLASH_START) &&
              (this->meleeWeaponAnimation <= PLAYER_MWA_ZORA_JUMPKICK_FINISH))) {
            Player_AnimSfx_PlayVoice(this, voiceSfxId);
        }

        Player_OverrideBlureColors(play, this, 0, 4);
    }

    this->meleeWeaponState = meleeWeaponState;
}

/**
 * Checks the current state of `focusActor` and if it is a hostile actor (if applicable).
 * If so, sets `PLAYER_STATE3_HOSTILE_LOCK_ON` which will control Player's "battle" response to
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
        CHECK_FLAG_ALL(this->focusActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)) {
        this->stateFlags3 |= PLAYER_STATE3_HOSTILE_LOCK_ON;
        return true;
    }

    if (this->stateFlags3 & PLAYER_STATE3_HOSTILE_LOCK_ON) {
        this->stateFlags3 &= ~PLAYER_STATE3_HOSTILE_LOCK_ON;

        // sync world and shape yaw when not moving
        if (this->speedXZ == 0.0f) {
            this->yaw = this->actor.shape.rot.y;
        }
    }

    return false;
}

/**
 * Returns true if currently Z-Targeting, false if not.
 * Z-Targeting here is a blanket term that covers both the "actor lock-on" and "parallel" states.
 *
 * This variant of the function calls `Player_CheckHostileLockOn`, which does not update the hostile
 * lock-on actor state.
 */
bool Player_IsZTargeting(Player* this) {
    return Player_CheckHostileLockOn(this) || Player_FriendlyLockOnOrParallel(this);
}

/**
 * Returns true if currently Z-Targeting, false if not.
 * Z-Targeting here is a blanket term that covers both the "actor lock-on" and "parallel" states.
 *
 * This variant of the function calls `Player_UpdateHostileLockOn`, which updates the hostile
 * lock-on actor state before checking its state.
 */
bool Player_IsZTargetingWithHostileUpdate(Player* this) {
    return Player_UpdateHostileLockOn(this) || Player_FriendlyLockOnOrParallel(this);
}

void func_8082FC60(Player* this) {
    this->unk_B44 = 0.0f;
    this->unk_B40 = 0.0f;
}

bool Player_ItemIsInUse(Player* this, ItemId item) {
    if ((item < ITEM_FD) && (Player_ItemToItemAction(this, item) == this->itemAction)) {
        return true;
    } else {
        return false;
    }
}

bool Player_ItemIsItemAction(Player* this, ItemId item, PlayerItemAction itemAction) {
    if ((item < ITEM_FD) && (Player_ItemToItemAction(this, item) == itemAction)) {
        return true;
    } else {
        return false;
    }
}

// #region 2S2H [Dpad]
DpadEquipSlot func_Dpad_8082FD0C(Player* this, PlayerItemAction itemAction) {
    s32 btn;

    for (btn = EQUIP_SLOT_D_RIGHT; btn <= EQUIP_SLOT_D_UP; btn++) {
        if (Player_ItemIsItemAction(this, DPAD_GET_CUR_FORM_BTN_ITEM(btn), itemAction)) {
            return btn;
        }
    }

    return EQUIP_SLOT_D_NONE;
}

u16 sDpadItemButtons[] = {
    BTN_DRIGHT,
    BTN_DLEFT,
    BTN_DDOWN,
    BTN_DUP,
};

// Return currently-pressed button, in order of priority DRIGHT, DLEFT, DDOWN, DUP.
DpadEquipSlot func_Dpad_8082FDC4(void) {
    DpadEquipSlot i;

    for (i = 0; i < ARRAY_COUNT(sDpadItemButtons); i++) {
        if (CHECK_BTN_ALL(sPlayerControlInput->press.button, sDpadItemButtons[i])) {
            break;
        }
    }

    return i;
}
// #endregion

EquipSlot func_8082FD0C(Player* this, PlayerItemAction itemAction) {
    s32 btn;

    for (btn = EQUIP_SLOT_C_LEFT; btn <= EQUIP_SLOT_C_RIGHT; btn++) {
        if (Player_ItemIsItemAction(this, GET_CUR_FORM_BTN_ITEM(btn), itemAction)) {
            return btn;
        }
    }

    return EQUIP_SLOT_NONE;
}

u16 sPlayerItemButtons[] = {
    BTN_B,
    BTN_CLEFT,
    BTN_CDOWN,
    BTN_CRIGHT,
};

// Return currently-pressed button, in order of priority B, CLEFT, CDOWN, CRIGHT.
EquipSlot func_8082FDC4(void) {
    EquipSlot i;

    for (i = 0; i < ARRAY_COUNT(sPlayerItemButtons); i++) {
        if (CHECK_BTN_ALL(sPlayerControlInput->press.button, sPlayerItemButtons[i])) {
            break;
        }
    }

    return i;
}

/**
 * Handles the high level item usage and changing process based on the B and C buttons.
 */
void Player_ProcessItemButtons(Player* this, PlayState* play) {
    if (this->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_20000000)) {
        return;
    }
    if (this->stateFlags2 & PLAYER_STATE2_2000000) {
        return;
    }
    if (this->stateFlags3 & PLAYER_STATE3_20000000) {
        return;
    }
    if (func_801240DC(this)) {
        return;
    }

    if (this->transformation == PLAYER_FORM_HUMAN) {
        if (this->currentMask != PLAYER_MASK_NONE) {
            PlayerItemAction maskItemAction = GET_IA_FROM_MASK(this->currentMask);
            // #region 2S2H [Dpad] - Changed from EquipSlot to s32 to allow for higher ranges
            s32 btn = func_8082FD0C(this, maskItemAction);

            if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                if (btn <= EQUIP_SLOT_NONE) {
                    DpadEquipSlot dpadBtn = func_Dpad_8082FD0C(this, maskItemAction);

                    if (dpadBtn > EQUIP_SLOT_D_NONE) {
                        btn = DPAD_TO_HELD_ITEM(dpadBtn);
                    }
                }
            }
            // #endregion

            if (GameInteractor_Should(VB_UNEQUIP_MASK_NOT_ON_BUTTON, btn <= EQUIP_SLOT_NONE, this, &btn)) {
                // #region 2S2H [Dpad] - need to convert between helditem value to actual item
                ItemId maskItem;
                if (IS_HELD_DPAD(this->unk_154)) {
                    maskItem = DPAD_GET_CUR_FORM_BTN_ITEM(HELD_ITEM_TO_DPAD(this->unk_154));
                } else {
                    maskItem = GET_CUR_FORM_BTN_ITEM(this->unk_154);
                }

                s32 maskIdMinusOne = GET_MASK_FROM_IA(Player_ItemToItemAction(this, maskItem)) - 1;
                // #endregion

                if ((maskIdMinusOne < PLAYER_MASK_TRUTH - 1) || (maskIdMinusOne >= PLAYER_MASK_MAX - 1)) {
                    maskIdMinusOne = this->currentMask - 1;
                }
                Player_UseItem(play, this, Player_MaskIdToItemId(maskIdMinusOne));
                return;
            }

            if ((this->currentMask == PLAYER_MASK_GIANT) && (gSaveContext.save.saveInfo.playerData.magic == 0)) {
                func_80838A20(play, this);
            }

            this->unk_154 = btn;
        }
    }

    if (((this->actor.id == ACTOR_PLAYER) && (this->itemAction >= PLAYER_IA_FISHING_ROD)) &&
        !(((Player_GetHeldBButtonSword(this) == PLAYER_B_SWORD_NONE) || (gSaveContext.jinxTimer == 0)) &&
          (Player_ItemIsInUse(this, (IREG(1) != 0) ? ITEM_FISHING_ROD : Inventory_GetBtnBItem(play)) ||
           // #region 2S2H [Dpad]
           (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0) &&
            (Player_ItemIsInUse(this, DPAD_BTN_ITEM(EQUIP_SLOT_D_RIGHT)) ||
             Player_ItemIsInUse(this, DPAD_BTN_ITEM(EQUIP_SLOT_D_LEFT)) ||
             Player_ItemIsInUse(this, DPAD_BTN_ITEM(EQUIP_SLOT_D_DOWN)) ||
             Player_ItemIsInUse(this, DPAD_BTN_ITEM(EQUIP_SLOT_D_UP)))) ||
           // #end region
           Player_ItemIsInUse(this, C_BTN_ITEM(EQUIP_SLOT_C_LEFT)) ||
           Player_ItemIsInUse(this, C_BTN_ITEM(EQUIP_SLOT_C_DOWN)) ||
           Player_ItemIsInUse(this, C_BTN_ITEM(EQUIP_SLOT_C_RIGHT))))) {
        Player_UseItem(play, this, ITEM_NONE);
    } else {
        s32 pad;
        ItemId item;
        EquipSlot i = func_8082FDC4();

        i = GameInteractor_Should(VB_FD_ALWAYS_WIELD_SWORD, (i >= EQUIP_SLOT_A) &&
                                                                (this->transformation == PLAYER_FORM_FIERCE_DEITY) &&
                                                                (this->heldItemAction != PLAYER_IA_SWORD_TWO_HANDED))
                ? EQUIP_SLOT_B
                : i;

        item = Player_GetItemOnButton(play, this, i);

        // #region 2S2H [Dpad]
        if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
            if (i >= EQUIP_SLOT_A) {
                DpadEquipSlot j = func_Dpad_8082FDC4();
                ItemId dpadItem = Player_Dpad_GetItemOnButton(play, this, j);
                if (dpadItem < item) {
                    item = dpadItem;
                }
                i = (j >= EQUIP_SLOT_D_MAX) ? i : DPAD_TO_HELD_ITEM(j);
            }
        }
        // #endregion

        if (item >= ITEM_FD) {
            for (i = 0; i < ARRAY_COUNT(sPlayerItemButtons); i++) {
                if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, sPlayerItemButtons[i])) {
                    break;
                }
            }

            item = Player_GetItemOnButton(play, this, i);
            if ((item < ITEM_FD) && (Player_ItemToItemAction(this, item) == this->heldItemAction)) {
                sPlayerHeldItemButtonIsHeldDown = true;
            }
            // #region 2S2H [Dpad]
            else if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                for (i = 0; i < ARRAY_COUNT(sDpadItemButtons); i++) {
                    if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, sDpadItemButtons[i])) {
                        break;
                    }
                }

                item = Player_Dpad_GetItemOnButton(play, this, i);
                if ((item < ITEM_FD) && (Player_ItemToItemAction(this, item) == this->heldItemAction)) {
                    sPlayerHeldItemButtonIsHeldDown = true;
                }
            }
            // #endregion
        } else if (item == ITEM_F0) {
            if (this->blastMaskTimer == 0) {
                EnBom* bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.focus.pos.x,
                                                  this->actor.focus.pos.y, this->actor.focus.pos.z,
                                                  BOMB_EXPLOSIVE_TYPE_BOMB, 0, 0, BOMB_TYPE_BODY);

                if (bomb != NULL) {
                    bomb->timer = 0;
                    if (GameInteractor_Should(VB_SET_BLAST_MASK_COOLDOWN_TIMER, true)) {
                        this->blastMaskTimer = 310;
                    }
                }
            }
        } else if (item == ITEM_F1) {
            func_80839978(play, this);
        } else if (item == ITEM_F2) {
            func_80839A10(play, this);
        } else if ((Player_BButtonSwordFromIA(this, Player_ItemToItemAction(this, item)) != PLAYER_B_SWORD_NONE) &&
                   (gSaveContext.jinxTimer != 0)) {
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                Message_StartTextbox(play, 0xF7, NULL);
            }
        } else {
            this->heldItemButton = i;
            Player_UseItem(play, this, item);
        }
    }
}

void Player_StartChangingHeldItem(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;
    s32 pad[3];
    u8 nextModelAnimType;
    s32 itemChangeType;
    s8 heldItemAction = Player_ItemToItemAction(this, this->heldItemId);
    s32 pad3;
    f32 startFrame;
    f32 endFrame;
    f32 frameSpeed;

    Player_SetUpperAction(play, this, Player_UpperAction_ChangeHeldItem);

    nextModelAnimType = gPlayerModelTypes[this->nextModelGroup].modelAnimType;
    itemChangeType = sPlayerItemChangeTypes[gPlayerModelTypes[this->modelGroup].modelAnimType][nextModelAnimType];

    if ((heldItemAction == PLAYER_IA_ZORA_BOOMERANG) || (this->heldItemAction == PLAYER_IA_ZORA_BOOMERANG)) {
        itemChangeType = (heldItemAction == PLAYER_IA_NONE) ? -PLAYER_ITEM_CHG_14 : PLAYER_ITEM_CHG_14;
    } else if ((heldItemAction == PLAYER_IA_BOTTLE_EMPTY) || (heldItemAction == PLAYER_IA_11) ||
               ((heldItemAction == PLAYER_IA_NONE) &&
                ((this->heldItemAction == PLAYER_IA_BOTTLE_EMPTY) || (this->heldItemAction == PLAYER_IA_11)))) {
        itemChangeType = (heldItemAction == PLAYER_IA_NONE) ? -PLAYER_ITEM_CHG_13 : PLAYER_ITEM_CHG_13;
    }

    this->itemChangeType = ABS_ALT(itemChangeType);
    anim = sPlayerItemChangeInfo[this->itemChangeType].anim;

    if ((anim == &gPlayerAnim_link_normal_fighter2free) && (this->currentShield == PLAYER_SHIELD_NONE)) {
        anim = &gPlayerAnim_link_normal_free2fighter_free;
    }

    endFrame = Animation_GetLastFrame(anim);

    if (itemChangeType >= 0) {
        frameSpeed = 1.2f;
        startFrame = 0.0f;
    } else {
        frameSpeed = -1.2f;
        startFrame = endFrame;
        endFrame = 0.0f;
    }

    if (heldItemAction != PLAYER_IA_NONE) {
        frameSpeed *= 2.0f;
    }

    PlayerAnimation_Change(play, &this->skelAnimeUpper, anim, frameSpeed, startFrame, endFrame, ANIMMODE_ONCE, 0.0f);

    this->stateFlags3 &= ~PLAYER_STATE3_START_CHANGING_HELD_ITEM;
}

void Player_UpdateItems(Player* this, PlayState* play) {
    if ((this->actor.id == ACTOR_PLAYER) && !(this->stateFlags3 & PLAYER_STATE3_START_CHANGING_HELD_ITEM)) {
        if ((this->heldItemAction == this->itemAction) || (this->stateFlags1 & PLAYER_STATE1_400000)) {
            if ((gSaveContext.save.saveInfo.playerData.health != 0) && (play->csCtx.state == CS_STATE_IDLE)) {
                if ((this->csAction == PLAYER_CSACTION_NONE) && (play->bButtonAmmoPlusOne == 0) &&
                    (play->activeCamId == CAM_ID_MAIN)) {
                    if (!func_8082DA90(play) && (gSaveContext.timerStates[TIMER_ID_MINIGAME_2] != TIMER_STATE_STOP)) {
                        Player_ProcessItemButtons(this, play);
                    }
                }
            }
        }
    }

    if (this->stateFlags3 & PLAYER_STATE3_START_CHANGING_HELD_ITEM) {
        Player_StartChangingHeldItem(this, play);
    }
}

// EN_ARROW ammo related?
s32 func_808305BC(PlayState* play, Player* this, ItemId* item, ArrowType* typeParam) {
    if (this->heldItemAction == PLAYER_IA_DEKU_NUT) {
        *item = ITEM_DEKU_NUT;
        *typeParam = (this->transformation == PLAYER_FORM_DEKU) ? ARROW_TYPE_DEKU_BUBBLE : ARROW_TYPE_SLINGSHOT;
    } else {
        *item = ITEM_BOW;
        *typeParam = (this->stateFlags1 & PLAYER_STATE1_800000)
                         ? ARROW_TYPE_NORMAL_HORSE
                         : (this->heldItemAction - PLAYER_IA_BOW + ARROW_TYPE_NORMAL);
    }

    if (this->transformation == PLAYER_FORM_DEKU) {
        return ((gSaveContext.save.saveInfo.playerData.magic >= 2) ||
                (CHECK_WEEKEVENTREG(WEEKEVENTREG_08_01) && (play->sceneId == SCENE_BOWLING)))
                   ? 1
                   : 0;
    }
    if (this->stateFlags3 & PLAYER_STATE3_400) {
        return 1;
    }
    if (gSaveContext.minigameStatus == MINIGAME_STATUS_ACTIVE) {
        return play->interfaceCtx.minigameAmmo;
    }
    if (play->bButtonAmmoPlusOne != 0) {
        return play->bButtonAmmoPlusOne;
    }

    return AMMO(*item);
}

u16 D_8085CFB0[] = {
    NA_SE_PL_BOW_DRAW,
    NA_SE_NONE,
    NA_SE_IT_HOOKSHOT_READY,
};

u8 sMagicArrowCosts[] = {
    4, // ARROW_MAGIC_FIRE
    4, // ARROW_MAGIC_ICE
    8, // ARROW_MAGIC_LIGHT
    2, // ARROW_MAGIC_DEKU_BUBBLE
};

// Draw bow or hookshot / first person items?
s32 func_808306F8(Player* this, PlayState* play) {
    if ((this->heldItemAction >= PLAYER_IA_BOW_FIRE) && (this->heldItemAction <= PLAYER_IA_BOW_LIGHT) &&
        (gSaveContext.magicState != MAGIC_STATE_IDLE)) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
    } else {
        Player_SetUpperAction(play, this, Player_UpperAction_7);

        this->stateFlags3 |= PLAYER_STATE3_40;
        this->unk_ACC = 14;

        if (this->unk_B28 >= 0) {
            s32 var_v1 = ABS_ALT(this->unk_B28);
            ItemId item;
            ArrowType arrowType;
            ArrowMagic magicArrowType;

            if (var_v1 != 2) {
                // 2S2H [Port] When using action swap, D_8085CFB0 is indexed with -1 leading
                // to UB sent into Player_PlaySfx. On console this resolves as 1 and nothing noticable happens.
                // For the port, sometimes this UB would crash so we are opting to just request NA_SE_NONE instead.
                if (var_v1 - 1 < 0) {
                    Player_PlaySfx(this, NA_SE_NONE);
                } else {
                    Player_PlaySfx(this, D_8085CFB0[var_v1 - 1]);
                }
            }

            if (!Player_IsHoldingHookshot(this) && (func_808305BC(play, this, &item, &arrowType) > 0)) {
                if (this->unk_B28 >= 0) {
                    magicArrowType = ARROW_GET_MAGIC_FROM_TYPE(arrowType);

                    if ((ARROW_GET_MAGIC_FROM_TYPE(arrowType) >= ARROW_MAGIC_FIRE) &&
                        (ARROW_GET_MAGIC_FROM_TYPE(arrowType) <= ARROW_MAGIC_LIGHT)) {
                        if (((void)0, gSaveContext.save.saveInfo.playerData.magic) < sMagicArrowCosts[magicArrowType]) {
                            arrowType = ARROW_TYPE_NORMAL;
                            magicArrowType = ARROW_MAGIC_INVALID;
                        }
                    } else if ((arrowType == ARROW_TYPE_DEKU_BUBBLE) &&
                               (!CHECK_WEEKEVENTREG(WEEKEVENTREG_08_01) || (play->sceneId != SCENE_BOWLING))) {
                        magicArrowType = ARROW_MAGIC_DEKU_BUBBLE;
                    } else {
                        magicArrowType = ARROW_MAGIC_INVALID;
                    }

                    this->heldActor = Actor_SpawnAsChild(
                        &play->actorCtx, &this->actor, play, ACTOR_EN_ARROW, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, arrowType);

                    if ((this->heldActor != NULL) && (magicArrowType > ARROW_MAGIC_INVALID)) {
                        Magic_Consume(play, sMagicArrowCosts[magicArrowType], MAGIC_CONSUME_NOW);
                    }
                }
            }
        }

        return true;
    }

    return false;
}

void Player_FinishItemChange(PlayState* play, Player* this) {
    s32 isGoronOrDeku = (this->transformation == PLAYER_FORM_GORON) || (this->transformation == PLAYER_FORM_DEKU);

    if ((this->heldItemAction != PLAYER_IA_NONE) && !isGoronOrDeku) {
        if (Player_SwordFromIA(this, this->heldItemAction) > PLAYER_SWORD_NONE) {
            func_8082E1F0(this, NA_SE_IT_SWORD_PUTAWAY);
        } else {
            func_8082E1F0(this, NA_SE_PL_CHANGE_ARMS);
        }
    }

    Player_UseItem(play, this, this->heldItemId);

    if (!isGoronOrDeku) {
        if (Player_SwordFromIA(this, this->heldItemAction) > PLAYER_SWORD_NONE) {
            func_8082E1F0(this, NA_SE_IT_SWORD_PICKOUT);
        } else if (this->heldItemAction != PLAYER_IA_NONE) {
            func_8082E1F0(this, NA_SE_PL_CHANGE_ARMS);
        }
    }
}

void func_808309CC(PlayState* play, Player* this) {
    if (Player_UpperAction_ChangeHeldItem == this->upperActionFunc) {
        Player_FinishItemChange(play, this);
    }

    Player_SetUpperAction(play, this, sItemActionUpdateFuncs[this->heldItemAction]);
    this->unk_ACC = 0;
    this->idleType = PLAYER_IDLE_DEFAULT;
    Player_DetachHeldActor(play, this);
    this->stateFlags3 &= ~PLAYER_STATE3_START_CHANGING_HELD_ITEM;
}

PlayerAnimationHeader* D_8085CFBC[2] = {
    &gPlayerAnim_link_anchor_waitR2defense,
    &gPlayerAnim_link_anchor_waitR2defense_long,
};
PlayerAnimationHeader* D_8085CFC4[2] = {
    &gPlayerAnim_link_anchor_waitL2defense,
    &gPlayerAnim_link_anchor_waitL2defense_long,
};
PlayerAnimationHeader* D_8085CFCC[2] = {
    &gPlayerAnim_link_anchor_defense_hit,
    &gPlayerAnim_link_anchor_defense_long_hitL,
};
PlayerAnimationHeader* D_8085CFD4[2] = {
    &gPlayerAnim_link_anchor_defense_hit,
    &gPlayerAnim_link_anchor_defense_long_hitR,
};
PlayerAnimationHeader* D_8085CFDC[2] = {
    &gPlayerAnim_link_normal_defense_hit,
    &gPlayerAnim_link_fighter_defense_long_hit,
};

PlayerAnimationHeader* func_80830A58(PlayState* play, Player* this) {
    Player_SetUpperAction(play, this, Player_UpperAction_3);
    Player_DetachHeldActor(play, this);

    if (this->unk_B40 < 0.5f) {
        return D_8085CFBC[Player_IsHoldingTwoHandedWeapon(this)];
    } else {
        return D_8085CFC4[Player_IsHoldingTwoHandedWeapon(this)];
    }
}

void func_80830AE8(Player* this) {
    s32 sfxId = (this->transformation == PLAYER_FORM_GORON)
                    ? NA_SE_PL_GORON_SQUAT
                    : ((this->transformation == PLAYER_FORM_DEKU) ? NA_SE_PL_CHANGE_ARMS : NA_SE_IT_SHIELD_SWING);

    Player_PlaySfx(this, sfxId);
}

void func_80830B38(Player* this) {
    s32 sfxId = (this->transformation == PLAYER_FORM_GORON)
                    ? NA_SE_PL_BALL_TO_GORON
                    : ((this->transformation == PLAYER_FORM_DEKU) ? NA_SE_PL_TAKE_OUT_SHIELD : NA_SE_IT_SHIELD_REMOVE);

    Player_PlaySfx(this, sfxId);
}

s32 func_80830B88(PlayState* play, Player* this) {
    if (GameInteractor_Should(VB_SHIELD_FROM_BUTTON_HOLD, CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_R))) {
        if (!(this->stateFlags1 & (PLAYER_STATE1_400000 | PLAYER_STATE1_800000 | PLAYER_STATE1_20000000))) {
            if (!(this->stateFlags1 & PLAYER_STATE1_8000000) || ((this->currentBoots >= PLAYER_BOOTS_ZORA_UNDERWATER) &&
                                                                 (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
                if ((play->bButtonAmmoPlusOne == 0) && (this->heldItemAction == this->itemAction)) {
                    if ((this->transformation == PLAYER_FORM_FIERCE_DEITY) ||
                        (!Player_IsGoronOrDeku(this) &&
                         ((((this->transformation == PLAYER_FORM_ZORA)) &&
                           !(this->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN)) ||
                          ((this->transformation == PLAYER_FORM_HUMAN) &&
                           (this->currentShield != PLAYER_SHIELD_NONE))) &&
                         Player_IsZTargeting(this))) {
                        PlayerAnimationHeader* anim = func_80830A58(play, this);
                        f32 endFrame = Animation_GetLastFrame(anim);

                        PlayerAnimation_Change(play, &this->skelAnimeUpper, anim, PLAYER_ANIM_NORMAL_SPEED, endFrame,
                                               endFrame, ANIMMODE_ONCE, 0.0f);
                        func_80830AE8(this);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void func_80830CE8(PlayState* play, Player* this) {
    Player_SetUpperAction(play, this, Player_UpperAction_5);

    if (this->itemAction <= PLAYER_IA_MINUS1) {
        func_80123C58(this);
    }

    Animation_Reverse(&this->skelAnimeUpper);
    func_80830B38(this);
}

void Player_WaitToFinishItemChange(PlayState* play, Player* this) {
    ItemChangeInfo* itemChangeEntry = &sPlayerItemChangeInfo[this->itemChangeType];
    f32 changeFrame = itemChangeEntry->changeFrame;

    if (this->skelAnimeUpper.playSpeed < 0.0f) {
        changeFrame -= 1.0f;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnimeUpper, changeFrame)) {
        Player_FinishItemChange(play, this);
    }

    Player_UpdateHostileLockOn(this);
}

s32 func_80830DF0(Player* this, PlayState* play) {
    if (this->stateFlags3 & PLAYER_STATE3_START_CHANGING_HELD_ITEM) {
        Player_StartChangingHeldItem(this, play);
    } else {
        return false;
    }
    return true;
}

s32 func_80830E30(Player* this, PlayState* play) {
    if ((this->heldItemAction == PLAYER_IA_11) || (this->transformation == PLAYER_FORM_ZORA)) {
        Player_SetUpperAction(play, this, Player_UpperAction_12);

        PlayerAnimation_PlayOnce(play, &this->skelAnimeUpper,
                                 (this->meleeWeaponAnimation == PLAYER_MWA_ZORA_PUNCH_LEFT)
                                     ? &gPlayerAnim_pz_cutterwaitA
                                     : ((this->meleeWeaponAnimation == PLAYER_MWA_ZORA_PUNCH_COMBO)
                                            ? &gPlayerAnim_pz_cutterwaitB
                                            : &gPlayerAnim_pz_cutterwaitC));
        this->unk_ACC = 0xA;
    } else {
        if (!func_808306F8(this, play)) {
            return false;
        }

        PlayerAnimation_PlayOnce(play, &this->skelAnimeUpper,
                                 Player_IsHoldingHookshot(this)
                                     ? &gPlayerAnim_link_hook_shot_ready
                                     : ((this->transformation == PLAYER_FORM_DEKU) ? &gPlayerAnim_pn_tamahakidf
                                                                                   : &gPlayerAnim_link_bow_bow_ready));
    }

    if (this->stateFlags1 & PLAYER_STATE1_800000) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_uma_anim_walk);
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->transformation != PLAYER_FORM_ZORA)) {
        Player_Anim_PlayLoop(play, this, Player_GetIdleAnim(this));
    }

    return true;
}

bool func_80830F9C(PlayState* play) {
    return (play->bButtonAmmoPlusOne > 0) && CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_B);
}

bool func_80830FD4(PlayState* play) {
    return (play->bButtonAmmoPlusOne != 0) &&
           ((play->bButtonAmmoPlusOne < 0) ||
            CHECK_BTN_ANY(sPlayerControlInput->cur.button,
                          BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP | BTN_B | BTN_A | BTN_DPAD_EQUIP));
}

bool func_80831010(Player* this, PlayState* play) {
    if ((this->unk_AA5 == PLAYER_UNKAA5_0) || (this->unk_AA5 == PLAYER_UNKAA5_3)) {
        if (Player_IsZTargeting(this) || (this->focusActor != NULL) ||
            (Camera_CheckValidMode(Play_GetCamera(play, CAM_ID_MAIN), CAM_MODE_BOWARROW) == 0)) {
            return true;
        }
        this->unk_AA5 = PLAYER_UNKAA5_3;
    }
    return false;
}

bool func_80831094(Player* this, PlayState* play) {
    if ((this->doorType == PLAYER_DOORTYPE_NONE) && !(this->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN)) {
        if (sPlayerUseHeldItem || func_80830F9C(play)) {
            if (func_80830E30(this, play)) {
                return func_80831010(this, play);
            }
        }
    }
    return false;
}

bool func_80831124(PlayState* play, Player* this) {
    if (this->actor.child != NULL) {
        if (this->heldActor == NULL) {
            this->heldActor = this->actor.child;
            Player_RequestRumble(play, this, 255, 10, 250, SQ(0));
            Player_PlaySfx(this, NA_SE_IT_HOOKSHOT_RECEIVE);
        }
        return true;
    }
    return false;
}

bool func_80831194(PlayState* play, Player* this) {
    if (this->heldActor != NULL) {
        if (!Player_IsHoldingHookshot(this)) {
            ItemId item;
            ArrowType arrowType;

            func_808305BC(play, this, &item, &arrowType);
            if ((this->transformation != PLAYER_FORM_DEKU) && !(this->stateFlags3 & PLAYER_STATE3_400)) {
                if (gSaveContext.minigameStatus == MINIGAME_STATUS_ACTIVE) {
                    if ((play->sceneId != SCENE_SYATEKI_MIZU) && (play->sceneId != SCENE_F01) &&
                        (play->sceneId != SCENE_SYATEKI_MORI)) {
                        play->interfaceCtx.minigameAmmo--;
                    }
                } else if (play->bButtonAmmoPlusOne != 0) {
                    play->bButtonAmmoPlusOne--;
                } else {
                    Inventory_ChangeAmmo(item, -1);
                }
            }

            if (play->bButtonAmmoPlusOne == 1) {
                play->bButtonAmmoPlusOne = -10;
            }

            Player_RequestRumble(play, this, 150, 10, 150, SQ(0));
        } else {
            Player_RequestRumble(play, this, 255, 20, 150, SQ(0));
            this->unk_B48 = 0.0f;
        }

        this->unk_D57 = (this->transformation == PLAYER_FORM_DEKU) ? 20 : 4;
        GameInteractor_ExecuteOnPlayerReleaseHeldActor(play, this, this->heldActor);

        this->heldActor->parent = NULL;
        this->actor.child = NULL;
        this->heldActor = NULL;
        return true;
    }

    return false;
}

void Player_SetParallel(Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_PARALLEL;

    if (!(this->skelAnime.movementFlags & ANIM_FLAG_80) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) && (sShapeYawToTouchedWall < 0x2000)) {
        // snap to the wall
        this->yaw = this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
    }

    this->parallelYaw = this->actor.shape.rot.y;
}

bool func_808313A8(PlayState* play, Player* this, Actor* actor) {
    if (actor == NULL) {
        func_8082DE50(play, this);
        func_80836988(this, play);
        return true;
    }

    return false;
}

void func_808313F0(Player* this, PlayState* play) {
    if (!func_808313A8(play, this, this->heldActor)) {
        Player_SetUpperAction(play, this, Player_UpperAction_CarryActor);
        PlayerAnimation_PlayLoop(play, &this->skelAnimeUpper, &gPlayerAnim_link_normal_carryB_wait);
    }
}

// Stops the current fanfare if a stateflag is set; these two are Kamaro Dancing and Bremen Marching.
void func_80831454(Player* this) {
    if ((this->stateFlags3 & PLAYER_STATE3_20000000) || (this->stateFlags2 & PLAYER_STATE2_2000000)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
    }
}

s32 Player_SetAction(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 arg3) {
    s32 i;
    f32* ptr;

    if (actionFunc == this->actionFunc) {
        return false;
    }

    play->actorCtx.flags &= ~ACTORCTX_FLAG_PICTO_BOX_ON;

    if (this->actor.flags & ACTOR_FLAG_OCARINA_INTERACTION) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
        this->actor.flags &= ~ACTOR_FLAG_OCARINA_INTERACTION;
    } else if ((Player_Action_96 == this->actionFunc) || (Player_Action_93 == this->actionFunc)) {
        this->actor.shape.shadowDraw = ActorShadow_DrawFeet;
        this->actor.shape.shadowScale = this->ageProperties->shadowScale;
        this->unk_ABC = 0.0f;
        if (Player_Action_96 == this->actionFunc) {
            if (this->stateFlags3 & PLAYER_STATE3_80000) {
                Magic_Reset(play);
            }
            func_8082DD2C(play, this);
            this->actor.shape.rot.x = 0;
            this->actor.shape.rot.z = 0;
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_PLAYER_800;
        } else {
            Actor_SetScale(&this->actor, 0.01f);
        }
    } else if ((this->transformation == PLAYER_FORM_GORON) &&
               (Player_GetMeleeWeaponHeld(this) != PLAYER_MELEEWEAPON_NONE)) {
        Player_UseItem(play, this, ITEM_NONE);
    }

    func_800AEF44(Effect_GetByIndex(this->meleeWeaponEffectIndex[2]));
    this->actionFunc = actionFunc;

    if ((this->itemAction != this->heldItemAction) && (!(arg3 & 1) || !(this->stateFlags1 & PLAYER_STATE1_400000))) {
        func_80123C58(this);
    }

    if (!(arg3 & 1) && !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
        func_808309CC(play, this);
        PlayerAnimation_PlayLoop(play, &this->skelAnimeUpper, Player_GetIdleAnim(this));
        this->stateFlags1 &= ~PLAYER_STATE1_400000;
    }

    func_80831454(this);
    Player_Anim_ResetMove(this);

    this->stateFlags1 &= ~(PLAYER_STATE1_TALKING | PLAYER_STATE1_4000000 | PLAYER_STATE1_10000000 |
                           PLAYER_STATE1_20000000 | PLAYER_STATE1_80000000);
    this->stateFlags2 &= ~(PLAYER_STATE2_80000 | PLAYER_STATE2_800000 | PLAYER_STATE2_2000000 |
                           PLAYER_STATE2_USING_OCARINA | PLAYER_STATE2_IDLE_FIDGET);
    this->stateFlags3 &=
        ~(PLAYER_STATE3_2 | PLAYER_STATE3_8 | PLAYER_STATE3_FLYING_WITH_HOOKSHOT | PLAYER_STATE3_200 |
          PLAYER_STATE3_2000 | PLAYER_STATE3_8000 | PLAYER_STATE3_10000 | PLAYER_STATE3_20000 | PLAYER_STATE3_40000 |
          PLAYER_STATE3_80000 | PLAYER_STATE3_200000 | PLAYER_STATE3_1000000 | PLAYER_STATE3_20000000);

    this->av1.actionVar1 = 0;
    this->av2.actionVar2 = 0;
    this->idleType = PLAYER_IDLE_DEFAULT;
    this->unk_B86[0] = 0;
    this->unk_B86[1] = 0;
    this->unk_B8A = 0;
    this->unk_B8C = 0;
    this->unk_B8E = 0;

    // TODO: Is there no other way to write this that works?
    i = 0;
    ptr = this->unk_B10;
    do {
        *ptr = 0.0f;
        ptr++;
        i++;
    } while (i < ARRAY_COUNT(this->unk_B10));

    this->actor.shape.rot.z = 0;

    Player_ResetCylinder(this);
    func_8082E00C(this);

    return true;
}

void Player_SetAction_PreserveMoveFlags(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 arg3) {
    s32 savedMovementFlags = this->skelAnime.movementFlags;

    this->skelAnime.movementFlags = 0;
    Player_SetAction(play, this, actionFunc, arg3);
    this->skelAnime.movementFlags = savedMovementFlags;
}

void Player_SetAction_PreserveItemAction(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 arg3) {
    if (this->itemAction > PLAYER_IA_MINUS1) {
        PlayerItemAction heldItemAction = this->itemAction;

        this->itemAction = this->heldItemAction;
        Player_SetAction(play, this, actionFunc, arg3);
        this->itemAction = heldItemAction;

        Player_SetModels(this, Player_ActionToModelGroup(this, this->itemAction));
    }
}

void Player_DestroyHookshot(Player* this) {
    if (Player_IsHoldingHookshot(this)) {
        if (this->heldActor != NULL) {
            Actor_Kill(this->heldActor);
            this->actor.child = NULL;
            this->heldActor = NULL;
        }
    }
}

s32 func_80831814(Player* this, PlayState* play, PlayerUnkAA5 arg2) {
    if (!(this->stateFlags1 &
          (PLAYER_STATE1_4 | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_2000 | PLAYER_STATE1_4000))) {
        if (Camera_CheckValidMode(Play_GetCamera(play, CAM_ID_MAIN), CAM_MODE_FIRSTPERSON) != 0) {
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                (func_801242B4(this) && (this->actor.depthInWater < this->ageProperties->unk_2C))) {
                this->unk_AA5 = arg2;
                return true;
            }
        }
    }
    return false;
}

// Toggle Lens
void func_808318C0(PlayState* play) {
    if (Magic_Consume(play, 0, MAGIC_CONSUME_LENS)) {
        if (play->actorCtx.lensActive) {
            Actor_DeactivateLens(play);
        } else {
            play->actorCtx.lensActive = true;
        }

        Audio_PlaySfx(play->actorCtx.lensActive ? NA_SE_SY_GLASSMODE_ON : NA_SE_SY_GLASSMODE_OFF);
    } else {
        Audio_PlaySfx(NA_SE_SY_ERROR);
    }
}

// Toggle Lens from a button press
void func_80831944(PlayState* play, Player* this) {
    if (Player_GetItemOnButton(play, this, func_8082FDC4()) == ITEM_LENS_OF_TRUTH) {
        func_808318C0(play);
    }
    // #region 2S2H [Dpad]
    else if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
        if (Player_Dpad_GetItemOnButton(play, this, func_Dpad_8082FDC4()) == ITEM_LENS_OF_TRUTH) {
            func_808318C0(play);
        }
    }
    // #endregion
}

void Player_UseItem(PlayState* play, Player* this, ItemId item) {
    PlayerItemAction itemAction = Player_ItemToItemAction(this, item);

    if ((((this->heldItemAction == this->itemAction) &&
          (!(this->stateFlags1 & PLAYER_STATE1_400000) ||
           (Player_MeleeWeaponFromIA(itemAction) != PLAYER_MELEEWEAPON_NONE) || (itemAction == PLAYER_IA_NONE))) ||
         ((this->itemAction <= PLAYER_IA_MINUS1) &&
          ((Player_MeleeWeaponFromIA(itemAction) != PLAYER_MELEEWEAPON_NONE) || (itemAction == PLAYER_IA_NONE)))) &&
        ((itemAction == PLAYER_IA_NONE) || !(this->stateFlags1 & PLAYER_STATE1_8000000) ||
         (GameInteractor_Should(VB_USE_ITEM_CONSIDER_ITEM_ACTION, itemAction == PLAYER_IA_MASK_ZORA, &itemAction)) ||
         ((this->currentBoots >= PLAYER_BOOTS_ZORA_UNDERWATER) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)))) {
        s32 var_v1 = ((itemAction >= PLAYER_IA_MASK_MIN) && (itemAction <= PLAYER_IA_MASK_MAX) &&
                      (!GameInteractor_Should(VB_USE_ITEM_CONSIDER_LINK_HUMAN,
                                              this->transformation == PLAYER_FORM_HUMAN, &itemAction) ||
                       (itemAction >= PLAYER_IA_MASK_GIANT)));
        CollisionPoly* sp5C;
        s32 sp58;
        f32 sp54;
        PlayerExplosive explosiveType;

        if (var_v1 || (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK) && (itemAction != PLAYER_IA_NONE)) ||
            (itemAction == PLAYER_IA_OCARINA) ||
            ((itemAction > PLAYER_IA_BOTTLE_MIN) && itemAction < PLAYER_IA_MASK_MIN) ||
            ((itemAction == PLAYER_IA_PICTOGRAPH_BOX) && (this->talkActor != NULL) &&
             (this->exchangeItemAction > PLAYER_IA_NONE))) {
            if (var_v1) {
                PlayerTransformation playerForm = (itemAction < PLAYER_IA_MASK_FIERCE_DEITY)
                                                      ? PLAYER_FORM_HUMAN
                                                      : itemAction - PLAYER_IA_MASK_FIERCE_DEITY;

                if (((this->currentMask != PLAYER_MASK_GIANT) && (itemAction == PLAYER_IA_MASK_GIANT) &&
                     ((gSaveContext.magicState != MAGIC_STATE_IDLE) ||
                      (gSaveContext.save.saveInfo.playerData.magic == 0))) ||
                    (!(this->stateFlags1 & PLAYER_STATE1_8000000) &&
                     BgCheck_EntityCheckCeiling(&play->colCtx, &sp54, &this->actor.world.pos,
                                                sPlayerAgeProperties[playerForm].ceilingCheckHeight, &sp5C, &sp58,
                                                &this->actor))) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    return;
                }
            }
            if ((itemAction == PLAYER_IA_MAGIC_BEANS) && (AMMO(ITEM_MAGIC_BEANS) == 0)) {
                Audio_PlaySfx(NA_SE_SY_ERROR);
            } else {
                this->itemAction = itemAction;
                this->unk_AA5 = PLAYER_UNKAA5_5;
            }
        } else if (((itemAction == PLAYER_IA_DEKU_STICK) && (AMMO(ITEM_DEKU_STICK) == 0)) ||
                   (((play->unk_1887D != 0) || (play->unk_1887E != 0)) &&
                    GameInteractor_Should(VB_LIMIT_EXPLOSIVES,
                                          play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].length >= 5)) ||
                   ((play->unk_1887D == 0) && (play->unk_1887E == 0) &&
                    ((explosiveType = Player_ExplosiveFromIA(this, itemAction)) > PLAYER_EXPLOSIVE_NONE) &&
                    ((AMMO(sPlayerExplosiveInfo[explosiveType].itemId) == 0) ||
                     GameInteractor_Should(VB_LIMIT_EXPLOSIVES,
                                           play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].length >= 3)))) {
            // Prevent some items from being used if player is out of ammo.
            // Also prevent explosives from being used if too many are active
            Audio_PlaySfx(NA_SE_SY_ERROR);
        } else if (itemAction == PLAYER_IA_LENS_OF_TRUTH) {
            // Handle Lens of Truth
            func_808318C0(play);
        } else if (itemAction == PLAYER_IA_PICTOGRAPH_BOX) {
            // Handle Pictograph Box
            if (!func_80831814(this, play, PLAYER_UNKAA5_2)) {
                Audio_PlaySfx(NA_SE_SY_ERROR);
            }
        } else if ((itemAction == PLAYER_IA_DEKU_NUT) &&
                   ((this->transformation != PLAYER_FORM_DEKU) || (this->heldItemButton != 0))) {
            // Handle Deku Nuts
            if (AMMO(ITEM_DEKU_NUT) != 0) {
                func_8083A658(play, this);
            } else {
                Audio_PlaySfx(NA_SE_SY_ERROR);
            }
        } else if (GameInteractor_Should(VB_USE_ITEM_CONSIDER_LINK_HUMAN, this->transformation == PLAYER_FORM_HUMAN,
                                         &itemAction) &&
                   (itemAction >= PLAYER_IA_MASK_MIN) && (itemAction < PLAYER_IA_MASK_GIANT)) {
            PlayerMask maskId = GET_MASK_FROM_IA(itemAction);

            if (GameInteractor_Should(VB_USE_ITEM_EQUIP_MASK, true, &maskId)) {
                // Handle wearable masks
                this->prevMask = this->currentMask;
                if (maskId == this->currentMask) {
                    this->currentMask = PLAYER_MASK_NONE;
                    func_8082E1F0(this, NA_SE_PL_TAKE_OUT_SHIELD);
                } else {
                    this->currentMask = maskId;
                    func_8082E1F0(this, NA_SE_PL_CHANGE_ARMS);
                }
                gSaveContext.save.equippedMask = this->currentMask;
            }
        } else if ((itemAction != this->heldItemAction) ||
                   ((this->heldActor == NULL) && (Player_ExplosiveFromIA(this, itemAction) > PLAYER_EXPLOSIVE_NONE))) {
            u8 nextAnimType;

            // Handle using a new held item
            this->nextModelGroup = Player_ActionToModelGroup(this, itemAction);
            nextAnimType = gPlayerModelTypes[this->nextModelGroup].modelAnimType;
            var_v1 = ((this->transformation != PLAYER_FORM_GORON) || (itemAction == PLAYER_IA_POWDER_KEG));

            if (var_v1 && (this->heldItemAction >= 0) && (item != this->heldItemId) &&
                (sPlayerItemChangeTypes[gPlayerModelTypes[this->modelGroup].modelAnimType][nextAnimType] !=
                 PLAYER_ITEM_CHG_0)) {
                // Start the held item change process
                this->heldItemId = item;
                this->stateFlags3 |= PLAYER_STATE3_START_CHANGING_HELD_ITEM;
            } else {
                // Init new held item for use
                Player_DestroyHookshot(this);
                Player_DetachHeldActor(play, this);
                Player_InitItemActionWithAnim(play, this, itemAction);
                if (!var_v1) {
                    sPlayerUseHeldItem = true;
                    sPlayerHeldItemButtonIsHeldDown = true;
                }
            }
        } else {
            sPlayerUseHeldItem = true;
            sPlayerHeldItemButtonIsHeldDown = true;
        }
    }
}

void func_80831F34(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    s32 sp24 = func_801242B4(this);

    func_8082DE50(play, this);
    Player_SetAction(play, this, sp24 ? Player_Action_62 : Player_Action_24, 0);
    Player_Anim_PlayOnce(play, this, anim);

    if (anim == &gPlayerAnim_link_derth_rebirth) {
        this->skelAnime.endFrame = 84.0f;
    }

    this->stateFlags1 |= PLAYER_STATE1_DEAD;

    func_8082DAD4(this);
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DOWN);

    if (this == GET_PLAYER(play)) {
        this->csId = play->playerCsIds[PLAYER_CS_ID_DEATH];
        Audio_SetBgmVolumeOff();
        gSaveContext.powderKegTimer = 0;
        gSaveContext.unk_1014 = 0;
        gSaveContext.jinxTimer = 0;

        if (Inventory_ConsumeFairy(play)) {
            play->gameOverCtx.state = GAMEOVER_REVIVE_START;
            this->av1.actionVar1 = 1;
        } else {
            play->gameOverCtx.state = GAMEOVER_DEATH_START;
            Audio_StopFanfare(0);
            Audio_PlayFanfare(NA_BGM_GAME_OVER);
            gSaveContext.seqId = NA_BGM_DISABLED;
            gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
        }

        ShrinkWindow_Letterbox_SetSizeTarget(32);
    }
}

bool Player_CanUpdateItems(Player* this) {
    return (!(Player_Action_WaitForPutAway == this->actionFunc) ||
            ((this->stateFlags3 & PLAYER_STATE3_START_CHANGING_HELD_ITEM) &&
             ((this->heldItemId == ITEM_FC) || (this->heldItemId == ITEM_NONE)))) &&
           (!(Player_UpperAction_ChangeHeldItem == this->upperActionFunc) ||
            Player_ItemToItemAction(this, this->heldItemId) == this->heldItemAction);
}

// Whether action is Bremen marching or Kamaro dancing
bool func_8083213C(Player* this) {
    return (Player_Action_11 == this->actionFunc) || (Player_Action_12 == this->actionFunc);
}

bool Player_UpdateUpperBody(Player* this, PlayState* play) {
    if (!(this->stateFlags1 & PLAYER_STATE1_800000) && (this->actor.parent != NULL) && Player_IsHoldingHookshot(this)) {
        Player_SetAction(play, this, Player_Action_HookshotFly, 1);
        this->stateFlags3 |= PLAYER_STATE3_FLYING_WITH_HOOKSHOT;
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_hook_fly_start);
        Player_AnimReplace_Setup(
            play, this,
            (ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE | ANIM_FLAG_80));
        func_8082DAD4(this);
        this->yaw = this->actor.shape.rot.y;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        this->unk_AA6_rotFlags |= UNKAA6_ROT_FOCUS_X | UNKAA6_ROT_FOCUS_Y | UNKAA6_ROT_UPPER_X;
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_LASH);
        return true;
    }

    if (Player_CanUpdateItems(this)) {
        Player_UpdateItems(this, play);
        if (Player_Action_64 == this->actionFunc) {
            return true;
        }
    }

    if (!this->upperActionFunc(this, play)) {
        return false;
    }

    if (this->skelAnimeUpperBlendWeight != 0.0f) {
        if ((Player_CheckForIdleAnim(this) == IDLE_ANIM_NONE) || (this->speedXZ != 0.0f)) {
            AnimTaskQueue_AddCopyUsingMapInverted(play, this->skelAnime.limbCount, this->skelAnimeUpper.jointTable,
                                                  this->skelAnime.jointTable, sPlayerUpperBodyLimbCopyMap);
        }
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
            !(this->skelAnime.movementFlags & ANIM_FLAG_ENABLE_MOVEMENT)) {
            Math_StepToF(&this->skelAnimeUpperBlendWeight, 0.0f, 0.25f);
            AnimTaskQueue_AddInterp(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                    this->skelAnimeUpper.jointTable, 1.0f - this->skelAnimeUpperBlendWeight);
        }
    } else if ((Player_CheckForIdleAnim(this) == IDLE_ANIM_NONE) || (this->speedXZ != 0.0f) ||
               (this->skelAnime.movementFlags & ANIM_FLAG_ENABLE_MOVEMENT)) {
        AnimTaskQueue_AddCopyUsingMap(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                      this->skelAnimeUpper.jointTable, sPlayerUpperBodyLimbCopyMap);
    } else {
        AnimTaskQueue_AddCopy(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                              this->skelAnimeUpper.jointTable);
    }

    return true;
}

bool func_808323C0(Player* this, s16 csId) {
    if ((csId > CS_ID_NONE) && (CutsceneManager_GetCurrentCsId() != csId)) {
        if (!CutsceneManager_IsNext(csId)) {
            CutsceneManager_Queue(csId);

            return false;
        }
        CutsceneManager_Start(csId, &this->actor);
    }
    this->csId = csId;

    return true;
}

bool func_80832444(Player* this) {
    if (this->csId > CS_ID_NONE) {
        if (!CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_Queue(this->csId);
            return false;
        }

        CutsceneManager_Start(this->csId, &this->actor);
    }
    return true;
}

bool func_8083249C(Player* this) {
    if ((this->csId > CS_ID_NONE) && (CutsceneManager_GetCurrentCsId() != this->csId)) {
        return func_80832444(this);
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
 * This will also initiate a cutscene with the cutscene id provided.
 *
 * @return  From `Player_PutAwayHeldItem`: true if an item needs to be put away, false if not.
 */
s32 Player_SetupWaitForPutAwayWithCs(PlayState* play, Player* this, AfterPutAwayFunc afterPutAwayFunc, s32 csId) {
    this->afterPutAwayFunc = afterPutAwayFunc;
    this->csId = csId;
    Player_SetAction(play, this, Player_Action_WaitForPutAway, 0);
    func_8083249C(this);
    this->stateFlags2 |= PLAYER_STATE2_40;

    return Player_PutAwayHeldItem(play, this);
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
    return Player_SetupWaitForPutAwayWithCs(play, this, afterPutAwayFunc, CS_ID_NONE);
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

    if (!(this->stateFlags2 & (PLAYER_STATE2_20 | PLAYER_STATE2_40))) {
        Actor* focusActor = this->focusActor;

        if ((focusActor != NULL) &&
            ((play->actorCtx.attention.reticleSpinCounter != 0) || (this != GET_PLAYER(play))) &&
            (focusActor->id != ACTOR_OBJ_NOZOKI)) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &focusActor->focus.pos),
                               0xFA0);
        } else if ((this->stateFlags1 & PLAYER_STATE1_PARALLEL) &&
                   !(this->stateFlags2 & (PLAYER_STATE2_20 | PLAYER_STATE2_40))) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, this->parallelYaw, 0xFA0);
        }
    } else if (!(this->stateFlags2 & PLAYER_STATE2_40)) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->yaw, 0x7D0);
    }

    this->unk_B4C = this->actor.shape.rot.y - previousYaw;
}

/**
 * Step a value by `step` to a `target` value.
 * Constrains the value to be no further than `constraintRange` from `constraintMid` (accounting for wrapping).
 * Constrains the value to be no further than `overflowRange` from 0.
 * If this second constraint is enforced, return how much the value was past by the range, or return 0.
 *
 * @return The amount by which the value overflowed the absolute range defined by `overflowRange`
 */
s16 Player_ScaledStepBinangClamped(s16* pValue, s16 target, s16 step, s16 overflowRange, s16 constraintMid,
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

s16 func_80832754(Player* this, s32 arg1) {
    s16 targetUpperBodyYaw;
    s16 yaw = this->actor.shape.rot.y;

    if (arg1) {
        this->upperLimbRot.x = this->actor.focus.rot.x;
        yaw = this->actor.focus.rot.y;
        this->unk_AA6_rotFlags |= UNKAA6_ROT_FOCUS_X | UNKAA6_ROT_UPPER_X;
    } else {
        s16 temp = Player_ScaledStepBinangClamped(&this->headLimbRot.x, this->actor.focus.rot.x, 0x258, 0x2710,
                                                  this->actor.focus.rot.x, 0);

        Player_ScaledStepBinangClamped(&this->upperLimbRot.x, temp, 0xC8, 0xFA0, this->headLimbRot.x, 0x2710);

        // Step the upper body and head yaw to the focus yaw.
        // Eventually prefers turning the upper body rather than the head.
        targetUpperBodyYaw = this->actor.focus.rot.y - yaw;
        Player_ScaledStepBinangClamped(&targetUpperBodyYaw, 0, 0xC8, 0x5DC0, this->upperLimbRot.y, 0x1F40);
        yaw = this->actor.focus.rot.y - targetUpperBodyYaw;
        Player_ScaledStepBinangClamped(&this->headLimbRot.y, (targetUpperBodyYaw - this->upperLimbRot.y), 0xC8, 0x1F40,
                                       targetUpperBodyYaw, 0x1F40);
        Player_ScaledStepBinangClamped(&this->upperLimbRot.y, targetUpperBodyYaw, 0xC8, 0x1F40, this->headLimbRot.y,
                                       0x1F40);

        this->unk_AA6_rotFlags |=
            UNKAA6_ROT_FOCUS_X | UNKAA6_ROT_HEAD_X | UNKAA6_ROT_HEAD_Y | UNKAA6_ROT_UPPER_X | UNKAA6_ROT_UPPER_Y;
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
 * Tatl is hovering over will be chosen. If there is an active lock-on, the next available
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
    Actor* nextLockOnActor;
    s32 zButtonHeld = CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_Z);
    s32 isTalking;
    s32 usingHoldTargeting;

    if (!zButtonHeld) {
        this->stateFlags1 &= ~PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) || (this->csAction != PLAYER_CSACTION_NONE) ||
        (this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_20000000)) ||
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
        (this->stateFlags1 & (PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_ZORA_BOOMERANG_THROWN))) {
        if (!isTalking) {
            if (!(this->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN) &&
                ((this->heldItemAction != PLAYER_IA_FISHING_ROD) || (this->unk_B28 == 0)) &&
                CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_Z)) {

                if (this == GET_PLAYER(play)) {
                    // The next lock-on actor defaults to the actor Tatl is hovering over.
                    // This may change to the arrow hover actor below.
                    nextLockOnActor = play->actorCtx.attention.tatlHoverActor;
                } else {
                    // Kafei will always lock onto the player.
                    nextLockOnActor = &GET_PLAYER(play)->actor;
                }

                // Get saved Z Target setting.
                // Kafei uses Hold Targeting.
                usingHoldTargeting = (gSaveContext.options.zTargetSetting != 0) || (this != GET_PLAYER(play));

                this->stateFlags1 |= PLAYER_STATE1_Z_TARGETING;

                if ((this->currentMask != PLAYER_MASK_GIANT) && (nextLockOnActor != NULL) &&
                    !(nextLockOnActor->flags & ACTOR_FLAG_LOCK_ON_DISABLED) &&
                    !(this->stateFlags3 & (PLAYER_STATE3_200 | PLAYER_STATE3_2000))) {

                    // Tatl hovers over the current lock-on actor, so `nextLockOnActor` and `focusActor`
                    // will be the same if already locked on.
                    // In this case, `nextLockOnActor` will be the arrow hover actor instead.
                    if ((nextLockOnActor == this->focusActor) && (this == GET_PLAYER(play))) {
                        nextLockOnActor = play->actorCtx.attention.arrowHoverActor;
                    }

                    if ((nextLockOnActor != NULL) && (((nextLockOnActor != this->focusActor)) ||
                                                      (nextLockOnActor->flags & ACTOR_FLAG_FOCUS_ACTOR_REFINDABLE))) {
                        // Set new lock-on

                        nextLockOnActor->flags &= ~ACTOR_FLAG_FOCUS_ACTOR_REFINDABLE;

                        if (!usingHoldTargeting) {
                            this->stateFlags2 |= PLAYER_STATE2_LOCK_ON_WITH_SWITCH;
                        }

                        this->focusActor = nextLockOnActor;
                        this->zTargetActiveTimer = 15;
                        this->stateFlags2 &= ~(PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER | PLAYER_STATE2_200000);
                    } else if (!usingHoldTargeting) {
                        Player_ReleaseLockOn(this);
                    }
                    this->stateFlags1 &= ~PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE;
                } else {
                    // Lock-on was not started above. Set Parallel Mode.
                    if (!(this->stateFlags1 & (PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE)) &&
                        (Player_Action_95 != this->actionFunc)) {
                        Player_SetParallel(this);
                    }
                }
            }

            if (this->focusActor != NULL) {
                if ((this == GET_PLAYER(play)) && (this->focusActor != this->autoLockOnActor) &&
                    Attention_ShouldReleaseLockOn(this->focusActor, this, ignoreLeash)) {
                    Player_ReleaseLockOn(this);
                    this->stateFlags1 |= PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE;
                } else if (this->focusActor != NULL) {
                    this->focusActor->targetPriority = 0x28;
                }
            } else if (this->autoLockOnActor != NULL) {
                // Because of the previous if condition above, `autoLockOnActor` does not take precedence
                // over `focusActor` if it already exists.
                // However, `autoLockOnActor` is expected to be set with `Player_SetAutoLockOnActor`
                // which will release any existing lock-on before setting the new one.
                this->focusActor = this->autoLockOnActor;
            }
        }

        if ((this->focusActor != NULL) && !(this->stateFlags3 & (PLAYER_STATE3_200 | PLAYER_STATE3_2000))) {
            this->stateFlags1 &= ~(PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS | PLAYER_STATE1_PARALLEL);

            // Check if an actor is not hostile, aka "friendly", to set `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS`.
            //
            // When carrying another actor, `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS` will be set even if the actor
            // is hostile. This is a special case to allow Player to have more freedom of movement and be able
            // to throw a carried actor at the lock-on actor, even if it is hostile.
            if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) ||
                !CHECK_FLAG_ALL(this->focusActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)) {
                this->stateFlags1 |= PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS;
            }
        } else if (this->stateFlags1 & PLAYER_STATE1_PARALLEL) {
            this->stateFlags2 &= ~PLAYER_STATE2_LOCK_ON_WITH_SWITCH;
        } else {
            Player_ClearZTargeting(this);
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

    if ((this->unk_AA5 != PLAYER_UNKAA5_0) || func_8082DA90(play) || (this->stateFlags1 & PLAYER_STATE1_1)) {
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

        if (this->transformation == PLAYER_FORM_FIERCE_DEITY) {
            *outSpeedTarget *= 1.5f;
        }

        if (sControlStickMagnitude != 0.0f) {
            f32 floorPitchInfluence = Math_SinS(this->floorPitch);
            f32 speedCap = this->unk_B50;
            f32 var_fa1;

            if (this->unk_AB8 != 0.0f) {
                var_fa1 = (this->focusActor != NULL) ? 0.002f : 0.008f;

                speedCap -= this->unk_AB8 * var_fa1;
                speedCap = CLAMP_MIN(speedCap, 2.0f);
            }

            *outSpeedTarget = (*outSpeedTarget * 0.14f) - (8.0f * floorPitchInfluence * floorPitchInfluence);
            *outSpeedTarget = CLAMP(*outSpeedTarget, 0.0f, speedCap);

            //! FAKE
            if (floorPitchInfluence) {}

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
    return Math_StepToF(&this->speedXZ, 0.0f, R_DECELERATE_RATE / 100.0f);
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
            if ((play->actorCtx.attention.reticleSpinCounter != 0) && !(this->stateFlags2 & PLAYER_STATE2_40)) {
                *outYawTarget = Math_Vec3f_Yaw(&this->actor.world.pos, &this->focusActor->focus.pos);
            }
        } else if (Player_FriendlyLockOnOrParallel(this)) {
            *outYawTarget = this->parallelYaw;
        }

        return false;
    }

    *outYawTarget += Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
    return true;
}

typedef enum ActionHandlerIndex {
    /* 0x0 */ PLAYER_ACTION_HANDLER_0,
    /* 0x1 */ PLAYER_ACTION_HANDLER_1,
    /* 0x2 */ PLAYER_ACTION_HANDLER_2,
    /* 0x3 */ PLAYER_ACTION_HANDLER_3,
    /* 0x4 */ PLAYER_ACTION_HANDLER_TALK,
    /* 0x5 */ PLAYER_ACTION_HANDLER_5,
    /* 0x6 */ PLAYER_ACTION_HANDLER_6,
    /* 0x7 */ PLAYER_ACTION_HANDLER_7,
    /* 0x8 */ PLAYER_ACTION_HANDLER_8,
    /* 0x9 */ PLAYER_ACTION_HANDLER_9,
    /* 0xA */ PLAYER_ACTION_HANDLER_10,
    /* 0xB */ PLAYER_ACTION_HANDLER_11,
    /* 0xC */ PLAYER_ACTION_HANDLER_12,
    /* 0xD */ PLAYER_ACTION_HANDLER_13,
    /* 0xE */ PLAYER_ACTION_HANDLER_14,
    /* 0xF */ PLAYER_ACTION_HANDLER_MAX
} ActionHandlerIndex;

/**
 * The values of following arrays are used as indices for the `sActionHandlerFuncs` array.
 * Each index correspond to a function which will be called sequentially until any of them return `true`.
 * Negative marks the end of the array.
 */
s8 sActionHandlerList1[] = {
    /* 0 */ PLAYER_ACTION_HANDLER_13,
    /* 1 */ PLAYER_ACTION_HANDLER_2,
    /* 2 */ PLAYER_ACTION_HANDLER_TALK,
    /* 3 */ PLAYER_ACTION_HANDLER_9,
    /* 4 */ PLAYER_ACTION_HANDLER_10,
    /* 5 */ PLAYER_ACTION_HANDLER_11,
    /* 6 */ PLAYER_ACTION_HANDLER_8,
    /* 7 */ -PLAYER_ACTION_HANDLER_7,
};

s8 sActionHandlerList2[] = {
    /*  0 */ PLAYER_ACTION_HANDLER_13,
    /*  1 */ PLAYER_ACTION_HANDLER_1,
    /*  2 */ PLAYER_ACTION_HANDLER_2,
    /*  3 */ PLAYER_ACTION_HANDLER_5,
    /*  4 */ PLAYER_ACTION_HANDLER_3,
    /*  5 */ PLAYER_ACTION_HANDLER_TALK,
    /*  6 */ PLAYER_ACTION_HANDLER_9,
    /*  7 */ PLAYER_ACTION_HANDLER_10,
    /*  8 */ PLAYER_ACTION_HANDLER_11,
    /*  9 */ PLAYER_ACTION_HANDLER_7,
    /* 10 */ PLAYER_ACTION_HANDLER_8,
    /* 11 */ -PLAYER_ACTION_HANDLER_6,
};

s8 sActionHandlerList3[] = {
    /*  0 */ PLAYER_ACTION_HANDLER_13,
    /*  1 */ PLAYER_ACTION_HANDLER_1,
    /*  2 */ PLAYER_ACTION_HANDLER_2,
    /*  3 */ PLAYER_ACTION_HANDLER_3,
    /*  4 */ PLAYER_ACTION_HANDLER_TALK,
    /*  5 */ PLAYER_ACTION_HANDLER_9,
    /*  6 */ PLAYER_ACTION_HANDLER_10,
    /*  7 */ PLAYER_ACTION_HANDLER_11,
    /*  8 */ PLAYER_ACTION_HANDLER_8,
    /*  9 */ PLAYER_ACTION_HANDLER_7,
    /* 10 */ -PLAYER_ACTION_HANDLER_6,
};

s8 sActionHandlerList4[] = {
    /* 0 */ PLAYER_ACTION_HANDLER_13,
    /* 1 */ PLAYER_ACTION_HANDLER_2,
    /* 2 */ PLAYER_ACTION_HANDLER_TALK,
    /* 3 */ PLAYER_ACTION_HANDLER_9,
    /* 4 */ PLAYER_ACTION_HANDLER_10,
    /* 5 */ PLAYER_ACTION_HANDLER_11,
    /* 6 */ PLAYER_ACTION_HANDLER_8,
    /* 7 */ -PLAYER_ACTION_HANDLER_7,
};

s8 sActionHandlerList5[] = {
    /* 0 */ PLAYER_ACTION_HANDLER_13,
    /* 1 */ PLAYER_ACTION_HANDLER_2,
    /* 2 */ PLAYER_ACTION_HANDLER_TALK,
    /* 3 */ PLAYER_ACTION_HANDLER_9,
    /* 4 */ PLAYER_ACTION_HANDLER_10,
    /* 5 */ PLAYER_ACTION_HANDLER_11,
    /* 6 */ PLAYER_ACTION_HANDLER_12,
    /* 7 */ PLAYER_ACTION_HANDLER_8,
    /* 8 */ -PLAYER_ACTION_HANDLER_7,
};

s8 sActionHandlerListTurnInPlace[] = {
    /* 0 */ -PLAYER_ACTION_HANDLER_7,
};

s8 sActionHandlerListIdle[] = {
    /*  0 */ PLAYER_ACTION_HANDLER_0,
    /*  1 */ PLAYER_ACTION_HANDLER_11,
    /*  2 */ PLAYER_ACTION_HANDLER_1,
    /*  3 */ PLAYER_ACTION_HANDLER_2,
    /*  4 */ PLAYER_ACTION_HANDLER_3,
    /*  5 */ PLAYER_ACTION_HANDLER_5,
    /*  6 */ PLAYER_ACTION_HANDLER_TALK,
    /*  7 */ PLAYER_ACTION_HANDLER_9,
    /*  8 */ PLAYER_ACTION_HANDLER_8,
    /*  9 */ PLAYER_ACTION_HANDLER_7,
    /* 10 */ -PLAYER_ACTION_HANDLER_6,
};

s8 sActionHandlerList8[] = {
    /*  0 */ PLAYER_ACTION_HANDLER_0,
    /*  1 */ PLAYER_ACTION_HANDLER_11,
    /*  2 */ PLAYER_ACTION_HANDLER_1,
    /*  3 */ PLAYER_ACTION_HANDLER_2,
    /*  4 */ PLAYER_ACTION_HANDLER_3,
    /*  5 */ PLAYER_ACTION_HANDLER_12,
    /*  6 */ PLAYER_ACTION_HANDLER_5,
    /*  7 */ PLAYER_ACTION_HANDLER_TALK,
    /*  8 */ PLAYER_ACTION_HANDLER_9,
    /*  9 */ PLAYER_ACTION_HANDLER_8,
    /* 10 */ PLAYER_ACTION_HANDLER_7,
    /* 11 */ -PLAYER_ACTION_HANDLER_6,
};

s8 sActionHandlerList9[] = {
    /*  0 */ PLAYER_ACTION_HANDLER_13,
    /*  1 */ PLAYER_ACTION_HANDLER_1,
    /*  2 */ PLAYER_ACTION_HANDLER_2,
    /*  3 */ PLAYER_ACTION_HANDLER_3,
    /*  4 */ PLAYER_ACTION_HANDLER_12,
    /*  5 */ PLAYER_ACTION_HANDLER_5,
    /*  6 */ PLAYER_ACTION_HANDLER_TALK,
    /*  7 */ PLAYER_ACTION_HANDLER_9,
    /*  8 */ PLAYER_ACTION_HANDLER_10,
    /*  9 */ PLAYER_ACTION_HANDLER_11,
    /* 10 */ PLAYER_ACTION_HANDLER_8,
    /* 11 */ PLAYER_ACTION_HANDLER_7,
    /* 12 */ -PLAYER_ACTION_HANDLER_6,
};

s8 sActionHandlerList10[] = {
    /* 0 */ PLAYER_ACTION_HANDLER_10,
    /* 1 */ PLAYER_ACTION_HANDLER_8,
    /* 2 */ -PLAYER_ACTION_HANDLER_7,
};

s8 sActionHandlerList11[] = {
    /* 0 */ PLAYER_ACTION_HANDLER_0,
    /* 1 */ PLAYER_ACTION_HANDLER_12,
    /* 2 */ PLAYER_ACTION_HANDLER_5,
    /* 3 */ PLAYER_ACTION_HANDLER_TALK,
    /* 4 */ -PLAYER_ACTION_HANDLER_14,
};

s8 sActionHandlerList12[] = {
    /* 0 */ PLAYER_ACTION_HANDLER_13,
    /* 1 */ PLAYER_ACTION_HANDLER_2,
    /* 2 */ -PLAYER_ACTION_HANDLER_TALK,
};

s32 (*sActionHandlerFuncs[PLAYER_ACTION_HANDLER_MAX])(Player* this, PlayState* play) = {
    Player_ActionHandler_0,    // PLAYER_ACTION_HANDLER_0
    Player_ActionHandler_1,    // PLAYER_ACTION_HANDLER_1
    Player_ActionHandler_2,    // PLAYER_ACTION_HANDLER_2
    Player_ActionHandler_3,    // PLAYER_ACTION_HANDLER_3
    Player_ActionHandler_Talk, // PLAYER_ACTION_HANDLER_TALK
    Player_ActionHandler_5,    // PLAYER_ACTION_HANDLER_5
    Player_ActionHandler_6,    // PLAYER_ACTION_HANDLER_6
    Player_ActionHandler_7,    // PLAYER_ACTION_HANDLER_7
    Player_ActionHandler_8,    // PLAYER_ACTION_HANDLER_8
    Player_ActionHandler_9,    // PLAYER_ACTION_HANDLER_9
    Player_ActionHandler_10,   // PLAYER_ACTION_HANDLER_10
    Player_ActionHandler_11,   // PLAYER_ACTION_HANDLER_11
    Player_ActionHandler_12,   // PLAYER_ACTION_HANDLER_12
    Player_ActionHandler_13,   // PLAYER_ACTION_HANDLER_13
    Player_ActionHandler_14,   // PLAYER_ACTION_HANDLER_14
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
    if (!(this->stateFlags1 & (PLAYER_STATE1_1 | PLAYER_STATE1_DEAD | PLAYER_STATE1_20000000)) &&
        !func_8082DA90(play)) {
        if (updateUpperBody) {
            sUpperBodyIsBusy = Player_UpdateUpperBody(this, play);
            if (Player_Action_64 == this->actionFunc) {
                return true;
            }
        }

        if (func_801240DC(this)) {
            this->unk_AA6_rotFlags |= UNKAA6_ROT_FOCUS_X | UNKAA6_ROT_UPPER_X;
            return true;
        }

        if (!(this->stateFlags3 & PLAYER_STATE3_START_CHANGING_HELD_ITEM) &&
            (Player_UpperAction_ChangeHeldItem != this->upperActionFunc)) {
            // Process all entries in the Action Handler List with a positive index
            while (*actionHandlerList >= 0) {
                if (sActionHandlerFuncs[*actionHandlerList](this, play)) {
                    return true;
                }
                actionHandlerList++;
            }

            // Try the last entry in the list. Negate the index to make it positive again.
            if (sActionHandlerFuncs[-*actionHandlerList](this, play)) {
                return true;
            }
        }

        if (func_8083213C(this)) {
            return true;
        }
    } else if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        Player_UpdateUpperBody(this, play);
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
PlayerActionInterruptResult Player_TryActionInterrupt(PlayState* play, Player* this, SkelAnime* skelAnime,
                                                      f32 frameRange) {
    if ((skelAnime->endFrame - frameRange) <= skelAnime->curFrame) {
        f32 speedTarget;
        s16 yawTarget;

        if (Player_TryActionHandlerList(play, this, sActionHandlerListIdle, true)) {
            return PLAYER_INTERRUPT_NEW_ACTION;
        }

        if (sUpperBodyIsBusy ||
            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play)) {
            return PLAYER_INTERRUPT_MOVE;
        }
    }

    return PLAYER_INTERRUPT_NONE;
}

void func_808332A0(PlayState* play, Player* this, s32 magicCost, s32 isSwordBeam) {
    if (magicCost != 0) {
        this->unk_B08 = 0.0f;
    } else {
        this->unk_B08 = 0.5f;
    }

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;
    if ((this->actor.id == ACTOR_PLAYER) &&
        (isSwordBeam ||
         (GameInteractor_Should(VB_MAGIC_SPIN_ATTACK_CHECK_FORM, this->transformation == PLAYER_FORM_HUMAN)))) {
        s16 pitch = 0;
        Actor* thunder;

        if (isSwordBeam) {
            if (this->focusActor != NULL) {
                pitch = Math_Vec3f_Pitch(&this->bodyPartsPos[PLAYER_BODYPART_WAIST], &this->focusActor->focus.pos);
            }
            if (gSaveContext.save.saveInfo.playerData.magic == 0) {
                return;
            }
        }

        thunder = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_M_THUNDER, this->bodyPartsPos[PLAYER_BODYPART_WAIST].x,
                              this->bodyPartsPos[PLAYER_BODYPART_WAIST].y, this->bodyPartsPos[PLAYER_BODYPART_WAIST].z,
                              pitch, 0, 0, (this->heldItemAction - PLAYER_IA_SWORD_KOKIRI) | magicCost);

        if ((thunder != NULL) && isSwordBeam) {
            Magic_Consume(play, 1, MAGIC_CONSUME_DEITY_BEAM);
            this->unk_D57 = 4;
        }
    }
}

s32 Player_CanSpinAttack(Player* this) {
    s8 sp3C[ARRAY_COUNT(this->controlStickSpinAngles)];
    s8* iter;
    s8* iter2;
    s8 temp1;
    s8 temp2;
    s32 i;

    if (this->heldItemAction == PLAYER_IA_DEKU_STICK) {
        return false;
    }

    iter = &this->controlStickSpinAngles[0];
    iter2 = &sp3C[0];

    for (i = 0; i < ARRAY_COUNT(this->controlStickSpinAngles); i++, iter++, iter2++) {
        if ((*iter2 = *iter) < 0) {
            return false;
        }
        *iter2 *= 2;
    }

    temp1 = sp3C[0] - sp3C[1];

    if (ABS_ALT(temp1) < 10) {
        return false;
    }

    iter2 = &sp3C[1];

    for (i = 1; i < (ARRAY_COUNT(this->controlStickSpinAngles) - 1); i++, iter2++) {
        temp2 = *iter2 - *(iter2 + 1);
        if ((ABS_ALT(temp2) < 10) || (temp2 * temp1 < 0)) {
            return false;
        }
    }

    return true;
}

void func_808334D4(PlayState* play, Player* this) {
    PlayerAnimationHeader* anim;

    if ((this->meleeWeaponAnimation >= PLAYER_MWA_RIGHT_SLASH_1H) &&
        (this->meleeWeaponAnimation <= PLAYER_MWA_RIGHT_COMBO_2H)) {
        anim = D_8085CF58[Player_IsHoldingTwoHandedWeapon(this)];
    } else {
        anim = D_8085CF50[Player_IsHoldingTwoHandedWeapon(this)];
    }

    func_8082DC38(this);
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 8.0f, Animation_GetLastFrame(anim),
                           ANIMMODE_ONCE, -9.0f);
    func_808332A0(play, this, 2 << 8, false);
}

void func_808335B0(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_30, 1);
    func_808334D4(play, this);
}

s8 D_8085D090[] = {
    PLAYER_MWA_STAB_1H,        // PLAYER_STICK_DIR_FORWARD
    PLAYER_MWA_RIGHT_SLASH_1H, // PLAYER_STICK_DIR_LEFT, TODO: verify MWA as left/right does not match stick dir
    PLAYER_MWA_RIGHT_SLASH_1H, // PLAYER_STICK_DIR_BACKWARD
    PLAYER_MWA_LEFT_SLASH_1H,  // PLAYER_STICK_DIR_RIGHT
};

s8 D_8085D094[][3] = {
    { PLAYER_MWA_ZORA_PUNCH_LEFT, PLAYER_MWA_ZORA_PUNCH_COMBO, PLAYER_MWA_ZORA_PUNCH_KICK },
    { PLAYER_MWA_GORON_PUNCH_LEFT, PLAYER_MWA_GORON_PUNCH_RIGHT, PLAYER_MWA_GORON_PUNCH_BUTT },
};

PlayerMeleeWeaponAnimation func_808335F4(Player* this) {
    s32 controlStickDirection;
    PlayerMeleeWeaponAnimation meleeWeaponAnim;

    controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];
    if ((this->transformation == PLAYER_FORM_ZORA) || (this->transformation == PLAYER_FORM_GORON)) {
        s8* meleeWeaponAnims = (this->transformation == PLAYER_FORM_ZORA) ? D_8085D094[0] : D_8085D094[1];
        s32 unk_ADD = this->unk_ADD;

        meleeWeaponAnim = meleeWeaponAnims[unk_ADD];

        if (unk_ADD != 0) {
            this->meleeWeaponAnimation = meleeWeaponAnim;
            if (unk_ADD >= 2) {
                this->unk_ADD = -1;
            }
        }
    } else {
        if (Player_CanSpinAttack(this)) {
            meleeWeaponAnim = PLAYER_MWA_SPIN_ATTACK_1H;
        } else {
            if (controlStickDirection <= PLAYER_STICK_DIR_NONE) {
                meleeWeaponAnim = Player_IsZTargeting(this) ? PLAYER_MWA_FORWARD_SLASH_1H : PLAYER_MWA_RIGHT_SLASH_1H;
            } else {
                meleeWeaponAnim = D_8085D090[controlStickDirection];
                if (meleeWeaponAnim == PLAYER_MWA_STAB_1H) {
                    this->stateFlags2 |= PLAYER_STATE2_40000000;
                    if (!Player_IsZTargeting(this)) {
                        meleeWeaponAnim = PLAYER_MWA_FORWARD_SLASH_1H;
                    }
                }
            }

            if (this->heldItemAction == PLAYER_IA_DEKU_STICK) {
                meleeWeaponAnim = PLAYER_MWA_FORWARD_SLASH_1H;
            }
        }

        if (Player_IsHoldingTwoHandedWeapon(this)) {
            meleeWeaponAnim++;
        }
    }
    return meleeWeaponAnim;
}

void func_80833728(Player* this, s32 index, u32 dmgFlags, s32 damage) {
    this->meleeWeaponQuads[index].elem.atDmgInfo.dmgFlags = dmgFlags;
    this->meleeWeaponQuads[index].elem.atDmgInfo.damage = damage;

    if (dmgFlags == DMG_DEKU_STICK) {
        this->meleeWeaponQuads[index].elem.atElemFlags = (ATELEM_ON | ATELEM_NEAREST | ATELEM_SFX_WOOD);
    } else {
        this->meleeWeaponQuads[index].elem.atElemFlags = (ATELEM_ON | ATELEM_NEAREST);
    }
}

MeleeWeaponDamageInfo D_8085D09C[PLAYER_MELEEWEAPON_MAX] = {
    { DMG_GORON_PUNCH, 2, 2, 0, 0 }, // PLAYER_MELEEWEAPON_NONE
    { DMG_SWORD, 4, 8, 1, 2 },       // PLAYER_MELEEWEAPON_SWORD_KOKIRI
    { DMG_SWORD, 4, 8, 2, 4 },       // PLAYER_MELEEWEAPON_SWORD_RAZOR
    { DMG_SWORD, 4, 8, 3, 6 },       // PLAYER_MELEEWEAPON_SWORD_GILDED
    { DMG_SWORD, 4, 8, 4, 8 },       // PLAYER_MELEEWEAPON_SWORD_TWO_HANDED
    { DMG_DEKU_STICK, 0, 0, 2, 4 },  // PLAYER_MELEEWEAPON_DEKU_STICK
    { DMG_ZORA_PUNCH, 1, 2, 0, 0 },  // PLAYER_MELEEWEAPON_ZORA_BOOMERANG
};

// New function in NE0: split out of func_80833864 to be able to call it to patch Power Crouch Stab.
void func_8083375C(Player* this, PlayerMeleeWeaponAnimation meleeWeaponAnim) {
    MeleeWeaponDamageInfo* dmgInfo = &D_8085D09C[0];
    s32 damage;

    if (this->actor.id == ACTOR_EN_TEST3) {
        // Was Kafei originally intended to be able to punch?
        meleeWeaponAnim = PLAYER_MWA_GORON_PUNCH_LEFT;
        this->meleeWeaponAnimation = -1;
    } else {
        //! @bug Quick Put Away Damage: Since 0 is also the "no weapon" value, producing a weapon quad without a weapon
        //! in hand, such as during Quick Put Away, produced a quad with the Goron punch properties, which does 0 damage
        //! as human.
        dmgInfo = &D_8085D09C[(this->transformation == PLAYER_FORM_GORON) ? PLAYER_MELEEWEAPON_NONE
                                                                          : Player_GetMeleeWeaponHeld(this)];
    }

    //! @bug Great Deku Sword: Presumably the dmgTransformed fields are intended for Fierce Deity, but also work for
    //! Deku if it is able to equip a sword (such as with the "0th day" glitch), giving Great Fairy's Sword damage.
    damage =
        ((meleeWeaponAnim >= PLAYER_MWA_FLIPSLASH_START) && (meleeWeaponAnim <= PLAYER_MWA_ZORA_JUMPKICK_FINISH))
            ? ((this->transformation == PLAYER_FORM_HUMAN) ? dmgInfo->dmgHumanStrong : dmgInfo->dmgTransformedStrong)
            : ((this->transformation == PLAYER_FORM_HUMAN) ? dmgInfo->dmgHumanNormal : dmgInfo->dmgTransformedNormal);

    func_80833728(this, 0, dmgInfo->dmgFlags, damage);
    func_80833728(this, 1, dmgInfo->dmgFlags, damage);
}

void func_80833864(PlayState* play, Player* this, PlayerMeleeWeaponAnimation meleeWeaponAnim) {
    func_8083375C(this, meleeWeaponAnim);
    Player_SetAction(play, this, Player_Action_84, 0);
    this->av2.actionVar2 = 0;

    if ((meleeWeaponAnim < PLAYER_MWA_FLIPSLASH_FINISH) || (meleeWeaponAnim > PLAYER_MWA_ZORA_JUMPKICK_FINISH)) {
        func_8082DC38(this);
    }

    // Accumulate consecutive slashes to do the "third slash" types
    if ((meleeWeaponAnim != this->meleeWeaponAnimation) || (this->unk_ADD >= 3)) {
        this->unk_ADD = 0;
    }

    this->unk_ADD++;
    if (this->unk_ADD >= 3) {
        meleeWeaponAnim += 2;
    }

    this->meleeWeaponAnimation = meleeWeaponAnim;
    Player_Anim_PlayOnceAdjusted(play, this, sMeleeAttackAnimInfo[meleeWeaponAnim].unk_0);
    this->unk_ADC = this->skelAnime.animLength + 4.0f;

    if ((meleeWeaponAnim < PLAYER_MWA_FLIPSLASH_START) || (meleeWeaponAnim > PLAYER_MWA_ZORA_JUMPKICK_START)) {
        Player_AnimReplace_Setup(play, this, (ANIM_FLAG_1 | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE));
    }
    this->yaw = this->actor.shape.rot.y;
}

void func_80833998(Player* this, s32 invincibilityTimer) {
    if (this->invincibilityTimer >= 0) {
        this->invincibilityTimer = invincibilityTimer;
        this->unk_B5F = 0;
    }
}

void func_808339B4(Player* this, s32 invincibilityTimer) {
    if (this->invincibilityTimer > invincibilityTimer) {
        this->invincibilityTimer = invincibilityTimer;
    }
    this->unk_B5F = 0;
}

// Player_InflictDamageImpl?
s32 func_808339D4(PlayState* play, Player* this, s32 damage) {
    if ((this->invincibilityTimer != 0) || (this->stateFlags3 & PLAYER_STATE3_400000) ||
        (this->actor.id != ACTOR_PLAYER)) {
        return 1;
    }

    if (this->actor.category != ACTORCAT_PLAYER) {
        this->actor.colChkInfo.damage = -damage;
        return Actor_ApplyDamage(&this->actor);
    }

    if (GameInteractor_Should(VB_MULTIPLY_INFLICTED_DMG, this->currentMask == PLAYER_MASK_GIANT, &damage)) {
        damage >>= 2;
    }

    return Health_ChangeBy(play, damage);
}

void func_80833A64(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.jointTable[LIMB_ROOT_POS];
    Player_AnimReplace_SetupLedgeClimb(this, ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y);
}

void func_80833AA0(Player* this, PlayState* play) {
    if (Player_SetAction(play, this, Player_Action_25, 0)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
        this->av2.actionVar2 = 1;
    }
    if (this->unk_AA5 != PLAYER_UNKAA5_4) {
        this->unk_AA5 = PLAYER_UNKAA5_0;
    }
}

// TODO: this can be one array, but should it be?
PlayerAnimationHeader* D_8085D0D4[] = {
    &gPlayerAnim_link_normal_front_shit,
    &gPlayerAnim_link_normal_front_shitR,
    &gPlayerAnim_link_normal_back_shit,
    &gPlayerAnim_link_normal_back_shitR,
    // };
    // PlayerAnimationHeader* D_8085D0E4[] = {
    &gPlayerAnim_link_normal_front_hit,
    &gPlayerAnim_link_anchor_front_hitR,
    &gPlayerAnim_link_normal_back_hit,
    &gPlayerAnim_link_anchor_back_hitR,
};

void func_80833B18(PlayState* play, Player* this, s32 arg2, f32 speed, f32 velocityY, s16 arg5,
                   s32 invincibilityTimer) {
    PlayerAnimationHeader* anim = NULL;

    if (this->stateFlags1 & PLAYER_STATE1_2000) {
        func_80833A64(this);
    }

    this->unk_B64 = 0;

    Player_PlaySfx(this, NA_SE_PL_DAMAGE);

    if (func_808339D4(play, this, -this->actor.colChkInfo.damage) == 0) {
        this->stateFlags2 &= ~PLAYER_STATE2_80;
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->stateFlags1 & PLAYER_STATE1_8000000)) {
            return;
        }
    }

    if (this->actor.colChkInfo.damage != 0) {
        func_80833998(this, invincibilityTimer);
    }

    if (this->stateFlags2 & PLAYER_STATE2_10) {
        return;
    }

    if (arg2 == 3) {
        Player_SetAction(play, this, Player_Action_82, 0);
        anim = &gPlayerAnim_link_normal_ice_down;
        func_8082DAD4(this);
        this->actor.velocity.y = 0.0f;

        Player_RequestRumble(play, this, 255, 10, 40, SQ(0));

        Player_PlaySfx(this, NA_SE_PL_FREEZE_S);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FREEZE);
    } else if (arg2 == 4) {
        Player_SetAction(play, this, Player_Action_83, 0);
        Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_link_normal_electric_shock);
        func_8082DAD4(this);

        this->av2.actionVar2 = 20;
        this->actor.velocity.y = 0.0f;

        Player_RequestRumble(play, this, 255, 80, 150, SQ(0));
    } else {
        arg5 -= this->actor.shape.rot.y;

        if (this->stateFlags1 & PLAYER_STATE1_8000000) {
            Player_SetAction(play, this, Player_Action_61, 0);
            Player_RequestRumble(play, this, 180, 20, 50, SQ(0));

            if (arg2 == 1) {
                this->speedXZ = speed * 1.5f;
                this->actor.velocity.y = velocityY * 0.7f;
            } else {
                this->speedXZ = 4.0f;
                this->actor.velocity.y = 0.0f;
            }

            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);
            anim = &gPlayerAnim_link_swimer_swim_hit;
        } else if ((arg2 == 1) || (arg2 == 2) || !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                   (this->stateFlags1 &
                    (PLAYER_STATE1_4 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 | PLAYER_STATE1_200000))) {
            Player_SetAction(play, this, Player_Action_21, 0);

            this->stateFlags3 |= PLAYER_STATE3_2;

            Player_RequestRumble(play, this, 255, 20, 150, SQ(0));
            func_8082DAD4(this);

            if (arg2 == 2) {
                this->av2.actionVar2 = 4;

                this->actor.speed = 3.0f;
                this->speedXZ = 3.0f;
                this->actor.velocity.y = 6.0f;

                Player_Anim_PlayOnceFreeze(play, this, D_8085BE84[PLAYER_ANIMGROUP_damage_run][this->modelAnimType]);
                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);
            } else {
                this->actor.speed = speed;
                this->speedXZ = speed;
                this->actor.velocity.y = velocityY;

                if (ABS_ALT(arg5) > 0x4000) {
                    anim = &gPlayerAnim_link_normal_front_downA;
                } else {
                    anim = &gPlayerAnim_link_normal_back_downA;
                }
                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FALL_L);
            }
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        } else if ((this->speedXZ > 4.0f) && !Player_CheckHostileLockOn(this)) {
            this->unk_B64 = 20;

            Player_RequestRumble(play, this, 120, 20, 10, SQ(0));
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);

            return;
        } else {
            PlayerAnimationHeader** animPtr = D_8085D0D4;

            Player_SetAction(play, this, Player_Action_20, 0);
            func_8082FC60(this);

            if (this->actor.colChkInfo.damage < 5) {
                Player_RequestRumble(play, this, 120, 20, 10, SQ(0));
            } else {
                Player_RequestRumble(play, this, 180, 20, 100, SQ(0));
                this->speedXZ = 23.0f;

                animPtr += 4;
            }

            if (ABS_ALT(arg5) <= 0x4000) {
                animPtr += 2;
            }

            if (Player_CheckHostileLockOn(this)) {
                animPtr++;
            }

            anim = *animPtr;
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);
        }

        this->actor.shape.rot.y += arg5;
        this->yaw = this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;

        if (ABS_ALT(arg5) > 0x4000) {
            this->actor.shape.rot.y += 0x8000;
        }
    }

    func_8082DE50(play, this);

    this->stateFlags1 |= PLAYER_STATE1_4000000;

    if (anim != NULL) {
        Player_Anim_PlayOnceAdjusted(play, this, anim);
    }
}

s32 func_808340AC(FloorType floorType) {
    s32 temp_v0 = floorType - FLOOR_TYPE_2;

    if ((temp_v0 >= FLOOR_TYPE_2 - FLOOR_TYPE_2) && (temp_v0 <= FLOOR_TYPE_3 - FLOOR_TYPE_2)) {
        return temp_v0;
    }
    return -1;
}

bool func_808340D4(FloorType floorType) {
    return (floorType == FLOOR_TYPE_4) || (floorType == FLOOR_TYPE_7) || (floorType == FLOOR_TYPE_12);
}

void func_80834104(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_77, 0);
    this->stateFlags1 |= PLAYER_STATE1_20000000 | PLAYER_STATE1_80000000;
}

void func_80834140(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    if (!(this->stateFlags1 & PLAYER_STATE1_DEAD)) {
        func_80834104(play, this);
        if (func_8082DA90(play)) {
            this->av2.actionVar2 = -30;
        }
        this->stateFlags1 |= PLAYER_STATE1_DEAD;
        PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, 84.0f, ANIMMODE_ONCE,
                               -6.0f);
        this->av1.actionVar1 = 1;
        this->speedXZ = 0.0f;
    }
}

s32 Player_UpdateBodyBurn(PlayState* play, Player* this) {
    f32 temp_fv0;
    f32 flameScale;
    f32 flameIntensity;
    s32 i;
    s32 timerStep;
    s32 spawnedFlame = false;
    s32 var_v0;
    s32 var_v1;
    u8* timerPtr = this->bodyFlameTimers;

    if ((this->transformation == PLAYER_FORM_ZORA) || (this->transformation == PLAYER_FORM_DEKU)) {
        timerStep = 0;
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            if (this->cylinder.base.ocFlags1 & OC1_HIT) {
                Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
                this->speedXZ = 0.0f;
            }
            func_80834140(play, this, &gPlayerAnim_link_derth_rebirth);
        }
    } else {
        if (this->transformation == PLAYER_FORM_GORON) {
            var_v1 = 20;
        } else {
            var_v1 = (s32)(this->speedXZ * 0.4f) + 1;
        }

        if (this->stateFlags2 & PLAYER_STATE2_8) {
            var_v0 = 100;
        } else {
            var_v0 = 0;
        }

        timerStep = var_v0 + var_v1;
    }

    for (i = 0; i < PLAYER_BODYPART_MAX; i++, timerPtr++) {
        if (*timerPtr <= timerStep) {
            *timerPtr = 0;
        } else {
            spawnedFlame = true;
            *timerPtr -= timerStep;
            if (*timerPtr > 20.0f) {
                temp_fv0 = (*timerPtr - 20.0f) * 0.01f;
                flameScale = CLAMP(temp_fv0, 0.19999999f, 0.2f);
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
        if ((play->gameplayFrames % 4) == 0) {
            Player_InflictDamage(play, -1);
        }
    } else {
        this->bodyIsBurning = false;
    }

    return this->stateFlags1 & PLAYER_STATE1_DEAD;
}

s32 func_808344C0(PlayState* play, Player* this) {
    s32 i = 0;

    while (i < ARRAY_COUNT(this->bodyFlameTimers)) {
        this->bodyFlameTimers[i] = Rand_S16Offset(0, 200);
        i++;
    }

    this->bodyIsBurning = true;
    return Player_UpdateBodyBurn(play, this);
}

s32 func_80834534(PlayState* play, Player* this) {
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FALL_L);
    return func_808344C0(play, this);
}

s32 func_8083456C(PlayState* play, Player* this) {
    if (this->actor.colChkInfo.acHitEffect == 1) {
        return func_80834534(play, this);
    }
    return false;
}

void func_808345A8(Player* this) {
    if ((this->invincibilityTimer > 0) && (this->invincibilityTimer < 20)) {
        this->invincibilityTimer = 20;
    }
}

void func_808345C8(void) {
    if (INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU) {
        gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
        gSaveContext.save.equippedMask = PLAYER_MASK_NONE;
    }
}

s32 func_80834600(Player* this, PlayState* play) {
    s32 pad74;
    s32 var_v0;

    if (this->unk_D6A != 0) {
        if (!Player_InBlockingCsMode(play, this)) {
            Player_InflictDamage(play, -16);
            this->unk_D6A = 0;
        }
    } else if ((var_v0 = ((Player_GetHeight(this) - 8.0f) < (this->unk_AB8 * this->actor.scale.y))) ||
               (this->actor.bgCheckFlags & BGCHECKFLAG_CRUSHED) || (sPlayerFloorType == FLOOR_TYPE_9) ||
               (this->stateFlags2 & PLAYER_STATE2_80000000)) {
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);

        if (var_v0) {
            func_80169FDC(play);
            func_808345C8();
            Scene_SetExitFade(play);
        } else {
            func_80169EFC(play);
            func_808345C8();
        }

        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_TAKEN_AWAY);
        play->haltAllActors = true;
        Audio_PlaySfx(NA_SE_OC_ABYSS);
    } else if ((this->unk_B75 != 0) && ((this->unk_B75 >= 3) || (this->invincibilityTimer == 0))) {
        u8 sp6C[] = { 0, 2, 1, 1 };

        if (!func_8083456C(play, this)) {
            if (this->unk_B75 == 4) {
                this->bodyShockTimer = 40;
            }

            this->actor.colChkInfo.damage += this->unk_B74;
            func_80833B18(play, this, sp6C[this->unk_B75 - 1], this->unk_B78, this->unk_B7C, this->unk_B76, 20);
        }
    } else if ((this->shieldQuad.base.acFlags & AC_BOUNCED) || (this->shieldCylinder.base.acFlags & AC_BOUNCED) ||
               ((this->invincibilityTimer < 0) && (this->cylinder.base.acFlags & AC_HIT) &&
                (this->cylinder.elem.acHitElem != NULL) &&
                (this->cylinder.elem.acHitElem->atDmgInfo.dmgFlags != DMG_UNBLOCKABLE))) {
        PlayerAnimationHeader* var_a2;
        s32 sp64;

        Player_RequestRumble(play, this, 180, 20, 100, SQ(0));
        if ((this->invincibilityTimer >= 0) && !Player_IsGoronOrDeku(this)) {
            sp64 = (Player_Action_18 == this->actionFunc);
            if (!func_801242B4(this)) {
                Player_SetAction(play, this, Player_Action_19, 0);
            }

            this->av1.actionVar1 = sp64;
            if ((s8)sp64 == 0) {
                Player_SetUpperAction(play, this, Player_UpperAction_4);
                var_a2 = (this->unk_B40 < 0.5f) ? D_8085CFD4[Player_IsHoldingTwoHandedWeapon(this)]
                                                : D_8085CFCC[Player_IsHoldingTwoHandedWeapon(this)];
                PlayerAnimation_PlayOnce(play, &this->skelAnimeUpper, var_a2);
            } else {
                Player_Anim_PlayOnce(play, this, D_8085CFDC[Player_IsHoldingTwoHandedWeapon(this)]);
            }
        }

        if (!(this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 | PLAYER_STATE1_200000))) {
            this->speedXZ = -18.0f;
            this->yaw = this->actor.shape.rot.y;
        }

        return false;
    } else if ((this->unk_D6B != 0) || (this->invincibilityTimer > 0) || (this->stateFlags1 & PLAYER_STATE1_4000000) ||
               (this->csAction != PLAYER_CSACTION_NONE) || (this->meleeWeaponQuads[0].base.atFlags & AT_HIT) ||
               (this->meleeWeaponQuads[1].base.atFlags & AT_HIT) || (this->cylinder.base.atFlags & AT_HIT) ||
               (this->shieldCylinder.base.atFlags & AT_HIT)) {
        return false;
    } else if (this->cylinder.base.acFlags & AC_HIT) {
        Actor* sp60 = this->cylinder.base.ac;
        s32 var_a2_2;

        if (sp60->flags & ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT) {
            Player_PlaySfx(this, NA_SE_PL_BODY_HIT);
        }

        if (this->actor.colChkInfo.acHitEffect == 2) {
            var_a2_2 = 3;
        } else if (this->actor.colChkInfo.acHitEffect == 3) {
            var_a2_2 = 4;
        } else if (this->actor.colChkInfo.acHitEffect == 7) {
            var_a2_2 = 1;
            this->bodyShockTimer = 40;
        } else if (this->actor.colChkInfo.acHitEffect == 9) {
            var_a2_2 = 1;
            if (func_80834534(play, this)) {
                return true;
            }

        } else if (((this->actor.colChkInfo.acHitEffect == 4) && (this->currentMask != PLAYER_MASK_GIANT)) ||
                   (this->stateFlags3 & PLAYER_STATE3_1000)) {
            var_a2_2 = 1;
        } else {
            var_a2_2 = 0;
            if (func_8083456C(play, this)) {
                return true;
            }
        }
        func_80833B18(play, this, var_a2_2, 4.0f, 5.0f, Actor_WorldYawTowardActor(sp60, &this->actor), 20);
    } else if (this->invincibilityTimer != 0) {
        return false;
    } else {
        s32 sp58 = func_808340AC(sPlayerFloorType);
        u32 isSurfaceWallDamage = SurfaceType_IsWallDamage(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
        s32 var_a1 = false;
        s32 var_v1_2;
        s32 pad48;

        if ((sp58 < 0) || (!isSurfaceWallDamage && (this->transformation == PLAYER_FORM_GORON) &&
                           !(this->actor.depthInWater > 0.0f))) {
            var_a1 = (this->actor.wallPoly != NULL) &&
                     SurfaceType_IsWallDamage(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId);
            if (!var_a1) {
                //! FAKE?
                goto label;
            }
        }
        var_v1_2 = var_a1 ? this->actor.wallBgId : this->actor.floorBgId;
        if (((this->transformation == PLAYER_FORM_DEKU) || (this->transformation == PLAYER_FORM_ZORA)) &&
            ((sp58 >= 0) && !isSurfaceWallDamage && !(this->stateFlags1 & PLAYER_STATE1_8000000) &&
             (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.depthInWater < -30.0f))) {
            func_80834534(play, this);
        } else {
            this->actor.colChkInfo.damage = 4;
            func_80833B18(play, this, (var_v1_2 == BGCHECK_SCENE) ? 0 : 1, 4.0f, 5.0f,
                          var_a1 ? this->actor.wallYaw : this->actor.shape.rot.y, 20);
            return true;
        }
    }

    //! FAKE?
    if (0) {
    label:
        return false;
    }

    return true;
}

void func_80834CD0(Player* this, f32 arg1, u16 sfxId) {
    this->actor.velocity.y = arg1 * sWaterSpeedFactor;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;

    if (sfxId != NA_SE_NONE) {
        Player_AnimSfx_PlayFloorJump(this);
        Player_AnimSfx_PlayVoice(this, sfxId);
    }

    this->stateFlags1 |= PLAYER_STATE1_40000;
    this->fallStartHeight = this->actor.world.pos.y;
}

void func_80834D50(PlayState* play, Player* this, PlayerAnimationHeader* anim, f32 speed, u16 sfxId) {
    Player_SetAction(play, this, Player_Action_25, 1);
    if (anim != NULL) {
        Player_Anim_PlayOnceAdjusted(play, this, anim);
    }
    func_80834CD0(this, speed, sfxId);
}

void func_80834DB8(Player* this, PlayerAnimationHeader* anim, f32 speed, PlayState* play) {
    func_80834D50(play, this, anim, speed, NA_SE_VO_LI_SWORD_N);
}

s32 Player_ActionHandler_12(Player* this, PlayState* play) {
    if ((this->transformation != PLAYER_FORM_GORON) &&
        ((this->transformation != PLAYER_FORM_DEKU) || func_801242B4(this) ||
         (this->ledgeClimbType <= PLAYER_LEDGE_CLIMB_3)) &&
        !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->ledgeClimbType >= PLAYER_LEDGE_CLIMB_2) &&
        (!(this->stateFlags1 & PLAYER_STATE1_8000000) || (this->yDistToLedge < this->ageProperties->unk_14))) {
        s32 var_v1 = false;
        PlayerAnimationHeader* anim;
        f32 yDistToLedge;

        if (func_801242B4(this)) {
            f32 depth = (this->transformation == PLAYER_FORM_FIERCE_DEITY) ? 80.0f : 50.0f;

            if (this->actor.depthInWater < depth) {
                if ((this->ledgeClimbType <= PLAYER_LEDGE_CLIMB_1) ||
                    (this->ageProperties->unk_10 < this->yDistToLedge)) {
                    return false;
                }
            } else if ((this->currentBoots < PLAYER_BOOTS_ZORA_UNDERWATER) ||
                       (this->ledgeClimbType >= PLAYER_LEDGE_CLIMB_3)) {
                return false;
            }
        } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                   ((this->ageProperties->unk_14 <= this->yDistToLedge) &&
                    (this->stateFlags1 & PLAYER_STATE1_8000000))) {
            return false;
        }

        if ((this->actor.wallBgId != BGCHECK_SCENE) && (sPlayerTouchedWallFlags & WALL_FLAG_6)) {
            if (this->ledgeClimbDelayTimer >= 6) {
                this->stateFlags2 |= PLAYER_STATE2_4;
                if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A)) {
                    var_v1 = true;
                }
            }
        } else if ((this->ledgeClimbDelayTimer >= 6) || CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A)) {
            var_v1 = true;
        }

        if (var_v1) {
            Player_SetAction(play, this, Player_Action_33, 0);
            yDistToLedge = this->yDistToLedge;

            if (this->ageProperties->unk_14 <= yDistToLedge) {
                anim = &gPlayerAnim_link_normal_250jump_start;
                this->speedXZ = 1.0f;
            } else {
                CollisionPoly* poly;
                s32 bgId;
                f32 wallPolyNormalX = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.x);
                f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.z);
                f32 var_fv1 = this->distToInteractWall + 0.5f;
                f32 yIntersect;
                s32 pad;

                this->stateFlags1 |= PLAYER_STATE1_4;

                if (func_801242B4(this)) {
                    yDistToLedge -= 60.0f * this->ageProperties->unk_08;
                    anim = &gPlayerAnim_link_swimer_swim_15step_up;
                    this->stateFlags1 &= ~PLAYER_STATE1_8000000;
                } else if (this->ageProperties->unk_18 <= yDistToLedge) {
                    yDistToLedge -= 59.0f * this->ageProperties->unk_08;
                    anim = &gPlayerAnim_link_normal_150step_up;
                } else {
                    yDistToLedge -= 41.0f * this->ageProperties->unk_08;
                    anim = &gPlayerAnim_link_normal_100step_up;
                }

                this->unk_ABC -= yDistToLedge * 100.0f;

                this->actor.world.pos.x -= var_fv1 * wallPolyNormalX;
                this->actor.world.pos.y += this->yDistToLedge + 10.0f;
                this->actor.world.pos.z -= var_fv1 * wallPolyNormalZ;

                yIntersect =
                    BgCheck_EntityRaycastFloor5(&play->colCtx, &poly, &bgId, &this->actor, &this->actor.world.pos);
                if ((this->actor.world.pos.y - yIntersect) <= 20.0f) {
                    this->actor.world.pos.y = yIntersect;
                    if (bgId != BGCHECK_SCENE) {
                        DynaPoly_SetPlayerOnTop(&play->colCtx, bgId);
                    }
                }

                func_8082DAD4(this);
                this->actor.velocity.y = 0.0f;
            }

            this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
            PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, 1.3f);
            AnimTaskQueue_DisableTransformTasksForGroup(play);
            this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
            return true;
        }
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->ledgeClimbType == PLAYER_LEDGE_CLIMB_1) &&
               (this->ledgeClimbDelayTimer >= 3)) {
        f32 temp = (this->yDistToLedge * 0.08f) + 5.5f;

        func_80834DB8(this, &gPlayerAnim_link_normal_jump, temp, play);
        this->speedXZ = 2.5f;
        return true;
    }

    return false;
}

void func_80835324(PlayState* play, Player* this, f32 arg2, s16 arg3) {
    Player_SetAction(play, this, Player_Action_35, 0);
    func_8082DD2C(play, this);

    this->csId = CS_ID_NONE;
    this->av1.actionVar1 = 1;
    this->av2.actionVar2 = 1;

    this->unk_3A0.x = this->actor.world.pos.x + Math_SinS(arg3) * arg2;
    this->unk_3A0.z = this->actor.world.pos.z + Math_CosS(arg3) * arg2;

    Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
}

void func_808353DC(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_54, 0);
    Player_Anim_PlayLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
}

s32 func_80835428(PlayState* play, Player* this) {
    if (!func_8082DA90(play) && (this->stateFlags1 & PLAYER_STATE1_80000000)) {
        func_80834104(play, this);
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FALL_S);
        Audio_PlaySfx_2(NA_SE_OC_SECRET_WARP_IN);
        return true;
    }
    return false;
}

/**
 * The actual entrances each "return entrance" value can map to.
 * This is used by scenes that are shared between locations.
 *
 * This 1D array is split into groups of entrances.
 * The start of each group is indexed by `sReturnEntranceGroupIndices` values.
 * The resulting groups are then indexed by the spawn value.
 *
 * The spawn value (`PlayState.curSpawn`) is set to a different value depending on the entrance used to enter the
 * scene, which allows these dynamic "return entrances" to link back to the previous scene.
 *
 * Seems unused in MM
 */
u16 sReturnEntranceGroupData[] = {
    // 0xFE00
    /* 0 */ 0x1000,
};

/**
 * The values are indices into `sReturnEntranceGroupData` marking the start of each group
 */
u8 sReturnEntranceGroupIndices[] = {
    0, // 0xFE00
};

// subfunction of OoT's func_80839034
void func_808354A4(PlayState* play, s32 exitIndex, s32 arg2) {
    play->nextEntrance = play->setupExitList[exitIndex];

    if (play->nextEntrance == 0xFFFF) {
        gSaveContext.respawnFlag = 4;
        play->nextEntrance = gSaveContext.respawn[RESPAWN_MODE_UNK_3].entrance;
        play->transitionType = TRANS_TYPE_FADE_WHITE;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
    } else if (play->nextEntrance >= 0xFE00) {
        play->nextEntrance =
            sReturnEntranceGroupData[sReturnEntranceGroupIndices[play->nextEntrance - 0xFE00] + play->curSpawn];

        Scene_SetExitFade(play);
    } else {
        if (arg2) {
            gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = play->nextEntrance;
            func_80169EFC(play);
            gSaveContext.respawnFlag = -2;
        }

        gSaveContext.retainWeatherMode = true;
        Scene_SetExitFade(play);
    }

    play->transitionTrigger = TRANS_TRIGGER_START;
}

void func_808355D8(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    func_80833AA0(this, play);
    this->av2.actionVar2 = -2;
    Player_Anim_PlayOnceFreezeAdjusted(play, this, anim);
    func_8082E1F0(this, NA_SE_IT_DEKUNUTS_FLOWER_CLOSE);
}

s32 Player_HandleExitsAndVoids(PlayState* play, Player* this, CollisionPoly* poly, s32 bgId) {
    s32 exitIndexPlusOne;
    FloorType floorType;
    s32 sp34;
    s32 sp30;

    if ((this == GET_PLAYER(play)) && !(this->stateFlags1 & PLAYER_STATE1_DEAD) && !func_8082DA90(play) &&
        (this->csAction == PLAYER_CSACTION_NONE) && !(this->stateFlags1 & PLAYER_STATE1_1)) {
        exitIndexPlusOne = 0;

        if (((poly != NULL) &&
             (exitIndexPlusOne = SurfaceType_GetSceneExitIndex(&play->colCtx, poly, bgId), (exitIndexPlusOne != 0)) &&
             (((play->sceneId != SCENE_GORONRACE) && (play->sceneId != SCENE_DEKU_KING)) || (exitIndexPlusOne < 3)) &&
             (((play->sceneId != SCENE_20SICHITAI) && (play->sceneId != SCENE_20SICHITAI2)) ||
              (exitIndexPlusOne < 0x15)) &&
             ((play->sceneId != SCENE_11GORONNOSATO) || (exitIndexPlusOne < 6))) ||
            (func_808340D4(sPlayerFloorType) && (this->floorProperty == FLOOR_PROPERTY_12))) {

            sp34 = this->unk_D68 - (s32)this->actor.world.pos.y;

            if (!(this->stateFlags1 & (PLAYER_STATE1_800000 | PLAYER_STATE1_8000000 | PLAYER_STATE1_20000000)) &&
                !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (sp34 < 400) && (sPlayerYDistToFloor > 100.0f)) {
                if ((this->floorProperty != FLOOR_PROPERTY_5) && (this->floorProperty != FLOOR_PROPERTY_12)) {
                    this->speedXZ = 0.0f;
                }
                return false;
            }

            if (this->stateFlags3 & PLAYER_STATE3_1000000) {
                func_808355D8(play, this, &gPlayerAnim_pn_kakkufinish);
            }

            if (exitIndexPlusOne == 0) {
                func_80169EFC(play);
                Scene_SetExitFade(play);
            } else {
                func_808354A4(play, exitIndexPlusOne - 1,
                              SurfaceType_GetFloorEffect(&play->colCtx, poly, bgId) == FLOOR_EFFECT_2);

                if ((this->stateFlags1 & PLAYER_STATE1_8000000) && (this->floorProperty == FLOOR_PROPERTY_5)) {
                    Audio_PlaySfx_2(NA_SE_OC_TUNAMI);
                    Audio_MuteAllSeqExceptSystemAndOcarina(5);
                    gSaveContext.seqId = NA_BGM_DISABLED;
                    gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
                } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
                           (this->floorProperty == FLOOR_PROPERTY_12)) {
                    Audio_PlaySfx_2(NA_SE_OC_SECRET_WARP_IN);
                }

                if (this->stateFlags1 & PLAYER_STATE1_800000) {
                    if (D_801BDAA0) {
                        D_801BDAA0 = false;
                    } else {
                        gHorseIsMounted = true;
                    }
                }
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_800000 | PLAYER_STATE1_8000000 | PLAYER_STATE1_20000000)) &&
                ((floorType = SurfaceType_GetFloorType(&play->colCtx, poly, bgId)) != FLOOR_TYPE_10) &&
                ((sp34 < 100) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
                if (floorType == FLOOR_TYPE_11) {
                    Audio_PlaySfx_2(NA_SE_OC_SECRET_HOLE_OUT);
                    Audio_MuteAllSeqExceptSystemAndOcarina(5);
                    gSaveContext.seqId = NA_BGM_DISABLED;
                    gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
                } else {
                    func_8085B74C(play);
                }
            } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                Player_StopHorizontalMovement(this);
            }

            Camera_ChangeSetting(Play_GetCamera(play, CAM_ID_MAIN), CAM_SET_SCENE0);
            this->stateFlags1 |= PLAYER_STATE1_1 | PLAYER_STATE1_20000000;
            return true;
        }
        if ((this->stateFlags1 & PLAYER_STATE1_8000000) && (this->actor.floorPoly == NULL)) {
            BgCheck_EntityRaycastFloor7(&play->colCtx, &this->actor.floorPoly, &sp30, &this->actor,
                                        &this->actor.world.pos);
            if (this->actor.floorPoly == NULL) {
                func_80169EFC(play);
                return false;
            }
            //! FAKE
            if (1) {}
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_80000000)) {
            if (((this->actor.world.pos.y < -4000.0f) ||
                 (((this->floorProperty == FLOOR_PROPERTY_5) || (this->floorProperty == FLOOR_PROPERTY_12) ||
                   (this->floorProperty == FLOOR_PROPERTY_13)) &&
                  ((sPlayerYDistToFloor < 100.0f) || (this->fallDistance > 400))))) {
                if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                    if (this->floorProperty == FLOOR_PROPERTY_5) {
                        func_80169FDC(play);
                        func_808345C8();
                    } else {
                        func_80169EFC(play);
                    }
                    if (!SurfaceType_IsWallDamage(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId)) {
                        gSaveContext.respawnFlag = -5;
                    }

                    play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
                    Audio_PlaySfx(NA_SE_OC_ABYSS);
                } else {
                    if (this->stateFlags3 & PLAYER_STATE3_1000000) {
                        func_808355D8(play, this, &gPlayerAnim_pn_kakkufinish);
                    }

                    if (this->floorProperty == FLOOR_PROPERTY_13) {
                        Player_SetAction(play, this, Player_Action_1, 0);
                        this->stateFlags1 |= PLAYER_STATE1_20000000;
                    } else {
                        func_80834104(play, this);
                        this->av2.actionVar2 = 9999;
                        if (this->floorProperty == FLOOR_PROPERTY_5) {
                            this->av1.actionVar1 = -1;
                        } else {
                            this->av1.actionVar1 = 1;
                        }
                    }
                }
            }
        }

        this->unk_D68 = this->actor.world.pos.y;
    }

    return false;
}

/**
 * Gets a position relative to player's yaw.
 * An offset is applied to the provided base position in the direction of shape y rotation.
 * The resulting position is stored in `dst`
 */
void Player_TranslateAndRotateY(Player* this, Vec3f* translation, Vec3f* src, Vec3f* dst) {
    Lib_Vec3f_TranslateAndRotateY(translation, this->actor.shape.rot.y, src, dst);
}

// Player_GetPosInACertainDirectionFromARadiusAway
void func_80835BF8(Vec3f* srcPos, s16 rotY, f32 radius, Vec3f* dstPos) {
    dstPos->x = Math_SinS(rotY) * radius + srcPos->x;
    dstPos->z = Math_CosS(rotY) * radius + srcPos->z;
}

Actor* Player_SpawnFairy(PlayState* play, Player* this, Vec3f* translation, Vec3f* pos, s32 fairyParams) {
    Vec3f spawnPos;

    Player_TranslateAndRotateY(this, translation, pos, &spawnPos);

    return Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, spawnPos.x, spawnPos.y, spawnPos.z, 0, 0, 0, fairyParams);
}

f32 func_80835CD8(PlayState* play, Player* this, Vec3f* arg2, Vec3f* pos, CollisionPoly** outPoly, s32* outBgId) {
    Player_TranslateAndRotateY(this, &this->actor.world.pos, arg2, pos);

    return BgCheck_EntityRaycastFloor5(&play->colCtx, outPoly, outBgId, &this->actor, pos);
}

f32 func_80835D2C(PlayState* play, Player* this, Vec3f* arg2, Vec3f* pos) {
    CollisionPoly* poly;
    s32 bgId;

    return func_80835CD8(play, this, arg2, pos, &poly, &bgId);
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

    Player_TranslateAndRotateY(this, &this->actor.world.pos, offset, &posB);

    return BgCheck_EntityLineTest2(&play->colCtx, &posA, &posB, posResult, wallPoly, true, false, false, true, bgId,
                                   &this->actor);
}

Vec3f D_8085D100 = { 0.0f, 50.0f, 0.0f };

s32 func_80835DF8(PlayState* play, Player* this, CollisionPoly** outPoly, s32* outBgId) {
    Vec3f pos;
    f32 yIntersect = func_80835CD8(play, this, &D_8085D100, &pos, outPoly, outBgId);

    if ((*outBgId == BGCHECK_SCENE) && (fabsf(this->actor.world.pos.y - yIntersect) < 10.0f)) {
        Environment_ChangeLightSetting(play, SurfaceType_GetLightSettingIndex(&play->colCtx, *outPoly, *outBgId));
        return true;
    }
    return false;
}

/**
 * PLAYER_DOORTYPE_STAIRCASE: DoorSpiral
 */
void Player_Door_Staircase(PlayState* play, Player* this, Actor* door) {
    static Vec3f D_8085D10C = { 20.0f, 0.0f, 20.0f };
    DoorSpiral* doorStaircase = (DoorSpiral*)door;

    this->yaw = doorStaircase->actor.home.rot.y + 0x8000;
    this->actor.shape.rot.y = this->yaw;
    if (this->speedXZ <= 0.0f) {
        this->speedXZ = 0.1f;
    }
    func_80835324(play, this, 50.0f, this->actor.shape.rot.y);

    this->unk_397 = this->doorType;
    this->av1.actionVar1 = 0;
    this->stateFlags1 |= PLAYER_STATE1_20000000;
    func_80835BF8(&doorStaircase->actor.world.pos, doorStaircase->actor.shape.rot.y, -140.0f, &this->unk_3A0);

    D_8085D10C.x = (this->doorDirection != 0) ? -400.0f : 400.0f;
    D_8085D10C.z = 200.0f;
    Player_TranslateAndRotateY(this, &this->unk_3A0, &D_8085D10C, &this->unk_3AC);

    doorStaircase->shouldClimb = true;

    func_8082DAD4(this);

    if (this->doorTimer != 0) {
        this->av2.actionVar2 = 0;
        Player_Anim_PlayOnceMorph(play, this, Player_GetIdleAnim(this));
        this->skelAnime.endFrame = 0.0f;
    } else {
        this->speedXZ = 0.1f;
    }

    Camera_ChangeSetting(Play_GetCamera(play, CAM_ID_MAIN), CAM_SET_SCENE0);
    this->cv.doorBgCamIndex =
        play->transitionActors.list[DOOR_GET_TRANSITION_ID(&doorStaircase->actor)].sides[0].bgCamIndex;
    Actor_DeactivateLens(play);
    this->floorSfxOffset = NA_SE_PL_WALK_CONCRETE - SFX_FLAG;
}

/**
 * PLAYER_DOORTYPE_SLIDING: DoorShutter, BgOpenShutter
 */
void Player_Door_Sliding(PlayState* play, Player* this, Actor* door) {
    SlidingDoorActor* doorSliding = (SlidingDoorActor*)door;
    Vec3f sp38;

    this->yaw = doorSliding->dyna.actor.home.rot.y;
    if (this->doorDirection > 0) {
        this->yaw -= 0x8000;
    }
    this->actor.shape.rot.y = this->yaw;
    if (this->speedXZ <= 0.0f) {
        this->speedXZ = 0.1f;
    }

    func_80835324(play, this, 50.0f, this->actor.shape.rot.y);
    this->av1.actionVar1 = 0;
    this->unk_397 = this->doorType;
    this->stateFlags1 |= PLAYER_STATE1_20000000;
    Actor_WorldToActorCoords(&doorSliding->dyna.actor, &sp38, &this->actor.world.pos);

    func_80835BF8(&this->actor.world.pos, doorSliding->dyna.actor.shape.rot.y,
                  (42.0f - fabsf(sp38.z)) * this->doorDirection, &this->actor.world.pos);
    func_80835BF8(&this->actor.world.pos, doorSliding->dyna.actor.shape.rot.y, this->doorDirection * 20.0f,
                  &this->unk_3A0);
    func_80835BF8(&this->actor.world.pos, doorSliding->dyna.actor.shape.rot.y, this->doorDirection * -120.0f,
                  &this->unk_3AC);

    doorSliding->unk_15C = 1;
    func_8082DAD4(this);

    if (this->doorTimer != 0) {
        this->av2.actionVar2 = 0;
        Player_Anim_PlayOnceMorph(play, this, Player_GetIdleAnim(this));
        this->skelAnime.endFrame = 0.0f;
    } else {
        this->speedXZ = 0.1f;
    }

    if (doorSliding->dyna.actor.category == ACTORCAT_DOOR) {
        this->cv.doorBgCamIndex = play->transitionActors.list[DOOR_GET_TRANSITION_ID(&doorSliding->dyna.actor)]
                                      .sides[this->doorDirection > 0 ? 0 : 1]
                                      .bgCamIndex;
        Actor_DeactivateLens(play);
    }
}

// sPlayerOpenDoorLeftAnimPerForm
PlayerAnimationHeader* D_8085D118[] = {
    &gPlayerAnim_pg_doorA_open, // PLAYER_FORM_GORON
    &gPlayerAnim_pz_doorA_open, // PLAYER_FORM_ZORA
    &gPlayerAnim_pn_doorA_open, // PLAYER_FORM_DEKU
};
// sPlayerOpenDoorRightAnimPerForm
PlayerAnimationHeader* D_8085D124[] = {
    &gPlayerAnim_pg_doorB_open, // PLAYER_FORM_GORON
    &gPlayerAnim_pz_doorB_open, // PLAYER_FORM_ZORA
    &gPlayerAnim_pn_doorB_open, // PLAYER_FORM_DEKU
};

/**
 * PLAYER_DOORTYPE_TALKING: EnDoorEtc
 * PLAYER_DOORTYPE_HANDLE: EnDoor
 * PLAYER_DOORTYPE_FAKE:
 * PLAYER_DOORTYPE_PROXIMITY: EnDoor
 */
void Player_Door_Knob(PlayState* play, Player* this, Actor* door) {
    s32 temp = this->transformation - 1;
    PlayerAnimationHeader* anim;
    f32 temp_fv0; // sp5C
    KnobDoorActor* knobDoor = (KnobDoorActor*)door;

    knobDoor->animIndex = this->transformation;

    if (this->doorDirection < 0) {
        if (this->transformation == PLAYER_FORM_FIERCE_DEITY) {
            anim = D_8085BE84[PLAYER_ANIMGROUP_doorA_free][this->modelAnimType];
        } else if (this->transformation == PLAYER_FORM_HUMAN) {
            anim = D_8085BE84[PLAYER_ANIMGROUP_doorA][this->modelAnimType];
        } else {
            anim = D_8085D118[temp];
        }
    } else {
        knobDoor->animIndex += PLAYER_FORM_MAX;

        if (this->transformation == PLAYER_FORM_FIERCE_DEITY) {
            anim = D_8085BE84[PLAYER_ANIMGROUP_doorB_free][this->modelAnimType];
        } else if (this->transformation == PLAYER_FORM_HUMAN) {
            anim = D_8085BE84[PLAYER_ANIMGROUP_doorB][this->modelAnimType];
        } else {
            anim = D_8085D124[temp];
        }
    }

    Player_SetAction(play, this, Player_Action_36, 0);
    this->stateFlags2 |= PLAYER_STATE2_800000;
    Player_PutAwayHeldItem(play, this);
    if (this->doorDirection < 0) {
        this->actor.shape.rot.y = knobDoor->dyna.actor.shape.rot.y;
    } else {
        this->actor.shape.rot.y = knobDoor->dyna.actor.shape.rot.y - 0x8000;
    }

    this->yaw = this->actor.shape.rot.y;
    temp_fv0 = this->doorDirection * 22.0f;
    func_80835BF8(&knobDoor->dyna.actor.world.pos, knobDoor->dyna.actor.shape.rot.y, temp_fv0, &this->actor.world.pos);
    Player_Anim_PlayOnceWaterAdjustment(play, this, anim);

    if (this->doorTimer != 0) {
        this->skelAnime.endFrame = 0.0f;
    }

    func_8082DAD4(this);
    Player_AnimReplace_Setup(play, this,
                             ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_4 | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_80 |
                                 ANIM_FLAG_200);
    knobDoor->requestOpen = true;
    if (this->doorType != PLAYER_DOORTYPE_FAKE) {
        CollisionPoly* poly;
        s32 bgId;
        Vec3f pos;
        EnDoorType enDoorType = ENDOOR_GET_TYPE(&knobDoor->dyna.actor);

        this->stateFlags1 |= PLAYER_STATE1_20000000;

        if (this->actor.category == ACTORCAT_PLAYER) {
            Actor_DeactivateLens(play);
            func_80835BF8(&knobDoor->dyna.actor.world.pos, knobDoor->dyna.actor.shape.rot.y, -temp_fv0, &pos);
            pos.y = knobDoor->dyna.actor.world.pos.y + 10.0f;
            BgCheck_EntityRaycastFloor5(&play->colCtx, &poly, &bgId, &this->actor, &pos);

            if (Player_HandleExitsAndVoids(play, this, poly, BGCHECK_SCENE)) {
                gSaveContext.entranceSpeed = 2.0f;
            } else if (enDoorType != ENDOOR_TYPE_FRAMED) {
                Camera* mainCam;

                this->av1.actionVar1 = 38.0f * sInvWaterSpeedFactor;
                mainCam = Play_GetCamera(play, CAM_ID_MAIN);

                Camera_ChangeDoorCam(mainCam, &knobDoor->dyna.actor,
                                     play->transitionActors.list[DOOR_GET_TRANSITION_ID(&knobDoor->dyna.actor)]
                                         .sides[(this->doorDirection > 0) ? 0 : 1]
                                         .bgCamIndex,
                                     0.0f, this->av1.actionVar1, 26.0f * sInvWaterSpeedFactor,
                                     10.0f * sInvWaterSpeedFactor);
            }
        }
    }
}

// door stuff
s32 Player_ActionHandler_1(Player* this, PlayState* play) {
    if ((gSaveContext.save.saveInfo.playerData.health != 0) && (this->doorType != PLAYER_DOORTYPE_NONE)) {
        if ((this->actor.category != ACTORCAT_PLAYER) ||
            ((((this->doorType <= PLAYER_DOORTYPE_TALKING) && CutsceneManager_IsNext(CS_ID_GLOBAL_TALK)) ||
              ((this->doorType >= PLAYER_DOORTYPE_HANDLE) && CutsceneManager_IsNext(CS_ID_GLOBAL_DOOR))) &&
             (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) &&
              (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A) ||
               (Player_Action_TryOpeningDoor == this->actionFunc) || (this->doorType == PLAYER_DOORTYPE_STAIRCASE) ||
               (this->doorType == PLAYER_DOORTYPE_PROXIMITY))))) {
            Actor* doorActor = this->doorActor;
            Actor* var_v0_3;

            if (this->doorType <= PLAYER_DOORTYPE_TALKING) {
                Player_StartTalking(play, doorActor);
                if (doorActor->textId == 0x1821) {
                    doorActor->flags |= ACTOR_FLAG_TALK;
                }
                return true;
            }

            gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 0;

            if (this->doorType == PLAYER_DOORTYPE_STAIRCASE) {
                Player_Door_Staircase(play, this, doorActor);
            } else if (this->doorType == PLAYER_DOORTYPE_SLIDING) {
                Player_Door_Sliding(play, this, doorActor);
            } else {
                Player_Door_Knob(play, this, doorActor);
            }

            if (this->actor.category == ACTORCAT_PLAYER) {
                this->csId = CS_ID_GLOBAL_DOOR;
                CutsceneManager_Start(this->csId, &this->actor);
            }

            if (this->actor.category == ACTORCAT_PLAYER) {
                if ((this->doorType < PLAYER_DOORTYPE_FAKE) && (doorActor->category == ACTORCAT_DOOR) &&
                    ((this->doorType != PLAYER_DOORTYPE_HANDLE) ||
                     (ENDOOR_GET_TYPE(doorActor) != ENDOOR_TYPE_FRAMED))) {
                    s8 roomNum = play->transitionActors.list[DOOR_GET_TRANSITION_ID(doorActor)]
                                     .sides[(this->doorDirection > 0) ? 0 : 1]
                                     .room;

                    if ((roomNum >= 0) && (roomNum != play->roomCtx.curRoom.num)) {
                        Room_RequestNewRoom(play, &play->roomCtx, roomNum);
                    }
                }
            }

            doorActor->room = play->roomCtx.curRoom.num;
            if (((var_v0_3 = doorActor->child) != NULL) || ((var_v0_3 = doorActor->parent) != NULL)) {
                var_v0_3->room = play->roomCtx.curRoom.num;
            }
            return true;
        }
    }

    return false;
}

void func_80836888(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;

    Player_SetAction(play, this, Player_Action_2, 1);

    if (this->unk_B40 < 0.5f) {
        anim = func_8082EF54(this);
        this->unk_B40 = 0.0f;
    } else {
        anim = func_8082EF9C(this);
        this->unk_B40 = 1.0f;
    }

    this->unk_B44 = this->unk_B40;
    Player_Anim_PlayLoop(play, this, anim);
    this->yaw = this->actor.shape.rot.y;
}

void func_8083692C(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_3, 1);
    Player_Anim_PlayOnceMorph(play, this, Player_GetIdleAnim(this));
    this->yaw = this->actor.shape.rot.y;
}

void func_80836988(Player* this, PlayState* play) {
    if (Player_CheckHostileLockOn(this)) {
        func_80836888(this, play);
    } else if (Player_FriendlyLockOnOrParallel(this)) {
        func_8083692C(this, play);
    } else {
        func_8085B384(this, play);
    }
}

void func_808369F4(Player* this, PlayState* play) {
    PlayerActionFunc actionFunc;

    if (Player_CheckHostileLockOn(this)) {
        actionFunc = Player_Action_2;
    } else if (Player_FriendlyLockOnOrParallel(this)) {
        actionFunc = Player_Action_3;
    } else {
        actionFunc = Player_Action_Idle;
    }
    Player_SetAction(play, this, actionFunc, 1);
}

void func_80836A5C(Player* this, PlayState* play) {
    func_808369F4(this, play);
    if (Player_CheckHostileLockOn(this)) {
        this->av2.actionVar2 = 1;
    }
}

void func_80836A98(Player* this, PlayerAnimationHeader* anim, PlayState* play) {
    func_80836A5C(this, play);
    Player_Anim_PlayOnceWaterAdjustment(play, this, anim);
}

void func_80836AD8(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_96, 0);
    this->unk_B28 = 0;
    this->unk_B86[1] = 0;
    this->unk_AF0[0].x = 0.0f;
    this->unk_AF0[0].y = 0.0f;
    this->unk_AF0[0].z = 0.0f;
    this->unk_B08 = 0.0f;
    this->unk_B0C = 0.0f;
    Player_PlaySfx(this, NA_SE_PL_GORON_TO_BALL);
}

void func_80836B3C(PlayState* play, Player* this, f32 arg2) {
    if (GameInteractor_Should(VB_PATCH_SIDEROLL, true)) {
        this->yaw = this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }

    if (this->transformation == PLAYER_FORM_GORON) {
        func_80836AD8(play, this);
        PlayerAnimation_Change(play, &this->skelAnime, D_8085BE84[PLAYER_ANIMGROUP_landing_roll][this->modelAnimType],
                               1.5f * sWaterSpeedFactor, 0.0f, 6.0f, ANIMMODE_ONCE, 0.0f);
    } else {
        PlayerAnimationHeader* anim = D_8085BE84[PLAYER_ANIMGROUP_landing_roll][this->modelAnimType];

        Player_SetAction(play, this, Player_Action_26, 0);
        PlayerAnimation_Change(play, &this->skelAnime, anim, 1.25f * sWaterSpeedFactor, arg2,
                               Animation_GetLastFrame(anim), ANIMMODE_ONCE, 0.0f);
    }
}

void func_80836C70(PlayState* play, Player* this, PlayerBodyPart bodyPartIndex) {
    static Vec3f D_8085D130 = { 0, 0, 0 };
    s32 i;

    for (i = 0; i < 4; i++) {
        Vec3f velocity;

        velocity.x = Rand_CenteredFloat(4.0f);
        velocity.y = Rand_ZeroFloat(2.0f);
        velocity.z = Rand_CenteredFloat(4.0f);
        D_8085D130.y = -0.2f;
        EffectSsHahen_Spawn(play, &this->bodyPartsPos[bodyPartIndex], &velocity, &D_8085D130, 0, 10, OBJECT_LINK_NUTS,
                            16, object_link_nuts_DL_008860);
    }
}

void func_80836D8C(Player* this) {
    this->actor.focus.rot.x = 0;
    this->actor.focus.rot.z = 0;
    this->headLimbRot.x = 0;
    this->headLimbRot.y = 0;
    this->headLimbRot.z = 0;
    this->upperLimbRot.x = 0;
    this->upperLimbRot.y = 0;
    this->upperLimbRot.z = 0;
    this->actor.shape.rot.y = this->actor.focus.rot.y;
    this->yaw = this->actor.focus.rot.y;
}

s32 func_80836DC0(PlayState* play, Player* this) {
    if ((MREG(48) != 0) || func_800C9DDC(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId)) {
        Player_SetAction(play, this, Player_Action_93, 0);
        this->stateFlags1 &= ~(PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE);
        Player_Anim_PlayOnceMorph(play, this, &gPlayerAnim_pn_attack);
        Player_StopHorizontalMovement(this);
        func_80836D8C(this);
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->unk_B48 = -2000.0f;
        this->actor.shape.shadowScale = 13.0f;
        func_8082E1F0(this, NA_SE_PL_DEKUNUTS_IN_GRD);
        return true;
    }

    return false;
}

void Player_RequestQuake(PlayState* play, u16 speed, s16 y, s16 duration) {
    s16 quakeIndex = Quake_Request(Play_GetCamera(play, CAM_ID_MAIN), QUAKE_TYPE_3);

    Quake_SetSpeed(quakeIndex, speed);
    Quake_SetPerturbations(quakeIndex, y, 0, 0, 0);
    Quake_SetDuration(quakeIndex, duration);
}

FallImpactInfo sFallImpactInfos[] = {
    { -8, 180, 40, 100, NA_SE_VO_LI_LAND_DAMAGE_S },
    { -16, 255, 140, 150, NA_SE_VO_LI_LAND_DAMAGE_S },
};

// Player_FallAgainstTheFloor, Player_LetTheBodiesHitTheFloor, Player_ImpactFloor, Player_ProcessFallDamage,
// Player_DamageOnFloorImpact, Player_CalculateFallDamage
s32 func_80836F10(PlayState* play, Player* this) {
    s32 fallDistance;

    if ((sPlayerFloorType == FLOOR_TYPE_6) || (sPlayerFloorType == FLOOR_TYPE_9) ||
        (this->csAction != PLAYER_CSACTION_NONE)) {
        fallDistance = 0;
    } else {
        fallDistance = this->fallDistance;
    }

    Math_StepToF(&this->speedXZ, 0.0f, 1.0f);
    this->stateFlags1 &= ~(PLAYER_STATE1_40000 | PLAYER_STATE1_80000);

    // Height enough for fall damage
    if (fallDistance >= 400) {
        s32 index;
        FallImpactInfo* entry;

        if (this->fallDistance < 800) {
            // small fall
            index = 0;
        } else {
            // big fall
            index = 1;
        }

        Player_PlaySfx(this, NA_SE_PL_BODY_HIT);

        entry = &sFallImpactInfos[index];
        Player_AnimSfx_PlayVoice(this, entry->sfxId);

        if (Player_InflictDamage(play, entry->damage)) {
            // Player's dead
            return -1;
        }

        func_80833998(this, 40);
        Player_RequestQuake(play, 32967, 2, 30);
        Player_RequestRumble(play, this, entry->sourceIntensity, entry->decayTimer, entry->decayStep, SQ(0));

        return index + 1;
    }

    // Tiny fall, won't damage player
    if (fallDistance > 200) {
        fallDistance *= 2;
        fallDistance = CLAMP_MAX(fallDistance, 255);

        Player_RequestRumble(play, this, fallDistance, fallDistance * 0.1f, fallDistance, SQ(0));
        if (sPlayerFloorType == FLOOR_TYPE_6) {
            //! @bug unreachable code: When sPlayerFloorType is equal to FLOOR_TYPE_6 then fallDistance is
            //! ignored (set to zero), so the previous check based on said variable will always fail, producing this
            //! current check to always be false.
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_CLIMB_END);
        }
    }

    Player_AnimSfx_PlayFloorLand(this);
    return 0;
}

s32 func_808370D4(PlayState* play, Player* this) {
    if ((this->fallDistance < 800) &&
        (this->controlStickDirections[this->controlStickDataIndex] == PLAYER_STICK_DIR_FORWARD) &&
        !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
        func_80836B3C(play, this, 0.0f);

        return true;
    }
    return false;
}

void func_80837134(PlayState* play, Player* this) {
    PlayerAnimationHeader* anim = D_8085BE84[PLAYER_ANIMGROUP_landing][this->modelAnimType];
    s32 temp_v0_2; // sp28

    this->stateFlags1 &= ~(PLAYER_STATE1_40000 | PLAYER_STATE1_80000);

    if (this->transformation == PLAYER_FORM_DEKU) {
        s32 var_v1 = false;

        if ((BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pn_rakkafinish)) ||
            (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pn_kakkufinish))) {
            func_80836C70(play, this, PLAYER_BODYPART_LEFT_HAND);
            func_80836C70(play, this, PLAYER_BODYPART_RIGHT_HAND);
            var_v1 = true;
        }

        if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A) && func_80836DC0(play, this)) {
            return;
        }

        if (var_v1) {
            func_80836A98(this, anim, play);
            Player_AnimSfx_PlayFloorLand(this);
            return;
        }
    } else if (this->stateFlags2 & PLAYER_STATE2_80000) {
        if (Player_CheckHostileLockOn(this)) {
            anim = D_8085C2A4[this->av1.actionVar1].unk_8;
        } else {
            anim = D_8085C2A4[this->av1.actionVar1].unk_4;
        }
    } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_run_jump)) {
        anim = &gPlayerAnim_link_normal_run_jump_end;
    } else if (Player_CheckHostileLockOn(this)) {
        anim = &gPlayerAnim_link_anchor_landingR;
        func_8082FC60(this);
    } else if (this->fallDistance <= 80) {
        anim = D_8085BE84[PLAYER_ANIMGROUP_short_landing][this->modelAnimType];
    } else if (func_808370D4(play, this)) {
        return;
    }

    temp_v0_2 = func_80836F10(play, this);
    if (temp_v0_2 > 0) { // Player suffered damage because of this fall
        func_80836A98(this, D_8085BE84[PLAYER_ANIMGROUP_landing][this->modelAnimType], play);
        this->skelAnime.endFrame = 8.0f;

        // `func_80836A98` above can choose from a few different "idle" action variants.
        // However `fallDamageStunTimer` is only processed by `Player_Action_Idle`.
        // This means it is possible for the stun to not take effect
        // (for example, by holding Z when landing).
        if (temp_v0_2 == 1) {
            this->av2.fallDamageStunTimer = 10;
        } else {
            this->av2.fallDamageStunTimer = 20;
        }
    } else if (temp_v0_2 == 0) {
        func_80836A98(this, anim, play);
    }
}

void func_808373A4(PlayState* play, Player* this) {
    Player_Anim_PlayOnceMorph(play, this, &gPlayerAnim_pn_attack);
    this->unk_B10[0] = 20000.0f;
    this->unk_B10[1] = 0x30000;
    Player_PlaySfx(this, NA_SE_PL_DEKUNUTS_ATTACK);
}

s32 func_808373F8(PlayState* play, Player* this, u16 sfxId) {
    PlayerAnimationHeader* anim;
    f32 speed;
    s16 yawDiff = this->yaw - this->actor.shape.rot.y;

    if ((IREG(66) / 100.0f) < this->speedXZ) {
        speed = IREG(67) / 100.0f;
    } else {
        speed = (IREG(68) / 100.0f + (IREG(69) * this->speedXZ) / 1000.0f);

        if ((this->transformation == PLAYER_FORM_DEKU) && (speed < 8.0f)) {
            speed = 8.0f;
        } else if (speed < 5.0f) {
            speed = 5.0f;
        }
    }

    if ((ABS_ALT(yawDiff) >= 0x1000) || (this->speedXZ <= 4.0f)) {
        anim = &gPlayerAnim_link_normal_jump;
    } else {
        s32 var_v1;

        if ((this->transformation != PLAYER_FORM_DEKU) &&
            ((sPrevFloorProperty == FLOOR_PROPERTY_1) || (sPrevFloorProperty == FLOOR_PROPERTY_2))) {
            if (sPrevFloorProperty == FLOOR_PROPERTY_1) {
                var_v1 = 4;
            } else {
                var_v1 = 5;
            }

            func_80834D50(play, this, D_8085C2A4[var_v1].unk_0, speed, ((var_v1 == 4) ? NA_SE_VO_LI_SWORD_N : sfxId));
            this->av2.actionVar2 = -1;
            this->stateFlags2 |= PLAYER_STATE2_80000;
            this->av1.actionVar1 = var_v1;
            return true;
        }
        anim = &gPlayerAnim_link_normal_run_jump;
    }

    // Deku hopping
    if (this->transformation == PLAYER_FORM_DEKU) {
        speed *= 0.3f + ((5 - this->remainingHopsCounter) * 0.18f);
        if (speed < 4.0f) {
            speed = 4.0f;
        }

        if ((this->actor.depthInWater > 0.0f) && (this->remainingHopsCounter != 0)) {
            this->actor.world.pos.y += this->actor.depthInWater;
            func_80834D50(play, this, anim, speed, NA_SE_NONE);
            this->av2.actionVar2 = 1;
            this->stateFlags3 |= PLAYER_STATE3_200000;
            Player_PlaySfx(this, (NA_SE_PL_DEKUNUTS_JUMP5 + 1 - this->remainingHopsCounter));
            Player_AnimSfx_PlayVoice(this, sfxId);
            this->remainingHopsCounter--;
            if (GameInteractor_Should(VB_DEKU_LINK_SPIN_ON_LAST_HOP, this->remainingHopsCounter == 0)) {
                this->stateFlags2 |= PLAYER_STATE2_80000;
                func_808373A4(play, this);
            }

            return true;
        }

        if (this->actor.velocity.y > 0.0f) {
            sfxId = NA_SE_NONE;
        }
    }

    func_80834D50(play, this, anim, speed, sfxId);
    this->av2.actionVar2 = 1;

    return true;
}

s32 func_80837730(PlayState* play, Player* this, f32 arg2, s32 scale) {
    f32 sp3C = fabsf(arg2);

    if (sp3C > 2.0f) {
        WaterBox* waterBox;
        f32 sp34;
        Vec3f pos;

        Math_Vec3f_Copy(&pos, &this->bodyPartsPos[PLAYER_BODYPART_WAIST]);
        pos.y += 20.0f;
        if (WaterBox_GetSurface1(play, &play->colCtx, pos.x, pos.z, &pos.y, &waterBox)) {
            sp34 = pos.y - this->bodyPartsPos[PLAYER_BODYPART_LEFT_FOOT].y;
            if ((sp34 > -2.0f) && (sp34 < 100.0f)) {
                EffectSsGSplash_Spawn(play, &pos, NULL, NULL,
                                      (sp3C <= 10.0f) ? EFFSSGSPLASH_TYPE_0 : EFFSSGSPLASH_TYPE_1, scale);
                return true;
            }
        }
    }

    return false;
}

s32 func_8083784C(Player* this) {
    if (this->actor.velocity.y < 0.0f) {
        if ((this->actor.depthInWater > 0.0f) &&
            ((this->ageProperties->unk_2C - this->actor.depthInWater) < sPlayerYDistToFloor)) {
            if ((this->remainingHopsCounter != 0) && (gSaveContext.save.saveInfo.playerData.health != 0) &&
                !(this->stateFlags1 & PLAYER_STATE1_4000000)) {
                if (((this->talkActor == NULL) || !(this->talkActor->flags & ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED))) {
                    return true;
                }
            }
        }
    }

    return false;
}

void func_808378FC(PlayState* play, Player* this) {
    if (!Player_IsZTargetingWithHostileUpdate(this)) {
        this->stateFlags2 |= PLAYER_STATE2_20;
    }

    if (func_8083784C(this) && func_808373F8(play, this, NA_SE_VO_LI_AUTO_JUMP)) {
        func_80837730(play, this, 20.0f, this->actor.velocity.y * 50.0f);
    }
}

bool func_8083798C(Player* this) {
    return (this->interactRangeActor != NULL) && (this->heldActor == NULL) &&
           (this->transformation != PLAYER_FORM_DEKU);
}

void func_808379C0(PlayState* play, Player* this) {
    if (func_8083798C(this)) {
        Actor* interactRangeActor = this->interactRangeActor;
        PlayerAnimationHeader* anim;

        if ((interactRangeActor->id == ACTOR_EN_ISHI) &&
            (ENISHI_GET_SIZE_FLAG(interactRangeActor) != ISHI_SIZE_SMALL_ROCK)) {
            Player_SetAction(play, this, Player_Action_38, 0);
            anim = &gPlayerAnim_link_silver_carry;
        } else if (((interactRangeActor->id == ACTOR_EN_BOMBF) || (interactRangeActor->id == ACTOR_EN_KUSA) ||
                    (interactRangeActor->id == ACTOR_EN_KUSA2) || (interactRangeActor->id == ACTOR_OBJ_GRASS_CARRY)) &&
                   (Player_GetStrength() <= PLAYER_STRENGTH_DEKU)) {
            Player_SetAction(play, this, Player_Action_40, 0);
            anim = &gPlayerAnim_link_normal_nocarry_free;

            this->actor.world.pos.x =
                (Math_SinS(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.x;
            this->actor.world.pos.z =
                (Math_CosS(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.z;

            this->yaw = this->actor.shape.rot.y = interactRangeActor->yawTowardsPlayer + 0x8000;
        } else {
            Player_SetAction(play, this, Player_Action_37, 0);
            anim = D_8085BE84[PLAYER_ANIMGROUP_carryB][this->modelAnimType];
        }

        Player_Anim_PlayOnce(play, this, anim);
    } else {
        func_80836988(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
    }
}

void Player_SetupTalk(PlayState* play, Player* this) {
    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_Talk, 0);

    this->exchangeItemAction = PLAYER_IA_NONE;
    this->stateFlags1 |= (PLAYER_STATE1_TALKING | PLAYER_STATE1_20000000);
    if (this->actor.textId != 0) {
        Message_StartTextbox(play, this->actor.textId, this->talkActor);
    }
    this->focusActor = this->talkActor;
}

void func_80837BD0(PlayState* play, Player* this) {
    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_52, 0);
}

void func_80837BF8(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_45, 0);
}

void func_80837C20(PlayState* play, Player* this) {
    s32 sp1C = this->av2.actionVar2;
    s32 sp18 = this->av1.actionVar1;

    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_50, 0);
    this->actor.velocity.y = 0.0f;

    this->av2.actionVar2 = sp1C;
    this->av1.actionVar1 = sp18;
}

void func_80837C78(PlayState* play, Player* this) {
    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_65, 0);
    this->stateFlags1 |= (PLAYER_STATE1_400 | PLAYER_STATE1_20000000);

    if (this->getItemId == GI_HEART_CONTAINER) {
        this->av2.actionVar2 = 20;
    } else if (this->getItemId >= GI_NONE) {
        this->av2.actionVar2 = 1;
    } else {
        this->getItemId = -this->getItemId;
    }
}

void func_80837CEC(PlayState* play, Player* this, CollisionPoly* arg2, f32 arg3, PlayerAnimationHeader* anim) {
    f32 nx = COLPOLY_GET_NORMAL(arg2->normal.x);
    f32 nz = COLPOLY_GET_NORMAL(arg2->normal.z);

    Player_SetAction(play, this, Player_Action_48, 0);
    func_8082DE50(play, this);
    Player_Anim_PlayOnce(play, this, anim);

    this->actor.world.pos.x -= (arg3 + 1.0f) * nx;
    this->actor.world.pos.z -= (arg3 + 1.0f) * nz;
    this->actor.shape.rot.y = Math_Atan2S_XY(nz, nx);
    this->yaw = this->actor.shape.rot.y;

    func_8082DAD4(this);
    this->actor.velocity.y = 0.0f;
    Player_Anim_ResetPrevTranslRot(this);
}

s32 func_80837DEC(Player* this, PlayState* play) {
    if ((this->transformation != PLAYER_FORM_GORON) && (this->transformation != PLAYER_FORM_DEKU) &&
        (this->actor.depthInWater < -80.0f)) {
        //! @bug `floorPitch` and `floorPitchAlt` are cleared to 0 before this function is called,
        //! because the player left the ground. The angles will always be zero and therefore will always
        //! pass these checks. The intention seems to be to prevent ledge hanging or vine grabbing when
        //! walking off of a steep enough slope.
        if ((ABS_ALT(this->floorPitch) < 0xAAA) && (ABS_ALT(this->floorPitchAlt) < 0xAAA)) {
            CollisionPoly* entityPoly;
            CollisionPoly* sp90;
            s32 entityBgId;
            s32 sp88;
            Vec3f sp7C;
            Vec3f sp70;
            f32 temp_fv1_2;
            f32 entityNormalX;
            f32 entityNormalY;
            f32 entityNormalZ;
            f32 temp_fv0_2;
            f32 var_fv1;

            sp7C.x = this->actor.prevPos.x - this->actor.world.pos.x;
            sp7C.z = this->actor.prevPos.z - this->actor.world.pos.z;

            var_fv1 = sqrtf(SQXZ(sp7C));
            if (var_fv1 != 0.0f) {
                var_fv1 = 5.0f / var_fv1;
            } else {
                var_fv1 = 0.0f;
            }

            sp7C.x = this->actor.prevPos.x + (sp7C.x * var_fv1);
            sp7C.y = this->actor.world.pos.y;
            sp7C.z = this->actor.prevPos.z + (sp7C.z * var_fv1);

            if (BgCheck_EntityLineTest2(&play->colCtx, &this->actor.world.pos, &sp7C, &sp70, &entityPoly, true, false,
                                        false, true, &entityBgId, &this->actor)) {
                if (ABS_ALT(entityPoly->normal.y) < 0x258) {
                    s32 var_v1_2; // sp54

                    entityNormalX = COLPOLY_GET_NORMAL(entityPoly->normal.x);
                    entityNormalY = COLPOLY_GET_NORMAL(entityPoly->normal.y);
                    entityNormalZ = COLPOLY_GET_NORMAL(entityPoly->normal.z);

                    temp_fv0_2 = Math3D_UDistPlaneToPos(entityNormalX, entityNormalY, entityNormalZ, entityPoly->dist,
                                                        &this->actor.world.pos);

                    sp70.x = this->actor.world.pos.x - ((temp_fv0_2 + 1.0f) * entityNormalX);
                    sp70.z = this->actor.world.pos.z - ((temp_fv0_2 + 1.0f) * entityNormalZ);
                    sp70.y = this->actor.world.pos.y + 268 * 0.1f;

                    temp_fv1_2 = this->actor.world.pos.y -
                                 BgCheck_EntityRaycastFloor5(&play->colCtx, &sp90, &sp88, &this->actor, &sp70);
                    if ((temp_fv1_2 >= -11.0f) && (temp_fv1_2 <= 0.0f)) {
                        var_v1_2 = (sPrevFloorProperty == FLOOR_PROPERTY_6);
                        if (!var_v1_2) {
                            if (SurfaceType_GetWallFlags(&play->colCtx, entityPoly, entityBgId) & WALL_FLAG_3) {
                                var_v1_2 = true;
                            }
                        }

                        func_80837CEC(play, this, entityPoly, temp_fv0_2,
                                      var_v1_2 ? &gPlayerAnim_link_normal_Fclimb_startB
                                               : &gPlayerAnim_link_normal_fall);
                        if (var_v1_2) {
                            Player_SetupWaitForPutAway(play, this, func_80837C20);

                            this->actor.shape.rot.y = this->yaw += 0x8000;
                            this->stateFlags1 |= PLAYER_STATE1_200000;
                            Player_AnimReplace_Setup(play, this,
                                                     ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_4 |
                                                         ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE | ANIM_FLAG_80);
                            this->av2.actionVar2 = -1;
                            this->av1.actionVar1 = var_v1_2;
                        } else {
                            this->stateFlags1 |= PLAYER_STATE1_2000;
                            this->stateFlags1 &= ~PLAYER_STATE1_PARALLEL;
                        }

                        Player_PlaySfx(this, NA_SE_PL_SLIPDOWN);
                        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HANG);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void func_808381A0(Player* this, PlayerAnimationHeader* anim, PlayState* play) {
    Player_SetAction(play, this, Player_Action_49, 0);
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, 1.3f);
}

Vec3f D_8085D148 = { 0.0f, 50.0f, 0.0f };

s32 func_808381F8(PlayState* play, Player* this) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f pos;
    f32 yIntersect;

    Player_TranslateAndRotateY(this, &this->actor.prevPos, &D_8085D148, &pos);

    yIntersect = BgCheck_EntityRaycastFloor5(&play->colCtx, &poly, &bgId, &this->actor, &pos);

    return fabsf(yIntersect - this->actor.world.pos.y) < 10.0f;
}

Vec3f D_8085D154 = { 0.0f, 0.0f, 100.0f };

void func_8083827C(Player* this, PlayState* play) {
    s32 temp_t0; // sp64
    CollisionPoly* sp60;
    s32 sp5C;
    WaterBox* waterBox;
    Vec3f sp4C;
    f32 sp48;
    f32 sp44;

    this->fallDistance = this->fallStartHeight - (s32)this->actor.world.pos.y;
    if (!(this->stateFlags1 & (PLAYER_STATE1_8000000 | PLAYER_STATE1_20000000)) &&
        ((this->stateFlags1 & PLAYER_STATE1_80000000) ||
         !(this->stateFlags3 & (PLAYER_STATE3_200 | PLAYER_STATE3_2000))) &&
        !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (func_80835428(play, this)) {
            return;
        }

        if (sPrevFloorProperty == FLOOR_PROPERTY_8) {
            this->actor.world.pos.x = this->actor.prevPos.x;
            this->actor.world.pos.z = this->actor.prevPos.z;
            return;
        }

        if ((this->stateFlags3 & PLAYER_STATE3_2) || (this->skelAnime.movementFlags & ANIM_FLAG_80)) {
            return;
        }

        if ((Player_Action_25 == this->actionFunc) || (Player_Action_27 == this->actionFunc) ||
            (Player_Action_28 == this->actionFunc) || (Player_Action_96 == this->actionFunc) ||
            (Player_Action_82 == this->actionFunc) || (Player_Action_83 == this->actionFunc)) {
            return;
        }

        if ((sPrevFloorProperty == FLOOR_PROPERTY_7) || (this->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) ||
            ((this->skelAnime.movementFlags & ANIM_FLAG_ENABLE_MOVEMENT) && func_808381F8(play, this))) {
            Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
            if (this->speedXZ > 0.0f) {
                Player_StopHorizontalMovement(this);
            }
            this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND_TOUCH;
            return;
        }

        temp_t0 = BINANG_SUB(this->yaw, this->actor.shape.rot.y);
        Player_SetAction(play, this, Player_Action_25, 1);
        func_8082DD2C(play, this);

        this->floorSfxOffset = this->prevFloorSfxOffset;
        if ((this->transformation != PLAYER_FORM_GORON) &&
            ((this->transformation != PLAYER_FORM_DEKU) || (this->remainingHopsCounter != 0)) &&
            (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_LEAVE)) {
            if (!(this->stateFlags1 & PLAYER_STATE1_8000000)) {
                if ((sPrevFloorProperty != FLOOR_PROPERTY_6) && (sPrevFloorProperty != FLOOR_PROPERTY_9) &&
                    (sPlayerYDistToFloor > 20.0f) && (this->meleeWeaponState == PLAYER_MELEE_WEAPON_STATE_0)) {
                    if ((ABS_ALT(temp_t0) < 0x2000) && (this->speedXZ > 3.0f)) {
                        if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
                            if (((this->transformation == PLAYER_FORM_ZORA) &&
                                 CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A)) ||
                                ((sPrevFloorProperty == FLOOR_PROPERTY_11) &&
                                 (this->transformation != PLAYER_FORM_GORON) &&
                                 (this->transformation != PLAYER_FORM_DEKU))) {

                                sp48 = func_80835CD8(play, this, &D_8085D154, &sp4C, &sp60, &sp5C);
                                sp44 = this->actor.world.pos.y;

                                if (GameInteractor_Should(
                                        VB_LINK_DIVE_OVER_WATER,
                                        WaterBox_GetSurface1(play, &play->colCtx, sp4C.x, sp4C.z, &sp44, &waterBox) &&
                                            ((sp44 - sp48) > 50.0f))) {
                                    func_80834DB8(this, &gPlayerAnim_link_normal_run_jump_water_fall, 6.0f, play);
                                    Player_SetAction(play, this, Player_Action_27, 0);
                                    return;
                                }
                            }
                        }
                        func_808373F8(play, this, NA_SE_VO_LI_AUTO_JUMP);
                        return;
                    }
                }
            }
        }

        // Checking if the ledge is tall enough for Player to hang from
        if ((sPrevFloorProperty == FLOOR_PROPERTY_9) || (sPlayerYDistToFloor <= this->ageProperties->unk_34) ||
            !func_80837DEC(this, play)) {
            Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
        }
    } else {
        this->fallStartHeight = this->actor.world.pos.y;
        this->remainingHopsCounter = 5;
    }
}

s32 func_8083868C(PlayState* play, Player* this) {
    s32 camMode;
    Camera* camera;

    if (this->unk_AA5 == PLAYER_UNKAA5_3) {
        if (func_800B7118(this)) {
            if (this->transformation == PLAYER_FORM_HUMAN) {
                camMode = CAM_MODE_SLINGSHOT;
            } else if (this->transformation == PLAYER_FORM_DEKU) {
                camMode = CAM_MODE_DEKUSHOOT;
            } else {
                camMode = CAM_MODE_BOWARROW;
            }
        } else {
            camMode = CAM_MODE_ZORAFIN;
        }
    } else {
        camMode = CAM_MODE_FIRSTPERSON;
    }

    camera = (this->actor.id == ACTOR_PLAYER) ? Play_GetCamera(play, CAM_ID_MAIN)
                                              : Play_GetCamera(play, ((EnTest3*)this)->subCamId);

    return Camera_ChangeMode(camera, camMode);
}

void Player_StopCutscene(Player* this) {
    if (this->csId > CS_ID_NONE) {
        CutsceneManager_Stop(this->csId);
        this->csId = CS_ID_NONE;
    }
}

/**
 * @brief If appropriate, setup action for performing a `csAction`
 *
 * @return true if a `csAction` is started, false if not
 */
s32 Player_StartCsAction(PlayState* play, Player* this) {
    if (this->unk_AA5 == PLAYER_UNKAA5_4) {
        Player_StopCutscene(this);
        this->actor.flags &= ~ACTOR_FLAG_TALK;
        Player_SetAction(play, this, Player_Action_CsAction, 0);

        if (this->cv.haltActorsDuringCsAction) {
            this->stateFlags1 |= PLAYER_STATE1_20000000;
        }
        func_8082DC38(this);

        return true;
    }
    return false;
}

void func_80838830(Player* this, s16 objectId) {
    s32 pad[2];

    if (objectId != OBJECT_UNSET_0) {
        // #region 2S2H [Port] We don't care to wait for the item to load, mark it as loaded immediately
        this->giObjectLoading = false;
        // osCreateMesgQueue(&this->giObjectLoadQueue, &this->giObjectLoadMsg, 1);
        // DmaMgr_SendRequestImpl(&this->giObjectDmaRequest, this->giObjectSegment, gObjectTable[objectId].vromStart,
        //                        gObjectTable[objectId].vromEnd - gObjectTable[objectId].vromStart, 0,
        //                        &this->giObjectLoadQueue, OS_MESG_PTR(NULL));
        // #endregion
    }
}

PlayerAnimationHeader* D_8085D160[PLAYER_FORM_MAX] = {
    &gPlayerAnim_pz_maskoffstart, // PLAYER_FORM_FIERCE_DEITY
    &gPlayerAnim_pg_maskoffstart, // PLAYER_FORM_GORON
    &gPlayerAnim_pz_maskoffstart, // PLAYER_FORM_ZORA
    &gPlayerAnim_pn_maskoffstart, // PLAYER_FORM_DEKU
    &gPlayerAnim_cl_setmask,      // PLAYER_FORM_HUMAN
};

void func_808388B8(PlayState* play, Player* this, PlayerTransformation playerForm) {
    func_8082DE50(play, this);
    Player_SetAction_PreserveItemAction(play, this, Player_Action_86, 0);
    Player_Anim_PlayOnceMorphAdjusted(play, this, D_8085D160[this->transformation]);
    gSaveContext.save.playerForm = playerForm;
    this->stateFlags1 |= PLAYER_STATE1_2;

    D_80862B50 = play->envCtx.adjLightSettings;
    this->actor.velocity.y = 0.0f;
    Actor_DeactivateLens(play);
}

void func_808389BC(PlayState* play, Player* this) {
    Player_SetAction_PreserveItemAction(play, this, Player_Action_89, 0);
    Player_Anim_PlayOnceMorphAdjusted(play, this, &gPlayerAnim_cl_setmask);
    this->stateFlags1 |= (PLAYER_STATE1_100 | PLAYER_STATE1_20000000);
    func_8082DAD4(this);
}

void func_80838A20(PlayState* play, Player* this) {
    Player_SetAction_PreserveItemAction(play, this, Player_Action_90, 0);
    Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_cl_maskoff);
    this->currentMask = PLAYER_MASK_NONE;
    this->stateFlags1 |= (PLAYER_STATE1_100 | PLAYER_STATE1_20000000);
    func_8082DAD4(this);
    Magic_Reset(play);
}

u8 sPlayerMass[PLAYER_FORM_MAX] = {
    100, // PLAYER_FORM_FIERCE_DEITY
    200, // PLAYER_FORM_GORON
    80,  // PLAYER_FORM_ZORA
    20,  // PLAYER_FORM_DEKU
    50,  // PLAYER_FORM_HUMAN
};

PlayerAnimationHeader* D_8085D17C[PLAYER_FORM_MAX] = {
    &gPlayerAnim_link_normal_okarina_start, // PLAYER_FORM_FIERCE_DEITY
    &gPlayerAnim_pg_gakkistart,             // PLAYER_FORM_GORON
    &gPlayerAnim_pz_gakkistart,             // PLAYER_FORM_ZORA
    &gPlayerAnim_pn_gakkistart,             // PLAYER_FORM_DEKU
    &gPlayerAnim_link_normal_okarina_start, // PLAYER_FORM_HUMAN
};
PlayerAnimationHeader* D_8085D190[PLAYER_FORM_MAX] = {
    &gPlayerAnim_link_normal_okarina_swing, // PLAYER_FORM_FIERCE_DEITY
    &gPlayerAnim_pg_gakkiplay,              // PLAYER_FORM_GORON
    &gPlayerAnim_pz_gakkiplay,              // PLAYER_FORM_ZORA
    &gPlayerAnim_pn_gakkiplay,              // PLAYER_FORM_DEKU
    &gPlayerAnim_link_normal_okarina_swing, // PLAYER_FORM_HUMAN
};

u8 D_8085D1A4[PLAYER_IA_MAX] = {
    GI_NONE,                // PLAYER_IA_NONE
    GI_NONE,                // PLAYER_IA_LAST_USED
    GI_NONE,                // PLAYER_IA_FISHING_ROD
    GI_SWORD_KOKIRI,        // PLAYER_IA_SWORD_KOKIRI
    GI_SWORD_RAZOR,         // PLAYER_IA_SWORD_RAZOR
    GI_SWORD_GILDED,        // PLAYER_IA_SWORD_GILDED
    GI_SWORD_GREAT_FAIRY,   // PLAYER_IA_SWORD_TWO_HANDED
    GI_DEKU_STICKS_1,       // PLAYER_IA_DEKU_STICK
    GI_SWORD_KOKIRI,        // PLAYER_IA_ZORA_BOOMERANG
    GI_QUIVER_30,           // PLAYER_IA_BOW
    GI_ARROW_FIRE,          // PLAYER_IA_BOW_FIRE
    GI_ARROW_ICE,           // PLAYER_IA_BOW_ICE
    GI_ARROW_LIGHT,         // PLAYER_IA_BOW_LIGHT
    GI_HOOKSHOT,            // PLAYER_IA_HOOKSHOT
    GI_BOMBS_1,             // PLAYER_IA_BOMB
    GI_POWDER_KEG,          // PLAYER_IA_POWDER_KEG
    GI_BOMBCHUS_10,         // PLAYER_IA_BOMBCHU
    GI_40,                  // PLAYER_IA_11
    GI_DEKU_NUTS_1,         // PLAYER_IA_DEKU_NUT
    GI_PICTOGRAPH_BOX,      // PLAYER_IA_PICTOGRAPH_BOX
    GI_OCARINA_OF_TIME,     // PLAYER_IA_OCARINA
    GI_BOTTLE,              // PLAYER_IA_BOTTLE_EMPTY
    GI_FISH,                // PLAYER_IA_BOTTLE_FISH
    GI_75,                  // PLAYER_IA_BOTTLE_SPRING_WATER
    GI_ICE_TRAP,            // PLAYER_IA_BOTTLE_HOT_SPRING_WATER
    GI_ZORA_EGG,            // PLAYER_IA_BOTTLE_ZORA_EGG
    GI_GOLD_DUST,           // PLAYER_IA_BOTTLE_DEKU_PRINCESS
    GI_6C,                  // PLAYER_IA_BOTTLE_GOLD_DUST
    GI_SEAHORSE,            // PLAYER_IA_BOTTLE_1C
    GI_MUSHROOM,            // PLAYER_IA_BOTTLE_SEAHORSE
    GI_HYLIAN_LOACH,        // PLAYER_IA_BOTTLE_MUSHROOM
    GI_DEKU_PRINCESS,       // PLAYER_IA_BOTTLE_HYLIAN_LOACH
    GI_BUG,                 // PLAYER_IA_BOTTLE_BUG
    GI_POE,                 // PLAYER_IA_BOTTLE_POE
    GI_BIG_POE,             // PLAYER_IA_BOTTLE_BIG_POE
    GI_POTION_RED,          // PLAYER_IA_BOTTLE_POTION_RED
    GI_POTION_BLUE,         // PLAYER_IA_BOTTLE_POTION_BLUE
    GI_POTION_GREEN,        // PLAYER_IA_BOTTLE_POTION_GREEN
    GI_MILK_HALF,           // PLAYER_IA_BOTTLE_MILK
    GI_MILK_HALF,           // PLAYER_IA_BOTTLE_MILK_HALF
    GI_CHATEAU,             // PLAYER_IA_BOTTLE_CHATEAU
    GI_FAIRY,               // PLAYER_IA_BOTTLE_FAIRY
    GI_MOONS_TEAR,          // PLAYER_IA_MOONS_TEAR
    GI_DEED_LAND,           // PLAYER_IA_DEED_LAND
    GI_ROOM_KEY,            // PLAYER_IA_ROOM_KEY
    GI_LETTER_TO_KAFEI,     // PLAYER_IA_LETTER_TO_KAFEI
    GI_MAGIC_BEANS,         // PLAYER_IA_MAGIC_BEANS
    GI_DEED_SWAMP,          // PLAYER_IA_DEED_SWAMP
    GI_DEED_MOUNTAIN,       // PLAYER_IA_DEED_MOUNTAIN
    GI_DEED_OCEAN,          // PLAYER_IA_DEED_OCEAN
    GI_MOONS_TEAR,          // PLAYER_IA_32
    GI_LETTER_TO_MAMA,      // PLAYER_IA_LETTER_MAMA
    GI_A7,                  // PLAYER_IA_34
    GI_A8,                  // PLAYER_IA_35
    GI_PENDANT_OF_MEMORIES, // PLAYER_IA_PENDANT_OF_MEMORIES
    GI_PENDANT_OF_MEMORIES, // PLAYER_IA_37
    GI_PENDANT_OF_MEMORIES, // PLAYER_IA_38
    GI_PENDANT_OF_MEMORIES, // PLAYER_IA_39
    GI_MASK_TRUTH,          // PLAYER_IA_MASK_TRUTH
    GI_MASK_KAFEIS_MASK,    // PLAYER_IA_MASK_KAFEIS_MASK
    GI_MASK_ALL_NIGHT,      // PLAYER_IA_MASK_ALL_NIGHT
    GI_MASK_BUNNY,          // PLAYER_IA_MASK_BUNNY
    GI_MASK_KEATON,         // PLAYER_IA_MASK_KEATON
    GI_MASK_GARO,           // PLAYER_IA_MASK_GARO
    GI_MASK_ROMANI,         // PLAYER_IA_MASK_ROMANI
    GI_MASK_CIRCUS_LEADER,  // PLAYER_IA_MASK_CIRCUS_LEADER
    GI_MASK_POSTMAN,        // PLAYER_IA_MASK_POSTMAN
    GI_MASK_COUPLE,         // PLAYER_IA_MASK_COUPLE
    GI_MASK_GREAT_FAIRY,    // PLAYER_IA_MASK_GREAT_FAIRY
    GI_MASK_GIBDO,          // PLAYER_IA_MASK_GIBDO
    GI_MASK_DON_GERO,       // PLAYER_IA_MASK_DON_GERO
    GI_MASK_KAMARO,         // PLAYER_IA_MASK_KAMARO
    GI_MASK_CAPTAIN,        // PLAYER_IA_MASK_CAPTAIN
    GI_MASK_STONE,          // PLAYER_IA_MASK_STONE
    GI_MASK_BREMEN,         // PLAYER_IA_MASK_BREMEN
    GI_MASK_BLAST,          // PLAYER_IA_MASK_BLAST
    GI_MASK_SCENTS,         // PLAYER_IA_MASK_SCENTS
    GI_MASK_GIANT,          // PLAYER_IA_MASK_GIANT
    GI_MASK_FIERCE_DEITY,   // PLAYER_IA_MASK_FIERCE_DEITY
    GI_MASK_GORON,          // PLAYER_IA_MASK_GORON
    GI_MASK_ZORA,           // PLAYER_IA_MASK_ZORA
    GI_MASK_DEKU,           // PLAYER_IA_MASK_DEKU
    GI_LENS_OF_TRUTH,       // PLAYER_IA_LENS_OF_TRUTH
};

PlayerAnimationHeader* D_8085D1F8[] = {
    &gPlayerAnim_link_normal_give_other,
    &gPlayerAnim_link_normal_take_out, // Hold up cutscene item; "this item doesn't work here"
};

s32 Player_ActionHandler_13(Player* this, PlayState* play) {
    PlayerBottle bottleAction;

    if (this->unk_AA5 != PLAYER_UNKAA5_0) {
        if (!(this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) &&
            !(this->stateFlags1 & PLAYER_STATE1_8000000) && !(this->stateFlags1 & PLAYER_STATE1_800000) &&
            !(this->stateFlags3 & PLAYER_STATE3_8) && !(this->skelAnime.movementFlags & ANIM_FLAG_ENABLE_MOVEMENT)) {
            Player_StopCutscene(this);
            func_80833AA0(this, play);
            return true;
        }
        if (!Player_StartCsAction(play, this)) {
            if (this->unk_AA5 == PLAYER_UNKAA5_5) {
                if ((this->itemAction >= PLAYER_IA_MASK_MIN) && (this->itemAction <= PLAYER_IA_MASK_MAX)) {
                    PlayerMask maskId = GET_MASK_FROM_IA(this->itemAction);

                    this->prevMask = this->currentMask;
                    if ((u32)(maskId == this->currentMask) || (this->itemAction < PLAYER_IA_MASK_GIANT) ||
                        ((this->itemAction == PLAYER_IA_MASK_GIANT) && (this->transformation != PLAYER_FORM_HUMAN))) {
                        if (maskId == this->currentMask) {
                            this->currentMask = PLAYER_MASK_NONE;
                        } else {
                            this->currentMask = maskId;
                        }

                        if (this->transformation == PLAYER_FORM_HUMAN) {
                            func_80838A20(play, this);
                            return true;
                        }

                        func_808388B8(play, this, PLAYER_FORM_HUMAN);
                    } else {
                        this->currentMask = maskId;
                        if (this->currentMask == PLAYER_MASK_GIANT) {
                            func_808389BC(play, this);
                            return true;
                        }
                        func_808388B8(play, this, this->itemAction - PLAYER_IA_MASK_FIERCE_DEITY);
                    }
                    gSaveContext.save.equippedMask = this->currentMask;
                } else if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK) ||
                           (this->itemAction == PLAYER_IA_PICTOGRAPH_BOX) ||
                           ((this->itemAction != this->unk_B2B) &&
                            ((this->itemAction == PLAYER_IA_BOTTLE_BIG_POE) ||
                             ((this->itemAction >= PLAYER_IA_BOTTLE_ZORA_EGG) &&
                              (this->itemAction <= PLAYER_IA_BOTTLE_HYLIAN_LOACH)) ||
                             (this->itemAction > PLAYER_IA_BOTTLE_FAIRY) ||
                             ((this->talkActor != NULL) && (this->exchangeItemAction > PLAYER_IA_NONE) &&
                              (((this->exchangeItemAction == PLAYER_IA_MAGIC_BEANS) &&
                                (this->itemAction == PLAYER_IA_MAGIC_BEANS)) ||
                               ((this->exchangeItemAction != PLAYER_IA_MAGIC_BEANS) &&
                                (Player_BottleFromIA(this, this->itemAction) > PLAYER_BOTTLE_NONE))))))) {
                    Actor* talkActor;
                    s32 heldItemTemp = this->itemAction;

                    Player_StopCutscene(this);
                    this->itemAction = PLAYER_IA_NONE;
                    Player_SetAction_PreserveItemAction(play, this, Player_Action_ExchangeItem, 0);
                    talkActor = this->talkActor;
                    this->itemAction = heldItemTemp;
                    this->csId = CS_ID_NONE;

                    if ((talkActor != NULL) && (((this->exchangeItemAction == PLAYER_IA_MAGIC_BEANS) &&
                                                 (this->itemAction == PLAYER_IA_MAGIC_BEANS)) ||
                                                ((this->exchangeItemAction != PLAYER_IA_MAGIC_BEANS) &&
                                                 (this->exchangeItemAction > PLAYER_IA_NONE)))) {
                        this->stateFlags1 |= (PLAYER_STATE1_20000000 | PLAYER_STATE1_TALKING);
                        if (this->exchangeItemAction == PLAYER_IA_MAGIC_BEANS) {
                            Inventory_ChangeAmmo(ITEM_MAGIC_BEANS, -1);
                            Player_SetAction_PreserveItemAction(play, this, Player_Action_17, 0);
                            this->yaw = talkActor->yawTowardsPlayer + 0x8000;
                            this->actor.shape.rot.y = this->yaw;
                            if (talkActor->xzDistToPlayer < 40.0f) {
                                Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_normal_backspace);
                                Player_AnimReplace_Setup(play, this,
                                                         ANIM_FLAG_1 | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE);
                            } else {
                                Player_Anim_PlayOnceMorph(play, this, D_8085BE84[31][this->modelAnimType]);
                            }
                            this->stateFlags1 |= PLAYER_STATE1_20000000;
                            this->av2.actionVar2 = 80;
                            this->av1.actionVar1 = -1;
                            this->focusActor = this->talkActor;
                        } else {
                            this->csId = CS_ID_GLOBAL_TALK;
                        }

                        talkActor->flags |= ACTOR_FLAG_TALK;
                        this->actor.textId = 0;
                        this->focusActor = this->talkActor;
                    } else {
                        this->stateFlags1 |= (PLAYER_STATE1_20000000 | PLAYER_STATE1_10000000 | PLAYER_STATE1_TALKING);
                        this->csId = play->playerCsIds[PLAYER_CS_ID_ITEM_SHOW];
                        this->av1.actionVar1 = 1;
                        this->actor.textId = 0xFE;
                    }
                    this->actor.flags |= ACTOR_FLAG_TALK;
                    this->exchangeItemAction = this->itemAction;
                    if (this->av1.actionVar1 >= 0) {
                        Player_Anim_PlayOnce(play, this, D_8085D1F8[this->av1.actionVar1]);
                    }
                    func_8082DAD4(this);
                    return true;
                } else {
                    bottleAction = Player_BottleFromIA(this, this->itemAction);

                    if (bottleAction > PLAYER_BOTTLE_NONE) {
                        Player_StopCutscene(this);
                        if (bottleAction >= PLAYER_BOTTLE_FAIRY) {
                            Player_SetAction_PreserveItemAction(play, this, Player_Action_69, 0);
                            Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_bottle_bug_out);
                        } else if ((bottleAction > PLAYER_BOTTLE_EMPTY) && (bottleAction < PLAYER_BOTTLE_POE)) {
                            Player_SetAction_PreserveItemAction(play, this, Player_Action_70, 0);
                            Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_bottle_fish_out);
                            this->csId = play->playerCsIds[PLAYER_CS_ID_ITEM_BOTTLE];
                        } else {
                            Player_SetAction_PreserveItemAction(play, this, Player_Action_67, 0);
                            Player_Anim_PlayOnceMorphAdjusted(play, this,
                                                              (this->transformation == PLAYER_FORM_DEKU)
                                                                  ? &gPlayerAnim_pn_drinkstart
                                                                  : &gPlayerAnim_link_bottle_drink_demo_start);
                        }
                    } else {
                        Actor* ocarinaInteractionActor = this->ocarinaInteractionActor;

                        if ((ocarinaInteractionActor == NULL) || (ocarinaInteractionActor->id == ACTOR_EN_ZOT) ||
                            (ocarinaInteractionActor->csId == CS_ID_NONE)) {
                            if (!func_808323C0(this, play->playerCsIds[PLAYER_CS_ID_ITEM_OCARINA])) {
                                return false;
                            }
                        } else {
                            this->csId = CS_ID_NONE;
                        }
                        Player_SetAction_PreserveItemAction(play, this, Player_Action_63, 0);
                        if ((this->skelAnime.playSpeed < 0.0f) ||
                            ((!BEN_ANIM_EQUAL(this->skelAnime.animation, D_8085D17C[this->transformation])) &&
                             (!BEN_ANIM_EQUAL(this->skelAnime.animation, D_8085D190[this->transformation])))) {
                            Player_Anim_PlayOnceAdjusted(play, this, D_8085D17C[this->transformation]);
                        }
                        this->stateFlags2 |= PLAYER_STATE2_USING_OCARINA;
                        if (ocarinaInteractionActor != NULL) {
                            this->actor.flags |= ACTOR_FLAG_OCARINA_INTERACTION;
                            if (ocarinaInteractionActor->id == ACTOR_EN_ZOT) {
                                // Delays setting `ACTOR_FLAG_OCARINA_INTERACTION` until a Zora guitar strum.
                                // Uses a negative xzDist to signal this special case (normally unobtainable xzDist).
                                // See `func_80852290`.
                                this->ocarinaInteractionDistance = -1.0f;
                            } else {
                                ocarinaInteractionActor->flags |= ACTOR_FLAG_OCARINA_INTERACTION;
                            }
                        }
                    }
                }
            } else {
                if (func_8083868C(play, this) != CAM_MODE_NORMAL) {
                    Player_StopCutscene(this);
                    if (!(this->stateFlags1 & PLAYER_STATE1_800000)) {
                        Player_SetAction(play, this, Player_Action_43, 1);
                        this->av2.actionVar2 = 13;
                        func_80836D8C(this);
                        if (this->unk_AA5 == PLAYER_UNKAA5_2) {
                            play->actorCtx.flags |= ACTORCTX_FLAG_PICTO_BOX_ON;
                        }
                    }
                    this->stateFlags1 |= PLAYER_STATE1_100000;
                    Audio_PlaySfx(NA_SE_SY_CAMERA_ZOOM_UP);
                    Player_StopHorizontalMovement(this);
                    return true;
                }
                this->unk_AA5 = PLAYER_UNKAA5_0;
                Audio_PlaySfx(NA_SE_SY_ERROR);
                return false;
            }
            this->stateFlags1 |= (PLAYER_STATE1_20000000 | PLAYER_STATE1_10000000);
            func_8082DAD4(this);
        }
        return true;
    }
    return false;
}

s32 Player_ActionHandler_Talk(Player* this, PlayState* play) {
    if (gSaveContext.save.saveInfo.playerData.health != 0) {
        Actor* talkOfferActor = this->talkActor;
        Actor* lockOnActor = this->focusActor;
        Actor* cUpTalkActor = NULL;
        s32 forceTalkToTatl = false;
        s32 canTalkToLockOnWithCUp = false;

        if (this->tatlActor != NULL) {
            canTalkToLockOnWithCUp =
                (lockOnActor != NULL) &&
                (CHECK_FLAG_ALL(lockOnActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_WITH_C_UP) ||
                 (lockOnActor->hintId != TATL_HINT_ID_NONE));

            if (canTalkToLockOnWithCUp || (this->tatlTextId != 0)) {
                //! @bug The comparison `((ABS_ALT(this->tatlTextId) & 0xFF00) != 0x10000)` always evaluates to `true`
                // Likely changed 0x200 -> 0x10000 to disable this check from OoT
                forceTalkToTatl = (this->tatlTextId < 0) && ((ABS_ALT(this->tatlTextId) & 0xFF00) != 0x10000);

                if (forceTalkToTatl || !canTalkToLockOnWithCUp) {
                    // If `lockOnActor` can't be talked to with c-up, the only option left is Tatl
                    cUpTalkActor = this->tatlActor;
                    if (forceTalkToTatl) {
                        // Clearing these pointers guarantees that `cUpTalkActor` will take priority
                        lockOnActor = NULL;
                        talkOfferActor = NULL;
                    }
                } else {
                    // Tatl is not the talk actor, so the only option left for talking with c-up is `lockOnActor`
                    // (though, `lockOnActor` may be NULL at this point).
                    cUpTalkActor = lockOnActor;
                }
            }
        }

        if ((talkOfferActor != NULL) || (cUpTalkActor != NULL)) {
            if ((lockOnActor != NULL) && (lockOnActor != talkOfferActor) && (lockOnActor != cUpTalkActor)) {
                goto dont_talk;
            }

            if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
                if ((this->heldActor == NULL) ||
                    (!forceTalkToTatl && (talkOfferActor != this->heldActor) && (cUpTalkActor != this->heldActor) &&
                     ((talkOfferActor == NULL) || !(talkOfferActor->flags & ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED)))) {
                    goto dont_talk;
                }
            }

            // FAKE: used to maintain matching using goto's. Goto's not required, but improves readability.
            if (1) {}
            if (1) {}

            if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                if (!(this->stateFlags1 & PLAYER_STATE1_800000) && !func_801242B4(this)) {
                    goto dont_talk;
                }
            }

            if (talkOfferActor != NULL) {
                // At this point the talk offer can be accepted.
                // "Speak" or "Check" will appear on the A button in the HUD.
                if ((lockOnActor == NULL) || (lockOnActor == talkOfferActor)) {
                    this->stateFlags2 |= PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER;
                }

                if (!CutsceneManager_IsNext(CS_ID_GLOBAL_TALK)) {
                    return false;
                }

                if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A) ||
                    (talkOfferActor->flags & ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED)) {
                    // Talk Offer has been accepted.
                    // Clearing `cUpTalkActor` guarantees that `talkOfferActor` is the actor that will be spoken to
                    cUpTalkActor = NULL;
                } else if (cUpTalkActor == NULL) {
                    return false;
                }
            }

            if (cUpTalkActor != NULL) {
                if (!forceTalkToTatl) {
                    this->stateFlags2 |= PLAYER_STATE2_200000;
                    // This code is the same as the OoT code, except for the
                    // !CutsceneManager_IsNext(CS_ID_GLOBAL_TALK), which is what prevented Tatl ISG from
                    // working
                    bool vanillaCondition = !CutsceneManager_IsNext(CS_ID_GLOBAL_TALK);
                    if (GameInteractor_Should(VB_TATL_CONVERSATION_AVAILABLE, vanillaCondition) ||
                        !CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_CUP)) {
                        return false;
                    }
                }

                talkOfferActor = cUpTalkActor;
                this->talkActor = NULL;

                if (forceTalkToTatl || !canTalkToLockOnWithCUp) {
                    cUpTalkActor->textId = ABS_ALT(this->tatlTextId);
                } else if (cUpTalkActor->hintId != 0xFF) {
                    cUpTalkActor->textId = cUpTalkActor->hintId + 0x1900;
                }
            }

            // `sSavedCurrentMask` saves the current mask just before the current action runs on this frame.
            // This saved mask value is then restored just before starting a conversation.
            //
            // This handles an edge case where a conversation is started on the same frame that a mask was taken on or
            // off. Because Player updates early before most actors, the text ID being offered comes from the previous
            // frame. If a mask was taken on or off the same frame this function runs, the wrong text will be used.
            this->currentMask = sSavedCurrentMask;
            gSaveContext.save.equippedMask = this->currentMask;

            Player_StartTalking(play, talkOfferActor);

            return true;
        }
    }

dont_talk:
    return false;
}

s32 Player_ActionHandler_0(Player* this, PlayState* play) {
    if (this->unk_AA5 != PLAYER_UNKAA5_0) {
        Player_ActionHandler_13(this, play);
        return true;
    } else if ((this->focusActor != NULL) &&
               (CHECK_FLAG_ALL(this->focusActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_WITH_C_UP) ||
                (this->focusActor->hintId != TATL_HINT_ID_NONE))) {
        this->stateFlags2 |= PLAYER_STATE2_200000;
    } else if (GameInteractor_Should(VB_FIRST_PERSON_CAMERA,
                                     (this->tatlTextId == 0) && !Player_CheckHostileLockOn(this) &&
                                         CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_CUP) &&
                                         !func_80831814(this, play, PLAYER_UNKAA5_1))) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
    }
    return false;
}

// Jumpslash/Jumpkick start
void func_808395F0(PlayState* play, Player* this, PlayerMeleeWeaponAnimation meleeWeaponAnim, f32 linearVelocity,
                   f32 yVelocity) {
    if (this->transformation == PLAYER_FORM_ZORA) {
        linearVelocity *= 1.1f;
        meleeWeaponAnim = PLAYER_MWA_ZORA_JUMPKICK_START;
        yVelocity *= 0.9f;
    }

    func_80833864(play, this, meleeWeaponAnim);
    Player_SetAction(play, this, Player_Action_29, 0);
    this->stateFlags3 |= PLAYER_STATE3_2;
    this->speedXZ = linearVelocity;
    this->yaw = this->actor.shape.rot.y;
    this->actor.velocity.y = yVelocity;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    Player_AnimSfx_PlayFloorJump(this);
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_L);
}

s32 func_808396B8(PlayState* play, Player* this) {
    if (!(this->stateFlags1 & PLAYER_STATE1_400000) &&
        (((this->actor.id != ACTOR_PLAYER) && CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_B)) ||
         ((Player_GetMeleeWeaponHeld(this) != PLAYER_MELEEWEAPON_NONE) &&
          ((this->transformation != PLAYER_FORM_GORON) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) &&
          ((this->transformation != PLAYER_FORM_ZORA) || !(this->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN)) &&
          sPlayerUseHeldItem))) {
        return true;
    }

    return false;
}

s32 func_80839770(Player* this, PlayState* play) {
    if (func_808396B8(play, this)) {
        if ((this->transformation != PLAYER_FORM_GORON) && (sPlayerFloorType != FLOOR_TYPE_7)) {
            func_808395F0(play, this,
                          (this->transformation == PLAYER_FORM_ZORA) ? PLAYER_MWA_ZORA_JUMPKICK_START
                                                                     : PLAYER_MWA_JUMPSLASH_START,
                          3.0f, 4.5f);
            return true;
        }
    }
    return false;
}

s32 func_80839800(Player* this, PlayState* play) {
    if ((this->controlStickDirections[this->controlStickDataIndex] == PLAYER_STICK_DIR_FORWARD) &&
        (sPlayerFloorType != FLOOR_TYPE_7)) {
        func_80836B3C(play, this, 0.0f);
        return true;
    }
    return false;
}

void func_80839860(Player* this, PlayState* play, s32 controlStickDirection) {
    s32 pad;
    f32 speed;

    if (!(controlStickDirection & 1)) {
        // forwards, backwards, or none
        speed = 5.8f;
    } else {
        // left or right
        speed = 3.5f;
    }

    if (this->currentBoots == PLAYER_BOOTS_GIANT) {
        speed /= 2.0f;
    }

    //! FAKE
    if (controlStickDirection == PLAYER_STICK_DIR_BACKWARD) {}

    func_80834D50(play, this, D_8085C2A4[controlStickDirection].unk_0, speed, NA_SE_VO_LI_SWORD_N);

    this->av2.actionVar2 = 1;
    this->av1.actionVar1 = controlStickDirection;

    this->yaw = this->actor.shape.rot.y + (controlStickDirection << 0xE);

    if (!(controlStickDirection & 1)) {
        // forwards, backwards, or none
        this->speedXZ = 6.0f;
    } else {
        // left or right
        this->speedXZ = 8.5f;
    }

    this->stateFlags2 |= PLAYER_STATE2_80000;
    Player_PlaySfx(this, ((controlStickDirection << 0xE) == (PLAYER_STICK_DIR_BACKWARD << 0xE)) ? NA_SE_PL_ROLL
                                                                                                : NA_SE_PL_SKIP);
}

void func_80839978(PlayState* play, Player* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->itemAction = PLAYER_IA_OCARINA;
        Player_SetAction_PreserveItemAction(play, this, Player_Action_11, 0);
        Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_clink_normal_okarina_walk);
        Player_AnimReplace_Setup(play, this, ANIM_FLAG_4 | ANIM_FLAG_200);
        this->stateFlags3 |= PLAYER_STATE3_20000000;
        this->unk_B48 = this->speedXZ;
        Audio_PlayFanfare(NA_BGM_BREMEN_MARCH);
    }
}

void func_80839A10(PlayState* play, Player* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->itemAction = PLAYER_IA_NONE;
        Player_SetAction_PreserveItemAction(play, this, Player_Action_12, 0);
        Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_alink_dance_loop);
        this->stateFlags2 |= PLAYER_STATE2_2000000;
        Audio_PlayFanfare(NA_BGM_KAMARO_DANCE);
    }
}

s32 func_80839A84(PlayState* play, Player* this) {
    if (this->transformation == PLAYER_FORM_DEKU) {
        if (func_80836DC0(play, this)) {
            return true;
        }
    } else {
        return false;
    }

    Player_SetAction(play, this, Player_Action_95, 0);
    this->stateFlags1 &= ~(PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE);
    this->unk_ADC = 4;
    func_808373A4(play, this);
    return true;
}

s32 Player_ActionHandler_10(Player* this, PlayState* play) {
    if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A) && (play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) &&
        (sPlayerFloorType != FLOOR_TYPE_7) && (sPlayerFloorEffect != FLOOR_EFFECT_1)) {
        s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];

        if (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) {
            if (Player_IsZTargeting(this)) {
                if (this->actor.category != ACTORCAT_PLAYER) {
                    if (controlStickDirection <= PLAYER_STICK_DIR_NONE) {
                        func_80834DB8(this, &gPlayerAnim_link_normal_jump, REG(69) / 100.0f, play);
                    } else {
                        func_80836B3C(play, this, 0.0f);
                    }
                } else if (GameInteractor_Should(VB_START_JUMPSLASH,
                                                 !(this->stateFlags1 & PLAYER_STATE1_8000000) &&
                                                     (Player_GetMeleeWeaponHeld(this) != PLAYER_MELEEWEAPON_NONE) &&
                                                     Player_CanUpdateItems(this) &&
                                                     (this->transformation != PLAYER_FORM_GORON))) {
                    func_808395F0(play, this, PLAYER_MWA_JUMPSLASH_START, 5.0f, 5.0f);
                } else if (!func_80839A84(play, this)) {
                    func_80836B3C(play, this, 0.0f);
                }

                return true;
            }
        } else {
            func_80839860(this, play, controlStickDirection);
            return true;
        }
    }

    return false;
}

void func_80839CD8(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;
    f32 var_fv0 = this->unk_B38 - 3.0f;

    if (var_fv0 < 0.0f) {
        var_fv0 += 29.0f;
    }

    if (var_fv0 < 14.0f) {
        anim = D_8085BE84[PLAYER_ANIMGROUP_walk_endL][this->modelAnimType];
        var_fv0 = 11.0f - var_fv0;
        if (var_fv0 < 0.0f) {
            var_fv0 = -var_fv0 * 1.375f;
        }
        var_fv0 /= 11.0f;
    } else {
        anim = D_8085BE84[PLAYER_ANIMGROUP_walk_endR][this->modelAnimType];
        var_fv0 = 26.0f - var_fv0;
        if (var_fv0 < 0.0f) {
            var_fv0 = -var_fv0 * 2;
        }
        var_fv0 /= 12.0f;
    }

    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, Animation_GetLastFrame(anim),
                           ANIMMODE_ONCE, 4.0f * var_fv0);
    this->yaw = this->actor.shape.rot.y;
}

void func_80839E3C(Player* this, PlayState* play) {
    func_808369F4(this, play);
    func_80839CD8(this, play);
}

void func_80839E74(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_Idle, 1);
    Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
    this->yaw = this->actor.shape.rot.y;
}

void func_80839ED0(Player* this, PlayState* play) {
    if (!(this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT) && (Player_Action_64 != this->actionFunc) &&
        !func_8083213C(this)) {
        func_80836D8C(this);
        if (!(this->stateFlags1 & PLAYER_STATE1_TALKING)) {
            if (func_801242B4(this)) {
                func_808353DC(play, this);
            } else {
                func_80836988(this, play);
            }
        }
        if (this->unk_AA5 < PLAYER_UNKAA5_5) {
            this->unk_AA5 = PLAYER_UNKAA5_0;
        }
    }
    this->stateFlags1 &= ~(PLAYER_STATE1_4 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 | PLAYER_STATE1_100000);
}

s32 func_80839F98(PlayState* play, Player* this) {
    if (!(this->stateFlags1 & PLAYER_STATE1_8000000)) {
        if (this->speedXZ != 0.0f) {
            func_80836B3C(play, this, 0.0f);
            return true;
        }
        func_80836AD8(play, this);
        PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_pg_maru_change, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f,
                               7.0f, ANIMMODE_ONCE, 0.0f);
        return true;
    }
    return false;
}

// Toggles swimming/walking underwater as Zora
void func_8083A04C(Player* this) {
    if (this->currentBoots == PLAYER_BOOTS_ZORA_UNDERWATER) {
        if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A)) {
            this->currentBoots = PLAYER_BOOTS_ZORA_LAND;
        }
        if (Player_Action_54 == this->actionFunc) {
            this->av2.actionVar2 = 20;
        }
    } else {
        if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_B)) {
            this->currentBoots = PLAYER_BOOTS_ZORA_UNDERWATER;
        }
    }
}

s32 Player_ActionHandler_14(Player* this, PlayState* play) {
    if (!sUpperBodyIsBusy && (this->transformation == PLAYER_FORM_ZORA)) {
        func_8083A04C(this);
    }
    return false;
}

s32 Player_ActionHandler_6(Player* this, PlayState* play) {
    if (!sUpperBodyIsBusy && !(this->stateFlags1 & PLAYER_STATE1_800000) && !Player_UpdateHostileLockOn(this)) {
        if ((this->transformation == PLAYER_FORM_ZORA) && (this->stateFlags1 & PLAYER_STATE1_8000000)) {
            func_8083A04C(this);
        } else if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A) && !Player_UpdateHostileLockOn(this)) {
            if (this->transformation == PLAYER_FORM_GORON) {
                if (func_80839F98(play, this)) {
                    return true;
                }
            } else if (func_80839A84(play, this) || func_80839800(this, play)) {
                return true;
            }

            if ((this->putAwayCooldownTimer == 0) && (this->heldItemAction >= PLAYER_IA_SWORD_KOKIRI) &&
                GameInteractor_Should(VB_SHOULD_PUTAWAY, (this->transformation != PLAYER_FORM_FIERCE_DEITY))) {
                Player_UseItem(play, this, ITEM_NONE);
            } else {
                this->stateFlags2 ^= PLAYER_STATE2_100000;
            }
        }
    }

    return false;
}

s32 Player_ActionHandler_11(Player* this, PlayState* play) {
    if (GameInteractor_Should(VB_SHIELD_FROM_BUTTON_HOLD, CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_R)) &&
        (this->unk_AA5 == PLAYER_UNKAA5_0) && (play->bButtonAmmoPlusOne == 0)) {
        if (Player_IsGoronOrDeku(this) ||
            ((((this->transformation == PLAYER_FORM_ZORA) &&
               !(this->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN)) ||
              ((this->transformation == PLAYER_FORM_HUMAN) && (this->currentShield != PLAYER_SHIELD_NONE))) &&
             !Player_FriendlyLockOnOrParallel(this) && (this->focusActor == NULL))) {
            func_8082DC38(this);
            Player_DetachHeldActor(play, this);
            if (Player_SetAction(play, this, Player_Action_18, 0)) {
                this->stateFlags1 |= PLAYER_STATE1_400000;
                if (this->transformation != PLAYER_FORM_GORON) {
                    PlayerAnimationHeader* anim;
                    f32 endFrame;

                    if (!Player_IsGoronOrDeku(this)) {
                        Player_SetModelsForHoldingShield(this);
                        anim = D_8085BE84[PLAYER_ANIMGROUP_defense][this->modelAnimType];
                    } else {
                        anim = (this->transformation == PLAYER_FORM_DEKU) ? &gPlayerAnim_pn_gurd
                                                                          : &gPlayerAnim_clink_normal_defense_ALL;
                    }

                    if (!BEN_ANIM_EQUAL(anim, this->skelAnime.animation)) {
                        if (Player_CheckHostileLockOn(this)) {
                            this->unk_B3C = 1.0f;
                        } else {
                            this->unk_B3C = 0.0f;
                            func_8082FC60(this);
                        }
                        this->upperLimbRot.x = 0;
                        this->upperLimbRot.y = 0;
                        this->upperLimbRot.z = 0;
                    }

                    endFrame = Animation_GetLastFrame(anim);
                    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED,
                                           (anim == &gPlayerAnim_pn_gurd) ? 0.0f : endFrame, endFrame, ANIMMODE_ONCE,
                                           0.0f);
                }
                func_80830AE8(this);
            }

            return true;
        }
    }

    return false;
}

s32 func_8083A4A4(Player* this, f32* speedTarget, s16* yawTarget, f32 decelerationRate) {
    s16 yawDiff = this->yaw - *yawTarget;

    if (ABS_ALT(yawDiff) > 0x6000) {
        if (Math_StepToF(&this->speedXZ, 0.0f, decelerationRate)) {
            *speedTarget = 0.0f;
            *yawTarget = this->yaw;
        } else {
            return true;
        }
    }
    return false;
}

void func_8083A548(Player* this) {
    if ((this->unk_ADC > 0) &&
        !GameInteractor_Should(VB_CHECK_HELD_ITEM_BUTTON_PRESS, CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_B),
                               sDpadItemButtons, sPlayerItemButtons)) {
        this->unk_ADC = -this->unk_ADC;
    }
}

s32 Player_ActionHandler_8(Player* this, PlayState* play) {
    if (GameInteractor_Should(VB_CHECK_HELD_ITEM_BUTTON_PRESS, CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_B),
                              sDpadItemButtons, sPlayerItemButtons)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_400000) &&
            (Player_GetMeleeWeaponHeld(this) != PLAYER_MELEEWEAPON_NONE)) {
            if ((this->unk_ADC > 0) && (((this->transformation == PLAYER_FORM_ZORA)) ||
                                        ((this->unk_ADC == 1) && (this->heldItemAction != PLAYER_IA_DEKU_STICK)))) {
                if (this->transformation == PLAYER_FORM_ZORA) {
                    func_80830E30(this, play);
                } else {
                    func_808335B0(play, this);
                }
                return true;
            }
        }
    } else {
        func_8083A548(this);
    }
    return false;
}

s32 func_8083A658(PlayState* play, Player* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Player_SetAction(play, this, Player_Action_64, 0);
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_light_bom);
        this->unk_AA5 = PLAYER_UNKAA5_0;
        return true;
    }
    return false;
}

struct_8085D200 D_8085D200[] = {
    { &gPlayerAnim_link_bottle_bug_miss, &gPlayerAnim_link_bottle_bug_in, 2, 3 },
    { &gPlayerAnim_link_bottle_fish_miss, &gPlayerAnim_link_bottle_fish_in, 5, 3 },
};

s32 func_8083A6C0(PlayState* play, Player* this) {
    if (sPlayerUseHeldItem) {
        if (Player_GetBottleHeld(this) > PLAYER_BOTTLE_NONE) {
            Player_SetAction(play, this, Player_Action_68, 0);
            if (this->actor.depthInWater > 12.0f) {
                this->av2.actionVar2 = 1;
            }
            Player_Anim_PlayOnceAdjusted(play, this, D_8085D200[this->av2.actionVar2].unk_0);
            Player_PlaySfx(this, NA_SE_IT_SWORD_SWING);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_AUTO_JUMP);
            return true;
        }
        return false;
    }
    return false;
}

void func_8083A794(Player* this, PlayState* play) {
    if ((Player_Action_13 != this->actionFunc) && (Player_Action_14 != this->actionFunc)) {
        this->unk_B70 = 0;
        this->unk_B34 = 0.0f;
        this->unk_B38 = 0.0f;
        Player_Anim_PlayLoopMorph(play, this, D_8085BE84[PLAYER_ANIMGROUP_run][this->modelAnimType]);
    }

    Player_SetAction(play, this, Player_IsZTargeting(this) ? Player_Action_14 : Player_Action_13, 1);
}

void func_8083A844(Player* this, PlayState* play, s16 yaw) {
    this->yaw = yaw;
    this->actor.shape.rot.y = this->yaw;
    func_8083A794(this, play);
}

s32 func_8083A878(PlayState* play, Player* this, f32 arg2) {
    WaterBox* waterBox;
    f32 ySurface = this->actor.world.pos.y;

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &ySurface,
                             &waterBox)) {
        ySurface -= this->actor.world.pos.y;
        if (this->ageProperties->unk_24 <= ySurface) {
            Player_SetAction(play, this, Player_Action_55, 0);
            Player_Anim_PlayLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim);
            this->stateFlags1 |= (PLAYER_STATE1_8000000 | PLAYER_STATE1_20000000);
            this->av2.actionVar2 = 20;
            this->speedXZ = 2.0f;
            func_80123140(play, this);
            return false;
        }
    }
    func_80835324(play, this, arg2, this->actor.shape.rot.y);
    this->stateFlags1 |= PLAYER_STATE1_20000000;
    return true;
}

/**
 * Update for using telescopes. SCENE_AYASHIISHOP acts quite differently: it has a different camera mode and cannot use
 * zooming.
 *
 * - Stick inputs move the view; shape.rot.y is used as a base position which cannot be looked too far away from. (This
 * is not necessarily the same as the original angle of the spawn.)
 * - A can be used to zoom (except in SCENE_AYASHIISHOP)
 * - B exits, using the RESPAWN_MODE_DOWN entrance
 */
void func_8083A98C(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Player* this = (Player*)thisx;
    s32 camMode;

    if (play->csCtx.state != CS_STATE_IDLE) {
        return;
    }

    if (DECR(this->av2.actionVar2) != 0) {
        camMode = (play->sceneId != SCENE_AYASHIISHOP) ? CAM_MODE_FIRSTPERSON : CAM_MODE_DEKUHIDE;

        // Show controls overlay. SCENE_AYASHIISHOP does not have Zoom, so has a different one.
        if (this->av2.actionVar2 == 1) {
            Message_StartTextbox(play, (play->sceneId == SCENE_AYASHIISHOP) ? 0x2A00 : 0x5E6, NULL);
        }
    } else {
        sPlayerControlInput = play->state.input;
        if (play->view.fovy >= 25.0f) {
            s16 prevFocusX = thisx->focus.rot.x;
            s16 prevFocusY = thisx->focus.rot.y;
            s16 inputY;
            s16 inputX;
            s16 newYaw; // from base position shape.rot.y

            // Pitch:
            inputY = sPlayerControlInput->rel.stick_y * 4;
            inputY *= -GameInteractor_InvertControl(GI_INVERT_TELESCOPE_Y);
            // Add input, clamped to prevent turning too fast
            thisx->focus.rot.x += CLAMP(inputY, -0x12C, 0x12C);
            // Prevent looking too far up or down
            thisx->focus.rot.x = CLAMP(thisx->focus.rot.x, -0x2EE0, 0x2EE0);

            // Yaw: shape.rot.y is used as a fixed starting position
            inputX = sPlayerControlInput->rel.stick_x * -4;
            inputX *= GameInteractor_InvertControl(GI_INVERT_TELESCOPE_X);
            // Start from current position: no input -> no change
            newYaw = thisx->focus.rot.y - thisx->shape.rot.y;
            // Add input, clamped to prevent turning too fast
            newYaw += CLAMP(inputX, -0x12C, 0x12C);
            // Prevent looking too far left or right of base position
            newYaw = CLAMP(newYaw, -0x3E80, 0x3E80);
            thisx->focus.rot.y = thisx->shape.rot.y + newYaw;

            if (play->sceneId == SCENE_00KEIKOKU) {
                f32 focusDeltaX = (s16)(thisx->focus.rot.x - prevFocusX);
                f32 focusDeltaY = (s16)(thisx->focus.rot.y - prevFocusY);

                Audio_PlaySfx_AtPosWithFreq(&gSfxDefaultPos, NA_SE_PL_TELESCOPE_MOVEMENT - SFX_FLAG,
                                            sqrtf(SQ(focusDeltaX) + SQ(focusDeltaY)) / 300.0f);
            }
        }

        if (play->sceneId == SCENE_AYASHIISHOP) {
            camMode = CAM_MODE_DEKUHIDE;
        } else if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A)) { // Zoom
            camMode = CAM_MODE_TARGET;
        } else {
            camMode = CAM_MODE_NORMAL;
        }

        // Exit
        if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_B)) {
            Message_CloseTextbox(play);

            if (play->sceneId == SCENE_00KEIKOKU) {
                gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = ENTRANCE(ASTRAL_OBSERVATORY, 2);
            } else {
                u16 entrance;

                if (play->sceneId == SCENE_AYASHIISHOP) {
                    entrance = ENTRANCE(CURIOSITY_SHOP, 3);
                } else {
                    entrance = ENTRANCE(PIRATES_FORTRESS_INTERIOR, 8);
                }
                gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = entrance;
            }

            func_80169EFC(play);
            gSaveContext.respawnFlag = -2;
            play->transitionType = TRANS_TYPE_CIRCLE;
        }
    }

    Camera_ChangeSetting(Play_GetCamera(play, CAM_ID_MAIN), CAM_SET_TELESCOPE);
    Camera_ChangeMode(Play_GetCamera(play, CAM_ID_MAIN), camMode);
}

// Set up using a telescope
void Player_StartMode_Telescope(PlayState* play, Player* this) {
    this->actor.update = func_8083A98C;
    this->actor.draw = NULL;
    if (play->sceneId == SCENE_00KEIKOKU) {
        this->actor.focus.rot.x = 0xBD8;
        this->actor.focus.rot.y = -0x4D74;
        this->av2.actionVar2 = 20;
    } else if (play->sceneId == SCENE_AYASHIISHOP) {
        this->actor.focus.rot.x = 0x9A6;
        this->actor.focus.rot.y = 0x2102;
        this->av2.actionVar2 = 2;
    } else {
        this->actor.focus.rot.x = 0x9A6;
        this->actor.focus.rot.y = 0x2102;
        this->av2.actionVar2 = 20;
    }
    play->actorCtx.flags |= ACTORCTX_FLAG_TELESCOPE_ON;
}

void Player_StartMode_B(PlayState* play, Player* this) {
    func_8085B384(this, play);
}

void Player_StartMode_D(PlayState* play, Player* this) {
    if (func_8083A878(play, this, 180.0f)) {
        this->av2.actionVar2 = -20;
    }
}

void Player_StartMode_E(PlayState* play, Player* this) {
    this->speedXZ = 2.0f;
    gSaveContext.entranceSpeed = 2.0f;

    if (func_8083A878(play, this, 120.0f)) {
        this->av2.actionVar2 = -15;
    }
}

void Player_StartMode_F(PlayState* play, Player* this) {
    if (gSaveContext.entranceSpeed < 0.1f) {
        gSaveContext.entranceSpeed = 0.1f;
    }

    this->speedXZ = gSaveContext.entranceSpeed;
    if (func_8083A878(play, this, 800.0f)) {
        this->av2.actionVar2 = -80.0f / this->speedXZ;
        if (this->av2.actionVar2 < -20) {
            this->av2.actionVar2 = -20;
        }
    }
}

void func_8083AECC(Player* this, s16 yaw, PlayState* play) {
    Player_SetAction(play, this, Player_Action_6, 1);
    PlayerAnimation_CopyJointToMorph(play, &this->skelAnime);
    this->unk_B38 = 0.0f;
    this->unk_B34 = 0.0f;
    this->yaw = yaw;
}

void func_8083AF30(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_5, 1);
    Player_Anim_PlayLoopMorph(play, this, D_8085BE84[PLAYER_ANIMGROUP_walk][this->modelAnimType]);
}

void func_8083AF8C(Player* this, s16 yaw, PlayState* play) {
    Player_SetAction(play, this, Player_Action_15, 1);
    PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_anchor_back_walk, PLAYER_ANIM_NORMAL_SPEED, 0.0f,
                           Animation_GetLastFrame(&gPlayerAnim_link_anchor_back_walk), ANIMMODE_ONCE, -6.0f);
    this->speedXZ = 8.0f;
    this->yaw = yaw;
}

void func_8083B030(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_9, 1);
    Player_Anim_PlayLoopMorph(play, this, D_8085BE84[PLAYER_ANIMGROUP_side_walkR][this->modelAnimType]);
    this->unk_B38 = 0.0f;
}

void func_8083B090(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_16, 1);
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, &gPlayerAnim_link_anchor_back_brake, 6.0f / 3.0f);
}

void Player_SetupTurnInPlace(PlayState* play, Player* this, s16 yaw) {
    this->yaw = yaw;

    Player_SetAction(play, this, Player_Action_TurnInPlace, 1);

    this->turnRate = 0x4B0;
    this->turnRate *= sWaterSpeedFactor; // slow turn rate by half when in water

    PlayerAnimation_Change(play, &this->skelAnime, D_8085BE84[PLAYER_ANIMGROUP_45_turn][this->modelAnimType],
                           PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void func_8083B1A0(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;

    Player_SetAction(play, this, Player_Action_Idle, 1);
    if (this->unk_B40 < 0.5f) {
        anim = D_8085BE84[PLAYER_ANIMGROUP_waitR2wait][this->modelAnimType];
    } else {
        anim = D_8085BE84[PLAYER_ANIMGROUP_waitL2wait][this->modelAnimType];
    }
    Player_Anim_PlayOnce(play, this, anim);
    this->yaw = this->actor.shape.rot.y;
}

void func_8083B23C(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_2, 1);
    Player_Anim_PlayOnceMorph(play, this, D_8085BE84[PLAYER_ANIMGROUP_wait2waitR][this->modelAnimType]);
    this->av2.actionVar2 = 1;
}

void func_8083B29C(Player* this, PlayState* play) {
    if (this->speedXZ != 0.0f) {
        func_8083A794(this, play);
    } else {
        func_8083B1A0(this, play);
    }
}

void func_8083B2E4(Player* this, PlayState* play) {
    if (this->speedXZ != 0.0f) {
        func_8083A794(this, play);
    } else {
        func_80836988(this, play);
    }
}

void func_8083B32C(PlayState* play, Player* this, f32 arg2) {
    this->stateFlags1 |= PLAYER_STATE1_40000;
    this->stateFlags1 &= ~PLAYER_STATE1_8000000;
    func_8082DC64(play, this);

    if (func_80837730(play, this, arg2, 500)) {
        Player_PlaySfx(this, NA_SE_EV_JUMP_OUT_WATER);
    }
    func_80123140(play, this);
}

s32 func_8083B3B4(PlayState* play, Player* this, Input* input) {
    if ((!(this->stateFlags1 & PLAYER_STATE1_400) && !(this->stateFlags2 & PLAYER_STATE2_400) &&
         (this->transformation != PLAYER_FORM_ZORA)) &&
        ((input == NULL) ||
         ((((this->interactRangeActor == NULL) || (this->interactRangeActor->id != ACTOR_EN_ZOG)) &&
           CHECK_BTN_ALL(input->press.button, BTN_A)) &&
          ((ABS_ALT(this->unk_AAA) < 0x2EE0) && (this->currentBoots < PLAYER_BOOTS_ZORA_UNDERWATER) &&
           ((s32)SurfaceType_GetConveyorSpeed(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) <=
            CONVEYOR_SPEED_SLOW))))) {
        if (Player_Action_CsAction != this->actionFunc) {
            Player_SetAction(play, this, Player_Action_59, 0);
        }

        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_swimer_swim_deep_start);
        this->unk_AAA = 0;
        this->stateFlags2 |= PLAYER_STATE2_400;
        this->actor.velocity.y = 0.0f;
        if (input != NULL) {
            this->stateFlags2 |= PLAYER_STATE2_800;
            Player_PlaySfx(this, NA_SE_PL_DIVE_BUBBLE);
        }

        return true;
    }

    if ((this->transformation != PLAYER_FORM_DEKU) &&
        ((this->stateFlags1 & PLAYER_STATE1_400) ||
         ((this->stateFlags2 & PLAYER_STATE2_400) &&
          (((Player_Action_56 != this->actionFunc) && !(this->stateFlags3 & PLAYER_STATE3_8000)) ||
           (this->unk_AAA < -0x1555)))) &&
        ((this->actor.depthInWater - this->actor.velocity.y) < this->ageProperties->unk_30)) {
        s32 temp_v0_3;
        s16 sp2A;
        f32 sp24;

        this->stateFlags2 &= ~PLAYER_STATE2_400;
        func_8082DC64(play, this);
        temp_v0_3 = func_80837730(play, this, this->actor.velocity.y, 0x1F4);
        if (this->stateFlags3 & PLAYER_STATE3_8000) {
            sp2A = this->unk_B86[1];
            sp24 = this->unk_B48 * 1.5f;
            Player_SetAction(play, this, Player_Action_28, 1);
            this->stateFlags3 |= PLAYER_STATE3_8000;
            this->stateFlags1 &= ~PLAYER_STATE1_8000000;
            sp24 = CLAMP_MAX(sp24, 13.5f);
            this->speedXZ = Math_CosS(this->unk_AAA) * sp24;
            this->actor.velocity.y = -Math_SinS(this->unk_AAA) * sp24;
            this->unk_B86[1] = sp2A;
            Player_PlaySfx(this, NA_SE_EV_JUMP_OUT_WATER);
            return true;
        }

        if (temp_v0_3) {
            Player_PlaySfx(this, NA_SE_PL_FACE_UP);
        } else {
            Player_PlaySfx(this, NA_SE_PL_FACE_UP);
        }

        if (input != NULL) {
            Player_SetAction(play, this, Player_Action_60, 1);
            if (this->stateFlags1 & PLAYER_STATE1_400) {
                this->stateFlags1 |= (PLAYER_STATE1_400 | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_20000000);
            }
            this->av2.actionVar2 = 2;
        }

        Player_Anim_PlayOnceMorph(play, this,
                                  (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)
                                      ? &gPlayerAnim_link_swimer_swim_get
                                      : &gPlayerAnim_link_swimer_swim_deep_end);
        return true;
    }

    return false;
}

void func_8083B73C(PlayState* play, Player* this, s16 yaw) {
    Player_SetAction(play, this, Player_Action_57, 0);
    Player_Anim_PlayLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim);
    this->actor.shape.rot.y = yaw;
    this->yaw = yaw;
}

void func_8083B798(PlayState* play, Player* this) {
    if (this->transformation == PLAYER_FORM_ZORA) {
        Player_SetAction(play, this, Player_Action_57, 0);
        PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_swimer_swim, PLAYER_ANIM_NORMAL_SPEED,
                               Animation_GetLastFrame(&gPlayerAnim_link_swimer_swim), 0.0f, ANIMMODE_LOOP, 0.0f);
        this->unk_B48 = 2.0f;
    } else {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_swimer_swim);
        this->av2.actionVar2 = 1;
    }

    this->unk_AAA = 0x3E80;
}

void func_8083B850(PlayState* play, Player* this) {
    this->currentBoots = PLAYER_BOOTS_ZORA_LAND;
    this->prevBoots = PLAYER_BOOTS_ZORA_LAND;
    Player_SetAction(play, this, Player_Action_56, 0);
    this->unk_B48 = sqrtf(SQ(this->speedXZ) + SQ(this->actor.velocity.y));
    Player_OverrideBlureColors(play, this, 1, 8);
    this->currentBoots = PLAYER_BOOTS_ZORA_LAND;
    this->prevBoots = PLAYER_BOOTS_ZORA_LAND;
}

void func_8083B8D0(PlayState* play, Player* this) {
    if (func_80837730(play, this, this->actor.velocity.y, 500)) {
        Player_PlaySfx(this, NA_SE_EV_DIVE_INTO_WATER);
        if (this->fallDistance > 800) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_CLIMB_END);
        }
    }
}

void func_8083B930(PlayState* play, Player* this) {
    PlayerAnimationHeader* var_a2;

    if ((this->currentBoots < PLAYER_BOOTS_ZORA_UNDERWATER) || !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
        (Player_Action_96 == this->actionFunc)) {
        func_8082DE50(play, this);

        if (Player_Action_28 == this->actionFunc) {
            func_8083B850(play, this);
            this->stateFlags3 |= PLAYER_STATE3_8000;
        } else if ((this->transformation == PLAYER_FORM_ZORA) && (Player_Action_27 == this->actionFunc)) {
            func_8083B850(play, this);
            this->stateFlags3 |= PLAYER_STATE3_8000;
            Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_pz_fishswim);
        } else if ((this->currentBoots < PLAYER_BOOTS_ZORA_UNDERWATER) && (this->stateFlags2 & PLAYER_STATE2_400)) {
            this->stateFlags2 &= ~PLAYER_STATE2_400;
            func_8083B3B4(play, this, NULL);
            this->av1.actionVar1 = 1;
        } else if (Player_Action_27 == this->actionFunc) {
            Player_SetAction(play, this, Player_Action_59, 0);
            func_8083B798(play, this);
        } else {
            Player_SetAction(play, this, Player_Action_54, 1);
            Player_Anim_PlayOnceMorph(play, this,
                                      (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)
                                          ? &gPlayerAnim_link_swimer_wait2swim_wait
                                          : &gPlayerAnim_link_swimer_land2swim_wait);
        }
    }
    if (!(this->stateFlags1 & PLAYER_STATE1_8000000) || (this->actor.depthInWater < this->ageProperties->unk_2C)) {
        func_8083B8D0(play, this);
    }

    this->stateFlags1 |= PLAYER_STATE1_8000000;
    this->stateFlags2 |= PLAYER_STATE2_400;
    this->stateFlags1 &= ~(PLAYER_STATE1_40000 | PLAYER_STATE1_80000);

    this->unk_AEC = 0.0f;
    func_80123140(play, this);
}

void func_8083BB4C(PlayState* play, Player* this) {
    f32 sp1C = this->actor.depthInWater - this->ageProperties->unk_2C;

    if (sp1C < 0.0f) {
        this->underwaterTimer = 0;
        if ((this->transformation == PLAYER_FORM_ZORA) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            this->currentBoots = PLAYER_BOOTS_ZORA_LAND;
        }
        Audio_SetBaseFilter(0);
    } else {
        Audio_SetBaseFilter(0x20);
        if ((this->transformation == PLAYER_FORM_ZORA) || (sp1C < 10.0f)) {
            this->underwaterTimer = 0;
        } else if (this->underwaterTimer < 300) {
            this->underwaterTimer++;
        }
    }

    if ((this->actor.parent == NULL) && (Player_Action_33 != this->actionFunc) &&
        (Player_Action_49 != this->actionFunc) &&
        ((Player_Action_28 != this->actionFunc) || (this->actor.velocity.y < -2.0f))) {
        if (this->ageProperties->unk_2C < this->actor.depthInWater) {
            if (this->transformation == PLAYER_FORM_GORON) {
                func_80834140(play, this, &gPlayerAnim_link_swimer_swim_down);
                func_808345C8();
                func_8083B8D0(play, this);
            } else if (this->transformation == PLAYER_FORM_DEKU) {
                if (this->remainingHopsCounter != 0) {
                    func_808373F8(play, this, NA_SE_VO_LI_AUTO_JUMP);
                } else {
                    if ((play->sceneId == SCENE_20SICHITAI) && (this->unk_3CF == 0)) {
                        if (CHECK_EVENTINF(EVENTINF_50)) {
                            play->nextEntrance = ENTRANCE(TOURIST_INFORMATION, 2);
                        } else {
                            play->nextEntrance = ENTRANCE(TOURIST_INFORMATION, 1);
                        }
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
                        this->stateFlags1 |= PLAYER_STATE1_200;
                        Audio_PlaySfx(NA_SE_SY_DEKUNUTS_JUMP_FAILED);
                    } else if ((this->unk_3CF == 0) &&
                               ((play->sceneId == SCENE_30GYOSON) || (play->sceneId == SCENE_31MISAKI) ||
                                (play->sceneId == SCENE_TORIDE))) {
                        func_80169EFC(play);
                        func_808345C8();
                    } else {
                        Player_SetAction(play, this, Player_Action_1, 0);
                        this->stateFlags1 |= PLAYER_STATE1_20000000;
                    }
                    func_8083B8D0(play, this);
                }
            } else if (!(this->stateFlags1 & PLAYER_STATE1_8000000) ||
                       (((this->currentBoots < PLAYER_BOOTS_ZORA_UNDERWATER) ||
                         !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) &&
                        (Player_Action_43 != this->actionFunc) && (Player_Action_61 != this->actionFunc) &&
                        (Player_Action_62 != this->actionFunc) && (Player_Action_54 != this->actionFunc) &&
                        (Player_Action_57 != this->actionFunc) && (Player_Action_58 != this->actionFunc) &&
                        (Player_Action_59 != this->actionFunc) && (Player_Action_60 != this->actionFunc) &&
                        (Player_Action_55 != this->actionFunc) && (Player_Action_56 != this->actionFunc))) {
                func_8083B930(play, this);
            }
        } else if ((this->stateFlags1 & PLAYER_STATE1_8000000) &&
                   (this->actor.depthInWater < this->ageProperties->unk_24) &&
                   (((Player_Action_56 != this->actionFunc) && !(this->stateFlags3 & PLAYER_STATE3_8000)) ||
                    (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
            if (this->skelAnime.movementFlags == 0) {
                Player_SetupTurnInPlace(play, this, this->actor.shape.rot.y);
            }
            func_8083B32C(play, this, this->actor.velocity.y);
        }
    }
}

void func_8083BF54(PlayState* play, Player* this) {
    Vec3f sp84;
    s32 temp_v0;
    s32 var_a2;

    this->actor.terminalVelocity = -20.0f;
    this->actor.gravity = REG(68) / 100.0f;

    var_a2 = false;
    temp_v0 = func_808340D4(sPlayerFloorType);

    if (temp_v0 || ((var_a2 = (sPlayerFloorType == FLOOR_TYPE_14) || (sPlayerFloorType == FLOOR_TYPE_15)) ||
                    (sPlayerFloorType == FLOOR_TYPE_13))) {
        f32 temp_fv1_2;
        f32 var_fa1;
        f32 var_ft4;
        f32 var_fv0;
        u16 sfxId;

        var_ft4 = fabsf(this->speedXZ + D_80862B3C) * 20.0f;
        if (temp_v0) {
            if (sPlayerFloorType == FLOOR_TYPE_4) {
                var_fa1 = 1300.0f;
            } else if (sPlayerFloorType == FLOOR_TYPE_7) {
                var_fa1 = 20000.0f;
                var_ft4 = 0.0f;
            } else {
                var_fa1 = 10000.0f;
                var_ft4 *= 1.6f;
            }
            sfxId = NA_SE_PL_SINK_ON_SAND - SFX_FLAG;
        } else if (var_a2) {
            if (sPlayerFloorType == FLOOR_TYPE_14) {
                var_fa1 = 400.0f;
                var_ft4 *= 10.0f;
            } else {
                var_fa1 = 1300.0f;
                var_ft4 = 0.0f;
            }
            sfxId = NA_SE_PL_SINK_ON_SNOW - SFX_FLAG;
        } else {
            var_fa1 = (this->transformation == PLAYER_FORM_GORON) ? 10000.0f : 1000.0f;
            var_ft4 = 0.0f;
            sfxId = NA_SE_PL_SINK_ON_SAND - SFX_FLAG;
        }

        var_fa1 = CLAMP_MIN(var_fa1, this->unk_AB8);

        var_fv0 = (sPlayerFloorType == FLOOR_TYPE_14) ? 200.0f : (var_fa1 - this->unk_AB8) * 0.02f;
        var_fv0 = CLAMP(var_fv0, 0.0f, 300.0f);

        temp_fv1_2 = this->unk_AB8;
        this->unk_AB8 += var_fv0 - var_ft4;
        this->unk_AB8 = CLAMP(this->unk_AB8, 0.0f, var_fa1);

        if ((this->speedXZ == 0.0f) && (fabsf(this->unk_AB8 - temp_fv1_2) > 2.0f)) {
            Actor_PlaySfx_Flagged2(&this->actor, sfxId);
        }

        this->actor.gravity -= this->unk_AB8 * 0.004f;
    } else {
        this->unk_AB8 = 0.0f;
    }

    if ((this->stateFlags3 & PLAYER_STATE3_10) && (this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
        if (this->actor.depthInWater < 50.0f) {
            f32 temp_fv1_5;
            Vec3f* bodyPartsPos;
            f32 var_fa0_3;
            f32 var_ft4_2;

            var_ft4_2 = fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].x - this->unk_D6C.x) +
                        fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].y - this->unk_D6C.y) +
                        fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].z - this->unk_D6C.z);
            var_ft4_2 = CLAMP_MAX(var_ft4_2, 4.0f);

            this->unk_AEC += var_ft4_2;
            if (this->unk_AEC > 15.0f) {
                this->unk_AEC = 0.0f;
                sp84.x = (Rand_ZeroOne() * 10.0f) + this->actor.world.pos.x;
                sp84.y = this->actor.world.pos.y + this->actor.depthInWater;
                sp84.z = (Rand_ZeroOne() * 10.0f) + this->actor.world.pos.z;

                EffectSsGRipple_Spawn(play, &sp84, 100, 500, 0);

                if ((this->speedXZ > 4.0f) && !func_801242B4(this) &&
                    ((this->actor.world.pos.y + this->actor.depthInWater) <
                     this->bodyPartsPos[PLAYER_BODYPART_WAIST].y)) {
                    func_80837730(play, this, 20.0f,
                                  (fabsf(this->speedXZ) * 50.0f) + (this->actor.depthInWater * 5.0f));
                } else if (this->stateFlags3 & PLAYER_STATE3_8000) {
                    s32 i;

                    var_fa0_3 = (this->actor.world.pos.y + this->actor.depthInWater) - 5.0f;
                    bodyPartsPos = this->bodyPartsPos;

                    for (i = 0; i < PLAYER_BODYPART_MAX; i++, bodyPartsPos++) {
                        temp_fv1_5 = bodyPartsPos->y - var_fa0_3;

                        if (temp_fv1_5 > 0.0f) {
                            func_80837730(play, this, 20.0f, fabsf(this->speedXZ) * 20.0f + (temp_fv1_5 * 10.0f));
                        }
                    }
                }
            }
        }

        if (this->ageProperties->unk_2C < this->actor.depthInWater) {
            s32 numBubbles = 0;
            s32 i;
            f32 var_fv1;

            var_fv1 = (this->stateFlags1 & PLAYER_STATE1_4000000)
                          ? -fabsf(this->speedXZ)
                          : ((Player_Action_56 == this->actionFunc)
                                 ? (ABS_ALT(this->unk_B8A) * -0.004f) + (this->unk_B48 * -0.38f)
                                 : this->actor.velocity.y);

            if ((var_fv1 > -1.0f) || ((this->currentBoots == PLAYER_BOOTS_ZORA_UNDERWATER) &&
                                      (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
                if (Rand_ZeroOne() < 0.2f) {
                    numBubbles = 1;
                }
            } else {
                numBubbles = var_fv1 * -0.3f;
                if (numBubbles > 8) {
                    numBubbles = 8;
                }
            }

            for (i = 0; i < numBubbles; i++) {
                EffectSsBubble_Spawn(play, &this->actor.world.pos, 20.0f, 10.0f, 20.0f, 0.13f);
            }
        }
    }
}

s32 func_8083C62C(Player* this, s32 arg1) {
    Actor* focusActor = this->focusActor;
    Vec3f headPos;
    s16 pitchTarget;
    s16 yawTarget;

    headPos.x = this->actor.world.pos.x;
    headPos.y = this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    headPos.z = this->actor.world.pos.z;

    pitchTarget = Math_Vec3f_Pitch(&headPos, &focusActor->focus.pos);
    yawTarget = Math_Vec3f_Yaw(&headPos, &focusActor->focus.pos);

    Math_SmoothStepToS(&this->actor.focus.rot.y, yawTarget, 4, 0x2710, 0);
    Math_SmoothStepToS(&this->actor.focus.rot.x, pitchTarget, 4, 0x2710, 0);

    this->unk_AA6_rotFlags |= UNKAA6_ROT_FOCUS_Y;

    return func_80832754(this, arg1);
}

Vec3f D_8085D218 = { 0.0f, 100.0f, 40.0f };

void func_8083C6E8(Player* this, PlayState* play) {
    if (this->focusActor != NULL) {
        if (func_800B7128(this) || func_8082EF20(this)) {
            func_8083C62C(this, true);
        } else {
            func_8083C62C(this, false);
        }
        return;
    }

    if (sPlayerFloorType == FLOOR_TYPE_11) {
        Math_SmoothStepToS(&this->actor.focus.rot.x, -0x4E20, 10, 0xFA0, 0x320);
    } else {
        s16 sp46 = 0;
        f32 yIntersect;
        Vec3f pos;
        s16 temp_v0;

        yIntersect = func_80835D2C(play, this, &D_8085D218, &pos);
        if (yIntersect > BGCHECK_Y_MIN) {
            temp_v0 = Math_Atan2S_XY(40.0f, this->actor.world.pos.y - yIntersect);
            sp46 = CLAMP(temp_v0, -0xFA0, 0xFA0);
        }
        this->actor.focus.rot.y = this->actor.shape.rot.y;
        Math_SmoothStepToS(&this->actor.focus.rot.x, sp46, 14, 0xFA0, 30);
    }

    func_80832754(this, func_800B7128(this) || func_8082EF20(this));
}

void func_8083C85C(Player* this) {
    Math_ScaledStepToS(&this->upperLimbRot.x, D_80862B3C * -500.0f, 0x384);
    this->headLimbRot.x = (-(f32)this->upperLimbRot.x * 0.5f);
    this->unk_AA6_rotFlags |= UNKAA6_ROT_HEAD_X | UNKAA6_ROT_UPPER_X;
}

void func_8083C8E8(Player* this, PlayState* play) {
    if (!func_800B7128(this) && !func_8082EF20(this) && ((this->speedXZ > 5.0f) || (D_80862B3C != 0.0f))) {
        s16 temp1;
        s16 temp2;

        temp1 = this->speedXZ * 200.0f;
        temp2 = BINANG_SUB(this->yaw, this->actor.shape.rot.y) * this->speedXZ * 0.1f;

        temp1 = CLAMP(temp1, -0xFA0, 0xFA0);

        temp1 += TRUNCF_BINANG(D_80862B3C * -500.0f);

        temp1 = CLAMP(temp1, -0x2EE0, 0x2EE0);

        temp2 = CLAMP(-temp2, -0xFA0, 0xFA0);

        Math_ScaledStepToS(&this->upperLimbRot.x, temp1, 0x384);
        this->headLimbRot.x = -(f32)this->upperLimbRot.x * 0.5f;
        Math_ScaledStepToS(&this->headLimbRot.z, temp2, 0x12C);
        Math_ScaledStepToS(&this->upperLimbRot.z, temp2, 0xC8);
        this->unk_AA6_rotFlags |= UNKAA6_ROT_HEAD_X | UNKAA6_ROT_HEAD_Z | UNKAA6_ROT_UPPER_X | UNKAA6_ROT_UPPER_Z;
    } else {
        func_8083C6E8(this, play);
    }
}

void func_8083CB04(Player* this, f32 arg1, s16 arg2, f32 arg3, f32 arg4, s16 arg5) {
    Math_AsymStepToF(&this->speedXZ, arg1, arg3, arg4);
    Math_ScaledStepToS(&this->yaw, arg2, arg5);
}

void func_8083CB58(Player* this, f32 arg1, s16 arg2) {
    func_8083CB04(this, arg1, arg2, REG(19) / 100.0f, 1.5f, REG(27));
}

s32 func_8083CBC4(Player* this, f32 arg1, s16 arg2, f32 arg3, f32 arg4, f32 arg5, s16 arg6) {
    s16 temp_v0 = this->yaw - arg2;

    if ((this->unk_B50 * 1.5f) < fabsf(this->speedXZ)) {
        arg5 *= 4.0f;
        arg3 *= 4.0f;
    }

    if (ABS_ALT(temp_v0) > 0x6000) {
        if (!Math_StepToF(&this->speedXZ, 0.0f, arg3)) {
            return false;
        }

        this->yaw = arg2;
    } else {
        Math_AsymStepToF(&this->speedXZ, arg1, arg4, arg5);
        Math_ScaledStepToS(&this->yaw, arg2, arg6);
    }

    return true;
}

struct_8085D224 D_8085D224[][2] = {
    {
        { &gPlayerAnim_link_uma_left_up, 35.17f, 6.6099997f },
        { &gPlayerAnim_link_uma_right_up, -34.16f, 7.91f },
    },
    {
        { &gPlayerAnim_cl_uma_leftup, 22.718237f, 2.3294117f },
        { &gPlayerAnim_cl_uma_rightup, -22.0f, 1.9800001f },
    },
};

u16 D_8085D254[] = {
    0x1804, // PLAYER_FORM_GORON
    0x1805, // PLAYER_FORM_ZORA
    0x1806, // PLAYER_FORM_DEKU
    0x1806, // PLAYER_FORM_HUMAN
};

u16 D_8085D25C[] = {
    0x1804, // PLAYER_FORM_FIERCE_DEITY
    0x1804, // PLAYER_FORM_GORON
    0x1805, // PLAYER_FORM_ZORA
    0x1806, // PLAYER_FORM_DEKU
};

// Player_MountHorse
s32 Player_ActionHandler_3(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;

    if (rideActor != NULL) {
        if ((rideActor->type != HORSE_TYPE_2) && (this->transformation != PLAYER_FORM_FIERCE_DEITY)) {
            if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A)) {
                if (CutsceneManager_IsNext(CS_ID_GLOBAL_TALK)) {
                    rideActor->actor.textId = D_8085D254[this->transformation - 1];
                    Player_StartTalking(play, &rideActor->actor);
                    return true;
                }
            }

            CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
        } else if ((rideActor->type == HORSE_TYPE_2) && (this->transformation != PLAYER_FORM_HUMAN)) {
            if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A)) {
                if (CutsceneManager_IsNext(CS_ID_GLOBAL_TALK)) {
                    rideActor->actor.textId = D_8085D25C[this->transformation];
                    Player_StartTalking(play, &rideActor->actor);
                    return true;
                }
            }

            CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
        } else {
            if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A)) {
                s32 pad[2];
                f32 sp28 = Math_CosS(rideActor->actor.shape.rot.y);
                f32 sp24 = Math_SinS(rideActor->actor.shape.rot.y);
                struct_8085D224* entry;
                f32 temp_fv0;
                f32 temp_fv1;

                Player_SetupWaitForPutAway(play, this, func_80837BD0);

                this->stateFlags1 |= PLAYER_STATE1_800000;
                this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER;
                this->bodyIsBurning = false;

                if (this->transformation == PLAYER_FORM_FIERCE_DEITY) {
                    entry = D_8085D224[0];
                } else {
                    entry = D_8085D224[1];
                }
                if (this->mountSide >= 0) {
                    entry++;
                }

                temp_fv0 = entry->unk_4;
                temp_fv1 = entry->unk_8;
                this->actor.world.pos.x =
                    rideActor->actor.world.pos.x + rideActor->riderPos.x + ((temp_fv0 * sp28) + (temp_fv1 * sp24));
                this->actor.world.pos.z =
                    rideActor->actor.world.pos.z + rideActor->riderPos.z + ((temp_fv1 * sp28) - (temp_fv0 * sp24));
                this->unk_B48 = rideActor->actor.world.pos.y - this->actor.world.pos.y;

                this->yaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

                Player_MountHorse(play, this, &rideActor->actor);
                Player_Anim_PlayOnce(play, this, entry->anim);
                Player_AnimReplace_Setup(play, this,
                                         ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_ENABLE_MOVEMENT |
                                             ANIM_FLAG_NOMOVE | ANIM_FLAG_80);
                this->actor.parent = this->rideActor;
                func_8082DAD4(this);
                Actor_DeactivateLens(play);

                return true;
            }
        }
    }

    return false;
}

PlayerAnimationHeader* sSlopeSlideAnims[] = {
    &gPlayerAnim_link_normal_down_slope_slip,
    &gPlayerAnim_link_normal_up_slope_slip,
};

s32 Player_HandleSlopes(PlayState* play, Player* this) {
    if (!Player_InBlockingCsMode(play, this) && !(this->cylinder.base.ocFlags1 & OC1_HIT) &&
        (Player_Action_SlideOnSlope != this->actionFunc) && (Player_Action_96 != this->actionFunc) &&
        (sPlayerFloorEffect == FLOOR_EFFECT_1)) {
        s16 playerVelYaw = Math_Atan2S_XY(this->actor.velocity.z, this->actor.velocity.x);
        Vec3f slopeNormal;
        s16 downwardSlopeYaw;
        s16 velYawToDownwardSlope;
        f32 slopeSlowdownSpeed;
        f32 temp_fv1;
        f32 var_fa1;
        f32 slopeSlowdownSpeedStep;

        Actor_GetSlopeDirection(this->actor.floorPoly, &slopeNormal, &downwardSlopeYaw);
        velYawToDownwardSlope = downwardSlopeYaw - playerVelYaw;

        if (ABS_ALT(velYawToDownwardSlope) > 0x3E80) { // 87.9 degrees
            var_fa1 = (Player_Action_96 == this->actionFunc) ? Math_CosS(this->floorPitch) : slopeNormal.y;
            slopeSlowdownSpeed = (1.0f - var_fa1) * 40.0f;
            temp_fv1 = fabsf(this->actor.speed) + slopeSlowdownSpeed;
            slopeSlowdownSpeedStep = SQ(temp_fv1) * 0.011f;
            slopeSlowdownSpeedStep = CLAMP_MIN(slopeSlowdownSpeedStep, 2.2f);

            // slows down speed as player is climbing a slope
            this->pushedYaw = downwardSlopeYaw;
            Math_StepToF(&this->pushedSpeed, slopeSlowdownSpeed, slopeSlowdownSpeedStep);
        } else {
            // moving downward on the slope, causing player to slip and then slide down
            Player_SetAction(play, this, Player_Action_SlideOnSlope, 0);
            func_8082DE50(play, this);

            // facingUpSlope has not yet been updated based on slope, so it will always be 0 here.
            Player_Anim_PlayLoopMorph(play, this, sSlopeSlideAnims[this->av1.facingUpSlope]);

            this->speedXZ = sqrtf(SQXZ(this->actor.velocity));
            this->yaw = downwardSlopeYaw;

            if (sFloorPitchShape >= 0) {
                this->av1.facingUpSlope = true;
                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HANG);
            }

            return true;
        }
    }

    return false;
}

void func_8083D168(PlayState* play, Player* this, GetItemEntry* giEntry) {
    Item00Type dropType = GIFIELD_GET_DROP_TYPE(giEntry->field);

    if (!(giEntry->field & GIFIELD_NO_COLLECTIBLE)) {
        Item_DropCollectible(play, &this->actor.world.pos, dropType | 0x8000);

        if ((dropType == ITEM00_BOMBS_A) || (dropType == ITEM00_ARROWS_30) || (dropType == ITEM00_ARROWS_40) ||
            (dropType == ITEM00_ARROWS_50) || (dropType == ITEM00_RUPEE_GREEN) || (dropType == ITEM00_RUPEE_BLUE) ||
            (dropType == ITEM00_RUPEE_RED) || (dropType == ITEM00_RUPEE_PURPLE) || (dropType == ITEM00_RUPEE_HUGE)) {
            return;
        }
    }

    Item_Give(play, giEntry->itemId);
    Audio_PlaySfx((this->getItemId < GI_NONE) ? NA_SE_SY_GET_BOXITEM : NA_SE_SY_GET_ITEM);
}

s32 Player_ActionHandler_2(Player* this, PlayState* play) {
    if (gSaveContext.save.saveInfo.playerData.health != 0) {
        Actor* interactRangeActor = this->interactRangeActor;

        if (interactRangeActor != NULL) {
            if (this->getItemId > GI_NONE) {
                if (this->getItemId < GI_MAX) {
                    GetItemEntry* giEntry = &sGetItemTable[this->getItemId - 1];

                    interactRangeActor->parent = &this->actor;
                    if ((Item_CheckObtainability(giEntry->itemId) == ITEM_NONE) ||
                        ((s16)giEntry->objectId == OBJECT_GI_BOMB_2)) {
                        Player_DetachHeldActor(play, this);
                        func_80838830(this, giEntry->objectId);

                        if (!(this->stateFlags2 & PLAYER_STATE2_400) ||
                            (this->currentBoots == PLAYER_BOOTS_ZORA_UNDERWATER)) {
                            Player_StopCutscene(this);
                            Player_SetupWaitForPutAwayWithCs(play, this, func_80837C78,
                                                             play->playerCsIds[PLAYER_CS_ID_ITEM_GET]);
                            Player_Anim_PlayOnceAdjusted(play, this,
                                                         (this->transformation == PLAYER_FORM_DEKU)
                                                             ? &gPlayerAnim_pn_getB
                                                             : &gPlayerAnim_link_demo_get_itemB);
                        }

                        this->stateFlags1 |=
                            (PLAYER_STATE1_400 | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_20000000);
                        func_8082DAD4(this);

                        return true;
                    }

                    func_8083D168(play, this, giEntry);
                    this->getItemId = GI_NONE;
                }
            } else if (this->csAction == PLAYER_CSACTION_NONE) {
                if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
                    if (this->getItemId != GI_NONE) {
                        if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A)) {
                            GetItemEntry* giEntry = &sGetItemTable[-this->getItemId - 1];
                            EnBox* chest = (EnBox*)interactRangeActor;

                            if ((giEntry->itemId != ITEM_NONE) &&
                                (((Item_CheckObtainability(giEntry->itemId) == ITEM_NONE) &&
                                  (giEntry->field & GIFIELD_40)) ||
                                 (((Item_CheckObtainability(giEntry->itemId) != ITEM_NONE)) &&
                                  (giEntry->field & GIFIELD_20)))) {
                                this->getItemId =
                                    (giEntry->itemId == ITEM_MASK_CAPTAIN) ? -GI_RECOVERY_HEART : -GI_RUPEE_BLUE;
                                giEntry = &sGetItemTable[-this->getItemId - 1];
                            }

                            if (GameInteractor_Should(VB_GIVE_ITEM_FROM_CHEST, true, chest)) {
                                // This inverts the sign of the getItemId and sets the player's action to GetItem
                                // (Player_Action_65)
                                Player_SetupWaitForPutAway(play, this, func_80837C78);
                            }
                            this->stateFlags1 |=
                                (PLAYER_STATE1_400 | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_20000000);
                            func_80838830(this, giEntry->objectId);

                            this->actor.world.pos.x =
                                interactRangeActor->world.pos.x -
                                (Math_SinS(interactRangeActor->shape.rot.y) * this->ageProperties->unk_9C);
                            this->actor.world.pos.z =
                                interactRangeActor->world.pos.z -
                                (Math_CosS(interactRangeActor->shape.rot.y) * this->ageProperties->unk_9C);
                            this->actor.world.pos.y = interactRangeActor->world.pos.y;
                            this->yaw = this->actor.shape.rot.y = interactRangeActor->shape.rot.y;

                            func_8082DAD4(this);
                            if (GameInteractor_Should(VB_PLAY_SLOW_CHEST_CS,
                                                      (giEntry->itemId != ITEM_NONE) && (giEntry->gid >= 0) &&
                                                          (Item_CheckObtainability(giEntry->itemId) == ITEM_NONE),
                                                      chest)) {
                                this->csId = chest->csId2;
                                Player_Anim_PlayOnceAdjusted(play, this, this->ageProperties->openChestAnim);
                                Player_AnimReplace_Setup(play, this,
                                                         ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_4 |
                                                             ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE |
                                                             ANIM_FLAG_80);
                                this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER;
                                chest->unk_1EC = 1;
                            } else {
                                Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_box_kick);
                                chest->unk_1EC = -1;
                            }

                            return true;
                        }
                    } else if (!(this->stateFlags1 & PLAYER_STATE1_8000000) &&
                               (this->transformation != PLAYER_FORM_DEKU)) {
                        if ((this->heldActor == NULL) || Player_IsHoldingHookshot(this)) {
                            EnBom* bomb = (EnBom*)interactRangeActor;

                            if (((this->transformation != PLAYER_FORM_GORON) &&
                                 (((bomb->actor.id == ACTOR_EN_BOM) && bomb->isPowderKeg) ||
                                  ((interactRangeActor->id == ACTOR_EN_ISHI) && (interactRangeActor->params & 1)) ||
                                  (interactRangeActor->id == ACTOR_EN_MM)))) {
                                return false;
                            }

                            this->stateFlags2 |= PLAYER_STATE2_10000;
                            if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A)) {
                                Player_SetupWaitForPutAway(play, this, func_808379C0);
                                func_8082DAD4(this);
                                this->stateFlags1 |= PLAYER_STATE1_CARRYING_ACTOR;

                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

// Player_SetAction_Throwing
void func_8083D6DC(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_42, 1);
    Player_Anim_PlayOnce(play, this, D_8085BE84[PLAYER_ANIMGROUP_throw][this->modelAnimType]);
}

/**
 * Checks if an actor can be thrown or dropped.
 * It is assumed that the `actor` argument is the actor currently being carried.
 *
 * @return true if it can be thrown, false if it can be dropped.
 */
s32 Player_CanThrowCarriedActor(Player* this, Actor* heldActor) {
    // If the actor arg is null, true will be returned.
    // It doesn't make sense for a non-existent actor to be thrown or dropped, so
    // the safety check should happen before this function is even called.
    if ((heldActor != NULL) && !(heldActor->flags & ACTOR_FLAG_THROW_ONLY) &&
        ((this->speedXZ < 1.1f) || (heldActor->id == ACTOR_EN_BOM_CHU))) {
        return false;
    }

    return true;
}

s32 Player_ActionHandler_9(Player* this, PlayState* play) {
    if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        if ((this->heldActor != NULL) &&
            CHECK_BTN_ANY(sPlayerControlInput->press.button,
                          BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_B | BTN_A | BTN_DPAD_EQUIP)) {
            if (!func_808313A8(play, this, this->heldActor)) {
                if (!Player_CanThrowCarriedActor(this, this->heldActor)) {
                    Player_SetAction(play, this, Player_Action_41, 1);
                    Player_Anim_PlayOnce(play, this, D_8085BE84[PLAYER_ANIMGROUP_put][this->modelAnimType]);
                    return true;
                }
                func_8083D6DC(this, play);
            }

            return true;
        }
    }
    return false;
}

s32 func_8083D860(Player* this, PlayState* play) {
    if ((this->yDistToLedge >= 79.0f) &&
        (!(this->stateFlags1 & PLAYER_STATE1_8000000) || (this->currentBoots == PLAYER_BOOTS_ZORA_UNDERWATER) ||
         (this->actor.depthInWater < this->ageProperties->unk_2C))) {
        s32 var_t0 = (sPlayerTouchedWallFlags & WALL_FLAG_3) ? 2 : 0;
        s32 temp_t2 = sPlayerTouchedWallFlags & WALL_FLAG_1;

        if ((var_t0 != 0) || temp_t2 ||
            SurfaceType_CheckWallFlag2(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) {
            CollisionPoly* wallPoly = this->actor.wallPoly;
            f32 sp78;
            f32 sp74;
            f32 zOut;
            f32 yOut;
            Vec3f sp48[3];
            s32 i;
            f32 sp40;
            Vec3f* sp3C;
            f32 xOut;

            yOut = xOut = 0.0f;
            if (var_t0 != 0) {
                sp78 = this->actor.world.pos.x;
                sp74 = this->actor.world.pos.z;
            } else {
                sp3C = sp48;
                CollisionPoly_GetVerticesByBgId(wallPoly, this->actor.wallBgId, &play->colCtx, sp48);
                sp78 = xOut = sp48[0].x;
                sp74 = zOut = sp48[0].z;
                yOut = sp48[0].y;

                for (i = 1; i < ARRAY_COUNT(sp48); i++) {
                    sp3C++;

                    if (sp78 > sp3C->x) {
                        sp78 = sp3C->x;
                    } else if (xOut < sp3C->x) {
                        xOut = sp3C->x;
                    }

                    if (sp74 > sp3C->z) {
                        sp74 = sp3C->z;
                    } else if (zOut < sp3C->z) {
                        zOut = sp3C->z;
                    }

                    if (yOut > sp3C->y) {
                        yOut = sp3C->y;
                    }
                }

                sp78 = (sp78 + xOut) * 0.5f;
                sp74 = (sp74 + zOut) * 0.5f;

                xOut = ((this->actor.world.pos.x - sp78) * COLPOLY_GET_NORMAL(wallPoly->normal.z)) -
                       ((this->actor.world.pos.z - sp74) * COLPOLY_GET_NORMAL(wallPoly->normal.x));

                sp40 = this->actor.world.pos.y - yOut;
                yOut = ((s32)((sp40 / 15.0f) + 0.5f) * 15.0f) - sp40;
                xOut = fabsf(xOut);
            }

            if (xOut < 8.0f) {
                f32 wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 distToInteractWall = this->distToInteractWall;
                PlayerAnimationHeader* anim;

                Player_SetupWaitForPutAway(play, this, func_80837C20);

                this->stateFlags1 |= PLAYER_STATE1_200000;
                this->stateFlags1 &= ~PLAYER_STATE1_8000000;

                if ((var_t0 != 0) || temp_t2) {
                    if ((this->av1.actionVar1 = var_t0) != 0) {
                        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                            anim = &gPlayerAnim_link_normal_Fclimb_startA;

                        } else {
                            anim = &gPlayerAnim_link_normal_Fclimb_hold2upL;
                        }
                        distToInteractWall = (this->ageProperties->unk_3C + 4.0f) - distToInteractWall;
                    } else {
                        anim = this->ageProperties->unk_AC;
                        distToInteractWall = 20.5f;
                    }

                    this->av2.actionVar2 = -2;
                    this->actor.world.pos.y += yOut;

                    this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
                } else {
                    anim = this->ageProperties->unk_B0;
                    distToInteractWall = (this->ageProperties->wallCheckRadius - this->ageProperties->unk_3C) + 17.0f;
                    this->av2.actionVar2 = -4;

                    this->actor.shape.rot.y = this->yaw = i = this->actor.wallYaw; //! FAKE
                }

                this->actor.world.pos.x = (distToInteractWall * wallPolyNormalX) + sp78;
                this->actor.world.pos.z = (distToInteractWall * wallPolyNormalZ) + sp74;
                func_8082DAD4(this);
                Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
                Player_Anim_PlayOnce(play, this, anim);
                Player_AnimReplace_Setup(play, this,
                                         ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_4 | ANIM_FLAG_ENABLE_MOVEMENT |
                                             ANIM_FLAG_NOMOVE | ANIM_FLAG_80);
                return true;
            }
        }
    }

    return false;
}

void func_8083DCC4(Player* this, PlayerAnimationHeader* anim, PlayState* play) {
    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_51, 0);
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, 4.0f / 3.0f);
}

s32 func_8083DD1C(PlayState* play, Player* this, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    CollisionPoly* wallPoly;
    s32 bgId;
    Vec3f sp74;
    Vec3f sp68;
    Vec3f sp5C;
    f32 cos = Math_CosS(this->actor.shape.rot.y);
    f32 sin = Math_SinS(this->actor.shape.rot.y);

    sp74.x = this->actor.world.pos.x + (arg5 * sin);
    sp74.z = this->actor.world.pos.z + (arg5 * cos);
    sp68.x = this->actor.world.pos.x + (arg4 * sin);
    sp68.z = this->actor.world.pos.z + (arg4 * cos);
    sp74.y = sp68.y = this->actor.world.pos.y + arg2;

    if (BgCheck_EntityLineTest2(&play->colCtx, &sp74, &sp68, &sp5C, &this->actor.wallPoly, true, false, false, true,
                                &bgId, &this->actor)) {
        f32 wallPolyNormalX;
        f32 wallPolyNormalZ;

        wallPoly = this->actor.wallPoly;
        this->actor.bgCheckFlags |= BGCHECKFLAG_PLAYER_WALL_INTERACT;
        this->actor.wallBgId = bgId;
        sPlayerTouchedWallFlags = SurfaceType_GetWallFlags(&play->colCtx, wallPoly, bgId);

        wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
        wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);

        Math_ScaledStepToS(&this->actor.shape.rot.y, Math_Atan2S_XY(-wallPolyNormalZ, -wallPolyNormalX), 0x320);

        this->yaw = this->actor.shape.rot.y;
        this->actor.world.pos.x = sp5C.x - (Math_SinS(this->actor.shape.rot.y) * arg3);
        this->actor.world.pos.z = sp5C.z - (Math_CosS(this->actor.shape.rot.y) * arg3);

        return true;
    }

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_PLAYER_WALL_INTERACT;
    return false;
}

void func_8083DEE4(PlayState* play, Player* this) {
    f32 temp_fv0 = this->ageProperties->wallCheckRadius;

    func_8083DD1C(play, this, 268 * 0.1f, temp_fv0 + 5.0f, temp_fv0 + 15.0f, 0.0f);
}

void func_8083DF38(Player* this, PlayerAnimationHeader* anim, PlayState* play) {
    if (!Player_SetupWaitForPutAway(play, this, func_80837BF8)) {
        Player_SetAction(play, this, Player_Action_45, 0);
    }

    Player_Anim_PlayOnce(play, this, anim);
    func_8082DAD4(this);

    this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
}

s32 Player_ActionHandler_5(Player* this, PlayState* play) {
    if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) && (sShapeYawToTouchedWall < 0x3000)) {
        if ((this->speedXZ > 0.0f) && func_8083D860(this, play)) {
            return true;
        }

        if (!func_801242B4(this) && ((this->speedXZ == 0.0f) || !(this->stateFlags2 & PLAYER_STATE2_4)) &&
            (sPlayerTouchedWallFlags & WALL_FLAG_6) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
            (this->yDistToLedge >= 39.0f)) {
            this->stateFlags2 |= PLAYER_STATE2_1;

            if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A)) {
                DynaPolyActor* dyna;

                if ((this->actor.wallBgId != BGCHECK_SCENE) &&
                    ((dyna = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId)) != NULL)) {
                    this->rightHandActor = &dyna->actor;
                } else {
                    this->rightHandActor = NULL;
                }

                func_8083DF38(this, &gPlayerAnim_link_normal_push_wait, play);
                return true;
            }
        }
    }

    return false;
}

s32 func_8083E14C(PlayState* play, Player* this) {
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
        ((this->stateFlags2 & PLAYER_STATE2_10) || CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A))) {
        DynaPolyActor* var_v1 = NULL;

        if (this->actor.wallBgId != BGCHECK_SCENE) {
            var_v1 = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);
        }

        if (&var_v1->actor == this->rightHandActor) {
            if (this->stateFlags2 & PLAYER_STATE2_10) {
                return true;
            }
            return false;
        }
    }

    func_808369F4(this, play);
    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_push_wait_end);
    this->stateFlags2 &= ~PLAYER_STATE2_10;
    return true;
}

void func_8083E234(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_46, 0);
    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_push_start);
    this->stateFlags2 |= PLAYER_STATE2_10;
}

void func_8083E28C(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_47, 0);
    Player_Anim_PlayOnce(play, this, D_8085BE84[PLAYER_ANIMGROUP_pull_start][this->modelAnimType]);
    this->stateFlags2 |= PLAYER_STATE2_10;
}

void func_8083E2F4(Player* this, PlayState* play) {
    this->stateFlags1 &= ~(PLAYER_STATE1_200000 | PLAYER_STATE1_8000000);
    func_80833AA0(this, play);

    if (this->transformation == PLAYER_FORM_DEKU) {
        this->speedXZ = -1.7f;
    } else {
        this->speedXZ = -0.4f;
    }
}

s32 func_8083E354(Player* this, PlayState* play) {
    if (!CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT)) {
        if ((sPlayerTouchedWallFlags & WALL_FLAG_3) || (sPlayerTouchedWallFlags & WALL_FLAG_1) ||
            SurfaceType_CheckWallFlag2(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) {
            return false;
        }
    }

    func_8083E2F4(this, play);
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_AUTO_JUMP);
    return true;
}

s32 func_8083E404(Player* this, f32 arg1, s16 arg2) {
    f32 sp1C = BINANG_SUB(arg2, this->actor.shape.rot.y);
    f32 temp_fv1;

    if (this->focusActor != NULL) {
        func_8083C62C(this, func_800B7128(this) || func_8082EF20(this));
    }

    // Using Should hook, but ignoring return value, to be able to modify the speed argument
    GameInteractor_Should(VB_ZTARGET_SPEED_CHECK, false, &arg1);

    temp_fv1 = fabsf(sp1C) / 0x8000;
    if (((SQ(temp_fv1) * 50.0f) + 6.0f) < arg1) {
        return 1;
    }

    if ((((1.0f - temp_fv1) * 10.0f) + 6.8f) < arg1) {
        return -1;
    }
    return 0;
}

s32 func_8083E514(Player* this, f32* arg2, s16* arg3, PlayState* play) {
    s16 temp_v1 = *arg3 - this->parallelYaw;
    u16 var_a2 = ABS_ALT(temp_v1);

    if ((func_800B7128(this) || func_8082EF20(this)) && (this->focusActor == NULL)) {
        *arg2 *= Math_SinS(var_a2);

        if (*arg2 != 0.0f) {
            *arg3 = (((temp_v1 >= 0) ? 1 : -1) * 0x4000) + this->actor.shape.rot.y;
        } else {
            *arg3 = this->actor.shape.rot.y;
        }

        if (this->focusActor != NULL) {
            func_8083C62C(this, true);
        } else {
            Math_SmoothStepToS(&this->actor.focus.rot.x, (sPlayerControlInput->rel.stick_y * 240.0f), 0xE, 0xFA0, 0x1E);
            func_80832754(this, true);
        }
    } else {
        if (this->focusActor != NULL) {
            return func_8083E404(this, *arg2, *arg3);
        }

        func_8083C6E8(this, play);
        if ((*arg2 != 0.0f) && (var_a2 < 0x1770)) {
            return 1;
        }

        if ((Math_SinS(0x4000 - (var_a2 >> 1)) * 200.0f) < *arg2) {
            return -1;
        }
    }
    return 0;
}

s32 func_8083E758(Player* this, f32* arg1, s16* arg2) {
    f32 temp_fv0;
    u16 temp_v0;
    s16 var_v1;

    var_v1 = *arg2 - this->actor.shape.rot.y;
    temp_v0 = ABS_ALT(var_v1);
    temp_fv0 = Math_CosS(temp_v0);
    *arg1 *= temp_fv0;

    // Can't be (*arg1 != 0.0f)
    if (*arg1 != 0) {
        if (temp_fv0 > 0.0f) {
            return 1;
        }
        return -1;
    }
    return 0;
}

s32 func_8083E7F8(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    func_8083C6E8(this, play);

    if ((*arg1 != 0.0f) || (ABS_ALT(this->unk_B4C) > 0x190)) {
        s16 temp_a0 = *arg2 - (u16)Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
        u16 temp;

        temp = (ABS_ALT(temp_a0) - 0x2000);
        if ((temp < 0x4000) || (this->unk_B4C != 0)) {
            return -1;
        }
        return 1;
    }

    return 0;
}

void func_8083E8E0(Player* this, f32 arg1, s16 arg2) {
    s16 temp = arg2 - this->actor.shape.rot.y;

    if (arg1 > 0.0f) {
        if (temp < 0) {
            this->unk_B44 = 0.0f;
        } else {
            this->unk_B44 = 1.0f;
        }
    }

    Math_StepToF(&this->unk_B40, this->unk_B44, 0.3f);
}

void func_8083E958(PlayState* play, Player* this) {
    PlayerAnimation_BlendToJoint(play, &this->skelAnime, func_8082EF54(this), this->unk_B38, func_8082EF9C(this),
                                 this->unk_B38, this->unk_B40, this->blendTableBuffer);
}

s32 func_8083E9C4(f32 arg0, f32 arg1, f32 arg2, f32 arg3) {
    f32 temp_fv0;

    if ((arg3 == 0.0f) && (arg1 > 0.0f)) {
        arg3 = arg2;
    }
    temp_fv0 = (arg0 + arg1) - arg3;
    if (((temp_fv0 * arg1) >= 0.0f) && (((temp_fv0 - arg1) * arg1) < 0.0f)) {
        return true;
    }
    return false;
}

void func_8083EA44(Player* this, f32 arg1) {
    s32 sp24;
    f32 updateScale = R_UPDATE_RATE / 2.0f;

    arg1 *= updateScale;
    if (arg1 < -7.25f) {
        arg1 = -7.25f;
    } else if (arg1 > 7.25f) {
        arg1 = 7.25f;
    }

    sp24 = func_8083E9C4(this->unk_B38, arg1, 29.0f, 10.0f);

    if (sp24 || func_8083E9C4(this->unk_B38, arg1, 29.0f, 24.0f)) {
        Player_AnimSfx_PlayFloorWalk(this, this->speedXZ);
        if (this->speedXZ > 4.0f) {
            this->stateFlags2 |= PLAYER_STATE2_8;
        }
        this->actor.shape.unk_17 = sp24 ? 1 : 2;
    }

    this->unk_B38 += arg1;
    if (this->unk_B38 < 0.0f) {
        this->unk_B38 += 29.0f;
    } else if (this->unk_B38 >= 29.0f) {
        this->unk_B38 -= 29.0f;
    }
}

void Player_ChooseNextIdleAnim(PlayState* play, Player* this) {
    PlayerAnimationHeader* anim;
    u32 healthIsCritical;
    PlayerAnimationHeader** fidgetAnimPtr;
    s32 fidgetType;
    s32 commonType;
    f32 morphFrames;
    s16 endFrame;

    if (((this->actor.id != ACTOR_PLAYER) && !(healthIsCritical = (this->actor.colChkInfo.health < 0x64))) ||
        ((this->actor.id == ACTOR_PLAYER) &&
         (((this->focusActor != NULL) ||
           ((this->transformation != PLAYER_FORM_FIERCE_DEITY) && (this->transformation != PLAYER_FORM_HUMAN)) ||
           (this->currentMask == PLAYER_MASK_SCENTS)) ||
          (!(healthIsCritical = LifeMeter_IsCritical()) && (this->idleType = ((this->idleType + 1) & 1)))))) {
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
            fidgetType = play->roomCtx.curRoom.environmentType;

            if (healthIsCritical) {
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
                // The type determined by `curRoom.environmentType` will be used if a common type is rejected.
                if (commonType < 4) {
                    // `FIDGET_ADJUST_TUNIC` and `FIDGET_TAP_FEET` are accepted unconditionally.
                    // The sword and shield related common types have extra restrictions.
                    //
                    // Note that `FIDGET_SWORD_SWING` is the first common fidget type, which is why
                    // all operations are done relative to this type.
                    if ((((commonType + FIDGET_SWORD_SWING) != FIDGET_SWORD_SWING) &&
                         ((commonType + FIDGET_SWORD_SWING) != FIDGET_ADJUST_SHIELD)) ||
                        ((this->rightHandType == PLAYER_MODELTYPE_RH_SHIELD) &&
                         (((commonType + FIDGET_SWORD_SWING) == FIDGET_ADJUST_SHIELD) ||
                          (Player_GetMeleeWeaponHeld(this) != PLAYER_MELEEWEAPON_NONE)))) {
                        //! @bug It is possible for `FIDGET_ADJUST_SHIELD` to be used even if
                        //! a shield is not currently equipped. This is because of how being shieldless
                        //! is implemented. There is no sword-only model type, only
                        //! `PLAYER_MODELGROUP_SWORD_AND_SHIELD` exists. Therefore, the right hand type will be
                        //! `PLAYER_MODELTYPE_RH_SHIELD` if sword is in hand, even if no shield is equipped.
                        if (((commonType + FIDGET_SWORD_SWING) == FIDGET_SWORD_SWING) &&
                            Player_IsHoldingTwoHandedWeapon(this)) {
                            //! @bug This code is unreachable.
                            //! The check above groups the `Player_GetMeleeWeaponHeld` check and
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

    endFrame = Animation_GetLastFrame(anim);
    if ((BEN_ANIM_EQUAL(this->skelAnime.animation, anim)) ||
        (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pz_attackAend)) ||
        (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pz_attackBend)) ||
        (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pz_attackCend))) {
        morphFrames = 0.0f;
    } else {
        morphFrames = -6.0f;
    }

    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED * sWaterSpeedFactor, 0.0f, endFrame,
                           ANIMMODE_ONCE, morphFrames);
}

void func_8083EE60(Player* this, PlayState* play) {
    f32 temp_fv0;
    f32 var_fs0;

    if (this->unk_B34 < 1.0f) {
        f32 temp_fs0 = R_UPDATE_RATE / 2.0f;

        func_8083EA44(this, REG(35) / 1000.0f);
        PlayerAnimation_LoadToJoint(play, &this->skelAnime, D_8085BE84[PLAYER_ANIMGROUP_back_walk][this->modelAnimType],
                                    this->unk_B38);
        this->unk_B34 += (1.0f * 1.0f) * temp_fs0;
        if (this->unk_B34 >= 1.0f) {
            this->unk_B34 = 1.0f;
        }
        var_fs0 = this->unk_B34;
    } else {
        temp_fv0 = this->speedXZ - (REG(48) / 100.0f);

        if (temp_fv0 < 0.0f) {
            var_fs0 = 1.0f;
            func_8083EA44(this, ((REG(35)) / 1000.0f) + (((REG(36)) / 1000.0f) * this->speedXZ));

            PlayerAnimation_LoadToJoint(play, &this->skelAnime,
                                        D_8085BE84[PLAYER_ANIMGROUP_back_walk][this->modelAnimType], this->unk_B38);
        } else {
            var_fs0 = (REG(37) / 1000.0f) * temp_fv0;
            if (var_fs0 < 1.0f) {
                func_8083EA44(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->speedXZ));
            } else {
                var_fs0 = 1.0f;
                func_8083EA44(this, (REG(39) / 100.0f) + ((REG(38) / 1000.0f) * temp_fv0));
            }

            PlayerAnimation_LoadToMorph(play, &this->skelAnime,
                                        D_8085BE84[PLAYER_ANIMGROUP_back_walk][this->modelAnimType], this->unk_B38);
            PlayerAnimation_LoadToJoint(play, &this->skelAnime, &gPlayerAnim_link_normal_back_run,
                                        this->unk_B38 * (16.0f / 29.0f));
        }
    }
    if (var_fs0 < 1.0f) {
        PlayerAnimation_InterpJointMorph(play, &this->skelAnime, 1.0f - var_fs0);
    }
}

void func_8083F144(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_7, 1);
    Player_Anim_PlayOnceMorph(play, this, &gPlayerAnim_link_normal_back_brake);
}

s32 func_8083F190(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    if (this->speedXZ > 6.0f) {
        func_8083F144(this, play);
        return true;
    }

    if (*arg1 != 0.0f) {
        if (Player_DecelerateToZero(this)) {
            *arg1 = 0.0f;
            *arg2 = this->yaw;
        } else {
            return true;
        }
    }
    return false;
}

void func_8083F230(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_8, 1);
    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_back_brake_end);
}

void func_8083F27C(PlayState* play, Player* this) {
    f32 temp_fv0;
    PlayerAnimationHeader* sp38;
    PlayerAnimationHeader* sp34;

    sp38 = D_8085BE84[PLAYER_ANIMGROUP_side_walkL][this->modelAnimType];
    sp34 = D_8085BE84[PLAYER_ANIMGROUP_side_walkR][this->modelAnimType];

    this->skelAnime.animation = sp38;

    func_8083EA44(this, (REG(30) / 1000.0f) + ((REG(32) / 1000.0f) * this->speedXZ));

    temp_fv0 = this->unk_B38 * (16.0f / 29.0f);
    PlayerAnimation_BlendToJoint(play, &this->skelAnime, sp34, temp_fv0, sp38, temp_fv0, this->unk_B40,
                                 this->blendTableBuffer);
}

void func_8083F358(Player* this, s32 arg1, PlayState* play) {
    PlayerAnimationHeader* climbAnim;
    f32 var_fv1;
    s16 var_a1;

    if (ABS_ALT(sFloorPitchShape) < 0xE38) {
        var_a1 = 0;
    } else {
        var_a1 = CLAMP(sFloorPitchShape, -0x2AAA, 0x2AAA);
    }

    Math_ScaledStepToS(&this->unk_B70, var_a1, 0x190);
    if ((this->modelAnimType == PLAYER_ANIMTYPE_3) || ((this->unk_B70 == 0) && (this->unk_AB8 <= 0.0f))) {
        if (!arg1) {
            PlayerAnimation_LoadToJoint(play, &this->skelAnime, D_8085BE84[PLAYER_ANIMGROUP_walk][this->modelAnimType],
                                        this->unk_B38);
        } else {
            PlayerAnimation_LoadToMorph(play, &this->skelAnime, D_8085BE84[PLAYER_ANIMGROUP_walk][this->modelAnimType],
                                        this->unk_B38);
        }
        return;
    }

    if (this->unk_B70 != 0) {
        var_fv1 = this->unk_B70 / (f32)0x2AAA;
    } else {
        var_fv1 = this->unk_AB8 * 0.0006f;
    }

    var_fv1 *= fabsf(this->speedXZ) * 0.5f;
    if (var_fv1 > 1.0f) {
        var_fv1 = 1.0f;
    }

    if (var_fv1 < 0.0f) {
        climbAnim = &gPlayerAnim_link_normal_climb_down;
        var_fv1 = -var_fv1;
    } else {
        climbAnim = &gPlayerAnim_link_normal_climb_up;
    }

    if (!arg1) {
        PlayerAnimation_BlendToJoint(play, &this->skelAnime, D_8085BE84[PLAYER_ANIMGROUP_walk][this->modelAnimType],
                                     this->unk_B38, climbAnim, this->unk_B38, var_fv1, this->blendTableBuffer);
    } else {
        PlayerAnimation_BlendToMorph(play, &this->skelAnime, D_8085BE84[PLAYER_ANIMGROUP_walk][this->modelAnimType],
                                     this->unk_B38, climbAnim, this->unk_B38, var_fv1, this->blendTableBuffer);
    }
}

void func_8083F57C(Player* this, PlayState* play) {
    f32 temp_fv0;
    f32 var_fs0;

    if (this->unk_B34 < 1.0f) {
        f32 temp_fs0;

        temp_fs0 = R_UPDATE_RATE / 2.0f;
        func_8083EA44(this, REG(35) / 1000.0f);
        PlayerAnimation_LoadToJoint(play, &this->skelAnime, D_8085BE84[PLAYER_ANIMGROUP_walk][this->modelAnimType],
                                    this->unk_B38);

        // required
        this->unk_B34 += 1 * temp_fs0;
        if (this->unk_B34 >= 1.0f) {
            this->unk_B34 = 1.0f;
        }
        var_fs0 = this->unk_B34;
    } else {
        temp_fv0 = (this->speedXZ - (REG(48) / 100.0f));
        if (temp_fv0 < 0.0f) {
            var_fs0 = 1.0f;
            func_8083EA44(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->speedXZ));
            func_8083F358(this, false, play);
        } else {
            var_fs0 = (REG(37) / 1000.0f) * temp_fv0;
            if (var_fs0 < 1.0f) {
                func_8083EA44(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->speedXZ));
            } else {
                var_fs0 = 1.0f;
                func_8083EA44(this, (REG(39) / 100.0f) + ((REG(38) / 1000.0f) * temp_fv0));
            }
            func_8083F358(this, true, play);
            PlayerAnimation_LoadToJoint(play, &this->skelAnime, func_8082EEE0(this), this->unk_B38 * (20.0f / 29.0f));
        }
    }

    if (var_fs0 < 1.0f) {
        PlayerAnimation_InterpJointMorph(play, &this->skelAnime, 1.0f - var_fs0);
    }
}

void func_8083F828(Vec3f* arg0, Vec3f* arg1, f32 arg2, f32 arg3, f32 arg4) {
    arg1->x = Rand_CenteredFloat(arg3) + arg0->x;
    arg1->y = Rand_CenteredFloat(arg4) + (arg0->y + arg2);
    arg1->z = Rand_CenteredFloat(arg3) + arg0->z;
}

Color_RGBA8 D_8085D26C = { 255, 255, 255, 255 };
Vec3f D_8085D270 = { 0.0f, 0.04f, 0.0f };

s32 func_8083F8A8(PlayState* play, Player* this, f32 radius, s32 countMax, f32 randAccelWeight, s32 scale,
                  s32 scaleStep, s32 useLighting) {
    static Vec3f D_8085D27C = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_8085D288 = { 0.0f, 0.0f, 0.0f };

    if ((countMax < 0) || (this->floorSfxOffset == NA_SE_PL_WALK_SNOW - SFX_FLAG)) {
        s32 count = func_80173B48(&play->state) / 20000000;
        Vec3f pos;
        s32 i;

        count = (count >= ABS_ALT(countMax)) ? ABS_ALT(countMax) : count;
        for (i = 0; i < count; i++) {
            func_8083F828(&this->actor.world.pos, &pos, 0.0f, 40.0f, 10.0f);
            D_8085D27C.x = Rand_CenteredFloat(3.0f);
            D_8085D27C.z = Rand_CenteredFloat(3.0f);
            EffectSsDust_Spawn(play, 0, &pos, &D_8085D27C, &D_8085D270, &D_8085D26C, &D_8085D26C, scale, scaleStep, 42,
                               0);
        }

        return true;
    } else if ((this->floorSfxOffset == NA_SE_PL_WALK_GROUND - SFX_FLAG) ||
               (this->floorSfxOffset == NA_SE_PL_WALK_SAND - SFX_FLAG)) {
        s32 count = func_80173B48(&play->state) / 12000000;

        if (count > 0) {
            Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, radius,
                                     (count < countMax) ? count : countMax, randAccelWeight, scale, scaleStep,
                                     useLighting);

            return true;
        }
    } else if (this->floorSfxOffset == NA_SE_PL_WALK_GRASS - SFX_FLAG) {
        s32 count = func_80173B48(&play->state) / 12000000;
        Vec3f velocity;
        Vec3f pos;
        s32 i;

        count = (count >= countMax) ? countMax : count;
        for (i = 0; i < count; i++) {
            func_8083F828(&this->actor.world.pos, &pos, 0.0f, 20.0f, 20.0f);
            velocity.x = Rand_CenteredFloat(3.0f);
            velocity.y = Rand_ZeroFloat(2.0f);
            velocity.z = Rand_CenteredFloat(3.0f);
            D_8085D288.y = -0.1f;
            EffectSsHahen_Spawn(play, &pos, &velocity, &D_8085D288, 0, 0x96, 1, 0x10, gKakeraLeafTipDL);
        }
    }

    return false;
}

s32 func_8083FBC4(PlayState* play, Player* this) {
    if ((this->floorSfxOffset == NA_SE_PL_WALK_GROUND - SFX_FLAG) ||
        (this->floorSfxOffset == NA_SE_PL_WALK_SAND - SFX_FLAG)) {
        Vec3f* feetPos = this->actor.shape.feetPos;
        s32 i;

        for (i = 0; i < ARRAY_COUNT(this->actor.shape.feetPos); i++) {
            func_800B1210(play, feetPos, &gZeroVec3f, &gZeroVec3f, 50, 30);
            feetPos++;
        }

        return true;
    }

    if (this->floorSfxOffset == NA_SE_PL_WALK_SNOW - SFX_FLAG) {
        Vec3f* feetPos = this->actor.shape.feetPos;
        s32 i;

        for (i = 0; i < ARRAY_COUNT(this->actor.shape.feetPos); i++) {
            EffectSsDust_Spawn(play, 0, feetPos, &gZeroVec3f, &D_8085D270, &D_8085D26C, &D_8085D26C, 100, 40, 17, 0);
            feetPos++;
        }

        return true;
    }

    return false;
}

s32 func_8083FCF0(PlayState* play, Player* this, f32 arg2, f32 arg3, f32 arg4) {
    if (arg4 < this->skelAnime.curFrame) {
        func_8082DC38(this);
    } else if (arg2 <= this->skelAnime.curFrame) {
        this->stateFlags3 |= PLAYER_STATE3_2000000;
        func_8082FA5C(play, this,
                      (arg3 <= this->skelAnime.curFrame) ? PLAYER_MELEE_WEAPON_STATE_1
                                                         : PLAYER_MELEE_WEAPON_STATE_MINUS_1);
        return true;
    }
    return false;
}

// Crouch-stabbing
s32 func_8083FD80(Player* this, PlayState* play) {
    if (!Player_IsGoronOrDeku(this) && (Player_GetMeleeWeaponHeld(this) != PLAYER_MELEEWEAPON_NONE) &&
        (this->transformation != PLAYER_FORM_ZORA) && sPlayerUseHeldItem) {
        //! Calling this function sets the meleeWeaponQuads' damage properties correctly, patching "Power Crouch Stab".
        if (GameInteractor_Should(VB_PATCH_POWER_CROUCH_STAB, true)) {
            func_8083375C(this, PLAYER_MWA_STAB_1H);
        }
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_defense_kiru);
        this->av1.actionVar1 = 1;
        this->meleeWeaponAnimation = PLAYER_MWA_STAB_1H;
        this->yaw = this->actor.shape.rot.y + this->upperLimbRot.y;
        this->unk_ADD = 0;
        return true;
    }
    return false;
}

bool func_8083FE38(Player* this, PlayState* play) {
    return Player_ActionHandler_13(this, play) || Player_ActionHandler_Talk(this, play) ||
           Player_ActionHandler_2(this, play);
}

void Player_RequestQuakeAndRumble(PlayState* play, Player* this, u16 sfxId) {
    Player_RequestQuake(play, 27767, 7, 20);
    Player_RequestRumble(play, this, 255, 20, 150, SQ(0));
    Player_PlaySfx(this, sfxId);
}

void func_8083FEF4(PlayState* play, Player* this) {
    Inventory_ChangeAmmo(ITEM_DEKU_STICK, -1);
    Player_UseItem(play, this, ITEM_NONE);
}

bool func_8083FF30(PlayState* play, Player* this) {
    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) && (this->unk_B0C > 0.5f)) {
        if (AMMO(ITEM_DEKU_STICK) != 0) {
            EffectSsStick_Spawn(play, &this->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND],
                                BINANG_ADD(this->actor.shape.rot.y, 0x8000));
            this->unk_B0C = 0.5f;
            func_8083FEF4(play, this);
            Player_PlaySfx(this, NA_SE_IT_WOODSTICK_BROKEN);
        }
        return true;
    }

    return false;
}

// handles razor sword health and breaking
bool func_8083FFEC(PlayState* play, Player* this) {
    if (this->heldItemAction == PLAYER_IA_SWORD_RAZOR) {
        if (gSaveContext.save.saveInfo.playerData.swordHealth > 0) {
            if (GameInteractor_Should(VB_LOWER_RAZOR_SWORD_DURABILITY, true)) {
                gSaveContext.save.saveInfo.playerData.swordHealth--;
            }
            if (gSaveContext.save.saveInfo.playerData.swordHealth <= 0) {
                Item_Give(play, ITEM_SWORD_KOKIRI);
                Player_UseItem(play, this, ITEM_SWORD_KOKIRI);
                Player_PlaySfx(this, NA_SE_IT_MAJIN_SWORD_BROKEN);
                if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                    Message_StartTextbox(play, 0xF9, NULL);
                }
            }
        }
        return true;
    }
    return false;
}

// Could return the last function, but never used as such
void func_80840094(PlayState* play, Player* this) {
    func_8083FF30(play, this);
    func_8083FFEC(play, this);
}

PlayerAnimationHeader* D_8085D294[] = {
    &gPlayerAnim_link_fighter_rebound,
    &gPlayerAnim_link_fighter_rebound_long,
    &gPlayerAnim_link_fighter_reboundR,
    &gPlayerAnim_link_fighter_rebound_longR,
};

void func_808400CC(PlayState* play, Player* this) {
    if (Player_Action_18 != this->actionFunc) {
        func_8082DD2C(play, this);
        if ((this->transformation != PLAYER_FORM_HUMAN) && (this->transformation != PLAYER_FORM_FIERCE_DEITY)) {
            u8 savedMovementFlags = this->skelAnime.movementFlags;
            s32 pad;

            this->skelAnime.movementFlags = 0;
            Player_SetAction(play, this, Player_Action_85, 0);
            this->skelAnime.movementFlags = savedMovementFlags;
        } else {
            s32 var_v1;
            s32 pad;

            Player_SetAction(play, this, Player_Action_85, 0);
            if (Player_CheckHostileLockOn(this)) {
                var_v1 = 2;
            } else {
                var_v1 = 0;
            }
            Player_Anim_PlayOnceAdjusted(play, this, D_8085D294[Player_IsHoldingTwoHandedWeapon(this) + var_v1]);
        }
    }

    Player_RequestRumble(play, this, 180, 20, 100, SQ(0));
    this->speedXZ = -18.0f;
    func_80840094(play, this);
}

s32 func_808401F4(PlayState* play, Player* this) {
    if (this->meleeWeaponState >= PLAYER_MELEE_WEAPON_STATE_1) {
        s32 temp_v0_3;

        if (this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) {
            if (!(this->meleeWeaponQuads[0].base.atFlags & AT_BOUNCED) &&
                !(this->meleeWeaponQuads[1].base.atFlags & AT_BOUNCED)) {
                if (this->skelAnime.curFrame >= 2.0f) {
                    CollisionPoly* poly;
                    s32 bgId;
                    Vec3f spC8;
                    Vec3f pos;
                    Vec3f spB0;
                    Vec3f* var_a1;
                    Vec3f* temp_a0 = &this->meleeWeaponInfo[0].tip;
                    f32 var_fv1;

                    if (this->speedXZ >= 0.0f) {
                        var_a1 = &this->meleeWeaponInfo[0].base;
                        if ((this->transformation == PLAYER_FORM_GORON) || (this->actor.id == ACTOR_EN_TEST3)) {
                            var_a1 = &this->unk_AF0[1];
                        }

                        var_fv1 = Math_Vec3f_DistXYZAndStoreDiff(temp_a0, var_a1, &spB0);
                        if (var_fv1 != 0.0f) {
                            var_fv1 = (var_fv1 + 10.0f) / var_fv1;
                        }

                        spC8.x = temp_a0->x + (spB0.x * var_fv1);
                        spC8.y = temp_a0->y + (spB0.y * var_fv1);
                        spC8.z = temp_a0->z + (spB0.z * var_fv1);
                        if (BgCheck_EntityLineTest2(&play->colCtx, &spC8, temp_a0, &pos, &poly, true, false, false,
                                                    true, &bgId, &this->actor)) {
                            if (!SurfaceType_IsIgnoredByEntities(&play->colCtx, poly, bgId) &&
                                (SurfaceType_GetFloorType(&play->colCtx, poly, bgId) != FLOOR_TYPE_6) &&
                                !func_800B90AC(play, &this->actor, poly, bgId, &pos)) {
                                if (this->transformation == PLAYER_FORM_GORON) {
                                    MtxF sp64;
                                    Vec3s actorRot;
                                    DynaPolyActor* temp_v0;

                                    func_8082DF2C(play);
                                    Player_RequestQuakeAndRumble(play, this, NA_SE_IT_HAMMER_HIT);
                                    if (this->transformation == PLAYER_FORM_GORON) {
                                        Actor_SetPlayerImpact(play, PLAYER_IMPACT_BONK, 2, 100.0f,
                                                              &this->actor.world.pos);
                                        func_800C0094(poly, pos.x, pos.y, pos.z, &sp64);
                                        Matrix_MtxFToYXZRot(&sp64, &actorRot, true);
                                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TEST, pos.x, pos.y, pos.z,
                                                    actorRot.x, actorRot.y, actorRot.z, 500);
                                    }

                                    if (bgId != BGCHECK_SCENE) {
                                        temp_v0 = DynaPoly_GetActor(&play->colCtx, bgId);

                                        if (((this->meleeWeaponQuads[0].base.atFlags & AT_HIT) &&
                                             (&temp_v0->actor == this->meleeWeaponQuads[0].base.at)) ||
                                            ((this->meleeWeaponQuads[1].base.atFlags & AT_HIT) &&
                                             (&temp_v0->actor == this->meleeWeaponQuads[1].base.at))) {
                                            return false;
                                        }
                                    }

                                    func_808400CC(play, this);
                                    if (this->transformation != PLAYER_FORM_GORON) {
                                        return true;
                                    }
                                    return false;
                                }

                                if (this->speedXZ >= 0.0f) {
                                    SurfaceMaterial surfaceMaterial =
                                        SurfaceType_GetMaterial(&play->colCtx, poly, bgId);

                                    if (surfaceMaterial == SURFACE_MATERIAL_WOOD) {
                                        CollisionCheck_SpawnShieldParticlesWood(play, &pos, &this->actor.projectedPos);
                                    } else {
                                        pos.x += 8.0f * COLPOLY_GET_NORMAL(poly->normal.x);
                                        pos.y += 8.0f * COLPOLY_GET_NORMAL(poly->normal.y);
                                        pos.x += 8.0f * COLPOLY_GET_NORMAL(poly->normal.z);
                                        CollisionCheck_SpawnShieldParticles(play, &pos);

                                        if (surfaceMaterial == SURFACE_MATERIAL_DIRT_SOFT) {
                                            Player_PlaySfx(this, NA_SE_IT_WALL_HIT_SOFT);
                                        } else {
                                            Player_PlaySfx(this, NA_SE_IT_WALL_HIT_HARD);
                                        }
                                    }

                                    func_80840094(play, this);
                                    Player_RequestRumble(play, this, 180, 20, 100, SQ(0));
                                    this->speedXZ = -14.0f;
                                }
                            }
                        }
                    }
                }
            } else {
                func_808400CC(play, this);
                func_8082DF2C(play);
                return true;
            }
        }

        temp_v0_3 = (this->meleeWeaponQuads[0].base.atFlags & AT_HIT) != 0;
        if (temp_v0_3 || (this->meleeWeaponQuads[1].base.atFlags & AT_HIT)) {
            if ((this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) &&
                (this->transformation != PLAYER_FORM_GORON)) {
                Actor* temp_v1 = this->meleeWeaponQuads[temp_v0_3 ? 0 : 1].base.at;

                if ((temp_v1 != NULL) && (temp_v1->id != ACTOR_EN_KANBAN)) {
                    func_8082DF2C(play);
                }
            }

            if (!func_8083FF30(play, this)) {
                func_8083FFEC(play, this);
                if (this->actor.colChkInfo.atHitEffect == 1) {
                    this->actor.colChkInfo.damage = 8;
                    func_80833B18(play, this, 4, 0.0f, 0.0f, this->actor.shape.rot.y, 20);
                    return true;
                }
            }
        }
    }

    return false;
}

Vec3f D_8085D2A4 = { 0.0f, 0.0f, 5.0f };

void func_80840770(PlayState* play, Player* this) {
    if (this->av2.actionVar2 != 0) {
        if (this->av2.actionVar2 > 0) {
            this->av2.actionVar2--;
            if (this->av2.actionVar2 == 0) {
                if (this->stateFlags1 & PLAYER_STATE1_8000000) {
                    PlayerAnimation_Change(
                        play, &this->skelAnime, &gPlayerAnim_link_swimer_swim_wait, PLAYER_ANIM_NORMAL_SPEED, 0.0f,
                        Animation_GetLastFrame(&gPlayerAnim_link_swimer_swim_wait), ANIMMODE_ONCE, -16.0f);
                } else {
                    PlayerAnimation_Change(
                        play, &this->skelAnime, &gPlayerAnim_link_derth_rebirth, PLAYER_ANIM_NORMAL_SPEED, 99.0f,
                        Animation_GetLastFrame(&gPlayerAnim_link_derth_rebirth), ANIMMODE_ONCE, 0.0f);
                }
                gSaveContext.healthAccumulator = 0xA0;
                this->av2.actionVar2 = -1;
            }
        } else if (gSaveContext.healthAccumulator == 0) {
            Player_StopCutscene(this);

            this->stateFlags1 &= ~PLAYER_STATE1_DEAD;
            if (this->stateFlags1 & PLAYER_STATE1_8000000) {
                func_808353DC(play, this);
            } else {
                func_8085B384(this, play);
            }

            this->unk_D6B = 20;
            func_808339B4(this, -20);
            Audio_SetBgmVolumeOn();
        }
    } else if (this->av1.actionVar1 != 0) {
        Player_StopCutscene(this);
        this->csId = play->playerCsIds[PLAYER_CS_ID_REVIVE];
        this->av2.actionVar2 = 60;
        Player_SpawnFairy(play, this, &this->actor.world.pos, &D_8085D2A4, FAIRY_PARAMS(FAIRY_TYPE_5, false, 0));
        Player_PlaySfx(this, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
    } else if (play->gameOverCtx.state == GAMEOVER_DEATH_WAIT_GROUND) {
        play->gameOverCtx.state = GAMEOVER_DEATH_FADE_OUT;
    }
}

void func_80840980(Player* this, u16 sfxId) {
    Player_AnimSfx_PlayVoice(this, sfxId);
}

void func_808409A8(PlayState* play, Player* this, f32 speed, f32 yVelocity) {
    Actor* heldActor = this->heldActor;

    if (!func_808313A8(play, this, heldActor)) {
        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speed = speed;
        heldActor->velocity.y = yVelocity;
        func_808309CC(play, this);
        Player_PlaySfx(this, NA_SE_PL_THROW);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
    }
}

// Check if bonked and if so, rumble, play sound, etc.
s32 func_80840A30(PlayState* play, Player* this, f32* arg2, f32 arg3) {
    Actor* cylinderOc = NULL;

    if (arg3 <= *arg2) {
        // If interacting with a wall and close to facing it
        if (((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) && (sWorldYawToTouchedWall < 0x1C00)) ||
            // or, impacting something's cylinder
            (((this->cylinder.base.ocFlags1 & OC1_HIT) && (cylinderOc = this->cylinder.base.oc) != NULL) &&
             // and that something is a Beaver Race ring,
             ((cylinderOc->id == ACTOR_EN_TWIG) ||
              // or something is a tree and `this` is close to facing it (note the this actor's facing direction would
              // be antiparallel to the cylinder's actor's yaw if this was directly facing it)
              (((cylinderOc->id == ACTOR_EN_WOOD02) || (cylinderOc->id == ACTOR_EN_SNOWWD) ||
                (cylinderOc->id == ACTOR_OBJ_TREE)) &&
               (ABS_ALT(BINANG_SUB(this->actor.world.rot.y, cylinderOc->yawTowardsPlayer)) > 0x6000))))) {

            if (!func_8082DA90(play)) {
                if (this->doorType == PLAYER_DOORTYPE_STAIRCASE) {
                    func_8085B384(this, play);
                    return true;
                }

                if (GameInteractor_Should(VB_APPLY_BONK_TO_ACTOR, cylinderOc != NULL, cylinderOc)) {
                    cylinderOc->home.rot.y = 1;
                } else if (this->actor.wallBgId != BGCHECK_SCENE) { // i.e. was an actor
                    DynaPolyActor* wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);

                    // Large crates, barrels and palm trees
                    if ((wallPolyActor != NULL) &&
                        ((wallPolyActor->actor.id == ACTOR_OBJ_KIBAKO2) ||
                         (wallPolyActor->actor.id == ACTOR_OBJ_TARU) || (wallPolyActor->actor.id == ACTOR_OBJ_YASI))) {
                        wallPolyActor->actor.home.rot.z = 1;
                    }
                }

                if (!(this->stateFlags3 & PLAYER_STATE3_1000)) {
                    if ((this->stateFlags3 & PLAYER_STATE3_8000) && (Player_Action_28 != this->actionFunc)) {
                        Player_SetAction(play, this, Player_Action_61, 0);
                        Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_swimer_swim_hit);
                        func_8082DD2C(play, this);
                        this->speedXZ *= 0.2f;
                    } else {
                        Player_SetAction(play, this, Player_Action_26, 0);
                        Player_Anim_PlayOnce(play, this, D_8085BE84[PLAYER_ANIMGROUP_hip_down][this->modelAnimType]);
                        this->av2.actionVar2 = 1;
                    }
                }

                this->speedXZ = -this->speedXZ;
                Player_RequestQuake(play, 33267, 3, 12);
                Player_RequestRumble(play, this, 255, 20, 150, SQ(0));
                Actor_SetPlayerImpact(play, PLAYER_IMPACT_BONK, 2, 100.0f, &this->actor.world.pos);
                Player_PlaySfx(this, NA_SE_PL_BODY_HIT);
                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_CLIMB_END);
                return true;
            }
        }
    }
    return false;
}

s32 func_80840CD4(Player* this, PlayState* play) {
    if (Player_StartCsAction(play, this)) {
        this->stateFlags2 |= PLAYER_STATE2_20000;
    } else if (!GameInteractor_Should(VB_CHECK_HELD_ITEM_BUTTON_PRESS,
                                      CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_B), sDpadItemButtons,
                                      sPlayerItemButtons)) {
        PlayerMeleeWeaponAnimation meleeWeaponAnim;

        if ((this->unk_B08 >= 0.85f) || Player_CanSpinAttack(this)) {
            meleeWeaponAnim = D_8085CF84[Player_IsHoldingTwoHandedWeapon(this)];
        } else {
            meleeWeaponAnim = D_8085CF80[Player_IsHoldingTwoHandedWeapon(this)];
        }
        func_80833864(play, this, meleeWeaponAnim);
        func_808339B4(this, -8);
        this->stateFlags2 |= PLAYER_STATE2_20000;
        if (this->controlStickDirections[this->controlStickDataIndex] == PLAYER_STICK_DIR_FORWARD) {
            this->stateFlags2 |= PLAYER_STATE2_40000000;
        }
    } else {
        return false;
    }

    return true;
}

void func_80840DEC(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_31, 1);
}

void func_80840E24(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_32, 1);
}

void func_80840E5C(Player* this, PlayState* play) {
    func_808369F4(this, play);
    func_8082DC38(this);
    Player_Anim_PlayOnceMorph(play, this, D_8085CF68[Player_IsHoldingTwoHandedWeapon(this)]);
    this->yaw = this->actor.shape.rot.y;
}

void func_80840EC0(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_30, 1);
    Player_Anim_PlayLoop(play, this, D_8085CF60[Player_IsHoldingTwoHandedWeapon(this)]);
    this->av2.actionVar2 = 1;
    this->unk_B38 = 0.0f;
}

// Spin attack size
void func_80840F34(Player* this) {
    Math_StepToF(&this->unk_B08, CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK) ? 1.0f : 0.5f, 0.02f);
}

s32 func_80840F90(PlayState* play, Player* this, CsCmdActorCue* cue, f32 arg3, s16 arg4, s32 arg5) {
    if ((arg5 != 0) && (this->speedXZ == 0.0f)) {
        return PlayerAnimation_Update(play, &this->skelAnime);
    }

    if (arg5 != 2) {
        f32 halfUpdateRate = R_UPDATE_RATE / 2.0f;
        f32 curDiffX = cue->endPos.x - this->actor.world.pos.x;
        f32 curDiffZ = cue->endPos.z - this->actor.world.pos.z;
        f32 scaledCurDist = sqrtf(SQ(curDiffX) + SQ(curDiffZ)) / halfUpdateRate;
        s32 framesLeft = (cue->endFrame - play->csCtx.curFrame) + 1;

        arg4 = Math_Atan2S_XY(curDiffZ, curDiffX);

        if (arg5 == 1) {
            f32 distX = cue->endPos.x - cue->startPos.x;
            f32 distZ = cue->endPos.z - cue->startPos.z;
            s32 temp =
                (((sqrtf(SQ(distX) + SQ(distZ)) / halfUpdateRate) / (cue->endFrame - cue->startFrame)) / 1.5f) * 4.0f;
            if (temp >= framesLeft) {
                arg3 = 0.0f;
                arg4 = this->actor.shape.rot.y;
            } else {
                arg3 = scaledCurDist / ((framesLeft - temp) + 1);
            }
        } else {
            arg3 = scaledCurDist / framesLeft;
        }
    }

    this->stateFlags2 |= PLAYER_STATE2_20;
    func_8083F57C(this, play);
    func_8083CB58(this, arg3, arg4);
    if ((arg3 == 0.0f) && (this->speedXZ == 0.0f)) {
        func_80839CD8(this, play);
    }

    return false;
}

s32 func_808411D4(PlayState* play, Player* this, f32* arg2, s32 arg3) {
    f32 xDiff = this->unk_3A0.x - this->actor.world.pos.x;
    f32 yDiff = this->unk_3A0.z - this->actor.world.pos.z;
    s32 sp2C;
    s32 pad2;
    s16 var_v1;

    sp2C = sqrtf(SQ(xDiff) + SQ(yDiff));
    var_v1 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_3A0);
    if (sp2C < arg3) {
        *arg2 = 0.0f;
        var_v1 = this->actor.shape.rot.y;
    }
    if (func_80840F90(play, this, NULL, *arg2, var_v1, 2)) {
        return 0;
    }
    return sp2C;
}

void Player_StartMode_Nothing(PlayState* play, Player* this) {
    this->actor.update = Player_DoNothing;
    this->actor.draw = NULL;
}

void Player_StartMode_BlueWarp(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_BlueWarpArrive, 0);

    this->stateFlags1 |= PLAYER_STATE1_20000000;
    PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_okarina_warp_goal, PLAYER_ANIM_ADJUSTED_SPEED,
                           0.0f, 24.0f, ANIMMODE_ONCE, 0.0f);

    // Start high up in the air
    this->actor.world.pos.y += 800.0f;
}

/**
 * Put the sword item in hand. If `playSfx` is true, the sword unsheathing sound will play.
 * Sword will depend on transformation, but due to improper carryover from OoT,
 * this will lead to OoB for goron, deku or human.
 *
 * Note: This will not play an animation, the sword instantly appears in hand.
 *       It is expected that this function is called while an appropriate animation
 *       is already playing, for example in a cutscene.
 */
void Player_PutSwordInHand(PlayState* play, Player* this, s32 playSfx) {
    static u8 sSwordItemIds[] = { ITEM_SWORD_RAZOR, ITEM_SWORD_KOKIRI };
    ItemId swordItemId;
    //! @bug OoB read if player is goron, deku or human
    // 2S2H [Port] - Set item to kokiri sword instead of OOB behaviour
    if (this->transformation >= 2) {
        swordItemId = ITEM_SWORD_KOKIRI;
    } else {
        swordItemId = sSwordItemIds[this->transformation];
    }
    PlayerItemAction swordItemAction = sItemItemActions[swordItemId];
    Player_DestroyHookshot(this);
    Player_DetachHeldActor(play, this);

    this->heldItemId = swordItemId;
    this->nextModelGroup = Player_ActionToModelGroup(this, swordItemAction);

    Player_InitItemAction(play, this, swordItemAction);
    func_808309CC(play, this);

    if (playSfx) {
        Player_PlaySfx(this, NA_SE_IT_SWORD_PICKOUT);
    }
}

void Player_StartMode_TimeTravel(PlayState* play, Player* this) {
    static Vec3f sPedestalPos = { -1.0f, 69.0f, 20.0f };

    Player_SetAction(play, this, Player_Action_TimeTravelEnd, 0);
    this->stateFlags1 |= PLAYER_STATE1_20000000;

    Math_Vec3f_Copy(&this->actor.world.pos, &sPedestalPos);
    this->yaw = this->actor.shape.rot.y = -0x8000;

    // The start frame and end frame are both set to 0 so that that the animation is frozen.
    // `Player_Action_TimeTravelEnd` will play the animation after `animDelayTimer` completes.
    PlayerAnimation_Change(play, &this->skelAnime, this->ageProperties->timeTravelEndAnim, PLAYER_ANIM_ADJUSTED_SPEED,
                           0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
    Player_AnimReplace_Setup(play, this,
                             ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_4 | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_80 |
                                 ANIM_FLAG_200);

    if (this->transformation == PLAYER_FORM_FIERCE_DEITY) {
        Player_PutSwordInHand(play, this, false);
    }

    this->av2.animDelayTimer = 20;
}

void Player_StartMode_Door(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_TryOpeningDoor, 0);
    Player_AnimReplace_Setup(
        play, this, ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE | ANIM_FLAG_80);
}

void Player_StartMode_Grotto(PlayState* play, Player* this) {
    func_80834DB8(this, &gPlayerAnim_link_normal_jump, 12.0f, play);
    Player_SetAction(play, this, Player_Action_ExitGrotto, 0);
    this->stateFlags1 |= PLAYER_STATE1_20000000;
    this->fallStartHeight = this->actor.world.pos.y;
}

void Player_StartMode_KnockedOver(PlayState* play, Player* this) {
    func_80833B18(play, this, 1, 2.0f, 2.0f, this->actor.shape.rot.y + 0x8000, 0);
}

void Player_StartMode_WarpSong(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_StartWarpSongArrive, 0);
    this->actor.draw = NULL; // Start invisible
    this->stateFlags1 |= PLAYER_STATE1_20000000;
}

void Player_StartMode_Owl(PlayState* play, Player* this) {
    if (gSaveContext.save.isOwlSave) {
        Player_SetAction(play, this, Player_Action_OwlSaveArrive, 0);
        Player_Anim_PlayLoopMorph(play, this, D_8085BE84[PLAYER_ANIMGROUP_nwait][this->modelAnimType]);
        this->stateFlags1 |= PLAYER_STATE1_20000000;
        this->av2.actionVar2 = 40;
        gSaveContext.save.isOwlSave = false;
    } else {
        Player_SetAction(play, this, Player_Action_Idle, 0);
        Player_Anim_PlayLoopMorph(play, this, D_8085BE84[PLAYER_ANIMGROUP_nwait][this->modelAnimType]);
        this->stateFlags1 |= PLAYER_STATE1_20000000;
        this->stateFlags2 |= PLAYER_STATE2_20000000;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TEST7, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, ENTEST7_ARRIVE);
    }
}

void Player_StartMode_WarpTag(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_91, 0);
    if (PLAYER_GET_START_MODE(&this->actor) == PLAYER_START_MODE_8) {
        Player_Anim_PlayOnceAdjustedReverse(play, this, D_8085D17C[this->transformation]);
        this->itemAction = PLAYER_IA_OCARINA;
        Player_SetModels(this, Player_ActionToModelGroup(this, this->itemAction));
    } else {
        Player_Anim_PlayLoopAdjusted(play, this, D_8085BE84[PLAYER_ANIMGROUP_nwait][this->modelAnimType]);
    }
    this->stateFlags1 |= PLAYER_STATE1_20000000;
    this->unk_ABC = -10000.0f;
    this->av2.actionVar2 = 0x2710;
    this->unk_B10[5] = 8.0f;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_STOP),
};

Vec3s sPlayerSkeletonBaseTransl = { -57, 3377, 0 };

void Player_InitCommon(Player* this, PlayState* play, FlexSkeletonHeader* skelHeader) {
    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->yaw = this->actor.world.rot.y;

    if ((PLAYER_GET_START_MODE(&this->actor) != PLAYER_START_MODE_TELESCOPE) &&
        ((gSaveContext.respawnFlag != 2) || (gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams !=
                                             PLAYER_PARAMS(0xFF, PLAYER_START_MODE_TELESCOPE)))) {
        func_808309CC(play, this);
        SkelAnime_InitPlayer(play, &this->skelAnime, skelHeader, D_8085BE84[PLAYER_ANIMGROUP_wait][this->modelAnimType],
                             1 | 8, this->jointTableBuffer, this->morphTableBuffer, PLAYER_LIMB_MAX);
        this->skelAnime.baseTransl = sPlayerSkeletonBaseTransl;

        SkelAnime_InitPlayer(play, &this->skelAnimeUpper, skelHeader, Player_GetIdleAnim(this), 1 | 8,
                             this->jointTableUpperBuffer, this->morphTableUpperBuffer, PLAYER_LIMB_MAX);
        this->skelAnimeUpper.baseTransl = sPlayerSkeletonBaseTransl;

        if (this->transformation == PLAYER_FORM_GORON) {
            SkelAnime_InitFlex(play, &this->unk_2C8, &gLinkGoronShieldingSkel, &gLinkGoronShieldingAnim,
                               this->jointTable, this->morphTable, LINK_GORON_SHIELDING_LIMB_MAX);
        }

        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFeet, this->ageProperties->shadowScale);
    }

    this->subCamId = CAM_ID_NONE;
    Collider_InitAndSetCylinder(play, &this->cylinder, &this->actor, &D_8085C2EC);
    Collider_InitAndSetCylinder(play, &this->shieldCylinder, &this->actor, &D_8085C318);
    Collider_InitAndSetQuad(play, &this->meleeWeaponQuads[0], &this->actor, &D_8085C344);
    Collider_InitAndSetQuad(play, &this->meleeWeaponQuads[1], &this->actor, &D_8085C344);
    Collider_InitAndSetQuad(play, &this->shieldQuad, &this->actor, &D_8085C394);
}

void func_80841A50(PlayState* play, Player* this) {
    if ((play->roomCtx.curRoom.num >= 0) && (play->roomCtx.prevRoom.num < 0)) {
        Math_Vec3f_Copy(&this->unk_3C0, &this->actor.world.pos);
        this->unk_3CC = this->actor.shape.rot.y;
        this->unk_3CE = play->roomCtx.curRoom.num;
        this->unk_3CF = 1;
    }
}

typedef void (*PlayerStartModeFunc)(PlayState*, Player*);

// Initialisation functions for various gameplay modes depending on spawn params.
// There may be at most 0x10 due to it using a single nybble.
PlayerStartModeFunc sStartModeFuncs[PLAYER_START_MODE_MAX] = {
    Player_StartMode_Nothing,     // PLAYER_START_MODE_NOTHING
    Player_StartMode_TimeTravel,  // PLAYER_START_MODE_TIME_TRAVEL
    Player_StartMode_BlueWarp,    // PLAYER_START_MODE_BLUE_WARP
    Player_StartMode_Door,        // PLAYER_START_MODE_DOOR
    Player_StartMode_Grotto,      // PLAYER_START_MODE_GROTTO
    Player_StartMode_WarpSong,    // PLAYER_START_MODE_WARP_SONG
    Player_StartMode_Owl,         // PLAYER_START_MODE_OWL
    Player_StartMode_KnockedOver, // PLAYER_START_MODE_KNOCKED_OVER
    Player_StartMode_WarpTag,     // PLAYER_START_MODE_8
    Player_StartMode_WarpTag,     // PLAYER_START_MODE_9
    Player_StartMode_E,           // PLAYER_START_MODE_A
    Player_StartMode_B,           // PLAYER_START_MODE_B
    Player_StartMode_Telescope,   // PLAYER_START_MODE_TELESCOPE
    Player_StartMode_D,           // PLAYER_START_MODE_D
    Player_StartMode_E,           // PLAYER_START_MODE_E
    Player_StartMode_F,           // PLAYER_START_MODE_F
};

// sBlureInit
EffectBlureInit2 D_8085D30C = {
    0,
    EFFECT_BLURE_ELEMENT_FLAG_8,
    0,
    { 255, 255, 255, 255 },
    { 255, 255, 255, 64 },
    { 255, 255, 255, 0 },
    { 255, 255, 255, 0 },
    4,
    0,
    EFF_BLURE_DRAW_MODE_SMOOTH,
    0,
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
};

// sTireMarkInit ?
EffectTireMarkInit D_8085D330 = { 0, 63, { 0, 0, 15, 100 } };

// sTireMarkGoronColor ?
Color_RGBA8 D_8085D338 = { 0, 0, 15, 100 };
// sTireMarkOtherColor ?
Color_RGBA8 D_8085D33C = { 0, 0, 0, 150 };

void Player_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    Player* this = (Player*)thisx;
    s8 objectSlot;
    s32 respawnFlag;
    s32 var_a1;
    PlayerStartMode startMode;

    play->playerInit = Player_InitCommon;
    play->playerUpdate = Player_UpdateCommon;
    play->unk_18770 = func_8085B170;
    play->startPlayerFishing = Player_StartFishing;
    play->grabPlayer = Player_GrabPlayer;
    play->tryPlayerCsAction = Player_TryCsAction;
    play->func_18780 = func_8085B384;
    play->damagePlayer = Player_InflictDamage;
    play->talkWithPlayer = Player_StartTalking;
    play->unk_1878C = func_8085B74C;
    play->unk_18790 = func_8085B820;
    play->unk_18794 = func_8085B854;
    play->setPlayerTalkAnim = func_8085B930;

    gActorOverlayTable[ACTOR_PLAYER].profile->objectId = GAMEPLAY_KEEP;

    this->actor.room = -1;
    this->csId = CS_ID_NONE;

    if (this->actor.shape.rot.x != 0) {
        this->transformation = this->actor.shape.rot.x - 1;

        objectSlot = Object_GetSlot(&play->objectCtx, gPlayerFormObjectIds[this->transformation]);
        this->actor.objectSlot = objectSlot;
        if (objectSlot <= OBJECT_SLOT_NONE) {
            Actor_Kill(&this->actor);
            return;
        }

        Actor_SetObjectDependency(play, &this->actor);
    } else {
        this->transformation = GET_PLAYER_FORM;
        if (this->transformation == PLAYER_FORM_HUMAN) {
            if (gSaveContext.save.equippedMask == PLAYER_MASK_GIANT) {
                gSaveContext.save.equippedMask = PLAYER_MASK_NONE;
            }
            this->currentMask = gSaveContext.save.equippedMask;
        } else {
            this->currentMask = this->transformation + PLAYER_MASK_FIERCE_DEITY;
            gSaveContext.save.equippedMask = PLAYER_MASK_NONE;
        }

        Inventory_UpdateDeitySwordEquip(play);

        this->unk_B28 = 0;
        this->unk_B90 = 0;
        this->unk_B92 = 0;
        this->unk_B94 = 0;
        this->unk_B96 = 0;
        this->stateFlags1 &= ~(PLAYER_STATE1_8 | PLAYER_STATE1_CHARGING_SPIN_ATTACK |
                               PLAYER_STATE1_USING_ZORA_BOOMERANG | PLAYER_STATE1_ZORA_BOOMERANG_THROWN);
        this->stateFlags2 &= ~(PLAYER_STATE2_20000 | PLAYER_STATE2_1000000 | PLAYER_STATE2_40000000);
        this->stateFlags3 &= ~(PLAYER_STATE3_8 | PLAYER_STATE3_40 | PLAYER_STATE3_FLYING_WITH_HOOKSHOT |
                               PLAYER_STATE3_100 | PLAYER_STATE3_200 | PLAYER_STATE3_800 | PLAYER_STATE3_1000 |
                               PLAYER_STATE3_2000 | PLAYER_STATE3_8000 | PLAYER_STATE3_10000 | PLAYER_STATE3_40000 |
                               PLAYER_STATE3_80000 | PLAYER_STATE3_100000 | PLAYER_STATE3_200000 |
                               PLAYER_STATE3_ZORA_BOOMERANG_CAUGHT | PLAYER_STATE3_1000000 | PLAYER_STATE3_2000000);
        this->unk_B08 = 0.0f;
        this->unk_B0C = 0.0f;
    }

    if (this->transformation == PLAYER_FORM_ZORA) {
        if (this->stateFlags1 & PLAYER_STATE1_8000000) {
            this->unk_B10[0] = 1.0f;
        } else {
            this->unk_B10[0] = 0.0f;
        }
    }

    this->actor.flags &= ~(ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES | ACTOR_FLAG_CAN_PRESS_SWITCHES);
    if (this->transformation != PLAYER_FORM_DEKU) {
        this->actor.flags |= ACTOR_FLAG_CAN_PRESS_SWITCHES;
        if (this->transformation == PLAYER_FORM_GORON) {
            this->actor.flags |= ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES;
        }
    }

    this->ageProperties = &sPlayerAgeProperties[this->transformation];

    this->itemAction = PLAYER_IA_NONE;
    this->heldItemAction = PLAYER_IA_NONE;
    this->heldItemId = ITEM_NONE;

    Player_UseItem(play, this, ITEM_NONE);
    Player_SetEquipmentData(play, this);
    this->prevBoots = this->currentBoots;
    Player_InitCommon(this, play, gPlayerSkeletons[this->transformation]);

    if (this->actor.shape.rot.z != 0) {
        EffectTireMark* tireMark;

        this->actor.shape.rot.z = 0;
        Player_OverrideBlureColors(play, this, 0, 4);

        tireMark = Effect_GetByIndex(this->meleeWeaponEffectIndex[2]);
        if (this->transformation == PLAYER_FORM_GORON) {
            tireMark->color = D_8085D338;
        } else {
            tireMark->color = D_8085D33C;
        }

        if ((this->csAction == PLAYER_CSACTION_9) || (this->csAction == PLAYER_CSACTION_93)) {
            Player_SetAction(play, this, Player_Action_CsAction, 0);
            this->stateFlags1 |= PLAYER_STATE1_20000000;
        } else {
            Player_SetAction(play, this, Player_Action_87, 0);
            this->actor.shape.rot.y = this->yaw;

            if (this->prevMask != PLAYER_MASK_NONE) {
                Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_cl_maskoff);
            } else if (this->transformation == PLAYER_FORM_HUMAN) {
                PlayerAnimation_Change(play, &this->skelAnime, D_8085D160[this->transformation],
                                       -PLAYER_ANIM_ADJUSTED_SPEED, 9.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
            } else {
                Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_cl_setmaskend);
            }

            this->stateFlags1 |= (PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000);
            this->stateFlags3 |= PLAYER_STATE3_20000;
            this->unk_B10[5] = 3.0f;
        }
        return;
    }

    this->prevMask = this->currentMask;

    Effect_Add(play, &this->meleeWeaponEffectIndex[0], EFFECT_BLURE2, 0, 0, &D_8085D30C);
    Effect_Add(play, &this->meleeWeaponEffectIndex[1], EFFECT_BLURE2, 0, 0, &D_8085D30C);

    Player_OverrideBlureColors(play, this, 0, 4);
    if (this->transformation == PLAYER_FORM_GORON) {
        D_8085D330.color = D_8085D338;
    } else {
        D_8085D330.color = D_8085D33C;
    }
    Effect_Add(play, &this->meleeWeaponEffectIndex[2], EFFECT_TIRE_MARK, 0, 0, &D_8085D330);

    if (this->actor.shape.rot.x != 0) {
        this->actor.shape.rot.x = 0;
        this->csAction = PLAYER_CSACTION_68;
        Player_SetAction(play, this, Player_Action_CsAction, 0);
        this->stateFlags1 |= PLAYER_STATE1_20000000;
        return;
    }

    play->bButtonAmmoPlusOne = 0;
    play->unk_1887D = 0;
    play->unk_1887E = 0;
    this->giObjectSegment = ZeldaArena_Malloc(0x2000);
    this->maskObjectSegment = ZeldaArena_Malloc(0x3800);

    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 255, 128, 0, -1);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    Play_AssignPlayerCsIdsFromScene(play, this->actor.csId);

    respawnFlag = gSaveContext.respawnFlag;
    if (respawnFlag != 0) {
        if (respawnFlag == -3) {
            this->actor.params = gSaveContext.respawn[RESPAWN_MODE_UNK_3].playerParams;
        } else {
            if ((respawnFlag == 1) || (respawnFlag == -1)) {
                this->unk_D6A = -2;
            }

            if (respawnFlag != -7) {
                s32 respawnIndex;

                if ((respawnFlag == -8) || (respawnFlag == -5) || (respawnFlag == -4)) {
                    respawnFlag = 1;
                }

                if ((respawnFlag < 0) && (respawnFlag != -1) && (respawnFlag != -6)) {
                    respawnIndex = RESPAWN_MODE_DOWN;
                } else {
                    respawnIndex = (respawnFlag < 0) ? RESPAWN_MODE_TOP : respawnFlag - 1;

                    Math_Vec3f_Copy(&this->actor.world.pos, &gSaveContext.respawn[respawnIndex].pos);
                    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
                    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
                    Math_Vec3f_Copy(&this->actor.focus.pos, &this->actor.world.pos);

                    this->fallStartHeight = this->actor.world.pos.y;

                    this->yaw = this->actor.shape.rot.y = gSaveContext.respawn[respawnIndex].yaw;
                    this->actor.params = gSaveContext.respawn[respawnIndex].playerParams;
                }

                play->actorCtx.sceneFlags.switches[2] = gSaveContext.respawn[respawnIndex].tempSwitchFlags;
                play->actorCtx.sceneFlags.collectible[1] = gSaveContext.respawn[respawnIndex].unk_18;
                play->actorCtx.sceneFlags.collectible[2] = gSaveContext.respawn[respawnIndex].tempCollectFlags;
            }
        }
    }

    var_a1 = ((respawnFlag == 4) || (gSaveContext.respawnFlag == -4)) ? 1 : 0;
    if (func_801226E0(play, var_a1) == 0) {
        gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = PLAYER_PARAMS(thisx->params, PLAYER_START_MODE_D);
    }

    gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 1;
    if (respawnFlag == 0) {
        gSaveContext.respawn[RESPAWN_MODE_TOP] = gSaveContext.respawn[RESPAWN_MODE_DOWN];
    }
    gSaveContext.respawn[RESPAWN_MODE_TOP].playerParams =
        PLAYER_PARAMS(gSaveContext.respawn[RESPAWN_MODE_TOP].playerParams, PLAYER_START_MODE_D);

    startMode = PLAYER_GET_START_MODE(&this->actor);

    if (((startMode == PLAYER_START_MODE_WARP_SONG) || (startMode == PLAYER_START_MODE_OWL)) &&
        (gSaveContext.save.cutsceneIndex >= 0xFFF0)) {
        startMode = PLAYER_START_MODE_D;
    }

    // 2S2H [Enhancement] When we have a pause save entrance, we need unset the values to prevent them from lingering
    // Load into PLAYER_START_MODE_D for stationary Link spawn
    if (gSaveContext.save.shipSaveInfo.pauseSaveEntrance != -1) {
        startMode = PLAYER_START_MODE_D;
        gSaveContext.save.shipSaveInfo.pauseSaveEntrance = -1;
        gSaveContext.save.isOwlSave = false;
    }

    sStartModeFuncs[startMode](play, this);

    if ((this->actor.draw != NULL) && gSaveContext.save.hasTatl &&
        ((gSaveContext.gameMode == GAMEMODE_NORMAL) || (gSaveContext.gameMode == GAMEMODE_END_CREDITS)) &&
        (play->sceneId != SCENE_SPOT00)) {
        static Vec3f sTatlSpawnPosOffset = { 0.0f, 50.0f, 0.0f };

        this->tatlActor = Player_SpawnFairy(play, this, &this->actor.world.pos, &sTatlSpawnPosOffset,
                                            FAIRY_PARAMS(FAIRY_TYPE_0, false, 0));

        if (gSaveContext.dogParams != 0) {
            gSaveContext.dogParams |= 0x8000;
        }

        if (gSaveContext.powderKegTimer != 0) {
            this->nextModelGroup = Player_ActionToModelGroup(this, PLAYER_IA_POWDER_KEG);
            this->heldItemId = ITEM_POWDER_KEG;
            Player_InitItemAction(play, this, PLAYER_IA_POWDER_KEG);
            func_808313F0(this, play);
        } else if (gSaveContext.unk_1014 != 0) {
            func_8082F5FC(this, Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_MM,
                                                   this->actor.world.pos.x, this->actor.world.pos.y,
                                                   this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0x8000));
            func_808313F0(this, play);
        }
    }

    Map_SetAreaEntrypoint(play);
    func_80841A50(play, this);
    this->unk_3CF = 0;
    R_PLAY_FILL_SCREEN_ON = 0;
}

void Player_ApproachZeroBinang(s16* pValue) {
    s16 step;

    step = (ABS_ALT(*pValue) * 100.0f) / 1000.0f;
    step = CLAMP(step, 0x190, 0xFA0);

    Math_ScaledStepToS(pValue, 0, step);
}

void func_808425B4(Player* this) {
    if (!(this->unk_AA6_rotFlags & UNKAA6_ROT_FOCUS_Y)) {
        s16 diff = this->actor.focus.rot.y - this->actor.shape.rot.y;

        Player_ApproachZeroBinang(&diff);
        this->actor.focus.rot.y = this->actor.shape.rot.y + diff;
    }

    if (!(this->unk_AA6_rotFlags & UNKAA6_ROT_FOCUS_X)) {
        Player_ApproachZeroBinang(&this->actor.focus.rot.x);
    }

    if (!(this->unk_AA6_rotFlags & UNKAA6_ROT_HEAD_X)) {
        Player_ApproachZeroBinang(&this->headLimbRot.x);
    }

    if (!(this->unk_AA6_rotFlags & UNKAA6_ROT_UPPER_X)) {
        Player_ApproachZeroBinang(&this->upperLimbRot.x);
    }

    if (!(this->unk_AA6_rotFlags & UNKAA6_ROT_FOCUS_Z)) {
        Player_ApproachZeroBinang(&this->actor.focus.rot.z);
    }

    if (!(this->unk_AA6_rotFlags & UNKAA6_ROT_HEAD_Y)) {
        Player_ApproachZeroBinang(&this->headLimbRot.y);
    }

    if (!(this->unk_AA6_rotFlags & UNKAA6_ROT_HEAD_Z)) {
        Player_ApproachZeroBinang(&this->headLimbRot.z);
    }

    if (!(this->unk_AA6_rotFlags & UNKAA6_ROT_UPPER_Y)) {
        if (this->upperLimbYawSecondary != 0) {
            Player_ApproachZeroBinang(&this->upperLimbYawSecondary);
        } else {
            Player_ApproachZeroBinang(&this->upperLimbRot.y);
        }
    }

    if (!(this->unk_AA6_rotFlags & UNKAA6_ROT_UPPER_Z)) {
        Player_ApproachZeroBinang(&this->upperLimbRot.z);
    }

    this->unk_AA6_rotFlags = 0;
}

/**
 * Updates the two main interface elements that player is responsible for:
 *     - Do Action label on the A/B buttons
 *     - Tatl C-up icon for hints
 */
void Player_UpdateInterface(PlayState* play, Player* this) {
    DoAction doActionB;
    s32 sp38;

    if (this != GET_PLAYER(play)) {
        return;
    }

    doActionB = DO_ACTION_UNDEFINED;
    sp38 = func_801242B4(this) || (Player_Action_28 == this->actionFunc);

    // Set B do action
    if (this->transformation == PLAYER_FORM_GORON) {
        if (this->stateFlags3 & PLAYER_STATE3_80000) {
            doActionB = DO_ACTION_NONE;
        } else if (this->stateFlags3 & PLAYER_STATE3_1000) {
            doActionB = DO_ACTION_POUND;
        } else {
            doActionB = DO_ACTION_PUNCH;
        }
    } else if (this->transformation == PLAYER_FORM_ZORA) {
        if ((!(this->stateFlags1 & PLAYER_STATE1_8000000)) ||
            (!sp38 && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
            doActionB = DO_ACTION_PUNCH;
        } else {
            doActionB = DO_ACTION_DIVE;
        }
    } else if (this->transformation == PLAYER_FORM_DEKU) {
        doActionB = DO_ACTION_SHOOT;
    } else { // PLAYER_FORM_HUMAN
        if (this->currentMask == PLAYER_MASK_BLAST) {
            doActionB = DO_ACTION_EXPLODE;
        } else if (this->currentMask == PLAYER_MASK_BREMEN) {
            doActionB = DO_ACTION_MARCH;
        } else if (this->currentMask == PLAYER_MASK_KAMARO) {
            doActionB = DO_ACTION_DANCE;
        }
    }

    if (doActionB > DO_ACTION_UNDEFINED) {
        Interface_SetBButtonPlayerDoAction(play, doActionB);
    } else if (play->interfaceCtx.bButtonPlayerDoActionActive) {
        play->interfaceCtx.bButtonPlayerDoActionActive = false;
        play->interfaceCtx.bButtonPlayerDoAction = 0;
    }

    // Set A do action
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) ||
        ((play->msgCtx.currentTextId >= 0x100) && (play->msgCtx.currentTextId <= 0x200)) ||
        ((play->msgCtx.currentTextId >= 0x1BB2) && (play->msgCtx.currentTextId < 0x1BB7))) {
        Actor* heldActor = this->heldActor;
        Actor* interactRangeActor = this->interactRangeActor;
        s32 pad;
        s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];
        s32 sp24;
        DoAction doActionA =
            ((this->transformation == PLAYER_FORM_GORON) && !(this->stateFlags1 & PLAYER_STATE1_400000))
                ? DO_ACTION_CURL
                : DO_ACTION_NONE;

        if (play->actorCtx.flags & ACTORCTX_FLAG_PICTO_BOX_ON) {
            doActionA = DO_ACTION_SNAP;
        } else if (Player_InBlockingCsMode(play, this) || (this->actor.flags & ACTOR_FLAG_OCARINA_INTERACTION) ||
                   (this->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) ||
                   (this->stateFlags3 & PLAYER_STATE3_80000) || (Player_Action_80 == this->actionFunc)) {
            doActionA = DO_ACTION_NONE;
        } else if (this->stateFlags1 & PLAYER_STATE1_100000) {
            doActionA = DO_ACTION_RETURN;
        } else if ((this->heldItemAction == PLAYER_IA_FISHING_ROD) && (this->unk_B28 != 0)) {
            doActionA = (this->unk_B28 == 2) ? DO_ACTION_REEL : DO_ACTION_NONE;
        } else if (this->stateFlags3 & PLAYER_STATE3_2000) {
            doActionA = DO_ACTION_DOWN;
        } else if ((this->doorType != PLAYER_DOORTYPE_NONE) && (this->doorType != PLAYER_DOORTYPE_STAIRCASE) &&
                   !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            doActionA = DO_ACTION_OPEN;
        } else if (this->stateFlags3 & PLAYER_STATE3_200000) {
            static u8 D_8085D34C[] = {
                DO_ACTION_1, DO_ACTION_2, DO_ACTION_3, DO_ACTION_4, DO_ACTION_5, DO_ACTION_6, DO_ACTION_7, DO_ACTION_8,
            };

            doActionA = D_8085D34C[this->remainingHopsCounter];
        } else if ((!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) || (heldActor == NULL)) &&
                   (interactRangeActor != NULL) && (this->getItemId < GI_NONE)) {
            doActionA = DO_ACTION_OPEN;
        } else if (!sp38 && (this->stateFlags2 & PLAYER_STATE2_1)) {
            doActionA = DO_ACTION_GRAB;
        } else if ((this->stateFlags2 & PLAYER_STATE2_4) ||
                   (!(this->stateFlags1 & PLAYER_STATE1_800000) && (this->rideActor != NULL))) {
            doActionA = DO_ACTION_CLIMB;
        } else if ((this->stateFlags1 & PLAYER_STATE1_800000) &&
                   (!EN_HORSE_CHECK_4((EnHorse*)this->rideActor) && (Player_Action_53 != this->actionFunc))) {
            if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
                if ((this->talkActor->category == ACTORCAT_NPC) || (this->talkActor->id == ACTOR_DM_CHAR08)) {
                    doActionA = DO_ACTION_SPEAK;
                } else {
                    doActionA = DO_ACTION_CHECK;
                }
            } else if (!func_8082DA90(play) && !func_800B7128(this) && !(this->stateFlags1 & PLAYER_STATE1_100000)) {
                doActionA = DO_ACTION_FASTER;
            } else {
                doActionA = DO_ACTION_NONE;
            }
        } else if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
            if ((this->talkActor->category == ACTORCAT_NPC) || (this->talkActor->category == ACTORCAT_ENEMY) ||
                (this->talkActor->id == ACTOR_DM_CHAR08)) {
                doActionA = DO_ACTION_SPEAK;
            } else {
                doActionA = DO_ACTION_CHECK;
            }
        } else if ((this->stateFlags1 & (PLAYER_STATE1_2000 | PLAYER_STATE1_200000)) ||
                   ((this->stateFlags1 & PLAYER_STATE1_800000) && (this->stateFlags2 & PLAYER_STATE2_400000))) {
            doActionA = DO_ACTION_DOWN;
        } else if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->getItemId == GI_NONE) &&
                   (heldActor != NULL)) {
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (heldActor->id == ACTOR_EN_NIW)) {
                if (!Player_CanThrowCarriedActor(this, heldActor)) {
                    doActionA = DO_ACTION_DROP;
                } else {
                    doActionA = DO_ACTION_THROW;
                }
            } else {
                doActionA = DO_ACTION_NONE;
            }
        } else if (this->stateFlags2 & PLAYER_STATE2_10000) {
            doActionA = DO_ACTION_GRAB;
        } else if (this->stateFlags2 & PLAYER_STATE2_800) {
            static u8 D_8085D354[] = { DO_ACTION_1, DO_ACTION_2 };
            s32 var_v0;

            var_v0 = ((120.0f - this->actor.depthInWater) / 40.0f);
            var_v0 = CLAMP(var_v0, 0, ARRAY_COUNT(D_8085D354) - 1);

            doActionA = D_8085D354[var_v0];
        } else if (this->stateFlags3 & PLAYER_STATE3_100) {
            doActionA = DO_ACTION_JUMP;
        } else if (this->stateFlags3 & PLAYER_STATE3_1000) {
            doActionA = DO_ACTION_RETURN;
        } else if (!Player_IsZTargeting(this) && (this->stateFlags1 & PLAYER_STATE1_8000000) && !sp38) {
            doActionA = DO_ACTION_SURFACE;
        } else if (((this->transformation != PLAYER_FORM_DEKU) &&
                    (sp38 || ((this->stateFlags1 & PLAYER_STATE1_8000000) &&
                              !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)))) ||
                   ((this->transformation == PLAYER_FORM_DEKU) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
                    func_800C9DDC(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            doActionA = (this->transformation == PLAYER_FORM_ZORA) ? DO_ACTION_SWIM
                        : ((this->stateFlags1 & PLAYER_STATE1_8000000) && (interactRangeActor != NULL) &&
                           (interactRangeActor->id == ACTOR_EN_ZOG))
                            ? DO_ACTION_GRAB
                            : DO_ACTION_DIVE;
        } else {
            sp24 = Player_IsZTargeting(this);
            if ((sp24 && (this->transformation != PLAYER_FORM_DEKU)) || !(this->stateFlags1 & PLAYER_STATE1_400000) ||
                !Player_IsGoronOrDeku(this)) {
                if ((this->transformation != PLAYER_FORM_GORON) &&
                    !(this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_4000)) &&
                    (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) &&
                    (Player_CheckHostileLockOn(this) ||
                     ((sPlayerFloorType != FLOOR_TYPE_7) && (Player_FriendlyLockOnOrParallel(this) ||
                                                             ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) &&
                                                              !(this->stateFlags1 & PLAYER_STATE1_400000) &&
                                                              (controlStickDirection == PLAYER_STICK_DIR_FORWARD)))))) {
                    doActionA = DO_ACTION_ATTACK;
                } else if ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) && sp24 &&
                           (controlStickDirection >= PLAYER_STICK_DIR_LEFT)) {
                    doActionA = DO_ACTION_JUMP;
                } else if ((this->transformation == PLAYER_FORM_DEKU) && !(this->stateFlags1 & PLAYER_STATE1_8000000) &&
                           (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                    doActionA = DO_ACTION_ATTACK;
                } else if (GameInteractor_Should(VB_SHOULD_PUTAWAY, ((this->transformation == PLAYER_FORM_HUMAN) ||
                                                                     (this->transformation == PLAYER_FORM_ZORA))) &&
                           ((this->heldItemAction >= PLAYER_IA_SWORD_KOKIRI) ||
                            ((this->stateFlags2 & PLAYER_STATE2_100000) &&
                             (play->actorCtx.attention.tatlHoverActor == NULL)))) {
                    doActionA = DO_ACTION_PUTAWAY;

                    if (play->msgCtx.currentTextId == 0) {} //! FAKE
                }
            }
        }

        if (doActionA != DO_ACTION_PUTAWAY) {
            if (GameInteractor_Should(VB_RESET_PUTAWAY_TIMER, true)) {
                this->putAwayCooldownTimer = 20;
            }
        } else if (this->putAwayCooldownTimer != 0) {
            doActionA = DO_ACTION_NONE;
            this->putAwayCooldownTimer--;
        }

        Interface_SetAButtonDoAction(play, doActionA);

        // Set Tatl state
        if (!Play_InCsMode(play) && (this->stateFlags2 & PLAYER_STATE2_200000) &&
            !(this->stateFlags3 & PLAYER_STATE3_100)) {
            if (this->focusActor != NULL) {
                Interface_SetTatlCall(play, TATL_STATE_2B);
            } else {
                Interface_SetTatlCall(play, TATL_STATE_2A);
            }
            CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
        } else {
            Interface_SetTatlCall(play, TATL_STATE_2C);
        }
    }
}

s32 func_808430E0(Player* this) {
    if ((this->transformation == PLAYER_FORM_DEKU) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        func_8083784C(this)) {
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        return false;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_8000000)) {
        sPlayerFloorType = FLOOR_TYPE_0;
    }
    this->floorPitch = 0;
    this->floorPitchAlt = 0;
    sFloorPitchShape = 0;
    return true;
}

/**
 * Performs the following tasks related to scene collision:
 *
 * This includes:
 * - Update BgCheckInfo, parameters adjusted due to various state flags
 * - Update floor type, floor property and floor sfx offset
 * - Update conveyor, reverb and light settings according to the current floor poly
 * - Handle exits and voids
 * - Update information relating to the "interact wall"
 * - Update information for ledge climbing
 * - Calculate floor poly angles
 */
void Player_ProcessSceneCollision(PlayState* play, Player* this) {
    u8 nextLedgeClimbType = PLAYER_LEDGE_CLIMB_NONE;
    CollisionPoly* floorPoly;
    f32 wallCheckRadius;
    f32 speedScale;
    f32 ceilingCheckHeight;
    u32 updBgCheckInfoFlags;
    s32 spAC = (Player_Action_35 == this->actionFunc) && (this->unk_397 == 4);

    sPrevFloorProperty = this->floorProperty;

    wallCheckRadius = this->ageProperties->wallCheckRadius;
    ceilingCheckHeight = this->ageProperties->ceilingCheckHeight;

    if (this->stateFlags1 & (PLAYER_STATE1_20000000 | PLAYER_STATE1_80000000)) {
        if ((!(this->stateFlags1 & PLAYER_STATE1_DEAD) && !(this->stateFlags2 & PLAYER_STATE2_4000) &&
             (this->stateFlags1 & PLAYER_STATE1_80000000)) ||
            spAC) {
            updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10 | UPDBGCHECKINFO_FLAG_20;
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        } else if ((this->stateFlags1 & PLAYER_STATE1_1) && (play->roomCtx.curRoom.type != ROOM_TYPE_DUNGEON) &&
                   ((this->unk_D68 - (s32)this->actor.world.pos.y) >= 100)) {
            updBgCheckInfoFlags =
                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10 | UPDBGCHECKINFO_FLAG_20;
        } else if (!(this->stateFlags1 & PLAYER_STATE1_1) &&
                   ((Player_Action_36 == this->actionFunc) || (Player_Action_35 == this->actionFunc))) {
            updBgCheckInfoFlags =
                UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10 | UPDBGCHECKINFO_FLAG_20;
            this->actor.bgCheckFlags &= ~(BGCHECKFLAG_WALL | BGCHECKFLAG_PLAYER_WALL_INTERACT);
        } else {
            updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                  UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10 | UPDBGCHECKINFO_FLAG_20;
        }
    } else {
        if (Player_Action_93 == this->actionFunc) {
            updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_10 | UPDBGCHECKINFO_FLAG_800;
        } else if ((this->stateFlags3 & (PLAYER_STATE3_1000 | PLAYER_STATE3_80000)) && (this->speedXZ >= 8.0f)) {
            updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                  UPDBGCHECKINFO_FLAG_10 | UPDBGCHECKINFO_FLAG_20 | UPDBGCHECKINFO_FLAG_100 |
                                  UPDBGCHECKINFO_FLAG_200;
        } else {
            updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                  UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10 | UPDBGCHECKINFO_FLAG_20;
        }
    }

    if (this->stateFlags3 & PLAYER_STATE3_1) {
        updBgCheckInfoFlags &= ~(UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
    }

    if (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_4) {
        this->stateFlags3 |= PLAYER_STATE3_10;
    }

    if (func_801242B4(this)) {
        updBgCheckInfoFlags &= ~(UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 268 * 0.1f, wallCheckRadius, ceilingCheckHeight, updBgCheckInfoFlags);

    this->unk_AC0 -= (this->actor.world.pos.y - this->actor.prevPos.y) / this->actor.scale.y;
    this->unk_AC0 = CLAMP(this->unk_AC0, -1000.0f, 1000.0f);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) {
        this->actor.velocity.y = 0.0f;
    }

    sPlayerYDistToFloor = this->actor.world.pos.y - this->actor.floorHeight;
    sPlayerConveyorSpeedIndex = CONVEYOR_SPEED_DISABLED;
    floorPoly = this->actor.floorPoly;

    if ((floorPoly != NULL) && (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_4)) {
        this->floorProperty = SurfaceType_GetFloorProperty(&play->colCtx, floorPoly, this->actor.floorBgId);

        if (this == GET_PLAYER(play)) {
            Audio_SetCodeReverb(SurfaceType_GetEcho(&play->colCtx, floorPoly, this->actor.floorBgId));

            if (this->actor.floorBgId == BGCHECK_SCENE) {
                Environment_ChangeLightSetting(
                    play, SurfaceType_GetLightSettingIndex(&play->colCtx, floorPoly, this->actor.floorBgId));
            } else {
                DynaPoly_SetPlayerAbove(&play->colCtx, this->actor.floorBgId);
            }
        }

        sPlayerConveyorSpeedIndex = SurfaceType_GetConveyorSpeed(&play->colCtx, floorPoly, this->actor.floorBgId);

        if (sPlayerConveyorSpeedIndex != CONVEYOR_SPEED_DISABLED) {
            sPlayerIsOnFloorConveyor = SurfaceType_IsFloorConveyor(&play->colCtx, floorPoly, this->actor.floorBgId);

            if ((!sPlayerIsOnFloorConveyor && (this->actor.depthInWater > 20.0f)) ||
                (sPlayerIsOnFloorConveyor && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
                sPlayerConveyorYaw = CONVEYOR_DIRECTION_TO_BINANG(
                    SurfaceType_GetConveyorDirection(&play->colCtx, floorPoly, this->actor.floorBgId));
            } else {
                sPlayerConveyorSpeedIndex = CONVEYOR_SPEED_DISABLED;
            }
        }
    }

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_PLAYER_WALL_INTERACT;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        static Vec3f sInteractWallCheckOffset = { 0.0f, 0.0f, 0.0f };
        CollisionPoly* wallPoly;
        s32 wallBgId;
        s16 yawDiff;
        s32 pad;

        sInteractWallCheckOffset.y = 178.0f * 0.1f;
        sInteractWallCheckOffset.z = this->ageProperties->wallCheckRadius + 10.0f;

        if (Player_PosVsWallLineTest(play, this, &sInteractWallCheckOffset, &wallPoly, &wallBgId,
                                     &sInteractWallCheckResult)) {
            this->actor.bgCheckFlags |= BGCHECKFLAG_PLAYER_WALL_INTERACT;

            if (this->actor.wallPoly != wallPoly) {
                this->actor.wallPoly = wallPoly;
                this->actor.wallBgId = wallBgId;
                this->actor.wallYaw = Math_Atan2S_XY(wallPoly->normal.z, wallPoly->normal.x);
            }
        }

        yawDiff = this->actor.shape.rot.y - BINANG_ADD(this->actor.wallYaw, 0x8000);
        sPlayerTouchedWallFlags = SurfaceType_GetWallFlags(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId);
        sShapeYawToTouchedWall = ABS_ALT(yawDiff);

        yawDiff = BINANG_SUB(this->yaw, BINANG_ADD(this->actor.wallYaw, 0x8000));
        sWorldYawToTouchedWall = ABS_ALT(yawDiff);

        speedScale = sWorldYawToTouchedWall * 0.00008f;

        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (speedScale >= 1.0f)) {
            this->unk_B50 = R_RUN_SPEED_LIMIT / 100.0f;
        } else {
            this->unk_B50 = ceilingCheckHeight = (R_RUN_SPEED_LIMIT / 100.0f) * speedScale;
            if (this->unk_B50 < 0.1f) {
                this->unk_B50 = 0.1f;
            }
        }

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) && (sShapeYawToTouchedWall < 0x3000)) {
            CollisionPoly* wallPoly = this->actor.wallPoly;

            if (ABS_ALT(wallPoly->normal.y) < 600) {
                f32 wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 wallPolyNormalY = COLPOLY_GET_NORMAL(wallPoly->normal.y);
                f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 ledgeCheckOffsetXZ;
                CollisionPoly* ledgeFloorPoly;
                CollisionPoly* poly;
                s32 bgId;
                Vec3f ledgeCheckPos;
                f32 ledgePosY;
                f32 ceillingPosY;
                s32 wallYawDiff;

                this->distToInteractWall = Math3D_UDistPlaneToPos(wallPolyNormalX, wallPolyNormalY, wallPolyNormalZ,
                                                                  wallPoly->dist, &this->actor.world.pos);

                ledgeCheckOffsetXZ = this->distToInteractWall + 10.0f;

                ledgeCheckPos.x = this->actor.world.pos.x - (ledgeCheckOffsetXZ * wallPolyNormalX);
                ledgeCheckPos.z = this->actor.world.pos.z - (ledgeCheckOffsetXZ * wallPolyNormalZ);
                ledgeCheckPos.y = this->actor.world.pos.y + this->ageProperties->unk_0C;

                ledgePosY =
                    BgCheck_EntityRaycastFloor5(&play->colCtx, &ledgeFloorPoly, &bgId, &this->actor, &ledgeCheckPos);
                this->yDistToLedge = ledgePosY - this->actor.world.pos.y;

                if ((this->yDistToLedge < 178.0f * 0.1f) ||
                    BgCheck_EntityCheckCeiling(&play->colCtx, &ceillingPosY, &this->actor.world.pos,
                                               (ledgePosY - this->actor.world.pos.y) + 20.0f, &poly, &bgId,
                                               &this->actor)) {
                    this->yDistToLedge = LEDGE_DIST_MAX;
                } else {
                    sInteractWallCheckOffset.y = (ledgePosY + 5.0f) - this->actor.world.pos.y;

                    if (Player_PosVsWallLineTest(play, this, &sInteractWallCheckOffset, &poly, &bgId,
                                                 &sInteractWallCheckResult) &&
                        (wallYawDiff = (s32)(this->actor.wallYaw - Math_Atan2S_XY(poly->normal.z, poly->normal.x)),
                         ABS_ALT(wallYawDiff) < 0x4000) &&
                        !SurfaceType_CheckWallFlag1(&play->colCtx, poly, bgId)) {
                        this->yDistToLedge = LEDGE_DIST_MAX;
                    } else if (!SurfaceType_CheckWallFlag0(&play->colCtx, wallPoly, this->actor.wallBgId)) {
                        if (this->ageProperties->unk_1C <= this->yDistToLedge) {
                            if (ABS_ALT(ledgeFloorPoly->normal.y) > 0x5DC0) {
                                if ((this->ageProperties->unk_14 <= this->yDistToLedge) || func_801242B4(this)) {
                                    nextLedgeClimbType = PLAYER_LEDGE_CLIMB_4;
                                } else if (this->ageProperties->unk_18 <= this->yDistToLedge) {
                                    nextLedgeClimbType = PLAYER_LEDGE_CLIMB_3;
                                } else {
                                    nextLedgeClimbType = PLAYER_LEDGE_CLIMB_2;
                                }
                            }
                        } else {
                            nextLedgeClimbType = PLAYER_LEDGE_CLIMB_1;
                        }
                    }
                }
            }
        }
    } else {
        this->unk_B50 = R_RUN_SPEED_LIMIT / 100.0f;
        this->yDistToLedge = 0.0f;
        this->ledgeClimbDelayTimer = 0;
    }

    if (nextLedgeClimbType == this->ledgeClimbType) {
        if (this->speedXZ != 0.0f) {
            if (this->ledgeClimbDelayTimer < 100) {
                this->ledgeClimbDelayTimer++;
            }
        }
    } else {
        this->ledgeClimbType = nextLedgeClimbType;
        this->ledgeClimbDelayTimer = 0;
    }

    sPlayerFloorType = SurfaceType_GetFloorType(&play->colCtx, floorPoly, this->actor.floorBgId);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        f32 floorPolyNormalX;
        f32 floorPolyNormalY;
        f32 floorPolyNormalZ;
        f32 sin;
        s32 pad;
        f32 cos;

        sPlayerFloorEffect = SurfaceType_GetFloorEffect(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

        if (!func_808430E0(this)) {
            floorPolyNormalY = COLPOLY_GET_NORMAL(floorPoly->normal.y);

            if (this->actor.floorBgId != BGCHECK_SCENE) {
                DynaPoly_SetPlayerOnTop(&play->colCtx, this->actor.floorBgId);
            } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) && (this->actor.depthInWater <= 24.0f) &&
                       (sPlayerFloorEffect != FLOOR_EFFECT_1) &&
                       (sPlayerConveyorSpeedIndex == CONVEYOR_SPEED_DISABLED) && (floorPolyNormalY > 0.5f)) {
                if (CutsceneManager_GetCurrentCsId() != play->playerCsIds[PLAYER_CS_ID_SONG_WARP]) {
                    func_80841A50(play, this);
                }
            }

            floorPolyNormalX = COLPOLY_GET_NORMAL(floorPoly->normal.x);
            floorPolyNormalY = 1.0f / floorPolyNormalY;
            floorPolyNormalZ = COLPOLY_GET_NORMAL(floorPoly->normal.z);

            sin = Math_SinS(this->yaw);
            cos = Math_CosS(this->yaw);

            this->floorPitch =
                Math_Atan2S_XY(1.0f, (-(floorPolyNormalX * sin) - (floorPolyNormalZ * cos)) * floorPolyNormalY);
            this->floorPitchAlt =
                Math_Atan2S_XY(1.0f, (-(floorPolyNormalX * cos) - (floorPolyNormalZ * sin)) * floorPolyNormalY);

            sin = Math_SinS(this->actor.shape.rot.y);
            cos = Math_CosS(this->actor.shape.rot.y);

            sFloorPitchShape =
                Math_Atan2S_XY(1.0f, (-(floorPolyNormalX * sin) - (floorPolyNormalZ * cos)) * floorPolyNormalY);

            Player_HandleSlopes(play, this);
        }
    } else {
        func_808430E0(this);
        sPlayerFloorEffect = FLOOR_EFFECT_0;
    }

    if (floorPoly != NULL) {
        this->prevFloorSfxOffset = this->floorSfxOffset;

        if (spAC) {
            this->floorSfxOffset = NA_SE_PL_WALK_CONCRETE - SFX_FLAG;
            return;
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
            if (this->actor.depthInWater < 50.0f) {
                if (this->actor.depthInWater < 20.0f) {
                    this->floorSfxOffset = (sPlayerFloorType == FLOOR_TYPE_13) ? NA_SE_PL_WALK_DIRT - SFX_FLAG
                                                                               : NA_SE_PL_WALK_WATER0 - SFX_FLAG;
                } else {
                    this->floorSfxOffset = (sPlayerFloorType == FLOOR_TYPE_13) ? NA_CODE_DIRT_DEEP - SFX_FLAG
                                                                               : NA_SE_PL_WALK_WATER1 - SFX_FLAG;
                }

                return;
            }
        }

        if (this->stateFlags2 & PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND) {
            this->floorSfxOffset = NA_SE_PL_WALK_SAND - SFX_FLAG;
        } else if (COLPOLY_GET_NORMAL(floorPoly->normal.y) > 0.5f) {
            this->floorSfxOffset = SurfaceType_GetSfxOffset(&play->colCtx, floorPoly, this->actor.floorBgId);
        }
    }
}

void Player_UpdateCamAndSeqModes(PlayState* play, Player* this) {
    u8 seqMode;
    s32 pad[2];
    Camera* camera;
    s32 camMode;

    if (this == GET_PLAYER(play)) {
        seqMode = SEQ_MODE_DEFAULT;
        if (this->stateFlags1 & PLAYER_STATE1_100000) {
            seqMode = SEQ_MODE_STILL;
        } else if (this->csAction != PLAYER_CSACTION_NONE) {
            Camera_ChangeMode(Play_GetCamera(play, CAM_ID_MAIN), CAM_MODE_NORMAL);
        } else {
            camera = (this->actor.id == ACTOR_PLAYER) ? Play_GetCamera(play, CAM_ID_MAIN)
                                                      : Play_GetCamera(play, ((EnTest3*)this)->subCamId);
            if ((this->actor.parent != NULL) && (this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT)) {
                camMode = CAM_MODE_HOOKSHOT;
                Camera_SetViewParam(camera, CAM_VIEW_TARGET, this->actor.parent);
            } else if (Player_Action_21 == this->actionFunc) {
                camMode = CAM_MODE_STILL;
            } else if (this->stateFlags3 & PLAYER_STATE3_8000) {
                if (this->stateFlags1 & PLAYER_STATE1_8000000) {
                    camMode = CAM_MODE_GORONDASH;
                } else {
                    camMode = CAM_MODE_FREEFALL;
                }
            } else if (this->stateFlags3 & PLAYER_STATE3_80000) {
                if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                    camMode = CAM_MODE_GORONDASH;
                } else {
                    camMode = CAM_MODE_GORONJUMP;
                }
            } else if (this->stateFlags2 & PLAYER_STATE2_100) {
                camMode = CAM_MODE_PUSHPULL;
            } else if (this->focusActor != NULL) {
                if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK)) {
                    camMode = CAM_MODE_TALK;
                } else if (this->stateFlags1 & PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS) {
                    if (this->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN) {
                        camMode = CAM_MODE_FOLLOWBOOMERANG;
                    } else {
                        camMode = CAM_MODE_FOLLOWTARGET;
                    }
                } else {
                    camMode = CAM_MODE_BATTLE;
                }
                Camera_SetViewParam(camera, CAM_VIEW_TARGET, this->focusActor);
            } else if (this->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) {
                camMode = CAM_MODE_CHARGE;
            } else if (this->stateFlags3 & PLAYER_STATE3_100) {
                camMode = CAM_MODE_DEKUHIDE;
            } else if (this->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN) {
                camMode = CAM_MODE_FOLLOWBOOMERANG;
                Camera_SetViewParam(camera, CAM_VIEW_TARGET, this->zoraBoomerangActor);
            } else if (this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000)) {
                if (Player_FriendlyLockOnOrParallel(this)) {
                    camMode = CAM_MODE_HANGZ;
                } else {
                    camMode = CAM_MODE_HANG;
                }
            } else if ((this->stateFlags3 & PLAYER_STATE3_2000) && (this->actor.velocity.y < 0.0f)) {
                if (this->stateFlags1 & (PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE)) {
                    camMode = CAM_MODE_DEKUFLYZ;
                } else {
                    camMode = CAM_MODE_DEKUFLY;
                }
            } else if (this->stateFlags1 & (PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE)) {
                if (func_800B7128(this) || func_8082EF20(this)) {
                    camMode = CAM_MODE_BOWARROWZ;
                } else if (this->stateFlags1 & PLAYER_STATE1_200000) {
                    camMode = CAM_MODE_CLIMBZ;
                } else {
                    camMode = CAM_MODE_TARGET;
                }
            } else if ((this->stateFlags1 & PLAYER_STATE1_400000) && (this->transformation != 0)) {
                camMode = CAM_MODE_STILL;
            } else if (this->stateFlags1 & PLAYER_STATE1_40000) {
                camMode = CAM_MODE_JUMP;
            } else if (this->stateFlags1 & PLAYER_STATE1_200000) {
                camMode = CAM_MODE_CLIMB;
            } else if (this->stateFlags1 & PLAYER_STATE1_80000) {
                camMode = CAM_MODE_FREEFALL;
            } else if (((Player_Action_84 == this->actionFunc) &&
                        (this->meleeWeaponAnimation >= PLAYER_MWA_FORWARD_SLASH_1H) &&
                        (this->meleeWeaponAnimation <= PLAYER_MWA_ZORA_PUNCH_KICK)) ||
                       (this->stateFlags3 & PLAYER_STATE3_8) ||
                       ((Player_Action_52 == this->actionFunc) && (this->av2.actionVar2 == 0)) ||
                       (Player_Action_53 == this->actionFunc)) {
                camMode = CAM_MODE_STILL;
            } else {
                camMode = CAM_MODE_NORMAL;
                if ((this->speedXZ == 0.0f) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_800000) || (this->rideActor->speed == 0.0f))) {
                    seqMode = SEQ_MODE_STILL;
                }
            }

            Camera_ChangeMode(camera, camMode);
        }

        if (play->actorCtx.attention.bgmEnemy != NULL) {
            if (GameInteractor_Should(VB_PLAY_ENEMY_PROXIMITY_MUSIC, true)) {
                seqMode = SEQ_MODE_ENEMY;
                Audio_UpdateEnemyBgmVolume(sqrtf(play->actorCtx.attention.bgmEnemy->xyzDistToPlayerSq));
            }
        }

        Audio_SetSequenceMode(seqMode);
    }
}

Vec3f D_8085D364 = { 0.0f, 0.5f, 0.0f };
Vec3f D_8085D370 = { 0.0f, 0.5f, 0.0f };
Color_RGBA8 D_8085D37C = { 255, 255, 100, 255 };
Color_RGBA8 D_8085D380 = { 255, 50, 0, 0 };

void func_808442D8(PlayState* play, Player* this) {
    f32 var_fa0;
    f32 temp_fv1;

    if (this->unk_B0C == 0.0f) {
        Player_UseItem(play, this, ITEM_NONE);
        return;
    }

    var_fa0 = 1.0f;
    if (DECR(this->unk_B28) == 0) {
        Inventory_ChangeAmmo(ITEM_DEKU_STICK, -1);
        this->unk_B28 = 1;
        this->unk_B0C = 0.0f;
        var_fa0 = 0.0f;
    } else if (this->unk_B28 >= 0xC9) {
        var_fa0 = (0xD2 - this->unk_B28) / 10.0f;
    } else if (this->unk_B28 < 0x14) {
        var_fa0 = this->unk_B28 / 20.0f;
        this->unk_B0C = var_fa0;
    }

    if (var_fa0 > 0.0f) {
        func_800B0EB0(play, &this->meleeWeaponInfo[0].tip, &D_8085D364, &D_8085D370, &D_8085D37C, &D_8085D380,
                      (var_fa0 * 200.0f), 0, 8);
        if (play->roomCtx.curRoom.enablePosLights || (MREG(93) != 0)) {
            temp_fv1 = (Rand_ZeroOne() * 30.0f) + 225.0f;
            Lights_PointSetColorAndRadius(&this->lightInfo, temp_fv1, temp_fv1 * 0.7f, 0, var_fa0 * 300.0f);
        }
    }
}

void Player_UpdateBodyShock(PlayState* play, Player* this) {
    this->bodyShockTimer--;
    this->unk_B66 += this->bodyShockTimer;
    if (this->unk_B66 > 20) {
        Vec3f pos;
        Vec3f* bodyPartsPos;
        s32 scale;
        s32 randIndex;

        this->unk_B66 -= 20;
        scale = this->bodyShockTimer * 2;
        if (scale > 40) {
            scale = 40;
        }

        randIndex = Rand_ZeroFloat(PLAYER_BODYPART_MAX - 0.1f);
        bodyPartsPos = randIndex + this->bodyPartsPos;

        pos.x = (Rand_CenteredFloat(5.0f) + bodyPartsPos->x) - this->actor.world.pos.x;
        pos.y = (Rand_CenteredFloat(5.0f) + bodyPartsPos->y) - this->actor.world.pos.y;
        pos.z = (Rand_CenteredFloat(5.0f) + bodyPartsPos->z) - this->actor.world.pos.z;
        EffectSsFhgFlash_SpawnShock(play, &this->actor, &pos, scale, FHGFLASH_SHOCK_PLAYER);
        Actor_PlaySfx_Flagged2(&this->actor, NA_SE_PL_SPARK - SFX_FLAG);
    }
}

/**
 * Rumbles the controller when close to a secret.
 */
void Player_DetectSecrets(PlayState* play, Player* this) {
    f32 step = (SQ(200.0f) * 5.0f) - (this->closestSecretDistSq * 5.0f);

    if (step < 0.0f) {
        step = 0.0f;
    }

    this->secretRumbleCharge += step;
    if (this->secretRumbleCharge > SQ(2000.0f)) {
        this->secretRumbleCharge = 0.0f;
        Player_RequestRumble(play, this, 120, 20, 10, SQ(0));
    }
}

// Making a player csAction negative will behave as its positive counterpart
// except will disable setting the start position
s8 sPlayerCueToCsActionMap[PLAYER_CUEID_MAX] = {
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_NONE
    PLAYER_CSACTION_2,    // PLAYER_CUEID_1
    PLAYER_CSACTION_2,    // PLAYER_CUEID_2
    PLAYER_CSACTION_4,    // PLAYER_CUEID_3
    PLAYER_CSACTION_3,    // PLAYER_CUEID_4
    PLAYER_CSACTION_56,   // PLAYER_CUEID_5
    PLAYER_CSACTION_8,    // PLAYER_CUEID_6
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_7
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_8
    PLAYER_CSACTION_135,  // PLAYER_CUEID_9
    PLAYER_CSACTION_21,   // PLAYER_CUEID_10
    PLAYER_CSACTION_61,   // PLAYER_CUEID_11
    PLAYER_CSACTION_62,   // PLAYER_CUEID_12
    PLAYER_CSACTION_60,   // PLAYER_CUEID_13
    PLAYER_CSACTION_63,   // PLAYER_CUEID_14
    PLAYER_CSACTION_64,   // PLAYER_CUEID_15
    PLAYER_CSACTION_65,   // PLAYER_CUEID_16
    PLAYER_CSACTION_66,   // PLAYER_CUEID_17
    PLAYER_CSACTION_70,   // PLAYER_CUEID_18
    PLAYER_CSACTION_19,   // PLAYER_CUEID_19
    PLAYER_CSACTION_71,   // PLAYER_CUEID_20
    PLAYER_CSACTION_72,   // PLAYER_CUEID_21
    PLAYER_CSACTION_67,   // PLAYER_CUEID_22
    PLAYER_CSACTION_73,   // PLAYER_CUEID_23
    PLAYER_CSACTION_74,   // PLAYER_CUEID_24
    PLAYER_CSACTION_75,   // PLAYER_CUEID_25
    PLAYER_CSACTION_68,   // PLAYER_CUEID_26
    PLAYER_CSACTION_69,   // PLAYER_CUEID_27
    PLAYER_CSACTION_76,   // PLAYER_CUEID_28
    PLAYER_CSACTION_116,  // PLAYER_CUEID_29
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_30
    PLAYER_CSACTION_40,   // PLAYER_CUEID_31
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_32
    -PLAYER_CSACTION_52,  // PLAYER_CUEID_33
    PLAYER_CSACTION_42,   // PLAYER_CUEID_34
    PLAYER_CSACTION_43,   // PLAYER_CUEID_35
    PLAYER_CSACTION_57,   // PLAYER_CUEID_36
    PLAYER_CSACTION_81,   // PLAYER_CUEID_37
    PLAYER_CSACTION_41,   // PLAYER_CUEID_38
    PLAYER_CSACTION_53,   // PLAYER_CUEID_39
    PLAYER_CSACTION_54,   // PLAYER_CUEID_40
    PLAYER_CSACTION_44,   // PLAYER_CUEID_41
    PLAYER_CSACTION_55,   // PLAYER_CUEID_42
    PLAYER_CSACTION_45,   // PLAYER_CUEID_43
    PLAYER_CSACTION_46,   // PLAYER_CUEID_44
    PLAYER_CSACTION_47,   // PLAYER_CUEID_45
    PLAYER_CSACTION_48,   // PLAYER_CUEID_46
    PLAYER_CSACTION_49,   // PLAYER_CUEID_47
    PLAYER_CSACTION_50,   // PLAYER_CUEID_48
    PLAYER_CSACTION_51,   // PLAYER_CUEID_49
    PLAYER_CSACTION_77,   // PLAYER_CUEID_50
    PLAYER_CSACTION_78,   // PLAYER_CUEID_51
    PLAYER_CSACTION_79,   // PLAYER_CUEID_52
    PLAYER_CSACTION_80,   // PLAYER_CUEID_53
    PLAYER_CSACTION_81,   // PLAYER_CUEID_54
    PLAYER_CSACTION_82,   // PLAYER_CUEID_55
    PLAYER_CSACTION_83,   // PLAYER_CUEID_56
    PLAYER_CSACTION_84,   // PLAYER_CUEID_57
    PLAYER_CSACTION_85,   // PLAYER_CUEID_58
    PLAYER_CSACTION_86,   // PLAYER_CUEID_59
    PLAYER_CSACTION_87,   // PLAYER_CUEID_60
    PLAYER_CSACTION_88,   // PLAYER_CUEID_61
    PLAYER_CSACTION_89,   // PLAYER_CUEID_62
    PLAYER_CSACTION_90,   // PLAYER_CUEID_63
    PLAYER_CSACTION_91,   // PLAYER_CUEID_64
    PLAYER_CSACTION_92,   // PLAYER_CUEID_65
    PLAYER_CSACTION_94,   // PLAYER_CUEID_66
    PLAYER_CSACTION_95,   // PLAYER_CUEID_67
    PLAYER_CSACTION_100,  // PLAYER_CUEID_68
    PLAYER_CSACTION_101,  // PLAYER_CUEID_69
    PLAYER_CSACTION_98,   // PLAYER_CUEID_70
    PLAYER_CSACTION_99,   // PLAYER_CUEID_71
    PLAYER_CSACTION_102,  // PLAYER_CUEID_72
    PLAYER_CSACTION_103,  // PLAYER_CUEID_73
    PLAYER_CSACTION_104,  // PLAYER_CUEID_74
    PLAYER_CSACTION_112,  // PLAYER_CUEID_75
    PLAYER_CSACTION_113,  // PLAYER_CUEID_76
    PLAYER_CSACTION_117,  // PLAYER_CUEID_77
    PLAYER_CSACTION_104,  // PLAYER_CUEID_78
    PLAYER_CSACTION_104,  // PLAYER_CUEID_79
    PLAYER_CSACTION_105,  // PLAYER_CUEID_80
    PLAYER_CSACTION_106,  // PLAYER_CUEID_81
    PLAYER_CSACTION_107,  // PLAYER_CUEID_82
    PLAYER_CSACTION_108,  // PLAYER_CUEID_83
    PLAYER_CSACTION_109,  // PLAYER_CUEID_84
    PLAYER_CSACTION_110,  // PLAYER_CUEID_85
    PLAYER_CSACTION_118,  // PLAYER_CUEID_86
    PLAYER_CSACTION_119,  // PLAYER_CUEID_87
    PLAYER_CSACTION_120,  // PLAYER_CUEID_88
    PLAYER_CSACTION_114,  // PLAYER_CUEID_89
    PLAYER_CSACTION_111,  // PLAYER_CUEID_90
    PLAYER_CSACTION_122,  // PLAYER_CUEID_91
};

f32 D_8085D3E0[PLAYER_FORM_MAX] = {
    0.8f, // PLAYER_FORM_FIERCE_DEITY
    0.6f, // PLAYER_FORM_GORON
    0.8f, // PLAYER_FORM_ZORA
    1.5f, // PLAYER_FORM_DEKU
    1.0f, // PLAYER_FORM_HUMAN
};

void func_80844784(PlayState* play, Player* this) {
    f32 var_fv0;
    s16 var_a3;
    f32 temp_ft4;
    s32 temp_ft2;
    f32 temp_fv1_2;
    f32 sp58;
    f32 sp54;
    f32 sp50;
    f32 sp4C;
    f32 sp48;
    f32 sp44;
    f32 temp_fa0;
    f32 temp_fa1;
    s16 temp_v0;
    f32 temp_fv0_2;

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (sPlayerFloorType == FLOOR_TYPE_5) &&
        (this->currentBoots < PLAYER_BOOTS_ZORA_UNDERWATER)) {
        var_a3 = this->yaw;
        var_fv0 = this->speedXZ;
        temp_v0 = this->actor.world.rot.y - var_a3;

        if ((ABS_ALT(temp_v0) > 0x6000) && (this->actor.speed != 0.0f)) {
            var_fv0 = 0.0f;
            var_a3 += 0x8000;
        }

        if (Math_StepToF(&this->actor.speed, var_fv0, 0.35f) && (var_fv0 == 0.0f)) {
            this->actor.world.rot.y = this->yaw;
        }

        if (this->speedXZ != 0.0f) {
            temp_ft2 = (fabsf(this->speedXZ) * 700.0f) - (fabsf(this->actor.speed) * 100.0f);
            temp_ft2 = CLAMP(temp_ft2, 0, 0x546);

            Math_ScaledStepToS(&this->actor.world.rot.y, var_a3, temp_ft2);
        }
        if ((this->speedXZ == 0.0f) && (this->actor.speed != 0.0f)) {
            Audio_PlaySfx_AtPosWithSyncedFreqAndVolume(
                &this->actor.projectedPos, Player_GetFloorSfx(this, NA_SE_PL_SLIP_LEVEL - SFX_FLAG), this->actor.speed);
        }
    } else {
        this->actor.speed = this->speedXZ;
        this->actor.world.rot.y = this->yaw;
    }

    Actor_UpdateVelocityWithGravity(&this->actor);
    D_80862B3C = 0.0f;
    if ((gSaveContext.save.saveInfo.playerData.health != 0) &&
        ((this->pushedSpeed != 0.0f) || (this->windSpeed != 0.0f) || (play->envCtx.windSpeed >= 50.0f)) &&
        (!Player_InCsMode(play)) &&
        !(this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 | PLAYER_STATE1_200000)) &&
        !(this->stateFlags3 & PLAYER_STATE3_100) && (Player_Action_33 != this->actionFunc) &&
        (this->actor.id == ACTOR_PLAYER)) {
        this->actor.velocity.x += this->pushedSpeed * Math_SinS(this->pushedYaw);
        this->actor.velocity.z += this->pushedSpeed * Math_CosS(this->pushedYaw);
        temp_fv1_2 = 10.0f - this->actor.velocity.y;
        if (temp_fv1_2 > 0.0f) {
            sp58 = D_8085D3E0[this->transformation];
            sp54 = this->windSpeed * sp58;
            sp50 = Math_SinS(this->windAngleX) * sp54;
            sp4C = Math_CosS(this->windAngleX) * sp54;
            sp48 = Math_SinS(this->windAngleY) * sp4C;
            sp44 = Math_CosS(this->windAngleY) * sp4C;

            if ((sp50 > 0.0f) && (this->transformation == PLAYER_FORM_DEKU) &&
                !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                if (Player_SetAction(play, this, Player_Action_94, 1)) {
                    this->stateFlags3 |= PLAYER_STATE3_2000 | PLAYER_STATE3_1000000;
                    func_8082E1F0(this, NA_SE_IT_DEKUNUTS_FLOWER_OPEN);
                    Audio_SetSfxTimerLerpInterval(4, 2);
                }

                this->av2.actionVar2 = 0x270F;
                Math_Vec3f_Copy(this->unk_AF0, &this->actor.world.pos);
            }

            if (play->envCtx.windSpeed >= 50.0f) {
                temp_fa0 = play->envCtx.windDirection.x;
                temp_fa1 = play->envCtx.windDirection.y;
                temp_ft4 = play->envCtx.windDirection.z;

                temp_fv0_2 = sqrtf(SQ(temp_fa0) + SQ(temp_fa1) + SQ(temp_ft4));
                if (temp_fv0_2 != 0.0f) {
                    temp_fv0_2 = ((play->envCtx.windSpeed - 50.0f) * 0.1f * sp58) / temp_fv0_2;

                    sp48 -= temp_fa0 * temp_fv0_2;
                    sp50 -= temp_fa1 * temp_fv0_2;
                    sp44 -= temp_ft4 * temp_fv0_2;
                }
            }

            if (temp_fv1_2 < sp50) {
                temp_fv1_2 /= sp50;

                sp48 *= temp_fv1_2;
                sp50 *= temp_fv1_2;
                sp44 *= temp_fv1_2;
            }

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                D_80862B3C = (sp44 * Math_CosS(this->yaw)) + (Math_SinS(this->yaw) * sp48);
                if (fabsf(D_80862B3C) > 4.0f) {
                    func_8083FBC4(play, this);
                }

                Audio_PlaySfx_AtPosWithSyncedFreqAndVolume(&this->actor.projectedPos,
                                                           Player_GetFloorSfx(this, NA_SE_PL_SLIP_LEVEL - SFX_FLAG),
                                                           fabsf(D_80862B3C));
            }

            this->actor.velocity.x += sp48;
            this->actor.velocity.z += sp44;
            this->actor.velocity.y += sp50;
        }
    }

    Actor_UpdatePos(&this->actor);
}

Color_RGBA8 D_8085D3F4 = { 100, 255, 255, 0 };
Color_RGBA8 D_8085D3F8 = { 0, 100, 100, 0 };

void func_80844D80(PlayState* play, Player* this) {
    Vec3f pos;
    Vec3f spA0;
    Vec3f velocity;
    Vec3f accel;
    Vec3f sp7C;
    s32 i;

    Math_Vec3f_Diff(&this->meleeWeaponInfo[0].tip, &this->meleeWeaponInfo[0].base, &sp7C);
    Math_Vec3f_SumScaled(&this->meleeWeaponInfo[0].base, &sp7C, 0.3f, &spA0);

    for (i = 0; i < 2; i++) {
        Math_Vec3f_SumScaled(&this->meleeWeaponInfo[0].base, &sp7C, Rand_ZeroOne(), &pos);
        Math_Vec3f_AddRand(&pos, 15.0f, &pos);
        Math_Vec3f_DistXYZAndStoreNormDiff(&spA0, &pos, 1.7f, &velocity);
        Math_Vec3f_ScaleAndStore(&velocity, 0.01f, &accel);
        EffectSsKirakira_SpawnDispersed(play, &pos, &velocity, &accel, &D_8085D3F4, &D_8085D3F8,
                                        Rand_S16Offset(-20, -120), 15);
    }
}

f32 D_8085D3FC[] = { 0.005f, 0.05f };

f32 sWaterConveyorSpeeds[CONVEYOR_SPEED_MAX - 1] = {
    2.0f,  // CONVEYOR_SPEED_SLOW
    4.0f,  // CONVEYOR_SPEED_MEDIUM
    11.0f, // CONVEYOR_SPEED_FAST
};
f32 sFloorConveyorSpeeds[CONVEYOR_SPEED_MAX - 1] = {
    0.5f, // CONVEYOR_SPEED_SLOW
    1.0f, // CONVEYOR_SPEED_MEDIUM
    3.0f, // CONVEYOR_SPEED_FAST
};

void Player_UpdateCommon(Player* this, PlayState* play, Input* input) {
    f32 temp_fv0;
    f32 temp_fv1;

    sPlayerControlInput = input;
    if (this->unk_D6A < 0) {
        this->unk_D6A++;
        if (this->unk_D6A == 0) {
            this->unk_D6A = 1;
            Audio_PlaySfx(NA_SE_OC_REVENGE);
        }
    }

    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.home.pos);

    temp_fv1 = fabsf(this->speedXZ) * (fabsf(Math_SinS(this->floorPitch) * 800.0f) + 100.0f);

    Math_StepToF(&this->unk_AC0, 0.0f, CLAMP_MIN(temp_fv1, 300.0f));

    if (this->unk_D57 != 0) {
        this->unk_D57--;
    }

    if (this->textboxBtnCooldownTimer != 0) {
        this->textboxBtnCooldownTimer--;
    }

    if (this->unk_D6B != 0) {
        this->unk_D6B--;
    }

    if (this->invincibilityTimer < 0) {
        this->invincibilityTimer++;
    } else if (this->invincibilityTimer > 0) {
        this->invincibilityTimer--;
    }

    if (this->unk_B64 != 0) {
        this->unk_B64--;
    }

    if (this->blastMaskTimer != 0) {
        this->blastMaskTimer--;
    }

    if (gSaveContext.jinxTimer != 0) {
        gSaveContext.jinxTimer--;
    }

    func_80122C20(play, &this->unk_3D0);
    if ((this->transformation == PLAYER_FORM_FIERCE_DEITY) && Player_IsZTargeting(this)) {
        func_80844D80(play, this);
    }
    if (this->transformation == PLAYER_FORM_ZORA) {
        s32 var_v0 = (this->stateFlags1 & PLAYER_STATE1_8000000) ? 1 : 0;

        Math_StepToF(&this->unk_B10[0], var_v0, D_8085D3FC[var_v0]);
    }

    Player_UpdateZTargeting(this, play);

    if (play->roomCtx.curRoom.enablePosLights) {
        Lights_PointSetColorAndRadius(&this->lightInfo, 255, 255, 255, 60);
    } else {
        this->lightInfo.params.point.radius = -1;
    }

    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) && (this->unk_B28 != 0)) {
        func_808442D8(play, this);
    } else if (this->heldItemAction == PLAYER_IA_FISHING_ROD) {
        if (this->unk_B28 < 0) {
            this->unk_B28++;
        }
    }

    if (this->bodyShockTimer != 0) {
        Player_UpdateBodyShock(play, this);
    }

    if (this->bodyIsBurning) {
        Player_UpdateBodyBurn(play, this);
    }

    if (this->stateFlags2 & PLAYER_STATE2_8000) {
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            Player_StopHorizontalMovement(this);
            Actor_MoveWithGravity(&this->actor);
        }
        Player_ProcessSceneCollision(play, this);
    } else {
        f32 temp_fa0;
        f32 var_fv1_2;
        s32 var_v1;
        s32 pad;

        if (this->currentBoots != this->prevBoots) {
            if (this->currentBoots == PLAYER_BOOTS_ZORA_UNDERWATER) {
                if (this->stateFlags1 & PLAYER_STATE1_8000000) {
                    func_8082DC64(play, this);
                    if (this->ageProperties->unk_2C < this->actor.depthInWater) {
                        this->stateFlags2 |= PLAYER_STATE2_400;
                    }
                }
            } else if ((this->stateFlags1 & PLAYER_STATE1_8000000) &&
                       ((this->prevBoots == PLAYER_BOOTS_ZORA_UNDERWATER) ||
                        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
                func_8083B930(play, this);
                this->stateFlags2 &= ~PLAYER_STATE2_400;
                if (Player_Action_54 == this->actionFunc) {
                    this->av2.actionVar2 = 20;
                }
            }
            this->prevBoots = this->currentBoots;
        }
        if ((this->actor.parent == NULL) && (this->stateFlags1 & PLAYER_STATE1_800000)) {
            this->actor.parent = this->rideActor;
            func_80837BD0(play, this);
            this->av2.actionVar2 = -1;
            Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_uma_wait_1);
            Player_AnimReplace_Setup(play, this,
                                     ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE |
                                         ANIM_FLAG_80);
        }

        if (this->unk_ADC == 0) {
            this->unk_ADD = 0;
        } else if (this->unk_ADC < 0) {
            this->unk_ADC++;
        } else {
            this->unk_ADC--;
        }

        if (!(this->stateFlags3 & PLAYER_STATE3_2000)) {
            Math_ScaledStepToS(&this->unk_AAA, 0, 0x190);
        }

        if ((this->transformation >= PLAYER_FORM_GORON) && (this->transformation <= PLAYER_FORM_DEKU)) {
            FaceChange_UpdateBlinkingNonHuman(&this->faceChange, 20, 80, 3);
        } else {
            FaceChange_UpdateBlinking(&this->faceChange, 20, 80, 6);
        }

        this->actor.shape.face = ((play->gameplayFrames & 0x20) ? 0 : 3) + this->faceChange.face;

        if (GameInteractor_Should(VB_CONSIDER_BUNNY_HOOD_EQUIPPED, this->currentMask == PLAYER_MASK_BUNNY, this)) {
            Player_UpdateBunnyEars(this);
        }

        if (func_800B7118(this)) {
            func_808484F0(this);
        }

        if (!play->soaringCsOrSoTCsPlaying && !(this->skelAnime.movementFlags & ANIM_FLAG_80)) {
            if (!(this->stateFlags1 & PLAYER_STATE1_2) && (this->actor.parent == NULL)) {
                func_80844784(play, this);
            }
            Player_ProcessSceneCollision(play, this);
        } else {
            sPlayerFloorType = FLOOR_TYPE_0;
            this->floorProperty = FLOOR_PROPERTY_0;
            if (this->stateFlags1 & PLAYER_STATE1_800000) {
                this->actor.floorPoly = this->rideActor->floorPoly;
                this->actor.floorBgId = this->rideActor->floorBgId;
            }
            sPlayerConveyorSpeedIndex = CONVEYOR_SPEED_DISABLED;
            this->pushedSpeed = 0.0f;
        }

        Player_HandleExitsAndVoids(play, this, this->actor.floorPoly, this->actor.floorBgId);
        if (sPlayerConveyorSpeedIndex != CONVEYOR_SPEED_DISABLED) {
            f32 conveyorSpeed;
            s32 pad2;

            sPlayerConveyorSpeedIndex--;
            if (!sPlayerIsOnFloorConveyor) {
                conveyorSpeed = sWaterConveyorSpeeds[sPlayerConveyorSpeedIndex];
                if (!(this->stateFlags1 & PLAYER_STATE1_8000000)) {
                    conveyorSpeed /= 4.0f;
                }
            } else {
                conveyorSpeed = sFloorConveyorSpeeds[sPlayerConveyorSpeedIndex];
            }

            Math_StepToF(&this->pushedSpeed, conveyorSpeed, conveyorSpeed * 0.1f);
            Math_ScaledStepToS(&this->pushedYaw, sPlayerConveyorYaw,
                               ((this->stateFlags1 & PLAYER_STATE1_8000000) ? 400.0f : 800.0f) * conveyorSpeed);
        } else if (this->pushedSpeed != 0.0f) {
            Math_StepToF(&this->pushedSpeed, 0.0f, (this->stateFlags1 & PLAYER_STATE1_8000000) ? 0.5f : 2.0f);
        }
        if (!(this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_20000000)) &&
            !(this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT) && (Player_Action_80 != this->actionFunc)) {
            func_8083BB4C(play, this);
            if (!Play_InCsMode(play)) {
                if ((this->actor.id == ACTOR_PLAYER) && !(this->stateFlags1 & PLAYER_STATE1_80000000) &&
                    (gSaveContext.save.saveInfo.playerData.health == 0) &&
                    func_808323C0(this, play->playerCsIds[PLAYER_CS_ID_DEATH])) {
                    if (this->stateFlags3 & PLAYER_STATE3_1000000) {
                        func_808355D8(play, this, &gPlayerAnim_pn_kakkufinish);
                    } else if (this->stateFlags1 &
                               (PLAYER_STATE1_4 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 | PLAYER_STATE1_200000)) {
                        func_8082DD2C(play, this);
                        func_80833AA0(this, play);
                    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                               (this->stateFlags1 & PLAYER_STATE1_8000000)) {
                        func_80831F34(play, this,
                                      func_801242B4(this)
                                          ? &gPlayerAnim_link_swimer_swim_down
                                          : ((this->bodyShockTimer != 0) ? &gPlayerAnim_link_normal_electric_shock_end
                                                                         : &gPlayerAnim_link_derth_rebirth));
                    }
                } else {
                    if ((this->actor.parent == NULL) &&
                        (func_8082DA90(play) || (this->unk_D6B != 0) || !func_80834600(this, play))) {
                        func_8083827C(this, play);
                    } else {
                        this->fallStartHeight = this->actor.world.pos.y;
                    }

                    Player_DetectSecrets(play, this);
                }
            }
        } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (Player_Action_35 == this->actionFunc) &&
                   (this->unk_397 == 4)) {
            this->actor.world.pos.y = this->actor.prevPos.y;
        }

        if (play->csCtx.state != CS_STATE_IDLE) {
            if ((this->csAction != PLAYER_CSACTION_5) && !(this->stateFlags1 & PLAYER_STATE1_800000)) {
                if (!(this->stateFlags2 & PLAYER_STATE2_80) && (this->actor.id == ACTOR_PLAYER)) {
                    if ((play->csCtx.playerCue != NULL) &&
                        (sPlayerCueToCsActionMap[play->csCtx.playerCue->id] != PLAYER_CSACTION_NONE)) {
                        Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_5);
                        Player_StopHorizontalMovement(this);
                    } else if (((u32)this->csAction == PLAYER_CSACTION_NONE) &&
                               !(this->stateFlags2 & (PLAYER_STATE2_400 | PLAYER_STATE2_USING_OCARINA)) &&
                               (play->csCtx.state != CS_STATE_STOP)) {
                        Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_20);
                        Player_StopHorizontalMovement(this);
                    }
                }
            }
        }

        if ((u32)this->csAction != PLAYER_CSACTION_NONE) {
            if ((this->csAction != PLAYER_CSACTION_END) ||
                !(this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 |
                                       PLAYER_STATE1_200000 | PLAYER_STATE1_4000000))) {
                if (Player_Action_CsAction != this->actionFunc) {
                    this->unk_AA5 = PLAYER_UNKAA5_4;
                    if (this->csAction == PLAYER_CSACTION_5) {
                        Player_StartCsAction(play, this);
                        func_8082DAD4(this);
                    }
                }
            } else if (Player_Action_CsAction != this->actionFunc) {
                Player_CsAction_End(play, this, NULL);
            }
        } else {
            this->prevCsAction = PLAYER_CSACTION_NONE;
        }

        func_8083BF54(play, this);
        Lights_PointSetPosition(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y + 40.0f,
                                this->actor.world.pos.z);

        if (((this->focusActor == NULL) || (this->focusActor == this->talkActor) ||
             (this->focusActor->hintId == TATL_HINT_ID_NONE)) &&
            (this->tatlTextId == 0)) {
            this->stateFlags2 &= ~(PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER | PLAYER_STATE2_200000);
        }

        this->stateFlags1 &= ~(PLAYER_STATE1_10 | PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_400000);
        this->stateFlags2 &=
            ~(PLAYER_STATE2_1 | PLAYER_STATE2_4 | PLAYER_STATE2_8 | PLAYER_STATE2_20 | PLAYER_STATE2_40 |
              PLAYER_STATE2_100 | PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND | PLAYER_STATE2_1000 | PLAYER_STATE2_4000 |
              PLAYER_STATE2_10000 | PLAYER_STATE2_400000 | PLAYER_STATE2_4000000);
        this->stateFlags3 &= ~(PLAYER_STATE3_10 | PLAYER_STATE3_40 | PLAYER_STATE3_100 | PLAYER_STATE3_800 |
                               PLAYER_STATE3_1000 | PLAYER_STATE3_100000 | PLAYER_STATE3_2000000 |
                               PLAYER_STATE3_4000000 | PLAYER_STATE3_8000000 | PLAYER_STATE3_10000000);
        func_808425B4(this);
        Player_ProcessControlStick(play, this);

        sWaterSpeedFactor = (this->stateFlags1 & PLAYER_STATE1_8000000) ? 0.5f : 1.0f;
        sInvWaterSpeedFactor = 1.0f / sWaterSpeedFactor;

        sPlayerUseHeldItem = sPlayerHeldItemButtonIsHeldDown = false;

        var_v1 = Play_InCsMode(play);
        sSavedCurrentMask = this->currentMask;
        if (!(this->stateFlags3 & PLAYER_STATE3_4)) {
            this->actionFunc(this, play);
        }

        if (!var_v1) {
            Player_UpdateInterface(play, this);
        }

        Player_UpdateCamAndSeqModes(play, this);

        if (this->skelAnime.movementFlags & ANIM_FLAG_ENABLE_MOVEMENT) {
            AnimTaskQueue_AddActorMovement(play, &this->actor, &this->skelAnime,
                                           (this->skelAnime.movementFlags & ANIM_FLAG_4) ? 1.0f
                                                                                         : this->ageProperties->unk_08);
        }

        Player_UpdateShapeYaw(this, play);

        if (this->actor.flags & ACTOR_FLAG_TALK) {
            this->talkActorDistance = 0.0f;
        } else {
            this->talkActor = NULL;
            this->exchangeItemAction = PLAYER_IA_NONE;
            this->talkActorDistance = FLT_MAX;
        }
        if (!(this->actor.flags & ACTOR_FLAG_OCARINA_INTERACTION) && (this->unk_AA5 != PLAYER_UNKAA5_5)) {
            this->ocarinaInteractionActor = NULL;
            this->ocarinaInteractionDistance = FLT_MAX;
        }
        if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            this->interactRangeActor = NULL;
            this->getItemDirection = 0x6000;
        }
        if (this->actor.parent == NULL) {
            this->rideActor = NULL;
        }

        this->tatlTextId = 0;
        this->unk_B2B = -1;
        this->closestSecretDistSq = FLT_MAX;
        this->doorType = PLAYER_DOORTYPE_NONE;
        this->unk_B75 = 0;
        this->autoLockOnActor = NULL;

        Math_StepToF(&this->windSpeed, 0.0f, 0.5f);
        if ((this->unk_B62 != 0) ||
            ((gSaveContext.magicState == MAGIC_STATE_IDLE) && (gSaveContext.save.saveInfo.playerData.magic != 0) &&
             (this->stateFlags1 & PLAYER_STATE1_10))) {
            func_8082F1AC(play, this);
        }

        temp_fv0 = this->actor.world.pos.y - this->actor.prevPos.y;
        var_fv1_2 =
            temp_fv0 +
            ((this->bodyPartsPos[PLAYER_BODYPART_LEFT_FOOT].y + this->bodyPartsPos[PLAYER_BODYPART_RIGHT_FOOT].y) *
             0.5f);
        temp_fv0 += this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 10.0f;

        if (this->cylinder.elem.atDmgInfo.dmgFlags == 0x80000) {
            this->cylinder.dim.height = 80;
            var_fv1_2 = ((temp_fv0 + var_fv1_2) * 0.5f) - 40.0f;
        } else {
            this->cylinder.dim.height = temp_fv0 - var_fv1_2;

            if (this->cylinder.dim.height < 0) {
                temp_fa0 = temp_fv0;
                temp_fv0 = var_fv1_2;
                var_fv1_2 = temp_fa0;
                this->cylinder.dim.height = -this->cylinder.dim.height;
            }
        }

        this->cylinder.dim.yShift = var_fv1_2 - this->actor.world.pos.y;

        if (this->unk_B62 != 0) {
            this->shieldCylinder.base.acFlags = AC_NONE;
            this->shieldCylinder.elem.atDmgInfo.dmgFlags = 0x80000;
            this->shieldCylinder.elem.atElemFlags = ATELEM_ON;
            this->shieldCylinder.elem.acElemFlags = ACELEM_NONE;
            this->shieldCylinder.dim.height = 80;
            this->shieldCylinder.dim.radius = 50;
            this->shieldCylinder.dim.yShift = ((temp_fv0 + var_fv1_2) * 0.5f - 40.0f) - this->actor.world.pos.y;

            Collider_UpdateCylinder(&this->actor, &this->shieldCylinder);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->shieldCylinder.base);
        } else if (this->stateFlags1 & PLAYER_STATE1_400000) {
            if ((this->transformation == PLAYER_FORM_GORON) || (this->transformation == PLAYER_FORM_DEKU)) {
                this->shieldCylinder.base.acFlags = AC_ON | AC_HARD | AC_TYPE_ENEMY;
                this->shieldCylinder.elem.atDmgInfo.dmgFlags = 0x100000;
                this->shieldCylinder.elem.atElemFlags = ATELEM_NONE;
                this->shieldCylinder.elem.acElemFlags = ACELEM_ON;

                if (this->transformation == PLAYER_FORM_GORON) {
                    this->shieldCylinder.dim.height = 35;
                } else {
                    this->shieldCylinder.dim.height = 30;
                }

                if (this->transformation == PLAYER_FORM_GORON) {
                    this->shieldCylinder.dim.radius = 30;
                } else {
                    this->shieldCylinder.dim.radius = 20;
                }

                this->shieldCylinder.dim.yShift = 0;
                Collider_UpdateCylinder(&this->actor, &this->shieldCylinder);
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->shieldCylinder.base);
                this->cylinder.dim.yShift = 0;
                this->cylinder.dim.height = this->shieldCylinder.dim.height;
            } else {
                this->cylinder.dim.height *= 0.8f;
            }
        }

        Collider_UpdateCylinder(&this->actor, &this->cylinder);
        if (!(this->stateFlags2 & PLAYER_STATE2_4000)) {
            if (!(this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_DEAD | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 |
                                       PLAYER_STATE1_800000)) &&
                !(this->stateFlags3 & PLAYER_STATE3_10000000)) {
                if ((Player_Action_93 != this->actionFunc) && (Player_Action_SlideOnSlope != this->actionFunc) &&
                    (this->actor.draw != NULL)) {
                    if ((this->actor.id != ACTOR_PLAYER) && (this->csAction == PLAYER_CSACTION_110)) {
                        this->cylinder.dim.radius = 8;
                    }
                    CollisionCheck_SetOC(play, &play->colChkCtx, &this->cylinder.base);
                }
            }
            if (!(this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_4000000)) &&
                (this->invincibilityTimer <= 0)) {
                if ((Player_Action_93 != this->actionFunc) &&
                    ((Player_Action_96 != this->actionFunc) || (this->av1.actionVar1 != 1))) {
                    if (this->cylinder.base.atFlags != AT_NONE) {
                        CollisionCheck_SetAT(play, &play->colChkCtx, &this->cylinder.base);
                    }
                    CollisionCheck_SetAC(play, &play->colChkCtx, &this->cylinder.base);
                }
            }
        }

        AnimTaskQueue_SetNextGroup(play);
    }

    func_801229FC(this);
    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);

    if ((this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000)) ||
        (this != GET_PLAYER(play))) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        this->actor.colChkInfo.mass = sPlayerMass[this->transformation];
    }

    this->stateFlags3 &= ~(PLAYER_STATE3_4 | PLAYER_STATE3_400);
    Collider_ResetCylinderAC(play, &this->cylinder.base);
    Collider_ResetCylinderAC(play, &this->shieldCylinder.base);
    Collider_ResetCylinderAT(play, &this->shieldCylinder.base);
    Collider_ResetQuadAT(play, &this->meleeWeaponQuads[0].base);
    Collider_ResetQuadAT(play, &this->meleeWeaponQuads[1].base);
    Collider_ResetQuadAC(play, &this->shieldQuad.base);

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) ||
        (this->actor.floorBgId != BGCHECK_SCENE)) {
        this->unk_AC0 = 0.0f;
    }
    this->actor.shape.yOffset = this->unk_ABC + this->unk_AC0;
}

Vec3f D_8085D41C = { 0.0f, 0.0f, -30.0f };

static bool sNoclipEnabled;

s32 Player_UpdateNoclip(Player* this, PlayState* play) {
    sPlayerControlInput = &play->state.input[0];

    if (!CVarGetInteger("gDeveloperTools.DebugEnabled", 0)) {
        sNoclipEnabled = false;
        return true;
    }

    s32 mask = CVarGetInteger("gDeveloperTools.NoClipBtn", BTN_L | BTN_DRIGHT);
    if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, mask) &&
        CHECK_BTN_ANY(sPlayerControlInput->press.button, mask)) {
        sNoclipEnabled ^= 1;

        if (sNoclipEnabled) {
            Camera_ChangeMode(Play_GetCamera(play, CAM_ID_MAIN), CAM_MODE_ZORAFINZ);
        }
    }

    if (sNoclipEnabled) {
        f32 speed;

        if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_R)) {
            speed = 100.0f;
        } else {
            speed = 20.0f;
        }

        // DebugCamera_ScreenText(3, 2, "DEBUG MODE");

        if (!CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_L)) {
            if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_B)) {
                this->actor.world.pos.y += speed;
            } else if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A)) {
                this->actor.world.pos.y -= speed;
            }

            if (CHECK_BTN_ANY(sPlayerControlInput->cur.button, BTN_DUP | BTN_DLEFT | BTN_DDOWN | BTN_DRIGHT)) {
                s16 angle;
                s16 temp;

                angle = temp = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));

                if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_DDOWN)) {
                    angle = temp + 0x8000;
                } else if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_DLEFT)) {
                    angle = temp + 0x4000 * GameInteractor_InvertControl(GI_INVERT_DEBUG_DPAD_X);
                } else if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_DRIGHT)) {
                    angle = temp - 0x4000 * GameInteractor_InvertControl(GI_INVERT_DEBUG_DPAD_X);
                }

                this->actor.world.pos.x += speed * Math_SinS(angle);
                this->actor.world.pos.z += speed * Math_CosS(angle);
            }
        }

        Player_StopHorizontalMovement(this);

        this->actor.gravity = 0.0f;
        this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;

        // if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sPlayerControlInput->press.button,
        // BTN_DLEFT)) {
        //     Flags_SetTempClear(play, play->roomCtx.curRoom.num);
        // }

        Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);

        return false;
    }

    return true;
}

void Player_Update(Actor* thisx, PlayState* play) {
    static Vec3f sDogSpawnPos;
    Player* this = (Player*)thisx;
    s32 dogParams;
    s32 pad;
    Input input;
    s32 pad2;

    // 2S2H [port] bring over SoH's noclip
    // Could be an if else. I think this looks nicer.
    if (!Player_UpdateNoclip(this, play)) {
        goto skipUpdate;
    }

    this->stateFlags3 &= ~PLAYER_STATE3_10;

    // This block is a leftover dog-following mechanic from OoT
    if (gSaveContext.dogParams < 0) {
        if (Object_GetSlot(&play->objectCtx, OBJECT_DOG) < 0) {
            gSaveContext.dogParams = 0;
        } else {
            Actor* dog;

            gSaveContext.dogParams &= (u16)~0x8000;
            Player_TranslateAndRotateY(this, &this->actor.world.pos, &D_8085D41C, &sDogSpawnPos);

            dogParams = gSaveContext.dogParams;

            dog = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_DG, sDogSpawnPos.x, sDogSpawnPos.y, sDogSpawnPos.z, 0,
                              this->actor.shape.rot.y, 0, dogParams | 0x8000);
            if (dog != NULL) {
                dog->room = -1;
            }
        }
    }

    if ((this->interactRangeActor != NULL) && (this->interactRangeActor->update == NULL)) {
        this->interactRangeActor = NULL;
    }

    if ((this->heldActor != NULL) && (this->heldActor->update == NULL)) {
        Player_DetachHeldActor(play, this);
    }

    if (play->actorCtx.isOverrideInputOn && (this == GET_PLAYER(play))) {
        input = play->actorCtx.overrideInput;
    } else if ((this->csAction == PLAYER_CSACTION_5) ||
               (this->stateFlags1 & (PLAYER_STATE1_20 | PLAYER_STATE1_20000000)) || (this != GET_PLAYER(play)) ||
               func_8082DA90(play) || (gSaveContext.save.saveInfo.playerData.health == 0)) {
        memset(&input, 0, sizeof(Input));
        this->fallStartHeight = this->actor.world.pos.y;
    } else {
        input = *CONTROLLER1(&play->state);
        if (this->textboxBtnCooldownTimer != 0) {
            // Prevent the usage of A/B/C-up.
            // Helps avoid accidental inputs when mashing to close the final textbox.
            input.cur.button &= ~(BTN_CUP | BTN_B | BTN_A);
            input.press.button &= ~(BTN_CUP | BTN_B | BTN_A);
        }
    }

    GameInteractor_ExecuteOnPassPlayerInputs(&input);

    Player_UpdateCommon(this, play, &input);
skipUpdate:
    play->actorCtx.isOverrideInputOn = false;
    memset(&play->actorCtx.overrideInput, 0, sizeof(Input));

    MREG(52) = this->actor.world.pos.x;
    MREG(53) = this->actor.world.pos.y;
    MREG(54) = this->actor.world.pos.z;
    MREG(55) = this->actor.world.rot.y;
}

void Player_DrawGameplay(PlayState* play, Player* this, s32 lod, Gfx* cullDList,
                         OverrideLimbDrawFlex overrideLimbDraw) {
    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0C, cullDList);
    gSPSegment(POLY_XLU_DISP++, 0x0C, cullDList);

    Player_DrawImpl(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, lod,
                    this->transformation, 0, this->actor.shape.face, overrideLimbDraw, Player_PostLimbDrawGameplay,
                    &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80846460(Player* this) {
    Vec3f* pos;
    Vec3f* bodyPartPosPtr;
    s32 i;

    this->actor.focus.pos.x = this->actor.world.pos.x;
    this->actor.focus.pos.z = this->actor.world.pos.z;
    this->actor.focus.pos.y = this->actor.world.pos.y + 24.0f;

    pos = &this->actor.world.pos;
    bodyPartPosPtr = this->bodyPartsPos;
    for (i = 0; i < PLAYER_BODYPART_MAX; i++) {
        Math_Vec3f_Copy(bodyPartPosPtr, pos);
        bodyPartPosPtr++;
    }

    this->bodyPartsPos[PLAYER_BODYPART_HEAD].y = this->actor.world.pos.y + 24.0f;
    this->bodyPartsPos[PLAYER_BODYPART_WAIST].y = this->actor.world.pos.y + 60.0f;
    Math_Vec3f_Copy(&this->actor.shape.feetPos[0], pos);
    Math_Vec3f_Copy(&this->actor.shape.feetPos[1], pos);
}

struct_80124618 D_8085D428[] = {
    { 0, { 0, 0, 0 } },       { 1, { 80, 170, 80 } },   { 3, { 100, 80, 100 } },
    { 7, { 100, 100, 100 } }, { 8, { 100, 100, 100 } },
};
struct_80124618 D_8085D450[] = {
    { 0, { 0, 0, 0 } },       { 1, { 80, 170, 80 } },   { 3, { 100, 80, 100 } },
    { 7, { 100, 100, 100 } }, { 8, { 100, 100, 100 } },
};
struct_80124618 D_8085D478[] = {
    { 0, { 0, 0, 0 } },
    { 8, { 0, 0, 0 } },
};
struct_80124618 D_8085D488[] = {
    { 0, { 100, 100, 100 } }, { 1, { 100, 60, 100 } },  { 3, { 100, 140, 100 } },
    { 7, { 100, 80, 100 } },  { 9, { 100, 100, 100 } },
};
struct_80124618 D_8085D4B0[] = {
    { 0, { 100, 100, 100 } }, { 1, { 100, 70, 100 } },  { 3, { 100, 120, 100 } },
    { 6, { 100, 80, 100 } },  { 8, { 100, 100, 100 } }, { 9, { 100, 100, 100 } },
};
struct_80124618 D_8085D4E0[] = {
    { 0, { 0, 0, 0 } },       { 1, { 0, 0, 0 } },    { 3, { 100, 130, 100 } },
    { 5, { 130, 130, 130 } }, { 7, { 80, 90, 80 } }, { 9, { 100, 100, 100 } },
};
struct_80124618 D_8085D510[] = {
    { 0, { 0, 50, 0 } },
    { 1, { 0, 50, 0 } },
};
struct_80124618 D_8085D520[] = {
    { 0, { 100, 120, 100 } },
    { 1, { 100, 120, 100 } },
};
struct_80124618 D_8085D530[] = {
    { 0, { 160, 120, 160 } },
    { 1, { 160, 120, 160 } },
};
struct_80124618 D_8085D540[] = {
    { 0, { 0, 0, 0 } },
    { 2, { 100, 100, 100 } },
};

struct_80124618* D_8085D550[3] = {
    D_8085D488,
    D_8085D4B0,
    D_8085D4E0,
};
struct_80124618* D_8085D55C[3] = {
    D_8085D428,
    D_8085D450,
    D_8085D478,
};
struct_80124618* D_8085D568[3] = {
    D_8085D510,
    D_8085D520,
    D_8085D530,
};

Gfx* D_8085D574[] = {
    object_link_nuts_DL_009C48,
    object_link_nuts_DL_009AB8,
    object_link_nuts_DL_009DB8,
};

Color_RGB8 D_8085D580 = { 255, 255, 255 };
Color_RGB8 D_8085D584 = { 80, 80, 200 };

void Player_Draw(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;
    f32 one = 1.0f;
    s32 spEC = false;

    Math_Vec3f_Copy(&this->unk_D6C, &this->bodyPartsPos[PLAYER_BODYPART_WAIST]);
    if (this->stateFlags3 & (PLAYER_STATE3_100 | PLAYER_STATE3_40000)) {
        struct_80124618** spE8 = D_8085D550;
        struct_80124618** spE4;
        f32 spE0;
        Gfx** spDC;
        s32 i;

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        func_8012C268(&play->state);
        spEC = true;
        if (this->stateFlags3 & PLAYER_STATE3_40000) {
            Matrix_SetTranslateRotateYXZ(this->unk_AF0[0].x, this->unk_AF0[0].y, this->unk_AF0[0].z, &gZeroVec3s);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            spE8 = D_8085D568;
            spE0 = 0.0f;
        } else {
            Matrix_Translate(0.0f, -this->unk_ABC, 0.0f, MTXMODE_APPLY);
            spE0 = this->av2.actionVar2 - 6;
            if (spE0 < 0.0f) {
                spE8 = D_8085D55C;
                spE0 = this->unk_B86[0];
            }
        }

        spE4 = spE8;
        spDC = D_8085D574;

        for (i = 0; i < 3; i++, spE4++, spDC++) {
            Matrix_Push();
            func_80124618(*spE4, spE0, &this->unk_AF0[1]);
            Matrix_Scale(this->unk_AF0[1].x, this->unk_AF0[1].y, this->unk_AF0[1].z, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, *spDC);

            Matrix_Pop();
        }
        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (!(this->stateFlags2 & PLAYER_STATE2_20000000) && (this->unk_ABC > -3900.0f)) {
        OPEN_DISPS(play->state.gfxCtx);

        if (!spEC) {
            func_8012C268(&play->state);
        }

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        func_800B8050(&this->actor, play, 0);
        func_800B8118(&this->actor, play, 0);
        func_80122868(play, this);

        if (this->stateFlags3 & PLAYER_STATE3_1000) {
            Color_RGB8 spBC;
            f32 spB8 = this->unk_ABC + 1.0f;
            f32 spB4 = 1.0f - (this->unk_ABC * 0.5f);

            func_80846460(this);
            Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + (1200.0f * this->actor.scale.y * spB8),
                             this->actor.world.pos.z, MTXMODE_NEW);

            if (this->unk_B86[0] != 0) {
                Matrix_RotateYS(this->unk_B28, MTXMODE_APPLY);
                Matrix_RotateXS(this->unk_B86[0], MTXMODE_APPLY);
                Matrix_RotateYS(-this->unk_B28, MTXMODE_APPLY);
            }

            Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
            Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);

            Matrix_Scale(this->actor.scale.x * spB4 * 1.15f, this->actor.scale.y * spB8 * 1.15f,
                         CLAMP_MIN(spB8, spB4) * this->actor.scale.z * 1.15f, MTXMODE_APPLY);
            Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
            Scene_SetRenderModeXlu(play, 0, 1);
            Color_RGB8_Lerp(&D_8085D580, &D_8085D584, this->unk_B10[0], &spBC);

            gDPSetEnvColor(POLY_OPA_DISP++, spBC.r, spBC.g, spBC.b, 255);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gLinkGoronCurledDL);

            if (this->unk_B86[1] != 0) {
                if (this->unk_B86[1] < 3) {
                    func_80124618(D_8085D540, this->unk_B86[1], this->unk_AF0);
                    Matrix_Scale(this->unk_AF0[0].x, this->unk_AF0[0].y, this->unk_AF0[0].z, MTXMODE_APPLY);
                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                }

                gSPDisplayList(POLY_OPA_DISP++, object_link_goron_DL_00C540);
            }

            func_80122BA4(play, &this->unk_3D0, 1, 255);
            func_80122BA4(play, &this->unk_3D0, 2, 255);

            if (this->unk_B86[1] < 3) {
                if (this->av1.actionVar1 >= 5) {
                    f32 var_fa1;
                    u8 sp9B;

                    var_fa1 = (this->av1.actionVar1 - 4) * 0.02f;

                    if (this->unk_B86[1] != 0) {
                        sp9B = (-this->unk_B86[1] * 0x55) + 0xFF;
                    } else {
                        sp9B = (200.0f * var_fa1);
                    }

                    if (this->unk_B86[1] != 0) {
                        var_fa1 = 0.65f;
                    } else {
                        var_fa1 *= one;
                    }

                    Matrix_Scale(1.0f, var_fa1, var_fa1, MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                    AnimatedMat_DrawXlu(play, Lib_SegmentedToVirtual(&object_link_goron_Matanimheader_013138));
                    gDPSetEnvColor(POLY_XLU_DISP++, 155, 0, 0, sp9B);
                    gSPDisplayList(POLY_XLU_DISP++, object_link_goron_DL_0127B0);
                    AnimatedMat_DrawXlu(play, Lib_SegmentedToVirtual(&object_link_goron_Matanimheader_014684));
                    gSPDisplayList(POLY_XLU_DISP++, object_link_goron_DL_0134D0);
                }
            }
        } else if ((this->transformation == PLAYER_FORM_GORON) && (this->stateFlags1 & PLAYER_STATE1_400000)) {
            func_80846460(this);
            SkelAnime_DrawFlexOpa(play, this->unk_2C8.skeleton, this->unk_2C8.jointTable, this->unk_2C8.dListCount,
                                  NULL, NULL, NULL);
        } else {
            OverrideLimbDrawFlex sp84 = Player_OverrideLimbDrawGameplayDefault;
            s32 lod = ((this->csAction != PLAYER_CSACTION_NONE) || (this->actor.projectedPos.z < 320.0f)) ? 0 : 1;
            Vec3f sp74;

            //! FAKE
            if (this->transformation == PLAYER_FORM_FIERCE_DEITY) {}

            if (this->stateFlags1 & PLAYER_STATE1_100000) {
                SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &this->actor.focus.pos, &sp74);
                if (sp74.z < -4.0f) {
                    sp84 = Player_OverrideLimbDrawGameplayFirstPerson;
                }
            }

            if (this->stateFlags2 & PLAYER_STATE2_4000000) {
                s16 temp_s0_2 = play->gameplayFrames * 600;
                s16 sp70 = (play->gameplayFrames * 1000) & 0xFFFF;

                Matrix_Push();

                this->actor.scale.y = -this->actor.scale.y;
                Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x,
                                             this->actor.world.pos.y + (2.0f * this->actor.depthInWater) +
                                                 (this->unk_ABC * this->actor.scale.y),
                                             this->actor.world.pos.z, &this->actor.shape.rot);
                Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
                Matrix_RotateXS(temp_s0_2, MTXMODE_APPLY);
                Matrix_RotateYS(sp70, MTXMODE_APPLY);
                Matrix_Scale(1.1f, 0.95f, 1.05f, MTXMODE_APPLY);
                Matrix_RotateYS(-sp70, MTXMODE_APPLY);
                Matrix_RotateXS(-temp_s0_2, MTXMODE_APPLY);
                Player_DrawGameplay(play, this, lod, gCullFrontDList, sp84);
                this->actor.scale.y = -this->actor.scale.y;

                Matrix_Pop();
            }

            gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BOTH);

            gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BOTH);

            if ((this->transformation == PLAYER_FORM_ZORA) && (this->unk_B62 != 0) &&
                !(this->stateFlags3 & PLAYER_STATE3_8000)) {
                Matrix_Push();
                Matrix_RotateXS(-0x4000, MTXMODE_APPLY);
                Matrix_Translate(0.0f, 0.0f, -1800.0f, MTXMODE_APPLY);
                Player_DrawZoraShield(play, this);
                Matrix_Pop();
            }

            Player_DrawGameplay(play, this, lod, gCullBackDList, sp84);
        }

        func_801229A0(play, this);
        if (this->stateFlags2 & PLAYER_STATE2_4000) {
            f32 temp_fa0 = this->unk_B48;

            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, -(s32)play->gameplayFrames & 0x7F, 0x20, 0x20, 1, 0,
                                          ((s32)play->gameplayFrames * -2) & 0x7F, 0x20, 0x20, 0, -1, 0, -2));

            Matrix_Scale(temp_fa0, temp_fa0, temp_fa0, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

            gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, 255);

            gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);
        }

        if (this->getItemDrawIdPlusOne > GID_NONE + 1) {
            Player_DrawGetItem(play, this);
        }

        func_80122D44(play, &this->unk_3D0);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    play->actorCtx.flags &= ~ACTORCTX_FLAG_3;
}

void Player_Destroy(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;

    Effect_Destroy(play, this->meleeWeaponEffectIndex[0]);
    Effect_Destroy(play, this->meleeWeaponEffectIndex[1]);
    Effect_Destroy(play, this->meleeWeaponEffectIndex[2]);
    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
    Collider_DestroyCylinder(play, &this->cylinder);
    Collider_DestroyCylinder(play, &this->shieldCylinder);
    Collider_DestroyQuad(play, &this->meleeWeaponQuads[0]);
    Collider_DestroyQuad(play, &this->meleeWeaponQuads[1]);
    Collider_DestroyQuad(play, &this->shieldQuad);
    ZeldaArena_Free(this->giObjectSegment);
    ZeldaArena_Free(this->maskObjectSegment);
    Magic_Reset(play);
    func_80831454(this);
}

s32 Ship_HandleFirstPersonAiming(PlayState* play, Player* this, s32 arg2) {
    s16 var_s0;
    s32 stickX = 0;
    s32 stickY = 0;
    float gyroX = 0.0f;
    float gyroY = 0.0f;

    if (!(CVarGetInteger("gEnhancements.Camera.FirstPerson.MoveInFirstPerson", 0) &&
          CVarGetInteger("gEnhancements.Camera.FirstPerson.RightStickEnabled", 0))) {
        s32 leftStickX = sPlayerControlInput->rel.stick_x; // -60 to 60
        s32 leftStickY = sPlayerControlInput->rel.stick_y; // -60 to 60

        leftStickX *= GameInteractor_InvertControl(GI_INVERT_FIRST_PERSON_AIM_X);
        leftStickY *= -GameInteractor_InvertControl(GI_INVERT_FIRST_PERSON_AIM_Y);

        stickX += leftStickX * CVarGetFloat("gEnhancements.Camera.FirstPerson.SensitivityX", 1.0f);
        stickY += leftStickY * CVarGetFloat("gEnhancements.Camera.FirstPerson.SensitivityY", 1.0f);
    }

    if (CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroEnabled", 0)) {
        gyroX = sPlayerControlInput->cur.gyro_y * 720; // -40 to 40, avg -4 to 4
        gyroY = sPlayerControlInput->cur.gyro_x * 720; // -20 to 20, avg -2 to 2

        gyroX *= GameInteractor_InvertControl(GI_INVERT_FIRST_PERSON_GYRO_X);
        gyroY *= -GameInteractor_InvertControl(GI_INVERT_FIRST_PERSON_GYRO_Y);

        gyroX *= CVarGetFloat("gEnhancements.Camera.FirstPerson.GyroSensitivityX", 1.0f);
        gyroY *= CVarGetFloat("gEnhancements.Camera.FirstPerson.GyroSensitivityY", 1.0f);
    }

    if (CVarGetInteger("gEnhancements.Camera.FirstPerson.RightStickEnabled", 0)) {
        s32 rightStickX = sPlayerControlInput->cur.right_stick_x; // -40 to 40, avg -4 to 4
        s32 rightStickY = sPlayerControlInput->cur.right_stick_y; // -20 to 20, avg -2 to 2

        rightStickX *= GameInteractor_InvertControl(GI_INVERT_FIRST_PERSON_RIGHT_STICK_X);
        rightStickY *= -GameInteractor_InvertControl(GI_INVERT_FIRST_PERSON_RIGHT_STICK_Y);

        stickX += rightStickX * CVarGetFloat("gEnhancements.Camera.FirstPerson.RightStickSensitivityX", 1.0f);
        stickY += rightStickY * CVarGetFloat("gEnhancements.Camera.FirstPerson.RightStickSensitivityY", 1.0f);
    }

    stickX = CLAMP(stickX, -60, 60);
    stickY = CLAMP(stickY, -60, 60);

    if (!func_800B7128(this) && !func_8082EF20(this) && !arg2) { // First person without weapon
        var_s0 = stickY * 0xF0;
        if (CVarGetInteger("gEnhancements.Camera.FirstPerson.DisableFirstPersonAutoCenterView", 0) ||
            CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroEnabled", 0)) {
            this->actor.focus.rot.x += var_s0 * 0.1f;
        } else {
            Math_SmoothStepToS(&this->actor.focus.rot.x, var_s0, 0xE, 0xFA0, 0x1E);
        }
        this->actor.focus.rot.x += gyroY;
        this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -14000, 14000);

        var_s0 = stickX * -0x10;
        var_s0 = CLAMP(var_s0, -0xBB8, 0xBB8);
        this->actor.focus.rot.y += var_s0 + gyroX;
    } else { // First person with weapon
        s16 temp3;

        temp3 = ((stickY >= 0) ? 1 : -1) * (s32)((1.0f - Math_CosS(stickY * 0xC8)) * 1500.0f);
        this->actor.focus.rot.x += temp3 + gyroY;

        if (this->stateFlags1 & PLAYER_STATE1_800000) {
            this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -0x1F40, 0xFA0);
        } else {
            this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -0x36B0, 0x36B0);
        }

        var_s0 = this->actor.focus.rot.y - this->actor.shape.rot.y;
        temp3 = ((stickX >= 0) ? 1 : -1) * (s32)((1.0f - Math_CosS(stickX * 0xC8)) * -1500.0f);
        var_s0 += temp3;

        this->actor.focus.rot.y = CLAMP(var_s0 + gyroX, -0x4AAA, 0x4AAA) + this->actor.shape.rot.y;
    }

    bool playerMovementLocked = (this->actionFunc == Player_Action_52) || // Riding on Epona
                                (this->actionFunc == Player_Action_80) || // Riding swamp boat (non-archery)
                                (this->actionFunc == Player_Action_81);   // Bow minigames

    if (!playerMovementLocked && CVarGetInteger("gEnhancements.Camera.FirstPerson.MoveInFirstPerson", 0) &&
        CVarGetInteger("gEnhancements.Camera.FirstPerson.RightStickEnabled", 0)) {
        f32 movementSpeed = 8.25f; // account for form
        if (GameInteractor_Should(VB_CONSIDER_BUNNY_HOOD_EQUIPPED, this->currentMask == PLAYER_MASK_BUNNY, this)) {
            movementSpeed *= 1.5f;
        }

        f32 relX =
            (-sPlayerControlInput->rel.stick_x / 10) * GameInteractor_InvertControl(GI_INVERT_FIRST_PERSON_MOVING_X);
        f32 relY = (sPlayerControlInput->rel.stick_y / 10);

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
        func_8083EA44(this, distance / 4.5f);

        this->actor.world.pos.x += (relX2 * movementSpeed) + this->actor.colChkInfo.displacement.x;
        this->actor.world.pos.z += (relY2 * movementSpeed) + this->actor.colChkInfo.displacement.z;
    }

    this->unk_AA6_rotFlags |= UNKAA6_ROT_FOCUS_Y;

    return func_80832754(this, (play->bButtonAmmoPlusOne != 0) || func_800B7128(this) || func_8082EF20(this));
}

s32 func_80847190(PlayState* play, Player* this, s32 arg2) {
    // #region 2S2H [Enhancements] Use our own heavily modified version of this for customizations
    return Ship_HandleFirstPersonAiming(play, this, arg2);
    // #endregion

    s32 pad;
    s16 var_s0;
    s32 stickX = sPlayerControlInput->rel.stick_x;

    stickX *= GameInteractor_InvertControl(GI_INVERT_FIRST_PERSON_AIM_X);

    if (!func_800B7128(this) && !func_8082EF20(this) && !arg2) {
        var_s0 = sPlayerControlInput->rel.stick_y * 0xF0;
        Math_SmoothStepToS(&this->actor.focus.rot.x, var_s0, 0xE, 0xFA0, 0x1E);

        var_s0 = stickX * -0x10;
        var_s0 = CLAMP(var_s0, -0xBB8, 0xBB8);
        this->actor.focus.rot.y += var_s0;
    } else {
        s16 temp3;

        temp3 = ((sPlayerControlInput->rel.stick_y >= 0) ? 1 : -1) *
                (s32)((1.0f - Math_CosS(sPlayerControlInput->rel.stick_y * 0xC8)) * 1500.0f);
        this->actor.focus.rot.x += temp3;

        if (this->stateFlags1 & PLAYER_STATE1_800000) {
            this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -0x1F40, 0xFA0);
        } else {
            this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -0x36B0, 0x36B0);
        }

        var_s0 = this->actor.focus.rot.y - this->actor.shape.rot.y;
        temp3 = ((stickX >= 0) ? 1 : -1) * (s32)((1.0f - Math_CosS(stickX * 0xC8)) * -1500.0f);
        var_s0 += temp3;

        this->actor.focus.rot.y = CLAMP(var_s0, -0x4AAA, 0x4AAA) + this->actor.shape.rot.y;
    }

    this->unk_AA6_rotFlags |= UNKAA6_ROT_FOCUS_Y;

    return func_80832754(this, (play->bButtonAmmoPlusOne != 0) || func_800B7128(this) || func_8082EF20(this));
}

void func_8084748C(Player* this, f32* speed, f32 speedTarget, s16 yawTarget) {
    f32 incrStep = this->skelAnime.curFrame - 10.0f;
    f32 maxSpeed = (R_RUN_SPEED_LIMIT / 100.0f) * 0.8f;

    if (GameInteractor_Should(VB_SPEED_MODIFIER_SWIM, true, &incrStep, &maxSpeed, speed, &speedTarget)) {

        if (*speed > maxSpeed) {
            *speed = maxSpeed;
        }

        if ((0.0f < incrStep) && (incrStep < 16.0f)) {
            incrStep = fabsf(incrStep) * 0.5f;
        } else {
            speedTarget = 0.0f;
            incrStep = 0.0f;
        }
        Math_AsymStepToF(speed, speedTarget * 0.8f, incrStep, (fabsf(*speed) * 0.02f) + 0.05f);
    }
    Math_ScaledStepToS(&this->yaw, yawTarget, 0x640); // 1 ESS turn, also one frame of first-person rotation
}

void func_808475B4(Player* this) {
    f32 sp4;
    f32 temp_fa1;
    f32 temp_fv0;
    f32 var_ft4 = -5.0f;
    f32 var_ft5 = this->ageProperties->unk_28;
    f32 var_ft5_4;

    temp_fv0 = this->actor.depthInWater - var_ft5;
    if (this->actor.velocity.y < 0.0f) {
        var_ft5 += 1.0f;
    }

    if (this->actor.depthInWater < var_ft5) {
        temp_fv0 = CLAMP(temp_fv0, -0.4f, -0.1f);
        sp4 = temp_fv0 - ((this->actor.velocity.y <= 0.0f) ? 0.0f : this->actor.velocity.y * 0.5f);
    } else {
        if (!(this->stateFlags1 & PLAYER_STATE1_DEAD) && (this->currentBoots >= PLAYER_BOOTS_ZORA_UNDERWATER) &&
            (this->actor.velocity.y >= -5.0f)) {
            sp4 = -0.3f;
        } else if ((this->transformation == PLAYER_FORM_DEKU) && (this->actor.velocity.y < 0.0f)) {
            var_ft4 = 0.0f;
            sp4 = -this->actor.velocity.y;
        } else {
            var_ft4 = 2.0f;
            var_ft5_4 = CLAMP(temp_fv0, 0.1f, 0.4f);
            sp4 = ((this->actor.velocity.y >= 0.0f) ? 0.0f : this->actor.velocity.y * -0.3f) + var_ft5_4;
        }

        if (this->actor.depthInWater > 100.0f) {
            this->stateFlags2 |= PLAYER_STATE2_400;
        }
    }

    this->actor.velocity.y += sp4;
    if (((this->actor.velocity.y - var_ft4) * sp4) > 0.0f) {
        this->actor.velocity.y = var_ft4;
    }
    this->actor.gravity = 0.0f;
}

void func_808477D0(PlayState* play, Player* this, Input* input, f32 arg3) {
    f32 var_fv0;

    if ((input != NULL) && CHECK_BTN_ANY(input->press.button, BTN_B | BTN_A)) {
        var_fv0 = 1.0f;
    } else {
        var_fv0 = 0.5f;
    }

    var_fv0 *= arg3;
    if (GameInteractor_Should(VB_CLAMP_ANIMATION_SPEED, true, &var_fv0)) {
        var_fv0 = CLAMP(var_fv0, 1.0f, 2.5f);
    }
    this->skelAnime.playSpeed = var_fv0;

    PlayerAnimation_Update(play, &this->skelAnime);
}

s32 func_80847880(PlayState* play, Player* this) {
    if (play->bButtonAmmoPlusOne != 0) {
        if (play->sceneId == SCENE_20SICHITAI) {
            Player_SetAction(play, this, Player_Action_80, 0);
            play->bButtonAmmoPlusOne = 0;
            this->csAction = PLAYER_CSACTION_NONE;
            return true;
        }

        func_8082DE50(play, this);
        Player_SetAction(play, this, Player_Action_81, 0);
        if (!func_800B7118(this) || Player_IsHoldingHookshot(this)) {
            Player_UseItem(play, this, ITEM_BOW);
        }
        Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
        this->csAction = PLAYER_CSACTION_NONE;
        this->stateFlags1 |= PLAYER_STATE1_100000;
        Player_StopHorizontalMovement(this);
        func_80836D8C(this);

        return true;
    }
    return false;
}

s32 func_80847994(PlayState* play, Player* this) {
    if (this->stateFlags3 & PLAYER_STATE3_20) {
        this->stateFlags3 &= ~PLAYER_STATE3_20;
        this->itemAction = PLAYER_IA_OCARINA;
        this->unk_AA5 = PLAYER_UNKAA5_5;
        Player_ActionHandler_13(this, play);
        return true;
    }
    return false;
}

void func_808479F4(PlayState* play, Player* this, f32 arg2) {
    if (this->actor.wallBgId != BGCHECK_SCENE) {
        DynaPolyActor* actor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);

        if (actor != NULL) {
            func_800B72F8(actor, arg2, this->actor.world.rot.y);
        }
    }
}

void func_80847A50(Player* this) {
    Player_PlaySfx(this, ((this->av1.actionVar1 != 0) ? NA_SE_PL_WALK_METAL1 : NA_SE_PL_WALK_LADDER) +
                             this->ageProperties->surfaceSfxIdOffset);
}

Vec3f D_8085D588[] = {
    { 30.0f, 0.0f, 0.0f },
    { -30.0f, 0.0f, 0.0f },
};
Vec3f D_8085D5A0[] = {
    { 60.0f, 20.0f, 0.0f },
    { -60.0f, 20.0f, 0.0f },
};
Vec3f D_8085D5B8[] = {
    { 60.0f, -20.0f, 0.0f },
    { -60.0f, -20.0f, 0.0f },
};
Vec3f D_8085D5D0 = { 0.0f, 0.0f, -30.0f };

// related to mounting/unmounting the horse
s32 func_80847A94(PlayState* play, Player* this, s32 arg2, f32* arg3) {
    Actor* rideActor = this->rideActor;
    f32 sp60 = rideActor->world.pos.y + 20.0f;
    f32 sp5C = rideActor->world.pos.y - 20.0f;
    Vec3f sp50;
    Vec3f sp44;
    CollisionPoly* wallPoly;
    CollisionPoly* floorPoly;
    s32 wallBgId;
    s32 floorBgId;

    *arg3 = func_80835CD8(play, this, &D_8085D588[arg2], &sp50, &floorPoly, &floorBgId);

    if ((sp5C < *arg3) && (*arg3 < sp60)) {
        if (!Player_PosVsWallLineTest(play, this, &D_8085D5A0[arg2], &wallPoly, &wallBgId, &sp44)) {
            if (!Player_PosVsWallLineTest(play, this, &D_8085D5B8[arg2], &wallPoly, &wallBgId, &sp44)) {
                this->actor.floorPoly = floorPoly;
                //! @note: no poly is assigned to `wallBgId` when `Player_PosVsWallLineTest` fails.
                //! Therefore, the default value `BGCHECK_SCENE` is assigned.
                this->actor.floorBgId = wallBgId;
                this->floorSfxOffset = SurfaceType_GetSfxOffset(&play->colCtx, floorPoly, floorBgId);
                return true;
            }
        }
    }
    return false;
}

s32 func_80847BF0(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    s32 var_a2;
    f32 sp34;

    if (this->av2.actionVar2 < 0) {
        this->av2.actionVar2 = 0x63;
    } else {
        var_a2 = (this->mountSide < 0) ? 0 : 1;

        if (!func_80847A94(play, this, var_a2, &sp34)) {
            var_a2 ^= 1;
            if (!func_80847A94(play, this, var_a2, &sp34)) {
                return false;
            }

            this->mountSide = -this->mountSide;
        }

        if (play->csCtx.state == CS_STATE_IDLE) {
            if (!func_8082DA90(play)) {
                if (EN_HORSE_CHECK_1(rideActor) || EN_HORSE_CHECK_4(rideActor)) {
                    this->stateFlags2 |= PLAYER_STATE2_400000;

                    if (EN_HORSE_CHECK_1(rideActor) ||
                        (EN_HORSE_CHECK_4(rideActor) && CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A))) {
                        rideActor->actor.child = NULL;

                        Player_SetAction_PreserveMoveFlags(play, this, Player_Action_53, 0);
                        this->unk_B48 = sp34 - rideActor->actor.world.pos.y;

                        Player_Anim_PlayOnce(play, this,
                                             (this->mountSide < 0) ? &gPlayerAnim_link_uma_left_down
                                                                   : &gPlayerAnim_link_uma_right_down);

                        return true;
                    }
                }
            }
        }
    }

    return false;
}

// Used in 2 horse-related functions
void func_80847E2C(Player* this, f32 arg1, f32 minFrame) {
    f32 addend;
    f32 dir;

    if ((this->unk_B48 != 0.0f) && (minFrame <= this->skelAnime.curFrame)) {
        if (arg1 < fabsf(this->unk_B48)) {
            dir = (this->unk_B48 >= 0.0f) ? 1 : -1;
            addend = dir * arg1;
        } else {
            addend = this->unk_B48;
        }
        this->actor.world.pos.y += addend;
        this->unk_B48 -= addend;
    }
}

bool func_80847ED4(Player* this) {
    return (this->interactRangeActor != NULL) && (this->interactRangeActor->id == ACTOR_EN_ZOG) &&
           CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A);
}

void func_80847F1C(Player* this) {
    s32 pad;
    f32 yPos;
    s16 yaw;
    Actor* interactRangeActor = this->interactRangeActor;

    if (func_80847ED4(this)) {
        yPos = this->actor.world.pos.y;
        yaw = this->yaw - interactRangeActor->shape.rot.y;
        Lib_Vec3f_TranslateAndRotateY(&interactRangeActor->world.pos, interactRangeActor->shape.rot.y, &D_8085D5D0,
                                      &this->actor.world.pos);
        this->actor.world.pos.y = yPos;
        this->actor.shape.rot.y = interactRangeActor->shape.rot.y;

        interactRangeActor->speed = Math_CosS(ABS_ALT(yaw)) * this->speedXZ * 0.5f;
        if (interactRangeActor->speed < 0.0f) {
            interactRangeActor->speed = 0.0f;
        }
        Player_SetParallel(this);
    }
}

AnimSfxEntry D_8085D5DC[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 0, NA_SE_PL_SWIM, STOP),
};

void func_80847FF8(Player* this, f32* arg1, f32 arg2, s16 arg3) {
    func_8084748C(this, arg1, arg2, arg3);
    Player_PlayAnimSfx(this, D_8085D5DC);
    func_80847F1C(this);
}

void func_80848048(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_58, 0);
    Player_Anim_PlayLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim);
}

s32 func_80848094(PlayState* play, Player* this, f32* arg2, s16* arg3) {
    PlayerAnimationHeader* anim;
    s16 temp_v0 = this->yaw - *arg3;
    s32 temp_v0_2;

    if (ABS_ALT(temp_v0) > 0x6000) {
        anim = &gPlayerAnim_link_swimer_swim_wait;
        if (Math_StepToF(&this->speedXZ, 0.0f, 1.0f)) {
            this->yaw = *arg3;
        } else {
            *arg2 = 0.0f;
            *arg3 = this->yaw;
        }
    } else {
        temp_v0_2 = func_8083E514(this, arg2, arg3, play);
        if (temp_v0_2 > 0) {
            anim = &gPlayerAnim_link_swimer_swim;
        } else if (temp_v0_2 < 0) {
            anim = &gPlayerAnim_link_swimer_back_swim;
        } else {
            s16 diff = BINANG_SUB(this->actor.shape.rot.y, *arg3);

            if (diff > 0) {
                anim = &gPlayerAnim_link_swimer_Rside_swim;
            } else {
                anim = &gPlayerAnim_link_swimer_Lside_swim;
            }
        }
    }

    if (!BEN_ANIM_EQUAL(anim, this->skelAnime.animation)) {
        Player_Anim_PlayLoopSlowMorph(play, this, anim);
        return true;
    }
    return false;
}

void func_808481CC(PlayState* play, Player* this, f32 arg2) {
    f32 speedTarget;
    s16 yawTarget;

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
    func_8084748C(this, &this->speedXZ, speedTarget / 2.0f, yawTarget);
    func_8084748C(this, &this->actor.velocity.y, arg2, this->yaw);
}

void func_80848250(PlayState* play, Player* this) {
    this->getItemDrawIdPlusOne = GID_NONE + 1;
    this->stateFlags1 &= ~(PLAYER_STATE1_400 | PLAYER_STATE1_CARRYING_ACTOR);
    this->getItemId = GI_NONE;
    Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
}

void func_80848294(PlayState* play, Player* this) {
    func_80848250(play, this);
    Player_Anim_ResetModelRotY(this);
    func_80839E74(this, play);
    this->yaw = this->actor.shape.rot.y;
}

// Player_GetItem?
s32 func_808482E0(PlayState* play, Player* this) {
    if (this->getItemId == GI_NONE) {
        return true;
    }

    if (this->av1.actionVar1 == 0) {
        GetItemEntry* giEntry = &sGetItemTable[this->getItemId - 1];

        this->av1.actionVar1 = 1;
        Message_StartTextbox(play, giEntry->textId, &this->actor);
        Item_Give(play, giEntry->itemId);

        if ((this->getItemId >= GI_MASK_DEKU) && (this->getItemId <= GI_MASK_KAFEIS_MASK)) {
            Audio_PlayFanfare(NA_BGM_GET_NEW_MASK);
        } else if (((this->getItemId >= GI_RUPEE_GREEN) && (this->getItemId <= GI_RUPEE_10)) ||
                   (this->getItemId == GI_RECOVERY_HEART)) {
            Audio_PlaySfx(NA_SE_SY_GET_BOXITEM);
        } else {
            s32 seqId;
            bool vanillaCondition = (this->getItemId == GI_HEART_CONTAINER) ||
                                    ((this->getItemId == GI_HEART_PIECE) && EQ_MAX_QUEST_HEART_PIECE_COUNT);
            if (GameInteractor_Should(VB_PLAY_HEART_CONTAINER_GET_FANFARE, vanillaCondition, this->getItemId)) {
                seqId = NA_BGM_GET_HEART;
            } else {
                s32 var_v1;

                if ((this->getItemId == GI_HEART_PIECE) ||
                    ((this->getItemId >= GI_RUPEE_PURPLE) && (this->getItemId <= GI_RUPEE_HUGE))) {
                    var_v1 = NA_BGM_GET_SMALL_ITEM;
                } else {
                    var_v1 = NA_BGM_GET_ITEM;
                }
                seqId = var_v1;
            }

            Audio_PlayFanfare(seqId);
        }
    } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (GameInteractor_Should(VB_PLAY_SONG_OF_TIME_CS, this->getItemId == GI_OCARINA_OF_TIME, this)) {
            // zelda teaching song of time cs?
            play->nextEntrance = ENTRANCE(CUTSCENE, 0);
            gSaveContext.nextCutsceneIndex = 0xFFF2;
            play->transitionTrigger = TRANS_TRIGGER_START;
            play->transitionType = TRANS_TYPE_FADE_WHITE;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
            this->stateFlags1 &= ~PLAYER_STATE1_20000000;
            Player_TryCsAction(play, NULL, PLAYER_CSACTION_WAIT);
        }
        this->getItemId = GI_NONE;
    }

    return false;
}

AnimSfxEntry D_8085D5E0[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 60, NA_SE_IT_MASTER_SWORD_SWING, STOP),
};

void func_808484CC(Player* this) {
    Player_PlayAnimSfx(this, D_8085D5E0);
}

void func_808484F0(Player* this) {
    this->unk_B08 += this->unk_B0C;
    this->unk_B0C -= this->unk_B08 * 5.0f;
    this->unk_B0C *= 0.3f;

    if (fabsf(this->unk_B0C) < 0.00001f) {
        this->unk_B0C = 0.0f;
        if (fabsf(this->unk_B08) < 0.00001f) {
            this->unk_B08 = 0.0f;
        }
    }
}

s32 Player_ActionHandler_7(Player* this, PlayState* play) {
    if (!func_8083A6C0(play, this)) {
        if (func_808396B8(play, this)) {
            PlayerMeleeWeaponAnimation meleeWeaponAnim = func_808335F4(this);

            func_80833864(play, this, meleeWeaponAnim);
            if ((meleeWeaponAnim >= PLAYER_MWA_SPIN_ATTACK_1H) ||
                ((this->transformation == PLAYER_FORM_FIERCE_DEITY) && Player_IsZTargeting(this))) {
                this->stateFlags2 |= PLAYER_STATE2_20000;
                func_808332A0(play, this, 0, meleeWeaponAnim < PLAYER_MWA_SPIN_ATTACK_1H);
            }
        } else {
            return false;
        }
    }
    return true;
}

// elegy of emptiness
void func_80848640(PlayState* play, Player* this) {
    EnTorch2* torch2;
    Actor* effChange;

    torch2 = play->actorCtx.elegyShells[this->transformation];
    if (torch2 != NULL) {
        Math_Vec3f_Copy(&torch2->actor.home.pos, &this->actor.world.pos);
        torch2->actor.home.rot.y = this->actor.shape.rot.y;
        torch2->state = 0;
        torch2->framesUntilNextState = 20;
    } else {
        torch2 = (EnTorch2*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TORCH2, this->actor.world.pos.x,
                                        this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                                        this->transformation);
    }

    if (torch2 != NULL) {
        play->actorCtx.elegyShells[this->transformation] = torch2;
        Play_SetupRespawnPoint(play, this->transformation + 3, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_B));
    }

    effChange = Actor_Spawn(&play->actorCtx, play, ACTOR_EFF_CHANGE, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                            (GET_PLAYER_FORM << 3) | this->transformation);
    if (effChange != NULL) {
        //! @bug: This function should only pass Player*: it uses *(this + 0x153), which is meant to be
        //! player->currentMask, but in this case is garbage in the skelAnime
        Player_PlaySfx((Player*)effChange, NA_SE_PL_TRANSFORM);
    }
}

s32 Player_UpperAction_0(Player* this, PlayState* play) {
    if (func_80830B88(play, this)) {
        return true;
    }
    return false;
}

s32 Player_UpperAction_1(Player* this, PlayState* play) {
    if (func_80830B88(play, this) || func_80830DF0(this, play)) {
        return true;
    }
    return false;
}

s32 Player_UpperAction_ChangeHeldItem(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnimeUpper) ||
        ((Player_ItemToItemAction(this, this->heldItemId) == this->heldItemAction) &&
         (sPlayerUseHeldItem =
              (sPlayerUseHeldItem || GameInteractor_Should(VB_USE_HELD_ITEM_AFTER_CHANGE,
                                                           (this->modelAnimType != PLAYER_ANIMTYPE_3) &&
                                                               (this->heldItemAction != PLAYER_IA_DEKU_STICK) &&
                                                               (play->bButtonAmmoPlusOne == 0),
                                                           this))))) {
        Player_SetUpperAction(play, this, sItemActionUpdateFuncs[this->heldItemAction]);
        this->unk_ACC = 0;
        this->idleType = PLAYER_IDLE_DEFAULT;
        sPlayerHeldItemButtonIsHeldDown = sPlayerUseHeldItem;
        return this->upperActionFunc(this, play);
    }

    if (Player_CheckForIdleAnim(this) != IDLE_ANIM_NONE) {
        Player_WaitToFinishItemChange(play, this);
        Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
        this->idleType = PLAYER_IDLE_DEFAULT;
    } else {
        Player_WaitToFinishItemChange(play, this);
    }

    return true;
}

s32 Player_UpperAction_3(Player* this, PlayState* play) {
    PlayerAnimation_Update(play, &this->skelAnimeUpper);
    if (!CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_R)) {
        func_80830CE8(play, this);
    } else {
        this->stateFlags1 |= PLAYER_STATE1_400000;
        Player_SetModelsForHoldingShield(this);
        if ((this->transformation == PLAYER_FORM_ZORA) && CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_B)) {
            func_8082F164(this, BTN_R | BTN_B);
        }
    }
    return true;
}

s32 Player_UpperAction_4(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnimeUpper)) {
        PlayerAnimationHeader* anim;
        f32 endFrame;

        anim = func_80830A58(play, this);
        endFrame = Animation_GetLastFrame(anim);
        PlayerAnimation_Change(play, &this->skelAnimeUpper, anim, PLAYER_ANIM_NORMAL_SPEED, endFrame, endFrame,
                               ANIMMODE_ONCE, 0.0f);
    }

    this->stateFlags1 |= PLAYER_STATE1_400000;
    Player_SetModelsForHoldingShield(this);
    return true;
}

s32 Player_UpperAction_5(Player* this, PlayState* play) {
    sPlayerUseHeldItem = sPlayerHeldItemButtonIsHeldDown;
    if (sPlayerUseHeldItem || PlayerAnimation_Update(play, &this->skelAnimeUpper)) {
        Player_SetUpperAction(play, this, sItemActionUpdateFuncs[this->heldItemAction]);
        PlayerAnimation_PlayLoop(play, &this->skelAnimeUpper, D_8085BE84[PLAYER_ANIMGROUP_wait][this->modelAnimType]);
        this->idleType = PLAYER_IDLE_DEFAULT;
        this->upperActionFunc(this, play);
        return false;
    }
    return true;
}

s32 Player_UpperAction_6(Player* this, PlayState* play) {
    if (this->unk_B28 >= 0) {
        this->unk_B28 = -this->unk_B28;
    }

    if (!Player_IsHoldingHookshot(this) || func_80831124(play, this)) {
        if (!func_80830B88(play, this) && !func_80831094(this, play)) {
            return false;
        }
    }
    return true;
}

PlayerAnimationHeader* D_8085D5E4[] = {
    &gPlayerAnim_link_hook_walk2ready,
    &gPlayerAnim_link_bow_walk2ready,
    &gPlayerAnim_pn_tamahakidf,
};

PlayerAnimationHeader* D_8085D5F0[] = {
    &gPlayerAnim_link_hook_wait,
    &gPlayerAnim_link_bow_bow_wait,
    &gPlayerAnim_pn_tamahakidf,
};

u16 D_8085D5FC[] = {
    NA_SE_IT_BOW_FLICK,
    NA_SE_PL_DEKUNUTS_MISS_FIRE,
    NA_SE_NONE,
    NA_SE_NONE,
};

s32 Player_UpperAction_7(Player* this, PlayState* play) {
    s32 index;
    s32 temp;

    if (Player_IsHoldingHookshot(this)) {
        index = 0;
    } else {
        temp = (this->transformation != PLAYER_FORM_DEKU) ? 1 : 2;
        index = temp;
    }

    if (this->transformation != PLAYER_FORM_DEKU) {
        Math_ScaledStepToS(&this->upperLimbRot.z, 0x4B0, 0x190);
        this->unk_AA6_rotFlags |= UNKAA6_ROT_UPPER_Z;
    }

    if ((this->unk_ACE == 0) && (Player_CheckForIdleAnim(this) == IDLE_ANIM_NONE) &&
        (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_bow_side_walk))) {
        PlayerAnimation_PlayOnce(play, &this->skelAnimeUpper, D_8085D5E4[index]);
        this->unk_ACE = -1;
    } else if (PlayerAnimation_Update(play, &this->skelAnimeUpper)) {
        PlayerAnimation_PlayLoop(play, &this->skelAnimeUpper, D_8085D5F0[index]);
        this->unk_ACE = 1;
    } else if (this->unk_ACE == 1) {
        this->unk_ACE = 2;
    }

    if (this->unk_ACC >= 0xB) {
        this->unk_ACC--;
    }

    func_80831010(this, play);
    if ((this->unk_ACE > 0) && ((this->unk_B28 < 0) || (!sPlayerHeldItemButtonIsHeldDown && !func_80830FD4(play)))) {
        Player_SetUpperAction(play, this, Player_UpperAction_8);
        if (this->unk_B28 >= 0) {
            if (index != 0) {
                if (!func_80831194(play, this)) {
                    // 2S2H [Port] When using action swap without arrows, D_8085D5FC is indexed with -1 leading
                    // to UB sent into Player_PlaySfx. On console this resolves as 58104 and causes a crash.
                    // For the port hard crashing is not desirable, so we are opting to clear the game state
                    if (this->unk_B28 - 1 < 0) {
                        Ship_HandleConsoleCrashAsReset();
                        Player_PlaySfx(this, NA_SE_NONE);
                    } else {
                        Player_PlaySfx(this, D_8085D5FC[this->unk_B28 - 1]);
                    }
                }

                if (this->transformation == PLAYER_FORM_DEKU) {
                    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnimeUpper, &gPlayerAnim_pn_tamahaki,
                                                     PLAYER_ANIM_ADJUSTED_SPEED);
                }
            } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                func_80831194(play, this);
            }
        }
        this->unk_ACC = 0xA;
        Player_StopHorizontalMovement(this);
    } else {
        this->stateFlags3 |= PLAYER_STATE3_40;
    }

    return 1;
}

s32 Player_UpperAction_8(Player* this, PlayState* play) {
    s32 animFinished = PlayerAnimation_Update(play, &this->skelAnimeUpper);

    if (Player_IsHoldingHookshot(this) && !func_80831124(play, this)) {
        return true;
    }

    if (!func_80830B88(play, this) &&
        ((((this->unk_B28 < 0) && sPlayerHeldItemButtonIsHeldDown) ||
          ((animFinished || (this->transformation != PLAYER_FORM_DEKU)) && sPlayerUseHeldItem)) ||
         func_80830F9C(play))) {

        this->unk_B28 = ABS_ALT(this->unk_B28);
        if (func_808306F8(this, play)) {
            if (Player_IsHoldingHookshot(this)) {
                this->unk_ACE = 1;
            } else {
                PlayerAnimation_PlayOnce(play, &this->skelAnimeUpper,
                                         (this->transformation == PLAYER_FORM_DEKU)
                                             ? &gPlayerAnim_pn_tamahakidf
                                             : &gPlayerAnim_link_bow_bow_shoot_next);
            }
        }
    } else {
        if (this->unk_ACC != 0) {
            this->unk_ACC--;
        }

        if ((Player_IsZTargeting(this)) || (this->unk_AA5 != PLAYER_UNKAA5_0) ||
            (this->stateFlags1 & PLAYER_STATE1_100000)) {
            if (this->unk_ACC == 0) {
                this->unk_ACC++;
            }
            return true;
        }

        if (Player_IsHoldingHookshot(this)) {
            Player_SetUpperAction(play, this, Player_UpperAction_6);
        } else {
            Player_SetUpperAction(play, this, Player_UpperAction_9);
            PlayerAnimation_PlayOnce(play, &this->skelAnimeUpper,
                                     (this->transformation == PLAYER_FORM_DEKU) ? &gPlayerAnim_pn_tamahakidf
                                                                                : &gPlayerAnim_link_bow_bow_shoot_end);
        }
        this->unk_ACC = 0;
    }

    return true;
}

s32 Player_UpperAction_9(Player* this, PlayState* play) {
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || PlayerAnimation_Update(play, &this->skelAnimeUpper)) {
        Player_SetUpperAction(play, this, Player_UpperAction_6);
    }
    return true;
}

s32 Player_UpperAction_CarryActor(Player* this, PlayState* play) {
    Actor* heldActor = this->heldActor;

    if (heldActor == NULL) {
        func_808309CC(play, this);
    }

    if (func_80830B88(play, this)) {
        return true;
    }

    if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        if (PlayerAnimation_Update(play, &this->skelAnimeUpper)) {
            PlayerAnimation_PlayLoop(play, &this->skelAnimeUpper, &gPlayerAnim_link_normal_carryB_wait);
        }

        if ((heldActor->id == ACTOR_EN_NIW) && (this->actor.velocity.y <= 0.0f)) {
            this->actor.terminalVelocity = -2.0f;
            this->actor.gravity = -0.5f;
            this->fallStartHeight = this->actor.world.pos.y;
        }
        return true;
    }
    return Player_UpperAction_0(this, play);
}

s32 Player_UpperAction_11(Player* this, PlayState* play) {
    if (func_80830B88(play, this)) {
        return true;
    }

    if (this->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN) {
        Player_SetUpperAction(play, this, Player_UpperAction_15);
    } else if (func_80831094(this, play)) {
        return true;
    }

    return false;
}

s32 Player_UpperAction_12(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnimeUpper)) {
        Player_SetUpperAction(play, this, Player_UpperAction_13);
        PlayerAnimation_PlayLoop(play, &this->skelAnimeUpper, &gPlayerAnim_pz_cutterwaitanim);
    }
    if (BEN_ANIM_EQUAL(this->skelAnimeUpper.animation, gPlayerAnim_pz_cutterwaitanim)) {
        func_80831010(this, play);
    }
    return true;
}

s32 Player_UpperAction_13(Player* this, PlayState* play) {
    PlayerAnimation_Update(play, &this->skelAnimeUpper);
    func_80831010(this, play);
    if (!sPlayerHeldItemButtonIsHeldDown) {
        Player_SetUpperAction(play, this, Player_UpperAction_14);
        PlayerAnimation_PlayOnce(play, &this->skelAnimeUpper, &gPlayerAnim_pz_cutterattack);
    }
    return true;
}

s32 Player_UpperAction_14(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnimeUpper)) {
        Player_SetUpperAction(play, this, Player_UpperAction_15);
        this->unk_ACC = 0;
    } else if (PlayerAnimation_OnFrame(&this->skelAnimeUpper, 6.0f)) {
        Vec3f pos;
        s16 untargetedRotY;

        func_80835BF8(&this->bodyPartsPos[PLAYER_BODYPART_LEFT_HAND], this->actor.shape.rot.y, 0.0f, &pos);
        pos.y = this->actor.world.pos.y + 50.0f;

        untargetedRotY = this->actor.shape.rot.y - 0x190;
        this->zoraBoomerangActor = Actor_Spawn(
            &play->actorCtx, play, ACTOR_EN_BOOM, pos.x, pos.y, pos.z, this->actor.focus.rot.x,
            (this->focusActor != NULL) ? this->actor.shape.rot.y + 0x36B0 : untargetedRotY, 0, ZORA_BOOMERANG_LEFT);

        if (this->zoraBoomerangActor != NULL) {
            EnBoom* leftZoraBoomerang = (EnBoom*)this->zoraBoomerangActor;
            EnBoom* rightZoraBoomerang;

            leftZoraBoomerang->moveTo = this->focusActor;
            if (leftZoraBoomerang->moveTo != NULL) {
                leftZoraBoomerang->unk_1CF = 0x10;
            }
            leftZoraBoomerang->unk_1CC = leftZoraBoomerang->unk_1CF + 0x24;

            func_80835BF8(&this->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND], this->actor.shape.rot.y, 0.0f, &pos);

            untargetedRotY = (this->actor.shape.rot.y + 0x190);
            rightZoraBoomerang =
                (EnBoom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOOM, pos.x, pos.y, pos.z, this->actor.focus.rot.x,
                                     (this->focusActor != NULL) ? this->actor.shape.rot.y - 0x36B0 : untargetedRotY, 0,
                                     ZORA_BOOMERANG_RIGHT);

            if (rightZoraBoomerang != NULL) {
                rightZoraBoomerang->moveTo = this->focusActor;
                if (rightZoraBoomerang->moveTo != NULL) {
                    rightZoraBoomerang->unk_1CF = 0x10;
                }

                rightZoraBoomerang->unk_1CC = rightZoraBoomerang->unk_1CF + 0x24;
                leftZoraBoomerang->actor.child = &rightZoraBoomerang->actor;
                rightZoraBoomerang->actor.parent = &leftZoraBoomerang->actor;
            }

            this->stateFlags1 |= PLAYER_STATE1_ZORA_BOOMERANG_THROWN;
            this->stateFlags3 &= ~PLAYER_STATE3_ZORA_BOOMERANG_CAUGHT;

            if (!Player_CheckHostileLockOn(this)) {
                Player_SetParallel(this);
            }

            this->unk_D57 = 20;

            Player_PlaySfx(this, NA_SE_IT_BOOMERANG_THROW);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
        }
    }

    return true;
}

s32 Player_UpperAction_15(Player* this, PlayState* play) {
    if (func_80830B88(play, this)) {
        return true;
    }

    if (this->stateFlags3 & PLAYER_STATE3_ZORA_BOOMERANG_CAUGHT) {
        Player_SetUpperAction(play, this, Player_UpperAction_16);
        PlayerAnimation_PlayOnce(play, &this->skelAnimeUpper, &gPlayerAnim_pz_cuttercatch);
        this->stateFlags3 &= ~PLAYER_STATE3_ZORA_BOOMERANG_CAUGHT;
        Player_PlaySfx(this, NA_SE_PL_CATCH_BOOMERANG);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
        return true;
    }

    return false;
}

s32 Player_UpperAction_16(Player* this, PlayState* play) {
    if (!Player_UpperAction_11(this, play) && PlayerAnimation_Update(play, &this->skelAnimeUpper)) {
        if (this->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN) {
            Player_SetUpperAction(play, this, Player_UpperAction_15);
            this->unk_ACC = 0;
        } else {
            Player_SetUpperAction(play, this, Player_UpperAction_0);
        }
    }
    return true;
}

void Player_Action_OwlSaveArrive(Player* this, PlayState* play) {
    PlayerAnimation_Update(play, &this->skelAnime);
    func_808323C0(this, play->playerCsIds[PLAYER_CS_ID_ITEM_BOTTLE]);

    if (DECR(this->av2.actionVar2) == 0) {
        if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
            Player_StopCutscene(this);
            Player_SetAction(play, this, Player_Action_Idle, 0);
            this->stateFlags1 &= ~PLAYER_STATE1_20000000;
        }
    } else if (this->av2.actionVar2 == 30) {
        if (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE) {
            this->av2.actionVar2++;
        } else {
            Message_StartTextbox(play, 0xC03, NULL);
        }
    }
}

void Player_Action_1(Player* this, PlayState* play) {
    this->stateFlags3 |= PLAYER_STATE3_10000000;
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_UpdateUpperBody(this, play);

    if (R_PLAY_FILL_SCREEN_ON == 0) {
        R_PLAY_FILL_SCREEN_ON = 20;
        R_PLAY_FILL_SCREEN_ALPHA = 0;
        R_PLAY_FILL_SCREEN_R = R_PLAY_FILL_SCREEN_G = R_PLAY_FILL_SCREEN_B = R_PLAY_FILL_SCREEN_ALPHA;
        Audio_PlaySfx(NA_SE_SY_DEKUNUTS_JUMP_FAILED);
    } else if (R_PLAY_FILL_SCREEN_ON > 0) {
        R_PLAY_FILL_SCREEN_ALPHA += R_PLAY_FILL_SCREEN_ON;
        if (R_PLAY_FILL_SCREEN_ALPHA > 255) {
            R_PLAY_FILL_SCREEN_ALPHA = 255;
            if (this->unk_B86[0] == 0) {
                this->unk_B86[0] = 1;
                func_8082DE50(play, this);
            } else {
                R_PLAY_FILL_SCREEN_ON = -20;
                this->stateFlags1 &= ~PLAYER_STATE1_8000000;
                this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
                Player_SetEquipmentData(play, this);
                this->prevBoots = this->currentBoots;

                if (this->unk_3CF != 0) {
                    Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_3C0);
                    this->actor.shape.rot.y = this->unk_3CC;
                } else {
                    Math_Vec3f_Copy(&this->actor.world.pos, &gSaveContext.respawn[RESPAWN_MODE_DOWN].pos);
                    this->actor.shape.rot.y = gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw;
                }

                Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
                this->speedXZ = 0.0f;
                this->yaw = this->actor.shape.rot.y;
                this->actor.velocity.y = 0.0f;
                Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));

                if ((play->roomCtx.curRoom.num == this->unk_3CE) && (play->roomCtx.prevRoom.num < 0)) {
                    this->av2.actionVar2 = 5;
                } else {
                    play->roomCtx.curRoom.num = -1;
                    play->roomCtx.prevRoom.num = -1;
                    play->roomCtx.curRoom.segment = NULL;
                    play->roomCtx.prevRoom.segment = NULL;

                    Room_FinishRoomChange(play, &play->roomCtx);
                    this->av2.actionVar2 = -1;
                    this->av1.actionVar1 = this->unk_3CE;
                }
            }
        }
    } else if (this->av2.actionVar2 < 0) {
        if (Room_RequestNewRoom(play, &play->roomCtx, this->av1.actionVar1)) {
            Map_InitRoomData(play, play->roomCtx.curRoom.num);
            Map_SetAreaEntrypoint(play);
            this->av2.actionVar2 = 5;
        }
    } else if (this->av2.actionVar2 > 0) {
        this->av2.actionVar2--;
    } else {
        R_PLAY_FILL_SCREEN_ALPHA += R_PLAY_FILL_SCREEN_ON;
        if (R_PLAY_FILL_SCREEN_ALPHA < 0) {
            R_PLAY_FILL_SCREEN_ALPHA = 0;
            R_PLAY_FILL_SCREEN_ON = 0;
            func_808339B4(this, -40);
            func_8085B384(this, play);
            this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
        }
    }
}

void Player_Action_2(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp_v0;

    if (this->av2.actionVar2 != 0) {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            Player_Anim_ResetMove(this);
            Player_Anim_PlayLoop(play, this, func_8082EF54(this));
            this->av2.actionVar2 = 0;
            this->stateFlags3 &= ~PLAYER_STATE3_8;
        }
        func_8082FC60(this);
    } else {
        func_8083E958(play, this);
    }

    Player_DecelerateToZero(this);

    if (Player_TryActionHandlerList(play, this, sActionHandlerList1, true)) {
        return;
    }

    if (!Player_UpdateHostileLockOn(this) &&
        (!Player_FriendlyLockOnOrParallel(this) || (Player_UpperAction_3 != this->upperActionFunc))) {
        func_8083B29C(this, play);
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

    temp_v0 = func_8083E404(this, speedTarget, yawTarget);
    if (temp_v0 > 0) {
        func_8083A844(this, play, yawTarget);
    } else if (temp_v0 < 0) {
        func_8083AF8C(this, yawTarget, play);
    } else if (speedTarget > 4.0f) {
        func_8083B030(this, play);
    } else {
        u32 temp_v0_2;

        func_8083EA44(this, this->speedXZ * 0.3f + 1.0f);
        func_8083E8E0(this, speedTarget, yawTarget);

        temp_v0_2 = this->unk_B38;
        if ((temp_v0_2 < 6) || ((temp_v0_2 - 0xE) < 6)) {
            Math_StepToF(&this->speedXZ, 0.0f, 1.5f);
        } else {
            s16 temp_v0_3 = yawTarget - this->yaw;
            s32 var_v1 = ABS_ALT(temp_v0_3);

            if (var_v1 > 0x4000) {
                if (Math_StepToF(&this->speedXZ, 0.0f, 1.5f)) {
                    this->yaw = yawTarget;
                }
            } else {
                Math_AsymStepToF(&this->speedXZ, speedTarget * 0.3f, 2.0f, 1.5f);
                Math_ScaledStepToS(&this->yaw, yawTarget, var_v1 * 0.1f);
            }
        }
    }
}

void Player_Action_3(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp_v0;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_ResetMove(this);
        Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
        this->stateFlags3 &= ~PLAYER_STATE3_8;
    }

    Player_DecelerateToZero(this);

    if (Player_TryActionHandlerList(play, this, sActionHandlerList2, true)) {
        return;
    }

    if (Player_UpdateHostileLockOn(this)) {
        func_8083B23C(this, play);
        return;
    }
    if (!Player_FriendlyLockOnOrParallel(this)) {
        Player_SetAction_PreserveMoveFlags(play, this, Player_Action_Idle, 1);
        this->yaw = this->actor.shape.rot.y;
        return;
    }
    if (Player_UpperAction_3 == this->upperActionFunc) {
        func_8083B23C(this, play);
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

    temp_v0 = func_8083E514(this, &speedTarget, &yawTarget, play);
    if (temp_v0 > 0) {
        func_8083A844(this, play, yawTarget);
    } else if (temp_v0 < 0) {
        func_8083AECC(this, yawTarget, play);
    } else if (speedTarget > 4.9f) {
        func_8083B030(this, play);
        func_8082FC60(this);
    } else if (speedTarget != 0.0f) {
        func_8083AF30(this, play);
    } else {
        s16 temp_v0_2 = yawTarget - this->actor.shape.rot.y;

        if (ABS_ALT(temp_v0_2) > 0x320) {
            Player_SetupTurnInPlace(play, this, yawTarget);
        }
    }
}

void Player_Action_Idle(Player* this, PlayState* play) {
    s32 idleAnimResult = Player_CheckForIdleAnim(this);
    s32 animDone = PlayerAnimation_Update(play, &this->skelAnime);
    f32 speedTarget;
    s16 yawTarget;
    s16 yawDiff;

    func_8083C85C(this);

    if (idleAnimResult > IDLE_ANIM_NONE) {
        Player_ProcessFidgetAnimSfxList(this, idleAnimResult - 1);
    }

    if (animDone ||
        ((this->currentMask == PLAYER_MASK_SCENTS) &&
         (!BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_cl_msbowait))) ||
        ((this->currentMask != PLAYER_MASK_SCENTS) &&
         (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_cl_msbowait)))) {
        if (this->av2.fallDamageStunTimer != 0) {
            if (DECR(this->av2.fallDamageStunTimer) == 0) {
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }

            // Offset model y position.
            // Depending on if the timer is even or odd, the offset will be 40 or -40 model space units.
            this->skelAnime.jointTable[LIMB_ROOT_POS].y =
                (this->skelAnime.jointTable[LIMB_ROOT_POS].y + ((this->av2.fallDamageStunTimer & 1) * 0x50)) - 0x28;
        } else {
            Player_Anim_ResetMove(this);
            Player_ChooseNextIdleAnim(play, this);
        }
        this->stateFlags3 &= ~PLAYER_STATE3_8;
    }

    Player_DecelerateToZero(this);

    if (this->av2.fallDamageStunTimer != 0) {
        return;
    }

    if (func_80847880(play, this)) {
        return;
    }

    if (Player_TryActionHandlerList(play, this, sActionHandlerListIdle, true)) {
        return;
    }

    if (Player_UpdateHostileLockOn(this)) {
        func_8083B23C(this, play);
        return;
    }

    if (Player_FriendlyLockOnOrParallel(this)) {
        func_8083692C(this, play);
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

    if (speedTarget != 0.0f) {
        func_8083A844(this, play, yawTarget);
        return;
    }

    yawDiff = yawTarget - this->actor.shape.rot.y;

    if (ABS_ALT(yawDiff) > 0x320) {
        Player_SetupTurnInPlace(play, this, yawTarget);
    } else {
        Math_ScaledStepToS(&this->actor.shape.rot.y, yawTarget, 0x4B0);
        this->yaw = this->actor.shape.rot.y;
        if (BEN_ANIM_EQUAL(Player_GetIdleAnim(this), this->skelAnime.animation)) {
            func_8083C6E8(this, play);
        }
    }
}

void Player_Action_5(Player* this, PlayState* play) {
    f32 var_fv0;
    s16 temp_v0_3;
    f32 speedTarget;
    s16 yawTarget;
    s32 var_v0;
    s32 temp_v0_2;
    s32 var_v1;
    f32 var_fv1;

    this->skelAnime.mode = ANIMMODE_LOOP;
    PlayerAnimation_SetUpdateFunction(&this->skelAnime);

    this->skelAnime.animation = func_8082EFE4(this);
    if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_bow_side_walk)) {
        var_fv0 = 24.0f;
        var_fv1 = -(MREG(95) / 100.0f);
    } else {
        var_fv0 = 29.0f;
        var_fv1 = MREG(95) / 100.0f;
    }

    this->skelAnime.animLength = var_fv0;
    this->skelAnime.endFrame = var_fv0 - 1.0f;
    if (BINANG_SUB(this->yaw, this->actor.shape.rot.y) >= 0) {
        var_v0 = 1;
    } else {
        var_v0 = -1;
    }

    this->skelAnime.playSpeed = var_v0 * (this->speedXZ * var_fv1);

    PlayerAnimation_Update(play, &this->skelAnime);
    if (PlayerAnimation_OnFrame(&this->skelAnime, 0.0f) || PlayerAnimation_OnFrame(&this->skelAnime, var_fv0 / 2.0f)) {
        Player_AnimSfx_PlayFloorWalk(this, this->speedXZ);
    }

    if (Player_TryActionHandlerList(play, this, sActionHandlerList3, true)) {
        return;
    }

    if (Player_UpdateHostileLockOn(this)) {
        func_8083B23C(this, play);
        return;
    }
    if (!Player_FriendlyLockOnOrParallel(this)) {
        func_8085B384(this, play);
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

    temp_v0_2 = func_8083E514(this, &speedTarget, &yawTarget, play);
    if (temp_v0_2 > 0) {
        func_8083A844(this, play, yawTarget);
        return;
    }
    if (temp_v0_2 < 0) {
        func_8083AECC(this, yawTarget, play);
        return;
    }
    if (speedTarget > 4.9f) {
        func_8083B030(this, play);
        func_8082FC60(this);
        return;
    }
    if ((speedTarget == 0.0f) && (this->speedXZ == 0.0f)) {
        func_8083692C(this, play);
        return;
    }

    temp_v0_3 = yawTarget - this->yaw;
    var_v1 = ABS_ALT(temp_v0_3);
    if (var_v1 > 0x4000) {
        if (Math_StepToF(&this->speedXZ, 0.0f, 1.5f)) {
            this->yaw = yawTarget;
        }
    } else {
        Math_AsymStepToF(&this->speedXZ, speedTarget * 0.4f, 1.5f, 1.5f);
        Math_ScaledStepToS(&this->yaw, yawTarget, var_v1 * 0.1f);
    }
}

void Player_Action_6(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 sp2C;

    func_8083EE60(this, play);
    if (Player_TryActionHandlerList(play, this, sActionHandlerList4, true)) {
        return;
    }

    if (!Player_IsZTargetingWithHostileUpdate(this)) {
        func_8083A844(this, play, this->yaw);
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

    sp2C = func_8083E514(this, &speedTarget, &yawTarget, play);
    if (sp2C >= 0) {
        if (!func_8083F190(this, &speedTarget, &yawTarget, play)) {
            if (sp2C != 0) {
                func_8083A794(this, play);
            } else if (speedTarget > 4.9f) {
                func_8083B030(this, play);
            } else {
                func_8083AF30(this, play);
            }
        }
    } else {
        s16 sp2A = yawTarget - this->yaw;

        Math_AsymStepToF(&this->speedXZ, speedTarget * 1.5f, 1.5f, 2.0f);
        Math_ScaledStepToS(&this->yaw, yawTarget, sp2A * 0.1f);
        if ((speedTarget == 0.0f) && (this->speedXZ == 0.0f)) {
            func_8083692C(this, play);
        }
    }
}

void Player_Action_7(Player* this, PlayState* play) {
    s32 animFinished = PlayerAnimation_Update(play, &this->skelAnime);
    f32 speedTarget;
    s16 yawTarget;

    Player_DecelerateToZero(this);

    if (Player_TryActionHandlerList(play, this, sActionHandlerList4, true)) {
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

    if (this->speedXZ != 0.0f) {
        return;
    }

    this->yaw = this->actor.shape.rot.y;
    if (func_8083E514(this, &speedTarget, &yawTarget, play) > 0) {
        func_8083A794(this, play);
    } else if ((speedTarget != 0.0f) || animFinished) {
        func_8083F230(this, play);
    }
}

void Player_Action_8(Player* this, PlayState* play) {
    s32 animFinished = PlayerAnimation_Update(play, &this->skelAnime);

    if (Player_TryActionHandlerList(play, this, sActionHandlerList4, true)) {
        return;
    }

    if (animFinished) {
        func_8083692C(this, play);
    }
}

void Player_Action_9(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 var_v0;

    func_8083F27C(play, this);
    if (Player_TryActionHandlerList(play, this, sActionHandlerList5, true)) {
        return;
    }

    if (!Player_IsZTargetingWithHostileUpdate(this)) {
        func_8083A794(this, play);
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

    if (Player_FriendlyLockOnOrParallel(this)) {
        var_v0 = func_8083E514(this, &speedTarget, &yawTarget, play);
    } else {
        var_v0 = func_8083E404(this, speedTarget, yawTarget);
    }

    if (var_v0 > 0) {
        func_8083A794(this, play);
    } else if (var_v0 < 0) {
        if (Player_FriendlyLockOnOrParallel(this)) {
            func_8083AECC(this, yawTarget, play);
        } else {
            func_8083AF8C(this, yawTarget, play);
        }
    } else if ((this->speedXZ < 3.6f) && (speedTarget < 4.0f)) {
        if (!Player_CheckHostileLockOn(this) && Player_FriendlyLockOnOrParallel(this)) {
            func_8083AF30(this, play);
        } else {
            func_80836988(this, play);
        }
    } else {
        s16 temp_v0;
        s32 var_v1;
        s32 pad;

        func_8083E8E0(this, speedTarget, yawTarget);

        temp_v0 = yawTarget - this->yaw;
        var_v1 = ABS_ALT(temp_v0);
        if (var_v1 > 0x4000) {
            if (Math_StepToF(&this->speedXZ, 0.0f, 3.0f)) {
                this->yaw = yawTarget;
            }
        } else {
            speedTarget *= 0.9f;
            Math_AsymStepToF(&this->speedXZ, speedTarget, 2.0f, 3.0f);
            Math_ScaledStepToS(&this->yaw, yawTarget, var_v1 * 0.1f);
        }
    }
}

/**
 * Turn in place until the angle pointed to by the control stick is reached.
 *
 * This is the state that the speedrunning community refers to as "ESS" or "ESS Position".
 * See the bug comment below and https://www.zeldaspeedruns.com/mm/tech/ess-and-hess
 * for more information.
 */
void Player_Action_TurnInPlace(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    PlayerAnimation_Update(play, &this->skelAnime);
    if (Player_IsHoldingTwoHandedWeapon(this)) {
        AnimTaskQueue_AddLoadPlayerFrame(play, Player_GetIdleAnim(this), 0, this->skelAnime.limbCount,
                                         this->skelAnime.morphTable);
        AnimTaskQueue_AddCopyUsingMap(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                      this->skelAnime.morphTable, sPlayerUpperBodyLimbCopyMap);
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

    //! @bug This action does not handle xzSpeed in any capacity.
    //! Player's current speed value will be maintained the entire time this action is running.
    //! This is the core bug that allows many different glitches to manifest.
    //!
    //! One possible fix is to kill all speed instantly in `Player_SetupTurnInPlace`.
    //! Another possible fix is to gradually kill speed by calling `Player_DecelerateToZero`
    //! here, which plenty of other "standing" actions do.

    if ((this != GET_PLAYER(play)) && (this->focusActor == NULL)) {
        yawTarget = this->actor.home.rot.y;
    }

    if (Player_TryActionHandlerList(play, this, sActionHandlerListTurnInPlace, true)) {
        return;
    }

    if (speedTarget != 0.0f) {
        this->actor.shape.rot.y = yawTarget;
        func_8083A794(this, play);
    } else if (Math_ScaledStepToS(&this->actor.shape.rot.y, yawTarget, this->turnRate)) {
        func_80839E74(this, play);
    }
    this->yaw = this->actor.shape.rot.y;
}

void Player_Action_11(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20;

    if (this->speedXZ < 1.0f) {
        this->skelAnime.animation = &gPlayerAnim_clink_normal_okarina_walk;
    } else {
        this->skelAnime.animation = &gPlayerAnim_clink_normal_okarina_walkB;
    }
    PlayerAnimation_Update(play, &this->skelAnime);

    if (!func_80847880(play, this) && (!Player_TryActionHandlerList(play, this, sActionHandlerListIdle, true) ||
                                       (Player_Action_11 == this->actionFunc))) {
        f32 speedTarget;
        f32 temp_fv0;
        f32 temp_fv1;
        s16 yawTarget;
        s16 sp30;

        if (!CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_B)) {
            func_80839E74(this, play);
            return;
        }

        this->speedXZ = this->unk_B48;
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);
        sp30 = yawTarget;

        if (!func_8083A4A4(this, &speedTarget, &yawTarget, R_DECELERATE_RATE / 100.0f)) {
            func_8083CB04(this, speedTarget, yawTarget, REG(19) / 100.0f, 1.5f, 0x3E8);
            func_8083C8E8(this, play);
            if ((this->speedXZ == 0.0f) && (speedTarget == 0.0f)) {
                this->yaw = sp30;
                this->actor.shape.rot.y = this->yaw;
            }
        }

        this->unk_B48 = this->speedXZ;
        temp_fv0 = this->skelAnime.curFrame + 5.0f;
        temp_fv1 = this->skelAnime.animLength / 2.0f;

        // effectively an fmodf
        temp_fv0 -= temp_fv1 * (s32)(temp_fv0 / temp_fv1);
        this->speedXZ *= Math_CosS(temp_fv0 * 1000.0f) * 0.4f;
    }
}

void Player_Action_12(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20;
    PlayerAnimation_Update(play, &this->skelAnime);

    Player_DecelerateToZero(this);

    if (!func_80847880(play, this)) {
        if (!Player_TryActionHandlerList(play, this, sActionHandlerListIdle, false) ||
            (Player_Action_12 == this->actionFunc)) {
            if (!CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_B)) {
                func_80839E74(this, play);
            }
        }
    }
}

void Player_Action_13(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_20;
    func_8083F57C(this, play);
    if (Player_TryActionHandlerList(play, this, sActionHandlerList8, true)) {
        return;
    }

    if (Player_IsZTargetingWithHostileUpdate(this)) {
        func_8083A794(this, play);
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

    if (GameInteractor_Should(VB_CONSIDER_BUNNY_HOOD_EQUIPPED, this->currentMask == PLAYER_MASK_BUNNY, this)) {
        speedTarget *= 1.5f;
    }

    if (!func_8083A4A4(this, &speedTarget, &yawTarget, R_DECELERATE_RATE / 100.0f)) {

        GameInteractor_Should(VB_SPEED_MODIFIER_WALK, true, &speedTarget);

        func_8083CB58(this, speedTarget, yawTarget);
        func_8083C8E8(this, play);
        if ((this->speedXZ == 0.0f) && (speedTarget == 0.0f)) {
            func_80839E3C(this, play);
        }
    }
}

void Player_Action_14(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_20;

    func_8083F57C(this, play);
    if (Player_TryActionHandlerList(play, this, sActionHandlerList9, true)) {
        return;
    }

    if (!Player_IsZTargetingWithHostileUpdate(this)) {
        func_8083A794(this, play);
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

    if (!func_8083A4A4(this, &speedTarget, &yawTarget, R_DECELERATE_RATE / 100.0f)) {
        if ((Player_FriendlyLockOnOrParallel(this) && (speedTarget != 0) &&
             (func_8083E514(this, &speedTarget, &yawTarget, play) <= 0)) ||
            (!Player_FriendlyLockOnOrParallel(this) && (func_8083E404(this, speedTarget, yawTarget) <= 0))) {
            func_80836988(this, play);
        } else {
            func_8083CB58(this, speedTarget, yawTarget);
            func_8083C8E8(this, play);
            if ((this->speedXZ == 0.0f) && (speedTarget == 0.0f)) {
                func_80836988(this, play);
            }
        }
    }
}

void Player_Action_15(Player* this, PlayState* play) {
    s32 animFinished = PlayerAnimation_Update(play, &this->skelAnime);
    f32 speedTarget;
    s16 yawTarget;

    if (Player_TryActionHandlerList(play, this, sActionHandlerList5, true)) {
        return;
    }

    if (!Player_IsZTargetingWithHostileUpdate(this)) {
        func_8083A794(this, play);
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

    if ((this->skelAnime.morphWeight == 0.0f) && (this->skelAnime.curFrame > 5.0f)) {
        Player_DecelerateToZero(this);

        if ((this->skelAnime.curFrame > 10.0f) && (func_8083E404(this, speedTarget, yawTarget) < 0)) {
            func_8083AF8C(this, yawTarget, play);
        } else if (animFinished) {
            func_8083B090(this, play);
        }
    }
}

void Player_Action_16(Player* this, PlayState* play) {
    s32 animFinished = PlayerAnimation_Update(play, &this->skelAnime);
    f32 speedTarget;
    s16 yawTarget;

    Player_DecelerateToZero(this);

    if (Player_TryActionHandlerList(play, this, sActionHandlerList10, true)) {
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

    if (this->speedXZ == 0.0f) {
        this->yaw = this->actor.shape.rot.y;
        if (func_8083E404(this, speedTarget, yawTarget) > 0) {
            func_8083A794(this, play);
        } else if ((speedTarget != 0.0f) || animFinished) {
            func_80836988(this, play);
        }
    }
}

void Player_Action_17(Player* this, PlayState* play) {
    if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_backspace)) {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            Player_Anim_ResetMove(this);
            Player_Anim_PlayOnceMorph(play, this, D_8085BE84[PLAYER_ANIMGROUP_check][this->modelAnimType]);
        }
    } else {
        Player_Anim_PlayLoopOnceFinished(play, this, D_8085BE84[PLAYER_ANIMGROUP_check_wait][this->modelAnimType]);
    }

    if (DECR(this->av2.actionVar2) == 0) {
        if (!Player_ActionHandler_13(this, play)) {
            func_80836A98(this, D_8085BE84[PLAYER_ANIMGROUP_check_end][this->modelAnimType], play);
        }
        this->actor.flags &= ~ACTOR_FLAG_TALK;
        Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
    }
}

// Player_Action_Shielding
void Player_Action_18(Player* this, PlayState* play) {
    Player_DecelerateToZero(this);

    if (this->transformation == PLAYER_FORM_GORON) {
        SkelAnime_Update(&this->unk_2C8);

        if (!func_8083FE38(this, play)) {
            if (!Player_ActionHandler_11(this, play)) {
                this->stateFlags1 &= ~PLAYER_STATE1_400000;

                if (this->itemAction <= PLAYER_IA_MINUS1) {
                    func_80123C58(this);
                }

                func_80836A98(this, D_8085BE84[PLAYER_ANIMGROUP_defense_end][this->modelAnimType], play);
                func_80830B38(this);
            } else {
                this->stateFlags1 |= PLAYER_STATE1_400000;
            }
        }

        return;
    }

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (!Player_IsGoronOrDeku(this)) {
            Player_Anim_PlayLoop(play, this, D_8085BE84[PLAYER_ANIMGROUP_defense_wait][this->modelAnimType]);
        }

        this->av2.actionVar2 = 1;
        this->av1.actionVar1 = 0;
    }

    if (!Player_IsGoronOrDeku(this)) {
        this->stateFlags1 |= PLAYER_STATE1_400000;
        Player_UpdateUpperBody(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_400000;
        if (this->transformation == PLAYER_FORM_ZORA) {
            func_8082F164(this, BTN_R | BTN_B);
        }
    }

    if (this->av2.actionVar2 != 0) {
        f32 yStick = sPlayerControlInput->rel.stick_y * 180;
        f32 xStick = sPlayerControlInput->rel.stick_x * -120;
        s16 temp_a0 = this->actor.shape.rot.y - Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
        s16 var_a1;
        s16 temp_ft5;
        s16 var_a2;
        s16 var_a3;

        xStick *= GameInteractor_InvertControl(GI_INVERT_SHIELD_X);
        yStick *= GameInteractor_InvertControl(GI_INVERT_SHIELD_Y);
        var_a1 = (yStick * Math_CosS(temp_a0)) + (Math_SinS(temp_a0) * xStick);
        temp_ft5 = (xStick * Math_CosS(temp_a0)) - (Math_SinS(temp_a0) * yStick);

        var_a1 = CLAMP_MAX(var_a1, 0xDAC);
        var_a2 = ABS_ALT(var_a1 - this->actor.focus.rot.x) * 0.25f;
        var_a2 = CLAMP_MIN(var_a2, 0x64);

        var_a3 = ABS_ALT(temp_ft5 - this->upperLimbRot.y) * 0.25f;
        var_a3 = CLAMP_MIN(var_a3, 0x32);
        Math_ScaledStepToS(&this->actor.focus.rot.x, var_a1, var_a2);

        this->upperLimbRot.x = this->actor.focus.rot.x;
        Math_ScaledStepToS(&this->upperLimbRot.y, temp_ft5, var_a3);

        if (this->av1.actionVar1 != 0) {
            if (!func_808401F4(play, this)) {
                if (this->skelAnime.curFrame < 2.0f) {
                    func_8082FA5C(play, this, PLAYER_MELEE_WEAPON_STATE_1);
                }
            } else {
                this->av2.actionVar2 = 1;
                this->av1.actionVar1 = 0;
            }
        } else if (!func_8083FE38(this, play)) {
            if (Player_ActionHandler_11(this, play)) {
                func_8083FD80(this, play);
            } else {
                this->stateFlags1 &= ~PLAYER_STATE1_400000;
                func_8082DC38(this);

                if (Player_IsGoronOrDeku(this)) {
                    func_80836A5C(this, play);
                    PlayerAnimation_Change(play, &this->skelAnime, this->skelAnime.animation, PLAYER_ANIM_NORMAL_SPEED,
                                           Animation_GetLastFrame(this->skelAnime.animation), 0.0f, 2, 0.0f);
                } else {
                    if (this->itemAction <= PLAYER_IA_MINUS1) {
                        func_80123C58(this);
                    }

                    func_80836A98(this, D_8085BE84[PLAYER_ANIMGROUP_defense_end][this->modelAnimType], play);
                }

                Player_PlaySfx(this, NA_SE_IT_SHIELD_REMOVE);
                return;
            }
        } else {
            return;
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_400000;
    Player_SetModelsForHoldingShield(this);
    this->unk_AA6_rotFlags |= UNKAA6_ROT_FOCUS_X | UNKAA6_ROT_UPPER_X | UNKAA6_ROT_UPPER_Y;
}

void Player_Action_19(Player* this, PlayState* play) {
    Player_DecelerateToZero(this);

    if (this->av1.actionVar1 == 0) {
        sUpperBodyIsBusy = Player_UpdateUpperBody(this, play);
        if ((Player_UpperAction_3 == this->upperActionFunc) ||
            (Player_TryActionInterrupt(play, this, &this->skelAnimeUpper, 4.0f) >= PLAYER_INTERRUPT_MOVE)) {
            Player_SetAction(play, this, Player_Action_2, 1);
        }
    } else {
        PlayerActionInterruptResult interruptResult;

        this->stateFlags1 |= PLAYER_STATE1_400000;

        interruptResult = Player_TryActionInterrupt(play, this, &this->skelAnime, 4.0f);

        if ((interruptResult != PLAYER_INTERRUPT_NEW_ACTION) &&
            ((interruptResult >= PLAYER_INTERRUPT_MOVE) || PlayerAnimation_Update(play, &this->skelAnime))) {
            PlayerAnimationHeader* anim;
            f32 endFrame;

            Player_SetAction(play, this, Player_Action_18, 1);
            Player_SetModelsForHoldingShield(this);
            anim = D_8085BE84[PLAYER_ANIMGROUP_defense][this->modelAnimType];
            endFrame = Animation_GetLastFrame(anim);
            PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, endFrame, endFrame,
                                   ANIMMODE_ONCE, 0.0f);
        }
    }
}

void Player_Action_20(Player* this, PlayState* play) {
    PlayerActionInterruptResult interruptResult;

    Player_DecelerateToZero(this);

    interruptResult = Player_TryActionInterrupt(play, this, &this->skelAnime, 16.0f);

    if (interruptResult != PLAYER_INTERRUPT_NEW_ACTION) {
        if (PlayerAnimation_Update(play, &this->skelAnime) || (interruptResult >= PLAYER_INTERRUPT_MOVE)) {
            func_80836988(this, play);
        }
    }
}

void Player_Action_21(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20 | PLAYER_STATE2_40;
    func_808345A8(this);

    if (!(this->stateFlags1 & PLAYER_STATE1_20000000) && (this->av2.actionVar2 == 0) && (this->unk_B75 != 0)) {
        s16 temp_v0 = this->unk_B76;
        s16 temp_v1 = this->actor.shape.rot.y - temp_v0;

        this->actor.shape.rot.y = temp_v0;
        this->yaw = temp_v0;
        this->speedXZ = this->unk_B78;

        if (ABS_ALT(temp_v1) > 0x4000) {
            this->actor.shape.rot.y = temp_v0 + 0x8000;
        }

        if (this->actor.velocity.y < 0.0f) {
            this->actor.gravity = 0.0f;
            this->actor.velocity.y = 0.0f;
        }
    }

    if (PlayerAnimation_Update(play, &this->skelAnime) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->av2.actionVar2 != 0) {
            this->av2.actionVar2--;
            if (this->av2.actionVar2 == 0) {
                func_8085B384(this, play);
            }
        } else if ((this->stateFlags1 & PLAYER_STATE1_20000000) ||
                   (!(this->cylinder.base.acFlags & AC_HIT) && (this->unk_B75 == 0))) {
            if (this->stateFlags1 & PLAYER_STATE1_20000000) {
                this->av2.actionVar2++;
            } else {
                Player_SetAction(play, this, Player_Action_22, 0);
                this->stateFlags1 |= PLAYER_STATE1_4000000;
            }

            Player_Anim_PlayOnce(play, this,
                                 (this->yaw != this->actor.shape.rot.y) ? &gPlayerAnim_link_normal_front_downB
                                                                        : &gPlayerAnim_link_normal_back_downB);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FREEZE);
        }
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Player_AnimSfx_PlayFloor(this, NA_SE_PL_BOUND);
    }
}

void Player_Action_22(Player* this, PlayState* play) {
    this->stateFlags2 |= (PLAYER_STATE2_20 | PLAYER_STATE2_40);
    func_808345A8(this);

    Player_DecelerateToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime) && (this->speedXZ == 0.0f)) {
        if (this->stateFlags1 & PLAYER_STATE1_20000000) {
            this->av2.actionVar2++;
        } else {
            Player_SetAction(play, this, Player_Action_23, 0);
            this->stateFlags1 |= PLAYER_STATE1_4000000;
        }

        Player_Anim_PlayOnceAdjusted(play, this,
                                     (this->yaw != this->actor.shape.rot.y) ? &gPlayerAnim_link_normal_front_down_wake
                                                                            : &gPlayerAnim_link_normal_back_down_wake);
        this->yaw = this->actor.shape.rot.y;
    }
}

AnimSfxEntry D_8085D604[] = {
    ANIMSFX(ANIMSFX_TYPE_8, 20, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_8, 30, NA_SE_NONE, STOP),
};

void Player_Action_23(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20;

    func_808345A8(this);
    if (this->stateFlags1 & PLAYER_STATE1_20000000) {
        PlayerAnimation_Update(play, &this->skelAnime);
    } else {
        PlayerActionInterruptResult interruptResult = Player_TryActionInterrupt(play, this, &this->skelAnime, 16.0f);

        if (interruptResult != PLAYER_INTERRUPT_NEW_ACTION) {
            if (PlayerAnimation_Update(play, &this->skelAnime) || (interruptResult >= PLAYER_INTERRUPT_MOVE)) {
                func_80836988(this, play);
            }
        }
    }

    Player_PlayAnimSfx(this, D_8085D604);
}

AnimSfxEntry D_8085D60C[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 60, NA_SE_PL_BOUND, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_8, 140, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_8, 164, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_8, 170, NA_SE_NONE, STOP),
};

void Player_Action_24(Player* this, PlayState* play) {
    if ((this->transformation != PLAYER_FORM_GORON) && (this->actor.depthInWater <= 0.0f)) {
        if ((play->roomCtx.curRoom.environmentType == ROOM_ENV_HOT) || (sPlayerFloorType == FLOOR_TYPE_9) ||
            ((func_808340AC(sPlayerFloorType) >= 0) &&
             !SurfaceType_IsWallDamage(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            func_808344C0(play, this);
        }
    }

    Player_DecelerateToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this == GET_PLAYER(play)) {
            func_80840770(play, this);
        }
    } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_derth_rebirth)) {
        Player_PlayAnimSfx(this, D_8085D60C);
    } else if ((BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_electric_shock_end)) &&
               PlayerAnimation_OnFrame(&this->skelAnime, 88.0f)) {
        Player_AnimSfx_PlayFloor(this, NA_SE_PL_BOUND);
    }
}

s32 func_8084C124(PlayState* play, Player* this) {
    if (func_80837730(play, this, 3.0f, 500)) {
        Player_PlaySfx(this, NA_SE_EV_DIVE_INTO_WATER);
        return true;
    }
    return false;
}

void Player_Action_25(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    Actor* heldActor;

    if (Player_CheckHostileLockOn(this)) {
        this->actor.gravity = -1.2f;
    }

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
            heldActor = this->heldActor;
            if (!func_808313A8(play, this, heldActor) && (heldActor->id == ACTOR_EN_NIW) &&
                CHECK_BTN_ANY(sPlayerControlInput->press.button,
                              BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_B | BTN_A | BTN_DPAD_EQUIP)) {
                func_808409A8(play, this, this->speedXZ + 2.0f, this->actor.velocity.y + 2.0f);
            }
        }

        PlayerAnimation_Update(play, &this->skelAnime);
        if ((BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_newroll_jump_20f)) &&
            PlayerAnimation_OnFrame(&this->skelAnime, 4.0f)) {
            Player_PlaySfx(this, NA_SE_PL_ROLL);
        }

        if (this->transformation == PLAYER_FORM_DEKU) {
            s16 prevYaw = this->yaw;

            func_808378FC(play, this);
            if (GameInteractor_Should(VB_APPLY_AIR_CONTROL, true, &speedTarget)) {
                func_8083CBC4(this, speedTarget * 0.5f, yawTarget, 2.0f, 0.2f, 0.1f, 0x190);
            }

            if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pn_attack)) {
                this->stateFlags2 |= (PLAYER_STATE2_20 | PLAYER_STATE2_40);

                this->unk_B10[0] += -800.0f;
                this->actor.shape.rot.y += BINANG_ADD(TRUNCF_BINANG(this->unk_B10[0]), BINANG_SUB(this->yaw, prevYaw));
                Math_StepToF(&this->unk_B10[1], 0.0f, this->unk_B10[0]);
            }
        } else {
            if (GameInteractor_Should(VB_APPLY_AIR_CONTROL, true, &speedTarget)) {
                func_8083CBC4(this, speedTarget, yawTarget, 1.0f, 0.05f, 0.1f, 0xC8);
            }
        }

        Player_UpdateUpperBody(this, play);
        if ((((this->stateFlags2 & PLAYER_STATE2_80000) &&
              ((this->av1.actionVar1 == 2) || (this->av1.actionVar1 >= 4))) ||
             !func_80839770(this, play)) &&
            (this->actor.velocity.y < 0.0f)) {
            if (this->av2.actionVar2 >= 0) {
                if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->av2.actionVar2 == 0) ||
                    (this->fallDistance > 0)) {
                    if ((sPlayerYDistToFloor > 800.0f) || (this->stateFlags3 & PLAYER_STATE3_10000)) {
                        func_80840980(this, NA_SE_VO_LI_FALL_S);
                        this->stateFlags3 &= ~PLAYER_STATE3_10000;
                    }
                    PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_normal_landing,
                                           PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f, ANIMMODE_ONCE, 8.0f);
                    this->av2.actionVar2 = -1;
                }
            } else {
                if ((this->av2.actionVar2 == -1) && (this->fallDistance > 120) && (sPlayerYDistToFloor > 280.0f)) {
                    this->av2.actionVar2 = -2;
                    func_80840980(this, NA_SE_VO_LI_FALL_L);
                }

                if ((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
                    !(this->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_8000000)) &&
                    (this->speedXZ > 0.0f)) {
                    if ((this->transformation != PLAYER_FORM_GORON) &&
                        ((this->transformation != PLAYER_FORM_DEKU) || (this->remainingHopsCounter != 0))) {
                        if ((this->yDistToLedge >= 150.0f) &&
                            (this->controlStickDirections[this->controlStickDataIndex] == PLAYER_STICK_DIR_FORWARD)) {
                            if (func_8083D860(this, play)) {
                                func_8084C124(play, this);
                            }
                        } else if ((this->ledgeClimbType >= PLAYER_LEDGE_CLIMB_2) &&
                                   ((this->yDistToLedge < (150.0f * this->ageProperties->unk_08)) &&
                                    (((this->actor.world.pos.y - this->actor.floorHeight) + this->yDistToLedge)) >
                                        (70.0f * this->ageProperties->unk_08))) {
                            AnimTaskQueue_DisableTransformTasksForGroup(play);
                            if (this->stateFlags3 & PLAYER_STATE3_10000) {
                                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HOOKSHOT_HANG);
                            } else {
                                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HANG);
                            }

                            this->actor.world.pos.y += this->yDistToLedge;
                            func_80837CEC(play, this, this->actor.wallPoly, this->distToInteractWall,
                                          GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_hold, this->modelAnimType));
                            this->yaw += 0x8000;
                            this->actor.shape.rot.y = this->yaw;
                            this->stateFlags1 |= PLAYER_STATE1_2000;

                            func_8084C124(play, this);
                        }
                    }
                }
            }
        }
    } else {
        func_80837134(play, this);
        Player_UpdateUpperBody(this, play);
    }

    Player_ActionHandler_13(this, play);
}

// sPlayerRollingAnimSfx
AnimSfxEntry D_8085D61C[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 1, NA_SE_VO_LI_SWORD_N, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_BY_AGE, 6, NA_SE_PL_WALK_GROUND, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 6, NA_SE_PL_ROLL, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 18, NA_SE_NONE, STOP),
};

// Player_Action_Rolling // Handles bonking too?
void Player_Action_26(Player* this, PlayState* play) {
    s32 animFinished;

    this->stateFlags2 |= PLAYER_STATE2_20;
    this->stateFlags3 |= PLAYER_STATE3_8000000;

    animFinished = PlayerAnimation_Update(play, &this->skelAnime);
    if (PlayerAnimation_OnFrame(&this->skelAnime, 8.0f)) {
        func_808339B4(this, -10);
    }

    if (this->skelAnime.curFrame >= 8.0f) {
        if (this->skelAnime.curFrame < 18.0f) {
            Player_SetCylinderForAttack(this, DMG_NORMAL_ROLL, 1, 12);
        } else {
            Player_ResetCylinder(this);
        }
    }

    if (func_8083FE38(this, play)) {
        return;
    }

    if (this->av2.actionVar2 != 0) {
        PlayerActionInterruptResult interruptResult;

        Math_StepToF(&this->speedXZ, 0.0f, 2.0f);

        interruptResult = Player_TryActionInterrupt(play, this, &this->skelAnime, 5.0f);

        if (interruptResult != PLAYER_INTERRUPT_NEW_ACTION) {
            if ((interruptResult >= PLAYER_INTERRUPT_MOVE) || animFinished) {
                func_80836A5C(this, play);
            }
        }
    } else if (!func_80840A30(play, this, &this->speedXZ, 6.0f)) {
        if ((this->skelAnime.curFrame < 15.0f) || !Player_ActionHandler_7(this, play)) {
            f32 speedTarget;
            s16 yawTarget;

            if (this->skelAnime.curFrame >= 20.0f) {
                func_80836A5C(this, play);
                return;
            }

            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);
            speedTarget *= 1.5f;

            if ((speedTarget < 3.0f) ||
                (this->controlStickDirections[this->controlStickDataIndex] != PLAYER_STICK_DIR_FORWARD)) {
                speedTarget = 3.0f;
            }
            func_8083CB58(this, speedTarget, this->actor.shape.rot.y);

            if (func_8083FBC4(play, this)) {
                Actor_PlaySfx_Flagged2(&this->actor, (this->floorSfxOffset == NA_SE_PL_WALK_SNOW - SFX_FLAG)
                                                         ? NA_SE_PL_ROLL_SNOW_DUST - SFX_FLAG
                                                         : NA_SE_PL_ROLL_DUST - SFX_FLAG);
            }

            Player_PlayAnimSfx(this, D_8085D61C);
        }
    }
}

void Player_Action_27(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_run_jump_water_fall_wait);
    }

    Math_StepToF(&this->speedXZ, 0.0f, 0.05f);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->fallDistance >= 400) {
            this->actor.colChkInfo.damage = 0x10;
            func_80833B18(play, this, 1, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
        } else {
            func_80836B3C(play, this, 4.0f);
        }
    }
}

void Player_Action_28(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_pz_fishswim);
    }

    Math_SmoothStepToS(&this->unk_B86[1], 0, 6, 0x7D0, 0x190);
    if (!func_80840A30(play, this, &this->speedXZ, 0.0f)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            if (this->unk_AAA > 0x36B0) {
                this->actor.colChkInfo.damage = 0x10;
                func_80833B18(play, this, 1, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
            } else {
                func_80836B3C(play, this, 4.0f);
            }
        } else {
            this->actor.gravity = -1.0f;
            this->unk_AAA = Math_Atan2S_XY(this->actor.speed, -this->actor.velocity.y);
            func_8082F164(this, BTN_R);
        }
    }
}

void Player_Action_29(Player* this, PlayState* play) {
    AttackAnimInfo* attackInfoEntry = &sMeleeAttackAnimInfo[this->meleeWeaponAnimation];
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_20;

    if (this->transformation == PLAYER_FORM_ZORA) {
        this->actor.gravity = -0.8f;
    } else {
        this->actor.gravity = -1.2f;
    }

    PlayerAnimation_Update(play, &this->skelAnime);

    if (!func_808401F4(play, this)) {
        func_8083FCF0(play, this, 6.0f, attackInfoEntry->unk_C, attackInfoEntry->unk_D);
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
            func_8083CBC4(this, speedTarget, this->yaw, 1.0f, 0.05f, 0.1f, 200);
        } else if (func_80836F10(play, this) >= 0) { // Player didn't die because of this fall
            this->meleeWeaponAnimation += 3;
            func_80833864(play, this, this->meleeWeaponAnimation);
            this->unk_ADD = 3;
            this->meleeWeaponState = PLAYER_MELEE_WEAPON_STATE_0;
            Player_AnimSfx_PlayFloorLand(this);
        }
    }
}

void Player_Action_30(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp_v0;

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_ResetMove(this);
        Player_SetParallel(this);
        this->stateFlags1 &= ~PLAYER_STATE1_PARALLEL;
        Player_Anim_PlayLoop(play, this, D_8085CF60[Player_IsHoldingTwoHandedWeapon(this)]);
        this->av2.actionVar2 = -1;
    }

    Player_DecelerateToZero(this);

    if (!func_8083FE38(this, play) && (this->av2.actionVar2 != 0)) {
        func_80840F34(this);
        if (this->av2.actionVar2 < 0) {
            if (this->unk_B08 >= 0.1f) {
                this->unk_ADD = 0;
                this->av2.actionVar2 = 1;
            } else if (!GameInteractor_Should(VB_CHECK_HELD_ITEM_BUTTON_PRESS,
                                              CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_B), sDpadItemButtons,
                                              sPlayerItemButtons)) {
                func_80840E5C(this, play);
            }
        } else if (!func_80840CD4(this, play)) {
            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
            temp_v0 = func_8083E7F8(this, &speedTarget, &yawTarget, play);
            if (temp_v0 > 0) {
                func_80840DEC(this, play);
            } else if (temp_v0 < 0) {
                func_80840E24(this, play);
            }
        }
    }
}

void Player_Action_31(Player* this, PlayState* play) {
    s32 var_v1;
    s32 temp_v0_2;
    f32 temp_ft4;
    f32 var_fa0;
    f32 speedTarget;
    s16 yawTarget;
    s16 temp_v0;
    f32 temp_fv1;
    s32 pad;
    s32 sp44;

    temp_v0 = this->yaw - this->actor.shape.rot.y;
    var_v1 = ABS_ALT(temp_v0);

    temp_ft4 = fabsf(this->speedXZ);
    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    var_fa0 = temp_ft4 * 1.5f;
    var_fa0 = CLAMP_MIN(var_fa0, 1.5f);

    var_fa0 = ((var_v1 < 0x4000) ? -1.0f : 1.0f) * var_fa0;

    func_8083EA44(this, var_fa0);

    var_fa0 = CLAMP(temp_ft4 * 0.5f, 0.5f, 1.0f);

    PlayerAnimation_BlendToJoint(play, &this->skelAnime, D_8085CF60[Player_IsHoldingTwoHandedWeapon(this)], 0.0f,
                                 D_8085CF70[Player_IsHoldingTwoHandedWeapon(this)], this->unk_B38 * 0.7241379f, var_fa0,
                                 this->blendTableBuffer);
    if (!func_8083FE38(this, play) && !func_80840CD4(this, play)) {
        func_80840F34(this);
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        temp_v0_2 = func_8083E7F8(this, &speedTarget, &yawTarget, play);
        if (temp_v0_2 < 0) {
            func_80840E24(this, play);
            return;
        }

        if (temp_v0_2 == 0) {
            speedTarget = 0.0f;
            yawTarget = this->yaw;
        }

        sp44 = ABS_ALT(BINANG_SUB(yawTarget, this->yaw));
        if (sp44 > 0x4000) {
            if (Math_StepToF(&this->speedXZ, 0.0f, 1.0f)) {
                this->yaw = yawTarget;
            }
        } else {
            Math_AsymStepToF(&this->speedXZ, speedTarget * 0.2f, 1.0f, 0.5f);
            Math_ScaledStepToS(&this->yaw, yawTarget, sp44 * 0.1f);
            if ((speedTarget == 0.0f) && (this->speedXZ == 0.0f)) {
                func_80840EC0(this, play);
            }
        }
    }
}

void Player_Action_32(Player* this, PlayState* play) {
    f32 sp5C = fabsf(this->speedXZ);
    f32 var_fa0;

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (sp5C == 0.0f) {
        sp5C = ABS_ALT(this->unk_B4C) * 0.0015f;
        if (sp5C < 400.0f) {
            sp5C = 0.0f;
        }

        func_8083EA44(this, ((this->unk_B4C >= 0) ? 1 : -1) * sp5C);
    } else {
        var_fa0 = sp5C * 1.5f;
        var_fa0 = CLAMP_MIN(var_fa0, 1.5f);
        func_8083EA44(this, var_fa0);
    }

    var_fa0 = CLAMP(sp5C * 0.5f, 0.5f, 1.0f);

    PlayerAnimation_BlendToJoint(play, &this->skelAnime, D_8085CF60[Player_IsHoldingTwoHandedWeapon(this)], 0.0f,
                                 D_8085CF78[Player_IsHoldingTwoHandedWeapon(this)], this->unk_B38 * 0.7241379f, var_fa0,
                                 this->blendTableBuffer);
    if (!func_8083FE38(this, play) && !func_80840CD4(this, play)) {
        f32 speedTarget;
        s16 yawTarget;
        s32 temp_v0;
        s16 temp_v0_2;
        s32 var_v1;

        func_80840F34(this);
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        temp_v0 = func_8083E7F8(this, &speedTarget, &yawTarget, play);
        if (temp_v0 > 0) {
            func_80840DEC(this, play);
            return;
        }

        if (temp_v0 == 0) {
            speedTarget = 0.0f;
            yawTarget = this->yaw;
        }

        var_v1 = ABS_ALT(BINANG_SUB(yawTarget, this->yaw));
        if (var_v1 > 0x4000) {
            if (Math_StepToF(&this->speedXZ, 0.0f, 1.0f)) {
                this->yaw = yawTarget;
            }
        } else {
            Math_AsymStepToF(&this->speedXZ, speedTarget * 0.2f, 1.0f, 0.5f);
            Math_ScaledStepToS(&this->yaw, yawTarget, var_v1 * 0.1f);
            if ((speedTarget == 0.0f) && (this->speedXZ == 0.0f) && (sp5C == 0.0f)) {
                func_80840EC0(this, play);
            }
        }
    }
}

void Player_Action_33(Player* this, PlayState* play) {
    s32 animFinished;
    f32 frame;
    PlayerActionInterruptResult interruptResult;

    this->stateFlags2 |= PLAYER_STATE2_20;
    animFinished = PlayerAnimation_Update(play, &this->skelAnime);

    if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_250jump_start)) {
        this->speedXZ = 1.0f;
        if (PlayerAnimation_OnFrame(&this->skelAnime, 8.0f)) {
            f32 speed = this->yDistToLedge;

            speed = CLAMP_MAX(speed, this->ageProperties->unk_0C);
            if (this->stateFlags1 & PLAYER_STATE1_8000000) {
                speed *= 0.085f;
            } else {
                speed *= 0.072f;
            }

            if (this->transformation == PLAYER_FORM_HUMAN) {
                speed += 1.0f;
            }

            func_80834D50(play, this, NULL, speed, NA_SE_VO_LI_AUTO_JUMP);
            this->av2.actionVar2 = -1;
        }
    } else {
        interruptResult = Player_TryActionInterrupt(play, this, &this->skelAnime, 4.0f);

        if (interruptResult == PLAYER_INTERRUPT_NEW_ACTION) {
            this->stateFlags1 &= ~(PLAYER_STATE1_4 | PLAYER_STATE1_4000 | PLAYER_STATE1_40000);
            return;
        }

        if (animFinished || (interruptResult >= PLAYER_INTERRUPT_MOVE)) {
            func_80839E74(this, play);
            this->stateFlags1 &= ~(PLAYER_STATE1_4 | PLAYER_STATE1_4000 | PLAYER_STATE1_40000);
            this->unk_ABC = 0.0f;
            return;
        }

        frame = 0.0f;
        if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_swimer_swim_15step_up)) {
            if (PlayerAnimation_OnFrame(&this->skelAnime, 30.0f)) {
                func_8083B32C(play, this, 10.0f);
            }
            frame = 50.0f;
        } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_150step_up)) {
            frame = 30.0f;
        } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_100step_up)) {
            frame = 16.0f;
        }

        if (PlayerAnimation_OnFrame(&this->skelAnime, frame)) {
            Player_AnimSfx_PlayFloorLand(this);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_CLIMB_END);
        }

        if ((BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_100step_up)) ||
            (this->skelAnime.curFrame > 5.0f)) {
            if (this->av2.actionVar2 == 0) {
                Player_AnimSfx_PlayFloorJump(this);
                this->av2.actionVar2 = 1;
            }
            Math_SmoothStepToF(&this->unk_ABC, 0.0f, 0.1f, 400.0f, 150.0f);
        }
    }
}

/**
 * Allow the held item put away process to complete before running `afterPutAwayFunc`
 */
void Player_Action_WaitForPutAway(Player* this, PlayState* play) {
    s32 upperBodyIsBusy;

    this->stateFlags2 |= (PLAYER_STATE2_20 | PLAYER_STATE2_40);
    if (this->afterPutAwayFunc == func_80837BF8) {
        this->stateFlags2 |= PLAYER_STATE2_1;
    }

    PlayerAnimation_Update(play, &this->skelAnime);
    func_8083249C(this);

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
    // the actor, so `!upperBodyIsBusy` could never pass.

    upperBodyIsBusy = Player_UpdateUpperBody(this, play);

    if (((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->heldActor != NULL) &&
         (this->getItemId == GI_NONE)) ||
        !upperBodyIsBusy) {
        this->afterPutAwayFunc(play, this);
    }
}

void Player_Action_35(Player* this, PlayState* play) {
    if (!Player_ActionHandler_13(this, play)) {
        if ((this->stateFlags3 & PLAYER_STATE3_10) && !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            func_80833AA0(this, play);
            this->stateFlags1 |= PLAYER_STATE1_20000000;
        } else if (this->av2.actionVar2 == 0) {
            PlayerAnimation_Update(play, &this->skelAnime);
            if (DECR(this->doorTimer) == 0) {
                this->speedXZ = 0.1f;
                this->av2.actionVar2 = 1;
            }
        } else if (this->av1.actionVar1 == 0) {
            f32 sp6C = 5.0f * sWaterSpeedFactor;
            s32 var_t0 = func_808411D4(play, this, &sp6C, -1);

            if (this->unk_397 == 4) {
                if (R_PLAY_FILL_SCREEN_ON < 0) {
                    if (play->roomCtx.status != 1) {
                        R_PLAY_FILL_SCREEN_ALPHA += R_PLAY_FILL_SCREEN_ON;
                        if (R_PLAY_FILL_SCREEN_ALPHA < 0) {
                            R_PLAY_FILL_SCREEN_ALPHA = 0;
                        }

                        this->actor.world.pos.y += (this->doorDirection != 0) ? 3.0f : -3.0f;
                        this->actor.prevPos.y = this->actor.world.pos.y;
                    }
                } else if (R_PLAY_FILL_SCREEN_ON == 0) {
                    CollisionPoly* sp64;
                    s32 sp60;

                    if (func_80835DF8(play, this, &sp64, &sp60)) {
                        this->actor.floorPoly = sp64;
                        this->actor.floorBgId = sp60;
                    }
                }
            }

            if (var_t0 < 0x1E) {
                this->av1.actionVar1 = 1;
                this->stateFlags1 |= PLAYER_STATE1_20000000;
                this->unk_3A0.x = this->unk_3AC.x;
                this->unk_3A0.z = this->unk_3AC.z;
            }
        } else {
            f32 sp5C = 5.0f;
            s32 sp58 = 0x14;
            s32 temp_v0_8;

            if (this->stateFlags1 & PLAYER_STATE1_1) {
                sp5C = gSaveContext.entranceSpeed;
                if (sPlayerConveyorSpeedIndex != CONVEYOR_SPEED_DISABLED) {
                    this->unk_3A0.x = (Math_SinS(sPlayerConveyorYaw) * 400.0f) + this->actor.world.pos.x;
                    this->unk_3A0.z = (Math_CosS(sPlayerConveyorYaw) * 400.0f) + this->actor.world.pos.z;
                }
            } else {
                if (this->av2.actionVar2 < 0) {
                    this->av2.actionVar2++;
                    sp5C = gSaveContext.entranceSpeed;
                    sp58 = -1;
                } else if (this->unk_397 == 4) {
                    if (R_PLAY_FILL_SCREEN_ON == 0) {
                        R_PLAY_FILL_SCREEN_ON = 16;
                        R_PLAY_FILL_SCREEN_ALPHA = 0;

                        R_PLAY_FILL_SCREEN_R = R_PLAY_FILL_SCREEN_G = R_PLAY_FILL_SCREEN_B = R_PLAY_FILL_SCREEN_ALPHA;
                    } else if (R_PLAY_FILL_SCREEN_ON >= 0) {
                        R_PLAY_FILL_SCREEN_ALPHA += R_PLAY_FILL_SCREEN_ON;
                        if (R_PLAY_FILL_SCREEN_ALPHA > 255) {
                            TransitionActorEntry* temp_v1_4; // sp50
                            s32 roomNum;

                            temp_v1_4 = &play->transitionActors.list[this->doorNext];
                            roomNum = temp_v1_4->sides[0].room;
                            R_PLAY_FILL_SCREEN_ALPHA = 255;

                            if ((roomNum != play->roomCtx.curRoom.num) && (play->roomCtx.curRoom.num >= 0)) {
                                play->roomCtx.prevRoom = play->roomCtx.curRoom;

                                play->roomCtx.curRoom.num = -1;
                                play->roomCtx.curRoom.segment = NULL;
                                Room_FinishRoomChange(play, &play->roomCtx);
                            } else {
                                static Vec3f D_8085D62C = { 0.0f, 0.0f, 0.0f };
                                static Vec3f D_8085D638 = { 0.0f, 0.0f, 0.0f };
                                static Vec3f D_8085D644 = { 0.0f, 0.0f, 0.0f };

                                R_PLAY_FILL_SCREEN_ON = -16;
                                if (play->roomCtx.curRoom.num < 0) {
                                    Room_RequestNewRoom(play, &play->roomCtx, temp_v1_4->sides[0].room);
                                    play->roomCtx.prevRoom.num = -1;
                                    play->roomCtx.prevRoom.segment = NULL;
                                }

                                this->actor.world.pos.x = temp_v1_4->pos.x;
                                this->actor.world.pos.y = temp_v1_4->pos.y;
                                this->actor.world.pos.z = temp_v1_4->pos.z;

                                this->actor.shape.rot.y = ((((temp_v1_4->rotY >> 7) & 0x1FF) / 180.0f) * 0x8000);

                                D_8085D62C.x = (this->doorDirection != 0) ? -120.0f : 120.0f;
                                D_8085D62C.y = (this->doorDirection != 0) ? -75.0f : 75.0f;
                                D_8085D62C.z = -240.0f;
                                if (this->doorDirection != 0) {
                                    Camera_ChangeDoorCam(play->cameraPtrs[0], &this->actor, -2, 0.0f,
                                                         temp_v1_4->pos.x + 0x32, temp_v1_4->pos.y + 0x5F,
                                                         temp_v1_4->pos.z - 0x32);
                                } else {
                                    Camera_ChangeDoorCam(play->cameraPtrs[0], &this->actor, -2, 0.0f,
                                                         temp_v1_4->pos.x - 0x32, temp_v1_4->pos.y + 5,
                                                         temp_v1_4->pos.z - 0x32);
                                }

                                Player_TranslateAndRotateY(this, &this->actor.world.pos, &D_8085D62C,
                                                           &this->actor.world.pos);

                                D_8085D638.x = (this->doorDirection != 0) ? 130.0f : -130.0f;
                                D_8085D638.z = 160.0f;
                                Player_TranslateAndRotateY(this, &this->actor.world.pos, &D_8085D638, &this->unk_3A0);
                                D_8085D644.z = 160.0f;
                                Player_TranslateAndRotateY(this, &this->unk_3A0, &D_8085D644, &this->unk_3AC);

                                this->actor.shape.rot.y += (this->doorDirection != 0) ? 0x4000 : -0x4000;
                                this->av1.actionVar1 = 0;

                                this->actor.world.rot.y = this->yaw = this->actor.shape.rot.y;
                            }
                        }

                        this->actor.world.pos.y += (this->doorDirection != 0) ? 3.0f : -3.0f;
                        this->actor.prevPos.y = this->actor.world.pos.y;
                    }
                }
            }

            temp_v0_8 = func_808411D4(play, this, &sp5C, sp58);
            if ((this->av2.actionVar2 == 0) || ((temp_v0_8 == 0) && (this->speedXZ == 0.0f) &&
                                                (Play_GetCamera(play, CAM_ID_MAIN)->stateFlags & CAM_STATE_4))) {
                if (this->unk_397 == 4) {
                    Map_InitRoomData(play, play->roomCtx.curRoom.num);
                    Map_SetAreaEntrypoint(play);
                }

                R_PLAY_FILL_SCREEN_ON = 0;
                Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
                Player_StopCutscene(this);
                if (!(this->stateFlags3 & PLAYER_STATE3_20000)) {
                    func_801226E0(play, ((void)0, gSaveContext.respawn[RESPAWN_MODE_DOWN].data));
                }

                if (play->bButtonAmmoPlusOne != 0) {
                    play->func_18780(this, play);
                    Player_SetAction(play, this, Player_Action_80, 0);
                    if (play->sceneId == SCENE_20SICHITAI) {
                        play->bButtonAmmoPlusOne = 0;
                    }
                } else if (!Player_ActionHandler_Talk(this, play)) {
                    func_8083B2E4(this, play);
                }
            }
        }
    }

    if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        Player_UpdateUpperBody(this, play);
    }
}

// door stuff
void Player_Action_36(Player* this, PlayState* play) {
    EnDoor* doorActor = (EnDoor*)this->doorActor;
    s32 framedDoor = (doorActor != NULL) && (doorActor->doorType == ENDOOR_TYPE_FRAMED);
    s32 animFinished;
    CollisionPoly* poly;
    s32 bgId;

    this->stateFlags2 |= PLAYER_STATE2_20;

    if (DECR(this->av1.actionVar1) == 0) {
        func_80835DF8(play, this, &poly, &bgId);
    }

    animFinished = PlayerAnimation_Update(play, &this->skelAnime);
    Player_UpdateUpperBody(this, play);

    if (animFinished) {
        if (this->av2.actionVar2 == 0) {
            if (DECR(this->doorTimer) == 0) {
                this->av2.actionVar2 = 1;
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        } else {
            Player_StopCutscene(this);
            func_80839E74(this, play);

            if ((this->actor.category == ACTORCAT_PLAYER) && !framedDoor) {
                if (play->roomCtx.prevRoom.num >= 0) {
                    Room_FinishRoomChange(play, &play->roomCtx);
                }

                Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
                Play_SetupRespawnPoint(play, RESPAWN_MODE_DOWN, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_B));
            }
        }
    } else if (!(this->stateFlags1 & PLAYER_STATE1_20000000) && PlayerAnimation_OnFrame(&this->skelAnime, 15.0f)) {
        Player_StopCutscene(this);
        play->func_18780(this, play);
    } else if (framedDoor && PlayerAnimation_OnFrame(&this->skelAnime, 15.0f)) {
        s16 exitIndexPlusOne = (this->doorDirection < 0) ? doorActor->knobDoor.dyna.actor.world.rot.x
                                                         : doorActor->knobDoor.dyna.actor.world.rot.z;

        if (exitIndexPlusOne != 0) {
            func_808354A4(play, exitIndexPlusOne - 1, false);
        }
    }
}

// grab/hold an actor (?)
void Player_Action_37(Player* this, PlayState* play) {
    Player_DecelerateToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        func_80836988(this, play);
        func_808313F0(this, play);
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor* interactRangeActor = this->interactRangeActor;

        if (!func_808313A8(play, this, interactRangeActor)) {
            this->actor.child = interactRangeActor;
            this->heldActor = interactRangeActor;
            interactRangeActor->parent = &this->actor;
            interactRangeActor->bgCheckFlags &=
                ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_GROUND_LEAVE | BGCHECKFLAG_WALL |
                  BGCHECKFLAG_CEILING | BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH | BGCHECKFLAG_GROUND_STRICT);
            this->leftHandWorld.rot.y = interactRangeActor->shape.rot.y - this->actor.shape.rot.y;
        }
    } else {
        Math_ScaledStepToS(&this->leftHandWorld.rot.y, 0, 0xFA0);
    }
}

// grab/hold an actor (?)
void Player_Action_38(Player* this, PlayState* play) {
    Player_DecelerateToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_silver_wait);
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        if (PlayerAnimation_OnFrame(&this->skelAnime, 27.0f)) {
            Actor* interactRangeActor = this->interactRangeActor;

            this->heldActor = interactRangeActor;
            this->actor.child = interactRangeActor;
            interactRangeActor->parent = &this->actor;
        } else if (PlayerAnimation_OnFrame(&this->skelAnime, 25.0f)) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_L);
        }
    } else if (CHECK_BTN_ANY(sPlayerControlInput->press.button,
                             BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_B | BTN_A | BTN_DPAD_EQUIP)) {
        Player_SetAction(play, this, Player_Action_39, 1);
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_silver_throw);
    }
}

// throw held actor (?)
void Player_Action_39(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        func_80836988(this, play);
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, 6.0f)) {
        Actor* heldActor = this->heldActor;

        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speed = 10.0f;
        heldActor->velocity.y = 20.0f;
        func_808309CC(play, this);
        Player_PlaySfx(this, NA_SE_PL_THROW);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
    }
}

void Player_Action_40(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_nocarry_free_wait);
        this->av2.actionVar2 = 15;
    } else if (this->av2.actionVar2 != 0) {
        this->av2.actionVar2--;
        if (this->av2.actionVar2 == 0) {
            func_80836A98(this, &gPlayerAnim_link_normal_nocarry_free_end, play);
            this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);
        }
    }
}

// Player_Action_PutDownObject?
void Player_Action_41(Player* this, PlayState* play) {
    Player_DecelerateToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        func_80836988(this, play);
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor* heldActor = this->heldActor;

        if (!func_808313A8(play, this, heldActor)) {
            heldActor->velocity.y = 0.0f;
            heldActor->speed = 0.0f;
            func_808309CC(play, this);
            if (heldActor->id == ACTOR_EN_BOM_CHU) {
                func_80831814(this, play, PLAYER_UNKAA5_0);
            }
        }
    }
}

// Player_Action_Throwing
void Player_Action_42(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    Player_DecelerateToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime) ||
        ((this->skelAnime.curFrame >= 8.0f) &&
         Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play))) {
        func_80836988(this, play);
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, 3.0f)) {
        func_808409A8(play, this, this->speedXZ + 8.0f, 12.0f);
    }
}

void Player_Action_43(Player* this, PlayState* play) {
    if (this->stateFlags1 & PLAYER_STATE1_8000000) {
        func_808475B4(this);
        func_8084748C(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
    } else {
        Player_DecelerateToZero(this);
    }

    if (this->unk_AA5 == PLAYER_UNKAA5_3) {
        if (func_800B7118(this) || Player_IsUsingZoraBoomerang(this)) {
            Player_UpdateUpperBody(this, play);
        }
    }

    if (((this->unk_AA5 == PLAYER_UNKAA5_2) && !(play->actorCtx.flags & ACTORCTX_FLAG_PICTO_BOX_ON)) ||
        ((this->unk_AA5 != PLAYER_UNKAA5_2) &&
         ((((this->csAction != PLAYER_CSACTION_NONE) || ((u32)this->unk_AA5 == PLAYER_UNKAA5_0) ||
            (this->unk_AA5 >= PLAYER_UNKAA5_5) || Player_UpdateHostileLockOn(this) || (this->focusActor != NULL) ||
            (func_8083868C(play, this) == CAM_MODE_NORMAL) ||
            ((this->unk_AA5 == PLAYER_UNKAA5_3) &&
             (((Player_ItemToItemAction(this, Inventory_GetBtnBItem(play)) != this->heldItemAction) &&
               CHECK_BTN_ANY(sPlayerControlInput->press.button, BTN_B)) ||
              (CHECK_BTN_ANY(sPlayerControlInput->press.button, BTN_R | BTN_A) &&
               GameInteractor_Should(VB_EXIT_FIRST_PERSON_MODE_FROM_BUTTON, true)) ||
              Player_FriendlyLockOnOrParallel(this) || (!func_800B7128(this) && !func_8082EF20(this))))) ||
           ((this->unk_AA5 == PLAYER_UNKAA5_1) &&
            CHECK_BTN_ANY(sPlayerControlInput->press.button,
                          BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP | BTN_R | BTN_B | BTN_A | BTN_DPAD_EQUIP) &&
            GameInteractor_Should(VB_EXIT_FIRST_PERSON_MODE_FROM_BUTTON, true))) ||
          Player_ActionHandler_Talk(this, play)))) {
        func_80839ED0(this, play);
        Audio_PlaySfx(NA_SE_SY_CAMERA_ZOOM_UP);
    } else if ((DECR(this->av2.actionVar2) == 0) || (this->unk_AA5 != PLAYER_UNKAA5_3)) {
        if (func_801240DC(this)) {
            this->unk_AA6_rotFlags |= UNKAA6_ROT_FOCUS_X | UNKAA6_ROT_FOCUS_Y | UNKAA6_ROT_UPPER_X;
        } else {
            this->actor.shape.rot.y = func_80847190(play, this, 0);
        }
    }

    this->yaw = this->actor.shape.rot.y;
}

void Player_Action_Talk(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20;

    func_8083249C(this);
    Player_UpdateUpperBody(this, play);

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~ACTOR_FLAG_TALK;
        if (!CHECK_FLAG_ALL(this->talkActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)) {
            this->stateFlags2 &= ~PLAYER_STATE2_LOCK_ON_WITH_SWITCH;
        }

        Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);

        if (this->stateFlags1 & PLAYER_STATE1_800000) {
            s32 sp44 = this->av2.actionVar2;

            func_80837BD0(play, this);
            this->av2.actionVar2 = sp44;
        } else if (!func_80847994(play, this) && !func_80847880(play, this) && !Player_StartCsAction(play, this) &&
                   ((this->talkActor != this->interactRangeActor) || !Player_ActionHandler_2(this, play))) {
            if (func_801242B4(this)) {
                func_808353DC(play, this);
            } else {
                func_8085B384(this, play);
            }
        }

        this->textboxBtnCooldownTimer = 10;
        return;
    }

    if (this->stateFlags1 & PLAYER_STATE1_800000) {
        Player_Action_52(this, play);
    } else if (func_801242B4(this)) {
        Player_Action_54(this, play);
        if (this->actor.depthInWater > 100.0f) {
            this->actor.velocity.y = 0.0f;
            this->actor.gravity = 0.0f;
        }
    } else if (!Player_CheckHostileLockOn(this) && PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->skelAnime.movementFlags != 0) {
            Player_Anim_ResetMove(this);
            if ((this->talkActor->category == ACTORCAT_NPC) && (this->heldItemAction != PLAYER_IA_FISHING_ROD)) {
                Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_normal_talk_free);
            } else {
                Player_Anim_PlayLoop(play, this, Player_GetIdleAnim(this));
            }
        } else {
            Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_link_normal_talk_free_wait);
        }
    }

    if (this->focusActor != NULL) {
        this->yaw = func_8083C62C(this, false);
        this->actor.shape.rot.y = this->yaw;
        if (this->av1.actionVar1 != 0) {
            if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
                if (PlayerAnimation_Update(play, &this->skelAnimeUpper)) {
                    this->av1.actionVar1--;
                    if (this->av1.actionVar1 != 0) {
                        PlayerAnimation_Change(
                            play, &this->skelAnimeUpper, &gPlayerAnim_link_normal_talk_free, PLAYER_ANIM_NORMAL_SPEED,
                            0.0f, Animation_GetLastFrame(&gPlayerAnim_link_normal_talk_free), ANIMMODE_ONCE, -6.0f);
                    }
                }
            }
            AnimTaskQueue_AddCopyUsingMapInverted(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                                  this->skelAnimeUpper.jointTable, sPlayerUpperBodyLimbCopyMap);
        } else if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) &&
                   (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_talk_free_wait))) {
            s32 temp_v0 = this->actor.focus.rot.y - this->actor.shape.rot.y;

            if (ABS_ALT(temp_v0) > 0xFA0) {
                PlayerAnimation_Change(
                    play, &this->skelAnimeUpper, D_8085BE84[PLAYER_ANIMGROUP_45_turn][this->modelAnimType], 0.4f, 0.0f,
                    Animation_GetLastFrame(D_8085BE84[PLAYER_ANIMGROUP_45_turn][this->modelAnimType]), ANIMMODE_ONCE,
                    -6.0f);
                this->av1.actionVar1 = 2;
            }
        }
    }
}

void Player_Action_45(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp_v0;

    this->stateFlags2 |= (PLAYER_STATE2_1 | PLAYER_STATE2_40 | PLAYER_STATE2_100);
    func_8083DEE4(play, this);

    if (PlayerAnimation_Update(play, &this->skelAnime) && !func_8083E14C(play, this)) {
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        temp_v0 = func_8083E758(this, &speedTarget, &yawTarget);
        if (temp_v0 > 0) {
            func_8083E234(this, play);
        } else if (temp_v0 < 0) {
            func_8083E28C(this, play);
        }
    }
}

AnimSfxEntry D_8085D650[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 3, NA_SE_PL_SLIP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 21, NA_SE_PL_SLIP, STOP),
};

void Player_Action_46(Player* this, PlayState* play) {
    this->stateFlags2 |= (PLAYER_STATE2_1 | PLAYER_STATE2_40 | PLAYER_STATE2_100);

    if (Player_Anim_PlayLoopOnceFinished(play, this, &gPlayerAnim_link_normal_pushing)) {
        this->av2.actionVar2 = 1;
    } else if ((this->av2.actionVar2 == 0) && PlayerAnimation_OnFrame(&this->skelAnime, 11.0f)) {
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_PUSH);
    }

    Player_PlayAnimSfx(this, D_8085D650);
    func_8083DEE4(play, this);

    if (!func_8083E14C(play, this)) {
        f32 speedTarget;
        s16 yawTarget;
        s32 temp_v0;

        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        temp_v0 = func_8083E758(this, &speedTarget, &yawTarget);
        if (temp_v0 < 0) {
            func_8083E28C(this, play);
        } else if (temp_v0 == 0) {
            func_8083DF38(this, &gPlayerAnim_link_normal_push_end, play);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_10;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_10) {
        func_808479F4(play, this, 2.0f);
        this->speedXZ = 2.0f;
    }
}

AnimSfxEntry D_8085D658[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 4, NA_SE_PL_SLIP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 24, NA_SE_PL_SLIP, STOP),
};

Vec3f D_8085D660 = { 0.0f, 268 * 0.1f, -60.0f };

void Player_Action_47(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim = D_8085BE84[PLAYER_ANIMGROUP_pulling][this->modelAnimType];

    this->stateFlags2 |= (PLAYER_STATE2_1 | PLAYER_STATE2_40 | PLAYER_STATE2_100);

    if (Player_Anim_PlayLoopOnceFinished(play, this, anim)) {
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        if (PlayerAnimation_OnFrame(&this->skelAnime, 11.0f)) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_PUSH);
        }

        //! FAKE
        if (1) {}
    } else {
        Player_PlayAnimSfx(this, D_8085D658);
    }

    func_8083DEE4(play, this);
    if (!func_8083E14C(play, this)) {
        f32 speedTarget;
        s16 yawTarget;
        s32 temp_v0;

        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        temp_v0 = func_8083E758(this, &speedTarget, &yawTarget);
        if (temp_v0 > 0) {
            func_8083E234(this, play);
        } else if (temp_v0 == 0) {
            func_8083DF38(this, D_8085BE84[PLAYER_ANIMGROUP_pull_end][this->modelAnimType], play);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_10;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_10) {
        Vec3f sp64;
        f32 yIntersect = func_80835D2C(play, this, &D_8085D660, &sp64) - this->actor.world.pos.y;
        CollisionPoly* poly;
        s32 bgId;
        Vec3f sp4C;
        Vec3f sp40;

        if (fabsf(yIntersect) < 268 * 0.1f) {
            sp64.y -= 7.0f;
            sp4C.x = this->actor.world.pos.x;
            sp4C.z = this->actor.world.pos.z;
            sp4C.y = sp64.y;
            if (!BgCheck_EntityLineTest2(&play->colCtx, &sp4C, &sp64, &sp40, &poly, true, false, false, true, &bgId,
                                         &this->actor)) {
                func_808479F4(play, this, -2.0f);
                return;
            }
        }
        this->stateFlags2 &= ~PLAYER_STATE2_10;
    }
}

void Player_Action_48(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_40;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this,
                             (this->av1.actionVar1 > 0)
                                 ? &gPlayerAnim_link_normal_fall_wait
                                 : D_8085BE84[PLAYER_ANIMGROUP_jump_climb_wait][this->modelAnimType]);
    } else if (this->av1.actionVar1 == 0) {
        f32 frame;

        if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_fall)) {
            frame = 11.0f;
        } else {
            frame = 1.0f;
        }

        if (PlayerAnimation_OnFrame(&this->skelAnime, frame)) {
            Player_AnimSfx_PlayFloor(this, NA_SE_PL_WALK_GROUND);
            if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_fall)) {
                this->av1.actionVar1 = 1;
            } else {
                this->av1.actionVar1 = -1;
            }
        }
    }

    Math_ScaledStepToS(&this->actor.shape.rot.y, this->yaw, 0x800);
    if (this->av1.actionVar1 != 0) {
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        if (this->controlStickSpinAngles[this->controlStickDataIndex] >= 0) {
            func_808381A0(this,
                          (this->av1.actionVar1 > 0) ? D_8085BE84[PLAYER_ANIMGROUP_fall_up][this->modelAnimType]
                                                     : D_8085BE84[PLAYER_ANIMGROUP_jump_climb_up][this->modelAnimType],
                          play);
        } else if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A) || (this->actor.shape.feetFloorFlags != 0)) {
            func_80833A64(this);

            if (this->av1.actionVar1 < 0) {
                this->speedXZ = -0.8f;
            } else {
                this->speedXZ = 0.8f;
            }

            func_80833AA0(this, play);
            this->stateFlags1 &= ~(PLAYER_STATE1_4 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000);
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        }
    }
}

void Player_Action_49(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_40;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        this->yaw = this->skelAnime.jointTable[LIMB_ROOT_ROT].y + this->actor.shape.rot.y;
        Player_AnimReplace_SetupLedgeClimb(this, ANIM_FLAG_1);
        this->actor.shape.rot.y = this->yaw;
        func_80839E74(this, play);
        this->stateFlags1 &= ~(PLAYER_STATE1_4 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000);
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, this->skelAnime.endFrame - 6.0f)) {
        Player_AnimSfx_PlayFloorLand(this);
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, this->skelAnime.endFrame - 34.0f)) {
        Player_PlaySfx(this, NA_SE_PL_CLIMB_CLIFF);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_CLIMB_END);
        func_8084C124(play, this);
    }
}

void Player_Action_50(Player* this, PlayState* play) {
    s32 yStick = sPlayerControlInput->rel.stick_y;
    s32 xStick = sPlayerControlInput->rel.stick_x;
    f32 var_fv0;
    f32 var_fv1;
    Vec3f sp7C;
    s32 sp78;
    Vec3f sp6C;
    Vec3f sp60;
    DynaPolyActor* dyna;
    PlayerAnimationHeader* anim1;
    PlayerAnimationHeader* anim2;

    xStick *= GameInteractor_InvertControl(GI_INVERT_MOVEMENT_X);

    this->fallStartHeight = this->actor.world.pos.y;

    this->stateFlags2 |= PLAYER_STATE2_40;

    if ((this->av1.actionVar1 != 0) && (ABS_ALT(yStick) < ABS_ALT(xStick))) {
        var_fv0 = ABS_ALT(xStick) * 0.0325f;
        yStick = 0;
    } else {
        var_fv0 = ABS_ALT(yStick) * 0.05f;
        xStick = 0;
    }

    if (var_fv0 < 1.0f) {
        var_fv0 = 1.0f;
    } else if (var_fv0 > 3.35f) {
        var_fv0 = 3.35f;
    }

    if (this->skelAnime.playSpeed >= 0.0f) {
        var_fv1 = 1.0f;
    } else {
        var_fv1 = -1.0f;
    }

    if (GameInteractor_Should(VB_SET_CLIMB_SPEED, true, &var_fv1)) {
        this->skelAnime.playSpeed = var_fv1 * var_fv0;
    }

    if (this->av2.actionVar2 >= 0) {
        if ((this->actor.wallPoly != NULL) && (this->actor.wallBgId != BGCHECK_SCENE)) {
            dyna = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);

            if (dyna != NULL) {
                Math_Vec3f_Diff(&dyna->actor.world.pos, &dyna->actor.prevPos, &sp7C);
                Math_Vec3f_Sum(&this->actor.world.pos, &sp7C, &this->actor.world.pos);
            }
        }

        Actor_UpdateBgCheckInfo(play, &this->actor, 268 * 0.1f, 6.0f, this->ageProperties->ceilingCheckHeight + 15.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
        func_8083DD1C(play, this, 268 * 0.1f, this->ageProperties->unk_3C, 50.0f, -20.0f);
    }

    func_80831944(play, this);

    if ((this->av2.actionVar2 < 0) || !func_8083E354(this, play)) {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            if (this->av2.actionVar2 < 0) {
                this->av2.actionVar2 = ABS_ALT(this->av2.actionVar2) & 1;
            } else if (yStick != 0) {
                f32 yIntersect;

                sp78 = this->av1.actionVar1 + this->av2.actionVar2;

                if (yStick > 0) {
                    sp6C.x = 0.0f;
                    sp6C.y = this->ageProperties->unk_40;
                    sp6C.z = this->ageProperties->unk_3C + 10.0f;

                    yIntersect = func_80835D2C(play, this, &sp6C, &sp60);

                    if (this->actor.world.pos.y < yIntersect) {
                        if (this->av1.actionVar1 != 0) {
                            this->actor.world.pos.y = yIntersect;
                            this->stateFlags1 &= ~PLAYER_STATE1_200000;
                            func_80837CEC(play, this, this->actor.wallPoly, this->ageProperties->unk_3C,
                                          &gPlayerAnim_link_normal_jump_climb_up_free);
                            this->yaw += 0x8000;
                            this->actor.shape.rot.y = this->yaw;
                            func_808381A0(this, &gPlayerAnim_link_normal_jump_climb_up_free, play);
                            this->stateFlags1 |= PLAYER_STATE1_4000;
                        } else {
                            func_8083DCC4(this, this->ageProperties->unk_D4[this->av2.actionVar2], play);
                        }
                    } else {
                        this->skelAnime.prevTransl = this->ageProperties->unk_4A[sp78];
                        Player_Anim_PlayOnce(play, this, this->ageProperties->unk_B4[sp78]);
                    }
                } else if ((this->actor.world.pos.y - this->actor.floorHeight) < 15.0f) {
                    if (this->av1.actionVar1 != 0) {
                        func_8083E2F4(this, play);
                    } else {
                        if (this->av2.actionVar2 != 0) {
                            this->skelAnime.prevTransl = this->ageProperties->unk_44;
                        }

                        func_8083DCC4(this, this->ageProperties->unk_CC[this->av2.actionVar2], play);
                        this->av2.actionVar2 = 1;
                    }
                } else {
                    sp78 ^= 1;
                    this->skelAnime.prevTransl = this->ageProperties->unk_62[sp78];
                    anim1 = this->ageProperties->unk_B4[sp78];
                    PlayerAnimation_Change(play, &this->skelAnime, anim1, -1.0f, Animation_GetLastFrame(anim1), 0.0f, 2,
                                           0.0f);
                }

                this->av2.actionVar2 ^= 1;
            } else if ((this->av1.actionVar1 != 0) && (xStick != 0)) {
                anim2 = this->ageProperties->unk_C4[this->av2.actionVar2];

                if (xStick > 0) {
                    this->skelAnime.prevTransl = this->ageProperties->unk_7A[this->av2.actionVar2];
                    Player_Anim_PlayOnce(play, this, anim2);
                } else {
                    this->skelAnime.prevTransl = this->ageProperties->unk_7A[this->av2.actionVar2 + 2];
                    PlayerAnimation_Change(play, &this->skelAnime, anim2, -1.0f, Animation_GetLastFrame(anim2), 0.0f, 2,
                                           0.0f);
                }
            } else {
                this->stateFlags2 |= PLAYER_STATE2_1000;
            }

            return;
        }
    }

    if (this->av2.actionVar2 < 0) {
        if (((this->av2.actionVar2 == -2) &&
             (PlayerAnimation_OnFrame(&this->skelAnime, 14.0f) || PlayerAnimation_OnFrame(&this->skelAnime, 29.0f))) ||
            ((this->av2.actionVar2 == -4) &&
             (PlayerAnimation_OnFrame(&this->skelAnime, 22.0f) || PlayerAnimation_OnFrame(&this->skelAnime, 35.0f) ||
              PlayerAnimation_OnFrame(&this->skelAnime, 49.0f) || PlayerAnimation_OnFrame(&this->skelAnime, 55.0f)))) {
            func_80847A50(this);
        }
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, (this->skelAnime.playSpeed > 0.0f) ? 20.0f : 0.0f)) {
        func_80847A50(this);
    }
}

f32 D_8085D66C[] = { 11.0f, 21.0f };
f32 D_8085D674[] = { 40.0f, 50.0f };

AnimSfxEntry D_8085D67C[] = {
    ANIMSFX(ANIMSFX_TYPE_SURFACE, 10, NA_SE_PL_WALK_LADDER, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_SURFACE, 20, NA_SE_PL_WALK_LADDER, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_SURFACE, 30, NA_SE_PL_WALK_LADDER, STOP),
};

void Player_Action_51(Player* this, PlayState* play) {
    PlayerActionInterruptResult interruptResult;

    this->stateFlags2 |= PLAYER_STATE2_40;

    interruptResult = Player_TryActionInterrupt(play, this, &this->skelAnime, 4.0f);

    if (interruptResult == PLAYER_INTERRUPT_NEW_ACTION) {
        this->stateFlags1 &= ~PLAYER_STATE1_200000;
    } else if ((interruptResult >= PLAYER_INTERRUPT_MOVE) || PlayerAnimation_Update(play, &this->skelAnime)) {
        func_80839E74(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_200000;
    } else {
        f32* var_v1 = D_8085D66C;

        if (this->av2.actionVar2 != 0) {
            Player_PlayAnimSfx(this, D_8085D67C);
            var_v1 = D_8085D674;
        }

        if (PlayerAnimation_OnFrame(&this->skelAnime, var_v1[0]) ||
            PlayerAnimation_OnFrame(&this->skelAnime, var_v1[1])) {
            CollisionPoly* poly;
            s32 bgId;
            Vec3f pos;

            pos.x = this->actor.world.pos.x;
            pos.y = this->actor.world.pos.y + 20.0f;
            pos.z = this->actor.world.pos.z;
            if (BgCheck_EntityRaycastFloor5(&play->colCtx, &poly, &bgId, &this->actor, &pos) != 0.0f) {
                this->floorSfxOffset = SurfaceType_GetSfxOffset(&play->colCtx, poly, bgId);
                Player_AnimSfx_PlayFloorLand(this);
            }
        }
    }
}

void func_8084FD7C(PlayState* play, Player* this, Actor* actor) {
    s16 var_a3;

    if (this->unk_B86[0] != 0) {
        this->unk_B86[0]--;
        return;
    }

    this->upperLimbRot.y = func_80847190(play, this, 1) - this->actor.shape.rot.y;

    var_a3 = ABS_ALT(this->upperLimbRot.y) - 0x4000;
    if (var_a3 > 0) {
        var_a3 = CLAMP_MAX(var_a3, 0x15E);
        actor->shape.rot.y += var_a3 * ((this->upperLimbRot.y >= 0) ? 1 : -1);
        actor->world.rot.y = actor->shape.rot.y;
    }

    this->upperLimbRot.y += 0x2710;
    this->upperLimbYawSecondary = -0x1388;
}

bool func_8084FE48(Player* this) {
    return (this->focusActor == NULL) && !Player_IsZTargetingWithHostileUpdate(this);
}

PlayerAnimationHeader* D_8085D688[] = {
    &gPlayerAnim_link_uma_anim_stop,    &gPlayerAnim_link_uma_anim_stand,   &gPlayerAnim_link_uma_anim_walk,
    &gPlayerAnim_link_uma_anim_slowrun, &gPlayerAnim_link_uma_anim_fastrun, &gPlayerAnim_link_uma_anim_jump100,
    &gPlayerAnim_link_uma_anim_jump200,
};

PlayerAnimationHeader* D_8085D6A4[] = {
    NULL,
    NULL,
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

PlayerAnimationHeader* D_8085D6D0[] = {
    &gPlayerAnim_link_uma_wait_3,
    &gPlayerAnim_link_uma_wait_1,
    &gPlayerAnim_link_uma_wait_2,
};

u8 D_8085D6DC[][2] = {
    { 32, 58 },
    { 25, 42 },
};

Vec3s D_8085D6E0 = { -69, 7146, -266 };

AnimSfxEntry D_8085D6E8[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 48, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 58, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 68, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 92, NA_SE_PL_CALM_PAT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 110, NA_SE_PL_CALM_PAT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 126, NA_SE_PL_CALM_PAT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 132, NA_SE_PL_CALM_PAT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 136, NA_SE_PL_CALM_PAT, STOP),
};

void Player_Action_52(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;

    this->stateFlags2 |= PLAYER_STATE2_40;

    func_80847E2C(this, 1.0f, 10.0f);
    if (this->av2.actionVar2 == 0) {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            this->skelAnime.animation = &gPlayerAnim_link_uma_wait_1;
            this->av2.actionVar2 = 0x63;
        } else {
            s32 var_v0 = (this->mountSide < 0) ? 0 : 1;

            if (PlayerAnimation_OnFrame(&this->skelAnime, D_8085D6DC[var_v0][0])) {
                Player_SetCameraHorseSetting(play, this);
                Player_PlaySfx(this, NA_SE_PL_CLIMB_CLIFF);
            } else if (PlayerAnimation_OnFrame(&this->skelAnime, D_8085D6DC[var_v0][1])) {
                Player_PlaySfx(this, NA_SE_PL_SIT_ON_HORSE);
            }
        }
    } else {
        if (rideActor->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            func_80841A50(play, this);
        }

        Player_SetCameraHorseSetting(play, this);

        this->skelAnime.prevTransl = D_8085D6E0;

        if ((this->av2.actionVar2 < 0) ||
            ((rideActor->animIndex != (this->av2.actionVar2 & 0xFFFF)) &&
             ((rideActor->animIndex >= ENHORSE_ANIM_STOPPING) || (this->av2.actionVar2 >= 2)))) {
            s32 animIndex = rideActor->animIndex;

            if (animIndex < ENHORSE_ANIM_STOPPING) {
                f32 temp_fv0 = Rand_ZeroOne();
                s32 index = 0;

                animIndex = ENHORSE_ANIM_WHINNY;
                if (temp_fv0 < 0.1f) {
                    index = 2;
                } else if (temp_fv0 < 0.2f) {
                    index = 1;
                }

                Player_Anim_PlayOnce(play, this, D_8085D6D0[index]);
            } else {
                this->skelAnime.animation = D_8085D688[animIndex - 2];
                if (this->av2.actionVar2 >= 0) {
                    Animation_SetMorph(play, &this->skelAnime, 8.0f);
                }

                if (animIndex < ENHORSE_ANIM_WALK) {
                    func_808309CC(play, this);
                    this->av1.actionVar1 = 0;
                }
            }

            this->av2.actionVar2 = animIndex;
        }

        if (this->av2.actionVar2 == 1) {
            if (sUpperBodyIsBusy || Player_IsTalking(play)) {
                Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_uma_wait_3);
            } else if (PlayerAnimation_Update(play, &this->skelAnime)) {
                this->av2.actionVar2 = 0x63;
            } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_uma_wait_1)) {
                Player_PlayAnimSfx(this, D_8085D6E8);
            }
        } else {
            this->skelAnime.curFrame = rideActor->curFrame;
            PlayerAnimation_AnimateFrame(play, &this->skelAnime);
        }

        AnimTaskQueue_AddCopy(play, this->skelAnime.limbCount, this->skelAnime.morphTable, this->skelAnime.jointTable);

        if ((play->csCtx.state != CS_STATE_IDLE) || (this->csAction != PLAYER_CSACTION_NONE)) {
            this->unk_AA5 = PLAYER_UNKAA5_0;
            this->av1.actionVar1 = 0;
        } else if ((this->av2.actionVar2 < 2) || (this->av2.actionVar2 >= 4)) {
            sUpperBodyIsBusy = Player_UpdateUpperBody(this, play);
            if (sUpperBodyIsBusy) {
                this->av1.actionVar1 = 0;
            }
        }

        this->actor.world.pos.x = rideActor->actor.world.pos.x + rideActor->riderPos.x;
        this->actor.world.pos.y = rideActor->actor.world.pos.y + rideActor->riderPos.y - 27.0f;
        this->actor.world.pos.z = rideActor->actor.world.pos.z + rideActor->riderPos.z;

        this->yaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

        if (!sUpperBodyIsBusy) {
            if (this->av1.actionVar1 != 0) {
                if (PlayerAnimation_Update(play, &this->skelAnimeUpper)) {
                    rideActor->stateFlags &= ~ENHORSE_FLAG_8;
                    this->av1.actionVar1 = 0;
                }

                if (BEN_ANIM_EQUAL(this->skelAnimeUpper.animation, gPlayerAnim_link_uma_stop_muti)) {
                    if (PlayerAnimation_OnFrame(&this->skelAnimeUpper, 23.0f)) {
                        Player_PlaySfx(this, NA_SE_IT_LASH);
                        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_LASH);
                    }

                    AnimTaskQueue_AddCopy(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                          this->skelAnimeUpper.jointTable);
                } else {
                    if (PlayerAnimation_OnFrame(&this->skelAnimeUpper, 10.0f)) {
                        Player_PlaySfx(this, NA_SE_IT_LASH);
                        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_LASH);
                    }

                    AnimTaskQueue_AddCopyUsingMap(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                                  this->skelAnimeUpper.jointTable, sPlayerUpperBodyLimbCopyMap);
                }
            } else if (!CHECK_FLAG_ALL(this->actor.flags, 0x100)) {
                PlayerAnimationHeader* anim = NULL;

                if (EN_HORSE_CHECK_3(rideActor)) {
                    anim = &gPlayerAnim_link_uma_stop_muti;
                } else if (EN_HORSE_CHECK_2(rideActor)) {
                    if ((this->av2.actionVar2 >= 2) && (this->av2.actionVar2 != 0x63)) {
                        anim = D_8085D6A4[this->av2.actionVar2];
                    }
                }

                if (anim != NULL) {
                    PlayerAnimation_PlayOnce(play, &this->skelAnimeUpper, anim);
                    this->av1.actionVar1 = 1;
                }
            }
        }

        if (this->stateFlags1 & PLAYER_STATE1_100000) {
            if (CHECK_BTN_ANY(sPlayerControlInput->press.button, BTN_A) || !func_8084FE48(this)) {
                this->unk_AA5 = PLAYER_UNKAA5_0;
                this->stateFlags1 &= ~PLAYER_STATE1_100000;
            } else {
                func_8084FD7C(play, this, &rideActor->actor);
            }
        } else if ((this->csAction != PLAYER_CSACTION_NONE) ||
                   (!Player_IsTalking(play) &&
                    ((rideActor->actor.speed != 0.0f) || !Player_ActionHandler_Talk(this, play)) &&
                    !func_80847BF0(this, play) && !Player_ActionHandler_13(this, play))) {
            if (this->focusActor != NULL) {
                if (func_800B7128(this)) {
                    this->upperLimbRot.y = func_8083C62C(this, true) - this->actor.shape.rot.y;
                    this->upperLimbRot.y = CLAMP(this->upperLimbRot.y, -0x4AAA, 0x4AAA);
                    this->actor.focus.rot.y = this->actor.shape.rot.y + this->upperLimbRot.y;
                    this->upperLimbRot.y += 0xFA0;
                    this->unk_AA6_rotFlags |= UNKAA6_ROT_UPPER_Y;
                } else {
                    func_8083C62C(this, false);
                }

                this->upperLimbYawSecondary = 0;
            } else if (func_8084FE48(this)) {
                if (func_800B7128(this)) {
                    func_80831010(this, play);
                }

                this->unk_B86[0] = 0xC;
            } else if (func_800B7128(this)) {
                func_8084FD7C(play, this, &rideActor->actor);
            }
        }
    }

    if (this->csAction == PLAYER_CSACTION_END) {
        this->csAction = PLAYER_CSACTION_NONE;
    }
}

AnimSfxEntry D_8085D708[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 0, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 10, NA_SE_PL_GET_OFF_HORSE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 25, NA_SE_PL_SLIPDOWN, STOP),
};

void Player_Action_53(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_40;
    func_80847E2C(this, 1.0f, 10.0f);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Actor* rideActor = this->rideActor;

        Camera_ChangeSetting(Play_GetCamera(play, CAM_ID_MAIN), CAM_SET_NORMAL0);
        func_80839E74(this, play);

        this->stateFlags1 &= ~PLAYER_STATE1_800000;
        this->actor.parent = NULL;
        gHorseIsMounted = false;

        if (CHECK_QUEST_ITEM(QUEST_SONG_EPONA) || (DREG(1) != 0)) {
            gSaveContext.save.saveInfo.horseData.sceneId = play->sceneId;
            gSaveContext.save.saveInfo.horseData.pos.x = rideActor->world.pos.x;
            gSaveContext.save.saveInfo.horseData.pos.y = rideActor->world.pos.y;
            gSaveContext.save.saveInfo.horseData.pos.z = rideActor->world.pos.z;
            gSaveContext.save.saveInfo.horseData.yaw = rideActor->shape.rot.y;
        }
    } else {
        if (this->mountSide < 0) {
            D_8085D708[0].flags = ANIMSFX_FLAGS(ANIMSFX_TYPE_FLOOR_LAND, 40, CONTINUE);
        } else {
            D_8085D708[0].flags = ANIMSFX_FLAGS(ANIMSFX_TYPE_FLOOR_LAND, 29, CONTINUE);
        }

        Player_PlayAnimSfx(this, D_8085D708);
    }
}

s32 func_80850734(PlayState* play, Player* this) {
    if ((this->transformation == PLAYER_FORM_ZORA) && (this->windSpeed == 0.0f) &&
        (this->currentBoots < PLAYER_BOOTS_ZORA_UNDERWATER) && CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A)) {
        func_8083B850(play, this);
        this->stateFlags2 |= PLAYER_STATE2_400;
        PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_pz_waterroll, PLAYER_ANIM_ADJUSTED_SPEED, 4.0f,
                               Animation_GetLastFrame(&gPlayerAnim_pz_waterroll), ANIMMODE_ONCE, -6.0f);
        this->av2.actionVar2 = 5;
        this->unk_B86[0] = 0;
        this->unk_B48 = this->speedXZ;
        this->actor.velocity.y = 0.0f;
        Player_PlaySfx(this, NA_SE_PL_ZORA_SWIM_DASH);
        return true;
    }
    return false;
}

s32 func_80850854(PlayState* play, Player* this) {
    if ((this->transformation == PLAYER_FORM_DEKU) && (this->remainingHopsCounter != 0) &&
        (gSaveContext.save.saveInfo.playerData.health != 0) && (sControlStickMagnitude != 0.0f)) {
        func_808373F8(play, this, 0);
        return true;
    }
    return false;
}

void Player_Action_54(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_20;

    Player_Anim_PlayLoopOnceFinished(play, this, &gPlayerAnim_link_swimer_swim_wait);
    func_808475B4(this);

    if (this->av2.actionVar2 != 0) {
        this->av2.actionVar2--;
    }

    func_8082F164(this, BTN_R);

    if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A)) {
        this->av2.actionVar2 = 0;
    }

    if (!Player_IsTalking(play) && !Player_TryActionHandlerList(play, this, sActionHandlerList11, true) &&
        !func_8083B3B4(play, this, sPlayerControlInput) &&
        ((this->av2.actionVar2 != 0) || !func_80850734(play, this))) {
        speedTarget = 0.0f;
        yawTarget = this->actor.shape.rot.y;

        if (this->unk_AA5 > PLAYER_UNKAA5_2) {
            this->unk_AA5 = PLAYER_UNKAA5_0;
        }

        if (this->currentBoots >= PLAYER_BOOTS_ZORA_UNDERWATER) {
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                func_80836A98(this, D_8085BE84[PLAYER_ANIMGROUP_short_landing][this->modelAnimType], play);
                Player_AnimSfx_PlayFloorLand(this);
            }
        } else if (!func_80850854(play, this)) {
            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

            if (speedTarget != 0.0f) {
                if ((ABS_ALT(BINANG_SUB(this->actor.shape.rot.y, yawTarget)) > 0x6000) &&
                    !Math_StepToF(&this->speedXZ, 0.0f, 1.0f)) {
                    return;
                }

                if (Player_IsZTargetingWithHostileUpdate(this) || func_80847ED4(this)) {
                    func_80848048(play, this);
                } else {
                    func_8083B73C(play, this, yawTarget);
                }
            }
        }

        func_8084748C(this, &this->speedXZ, speedTarget, yawTarget);
        func_80847F1C(this);
    }
}

void Player_Action_55(Player* this, PlayState* play) {
    if (!Player_ActionHandler_13(this, play)) {
        this->stateFlags2 |= PLAYER_STATE2_20;
        func_808477D0(play, this, NULL, this->speedXZ);
        func_808475B4(this);

        if (DECR(this->av2.actionVar2) == 0) {
            func_808353DC(play, this);
        }
    }
}

void func_80850BA8(Player* this) {
    this->speedXZ = Math_CosS(this->unk_AAA) * this->unk_B48;
    this->actor.velocity.y = -Math_SinS(this->unk_AAA) * this->unk_B48;
}

void func_80850BF8(Player* this, f32 arg1) {
    f32 temp_fv0;
    s16 temp_ft0;
    s8 stickX = sPlayerControlInput->rel.stick_x;

    stickX *= GameInteractor_InvertControl(GI_INVERT_ZORA_SWIM_X);

    Math_AsymStepToF(&this->unk_B48, arg1, 1.0f, (fabsf(this->unk_B48) * 0.01f) + 0.4f);
    temp_fv0 = Math_CosS(stickX * 0x10E);

    temp_ft0 = (((stickX >= 0) ? 1 : -1) * (1.0f - temp_fv0) * -1100.0f);
    temp_ft0 = CLAMP(temp_ft0, -0x1F40, 0x1F40);

    this->yaw += temp_ft0;
}

void func_80850D20(PlayState* play, Player* this) {
    func_8083F8A8(play, this, 12.0f, -1, 1.0f, 160, 20, true);
}

void Player_Action_56(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 sp42;
    s16 yawTarget;
    s16 sp3E;
    s16 sp3C;
    s16 sp3A;
    s8 stickX = sPlayerControlInput->rel.stick_x;
    s8 stickY = sPlayerControlInput->rel.stick_y;

    stickX *= GameInteractor_InvertControl(GI_INVERT_ZORA_SWIM_X);
    stickY *= -GameInteractor_InvertControl(GI_INVERT_ZORA_SWIM_Y);

    this->stateFlags2 |= PLAYER_STATE2_20;

    func_808475B4(this);
    func_8082F164(this, BTN_R);

    if (Player_TryActionHandlerList(play, this, sActionHandlerList11, false)) {
        return;
    }

    if (func_8083B3B4(play, this, sPlayerControlInput)) {
        return;
    }

    if (func_80840A30(play, this, &this->speedXZ, 0.0f)) {
        return;
    }

    speedTarget = 0.0f;

    if (this->av2.actionVar2 != 0) {
        if ((!func_8082DA90(play) && !CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A)) ||
            (this->currentBoots != PLAYER_BOOTS_ZORA_LAND)) {
            this->unk_B86[0] = 1;
        }

        if (PlayerAnimation_Update(play, &this->skelAnime) && (DECR(this->av2.actionVar2) == 0)) {
            if (this->unk_B86[0] != 0) {
                this->stateFlags3 &= ~PLAYER_STATE3_8000;
                Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_pz_swimtowait);
            } else {
                Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_pz_fishswim);
            }
        } else {
            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
            Math_ScaledStepToS(&this->yaw, yawTarget, 0x640);

            if (this->skelAnime.curFrame >= 13.0f) {
                speedTarget = 12.0f;

                if (PlayerAnimation_OnFrame(&this->skelAnime, 13.0f)) {
                    this->unk_B48 = 16.0f;
                }
                this->stateFlags3 |= PLAYER_STATE3_8000;
            } else {
                speedTarget = 0.0f;
            }
        }

        Math_SmoothStepToS(&this->unk_B86[1], stickX * 0xC8, 0xA, 0x3E8, 0x64);
        Math_SmoothStepToS(&this->unk_B8E, this->unk_B86[1], IREG(40) + 1, IREG(41), IREG(42));
    } else if (this->unk_B86[0] == 0) {
        PlayerAnimation_Update(play, &this->skelAnime);

        if ((!func_8082DA90(play) && !CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A)) ||
            (this->currentBoots != PLAYER_BOOTS_ZORA_LAND) || (this->windSpeed > 9.0f)) {
            this->stateFlags3 &= ~PLAYER_STATE3_8000;
            Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_pz_swimtowait);
            this->unk_B86[0] = 1;
        } else {
            speedTarget = 9.0f;
            Actor_PlaySfx_Flagged2(&this->actor, NA_SE_PL_ZORA_SWIM_LV - SFX_FLAG);
        }

        // Y
        sp3E = stickY * 0xC8;
        if (this->unk_B8C != 0) {
            this->unk_B8C--;
            sp3E = CLAMP_MAX(sp3E, (s16)(this->floorPitch - 0xFA0));
        }

        if ((this->unk_AAA >= -0x1555) && (this->actor.depthInWater < (this->ageProperties->unk_24 + 10.0f))) {
            sp3E = CLAMP_MIN(sp3E, 0x7D0);
        }
        Math_SmoothStepToS(&this->unk_AAA, sp3E, 4, 0xFA0, 0x190);

        // X
        sp42 = stickX * 0x64;
        if (Math_ScaledStepToS(&this->unk_B8A, sp42, 0x384) && (sp42 == 0)) {
            Math_SmoothStepToS(&this->unk_B86[1], 0, 4, 0x5DC, 0x64);
            Math_SmoothStepToS(&this->unk_B8E, this->unk_B86[1], IREG(44) + 1, IREG(45), IREG(46));
        } else {
            sp3C = this->unk_B86[1];
            sp3A = (this->unk_B8A < 0) ? -0x3A98 : 0x3A98;
            this->unk_B86[1] += this->unk_B8A;
            Math_SmoothStepToS(&this->unk_B8E, this->unk_B86[1], IREG(47) + 1, IREG(48), IREG(49));

            if ((ABS_ALT(this->unk_B8A) > 0xFA0) && ((((sp3C + this->unk_B8A) - sp3A) * (sp3C - sp3A)) <= 0)) {
                Player_PlaySfx(this, NA_SE_PL_ZORA_SWIM_ROLL);
            }
        }

        if (sPlayerYDistToFloor < 20.0f) {
            func_80850D20(play, this);
        }
    } else {
        Math_SmoothStepToS(&this->unk_B86[1], 0, 4, 0xFA0, 0x190);
        if ((this->skelAnime.curFrame <= 5.0f) || !func_80850734(play, this)) {
            if (PlayerAnimation_Update(play, &this->skelAnime)) {
                func_808353DC(play, this);
            }
        }

        Player_ResetCylinder(this);
    }

    if ((this->unk_B8C < 8) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        DynaPolyActor* dynaActor;

        if ((this->actor.floorBgId == BGCHECK_SCENE) ||
            ((dynaActor = DynaPoly_GetActor(&play->colCtx, this->actor.floorBgId)) == NULL) ||
            (dynaActor->actor.id != ACTOR_EN_TWIG)) {
            this->unk_AAA += (s16)((-this->floorPitch - this->unk_AAA) * 2);
            this->unk_B8C = 0xF;
        }

        func_80850D20(play, this);
        Player_PlaySfx(this, NA_SE_PL_BODY_BOUND);
    }

    func_80850BF8(this, speedTarget);
    func_80850BA8(this);
}

void Player_Action_57(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s16 sp30;
    s16 var_v0;

    this->stateFlags2 |= PLAYER_STATE2_20;
    func_808475B4(this);
    func_8082F164(this, BTN_R);
    if (!Player_TryActionHandlerList(play, this, sActionHandlerList11, true) &&
        !func_8083B3B4(play, this, sPlayerControlInput) && !func_80850854(play, this)) {
        func_808477D0(play, this, sPlayerControlInput, this->speedXZ);
        if (func_8082DA90(play)) {
            speedTarget = this->speedXZ;
            yawTarget = this->actor.shape.rot.y;
        } else {
            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        }
        sp30 = this->actor.shape.rot.y - yawTarget;
        if (!func_80850734(play, this)) {
            if (Player_IsZTargetingWithHostileUpdate(this) || func_80847ED4(this)) {
                func_80848048(play, this);
            } else {
                if ((speedTarget == 0.0f) || (ABS_ALT(sp30) > 0x6000) ||
                    (this->currentBoots >= PLAYER_BOOTS_ZORA_UNDERWATER)) {
                    func_808353DC(play, this);
                }
            }
            func_80847FF8(this, &this->speedXZ, speedTarget, yawTarget);
        }
    }
}

void Player_Action_58(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    func_808477D0(play, this, sPlayerControlInput, this->speedXZ);
    func_808475B4(this);
    func_8082F164(this, BTN_R);

    if (!Player_TryActionHandlerList(play, this, sActionHandlerList11, true) &&
        !func_8083B3B4(play, this, sPlayerControlInput)) {
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (speedTarget == 0.0f) {
            func_808353DC(play, this);
        } else if (!Player_IsZTargetingWithHostileUpdate(this) && !func_80847ED4(this)) {
            func_8083B73C(play, this, yawTarget);
        } else {
            func_80848094(play, this, &speedTarget, &yawTarget);
        }

        func_80847FF8(this, &this->speedXZ, speedTarget, yawTarget);
    }
}

void Player_Action_59(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20;

    this->actor.gravity = 0.0f;
    Player_UpdateUpperBody(this, play);
    func_8082F164(this, BTN_R);

    if (Player_ActionHandler_13(this, play)) {
        return;
    }

    if (this->currentBoots >= PLAYER_BOOTS_ZORA_UNDERWATER) {
        func_808353DC(play, this);
    } else if (this->av1.actionVar1 == 0) {
        f32 temp_fv0;

        if (this->av2.actionVar2 == 0) {
            if (PlayerAnimation_Update(play, &this->skelAnime) ||
                ((this->skelAnime.curFrame >= 22.0f) && !CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A))) {
                func_8083B798(play, this);
            } else if (PlayerAnimation_OnFrame(&this->skelAnime, 20.0f)) {
                this->actor.velocity.y = -2.0f;
            }
            Player_DecelerateToZero(this);
        } else {
            func_808477D0(play, this, sPlayerControlInput, this->actor.velocity.y);
            this->unk_AAA = 0x3E80;

            if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A) && !Player_ActionHandler_2(this, play) &&
                !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.depthInWater < 120.0f)) {
                func_808481CC(play, this, -2.0f);
            } else {
                this->av1.actionVar1++;
                Player_Anim_PlayLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
            }
        }

        temp_fv0 = (this->actor.depthInWater - this->ageProperties->unk_30) * 0.04f;
        if (temp_fv0 < this->actor.velocity.y) {
            this->actor.velocity.y = temp_fv0;
        }
    } else if (this->av1.actionVar1 == 1) {
        PlayerAnimation_Update(play, &this->skelAnime);
        func_808475B4(this);
        if (this->unk_AAA < 0x2710) {
            this->av1.actionVar1++;
            this->av2.actionVar2 = this->actor.depthInWater;
            Player_Anim_PlayLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim);
        }
    } else if (!func_8083B3B4(play, this, sPlayerControlInput)) {
        f32 var_fv1 = (this->av2.actionVar2 * 0.018f) + 4.0f;

        if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
            sPlayerControlInput = NULL;
        }

        func_808477D0(play, this, sPlayerControlInput, fabsf(this->actor.velocity.y));
        Math_ScaledStepToS(&this->unk_AAA, -0x2710, 0x320);

        var_fv1 = CLAMP_MAX(var_fv1, 8.0f);
        func_808481CC(play, this, var_fv1);
    }
}

void Player_Action_60(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20;

    func_8082F164(this, BTN_R);
    if (((this->stateFlags1 & PLAYER_STATE1_400) || (this->skelAnime.curFrame <= 1.0f) || !func_80850734(play, this)) &&
        PlayerAnimation_Update(play, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_400) || func_808482E0(play, this)) {
            func_80848250(play, this);
            func_808353DC(play, this);
            func_8082DC64(play, this);
        }
    } else {
        if ((this->stateFlags1 & PLAYER_STATE1_400) && PlayerAnimation_OnFrame(&this->skelAnime, 10.0f)) {
            func_8082ECE0(this);
            func_8082DC64(play, this);
        } else if (PlayerAnimation_OnFrame(&this->skelAnime, 5.0f)) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_BREATH_DRINK);
        }
    }

    func_808475B4(this);
    func_8084748C(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
}

void Player_Action_61(Player* this, PlayState* play) {
    func_808475B4(this);
    Math_StepToF(&this->speedXZ, 0.0f, 0.4f);
    if (PlayerAnimation_Update(play, &this->skelAnime) && (this->speedXZ < 10.0f)) {
        func_808353DC(play, this);
    }
}

void Player_Action_62(Player* this, PlayState* play) {
    func_808475B4(this);
    if (PlayerAnimation_Update(play, &this->skelAnime) && (this == GET_PLAYER(play))) {
        func_80840770(play, this);
    }
    func_8084748C(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
}

bool func_80851C40(PlayState* play, Player* this) {
    return ((play->sceneId == SCENE_MILK_BAR) && Audio_IsSequencePlaying(NA_BGM_BALLAD_OF_THE_WIND_FISH)) ||
           (((play->sceneId != SCENE_MILK_BAR) && (this->csAction == PLAYER_CSACTION_68)) ||
            ((play->msgCtx.msgMode == MSGMODE_SONG_PLAYED) ||
             (play->msgCtx.msgMode == MSGMODE_SETUP_DISPLAY_SONG_PLAYED) ||
             (play->msgCtx.msgMode == MSGMODE_DISPLAY_SONG_PLAYED) ||
             ((play->msgCtx.ocarinaMode != OCARINA_MODE_ACTIVE) &&
              ((this->csAction == PLAYER_CSACTION_5) || (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) ||
               play->msgCtx.ocarinaAction == OCARINA_ACTION_FREE_PLAY_DONE))));
}

// Deku playing the pipes? The loops both overwrite unk_AF0[0].y,z and unk_AF0[1].x,y,z
void func_80851D30(PlayState* play, Player* this) {
    f32* var_s0 = &this->unk_AF0[0].y; // TODO: what is going on around here in the struct?
    Vec3f sp50;

    if (func_80851C40(play, this)) {
        s32 i;

        if (this->skelAnime.mode != ANIMMODE_LOOP) {
            Player_Anim_PlayLoopAdjusted(play, this, D_8085D190[this->transformation]);
        }
        func_80124618(D_801C03A0, this->skelAnime.curFrame, &sp50);

        for (i = 0; i < 5; i++) {
            *var_s0 = sp50.x;
            var_s0++;
        }
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE) {
        if (play->msgCtx.ocarinaButtonIndex != OCARINA_BTN_INVALID) {
            var_s0[play->msgCtx.ocarinaButtonIndex] = 1.2f;
            Player_Anim_PlayOnceAdjusted(play, this, D_8085D190[this->transformation]);
        } else {
            s32 i;

            for (i = 0; i < 5; i++) {
                Math_StepToF(var_s0++, 1.0f, 0.04000001f);
            }
        }
    }
}

void func_80851EAC(Player* this) {
    this->unk_B86[0] = -1;
    this->unk_B86[1] = -1;
    this->unk_B10[0] = 0.0f;
}

struct_8085D714 D_8085D714[] = {
    { 1, &gPlayerAnim_pg_gakkiplayA }, { 1, &gPlayerAnim_pg_gakkiplayL }, { 1, &gPlayerAnim_pg_gakkiplayD },
    { 0, &gPlayerAnim_pg_gakkiplayU }, { 0, &gPlayerAnim_pg_gakkiplayR },
};

void func_80851EC8(PlayState* play, Player* this) {
    struct_8085D714* temp3 = &D_8085D714[play->msgCtx.ocarinaButtonIndex];
    f32* temp2 = &this->unk_B10[play->msgCtx.ocarinaButtonIndex];
    s16* temp_a3 = &this->unk_B86[temp3->unk_0];

    temp_a3[0] = play->msgCtx.ocarinaButtonIndex;
    temp2[0] = 3.0f;
}

void func_80851F18(PlayState* play, Player* this) {
    struct_8085D714* temp;
    f32* temp_v0;
    s32 i;

    i = this->unk_B86[0];
    if (i >= 0) {
        temp = &D_8085D714[i];
        i = 0;
        temp_v0 = &this->unk_B10[this->unk_B86[i]];

        AnimTaskQueue_AddLoadPlayerFrame(play, temp->unk_4, *temp_v0, this->skelAnime.limbCount,
                                         this->skelAnime.morphTable);
        AnimTaskQueue_AddCopyUsingMap(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                      this->skelAnime.morphTable, D_8085BA08);
    }
    i = this->unk_B86[1];
    if (i >= 0) {
        temp = &D_8085D714[i];
        i = 1;
        temp_v0 = &this->unk_B10[this->unk_B86[i]];

        AnimTaskQueue_AddLoadPlayerFrame(play, temp->unk_4, *temp_v0, this->skelAnime.limbCount,
                                         (void*)ALIGN16((uintptr_t)this->blendTableBuffer));
        AnimTaskQueue_AddCopyUsingMap(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                      (void*)ALIGN16((uintptr_t)this->blendTableBuffer), D_8085BA20);
    }

    temp_v0 = this->unk_B10;
    for (i = 0; i < 5; i++) {
        *temp_v0 += 1.0f;
        if (*temp_v0 >= 9.0f) {
            *temp_v0 = 8.0f;
            if (this->unk_B86[0] == i) {
                this->unk_B86[0] = -1;
            } else if (this->unk_B86[1] == i) {
                this->unk_B86[1] = -1;
            }
        }
        temp_v0++;
    }
}

// Goron playing the drums?
void func_808521E0(PlayState* play, Player* this) {
    if (func_80851C40(play, this)) {
        if (!BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pg_gakkiplay)) {
            Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_pg_gakkiplay);
        }

        func_80124618(D_801C0490, this->skelAnime.curFrame, &this->unk_AF0[1]);
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE) {
        if (play->msgCtx.ocarinaButtonIndex != OCARINA_BTN_INVALID) {
            func_80851EC8(play, this);
        }

        func_80851F18(play, this);
    }
}

// Zora playing the guitar?
void func_80852290(PlayState* play, Player* this) {
    if (func_80851C40(play, this)) {
        if (this->skelAnime.mode != ANIMMODE_LOOP) {
            Player_Anim_PlayLoopAdjusted(play, this, D_8085D190[this->transformation]);
        }

        this->unk_B8A = 8;
    } else {
        f32 sp3C;
        s16 upperLimbRotX;
        s16 sp38;

        if ((play->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE) &&
            (play->msgCtx.ocarinaButtonIndex != OCARINA_BTN_INVALID)) {
            if ((this->ocarinaInteractionActor != NULL) && (this->ocarinaInteractionDistance < 0.0f)) {
                // Designed for tuning the guitar in zora hall for the zora: `ACTOR_EN_ZOT`
                // This actor will delay setting the `ACTOR_FLAG_OCARINA_INTERACTION` until here.
                // This is signaled by a negative `ocarinaInteractionDistance`.
                this->ocarinaInteractionActor->flags |= ACTOR_FLAG_OCARINA_INTERACTION;
                this->ocarinaInteractionDistance = 0.0f;
            }

            Player_Anim_PlayOnceAdjusted(play, this, D_8085D190[this->transformation]);
            this->unk_B8A = 8;
        }

        sPlayerControlInput = play->state.input;
        Lib_GetControlStickData(&sp3C, &sp38, sPlayerControlInput);

        if (BINANG_ADD(sp38, 0x4000) < 0) {
            sp38 -= 0x8000;
            sp3C = -sp3C;
        }

        if (sp38 < -0x1F40) {
            sp38 = -0x1F40;
        } else if (sp38 > 0x2EE0) {
            sp38 = 0x2EE0;
        }

        upperLimbRotX = (sp3C * -100.0f);
        upperLimbRotX = CLAMP_MAX(upperLimbRotX, 0xFA0);
        Math_SmoothStepToS(&this->upperLimbRot.x, upperLimbRotX, 4, 0x7D0, 0);
        Math_SmoothStepToS(&this->upperLimbRot.y, sp38, 4, 0x7D0, 0);
        this->headLimbRot.x = -this->upperLimbRot.x;
        this->unk_AA6_rotFlags |= UNKAA6_ROT_HEAD_X | UNKAA6_ROT_UPPER_X | UNKAA6_ROT_UPPER_Y;

        upperLimbRotX = ABS_ALT(this->upperLimbRot.x);
        if (upperLimbRotX < 0x7D0) {
            this->actor.shape.face = PLAYER_FACE_NEUTRAL;
        } else if (upperLimbRotX < 0xFA0) {
            this->actor.shape.face = PLAYER_FACE_OPENING;
        } else {
            this->actor.shape.face = PLAYER_FACE_HURT;
        }
    }

    if (DECR(this->unk_B8A) != 0) {
        this->unk_B86[0] += TRUNCF_BINANG(this->upperLimbRot.x * 2.5f);
        this->unk_B86[1] += TRUNCF_BINANG(this->upperLimbRot.y * 3.0f);
    } else {
        this->unk_B86[0] = 0;
        this->unk_B86[1] = 0;
    }
}

void func_8085255C(PlayState* play, Player* this) {
    if (this->transformation == PLAYER_FORM_DEKU) {
        func_80851D30(play, this);
    } else if (this->transformation == PLAYER_FORM_GORON) {
        func_808521E0(play, this);
    } else if (this->transformation == PLAYER_FORM_ZORA) {
        func_80852290(play, this);
    }
}

void func_808525C4(PlayState* play, Player* this) {
    if (this->av2.actionVar2++ >= 3) {
        if ((this->transformation == PLAYER_FORM_ZORA) || (this->transformation == PLAYER_FORM_DEKU)) {
            Player_Anim_PlayOnceFreeze(play, this, D_8085D190[this->transformation]);
        } else if (this->transformation == PLAYER_FORM_GORON) {
            func_80851EAC(this);
            Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_pg_gakkiwait);
        } else {
            Player_Anim_PlayLoopAdjusted(play, this, D_8085D190[this->transformation]);
        }

        this->unk_B48 = 1.0f;
    }
}

void Player_Action_63(Player* this, PlayState* play) {
    if ((this->unk_AA5 != PLAYER_UNKAA5_4) &&
        ((PlayerAnimation_Update(play, &this->skelAnime) &&
          (BEN_ANIM_EQUAL(this->skelAnime.animation, D_8085D17C[this->transformation]))) ||
         ((this->skelAnime.mode == ANIMMODE_LOOP) && (this->av2.actionVar2 == 0)))) {
        func_808525C4(play, this);
        if (!CVarGetInteger("gEnhancements.Playback.NoDropOcarinaInput", 0) || this->av2.actionVar2 == 1) {
            if (!(this->actor.flags & ACTOR_FLAG_OCARINA_INTERACTION) ||
                (this->ocarinaInteractionActor->id == ACTOR_EN_ZOT)) {
                Message_DisplayOcarinaStaff(play, OCARINA_ACTION_FREE_PLAY);
            }
        }
    } else if (this->av2.actionVar2 != 0) {
        if (play->msgCtx.ocarinaMode == OCARINA_MODE_END) {
            play->interfaceCtx.bButtonInterfaceDoActionActive = false;
            CutsceneManager_Stop(play->playerCsIds[PLAYER_CS_ID_ITEM_OCARINA]);
            this->actor.flags &= ~ACTOR_FLAG_OCARINA_INTERACTION;

            if ((this->talkActor != NULL) && (this->talkActor == this->ocarinaInteractionActor) &&
                (this->ocarinaInteractionDistance >= 0.0f)) {
                Player_StartTalking(play, this->talkActor);
            } else if (this->tatlTextId < 0) {
                this->talkActor = this->tatlActor;
                this->tatlActor->textId = -this->tatlTextId;
                Player_StartTalking(play, this->talkActor);
            } else if (!Player_ActionHandler_13(this, play)) {
                func_80836A5C(this, play);
                Player_Anim_PlayOnceAdjustedReverse(play, this, D_8085D17C[this->transformation]);
            }
        } else {
            s32 var_v1 = (play->msgCtx.ocarinaMode >= OCARINA_MODE_WARP_TO_GREAT_BAY_COAST) &&
                         (play->msgCtx.ocarinaMode <= OCARINA_MODE_WARP_TO_ENTRANCE);
            s32 pad[2];

            if (var_v1 || (play->msgCtx.ocarinaMode == OCARINA_MODE_APPLY_SOT) ||
                (play->msgCtx.ocarinaMode == OCARINA_MODE_APPLY_DOUBLE_SOT) ||
                (play->msgCtx.ocarinaMode == OCARINA_MODE_APPLY_INV_SOT_FAST) ||
                (play->msgCtx.ocarinaMode == OCARINA_MODE_APPLY_INV_SOT_SLOW)) {
                if (play->msgCtx.ocarinaMode == OCARINA_MODE_APPLY_SOT) {
                    if (!func_8082DA90(play)) {
                        if (gSaveContext.save.saveInfo.playerData.threeDayResetCount == 1) {
                            play->nextEntrance = ENTRANCE(CUTSCENE, 1);
                        } else {
                            play->nextEntrance = ENTRANCE(CUTSCENE, 0);
                        }

                        gSaveContext.nextCutsceneIndex = 0xFFF7;
                        play->transitionTrigger = TRANS_TRIGGER_START;
                    }
                } else {
                    Actor* actor;

                    play->interfaceCtx.bButtonInterfaceDoActionActive = false;
                    CutsceneManager_Stop(play->playerCsIds[PLAYER_CS_ID_ITEM_OCARINA]);
                    this->actor.flags &= ~ACTOR_FLAG_OCARINA_INTERACTION;

                    actor = Actor_Spawn(&play->actorCtx, play, var_v1 ? ACTOR_EN_TEST7 : ACTOR_EN_TEST6,
                                        this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0,
                                        0, play->msgCtx.ocarinaMode);
                    if (actor != NULL) {
                        this->stateFlags1 &= ~PLAYER_STATE1_20000000;
                        this->csAction = PLAYER_CSACTION_NONE;
                        Player_TryCsAction(play, NULL, PLAYER_CSACTION_19);
                        this->stateFlags1 |= PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000;
                    } else {
                        func_80836A5C(this, play);
                        Player_Anim_PlayOnceAdjustedReverse(play, this, D_8085D17C[this->transformation]);
                    }
                }
            } else if ((play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
                       (play->msgCtx.lastPlayedSong == OCARINA_SONG_ELEGY)) {
                play->interfaceCtx.bButtonInterfaceDoActionActive = false;
                CutsceneManager_Stop(play->playerCsIds[PLAYER_CS_ID_ITEM_OCARINA]);

                this->actor.flags &= ~ACTOR_FLAG_OCARINA_INTERACTION;
                Player_SetAction_PreserveItemAction(play, this, Player_Action_88, 0);
                this->stateFlags1 |= PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000;
            } else if (this->unk_AA5 == PLAYER_UNKAA5_4) {
                f32 temp_fa0 = this->skelAnime.jointTable[LIMB_ROOT_POS].x;
                f32 temp_fa1 = this->skelAnime.jointTable[LIMB_ROOT_POS].z;
                f32 var_fv1;

                var_fv1 = sqrtf(SQ(temp_fa0) + SQ(temp_fa1));
                if (var_fv1 != 0.0f) {
                    var_fv1 = (var_fv1 - 100.0f) / var_fv1;
                    var_fv1 = CLAMP_MIN(var_fv1, 0.0f);
                }

                this->skelAnime.jointTable[LIMB_ROOT_POS].x = temp_fa0 * var_fv1;
                this->skelAnime.jointTable[LIMB_ROOT_POS].z = temp_fa1 * var_fv1;
            } else {
                func_8085255C(play, this);
            }
        }
    }
}

void Player_Action_64(Player* this, PlayState* play) {
    Player_DecelerateToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        func_80836A98(this, &gPlayerAnim_link_normal_light_bom_end, play);
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, 3.0f)) {
        if (Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ARROW, this->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].x,
                        this->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].y,
                        this->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].z, 0xFA0, this->actor.shape.rot.y, 0,
                        ARROW_TYPE_DEKU_NUT) != NULL) {
            Inventory_ChangeAmmo(ITEM_DEKU_NUT, -1);
            this->unk_D57 = 4;
        }

        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
    }
}

AnimSfxEntry D_8085D73C[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_JUMP, 87, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 87, NA_SE_VO_LI_CLIMB_END, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 69, NA_SE_VO_LI_AUTO_JUMP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 123, NA_SE_NONE, STOP),
};

AnimSfxEntry D_8085D74C[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 13, NA_SE_VO_LI_AUTO_JUMP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_JUMP, 13, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 73, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 120, NA_SE_NONE, STOP),
};

void Player_Action_65(Player* this, PlayState* play) {
    func_8083249C(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->av2.actionVar2 != 0) {
            if (this->av2.actionVar2 > 1) {
                this->av2.actionVar2--;
            }

            if (func_808482E0(play, this) && (this->av2.actionVar2 == 1)) {
                Player_SetModels(this, Player_ActionToModelGroup(this, this->itemAction));

                if ((this->getItemDrawIdPlusOne == GID_REMAINS_ODOLWA + 1) ||
                    (this->getItemDrawIdPlusOne == GID_REMAINS_GOHT + 1) ||
                    (this->getItemDrawIdPlusOne == GID_REMAINS_GYORG + 1) ||
                    (this->getItemDrawIdPlusOne == GID_REMAINS_TWINMOLD + 1)) {
                    Player_StopCutscene(this);
                    func_80848250(play, this);
                    this->stateFlags1 &= ~PLAYER_STATE1_20000000;
                    Player_TryCsAction(play, NULL, PLAYER_CSACTION_93);
                } else {
                    s32 var_a2 = ((this->talkActor != NULL) && (this->exchangeItemAction <= PLAYER_IA_MINUS1)) ||
                                 (this->stateFlags3 & PLAYER_STATE3_20);

                    if (var_a2 || (gSaveContext.healthAccumulator == 0)) {
                        Player_StopCutscene(this);
                        if (var_a2) {
                            func_80848250(play, this);
                            this->exchangeItemAction = PLAYER_IA_NONE;
                            if (!func_80847994(play, this)) {
                                Player_StartTalking(play, this->talkActor);
                            }
                        } else {
                            func_80848294(play, this);
                        }
                    }
                }
            }
        } else {
            Player_Anim_ResetMove(this);

            if ((this->getItemId == GI_STRAY_FAIRY) || (this->getItemId == GI_SKULL_TOKEN) ||
                (this->getItemId == GI_ICE_TRAP)) {
                Player_StopCutscene(this);
                this->stateFlags1 &= ~(PLAYER_STATE1_400 | PLAYER_STATE1_CARRYING_ACTOR);
                if (this->getItemId == GI_STRAY_FAIRY) {
                    func_80839E74(this, play);
                } else {
                    this->actor.colChkInfo.damage = 0;
                    func_80833B18(play, this, 3, 0.0f, 0.0f, 0, 20);
                }
            } else {
                if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_box_kick)) {
                    Player_Anim_PlayOnceAdjusted(play, this,
                                                 (this->transformation == PLAYER_FORM_DEKU)
                                                     ? &gPlayerAnim_pn_getB
                                                     : &gPlayerAnim_link_demo_get_itemB);
                } else {
                    Player_Anim_PlayOnceAdjusted(play, this,
                                                 (this->transformation == PLAYER_FORM_DEKU)
                                                     ? &gPlayerAnim_pn_getA
                                                     : &gPlayerAnim_link_demo_get_itemA);
                }

                Player_AnimReplace_Setup(play, this,
                                         ANIM_FLAG_1 | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_4 | ANIM_FLAG_ENABLE_MOVEMENT |
                                             ANIM_FLAG_NOMOVE | ANIM_FLAG_80);
                Player_StopCutscene(this);
                this->csId = play->playerCsIds[PLAYER_CS_ID_ITEM_GET];
                this->av2.actionVar2 = 2;
            }
        }
    } else if (this->av2.actionVar2 == 0) {
        if (this->transformation == PLAYER_FORM_HUMAN) {
            Player_PlayAnimSfx(this, D_8085D73C);
        } else if (this->transformation == PLAYER_FORM_DEKU) {
            Player_PlayAnimSfx(this, D_8085D74C);
        }
    } else {
        if ((BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_demo_get_itemB)) ||
            (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pn_getB))) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, BINANG_ADD(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)), 0x8000),
                               0xFA0);
        } else if ((BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pn_getA)) &&
                   PlayerAnimation_OnFrame(&this->skelAnime, 10.0f)) {
            Player_AnimSfx_PlayFloorLand(this);
        }

        if (PlayerAnimation_OnFrame(&this->skelAnime, 21.0f)) {
            func_8082ECE0(this);
        }
    }
}

void Player_Action_TimeTravelEnd(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (!this->av1.startedAnim) {
            if (DECR(this->av2.animDelayTimer) == 0) {
                this->av1.startedAnim = true;

                // endFrame was previously set to 0 to freeze the animation.
                // Set it properly to allow the animation to play.
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        } else {
            func_80839E74(this, play);
        }
    } else if ((this->transformation == PLAYER_FORM_FIERCE_DEITY) &&
               PlayerAnimation_OnFrame(&this->skelAnime, 158.0f)) {
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
    } else if (this->transformation != PLAYER_FORM_FIERCE_DEITY) {
        static AnimSfxEntry sJumpOffPedestalAnimSfxList[] = {
            ANIMSFX(ANIMSFX_TYPE_VOICE, 5, NA_SE_VO_LI_AUTO_JUMP, CONTINUE),
            ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 15, NA_SE_NONE, STOP),
        };

        Player_PlayAnimSfx(this, sJumpOffPedestalAnimSfxList);
    } else {
        func_808484CC(this);
    }
}

Vec3f D_8085D764 = { 0.0f, 24.0f, 19.0f };
Vec3f D_8085D770 = { 0.0f, 0.0f, 2.0f };
Vec3f D_8085D77C = { 0.0f, 0.0f, -0.2f };

Color_RGBA8 D_8085D788 = { 255, 255, 255, 255 };
Color_RGBA8 D_8085D78C = { 255, 255, 255, 255 };

void func_808530E0(PlayState* play, Player* this) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    Player_TranslateAndRotateY(this, &this->actor.world.pos, &D_8085D764, &pos);
    Player_TranslateAndRotateY(this, &gZeroVec3f, &D_8085D770, &velocity);
    Player_TranslateAndRotateY(this, &gZeroVec3f, &D_8085D77C, &accel);
    func_800B0EB0(play, &pos, &velocity, &accel, &D_8085D788, &D_8085D78C, 40, 10, 10);
}

u8 D_8085D790[] = {
    1,     // PLAYER_IA_BOTTLE_POTION_RED
    1 | 2, // PLAYER_IA_BOTTLE_POTION_BLUE
    2,     // PLAYER_IA_BOTTLE_POTION_GREEN
    4,     // PLAYER_IA_BOTTLE_MILK
    4,     // PLAYER_IA_BOTTLE_MILK_HALF
    1 | 2, // PLAYER_IA_BOTTLE_CHATEAU
};

void Player_Action_67(Player* this, PlayState* play) {
    if (GameInteractor_Should(VB_USE_BOTTLE_ITEM, true, this)) {
        func_808323C0(this, play->playerCsIds[PLAYER_CS_ID_ITEM_BOTTLE]);

        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            if (this->av2.actionVar2 == 0) {
                if (this->itemAction == PLAYER_IA_BOTTLE_POE) {
                    s32 health = Rand_S16Offset(-1, 3);

                    if (health == 0) {
                        health = 3;
                    }
                    if ((health < 0) && (gSaveContext.save.saveInfo.playerData.health <= 0x10)) {
                        health = 3;
                    }

                    if (health < 0) {
                        Health_ChangeBy(play, -0x10);
                    } else {
                        gSaveContext.healthAccumulator = health * 0x10;
                    }
                } else {
                    s32 temp_v1 = D_8085D790[this->itemAction - PLAYER_IA_BOTTLE_POTION_RED];

                    if (temp_v1 & 1) {
                        gSaveContext.healthAccumulator = 0x140;
                    }
                    if (temp_v1 & 2) {
                        Magic_Add(play, MAGIC_FILL_TO_CAPACITY);
                    }
                    if (temp_v1 & 4) {
                        gSaveContext.healthAccumulator = 0x50;
                    }

                    if (this->itemAction == PLAYER_IA_BOTTLE_CHATEAU) {
                        SET_WEEKEVENTREG(WEEKEVENTREG_DRANK_CHATEAU_ROMANI);
                    }

                    gSaveContext.jinxTimer = 0;
                }

                Player_Anim_PlayLoopAdjusted(play, this,
                                             (this->transformation == PLAYER_FORM_DEKU)
                                                 ? &gPlayerAnim_pn_drink
                                                 : &gPlayerAnim_link_bottle_drink_demo_wait);
                this->av2.actionVar2 = 1;

            } else if (this->av2.actionVar2 < 0) {
                this->av2.actionVar2++;
                if (this->av2.actionVar2 == 0) {
                    this->av2.actionVar2 = 3;
                    this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
                } else if (this->av2.actionVar2 == -6) {
                    func_808530E0(play, this);
                }
            } else {
                Player_StopCutscene(this);
                func_80839E74(this, play);
            }
        } else if (this->av2.actionVar2 == 1) {
            if ((gSaveContext.healthAccumulator == 0) && (gSaveContext.magicState != MAGIC_STATE_FILL)) {
                if (this->transformation == PLAYER_FORM_DEKU) {
                    PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_pn_drinkend, PLAYER_ANIM_ADJUSTED_SPEED,
                                           0.0f, 5.0f, 2, -6.0f);
                    this->av2.actionVar2 = -7;
                } else {
                    Player_Anim_PlayOnceMorphAdjusted(play, this, &gPlayerAnim_link_bottle_drink_demo_end);
                    this->av2.actionVar2 = 2;
                }

                Player_UpdateBottleHeld(play, this,
                                        (this->itemAction == PLAYER_IA_BOTTLE_MILK) ? ITEM_MILK_HALF : ITEM_BOTTLE,
                                        PLAYER_IA_BOTTLE_EMPTY);
            }

            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DRINK - SFX_FLAG);
        } else if ((this->av2.actionVar2 == 2) && PlayerAnimation_OnFrame(&this->skelAnime, 29.0f)) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_BREATH_DRINK);
        }
    }
}

#define BOTTLE_CATCH_PARAMS_ANY -1

struct_8085D798 D_8085D798[] = {
    { ACTOR_EN_ELF, FAIRY_PARAMS(FAIRY_TYPE_2, false, 0), ITEM_FAIRY, PLAYER_IA_BOTTLE_FAIRY, 0x5E },
    { ACTOR_EN_FISH, BOTTLE_CATCH_PARAMS_ANY, ITEM_FISH, PLAYER_IA_BOTTLE_FISH, 0x62 },
    { ACTOR_EN_INSECT, BOTTLE_CATCH_PARAMS_ANY, ITEM_BUG, PLAYER_IA_BOTTLE_BUG, 0x63 },
    { ACTOR_EN_MUSHI2, BOTTLE_CATCH_PARAMS_ANY, ITEM_BUG, PLAYER_IA_BOTTLE_BUG, 0x63 },
    { ACTOR_EN_TEST5, ENTEST5_PARAMS(false), ITEM_SPRING_WATER, PLAYER_IA_BOTTLE_SPRING_WATER, 0x67 },
    { ACTOR_EN_TEST5, ENTEST5_PARAMS(true), ITEM_HOT_SPRING_WATER, PLAYER_IA_BOTTLE_HOT_SPRING_WATER, 0x68 },
    { ACTOR_BG_GORON_OYU, BOTTLE_CATCH_PARAMS_ANY, ITEM_HOT_SPRING_WATER, PLAYER_IA_BOTTLE_HOT_SPRING_WATER, 0x68 },
    { ACTOR_EN_ZORAEGG, BOTTLE_CATCH_PARAMS_ANY, ITEM_ZORA_EGG, PLAYER_IA_BOTTLE_ZORA_EGG, 0x69 },
    { ACTOR_EN_DNP, BOTTLE_CATCH_PARAMS_ANY, ITEM_DEKU_PRINCESS, PLAYER_IA_BOTTLE_DEKU_PRINCESS, 0x5F },
    { ACTOR_EN_OT, BOTTLE_CATCH_PARAMS_ANY, ITEM_SEAHORSE, PLAYER_IA_BOTTLE_SEAHORSE, 0x6E },
    { ACTOR_OBJ_KINOKO, BOTTLE_CATCH_PARAMS_ANY, ITEM_MUSHROOM, PLAYER_IA_BOTTLE_SEAHORSE, 0x6B },
    { ACTOR_EN_POH, BOTTLE_CATCH_PARAMS_ANY, ITEM_POE, PLAYER_IA_BOTTLE_POE, 0x65 },
    { ACTOR_EN_BIGPO, BOTTLE_CATCH_PARAMS_ANY, ITEM_BIG_POE, PLAYER_IA_BOTTLE_BIG_POE, 0x66 },
    { ACTOR_EN_ELF, FAIRY_PARAMS(FAIRY_TYPE_6, false, 0), ITEM_FAIRY, PLAYER_IA_BOTTLE_FAIRY, 0x5E },
};

void Player_Action_68(Player* this, PlayState* play) {
    struct_8085D200* sp24 = &D_8085D200[this->av2.actionVar2];

    Player_DecelerateToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->av1.actionVar1 != 0) {
            func_808323C0(this, play->playerCsIds[PLAYER_CS_ID_ITEM_SHOW]);

            if (this->av2.actionVar2 == 0) {
                Message_StartTextbox(play, D_8085D798[this->av1.actionVar1 - 1].textId, &this->actor);

                Audio_PlayFanfare(NA_BGM_GET_ITEM);
                this->av2.actionVar2 = 1;
            } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                Actor* talkActor;

                this->av1.actionVar1 = 0;
                Player_StopCutscene(this);
                Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));

                talkActor = this->talkActor;
                if ((talkActor != NULL) && (this->exchangeItemAction <= PLAYER_IA_MINUS1)) {
                    Player_StartTalking(play, talkActor);
                }
            }
        } else {
            func_80839E74(this, play);
        }
    } else {
        if (this->av1.actionVar1 == 0) {
            s32 temp_ft5 = this->skelAnime.curFrame - sp24->unk_8;

            if ((temp_ft5 >= 0) && (sp24->unk_9 >= temp_ft5)) {
                if ((this->av2.actionVar2 != 0) && (temp_ft5 == 0)) {
                    Player_PlaySfx(this, NA_SE_IT_SCOOP_UP_WATER);
                }

                if (Player_GetItemOnButton(play, this, this->heldItemButton) == ITEM_BOTTLE) {
                    Actor* interactRangeActor = this->interactRangeActor;

                    if (interactRangeActor != NULL) {
                        struct_8085D798* entry = D_8085D798;
                        s32 i;

                        for (i = 0; i < ARRAY_COUNT(D_8085D798); i++) {
                            if (((interactRangeActor->id == entry->actorId) &&
                                 ((entry->actorParams <= BOTTLE_CATCH_PARAMS_ANY) ||
                                  (interactRangeActor->params == entry->actorParams)))) {
                                break;
                            }
                            entry++;
                        }

                        if (GameInteractor_Should(VB_PLAY_BOTTLE_CATCH_TEXT, i < ARRAY_COUNT(D_8085D798), this,
                                                  interactRangeActor, entry->itemId, entry->itemAction)) {
                            this->av1.actionVar1 = i + 1;
                            this->av2.actionVar2 = 0;
                            this->stateFlags1 |= PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000;
                            interactRangeActor->parent = &this->actor;
                            Player_UpdateBottleHeld(play, this, entry->itemId, entry->itemAction);
                            Player_Anim_PlayOnceAdjusted(play, this, sp24->unk_4);
                        }
                    }
                }
                // #region 2S2H [Dpad]
                else if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                    if (Player_Dpad_GetItemOnButton(play, this, HELD_ITEM_TO_DPAD(this->heldItemButton)) ==
                        ITEM_BOTTLE) {
                        Actor* interactRangeActor = this->interactRangeActor;

                        if (interactRangeActor != NULL) {
                            struct_8085D798* entry = D_8085D798;
                            s32 i;

                            for (i = 0; i < ARRAY_COUNT(D_8085D798); i++) {
                                if (((interactRangeActor->id == entry->actorId) &&
                                     ((entry->actorParams <= BOTTLE_CATCH_PARAMS_ANY) ||
                                      (interactRangeActor->params == entry->actorParams)))) {
                                    break;
                                }
                                entry++;
                            }

                            if (GameInteractor_Should(VB_PLAY_BOTTLE_CATCH_TEXT, i < ARRAY_COUNT(D_8085D798), this,
                                                      interactRangeActor, entry->itemId, entry->itemAction)) {
                                this->av1.actionVar1 = i + 1;
                                this->av2.actionVar2 = 0;
                                this->stateFlags1 |= PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000;
                                interactRangeActor->parent = &this->actor;
                                Player_UpdateBottleHeld(play, this, entry->itemId, entry->itemAction);
                                Player_Anim_PlayOnceAdjusted(play, this, sp24->unk_4);
                            }
                        }
                    }
                }
                // #endregion
            }
        }

        if (this->skelAnime.curFrame <= 7.0f) {
            this->stateFlags3 |= PLAYER_STATE3_800;
        }
    }
}

Vec3f D_8085D7EC = { 0.0f, 0.0f, 5.0f };

void Player_Action_69(Player* this, PlayState* play) {
    if (GameInteractor_Should(VB_USE_BOTTLE_ITEM, true, this)) {
        func_808323C0(this, play->playerCsIds[PLAYER_CS_ID_ITEM_BOTTLE]);

        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            Player_StopCutscene(this);
            func_80839E74(this, play);
        } else if (PlayerAnimation_OnFrame(&this->skelAnime, 37.0f)) {
            s32 fairyParams = FAIRY_PARAMS(FAIRY_TYPE_8, false, 0);

            Player_PlaySfx(this, NA_SE_EV_BOTTLE_CAP_OPEN);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_AUTO_JUMP);
            if (this->itemAction == PLAYER_IA_BOTTLE_FAIRY) {
                Player_UpdateBottleHeld(play, this, ITEM_BOTTLE, PLAYER_IA_BOTTLE_EMPTY);
                Player_PlaySfx(this, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
                fairyParams = FAIRY_PARAMS(FAIRY_TYPE_1, false, 0);
            }

            Player_SpawnFairy(play, this, &this->leftHandWorld.pos, &D_8085D7EC, fairyParams);
        }
    }
}

void Player_Action_70(Player* this, PlayState* play) {
    static Vec3f D_8085D7F8 = { 10.0f, 268 * 0.1f, 30.0f };
    static s8 D_8085D804[PLAYER_FORM_MAX] = {
        0x2D, // PLAYER_FORM_FIERCE_DEITY
        0x4B, // PLAYER_FORM_GORON
        0x37, // PLAYER_FORM_ZORA
        0x23, // PLAYER_FORM_DEKU
        0x28, // PLAYER_FORM_HUMAN
    };
    static struct_8085D80C D_8085D80C[] = {
        { ACTOR_EN_FISH, FISH_PARAMS(ENFISH_0) },                   // PLAYER_BOTTLE_FISH
        { ACTOR_OBJ_AQUA, AQUA_PARAMS(AQUA_TYPE_COLD) },            // PLAYER_BOTTLE_SPRING_WATER
        { ACTOR_OBJ_AQUA, AQUA_PARAMS(AQUA_TYPE_HOT) },             // PLAYER_BOTTLE_HOT_SPRING_WATER
        { ACTOR_EN_ZORAEGG, ZORA_EGG_PARAMS(ZORA_EGG_TYPE_11, 0) }, // PLAYER_BOTTLE_ZORA_EGG
        { ACTOR_EN_DNP, DEKU_PRINCESS_PARAMS(DEKU_PRINCESS_TYPE_RELEASED_FROM_BOTTLE) }, // PLAYER_BOTTLE_DEKU_PRINCESS
        { ACTOR_EN_MUSHI2, ENMUSHI2_PARAMS(ENMUSHI2_0) },                                // PLAYER_BOTTLE_GOLD_DUST
        { ACTOR_EN_MUSHI2, ENMUSHI2_PARAMS(ENMUSHI2_0) },                                // PLAYER_BOTTLE_1C
        { ACTOR_EN_OT, SEAHORSE_PARAMS(SEAHORSE_TYPE_2, 0, 0) },                         // PLAYER_BOTTLE_SEAHORSE
        { ACTOR_EN_MUSHI2, ENMUSHI2_PARAMS(ENMUSHI2_0) },                                // PLAYER_BOTTLE_MUSHROOM
        { ACTOR_EN_MUSHI2, ENMUSHI2_PARAMS(ENMUSHI2_0) },                                // PLAYER_BOTTLE_HYLIAN_LOACH
        { ACTOR_EN_MUSHI2, ENMUSHI2_PARAMS(ENMUSHI2_0) },                                // PLAYER_BOTTLE_BUG
    };
    static AnimSfxEntry D_8085D838[] = {
        ANIMSFX(ANIMSFX_TYPE_VOICE, 38, NA_SE_VO_LI_AUTO_JUMP, CONTINUE),
        ANIMSFX(ANIMSFX_TYPE_GENERAL, 40, NA_SE_EV_BOTTLE_CAP_OPEN, STOP),
    };

    CollisionPoly* sp6C;
    s32 sp68;
    Vec3f sp5C;
    f32 temp_fa0;
    f32 temp_fv0;
    f32 temp_fv1;
    struct_8085D80C* sp4C;

    D_8085D7F8.z = D_8085D804[this->transformation];
    if (Player_PosVsWallLineTest(play, this, &D_8085D7F8, &sp6C, &sp68, &sp5C)) {
        temp_fv1 = this->actor.world.pos.x - sp5C.x;
        temp_fa0 = this->actor.world.pos.z - sp5C.z;
        temp_fv0 = sqrtf(SQ(temp_fv1) + SQ(temp_fa0));

        if (temp_fv0 != 0.0f) {
            temp_fv0 = 3.0f / temp_fv0;

            this->actor.world.pos.x += temp_fv1 * temp_fv0;
            this->actor.world.pos.z += temp_fa0 * temp_fv0;
        }
    }

    Player_DecelerateToZero(this);

    if (GameInteractor_Should(VB_USE_BOTTLE_ITEM, true, this)) {
        func_8083249C(this);

        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            Player_StopCutscene(this);
            if (!Player_ActionHandler_13(this, play)) {
                func_80839E74(this, play);
            }
        } else if (PlayerAnimation_OnFrame(&this->skelAnime, 76.0f)) {
            sp4C = &D_8085D80C[GET_BOTTLE_FROM_IA(this->itemAction) - 1];

            Actor_Spawn(&play->actorCtx, play, sp4C->actorId,
                        (Math_SinS(this->actor.shape.rot.y) * 5.0f) + this->leftHandWorld.pos.x,
                        this->leftHandWorld.pos.y,
                        (Math_CosS(this->actor.shape.rot.y) * 5.0f) + this->leftHandWorld.pos.z, 0x4000,
                        this->actor.shape.rot.y, 0, sp4C->params);
            Player_UpdateBottleHeld(play, this, ITEM_BOTTLE, PLAYER_IA_BOTTLE_EMPTY);
        } else {
            Player_PlayAnimSfx(this, D_8085D838);
        }
    }
}

AnimSfxEntry D_8085D840[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 30, NA_SE_PL_PUT_OUT_ITEM, STOP),
};

void Player_Action_ExchangeItem(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20;
    this->stateFlags3 |= PLAYER_STATE3_4000000;

    func_8083249C(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->exchangeItemAction == PLAYER_IA_NONE) {
            Actor* talkActor = this->talkActor;

            Player_StopCutscene(this);
            this->getItemDrawIdPlusOne = GID_NONE + 1;

            if ((talkActor->textId != 0) && (talkActor->textId != 0xFFFF)) {
                this->actor.flags |= ACTOR_FLAG_TALK;
            }
            Player_StartTalking(play, talkActor);
        } else {
            GetItemEntry* giEntry = &sGetItemTable[D_8085D1A4[this->exchangeItemAction] - 1];

            if (Player_BottleFromIA(this, this->itemAction) <= PLAYER_BOTTLE_NONE) {
                this->getItemDrawIdPlusOne = ABS_ALT(giEntry->gid);
            }

            if (this->av2.actionVar2 == 0) {
                if ((this->actor.textId != 0) && (this->actor.textId != 0xFFFF)) {
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                }

                this->av2.actionVar2 = 1;
            } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                Player_StopCutscene(this);
                this->getItemDrawIdPlusOne = GID_NONE + 1;
                this->actor.flags &= ~ACTOR_FLAG_TALK;
                func_80839E74(this, play);
                this->textboxBtnCooldownTimer = 10;
            }
        }
    } else if (this->av2.actionVar2 >= 0) {
        if ((Player_BottleFromIA(this, this->itemAction) > PLAYER_BOTTLE_NONE) &&
            PlayerAnimation_OnFrame(&this->skelAnime, 36.0f)) {
            Player_SetModels(this, PLAYER_MODELGROUP_BOTTLE);
        } else if (PlayerAnimation_OnFrame(&this->skelAnime, 2.0f)) {
            GetItemEntry* giEntry = &sGetItemTable[D_8085D1A4[this->itemAction] - 1];

            func_80838830(this, giEntry->objectId);
        }
        Player_PlayAnimSfx(this, D_8085D840);
    }

    if ((this->av1.actionVar1 == 0) && (this->focusActor != NULL)) {
        this->yaw = func_8083C62C(this, 0);
        this->actor.shape.rot.y = this->yaw;
    }
}

void Player_Action_72(Player* this, PlayState* play) {
    this->stateFlags2 |= (PLAYER_STATE2_20 | PLAYER_STATE2_40);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_re_dead_attack_wait);
    }

    if (play->sceneId != SCENE_SEA_BS) {
        func_8082F164(this, BTN_R);
    }

    if (func_8082DE88(this, 0, 0x64)) {
        func_80836988(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_80;
    }
}

void Player_Action_SlideOnSlope(Player* this, PlayState* play) {
    CollisionPoly* floorPoly;
    f32 speedXZTarget;
    f32 speedXZIncrStep;
    f32 speedXZDecrStep;
    s16 downwardSlopeYaw;
    s16 shapeYawTarget;
    Vec3f slopeNormal;

    this->stateFlags2 |= (PLAYER_STATE2_20 | PLAYER_STATE2_40);

    PlayerAnimation_Update(play, &this->skelAnime);

    func_8083FBC4(play, this);

    Audio_PlaySfx_AtPosWithSyncedFreqAndVolume(
        &this->actor.projectedPos, Player_GetFloorSfx(this, NA_SE_PL_SLIP_LEVEL - SFX_FLAG), this->actor.speed);

    if (Player_ActionHandler_13(this, play)) {
        return;
    }

    if ((this->transformation == PLAYER_FORM_GORON) && Player_ActionHandler_6(this, play)) {
        return;
    }

    floorPoly = this->actor.floorPoly;
    if (floorPoly == NULL) {
        func_80833AA0(this, play);
        return;
    }

    Actor_GetSlopeDirection(floorPoly, &slopeNormal, &downwardSlopeYaw);

    shapeYawTarget = downwardSlopeYaw;
    if (this->av1.facingUpSlope) {
        shapeYawTarget = downwardSlopeYaw + 0x8000;
    }

    if (this->speedXZ < 0.0f) {
        downwardSlopeYaw += 0x8000;
    }

    speedXZTarget = (1.0f - slopeNormal.y) * 40.0f;
    speedXZTarget = CLAMP(speedXZTarget, 0.0f, 10.0f);

    speedXZIncrStep = SQ(speedXZTarget) * 0.015f;
    speedXZDecrStep = slopeNormal.y * 0.01f;

    if (SurfaceType_GetFloorEffect(&play->colCtx, floorPoly, this->actor.floorBgId) != FLOOR_EFFECT_1) {
        speedXZTarget = 0.0f;
        speedXZDecrStep = slopeNormal.y * 10.0f;
    }

    speedXZIncrStep = CLAMP_MIN(speedXZIncrStep, 1.0f);

    if (Math_AsymStepToF(&this->speedXZ, speedXZTarget, speedXZIncrStep, speedXZDecrStep) && (speedXZTarget == 0.0f)) {
        func_80836A98(this,
                      (!this->av1.facingUpSlope) ? D_8085BE84[PLAYER_ANIMGROUP_down_slope_slip_end][this->modelAnimType]
                                                 : D_8085BE84[PLAYER_ANIMGROUP_up_slope_slip_end][this->modelAnimType],
                      play);
    }

    Math_SmoothStepToS(&this->yaw, downwardSlopeYaw, 0xA, 0xFA0, 0x320);
    Math_ScaledStepToS(&this->actor.shape.rot.y, shapeYawTarget, 0x7D0);
}

/**
 * Waits to start processing a Cutscene Action.
 * First, the timer `csDelayTimer` much reach 0.
 * Then, there must be a CS action available to start processing.
 *
 * When starting the cutscene action, `draw` will be set to make
 * Player appear, if he was invisible.
 */
void Player_Action_WaitForCutscene(Player* this, PlayState* play) {
    if ((DECR(this->av2.csDelayTimer) == 0) && Player_StartCsAction(play, this)) {
        func_80859CE0(play, this, 0);
        Player_SetAction(play, this, Player_Action_CsAction, 0);
        Player_Action_CsAction(this, play);
    }
}

void Player_Action_StartWarpSongArrive(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_WaitForCutscene, 0);
    this->av2.csDelayTimer = 40;

    Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0x10);
}

void Player_Action_BlueWarpArrive(Player* this, PlayState* play) {
    if (sPlayerYDistToFloor < 150.0f) {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            if (!this->av2.playedLandingSfx) {
                if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                    this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
                    Player_AnimSfx_PlayFloorLand(this);
                    this->av2.playedLandingSfx = true;
                }
            } else {
                func_8085B384(this, play);
            }
        }

        Math_SmoothStepToF(&this->actor.velocity.y, 2.0f, 0.3f, 8.0f, 0.5f);
    }

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.playerCue != NULL) {
            s32 pad;
            f32 savedYPos = this->actor.world.pos.y;

            Player_Cutscene_SetPosAndYawToStart(this, play->csCtx.playerCue);
            this->actor.world.pos.y = savedYPos;
        }
    }
}

void Player_Action_77(Player* this, PlayState* play) {
    if (this->skelAnime.animation == NULL) {
        this->stateFlags2 |= PLAYER_STATE2_4000;
    } else {
        PlayerAnimation_Update(play, &this->skelAnime);
        if ((BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_derth_rebirth)) &&
            PlayerAnimation_OnFrame(&this->skelAnime, 60.0f)) {
            Player_AnimSfx_PlayFloor(this, NA_SE_PL_BOUND);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);
        }
    }

    if ((this->av2.actionVar2++ >= 9) && !func_8082DA90(play)) {
        if (this->av1.actionVar1 != 0) {
            if (this->av1.actionVar1 < 0) {
                func_80169FDC(play);
            } else {
                func_80169EFC(play);
            }
            if (!SurfaceType_IsWallDamage(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId)) {
                gSaveContext.respawnFlag = -5;
            }

            play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
            Audio_PlaySfx(NA_SE_OC_ABYSS);
        } else {
            play->transitionType = TRANS_TYPE_FADE_BLACK;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
            gSaveContext.seqId = NA_BGM_DISABLED;
            gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
        }

        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

/**
 * Automatically open a door (no need for the A button).
 * Note: If no door is in useable range, a softlock will occur.
 */
void Player_Action_TryOpeningDoor(Player* this, PlayState* play) {
    Player_ActionHandler_1(this, play);
}

void Player_Action_ExitGrotto(Player* this, PlayState* play) {
    this->actor.gravity = -1.0f;

    PlayerAnimation_Update(play, &this->skelAnime);

    if (this->actor.velocity.y < 0.0f) {
        func_80833AA0(this, play);
    } else if (this->actor.velocity.y < 6.0f) {
        Math_StepToF(&this->speedXZ, 3.0f, 0.5f);
    }
}

void Player_Action_80(Player* this, PlayState* play) {
    if (play->bButtonAmmoPlusOne < 0) {
        play->bButtonAmmoPlusOne = 0;
        func_80839ED0(this, play);
    } else if (this->av1.actionVar1 == 0) {
        if ((play->sceneId != SCENE_20SICHITAI) && CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_B)) {
            play->bButtonAmmoPlusOne = 10;
            func_80847880(play, this);
            Player_SetAction(play, this, Player_Action_80, 1);
            this->av1.actionVar1 = 1;
        } else {
            play->bButtonAmmoPlusOne = 0;
            func_80847190(play, this, 0);

            if (play->actorCtx.flags & ACTORCTX_FLAG_PICTO_BOX_ON) {
                this->stateFlags1 |= PLAYER_STATE1_100000;
                func_8083868C(play, this);
            } else {
                this->stateFlags1 &= ~PLAYER_STATE1_100000;
                if ((play->sceneId == SCENE_20SICHITAI) &&
                    (Player_GetItemOnButton(play, this, func_8082FDC4()) == ITEM_PICTOGRAPH_BOX)) {
                    s32 requiredScopeTemp;

                    play->actorCtx.flags |= ACTORCTX_FLAG_PICTO_BOX_ON;
                }
                // #region 2S2H [Dpad]
                else if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                    if ((play->sceneId == SCENE_20SICHITAI) &&
                        (Player_Dpad_GetItemOnButton(play, this, func_Dpad_8082FDC4()) == ITEM_PICTOGRAPH_BOX)) {
                        play->actorCtx.flags |= ACTORCTX_FLAG_PICTO_BOX_ON;
                    }
                }
                // #endregion
            }
        }
    } else if (CHECK_BTN_ANY(sPlayerControlInput->press.button,
                             BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP | BTN_R | BTN_A | BTN_DPAD_EQUIP)) {
        play->bButtonAmmoPlusOne = -1;
        Player_Action_81(this, play);
        Player_SetAction(play, this, Player_Action_80, 0);
        this->av1.actionVar1 = 0;
    } else {
        play->bButtonAmmoPlusOne = 10;
        Player_Action_81(this, play);
    }
}

void Player_Action_81(Player* this, PlayState* play) {
    this->unk_AA5 = PLAYER_UNKAA5_3;
    func_8083868C(play, this);
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_UpdateUpperBody(this, play);
    this->upperLimbRot.y = func_80847190(play, this, 1) - this->actor.shape.rot.y;
    this->unk_AA6_rotFlags |= UNKAA6_ROT_UPPER_Y;

    if (play->bButtonAmmoPlusOne < 0) {
        play->bButtonAmmoPlusOne++;
        if (play->bButtonAmmoPlusOne == 0) {
            func_80839ED0(this, play);
        }
    }
}

void Player_Action_82(Player* this, PlayState* play) {
    if (this->av1.actionVar1 >= 0) {
        if (this->av1.actionVar1 < 6) {
            this->av1.actionVar1++;
        } else {
            this->unk_B48 = (this->av1.actionVar1 >> 1) * 22.0f;
            if (func_8082DE88(this, 1, 0x64)) {
                this->av1.actionVar1 = -1;
                EffectSsIcePiece_SpawnBurst(play, &this->actor.world.pos, this->actor.scale.x);
                Player_PlaySfx(this, NA_SE_PL_ICE_BROKEN);
            }

            if (this->transformation == PLAYER_FORM_ZORA) {
                func_80834104(play, this);
                this->skelAnime.animation = NULL;
                this->av2.actionVar2 = -0x28;
                this->av1.actionVar1 = 1;
                this->speedXZ = 0.0f;
            } else if (play->gameplayFrames % 4 == 0) {
                Player_InflictDamage(play, -1);
            }
        }

        this->stateFlags2 |= PLAYER_STATE2_4000;
    } else if (PlayerAnimation_Update(play, &this->skelAnime)) {
        func_80836988(this, play);
        func_808339B4(this, 20);
    }
}

void Player_Action_83(Player* this, PlayState* play) {
    PlayerAnimation_Update(play, &this->skelAnime);
    func_808345A8(this);

    if (((this->av2.actionVar2 % 25) != 0) || (func_808339D4(play, this, -1) != 0)) {
        if (DECR(this->av2.actionVar2) == 0) {
            func_80836988(this, play);
        }
    }

    this->bodyShockTimer = 40;
    Actor_PlaySfx_Flagged2(&this->actor, this->ageProperties->voiceSfxIdOffset + (NA_SE_VO_LI_TAKEN_AWAY - SFX_FLAG));
}

void Player_Action_84(Player* this, PlayState* play) {
    AttackAnimInfo* attackInfoEntry = &sMeleeAttackAnimInfo[this->meleeWeaponAnimation];

    if (this->skelAnime.curFrame < (this->skelAnime.endFrame - 6.0f)) {
        this->stateFlags2 |= PLAYER_STATE2_20;
    }

    if (func_808401F4(play, this)) {
        return;
    }

    if (this->speedXZ >= 0.0f) {
        func_8083FCF0(play, this, (this->transformation == PLAYER_FORM_GORON) ? 5.0f : 0.0f, attackInfoEntry->unk_C,
                      attackInfoEntry->unk_D);
    }

    if ((this->meleeWeaponAnimation == PLAYER_MWA_GORON_PUNCH_LEFT) ||
        (this->meleeWeaponAnimation == PLAYER_MWA_GORON_PUNCH_RIGHT)) {
        this->unk_3D0.unk_00 = 3;
    }

    //! @bug Lunge Storage: If this block is prevented from running at the end of an animation that produces a lunge,
    //! the prepared lunge will be retained until next time execution passes through here, which usually means the next
    //! sword slash.
    if ((this->stateFlags2 & PLAYER_STATE2_40000000) && PlayerAnimation_OnFrame(&this->skelAnime, 0.0f)) {
        this->speedXZ = 15.0f;
        this->stateFlags2 &= ~PLAYER_STATE2_40000000;
    }

    if (this->speedXZ > 12.0f) {
        func_8083FBC4(play, this);
    }

    Math_StepToF(&this->speedXZ, 0.0f, 5.0f);
    func_8083A548(this);

    if (PlayerAnimation_Update(play, &this->skelAnime) ||
        ((this->meleeWeaponAnimation >= PLAYER_MWA_FLIPSLASH_FINISH) &&
         (this->meleeWeaponAnimation <= PLAYER_MWA_ZORA_JUMPKICK_FINISH) && (this->skelAnime.curFrame > 2.0f) &&
         Player_CanSpinAttack(this))) {
        sPlayerUseHeldItem = this->av2.actionVar2;

        if (!Player_ActionHandler_7(this, play)) {
            PlayerAnimationHeader* anim =
                Player_CheckHostileLockOn(this) ? attackInfoEntry->unk_8 : attackInfoEntry->unk_4;

            func_8082DC38(this);

            if (anim == NULL) {
                this->skelAnime.movementFlags &= ~ANIM_FLAG_ENABLE_MOVEMENT;
                func_8085B384(this, play);
            } else {
                u8 savedMovementFlags = this->skelAnime.movementFlags;

                if (this->transformation == PLAYER_FORM_ZORA) {
                    if (Player_ActionHandler_8(this, play)) {
                        anim = this->skelAnimeUpper.animation;
                    }
                    this->unk_ADC = 0;
                } else if ((anim == &gPlayerAnim_link_fighter_Lpower_jump_kiru_end) &&
                           (this->modelAnimType != PLAYER_ANIMTYPE_3)) {
                    anim = &gPlayerAnim_link_fighter_power_jump_kiru_end;
                }

                this->skelAnime.movementFlags = 0;
                Player_SetAction(play, this, Player_Action_Idle, 1);
                Player_Anim_PlayOnceWaterAdjustment(play, this, anim);
                this->yaw = this->actor.shape.rot.y;
                this->skelAnime.movementFlags = savedMovementFlags;
            }
            this->stateFlags3 |= PLAYER_STATE3_8;
        }
    } else if (((this->transformation == PLAYER_FORM_ZORA) &&
                (this->meleeWeaponAnimation != PLAYER_MWA_ZORA_PUNCH_KICK) &&
                (this->meleeWeaponAnimation != PLAYER_MWA_ZORA_JUMPKICK_FINISH)) ||
               ((this->transformation == PLAYER_FORM_GORON) &&
                (this->meleeWeaponAnimation != PLAYER_MWA_GORON_PUNCH_BUTT))) {
        this->av2.actionVar2 |= CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_B) ? 1 : 0;
    }
}

void Player_Action_85(Player* this, PlayState* play) {
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_DecelerateToZero(this);

    if (this->skelAnime.curFrame >= 6.0f) {
        func_80836988(this, play);
    }
}

// Array colour interpolation
// arg0 is the interpolation parameter
// arg1,5,9 are out colours
// arg2,6,0xA are first values
// arg3,7,0xB are second values
// arg4,8,0xC are subtracted after interpolation
void func_80854CD0(f32 arg0, s16* arg1, u8* arg2, u8* arg3, u8* arg4, s16* arg5, u8* arg6, u8* arg7, u8* arg8,
                   s16* arg9, u8* argA, u8* argB, u8* argC) {
    s32 i;

    for (i = 0; i < 3; i++) {
        *arg1 = ((s32)((*arg2 - *arg3) * arg0) + *arg3) - *arg4;
        *arg5 = ((s32)((*arg6 - *arg7) * arg0) + *arg7) - *arg8;
        *arg9 = ((s32)((*argA - *argB) * arg0) + *argB) - *argC;

        arg1++;
        arg2++;
        arg3++;
        arg4++;
        arg5++;
        arg6++;
        arg7++;
        arg8++;
        arg9++;
        argA++;
        argB++;
        argC++;
    }
}

// Black, probably in-function static
u8 D_8085D844[] = { 0, 0, 0 };

// arg1 is the colour interpolation parameter
void func_80854EFC(PlayState* play, f32 arg1, struct_8085D848_unk_00* arg2) {
    struct_8085D848_unk_00 sp70;
    struct_8085D848_unk_00* var_t0;
    struct_8085D848_unk_00* var_v1;
    u8* var_t3;
    u8* var_t4;
    u8* new_var;
    s32 pad[4];

    new_var = play->envCtx.lightSettings.light1Color;
    sp70.fogNear = play->envCtx.lightSettings.fogNear;
    sp70.fogColor[0] = play->envCtx.lightSettings.fogColor[0];
    sp70.fogColor[1] = play->envCtx.lightSettings.fogColor[1];
    sp70.fogColor[2] = play->envCtx.lightSettings.fogColor[2];
    sp70.ambientColor[0] = play->envCtx.lightSettings.ambientColor[0];
    sp70.ambientColor[1] = play->envCtx.lightSettings.ambientColor[1];
    sp70.ambientColor[2] = play->envCtx.lightSettings.ambientColor[2];

    if (arg1 <= 1.0f) {
        arg1 -= 0.0f;

        var_v1 = &arg2[0];
        var_t0 = &sp70;
        var_t3 = D_8085D844;
        var_t4 = new_var;
    } else if (arg1 <= 2.0f) {
        arg1 -= 1.0f;
        var_v1 = &arg2[1];
        var_t0 = &arg2[0];
        var_t3 = D_8085D844;
        var_t4 = D_8085D844;

    } else if (arg1 <= 3.0f) {
        arg1 -= 2.0f;
        var_v1 = &arg2[2];
        var_t0 = &arg2[1];
        var_t3 = D_8085D844;
        var_t4 = D_8085D844;

    } else {
        arg1 -= 3.0f;
        var_v1 = &sp70;
        var_t0 = &arg2[2];
        var_t3 = new_var;
        var_t4 = D_8085D844;
    }

    play->envCtx.adjLightSettings.fogNear =
        (TRUNCF_BINANG((var_v1->fogNear - var_t0->fogNear) * arg1) + var_t0->fogNear) -
        play->envCtx.lightSettings.fogNear;

    func_80854CD0(arg1, play->envCtx.adjLightSettings.fogColor, var_v1->fogColor, var_t0->fogColor,
                  play->envCtx.lightSettings.fogColor, play->envCtx.adjLightSettings.ambientColor, var_v1->ambientColor,
                  var_t0->ambientColor, play->envCtx.lightSettings.ambientColor,
                  play->envCtx.adjLightSettings.light1Color, var_t3, var_t4, new_var);
}

struct_8085D848 D_8085D848[] = {
    {
        {
            { 650, { 0, 0, 0 }, { 10, 0, 30 } },
            { 300, { 200, 200, 255 }, { 0, 0, 0 } },
            { 600, { 0, 0, 0 }, { 0, 0, 200 } },
        },
        {
            { { -40.0f, 20.0f, -10.0f }, { 120, 200, 255 }, 1000 },
            { { 0.0f, -10.0f, 0.0f }, { 255, 255, 255 }, 5000 },
            { { -10.0f, 4.0f, 3.0f }, { 200, 200, 255 }, 5000 },
        },
    },
    {
        {
            { 650, { 0, 0, 0 }, { 10, 0, 30 } },
            { 300, { 200, 200, 255 }, { 0, 0, 0 } },
            { 600, { 0, 0, 0 }, { 0, 0, 200 } },
        },
        {
            { { 0.0f, 0.0f, 5.0f }, { 155, 255, 255 }, 100 },
            { { 0.0f, 0.0f, 5.0f }, { 155, 255, 255 }, 100 },
            { { 0.0f, 0.0f, 5.0f }, { 155, 255, 255 }, 100 },
        },
    },
};

// arg2 is the colour interpolation parameter
// arg3 both selects the light to use and scales the radius
// arg4 selects the env fog/colour info
void func_808550D0(PlayState* play, Player* this, f32 arg2, f32 arg3, s32 arg4) {
    struct_8085D848* temp_a2 = &D_8085D848[arg4];
    struct_8085D848_unk_18* lightInit = temp_a2->light;
    Vec3f pos;

    func_80854EFC(play, arg2, temp_a2->unk_00);

    if (arg3 > 2.0f) {
        arg3 -= 2.0f;
        lightInit += 2;
    } else if (arg3 > 1.0f) {
        arg3 -= 1.0f;
        lightInit++;
    }

    Player_TranslateAndRotateY(this, &this->actor.world.pos, &lightInit->pos, &pos);
    Lights_PointNoGlowSetInfo(&this->lightInfo, pos.x, pos.y, pos.z, lightInit->color[0], lightInit->color[1],
                              lightInit->color[2], lightInit->radius * arg3);
}

AnimSfxEntry D_8085D8F0[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 2, NA_SE_PL_PUT_OUT_ITEM, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 4, NA_SE_IT_SET_TRANSFORM_MASK, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 11, NA_SE_PL_FREEZE_S, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 30, NA_SE_PL_TRANSFORM_VOICE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 20, NA_SE_IT_TRANSFORM_MASK_BROKEN, STOP),
};

AnimSfxEntry D_8085D904[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 8, NA_SE_IT_SET_TRANSFORM_MASK, STOP),
};

void func_80855218(PlayState* play, Player* this, struct_8085D910** arg2) {
    if (PlayerAnimation_Update(play, &this->skelAnime) &&
        (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_cl_setmask))) {
        Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_cl_setmaskend);
    } else if ((BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_cl_setmask)) ||
               (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_cl_setmaskend))) {
        if (this->av1.actionVar1 >= 58) {
            Math_StepToS(&this->av2.actionVar2, 255, 50);
        }

        if (this->av1.actionVar1 >= 64) {
            Math_StepToF(&this->unk_B10[2], 0.0f, 0.015f);
        } else if (this->av1.actionVar1 >= 0xE) {
            Math_StepToF(&this->unk_B10[2], 0.3f, 0.3f);
        }

        if (this->av1.actionVar1 > 65) {
            Math_StepToF(&this->unk_B10[3], 0.0f, 0.02f);
        } else if (this->av1.actionVar1 >= 0x10) {
            Math_StepToF(&this->unk_B10[3], -0.1f, 0.1f);
        }

        if ((R_PLAY_FILL_SCREEN_ON == 0) && (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_cl_setmask))) {
            Player_PlayAnimSfx(this, D_8085D8F0);
        }
    } else {
        if (this->av1.actionVar1 >= 20) {
            Math_StepToS(&this->av2.actionVar2, 255, 20);
        }

        if (R_PLAY_FILL_SCREEN_ON == 0) {
            Player_PlayAnimSfx(this, D_8085D904);
            if (this->av1.actionVar1 == 15) {
                Player_PlaySfx(this, NA_SE_PL_FACE_CHANGE);
            }
        }
    }
}

u16 D_8085D908[] = {
    WEEKEVENTREG_30_80,               // PLAYER_FORM_FIERCE_DEITY
    WEEKEVENTREG_30_20,               // PLAYER_FORM_GORON
    WEEKEVENTREG_30_40,               // PLAYER_FORM_ZORA
    WEEKEVENTREG_30_10,               // PLAYER_FORM_DEKU
    PACK_WEEKEVENTREG_FLAG(16, 0x0A), // 2S2H [Port] Added to match OOB value read on console for human form
};
struct_8085D910 D_8085D910[] = {
    { 0x10, 0xA, 0x3B, 0x3F },
    { 9, 0x32, 0xA, 0xD },
};

void Player_Action_86(Player* this, PlayState* play) {
    struct_8085D910* sp4C = D_8085D910;
    s32 sp48 = false;

    if (GameInteractor_Should(VB_PREVENT_MASK_TRANSFORMATION_CS, false))
        return;

    func_808323C0(this, play->playerCsIds[PLAYER_CS_ID_MASK_TRANSFORMATION]);
    sPlayerControlInput = play->state.input;

    Camera_ChangeMode(GET_ACTIVE_CAM(play),
                      (this->transformation == PLAYER_FORM_HUMAN) ? CAM_MODE_NORMAL : CAM_MODE_JUMP);
    this->stateFlags2 |= PLAYER_STATE2_40;
    this->actor.shape.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000;

    func_80855218(play, this, &sp4C);

    if (this->av1.actionVar1 == 0x14) {
        Play_EnableMotionBlurPriority(100);
    }

    if (R_PLAY_FILL_SCREEN_ON != 0) {
        R_PLAY_FILL_SCREEN_ALPHA += R_PLAY_FILL_SCREEN_ON;
        if (R_PLAY_FILL_SCREEN_ALPHA > 255) {
            R_PLAY_FILL_SCREEN_ALPHA = 255;
            this->actor.update = func_8012301C;
            this->actor.draw = NULL;
            this->av1.actionVar1 = 0;
            Play_DisableMotionBlurPriority();
            //! @bug When taking off a transformation mask, PLAYER_FORM_HUMAN will index OOB leading
            // to the next value causing WEEKEVENT_REG 16 being set with 0x0A which sets two flags at once
            // WEEKEVENTREG_16_02 and WEEKEVENTREG_16_08
            // WEEKEVENTREG_16_02 corresponds to showing a text ID from the Gorman Brothers on the 3rd day
            // if the player has saved the farm, so this bug would prevent that text from displaying
            SET_WEEKEVENTREG(D_8085D908[GET_PLAYER_FORM]);
        }
    } else if ((this->av1.actionVar1++ > ((this->transformation == PLAYER_FORM_HUMAN) ? 0x53 : 0x37)) ||
               ((this->av1.actionVar1 >= 5) &&
                (sp48 =
                     ((this->transformation != PLAYER_FORM_HUMAN) || CHECK_WEEKEVENTREG(D_8085D908[GET_PLAYER_FORM])) &&
                     CHECK_BTN_ANY(sPlayerControlInput->press.button,
                                   BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP | BTN_B | BTN_A | BTN_DPAD_EQUIP)))) {
        R_PLAY_FILL_SCREEN_ON = 45;
        R_PLAY_FILL_SCREEN_R = 220;
        R_PLAY_FILL_SCREEN_G = 220;
        R_PLAY_FILL_SCREEN_B = 220;
        R_PLAY_FILL_SCREEN_ALPHA = 0;

        if (sp48) {
            if (CutsceneManager_GetCurrentCsId() == this->csId) {
                func_800E0348(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->csId)));
            }

            if (this->transformation == PLAYER_FORM_HUMAN) {
                AudioSfx_StopById(NA_SE_PL_TRANSFORM_VOICE);
                AudioSfx_StopById(NA_SE_IT_TRANSFORM_MASK_BROKEN);
            } else {
                AudioSfx_StopById(NA_SE_PL_FACE_CHANGE);
            }
        }

        Player_PlaySfx(this, NA_SE_SY_TRANSFORM_MASK_FLASH);
    }

    if (this->av1.actionVar1 >= sp4C->unk_0) {
        if (this->av1.actionVar1 < sp4C->unk_2) {
            Math_StepToF(&this->unk_B10[4], 1.0f, sp4C->unk_1 / 100.0f);
        } else if (this->av1.actionVar1 < sp4C->unk_3) {
            if (this->av1.actionVar1 == sp4C->unk_2) {
                Lib_PlaySfx_2(NA_SE_EV_LIGHTNING_HARD);
            }

            Math_StepToF(&this->unk_B10[4], 2.0f, 0.5f);
        } else {
            Math_StepToF(&this->unk_B10[4], 3.0f, 0.2f);
        }
    }

    if (this->av1.actionVar1 >= 0x10) {
        if (this->av1.actionVar1 < 0x40) {
            Math_StepToF(&this->unk_B10[5], 1.0f, 0.2f);
        } else if (this->av1.actionVar1 < 0x37) {
            Math_StepToF(&this->unk_B10[5], 2.0f, 1.0f);
        } else {
            Math_StepToF(&this->unk_B10[5], 3.0f, 0.55f);
        }
    }

    func_808550D0(play, this, this->unk_B10[4], this->unk_B10[5], (this->transformation == PLAYER_FORM_HUMAN) ? 0 : 1);
}

void Player_Action_87(Player* this, PlayState* play) {
    Camera_ChangeMode(GET_ACTIVE_CAM(play), (this->prevMask == PLAYER_MASK_NONE) ? CAM_MODE_NORMAL : CAM_MODE_JUMP);

    if (R_PLAY_FILL_SCREEN_ON != 0) {
        R_PLAY_FILL_SCREEN_ALPHA -= R_PLAY_FILL_SCREEN_ON;
        if (R_PLAY_FILL_SCREEN_ALPHA < 0) {
            R_PLAY_FILL_SCREEN_ON = 0;
            R_PLAY_FILL_SCREEN_ALPHA = 0;
        }
    }

    if (this->av1.actionVar1++ < 4) {
        if ((this->prevMask == PLAYER_MASK_NONE) && (this->av1.actionVar1 == 4)) {
            PlayerAnimation_Change(play, &this->skelAnime, Player_GetIdleAnim(this), PLAYER_ANIM_NORMAL_SPEED, 0.0f,
                                   20.0f, ANIMMODE_ONCE, 20.0f);
        }
    } else {
        s32 pad;
        f32 dist;
        s16 angle;

        Lib_GetControlStickData(&dist, &angle, play->state.input);
        if (PlayerAnimation_Update(play, &this->skelAnime) || ((this->av1.actionVar1 > 10) && (dist != 0.0f))) {
            if (R_PLAY_FILL_SCREEN_ON == 0) {
                this->stateFlags1 &= ~PLAYER_STATE1_2;
                this->prevMask = this->currentMask;
                this->csId = play->playerCsIds[PLAYER_CS_ID_MASK_TRANSFORMATION];
                Player_StopCutscene(this);
                play->envCtx.adjLightSettings = D_80862B50;
                func_8085B384(this, play);
                return;
            }
        }

        Math_StepToF(&this->unk_B10[5], 4.0f, 0.2f);
    }

    func_808550D0(play, this, 0, this->unk_B10[5], (this->prevMask == PLAYER_MASK_NONE) ? 0 : 1);
}

void Player_Action_88(Player* this, PlayState* play) {
    if (this->av2.actionVar2++ > 90) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        func_8085B384(this, play);
    } else if (this->av2.actionVar2 == 10) {
        func_80848640(play, this);
    }
}

// Giant's Mask
void Player_Action_89(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_40;

    func_80855218(play, this, NULL);
    this->av1.actionVar1++;

    if (!(this->stateFlags1 & PLAYER_STATE1_100)) {
        this->prevMask = this->currentMask;
        gSaveContext.save.equippedMask = this->currentMask = PLAYER_MASK_GIANT;
        Magic_Consume(play, 0, MAGIC_CONSUME_GIANTS_MASK);
        this->currentBoots = PLAYER_BOOTS_GIANT;
        this->prevBoots = PLAYER_BOOTS_GIANT;
        func_80123140(play, this);
        func_8085B384(this, play);
    }
}

void Player_Action_90(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_40;

    PlayerAnimation_Update(play, &this->skelAnime);

    if (!(this->stateFlags1 & PLAYER_STATE1_100)) {
        this->prevMask = this->currentMask;

        gSaveContext.save.equippedMask = this->currentMask = PLAYER_MASK_NONE;

        this->currentBoots = PLAYER_BOOTS_HYLIAN;
        this->prevBoots = PLAYER_BOOTS_HYLIAN;
        func_80123140(play, this);
        func_8085B384(this, play);
    }
}

void Player_Action_91(Player* this, PlayState* play) {
    s16 sp3E;
    s32 pad;
    PlayerAnimationHeader* anim;
    s32 var_a0;

    func_808323C0(this, play->playerCsIds[PLAYER_CS_ID_WARP_PAD_MOON]);
    sp3E = BINANG_SUB(this->actor.shape.rot.y, this->actor.world.rot.y);

    var_a0 = 0;
    if ((this->actor.floorHeight - this->actor.world.pos.y) < 60.0f) {
        Math_StepToF(&this->unk_B10[5], 200.0f, 150.0f);
        var_a0 = Math_StepToS(&this->av2.actionVar2, 0xFA0, 0x15E);
    }

    this->actor.shape.rot.y += this->av2.actionVar2;
    this->skelAnime.jointTable[LIMB_ROOT_POS].x = 0;
    this->skelAnime.jointTable[LIMB_ROOT_POS].z = 0;
    this->unk_ABC += this->unk_B10[5];

    if (this->unk_ABC >= 0.0f) {
        this->unk_ABC = 0.0f;
        if ((var_a0 != 0) && (sp3E < 0)) {
            if (BINANG_SUB(this->actor.shape.rot.y, this->actor.world.rot.y) >= 0) {
                this->actor.shape.rot.y = this->actor.world.rot.y;
                Player_StopCutscene(this);
                if (PLAYER_GET_START_MODE(&this->actor) == PLAYER_START_MODE_8) {
                    anim = D_8085D17C[this->transformation];
                    func_80836A5C(this, play);
                    PlayerAnimation_Change(play, &this->skelAnime, anim, -PLAYER_ANIM_ADJUSTED_SPEED,
                                           Animation_GetLastFrame(anim), 0.0f, ANIMMODE_ONCE, -6.0f);
                } else {
                    func_80839E74(this, play);
                }
            }
        }
    } else if (this->av1.actionVar1 == 0) {
        Player_PlaySfx(this, NA_SE_PL_WARP_PLATE_OUT);
        this->av1.actionVar1 = 1;
    }
}

void Player_Action_HookshotFly(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_hook_fly_wait);
    }

    Math_Vec3f_Sum(&this->actor.world.pos, &this->actor.velocity, &this->actor.world.pos);

    if (func_80831124(play, this)) {
        f32 var_fv0;

        Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
        Player_ProcessSceneCollision(play, this);

        var_fv0 = this->actor.world.pos.y - this->actor.floorHeight;
        var_fv0 = CLAMP_MAX(var_fv0, 20.0f);

        this->actor.world.pos.y -= var_fv0;
        this->actor.shape.rot.x = 0;
        this->speedXZ = 1.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.world.rot.x = this->actor.shape.rot.x;
        func_80833AA0(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_400;
        this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
        this->stateFlags3 |= PLAYER_STATE3_10000;
    } else if ((!BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_hook_fly_start)) ||
               (this->skelAnime.curFrame >= 4.0f)) {
        this->actor.gravity = 0.0f;
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->actor.world.rot.x, 0x800);
        Player_RequestRumble(play, this, 100, 2, 100, SQ(0));
    }
}

void func_80855F9C(PlayState* play, Player* this) {
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_20;
    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);
    Math_ScaledStepToS(&this->yaw, yawTarget, 0x258);
}

s32 func_80856000(PlayState* play, Player* this) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f pos;
    f32 sp28;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y - 20.0f;
    pos.z = this->actor.world.pos.z;
    return BgCheck_EntityCheckCeiling(&play->colCtx, &sp28, &pos, 30.0f, &poly, &bgId, &this->actor);
}

void func_80856074(PlayState* play, Player* this) {
    if (func_8083F8A8(play, this, 12.0f, 4, 0.0f, 10, 50, true)) {
        EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, 3.0f, 0, 4, 8, 2, -1, 10, NULL);
    }
}

void func_80856110(PlayState* play, Player* this, f32 arg2, f32 arg3, f32 arg4, s16 scale, s16 scaleStep, s16 life) {
    static Vec3f D_8085D918 = { 0.0f, 0.5f, 0.0f };        // velocity
    static Vec3f D_8085D924 = { 0.0f, 0.5f, 0.0f };        // accel
    static Color_RGBA8 D_8085D930 = { 255, 255, 55, 255 }; // primColor
    static Color_RGBA8 D_8085D934 = { 100, 50, 0, 0 };     // envColor
    Vec3f pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + arg2;
    pos.z = this->actor.world.pos.z;

    D_8085D918.y = arg3;
    D_8085D924.y = arg4;

    func_800B0EB0(play, &pos, &D_8085D918, &D_8085D924, &D_8085D930, &D_8085D934, scale, scaleStep, life);
}

// Deku Flower related
void Player_Action_93(Player* this, PlayState* play) {
    DynaPolyActor* dyna;
    s32 aux = 0xAE;
    f32 temp_fv0_2;
    s32 sp38;
    s32 var_v1;

    PlayerAnimation_Update(play, &this->skelAnime);

    if (Player_ActionHandler_13(this, play)) {
        return;
    }

    if (this->av1.actionVar1 == 0) {
        this->unk_ABC += this->unk_B48;
        if (this->unk_ABC < -1000.0f) {
            this->unk_ABC = -1000.0f;
            this->av1.actionVar1 = 1;
            this->unk_B48 = 0.0f;
        }
        func_80856074(play, this);
    } else if (this->av1.actionVar1 == 1) {
        this->unk_B48 += -22.0f;
        if (this->unk_B48 < -170.0f) {
            this->unk_B48 = -170.0f;
        }
        this->unk_ABC += this->unk_B48;
        if (this->unk_ABC < -3900.0f) {
            this->unk_ABC = -3900.0f;
            this->av1.actionVar1 = 2;
            this->actor.shape.rot.y = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
            this->actor.scale.y = 0.01f;
            this->yaw = this->actor.world.rot.y = this->actor.shape.rot.y;
        } else {
            temp_fv0_2 = Math_SinS((1000.0f + this->unk_ABC) * (-30.0f)) * 0.004f;
            this->actor.scale.y = 0.01f + temp_fv0_2;
            this->actor.scale.z = this->actor.scale.x = 0.01f - (this->unk_B48 * -0.000015f);

            this->actor.shape.rot.y += TRUNCF_BINANG(this->unk_B48 * 130.0f);
            if (this->actor.floorBgId != BGCHECK_SCENE) {
                dyna = DynaPoly_GetActor(&play->colCtx, this->actor.floorBgId);

                if (dyna != NULL) {
                    Math_Vec3f_StepToXZ(&this->actor.world.pos, &dyna->actor.world.pos, 1.0f);
                }
            }
        }

        func_80856074(play, this);
    } else if (this->av1.actionVar1 == 2) {
        if (!CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A)) {
            if (func_80856000(play, this)) {
                this->av2.actionVar2 = 0;
            } else {
                this->av1.actionVar1 = 3;
                if (this->av2.actionVar2 >= 10) {
                    this->unk_B48 = 2700.0f;
                } else {
                    this->unk_B48 = 1450.0f;
                }
                func_8082E1F0(this, NA_SE_PL_DEKUNUTS_OUT_GRD);
            }
        } else if (this->av2.actionVar2 < 15) {
            this->av2.actionVar2++;
            if (this->av2.actionVar2 == 10) {
                func_80856110(play, this, 20.0f, 3.8f, -0.1f, 140, 23, 15);
            }
        }
        func_80855F9C(play, this);
    } else {
        this->unk_ABC += this->unk_B48;

        temp_fv0_2 = this->unk_ABC;
        if (temp_fv0_2 >= 0.0f) {
            f32 speed;

            sp38 = (this->av2.actionVar2 >= 10);
            var_v1 = -1;
            speed = this->unk_B48 * this->actor.scale.y;
            if (this->actor.floorBgId != BGCHECK_SCENE) {
                dyna = DynaPoly_GetActor(&play->colCtx, this->actor.floorBgId);
                var_v1 = 0;
                if ((dyna != NULL) && (dyna->actor.id == ACTOR_OBJ_ETCETERA) && (dyna->actor.params & 0x100)) {
                    var_v1 = 1;
                    speed *= aux / 100.0f;
                }
            }

            Math_Vec3f_Copy(this->unk_AF0, &this->actor.world.pos);
            this->unk_ABC = 0.0f;
            this->actor.world.pos.y += temp_fv0_2 * this->actor.scale.y;
            func_80834DB8(this, &gPlayerAnim_pn_kakku, speed, play);
            Player_SetAction(play, this, Player_Action_94, 1);
            this->zoraBoomerangActor = NULL;

            this->stateFlags3 |= PLAYER_STATE3_200;
            if (sp38 != 0) {
                this->stateFlags3 |= PLAYER_STATE3_2000;
            }
            if (var_v1 < 0) {
                this->stateFlags3 |= PLAYER_STATE3_40000;
            }

            this->av1.actionVar1 = var_v1;
            this->av2.actionVar2 = 9999;
            Player_SetCylinderForAttack(this, DMG_DEKU_LAUNCH, 2, 20);
        } else if (this->unk_ABC < 0.0f) {
            func_80856074(play, this);
        }
    }

    if (this->unk_ABC < -1500.0f) {
        this->stateFlags3 |= PLAYER_STATE3_100;
        if (this->unk_B86[0] < 8) {
            this->unk_B86[0]++;
            if (this->unk_B86[0] == 8) {
                func_8082E1F0(this, NA_SE_PL_DEKUNUTS_BUD);
            }
        }
    }
}

void func_808566C0(PlayState* play, Player* this, PlayerBodyPart bodyPartIndex, f32 arg3, f32 arg4, f32 arg5,
                   s32 life) {
    Color_RGBA8 primColor = { 255, 200, 200, 0 };
    Color_RGBA8 envColor = { 255, 255, 0, 0 };
    static Vec3f D_8085D940 = { 0.0f, 0.3f, 0.0f };
    static Vec3f D_8085D94C = { 0.0f, -0.025f, 0.0f };
    Vec3f pos;
    s32 scale;
    f32 sp34;
    Vec3f* temp_v0;

    if (Rand_ZeroOne() < 0.5f) {
        sp34 = -1.0f;
    } else {
        sp34 = 1.0f;
    }

    D_8085D940.x = (Rand_ZeroFloat(arg4) + arg3) * sp34;
    D_8085D94C.x = arg5 * sp34;
    if (Rand_ZeroOne() < 0.5f) {
        sp34 = -1.0f;
    } else {
        sp34 = 1.0f;
    }

    temp_v0 = &this->bodyPartsPos[bodyPartIndex];
    D_8085D940.z = (Rand_ZeroFloat(arg4) + arg3) * sp34;
    D_8085D94C.z = arg5 * sp34;
    pos.x = temp_v0->x;
    pos.y = Rand_ZeroFloat(15.0f) + temp_v0->y;
    pos.z = temp_v0->z;
    if (Rand_ZeroOne() < 0.5f) {
        scale = 2000;
    } else {
        scale = -150;
    }

    EffectSsKirakira_SpawnDispersed(play, &pos, &D_8085D940, &D_8085D94C, &primColor, &envColor, scale, life);
}

void func_8085687C(Player* this) {
}

s32 func_80856888(f32* arg0, f32 arg1, f32 arg2) {
    if (arg2 != 0.0f) {
        if (arg1 < *arg0) {
            arg2 = -arg2;
        }

        *arg0 += arg2;
        if (((*arg0 - arg1) * arg2) >= 0.0f) {
            *arg0 = arg1;
            return true;
        }
    } else if (arg1 == *arg0) {
        return true;
    }

    return false;
}

f32 D_8085D958[] = { 600.0f, 960.0f };
Vec3f D_8085D960 = { -30.0f, 50.0f, 0.0f };
Vec3f D_8085D96C = { 30.0f, 50.0f, 0.0f };

// Flying as Deku?
void Player_Action_94(Player* this, PlayState* play) {
    if ((this->zoraBoomerangActor != NULL) && (this->zoraBoomerangActor->update == NULL)) {
        this->zoraBoomerangActor = NULL;
    }

    if (Player_ActionHandler_13(this, play)) {
        return;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        func_80837134(play, this);
        return;
    }

    if ((this->actor.velocity.y > 0.0f) && (this->stateFlags3 & PLAYER_STATE3_200)) {
        this->actor.terminalVelocity = -20.0f;
        this->actor.gravity = -5.5f;
        Player_SetCylinderForAttack(this, DMG_DEKU_LAUNCH, 2, 20);
        func_80856110(play, this, 0.0f, 0.0f, -1.0f, 500, 0, 8);

        if (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) {
            func_80833AA0(this, play);
        }
    } else if (!(this->stateFlags3 & PLAYER_STATE3_2000)) {
        func_80833AA0(this, play);
    } else if (this->stateFlags3 & PLAYER_STATE3_200) {
        if (this->actor.velocity.y < 0.0f) {
            if (this->av1.actionVar1 < 0) {
                func_80833AA0(this, play);
            } else {
                PlayerAnimation_Update(play, &this->skelAnime);
                if (this->skelAnime.curFrame > 6.0f) {
                    this->actor.velocity.y = 6.0f;
                    this->stateFlags3 &= ~PLAYER_STATE3_200;
                    this->stateFlags3 |= PLAYER_STATE3_1000000;
                    func_8082E1F0(this, NA_SE_IT_DEKUNUTS_FLOWER_OPEN);
                    Audio_SetSfxTimerLerpInterval(4, 2);
                }
            }
        }

        this->actor.terminalVelocity = -10.0f;
        this->actor.gravity = -0.5f;
        Player_ResetCylinder(this);
    } else if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A)) {
        func_808355D8(play, this, &gPlayerAnim_pn_kakkufinish);
    } else {
        s16 temp_a0;
        f32 temp_fv1;
        s16 sp76;
        s16 var_v1;
        s16 var_a1;
        f32 speedTarget;
        f32 sp68;
        s16 yawTarget;
        s16 temp_ft0;
        s32 temp;
        s16 var_v1_4;

        this->speedXZ = sqrtf(SQXZ(this->actor.velocity));
        if (this->speedXZ != 0.0f) {
            var_a1 = Math_Atan2S_XY(this->actor.velocity.z, this->actor.velocity.x);

            temp_a0 = this->actor.shape.rot.y - var_a1;
            if (ABS_ALT(temp_a0) > 0x4000) {
                this->speedXZ = -this->speedXZ;
                var_a1 += 0x8000;
            }
            this->yaw = var_a1;
        }

        if (this->windSpeed != 0.0f) {
            Math_SmoothStepToS(&this->unk_B8C, this->windAngleX, 3, 0x1F40, 0x190);
        }

        func_8085687C(this);

        if (this->av2.actionVar2 != 0) {
            this->av2.actionVar2--;
        }

        temp_fv1 = D_8085D958[this->av1.actionVar1] - Math_Vec3f_DistXZ(&this->actor.world.pos, this->unk_AF0);
        PlayerAnimation_Update(play, &this->skelAnime);

        if ((this->av2.actionVar2 != 0) && (temp_fv1 > 300.0f)) {
            sp76 = 0x1770;
            if (!BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pn_kakku)) {
                Player_Anim_PlayOnceFreezeAdjusted(play, this, &gPlayerAnim_pn_kakkufinish);
            } else if (PlayerAnimation_OnFrame(&this->skelAnime, 8.0f)) {
                s32 i;

                Player_TranslateAndRotateY(this, &this->actor.world.pos, &D_8085D960,
                                           &this->bodyPartsPos[PLAYER_BODYPART_LEFT_HAND]);
                Player_TranslateAndRotateY(this, &this->actor.world.pos, &D_8085D96C,
                                           &this->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND]);

                for (i = 0; i < 13; i++) {
                    func_808566C0(play, this, PLAYER_BODYPART_LEFT_HAND, 0.6f, 1.0f, 0.8f, 17);
                    func_808566C0(play, this, PLAYER_BODYPART_RIGHT_HAND, 0.6f, 1.0f, 0.8f, 17);
                }
            }
        } else if ((this->av2.actionVar2 == 0) || (temp_fv1 < 0.0f)) {
            sp76 = 0;
            func_808355D8(play, this, &gPlayerAnim_pn_rakkafinish);
        } else {
            sp76 = 0x1770 - (s32)((300.0f - temp_fv1) * 10.0f);

            if (!BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pn_batabata)) {
                Player_Anim_PlayLoopMorphAdjusted(play, this, &gPlayerAnim_pn_batabata);
            } else if (PlayerAnimation_OnFrame(&this->skelAnime, 6.0f)) {
                Player_PlaySfx(this, NA_SE_PL_DEKUNUTS_STRUGGLE);
            }
        }

        Math_AsymStepToS(&this->unk_B86[1], sp76, 0x190, 0x190);

        this->unk_B8A += this->unk_B86[1];
        temp = ABS_ALT(this->unk_B86[1]);
        if (temp > 0xFA0) {
            this->unk_B66 += (u8)(ABS_ALT(this->unk_B86[1]) * 0.01f);
        }

        if (this->unk_B66 > 200) {
            this->unk_B66 -= 200;
            func_808566C0(play, this, PLAYER_BODYPART_LEFT_HAND, 0.0f, 1.0f, 0.0f, 32);
            func_808566C0(play, this, PLAYER_BODYPART_RIGHT_HAND, 0.0f, 1.0f, 0.0f, 32);
        }

        Audio_PlaySfx_AtPosWithTimer(&this->actor.projectedPos, 0x1851, 2.0f * (this->unk_B86[1] * (1.0f / 6000.0f)));
        if ((this->zoraBoomerangActor == NULL) && CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_B)) {
            if (AMMO(ITEM_DEKU_NUT) == 0) {
                Audio_PlaySfx(NA_SE_SY_ERROR);
            } else {
                this->zoraBoomerangActor =
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ARROW, this->bodyPartsPos[PLAYER_BODYPART_WAIST].x,
                                this->bodyPartsPos[PLAYER_BODYPART_WAIST].y,
                                this->bodyPartsPos[PLAYER_BODYPART_WAIST].z, -1, 0, 0, ARROW_TYPE_DEKU_NUT);
                if (this->zoraBoomerangActor != NULL) {
                    this->zoraBoomerangActor->velocity.x = this->actor.velocity.x * 1.5f;
                    this->zoraBoomerangActor->velocity.z = this->actor.velocity.z * 1.5f;
                    Inventory_ChangeAmmo(ITEM_DEKU_NUT, -1);
                    Actor_PlaySfx(this->zoraBoomerangActor, NA_SE_PL_DEKUNUTS_DROP_BOMB);
                }
            }
        }

        if (this->actor.velocity.y < 0.0f) {
            if (sp76 != 0) {
                this->actor.terminalVelocity = -0.38f;
                this->actor.gravity = -0.2f;
            } else {
                this->actor.terminalVelocity = (this->unk_B86[1] * 0.0033f) + -20.0f;
                this->actor.gravity = (this->unk_B86[1] * 0.00004f) + (REG(68) / 100.0f);
            }
        }

        this->fallStartHeight = this->actor.world.pos.y;
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (speedTarget == 0.0f) {
            sp68 = 0.1f;
        } else {
            s16 temp_v0_6 = this->yaw - yawTarget;

            if (ABS_ALT(temp_v0_6) > 0x4000) {
                speedTarget = -speedTarget;
                yawTarget += 0x8000;
            }
            sp68 = 0.25f;
        }

        Math_SmoothStepToS(&this->unk_B8C, speedTarget * 600.0f, 8, 0xFA0, 0x64);
        Math_ScaledStepToS(&this->yaw, yawTarget, 0xFA);

        temp_ft0 = BINANG_SUB(yawTarget, this->yaw) * -2.0f;
        temp_ft0 = CLAMP(temp_ft0, -0x1F40, 0x1F40);
        Math_SmoothStepToS(&this->unk_B8E, temp_ft0, 0x14, 0x320, 0x14);
        speedTarget = (speedTarget * (this->unk_B86[1] * 0.0004f)) * fabsf(Math_SinS(this->unk_B8C));
        func_80856888(&this->speedXZ, speedTarget, sp68);

        speedTarget = sqrtf(SQ(this->speedXZ) + SQ(this->actor.velocity.y));
        if (speedTarget > 8.0f) {
            speedTarget = 8.0f / speedTarget;
            this->speedXZ *= speedTarget;
            this->actor.velocity.y *= speedTarget;
        }
    }

    func_808378FC(play, this);
}

// Deku spinning related
void Player_Action_95(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_20 | PLAYER_STATE2_40;

    PlayerAnimation_Update(play, &this->skelAnime);
    Player_SetCylinderForAttack(this, DMG_DEKU_SPIN, 1, 30);

    if (!Player_ActionHandler_13(this, play)) {
        s16 prevYaw = this->yaw;
        f32 speedTarget;
        s16 yawTarget;

        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);
        speedTarget *= 1.0f - (0.9f * ((11100.0f - this->unk_B10[0]) / 11100.0f));

        if (!func_8083A4A4(this, &speedTarget, &yawTarget, R_DECELERATE_RATE / 100.0f)) {
            func_8083CB58(this, speedTarget, yawTarget);
        }

        this->unk_B10[0] += -800.0f;
        this->actor.shape.rot.y += BINANG_ADD(TRUNCF_BINANG(this->unk_B10[0]), BINANG_SUB(this->yaw, prevYaw));

        if (Math_StepToF(&this->unk_B10[1], 0.0f, this->unk_B10[0])) {
            this->actor.shape.rot.y = this->yaw;
            func_8083B2E4(this, play);
        } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_pn_attack)) {
            this->stateFlags3 |= PLAYER_STATE3_100000;

            if (this->unk_B10[1] < 0.0f) {
                Player_Anim_PlayOnceMorph(play, this, Player_GetIdleAnim(this));
            }
        }

        func_808566C0(play, this, PLAYER_BODYPART_WAIST, 1.0f, 0.5f, 0.0f, 32);

        if (this->unk_B10[0] > 9500.0f) {
            func_8083F8A8(play, this, 2.0f, 1, 2.5f, 10, 18, true);
        }

        func_800AE930(&play->colCtx, Effect_GetByIndex(this->meleeWeaponEffectIndex[2]), &this->actor.world.pos, 2.0f,
                      this->yaw, this->actor.floorPoly, this->actor.floorBgId);
        Actor_PlaySfx_Flagged2(&this->actor, Player_GetFloorSfx(this, NA_SE_PL_SLIP_LEVEL - SFX_FLAG));
    }
}

void func_80857640(Player* this, f32 arg1, s32 arg2) {
    func_80834CD0(this, arg1, NA_SE_VO_LI_SWORD_N);
    Player_PlaySfx(this, NA_SE_PL_GORON_BALLJUMP);
    Player_StopHorizontalMovement(this);
    if (this->av2.actionVar2 < arg2) {
        this->av2.actionVar2 = arg2;
    }
    this->av1.actionVar1 = 1;
    this->unk_B48 = 1.0f;
}

void func_808576BC(PlayState* play, Player* this) {
    s32 var_v0 =
        ((this->actor.velocity.z * Math_CosS(this->yaw)) + (this->actor.velocity.x * Math_SinS(this->yaw))) * 800.0f;

    var_v0 -= this->av2.actionVar2;
    var_v0 = ABS_ALT(var_v0);

    if (var_v0 <= 0x7D0) {
        return;
    }

    if (var_v0 > 0x1770) {
        Actor_PlaySfx_Flagged2(&this->actor, NA_SE_PL_GORON_SLIP - SFX_FLAG);
    }

    if (func_8083F8A8(play, this, 12.0f, -1 - (var_v0 >> 0xC), (var_v0 >> 0xA) + 1.0f, (var_v0 >> 7) + 160, 20, true)) {
        Player_PlaySfx(this, (this->floorSfxOffset == NA_SE_PL_WALK_SNOW - SFX_FLAG)
                                 ? NA_SE_PL_ROLL_SNOW_DUST - SFX_FLAG
                                 : NA_SE_PL_ROLL_DUST - SFX_FLAG);
    }
}

void func_808577E0(Player* this) {
    f32 temp_fa1 = ABS_ALT(this->av2.actionVar2) * 0.00004f;

    if (this->unk_ABC < temp_fa1) {
        this->unk_B48 += 0.08f;
    } else {
        this->unk_B48 += -0.07f;
    }

    this->unk_B48 = CLAMP(this->unk_B48, -0.2f, 0.14f);
    if (fabsf(this->unk_B48) < 0.12f) {
        if (Math_StepUntilF(&this->unk_ABC, temp_fa1, this->unk_B48)) {
            this->unk_B48 = 0.0f;
        }
    } else {
        this->unk_ABC += this->unk_B48;
        this->unk_ABC = CLAMP(this->unk_ABC, -0.7f, 0.3f);
    }
}

s32 func_80857950(PlayState* play, Player* this) {
    if (((this->unk_B86[1] == 0) && !CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A)) ||
        ((this->av1.actionVar1 == 3) && (this->actor.velocity.y < 0.0f))) {
        Player_SetAction(play, this, Player_Action_Idle, 1);
        Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
        PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_pg_maru_change, -PLAYER_ANIM_ADJUSTED_SPEED, 7.0f,
                               0.0f, ANIMMODE_ONCE, 0.0f);
        Player_PlaySfx(this, NA_SE_PL_BALL_TO_GORON);
        return true;
    }

    return false;
}

s32 func_80857A44(PlayState* play, Player* this) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_ResetMove(this);

        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->actor.bgCheckFlags |= BGCHECKFLAG_PLAYER_800;
        this->av1.actionVar1 = 4;
        this->actor.shape.shadowScale = 30.0f;
        this->av2.actionVar2 = this->speedXZ * 500.0f;
        this->unk_B08 = this->speedXZ;
        this->unk_B0C = 0.0f;
        this->actor.home.rot.y = this->yaw;

        return true;
    }

    return false;
}

void func_80857AEC(PlayState* play, Player* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->unk_B0C += this->unk_B08 * 0.05f;

        if (this->unk_B86[1] == 0) {
            if (this->av1.actionVar1 == 1) {
                this->av1.actionVar1 = 2;
                Player_RequestQuakeAndRumble(play, this, NA_SE_PL_GORON_PUNCH);
                play->actorCtx.unk2 = 4;
                EffectSsBlast_SpawnWhiteShockwave(play, &this->actor.world.pos, &gZeroVec3f, &gZeroVec3f);
                this->av2.actionVar2 = 0;
                this->unk_B08 = 0.0f;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TEST, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, 0, 0, 0);
            } else {
                this->av1.actionVar1 = 4;
            }
        }

        Player_AnimSfx_PlayFloorLand(this);
    }
}

// Goron rolling related
void Player_Action_96(Player* this, PlayState* play) {
    if (Player_TryActionHandlerList(play, this, sActionHandlerList12, false)) {
        return;
    }

    if ((this->av1.actionVar1 == 0) && !func_80857A44(play, this)) {
        return;
    }

    this->stateFlags3 |= PLAYER_STATE3_1000;
    func_808577E0(this);

    if (!func_80857950(play, this)) {
        f32 speedTarget = 0.0f;
        s16 yawTarget = this->yaw;
        u16 spE0;
        s32 spDC;
        s32 spD8;

        if (func_80840A30(play, this, &this->unk_B08, (this->doorType == PLAYER_DOORTYPE_STAIRCASE) ? 0.0f : 12.0f)) {
            if (Player_Action_96 != this->actionFunc) {
                return;
            }

            this->speedXZ *= 0.1f;
            func_80834CD0(this, 10.0f, 0);
            if (this->unk_B86[1] != 0) {
                this->unk_B86[1] = 0;
                this->av1.actionVar1 = 3;
            }
        } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (this->unk_B08 >= 12.0f)) {
            s16 temp_v0 = this->yaw - BINANG_ADD(this->actor.wallYaw, 0x8000);
            s16 temp_v2;
            s32 var_a2 = ABS_ALT(temp_v0);

            this->unk_B0C += this->unk_B08 * 0.05f;
            temp_v2 = ((temp_v0 >= 0) ? 1 : -1) * ((var_a2 + 0x100) & ~0x1FF);
            this->yaw += BINANG_SUB(0x8000, (s16)(temp_v2 * 2));
            this->actor.home.rot.y = this->yaw;
            this->actor.shape.rot.y = this->yaw;

            this->unk_B8C = 4;
            Player_PlaySfx(this, NA_SE_IT_GORON_ROLLING_REFLECTION);
        }

        this->stateFlags2 |= (PLAYER_STATE2_20 | PLAYER_STATE2_40);

        if (this->unk_B8E != 0) {
            this->unk_B8E--;
        } else {
            Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
            speedTarget *= 2.6f;
        }

        if (this->unk_B8C != 0) {
            this->unk_B8C--;
            yawTarget = this->yaw;
        }

        if (this->unk_B86[1] != 0) {
            speedTarget = 18.0f;
            Math_StepToC(&this->av1.actionVar1, 4, 1);

            uint8_t vanillaSpikeModeCondition =
                (this->stateFlags3 & PLAYER_STATE3_80000) && (!CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A) ||
                                                              (gSaveContext.save.saveInfo.playerData.magic == 0) ||
                                                              ((this->av1.actionVar1 == 4) && (this->unk_B08 < 12.0f)));
            if (GameInteractor_Should(VB_GORON_ROLL_DISABLE_SPIKE_MODE, vanillaSpikeModeCondition)) {
                if (Math_StepToS(&this->unk_B86[1], 0, 1)) {
                    this->stateFlags3 &= ~PLAYER_STATE3_80000;
                    Magic_Reset(play);
                    Player_PlaySfx(this, NA_SE_PL_GORON_BALL_CHARGE_FAILED);
                }
                this->av1.actionVar1 = 4;
            } else if (this->unk_B86[1] < 7) {
                if (!(this->stateFlags3 & PLAYER_STATE3_80000)) {
                    this->unk_3D0.unk_00 = 2;
                }
                this->unk_B86[1]++;
            }
        }

        spDC = speedTarget * 900.0f;

        Math_AsymStepToF(&this->unk_B10[0], (this->unk_B8A != 0) ? 1.0f : 0.0f, 0.8f, 0.05f);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            func_80857AEC(play, this);
            if (this->av1.actionVar1 == 2) {
                if (this->unk_B8A == 0) {
                    this->av1.actionVar1 = 4;
                } else {
                    this->unk_B8A--;
                    this->unk_ABC = 0.0f;
                    this->unk_B48 = 0.14f;
                }
            } else if ((this->unk_B86[1] == 0) && CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_B) &&
                       (Inventory_GetBtnBItem(play) < ITEM_FD)) {
                func_80857640(this, 14.0f, 0x1F40);
            } else {
                f32 spCC;
                s16 spCA;
                s16 spC8;
                s32 var_a0;
                s32 spC0;
                f32 spBC;
                f32 spB8;
                f32 spB4;
                f32 spB0;
                f32 spAC;
                f32 spA8;
                f32 spA4;
                f32 spA0;
                Vec3f slopeNormal;
                s16 downwardSlopeYaw;
                s16 sp90 = this->yaw;
                s16 sp8E = this->yaw - this->actor.home.rot.y;
                f32 sp88 = Math_CosS(sp8E);

                if (this->unk_B86[1] == 0) {
                    this->unk_B0C = 0.0f;
                    if (this->av1.actionVar1 >= 0x36) {
                        if (GameInteractor_Should(VB_GORON_ROLL_CONSUME_MAGIC, true)) {
                            Magic_Consume(play, 2, MAGIC_CONSUME_GORON_ZORA);
                        }
                        this->unk_B08 = 18.0f;
                        this->unk_B86[1] = 1;
                        this->stateFlags3 |= PLAYER_STATE3_80000;
                        func_8082E1F0(this, NA_SE_PL_GORON_BALL_CHARGE_DASH);
                    }
                } else {
                    this->unk_B0C = CLAMP(this->unk_B0C, 0.0f, 0.9f);
                }

                spBC = (1.0f - this->unk_B0C) * this->unk_B08 * sp88;
                if ((spBC < 0.0f) || ((speedTarget == 0.0f) && (ABS_ALT(sp8E) > 0xFA0))) {
                    spBC = 0.0f;
                }

                Math_StepToF(&this->unk_B0C, 0.0f, fabsf(sp88) * 20.0f);
                var_a0 = spBC * 500.0f;
                var_a0 = CLAMP_MIN(var_a0, 0);

                spC0 = (s32)(speedTarget * 400.0f) - var_a0;
                spC0 = CLAMP_MIN(spC0, 0);

                spDC = CLAMP_MIN(spDC, var_a0);

                spAC = spBC * Math_SinS(this->actor.home.rot.y);
                spA8 = spBC * Math_CosS(this->actor.home.rot.y);
                spB4 = this->unk_B08 * Math_SinS(this->yaw);
                spB0 = this->unk_B08 * Math_CosS(this->yaw);

                spA4 = spB4 - spAC;
                spA0 = spB0 - spA8;
                this->speedXZ = spBC;
                this->yaw = this->actor.home.rot.y;
                spCC = speedTarget;
                spCA = yawTarget;

                if (func_8083A4A4(this, &spCC, &spCA, (this->av1.actionVar1 >= 5) ? 0.0f : 1.0f)) {
                    if (this->unk_B86[1] == 0) {
                        this->av1.actionVar1 = 4;
                    }

                    if (this->av1.actionVar1 == 4) {
                        spDC = -0xFA0;
                    }
                } else {
                    static Vec3f D_8085D978 = { -30.0f, 60.0f, 0.0f };
                    static Vec3f D_8085D984 = { 30.0f, 60.0f, 0.0f };
                    f32 sp84 = (((this->floorSfxOffset == NA_SE_PL_WALK_SNOW - SFX_FLAG) ||
                                 (this->floorSfxOffset == NA_SE_PL_WALK_ICE - SFX_FLAG) ||
                                 (this->floorSfxOffset == NA_SE_PL_WALK_SAND - SFX_FLAG) ||
                                 (sPlayerFloorType == FLOOR_TYPE_5)) &&
                                (spC0 >= 0x7D0))
                                   ? 0.08f
                                   : this->av2.actionVar2 * 0.0003f;
                    f32 sp80 = (Math_SinS(this->floorPitch) * 8.0f) + 0.6f;
                    s16 var_a3;
                    s16 sp7C;
                    Vec3f sp70;
                    f32 sp6C;
                    f32 var_fa1;

                    if (this->unk_B86[1] == 0) {
                        if (GameInteractor_Should(VB_GORON_ROLL_INCREASE_SPIKE_LEVEL,
                                                  (gSaveContext.magicState == MAGIC_STATE_IDLE) &&
                                                      (gSaveContext.save.saveInfo.playerData.magic >= 2) &&
                                                      (this->av2.actionVar2 >= 0x36B0))) {
                            this->av1.actionVar1++;
                            Actor_PlaySfx_Flagged2(&this->actor, NA_SE_PL_GORON_BALL_CHARGE - SFX_FLAG);
                        } else {
                            this->av1.actionVar1 = 4;
                        }
                    }

                    if (speedTarget != spCC) {
                        this->yaw = yawTarget;
                    }

                    sp84 = CLAMP_MIN(sp84, 0.0f);
                    sp80 = CLAMP_MIN(sp80, 0.0f);

                    Math_AsymStepToF(&this->speedXZ, speedTarget, sp84, sp80);
                    spC8 = TRUNCF_BINANG(fabsf(this->actor.speed) * 20.0f) + 300;
                    spC8 = CLAMP_MIN(spC8, 100);

                    sp7C = (s32)(BINANG_SUB(yawTarget, this->yaw) * -0.5f);
                    this->unk_B0C += (f32)(SQ(sp7C)) * 8e-9f;
                    Math_ScaledStepToS(&this->yaw, yawTarget, spC8);
                    sp6C = func_80835D2C(play, this, &D_8085D978, &sp70);

                    var_fa1 = func_80835D2C(play, this, &D_8085D984, &sp70) - sp6C;
                    if (fabsf(var_fa1) > 100.0f) {
                        var_fa1 = 0.0f;
                    }

                    var_a3 = Math_Atan2S_XY(60.0f, var_fa1);
                    if (ABS_ALT(var_a3) > 0x2AAA) {
                        var_a3 = 0;
                    }

                    Math_ScaledStepToS(&this->actor.shape.rot.z, var_a3 + sp7C, spC8);
                }

                spBC = this->speedXZ;
                this->actor.home.rot.y = this->yaw;
                this->yaw = sp90;
                Actor_GetSlopeDirection(this->actor.floorPoly, &slopeNormal, &downwardSlopeYaw);

                spB8 = sqrtf(SQ(spA4) + SQ(spA0));
                if (this->unk_B86[1] != 0) {
                    if ((ABS_ALT(sp8E) + ABS_ALT(this->floorPitch)) > 0x3A98) {
                        this->unk_B86[1] = 0;
                        this->av1.actionVar1 = 4;
                        this->unk_B8E = 0x14;
                        this->av2.actionVar2 = 0;
                        this->stateFlags3 &= ~PLAYER_STATE3_80000;
                        Magic_Reset(play);
                    }
                } else {
                    f32 temp_ft4_2 = (0.6f * slopeNormal.x) + spA4;
                    f32 temp_ft5 = (0.6f * slopeNormal.z) + spA0;
                    f32 temp_fv0_3 = sqrtf(SQ(temp_ft4_2) + SQ(temp_ft5));

                    if ((temp_fv0_3 < spB8) || (temp_fv0_3 < 6.0f)) {
                        spA4 = temp_ft4_2;
                        spA0 = temp_ft5;
                        spB8 = temp_fv0_3;
                    }
                }

                if (spB8 != 0.0f) {
                    s32 pad;
                    f32 sp54 = spB8 - 0.3f;

                    sp54 = CLAMP_MIN(sp54, 0.0f);

                    spB8 = sp54 / spB8;

                    spA4 *= spB8;
                    spA0 *= spB8;

                    if (sp54 != 0.0f) {
                        this->unk_B28 = Math_Atan2S_XY(spA0, spA4);
                    }

                    if (this->av2.actionVar2 == 0) {
                        s32 temp_v0_10 = this->unk_B86[0];
                        s32 temp_ft3_2 = sp54 * 800.0f;

                        this->unk_B86[0] += (s16)temp_ft3_2;
                        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (temp_ft3_2 != 0) &&
                            (((temp_v0_10 + temp_ft3_2) * temp_v0_10) <= 0)) {
                            spE0 = Player_GetFloorSfx(this, NA_SE_PL_GORON_ROLL);
                            Audio_PlaySfx_AtPosWithSyncedFreqAndVolume(&this->actor.projectedPos, spE0, sp54);
                        }
                    }
                }

                spAC = Math_SinS(this->actor.home.rot.y) * spBC;
                spA8 = Math_CosS(this->actor.home.rot.y) * spBC;

                spB4 = spAC + spA4;
                spB0 = spA8 + spA0;

                this->unk_B08 = sqrtf(SQ(spB4) + SQ(spB0));
                this->unk_B08 = CLAMP_MAX(this->unk_B08, 18.0f);

                this->yaw = Math_Atan2S_XY(spB0, spB4);
            }

            func_808576BC(play, this);

            if (ABS_ALT(this->av2.actionVar2) > 0xFA0) {
                this->stateFlags2 |= PLAYER_STATE2_8;
            }

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                this->speedXZ = this->unk_B08 * Math_CosS(this->floorPitch);
                this->actor.velocity.y = this->unk_B08 * Math_SinS(this->floorPitch);
            }

            if ((this->unk_B86[1] != 0) ||
                SurfaceType_HasMaterialProperty(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId,
                                                MATERIAL_PROPERTY_SOFT_IMPRINT)) {
                func_800AE930(&play->colCtx, Effect_GetByIndex(this->meleeWeaponEffectIndex[2]), &this->actor.world.pos,
                              15.0f, this->actor.shape.rot.y, this->actor.floorPoly, this->actor.floorBgId);
            } else {
                func_800AEF44(Effect_GetByIndex(this->meleeWeaponEffectIndex[2]));
            }
        } else {
            Math_ScaledStepToS(&this->actor.shape.rot.z, 0, 0x190);

            this->unk_B86[0] = 0;
            if (this->unk_B86[1] != 0) {
                this->actor.gravity = -1.0f;
                Math_ScaledStepToS(&this->actor.home.rot.y, yawTarget, 0x190);

                this->unk_B08 =
                    sqrtf(SQ(this->speedXZ) + SQ(this->actor.velocity.y)) * ((this->speedXZ >= 0.0f) ? 1.0f : -1.0f);
                this->unk_B08 = CLAMP_MAX(this->unk_B08, 18.0f);
            } else {
                this->unk_B48 += this->actor.velocity.y * 0.005f;
                if (this->av1.actionVar1 == 1) {
                    if (this->actor.velocity.y > 0.0f) {
                        if ((this->actor.velocity.y + this->actor.gravity) < 0.0f) {
                            this->actor.velocity.y = -this->actor.gravity;
                        }
                    } else {
                        this->unk_B8A = 0xA;
                        if (this->actor.velocity.y > -1.0f) {
                            this->actor.gravity = -0.2f;
                        } else {
                            this->unk_3D0.unk_00 = 1;
                            this->actor.gravity = -10.0f;
                        }
                    }
                }
                this->unk_B08 = this->speedXZ;
            }

            func_800AEF44(Effect_GetByIndex(this->meleeWeaponEffectIndex[2]));
        }

        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 0x7D0);

        Math_AsymStepToS(&this->av2.actionVar2, spDC, (spDC >= 0) ? 0x7D0 : 0x3E8, 0x4B0);

        if (this->av2.actionVar2 != 0) {
            spD8 = this->actor.shape.rot.x;
            this->actor.shape.rot.x += this->av2.actionVar2;

            Math_ScaledStepToS(&this->unk_B86[0], 0, ABS_ALT(this->av2.actionVar2));
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (((this->av2.actionVar2 + spD8) * spD8) <= 0)) {
                spE0 =
                    Player_GetFloorSfx(this, (this->unk_B86[1] != 0) ? NA_SE_PL_GORON_CHG_ROLL : NA_SE_PL_GORON_ROLL);
                Audio_PlaySfx_AtPosWithSyncedFreqAndVolume(&this->actor.projectedPos, spE0, this->unk_B08);
            }
        }

        if (this->av1.actionVar1 == 2) {
            Player_SetCylinderForAttack(this, DMG_GORON_POUND, 4, 60);
            Actor_SetPlayerImpact(play, PLAYER_IMPACT_GORON_GROUND_POUND, 2, 100.0f, &this->actor.world.pos);
        } else if (this->unk_B86[1] != 0) {
            Player_SetCylinderForAttack(this, DMG_GORON_SPIKES, 1, 25);
        } else {
            Player_SetCylinderForAttack(this, DMG_NORMAL_ROLL, 1, 25);
        }
    }
}

void Player_CsAnimHelper_PlayOnceMorphReset(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    Player_Anim_ZeroModelYaw(this);
    Player_Anim_PlayOnceMorph(play, this, anim);
    Player_StopHorizontalMovement(this);
}

void Player_CsAnimHelper_PlayOnceSlowMorphAdjustedReset(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    Player_Anim_ZeroModelYaw(this);
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, Animation_GetLastFrame(anim),
                           ANIMMODE_ONCE, -8.0f);
    Player_StopHorizontalMovement(this);
}

void Player_CsAnimHelper_PlayLoopSlowMorphAdjustedReset(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    Player_Anim_ZeroModelYaw(this);
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f);
    Player_StopHorizontalMovement(this);
}

PlayerCsAnim sPlayerCsActionAnimFuncs[] = {
    NULL,                                                // PLAYER_CSTYPE_NONE
    Player_CsAnim_StopHorizontalMovement,                // PLAYER_CSTYPE_ANIM_1
    Player_CsAnim_PlayOnceMorphReset,                    // PLAYER_CSTYPE_ANIM_2
    Player_CsAnim_PlayOnceSlowMorphAdjustedReset,        // PLAYER_CSTYPE_ANIM_3
    Player_CsAnim_PlayLoopSlowMorphAdjustedReset,        // PLAYER_CSTYPE_ANIM_4
    Player_CsAnim_ReplacePlayOnceNormalAdjusted,         // PLAYER_CSTYPE_ANIM_5
    Player_CsAnim_ReplacePlayOnce,                       // PLAYER_CSTYPE_ANIM_6
    Player_CsAnim_ReplacePlayLoopNormalAdjusted,         // PLAYER_CSTYPE_ANIM_7
    Player_CsAnim_ReplacePlayLoop,                       // PLAYER_CSTYPE_ANIM_8
    Player_CsAnim_PlayOnce,                              // PLAYER_CSTYPE_ANIM_9
    Player_CsAnim_PlayLoop,                              // PLAYER_CSTYPE_ANIM_10
    Player_CsAnim_Update,                                // PLAYER_CSTYPE_ANIM_11
    Player_CsAnim_PlayLoopAdjustedSlowMorphAnimSfxReset, // PLAYER_CSTYPE_ANIM_12
    Player_CsAnim_PlayLoopNormalAdjustedOnceFinished,    // PLAYER_CSTYPE_ANIM_13
    Player_CsAnim_PlayOnceFreezeReset,                   // PLAYER_CSTYPE_ANIM_14
    Player_CsAnim_PlayOnceAdjusted,                      // PLAYER_CSTYPE_ANIM_15
    Player_CsAnim_PlayLoopAdjusted,                      // PLAYER_CSTYPE_ANIM_16
    Player_CsAnim_PlayLoopAdjustedOnceFinished,          // PLAYER_CSTYPE_ANIM_17
    Player_CsAnim_PlayAnimSfx,                           // PLAYER_CSTYPE_ANIM_18
    Player_CsAnim_ReplacePlayOnceAdjustedReverse,        // PLAYER_CSTYPE_ANIM_19
};

void Player_CsAnim_StopHorizontalMovement(PlayState* play, Player* this, void* arg2) {
    Player_StopHorizontalMovement(this);
}

void Player_CsAnim_PlayOnceMorphReset(PlayState* play, Player* this, void* anim) {
    Player_CsAnimHelper_PlayOnceMorphReset(play, this, anim);
}

void Player_CsAnim_PlayOnceFreezeReset(PlayState* play, Player* this, void* anim) {
    Player_Anim_ZeroModelYaw(this);
    Player_Anim_PlayOnceFreeze(play, this, anim);
    Player_StopHorizontalMovement(this);
}

void Player_CsAnim_PlayOnceSlowMorphAdjustedReset(PlayState* play, Player* this, void* anim) {
    Player_CsAnimHelper_PlayOnceSlowMorphAdjustedReset(play, this, anim);
}

void Player_CsAnim_PlayLoopSlowMorphAdjustedReset(PlayState* play, Player* this, void* anim) {
    Player_CsAnimHelper_PlayLoopSlowMorphAdjustedReset(play, this, anim);
}

void Player_CsAnim_ReplacePlayOnceNormalAdjusted(PlayState* play, Player* this, void* anim) {
    Player_AnimReplace_PlayOnceNormalAdjusted(play, this, anim);
}

void Player_CsAnim_ReplacePlayOnce(PlayState* play, Player* this, void* anim) {
    Player_AnimReplace_PlayOnce(play, this, anim,
                                ANIM_FLAG_4 | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE | ANIM_FLAG_80);
}

void Player_CsAnim_ReplacePlayOnceAdjustedReverse(PlayState* play, Player* this, void* anim) {
    Player_Anim_PlayOnceAdjustedReverse(play, this, anim);
    Player_AnimReplace_Setup(play, this, ANIM_FLAG_4 | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE | ANIM_FLAG_80);
}

void Player_CsAnim_ReplacePlayLoopNormalAdjusted(PlayState* play, Player* this, void* anim) {
    Player_AnimReplace_PlayLoopNormalAdjusted(play, this, anim);
}

void Player_CsAnim_ReplacePlayLoop(PlayState* play, Player* this, void* anim) {
    Player_AnimReplace_PlayLoop(play, this, anim,
                                ANIM_FLAG_4 | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE | ANIM_FLAG_80);
}

void Player_CsAnim_PlayOnce(PlayState* play, Player* this, void* anim) {
    Player_Anim_PlayOnce(play, this, anim);
}

void Player_CsAnim_PlayLoop(PlayState* play, Player* this, void* anim) {
    Player_Anim_PlayLoop(play, this, anim);
}

void Player_CsAnim_PlayOnceAdjusted(PlayState* play, Player* this, void* anim) {
    Player_Anim_PlayOnceAdjusted(play, this, anim);
}

void Player_CsAnim_PlayLoopAdjusted(PlayState* play, Player* this, void* anim) {
    Player_Anim_PlayLoopAdjusted(play, this, anim);
}

void Player_CsAnim_Update(PlayState* play, Player* this, void* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
}

AnimSfxEntry D_8085D9E0[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 34, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 45, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 51, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 64, NA_SE_PL_CALM_HIT, STOP),
};
AnimSfxEntry D_8085D9F0[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 7, NA_SE_VO_LI_DEMO_DAMAGE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 18, NA_SE_PL_BOUND, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 18, NA_SE_VO_LI_FREEZE, STOP),
};
AnimSfxEntry D_8085D9FC[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_BY_AGE, 14, NA_SE_PL_LAND_GROUND, STOP),
};
AnimSfxEntry D_8085DA00[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 6, NA_SE_PL_GET_UP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 18, NA_SE_VO_LK_WAKE_UP, STOP),
};
AnimSfxEntry D_8085DA08[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_BY_AGE, 26, NA_SE_PL_LAND_GROUND, STOP),
};
AnimSfxEntry D_8085DA0C[] = {
    ANIMSFX(ANIMSFX_TYPE_8, 16, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_JUMP, 36, NA_SE_NONE, STOP),
};
AnimSfxEntry D_8085DA14[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_JUMP, 55, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 55, NA_SE_VO_LK_CATCH_DEMO, STOP),
};
AnimSfxEntry D_8085DA1C[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 4, NA_SE_VO_LK_USING_UP_ENERGY, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 16, NA_SE_PL_BOUND, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 16, NA_SE_VO_LI_DAMAGE_S, STOP),
};
AnimSfxEntry D_8085DA28[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_BY_AGE, 28, NA_SE_PL_LAND_GROUND, STOP),
};
AnimSfxEntry D_8085DA2C[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 1, NA_SE_VO_LK_USING_UP_ENERGY, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_JUMP, 42, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 44, NA_SE_VO_LI_FALL_L, STOP),
};
AnimSfxEntry D_8085DA38[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 1, NA_SE_PL_BOUND, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 1, NA_SE_VO_LI_DAMAGE_S, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_BY_AGE, 39, NA_SE_PL_LAND_GROUND, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 49, NA_SE_NONE, STOP),
};

// gPlayerAnim_cl_nigeru
AnimSfxEntry D_8085DA48[] = {
    ANIMSFX(ANIMSFX_TYPE_6, 1, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_6, 5, NA_SE_NONE, STOP),
};
AnimSfxEntry D_8085DA50[] = {
    ANIMSFX(ANIMSFX_TYPE_6, 10, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_6, 13, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_6, 16, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_6, 19, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_6, 22, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 22, NA_SE_PL_SLIP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 55, NA_SE_VO_LI_DAMAGE_S, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 62, NA_SE_NONE, STOP),
};

AnimSfxEntry D_8085DA70[] = {
    ANIMSFX(ANIMSFX_TYPE_6, 42, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_6, 48, NA_SE_NONE, STOP),
};
AnimSfxEntry D_8085DA78[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 2, NA_SE_PL_BOUND, STOP),
};
AnimSfxEntry D_8085DA7C[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 5, NA_SE_VO_LI_FREEZE, STOP),
};
AnimSfxEntry D_8085DA80[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 1, NA_SE_VO_LI_FALL_L, STOP),
};
AnimSfxEntry D_8085DA84[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 13, NA_SE_VO_LI_HANG, STOP),
};
AnimSfxEntry D_8085DA88[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 26, NA_SE_NONE, STOP),
};
AnimSfxEntry D_8085DA8C[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 4, NA_SE_VO_LI_SURPRISE, STOP),
};
AnimSfxEntry D_8085DA90[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 18, NA_SE_PL_SIT_ON_HORSE, STOP),
};

void Player_CsAnimHelper_PlayAnimSfxLostHorse(Player* this) {
    if (this->skelAnime.animation == &gPlayerAnim_lost_horse_wait) {
        Player_AnimSfx_PlayFloor(this, NA_SE_PL_SLIP_LEVEL - SFX_FLAG);
        Player_PlaySfx(this, NA_SE_VO_LK_DRAGGED_DAMAGE - SFX_FLAG);
    }
}

void Player_CsAnim_PlayLoopAdjustedSlowMorphAnimSfxReset(PlayState* play, Player* this, void* anim) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_CsAnimHelper_PlayLoopSlowMorphAdjustedReset(play, this, anim);
        this->av2.actionVar2 = 1;
    }

    if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_okiagaru_tatu)) {
        Player_PlayAnimSfx(this, D_8085DA08);
    } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_lost_horse)) {
        Player_PlayAnimSfx(this, D_8085DA14);
    } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_sirimochi)) {
        Player_PlayAnimSfx(this, D_8085DA38);
    } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_alink_somukeru)) {
        Player_PlayAnimSfx(this, D_8085DA7C);
    } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_al_fuwafuwa)) {
        Player_PlayAnimSfx(this, D_8085DA84);
    } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_cl_umanoru)) {
        Player_PlayAnimSfx(this, D_8085DA90);
    } else {
        Player_CsAnimHelper_PlayAnimSfxLostHorse(this);
    }
}

void Player_CsAnim_PlayLoopAdjustedOnceFinished(PlayState* play, Player* this, void* anim) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_ResetMove(this);
        Player_Anim_PlayLoopAdjusted(play, this, anim);
    }
}

void Player_CsAnim_PlayLoopNormalAdjustedOnceFinished(PlayState* play, Player* this, void* anim) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplace_PlayLoopNormalAdjusted(play, this, anim);
        this->av2.actionVar2 = 1;
    }
}

void Player_CsAnim_PlayAnimSfx(PlayState* play, Player* this, void* entry) {
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_PlayAnimSfx(this, entry);
}

void func_80859248(Player* this) {
    if ((this->csActor == NULL) || (this->csActor->update == NULL)) {
        this->csActor = NULL;
    }
    this->focusActor = this->csActor;
    if (this->csActor != NULL) {
        this->actor.shape.rot.y = func_8083C62C(this, 0);
    }
}

void func_8085929C(PlayState* play, Player* this, UNK_TYPE arg2) {
    this->stateFlags1 |= PLAYER_STATE1_8000000;
    this->stateFlags2 |= PLAYER_STATE2_400;
    this->stateFlags1 &= ~(PLAYER_STATE1_40000 | PLAYER_STATE1_80000);
    Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_swimer_swim);
    this->speedXZ = 0.0f;
}

void func_80859300(PlayState* play, Player* this, UNK_TYPE arg2) {
    this->actor.gravity = 0.0f;

    if (this->av1.actionVar1 == 0) {
        if ((this->transformation == PLAYER_FORM_DEKU) || func_8083B3B4(play, this, NULL)) {
            this->av1.actionVar1 = 1;
        } else {
            func_808477D0(play, this, NULL, fabsf(this->actor.velocity.y));
            Math_ScaledStepToS(&this->unk_AAA, -0x2710, 0x320);
            func_8084748C(this, &this->actor.velocity.y, 4.0f, this->yaw);
        }
    } else {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            if (this->av1.actionVar1 == 1) {
                Player_Anim_PlayLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
            } else {
                Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_swimer_swim_wait);
            }
        }
        func_808475B4(this);
        func_8084748C(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
    }
}

PlayerCsActionEntry sPlayerCsActionInitFuncs[PLAYER_CSACTION_MAX] = {
    /* PLAYER_CSACTION_NONE   */ { PLAYER_CSTYPE_NONE, { NULL } },
    /* PLAYER_CSACTION_1   */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_1 } },
    /* PLAYER_CSACTION_2   */ { PLAYER_CSTYPE_NONE, { NULL } },
    /* PLAYER_CSACTION_3   */ { PLAYER_CSTYPE_NONE, { NULL } },
    /* PLAYER_CSACTION_4   */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_demo_bikkuri } },
    /* PLAYER_CSACTION_5   */ { PLAYER_CSTYPE_NONE, { NULL } },
    /* PLAYER_CSACTION_END  */ { PLAYER_CSTYPE_NONE, { NULL } },
    /* PLAYER_CSACTION_WAIT */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_1 } },
    /* PLAYER_CSACTION_8   */ { PLAYER_CSTYPE_ANIM_2, { &gPlayerAnim_link_demo_furimuki } },
    /* PLAYER_CSACTION_9   */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_5 } },
    /* PLAYER_CSACTION_10  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_demo_warp } },
    /* PLAYER_CSACTION_11  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_clink_demo_standup } },
    /* PLAYER_CSACTION_12  */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_clink_demo_standup_wait } },
    /* PLAYER_CSACTION_13  */ { PLAYER_CSTYPE_ANIM_2, { &gPlayerAnim_link_demo_baru_op3 } },
    /* PLAYER_CSACTION_14  */ { PLAYER_CSTYPE_NONE, { NULL } },
    /* PLAYER_CSACTION_15  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_demo_jibunmiru } },
    /* PLAYER_CSACTION_16  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_14 } },
    /* PLAYER_CSACTION_17  */ { PLAYER_CSTYPE_ANIM_2, { &gPlayerAnim_link_normal_okarina_end } },
    /* PLAYER_CSACTION_18  */ { PLAYER_CSTYPE_ANIM_16, { &gPlayerAnim_link_normal_hang_up_down } },
    /* PLAYER_CSACTION_19  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_16 } },
    /* PLAYER_CSACTION_20  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_1 } },
    /* PLAYER_CSACTION_21  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_clink_demo_mimawasi } },
    /* PLAYER_CSACTION_22  */ { PLAYER_CSTYPE_ANIM_6, { &gPlayerAnim_om_get_mae } },
    /* PLAYER_CSACTION_23  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_demo_look_hand } },
    /* PLAYER_CSACTION_24  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_normal_wait_typeB_20f } },
    /* PLAYER_CSACTION_25  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_17 } },
    /* PLAYER_CSACTION_26  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_37 } },
    /* PLAYER_CSACTION_27  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_demo_zeldamiru } },
    /* PLAYER_CSACTION_28  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_demo_kenmiru1 } },
    /* PLAYER_CSACTION_29  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_demo_kenmiru2 } },
    /* PLAYER_CSACTION_30  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_demo_kenmiru2_modori } },
    /* PLAYER_CSACTION_31  */ { PLAYER_CSTYPE_ANIM_6, { &gameplay_keep_Linkanim_00D310 } },
    /* PLAYER_CSACTION_32  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_22 } },
    /* PLAYER_CSACTION_33  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_demo_rakka } },
    /* PLAYER_CSACTION_34  */ { PLAYER_CSTYPE_ANIM_4, { &gPlayerAnim_demo_pikupiku } },
    /* PLAYER_CSACTION_35  */ { PLAYER_CSTYPE_ANIM_3, { &gameplay_keep_Linkanim_00D2B8 } },
    /* PLAYER_CSACTION_36  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_25 } },
    /* PLAYER_CSACTION_37  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_27 } },
    /* PLAYER_CSACTION_38  */ { PLAYER_CSTYPE_ANIM_6, { &gameplay_keep_Linkanim_00D278 } },
    /* PLAYER_CSACTION_39  */ { PLAYER_CSTYPE_ANIM_6, { &gameplay_keep_Linkanim_00D288 } },
    /* PLAYER_CSACTION_40  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_rakuba } },
    /* PLAYER_CSACTION_41  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_bajyo_furikaeru } },
    /* PLAYER_CSACTION_42  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_okiagaru } },
    /* PLAYER_CSACTION_43  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_okiagaru_tatu } },
    /* PLAYER_CSACTION_44  */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_bajyo_walk } },
    /* PLAYER_CSACTION_45  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_rakka } },
    /* PLAYER_CSACTION_46  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_sirimochi } },
    /* PLAYER_CSACTION_47  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_spotlight } },
    /* PLAYER_CSACTION_48  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_al_hensin } },
    /* PLAYER_CSACTION_49  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_dl_jibunmiru } },
    /* PLAYER_CSACTION_50  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_vs_yousei } },
    /* PLAYER_CSACTION_51  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_urusai } },
    /* PLAYER_CSACTION_52  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_okarinatori } },
    /* PLAYER_CSACTION_53  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_lost_horse } },
    /* PLAYER_CSACTION_54  */ { PLAYER_CSTYPE_ANIM_4, { &gPlayerAnim_lost_horse_wait } },
    /* PLAYER_CSACTION_55  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_lost_horse2 } },
    /* PLAYER_CSACTION_56  */ { PLAYER_CSTYPE_ANIM_14, { &gPlayerAnim_okarinatori } },
    /* PLAYER_CSACTION_57  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_cl_tobikakaru } },
    /* PLAYER_CSACTION_58  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_5 } },
    /* PLAYER_CSACTION_59  */ { PLAYER_CSTYPE_ANIM_5, { &gameplay_keep_Linkanim_00D0A0 } },
    /* PLAYER_CSACTION_60  */ { PLAYER_CSTYPE_ANIM_2, { &gPlayerAnim_cl_furafura } },
    /* PLAYER_CSACTION_61  */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_cl_nigeru } },
    /* PLAYER_CSACTION_62  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_cl_ononoki } },
    /* PLAYER_CSACTION_63  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_al_gaku } },
    /* PLAYER_CSACTION_64  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_al_fuwafuwa } },
    /* PLAYER_CSACTION_65  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_al_fuwafuwa_modori } },
    /* PLAYER_CSACTION_66  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_al_elf_tobidasi } },
    /* PLAYER_CSACTION_67  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_3 } },
    /* PLAYER_CSACTION_68  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_29 } },
    /* PLAYER_CSACTION_69  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_31 } },
    /* PLAYER_CSACTION_70  */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_cl_tewofuru } },
    /* PLAYER_CSACTION_71  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_cl_jibun_miru } },
    /* PLAYER_CSACTION_72  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_cl_hoo } },
    /* PLAYER_CSACTION_73  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_al_yareyare } },
    /* PLAYER_CSACTION_74  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_al_yes } },
    /* PLAYER_CSACTION_75  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_al_no } },
    /* PLAYER_CSACTION_76  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_al_unun } },
    /* PLAYER_CSACTION_77  */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_dl_yusaburu } },
    /* PLAYER_CSACTION_78  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_dl_kokeru } },
    /* PLAYER_CSACTION_79  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_alink_powerup } },
    /* PLAYER_CSACTION_80  */ { PLAYER_CSTYPE_ANIM_4, { &gPlayerAnim_alink_rakkatyu } },
    /* PLAYER_CSACTION_81  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_alink_kyoro } },
    /* PLAYER_CSACTION_82  */ { PLAYER_CSTYPE_ANIM_4, { &gPlayerAnim_alink_yurayura } },
    /* PLAYER_CSACTION_83  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_alink_somukeru } },
    /* PLAYER_CSACTION_84  */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_alink_fukitobu } },
    /* PLAYER_CSACTION_85  */ { PLAYER_CSTYPE_ANIM_3, { &gameplay_keep_Linkanim_00CFC8 } },
    /* PLAYER_CSACTION_86  */ { PLAYER_CSTYPE_ANIM_4, { &gPlayerAnim_alink_tereru } },
    /* PLAYER_CSACTION_87  */ { PLAYER_CSTYPE_ANIM_5, { &gameplay_keep_Linkanim_00D1D0 } },
    /* PLAYER_CSACTION_88  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_alink_kaitenmiss } },
    /* PLAYER_CSACTION_89  */ { PLAYER_CSTYPE_ANIM_4, { &gameplay_keep_Linkanim_00CFC0 } },
    /* PLAYER_CSACTION_90  */ { PLAYER_CSTYPE_ANIM_4, { &gameplay_keep_Linkanim_00CFB8 } },
    /* PLAYER_CSACTION_91  */ { PLAYER_CSTYPE_ANIM_4, { &gameplay_keep_Linkanim_00D050 } },
    /* PLAYER_CSACTION_92  */ { PLAYER_CSTYPE_ANIM_4, { &gameplay_keep_Linkanim_00D048 } },
    /* PLAYER_CSACTION_93  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_42 } },
    /* PLAYER_CSACTION_94  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_alink_ozigi } },
    /* PLAYER_CSACTION_95  */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_alink_ozigi_modori } },
    /* PLAYER_CSACTION_96  */ { PLAYER_CSTYPE_ANIM_9, { &gPlayerAnim_link_normal_back_downA } },
    /* PLAYER_CSACTION_97  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_35 } },
    /* PLAYER_CSACTION_98  */ { PLAYER_CSTYPE_ANIM_15, { &gPlayerAnim_cl_maskoff } },
    /* PLAYER_CSACTION_99  */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_cl_kubisime } },
    /* PLAYER_CSACTION_100 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_alink_ee } },
    /* PLAYER_CSACTION_101 */ { PLAYER_CSTYPE_ANIM_3, { &gameplay_keep_Linkanim_00CFF0 } },
    /* PLAYER_CSACTION_102 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_40 } },
    /* PLAYER_CSACTION_103 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_45 } },
    /* PLAYER_CSACTION_104 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_cl_dakisime } },
    /* PLAYER_CSACTION_105 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_kf_omen } },
    /* PLAYER_CSACTION_106 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_kf_dakiau } },
    /* PLAYER_CSACTION_107 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_kf_hanare } },
    /* PLAYER_CSACTION_108 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_kf_miseau } },
    /* PLAYER_CSACTION_109 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_kf_awase } },
    /* PLAYER_CSACTION_110 */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_kf_tetunagu_loop } },
    /* PLAYER_CSACTION_111 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_keirei } },
    /* PLAYER_CSACTION_112 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_cl_umanoru } },
    /* PLAYER_CSACTION_113 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_cl_wakare } },
    /* PLAYER_CSACTION_114 */ { PLAYER_CSTYPE_ANIM_4, { &gPlayerAnim_alink_dance_loop } },
    /* PLAYER_CSACTION_115 */ { PLAYER_CSTYPE_ANIM_2, { &gPlayerAnim_link_demo_goma_furimuki } },
    /* PLAYER_CSACTION_116 */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_link_uma_anim_fastrun } },
    /* PLAYER_CSACTION_117 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_cl_umamiage } },
    /* PLAYER_CSACTION_118 */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_demo_suwari1 } },
    /* PLAYER_CSACTION_119 */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_demo_suwari2 } },
    /* PLAYER_CSACTION_120 */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_demo_suwari3 } },
    /* PLAYER_CSACTION_121 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_7 } },
    /* PLAYER_CSACTION_122 */ { PLAYER_CSTYPE_NONE, { NULL } },
    /* PLAYER_CSACTION_123 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_9 } },
    /* PLAYER_CSACTION_124 */ { PLAYER_CSTYPE_ANIM_7, { &gPlayerAnim_clink_demo_get1 } },
    /* PLAYER_CSACTION_125 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_clink_demo_get2 } },
    /* PLAYER_CSACTION_126 */ { PLAYER_CSTYPE_ANIM_5, { &gPlayerAnim_clink_demo_get3 } },
    /* PLAYER_CSACTION_127 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_demo_gurad } },
    /* PLAYER_CSACTION_128 */ { PLAYER_CSTYPE_ANIM_4, { &gPlayerAnim_link_demo_sita_wait } },
    /* PLAYER_CSACTION_129 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_L_1kyoro } },
    /* PLAYER_CSACTION_130 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_L_2kyoro } },
    /* PLAYER_CSACTION_131 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_L_sagaru } },
    /* PLAYER_CSACTION_132 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_L_bouzen } },
    /* PLAYER_CSACTION_133 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_L_kamaeru } },
    /* PLAYER_CSACTION_134 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_L_hajikareru } },
    /* PLAYER_CSACTION_135 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_L_ken_miru } },
    /* PLAYER_CSACTION_136 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_L_mukinaoru } },
    /* PLAYER_CSACTION_137 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_demo_return_to_past } },
    /* PLAYER_CSACTION_138 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_last_hit_motion1 } },
    /* PLAYER_CSACTION_139 */ { PLAYER_CSTYPE_ANIM_3, { &gPlayerAnim_link_last_hit_motion2 } },
};

PlayerCsActionEntry sPlayerCsActionUpdateFuncs[PLAYER_CSACTION_MAX] = {
    /* PLAYER_CSACTION_NONE   */ { PLAYER_CSTYPE_NONE, { NULL } },
    /* PLAYER_CSACTION_1   */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_0 } },
    /* PLAYER_CSACTION_2   */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_11 } },
    /* PLAYER_CSACTION_3   */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_13 } },
    /* PLAYER_CSACTION_4   */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_5   */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_48 } },
    /* PLAYER_CSACTION_END  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_End } },
    /* PLAYER_CSACTION_WAIT */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_2 } },
    /* PLAYER_CSACTION_8   */ { PLAYER_CSTYPE_ANIM_18, { D_8085DA70 } },
    /* PLAYER_CSACTION_9   */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_6 } },
    /* PLAYER_CSACTION_10  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_15 } },
    /* PLAYER_CSACTION_11  */ { PLAYER_CSTYPE_ANIM_18, { D_8085D9E0 } },
    /* PLAYER_CSACTION_12  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_13  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_14  */ { PLAYER_CSTYPE_NONE, { NULL } },
    /* PLAYER_CSACTION_15  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_16  */ { PLAYER_CSTYPE_ANIM_17, { &gPlayerAnim_link_normal_okarina_swing } },
    /* PLAYER_CSACTION_17  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_18  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_19  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_39 } },
    /* PLAYER_CSACTION_20  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_2 } },
    /* PLAYER_CSACTION_21  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_clink_demo_mimawasi_wait } },
    /* PLAYER_CSACTION_22  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_19 } },
    /* PLAYER_CSACTION_23  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_link_demo_look_hand_wait } },
    /* PLAYER_CSACTION_24  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_25  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_26  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_38 } },
    /* PLAYER_CSACTION_27  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_link_demo_zeldamiru_wait } },
    /* PLAYER_CSACTION_28  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_link_demo_kenmiru1_wait } },
    /* PLAYER_CSACTION_29  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_link_demo_kenmiru2_wait } },
    /* PLAYER_CSACTION_30  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_demo_link_nwait } },
    /* PLAYER_CSACTION_31  */ { PLAYER_CSTYPE_ANIM_12, { &gameplay_keep_Linkanim_00D318 } },
    /* PLAYER_CSACTION_32  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_23 } },
    /* PLAYER_CSACTION_33  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_TranslateReverse } },
    /* PLAYER_CSACTION_34  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_35  */ { PLAYER_CSTYPE_ANIM_12, { &gameplay_keep_Linkanim_00D2C0 } },
    /* PLAYER_CSACTION_36  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_26 } },
    /* PLAYER_CSACTION_37  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_28 } },
    /* PLAYER_CSACTION_38  */ { PLAYER_CSTYPE_ANIM_12, { &gameplay_keep_Linkanim_00D280 } },
    /* PLAYER_CSACTION_39  */ { PLAYER_CSTYPE_ANIM_12, { &gameplay_keep_Linkanim_00D290 } },
    /* PLAYER_CSACTION_40  */ { PLAYER_CSTYPE_ANIM_18, { D_8085D9F0 } },
    /* PLAYER_CSACTION_41  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_42  */ { PLAYER_CSTYPE_ANIM_18, { D_8085DA00 } },
    /* PLAYER_CSACTION_43  */ { PLAYER_CSTYPE_ANIM_13, { &gPlayerAnim_okiagaru_wait } },
    /* PLAYER_CSACTION_44  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_32 } },
    /* PLAYER_CSACTION_45  */ { PLAYER_CSTYPE_ANIM_18, { D_8085DA2C } },
    /* PLAYER_CSACTION_46  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_sirimochi_wait } },
    /* PLAYER_CSACTION_47  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_spotlight_wait } },
    /* PLAYER_CSACTION_48  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_al_hensin_loop } },
    /* PLAYER_CSACTION_49  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_dl_jibunmiru_wait } },
    /* PLAYER_CSACTION_50  */ { PLAYER_CSTYPE_ANIM_18, { D_8085DA50 } },
    /* PLAYER_CSACTION_51  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_52  */ { PLAYER_CSTYPE_ANIM_18, { D_8085D9FC } },
    /* PLAYER_CSACTION_53  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_lost_horse_wait } },
    /* PLAYER_CSACTION_54  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_32 } },
    /* PLAYER_CSACTION_55  */ { PLAYER_CSTYPE_ANIM_18, { D_8085DA1C } },
    /* PLAYER_CSACTION_56  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_57  */ { PLAYER_CSTYPE_ANIM_18, { D_8085DA0C } },
    /* PLAYER_CSACTION_58  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_8 } },
    /* PLAYER_CSACTION_59  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_60  */ { PLAYER_CSTYPE_ANIM_18, { D_8085DA28 } },
    /* PLAYER_CSACTION_61  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_32 } },
    /* PLAYER_CSACTION_62  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_63  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_64  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_al_fuwafuwa_loop } },
    /* PLAYER_CSACTION_65  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_33 } },
    /* PLAYER_CSACTION_66  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_67  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_4 } },
    /* PLAYER_CSACTION_68  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_30 } },
    /* PLAYER_CSACTION_69  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_70  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_71  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_33 } },
    /* PLAYER_CSACTION_72  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_73  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_74  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_75  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_76  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_77  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_78  */ { PLAYER_CSTYPE_ANIM_18, { D_8085DA78 } },
    /* PLAYER_CSACTION_79  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_alink_powerup_loop } },
    /* PLAYER_CSACTION_80  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_32 } },
    /* PLAYER_CSACTION_81  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_alink_kyoro_loop } },
    /* PLAYER_CSACTION_82  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_83  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_alink_somukeru_loop } },
    /* PLAYER_CSACTION_84  */ { PLAYER_CSTYPE_ANIM_18, { D_8085DA80 } },
    /* PLAYER_CSACTION_85  */ { PLAYER_CSTYPE_ANIM_12, { &gameplay_keep_Linkanim_00CFD0 } },
    /* PLAYER_CSACTION_86  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_87  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_88  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_89  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_90  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_91  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_92  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_93  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_43 } },
    /* PLAYER_CSACTION_94  */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_alink_ozigi_loop } },
    /* PLAYER_CSACTION_95  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_96  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_34 } },
    /* PLAYER_CSACTION_97  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_36 } },
    /* PLAYER_CSACTION_98  */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_46 } },
    /* PLAYER_CSACTION_99  */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_100 */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_alink_ee_loop } },
    /* PLAYER_CSACTION_101 */ { PLAYER_CSTYPE_ANIM_12, { &gameplay_keep_Linkanim_00CFF8 } },
    /* PLAYER_CSACTION_102 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_41 } },
    /* PLAYER_CSACTION_103 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_104 */ { PLAYER_CSTYPE_ANIM_13, { &gPlayerAnim_cl_dakisime_loop } },
    /* PLAYER_CSACTION_105 */ { PLAYER_CSTYPE_ANIM_13, { &gPlayerAnim_kf_omen_loop } },
    /* PLAYER_CSACTION_106 */ { PLAYER_CSTYPE_ANIM_13, { &gPlayerAnim_kf_dakiau_loop } },
    /* PLAYER_CSACTION_107 */ { PLAYER_CSTYPE_ANIM_13, { &gPlayerAnim_kf_hanare_loop } },
    /* PLAYER_CSACTION_108 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_109 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_110 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_111 */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_link_kei_wait } },
    /* PLAYER_CSACTION_112 */ { PLAYER_CSTYPE_ANIM_13, { &gPlayerAnim_cl_umanoru_loop } },
    /* PLAYER_CSACTION_113 */ { PLAYER_CSTYPE_ANIM_13, { &gPlayerAnim_cl_wakare_loop } },
    /* PLAYER_CSACTION_114 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_115 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_44 } },
    /* PLAYER_CSACTION_116 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_117 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_32 } },
    /* PLAYER_CSACTION_118 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_119 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_120 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_121 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_8 } },
    /* PLAYER_CSACTION_122 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_12 } },
    /* PLAYER_CSACTION_123 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_10 } },
    /* PLAYER_CSACTION_125 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_124 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_126 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_127 */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_link_demo_gurad_wait } },
    /* PLAYER_CSACTION_128 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_129 */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_L_kw } },
    /* PLAYER_CSACTION_130 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_131 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_132 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_133 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_134 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_18 } },
    /* PLAYER_CSACTION_135 */ { PLAYER_CSTYPE_ANIM_11, { NULL } },
    /* PLAYER_CSACTION_136 */ { PLAYER_CSTYPE_ANIM_12, { &gPlayerAnim_L_kennasi_w } },
    /* PLAYER_CSACTION_137 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_20 } },
    /* PLAYER_CSACTION_138 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_21 } },
    /* PLAYER_CSACTION_139 */ { PLAYER_CSTYPE_ACTION, { Player_CsAction_21 } },
};

void Player_CsAction_0(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80859248(this);

    if (func_801242B4(this)) {
        func_80859300(play, this, 0);
    } else {
        PlayerAnimation_Update(play, &this->skelAnime);
        if (func_801240DC(this) || (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            Player_UpdateUpperBody(this, play);
        } else if ((this->interactRangeActor != NULL) && (this->interactRangeActor->textId == 0xFFFF)) {
            Player_ActionHandler_2(this, play);
        }
    }
}

void Player_CsAction_1(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (this->stateFlags1 & PLAYER_STATE1_8000000) {
        func_8085929C(play, this, 0);
    } else {
        PlayerAnimationHeader* anim = D_8085BE84[PLAYER_ANIMGROUP_nwait][this->modelAnimType];

        if ((this->cueId == PLAYER_CUEID_6) || (this->cueId == PLAYER_CUEID_46)) {
            Player_Anim_PlayOnce(play, this, anim);
        } else {
            Player_Anim_ZeroModelYaw(this);
            PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f,
                                   Animation_GetLastFrame(anim), ANIMMODE_LOOP, -4.0f);
        }
        Player_StopHorizontalMovement(this);
    }
}

void Player_CsAction_2(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (func_80847880(play, this)) {
        return;
    }

    if ((this->csAction == PLAYER_CSACTION_20) && (play->csCtx.state == CS_STATE_IDLE)) {
        Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_END);
    } else if (this->stateFlags1 & PLAYER_STATE1_8000000) {
        func_80859300(play, this, 0);
        this->actor.velocity.y = 0.0f;
    } else {
        PlayerAnimation_Update(play, &this->skelAnime);
        if (func_801240DC(this) || (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            Player_UpdateUpperBody(this, play);
        }
    }
}

void Player_CsAction_3(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (this->actor.id == ACTOR_EN_TEST3) {
        func_80838830(this, OBJECT_GI_MSSA);
        this->stateFlags1 |= PLAYER_STATE1_400;
    }

    Player_Anim_PlayOnceAdjusted(play, this,
                                 (this->transformation == PLAYER_FORM_DEKU) ? &gPlayerAnim_pn_getA
                                                                            : &gPlayerAnim_link_demo_get_itemA);
}

void Player_CsAction_4(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
    if ((this->actor.id == ACTOR_EN_TEST3) && Animation_OnFrame(&this->skelAnime, 20.0f)) {
        this->getItemDrawIdPlusOne = GID_MASK_SUN + 1;
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_ESCAPED_SAKONS_HIDEOUT);
        Audio_PlayFanfare(NA_BGM_GET_NEW_MASK);
    }
}

void Player_CsAction_5(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 linearVelocity;
    s16 yaw;

    this->stateFlags1 &= ~PLAYER_STATE1_ZORA_BOOMERANG_THROWN;

    yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_3A0);
    linearVelocity = this->speedXZ;
    this->actor.world.rot.y = yaw;
    this->actor.shape.rot.y = yaw;
    this->yaw = yaw;
    if (linearVelocity <= 0.0f) {
        this->speedXZ = 0.1f;
    } else if (linearVelocity > 2.5f) {
        this->speedXZ = 2.5f;
    }

    if ((this->transformation != PLAYER_FORM_HUMAN) && (play->roomCtx.curRoom.type == ROOM_TYPE_BOSS)) {
        R_PLAY_FILL_SCREEN_ON = 45;
        R_PLAY_FILL_SCREEN_R = 255;
        R_PLAY_FILL_SCREEN_G = 255;
        R_PLAY_FILL_SCREEN_B = 255;
        R_PLAY_FILL_SCREEN_ALPHA = 0;
        Audio_PlaySfx(NA_SE_SY_WHITE_OUT_T);
    }
}

void Player_CsAction_6(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 sp24;

    if (R_PLAY_FILL_SCREEN_ON > 0) {
        R_PLAY_FILL_SCREEN_ALPHA += R_PLAY_FILL_SCREEN_ON;
        if (R_PLAY_FILL_SCREEN_ALPHA > 255) {
            R_PLAY_FILL_SCREEN_ON = -64;
            R_PLAY_FILL_SCREEN_ALPHA = 255;
            gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
            this->actor.update = func_8012301C;
            this->actor.draw = NULL;
            this->av1.actionVar1 = 0;
        }
    } else if (R_PLAY_FILL_SCREEN_ON < 0) {
        R_PLAY_FILL_SCREEN_ALPHA += R_PLAY_FILL_SCREEN_ON;
        if (R_PLAY_FILL_SCREEN_ALPHA < 0) {
            R_PLAY_FILL_SCREEN_ON = 0;
            R_PLAY_FILL_SCREEN_ALPHA = 0;
        }
    } else {
        sp24 = 2.5f;
        func_808411D4(play, this, &sp24, 0xA);
        this->av2.actionVar2++;
        if (this->av2.actionVar2 >= 0x15) {
            this->csAction = PLAYER_CSACTION_10;
        }
    }
}

void Player_CsAction_7(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->speedXZ = 2.5f;
    func_80835BF8(&this->actor.world.pos, this->actor.shape.rot.y, 180.0f, &this->unk_3A0);
}

void Player_CsAction_8(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 sp1C = 2.5f;

    func_808411D4(play, this, &sp1C, 0xA);
}

void Player_CsAction_9(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_8083B23C(this, play);
}

void Player_CsAction_10(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80859248(this);
    if (this->av2.actionVar2 != 0) {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            Player_Anim_PlayLoop(play, this, func_8082EF54(this));
            this->av2.actionVar2 = 0;
        }
        func_8082FC60(this);
    } else {
        func_8083E958(play, this);
    }
}

void Player_CsAction_11(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80840F90(play, this, cue, 0.0f, 0, 0);
}

void Player_CsAction_12(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.shape.face = PLAYER_FACE_SMILE;
    func_80840F90(play, this, cue, 0.0f, 0, 0);
}

void Player_CsAction_13(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80840F90(play, this, cue, 0.0f, 0, 1);
}

void Player_CsAction_14(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_CsAnimHelper_PlayOnceSlowMorphAdjustedReset(play, this, &gPlayerAnim_link_normal_okarina_start);
    this->itemAction = PLAYER_IA_OCARINA;
    Player_SetModels(this, Player_ActionToModelGroup(this, this->itemAction));
}

void Player_Cutscene_Translate(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 startX = cue->startPos.x;
    f32 startY = cue->startPos.y;
    f32 startZ = cue->startPos.z;
    f32 diffX = cue->endPos.x - startX;
    f32 diffY = cue->endPos.y - startY;
    f32 diffZ = cue->endPos.z - startZ;
    f32 progress = (((f32)(play->csCtx.curFrame - cue->startFrame)) / ((f32)(cue->endFrame - cue->startFrame)));

    this->actor.world.pos.x = (diffX * progress) + startX;
    this->actor.world.pos.y = (diffY * progress) + startY;
    this->actor.world.pos.z = (diffZ * progress) + startZ;
}

void Player_CsAction_15(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (cue != NULL) {
        Player_Cutscene_Translate(play, this, cue);
    }

    PlayerAnimation_Update(play, &this->skelAnime);
}

void Player_CsAction_16(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_Anim_PlayLoopMorph(play, this, D_8085BE84[PLAYER_ANIMGROUP_nwait][this->modelAnimType]);
    Player_StopHorizontalMovement(this);
}

void func_80859CE0(PlayState* play, Player* this, s32 arg2) {
    this->actor.draw = Player_Draw;
}

void Player_CsAction_17(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_PutSwordInHand(play, this, false);
    Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_demo_return_to_past);
}

void Player_CsAction_18(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
}

PlayerAnimationHeader* D_8085E354[PLAYER_FORM_MAX] = {
    &gPlayerAnim_L_okarina_get, // PLAYER_FORM_FIERCE_DEITY
    &gPlayerAnim_L_okarina_get, // PLAYER_FORM_GORON
    &gPlayerAnim_L_okarina_get, // PLAYER_FORM_ZORA
    &gPlayerAnim_L_okarina_get, // PLAYER_FORM_DEKU
    &gPlayerAnim_om_get,        // PLAYER_FORM_HUMAN
};

struct_8085E368 D_8085E368[PLAYER_FORM_MAX] = {
    { { -200, 700, 100 }, { 800, 600, 800 } }, // PLAYER_FORM_FIERCE_DEITY
    { { -200, 700, 100 }, { 800, 600, 800 } }, // PLAYER_FORM_GORON
    { { -200, 700, 100 }, { 800, 600, 800 } }, // PLAYER_FORM_ZORA
    { { -200, 700, 100 }, { 800, 600, 800 } }, // PLAYER_FORM_DEKU
    { { -200, 500, 0 }, { 600, 400, 600 } },   // PLAYER_FORM_HUMAN
};

Color_RGBA8 D_8085E3A4 = { 255, 255, 255, 0 };
Color_RGBA8 D_8085E3A8 = { 0, 128, 128, 0 };

void Player_CsAction_19(PlayState* play, Player* this, CsCmdActorCue* cue) {
    struct_8085E368* posInfo;
    Vec3f effectPos;
    Vec3f randPos;

    Player_CsAnim_PlayLoopNormalAdjustedOnceFinished(play, this, D_8085E354[this->transformation]);

    if (this->rightHandType != 0xFF) {
        this->rightHandType = 0xFF;
    } else {
        posInfo = &D_8085E368[this->transformation];
        randPos.x = Rand_CenteredFloat(posInfo->range.x) + posInfo->base.x;
        randPos.y = Rand_CenteredFloat(posInfo->range.y) + posInfo->base.y;
        randPos.z = Rand_CenteredFloat(posInfo->range.z) + posInfo->base.z;
        SkinMatrix_Vec3fMtxFMultXYZ(&this->shieldMf, &randPos, &effectPos);
        EffectSsKirakira_SpawnDispersed(play, &effectPos, &gZeroVec3f, &gZeroVec3f, &D_8085E3A4, &D_8085E3A8, 600, -10);
    }
}

void Player_CsAction_20(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_CsAction_End(play, this, cue);
    } else if (this->av2.actionVar2 == 0) {
        Item_Give(play, ITEM_SWORD_RAZOR);
        Player_PutSwordInHand(play, this, false);
    } else {
        func_808484CC(this);
    }
}

void Player_CsAction_21(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        func_8083FCF0(play, this, 0.0f, 99.0f, this->skelAnime.endFrame - 8.0f);
    }
    if (this->heldItemAction != PLAYER_IA_SWORD_GILDED) {
        Player_PutSwordInHand(play, this, true);
    }
}

void Player_CsAction_22(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (this->transformation != PLAYER_FORM_DEKU) {
        gSaveContext.save.playerForm = PLAYER_FORM_DEKU;
    }
}

void Player_CsAction_23(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
    if (GET_PLAYER_FORM != this->transformation) {
        this->actor.update = func_8012301C;
        this->actor.draw = NULL;
    }
}

void Player_CsAction_TranslateReverse(PlayState* play, Player* this, CsCmdActorCue* cue2) {
    CsCmdActorCue* cue = cue2;
    f32 xEnd = cue->endPos.x;
    f32 yEnd = cue->endPos.y;
    f32 zEnd = cue->endPos.z;
    f32 xDiff = cue->startPos.x - xEnd;
    f32 yDiff = cue->startPos.y - yEnd;
    f32 zDiff = cue->startPos.z - zEnd;
    f32 progress = (f32)(cue->endFrame - play->csCtx.curFrame) / (f32)(cue->endFrame - cue->startFrame);

    this->actor.world.pos.x = (xDiff * progress) + xEnd;
    this->actor.world.pos.y = (yDiff * progress) + yEnd;
    this->actor.world.pos.z = (zDiff * progress) + zEnd;
    PlayerAnimation_Update(play, &this->skelAnime);
}

void Player_CsAction_25(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (this->transformation != PLAYER_FORM_FIERCE_DEITY) {
        gSaveContext.save.playerForm = PLAYER_FORM_FIERCE_DEITY;
    }
}

void Player_CsAction_26(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
    if (GET_PLAYER_FORM != this->transformation) {
        this->actor.update = func_8012301C;
        this->actor.draw = NULL;
    }
}

void Player_CsAction_27(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_Anim_PlayOnce(play, this, &gPlayerAnim_demo_rakka);
    this->unk_AAA = -0x8000;
}

void Player_CsAction_28(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
    this->actor.gravity = 0.0f;
    Math_StepToF(&this->actor.velocity.y, -this->actor.terminalVelocity, -((f32)REG(68) / 100.0f));
}

void Player_CsAction_29(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_Anim_PlayOnceAdjusted(play, this, D_8085D17C[this->transformation]);
    this->itemAction = PLAYER_IA_OCARINA;
    Player_SetModels(this, Player_ActionToModelGroup(this, this->itemAction));
}

void Player_CsAction_30(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if ((PlayerAnimation_Update(play, &this->skelAnime)) &&
        (BEN_ANIM_EQUAL(this->skelAnime.animation, D_8085D17C[this->transformation]))) {
        func_808525C4(play, this);
        return;
    }
    if (this->av2.actionVar2 != 0) {
        func_8085255C(play, this);
    }
}

void Player_CsAction_31(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_Anim_PlayOnceAdjustedReverse(play, this, D_8085D17C[this->transformation]);
}

void Player_CsAction_32(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_Cutscene_Translate(play, this, cue);
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplace_PlayLoopNormalAdjusted(play, this, &gPlayerAnim_cl_umamiage_loop);
    }

    if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_cl_nigeru)) {
        Player_PlayAnimSfx(this, D_8085DA48);
    } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_alink_rakkatyu)) {
        Actor_PlaySfx_Flagged2(&this->actor, NA_SE_PL_FLYING_AIR - SFX_FLAG);
    } else {
        Player_CsAnimHelper_PlayAnimSfxLostHorse(this);
    }
}

void Player_CsAction_33(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_CsAction_16(play, this, cue);
    } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_al_fuwafuwa_modori)) {
        Player_PlayAnimSfx(this, D_8085DA88);
    } else if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_cl_jibun_miru)) {
        Player_PlayAnimSfx(this, D_8085DA8C);
    }
}

void Player_CsAction_34(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime) && (this->av2.actionVar2 == 0) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_back_downB);
        this->av2.actionVar2 = 1;
    }
    if (this->av2.actionVar2 != 0) {
        Player_DecelerateToZero(this);
    }
}

void Player_CsAction_35(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_normal_give_other, PLAYER_ANIM_NORMAL_SPEED,
                           (play->sceneId == SCENE_ALLEY) ? IREG(56) : 0.0f,
                           Animation_GetLastFrame(&gPlayerAnim_link_normal_give_other), ANIMMODE_ONCE, -8.0f);
}

void Player_CsAction_36(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->av2.actionVar2++ >= 0x15) {
            PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_pz_wait, PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f,
                                   ANIMMODE_LOOP, -16.0f);
        }
    }
}

void Player_CsAction_37(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (func_801242B4(this)) {
        func_8085929C(play, this, 0);
    } else {
        Player_CsAnim_PlayOnceSlowMorphAdjustedReset(play, this, &gPlayerAnim_link_demo_kousan);
    }
}

void Player_CsAction_38(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (func_801242B4(this)) {
        func_80859300(play, this, 0);
    } else {
        Player_CsAnim_Update(play, this, cue);
    }
}

void Player_CsAction_39(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_CsAnim_Update(play, this, cue);
    if (Player_ActionHandler_2(this, play)) {
        play->csCtx.state = CS_STATE_STOP;
        CutsceneManager_Stop(CutsceneManager_GetCurrentCsId());
    }
}

void Player_CsAction_40(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80838830(this, OBJECT_GI_RESERVE_C_01);
    Player_CsAnim_PlayOnceSlowMorphAdjustedReset(play, this, &gPlayerAnim_link_normal_give_other);
    this->stateFlags2 &= ~PLAYER_STATE2_1000000;
}

void Player_CsAction_41(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->av2.actionVar2 == 0) {
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) ||
                (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->getItemDrawIdPlusOne = GID_NONE + 1;
                this->av2.actionVar2 = -1;
            } else {
                this->getItemDrawIdPlusOne = GID_PENDANT_OF_MEMORIES + 1;
            }
        } else if (this->av2.actionVar2 < 0) {
            if (Actor_HasParent(&this->actor, play) ||
                !GameInteractor_Should(VB_GIVE_PENDANT_OF_MEMORIES_FROM_KAFEI, true)) {
                this->actor.parent = NULL;
                this->av2.actionVar2 = 1;
            } else {
                Actor_OfferGetItem(&this->actor, play, GI_PENDANT_OF_MEMORIES, 9999.9f, 9999.9f);
            }
        }
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_PENDANT_OF_MEMORIES);
    }
}

void Player_CsAction_42(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if ((this->transformation != PLAYER_FORM_HUMAN) && (play->roomCtx.curRoom.type == ROOM_TYPE_BOSS)) {
        R_PLAY_FILL_SCREEN_ON = 45;
        R_PLAY_FILL_SCREEN_R = 255;
        R_PLAY_FILL_SCREEN_G = 255;
        R_PLAY_FILL_SCREEN_B = 255;
        R_PLAY_FILL_SCREEN_ALPHA = 0;
        Audio_PlaySfx(NA_SE_SY_WHITE_OUT_T);
    }
}

void Player_CsAction_43(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (R_PLAY_FILL_SCREEN_ON > 0) {
        R_PLAY_FILL_SCREEN_ALPHA += R_PLAY_FILL_SCREEN_ON;
        if (R_PLAY_FILL_SCREEN_ALPHA > 255) {
            R_PLAY_FILL_SCREEN_ON = -64;
            R_PLAY_FILL_SCREEN_ALPHA = 255;
            gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
            this->actor.update = func_8012301C;
            this->actor.draw = NULL;
            this->av1.actionVar1 = 0;
        }
    } else if (R_PLAY_FILL_SCREEN_ON < 0) {
        R_PLAY_FILL_SCREEN_ALPHA += R_PLAY_FILL_SCREEN_ON;
        if (R_PLAY_FILL_SCREEN_ALPHA < 0) {
            R_PLAY_FILL_SCREEN_ON = 0;
            R_PLAY_FILL_SCREEN_ALPHA = 0;
        }
    } else {
        PlayerAnimation_Update(play, &this->skelAnime);
    }
}

void Player_CsAction_44(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime) && (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_DOOR)) {
        CutsceneManager_Stop(CS_ID_GLOBAL_DOOR);
    }
}

void Player_CsAction_45(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80848640(play, this);
}

void Player_CsAction_46(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        this->stateFlags2 |= PLAYER_STATE2_1000000;
    }
}

void Player_CsAction_End(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (func_801242B4(this)) {
        func_808353DC(play, this);
        func_8082DC64(play, this);
    } else {
        func_80839ED0(this, play);
        if (!Player_ActionHandler_Talk(this, play)) {
            Player_ActionHandler_2(this, play);
        }
    }

    this->csAction = PLAYER_CSACTION_NONE;
    this->unk_AA5 = PLAYER_UNKAA5_0;
}

void Player_Cutscene_SetPosAndYawToStart(Player* this, CsCmdActorCue* cue) {
    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;
    this->actor.world.pos.z = cue->startPos.z;

    this->yaw = this->actor.shape.rot.y = cue->rot.y;
}

void Player_Cutscene_8085ABA8(Player* this, CsCmdActorCue* cue) {
    f32 xDiff = cue->startPos.x - (s32)this->actor.world.pos.x;
    f32 yDiff = cue->startPos.y - (s32)this->actor.world.pos.y;
    f32 zDiff = cue->startPos.z - (s32)this->actor.world.pos.z;
    f32 dist;
    s16 temp_v0;

    temp_v0 = (s16)cue->rot.y - this->actor.shape.rot.y;
    dist = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
    if (this->speedXZ == 0.0f) {
        if ((dist > 50.0f) || (ABS_ALT(temp_v0) > 0x4000)) {
            Player_Cutscene_SetPosAndYawToStart(this, cue);
        }
    }

    this->skelAnime.movementFlags = 0;
    Player_Anim_ZeroModelYaw(this);
}

void func_8085AC9C(PlayState* play, Player* this, CsCmdActorCue* cue, PlayerCsActionEntry* csEntry) {
    if (csEntry->type > PLAYER_CSTYPE_NONE) {
        sPlayerCsActionAnimFuncs[csEntry->type](play, this, csEntry->csAnimArg2);
    } else if (csEntry->type <= PLAYER_CSTYPE_ACTION) {
        csEntry->csActionFunc(play, this, cue);
    }

    if ((D_80862B6C & ANIM_FLAG_4) && !(this->skelAnime.movementFlags & ANIM_FLAG_4)) {
        this->skelAnime.morphTable[LIMB_ROOT_POS].y /= this->ageProperties->unk_08;
        D_80862B6C = 0;
    }
}

void func_8085AD5C(PlayState* play, Player* this, PlayerCsAction csAction) {
    if ((csAction != PLAYER_CSACTION_1) && (csAction != PLAYER_CSACTION_WAIT) && (csAction != PLAYER_CSACTION_20) &&
        (csAction != PLAYER_CSACTION_END)) {
        Player_DetachHeldActor(play, this);
    }
}

void Player_CsAction_48(PlayState* play, Player* this, CsCmdActorCue* cue) {
    CsCmdActorCue* playerCue = (this->actor.id == ACTOR_EN_TEST3)
                                   ? play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_506)]
                                   : play->csCtx.playerCue;
    s32 var_a0 = false;
    s32 pad;
    s32 csAction;

    if ((play->csCtx.state == CS_STATE_IDLE) || (play->csCtx.state == CS_STATE_STOP) ||
        (play->csCtx.state == CS_STATE_RUN_UNSTOPPABLE)) {
        if ((sPlayerCueToCsActionMap[this->cueId] == PLAYER_CSACTION_68) && (play->sceneId == SCENE_OKUJOU)) {
            this->unk_AA5 = PLAYER_UNKAA5_5;

            if (Player_ActionHandler_13(this, play)) {
                this->csAction = PLAYER_CSACTION_NONE;
            }
            return;
        }

        var_a0 = true;

        if (sPlayerCueToCsActionMap[this->cueId] != PLAYER_CSACTION_16) {
            this->csAction = PLAYER_CSACTION_END;
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_END);
            this->cueId = PLAYER_CUEID_NONE;
            Player_StopHorizontalMovement(this);
            return;
        }
    }

    if (!var_a0 && (playerCue == NULL)) {
        this->actor.flags &= ~ACTOR_FLAG_INSIDE_CULLING_VOLUME;
        return;
    }

    if (!var_a0 && (this->cueId != playerCue->id)) {
        csAction = sPlayerCueToCsActionMap[playerCue->id];

        // Negative csActions will skip this block
        if ((csAction >= PLAYER_CSACTION_NONE) && !gDisablePlayerCsActionStartPos) {
            if ((csAction == PLAYER_CSACTION_2) || (csAction == PLAYER_CSACTION_3)) {
                Player_Cutscene_8085ABA8(this, playerCue);
            } else {
                Player_Cutscene_SetPosAndYawToStart(this, playerCue);
            }
        }

        if (csAction == PLAYER_CSACTION_108) {
            this->stateFlags3 |= PLAYER_STATE3_20000000;
        } else if (csAction == PLAYER_CSACTION_110) {
            this->stateFlags3 &= ~PLAYER_STATE3_20000000;
        }

        D_80862B6C = this->skelAnime.movementFlags;

        Player_Anim_ResetMove(this);
        func_8085AD5C(play, this, ABS_ALT(csAction));
        func_8085AC9C(play, this, playerCue, &sPlayerCsActionInitFuncs[ABS_ALT(csAction)]);

        this->av2.actionVar2 = 0;
        this->av1.actionVar1 = 0;
        this->cueId = playerCue->id;
    }

    csAction = sPlayerCueToCsActionMap[this->cueId];
    func_8085AC9C(play, this, playerCue, &sPlayerCsActionUpdateFuncs[ABS_ALT(csAction)]);

    if ((u16)playerCue->rot.x != 0) {
        Math_SmoothStepToS(&this->actor.focus.rot.x, (u16)playerCue->rot.x, 4, 0x2710, 0);
        func_80832754(this, false);
    }
}

void Player_Action_CsAction(Player* this, PlayState* play) {
    if (this->csAction != this->prevCsAction) {
        D_80862B6C = this->skelAnime.movementFlags;
        Player_Anim_ResetMove(this);

        this->prevCsAction = this->csAction;
        func_8085AD5C(play, this, this->csAction);
        func_8085AC9C(play, this, NULL, &sPlayerCsActionInitFuncs[this->csAction]);
    }

    func_8085AC9C(play, this, NULL, &sPlayerCsActionUpdateFuncs[this->csAction]);
}

s32 Player_StartFishing(PlayState* play) {
    Player* player = GET_PLAYER(play);

    func_8082DE50(play, player);
    Player_UseItem(play, player, ITEM_FISHING_ROD);
    return 1;
}

// Player_GrabPlayerImpl? Player_GrabPlayerNoChecks?
void func_8085B170(PlayState* play, Player* this) {
    func_8082DE50(play, this);
    Player_SetAction(play, this, Player_Action_72, 0);
    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_re_dead_attack);
    this->stateFlags2 |= PLAYER_STATE2_80;
    func_8082DAD4(this);
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HELD);
}

s32 Player_GrabPlayer(PlayState* play, Player* this) {
    if (!Player_InBlockingCsMode(play, this) && (this->invincibilityTimer >= 0) && !func_801240DC(this)) {
        if (!(this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 | PLAYER_STATE1_100000 |
                                   PLAYER_STATE1_200000 | PLAYER_STATE1_800000))) {
            if (!(this->stateFlags2 & PLAYER_STATE2_80) && !(this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT)) {
                func_8085B170(play, this);
                return true;
            }
        }
    }

    return false;
}

s32 Player_TryCsAction(PlayState* play, Player* this, PlayerCsAction csAction) {
    Player* player = GET_PLAYER(play);

    if (this != NULL) {
        if (csAction == PLAYER_CSACTION_NONE) {
            return Player_Action_36 == this->actionFunc;
        }

        // Specific to Kafei, any negative csAction works
        if ((this->actor.id == ACTOR_EN_TEST3) && (csAction < 0)) {
            // PLAYER_CSACTION_NEG1
            Player_SetupTurnInPlace(play, this, this->actor.home.rot.y);
            return false;
        }

        if (this->actor.id == ACTOR_EN_TEST3) {
            player = this;
        }
    }

    if ((player->actor.id == ACTOR_EN_TEST3) || !Player_InBlockingCsMode(play, player)) {
        func_8082DE50(play, player);
        Player_SetAction(play, player, Player_Action_CsAction, 0);
        player->csAction = csAction;
        player->csActor = &this->actor;
        func_8082DAD4(player);

        return true;
    }

    return false;
}

void func_8085B384(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_Idle, 1);
    Player_Anim_PlayOnceMorph(play, this, Player_GetIdleAnim(this));
    this->yaw = this->actor.shape.rot.y;
}

/**
 * Returns true if Player's health reaches zero
 */
s32 Player_InflictDamage(PlayState* play, s32 damage) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags2 & PLAYER_STATE2_80) || !Player_InBlockingCsMode(play, player)) {
        if (func_808339D4(play, player, damage) == 0) {
            player->stateFlags2 &= ~PLAYER_STATE2_80;
            return true;
        }
    }

    return false;
}

/**
 * Start talking to the specified actor.
 */
void Player_StartTalking(PlayState* play, Actor* actor) {
    s32 pad;
    Player* this = GET_PLAYER(play);

    func_808323C0(this, CS_ID_GLOBAL_TALK);

    if ((this->talkActor != NULL) || (actor == this->tatlActor) ||
        CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_WITH_C_UP)) {
        actor->flags |= ACTOR_FLAG_TALK;
    }

    this->talkActor = actor;
    this->exchangeItemAction = PLAYER_IA_NONE;
    this->focusActor = actor;

    if (actor->textId == 0xFFFF) {
        // Player will stand and look at the actor with no text appearing.
        // This can be used to delay text from appearing, for example.
        Player_SetCsActionWithHaltedActors(play, actor, PLAYER_CSACTION_1);
        actor->flags |= ACTOR_FLAG_TALK;
        Player_PutAwayHeldItem(play, this);
    } else {
        if (this->actor.flags & ACTOR_FLAG_TALK) {
            this->actor.textId = 0;
        } else {
            this->actor.flags |= ACTOR_FLAG_TALK;
            this->actor.textId = actor->textId;
        }

        if (this->stateFlags1 & PLAYER_STATE1_800000) {
            s32 sp24 = this->av2.actionVar2;

            Player_PutAwayHeldItem(play, this);
            Player_SetupTalk(play, this);
            this->av2.actionVar2 = sp24;
        } else {
            if (func_801242B4(this)) {
                Player_SetupWaitForPutAway(play, this, Player_SetupTalk);
                Player_Anim_PlayLoopSlowMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
            } else if ((actor->category != ACTORCAT_NPC) || (this->heldItemAction == PLAYER_IA_FISHING_ROD)) {
                Player_SetupTalk(play, this);

                if (!Player_CheckHostileLockOn(this)) {
                    if ((actor != this->tatlActor) && (actor->xzDistToPlayer < (actor->colChkInfo.cylRadius + 40))) {
                        Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_normal_backspace);
                    } else {
                        Player_Anim_PlayLoop(play, this, Player_GetIdleAnim(this));
                    }
                }
            } else {
                Player_SetupWaitForPutAway(play, this, Player_SetupTalk);
                Player_Anim_PlayOnceAdjusted(play, this,
                                             (actor->xzDistToPlayer < (actor->colChkInfo.cylRadius + 40))
                                                 ? &gPlayerAnim_link_normal_backspace
                                                 : &gPlayerAnim_link_normal_talk_free);
            }

            if (BEN_ANIM_EQUAL(this->skelAnime.animation, gPlayerAnim_link_normal_backspace)) {
                Player_AnimReplace_Setup(play, this, ANIM_FLAG_1 | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_NOMOVE);
            }
            func_8082DAD4(this);
        }

        this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_20000000;
    }

    if ((this->tatlActor == this->talkActor) && ((this->talkActor->textId & 0xFF00) != 0x200)) {
        this->tatlActor->flags |= ACTOR_FLAG_TALK;
    }
}

void func_8085B74C(PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 temp_fv1;
    f32 linearVelocity = player->speedXZ;

    if (linearVelocity < 0.0f) {
        linearVelocity = -linearVelocity;
        player->actor.world.rot.y += 0x8000;
    }

    temp_fv1 = R_RUN_SPEED_LIMIT / 100.0f;

    if (temp_fv1 < linearVelocity) {
        gSaveContext.entranceSpeed = temp_fv1;
    } else {
        gSaveContext.entranceSpeed = linearVelocity;
    }

    func_80835324(play, player, 400.0f,
                  (sPlayerConveyorSpeedIndex != CONVEYOR_SPEED_DISABLED) ? sPlayerConveyorYaw
                                                                         : player->actor.world.rot.y);
    player->stateFlags1 |= (PLAYER_STATE1_1 | PLAYER_STATE1_20000000);
}

void func_8085B820(PlayState* play, s16 arg1) {
    Player* player = GET_PLAYER(play);

    player->actor.focus.rot.y = arg1;
    func_80836D8C(player);
}

PlayerItemAction func_8085B854(PlayState* play, Player* this, ItemId itemId) {
    PlayerItemAction itemAction = Player_ItemToItemAction(this, itemId);

    if ((itemAction >= PLAYER_IA_MASK_MIN) && (itemAction <= PLAYER_IA_MASK_MAX) &&
        GameInteractor_Should(VB_GET_ITEM_ACTION_FROM_MASK, itemAction == GET_IA_FROM_MASK(this->currentMask),
                              itemAction)) {
        itemAction = PLAYER_IA_NONE;
    }

    if ((itemAction <= PLAYER_IA_NONE) || (itemAction >= PLAYER_IA_MAX)) {
        return PLAYER_IA_MINUS1;
    }

    this->itemAction = PLAYER_IA_NONE;
    this->actionFunc = NULL;
    Player_SetAction_PreserveItemAction(play, this, Player_Action_ExchangeItem, 0);
    this->csId = CS_ID_GLOBAL_TALK;
    this->itemAction = itemAction;
    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_give_other);
    this->stateFlags1 |= (PLAYER_STATE1_TALKING | PLAYER_STATE1_20000000);
    this->getItemDrawIdPlusOne = GID_NONE + 1;
    this->exchangeItemAction = itemAction;

    return itemAction;
}

s32 func_8085B930(PlayState* play, PlayerAnimationHeader* talkAnim, AnimationMode animMode) {
    Player* player = GET_PLAYER(play);

    if (!(player->actor.flags & ACTOR_FLAG_TALK)) {
        return false;
    }

    // 2S2H [Port] We are setting the result of Player_GetIdleAnim to talkAnim ahead of time
    // so that Animation_GetLastframe returns a real value
    if (talkAnim == NULL) {
        talkAnim = Player_GetIdleAnim(player);
    }

    //! @bug When Player_GetIdleAnim is used to get a wait animation, NULL is still passed to Animation_GetLastFrame,
    // causing it to read the frame count from address 0x80000000 casted to AnimationHeaderCommon via
    // Lib_SegmentedToVirtual operating on NULL, which ends up returning 15385 as the last frame
    PlayerAnimation_Change(play, &player->skelAnime, (talkAnim == NULL) ? Player_GetIdleAnim(player) : talkAnim,
                           PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, Animation_GetLastFrame(talkAnim), animMode, -6.0f);
    return true;
}
