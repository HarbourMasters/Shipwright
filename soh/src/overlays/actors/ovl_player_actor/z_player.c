/*
 * File: z_player.c
 * Overlay: ovl_player_actor
 * Description: Link
 */

#include "ultra64.h"
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
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_link_child/object_link_child.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"

typedef struct {
    /* 0x00 */ u8 itemId;
    /* 0x01 */ u8 field; // various bit-packed data
    /* 0x02 */ s8 gi;    // defines the draw id and chest opening animation
    /* 0x03 */ u8 textId;
    /* 0x04 */ u16 objectId;
} GetItemEntry; // size = 0x06

#define GET_ITEM(itemId, objectId, drawId, textId, field, chestAnim) \
    { itemId, field, (chestAnim != CHEST_ANIM_SHORT ? 1 : -1) * (drawId + 1), textId, objectId }

#define CHEST_ANIM_SHORT 0
#define CHEST_ANIM_LONG 1

#define GET_ITEM_NONE \
    { ITEM_NONE, 0, 0, 0, OBJECT_INVALID }

typedef enum {
    /* 0x00 */ KNOB_ANIM_ADULT_L,
    /* 0x01 */ KNOB_ANIM_CHILD_L,
    /* 0x02 */ KNOB_ANIM_ADULT_R,
    /* 0x03 */ KNOB_ANIM_CHILD_R
} KnobDoorAnim;

typedef struct {
    /* 0x00 */ u8 itemId;
    /* 0x02 */ s16 actorId;
} ExplosiveInfo; // size = 0x04

typedef struct {
    /* 0x00 */ s16 actorId;
    /* 0x02 */ u8 itemId;
    /* 0x03 */ u8 actionParam;
    /* 0x04 */ u8 textId;
} BottleCatchInfo; // size = 0x06

typedef struct {
    /* 0x00 */ s16 actorId;
    /* 0x02 */ s16 actorParams;
} BottleDropInfo; // size = 0x04

typedef struct {
    /* 0x00 */ s8 damage;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u16 sfxId;
} FallImpactInfo; // size = 0x06

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yaw;
} SpecialRespawnInfo; // size = 0x10

typedef struct {
    /* 0x00 */ u16 sfxId;
    /* 0x02 */ s16 field;
} struct_80832924; // size = 0x04

typedef struct {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ s16 unk_02;
} struct_808551A4; // size = 0x04

typedef struct {
    /* 0x00 */ LinkAnimationHeader* anim;
    /* 0x04 */ u8 unk_04;
} struct_808540F4; // size = 0x08

typedef struct {
    /* 0x00 */ LinkAnimationHeader* unk_00;
    /* 0x04 */ LinkAnimationHeader* unk_04;
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ u8 unk_09;
} struct_80854554; // size = 0x0C

typedef struct {
    /* 0x00 */ LinkAnimationHeader* unk_00;
    /* 0x04 */ LinkAnimationHeader* unk_04;
    /* 0x08 */ LinkAnimationHeader* unk_08;
    /* 0x0C */ u8 unk_0C;
    /* 0x0D */ u8 unk_0D;
} struct_80854190; // size = 0x10

typedef struct {
    /* 0x00 */ LinkAnimationHeader* anim;
    /* 0x04 */ f32 unk_04;
    /* 0x04 */ f32 unk_08;
} struct_80854578; // size = 0x0C

typedef struct {
    /* 0x00 */ s8 type;
    /* 0x04 */ union {
        void* ptr;
        void (*func)(GlobalContext*, Player*, CsCmdActorAction*);
    };
} struct_80854B18; // size = 0x08

typedef struct {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
} struct_80858AC8; // size = 0x0A

void func_80833770(GlobalContext* globalCtx, Player* this);
void func_80833790(GlobalContext* globalCtx, Player* this);
void func_8083379C(GlobalContext* globalCtx, Player* this);
void func_8083377C(GlobalContext* globalCtx, Player* this);
void func_808337D4(GlobalContext* globalCtx, Player* this);
void func_80833910(GlobalContext* globalCtx, Player* this);
void func_80833984(GlobalContext* globalCtx, Player* this);
void func_8083399C(GlobalContext* globalCtx, Player* this, s8 actionParam);
s32 func_8083485C(Player* this, GlobalContext* globalCtx);
s32 func_808349DC(Player* this, GlobalContext* globalCtx);
s32 func_80834A2C(Player* this, GlobalContext* globalCtx);
s32 func_80834B5C(Player* this, GlobalContext* globalCtx);
s32 func_80834C74(Player* this, GlobalContext* globalCtx);
s32 func_8083501C(Player* this, GlobalContext* globalCtx);
s32 func_808351D4(Player* this, GlobalContext* globalCtx);
s32 func_808353D8(Player* this, GlobalContext* globalCtx);
s32 func_80835588(Player* this, GlobalContext* globalCtx);
s32 func_808356E8(Player* this, GlobalContext* globalCtx);
s32 func_80835800(Player* this, GlobalContext* globalCtx);
s32 func_80835884(Player* this, GlobalContext* globalCtx);
s32 func_808358F0(Player* this, GlobalContext* globalCtx);
s32 func_808359FC(Player* this, GlobalContext* globalCtx);
s32 func_80835B60(Player* this, GlobalContext* globalCtx);
s32 func_80835C08(Player* this, GlobalContext* globalCtx);
void func_80835F44(GlobalContext* globalCtx, Player* this, s32 item);
void func_80839F90(Player* this, GlobalContext* globalCtx);
s32 func_80838A14(Player* this, GlobalContext* globalCtx);
s32 func_80839800(Player* this, GlobalContext* globalCtx);
s32 func_8083B040(Player* this, GlobalContext* globalCtx);
s32 func_8083B998(Player* this, GlobalContext* globalCtx);
s32 func_8083B644(Player* this, GlobalContext* globalCtx);
s32 func_8083BDBC(Player* this, GlobalContext* globalCtx);
s32 func_8083C1DC(Player* this, GlobalContext* globalCtx);
s32 func_8083C2B0(Player* this, GlobalContext* globalCtx);
s32 func_8083C544(Player* this, GlobalContext* globalCtx);
s32 func_8083C61C(GlobalContext* globalCtx, Player* this);
void func_8083CA20(GlobalContext* globalCtx, Player* this);
void func_8083CA54(GlobalContext* globalCtx, Player* this);
void func_8083CA9C(GlobalContext* globalCtx, Player* this);
s32 func_8083E0FC(Player* this, GlobalContext* globalCtx);
s32 func_8083E5A8(Player* this, GlobalContext* globalCtx);
s32 func_8083EB44(Player* this, GlobalContext* globalCtx);
s32 func_8083F7BC(Player* this, GlobalContext* globalCtx);
void func_80840450(Player* this, GlobalContext* globalCtx);
void func_808407CC(Player* this, GlobalContext* globalCtx);
void func_80840BC8(Player* this, GlobalContext* globalCtx);
void func_80840DE4(Player* this, GlobalContext* globalCtx);
void func_808414F8(Player* this, GlobalContext* globalCtx);
void func_8084170C(Player* this, GlobalContext* globalCtx);
void func_808417FC(Player* this, GlobalContext* globalCtx);
void func_8084193C(Player* this, GlobalContext* globalCtx);
void func_80841BA8(Player* this, GlobalContext* globalCtx);
void func_80842180(Player* this, GlobalContext* globalCtx);
void func_8084227C(Player* this, GlobalContext* globalCtx);
void func_8084279C(Player* this, GlobalContext* globalCtx);
void func_808423EC(Player* this, GlobalContext* globalCtx);
void func_8084251C(Player* this, GlobalContext* globalCtx);
void func_80843188(Player* this, GlobalContext* globalCtx);
void func_808435C4(Player* this, GlobalContext* globalCtx);
void func_8084370C(Player* this, GlobalContext* globalCtx);
void func_8084377C(Player* this, GlobalContext* globalCtx);
void func_80843954(Player* this, GlobalContext* globalCtx);
void func_80843A38(Player* this, GlobalContext* globalCtx);
void func_80843CEC(Player* this, GlobalContext* globalCtx);
void func_8084411C(Player* this, GlobalContext* globalCtx);
void func_80844708(Player* this, GlobalContext* globalCtx);
void func_80844A44(Player* this, GlobalContext* globalCtx);
void func_80844AF4(Player* this, GlobalContext* globalCtx);
void func_80844E68(Player* this, GlobalContext* globalCtx);
void func_80845000(Player* this, GlobalContext* globalCtx);
void func_80845308(Player* this, GlobalContext* globalCtx);
void func_80845668(Player* this, GlobalContext* globalCtx);
void func_808458D0(Player* this, GlobalContext* globalCtx);
void func_80845CA4(Player* this, GlobalContext* globalCtx);
void func_80845EF8(Player* this, GlobalContext* globalCtx);
void func_80846050(Player* this, GlobalContext* globalCtx);
void func_80846120(Player* this, GlobalContext* globalCtx);
void func_80846260(Player* this, GlobalContext* globalCtx);
void func_80846358(Player* this, GlobalContext* globalCtx);
void func_80846408(Player* this, GlobalContext* globalCtx);
void func_808464B0(Player* this, GlobalContext* globalCtx);
void func_80846578(Player* this, GlobalContext* globalCtx);
void func_80846648(GlobalContext* globalCtx, Player* this);
void func_80846660(GlobalContext* globalCtx, Player* this);
void func_808467D4(GlobalContext* globalCtx, Player* this);
void func_808468A8(GlobalContext* globalCtx, Player* this);
void func_808468E8(GlobalContext* globalCtx, Player* this);
void func_80846978(GlobalContext* globalCtx, Player* this);
void func_808469BC(GlobalContext* globalCtx, Player* this);
void func_80846A68(GlobalContext* globalCtx, Player* this);
void func_8084B1D8(Player* this, GlobalContext* globalCtx);
void func_8084B530(Player* this, GlobalContext* globalCtx);
void func_8084B78C(Player* this, GlobalContext* globalCtx);
void func_8084B898(Player* this, GlobalContext* globalCtx);
void func_8084B9E4(Player* this, GlobalContext* globalCtx);
void func_8084BBE4(Player* this, GlobalContext* globalCtx);
void func_8084BDFC(Player* this, GlobalContext* globalCtx);
void func_8084BF1C(Player* this, GlobalContext* globalCtx);
void Player_UpdateCommon(Player* this, GlobalContext* globalCtx, Input* input);
void func_8084C5F8(Player* this, GlobalContext* globalCtx);
void func_8084C760(Player* this, GlobalContext* globalCtx);
void func_8084C81C(Player* this, GlobalContext* globalCtx);
void func_8084CC98(Player* this, GlobalContext* globalCtx);
void func_8084D3E4(Player* this, GlobalContext* globalCtx);
void func_8084D610(Player* this, GlobalContext* globalCtx);
void func_8084D7C4(Player* this, GlobalContext* globalCtx);
void func_8084D84C(Player* this, GlobalContext* globalCtx);
void func_8084DAB4(Player* this, GlobalContext* globalCtx);
void func_8084DC48(Player* this, GlobalContext* globalCtx);
void func_8084E1EC(Player* this, GlobalContext* globalCtx);
void func_8084E30C(Player* this, GlobalContext* globalCtx);
void func_8084E368(Player* this, GlobalContext* globalCtx);
void func_8084E3C4(Player* this, GlobalContext* globalCtx);
void func_8084E604(Player* this, GlobalContext* globalCtx);
void func_8084E6D4(Player* this, GlobalContext* globalCtx);
void func_8084E9AC(Player* this, GlobalContext* globalCtx);
void func_8084EAC0(Player* this, GlobalContext* globalCtx);
void func_8084ECA4(Player* this, GlobalContext* globalCtx);
void func_8084EED8(Player* this, GlobalContext* globalCtx);
void func_8084EFC0(Player* this, GlobalContext* globalCtx);
void func_8084F104(Player* this, GlobalContext* globalCtx);
void func_8084F390(Player* this, GlobalContext* globalCtx);
void func_8084F608(Player* this, GlobalContext* globalCtx);
void func_8084F698(Player* this, GlobalContext* globalCtx);
void func_8084F710(Player* this, GlobalContext* globalCtx);
void func_8084F88C(Player* this, GlobalContext* globalCtx);
void func_8084F9A0(Player* this, GlobalContext* globalCtx);
void func_8084F9C0(Player* this, GlobalContext* globalCtx);
void func_8084FA54(Player* this, GlobalContext* globalCtx);
void func_8084FB10(Player* this, GlobalContext* globalCtx);
void func_8084FBF4(Player* this, GlobalContext* globalCtx);
s32 func_8084FCAC(Player* this, GlobalContext* globalCtx);
void func_8084FF7C(Player* this);
void func_8085002C(Player* this);
s32 func_80850224(Player* this, GlobalContext* globalCtx);
void func_808502D0(Player* this, GlobalContext* globalCtx);
void func_808505DC(Player* this, GlobalContext* globalCtx);
void func_8085063C(Player* this, GlobalContext* globalCtx);
void func_8085076C(Player* this, GlobalContext* globalCtx);
void func_808507F4(Player* this, GlobalContext* globalCtx);
void func_80850AEC(Player* this, GlobalContext* globalCtx);
void func_80850C68(Player* this, GlobalContext* globalCtx);
void func_80850E84(Player* this, GlobalContext* globalCtx);
void func_80851008(GlobalContext* globalCtx, Player* this, void* anim);
void func_80851030(GlobalContext* globalCtx, Player* this, void* anim);
void func_80851050(GlobalContext* globalCtx, Player* this, void* anim);
void func_80851094(GlobalContext* globalCtx, Player* this, void* anim);
void func_808510B4(GlobalContext* globalCtx, Player* this, void* anim);
void func_808510D4(GlobalContext* globalCtx, Player* this, void* anim);
void func_808510F4(GlobalContext* globalCtx, Player* this, void* anim);
void func_80851114(GlobalContext* globalCtx, Player* this, void* anim);
void func_80851134(GlobalContext* globalCtx, Player* this, void* anim);
void func_80851154(GlobalContext* globalCtx, Player* this, void* anim);
void func_80851174(GlobalContext* globalCtx, Player* this, void* anim);
void func_80851194(GlobalContext* globalCtx, Player* this, void* anim);
void func_808511B4(GlobalContext* globalCtx, Player* this, void* anim);
void func_808511D4(GlobalContext* globalCtx, Player* this, void* anim);
void func_808511FC(GlobalContext* globalCtx, Player* this, void* anim);
void func_80851248(GlobalContext* globalCtx, Player* this, void* anim);
void func_80851294(GlobalContext* globalCtx, Player* this, void* anim);
void func_808512E0(GlobalContext* globalCtx, Player* this, void* arg2);
void func_80851368(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808513BC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808514C0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_8085157C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808515A4(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851688(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851750(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851788(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851828(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808518DC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_8085190C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851998(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808519C0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808519EC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851A50(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851B90(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851BE8(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851CA4(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851D2C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851D80(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851DEC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851E28(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851E64(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851E90(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851ECC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851F84(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80851FB0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852048(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852080(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852174(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808521B8(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808521F4(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852234(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_8085225C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852280(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852358(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852388(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852298(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852328(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852480(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852450(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808524B0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808524D0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852514(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852544(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852554(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852564(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808525C0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852608(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852648(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808526EC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_8085283C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808528C8(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852944(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_808529D0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852C50(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2);
void func_80852E14(Player* this, GlobalContext* globalCtx);
s32 Player_IsDroppingFish(GlobalContext* globalCtx);
s32 Player_StartFishing(GlobalContext* globalCtx);
s32 func_80852F38(GlobalContext* globalCtx, Player* this);
s32 func_80852FFC(GlobalContext* globalCtx, Actor* actor, s32 csMode);
void func_80853080(Player* this, GlobalContext* globalCtx);
s32 Player_InflictDamage(GlobalContext* globalCtx, s32 damage);
void func_80853148(GlobalContext* globalCtx, Actor* actor);

// .bss part 1
static s32 D_80858AA0;
static s32 D_80858AA4;
static Vec3f D_80858AA8;
static Input* sControlInput;

// .data

static u8 D_80853410[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static PlayerAgeProperties sAgeProperties[] = {
    {
        56.0f,
        90.0f,
        1.0f,
        111.0f,
        70.0f,
        79.4f,
        59.0f,
        41.0f,
        19.0f,
        36.0f,
        44.8f,
        56.0f,
        68.0f,
        70.0f,
        18.0f,
        15.0f,
        70.0f,
        { 9, 4671, 359 },
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        },
        {
            { 9, 6122, 359 },
            { 9, 7693, 380 },
            { 9, 6122, 359 },
            { 9, 7693, 380 },
        },
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        },
        {
            { -1592, 4694, 380 },
            { -1591, 6122, 359 },
        },
        0,
        0x80,
        &gPlayerAnim_002718,
        &gPlayerAnim_002720,
        &gPlayerAnim_002838,
        &gPlayerAnim_002E70,
        &gPlayerAnim_002E78,
        { &gPlayerAnim_002E80, &gPlayerAnim_002E88, &gPlayerAnim_002D90, &gPlayerAnim_002D98 },
        { &gPlayerAnim_002D70, &gPlayerAnim_002D78 },
        { &gPlayerAnim_002E50, &gPlayerAnim_002E58 },
        { &gPlayerAnim_002E68, &gPlayerAnim_002E60 },
    },
    {
        40.0f,
        60.0f,
        11.0f / 17.0f,
        71.0f,
        50.0f,
        47.0f,
        39.0f,
        27.0f,
        19.0f,
        22.0f,
        29.6f,
        32.0f,
        48.0f,
        70.0f * (11.0f / 17.0f),
        14.0f,
        12.0f,
        55.0f,
        { -24, 3565, 876 },
        {
            { -24, 3474, 862 },
            { -24, 4977, 937 },
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        },
        {
            { -24, 4977, 937 },
            { -24, 6495, 937 },
            { 9, 6122, 359 },
            { 9, 7693, 380 },
        },
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        },
        {
            { -1592, 4694, 380 },
            { -1591, 6122, 359 },
        },
        0x20,
        0,
        &gPlayerAnim_002318,
        &gPlayerAnim_002360,
        &gPlayerAnim_0023A8,
        &gPlayerAnim_0023E0,
        &gPlayerAnim_0023E8,
        { &gPlayerAnim_0023F0, &gPlayerAnim_0023F8, &gPlayerAnim_002D90, &gPlayerAnim_002D98 },
        { &gPlayerAnim_002D70, &gPlayerAnim_002D78 },
        { &gPlayerAnim_0023C0, &gPlayerAnim_0023C8 },
        { &gPlayerAnim_0023D8, &gPlayerAnim_0023D0 },
    },
};

static u32 D_808535D0 = false;
static f32 D_808535D4 = 0.0f;
static s16 D_808535D8 = 0;
static s16 D_808535DC = 0;
static s32 D_808535E0 = 0;
static s32 D_808535E4 = 0;
static f32 D_808535E8 = 1.0f;
static f32 D_808535EC = 1.0f;
static u32 D_808535F0 = 0;
static u32 D_808535F4 = 0;
static s16 D_808535F8 = 0;
static s16 D_808535FC = 0;
static f32 D_80853600 = 0.0f;
static s32 D_80853604 = 0;
static s32 D_80853608 = 0;
static s32 D_8085360C = 0;
static s16 D_80853610 = 0;
static s32 D_80853614 = 0;
static s32 D_80853618 = 0;

static u16 D_8085361C[] = {
    NA_SE_VO_LI_SWEAT,
    NA_SE_VO_LI_SNEEZE,
    NA_SE_VO_LI_RELAX,
    NA_SE_VO_LI_FALL_L,
};

static GetItemEntry sGetItemTable[] = {
    GET_ITEM(ITEM_BOMBS_5, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_NUTS_5, OBJECT_GI_NUTS, GID_NUTS, 0x34, 0x0C, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_BOMBCHU, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_BOW, OBJECT_GI_BOW, GID_BOW, 0x31, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_SLINGSHOT, OBJECT_GI_PACHINKO, GID_SLINGSHOT, 0x30, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BOOMERANG, OBJECT_GI_BOOMERANG, GID_BOOMERANG, 0x35, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_STICK, OBJECT_GI_STICK, GID_STICK, 0x37, 0x0D, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_HOOKSHOT, OBJECT_GI_HOOKSHOT, GID_HOOKSHOT, 0x36, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_LONGSHOT, OBJECT_GI_HOOKSHOT, GID_LONGSHOT, 0x4F, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_LENS, OBJECT_GI_GLASSES, GID_LENS, 0x39, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_LETTER_ZELDA, OBJECT_GI_LETTER, GID_LETTER_ZELDA, 0x69, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_OCARINA_TIME, OBJECT_GI_OCARINA, GID_OCARINA_TIME, 0x3A, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_HAMMER, OBJECT_GI_HAMMER, GID_HAMMER, 0x38, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_COJIRO, OBJECT_GI_NIWATORI, GID_COJIRO, 0x02, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BOTTLE, OBJECT_GI_BOTTLE, GID_BOTTLE, 0x42, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_POTION_RED, OBJECT_GI_LIQUID, GID_POTION_RED, 0x43, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_POTION_GREEN, OBJECT_GI_LIQUID, GID_POTION_GREEN, 0x44, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_POTION_BLUE, OBJECT_GI_LIQUID, GID_POTION_BLUE, 0x45, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_FAIRY, OBJECT_GI_BOTTLE, GID_BOTTLE, 0x46, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_MILK_BOTTLE, OBJECT_GI_MILK, GID_MILK, 0x98, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_LETTER_RUTO, OBJECT_GI_BOTTLE_LETTER, GID_LETTER_RUTO, 0x99, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BEAN, OBJECT_GI_BEAN, GID_BEAN, 0x48, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_MASK_SKULL, OBJECT_GI_SKJ_MASK, GID_MASK_SKULL, 0x10, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_MASK_SPOOKY, OBJECT_GI_REDEAD_MASK, GID_MASK_SPOOKY, 0x11, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_CHICKEN, OBJECT_GI_NIWATORI, GID_CHICKEN, 0x48, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_MASK_KEATON, OBJECT_GI_KI_TAN_MASK, GID_MASK_KEATON, 0x12, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_MASK_BUNNY, OBJECT_GI_RABIT_MASK, GID_MASK_BUNNY, 0x13, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_MASK_TRUTH, OBJECT_GI_TRUTH_MASK, GID_MASK_TRUTH, 0x17, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_POCKET_EGG, OBJECT_GI_EGG, GID_EGG, 0x01, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_POCKET_CUCCO, OBJECT_GI_NIWATORI, GID_CHICKEN, 0x48, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_ODD_MUSHROOM, OBJECT_GI_MUSHROOM, GID_ODD_MUSHROOM, 0x03, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_ODD_POTION, OBJECT_GI_POWDER, GID_ODD_POTION, 0x04, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_SAW, OBJECT_GI_SAW, GID_SAW, 0x05, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_SWORD_BROKEN, OBJECT_GI_BROKENSWORD, GID_SWORD_BROKEN, 0x08, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_PRESCRIPTION, OBJECT_GI_PRESCRIPTION, GID_PRESCRIPTION, 0x09, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_FROG, OBJECT_GI_FROG, GID_FROG, 0x0D, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_EYEDROPS, OBJECT_GI_EYE_LOTION, GID_EYEDROPS, 0x0E, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_CLAIM_CHECK, OBJECT_GI_TICKETSTONE, GID_CLAIM_CHECK, 0x0A, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_SWORD_KOKIRI, OBJECT_GI_SWORD_1, GID_SWORD_KOKIRI, 0xA4, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_SWORD_BGS, OBJECT_GI_LONGSWORD, GID_SWORD_BGS, 0x4B, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_SHIELD_DEKU, OBJECT_GI_SHIELD_1, GID_SHIELD_DEKU, 0x4C, 0xA0, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_SHIELD_HYLIAN, OBJECT_GI_SHIELD_2, GID_SHIELD_HYLIAN, 0x4D, 0xA0, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_SHIELD_MIRROR, OBJECT_GI_SHIELD_3, GID_SHIELD_MIRROR, 0x4E, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_TUNIC_GORON, OBJECT_GI_CLOTHES, GID_TUNIC_GORON, 0x50, 0xA0, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_TUNIC_ZORA, OBJECT_GI_CLOTHES, GID_TUNIC_ZORA, 0x51, 0xA0, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BOOTS_IRON, OBJECT_GI_BOOTS_2, GID_BOOTS_IRON, 0x53, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BOOTS_HOVER, OBJECT_GI_HOVERBOOTS, GID_BOOTS_HOVER, 0x54, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_QUIVER_40, OBJECT_GI_ARROWCASE, GID_QUIVER_40, 0x56, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_QUIVER_50, OBJECT_GI_ARROWCASE, GID_QUIVER_50, 0x57, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BOMB_BAG_20, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_20, 0x58, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BOMB_BAG_30, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_30, 0x59, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BOMB_BAG_40, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_40, 0x5A, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_GAUNTLETS_SILVER, OBJECT_GI_GLOVES, GID_GAUNTLETS_SILVER, 0x5B, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_GAUNTLETS_GOLD, OBJECT_GI_GLOVES, GID_GAUNTLETS_GOLD, 0x5C, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_SCALE_SILVER, OBJECT_GI_SCALE, GID_SCALE_SILVER, 0xCD, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_SCALE_GOLDEN, OBJECT_GI_SCALE, GID_SCALE_GOLDEN, 0xCE, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_STONE_OF_AGONY, OBJECT_GI_MAP, GID_STONE_OF_AGONY, 0x68, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_GERUDO_CARD, OBJECT_GI_GERUDO, GID_GERUDO_CARD, 0x7B, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_OCARINA_FAIRY, OBJECT_GI_OCARINA_0, GID_OCARINA_FAIRY, 0x3A, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_SEEDS, OBJECT_GI_SEED, GID_SEEDS, 0xDC, 0x50, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_HEART_CONTAINER, OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xC6, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_HEART_PIECE_2, OBJECT_GI_HEARTS, GID_HEART_PIECE, 0xC2, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_KEY_BOSS, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, 0xC7, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, 0x67, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_DUNGEON_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, 0x66, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_KEY_SMALL, OBJECT_GI_KEY, GID_KEY_SMALL, 0x60, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_MAGIC_SMALL, OBJECT_GI_MAGICPOT, GID_MAGIC_SMALL, 0x52, 0x6F, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_MAGIC_LARGE, OBJECT_GI_MAGICPOT, GID_MAGIC_LARGE, 0x52, 0x6E, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_WALLET_ADULT, OBJECT_GI_PURSE, GID_WALLET_ADULT, 0x5E, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_WALLET_GIANT, OBJECT_GI_PURSE, GID_WALLET_GIANT, 0x5F, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_WEIRD_EGG, OBJECT_GI_EGG, GID_EGG, 0x9A, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_HEART, OBJECT_GI_HEART, GID_HEART, 0x55, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_ARROWS_SMALL, OBJECT_GI_ARROW, GID_ARROWS_SMALL, 0xE6, 0x48, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_ARROWS_MEDIUM, OBJECT_GI_ARROW, GID_ARROWS_MEDIUM, 0xE6, 0x49, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_ARROWS_LARGE, OBJECT_GI_ARROW, GID_ARROWS_LARGE, 0xE6, 0x4A, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_RUPEE_GREEN, OBJECT_GI_RUPY, GID_RUPEE_GREEN, 0x6F, 0x00, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_RUPEE_BLUE, OBJECT_GI_RUPY, GID_RUPEE_BLUE, 0xCC, 0x01, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_RUPEE_RED, OBJECT_GI_RUPY, GID_RUPEE_RED, 0xF0, 0x02, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_HEART_CONTAINER, OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xC6, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_MILK, OBJECT_GI_MILK, GID_MILK, 0x98, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_MASK_GORON, OBJECT_GI_GOLONMASK, GID_MASK_GORON, 0x14, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_MASK_ZORA, OBJECT_GI_ZORAMASK, GID_MASK_ZORA, 0x15, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_MASK_GERUDO, OBJECT_GI_GERUDOMASK, GID_MASK_GERUDO, 0x16, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BRACELET, OBJECT_GI_BRACELET, GID_BRACELET, 0x79, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_RUPEE_PURPLE, OBJECT_GI_RUPY, GID_RUPEE_PURPLE, 0xF1, 0x14, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_RUPEE_GOLD, OBJECT_GI_RUPY, GID_RUPEE_GOLD, 0xF2, 0x13, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_SWORD_BGS, OBJECT_GI_LONGSWORD, GID_SWORD_BGS, 0x0C, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_ARROW_FIRE, OBJECT_GI_M_ARROW, GID_ARROW_FIRE, 0x70, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_ARROW_ICE, OBJECT_GI_M_ARROW, GID_ARROW_ICE, 0x71, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_ARROW_LIGHT, OBJECT_GI_M_ARROW, GID_ARROW_LIGHT, 0x72, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_SKULL_TOKEN, OBJECT_GI_SUTARU, GID_SKULL_TOKEN, 0xB4, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_DINS_FIRE, OBJECT_GI_GODDESS, GID_DINS_FIRE, 0xAD, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_FARORES_WIND, OBJECT_GI_GODDESS, GID_FARORES_WIND, 0xAE, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_NAYRUS_LOVE, OBJECT_GI_GODDESS, GID_NAYRUS_LOVE, 0xAF, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BULLET_BAG_30, OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG, 0x07, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BULLET_BAG_40, OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG, 0x07, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_STICKS_5, OBJECT_GI_STICK, GID_STICK, 0x37, 0x0D, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_STICKS_10, OBJECT_GI_STICK, GID_STICK, 0x37, 0x0D, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_NUTS_5, OBJECT_GI_NUTS, GID_NUTS, 0x34, 0x0C, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_NUTS_10, OBJECT_GI_NUTS, GID_NUTS, 0x34, 0x0C, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_BOMB, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_BOMBS_10, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_BOMBS_20, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_BOMBS_30, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_SEEDS_30, OBJECT_GI_SEED, GID_SEEDS, 0xDC, 0x50, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_BOMBCHUS_5, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_BOMBCHUS_20, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_FISH, OBJECT_GI_FISH, GID_FISH, 0x47, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BUG, OBJECT_GI_INSECT, GID_BUG, 0x7A, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BLUE_FIRE, OBJECT_GI_FIRE, GID_BLUE_FIRE, 0x5D, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_POE, OBJECT_GI_GHOST, GID_POE, 0x97, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_BIG_POE, OBJECT_GI_GHOST, GID_BIG_POE, 0xF9, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_KEY_SMALL, OBJECT_GI_KEY, GID_KEY_SMALL, 0xF3, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_RUPEE_GREEN, OBJECT_GI_RUPY, GID_RUPEE_GREEN, 0xF4, 0x00, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_RUPEE_BLUE, OBJECT_GI_RUPY, GID_RUPEE_BLUE, 0xF5, 0x01, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_RUPEE_RED, OBJECT_GI_RUPY, GID_RUPEE_RED, 0xF6, 0x02, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_RUPEE_PURPLE, OBJECT_GI_RUPY, GID_RUPEE_PURPLE, 0xF7, 0x14, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_HEART_PIECE_2, OBJECT_GI_HEARTS, GID_HEART_PIECE, 0xFA, 0x80, CHEST_ANIM_LONG),
    GET_ITEM(ITEM_STICK_UPGRADE_20, OBJECT_GI_STICK, GID_STICK, 0x90, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_STICK_UPGRADE_30, OBJECT_GI_STICK, GID_STICK, 0x91, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_NUT_UPGRADE_30, OBJECT_GI_NUTS, GID_NUTS, 0xA7, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_NUT_UPGRADE_40, OBJECT_GI_NUTS, GID_NUTS, 0xA8, 0x80, CHEST_ANIM_SHORT),
    GET_ITEM(ITEM_BULLET_BAG_50, OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG_50, 0x6C, 0x80, CHEST_ANIM_LONG),
    GET_ITEM_NONE,
    GET_ITEM_NONE,
};

static LinkAnimationHeader* D_80853914[PLAYER_ANIMGROUP_MAX][PLAYER_ANIMTYPE_MAX] = {
    /* PLAYER_ANIMGROUP_0 */
    { &gPlayerAnim_003240, &gPlayerAnim_003238, &gPlayerAnim_003238, &gPlayerAnim_002BE0, &gPlayerAnim_003240,
      &gPlayerAnim_003240 },
      /* PLAYER_ANIMGROUP_1 */
      { &gPlayerAnim_003290, &gPlayerAnim_003268, &gPlayerAnim_003268, &gPlayerAnim_002BF8, &gPlayerAnim_003290,
        &gPlayerAnim_003290 },
        /* PLAYER_ANIMGROUP_2 */
        { &gPlayerAnim_003140, &gPlayerAnim_002B38, &gPlayerAnim_003138, &gPlayerAnim_002B40, &gPlayerAnim_003140,
          &gPlayerAnim_003140 },
          /* PLAYER_ANIMGROUP_3 */
          { &gPlayerAnim_002E98, &gPlayerAnim_0029E8, &gPlayerAnim_002E98, &gPlayerAnim_0029F0, &gPlayerAnim_002E98,
            &gPlayerAnim_002E98 },
            /* PLAYER_ANIMGROUP_4 */
            { &gPlayerAnim_002FB0, &gPlayerAnim_002FA8, &gPlayerAnim_002FB0, &gPlayerAnim_002A40, &gPlayerAnim_002FB0,
              &gPlayerAnim_002FB0 },
              /* PLAYER_ANIMGROUP_5 */
              { &gPlayerAnim_003220, &gPlayerAnim_002590, &gPlayerAnim_002590, &gPlayerAnim_002BC0, &gPlayerAnim_003220,
                &gPlayerAnim_003220 },
                /* PLAYER_ANIMGROUP_6 */
                { &gPlayerAnim_003230, &gPlayerAnim_0025D0, &gPlayerAnim_0025D0, &gPlayerAnim_002BD0, &gPlayerAnim_003230,
                  &gPlayerAnim_003230 },
                  /* PLAYER_ANIMGROUP_7 */
                  { &gPlayerAnim_002BB0, &gPlayerAnim_0031F8, &gPlayerAnim_0031F8, &gPlayerAnim_002BB0, &gPlayerAnim_002BB0,
                    &gPlayerAnim_002BB0 },
                    /* PLAYER_ANIMGROUP_8 */
                    { &gPlayerAnim_003088, &gPlayerAnim_002A70, &gPlayerAnim_002A70, &gPlayerAnim_003088, &gPlayerAnim_003088,
                      &gPlayerAnim_003088 },
                      /* PLAYER_ANIMGROUP_9 */
                      { &gPlayerAnim_002750, &gPlayerAnim_002748, &gPlayerAnim_002748, &gPlayerAnim_002750, &gPlayerAnim_002750,
                        &gPlayerAnim_002750 },
                        /* PLAYER_ANIMGROUP_10 */
                        { &gPlayerAnim_002330, &gPlayerAnim_002330, &gPlayerAnim_002330, &gPlayerAnim_002330, &gPlayerAnim_002330,
                          &gPlayerAnim_002330 },
                          /* PLAYER_ANIMGROUP_11 */
                          { &gPlayerAnim_002760, &gPlayerAnim_002758, &gPlayerAnim_002758, &gPlayerAnim_002760, &gPlayerAnim_002760,
                            &gPlayerAnim_002760 },
                            /* PLAYER_ANIMGROUP_12 */
                            { &gPlayerAnim_002338, &gPlayerAnim_002338, &gPlayerAnim_002338, &gPlayerAnim_002338, &gPlayerAnim_002338,
                              &gPlayerAnim_002338 },
                              /* PLAYER_ANIMGROUP_13 */
                              { &gPlayerAnim_002E08, &gPlayerAnim_002E00, &gPlayerAnim_002E00, &gPlayerAnim_002E08, &gPlayerAnim_002E08,
                                &gPlayerAnim_002E08 },
                                /* PLAYER_ANIMGROUP_14 */
                                { &gPlayerAnim_003028, &gPlayerAnim_003020, &gPlayerAnim_003020, &gPlayerAnim_003028, &gPlayerAnim_003028,
                                  &gPlayerAnim_003028 },
                                  /* PLAYER_ANIMGROUP_15 */
                                  { &gPlayerAnim_003170, &gPlayerAnim_003168, &gPlayerAnim_003168, &gPlayerAnim_003170, &gPlayerAnim_003170,
                                    &gPlayerAnim_003170 },
                                    /* PLAYER_ANIMGROUP_16 */
                                    { &gPlayerAnim_003038, &gPlayerAnim_003030, &gPlayerAnim_003030, &gPlayerAnim_002A68, &gPlayerAnim_003038,
                                      &gPlayerAnim_003038 },
                                      /* PLAYER_ANIMGROUP_17 */
                                      { &gPlayerAnim_002FC0, &gPlayerAnim_002FB8, &gPlayerAnim_002FB8, &gPlayerAnim_002FC8, &gPlayerAnim_002FC0,
                                        &gPlayerAnim_002FC0 },
                                        /* PLAYER_ANIMGROUP_18 */
                                        { &gPlayerAnim_003278, &gPlayerAnim_003270, &gPlayerAnim_003270, &gPlayerAnim_002BE8, &gPlayerAnim_003278,
                                          &gPlayerAnim_003278 },
                                          /* PLAYER_ANIMGROUP_19 */
                                          { &gPlayerAnim_003288, &gPlayerAnim_003280, &gPlayerAnim_003280, &gPlayerAnim_002BF0, &gPlayerAnim_003288,
                                            &gPlayerAnim_003288 },
                                            /* PLAYER_ANIMGROUP_20 */
                                            { &gPlayerAnim_002EB8, &gPlayerAnim_002EA0, &gPlayerAnim_002EA0, &gPlayerAnim_002EB8, &gPlayerAnim_0026C8,
                                              &gPlayerAnim_002EB8 },
                                              /* PLAYER_ANIMGROUP_21 */
                                              { &gPlayerAnim_002ED8, &gPlayerAnim_002ED0, &gPlayerAnim_002ED0, &gPlayerAnim_002ED8, &gPlayerAnim_0026D0,
                                                &gPlayerAnim_002ED8 },
                                                /* PLAYER_ANIMGROUP_22 */
                                                { &gPlayerAnim_002EB0, &gPlayerAnim_002EA8, &gPlayerAnim_002EA8, &gPlayerAnim_002EB0, &gPlayerAnim_002EB0,
                                                  &gPlayerAnim_002EB0 },
                                                  /* PLAYER_ANIMGROUP_23 */
                                                  { &gPlayerAnim_003190, &gPlayerAnim_003188, &gPlayerAnim_003188, &gPlayerAnim_002B68, &gPlayerAnim_003190,
                                                    &gPlayerAnim_003190 },
                                                    /* PLAYER_ANIMGROUP_24 */
                                                    { &gPlayerAnim_003178, &gPlayerAnim_002568, &gPlayerAnim_002568, &gPlayerAnim_002B58, &gPlayerAnim_003178,
                                                      &gPlayerAnim_003178 },
                                                      /* PLAYER_ANIMGROUP_25 */
                                                      { &gPlayerAnim_003180, &gPlayerAnim_002570, &gPlayerAnim_002570, &gPlayerAnim_002B60, &gPlayerAnim_003180,
                                                        &gPlayerAnim_003180 },
                                                        /* PLAYER_ANIMGROUP_26 */
                                                        { &gPlayerAnim_002D60, &gPlayerAnim_002D58, &gPlayerAnim_002D58, &gPlayerAnim_002D60, &gPlayerAnim_002D60,
                                                          &gPlayerAnim_002D60 },
                                                          /* PLAYER_ANIMGROUP_27 */
                                                          { &gPlayerAnim_002BB8, &gPlayerAnim_003218, &gPlayerAnim_003218, &gPlayerAnim_002BB8, &gPlayerAnim_002BB8,
                                                            &gPlayerAnim_002BB8 },
                                                            /* PLAYER_ANIMGROUP_28 */
                                                            { &gPlayerAnim_002BC8, &gPlayerAnim_003228, &gPlayerAnim_003228, &gPlayerAnim_002BC8, &gPlayerAnim_002BC8,
                                                              &gPlayerAnim_002BC8 },
                                                              /* PLAYER_ANIMGROUP_29 */
                                                              { &gPlayerAnim_0031C8, &gPlayerAnim_0031C0, &gPlayerAnim_0031C0, &gPlayerAnim_0031C8, &gPlayerAnim_0031C8,
                                                                &gPlayerAnim_0031C8 },
                                                                /* PLAYER_ANIMGROUP_30 */
                                                                { &gPlayerAnim_003118, &gPlayerAnim_003110, &gPlayerAnim_003110, &gPlayerAnim_003118, &gPlayerAnim_003118,
                                                                  &gPlayerAnim_003118 },
                                                                  /* PLAYER_ANIMGROUP_31 */
                                                                  { &gPlayerAnim_002DE8, &gPlayerAnim_002DE8, &gPlayerAnim_002DE8, &gPlayerAnim_002DE8, &gPlayerAnim_002DE8,
                                                                    &gPlayerAnim_002DE8 },
                                                                    /* PLAYER_ANIMGROUP_32 */
                                                                    { &gPlayerAnim_002E30, &gPlayerAnim_002E18, &gPlayerAnim_002E18, &gPlayerAnim_002E30, &gPlayerAnim_002E30,
                                                                      &gPlayerAnim_002E30 },
                                                                      /* PLAYER_ANIMGROUP_33 */
                                                                      { &gPlayerAnim_002E40, &gPlayerAnim_002E38, &gPlayerAnim_002E38, &gPlayerAnim_002E40, &gPlayerAnim_002E40,
                                                                        &gPlayerAnim_002E40 },
                                                                        /* PLAYER_ANIMGROUP_34 */
                                                                        { &gPlayerAnim_002E28, &gPlayerAnim_002E20, &gPlayerAnim_002E20, &gPlayerAnim_002E28, &gPlayerAnim_002E28,
                                                                          &gPlayerAnim_002E28 },
                                                                          /* PLAYER_ANIMGROUP_35 */
                                                                          { &gPlayerAnim_0030C8, &gPlayerAnim_0030C0, &gPlayerAnim_0030C0, &gPlayerAnim_0030C8, &gPlayerAnim_0030C8,
                                                                            &gPlayerAnim_0030C8 },
                                                                            /* PLAYER_ANIMGROUP_36 */
                                                                            { &gPlayerAnim_0030D8, &gPlayerAnim_0030D0, &gPlayerAnim_0030D0, &gPlayerAnim_0030D8, &gPlayerAnim_0030D8,
                                                                              &gPlayerAnim_0030D8 },
                                                                              /* PLAYER_ANIMGROUP_37 */
                                                                              { &gPlayerAnim_0030B8, &gPlayerAnim_0030B0, &gPlayerAnim_0030B0, &gPlayerAnim_0030B8, &gPlayerAnim_0030B8,
                                                                                &gPlayerAnim_0030B8 },
                                                                                /* PLAYER_ANIMGROUP_38 */
                                                                                { &gPlayerAnim_002F20, &gPlayerAnim_002F18, &gPlayerAnim_002F18, &gPlayerAnim_002F20, &gPlayerAnim_002F20,
                                                                                  &gPlayerAnim_002F20 },
                                                                                  /* PLAYER_ANIMGROUP_39 */
                                                                                  { &gPlayerAnim_002FF0, &gPlayerAnim_002FE8, &gPlayerAnim_002FE8, &gPlayerAnim_002FF0, &gPlayerAnim_002FF0,
                                                                                    &gPlayerAnim_002FF0 },
                                                                                    /* PLAYER_ANIMGROUP_40 */
                                                                                    { &gPlayerAnim_003010, &gPlayerAnim_003008, &gPlayerAnim_003008, &gPlayerAnim_003010, &gPlayerAnim_003010,
                                                                                      &gPlayerAnim_003010 },
                                                                                      /* PLAYER_ANIMGROUP_41 */
                                                                                      { &gPlayerAnim_003000, &gPlayerAnim_002FF8, &gPlayerAnim_002FF8, &gPlayerAnim_003000, &gPlayerAnim_003000,
                                                                                        &gPlayerAnim_003000 },
                                                                                        /* PLAYER_ANIMGROUP_42 */
                                                                                        { &gPlayerAnim_002EF0, &gPlayerAnim_002EE8, &gPlayerAnim_002EE8, &gPlayerAnim_002EF8, &gPlayerAnim_002EF0,
                                                                                          &gPlayerAnim_002EF0 },
                                                                                          /* PLAYER_ANIMGROUP_43 */
                                                                                          { &gPlayerAnim_0031E0, &gPlayerAnim_0031D8, &gPlayerAnim_0031D8, &gPlayerAnim_0031E8, &gPlayerAnim_0031E0,
                                                                                            &gPlayerAnim_0031E0 },
                                                                                            /* PLAYER_ANIMGROUP_44 */
                                                                                            { &gPlayerAnim_003468, &gPlayerAnim_003438, &gPlayerAnim_003438, &gPlayerAnim_003468, &gPlayerAnim_003468,
                                                                                              &gPlayerAnim_003468 },
};

static LinkAnimationHeader* D_80853D4C[][3] = {
    { &gPlayerAnim_002A28, &gPlayerAnim_002A38, &gPlayerAnim_002A30 },
    { &gPlayerAnim_002950, &gPlayerAnim_002960, &gPlayerAnim_002958 },
    { &gPlayerAnim_0029D0, &gPlayerAnim_0029E0, &gPlayerAnim_0029D8 },
    { &gPlayerAnim_002988, &gPlayerAnim_002998, &gPlayerAnim_002990 },
};

static LinkAnimationHeader* D_80853D7C[][2] = {
    { &gPlayerAnim_003248, &gPlayerAnim_003200 }, { &gPlayerAnim_003258, &gPlayerAnim_003210 },
    { &gPlayerAnim_003250, &gPlayerAnim_003208 }, { &gPlayerAnim_003250, &gPlayerAnim_003208 },
    { &gPlayerAnim_003430, &gPlayerAnim_0033F0 }, { &gPlayerAnim_003430, &gPlayerAnim_0033F0 },
    { &gPlayerAnim_003430, &gPlayerAnim_0033F0 }, { &gPlayerAnim_0033F8, &gPlayerAnim_0033D0 },
    { &gPlayerAnim_003400, &gPlayerAnim_0033D8 }, { &gPlayerAnim_003420, &gPlayerAnim_003420 },
    { &gPlayerAnim_003408, &gPlayerAnim_0033E0 }, { &gPlayerAnim_003410, &gPlayerAnim_0033E8 },
    { &gPlayerAnim_003418, &gPlayerAnim_003418 }, { &gPlayerAnim_003428, &gPlayerAnim_003428 }
};

static struct_80832924 D_80853DEC[] = {
    { NA_SE_VO_LI_SNEEZE, -0x2008 },
};

static struct_80832924 D_80853DF0[] = {
    { NA_SE_VO_LI_SWEAT, -0x2012 },
};

static struct_80832924 D_80853DF4[] = {
    { NA_SE_VO_LI_BREATH_REST, -0x200D },
};

static struct_80832924 D_80853DF8[] = {
    { NA_SE_VO_LI_BREATH_REST, -0x200A },
};

static struct_80832924 D_80853DFC[] = {
    { NA_SE_PL_CALM_HIT, 0x82C }, { NA_SE_PL_CALM_HIT, 0x830 },  { NA_SE_PL_CALM_HIT, 0x834 },
    { NA_SE_PL_CALM_HIT, 0x838 }, { NA_SE_PL_CALM_HIT, -0x83C },
};

static struct_80832924 D_80853E10[] = {
    { 0, 0x4019 }, { 0, 0x401E }, { 0, 0x402C }, { 0, 0x4030 }, { 0, 0x4034 }, { 0, -0x4038 },
};

static struct_80832924 D_80853E28[] = {
    { NA_SE_IT_SHIELD_POSTURE, 0x810 },
    { NA_SE_IT_SHIELD_POSTURE, 0x814 },
    { NA_SE_IT_SHIELD_POSTURE, -0x846 },
};

static struct_80832924 D_80853E34[] = {
    { NA_SE_IT_HAMMER_SWING, 0x80A },
    { NA_SE_VO_LI_AUTO_JUMP, 0x200A },
    { NA_SE_IT_SWORD_SWING, 0x816 },
    { NA_SE_VO_LI_SWORD_N, -0x2016 },
};

static struct_80832924 D_80853E44[] = {
    { NA_SE_IT_SWORD_SWING, 0x827 },
    { NA_SE_VO_LI_SWORD_N, -0x2027 },
};

static struct_80832924 D_80853E4C[] = {
    { NA_SE_VO_LI_RELAX, -0x2014 },
};

static struct_80832924* D_80853E50[] = {
    D_80853DEC, D_80853DF0, D_80853DF4, D_80853DF8, D_80853DFC, D_80853E10,
    D_80853E28, D_80853E34, D_80853E44, D_80853E4C, NULL,
};

static u8 D_80853E7C[] = {
    0, 0, 1, 1, 2, 2, 2, 2, 10, 10, 10, 10, 10, 10, 3, 3, 4, 4, 8, 8, 5, 5, 6, 6, 7, 7, 9, 9, 0,
};

// Used to map item IDs to action params
static s8 sItemActionParams[] = {
    PLAYER_AP_STICK,
    PLAYER_AP_NUT,
    PLAYER_AP_BOMB,
    PLAYER_AP_BOW,
    PLAYER_AP_BOW_FIRE,
    PLAYER_AP_DINS_FIRE,
    PLAYER_AP_SLINGSHOT,
    PLAYER_AP_OCARINA_FAIRY,
    PLAYER_AP_OCARINA_TIME,
    PLAYER_AP_BOMBCHU,
    PLAYER_AP_HOOKSHOT,
    PLAYER_AP_LONGSHOT,
    PLAYER_AP_BOW_ICE,
    PLAYER_AP_FARORES_WIND,
    PLAYER_AP_BOOMERANG,
    PLAYER_AP_LENS,
    PLAYER_AP_BEAN,
    PLAYER_AP_HAMMER,
    PLAYER_AP_BOW_LIGHT,
    PLAYER_AP_NAYRUS_LOVE,
    PLAYER_AP_BOTTLE,
    PLAYER_AP_BOTTLE_POTION_RED,
    PLAYER_AP_BOTTLE_POTION_GREEN,
    PLAYER_AP_BOTTLE_POTION_BLUE,
    PLAYER_AP_BOTTLE_FAIRY,
    PLAYER_AP_BOTTLE_FISH,
    PLAYER_AP_BOTTLE_MILK,
    PLAYER_AP_BOTTLE_LETTER,
    PLAYER_AP_BOTTLE_FIRE,
    PLAYER_AP_BOTTLE_BUG,
    PLAYER_AP_BOTTLE_BIG_POE,
    PLAYER_AP_BOTTLE_MILK_HALF,
    PLAYER_AP_BOTTLE_POE,
    PLAYER_AP_WEIRD_EGG,
    PLAYER_AP_CHICKEN,
    PLAYER_AP_LETTER_ZELDA,
    PLAYER_AP_MASK_KEATON,
    PLAYER_AP_MASK_SKULL,
    PLAYER_AP_MASK_SPOOKY,
    PLAYER_AP_MASK_BUNNY,
    PLAYER_AP_MASK_GORON,
    PLAYER_AP_MASK_ZORA,
    PLAYER_AP_MASK_GERUDO,
    PLAYER_AP_MASK_TRUTH,
    PLAYER_AP_SWORD_MASTER,
    PLAYER_AP_POCKET_EGG,
    PLAYER_AP_POCKET_CUCCO,
    PLAYER_AP_COJIRO,
    PLAYER_AP_ODD_MUSHROOM,
    PLAYER_AP_ODD_POTION,
    PLAYER_AP_SAW,
    PLAYER_AP_SWORD_BROKEN,
    PLAYER_AP_PRESCRIPTION,
    PLAYER_AP_FROG,
    PLAYER_AP_EYEDROPS,
    PLAYER_AP_CLAIM_CHECK,
    PLAYER_AP_BOW_FIRE,
    PLAYER_AP_BOW_ICE,
    PLAYER_AP_BOW_LIGHT,
    PLAYER_AP_SWORD_KOKIRI,
    PLAYER_AP_SWORD_MASTER,
    PLAYER_AP_SWORD_BGS,
};

static s32(*D_80853EDC[])(Player* this, GlobalContext* globalCtx) = {
    func_8083485C, func_8083485C, func_8083485C, func_808349DC, func_808349DC, func_808349DC, func_8083485C,
    func_8083485C, func_8083501C, func_8083501C, func_8083501C, func_8083501C, func_8083501C, func_8083501C,
    func_8083501C, func_8083501C, func_8083501C, func_8083501C, func_808356E8, func_808356E8, func_80835800,
    func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C,
    func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C,
    func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C,
    func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C,
    func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C,
    func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C, func_8083485C,
    func_8083485C, func_8083485C, func_8083485C, func_8083485C,
};

static void (*D_80853FE8[])(GlobalContext* globalCtx, Player* this) = {
    func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_8083377C,
    func_80833790, func_8083379C, func_8083379C, func_8083379C, func_8083379C, func_8083379C, func_8083379C,
    func_8083379C, func_8083379C, func_80833910, func_80833910, func_808337D4, func_808337D4, func_80833984,
    func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770,
    func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770,
    func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770,
    func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770,
    func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770,
    func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770, func_80833770,
    func_80833770, func_80833770, func_80833770, func_80833770,
};

typedef enum {
    /*  0 */ PLAYER_D_808540F4_0,
    /*  1 */ PLAYER_D_808540F4_1,
    /*  2 */ PLAYER_D_808540F4_2,
    /*  3 */ PLAYER_D_808540F4_3,
    /*  4 */ PLAYER_D_808540F4_4,
    /*  5 */ PLAYER_D_808540F4_5,
    /*  6 */ PLAYER_D_808540F4_6,
    /*  7 */ PLAYER_D_808540F4_7,
    /*  8 */ PLAYER_D_808540F4_8,
    /*  9 */ PLAYER_D_808540F4_9,
    /* 10 */ PLAYER_D_808540F4_10,
    /* 11 */ PLAYER_D_808540F4_11,
    /* 12 */ PLAYER_D_808540F4_12,
    /* 13 */ PLAYER_D_808540F4_13,
    /* 14 */ PLAYER_D_808540F4_MAX
} PlayerD_808540F4Index;

static struct_808540F4 D_808540F4[PLAYER_D_808540F4_MAX] = {
    /* PLAYER_D_808540F4_0 */ { &gPlayerAnim_002F50, 12 },
    /* PLAYER_D_808540F4_1 */ { &gPlayerAnim_003080, 6 },
    /* PLAYER_D_808540F4_2 */ { &gPlayerAnim_002C68, 8 },
    /* PLAYER_D_808540F4_3 */ { &gPlayerAnim_003090, 8 },
    /* PLAYER_D_808540F4_4 */ { &gPlayerAnim_002A20, 8 },
    /* PLAYER_D_808540F4_5 */ { &gPlayerAnim_002F30, 10 },
    /* PLAYER_D_808540F4_6 */ { &gPlayerAnim_002C58, 7 },
    /* PLAYER_D_808540F4_7 */ { &gPlayerAnim_002C60, 11 },
    /* PLAYER_D_808540F4_8 */ { &gPlayerAnim_002F50, 12 },
    /* PLAYER_D_808540F4_9 */ { &gPlayerAnim_003078, 4 },
    /* PLAYER_D_808540F4_10 */ { &gPlayerAnim_003058, 4 },
    /* PLAYER_D_808540F4_11 */ { &gPlayerAnim_002F38, 4 },
    /* PLAYER_D_808540F4_12 */ { &gPlayerAnim_0024E0, 5 },
    /* PLAYER_D_808540F4_13 */ { &gPlayerAnim_002F48, 13 },
};

static s8 D_80854164[PLAYER_ANIMTYPE_MAX][PLAYER_ANIMTYPE_MAX] = {
    { PLAYER_D_808540F4_8, -PLAYER_D_808540F4_5, -PLAYER_D_808540F4_3, -PLAYER_D_808540F4_6, PLAYER_D_808540F4_8,
      PLAYER_D_808540F4_11 },
    { PLAYER_D_808540F4_5, PLAYER_D_808540F4_0, -PLAYER_D_808540F4_1, PLAYER_D_808540F4_4, PLAYER_D_808540F4_5,
      PLAYER_D_808540F4_9 },
    { PLAYER_D_808540F4_3, PLAYER_D_808540F4_1, PLAYER_D_808540F4_0, PLAYER_D_808540F4_2, PLAYER_D_808540F4_3,
      PLAYER_D_808540F4_9 },
    { PLAYER_D_808540F4_6, -PLAYER_D_808540F4_4, -PLAYER_D_808540F4_2, PLAYER_D_808540F4_7, PLAYER_D_808540F4_6,
      PLAYER_D_808540F4_10 },
    { PLAYER_D_808540F4_8, -PLAYER_D_808540F4_5, -PLAYER_D_808540F4_3, -PLAYER_D_808540F4_6, PLAYER_D_808540F4_8,
      PLAYER_D_808540F4_11 },
    { PLAYER_D_808540F4_8, -PLAYER_D_808540F4_5, -PLAYER_D_808540F4_3, -PLAYER_D_808540F4_6, PLAYER_D_808540F4_8,
      PLAYER_D_808540F4_11 },
};

static ExplosiveInfo sExplosiveInfos[] = {
    { ITEM_BOMB, ACTOR_EN_BOM },
    { ITEM_BOMBCHU, ACTOR_EN_BOM_CHU },
};

static struct_80854190 D_80854190[] = {
    { &gPlayerAnim_002A80, &gPlayerAnim_002A90, &gPlayerAnim_002A88, 1, 4 },
    { &gPlayerAnim_0028C0, &gPlayerAnim_0028C8, &gPlayerAnim_002498, 1, 4 },
    { &gPlayerAnim_002A98, &gPlayerAnim_002AA0, &gPlayerAnim_002540, 0, 5 },
    { &gPlayerAnim_0028D0, &gPlayerAnim_0028D8, &gPlayerAnim_0024A0, 1, 7 },
    { &gPlayerAnim_002968, &gPlayerAnim_002970, &gPlayerAnim_0024C0, 1, 4 },
    { &gPlayerAnim_002880, &gPlayerAnim_002888, &gPlayerAnim_002478, 0, 5 },
    { &gPlayerAnim_002978, &gPlayerAnim_002980, &gPlayerAnim_0024C8, 2, 8 },
    { &gPlayerAnim_002890, &gPlayerAnim_002898, &gPlayerAnim_002480, 3, 8 },
    { &gPlayerAnim_0029A0, &gPlayerAnim_0029A8, &gPlayerAnim_0024D0, 0, 4 },
    { &gPlayerAnim_0028A0, &gPlayerAnim_0028A8, &gPlayerAnim_002488, 0, 5 },
    { &gPlayerAnim_0029B0, &gPlayerAnim_0029B8, &gPlayerAnim_0024D8, 0, 6 },
    { &gPlayerAnim_0028B0, &gPlayerAnim_0028B8, &gPlayerAnim_002490, 1, 5 },
    { &gPlayerAnim_002AA8, &gPlayerAnim_002AB0, &gPlayerAnim_002548, 0, 3 },
    { &gPlayerAnim_0028E0, &gPlayerAnim_0028E8, &gPlayerAnim_0024A8, 0, 3 },
    { &gPlayerAnim_002AB8, &gPlayerAnim_002AC0, &gPlayerAnim_002550, 1, 9 },
    { &gPlayerAnim_0028F0, &gPlayerAnim_0028F8, &gPlayerAnim_0024B0, 1, 8 },
    { &gPlayerAnim_002A60, &gPlayerAnim_002A50, &gPlayerAnim_002A50, 1, 10 },
    { &gPlayerAnim_002900, &gPlayerAnim_002910, &gPlayerAnim_002910, 1, 11 },
    { &gPlayerAnim_002A50, &gPlayerAnim_002A58, &gPlayerAnim_002A58, 1, 2 },
    { &gPlayerAnim_002910, &gPlayerAnim_002908, &gPlayerAnim_002908, 1, 2 },
    { &gPlayerAnim_002B80, &gPlayerAnim_002B88, &gPlayerAnim_002B88, 1, 5 },
    { &gPlayerAnim_002B70, &gPlayerAnim_002B78, &gPlayerAnim_002B78, 1, 4 },
    { &gPlayerAnim_002C40, &gPlayerAnim_002C50, &gPlayerAnim_002C48, 3, 10 },
    { &gPlayerAnim_002C70, &gPlayerAnim_002C80, &gPlayerAnim_002C78, 2, 11 },
    { &gPlayerAnim_002B28, &gPlayerAnim_002B30, &gPlayerAnim_002560, 0, 12 },
    { &gPlayerAnim_002940, &gPlayerAnim_002948, &gPlayerAnim_0024B8, 0, 15 },
    { &gPlayerAnim_0029C0, &gPlayerAnim_0029C8, &gPlayerAnim_002560, 0, 16 },
    { &gPlayerAnim_0029C0, &gPlayerAnim_0029C8, &gPlayerAnim_0024B8, 0, 16 },
};

static LinkAnimationHeader* D_80854350[] = {
    &gPlayerAnim_002AE8,
    &gPlayerAnim_002920,
};

static LinkAnimationHeader* D_80854358[] = {
    &gPlayerAnim_002AE0,
    &gPlayerAnim_002920,
};

static LinkAnimationHeader* D_80854360[] = {
    &gPlayerAnim_002AF0,
    &gPlayerAnim_002928,
};

static LinkAnimationHeader* D_80854368[] = {
    &gPlayerAnim_002AF8,
    &gPlayerAnim_002930,
};

static LinkAnimationHeader* D_80854370[] = {
    &gPlayerAnim_002B00,
    &gPlayerAnim_002938,
};

static LinkAnimationHeader* D_80854378[] = {
    &gPlayerAnim_002AD8,
    &gPlayerAnim_002918,
};

static u8 D_80854380[2] = { 0x18, 0x19 };
static u8 D_80854384[2] = { 0x1A, 0x1B };

static u16 D_80854388[] = { BTN_B, BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT };

static u8 sMagicSpellCosts[] = { 12, 24, 24, 12, 24, 12 };

static u16 D_80854398[] = { NA_SE_IT_BOW_DRAW, NA_SE_IT_SLING_DRAW, NA_SE_IT_HOOKSHOT_READY };

static u8 sMagicArrowCosts[] = { 4, 4, 8 };

static LinkAnimationHeader* D_808543A4[] = {
    &gPlayerAnim_0025C0,
    &gPlayerAnim_0025C8,
};

static LinkAnimationHeader* D_808543AC[] = {
    &gPlayerAnim_002580,
    &gPlayerAnim_002588,
};

static LinkAnimationHeader* D_808543B4[] = {
    &gPlayerAnim_002510,
    &gPlayerAnim_002518,
};

static LinkAnimationHeader* D_808543BC[] = {
    &gPlayerAnim_002510,
    &gPlayerAnim_002520,
};

static LinkAnimationHeader* D_808543C4[] = {
    &gPlayerAnim_002EC0,
    &gPlayerAnim_002A08,
};

static LinkAnimationHeader* D_808543CC[] = {
    &gPlayerAnim_0026F0,
    &gPlayerAnim_002CC8,
};

static LinkAnimationHeader* D_808543D4[] = {
    &gPlayerAnim_0026C0,
    &gPlayerAnim_002CC0,
};

// return type can't be void due to regalloc in func_8084FCAC
s32 func_80832210(Player* this) {
    this->actor.speedXZ = 0.0f;
    this->linearVelocity = 0.0f;
}

// return type can't be void due to regalloc in func_8083F72C
s32 func_80832224(Player* this) {
    func_80832210(this);
    this->unk_6AD = 0;
}

s32 func_8083224C(GlobalContext* globalCtx) {
    Player* this = GET_PLAYER(globalCtx);

    return CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_8);
}

void func_80832264(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayOnce(globalCtx, &this->skelAnime, anim);
}

void func_80832284(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayLoop(globalCtx, &this->skelAnime, anim);
}

void func_808322A4(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayLoopSetSpeed(globalCtx, &this->skelAnime, anim, 2.0f / 3.0f);
}

void func_808322D0(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, anim, 2.0f / 3.0f);
}

void func_808322FC(Player* this) {
    this->actor.shape.rot.y += this->skelAnime.jointTable[1].y;
    this->skelAnime.jointTable[1].y = 0;
}

void func_80832318(Player* this) {
    this->stateFlags2 &= ~PLAYER_STATE2_17;
    this->swordState = 0;
    this->swordInfo[0].active = this->swordInfo[1].active = this->swordInfo[2].active = 0;
}

void func_80832340(GlobalContext* globalCtx, Player* this) {
    Camera* camera;

    if (this->unk_46C != SUBCAM_NONE) {
        camera = globalCtx->cameraPtrs[this->unk_46C];
        if ((camera != NULL) && (camera->csId == 1100)) {
            OnePointCutscene_EndCutscene(globalCtx, this->unk_46C);
            this->unk_46C = SUBCAM_NONE;
        }
    }

    this->stateFlags2 &= ~(PLAYER_STATE2_10 | PLAYER_STATE2_11);
}

void func_808323B4(GlobalContext* globalCtx, Player* this) {
    Actor* heldActor = this->heldActor;

    if ((heldActor != NULL) && !Player_HoldsHookshot(this)) {
        this->actor.child = NULL;
        this->heldActor = NULL;
        this->interactRangeActor = NULL;
        heldActor->parent = NULL;
        this->stateFlags1 &= ~PLAYER_STATE1_11;
    }

    if (Player_GetExplosiveHeld(this) >= 0) {
        func_8083399C(globalCtx, this, PLAYER_AP_NONE);
        this->heldItemId = ITEM_NONE_FE;
    }
}

void func_80832440(GlobalContext* globalCtx, Player* this) {
    if ((this->stateFlags1 & PLAYER_STATE1_11) && (this->heldActor == NULL)) {
        if (this->interactRangeActor != NULL) {
            if (this->getItemId == GI_NONE) {
                this->stateFlags1 &= ~PLAYER_STATE1_11;
                this->interactRangeActor = NULL;
            }
        }
        else {
            this->stateFlags1 &= ~PLAYER_STATE1_11;
        }
    }

    func_80832318(this);
    this->unk_6AD = 0;

    func_80832340(globalCtx, this);
    func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));

    this->stateFlags1 &= ~(PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_20 | PLAYER_STATE1_21);
    this->stateFlags2 &= ~(PLAYER_STATE2_4 | PLAYER_STATE2_7 | PLAYER_STATE2_18);

    this->actor.shape.rot.x = 0;
    this->actor.shape.yOffset = 0.0f;

    this->unk_845 = this->unk_844 = 0;
}

s32 func_80832528(GlobalContext* globalCtx, Player* this) {
    if (this->heldItemActionParam >= PLAYER_AP_FISHING_POLE) {
        func_80835F44(globalCtx, this, ITEM_NONE);
        return 1;
    }
    else {
        return 0;
    }
}

void func_80832564(GlobalContext* globalCtx, Player* this) {
    func_80832440(globalCtx, this);
    func_808323B4(globalCtx, this);
}

s32 func_80832594(Player* this, s32 arg1, s32 arg2) {
    s16 temp = this->unk_A80 - D_808535D8;

    this->unk_850 += arg1 + (s16)(ABS(temp) * fabsf(D_808535D4) * 2.5415802156203426e-06f);

    if (CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B)) {
        this->unk_850 += 5;
    }

    return this->unk_850 > arg2;
}

void func_80832630(GlobalContext* globalCtx) {
    if (globalCtx->actorCtx.freezeFlashTimer == 0) {
        globalCtx->actorCtx.freezeFlashTimer = 1;
    }
}

void func_8083264C(Player* this, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    if (this->actor.category == ACTORCAT_PLAYER) {
        func_800AA000(arg4, arg1, arg2, arg3);
    }
}

void func_80832698(Player* this, u16 sfxId) {
    if (this->actor.category == ACTORCAT_PLAYER) {
        func_8002F7DC(&this->actor, sfxId + this->ageProperties->unk_92);
    }
    else {
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

u16 func_8083275C(Player* this, u16 sfxId) {
    return sfxId + this->unk_89E;
}

void func_80832770(Player* this, u16 sfxId) {
    func_8002F7DC(&this->actor, func_8083275C(this, sfxId));
}

u16 func_808327A4(Player* this, u16 sfxId) {
    return sfxId + this->unk_89E + this->ageProperties->unk_94;
}

void func_808327C4(Player* this, u16 sfxId) {
    func_8002F7DC(&this->actor, func_808327A4(this, sfxId));
}

void func_808327F8(Player* this, f32 arg1) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_WALK_HEAVYBOOTS;
    }
    else {
        sfxId = func_808327A4(this, NA_SE_PL_WALK_GROUND);
    }

    func_800F4010(&this->actor.projectedPos, sfxId, arg1);
}

void func_80832854(Player* this) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_JUMP_HEAVYBOOTS;
    }
    else {
        sfxId = func_808327A4(this, NA_SE_PL_JUMP);
    }

    func_8002F7DC(&this->actor, sfxId);
}

void func_808328A0(Player* this) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_LAND_HEAVYBOOTS;
    }
    else {
        sfxId = func_808327A4(this, NA_SE_PL_LAND);
    }

    func_8002F7DC(&this->actor, sfxId);
}

void func_808328EC(Player* this, u16 sfxId) {
    func_8002F7DC(&this->actor, sfxId);
    this->stateFlags2 |= PLAYER_STATE2_3;
}

void func_80832924(Player* this, struct_80832924* entry) {
    s32 data;
    s32 flags;
    u32 cont;
    s32 pad;

    do {
        data = ABS(entry->field);
        flags = data & 0x7800;
        if (LinkAnimation_OnFrame(&this->skelAnime, fabsf(data & 0x7FF))) {
            if (flags == 0x800) {
                func_8002F7DC(&this->actor, entry->sfxId);
            }
            else if (flags == 0x1000) {
                func_80832770(this, entry->sfxId);
            }
            else if (flags == 0x1800) {
                func_808327C4(this, entry->sfxId);
            }
            else if (flags == 0x2000) {
                func_80832698(this, entry->sfxId);
            }
            else if (flags == 0x2800) {
                func_808328A0(this);
            }
            else if (flags == 0x3000) {
                func_808327F8(this, 6.0f);
            }
            else if (flags == 0x3800) {
                func_80832854(this);
            }
            else if (flags == 0x4000) {
                func_808327F8(this, 0.0f);
            }
            else if (flags == 0x4800) {
                func_800F4010(&this->actor.projectedPos, this->ageProperties->unk_94 + NA_SE_PL_WALK_LADDER, 0.0f);
            }
        }
        cont = (entry->field >= 0);
        entry++;
    } while (cont);
}

void func_80832B0C(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(globalCtx, &this->skelAnime, anim, 1.0f, 0.0f, Animation_GetLastFrame(anim), ANIMMODE_ONCE,
        -6.0f);
}

void func_80832B78(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(globalCtx, &this->skelAnime, anim, 2.0f / 3.0f, 0.0f, Animation_GetLastFrame(anim),
        ANIMMODE_ONCE, -6.0f);
}

void func_80832BE8(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(globalCtx, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void func_80832C2C(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(globalCtx, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
}

void func_80832C6C(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(globalCtx, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -16.0f);
}

s32 func_80832CB0(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80832284(globalCtx, this, anim);
        return 1;
    }
    else {
        return 0;
    }
}

void func_80832CFC(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;
    this->skelAnime.prevRot = this->actor.shape.rot.y;
}

void func_80832D20(Player* this) {
    func_80832CFC(this);
    this->skelAnime.prevTransl.x *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.y *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.z *= this->ageProperties->unk_08;
}

void func_80832DB0(Player* this) {
    this->skelAnime.jointTable[1].y = 0;
}

void func_80832DBC(Player* this) {
    if (this->skelAnime.moveFlags != 0) {
        func_808322FC(this);
        this->skelAnime.jointTable[0].x = this->skelAnime.baseTransl.x;
        this->skelAnime.jointTable[0].z = this->skelAnime.baseTransl.z;
        if (this->skelAnime.moveFlags & 8) {
            if (this->skelAnime.moveFlags & 2) {
                this->skelAnime.jointTable[0].y = this->skelAnime.prevTransl.y;
            }
        }
        else {
            this->skelAnime.jointTable[0].y = this->skelAnime.baseTransl.y;
        }
        func_80832CFC(this);
        this->skelAnime.moveFlags = 0;
    }
}

void func_80832E48(Player* this, s32 flags) {
    Vec3f pos;

    this->skelAnime.moveFlags = flags;
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;
    SkelAnime_UpdateTranslation(&this->skelAnime, &pos, this->actor.shape.rot.y);

    if (flags & 1) {
        if (!LINK_IS_ADULT) {
            pos.x *= 0.64f;
            pos.z *= 0.64f;
        }
        this->actor.world.pos.x += pos.x * this->actor.scale.x;
        this->actor.world.pos.z += pos.z * this->actor.scale.z;
    }

    if (flags & 2) {
        if (!(flags & 4)) {
            pos.y *= this->ageProperties->unk_08;
        }
        this->actor.world.pos.y += pos.y * this->actor.scale.y;
    }

    func_808322FC(this);
}

void func_80832F54(GlobalContext* globalCtx, Player* this, s32 flags) {
    if (flags & 0x200) {
        func_80832D20(this);
    }
    else if ((flags & 0x100) || (this->skelAnime.moveFlags != 0)) {
        func_80832CFC(this);
    }
    else {
        this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
        this->skelAnime.prevRot = this->actor.shape.rot.y;
    }

    this->skelAnime.moveFlags = flags;
    func_80832210(this);
    AnimationContext_DisableQueue(globalCtx);
}

void func_80832FFC(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim, s32 flags, f32 playbackSpeed) {
    LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, anim, playbackSpeed);
    func_80832F54(globalCtx, this, flags);
}

void func_8083303C(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim, s32 flags) {
    func_80832FFC(globalCtx, this, anim, flags, 1.0f);
}

void func_80833064(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim, s32 flags) {
    func_80832FFC(globalCtx, this, anim, flags, 2.0f / 3.0f);
}

void func_8083308C(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    func_80833064(globalCtx, this, anim, 0x1C);
}

void func_808330AC(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim, s32 flags, f32 playbackSpeed) {
    LinkAnimation_PlayLoopSetSpeed(globalCtx, &this->skelAnime, anim, playbackSpeed);
    func_80832F54(globalCtx, this, flags);
}

void func_808330EC(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim, s32 flags) {
    func_808330AC(globalCtx, this, anim, flags, 1.0f);
}

void func_80833114(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim, s32 flags) {
    func_808330AC(globalCtx, this, anim, flags, 2.0f / 3.0f);
}

void func_8083313C(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    func_80833114(globalCtx, this, anim, 0x1C);
}

void func_8083315C(GlobalContext* globalCtx, Player* this) {
    s8 phi_v1;
    s8 phi_v0;

    this->unk_A7C = D_808535D4;
    this->unk_A80 = D_808535D8;

    func_80077D10(&D_808535D4, &D_808535D8, sControlInput);

    D_808535DC = Camera_GetInputDirYaw(GET_ACTIVE_CAM(globalCtx)) + D_808535D8;

    this->unk_846 = (this->unk_846 + 1) % 4;

    if (D_808535D4 < 55.0f) {
        phi_v0 = -1;
        phi_v1 = -1;
    }
    else {
        phi_v1 = (u16)(D_808535D8 + 0x2000) >> 9;
        phi_v0 = (u16)((s16)(D_808535DC - this->actor.shape.rot.y) + 0x2000) >> 14;
    }

    this->unk_847[this->unk_846] = phi_v1;
    this->unk_84B[this->unk_846] = phi_v0;
}

void func_8083328C(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* linkAnim) {
    LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, linkAnim, D_808535E8);
}

s32 func_808332B8(Player* this) {
    return (this->stateFlags1 & PLAYER_STATE1_27) && (this->currentBoots != PLAYER_BOOTS_IRON);
}

s32 func_808332E4(Player* this) {
    return (this->stateFlags1 & PLAYER_STATE1_24);
}

void func_808332F4(Player* this, GlobalContext* globalCtx) {
    GetItemEntry* giEntry = &sGetItemTable[this->getItemId - 1];

    this->unk_862 = ABS(giEntry->gi);
}

static LinkAnimationHeader* func_80833338(Player* this) {
    return D_80853914[PLAYER_ANIMGROUP_0][this->modelAnimType];
}

s32 func_80833350(Player* this) {
    LinkAnimationHeader** entry;
    s32 i;

    if (func_80833338(this) != this->skelAnime.animation) {
        for (i = 0, entry = &D_80853D7C[0][0]; i < 28; i++, entry++) {
            if (this->skelAnime.animation == *entry) {
                return i + 1;
            }
        }
        return 0;
    }

    return -1;
}

void func_808333FC(Player* this, s32 arg1) {
    if (D_80853E7C[arg1] != 0) {
        func_80832924(this, D_80853E50[D_80853E7C[arg1] - 1]);
    }
}

LinkAnimationHeader* func_80833438(Player* this) {
    if (this->unk_890 != 0) {
        return D_80853914[PLAYER_ANIMGROUP_3][this->modelAnimType];
    }
    else if (!(this->stateFlags1 & (PLAYER_STATE1_27 | PLAYER_STATE1_29)) &&
        (this->currentBoots == PLAYER_BOOTS_IRON)) {
        return D_80853914[PLAYER_ANIMGROUP_4][this->modelAnimType];
    }
    else {
        return D_80853914[PLAYER_ANIMGROUP_2][this->modelAnimType];
    }
}

s32 func_808334B4(Player* this) {
    return func_808332E4(this) && (this->unk_834 != 0);
}

LinkAnimationHeader* func_808334E4(Player* this) {
    if (func_808334B4(this)) {
        return &gPlayerAnim_002638;
    }
    else {
        return D_80853914[PLAYER_ANIMGROUP_6][this->modelAnimType];
    }
}

LinkAnimationHeader* func_80833528(Player* this) {
    if (func_808334B4(this)) {
        return &gPlayerAnim_002630;
    }
    else {
        return D_80853914[PLAYER_ANIMGROUP_5][this->modelAnimType];
    }
}

LinkAnimationHeader* func_8083356C(Player* this) {
    if (func_8002DD78(this)) {
        return &gPlayerAnim_0026E8;
    }
    else {
        return D_80853914[PLAYER_ANIMGROUP_23][this->modelAnimType];
    }
}

LinkAnimationHeader* func_808335B0(Player* this) {
    if (func_808334B4(this)) {
        return &gPlayerAnim_002620;
    }
    else {
        return D_80853914[PLAYER_ANIMGROUP_25][this->modelAnimType];
    }
}

LinkAnimationHeader* func_808335F4(Player* this) {
    if (func_808334B4(this)) {
        return &gPlayerAnim_002618;
    }
    else {
        return D_80853914[PLAYER_ANIMGROUP_24][this->modelAnimType];
    }
}

void func_80833638(Player* this, PlayerFunc82C arg1) {
    this->func_82C = arg1;
    this->unk_836 = 0;
    this->unk_830 = 0.0f;
    func_808326F0(this);
}

void func_80833664(GlobalContext* globalCtx, Player* this, s8 actionParam) {
    LinkAnimationHeader* current = this->skelAnime.animation;
    LinkAnimationHeader** iter = &D_80853914[0][this->modelAnimType];
    u32 i;

    this->stateFlags1 &= ~(PLAYER_STATE1_3 | PLAYER_STATE1_24);

    for (i = 0; i < ARRAY_COUNT(D_80853914); i++) {
        if (current == *iter) {
            break;
        }
        iter += ARRAY_COUNT(D_80853914[0]);
    }

    func_8083399C(globalCtx, this, actionParam);

    if (i < ARRAY_COUNT(D_80853914)) {
        // fake match
        // surely D_80853914 isn't a 1D array... unless?
        this->skelAnime.animation = D_80853914[0][i * ARRAY_COUNT(D_80853914[0]) + this->modelAnimType];
    }
}

s8 Player_ItemToActionParam(s32 item) {
    if (item >= ITEM_NONE_FE) {
        return PLAYER_AP_NONE;
    }
    else if (item == ITEM_LAST_USED) {
        return PLAYER_AP_LAST_USED;
    }
    else if (item == ITEM_FISHING_POLE) {
        return PLAYER_AP_FISHING_POLE;
    }
    else {
        return sItemActionParams[item];
    }
}

void func_80833770(GlobalContext* globalCtx, Player* this) {
}

void func_8083377C(GlobalContext* globalCtx, Player* this) {
    this->unk_85C = 1.0f;
}

void func_80833790(GlobalContext* globalCtx, Player* this) {
}

void func_8083379C(GlobalContext* globalCtx, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_3;

    if (this->heldItemActionParam != PLAYER_AP_SLINGSHOT) {
        this->unk_860 = -1;
    }
    else {
        this->unk_860 = -2;
    }
}

void func_808337D4(GlobalContext* globalCtx, Player* this) {
    s32 explosiveType;
    ExplosiveInfo* explosiveInfo;
    Actor* spawnedActor;

    if (this->stateFlags1 & PLAYER_STATE1_11) {
        func_80832528(globalCtx, this);
        return;
    }

    explosiveType = Player_GetExplosiveHeld(this);
    explosiveInfo = &sExplosiveInfos[explosiveType];

    spawnedActor = Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, explosiveInfo->actorId,
        this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0,
        this->actor.shape.rot.y, 0, 0);
    if (spawnedActor != NULL) {
        if ((explosiveType != 0) && (globalCtx->bombchuBowlingStatus != 0)) {
            globalCtx->bombchuBowlingStatus--;
            if (globalCtx->bombchuBowlingStatus == 0) {
                globalCtx->bombchuBowlingStatus = -1;
            }
        }
        else {
            Inventory_ChangeAmmo(explosiveInfo->itemId, -1);
        }

        this->interactRangeActor = spawnedActor;
        this->heldActor = spawnedActor;
        this->getItemId = GI_NONE;
        this->unk_3BC.y = spawnedActor->shape.rot.y - this->actor.shape.rot.y;
        this->stateFlags1 |= PLAYER_STATE1_11;
    }
}

void func_80833910(GlobalContext* globalCtx, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_3;
    this->unk_860 = -3;

    this->heldActor =
        Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_ARMS_HOOK, this->actor.world.pos.x,
            this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0);
}

void func_80833984(GlobalContext* globalCtx, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_24;
}

void func_8083399C(GlobalContext* globalCtx, Player* this, s8 actionParam) {
    this->unk_860 = 0;
    this->unk_85C = 0.0f;
    this->unk_858 = 0.0f;

    this->heldItemActionParam = this->itemActionParam = actionParam;
    this->modelGroup = this->nextModelGroup;

    this->stateFlags1 &= ~(PLAYER_STATE1_3 | PLAYER_STATE1_24);

    D_80853FE8[actionParam](globalCtx, this);

    Player_SetModelGroup(this, this->modelGroup);
}

void func_80833A20(Player* this, s32 newSwordState) {
    u16 itemSfx;
    u16 voiceSfx;

    if (this->swordState == 0) {
        if ((this->heldItemActionParam == PLAYER_AP_SWORD_BGS) && (gSaveContext.swordHealth > 0.0f)) {
            itemSfx = NA_SE_IT_HAMMER_SWING;
        }
        else {
            itemSfx = NA_SE_IT_SWORD_SWING;
        }

        voiceSfx = NA_SE_VO_LI_SWORD_N;
        if (this->heldItemActionParam == PLAYER_AP_HAMMER) {
            itemSfx = NA_SE_IT_HAMMER_SWING;
        }
        else if (this->swordAnimation >= 0x18) {
            itemSfx = 0;
            voiceSfx = NA_SE_VO_LI_SWORD_L;
        }
        else if (this->unk_845 >= 3) {
            itemSfx = NA_SE_IT_SWORD_SWING_HARD;
            voiceSfx = NA_SE_VO_LI_SWORD_L;
        }

        if (itemSfx != 0) {
            func_808328EC(this, itemSfx);
        }

        if ((this->swordAnimation < 0x10) || (this->swordAnimation >= 0x14)) {
            func_80832698(this, voiceSfx);
        }
    }

    this->swordState = newSwordState;
}

s32 func_80833B2C(Player* this) {
    if (this->stateFlags1 & (PLAYER_STATE1_16 | PLAYER_STATE1_17 | PLAYER_STATE1_30)) {
        return 1;
    }
    else {
        return 0;
    }
}

s32 func_80833B54(Player* this) {
    if ((this->unk_664 != NULL) && CHECK_FLAG_ALL(this->unk_664->flags, ACTOR_FLAG_0 | ACTOR_FLAG_2)) {
        this->stateFlags1 |= PLAYER_STATE1_4;
        return 1;
    }

    if (this->stateFlags1 & PLAYER_STATE1_4) {
        this->stateFlags1 &= ~PLAYER_STATE1_4;
        if (this->linearVelocity == 0.0f) {
            this->currentYaw = this->actor.shape.rot.y;
        }
    }

    return 0;
}

s32 func_80833BCC(Player* this) {
    return func_8008E9C4(this) || func_80833B2C(this);
}

s32 func_80833C04(Player* this) {
    return func_80833B54(this) || func_80833B2C(this);
}

void func_80833C3C(Player* this) {
    this->unk_870 = this->unk_874 = 0.0f;
}

s32 func_80833C50(Player* this, s32 item) {
    if ((item < ITEM_NONE_FE) && (Player_ItemToActionParam(item) == this->itemActionParam)) {
        return 1;
    }
    else {
        return 0;
    }
}

s32 func_80833C98(s32 item1, s32 actionParam) {
    if ((item1 < ITEM_NONE_FE) && (Player_ItemToActionParam(item1) == actionParam)) {
        return 1;
    }
    else {
        return 0;
    }
}

s32 func_80833CDC(GlobalContext* globalCtx, s32 index) {
    if (index >= 4) {
        return ITEM_NONE;
    }
    else if (globalCtx->bombchuBowlingStatus != 0) {
        return (globalCtx->bombchuBowlingStatus > 0) ? ITEM_BOMBCHU : ITEM_NONE;
    }
    else if (index == 0) {
        return B_BTN_ITEM;
    }
    else if (index == 1) {
        return C_BTN_ITEM(0);
    }
    else if (index == 2) {
        return C_BTN_ITEM(1);
    }
    else {
        return C_BTN_ITEM(2);
    }
}

void func_80833DF8(Player* this, GlobalContext* globalCtx) {
    s32 maskActionParam;
    s32 item;
    s32 i;

    if (this->currentMask != PLAYER_MASK_NONE) {
        if (CVar_GetS32("gMMBunnyHood", 0) != 0) {
            s32 maskItem = this->currentMask - PLAYER_MASK_KEATON + ITEM_MASK_KEATON;

            if (gSaveContext.equips.buttonItems[0] != maskItem && gSaveContext.equips.buttonItems[1] != maskItem &&
                gSaveContext.equips.buttonItems[2] != maskItem && gSaveContext.equips.buttonItems[3] != maskItem) {
                this->currentMask = PLAYER_MASK_NONE;
                func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
            }
        } else {
            maskActionParam = this->currentMask - 1 + PLAYER_AP_MASK_KEATON;
            if (!func_80833C98(C_BTN_ITEM(0), maskActionParam) && !func_80833C98(C_BTN_ITEM(1), maskActionParam) &&
                !func_80833C98(C_BTN_ITEM(2), maskActionParam)) {
                this->currentMask = PLAYER_MASK_NONE;
            }
        }
    }

    if (!(this->stateFlags1 & (PLAYER_STATE1_11 | PLAYER_STATE1_29)) && !func_8008F128(this)) {
        if (this->itemActionParam >= PLAYER_AP_FISHING_POLE) {
            if (!func_80833C50(this, B_BTN_ITEM) && !func_80833C50(this, C_BTN_ITEM(0)) &&
                !func_80833C50(this, C_BTN_ITEM(1)) && !func_80833C50(this, C_BTN_ITEM(2))) {
                func_80835F44(globalCtx, this, ITEM_NONE);
                return;
            }
        }

        for (i = 0; i < ARRAY_COUNT(D_80854388); i++) {
            if (CHECK_BTN_ALL(sControlInput->press.button, D_80854388[i])) {
                break;
            }
        }

        item = func_80833CDC(globalCtx, i);
        if (item >= ITEM_NONE_FE) {
            for (i = 0; i < ARRAY_COUNT(D_80854388); i++) {
                if (CHECK_BTN_ALL(sControlInput->cur.button, D_80854388[i])) {
                    break;
                }
            }

            item = func_80833CDC(globalCtx, i);
            if ((item < ITEM_NONE_FE) && (Player_ItemToActionParam(item) == this->heldItemActionParam)) {
                D_80853618 = true;
            }
        }
        else {
            this->heldItemButton = i;
            func_80835F44(globalCtx, this, item);
        }
    }
}

void func_808340DC(Player* this, GlobalContext* globalCtx) {
    LinkAnimationHeader* anim;
    f32 frameCount;
    f32 startFrame;
    f32 endFrame;
    f32 playSpeed;
    s32 sp38;
    s8 sp37;
    s32 nextAnimType;

    sp37 = Player_ItemToActionParam(this->heldItemId);
    func_80833638(this, func_80834A2C);

    nextAnimType = gPlayerModelTypes[this->nextModelGroup][PLAYER_MODELGROUPENTRY_ANIM];
    sp38 = D_80854164[gPlayerModelTypes[this->modelGroup][PLAYER_MODELGROUPENTRY_ANIM]][nextAnimType];
    if ((sp37 == PLAYER_AP_BOTTLE) || (sp37 == PLAYER_AP_BOOMERANG) ||
        ((sp37 == PLAYER_AP_NONE) &&
            ((this->heldItemActionParam == PLAYER_AP_BOTTLE) || (this->heldItemActionParam == PLAYER_AP_BOOMERANG)))) {
        sp38 = (sp37 == PLAYER_AP_NONE) ? -PLAYER_D_808540F4_13 : PLAYER_D_808540F4_13;
    }

    this->unk_15A = ABS(sp38);

    anim = D_808540F4[this->unk_15A].anim;
    if ((anim == &gPlayerAnim_002F30) && (this->currentShield == PLAYER_SHIELD_NONE)) {
        anim = &gPlayerAnim_002F40;
    }

    frameCount = Animation_GetLastFrame(anim);
    endFrame = frameCount;

    if (sp38 >= 0) {
        playSpeed = 1.2f;
        startFrame = 0.0f;
    }
    else {
        endFrame = 0.0f;
        playSpeed = -1.2f;
        startFrame = frameCount;
    }

    if (sp37 != PLAYER_AP_NONE) {
        playSpeed *= 2.0f;
    }

    LinkAnimation_Change(globalCtx, &this->skelAnime2, anim, playSpeed, startFrame, endFrame, ANIMMODE_ONCE, 0.0f);

    this->stateFlags1 &= ~PLAYER_STATE1_8;
}

void func_80834298(Player* this, GlobalContext* globalCtx) {
    if ((this->actor.category == ACTORCAT_PLAYER) && !(this->stateFlags1 & PLAYER_STATE1_8) &&
        ((this->heldItemActionParam == this->itemActionParam) || (this->stateFlags1 & PLAYER_STATE1_22)) &&
        (gSaveContext.health != 0) && (globalCtx->csCtx.state == CS_STATE_IDLE) && (this->csMode == 0) &&
        (globalCtx->shootingGalleryStatus == 0) && (globalCtx->activeCamera == MAIN_CAM) &&
        (globalCtx->sceneLoadFlag != 0x14) && (gSaveContext.timer1State != 10)) {
        func_80833DF8(this, globalCtx);
    }

    if (this->stateFlags1 & PLAYER_STATE1_8) {
        func_808340DC(this, globalCtx);
    }
}

s32 func_80834380(GlobalContext* globalCtx, Player* this, s32* itemPtr, s32* typePtr) {
    if (LINK_IS_ADULT) {
        *itemPtr = ITEM_BOW;
        if (this->stateFlags1 & PLAYER_STATE1_23) {
            *typePtr = ARROW_NORMAL_HORSE;
        }
        else {
            *typePtr = this->heldItemActionParam - 6;
        }
    }
    else {
        *itemPtr = ITEM_SLINGSHOT;
        *typePtr = ARROW_SEED;
    }

    if (gSaveContext.minigameState == 1) {
        return globalCtx->interfaceCtx.hbaAmmo;
    }
    else if (globalCtx->shootingGalleryStatus != 0) {
        return globalCtx->shootingGalleryStatus;
    }
    else {
        return AMMO(*itemPtr);
    }
}

s32 func_8083442C(Player* this, GlobalContext* globalCtx) {
    s32 item;
    s32 arrowType;
    s32 magicArrowType;

    if ((this->heldItemActionParam >= PLAYER_AP_BOW_FIRE) && (this->heldItemActionParam <= PLAYER_AP_BOW_0E) &&
        (gSaveContext.unk_13F0 != 0)) {
        func_80078884(NA_SE_SY_ERROR);
    }
    else {
        func_80833638(this, func_808351D4);

        this->stateFlags1 |= PLAYER_STATE1_9;
        this->unk_834 = 14;

        if (this->unk_860 >= 0) {
            func_8002F7DC(&this->actor, D_80854398[ABS(this->unk_860) - 1]);

            if (!Player_HoldsHookshot(this) && (func_80834380(globalCtx, this, &item, &arrowType) > 0)) {
                magicArrowType = arrowType - ARROW_FIRE;

                if (this->unk_860 >= 0) {
                    if ((magicArrowType >= 0) && (magicArrowType <= 2) &&
                        !func_80087708(globalCtx, sMagicArrowCosts[magicArrowType], 0)) {
                        arrowType = ARROW_NORMAL;
                    }

                    this->heldActor = Actor_SpawnAsChild(
                        &globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_EN_ARROW, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, arrowType);
                }
            }
        }

        return 1;
    }

    return 0;
}

void func_80834594(GlobalContext* globalCtx, Player* this) {
    if (this->heldItemActionParam != PLAYER_AP_NONE) {
        if (func_8008F2BC(this, this->heldItemActionParam) >= 0) {
            func_808328EC(this, NA_SE_IT_SWORD_PUTAWAY);
        }
        else {
            func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
        }
    }

    func_80835F44(globalCtx, this, this->heldItemId);

    if (func_8008F2BC(this, this->heldItemActionParam) >= 0) {
        func_808328EC(this, NA_SE_IT_SWORD_PICKOUT);
    }
    else if (this->heldItemActionParam != PLAYER_AP_NONE) {
        func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
    }
}

void func_80834644(GlobalContext* globalCtx, Player* this) {
    if (func_80834A2C == this->func_82C) {
        func_80834594(globalCtx, this);
    }

    func_80833638(this, D_80853EDC[this->heldItemActionParam]);
    this->unk_834 = 0;
    this->unk_6AC = 0;
    func_808323B4(globalCtx, this);
    this->stateFlags1 &= ~PLAYER_STATE1_8;
}

LinkAnimationHeader* func_808346C4(GlobalContext* globalCtx, Player* this) {
    func_80833638(this, func_80834B5C);
    func_808323B4(globalCtx, this);

    if (this->unk_870 < 0.5f) {
        return D_808543A4[Player_HoldsTwoHandedWeapon(this)];
    }
    else {
        return D_808543AC[Player_HoldsTwoHandedWeapon(this)];
    }
}

s32 func_80834758(GlobalContext* globalCtx, Player* this) {
    LinkAnimationHeader* anim;
    f32 frame;

    if (!(this->stateFlags1 & (PLAYER_STATE1_22 | PLAYER_STATE1_23 | PLAYER_STATE1_29)) &&
        (globalCtx->shootingGalleryStatus == 0) && (this->heldItemActionParam == this->itemActionParam) &&
        (this->currentShield != PLAYER_SHIELD_NONE) && !Player_IsChildWithHylianShield(this) && func_80833BCC(this) &&
        CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {

        anim = func_808346C4(globalCtx, this);
        frame = Animation_GetLastFrame(anim);
        LinkAnimation_Change(globalCtx, &this->skelAnime2, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);
        func_8002F7DC(&this->actor, NA_SE_IT_SHIELD_POSTURE);

        return 1;
    }
    else {
        return 0;
    }
}

s32 func_8083485C(Player* this, GlobalContext* globalCtx) {
    if (func_80834758(globalCtx, this)) {
        return 1;
    }
    else {
        return 0;
    }
}

void func_80834894(Player* this) {
    func_80833638(this, func_80834C74);

    if (this->itemActionParam < 0) {
        func_8008EC70(this);
    }

    Animation_Reverse(&this->skelAnime2);
    func_8002F7DC(&this->actor, NA_SE_IT_SHIELD_REMOVE);
}

void func_808348EC(GlobalContext* globalCtx, Player* this) {
    struct_808540F4* ptr = &D_808540F4[this->unk_15A];
    f32 frame;

    frame = ptr->unk_04;
    frame = (this->skelAnime2.playSpeed < 0.0f) ? frame - 1.0f : frame;

    if (LinkAnimation_OnFrame(&this->skelAnime2, frame)) {
        func_80834594(globalCtx, this);
    }

    func_80833B54(this);
}

s32 func_8083499C(Player* this, GlobalContext* globalCtx) {
    if (this->stateFlags1 & PLAYER_STATE1_8) {
        func_808340DC(this, globalCtx);
    }
    else {
        return 0;
    }

    return 1;
}

s32 func_808349DC(Player* this, GlobalContext* globalCtx) {
    if (func_80834758(globalCtx, this) || func_8083499C(this, globalCtx)) {
        return 1;
    }
    else {
        return 0;
    }
}

s32 func_80834A2C(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime2) ||
        ((Player_ItemToActionParam(this->heldItemId) == this->heldItemActionParam) &&
            (D_80853614 = (D_80853614 ||
                ((this->modelAnimType != PLAYER_ANIMTYPE_3) && (globalCtx->shootingGalleryStatus == 0)))))) {
        func_80833638(this, D_80853EDC[this->heldItemActionParam]);
        this->unk_834 = 0;
        this->unk_6AC = 0;
        D_80853618 = D_80853614;
        return this->func_82C(this, globalCtx);
    }

    if (func_80833350(this) != 0) {
        func_808348EC(globalCtx, this);
        func_80832264(globalCtx, this, func_80833338(this));
        this->unk_6AC = 0;
    }
    else {
        func_808348EC(globalCtx, this);
    }

    return 1;
}

s32 func_80834B5C(Player* this, GlobalContext* globalCtx) {
    LinkAnimation_Update(globalCtx, &this->skelAnime2);

    if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {
        func_80834894(this);
        return 1;
    }
    else {
        this->stateFlags1 |= PLAYER_STATE1_22;
        Player_SetModelsForHoldingShield(this);
        return 1;
    }
}

s32 func_80834BD4(Player* this, GlobalContext* globalCtx) {
    LinkAnimationHeader* anim;
    f32 frame;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime2)) {
        anim = func_808346C4(globalCtx, this);
        frame = Animation_GetLastFrame(anim);
        LinkAnimation_Change(globalCtx, &this->skelAnime2, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);
    }

    this->stateFlags1 |= PLAYER_STATE1_22;
    Player_SetModelsForHoldingShield(this);

    return 1;
}

s32 func_80834C74(Player* this, GlobalContext* globalCtx) {
    D_80853614 = D_80853618;

    if (D_80853614 || LinkAnimation_Update(globalCtx, &this->skelAnime2)) {
        func_80833638(this, D_80853EDC[this->heldItemActionParam]);
        LinkAnimation_PlayLoop(globalCtx, &this->skelAnime2, D_80853914[PLAYER_ANIMGROUP_0][this->modelAnimType]);
        this->unk_6AC = 0;
        this->func_82C(this, globalCtx);
        return 0;
    }

    return 1;
}

s32 func_80834D2C(Player* this, GlobalContext* globalCtx) {
    LinkAnimationHeader* anim;

    if (this->heldItemActionParam != PLAYER_AP_BOOMERANG) {
        if (!func_8083442C(this, globalCtx)) {
            return 0;
        }

        if (!Player_HoldsHookshot(this)) {
            anim = &gPlayerAnim_0026A0;
        }
        else {
            anim = &gPlayerAnim_002CA0;
        }
        LinkAnimation_PlayOnce(globalCtx, &this->skelAnime2, anim);
    }
    else {
        func_80833638(this, func_80835884);
        this->unk_834 = 10;
        LinkAnimation_PlayOnce(globalCtx, &this->skelAnime2, &gPlayerAnim_002628);
    }

    if (this->stateFlags1 & PLAYER_STATE1_23) {
        func_80832284(globalCtx, this, &gPlayerAnim_003380);
    }
    else if ((this->actor.bgCheckFlags & 1) && !func_80833B54(this)) {
        func_80832284(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_0][this->modelAnimType]);
    }

    return 1;
}

s32 func_80834E44(GlobalContext* globalCtx) {
    return (globalCtx->shootingGalleryStatus > 0) && CHECK_BTN_ALL(sControlInput->press.button, BTN_B);
}

s32 func_80834E7C(GlobalContext* globalCtx) {
    return (globalCtx->shootingGalleryStatus != 0) &&
        ((globalCtx->shootingGalleryStatus < 0) ||
            CHECK_BTN_ANY(sControlInput->cur.button, BTN_A | BTN_B | BTN_CUP | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN));
}

s32 func_80834EB8(Player* this, GlobalContext* globalCtx) {
    if ((this->unk_6AD == 0) || (this->unk_6AD == 2)) {
        if (func_80833BCC(this) || (Camera_CheckValidMode(Gameplay_GetCamera(globalCtx, 0), 7) == 0)) {
            return 1;
        }
        this->unk_6AD = 2;
    }

    return 0;
}

s32 func_80834F2C(Player* this, GlobalContext* globalCtx) {
    if ((this->doorType == PLAYER_DOORTYPE_NONE) && !(this->stateFlags1 & PLAYER_STATE1_25)) {
        if (D_80853614 || func_80834E44(globalCtx)) {
            if (func_80834D2C(this, globalCtx)) {
                return func_80834EB8(this, globalCtx);
            }
        }
    }

    return 0;
}

s32 func_80834FBC(Player* this) {
    if (this->actor.child != NULL) {
        if (this->heldActor == NULL) {
            this->heldActor = this->actor.child;
            func_8083264C(this, 255, 10, 250, 0);
            func_8002F7DC(&this->actor, NA_SE_IT_HOOKSHOT_RECEIVE);
        }

        return 1;
    }

    return 0;
}

s32 func_8083501C(Player* this, GlobalContext* globalCtx) {
    if (this->unk_860 >= 0) {
        this->unk_860 = -this->unk_860;
    }

    if ((!Player_HoldsHookshot(this) || func_80834FBC(this)) && !func_80834758(globalCtx, this) &&
        !func_80834F2C(this, globalCtx)) {
        return 0;
    } else if (this->rideActor != NULL) {
        this->unk_6AD = 2;  // OTRTODO: THIS IS A BAD IDEA BUT IT FIXES THE HORSE FIRST PERSON?
    }

    return 1;
}

s32 func_808350A4(GlobalContext* globalCtx, Player* this) {
    s32 item;
    s32 arrowType;

    if (this->heldActor != NULL) {
        if (!Player_HoldsHookshot(this)) {
            func_80834380(globalCtx, this, &item, &arrowType);

            if (gSaveContext.minigameState == 1) {
                globalCtx->interfaceCtx.hbaAmmo--;
            }
            else if (globalCtx->shootingGalleryStatus != 0) {
                globalCtx->shootingGalleryStatus--;
            }
            else {
                Inventory_ChangeAmmo(item, -1);
            }

            if (globalCtx->shootingGalleryStatus == 1) {
                globalCtx->shootingGalleryStatus = -10;
            }

            func_8083264C(this, 150, 10, 150, 0);
        }
        else {
            func_8083264C(this, 255, 20, 150, 0);
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

s32 func_808351D4(Player* this, GlobalContext* globalCtx) {
    s32 sp2C;

    if (!Player_HoldsHookshot(this)) {
        sp2C = 0;
    }
    else {
        sp2C = 1;
    }

    Math_ScaledStepToS(&this->unk_6C0, 1200, 400);
    this->unk_6AE |= 0x100;

    if ((this->unk_836 == 0) && (func_80833350(this) == 0) && (this->skelAnime.animation == &gPlayerAnim_0026E8)) {
        LinkAnimation_PlayOnce(globalCtx, &this->skelAnime2, D_808543CC[sp2C]);
        this->unk_836 = -1;
    }
    else if (LinkAnimation_Update(globalCtx, &this->skelAnime2)) {
        LinkAnimation_PlayLoop(globalCtx, &this->skelAnime2, D_808543D4[sp2C]);
        this->unk_836 = 1;
    }
    else if (this->unk_836 == 1) {
        this->unk_836 = 2;
    }

    if (this->unk_834 > 10) {
        this->unk_834--;
    }

    func_80834EB8(this, globalCtx);

    if ((this->unk_836 > 0) && ((this->unk_860 < 0) || (!D_80853618 && !func_80834E7C(globalCtx)))) {
        func_80833638(this, func_808353D8);
        if (this->unk_860 >= 0) {
            if (sp2C == 0) {
                if (!func_808350A4(globalCtx, this)) {
                    func_8002F7DC(&this->actor, D_808543DC[ABS(this->unk_860) - 1]);
                }
            }
            else if (this->actor.bgCheckFlags & 1) {
                func_808350A4(globalCtx, this);
            }
        }
        this->unk_834 = 10;
        func_80832210(this);
    }
    else {
        this->stateFlags1 |= PLAYER_STATE1_9;
    }

    return 1;
}

s32 func_808353D8(Player* this, GlobalContext* globalCtx) {
    LinkAnimation_Update(globalCtx, &this->skelAnime2);

    if (Player_HoldsHookshot(this) && !func_80834FBC(this)) {
        return 1;
    }

    if (!func_80834758(globalCtx, this) &&
        (D_80853614 || ((this->unk_860 < 0) && D_80853618) || func_80834E44(globalCtx))) {
        this->unk_860 = ABS(this->unk_860);

        if (func_8083442C(this, globalCtx)) {
            if (Player_HoldsHookshot(this)) {
                this->unk_836 = 1;
            }
            else {
                LinkAnimation_PlayOnce(globalCtx, &this->skelAnime2, &gPlayerAnim_0026B8);
            }
        }
    }
    else {
        if (this->unk_834 != 0) {
            this->unk_834--;
        }

        if (func_80833BCC(this) || (this->unk_6AD != 0) || (this->stateFlags1 & PLAYER_STATE1_20)) {
            if (this->unk_834 == 0) {
                this->unk_834++;
            }
            return 1;
        }

        if (Player_HoldsHookshot(this)) {
            func_80833638(this, func_8083501C);
        }
        else {
            func_80833638(this, func_80835588);
            LinkAnimation_PlayOnce(globalCtx, &this->skelAnime2, &gPlayerAnim_0026B0);
        }

        this->unk_834 = 0;
    }

    return 1;
}

s32 func_80835588(Player* this, GlobalContext* globalCtx) {
    if (!(this->actor.bgCheckFlags & 1) || LinkAnimation_Update(globalCtx, &this->skelAnime2)) {
        func_80833638(this, func_8083501C);
    }

    return 1;
}

void func_808355DC(Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_17;

    if (!(this->skelAnime.moveFlags & 0x80) && (this->actor.bgCheckFlags & 0x200) && (D_80853608 < 0x2000)) {
        this->currentYaw = this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
    }

    this->targetYaw = this->actor.shape.rot.y;
}

s32 func_80835644(GlobalContext* globalCtx, Player* this, Actor* arg2) {
    if (arg2 == NULL) {
        func_80832564(globalCtx, this);
        func_80839F90(this, globalCtx);
        return 1;
    }

    return 0;
}

void func_80835688(Player* this, GlobalContext* globalCtx) {
    if (!func_80835644(globalCtx, this, this->heldActor)) {
        func_80833638(this, func_808356E8);
        LinkAnimation_PlayLoop(globalCtx, &this->skelAnime2, &gPlayerAnim_002E10);
    }
}

s32 func_808356E8(Player* this, GlobalContext* globalCtx) {
    Actor* heldActor = this->heldActor;

    if (heldActor == NULL) {
        func_80834644(globalCtx, this);
    }

    if (func_80834758(globalCtx, this)) {
        return 1;
    }

    if (this->stateFlags1 & PLAYER_STATE1_11) {
        if (LinkAnimation_Update(globalCtx, &this->skelAnime2)) {
            LinkAnimation_PlayLoop(globalCtx, &this->skelAnime2, &gPlayerAnim_002E10);
        }

        if ((heldActor->id == ACTOR_EN_NIW) && (this->actor.velocity.y <= 0.0f)) {
            this->actor.minVelocityY = -2.0f;
            this->actor.gravity = -0.5f;
            this->fallStartHeight = this->actor.world.pos.y;
        }

        return 1;
    }

    return func_8083485C(this, globalCtx);
}

void func_808357E8(Player* this, Gfx** dLists) {
    this->leftHandDLists = &dLists[gSaveContext.linkAge];
}

s32 func_80835800(Player* this, GlobalContext* globalCtx) {
    if (func_80834758(globalCtx, this)) {
        return 1;
    }

    if (this->stateFlags1 & PLAYER_STATE1_25) {
        func_80833638(this, func_80835B60);
    }
    else if (func_80834F2C(this, globalCtx)) {
        return 1;
    }

    return 0;
}

s32 func_80835884(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime2)) {
        func_80833638(this, func_808358F0);
        LinkAnimation_PlayLoop(globalCtx, &this->skelAnime2, &gPlayerAnim_002638);
    }

    func_80834EB8(this, globalCtx);

    return 1;
}

s32 func_808358F0(Player* this, GlobalContext* globalCtx) {
    LinkAnimationHeader* animSeg = this->skelAnime.animation;

    if ((func_808334E4(this) == animSeg) || (func_80833528(this) == animSeg) || (func_808335B0(this) == animSeg) ||
        (func_808335F4(this) == animSeg)) {
        AnimationContext_SetCopyAll(globalCtx, this->skelAnime.limbCount, this->skelAnime2.jointTable,
            this->skelAnime.jointTable);
    }
    else {
        LinkAnimation_Update(globalCtx, &this->skelAnime2);
    }

    func_80834EB8(this, globalCtx);

    if (!D_80853618) {
        func_80833638(this, func_808359FC);
        LinkAnimation_PlayOnce(globalCtx, &this->skelAnime2,
            (this->unk_870 < 0.5f) ? &gPlayerAnim_002608 : &gPlayerAnim_002600);
    }

    return 1;
}

s32 func_808359FC(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime2)) {
        func_80833638(this, func_80835B60);
        this->unk_834 = 0;
    }
    else if (LinkAnimation_OnFrame(&this->skelAnime2, 6.0f)) {
        f32 posX = (Math_SinS(this->actor.shape.rot.y) * 10.0f) + this->actor.world.pos.x;
        f32 posZ = (Math_CosS(this->actor.shape.rot.y) * 10.0f) + this->actor.world.pos.z;
        s32 yaw = (this->unk_664 != NULL) ? this->actor.shape.rot.y + 14000 : this->actor.shape.rot.y;
        EnBoom* boomerang =
            (EnBoom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOOM, posX, this->actor.world.pos.y + 30.0f,
                posZ, this->actor.focus.rot.x, yaw, 0, 0);

        this->boomerangActor = &boomerang->actor;
        if (boomerang != NULL) {
            boomerang->moveTo = this->unk_664;
            boomerang->returnTimer = 20;
            this->stateFlags1 |= PLAYER_STATE1_25;
            if (!func_8008E9C4(this)) {
                func_808355DC(this);
            }
            this->unk_A73 = 4;
            func_8002F7DC(&this->actor, NA_SE_IT_BOOMERANG_THROW);
            func_80832698(this, NA_SE_VO_LI_SWORD_N);
        }
    }

    return 1;
}

s32 func_80835B60(Player* this, GlobalContext* globalCtx) {
    if (func_80834758(globalCtx, this)) {
        return 1;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_25)) {
        func_80833638(this, func_80835C08);
        LinkAnimation_PlayOnce(globalCtx, &this->skelAnime2, &gPlayerAnim_0025F8);
        func_808357E8(this, D_80125EF8);
        func_8002F7DC(&this->actor, NA_SE_PL_CATCH_BOOMERANG);
        func_80832698(this, NA_SE_VO_LI_SWORD_N);
        return 1;
    }

    return 0;
}

s32 func_80835C08(Player* this, GlobalContext* globalCtx) {
    if (!func_80835800(this, globalCtx) && LinkAnimation_Update(globalCtx, &this->skelAnime2)) {
        func_80833638(this, func_80835800);
    }

    return 1;
}

s32 func_80835C58(GlobalContext* globalCtx, Player* this, PlayerFunc674 func, s32 flags) {
    if (func == this->func_674) {
        return 0;
    }

    if (func_8084E3C4 == this->func_674) {
        Audio_OcaSetInstrument(0);
        this->stateFlags2 &= ~(PLAYER_STATE2_24 | PLAYER_STATE2_25);
    }
    else if (func_808507F4 == this->func_674) {
        func_80832340(globalCtx, this);
    }

    this->func_674 = func;

    if ((this->itemActionParam != this->heldItemActionParam) &&
        (!(flags & 1) || !(this->stateFlags1 & PLAYER_STATE1_22))) {
        func_8008EC70(this);
    }

    if (!(flags & 1) && (!(this->stateFlags1 & PLAYER_STATE1_11))) {
        func_80834644(globalCtx, this);
        this->stateFlags1 &= ~PLAYER_STATE1_22;
    }

    func_80832DBC(this);
    this->stateFlags1 &= ~(PLAYER_STATE1_2 | PLAYER_STATE1_6 | PLAYER_STATE1_26 | PLAYER_STATE1_28 | PLAYER_STATE1_29 |
        PLAYER_STATE1_31);
    this->stateFlags2 &= ~(PLAYER_STATE2_19 | PLAYER_STATE2_27 | PLAYER_STATE2_28);
    this->stateFlags3 &= ~(PLAYER_STATE3_1 | PLAYER_STATE3_3 | PLAYER_STATE3_7);
    this->unk_84F = 0;
    this->unk_850 = 0;
    this->unk_6AC = 0;
    func_808326F0(this);

    return 1;
}

void func_80835DAC(GlobalContext* globalCtx, Player* this, PlayerFunc674 func, s32 flags) {
    s32 temp;

    temp = this->skelAnime.moveFlags;
    this->skelAnime.moveFlags = 0;
    func_80835C58(globalCtx, this, func, flags);
    this->skelAnime.moveFlags = temp;
}

void func_80835DE4(GlobalContext* globalCtx, Player* this, PlayerFunc674 func, s32 flags) {
    s32 temp;

    if (this->itemActionParam >= 0) {
        temp = this->itemActionParam;
        this->itemActionParam = this->heldItemActionParam;
        func_80835C58(globalCtx, this, func, flags);
        this->itemActionParam = temp;
        Player_SetModels(this, Player_ActionToModelGroup(this, this->itemActionParam));
    }
}

void func_80835E44(GlobalContext* globalCtx, s16 camSetting) {
    if (!func_800C0CB8(globalCtx)) {
        if (camSetting == CAM_SET_SCENE_TRANSITION) {
            Interface_ChangeAlpha(2);
        }
    }
    else {
        Camera_ChangeSetting(Gameplay_GetCamera(globalCtx, 0), camSetting);
    }
}

void func_80835EA4(GlobalContext* globalCtx, s32 arg1) {
    func_80835E44(globalCtx, CAM_SET_TURN_AROUND);
    Camera_SetCameraData(Gameplay_GetCamera(globalCtx, 0), 4, 0, 0, arg1, 0, 0);
}

void func_80835EFC(Player* this) {
    if (Player_HoldsHookshot(this)) {
        Actor* heldActor = this->heldActor;

        if (heldActor != NULL) {
            Actor_Kill(heldActor);
            this->actor.child = NULL;
            this->heldActor = NULL;
        }
    }
}

void func_80835F44(GlobalContext* globalCtx, Player* this, s32 item) {
    s8 actionParam;
    s32 temp;
    s32 nextAnimType;

    actionParam = Player_ItemToActionParam(item);

    if (((this->heldItemActionParam == this->itemActionParam) &&
        (!(this->stateFlags1 & PLAYER_STATE1_22) || (Player_ActionToSword(actionParam) != 0) ||
            (actionParam == PLAYER_AP_NONE))) ||
        ((this->itemActionParam < 0) &&
            ((Player_ActionToSword(actionParam) != 0) || (actionParam == PLAYER_AP_NONE)))) {

        if ((actionParam == PLAYER_AP_NONE) || !(this->stateFlags1 & PLAYER_STATE1_27) ||
            ((this->actor.bgCheckFlags & 1) &&
                ((actionParam == PLAYER_AP_HOOKSHOT) || (actionParam == PLAYER_AP_LONGSHOT)))) {

            if ((globalCtx->bombchuBowlingStatus == 0) &&
                (((actionParam == PLAYER_AP_STICK) && (AMMO(ITEM_STICK) == 0)) ||
                    ((actionParam == PLAYER_AP_BEAN) && (AMMO(ITEM_BEAN) == 0)) ||
                    (temp = Player_ActionToExplosive(this, actionParam),
                        ((temp >= 0) && ((AMMO(sExplosiveInfos[temp].itemId) == 0) ||
                            (globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].length >= 3)))))) {
                func_80078884(NA_SE_SY_ERROR);
                return;
            }

            if (actionParam == PLAYER_AP_LENS) {
                if (func_80087708(globalCtx, 0, 3)) {
                    if (globalCtx->actorCtx.unk_03 != 0) {
                        func_800304B0(globalCtx);
                    }
                    else {
                        globalCtx->actorCtx.unk_03 = 1;
                    }
                    func_80078884((globalCtx->actorCtx.unk_03 != 0) ? NA_SE_SY_GLASSMODE_ON : NA_SE_SY_GLASSMODE_OFF);
                }
                else {
                    func_80078884(NA_SE_SY_ERROR);
                }
                return;
            }

            if (actionParam == PLAYER_AP_NUT) {
                if (AMMO(ITEM_NUT) != 0) {
                    func_8083C61C(globalCtx, this);
                }
                else {
                    func_80078884(NA_SE_SY_ERROR);
                }
                return;
            }

            temp = Player_ActionToMagicSpell(this, actionParam);
            if (temp >= 0) {
                if (((actionParam == PLAYER_AP_FARORES_WIND) && (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 0)) ||
                    ((gSaveContext.unk_13F4 != 0) && (gSaveContext.unk_13F0 == 0) &&
                        (gSaveContext.magic >= sMagicSpellCosts[temp]))) {
                    this->itemActionParam = actionParam;
                    this->unk_6AD = 4;
                }
                else {
                    func_80078884(NA_SE_SY_ERROR);
                }
                return;
            }

            if (actionParam >= PLAYER_AP_MASK_KEATON) {
                if (this->currentMask != PLAYER_MASK_NONE) {
                    this->currentMask = PLAYER_MASK_NONE;
                }
                else {
                    this->currentMask = actionParam - PLAYER_AP_MASK_KEATON + 1;
                }
                func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
                return;
            }

            if (((actionParam >= PLAYER_AP_OCARINA_FAIRY) && (actionParam <= PLAYER_AP_OCARINA_TIME)) ||
                (actionParam >= PLAYER_AP_BOTTLE_FISH)) {
                if (!func_8008E9C4(this) ||
                    ((actionParam >= PLAYER_AP_BOTTLE_POTION_RED) && (actionParam <= PLAYER_AP_BOTTLE_FAIRY))) {
                    func_8002D53C(globalCtx, &globalCtx->actorCtx.titleCtx);
                    this->unk_6AD = 4;
                    this->itemActionParam = actionParam;
                }
                return;
            }

            if ((actionParam != this->heldItemActionParam) ||
                ((this->heldActor == 0) && (Player_ActionToExplosive(this, actionParam) >= 0))) {
                this->nextModelGroup = Player_ActionToModelGroup(this, actionParam);
                nextAnimType = gPlayerModelTypes[this->nextModelGroup][PLAYER_MODELGROUPENTRY_ANIM];
                if ((this->heldItemActionParam >= 0) && (Player_ActionToMagicSpell(this, actionParam) < 0) &&
                    (item != this->heldItemId) &&
                    (D_80854164[gPlayerModelTypes[this->modelGroup][PLAYER_MODELGROUPENTRY_ANIM]][nextAnimType] !=
                        PLAYER_D_808540F4_0)) {
                    this->heldItemId = item;
                    this->stateFlags1 |= PLAYER_STATE1_8;
                }
                else {
                    func_80835EFC(this);
                    func_808323B4(globalCtx, this);
                    func_80833664(globalCtx, this, actionParam);
                }
                return;
            }

            D_80853614 = D_80853618 = true;
        }
    }
}

void func_80836448(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    s32 cond = func_808332B8(this);

    func_80832564(globalCtx, this);

    func_80835C58(globalCtx, this, cond ? func_8084E368 : func_80843CEC, 0);

    this->stateFlags1 |= PLAYER_STATE1_7;

    func_80832264(globalCtx, this, anim);
    if (anim == &gPlayerAnim_002878) {
        this->skelAnime.endFrame = 84.0f;
    }

    func_80832224(this);
    func_80832698(this, NA_SE_VO_LI_DOWN);

    if (this->actor.category == ACTORCAT_PLAYER) {
        func_800F47BC();

        if (Inventory_ConsumeFairy(globalCtx)) {
            globalCtx->gameOverCtx.state = GAMEOVER_REVIVE_START;
            this->unk_84F = 1;
        }
        else {
            globalCtx->gameOverCtx.state = GAMEOVER_DEATH_START;
            func_800F6AB0(0);
            Audio_PlayFanfare(NA_BGM_GAME_OVER);
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
        }

        OnePointCutscene_Init(globalCtx, 9806, cond ? 120 : 60, &this->actor, MAIN_CAM);
        ShrinkWindow_SetVal(0x20);
    }
}

s32 func_808365C8(Player* this) {
    return (!(func_808458D0 == this->func_674) ||
        ((this->stateFlags1 & PLAYER_STATE1_8) &&
            ((this->heldItemId == ITEM_LAST_USED) || (this->heldItemId == ITEM_NONE)))) &&
        (!(func_80834A2C == this->func_82C) ||
            (Player_ItemToActionParam(this->heldItemId) == this->heldItemActionParam));
}

s32 func_80836670(Player* this, GlobalContext* globalCtx) {
    if (!(this->stateFlags1 & PLAYER_STATE1_23) && (this->actor.parent != NULL) && Player_HoldsHookshot(this)) {
        func_80835C58(globalCtx, this, func_80850AEC, 1);
        this->stateFlags3 |= PLAYER_STATE3_7;
        func_80832264(globalCtx, this, &gPlayerAnim_002C90);
        func_80832F54(globalCtx, this, 0x9B);
        func_80832224(this);
        this->currentYaw = this->actor.shape.rot.y;
        this->actor.bgCheckFlags &= ~1;
        this->hoverBootsTimer = 0;
        this->unk_6AE |= 0x43;
        func_80832698(this, NA_SE_VO_LI_LASH);
        return 1;
    }

    if (func_808365C8(this)) {
        func_80834298(this, globalCtx);
        if (func_8084E604 == this->func_674) {
            return 1;
        }
    }

    if (!this->func_82C(this, globalCtx)) {
        return 0;
    }

    if (this->unk_830 != 0.0f) {
        if ((func_80833350(this) == 0) || (this->linearVelocity != 0.0f)) {
            AnimationContext_SetCopyFalse(globalCtx, this->skelAnime.limbCount, this->skelAnime2.jointTable,
                this->skelAnime.jointTable, D_80853410);
        }
        Math_StepToF(&this->unk_830, 0.0f, 0.25f);
        AnimationContext_SetInterp(globalCtx, this->skelAnime.limbCount, this->skelAnime.jointTable,
            this->skelAnime2.jointTable, 1.0f - this->unk_830);
    }
    else if ((func_80833350(this) == 0) || (this->linearVelocity != 0.0f)) {
        AnimationContext_SetCopyTrue(globalCtx, this->skelAnime.limbCount, this->skelAnime.jointTable,
            this->skelAnime2.jointTable, D_80853410);
    }
    else {
        AnimationContext_SetCopyAll(globalCtx, this->skelAnime.limbCount, this->skelAnime.jointTable,
            this->skelAnime2.jointTable);
    }

    return 1;
}

s32 func_80836898(GlobalContext* globalCtx, Player* this, PlayerFuncA74 func) {
    this->func_A74 = func;
    func_80835C58(globalCtx, this, func_808458D0, 0);
    this->stateFlags2 |= PLAYER_STATE2_6;
    return func_80832528(globalCtx, this);
}

void func_808368EC(Player* this, GlobalContext* globalCtx) {
    s16 previousYaw = this->actor.shape.rot.y;

    if (!(this->stateFlags2 & (PLAYER_STATE2_5 | PLAYER_STATE2_6))) {
        if ((this->unk_664 != NULL) &&
            ((globalCtx->actorCtx.targetCtx.unk_4B != 0) || (this->actor.category != ACTORCAT_PLAYER))) {
            Math_ScaledStepToS(&this->actor.shape.rot.y,
                Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_664->focus.pos), 4000);
        }
        else if ((this->stateFlags1 & PLAYER_STATE1_17) &&
            !(this->stateFlags2 & (PLAYER_STATE2_5 | PLAYER_STATE2_6))) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetYaw, 4000);
        }
    }
    else if (!(this->stateFlags2 & PLAYER_STATE2_6)) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->currentYaw, 2000);
    }

    this->unk_87C = this->actor.shape.rot.y - previousYaw;
}

s32 func_808369C8(s16* pValue, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5) {
    s16 temp1;
    s16 temp2;
    s16 temp3;

    temp1 = temp2 = arg4 - *pValue;
    temp2 = CLAMP(temp2, -arg5, arg5);
    *pValue += (s16)(temp1 - temp2);

    Math_ScaledStepToS(pValue, arg1, arg2);

    temp3 = *pValue;
    if (*pValue < -arg3) {
        *pValue = -arg3;
    }
    else if (*pValue > arg3) {
        *pValue = arg3;
    }
    return temp3 - *pValue;
}

s32 func_80836AB8(Player* this, s32 arg1) {
    s16 sp36;
    s16 var;

    var = this->actor.shape.rot.y;
    if (arg1 != 0) {
        var = this->actor.focus.rot.y;
        this->unk_6BC = this->actor.focus.rot.x;
        this->unk_6AE |= 0x41;
    }
    else {
        func_808369C8(&this->unk_6BC,
            func_808369C8(&this->unk_6B6, this->actor.focus.rot.x, 600, 10000, this->actor.focus.rot.x, 0),
            200, 4000, this->unk_6B6, 10000);
        sp36 = this->actor.focus.rot.y - var;
        func_808369C8(&sp36, 0, 200, 24000, this->unk_6BE, 8000);
        var = this->actor.focus.rot.y - sp36;
        func_808369C8(&this->unk_6B8, sp36 - this->unk_6BE, 200, 8000, sp36, 8000);
        func_808369C8(&this->unk_6BE, sp36, 200, 8000, this->unk_6B8, 8000);
        this->unk_6AE |= 0xD9;
    }

    return var;
}

void func_80836BEC(Player* this, GlobalContext* globalCtx) {
    s32 sp1C = 0;
    s32 zTrigPressed = CHECK_BTN_ALL(sControlInput->cur.button, BTN_Z);
    Actor* actorToTarget;
    s32 pad;
    s32 holdTarget;
    s32 cond;

    if (!zTrigPressed) {
        this->stateFlags1 &= ~PLAYER_STATE1_30;
    }

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) || (this->csMode != 0) ||
        (this->stateFlags1 & (PLAYER_STATE1_7 | PLAYER_STATE1_29)) || (this->stateFlags3 & PLAYER_STATE3_7)) {
        this->unk_66C = 0;
    }
    else if (zTrigPressed || (this->stateFlags2 & PLAYER_STATE2_13) || (this->unk_684 != NULL)) {
        if (this->unk_66C <= 5) {
            this->unk_66C = 5;
        }
        else {
            this->unk_66C--;
        }
    }
    else if (this->stateFlags1 & PLAYER_STATE1_17) {
        this->unk_66C = 0;
    }
    else if (this->unk_66C != 0) {
        this->unk_66C--;
    }

    if (this->unk_66C >= 6) {
        sp1C = 1;
    }

    cond = func_8083224C(globalCtx);
    if (cond || (this->unk_66C != 0) || (this->stateFlags1 & (PLAYER_STATE1_12 | PLAYER_STATE1_25))) {
        if (!cond) {
            if (!(this->stateFlags1 & PLAYER_STATE1_25) &&
                ((this->heldItemActionParam != PLAYER_AP_FISHING_POLE) || (this->unk_860 == 0)) &&
                CHECK_BTN_ALL(sControlInput->press.button, BTN_Z)) {

                if (this->actor.category == ACTORCAT_PLAYER) {
                    actorToTarget = globalCtx->actorCtx.targetCtx.arrowPointedActor;
                }
                else {
                    actorToTarget = &GET_PLAYER(globalCtx)->actor;
                }

                holdTarget = (gSaveContext.zTargetSetting != 0) || (this->actor.category != ACTORCAT_PLAYER);
                this->stateFlags1 |= PLAYER_STATE1_15;

                if ((actorToTarget != NULL) && !(actorToTarget->flags & ACTOR_FLAG_27)) {
                    if ((actorToTarget == this->unk_664) && (this->actor.category == ACTORCAT_PLAYER)) {
                        actorToTarget = globalCtx->actorCtx.targetCtx.unk_94;
                    }

                    if (actorToTarget != this->unk_664) {
                        if (!holdTarget) {
                            this->stateFlags2 |= PLAYER_STATE2_13;
                        }
                        this->unk_664 = actorToTarget;
                        this->unk_66C = 15;
                        this->stateFlags2 &= ~(PLAYER_STATE2_1 | PLAYER_STATE2_21);
                    }
                    else {
                        if (!holdTarget) {
                            func_8008EDF0(this);
                        }
                    }

                    this->stateFlags1 &= ~PLAYER_STATE1_30;
                }
                else {
                    if (!(this->stateFlags1 & (PLAYER_STATE1_17 | PLAYER_STATE1_30))) {
                        func_808355DC(this);
                    }
                }
            }

            if (this->unk_664 != NULL) {
                if ((this->actor.category == ACTORCAT_PLAYER) && (this->unk_664 != this->unk_684) &&
                    func_8002F0C8(this->unk_664, this, sp1C)) {
                    func_8008EDF0(this);
                    this->stateFlags1 |= PLAYER_STATE1_30;
                }
                else if (this->unk_664 != NULL) {
                    this->unk_664->targetPriority = 40;
                }
            }
            else if (this->unk_684 != NULL) {
                this->unk_664 = this->unk_684;
            }
        }

        if (this->unk_664 != NULL) {
            this->stateFlags1 &= ~(PLAYER_STATE1_16 | PLAYER_STATE1_17);
            if ((this->stateFlags1 & PLAYER_STATE1_11) ||
                !CHECK_FLAG_ALL(this->unk_664->flags, ACTOR_FLAG_0 | ACTOR_FLAG_2)) {
                this->stateFlags1 |= PLAYER_STATE1_16;
            }
        }
        else {
            if (this->stateFlags1 & PLAYER_STATE1_17) {
                this->stateFlags2 &= ~PLAYER_STATE2_13;
            }
            else {
                func_8008EE08(this);
            }
        }
    }
    else {
        func_8008EE08(this);
    }
}

s32 func_80836FAC(GlobalContext* globalCtx, Player* this, f32* arg2, s16* arg3, f32 arg4) {
    f32 temp_f2;
    f32 temp_f0;
    f32 temp_f14;
    f32 temp_f12;

    if ((this->unk_6AD != 0) || (globalCtx->sceneLoadFlag == 0x14) || (this->stateFlags1 & PLAYER_STATE1_0)) {
        *arg2 = 0.0f;
        *arg3 = this->actor.shape.rot.y;
    }
    else {
        *arg2 = D_808535D4;
        *arg3 = D_808535D8;

        if (arg4 != 0.0f) {
            *arg2 -= 20.0f;
            if (*arg2 < 0.0f) {
                *arg2 = 0.0f;
            }
            else {
                temp_f2 = 1.0f - Math_CosS(*arg2 * 450.0f);
                *arg2 = ((temp_f2 * temp_f2) * 30.0f) + 7.0f;
            }
        }
        else {
            *arg2 *= 0.8f;
        }

        if (D_808535D4 != 0.0f) {
            temp_f0 = Math_SinS(this->unk_898);
            temp_f12 = this->unk_880;
            temp_f14 = CLAMP(temp_f0, 0.0f, 0.6f);

            if (this->unk_6C4 != 0.0f) {
                temp_f12 = temp_f12 - (this->unk_6C4 * 0.008f);
                if (temp_f12 < 2.0f) {
                    temp_f12 = 2.0f;
                }
            }

            *arg2 = (*arg2 * 0.14f) - (8.0f * temp_f14 * temp_f14);
            *arg2 = CLAMP(*arg2, 0.0f, temp_f12);

            return 1;
        }
    }

    return 0;
}

s32 func_8083721C(Player* this) {
    return Math_StepToF(&this->linearVelocity, 0.0f, REG(43) / 100.0f);
}

s32 func_80837268(Player* this, f32* arg1, s16* arg2, f32 arg3, GlobalContext* globalCtx) {
    if (!func_80836FAC(globalCtx, this, arg1, arg2, arg3)) {
        *arg2 = this->actor.shape.rot.y;

        if (this->unk_664 != NULL) {
            if ((globalCtx->actorCtx.targetCtx.unk_4B != 0) && !(this->stateFlags2 & PLAYER_STATE2_6)) {
                *arg2 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_664->focus.pos);
                return 0;
            }
        }
        else if (func_80833B2C(this)) {
            *arg2 = this->targetYaw;
        }

        return 0;
    }
    else {
        *arg2 += Camera_GetInputDirYaw(GET_ACTIVE_CAM(globalCtx));
        return 1;
    }
}

static s8 D_808543E0[] = { 13, 2, 4, 9, 10, 11, 8, -7 };
static s8 D_808543E8[] = { 13, 1, 2, 5, 3, 4, 9, 10, 11, 7, 8, -6 };
static s8 D_808543F4[] = { 13, 1, 2, 3, 4, 9, 10, 11, 8, 7, -6 };
static s8 D_80854400[] = { 13, 2, 4, 9, 10, 11, 8, -7 };
static s8 D_80854408[] = { 13, 2, 4, 9, 10, 11, 12, 8, -7 };
static s8 D_80854414[] = { -7 };
static s8 D_80854418[] = { 0, 11, 1, 2, 3, 5, 4, 9, 8, 7, -6 };
static s8 D_80854424[] = { 0, 11, 1, 2, 3, 12, 5, 4, 9, 8, 7, -6 };
static s8 D_80854430[] = { 13, 1, 2, 3, 12, 5, 4, 9, 10, 11, 8, 7, -6 };
static s8 D_80854440[] = { 10, 8, -7 };
static s8 D_80854444[] = { 0, 12, 5, -4 };

static s32(*D_80854448[])(Player* this, GlobalContext* globalCtx) = {
    func_8083B998, func_80839800, func_8083E5A8, func_8083E0FC, func_8083B644, func_8083F7BC, func_8083C1DC,
    func_80850224, func_8083C544, func_8083EB44, func_8083BDBC, func_8083C2B0, func_80838A14, func_8083B040,
};

s32 func_80837348(GlobalContext* globalCtx, Player* this, s8* arg2, s32 arg3) {
    s32 i;

    if (!(this->stateFlags1 & (PLAYER_STATE1_0 | PLAYER_STATE1_7 | PLAYER_STATE1_29))) {
        if (arg3 != 0) {
            D_808535E0 = func_80836670(this, globalCtx);
            if (func_8084E604 == this->func_674) {
                return 1;
            }
        }

        if (func_8008F128(this)) {
            this->unk_6AE |= 0x41;
            return 1;
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_8) && (func_80834A2C != this->func_82C)) {
            while (*arg2 >= 0) {
                if (D_80854448[*arg2](this, globalCtx)) {
                    return 1;
                }
                arg2++;
            }

            if (D_80854448[-(*arg2)](this, globalCtx)) {
                return 1;
            }
        }
    }

    return 0;
}

s32 func_808374A0(GlobalContext* globalCtx, Player* this, SkelAnime* skelAnime, f32 arg3) {
    f32 sp24;
    s16 sp22;

    if ((skelAnime->endFrame - arg3) <= skelAnime->curFrame) {
        if (func_80837348(globalCtx, this, D_80854418, 1)) {
            return 0;
        }

        if (func_80837268(this, &sp24, &sp22, 0.018f, globalCtx)) {
            return 1;
        }
    }

    return -1;
}

void func_80837530(GlobalContext* globalCtx, Player* this, s32 arg2) {
    if (arg2 != 0) {
        this->unk_858 = 0.0f;
    }
    else {
        this->unk_858 = 0.5f;
    }

    this->stateFlags1 |= PLAYER_STATE1_12;

    if (this->actor.category == ACTORCAT_PLAYER) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_M_THUNDER, this->bodyPartsPos[PLAYER_BODYPART_WAIST].x,
            this->bodyPartsPos[PLAYER_BODYPART_WAIST].y, this->bodyPartsPos[PLAYER_BODYPART_WAIST].z, 0, 0, 0,
            Player_GetSwordHeld(this) | arg2);
    }
}

s32 func_808375D8(Player* this) {
    s8 sp3C[4];
    s8* iter;
    s8* iter2;
    s8 temp1;
    s8 temp2;
    s32 i;

    if ((this->heldItemActionParam == PLAYER_AP_STICK) || Player_HoldsBrokenKnife(this)) {
        return 0;
    }

    iter = &this->unk_847[0];
    iter2 = &sp3C[0];
    for (i = 0; i < 4; i++, iter++, iter2++) {
        if ((*iter2 = *iter) < 0) {
            return 0;
        }
        *iter2 *= 2;
    }

    temp1 = sp3C[0] - sp3C[1];
    if (ABS(temp1) < 10) {
        return 0;
    }

    iter2 = &sp3C[1];
    for (i = 1; i < 3; i++, iter2++) {
        temp2 = *iter2 - *(iter2 + 1);
        if ((ABS(temp2) < 10) || (temp2 * temp1 < 0)) {
            return 0;
        }
    }

    return 1;
}

void func_80837704(GlobalContext* globalCtx, Player* this) {
    LinkAnimationHeader* anim;

    if ((this->swordAnimation >= 4) && (this->swordAnimation < 8)) {
        anim = D_80854358[Player_HoldsTwoHandedWeapon(this)];
    }
    else {
        anim = D_80854350[Player_HoldsTwoHandedWeapon(this)];
    }

    func_80832318(this);
    LinkAnimation_Change(globalCtx, &this->skelAnime, anim, 1.0f, 8.0f, Animation_GetLastFrame(anim), ANIMMODE_ONCE,
        -9.0f);
    func_80837530(globalCtx, this, 0x200);
}

void func_808377DC(GlobalContext* globalCtx, Player* this) {
    func_80835C58(globalCtx, this, func_80844E68, 1);
    func_80837704(globalCtx, this);
}

static s8 D_80854480[] = { 12, 4, 4, 8 };
static s8 D_80854484[] = { 22, 23, 22, 23 };

s32 func_80837818(Player* this) {
    s32 sp1C = this->unk_84B[this->unk_846];
    s32 sp18;

    if (this->heldItemActionParam == PLAYER_AP_HAMMER) {
        if (sp1C < 0) {
            sp1C = 0;
        }
        sp18 = D_80854484[sp1C];
        this->unk_845 = 0;
    }
    else {
        if (func_808375D8(this)) {
            sp18 = 24;
        }
        else {
            if (sp1C < 0) {
                if (func_80833BCC(this)) {
                    sp18 = 0;
                }
                else {
                    sp18 = 4;
                }
            }
            else {
                sp18 = D_80854480[sp1C];
                if (sp18 == 12) {
                    this->stateFlags2 |= PLAYER_STATE2_30;
                    if (!func_80833BCC(this)) {
                        sp18 = 0;
                    }
                }
            }
            if (this->heldItemActionParam == PLAYER_AP_STICK) {
                sp18 = 0;
            }
        }
        if (Player_HoldsTwoHandedWeapon(this)) {
            sp18++;
        }
    }

    return sp18;
}

void func_80837918(Player* this, s32 quadIndex, u32 flags) {
    this->swordQuads[quadIndex].info.toucher.dmgFlags = flags;

    if (flags == 2) {
        this->swordQuads[quadIndex].info.toucherFlags = TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_WOOD;
    }
    else {
        this->swordQuads[quadIndex].info.toucherFlags = TOUCH_ON | TOUCH_NEAREST;
    }
}

static u32 D_80854488[][2] = {
    { 0x00000200, 0x08000000 }, { 0x00000100, 0x02000000 }, { 0x00000400, 0x04000000 },
    { 0x00000002, 0x08000000 }, { 0x00000040, 0x40000000 },
};

void func_80837948(GlobalContext* globalCtx, Player* this, s32 arg2) {
    s32 pad;
    u32 flags;
    s32 temp;

    func_80835C58(globalCtx, this, func_808502D0, 0);
    this->unk_844 = 8;
    if ((arg2 < 18) || (arg2 >= 20)) {
        func_80832318(this);
    }

    if ((arg2 != this->swordAnimation) || !(this->unk_845 < 3)) {
        this->unk_845 = 0;
    }

    this->unk_845++;
    if (this->unk_845 >= 3) {
        arg2 += 2;
    }

    this->swordAnimation = arg2;

    func_808322D0(globalCtx, this, D_80854190[arg2].unk_00);
    if ((arg2 != 16) && (arg2 != 17)) {
        func_80832F54(globalCtx, this, 0x209);
    }

    this->currentYaw = this->actor.shape.rot.y;

    if (Player_HoldsBrokenKnife(this)) {
        temp = 1;
    }
    else {
        temp = Player_GetSwordHeld(this) - 1;
    }

    if ((arg2 >= 16) && (arg2 < 20)) {
        flags = D_80854488[temp][1];
    }
    else {
        flags = D_80854488[temp][0];
    }

    func_80837918(this, 0, flags);
    func_80837918(this, 1, flags);
}

void func_80837AE0(Player* this, s32 timer) {
    if (this->invincibilityTimer >= 0) {
        this->invincibilityTimer = timer;
        this->unk_88F = 0;
    }
}

void func_80837AFC(Player* this, s32 timer) {
    if (this->invincibilityTimer > timer) {
        this->invincibilityTimer = timer;
    }
    this->unk_88F = 0;
}

s32 func_80837B18(GlobalContext* globalCtx, Player* this, s32 damage) {
    if ((this->invincibilityTimer != 0) || (this->actor.category != ACTORCAT_PLAYER)) {
        return 1;
    }

    return Health_ChangeBy(globalCtx, damage);
}

void func_80837B60(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
    func_80832E48(this, 3);
}

void func_80837B9C(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_8084411C, 0);
    func_80832284(globalCtx, this, &gPlayerAnim_003040);
    this->unk_850 = 1;
    if (this->unk_6AD != 3) {
        this->unk_6AD = 0;
    }
}

static LinkAnimationHeader* D_808544B0[] = {
    &gPlayerAnim_002F80, &gPlayerAnim_002F78, &gPlayerAnim_002DE0, &gPlayerAnim_002DD8,
    &gPlayerAnim_002F70, &gPlayerAnim_002528, &gPlayerAnim_002DC8, &gPlayerAnim_0024F0,
};

void func_80837C0C(GlobalContext* globalCtx, Player* this, s32 arg2, f32 arg3, f32 arg4, s16 arg5, s32 arg6) {
    LinkAnimationHeader* sp2C = NULL;
    LinkAnimationHeader** sp28;

    if (this->stateFlags1 & PLAYER_STATE1_13) {
        func_80837B60(this);
    }

    this->unk_890 = 0;

    func_8002F7DC(&this->actor, NA_SE_PL_DAMAGE);

    if (!func_80837B18(globalCtx, this, 0 - this->actor.colChkInfo.damage)) {
        this->stateFlags2 &= ~PLAYER_STATE2_7;
        if (!(this->actor.bgCheckFlags & 1) && !(this->stateFlags1 & PLAYER_STATE1_27)) {
            func_80837B9C(this, globalCtx);
        }
        return;
    }

    func_80837AE0(this, arg6);

    if (arg2 == 3) {
        func_80835C58(globalCtx, this, func_8084FB10, 0);

        sp2C = &gPlayerAnim_002FD0;

        func_80832224(this);
        func_8083264C(this, 255, 10, 40, 0);

        func_8002F7DC(&this->actor, NA_SE_PL_FREEZE_S);
        func_80832698(this, NA_SE_VO_LI_FREEZE);
    }
    else if (arg2 == 4) {
        func_80835C58(globalCtx, this, func_8084FBF4, 0);

        func_8083264C(this, 255, 80, 150, 0);

        func_808322A4(globalCtx, this, &gPlayerAnim_002F00);
        func_80832224(this);

        this->unk_850 = 20;
    }
    else {
        arg5 -= this->actor.shape.rot.y;
        if (this->stateFlags1 & PLAYER_STATE1_27) {
            func_80835C58(globalCtx, this, func_8084E30C, 0);
            func_8083264C(this, 180, 20, 50, 0);

            this->linearVelocity = 4.0f;
            this->actor.velocity.y = 0.0f;

            sp2C = &gPlayerAnim_003320;

            func_80832698(this, NA_SE_VO_LI_DAMAGE_S);
        }
        else if ((arg2 == 1) || (arg2 == 2) || !(this->actor.bgCheckFlags & 1) ||
            (this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21))) {
            func_80835C58(globalCtx, this, func_8084377C, 0);

            this->stateFlags3 |= PLAYER_STATE3_1;

            func_8083264C(this, 255, 20, 150, 0);
            func_80832224(this);

            if (arg2 == 2) {
                this->unk_850 = 4;

                this->actor.speedXZ = 3.0f;
                this->linearVelocity = 3.0f;
                this->actor.velocity.y = 6.0f;

                func_80832C2C(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_3][this->modelAnimType]);
                func_80832698(this, NA_SE_VO_LI_DAMAGE_S);
            }
            else {
                this->actor.speedXZ = arg3;
                this->linearVelocity = arg3;
                this->actor.velocity.y = arg4;

                if (ABS(arg5) > 0x4000) {
                    sp2C = &gPlayerAnim_002F58;
                }
                else {
                    sp2C = &gPlayerAnim_002DB0;
                }

                if ((this->actor.category != ACTORCAT_PLAYER) && (this->actor.colChkInfo.health == 0)) {
                    func_80832698(this, NA_SE_VO_BL_DOWN);
                }
                else {
                    func_80832698(this, NA_SE_VO_LI_FALL_L);
                }
            }

            this->hoverBootsTimer = 0;
            this->actor.bgCheckFlags &= ~1;
        }
        else {
            if ((this->linearVelocity > 4.0f) && !func_8008E9C4(this)) {
                this->unk_890 = 20;
                func_8083264C(this, 120, 20, 10, 0);
                func_80832698(this, NA_SE_VO_LI_DAMAGE_S);
                return;
            }

            sp28 = D_808544B0;

            func_80835C58(globalCtx, this, func_8084370C, 0);
            func_80833C3C(this);

            if (this->actor.colChkInfo.damage < 5) {
                func_8083264C(this, 120, 20, 10, 0);
            }
            else {
                func_8083264C(this, 180, 20, 100, 0);
                this->linearVelocity = 23.0f;
                sp28 += 4;
            }

            if (ABS(arg5) <= 0x4000) {
                sp28 += 2;
            }

            if (func_8008E9C4(this)) {
                sp28 += 1;
            }

            sp2C = *sp28;

            func_80832698(this, NA_SE_VO_LI_DAMAGE_S);
        }

        this->actor.shape.rot.y += arg5;
        this->currentYaw = this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (ABS(arg5) > 0x4000) {
            this->actor.shape.rot.y += 0x8000;
        }
    }

    func_80832564(globalCtx, this);

    this->stateFlags1 |= PLAYER_STATE1_26;

    if (sp2C != NULL) {
        func_808322D0(globalCtx, this, sp2C);
    }
}

s32 func_80838144(s32 arg0) {
    s32 temp = arg0 - 2;

    if ((temp >= 0) && (temp < 2)) {
        return temp;
    }
    else {
        return -1;
    }
}

s32 func_8083816C(s32 arg0) {
    return (arg0 == 4) || (arg0 == 7) || (arg0 == 12);
}

void func_8083819C(Player* this, GlobalContext* globalCtx) {
    if (this->currentShield == PLAYER_SHIELD_DEKU) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_SHIELD, this->actor.world.pos.x,
            this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 1);
        Inventory_DeleteEquipment(globalCtx, EQUIP_SHIELD);
        Message_StartTextbox(globalCtx, 0x305F, NULL);
    }
}

void func_8083821C(Player* this) {
    s32 i;

    // clang-format off
    for (i = 0; i < 18; i++) { this->flameTimers[i] = Rand_S16Offset(0, 200); }
    // clang-format on

    this->isBurning = true;
}

void func_80838280(Player* this) {
    if (this->actor.colChkInfo.acHitEffect == 1) {
        func_8083821C(this);
    }
    func_80832698(this, NA_SE_VO_LI_FALL_L);
}

void func_808382BC(Player* this) {
    if ((this->invincibilityTimer >= 0) && (this->invincibilityTimer < 20)) {
        this->invincibilityTimer = 20;
    }
}

s32 func_808382DC(Player* this, GlobalContext* globalCtx) {
    s32 pad;
    s32 sp68 = false;
    s32 sp64;

    if (this->unk_A86 != 0) {
        if (!Player_InBlockingCsMode(globalCtx, this)) {
            Player_InflictDamage(globalCtx, -16);
            this->unk_A86 = 0;
        }
    }
    else {
        sp68 = ((Player_GetHeight(this) - 8.0f) < (this->unk_6C4 * this->actor.scale.y));

        if (sp68 || (this->actor.bgCheckFlags & 0x100) || (D_808535E4 == 9) || (this->stateFlags2 & PLAYER_STATE2_31)) {
            func_80832698(this, NA_SE_VO_LI_DAMAGE_S);

            if (sp68) {
                Gameplay_TriggerRespawn(globalCtx);
                func_800994A0(globalCtx);
            }
            else {
                // Special case for getting crushed in Forest Temple's Checkboard Ceiling Hall or Shadow Temple's
                // Falling Spike Trap Room, to respawn the player in a specific place
                if (((globalCtx->sceneNum == SCENE_BMORI1) && (globalCtx->roomCtx.curRoom.num == 15)) ||
                    ((globalCtx->sceneNum == SCENE_HAKADAN) && (globalCtx->roomCtx.curRoom.num == 10))) {
                    static SpecialRespawnInfo checkboardCeilingRespawn = { { 1992.0f, 403.0f, -3432.0f }, 0 };
                    static SpecialRespawnInfo fallingSpikeTrapRespawn = { { 1200.0f, -1343.0f, 3850.0f }, 0 };
                    SpecialRespawnInfo* respawnInfo;

                    if (globalCtx->sceneNum == SCENE_BMORI1) {
                        respawnInfo = &checkboardCeilingRespawn;
                    }
                    else {
                        respawnInfo = &fallingSpikeTrapRespawn;
                    }

                    Gameplay_SetupRespawnPoint(globalCtx, RESPAWN_MODE_DOWN, 0xDFF);
                    gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = respawnInfo->pos;
                    gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = respawnInfo->yaw;
                }

                Gameplay_TriggerVoidOut(globalCtx);
            }

            func_80832698(this, NA_SE_VO_LI_TAKEN_AWAY);
            globalCtx->unk_11DE9 = 1;
            func_80078884(NA_SE_OC_ABYSS);
        }
        else if ((this->unk_8A1 != 0) && ((this->unk_8A1 >= 2) || (this->invincibilityTimer == 0))) {
            u8 sp5C[] = { 2, 1, 1 };

            func_80838280(this);

            if (this->unk_8A1 == 3) {
                this->shockTimer = 40;
            }

            this->actor.colChkInfo.damage += this->unk_8A0;
            func_80837C0C(globalCtx, this, sp5C[this->unk_8A1 - 1], this->unk_8A4, this->unk_8A8, this->unk_8A2, 20);
        }
        else {
            sp64 = (this->shieldQuad.base.acFlags & AC_BOUNCED) != 0;

            //! @bug The second set of conditions here seems intended as a way for Link to "block" hits by rolling.
            // However, `Collider.atFlags` is a byte so the flag check at the end is incorrect and cannot work.
            // Additionally, `Collider.atHit` can never be set while already colliding as AC, so it's also bugged.
            // This behavior was later fixed in MM, most likely by removing both the `atHit` and `atFlags` checks.
            if (sp64 || ((this->invincibilityTimer < 0) && (this->cylinder.base.acFlags & AC_HIT) &&
                (this->cylinder.info.atHit != NULL) && (this->cylinder.info.atHit->atFlags & 0x20000000))) {

                func_8083264C(this, 180, 20, 100, 0);

                if (!Player_IsChildWithHylianShield(this)) {
                    if (this->invincibilityTimer >= 0) {
                        LinkAnimationHeader* anim;
                        s32 sp54 = func_80843188 == this->func_674;

                        if (!func_808332B8(this)) {
                            func_80835C58(globalCtx, this, func_808435C4, 0);
                        }

                        if (!(this->unk_84F = sp54)) {
                            func_80833638(this, func_80834BD4);

                            if (this->unk_870 < 0.5f) {
                                anim = D_808543BC[Player_HoldsTwoHandedWeapon(this)];
                            }
                            else {
                                anim = D_808543B4[Player_HoldsTwoHandedWeapon(this)];
                            }
                            LinkAnimation_PlayOnce(globalCtx, &this->skelAnime2, anim);
                        }
                        else {
                            func_80832264(globalCtx, this, D_808543C4[Player_HoldsTwoHandedWeapon(this)]);
                        }
                    }

                    if (!(this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21))) {
                        this->linearVelocity = -18.0f;
                        this->currentYaw = this->actor.shape.rot.y;
                    }
                }

                if (sp64 && (this->shieldQuad.info.acHitInfo->toucher.effect == 1)) {
                    func_8083819C(this, globalCtx);
                }

                return 0;
            }

            if ((this->unk_A87 != 0) || (this->invincibilityTimer > 0) || (this->stateFlags1 & PLAYER_STATE1_26) ||
                (this->csMode != 0) || (this->swordQuads[0].base.atFlags & AT_HIT) ||
                (this->swordQuads[1].base.atFlags & AT_HIT)) {
                return 0;
            }

            if (this->cylinder.base.acFlags & AC_HIT) {
                Actor* ac = this->cylinder.base.ac;
                s32 sp4C;

                if (ac->flags & ACTOR_FLAG_24) {
                    func_8002F7DC(&this->actor, NA_SE_PL_BODY_HIT);
                }

                if (this->stateFlags1 & PLAYER_STATE1_27) {
                    sp4C = 0;
                }
                else if (this->actor.colChkInfo.acHitEffect == 2) {
                    sp4C = 3;
                }
                else if (this->actor.colChkInfo.acHitEffect == 3) {
                    sp4C = 4;
                }
                else if (this->actor.colChkInfo.acHitEffect == 4) {
                    sp4C = 1;
                }
                else {
                    func_80838280(this);
                    sp4C = 0;
                }

                func_80837C0C(globalCtx, this, sp4C, 4.0f, 5.0f, Actor_WorldYawTowardActor(ac, &this->actor), 20);
            }
            else if (this->invincibilityTimer != 0) {
                return 0;
            }
            else {
                static u8 D_808544F4[] = { 120, 60 };
                s32 sp48 = func_80838144(D_808535E4);

                if (((this->actor.wallPoly != NULL) &&
                      SurfaceType_IsWallDamage(&globalCtx->colCtx, this->actor.wallPoly, this->actor.wallBgId)) ||
                    ((sp48 >= 0) &&
                        SurfaceType_IsWallDamage(&globalCtx->colCtx, this->actor.floorPoly, this->actor.floorBgId) &&
                        (this->unk_A79 >= D_808544F4[sp48])) ||
                    ((sp48 >= 0) &&
                        ((this->currentTunic != PLAYER_TUNIC_GORON && CVar_GetS32("gSuperTunic", 0) == 0) || (this->unk_A79 >= D_808544F4[sp48])))) {
                    this->unk_A79 = 0;
                    this->actor.colChkInfo.damage = 4;
                    func_80837C0C(globalCtx, this, 0, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
                }
                else {
                    return 0;
                }
            }
        }
    }

    return 1;
}

void func_80838940(Player* this, LinkAnimationHeader* anim, f32 arg2, GlobalContext* globalCtx, u16 sfxId) {
    func_80835C58(globalCtx, this, func_8084411C, 1);

    if (anim != NULL) {
        func_808322D0(globalCtx, this, anim);
    }

    this->actor.velocity.y = arg2 * D_808535E8;
    this->hoverBootsTimer = 0;
    this->actor.bgCheckFlags &= ~1;

    func_80832854(this);
    func_80832698(this, sfxId);

    this->stateFlags1 |= PLAYER_STATE1_18;
}

void func_808389E8(Player* this, LinkAnimationHeader* anim, f32 arg2, GlobalContext* globalCtx) {
    func_80838940(this, anim, arg2, globalCtx, NA_SE_VO_LI_SWORD_N);
}

s32 func_80838A14(Player* this, GlobalContext* globalCtx) {
    s32 sp3C;
    LinkAnimationHeader* sp38;
    f32 sp34;
    f32 temp;
    f32 sp2C;
    f32 sp28;
    f32 sp24;

    if (!(this->stateFlags1 & PLAYER_STATE1_11) && (this->unk_88C >= 2) &&
        (!(this->stateFlags1 & PLAYER_STATE1_27) || (this->ageProperties->unk_14 > this->wallHeight))) {
        sp3C = 0;

        if (func_808332B8(this)) {
            if (this->actor.yDistToWater < 50.0f) {
                if ((this->unk_88C < 2) || (this->wallHeight > this->ageProperties->unk_10)) {
                    return 0;
                }
            }
            else if ((this->currentBoots != PLAYER_BOOTS_IRON) || (this->unk_88C > 2)) {
                return 0;
            }
        }
        else if (!(this->actor.bgCheckFlags & 1) ||
            ((this->ageProperties->unk_14 <= this->wallHeight) && (this->stateFlags1 & PLAYER_STATE1_27))) {
            return 0;
        }

        if ((this->actor.wallBgId != BGCHECK_SCENE) && (D_808535F0 & 0x40)) {
            if (this->unk_88D >= 6) {
                this->stateFlags2 |= PLAYER_STATE2_2;
                if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
                    sp3C = 1;
                }
            }
        }
        else if ((this->unk_88D >= 6) || CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
            sp3C = 1;
        }

        if (sp3C != 0) {
            func_80835C58(globalCtx, this, func_80845668, 0);

            this->stateFlags1 |= PLAYER_STATE1_18;

            sp34 = this->wallHeight;

            if (this->ageProperties->unk_14 <= sp34) {
                sp38 = &gPlayerAnim_002D48;
                this->linearVelocity = 1.0f;
            }
            else {
                sp2C = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.x);
                sp28 = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.z);
                sp24 = this->wallDistance + 0.5f;

                this->stateFlags1 |= PLAYER_STATE1_14;

                if (func_808332B8(this)) {
                    sp38 = &gPlayerAnim_0032E8;
                    sp34 -= (60.0f * this->ageProperties->unk_08);
                    this->stateFlags1 &= ~PLAYER_STATE1_27;
                }
                else if (this->ageProperties->unk_18 <= sp34) {
                    sp38 = &gPlayerAnim_002D40;
                    sp34 -= (59.0f * this->ageProperties->unk_08);
                }
                else {
                    sp38 = &gPlayerAnim_002D38;
                    sp34 -= (41.0f * this->ageProperties->unk_08);
                }

                this->actor.shape.yOffset -= sp34 * 100.0f;

                this->actor.world.pos.x -= sp24 * sp2C;
                this->actor.world.pos.y += this->wallHeight;
                this->actor.world.pos.z -= sp24 * sp28;

                func_80832224(this);
            }

            this->actor.bgCheckFlags |= 1;

            LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, sp38, 1.3f);
            AnimationContext_DisableQueue(globalCtx);

            this->actor.shape.rot.y = this->currentYaw = this->actor.wallYaw + 0x8000;

            return 1;
        }
    }
    else if ((this->actor.bgCheckFlags & 1) && (this->unk_88C == 1) && (this->unk_88D >= 3)) {
        temp = (this->wallHeight * 0.08f) + 5.5f;
        func_808389E8(this, &gPlayerAnim_002FE0, temp, globalCtx);
        this->linearVelocity = 2.5f;

        return 1;
    }

    return 0;
}

void func_80838E70(GlobalContext* globalCtx, Player* this, f32 arg2, s16 arg3) {
    func_80835C58(globalCtx, this, func_80845CA4, 0);
    func_80832440(globalCtx, this);

    this->unk_84F = 1;
    this->unk_850 = 1;

    this->unk_450.x = (Math_SinS(arg3) * arg2) + this->actor.world.pos.x;
    this->unk_450.z = (Math_CosS(arg3) * arg2) + this->actor.world.pos.z;

    func_80832264(globalCtx, this, func_80833338(this));
}

void func_80838F18(GlobalContext* globalCtx, Player* this) {
    func_80835C58(globalCtx, this, func_8084D610, 0);
    func_80832C6C(globalCtx, this, &gPlayerAnim_003328);
}

void func_80838F5C(GlobalContext* globalCtx, Player* this) {
    func_80835C58(globalCtx, this, func_8084F88C, 0);

    this->stateFlags1 |= PLAYER_STATE1_29 | PLAYER_STATE1_31;

    Camera_ChangeSetting(Gameplay_GetCamera(globalCtx, 0), CAM_SET_FREE0);
}

s32 func_80838FB8(GlobalContext* globalCtx, Player* this) {
    if ((globalCtx->sceneLoadFlag == 0) && (this->stateFlags1 & PLAYER_STATE1_31)) {
        func_80838F5C(globalCtx, this);
        func_80832284(globalCtx, this, &gPlayerAnim_003040);
        func_80832698(this, NA_SE_VO_LI_FALL_S);
        func_800788CC(NA_SE_OC_SECRET_WARP_IN);
        return 1;
    }

    return 0;
}

s16 D_808544F8[] = {
    0x045B, // DMT from Magic Fairy Fountain
    0x0482, // DMC from Double Defense Fairy Fountain
    0x0340, // Hyrule Castle from Dins Fire Fairy Fountain
    0x044B, // Kakariko from Potion Shop
    0x02A2, // Market (child day) from Potion Shop
    0x0201, // Kakariko from Bazaar
    0x03B8, // Market (child day) from Bazaar
    0x04EE, // Kakariko from House of Skulltulas
    0x03C0, // Back Alley (day) from Bombchu Shop
    0x0463, // Kakariko from Shooting Gallery
    0x01CD, // Market (child day) from Shooting Gallery
    0x0394, // Zoras Fountain from Farores Wind Fairy Fountain
    0x0340, // Hyrule Castle from Dins Fire Fairy Fountain
    0x057C, // Desert Colossus from Nayrus Love Fairy Fountain
};

u8 D_80854514[] = { 11, 9, 3, 5, 7, 0 };

s32 func_80839034(GlobalContext* globalCtx, Player* this, CollisionPoly* poly, u32 bgId) {
    s32 sp3C;
    s32 temp;
    s32 sp34;
    f32 linearVel;
    s32 yaw;

    if (this->actor.category == ACTORCAT_PLAYER) {
        sp3C = 0;

        if (!(this->stateFlags1 & PLAYER_STATE1_7) && (globalCtx->sceneLoadFlag == 0) && (this->csMode == 0) &&
            !(this->stateFlags1 & PLAYER_STATE1_0) &&
            (((poly != NULL) && (sp3C = SurfaceType_GetSceneExitIndex(&globalCtx->colCtx, poly, bgId), sp3C != 0)) ||
                (func_8083816C(D_808535E4) && (this->unk_A7A == 12)))) {

            sp34 = this->unk_A84 - (s32)this->actor.world.pos.y;

            if (!(this->stateFlags1 & (PLAYER_STATE1_23 | PLAYER_STATE1_27 | PLAYER_STATE1_29)) &&
                !(this->actor.bgCheckFlags & 1) && (sp34 < 100) && (D_80853600 > 100.0f)) {
                return 0;
            }

            if (sp3C == 0) {
                Gameplay_TriggerVoidOut(globalCtx);
                func_800994A0(globalCtx);
            }
            else {
                globalCtx->nextEntranceIndex = globalCtx->setupExitList[sp3C - 1];
                if (globalCtx->nextEntranceIndex == 0x7FFF) {
                    gSaveContext.respawnFlag = 2;
                    globalCtx->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex;
                    globalCtx->fadeTransition = 3;
                    gSaveContext.nextTransition = 3;
                }
                else if (globalCtx->nextEntranceIndex >= 0x7FF9) {
                    globalCtx->nextEntranceIndex =
                        D_808544F8[D_80854514[globalCtx->nextEntranceIndex - 0x7FF9] + globalCtx->curSpawn];
                    func_800994A0(globalCtx);
                }
                else {
                    if (SurfaceType_GetSlope(&globalCtx->colCtx, poly, bgId) == 2) {
                        gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = globalCtx->nextEntranceIndex;
                        Gameplay_TriggerVoidOut(globalCtx);
                        gSaveContext.respawnFlag = -2;
                    }
                    gSaveContext.unk_13C3 = 1;
                    func_800994A0(globalCtx);
                }
                globalCtx->sceneLoadFlag = 0x14;
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_23 | PLAYER_STATE1_29)) &&
                !(this->stateFlags2 & PLAYER_STATE2_18) && !func_808332B8(this) &&
                (temp = func_80041D4C(&globalCtx->colCtx, poly, bgId), (temp != 10)) &&
                ((sp34 < 100) || (this->actor.bgCheckFlags & 1))) {

                if (temp == 11) {
                    func_800788CC(NA_SE_OC_SECRET_HOLE_OUT);
                    func_800F6964(5);
                    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
                    gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
                }
                else {
                    linearVel = this->linearVelocity;

                    if (linearVel < 0.0f) {
                        this->actor.world.rot.y += 0x8000;
                        linearVel = -linearVel;
                    }

                    if (linearVel > R_RUN_SPEED_LIMIT / 100.0f) {
                        gSaveContext.entranceSpeed = R_RUN_SPEED_LIMIT / 100.0f;
                    }
                    else {
                        gSaveContext.entranceSpeed = linearVel;
                    }

                    if (D_808535F4 != 0) {
                        yaw = D_808535FC;
                    }
                    else {
                        yaw = this->actor.world.rot.y;
                    }
                    func_80838E70(globalCtx, this, 400.0f, yaw);
                }
            }
            else {
                if (!(this->actor.bgCheckFlags & 1)) {
                    func_80832210(this);
                }
            }

            this->stateFlags1 |= PLAYER_STATE1_0 | PLAYER_STATE1_29;

            func_80835E44(globalCtx, 0x2F);

            return 1;
        }
        else {
            if (globalCtx->sceneLoadFlag == 0) {

                if ((this->actor.world.pos.y < -4000.0f) ||
                    (((this->unk_A7A == 5) || (this->unk_A7A == 12)) &&
                        ((D_80853600 < 100.0f) || (this->fallDistance > 400.0f) ||
                            ((globalCtx->sceneNum != SCENE_HAKADAN) && (this->fallDistance > 200.0f)))) ||
                    ((globalCtx->sceneNum == SCENE_GANON_FINAL) && (this->fallDistance > 320.0f))) {

                    if (this->actor.bgCheckFlags & 1) {
                        if (this->unk_A7A == 5) {
                            Gameplay_TriggerRespawn(globalCtx);
                        }
                        else {
                            Gameplay_TriggerVoidOut(globalCtx);
                        }
                        globalCtx->fadeTransition = 4;
                        func_80078884(NA_SE_OC_ABYSS);
                    }
                    else {
                        func_80838F5C(globalCtx, this);
                        this->unk_850 = 9999;
                        if (this->unk_A7A == 5) {
                            this->unk_84F = -1;
                        }
                        else {
                            this->unk_84F = 1;
                        }
                    }
                }

                this->unk_A84 = this->actor.world.pos.y;
            }
        }
    }

    return 0;
}

void func_808395DC(Player* this, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3) {
    f32 cos = Math_CosS(this->actor.shape.rot.y);
    f32 sin = Math_SinS(this->actor.shape.rot.y);

    arg3->x = arg1->x + ((arg2->x * cos) + (arg2->z * sin));
    arg3->y = arg1->y + arg2->y;
    arg3->z = arg1->z + ((arg2->z * cos) - (arg2->x * sin));
}

Actor* Player_SpawnFairy(GlobalContext* globalCtx, Player* this, Vec3f* arg2, Vec3f* arg3, s32 type) {
    Vec3f pos;

    func_808395DC(this, arg2, arg3, &pos);

    return Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ELF, pos.x, pos.y, pos.z, 0, 0, 0, type);
}

f32 func_808396F4(GlobalContext* globalCtx, Player* this, Vec3f* arg2, Vec3f* arg3, CollisionPoly** arg4, s32* arg5) {
    func_808395DC(this, &this->actor.world.pos, arg2, arg3);

    return BgCheck_EntityRaycastFloor3(&globalCtx->colCtx, arg4, arg5, arg3);
}

f32 func_8083973C(GlobalContext* globalCtx, Player* this, Vec3f* arg2, Vec3f* arg3) {
    CollisionPoly* sp24;
    s32 sp20;

    return func_808396F4(globalCtx, this, arg2, arg3, &sp24, &sp20);
}

s32 func_80839768(GlobalContext* globalCtx, Player* this, Vec3f* arg2, CollisionPoly** arg3, s32* arg4, Vec3f* arg5) {
    Vec3f sp44;
    Vec3f sp38;

    sp44.x = this->actor.world.pos.x;
    sp44.y = this->actor.world.pos.y + arg2->y;
    sp44.z = this->actor.world.pos.z;

    func_808395DC(this, &this->actor.world.pos, arg2, &sp38);

    return BgCheck_EntityLineTest1(&globalCtx->colCtx, &sp44, &sp38, arg5, arg3, true, false, false, true, arg4);
}

s32 func_80839800(Player* this, GlobalContext* globalCtx) {
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
    CollisionPoly* sp58;
    Vec3f sp4C;

    if ((this->doorType != PLAYER_DOORTYPE_NONE) &&
        (!(this->stateFlags1 & PLAYER_STATE1_11) ||
            ((this->heldActor != NULL) && (this->heldActor->id == ACTOR_EN_RU1)))) {
        if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) || (func_8084F9A0 == this->func_674)) {
            doorActor = this->doorActor;

            if (this->doorType <= PLAYER_DOORTYPE_AJAR) {
                doorActor->textId = 0xD0;
                func_80853148(globalCtx, doorActor);
                return 0;
            }

            doorDirection = this->doorDirection;
            sp78 = Math_CosS(doorActor->shape.rot.y);
            sp74 = Math_SinS(doorActor->shape.rot.y);

            if (this->doorType == PLAYER_DOORTYPE_SLIDING) {
                doorShutter = (DoorShutter*)doorActor;

                this->currentYaw = doorShutter->dyna.actor.home.rot.y;
                if (doorDirection > 0) {
                    this->currentYaw -= 0x8000;
                }
                this->actor.shape.rot.y = this->currentYaw;

                if (this->linearVelocity <= 0.0f) {
                    this->linearVelocity = 0.1f;
                }

                func_80838E70(globalCtx, this, 50.0f, this->actor.shape.rot.y);

                this->unk_84F = 0;
                this->unk_447 = this->doorType;
                this->stateFlags1 |= PLAYER_STATE1_29;

                this->unk_450.x = this->actor.world.pos.x + ((doorDirection * 20.0f) * sp74);
                this->unk_450.z = this->actor.world.pos.z + ((doorDirection * 20.0f) * sp78);
                this->unk_45C.x = this->actor.world.pos.x + ((doorDirection * -120.0f) * sp74);
                this->unk_45C.z = this->actor.world.pos.z + ((doorDirection * -120.0f) * sp78);

                doorShutter->unk_164 = 1;
                func_80832224(this);

                if (this->doorTimer != 0) {
                    this->unk_850 = 0;
                    func_80832B0C(globalCtx, this, func_80833338(this));
                    this->skelAnime.endFrame = 0.0f;
                }
                else {
                    this->linearVelocity = 0.1f;
                }

                if (doorShutter->dyna.actor.category == ACTORCAT_DOOR) {
                    this->unk_46A = globalCtx->transiActorCtx.list[(u16)doorShutter->dyna.actor.params >> 10]
                        .sides[(doorDirection > 0) ? 0 : 1]
                        .effects;

                    func_800304B0(globalCtx);
                }
            }
            else {
                // This actor can be either EnDoor or DoorKiller.
                // Don't try to access any struct vars other than `animStyle` and `playerIsOpening`! These two variables
                // are common across the two actors' structs however most other variables are not!
                door = (EnDoor*)doorActor;

                door->animStyle = (doorDirection < 0.0f) ? (LINK_IS_ADULT ? KNOB_ANIM_ADULT_L : KNOB_ANIM_CHILD_L)
                    : (LINK_IS_ADULT ? KNOB_ANIM_ADULT_R : KNOB_ANIM_CHILD_R);

                if (door->animStyle == KNOB_ANIM_ADULT_L) {
                    sp5C = D_80853914[PLAYER_ANIMGROUP_9][this->modelAnimType];
                }
                else if (door->animStyle == KNOB_ANIM_CHILD_L) {
                    sp5C = D_80853914[PLAYER_ANIMGROUP_10][this->modelAnimType];
                }
                else if (door->animStyle == KNOB_ANIM_ADULT_R) {
                    sp5C = D_80853914[PLAYER_ANIMGROUP_11][this->modelAnimType];
                }
                else {
                    sp5C = D_80853914[PLAYER_ANIMGROUP_12][this->modelAnimType];
                }

                func_80835C58(globalCtx, this, func_80845EF8, 0);
                func_80832528(globalCtx, this);

                if (doorDirection < 0) {
                    this->actor.shape.rot.y = doorActor->shape.rot.y;
                }
                else {
                    this->actor.shape.rot.y = doorActor->shape.rot.y - 0x8000;
                }

                this->currentYaw = this->actor.shape.rot.y;

                sp6C = (doorDirection * 22.0f);
                this->actor.world.pos.x = doorActor->world.pos.x + sp6C * sp74;
                this->actor.world.pos.z = doorActor->world.pos.z + sp6C * sp78;

                func_8083328C(globalCtx, this, sp5C);

                if (this->doorTimer != 0) {
                    this->skelAnime.endFrame = 0.0f;
                }

                func_80832224(this);
                func_80832F54(globalCtx, this, 0x28F);

                if (doorActor->parent != NULL) {
                    doorDirection = -doorDirection;
                }

                door->playerIsOpening = 1;

                if (this->doorType != PLAYER_DOORTYPE_FAKE) {
                    this->stateFlags1 |= PLAYER_STATE1_29;
                    func_800304B0(globalCtx);

                    if (((doorActor->params >> 7) & 7) == 3) {
                        sp4C.x = doorActor->world.pos.x - (sp6C * sp74);
                        sp4C.y = doorActor->world.pos.y + 10.0f;
                        sp4C.z = doorActor->world.pos.z - (sp6C * sp78);

                        BgCheck_EntityRaycastFloor1(&globalCtx->colCtx, &sp58, &sp4C);

                        if (func_80839034(globalCtx, this, sp58, BGCHECK_SCENE)) {
                            gSaveContext.entranceSpeed = 2.0f;
                            gSaveContext.entranceSound = NA_SE_OC_DOOR_OPEN;
                        }
                    }
                    else {
                        Camera_ChangeDoorCam(Gameplay_GetCamera(globalCtx, 0), doorActor,
                            globalCtx->transiActorCtx.list[(u16)doorActor->params >> 10]
                            .sides[(doorDirection > 0) ? 0 : 1]
                            .effects,
                            0, 38.0f * D_808535EC, 26.0f * D_808535EC, 10.0f * D_808535EC);
                    }
                }
            }

            if ((this->doorType != PLAYER_DOORTYPE_FAKE) && (doorActor->category == ACTORCAT_DOOR)) {
                frontRoom = globalCtx->transiActorCtx.list[(u16)doorActor->params >> 10]
                    .sides[(doorDirection > 0) ? 0 : 1]
                    .room;

                if ((frontRoom >= 0) && (frontRoom != globalCtx->roomCtx.curRoom.num)) {
                    func_8009728C(globalCtx, &globalCtx->roomCtx, frontRoom);
                }
            }

            doorActor->room = globalCtx->roomCtx.curRoom.num;

            if (((attachedActor = doorActor->child) != NULL) || ((attachedActor = doorActor->parent) != NULL)) {
                attachedActor->room = globalCtx->roomCtx.curRoom.num;
            }

            return 1;
        }
    }

    return 0;
}

void func_80839E88(Player* this, GlobalContext* globalCtx) {
    LinkAnimationHeader* anim;

    func_80835C58(globalCtx, this, func_80840450, 1);

    if (this->unk_870 < 0.5f) {
        anim = func_808334E4(this);
        this->unk_870 = 0.0f;
    }
    else {
        anim = func_80833528(this);
        this->unk_870 = 1.0f;
    }

    this->unk_874 = this->unk_870;
    func_80832284(globalCtx, this, anim);
    this->currentYaw = this->actor.shape.rot.y;
}

void func_80839F30(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_808407CC, 1);
    func_80832B0C(globalCtx, this, func_80833338(this));
    this->currentYaw = this->actor.shape.rot.y;
}

void func_80839F90(Player* this, GlobalContext* globalCtx) {
    if (func_8008E9C4(this)) {
        func_80839E88(this, globalCtx);
    }
    else if (func_80833B2C(this)) {
        func_80839F30(this, globalCtx);
    }
    else {
        func_80853080(this, globalCtx);
    }
}

void func_80839FFC(Player* this, GlobalContext* globalCtx) {
    PlayerFunc674 func;

    if (func_8008E9C4(this)) {
        func = func_80840450;
    }
    else if (func_80833B2C(this)) {
        func = func_808407CC;
    }
    else {
        func = func_80840BC8;
    }

    func_80835C58(globalCtx, this, func, 1);
}

void func_8083A060(Player* this, GlobalContext* globalCtx) {
    func_80839FFC(this, globalCtx);
    if (func_8008E9C4(this)) {
        this->unk_850 = 1;
    }
}

void func_8083A098(Player* this, LinkAnimationHeader* anim, GlobalContext* globalCtx) {
    func_8083A060(this, globalCtx);
    func_8083328C(globalCtx, this, anim);
}

s32 func_8083A0D4(Player* this) {
    return (this->interactRangeActor != NULL) && (this->heldActor == NULL);
}

void func_8083A0F4(GlobalContext* globalCtx, Player* this) {
    if (func_8083A0D4(this)) {
        Actor* interactRangeActor = this->interactRangeActor;
        s32 interactActorId = interactRangeActor->id;

        if (interactActorId == ACTOR_BG_TOKI_SWD) {
            this->interactRangeActor->parent = &this->actor;
            func_80835C58(globalCtx, this, func_8084F608, 0);
            this->stateFlags1 |= PLAYER_STATE1_29;
        }
        else {
            LinkAnimationHeader* anim;

            if (interactActorId == ACTOR_BG_HEAVY_BLOCK) {
                func_80835C58(globalCtx, this, func_80846120, 0);
                this->stateFlags1 |= PLAYER_STATE1_29;
                anim = &gPlayerAnim_002F98;
            }
            else if ((interactActorId == ACTOR_EN_ISHI) && ((interactRangeActor->params & 0xF) == 1)) {
                func_80835C58(globalCtx, this, func_80846260, 0);
                anim = &gPlayerAnim_0032B0;
            }
            else if (((interactActorId == ACTOR_EN_BOMBF) || (interactActorId == ACTOR_EN_KUSA)) &&
                (Player_GetStrength() <= PLAYER_STR_NONE)) {
                func_80835C58(globalCtx, this, func_80846408, 0);
                this->actor.world.pos.x =
                    (Math_SinS(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.x;
                this->actor.world.pos.z =
                    (Math_CosS(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.z;
                this->currentYaw = this->actor.shape.rot.y = interactRangeActor->yawTowardsPlayer + 0x8000;
                anim = &gPlayerAnim_003060;
            }
            else {
                func_80835C58(globalCtx, this, func_80846050, 0);
                anim = D_80853914[PLAYER_ANIMGROUP_13][this->modelAnimType];
            }

            func_80832264(globalCtx, this, anim);
        }
    }
    else {
        func_80839F90(this, globalCtx);
        this->stateFlags1 &= ~PLAYER_STATE1_11;
    }
}

void func_8083A2F8(GlobalContext* globalCtx, Player* this) {
    func_80835DAC(globalCtx, this, func_8084B530, 0);

    this->stateFlags1 |= PLAYER_STATE1_6 | PLAYER_STATE1_29;

    if (this->actor.textId != 0) {
        Message_StartTextbox(globalCtx, this->actor.textId, this->targetActor);
        this->unk_664 = this->targetActor;
    }
}

void func_8083A360(GlobalContext* globalCtx, Player* this) {
    func_80835DAC(globalCtx, this, func_8084CC98, 0);
}

void func_8083A388(GlobalContext* globalCtx, Player* this) {
    func_80835C58(globalCtx, this, func_8084B78C, 0);
}

void func_8083A3B0(GlobalContext* globalCtx, Player* this) {
    s32 sp1C = this->unk_850;
    s32 sp18 = this->unk_84F;

    func_80835DAC(globalCtx, this, func_8084BF1C, 0);
    this->actor.velocity.y = 0.0f;

    this->unk_850 = sp1C;
    this->unk_84F = sp18;
}

void func_8083A40C(GlobalContext* globalCtx, Player* this) {
    func_80835DAC(globalCtx, this, func_8084C760, 0);
}

void func_8083A434(GlobalContext* globalCtx, Player* this) {
    func_80835DAC(globalCtx, this, func_8084E6D4, 0);

    this->stateFlags1 |= PLAYER_STATE1_10 | PLAYER_STATE1_29;

    if (this->getItemId == GI_HEART_CONTAINER_2) {
        this->unk_850 = 20;
    }
    else if (this->getItemId >= 0) {
        this->unk_850 = 1;
    }
    else {
        this->getItemId = -this->getItemId;
    }
}

s32 func_8083A4A8(Player* this, GlobalContext* globalCtx) {
    s16 yawDiff;
    LinkAnimationHeader* anim;
    f32 temp;

    yawDiff = this->currentYaw - this->actor.shape.rot.y;

    if ((ABS(yawDiff) < 0x1000) && (this->linearVelocity > 4.0f)) {
        anim = &gPlayerAnim_003148;
    }
    else {
        anim = &gPlayerAnim_002FE0;
    }

    if (this->linearVelocity > (IREG(66) / 100.0f)) {
        temp = IREG(67) / 100.0f;
    }
    else {
        temp = (IREG(68) / 100.0f) + ((IREG(69) * this->linearVelocity) / 1000.0f);
    }

    func_80838940(this, anim, temp, globalCtx, NA_SE_VO_LI_AUTO_JUMP);
    this->unk_850 = 1;

    return 1;
}

void func_8083A5C4(GlobalContext* globalCtx, Player* this, CollisionPoly* arg2, f32 arg3, LinkAnimationHeader* arg4) {
    f32 sp24 = COLPOLY_GET_NORMAL(arg2->normal.x);
    f32 sp20 = COLPOLY_GET_NORMAL(arg2->normal.z);

    func_80835C58(globalCtx, this, func_8084BBE4, 0);
    func_80832564(globalCtx, this);
    func_80832264(globalCtx, this, arg4);

    this->actor.world.pos.x -= (arg3 + 1.0f) * sp24;
    this->actor.world.pos.z -= (arg3 + 1.0f) * sp20;
    this->actor.shape.rot.y = this->currentYaw = Math_Atan2S(sp20, sp24);

    func_80832224(this);
    func_80832CFC(this);
}

s32 func_8083A6AC(Player* this, GlobalContext* globalCtx) {
    CollisionPoly* sp84;
    s32 sp80;
    Vec3f sp74;
    Vec3f sp68;
    f32 temp1;

    if ((this->actor.yDistToWater < -80.0f) && (ABS(this->unk_898) < 2730) && (ABS(this->unk_89A) < 2730)) {
        sp74.x = this->actor.prevPos.x - this->actor.world.pos.x;
        sp74.z = this->actor.prevPos.z - this->actor.world.pos.z;

        temp1 = sqrtf(SQ(sp74.x) + SQ(sp74.z));
        if (temp1 != 0.0f) {
            temp1 = 5.0f / temp1;
        }
        else {
            temp1 = 0.0f;
        }

        sp74.x = this->actor.prevPos.x + (sp74.x * temp1);
        sp74.y = this->actor.world.pos.y;
        sp74.z = this->actor.prevPos.z + (sp74.z * temp1);

        if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &this->actor.world.pos, &sp74, &sp68, &sp84, true, false, false,
            true, &sp80) &&
            ((ABS(sp84->normal.y) < 600) || (CVar_GetS32("gClimbEverything", 0) != 0))) {
            f32 nx = COLPOLY_GET_NORMAL(sp84->normal.x);
            f32 ny = COLPOLY_GET_NORMAL(sp84->normal.y);
            f32 nz = COLPOLY_GET_NORMAL(sp84->normal.z);
            f32 sp54;
            s32 sp50;

            sp54 = Math3D_UDistPlaneToPos(nx, ny, nz, sp84->dist, &this->actor.world.pos);

            sp50 = D_80853604 == 6;
            if (!sp50 && (func_80041DB8(&globalCtx->colCtx, sp84, sp80) & 8)) {
                sp50 = 1;
            }

            func_8083A5C4(globalCtx, this, sp84, sp54, sp50 ? &gPlayerAnim_002D88 : &gPlayerAnim_002F10);

            if (sp50) {
                func_80836898(globalCtx, this, func_8083A3B0);

                this->currentYaw += 0x8000;
                this->actor.shape.rot.y = this->currentYaw;

                this->stateFlags1 |= PLAYER_STATE1_21;
                func_80832F54(globalCtx, this, 0x9F);

                this->unk_850 = -1;
                this->unk_84F = sp50;
            }
            else {
                this->stateFlags1 |= PLAYER_STATE1_13;
                this->stateFlags1 &= ~PLAYER_STATE1_17;
            }

            func_8002F7DC(&this->actor, NA_SE_PL_SLIPDOWN);
            func_80832698(this, NA_SE_VO_LI_HANG);
            return 1;
        }
    }

    return 0;
}

void func_8083A9B8(Player* this, LinkAnimationHeader* anim, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_8084BDFC, 0);
    LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, anim, 1.3f);
}

static Vec3f D_8085451C = { 0.0f, 0.0f, 100.0f };

void func_8083AA10(Player* this, GlobalContext* globalCtx) {
    s32 sp5C;
    CollisionPoly* sp58;
    s32 sp54;
    WaterBox* sp50;
    Vec3f sp44;
    f32 sp40;
    f32 sp3C;

    this->fallDistance = this->fallStartHeight - (s32)this->actor.world.pos.y;

    if (!(this->stateFlags1 & (PLAYER_STATE1_27 | PLAYER_STATE1_29)) && !(this->actor.bgCheckFlags & 1)) {
        if (!func_80838FB8(globalCtx, this)) {
            if (D_80853604 == 8) {
                this->actor.world.pos.x = this->actor.prevPos.x;
                this->actor.world.pos.z = this->actor.prevPos.z;
                return;
            }

            if (!(this->stateFlags3 & PLAYER_STATE3_1) && !(this->skelAnime.moveFlags & 0x80) &&
                (func_8084411C != this->func_674) && (func_80844A44 != this->func_674)) {

                if ((D_80853604 == 7) || (this->swordState != 0)) {
                    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
                    func_80832210(this);
                    return;
                }

                if (this->hoverBootsTimer != 0) {
                    this->actor.velocity.y = 1.0f;
                    D_80853604 = 9;
                    return;
                }

                sp5C = (s16)(this->currentYaw - this->actor.shape.rot.y);

                func_80835C58(globalCtx, this, func_8084411C, 1);
                func_80832440(globalCtx, this);

                this->unk_89E = this->unk_A82;

                if ((this->actor.bgCheckFlags & 4) && !(this->stateFlags1 & PLAYER_STATE1_27) && (D_80853604 != 6) &&
                    (D_80853604 != 9) && (D_80853600 > 20.0f) && (this->swordState == 0) && (ABS(sp5C) < 0x2000) &&
                    (this->linearVelocity > 3.0f)) {

                    if ((D_80853604 == 11) && !(this->stateFlags1 & PLAYER_STATE1_11)) {

                        sp40 = func_808396F4(globalCtx, this, &D_8085451C, &sp44, &sp58, &sp54);
                        sp3C = this->actor.world.pos.y;

                        if (WaterBox_GetSurface1(globalCtx, &globalCtx->colCtx, sp44.x, sp44.z, &sp3C, &sp50) &&
                            ((sp3C - sp40) > 50.0f)) {
                            func_808389E8(this, &gPlayerAnim_003158, 6.0f, globalCtx);
                            func_80835C58(globalCtx, this, func_80844A44, 0);
                            return;
                        }
                    }

                    func_8083A4A8(this, globalCtx);
                    return;
                }

                if ((D_80853604 == 9) || (D_80853600 <= this->ageProperties->unk_34) ||
                    !func_8083A6AC(this, globalCtx)) {
                    func_80832284(globalCtx, this, &gPlayerAnim_003040);
                    return;
                }
            }
        }
    }
    else {
        this->fallStartHeight = this->actor.world.pos.y;
    }
}

s32 func_8083AD4C(GlobalContext* globalCtx, Player* this) {
    s32 cameraMode;

    if (this->unk_6AD == 2) {
        if (func_8002DD6C(this)) {
            if (LINK_IS_ADULT) {
                cameraMode = CAM_MODE_BOWARROW;
            }
            else {
                cameraMode = CAM_MODE_SLINGSHOT;
            }
        }
        else {
            cameraMode = CAM_MODE_BOOMERANG;
        }
    }
    else {
        cameraMode = CAM_MODE_FIRSTPERSON;
    }

    return Camera_ChangeMode(Gameplay_GetCamera(globalCtx, 0), cameraMode);
}

s32 func_8083ADD4(GlobalContext* globalCtx, Player* this) {
    if (this->unk_6AD == 3) {
        func_80835C58(globalCtx, this, func_80852E14, 0);
        if (this->unk_46A != 0) {
            this->stateFlags1 |= PLAYER_STATE1_29;
        }
        func_80832318(this);
        return 1;
    }
    else {
        return 0;
    }
}

void func_8083AE40(Player* this, s16 objectId) {
    s32 pad;
    u32 size;

    if (objectId != OBJECT_INVALID) {
        this->giObjectLoading = true;
        osCreateMesgQueue(&this->giObjectLoadQueue, &this->giObjectLoadMsg, 1);

        size = gObjectTable[objectId].vromEnd - gObjectTable[objectId].vromStart;

        LOG_HEX("size", size, "../z_player.c", 9090);
        ASSERT(size <= 1024 * 8, "size <= 1024 * 8", "../z_player.c", 9091);

        DmaMgr_SendRequest2(&this->giObjectDmaRequest, (u32)this->giObjectSegment, gObjectTable[objectId].vromStart,
            size, 0, &this->giObjectLoadQueue, NULL, "../z_player.c", 9099);
    }
}

void func_8083AF44(GlobalContext* globalCtx, Player* this, s32 magicSpell) {
    func_80835DE4(globalCtx, this, func_808507F4, 0);

    this->unk_84F = magicSpell - 3;
    func_80087708(globalCtx, sMagicSpellCosts[magicSpell], 4);

    LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, &gPlayerAnim_002D28, 0.83f);

    if (magicSpell == 5) {
        this->unk_46C = OnePointCutscene_Init(globalCtx, 1100, -101, NULL, MAIN_CAM);
    }
    else {
        func_80835EA4(globalCtx, 10);
    }
}

void func_8083B010(Player* this) {
    this->actor.focus.rot.x = this->actor.focus.rot.z = this->unk_6B6 = this->unk_6B8 = this->unk_6BA = this->unk_6BC =
        this->unk_6BE = this->unk_6C0 = 0;

    this->actor.focus.rot.y = this->actor.shape.rot.y;
}

static u8 D_80854528[] = {
    GI_LETTER_ZELDA, GI_WEIRD_EGG,    GI_CHICKEN,     GI_BEAN,        GI_POCKET_EGG,   GI_POCKET_CUCCO,
    GI_COJIRO,       GI_ODD_MUSHROOM, GI_ODD_POTION,  GI_SAW,         GI_SWORD_BROKEN, GI_PRESCRIPTION,
    GI_FROG,         GI_EYEDROPS,     GI_CLAIM_CHECK, GI_MASK_SKULL,  GI_MASK_SPOOKY,  GI_MASK_KEATON,
    GI_MASK_BUNNY,   GI_MASK_TRUTH,   GI_MASK_GORON,  GI_MASK_ZORA,   GI_MASK_GERUDO,  GI_LETTER_RUTO,
    GI_LETTER_RUTO,  GI_LETTER_RUTO,  GI_LETTER_RUTO, GI_LETTER_RUTO, GI_LETTER_RUTO,
};

static LinkAnimationHeader* D_80854548[] = {
    &gPlayerAnim_002F88,
    &gPlayerAnim_002690,
    &gPlayerAnim_003198,
};

s32 func_8083B040(Player* this, GlobalContext* globalCtx) {
    s32 sp2C;
    s32 sp28;
    GetItemEntry* giEntry;
    Actor* targetActor;

    if ((this->unk_6AD != 0) &&
        (func_808332B8(this) || (this->actor.bgCheckFlags & 1) || (this->stateFlags1 & PLAYER_STATE1_23))) {

        if (!func_8083ADD4(globalCtx, this)) {
            if (this->unk_6AD == 4) {
                sp2C = Player_ActionToMagicSpell(this, this->itemActionParam);
                if (sp2C >= 0) {
                    if ((sp2C != 3) || (gSaveContext.respawn[RESPAWN_MODE_TOP].data <= 0)) {
                        func_8083AF44(globalCtx, this, sp2C);
                    }
                    else {
                        func_80835C58(globalCtx, this, func_8085063C, 1);
                        this->stateFlags1 |= PLAYER_STATE1_28 | PLAYER_STATE1_29;
                        func_80832264(globalCtx, this, func_80833338(this));
                        func_80835EA4(globalCtx, 4);
                    }

                    func_80832224(this);
                    return 1;
                }

                sp2C = this->itemActionParam - PLAYER_AP_LETTER_ZELDA;
                if ((sp2C >= 0) ||
                    (sp28 = Player_ActionToBottle(this, this->itemActionParam) - 1,
                        ((sp28 >= 0) && (sp28 < 6) &&
                            ((this->itemActionParam > PLAYER_AP_BOTTLE_POE) ||
                                ((this->targetActor != NULL) &&
                                    (((this->itemActionParam == PLAYER_AP_BOTTLE_POE) && (this->exchangeItemId == EXCH_ITEM_POE)) ||
                                        (this->exchangeItemId == EXCH_ITEM_BLUE_FIRE))))))) {

                    if ((globalCtx->actorCtx.titleCtx.delayTimer == 0) && (globalCtx->actorCtx.titleCtx.alpha == 0)) {
                        func_80835DE4(globalCtx, this, func_8084F104, 0);

                        if (sp2C >= 0) {
                            giEntry = &sGetItemTable[D_80854528[sp2C] - 1];
                            func_8083AE40(this, giEntry->objectId);
                        }

                        this->stateFlags1 |= PLAYER_STATE1_6 | PLAYER_STATE1_28 | PLAYER_STATE1_29;

                        if (sp2C >= 0) {
                            sp2C = sp2C + 1;
                        }
                        else {
                            sp2C = sp28 + 0x18;
                        }

                        targetActor = this->targetActor;

                        if ((targetActor != NULL) &&
                            ((this->exchangeItemId == sp2C) || (this->exchangeItemId == EXCH_ITEM_BLUE_FIRE) ||
                                ((this->exchangeItemId == EXCH_ITEM_POE) &&
                                    (this->itemActionParam == PLAYER_AP_BOTTLE_BIG_POE)) ||
                                ((this->exchangeItemId == EXCH_ITEM_BEAN) &&
                                    (this->itemActionParam == PLAYER_AP_BOTTLE_BUG))) &&
                            ((this->exchangeItemId != EXCH_ITEM_BEAN) || (this->itemActionParam == PLAYER_AP_BEAN))) {
                            if (this->exchangeItemId == EXCH_ITEM_BEAN) {
                                Inventory_ChangeAmmo(ITEM_BEAN, -1);
                                func_80835DE4(globalCtx, this, func_8084279C, 0);
                                this->stateFlags1 |= PLAYER_STATE1_29;
                                this->unk_850 = 0x50;
                                this->unk_84F = -1;
                            }
                            targetActor->flags |= ACTOR_FLAG_8;
                            this->unk_664 = this->targetActor;
                        }
                        else if (sp2C == EXCH_ITEM_LETTER_RUTO) {
                            this->unk_84F = 1;
                            this->actor.textId = 0x4005;
                            func_80835EA4(globalCtx, 1);
                        }
                        else {
                            this->unk_84F = 2;
                            this->actor.textId = 0xCF;
                            func_80835EA4(globalCtx, 4);
                        }

                        this->actor.flags |= ACTOR_FLAG_8;
                        this->exchangeItemId = sp2C;

                        if (this->unk_84F < 0) {
                            func_80832B0C(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_32][this->modelAnimType]);
                        }
                        else {
                            func_80832264(globalCtx, this, D_80854548[this->unk_84F]);
                        }

                        func_80832224(this);
                    }
                    return 1;
                }

                sp2C = Player_ActionToBottle(this, this->itemActionParam);
                if (sp2C >= 0) {
                    if (sp2C == 0xC) {
                        func_80835DE4(globalCtx, this, func_8084EED8, 0);
                        func_808322D0(globalCtx, this, &gPlayerAnim_002650);
                        func_80835EA4(globalCtx, 3);
                    }
                    else if ((sp2C > 0) && (sp2C < 4)) {
                        func_80835DE4(globalCtx, this, func_8084EFC0, 0);
                        func_808322D0(globalCtx, this, &gPlayerAnim_002688);
                        func_80835EA4(globalCtx, (sp2C == 1) ? 1 : 5);
                    }
                    else {
                        func_80835DE4(globalCtx, this, func_8084EAC0, 0);
                        func_80832B78(globalCtx, this, &gPlayerAnim_002668);
                        func_80835EA4(globalCtx, 2);
                    }
                }
                else {
                    func_80835DE4(globalCtx, this, func_8084E3C4, 0);
                    func_808322D0(globalCtx, this, &gPlayerAnim_0030A0);
                    this->stateFlags2 |= PLAYER_STATE2_27;
                    func_80835EA4(globalCtx, (this->unk_6A8 != NULL) ? 0x5B : 0x5A);
                    if (this->unk_6A8 != NULL) {
                        this->stateFlags2 |= PLAYER_STATE2_25;
                        Camera_SetParam(Gameplay_GetCamera(globalCtx, 0), 8, this->unk_6A8);
                    }
                }
            }
            else if (func_8083AD4C(globalCtx, this)) {
                if (!(this->stateFlags1 & PLAYER_STATE1_23)) {
                    func_80835C58(globalCtx, this, func_8084B1D8, 1);
                    this->unk_850 = 13;
                    func_8083B010(this);
                }
                this->stateFlags1 |= PLAYER_STATE1_20;
                func_80078884(NA_SE_SY_CAMERA_ZOOM_UP);
                func_80832210(this);
                return 1;
            }
            else {
                this->unk_6AD = 0;
                func_80078884(NA_SE_SY_ERROR);
                return 0;
            }

            this->stateFlags1 |= PLAYER_STATE1_28 | PLAYER_STATE1_29;
        }

        func_80832224(this);
        return 1;
    }

    return 0;
}

s32 func_8083B644(Player* this, GlobalContext* globalCtx) {
    Actor* sp34 = this->targetActor;
    Actor* sp30 = this->unk_664;
    Actor* sp2C = NULL;
    s32 sp28 = 0;
    s32 sp24;

    sp24 = (sp30 != NULL) && (CHECK_FLAG_ALL(sp30->flags, ACTOR_FLAG_0 | ACTOR_FLAG_18) || (sp30->naviEnemyId != 0xFF));

    if (sp24 || (this->naviTextId != 0)) {
        sp28 = (this->naviTextId < 0) && ((ABS(this->naviTextId) & 0xFF00) != 0x200);
        if (sp28 || !sp24) {
            sp2C = this->naviActor;
            if (sp28) {
                sp30 = NULL;
                sp34 = NULL;
            }
        }
        else {
            sp2C = sp30;
        }
    }

    if ((sp34 != NULL) || (sp2C != NULL)) {
        if ((sp30 == NULL) || (sp30 == sp34) || (sp30 == sp2C)) {
            if (!(this->stateFlags1 & PLAYER_STATE1_11) ||
                ((this->heldActor != NULL) && (sp28 || (sp34 == this->heldActor) || (sp2C == this->heldActor) ||
                    ((sp34 != NULL) && (sp34->flags & ACTOR_FLAG_16))))) {
                if ((this->actor.bgCheckFlags & 1) || (this->stateFlags1 & PLAYER_STATE1_23) ||
                    (func_808332B8(this) && !(this->stateFlags2 & PLAYER_STATE2_10))) {

                    if (sp34 != NULL) {
                        this->stateFlags2 |= PLAYER_STATE2_1;
                        if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) || (sp34->flags & ACTOR_FLAG_16)) {
                            sp2C = NULL;
                        }
                        else if (sp2C == NULL) {
                            return 0;
                        }
                    }

                    if (sp2C != NULL) {
                        if (!sp28) {
                            this->stateFlags2 |= PLAYER_STATE2_21;
                        }

                        if (!CHECK_BTN_ALL(sControlInput->press.button, BTN_CUP) && !sp28) {
                            return 0;
                        }

                        sp34 = sp2C;
                        this->targetActor = NULL;

                        if (sp28 || !sp24) {
                            if (this->naviTextId >= 0) {
                                sp2C->textId = this->naviTextId;
                            }
                            else {
                                sp2C->textId = -this->naviTextId;
                            }
                        }
                        else {
                            if (sp2C->naviEnemyId != 0xFF) {
                                sp2C->textId = sp2C->naviEnemyId + 0x600;
                            }
                        }
                    }

                    this->currentMask = D_80858AA4;
                    func_80853148(globalCtx, sp34);
                    return 1;
                }
            }
        }
    }

    return 0;
}

s32 func_8083B8F4(Player* this, GlobalContext* globalCtx) {
    if (!(this->stateFlags1 & (PLAYER_STATE1_11 | PLAYER_STATE1_23)) &&
        Camera_CheckValidMode(Gameplay_GetCamera(globalCtx, 0), 6)) {
        if ((this->actor.bgCheckFlags & 1) ||
            (func_808332B8(this) && (this->actor.yDistToWater < this->ageProperties->unk_2C))) {
            this->unk_6AD = 1;
            return 1;
        }
    }

    return 0;
}

s32 func_8083B998(Player* this, GlobalContext* globalCtx) {
    if (this->unk_6AD != 0) {
        func_8083B040(this, globalCtx);
        return 1;
    }

    if ((this->unk_664 != NULL) &&
        (CHECK_FLAG_ALL(this->unk_664->flags, ACTOR_FLAG_0 | ACTOR_FLAG_18) || (this->unk_664->naviEnemyId != 0xFF))) {
        this->stateFlags2 |= PLAYER_STATE2_21;
    }
    else if ((this->naviTextId == 0) && !func_8008E9C4(this) && CHECK_BTN_ALL(sControlInput->press.button, BTN_CUP) &&
        (YREG(15) != 0x10) && (YREG(15) != 0x20) && !func_8083B8F4(this, globalCtx)) {
        func_80078884(NA_SE_SY_ERROR);
    }

    return 0;
}

void func_8083BA90(GlobalContext* globalCtx, Player* this, s32 arg2, f32 xzVelocity, f32 yVelocity) {
    func_80837948(globalCtx, this, arg2);
    func_80835C58(globalCtx, this, func_80844AF4, 0);

    this->stateFlags3 |= PLAYER_STATE3_1;

    this->currentYaw = this->actor.shape.rot.y;
    this->linearVelocity = xzVelocity;
    this->actor.velocity.y = yVelocity;

    this->actor.bgCheckFlags &= ~1;
    this->hoverBootsTimer = 0;

    func_80832854(this);
    func_80832698(this, NA_SE_VO_LI_SWORD_L);
}

s32 func_8083BB20(Player* this) {
    if (!(this->stateFlags1 & PLAYER_STATE1_22) && (Player_GetSwordHeld(this) != 0)) {
        if (D_80853614 ||
            ((this->actor.category != ACTORCAT_PLAYER) && CHECK_BTN_ALL(sControlInput->press.button, BTN_B))) {
            return 1;
        }
    }

    return 0;
}

s32 func_8083BBA0(Player* this, GlobalContext* globalCtx) {
    if (func_8083BB20(this) && (D_808535E4 != 7)) {
        func_8083BA90(globalCtx, this, 17, 3.0f, 4.5f);
        return 1;
    }

    return 0;
}

void func_8083BC04(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_80844708, 0);
    LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_16][this->modelAnimType],
        1.25f * D_808535E8);
}

s32 func_8083BC7C(Player* this, GlobalContext* globalCtx) {
    if ((this->unk_84B[this->unk_846] == 0) && (D_808535E4 != 7)) {
        func_8083BC04(this, globalCtx);
        return 1;
    }

    return 0;
}

void func_8083BCD0(Player* this, GlobalContext* globalCtx, s32 arg2) {
    func_80838940(this, D_80853D4C[arg2][0], !(arg2 & 1) ? 5.8f : 3.5f, globalCtx, NA_SE_VO_LI_SWORD_N);

    if (arg2) {}

    this->unk_850 = 1;
    this->unk_84F = arg2;

    this->currentYaw = this->actor.shape.rot.y + (arg2 << 0xE);
    this->linearVelocity = !(arg2 & 1) ? 6.0f : 8.5f;

    this->stateFlags2 |= PLAYER_STATE2_19;

    func_8002F7DC(&this->actor, ((arg2 << 0xE) == 0x8000) ? NA_SE_PL_ROLL : NA_SE_PL_SKIP);
}

s32 func_8083BDBC(Player* this, GlobalContext* globalCtx) {
    s32 sp2C;

    if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) && (globalCtx->roomCtx.curRoom.unk_03 != 2) &&
        (D_808535E4 != 7) &&
        (SurfaceType_GetSlope(&globalCtx->colCtx, this->actor.floorPoly, this->actor.floorBgId) != 1)) {
        sp2C = this->unk_84B[this->unk_846];

        if (sp2C <= 0) {
            if (func_80833BCC(this)) {
                if (this->actor.category != ACTORCAT_PLAYER) {
                    if (sp2C < 0) {
                        func_808389E8(this, &gPlayerAnim_002FE0, REG(69) / 100.0f, globalCtx);
                    }
                    else {
                        func_8083BC04(this, globalCtx);
                    }
                }
                else {
                    if (Player_GetSwordHeld(this) && func_808365C8(this)) {
                        func_8083BA90(globalCtx, this, 17, 5.0f, 5.0f);
                    }
                    else {
                        func_8083BC04(this, globalCtx);
                    }
                }
                return 1;
            }
        }
        else {
            func_8083BCD0(this, globalCtx, sp2C);
            return 1;
        }
    }

    return 0;
}

void func_8083BF50(Player* this, GlobalContext* globalCtx) {
    LinkAnimationHeader* anim;
    f32 sp30;

    sp30 = this->unk_868 - 3.0f;
    if (sp30 < 0.0f) {
        sp30 += 29.0f;
    }

    if (sp30 < 14.0f) {
        anim = D_80853914[PLAYER_ANIMGROUP_18][this->modelAnimType];
        sp30 = 11.0f - sp30;
        if (sp30 < 0.0f) {
            sp30 = 1.375f * -sp30;
        }
        sp30 /= 11.0f;
    }
    else {
        anim = D_80853914[PLAYER_ANIMGROUP_19][this->modelAnimType];
        sp30 = 26.0f - sp30;
        if (sp30 < 0.0f) {
            sp30 = 2 * -sp30;
        }
        sp30 /= 12.0f;
    }

    LinkAnimation_Change(globalCtx, &this->skelAnime, anim, 1.0f, 0.0f, Animation_GetLastFrame(anim), ANIMMODE_ONCE,
        4.0f * sp30);
    this->currentYaw = this->actor.shape.rot.y;
}

void func_8083C0B8(Player* this, GlobalContext* globalCtx) {
    func_80839FFC(this, globalCtx);
    func_8083BF50(this, globalCtx);
}

void func_8083C0E8(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_80840BC8, 1);
    func_80832264(globalCtx, this, func_80833338(this));
    this->currentYaw = this->actor.shape.rot.y;
}

void func_8083C148(Player* this, GlobalContext* globalCtx) {
    if (!(this->stateFlags3 & PLAYER_STATE3_7)) {
        func_8083B010(this);
        if (this->stateFlags1 & PLAYER_STATE1_27) {
            func_80838F18(globalCtx, this);
        }
        else {
            func_80839F90(this, globalCtx);
        }
        if (this->unk_6AD < 4) {
            this->unk_6AD = 0;
        }
    }

    this->stateFlags1 &= ~(PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_20);
}

s32 func_8083C1DC(Player* this, GlobalContext* globalCtx) {
    if (!func_80833B54(this) && (D_808535E0 == 0) && !(this->stateFlags1 & PLAYER_STATE1_23) &&
        CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
        if (func_8083BC7C(this, globalCtx)) {
            return 1;
        }
        if ((this->unk_837 == 0) && (this->heldItemActionParam >= PLAYER_AP_SWORD_MASTER)) {
            func_80835F44(globalCtx, this, ITEM_NONE);
        }
        else {
            this->stateFlags2 ^= PLAYER_STATE2_20;
        }
    }

    return 0;
}

s32 func_8083C2B0(Player* this, GlobalContext* globalCtx) {
    LinkAnimationHeader* anim;
    f32 frame;

    if ((globalCtx->shootingGalleryStatus == 0) && (this->currentShield != PLAYER_SHIELD_NONE) &&
        CHECK_BTN_ALL(sControlInput->cur.button, BTN_R) &&
        (Player_IsChildWithHylianShield(this) || (!func_80833B2C(this) && (this->unk_664 == NULL)))) {

        func_80832318(this);
        func_808323B4(globalCtx, this);

        if (func_80835C58(globalCtx, this, func_80843188, 0)) {
            this->stateFlags1 |= PLAYER_STATE1_22;

            if (!Player_IsChildWithHylianShield(this)) {
                Player_SetModelsForHoldingShield(this);
                anim = D_80853914[PLAYER_ANIMGROUP_20][this->modelAnimType];
            }
            else {
                anim = &gPlayerAnim_002400;
            }

            if (anim != this->skelAnime.animation) {
                if (func_8008E9C4(this)) {
                    this->unk_86C = 1.0f;
                }
                else {
                    this->unk_86C = 0.0f;
                    func_80833C3C(this);
                }
                this->unk_6BC = this->unk_6BE = this->unk_6C0 = 0;
            }

            frame = Animation_GetLastFrame(anim);
            LinkAnimation_Change(globalCtx, &this->skelAnime, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);

            if (Player_IsChildWithHylianShield(this)) {
                func_80832F54(globalCtx, this, 4);
            }

            func_8002F7DC(&this->actor, NA_SE_IT_SHIELD_POSTURE);
        }

        return 1;
    }

    return 0;
}

s32 func_8083C484(Player* this, f32* arg1, s16* arg2) {
    s16 yaw = this->currentYaw - *arg2;

    if (ABS(yaw) > 0x6000) {
        if (func_8083721C(this)) {
            *arg1 = 0.0f;
            *arg2 = this->currentYaw;
        }
        else {
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

s32 func_8083C544(Player* this, GlobalContext* globalCtx) {
    if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_22) && (Player_GetSwordHeld(this) != 0) && (this->unk_844 == 1) &&
            (this->heldItemActionParam != PLAYER_AP_STICK)) {
            if ((this->heldItemActionParam != PLAYER_AP_SWORD_BGS) || (gSaveContext.swordHealth > 0.0f)) {
                func_808377DC(globalCtx, this);
                return 1;
            }
        }
    }
    else {
        func_8083C50C(this);
    }

    return 0;
}

s32 func_8083C61C(GlobalContext* globalCtx, Player* this) {
    if ((globalCtx->roomCtx.curRoom.unk_03 != 2) && (this->actor.bgCheckFlags & 1) && (AMMO(ITEM_NUT) != 0)) {
        func_80835C58(globalCtx, this, func_8084E604, 0);
        func_80832264(globalCtx, this, &gPlayerAnim_003048);
        this->unk_6AD = 0;
        return 1;
    }

    return 0;
}

static struct_80854554 D_80854554[] = {
    { &gPlayerAnim_002648, &gPlayerAnim_002640, 2, 3 },
    { &gPlayerAnim_002680, &gPlayerAnim_002678, 5, 3 },
};

s32 func_8083C6B8(GlobalContext* globalCtx, Player* this) {
    Vec3f sp24;

    if (D_80853614) {
        if (Player_GetBottleHeld(this) >= 0) {
            func_80835C58(globalCtx, this, func_8084ECA4, 0);

            if (this->actor.yDistToWater > 12.0f) {
                this->unk_850 = 1;
            }

            func_808322D0(globalCtx, this, D_80854554[this->unk_850].unk_00);

            func_8002F7DC(&this->actor, NA_SE_IT_SWORD_SWING);
            func_80832698(this, NA_SE_VO_LI_AUTO_JUMP);
            return 1;
        }

        if (this->heldItemActionParam == PLAYER_AP_FISHING_POLE) {
            sp24 = this->actor.world.pos;
            sp24.y += 50.0f;

            if (!(this->actor.bgCheckFlags & 1) || (this->actor.world.pos.z > 1300.0f) ||
                BgCheck_SphVsFirstPoly(&globalCtx->colCtx, &sp24, 20.0f)) {
                func_80078884(NA_SE_SY_ERROR);
                return 0;
            }

            func_80835C58(globalCtx, this, func_80850C68, 0);
            this->unk_860 = 1;
            func_80832210(this);
            func_80832264(globalCtx, this, &gPlayerAnim_002C30);
            return 1;
        }
        else {
            return 0;
        }
    }

    return 0;
}

void func_8083C858(Player* this, GlobalContext* globalCtx) {
    PlayerFunc674 func;

    if (func_80833BCC(this)) {
        func = func_8084227C;
    }
    else {
        func = func_80842180;
    }

    func_80835C58(globalCtx, this, func, 1);
    func_80832BE8(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_2][this->modelAnimType]);

    this->unk_89C = 0;
    this->unk_864 = this->unk_868 = 0.0f;
}

void func_8083C8DC(Player* this, GlobalContext* globalCtx, s16 arg2) {
    this->actor.shape.rot.y = this->currentYaw = arg2;
    func_8083C858(this, globalCtx);
}

s32 func_8083C910(GlobalContext* globalCtx, Player* this, f32 arg2) {
    WaterBox* sp2C;
    f32 sp28;

    sp28 = this->actor.world.pos.y;
    if (WaterBox_GetSurface1(globalCtx, &globalCtx->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp28,
        &sp2C) != 0) {
        sp28 -= this->actor.world.pos.y;
        if (this->ageProperties->unk_24 <= sp28) {
            func_80835C58(globalCtx, this, func_8084D7C4, 0);
            func_80832C6C(globalCtx, this, &gPlayerAnim_0032F0);
            this->stateFlags1 |= PLAYER_STATE1_27 | PLAYER_STATE1_29;
            this->unk_850 = 20;
            this->linearVelocity = 2.0f;
            Player_SetBootData(globalCtx, this);
            return 0;
        }
    }

    func_80838E70(globalCtx, this, arg2, this->actor.shape.rot.y);
    this->stateFlags1 |= PLAYER_STATE1_29;
    return 1;
}

void func_8083CA20(GlobalContext* globalCtx, Player* this) {
    if (func_8083C910(globalCtx, this, 180.0f)) {
        this->unk_850 = -20;
    }
}

void func_8083CA54(GlobalContext* globalCtx, Player* this) {
    this->linearVelocity = 2.0f;
    gSaveContext.entranceSpeed = 2.0f;
    if (func_8083C910(globalCtx, this, 120.0f)) {
        this->unk_850 = -15;
    }
}

void func_8083CA9C(GlobalContext* globalCtx, Player* this) {
    if (gSaveContext.entranceSpeed < 0.1f) {
        gSaveContext.entranceSpeed = 0.1f;
    }

    this->linearVelocity = gSaveContext.entranceSpeed;

    if (func_8083C910(globalCtx, this, 800.0f)) {
        this->unk_850 = -80 / this->linearVelocity;
        if (this->unk_850 < -20) {
            this->unk_850 = -20;
        }
    }
}

void func_8083CB2C(Player* this, s16 yaw, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_808414F8, 1);
    LinkAnimation_CopyJointToMorph(globalCtx, &this->skelAnime);
    this->unk_864 = this->unk_868 = 0.0f;
    this->currentYaw = yaw;
}

void func_8083CB94(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_80840DE4, 1);
    func_80832BE8(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_1][this->modelAnimType]);
}

void func_8083CBF0(Player* this, s16 yaw, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_808423EC, 1);
    LinkAnimation_Change(globalCtx, &this->skelAnime, &gPlayerAnim_0024F8, 2.2f, 0.0f,
        Animation_GetLastFrame(&gPlayerAnim_0024F8), ANIMMODE_ONCE, -6.0f);
    this->linearVelocity = 8.0f;
    this->currentYaw = yaw;
}

void func_8083CC9C(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_8084193C, 1);
    func_80832BE8(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_25][this->modelAnimType]);
    this->unk_868 = 0.0f;
}

void func_8083CD00(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_8084251C, 1);
    LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, &gPlayerAnim_0024E8, 2.0f);
}

void func_8083CD54(GlobalContext* globalCtx, Player* this, s16 yaw) {
    this->currentYaw = yaw;
    func_80835C58(globalCtx, this, func_80841BA8, 1);
    this->unk_87E = 1200;
    this->unk_87E *= D_808535E8;
    LinkAnimation_Change(globalCtx, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_26][this->modelAnimType], 1.0f, 0.0f,
        0.0f, ANIMMODE_LOOP, -6.0f);
}

void func_8083CE0C(Player* this, GlobalContext* globalCtx) {
    LinkAnimationHeader* anim;

    func_80835C58(globalCtx, this, func_80840BC8, 1);

    if (this->unk_870 < 0.5f) {
        anim = D_80853914[PLAYER_ANIMGROUP_28][this->modelAnimType];
    }
    else {
        anim = D_80853914[PLAYER_ANIMGROUP_27][this->modelAnimType];
    }
    func_80832264(globalCtx, this, anim);

    this->currentYaw = this->actor.shape.rot.y;
}

void func_8083CEAC(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_80840450, 1);
    func_80832B0C(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_7][this->modelAnimType]);
    this->unk_850 = 1;
}

void func_8083CF10(Player* this, GlobalContext* globalCtx) {
    if (this->linearVelocity != 0.0f) {
        func_8083C858(this, globalCtx);
    }
    else {
        func_8083CE0C(this, globalCtx);
    }
}

void func_8083CF5C(Player* this, GlobalContext* globalCtx) {
    if (this->linearVelocity != 0.0f) {
        func_8083C858(this, globalCtx);
    }
    else {
        func_80839F90(this, globalCtx);
    }
}

s32 func_8083CFA8(GlobalContext* globalCtx, Player* this, f32 arg2, s32 splashScale) {
    f32 sp3C = fabsf(arg2);
    WaterBox* sp38;
    f32 sp34;
    Vec3f splashPos;
    s32 splashType;

    if (sp3C > 2.0f) {
        splashPos.x = this->bodyPartsPos[PLAYER_BODYPART_WAIST].x;
        splashPos.z = this->bodyPartsPos[PLAYER_BODYPART_WAIST].z;
        sp34 = this->actor.world.pos.y;
        if (WaterBox_GetSurface1(globalCtx, &globalCtx->colCtx, splashPos.x, splashPos.z, &sp34, &sp38)) {
            if ((sp34 - this->actor.world.pos.y) < 100.0f) {
                splashType = (sp3C <= 10.0f) ? 0 : 1;
                splashPos.y = sp34;
                EffectSsGSplash_Spawn(globalCtx, &splashPos, NULL, NULL, splashType, splashScale);
                return 1;
            }
        }
    }

    return 0;
}

void func_8083D0A8(GlobalContext* globalCtx, Player* this, f32 arg2) {
    this->stateFlags1 |= PLAYER_STATE1_18;
    this->stateFlags1 &= ~PLAYER_STATE1_27;

    func_80832340(globalCtx, this);
    if (func_8083CFA8(globalCtx, this, arg2, 500)) {
        func_8002F7DC(&this->actor, NA_SE_EV_JUMP_OUT_WATER);
    }

    Player_SetBootData(globalCtx, this);
}

s32 func_8083D12C(GlobalContext* globalCtx, Player* this, Input* arg2) {
    if (!(this->stateFlags1 & PLAYER_STATE1_10) && !(this->stateFlags2 & PLAYER_STATE2_10)) {
        if ((arg2 == NULL) || (CHECK_BTN_ALL(arg2->press.button, BTN_A) && (ABS(this->unk_6C2) < 12000) &&
            (this->currentBoots != PLAYER_BOOTS_IRON))) {

            func_80835C58(globalCtx, this, func_8084DC48, 0);
            func_80832264(globalCtx, this, &gPlayerAnim_003308);

            this->unk_6C2 = 0;
            this->stateFlags2 |= PLAYER_STATE2_10;
            this->actor.velocity.y = 0.0f;

            if (arg2 != NULL) {
                this->stateFlags2 |= PLAYER_STATE2_11;
                func_8002F7DC(&this->actor, NA_SE_PL_DIVE_BUBBLE);
            }

            return 1;
        }
    }

    if ((this->stateFlags1 & PLAYER_STATE1_10) || (this->stateFlags2 & PLAYER_STATE2_10)) {
        if (this->actor.velocity.y > 0.0f) {
            if (this->actor.yDistToWater < this->ageProperties->unk_30) {

                this->stateFlags2 &= ~PLAYER_STATE2_10;

                if (arg2 != NULL) {
                    func_80835C58(globalCtx, this, func_8084E1EC, 1);

                    if (this->stateFlags1 & PLAYER_STATE1_10) {
                        this->stateFlags1 |= PLAYER_STATE1_10 | PLAYER_STATE1_11 | PLAYER_STATE1_29;
                    }

                    this->unk_850 = 2;
                }

                func_80832340(globalCtx, this);
                func_80832B0C(globalCtx, this,
                    (this->stateFlags1 & PLAYER_STATE1_11) ? &gPlayerAnim_003318 : &gPlayerAnim_003300);

                if (func_8083CFA8(globalCtx, this, this->actor.velocity.y, 500)) {
                    func_8002F7DC(&this->actor, NA_SE_PL_FACE_UP);
                }

                return 1;
            }
        }
    }

    return 0;
}

void func_8083D330(GlobalContext* globalCtx, Player* this) {
    func_80832284(globalCtx, this, &gPlayerAnim_0032F0);
    this->unk_6C2 = 16000;
    this->unk_850 = 1;
}

void func_8083D36C(GlobalContext* globalCtx, Player* this) {
    if ((this->currentBoots != PLAYER_BOOTS_IRON) || !(this->actor.bgCheckFlags & 1)) {
        func_80832564(globalCtx, this);

        if ((this->currentBoots != PLAYER_BOOTS_IRON) && (this->stateFlags2 & PLAYER_STATE2_10)) {
            this->stateFlags2 &= ~PLAYER_STATE2_10;
            func_8083D12C(globalCtx, this, 0);
            this->unk_84F = 1;
        }
        else if (func_80844A44 == this->func_674) {
            func_80835C58(globalCtx, this, func_8084DC48, 0);
            func_8083D330(globalCtx, this);
        }
        else {
            func_80835C58(globalCtx, this, func_8084D610, 1);
            func_80832B0C(globalCtx, this, (this->actor.bgCheckFlags & 1) ? &gPlayerAnim_003330 : &gPlayerAnim_0032E0);
        }
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_27) || (this->actor.yDistToWater < this->ageProperties->unk_2C)) {
        if (func_8083CFA8(globalCtx, this, this->actor.velocity.y, 500)) {
            func_8002F7DC(&this->actor, NA_SE_EV_DIVE_INTO_WATER);

            if (this->fallDistance > 800.0f) {
                func_80832698(this, NA_SE_VO_LI_CLIMB_END);
            }
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_27;
    this->stateFlags2 |= PLAYER_STATE2_10;
    this->stateFlags1 &= ~(PLAYER_STATE1_18 | PLAYER_STATE1_19);
    this->unk_854 = 0.0f;

    Player_SetBootData(globalCtx, this);
}

void func_8083D53C(GlobalContext* globalCtx, Player* this) {
    if (this->actor.yDistToWater < this->ageProperties->unk_2C) {
        Audio_SetBaseFilter(0);
        this->unk_840 = 0;
    }
    else {
        Audio_SetBaseFilter(0x20);
        if (this->unk_840 < 300) {
            this->unk_840++;
        }
    }

    if ((func_80845668 != this->func_674) && (func_8084BDFC != this->func_674)) {
        if (this->ageProperties->unk_2C < this->actor.yDistToWater) {
            if (!(this->stateFlags1 & PLAYER_STATE1_27) ||
                (!((this->currentBoots == PLAYER_BOOTS_IRON) && (this->actor.bgCheckFlags & 1)) &&
                    (func_8084E30C != this->func_674) && (func_8084E368 != this->func_674) &&
                    (func_8084D610 != this->func_674) && (func_8084D84C != this->func_674) &&
                    (func_8084DAB4 != this->func_674) && (func_8084DC48 != this->func_674) &&
                    (func_8084E1EC != this->func_674) && (func_8084D7C4 != this->func_674))) {
                func_8083D36C(globalCtx, this);
                return;
            }
        }
        else if ((this->stateFlags1 & PLAYER_STATE1_27) && (this->actor.yDistToWater < this->ageProperties->unk_24)) {
            if ((this->skelAnime.moveFlags == 0) && (this->currentBoots != PLAYER_BOOTS_IRON)) {
                func_8083CD54(globalCtx, this, this->actor.shape.rot.y);
            }
            func_8083D0A8(globalCtx, this, this->actor.velocity.y);
        }
    }
}

void func_8083D6EC(GlobalContext* globalCtx, Player* this) {
    Vec3f ripplePos;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;

    this->actor.minVelocityY = -20.0f;
    this->actor.gravity = REG(68) / 100.0f;

    if (func_8083816C(D_808535E4)) {
        temp1 = fabsf(this->linearVelocity) * 20.0f;
        temp3 = 0.0f;

        if (D_808535E4 == 4) {
            if (this->unk_6C4 > 1300.0f) {
                temp2 = this->unk_6C4;
            }
            else {
                temp2 = 1300.0f;
            }
            if (this->currentBoots == PLAYER_BOOTS_HOVER) {
                temp1 += temp1;
            }
            else if (this->currentBoots == PLAYER_BOOTS_IRON) {
                temp1 *= 0.3f;
            }
        }
        else {
            temp2 = 20000.0f;
            if (this->currentBoots != PLAYER_BOOTS_HOVER) {
                temp1 += temp1;
            }
            else if ((D_808535E4 == 7) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
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
    }
    else {
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
                EffectSsGRipple_Spawn(globalCtx, &ripplePos, 100, 500, 0);

                if ((this->linearVelocity > 4.0f) && !func_808332B8(this) &&
                    ((this->actor.world.pos.y + this->actor.yDistToWater) <
                        this->bodyPartsPos[PLAYER_BODYPART_WAIST].y)) {
                    func_8083CFA8(globalCtx, this, 20.0f,
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
            }
            else {
                numBubbles = this->actor.velocity.y * -2.0f;
            }

            for (i = 0; i < numBubbles; i++) {
                EffectSsBubble_Spawn(globalCtx, &this->actor.world.pos, 20.0f, 10.0f, 20.0f, 0.13f);
            }
        }
    }
}

s32 func_8083DB98(Player* this, s32 arg1) {
    Actor* unk_664 = this->unk_664;
    Vec3f sp30;
    s16 sp2E;
    s16 sp2C;

    sp30.x = this->actor.world.pos.x;
    sp30.y = this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    sp30.z = this->actor.world.pos.z;
    sp2E = Math_Vec3f_Pitch(&sp30, &unk_664->focus.pos);
    sp2C = Math_Vec3f_Yaw(&sp30, &unk_664->focus.pos);
    Math_SmoothStepToS(&this->actor.focus.rot.y, sp2C, 4, 10000, 0);
    Math_SmoothStepToS(&this->actor.focus.rot.x, sp2E, 4, 10000, 0);
    this->unk_6AE |= 2;

    return func_80836AB8(this, arg1);
}

static Vec3f D_8085456C = { 0.0f, 100.0f, 40.0f };

void func_8083DC54(Player* this, GlobalContext* globalCtx) {
    s16 sp46;
    s16 temp2;
    f32 temp1;
    Vec3f sp34;

    if (this->unk_664 != NULL) {
        if (func_8002DD78(this) || func_808334B4(this)) {
            func_8083DB98(this, 1);
        }
        else {
            func_8083DB98(this, 0);
        }
        return;
    }

    if (D_808535E4 == 11) {
        Math_SmoothStepToS(&this->actor.focus.rot.x, -20000, 10, 4000, 800);
    }
    else {
        sp46 = 0;
        temp1 = func_8083973C(globalCtx, this, &D_8085456C, &sp34);
        if (temp1 > BGCHECK_Y_MIN) {
            temp2 = Math_Atan2S(40.0f, this->actor.world.pos.y - temp1);
            sp46 = CLAMP(temp2, -4000, 4000);
        }
        this->actor.focus.rot.y = this->actor.shape.rot.y;
        Math_SmoothStepToS(&this->actor.focus.rot.x, sp46, 14, 4000, 30);
    }

    func_80836AB8(this, func_8002DD78(this) || func_808334B4(this));
}

void func_8083DDC8(Player* this, GlobalContext* globalCtx) {
    s16 temp1;
    s16 temp2;

    if (!func_8002DD78(this) && !func_808334B4(this) && (this->linearVelocity > 5.0f)) {
        temp1 = this->linearVelocity * 200.0f;
        temp2 = (s16)(this->currentYaw - this->actor.shape.rot.y) * this->linearVelocity * 0.1f;
        temp1 = CLAMP(temp1, -4000, 4000);
        temp2 = CLAMP(-temp2, -4000, 4000);
        Math_ScaledStepToS(&this->unk_6BC, temp1, 900);
        this->unk_6B6 = -(f32)this->unk_6BC * 0.5f;
        Math_ScaledStepToS(&this->unk_6BA, temp2, 300);
        Math_ScaledStepToS(&this->unk_6C0, temp2, 200);
        this->unk_6AE |= 0x168;
    }
    else {
        func_8083DC54(this, globalCtx);
    }
}

void func_8083DF68(Player* this, f32 arg1, s16 arg2) {
    Math_AsymStepToF(&this->linearVelocity, arg1, REG(19) / 100.0f, 1.5f);
    Math_ScaledStepToS(&this->currentYaw, arg2, REG(27));
}

void func_8083DFE0(Player* this, f32* arg1, s16* arg2) {
    s16 yawDiff = this->currentYaw - *arg2;

    if (this->swordState == 0) {
        float maxSpeed = R_RUN_SPEED_LIMIT / 100.0f;
        if (CVar_GetS32("gMMBunnyHood", 0) != 0 && this->currentMask == PLAYER_MASK_BUNNY) {
            maxSpeed *= 1.5f;
        }
        this->linearVelocity = CLAMP(this->linearVelocity, -maxSpeed, maxSpeed);
    }

    if (ABS(yawDiff) > 0x6000) {
        if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
            this->currentYaw = *arg2;
        }
    }
    else {
        Math_AsymStepToF(&this->linearVelocity, *arg1, 0.05f, 0.1f);
        Math_ScaledStepToS(&this->currentYaw, *arg2, 200);
    }
}

static struct_80854578 D_80854578[] = {
    { &gPlayerAnim_003398, 35.17f, 6.6099997f },
    { &gPlayerAnim_0033A8, -34.16f, 7.91f },
};

s32 func_8083E0FC(Player* this, GlobalContext* globalCtx) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    f32 unk_04;
    f32 unk_08;
    f32 sp38;
    f32 sp34;
    s32 temp;

    if ((rideActor != NULL) && CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
        sp38 = Math_CosS(rideActor->actor.shape.rot.y);
        sp34 = Math_SinS(rideActor->actor.shape.rot.y);

        func_80836898(globalCtx, this, func_8083A360);

        this->stateFlags1 |= PLAYER_STATE1_23;
        this->actor.bgCheckFlags &= ~0x20;

        if (this->mountSide < 0) {
            temp = 0;
        }
        else {
            temp = 1;
        }

        unk_04 = D_80854578[temp].unk_04;
        unk_08 = D_80854578[temp].unk_08;
        this->actor.world.pos.x =
            rideActor->actor.world.pos.x + rideActor->riderPos.x + ((unk_04 * sp38) + (unk_08 * sp34));
        this->actor.world.pos.z =
            rideActor->actor.world.pos.z + rideActor->riderPos.z + ((unk_08 * sp38) - (unk_04 * sp34));

        this->unk_878 = rideActor->actor.world.pos.y - this->actor.world.pos.y;
        this->currentYaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

        Actor_MountHorse(globalCtx, this, &rideActor->actor);
        func_80832264(globalCtx, this, D_80854578[temp].anim);
        func_80832F54(globalCtx, this, 0x9B);
        this->actor.parent = this->rideActor;
        func_80832224(this);
        func_800304B0(globalCtx);
        return 1;
    }

    return 0;
}

void func_8083E298(CollisionPoly* arg0, Vec3f* arg1, s16* arg2) {
    arg1->x = COLPOLY_GET_NORMAL(arg0->normal.x);
    arg1->y = COLPOLY_GET_NORMAL(arg0->normal.y);
    arg1->z = COLPOLY_GET_NORMAL(arg0->normal.z);

    *arg2 = Math_Atan2S(arg1->z, arg1->x);
}

static LinkAnimationHeader* D_80854590[] = {
    &gPlayerAnim_002EE0,
    &gPlayerAnim_0031D0,
};

s32 func_8083E318(GlobalContext* globalCtx, Player* this, CollisionPoly* arg2) {
    s32 pad;
    s16 sp4A;
    Vec3f sp3C;
    s16 sp3A;
    f32 temp1;
    f32 temp2;
    s16 temp3;

    if (!Player_InBlockingCsMode(globalCtx, this) && (func_8084F390 != this->func_674) &&
        (SurfaceType_GetSlope(&globalCtx->colCtx, arg2, this->actor.floorBgId) == 1)) {
        sp4A = Math_Atan2S(this->actor.velocity.z, this->actor.velocity.x);
        func_8083E298(arg2, &sp3C, &sp3A);
        temp3 = sp3A - sp4A;

        if (ABS(temp3) > 16000) {
            temp1 = (1.0f - sp3C.y) * 40.0f;
            temp2 = (temp1 * temp1) * 0.015f;
            if (temp2 < 1.2f) {
                temp2 = 1.2f;
            }
            this->windDirection = sp3A;
            Math_StepToF(&this->windSpeed, temp1, temp2);
        }
        else {
            func_80835C58(globalCtx, this, func_8084F390, 0);
            func_80832564(globalCtx, this);
            if (D_80853610 >= 0) {
                this->unk_84F = 1;
            }
            func_80832BE8(globalCtx, this, D_80854590[this->unk_84F]);
            this->linearVelocity = sqrtf(SQ(this->actor.velocity.x) + SQ(this->actor.velocity.z));
            this->currentYaw = sp4A;
            return 1;
        }
    }

    return 0;
}

// unknown data (unused)
static s32 D_80854598[] = {
    0xFFDB0871, 0xF8310000, 0x00940470, 0xF3980000, 0xFFB504A9, 0x0C9F0000, 0x08010402,
};

void func_8083E4C4(GlobalContext* globalCtx, Player* this, GetItemEntry* giEntry) {
    s32 sp1C = giEntry->field & 0x1F;

    if (!(giEntry->field & 0x80)) {
        Item_DropCollectible(globalCtx, &this->actor.world.pos, sp1C | 0x8000);
        if ((sp1C != 4) && (sp1C != 8) && (sp1C != 9) && (sp1C != 0xA) && (sp1C != 0) && (sp1C != 1) && (sp1C != 2) &&
            (sp1C != 0x14) && (sp1C != 0x13)) {
            Item_Give(globalCtx, giEntry->itemId);
        }
    }
    else {
        Item_Give(globalCtx, giEntry->itemId);
    }

    func_80078884((this->getItemId < 0) ? NA_SE_SY_GET_BOXITEM : NA_SE_SY_GET_ITEM);
}

s32 func_8083E5A8(Player* this, GlobalContext* globalCtx) {
    Actor* interactedActor;

    if (iREG(67) || (((interactedActor = this->interactRangeActor) != NULL) &&
        func_8002D53C(globalCtx, &globalCtx->actorCtx.titleCtx))) {
        if (iREG(67) || (this->getItemId > GI_NONE)) {
            if (iREG(67)) {
                this->getItemId = iREG(68);
            }

            if (this->getItemId < GI_MAX) {
                GetItemEntry* giEntry = &sGetItemTable[this->getItemId - 1];

                if ((interactedActor != &this->actor) && !iREG(67)) {
                    interactedActor->parent = &this->actor;
                }

                iREG(67) = false;

                if ((Item_CheckObtainability(giEntry->itemId) == ITEM_NONE) || (globalCtx->sceneNum == SCENE_BOWLING)) {
                    func_808323B4(globalCtx, this);
                    func_8083AE40(this, giEntry->objectId);

                    if (!(this->stateFlags2 & PLAYER_STATE2_10) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
                        func_80836898(globalCtx, this, func_8083A434);
                        func_808322D0(globalCtx, this, &gPlayerAnim_002788);
                        func_80835EA4(globalCtx, 9);
                    }

                    this->stateFlags1 |= PLAYER_STATE1_10 | PLAYER_STATE1_11 | PLAYER_STATE1_29;
                    func_80832224(this);
                    return 1;
                }

                func_8083E4C4(globalCtx, this, giEntry);
                this->getItemId = GI_NONE;
            }
        }
        else if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) && !(this->stateFlags1 & PLAYER_STATE1_11) &&
            !(this->stateFlags2 & PLAYER_STATE2_10)) {
            if (this->getItemId != GI_NONE) {
                GetItemEntry* giEntry = &sGetItemTable[-this->getItemId - 1];
                EnBox* chest = (EnBox*)interactedActor;

                if (giEntry->itemId != ITEM_NONE) {
                    if (((Item_CheckObtainability(giEntry->itemId) == ITEM_NONE) && (giEntry->field & 0x40)) ||
                        ((Item_CheckObtainability(giEntry->itemId) != ITEM_NONE) && (giEntry->field & 0x20))) {
                        this->getItemId = -GI_RUPEE_BLUE;
                        giEntry = &sGetItemTable[GI_RUPEE_BLUE - 1];
                    }
                }

                func_80836898(globalCtx, this, func_8083A434);
                this->stateFlags1 |= PLAYER_STATE1_10 | PLAYER_STATE1_11 | PLAYER_STATE1_29;
                func_8083AE40(this, giEntry->objectId);
                this->actor.world.pos.x =
                    chest->dyna.actor.world.pos.x - (Math_SinS(chest->dyna.actor.shape.rot.y) * 29.4343f);
                this->actor.world.pos.z =
                    chest->dyna.actor.world.pos.z - (Math_CosS(chest->dyna.actor.shape.rot.y) * 29.4343f);
                this->currentYaw = this->actor.shape.rot.y = chest->dyna.actor.shape.rot.y;
                func_80832224(this);

                if ((giEntry->itemId != ITEM_NONE) && (giEntry->gi >= 0) &&
                    (Item_CheckObtainability(giEntry->itemId) == ITEM_NONE)) {
                    func_808322D0(globalCtx, this, this->ageProperties->unk_98);
                    func_80832F54(globalCtx, this, 0x28F);
                    chest->unk_1F4 = 1;
                    Camera_ChangeSetting(Gameplay_GetCamera(globalCtx, 0), CAM_SET_SLOW_CHEST_CS);
                }
                else {
                    func_80832264(globalCtx, this, &gPlayerAnim_002DF8);
                    chest->unk_1F4 = -1;
                }

                return 1;
            }

            if ((this->heldActor == NULL) || Player_HoldsHookshot(this)) {
                if ((interactedActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT) {
                    s32 sp24 = this->itemActionParam;

                    this->itemActionParam = PLAYER_AP_NONE;
                    this->modelAnimType = PLAYER_ANIMTYPE_0;
                    this->heldItemActionParam = this->itemActionParam;
                    func_80836898(globalCtx, this, func_8083A0F4);

                    if (sp24 == PLAYER_AP_SWORD_MASTER) {
                        this->nextModelGroup = Player_ActionToModelGroup(this, PLAYER_AP_LAST_USED);
                        func_8083399C(globalCtx, this, PLAYER_AP_LAST_USED);
                    }
                    else {
                        func_80835F44(globalCtx, this, ITEM_LAST_USED);
                    }
                }
                else {
                    s32 strength = Player_GetStrength();

                    if ((interactedActor->id == ACTOR_EN_ISHI) && ((interactedActor->params & 0xF) == 1) &&
                        (strength < PLAYER_STR_SILVER_G)) {
                        return 0;
                    }

                    func_80836898(globalCtx, this, func_8083A0F4);
                }

                func_80832224(this);
                this->stateFlags1 |= PLAYER_STATE1_11;
                return 1;
            }
        }
    }

    return 0;
}

void func_8083EA94(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_80846578, 1);
    func_80832264(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_29][this->modelAnimType]);
}

s32 func_8083EAF0(Player* this, Actor* actor) {
    if ((actor != NULL) && !(actor->flags & ACTOR_FLAG_23) &&
        ((this->linearVelocity < 1.1f) || (actor->id == ACTOR_EN_BOM_CHU))) {
        return 0;
    }

    return 1;
}

s32 func_8083EB44(Player* this, GlobalContext* globalCtx) {
    if ((this->stateFlags1 & PLAYER_STATE1_11) && (this->heldActor != NULL) &&
        CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN)) {
        if (!func_80835644(globalCtx, this, this->heldActor)) {
            if (!func_8083EAF0(this, this->heldActor)) {
                func_80835C58(globalCtx, this, func_808464B0, 1);
                func_80832264(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_30][this->modelAnimType]);
            }
            else {
                func_8083EA94(this, globalCtx);
            }
        }
        return 1;
    }

    return 0;
}

s32 func_8083EC18(Player* this, GlobalContext* globalCtx, u32 arg2) {
    if (this->wallHeight >= 79.0f) {
        if (!(this->stateFlags1 & PLAYER_STATE1_27) || (this->currentBoots == PLAYER_BOOTS_IRON) ||
            (this->actor.yDistToWater < this->ageProperties->unk_2C)) {
            s32 sp8C = (arg2 & 8) ? 2 : 0;

            if ((sp8C != 0) || (arg2 & 2) ||
                func_80041E4C(&globalCtx->colCtx, this->actor.wallPoly, this->actor.wallBgId)) {
                f32 phi_f20;
                CollisionPoly* sp84 = this->actor.wallPoly;
                f32 sp80;
                f32 sp7C;
                f32 phi_f12;
                f32 phi_f14;

                phi_f20 = phi_f12 = 0.0f;

                if (sp8C != 0) {
                    sp80 = this->actor.world.pos.x;
                    sp7C = this->actor.world.pos.z;
                }
                else {
                    Vec3f sp50[3];
                    s32 i;
                    f32 sp48;
                    Vec3f* sp44 = &sp50[0];
                    s32 pad;

                    CollisionPoly_GetVerticesByBgId(sp84, this->actor.wallBgId, &globalCtx->colCtx, sp50);

                    sp80 = phi_f12 = sp44->x;
                    sp7C = phi_f14 = sp44->z;
                    phi_f20 = sp44->y;
                    for (i = 1; i < 3; i++) {
                        sp44++;
                        if (sp80 > sp44->x) {
                            sp80 = sp44->x;
                        }
                        else if (phi_f12 < sp44->x) {
                            phi_f12 = sp44->x;
                        }

                        if (sp7C > sp44->z) {
                            sp7C = sp44->z;
                        }
                        else if (phi_f14 < sp44->z) {
                            phi_f14 = sp44->z;
                        }

                        if (phi_f20 > sp44->y) {
                            phi_f20 = sp44->y;
                        }
                    }

                    sp80 = (sp80 + phi_f12) * 0.5f;
                    sp7C = (sp7C + phi_f14) * 0.5f;

                    phi_f12 = ((this->actor.world.pos.x - sp80) * COLPOLY_GET_NORMAL(sp84->normal.z)) -
                        ((this->actor.world.pos.z - sp7C) * COLPOLY_GET_NORMAL(sp84->normal.x));
                    sp48 = this->actor.world.pos.y - phi_f20;

                    phi_f20 = ((f32)(s32)((sp48 / 15.000000223517418) + 0.5) * 15.000000223517418) - sp48;
                    phi_f12 = fabsf(phi_f12);
                }

                if (phi_f12 < 8.0f) {
                    f32 sp3C = COLPOLY_GET_NORMAL(sp84->normal.x);
                    f32 sp38 = COLPOLY_GET_NORMAL(sp84->normal.z);
                    f32 sp34 = this->wallDistance;
                    LinkAnimationHeader* sp30;

                    func_80836898(globalCtx, this, func_8083A3B0);
                    this->stateFlags1 |= PLAYER_STATE1_21;
                    this->stateFlags1 &= ~PLAYER_STATE1_27;

                    if ((sp8C != 0) || (arg2 & 2)) {
                        if ((this->unk_84F = sp8C) != 0) {
                            if (this->actor.bgCheckFlags & 1) {
                                sp30 = &gPlayerAnim_002D80;
                            }
                            else {
                                sp30 = &gPlayerAnim_002D68;
                            }
                            sp34 = (this->ageProperties->unk_38 - 1.0f) - sp34;
                        }
                        else {
                            sp30 = this->ageProperties->unk_A4;
                            sp34 = sp34 - 1.0f;
                        }
                        this->unk_850 = -2;
                        this->actor.world.pos.y += phi_f20;
                        this->actor.shape.rot.y = this->currentYaw = this->actor.wallYaw + 0x8000;
                    }
                    else {
                        sp30 = this->ageProperties->unk_A8;
                        this->unk_850 = -4;
                        this->actor.shape.rot.y = this->currentYaw = this->actor.wallYaw;
                    }

                    this->actor.world.pos.x = (sp34 * sp3C) + sp80;
                    this->actor.world.pos.z = (sp34 * sp38) + sp7C;
                    func_80832224(this);
                    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
                    func_80832264(globalCtx, this, sp30);
                    func_80832F54(globalCtx, this, 0x9F);

                    return 1;
                }
            }
        }
    }

    return 0;
}

void func_8083F070(Player* this, LinkAnimationHeader* anim, GlobalContext* globalCtx) {
    func_80835DAC(globalCtx, this, func_8084C5F8, 0);
    LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, anim, (4.0f / 3.0f));
}

s32 func_8083F0C8(Player* this, GlobalContext* globalCtx, u32 arg2) {
    CollisionPoly* wallPoly;
    Vec3f wallVertices[3];
    f32 tempX;
    f32 temp;
    f32 tempZ;
    f32 maxWallZ;
    s32 i;

    if (!LINK_IS_ADULT && !(this->stateFlags1 & PLAYER_STATE1_27) && (arg2 & 0x30)) {
        wallPoly = this->actor.wallPoly;
        CollisionPoly_GetVerticesByBgId(wallPoly, this->actor.wallBgId, &globalCtx->colCtx, wallVertices);

        // compute min and max x/z of wall vertices
        tempX = temp = wallVertices[0].x;
        tempZ = maxWallZ = wallVertices[0].z;
        for (i = 1; i < 3; i++) {
            if (tempX > wallVertices[i].x) {
                tempX = wallVertices[i].x;
            }
            else if (temp < wallVertices[i].x) {
                temp = wallVertices[i].x;
            }

            if (tempZ > wallVertices[i].z) {
                tempZ = wallVertices[i].z;
            }
            else if (maxWallZ < wallVertices[i].z) {
                maxWallZ = wallVertices[i].z;
            }
        }

        // average min and max x/z of wall vertices
        tempX = (tempX + temp) * 0.5f;
        tempZ = (tempZ + maxWallZ) * 0.5f;

        temp = ((this->actor.world.pos.x - tempX) * COLPOLY_GET_NORMAL(wallPoly->normal.z)) -
            ((this->actor.world.pos.z - tempZ) * COLPOLY_GET_NORMAL(wallPoly->normal.x));

        if (fabsf(temp) < 8.0f) {
            this->stateFlags2 |= PLAYER_STATE2_16;

            if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
                f32 wallPolyNormX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 wallPolyNormZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 wallDistance = this->wallDistance;

                func_80836898(globalCtx, this, func_8083A40C);
                this->stateFlags2 |= PLAYER_STATE2_18;
                this->actor.shape.rot.y = this->currentYaw = this->actor.wallYaw + 0x8000;
                this->actor.world.pos.x = tempX + (wallDistance * wallPolyNormX);
                this->actor.world.pos.z = tempZ + (wallDistance * wallPolyNormZ);
                func_80832224(this);
                this->actor.prevPos = this->actor.world.pos;
                func_80832264(globalCtx, this, &gPlayerAnim_002708);
                func_80832F54(globalCtx, this, 0x9D);

                return 1;
            }
        }
    }

    return 0;
}

s32 func_8083F360(GlobalContext* globalCtx, Player* this, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    CollisionPoly* wallPoly;
    s32 sp78;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    f32 yawCos;
    f32 yawSin;
    s32 temp;
    f32 temp1;
    f32 temp2;

    yawCos = Math_CosS(this->actor.shape.rot.y);
    yawSin = Math_SinS(this->actor.shape.rot.y);

    sp6C.x = this->actor.world.pos.x + (arg4 * yawSin);
    sp6C.z = this->actor.world.pos.z + (arg4 * yawCos);
    sp60.x = this->actor.world.pos.x + (arg3 * yawSin);
    sp60.z = this->actor.world.pos.z + (arg3 * yawCos);
    sp60.y = sp6C.y = this->actor.world.pos.y + arg1;

    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &sp6C, &sp60, &sp54, &this->actor.wallPoly, true, false, false,
        true, &sp78)) {
        wallPoly = this->actor.wallPoly;

        this->actor.bgCheckFlags |= 0x200;
        this->actor.wallBgId = sp78;

        D_808535F0 = func_80041DB8(&globalCtx->colCtx, wallPoly, sp78);

        temp1 = COLPOLY_GET_NORMAL(wallPoly->normal.x);
        temp2 = COLPOLY_GET_NORMAL(wallPoly->normal.z);
        temp = Math_Atan2S(-temp2, -temp1);
        Math_ScaledStepToS(&this->actor.shape.rot.y, temp, 800);

        this->currentYaw = this->actor.shape.rot.y;
        this->actor.world.pos.x = sp54.x - (Math_SinS(this->actor.shape.rot.y) * arg2);
        this->actor.world.pos.z = sp54.z - (Math_CosS(this->actor.shape.rot.y) * arg2);

        return 1;
    }

    this->actor.bgCheckFlags &= ~0x200;

    return 0;
}

s32 func_8083F524(GlobalContext* globalCtx, Player* this) {
    return func_8083F360(globalCtx, this, 26.0f, this->ageProperties->unk_38 + 5.0f, 30.0f, 0.0f);
}

s32 func_8083F570(Player* this, GlobalContext* globalCtx) {
    s16 temp;

    if ((this->linearVelocity != 0.0f) && (this->actor.bgCheckFlags & 8) && (D_808535F0 & 0x30)) {

        temp = this->actor.shape.rot.y - this->actor.wallYaw;
        if (this->linearVelocity < 0.0f) {
            temp += 0x8000;
        }

        if (ABS(temp) > 0x4000) {
            func_80835C58(globalCtx, this, func_8084C81C, 0);

            if (this->linearVelocity > 0.0f) {
                this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
                func_80832264(globalCtx, this, &gPlayerAnim_002700);
                func_80832F54(globalCtx, this, 0x9D);
                OnePointCutscene_Init(globalCtx, 9601, 999, NULL, MAIN_CAM);
            }
            else {
                this->actor.shape.rot.y = this->actor.wallYaw;
                LinkAnimation_Change(globalCtx, &this->skelAnime, &gPlayerAnim_002708, -1.0f,
                    Animation_GetLastFrame(&gPlayerAnim_002708), 0.0f, ANIMMODE_ONCE, 0.0f);
                func_80832F54(globalCtx, this, 0x9D);
                OnePointCutscene_Init(globalCtx, 9602, 999, NULL, MAIN_CAM);
            }

            this->currentYaw = this->actor.shape.rot.y;
            func_80832210(this);

            return 1;
        }
    }

    return 0;
}

void func_8083F72C(Player* this, LinkAnimationHeader* anim, GlobalContext* globalCtx) {
    if (!func_80836898(globalCtx, this, func_8083A388)) {
        func_80835C58(globalCtx, this, func_8084B78C, 0);
    }

    func_80832264(globalCtx, this, anim);
    func_80832224(this);

    this->actor.shape.rot.y = this->currentYaw = this->actor.wallYaw + 0x8000;
}

s32 func_8083F7BC(Player* this, GlobalContext* globalCtx) {
    DynaPolyActor* wallPolyActor;

    if (!(this->stateFlags1 & PLAYER_STATE1_11) && (this->actor.bgCheckFlags & 0x200) && (D_80853608 < 0x3000)) {

        if (((this->linearVelocity > 0.0f) && func_8083EC18(this, globalCtx, D_808535F0)) ||
            func_8083F0C8(this, globalCtx, D_808535F0)) {
            return 1;
        }

        if (!func_808332B8(this) && ((this->linearVelocity == 0.0f) || !(this->stateFlags2 & PLAYER_STATE2_2)) &&
            (D_808535F0 & 0x40) && (this->actor.bgCheckFlags & 1) && (this->wallHeight >= 39.0f)) {

            this->stateFlags2 |= PLAYER_STATE2_0;

            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A)) {

                if ((this->actor.wallBgId != BGCHECK_SCENE) &&
                    ((wallPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, this->actor.wallBgId)) != NULL)) {

                    if (wallPolyActor->actor.id == ACTOR_BG_HEAVY_BLOCK) {
                        if (Player_GetStrength() < PLAYER_STR_GOLD_G) {
                            return 0;
                        }

                        func_80836898(globalCtx, this, func_8083A0F4);
                        this->stateFlags1 |= PLAYER_STATE1_11;
                        this->interactRangeActor = &wallPolyActor->actor;
                        this->getItemId = GI_NONE;
                        this->currentYaw = this->actor.wallYaw + 0x8000;
                        func_80832224(this);

                        return 1;
                    }

                    this->unk_3C4 = &wallPolyActor->actor;
                }
                else {
                    this->unk_3C4 = NULL;
                }

                func_8083F72C(this, &gPlayerAnim_0030F8, globalCtx);

                return 1;
            }
        }
    }

    return 0;
}

s32 func_8083F9D0(GlobalContext* globalCtx, Player* this) {
    if ((this->actor.bgCheckFlags & 0x200) &&
        ((this->stateFlags2 & PLAYER_STATE2_4) || CHECK_BTN_ALL(sControlInput->cur.button, BTN_A))) {
        DynaPolyActor* wallPolyActor = NULL;

        if (this->actor.wallBgId != BGCHECK_SCENE) {
            wallPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, this->actor.wallBgId);
        }

        if (&wallPolyActor->actor == this->unk_3C4) {
            if (this->stateFlags2 & PLAYER_STATE2_4) {
                return 1;
            }
            else {
                return 0;
            }
        }
    }

    func_80839FFC(this, globalCtx);
    func_80832264(globalCtx, this, &gPlayerAnim_003100);
    this->stateFlags2 &= ~PLAYER_STATE2_4;
    return 1;
}

void func_8083FAB8(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_8084B898, 0);
    this->stateFlags2 |= PLAYER_STATE2_4;
    func_80832264(globalCtx, this, &gPlayerAnim_0030F0);
}

void func_8083FB14(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_8084B9E4, 0);
    this->stateFlags2 |= PLAYER_STATE2_4;
    func_80832264(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_35][this->modelAnimType]);
}

void func_8083FB7C(Player* this, GlobalContext* globalCtx) {
    this->stateFlags1 &= ~(PLAYER_STATE1_21 | PLAYER_STATE1_27);
    func_80837B9C(this, globalCtx);
    this->linearVelocity = -0.4f;
}

s32 func_8083FBC0(Player* this, GlobalContext* globalCtx) {
    if (!CHECK_BTN_ALL(sControlInput->press.button, BTN_A) && (this->actor.bgCheckFlags & 0x200) &&
        ((D_808535F0 & 8) || (D_808535F0 & 2) ||
            func_80041E4C(&globalCtx->colCtx, this->actor.wallPoly, this->actor.wallBgId))) {
        return 0;
    }

    func_8083FB7C(this, globalCtx);
    func_80832698(this, NA_SE_VO_LI_AUTO_JUMP);
    return 1;
}

s32 func_8083FC68(Player* this, f32 arg1, s16 arg2) {
    f32 sp1C = (s16)(arg2 - this->actor.shape.rot.y);
    f32 temp;

    if (this->unk_664 != NULL) {
        func_8083DB98(this, func_8002DD78(this) || func_808334B4(this));
    }

    temp = fabsf(sp1C) / 32768.0f;

    if (arg1 > (((temp * temp) * 50.0f) + 6.0f)) {
        return 1;
    }
    else if (arg1 > (((1.0f - temp) * 10.0f) + 6.8f)) {
        return -1;
    }

    return 0;
}

s32 func_8083FD78(Player* this, f32* arg1, s16* arg2, GlobalContext* globalCtx) {
    s16 sp2E = *arg2 - this->targetYaw;
    u16 sp2C = ABS(sp2E);

    if ((func_8002DD78(this) || func_808334B4(this)) && (this->unk_664 == NULL)) {
        *arg1 *= Math_SinS(sp2C);

        if (*arg1 != 0.0f) {
            *arg2 = (((sp2E >= 0) ? 1 : -1) << 0xE) + this->actor.shape.rot.y;
        }
        else {
            *arg2 = this->actor.shape.rot.y;
        }

        if (this->unk_664 != NULL) {
            func_8083DB98(this, 1);
        }
        else {
            Math_SmoothStepToS(&this->actor.focus.rot.x, sControlInput->rel.stick_y * 240.0f, 14, 4000, 30);
            func_80836AB8(this, 1);
        }
    }
    else {
        if (this->unk_664 != NULL) {
            return func_8083FC68(this, *arg1, *arg2);
        }
        else {
            func_8083DC54(this, globalCtx);
            if ((*arg1 != 0.0f) && (sp2C < 6000)) {
                return 1;
            }
            else if (*arg1 > Math_SinS((0x4000 - (sp2C >> 1))) * 200.0f) {
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
        }
        else {
            return -1;
        }
    }

    return 0;
}

s32 func_80840058(Player* this, f32* arg1, s16* arg2, GlobalContext* globalCtx) {
    func_8083DC54(this, globalCtx);

    if ((*arg1 != 0.0f) || (ABS(this->unk_87C) > 400)) {
        s16 temp1 = *arg2 - Camera_GetInputDirYaw(GET_ACTIVE_CAM(globalCtx));
        u16 temp2 = (ABS(temp1) - 0x2000) & 0xFFFF;

        if ((temp2 < 0x4000) || (this->unk_87C != 0)) {
            return -1;
        }
        else {
            return 1;
        }
    }

    return 0;
}

void func_80840138(Player* this, f32 arg1, s16 arg2) {
    s16 temp = arg2 - this->actor.shape.rot.y;

    if (arg1 > 0.0f) {
        if (temp < 0) {
            this->unk_874 = 0.0f;
        }
        else {
            this->unk_874 = 1.0f;
        }
    }

    Math_StepToF(&this->unk_870, this->unk_874, 0.3f);
}

void func_808401B0(GlobalContext* globalCtx, Player* this) {
    LinkAnimation_BlendToJoint(globalCtx, &this->skelAnime, func_808334E4(this), this->unk_868, func_80833528(this),
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
    }
    else if (arg1 > 7.25f) {
        arg1 = 7.25f;
    }

    if (1) {}

    if ((this->currentBoots == PLAYER_BOOTS_HOVER) && !(this->actor.bgCheckFlags & 1) && (this->hoverBootsTimer != 0)) {
        func_8002F8F0(&this->actor, NA_SE_PL_HOBBERBOOTS_LV - SFX_FLAG);
    }
    else if (func_8084021C(this->unk_868, arg1, 29.0f, 10.0f) || func_8084021C(this->unk_868, arg1, 29.0f, 24.0f)) {
        func_808327F8(this, this->linearVelocity);
        if (this->linearVelocity > 4.0f) {
            this->stateFlags2 |= PLAYER_STATE2_3;
        }
    }

    this->unk_868 += arg1;

    if (this->unk_868 < 0.0f) {
        this->unk_868 += 29.0f;
    }
    else if (this->unk_868 >= 29.0f) {
        this->unk_868 -= 29.0f;
    }
}

void func_80840450(Player* this, GlobalContext* globalCtx) {
    f32 sp44;
    s16 sp42;
    s32 temp1;
    u32 temp2;
    s16 temp3;
    s32 temp4;

    if (this->stateFlags3 & PLAYER_STATE3_3) {
        if (Player_GetSwordHeld(this)) {
            this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;
        }
        else {
            this->stateFlags3 &= ~PLAYER_STATE3_3;
        }
    }

    if (this->unk_850 != 0) {
        if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
            func_80832DBC(this);
            func_80832284(globalCtx, this, func_808334E4(this));
            this->unk_850 = 0;
            this->stateFlags3 &= ~PLAYER_STATE3_3;
        }
        func_80833C3C(this);
    }
    else {
        func_808401B0(globalCtx, this);
    }

    func_8083721C(this);

    if (!func_80837348(globalCtx, this, D_808543E0, 1)) {
        if (!func_80833B54(this) && (!func_80833B2C(this) || (func_80834B5C != this->func_82C))) {
            func_8083CF10(this, globalCtx);
            return;
        }

        func_80837268(this, &sp44, &sp42, 0.0f, globalCtx);

        temp1 = func_8083FC68(this, sp44, sp42);

        if (temp1 > 0) {
            func_8083C8DC(this, globalCtx, sp42);
            return;
        }

        if (temp1 < 0) {
            func_8083CBF0(this, sp42, globalCtx);
            return;
        }

        if (sp44 > 4.0f) {
            func_8083CC9C(this, globalCtx);
            return;
        }

        func_8084029C(this, (this->linearVelocity * 0.3f) + 1.0f);
        func_80840138(this, sp44, sp42);

        temp2 = this->unk_868;
        if ((temp2 < 6) || ((temp2 - 0xE) < 6)) {
            Math_StepToF(&this->linearVelocity, 0.0f, 1.5f);
            return;
        }

        temp3 = sp42 - this->currentYaw;
        temp4 = ABS(temp3);

        if (temp4 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.5f)) {
                this->currentYaw = sp42;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, sp44 * 0.3f, 2.0f, 1.5f);

        if (!(this->stateFlags3 & PLAYER_STATE3_3)) {
            Math_ScaledStepToS(&this->currentYaw, sp42, temp4 * 0.1f);
        }
    }
}

void func_808407CC(Player* this, GlobalContext* globalCtx) {
    f32 sp3C;
    s16 sp3A;
    s32 temp1;
    s16 temp2;
    s32 temp3;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80832DBC(this);
        func_80832264(globalCtx, this, func_80833338(this));
    }

    func_8083721C(this);

    if (!func_80837348(globalCtx, this, D_808543E8, 1)) {
        if (func_80833B54(this)) {
            func_8083CEAC(this, globalCtx);
            return;
        }

        if (!func_80833B2C(this)) {
            func_80835DAC(globalCtx, this, func_80840BC8, 1);
            this->currentYaw = this->actor.shape.rot.y;
            return;
        }

        if (func_80834B5C == this->func_82C) {
            func_8083CEAC(this, globalCtx);
            return;
        }

        func_80837268(this, &sp3C, &sp3A, 0.0f, globalCtx);

        temp1 = func_8083FD78(this, &sp3C, &sp3A, globalCtx);

        if (temp1 > 0) {
            func_8083C8DC(this, globalCtx, sp3A);
            return;
        }

        if (temp1 < 0) {
            func_8083CB2C(this, sp3A, globalCtx);
            return;
        }

        if (sp3C > 4.9f) {
            func_8083CC9C(this, globalCtx);
            func_80833C3C(this);
            return;
        }
        if (sp3C != 0.0f) {
            func_8083CB94(this, globalCtx);
            return;
        }

        temp2 = sp3A - this->actor.shape.rot.y;
        temp3 = ABS(temp2);

        if (temp3 > 800) {
            func_8083CD54(globalCtx, this, sp3A);
        }
    }
}

void func_808409CC(GlobalContext* globalCtx, Player* this) {
    LinkAnimationHeader* anim;
    LinkAnimationHeader** animPtr;
    s32 heathIsCritical;
    s32 sp38;
    s32 sp34;

    if ((this->unk_664 != NULL) ||
        (!(heathIsCritical = HealthMeter_IsCritical()) && ((this->unk_6AC = (this->unk_6AC + 1) & 1) != 0))) {
        this->stateFlags2 &= ~PLAYER_STATE2_28;
        anim = func_80833338(this);
    }
    else {
        this->stateFlags2 |= PLAYER_STATE2_28;
        if (this->stateFlags1 & PLAYER_STATE1_11) {
            anim = func_80833338(this);
        }
        else {
            sp38 = globalCtx->roomCtx.curRoom.unk_02;
            if (heathIsCritical) {
                if (this->unk_6AC >= 0) {
                    sp38 = 7;
                    this->unk_6AC = -1;
                }
                else {
                    sp38 = 8;
                }
            }
            else {
                sp34 = Rand_ZeroOne() * 5.0f;
                if (sp34 < 4) {
                    if (((sp34 != 0) && (sp34 != 3)) || ((this->rightHandType == PLAYER_MODELTYPE_RH_SHIELD) &&
                        ((sp34 == 3) || Player_GetSwordHeld(this)))) {
                        if ((sp34 == 1) && Player_HoldsTwoHandedWeapon(this) && CVar_GetS32("gTwoHandedIdle", 1) == 1) {
                            sp34 = 4;
                        }
                        sp38 = sp34 + 9;
                    }
                }
            }
            animPtr = &D_80853D7C[sp38][0];
            if (this->modelAnimType != PLAYER_ANIMTYPE_1) {
                animPtr = &D_80853D7C[sp38][1];
            }
            anim = *animPtr;
        }
    }

    LinkAnimation_Change(globalCtx, &this->skelAnime, anim, (2.0f / 3.0f) * D_808535E8, 0.0f,
        Animation_GetLastFrame(anim), ANIMMODE_ONCE, -6.0f);
}

void func_80840BC8(Player* this, GlobalContext* globalCtx) {
    s32 sp44;
    s32 sp40;
    f32 sp3C;
    s16 sp3A;
    s16 temp;

    sp44 = func_80833350(this);
    sp40 = LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (sp44 > 0) {
        func_808333FC(this, sp44 - 1);
    }

    if (sp40 != 0) {
        if (this->unk_850 != 0) {
            if (DECR(this->unk_850) == 0) {
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
            this->skelAnime.jointTable[0].y = (this->skelAnime.jointTable[0].y + ((this->unk_850 & 1) * 0x50)) - 0x28;
        }
        else {
            func_80832DBC(this);
            func_808409CC(globalCtx, this);
        }
    }

    func_8083721C(this);

    if (this->unk_850 == 0) {
        if (!func_80837348(globalCtx, this, D_80854418, 1)) {
            if (func_80833B54(this)) {
                func_8083CEAC(this, globalCtx);
                return;
            }

            if (func_80833B2C(this)) {
                func_80839F30(this, globalCtx);
                return;
            }

            func_80837268(this, &sp3C, &sp3A, 0.018f, globalCtx);

            if (sp3C != 0.0f) {
                func_8083C8DC(this, globalCtx, sp3A);
                return;
            }

            temp = sp3A - this->actor.shape.rot.y;
            if (ABS(temp) > 800) {
                func_8083CD54(globalCtx, this, sp3A);
                return;
            }

            Math_ScaledStepToS(&this->actor.shape.rot.y, sp3A, 1200);
            this->currentYaw = this->actor.shape.rot.y;
            if (func_80833338(this) == this->skelAnime.animation) {
                func_8083DC54(this, globalCtx);
            }
        }
    }
}

void func_80840DE4(Player* this, GlobalContext* globalCtx) {
    f32 frames;
    f32 coeff;
    f32 sp44;
    s16 sp42;
    s32 temp1;
    s16 temp2;
    s32 temp3;
    s32 direction;

    this->skelAnime.mode = 0;
    LinkAnimation_SetUpdateFunction(&this->skelAnime);

    this->skelAnime.animation = func_8083356C(this);

    if (this->skelAnime.animation == &gPlayerAnim_0026E8) {
        frames = 24.0f;
        coeff = -(MREG(95) / 100.0f);
    }
    else {
        frames = 29.0f;
        coeff = MREG(95) / 100.0f;
    }

    this->skelAnime.animLength = frames;
    this->skelAnime.endFrame = frames - 1.0f;

    if ((s16)(this->currentYaw - this->actor.shape.rot.y) >= 0) {
        direction = 1;
    }
    else {
        direction = -1;
    }

    this->skelAnime.playSpeed = direction * (this->linearVelocity * coeff);

    LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 0.0f) || LinkAnimation_OnFrame(&this->skelAnime, frames * 0.5f)) {
        func_808327F8(this, this->linearVelocity);
    }

    if (!func_80837348(globalCtx, this, D_808543F4, 1)) {
        if (func_80833B54(this)) {
            func_8083CEAC(this, globalCtx);
            return;
        }

        if (!func_80833B2C(this)) {
            func_80853080(this, globalCtx);
            return;
        }

        func_80837268(this, &sp44, &sp42, 0.0f, globalCtx);
        temp1 = func_8083FD78(this, &sp44, &sp42, globalCtx);

        if (temp1 > 0) {
            func_8083C8DC(this, globalCtx, sp42);
            return;
        }

        if (temp1 < 0) {
            func_8083CB2C(this, sp42, globalCtx);
            return;
        }

        if (sp44 > 4.9f) {
            func_8083CC9C(this, globalCtx);
            func_80833C3C(this);
            return;
        }

        if ((sp44 == 0.0f) && (this->linearVelocity == 0.0f)) {
            func_80839F30(this, globalCtx);
            return;
        }

        temp2 = sp42 - this->currentYaw;
        temp3 = ABS(temp2);

        if (temp3 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.5f)) {
                this->currentYaw = sp42;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, sp44 * 0.4f, 1.5f, 1.5f);
        Math_ScaledStepToS(&this->currentYaw, sp42, temp3 * 0.1f);
    }
}

void func_80841138(Player* this, GlobalContext* globalCtx) {
    f32 temp1;
    f32 temp2;

    if (this->unk_864 < 1.0f) {
        temp1 = R_UPDATE_RATE * 0.5f;
        func_8084029C(this, REG(35) / 1000.0f);
        LinkAnimation_LoadToJoint(globalCtx, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_31][this->modelAnimType],
            this->unk_868);
        this->unk_864 += 1 * temp1;
        if (this->unk_864 >= 1.0f) {
            this->unk_864 = 1.0f;
        }
        temp1 = this->unk_864;
    }
    else {
        temp2 = this->linearVelocity - (REG(48) / 100.0f);
        if (temp2 < 0.0f) {
            temp1 = 1.0f;
            func_8084029C(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));
            LinkAnimation_LoadToJoint(globalCtx, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_31][this->modelAnimType],
                this->unk_868);
        }
        else {
            temp1 = (REG(37) / 1000.0f) * temp2;
            if (temp1 < 1.0f) {
                func_8084029C(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));
            }
            else {
                temp1 = 1.0f;
                func_8084029C(this, 1.2f + ((REG(38) / 1000.0f) * temp2));
            }
            LinkAnimation_LoadToMorph(globalCtx, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_31][this->modelAnimType],
                this->unk_868);
            LinkAnimation_LoadToJoint(globalCtx, &this->skelAnime, &gPlayerAnim_002DD0,
                this->unk_868 * (16.0f / 29.0f));
        }
    }

    if (temp1 < 1.0f) {
        LinkAnimation_InterpJointMorph(globalCtx, &this->skelAnime, 1.0f - temp1);
    }
}

void func_8084140C(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_8084170C, 1);
    func_80832B0C(globalCtx, this, &gPlayerAnim_002DA0);
}

s32 func_80841458(Player* this, f32* arg1, s16* arg2, GlobalContext* globalCtx) {
    if (this->linearVelocity > 6.0f) {
        func_8084140C(this, globalCtx);
        return 1;
    }

    if (*arg1 != 0.0f) {
        if (func_8083721C(this)) {
            *arg1 = 0.0f;
            *arg2 = this->currentYaw;
        }
        else {
            return 1;
        }
    }

    return 0;
}

void func_808414F8(Player* this, GlobalContext* globalCtx) {
    f32 sp34;
    s16 sp32;
    s32 sp2C;
    s16 sp2A;

    func_80841138(this, globalCtx);

    if (!func_80837348(globalCtx, this, D_80854400, 1)) {
        if (!func_80833C04(this)) {
            func_8083C8DC(this, globalCtx, this->currentYaw);
            return;
        }

        func_80837268(this, &sp34, &sp32, 0.0f, globalCtx);
        sp2C = func_8083FD78(this, &sp34, &sp32, globalCtx);

        if (sp2C >= 0) {
            if (!func_80841458(this, &sp34, &sp32, globalCtx)) {
                if (sp2C != 0) {
                    func_8083C858(this, globalCtx);
                }
                else if (sp34 > 4.9f) {
                    func_8083CC9C(this, globalCtx);
                }
                else {
                    func_8083CB94(this, globalCtx);
                }
            }
        }
        else {
            sp2A = sp32 - this->currentYaw;

            Math_AsymStepToF(&this->linearVelocity, sp34 * 1.5f, 1.5f, 2.0f);
            Math_ScaledStepToS(&this->currentYaw, sp32, sp2A * 0.1f);

            if ((sp34 == 0.0f) && (this->linearVelocity == 0.0f)) {
                func_80839F30(this, globalCtx);
            }
        }
    }
}

void func_808416C0(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_808417FC, 1);
    func_80832264(globalCtx, this, &gPlayerAnim_002DA8);
}

void func_8084170C(Player* this, GlobalContext* globalCtx) {
    s32 sp34;
    f32 sp30;
    s16 sp2E;

    sp34 = LinkAnimation_Update(globalCtx, &this->skelAnime);
    func_8083721C(this);

    if (!func_80837348(globalCtx, this, D_80854400, 1)) {
        func_80837268(this, &sp30, &sp2E, 0.0f, globalCtx);

        if (this->linearVelocity == 0.0f) {
            this->currentYaw = this->actor.shape.rot.y;

            if (func_8083FD78(this, &sp30, &sp2E, globalCtx) > 0) {
                func_8083C858(this, globalCtx);
            }
            else if ((sp30 != 0.0f) || (sp34 != 0)) {
                func_808416C0(this, globalCtx);
            }
        }
    }
}

void func_808417FC(Player* this, GlobalContext* globalCtx) {
    s32 sp1C;

    sp1C = LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (!func_80837348(globalCtx, this, D_80854400, 1)) {
        if (sp1C != 0) {
            func_80839F30(this, globalCtx);
        }
    }
}

void func_80841860(GlobalContext* globalCtx, Player* this) {
    f32 frame;
    // fake match? see func_80833664
    LinkAnimationHeader* sp38 = D_80853914[0][this->modelAnimType + PLAYER_ANIMGROUP_24 * ARRAY_COUNT(D_80853914[0])];
    LinkAnimationHeader* sp34 = D_80853914[0][this->modelAnimType + PLAYER_ANIMGROUP_25 * ARRAY_COUNT(D_80853914[0])];

    this->skelAnime.animation = sp38;

    func_8084029C(this, (REG(30) / 1000.0f) + ((REG(32) / 1000.0f) * this->linearVelocity));

    frame = this->unk_868 * (16.0f / 29.0f);
    LinkAnimation_BlendToJoint(globalCtx, &this->skelAnime, sp34, frame, sp38, frame, this->unk_870, this->blendTable);
}

void func_8084193C(Player* this, GlobalContext* globalCtx) {
    f32 sp3C;
    s16 sp3A;
    s32 temp1;
    s16 temp2;
    s32 temp3;

    func_80841860(globalCtx, this);

    if (!func_80837348(globalCtx, this, D_80854408, 1)) {
        if (!func_80833C04(this)) {
            func_8083C858(this, globalCtx);
            return;
        }

        func_80837268(this, &sp3C, &sp3A, 0.0f, globalCtx);

        if (func_80833B2C(this)) {
            temp1 = func_8083FD78(this, &sp3C, &sp3A, globalCtx);
        }
        else {
            temp1 = func_8083FC68(this, sp3C, sp3A);
        }

        if (temp1 > 0) {
            func_8083C858(this, globalCtx);
            return;
        }

        if (temp1 < 0) {
            if (func_80833B2C(this)) {
                func_8083CB2C(this, sp3A, globalCtx);
            }
            else {
                func_8083CBF0(this, sp3A, globalCtx);
            }
            return;
        }

        if ((this->linearVelocity < 3.6f) && (sp3C < 4.0f)) {
            if (!func_8008E9C4(this) && func_80833B2C(this)) {
                func_8083CB94(this, globalCtx);
            }
            else {
                func_80839F90(this, globalCtx);
            }
            return;
        }

        func_80840138(this, sp3C, sp3A);

        temp2 = sp3A - this->currentYaw;
        temp3 = ABS(temp2);

        if (temp3 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 3.0f) != 0) {
                this->currentYaw = sp3A;
            }
            return;
        }

        sp3C *= 0.9f;
        Math_AsymStepToF(&this->linearVelocity, sp3C, 2.0f, 3.0f);
        Math_ScaledStepToS(&this->currentYaw, sp3A, temp3 * 0.1f);
    }
}

void func_80841BA8(Player* this, GlobalContext* globalCtx) {
    f32 sp34;
    s16 sp32;

    LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (Player_HoldsTwoHandedWeapon(this)) {
        AnimationContext_SetLoadFrame(globalCtx, func_80833338(this), 0, this->skelAnime.limbCount,
            this->skelAnime.morphTable);
        AnimationContext_SetCopyTrue(globalCtx, this->skelAnime.limbCount, this->skelAnime.jointTable,
            this->skelAnime.morphTable, D_80853410);
    }

    func_80837268(this, &sp34, &sp32, 0.018f, globalCtx);

    if (!func_80837348(globalCtx, this, D_80854414, 1)) {
        if (sp34 != 0.0f) {
            this->actor.shape.rot.y = sp32;
            func_8083C858(this, globalCtx);
        }
        else if (Math_ScaledStepToS(&this->actor.shape.rot.y, sp32, this->unk_87E)) {
            func_8083C0E8(this, globalCtx);
        }

        this->currentYaw = this->actor.shape.rot.y;
    }
}

void func_80841CC4(Player* this, s32 arg1, GlobalContext* globalCtx) {
    LinkAnimationHeader* anim;
    s16 target;
    f32 rate;

    if (ABS(D_80853610) < 3640) {
        target = 0;
    }
    else {
        target = CLAMP(D_80853610, -10922, 10922);
    }

    Math_ScaledStepToS(&this->unk_89C, target, 400);

    if ((this->modelAnimType == PLAYER_ANIMTYPE_3) || ((this->unk_89C == 0) && (this->unk_6C4 <= 0.0f))) {
        if (arg1 == 0) {
            LinkAnimation_LoadToJoint(globalCtx, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_1][this->modelAnimType],
                this->unk_868);
        }
        else {
            LinkAnimation_LoadToMorph(globalCtx, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_1][this->modelAnimType],
                this->unk_868);
        }
        return;
    }

    if (this->unk_89C != 0) {
        rate = this->unk_89C / 10922.0f;
    }
    else {
        rate = this->unk_6C4 * 0.0006f;
    }

    rate *= fabsf(this->linearVelocity) * 0.5f;

    if (rate > 1.0f) {
        rate = 1.0f;
    }

    if (rate < 0.0f) {
        anim = &gPlayerAnim_002E48;
        rate = -rate;
    }
    else {
        anim = &gPlayerAnim_002E90;
    }

    if (arg1 == 0) {
        LinkAnimation_BlendToJoint(globalCtx, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_1][this->modelAnimType],
            this->unk_868, anim, this->unk_868, rate, this->blendTable);
    }
    else {
        LinkAnimation_BlendToMorph(globalCtx, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_1][this->modelAnimType],
            this->unk_868, anim, this->unk_868, rate, this->blendTable);
    }
}

void func_80841EE4(Player* this, GlobalContext* globalCtx) {
    f32 temp1;
    f32 temp2;

    if (this->unk_864 < 1.0f) {
        temp1 = R_UPDATE_RATE * 0.5f;

        func_8084029C(this, REG(35) / 1000.0f);
        LinkAnimation_LoadToJoint(globalCtx, &this->skelAnime, D_80853914[PLAYER_ANIMGROUP_1][this->modelAnimType],
            this->unk_868);

        this->unk_864 += 1 * temp1;
        if (this->unk_864 >= 1.0f) {
            this->unk_864 = 1.0f;
        }

        temp1 = this->unk_864;
    }
    else {
        temp2 = this->linearVelocity - (REG(48) / 100.0f);

        if (temp2 < 0.0f) {
            temp1 = 1.0f;
            func_8084029C(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));

            func_80841CC4(this, 0, globalCtx);
        }
        else {
            temp1 = (REG(37) / 1000.0f) * temp2;
            if (temp1 < 1.0f) {
                func_8084029C(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));
            }
            else {
                temp1 = 1.0f;
                func_8084029C(this, 1.2f + ((REG(38) / 1000.0f) * temp2));
            }

            func_80841CC4(this, 1, globalCtx);

            LinkAnimation_LoadToJoint(globalCtx, &this->skelAnime, func_80833438(this),
                this->unk_868 * (20.0f / 29.0f));
        }
    }

    if (temp1 < 1.0f) {
        LinkAnimation_InterpJointMorph(globalCtx, &this->skelAnime, 1.0f - temp1);
    }
}

void func_80842180(Player* this, GlobalContext* globalCtx) {
    f32 sp2C;
    s16 sp2A;

    this->stateFlags2 |= PLAYER_STATE2_5;
    func_80841EE4(this, globalCtx);

    if (!func_80837348(globalCtx, this, D_80854424, 1)) {
        if (func_80833C04(this)) {
            func_8083C858(this, globalCtx);
            return;
        }

        func_80837268(this, &sp2C, &sp2A, 0.018f, globalCtx);

        if (!func_8083C484(this, &sp2C, &sp2A)) {
            if (CVar_GetS32("gMMBunnyHood", 0) != 0 && this->currentMask == PLAYER_MASK_BUNNY) {
                sp2C *= 1.5f;
            }
            func_8083DF68(this, sp2C, sp2A);
            func_8083DDC8(this, globalCtx);

            if ((this->linearVelocity == 0.0f) && (sp2C == 0.0f)) {
                func_8083C0B8(this, globalCtx);
            }
        }
    }
}

void func_8084227C(Player* this, GlobalContext* globalCtx) {
    f32 sp2C;
    s16 sp2A;

    this->stateFlags2 |= PLAYER_STATE2_5;
    func_80841EE4(this, globalCtx);

    if (!func_80837348(globalCtx, this, D_80854430, 1)) {
        if (!func_80833C04(this)) {
            func_8083C858(this, globalCtx);
            return;
        }

        func_80837268(this, &sp2C, &sp2A, 0.0f, globalCtx);

        if (!func_8083C484(this, &sp2C, &sp2A)) {
            if ((func_80833B2C(this) && (sp2C != 0.0f) && (func_8083FD78(this, &sp2C, &sp2A, globalCtx) <= 0)) ||
                (!func_80833B2C(this) && (func_8083FC68(this, sp2C, sp2A) <= 0))) {
                func_80839F90(this, globalCtx);
                return;
            }

            func_8083DF68(this, sp2C, sp2A);
            func_8083DDC8(this, globalCtx);

            if ((this->linearVelocity == 0) && (sp2C == 0)) {
                func_80839F90(this, globalCtx);
            }
        }
    }
}

void func_808423EC(Player* this, GlobalContext* globalCtx) {
    s32 sp34;
    f32 sp30;
    s16 sp2E;

    sp34 = LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (!func_80837348(globalCtx, this, D_80854408, 1)) {
        if (!func_80833C04(this)) {
            func_8083C858(this, globalCtx);
            return;
        }

        func_80837268(this, &sp30, &sp2E, 0.0f, globalCtx);

        if ((this->skelAnime.morphWeight == 0.0f) && (this->skelAnime.curFrame > 5.0f)) {
            func_8083721C(this);

            if ((this->skelAnime.curFrame > 10.0f) && (func_8083FC68(this, sp30, sp2E) < 0)) {
                func_8083CBF0(this, sp2E, globalCtx);
                return;
            }

            if (sp34 != 0) {
                func_8083CD00(this, globalCtx);
            }
        }
    }
}

void func_8084251C(Player* this, GlobalContext* globalCtx) {
    s32 sp34;
    f32 sp30;
    s16 sp2E;

    sp34 = LinkAnimation_Update(globalCtx, &this->skelAnime);

    func_8083721C(this);

    if (!func_80837348(globalCtx, this, D_80854440, 1)) {
        func_80837268(this, &sp30, &sp2E, 0.0f, globalCtx);

        if (this->linearVelocity == 0.0f) {
            this->currentYaw = this->actor.shape.rot.y;

            if (func_8083FC68(this, sp30, sp2E) > 0) {
                func_8083C858(this, globalCtx);
                return;
            }

            if ((sp30 != 0.0f) || (sp34 != 0)) {
                func_80839F90(this, globalCtx);
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

s32 func_8084269C(GlobalContext* globalCtx, Player* this) {
    Vec3f sp2C;

    if ((this->unk_89E == 0) || (this->unk_89E == 1)) {
        func_8084260C(&this->actor.shape.feetPos[FOOT_LEFT], &sp2C,
            this->actor.floorHeight - this->actor.shape.feetPos[FOOT_LEFT].y, 7.0f, 5.0f);
        func_800286CC(globalCtx, &sp2C, &D_808545B4, &D_808545C0, 50, 30);
        func_8084260C(&this->actor.shape.feetPos[FOOT_RIGHT], &sp2C,
            this->actor.floorHeight - this->actor.shape.feetPos[FOOT_RIGHT].y, 7.0f, 5.0f);
        func_800286CC(globalCtx, &this->actor.shape.feetPos[FOOT_RIGHT], &D_808545B4, &D_808545C0, 50, 30);
        return 1;
    }

    return 0;
}

void func_8084279C(Player* this, GlobalContext* globalCtx) {
    func_80832CB0(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_33][this->modelAnimType]);

    if (DECR(this->unk_850) == 0) {
        if (!func_8083B040(this, globalCtx)) {
            func_8083A098(this, D_80853914[PLAYER_ANIMGROUP_34][this->modelAnimType], globalCtx);
        }

        this->actor.flags &= ~ACTOR_FLAG_8;
        func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
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

s32 func_808428D8(Player* this, GlobalContext* globalCtx) {
    if (!Player_IsChildWithHylianShield(this) && Player_GetSwordHeld(this) && D_80853614) {
        func_80832264(globalCtx, this, &gPlayerAnim_002EC8);
        this->unk_84F = 1;
        this->swordAnimation = 0xC;
        this->currentYaw = this->actor.shape.rot.y + this->unk_6BE;
        return 1;
    }

    return 0;
}

s32 func_80842964(Player* this, GlobalContext* globalCtx) {
    return func_8083B040(this, globalCtx) || func_8083B644(this, globalCtx) || func_8083E5A8(this, globalCtx);
}

void func_808429B4(GlobalContext* globalCtx, s32 speed, s32 y, s32 countdown) {
    s32 quakeIdx = Quake_Add(Gameplay_GetCamera(globalCtx, 0), 3);

    Quake_SetSpeed(quakeIdx, speed);
    Quake_SetQuakeValues(quakeIdx, y, 0, 0, 0);
    Quake_SetCountdown(quakeIdx, countdown);
}

void func_80842A28(GlobalContext* globalCtx, Player* this) {
    func_808429B4(globalCtx, 27767, 7, 20);
    globalCtx->actorCtx.unk_02 = 4;
    func_8083264C(this, 255, 20, 150, 0);
    func_8002F7DC(&this->actor, NA_SE_IT_HAMMER_HIT);
}

void func_80842A88(GlobalContext* globalCtx, Player* this) {
    Inventory_ChangeAmmo(ITEM_STICK, -1);
    func_80835F44(globalCtx, this, ITEM_NONE);
}

s32 func_80842AC4(GlobalContext* globalCtx, Player* this) {
    if ((this->heldItemActionParam == PLAYER_AP_STICK) && (this->unk_85C > 0.5f)) {
        if (AMMO(ITEM_STICK) != 0) {
            EffectSsStick_Spawn(globalCtx, &this->bodyPartsPos[PLAYER_BODYPART_R_HAND],
                this->actor.shape.rot.y + 0x8000);
            this->unk_85C = 0.5f;
            func_80842A88(globalCtx, this);
            func_8002F7DC(&this->actor, NA_SE_IT_WOODSTICK_BROKEN);
        }

        return 1;
    }

    return 0;
}

s32 func_80842B7C(GlobalContext* globalCtx, Player* this) {
    if (this->heldItemActionParam == PLAYER_AP_SWORD_BGS) {
        if (!gSaveContext.bgsFlag && (gSaveContext.swordHealth > 0.0f)) {
            if ((gSaveContext.swordHealth -= 1.0f) <= 0.0f) {
                EffectSsStick_Spawn(globalCtx, &this->bodyPartsPos[PLAYER_BODYPART_R_HAND],
                    this->actor.shape.rot.y + 0x8000);
                func_800849EC(globalCtx);
                func_8002F7DC(&this->actor, NA_SE_IT_MAJIN_SWORD_BROKEN);
            }
        }

        return 1;
    }

    return 0;
}

void func_80842CF0(GlobalContext* globalCtx, Player* this) {
    func_80842AC4(globalCtx, this);
    func_80842B7C(globalCtx, this);
}

static LinkAnimationHeader* D_808545CC[] = {
    &gPlayerAnim_002B10,
    &gPlayerAnim_002B20,
    &gPlayerAnim_002B08,
    &gPlayerAnim_002B18,
};

void func_80842D20(GlobalContext* globalCtx, Player* this) {
    s32 pad;
    s32 sp28;

    if (func_80843188 != this->func_674) {
        func_80832440(globalCtx, this);
        func_80835C58(globalCtx, this, func_808505DC, 0);

        if (func_8008E9C4(this)) {
            sp28 = 2;
        }
        else {
            sp28 = 0;
        }

        func_808322D0(globalCtx, this, D_808545CC[Player_HoldsTwoHandedWeapon(this) + sp28]);
    }

    func_8083264C(this, 180, 20, 100, 0);
    this->linearVelocity = -18.0f;
    func_80842CF0(globalCtx, this);
}

s32 func_80842DF4(GlobalContext* globalCtx, Player* this) {
    f32 phi_f2;
    CollisionPoly* sp78;
    s32 sp74;
    Vec3f sp68;
    Vec3f sp5C;
    Vec3f sp50;
    s32 temp1;
    s32 sp48;

    if (this->swordState > 0) {
        if (this->swordAnimation < 0x18) {
            if (!(this->swordQuads[0].base.atFlags & AT_BOUNCED) && !(this->swordQuads[1].base.atFlags & AT_BOUNCED)) {
                if (this->skelAnime.curFrame >= 2.0f) {

                    phi_f2 = Math_Vec3f_DistXYZAndStoreDiff(&this->swordInfo[0].tip, &this->swordInfo[0].base, &sp50);
                    if (phi_f2 != 0.0f) {
                        phi_f2 = (phi_f2 + 10.0f) / phi_f2;
                    }

                    sp68.x = this->swordInfo[0].tip.x + (sp50.x * phi_f2);
                    sp68.y = this->swordInfo[0].tip.y + (sp50.y * phi_f2);
                    sp68.z = this->swordInfo[0].tip.z + (sp50.z * phi_f2);

                    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &sp68, &this->swordInfo[0].tip, &sp5C, &sp78, true,
                        false, false, true, &sp74) &&
                        !SurfaceType_IsIgnoredByEntities(&globalCtx->colCtx, sp78, sp74) &&
                        (func_80041D4C(&globalCtx->colCtx, sp78, sp74) != 6) &&
                        (func_8002F9EC(globalCtx, &this->actor, sp78, sp74, &sp5C) == 0)) {

                        if (this->heldItemActionParam == PLAYER_AP_HAMMER) {
                            func_80832630(globalCtx);
                            func_80842A28(globalCtx, this);
                            func_80842D20(globalCtx, this);
                            return 1;
                        }

                        if (this->linearVelocity >= 0.0f) {
                            sp48 = func_80041F10(&globalCtx->colCtx, sp78, sp74);

                            if (sp48 == 0xA) {
                                CollisionCheck_SpawnShieldParticlesWood(globalCtx, &sp5C, &this->actor.projectedPos);
                            }
                            else {
                                CollisionCheck_SpawnShieldParticles(globalCtx, &sp5C);
                                if (sp48 == 0xB) {
                                    func_8002F7DC(&this->actor, NA_SE_IT_WALL_HIT_SOFT);
                                }
                                else {
                                    func_8002F7DC(&this->actor, NA_SE_IT_WALL_HIT_HARD);
                                }
                            }

                            func_80842CF0(globalCtx, this);
                            this->linearVelocity = -14.0f;
                            func_8083264C(this, 180, 20, 100, 0);
                        }
                    }
                }
            }
            else {
                func_80842D20(globalCtx, this);
                func_80832630(globalCtx);
                return 1;
            }
        }

        temp1 = (this->swordQuads[0].base.atFlags & AT_HIT) || (this->swordQuads[1].base.atFlags & AT_HIT);

        if (temp1) {
            if (this->swordAnimation < 0x18) {
                Actor* at = this->swordQuads[temp1 ? 1 : 0].base.at;

                if ((at != NULL) && (at->id != ACTOR_EN_KANBAN)) {
                    func_80832630(globalCtx);
                }
            }

            if ((func_80842AC4(globalCtx, this) == 0) && (this->heldItemActionParam != PLAYER_AP_HAMMER)) {
                func_80842B7C(globalCtx, this);

                if (this->actor.colChkInfo.atHitEffect == 1) {
                    this->actor.colChkInfo.damage = 8;
                    func_80837C0C(globalCtx, this, 4, 0.0f, 0.0f, this->actor.shape.rot.y, 20);
                    return 1;
                }
            }
        }
    }

    return 0;
}

void func_80843188(Player* this, GlobalContext* globalCtx) {
    f32 sp54;
    f32 sp50;
    s16 sp4E;
    s16 sp4C;
    s16 sp4A;
    s16 sp48;
    s16 sp46;
    f32 sp40;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (!Player_IsChildWithHylianShield(this)) {
            func_80832284(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_21][this->modelAnimType]);
        }
        this->unk_850 = 1;
        this->unk_84F = 0;
    }

    if (!Player_IsChildWithHylianShield(this)) {
        this->stateFlags1 |= PLAYER_STATE1_22;
        func_80836670(this, globalCtx);
        this->stateFlags1 &= ~PLAYER_STATE1_22;
    }

    func_8083721C(this);

    if (this->unk_850 != 0) {
        sp54 = sControlInput->rel.stick_y * 100;
        sp50 = sControlInput->rel.stick_x * -120;
        sp4E = this->actor.shape.rot.y - Camera_GetInputDirYaw(GET_ACTIVE_CAM(globalCtx));

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

        sp46 = ABS(sp4A - this->unk_6BE) * 0.25f;
        if (sp46 < 50) {
            sp46 = 50;
        }

        Math_ScaledStepToS(&this->actor.focus.rot.x, sp4C, sp48);
        this->unk_6BC = this->actor.focus.rot.x;
        Math_ScaledStepToS(&this->unk_6BE, sp4A, sp46);

        if (this->unk_84F != 0) {
            if (!func_80842DF4(globalCtx, this)) {
                if (this->skelAnime.curFrame < 2.0f) {
                    func_80833A20(this, 1);
                }
            }
            else {
                this->unk_850 = 1;
                this->unk_84F = 0;
            }
        }
        else if (!func_80842964(this, globalCtx)) {
            if (func_8083C2B0(this, globalCtx)) {
                func_808428D8(this, globalCtx);
            }
            else {
                this->stateFlags1 &= ~PLAYER_STATE1_22;
                func_80832318(this);

                if (Player_IsChildWithHylianShield(this)) {
                    func_8083A060(this, globalCtx);
                    LinkAnimation_Change(globalCtx, &this->skelAnime, &gPlayerAnim_002400, 1.0f,
                        Animation_GetLastFrame(&gPlayerAnim_002400), 0.0f, ANIMMODE_ONCE, 0.0f);
                    func_80832F54(globalCtx, this, 4);
                }
                else {
                    if (this->itemActionParam < 0) {
                        func_8008EC70(this);
                    }
                    func_8083A098(this, D_80853914[PLAYER_ANIMGROUP_22][this->modelAnimType], globalCtx);
                }

                func_8002F7DC(&this->actor, NA_SE_IT_SHIELD_REMOVE);
                return;
            }
        }
        else {
            return;
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_22;
    Player_SetModelsForHoldingShield(this);

    this->unk_6AE |= 0xC1;
}

void func_808435C4(Player* this, GlobalContext* globalCtx) {
    s32 temp;
    LinkAnimationHeader* anim;
    f32 frames;

    func_8083721C(this);

    if (this->unk_84F == 0) {
        D_808535E0 = func_80836670(this, globalCtx);
        if ((func_80834B5C == this->func_82C) || (func_808374A0(globalCtx, this, &this->skelAnime2, 4.0f) > 0)) {
            func_80835C58(globalCtx, this, func_80840450, 1);
        }
    }
    else {
        temp = func_808374A0(globalCtx, this, &this->skelAnime, 4.0f);
        if ((temp != 0) && ((temp > 0) || LinkAnimation_Update(globalCtx, &this->skelAnime))) {
            func_80835C58(globalCtx, this, func_80843188, 1);
            this->stateFlags1 |= PLAYER_STATE1_22;
            Player_SetModelsForHoldingShield(this);
            anim = D_80853914[PLAYER_ANIMGROUP_20][this->modelAnimType];
            frames = Animation_GetLastFrame(anim);
            LinkAnimation_Change(globalCtx, &this->skelAnime, anim, 1.0f, frames, frames, ANIMMODE_ONCE, 0.0f);
        }
    }
}

void func_8084370C(Player* this, GlobalContext* globalCtx) {
    s32 sp1C;

    func_8083721C(this);

    sp1C = func_808374A0(globalCtx, this, &this->skelAnime, 16.0f);
    if ((sp1C != 0) && (LinkAnimation_Update(globalCtx, &this->skelAnime) || (sp1C > 0))) {
        func_80839F90(this, globalCtx);
    }
}

void func_8084377C(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;

    func_808382BC(this);

    if (!(this->stateFlags1 & PLAYER_STATE1_29) && (this->unk_850 == 0) && (this->unk_8A1 != 0)) {
        s16 temp = this->actor.shape.rot.y - this->unk_8A2;

        this->currentYaw = this->actor.shape.rot.y = this->unk_8A2;
        this->linearVelocity = this->unk_8A4;

        if (ABS(temp) > 0x4000) {
            this->actor.shape.rot.y = this->unk_8A2 + 0x8000;
        }

        if (this->actor.velocity.y < 0.0f) {
            this->actor.gravity = 0.0f;
            this->actor.velocity.y = 0.0f;
        }
    }

    if (LinkAnimation_Update(globalCtx, &this->skelAnime) && (this->actor.bgCheckFlags & 1)) {
        if (this->unk_850 != 0) {
            this->unk_850--;
            if (this->unk_850 == 0) {
                func_80853080(this, globalCtx);
            }
        }
        else if ((this->stateFlags1 & PLAYER_STATE1_29) ||
            (!(this->cylinder.base.acFlags & AC_HIT) && (this->unk_8A1 == 0))) {
            if (this->stateFlags1 & PLAYER_STATE1_29) {
                this->unk_850++;
            }
            else {
                func_80835C58(globalCtx, this, func_80843954, 0);
                this->stateFlags1 |= PLAYER_STATE1_26;
            }

            func_80832264(globalCtx, this,
                (this->currentYaw != this->actor.shape.rot.y) ? &gPlayerAnim_002F60 : &gPlayerAnim_002DB8);
            func_80832698(this, NA_SE_VO_LI_FREEZE);
        }
    }

    if (this->actor.bgCheckFlags & 2) {
        func_80832770(this, NA_SE_PL_BOUND);
    }
}

void func_80843954(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;
    func_808382BC(this);

    func_8083721C(this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime) && (this->linearVelocity == 0.0f)) {
        if (this->stateFlags1 & PLAYER_STATE1_29) {
            this->unk_850++;
        }
        else {
            func_80835C58(globalCtx, this, func_80843A38, 0);
            this->stateFlags1 |= PLAYER_STATE1_26;
        }

        func_808322D0(globalCtx, this,
            (this->currentYaw != this->actor.shape.rot.y) ? &gPlayerAnim_002F68 : &gPlayerAnim_002DC0);
        this->currentYaw = this->actor.shape.rot.y;
    }
}

static struct_80832924 D_808545DC[] = {
    { 0, 0x4014 },
    { 0, -0x401E },
};

void func_80843A38(Player* this, GlobalContext* globalCtx) {
    s32 sp24;

    this->stateFlags2 |= PLAYER_STATE2_5;
    func_808382BC(this);

    if (this->stateFlags1 & PLAYER_STATE1_29) {
        LinkAnimation_Update(globalCtx, &this->skelAnime);
    }
    else {
        sp24 = func_808374A0(globalCtx, this, &this->skelAnime, 16.0f);
        if ((sp24 != 0) && (LinkAnimation_Update(globalCtx, &this->skelAnime) || (sp24 > 0))) {
            func_80839F90(this, globalCtx);
        }
    }

    func_80832924(this, D_808545DC);
}

static Vec3f D_808545E4 = { 0.0f, 0.0f, 5.0f };

void func_80843AE8(GlobalContext* globalCtx, Player* this) {
    if (this->unk_850 != 0) {
        if (this->unk_850 > 0) {
            this->unk_850--;
            if (this->unk_850 == 0) {
                if (this->stateFlags1 & PLAYER_STATE1_27) {
                    LinkAnimation_Change(globalCtx, &this->skelAnime, &gPlayerAnim_003328, 1.0f, 0.0f,
                        Animation_GetLastFrame(&gPlayerAnim_003328), ANIMMODE_ONCE, -16.0f);
                }
                else {
                    LinkAnimation_Change(globalCtx, &this->skelAnime, &gPlayerAnim_002878, 1.0f, 99.0f,
                        Animation_GetLastFrame(&gPlayerAnim_002878), ANIMMODE_ONCE, 0.0f);
                }
                gSaveContext.healthAccumulator = 0x140;
                this->unk_850 = -1;
            }
        }
        else if (gSaveContext.healthAccumulator == 0) {
            this->stateFlags1 &= ~PLAYER_STATE1_7;
            if (this->stateFlags1 & PLAYER_STATE1_27) {
                func_80838F18(globalCtx, this);
            }
            else {
                func_80853080(this, globalCtx);
            }
            this->unk_A87 = 20;
            func_80837AFC(this, -20);
            func_800F47FC();
        }
    }
    else if (this->unk_84F != 0) {
        this->unk_850 = 60;
        Player_SpawnFairy(globalCtx, this, &this->actor.world.pos, &D_808545E4, FAIRY_REVIVE_DEATH);
        func_8002F7DC(&this->actor, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
        OnePointCutscene_Init(globalCtx, 9908, 125, &this->actor, MAIN_CAM);
    }
    else if (globalCtx->gameOverCtx.state == GAMEOVER_DEATH_WAIT_GROUND) {
        globalCtx->gameOverCtx.state = GAMEOVER_DEATH_DELAY_MENU;
    }
}

static struct_80832924 D_808545F0[] = {
    { NA_SE_PL_BOUND, 0x103C },
    { 0, 0x408C },
    { 0, 0x40A4 },
    { 0, -0x40AA },
};

void func_80843CEC(Player* this, GlobalContext* globalCtx) {
    if (this->currentTunic != PLAYER_TUNIC_GORON && CVar_GetS32("gSuperTunic", 0) == 0) {
        if ((globalCtx->roomCtx.curRoom.unk_02 == 3) || (D_808535E4 == 9) ||
            ((func_80838144(D_808535E4) >= 0) &&
                !SurfaceType_IsWallDamage(&globalCtx->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            func_8083821C(this);
        }
    }

    func_8083721C(this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (this->actor.category == ACTORCAT_PLAYER) {
            func_80843AE8(globalCtx, this);
        }
        return;
    }

    if (this->skelAnime.animation == &gPlayerAnim_002878) {
        func_80832924(this, D_808545F0);
    }
    else if (this->skelAnime.animation == &gPlayerAnim_002F08) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 88.0f)) {
            func_80832770(this, NA_SE_PL_BOUND);
        }
    }
}

void func_80843E14(Player* this, u16 sfxId) {
    func_80832698(this, sfxId);

    if ((this->heldActor != NULL) && (this->heldActor->id == ACTOR_EN_RU1)) {
        Audio_PlayActorSound2(this->heldActor, NA_SE_VO_RT_FALL);
    }
}

static FallImpactInfo D_80854600[] = {
    { -8, 180, 40, 100, NA_SE_VO_LI_LAND_DAMAGE_S },
    { -16, 255, 140, 150, NA_SE_VO_LI_LAND_DAMAGE_S },
};

s32 func_80843E64(GlobalContext* globalCtx, Player* this) {
    s32 sp34;

    if ((D_808535E4 == 6) || (D_808535E4 == 9)) {
        sp34 = 0;
    }
    else {
        sp34 = this->fallDistance;
    }

    Math_StepToF(&this->linearVelocity, 0.0f, 1.0f);

    this->stateFlags1 &= ~(PLAYER_STATE1_18 | PLAYER_STATE1_19);

    if (sp34 >= 400) {
        s32 impactIndex;
        FallImpactInfo* impactInfo;

        if (this->fallDistance < 800) {
            impactIndex = 0;
        }
        else {
            impactIndex = 1;
        }

        impactInfo = &D_80854600[impactIndex];

        if (Player_InflictDamage(globalCtx, impactInfo->damage)) {
            return -1;
        }

        func_80837AE0(this, 40);
        func_808429B4(globalCtx, 32967, 2, 30);
        func_8083264C(this, impactInfo->unk_01, impactInfo->unk_02, impactInfo->unk_03, 0);
        func_8002F7DC(&this->actor, NA_SE_PL_BODY_HIT);
        func_80832698(this, impactInfo->sfxId);

        return impactIndex + 1;
    }

    if (sp34 > 200) {
        sp34 *= 2;

        if (sp34 > 255) {
            sp34 = 255;
        }

        func_8083264C(this, (u8)sp34, (u8)(sp34 * 0.1f), (u8)sp34, 0);

        if (D_808535E4 == 6) {
            func_80832698(this, NA_SE_VO_LI_CLIMB_END);
        }
    }

    func_808328A0(this);

    return 0;
}

void func_8084409C(GlobalContext* globalCtx, Player* this, f32 speedXZ, f32 velocityY) {
    Actor* heldActor = this->heldActor;

    if (!func_80835644(globalCtx, this, heldActor)) {
        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speedXZ = speedXZ;
        heldActor->velocity.y = velocityY;
        func_80834644(globalCtx, this);
        func_8002F7DC(&this->actor, NA_SE_PL_THROW);
        func_80832698(this, NA_SE_VO_LI_SWORD_N);
    }
}

void func_8084411C(Player* this, GlobalContext* globalCtx) {
    f32 sp4C;
    s16 sp4A;

    if (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 40) {
        this->actor.gravity = 0.0f;
    }
    else if (func_8008E9C4(this)) {
        this->actor.gravity = -1.2f;
    }

    func_80837268(this, &sp4C, &sp4A, 0.0f, globalCtx);

    if (!(this->actor.bgCheckFlags & 1)) {
        if (this->stateFlags1 & PLAYER_STATE1_11) {
            Actor* heldActor = this->heldActor;

            if (!func_80835644(globalCtx, this, heldActor) && (heldActor->id == ACTOR_EN_NIW) &&
                CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN)) {
                func_8084409C(globalCtx, this, this->linearVelocity + 2.0f, this->actor.velocity.y + 2.0f);
            }
        }

        LinkAnimation_Update(globalCtx, &this->skelAnime);

        if (!(this->stateFlags2 & PLAYER_STATE2_19)) {
            func_8083DFE0(this, &sp4C, &sp4A);
        }

        func_80836670(this, globalCtx);

        if (((this->stateFlags2 & PLAYER_STATE2_19) && (this->unk_84F == 2)) || !func_8083BBA0(this, globalCtx)) {
            if (this->actor.velocity.y < 0.0f) {
                if (this->unk_850 >= 0) {
                    if ((this->actor.bgCheckFlags & 8) || (this->unk_850 == 0) || (this->fallDistance > 0)) {
                        if ((D_80853600 > 800.0f) || (this->stateFlags1 & PLAYER_STATE1_2)) {
                            func_80843E14(this, NA_SE_VO_LI_FALL_S);
                            this->stateFlags1 &= ~PLAYER_STATE1_2;
                        }

                        LinkAnimation_Change(globalCtx, &this->skelAnime, &gPlayerAnim_003020, 1.0f, 0.0f, 0.0f,
                            ANIMMODE_ONCE, 8.0f);
                        this->unk_850 = -1;
                    }
                }
                else {
                    if ((this->unk_850 == -1) && (this->fallDistance > 120.0f) && (D_80853600 > 280.0f)) {
                        this->unk_850 = -2;
                        func_80843E14(this, NA_SE_VO_LI_FALL_L);
                    }

                    if ((this->actor.bgCheckFlags & 0x200) && !(this->stateFlags2 & PLAYER_STATE2_19) &&
                        !(this->stateFlags1 & (PLAYER_STATE1_11 | PLAYER_STATE1_27)) && (this->linearVelocity > 0.0f)) {
                        if ((this->wallHeight >= 150.0f) && (this->unk_84B[this->unk_846] == 0)) {
                            func_8083EC18(this, globalCtx, D_808535F0);
                        }
                        else if ((this->unk_88C >= 2) && (this->wallHeight < 150.0f) &&
                            (((this->actor.world.pos.y - this->actor.floorHeight) + this->wallHeight) >
                                (70.0f * this->ageProperties->unk_08))) {
                            AnimationContext_DisableQueue(globalCtx);
                            if (this->stateFlags1 & PLAYER_STATE1_2) {
                                func_80832698(this, NA_SE_VO_LI_HOOKSHOT_HANG);
                            }
                            else {
                                func_80832698(this, NA_SE_VO_LI_HANG);
                            }
                            this->actor.world.pos.y += this->wallHeight;
                            func_8083A5C4(globalCtx, this, this->actor.wallPoly, this->wallDistance,
                                D_80853914[PLAYER_ANIMGROUP_39][this->modelAnimType]);
                            this->actor.shape.rot.y = this->currentYaw += 0x8000;
                            this->stateFlags1 |= PLAYER_STATE1_13;
                        }
                    }
                }
            }
        }
    }
    else {
        LinkAnimationHeader* anim = D_80853914[PLAYER_ANIMGROUP_14][this->modelAnimType];
        s32 sp3C;

        if (this->stateFlags2 & PLAYER_STATE2_19) {
            if (func_8008E9C4(this)) {
                anim = D_80853D4C[this->unk_84F][2];
            }
            else {
                anim = D_80853D4C[this->unk_84F][1];
            }
        }
        else if (this->skelAnime.animation == &gPlayerAnim_003148) {
            anim = &gPlayerAnim_003150;
        }
        else if (func_8008E9C4(this)) {
            anim = &gPlayerAnim_002538;
            func_80833C3C(this);
        }
        else if (this->fallDistance <= 80) {
            anim = D_80853914[PLAYER_ANIMGROUP_15][this->modelAnimType];
        }
        else if ((this->fallDistance < 800) && (this->unk_84B[this->unk_846] == 0) &&
            !(this->stateFlags1 & PLAYER_STATE1_11)) {
            func_8083BC04(this, globalCtx);
            return;
        }

        sp3C = func_80843E64(globalCtx, this);

        if (sp3C > 0) {
            func_8083A098(this, D_80853914[PLAYER_ANIMGROUP_14][this->modelAnimType], globalCtx);
            this->skelAnime.endFrame = 8.0f;
            if (sp3C == 1) {
                this->unk_850 = 10;
            }
            else {
                this->unk_850 = 20;
            }
        }
        else if (sp3C == 0) {
            func_8083A098(this, anim, globalCtx);
        }
    }
}

static struct_80832924 D_8085460C[] = {
    { NA_SE_VO_LI_SWORD_N, 0x2001 },
    { NA_SE_PL_WALK_GROUND, 0x1806 },
    { NA_SE_PL_ROLL, 0x806 },
    { 0, -0x2812 },
};

void func_80844708(Player* this, GlobalContext* globalCtx) {
    Actor* cylinderOc;
    s32 temp;
    s32 sp44;
    DynaPolyActor* wallPolyActor;
    s32 pad;
    f32 sp38;
    s16 sp36;

    this->stateFlags2 |= PLAYER_STATE2_5;

    cylinderOc = NULL;
    sp44 = LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 8.0f)) {
        func_80837AFC(this, -10);
    }

    if (func_80842964(this, globalCtx) == 0) {
        if (this->unk_850 != 0) {
            Math_StepToF(&this->linearVelocity, 0.0f, 2.0f);

            temp = func_808374A0(globalCtx, this, &this->skelAnime, 5.0f);
            if ((temp != 0) && ((temp > 0) || sp44)) {
                func_8083A060(this, globalCtx);
            }
        }
        else {
            if (this->linearVelocity >= 7.0f) {
                if (((this->actor.bgCheckFlags & 0x200) && (D_8085360C < 0x2000)) ||
                    ((this->cylinder.base.ocFlags1 & OC1_HIT) &&
                        (cylinderOc = this->cylinder.base.oc,
                            ((cylinderOc->id == ACTOR_EN_WOOD02) &&
                                (ABS((s16)(this->actor.world.rot.y - cylinderOc->yawTowardsPlayer)) > 0x6000))))) {

                    if (cylinderOc != NULL) {
                        cylinderOc->home.rot.y = 1;
                    }
                    else if (this->actor.wallBgId != BGCHECK_SCENE) {
                        wallPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, this->actor.wallBgId);
                        if ((wallPolyActor != NULL) && (wallPolyActor->actor.id == ACTOR_OBJ_KIBAKO2)) {
                            wallPolyActor->actor.home.rot.z = 1;
                        }
                    }

                    func_80832264(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_17][this->modelAnimType]);
                    this->linearVelocity = -this->linearVelocity;
                    func_808429B4(globalCtx, 33267, 3, 12);
                    func_8083264C(this, 255, 20, 150, 0);
                    func_8002F7DC(&this->actor, NA_SE_PL_BODY_HIT);
                    func_80832698(this, NA_SE_VO_LI_CLIMB_END);
                    this->unk_850 = 1;
                    return;
                }
            }

            if ((this->skelAnime.curFrame < 15.0f) || !func_80850224(this, globalCtx)) {
                if (this->skelAnime.curFrame >= 20.0f) {
                    func_8083A060(this, globalCtx);
                    return;
                }

                func_80837268(this, &sp38, &sp36, 0.018f, globalCtx);

                sp38 *= 1.5f;
                if ((sp38 < 3.0f) || (this->unk_84B[this->unk_846] != 0)) {
                    sp38 = 3.0f;
                }

                func_8083DF68(this, sp38, this->actor.shape.rot.y);

                if (func_8084269C(globalCtx, this)) {
                    func_8002F8F0(&this->actor, NA_SE_PL_ROLL_DUST - SFX_FLAG);
                }

                func_80832924(this, D_8085460C);
            }
        }
    }
}

void func_80844A44(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_5;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80832284(globalCtx, this, &gPlayerAnim_003160);
    }

    Math_StepToF(&this->linearVelocity, 0.0f, 0.05f);

    if (this->actor.bgCheckFlags & 1) {
        this->actor.colChkInfo.damage = 0x10;
        func_80837C0C(globalCtx, this, 1, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
    }
}

void func_80844AF4(Player* this, GlobalContext* globalCtx) {
    f32 sp2C;
    s16 sp2A;

    this->stateFlags2 |= PLAYER_STATE2_5;

    this->actor.gravity = -1.2f;
    LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (!func_80842DF4(globalCtx, this)) {
        func_8084285C(this, 6.0f, 7.0f, 99.0f);

        if (!(this->actor.bgCheckFlags & 1)) {
            func_80837268(this, &sp2C, &sp2A, 0.0f, globalCtx);
            func_8083DFE0(this, &sp2C, &this->currentYaw);
            return;
        }

        if (func_80843E64(globalCtx, this) >= 0) {
            this->swordAnimation += 2;
            func_80837948(globalCtx, this, this->swordAnimation);
            this->unk_845 = 3;
            func_808328A0(this);
        }
    }
}

s32 func_80844BE4(Player* this, GlobalContext* globalCtx) {
    s32 temp;

    if (func_8083ADD4(globalCtx, this)) {
        this->stateFlags2 |= PLAYER_STATE2_17;
    }
    else {
        if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
            if ((this->unk_858 >= 0.85f) || func_808375D8(this)) {
                temp = D_80854384[Player_HoldsTwoHandedWeapon(this)];
            }
            else {
                temp = D_80854380[Player_HoldsTwoHandedWeapon(this)];
            }

            func_80837948(globalCtx, this, temp);
            func_80837AFC(this, -8);

            this->stateFlags2 |= PLAYER_STATE2_17;
            if (this->unk_84B[this->unk_846] == 0) {
                this->stateFlags2 |= PLAYER_STATE2_30;
            }
        }
        else {
            return 0;
        }
    }

    return 1;
}

void func_80844CF8(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_80845000, 1);
}

void func_80844D30(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_80845308, 1);
}

void func_80844D68(Player* this, GlobalContext* globalCtx) {
    func_80839FFC(this, globalCtx);
    func_80832318(this);
    func_80832B0C(globalCtx, this, D_80854368[Player_HoldsTwoHandedWeapon(this)]);
    this->currentYaw = this->actor.shape.rot.y;
}

void func_80844DC8(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_80844E68, 1);
    this->unk_868 = 0.0f;
    func_80832284(globalCtx, this, D_80854360[Player_HoldsTwoHandedWeapon(this)]);
    this->unk_850 = 1;
}

void func_80844E3C(Player* this) {
    Math_StepToF(&this->unk_858, 1.0f, 0.02f);
}

void func_80844E68(Player* this, GlobalContext* globalCtx) {
    f32 sp34;
    s16 sp32;
    s32 temp;

    this->stateFlags1 |= PLAYER_STATE1_12;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80832DBC(this);
        func_808355DC(this);
        this->stateFlags1 &= ~PLAYER_STATE1_17;
        func_80832284(globalCtx, this, D_80854360[Player_HoldsTwoHandedWeapon(this)]);
        this->unk_850 = -1;
    }

    func_8083721C(this);

    if (!func_80842964(this, globalCtx) && (this->unk_850 != 0)) {
        func_80844E3C(this);

        if (this->unk_850 < 0) {
            if (this->unk_858 >= 0.1f) {
                this->unk_845 = 0;
                this->unk_850 = 1;
            }
            else if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
                func_80844D68(this, globalCtx);
            }
        }
        else if (!func_80844BE4(this, globalCtx)) {
            func_80837268(this, &sp34, &sp32, 0.0f, globalCtx);

            temp = func_80840058(this, &sp34, &sp32, globalCtx);
            if (temp > 0) {
                func_80844CF8(this, globalCtx);
            }
            else if (temp < 0) {
                func_80844D30(this, globalCtx);
            }
        }
    }
}

void func_80845000(Player* this, GlobalContext* globalCtx) {
    s16 temp1;
    s32 temp2;
    f32 sp5C;
    f32 sp58;
    f32 sp54;
    s16 sp52;
    s32 temp4;
    s16 temp5;
    s32 sp44;

    temp1 = this->currentYaw - this->actor.shape.rot.y;
    temp2 = ABS(temp1);

    sp5C = fabsf(this->linearVelocity);
    sp58 = sp5C * 1.5f;

    this->stateFlags1 |= PLAYER_STATE1_12;

    if (sp58 < 1.5f) {
        sp58 = 1.5f;
    }

    sp58 = ((temp2 < 0x4000) ? -1.0f : 1.0f) * sp58;

    func_8084029C(this, sp58);

    sp58 = CLAMP(sp5C * 0.5f, 0.5f, 1.0f);

    LinkAnimation_BlendToJoint(globalCtx, &this->skelAnime, D_80854360[Player_HoldsTwoHandedWeapon(this)], 0.0f,
        D_80854370[Player_HoldsTwoHandedWeapon(this)], this->unk_868 * (21.0f / 29.0f), sp58,
        this->blendTable);

    if (!func_80842964(this, globalCtx) && !func_80844BE4(this, globalCtx)) {
        func_80844E3C(this);
        func_80837268(this, &sp54, &sp52, 0.0f, globalCtx);

        temp4 = func_80840058(this, &sp54, &sp52, globalCtx);

        if (temp4 < 0) {
            func_80844D30(this, globalCtx);
            return;
        }

        if (temp4 == 0) {
            sp54 = 0.0f;
            sp52 = this->currentYaw;
        }

        temp5 = sp52 - this->currentYaw;
        sp44 = ABS(temp5);

        if (sp44 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
                this->currentYaw = sp52;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, sp54 * 0.2f, 1.0f, 0.5f);
        Math_ScaledStepToS(&this->currentYaw, sp52, sp44 * 0.1f);

        if ((sp54 == 0.0f) && (this->linearVelocity == 0.0f)) {
            func_80844DC8(this, globalCtx);
        }
    }
}

void func_80845308(Player* this, GlobalContext* globalCtx) {
    f32 sp5C;
    f32 sp58;
    f32 sp54;
    s16 sp52;
    s32 temp4;
    s16 temp5;
    s32 sp44;

    sp5C = fabsf(this->linearVelocity);

    this->stateFlags1 |= PLAYER_STATE1_12;

    if (sp5C == 0.0f) {
        sp5C = ABS(this->unk_87C) * 0.0015f;
        if (sp5C < 400.0f) {
            sp5C = 0.0f;
        }
        func_8084029C(this, ((this->unk_87C >= 0) ? 1 : -1) * sp5C);
    }
    else {
        sp58 = sp5C * 1.5f;
        if (sp58 < 1.5f) {
            sp58 = 1.5f;
        }
        func_8084029C(this, sp58);
    }

    sp58 = CLAMP(sp5C * 0.5f, 0.5f, 1.0f);

    LinkAnimation_BlendToJoint(globalCtx, &this->skelAnime, D_80854360[Player_HoldsTwoHandedWeapon(this)], 0.0f,
        D_80854378[Player_HoldsTwoHandedWeapon(this)], this->unk_868 * (21.0f / 29.0f), sp58,
        this->blendTable);

    if (!func_80842964(this, globalCtx) && !func_80844BE4(this, globalCtx)) {
        func_80844E3C(this);
        func_80837268(this, &sp54, &sp52, 0.0f, globalCtx);

        temp4 = func_80840058(this, &sp54, &sp52, globalCtx);

        if (temp4 > 0) {
            func_80844CF8(this, globalCtx);
            return;
        }

        if (temp4 == 0) {
            sp54 = 0.0f;
            sp52 = this->currentYaw;
        }

        temp5 = sp52 - this->currentYaw;
        sp44 = ABS(temp5);

        if (sp44 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
                this->currentYaw = sp52;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, sp54 * 0.2f, 1.0f, 0.5f);
        Math_ScaledStepToS(&this->currentYaw, sp52, sp44 * 0.1f);

        if ((sp54 == 0.0f) && (this->linearVelocity == 0.0f) && (sp5C == 0.0f)) {
            func_80844DC8(this, globalCtx);
        }
    }
}

void func_80845668(Player* this, GlobalContext* globalCtx) {
    s32 sp3C;
    f32 temp1;
    s32 temp2;
    f32 temp3;

    this->stateFlags2 |= PLAYER_STATE2_5;
    sp3C = LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (this->skelAnime.animation == &gPlayerAnim_002D48) {
        this->linearVelocity = 1.0f;

        if (LinkAnimation_OnFrame(&this->skelAnime, 8.0f)) {
            temp1 = this->wallHeight;

            if (temp1 > this->ageProperties->unk_0C) {
                temp1 = this->ageProperties->unk_0C;
            }

            if (this->stateFlags1 & PLAYER_STATE1_27) {
                temp1 *= 0.085f;
            }
            else {
                temp1 *= 0.072f;
            }

            if (!LINK_IS_ADULT) {
                temp1 += 1.0f;
            }

            func_80838940(this, NULL, temp1, globalCtx, NA_SE_VO_LI_AUTO_JUMP);
            this->unk_850 = -1;
            return;
        }
    }
    else {
        temp2 = func_808374A0(globalCtx, this, &this->skelAnime, 4.0f);

        if (temp2 == 0) {
            this->stateFlags1 &= ~(PLAYER_STATE1_14 | PLAYER_STATE1_18);
            return;
        }

        if ((sp3C != 0) || (temp2 > 0)) {
            func_8083C0E8(this, globalCtx);
            this->stateFlags1 &= ~(PLAYER_STATE1_14 | PLAYER_STATE1_18);
            return;
        }

        temp3 = 0.0f;

        if (this->skelAnime.animation == &gPlayerAnim_0032E8) {
            if (LinkAnimation_OnFrame(&this->skelAnime, 30.0f)) {
                func_8083D0A8(globalCtx, this, 10.0f);
            }
            temp3 = 50.0f;
        }
        else if (this->skelAnime.animation == &gPlayerAnim_002D40) {
            temp3 = 30.0f;
        }
        else if (this->skelAnime.animation == &gPlayerAnim_002D38) {
            temp3 = 16.0f;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, temp3)) {
            func_808328A0(this);
            func_80832698(this, NA_SE_VO_LI_CLIMB_END);
        }

        if ((this->skelAnime.animation == &gPlayerAnim_002D38) || (this->skelAnime.curFrame > 5.0f)) {
            if (this->unk_850 == 0) {
                func_80832854(this);
                this->unk_850 = 1;
            }
            Math_StepToF(&this->actor.shape.yOffset, 0.0f, 150.0f);
        }
    }
}

void func_808458D0(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;
    LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (((this->stateFlags1 & PLAYER_STATE1_11) && (this->heldActor != NULL) && (this->getItemId == GI_NONE)) ||
        !func_80836670(this, globalCtx)) {
        this->func_A74(globalCtx, this);
    }
}

s32 func_80845964(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2, f32 arg3, s16 arg4, s32 arg5) {
    if ((arg5 != 0) && (this->linearVelocity == 0.0f)) {
        return LinkAnimation_Update(globalCtx, &this->skelAnime);
    }

    if (arg5 != 2) {
        f32 sp34 = R_UPDATE_RATE * 0.5f;
        f32 selfDistX = arg2->endPos.x - this->actor.world.pos.x;
        f32 selfDistZ = arg2->endPos.z - this->actor.world.pos.z;
        f32 sp28 = sqrtf(SQ(selfDistX) + SQ(selfDistZ)) / sp34;
        s32 sp24 = (arg2->endFrame - globalCtx->csCtx.frames) + 1;

        arg4 = Math_Atan2S(selfDistZ, selfDistX);

        if (arg5 == 1) {
            f32 distX = arg2->endPos.x - arg2->startPos.x;
            f32 distZ = arg2->endPos.z - arg2->startPos.z;
            s32 temp = (((sqrtf(SQ(distX) + SQ(distZ)) / sp34) / (arg2->endFrame - arg2->startFrame)) / 1.5f) * 4.0f;

            if (temp >= sp24) {
                arg4 = this->actor.shape.rot.y;
                arg3 = 0.0f;
            }
            else {
                arg3 = sp28 / ((sp24 - temp) + 1);
            }
        }
        else {
            arg3 = sp28 / sp24;
        }
    }

    this->stateFlags2 |= PLAYER_STATE2_5;
    func_80841EE4(this, globalCtx);
    func_8083DF68(this, arg3, arg4);

    if ((arg3 == 0.0f) && (this->linearVelocity == 0.0f)) {
        func_8083BF50(this, globalCtx);
    }

    return 0;
}

s32 func_80845BA0(GlobalContext* arg0, Player* arg1, f32* arg2, s32 arg3) {
    f32 dx = arg1->unk_450.x - arg1->actor.world.pos.x;
    f32 dz = arg1->unk_450.z - arg1->actor.world.pos.z;
    s32 sp2C = sqrtf(SQ(dx) + SQ(dz));
    s16 yaw = Math_Vec3f_Yaw(&arg1->actor.world.pos, &arg1->unk_450);

    if (sp2C < arg3) {
        *arg2 = 0.0f;
        yaw = arg1->actor.shape.rot.y;
    }

    if (func_80845964(arg0, arg1, NULL, *arg2, yaw, 2)) {
        return 0;
    }

    return sp2C;
}

s32 func_80845C68(GlobalContext* globalCtx, s32 arg1) {
    if (arg1 == 0) {
        Gameplay_SetupRespawnPoint(globalCtx, RESPAWN_MODE_DOWN, 0xDFF);
    }
    gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 0;
    return arg1;
}

void func_80845CA4(Player* this, GlobalContext* globalCtx) {
    f32 sp3C;
    s32 temp;
    f32 sp34;
    s32 sp30;
    s32 pad;

    if (!func_8083B040(this, globalCtx)) {
        if (this->unk_850 == 0) {
            LinkAnimation_Update(globalCtx, &this->skelAnime);

            if (DECR(this->doorTimer) == 0) {
                this->linearVelocity = 0.1f;
                this->unk_850 = 1;
            }
        }
        else if (this->unk_84F == 0) {
            sp3C = 5.0f * D_808535E8;

            if (func_80845BA0(globalCtx, this, &sp3C, -1) < 30) {
                this->unk_84F = 1;
                this->stateFlags1 |= PLAYER_STATE1_29;

                this->unk_450.x = this->unk_45C.x;
                this->unk_450.z = this->unk_45C.z;
            }
        }
        else {
            sp34 = 5.0f;
            sp30 = 20;

            if (this->stateFlags1 & PLAYER_STATE1_0) {
                sp34 = gSaveContext.entranceSpeed;

                if (D_808535F4 != 0) {
                    this->unk_450.x = (Math_SinS(D_808535FC) * 400.0f) + this->actor.world.pos.x;
                    this->unk_450.z = (Math_CosS(D_808535FC) * 400.0f) + this->actor.world.pos.z;
                }
            }
            else if (this->unk_850 < 0) {
                this->unk_850++;

                sp34 = gSaveContext.entranceSpeed;
                sp30 = -1;
            }

            temp = func_80845BA0(globalCtx, this, &sp34, sp30);

            if ((this->unk_850 == 0) ||
                ((temp == 0) && (this->linearVelocity == 0.0f) && (Gameplay_GetCamera(globalCtx, 0)->unk_14C & 0x10))) {

                func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
                func_80845C68(globalCtx, gSaveContext.respawn[RESPAWN_MODE_DOWN].data);

                if (!func_8083B644(this, globalCtx)) {
                    func_8083CF5C(this, globalCtx);
                }
            }
        }
    }

    if (this->stateFlags1 & PLAYER_STATE1_11) {
        func_80836670(this, globalCtx);
    }
}

void func_80845EF8(Player* this, GlobalContext* globalCtx) {
    s32 sp2C;

    this->stateFlags2 |= PLAYER_STATE2_5;
    sp2C = LinkAnimation_Update(globalCtx, &this->skelAnime);

    func_80836670(this, globalCtx);

    if (sp2C) {
        if (this->unk_850 == 0) {
            if (DECR(this->doorTimer) == 0) {
                this->unk_850 = 1;
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        }
        else {
            func_8083C0E8(this, globalCtx);
            if (globalCtx->roomCtx.prevRoom.num >= 0) {
                func_80097534(globalCtx, &globalCtx->roomCtx);
            }
            func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
            Gameplay_SetupRespawnPoint(globalCtx, 0, 0xDFF);
        }
        return;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_29) && LinkAnimation_OnFrame(&this->skelAnime, 15.0f)) {
        globalCtx->func_11D54(this, globalCtx);
    }
}

void func_80846050(Player* this, GlobalContext* globalCtx) {
    func_8083721C(this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80839F90(this, globalCtx);
        func_80835688(this, globalCtx);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor* interactRangeActor = this->interactRangeActor;

        if (!func_80835644(globalCtx, this, interactRangeActor)) {
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

static struct_80832924 D_8085461C[] = {
    { NA_SE_VO_LI_SWORD_L, 0x2031 },
    { NA_SE_VO_LI_SWORD_N, -0x20E6 },
};

void func_80846120(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime) && (this->unk_850++ > 20)) {
        if (!func_8083B040(this, globalCtx)) {
            func_8083A098(this, &gPlayerAnim_002FA0, globalCtx);
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

        heldActor->speedXZ = Math_SinS(heldActor->shape.rot.x) * 40.0f;
        heldActor->velocity.y = Math_CosS(heldActor->shape.rot.x) * 40.0f;
        heldActor->gravity = -2.0f;
        heldActor->minVelocityY = -30.0f;
        func_808323B4(globalCtx, this);
        return;
    }

    func_80832924(this, D_8085461C);
}

void func_80846260(Player* this, GlobalContext* globalCtx) {
    func_8083721C(this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80832284(globalCtx, this, &gPlayerAnim_0032C0);
        this->unk_850 = 1;
        return;
    }

    if (this->unk_850 == 0) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 27.0f)) {
            Actor* interactRangeActor = this->interactRangeActor;

            this->heldActor = interactRangeActor;
            this->actor.child = interactRangeActor;
            interactRangeActor->parent = &this->actor;
            return;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, 25.0f)) {
            func_80832698(this, NA_SE_VO_LI_SWORD_L);
            return;
        }

    }
    else if (CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN)) {
        func_80835C58(globalCtx, this, func_80846358, 1);
        func_80832264(globalCtx, this, &gPlayerAnim_0032B8);
    }
}

void func_80846358(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80839F90(this, globalCtx);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 6.0f)) {
        Actor* heldActor = this->heldActor;

        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speedXZ = 10.0f;
        heldActor->velocity.y = 20.0f;
        func_80834644(globalCtx, this);
        func_8002F7DC(&this->actor, NA_SE_PL_THROW);
        func_80832698(this, NA_SE_VO_LI_SWORD_N);
    }
}

void func_80846408(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80832284(globalCtx, this, &gPlayerAnim_003070);
        this->unk_850 = 15;
        return;
    }

    if (this->unk_850 != 0) {
        this->unk_850--;
        if (this->unk_850 == 0) {
            func_8083A098(this, &gPlayerAnim_003068, globalCtx);
            this->stateFlags1 &= ~PLAYER_STATE1_11;
            func_80832698(this, NA_SE_VO_LI_DAMAGE_S);
        }
    }
}

void func_808464B0(Player* this, GlobalContext* globalCtx) {
    func_8083721C(this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80839F90(this, globalCtx);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor* heldActor = this->heldActor;

        if (!func_80835644(globalCtx, this, heldActor)) {
            heldActor->velocity.y = 0.0f;
            heldActor->speedXZ = 0.0f;
            func_80834644(globalCtx, this);
            if (heldActor->id == ACTOR_EN_BOM_CHU) {
                func_8083B8F4(this, globalCtx);
            }
        }
    }
}

void func_80846578(Player* this, GlobalContext* globalCtx) {
    f32 sp34;
    s16 sp32;

    func_8083721C(this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime) ||
        ((this->skelAnime.curFrame >= 8.0f) && func_80837268(this, &sp34, &sp32, 0.018f, globalCtx))) {
        func_80839F90(this, globalCtx);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 3.0f)) {
        func_8084409C(globalCtx, this, this->linearVelocity + 8.0f, 12.0f);
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

void func_8084663C(Actor* thisx, GlobalContext* globalCtx) {
}

void func_80846648(GlobalContext* globalCtx, Player* this) {
    this->actor.update = func_8084663C;
    this->actor.draw = NULL;
}

void func_80846660(GlobalContext* globalCtx, Player* this) {
    func_80835C58(globalCtx, this, func_8084F710, 0);
    if ((globalCtx->sceneNum == SCENE_SPOT06) && (gSaveContext.sceneSetupIndex >= 4)) {
        this->unk_84F = 1;
    }
    this->stateFlags1 |= PLAYER_STATE1_29;
    LinkAnimation_Change(globalCtx, &this->skelAnime, &gPlayerAnim_003298, 2.0f / 3.0f, 0.0f, 24.0f, ANIMMODE_ONCE,
        0.0f);
    this->actor.world.pos.y += 800.0f;
}

static u8 D_808546F0[] = { ITEM_SWORD_MASTER, ITEM_SWORD_KOKIRI };

void func_80846720(GlobalContext* globalCtx, Player* this, s32 arg2) {
    s32 item = D_808546F0[(void)0, gSaveContext.linkAge];
    s32 actionParam = sItemActionParams[item];

    func_80835EFC(this);
    func_808323B4(globalCtx, this);

    this->heldItemId = item;
    this->nextModelGroup = Player_ActionToModelGroup(this, actionParam);

    func_8083399C(globalCtx, this, actionParam);
    func_80834644(globalCtx, this);

    if (arg2 != 0) {
        func_8002F7DC(&this->actor, NA_SE_IT_SWORD_PICKOUT);
    }
}

static Vec3f D_808546F4 = { -1.0f, 69.0f, 20.0f };

void func_808467D4(GlobalContext* globalCtx, Player* this) {
    func_80835C58(globalCtx, this, func_8084E9AC, 0);
    this->stateFlags1 |= PLAYER_STATE1_29;
    Math_Vec3f_Copy(&this->actor.world.pos, &D_808546F4);
    this->currentYaw = this->actor.shape.rot.y = -0x8000;
    LinkAnimation_Change(globalCtx, &this->skelAnime, this->ageProperties->unk_A0, 2.0f / 3.0f, 0.0f, 0.0f,
        ANIMMODE_ONCE, 0.0f);
    func_80832F54(globalCtx, this, 0x28F);
    if (LINK_IS_ADULT) {
        func_80846720(globalCtx, this, 0);
    }
    this->unk_850 = 20;
}

void func_808468A8(GlobalContext* globalCtx, Player* this) {
    func_80835C58(globalCtx, this, func_8084F9A0, 0);
    func_80832F54(globalCtx, this, 0x9B);
}

void func_808468E8(GlobalContext* globalCtx, Player* this) {
    func_808389E8(this, &gPlayerAnim_002FE0, 12.0f, globalCtx);
    func_80835C58(globalCtx, this, func_8084F9C0, 0);
    this->stateFlags1 |= PLAYER_STATE1_29;
    this->fallStartHeight = this->actor.world.pos.y;
    OnePointCutscene_Init(globalCtx, 5110, 40, &this->actor, MAIN_CAM);
}

void func_80846978(GlobalContext* globalCtx, Player* this) {
    func_80837C0C(globalCtx, this, 1, 2.0f, 2.0f, this->actor.shape.rot.y + 0x8000, 0);
}

void func_808469BC(GlobalContext* globalCtx, Player* this) {
    func_80835C58(globalCtx, this, func_8084F698, 0);
    this->actor.draw = NULL;
    this->stateFlags1 |= PLAYER_STATE1_29;
}

static s16 D_80854700[] = { ACTOR_MAGIC_WIND, ACTOR_MAGIC_DARK, ACTOR_MAGIC_FIRE };

Actor* func_80846A00(GlobalContext* globalCtx, Player* this, s32 arg2) {
    return Actor_Spawn(&globalCtx->actorCtx, globalCtx, D_80854700[arg2], this->actor.world.pos.x,
        this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
}

void func_80846A68(GlobalContext* globalCtx, Player* this) {
    this->actor.draw = NULL;
    func_80835C58(globalCtx, this, func_8085076C, 0);
    this->stateFlags1 |= PLAYER_STATE1_29;
}

static InitChainEntry D_80854708[] = {
    ICHAIN_F32(targetArrowOffset, 500, ICHAIN_STOP),
};

static EffectBlureInit2 D_8085470C = {
    0, 8, 0, { 255, 255, 255, 255 }, { 255, 255, 255, 64 }, { 255, 255, 255, 0 }, { 255, 255, 255, 0 }, 4,
    0, 2, 0, { 0, 0, 0, 0 },         { 0, 0, 0, 0 },
};

static Vec3s D_80854730 = { -57, 3377, 0 };

void Player_InitCommon(Player* this, GlobalContext* globalCtx, FlexSkeletonHeader* skelHeader) {
    this->ageProperties = &sAgeProperties[gSaveContext.linkAge];
    Actor_ProcessInitChain(&this->actor, D_80854708);
    this->swordEffectIndex = TOTAL_EFFECT_COUNT;
    this->currentYaw = this->actor.world.rot.y;
    func_80834644(globalCtx, this);

    SkelAnime_InitLink(globalCtx, &this->skelAnime, skelHeader, D_80853914[PLAYER_ANIMGROUP_0][this->modelAnimType], 9,
        this->jointTable, this->morphTable, PLAYER_LIMB_MAX);
    this->skelAnime.baseTransl = D_80854730;
    SkelAnime_InitLink(globalCtx, &this->skelAnime2, skelHeader, func_80833338(this), 9, this->jointTable2,
        this->morphTable2, PLAYER_LIMB_MAX);
    this->skelAnime2.baseTransl = D_80854730;

    Effect_Add(globalCtx, &this->swordEffectIndex, EFFECT_BLURE2, 0, 0, &D_8085470C);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFeet, this->ageProperties->unk_04);
    this->unk_46C = SUBCAM_NONE;
    Collider_InitCylinder(globalCtx, &this->cylinder);
    Collider_SetCylinder(globalCtx, &this->cylinder, &this->actor, &D_80854624);
    Collider_InitQuad(globalCtx, &this->swordQuads[0]);
    Collider_SetQuad(globalCtx, &this->swordQuads[0], &this->actor, &D_80854650);
    Collider_InitQuad(globalCtx, &this->swordQuads[1]);
    Collider_SetQuad(globalCtx, &this->swordQuads[1], &this->actor, &D_80854650);
    Collider_InitQuad(globalCtx, &this->shieldQuad);
    Collider_SetQuad(globalCtx, &this->shieldQuad, &this->actor, &D_808546A0);
}

static void (*D_80854738[])(GlobalContext* globalCtx, Player* this) = {
    func_80846648, func_808467D4, func_80846660, func_808468A8, func_808468E8, func_808469BC,
    func_80846A68, func_80846978, func_8083CA54, func_8083CA54, func_8083CA54, func_8083CA54,
    func_8083CA54, func_8083CA20, func_8083CA54, func_8083CA9C,
};

static Vec3f D_80854778 = { 0.0f, 50.0f, 0.0f };

void Player_Init(Actor* thisx, GlobalContext* globalCtx2) {
    Player* this = (Player*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    SceneTableEntry* scene = globalCtx->loadedScene;
    u32 titleFileSize;
    s32 initMode;
    s32 sp50;
    s32 sp4C;

    globalCtx->shootingGalleryStatus = globalCtx->bombchuBowlingStatus = 0;

    globalCtx->playerInit = Player_InitCommon;
    globalCtx->playerUpdate = Player_UpdateCommon;
    globalCtx->isPlayerDroppingFish = Player_IsDroppingFish;
    globalCtx->startPlayerFishing = Player_StartFishing;
    globalCtx->grabPlayer = func_80852F38;
    globalCtx->startPlayerCutscene = func_80852FFC;
    globalCtx->func_11D54 = func_80853080;
    globalCtx->damagePlayer = Player_InflictDamage;
    globalCtx->talkWithPlayer = func_80853148;

    thisx->room = -1;
    this->ageProperties = &sAgeProperties[gSaveContext.linkAge];
    this->itemActionParam = this->heldItemActionParam = -1;
    this->heldItemId = ITEM_NONE;

    func_80835F44(globalCtx, this, ITEM_NONE);
    Player_SetEquipmentData(globalCtx, this);
    this->prevBoots = this->currentBoots;
    Player_InitCommon(this, globalCtx, gPlayerSkelHeaders[((void)0, gSaveContext.linkAge)]);
    this->giObjectSegment = (void*)(((u32)ZeldaArena_MallocDebug(0x3008, "../z_player.c", 17175) + 8) & ~0xF);

    sp50 = gSaveContext.respawnFlag;

    if (sp50 != 0) {
        if (sp50 == -3) {
            thisx->params = gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams;
        }
        else {
            if ((sp50 == 1) || (sp50 == -1)) {
                this->unk_A86 = -2;
            }

            if (sp50 < 0) {
                sp4C = 0;
            }
            else {
                sp4C = sp50 - 1;
                Math_Vec3f_Copy(&thisx->world.pos, &gSaveContext.respawn[sp50 - 1].pos);
                Math_Vec3f_Copy(&thisx->home.pos, &thisx->world.pos);
                Math_Vec3f_Copy(&thisx->prevPos, &thisx->world.pos);
                this->fallStartHeight = thisx->world.pos.y;
                this->currentYaw = thisx->shape.rot.y = gSaveContext.respawn[sp4C].yaw;
                thisx->params = gSaveContext.respawn[sp4C].playerParams;
            }

            globalCtx->actorCtx.flags.tempSwch = gSaveContext.respawn[sp4C].tempSwchFlags & 0xFFFFFF;
            globalCtx->actorCtx.flags.tempCollect = gSaveContext.respawn[sp4C].tempCollectFlags;
        }
    }

    if ((sp50 == 0) || (sp50 < -1)) {
        titleFileSize = scene->titleFile.vromEnd - scene->titleFile.vromStart;
        if (gSaveContext.showTitleCard) {
            if ((gSaveContext.sceneSetupIndex < 4) &&
                (gEntranceTable[((void)0, gSaveContext.entranceIndex) + ((void)0, gSaveContext.sceneSetupIndex)].field &
                    0x4000) &&
                ((globalCtx->sceneNum != SCENE_DDAN) || (gSaveContext.eventChkInf[11] & 1)) &&
                ((globalCtx->sceneNum != SCENE_NIGHT_SHOP) || (gSaveContext.eventChkInf[2] & 0x20))) {
                TitleCard_InitPlaceName(globalCtx, &globalCtx->actorCtx.titleCtx, this->giObjectSegment, 160, 120, 144,
                    24, 20);
            }
        }
        gSaveContext.showTitleCard = true;
    }

    if (func_80845C68(globalCtx, (sp50 == 2) ? 1 : 0) == 0) {
        gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = (thisx->params & 0xFF) | 0xD00;
    }

    gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 1;

    if (globalCtx->sceneNum <= SCENE_GANONTIKA_SONOGO) {
        gSaveContext.infTable[26] |= gBitFlags[globalCtx->sceneNum];
    }

    initMode = (thisx->params & 0xF00) >> 8;
    if ((initMode == 5) || (initMode == 6)) {
        if (gSaveContext.cutsceneIndex >= 0xFFF0) {
            initMode = 13;
        }
    }

    D_80854738[initMode](globalCtx, this);

    if (initMode != 0) {
        if ((gSaveContext.gameMode == 0) || (gSaveContext.gameMode == 3)) {
            this->naviActor = Player_SpawnFairy(globalCtx, this, &thisx->world.pos, &D_80854778, FAIRY_NAVI);
            if (gSaveContext.dogParams != 0) {
                gSaveContext.dogParams |= 0x8000;
            }
        }
    }

    if (gSaveContext.nayrusLoveTimer != 0) {
        gSaveContext.unk_13F0 = 3;
        func_80846A00(globalCtx, this, 1);
        this->stateFlags3 &= ~PLAYER_STATE3_6;
    }

    if (gSaveContext.entranceSound != 0) {
        Audio_PlayActorSound2(&this->actor, ((void)0, gSaveContext.entranceSound));
        gSaveContext.entranceSound = 0;
    }

    Map_SavePlayerInitialInfo(globalCtx);
    MREG(64) = 0;
}

void func_808471F4(s16* pValue) {
    s16 step;

    step = (ABS(*pValue) * 100.0f) / 1000.0f;
    step = CLAMP(step, 400, 4000);

    Math_ScaledStepToS(pValue, 0, step);
}

void func_80847298(Player* this) {
    s16 sp26;

    if (!(this->unk_6AE & 2)) {
        sp26 = this->actor.focus.rot.y - this->actor.shape.rot.y;
        func_808471F4(&sp26);
        this->actor.focus.rot.y = this->actor.shape.rot.y + sp26;
    }

    if (!(this->unk_6AE & 1)) {
        func_808471F4(&this->actor.focus.rot.x);
    }

    if (!(this->unk_6AE & 8)) {
        func_808471F4(&this->unk_6B6);
    }

    if (!(this->unk_6AE & 0x40)) {
        func_808471F4(&this->unk_6BC);
    }

    if (!(this->unk_6AE & 4)) {
        func_808471F4(&this->actor.focus.rot.z);
    }

    if (!(this->unk_6AE & 0x10)) {
        func_808471F4(&this->unk_6B8);
    }

    if (!(this->unk_6AE & 0x20)) {
        func_808471F4(&this->unk_6BA);
    }

    if (!(this->unk_6AE & 0x80)) {
        if (this->unk_6B0 != 0) {
            func_808471F4(&this->unk_6B0);
        }
        else {
            func_808471F4(&this->unk_6BE);
        }
    }

    if (!(this->unk_6AE & 0x100)) {
        func_808471F4(&this->unk_6C0);
    }

    this->unk_6AE = 0;
}

static f32 D_80854784[] = { 120.0f, 240.0f, 360.0f };

static u8 sDiveDoActions[] = { DO_ACTION_1, DO_ACTION_2, DO_ACTION_3, DO_ACTION_4,
                               DO_ACTION_5, DO_ACTION_6, DO_ACTION_7, DO_ACTION_8 };

void func_808473D4(GlobalContext* globalCtx, Player* this) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_NONE) && (this->actor.category == ACTORCAT_PLAYER)) {
        Actor* heldActor = this->heldActor;
        Actor* interactRangeActor = this->interactRangeActor;
        s32 sp24;
        s32 sp20 = this->unk_84B[this->unk_846];
        s32 sp1C = func_808332B8(this);
        s32 doAction = DO_ACTION_NONE;

        if (!Player_InBlockingCsMode(globalCtx, this)) {
            if (this->stateFlags1 & PLAYER_STATE1_20) {
                doAction = DO_ACTION_RETURN;
            }
            else if ((this->heldItemActionParam == PLAYER_AP_FISHING_POLE) && (this->unk_860 != 0)) {
                if (this->unk_860 == 2) {
                    doAction = DO_ACTION_REEL;
                }
            }
            else if ((func_8084E3C4 != this->func_674) && !(this->stateFlags2 & PLAYER_STATE2_18)) {
                if ((this->doorType != PLAYER_DOORTYPE_NONE) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_11) ||
                        ((heldActor != NULL) && (heldActor->id == ACTOR_EN_RU1)))) {
                    doAction = DO_ACTION_OPEN;
                }
                else if ((!(this->stateFlags1 & PLAYER_STATE1_11) || (heldActor == NULL)) &&
                    (interactRangeActor != NULL) &&
                    ((!sp1C && (this->getItemId == GI_NONE)) ||
                        ((this->getItemId < 0) && !(this->stateFlags1 & PLAYER_STATE1_27)))) {
                    if (this->getItemId < 0) {
                        doAction = DO_ACTION_OPEN;
                    }
                    else if ((interactRangeActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT) {
                        doAction = DO_ACTION_DROP;
                    }
                    else {
                        doAction = DO_ACTION_GRAB;
                    }
                }
                else if (!sp1C && (this->stateFlags2 & PLAYER_STATE2_0)) {
                    doAction = DO_ACTION_GRAB;
                }
                else if ((this->stateFlags2 & PLAYER_STATE2_2) ||
                    (!(this->stateFlags1 & PLAYER_STATE1_23) && (this->rideActor != NULL))) {
                    doAction = DO_ACTION_CLIMB;
                }
                else if ((this->stateFlags1 & PLAYER_STATE1_23) && !EN_HORSE_CHECK_4((EnHorse*)this->rideActor) &&
                    (func_8084D3E4 != this->func_674)) {
                    if ((this->stateFlags2 & PLAYER_STATE2_1) && (this->targetActor != NULL)) {
                        if (this->targetActor->category == ACTORCAT_NPC) {
                            doAction = DO_ACTION_SPEAK;
                        }
                        else {
                            doAction = DO_ACTION_CHECK;
                        }
                    }
                    else if (!func_8002DD78(this) && !(this->stateFlags1 & PLAYER_STATE1_20)) {
                        doAction = DO_ACTION_FASTER;
                    }
                }
                else if ((this->stateFlags2 & PLAYER_STATE2_1) && (this->targetActor != NULL)) {
                    if (this->targetActor->category == ACTORCAT_NPC) {
                        doAction = DO_ACTION_SPEAK;
                    }
                    else {
                        doAction = DO_ACTION_CHECK;
                    }
                }
                else if ((this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_21)) ||
                    ((this->stateFlags1 & PLAYER_STATE1_23) && (this->stateFlags2 & PLAYER_STATE2_22))) {
                    doAction = DO_ACTION_DOWN;
                }
                else if (this->stateFlags2 & PLAYER_STATE2_16) {
                    doAction = DO_ACTION_ENTER;
                }
                else if ((this->stateFlags1 & PLAYER_STATE1_11) && (this->getItemId == GI_NONE) &&
                    (heldActor != NULL)) {
                    if ((this->actor.bgCheckFlags & 1) || (heldActor->id == ACTOR_EN_NIW)) {
                        if (func_8083EAF0(this, heldActor) == 0) {
                            doAction = DO_ACTION_DROP;
                        }
                        else {
                            doAction = DO_ACTION_THROW;
                        }
                    }
                }
                else if (!(this->stateFlags1 & PLAYER_STATE1_27) && func_8083A0D4(this) &&
                    (this->getItemId < GI_MAX)) {
                    doAction = DO_ACTION_GRAB;
                }
                else if (this->stateFlags2 & PLAYER_STATE2_11) {
                    sp24 = (D_80854784[CUR_UPG_VALUE(UPG_SCALE)] - this->actor.yDistToWater) / 40.0f;
                    sp24 = CLAMP(sp24, 0, 7);
                    doAction = sDiveDoActions[sp24];
                }
                else if (sp1C && !(this->stateFlags2 & PLAYER_STATE2_10)) {
                    doAction = DO_ACTION_DIVE;
                }
                else if (!sp1C && (!(this->stateFlags1 & PLAYER_STATE1_22) || func_80833BCC(this) ||
                    !Player_IsChildWithHylianShield(this))) {
                    if ((!(this->stateFlags1 & PLAYER_STATE1_14) && (sp20 <= 0) &&
                        (func_8008E9C4(this) ||
                            ((D_808535E4 != 7) &&
                                (func_80833B2C(this) || ((globalCtx->roomCtx.curRoom.unk_03 != 2) &&
                                    !(this->stateFlags1 & PLAYER_STATE1_22) && (sp20 == 0))))))) {
                        doAction = DO_ACTION_ATTACK;
                    }
                    else if ((globalCtx->roomCtx.curRoom.unk_03 != 2) && func_80833BCC(this) && (sp20 > 0)) {
                        doAction = DO_ACTION_JUMP;
                    }
                    else if ((this->heldItemActionParam >= PLAYER_AP_SWORD_MASTER) ||
                        ((this->stateFlags2 & PLAYER_STATE2_20) &&
                            (globalCtx->actorCtx.targetCtx.arrowPointedActor == NULL))) {
                        doAction = DO_ACTION_PUTAWAY;
                    }
                }
            }
        }

        if (doAction != DO_ACTION_PUTAWAY) {
            this->unk_837 = 20;
        }
        else if (this->unk_837 != 0) {
            doAction = DO_ACTION_NONE;
            this->unk_837--;
        }

        Interface_SetDoAction(globalCtx, doAction);

        if (this->stateFlags2 & PLAYER_STATE2_21) {
            if (this->unk_664 != NULL) {
                Interface_SetNaviCall(globalCtx, 0x1E);
            }
            else {
                Interface_SetNaviCall(globalCtx, 0x1D);
            }
            Interface_SetNaviCall(globalCtx, 0x1E);
        }
        else {
            Interface_SetNaviCall(globalCtx, 0x1F);
        }
    }
}

s32 func_80847A78(Player* this) {
    s32 cond;

    if ((this->currentBoots == PLAYER_BOOTS_HOVER) && (this->hoverBootsTimer != 0)) {
        this->hoverBootsTimer--;
    }
    else {
        this->hoverBootsTimer = 0;
    }

    cond = (this->currentBoots == PLAYER_BOOTS_HOVER) &&
        ((this->actor.yDistToWater >= 0.0f) || (func_80838144(D_808535E4) >= 0) || func_8083816C(D_808535E4));

    if (cond && (this->actor.bgCheckFlags & 1) && (this->hoverBootsTimer != 0)) {
        this->actor.bgCheckFlags &= ~1;
    }

    if (this->actor.bgCheckFlags & 1) {
        if (!cond) {
            this->hoverBootsTimer = 19;
        }
        return 0;
    }

    D_808535E4 = 0;
    this->unk_898 = this->unk_89A = D_80853610 = 0;

    return 1;
}

static Vec3f D_80854798 = { 0.0f, 18.0f, 0.0f };

void func_80847BA0(GlobalContext* globalCtx, Player* this) {
    u8 spC7 = 0;
    CollisionPoly* spC0;
    Vec3f spB4;
    f32 spB0;
    f32 spAC;
    f32 spA8;
    u32 spA4;

    D_80853604 = this->unk_A7A;

    if (this->stateFlags2 & PLAYER_STATE2_18) {
        spB0 = 10.0f;
        spAC = 15.0f;
        spA8 = 30.0f;
    }
    else {
        spB0 = this->ageProperties->unk_38;
        spAC = 26.0f;
        spA8 = this->ageProperties->unk_00;
    }

    if (this->stateFlags1 & (PLAYER_STATE1_29 | PLAYER_STATE1_31)) {
        if (this->stateFlags1 & PLAYER_STATE1_31) {
            this->actor.bgCheckFlags &= ~1;
            spA4 = 0x38;
        }
        else if ((this->stateFlags1 & PLAYER_STATE1_0) && ((this->unk_A84 - (s32)this->actor.world.pos.y) >= 100)) {
            spA4 = 0x39;
        }
        else if (!(this->stateFlags1 & PLAYER_STATE1_0) &&
            ((func_80845EF8 == this->func_674) || (func_80845CA4 == this->func_674))) {
            this->actor.bgCheckFlags &= ~0x208;
            spA4 = 0x3C;
        }
        else {
            spA4 = 0x3F;
        }
    }
    else {
        spA4 = 0x3F;
    }

    if (this->stateFlags3 & PLAYER_STATE3_0) {
        spA4 &= ~6;
    }

    if (spA4 & 4) {
        this->stateFlags3 |= PLAYER_STATE3_4;
    }

    Math_Vec3f_Copy(&spB4, &this->actor.world.pos);
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, spAC, spB0, spA8, spA4);

    if (this->actor.bgCheckFlags & 0x10) {
        this->actor.velocity.y = 0.0f;
    }

    D_80853600 = this->actor.world.pos.y - this->actor.floorHeight;
    D_808535F4 = 0;

    spC0 = this->actor.floorPoly;

    if (spC0 != NULL) {
        this->unk_A7A = func_80041EA4(&globalCtx->colCtx, spC0, this->actor.floorBgId);
        this->unk_A82 = this->unk_89E;

        if (this->actor.bgCheckFlags & 0x20) {
            if (this->actor.yDistToWater < 20.0f) {
                this->unk_89E = 4;
            }
            else {
                this->unk_89E = 5;
            }
        }
        else {
            if (this->stateFlags2 & PLAYER_STATE2_9) {
                this->unk_89E = 1;
            }
            else {
                this->unk_89E = SurfaceType_GetSfx(&globalCtx->colCtx, spC0, this->actor.floorBgId);
            }
        }

        if (this->actor.category == ACTORCAT_PLAYER) {
            Audio_SetCodeReverb(SurfaceType_GetEcho(&globalCtx->colCtx, spC0, this->actor.floorBgId));

            if (this->actor.floorBgId == BGCHECK_SCENE) {
                func_80074CE8(globalCtx,
                    SurfaceType_GetLightSettingIndex(&globalCtx->colCtx, spC0, this->actor.floorBgId));
            }
            else {
                func_80043508(&globalCtx->colCtx, this->actor.floorBgId);
            }
        }

        D_808535F4 = SurfaceType_GetConveyorSpeed(&globalCtx->colCtx, spC0, this->actor.floorBgId);
        if (D_808535F4 != 0) {
            D_808535F8 = SurfaceType_IsConveyor(&globalCtx->colCtx, spC0, this->actor.floorBgId);
            if (((D_808535F8 == 0) && (this->actor.yDistToWater > 20.0f) &&
                (this->currentBoots != PLAYER_BOOTS_IRON)) ||
                ((D_808535F8 != 0) && (this->actor.bgCheckFlags & 1))) {
                D_808535FC = SurfaceType_GetConveyorDirection(&globalCtx->colCtx, spC0, this->actor.floorBgId) << 10;
            }
            else {
                D_808535F4 = 0;
            }
        }
    }

    func_80839034(globalCtx, this, spC0, this->actor.floorBgId);

    this->actor.bgCheckFlags &= ~0x200;

    if (this->actor.bgCheckFlags & 8) {
        CollisionPoly* spA0;
        s32 sp9C;
        s16 sp9A;
        s32 pad;

        D_80854798.y = 18.0f;
        D_80854798.z = this->ageProperties->unk_38 + 10.0f;

        if (!(this->stateFlags2 & PLAYER_STATE2_18) &&
            func_80839768(globalCtx, this, &D_80854798, &spA0, &sp9C, &D_80858AA8)) {
            this->actor.bgCheckFlags |= 0x200;
            if (this->actor.wallPoly != spA0) {
                this->actor.wallPoly = spA0;
                this->actor.wallBgId = sp9C;
                this->actor.wallYaw = Math_Atan2S(spA0->normal.z, spA0->normal.x);
            }
        }

        sp9A = this->actor.shape.rot.y - (s16)(this->actor.wallYaw + 0x8000);

        D_808535F0 = func_80041DB8(&globalCtx->colCtx, this->actor.wallPoly, this->actor.wallBgId);

        D_80853608 = ABS(sp9A);

        sp9A = this->currentYaw - (s16)(this->actor.wallYaw + 0x8000);

        D_8085360C = ABS(sp9A);

        spB0 = D_8085360C * 0.00008f;
        if (!(this->actor.bgCheckFlags & 1) || spB0 >= 1.0f) {
            this->unk_880 = R_RUN_SPEED_LIMIT / 100.0f;
        }
        else {
            spAC = (R_RUN_SPEED_LIMIT / 100.0f * spB0);
            this->unk_880 = spAC;
            if (spAC < 0.1f) {
                this->unk_880 = 0.1f;
            }
        }

        if ((this->actor.bgCheckFlags & 0x200) && (D_80853608 < 0x3000)) {
            CollisionPoly* wallPoly = this->actor.wallPoly;

            if ((ABS(wallPoly->normal.y) < 600) || (CVar_GetS32("gClimbEverything", 0) != 0)) {
                f32 sp8C = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 sp88 = COLPOLY_GET_NORMAL(wallPoly->normal.y);
                f32 sp84 = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 wallHeight;
                CollisionPoly* sp7C;
                CollisionPoly* sp78;
                s32 sp74;
                Vec3f sp68;
                f32 sp64;
                f32 sp60;
                s32 temp3;

                this->wallDistance = Math3D_UDistPlaneToPos(sp8C, sp88, sp84, wallPoly->dist, &this->actor.world.pos);

                spB0 = this->wallDistance + 10.0f;
                sp68.x = this->actor.world.pos.x - (spB0 * sp8C);
                sp68.z = this->actor.world.pos.z - (spB0 * sp84);
                sp68.y = this->actor.world.pos.y + this->ageProperties->unk_0C;

                sp64 = BgCheck_EntityRaycastFloor1(&globalCtx->colCtx, &sp7C, &sp68);
                wallHeight = sp64 - this->actor.world.pos.y;
                this->wallHeight = wallHeight;

                if ((this->wallHeight < 18.0f) ||
                    BgCheck_EntityCheckCeiling(&globalCtx->colCtx, &sp60, &this->actor.world.pos,
                        (sp64 - this->actor.world.pos.y) + 20.0f, &sp78, &sp74, &this->actor)) {
                    this->wallHeight = 399.96002f;
                }
                else {
                    D_80854798.y = (sp64 + 5.0f) - this->actor.world.pos.y;

                    if (func_80839768(globalCtx, this, &D_80854798, &sp78, &sp74, &D_80858AA8) &&
                        (temp3 = this->actor.wallYaw - Math_Atan2S(sp78->normal.z, sp78->normal.x),
                            ABS(temp3) < 0x4000) &&
                        !func_80041E18(&globalCtx->colCtx, sp78, sp74)) {
                        this->wallHeight = 399.96002f;
                    }
                    else if (func_80041DE4(&globalCtx->colCtx, wallPoly, this->actor.wallBgId) == 0) {
                        if (this->ageProperties->unk_1C <= this->wallHeight) {
                            if (ABS(sp7C->normal.y) > 28000) {
                                if (this->ageProperties->unk_14 <= this->wallHeight) {
                                    spC7 = 4;
                                }
                                else if (this->ageProperties->unk_18 <= this->wallHeight) {
                                    spC7 = 3;
                                }
                                else {
                                    spC7 = 2;
                                }
                            }
                        }
                        else {
                            spC7 = 1;
                        }
                    }
                }
            }
        }
    }
    else {
        this->unk_880 = R_RUN_SPEED_LIMIT / 100.0f;
        this->unk_88D = 0;
        this->wallHeight = 0.0f;
    }

    if (spC7 == this->unk_88C) {
        if ((this->linearVelocity != 0.0f) && (this->unk_88D < 100)) {
            this->unk_88D++;
        }
    }
    else {
        this->unk_88C = spC7;
        this->unk_88D = 0;
    }

    if (this->actor.bgCheckFlags & 1) {
        D_808535E4 = func_80041D4C(&globalCtx->colCtx, spC0, this->actor.floorBgId);

        if (!func_80847A78(this)) {
            f32 sp58;
            f32 sp54;
            f32 sp50;
            f32 sp4C;
            s32 pad2;
            f32 sp44;
            s32 pad3;

            if (this->actor.floorBgId != BGCHECK_SCENE) {
                func_800434C8(&globalCtx->colCtx, this->actor.floorBgId);
            }

            sp58 = COLPOLY_GET_NORMAL(spC0->normal.x);
            sp54 = 1.0f / COLPOLY_GET_NORMAL(spC0->normal.y);
            sp50 = COLPOLY_GET_NORMAL(spC0->normal.z);

            sp4C = Math_SinS(this->currentYaw);
            sp44 = Math_CosS(this->currentYaw);

            this->unk_898 = Math_Atan2S(1.0f, (-(sp58 * sp4C) - (sp50 * sp44)) * sp54);
            this->unk_89A = Math_Atan2S(1.0f, (-(sp58 * sp44) - (sp50 * sp4C)) * sp54);

            sp4C = Math_SinS(this->actor.shape.rot.y);
            sp44 = Math_CosS(this->actor.shape.rot.y);

            D_80853610 = Math_Atan2S(1.0f, (-(sp58 * sp4C) - (sp50 * sp44)) * sp54);

            func_8083E318(globalCtx, this, spC0);
        }
    }
    else {
        func_80847A78(this);
    }

    if (this->unk_A7B == D_808535E4) {
        this->unk_A79++;
    }
    else {
        this->unk_A7B = D_808535E4;
        this->unk_A79 = 0;
    }
}

void Player_UpdateCamAndSeqModes(GlobalContext* globalCtx, Player* this) {
    u8 seqMode;
    s32 pad;
    Actor* unk_664;
    s32 camMode;

    if (this->actor.category == ACTORCAT_PLAYER) {
        seqMode = SEQ_MODE_DEFAULT;

        if (this->csMode != 0) {
            Camera_ChangeMode(Gameplay_GetCamera(globalCtx, 0), CAM_MODE_NORMAL);
        }
        else if (!(this->stateFlags1 & PLAYER_STATE1_20)) {
            if ((this->actor.parent != NULL) && (this->stateFlags3 & PLAYER_STATE3_7)) {
                camMode = CAM_MODE_HOOKSHOT;
                Camera_SetParam(Gameplay_GetCamera(globalCtx, 0), 8, this->actor.parent);
            }
            else if (func_8084377C == this->func_674) {
                camMode = CAM_MODE_STILL;
            }
            else if (this->stateFlags2 & PLAYER_STATE2_8) {
                camMode = CAM_MODE_PUSHPULL;
            }
            else if ((unk_664 = this->unk_664) != NULL) {
                if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_8)) {
                    camMode = CAM_MODE_TALK;
                }
                else if (this->stateFlags1 & PLAYER_STATE1_16) {
                    if (this->stateFlags1 & PLAYER_STATE1_25) {
                        camMode = CAM_MODE_FOLLOWBOOMERANG;
                    }
                    else {
                        camMode = CAM_MODE_FOLLOWTARGET;
                    }
                }
                else {
                    camMode = CAM_MODE_BATTLE;
                }
                Camera_SetParam(Gameplay_GetCamera(globalCtx, 0), 8, unk_664);
            }
            else if (this->stateFlags1 & PLAYER_STATE1_12) {
                camMode = CAM_MODE_CHARGE;
            }
            else if (this->stateFlags1 & PLAYER_STATE1_25) {
                camMode = CAM_MODE_FOLLOWBOOMERANG;
                Camera_SetParam(Gameplay_GetCamera(globalCtx, 0), 8, this->boomerangActor);
            }
            else if (this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14)) {
                if (func_80833B2C(this)) {
                    camMode = CAM_MODE_HANGZ;
                }
                else {
                    camMode = CAM_MODE_HANG;
                }
            }
            else if (this->stateFlags1 & (PLAYER_STATE1_17 | PLAYER_STATE1_30)) {
                if (func_8002DD78(this) || func_808334B4(this)) {
                    camMode = CAM_MODE_BOWARROWZ;
                }
                else if (this->stateFlags1 & PLAYER_STATE1_21) {
                    camMode = CAM_MODE_CLIMBZ;
                }
                else {
                    camMode = CAM_MODE_TARGET;
                }
            }
            else if (this->stateFlags1 & (PLAYER_STATE1_18 | PLAYER_STATE1_21)) {
                if ((func_80845668 == this->func_674) || (this->stateFlags1 & PLAYER_STATE1_21)) {
                    camMode = CAM_MODE_CLIMB;
                }
                else {
                    camMode = CAM_MODE_JUMP;
                }
            }
            else if (this->stateFlags1 & PLAYER_STATE1_19) {
                camMode = CAM_MODE_FREEFALL;
            }
            else if ((this->swordState != 0) && (this->swordAnimation >= 0) && (this->swordAnimation < 0x18)) {
                camMode = CAM_MODE_STILL;
            }
            else {
                camMode = CAM_MODE_NORMAL;
                if ((this->linearVelocity == 0.0f) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_23) || (this->rideActor->speedXZ == 0.0f))) {
                    // not moving
                    seqMode = SEQ_MODE_STILL;
                }
            }

            Camera_ChangeMode(Gameplay_GetCamera(globalCtx, 0), camMode);
        }
        else {
            // First person mode
            seqMode = SEQ_MODE_STILL;
        }

        if (globalCtx->actorCtx.targetCtx.bgmEnemy != NULL) {
            seqMode = SEQ_MODE_ENEMY;
            Audio_SetBgmEnemyVolume(sqrtf(globalCtx->actorCtx.targetCtx.bgmEnemy->xyzDistToPlayerSq));
        }

        if (globalCtx->sceneNum != SCENE_TURIBORI) {
            Audio_SetSequenceMode(seqMode);
        }
    }
}

static Vec3f D_808547A4 = { 0.0f, 0.5f, 0.0f };
static Vec3f D_808547B0 = { 0.0f, 0.5f, 0.0f };

static Color_RGBA8 D_808547BC = { 255, 255, 100, 255 };
static Color_RGBA8 D_808547C0 = { 255, 50, 0, 0 };

void func_80848A04(GlobalContext* globalCtx, Player* this) {
    f32 temp;

    if (this->unk_85C == 0.0f) {
        func_80835F44(globalCtx, this, 0xFF);
        return;
    }

    temp = 1.0f;
    if (DECR(this->unk_860) == 0) {
        Inventory_ChangeAmmo(ITEM_STICK, -1);
        this->unk_860 = 1;
        temp = 0.0f;
        this->unk_85C = temp;
    }
    else if (this->unk_860 > 200) {
        temp = (210 - this->unk_860) / 10.0f;
    }
    else if (this->unk_860 < 20) {
        temp = this->unk_860 / 20.0f;
        this->unk_85C = temp;
    }

    func_8002836C(globalCtx, &this->swordInfo[0].tip, &D_808547A4, &D_808547B0, &D_808547BC, &D_808547C0, temp * 200.0f,
        0, 8);
}

void func_80848B44(GlobalContext* globalCtx, Player* this) {
    Vec3f shockPos;
    Vec3f* randBodyPart;
    s32 shockScale;

    this->shockTimer--;
    this->unk_892 += this->shockTimer;

    if (this->unk_892 > 20) {
        shockScale = this->shockTimer * 2;
        this->unk_892 -= 20;

        if (shockScale > 40) {
            shockScale = 40;
        }

        randBodyPart = this->bodyPartsPos + (s32)Rand_ZeroFloat(ARRAY_COUNT(this->bodyPartsPos) - 0.1f);
        shockPos.x = (Rand_CenteredFloat(5.0f) + randBodyPart->x) - this->actor.world.pos.x;
        shockPos.y = (Rand_CenteredFloat(5.0f) + randBodyPart->y) - this->actor.world.pos.y;
        shockPos.z = (Rand_CenteredFloat(5.0f) + randBodyPart->z) - this->actor.world.pos.z;

        EffectSsFhgFlash_SpawnShock(globalCtx, &this->actor, &shockPos, shockScale, FHGFLASH_SHOCK_PLAYER);
        func_8002F8F0(&this->actor, NA_SE_PL_SPARK - SFX_FLAG);
    }
}

void func_80848C74(GlobalContext* globalCtx, Player* this) {
    s32 spawnedFlame;
    u8* timerPtr;
    s32 timerStep;
    f32 flameScale;
    f32 flameIntensity;
    s32 dmgCooldown;
    s32 i;
    s32 sp58;
    s32 sp54;

    if (this->currentTunic == PLAYER_TUNIC_GORON || CVar_GetS32("gSuperTunic", 0) != 0) {
        sp54 = 20;
    }
    else {
        sp54 = (s32)(this->linearVelocity * 0.4f) + 1;
    }

    spawnedFlame = false;
    timerPtr = this->flameTimers;

    if (this->stateFlags2 & PLAYER_STATE2_3) {
        sp58 = 100;
    }
    else {
        sp58 = 0;
    }

    func_8083819C(this, globalCtx);

    for (i = 0; i < PLAYER_BODYPART_MAX; i++, timerPtr++) {
        timerStep = sp58 + sp54;

        if (*timerPtr <= timerStep) {
            *timerPtr = 0;
        }
        else {
            spawnedFlame = true;
            *timerPtr -= timerStep;

            if (*timerPtr > 20.0f) {
                flameIntensity = (*timerPtr - 20.0f) * 0.01f;
                flameScale = CLAMP(flameIntensity, 0.19999999f, 0.2f);
            }
            else {
                flameScale = *timerPtr * 0.01f;
            }

            flameIntensity = (*timerPtr - 25.0f) * 0.02f;
            flameIntensity = CLAMP(flameIntensity, 0.0f, 1.0f);
            EffectSsFireTail_SpawnFlameOnPlayer(globalCtx, flameScale, i, flameIntensity);
        }
    }

    if (spawnedFlame) {
        func_8002F7DC(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);

        if (globalCtx->sceneNum == SCENE_JYASINBOSS) {
            dmgCooldown = 0;
        }
        else {
            dmgCooldown = 7;
        }

        if ((dmgCooldown & globalCtx->gameplayFrames) == 0) {
            Player_InflictDamage(globalCtx, -1);
        }
    }
    else {
        this->isBurning = false;
    }
}

void func_80848EF8(Player* this, GlobalContext* globalCtx) {
    if (CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY)) {
        f32 temp = 200000.0f - (this->unk_6A4 * 5.0f);

        if (temp < 0.0f) {
            temp = 0.0f;
        }

        this->unk_6A0 += temp;

        /*Prevent it on horse, while jumping and on title screen.
        If you fly around no stone of agony for you! */
        if (CVar_GetS32("gVisualAgony", 0) !=0 && !this->stateFlags1) {
            int rectLeft    = OTRGetRectDimensionFromLeftEdge(26); //Left X Pos
            int rectTop     = 60; //Top Y Pos
            int rectWidth   = 24; //Texture Width
            int rectHeight  = 24; //Texture Heigh
            int DefaultIconA= 50; //Default icon alphe (55 on 255)

            OPEN_DISPS(globalCtx->state.gfxCtx, "../z_player.c", 2824);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, DefaultIconA);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            if (this->unk_6A0 > 4000000.0f) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, 255);
            } else {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, DefaultIconA);
            }
            if (temp == 0 || temp <= 0.1f) {
               /*Fail check, it is used to draw off the icon when
               link is standing out range but do not refresh unk_6A0.
               Also used to make a default value in my case.*/
               gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, DefaultIconA);
            }
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
            gDPSetOtherMode(OVERLAY_DISP++, G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_IA16 | G_TL_TILE | G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE, G_AC_NONE | G_ZS_PRIM | G_RM_XLU_SURF | G_RM_XLU_SURF2);
            gDPLoadTextureBlock(OVERLAY_DISP++, gStoneOfAgonyIconTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 24, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gDPSetOtherMode(OVERLAY_DISP++, G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_IA16 | G_TL_TILE | G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE, G_AC_NONE | G_ZS_PRIM | G_RM_XLU_SURF | G_RM_XLU_SURF2);
            gSPWideTextureRectangle(OVERLAY_DISP++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2, (rectTop + rectHeight) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
            CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_player.c", 3500);
        }

        if (this->unk_6A0 > 4000000.0f) {
            this->unk_6A0 = 0.0f;
            if (CVar_GetS32("gVisualAgony", 0) !=0 && !this->stateFlags1) {
                //This audio is placed here and not in previous CVar check to prevent ears ra.. :)
                Audio_PlaySoundGeneral(NA_SE_SY_MESSAGE_WOMAN, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E0);
            }
            func_8083264C(this, 120, 20, 10, 0);
        }
    }
}

static s8 D_808547C4[] = {
    0,  3,  3,  5,   4,   8,   9,   13, 14, 15, 16, 17, 18, -22, 23, 24, 25,  26, 27,  28,  29, 31, 32, 33, 34, -35,
    30, 36, 38, -39, -40, -41, 42,  43, 45, 46, 0,  0,  0,  67,  48, 47, -50, 51, -52, -53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63,  64,  -65, -66, 68, 11, 69, 70, 71, 8,  8,   72, 73, 78,  79, 80,  89,  90, 91, 92, 77, 19, 94,
};

static Vec3f D_80854814 = { 0.0f, 0.0f, 200.0f };

static f32 D_80854820[] = { 2.0f, 4.0f, 7.0f };
static f32 D_8085482C[] = { 0.5f, 1.0f, 3.0f };

void Player_UpdateCommon(Player* this, GlobalContext* globalCtx, Input* input) {
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

    if (this->unk_88E != 0) {
        this->unk_88E--;
    }

    if (this->unk_A87 != 0) {
        this->unk_A87--;
    }

    if (this->invincibilityTimer < 0) {
        this->invincibilityTimer++;
    }
    else if (this->invincibilityTimer > 0) {
        this->invincibilityTimer--;
    }

    if (this->unk_890 != 0) {
        this->unk_890--;
    }

    func_808473D4(globalCtx, this);
    func_80836BEC(this, globalCtx);

    if ((this->heldItemActionParam == PLAYER_AP_STICK) && (this->unk_860 != 0)) {
        func_80848A04(globalCtx, this);
    }
    else if ((this->heldItemActionParam == PLAYER_AP_FISHING_POLE) && (this->unk_860 < 0)) {
        this->unk_860++;
    }

    if (this->shockTimer != 0) {
        func_80848B44(globalCtx, this);
    }

    if (this->isBurning) {
        func_80848C74(globalCtx, this);
    }

    if ((this->stateFlags3 & PLAYER_STATE3_6) && (gSaveContext.nayrusLoveTimer != 0) && (gSaveContext.unk_13F0 == 0)) {
        gSaveContext.unk_13F0 = 3;
        func_80846A00(globalCtx, this, 1);
        this->stateFlags3 &= ~PLAYER_STATE3_6;
    }

    if (this->stateFlags2 & PLAYER_STATE2_15) {
        if (!(this->actor.bgCheckFlags & 1)) {
            func_80832210(this);
            Actor_MoveForward(&this->actor);
        }

        func_80847BA0(globalCtx, this);
    }
    else {
        f32 temp_f0;
        f32 phi_f12;

        if (this->currentBoots != this->prevBoots) {
            if (this->currentBoots == PLAYER_BOOTS_IRON) {
                if (this->stateFlags1 & PLAYER_STATE1_27) {
                    func_80832340(globalCtx, this);
                    if (this->ageProperties->unk_2C < this->actor.yDistToWater) {
                        this->stateFlags2 |= PLAYER_STATE2_10;
                    }
                }
            }
            else {
                if (this->stateFlags1 & PLAYER_STATE1_27) {
                    if ((this->prevBoots == PLAYER_BOOTS_IRON) || (this->actor.bgCheckFlags & 1)) {
                        func_8083D36C(globalCtx, this);
                        this->stateFlags2 &= ~PLAYER_STATE2_10;
                    }
                }
            }

            this->prevBoots = this->currentBoots;
        }

        if ((this->actor.parent == NULL) && (this->stateFlags1 & PLAYER_STATE1_23)) {
            this->actor.parent = this->rideActor;
            func_8083A360(globalCtx, this);
            this->stateFlags1 |= PLAYER_STATE1_23;
            func_80832264(globalCtx, this, &gPlayerAnim_0033B8);
            func_80832F54(globalCtx, this, 0x9B);
            this->unk_850 = 99;
        }

        if (this->unk_844 == 0) {
            this->unk_845 = 0;
        }
        else if (this->unk_844 < 0) {
            this->unk_844++;
        }
        else {
            this->unk_844--;
        }

        Math_ScaledStepToS(&this->unk_6C2, 0, 400);
        func_80032CB4(this->unk_3A8, 20, 80, 6);

        this->actor.shape.face = this->unk_3A8[0] + ((globalCtx->gameplayFrames & 32) ? 0 : 3);

        if (this->currentMask == PLAYER_MASK_BUNNY) {
            func_8085002C(this);
        }

        if (func_8002DD6C(this) != 0) {
            func_8084FF7C(this);
        }

        if (!(this->skelAnime.moveFlags & 0x80)) {
            if (((this->actor.bgCheckFlags & 1) && (D_808535E4 == 5) && (this->currentBoots != PLAYER_BOOTS_IRON)) ||
                ((this->currentBoots == PLAYER_BOOTS_HOVER) &&
                    !(this->stateFlags1 & (PLAYER_STATE1_27 | PLAYER_STATE1_29)))) {
                f32 sp70 = this->linearVelocity;
                s16 sp6E = this->currentYaw;
                s16 yawDiff = this->actor.world.rot.y - sp6E;
                s32 pad;

                if ((ABS(yawDiff) > 0x6000) && (this->actor.speedXZ != 0.0f)) {
                    sp70 = 0.0f;
                    sp6E += 0x8000;
                }

                if (Math_StepToF(&this->actor.speedXZ, sp70, 0.35f) && (sp70 == 0.0f)) {
                    this->actor.world.rot.y = this->currentYaw;
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
            }
            else {
                this->actor.speedXZ = this->linearVelocity;
                this->actor.world.rot.y = this->currentYaw;
            }

            func_8002D868(&this->actor);

            if ((this->windSpeed != 0.0f) && !Player_InCsMode(globalCtx) &&
                !(this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21)) &&
                (func_80845668 != this->func_674) && (func_808507F4 != this->func_674)) {
                this->actor.velocity.x += this->windSpeed * Math_SinS(this->windDirection);
                this->actor.velocity.z += this->windSpeed * Math_CosS(this->windDirection);
            }

            func_8002D7EC(&this->actor);
            func_80847BA0(globalCtx, this);
        }
        else {
            D_808535E4 = 0;
            this->unk_A7A = 0;

            if (!(this->stateFlags1 & PLAYER_STATE1_0) && (this->stateFlags1 & PLAYER_STATE1_23)) {
                EnHorse* rideActor = (EnHorse*)this->rideActor;
                CollisionPoly* sp5C;
                s32 sp58;
                Vec3f sp4C;

                if (!(rideActor->actor.bgCheckFlags & 1)) {
                    func_808396F4(globalCtx, this, &D_80854814, &sp4C, &sp5C, &sp58);
                }
                else {
                    sp5C = rideActor->actor.floorPoly;
                    sp58 = rideActor->actor.floorBgId;
                }

                if ((sp5C != NULL) && func_80839034(globalCtx, this, sp5C, sp58)) {
                    if (DREG(25) != 0) {
                        DREG(25) = 0;
                    }
                    else {
                        AREG(6) = 1;
                    }
                }
            }

            D_808535F4 = 0;
            this->windSpeed = 0.0f;
        }

        if ((D_808535F4 != 0) && (this->currentBoots != PLAYER_BOOTS_IRON)) {
            f32 sp48;

            D_808535F4--;

            if (D_808535F8 == 0) {
                sp48 = D_80854820[D_808535F4];

                if (!(this->stateFlags1 & PLAYER_STATE1_27)) {
                    sp48 *= 0.25f;
                }
            }
            else {
                sp48 = D_8085482C[D_808535F4];
            }

            Math_StepToF(&this->windSpeed, sp48, sp48 * 0.1f);

            Math_ScaledStepToS(&this->windDirection, D_808535FC,
                ((this->stateFlags1 & PLAYER_STATE1_27) ? 400.0f : 800.0f) * sp48);
        }
        else if (this->windSpeed != 0.0f) {
            Math_StepToF(&this->windSpeed, 0.0f, (this->stateFlags1 & PLAYER_STATE1_27) ? 0.5f : 1.0f);
        }

        if (!Player_InBlockingCsMode(globalCtx, this) && !(this->stateFlags2 & PLAYER_STATE2_18)) {
            func_8083D53C(globalCtx, this);

            if ((this->actor.category == ACTORCAT_PLAYER) && (gSaveContext.health == 0)) {
                if (this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21)) {
                    func_80832440(globalCtx, this);
                    func_80837B9C(this, globalCtx);
                }
                else if ((this->actor.bgCheckFlags & 1) || (this->stateFlags1 & PLAYER_STATE1_27)) {
                    func_80836448(globalCtx, this,
                        func_808332B8(this) ? &gPlayerAnim_003310
                        : (this->shockTimer != 0) ? &gPlayerAnim_002F08
                        : &gPlayerAnim_002878);
                }
            }
            else {
                if ((this->actor.parent == NULL) &&
                    ((globalCtx->sceneLoadFlag == 0x14) || (this->unk_A87 != 0) || !func_808382DC(this, globalCtx))) {
                    func_8083AA10(this, globalCtx);
                }
                else {
                    this->fallStartHeight = this->actor.world.pos.y;
                }
                func_80848EF8(this, globalCtx);
            }
        }

        if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (this->csMode != 6) &&
            !(this->stateFlags1 & PLAYER_STATE1_23) && !(this->stateFlags2 & PLAYER_STATE2_7) &&
            (this->actor.category == ACTORCAT_PLAYER)) {
            CsCmdActorAction* linkActionCsCmd = globalCtx->csCtx.linkAction;

            if ((linkActionCsCmd != NULL) && (D_808547C4[linkActionCsCmd->action] != 0)) {
                func_8002DF54(globalCtx, NULL, 6);
                func_80832210(this);
            }
            else if ((this->csMode == 0) && !(this->stateFlags2 & PLAYER_STATE2_10) &&
                (globalCtx->csCtx.state != CS_STATE_UNSKIPPABLE_INIT)) {
                func_8002DF54(globalCtx, NULL, 0x31);
                func_80832210(this);
            }
        }

        if (this->csMode != 0) {
            if ((this->csMode != 7) ||
                !(this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21 | PLAYER_STATE1_26))) {
                this->unk_6AD = 3;
            }
            else if (func_80852E14 != this->func_674) {
                func_80852944(globalCtx, this, NULL);
            }
        }
        else {
            this->prevCsMode = 0;
        }

        func_8083D6EC(globalCtx, this);

        if ((this->unk_664 == NULL) && (this->naviTextId == 0)) {
            this->stateFlags2 &= ~(PLAYER_STATE2_1 | PLAYER_STATE2_21);
        }

        this->stateFlags1 &= ~(PLAYER_STATE1_1 | PLAYER_STATE1_9 | PLAYER_STATE1_12 | PLAYER_STATE1_22);
        this->stateFlags2 &= ~(PLAYER_STATE2_0 | PLAYER_STATE2_2 | PLAYER_STATE2_3 | PLAYER_STATE2_5 | PLAYER_STATE2_6 |
            PLAYER_STATE2_8 | PLAYER_STATE2_9 | PLAYER_STATE2_12 | PLAYER_STATE2_14 |
            PLAYER_STATE2_16 | PLAYER_STATE2_22 | PLAYER_STATE2_26);
        this->stateFlags3 &= ~PLAYER_STATE3_4;

        func_80847298(this);
        func_8083315C(globalCtx, this);

        if (this->stateFlags1 & PLAYER_STATE1_27) {
            D_808535E8 = 0.5f;
        }
        else {
            D_808535E8 = 1.0f;
        }

        D_808535EC = 1.0f / D_808535E8;
        D_80853614 = D_80853618 = 0;
        D_80858AA4 = this->currentMask;

        if (!(this->stateFlags3 & PLAYER_STATE3_2)) {
            this->func_674(this, globalCtx);
        }

        Player_UpdateCamAndSeqModes(globalCtx, this);

        if (this->skelAnime.moveFlags & 8) {
            AnimationContext_SetMoveActor(globalCtx, &this->actor, &this->skelAnime,
                (this->skelAnime.moveFlags & 4) ? 1.0f : this->ageProperties->unk_08);
        }

        func_808368EC(this, globalCtx);

        if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_8)) {
            this->targetActorDistance = 0.0f;
        }
        else {
            this->targetActor = NULL;
            this->targetActorDistance = FLT_MAX;
            this->exchangeItemId = EXCH_ITEM_NONE;
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_11)) {
            this->interactRangeActor = NULL;
            this->getItemDirection = 0x6000;
        }

        if (this->actor.parent == NULL) {
            this->rideActor = NULL;
        }

        this->naviTextId = 0;

        if (!(this->stateFlags2 & PLAYER_STATE2_25)) {
            this->unk_6A8 = NULL;
        }

        this->stateFlags2 &= ~PLAYER_STATE2_23;
        this->unk_6A4 = FLT_MAX;

        temp_f0 = this->actor.world.pos.y - this->actor.prevPos.y;

        this->doorType = PLAYER_DOORTYPE_NONE;
        this->unk_8A1 = 0;
        this->unk_684 = NULL;

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

        if (this->stateFlags1 & PLAYER_STATE1_22) {
            this->cylinder.dim.height = this->cylinder.dim.height * 0.8f;
        }

        Collider_UpdateCylinder(&this->actor, &this->cylinder);

        if (!(this->stateFlags2 & PLAYER_STATE2_14)) {
            if (!(this->stateFlags1 & (PLAYER_STATE1_7 | PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_23))) {
                CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->cylinder.base);
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_7 | PLAYER_STATE1_26)) && (this->invincibilityTimer <= 0)) {
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->cylinder.base);

                if (this->invincibilityTimer < 0) {
                    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &this->cylinder.base);
                }
            }
        }

        AnimationContext_SetNextQueue(globalCtx);
    }

    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
    Math_Vec3f_Copy(&this->unk_A88, &this->bodyPartsPos[PLAYER_BODYPART_WAIST]);

    if (this->stateFlags1 & (PLAYER_STATE1_7 | PLAYER_STATE1_28 | PLAYER_STATE1_29)) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    }
    else {
        this->actor.colChkInfo.mass = 50;
    }

    this->stateFlags3 &= ~PLAYER_STATE3_2;

    Collider_ResetCylinderAC(globalCtx, &this->cylinder.base);

    Collider_ResetQuadAT(globalCtx, &this->swordQuads[0].base);
    Collider_ResetQuadAT(globalCtx, &this->swordQuads[1].base);

    Collider_ResetQuadAC(globalCtx, &this->shieldQuad.base);
    Collider_ResetQuadAT(globalCtx, &this->shieldQuad.base);
}

static Vec3f D_80854838 = { 0.0f, 0.0f, -30.0f };

void Player_Update(Actor* thisx, GlobalContext* globalCtx) {
    static Vec3f sDogSpawnPos;
    Player* this = (Player*)thisx;
    s32 dogParams;
    s32 pad;
    Input sp44;
    Actor* dog;

    if (func_8084FCAC(this, globalCtx)) {
        if (gSaveContext.dogParams < 0) {
            if (Object_GetIndex(&globalCtx->objectCtx, OBJECT_DOG) < 0) {
                gSaveContext.dogParams = 0;
            }
            else {
                gSaveContext.dogParams &= 0x7FFF;
                func_808395DC(this, &this->actor.world.pos, &D_80854838, &sDogSpawnPos);
                dogParams = gSaveContext.dogParams;

                dog = Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_DOG, sDogSpawnPos.x, sDogSpawnPos.y,
                    sDogSpawnPos.z, 0, this->actor.shape.rot.y, 0, dogParams | 0x8000);
                if (dog != NULL) {
                    dog->room = 0;
                }
            }
        }

        if ((this->interactRangeActor != NULL) && (this->interactRangeActor->update == NULL)) {
            this->interactRangeActor = NULL;
        }

        if ((this->heldActor != NULL) && (this->heldActor->update == NULL)) {
            func_808323B4(globalCtx, this);
        }

        if (this->stateFlags1 & (PLAYER_STATE1_5 | PLAYER_STATE1_29)) {
            memset(&sp44, 0, sizeof(sp44));
        }
        else {
            sp44 = globalCtx->state.input[0];
            if (this->unk_88E != 0) {
                sp44.cur.button &= ~(BTN_A | BTN_B | BTN_CUP);
                sp44.press.button &= ~(BTN_A | BTN_B | BTN_CUP);
            }
        }

        Player_UpdateCommon(this, globalCtx, &sp44);
    }

    MREG(52) = this->actor.world.pos.x;
    MREG(53) = this->actor.world.pos.y;
    MREG(54) = this->actor.world.pos.z;
    MREG(55) = this->actor.world.rot.y;
}

static struct_80858AC8 D_80858AC8;
static Vec3s D_80858AD8[25];

static Gfx* sMaskDlists[PLAYER_MASK_MAX - 1] = {
    gLinkChildKeatonMaskDL, gLinkChildSkullMaskDL, gLinkChildSpookyMaskDL, gLinkChildBunnyHoodDL,
    gLinkChildGoronMaskDL,  gLinkChildZoraMaskDL,  gLinkChildGerudoMaskDL, gLinkChildMaskOfTruthDL,
};

static Vec3s D_80854864 = { 0, 0, 0 };

void Player_DrawGameplay(GlobalContext* globalCtx, Player* this, s32 lod, Gfx* cullDList,
    OverrideLimbDrawOpa overrideLimbDraw) {
    static s32 D_8085486C = 255;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_player.c", 19228);

    gSPSegment(POLY_OPA_DISP++, 0x0C, cullDList);
    gSPSegment(POLY_XLU_DISP++, 0x0C, cullDList);

    func_8008F470(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, lod,
        this->currentTunic, this->currentBoots, this->actor.shape.face, overrideLimbDraw,
        func_80090D20, this);

    if ((overrideLimbDraw == func_80090014) && (this->currentMask != PLAYER_MASK_NONE)) {
        Mtx* sp70 = Graph_Alloc(globalCtx->state.gfxCtx, 2 * sizeof(Mtx));

        if (this->currentMask == PLAYER_MASK_BUNNY) {
            Vec3s sp68;

            gSPSegment(POLY_OPA_DISP++, 0x0B, sp70);

            sp68.x = D_80858AC8.unk_02 + 0x3E2;
            sp68.y = D_80858AC8.unk_04 + 0xDBE;
            sp68.z = D_80858AC8.unk_00 - 0x348A;
            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f, -240.0f, &sp68);
            Matrix_ToMtx(sp70++, "../z_player.c", 19273);

            sp68.x = D_80858AC8.unk_02 - 0x3E2;
            sp68.y = -0xDBE - D_80858AC8.unk_04;
            sp68.z = D_80858AC8.unk_00 - 0x348A;
            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f, 240.0f, &sp68);
            Matrix_ToMtx(sp70, "../z_player.c", 19279);
        }

        gSPDisplayList(POLY_OPA_DISP++, sMaskDlists[this->currentMask - 1]);
    }

    if ((this->currentBoots == PLAYER_BOOTS_HOVER) && !(this->actor.bgCheckFlags & 1) &&
        !(this->stateFlags1 & PLAYER_STATE1_23) && (this->hoverBootsTimer != 0)) {
        s32 sp5C;
        s32 hoverBootsTimer = this->hoverBootsTimer;

        if (this->hoverBootsTimer < 19) {
            if (hoverBootsTimer >= 15) {
                D_8085486C = (19 - hoverBootsTimer) * 51.0f;
            }
            else if (hoverBootsTimer < 19) {
                sp5C = hoverBootsTimer;

                if (sp5C > 9) {
                    sp5C = 9;
                }

                D_8085486C = (-sp5C * 4) + 36;
                D_8085486C = D_8085486C * D_8085486C;
                D_8085486C = (s32)((Math_CosS(D_8085486C) * 100.0f) + 100.0f) + 55.0f;
                D_8085486C = D_8085486C * (sp5C * (1.0f / 9.0f));
            }

            Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y + 2.0f,
                this->actor.world.pos.z, &D_80854864);
            Matrix_Scale(4.0f, 4.0f, 4.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_player.c", 19317),
                G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 16, 32, 1, 0,
                    (globalCtx->gameplayFrames * -15) % 128, 16, 32));
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, D_8085486C);
            gDPSetEnvColor(POLY_XLU_DISP++, 120, 90, 30, 128);
            gSPDisplayList(POLY_XLU_DISP++, gHoverBootsCircleDL);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_player.c", 19328);
}

void Player_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    Player* this = (Player*)thisx;

     Vec3f pos;
    Vec3s rot;
    f32 scale;

    // OTRTODO: This is crashing randomly, so its temporarily been disabled
    // return;

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        pos.x = 2.0f;
        pos.y = -130.0f;
        pos.z = -150.0f;
        scale = 0.046f;
    } else if (CUR_EQUIP_VALUE(EQUIP_SWORD) != 2) {
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

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_player.c", 19346);


    if (!(this->stateFlags2 & PLAYER_STATE2_29)) {
        OverrideLimbDrawOpa overrideLimbDraw = func_80090014;
        s32 lod;
        s32 pad;

        if ((this->csMode != 0) || (func_8008E9C4(this) && 0) || (this->actor.projectedPos.z < 160.0f)) {
            lod = 0;
        }
        else {
            lod = 1;
        }

        if (CVar_GetS32("gDisableLOD", 0) != 0)
            lod = 0;

        func_80093C80(globalCtx);
        func_80093D84(globalCtx->state.gfxCtx);

        if (this->invincibilityTimer > 0) {
            this->unk_88F += CLAMP(50 - this->invincibilityTimer, 8, 40);
            POLY_OPA_DISP =
                Gfx_SetFog2(POLY_OPA_DISP, 255, 0, 0, 0, 0, 4000 - (s32)(Math_CosS(this->unk_88F * 256) * 2000.0f));
        }

        func_8002EBCC(&this->actor, globalCtx, 0);
        func_8002ED80(&this->actor, globalCtx, 0);

        if (this->unk_6AD != 0) {
            Vec3f projectedHeadPos;

            SkinMatrix_Vec3fMtxFMultXYZ(&globalCtx->viewProjectionMtxF, &this->actor.focus.pos, &projectedHeadPos);
            if (projectedHeadPos.z < -4.0f) {
                overrideLimbDraw = func_800902F0;
            }
        }
        else if (this->stateFlags2 & PLAYER_STATE2_18) {
            if (this->actor.projectedPos.z < 0.0f) {
                overrideLimbDraw = func_80090440;
            }
        }

        if (this->stateFlags2 & PLAYER_STATE2_26) {
            f32 sp78 = ((u16)(globalCtx->gameplayFrames * 600) * M_PI) / 0x8000;
            f32 sp74 = ((u16)(globalCtx->gameplayFrames * 1000) * M_PI) / 0x8000;

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
            Player_DrawGameplay(globalCtx, this, lod, gCullFrontDList, overrideLimbDraw);
            this->actor.scale.y = -this->actor.scale.y;
            Matrix_Pop();
        }

        gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BOTH);
        gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BOTH);

        Player_DrawGameplay(globalCtx, this, lod, gCullBackDList, overrideLimbDraw);

        if (this->invincibilityTimer > 0) {
            POLY_OPA_DISP = Gameplay_SetFog(globalCtx, POLY_OPA_DISP);
        }

        if (this->stateFlags2 & PLAYER_STATE2_14) {
            f32 scale = (this->unk_84F >> 1) * 22.0f;

            gSPSegment(POLY_XLU_DISP++, 0x08,
                Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, (0 - globalCtx->gameplayFrames) % 128, 32, 32, 1,
                    0, (globalCtx->gameplayFrames * -2) % 128, 32, 32));

            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_player.c", 19459),
                G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, 255);
            gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);
        }

        if (this->unk_862 > 0) {
            Player_DrawGetItem(globalCtx, this);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_player.c", 19473);
}

void Player_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    Player* this = (Player*)thisx;

    Effect_Delete(globalCtx, this->swordEffectIndex);

    Collider_DestroyCylinder(globalCtx, &this->cylinder);
    Collider_DestroyQuad(globalCtx, &this->swordQuads[0]);
    Collider_DestroyQuad(globalCtx, &this->swordQuads[1]);
    Collider_DestroyQuad(globalCtx, &this->shieldQuad);

    func_800876C8(globalCtx);

    gSaveContext.linkAge = globalCtx->linkAgeOnLoad;
}

s16 func_8084ABD8(GlobalContext* globalCtx, Player* this, s32 arg2, s16 arg3) {
    s32 temp1;
    s16 temp2;
    s16 temp3;

    if (!func_8002DD78(this) && !func_808334B4(this) && (arg2 == 0)) {
        temp2 = sControlInput->rel.stick_y * 240.0f;
        Math_SmoothStepToS(&this->actor.focus.rot.x, temp2, 14, 4000, 30);

        temp2 = sControlInput->rel.stick_x * -16.0f;
        temp2 = CLAMP(temp2, -3000, 3000);
        this->actor.focus.rot.y += temp2;
    }
    else {
        temp1 = (this->stateFlags1 & PLAYER_STATE1_23) ? 3500 : 14000;
        temp3 = ((sControlInput->rel.stick_y >= 0) ? 1 : -1) *
            (s32)((1.0f - Math_CosS(sControlInput->rel.stick_y * 200)) * 1500.0f);
        this->actor.focus.rot.x += temp3;

        if (fabsf(sControlInput->cur.gyro_x) > 0.01f) {
            this->actor.focus.rot.x -= (sControlInput->cur.gyro_x) * 750.0f;
        }

        this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -temp1, temp1);

        temp1 = 19114;
        temp2 = this->actor.focus.rot.y - this->actor.shape.rot.y;
        temp3 = ((sControlInput->rel.stick_x >= 0) ? 1 : -1) *
            (s32)((1.0f - Math_CosS(sControlInput->rel.stick_x * 200)) * -1500.0f);
        temp2 += temp3;

        this->actor.focus.rot.y = CLAMP(temp2, -temp1, temp1) + this->actor.shape.rot.y;

        if (fabsf(sControlInput->cur.gyro_y) > 0.01f) {
            this->actor.focus.rot.y += (sControlInput->cur.gyro_y) * 750.0f;
        }
    }

    this->unk_6AE |= 2;
    return func_80836AB8(this, (globalCtx->shootingGalleryStatus != 0) || func_8002DD78(this) || func_808334B4(this)) -
        arg3;
}

void func_8084AEEC(Player* this, f32* arg1, f32 arg2, s16 arg3) {
    f32 temp1;
    f32 temp2;

    temp1 = this->skelAnime.curFrame - 10.0f;

    temp2 = (R_RUN_SPEED_LIMIT / 100.0f) * 0.8f;
    if (*arg1 > temp2) {
        *arg1 = temp2;
    }

    if ((0.0f < temp1) && (temp1 < 10.0f)) {
        temp1 *= 6.0f;
    }
    else {
        temp1 = 0.0f;
        arg2 = 0.0f;
    }

    Math_AsymStepToF(arg1, arg2 * 0.8f, temp1, (fabsf(*arg1) * 0.02f) + 0.05f);
    Math_ScaledStepToS(&this->currentYaw, arg3, 1600);
}

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
        }
        else {
            phi_f16 = this->actor.velocity.y * 0.5f;
        }
        phi_f18 = -0.1f - phi_f16;
    }
    else {
        if (!(this->stateFlags1 & PLAYER_STATE1_7) && (this->currentBoots == PLAYER_BOOTS_IRON) &&
            (this->actor.velocity.y >= -3.0f)) {
            phi_f18 = -0.2f;
        }
        else {
            phi_f14 = 2.0f;
            if (this->actor.velocity.y >= 0.0f) {
                phi_f16 = 0.0f;
            }
            else {
                phi_f16 = this->actor.velocity.y * -0.3f;
            }
            phi_f18 = phi_f16 + 0.1f;
        }

        yDistToWater = this->actor.yDistToWater;
        if (yDistToWater > 100.0f) {
            this->stateFlags2 |= PLAYER_STATE2_10;
        }
    }

    this->actor.velocity.y += phi_f18;

    if (((this->actor.velocity.y - phi_f14) * phi_f18) > 0) {
        this->actor.velocity.y = phi_f14;
    }

    this->actor.gravity = 0.0f;
}

void func_8084B158(GlobalContext* globalCtx, Player* this, Input* input, f32 arg3) {
    f32 temp;

    if ((input != NULL) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B)) {
        temp = 1.0f;
    }
    else {
        temp = 0.5f;
    }

    temp *= arg3;

    if (temp < 1.0f) {
        temp = 1.0f;
    }

    this->skelAnime.playSpeed = temp;
    LinkAnimation_Update(globalCtx, &this->skelAnime);
}

void func_8084B1D8(Player* this, GlobalContext* globalCtx) {
    if (this->stateFlags1 & PLAYER_STATE1_27) {
        func_8084B000(this);
        func_8084AEEC(this, &this->linearVelocity, 0, this->actor.shape.rot.y);
    }
    else {
        func_8083721C(this);
    }

    if ((this->unk_6AD == 2) && (func_8002DD6C(this) || func_808332E4(this))) {
        func_80836670(this, globalCtx);
    }

    if ((this->csMode != 0) || (this->unk_6AD == 0) || (this->unk_6AD >= 4) || func_80833B54(this) ||
        (this->unk_664 != NULL) || !func_8083AD4C(globalCtx, this) ||
        (((this->unk_6AD == 2) && (CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B | BTN_R) ||
            func_80833B2C(this) || (!func_8002DD78(this) && !func_808334B4(this)))) ||
            ((this->unk_6AD == 1) &&
                CHECK_BTN_ANY(sControlInput->press.button,
                    BTN_A | BTN_B | BTN_R | BTN_CUP | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN)))) {
        func_8083C148(this, globalCtx);
        func_80078884(NA_SE_SY_CAMERA_ZOOM_UP);
    }
    else if ((DECR(this->unk_850) == 0) || (this->unk_6AD != 2)) {
        if (func_8008F128(this)) {
            this->unk_6AE |= 0x43;
        }
        else {
            this->actor.shape.rot.y = func_8084ABD8(globalCtx, this, 0, 0);
        }
    }

    this->currentYaw = this->actor.shape.rot.y;
}

s32 func_8084B3CC(GlobalContext* globalCtx, Player* this) {
    if (globalCtx->shootingGalleryStatus != 0) {
        func_80832564(globalCtx, this);
        func_80835C58(globalCtx, this, func_8084FA54, 0);

        if (!func_8002DD6C(this) || Player_HoldsHookshot(this)) {
            func_80835F44(globalCtx, this, 3);
        }

        this->stateFlags1 |= PLAYER_STATE1_20;
        func_80832264(globalCtx, this, func_80833338(this));
        func_80832210(this);
        func_8083B010(this);
        return 1;
    }

    return 0;
}

void func_8084B498(Player* this) {
    this->itemActionParam =
        (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY) ? PLAYER_AP_OCARINA_FAIRY : PLAYER_AP_OCARINA_TIME;
}

s32 func_8084B4D4(GlobalContext* globalCtx, Player* this) {
    if (this->stateFlags3 & PLAYER_STATE3_5) {
        this->stateFlags3 &= ~PLAYER_STATE3_5;
        func_8084B498(this);
        this->unk_6AD = 4;
        func_8083B040(this, globalCtx);
        return 1;
    }

    return 0;
}

void func_8084B530(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_5;
    func_80836670(this, globalCtx);

    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~ACTOR_FLAG_8;

        if (!CHECK_FLAG_ALL(this->targetActor->flags, ACTOR_FLAG_0 | ACTOR_FLAG_2)) {
            this->stateFlags2 &= ~PLAYER_STATE2_13;
        }

        func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));

        if (!func_8084B4D4(globalCtx, this) && !func_8084B3CC(globalCtx, this) && !func_8083ADD4(globalCtx, this)) {
            if ((this->targetActor != this->interactRangeActor) || !func_8083E5A8(this, globalCtx)) {
                if (this->stateFlags1 & PLAYER_STATE1_23) {
                    s32 sp24 = this->unk_850;
                    func_8083A360(globalCtx, this);
                    this->unk_850 = sp24;
                }
                else if (func_808332B8(this)) {
                    func_80838F18(globalCtx, this);
                }
                else {
                    func_80853080(this, globalCtx);
                }
            }
        }

        this->unk_88E = 10;
        return;
    }

    if (this->stateFlags1 & PLAYER_STATE1_23) {
        func_8084CC98(this, globalCtx);
    }
    else if (func_808332B8(this)) {
        func_8084D610(this, globalCtx);
    }
    else if (!func_8008E9C4(this) && LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (this->skelAnime.moveFlags != 0) {
            func_80832DBC(this);
            if ((this->targetActor->category == ACTORCAT_NPC) &&
                (this->heldItemActionParam != PLAYER_AP_FISHING_POLE)) {
                func_808322D0(globalCtx, this, &gPlayerAnim_0031A0);
            }
            else {
                func_80832284(globalCtx, this, func_80833338(this));
            }
        }
        else {
            func_808322A4(globalCtx, this, &gPlayerAnim_0031A8);
        }
    }

    if (this->unk_664 != NULL) {
        this->currentYaw = this->actor.shape.rot.y = func_8083DB98(this, 0);
    }
}

void func_8084B78C(Player* this, GlobalContext* globalCtx) {
    f32 sp34;
    s16 sp32;
    s32 temp;

    this->stateFlags2 |= PLAYER_STATE2_0 | PLAYER_STATE2_6 | PLAYER_STATE2_8;
    func_8083F524(globalCtx, this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (!func_8083F9D0(globalCtx, this)) {
            func_80837268(this, &sp34, &sp32, 0.0f, globalCtx);
            temp = func_8083FFB8(this, &sp34, &sp32);
            if (temp > 0) {
                func_8083FAB8(this, globalCtx);
            }
            else if (temp < 0) {
                func_8083FB14(this, globalCtx);
            }
        }
    }
}

void func_8084B840(GlobalContext* globalCtx, Player* this, f32 arg2) {
    if (this->actor.wallBgId != BGCHECK_SCENE) {
        DynaPolyActor* dynaPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, this->actor.wallBgId);

        if (dynaPolyActor != NULL) {
            func_8002DFA4(dynaPolyActor, arg2, this->actor.world.rot.y);
        }
    }
}

static struct_80832924 D_80854870[] = {
    { NA_SE_PL_SLIP, 0x1003 },
    { NA_SE_PL_SLIP, -0x1015 },
};

void func_8084B898(Player* this, GlobalContext* globalCtx) {
    f32 sp34;
    s16 sp32;
    s32 temp;

    this->stateFlags2 |= PLAYER_STATE2_0 | PLAYER_STATE2_6 | PLAYER_STATE2_8;

    if (func_80832CB0(globalCtx, this, &gPlayerAnim_003108)) {
        this->unk_850 = 1;
    }
    else if (this->unk_850 == 0) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 11.0f)) {
            func_80832698(this, NA_SE_VO_LI_PUSH);
        }
    }

    func_80832924(this, D_80854870);
    func_8083F524(globalCtx, this);

    if (!func_8083F9D0(globalCtx, this)) {
        func_80837268(this, &sp34, &sp32, 0.0f, globalCtx);
        temp = func_8083FFB8(this, &sp34, &sp32);
        if (temp < 0) {
            func_8083FB14(this, globalCtx);
        }
        else if (temp == 0) {
            func_8083F72C(this, &gPlayerAnim_0030E0, globalCtx);
        }
        else {
            this->stateFlags2 |= PLAYER_STATE2_4;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_4) {
        func_8084B840(globalCtx, this, 2.0f);
        this->linearVelocity = 2.0f;
    }
}

static struct_80832924 D_80854878[] = {
    { NA_SE_PL_SLIP, 0x1004 },
    { NA_SE_PL_SLIP, -0x1018 },
};

static Vec3f D_80854880 = { 0.0f, 26.0f, -40.0f };

void func_8084B9E4(Player* this, GlobalContext* globalCtx) {
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

    anim = D_80853914[PLAYER_ANIMGROUP_36][this->modelAnimType];
    this->stateFlags2 |= PLAYER_STATE2_0 | PLAYER_STATE2_6 | PLAYER_STATE2_8;

    if (func_80832CB0(globalCtx, this, anim)) {
        this->unk_850 = 1;
    }
    else {
        if (this->unk_850 == 0) {
            if (LinkAnimation_OnFrame(&this->skelAnime, 11.0f)) {
                func_80832698(this, NA_SE_VO_LI_PUSH);
            }
        }
        else {
            func_80832924(this, D_80854878);
        }
    }

    func_8083F524(globalCtx, this);

    if (!func_8083F9D0(globalCtx, this)) {
        func_80837268(this, &sp70, &sp6E, 0.0f, globalCtx);
        temp1 = func_8083FFB8(this, &sp70, &sp6E);
        if (temp1 > 0) {
            func_8083FAB8(this, globalCtx);
        }
        else if (temp1 == 0) {
            func_8083F72C(this, D_80853914[PLAYER_ANIMGROUP_37][this->modelAnimType], globalCtx);
        }
        else {
            this->stateFlags2 |= PLAYER_STATE2_4;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_4) {
        temp2 = func_8083973C(globalCtx, this, &D_80854880, &sp5C) - this->actor.world.pos.y;
        if (fabsf(temp2) < 20.0f) {
            sp44.x = this->actor.world.pos.x;
            sp44.z = this->actor.world.pos.z;
            sp44.y = sp5C.y;
            if (!BgCheck_EntityLineTest1(&globalCtx->colCtx, &sp44, &sp5C, &sp38, &sp54, true, false, false, true,
                &sp50)) {
                func_8084B840(globalCtx, this, -2.0f);
                return;
            }
        }
        this->stateFlags2 &= ~PLAYER_STATE2_4;
    }
}

void func_8084BBE4(Player* this, GlobalContext* globalCtx) {
    f32 sp3C;
    s16 sp3A;
    LinkAnimationHeader* anim;
    f32 temp;

    this->stateFlags2 |= PLAYER_STATE2_6;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        // clang-format off
        anim = (this->unk_84F > 0) ? &gPlayerAnim_002F28 : D_80853914[PLAYER_ANIMGROUP_40][this->modelAnimType]; func_80832284(globalCtx, this, anim);
        // clang-format on
    }
    else if (this->unk_84F == 0) {
        if (this->skelAnime.animation == &gPlayerAnim_002F10) {
            temp = 11.0f;
        }
        else {
            temp = 1.0f;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, temp)) {
            func_80832770(this, NA_SE_PL_WALK_GROUND);
            if (this->skelAnime.animation == &gPlayerAnim_002F10) {
                this->unk_84F = 1;
            }
            else {
                this->unk_84F = -1;
            }
        }
    }

    Math_ScaledStepToS(&this->actor.shape.rot.y, this->currentYaw, 0x800);

    if (this->unk_84F != 0) {
        func_80837268(this, &sp3C, &sp3A, 0.0f, globalCtx);
        if (this->unk_847[this->unk_846] >= 0) {
            if (this->unk_84F > 0) {
                anim = D_80853914[PLAYER_ANIMGROUP_38][this->modelAnimType];
            }
            else {
                anim = D_80853914[PLAYER_ANIMGROUP_41][this->modelAnimType];
            }
            func_8083A9B8(this, anim, globalCtx);
            return;
        }

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A) || (this->actor.shape.feetFloorFlags != 0)) {
            func_80837B60(this);
            if (this->unk_84F < 0) {
                this->linearVelocity = -0.8f;
            }
            else {
                this->linearVelocity = 0.8f;
            }
            func_80837B9C(this, globalCtx);
            this->stateFlags1 &= ~(PLAYER_STATE1_13 | PLAYER_STATE1_14);
        }
    }
}

void func_8084BDFC(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_6;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80832E48(this, 1);
        func_8083C0E8(this, globalCtx);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, this->skelAnime.endFrame - 6.0f)) {
        func_808328A0(this);
    }
    else if (LinkAnimation_OnFrame(&this->skelAnime, this->skelAnime.endFrame - 34.0f)) {
        this->stateFlags1 &= ~(PLAYER_STATE1_13 | PLAYER_STATE1_14);
        func_8002F7DC(&this->actor, NA_SE_PL_CLIMB_CLIFF);
        func_80832698(this, NA_SE_VO_LI_CLIMB_END);
    }
}

void func_8084BEE4(Player* this) {
    func_8002F7DC(&this->actor, (this->unk_84F != 0) ? NA_SE_PL_WALK_WALL : NA_SE_PL_WALK_LADDER);
}

void func_8084BF1C(Player* this, GlobalContext* globalCtx) {
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
    this->stateFlags2 |= PLAYER_STATE2_6;

    if ((this->unk_84F != 0) && (ABS(sp84) < ABS(sp80))) {
        phi_f0 = ABS(sp80) * 0.0325f;
        sp84 = 0;
    }
    else {
        phi_f0 = ABS(sp84) * 0.05f;
        sp80 = 0;
    }

    if (phi_f0 < 1.0f) {
        phi_f0 = 1.0f;
    }
    else if (phi_f0 > 3.35f) {
        phi_f0 = 3.35f;
    }

    if (this->skelAnime.playSpeed >= 0.0f) {
        phi_f2 = 1.0f;
    }
    else {
        phi_f2 = -1.0f;
    }

    this->skelAnime.playSpeed = phi_f2 * phi_f0;

    if (this->unk_850 >= 0) {
        if ((this->actor.wallPoly != NULL) && (this->actor.wallBgId != BGCHECK_SCENE)) {
            DynaPolyActor* wallPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, this->actor.wallBgId);
            if (wallPolyActor != NULL) {
                Math_Vec3f_Diff(&wallPolyActor->actor.world.pos, &wallPolyActor->actor.prevPos, &sp6C);
                Math_Vec3f_Sum(&this->actor.world.pos, &sp6C, &this->actor.world.pos);
            }
        }

        Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 26.0f, 6.0f, this->ageProperties->unk_00, 7);
        func_8083F360(globalCtx, this, 26.0f, this->ageProperties->unk_3C, 50.0f, -20.0f);
    }

    if ((this->unk_850 < 0) || !func_8083FBC0(this, globalCtx)) {
        if (LinkAnimation_Update(globalCtx, &this->skelAnime) != 0) {
            if (this->unk_850 < 0) {
                this->unk_850 = ABS(this->unk_850) & 1;
                return;
            }

            if (sp84 != 0) {
                sp68 = this->unk_84F + this->unk_850;

                if (sp84 > 0) {
                    D_8085488C.y = this->ageProperties->unk_40;
                    temp_f0 = func_8083973C(globalCtx, this, &D_8085488C, &sp5C);

                    if (this->actor.world.pos.y < temp_f0) {
                        if (this->unk_84F != 0) {
                            this->actor.world.pos.y = temp_f0;
                            this->stateFlags1 &= ~PLAYER_STATE1_21;
                            func_8083A5C4(globalCtx, this, this->actor.wallPoly, this->ageProperties->unk_3C,
                                &gPlayerAnim_003000);
                            this->currentYaw += 0x8000;
                            this->actor.shape.rot.y = this->currentYaw;
                            func_8083A9B8(this, &gPlayerAnim_003000, globalCtx);
                            this->stateFlags1 |= PLAYER_STATE1_14;
                        }
                        else {
                            func_8083F070(this, this->ageProperties->unk_CC[this->unk_850], globalCtx);
                        }
                    }
                    else {
                        this->skelAnime.prevTransl = this->ageProperties->unk_4A[sp68];
                        func_80832264(globalCtx, this, this->ageProperties->unk_AC[sp68]);
                    }
                }
                else {
                    if ((this->actor.world.pos.y - this->actor.floorHeight) < 15.0f) {
                        if (this->unk_84F != 0) {
                            func_8083FB7C(this, globalCtx);
                        }
                        else {
                            if (this->unk_850 != 0) {
                                this->skelAnime.prevTransl = this->ageProperties->unk_44;
                            }
                            func_8083F070(this, this->ageProperties->unk_C4[this->unk_850], globalCtx);
                            this->unk_850 = 1;
                        }
                    }
                    else {
                        sp68 ^= 1;
                        this->skelAnime.prevTransl = this->ageProperties->unk_62[sp68];
                        anim1 = this->ageProperties->unk_AC[sp68];
                        LinkAnimation_Change(globalCtx, &this->skelAnime, anim1, -1.0f, Animation_GetLastFrame(anim1),
                            0.0f, ANIMMODE_ONCE, 0.0f);
                    }
                }
                this->unk_850 ^= 1;
            }
            else {
                if ((this->unk_84F != 0) && (sp80 != 0)) {
                    anim2 = this->ageProperties->unk_BC[this->unk_850];

                    if (sp80 > 0) {
                        this->skelAnime.prevTransl = this->ageProperties->unk_7A[this->unk_850];
                        func_80832264(globalCtx, this, anim2);
                    }
                    else {
                        this->skelAnime.prevTransl = this->ageProperties->unk_86[this->unk_850];
                        LinkAnimation_Change(globalCtx, &this->skelAnime, anim2, -1.0f, Animation_GetLastFrame(anim2),
                            0.0f, ANIMMODE_ONCE, 0.0f);
                    }
                }
                else {
                    this->stateFlags2 |= PLAYER_STATE2_12;
                }
            }

            return;
        }
    }

    if (this->unk_850 < 0) {
        if (((this->unk_850 == -2) &&
            (LinkAnimation_OnFrame(&this->skelAnime, 14.0f) || LinkAnimation_OnFrame(&this->skelAnime, 29.0f))) ||
            ((this->unk_850 == -4) &&
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

static struct_80832924 D_808548A8[] = {
    { NA_SE_PL_WALK_LADDER, 0x80A },
    { NA_SE_PL_WALK_LADDER, 0x814 },
    { NA_SE_PL_WALK_LADDER, -0x81E },
};

void func_8084C5F8(Player* this, GlobalContext* globalCtx) {
    s32 temp;
    f32* sp38;
    CollisionPoly* sp34;
    s32 sp30;
    Vec3f sp24;

    this->stateFlags2 |= PLAYER_STATE2_6;

    temp = func_808374A0(globalCtx, this, &this->skelAnime, 4.0f);

    if (temp == 0) {
        this->stateFlags1 &= ~PLAYER_STATE1_21;
        return;
    }

    if ((temp > 0) || LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_8083C0E8(this, globalCtx);
        this->stateFlags1 &= ~PLAYER_STATE1_21;
        return;
    }

    sp38 = D_80854898;

    if (this->unk_850 != 0) {
        func_80832924(this, D_808548A8);
        sp38 = D_808548A0;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, sp38[0]) || LinkAnimation_OnFrame(&this->skelAnime, sp38[1])) {
        sp24.x = this->actor.world.pos.x;
        sp24.y = this->actor.world.pos.y + 20.0f;
        sp24.z = this->actor.world.pos.z;
        if (BgCheck_EntityRaycastFloor3(&globalCtx->colCtx, &sp34, &sp30, &sp24) != 0.0f) {
            this->unk_89E = func_80041F10(&globalCtx->colCtx, sp34, sp30);
            func_808328A0(this);
        }
    }
}

static struct_80832924 D_808548B4[] = {
    { 0, 0x3028 }, { 0, 0x3030 }, { 0, 0x3038 }, { 0, 0x3040 },  { 0, 0x3048 },
    { 0, 0x3050 }, { 0, 0x3058 }, { 0, 0x3060 }, { 0, -0x3068 },
};

void func_8084C760(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_6;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_0)) {
            if (this->skelAnime.moveFlags != 0) {
                this->skelAnime.moveFlags = 0;
                return;
            }

            if (!func_8083F570(this, globalCtx)) {
                this->linearVelocity = sControlInput->rel.stick_y * 0.03f;
            }
        }
        return;
    }

    func_80832924(this, D_808548B4);
}

static struct_80832924 D_808548D8[] = {
    { 0, 0x300A }, { 0, 0x3012 }, { 0, 0x301A }, { 0, 0x3022 },  { 0, 0x3034 },
    { 0, 0x303C }, { 0, 0x3044 }, { 0, 0x304C }, { 0, -0x3054 },
};

void func_8084C81C(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_6;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_8083C0E8(this, globalCtx);
        this->stateFlags2 &= ~PLAYER_STATE2_18;
        return;
    }

    func_80832924(this, D_808548D8);
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

s32 func_8084C89C(GlobalContext* globalCtx, Player* this, s32 arg2, f32* arg3) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    f32 sp50;
    f32 sp4C;
    Vec3f sp40;
    Vec3f sp34;
    CollisionPoly* sp30;
    s32 sp2C;

    sp50 = rideActor->actor.world.pos.y + 20.0f;
    sp4C = rideActor->actor.world.pos.y - 20.0f;

    *arg3 = func_8083973C(globalCtx, this, &D_808548FC[arg2], &sp40);

    return (sp4C < *arg3) && (*arg3 < sp50) &&
        !func_80839768(globalCtx, this, &D_80854914[arg2], &sp30, &sp2C, &sp34) &&
        !func_80839768(globalCtx, this, &D_8085492C[arg2], &sp30, &sp2C, &sp34);
}

s32 func_8084C9BC(Player* this, GlobalContext* globalCtx) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    s32 sp38;
    f32 sp34;

    if (this->unk_850 < 0) {
        this->unk_850 = 99;
    }
    else {
        sp38 = (this->mountSide < 0) ? 0 : 1;
        if (!func_8084C89C(globalCtx, this, sp38, &sp34)) {
            sp38 ^= 1;
            if (!func_8084C89C(globalCtx, this, sp38, &sp34)) {
                return 0;
            }
            else {
                this->mountSide = -this->mountSide;
            }
        }

        if ((globalCtx->csCtx.state == CS_STATE_IDLE) && (globalCtx->transitionMode == 0) &&
            (EN_HORSE_CHECK_1(rideActor) || EN_HORSE_CHECK_4(rideActor))) {
            this->stateFlags2 |= PLAYER_STATE2_22;

            if (EN_HORSE_CHECK_1(rideActor) ||
                (EN_HORSE_CHECK_4(rideActor) && CHECK_BTN_ALL(sControlInput->press.button, BTN_A))) {
                rideActor->actor.child = NULL;
                func_80835DAC(globalCtx, this, func_8084D3E4, 0);
                this->unk_878 = sp34 - rideActor->actor.world.pos.y;
                func_80832264(globalCtx, this, (this->mountSide < 0) ? &gPlayerAnim_003390 : &gPlayerAnim_0033A0);
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
            }
            else {
                dir = -1;
            }
            temp = dir * arg1;
        }
        else {
            temp = this->unk_878;
        }
        this->actor.world.pos.y += temp;
        this->unk_878 -= temp;
    }
}

static LinkAnimationHeader* D_80854944[] = {
    &gPlayerAnim_003370,
    &gPlayerAnim_003368,
    &gPlayerAnim_003380,
    &gPlayerAnim_003358,
    &gPlayerAnim_003338,
    &gPlayerAnim_003348,
    &gPlayerAnim_003350,
    NULL,
    NULL,
};

static LinkAnimationHeader* D_80854968[] = {
    &gPlayerAnim_003388,
    &gPlayerAnim_003388,
    &gPlayerAnim_003388,
    &gPlayerAnim_003360,
    &gPlayerAnim_003340,
    &gPlayerAnim_003340,
    &gPlayerAnim_003340,
    NULL,
    NULL,
};

static LinkAnimationHeader* D_8085498C[] = {
    &gPlayerAnim_0033C8,
    &gPlayerAnim_0033B8,
    &gPlayerAnim_0033C0,
};

static u8 D_80854998[2][2] = {
    { 32, 58 },
    { 25, 42 },
};

static Vec3s D_8085499C = { -69, 7146, -266 };

static struct_80832924 D_808549A4[] = {
    { NA_SE_PL_CALM_HIT, 0x830 }, { NA_SE_PL_CALM_HIT, 0x83A },  { NA_SE_PL_CALM_HIT, 0x844 },
    { NA_SE_PL_CALM_PAT, 0x85C }, { NA_SE_PL_CALM_PAT, 0x86E },  { NA_SE_PL_CALM_PAT, 0x87E },
    { NA_SE_PL_CALM_PAT, 0x884 }, { NA_SE_PL_CALM_PAT, -0x888 },
};

void func_8084CC98(Player* this, GlobalContext* globalCtx) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    u8* arr;

    this->stateFlags2 |= PLAYER_STATE2_6;

    func_8084CBF4(this, 1.0f, 10.0f);

    if (this->unk_850 == 0) {
        if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
            this->skelAnime.animation = &gPlayerAnim_0033B8;
            this->unk_850 = 99;
            return;
        }

        arr = D_80854998[(this->mountSide < 0) ? 0 : 1];

        if (LinkAnimation_OnFrame(&this->skelAnime, arr[0])) {
            func_8002F7DC(&this->actor, NA_SE_PL_CLIMB_CLIFF);
            return;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, arr[1])) {
            func_8002DE74(globalCtx, this);
            func_8002F7DC(&this->actor, NA_SE_PL_SIT_ON_HORSE);
            return;
        }

        return;
    }

    func_8002DE74(globalCtx, this);
    this->skelAnime.prevTransl = D_8085499C;

    if ((rideActor->animationIdx != this->unk_850) && ((rideActor->animationIdx >= 2) || (this->unk_850 >= 2))) {
        if ((this->unk_850 = rideActor->animationIdx) < 2) {
            f32 rand = Rand_ZeroOne();
            s32 temp = 0;

            this->unk_850 = 1;

            if (rand < 0.1f) {
                temp = 2;
            }
            else if (rand < 0.2f) {
                temp = 1;
            }
            func_80832264(globalCtx, this, D_8085498C[temp]);
        }
        else {
            this->skelAnime.animation = D_80854944[this->unk_850 - 2];
            Animation_SetMorph(globalCtx, &this->skelAnime, 8.0f);
            if (this->unk_850 < 4) {
                func_80834644(globalCtx, this);
                this->unk_84F = 0;
            }
        }
    }

    if (this->unk_850 == 1) {
        if ((D_808535E0 != 0) || func_8083224C(globalCtx)) {
            func_80832264(globalCtx, this, &gPlayerAnim_0033C8);
        }
        else if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
            this->unk_850 = 99;
        }
        else if (this->skelAnime.animation == &gPlayerAnim_0033B8) {
            func_80832924(this, D_808549A4);
        }
    }
    else {
        this->skelAnime.curFrame = rideActor->curFrame;
        LinkAnimation_AnimateFrame(globalCtx, &this->skelAnime);
    }

    AnimationContext_SetCopyAll(globalCtx, this->skelAnime.limbCount, this->skelAnime.morphTable,
        this->skelAnime.jointTable);

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) || (this->csMode != 0)) {
        if (this->csMode == 7) {
            this->csMode = 0;
        }
        this->unk_6AD = 0;
        this->unk_84F = 0;
    }
    else if ((this->unk_850 < 2) || (this->unk_850 >= 4)) {
        D_808535E0 = func_80836670(this, globalCtx);
        if (D_808535E0 != 0) {
            this->unk_84F = 0;
        }
    }

    this->actor.world.pos.x = rideActor->actor.world.pos.x + rideActor->riderPos.x;
    this->actor.world.pos.y = (rideActor->actor.world.pos.y + rideActor->riderPos.y) - 27.0f;
    this->actor.world.pos.z = rideActor->actor.world.pos.z + rideActor->riderPos.z;

    this->currentYaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

    if ((this->csMode != 0) ||
        (!func_8083224C(globalCtx) && ((rideActor->actor.speedXZ != 0.0f) || !func_8083B644(this, globalCtx)) &&
            !func_8083C1DC(this, globalCtx))) {
        if (D_808535E0 == 0) {
            if (this->unk_84F != 0) {
                if (LinkAnimation_Update(globalCtx, &this->skelAnime2)) {
                    rideActor->stateFlags &= ~ENHORSE_FLAG_8;
                    this->unk_84F = 0;
                }

                if (this->skelAnime2.animation == &gPlayerAnim_0033B0) {
                    if (LinkAnimation_OnFrame(&this->skelAnime2, 23.0f)) {
                        func_8002F7DC(&this->actor, NA_SE_IT_LASH);
                        func_80832698(this, NA_SE_VO_LI_LASH);
                    }

                    AnimationContext_SetCopyAll(globalCtx, this->skelAnime.limbCount, this->skelAnime.jointTable,
                        this->skelAnime2.jointTable);
                }
                else {
                    if (LinkAnimation_OnFrame(&this->skelAnime2, 10.0f)) {
                        func_8002F7DC(&this->actor, NA_SE_IT_LASH);
                        func_80832698(this, NA_SE_VO_LI_LASH);
                    }

                    AnimationContext_SetCopyTrue(globalCtx, this->skelAnime.limbCount, this->skelAnime.jointTable,
                        this->skelAnime2.jointTable, D_80853410);
                }
            }
            else {
                LinkAnimationHeader* anim = NULL;

                if (EN_HORSE_CHECK_3(rideActor)) {
                    anim = &gPlayerAnim_0033B0;
                }
                else if (EN_HORSE_CHECK_2(rideActor)) {
                    if ((this->unk_850 >= 2) && (this->unk_850 != 99)) {
                        anim = D_80854968[this->unk_850 - 2];
                    }
                }

                if (anim != NULL) {
                    LinkAnimation_PlayOnce(globalCtx, &this->skelAnime2, anim);
                    this->unk_84F = 1;
                }
            }
        }

        if (this->stateFlags1 & PLAYER_STATE1_20) {
            if (!func_8083AD4C(globalCtx, this) || CHECK_BTN_ANY(sControlInput->press.button, BTN_A) ||
                func_80833BCC(this)) {
                this->unk_6AD = 0;
                this->stateFlags1 &= ~PLAYER_STATE1_20;
            }
            else {
                this->unk_6BE = func_8084ABD8(globalCtx, this, 1, -5000) - this->actor.shape.rot.y;
                this->unk_6BE += 5000;
                this->unk_6B0 = -5000;
            }
            return;
        }

        if ((this->csMode != 0) || (!func_8084C9BC(this, globalCtx) && !func_8083B040(this, globalCtx))) {
            if (this->unk_664 != NULL) {
                if (func_8002DD78(this) != 0) {
                    this->unk_6BE = func_8083DB98(this, 1) - this->actor.shape.rot.y;
                    this->unk_6BE = CLAMP(this->unk_6BE, -0x4AAA, 0x4AAA);
                    this->actor.focus.rot.y = this->actor.shape.rot.y + this->unk_6BE;
                    this->unk_6BE += 5000;
                    this->unk_6AE |= 0x80;
                }
                else {
                    func_8083DB98(this, 0);
                }
            }
            else {
                if (func_8002DD78(this) != 0) {
                    this->unk_6BE = func_8084ABD8(globalCtx, this, 1, -5000) - this->actor.shape.rot.y;
                    this->unk_6BE += 5000;
                    this->unk_6B0 = -5000;
                }
            }
        }
    }
}

static struct_80832924 D_808549C4[] = {
    { 0, 0x2800 },
    { NA_SE_PL_GET_OFF_HORSE, 0x80A },
    { NA_SE_PL_SLIPDOWN, -0x819 },
};

void func_8084D3E4(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_6;
    func_8084CBF4(this, 1.0f, 10.0f);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        EnHorse* rideActor = (EnHorse*)this->rideActor;

        func_8083C0E8(this, globalCtx);
        this->stateFlags1 &= ~PLAYER_STATE1_23;
        this->actor.parent = NULL;
        AREG(6) = 0;

        if (Flags_GetEventChkInf(0x18) || (DREG(1) != 0)) {
            gSaveContext.horseData.pos.x = rideActor->actor.world.pos.x;
            gSaveContext.horseData.pos.y = rideActor->actor.world.pos.y;
            gSaveContext.horseData.pos.z = rideActor->actor.world.pos.z;
            gSaveContext.horseData.angle = rideActor->actor.shape.rot.y;
        }
    }
    else {
        Camera_ChangeSetting(Gameplay_GetCamera(globalCtx, 0), CAM_SET_NORMAL0);

        if (this->mountSide < 0) {
            D_808549C4[0].field = 0x2828;
        }
        else {
            D_808549C4[0].field = 0x281D;
        }
        func_80832924(this, D_808549C4);
    }
}

static struct_80832924 D_808549D0[] = {
    { NA_SE_PL_SWIM, -0x800 },
};

void func_8084D530(Player* this, f32* arg1, f32 arg2, s16 arg3) {
    func_8084AEEC(this, arg1, arg2, arg3);
    func_80832924(this, D_808549D0);
}

void func_8084D574(GlobalContext* globalCtx, Player* this, s16 arg2) {
    func_80835C58(globalCtx, this, func_8084D84C, 0);
    this->actor.shape.rot.y = this->currentYaw = arg2;
    func_80832C6C(globalCtx, this, &gPlayerAnim_0032F0);
}

void func_8084D5CC(GlobalContext* globalCtx, Player* this) {
    func_80835C58(globalCtx, this, func_8084DAB4, 0);
    func_80832C6C(globalCtx, this, &gPlayerAnim_0032F0);
}

void func_8084D610(Player* this, GlobalContext* globalCtx) {
    f32 sp34;
    s16 sp32;

    func_80832CB0(globalCtx, this, &gPlayerAnim_003328);
    func_8084B000(this);

    if (!func_8083224C(globalCtx) && !func_80837348(globalCtx, this, D_80854444, 1) &&
        !func_8083D12C(globalCtx, this, sControlInput)) {
        if (this->unk_6AD != 1) {
            this->unk_6AD = 0;
        }

        if (this->currentBoots == PLAYER_BOOTS_IRON) {
            sp34 = 0.0f;
            sp32 = this->actor.shape.rot.y;

            if (this->actor.bgCheckFlags & 1) {
                func_8083A098(this, D_80853914[PLAYER_ANIMGROUP_15][this->modelAnimType], globalCtx);
                func_808328A0(this);
            }
        }
        else {
            func_80837268(this, &sp34, &sp32, 0.0f, globalCtx);

            if (sp34 != 0.0f) {
                s16 temp = this->actor.shape.rot.y - sp32;

                if ((ABS(temp) > 0x6000) && !Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
                    return;
                }

                if (func_80833C04(this)) {
                    func_8084D5CC(globalCtx, this);
                }
                else {
                    func_8084D574(globalCtx, this, sp32);
                }
            }
        }

        func_8084AEEC(this, &this->linearVelocity, sp34, sp32);
    }
}

void func_8084D7C4(Player* this, GlobalContext* globalCtx) {
    if (!func_8083B040(this, globalCtx)) {
        this->stateFlags2 |= PLAYER_STATE2_5;

        func_8084B158(globalCtx, this, NULL, this->linearVelocity);
        func_8084B000(this);

        if (DECR(this->unk_850) == 0) {
            func_80838F18(globalCtx, this);
        }
    }
}

void func_8084D84C(Player* this, GlobalContext* globalCtx) {
    f32 sp34;
    s16 sp32;
    s16 temp;

    this->stateFlags2 |= PLAYER_STATE2_5;

    func_8084B158(globalCtx, this, sControlInput, this->linearVelocity);
    func_8084B000(this);

    if (!func_80837348(globalCtx, this, D_80854444, 1) && !func_8083D12C(globalCtx, this, sControlInput)) {
        func_80837268(this, &sp34, &sp32, 0.0f, globalCtx);

        temp = this->actor.shape.rot.y - sp32;
        if ((sp34 == 0.0f) || (ABS(temp) > 0x6000) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
            func_80838F18(globalCtx, this);
        }
        else if (func_80833C04(this)) {
            func_8084D5CC(globalCtx, this);
        }

        func_8084D530(this, &this->linearVelocity, sp34, sp32);
    }
}

s32 func_8084D980(GlobalContext* globalCtx, Player* this, f32* arg2, s16* arg3) {
    LinkAnimationHeader* anim;
    s16 temp1;
    s32 temp2;

    temp1 = this->currentYaw - *arg3;

    if (ABS(temp1) > 0x6000) {
        anim = &gPlayerAnim_003328;

        if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
            this->currentYaw = *arg3;
        }
        else {
            *arg2 = 0.0f;
            *arg3 = this->currentYaw;
        }
    }
    else {
        temp2 = func_8083FD78(this, arg2, arg3, globalCtx);

        if (temp2 > 0) {
            anim = &gPlayerAnim_0032F0;
        }
        else if (temp2 < 0) {
            anim = &gPlayerAnim_0032D8;
        }
        else if ((temp1 = this->actor.shape.rot.y - *arg3) > 0) {
            anim = &gPlayerAnim_0032D0;
        }
        else {
            anim = &gPlayerAnim_0032C8;
        }
    }

    if (anim != this->skelAnime.animation) {
        func_80832C6C(globalCtx, this, anim);
        return 1;
    }

    return 0;
}

void func_8084DAB4(Player* this, GlobalContext* globalCtx) {
    f32 sp2C;
    s16 sp2A;

    func_8084B158(globalCtx, this, sControlInput, this->linearVelocity);
    func_8084B000(this);

    if (!func_80837348(globalCtx, this, D_80854444, 1) && !func_8083D12C(globalCtx, this, sControlInput)) {
        func_80837268(this, &sp2C, &sp2A, 0.0f, globalCtx);

        if (sp2C == 0.0f) {
            func_80838F18(globalCtx, this);
        }
        else if (!func_80833C04(this)) {
            func_8084D574(globalCtx, this, sp2A);
        }
        else {
            func_8084D980(globalCtx, this, &sp2C, &sp2A);
        }

        func_8084D530(this, &this->linearVelocity, sp2C, sp2A);
    }
}

void func_8084DBC4(GlobalContext* globalCtx, Player* this, f32 arg2) {
    f32 sp2C;
    s16 sp2A;

    func_80837268(this, &sp2C, &sp2A, 0.0f, globalCtx);
    func_8084AEEC(this, &this->linearVelocity, sp2C * 0.5f, sp2A);
    func_8084AEEC(this, &this->actor.velocity.y, arg2, this->currentYaw);
}

void func_8084DC48(Player* this, GlobalContext* globalCtx) {
    f32 sp2C;

    this->stateFlags2 |= PLAYER_STATE2_5;
    this->actor.gravity = 0.0f;
    func_80836670(this, globalCtx);

    if (!func_8083B040(this, globalCtx)) {
        if (this->currentBoots == PLAYER_BOOTS_IRON) {
            func_80838F18(globalCtx, this);
            return;
        }

        if (this->unk_84F == 0) {
            if (this->unk_850 == 0) {
                if (LinkAnimation_Update(globalCtx, &this->skelAnime) ||
                    ((this->skelAnime.curFrame >= 22.0f) && !CHECK_BTN_ALL(sControlInput->cur.button, BTN_A))) {
                    func_8083D330(globalCtx, this);
                }
                else if (LinkAnimation_OnFrame(&this->skelAnime, 20.0f) != 0) {
                    this->actor.velocity.y = -2.0f;
                }

                func_8083721C(this);
                return;
            }

            func_8084B158(globalCtx, this, sControlInput, this->actor.velocity.y);
            this->unk_6C2 = 16000;

            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A) && !func_8083E5A8(this, globalCtx) &&
                !(this->actor.bgCheckFlags & 1) && (this->actor.yDistToWater < D_80854784[CUR_UPG_VALUE(UPG_SCALE)])) {
                func_8084DBC4(globalCtx, this, -2.0f);
            }
            else {
                this->unk_84F++;
                func_80832C6C(globalCtx, this, &gPlayerAnim_003328);
            }
        }
        else if (this->unk_84F == 1) {
            LinkAnimation_Update(globalCtx, &this->skelAnime);
            func_8084B000(this);

            if (this->unk_6C2 < 10000) {
                this->unk_84F++;
                this->unk_850 = this->actor.yDistToWater;
                func_80832C6C(globalCtx, this, &gPlayerAnim_0032F0);
            }
        }
        else if (!func_8083D12C(globalCtx, this, sControlInput)) {
            sp2C = (this->unk_850 * 0.018f) + 4.0f;

            if (this->stateFlags1 & PLAYER_STATE1_11) {
                sControlInput = NULL;
            }

            func_8084B158(globalCtx, this, sControlInput, fabsf(this->actor.velocity.y));
            Math_ScaledStepToS(&this->unk_6C2, -10000, 800);

            if (sp2C > 8.0f) {
                sp2C = 8.0f;
            }

            func_8084DBC4(globalCtx, this, sp2C);
        }
    }
}

void func_8084DF6C(GlobalContext* globalCtx, Player* this) {
    this->unk_862 = 0;
    this->stateFlags1 &= ~(PLAYER_STATE1_10 | PLAYER_STATE1_11);
    this->getItemId = GI_NONE;
    func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
}

void func_8084DFAC(GlobalContext* globalCtx, Player* this) {
    func_8084DF6C(globalCtx, this);
    func_808322FC(this);
    func_8083C0E8(this, globalCtx);
    this->currentYaw = this->actor.shape.rot.y;
}

s32 func_8084DFF4(GlobalContext* globalCtx, Player* this) {
    GetItemEntry* giEntry;
    s32 temp1;
    s32 temp2;

    if (this->getItemId == GI_NONE) {
        return 1;
    }

    if (this->unk_84F == 0) {
        giEntry = &sGetItemTable[this->getItemId - 1];
        this->unk_84F = 1;

        Message_StartTextbox(globalCtx, giEntry->textId, &this->actor);
        Item_Give(globalCtx, giEntry->itemId);

        if (((this->getItemId >= GI_RUPEE_GREEN) && (this->getItemId <= GI_RUPEE_RED)) ||
            ((this->getItemId >= GI_RUPEE_PURPLE) && (this->getItemId <= GI_RUPEE_GOLD)) ||
            ((this->getItemId >= GI_RUPEE_GREEN_LOSE) && (this->getItemId <= GI_RUPEE_PURPLE_LOSE)) ||
            (this->getItemId == GI_HEART)) {
            Audio_PlaySoundGeneral(NA_SE_SY_GET_BOXITEM, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
        else {
            if ((this->getItemId == GI_HEART_CONTAINER_2) || (this->getItemId == GI_HEART_CONTAINER) ||
                ((this->getItemId == GI_HEART_PIECE) &&
                    ((gSaveContext.inventory.questItems & 0xF0000000) == 0x40000000))) {
                temp1 = NA_BGM_HEART_GET | 0x900;
            }
            else {
                temp1 = temp2 = (this->getItemId == GI_HEART_PIECE) ? NA_BGM_SMALL_ITEM_GET : NA_BGM_ITEM_GET | 0x900;
            }
            Audio_PlayFanfare(temp1);
        }
    }
    else {
        if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
            if (this->getItemId == GI_GAUNTLETS_SILVER) {
                globalCtx->nextEntranceIndex = 0x0123;
                globalCtx->sceneLoadFlag = 0x14;
                gSaveContext.nextCutsceneIndex = 0xFFF1;
                globalCtx->fadeTransition = 0xF;
                this->stateFlags1 &= ~PLAYER_STATE1_29;
                func_80852FFC(globalCtx, NULL, 8);
            }
            this->getItemId = GI_NONE;
        }
    }

    return 0;
}

void func_8084E1EC(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_5;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_10) || func_8084DFF4(globalCtx, this)) {
            func_8084DF6C(globalCtx, this);
            func_80838F18(globalCtx, this);
            func_80832340(globalCtx, this);
        }
    }
    else {
        if ((this->stateFlags1 & PLAYER_STATE1_10) && LinkAnimation_OnFrame(&this->skelAnime, 10.0f)) {
            func_808332F4(this, globalCtx);
            func_80832340(globalCtx, this);
            func_80835EA4(globalCtx, 8);
        }
        else if (LinkAnimation_OnFrame(&this->skelAnime, 5.0f)) {
            func_80832698(this, NA_SE_VO_LI_BREATH_DRINK);
        }
    }

    func_8084B000(this);
    func_8084AEEC(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

void func_8084E30C(Player* this, GlobalContext* globalCtx) {
    func_8084B000(this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80838F18(globalCtx, this);
    }

    func_8084AEEC(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

void func_8084E368(Player* this, GlobalContext* globalCtx) {
    func_8084B000(this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80843AE8(globalCtx, this);
    }

    func_8084AEEC(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

static s16 D_808549D4[] = { 0x0600, 0x04F6, 0x0604, 0x01F1, 0x0568, 0x05F4 };

void func_8084E3C4(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_808322A4(globalCtx, this, &gPlayerAnim_0030A8);
        this->unk_850 = 1;
        if (this->stateFlags2 & (PLAYER_STATE2_23 | PLAYER_STATE2_25)) {
            this->stateFlags2 |= PLAYER_STATE2_24;
        }
        else {
            func_8010BD58(globalCtx, OCARINA_ACTION_FREE_PLAY);
        }
        return;
    }

    if (this->unk_850 == 0) {
        return;
    }

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));

        if ((this->targetActor != NULL) && (this->targetActor == this->unk_6A8)) {
            func_80853148(globalCtx, this->targetActor);
        }
        else if (this->naviTextId < 0) {
            this->targetActor = this->naviActor;
            this->naviActor->textId = -this->naviTextId;
            func_80853148(globalCtx, this->targetActor);
        }
        else if (!func_8083B040(this, globalCtx)) {
            func_8083A098(this, &gPlayerAnim_003098, globalCtx);
        }

        this->stateFlags2 &= ~(PLAYER_STATE2_23 | PLAYER_STATE2_24 | PLAYER_STATE2_25);
        this->unk_6A8 = NULL;
    }
    else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_02) {
        gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex = D_808549D4[globalCtx->msgCtx.lastPlayedSong];
        gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams = 0x5FF;
        gSaveContext.respawn[RESPAWN_MODE_RETURN].data = globalCtx->msgCtx.lastPlayedSong;

        this->csMode = 0;
        this->stateFlags1 &= ~PLAYER_STATE1_29;

        func_80852FFC(globalCtx, NULL, 8);
        globalCtx->mainCamera.unk_14C &= ~8;

        this->stateFlags1 |= PLAYER_STATE1_28 | PLAYER_STATE1_29;
        this->stateFlags2 |= PLAYER_STATE2_27;

        if (Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0xF) == NULL) {
            Environment_WarpSongLeave(globalCtx);
        }

        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
    }
}

void func_8084E604(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_8083A098(this, &gPlayerAnim_003050, globalCtx);
    }
    else if (LinkAnimation_OnFrame(&this->skelAnime, 3.0f)) {
        Inventory_ChangeAmmo(ITEM_NUT, -1);
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ARROW, this->bodyPartsPos[PLAYER_BODYPART_R_HAND].x,
            this->bodyPartsPos[PLAYER_BODYPART_R_HAND].y, this->bodyPartsPos[PLAYER_BODYPART_R_HAND].z, 4000,
            this->actor.shape.rot.y, 0, ARROW_NUT);
        func_80832698(this, NA_SE_VO_LI_SWORD_N);
    }

    func_8083721C(this);
}

static struct_80832924 D_808549E0[] = {
    { 0, 0x3857 },
    { NA_SE_VO_LI_CLIMB_END, 0x2057 },
    { NA_SE_VO_LI_AUTO_JUMP, 0x2045 },
    { 0, -0x287B },
};

void func_8084E6D4(Player* this, GlobalContext* globalCtx) {
    s32 cond;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (this->unk_850 != 0) {
            if (this->unk_850 >= 2) {
                this->unk_850--;
            }

            if (func_8084DFF4(globalCtx, this) && (this->unk_850 == 1)) {
                cond = ((this->targetActor != NULL) && (this->exchangeItemId < 0)) ||
                    (this->stateFlags3 & PLAYER_STATE3_5);

                if (cond || (gSaveContext.healthAccumulator == 0)) {
                    if (cond) {
                        func_8084DF6C(globalCtx, this);
                        this->exchangeItemId = EXCH_ITEM_NONE;

                        if (func_8084B4D4(globalCtx, this) == 0) {
                            func_80853148(globalCtx, this->targetActor);
                        }
                    }
                    else {
                        func_8084DFAC(globalCtx, this);
                    }
                }
            }
        }
        else {
            func_80832DBC(this);

            if (this->getItemId == GI_ICE_TRAP) {
                this->stateFlags1 &= ~(PLAYER_STATE1_10 | PLAYER_STATE1_11);

                if (this->getItemId != GI_ICE_TRAP) {
                    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x,
                        this->actor.world.pos.y + 100.0f, this->actor.world.pos.z, 0, 0, 0, 0);
                    func_8083C0E8(this, globalCtx);
                }
                else {
                    this->actor.colChkInfo.damage = 0;
                    func_80837C0C(globalCtx, this, 3, 0.0f, 0.0f, 0, 20);
                }
                return;
            }

            if (this->skelAnime.animation == &gPlayerAnim_002DF8) {
                func_808322D0(globalCtx, this, &gPlayerAnim_002788);
            }
            else {
                func_808322D0(globalCtx, this, &gPlayerAnim_002780);
            }

            this->unk_850 = 2;
            func_80835EA4(globalCtx, 9);
        }
    }
    else {
        if (this->unk_850 == 0) {
            if (!LINK_IS_ADULT) {
                func_80832924(this, D_808549E0);
            }
            return;
        }

        if (this->skelAnime.animation == &gPlayerAnim_002788) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000, 4000);
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, 21.0f)) {
            func_808332F4(this, globalCtx);
        }
    }
}

static struct_80832924 D_808549F0[] = {
    { NA_SE_IT_MASTER_SWORD_SWING, -0x83C },
};

void func_8084E988(Player* this) {
    func_80832924(this, D_808549F0);
}

static struct_80832924 D_808549F4[] = {
    { NA_SE_VO_LI_AUTO_JUMP, 0x2005 },
    { 0, -0x280F },
};

void func_8084E9AC(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (this->unk_84F == 0) {
            if (DECR(this->unk_850) == 0) {
                this->unk_84F = 1;
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        }
        else {
            func_8083C0E8(this, globalCtx);
        }
    }
    else {
        if (LINK_IS_ADULT && LinkAnimation_OnFrame(&this->skelAnime, 158.0f)) {
            func_80832698(this, NA_SE_VO_LI_SWORD_N);
            return;
        }

        if (!LINK_IS_ADULT) {
            func_80832924(this, D_808549F4);
        }
        else {
            func_8084E988(this);
        }
    }
}

static u8 D_808549FC[] = {
    0x01, 0x03, 0x02, 0x04, 0x04,
};

void func_8084EAC0(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (this->unk_850 == 0) {
            if (this->itemActionParam == PLAYER_AP_BOTTLE_POE) {
                s32 rand = Rand_S16Offset(-1, 3);

                if (rand == 0) {
                    rand = 3;
                }

                if ((rand < 0) && (gSaveContext.health <= 0x10)) {
                    rand = 3;
                }

                if (rand < 0) {
                    Health_ChangeBy(globalCtx, -0x10);
                }
                else {
                    gSaveContext.healthAccumulator = rand * 0x10;
                }
            }
            else {
                s32 sp28 = D_808549FC[this->itemActionParam - PLAYER_AP_BOTTLE_POTION_RED];

                if (sp28 & 1) {
                    gSaveContext.healthAccumulator = 0x140;
                }

                if (sp28 & 2) {
                    Magic_Fill(globalCtx);
                }

                if (sp28 & 4) {
                    gSaveContext.healthAccumulator = 0x50;
                }
            }

            func_808322A4(globalCtx, this, &gPlayerAnim_002670);
            this->unk_850 = 1;
            return;
        }

        func_8083C0E8(this, globalCtx);
        func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
    }
    else if (this->unk_850 == 1) {
        if ((gSaveContext.healthAccumulator == 0) && (gSaveContext.unk_13F0 != 9)) {
            func_80832B78(globalCtx, this, &gPlayerAnim_002660);
            this->unk_850 = 2;
            Player_UpdateBottleHeld(globalCtx, this, ITEM_BOTTLE, PLAYER_AP_BOTTLE);
        }
        func_80832698(this, NA_SE_VO_LI_DRINK - SFX_FLAG);
    }
    else if ((this->unk_850 == 2) && LinkAnimation_OnFrame(&this->skelAnime, 29.0f)) {
        func_80832698(this, NA_SE_VO_LI_BREATH_DRINK);
    }
}

static BottleCatchInfo D_80854A04[] = {
    { ACTOR_EN_ELF, ITEM_FAIRY, 0x2A, 0x46 },
    { ACTOR_EN_FISH, ITEM_FISH, 0x1F, 0x47 },
    { ACTOR_EN_ICE_HONO, ITEM_BLUE_FIRE, 0x20, 0x5D },
    { ACTOR_EN_INSECT, ITEM_BUG, 0x21, 0x7A },
};

void func_8084ECA4(Player* this, GlobalContext* globalCtx) {
    struct_80854554* sp24;
    BottleCatchInfo* catchInfo;
    s32 temp;
    s32 i;

    sp24 = &D_80854554[this->unk_850];
    func_8083721C(this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (this->unk_84F != 0) {
            if (this->unk_850 == 0) {
                Message_StartTextbox(globalCtx, D_80854A04[this->unk_84F - 1].textId, &this->actor);
                Audio_PlayFanfare(NA_BGM_ITEM_GET | 0x900);
                this->unk_850 = 1;
            }
            else if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
                this->unk_84F = 0;
                func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
            }
        }
        else {
            func_8083C0E8(this, globalCtx);
        }
    }
    else {
        if (this->unk_84F == 0) {
            temp = this->skelAnime.curFrame - sp24->unk_08;

            if (temp >= 0) {
                if (sp24->unk_09 >= temp) {
                    if (this->unk_850 != 0) {
                        if (temp == 0) {
                            func_8002F7DC(&this->actor, NA_SE_IT_SCOOP_UP_WATER);
                        }
                    }

                    if (this->interactRangeActor != NULL) {
                        catchInfo = &D_80854A04[0];
                        for (i = 0; i < 4; i++, catchInfo++) {
                            if (this->interactRangeActor->id == catchInfo->actorId) {
                                break;
                            }
                        }

                        if (i < 4) {
                            this->unk_84F = i + 1;
                            this->unk_850 = 0;
                            this->stateFlags1 |= PLAYER_STATE1_28 | PLAYER_STATE1_29;
                            this->interactRangeActor->parent = &this->actor;
                            Player_UpdateBottleHeld(globalCtx, this, catchInfo->itemId, ABS(catchInfo->actionParam));
                            func_808322D0(globalCtx, this, sp24->unk_04);
                            func_80835EA4(globalCtx, 4);
                        }
                    }
                }
            }
        }
    }

    if (this->skelAnime.curFrame <= 7.0f) {
        this->stateFlags1 |= PLAYER_STATE1_1;
    }
}

static Vec3f D_80854A1C = { 0.0f, 0.0f, 5.0f };

void func_8084EED8(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_8083C0E8(this, globalCtx);
        func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 37.0f)) {
        Player_SpawnFairy(globalCtx, this, &this->leftHandPos, &D_80854A1C, FAIRY_REVIVE_BOTTLE);
        Player_UpdateBottleHeld(globalCtx, this, ITEM_BOTTLE, PLAYER_AP_BOTTLE);
        func_8002F7DC(&this->actor, NA_SE_EV_BOTTLE_CAP_OPEN);
        func_8002F7DC(&this->actor, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
    }
    else if (LinkAnimation_OnFrame(&this->skelAnime, 47.0f)) {
        gSaveContext.healthAccumulator = 0x140;
    }
}

static BottleDropInfo D_80854A28[] = {
    { ACTOR_EN_FISH, FISH_DROPPED },
    { ACTOR_EN_ICE_HONO, 0 },
    { ACTOR_EN_INSECT, 2 },
};

static struct_80832924 D_80854A34[] = {
    { NA_SE_VO_LI_AUTO_JUMP, 0x2026 },
    { NA_SE_EV_BOTTLE_CAP_OPEN, -0x828 },
};

void func_8084EFC0(Player* this, GlobalContext* globalCtx) {
    func_8083721C(this);

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_8083C0E8(this, globalCtx);
        func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 76.0f)) {
        BottleDropInfo* dropInfo = &D_80854A28[this->itemActionParam - PLAYER_AP_BOTTLE_FISH];

        Actor_Spawn(&globalCtx->actorCtx, globalCtx, dropInfo->actorId,
            (Math_SinS(this->actor.shape.rot.y) * 5.0f) + this->leftHandPos.x, this->leftHandPos.y,
            (Math_CosS(this->actor.shape.rot.y) * 5.0f) + this->leftHandPos.z, 0x4000, this->actor.shape.rot.y,
            0, dropInfo->actorParams);

        Player_UpdateBottleHeld(globalCtx, this, ITEM_BOTTLE, PLAYER_AP_BOTTLE);
        return;
    }

    func_80832924(this, D_80854A34);
}

static struct_80832924 D_80854A3C[] = {
    { NA_SE_PL_PUT_OUT_ITEM, -0x81E },
};

void func_8084F104(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_5;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (this->unk_850 < 0) {
            func_8083C0E8(this, globalCtx);
        }
        else if (this->exchangeItemId == EXCH_ITEM_NONE) {
            Actor* targetActor = this->targetActor;

            this->unk_862 = 0;
            if (targetActor->textId != 0xFFFF) {
                this->actor.flags |= ACTOR_FLAG_8;
            }

            func_80853148(globalCtx, targetActor);
        }
        else {
            GetItemEntry* giEntry = &sGetItemTable[D_80854528[this->exchangeItemId - 1] - 1];

            if (this->itemActionParam >= PLAYER_AP_LETTER_ZELDA) {
                if (giEntry->gi >= 0) {
                    this->unk_862 = giEntry->gi;
                }
                else {
                    this->unk_862 = -giEntry->gi;
                }
            }

            if (this->unk_850 == 0) {
                Message_StartTextbox(globalCtx, this->actor.textId, &this->actor);

                if ((this->itemActionParam == PLAYER_AP_CHICKEN) || (this->itemActionParam == PLAYER_AP_POCKET_CUCCO)) {
                    func_8002F7DC(&this->actor, NA_SE_EV_CHICKEN_CRY_M);
                }

                this->unk_850 = 1;
            }
            else if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
                this->actor.flags &= ~ACTOR_FLAG_8;
                this->unk_862 = 0;

                if (this->unk_84F == 1) {
                    func_80832264(globalCtx, this, &gPlayerAnim_002698);
                    this->unk_850 = -1;
                }
                else {
                    func_8083C0E8(this, globalCtx);
                }

                func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
            }
        }
    }
    else if (this->unk_850 >= 0) {
        func_80832924(this, D_80854A3C);
    }

    if ((this->unk_84F == 0) && (this->unk_664 != NULL)) {
        this->currentYaw = this->actor.shape.rot.y = func_8083DB98(this, 0);
    }
}

void func_8084F308(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80832284(globalCtx, this, &gPlayerAnim_003128);
    }

    if (func_80832594(this, 0, 100)) {
        func_80839F90(this, globalCtx);
        this->stateFlags2 &= ~PLAYER_STATE2_7;
    }
}

void func_8084F390(Player* this, GlobalContext* globalCtx) {
    CollisionPoly* floorPoly;
    f32 sp50;
    f32 sp4C;
    f32 sp48;
    s16 sp46;
    s16 sp44;
    Vec3f sp38;

    this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;
    LinkAnimation_Update(globalCtx, &this->skelAnime);
    func_8084269C(globalCtx, this);
    func_800F4138(&this->actor.projectedPos, NA_SE_PL_SLIP_LEVEL - SFX_FLAG, this->actor.speedXZ);

    if (func_8083B040(this, globalCtx) == 0) {
        floorPoly = this->actor.floorPoly;

        if (floorPoly == NULL) {
            func_80837B9C(this, globalCtx);
            return;
        }

        func_8083E298(floorPoly, &sp38, &sp46);

        sp44 = sp46;
        if (this->unk_84F != 0) {
            sp44 = sp46 + 0x8000;
        }

        if (this->linearVelocity < 0) {
            sp46 += 0x8000;
        }

        sp50 = (1.0f - sp38.y) * 40.0f;
        sp50 = CLAMP(sp50, 0, 10.0f);
        sp4C = (sp50 * sp50) * 0.015f;
        sp48 = sp38.y * 0.01f;

        if (SurfaceType_GetSlope(&globalCtx->colCtx, floorPoly, this->actor.floorBgId) != 1) {
            sp50 = 0;
            sp48 = sp38.y * 10.0f;
        }

        if (sp4C < 1.0f) {
            sp4C = 1.0f;
        }

        if (Math_AsymStepToF(&this->linearVelocity, sp50, sp4C, sp48) && (sp50 == 0)) {
            LinkAnimationHeader* anim;
            if (this->unk_84F == 0) {
                anim = D_80853914[PLAYER_ANIMGROUP_42][this->modelAnimType];
            }
            else {
                anim = D_80853914[PLAYER_ANIMGROUP_43][this->modelAnimType];
            }
            func_8083A098(this, anim, globalCtx);
        }

        Math_SmoothStepToS(&this->currentYaw, sp46, 10, 4000, 800);
        Math_ScaledStepToS(&this->actor.shape.rot.y, sp44, 2000);
    }
}

void func_8084F608(Player* this, GlobalContext* globalCtx) {
    if ((DECR(this->unk_850) == 0) && func_8083ADD4(globalCtx, this)) {
        func_80852280(globalCtx, this, NULL);
        func_80835C58(globalCtx, this, func_80852E14, 0);
        func_80852E14(this, globalCtx);
    }
}

void func_8084F698(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_8084F608, 0);
    this->unk_850 = 40;
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0x10);
}

void func_8084F710(Player* this, GlobalContext* globalCtx) {
    s32 pad;

    if ((this->unk_84F != 0) && (globalCtx->csCtx.frames < 0x131)) {
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
    }
    else if (D_80853600 < 150.0f) {
        if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
            if (this->unk_850 == 0) {
                if (this->actor.bgCheckFlags & 1) {
                    this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
                    func_808328A0(this);
                    this->unk_850 = 1;
                }
            }
            else {
                if ((globalCtx->sceneNum == SCENE_SPOT04) && func_8083ADD4(globalCtx, this)) {
                    return;
                }
                func_80853080(this, globalCtx);
            }
        }
        Math_SmoothStepToF(&this->actor.velocity.y, 2.0f, 0.3f, 8.0f, 0.5f);
    }

    if ((globalCtx->sceneNum == SCENE_KENJYANOMA) && func_8083ADD4(globalCtx, this)) {
        return;
    }

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.linkAction != NULL)) {
        f32 sp28 = this->actor.world.pos.y;
        func_808529D0(globalCtx, this, globalCtx->csCtx.linkAction);
        this->actor.world.pos.y = sp28;
    }
}

void func_8084F88C(Player* this, GlobalContext* globalCtx) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);

    if ((this->unk_850++ > 8) && (globalCtx->sceneLoadFlag == 0)) {

        if (this->unk_84F != 0) {
            if (globalCtx->sceneNum == 9) {
                Gameplay_TriggerRespawn(globalCtx);
                globalCtx->nextEntranceIndex = 0x0088;
            }
            else if (this->unk_84F < 0) {
                Gameplay_TriggerRespawn(globalCtx);
            }
            else {
                Gameplay_TriggerVoidOut(globalCtx);
            }

            globalCtx->fadeTransition = 4;
            func_80078884(NA_SE_OC_ABYSS);
        }
        else {
            globalCtx->fadeTransition = 2;
            gSaveContext.nextTransition = 2;
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = 0xFF;
        }

        globalCtx->sceneLoadFlag = 0x14;
    }
}

void func_8084F9A0(Player* this, GlobalContext* globalCtx) {
    func_80839800(this, globalCtx);
}

void func_8084F9C0(Player* this, GlobalContext* globalCtx) {
    this->actor.gravity = -1.0f;

    LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (this->actor.velocity.y < 0.0f) {
        func_80837B9C(this, globalCtx);
    }
    else if (this->actor.velocity.y < 6.0f) {
        Math_StepToF(&this->linearVelocity, 3.0f, 0.5f);
    }
}

void func_8084FA54(Player* this, GlobalContext* globalCtx) {
    this->unk_6AD = 2;

    func_8083AD4C(globalCtx, this);
    LinkAnimation_Update(globalCtx, &this->skelAnime);
    func_80836670(this, globalCtx);

    this->unk_6BE = func_8084ABD8(globalCtx, this, 1, 0) - this->actor.shape.rot.y;
    this->unk_6AE |= 0x80;

    if (globalCtx->shootingGalleryStatus < 0) {
        globalCtx->shootingGalleryStatus++;
        if (globalCtx->shootingGalleryStatus == 0) {
            func_8083C148(this, globalCtx);
        }
    }
}

void func_8084FB10(Player* this, GlobalContext* globalCtx) {
    if (this->unk_84F >= 0) {
        if (this->unk_84F < 6) {
            this->unk_84F++;
        }

        if (func_80832594(this, 1, 100)) {
            this->unk_84F = -1;
            EffectSsIcePiece_SpawnBurst(globalCtx, &this->actor.world.pos, this->actor.scale.x);
            func_8002F7DC(&this->actor, NA_SE_PL_ICE_BROKEN);
        }
        else {
            this->stateFlags2 |= PLAYER_STATE2_14;
        }

        if ((globalCtx->gameplayFrames % 4) == 0) {
            Player_InflictDamage(globalCtx, -1);
        }
    }
    else {
        if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
            func_80839F90(this, globalCtx);
            func_80837AFC(this, -20);
        }
    }
}

void func_8084FBF4(Player* this, GlobalContext* globalCtx) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);
    func_808382BC(this);

    if (((this->unk_850 % 25) != 0) || func_80837B18(globalCtx, this, -1)) {
        if (DECR(this->unk_850) == 0) {
            func_80839F90(this, globalCtx);
        }
    }

    this->shockTimer = 40;
    func_8002F8F0(&this->actor, NA_SE_VO_LI_TAKEN_AWAY - SFX_FLAG + this->ageProperties->unk_92);
}

s32 func_8084FCAC(Player* this, GlobalContext* globalCtx) {
    sControlInput = &globalCtx->state.input[0];

    if (CVar_GetS32("gDebugEnabled", 0) && ((CHECK_BTN_ALL(sControlInput->cur.button, BTN_A | BTN_L | BTN_R) &&
        CHECK_BTN_ALL(sControlInput->press.button, BTN_B)) ||
        (CHECK_BTN_ALL(sControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sControlInput->press.button, BTN_DRIGHT)))) {

        D_808535D0 ^= 1;

        if (D_808535D0) {
            Camera_ChangeMode(Gameplay_GetCamera(globalCtx, 0), CAM_MODE_BOWARROWZ);
        }
    }

    if (D_808535D0) {
        f32 speed;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {
            speed = 100.0f;
        }
        else {
            speed = 20.0f;
        }

        func_8006375C(3, 2, "DEBUG MODE");

        if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_L)) {
            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
                this->actor.world.pos.y += speed;
            }
            else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A)) {
                this->actor.world.pos.y -= speed;
            }

            if (CHECK_BTN_ANY(sControlInput->cur.button, BTN_DUP | BTN_DLEFT | BTN_DDOWN | BTN_DRIGHT)) {
                s16 angle;
                s16 temp;

                angle = temp = Camera_GetInputDirYaw(GET_ACTIVE_CAM(globalCtx));

                if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DDOWN)) {
                    angle = temp + 0x8000;
                }
                else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DLEFT)) {
                    angle = temp + 0x4000;
                }
                else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DRIGHT)) {
                    angle = temp - 0x4000;
                }

                this->actor.world.pos.x += speed * Math_SinS(angle);
                this->actor.world.pos.z += speed * Math_CosS(angle);
            }
        }

        func_80832210(this);

        this->actor.gravity = 0.0f;
        this->actor.velocity.z = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.x = 0.0f;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sControlInput->press.button, BTN_DLEFT)) {
            Flags_SetTempClear(globalCtx, globalCtx->roomCtx.curRoom.num);
        }

        Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);

        return 0;
    }

    return 1;
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

void func_8085002C(Player* this) {
    s32 pad;
    s16 sp2A;
    s16 sp28;
    s16 sp26;

    D_80858AC8.unk_06 -= D_80858AC8.unk_06 >> 3;
    D_80858AC8.unk_08 -= D_80858AC8.unk_08 >> 3;
    D_80858AC8.unk_06 += -D_80858AC8.unk_00 >> 2;
    D_80858AC8.unk_08 += -D_80858AC8.unk_02 >> 2;

    sp26 = this->actor.world.rot.y - this->actor.shape.rot.y;

    sp28 = (s32)(this->actor.speedXZ * -200.0f * Math_CosS(sp26) * (Rand_CenteredFloat(2.0f) + 10.0f)) & 0xFFFF;
    sp2A = (s32)(this->actor.speedXZ * 100.0f * Math_SinS(sp26) * (Rand_CenteredFloat(2.0f) + 10.0f)) & 0xFFFF;

    D_80858AC8.unk_06 += sp28 >> 2;
    D_80858AC8.unk_08 += sp2A >> 2;

    if (D_80858AC8.unk_06 > 6000) {
        D_80858AC8.unk_06 = 6000;
    }
    else if (D_80858AC8.unk_06 < -6000) {
        D_80858AC8.unk_06 = -6000;
    }

    if (D_80858AC8.unk_08 > 6000) {
        D_80858AC8.unk_08 = 6000;
    }
    else if (D_80858AC8.unk_08 < -6000) {
        D_80858AC8.unk_08 = -6000;
    }

    D_80858AC8.unk_00 += D_80858AC8.unk_06;
    D_80858AC8.unk_02 += D_80858AC8.unk_08;

    if (D_80858AC8.unk_00 < 0) {
        D_80858AC8.unk_04 = D_80858AC8.unk_00 >> 1;
    }
    else {
        D_80858AC8.unk_04 = 0;
    }
}

s32 func_80850224(Player* this, GlobalContext* globalCtx) {
    if (func_8083C6B8(globalCtx, this) == 0) {
        if (func_8083BB20(this) != 0) {
            s32 sp24 = func_80837818(this);

            func_80837948(globalCtx, this, sp24);

            if (sp24 >= 0x18) {
                this->stateFlags2 |= PLAYER_STATE2_17;
                func_80837530(globalCtx, this, 0);
                return 1;
            }
        }
        else {
            return 0;
        }
    }

    return 1;
}

static Vec3f D_80854A40 = { 0.0f, 40.0f, 45.0f };

void func_808502D0(Player* this, GlobalContext* globalCtx) {
    struct_80854190* sp44 = &D_80854190[this->swordAnimation];

    this->stateFlags2 |= PLAYER_STATE2_5;

    if (!func_80842DF4(globalCtx, this)) {
        func_8084285C(this, 0.0f, sp44->unk_0C, sp44->unk_0D);

        if ((this->stateFlags2 & PLAYER_STATE2_30) && (this->heldItemActionParam != PLAYER_AP_HAMMER) &&
            LinkAnimation_OnFrame(&this->skelAnime, 0.0f)) {
            this->linearVelocity = 15.0f;
            this->stateFlags2 &= ~PLAYER_STATE2_30;
        }

        if (this->linearVelocity > 12.0f) {
            func_8084269C(globalCtx, this);
        }

        Math_StepToF(&this->linearVelocity, 0.0f, 5.0f);
        func_8083C50C(this);

        if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
            if (!func_80850224(this, globalCtx)) {
                u8 sp43 = this->skelAnime.moveFlags;
                LinkAnimationHeader* sp3C;

                if (func_8008E9C4(this)) {
                    sp3C = sp44->unk_08;
                }
                else {
                    sp3C = sp44->unk_04;
                }

                func_80832318(this);
                this->skelAnime.moveFlags = 0;

                if ((sp3C == &gPlayerAnim_002908) && (this->modelAnimType != PLAYER_ANIMTYPE_3)) {
                    sp3C = &gPlayerAnim_002AC8;
                }

                func_8083A098(this, sp3C, globalCtx);

                this->skelAnime.moveFlags = sp43;
                this->stateFlags3 |= PLAYER_STATE3_3;
            }
        }
        else if (this->heldItemActionParam == PLAYER_AP_HAMMER) {
            if ((this->swordAnimation == 0x16) || (this->swordAnimation == 0x13)) {
                static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
                Vec3f shockwavePos;
                f32 sp2C;

                shockwavePos.y = func_8083973C(globalCtx, this, &D_80854A40, &shockwavePos);
                sp2C = this->actor.world.pos.y - shockwavePos.y;

                Math_ScaledStepToS(&this->actor.focus.rot.x, Math_Atan2S(45.0f, sp2C), 800);
                func_80836AB8(this, 1);

                if ((((this->swordAnimation == 0x16) && LinkAnimation_OnFrame(&this->skelAnime, 7.0f)) ||
                    ((this->swordAnimation == 0x13) && LinkAnimation_OnFrame(&this->skelAnime, 2.0f))) &&
                    (sp2C > -40.0f) && (sp2C < 40.0f)) {
                    func_80842A28(globalCtx, this);
                    EffectSsBlast_SpawnWhiteShockwave(globalCtx, &shockwavePos, &zeroVec, &zeroVec);
                }
            }
        }
    }
}

void func_808505DC(Player* this, GlobalContext* globalCtx) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);
    func_8083721C(this);

    if (this->skelAnime.curFrame >= 6.0f) {
        func_80839FFC(this, globalCtx);
    }
}

void func_8085063C(Player* this, GlobalContext* globalCtx) {
    this->stateFlags2 |= PLAYER_STATE2_5;

    LinkAnimation_Update(globalCtx, &this->skelAnime);
    func_80836670(this, globalCtx);

    if (this->unk_850 == 0) {
        Message_StartTextbox(globalCtx, 0x3B, &this->actor);
        this->unk_850 = 1;
        return;
    }

    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        s32 respawnData = gSaveContext.respawn[RESPAWN_MODE_TOP].data;

        if (globalCtx->msgCtx.choiceIndex == 0) {
            gSaveContext.respawnFlag = 3;
            globalCtx->sceneLoadFlag = 0x14;
            globalCtx->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_TOP].entranceIndex;
            globalCtx->fadeTransition = 5;
            func_80088AF0(globalCtx);
            return;
        }

        if (globalCtx->msgCtx.choiceIndex == 1) {
            gSaveContext.respawn[RESPAWN_MODE_TOP].data = -respawnData;
            gSaveContext.fw.set = 0;
            func_80078914(&gSaveContext.respawn[RESPAWN_MODE_TOP].pos, NA_SE_PL_MAGIC_WIND_VANISH);
        }

        func_80853080(this, globalCtx);
        func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
    }
}

void func_8085076C(Player* this, GlobalContext* globalCtx) {
    s32 respawnData = gSaveContext.respawn[RESPAWN_MODE_TOP].data;

    if (this->unk_850 > 20) {
        this->actor.draw = Player_Draw;
        this->actor.world.pos.y += 60.0f;
        func_80837B9C(this, globalCtx);
        return;
    }

    if (this->unk_850++ == 20) {
        gSaveContext.respawn[RESPAWN_MODE_TOP].data = respawnData + 1;
        func_80078914(&gSaveContext.respawn[RESPAWN_MODE_TOP].pos, NA_SE_PL_MAGIC_WIND_WARP);
    }
}

static LinkAnimationHeader* D_80854A58[] = {
    &gPlayerAnim_002CF8,
    &gPlayerAnim_002CE0,
    &gPlayerAnim_002D10,
};

static LinkAnimationHeader* D_80854A64[] = {
    &gPlayerAnim_002D00,
    &gPlayerAnim_002CE8,
    &gPlayerAnim_002D18,
};

static LinkAnimationHeader* D_80854A70[] = {
    &gPlayerAnim_002D08,
    &gPlayerAnim_002CF0,
    &gPlayerAnim_002D20,
};

static u8 D_80854A7C[] = { 70, 10, 10 };

static struct_80832924 D_80854A80[] = {
    { NA_SE_PL_SKIP, 0x814 },
    { NA_SE_VO_LI_SWORD_N, 0x2014 },
    { 0, -0x301A },
};

static struct_80832924 D_80854A8C[][2] = {
    {
        { 0, 0x4014 },
        { NA_SE_VO_LI_MAGIC_FROL, -0x201E },
    },
    {
        { 0, 0x4014 },
        { NA_SE_VO_LI_MAGIC_NALE, -0x202C },
    },
    {
        { NA_SE_VO_LI_MAGIC_ATTACK, 0x2014 },
        { NA_SE_IT_SWORD_SWING_HARD, -0x814 },
    },
};

void func_808507F4(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (this->unk_84F < 0) {
            if ((this->itemActionParam == PLAYER_AP_NAYRUS_LOVE) || (gSaveContext.unk_13F0 == 0)) {
                func_80839FFC(this, globalCtx);
                func_8005B1A4(Gameplay_GetCamera(globalCtx, 0));
            }
        }
        else {
            if (this->unk_850 == 0) {
                LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, D_80854A58[this->unk_84F], 0.83f);

                if (func_80846A00(globalCtx, this, this->unk_84F) != NULL) {
                    this->stateFlags1 |= PLAYER_STATE1_28 | PLAYER_STATE1_29;
                    if ((this->unk_84F != 0) || (gSaveContext.respawn[RESPAWN_MODE_TOP].data <= 0)) {
                        gSaveContext.unk_13F0 = 1;
                    }
                }
                else {
                    func_800876C8(globalCtx);
                }
            }
            else {
                LinkAnimation_PlayLoopSetSpeed(globalCtx, &this->skelAnime, D_80854A64[this->unk_84F], 0.83f);

                if (this->unk_84F == 0) {
                    this->unk_850 = -10;
                }
            }

            this->unk_850++;
        }
    }
    else {
        if (this->unk_850 < 0) {
            this->unk_850++;

            if (this->unk_850 == 0) {
                gSaveContext.respawn[RESPAWN_MODE_TOP].data = 1;
                Gameplay_SetupRespawnPoint(globalCtx, RESPAWN_MODE_TOP, 0x6FF);
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
                this->unk_850 = 2;
            }
        }
        else if (this->unk_84F >= 0) {
            if (this->unk_850 == 0) {
                func_80832924(this, D_80854A80);
            }
            else if (this->unk_850 == 1) {
                func_80832924(this, D_80854A8C[this->unk_84F]);
                if ((this->unk_84F == 2) && LinkAnimation_OnFrame(&this->skelAnime, 30.0f)) {
                    this->stateFlags1 &= ~(PLAYER_STATE1_28 | PLAYER_STATE1_29);
                }
            }
            else if (D_80854A7C[this->unk_84F] < this->unk_850++) {
                LinkAnimation_PlayOnceSetSpeed(globalCtx, &this->skelAnime, D_80854A70[this->unk_84F], 0.83f);
                this->currentYaw = this->actor.shape.rot.y;
                this->unk_84F = -1;
            }
        }
    }

    func_8083721C(this);
}

void func_80850AEC(Player* this, GlobalContext* globalCtx) {
    f32 temp;

    this->stateFlags2 |= PLAYER_STATE2_5;

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80832284(globalCtx, this, &gPlayerAnim_002C98);
    }

    Math_Vec3f_Sum(&this->actor.world.pos, &this->actor.velocity, &this->actor.world.pos);

    if (func_80834FBC(this)) {
        Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
        func_80847BA0(globalCtx, this);

        temp = this->actor.world.pos.y - this->actor.floorHeight;
        if (temp > 20.0f) {
            temp = 20.0f;
        }

        this->actor.world.rot.x = this->actor.shape.rot.x = 0;
        this->actor.world.pos.y -= temp;
        this->linearVelocity = 1.0f;
        this->actor.velocity.y = 0.0f;
        func_80837B9C(this, globalCtx);
        this->stateFlags2 &= ~PLAYER_STATE2_10;
        this->actor.bgCheckFlags |= 1;
        this->stateFlags1 |= PLAYER_STATE1_2;
        return;
    }

    if ((this->skelAnime.animation != &gPlayerAnim_002C90) || (4.0f <= this->skelAnime.curFrame)) {
        this->actor.gravity = 0.0f;
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->actor.world.rot.x, 0x800);
        func_8083264C(this, 100, 2, 100, 0);
    }
}

void func_80850C68(Player* this, GlobalContext* globalCtx) {
    if ((this->unk_850 != 0) && ((this->unk_858 != 0.0f) || (this->unk_85C != 0.0f))) {
        f32 updateScale = R_UPDATE_RATE * 0.5f;

        this->skelAnime.curFrame += this->skelAnime.playSpeed * updateScale;
        if (this->skelAnime.curFrame >= this->skelAnime.animLength) {
            this->skelAnime.curFrame -= this->skelAnime.animLength;
        }

        LinkAnimation_BlendToJoint(globalCtx, &this->skelAnime, &gPlayerAnim_002C38, this->skelAnime.curFrame,
            (this->unk_858 < 0.0f) ? &gPlayerAnim_002C18 : &gPlayerAnim_002C20, 5.0f,
            fabsf(this->unk_858), this->blendTable);
        LinkAnimation_BlendToMorph(globalCtx, &this->skelAnime, &gPlayerAnim_002C38, this->skelAnime.curFrame,
            (this->unk_85C < 0.0f) ? &gPlayerAnim_002C28 : &gPlayerAnim_002C10, 5.0f,
            fabsf(this->unk_85C), D_80858AD8);
        LinkAnimation_InterpJointMorph(globalCtx, &this->skelAnime, 0.5f);
    }
    else if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        this->unk_860 = 2;
        func_80832284(globalCtx, this, &gPlayerAnim_002C38);
        this->unk_850 = 1;
    }

    func_8083721C(this);

    if (this->unk_860 == 0) {
        func_80853080(this, globalCtx);
    }
    else if (this->unk_860 == 3) {
        func_80835C58(globalCtx, this, func_80850E84, 0);
        func_80832B0C(globalCtx, this, &gPlayerAnim_002C00);
    }
}

void func_80850E84(Player* this, GlobalContext* globalCtx) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime) && (this->unk_860 == 0)) {
        func_8083A098(this, &gPlayerAnim_002C08, globalCtx);
    }
}

static void (*D_80854AA4[])(GlobalContext*, Player*, void*) = {
    NULL,          func_80851008, func_80851030, func_80851094, func_808510B4, func_808510D4, func_808510F4,
    func_80851114, func_80851134, func_80851154, func_80851174, func_808511D4, func_808511FC, func_80851294,
    func_80851050, func_80851194, func_808511B4, func_80851248, func_808512E0,
};

static struct_80832924 D_80854AF0[] = {
    { 0, 0x2822 },
    { NA_SE_PL_CALM_HIT, 0x82D },
    { NA_SE_PL_CALM_HIT, 0x833 },
    { NA_SE_PL_CALM_HIT, -0x840 },
};

static struct_80832924 D_80854B00[] = {
    { NA_SE_VO_LI_SURPRISE, 0x2003 }, { 0, 0x300F }, { 0, 0x3018 }, { 0, 0x301E }, { NA_SE_VO_LI_FALL_L, -0x201F },
};

static struct_80832924 D_80854B14[] = {
    { 0, -0x300A },
};

static struct_80854B18 D_80854B18[] = {
    { 0, NULL },
    { -1, func_808515A4 },
    { 2, &gPlayerAnim_002790 },
    { 0, NULL },
    { 0, NULL },
    { 3, &gPlayerAnim_002740 },
    { 0, NULL },
    { 0, NULL },
    { -1, func_808515A4 },
    { 2, &gPlayerAnim_002778 },
    { -1, func_80851788 },
    { 3, &gPlayerAnim_002860 },
    { -1, func_808518DC },
    { 7, &gPlayerAnim_002348 },
    { 5, &gPlayerAnim_002350 },
    { 5, &gPlayerAnim_002358 },
    { 5, &gPlayerAnim_0023B0 },
    { 7, &gPlayerAnim_0023B8 },
    { -1, func_808519EC },
    { 2, &gPlayerAnim_002728 },
    { 2, &gPlayerAnim_002738 },
    { 0, NULL },
    { -1, func_80851B90 },
    { 3, &gPlayerAnim_0027A8 },
    { 9, &gPlayerAnim_002DB0 },
    { 2, &gPlayerAnim_002DC0 },
    { -1, func_80851D2C },
    { 2, &gPlayerAnim_003098 },
    { 3, &gPlayerAnim_002780 },
    { -1, func_808515A4 },
    { 2, &gPlayerAnim_003088 },
    { 0, NULL },
    { 0, NULL },
    { 5, &gPlayerAnim_002320 },
    { -1, func_80851368 },
    { -1, func_80851E64 },
    { 5, &gPlayerAnim_002328 },
    { 16, &gPlayerAnim_002F90 },
    { -1, func_80851F84 },
    { -1, func_80851E90 },
    { 6, &gPlayerAnim_002410 },
    { 6, &gPlayerAnim_002418 },
    { -1, func_80852080 },
    { 5, &gPlayerAnim_002390 },
    { -1, func_808521F4 },
    { -1, func_8085225C },
    { -1, func_80852280 },
    { 5, &gPlayerAnim_0023A0 },
    { 5, &gPlayerAnim_002368 },
    { -1, func_808515A4 },
    { 5, &gPlayerAnim_002370 },
    { 5, &gPlayerAnim_0027B0 },
    { 5, &gPlayerAnim_0027B8 },
    { 5, &gPlayerAnim_0027C0 },
    { 3, &gPlayerAnim_002768 },
    { 3, &gPlayerAnim_0027D8 },
    { 4, &gPlayerAnim_0027E0 },
    { 3, &gPlayerAnim_002380 },
    { 3, &gPlayerAnim_002828 },
    { 6, &gPlayerAnim_002470 },
    { 6, &gPlayerAnim_0032A8 },
    { 14, &gPlayerAnim_0032A0 },
    { 3, &gPlayerAnim_0032A0 },
    { 5, &gPlayerAnim_002AE8 },
    { 16, &gPlayerAnim_002450 },
    { 15, &gPlayerAnim_002460 },
    { 15, &gPlayerAnim_002458 },
    { 3, &gPlayerAnim_002440 },
    { 3, &gPlayerAnim_002438 },
    { 3, &gPlayerAnim_002C88 },
    { 6, &gPlayerAnim_003450 },
    { 6, &gPlayerAnim_003448 },
    { 6, &gPlayerAnim_003460 },
    { 6, &gPlayerAnim_003440 },
    { 3, &gPlayerAnim_002798 },
    { 3, &gPlayerAnim_002818 },
    { 4, &gPlayerAnim_002848 },
    { 3, &gPlayerAnim_002850 },
    { 3, &gPlayerAnim_0034E0 },
    { 3, &gPlayerAnim_0034D8 },
    { 6, &gPlayerAnim_0034C8 },
    { 3, &gPlayerAnim_003470 },
    { 3, &gPlayerAnim_003478 },
    { 3, &gPlayerAnim_0034C0 },
    { 3, &gPlayerAnim_003480 },
    { 3, &gPlayerAnim_003490 },
    { 3, &gPlayerAnim_003488 },
    { 3, &gPlayerAnim_003498 },
    { 3, &gPlayerAnim_0034B0 },
    { -1, func_808524B0 },
    { 3, &gPlayerAnim_003420 },
    { -1, func_80852544 },
    { -1, func_80852564 },
    { 3, &gPlayerAnim_003250 },
    { -1, func_80852608 },
    { 3, &gPlayerAnim_002810 },
    { 3, &gPlayerAnim_002838 },
    { 3, &gPlayerAnim_002CD0 },
    { 3, &gPlayerAnim_002CD8 },
    { 3, &gPlayerAnim_002868 },
    { 3, &gPlayerAnim_0027E8 },
    { 3, &gPlayerAnim_0027F8 },
    { 3, &gPlayerAnim_002800 },
};

static struct_80854B18 D_80854E50[] = {
    { 0, NULL },
    { -1, func_808514C0 },
    { -1, func_8085157C },
    { -1, func_80851998 },
    { -1, func_808519C0 },
    { 11, NULL },
    { -1, func_80852C50 },
    { -1, func_80852944 },
    { -1, func_80851688 },
    { -1, func_80851750 },
    { -1, func_80851828 },
    { -1, func_808521B8 },
    { -1, func_8085190C },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { 18, D_80854AF0 },
    { 11, NULL },
    { -1, func_80851A50 },
    { 12, &gPlayerAnim_002730 },
    { 11, NULL },
    { 0, NULL },
    { -1, func_80851BE8 },
    { 11, NULL },
    { -1, func_80851CA4 },
    { 11, NULL },
    { 17, &gPlayerAnim_0030A8 },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { -1, func_80851D80 },
    { -1, func_80851DEC },
    { -1, func_80851E28 },
    { 18, D_80854B00 },
    { -1, func_808513BC },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { -1, func_80851ECC },
    { -1, func_80851FB0 },
    { -1, func_80852048 },
    { -1, func_80852174 },
    { 13, &gPlayerAnim_002398 },
    { -1, func_80852234 },
    { 0, NULL },
    { 0, NULL },
    { 11, NULL },
    { -1, func_80852450 },
    { -1, func_80851688 },
    { -1, func_80852298 },
    { 13, &gPlayerAnim_0027D0 },
    { -1, func_80852480 },
    { 13, &gPlayerAnim_0027C8 },
    { -1, func_80852328 },
    { 11, NULL },
    { 11, NULL },
    { 12, &gPlayerAnim_002388 },
    { -1, func_80852358 },
    { 11, NULL },
    { 18, D_80854B14 },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { -1, func_80852388 },
    { 17, &gPlayerAnim_002450 },
    { 12, &gPlayerAnim_002448 },
    { 12, &gPlayerAnim_002450 },
    { 11, NULL },
    { -1, func_808526EC },
    { 17, &gPlayerAnim_003468 },
    { -1, func_808526EC },
    { 17, &gPlayerAnim_003468 },
    { 12, &gPlayerAnim_0027A0 },
    { 12, &gPlayerAnim_002820 },
    { 11, NULL },
    { 12, &gPlayerAnim_002858 },
    { 12, &gPlayerAnim_0034D0 },
    { 13, &gPlayerAnim_0034F0 },
    { 12, &gPlayerAnim_0034E8 },
    { 12, &gPlayerAnim_0034A8 },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { -1, func_80852648 },
    { 11, NULL },
    { 12, &gPlayerAnim_0034A0 },
    { -1, func_808524D0 },
    { -1, func_80852514 },
    { -1, func_80852554 },
    { -1, func_808525C0 },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { -1, func_8085283C },
    { -1, func_808528C8 },
    { -1, func_808528C8 },
    { 12, &gPlayerAnim_002870 },
    { 12, &gPlayerAnim_0027F0 },
    { 12, &gPlayerAnim_002808 },
    { 12, &gPlayerAnim_002450 },
};

void func_80850ED8(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    func_80832DB0(this);
    func_80832B0C(globalCtx, this, anim);
    func_80832210(this);
}

void func_80850F1C(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    func_80832DB0(this);
    LinkAnimation_Change(globalCtx, &this->skelAnime, anim, (2.0f / 3.0f), 0.0f, Animation_GetLastFrame(anim),
        ANIMMODE_ONCE, -8.0f);
    func_80832210(this);
}

void func_80850F9C(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim) {
    func_80832DB0(this);
    LinkAnimation_Change(globalCtx, &this->skelAnime, anim, (2.0f / 3.0f), 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f);
    func_80832210(this);
}

void func_80851008(GlobalContext* globalCtx, Player* this, void* anim) {
    func_80832210(this);
}

void func_80851030(GlobalContext* globalCtx, Player* this, void* anim) {
    func_80850ED8(globalCtx, this, anim);
}

void func_80851050(GlobalContext* globalCtx, Player* this, void* anim) {
    func_80832DB0(this);
    func_80832C2C(globalCtx, this, anim);
    func_80832210(this);
}

void func_80851094(GlobalContext* globalCtx, Player* this, void* anim) {
    func_80850F1C(globalCtx, this, anim);
}

void func_808510B4(GlobalContext* globalCtx, Player* this, void* anim) {
    func_80850F9C(globalCtx, this, anim);
}

void func_808510D4(GlobalContext* globalCtx, Player* this, void* anim) {
    func_8083308C(globalCtx, this, anim);
}

void func_808510F4(GlobalContext* globalCtx, Player* this, void* anim) {
    func_8083303C(globalCtx, this, anim, 0x9C);
}

void func_80851114(GlobalContext* globalCtx, Player* this, void* anim) {
    func_8083313C(globalCtx, this, anim);
}

void func_80851134(GlobalContext* globalCtx, Player* this, void* anim) {
    func_808330EC(globalCtx, this, anim, 0x9C);
}

void func_80851154(GlobalContext* globalCtx, Player* this, void* anim) {
    func_80832264(globalCtx, this, anim);
}

void func_80851174(GlobalContext* globalCtx, Player* this, void* anim) {
    func_80832284(globalCtx, this, anim);
}

void func_80851194(GlobalContext* globalCtx, Player* this, void* anim) {
    func_808322D0(globalCtx, this, anim);
}

void func_808511B4(GlobalContext* globalCtx, Player* this, void* anim) {
    func_808322A4(globalCtx, this, anim);
}

void func_808511D4(GlobalContext* globalCtx, Player* this, void* anim) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);
}

void func_808511FC(GlobalContext* globalCtx, Player* this, void* anim) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80850F9C(globalCtx, this, anim);
        this->unk_850 = 1;
    }
}

void func_80851248(GlobalContext* globalCtx, Player* this, void* anim) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80832DBC(this);
        func_808322A4(globalCtx, this, anim);
    }
}

void func_80851294(GlobalContext* globalCtx, Player* this, void* anim) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_8083313C(globalCtx, this, anim);
        this->unk_850 = 1;
    }
}

void func_808512E0(GlobalContext* globalCtx, Player* this, void* arg2) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);
    func_80832924(this, arg2);
}

void func_80851314(Player* this) {
    if ((this->unk_448 == NULL) || (this->unk_448->update == NULL)) {
        this->unk_448 = NULL;
    }

    this->unk_664 = this->unk_448;

    if (this->unk_664 != NULL) {
        this->actor.shape.rot.y = func_8083DB98(this, 0);
    }
}

void func_80851368(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    this->stateFlags1 |= PLAYER_STATE1_27;
    this->stateFlags2 |= PLAYER_STATE2_10;
    this->stateFlags1 &= ~(PLAYER_STATE1_18 | PLAYER_STATE1_19);

    func_80832284(globalCtx, this, &gPlayerAnim_0032F0);
}

void func_808513BC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    this->actor.gravity = 0.0f;

    if (this->unk_84F == 0) {
        if (func_8083D12C(globalCtx, this, NULL)) {
            this->unk_84F = 1;
        }
        else {
            func_8084B158(globalCtx, this, NULL, fabsf(this->actor.velocity.y));
            Math_ScaledStepToS(&this->unk_6C2, -10000, 800);
            func_8084AEEC(this, &this->actor.velocity.y, 4.0f, this->currentYaw);
        }
        return;
    }

    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        if (this->unk_84F == 1) {
            func_80832C6C(globalCtx, this, &gPlayerAnim_003328);
        }
        else {
            func_80832284(globalCtx, this, &gPlayerAnim_003328);
        }
    }

    func_8084B000(this);
    func_8084AEEC(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

void func_808514C0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80851314(this);

    if (func_808332B8(this)) {
        func_808513BC(globalCtx, this, 0);
        return;
    }

    LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (func_8008F128(this) || (this->stateFlags1 & PLAYER_STATE1_11)) {
        func_80836670(this, globalCtx);
        return;
    }

    if ((this->interactRangeActor != NULL) && (this->interactRangeActor->textId == 0xFFFF)) {
        func_8083E5A8(this, globalCtx);
    }
}

void func_8085157C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);
}

void func_808515A4(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimationHeader* anim;

    if (func_808332B8(this)) {
        func_80851368(globalCtx, this, 0);
        return;
    }

    anim = D_80853914[PLAYER_ANIMGROUP_44][this->modelAnimType];

    if ((this->unk_446 == 6) || (this->unk_446 == 0x2E)) {
        func_80832264(globalCtx, this, anim);
    }
    else {
        func_80832DB0(this);
        LinkAnimation_Change(globalCtx, &this->skelAnime, anim, (2.0f / 3.0f), 0.0f, Animation_GetLastFrame(anim),
            ANIMMODE_LOOP, -4.0f);
    }

    func_80832210(this);
}

void func_80851688(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    if (func_8084B3CC(globalCtx, this) == 0) {
        if ((this->csMode == 0x31) && (globalCtx->csCtx.state == CS_STATE_IDLE)) {
            func_8002DF54(globalCtx, NULL, 7);
            return;
        }

        if (func_808332B8(this) != 0) {
            func_808513BC(globalCtx, this, 0);
            return;
        }

        LinkAnimation_Update(globalCtx, &this->skelAnime);

        if (func_8008F128(this) || (this->stateFlags1 & PLAYER_STATE1_11)) {
            func_80836670(this, globalCtx);
        }
    }
}

static struct_80832924 D_80855188[] = {
    { 0, 0x302A },
    { 0, -0x3030 },
};

void func_80851750(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);
    func_80832924(this, D_80855188);
}

void func_80851788(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    this->stateFlags1 &= ~PLAYER_STATE1_25;

    this->currentYaw = this->actor.shape.rot.y = this->actor.world.rot.y =
        Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_450);

    if (this->linearVelocity <= 0.0f) {
        this->linearVelocity = 0.1f;
    }
    else if (this->linearVelocity > 2.5f) {
        this->linearVelocity = 2.5f;
    }
}

void func_80851828(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    f32 sp1C = 2.5f;

    func_80845BA0(globalCtx, this, &sp1C, 10);

    if (globalCtx->sceneNum == SCENE_BDAN_BOSS) {
        if (this->unk_850 == 0) {
            if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_NONE) {
                return;
            }
        }
        else {
            if (Message_GetState(&globalCtx->msgCtx) != TEXT_STATE_NONE) {
                return;
            }
        }
    }

    this->unk_850++;
    if (this->unk_850 > 20) {
        this->csMode = 0xB;
    }
}

void func_808518DC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_8083CEAC(this, globalCtx);
}

void func_8085190C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80851314(this);

    if (this->unk_850 != 0) {
        if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
            func_80832284(globalCtx, this, func_808334E4(this));
            this->unk_850 = 0;
        }

        func_80833C3C(this);
    }
    else {
        func_808401B0(globalCtx, this);
    }
}

void func_80851998(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80845964(globalCtx, this, arg2, 0.0f, 0, 0);
}

void func_808519C0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80845964(globalCtx, this, arg2, 0.0f, 0, 1);
}

// unused
static LinkAnimationHeader* D_80855190[] = {
    &gPlayerAnim_002720,
    &gPlayerAnim_002360,
};

static Vec3f D_80855198 = { -1.0f, 70.0f, 20.0f };

void func_808519EC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    Math_Vec3f_Copy(&this->actor.world.pos, &D_80855198);
    this->actor.shape.rot.y = -0x8000;
    func_808322D0(globalCtx, this, this->ageProperties->unk_9C);
    func_80832F54(globalCtx, this, 0x28F);
}

static struct_808551A4 D_808551A4[] = {
    { NA_SE_IT_SWORD_PUTAWAY_STN, 0 },
    { NA_SE_IT_SWORD_STICK_STN, NA_SE_VO_LI_SWORD_N },
};

static struct_80832924 D_808551AC[] = {
    { 0, 0x401D },
    { 0, -0x4027 },
};

void func_80851A50(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    struct_808551A4* sp2C;
    Gfx** dLists;

    LinkAnimation_Update(globalCtx, &this->skelAnime);

    if ((LINK_IS_ADULT && LinkAnimation_OnFrame(&this->skelAnime, 70.0f)) ||
        (!LINK_IS_ADULT && LinkAnimation_OnFrame(&this->skelAnime, 87.0f))) {
        sp2C = &D_808551A4[gSaveContext.linkAge];
        this->interactRangeActor->parent = &this->actor;

        if (!LINK_IS_ADULT) {
            dLists = D_80125DE8;
        }
        else {
            dLists = D_80125E18;
        }
        this->leftHandDLists = &dLists[gSaveContext.linkAge];

        func_8002F7DC(&this->actor, sp2C->unk_00);
        if (!LINK_IS_ADULT) {
            func_80832698(this, sp2C->unk_02);
        }
    }
    else if (LINK_IS_ADULT) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 66.0f)) {
            func_80832698(this, NA_SE_VO_LI_SWORD_L);
        }
    }
    else {
        func_80832924(this, D_808551AC);
    }
}

void func_80851B90(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Change(globalCtx, &this->skelAnime, &gPlayerAnim_002860, -(2.0f / 3.0f), 12.0f, 12.0f, ANIMMODE_ONCE,
        0.0f);
}

static struct_80832924 D_808551B4[] = {
    { 0, -0x281E },
};

void func_80851BE8(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);

    this->unk_850++;

    if (this->unk_850 >= 180) {
        if (this->unk_850 == 180) {
            LinkAnimation_Change(globalCtx, &this->skelAnime, &gPlayerAnim_003298, (2.0f / 3.0f), 10.0f,
                Animation_GetLastFrame(&gPlayerAnim_003298), ANIMMODE_ONCE, -8.0f);
        }
        func_80832924(this, D_808551B4);
    }
}

void func_80851CA4(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime) && (this->unk_850 == 0) && (this->actor.bgCheckFlags & 1)) {
        func_80832264(globalCtx, this, &gPlayerAnim_002DB8);
        this->unk_850 = 1;
    }

    if (this->unk_850 != 0) {
        func_8083721C(this);
    }
}

void func_80851D2C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80850F1C(globalCtx, this, &gPlayerAnim_0030A0);
    func_8084B498(this);
    Player_SetModels(this, Player_ActionToModelGroup(this, this->itemActionParam));
}

static struct_80832924 D_808551B8[] = {
    { NA_SE_IT_SWORD_PICKOUT, -0x80C },
};

void func_80851D80(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 6.0f)) {
        func_80846720(globalCtx, this, 0);
    }
    else {
        func_80832924(this, D_808551B8);
    }
}

void func_80851DEC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);
    Math_StepToS(&this->actor.shape.face, 0, 1);
}

void func_80851E28(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);
    Math_StepToS(&this->actor.shape.face, 2, 1);
}

void func_80851E64(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80833064(globalCtx, this, &gPlayerAnim_003318, 0x98);
}

void func_80851E90(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_8083303C(globalCtx, this, &gPlayerAnim_002408, 0x9C);
    func_80832698(this, NA_SE_VO_LI_GROAN);
}

void func_80851ECC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_808330EC(globalCtx, this, &gPlayerAnim_002428, 0x9C);
    }
}

void func_80851F14(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim, struct_80832924* arg3) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_808322A4(globalCtx, this, anim);
        this->unk_850 = 1;
    }
    else if (this->unk_850 == 0) {
        func_80832924(this, arg3);
    }
}

void func_80851F84(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    this->actor.shape.shadowDraw = NULL;
    func_80851134(globalCtx, this, &gPlayerAnim_002420);
}

static struct_80832924 D_808551BC[] = {
    { NA_SE_VO_LI_RELAX, 0x2023 },
    { NA_SE_PL_SLIPDOWN, 0x8EC },
    { NA_SE_PL_SLIPDOWN, -0x900 },
};

void func_80851FB0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_808330EC(globalCtx, this, &gPlayerAnim_002430, 0x9C);
        this->unk_850 = 1;
    }
    else if (this->unk_850 == 0) {
        func_80832924(this, D_808551BC);
        if (LinkAnimation_OnFrame(&this->skelAnime, 240.0f)) {
            this->actor.shape.shadowDraw = ActorShadow_DrawFeet;
        }
    }
}

static struct_80832924 D_808551C8[] = {
    { NA_SE_PL_LAND_LADDER, 0x843 },
    { 0, 0x4854 },
    { 0, 0x485A },
    { 0, -0x4860 },
};

void func_80852048(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);
    func_80832924(this, D_808551C8);
}

void func_80852080(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80833064(globalCtx, this, &gPlayerAnim_002340, 0x9D);
    func_80832698(this, NA_SE_VO_LI_FALL_L);
}

void func_808520BC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    f32 startX = arg2->startPos.x;
    f32 startY = arg2->startPos.y;
    f32 startZ = arg2->startPos.z;
    f32 distX = (arg2->endPos.x - startX);
    f32 distY = (arg2->endPos.y - startY);
    f32 distZ = (arg2->endPos.z - startZ);
    f32 sp4 = (f32)(globalCtx->csCtx.frames - arg2->startFrame) / (f32)(arg2->endFrame - arg2->startFrame);

    this->actor.world.pos.x = distX * sp4 + startX;
    this->actor.world.pos.y = distY * sp4 + startY;
    this->actor.world.pos.z = distZ * sp4 + startZ;
}

static struct_80832924 D_808551D8[] = {
    { NA_SE_PL_BOUND, 0x1014 },
    { NA_SE_PL_BOUND, -0x101E },
};

void func_80852174(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_808520BC(globalCtx, this, arg2);
    LinkAnimation_Update(globalCtx, &this->skelAnime);
    func_80832924(this, D_808551D8);
}

void func_808521B8(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    if (arg2 != NULL) {
        func_808520BC(globalCtx, this, arg2);
    }
    LinkAnimation_Update(globalCtx, &this->skelAnime);
}

void func_808521F4(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80832B0C(globalCtx, this, D_80853914[PLAYER_ANIMGROUP_44][this->modelAnimType]);
    func_80832210(this);
}

void func_80852234(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);
}

void func_8085225C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80832F54(globalCtx, this, 0x98);
}

void func_80852280(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    this->actor.draw = Player_Draw;
}

void func_80852298(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_8083313C(globalCtx, this, &gPlayerAnim_002378);
        this->unk_850 = 1;
    }
    else if (this->unk_850 == 0) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 10.0f)) {
            func_80846720(globalCtx, this, 1);
        }
    }
}

static struct_80832924 D_808551E0[] = {
    { 0, 0x300A },
    { 0, -0x3018 },
};

void func_80852328(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80851F14(globalCtx, this, &gPlayerAnim_002770, D_808551E0);
}

static struct_80832924 D_808551E8[] = {
    { 0, 0x400F },
    { 0, -0x4023 },
};

void func_80852358(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80851F14(globalCtx, this, &gPlayerAnim_002830, D_808551E8);
}

void func_80852388(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_808322A4(globalCtx, this, &gPlayerAnim_002468);
        this->unk_850 = 1;
    }

    if ((this->unk_850 != 0) && (globalCtx->csCtx.frames >= 900)) {
        this->rightHandType = PLAYER_MODELTYPE_LH_OPEN;
    }
    else {
        this->rightHandType = PLAYER_MODELTYPE_RH_FF;
    }
}

void func_80852414(GlobalContext* globalCtx, Player* this, LinkAnimationHeader* anim, struct_80832924* arg3) {
    func_80851294(globalCtx, this, anim);
    if (this->unk_850 == 0) {
        func_80832924(this, arg3);
    }
}

static struct_80832924 D_808551F0[] = {
    { 0, 0x300F },
    { 0, -0x3021 },
};

void func_80852450(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80852414(globalCtx, this, &gPlayerAnim_002378, D_808551F0);
}

static struct_80832924 D_808551F8[] = {
    { NA_SE_PL_KNOCK, -0x84E },
};

void func_80852480(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80852414(globalCtx, this, &gPlayerAnim_0027D0, D_808551F8);
}

void func_808524B0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80837704(globalCtx, this);
}

void func_808524D0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    sControlInput->press.button |= BTN_B;

    func_80844E68(this, globalCtx);
}

void func_80852514(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80844E68(this, globalCtx);
}

void func_80852544(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
}

void func_80852554(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
}

void func_80852564(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    this->stateFlags3 |= PLAYER_STATE3_1;
    this->linearVelocity = 2.0f;
    this->actor.velocity.y = -1.0f;

    func_80832264(globalCtx, this, &gPlayerAnim_002DB0);
    func_80832698(this, NA_SE_VO_LI_FALL_L);
}

static void (*D_808551FC[])(Player* this, GlobalContext* globalCtx) = {
    func_8084377C,
    func_80843954,
    func_80843A38,
};

void func_808525C0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    D_808551FC[this->unk_850](this, globalCtx);
}

void func_80852608(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    func_80846720(globalCtx, this, 0);
    func_808322D0(globalCtx, this, &gPlayerAnim_002838);
}

void func_80852648(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(globalCtx, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 10.0f)) {
        this->heldItemActionParam = this->itemActionParam = PLAYER_AP_NONE;
        this->heldItemId = ITEM_NONE;
        this->modelGroup = this->nextModelGroup = Player_ActionToModelGroup(this, PLAYER_AP_NONE);
        this->leftHandDLists = D_80125E08;
        Inventory_ChangeEquipment(EQUIP_SWORD, 2);
        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_MASTER;
        Inventory_DeleteEquipment(globalCtx, 0);
    }
}

static LinkAnimationHeader* D_80855208[] = {
    &gPlayerAnim_0034B8,
    &gPlayerAnim_003458,
};

static Vec3s D_80855210[2][2] = {
    { { -200, 700, 100 }, { 800, 600, 800 } },
    { { -200, 500, 0 }, { 600, 400, 600 } },
};

void func_808526EC(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 0 };
    static Color_RGBA8 envColor = { 0, 128, 128, 0 };
    s32 age = gSaveContext.linkAge;
    Vec3f sparklePos;
    Vec3f sp34;
    Vec3s* ptr;

    func_80851294(globalCtx, this, D_80855208[age]);

    if (this->rightHandType != PLAYER_MODELTYPE_RH_FF) {
        this->rightHandType = PLAYER_MODELTYPE_RH_FF;
        return;
    }

    ptr = D_80855210[gSaveContext.linkAge];

    sp34.x = ptr[0].x + Rand_CenteredFloat(ptr[1].x);
    sp34.y = ptr[0].y + Rand_CenteredFloat(ptr[1].y);
    sp34.z = ptr[0].z + Rand_CenteredFloat(ptr[1].z);

    SkinMatrix_Vec3fMtxFMultXYZ(&this->shieldMf, &sp34, &sparklePos);

    EffectSsKiraKira_SpawnDispersed(globalCtx, &sparklePos, &zeroVec, &zeroVec, &primColor, &envColor, 600, -10);
}

void func_8085283C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_80852944(globalCtx, this, arg2);
    }
    else if (this->unk_850 == 0) {
        Item_Give(globalCtx, ITEM_SWORD_MASTER);
        func_80846720(globalCtx, this, 0);
    }
    else {
        func_8084E988(this);
    }
}

void func_808528C8(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(globalCtx, &this->skelAnime)) {
        func_8084285C(this, 0.0f, 99.0f, this->skelAnime.endFrame - 8.0f);
    }

    if (this->heldItemActionParam != PLAYER_AP_SWORD_MASTER) {
        func_80846720(globalCtx, this, 1);
    }
}

void func_80852944(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    if (func_808332B8(this)) {
        func_80838F18(globalCtx, this);
        func_80832340(globalCtx, this);
    }
    else {
        func_8083C148(this, globalCtx);
        if (!func_8083B644(this, globalCtx)) {
            func_8083E5A8(this, globalCtx);
        }
    }

    this->csMode = 0;
    this->unk_6AD = 0;
}

void func_808529D0(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    this->actor.world.pos.x = arg2->startPos.x;
    this->actor.world.pos.y = arg2->startPos.y;
    if ((globalCtx->sceneNum == SCENE_SPOT04) && !LINK_IS_ADULT) {
        this->actor.world.pos.y -= 1.0f;
    }
    this->actor.world.pos.z = arg2->startPos.z;
    this->currentYaw = this->actor.shape.rot.y = arg2->rot.y;
}

void func_80852A54(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    f32 dx = arg2->startPos.x - (s32)this->actor.world.pos.x;
    f32 dy = arg2->startPos.y - (s32)this->actor.world.pos.y;
    f32 dz = arg2->startPos.z - (s32)this->actor.world.pos.z;
    f32 dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));
    s16 yawDiff = arg2->rot.y - this->actor.shape.rot.y;

    if ((this->linearVelocity == 0.0f) && ((dist > 50.0f) || (ABS(yawDiff) > 0x4000))) {
        func_808529D0(globalCtx, this, arg2);
    }

    this->skelAnime.moveFlags = 0;
    func_80832DB0(this);
}

void func_80852B4C(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2, struct_80854B18* arg3) {
    if (arg3->type > 0) {
        D_80854AA4[arg3->type](globalCtx, this, arg3->ptr);
    }
    else if (arg3->type < 0) {
        arg3->func(globalCtx, this, arg2);
    }

    if ((D_80858AA0 & 4) && !(this->skelAnime.moveFlags & 4)) {
        this->skelAnime.morphTable[0].y /= this->ageProperties->unk_08;
        D_80858AA0 = 0;
    }
}

void func_80852C0C(GlobalContext* globalCtx, Player* this, s32 csMode) {
    if ((csMode != 1) && (csMode != 8) && (csMode != 0x31) && (csMode != 7)) {
        func_808323B4(globalCtx, this);
    }
}

void func_80852C50(GlobalContext* globalCtx, Player* this, CsCmdActorAction* arg2) {
    CsCmdActorAction* linkCsAction = globalCtx->csCtx.linkAction;
    s32 pad;
    s32 sp24;

    if (globalCtx->csCtx.state == CS_STATE_UNSKIPPABLE_INIT) {
        func_8002DF54(globalCtx, NULL, 7);
        this->unk_446 = 0;
        func_80832210(this);
        return;
    }

    if (linkCsAction == NULL) {
        this->actor.flags &= ~ACTOR_FLAG_6;
        return;
    }

    if (this->unk_446 != linkCsAction->action) {
        sp24 = D_808547C4[linkCsAction->action];
        if (sp24 >= 0) {
            if ((sp24 == 3) || (sp24 == 4)) {
                func_80852A54(globalCtx, this, linkCsAction);
            }
            else {
                func_808529D0(globalCtx, this, linkCsAction);
            }
        }

        D_80858AA0 = this->skelAnime.moveFlags;

        func_80832DBC(this);
        osSyncPrintf("TOOL MODE=%d\n", sp24);
        func_80852C0C(globalCtx, this, ABS(sp24));
        func_80852B4C(globalCtx, this, linkCsAction, &D_80854B18[ABS(sp24)]);

        this->unk_850 = 0;
        this->unk_84F = 0;
        this->unk_446 = linkCsAction->action;
    }

    sp24 = D_808547C4[this->unk_446];
    func_80852B4C(globalCtx, this, linkCsAction, &D_80854E50[ABS(sp24)]);
}

void func_80852E14(Player* this, GlobalContext* globalCtx) {
    if (this->csMode != this->prevCsMode) {
        D_80858AA0 = this->skelAnime.moveFlags;

        func_80832DBC(this);
        this->prevCsMode = this->csMode;
        osSyncPrintf("DEMO MODE=%d\n", this->csMode);
        func_80852C0C(globalCtx, this, this->csMode);
        func_80852B4C(globalCtx, this, NULL, &D_80854B18[this->csMode]);
    }

    func_80852B4C(globalCtx, this, NULL, &D_80854E50[this->csMode]);
}

s32 Player_IsDroppingFish(GlobalContext* globalCtx) {
    Player* this = GET_PLAYER(globalCtx);

    return (func_8084EFC0 == this->func_674) && (this->itemActionParam == PLAYER_AP_BOTTLE_FISH);
}

s32 Player_StartFishing(GlobalContext* globalCtx) {
    Player* this = GET_PLAYER(globalCtx);

    func_80832564(globalCtx, this);
    func_80835F44(globalCtx, this, ITEM_FISHING_POLE);
    return 1;
}

s32 func_80852F38(GlobalContext* globalCtx, Player* this) {
    if (!Player_InBlockingCsMode(globalCtx, this) && (this->invincibilityTimer >= 0) && !func_8008F128(this) &&
        !(this->stateFlags3 & PLAYER_STATE3_7)) {
        func_80832564(globalCtx, this);
        func_80835C58(globalCtx, this, func_8084F308, 0);
        func_80832264(globalCtx, this, &gPlayerAnim_003120);
        this->stateFlags2 |= PLAYER_STATE2_7;
        func_80832224(this);
        func_80832698(this, NA_SE_VO_LI_HELD);
        return true;
    }

    return false;
}

// Sets up player cutscene
s32 func_80852FFC(GlobalContext* globalCtx, Actor* actor, s32 csMode) {
    Player* this = GET_PLAYER(globalCtx);

    if (!Player_InBlockingCsMode(globalCtx, this)) {
        func_80832564(globalCtx, this);
        func_80835C58(globalCtx, this, func_80852E14, 0);
        this->csMode = csMode;
        this->unk_448 = actor;
        func_80832224(this);
        return 1;
    }

    return 0;
}

void func_80853080(Player* this, GlobalContext* globalCtx) {
    func_80835C58(globalCtx, this, func_80840BC8, 1);
    func_80832B0C(globalCtx, this, func_80833338(this));
    this->currentYaw = this->actor.shape.rot.y;
}

s32 Player_InflictDamage(GlobalContext* globalCtx, s32 damage) {
    Player* this = GET_PLAYER(globalCtx);

    if (!Player_InBlockingCsMode(globalCtx, this) && !func_80837B18(globalCtx, this, damage)) {
        this->stateFlags2 &= ~PLAYER_STATE2_7;
        return 1;
    }

    return 0;
}

// Start talking with the given actor
void func_80853148(GlobalContext* globalCtx, Actor* actor) {
    Player* this = GET_PLAYER(globalCtx);
    s32 pad;

    if ((this->targetActor != NULL) || (actor == this->naviActor) ||
        CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_0 | ACTOR_FLAG_18)) {
        actor->flags |= ACTOR_FLAG_8;
    }

    this->targetActor = actor;
    this->exchangeItemId = EXCH_ITEM_NONE;

    if (actor->textId == 0xFFFF) {
        func_8002DF54(globalCtx, actor, 1);
        actor->flags |= ACTOR_FLAG_8;
        func_80832528(globalCtx, this);
    }
    else {
        if (this->actor.flags & ACTOR_FLAG_8) {
            this->actor.textId = 0;
        }
        else {
            this->actor.flags |= ACTOR_FLAG_8;
            this->actor.textId = actor->textId;
        }

        if (this->stateFlags1 & PLAYER_STATE1_23) {
            s32 sp24 = this->unk_850;

            func_80832528(globalCtx, this);
            func_8083A2F8(globalCtx, this);

            this->unk_850 = sp24;
        }
        else {
            if (func_808332B8(this)) {
                func_80836898(globalCtx, this, func_8083A2F8);
                func_80832C6C(globalCtx, this, &gPlayerAnim_003328);
            }
            else if ((actor->category != ACTORCAT_NPC) || (this->heldItemActionParam == PLAYER_AP_FISHING_POLE)) {
                func_8083A2F8(globalCtx, this);

                if (!func_8008E9C4(this)) {
                    if ((actor != this->naviActor) && (actor->xzDistToPlayer < 40.0f)) {
                        func_808322D0(globalCtx, this, &gPlayerAnim_002DF0);
                    }
                    else {
                        func_80832284(globalCtx, this, func_80833338(this));
                    }
                }
            }
            else {
                func_80836898(globalCtx, this, func_8083A2F8);
                func_808322D0(globalCtx, this,
                    (actor->xzDistToPlayer < 40.0f) ? &gPlayerAnim_002DF0 : &gPlayerAnim_0031A0);
            }

            if (this->skelAnime.animation == &gPlayerAnim_002DF0) {
                func_80832F54(globalCtx, this, 0x19);
            }

            func_80832224(this);
        }

        this->stateFlags1 |= PLAYER_STATE1_6 | PLAYER_STATE1_29;
    }

    if ((this->naviActor == this->targetActor) && ((this->targetActor->textId & 0xFF00) != 0x200)) {
        this->naviActor->flags |= ACTOR_FLAG_8;
        func_80835EA4(globalCtx, 0xB);
    }
}
