/*
 * File: z_kaleido_scope_NES.c
 * Overlay: ovl_kaleido_scope
 * Description: Pause Menu
 */

#include "z_kaleido_scope.h"
#include "z64map.h"
#include "z64skybox.h"
#include "z64view.h"
#include "overlays/gamestates/ovl_opening/z_opening.h"

#include "archives/icon_item_static/icon_item_static_yar.h"
#include "interface/icon_item_gameover_static/icon_item_gameover_static.h"
#include "interface/icon_item_jpn_static/icon_item_jpn_static.h"
#include "interface/icon_item_vtx_static/icon_item_vtx_static.h"
#include "BenPort.h"
#include <libultraship/bridge/gfxdebuggerbridge.h>
#include "archives/item_name_static/item_name_static.h"
#include "archives/map_name_static/map_name_static.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/Enhancements/Saving/SavingEnhancements.h"
#include "2s2h/BenGui/CosmeticEditor.h"

#include "2s2h_assets.h"

#include "2s2h/GameInteractor/GameInteractor.h"

#include "2s2h/Enhancements/Songs/Songs.h"
#include <libultraship/bridge/consolevariablebridge.h>

#pragma increment_block_number "n64-us:128"

// Page Textures (Background of Page):
// Broken up into multiple textures.
// Numbered by column/row.
TexturePtr sMaskPageBgTextures[] = {
    // Column 0
    gPauseMasks00Tex,
    gPauseMasks01Tex,
    gPauseMasks02Tex,
    gPauseMasks03Tex,
    gPauseMasks04Tex,
    // Column 1
    gPauseMasks10ENGTex,
    gPauseMasks11Tex,
    gPauseMasks12Tex,
    gPauseMasks13Tex,
    gPauseMasks14Tex,
    // Column 2
    gPauseMasks20Tex,
    gPauseMasks21Tex,
    gPauseMasks22Tex,
    gPauseMasks23Tex,
    gPauseMasks24Tex,
};
TexturePtr sItemPageBgTextures[] = {
    // Column 0
    gPauseSelectItem00ENGTex,
    gPauseSelectItem01Tex,
    gPauseSelectItem02Tex,
    gPauseSelectItem03Tex,
    gPauseSelectItem04Tex,
    // Column 1
    gPauseSelectItem10ENGTex,
    gPauseSelectItem11Tex,
    gPauseSelectItem12Tex,
    gPauseSelectItem13Tex,
    gPauseSelectItem14Tex,
    // Column 2
    gPauseSelectItem20ENGTex,
    gPauseSelectItem21Tex,
    gPauseSelectItem22Tex,
    gPauseSelectItem23Tex,
    gPauseSelectItem24Tex,
};
TexturePtr sMapPageBgTextures[] = {
    // Column 0
    gPauseMap00Tex,
    gPauseMap01Tex,
    gPauseMap02Tex,
    gPauseMap03Tex,
    gPauseMap04Tex,
    // Column 1
    gPauseMap10ENGTex,
    gPauseMap11Tex,
    gPauseMap12Tex,
    gPauseMap13Tex,
    gPauseMap14Tex,
    // Column 2
    gPauseMap20Tex,
    gPauseMap21Tex,
    gPauseMap22Tex,
    gPauseMap23Tex,
    gPauseMap24Tex,
};
TexturePtr sQuestPageBgTextures[] = {
    // Column 0
    gPauseQuestStatus00ENGTex,
    gPauseQuestStatus01Tex,
    gPauseQuestStatus02Tex,
    gPauseQuestStatus03Tex,
    gPauseQuestStatus04Tex,
    // Column 1
    gPauseQuestStatus10ENGTex,
    gPauseQuestStatus11Tex,
    gPauseQuestStatus12Tex,
    gPauseQuestStatus13Tex,
    gPauseQuestStatus14Tex,
    // Column 2
    gPauseQuestStatus20ENGTex,
    gPauseQuestStatus21Tex,
    gPauseQuestStatus22Tex,
    gPauseQuestStatus23Tex,
    gPauseQuestStatus24Tex,
};

s16 gVtxPageMapWorldQuadsWidth[VTX_PAGE_MAP_WORLD_QUADS] = {
    80,  // mapPageVtx[60]  clouds Clock Town 1
    64,  // mapPageVtx[64]  clouds Clock Town 2
    64,  // mapPageVtx[68]  clouds Woodfall 1
    64,  // mapPageVtx[72]  clouds Woodfall 2
    16,  // mapPageVtx[76]  clouds Woodfall 3
    64,  // mapPageVtx[80]  clouds Snowhead 1
    112, // mapPageVtx[84]  clouds Snowhead 2
    112, // mapPageVtx[88]  clouds Snowhead 3
    48,  // mapPageVtx[92]  clouds Romani Ranch
    64,  // mapPageVtx[96]  clouds Great Bay 1
    64,  // mapPageVtx[100] clouds Great Bay 2
    64,  // mapPageVtx[104] clouds Great Bay 3
    64,  // mapPageVtx[108] clouds Great Bay 4
    48,  // mapPageVtx[112] clouds Stone Tower 1
    64,  // mapPageVtx[116] clouds Stone Tower 2
    8,   // mapPageVtx[120] region Great Bay
    8,   // mapPageVtx[124] region Zora Hall
    8,   // mapPageVtx[128] region Romani Ranch
    8,   // mapPageVtx[132] region Deku Palace
    8,   // mapPageVtx[136] region Woodfall
    8,   // mapPageVtx[140] region Clock Town
    8,   // mapPageVtx[144] region Snowhead
    8,   // mapPageVtx[148] region Ikana Graveyard
    8,   // mapPageVtx[152] region Ikana Canyon
    8,   // mapPageVtx[156] region Goron Village
    8,   // mapPageVtx[160] region Stone Tower
    24,  // mapPageVtx[164] owl warp Great Bay Coast
    24,  // mapPageVtx[168] owl warp Zora Cape
    24,  // mapPageVtx[172] owl warp Snowhead
    24,  // mapPageVtx[176] owl warp Mountain Village
    24,  // mapPageVtx[180] owl warp Clock Town
    24,  // mapPageVtx[184] owl warp Milk Road
    24,  // mapPageVtx[188] owl warp Woodfall
    24,  // mapPageVtx[192] owl warp Southern Swamp
    24,  // mapPageVtx[196] owl warp Ikana Canyon
    24,  // mapPageVtx[200] owl warp Stone Tower
};
s16 gVtxPageMapWorldQuadsHeight[VTX_PAGE_MAP_WORLD_QUADS] = {
    40, // mapPageVtx[60]  clouds Clock Town 1
    31, // mapPageVtx[64]  clouds Clock Town 2
    53, // mapPageVtx[68]  clouds Woodfall 1
    53, // mapPageVtx[72]  clouds Woodfall 2
    53, // mapPageVtx[76]  clouds Woodfall 3
    52, // mapPageVtx[80]  clouds Snowhead 1
    35, // mapPageVtx[84]  clouds Snowhead 2
    35, // mapPageVtx[88]  clouds Snowhead 3
    32, // mapPageVtx[92]  clouds Romani Ranch
    64, // mapPageVtx[96]  clouds Great Bay 1
    64, // mapPageVtx[100] clouds Great Bay 2
    64, // mapPageVtx[104] clouds Great Bay 3
    64, // mapPageVtx[108] clouds Great Bay 4
    82, // mapPageVtx[112] clouds Stone Tower 1
    61, // mapPageVtx[116] clouds Stone Tower 2
    8,  // mapPageVtx[120] region Great Bay
    8,  // mapPageVtx[124] region Zora Hall
    8,  // mapPageVtx[128] region Romani Ranch
    8,  // mapPageVtx[132] region Deku Palace
    8,  // mapPageVtx[136] region Woodfall
    8,  // mapPageVtx[140] region Clock Town
    8,  // mapPageVtx[144] region Snowhead
    8,  // mapPageVtx[148] region Ikana Graveyard
    8,  // mapPageVtx[152] region Ikana Canyon
    8,  // mapPageVtx[156] region Goron Village
    8,  // mapPageVtx[160] region Stone Tower
    12, // mapPageVtx[164] owl warp Great Bay Coast
    12, // mapPageVtx[168] owl warp Zora Cape
    12, // mapPageVtx[172] owl warp Snowhead
    12, // mapPageVtx[176] owl warp Mountain Village
    12, // mapPageVtx[180] owl warp Clock Town
    12, // mapPageVtx[184] owl warp Milk Road
    12, // mapPageVtx[188] owl warp Woodfall
    12, // mapPageVtx[192] owl warp Southern Swamp
    12, // mapPageVtx[196] owl warp Ikana Canyon
    12, // mapPageVtx[200] owl warp Stone Tower
};

s16 sGameOverPrimR = 0;
s16 sGameOverPrimG = 0;
s16 sGameOverPrimB = 0;
s16 sGameOverPrimAlpha = 255;
s16 sCursorPrimR = 0;
s16 sCursorPrimG = 0;
s16 sCursorPrimB = 0;
s16 sCursorEnvR = 0;
s16 sCursorEnvG = 0;
s16 sCursorEnvB = 0;
s16 sGameOverEnvR = 255;
s16 sGameOverEnvG = 0;
s16 sGameOverEnvB = 0;

s16 sInDungeonScene = false;

f32 sPageSwitchEyeDx[] = {
    -PAUSE_EYE_DIST * (PAUSE_MAP_X - PAUSE_ITEM_X) / 16, // From PAUSE_ITEM to PAUSE_MAP (switching right)
    -PAUSE_EYE_DIST*(PAUSE_MASK_X - PAUSE_ITEM_X) / 16,  // From PAUSE_ITEM to PAUSE_MASK (switching left)
    -PAUSE_EYE_DIST*(PAUSE_QUEST_X - PAUSE_MAP_X) / 16,  // From PAUSE_MAP to PAUSE_QUEST (switching right)
    -PAUSE_EYE_DIST*(PAUSE_ITEM_X - PAUSE_MAP_X) / 16,   // From PAUSE_MAP to PAUSE_ITEM (switching left)
    -PAUSE_EYE_DIST*(PAUSE_MASK_X - PAUSE_QUEST_X) / 16, // From PAUSE_QUEST to PAUSE_MASK (switching right)
    -PAUSE_EYE_DIST*(PAUSE_MAP_X - PAUSE_QUEST_X) / 16,  // From PAUSE_QUEST to PAUSE_MAP (switching left)
    -PAUSE_EYE_DIST*(PAUSE_ITEM_X - PAUSE_MASK_X) / 16,  // From PAUSE_MASK to PAUSE_ITEM (switching right)
    -PAUSE_EYE_DIST*(PAUSE_QUEST_X - PAUSE_MASK_X) / 16, // From PAUSE_MASK to PAUSE_QUEST (switching left)
};

f32 sPageSwitchEyeDz[] = {
    -PAUSE_EYE_DIST * (PAUSE_MAP_Z - PAUSE_ITEM_Z) / 16, // From PAUSE_ITEM to PAUSE_MAP (switching right)
    -PAUSE_EYE_DIST*(PAUSE_MASK_Z - PAUSE_ITEM_Z) / 16,  // From PAUSE_ITEM to PAUSE_MASK (switching left)
    -PAUSE_EYE_DIST*(PAUSE_QUEST_Z - PAUSE_MAP_Z) / 16,  // From PAUSE_MAP to PAUSE_QUEST (switching right)
    -PAUSE_EYE_DIST*(PAUSE_ITEM_Z - PAUSE_MAP_Z) / 16,   // From PAUSE_MAP to PAUSE_ITEM (switching left)
    -PAUSE_EYE_DIST*(PAUSE_MASK_Z - PAUSE_QUEST_Z) / 16, // From PAUSE_QUEST to PAUSE_MASK (switching right)
    -PAUSE_EYE_DIST*(PAUSE_MAP_Z - PAUSE_QUEST_Z) / 16,  // From PAUSE_QUEST to PAUSE_MAP (switching left)
    -PAUSE_EYE_DIST*(PAUSE_ITEM_Z - PAUSE_MASK_Z) / 16,  // From PAUSE_MASK to PAUSE_ITEM (switching right)
    -PAUSE_EYE_DIST*(PAUSE_QUEST_Z - PAUSE_MASK_Z) / 16, // From PAUSE_MASK to PAUSE_QUEST (switching left)
};

u16 sPageSwitchNextPageIndex[] = {
    PAUSE_MAP,   // From PAUSE_ITEM (switching right)
    PAUSE_MASK,  // From PAUSE_ITEM (switching left)
    PAUSE_QUEST, // From PAUSE_MAP (switching right)
    PAUSE_ITEM,  // From PAUSE_MAP (switching left)
    PAUSE_MASK,  // From PAUSE_QUEST (switching right)
    PAUSE_MAP,   // From PAUSE_QUEST (switching left)
    PAUSE_ITEM,  // From PAUSE_MASK (switching right)
    PAUSE_QUEST, // From PAUSE_MASK (switching left)
};

f32 sPauseMenuVerticalOffset = 0.0f;
f32 D_8082B90C = 0.0f;
f32 sPauseCursorLeftMoveOffsetX = 40.0f;
f32 sPauseCursorRightMoveOffsetX = -40.0f;

s16 sPauseCursorLeftX = 0;
s16 sPauseCursorRightX = 0;

s16 D_8082B920 = 10;

s16 sPauseZRCursorColorTimerInits[] = { 20, 4, 20, 10 };

// Unused remnant of OoT
u8 gAreaGsFlags[] = {
    0x0F, 0x1F, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x07, 0x07, 0x03, 0x0F,
    0x07, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0x1F, 0x0F, 0x03, 0x0F, 0x00, 0x00,
};

// TODO: Also applies to owl warps
s16 sGameOverRectPosY = 66;

void Kaleido_LoadMapNameStatic(void** segment, u32 texIndex) {
    static const char* gMapNameStatics[] = {
        gMapPointGreatBayENGTex,      gMapPointZoraHallENGTex,        gMapPointRomaniRanchENGTex,
        gMapPointDekuPalaceENGTex,    gMapPointWoodfallENGTex,        gMapPointClockTownENGTex,
        gMapPointSnowheadENGTex,      gMapPointIkanaGraveyardENGTex,  gMapPointIkanaCanyonENGTex,
        gMapPointGoronVillageENGTex,  gMapPointStoneTowerENGTex,      gMapPointGreatBayCoastENGTex,
        gMapPointSouthernSwampENGTex, gMapPointMountainVillageENGTex, gMapPointMilkRoadENGTex,
        gMapPointZoraCapeENGTex,
    };

    // 2S2H [Port] Bounds check texture to load to prevent crashes
    if (texIndex < ARRAY_COUNTU(gMapNameStatics)) {
        *segment = gMapNameStatics[texIndex];
    } else {
        *segment = gEmptyTexture;
    }

    // CmpDma_LoadFile(SEGMENT_ROM_START(map_name_static), texIndex, segment, 0x400);
}

//! note: nothing from `map_name_static` is of size `0xA00` in US 1.0
void Kaleido_LoadMapNameStaticLarge(void** segment, u32 texIndex) {
    // 2S2H [Port] Just call the normal map name func since the textures are the same
    Kaleido_LoadMapNameStatic(segment, texIndex);

    // CmpDma_LoadFile(SEGMENT_ROM_START(map_name_static), texIndex, segment, 0xA00);
}

void Kaleido_LoadItemNameStatic(void** segment, u32 texIndex) {
    static const char* gItemNameStatics[] = {
        gItemNameOcarinaOfTimeENGTex,
        gItemNameHerosBowENGTex,
        gItemNameFireArrowENGTex,
        gItemNameIceArrowENGTex,
        gItemNameLightArrowENGTex,
        gItemNameFairyOcarinaJPNTex,
        gItemNameBombENGTex,
        gItemNameBombchuENGTex,
        gItemNameDekuStickENGTex,
        gItemNameDekuNutENGTex,
        gItemNameMagicBeansENGTex,
        gItemNameLongshotJPNTex,
        gItemNamePowderKegENGTex,
        gItemNamePictographBoxENGTex,
        gItemNameLensOfTruthENGTex,
        gItemNameHookshotENGTex,
        gItemNameGreatFairysSwordENGTex,
        gItemNameFairySlingshotJPNTex,
        gItemNameEmptyBottleENGTex,
        gItemNameRedPotionENGTex,
        gItemNameGreenPotionENGTex,
        gItemNameBluePotionENGTex,
        gItemNameFairyENGTex,
        gItemNameDekuPrincessENGTex,
        gItemNameFullMilkENGTex,
        gItemNameHalfMilkENGTex,
        gItemNameFishENGTex,
        gItemNameBugENGTex,
        gItemNameBlueFireENGTex,
        gItemNamePoeENGTex,
        gItemNameBigPoeENGTex,
        gItemNameSpringWaterENGTex,
        gItemNameHotSpringWaterENGTex,
        gItemNameZoraEggENGTex,
        gItemNameGoldDustENGTex,
        gItemNameMagicalMushroomENGTex,
        gItemNameSeaHorseENGTex,
        gItemNameChateauRomaniENGTex,
        gItemNameHylianLoachJPNTex,
        gItemNameObabasDrinkJPNTex,
        gItemNameMoonsTearENGTex,
        gItemNameLandTitleDeedENGTex,
        gItemNameSwampTitleDeedENGTex,
        gItemNameMountainTitleDeedENGTex,
        gItemNameOceanTitleDeedENGTex,
        gItemNameRoomKeyENGTex,
        gItemNameSpecialDeliveryToMamaENGTex,
        gItemNameLetterToKafeiENGTex,
        gItemNamePendantOfMemoriesENGTex,
        gItemNameMoonsStoneJPNTex,
        gItemNameDekuMaskENGTex,
        gItemNameGoronMaskENGTex,
        gItemNameZoraMaskENGTex,
        gItemNameFierceDeitysMaskENGTex,
        gItemNameMaskOfTruthENGTex,
        gItemNameKafeisMaskENGTex,
        gItemNameAllNightMaskENGTex,
        gItemNameBunnyHoodENGTex,
        gItemNameKeatonMaskENGTex,
        gItemNameGarosMaskENGTex,
        gItemNameRomanisMaskENGTex,
        gItemNameCircusLeadersMaskENGTex,
        gItemNamePostmansHatENGTex,
        gItemNameCouplesMaskENGTex,
        gItemNameGreatFairysMaskENGTex,
        gItemNameGibdoMaskENGTex,
        gItemNameDonGerosMaskENGTex,
        gItemNameKamarosMaskENGTex,
        gItemNameCaptainsHatENGTex,
        gItemNameStoneMaskENGTex,
        gItemNameBremenMaskENGTex,
        gItemNameBlastMaskENGTex,
        gItemNameMaskOfScentsENGTex,
        gItemNameGiantsMaskENGTex,
        gItemNameWindMedallionJPNTex,
        gItemNameFireMedallionJPNTex,
        gItemNameIceMedallionJPNTex,
        gItemNameKokiriSwordENGTex,
        gItemNameRazorSwordENGTex,
        gItemNameGildedSwordENGTex,
        gItemNameBrokenGiantsKnifeJPNTex,
        gItemNameHerosShieldENGTex,
        gItemNameMirrorShieldENGTex,
        gItemNameQuiver30ENGTex,
        gItemNameQuiver40ENGTex,
        gItemNameQuiver50ENGTex,
        gItemNameBombBag20ENGTex,
        gItemNameBombBag30ENGTex,
        gItemNameBombBag40ENGTex,
        gItemNameBigKey1JPNTex,
        gItemNameBigKey2JPNTex,
        gItemNameBigKey3JPNTex,
        gItemNameBigKey4JPNTex,
        gItemNameOdolwasRemainsENGTex,
        gItemNameGohtsRemainsENGTex,
        gItemNameGyorgsRemainsENGTex,
        gItemNameTwinmoldsRemainsENGTex,
        gItemNameSonataOfAwakeningENGTex,
        gItemNameGoronLullabyENGTex,
        gItemNameNewWaveBossaNovaENGTex,
        gItemNameElegyOfEmptynessENGTex,
        gItemNameOathToOrderENGTex,
        gItemNameNocturneOfShadowJPNTex,
        gItemNameSongOfTimeENGTex,
        gItemNameSongOfHealingENGTex,
        gItemNameEponasSongENGTex,
        gItemNameSongOfSoaringENGTex,
        gItemNameSongOfStormsENGTex,
        gItemNameSunsSongJPNTex,
        gItemNameBombersNotebookENGTex,
        gItemNameGoldSkulltulaJPNTex,
        gItemNamePieceOfHeartENGTex,
        gItemNamePieceOfHeartJPNTex,
        gItemNameSunsSong2JPNTex,
        gItemNameSongOfTimeJPNTex,
        gItemNameLullabyIntroENGTex,
        gItemNameBigKeyENGTex,
        gItemNameCompassENGTex,
        gItemNameDungeonMapENGTex,
        gItemNameStrayFairiesENGTex,
    };

    // 2S2H [Port] Bounds check texture to load to prevent crashes
    if (texIndex < ARRAY_COUNTU(gItemNameStatics)) {
        *segment = gItemNameStatics[texIndex];
    } else {
        *segment = gEmptyTexture;
    }

    // CmpDma_LoadFile(SEGMENT_ROM_START(item_name_static), texIndex, segment, 0x400);
}

void KaleidoScope_MoveCursorToSpecialPos(PlayState* play, s16 cursorSpecialPos) {
    PauseContext* pauseCtx = &play->pauseCtx;

    pauseCtx->cursorSlot[pauseCtx->pageIndex] = 0;
    pauseCtx->cursorSpecialPos = cursorSpecialPos;
    pauseCtx->pageSwitchInputTimer = 0;

    Audio_PlaySfx(NA_SE_SY_DECIDE);

    gSaveContext.buttonStatus[EQUIP_SLOT_B] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = BTN_DISABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = BTN_DISABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = BTN_DISABLED;
    // #region 2S2H [Dpad]
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] = BTN_DISABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT] = BTN_DISABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN] = BTN_DISABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP] = BTN_DISABLED;
    // #endregion
    gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_DISABLED;

    gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
}

void KaleidoScope_MoveCursorFromSpecialPos(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;

    pauseCtx->nameDisplayTimer = 0;
    pauseCtx->cursorSpecialPos = 0;

    Audio_PlaySfx(NA_SE_SY_CURSOR);

    pauseCtx->cursorShrinkRate = 4.0f;

    gSaveContext.buttonStatus[EQUIP_SLOT_B] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
    gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
    gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
    // #region 2S2H [Dpad]
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] =
        gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT] =
        gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN] =
        gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP] = gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
    // #endregion
    gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;

    gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;

    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
}

void KaleidoScope_DrawTexQuadRGBA32(GraphicsContext* gfxCtx, TexturePtr texture, u16 width, u16 height, u16 point) {
    OPEN_DISPS(gfxCtx);

    gDPLoadTextureBlock(POLY_OPA_DISP++, texture, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, height, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, point, point + 2, point + 3, point + 1, 0);

    CLOSE_DISPS(gfxCtx);
}

#define SWITCH_PAGE_LEFT 0
#define SWITCH_PAGE_RIGHT 2

void KaleidoScope_SwitchPage(PauseContext* pauseCtx, u8 direction) {
    pauseCtx->cursorSlot[pauseCtx->pageIndex] = 0;
    pauseCtx->mainState = PAUSE_MAIN_STATE_SWITCHING_PAGE;
    pauseCtx->switchPageTimer = 0;

    if (direction == SWITCH_PAGE_LEFT) {
        pauseCtx->nextPageMode = (pauseCtx->pageIndex * 2) + 1;
        Audio_PlaySfx(NA_SE_SY_WIN_SCROLL_LEFT);
        pauseCtx->cursorSpecialPos = PAUSE_CURSOR_PAGE_RIGHT;
    } else { // SWITCH_PAGE_RIGHT
        pauseCtx->nextPageMode = pauseCtx->pageIndex * 2;
        Audio_PlaySfx(NA_SE_SY_WIN_SCROLL_RIGHT);
        pauseCtx->cursorSpecialPos = PAUSE_CURSOR_PAGE_LEFT;
    }

    gSaveContext.buttonStatus[EQUIP_SLOT_B] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = BTN_DISABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = BTN_DISABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = BTN_DISABLED;
    // #region 2S2H [Dpad]
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] = BTN_DISABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT] = BTN_DISABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN] = BTN_DISABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP] = BTN_DISABLED;
    // #endregion
    gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_DISABLED;

    gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
}

/**
 * Process inputs to decide whether to switch pages
 */
void KaleidoScope_HandlePageToggles(PlayState* play, Input* input) {
    PauseContext* pauseCtx = &play->pauseCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    // 2S2H [Debug] Restoring input check for debug inventory editor based on OOT debug
    if (CVarGetInteger("gDeveloperTools.DebugEnabled", 0) && (pauseCtx->debugEditor == DEBUG_EDITOR_NONE) &&
        CHECK_BTN_ALL(input->press.button, BTN_L)) {
        pauseCtx->debugEditor = DEBUG_EDITOR_INVENTORY_INIT;
        return;
    }

    //! FAKE
    if (1) {}

    if ((pauseCtx->debugEditor != DEBUG_EDITOR_NONE) || pauseCtx->itemDescriptionOn) {
        return;
    }

    if (CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT) || CHECK_BTN_ALL(input->press.button, BTN_R)) {
        if (!GameInteractor_Should(VB_KALEIDO_SWITCH_PAGE_WITH_DPAD, true, BTN_DRIGHT)) {
            return;
        }
        // Switch the page to the right regardless of where the cursor is
        if (interfaceCtx->aButtonDoActionDelayed == DO_ACTION_DECIDE) {
            Interface_SetAButtonDoAction(play, DO_ACTION_INFO);
        }
        KaleidoScope_SwitchPage(pauseCtx, SWITCH_PAGE_RIGHT);
        return;
    }

    if (CHECK_BTN_ALL(input->cur.button, BTN_DLEFT) || CHECK_BTN_ALL(input->press.button, BTN_Z)) {
        if (!GameInteractor_Should(VB_KALEIDO_SWITCH_PAGE_WITH_DPAD, true, BTN_DLEFT)) {
            return;
        }
        // Switch the page to the left regardless of where the cursor is
        if (interfaceCtx->aButtonDoActionDelayed == DO_ACTION_DECIDE) {
            Interface_SetAButtonDoAction(play, DO_ACTION_INFO);
        }
        KaleidoScope_SwitchPage(pauseCtx, SWITCH_PAGE_LEFT);
        return;
    }

    if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
        if (pauseCtx->stickAdjX < -30) {
            pauseCtx->pageSwitchInputTimer++;
            // Switch the page to the left after a certain number of frames with held input or after a second press
            if ((pauseCtx->pageSwitchInputTimer >= 10) || (pauseCtx->pageSwitchInputTimer == 0)) {
                KaleidoScope_SwitchPage(pauseCtx, SWITCH_PAGE_LEFT);
            }
        } else {
            // stickAdjX is no longer held, so that the next input to the left will immediately switch pages
            pauseCtx->pageSwitchInputTimer = -1;
        }
    } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) {
        if (pauseCtx->stickAdjX > 30) {
            pauseCtx->pageSwitchInputTimer++;
            // Switch the page to the right after a certain number of frames with held input or after a second press
            if ((pauseCtx->pageSwitchInputTimer >= 10) || (pauseCtx->pageSwitchInputTimer == 0)) {
                KaleidoScope_SwitchPage(pauseCtx, SWITCH_PAGE_RIGHT);
            }
        } else {
            // stickAdjY is no longer held, so that the next input to the right will immediately switch pages
            pauseCtx->pageSwitchInputTimer = -1;
        }
    }
}

// Draw 15 (PAGE_BG_QUADS) quads with IA8 80x32 textures
Gfx* KaleidoScope_DrawPageSections(Gfx* gfx, Vtx* vertices, TexturePtr* textures) {
    s32 i;
    s32 j;

    gSPVertex(gfx++, vertices, 8 * 4, 0);

    i = 0;
    j = 0;
    // Draw first 8/15 background textures
    while (j < 32) {
        gDPPipeSync(gfx++);
        gDPLoadTextureBlock(gfx++, textures[i], G_IM_FMT_IA, G_IM_SIZ_8b, 80, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(gfx++, j, j + 2, j + 3, j + 1, 0);

        j += 4;
        i++;
    }

    gSPVertex(gfx++, vertices + (8 * 4), 7 * 4, 0);

    j = 0;
    // Draw last 7/15 background textures
    while (j < 28) {
        gDPPipeSync(gfx++);
        gDPLoadTextureBlock(gfx++, textures[i], G_IM_FMT_IA, G_IM_SIZ_8b, 80, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(gfx++, j, j + 2, j + 3, j + 1, 0);

        j += 4;
        i++;
    }

    return gfx;
}

s16 sCursorPrimColorTarget[][3] = {
    { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 0 }, { 255, 255, 0 }, { 100, 150, 255 }, { 100, 255, 255 },
};

s16 sCursorEnvColorTarget[][3] = {
    { 0, 0, 0 }, { 170, 170, 170 }, { 0, 0, 0 }, { 255, 160, 0 }, { 0, 0, 100 }, { 0, 150, 255 },
};

static TexturePtr sSaveTexs[] = {
    gPauseSave00Tex,    gPauseSave01Tex, gPauseSave02Tex, gPauseSave03Tex, gPauseSave04Tex,
    gPauseSave10JPNTex, gPauseSave11Tex, gPauseSave12Tex, gPauseSave13Tex, gPauseSave14Tex,
    gPauseSave20Tex,    gPauseSave21Tex, gPauseSave22Tex, gPauseSave23Tex, gPauseSave24Tex,
};

static TexturePtr sGameOverTexs[] = {
    gPauseSave00Tex,     gPauseSave01Tex, gPauseSave02Tex, gPauseSave03Tex, gPauseSave04Tex,
    gPauseGameOver10Tex, gPauseSave11Tex, gPauseSave12Tex, gPauseSave13Tex, gPauseSave14Tex,
    gPauseSave20Tex,     gPauseSave21Tex, gPauseSave22Tex, gPauseSave23Tex, gPauseSave24Tex,
};

void KaleidoScope_DrawPages(PlayState* play, GraphicsContext* gfxCtx) {
    static s16 sCursorColorTimer = 10;
    static s16 sCursorColorTargetIndex = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 stepR;
    s16 stepG;
    s16 stepB;

    FrameInterpolation_RecordOpenChild(NULL, pauseCtx->state + pauseCtx->pageIndex * 100);
    OPEN_DISPS(gfxCtx);

    if (!IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
        if (pauseCtx->state != PAUSE_STATE_SAVEPROMPT) {

            stepR =
                ABS_ALT(sCursorPrimR - sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0]) /
                sCursorColorTimer;
            stepG =
                ABS_ALT(sCursorPrimG - sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1]) /
                sCursorColorTimer;
            stepB =
                ABS_ALT(sCursorPrimB - sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2]) /
                sCursorColorTimer;

            if (sCursorPrimR >= sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0]) {
                sCursorPrimR -= stepR;
            } else {
                sCursorPrimR += stepR;
            }

            if (sCursorPrimG >= sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1]) {
                sCursorPrimG -= stepG;
            } else {
                sCursorPrimG += stepG;
            }

            if (sCursorPrimB >= sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2]) {
                sCursorPrimB -= stepB;
            } else {
                sCursorPrimB += stepB;
            }

            stepR =
                ABS_ALT(sCursorEnvR - sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0]) /
                sCursorColorTimer;
            stepG =
                ABS_ALT(sCursorEnvG - sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1]) /
                sCursorColorTimer;
            stepB =
                ABS_ALT(sCursorEnvB - sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2]) /
                sCursorColorTimer;

            if (sCursorEnvR >= sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0]) {
                sCursorEnvR -= stepR;
            } else {
                sCursorEnvR += stepR;
            }

            if (sCursorEnvG >= sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1]) {
                sCursorEnvG -= stepG;
            } else {
                sCursorEnvG += stepG;
            }

            if (sCursorEnvB >= sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2]) {
                sCursorEnvB -= stepB;
            } else {
                sCursorEnvB += stepB;
            }

            sCursorColorTimer--;
            if (sCursorColorTimer == 0) {
                sCursorPrimR = sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0];
                sCursorPrimG = sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1];
                sCursorPrimB = sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2];
                sCursorEnvR = sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0];
                sCursorEnvG = sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1];
                sCursorEnvB = sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2];
                sCursorColorTargetIndex ^= 1;
                sCursorColorTimer = 10;
            }
        }

        if ((pauseCtx->pageIndex != PAUSE_ITEM) && (pauseCtx->pageIndex != PAUSE_QUEST)) {
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0,
                              TEXEL0, 0, SHADE, 0);

            gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255, COSMETIC_ID("Menus.PausePanels"));

            Matrix_RotateYF(0.0f, MTXMODE_NEW);
            Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateXFApply(-pauseCtx->itemPageRoll / 100.0f);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

            POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->itemPageVtx, sItemPageBgTextures);

            GameInteractor_ExecuteBeforeKaleidoDrawPage(pauseCtx, PAUSE_ITEM);
            KaleidoScope_DrawItemSelect(play);
            GameInteractor_ExecuteAfterKaleidoDrawPage(pauseCtx, PAUSE_ITEM);
        }

        if ((pauseCtx->pageIndex != PAUSE_MAP) && (pauseCtx->pageIndex != PAUSE_MASK)) {
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0,
                              TEXEL0, 0, SHADE, 0);

            gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255, COSMETIC_ID("Menus.PausePanels"));

            Matrix_RotateYF(-1.57f, MTXMODE_NEW);
            Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateXFApply(-pauseCtx->mapPageRoll / 100.0f);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

            POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->mapPageVtx, sMapPageBgTextures);

            GameInteractor_ExecuteBeforeKaleidoDrawPage(pauseCtx, PAUSE_MAP);
            if (sInDungeonScene) {
                KaleidoScope_DrawDungeonMap(play);
                Gfx_SetupDL42_Opa(gfxCtx);
                gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                MapDisp_DrawDungeonMap(play);
            } else {
                KaleidoScope_DrawWorldMap(play);
            }
            GameInteractor_ExecuteAfterKaleidoDrawPage(pauseCtx, PAUSE_MAP);
        }

        if ((pauseCtx->pageIndex != PAUSE_QUEST) && (pauseCtx->pageIndex != PAUSE_ITEM)) {
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

            gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0,
                              TEXEL0, 0, SHADE, 0);

            gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255, COSMETIC_ID("Menus.PausePanels"));

            Matrix_RotateYF(-3.14f, MTXMODE_NEW);
            Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateXFApply(-pauseCtx->questPageRoll / 100.0f);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

            POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->questPageVtx, sQuestPageBgTextures);

            GameInteractor_ExecuteBeforeKaleidoDrawPage(pauseCtx, PAUSE_QUEST);
            KaleidoScope_DrawQuestStatus(play);
            GameInteractor_ExecuteAfterKaleidoDrawPage(pauseCtx, PAUSE_QUEST);
        }

        if ((pauseCtx->pageIndex != PAUSE_MASK) && (pauseCtx->pageIndex != PAUSE_MAP)) {
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

            gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0,
                              TEXEL0, 0, SHADE, 0);

            gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255, COSMETIC_ID("Menus.PausePanels"));

            Matrix_RotateYF(1.57f, MTXMODE_NEW);
            Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateXFApply(-pauseCtx->maskPageRoll / 100.0f);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

            POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->maskPageVtx, sMaskPageBgTextures);

            GameInteractor_ExecuteBeforeKaleidoDrawPage(pauseCtx, PAUSE_MASK);
            KaleidoScope_DrawMaskSelect(play);
            GameInteractor_ExecuteAfterKaleidoDrawPage(pauseCtx, PAUSE_MASK);
        }

        switch (pauseCtx->pageIndex) {
            case PAUSE_ITEM:
                if (pauseCtx->mainState <= PAUSE_MAIN_STATE_EQUIP_MASK) {
                    gDPPipeSync(POLY_OPA_DISP++);

                    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0,
                                      PRIMITIVE, 0, TEXEL0, 0, SHADE, 0);

                    gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255,
                                            COSMETIC_ID("Menus.PausePanels"));

                    Matrix_RotateYF(0.0f, MTXMODE_NEW);
                    Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                    Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                    Matrix_RotateXFApply(-pauseCtx->itemPageRoll / 100.0f);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

                    POLY_OPA_DISP =
                        KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->itemPageVtx, sItemPageBgTextures);

                    GameInteractor_ExecuteBeforeKaleidoDrawPage(pauseCtx, pauseCtx->pageIndex);
                    KaleidoScope_DrawItemSelect(play);
                    GameInteractor_ExecuteAfterKaleidoDrawPage(pauseCtx, pauseCtx->pageIndex);
                }
                break;

            case PAUSE_MAP:
                gDPPipeSync(POLY_OPA_DISP++);

                gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE,
                                  0, TEXEL0, 0, SHADE, 0);

                gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255, COSMETIC_ID("Menus.PausePanels"));

                Matrix_RotateYF(-1.57f, MTXMODE_NEW);
                Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                Matrix_RotateXFApply(-pauseCtx->mapPageRoll / 100.0f);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

                POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->mapPageVtx, sMapPageBgTextures);

                GameInteractor_ExecuteBeforeKaleidoDrawPage(pauseCtx, pauseCtx->pageIndex);
                if (sInDungeonScene) {
                    KaleidoScope_DrawDungeonMap(play);
                    Gfx_SetupDL42_Opa(gfxCtx);

                    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

                    MapDisp_DrawDungeonMap(play);
                } else {
                    Matrix_RotateYF(R_PAUSE_WORLD_MAP_YAW / 1000.0f, MTXMODE_NEW);

                    if ((pauseCtx->state == PAUSE_STATE_OPENING_3) || (pauseCtx->state == PAUSE_STATE_OWL_WARP_3) ||
                        (pauseCtx->state >= PAUSE_STATE_OWL_WARP_6) ||
                        ((pauseCtx->state == PAUSE_STATE_SAVEPROMPT) &&
                         ((pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_3) ||
                          (pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_7)))) {
                        Matrix_Translate(0.0f, (R_PAUSE_WORLD_MAP_Y_OFFSET - 8000) / 100.0f,
                                         R_PAUSE_WORLD_MAP_DEPTH / 100.0f, MTXMODE_APPLY);
                    } else {
                        Matrix_Translate(0.0f, R_PAUSE_WORLD_MAP_Y_OFFSET / 100.0f, R_PAUSE_WORLD_MAP_DEPTH / 100.0f,
                                         MTXMODE_APPLY);
                    }

                    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
                    Matrix_RotateXFApply(-pauseCtx->mapPageRoll / 100.0f);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

                    KaleidoScope_DrawWorldMap(play);
                }
                GameInteractor_ExecuteAfterKaleidoDrawPage(pauseCtx, pauseCtx->pageIndex);
                break;

            case PAUSE_QUEST:
                gDPPipeSync(POLY_OPA_DISP++);

                gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE,
                                  0, TEXEL0, 0, SHADE, 0);

                gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255, COSMETIC_ID("Menus.PausePanels"));

                gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

                Matrix_RotateYF(-3.14f, MTXMODE_NEW);
                Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                Matrix_RotateXFApply(-pauseCtx->questPageRoll / 100.0f);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

                POLY_OPA_DISP =
                    KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->questPageVtx, sQuestPageBgTextures);

                GameInteractor_ExecuteBeforeKaleidoDrawPage(pauseCtx, pauseCtx->pageIndex);
                KaleidoScope_DrawQuestStatus(play);
                GameInteractor_ExecuteAfterKaleidoDrawPage(pauseCtx, pauseCtx->pageIndex);
                break;

            case PAUSE_MASK:
                gDPPipeSync(POLY_OPA_DISP++);

                gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE,
                                  0, TEXEL0, 0, SHADE, 0);

                gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255, COSMETIC_ID("Menus.PausePanels"));

                Matrix_RotateYF(1.57f, MTXMODE_NEW);
                Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                Matrix_RotateXFApply(-pauseCtx->maskPageRoll / 100.0f);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

                POLY_OPA_DISP =
                    KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->maskPageVtx, sMaskPageBgTextures);

                GameInteractor_ExecuteBeforeKaleidoDrawPage(pauseCtx, pauseCtx->pageIndex);
                KaleidoScope_DrawMaskSelect(play);
                GameInteractor_ExecuteAfterKaleidoDrawPage(pauseCtx, pauseCtx->pageIndex);
                break;
        }
    }

    if (CVarGetInteger("gEnhancements.Saving.PauseSave", 0) || CVarGetInteger("gEnhancements.Kaleido.GameOver", 0)) {
        Gfx_SetupDL42_Opa(gfxCtx);
        if ((pauseCtx->state == PAUSE_STATE_SAVEPROMPT) || IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
            KaleidoScope_UpdatePrompt(play);
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0,
                              TEXEL0, 0, SHADE, 0);
            gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255, COSMETIC_ID("Menus.PausePanels"));

            switch (pauseCtx->pageIndex) {
                case PAUSE_ITEM:
                    pauseCtx->itemPageRoll = pauseCtx->roll + 314.0f;
                    Matrix_RotateYF(0.0f, MTXMODE_NEW);
                    Matrix_Translate(0, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                    Matrix_Scale(0.78f, 0.78f, 0.78f, 1);
                    Matrix_RotateXFApply(-pauseCtx->roll / 100.0f);
                    break;
                case PAUSE_MAP:
                    pauseCtx->mapPageRoll = pauseCtx->roll + 314.0f;
                    Matrix_RotateYF(-1.57f, MTXMODE_NEW);
                    Matrix_Translate(0, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                    Matrix_Scale(0.78f, 0.78f, 0.78f, 1);
                    Matrix_RotateXFApply(-pauseCtx->roll / 100.0f);
                    break;
                case PAUSE_QUEST:
                    pauseCtx->questPageRoll = pauseCtx->roll + 314.0f;
                    Matrix_RotateYF(-3.14f, MTXMODE_NEW);
                    Matrix_Translate(0, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                    Matrix_Scale(0.78f, 0.78f, 0.78f, 1);
                    Matrix_RotateXFApply(-pauseCtx->roll / 100.0f);
                    break;
                case PAUSE_MASK:
                    pauseCtx->maskPageRoll = pauseCtx->roll + 314.0f;
                    Matrix_RotateYF(1.57f, MTXMODE_NEW);
                    Matrix_Translate(0, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                    Matrix_Scale(0.78f, 0.78f, 0.78f, 1);
                    Matrix_RotateXFApply(-pauseCtx->roll / 100.0f);
                    break;
            }
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

            if (IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
                POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->promptPageVtx, sGameOverTexs);
            } else {
                POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->promptPageVtx, sSaveTexs);
            }

            gSPVertex(POLY_OPA_DISP++, &pauseCtx->promptPageVtx[60], 32, 0);

            if (((pauseCtx->state == PAUSE_STATE_SAVEPROMPT) &&
                 ((pauseCtx->savePromptState <= PAUSE_SAVEPROMPT_STATE_4) ||
                  (pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_6))) ||
                (pauseCtx->state == PAUSE_STATE_GAMEOVER_SAVE_PROMPT)) {
                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
                POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, gPauseSavePromptENGTex, 152, 16, 0);
                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0,
                                  TEXEL0, 0, PRIMITIVE, 0);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 100, 100, 255, pauseCtx->promptAlpha);
                if (pauseCtx->promptChoice == PAUSE_PROMPT_YES) {
                    gSPDisplayList(POLY_OPA_DISP++, gPromptCursorLeftDL);
                } else {
                    gSPDisplayList(POLY_OPA_DISP++, gPromptCursorRightDL);
                }
                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
                POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, gPauseYesENGTex, 48, 16, 12);

                POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, gPauseNoENGTex, 48, 16, 16);

            } else if (((pauseCtx->state == PAUSE_STATE_SAVEPROMPT) &&
                        (pauseCtx->savePromptState >= PAUSE_SAVEPROMPT_STATE_5)) ||
                       (pauseCtx->state == PAUSE_STATE_GAMEOVER_8)) {
                POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, gPauseSaveConfirmationENGTex, 152, 16, 0);
            } else if ((pauseCtx->state == PAUSE_STATE_GAMEOVER_CONTINUE_PROMPT) ||
                       (pauseCtx->state == PAUSE_STATE_GAMEOVER_10)) {
                POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, gContinuePlayingENGTex, 152, 16, 0);
                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0,
                                  TEXEL0, 0, PRIMITIVE, 0);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 100, 100, 255, pauseCtx->promptAlpha);
                if (pauseCtx->promptChoice == PAUSE_PROMPT_YES) {
                    gSPDisplayList(POLY_OPA_DISP++, gPromptCursorLeftDL);
                } else {
                    gSPDisplayList(POLY_OPA_DISP++, gPromptCursorRightDL);
                }
                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
                POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, gPauseYesENGTex, 48, 16, 12);

                POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, gPauseNoENGTex, 48, 16, 16);
            }

            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

            if ((pauseCtx->state != PAUSE_STATE_GAMEOVER_CONTINUE_PROMPT) &&
                (pauseCtx->state != PAUSE_STATE_GAMEOVER_10)) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, pauseCtx->alpha);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
            }
        }
    }

    CLOSE_DISPS(gfxCtx);
    FrameInterpolation_RecordCloseChild();
}

TexturePtr D_8082B998[] = {
    gPauseToMasksENGTex,
    gPauseToSelectItemENGTex,
    gPauseToMapENGTex,
    gPauseToQuestStatusENGTex,
};
TexturePtr D_8082B9A8[] = {
    gPauseToMapENGTex,
    gPauseToQuestStatusENGTex,
    gPauseToMasksENGTex,
    gPauseToSelectItemENGTex,
};
void KaleidoScope_DrawInfoPanel(PlayState* play) {
    static s16 sPauseZRCursorColorTargets[][4] = {
        { 180, 210, 255, 220 },
        { 100, 100, 150, 220 },
    };
    static s16 sPauseZRCursorColorTimer = 20;
    static s16 sPauseZRCursorColorIndex = 0;
    static s16 sPauseZRCursorRed;
    static s16 sPauseZRCursorGreen;
    static s16 sPauseZRCursorBlue;
    static s16 sPauseZRCursorAlpha;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 stepR;
    s16 stepG;
    s16 stepB;
    s16 stepA;
    s16 y;
    s16 i;
    s16 j;

    OPEN_DISPS(play->state.gfxCtx);

    stepR =
        ABS_ALT(sPauseZRCursorRed - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][0]) / sPauseZRCursorColorTimer;
    stepG = ABS_ALT(sPauseZRCursorGreen - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][1]) /
            sPauseZRCursorColorTimer;
    stepB = ABS_ALT(sPauseZRCursorBlue - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][2]) /
            sPauseZRCursorColorTimer;
    stepA = ABS_ALT(sPauseZRCursorAlpha - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][3]) /
            sPauseZRCursorColorTimer;

    if (sPauseZRCursorRed >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][0]) {
        sPauseZRCursorRed -= stepR;
    } else {
        sPauseZRCursorRed += stepR;
    }

    if (sPauseZRCursorGreen >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][1]) {
        sPauseZRCursorGreen -= stepG;
    } else {
        sPauseZRCursorGreen += stepG;
    }

    if (sPauseZRCursorBlue >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][2]) {
        sPauseZRCursorBlue -= stepB;
    } else {
        sPauseZRCursorBlue += stepB;
    }

    if (sPauseZRCursorAlpha >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][3]) {
        sPauseZRCursorAlpha -= stepA;
    } else {
        sPauseZRCursorAlpha += stepA;
    }

    sPauseZRCursorColorTimer--;
    if (sPauseZRCursorColorTimer == 0) {
        sPauseZRCursorRed = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][0];
        sPauseZRCursorGreen = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][1];
        sPauseZRCursorBlue = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][2];
        sPauseZRCursorAlpha = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][3];
        sPauseZRCursorColorTimer = sPauseZRCursorColorTimerInits[0];
        sPauseZRCursorColorIndex ^= 1;
    }

    y = pauseCtx->infoPanelOffsetY - 76;
    for (j = 0, i = 0; i < 7; i++, j += 4) {
        pauseCtx->infoPanelVtx[j + 0].v.ob[0] = pauseCtx->infoPanelVtx[j + 2].v.ob[0] = -72;

        pauseCtx->infoPanelVtx[j + 1].v.ob[0] = pauseCtx->infoPanelVtx[j + 3].v.ob[0] = 0;

        pauseCtx->infoPanelVtx[j + 0].v.ob[1] = pauseCtx->infoPanelVtx[j + 1].v.ob[1] = y;

        pauseCtx->infoPanelVtx[j + 2].v.ob[1] = pauseCtx->infoPanelVtx[j + 3].v.ob[1] = y - 24;

        pauseCtx->infoPanelVtx[j + 0].v.ob[2] = pauseCtx->infoPanelVtx[j + 1].v.ob[2] =
            pauseCtx->infoPanelVtx[j + 2].v.ob[2] = pauseCtx->infoPanelVtx[j + 3].v.ob[2] = 0;

        pauseCtx->infoPanelVtx[j + 0].v.flag = pauseCtx->infoPanelVtx[j + 1].v.flag =
            pauseCtx->infoPanelVtx[j + 2].v.flag = pauseCtx->infoPanelVtx[j + 3].v.flag = 0;

        pauseCtx->infoPanelVtx[j + 0].v.tc[0] = pauseCtx->infoPanelVtx[j + 0].v.tc[1] =
            pauseCtx->infoPanelVtx[j + 1].v.tc[1] = pauseCtx->infoPanelVtx[j + 2].v.tc[0] = 0;

        pauseCtx->infoPanelVtx[j + 1].v.tc[0] = pauseCtx->infoPanelVtx[j + 3].v.tc[0] = 72 * (1 << 5);

        pauseCtx->infoPanelVtx[j + 2].v.tc[1] = pauseCtx->infoPanelVtx[j + 3].v.tc[1] = 24 * (1 << 5);

        pauseCtx->infoPanelVtx[j + 0].v.cn[0] = pauseCtx->infoPanelVtx[j + 2].v.cn[0] =
            pauseCtx->infoPanelVtx[j + 0].v.cn[1] = pauseCtx->infoPanelVtx[j + 2].v.cn[1] =
                pauseCtx->infoPanelVtx[j + 0].v.cn[2] = pauseCtx->infoPanelVtx[j + 2].v.cn[2] =
                    pauseCtx->infoPanelVtx[j + 1].v.cn[0] = pauseCtx->infoPanelVtx[j + 3].v.cn[0] =
                        pauseCtx->infoPanelVtx[j + 1].v.cn[1] = pauseCtx->infoPanelVtx[j + 3].v.cn[1] =
                            pauseCtx->infoPanelVtx[j + 1].v.cn[2] = pauseCtx->infoPanelVtx[j + 3].v.cn[2] = 200;

        pauseCtx->infoPanelVtx[j + 0].v.cn[3] = pauseCtx->infoPanelVtx[j + 2].v.cn[3] =
            pauseCtx->infoPanelVtx[j + 1].v.cn[3] = pauseCtx->infoPanelVtx[j + 3].v.cn[3] = pauseCtx->alpha;
    }

    pauseCtx->infoPanelVtx[4].v.ob[0] = pauseCtx->infoPanelVtx[6].v.ob[0] = pauseCtx->infoPanelVtx[0].v.ob[0] + 72;

    pauseCtx->infoPanelVtx[5].v.ob[0] = pauseCtx->infoPanelVtx[7].v.ob[0] = pauseCtx->infoPanelVtx[4].v.ob[0] + 72;

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {
        pauseCtx->infoPanelVtx[8].v.ob[0] = pauseCtx->infoPanelVtx[10].v.ob[0] = sPauseCursorLeftX;

        pauseCtx->infoPanelVtx[9].v.ob[0] = pauseCtx->infoPanelVtx[11].v.ob[0] = pauseCtx->infoPanelVtx[8].v.ob[0] + 24;

        pauseCtx->infoPanelVtx[8].v.ob[1] = pauseCtx->infoPanelVtx[9].v.ob[1] = D_8082B920;

        pauseCtx->infoPanelVtx[10].v.ob[1] = pauseCtx->infoPanelVtx[11].v.ob[1] =
            pauseCtx->infoPanelVtx[8].v.ob[1] - 32;
    } else {
        pauseCtx->infoPanelVtx[8].v.ob[0] = pauseCtx->infoPanelVtx[10].v.ob[0] = sPauseCursorLeftX + 3;

        pauseCtx->infoPanelVtx[9].v.ob[0] = pauseCtx->infoPanelVtx[11].v.ob[0] = pauseCtx->infoPanelVtx[8].v.ob[0] + 18;

        pauseCtx->infoPanelVtx[8].v.ob[1] = pauseCtx->infoPanelVtx[9].v.ob[1] = D_8082B920 - 3;

        pauseCtx->infoPanelVtx[10].v.ob[1] = pauseCtx->infoPanelVtx[11].v.ob[1] =
            pauseCtx->infoPanelVtx[8].v.ob[1] - 26;
    }

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {
        pauseCtx->infoPanelVtx[12].v.ob[0] = pauseCtx->infoPanelVtx[14].v.ob[0] = sPauseCursorRightX;

        pauseCtx->infoPanelVtx[13].v.ob[0] = pauseCtx->infoPanelVtx[15].v.ob[0] =
            pauseCtx->infoPanelVtx[12].v.ob[0] + 24;

        pauseCtx->infoPanelVtx[12].v.ob[1] = pauseCtx->infoPanelVtx[13].v.ob[1] = D_8082B920;

        pauseCtx->infoPanelVtx[14].v.ob[1] = pauseCtx->infoPanelVtx[15].v.ob[1] =
            pauseCtx->infoPanelVtx[12].v.ob[1] - 32;
    } else {
        pauseCtx->infoPanelVtx[12].v.ob[0] = pauseCtx->infoPanelVtx[14].v.ob[0] = sPauseCursorRightX + 3;

        pauseCtx->infoPanelVtx[13].v.ob[0] = pauseCtx->infoPanelVtx[15].v.ob[0] =
            pauseCtx->infoPanelVtx[12].v.ob[0] + 18;

        pauseCtx->infoPanelVtx[12].v.ob[1] = pauseCtx->infoPanelVtx[13].v.ob[1] = D_8082B920 - 3;

        pauseCtx->infoPanelVtx[14].v.ob[1] = pauseCtx->infoPanelVtx[15].v.ob[1] =
            pauseCtx->infoPanelVtx[12].v.ob[1] - 26;
    }

    pauseCtx->infoPanelVtx[9].v.tc[0] = pauseCtx->infoPanelVtx[11].v.tc[0] = pauseCtx->infoPanelVtx[13].v.tc[0] =
        pauseCtx->infoPanelVtx[15].v.tc[0] = 24 * (1 << 5);

    pauseCtx->infoPanelVtx[10].v.tc[1] = pauseCtx->infoPanelVtx[11].v.tc[1] = pauseCtx->infoPanelVtx[14].v.tc[1] =
        pauseCtx->infoPanelVtx[15].v.tc[1] = 32 * (1 << 5);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    Matrix_Translate(0.0f, 0.0f, -144.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 140, 90, 255);
    gSPVertex(POLY_OPA_DISP++, &pauseCtx->infoPanelVtx[0], 16, 0);

    gSPDisplayList(POLY_OPA_DISP++, gItemNamePanelDL);

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) &&
        (!pauseCtx->mainState || (pauseCtx->mainState == PAUSE_MAIN_STATE_UNK))) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 140, 90, sPauseZRCursorAlpha);
    }

    gSPDisplayList(POLY_OPA_DISP++, gZButtonIconDL);

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) &&
        (!pauseCtx->mainState || (pauseCtx->mainState == PAUSE_MAIN_STATE_UNK))) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 140, 90, sPauseZRCursorAlpha);
    }

    gSPDisplayList(POLY_OPA_DISP++, gRButtonIconDL);

    if (pauseCtx->cursorSpecialPos != 0) {
        j = (pauseCtx->cursorSpecialPos * 4) - 32;
        pauseCtx->cursorVtx[0].v.ob[0] = pauseCtx->infoPanelVtx[j].v.ob[0];
        pauseCtx->cursorVtx[0].v.ob[1] = pauseCtx->infoPanelVtx[j].v.ob[1];
    }

    y = pauseCtx->infoPanelOffsetY - 80;
    pauseCtx->infoPanelVtx[16].v.ob[1] = pauseCtx->infoPanelVtx[17].v.ob[1] = y;

    pauseCtx->infoPanelVtx[18].v.ob[1] = pauseCtx->infoPanelVtx[19].v.ob[1] = pauseCtx->infoPanelVtx[16].v.ob[1] - 16;

    pauseCtx->infoPanelVtx[18].v.tc[1] = pauseCtx->infoPanelVtx[19].v.tc[1] = 16 * (1 << 5);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 20, 30, 40, 0);

    if (pauseCtx->itemDescriptionOn ||
        ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->namedItem != PAUSE_ITEM_NONE) &&
         (pauseCtx->nameDisplayTimer < 40) &&
         (!pauseCtx->mainState || (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PLAYBACK) ||
          (pauseCtx->mainState == PAUSE_MAIN_STATE_UNK) ||
          ((pauseCtx->mainState >= PAUSE_MAIN_STATE_SONG_PROMPT_INIT) &&
           (pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PROMPT_UNUSED)) ||
          (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) &&
         (pauseCtx->cursorSpecialPos == 0))) {
        if (!pauseCtx->mainState || (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PLAYBACK) ||
            (pauseCtx->mainState == PAUSE_MAIN_STATE_UNK) ||
            ((pauseCtx->mainState >= PAUSE_MAIN_STATE_SONG_PROMPT_INIT) &&
             (pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PROMPT_UNUSED)) ||
            (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) {

            pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -63;

            pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 128;

            pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 128 * (1 << 5);

            gSPVertex(POLY_OPA_DISP++, &pauseCtx->infoPanelVtx[16], 4, 0);

            if (pauseCtx->nameColorSet == PAUSE_NAME_COLOR_SET_GREY) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 70, 70, 70, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
            }

            POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, pauseCtx->nameSegment, G_IM_FMT_IA, 128, 16, 0);
        }
    } else if ((pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PLAYBACK) ||
               (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT_UNUSED) ||
               (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) ||
               (pauseCtx->mainState == PAUSE_MAIN_STATE_UNK)) {
        pauseCtx->infoPanelVtx[20].v.ob[1] = pauseCtx->infoPanelVtx[21].v.ob[1] = y;

        pauseCtx->infoPanelVtx[22].v.ob[1] = pauseCtx->infoPanelVtx[23].v.ob[1] =
            pauseCtx->infoPanelVtx[20].v.ob[1] - 16;

        pauseCtx->infoPanelVtx[22].v.tc[1] = pauseCtx->infoPanelVtx[23].v.tc[1] = 16 * (1 << 5);

        gSPVertex(POLY_OPA_DISP++, &pauseCtx->infoPanelVtx[16], 8, 0);

        if (pauseCtx->state == PAUSE_STATE_SAVEPROMPT) {
            pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -33;

            pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 24;

            pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 0x10;

            pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                pauseCtx->infoPanelVtx[20].v.ob[0] + 0x30;

            pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 24 * (1 << 5);

            // #region 2S2H - fixed vtx size for correct texture size
            pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] = 64 * (1 << 5);

            gSPDisplayList(POLY_OPA_DISP++, gAButtonIconDL);
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

            //! @bug: Incorrect dimensions. Should be 64x16
            //! Fixed 11/23/23
            POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, gPauseToDecideENGTex, G_IM_FMT_IA, 64, 16, 4);

        } else if (pauseCtx->cursorSpecialPos != 0) {
            if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {

                pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -63;

                pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + 128;

                pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 128 * (1 << 5);

                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 200, 0, 255);

                if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
                    POLY_OPA_DISP =
                        Gfx_DrawTexQuad4b(POLY_OPA_DISP, D_8082B998[pauseCtx->pageIndex], G_IM_FMT_IA, 128, 16, 0);
                } else {
                    POLY_OPA_DISP =
                        Gfx_DrawTexQuad4b(POLY_OPA_DISP, D_8082B9A8[pauseCtx->pageIndex], G_IM_FMT_IA, 128, 16, 0);
                }
            }
        } else if ((!pauseCtx->pageIndex || (pauseCtx->pageIndex == PAUSE_MASK)) &&
                   (pauseCtx->namedItem != PAUSE_ITEM_NONE)) {
            pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -49;

            pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 48;

            pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 47;

            pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                pauseCtx->infoPanelVtx[20].v.ob[0] + 64;

            pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 48 * (1 << 5);

            pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] = 64 * (1 << 5);

            gSPDisplayList(POLY_OPA_DISP++, gCButtonIconsDL);

            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

            POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, gPauseToEquipENGTex, G_IM_FMT_IA, 64, 16, 4);
        } else if ((pauseCtx->pageIndex == PAUSE_MAP) && sInDungeonScene) {
            // No code in this case
        } else if ((pauseCtx->pageIndex == PAUSE_QUEST) &&
                   (pauseCtx->cursorSlot[PAUSE_QUEST] == QUEST_BOMBERS_NOTEBOOK)) {
            if (pauseCtx->namedItem != PAUSE_ITEM_NONE) {
                // The cursor is on the bombers notebook
                pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -58;

                pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + 24;

                pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + 0x14;

                pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                    pauseCtx->infoPanelVtx[20].v.ob[0] + 0x60;

                pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 24 * (1 << 5);

                pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] = 96 * (1 << 5);

                gSPDisplayList(POLY_OPA_DISP++, gAButtonIconDL);

                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

                POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, gPauseToViewNotebookENGTex, G_IM_FMT_IA, 96, 16, 4);
            }
        } else if ((pauseCtx->pageIndex == PAUSE_QUEST) && (pauseCtx->cursorSlot[PAUSE_QUEST] >= QUEST_SONG_SONATA) &&
                   (pauseCtx->cursorSlot[PAUSE_QUEST] <= QUEST_SONG_SUN) && (pauseCtx->namedItem != PAUSE_ITEM_NONE)) {
            // The cursor is on a learned song
            pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -55;

            pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 24;

            pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 20;

            pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                pauseCtx->infoPanelVtx[20].v.ob[0] + 96;

            pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 24 * (1 << 5);

            pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] = 96 * (1 << 5);

            gSPDisplayList(POLY_OPA_DISP++, gAButtonIconDL);

            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

            POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, gPauseToPlayMelodyENGTex, G_IM_FMT_IA, 96, 16, 4);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

u8 sUnpausedButtonStatus[5];
// #region 2S2H [Dpad]
u8 sUnpausedDpadStatus[4];
// #endregion
f32 sCursorCirclesX[4];
f32 sCursorCirclesY[4];

void KaleidoScope_UpdateNamePanel(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    u16 namedItem;

    if ((pauseCtx->namedItem != pauseCtx->cursorItem[pauseCtx->pageIndex]) ||
        ((pauseCtx->pageIndex == PAUSE_MAP) && (pauseCtx->cursorSpecialPos != 0))) {

        pauseCtx->namedItem = pauseCtx->cursorItem[pauseCtx->pageIndex];
        namedItem = pauseCtx->namedItem;

        // osCreateMesgQueue(&pauseCtx->loadQueue, &pauseCtx->loadMsg, 1);

        if (pauseCtx->namedItem != PAUSE_ITEM_NONE) {
            if ((pauseCtx->pageIndex == PAUSE_MAP) && !sInDungeonScene) {
                Kaleido_LoadMapNameStatic(&pauseCtx->nameSegment, namedItem);
            } else {
                Kaleido_LoadItemNameStatic(&pauseCtx->nameSegment, namedItem);
            }
            pauseCtx->nameDisplayTimer = 0;
        }
    } else if (pauseCtx->nameColorSet == PAUSE_NAME_COLOR_SET_WHITE) {
        if (((pauseCtx->pageIndex == PAUSE_QUEST) && (pauseCtx->cursorSlot[PAUSE_QUEST] >= QUEST_SONG_SONATA) &&
             (pauseCtx->cursorSlot[PAUSE_QUEST] <= QUEST_SONG_SUN) &&
             (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) ||
            ((pauseCtx->pageIndex == PAUSE_QUEST) && (pauseCtx->cursorSlot[PAUSE_QUEST] == QUEST_BOMBERS_NOTEBOOK)) ||
            (pauseCtx->pageIndex == PAUSE_ITEM) || (pauseCtx->pageIndex == PAUSE_MASK)) {

            pauseCtx->nameDisplayTimer++;
            if (pauseCtx->nameDisplayTimer > 70) {
                pauseCtx->nameDisplayTimer = 0;
            }
        } else {
            pauseCtx->nameDisplayTimer = 0;
        }
    } else {
        pauseCtx->nameDisplayTimer = 0;
    }
}

void KaleidoScope_DrawOwlWarpMapPage(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0,
                      SHADE, 0);

    gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255, COSMETIC_ID("Menus.PausePanels"));

    Matrix_RotateYF(-1.57f, MTXMODE_NEW);
    Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
    Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
    Matrix_RotateXFApply(-pauseCtx->mapPageRoll / 100.0f);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->mapPageVtx, sMapPageBgTextures);

    Matrix_RotateYF(R_PAUSE_WORLD_MAP_YAW / 1000.0f, MTXMODE_NEW);

    if ((pauseCtx->state == PAUSE_STATE_OPENING_3) || (pauseCtx->state == PAUSE_STATE_OWL_WARP_3) ||
        (pauseCtx->state >= PAUSE_STATE_OWL_WARP_6) ||
        ((pauseCtx->state == PAUSE_STATE_SAVEPROMPT) && ((pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_3) ||
                                                         (pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_7)))) {
        Matrix_Translate(0.0f, (R_PAUSE_WORLD_MAP_Y_OFFSET - 8000) / 100.0f, R_PAUSE_WORLD_MAP_DEPTH / 100.0f,
                         MTXMODE_APPLY);
    } else {
        Matrix_Translate(0.0f, R_PAUSE_WORLD_MAP_Y_OFFSET / 100.0f, R_PAUSE_WORLD_MAP_DEPTH / 100.0f, MTXMODE_APPLY);
    }

    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateXFApply(-pauseCtx->mapPageRoll / 100.0f);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    KaleidoScope_DrawWorldMap(play);

    CLOSE_DISPS(play->state.gfxCtx);
}

void KaleidoScope_DrawOwlWarpInfoPanel(PlayState* play) {
    static s16 sPauseZRCursorColorTargets[][4] = {
        { 180, 210, 255, 220 },
        { 100, 100, 150, 220 },
    };
    static s16 sPauseZRCursorColorTimer = 20;
    static s16 sPauseZRCursorColorIndex = 0;
    static s16 sPauseZRCursorRed;
    static s16 sPauseZRCursorGreen;
    static s16 sPauseZRCursorBlue;
    static s16 sPauseZRCursorAlpha;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 stepR;
    s16 stepG;
    s16 stepB;
    s16 stepA;
    s16 y;
    s16 i;
    s16 j;

    OPEN_DISPS(play->state.gfxCtx);

    stepR =
        ABS_ALT(sPauseZRCursorRed - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][0]) / sPauseZRCursorColorTimer;
    stepG = ABS_ALT(sPauseZRCursorGreen - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][1]) /
            sPauseZRCursorColorTimer;
    stepB = ABS_ALT(sPauseZRCursorBlue - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][2]) /
            sPauseZRCursorColorTimer;
    stepA = ABS_ALT(sPauseZRCursorAlpha - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][3]) /
            sPauseZRCursorColorTimer;

    if (sPauseZRCursorRed >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][0]) {
        sPauseZRCursorRed -= stepR;
    } else {
        sPauseZRCursorRed += stepR;
    }

    if (sPauseZRCursorGreen >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][1]) {
        sPauseZRCursorGreen -= stepG;
    } else {
        sPauseZRCursorGreen += stepG;
    }

    if (sPauseZRCursorBlue >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][2]) {
        sPauseZRCursorBlue -= stepB;
    } else {
        sPauseZRCursorBlue += stepB;
    }

    if (sPauseZRCursorAlpha >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][3]) {
        sPauseZRCursorAlpha -= stepA;
    } else {
        sPauseZRCursorAlpha += stepA;
    }

    sPauseZRCursorColorTimer--;
    if (sPauseZRCursorColorTimer == 0) {
        sPauseZRCursorRed = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][0];
        sPauseZRCursorGreen = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][1];
        sPauseZRCursorBlue = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][2];
        sPauseZRCursorAlpha = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][3];
        sPauseZRCursorColorTimer = sPauseZRCursorColorTimerInits[0];
        sPauseZRCursorColorIndex ^= 1;
    }

    y = pauseCtx->infoPanelOffsetY - 76;
    for (j = 0, i = 0; i < 7; i++, j += 4) {
        pauseCtx->infoPanelVtx[j + 0].v.ob[0] = pauseCtx->infoPanelVtx[j + 2].v.ob[0] = -72;

        pauseCtx->infoPanelVtx[j + 1].v.ob[0] = pauseCtx->infoPanelVtx[j + 3].v.ob[0] = 0;

        pauseCtx->infoPanelVtx[j + 0].v.ob[1] = pauseCtx->infoPanelVtx[j + 1].v.ob[1] = y;

        pauseCtx->infoPanelVtx[j + 2].v.ob[1] = pauseCtx->infoPanelVtx[j + 3].v.ob[1] = y - 24;

        pauseCtx->infoPanelVtx[j + 0].v.ob[2] = pauseCtx->infoPanelVtx[j + 1].v.ob[2] =
            pauseCtx->infoPanelVtx[j + 2].v.ob[2] = pauseCtx->infoPanelVtx[j + 3].v.ob[2] = 0;

        pauseCtx->infoPanelVtx[j + 0].v.flag = pauseCtx->infoPanelVtx[j + 1].v.flag =
            pauseCtx->infoPanelVtx[j + 2].v.flag = pauseCtx->infoPanelVtx[j + 3].v.flag = 0;

        pauseCtx->infoPanelVtx[j + 0].v.tc[0] = pauseCtx->infoPanelVtx[j + 0].v.tc[1] =
            pauseCtx->infoPanelVtx[j + 1].v.tc[1] = pauseCtx->infoPanelVtx[j + 2].v.tc[0] = 0;

        pauseCtx->infoPanelVtx[j + 1].v.tc[0] = pauseCtx->infoPanelVtx[j + 3].v.tc[0] = 72 * (1 << 5);

        pauseCtx->infoPanelVtx[j + 2].v.tc[1] = pauseCtx->infoPanelVtx[j + 3].v.tc[1] = 24 * (1 << 5);

        pauseCtx->infoPanelVtx[j + 0].v.cn[0] = pauseCtx->infoPanelVtx[j + 2].v.cn[0] =
            pauseCtx->infoPanelVtx[j + 0].v.cn[1] = pauseCtx->infoPanelVtx[j + 2].v.cn[1] =
                pauseCtx->infoPanelVtx[j + 0].v.cn[2] = pauseCtx->infoPanelVtx[j + 2].v.cn[2] =
                    pauseCtx->infoPanelVtx[j + 1].v.cn[0] = pauseCtx->infoPanelVtx[j + 3].v.cn[0] =
                        pauseCtx->infoPanelVtx[j + 1].v.cn[1] = pauseCtx->infoPanelVtx[j + 3].v.cn[1] =
                            pauseCtx->infoPanelVtx[j + 1].v.cn[2] = pauseCtx->infoPanelVtx[j + 3].v.cn[2] = 200;

        pauseCtx->infoPanelVtx[j + 0].v.cn[3] = pauseCtx->infoPanelVtx[j + 2].v.cn[3] =
            pauseCtx->infoPanelVtx[j + 1].v.cn[3] = pauseCtx->infoPanelVtx[j + 3].v.cn[3] = pauseCtx->alpha;
    }

    pauseCtx->infoPanelVtx[4].v.ob[0] = pauseCtx->infoPanelVtx[6].v.ob[0] = pauseCtx->infoPanelVtx[0].v.ob[0] + 72;

    pauseCtx->infoPanelVtx[5].v.ob[0] = pauseCtx->infoPanelVtx[7].v.ob[0] = pauseCtx->infoPanelVtx[4].v.ob[0] + 72;

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {
        pauseCtx->infoPanelVtx[8].v.ob[0] = pauseCtx->infoPanelVtx[10].v.ob[0] = sPauseCursorLeftX;

        pauseCtx->infoPanelVtx[9].v.ob[0] = pauseCtx->infoPanelVtx[11].v.ob[0] = pauseCtx->infoPanelVtx[8].v.ob[0] + 24;

        pauseCtx->infoPanelVtx[8].v.ob[1] = pauseCtx->infoPanelVtx[9].v.ob[1] = D_8082B920;

        pauseCtx->infoPanelVtx[10].v.ob[1] = pauseCtx->infoPanelVtx[11].v.ob[1] =
            pauseCtx->infoPanelVtx[8].v.ob[1] - 32;
    } else {
        pauseCtx->infoPanelVtx[8].v.ob[0] = pauseCtx->infoPanelVtx[10].v.ob[0] = sPauseCursorLeftX + 3;

        pauseCtx->infoPanelVtx[9].v.ob[0] = pauseCtx->infoPanelVtx[11].v.ob[0] = pauseCtx->infoPanelVtx[8].v.ob[0] + 18;

        pauseCtx->infoPanelVtx[8].v.ob[1] = pauseCtx->infoPanelVtx[9].v.ob[1] = D_8082B920 - 3;

        pauseCtx->infoPanelVtx[10].v.ob[1] = pauseCtx->infoPanelVtx[11].v.ob[1] =
            pauseCtx->infoPanelVtx[8].v.ob[1] - 26;
    }

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {
        pauseCtx->infoPanelVtx[12].v.ob[0] = pauseCtx->infoPanelVtx[14].v.ob[0] = sPauseCursorRightX;

        pauseCtx->infoPanelVtx[13].v.ob[0] = pauseCtx->infoPanelVtx[15].v.ob[0] =
            pauseCtx->infoPanelVtx[12].v.ob[0] + 24;

        pauseCtx->infoPanelVtx[12].v.ob[1] = pauseCtx->infoPanelVtx[13].v.ob[1] = D_8082B920;

        pauseCtx->infoPanelVtx[14].v.ob[1] = pauseCtx->infoPanelVtx[15].v.ob[1] =
            pauseCtx->infoPanelVtx[12].v.ob[1] - 32;
    } else {
        pauseCtx->infoPanelVtx[12].v.ob[0] = pauseCtx->infoPanelVtx[14].v.ob[0] = sPauseCursorRightX + 3;

        pauseCtx->infoPanelVtx[13].v.ob[0] = pauseCtx->infoPanelVtx[15].v.ob[0] =
            pauseCtx->infoPanelVtx[12].v.ob[0] + 18;

        pauseCtx->infoPanelVtx[12].v.ob[1] = pauseCtx->infoPanelVtx[13].v.ob[1] = D_8082B920 - 3;

        pauseCtx->infoPanelVtx[14].v.ob[1] = pauseCtx->infoPanelVtx[15].v.ob[1] =
            pauseCtx->infoPanelVtx[12].v.ob[1] - 26;
    }

    pauseCtx->infoPanelVtx[9].v.tc[0] = pauseCtx->infoPanelVtx[11].v.tc[0] = pauseCtx->infoPanelVtx[13].v.tc[0] =
        pauseCtx->infoPanelVtx[15].v.tc[0] = 24 * (1 << 5);

    pauseCtx->infoPanelVtx[10].v.tc[1] = pauseCtx->infoPanelVtx[11].v.tc[1] = pauseCtx->infoPanelVtx[14].v.tc[1] =
        pauseCtx->infoPanelVtx[15].v.tc[1] = 32 * (1 << 5);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    Matrix_Translate(0.0f, 0.0f, -144.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 140, 90, 255);
    gSPVertex(POLY_OPA_DISP++, &pauseCtx->infoPanelVtx[0], 16, 0);

    gSPDisplayList(POLY_OPA_DISP++, gItemNamePanelDL);

    if (pauseCtx->cursorSpecialPos != 0) {
        j = (pauseCtx->cursorSpecialPos * 4) - 32;
        pauseCtx->cursorVtx[0].v.ob[0] = pauseCtx->infoPanelVtx[j].v.ob[0];
        pauseCtx->cursorVtx[0].v.ob[1] = pauseCtx->infoPanelVtx[j].v.ob[1];
    }

    y = pauseCtx->infoPanelOffsetY - 80;
    pauseCtx->infoPanelVtx[16].v.ob[1] = pauseCtx->infoPanelVtx[17].v.ob[1] = y;

    pauseCtx->infoPanelVtx[18].v.ob[1] = pauseCtx->infoPanelVtx[19].v.ob[1] = pauseCtx->infoPanelVtx[16].v.ob[1] - 16;

    pauseCtx->infoPanelVtx[18].v.tc[1] = pauseCtx->infoPanelVtx[19].v.tc[1] = 16 * (1 << 5);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 20, 30, 40, 0);

    if ((pauseCtx->state == PAUSE_STATE_OWL_WARP_SELECT) && (pauseCtx->namedItem != PAUSE_ITEM_NONE) &&
        (pauseCtx->nameDisplayTimer < 40)) {
        pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -63;

        pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
            pauseCtx->infoPanelVtx[16].v.ob[0] + 128;

        pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 128 * (1 << 5);

        gSPVertex(POLY_OPA_DISP++, &pauseCtx->infoPanelVtx[16], 4, 0);

        if (pauseCtx->nameColorSet == PAUSE_NAME_COLOR_SET_GREY) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 70, 70, 70, 255);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        }

        POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, pauseCtx->nameSegment, G_IM_FMT_IA, 128, 16, 0);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void KaleidoScope_UpdateOwlWarpNamePanel(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    u16 texIndex;

    if ((pauseCtx->namedItem != pauseCtx->cursorItem[pauseCtx->pageIndex]) ||
        ((pauseCtx->pageIndex == PAUSE_MAP) && (pauseCtx->cursorSpecialPos != 0))) {

        pauseCtx->namedItem = pauseCtx->cursorItem[pauseCtx->pageIndex];
        texIndex = pauseCtx->namedItem;

        // osCreateMesgQueue(&pauseCtx->loadQueue, &pauseCtx->loadMsg, 1);

        if (pauseCtx->namedItem != PAUSE_ITEM_NONE) {
            if ((pauseCtx->pageIndex == PAUSE_MAP) && !sInDungeonScene) {
                Kaleido_LoadMapNameStatic(&pauseCtx->nameSegment, texIndex);
            } else {
                Kaleido_LoadItemNameStatic(&pauseCtx->nameSegment, texIndex);
            }
            pauseCtx->nameDisplayTimer = 0;
        }
    } else if (pauseCtx->nameColorSet == PAUSE_NAME_COLOR_SET_WHITE) {
        if (((pauseCtx->pageIndex == PAUSE_QUEST) && (pauseCtx->cursorSlot[PAUSE_QUEST] >= QUEST_SONG_SONATA) &&
             (pauseCtx->cursorSlot[PAUSE_QUEST] <= QUEST_SONG_SUN) &&
             (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) ||
            (pauseCtx->pageIndex == PAUSE_ITEM)) {

            pauseCtx->nameDisplayTimer++;
            if (pauseCtx->nameDisplayTimer > 70) {
                pauseCtx->nameDisplayTimer = 0;
            }
        } else {
            pauseCtx->nameDisplayTimer = 0;
        }
    } else {
        pauseCtx->nameDisplayTimer = 0;
    }
}

void KaleidoScope_UpdateSwitchPage(PlayState* play, Input* input) {
    PauseContext* pauseCtx = &play->pauseCtx;

    //! FAKE
    if (1) {
        pauseCtx->eye.x += sPageSwitchEyeDx[pauseCtx->nextPageMode] * 2.0f;
        pauseCtx->eye.z += sPageSwitchEyeDz[pauseCtx->nextPageMode] * 2.0f;

        if (pauseCtx->switchPageTimer < 32) {
            sPauseCursorLeftX -= TRUNCF_BINANG(sPauseCursorLeftMoveOffsetX / 4);
            sPauseCursorRightX -= TRUNCF_BINANG(sPauseCursorRightMoveOffsetX / 4);
        } else {
            sPauseCursorLeftX += TRUNCF_BINANG(sPauseCursorLeftMoveOffsetX / 4);
            sPauseCursorRightX += TRUNCF_BINANG(sPauseCursorRightMoveOffsetX / 4);
        }

        pauseCtx->switchPageTimer += 8;

        if (pauseCtx->switchPageTimer == 64) {
            pauseCtx->switchPageTimer = 0;
            pauseCtx->pageIndex = sPageSwitchNextPageIndex[pauseCtx->nextPageMode];
            pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;

            KaleidoScope_HandlePageToggles(play, input);
        }
    }
}

void KaleidoScope_SetView(PauseContext* pauseCtx, f32 eyeX, f32 eyeY, f32 eyeZ) {
    Vec3f eye;
    Vec3f at;
    Vec3f up;

    eye.x = eyeX;
    eye.y = eyeY;
    eye.z = eyeZ;
    at.x = at.y = at.z = 0.0f;
    up.x = up.z = 0.0f;
    up.y = 1.0f;

    View_LookAt(&pauseCtx->view, &eye, &at, &up);
    View_Apply(&pauseCtx->view,
               VIEW_ALL | VIEW_FORCE_VIEWING | VIEW_FORCE_VIEWPORT | VIEW_FORCE_PROJECTION_PERSPECTIVE);
}

// CLAMP_MIN(*, 1) because C arrays can't have 0 length
s16 sVtxPageMaskQuadsX[CLAMP_MIN(VTX_PAGE_MASK_QUADS, 1)] = { 0 };
s16 sVtxPageItemQuadsX[CLAMP_MIN(VTX_PAGE_ITEM_QUADS, 1)] = { 0 };
s16 sVtxPageMapDungeonQuadsX[VTX_PAGE_MAP_DUNGEON_QUADS] = {
    -21,  // mapPageVtx[60] QUAD_MAP_DUNGEON_TITLE
    -104, // mapPageVtx[64] QUAD_MAP_DUNGEON_BOSS_KEY
    -76,  // mapPageVtx[68] QUAD_MAP_DUNGEON_COMPASS
    -48,  // mapPageVtx[72] QUAD_MAP_DUNGEON_MAP
    -104, // mapPageVtx[76] QUAD_MAP_DUNGEON_STRAY_FAIRY_GLOWING_CIRCLE
    -40   // mapPageVtx[80] QUAD_MAP_DUNGEON_UNUSED
};
s16 sVtxPageQuestQuadsX[CLAMP_MIN(VTX_PAGE_QUEST_QUADS, 1)] = { 0 };
s16 sVtxPageMapWorldQuadsX[VTX_PAGE_MAP_WORLD_QUADS] = {
    -41,  // mapPageVtx[60] world map clouds Clock Town 1
    9,    // mapPageVtx[64] world map clouds Clock Town 2
    43,   // mapPageVtx[68] world map clouds Woodfall 1
    -21,  // mapPageVtx[72] world map clouds Woodfall 2
    -37,  // mapPageVtx[76] world map clouds Woodfall 3
    43,   // mapPageVtx[80] world map clouds Snowhead 1
    -69,  // mapPageVtx[84] world map clouds Snowhead 2
    -69,  // mapPageVtx[88] world map clouds Snowhead 3
    -32,  // mapPageVtx[92] world map clouds Romani Ranch
    -109, // mapPageVtx[96] world map clouds Great Bay 1
    -45,  // mapPageVtx[100] world map clouds Great Bay 2
    -109, // mapPageVtx[104] world map clouds Great Bay 3
    -45,  // mapPageVtx[108] world map clouds Great Bay 4
    59,   // mapPageVtx[112] world map clouds Stone Tower 1
    -5,   // mapPageVtx[116] world map clouds Stone Tower 2
    -70,  // mapPageVtx[120] world map region Great Bay
    -52,  // mapPageVtx[124] world map region Zora Hall
    -11,  // mapPageVtx[128] world map region Romani Ranch
    -2,   // mapPageVtx[132] world map region Deku Palace
    8,    // mapPageVtx[136] world map region Woodfall
    2,    // mapPageVtx[140] world map region Clock Town
    1,    // mapPageVtx[144] world map region Snowhead
    43,   // mapPageVtx[148] world map region Ikana Graveyard
    52,   // mapPageVtx[152] world map region Ikana Canyon
    54,   // mapPageVtx[156] world map region Goron Village
    62,   // mapPageVtx[160] world map region Stone Tower
    -80,  // mapPageVtx[164] world map owl warp Great Bay Coast
    -64,  // mapPageVtx[168] world map owl warp Zora Cape
    -9,   // mapPageVtx[172] world map owl warp Snowhead
    -3,   // mapPageVtx[176] world map owl warp Mountain Village
    -7,   // mapPageVtx[180] world map owl warp Clock Town
    -16,  // mapPageVtx[184] world map owl warp Milk Road
    -1,   // mapPageVtx[188] world map owl warp Woodfall
    23,   // mapPageVtx[192] world map owl warp Southern Swamp
    44,   // mapPageVtx[196] world map owl warp Ikana Canyon
    54,   // mapPageVtx[200] world map owl warp Stone Tower
};
s16 sVtxPagePromptQuadsX[VTX_PAGE_SAVE_QUADS] = {
    -76, // promptPageVtx[60] QUAD_PROMPT_MESSAGE
    -58, // promptPageVtx[64] QUAD_PROMPT_CURSOR_LEFT
    10,  // promptPageVtx[68] QUAD_PROMPT_CURSOR_RIGHT
    -58, // promptPageVtx[72] QUAD_PROMPT_CHOICE_YES
    10,  // promptPageVtx[76] QUAD_PROMPT_CHOICE_NO
};

// CLAMP_MIN(*, 1) because C arrays can't have 0 length
s16 sVtxPageMaskQuadsWidth[CLAMP_MIN(VTX_PAGE_MASK_QUADS, 1)] = { 0 };
s16 sVtxPageItemQuadsWidth[CLAMP_MIN(VTX_PAGE_ITEM_QUADS, 1)] = { 0 };
s16 sVtxPageMapDungeonQuadsWidth[VTX_PAGE_MAP_DUNGEON_QUADS] = {
    128, // mapPageVtx[60] QUAD_MAP_DUNGEON_TITLE
    24,  // mapPageVtx[64] QUAD_MAP_DUNGEON_BOSS_KEY
    24,  // mapPageVtx[68] QUAD_MAP_DUNGEON_COMPASS
    24,  // mapPageVtx[72] QUAD_MAP_DUNGEON_MAP
    32,  // mapPageVtx[76] QUAD_MAP_DUNGEON_STRAY_FAIRY_GLOWING_CIRCLE
    24,  // mapPageVtx[80] QUAD_MAP_DUNGEON_UNUSED
};
s16 sVtxPageQuestQuadsWidth[CLAMP_MIN(VTX_PAGE_QUEST_QUADS, 1)] = { 0 };
s16 sVtxPagePromptQuadsWidth[VTX_PAGE_SAVE_QUADS] = {
    152, // promptPageVtx[60] QUAD_PROMPT_MESSAGE
    48,  // promptPageVtx[64] QUAD_PROMPT_CURSOR_LEFT
    48,  // promptPageVtx[68] QUAD_PROMPT_CURSOR_RIGHT
    48,  // promptPageVtx[72] QUAD_PROMPT_CHOICE_YES
    48,  // promptPageVtx[76] QUAD_PROMPT_CHOICE_NO
};

// CLAMP_MIN(*, 1) because C arrays can't have 0 length
s16 sVtxPageMaskQuadsY[CLAMP_MIN(VTX_PAGE_MASK_QUADS, 1)] = { 0 };
s16 sVtxPageItemQuadsY[CLAMP_MIN(VTX_PAGE_ITEM_QUADS, 1)] = { 0 };

s16 sVtxPageMapDungeonQuadsY[VTX_PAGE_MAP_DUNGEON_QUADS] = {
    54,  // mapPageVtx[60] QUAD_MAP_DUNGEON_TITLE
    -42, // mapPageVtx[64] QUAD_MAP_DUNGEON_BOSS_KEY
    -42, // mapPageVtx[68] QUAD_MAP_DUNGEON_COMPASS
    -42, // mapPageVtx[72] QUAD_MAP_DUNGEON_MAP
    -24, // mapPageVtx[76] QUAD_MAP_DUNGEON_STRAY_FAIRY_GLOWING_CIRCLE
    50,  // mapPageVtx[80] QUAD_MAP_DUNGEON_UNUSED
};
s16 sVtxPageQuestQuadsY[CLAMP_MIN(VTX_PAGE_QUEST_QUADS, 1)] = { 0 };
s16 sVtxPageMapWorldQuadsY[VTX_PAGE_MAP_WORLD_QUADS] = {
    11,  // mapPageVtx[60] world map clouds Clock Town 1
    22,  // mapPageVtx[64] world map clouds Clock Town 2
    -16, // mapPageVtx[68] world map clouds Woodfall 1
    -16, // mapPageVtx[72] world map clouds Woodfall 2
    -16, // mapPageVtx[76] world map clouds Woodfall 3
    59,  // mapPageVtx[80] world map clouds Snowhead 1
    24,  // mapPageVtx[84] world map clouds Snowhead 2
    59,  // mapPageVtx[88] world map clouds Snowhead 3
    -11, // mapPageVtx[92] world map clouds Romani Ranch
    -5,  // mapPageVtx[96] world map clouds Great Bay 1
    -5,  // mapPageVtx[100] world map clouds Great Bay 2
    59,  // mapPageVtx[104] world map clouds Great Bay 3
    59,  // mapPageVtx[108] world map clouds Great Bay 4
    39,  // mapPageVtx[112] world map clouds Stone Tower 1
    31,  // mapPageVtx[116] world map clouds Stone Tower 2
    -10, // mapPageVtx[120] world map region Great Bay
    -36, // mapPageVtx[124] world map region Zora Hall
    -19, // mapPageVtx[128] world map region Romani Ranch
    -53, // mapPageVtx[132] world map region Deku Palace
    -27, // mapPageVtx[136] world map region Woodfall
    -2,  // mapPageVtx[140] world map region Clock Town
    41,  // mapPageVtx[144] world map region Snowhead
    11,  // mapPageVtx[148] world map region Ikana Graveyard
    -3,  // mapPageVtx[152] world map region Ikana Canyon
    45,  // mapPageVtx[156] world map region Goron Village
    18,  // mapPageVtx[160] world map region Stone Tower
    -8,  // mapPageVtx[164] world map owl warp Great Bay Coast
    -38, // mapPageVtx[168] world map owl warp Zora Cape
    39,  // mapPageVtx[172] world map owl warp Snowhead
    26,  // mapPageVtx[176] world map owl warp Mountain Village
    1,   // mapPageVtx[180] world map owl warp Clock Town
    -7,  // mapPageVtx[184] world map owl warp Milk Road
    -28, // mapPageVtx[188] world map owl warp Woodfall
    -27, // mapPageVtx[192] world map owl warp Southern Swamp
    -1,  // mapPageVtx[196] world map owl warp Ikana Canyon
    24,  // mapPageVtx[200] world map owl warp Stone Tower
};
s16 sVtxPagePromptQuadsY[VTX_PAGE_SAVE_QUADS] = {
    36, // promptPageVtx[60] QUAD_PROMPT_MESSAGE
    10, // promptPageVtx[64] QUAD_PROMPT_CURSOR_LEFT
    10, // promptPageVtx[68] QUAD_PROMPT_CURSOR_RIGHT
    -6, // promptPageVtx[72] QUAD_PROMPT_CHOICE_YES
    -6, // promptPageVtx[76] QUAD_PROMPT_CHOICE_NO
};

// CLAMP_MIN(*, 1) because C arrays can't have 0 length
s16 sVtxPageMaskQuadsHeight[CLAMP_MIN(VTX_PAGE_MASK_QUADS, 1)] = { 0 };
s16 sVtxPageItemQuadsHeight[CLAMP_MIN(VTX_PAGE_ITEM_QUADS, 1)] = { 0 };
s16 sVtxPageMapDungeonQuadsHeight[] = {
    16, // mapPageVtx[60] QUAD_MAP_DUNGEON_TITLE
    24, // mapPageVtx[64] QUAD_MAP_DUNGEON_BOSS_KEY
    24, // mapPageVtx[68] QUAD_MAP_DUNGEON_COMPASS
    24, // mapPageVtx[72] QUAD_MAP_DUNGEON_MAP
    24, // mapPageVtx[76] QUAD_MAP_DUNGEON_STRAY_FAIRY_GLOWING_CIRCLE
    24, // mapPageVtx[80] QUAD_MAP_DUNGEON_UNUSED
};
s16 sVtxPageQuestQuadsHeight[CLAMP_MIN(VTX_PAGE_QUEST_QUADS, 1)] = { 0 };
s16 sVtxPagePromptQuadsHeight[VTX_PAGE_SAVE_QUADS] = {
    16, // promptPageVtx[60] QUAD_PROMPT_MESSAGE
    48, // promptPageVtx[64] QUAD_PROMPT_CURSOR_LEFT
    48, // promptPageVtx[68] QUAD_PROMPT_CURSOR_RIGHT
    16, // promptPageVtx[72] QUAD_PROMPT_CHOICE_YES
    16, // promptPageVtx[76] QUAD_PROMPT_CHOICE_NO
};

s16* sVtxPageQuadsX[VTX_PAGE_MAX] = {
    sVtxPageMaskQuadsX,       // VTX_PAGE_MASK
    sVtxPageItemQuadsX,       // VTX_PAGE_ITEM
    sVtxPageMapDungeonQuadsX, // VTX_PAGE_MAP_DUNGEON
    sVtxPageQuestQuadsX,      // VTX_PAGE_QUEST
    sVtxPageMapWorldQuadsX,   // VTX_PAGE_MAP_WORLD
    sVtxPagePromptQuadsX,     // VTX_PAGE_PROMPT
};

s16* sVtxPageQuadsWidth[VTX_PAGE_MAX] = {
    sVtxPageMaskQuadsWidth,       // VTX_PAGE_MASK
    sVtxPageItemQuadsWidth,       // VTX_PAGE_ITEM
    sVtxPageMapDungeonQuadsWidth, // VTX_PAGE_MAP_DUNGEON
    sVtxPageQuestQuadsWidth,      // VTX_PAGE_QUEST
    gVtxPageMapWorldQuadsWidth,   // VTX_PAGE_MAP_WORLD
    sVtxPagePromptQuadsWidth,     // VTX_PAGE_PROMPT
};

s16* sVtxPageQuadsY[VTX_PAGE_MAX] = {
    sVtxPageMaskQuadsY,       // VTX_PAGE_MASK
    sVtxPageItemQuadsY,       // VTX_PAGE_ITEM
    sVtxPageMapDungeonQuadsY, // VTX_PAGE_MAP_DUNGEON
    sVtxPageQuestQuadsY,      // VTX_PAGE_QUEST
    sVtxPageMapWorldQuadsY,   // VTX_PAGE_MAP_WORLD
    sVtxPagePromptQuadsY,     // VTX_PAGE_PROMPT
};

s16* sVtxPageQuadsHeight[VTX_PAGE_MAX] = {
    sVtxPageMaskQuadsHeight,       // VTX_PAGE_MASK
    sVtxPageItemQuadsHeight,       // VTX_PAGE_ITEM
    sVtxPageMapDungeonQuadsHeight, // VTX_PAGE_MAP_DUNGEON
    sVtxPageQuestQuadsHeight,      // VTX_PAGE_QUEST
    gVtxPageMapWorldQuadsHeight,   // VTX_PAGE_MAP_WORLD
    sVtxPagePromptQuadsHeight,     // VTX_PAGE_PROMPT
};

s16 sVtxPageGameOverSaveQuadsY[VTX_PAGE_SAVE_QUADS] = {
    14,  // promptPageVtx[60] QUAD_PROMPT_MESSAGE
    -2,  // promptPageVtx[64] QUAD_PROMPT_CURSOR_LEFT
    -2,  // promptPageVtx[68] QUAD_PROMPT_CURSOR_RIGHT
    -18, // promptPageVtx[72] QUAD_PROMPT_CHOICE_YES
    -18, // promptPageVtx[76] QUAD_PROMPT_CHOICE_NO
};

s16 KaleidoScope_SetPageVertices(PlayState* play, Vtx* vtx, s16 vtxPage, s16 numQuads) {
    PauseContext* pauseCtx = &play->pauseCtx;
    GameOverContext* gameOverCtx = &play->gameOverCtx;
    s16* quadsX;
    s16* quadsWidth;
    s16* quadsY;
    s16* quadsHeight;
    s16 pageBgQuadX;
    s16 pageBgQuadY;
    s16 i;
    s16 j;
    s16 k;

    // Vertices for KaleidoScope_DrawPageSections

    pageBgQuadX = 0 - (PAGE_BG_COLS * PAGE_BG_QUAD_WIDTH) / 2 - PAGE_BG_QUAD_WIDTH;

    // For each column
    for (k = 0, i = 0; i < 3; i++) {
        pageBgQuadX += PAGE_BG_QUAD_WIDTH;

        // For each row
        for (pageBgQuadY = (PAGE_BG_ROWS * PAGE_BG_QUAD_HEIGHT) / 2, j = 0; j < 5; j++, k += 4, pageBgQuadY -= 32) {
            vtx[k + 0].v.ob[0] = vtx[k + 2].v.ob[0] = pageBgQuadX;
            vtx[k + 1].v.ob[0] = vtx[k + 3].v.ob[0] = vtx[k + 0].v.ob[0] + PAGE_BG_QUAD_WIDTH;

            vtx[k + 0].v.ob[1] = vtx[k + 1].v.ob[1] = pageBgQuadY + pauseCtx->offsetY;
            vtx[k + 2].v.ob[1] = vtx[k + 3].v.ob[1] = vtx[k + 0].v.ob[1] - PAGE_BG_QUAD_HEIGHT;

            vtx[k + 0].v.ob[2] = vtx[k + 1].v.ob[2] = vtx[k + 2].v.ob[2] = vtx[k + 3].v.ob[2] = 0;

            vtx[k + 0].v.flag = vtx[k + 1].v.flag = vtx[k + 2].v.flag = vtx[k + 3].v.flag = 0;

            vtx[k + 0].v.tc[0] = vtx[k + 0].v.tc[1] = 0;
            vtx[k + 1].v.tc[1] = vtx[k + 2].v.tc[0] = 0;
            vtx[k + 1].v.tc[0] = vtx[k + 3].v.tc[0] = PAGE_BG_QUAD_TEX_WIDTH * (1 << 5);
            vtx[k + 2].v.tc[1] = vtx[k + 3].v.tc[1] = PAGE_BG_QUAD_TEX_HEIGHT * (1 << 5);

            vtx[k + 0].v.cn[0] = vtx[k + 2].v.cn[0] = vtx[k + 0].v.cn[1] = vtx[k + 2].v.cn[1] = vtx[k + 0].v.cn[2] =
                vtx[k + 2].v.cn[2] = 0;

            vtx[k + 1].v.cn[0] = vtx[k + 3].v.cn[0] = vtx[k + 1].v.cn[1] = vtx[k + 3].v.cn[1] = vtx[k + 1].v.cn[2] =
                vtx[k + 3].v.cn[2] = 0;

            vtx[k + 0].v.cn[3] = vtx[k + 2].v.cn[3] = vtx[k + 1].v.cn[3] = vtx[k + 3].v.cn[3] = pauseCtx->alpha;
        }
    }

    if (numQuads != 0) {
        quadsX = sVtxPageQuadsX[vtxPage];
        quadsWidth = sVtxPageQuadsWidth[vtxPage];
        quadsY = sVtxPageQuadsY[vtxPage];
        quadsHeight = sVtxPageQuadsHeight[vtxPage];

        for (i = 0; i < numQuads; i++, k += 4) {
            vtx[k + 2].v.ob[0] = vtx[k + 0].v.ob[0] = quadsX[i];

            vtx[k + 1].v.ob[0] = vtx[k + 3].v.ob[0] = vtx[k + 0].v.ob[0] + quadsWidth[i];

            if (!IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
                vtx[k + 0].v.ob[1] = vtx[k + 1].v.ob[1] = quadsY[i] + pauseCtx->offsetY;
            } else if (gameOverCtx->state == GAMEOVER_INACTIVE) {
                vtx[k + 0].v.ob[1] = vtx[k + 1].v.ob[1] = quadsY[i] + pauseCtx->offsetY;
            } else {
                vtx[k + 0].v.ob[1] = vtx[k + 1].v.ob[1] = sVtxPageGameOverSaveQuadsY[i] + pauseCtx->offsetY;
            }

            vtx[k + 2].v.ob[1] = vtx[k + 3].v.ob[1] = vtx[k + 0].v.ob[1] - quadsHeight[i];

            vtx[k + 0].v.ob[2] = vtx[k + 1].v.ob[2] = vtx[k + 2].v.ob[2] = vtx[k + 3].v.ob[2] = 0;

            vtx[k + 0].v.flag = vtx[k + 1].v.flag = vtx[k + 2].v.flag = vtx[k + 3].v.flag = 0;

            vtx[k + 0].v.tc[0] = vtx[k + 0].v.tc[1] = vtx[k + 1].v.tc[1] = vtx[k + 2].v.tc[0] = 0;
            vtx[k + 1].v.tc[0] = vtx[k + 3].v.tc[0] = quadsWidth[i] << 5;
            vtx[k + 2].v.tc[1] = vtx[k + 3].v.tc[1] = quadsHeight[i] << 5;

            vtx[k + 0].v.cn[0] = vtx[k + 2].v.cn[0] = vtx[k + 0].v.cn[1] = vtx[k + 2].v.cn[1] = vtx[k + 0].v.cn[2] =
                vtx[k + 2].v.cn[2] = 255;

            vtx[k + 1].v.cn[0] = vtx[k + 3].v.cn[0] = vtx[k + 1].v.cn[1] = vtx[k + 3].v.cn[1] = vtx[k + 1].v.cn[2] =
                vtx[k + 3].v.cn[2] = 255;

            vtx[k + 0].v.cn[3] = vtx[k + 2].v.cn[3] = vtx[k + 1].v.cn[3] = vtx[k + 3].v.cn[3] = pauseCtx->alpha;
        }
    }
    return k;
}

s16 sQuestVtxRectLeft[] = {
    45,   // QUEST_REMAINS_ODOLWA
    78,   // QUEST_REMAINS_GOHT
    10,   // QUEST_REMAINS_GYORG
    45,   // QUEST_REMAINS_TWINMOLD
    80,   // QUEST_SHIELD
    11,   // QUEST_SWORD
    -109, // QUEST_SONG_SONATA
    -87,  // QUEST_SONG_LULLABY
    -65,  // QUEST_SONG_BOSSA_NOVA
    -41,  // QUEST_SONG_ELEGY
    -19,  // QUEST_SONG_OATH
    -18,  // QUEST_SONG_SARIA
    -109, // QUEST_SONG_TIME
    -87,  // QUEST_SONG_HEALING
    -65,  // QUEST_SONG_EPONA
    -41,  // QUEST_SONG_SOARING
    -19,  // QUEST_SONG_STORMS
    -18,  // QUEST_SONG_SUN
    -103, // QUEST_BOMBERS_NOTEBOOK
    7,    // QUEST_QUIVER
    82,   // QUEST_BOMB_BAG
    -110, // QUEST_SKULL_TOKEN (unused)
    -54,  // QUEST_HEART_PIECE
    -98,  // ocarina song button index 0
    -86,  // ocarina song button index 1
    -74,  // ocarina song button index 2
    -62,  // ocarina song button index 3
    -50,  // ocarina song button index 4
    -38,  // ocarina song button index 5
    -26,  // ocarina song button index 6
    -14,  // ocarina song button index 7
    -98,  // ocarina input button index 0
    -86,  // ocarina input button index 1
    -74,  // ocarina input button index 2
    -62,  // ocarina input button index 3
    -50,  // ocarina input button index 4
    -38,  // ocarina input button index 5
    -26,  // ocarina input button index 6
    -14,  // ocarina input button index 7
};

s16 sQuestVtxRectTop[] = {
    62,  // QUEST_REMAINS_ODOLWA
    42,  // QUEST_REMAINS_GOHT
    42,  // QUEST_REMAINS_GYORG
    20,  // QUEST_REMAINS_TWINMOLD
    -9,  // QUEST_SHIELD
    -9,  // QUEST_SWORD
    -20, // QUEST_SONG_SONATA
    -20, // QUEST_SONG_LULLABY
    -20, // QUEST_SONG_BOSSA_NOVA
    -20, // QUEST_SONG_ELEGY
    -20, // QUEST_SONG_OATH
    -20, // QUEST_SONG_SARIA
    2,   // QUEST_SONG_TIME
    2,   // QUEST_SONG_HEALING
    2,   // QUEST_SONG_EPONA
    2,   // QUEST_SONG_SOARING
    2,   // QUEST_SONG_STORMS
    2,   // QUEST_SONG_SUN
    54,  // QUEST_BOMBERS_NOTEBOOK
    -44, // QUEST_QUIVER
    -44, // QUEST_BOMB_BAG
    34,  // QUEST_SKULL_TOKEN (unused)
    58,  // QUEST_HEART_PIECE
    -52, // ocarina song button index 0
    -52, // ocarina song button index 1
    -52, // ocarina song button index 2
    -52, // ocarina song button index 3
    -52, // ocarina song button index 4
    -52, // ocarina song button index 5
    -52, // ocarina song button index 6
    -52, // ocarina song button index 7
    -52, // ocarina input button index 0
    -52, // ocarina input button index 1
    -52, // ocarina input button index 2
    -52, // ocarina input button index 3
    -52, // ocarina input button index 4
    -52, // ocarina input button index 5
    -52, // ocarina input button index 6
    -52, // ocarina input button index 7
};

s16 sQuestVtxWidths[] = {
    32, // QUEST_REMAINS_ODOLWA
    32, // QUEST_REMAINS_GOHT
    32, // QUEST_REMAINS_GYORG
    32, // QUEST_REMAINS_TWINMOLD
    32, // QUEST_SHIELD
    32, // QUEST_SWORD
    16, // QUEST_SONG_SONATA
    16, // QUEST_SONG_LULLABY
    16, // QUEST_SONG_BOSSA_NOVA
    16, // QUEST_SONG_ELEGY
    16, // QUEST_SONG_OATH
    16, // QUEST_SONG_SARIA
    16, // QUEST_SONG_TIME
    16, // QUEST_SONG_HEALING
    16, // QUEST_SONG_EPONA
    16, // QUEST_SONG_SOARING
    16, // QUEST_SONG_STORMS
    16, // QUEST_SONG_SUN
    32, // QUEST_BOMBERS_NOTEBOOK
    32, // QUEST_QUIVER
    32, // QUEST_BOMB_BAG
    24, // QUEST_SKULL_TOKEN (unused)
    48, // QUEST_HEART_PIECE
    16, // ocarina song button index 0
    16, // ocarina song button index 1
    16, // ocarina song button index 2
    16, // ocarina song button index 3
    16, // ocarina song button index 4
    16, // ocarina song button index 5
    16, // ocarina song button index 6
    16, // ocarina song button index 7
    16, // ocarina input button index 0
    16, // ocarina input button index 1
    16, // ocarina input button index 2
    16, // ocarina input button index 3
    16, // ocarina input button index 4
    16, // ocarina input button index 5
    16, // ocarina input button index 6
    16, // ocarina input button index 7
};

s16 sQuestVtxHeights[] = {
    32, // QUEST_REMAINS_ODOLWA
    32, // QUEST_REMAINS_GOHT
    32, // QUEST_REMAINS_GYORG
    32, // QUEST_REMAINS_TWINMOLD
    32, // QUEST_SHIELD
    32, // QUEST_SWORD
    24, // QUEST_SONG_SONATA
    24, // QUEST_SONG_LULLABY
    24, // QUEST_SONG_BOSSA_NOVA
    24, // QUEST_SONG_ELEGY
    24, // QUEST_SONG_OATH
    24, // QUEST_SONG_SARIA
    24, // QUEST_SONG_TIME
    24, // QUEST_SONG_HEALING
    24, // QUEST_SONG_EPONA
    24, // QUEST_SONG_SOARING
    24, // QUEST_SONG_STORMS
    24, // QUEST_SONG_SUN
    32, // QUEST_BOMBERS_NOTEBOOK
    32, // QUEST_QUIVER
    32, // QUEST_BOMB_BAG
    24, // QUEST_SKULL_TOKEN (unused)
    48, // QUEST_HEART_PIECE
    16, // ocarina song button index 0
    16, // ocarina song button index 1
    16, // ocarina song button index 2
    16, // ocarina song button index 3
    16, // ocarina song button index 4
    16, // ocarina song button index 5
    16, // ocarina song button index 6
    16, // ocarina song button index 7
    16, // ocarina input button index 0
    16, // ocarina input button index 1
    16, // ocarina input button index 2
    16, // ocarina input button index 3
    16, // ocarina input button index 4
    16, // ocarina input button index 5
    16, // ocarina input button index 6
    16, // ocarina input button index 7
};

void KaleidoScope_SetVertices(PlayState* play, GraphicsContext* gfxCtx) {
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 i;
    s16 j;
    s16 k;
    s16 vtx_x;
    s16 vtx_y;

    pauseCtx->offsetY = 0;

    if ((pauseCtx->state == PAUSE_STATE_OPENING_3) || (pauseCtx->state == PAUSE_STATE_OWL_WARP_3) ||
        (pauseCtx->state >= PAUSE_STATE_OWL_WARP_6) ||
        ((pauseCtx->state == PAUSE_STATE_SAVEPROMPT) && ((pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_3) ||
                                                         (pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_7)))) {
        pauseCtx->offsetY = 80;
    }

    if (pauseCtx->pageIndex != PAUSE_QUEST) {
        pauseCtx->itemPageVtx = GRAPH_ALLOC(gfxCtx, ((PAGE_BG_QUADS + VTX_PAGE_ITEM_QUADS) * 4) * sizeof(Vtx));
        KaleidoScope_SetPageVertices(play, pauseCtx->itemPageVtx, VTX_PAGE_ITEM, VTX_PAGE_ITEM_QUADS);

        // #region 2S2H [Dpad] Increase size of allocation when you have dpad items
        u32 quadItemMax = QUAD_ITEM_MAX;
        quadItemMax += EQUIP_SLOT_D_MAX;
        pauseCtx->itemVtx = GRAPH_ALLOC(gfxCtx, (quadItemMax * 4) * sizeof(Vtx));
        // #endregion

        // QUAD_ITEM_GRID_FIRST..QUAD_ITEM_GRID_LAST

        // Loop over grid rows
        for (k = 0, i = 0, vtx_y = (ITEM_GRID_ROWS * ITEM_GRID_CELL_HEIGHT) / 2 - 6; k < ITEM_GRID_ROWS;
             k++, vtx_y -= ITEM_GRID_CELL_HEIGHT) {
            // Loop over grid columns
            for (vtx_x = 0 - (ITEM_GRID_COLS * ITEM_GRID_CELL_WIDTH) / 2, j = 0; j < ITEM_GRID_COLS;
                 j++, i += 4, vtx_x += ITEM_GRID_CELL_WIDTH) {
                pauseCtx->itemVtx[i + 0].v.ob[0] = pauseCtx->itemVtx[i + 2].v.ob[0] = vtx_x + ITEM_GRID_QUAD_MARGIN;
                pauseCtx->itemVtx[i + 1].v.ob[0] = pauseCtx->itemVtx[i + 3].v.ob[0] =
                    pauseCtx->itemVtx[i + 0].v.ob[0] + ITEM_GRID_QUAD_WIDTH;

                pauseCtx->itemVtx[i + 0].v.ob[1] = pauseCtx->itemVtx[i + 1].v.ob[1] =
                    vtx_y + pauseCtx->offsetY - ITEM_GRID_QUAD_MARGIN;
                pauseCtx->itemVtx[i + 2].v.ob[1] = pauseCtx->itemVtx[i + 3].v.ob[1] =
                    pauseCtx->itemVtx[i + 0].v.ob[1] - ITEM_GRID_QUAD_WIDTH;

                pauseCtx->itemVtx[i + 0].v.ob[2] = pauseCtx->itemVtx[i + 1].v.ob[2] = pauseCtx->itemVtx[i + 2].v.ob[2] =
                    pauseCtx->itemVtx[i + 3].v.ob[2] = 0;

                pauseCtx->itemVtx[i + 0].v.flag = pauseCtx->itemVtx[i + 1].v.flag = pauseCtx->itemVtx[i + 2].v.flag =
                    pauseCtx->itemVtx[i + 3].v.flag = 0;

                pauseCtx->itemVtx[i + 0].v.tc[0] = pauseCtx->itemVtx[i + 0].v.tc[1] = pauseCtx->itemVtx[i + 1].v.tc[1] =
                    pauseCtx->itemVtx[i + 2].v.tc[0] = 0;

                pauseCtx->itemVtx[i + 1].v.tc[0] = pauseCtx->itemVtx[i + 2].v.tc[1] = pauseCtx->itemVtx[i + 3].v.tc[0] =
                    pauseCtx->itemVtx[i + 3].v.tc[1] = ITEM_GRID_QUAD_TEX_SIZE * (1 << 5);

                pauseCtx->itemVtx[i + 0].v.cn[0] = pauseCtx->itemVtx[i + 1].v.cn[0] = pauseCtx->itemVtx[i + 2].v.cn[0] =
                    pauseCtx->itemVtx[i + 3].v.cn[0] = pauseCtx->itemVtx[i + 0].v.cn[1] =
                        pauseCtx->itemVtx[i + 1].v.cn[1] = pauseCtx->itemVtx[i + 2].v.cn[1] =
                            pauseCtx->itemVtx[i + 3].v.cn[1] = pauseCtx->itemVtx[i + 0].v.cn[2] =
                                pauseCtx->itemVtx[i + 1].v.cn[2] = pauseCtx->itemVtx[i + 2].v.cn[2] =
                                    pauseCtx->itemVtx[i + 3].v.cn[2] = 255;

                pauseCtx->itemVtx[i + 0].v.cn[3] = pauseCtx->itemVtx[i + 1].v.cn[3] = pauseCtx->itemVtx[i + 2].v.cn[3] =
                    pauseCtx->itemVtx[i + 3].v.cn[3] = 255;
            }
        }

        // QUAD_ITEM_GRID_SELECTED_C_LEFT, QUAD_ITEM_GRID_SELECTED_C_DOWN, QUAD_ITEM_GRID_SELECTED_C_RIGHT

        for (j = EQUIP_SLOT_C_LEFT; j <= EQUIP_SLOT_C_RIGHT; j++, i += 4) {
            if (GET_CUR_FORM_BTN_SLOT(j) != ITEM_NONE) {
                k = GET_CUR_FORM_BTN_SLOT(j) * 4;

                pauseCtx->itemVtx[i + 0].v.ob[0] = pauseCtx->itemVtx[i + 2].v.ob[0] =
                    pauseCtx->itemVtx[k].v.ob[0] + ITEM_GRID_SELECTED_QUAD_MARGIN;

                pauseCtx->itemVtx[i + 1].v.ob[0] = pauseCtx->itemVtx[i + 3].v.ob[0] =
                    pauseCtx->itemVtx[i + 0].v.ob[0] + ITEM_GRID_SELECTED_QUAD_WIDTH;

                pauseCtx->itemVtx[i + 0].v.ob[1] = pauseCtx->itemVtx[i + 1].v.ob[1] =
                    pauseCtx->itemVtx[k].v.ob[1] - ITEM_GRID_SELECTED_QUAD_MARGIN;

                pauseCtx->itemVtx[i + 2].v.ob[1] = pauseCtx->itemVtx[i + 3].v.ob[1] =
                    pauseCtx->itemVtx[i + 0].v.ob[1] - ITEM_GRID_SELECTED_QUAD_WIDTH;

                pauseCtx->itemVtx[i + 0].v.ob[2] = pauseCtx->itemVtx[i + 1].v.ob[2] = pauseCtx->itemVtx[i + 2].v.ob[2] =
                    pauseCtx->itemVtx[i + 3].v.ob[2] = 0;

                pauseCtx->itemVtx[i + 0].v.flag = pauseCtx->itemVtx[i + 1].v.flag = pauseCtx->itemVtx[i + 2].v.flag =
                    pauseCtx->itemVtx[i + 3].v.flag = 0;

                pauseCtx->itemVtx[i + 0].v.tc[0] = pauseCtx->itemVtx[i + 0].v.tc[1] = pauseCtx->itemVtx[i + 1].v.tc[1] =
                    pauseCtx->itemVtx[i + 2].v.tc[0] = 0;

                pauseCtx->itemVtx[i + 1].v.tc[0] = pauseCtx->itemVtx[i + 2].v.tc[1] = pauseCtx->itemVtx[i + 3].v.tc[0] =
                    pauseCtx->itemVtx[i + 3].v.tc[1] = ITEM_GRID_SELECTED_QUAD_TEX_SIZE * (1 << 5);

                pauseCtx->itemVtx[i + 0].v.cn[0] = pauseCtx->itemVtx[i + 1].v.cn[0] = pauseCtx->itemVtx[i + 2].v.cn[0] =
                    pauseCtx->itemVtx[i + 3].v.cn[0] = pauseCtx->itemVtx[i + 0].v.cn[1] =
                        pauseCtx->itemVtx[i + 1].v.cn[1] = pauseCtx->itemVtx[i + 2].v.cn[1] =
                            pauseCtx->itemVtx[i + 3].v.cn[1] = pauseCtx->itemVtx[i + 0].v.cn[2] =
                                pauseCtx->itemVtx[i + 1].v.cn[2] = pauseCtx->itemVtx[i + 2].v.cn[2] =
                                    pauseCtx->itemVtx[i + 3].v.cn[2] = 255;

                pauseCtx->itemVtx[i + 0].v.cn[3] = pauseCtx->itemVtx[i + 1].v.cn[3] = pauseCtx->itemVtx[i + 2].v.cn[3] =
                    pauseCtx->itemVtx[i + 3].v.cn[3] = pauseCtx->alpha;
            } else {
                // No item equipped on the C button, put the quad out of view

                pauseCtx->itemVtx[i + 2].v.ob[0] = -300;
                pauseCtx->itemVtx[i + 0].v.ob[0] = pauseCtx->itemVtx[i + 2].v.ob[0];

                pauseCtx->itemVtx[i + 1].v.ob[0] = pauseCtx->itemVtx[i + 3].v.ob[0] =
                    pauseCtx->itemVtx[i + 0].v.ob[0] + ITEM_GRID_SELECTED_QUAD_WIDTH;

                pauseCtx->itemVtx[i + 0].v.ob[1] = pauseCtx->itemVtx[i + 1].v.ob[1] = 300;
                pauseCtx->itemVtx[i + 2].v.ob[1] = pauseCtx->itemVtx[i + 3].v.ob[1] =
                    pauseCtx->itemVtx[i + 0].v.ob[1] - ITEM_GRID_SELECTED_QUAD_HEIGHT;
            }
        }
        // #region 2S2H [Dpad]
        for (j = EQUIP_SLOT_D_RIGHT; j <= EQUIP_SLOT_D_UP; j++, i += 4) {
            if (DPAD_GET_CUR_FORM_BTN_SLOT(j) != ITEM_NONE) {
                k = DPAD_GET_CUR_FORM_BTN_SLOT(j) * 4;

                pauseCtx->itemVtx[i + 0].v.ob[0] = pauseCtx->itemVtx[i + 2].v.ob[0] =
                    pauseCtx->itemVtx[k].v.ob[0] + ITEM_GRID_SELECTED_QUAD_MARGIN;

                pauseCtx->itemVtx[i + 1].v.ob[0] = pauseCtx->itemVtx[i + 3].v.ob[0] =
                    pauseCtx->itemVtx[i + 0].v.ob[0] + ITEM_GRID_SELECTED_QUAD_WIDTH;

                pauseCtx->itemVtx[i + 0].v.ob[1] = pauseCtx->itemVtx[i + 1].v.ob[1] =
                    pauseCtx->itemVtx[k].v.ob[1] - ITEM_GRID_SELECTED_QUAD_MARGIN;

                pauseCtx->itemVtx[i + 2].v.ob[1] = pauseCtx->itemVtx[i + 3].v.ob[1] =
                    pauseCtx->itemVtx[i + 0].v.ob[1] - ITEM_GRID_SELECTED_QUAD_WIDTH;

                pauseCtx->itemVtx[i + 0].v.ob[2] = pauseCtx->itemVtx[i + 1].v.ob[2] = pauseCtx->itemVtx[i + 2].v.ob[2] =
                    pauseCtx->itemVtx[i + 3].v.ob[2] = 0;

                pauseCtx->itemVtx[i + 0].v.flag = pauseCtx->itemVtx[i + 1].v.flag = pauseCtx->itemVtx[i + 2].v.flag =
                    pauseCtx->itemVtx[i + 3].v.flag = 0;

                pauseCtx->itemVtx[i + 0].v.tc[0] = pauseCtx->itemVtx[i + 0].v.tc[1] = pauseCtx->itemVtx[i + 1].v.tc[1] =
                    pauseCtx->itemVtx[i + 2].v.tc[0] = 0;

                pauseCtx->itemVtx[i + 1].v.tc[0] = pauseCtx->itemVtx[i + 2].v.tc[1] = pauseCtx->itemVtx[i + 3].v.tc[0] =
                    pauseCtx->itemVtx[i + 3].v.tc[1] = ITEM_GRID_SELECTED_QUAD_TEX_SIZE * (1 << 5);

                pauseCtx->itemVtx[i + 0].v.cn[0] = pauseCtx->itemVtx[i + 1].v.cn[0] = pauseCtx->itemVtx[i + 2].v.cn[0] =
                    pauseCtx->itemVtx[i + 3].v.cn[0] = pauseCtx->itemVtx[i + 0].v.cn[1] =
                        pauseCtx->itemVtx[i + 1].v.cn[1] = pauseCtx->itemVtx[i + 2].v.cn[1] =
                            pauseCtx->itemVtx[i + 3].v.cn[1] = pauseCtx->itemVtx[i + 0].v.cn[2] =
                                pauseCtx->itemVtx[i + 1].v.cn[2] = pauseCtx->itemVtx[i + 2].v.cn[2] =
                                    pauseCtx->itemVtx[i + 3].v.cn[2] = 255;

                pauseCtx->itemVtx[i + 0].v.cn[3] = pauseCtx->itemVtx[i + 1].v.cn[3] = pauseCtx->itemVtx[i + 2].v.cn[3] =
                    pauseCtx->itemVtx[i + 3].v.cn[3] = pauseCtx->alpha;
            } else {
                // No item equipped on the C button, put the quad out of view

                pauseCtx->itemVtx[i + 2].v.ob[0] = -300;
                pauseCtx->itemVtx[i + 0].v.ob[0] = pauseCtx->itemVtx[i + 2].v.ob[0];

                pauseCtx->itemVtx[i + 1].v.ob[0] = pauseCtx->itemVtx[i + 3].v.ob[0] =
                    pauseCtx->itemVtx[i + 0].v.ob[0] + ITEM_GRID_SELECTED_QUAD_WIDTH;

                pauseCtx->itemVtx[i + 0].v.ob[1] = pauseCtx->itemVtx[i + 1].v.ob[1] = 300;
                pauseCtx->itemVtx[i + 2].v.ob[1] = pauseCtx->itemVtx[i + 3].v.ob[1] =
                    pauseCtx->itemVtx[i + 0].v.ob[1] - ITEM_GRID_SELECTED_QUAD_HEIGHT;
            }
        }
        // #endregion
    }

    if (pauseCtx->pageIndex != PAUSE_MASK) {
        if (!sInDungeonScene) {
            pauseCtx->mapPageVtx = GRAPH_ALLOC(
                gfxCtx, ((PAGE_BG_QUADS + VTX_PAGE_MAP_WORLD_QUADS + WORLD_MAP_IMAGE_FRAG_NUM) * 4) * sizeof(Vtx));
            j = KaleidoScope_SetPageVertices(play, pauseCtx->mapPageVtx, VTX_PAGE_MAP_WORLD, VTX_PAGE_MAP_WORLD_QUADS);

            for (i = 0, vtx_y = 59; i < WORLD_MAP_IMAGE_FRAG_NUM; i++, j += 4, vtx_y -= WORLD_MAP_IMAGE_FRAG_HEIGHT) {
                pauseCtx->mapPageVtx[j + 2].v.ob[0] = 0 - (WORLD_MAP_IMAGE_WIDTH / 2) - 1;
                pauseCtx->mapPageVtx[j + 0].v.ob[0] = pauseCtx->mapPageVtx[j + 2].v.ob[0];

                pauseCtx->mapPageVtx[j + 1].v.ob[0] = pauseCtx->mapPageVtx[j + 3].v.ob[0] =
                    pauseCtx->mapPageVtx[j + 0].v.ob[0] + WORLD_MAP_IMAGE_WIDTH;

                pauseCtx->mapPageVtx[j + 0].v.ob[1] = pauseCtx->mapPageVtx[j + 1].v.ob[1] = vtx_y + pauseCtx->offsetY;

                pauseCtx->mapPageVtx[j + 2].v.ob[1] = pauseCtx->mapPageVtx[j + 3].v.ob[1] =
                    pauseCtx->mapPageVtx[j + 0].v.ob[1] - WORLD_MAP_IMAGE_FRAG_HEIGHT;

                pauseCtx->mapPageVtx[j + 0].v.ob[2] = pauseCtx->mapPageVtx[j + 1].v.ob[2] =
                    pauseCtx->mapPageVtx[j + 2].v.ob[2] = pauseCtx->mapPageVtx[j + 3].v.ob[2] = 0;

                pauseCtx->mapPageVtx[j + 0].v.flag = pauseCtx->mapPageVtx[j + 1].v.flag =
                    pauseCtx->mapPageVtx[j + 2].v.flag = pauseCtx->mapPageVtx[j + 3].v.flag = 0;

                pauseCtx->mapPageVtx[j + 0].v.tc[0] = pauseCtx->mapPageVtx[j + 0].v.tc[1] =
                    pauseCtx->mapPageVtx[j + 1].v.tc[1] = pauseCtx->mapPageVtx[j + 2].v.tc[0] = 0;

                pauseCtx->mapPageVtx[j + 1].v.tc[0] = pauseCtx->mapPageVtx[j + 3].v.tc[0] =
                    WORLD_MAP_IMAGE_WIDTH * (1 << 5);
                pauseCtx->mapPageVtx[j + 2].v.tc[1] = pauseCtx->mapPageVtx[j + 3].v.tc[1] =
                    WORLD_MAP_IMAGE_FRAG_HEIGHT * (1 << 5);

                pauseCtx->mapPageVtx[j + 0].v.cn[0] = pauseCtx->mapPageVtx[j + 2].v.cn[0] =
                    pauseCtx->mapPageVtx[j + 0].v.cn[1] = pauseCtx->mapPageVtx[j + 2].v.cn[1] =
                        pauseCtx->mapPageVtx[j + 0].v.cn[2] = pauseCtx->mapPageVtx[j + 2].v.cn[2] =
                            pauseCtx->mapPageVtx[j + 1].v.cn[0] = pauseCtx->mapPageVtx[j + 3].v.cn[0] =
                                pauseCtx->mapPageVtx[j + 1].v.cn[1] = pauseCtx->mapPageVtx[j + 3].v.cn[1] =
                                    pauseCtx->mapPageVtx[j + 1].v.cn[2] = pauseCtx->mapPageVtx[j + 3].v.cn[2] =
                                        pauseCtx->mapPageVtx[j + 0].v.cn[3] = pauseCtx->mapPageVtx[j + 2].v.cn[3] =
                                            pauseCtx->mapPageVtx[j + 1].v.cn[3] = pauseCtx->mapPageVtx[j + 3].v.cn[3] =
                                                pauseCtx->alpha;
            }

            pauseCtx->mapPageVtx[j - 2].v.ob[1] = pauseCtx->mapPageVtx[j - 1].v.ob[1] =
                pauseCtx->mapPageVtx[j - 4].v.ob[1] - (WORLD_MAP_IMAGE_HEIGHT % WORLD_MAP_IMAGE_FRAG_HEIGHT);

            pauseCtx->mapPageVtx[j - 2].v.tc[1] = pauseCtx->mapPageVtx[j - 1].v.tc[1] =
                (WORLD_MAP_IMAGE_HEIGHT % WORLD_MAP_IMAGE_FRAG_HEIGHT) * (1 << 5);

        } else {
            pauseCtx->mapPageVtx =
                GRAPH_ALLOC(gfxCtx, ((PAGE_BG_QUADS + VTX_PAGE_MAP_DUNGEON_QUADS) * 4) * sizeof(Vtx));
            KaleidoScope_SetPageVertices(play, pauseCtx->mapPageVtx, VTX_PAGE_MAP_DUNGEON, VTX_PAGE_MAP_DUNGEON_QUADS);
        }
    }

    if (pauseCtx->pageIndex != PAUSE_ITEM) {
        pauseCtx->questPageVtx = GRAPH_ALLOC(gfxCtx, ((PAGE_BG_QUADS + VTX_PAGE_QUEST_QUADS) * 4) * sizeof(Vtx));
        KaleidoScope_SetPageVertices(play, pauseCtx->questPageVtx, VTX_PAGE_QUEST, VTX_PAGE_QUEST_QUADS);

        pauseCtx->questVtx = GRAPH_ALLOC(gfxCtx, (39 * 4) * sizeof(Vtx));

        for (k = 0, j = 0; j < 39; j++, k += 4) {
            if (j <= QUEST_SWORD) {
                pauseCtx->questVtx[k + 0].v.ob[0] = pauseCtx->questVtx[k + 2].v.ob[0] = sQuestVtxRectLeft[j];

                pauseCtx->questVtx[k + 1].v.ob[0] = pauseCtx->questVtx[k + 3].v.ob[0] =
                    pauseCtx->questVtx[k + 0].v.ob[0] + sQuestVtxWidths[j];

                pauseCtx->questVtx[k + 0].v.ob[1] = pauseCtx->questVtx[k + 1].v.ob[1] =
                    sQuestVtxRectTop[j] + pauseCtx->offsetY;

                pauseCtx->questVtx[k + 2].v.ob[1] = pauseCtx->questVtx[k + 3].v.ob[1] =
                    pauseCtx->questVtx[k + 0].v.ob[1] - sQuestVtxHeights[j];
            } else {
                pauseCtx->questVtx[k + 0].v.ob[0] = pauseCtx->questVtx[k + 2].v.ob[0] = sQuestVtxRectLeft[j] + 2;

                pauseCtx->questVtx[k + 1].v.ob[0] = pauseCtx->questVtx[k + 3].v.ob[0] =
                    pauseCtx->questVtx[k + 0].v.ob[0] + sQuestVtxWidths[j] - 4;

                pauseCtx->questVtx[k + 0].v.ob[1] = pauseCtx->questVtx[k + 1].v.ob[1] =
                    sQuestVtxRectTop[j] + pauseCtx->offsetY - 2;

                pauseCtx->questVtx[k + 2].v.ob[1] = pauseCtx->questVtx[k + 3].v.ob[1] =
                    pauseCtx->questVtx[k + 0].v.ob[1] - sQuestVtxHeights[j] + 4;
            }

            pauseCtx->questVtx[k + 0].v.ob[2] = pauseCtx->questVtx[k + 1].v.ob[2] = pauseCtx->questVtx[k + 2].v.ob[2] =
                pauseCtx->questVtx[k + 3].v.ob[2] = 0;

            pauseCtx->questVtx[k + 0].v.flag = pauseCtx->questVtx[k + 1].v.flag = pauseCtx->questVtx[k + 2].v.flag =
                pauseCtx->questVtx[k + 3].v.flag = 0;

            pauseCtx->questVtx[k + 0].v.tc[0] = pauseCtx->questVtx[k + 0].v.tc[1] = pauseCtx->questVtx[k + 1].v.tc[1] =
                pauseCtx->questVtx[k + 2].v.tc[0] = 0;

            pauseCtx->questVtx[k + 1].v.tc[0] = pauseCtx->questVtx[k + 3].v.tc[0] = sQuestVtxWidths[j] << 5;
            pauseCtx->questVtx[k + 2].v.tc[1] = pauseCtx->questVtx[k + 3].v.tc[1] = sQuestVtxHeights[j] << 5;

            pauseCtx->questVtx[k + 0].v.cn[0] = pauseCtx->questVtx[k + 1].v.cn[0] = pauseCtx->questVtx[k + 2].v.cn[0] =
                pauseCtx->questVtx[k + 3].v.cn[0] = pauseCtx->questVtx[k + 0].v.cn[1] =
                    pauseCtx->questVtx[k + 1].v.cn[1] = pauseCtx->questVtx[k + 2].v.cn[1] =
                        pauseCtx->questVtx[k + 3].v.cn[1] = pauseCtx->questVtx[k + 0].v.cn[2] =
                            pauseCtx->questVtx[k + 1].v.cn[2] = pauseCtx->questVtx[k + 2].v.cn[2] =
                                pauseCtx->questVtx[k + 3].v.cn[2] = 255;

            pauseCtx->questVtx[k + 0].v.cn[3] = pauseCtx->questVtx[k + 1].v.cn[3] = pauseCtx->questVtx[k + 2].v.cn[3] =
                pauseCtx->questVtx[k + 3].v.cn[3] = pauseCtx->alpha;
        }
    }

    if (pauseCtx->pageIndex != PAUSE_MAP) {
        pauseCtx->maskPageVtx = GRAPH_ALLOC(gfxCtx, ((PAGE_BG_QUADS + VTX_PAGE_MASK_QUADS) * 4) * sizeof(Vtx));
        KaleidoScope_SetPageVertices(play, pauseCtx->maskPageVtx, VTX_PAGE_MASK, VTX_PAGE_MASK_QUADS);

        // #region 2S2H [Dpad] Increase size of allocation when you have dpad items
        u32 quadMaskMax = QUAD_MASK_MAX;
        quadMaskMax += EQUIP_SLOT_D_MAX;
        pauseCtx->maskVtx = GRAPH_ALLOC(gfxCtx, (quadMaskMax * 4) * sizeof(Vtx));
        // #endregion

        // Loop over grid rows
        for (k = 0, i = 0, vtx_y = (MASK_GRID_ROWS * MASK_GRID_CELL_HEIGHT) / 2 - 6; k < MASK_GRID_ROWS;
             k++, vtx_y -= MASK_GRID_CELL_HEIGHT) {
            // Loop over grid columns
            for (vtx_x = 0 - (MASK_GRID_COLS * MASK_GRID_CELL_WIDTH) / 2, j = 0; j < MASK_GRID_COLS;
                 j++, i += 4, vtx_x += MASK_GRID_CELL_WIDTH) {
                pauseCtx->maskVtx[i + 0].v.ob[0] = pauseCtx->maskVtx[i + 2].v.ob[0] = vtx_x + MASK_GRID_QUAD_MARGIN;
                pauseCtx->maskVtx[i + 1].v.ob[0] = pauseCtx->maskVtx[i + 3].v.ob[0] =
                    pauseCtx->maskVtx[i + 0].v.ob[0] + MASK_GRID_QUAD_WIDTH;

                pauseCtx->maskVtx[i + 0].v.ob[1] = pauseCtx->maskVtx[i + 1].v.ob[1] =
                    vtx_y + pauseCtx->offsetY - MASK_GRID_QUAD_MARGIN;
                pauseCtx->maskVtx[i + 2].v.ob[1] = pauseCtx->maskVtx[i + 3].v.ob[1] =
                    pauseCtx->maskVtx[i + 0].v.ob[1] - MASK_GRID_QUAD_HEIGHT;

                pauseCtx->maskVtx[i + 0].v.ob[2] = pauseCtx->maskVtx[i + 1].v.ob[2] = pauseCtx->maskVtx[i + 2].v.ob[2] =
                    pauseCtx->maskVtx[i + 3].v.ob[2] = 0;

                pauseCtx->maskVtx[i + 0].v.flag = pauseCtx->maskVtx[i + 1].v.flag = pauseCtx->maskVtx[i + 2].v.flag =
                    pauseCtx->maskVtx[i + 3].v.flag = 0;

                pauseCtx->maskVtx[i + 0].v.tc[0] = pauseCtx->maskVtx[i + 0].v.tc[1] = pauseCtx->maskVtx[i + 1].v.tc[1] =
                    pauseCtx->maskVtx[i + 2].v.tc[0] = 0;

                pauseCtx->maskVtx[i + 1].v.tc[0] = pauseCtx->maskVtx[i + 2].v.tc[1] = pauseCtx->maskVtx[i + 3].v.tc[0] =
                    pauseCtx->maskVtx[i + 3].v.tc[1] = MASK_GRID_QUAD_TEX_SIZE * (1 << 5);

                pauseCtx->maskVtx[i + 0].v.cn[0] = pauseCtx->maskVtx[i + 1].v.cn[0] = pauseCtx->maskVtx[i + 2].v.cn[0] =
                    pauseCtx->maskVtx[i + 3].v.cn[0] = pauseCtx->maskVtx[i + 0].v.cn[1] =
                        pauseCtx->maskVtx[i + 1].v.cn[1] = pauseCtx->maskVtx[i + 2].v.cn[1] =
                            pauseCtx->maskVtx[i + 3].v.cn[1] = pauseCtx->maskVtx[i + 0].v.cn[2] =
                                pauseCtx->maskVtx[i + 1].v.cn[2] = pauseCtx->maskVtx[i + 2].v.cn[2] =
                                    pauseCtx->maskVtx[i + 3].v.cn[2] = 255;

                pauseCtx->maskVtx[i + 0].v.cn[3] = pauseCtx->maskVtx[i + 1].v.cn[3] = pauseCtx->maskVtx[i + 2].v.cn[3] =
                    pauseCtx->maskVtx[i + 3].v.cn[3] = 255;
            }
        }

        // QUAD_MASK_GRID_SELECTED_C_LEFT, QUAD_MASK_GRID_SELECTED_C_DOWN, QUAD_MASK_GRID_SELECTED_C_RIGHT

        for (j = EQUIP_SLOT_C_LEFT; j <= EQUIP_SLOT_C_RIGHT; j++, i += 4) {
            if (GET_CUR_FORM_BTN_SLOT(j) != ITEM_NONE) {
                k = (GET_CUR_FORM_BTN_SLOT(j) - ITEM_NUM_SLOTS) * 4;

                pauseCtx->maskVtx[i + 0].v.ob[0] = pauseCtx->maskVtx[i + 2].v.ob[0] =
                    pauseCtx->maskVtx[k].v.ob[0] + MASK_GRID_SELECTED_QUAD_MARGIN;

                pauseCtx->maskVtx[i + 1].v.ob[0] = pauseCtx->maskVtx[i + 3].v.ob[0] =
                    pauseCtx->maskVtx[i + 0].v.ob[0] + MASK_GRID_SELECTED_QUAD_WIDTH;

                pauseCtx->maskVtx[i + 0].v.ob[1] = pauseCtx->maskVtx[i + 1].v.ob[1] =
                    pauseCtx->maskVtx[k].v.ob[1] - MASK_GRID_SELECTED_QUAD_MARGIN;

                pauseCtx->maskVtx[i + 2].v.ob[1] = pauseCtx->maskVtx[i + 3].v.ob[1] =
                    pauseCtx->maskVtx[i + 0].v.ob[1] - MASK_GRID_SELECTED_QUAD_HEIGHT;

                pauseCtx->maskVtx[i + 0].v.ob[2] = pauseCtx->maskVtx[i + 1].v.ob[2] = pauseCtx->maskVtx[i + 2].v.ob[2] =
                    pauseCtx->maskVtx[i + 3].v.ob[2] = 0;

                pauseCtx->maskVtx[i + 0].v.flag = pauseCtx->maskVtx[i + 1].v.flag = pauseCtx->maskVtx[i + 2].v.flag =
                    pauseCtx->maskVtx[i + 3].v.flag = 0;

                pauseCtx->maskVtx[i + 0].v.tc[0] = pauseCtx->maskVtx[i + 0].v.tc[1] = pauseCtx->maskVtx[i + 1].v.tc[1] =
                    pauseCtx->maskVtx[i + 2].v.tc[0] = 0;

                pauseCtx->maskVtx[i + 1].v.tc[0] = pauseCtx->maskVtx[i + 2].v.tc[1] = pauseCtx->maskVtx[i + 3].v.tc[0] =
                    pauseCtx->maskVtx[i + 3].v.tc[1] = MASK_GRID_SELECTED_QUAD_TEX_SIZE * (1 << 5);

                pauseCtx->maskVtx[i + 0].v.cn[0] = pauseCtx->maskVtx[i + 1].v.cn[0] = pauseCtx->maskVtx[i + 2].v.cn[0] =
                    pauseCtx->maskVtx[i + 3].v.cn[0] = pauseCtx->maskVtx[i + 0].v.cn[1] =
                        pauseCtx->maskVtx[i + 1].v.cn[1] = pauseCtx->maskVtx[i + 2].v.cn[1] =
                            pauseCtx->maskVtx[i + 3].v.cn[1] = pauseCtx->maskVtx[i + 0].v.cn[2] =
                                pauseCtx->maskVtx[i + 1].v.cn[2] = pauseCtx->maskVtx[i + 2].v.cn[2] =
                                    pauseCtx->maskVtx[i + 3].v.cn[2] = 255;

                pauseCtx->maskVtx[i + 0].v.cn[3] = pauseCtx->maskVtx[i + 1].v.cn[3] = pauseCtx->maskVtx[i + 2].v.cn[3] =
                    pauseCtx->maskVtx[i + 3].v.cn[3] = pauseCtx->alpha;
            } else {
                pauseCtx->maskVtx[i + 2].v.ob[0] = -300;
                pauseCtx->maskVtx[i + 0].v.ob[0] = pauseCtx->maskVtx[i + 2].v.ob[0];

                pauseCtx->maskVtx[i + 1].v.ob[0] = pauseCtx->maskVtx[i + 3].v.ob[0] =
                    pauseCtx->maskVtx[i + 0].v.ob[0] + MASK_GRID_SELECTED_QUAD_WIDTH;

                pauseCtx->maskVtx[i + 0].v.ob[1] = pauseCtx->maskVtx[i + 1].v.ob[1] = 300;
                pauseCtx->maskVtx[i + 2].v.ob[1] = pauseCtx->maskVtx[i + 3].v.ob[1] =
                    pauseCtx->maskVtx[i + 0].v.ob[1] - MASK_GRID_SELECTED_QUAD_HEIGHT;
            }
        }
        // #region 2S2H [Dpad]
        for (j = EQUIP_SLOT_D_RIGHT; j <= EQUIP_SLOT_D_UP; j++, i += 4) {
            if (DPAD_GET_CUR_FORM_BTN_SLOT(j) != ITEM_NONE) {
                k = (DPAD_GET_CUR_FORM_BTN_SLOT(j) - ITEM_NUM_SLOTS) * 4;

                pauseCtx->maskVtx[i + 0].v.ob[0] = pauseCtx->maskVtx[i + 2].v.ob[0] =
                    pauseCtx->maskVtx[k].v.ob[0] + MASK_GRID_SELECTED_QUAD_MARGIN;

                pauseCtx->maskVtx[i + 1].v.ob[0] = pauseCtx->maskVtx[i + 3].v.ob[0] =
                    pauseCtx->maskVtx[i + 0].v.ob[0] + MASK_GRID_SELECTED_QUAD_WIDTH;

                pauseCtx->maskVtx[i + 0].v.ob[1] = pauseCtx->maskVtx[i + 1].v.ob[1] =
                    pauseCtx->maskVtx[k].v.ob[1] - MASK_GRID_SELECTED_QUAD_MARGIN;

                pauseCtx->maskVtx[i + 2].v.ob[1] = pauseCtx->maskVtx[i + 3].v.ob[1] =
                    pauseCtx->maskVtx[i + 0].v.ob[1] - MASK_GRID_SELECTED_QUAD_HEIGHT;

                pauseCtx->maskVtx[i + 0].v.ob[2] = pauseCtx->maskVtx[i + 1].v.ob[2] = pauseCtx->maskVtx[i + 2].v.ob[2] =
                    pauseCtx->maskVtx[i + 3].v.ob[2] = 0;

                pauseCtx->maskVtx[i + 0].v.flag = pauseCtx->maskVtx[i + 1].v.flag = pauseCtx->maskVtx[i + 2].v.flag =
                    pauseCtx->maskVtx[i + 3].v.flag = 0;

                pauseCtx->maskVtx[i + 0].v.tc[0] = pauseCtx->maskVtx[i + 0].v.tc[1] = pauseCtx->maskVtx[i + 1].v.tc[1] =
                    pauseCtx->maskVtx[i + 2].v.tc[0] = 0;

                pauseCtx->maskVtx[i + 1].v.tc[0] = pauseCtx->maskVtx[i + 2].v.tc[1] = pauseCtx->maskVtx[i + 3].v.tc[0] =
                    pauseCtx->maskVtx[i + 3].v.tc[1] = MASK_GRID_SELECTED_QUAD_TEX_SIZE * (1 << 5);

                pauseCtx->maskVtx[i + 0].v.cn[0] = pauseCtx->maskVtx[i + 1].v.cn[0] = pauseCtx->maskVtx[i + 2].v.cn[0] =
                    pauseCtx->maskVtx[i + 3].v.cn[0] = pauseCtx->maskVtx[i + 0].v.cn[1] =
                        pauseCtx->maskVtx[i + 1].v.cn[1] = pauseCtx->maskVtx[i + 2].v.cn[1] =
                            pauseCtx->maskVtx[i + 3].v.cn[1] = pauseCtx->maskVtx[i + 0].v.cn[2] =
                                pauseCtx->maskVtx[i + 1].v.cn[2] = pauseCtx->maskVtx[i + 2].v.cn[2] =
                                    pauseCtx->maskVtx[i + 3].v.cn[2] = 255;

                pauseCtx->maskVtx[i + 0].v.cn[3] = pauseCtx->maskVtx[i + 1].v.cn[3] = pauseCtx->maskVtx[i + 2].v.cn[3] =
                    pauseCtx->maskVtx[i + 3].v.cn[3] = pauseCtx->alpha;
            } else {
                pauseCtx->maskVtx[i + 2].v.ob[0] = -300;
                pauseCtx->maskVtx[i + 0].v.ob[0] = pauseCtx->maskVtx[i + 2].v.ob[0];

                pauseCtx->maskVtx[i + 1].v.ob[0] = pauseCtx->maskVtx[i + 3].v.ob[0] =
                    pauseCtx->maskVtx[i + 0].v.ob[0] + MASK_GRID_SELECTED_QUAD_WIDTH;

                pauseCtx->maskVtx[i + 0].v.ob[1] = pauseCtx->maskVtx[i + 1].v.ob[1] = 300;
                pauseCtx->maskVtx[i + 2].v.ob[1] = pauseCtx->maskVtx[i + 3].v.ob[1] =
                    pauseCtx->maskVtx[i + 0].v.ob[1] - MASK_GRID_SELECTED_QUAD_HEIGHT;
            }
        }
        // #endregion
    }

    pauseCtx->cursorVtx = GRAPH_ALLOC(play->state.gfxCtx, (PAUSE_QUAD_CURSOR_MAX * 4) * sizeof(Vtx));

    for (i = 0; i < (PAUSE_QUAD_CURSOR_MAX * 4); i++) {
        pauseCtx->cursorVtx[i].v.ob[0] = pauseCtx->cursorVtx[i].v.ob[1] = pauseCtx->cursorVtx[i].v.ob[2] = 0;

        pauseCtx->cursorVtx[i].v.flag = 0;

        pauseCtx->cursorVtx[i].v.tc[0] = pauseCtx->cursorVtx[i].v.tc[1] = 0;

        pauseCtx->cursorVtx[i].v.cn[0] = pauseCtx->cursorVtx[i].v.cn[1] = pauseCtx->cursorVtx[i].v.cn[2] =
            pauseCtx->cursorVtx[i].v.cn[3] = 255;
    }

    // PAUSE_QUAD_CURSOR_0
    pauseCtx->cursorVtx[1].v.tc[0] = pauseCtx->cursorVtx[2].v.tc[1] = pauseCtx->cursorVtx[3].v.tc[0] =
        pauseCtx->cursorVtx[3].v.tc[1]
        // PAUSE_QUAD_CURSOR_1
        = pauseCtx->cursorVtx[5].v.tc[0] = pauseCtx->cursorVtx[6].v.tc[1] = pauseCtx->cursorVtx[7].v.tc[0] =
            pauseCtx->cursorVtx[7].v.tc[1]
        // PAUSE_QUAD_CURSOR_2
        = pauseCtx->cursorVtx[9].v.tc[0] = pauseCtx->cursorVtx[10].v.tc[1] = pauseCtx->cursorVtx[11].v.tc[0] =
            pauseCtx->cursorVtx[11].v.tc[1]
        // PAUSE_QUAD_CURSOR_3
        = pauseCtx->cursorVtx[13].v.tc[0] = pauseCtx->cursorVtx[14].v.tc[1] = pauseCtx->cursorVtx[15].v.tc[0] =
            pauseCtx->cursorVtx[15].v.tc[1] = 16 * (1 << 5);

    // PAUSE_QUAD_CURSOR_4
    pauseCtx->cursorVtx[17].v.tc[0] = pauseCtx->cursorVtx[18].v.tc[1] = pauseCtx->cursorVtx[19].v.tc[0] =
        pauseCtx->cursorVtx[19].v.tc[1] = 32 * (1 << 5);

    pauseCtx->infoPanelVtx = GRAPH_ALLOC(gfxCtx, 28 * sizeof(Vtx));

    if ((pauseCtx->state == PAUSE_STATE_SAVEPROMPT) || IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
        pauseCtx->promptPageVtx = GRAPH_ALLOC(gfxCtx, ((PAGE_BG_QUADS + VTX_PAGE_SAVE_QUADS) * 4) * sizeof(Vtx));
        KaleidoScope_SetPageVertices(play, pauseCtx->promptPageVtx, VTX_PAGE_PROMPT, VTX_PAGE_SAVE_QUADS);
    }
}

f32 sItemMaskCursorsX[] = {
    -62.0f, // Column 1
    -36.0f, // Column 2
    -10.0f, // Column 3
    16.0f,  // Column 4
    42.0f,  // Column 5
    68.0f,  // Column 6
};

f32 sItemMaskCursorsY[] = {
    31.0f,  // Row 1
    5.0f,   // Row 2
    -21.0f, // Row 3
    -47.0f, // Row 4
};

f32 sWorldMapCursorsX[REGION_MAX] = {
    -49.0f, // REGION_GREAT_BAY
    -35.0f, // REGION_ZORA_HALL
    -3.0f,  // REGION_ROMANI_RANCH
    4.0f,   // REGION_DEKU_PALACE
    12.0f,  // REGION_WOODFALL
    8.0f,   // REGION_CLOCK_TOWN
    7.0f,   // REGION_SNOWHEAD
    40.0f,  // REGION_IKANA_GRAVEYARD
    47.0f,  // REGION_IKANA_CANYON
    49.0f,  // REGION_GORON_VILLAGE
    55.0f,  // REGION_STONE_TOWER
};

f32 sWorldMapCursorsY[REGION_MAX] = {
    -15.0f, // REGION_GREAT_BAY
    -36.0f, // REGION_ZORA_HALL
    -23.0f, // REGION_ROMANI_RANCH
    -49.0f, // REGION_DEKU_PALACE
    -29.0f, // REGION_WOODFALL
    -8.0f,  // REGION_CLOCK_TOWN
    25.0f,  // REGION_SNOWHEAD
    2.0f,   // REGION_IKANA_GRAVEYARD
    -9.0f,  // REGION_IKANA_CANYON
    28.0f,  // REGION_GORON_VILLAGE
    7.0f,   // REGION_STONE_TOWER
};

f32 sOwlWarpWorldMapCursorsX[OWL_WARP_MAX - 1] = {
    -50.0f, // OWL_WARP_GREAT_BAY_COAST
    -38.0f, // OWL_WARP_ZORA_CAPE
    6.0f,   // OWL_WARP_SNOWHEAD
    11.0f,  // OWL_WARP_MOUNTAIN_VILLAGE
    8.0f,   // OWL_WARP_CLOCK_TOWN
    0.0f,   // OWL_WARP_MILK_ROAD
    12.0f,  // OWL_WARP_WOODFALL
    31.0f,  // OWL_WARP_SOUTHERN_SWAMP
    48.0f,  // OWL_WARP_IKANA_CANYON
    56.0f,  // OWL_WARP_STONE_TOWER
};

f32 sOwlWarpWorldMapCursorsY[OWL_WARP_MAX - 1] = {
    -14.0f, // OWL_WARP_GREAT_BAY_COAST
    -39.0f, // OWL_WARP_ZORA_CAPE
    23.0f,  // OWL_WARP_SNOWHEAD
    11.0f,  // OWL_WARP_MOUNTAIN_VILLAGE
    -8.0f,  // OWL_WARP_CLOCK_TOWN
    -15.0f, // OWL_WARP_MILK_ROAD
    -31.0f, // OWL_WARP_WOODFALL
    -30.0f, // OWL_WARP_SOUTHERN_SWAMP
    -10.0f, // OWL_WARP_IKANA_CANYON
    11.0f,  // OWL_WARP_STONE_TOWER
};

f32 sDungeonMapCursorsX[] = {
    -72.0f, // DUNGEON_BOSS_KEY
    -47.0f, // DUNGEON_COMPASS
    -26.0f, // DUNGEON_MAP
    -50.0f, // DUNGEON_STRAY_FAIRIES
    -48.0f, // DUNGEON_FLOOR_INDEX_4 (top floor)
    -48.0f, // DUNGEON_FLOOR_INDEX_3
    -48.0f, // DUNGEON_FLOOR_INDEX_2
    -48.0f, // DUNGEON_FLOOR_INDEX_1
    -48.0f, // DUNGEON_FLOOR_INDEX_0 (bottom floor)
};

f32 sDungeonMapCursorsY[] = {
    -47.0f, // DUNGEON_BOSS_KEY
    -47.0f, // DUNGEON_COMPASS
    -47.0f, // DUNGEON_MAP
    -30.0f, // DUNGEON_STRAY_FAIRIES
    33.0f,  // DUNGEON_FLOOR_INDEX_4 (top floor)
    22.0f,  // DUNGEON_FLOOR_INDEX_3
    11.0f,  // DUNGEON_FLOOR_INDEX_2
    -2.0f,  // DUNGEON_FLOOR_INDEX_1
    -13.0f, // DUNGEON_FLOOR_INDEX_0 (bottom floor)
};

f32 sQuestStatusCursorsX[] = {
    52.0f,  // QUEST_REMAINS_ODOLWA
    80.0f,  // QUEST_REMAINS_GOHT
    24.0f,  // QUEST_REMAINS_GYORG
    52.0f,  // QUEST_REMAINS_TWINMOLD
    81.0f,  // QUEST_SHIELD
    24.0f,  // QUEST_SWORD
    -78.0f, // QUEST_SONG_SONATA
    -61.0f, // QUEST_SONG_LULLABY
    -44.0f, // QUEST_SONG_BOSSA_NOVA
    -24.0f, // QUEST_SONG_ELEGY
    -6.0f,  // QUEST_SONG_OATH
    -3.0f,  // QUEST_SONG_SARIA
    -78.0f, // QUEST_SONG_TIME
    -61.0f, // QUEST_SONG_HEALING
    -44.0f, // QUEST_SONG_EPONA
    -24.0f, // QUEST_SONG_SOARING
    -6.0f,  // QUEST_SONG_STORMS
    -3.0f,  // QUEST_SONG_SUN
    -68.0f, // QUEST_BOMBERS_NOTEBOOK
    22.0f,  // QUEST_QUIVER
    83.0f,  // QUEST_BOMB_BAG
    -67.0f, // QUEST_SKULL_TOKEN
    -21.0f, // QUEST_HEART_PIECE
};

f32 sQuestStatusCursorsY[] = {
    33.0f,  // QUEST_REMAINS_ODOLWA
    18.0f,  // QUEST_REMAINS_GOHT
    18.0f,  // QUEST_REMAINS_GYORG
    0.0f,   // QUEST_REMAINS_TWINMOLD
    -23.0f, // QUEST_SHIELD
    -23.0f, // QUEST_SWORD
    -29.0f, // QUEST_SONG_SONATA
    -29.0f, // QUEST_SONG_LULLABY
    -29.0f, // QUEST_SONG_BOSSA_NOVA
    -29.0f, // QUEST_SONG_ELEGY
    -29.0f, // QUEST_SONG_OATH
    -29.0f, // QUEST_SONG_SARIA
    -11.0f, // QUEST_SONG_TIME
    -11.0f, // QUEST_SONG_HEALING
    -11.0f, // QUEST_SONG_EPONA
    -11.0f, // QUEST_SONG_SOARING
    -11.0f, // QUEST_SONG_STORMS
    -11.0f, // QUEST_SONG_SUN
    27.0f,  // QUEST_BOMBERS_NOTEBOOK
    -53.0f, // QUEST_QUIVER
    -53.0f, // QUEST_BOMB_BAG
    14.0f,  // QUEST_SKULL_TOKEN
    26.0f,  // QUEST_HEART_PIECE
};

void KaleidoScope_UpdateCursorSize(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 index;

    pauseCtx->cursorVtx[0].v.ob[0] = pauseCtx->cursorVtx[2].v.ob[0] = -8;
    pauseCtx->cursorVtx[1].v.ob[0] = pauseCtx->cursorVtx[3].v.ob[0] = pauseCtx->cursorVtx[0].v.ob[0] + 16;

    pauseCtx->cursorVtx[0].v.ob[1] = pauseCtx->cursorVtx[1].v.ob[1] = 8;
    pauseCtx->cursorVtx[2].v.ob[1] = pauseCtx->cursorVtx[3].v.ob[1] = pauseCtx->cursorVtx[0].v.ob[1] - 16;

    if (pauseCtx->cursorSpecialPos == 0) {
        switch (pauseCtx->pageIndex) {
            case PAUSE_ITEM:
                pauseCtx->cursorX = sItemMaskCursorsX[pauseCtx->cursorXIndex[PAUSE_ITEM]];
                pauseCtx->cursorY = sItemMaskCursorsY[pauseCtx->cursorYIndex[PAUSE_ITEM]];
                pauseCtx->cursorWidth = 15.0f;
                pauseCtx->cursorHeight = 15.0f;
                break;

            case PAUSE_MAP:
                if (!sInDungeonScene) {
                    if (IS_PAUSE_STATE_OWL_WARP(pauseCtx) || PauseOwlWarp_IsOwlWarpEnabled()) {
                        pauseCtx->cursorX = sOwlWarpWorldMapCursorsX[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]];
                        pauseCtx->cursorY = sOwlWarpWorldMapCursorsY[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]];
                    } else {
                        pauseCtx->cursorX = sWorldMapCursorsX[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]];
                        pauseCtx->cursorY = sWorldMapCursorsY[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]];
                    }
                    if (!IS_PAUSE_STATE_OWL_WARP(pauseCtx) && !PauseOwlWarp_IsOwlWarpEnabled()) {
                        pauseCtx->cursorHeight = 10.0f;
                        pauseCtx->cursorWidth = 10.0f;
                    } else {
                        pauseCtx->cursorHeight = 15.0f;
                        pauseCtx->cursorWidth = 15.0f;
                    }
                    // Flip the position of the cursor with an additional offset to align with flipped map points
                    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
                        pauseCtx->cursorX = (pauseCtx->cursorX * -1.0) + 5.0f;
                    }
                } else {
                    pauseCtx->cursorX = sDungeonMapCursorsX[pauseCtx->cursorPoint[PAUSE_MAP]];
                    pauseCtx->cursorY = sDungeonMapCursorsY[pauseCtx->cursorPoint[PAUSE_MAP]];
                    if (pauseCtx->cursorSlot[pauseCtx->pageIndex] >= DUNGEON_FLOOR_INDEX_4) {
                        // Floors
                        pauseCtx->cursorWidth = 18.0f;
                        pauseCtx->cursorHeight = 12.0f;
                    } else if (pauseCtx->cursorSlot[pauseCtx->pageIndex] == DUNGEON_STRAY_FAIRIES) {
                        // Stray Fairies
                        pauseCtx->cursorHeight = 12.0f;
                        pauseCtx->cursorWidth = 40.0f;
                    } else {
                        // Map, Compass, Boss Key
                        pauseCtx->cursorHeight = 15.0f;
                        pauseCtx->cursorWidth = 15.0f;
                    }
                }
                break;

            case PAUSE_QUEST:
                if (!R_PAUSE_DBG_QUEST_CURSOR_ON) {
                    pauseCtx->cursorX = sQuestStatusCursorsX[pauseCtx->cursorPoint[PAUSE_QUEST]];
                    pauseCtx->cursorY = sQuestStatusCursorsY[pauseCtx->cursorPoint[PAUSE_QUEST]];
                } else {
                    pauseCtx->cursorX = R_PAUSE_DBG_QUEST_CURSOR_X;
                    pauseCtx->cursorY = R_PAUSE_DBG_QUEST_CURSOR_Y;
                }
                pauseCtx->cursorHeight = 17.0f;
                pauseCtx->cursorWidth = 17.0f;
                if (pauseCtx->cursorSlot[pauseCtx->pageIndex] == QUEST_17) {
                    pauseCtx->cursorHeight = 25.0f;
                    pauseCtx->cursorWidth = 25.0f;
                } else if (pauseCtx->cursorSlot[pauseCtx->pageIndex] <= QUEST_SWORD) {
                    pauseCtx->cursorHeight = 18.0f;
                    pauseCtx->cursorWidth = 18.0f;
                } else if ((pauseCtx->cursorSlot[pauseCtx->pageIndex] >= QUEST_SONG_SONATA) &&
                           (pauseCtx->cursorSlot[pauseCtx->pageIndex] <= QUEST_SONG_SUN)) {
                    pauseCtx->cursorHeight = 12.0f;
                    pauseCtx->cursorWidth = 12.0f;
                }
                break;

            case PAUSE_MASK:
                pauseCtx->cursorX = sItemMaskCursorsX[pauseCtx->cursorXIndex[PAUSE_MASK]];
                pauseCtx->cursorY = sItemMaskCursorsY[pauseCtx->cursorYIndex[PAUSE_MASK]];
                pauseCtx->cursorWidth = 15.0f;
                pauseCtx->cursorHeight = 15.0f;
                break;

            default:
                break;
        }

        pauseCtx->cursorSpinPhase += 0x300;
    } else {
        if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
            pauseCtx->cursorX = -93.0f;
        } else { // PAUSE_CURSOR_PAGE_RIGHT
            pauseCtx->cursorX = 101.0f;
        }
        pauseCtx->cursorSpinPhase += 0x300;
        pauseCtx->cursorHeight = 16.0f;
        pauseCtx->cursorWidth = 16.0f;
        pauseCtx->cursorY = -4.0f;
    }

    if (pauseCtx->cursorWidth < pauseCtx->cursorShrinkRate) {
        pauseCtx->cursorWidth = 0;
    } else {
        pauseCtx->cursorWidth -= pauseCtx->cursorShrinkRate;
    }

    if (pauseCtx->cursorHeight < pauseCtx->cursorShrinkRate) {
        pauseCtx->cursorHeight = 0.0f;
    } else {
        pauseCtx->cursorHeight -= pauseCtx->cursorShrinkRate;
    }

    if (pauseCtx->cursorShrinkRate != 0.0f) {
        pauseCtx->cursorShrinkRate -= 1.0f;
    }

    sCursorCirclesX[0] = Math_SinS(pauseCtx->cursorSpinPhase) * pauseCtx->cursorWidth;
    sCursorCirclesX[1] = Math_SinS(pauseCtx->cursorSpinPhase + 0x4000) * pauseCtx->cursorWidth;
    sCursorCirclesX[2] = Math_SinS(pauseCtx->cursorSpinPhase + 0x8000) * pauseCtx->cursorWidth;
    sCursorCirclesX[3] = Math_SinS(pauseCtx->cursorSpinPhase + 0xC000) * pauseCtx->cursorWidth;
    sCursorCirclesY[0] = Math_CosS(pauseCtx->cursorSpinPhase) * pauseCtx->cursorHeight;
    sCursorCirclesY[1] = Math_CosS(pauseCtx->cursorSpinPhase + 0x4000) * pauseCtx->cursorHeight;
    sCursorCirclesY[2] = Math_CosS(pauseCtx->cursorSpinPhase + 0x8000) * pauseCtx->cursorHeight;
    sCursorCirclesY[3] = Math_CosS(pauseCtx->cursorSpinPhase + 0xC000) * pauseCtx->cursorHeight;
}

void KaleidoScope_DrawCursor(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    Color_RGBA8 cursorGlow; // 2S2H [Cosmetic]
    s16 i;

    // #region 2S2H [Port] Track cursor position so we can skip interpolation for one frame whenever it moves
    static f32 prevX = 0;
    static f32 prevY = 0;
    static u8 cursorInterpState = 0;

    if (prevX != pauseCtx->cursorX || prevY != pauseCtx->cursorY) {
        cursorInterpState ^= 1; // Flip state
    }
    prevX = pauseCtx->cursorX;
    prevY = pauseCtx->cursorY;
    // #endregion

    OPEN_DISPS(play->state.gfxCtx);

    if ((pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) ||
        (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) ||
        ((pauseCtx->pageIndex == PAUSE_QUEST) && ((pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PLAYBACK) ||
                                                  (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) ||
                                                  (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)))) {
        FrameInterpolation_RecordOpenChild("Pause cursor", cursorInterpState ? 1 : 0);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, sCursorPrimR, sCursorPrimG, sCursorPrimB, 255,
                                COSMETIC_ID("Menus.PauseCursor"));
        cursorGlow = CosmeticEditor_GetPauseCursorGlow(sCursorEnvR, sCursorEnvG, sCursorEnvB,
                                                       sCursorEnvColorTarget[pauseCtx->cursorColorSet],
                                                       sCursorEnvColorTarget[pauseCtx->cursorColorSet + 1]);
        gDPSetEnvColor(POLY_OPA_DISP++, cursorGlow.r, cursorGlow.g, cursorGlow.b, 255);

        Matrix_Translate(pauseCtx->cursorX, pauseCtx->cursorY, -50.0f, MTXMODE_NEW);
        Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

        for (i = 0; i < 4; i++) {
            Matrix_Push();
            Matrix_Translate(sCursorCirclesX[i], sCursorCirclesY[i], -50.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gDPPipeSync(POLY_OPA_DISP++);
            gDPLoadTextureBlock(POLY_OPA_DISP++, gPauseMenuCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->cursorVtx[0], 4, 0);
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
            Matrix_Pop();
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void KaleidoScope_DrawGameOver(PlayState* play) {
    static s16 sGameOverUlt = 0;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL39_Opa(gfxCtx);

    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_PASS, G_RM_XLU_SURF2);
    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, 0, 0, 0, TEXEL0, PRIMITIVE, ENVIRONMENT,
                      COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 80, sGameOverPrimR, sGameOverPrimG, sGameOverPrimB, sGameOverPrimAlpha);
    gDPSetEnvColor(POLY_OPA_DISP++, sGameOverEnvR, sGameOverEnvG, sGameOverEnvB, 255);

    sGameOverUlt -= 2;

    gDPLoadTextureBlock(POLY_OPA_DISP++, gGameOverP1Tex, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gDPLoadMultiBlock(POLY_OPA_DISP++, gGameOverMaskTex, 256, 1, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 32, 0,
                      G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, 5, G_TX_NOLOD, G_TX_NOLOD);

    gDPSetTileSize(POLY_OPA_DISP++, 1, 0, sGameOverUlt & 0x7F, 252, (sGameOverUlt & 0x7F) + 124);

    gSPTextureRectangle(POLY_OPA_DISP++, 0x100, sGameOverRectPosY << 2, 0x200, (sGameOverRectPosY + 32) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    gDPLoadTextureBlock(POLY_OPA_DISP++, gGameOverP2Tex, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 32, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSPTextureRectangle(POLY_OPA_DISP++, 0x200, sGameOverRectPosY << 2, 0x300, (sGameOverRectPosY + 32) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    gDPLoadTextureBlock(POLY_OPA_DISP++, gGameOverP3Tex, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 32, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSPTextureRectangle(POLY_OPA_DISP++, 0x300, sGameOverRectPosY << 2, 0x400, (sGameOverRectPosY + 32) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    CLOSE_DISPS(gfxCtx);
}

void KaleidoScope_Draw(PlayState* play) {
    s32 pad;
    PauseContext* pauseCtx = &play->pauseCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x02, interfaceCtx->parameterSegment);
    gSPSegment(POLY_OPA_DISP++, 0x08, pauseCtx->iconItemSegment);
    gSPSegment(POLY_OPA_DISP++, 0x09, pauseCtx->iconItem24Segment);
    gSPSegment(POLY_OPA_DISP++, 0x0A, pauseCtx->nameSegment);
    gSPSegment(POLY_OPA_DISP++, 0x0C, pauseCtx->iconItemAltSegment);
    gSPSegment(POLY_OPA_DISP++, 0x0D, pauseCtx->iconItemLangSegment);
    gSPSegment(POLY_OPA_DISP++, 0x0B, pauseCtx->iconItemVtxSegment);

    if (pauseCtx->debugEditor == DEBUG_EDITOR_NONE) {
        KaleidoScope_SetView(pauseCtx, pauseCtx->eye.x, pauseCtx->eye.y, pauseCtx->eye.z);
        Gfx_SetupDL42_Opa(play->state.gfxCtx);

        if (!IS_PAUSE_STATE_OWL_WARP(pauseCtx)) {
            // Draw Default or Game Over Menus
            KaleidoScope_SetVertices(play, play->state.gfxCtx);
            KaleidoScope_DrawPages(play, play->state.gfxCtx);

            Gfx_SetupDL42_Opa(play->state.gfxCtx);
            gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

            KaleidoScope_SetView(pauseCtx, 0.0f, 0.0f, 64.0f);

            if (!IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
                KaleidoScope_DrawInfoPanel(play);
            }

            KaleidoScope_UpdateCursorSize(play);

            if (pauseCtx->state == PAUSE_STATE_MAIN) {
                KaleidoScope_DrawCursor(play);
            }

            if ((pauseCtx->state >= PAUSE_STATE_GAMEOVER_3) && (pauseCtx->state <= PAUSE_STATE_GAMEOVER_10) &&
                (play->gameOverCtx.state != GAMEOVER_INACTIVE)) {
                KaleidoScope_DrawGameOver(play);
            }
        } else {
            // Draw Owl Warp Menu
            KaleidoScope_SetVertices(play, play->state.gfxCtx);
            KaleidoScope_DrawPages(play, play->state.gfxCtx);
            KaleidoScope_DrawOwlWarpMapPage(play);

            Gfx_SetupDL42_Opa(play->state.gfxCtx);
            gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

            KaleidoScope_SetView(pauseCtx, 0.0f, 0.0f, 64.0f);
            KaleidoScope_DrawOwlWarpInfoPanel(play);
            KaleidoScope_UpdateCursorSize(play);

            if (pauseCtx->state == PAUSE_STATE_OWL_WARP_SELECT) {
                KaleidoScope_DrawCursor(play);
            }
        }
    }

    if ((pauseCtx->debugEditor == DEBUG_EDITOR_INVENTORY_INIT) || (pauseCtx->debugEditor == DEBUG_EDITOR_INVENTORY)) {
        KaleidoScope_DrawInventoryEditor(play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void KaleidoScope_GrayOutTextureRGBA32(u32* texture, u16 pixelCount) {
    u32 rgb;
    u32 gray;
    u16 i;
    s32 j;

    //! FAKE
    if (texture == NULL) {}

    for (i = 0; i < pixelCount; i++) {
        //! FAKE
        j = i;
        if ((texture[j] & 0xFFFFFF00) != 0) {
            rgb = texture[j] >> 8;
            gray = ((((rgb & 0xFF0000) >> 16) + ((rgb & 0xFF00) >> 7) + (rgb & 0xFF)) / 7) & 0xFF;

            rgb = gray;
            rgb <<= 8;
            rgb |= gray;
            rgb <<= 8;
            rgb |= gray;

            texture[j] = (rgb << 8) | (texture[j] & 0xFF);
        }
    }
}

void KaleidoScope_UpdateOpening(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;

    pauseCtx->eye.x += sPageSwitchEyeDx[pauseCtx->nextPageMode] * 2.0f;
    pauseCtx->eye.z += sPageSwitchEyeDz[pauseCtx->nextPageMode] * 2.0f;
    pauseCtx->switchPageTimer += 8;

    if (pauseCtx->switchPageTimer == 64) {
        // Finished opening
        Interface_UpdateButtonsAlt(play, 1);

        if (pauseCtx->cursorSpecialPos == 0) {
            gSaveContext.buttonStatus[EQUIP_SLOT_B] = gPageSwitchNextButtonStatus[pauseCtx->pageIndex][0];
            gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = gPageSwitchNextButtonStatus[pauseCtx->pageIndex][1];
            gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = gPageSwitchNextButtonStatus[pauseCtx->pageIndex][1];
            gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = gPageSwitchNextButtonStatus[pauseCtx->pageIndex][1];
            // #region 2S2H [Dpad]
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] =
                gPageSwitchNextButtonStatus[pauseCtx->pageIndex][1];
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT] =
                gPageSwitchNextButtonStatus[pauseCtx->pageIndex][1];
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN] =
                gPageSwitchNextButtonStatus[pauseCtx->pageIndex][1];
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP] =
                gPageSwitchNextButtonStatus[pauseCtx->pageIndex][1];
            // #endregion
            gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
        } else {
            gSaveContext.buttonStatus[EQUIP_SLOT_B] = BTN_ENABLED;
            gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = BTN_DISABLED;
            gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = BTN_DISABLED;
            gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = BTN_DISABLED;
            // #region 2S2H [Dpad]
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] = BTN_DISABLED;
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT] = BTN_DISABLED;
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN] = BTN_DISABLED;
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP] = BTN_DISABLED;
            // #endregion
            gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_DISABLED;
        }

        pauseCtx->pageIndex = sPageSwitchNextPageIndex[pauseCtx->nextPageMode];
        pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
        pauseCtx->state++; // PAUSE_STATE_MAIN
        pauseCtx->alpha = 255;
        Interface_SetBButtonInterfaceDoAction(play, DO_ACTION_RETURN);
    } else if (pauseCtx->switchPageTimer == 64) {
        pauseCtx->pageIndex = sPageSwitchNextPageIndex[pauseCtx->nextPageMode];
        pauseCtx->nextPageMode = (pauseCtx->pageIndex * 2) + 1;
    }
}

u16 sCursorPointsToOcarinaModes[] = {
    OCARINA_MODE_WARP_TO_GREAT_BAY_COAST,  OCARINA_MODE_WARP_TO_ZORA_CAPE,        OCARINA_MODE_WARP_TO_SNOWHEAD,
    OCARINA_MODE_WARP_TO_MOUNTAIN_VILLAGE, OCARINA_MODE_WARP_TO_SOUTH_CLOCK_TOWN, OCARINA_MODE_WARP_TO_MILK_ROAD,
    OCARINA_MODE_WARP_TO_WOODFALL,         OCARINA_MODE_WARP_TO_SOUTHERN_SWAMP,   OCARINA_MODE_WARP_TO_IKANA_CANYON,
    OCARINA_MODE_WARP_TO_STONE_TOWER,
};

void KaleidoScope_Update(PlayState* play) {
    static s16 sUnpausedHudVisibility = HUD_VISIBILITY_IDLE;
    static s16 sNextMainState = PAUSE_MAIN_STATE_IDLE;
    static s16 sDelayTimer = 10;
    static s16 sGameOverColorTimer = 0;
    static s16 sStickXRepeatTimer = 0;
    static s16 sStickYRepeatTimer = 0;
    static s16 sStickXRepeatState = 0;
    static s16 sStickYRepeatState = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    MessageContext* msgCtx = &play->msgCtx;
    GameOverContext* gameOverCtx = &play->gameOverCtx;
    SramContext* sramCtx = &play->sramCtx;
    Input* input = CONTROLLER1(&play->state);
    size_t size0;
    size_t size1;
    size_t size2;
    u16 itemId;
    u16 worldMapCursorPoint;
    void* iconItemLangSegment;
    s16 stepR;
    s16 stepG;
    s16 stepB;
    s16 stepA;
    s16 stickAdjX = input->rel.stick_x;

    MapDisp_UpdateDungeonMap(play);

    pauseCtx->stickAdjX = input->rel.stick_x;
    pauseCtx->stickAdjY = input->rel.stick_y;

    switch (pauseCtx->state) {
        case PAUSE_STATE_OPENING_2:
            sUnpausedHudVisibility = gSaveContext.hudVisibility;
            sPauseMenuVerticalOffset = -6240.0f;

            sUnpausedButtonStatus[EQUIP_SLOT_B] = gSaveContext.buttonStatus[EQUIP_SLOT_B];
            sUnpausedButtonStatus[EQUIP_SLOT_C_LEFT] = gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT];
            sUnpausedButtonStatus[EQUIP_SLOT_C_DOWN] = gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN];
            sUnpausedButtonStatus[EQUIP_SLOT_C_RIGHT] = gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT];
            sUnpausedButtonStatus[EQUIP_SLOT_A] = gSaveContext.buttonStatus[EQUIP_SLOT_A];
            // #region 2S2H [Dpad]
            sUnpausedDpadStatus[EQUIP_SLOT_D_RIGHT] = gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT];
            sUnpausedDpadStatus[EQUIP_SLOT_D_LEFT] = gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT];
            sUnpausedDpadStatus[EQUIP_SLOT_D_DOWN] = gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN];
            sUnpausedDpadStatus[EQUIP_SLOT_D_UP] = gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP];
            // #endregion

            pauseCtx->cursorXIndex[PAUSE_MAP] = 0;
            pauseCtx->cursorSlot[PAUSE_MAP] = R_PLAYER_FLOOR_REVERSE_INDEX + DUNGEON_FLOOR_INDEX_4;
            pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->cursorMapDungeonItem =
                R_PLAYER_FLOOR_REVERSE_INDEX + DUNGEON_FLOOR_INDEX_4;

            sPauseCursorLeftX = -175;
            sPauseCursorRightX = 155;
            pauseCtx->roll = -314.0f;

            pauseCtx->iconItemSegment = (void*)ALIGN16((uintptr_t)play->objectCtx.spaceStart);
            size0 = SEGMENT_ROM_SIZE(icon_item_static_syms);
            CmpDma_LoadAllFiles(SEGMENT_ROM_START(icon_item_static_yar), pauseCtx->iconItemSegment, size0);

            gSegments[0x08] = OS_K0_TO_PHYSICAL(pauseCtx->iconItemSegment);

            for (itemId = 0; itemId <= ITEM_BOW_FIRE; itemId++) {
                if (!gPlayerFormItemRestrictions[GET_PLAYER_FORM][(s32)itemId]) {
                    // 2S2H [Port] This is now handled by gSPGrayscale in KaleidoScope_DrawItemSelect
                    // KaleidoScope_GrayOutTextureRGBA32(Lib_SegmentedToVirtual(gItemIcons[(s32)itemId]),
                    //                                                     ICON_ITEM_TEX_WIDTH * ICON_ITEM_TEX_HEIGHT);
                }
            }

            pauseCtx->iconItem24Segment = (void*)ALIGN16((uintptr_t)pauseCtx->iconItemSegment + size0);
            size1 = SEGMENT_ROM_SIZE(icon_item_24_static_syms);
            CmpDma_LoadAllFiles(SEGMENT_ROM_START(icon_item_24_static_yar), pauseCtx->iconItem24Segment, size1);

            pauseCtx->iconItemAltSegment = (void*)ALIGN16((uintptr_t)pauseCtx->iconItem24Segment + size1);
            if (Map_CurRoomHasMapI(play)) {
                size_t size = SEGMENT_ROM_SIZE(icon_item_dungeon_static);

                DmaMgr_SendRequest0(pauseCtx->iconItemAltSegment, SEGMENT_ROM_START(icon_item_dungeon_static), size);
                iconItemLangSegment =
                    MapDisp_AllocDungeonMap(play, (void*)ALIGN16((uintptr_t)pauseCtx->iconItemAltSegment + size));
                sInDungeonScene = true;
            } else {
                size_t size;

                sInDungeonScene = false;
                size = SEGMENT_ROM_SIZE(icon_item_field_static);
                DmaMgr_SendRequest0(pauseCtx->iconItemAltSegment, SEGMENT_ROM_START(icon_item_field_static), size);
                iconItemLangSegment = (void*)ALIGN16((uintptr_t)pauseCtx->iconItemAltSegment + size);
            }

            pauseCtx->iconItemLangSegment = iconItemLangSegment;
            size2 = SEGMENT_ROM_SIZE(icon_item_jpn_static);
            DmaMgr_SendRequest0(pauseCtx->iconItemLangSegment, SEGMENT_ROM_START(icon_item_jpn_static), size2);

            pauseCtx->nameSegment = (void*)ALIGN16((uintptr_t)pauseCtx->iconItemLangSegment + size2);
            Interface_SetAButtonDoAction(play, DO_ACTION_INFO);

            //! note: `worldMapArea` never set to a value other than 0
            if (((void)0, gSaveContext.worldMapArea) < 22) {
                Kaleido_LoadMapNameStaticLarge(&pauseCtx->nameSegment /* + 0x400 */,
                                               ((void)0, gSaveContext.worldMapArea));
            }

            pauseCtx->iconItemVtxSegment = (void*)ALIGN16((uintptr_t)pauseCtx->nameSegment + 0xA00);
            DmaMgr_SendRequest0(pauseCtx->iconItemVtxSegment, SEGMENT_ROM_START(icon_item_vtx_static),
                                SEGMENT_ROM_SIZE(icon_item_vtx_static));

            pauseCtx->unk_2B6 = 255;
            pauseCtx->unk_2B7 = 255;
            pauseCtx->unk_2B8 = 255;
            pauseCtx->state = PAUSE_STATE_OPENING_3;
            break;

        case PAUSE_STATE_OPENING_3:
            pauseCtx->infoPanelOffsetY += 10;
            pauseCtx->itemPageRoll = pauseCtx->mapPageRoll = pauseCtx->questPageRoll = pauseCtx->maskPageRoll -= 40.0f;

            interfaceCtx->startAlpha += 63;
            sPauseCursorLeftX += TRUNCF_BINANG(sPauseCursorLeftMoveOffsetX / 4);
            sPauseCursorRightX += TRUNCF_BINANG(sPauseCursorRightMoveOffsetX / 4);
            pauseCtx->alpha += 31;
            if (pauseCtx->itemPageRoll == 0) {
                interfaceCtx->startAlpha = 255;
                sPauseMenuVerticalOffset = 0.0f;
                pauseCtx->state = PAUSE_STATE_OPENING_4;
            }

            KaleidoScope_UpdateOpening(play);
            break;

        case PAUSE_STATE_OPENING_4:
            pauseCtx->alpha += 31;
            KaleidoScope_UpdateOpening(play);

            if (pauseCtx->state == PAUSE_STATE_MAIN) {
                KaleidoScope_UpdateNamePanel(play);
            }
            break;

        case PAUSE_STATE_MAIN:
            switch (pauseCtx->mainState) {
                case PAUSE_MAIN_STATE_IDLE:
                    if (!pauseCtx->itemDescriptionOn &&
                        (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_B))) {
                        Interface_SetAButtonDoAction(play, DO_ACTION_NONE);
                        if (GameInteractor_Should(VB_SAVE_ON_B_BUTTON_IN_PAUSE_MENU, false)) {
                            pauseCtx->state = PAUSE_STATE_SAVEPROMPT;
                            Audio_PlaySfx_MessageDecide();
                            break;
                        }
                        pauseCtx->state = PAUSE_STATE_UNPAUSE_SETUP;
                        sPauseMenuVerticalOffset = -6240.0f;
                        Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_CLOSE);
                    }
                    break;

                case PAUSE_MAIN_STATE_SWITCHING_PAGE:
                    KaleidoScope_UpdateSwitchPage(play, input);
                    break;

                case PAUSE_MAIN_STATE_SONG_PLAYBACK:
                    // Song is in playback
                    pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
                    if (pauseCtx->ocarinaStaff->state == 0) {
                        // Song playback is finished, give song prompt to player
                        pauseCtx->mainState = PAUSE_MAIN_STATE_SONG_PROMPT_INIT;
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                    }
                    break;

                case PAUSE_MAIN_STATE_EQUIP_ITEM:
                    KaleidoScope_UpdateItemEquip(play);
                    break;

                case PAUSE_MAIN_STATE_SONG_PROMPT_INIT:
                    break;

                case PAUSE_MAIN_STATE_SONG_PROMPT:
                    pauseCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();

                    if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        // Abort having the player play the song and close the pause menu
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                        Interface_SetAButtonDoAction(play, DO_ACTION_NONE);
                        if (GameInteractor_Should(VB_SAVE_ON_B_BUTTON_IN_PAUSE_MENU, false)) {
                            pauseCtx->state = PAUSE_STATE_SAVEPROMPT;
                            Audio_PlaySfx_MessageDecide();
                            break;
                        }
                        pauseCtx->state = PAUSE_STATE_UNPAUSE_SETUP;
                        sPauseMenuVerticalOffset = -6240.0f;
                        Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_CLOSE);
                        pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    } else if (pauseCtx->ocarinaStaff->state == pauseCtx->ocarinaSongIndex) {
                        // The player successfully played the song
                        Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                        sNextMainState = PAUSE_MAIN_STATE_IDLE;
                        sDelayTimer = 30;
                        pauseCtx->mainState = PAUSE_MAIN_STATE_SONG_PROMPT_DONE;
                    } else if (pauseCtx->ocarinaStaff->state == 0xFF) {
                        // The player failed to play the song
                        Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                        sNextMainState = PAUSE_MAIN_STATE_SONG_PROMPT_INIT;
                        sDelayTimer = 20;
                        pauseCtx->mainState = PAUSE_MAIN_STATE_SONG_PROMPT_DONE;
                    }
                    break;

                case PAUSE_MAIN_STATE_SONG_PROMPT_DONE:
                    sDelayTimer--;
                    if (sDelayTimer == 0) {
                        pauseCtx->mainState = sNextMainState;
                        if (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) {
                            AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                        }
                    }
                    break;

                case PAUSE_MAIN_STATE_SONG_PROMPT_UNUSED:
                    break;

                case PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG:
                    if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                        Interface_SetAButtonDoAction(play, DO_ACTION_NONE);
                        if (GameInteractor_Should(VB_SAVE_ON_B_BUTTON_IN_PAUSE_MENU, false)) {
                            pauseCtx->state = PAUSE_STATE_SAVEPROMPT;
                            Audio_PlaySfx_MessageDecide();
                            break;
                        }
                        pauseCtx->state = PAUSE_STATE_UNPAUSE_SETUP;
                        sPauseMenuVerticalOffset = -6240.0f;
                        Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_CLOSE);
                        pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    }
                    break;

                case PAUSE_MAIN_STATE_SONG_PLAYBACK_INIT:
                    break;

                case PAUSE_MAIN_STATE_EQUIP_MASK:
                    KaleidoScope_UpdateMaskEquip(play);
                    break;

                case PAUSE_MAIN_STATE_BOMBERS_NOTEBOOK_OPEN:
                    if (!play->pauseCtx.bombersNotebookOpen) {
                        pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    }
                    break;

                default:
                    pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    break;
            }
            break;

        case PAUSE_STATE_SAVEPROMPT:
            switch (pauseCtx->savePromptState) {
                case PAUSE_SAVEPROMPT_STATE_APPEARING:
                    pauseCtx->roll -= 78.5f;
                    sPauseCursorLeftX -= TRUNCF_BINANG(sPauseCursorLeftMoveOffsetX / 4);
                    sPauseCursorRightX -= TRUNCF_BINANG(sPauseCursorRightMoveOffsetX / 4);
                    if (pauseCtx->roll <= -628.0f) {
                        pauseCtx->roll = -628.0f;
                        pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_1;
                    }
                    break;

                case PAUSE_SAVEPROMPT_STATE_1:
                    if (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        if (pauseCtx->promptChoice != PAUSE_PROMPT_YES) {
                            Interface_SetAButtonDoAction(play, DO_ACTION_NONE);
                            pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_RETURN_TO_MENU;
                        } else {
                            Audio_PlaySfx(NA_SE_SY_PIECE_OF_HEART);
                            // 2S2H [Enhancement] Persist this in case the user is 0th daying
                            bool currentOwlSaveState = gSaveContext.save.isOwlSave;
                            if (CVarGetInteger("gEnhancements.Saving.PauseSave", 0)) {
                                gSaveContext.save.isOwlSave = true;
                                SavingEnhancements_PersistSaveEntranceInfo();
                                SavingEnhancements_AdvancePlaytime();
                            }
                            Play_SaveCycleSceneFlags(play);
                            gSaveContext.save.saveInfo.playerData.savedSceneId = play->sceneId;
                            func_8014546C(sramCtx);
                            if (!gSaveContext.flashSaveAvailable ||
                                gSaveContext.fileNum ==
                                    255) { // 2S2H [Enhancement] Don't let them save if they are in debug save
                                pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_5;
                            } else {
                                if (CVarGetInteger("gEnhancements.Saving.PauseSave", 0)) {
                                    Sram_SetFlashPagesOwlSave(
                                        sramCtx,
                                        gFlashOwlSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                                        gFlashOwlSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER]);
                                    Sram_StartWriteToFlashOwlSave(sramCtx);
                                    gSaveContext.save.isOwlSave = currentOwlSaveState;
                                    SavingEnhancements_ClearSaveEntranceInfo();
                                } else {
                                    Sram_SetFlashPagesDefault(sramCtx, gFlashSaveStartPages[gSaveContext.fileNum],
                                                              gFlashSaveNumPages[gSaveContext.fileNum]);
                                    Sram_StartWriteToFlashDefault(sramCtx);
                                }
                                pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_4;
                            }
                            sDelayTimer = 90;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
                        Interface_SetAButtonDoAction(play, DO_ACTION_NONE);
                        pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_3;
                        sPauseMenuVerticalOffset = -6240.0f;
                        D_8082B90C = pauseCtx->roll;
                        Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_CLOSE);
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        Interface_SetAButtonDoAction(play, DO_ACTION_NONE);
                        pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_RETURN_TO_MENU;
                        D_8082B90C = pauseCtx->roll;
                    }
                    break;

                case PAUSE_SAVEPROMPT_STATE_4:
                    if (sramCtx->status == 0) {
                        pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_5;
                    }
                    break;

                case PAUSE_SAVEPROMPT_STATE_5:
                    if (CHECK_BTN_ALL(input->press.button, BTN_B) || CHECK_BTN_ALL(input->press.button, BTN_A) ||
                        CHECK_BTN_ALL(input->press.button, BTN_START) || (--sDelayTimer == 0)) {
                        Interface_SetAButtonDoAction(play, DO_ACTION_NONE);
                        pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_3;
                        sPauseMenuVerticalOffset = -6240.0f;
                        D_8082B90C = pauseCtx->roll;
                        Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_CLOSE);
                    }
                    break;

                case PAUSE_SAVEPROMPT_STATE_3:
                case PAUSE_SAVEPROMPT_STATE_7:
                    if (pauseCtx->roll != (D_8082B90C + 160.0f)) {
                        pauseCtx->itemPageRoll = pauseCtx->mapPageRoll = pauseCtx->questPageRoll =
                            pauseCtx->maskPageRoll += 40.0f;
                        pauseCtx->roll += 40.0f;
                        pauseCtx->infoPanelOffsetY -= 10;
                        sPauseCursorLeftX -= TRUNCF_BINANG(sPauseCursorLeftMoveOffsetX / 4);
                        sPauseCursorRightX -= TRUNCF_BINANG(sPauseCursorRightMoveOffsetX / 4);
                        pauseCtx->alpha -= 63;
                        if (pauseCtx->roll == (D_8082B90C + 160.0f)) {
                            pauseCtx->alpha = 0;
                        }
                    } else {
                        pauseCtx->debugEditor = DEBUG_EDITOR_NONE;
                        pauseCtx->state = PAUSE_STATE_UNPAUSE_CLOSE;
                        pauseCtx->itemPageRoll = pauseCtx->mapPageRoll = pauseCtx->questPageRoll =
                            pauseCtx->maskPageRoll = 160.0f;
                        pauseCtx->roll = -434.0f;
                        pauseCtx->namedItem = PAUSE_ITEM_NONE;
                        pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    }
                    break;

                case PAUSE_SAVEPROMPT_STATE_RETURN_TO_MENU:
                    pauseCtx->roll += 78.5f;
                    sPauseCursorLeftX += TRUNCF_BINANG(sPauseCursorLeftMoveOffsetX / 4);
                    sPauseCursorRightX += TRUNCF_BINANG(sPauseCursorRightMoveOffsetX / 4);

                    if (pauseCtx->roll >= -314.0f) {
                        pauseCtx->state = PAUSE_STATE_MAIN;
                        pauseCtx->savePromptState = PAUSE_SAVEPROMPT_STATE_APPEARING;
                        pauseCtx->roll = -314.0f;
                        pauseCtx->itemPageRoll = pauseCtx->mapPageRoll = pauseCtx->questPageRoll =
                            pauseCtx->maskPageRoll = 0.0f;
                        Interface_SetAButtonDoAction(play, DO_ACTION_INFO);
                        gSaveContext.buttonStatus[EQUIP_SLOT_B] =
                            gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][0];
                        gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] =
                            gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
                        gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] =
                            gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
                        gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] =
                            gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
                        // #region 2S2H [Dpad]
                        gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] =
                            gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][0];
                        gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT] =
                            gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
                        gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN] =
                            gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
                        gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] =
                            gPageSwitchNextButtonStatus[pauseCtx->pageIndex + 1][1];
                        // #endregion
                        if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) ||
                            (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT)) {
                            KaleidoScope_MoveCursorToSpecialPos(play, pauseCtx->cursorSpecialPos);
                        } else {
                            KaleidoScope_MoveCursorFromSpecialPos(play);
                        }
                    }
                    break;

                case PAUSE_SAVEPROMPT_STATE_6:
                    if (interfaceCtx->screenFillAlpha != 255) {
                        interfaceCtx->screenFillAlpha += 10;
                        if (interfaceCtx->screenFillAlpha >= 255) {
                            interfaceCtx->screenFillAlpha = 255;
                            pauseCtx->state = PAUSE_STATE_OFF;
                            GameState_SetFramerateDivisor(&play->state, 3);
                            R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_UNK4;
                            Object_LoadAll(&play->objectCtx);
                            BgCheck_InitCollisionHeaders(&play->colCtx, play);
                            STOP_GAMESTATE(&play->state);
                            SET_NEXT_GAMESTATE(&play->state, TitleSetup_Init, sizeof(TitleSetupState));
                            Audio_MuteAllSeqExceptSystemAndOcarina(20);
                            gSaveContext.seqId = NA_BGM_DISABLED;
                            gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
                        }
                    }
                    break;
            }
            break;

        case PAUSE_STATE_GAMEOVER_2:
            pauseCtx->cursorSlot[PAUSE_MAP] = R_PLAYER_FLOOR_REVERSE_INDEX + DUNGEON_FLOOR_INDEX_4;
            pauseCtx->cursorPoint[PAUSE_MAP] = R_PLAYER_FLOOR_REVERSE_INDEX + DUNGEON_FLOOR_INDEX_4;
            sPauseCursorLeftX = -175;
            sPauseCursorRightX = 155;
            pauseCtx->roll = -434.0f;
            Interface_SetHudVisibility(HUD_VISIBILITY_NONE);

            pauseCtx->iconItemSegment =
                (void*)(((uintptr_t)play->objectCtx.spaceStart + 0x30) & ~0x3F); // Messed up ALIGN64
            size0 = SEGMENT_ROM_SIZE(icon_item_static_syms);
            CmpDma_LoadAllFiles(SEGMENT_ROM_START(icon_item_static_yar), pauseCtx->iconItemSegment, size0);

            pauseCtx->iconItem24Segment = (void*)ALIGN16((uintptr_t)pauseCtx->iconItemSegment + size0);
            size1 = SEGMENT_ROM_SIZE(icon_item_24_static_syms);
            CmpDma_LoadAllFiles(SEGMENT_ROM_START(icon_item_24_static_yar), pauseCtx->iconItem24Segment, size1);

            pauseCtx->iconItemAltSegment = (void*)ALIGN16((uintptr_t)pauseCtx->iconItem24Segment + size1);
            size2 = SEGMENT_ROM_SIZE(icon_item_gameover_static);
            DmaMgr_SendRequest0(pauseCtx->iconItemAltSegment, SEGMENT_ROM_START(icon_item_gameover_static), size2);

            pauseCtx->iconItemLangSegment = (void*)ALIGN16((uintptr_t)pauseCtx->iconItemAltSegment + size2);
            DmaMgr_SendRequest0(pauseCtx->iconItemLangSegment, SEGMENT_ROM_START(icon_item_jpn_static),
                                SEGMENT_ROM_SIZE(icon_item_jpn_static));

            gSaveContext.timerStates[TIMER_ID_MOON_CRASH] = TIMER_STATE_OFF;

            sGameOverPrimR = 255;
            sGameOverPrimG = 130;
            sGameOverPrimB = 0;
            sGameOverPrimAlpha = 0;
            sGameOverEnvR = 30;
            sGameOverEnvG = 0;
            sGameOverEnvB = 0;
            sGameOverRectPosY = 98;
            sGameOverColorTimer = 30;

            pauseCtx->promptChoice = PAUSE_PROMPT_YES;
            pauseCtx->state++;
            if (gameOverCtx->state == GAMEOVER_INACTIVE) {
                pauseCtx->state++; // GAMEOVER_DEATH_START
            }
            break;

        case PAUSE_STATE_GAMEOVER_3:
            stepR = ABS_ALT(sGameOverPrimR - 30) / sGameOverColorTimer;
            stepG = ABS_ALT(sGameOverPrimG - 0) / sGameOverColorTimer;
            stepB = ABS_ALT(sGameOverPrimB - 0) / sGameOverColorTimer;
            stepA = ABS_ALT(sGameOverPrimAlpha - 255) / sGameOverColorTimer;
            if (sGameOverPrimR >= 30) {
                sGameOverPrimR -= stepR;
            } else {
                sGameOverPrimR += stepR;
            }
            if (sGameOverPrimG >= 0) {
                sGameOverPrimG -= stepG;
            } else {
                sGameOverPrimG += stepG;
            }
            if (sGameOverPrimB >= 0) {
                sGameOverPrimB -= stepB;
            } else {
                sGameOverPrimB += stepB;
            }
            if (sGameOverPrimAlpha >= 255) {
                sGameOverPrimAlpha -= stepA;
            } else {
                sGameOverPrimAlpha += stepA;
            }

            stepR = ABS_ALT(sGameOverEnvR - 255) / sGameOverColorTimer;
            stepG = ABS_ALT(sGameOverEnvG - 130) / sGameOverColorTimer;
            stepB = ABS_ALT(sGameOverEnvB - 0) / sGameOverColorTimer;
            if (sGameOverEnvR >= 255) {
                sGameOverEnvR -= stepR;
            } else {
                sGameOverEnvR += stepR;
            }
            if (sGameOverEnvG >= 130) {
                sGameOverEnvG -= stepG;
            } else {
                sGameOverEnvG += stepG;
            }
            if (sGameOverEnvB >= 0) {
                sGameOverEnvB -= stepB;
            } else {
                sGameOverEnvB += stepB;
            }

            sGameOverColorTimer--;
            if (sGameOverColorTimer == 0) {
                sGameOverPrimR = 30;
                sGameOverPrimG = 0;
                sGameOverPrimB = 0;
                sGameOverPrimAlpha = 255;

                sGameOverEnvR = 255;
                sGameOverEnvG = 130;
                sGameOverEnvB = 0;

                sGameOverColorTimer = 40;
                pauseCtx->state++;
            }
            break;

        case PAUSE_STATE_GAMEOVER_4:
            sGameOverColorTimer--;
            if (sGameOverColorTimer == 0) {
                pauseCtx->state = PAUSE_STATE_GAMEOVER_5;
            }
            break;

        case PAUSE_STATE_GAMEOVER_5:
            pauseCtx->infoPanelOffsetY += 10;
            pauseCtx->roll -= 40.0f;
            pauseCtx->itemPageRoll = pauseCtx->mapPageRoll = pauseCtx->questPageRoll = pauseCtx->maskPageRoll =
                pauseCtx->roll;
            interfaceCtx->startAlpha += 63;
            sGameOverRectPosY -= 3;
            sPauseCursorLeftX += TRUNCF_BINANG(sPauseCursorLeftMoveOffsetX / 4);
            sPauseCursorRightX += TRUNCF_BINANG(sPauseCursorRightMoveOffsetX / 4);
            pauseCtx->alpha += 31;
            if (pauseCtx->roll < -628.0f) {
                pauseCtx->roll = -628.0f;
                interfaceCtx->startAlpha = 255;
                sGameOverRectPosY = 66;
                sPauseMenuVerticalOffset = 0.0f;
                pauseCtx->alpha = 255;
                if (gameOverCtx->state == GAMEOVER_INACTIVE) {
                    pauseCtx->state = PAUSE_STATE_GAMEOVER_SAVE_PROMPT;
                } else {
                    pauseCtx->state = PAUSE_STATE_GAMEOVER_CONTINUE_PROMPT;
                }
            }
            break;

        case PAUSE_STATE_GAMEOVER_SAVE_PROMPT:
            if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                if (pauseCtx->promptChoice != PAUSE_PROMPT_YES) {
                    pauseCtx->promptChoice = PAUSE_PROMPT_YES;
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                    pauseCtx->state = PAUSE_STATE_OFF;
                    GameState_SetFramerateDivisor(&play->state, 3);
                    R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_UNK4;
                    Object_LoadAll(&play->objectCtx);
                    BgCheck_InitCollisionHeaders(&play->colCtx, play);
                } else {
                    Audio_PlaySfx(NA_SE_SY_PIECE_OF_HEART);
                    pauseCtx->promptChoice = PAUSE_PROMPT_YES;
                    Play_SaveCycleSceneFlags(play);
                    gSaveContext.save.saveInfo.playerData.savedSceneId = play->sceneId;
                    gSaveContext.save.saveInfo.playerData.health = 0x30;
                    func_8014546C(sramCtx);
                    if (!gSaveContext.flashSaveAvailable) {
                        pauseCtx->state = PAUSE_STATE_GAMEOVER_8;
                    } else {
                        Sram_SetFlashPagesDefault(sramCtx, gFlashSaveStartPages[gSaveContext.fileNum],
                                                  gFlashSaveNumPages[gSaveContext.fileNum]);
                        Sram_StartWriteToFlashDefault(sramCtx);
                        pauseCtx->state = PAUSE_STATE_GAMEOVER_7;
                    }
                    sDelayTimer = 90;
                }
            } else if ((pauseCtx->promptChoice == PAUSE_PROMPT_YES) && (stickAdjX >= 30)) {
                Audio_PlaySfx(NA_SE_SY_CURSOR);
                pauseCtx->promptChoice = PAUSE_PROMPT_NO;
            } else if ((pauseCtx->promptChoice != PAUSE_PROMPT_YES) && (stickAdjX <= -30)) {
                Audio_PlaySfx(NA_SE_SY_CURSOR);
                pauseCtx->promptChoice = PAUSE_PROMPT_YES;
            }
            break;

        case PAUSE_STATE_GAMEOVER_7:
            if (sramCtx->status == 0) {
                pauseCtx->state = PAUSE_STATE_OFF;
                GameState_SetFramerateDivisor(&play->state, 3);
                R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_UNK4;
                Object_LoadAll(&play->objectCtx);
                BgCheck_InitCollisionHeaders(&play->colCtx, play);
            }
            break;

        case PAUSE_STATE_GAMEOVER_8:
            sDelayTimer--;
            if (sDelayTimer == 0) {
                pauseCtx->state = PAUSE_STATE_GAMEOVER_CONTINUE_PROMPT;
                gameOverCtx->state++;
            } else if ((sDelayTimer <= 80) &&
                       (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_START))) {
                pauseCtx->state = PAUSE_STATE_GAMEOVER_CONTINUE_PROMPT;
                gameOverCtx->state++;
                Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_CLOSE);
            }
            break;

        case PAUSE_STATE_GAMEOVER_CONTINUE_PROMPT:
            if (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_START)) {
                if (pauseCtx->promptChoice == PAUSE_PROMPT_YES) {
                    Audio_PlaySfx(NA_SE_SY_PIECE_OF_HEART);
                    Play_SaveCycleSceneFlags(play);
                    if (gSaveContext.save.entrance == ENTRANCE(UNSET_0D, 0)) {}
                } else { // PAUSE_PROMPT_NO
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                }
                pauseCtx->state = PAUSE_STATE_GAMEOVER_10;
            }
            break;

        case PAUSE_STATE_GAMEOVER_10:
            if (interfaceCtx->screenFillAlpha != 255) {
                interfaceCtx->screenFillAlpha += 10;
                if (interfaceCtx->screenFillAlpha >= 255) {
                    interfaceCtx->screenFillAlpha = 255;

                    pauseCtx->state = PAUSE_STATE_OFF;
                    GameState_SetFramerateDivisor(&play->state, 3);
                    R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_UNK4;
                    Object_LoadAll(&play->objectCtx);
                    BgCheck_InitCollisionHeaders(&play->colCtx, play);

                    if (pauseCtx->promptChoice == PAUSE_PROMPT_YES) {
                        func_80169FDC(play);
                        gSaveContext.respawnFlag = -2;
                        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
                        gSaveContext.save.saveInfo.playerData.health = 0x30;
                        Audio_SetSpec(0xA);
                        gSaveContext.healthAccumulator = 0;
                        gSaveContext.magicState = MAGIC_STATE_IDLE;
                        gSaveContext.magicFlag = 0;
                        gSaveContext.magicCapacity = 0;
                        gSaveContext.magicFillTarget = gSaveContext.save.saveInfo.playerData.magic;
                        gSaveContext.save.saveInfo.playerData.magicLevel = 0;
                        gSaveContext.save.saveInfo.playerData.magic = 0;
                    } else { // PAUSE_PROMPT_NO
                        STOP_GAMESTATE(&play->state);
                        SET_NEXT_GAMESTATE(&play->state, TitleSetup_Init, sizeof(TitleSetupState));
                    }
                }
            }
            break;

        case PAUSE_STATE_OWL_WARP_2:
            sPauseMenuVerticalOffset = -6240.0f;

            sUnpausedButtonStatus[EQUIP_SLOT_B] = gSaveContext.buttonStatus[EQUIP_SLOT_B];
            sUnpausedButtonStatus[EQUIP_SLOT_C_LEFT] = gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT];
            sUnpausedButtonStatus[EQUIP_SLOT_C_DOWN] = gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN];
            sUnpausedButtonStatus[EQUIP_SLOT_C_RIGHT] = gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT];
            sUnpausedButtonStatus[EQUIP_SLOT_A] = gSaveContext.buttonStatus[EQUIP_SLOT_A];
            // #region 2S2H [Dpad]
            sUnpausedDpadStatus[EQUIP_SLOT_D_RIGHT] = gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT];
            sUnpausedDpadStatus[EQUIP_SLOT_D_LEFT] = gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT];
            sUnpausedDpadStatus[EQUIP_SLOT_D_DOWN] = gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN];
            sUnpausedDpadStatus[EQUIP_SLOT_D_UP] = gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP];
            // #endregion

            pauseCtx->cursorXIndex[PAUSE_MAP] = 0;
            pauseCtx->cursorSlot[PAUSE_MAP] = R_PLAYER_FLOOR_REVERSE_INDEX + DUNGEON_FLOOR_INDEX_4;
            pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->cursorMapDungeonItem =
                R_PLAYER_FLOOR_REVERSE_INDEX + DUNGEON_FLOOR_INDEX_4;

            sPauseCursorLeftX = -175;
            sPauseCursorRightX = 155;

            pauseCtx->iconItemSegment = (void*)ALIGN16((uintptr_t)play->objectCtx.spaceStart);
            size0 = SEGMENT_ROM_SIZE(icon_item_static_syms);
            CmpDma_LoadAllFiles(SEGMENT_ROM_START(icon_item_static_yar), pauseCtx->iconItemSegment, size0);

            pauseCtx->iconItemAltSegment = (void*)ALIGN16((uintptr_t)pauseCtx->iconItemSegment + size0);
            sInDungeonScene = false;
            size1 = SEGMENT_ROM_SIZE(icon_item_field_static);
            DmaMgr_SendRequest0(pauseCtx->iconItemAltSegment, SEGMENT_ROM_START(icon_item_field_static), size1);

            pauseCtx->iconItemLangSegment = (void*)ALIGN16((uintptr_t)pauseCtx->iconItemAltSegment + size1);
            size2 = SEGMENT_ROM_SIZE(icon_item_jpn_static);
            DmaMgr_SendRequest0(pauseCtx->iconItemLangSegment, SEGMENT_ROM_START(icon_item_jpn_static), size2);

            pauseCtx->nameSegment = (void*)ALIGN16((uintptr_t)pauseCtx->iconItemLangSegment + size2);
            Interface_SetAButtonDoAction(play, DO_ACTION_WARP);
            worldMapCursorPoint = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
            Kaleido_LoadMapNameStatic(&pauseCtx->nameSegment, worldMapCursorPoint);

            pauseCtx->iconItemVtxSegment = (void*)ALIGN16((uintptr_t)pauseCtx->nameSegment + 0xA00);
            DmaMgr_SendRequest0(pauseCtx->iconItemVtxSegment, SEGMENT_ROM_START(icon_item_vtx_static),
                                SEGMENT_ROM_SIZE(icon_item_vtx_static));

            pauseCtx->state = PAUSE_STATE_OWL_WARP_3;
            sGameOverRectPosY = 98;
            pauseCtx->promptChoice = PAUSE_PROMPT_YES;
            break;

        case PAUSE_STATE_OWL_WARP_3:
            R_PAUSE_OWL_WARP_ALPHA += 20;
            pauseCtx->infoPanelOffsetY += 10;
            pauseCtx->mapPageRoll -= 40.0f;
            interfaceCtx->startAlpha += 63;
            sGameOverRectPosY -= 3;
            sPauseCursorLeftX += TRUNCF_BINANG(sPauseCursorLeftMoveOffsetX / 4);
            sPauseCursorRightX += TRUNCF_BINANG(sPauseCursorRightMoveOffsetX / 4);
            pauseCtx->alpha += 31;

            if (pauseCtx->mapPageRoll == 0) {
                interfaceCtx->startAlpha = 255;
                sPauseMenuVerticalOffset = 0.0f;
                pauseCtx->alpha = 255;
                pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                pauseCtx->cursorSpecialPos = 0;
                pauseCtx->state = PAUSE_STATE_OWL_WARP_SELECT;
                R_PAUSE_OWL_WARP_ALPHA = 120;
            }
            break;

        case PAUSE_STATE_OWL_WARP_SELECT:
            if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_B)) {
                Interface_SetAButtonDoAction(play, DO_ACTION_NONE);
                pauseCtx->state = PAUSE_STATE_OWL_WARP_6;
                sPauseMenuVerticalOffset = -6240.0f;
                Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_CLOSE);
                play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                gSaveContext.prevHudVisibility = HUD_VISIBILITY_ALL;
            } else if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                Audio_PlaySfx(NA_SE_SY_DECIDE);
                Message_StartTextbox(play, 0x1B93, NULL);
                pauseCtx->state = PAUSE_STATE_OWL_WARP_CONFIRM;
            } else {
                KaleidoScope_UpdateOwlWarpNamePanel(play);
            }
            break;

        case PAUSE_STATE_OWL_WARP_CONFIRM:
            if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                msgCtx->msgLength = 0;
                msgCtx->msgMode = MSGMODE_NONE;
                if (msgCtx->choiceIndex == 0) {
                    Interface_SetAButtonDoAction(play, DO_ACTION_NONE);
                    pauseCtx->state = PAUSE_STATE_OWL_WARP_6;
                    sPauseMenuVerticalOffset = -6240.0f;
                    Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_CLOSE);
                    play->msgCtx.ocarinaMode = sCursorPointsToOcarinaModes[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]];
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                } else {
                    pauseCtx->state = PAUSE_STATE_OWL_WARP_SELECT;
                    Interface_SetAButtonDoAction(play, DO_ACTION_WARP);
                    Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                msgCtx->msgLength = 0;
                msgCtx->msgMode = MSGMODE_NONE;
                pauseCtx->state = PAUSE_STATE_OWL_WARP_SELECT;
                Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
            } else if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
                msgCtx->msgLength = 0;
                msgCtx->msgMode = MSGMODE_NONE;
                Interface_SetAButtonDoAction(play, DO_ACTION_NONE);
                pauseCtx->state = PAUSE_STATE_OWL_WARP_6;
                sPauseMenuVerticalOffset = -6240.0f;
                Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_CLOSE);
                play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                gSaveContext.prevHudVisibility = HUD_VISIBILITY_ALL;
            }
            break;

        case PAUSE_STATE_OWL_WARP_6:
            if (pauseCtx->mapPageRoll != 160.0f) {
                R_PAUSE_OWL_WARP_ALPHA -= 60;
                if (R_PAUSE_OWL_WARP_ALPHA <= 0) {
                    R_PAUSE_OWL_WARP_ALPHA = 0;
                }
                pauseCtx->infoPanelOffsetY -= 10;
                pauseCtx->mapPageRoll += 40.0f;
                interfaceCtx->startAlpha -= 63;
                sPauseCursorLeftX -= TRUNCF_BINANG(sPauseCursorLeftMoveOffsetX / 4);
                sPauseCursorRightX -= TRUNCF_BINANG(sPauseCursorRightMoveOffsetX / 4);
                pauseCtx->alpha -= 63;
                if (pauseCtx->mapPageRoll == 160.0f) {
                    pauseCtx->alpha = 0;
                }
            } else {
                pauseCtx->debugEditor = DEBUG_EDITOR_NONE;
                pauseCtx->state = PAUSE_STATE_UNPAUSE_CLOSE;
                pauseCtx->itemPageRoll = pauseCtx->mapPageRoll = pauseCtx->questPageRoll = pauseCtx->maskPageRoll =
                    160.0f;
                pauseCtx->namedItem = PAUSE_ITEM_NONE;
                interfaceCtx->startAlpha = 0;
                pauseCtx->pageIndex = pauseCtx->unk_2C8;
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = pauseCtx->unk_2CA;
            }
            break;

        case PAUSE_STATE_UNPAUSE_SETUP:
            if (pauseCtx->itemPageRoll != 160.0f) {
                pauseCtx->infoPanelOffsetY -= 10;
                pauseCtx->itemPageRoll = pauseCtx->mapPageRoll = pauseCtx->questPageRoll = pauseCtx->maskPageRoll +=
                    40.0f;
                interfaceCtx->startAlpha -= 63;
                sPauseCursorLeftX -= TRUNCF_BINANG(sPauseCursorLeftMoveOffsetX / 4);
                sPauseCursorRightX -= TRUNCF_BINANG(sPauseCursorRightMoveOffsetX / 4);
                pauseCtx->alpha -= 63;
                if (pauseCtx->itemPageRoll == 160.0f) {
                    pauseCtx->alpha = 0;
                }
            } else {
                pauseCtx->debugEditor = DEBUG_EDITOR_NONE;
                pauseCtx->state = PAUSE_STATE_UNPAUSE_CLOSE;
                pauseCtx->maskPageRoll = 160.0f;
                pauseCtx->questPageRoll = 160.0f;
                pauseCtx->mapPageRoll = 160.0f;
                pauseCtx->itemPageRoll = 160.0f;
                pauseCtx->namedItem = PAUSE_ITEM_NONE;
                interfaceCtx->startAlpha = 0;
            }
            break;

        case PAUSE_STATE_UNPAUSE_CLOSE:
            if (!GameInteractor_Should(VB_KALEIDO_UNPAUSE_CLOSE, true)) {
                break;
            }
            pauseCtx->state = PAUSE_STATE_OFF;
            GameState_SetFramerateDivisor(&play->state, 3);
            R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_UNK4;
            Object_LoadAll(&play->objectCtx);
            BgCheck_InitCollisionHeaders(&play->colCtx, play);

            gSaveContext.buttonStatus[EQUIP_SLOT_B] = sUnpausedButtonStatus[EQUIP_SLOT_B];
            gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = sUnpausedButtonStatus[EQUIP_SLOT_C_LEFT];
            gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = sUnpausedButtonStatus[EQUIP_SLOT_C_DOWN];
            gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = sUnpausedButtonStatus[EQUIP_SLOT_C_RIGHT];
            gSaveContext.buttonStatus[EQUIP_SLOT_A] = sUnpausedButtonStatus[EQUIP_SLOT_A];
            // #region 2S2H [Dpad]
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] = sUnpausedDpadStatus[EQUIP_SLOT_D_RIGHT];
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT] = sUnpausedDpadStatus[EQUIP_SLOT_D_LEFT];
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN] = sUnpausedDpadStatus[EQUIP_SLOT_D_DOWN];
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP] = sUnpausedDpadStatus[EQUIP_SLOT_D_UP];
            // #endregion

            Interface_UpdateButtonsPart2(play);
            gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
            Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
            MsgEvent_SendNullTask();
            Skybox_Reload(play, &play->skyboxCtx, play->skyboxId);

            if ((msgCtx->msgMode != MSGMODE_NONE) && (msgCtx->currentTextId == 0xFF)) {
                Interface_SetBButtonInterfaceDoAction(play, DO_ACTION_STOP);
                Interface_SetAButtonDoAction(play, DO_ACTION_STOP);
                Interface_SetHudVisibility(HUD_VISIBILITY_A_B_C);
            } else {
                interfaceCtx->bButtonInterfaceDoActionActive = interfaceCtx->bButtonInterfaceDoAction = 0;
            }
            gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
            Interface_SetHudVisibility(sUnpausedHudVisibility);
            Audio_SetPauseState(false);
            break;

        default:
            break;
    }

    // If stickAdj is held, set a delay to allow the cursor to read the next input.
    // The first delay is given a longer time than all subsequent delays.
    if (!IS_PAUSE_STATE_GAMEOVER(pauseCtx) && (pauseCtx->state != PAUSE_STATE_SAVEPROMPT)) {
        if (pauseCtx->stickAdjX < -30) {
            if (sStickXRepeatState == -1) {
                sStickXRepeatTimer--;
                if (sStickXRepeatTimer < 0) {
                    // Allow the input to register and apply the delay for all subsequent repeated inputs
                    sStickXRepeatTimer = 2;
                } else {
                    // Cancel the current input
                    pauseCtx->stickAdjX = 0;
                }
            } else {
                // Allow the input to register and apply the delay for the first repeated input
                sStickXRepeatTimer = 10;
                sStickXRepeatState = -1;
            }
        } else if (pauseCtx->stickAdjX > 30) {
            if (sStickXRepeatState == 1) {
                sStickXRepeatTimer--;
                if (sStickXRepeatTimer < 0) {
                    // Allow the input to register and apply the delay for all subsequent repeated inputs
                    sStickXRepeatTimer = 2;
                } else {
                    // Cancel the current input
                    pauseCtx->stickAdjX = 0;
                }
            } else {
                // Allow the input to register and apply the delay for the first repeated input
                sStickXRepeatTimer = 10;
                sStickXRepeatState = 1;
            }
        } else {
            sStickXRepeatState = 0;
        }

        if (pauseCtx->stickAdjY < -30) {
            if (sStickYRepeatState == -1) {
                sStickYRepeatTimer--;
                if (sStickYRepeatTimer < 0) {
                    // Allow the input to register and apply the delay for all subsequent repeated inputs
                    sStickYRepeatTimer = 2;
                } else {
                    // Cancel the current input
                    pauseCtx->stickAdjY = 0;
                }
            } else {
                // Allow the input to register and apply the delay for the first repeated input
                sStickYRepeatTimer = 10;
                sStickYRepeatState = -1;
            }
        } else if (pauseCtx->stickAdjY > 30) {
            if (sStickYRepeatState == 1) {
                sStickYRepeatTimer--;
                if (sStickYRepeatTimer < 0) {
                    // Allow the input to register and apply the delay for all subsequent repeated inputs
                    sStickYRepeatTimer = 2;
                } else {
                    // Cancel the current input
                    pauseCtx->stickAdjY = 0;
                }
            } else {
                // Allow the input to register and apply the delay for the first repeated input
                sStickYRepeatTimer = 10;
                sStickYRepeatState = 1;
            }
        } else {
            sStickYRepeatState = 0;
        }
    }

    // Process the Cursor input
    if ((R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_READY) && (pauseCtx->debugEditor == DEBUG_EDITOR_NONE) &&
        !IS_PAUSE_STATE_OWL_WARP(pauseCtx) &&
        (((pauseCtx->state >= PAUSE_STATE_OPENING_3) && (pauseCtx->state <= PAUSE_STATE_SAVEPROMPT)) ||
         ((pauseCtx->state >= PAUSE_STATE_GAMEOVER_2) && (pauseCtx->state <= PAUSE_STATE_UNPAUSE_SETUP)))) {
        if (!IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
            switch (pauseCtx->pageIndex) {
                case PAUSE_ITEM:
                    KaleidoScope_UpdateItemCursor(play);
                    break;

                case PAUSE_MAP:
                    if (sInDungeonScene) {
                        KaleidoScope_UpdateDungeonCursor(play);
                    } else {
                        KaleidoScope_UpdateWorldMapCursor(play);
                    }
                    break;

                case PAUSE_QUEST:
                    KaleidoScope_UpdateQuestCursor(play);
                    break;

                case PAUSE_MASK:
                    KaleidoScope_UpdateMaskCursor(play);
                    break;
            }

            if ((pauseCtx->state == PAUSE_STATE_MAIN) &&
                ((pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) ||
                 (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG))) {
                KaleidoScope_HandlePageToggles(play, input);
            }
        }
        if (pauseCtx->state == PAUSE_STATE_MAIN) {
            KaleidoScope_UpdateNamePanel(play);
        }
    } else if (pauseCtx->state == PAUSE_STATE_OWL_WARP_SELECT) {
        KaleidoScope_UpdateWorldMapCursor(play);
        KaleidoScope_UpdateNamePanel(play);
    }

    // Update the inventory editor
    if ((pauseCtx->debugEditor == DEBUG_EDITOR_INVENTORY_INIT) || (pauseCtx->debugEditor == DEBUG_EDITOR_INVENTORY)) {
        KaleidoScope_UpdateInventoryEditor(play);
    }
    GameInteractor_ExecuteOnKaleidoUpdate(pauseCtx);
}
