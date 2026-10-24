#include "global.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/parameter_static/parameter_static.h"
#include <soh_assets.h>

// Bit Flag array in which gBitFlags[n] is literally (1u << n)
u32 gBitFlags[] = {
    (1u << 0),  (1u << 1),  (1u << 2),  (1u << 3),  (1u << 4),  (1u << 5),  (1u << 6),  (1u << 7),
    (1u << 8),  (1u << 9),  (1u << 10), (1u << 11), (1u << 12), (1u << 13), (1u << 14), (1u << 15),
    (1u << 16), (1u << 17), (1u << 18), (1u << 19), (1u << 20), (1u << 21), (1u << 22), (1u << 23),
    (1u << 24), (1u << 25), (1u << 26), (1u << 27), (1u << 28), (1u << 29), (1u << 30), (1u << 31),
};

u16 gEquipMasks[] = { 0x000F, 0x00F0, 0x0F00, 0xF000 };
u16 gEquipNegMasks[] = { 0xFFF0, 0xFF0F, 0xF0FF, 0x0FFF };
u32 gUpgradeMasks[] = {
    0x00000007, 0x00000038, 0x000001C0, 0x00000E00, 0x00003000, 0x0001C000, 0x000E0000, 0x00700000,
};
u32 gUpgradeNegMasks[] = {
    0xFFFFFFF8, 0xFFFFFFC7, 0xFFFFFE3F, 0xFFFFF1FF, 0xFFFFCFFF, 0xFFFE3FFF, 0xFFF1FFFF, 0xFF8FFFFF,
};
u8 gEquipShifts[] = { 0, 4, 8, 12 };
u8 gUpgradeShifts[] = { 0, 3, 6, 9, 12, 14, 17, 20 };

u16 gUpgradeCapacities[][4] = {
    { 0, 30, 40, 50 },     // Quivers
    { 0, 20, 30, 40 },     // Bomb Bags
    { 0, 0, 0, 0 },        // Unused (Scale)
    { 0, 0, 0, 0 },        // Unused (Strength)
    { 99, 200, 500, 999 }, // Wallets
    { 0, 30, 40, 50 },     // Deku Seed Bullet Bags
    { 0, 10, 20, 30 },     // Deku Stick Upgrades
    { 0, 20, 30, 40 },     // Deku Nut Upgrades
};

u32 gGsFlagsMasks[] = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
u32 gGsFlagsShifts[] = { 0, 8, 16, 24 };

void* gItemIcons[] = {
    gItemIconDekuStickTex,
    gItemIconDekuNutTex,
    gItemIconBombTex,
    gItemIconBowTex,
    gItemIconArrowFireTex,
    gItemIconDinsFireTex,
    gItemIconSlingshotTex,
    gItemIconOcarinaFairyTex,
    gItemIconOcarinaOfTimeTex,
    gItemIconBombchuTex,
    gItemIconHookshotTex,
    gItemIconLongshotTex,
    gItemIconArrowIceTex,
    gItemIconFaroresWindTex,
    gItemIconBoomerangTex,
    gItemIconLensOfTruthTex,
    gItemIconMagicBeanTex,
    gItemIconHammerTex,
    gItemIconArrowLightTex,
    gItemIconNayrusLoveTex,
    gItemIconBottleEmptyTex,
    gItemIconBottlePotionRedTex,
    gItemIconBottlePotionGreenTex,
    gItemIconBottlePotionBlueTex,
    gItemIconBottleFairyTex,
    gItemIconBottleFishTex,
    gItemIconBottleMilkFullTex,
    gItemIconBottleRutosLetterTex,
    gItemIconBottleBlueFireTex,
    gItemIconBottleBugTex,
    gItemIconBottleBigPoeTex,
    gItemIconBottleMilkHalfTex,
    gItemIconBottlePoeTex,
    gItemIconWeirdEggTex,
    gItemIconChickenTex,
    gItemIconZeldasLetterTex,
    gItemIconMaskKeatonTex,
    gItemIconMaskSkullTex,
    gItemIconMaskSpookyTex,
    gItemIconMaskBunnyHoodTex,
    gItemIconMaskGoronTex,
    gItemIconMaskZoraTex,
    gItemIconMaskGerudoTex,
    gItemIconMaskTruthTex,
    gItemIconSoldOutTex,
    gItemIconPocketEggTex,
    gItemIconPocketCuccoTex,
    gItemIconCojiroTex,
    gItemIconOddMushroomTex,
    gItemIconOddPotionTex,
    gItemIconPoachersSawTex,
    gItemIconBrokenGoronsSwordTex,
    gItemIconPrescriptionTex,
    gItemIconEyeballFrogTex,
    gItemIconEyeDropsTex,
    gItemIconClaimCheckTex,
    gItemIconBowFireTex,
    gItemIconBowIceTex,
    gItemIconBowLightTex,
    gItemIconSwordKokiriTex,
    gItemIconSwordMasterTex,
    gItemIconSwordBiggoronTex,
    gItemIconShieldDekuTex,
    gItemIconShieldHylianTex,
    gItemIconShieldMirrorTex,
    gItemIconTunicKokiriTex,
    gItemIconTunicGoronTex,
    gItemIconTunicZoraTex,
    gItemIconBootsKokiriTex,
    gItemIconBootsIronTex,
    gItemIconBootsHoverTex,
    gItemIconBulletBag30Tex,
    gItemIconBulletBag40Tex,
    gItemIconBulletBag50Tex,
    gItemIconQuiver30Tex,
    gItemIconQuiver40Tex,
    gItemIconQuiver50Tex,
    gItemIconBombBag20Tex,
    gItemIconBombBag30Tex,
    gItemIconBombBag40Tex,
    gItemIconGoronsBraceletTex,
    gItemIconSilverGauntletsTex,
    gItemIconGoldenGauntletsTex,
    gItemIconScaleSilverTex,
    gItemIconScaleGoldenTex,
    gItemIconBrokenGiantsKnifeTex,
    gItemIconAdultsWalletTex,
    gItemIconGiantsWalletTex,
    gItemIconDekuSeedsTex,
    gItemIconFishingPoleTex,
    gSongNoteTex,
    gSongNoteTex,
    gSongNoteTex,
    gSongNoteTex,
    gSongNoteTex,
    gSongNoteTex,
    gSongNoteTex,
    gSongNoteTex,
    gSongNoteTex,
    gSongNoteTex,
    gSongNoteTex,
    gSongNoteTex,
    gQuestIconMedallionForestTex,
    gQuestIconMedallionFireTex,
    gQuestIconMedallionWaterTex,
    gQuestIconMedallionSpiritTex,
    gQuestIconMedallionShadowTex,
    gQuestIconMedallionLightTex,
    gQuestIconKokiriEmeraldTex,
    gQuestIconGoronRubyTex,
    gQuestIconZoraSapphireTex,
    gQuestIconStoneOfAgonyTex,
    gQuestIconGerudosCardTex,
    gQuestIconGoldSkulltulaTex,
    gQuestIconHeartContainerTex,
    gQuestIconHeartPieceTex,
    gQuestIconDungeonBossKeyTex,
    gQuestIconDungeonCompassTex,
    gQuestIconDungeonMapTex,
    gQuestIconSmallKeyTex,
    gQuestIconMagicJarSmallTex,
    gQuestIconMagicJarBigTex,
    gHeartPieceIcon1Tex,
    gHeartPieceIcon2Tex,
    gHeartPieceIcon3Tex,
    gOcarinaBtnIconCUpTex,
    gOcarinaBtnIconCDownTex,
    gOcarinaBtnIconCLeftTex,
    gOcarinaBtnIconCRightTex,
    gOcarinaBtnIconATex,
    // Push down array to reach newly added item IDs
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // ITEM_CUSTOM
    // Start custom items
    gRocsFeatherTex,
};

// Used to map item IDs to inventory slots
u8 gItemSlots[] = {
    SLOT_STICK,       SLOT_NUT,          SLOT_BOMB,        SLOT_BOW,         SLOT_ARROW_FIRE,  SLOT_DINS_FIRE,
    SLOT_SLINGSHOT,   SLOT_OCARINA,      SLOT_OCARINA,     SLOT_BOMBCHU,     SLOT_HOOKSHOT,    SLOT_HOOKSHOT,
    SLOT_ARROW_ICE,   SLOT_FARORES_WIND, SLOT_BOOMERANG,   SLOT_LENS,        SLOT_BEAN,        SLOT_HAMMER,
    SLOT_ARROW_LIGHT, SLOT_NAYRUS_LOVE,  SLOT_BOTTLE_1,    SLOT_BOTTLE_1,    SLOT_BOTTLE_1,    SLOT_BOTTLE_1,
    SLOT_BOTTLE_1,    SLOT_BOTTLE_1,     SLOT_BOTTLE_1,    SLOT_BOTTLE_1,    SLOT_BOTTLE_1,    SLOT_BOTTLE_1,
    SLOT_BOTTLE_1,    SLOT_BOTTLE_1,     SLOT_BOTTLE_1,    SLOT_TRADE_CHILD, SLOT_TRADE_CHILD, SLOT_TRADE_CHILD,
    SLOT_TRADE_CHILD, SLOT_TRADE_CHILD,  SLOT_TRADE_CHILD, SLOT_TRADE_CHILD, SLOT_TRADE_CHILD, SLOT_TRADE_CHILD,
    SLOT_TRADE_CHILD, SLOT_TRADE_CHILD,  SLOT_TRADE_CHILD, SLOT_TRADE_ADULT, SLOT_TRADE_ADULT, SLOT_TRADE_ADULT,
    SLOT_TRADE_ADULT, SLOT_TRADE_ADULT,  SLOT_TRADE_ADULT, SLOT_TRADE_ADULT, SLOT_TRADE_ADULT, SLOT_TRADE_ADULT,
    SLOT_TRADE_ADULT, SLOT_TRADE_ADULT,
};

void Inventory_ChangeEquipment(s16 equipment, u16 value) {
    gSaveContext.equips.equipment &= gEquipNegMasks[equipment];
    gSaveContext.equips.equipment |= value << gEquipShifts[equipment];

    GameInteractor_ExecuteOnLinkEquipmentChange();
}

u8 Inventory_DeleteEquipment(PlayState* play, s16 equipment) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    u16 equipValue = gSaveContext.equips.equipment & gEquipMasks[equipment];

    // "Erasing equipment item = %d  zzz=%d"
    osSyncPrintf("装備アイテム抹消 = %d  zzz=%d\n", equipment, equipValue);

    if (equipValue) {
        equipValue >>= gEquipShifts[equipment];

        gSaveContext.equips.equipment &= gEquipNegMasks[equipment];
        gSaveContext.inventory.equipment ^= OWNED_EQUIP_FLAG(equipment, equipValue - 1);

        if (equipment == EQUIP_TYPE_TUNIC) {
            gSaveContext.equips.equipment |= EQUIP_VALUE_TUNIC_KOKIRI << (EQUIP_TYPE_TUNIC * 4);
        }

        GameInteractor_ExecuteOnEquipmentDelete(equipment, equipValue);

        if (equipment == EQUIP_TYPE_SWORD) {
            gSaveContext.equips.buttonItems[0] = ITEM_NONE;
            gSaveContext.infTable[29] = 1;
        }

        Player_SetEquipmentData(play, player);
        play->pauseCtx.cursorSpecialPos = PAUSE_CURSOR_PAGE_LEFT;
    }

    return equipValue;
}

void Inventory_ChangeUpgrade(s16 upgrade, s16 value) {
    gSaveContext.inventory.upgrades &= gUpgradeNegMasks[upgrade];
    gSaveContext.inventory.upgrades |= value << gUpgradeShifts[upgrade];
}
