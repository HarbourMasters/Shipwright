#include "stairs.h"
#include <string.h>

struct ActorIdAllocTypePair {
    u16 id;
    u16 allocType;
};

#define DEFINE_ACTOR_INTERNAL(name, id, allocType) { id, allocType },
#define DEFINE_ACTOR(name, id, allocType) { id, allocType },
#define DEFINE_ACTOR_UNSET(_0)

static const std::vector<ActorIdAllocTypePair> stairActorTable = {
#include "tables/actor_table.h"
};

#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET
#undef DEFINE_ACTOR

size_t stairsAllocSize = 0x1D4790 - 0x26960 - 0x2200 - (0x55 * 0x60) - (3 * (48 * 16) / 2) - (4 * (32 * 32 * 4)) - 0x1000 - (20 * sizeof(MtxF));
Arena sStairsArena;

// using sizes from the debug rom
std::unordered_map<u16, size_t> actorOverlaySizes = {
    { ACTOR_PLAYER,                0x0},
    { ACTOR_EN_TEST,               0x58B0},
    { ACTOR_EN_GIRLA,              0x2F90},
    { ACTOR_EN_PART,               0x16D0},
    { ACTOR_EN_LIGHT,              0xE90},
    { ACTOR_EN_DOOR,               0xE60},
    { ACTOR_EN_BOX,                0x1D50},
    { ACTOR_BG_DY_YOSEIZO,         0x30F0},
    { ACTOR_BG_HIDAN_FIREWALL,     0x7E0},
    { ACTOR_EN_POH,                0x4370},
    { ACTOR_EN_OKUTA,              0x2640},
    { ACTOR_BG_YDAN_SP,            0x1810},
    { ACTOR_EN_BOM,                0xF60},
    { ACTOR_EN_WALLMAS,            0x1B00},
    { ACTOR_EN_DODONGO,            0x2D80},
    { ACTOR_EN_FIREFLY,            0x2210},
    { ACTOR_EN_HORSE,              0xC260},
    { ACTOR_EN_ITEM00,             0x0},
    { ACTOR_EN_ARROW,              0x17E0},
    { ACTOR_EN_ELF,                0x4AF0},
    { ACTOR_EN_NIW,                0x3800},
    { ACTOR_EN_TITE,               0x2E80},
    { ACTOR_EN_REEBA,              0x1C60},
    { ACTOR_EN_PEEHAT,             0x3810},
    { ACTOR_EN_BUTTE,              0x1670},
    { ACTOR_EN_INSECT,             0x2640},
    { ACTOR_EN_FISH,               0x2290},
    { ACTOR_EN_HOLL,               0x1020},
    { ACTOR_EN_SCENE_CHANGE,       0x1B0},
    { ACTOR_EN_ZF,                 0x6AF0},
    { ACTOR_EN_HATA,               0x580},
    { ACTOR_BOSS_DODONGO,          0x9C30},
    { ACTOR_BOSS_GOMA,             0x60F0},
    { ACTOR_EN_ZL1,                0x3E50},
    { ACTOR_EN_VIEWER,             0x3260},
    { ACTOR_EN_GOMA,               0x2DE0},
    { ACTOR_BG_PUSHBOX,            0x370},
    { ACTOR_EN_BUBBLE,             0x1470},
    { ACTOR_DOOR_SHUTTER,          0x2380},
    { ACTOR_EN_DODOJR,             0x1E90},
    { ACTOR_EN_BDFIRE,             0xC30},
    { ACTOR_EN_BOOM,               0x940},
    { ACTOR_EN_TORCH2,             0x27F0},
    { ACTOR_EN_BILI,               0x2310},
    { ACTOR_EN_TP,                 0x1F10},
    { ACTOR_EN_ST,                 0x2CB0},
    { ACTOR_EN_BW,                 0x3400},
    { ACTOR_EN_A_OBJ,              0x0},
    { ACTOR_EN_EIYER,              0x1C90},
    { ACTOR_EN_RIVER_SOUND,        0x980},
    { ACTOR_EN_HORSE_NORMAL,       0x2680},
    { ACTOR_EN_OSSAN,              0x7280},
    { ACTOR_BG_TREEMOUTH,          0x16E0},
    { ACTOR_BG_DODOAGO,            0xE30},
    { ACTOR_BG_HIDAN_DALM,         0x840},
    { ACTOR_BG_HIDAN_HROCK,        0x830},
    { ACTOR_EN_HORSE_GANON,        0xD70},
    { ACTOR_BG_HIDAN_ROCK,         0x1180},
    { ACTOR_BG_HIDAN_RSEKIZOU,     0xCC0},
    { ACTOR_BG_HIDAN_SEKIZOU,      0x1510},
    { ACTOR_BG_HIDAN_SIMA,         0xFF0},
    { ACTOR_BG_HIDAN_SYOKU,        0x450},
    { ACTOR_EN_XC,                 0x6BE0},
    { ACTOR_BG_HIDAN_CURTAIN,      0xC80},
    { ACTOR_BG_SPOT00_HANEBASI,    0x1240},
    { ACTOR_EN_MB,                 0x4200},
    { ACTOR_EN_BOMBF,              0x1520},
    { ACTOR_EN_ZL2,                0x4A20},
    { ACTOR_BG_HIDAN_FSLIFT,       0x4B0},
    { ACTOR_EN_OE2,                0xC0},
    { ACTOR_BG_YDAN_HASI,          0x840},
    { ACTOR_BG_YDAN_MARUTA,        0x6D0},
    { ACTOR_BOSS_GANONDROF,        0x53D0},
    { ACTOR_EN_AM,                 0x2450},
    { ACTOR_EN_DEKUBABA,           0x3D90},
    { ACTOR_EN_M_FIRE1,            0x1A0},
    { ACTOR_EN_M_THUNDER,          0x16B0},
    { ACTOR_BG_DDAN_JD,            0x640},
    { ACTOR_BG_BREAKWALL,          0xED0},
    { ACTOR_EN_JJ,                 0x1610},
    { ACTOR_EN_HORSE_ZELDA,        0xAD0},
    { ACTOR_BG_DDAN_KD,            0x910},
    { ACTOR_DOOR_WARP1,            0x4530},
    { ACTOR_OBJ_SYOKUDAI,          0xCF0},
    { ACTOR_ITEM_B_HEART,          0x4A0},
    { ACTOR_EN_DEKUNUTS,           0x17E0},
    { ACTOR_BG_MENKURI_KAITEN,     0x180},
    { ACTOR_BG_MENKURI_EYE,        0x540},
    { ACTOR_EN_VALI,               0x2800},
    { ACTOR_BG_MIZU_MOVEBG,        0x1220},
    { ACTOR_BG_MIZU_WATER,         0xED0},
    { ACTOR_ARMS_HOOK,             0xE10},
    { ACTOR_EN_FHG,                0x2D60},
    { ACTOR_BG_MORI_HINERI,        0xE70},
    { ACTOR_EN_BB,                 0x3D50},
    { ACTOR_BG_TOKI_HIKARI,        0xF40},
    { ACTOR_EN_YUKABYUN,           0x690},
    { ACTOR_BG_TOKI_SWD,           0x16C0},
    { ACTOR_EN_FHG_FIRE,           0x29C0},
    { ACTOR_BG_MJIN,               0x440},
    { ACTOR_BG_HIDAN_KOUSI,        0x6C0},
    { ACTOR_DOOR_TOKI,             0x160},
    { ACTOR_BG_HIDAN_HAMSTEP,      0x10F0},
    { ACTOR_EN_BIRD,               0x4B0},
    { ACTOR_EN_WOOD02,             0x1270},
    { ACTOR_EN_LIGHTBOX,           0x470},
    { ACTOR_EN_PU_BOX,             0x340},
    { ACTOR_EN_TRAP,               0x1290},
    { ACTOR_EN_AROW_TRAP,          0x150},
    { ACTOR_EN_VASE,               0xF0},
    { ACTOR_EN_TA,                 0x3AC0},
    { ACTOR_EN_TK,                 0x1F90},
    { ACTOR_BG_MORI_BIGST,         0xC40},
    { ACTOR_BG_MORI_ELEVATOR,      0xD60},
    { ACTOR_BG_MORI_KAITENKABE,    0x7B0},
    { ACTOR_BG_MORI_RAKKATENJO,    0xCA0},
    { ACTOR_EN_VM,                 0x1990},
    { ACTOR_DEMO_EFFECT,           0x6440},
    { ACTOR_DEMO_KANKYO,           0x4150},
    { ACTOR_BG_HIDAN_FWBIG,        0xD50},
    { ACTOR_EN_FLOORMAS,           0x3490},
    { ACTOR_EN_HEISHI1,            0x1BD0},
    { ACTOR_EN_RD,                 0x28D0},
    { ACTOR_EN_PO_SISTERS,         0x4DC0},
    { ACTOR_BG_HEAVY_BLOCK,        0x1A10},
    { ACTOR_BG_PO_EVENT,           0x1EA0},
    { ACTOR_OBJ_MURE,              0x14A0},
    { ACTOR_EN_SW,                 0x3900},
    { ACTOR_BOSS_FD,               0x78B0},
    { ACTOR_OBJECT_KANKYO,         0x35D0},
    { ACTOR_EN_DU,                 0x1AC0},
    { ACTOR_EN_FD,                 0x2DF0},
    { ACTOR_EN_HORSE_LINK_CHILD,   0x1E80},
    { ACTOR_DOOR_ANA,              0x6F0},
    { ACTOR_BG_SPOT02_OBJECTS,     0x1480},
    { ACTOR_BG_HAKA,               0x740},
    { ACTOR_MAGIC_WIND,            0x1F50},
    { ACTOR_MAGIC_FIRE,            0x2390},
    { ACTOR_EN_RU1,                0x7940},
    { ACTOR_BOSS_FD2,              0x4200},
    { ACTOR_EN_FD_FIRE,            0xDB0},
    { ACTOR_EN_DH,                 0x1B40},
    { ACTOR_EN_DHA,                0xFF0},
    { ACTOR_EN_RL,                 0x1190},
    { ACTOR_EN_ENCOUNT1,           0x1270},
    { ACTOR_DEMO_DU,               0x39E0},
    { ACTOR_DEMO_IM,               0x42A0},
    { ACTOR_DEMO_TRE_LGT,          0x830},
    { ACTOR_EN_FW,                 0x1820},
    { ACTOR_BG_VB_SIMA,            0x7A0},
    { ACTOR_EN_VB_BALL,            0x1260},
    { ACTOR_BG_HAKA_MEGANE,        0x3F0},
    { ACTOR_BG_HAKA_MEGANEBG,      0x6B0},
    { ACTOR_BG_HAKA_SHIP,          0xBA0},
    { ACTOR_BG_HAKA_SGAMI,         0xC20},
    { ACTOR_EN_HEISHI2,            0x27A0},
    { ACTOR_EN_ENCOUNT2,           0x1510},
    { ACTOR_EN_FIRE_ROCK,          0x1450},
    { ACTOR_EN_BROB,               0x10F0},
    { ACTOR_MIR_RAY,               0x1A30},
    { ACTOR_BG_SPOT09_OBJ,         0x6A0},
    { ACTOR_BG_SPOT18_OBJ,         0xAC0},
    { ACTOR_BOSS_VA,               0x17690},
    { ACTOR_BG_HAKA_TUBO,          0xAB0},
    { ACTOR_BG_HAKA_TRAP,          0x15B0},
    { ACTOR_BG_HAKA_HUTA,          0xA80},
    { ACTOR_BG_HAKA_ZOU,           0x11D0},
    { ACTOR_BG_SPOT17_FUNEN,       0x310},
    { ACTOR_EN_SYATEKI_ITM,        0x11C0},
    { ACTOR_EN_SYATEKI_MAN,        0x1080},
    { ACTOR_EN_TANA,               0x430},
    { ACTOR_EN_NB,                 0x49F0},
    { ACTOR_BOSS_MO,               0x10AD0},
    { ACTOR_EN_SB,                 0x1480},
    { ACTOR_EN_BIGOKUTA,           0x2C40},
    { ACTOR_EN_KAREBABA,           0x1A60},
    { ACTOR_BG_BDAN_OBJECTS,       0x1370},
    { ACTOR_DEMO_SA,               0x2DD0},
    { ACTOR_DEMO_GO,               0xE00},
    { ACTOR_EN_IN,                 0x2E70},
    { ACTOR_EN_TR,                 0x1970},
    { ACTOR_BG_SPOT16_BOMBSTONE,   0x1810},
    { ACTOR_BG_HIDAN_KOWARERUKABE, 0x1060},
    { ACTOR_BG_BOMBWALL,           0x990},
    { ACTOR_BG_SPOT08_ICEBLOCK,    0x1180},
    { ACTOR_EN_RU2,                0x3010},
    { ACTOR_OBJ_DEKUJR,            0x6E0},
    { ACTOR_BG_MIZU_UZU,           0x1D0},
    { ACTOR_BG_SPOT06_OBJECTS,     0x14B0},
    { ACTOR_BG_ICE_OBJECTS,        0xF30},
    { ACTOR_BG_HAKA_WATER,         0x8C0},
    { ACTOR_EN_MA2,                0x1100},
    { ACTOR_EN_BOM_CHU,            0x1730},
    { ACTOR_EN_HORSE_GAME_CHECK,   0x1110},
    { ACTOR_BOSS_TW,               0x165F0},
    { ACTOR_EN_RR,                 0x27D0},
    { ACTOR_EN_BA,                 0x1F90},
    { ACTOR_EN_BX,                 0xBB0},
    { ACTOR_EN_ANUBICE,            0x1480},
    { ACTOR_EN_ANUBICE_FIRE,       0xE60},
    { ACTOR_BG_MORI_HASHIGO,       0xB00},
    { ACTOR_BG_MORI_HASHIRA4,      0x770},
    { ACTOR_BG_MORI_IDOMIZU,       0x780},
    { ACTOR_BG_SPOT16_DOUGHNUT,    0x780},
    { ACTOR_BG_BDAN_SWITCH,        0x1540},
    { ACTOR_EN_MA1,                0x1310},
    { ACTOR_BOSS_GANON,            0x266D0},
    { ACTOR_BOSS_SST,              0xC700},
    { ACTOR_EN_NY,                 0x1AF0},
    { ACTOR_EN_FR,                 0x2E70},
    { ACTOR_ITEM_SHIELD,           0xAB0},
    { ACTOR_BG_ICE_SHELTER,        0x1380},
    { ACTOR_EN_ICE_HONO,           0x1320},
    { ACTOR_ITEM_OCARINA,          0x7F0},
    { ACTOR_MAGIC_DARK,            0x19A0},
    { ACTOR_DEMO_6K,               0x3180},
    { ACTOR_EN_ANUBICE_TAG,        0x3D0},
    { ACTOR_BG_HAKA_GATE,          0x11D0},
    { ACTOR_BG_SPOT15_SAKU,        0x3E0},
    { ACTOR_BG_JYA_GOROIWA,        0x750},
    { ACTOR_BG_JYA_ZURERUKABE,     0x830},
    { ACTOR_BG_JYA_COBRA,          0x2080},
    { ACTOR_BG_JYA_KANAAMI,        0x460},
    { ACTOR_FISHING,               0x1B7B0},
    { ACTOR_OBJ_OSHIHIKI,          0x1E50},
    { ACTOR_BG_GATE_SHUTTER,       0x560},
    { ACTOR_EFF_DUST,              0x14E0},
    { ACTOR_BG_SPOT01_FUSYA,       0x320},
    { ACTOR_BG_SPOT01_IDOHASHIRA,  0xE00},
    { ACTOR_BG_SPOT01_IDOMIZU,     0x3A0},
    { ACTOR_BG_PO_SYOKUDAI,        0xA30},
    { ACTOR_BG_GANON_OTYUKA,       0x2840},
    { ACTOR_BG_SPOT15_RRBOX,       0xEF0},
    { ACTOR_BG_UMAJUMP,            0x180},
    { ACTOR_ARROW_FIRE,            0x1FC0},
    { ACTOR_ARROW_ICE,             0x1FE0},
    { ACTOR_ARROW_LIGHT,           0x1FF0},
    { ACTOR_ITEM_ETCETERA,         0x9F0},
    { ACTOR_OBJ_KIBAKO,            0xD30},
    { ACTOR_OBJ_TSUBO,             0x1090},
    { ACTOR_EN_WONDER_ITEM,        0x1090},
    { ACTOR_EN_IK,                 0x4CA0},
    { ACTOR_DEMO_IK,               0x18E0},
    { ACTOR_EN_SKJ,                0x3C00},
    { ACTOR_EN_SKJNEEDLE,          0x3C0},
    { ACTOR_EN_G_SWITCH,           0x1EA0},
    { ACTOR_DEMO_EXT,              0xAB0},
    { ACTOR_DEMO_SHD,              0x2490},
    { ACTOR_EN_DNS,                0x1590},
    { ACTOR_ELF_MSG,               0xBE0},
    { ACTOR_EN_HONOTRAP,           0x16C0},
    { ACTOR_EN_TUBO_TRAP,          0xDC0},
    { ACTOR_OBJ_ICE_POLY,          0xA40},
    { ACTOR_BG_SPOT03_TAKI,        0x990},
    { ACTOR_BG_SPOT07_TAKI,        0x6A0},
    { ACTOR_EN_FZ,                 0x2100},
    { ACTOR_EN_PO_RELAY,           0x1850},
    { ACTOR_BG_RELAY_OBJECTS,      0x790},
    { ACTOR_EN_DIVING_GAME,        0x1AE0},
    { ACTOR_EN_KUSA,               0x15B0},
    { ACTOR_OBJ_BEAN,              0x29E0},
    { ACTOR_OBJ_BOMBIWA,           0x570},
    { ACTOR_OBJ_SWITCH,            0x20A0},
    { ACTOR_OBJ_ELEVATOR,          0x490},
    { ACTOR_OBJ_LIFT,              0xAD0},
    { ACTOR_OBJ_HSBLOCK,           0x700},
    { ACTOR_EN_OKARINA_TAG,        0x1940},
    { ACTOR_EN_YABUSAME_MARK,      0xC00},
    { ACTOR_EN_GOROIWA,            0x2680},
    { ACTOR_EN_EX_RUPPY,           0x12B0},
    { ACTOR_EN_TORYO,              0xC70},
    { ACTOR_EN_DAIKU,              0x17F0},
    { ACTOR_EN_NWC,                0xAD0},
    { ACTOR_EN_BLKOBJ,             0x670},
    { ACTOR_ITEM_INBOX,            0x150},
    { ACTOR_EN_GE1,                0x2260},
    { ACTOR_OBJ_BLOCKSTOP,         0x190},
    { ACTOR_EN_SDA,                0x1900},
    { ACTOR_EN_CLEAR_TAG,          0xB850},
    { ACTOR_EN_NIW_LADY,           0x1E10},
    { ACTOR_EN_GM,                 0xE90},
    { ACTOR_EN_MS,                 0x700},
    { ACTOR_EN_HS,                 0xC00},
    { ACTOR_BG_INGATE,             0x420},
    { ACTOR_EN_KANBAN,             0x33B0},
    { ACTOR_EN_HEISHI3,            0xA90},
    { ACTOR_EN_SYATEKI_NIW,        0x21B0},
    { ACTOR_EN_ATTACK_NIW,         0x1250},
    { ACTOR_BG_SPOT01_IDOSOKO,     0x2C0},
    { ACTOR_EN_SA,                 0x2290},
    { ACTOR_EN_WONDER_TALK,        0xDE0},
    { ACTOR_BG_GJYO_BRIDGE,        0x570},
    { ACTOR_EN_DS,                 0xC00},
    { ACTOR_EN_MK,                 0xE70},
    { ACTOR_EN_BOM_BOWL_MAN,       0x1810},
    { ACTOR_EN_BOM_BOWL_PIT,       0x9F0},
    { ACTOR_EN_OWL,                0x3EB0},
    { ACTOR_EN_ISHI,               0x9220},
    { ACTOR_OBJ_HANA,              0x300},
    { ACTOR_OBJ_LIGHTSWITCH,       0x16D0},
    { ACTOR_OBJ_MURE2,             0xA90},
    { ACTOR_EN_GO,                 0x47C0},
    { ACTOR_EN_FU,                 0xD90},
    { ACTOR_EN_CHANGER,            0xF20},
    { ACTOR_BG_JYA_MEGAMI,         0x1310},
    { ACTOR_BG_JYA_LIFT,           0x590},
    { ACTOR_BG_JYA_BIGMIRROR,      0xAC0},
    { ACTOR_BG_JYA_BOMBCHUIWA,     0xD00},
    { ACTOR_BG_JYA_AMISHUTTER,     0x420},
    { ACTOR_BG_JYA_BOMBIWA,        0x750},
    { ACTOR_BG_SPOT18_BASKET,      0x1070},
    { ACTOR_EN_GANON_ORGAN,        0x7180},
    { ACTOR_EN_SIOFUKI,            0xE20},
    { ACTOR_EN_STREAM,             0x610},
    { ACTOR_EN_MM,                 0x16E0},
    { ACTOR_EN_KO,                 0x41F0},
    { ACTOR_EN_KZ,                 0x15F0},
    { ACTOR_EN_WEATHER_TAG,        0x1270},
    { ACTOR_BG_SST_FLOOR,          0x5F0},
    { ACTOR_EN_ANI,                0xDB0},
    { ACTOR_EN_EX_ITEM,            0x1570},
    { ACTOR_BG_JYA_IRONOBJ,        0xEA0},
    { ACTOR_EN_JS,                 0x990},
    { ACTOR_EN_JSJUTAN,            0x59D0},
    { ACTOR_EN_CS,                 0x1280},
    { ACTOR_EN_MD,                 0x26B0},
    { ACTOR_EN_HY,                 0x3A40},
    { ACTOR_EN_GANON_MANT,         0x42A0},
    { ACTOR_EN_OKARINA_EFFECT,     0x5C0},
    { ACTOR_EN_MAG,                0x2550},
    { ACTOR_DOOR_GERUDO,           0x670},
    { ACTOR_ELF_MSG2,              0x820},
    { ACTOR_DEMO_GT,               0x5C90},
    { ACTOR_EN_PO_FIELD,           0x3C40},
    { ACTOR_EFC_ERUPC,             0xC20},
    { ACTOR_BG_ZG,                 0x570},
    { ACTOR_EN_HEISHI4,            0x1130},
    { ACTOR_EN_ZL3,                0x8400},
    { ACTOR_BOSS_GANON2,           0x13700},
    { ACTOR_EN_KAKASI,             0xEA0},
    { ACTOR_EN_TAKARA_MAN,         0x9D0},
    { ACTOR_OBJ_MAKEOSHIHIKI,      0x590},
    { ACTOR_OCEFF_SPOT,            0x1070},
    { ACTOR_END_TITLE,             0x4280},
    { ACTOR_EN_TORCH,              0xF0},
    { ACTOR_DEMO_EC,               0x3AA0},
    { ACTOR_SHOT_SUN,              0x740},
    { ACTOR_EN_DY_EXTRA,           0x6A0},
    { ACTOR_EN_WONDER_TALK2,       0x10A0},
    { ACTOR_EN_GE2,                0x1A50},
    { ACTOR_OBJ_ROOMTIMER,         0x240},
    { ACTOR_EN_SSH,                0x2620},
    { ACTOR_EN_STH,                0x4260},
    { ACTOR_OCEFF_WIPE,            0xE20},
    { ACTOR_OCEFF_STORM,           0x1CB0},
    { ACTOR_EN_WEIYER,             0x1A20},
    { ACTOR_BG_SPOT05_SOKO,        0x310},
    { ACTOR_BG_JYA_1FLIFT,         0x760},
    { ACTOR_BG_JYA_HAHENIRON,      0x7E0},
    { ACTOR_BG_SPOT12_GATE,        0x490},
    { ACTOR_BG_SPOT12_SAKU,        0x540},
    { ACTOR_EN_HINTNUTS,           0x1A10},
    { ACTOR_EN_NUTSBALL,           0x690},
    { ACTOR_BG_SPOT00_BREAK,       0x1A0},
    { ACTOR_EN_SHOPNUTS,           0xF70},
    { ACTOR_EN_IT,                 0x180},
    { ACTOR_EN_GELDB,              0x5440},
    { ACTOR_OCEFF_WIPE2,           0x1840},
    { ACTOR_OCEFF_WIPE3,           0x1820},
    { ACTOR_EN_NIW_GIRL,           0xCC0},
    { ACTOR_EN_DOG,                0x1200},
    { ACTOR_EN_SI,                 0x4F0},
    { ACTOR_BG_SPOT01_OBJECTS2,    0x520},
    { ACTOR_OBJ_COMB,              0x8C0},
    { ACTOR_BG_SPOT11_BAKUDANKABE, 0x670},
    { ACTOR_OBJ_KIBAKO2,           0x700},
    { ACTOR_EN_DNT_DEMO,           0x1200},
    { ACTOR_EN_DNT_JIJI,           0x17E0},
    { ACTOR_EN_DNT_NOMAL,          0x3100},
    { ACTOR_EN_GUEST,              0xB30},
    { ACTOR_BG_BOM_GUARD,          0x280},
    { ACTOR_EN_HS2,                0x610},
    { ACTOR_DEMO_KEKKAI,           0x1490},
    { ACTOR_BG_SPOT08_BAKUDANKABE, 0x690},
    { ACTOR_BG_SPOT17_BAKUDANKABE, 0x840},
    { ACTOR_OBJ_MURE3,             0x7B0},
    { ACTOR_EN_TG,                 0x720},
    { ACTOR_EN_MU,                 0x970},
    { ACTOR_EN_GO2,                0x6190},
    { ACTOR_EN_WF,                 0x43B0},
    { ACTOR_EN_SKB,                0x1910},
    { ACTOR_DEMO_GJ,               0x3F40},
    { ACTOR_DEMO_GEFF,             0xAF0},
    { ACTOR_BG_GND_FIREMEIRO,      0x5E0},
    { ACTOR_BG_GND_DARKMEIRO,      0x890},
    { ACTOR_BG_GND_SOULMEIRO,      0x8F0},
    { ACTOR_BG_GND_NISEKABE,       0x170},
    { ACTOR_BG_GND_ICEBLOCK,       0x11B0},
    { ACTOR_EN_GB,                 0x17F0},
    { ACTOR_EN_GS,                 0x1F00},
    { ACTOR_BG_MIZU_BWALL,         0x17B0},
    { ACTOR_BG_MIZU_SHUTTER,       0x890},
    { ACTOR_EN_DAIKU_KAKARIKO,     0x14B0},
    { ACTOR_BG_BOWL_WALL,          0xA80},
    { ACTOR_EN_WALL_TUBO,          0x730},
    { ACTOR_EN_PO_DESERT,          0xE50},
    { ACTOR_EN_CROW,               0x1670},
    { ACTOR_DOOR_KILLER,           0x1650},
    { ACTOR_BG_SPOT11_OASIS,       0x7A0},
    { ACTOR_BG_SPOT18_FUTA,        0x190},
    { ACTOR_BG_SPOT18_SHUTTER,     0x540},
    { ACTOR_EN_MA3,                0x1060},
    { ACTOR_EN_COW,                0x1440},
    { ACTOR_BG_ICE_TURARA,         0x820},
    { ACTOR_BG_ICE_SHUTTER,        0x460},
    { ACTOR_EN_KAKASI2,            0xD10},
    { ACTOR_EN_KAKASI3,            0x1280},
    { ACTOR_OCEFF_WIPE4,           0x10B0},
    { ACTOR_EN_EG,                 0x210},
    { ACTOR_BG_MENKURI_NISEKABE,   0x150},
    { ACTOR_EN_ZO,                 0x27A0},
    { ACTOR_OBJ_MAKEKINSUTA,       0x2A0},
    { ACTOR_EN_GE3,                0xCF0},
    { ACTOR_OBJ_TIMEBLOCK,         0xDC0},
    { ACTOR_OBJ_HAMISHI,           0x8D0},
    { ACTOR_EN_ZL4,                0x4A70},
    { ACTOR_EN_MM2,                0xED0},
    { ACTOR_BG_JYA_BLOCK,          0x300},
    { ACTOR_OBJ_WARP2BLOCK,        0xCB0},
};

std::unordered_set<u16> registeredStairOverlays;

std::unordered_map<uintptr_t, void*> generalPtrMap;
std::unordered_map<u16, void*> actorPtrMap;
std::unordered_map<u16, void*> overlayPtrMap;
std::unordered_map<u16, void*> mallocRPtrMap;

void* absolutePtr = nullptr;

s32 absoluteSpaceFlag = 0;

u16 Stairs_GetAllocType(u16 id) {
    for (const ActorIdAllocTypePair& pair : stairActorTable) {
        if (pair.id == id) {
            return pair.allocType;
        }
    }
    return 0xFFFF;
}

s32 Stairs_RegisterOverlay(u16 id) {
    if (registeredStairOverlays.contains(id)) {
        return false;
    }
    registeredStairOverlays.insert(id);
    return true;
}

s32 Stairs_UnregisterOverlay(u16 id) {
    if (registeredStairOverlays.contains(id)) {
        registeredStairOverlays.erase(id);
        return true;
    }
    return false;
}

s32 Stairs_RegisterAbsoluteSpace() {
    if (absoluteSpaceFlag) {
        return false;
    }
    absoluteSpaceFlag = true;
    return true;
}

s32 Stairs_GetAbsoluteSpace() {
    return absoluteSpaceFlag;
}

s32 Stairs_UnregisterAbsoluteSpace() {
    absoluteSpaceFlag = false;
}

size_t Stairs_GetOverlaySize(u16 id) {
    return actorOverlaySizes[id];
}

size_t Stairs_GetSize() {
    return stairsAllocSize;
}

s32 Stairs_DecreaseSize(size_t size) {
    if (size > stairsAllocSize) {
        return false;
    }
    stairsAllocSize -= size;
    return true;
}

void* StairsArena_Malloc(size_t size) {
    void* ptr = __osMalloc(&sStairsArena, size);

    // StairsArena_CheckPointer(ptr, size, "stairs_malloc", "確保"); // "Secure"
    return ptr;
}

void* StairsArena_MallocGeneral(size_t size, uintptr_t id) {
    void* ptr = __osMalloc(&sStairsArena, size);
    generalPtrMap[id] = ptr;

    // StairsArena_CheckPointer(ptr, size, "stairs_malloc", "確保"); // "Secure"
    return ptr;
}

void* StairsArena_MallocActor(size_t size, u16 id) {
    void* ptr = __osMalloc(&sStairsArena, size);
    actorPtrMap[id] = ptr;

    // StairsArena_CheckPointer(ptr, size, "stairs_malloc", "確保"); // "Secure"
    return ptr;
}

void* StairsArena_MallocOverlay(size_t size, u16 id) {
    void* ptr = __osMalloc(&sStairsArena, size);
    overlayPtrMap[id] = ptr;

    // StairsArena_CheckPointer(ptr, size, "stairs_malloc", "確保"); // "Secure"
    return ptr;
}

void* StairsArena_MallocR(size_t size) {
    void* ptr = __osMallocR(&sStairsArena, size);

    // StairsArena_CheckPointer(ptr, size, "stairs_malloc_r", "確保"); // "Secure"
    return ptr;
}

void* StairsArena_MallocRGeneral(size_t size, u16 id) {
    void* ptr = __osMallocR(&sStairsArena, size);
    mallocRPtrMap[id] = ptr;

    // StairsArena_CheckPointer(ptr, size, "stairs_malloc_r", "確保"); // "Secure"
    return ptr;
}

void* StairsArena_MallocRAbsolute(size_t size) {
    absolutePtr = __osMallocR(&sStairsArena, size);

    // StairsArena_CheckPointer(ptr, size, "stairs_malloc_r", "確保"); // "Secure"
    return absolutePtr;
}

void* StairsArena_Realloc(void* ptr, size_t newSize) {
    ptr = __osRealloc(&sStairsArena, ptr, newSize);
    // StairsArena_CheckPointer(ptr, newSize, "stairs_realloc", "再確保"); // "Re-securing"
    return ptr;
}

void StairsArena_Free(void* ptr) {
    __osFree(&sStairsArena, ptr);
}

void StairsArena_FreeGeneral(uintptr_t id) {
    void* ptr = generalPtrMap[id];
    if (ptr) {
        __osFree(&sStairsArena, ptr);
    }
    generalPtrMap[id] = nullptr;
}

void StairsArena_FreeActor(u16 id) {
    void* ptr = actorPtrMap[id];
    if (ptr) {
        __osFree(&sStairsArena, ptr);
    }
    actorPtrMap[id] = nullptr;
}

void StairsArena_FreeOverlay(u16 id) {
    void* ptr = overlayPtrMap[id];
    if (ptr) {
        __osFree(&sStairsArena, ptr);
    }
    overlayPtrMap[id] = nullptr;
}

void StairsArena_FreeMallocR(u16 id) {
    void* ptr = mallocRPtrMap[id];
    if (ptr) {
        __osFree(&sStairsArena, ptr);
    }
    mallocRPtrMap[id] = nullptr;
}

void StairsArena_FreeAbsolute() {
    __osFree(&sStairsArena, absolutePtr);
    absolutePtr = nullptr;
}

void* StairsArena_Calloc(size_t num, size_t size) {
    void* ret;
    size_t n = num * size;

    ret = __osMalloc(&sStairsArena, n);
    if (ret != NULL) {
        memset(ret, 0,n);
    }

    // StairsArena_CheckPointer(ret, n, "stairs_calloc", "確保");
    return ret;
}

void StairsArena_Display() {
    osSyncPrintf("ゼルダヒープ表示\n"); // "Zelda heap display"
    __osDisplayArena(&sStairsArena);
}

void StairsArena_GetSizes(u32* outMaxFree, u32* outFree, u32* outAlloc) {
    ArenaImpl_GetSizes(&sStairsArena, outMaxFree, outFree, outAlloc);
}

void StairsArena_Check() {
    __osCheckArena(&sStairsArena);
}

void StairsArena_Init(void* start, size_t size) {
    __osMallocInit(&sStairsArena, start, size);
}

void StairsArena_Cleanup() {
    __osMallocCleanup(&sStairsArena);
    registeredStairOverlays.clear();
    generalPtrMap.clear();
    actorPtrMap.clear();
    overlayPtrMap.clear();
    mallocRPtrMap.clear();
    absolutePtr = nullptr;
    absoluteSpaceFlag = 0;
    stairsAllocSize = 0x1D4790 - 0x26960 - 0x2200 - (0x55 * 0x60) - (3 * (48 * 16) / 2) - (4 * (32 * 32 * 4)) - 0x1000 - (20 * sizeof(MtxF));
}

u8 StairsArena_IsInitalized() {
    return __osMallocIsInitalized(&sStairsArena);
}
