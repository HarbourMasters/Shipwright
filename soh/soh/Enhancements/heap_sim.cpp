#include <unordered_map>
#include <unordered_set>
#include <libultraship/bridge.h>
#include <spdlog/spdlog.h>

#include "heap_sim.h"

// Actor overlay must fully fit in this space when its alloc type is absolute
// (retail value; the debug rom uses 0x27A0)
#define HEAPSIM_ACTOROVL_ABSOLUTE_SPACE_SIZE 0x24E0

// N64 play state heap size (see GameState_Realloc in Play_Init, which doubles
// it for SoH), minus init allocations N64 makes but SoH skips or shrinks
// (sizes from an NTSC 1.2 rom): kaleido overlay area (max of kaleido_scope /
// player_actor), parameter_static, do-action texture buffer, minimap segment.
// Allocations SoH still makes at N64 size are deducted live by the
// GameState_Alloc hook; scene-dependent ones (skybox textures, dungeon map
// marks) are deducted per play below.
static const size_t kHeapSimSize = 0x1D4790 - 0x26740 - 0x3B00 - (3 * 0x180) - 0x1000;

// ovl_map_mark_data, loaded on N64 for dungeon scenes only
static const size_t kMapMarkOvlSize = 0x6B60;

// Simulated heap space left while Play_Init runs its GameState allocations;
// whatever remains becomes shadow ZeldaArena, like on console
static size_t heapSimAllocSize = kHeapSimSize;
static bool trackingGameAllocs = false;

static Arena sHeapSimArena;

// real ZeldaArena pointer -> shadow allocation
static std::unordered_map<void*, void*> mirrorMap;
// actor id -> shadow overlay allocation
static std::unordered_map<u16, void*> overlayPtrMap;
static std::unordered_set<u16> registeredOverlays;
static std::unordered_set<s32> registeredEffectOvls;
static void* absolutePtr = nullptr;

#define DEFINE_ACTOR_INTERNAL(name, id, allocType) { id, allocType },
#define DEFINE_ACTOR(name, id, allocType) { id, allocType },
#define DEFINE_ACTOR_UNSET(_0)

static const std::unordered_map<u16, u16> stairActorAllocTypes = {
#include "tables/actor_table.h"
};

#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET
#undef DEFINE_ACTOR

// overlay sizes from an NTSC 1.2 rom (decompressed file size + bss);
// actors missing here (Player, En_Item00, En_A_Obj, custom) have no overlay
static const std::unordered_map<u16, size_t> actorOverlaySizes = {
    { ACTOR_EN_TEST, 0x58B0 },
    { ACTOR_EN_GIRLA, 0x2920 },
    { ACTOR_EN_PART, 0x1610 },
    { ACTOR_EN_LIGHT, 0xDF0 },
    { ACTOR_EN_DOOR, 0xE40 },
    { ACTOR_EN_BOX, 0x1B40 },
    { ACTOR_BG_DY_YOSEIZO, 0x2E00 },
    { ACTOR_BG_HIDAN_FIREWALL, 0x760 },
    { ACTOR_EN_POH, 0x4190 },
    { ACTOR_EN_OKUTA, 0x25E0 },
    { ACTOR_BG_YDAN_SP, 0x1770 },
    { ACTOR_EN_BOM, 0xED0 },
    { ACTOR_EN_WALLMAS, 0x1A10 },
    { ACTOR_EN_DODONGO, 0x2DA0 },
    { ACTOR_EN_FIREFLY, 0x2170 },
    { ACTOR_EN_HORSE, 0xC260 },
    { ACTOR_EN_ARROW, 0x16F0 },
    { ACTOR_EN_ELF, 0x49C0 },
    { ACTOR_EN_NIW, 0x3330 },
    { ACTOR_EN_TITE, 0x2DA0 },
    { ACTOR_EN_REEBA, 0x1A70 },
    { ACTOR_EN_PEEHAT, 0x3700 },
    { ACTOR_EN_BUTTE, 0x15D0 },
    { ACTOR_EN_INSECT, 0x2520 },
    { ACTOR_EN_FISH, 0x2110 },
    { ACTOR_EN_HOLL, 0xFD0 },
    { ACTOR_EN_SCENE_CHANGE, 0x130 },
    { ACTOR_EN_ZF, 0x6B00 },
    { ACTOR_EN_HATA, 0x590 },
    { ACTOR_BOSS_DODONGO, 0x9AE0 },
    { ACTOR_BOSS_GOMA, 0x5F80 },
    { ACTOR_EN_ZL1, 0x3E00 },
    { ACTOR_EN_VIEWER, 0x2ED0 },
    { ACTOR_EN_GOMA, 0x2C90 },
    { ACTOR_BG_PUSHBOX, 0x300 },
    { ACTOR_EN_BUBBLE, 0x1420 },
    { ACTOR_DOOR_SHUTTER, 0x2280 },
    { ACTOR_EN_DODOJR, 0x1EA0 },
    { ACTOR_EN_BDFIRE, 0xB90 },
    { ACTOR_EN_BOOM, 0x8C0 },
    { ACTOR_EN_TORCH2, 0x27A0 },
    { ACTOR_EN_BILI, 0x22D0 },
    { ACTOR_EN_TP, 0x1E50 },
    { ACTOR_EN_ST, 0x2C70 },
    { ACTOR_EN_BW, 0x3370 },
    { ACTOR_EN_EIYER, 0x1C60 },
    { ACTOR_EN_RIVER_SOUND, 0x990 },
    { ACTOR_EN_HORSE_NORMAL, 0x2620 },
    { ACTOR_EN_OSSAN, 0x65E0 },
    { ACTOR_BG_TREEMOUTH, 0x1660 },
    { ACTOR_BG_DODOAGO, 0xDB0 },
    { ACTOR_BG_HIDAN_DALM, 0x850 },
    { ACTOR_BG_HIDAN_HROCK, 0x830 },
    { ACTOR_EN_HORSE_GANON, 0xD80 },
    { ACTOR_BG_HIDAN_ROCK, 0x10F0 },
    { ACTOR_BG_HIDAN_RSEKIZOU, 0xBE0 },
    { ACTOR_BG_HIDAN_SEKIZOU, 0x1450 },
    { ACTOR_BG_HIDAN_SIMA, 0xF20 },
    { ACTOR_BG_HIDAN_SYOKU, 0x460 },
    { ACTOR_EN_XC, 0x6790 },
    { ACTOR_BG_HIDAN_CURTAIN, 0xAA0 },
    { ACTOR_BG_SPOT00_HANEBASI, 0x1110 },
    { ACTOR_EN_MB, 0x4230 },
    { ACTOR_EN_BOMBF, 0x1470 },
    { ACTOR_EN_ZL2, 0x4730 },
    { ACTOR_BG_HIDAN_FSLIFT, 0x4D0 },
    { ACTOR_EN_OE2, 0xE0 },
    { ACTOR_BG_YDAN_HASI, 0x7B0 },
    { ACTOR_BG_YDAN_MARUTA, 0x6E0 },
    { ACTOR_BOSS_GANONDROF, 0x4D70 },
    { ACTOR_EN_AM, 0x2400 },
    { ACTOR_EN_DEKUBABA, 0x3AA0 },
    { ACTOR_EN_M_FIRE1, 0x1A0 },
    { ACTOR_EN_M_THUNDER, 0x15F0 },
    { ACTOR_BG_DDAN_JD, 0x650 },
    { ACTOR_BG_BREAKWALL, 0xE70 },
    { ACTOR_EN_JJ, 0x15D0 },
    { ACTOR_EN_HORSE_ZELDA, 0xAF0 },
    { ACTOR_BG_DDAN_KD, 0x8F0 },
    { ACTOR_DOOR_WARP1, 0x42B0 },
    { ACTOR_OBJ_SYOKUDAI, 0xC40 },
    { ACTOR_ITEM_B_HEART, 0x3F0 },
    { ACTOR_EN_DEKUNUTS, 0x1800 },
    { ACTOR_BG_MENKURI_KAITEN, 0x190 },
    { ACTOR_BG_MENKURI_EYE, 0x4A0 },
    { ACTOR_EN_VALI, 0x26A0 },
    { ACTOR_BG_MIZU_MOVEBG, 0x11A0 },
    { ACTOR_BG_MIZU_WATER, 0xCD0 },
    { ACTOR_ARMS_HOOK, 0xD60 },
    { ACTOR_EN_FHG, 0x2930 },
    { ACTOR_BG_MORI_HINERI, 0xD00 },
    { ACTOR_EN_BB, 0x3CD0 },
    { ACTOR_BG_TOKI_HIKARI, 0xDA0 },
    { ACTOR_EN_YUKABYUN, 0x610 },
    { ACTOR_BG_TOKI_SWD, 0x1650 },
    { ACTOR_EN_FHG_FIRE, 0x2620 },
    { ACTOR_BG_MJIN, 0x3E0 },
    { ACTOR_BG_HIDAN_KOUSI, 0x580 },
    { ACTOR_DOOR_TOKI, 0x160 },
    { ACTOR_BG_HIDAN_HAMSTEP, 0xEB0 },
    { ACTOR_EN_BIRD, 0x4C0 },
    { ACTOR_EN_WOOD02, 0x11E0 },
    { ACTOR_EN_LIGHTBOX, 0x480 },
    { ACTOR_EN_PU_BOX, 0x340 },
    { ACTOR_EN_TRAP, 0x12A0 },
    { ACTOR_EN_AROW_TRAP, 0x150 },
    { ACTOR_EN_VASE, 0x100 },
    { ACTOR_EN_TA, 0x39C0 },
    { ACTOR_EN_TK, 0x1E30 },
    { ACTOR_BG_MORI_BIGST, 0x930 },
    { ACTOR_BG_MORI_ELEVATOR, 0xAF0 },
    { ACTOR_BG_MORI_KAITENKABE, 0x660 },
    { ACTOR_BG_MORI_RAKKATENJO, 0x970 },
    { ACTOR_EN_VM, 0x18B0 },
    { ACTOR_DEMO_EFFECT, 0x5AF0 },
    { ACTOR_DEMO_KANKYO, 0x3D00 },
    { ACTOR_BG_HIDAN_FWBIG, 0xCE0 },
    { ACTOR_EN_FLOORMAS, 0x33E0 },
    { ACTOR_EN_HEISHI1, 0x1510 },
    { ACTOR_EN_RD, 0x28B0 },
    { ACTOR_EN_PO_SISTERS, 0x4CF0 },
    { ACTOR_BG_HEAVY_BLOCK, 0x18F0 },
    { ACTOR_BG_PO_EVENT, 0x1E40 },
    { ACTOR_OBJ_MURE, 0x1010 },
    { ACTOR_EN_SW, 0x37F0 },
    { ACTOR_BOSS_FD, 0x7330 },
    { ACTOR_OBJECT_KANKYO, 0x3220 },
    { ACTOR_EN_DU, 0x1AA0 },
    { ACTOR_EN_FD, 0x2CC0 },
    { ACTOR_EN_HORSE_LINK_CHILD, 0x1E00 },
    { ACTOR_DOOR_ANA, 0x670 },
    { ACTOR_BG_SPOT02_OBJECTS, 0x1350 },
    { ACTOR_BG_HAKA, 0x6C0 },
    { ACTOR_MAGIC_WIND, 0x1D00 },
    { ACTOR_MAGIC_FIRE, 0x22D0 },
    { ACTOR_EN_RU1, 0x76A0 },
    { ACTOR_BOSS_FD2, 0x3D30 },
    { ACTOR_EN_FD_FIRE, 0xD10 },
    { ACTOR_EN_DH, 0x1AD0 },
    { ACTOR_EN_DHA, 0x1000 },
    { ACTOR_EN_RL, 0xEE0 },
    { ACTOR_EN_ENCOUNT1, 0xB60 },
    { ACTOR_DEMO_DU, 0x37E0 },
    { ACTOR_DEMO_IM, 0x3F70 },
    { ACTOR_DEMO_TRE_LGT, 0x710 },
    { ACTOR_EN_FW, 0x17B0 },
    { ACTOR_BG_VB_SIMA, 0x710 },
    { ACTOR_EN_VB_BALL, 0x11A0 },
    { ACTOR_BG_HAKA_MEGANE, 0x400 },
    { ACTOR_BG_HAKA_MEGANEBG, 0x6C0 },
    { ACTOR_BG_HAKA_SHIP, 0xA40 },
    { ACTOR_BG_HAKA_SGAMI, 0xC20 },
    { ACTOR_EN_HEISHI2, 0x2200 },
    { ACTOR_EN_ENCOUNT2, 0x1230 },
    { ACTOR_EN_FIRE_ROCK, 0x1110 },
    { ACTOR_EN_BROB, 0x10F0 },
    { ACTOR_MIR_RAY, 0x18C0 },
    { ACTOR_BG_SPOT09_OBJ, 0x510 },
    { ACTOR_BG_SPOT18_OBJ, 0x8D0 },
    { ACTOR_BOSS_VA, 0x171F0 },
    { ACTOR_BG_HAKA_TUBO, 0xA20 },
    { ACTOR_BG_HAKA_TRAP, 0x15D0 },
    { ACTOR_BG_HAKA_HUTA, 0xAA0 },
    { ACTOR_BG_HAKA_ZOU, 0x11F0 },
    { ACTOR_BG_SPOT17_FUNEN, 0x250 },
    { ACTOR_EN_SYATEKI_ITM, 0xDA0 },
    { ACTOR_EN_SYATEKI_MAN, 0xDC0 },
    { ACTOR_EN_TANA, 0x2A0 },
    { ACTOR_EN_NB, 0x45D0 },
    { ACTOR_BOSS_MO, 0x100B0 },
    { ACTOR_EN_SB, 0x1440 },
    { ACTOR_EN_BIGOKUTA, 0x2B10 },
    { ACTOR_EN_KAREBABA, 0x18F0 },
    { ACTOR_BG_BDAN_OBJECTS, 0x12D0 },
    { ACTOR_DEMO_SA, 0x2B20 },
    { ACTOR_DEMO_GO, 0xD60 },
    { ACTOR_EN_IN, 0x2DA0 },
    { ACTOR_EN_TR, 0x1900 },
    { ACTOR_BG_SPOT16_BOMBSTONE, 0x1540 },
    { ACTOR_BG_HIDAN_KOWARERUKABE, 0xED0 },
    { ACTOR_BG_BOMBWALL, 0x8C0 },
    { ACTOR_BG_SPOT08_ICEBLOCK, 0x1040 },
    { ACTOR_EN_RU2, 0x2D80 },
    { ACTOR_OBJ_DEKUJR, 0x640 },
    { ACTOR_BG_MIZU_UZU, 0x1D0 },
    { ACTOR_BG_SPOT06_OBJECTS, 0x1410 },
    { ACTOR_BG_ICE_OBJECTS, 0xF40 },
    { ACTOR_BG_HAKA_WATER, 0x7E0 },
    { ACTOR_EN_MA2, 0x1060 },
    { ACTOR_EN_BOM_CHU, 0x16A0 },
    { ACTOR_EN_HORSE_GAME_CHECK, 0x10D0 },
    { ACTOR_BOSS_TW, 0x15B00 },
    { ACTOR_EN_RR, 0x2530 },
    { ACTOR_EN_BA, 0x1ED0 },
    { ACTOR_EN_BX, 0xAF0 },
    { ACTOR_EN_ANUBICE, 0x12B0 },
    { ACTOR_EN_ANUBICE_FIRE, 0xDC0 },
    { ACTOR_BG_MORI_HASHIGO, 0x8C0 },
    { ACTOR_BG_MORI_HASHIRA4, 0x590 },
    { ACTOR_BG_MORI_IDOMIZU, 0x640 },
    { ACTOR_BG_SPOT16_DOUGHNUT, 0x5B0 },
    { ACTOR_BG_BDAN_SWITCH, 0x1430 },
    { ACTOR_EN_MA1, 0x12E0 },
    { ACTOR_BOSS_GANON, 0x25DE0 },
    { ACTOR_BOSS_SST, 0xC5C0 },
    { ACTOR_EN_NY, 0x1930 },
    { ACTOR_EN_FR, 0x2A90 },
    { ACTOR_ITEM_SHIELD, 0xA10 },
    { ACTOR_BG_ICE_SHELTER, 0x1230 },
    { ACTOR_EN_ICE_HONO, 0x11F0 },
    { ACTOR_ITEM_OCARINA, 0x7D0 },
    { ACTOR_MAGIC_DARK, 0x1850 },
    { ACTOR_DEMO_6K, 0x2D10 },
    { ACTOR_EN_ANUBICE_TAG, 0x2D0 },
    { ACTOR_BG_HAKA_GATE, 0x1090 },
    { ACTOR_BG_SPOT15_SAKU, 0x340 },
    { ACTOR_BG_JYA_GOROIWA, 0x780 },
    { ACTOR_BG_JYA_ZURERUKABE, 0x6B0 },
    { ACTOR_BG_JYA_COBRA, 0x1D20 },
    { ACTOR_BG_JYA_KANAAMI, 0x3B0 },
    { ACTOR_FISHING, 0x1AAB0 },
    { ACTOR_OBJ_OSHIHIKI, 0x1AB0 },
    { ACTOR_BG_GATE_SHUTTER, 0x480 },
    { ACTOR_EFF_DUST, 0x13E0 },
    { ACTOR_BG_SPOT01_FUSYA, 0x2A0 },
    { ACTOR_BG_SPOT01_IDOHASHIRA, 0xC00 },
    { ACTOR_BG_SPOT01_IDOMIZU, 0x310 },
    { ACTOR_BG_PO_SYOKUDAI, 0x950 },
    { ACTOR_BG_GANON_OTYUKA, 0x2640 },
    { ACTOR_BG_SPOT15_RRBOX, 0xDE0 },
    { ACTOR_BG_UMAJUMP, 0x190 },
    { ACTOR_ARROW_FIRE, 0x1EC0 },
    { ACTOR_ARROW_ICE, 0x1EE0 },
    { ACTOR_ARROW_LIGHT, 0x1EF0 },
    { ACTOR_ITEM_ETCETERA, 0x8D0 },
    { ACTOR_OBJ_KIBAKO, 0xD00 },
    { ACTOR_OBJ_TSUBO, 0xFF0 },
    { ACTOR_EN_WONDER_ITEM, 0xD30 },
    { ACTOR_EN_IK, 0x4640 },
    { ACTOR_DEMO_IK, 0x1510 },
    { ACTOR_EN_SKJ, 0x3940 },
    { ACTOR_EN_SKJNEEDLE, 0x310 },
    { ACTOR_EN_G_SWITCH, 0x1830 },
    { ACTOR_DEMO_EXT, 0x940 },
    { ACTOR_DEMO_SHD, 0x2410 },
    { ACTOR_EN_DNS, 0x1390 },
    { ACTOR_ELF_MSG, 0x5F0 },
    { ACTOR_EN_HONOTRAP, 0x1550 },
    { ACTOR_EN_TUBO_TRAP, 0xCA0 },
    { ACTOR_OBJ_ICE_POLY, 0x9B0 },
    { ACTOR_BG_SPOT03_TAKI, 0x8F0 },
    { ACTOR_BG_SPOT07_TAKI, 0x5C0 },
    { ACTOR_EN_FZ, 0x2010 },
    { ACTOR_EN_PO_RELAY, 0x1710 },
    { ACTOR_BG_RELAY_OBJECTS, 0x7B0 },
    { ACTOR_EN_DIVING_GAME, 0x19B0 },
    { ACTOR_EN_KUSA, 0x14E0 },
    { ACTOR_OBJ_BEAN, 0x2790 },
    { ACTOR_OBJ_BOMBIWA, 0x570 },
    { ACTOR_OBJ_SWITCH, 0x1DC0 },
    { ACTOR_OBJ_ELEVATOR, 0x3C0 },
    { ACTOR_OBJ_LIFT, 0xA20 },
    { ACTOR_OBJ_HSBLOCK, 0x5D0 },
    { ACTOR_EN_OKARINA_TAG, 0x1500 },
    { ACTOR_EN_YABUSAME_MARK, 0x6D0 },
    { ACTOR_EN_GOROIWA, 0x23C0 },
    { ACTOR_EN_EX_RUPPY, 0x10C0 },
    { ACTOR_EN_TORYO, 0xC90 },
    { ACTOR_EN_DAIKU, 0x1740 },
    { ACTOR_EN_NWC, 0xA40 },
    { ACTOR_EN_BLKOBJ, 0x560 },
    { ACTOR_ITEM_INBOX, 0x160 },
    { ACTOR_EN_GE1, 0x2030 },
    { ACTOR_OBJ_BLOCKSTOP, 0x1A0 },
    { ACTOR_EN_SDA, 0x1700 },
    { ACTOR_EN_CLEAR_TAG, 0xB5A0 },
    { ACTOR_EN_NIW_LADY, 0x1900 },
    { ACTOR_EN_GM, 0xD30 },
    { ACTOR_EN_MS, 0x6F0 },
    { ACTOR_EN_HS, 0xBA0 },
    { ACTOR_BG_INGATE, 0x390 },
    { ACTOR_EN_KANBAN, 0x3150 },
    { ACTOR_EN_HEISHI3, 0x9D0 },
    { ACTOR_EN_SYATEKI_NIW, 0x2090 },
    { ACTOR_EN_ATTACK_NIW, 0x1260 },
    { ACTOR_BG_SPOT01_IDOSOKO, 0x210 },
    { ACTOR_EN_SA, 0x2270 },
    { ACTOR_EN_WONDER_TALK, 0x690 },
    { ACTOR_BG_GJYO_BRIDGE, 0x500 },
    { ACTOR_EN_DS, 0xC20 },
    { ACTOR_EN_MK, 0xE90 },
    { ACTOR_EN_BOM_BOWL_MAN, 0x1540 },
    { ACTOR_EN_BOM_BOWL_PIT, 0x970 },
    { ACTOR_EN_OWL, 0x3BA0 },
    { ACTOR_EN_ISHI, 0x9150 },
    { ACTOR_OBJ_HANA, 0x310 },
    { ACTOR_OBJ_LIGHTSWITCH, 0x1430 },
    { ACTOR_OBJ_MURE2, 0xA20 },
    { ACTOR_EN_GO, 0x4640 },
    { ACTOR_EN_FU, 0xD50 },
    { ACTOR_EN_CHANGER, 0x9E0 },
    { ACTOR_BG_JYA_MEGAMI, 0x11E0 },
    { ACTOR_BG_JYA_LIFT, 0x550 },
    { ACTOR_BG_JYA_BIGMIRROR, 0x840 },
    { ACTOR_BG_JYA_BOMBCHUIWA, 0xB30 },
    { ACTOR_BG_JYA_AMISHUTTER, 0x390 },
    { ACTOR_BG_JYA_BOMBIWA, 0x5C0 },
    { ACTOR_BG_SPOT18_BASKET, 0xFF0 },
    { ACTOR_EN_GANON_ORGAN, 0x7000 },
    { ACTOR_EN_SIOFUKI, 0xDB0 },
    { ACTOR_EN_STREAM, 0x590 },
    { ACTOR_EN_MM, 0x1620 },
    { ACTOR_EN_KO, 0x4140 },
    { ACTOR_EN_KZ, 0x15A0 },
    { ACTOR_EN_WEATHER_TAG, 0xEF0 },
    { ACTOR_BG_SST_FLOOR, 0x560 },
    { ACTOR_EN_ANI, 0xD70 },
    { ACTOR_EN_EX_ITEM, 0x1170 },
    { ACTOR_BG_JYA_IRONOBJ, 0xDB0 },
    { ACTOR_EN_JS, 0x9D0 },
    { ACTOR_EN_JSJUTAN, 0x5920 },
    { ACTOR_EN_CS, 0x1230 },
    { ACTOR_EN_MD, 0x2670 },
    { ACTOR_EN_HY, 0x3940 },
    { ACTOR_EN_GANON_MANT, 0x4220 },
    { ACTOR_EN_OKARINA_EFFECT, 0x3C0 },
    { ACTOR_EN_MAG, 0x4F10 },
    { ACTOR_DOOR_GERUDO, 0x5F0 },
    { ACTOR_ELF_MSG2, 0x470 },
    { ACTOR_DEMO_GT, 0x5600 },
    { ACTOR_EN_PO_FIELD, 0x3A70 },
    { ACTOR_EFC_ERUPC, 0xAE0 },
    { ACTOR_BG_ZG, 0x470 },
    { ACTOR_EN_HEISHI4, 0xF00 },
    { ACTOR_EN_ZL3, 0x7E50 },
    { ACTOR_BOSS_GANON2, 0x12E20 },
    { ACTOR_EN_KAKASI, 0xD40 },
    { ACTOR_EN_TAKARA_MAN, 0x8C0 },
    { ACTOR_OBJ_MAKEOSHIHIKI, 0x490 },
    { ACTOR_OCEFF_SPOT, 0xF30 },
    { ACTOR_END_TITLE, 0x4130 },
    { ACTOR_EN_TORCH, 0xF0 },
    { ACTOR_DEMO_EC, 0x3860 },
    { ACTOR_SHOT_SUN, 0x6C0 },
    { ACTOR_EN_DY_EXTRA, 0x580 },
    { ACTOR_EN_WONDER_TALK2, 0x6A0 },
    { ACTOR_EN_GE2, 0x19A0 },
    { ACTOR_OBJ_ROOMTIMER, 0x250 },
    { ACTOR_EN_SSH, 0x25F0 },
    { ACTOR_EN_STH, 0x40B0 },
    { ACTOR_OCEFF_WIPE, 0xD50 },
    { ACTOR_OCEFF_STORM, 0x1BA0 },
    { ACTOR_EN_WEIYER, 0x1A00 },
    { ACTOR_BG_SPOT05_SOKO, 0x320 },
    { ACTOR_BG_JYA_1FLIFT, 0x690 },
    { ACTOR_BG_JYA_HAHENIRON, 0x7F0 },
    { ACTOR_BG_SPOT12_GATE, 0x410 },
    { ACTOR_BG_SPOT12_SAKU, 0x4C0 },
    { ACTOR_EN_HINTNUTS, 0x1A30 },
    { ACTOR_EN_NUTSBALL, 0x620 },
    { ACTOR_BG_SPOT00_BREAK, 0x1A0 },
    { ACTOR_EN_SHOPNUTS, 0xF10 },
    { ACTOR_EN_IT, 0x190 },
    { ACTOR_EN_GELDB, 0x53B0 },
    { ACTOR_OCEFF_WIPE2, 0x1770 },
    { ACTOR_OCEFF_WIPE3, 0x1750 },
    { ACTOR_EN_NIW_GIRL, 0xAD0 },
    { ACTOR_EN_DOG, 0x11B0 },
    { ACTOR_EN_SI, 0x500 },
    { ACTOR_BG_SPOT01_OBJECTS2, 0x4C0 },
    { ACTOR_OBJ_COMB, 0x860 },
    { ACTOR_BG_SPOT11_BAKUDANKABE, 0x640 },
    { ACTOR_OBJ_KIBAKO2, 0x6C0 },
    { ACTOR_EN_DNT_DEMO, 0xD20 },
    { ACTOR_EN_DNT_JIJI, 0x1510 },
    { ACTOR_EN_DNT_NOMAL, 0x2E10 },
    { ACTOR_EN_GUEST, 0x9A0 },
    { ACTOR_BG_BOM_GUARD, 0x220 },
    { ACTOR_EN_HS2, 0x5E0 },
    { ACTOR_DEMO_KEKKAI, 0x12E0 },
    { ACTOR_BG_SPOT08_BAKUDANKABE, 0x6A0 },
    { ACTOR_BG_SPOT17_BAKUDANKABE, 0x6E0 },
    { ACTOR_OBJ_MURE3, 0x7D0 },
    { ACTOR_EN_TG, 0x6D0 },
    { ACTOR_EN_MU, 0x920 },
    { ACTOR_EN_GO2, 0x6020 },
    { ACTOR_EN_WF, 0x4310 },
    { ACTOR_EN_SKB, 0x18F0 },
    { ACTOR_DEMO_GJ, 0x3CB0 },
    { ACTOR_DEMO_GEFF, 0x820 },
    { ACTOR_BG_GND_FIREMEIRO, 0x540 },
    { ACTOR_BG_GND_DARKMEIRO, 0x7C0 },
    { ACTOR_BG_GND_SOULMEIRO, 0x860 },
    { ACTOR_BG_GND_NISEKABE, 0x170 },
    { ACTOR_BG_GND_ICEBLOCK, 0x1100 },
    { ACTOR_EN_GB, 0x1730 },
    { ACTOR_EN_GS, 0x1EA0 },
    { ACTOR_BG_MIZU_BWALL, 0x14D0 },
    { ACTOR_BG_MIZU_SHUTTER, 0x800 },
    { ACTOR_EN_DAIKU_KAKARIKO, 0x13C0 },
    { ACTOR_BG_BOWL_WALL, 0x980 },
    { ACTOR_EN_WALL_TUBO, 0x4F0 },
    { ACTOR_EN_PO_DESERT, 0xDC0 },
    { ACTOR_EN_CROW, 0x16A0 },
    { ACTOR_DOOR_KILLER, 0x1570 },
    { ACTOR_BG_SPOT11_OASIS, 0x730 },
    { ACTOR_BG_SPOT18_FUTA, 0x1A0 },
    { ACTOR_BG_SPOT18_SHUTTER, 0x550 },
    { ACTOR_EN_MA3, 0xFB0 },
    { ACTOR_EN_COW, 0x1460 },
    { ACTOR_BG_ICE_TURARA, 0x830 },
    { ACTOR_BG_ICE_SHUTTER, 0x470 },
    { ACTOR_EN_KAKASI2, 0x720 },
    { ACTOR_EN_KAKASI3, 0x10E0 },
    { ACTOR_OCEFF_WIPE4, 0xFE0 },
    { ACTOR_EN_EG, 0x1B0 },
    { ACTOR_BG_MENKURI_NISEKABE, 0x150 },
    { ACTOR_EN_ZO, 0x25B0 },
    { ACTOR_OBJ_MAKEKINSUTA, 0x150 },
    { ACTOR_EN_GE3, 0xB50 },
    { ACTOR_OBJ_TIMEBLOCK, 0xC40 },
    { ACTOR_OBJ_HAMISHI, 0x850 },
    { ACTOR_EN_ZL4, 0x4A30 },
    { ACTOR_EN_MM2, 0xDC0 },
    { ACTOR_BG_JYA_BLOCK, 0x270 },
    { ACTOR_OBJ_WARP2BLOCK, 0xB30 },
};

// effect ss overlay sizes from an NTSC 1.2 rom; N64 loads these into the
// tail of the heap on first spawn of each type and keeps them until the
// play state ends. SoH compiles the effects in, so mirror them here.
static const std::unordered_map<s32, size_t> effectSsOverlaySizes = {
    { EFFECT_SS_DUST, 0x830 },      { EFFECT_SS_KIRAKIRA, 0x670 },   { EFFECT_SS_BOMB, 0x420 },
    { EFFECT_SS_BOMB2, 0x930 },     { EFFECT_SS_BLAST, 0x390 },      { EFFECT_SS_G_SPK, 0x5B0 },
    { EFFECT_SS_D_FIRE, 0x4F0 },    { EFFECT_SS_BUBBLE, 0x480 },     { EFFECT_SS_G_RIPPLE, 0x560 },
    { EFFECT_SS_G_SPLASH, 0x4B0 },  { EFFECT_SS_G_MAGMA, 0x260 },    { EFFECT_SS_G_FIRE, 0x290 },
    { EFFECT_SS_LIGHTNING, 0x6D0 }, { EFFECT_SS_DT_BUBBLE, 0x590 },  { EFFECT_SS_HAHEN, 0x640 },
    { EFFECT_SS_STICK, 0x3A0 },     { EFFECT_SS_SIBUKI, 0x6D0 },     { EFFECT_SS_SIBUKI2, 0x330 },
    { EFFECT_SS_G_MAGMA2, 0x510 },  { EFFECT_SS_STONE1, 0x390 },     { EFFECT_SS_HITMARK, 0x550 },
    { EFFECT_SS_FHG_FLASH, 0xF80 }, { EFFECT_SS_K_FIRE, 0x430 },     { EFFECT_SS_SOLDER_SRCH_BALL, 0x1B0 },
    { EFFECT_SS_KAKERA, 0x1090 },   { EFFECT_SS_ICE_PIECE, 0x440 },  { EFFECT_SS_EN_ICE, 0x8C0 },
    { EFFECT_SS_FIRE_TAIL, 0x700 }, { EFFECT_SS_EN_FIRE, 0x740 },    { EFFECT_SS_EXTRA, 0x3C0 },
    { EFFECT_SS_FCIRCLE, 0x4B0 },   { EFFECT_SS_DEAD_DB, 0x4E0 },    { EFFECT_SS_DEAD_DD, 0x590 },
    { EFFECT_SS_DEAD_DS, 0x480 },   { EFFECT_SS_DEAD_SOUND, 0x140 }, { EFFECT_SS_ICE_SMOKE, 0x4C0 },
};

// skybox texture buffers (staticSegments + palettes) N64 game-allocs in
// Skybox_Setup but SoH loads as resources; sizes from an NTSC 1.2 rom.
// Ids missing here (SKYBOX_NONE etc.) alloc nothing.
static const std::unordered_map<s16, size_t> skyboxAllocSizes = {
    { SKYBOX_NORMAL_SKY, 0x18200 },           { SKYBOX_BAZAAR, 0x20400 },
    { SKYBOX_OVERCAST_SUNSET, 0x18200 },      { SKYBOX_MARKET_ADULT, 0x40800 },
    { SKYBOX_CUTSCENE_MAP, 0x20200 },         { SKYBOX_HOUSE_LINK, 0x40800 },
    { SKYBOX_MARKET_CHILD_DAY, 0x40800 },     { SKYBOX_MARKET_CHILD_NIGHT, 0x40800 },
    { SKYBOX_HAPPY_MASK_SHOP, 0x20400 },      { SKYBOX_HOUSE_KNOW_IT_ALL_BROTHERS, 0x40800 },
    { SKYBOX_HOUSE_OF_TWINS, 0x30600 },       { SKYBOX_STABLES, 0x40800 },
    { SKYBOX_HOUSE_KAKARIKO, 0x40800 },       { SKYBOX_KOKIRI_SHOP, 0x20400 },
    { SKYBOX_GORON_SHOP, 0x20400 },           { SKYBOX_ZORA_SHOP, 0x20400 },
    { SKYBOX_POTION_SHOP_KAKARIKO, 0x20400 }, { SKYBOX_POTION_SHOP_MARKET, 0x20400 },
    { SKYBOX_BOMBCHU_SHOP, 0x20400 },         { SKYBOX_HOUSE_RICHARD, 0x40800 },
    { SKYBOX_HOUSE_IMPA, 0x40800 },           { SKYBOX_TENT, 0x30600 },
    { SKYBOX_HOUSE_MIDO, 0x30600 },           { SKYBOX_HOUSE_SARIA, 0x30600 },
    { SKYBOX_HOUSE_ALLEY, 0x30600 },
};

static bool HeapSim_IsInitialized() {
    return __osMallocIsInitialized(&sHeapSimArena);
}

extern "C" void HeapSim_BeginSimulation(void) {
    if (HeapSim_IsInitialized()) {
        // shouldn't happen: Play_Destroy cleans up before next Play_Init
        HeapSim_Cleanup();
    }
    if (!CVarGetInteger(CVAR_ENHANCEMENT("SimulateN64Heap"), 0)) {
        return;
    }
    heapSimAllocSize = kHeapSimSize;
    trackingGameAllocs = true;
}

static void HeapSim_DeductGameAlloc(size_t size) {
    if (size >= heapSimAllocSize) {
        heapSimAllocSize = 0;
    } else {
        heapSimAllocSize -= size;
    }
}

extern "C" void HeapSim_MirrorGameAlloc(void* ptr, size_t size) {
    if (!trackingGameAllocs || ptr == NULL) {
        return;
    }
    HeapSim_DeductGameAlloc(size);
}

// Called from MapMark_Init; N64 game-allocs the map mark data overlay there
extern "C" void HeapSim_MirrorMapMarkLoad(void) {
    if (!trackingGameAllocs) {
        return;
    }
    HeapSim_DeductGameAlloc(kMapMarkOvlSize);
}

extern "C" void HeapSim_InitArena(GameState* gameState) {
    if (!trackingGameAllocs) {
        return;
    }
    trackingGameAllocs = false;
    // skybox texture buffers N64 would have game-alloc'd during Play_Init
    auto sky = skyboxAllocSizes.find(((PlayState*)gameState)->skyboxId);
    if (sky != skyboxAllocSizes.end()) {
        HeapSim_DeductGameAlloc(sky->second);
    }
    uintptr_t alloc = (uintptr_t)GameState_Alloc(gameState, heapSimAllocSize, (char*)__FILE__, __LINE__);
    if (alloc == 0) {
        SPDLOG_WARN("heap sim: failed to reserve {:x} bytes for shadow heap", (u32)heapSimAllocSize);
        return;
    }
    uintptr_t aligned = (alloc + 8) & ~0xF;
    __osMallocInit(&sHeapSimArena, (void*)aligned, heapSimAllocSize - (aligned - alloc));
    SPDLOG_INFO("heap sim: simulated zelda heap size={:06x} (game allocs deducted {:06x} from {:06x})",
                (u32)heapSimAllocSize, (u32)(kHeapSimSize - heapSimAllocSize), (u32)kHeapSimSize);
}

extern "C" void HeapSim_Cleanup(void) {
    __osMallocCleanup(&sHeapSimArena);
    mirrorMap.clear();
    overlayPtrMap.clear();
    registeredOverlays.clear();
    registeredEffectOvls.clear();
    absolutePtr = nullptr;
    trackingGameAllocs = false;
    heapSimAllocSize = kHeapSimSize;
}

// N64 loads each effect ss overlay into the heap tail on first spawn and
// keeps it for the rest of the play state
extern "C" s32 HeapSim_MirrorEffectSsOverlay(s32 type) {
    if (!HeapSim_IsInitialized() || registeredEffectOvls.contains(type)) {
        return true;
    }
    auto it = effectSsOverlaySizes.find(type);
    if (it == effectSsOverlaySizes.end()) {
        // custom effect: no overlay to load
        return true;
    }
    void* shadow = __osMallocR(&sHeapSimArena, it->second);
    if (shadow == NULL) {
        SPDLOG_WARN("heap sim: effect ss overlay {} ({:#x} bytes) would not have loaded on N64", type, it->second);
        return false;
    }
    registeredEffectOvls.insert(type);
    return true;
}

extern "C" s32 HeapSim_MirrorMalloc(void* ptr, size_t size) {
    if (!HeapSim_IsInitialized() || ptr == NULL) {
        return true;
    }
    void* shadow = __osMalloc(&sHeapSimArena, size);
    if (shadow == NULL) {
        return false;
    }
    mirrorMap[ptr] = shadow;
    return true;
}

// MallocR allocates from tail end of arena instead of head.
// Long-lived allocs go there so they don't fragment head.
extern "C" s32 HeapSim_MirrorMallocR(void* ptr, size_t size) {
    if (!HeapSim_IsInitialized() || ptr == NULL) {
        return true;
    }
    void* shadow = __osMallocR(&sHeapSimArena, size);
    if (shadow == NULL) {
        return false;
    }
    mirrorMap[ptr] = shadow;
    return true;
}

extern "C" void HeapSim_MirrorFree(void* ptr) {
    if (!HeapSim_IsInitialized() || ptr == NULL) {
        return;
    }
    auto it = mirrorMap.find(ptr);
    if (it == mirrorMap.end()) {
        return;
    }
    __osFree(&sHeapSimArena, it->second);
    mirrorMap.erase(it);
}

extern "C" s32 HeapSim_RegisterActorOverlayIfNeeded(ActorDBEntry* dbEntry) {
    if (!HeapSim_IsInitialized()) {
        return true;
    }
    u16 id = dbEntry->id;
    if (registeredOverlays.contains(id)) {
        return true;
    }
    auto typeIt = stairActorAllocTypes.find(id);
    auto sizeIt = actorOverlaySizes.find(id);
    if (typeIt == stairActorAllocTypes.end() || sizeIt == actorOverlaySizes.end() || sizeIt->second == 0) {
        // internal or custom actor: no overlay to load
        registeredOverlays.insert(id);
        return true;
    }
    u16 allocType = typeIt->second;
    if (allocType == ALLOCTYPE_ABSOLUTE) {
        if (absolutePtr == NULL) {
            absolutePtr = __osMallocR(&sHeapSimArena, HEAPSIM_ACTOROVL_ABSOLUTE_SPACE_SIZE);
            if (absolutePtr == NULL) {
                return false;
            }
        }
        // loads into the shared absolute space, nothing more to allocate
    } else {
        void* shadow = allocType == ALLOCTYPE_PERMANENT ? __osMallocR(&sHeapSimArena, sizeIt->second)
                                                        : __osMalloc(&sHeapSimArena, sizeIt->second);
        if (shadow == NULL) {
            return false;
        }
        overlayPtrMap[id] = shadow;
    }
    registeredOverlays.insert(id);
    return true;
}

extern "C" void HeapSim_UnregisterActorOverlayIfNeeded(ActorDBEntry* dbEntry) {
    if (!HeapSim_IsInitialized()) {
        return;
    }
    u16 id = dbEntry->id;
    if (!registeredOverlays.contains(id)) {
        return;
    }
    auto typeIt = stairActorAllocTypes.find(id);
    u16 allocType = typeIt != stairActorAllocTypes.end() ? typeIt->second : (u16)ALLOCTYPE_NORMAL;
    if (allocType == ALLOCTYPE_PERMANENT) {
        // permanent overlays stay loaded
        return;
    }
    if (allocType == ALLOCTYPE_NORMAL) {
        auto it = overlayPtrMap.find(id);
        if (it != overlayPtrMap.end()) {
            __osFree(&sHeapSimArena, it->second);
            overlayPtrMap.erase(it);
        }
    }
    // absolute: shared space is freed separately in HeapSim_FreeAbsoluteSpace
    registeredOverlays.erase(id);
}

extern "C" void HeapSim_FreeAbsoluteSpace(void) {
    if (!HeapSim_IsInitialized() || absolutePtr == NULL) {
        return;
    }
    __osFree(&sHeapSimArena, absolutePtr);
    absolutePtr = nullptr;
}
