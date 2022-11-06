/*
 * File: z_en_girla.c
 * Overlay: En_GirlA
 * Description: Shop Items
 */

#include "z_en_girla.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnGirlA_Init(Actor* thisx, PlayState* play);
void EnGirlA_Destroy(Actor* thisx, PlayState* play);
void EnGirlA_Update(Actor* thisx, PlayState* play);

void EnGirlA_SetItemOutOfStock(PlayState* play, EnGirlA* this);
void EnGirlA_UpdateStockedItem(PlayState* play, EnGirlA* this);
void EnGirlA_InitializeItemAction(EnGirlA* this, PlayState* play);
void EnGirlA_Update2(EnGirlA* this, PlayState* play);
void func_80A3C498(Actor* thisx, PlayState* play, s32 flags);
void EnGirlA_Draw(Actor* thisx, PlayState* play);

s32 EnGirlA_CanBuy_Arrows(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Bombs(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_DekuNuts(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_DekuSticks(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Fish(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_RedPotion(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_GreenPotion(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_BluePotion(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Longsword(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_HylianShield(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_DekuShield(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_GoronTunic(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_ZoraTunic(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Health(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_MilkBottle(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_WeirdEgg(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Unk19(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Unk20(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Bombchus(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_DekuSeeds(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_SoldOut(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_BlueFire(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Bugs(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Poe(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Fairy(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuy_Randomizer(PlayState* play, EnGirlA* this);

void EnGirlA_ItemGive_DekuNuts(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_Arrows(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_Bombs(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_DekuSticks(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_BottledItem(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_Longsword(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_HylianShield(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_DekuShield(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_GoronTunic(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_ZoraTunic(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_Health(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_MilkBottle(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_WeirdEgg(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_Unk19(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_Unk20(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_DekuSeeds(PlayState* play, EnGirlA* this);
void EnGirlA_ItemGive_Randomizer(PlayState* play, EnGirlA* this);
void EnGirlA_BuyEvent_ShieldDiscount(PlayState* play, EnGirlA* this);
void EnGirlA_BuyEvent_ObtainBombchuPack(PlayState* play, EnGirlA* this);
void EnGirlA_BuyEvent_GoronTunic(PlayState* play, EnGirlA* this);
void EnGirlA_BuyEvent_ZoraTunic(PlayState* play, EnGirlA* this);
void EnGirlA_BuyEvent_Randomizer(PlayState* play, EnGirlA* this);

const ActorInit En_GirlA_InitVars = {
    ACTOR_EN_GIRLA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnGirlA),
    (ActorFunc)EnGirlA_Init,
    (ActorFunc)EnGirlA_Destroy,
    (ActorFunc)EnGirlA_Update,
    NULL,
    NULL,
};

static char* sShopItemDescriptions[] = {
    "デクの実×5   ",  // "Deku nut x5"
    "矢×30        ",  // "Arrow x30"
    "矢×50        ",  // "Arrow x50"
    "爆弾×5       ",  // "bomb"
    "デクの実×10  ",  // "Deku nut x10"
    "デクの棒      ", // "Deku stick"
    "爆弾×10      ",  // "Bomb x10"
    "さかな        ", // "Fish"
    "赤クスリ      ", // "Red medicine"
    "緑クスリ      ", // "Green medicine"
    "青クスリ      ", // "Blue medicine"
    "巨人のナイフ  ", // "Giant knife"
    "ハイリアの盾  ", // "Hyria Shield"
    "デクの盾      ", // "Deku Shield"
    "ゴロンの服    ", // "Goron's clothes"
    "ゾ─ラの服    ",  // "Zora's clothes"
    "回復のハート  ", // "Heart of recovery"
    "ロンロン牛乳  ", // "Ron Ron milk"
    "鶏の卵        ", // "Chicken egg"
    "インゴー牛乳  ", // "Ingo milk"
    "インゴー卵    ", // "Ingo egg"
    "もだえ石      ", // "Modae stone"
    "大人の財布    ", // "Adult wallet"
    "ハートの欠片  ", // "Heart fragment"
    "ボムチュウ    ", // "Bombchu"
    "ボムチュウ    ", // "Bombchu"
    "ボムチュウ    ", // "Bombchu"
    "ボムチュウ    ", // "Bombchu"
    "ボムチュウ    ", // "Bombchu"
    "デクのタネ    ", // "Deku seeds"
    "キータンのお面", // "Ketan's mask"
    "こわそなお面  ", // "Scary face"
    "ドクロのお面  ", // "Skull mask"
    "ウサギずきん  ", // "Rabbit hood"
    "まことの仮面  ", // "True mask"
    "ゾーラのお面  ", // "Zora's mask"
    "ゴロンのお面  ", // "Goron's mask"
    "ゲルドのお面  ", // "Gerd's mask"
    "ＳＯＬＤＯＵＴ",
    "炎            ", // "Flame"
    "虫            ", // "Bugs"
    "チョウチョ    ", // "Butterfly"
    "ポウ          ", // "Poe"
    "妖精の魂      ", // "Fairy soul"
    "矢×10        ",  // "Arrow"
    "爆弾×20      ",  // "Bomb x20"
    "爆弾×30      ",  // "Bomb x30"
    "爆弾×5       ",  // "Bomb x5"
    "赤クスリ      ", // "Red medicine"
    "赤クスリ      ", // "Red medicine"
    "Random Item  "  // "Random Item"
};

static s16 sMaskShopItems[8] = {
    ITEM_MASK_KEATON, ITEM_MASK_SPOOKY, ITEM_MASK_SKULL, ITEM_MASK_BUNNY,
    ITEM_MASK_TRUTH,  ITEM_MASK_ZORA,   ITEM_MASK_GORON, ITEM_MASK_GERUDO,
};

static u16 sMaskShopFreeToBorrowTextIds[5] = { 0x70B6, 0x70B5, 0x70B4, 0x70B7, 0x70BB };

typedef struct {
    /* 0x00 */ s16 objID;
    /* 0x02 */ s16 giDrawId;
    /* 0x04 */ void (*hiliteFunc)(Actor*, PlayState*, s32);
    /* 0x08 */ s16 price;
    /* 0x0A */ s16 count;
    /* 0x0C */ u16 itemDescTextId;
    /* 0x0C */ u16 itemBuyPromptTextId;
    /* 0x10 */ s32 getItemId;
    /* 0x14 */ s32 (*canBuyFunc)(PlayState*, EnGirlA*);
    /* 0x18 */ void (*itemGiveFunc)(PlayState*, EnGirlA*);
    /* 0x1C */ void (*buyEventFunc)(PlayState*, EnGirlA*);
} ShopItemEntry; // size = 0x20

static ShopItemEntry shopItemEntries[] = {
    // SI_DEKU_NUTS_5
    { OBJECT_GI_NUTS, GID_NUTS, func_8002ED80, 15, 5, 0x00B2, 0x007F, GI_NUTS_5_2, EnGirlA_CanBuy_DekuNuts,
      EnGirlA_ItemGive_DekuNuts, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_ARROWS_30 */
    { OBJECT_GI_ARROW, GID_ARROWS_MEDIUM, func_8002EBCC, 60, 30, 0x00C1, 0x009B, GI_ARROWS_MEDIUM,
      EnGirlA_CanBuy_Arrows, EnGirlA_ItemGive_Arrows, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_ARROWS_50 */
    { OBJECT_GI_ARROW, GID_ARROWS_LARGE, func_8002EBCC, 90, 50, 0x00B0, 0x007D, GI_ARROWS_LARGE, EnGirlA_CanBuy_Arrows,
      EnGirlA_ItemGive_Arrows, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_BOMBS_5_R25 */
    { OBJECT_GI_BOMB_1, GID_BOMB, func_8002EBCC, 25, 5, 0x00A3, 0x008B, GI_BOMBS_5, EnGirlA_CanBuy_Bombs,
      EnGirlA_ItemGive_Bombs, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_DEKU_NUTS_10 */
    { OBJECT_GI_NUTS, GID_NUTS, func_8002ED80, 30, 10, 0x00A2, 0x0087, GI_NUTS_10, EnGirlA_CanBuy_DekuNuts,
      EnGirlA_ItemGive_DekuNuts, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_DEKU_STICK */
    { OBJECT_GI_STICK, GID_STICK, NULL, 10, 1, 0x00A1, 0x0088, GI_STICKS_1, EnGirlA_CanBuy_DekuSticks,
      EnGirlA_ItemGive_DekuSticks, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_BOMBS_10 */
    { OBJECT_GI_BOMB_1, GID_BOMB, func_8002EBCC, 50, 10, 0x00B1, 0x007C, GI_BOMBS_10, EnGirlA_CanBuy_Bombs,
      EnGirlA_ItemGive_Bombs, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_FISH */
    { OBJECT_GI_FISH, GID_FISH, func_8002ED80, 200, 1, 0x00B3, 0x007E, GI_FISH, EnGirlA_CanBuy_Fish, NULL,
      EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_RED_POTION_R30 */
    { OBJECT_GI_LIQUID, GID_POTION_RED, func_8002EBCC, 30, 1, 0x00A5, 0x008E, GI_POTION_RED, EnGirlA_CanBuy_RedPotion,
      EnGirlA_ItemGive_BottledItem, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_GREEN_POTION */
    { OBJECT_GI_LIQUID, GID_POTION_GREEN, func_8002EBCC, 30, 1, 0x00A6, 0x008F, GI_POTION_GREEN,
      EnGirlA_CanBuy_GreenPotion, EnGirlA_ItemGive_BottledItem, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_BLUE_POTION */
    { OBJECT_GI_LIQUID, GID_POTION_BLUE, func_8002EBCC, 60, 1, 0x00A7, 0x0090, GI_POTION_BLUE,
      EnGirlA_CanBuy_BluePotion, EnGirlA_ItemGive_BottledItem, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_LONGSWORD */
    { OBJECT_GI_LONGSWORD, GID_SWORD_BGS, func_8002EBCC, 1000, 1, 0x00A8, 0x0091, GI_SWORD_KNIFE,
      EnGirlA_CanBuy_Longsword, EnGirlA_ItemGive_Longsword, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_HYLIAN_SHIELD */
    { OBJECT_GI_SHIELD_2, GID_SHIELD_HYLIAN, func_8002EBCC, 80, 1, 0x00A9, 0x0092, GI_SHIELD_HYLIAN,
      EnGirlA_CanBuy_HylianShield, EnGirlA_ItemGive_HylianShield, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_DEKU_SHIELD */
    { OBJECT_GI_SHIELD_1, GID_SHIELD_DEKU, func_8002EBCC, 40, 1, 0x009F, 0x0089, GI_SHIELD_DEKU,
      EnGirlA_CanBuy_DekuShield, EnGirlA_ItemGive_DekuShield, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_GORON_TUNIC */
    { OBJECT_GI_CLOTHES, GID_TUNIC_GORON, NULL, 200, 1, 0x00AA, 0x0093, GI_TUNIC_GORON, EnGirlA_CanBuy_GoronTunic,
      EnGirlA_ItemGive_GoronTunic, EnGirlA_BuyEvent_GoronTunic },
    /* SI_ZORA_TUNIC */
    { OBJECT_GI_CLOTHES, GID_TUNIC_ZORA, NULL, 300, 1, 0x00AB, 0x0094, GI_TUNIC_ZORA, EnGirlA_CanBuy_ZoraTunic,
      EnGirlA_ItemGive_ZoraTunic, EnGirlA_BuyEvent_ZoraTunic },
    /* SI_HEART */
    { OBJECT_GI_HEART, GID_HEART, NULL, 10, 16, 0x00AC, 0x0095, GI_HEART, EnGirlA_CanBuy_Health,
      EnGirlA_ItemGive_Health, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_MILK_BOTTLE */
    { OBJECT_GI_MILK, GID_MILK, func_80A3C498, 100, 1, 0x00AD, 0x0097, GI_MILK_BOTTLE, EnGirlA_CanBuy_MilkBottle,
      EnGirlA_ItemGive_MilkBottle, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_WEIRD_EGG */
    { OBJECT_GI_EGG, GID_EGG, func_8002EBCC, 100, 1, 0x00AE, 0x0099, GI_WEIRD_EGG, EnGirlA_CanBuy_WeirdEgg,
      EnGirlA_ItemGive_WeirdEgg, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_19 */
    { OBJECT_GI_MILK, GID_MILK, func_80A3C498, 10000, 1, 0x00B4, 0x0085, GI_NONE, EnGirlA_CanBuy_Unk19,
      EnGirlA_ItemGive_Unk19, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_20 */
    { OBJECT_GI_EGG, GID_EGG, func_8002EBCC, 10000, 1, 0x00B5, 0x0085, GI_NONE, EnGirlA_CanBuy_Unk20,
      EnGirlA_ItemGive_Unk20, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_BOMBCHU_10_1 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, func_8002EBCC, 100, 10, 0x00BC, 0x008C, GI_BOMBCHUS_10, EnGirlA_CanBuy_Bombchus,
      NULL, EnGirlA_BuyEvent_ObtainBombchuPack },
    /* SI_BOMBCHU_20_1 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, func_8002EBCC, 180, 20, 0x0061, 0x002A, GI_BOMBCHUS_20, EnGirlA_CanBuy_Bombchus,
      NULL, EnGirlA_BuyEvent_ObtainBombchuPack },
    /* SI_BOMBCHU_20_2 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, func_8002EBCC, 180, 20, 0x0061, 0x002A, GI_BOMBCHUS_20, EnGirlA_CanBuy_Bombchus,
      NULL, EnGirlA_BuyEvent_ObtainBombchuPack },
    /* SI_BOMBCHU_10_2 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, func_8002EBCC, 100, 10, 0x00BC, 0x008C, GI_BOMBCHUS_10, EnGirlA_CanBuy_Bombchus,
      NULL, EnGirlA_BuyEvent_ObtainBombchuPack },
    /* SI_BOMBCHU_10_3 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, func_8002EBCC, 100, 10, 0x00BC, 0x008C, GI_BOMBCHUS_10, EnGirlA_CanBuy_Bombchus,
      NULL, EnGirlA_BuyEvent_ObtainBombchuPack },
    /* SI_BOMBCHU_20_3 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, func_8002EBCC, 180, 20, 0x0061, 0x002A, GI_BOMBCHUS_20, EnGirlA_CanBuy_Bombchus,
      NULL, EnGirlA_BuyEvent_ObtainBombchuPack },
    /* SI_BOMBCHU_20_4 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, func_8002EBCC, 180, 20, 0x0061, 0x002A, GI_BOMBCHUS_20, EnGirlA_CanBuy_Bombchus,
      NULL, EnGirlA_BuyEvent_ObtainBombchuPack },
    /* SI_BOMBCHU_10_4 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, func_8002EBCC, 100, 10, 0x00BC, 0x008C, GI_BOMBCHUS_10, EnGirlA_CanBuy_Bombchus,
      NULL, EnGirlA_BuyEvent_ObtainBombchuPack },
    /* SI_DEKU_SEEDS_30 */
    { OBJECT_GI_SEED, GID_SEEDS, func_8002EBCC, 30, 30, 0x00DF, 0x00DE, GI_SEEDS_30, EnGirlA_CanBuy_DekuSeeds,
      EnGirlA_ItemGive_DekuSeeds, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_KEATON_MASK */
    { OBJECT_GI_KI_TAN_MASK, GID_MASK_KEATON, func_8002EBCC, 0, 1, 0x70B2, 0x70BE, GI_MASK_KEATON,
      EnGirlA_CanBuy_DekuSeeds, EnGirlA_ItemGive_DekuSeeds, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_SPOOKY_MASK */
    { OBJECT_GI_REDEAD_MASK, GID_MASK_SPOOKY, func_8002EBCC, 0, 1, 0x70B1, 0x70BD, GI_MASK_SPOOKY,
      EnGirlA_CanBuy_DekuSeeds, EnGirlA_ItemGive_DekuSeeds, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_SKULL_MASK */
    { OBJECT_GI_SKJ_MASK, GID_MASK_SKULL, func_8002EBCC, 0, 1, 0x70B0, 0x70BC, GI_MASK_SKULL, EnGirlA_CanBuy_DekuSeeds,
      EnGirlA_ItemGive_DekuSeeds, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_BUNNY_HOOD */
    { OBJECT_GI_RABIT_MASK, GID_MASK_BUNNY, func_8002EBCC, 0, 1, 0x70B3, 0x70BF, GI_MASK_BUNNY,
      EnGirlA_CanBuy_DekuSeeds, EnGirlA_ItemGive_DekuSeeds, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_MASK_OF_TRUTH */
    { OBJECT_GI_TRUTH_MASK, GID_MASK_TRUTH, func_80A3C498, 0, 1, 0x70AF, 0x70C3, GI_MASK_TRUTH,
      EnGirlA_CanBuy_DekuSeeds, EnGirlA_ItemGive_DekuSeeds, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_ZORA_MASK */
    { OBJECT_GI_ZORAMASK, GID_MASK_ZORA, NULL, 0, 1, 0x70B9, 0x70C1, GI_MASK_ZORA, EnGirlA_CanBuy_DekuSeeds,
      EnGirlA_ItemGive_DekuSeeds, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_GORON_MASK */
    { OBJECT_GI_GOLONMASK, GID_MASK_GORON, NULL, 0, 1, 0x70B8, 0x70C0, GI_MASK_GORON, EnGirlA_CanBuy_DekuSeeds,
      EnGirlA_ItemGive_DekuSeeds, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_GERUDO_MASK */
    { OBJECT_GI_GERUDOMASK, GID_MASK_GERUDO, NULL, 0, 1, 0x70BA, 0x70C2, GI_MASK_GERUDO, EnGirlA_CanBuy_DekuSeeds,
      EnGirlA_ItemGive_DekuSeeds, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_SOLD_OUT */
    { OBJECT_GI_SOLDOUT, GID_SOLDOUT, func_8002EBCC, 0, 0, 0x00BD, 0x70C2, GI_MASK_GERUDO, EnGirlA_CanBuy_SoldOut, NULL,
      NULL },
    /* SI_BLUE_FIRE */
    { OBJECT_GI_FIRE, GID_BLUE_FIRE, func_8002EBCC, 300, 1, 0x00B9, 0x00B8, GI_BLUE_FIRE, EnGirlA_CanBuy_BlueFire,
      EnGirlA_ItemGive_BottledItem, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_BUGS */
    { OBJECT_GI_INSECT, GID_BUG, func_80A3C498, 50, 1, 0x00BB, 0x00BA, GI_BUGS, EnGirlA_CanBuy_Bugs,
      EnGirlA_ItemGive_BottledItem, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_BIG_POE */
    { OBJECT_GI_GHOST, GID_BIG_POE, func_80A3C498, 50, 1, 0x506F, 0x5070, GI_BIG_POE, EnGirlA_CanBuy_Poe,
      EnGirlA_ItemGive_BottledItem, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_POE */
    { OBJECT_GI_GHOST, GID_POE, func_80A3C498, 30, 1, 0x506D, 0x506E, GI_POE, EnGirlA_CanBuy_Poe,
      EnGirlA_ItemGive_BottledItem, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_FAIRY */
    { OBJECT_GI_SOUL, GID_FAIRY, func_80A3C498, 50, 1, 0x00B7, 0x00B6, GI_FAIRY, EnGirlA_CanBuy_Fairy,
      EnGirlA_ItemGive_BottledItem, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_ARROWS_10 */
    { OBJECT_GI_ARROW, GID_ARROWS_SMALL, func_8002EBCC, 20, 10, 0x00A0, 0x008A, GI_ARROWS_SMALL, EnGirlA_CanBuy_Arrows,
      EnGirlA_ItemGive_Arrows, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_BOMBS_20 */
    { OBJECT_GI_BOMB_1, GID_BOMB, func_8002EBCC, 80, 20, 0x001C, 0x0006, GI_BOMBS_20, EnGirlA_CanBuy_Bombs,
      EnGirlA_ItemGive_Bombs, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_BOMBS_30 */
    { OBJECT_GI_BOMB_1, GID_BOMB, func_8002EBCC, 120, 30, 0x001D, 0x001E, GI_BOMBS_30, EnGirlA_CanBuy_Bombs,
      EnGirlA_ItemGive_Bombs, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_BOMBS_5_R35 */
    { OBJECT_GI_BOMB_1, GID_BOMB, func_8002EBCC, 35, 5, 0x00CB, 0x00CA, GI_BOMBS_5, EnGirlA_CanBuy_Bombs,
      EnGirlA_ItemGive_Bombs, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_RED_POTION_R40 */
    { OBJECT_GI_LIQUID, GID_POTION_RED, func_8002EBCC, 40, 1, 0x0064, 0x0062, GI_POTION_RED, EnGirlA_CanBuy_RedPotion,
      EnGirlA_ItemGive_BottledItem, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_RED_POTION_R50 */
    { OBJECT_GI_LIQUID, GID_POTION_RED, func_8002EBCC, 50, 1, 0x0065, 0x0063, GI_POTION_RED, EnGirlA_CanBuy_RedPotion,
      EnGirlA_ItemGive_BottledItem, EnGirlA_BuyEvent_ShieldDiscount },
    /* SI_RANDOMIZED_ITEM */
    { OBJECT_INVALID, GID_MAXIMUM, NULL, 40, 1, 0x9100, 0x9100 + NUM_SHOP_ITEMS, GI_NONE, EnGirlA_CanBuy_Randomizer,
      EnGirlA_ItemGive_Randomizer, EnGirlA_BuyEvent_Randomizer }
};

// Defines the Hylian Shield discount amount
static s16 sShieldDiscounts[] = { 5, 10, 15, 20, 25, 30, 35, 40 };

void EnGirlA_SetupAction(EnGirlA* this, EnGirlAActionFunc func) {
    this->actionFunc = func;
}

s32 EnGirlA_TryChangeShopItem(EnGirlA* this, PlayState* play) {
    switch (this->actor.params) {
        case SI_MILK_BOTTLE:
            if (gSaveContext.itemGetInf[0] & 0x4) {
                this->actor.params = SI_HEART;
                return true;
            }
            break;
        case SI_BOMBCHU_10_2:
            if (gSaveContext.itemGetInf[0] & 0x40) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_10_3:
            if (gSaveContext.itemGetInf[0] & 0x80) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_20_3:
            if (gSaveContext.itemGetInf[0] & 0x100) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_20_4:
            if (gSaveContext.itemGetInf[0] & 0x200) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_10_4:
            if (gSaveContext.itemGetInf[0] & 0x400) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_10_1:
            if (gSaveContext.itemGetInf[0] & 0x8) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_20_1:
            if (gSaveContext.itemGetInf[0] & 0x10) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_20_2:
            if (gSaveContext.itemGetInf[0] & 0x20) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_RANDOMIZED_ITEM: {
            ShopItemIdentity shopItemIdentity = Randomizer_IdentifyShopItem(play->sceneNum, this->randoSlotIndex);
            if (Flags_GetRandomizerInf(shopItemIdentity.randomizerInf)) {
                this->actor.params = SI_SOLD_OUT;
                GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheckWithoutObtainabilityCheck(shopItemIdentity.randomizerCheck, shopItemIdentity.ogItemId);

                // Undo the rotation for spiritual stones
                if (getItemEntry.getItemId >= RG_KOKIRI_EMERALD && getItemEntry.getItemId <= RG_ZORA_SAPPHIRE) {
                    this->actor.shape.rot.y = this->actor.shape.rot.y - 20000;
                }
                return true;
            }
            break;
        }
    }

    return false;
}

void EnGirlA_InitItem(EnGirlA* this, PlayState* play) {
    s16 params = this->actor.params;

    osSyncPrintf("%s(%2d)初期設定\n", sShopItemDescriptions[params], params);

    if ((params >= SI_MAX) && (params < 0)) {
        Actor_Kill(&this->actor);
        osSyncPrintf(VT_COL(RED, WHITE));
        osSyncPrintf("引数がおかしいよ(arg_data=%d)！！\n", this->actor.params);
        osSyncPrintf(VT_RST);
        ASSERT((params >= SI_MAX) && (params < 0));
        return;
    }

    if (!gSaveContext.n64ddFlag || !Randomizer_GetSettingValue(RSK_SHOPSANITY)) {
        this->objBankIndex = Object_GetIndex(&play->objectCtx, shopItemEntries[params].objID);
    } else {
        s16 objectId = shopItemEntries[params].objID;

        if (params == SI_RANDOMIZED_ITEM) {
            ShopItemIdentity shopItemIdentity = Randomizer_IdentifyShopItem(play->sceneNum, this->randoSlotIndex);
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheckWithoutObtainabilityCheck(shopItemIdentity.randomizerCheck, shopItemIdentity.ogItemId);

            objectId = getItemEntry.objectId;
        }

        // Weird edge case here, sold out object reports as loaded for Kokiri shop but doesn't render so we force it to load here
        if (Object_IsLoaded(&play->objectCtx, objectId) && (params != SI_SOLD_OUT && play->sceneNum == SCENE_KOKIRI_SHOP)) {
            this->objBankIndex = Object_GetIndex(&play->objectCtx, objectId);
        } else {
            this->objBankIndex = Object_Spawn(&play->objectCtx, objectId);
        }
    }

    if (this->objBankIndex < 0) {
        Actor_Kill(&this->actor);
        osSyncPrintf(VT_COL(RED, WHITE));
        osSyncPrintf("バンクが無いよ！！(%s)\n", sShopItemDescriptions[params]);
        osSyncPrintf(VT_RST);
        ASSERT(this->objBankIndex < 0);
        return;
    }

    this->actor.params = params;
    this->actionFunc2 = EnGirlA_InitializeItemAction;
}

void EnGirlA_Init(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    EnGirlA_TryChangeShopItem(this, play);
    EnGirlA_InitItem(this, play);
    osSyncPrintf("%s(%2d)初期設定\n", sShopItemDescriptions[this->actor.params], this->actor.params);
}

void EnGirlA_Destroy(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    if (this->isInitialized) {
        SkelAnime_Free(&this->skelAnime, play);
    }
}

s32 EnGirlA_CanBuy_Arrows(PlayState* play, EnGirlA* this) {
    if (Item_CheckObtainability(ITEM_BOW) == ITEM_NONE) {
        return CANBUY_RESULT_CANT_GET_NOW_5;
    }
    if (AMMO(ITEM_BOW) >= CUR_CAPACITY(UPG_QUIVER)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_Bombs(PlayState* play, EnGirlA* this) {
    if (!gSaveContext.n64ddFlag && !CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (AMMO(ITEM_BOMB) >= CUR_CAPACITY(UPG_BOMB_BAG)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_DekuNuts(PlayState* play, EnGirlA* this) {
    if ((CUR_CAPACITY(UPG_NUTS) != 0) && (AMMO(ITEM_NUT) >= CUR_CAPACITY(UPG_NUTS))) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if ((Item_CheckObtainability(ITEM_NUT) == ITEM_NONE) && !CVar_GetS32("gFastDrops", 0)) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_DekuSticks(PlayState* play, EnGirlA* this) {
    if ((CUR_CAPACITY(UPG_STICKS) != 0) && (AMMO(ITEM_STICK) >= CUR_CAPACITY(UPG_STICKS))) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if ((Item_CheckObtainability(ITEM_STICK) == ITEM_NONE) && !CVar_GetS32("gFastDrops", 0)) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_Fish(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_FISH) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_RedPotion(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_POTION_RED) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_GreenPotion(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_POTION_GREEN) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_BluePotion(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_POTION_BLUE) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_Longsword(PlayState* play, EnGirlA* this) {
    if ((gBitFlags[2] & gSaveContext.inventory.equipment) && !(gBitFlags[3] & gSaveContext.inventory.equipment)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_SWORD_BGS) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_HylianShield(PlayState* play, EnGirlA* this) {
    if (gBitFlags[5] & gSaveContext.inventory.equipment) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_SHIELD_HYLIAN) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_DekuShield(PlayState* play, EnGirlA* this) {
    if (gBitFlags[4] & gSaveContext.inventory.equipment) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_SHIELD_DEKU) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_GoronTunic(PlayState* play, EnGirlA* this) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD && (!gSaveContext.n64ddFlag || !Randomizer_GetSettingValue(RSK_SHOPSANITY))) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gBitFlags[9] & gSaveContext.inventory.equipment) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_TUNIC_GORON) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_ZoraTunic(PlayState* play, EnGirlA* this) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD && (!gSaveContext.n64ddFlag || !Randomizer_GetSettingValue(RSK_SHOPSANITY))) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gBitFlags[10] & gSaveContext.inventory.equipment) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_TUNIC_ZORA) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_Health(PlayState* play, EnGirlA* this) {
    if (gSaveContext.healthCapacity == gSaveContext.health) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_MilkBottle(PlayState* play, EnGirlA* this) {
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_MILK_BOTTLE) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_WeirdEgg(PlayState* play, EnGirlA* this) {
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_LETTER_ZELDA) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_Unk19(PlayState* play, EnGirlA* this) {
    return CANBUY_RESULT_NEED_RUPEES;
}

s32 EnGirlA_CanBuy_Unk20(PlayState* play, EnGirlA* this) {
    return CANBUY_RESULT_NEED_RUPEES;
}

s32 EnGirlA_CanBuy_Bombchus(PlayState* play, EnGirlA* this) {
    // When in rando, don't allow buying bombchus when the player doesn't have required explosives
    // If bombchus are in logic, the player needs to have bombchus; otherwise they need a bomb bag
    if (gSaveContext.n64ddFlag) {
        u8 bombchusInLogic = Randomizer_GetSettingValue(RSK_BOMBCHUS_IN_LOGIC);
        if ((!bombchusInLogic && CUR_CAPACITY(UPG_BOMB_BAG) == 0) ||
            (bombchusInLogic && INV_CONTENT(ITEM_BOMBCHU) == ITEM_NONE)) {
            return CANBUY_RESULT_CANT_GET_NOW;
        }
    }
    if (AMMO(ITEM_BOMBCHU) >= 50) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_BOMBCHU) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_DekuSeeds(PlayState* play, EnGirlA* this) {
    if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if ((Item_CheckObtainability(ITEM_SEEDS) == ITEM_NONE) && !CVar_GetS32("gFastDrops", 0)) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_SoldOut(PlayState* play, EnGirlA* this) {
    return CANBUY_RESULT_CANT_GET_NOW_5;
}

s32 EnGirlA_CanBuy_BlueFire(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_BLUE_FIRE) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_Bugs(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_BUG) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_Poe(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_POE) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_Fairy(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (gSaveContext.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_FAIRY) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 EnGirlA_CanBuy_Randomizer(PlayState* play, EnGirlA* this) {
    ShopItemIdentity shopItemIdentity = Randomizer_IdentifyShopItem(play->sceneNum, this->randoSlotIndex);
    GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheckWithoutObtainabilityCheck(shopItemIdentity.randomizerCheck, shopItemIdentity.ogItemId);
    ItemObtainability itemObtainability = Randomizer_GetItemObtainabilityFromRandomizerCheck(shopItemIdentity.randomizerCheck);

    if (itemObtainability == CANT_OBTAIN_NEED_EMPTY_BOTTLE) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    
    if (itemObtainability == CANT_OBTAIN_NEED_UPGRADE) {
        return CANBUY_RESULT_CANT_GET_NOW_5;
    }

    if (
        Flags_GetRandomizerInf(shopItemIdentity.randomizerInf) || 
        itemObtainability == CANT_OBTAIN_ALREADY_HAVE || 
        itemObtainability == CANT_OBTAIN_MISC
    ) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }

    if (gSaveContext.rupees < shopItemIdentity.itemPrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }

    return CANBUY_RESULT_SUCCESS_FANFARE;
}

void EnGirlA_ItemGive_Arrows(PlayState* play, EnGirlA* this) {
    Inventory_ChangeAmmo(ITEM_BOW, this->itemCount);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_Bombs(PlayState* play, EnGirlA* this) {
    switch (this->itemCount) {
        case 5:
            Item_Give(play, ITEM_BOMBS_5);
            break;
        case 10:
            Item_Give(play, ITEM_BOMBS_10);
            break;
        case 20:
            Item_Give(play, ITEM_BOMBS_20);
            break;
        case 30:
            Item_Give(play, ITEM_BOMBS_30);
            break;
    }
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_DekuNuts(PlayState* play, EnGirlA* this) {
    switch (this->itemCount) {
        case 5:
            Item_Give(play, ITEM_NUTS_5);
            break;
        case 10:
            Item_Give(play, ITEM_NUTS_10);
            break;
    }
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_DekuSticks(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_STICK);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_Longsword(PlayState* play, EnGirlA* this) {
    func_800849EC(play);
    gSaveContext.swordHealth = 8;
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_HylianShield(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_SHIELD_HYLIAN);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_DekuShield(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_SHIELD_DEKU);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_GoronTunic(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_TUNIC_GORON);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_ZoraTunic(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_TUNIC_ZORA);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_Health(PlayState* play, EnGirlA* this) {
    Health_ChangeBy(play, this->itemCount);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_MilkBottle(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_MILK_BOTTLE);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_WeirdEgg(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_WEIRD_EGG);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_Unk19(PlayState* play, EnGirlA* this) {
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_Unk20(PlayState* play, EnGirlA* this) {
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_DekuSeeds(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_SEEDS_30);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_ItemGive_BottledItem(PlayState* play, EnGirlA* this) {
    switch (this->actor.params) {
        case SI_FISH:
            Item_Give(play, ITEM_FISH);
            break;
        case SI_RED_POTION_R30:
            Item_Give(play, ITEM_POTION_RED);
            break;
        case SI_GREEN_POTION:
            Item_Give(play, ITEM_POTION_GREEN);
            break;
        case SI_BLUE_POTION:
            Item_Give(play, ITEM_POTION_BLUE);
            break;
        case SI_BLUE_FIRE:
            Item_Give(play, ITEM_BLUE_FIRE);
            break;
        case SI_BUGS:
            Item_Give(play, ITEM_BUG);
            break;
        case SI_BIG_POE:
            Item_Give(play, ITEM_BIG_POE);
            break;
        case SI_POE:
            Item_Give(play, ITEM_POE);
            break;
        case SI_FAIRY:
            Item_Give(play, ITEM_FAIRY);
            break;
    }
    Rupees_ChangeBy(-this->basePrice);
}

// This is called when EnGirlA_CanBuy_Randomizer returns CANBUY_RESULT_SUCCESS
// The giving of the item is handled here, and no fanfare is played
void EnGirlA_ItemGive_Randomizer(PlayState* play, EnGirlA* this) {
    Player* player = GET_PLAYER(play);
    ShopItemIdentity shopItemIdentity = Randomizer_IdentifyShopItem(play->sceneNum, this->randoSlotIndex);
    GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheckWithoutObtainabilityCheck(shopItemIdentity.randomizerCheck, shopItemIdentity.ogItemId);

    if (getItemEntry.modIndex == MOD_NONE) {
        // RANDOTOD: Move this into Item_Give() or some other more central location
        if (getItemEntry.getItemId == GI_SWORD_BGS) {
            gSaveContext.bgsFlag = true;
        }
        Item_Give(play, getItemEntry.itemId);
    } else if (getItemEntry.modIndex == MOD_RANDOMIZER && getItemEntry.getItemId != RG_ICE_TRAP) {
        Randomizer_Item_Give(play, getItemEntry);
    }

    Flags_SetRandomizerInf(shopItemIdentity.randomizerInf);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_BuyEvent_ShieldDiscount(PlayState* play, EnGirlA* this) {
    if (this->actor.params == SI_HYLIAN_SHIELD) {
        if (gSaveContext.infTable[7] & 0x40) {
            Rupees_ChangeBy(-(this->basePrice - sShieldDiscounts[(s32)Rand_ZeroFloat(7.9f)]));
            return;
        }
    }
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_BuyEvent_GoronTunic(PlayState* play, EnGirlA* this) {
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_BuyEvent_ZoraTunic(PlayState* play, EnGirlA* this) {
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_BuyEvent_ObtainBombchuPack(PlayState* play, EnGirlA* this) {
    Rupees_ChangeBy(-this->basePrice);

	// Normally, buying a bombchu pack sets a flag indicating the pack is now sold out
    // If they're in logic for rando, skip setting that flag so they can be purchased repeatedly
    if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_BOMBCHUS_IN_LOGIC)) {
        return;
    }

    switch (this->actor.params) {
        case SI_BOMBCHU_10_2:
            gSaveContext.itemGetInf[0] |= 0x40;
            break;
        case SI_BOMBCHU_10_3:
            gSaveContext.itemGetInf[0] |= 0x80;
            break;
        case SI_BOMBCHU_20_3:
            gSaveContext.itemGetInf[0] |= 0x100;
            break;
        case SI_BOMBCHU_20_4:
            gSaveContext.itemGetInf[0] |= 0x200;
            break;
        case SI_BOMBCHU_10_4:
            gSaveContext.itemGetInf[0] |= 0x400;
            break;
        case SI_BOMBCHU_10_1:
            gSaveContext.itemGetInf[0] |= 0x8;
            break;
        case SI_BOMBCHU_20_1:
            gSaveContext.itemGetInf[0] |= 0x10;
            break;
        case SI_BOMBCHU_20_2:
            gSaveContext.itemGetInf[0] |= 0x20;
            break;
    }
}

// This is called when EnGirlA_CanBuy_Randomizer returns CANBUY_RESULT_SUCCESS_FANFARE
// The giving of the item is handled in ossan.c, and a fanfare is played
void EnGirlA_BuyEvent_Randomizer(PlayState* play, EnGirlA* this) {
    ShopItemIdentity shopItemIdentity = Randomizer_IdentifyShopItem(play->sceneNum, this->randoSlotIndex);
    Flags_SetRandomizerInf(shopItemIdentity.randomizerInf);
    Rupees_ChangeBy(-this->basePrice);
}

void EnGirlA_Noop(EnGirlA* this, PlayState* play) {
}

void EnGirlA_SetItemDescription(PlayState* play, EnGirlA* this) {
    ShopItemEntry* tmp = &shopItemEntries[this->actor.params];
    s32 params = this->actor.params;
    s32 maskId;
    s32 isMaskFreeToBorrow;

    if ((this->actor.params >= SI_KEATON_MASK) && (this->actor.params <= SI_MASK_OF_TRUTH)) {
        maskId = this->actor.params - SI_KEATON_MASK;
        isMaskFreeToBorrow = false;
        switch (this->actor.params) {
            case SI_KEATON_MASK:
                if (gSaveContext.itemGetInf[3] & 0x100) {
                    isMaskFreeToBorrow = true;
                }
                break;
            case SI_SPOOKY_MASK:
                if (gSaveContext.itemGetInf[3] & 0x400) {
                    isMaskFreeToBorrow = true;
                }
                break;
            case SI_SKULL_MASK:
                if (gSaveContext.itemGetInf[3] & 0x200) {
                    isMaskFreeToBorrow = true;
                }
                break;
            case SI_BUNNY_HOOD:
                if (gSaveContext.itemGetInf[3] & 0x800) {
                    isMaskFreeToBorrow = true;
                }
                break;
            case SI_MASK_OF_TRUTH:
                if (gSaveContext.itemGetInf[3] & 0x800) {
                    isMaskFreeToBorrow = true;
                }
                break;
        }
        if (isMaskFreeToBorrow) {
            this->actor.textId = sMaskShopFreeToBorrowTextIds[maskId];
        } else {
            this->actor.textId = tmp->itemDescTextId;
        }
    } else {
        this->actor.textId = tmp->itemDescTextId;
    }

    if (params == SI_RANDOMIZED_ITEM) {
        ShopItemIdentity shopItemIdentity = Randomizer_IdentifyShopItem(play->sceneNum, this->randoSlotIndex);
        this->actor.textId = 0x9100 + (shopItemIdentity.randomizerInf - RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1);
    }

    this->isInvisible = false;
    this->actor.draw = EnGirlA_Draw;
}

void EnGirlA_SetItemOutOfStock(PlayState* play, EnGirlA* this) {
    this->isInvisible = true;
    this->actor.draw = NULL;
    if (((this->actor.params >= SI_KEATON_MASK) && (this->actor.params <= SI_GERUDO_MASK)) || this->actor.params == SI_RANDOMIZED_ITEM) {
        this->actor.textId = 0xBD;
    }
}

void EnGirlA_UpdateStockedItem(PlayState* play, EnGirlA* this) {
    ShopItemEntry* itemEntry;

    if (EnGirlA_TryChangeShopItem(this, play)) {
        EnGirlA_InitItem(this, play);
        itemEntry = &shopItemEntries[this->actor.params];

        if (this->actor.params == SI_RANDOMIZED_ITEM) {
            ShopItemIdentity shopItemIdentity = Randomizer_IdentifyShopItem(play->sceneNum, this->randoSlotIndex);
            this->actor.textId = 0x9100 + (shopItemIdentity.randomizerInf - RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1);
        } else {
            this->actor.textId = itemEntry->itemDescTextId;
        }
    } else {
        this->isInvisible = false;
        this->actor.draw = EnGirlA_Draw;
    }
}

s32 EnGirlA_TrySetMaskItemDescription(EnGirlA* this, PlayState* play) {
    s32 params;

    if ((this->actor.params >= SI_KEATON_MASK) && (this->actor.params <= SI_GERUDO_MASK)) {
        params = this->actor.params - SI_KEATON_MASK;
        if (INV_CONTENT(ITEM_TRADE_CHILD) == sMaskShopItems[params]) {
            EnGirlA_SetItemOutOfStock(play, this);
        } else {
            EnGirlA_SetItemDescription(play, this);
        }
        return true;
    }
    return false;
}

void EnGirlA_InitializeItemAction(EnGirlA* this, PlayState* play) {
    s16 params = this->actor.params;
    ShopItemEntry* itemEntry = &shopItemEntries[params];

    if (Object_IsLoaded(&play->objectCtx, this->objBankIndex)) {
        this->actor.flags &= ~ACTOR_FLAG_4;
        this->actor.objBankIndex = this->objBankIndex;
        switch (this->actor.params) {
            case SI_KEATON_MASK:
                if (gSaveContext.itemGetInf[3] & 0x100) {
                    this->actor.textId = 0x70B6;
                } else {
                    this->actor.textId = itemEntry->itemDescTextId;
                }
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_SPOOKY_MASK:
                if (gSaveContext.itemGetInf[3] & 0x400) {
                    this->actor.textId = 0x70B5;
                } else {
                    this->actor.textId = itemEntry->itemDescTextId;
                }
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_SKULL_MASK:
                if (gSaveContext.itemGetInf[3] & 0x200) {
                    this->actor.textId = 0x70B4;
                } else {
                    this->actor.textId = itemEntry->itemDescTextId;
                }
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_BUNNY_HOOD:
                if (gSaveContext.itemGetInf[3] & 0x800) {
                    this->actor.textId = 0x70B7;
                } else {
                    this->actor.textId = itemEntry->itemDescTextId;
                }
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_MASK_OF_TRUTH:
                if (gSaveContext.itemGetInf[3] & 0x800) {
                    this->actor.textId = 0x70BB;
                    this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                } else {
                    this->actor.textId = itemEntry->itemDescTextId;
                    this->itemBuyPromptTextId = 0xEB;
                }
                break;
            case SI_ZORA_MASK:
                this->actor.textId = itemEntry->itemDescTextId;
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_GORON_MASK:
                this->actor.textId = itemEntry->itemDescTextId;
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_GERUDO_MASK:
                this->actor.textId = itemEntry->itemDescTextId;
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            default:
                this->actor.textId = itemEntry->itemDescTextId;
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
        }
        if (!EnGirlA_TrySetMaskItemDescription(this, play)) {
            EnGirlA_SetItemDescription(play, this);
        }

        this->setOutOfStockFunc = EnGirlA_SetItemOutOfStock;
        this->updateStockedItemFunc = EnGirlA_UpdateStockedItem;
        this->getItemId = itemEntry->getItemId;
        this->canBuyFunc = itemEntry->canBuyFunc;
        this->itemGiveFunc = itemEntry->itemGiveFunc;
        this->buyEventFunc = itemEntry->buyEventFunc;
        // If chus are in logic, make the 10 pack affordable without a wallet upgrade
        if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_BOMBCHUS_IN_LOGIC) &&
            this->getItemId == GI_BOMBCHUS_10) {
            this->basePrice = 99;
        } else {
            this->basePrice = itemEntry->price;
        }
        this->itemCount = itemEntry->count;
        this->hiliteFunc = itemEntry->hiliteFunc;
        this->giDrawId = itemEntry->giDrawId;
        osSyncPrintf("%s(%2d)\n", sShopItemDescriptions[params], params);
        this->actor.flags &= ~ACTOR_FLAG_0;
        Actor_SetScale(&this->actor, 0.25f);
        this->actor.shape.yOffset = 24.0f;
        this->actor.shape.shadowScale = 4.0f;
        this->actor.floorHeight = this->actor.home.pos.y;
        this->actor.gravity = 0.0f;
        EnGirlA_SetupAction(this, EnGirlA_Noop);
        this->isInitialized = true;
        this->actionFunc2 = EnGirlA_Update2;
        this->isSelected = false;
        this->yRotation = 0;
        this->yRotationInit = this->actor.shape.rot.y;

        if (params == SI_RANDOMIZED_ITEM) {
            ShopItemIdentity shopItemIdentity = Randomizer_IdentifyShopItem(play->sceneNum, this->randoSlotIndex);
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheckWithoutObtainabilityCheck(shopItemIdentity.randomizerCheck, shopItemIdentity.ogItemId);
            this->actor.textId = 0x9100 + (shopItemIdentity.randomizerInf - RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1);
            this->itemBuyPromptTextId = 0x9100 + ((shopItemIdentity.randomizerInf - RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1) + NUM_SHOP_ITEMS);
            this->getItemId = getItemEntry.getItemId;
            this->basePrice = shopItemIdentity.itemPrice;
            this->giDrawId = getItemEntry.gid;

            // Correct the rotation for spiritual stones
            if (getItemEntry.getItemId >= RG_KOKIRI_EMERALD && getItemEntry.getItemId <= RG_ZORA_SAPPHIRE) {
                this->actor.shape.rot.y = this->actor.shape.rot.y + 20000;
            }
        }
    }
}

void EnGirlA_Update2(EnGirlA* this, PlayState* play) {
    Actor_SetScale(&this->actor, 0.25f);
    this->actor.shape.yOffset = 24.0f;
    this->actor.shape.shadowScale = 4.0f;
    EnGirlA_TrySetMaskItemDescription(this, play);
    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 5.0f);
    this->actor.shape.rot.x = 0.0f;
    if (this->actor.params != SI_SOLD_OUT) {
        if (this->isSelected) {
            this->yRotation += 0x1F4;
        } else {
            Math_SmoothStepToS(&this->yRotation, 0, 10, 0x7D0, 0);
        }
    }
}

void EnGirlA_Update(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    this->actionFunc2(this, play);
}

void func_80A3C498(Actor* thisx, PlayState* play, s32 flags) {
    func_8002EBCC(thisx, play, 0);
    func_8002ED80(thisx, play, 0);
}

void EnGirlA_Draw(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    Matrix_RotateY(((this->yRotation * 360.0f) / 65536.0f) * (M_PI / 180.0f), MTXMODE_APPLY);
    if (this->hiliteFunc != NULL) {
        this->hiliteFunc(thisx, play, 0);
    }

    if (this->actor.params == SI_RANDOMIZED_ITEM) {
        ShopItemIdentity shopItemIdentity = Randomizer_IdentifyShopItem(play->sceneNum, this->randoSlotIndex);
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheckWithoutObtainabilityCheck(shopItemIdentity.randomizerCheck, shopItemIdentity.ogItemId);

        EnItem00_CustomItemsParticles(&this->actor, play, getItemEntry);
        GetItemEntry_Draw(play, getItemEntry);
        return;
    }

    GetItem_Draw(play, this->giDrawId);
}
