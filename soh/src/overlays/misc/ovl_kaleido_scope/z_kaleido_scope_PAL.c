#include "z_kaleido_scope.h"
#include <stdlib.h>
#include <string.h>

#include "textures/item_name_static/item_name_static.h"
#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/icon_item_nes_static/icon_item_nes_static.h"
#include "textures/icon_item_ger_static/icon_item_ger_static.h"
#include "textures/icon_item_fra_static/icon_item_fra_static.h"
#include "textures/icon_item_gameover_static/icon_item_gameover_static.h"
#include "textures/icon_item_gameover_static/icon_item_gameover_static.h"
#include "textures/map_name_static/map_name_static.h"
#include "textures/map_48x85_static/map_48x85_static.h"
#include "vt.h"

#include "soh/frame_interpolation.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include "soh/Enhancements/randomizer/randomizer_grotto.h"
#include "soh/Enhancements/cosmetics/cosmeticsTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"


static void* sEquipmentFRATexs[] = {
    gPauseEquipment00FRATex, gPauseEquipment01Tex, gPauseEquipment02Tex, gPauseEquipment03Tex, gPauseEquipment04Tex,
    gPauseEquipment10FRATex, gPauseEquipment11Tex, gPauseEquipment12Tex, gPauseEquipment13Tex, gPauseEquipment14Tex,
    gPauseEquipment20FRATex, gPauseEquipment21Tex, gPauseEquipment22Tex, gPauseEquipment23Tex, gPauseEquipment24Tex,
};
static void* sSelectItemFRATexs[] = {
    gPauseSelectItem00FRATex, gPauseSelectItem01Tex,    gPauseSelectItem02Tex,    gPauseSelectItem03Tex,
    gPauseSelectItem04Tex,    gPauseSelectItem10FRATex, gPauseSelectItem11Tex,    gPauseSelectItem12Tex,
    gPauseSelectItem13Tex,    gPauseSelectItem14Tex,    gPauseSelectItem20FRATex, gPauseSelectItem21Tex,
    gPauseSelectItem22Tex,    gPauseSelectItem23Tex,    gPauseSelectItem24Tex,
};
static void* sMapFRATexs[] = {
    gPauseMap00Tex,    gPauseMap01Tex, gPauseMap02Tex, gPauseMap03Tex, gPauseMap04Tex,
    gPauseMap10FRATex, gPauseMap11Tex, gPauseMap12Tex, gPauseMap13Tex, gPauseMap14Tex,
    gPauseMap20Tex,    gPauseMap21Tex, gPauseMap22Tex, gPauseMap23Tex, gPauseMap24Tex,
};
static void* sQuestStatusFRATexs[] = {
    gPauseQuestStatus00Tex, gPauseQuestStatus01Tex,    gPauseQuestStatus02Tex, gPauseQuestStatus03Tex,
    gPauseQuestStatus04Tex, gPauseQuestStatus10FRATex, gPauseQuestStatus11Tex, gPauseQuestStatus12Tex,
    gPauseQuestStatus13Tex, gPauseQuestStatus14Tex,    gPauseQuestStatus20Tex, gPauseQuestStatus21Tex,
    gPauseQuestStatus22Tex, gPauseQuestStatus23Tex,    gPauseQuestStatus24Tex,
};
static void* sSaveFRATexs[] = {
    gPauseSave00FRATex, gPauseSave01Tex, gPauseSave02Tex, gPauseSave03Tex, gPauseSave04Tex,
    gPauseSave10FRATex, gPauseSave11Tex, gPauseSave12Tex, gPauseSave13Tex, gPauseSave14Tex,
    gPauseSave20FRATex, gPauseSave21Tex, gPauseSave22Tex, gPauseSave23Tex, gPauseSave24Tex,
};

static void* sEquipmentGERTexs[] = {
    gPauseEquipment00GERTex, gPauseEquipment01Tex, gPauseEquipment02Tex, gPauseEquipment03Tex, gPauseEquipment04Tex,
    gPauseEquipment10GERTex, gPauseEquipment11Tex, gPauseEquipment12Tex, gPauseEquipment13Tex, gPauseEquipment14Tex,
    gPauseEquipment20GERTex, gPauseEquipment21Tex, gPauseEquipment22Tex, gPauseEquipment23Tex, gPauseEquipment24Tex,
};
static void* sSelectItemGERTexs[] = {
    gPauseSelectItem00GERTex, gPauseSelectItem01Tex,    gPauseSelectItem02Tex,    gPauseSelectItem03Tex,
    gPauseSelectItem04Tex,    gPauseSelectItem10GERTex, gPauseSelectItem11Tex,    gPauseSelectItem12Tex,
    gPauseSelectItem13Tex,    gPauseSelectItem14Tex,    gPauseSelectItem20GERTex, gPauseSelectItem21Tex,
    gPauseSelectItem22Tex,    gPauseSelectItem23Tex,    gPauseSelectItem24Tex,
};
static void* sMapGERTexs[] = {
    gPauseMap00Tex,    gPauseMap01Tex, gPauseMap02Tex, gPauseMap03Tex, gPauseMap04Tex,
    gPauseMap10GERTex, gPauseMap11Tex, gPauseMap12Tex, gPauseMap13Tex, gPauseMap14Tex,
    gPauseMap20Tex,    gPauseMap21Tex, gPauseMap22Tex, gPauseMap23Tex, gPauseMap24Tex,
};
static void* sQuestStatusGERTexs[] = {
    gPauseQuestStatus00Tex, gPauseQuestStatus01Tex,    gPauseQuestStatus02Tex, gPauseQuestStatus03Tex,
    gPauseQuestStatus04Tex, gPauseQuestStatus10GERTex, gPauseQuestStatus11Tex, gPauseQuestStatus12Tex,
    gPauseQuestStatus13Tex, gPauseQuestStatus14Tex,    gPauseQuestStatus20Tex, gPauseQuestStatus21Tex,
    gPauseQuestStatus22Tex, gPauseQuestStatus23Tex,    gPauseQuestStatus24Tex,
};
static void* sSaveGERTexs[] = {
    gPauseSave00Tex,    gPauseSave01Tex, gPauseSave02Tex, gPauseSave03Tex, gPauseSave04Tex,
    gPauseSave10GERTex, gPauseSave11Tex, gPauseSave12Tex, gPauseSave13Tex, gPauseSave14Tex,
    gPauseSave20GERTex, gPauseSave21Tex, gPauseSave22Tex, gPauseSave23Tex, gPauseSave24Tex,
};

static void* sEquipmentENGTexs[] = {
    gPauseEquipment00Tex,    gPauseEquipment01Tex, gPauseEquipment02Tex, gPauseEquipment03Tex, gPauseEquipment04Tex,
    gPauseEquipment10ENGTex, gPauseEquipment11Tex, gPauseEquipment12Tex, gPauseEquipment13Tex, gPauseEquipment14Tex,
    gPauseEquipment20Tex,    gPauseEquipment21Tex, gPauseEquipment22Tex, gPauseEquipment23Tex, gPauseEquipment24Tex,
};
static void* sSelectItemENGTexs[] = {
    gPauseSelectItem00ENGTex, gPauseSelectItem01Tex,    gPauseSelectItem02Tex,    gPauseSelectItem03Tex,
    gPauseSelectItem04Tex,    gPauseSelectItem10ENGTex, gPauseSelectItem11Tex,    gPauseSelectItem12Tex,
    gPauseSelectItem13Tex,    gPauseSelectItem14Tex,    gPauseSelectItem20ENGTex, gPauseSelectItem21Tex,
    gPauseSelectItem22Tex,    gPauseSelectItem23Tex,    gPauseSelectItem24Tex,
};
static void* sMapENGTexs[] = {
    gPauseMap00Tex,    gPauseMap01Tex, gPauseMap02Tex, gPauseMap03Tex, gPauseMap04Tex,
    gPauseMap10ENGTex, gPauseMap11Tex, gPauseMap12Tex, gPauseMap13Tex, gPauseMap14Tex,
    gPauseMap20Tex,    gPauseMap21Tex, gPauseMap22Tex, gPauseMap23Tex, gPauseMap24Tex,
};
static void* sQuestStatusENGTexs[] = {
    gPauseQuestStatus00ENGTex, gPauseQuestStatus01Tex,    gPauseQuestStatus02Tex,    gPauseQuestStatus03Tex,
    gPauseQuestStatus04Tex,    gPauseQuestStatus10ENGTex, gPauseQuestStatus11Tex,    gPauseQuestStatus12Tex,
    gPauseQuestStatus13Tex,    gPauseQuestStatus14Tex,    gPauseQuestStatus20ENGTex, gPauseQuestStatus21Tex,
    gPauseQuestStatus22Tex,    gPauseQuestStatus23Tex,    gPauseQuestStatus24Tex,
};
static void* sSaveENGTexs[] = {
    gPauseSave00Tex,    gPauseSave01Tex, gPauseSave02Tex, gPauseSave03Tex, gPauseSave04Tex,
    gPauseSave10ENGTex, gPauseSave11Tex, gPauseSave12Tex, gPauseSave13Tex, gPauseSave14Tex,
    gPauseSave20Tex,    gPauseSave21Tex, gPauseSave22Tex, gPauseSave23Tex, gPauseSave24Tex,
};

static void* sGameOverTexs[] = {
    gPauseSave00Tex,     gPauseSave01Tex, gPauseSave02Tex, gPauseSave03Tex, gPauseSave04Tex,
    gPauseGameOver10Tex, gPauseSave11Tex, gPauseSave12Tex, gPauseSave13Tex, gPauseSave14Tex,
    gPauseSave20Tex,     gPauseSave21Tex, gPauseSave22Tex, gPauseSave23Tex, gPauseSave24Tex,
};

static void* sDungeonMapTexs[] = {
    gDekuTreePauseScreenMapFloor3LeftTex,
    gDekuTreePauseScreenMapFloor3RightTex,
    gDekuTreePauseScreenMapFloor2LeftTex,
    gDekuTreePauseScreenMapFloor2RightTex,
    gDekuTreePauseScreenMapFloor1LeftTex,
    gDekuTreePauseScreenMapFloor1RightTex,
    gDekuTreePauseScreenMapBasement1LeftTex,
    gDekuTreePauseScreenMapBasement1RightTex,
    gDekuTreePauseScreenMapBasement2LeftTex,
    gDekuTreePauseScreenMapBasement2RightTex,
    gDodongosCavernPauseScreenMapFloor2LeftTex,
    gDodongosCavernPauseScreenMapFloor2RightTex,
    gDodongosCavernPauseScreenMapFloor1LeftTex,
    gDodongosCavernPauseScreenMapFloor1RightTex,
    gJabuPauseScreenMapFloor1LeftTex,
    gJabuPauseScreenMapFloor1RightTex,
    gJabuPauseScreenMapBasement1LeftTex,
    gJabuPauseScreenMapBasement1RightTex,
    gForestTemplePauseScreenMapFloor2LeftTex,
    gForestTemplePauseScreenMapFloor2RightTex,
    gForestTemplePauseScreenMapFloor1LeftTex,
    gForestTemplePauseScreenMapFloor1RightTex,
    gForestTemplePauseScreenMapBasement1LeftTex,
    gForestTemplePauseScreenMapBasement1RightTex,
    gForestTemplePauseScreenMapBasement2LeftTex,
    gForestTemplePauseScreenMapBasement2RightTex,
    gFireTemplePauseScreenMapFloor5LeftTex,
    gFireTemplePauseScreenMapFloor5RightTex,
    gFireTemplePauseScreenMapFloor4LeftTex,
    gFireTemplePauseScreenMapFloor4RightTex,
    gFireTemplePauseScreenMapFloor3LeftTex,
    gFireTemplePauseScreenMapFloor3RightTex,
    gFireTemplePauseScreenMapFloor2LeftTex,
    gFireTemplePauseScreenMapFloor2RightTex,
    gFireTemplePauseScreenMapFloor1LeftTex,
    gFireTemplePauseScreenMapFloor1RightTex,
    gWaterTemplePauseScreenMapFloor3LeftTex,
    gWaterTemplePauseScreenMapFloor3RightTex,
    gWaterTemplePauseScreenMapFloor2LeftTex,
    gWaterTemplePauseScreenMapFloor2RightTex,
    gWaterTemplePauseScreenMapFloor1LeftTex,
    gWaterTemplePauseScreenMapFloor1RightTex,
    gWaterTemplePauseScreenMapBasement1LeftTex,
    gWaterTemplePauseScreenMapBasement1RightTex,
    gSpiritTemplePauseScreenMapFloor4LeftTex,
    gSpiritTemplePauseScreenMapFloor4RightTex,
    gSpiritTemplePauseScreenMapFloor3LeftTex,
    gSpiritTemplePauseScreenMapFloor3RightTex,
    gSpiritTemplePauseScreenMapFloor2LeftTex,
    gSpiritTemplePauseScreenMapFloor2RightTex,
    gSpiritTemplePauseScreenMapFloor1LeftTex,
    gSpiritTemplePauseScreenMapFloor1RightTex,
    gShadowTemplePauseScreenMapBasement1LeftTex,
    gShadowTemplePauseScreenMapBasement1RightTex,
    gShadowTemplePauseScreenMapBasement2LeftTex,
    gShadowTemplePauseScreenMapBasement2RightTex,
    gShadowTemplePauseScreenMapBasement3LeftTex,
    gShadowTemplePauseScreenMapBasement3RightTex,
    gShadowTemplePauseScreenMapBasement4LeftTex,
    gShadowTemplePauseScreenMapBasement4RightTex,
    gBottomOfTheWellPauseScreenMapBasement1LeftTex,
    gBottomOfTheWellPauseScreenMapBasement1RightTex,
    gBottomOfTheWellPauseScreenMapBasement2LeftTex,
    gBottomOfTheWellPauseScreenMapBasement2RightTex,
    gBottomOfTheWellPauseScreenMapBasement3LeftTex,
    gBottomOfTheWellPauseScreenMapBasement3RightTex,
    gIceCavernPauseScreenMapFloor1LeftTex,
    gIceCavernPauseScreenMapFloor1RightTex,
};

static void* sEquipmentTexs[] = {
    sEquipmentENGTexs,
    sEquipmentGERTexs,
    sEquipmentFRATexs,
};

static void* sSelectItemTexs[] = {
    sSelectItemENGTexs,
    sSelectItemGERTexs,
    sSelectItemFRATexs,
};

static void* sMapTexs[] = {
    sMapENGTexs,
    sMapGERTexs,
    sMapFRATexs,
};

static void* sQuestStatusTexs[] = {
    sQuestStatusENGTexs,
    sQuestStatusGERTexs,
    sQuestStatusFRATexs,
};

static void* sSaveTexs[] = {
    sSaveENGTexs,
    sSaveGERTexs,
    sSaveFRATexs,
};

static void* iconNameTextures[] =
{
    gDekuStickItemNameENGTex,
    gDekuNutItemNameENGTex,
    gBombItemNameENGTex,
    gFairyBowItemNameENGTex,
    gFireArrowItemNameENGTex,
    gDinsFireItemNameENGTex,
    gFairySlingshotItemNameENGTex,
    gFairyOcarinaItemNameENGTex,
    gOcarinaOfTimeItemNameENGTex,
    gBombchuItemNameENGTex,
    gHookshotItemNameENGTex,
    gLongshotItemNameENGTex,
    gIceArrowItemNameENGTex,
    gFaroresWindItemNameENGTex,
    gBoomerangItemNameENGTex,
    gLensItemNameENGTex,
    gMagicBeansItemNameENGTex,
    gMegatonHammerItemNameENGTex,
    gLightArrowItemNameENGTex,
    gNayrusLoveItemNameENGTex,
    gEmptyBottleItemNameENGTex,
    gRedPotionItemNameENGTex,
    gGreenPotionItemNameENGTex,
    gBluePotionItemNameENGTex,
    gBottledFairyItemNameENGTex,
    gFishItemNameENGTex,
    gFullMilkItemNameENGTex,
    gRutosLetterItemNameENGTex,
    gBlueFireItemNameENGTex,
    gBugItemNameENGTex,
    gBigPoeItemNameENGTex,
    gHalfMilkItemNameENGTex,
    gPoeItemNameENGTex,
    gWeirdEggItemNameENGTex,
    gCuccoItemNameENGTex,
    gZeldasLetterItemNameENGTex,
    gKeatonMaskItemNameENGTex,
    gSkullMaskItemNameENGTex,
    gSpookyMaskItemNameENGTex,
    gBunnyHoodItemNameENGTex,
    gGoronMaskItemNameENGTex,
    gZoraMaskItemNameENGTex,
    gGerudoMaskItemNameENGTex,
    gMaskofTruthItemNameENGTex,
    gSOLDOUTItemNameENGTex,
    gPocketEggItemNameENGTex,
    gPocketCuccoItemNameENGTex,
    gCojiroItemNameENGTex,
    gOddMushroomItemNameENGTex,
    gOddPotionItemNameENGTex,
    gPoachersSawItemNameENGTex,
    gBrokenGoronsSwordItemNameENGTex,
    gPrescriptionItemNameENGTex,
    gEyeBallFrogItemNameENGTex,
    gEyeDropsItemNameENGTex,
    gClaimCheckItemNameENGTex,
    gUnusedWindMedallionItemName1JPNTex,
    gUnusedFireMedallionItemName1JPNTex,
    gUnusedIceMedallionItemName1JPNTex,
    gKokiriSwordItemNameENGTex,
    gMasterSwordItemNameENGTex,
    gGiantsKnifeItemNameENGTex,
    gDekuShieldItemNameENGTex,
    gHylianShieldItemNameENGTex,
    gMirrorShieldItemNameENGTex,
    gKokiriTunicItemNameENGTex,
    gGoronTunicItemNameENGTex,
    gZoraTunicItemNameENGTex,
    gKokiriBootsItemNameENGTex,
    gIronBootsItemNameENGTex,
    gHoverBootsItemNameENGTex,
    gBulletBag30ItemNameENGTex,
    gBulletBag40ItemNameENGTex,
    gBulletBag50ItemNameENGTex,
    gQuiver30ItemNameENGTex,
    gQuiver40ItemNameENGTex,
    gQuiver50ItemNameENGTex,
    gBombBag20ItemNameENGTex,
    gBombBag30ItemNameENGTex,
    gBombBag40ItemNameENGTex,
    gGoronsBraceletItemNameENGTex,
    gSilverGauntletsItemNameENGTex,
    gGoldenGauntletsItemNameENGTex,
    gSilverScaleItemNameENGTex,
    gGoldenScaleItemNameENGTex,
    gBrokenGiantsKnifeItemNameENGTex,
    gUnusedBossKeyItemName1JPNTex,
    gUnusedBossKeyItemName2JPNTex,
    gUnusedBossKeyItemName3JPNTex,
    gUnusedBossKeyItemName4JPNTex,
    gMinuetOfForestItemNameENGTex,
    gBoleroOfFireItemNameENGTex,
    gSerenadeOfWaterItemNameENGTex,
    gRequiemOfSpiritItemNameENGTex,
    gNocturneOfShadowItemNameENGTex,
    gPreludeOfLightItemNameENGTex,
    gZeldasLullabyItemNameENGTex,
    gEponasSongItemNameENGTex,
    gSariasSongItemNameENGTex,
    gSunsSongItemNameENGTex,
    gSongOfTimeItemNameENGTex,
    gSongOfStormsItemNameENGTex,
    gForestMedallionItemNameENGTex,
    gFireMedallionItemNameENGTex,
    gWaterMedallionItemNameENGTex,
    gSpiritMedallionItemNameENGTex,
    gShadowMedallionItemNameENGTex,
    gLightMedallionItemNameENGTex,
    gKokiriEmeraldItemNameENGTex,
    gGoronsRubyItemNameENGTex,
    gZorasSapphireItemNameENGTex,
    gStoneofAgonyItemNameENGTex,
    gGerudosCardItemNameENGTex,
    gGoldSkulltulaItemNameENGTex,
    gPieceOfHeartItemNameENGTex,
    gUnusedPieceOfHeartItemName1JPNTex,
    gUnusedBigKeyItemNameENGTex,
    gCompassItemNameENGTex,
    gDungeonMapItemNameENGTex,
    gUnusedBossKeyItemName5JPNTex,
    gUnusedBossKeyItemName6JPNTex,
    gUnusedBossKeyItemName7JPNTex,
    gBiggoronsSwordItemNameENGTex,
    gDekuStickItemNameGERTex,
    gDekuNutItemNameGERTex,
    gBombItemNameItemNameGERTex,
    gFairyBowItemNameGERTex,
    gFireArrowItemNameUnk2GERTex,
    gDinsFireItemNameGERTex,
    gFairySlingshotItemNameGERTex,
    gFairyOcarinaItemNameGERTex,
    gOcarinaOfTimeItemNameGERTex,
    gBombchuItemNameGERTex,
    gHookshotItemNameGERTex,
    gLongshotItemNameGERTex,
    gIceArrowItemNameGERTex,
    gFaroresWindItemNameGERTex,
    gBoomerangItemNameGERTex,
    gLensItemNameGERTex,
    gMagicBeansItemNameGERTex,
    gMegatonHammerItemNameGERTex,
    gLightArrowItemNameGERTex,
    gNayrusLoveItemNameGERTex,
    gEmptyBottleItemNameGERTex,
    gRedPotionItemNameGERTex,
    gGreenPotionItemNameGERTex,
    gBluePotionItemNameGERTex,
    gBottledFairyItemNameGERTex,
    gFishItemNameGERTex,
    gFullMilkItemNameGERTex,
    gRutosLetterItemNameGERTex,
    gBlueFireItemNameGERTex,
    gBugItemNameGERTex,
    gBigPoeItemNameGERTex,
    gHalfMilkItemNameGERTex,
    gPoeItemNameGERTex,
    gWeirdEggItemNameGERTex,
    gCuccoItemNameGERTex,
    gZeldasLetterItemNameGERTex,
    gKeatonMaskItemNameGERTex,
    gSkullMaskItemNameGERTex,
    gSpookyMaskItemNameGERTex,
    gBunnyHoodItemNameGERTex,
    gGoronMaskItemNameGERTex,
    gZoraMaskItemNameGERTex,
    gGerudoMaskItemNameGERTex,
    gMaskofTruthItemNameGERTex,
    gSOLDOUTItemNameGERTex,
    gPocketEggItemNameGERTex,
    gPocketCuccoItemNameGERTex,
    gCojiroItemNameGERTex,
    gOddMushroomItemNameGERTex,
    gOddPotionItemNameGERTex,
    gPoachersSawItemNameGERTex,
    gBrokenGoronsSwordItemNameGERTex,
    gPrescriptionItemNameGERTex,
    gEyeBallFrogItemNameGERTex,
    gEyeDropsItemNameGERTex,
    gClaimCheckItemNameGERTex,
    gUnusedWindMedallionItemName2JPNTex,
    gUnusedFireMedallionItemName2JPNTex,
    gUnusedIceMedallionItemName2JPNTex,
    gKokiriSwordItemNameGERTex,
    gMasterSwordItemNameGERTex,
    gGiantsKnifeItemNameGERTex,
    gDekuShieldItemNameGERTex,
    gHylianShieldItemNameGERTex,
    gMirrorShieldItemNameGERTex,
    gKokiriTunicItemNameGERTex,
    gGoronTunicItemNameGERTex,
    gZoraTunicItemNameGERTex,
    gKokiriBootsItemNameGERTex,
    gIronBootsItemNameGERTex,
    gHoverBootsItemNameGERTex,
    gBulletBag30ItemNameGERTex,
    gBulletBag40ItemNameGERTex,
    gBulletBag50ItemNameGERTex,
    gQuiver30ItemNameGERTex,
    gQuiver40ItemNameGERTex,
    gQuiver50ItemNameGERTex,
    gBombBag20ItemNameGERTex,
    gBombBag30ItemNameGERTex,
    gBombBag40ItemNameGERTex,
    gGoronsBraceletItemNameGERTex,
    gSilverGauntletsItemNameGERTex,
    gGoldenGauntletsItemNameGERTex,
    gSilverScaleItemNameGERTex,
    gGoldenScaleItemNameGERTex,
    gBrokenGiantsKnifeItemNameGERTex,
    gUnusedBossKeyItemName8JPNTex,
    gUnusedBossKeyItemName9JPNTex,
    gUnusedBossKeyItemName10JPNTex,
    gUnusedBossKeyItemName11JPNTex,
    gMinuetOfForestItemNameGERTex,
    gBoleroOfFireItemNameGERTex,
    gSerenadeOfWaterItemNameGERTex,
    gRequiemOfSpiritItemNameGERTex,
    gNocturneOfShadowItemNameGERTex,
    gPreludeOfLightItemNameGERTex,
    gZeldasLullabyItemNameGERTex,
    gEponasSongItemNameGERTex,
    gSariasSongItemNameGERTex,
    gSunsSongItemNameGERTex,
    gSongOfTimeItemNameGERTex,
    gSongOfStormsItemNameGERTex,
    gForestMedallionItemNameGERTex,
    gFireMedallionItemNameGERTex,
    gWaterMedallionItemNameGERTex,
    gSpiritMedallionItemNameGERTex,
    gShadowMedallionItemNameGERTex,
    gLightMedallionItemNameGERTex,
    gKokiriEmeraldItemNameGERTex,
    gGoronsRubyItemNameGERTex,
    gZorasSapphireItemNameGERTex,
    gStoneofAgonyItemNameGERTex,
    gGerudosCardItemNameGERTex,
    gGoldSkulltulaItemNameGERTex,
    gHeartContainerItemNameGERTex,
    gUnusedPieceOfHeartItemName2JPNTex,
    gBigKeyItemNameGERTex,
    gCompassItemNameGERTex,
    gDungeonMapItemNameGERTex,
    gUnusedBossKeyItemName12JPNTex,
    gUnusedBossKeyItemName13JPNTex,
    gUnusedBossKeyItemName14JPNTex,
    gBiggoronsSwordItemNameGERTex,
    gDekuStickItemNameFRATex,
    gDekuNutItemNameFRATex,
    gBombItemNameFRATex,
    gFairyBowItemNameFRATex,
    gFireArrowItemNameFRATex,
    gDinsFireItemNameFRATex,
    gFairySlingshotItemNameFRATex,
    gFairyOcarinaItemNameFRATex,
    gOcarinaOfTimeItemNameFRATex,
    gBombchuItemNameFRATex,
    gHookshotItemNameFRATex,
    gLongshotItemNameFRATex,
    gIceArrowItemNameFRATex,
    gFaroresWindItemNameFRATex,
    gBoomerangItemNameFRATex,
    gLensItemNameFRATex,
    gMagicBeansItemNameFRATex,
    gMegatonHammerItemNameFRATex,
    gLightArrowItemNameFRATex,
    gNayrusLoveItemNameFRATex,
    gEmptyBottleItemNameFRATex,
    gRedPotionItemNameFRATex,
    gGreenPotionItemNameFRATex,
    gBluePotionItemNameFRATex,
    gBottledFairyItemNameFRATex,
    gFishItemNameFRATex,
    gFullMilkItemNameFRATex,
    gRutosLetterItemNameFRATex,
    gBlueFireItemNameFRATex,
    gBugItemNameFRATex,
    gBigPoeItemNameFRATex,
    gHalfMilkItemNameFRATex,
    gPoeItemNameFRATex,
    gWeirdEggItemNameFRATex,
    gCuccoItemNameFRATex,
    gZeldasLetterItemNameFRATex,
    gKeatonMaskItemNameFRATex,
    gSkullMaskItemNameFRATex,
    gSpookyMaskItemNameFRATex,
    gBunnyHoodItemNameFRATex,
    gGoronMaskItemNameFRATex,
    gZoraMaskItemNameFRATex,
    gGerudoMaskItemNameFRATex,
    gMaskofTruthItemNameFRATex,
    gSOLDOUTItemNameFRATex,
    gPocketEggItemNameFRATex,
    gPocketCuccoItemNameFRATex,
    gCojiroItemNameFRATex,
    gOddMushroomItemNameFRATex,
    gOddPotionItemNameFRATex,
    gPoachersSawItemNameFRATex,
    gBrokenGoronsSwordItemNameFRATex,
    gPrescriptionItemNameFRATex,
    gEyeBallFrogItemNameFRATex,
    gEyeDropsItemNameFRATex,
    gClaimCheckItemNameFRATex,
    gUnusedWindMedallionItemName3JPNTex,
    gUnusedFireMedallionItemName3JPNTex,
    gUnusedIceMedallionItemName3JPNTex,
    gKokiriSwordItemNameFRATex,
    gMasterSwordItemNameFRATex,
    gGiantsKnifeItemNameFRATex,
    gDekuShieldItemNameFRATex,
    gHylianShieldItemNameFRATex,
    gMirrorShieldItemNameFRATex,
    gKokiriTunicItemNameFRATex,
    gGoronTunicItemNameFRATex,
    gZoraTunicItemNameFRATex,
    gKokiriBootsItemNameFRATex,
    gIronBootsItemNameFRATex,
    gHoverBootsItemNameFRATex,
    gBulletBag30ItemNameFRATex,
    gBulletBag40ItemNameFRATex,
    gBulletBag50ItemNameFRATex,
    gQuiver30ItemNameFRATex,
    gQuiver40ItemNameFRATex,
    gQuiver50ItemNameFRATex,
    gBombBag20ItemNameFRATex,
    gBombBag30ItemNameFRATex,
    gBombBag40ItemNameFRATex,
    gGoronsBraceletItemNameFRATex,
    gSilverGauntletsItemNameFRATex,
    gGoldenGauntletsItemNameFRATex,
    gSilverScaleItemNameFRATex,
    gGoldenScaleItemNameFRATex,
    gBrokenGiantsKnifeItemNameFRATex,
    gUnusedBossKeyItemName15JPNTex,
    gUnusedBossKeyItemName16JPNTex,
    gUnusedBossKeyItemName17JPNTex,
    gUnusedBossKeyItemName18JPNTex,
    gMinuetOfForestItemNameFRATex,
    gBoleroOfFireItemNameFRATex,
    gSerenadeOfWaterItemNameFRATex,
    gRequiemOfSpiritItemNameFRATex,
    gNocturneOfShadowItemNameFRATex,
    gPreludeOfLightItemNameFRATex,
    gZeldasLullabyItemNameFRATex,
    gEponasSongItemNameFRATex,
    gSariasSongItemNameFRATex,
    gSunsSongItemNameFRATex,
    gSongOfTimeItemNameFRATex,
    gSongOfStormsItemNameFRATex,
    gForestMedallionItemNameFRATex,
    gFireMedallionItemNameFRATex,
    gWaterMedallionItemNameFRATex,
    gSpiritMedallionItemNameFRATex,
    gShadowMedallionItemNameFRATex,
    gLightMedallionItemNameFRATex,
    gKokiriEmeraldItemNameFRATex,
    gGoronsRubyItemNameFRATex,
    gZorasSapphireItemNameFRATex,
    gStoneofAgonyItemNameFRATex,
    gGerudosCardItemNameFRATex,
    gGoldSkulltulaItemNameFRATex,
    gHeartContainerItemNameFRATex,
    gUnusedPieceOfHeartItemName3JPNTex,
    gBossKeyItemNameFRATex,
    gCompassItemNameFRATex,
    gDungeonMapItemNameFRATex,
    gUnusedBossKeyItemName19JPNTex,
    gUnusedBossKeyItemName20JPNTex,
    gUnusedBossKeyItemName21JPNTex,
    gBiggoronsSwordItemNameFRATex,
    gBiggoronsSwordItemNameFRATex,
};

static void* mapNameTextures[] =
{
    gHauntedWastelandPointNameENGTex,
    gGerudosFortressPointNameENGTex,
    gGerudoValleyPointNameENGTex,
    gHyliaLakesidePointNameENGTex,
    gLonLonRanchPointNameENGTex,
    gMarketPointNameENGTex,
    gHyruleFieldPointNameENGTex,
    gDeathMountainPointNameENGTex,
    gKakarikoVillagePointNameENGTex,
    gLostWoodsPointNameENGTex,
    gKokiriForestPointNameENGTex,
    gZorasDomainPointNameENGTex,
    gHauntedWastelandPointNameGERTex,
    gGerudosFortressPointNameGERTex,
    gGerudoValleyPointNameGERTex,
    gHyliaLakesidePointNameGERTex,
    gLonLonRanchPointNameGERTex,
    gMarketPointNameGERTex,
    gHyruleFieldPointNameGERTex,
    gDeathMountainPointNameGERTex,
    gKakarikoVillagePointNameGERTex,
    gLostWoodsPointNameGERTex,
    gKokiriForestPointNameGERTex,
    gZorasDomainPointNameGERTex,
    gHauntedWastelandPointNameFRATex,
    gGerudosFortressPointNameFRATex,
    gGerudoValleyPointNameFRATex,
    gHyliaLakesidePointNameFRATex,
    gLonLonRanchPointNameFRATex,
    gMarketPointNameFRATex,
    gHyruleFieldPointNameFRATex,
    gDeathMountainPointNameFRATex,
    gKakarikoVillagePointNameFRATex,
    gLostWoodsPointNameFRATex,
    gKokiriForestPointNameFRATex,
    gZorasDomainPointNameFRATex,
    gHyruleFieldPositionNameENGTex,
    gKakarikoVillagePositionNameENGTex,
    gGraveyardPositionNameENGTex,
    gZorasRiverPositionNameENGTex,
    gKokiriForestPositionNameENGTex,
    gSacredForestMeadowPositionNameENGTex,
    gLakeHyliaPositionNameENGTex,
    gZorasDomainPositionNameENGTex,
    gZorasFountainPositionNameENGTex,
    gGerudoValleyPositionNameENGTex,
    gLostWoodsPositionNameENGTex,
    gDesertColossusPositionNameENGTex,
    gGerudosFortressPositionNameENGTex,
    gHauntedWastelandPositionNameENGTex,
    gMarketPositionNameENGTex,
    gHyruleCastlePositionNameENGTex,
    gDeathMountainTrailPositionNameENGTex,
    gDeathMountainCraterPositionNameENGTex,
    gGoronCityPositionNameENGTex,
    gLonLonRanchPositionNameENGTex,
    gQuestionMarkPositionNameENGTex,
    gGanonsCastlePositionNameENGTex,
    gHyruleFieldPositionNameGERTex,
    gKakarikoVillagePositionNameGERTex,
    gGraveyardPositionNameGERTex,
    gZorasRiverPositionNameGERTex,
    gKokiriForestPositionNameGERTex,
    gSacredForestMeadowPositionNameGERTex,
    gLakeHyliaPositionNameGERTex,
    gZorasDomainPositionNameGERTex,
    gZorasFountainPositionNameGERTex,
    gGerudoValleyPositionNameGERTex,
    gLostWoodsPositionNameGERTex,
    gDesertColossusPositionNameGERTex,
    gGerudosFortressPositionNameGERTex,
    gHauntedWastelandPositionNameGERTex,
    gMarketPositionNameGERTex,
    gHyruleCastlePositionNameGERTex,
    gDeathMountainTrailPositionNameGERTex,
    gDeathMountainCraterPositionNameGERTex,
    gGoronCityPositionNameGERTex,
    gLonLonRanchPositionNameGERTex,
    gQuestionMarkPositionNameGERTex,
    gGanonsCastlePositionNameGERTex,
    gHyruleFieldPositionNameFRATex,
    gKakarikoVillagePositionNameFRATex,
    gGraveyardPositionNameFRATex,
    gZorasRiverPositionNameFRATex,
    gKokiriForestPositionNameFRATex,
    gSacredForestMeadowPositionNameFRATex,
    gLakeHyliaPositionNameFRATex,
    gZorasDomainPositionNameFRATex,
    gZorasFountainPositionNameFRATex,
    gGerudoValleyPositionNameFRATex,
    gLostWoodsPositionNameFRATex,
    gDesertColossusPositionNameFRATex,
    gGerudosFortressPositionNameFRATex,
    gHauntedWastelandPositionNameFRATex,
    gMarketPositionNameFRATex,
    gHyruleCastlePositionNameFRATex,
    gDeathMountainTrailPositionNameFRATex,
    gDeathMountainCraterPositionNameFRATex,
    gGoronCityPositionNameFRATex,
    gLonLonRanchPositionNameFRATex,
    gQuestionMarkPositionNameFRATex,
    gGanonsCastlePositionNameFRATex,
};

s16 D_8082AAEC[] = {
    32, 112, 32, 48, 32, 32, 32, 48, 32, 64, 32, 48, 48, 48, 48, 64, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 80, 64,
};

s16 D_8082AB2C[] = {
    24, 72, 13, 22, 19, 20, 19, 27, 14, 26, 22, 21, 49, 32, 45, 60, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 16, 32, 8,
};

static u8 D_8082AB6C[][ARRAY_COUNT(gSaveContext.buttonStatus)] = {
    { BTN_ENABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_ENABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED },
    { BTN_ENABLED, BTN_ENABLED, BTN_ENABLED, BTN_ENABLED, BTN_DISABLED, BTN_ENABLED, BTN_ENABLED, BTN_ENABLED, BTN_ENABLED },
    { BTN_ENABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED },
    { BTN_ENABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_ENABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED },
    { BTN_ENABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_ENABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED, BTN_DISABLED },
    { BTN_ENABLED, BTN_ENABLED, BTN_ENABLED, BTN_ENABLED, BTN_DISABLED, BTN_ENABLED, BTN_ENABLED, BTN_ENABLED, BTN_ENABLED },
};

static s16 D_8082AB8C = 0;
static s16 D_8082AB90 = 0;
static s16 D_8082AB94 = 0;
static s16 D_8082AB98 = 255;

static s16 D_8082AB9C = 255;
static s16 D_8082ABA0 = 0;
static s16 D_8082ABA4 = 0;

static s16 sInDungeonScene = false;

static f32 D_8082ABAC[] = {
    -4.0f, 4.0f, 4.0f, 4.0f, 4.0f, -4.0f, -4.0f, -4.0f,
};

static f32 D_8082ABCC[] = {
    -4.0f, -4.0f, -4.0f, 4.0f, 4.0f, 4.0f, 4.0f, -4.0f,
};

static u16 D_8082ABEC[] = {
    PAUSE_MAP, PAUSE_EQUIP, PAUSE_QUEST, PAUSE_ITEM, PAUSE_EQUIP, PAUSE_MAP, PAUSE_ITEM, PAUSE_QUEST,
};

u8 gSlotAgeReqs[] = {
    AGE_REQ_CHILD,  // SLOT_DEKU_STICK
    AGE_REQ_NONE,   // SLOT_DEKU_NUT
    AGE_REQ_NONE,   // SLOT_BOMB
    AGE_REQ_ADULT,  // SLOT_BOW
    AGE_REQ_ADULT,  // SLOT_ARROW_FIRE
    AGE_REQ_NONE,   // SLOT_DINS_FIRE
    AGE_REQ_CHILD,  // SLOT_SLINGSHOT
    AGE_REQ_NONE,   // SLOT_OCARINA
    AGE_REQ_NONE,   // SLOT_BOMBCHU
    AGE_REQ_ADULT,  // SLOT_HOOKSHOT
    AGE_REQ_ADULT,  // SLOT_ARROW_ICE
    AGE_REQ_NONE,   // SLOT_FARORES_WIND
    AGE_REQ_CHILD,  // SLOT_BOOMERANG
    AGE_REQ_NONE,   // SLOT_LENS_OF_TRUTH
    AGE_REQ_CHILD,  // SLOT_MAGIC_BEAN
    AGE_REQ_ADULT,  // SLOT_HAMMER
    AGE_REQ_ADULT,  // SLOT_ARROW_LIGHT
    AGE_REQ_NONE,   // SLOT_NAYRUS_LOVE
    AGE_REQ_NONE,   // SLOT_BOTTLE_1
    AGE_REQ_NONE,   // SLOT_BOTTLE_2
    AGE_REQ_NONE,   // SLOT_BOTTLE_3
    AGE_REQ_NONE,   // SLOT_BOTTLE_4
    AGE_REQ_ADULT,  // SLOT_TRADE_ADULT
    AGE_REQ_CHILD,  // SLOT_TRADE_CHILD
};

u8 gEquipAgeReqs[][4] = {
    {
        AGE_REQ_ADULT,  // 0 UPG_QUIVER
        AGE_REQ_CHILD,  // EQUIP_TYPE_SWORD EQUIP_VALUE_SWORD_KOKIRI
        AGE_REQ_ADULT,  // EQUIP_TYPE_SWORD EQUIP_VALUE_SWORD_MASTER
        AGE_REQ_ADULT   // EQUIP_TYPE_SWORD EQUIP_VALUE_SWORD_BIGGORON
    },
    {
        AGE_REQ_NONE,   // 0 UPG_BOMB_BAG
        AGE_REQ_CHILD,  // EQUIP_TYPE_SHIELD EQUIP_VALUE_SHIELD_DEKU
        AGE_REQ_NONE,   // EQUIP_TYPE_SHIELD EQUIP_VALUE_SHIELD_HYLIAN
        AGE_REQ_ADULT   // EQUIP_TYPE_SHIELD EQUIP_VALUE_SHIELD_MIRROR
    },
    {
        AGE_REQ_ADULT,  // 0 UPG_STRENGTH
        AGE_REQ_NONE,   // EQUIP_TYPE_TUNIC EQUIP_VALUE_TUNIC_KOKIRI
        AGE_REQ_ADULT,  // EQUIP_TYPE_TUNIC EQUIP_VALUE_TUNIC_GORON
        AGE_REQ_ADULT   // EQUIP_TYPE_TUNIC EQUIP_VALUE_TUNIC_ZORA
    },
    {
        AGE_REQ_NONE,   // 0 UPG_SCALE
        AGE_REQ_NONE,   // EQUIP_TYPE_BOOTS EQUIP_VALUE_BOOTS_KOKIRI
        AGE_REQ_ADULT,  // EQUIP_TYPE_BOOTS EQUIP_VALUE_BOOTS_IRON
        AGE_REQ_ADULT   // EQUIP_TYPE_BOOTS EQUIP_VALUE_BOOTS_HOVER
    },

};

u8 gItemAgeReqs[] = {
    AGE_REQ_CHILD,  // ITEM_DEKU_STICK
    AGE_REQ_NONE,   // ITEM_DEKU_NUT
    AGE_REQ_NONE,   // ITEM_BOMB
    AGE_REQ_ADULT,  // ITEM_BOW
    AGE_REQ_ADULT,  // ITEM_ARROW_FIRE
    AGE_REQ_NONE,   // ITEM_DINS_FIRE
    AGE_REQ_CHILD,  // ITEM_SLINGSHOT
    AGE_REQ_NONE,   // ITEM_OCARINA_FAIRY
    AGE_REQ_NONE,   // ITEM_OCARINA_OF_TIME
    AGE_REQ_NONE,   // ITEM_BOMBCHU
    AGE_REQ_ADULT,  // ITEM_HOOKSHOT
    AGE_REQ_ADULT,  // ITEM_LONGSHOT
    AGE_REQ_ADULT,  // ITEM_ARROW_ICE
    AGE_REQ_NONE,   // ITEM_FARORES_WIND
    AGE_REQ_CHILD,  // ITEM_BOOMERANG
    AGE_REQ_NONE,   // ITEM_LENS_OF_TRUTH
    AGE_REQ_CHILD,  // ITEM_MAGIC_BEAN
    AGE_REQ_ADULT,  // ITEM_HAMMER
    AGE_REQ_ADULT,  // ITEM_ARROW_LIGHT
    AGE_REQ_NONE,   // ITEM_NAYRUS_LOVE
    AGE_REQ_NONE,   // ITEM_BOTTLE_EMPTY
    AGE_REQ_NONE,   // ITEM_BOTTLE_POTION_RED
    AGE_REQ_NONE,   // ITEM_BOTTLE_POTION_GREEN
    AGE_REQ_NONE,   // ITEM_BOTTLE_POTION_BLUE
    AGE_REQ_NONE,   // ITEM_BOTTLE_FAIRY
    AGE_REQ_NONE,   // ITEM_BOTTLE_FISH
    AGE_REQ_NONE,   // ITEM_BOTTLE_MILK_FULL
    AGE_REQ_NONE,   // ITEM_BOTTLE_RUTOS_LETTER
    AGE_REQ_NONE,   // ITEM_BOTTLE_BLUE_FIRE
    AGE_REQ_NONE,   // ITEM_BOTTLE_BUG
    AGE_REQ_NONE,   // ITEM_BOTTLE_BIG_POE
    AGE_REQ_NONE,   // ITEM_BOTTLE_MILK_HALF
    AGE_REQ_NONE,   // ITEM_BOTTLE_POE
    AGE_REQ_CHILD,  // ITEM_WEIRD_EGG
    AGE_REQ_CHILD,  // ITEM_CHICKEN
    AGE_REQ_CHILD,  // ITEM_ZELDAS_LETTER
    AGE_REQ_CHILD,  // ITEM_MASK_KEATON
    AGE_REQ_CHILD,  // ITEM_MASK_SKULL
    AGE_REQ_CHILD,  // ITEM_MASK_SPOOKY
    AGE_REQ_CHILD,  // ITEM_MASK_BUNNY_HOOD
    AGE_REQ_CHILD,  // ITEM_MASK_GORON
    AGE_REQ_CHILD,  // ITEM_MASK_ZORA
    AGE_REQ_CHILD,  // ITEM_MASK_GERUDO
    AGE_REQ_CHILD,  // ITEM_MASK_TRUTH
    AGE_REQ_CHILD,  // ITEM_SOLD_OUT
    AGE_REQ_ADULT,  // ITEM_POCKET_EGG
    AGE_REQ_ADULT,  // ITEM_POCKET_CUCCO
    AGE_REQ_ADULT,  // ITEM_COJIRO
    AGE_REQ_ADULT,  // ITEM_ODD_MUSHROOM
    AGE_REQ_ADULT,  // ITEM_ODD_POTION
    AGE_REQ_ADULT,  // ITEM_POACHERS_SAW
    AGE_REQ_ADULT,  // ITEM_BROKEN_GORONS_SWORD
    AGE_REQ_ADULT,  // ITEM_PRESCRIPTION
    AGE_REQ_ADULT,  // ITEM_EYEBALL_FROG
    AGE_REQ_ADULT,  // ITEM_EYE_DROPS
    AGE_REQ_ADULT,  // ITEM_CLAIM_CHECK
    AGE_REQ_ADULT,  // ITEM_BOW_FIRE
    AGE_REQ_ADULT,  // ITEM_BOW_ICE
    AGE_REQ_ADULT,  // ITEM_BOW_LIGHT
    AGE_REQ_CHILD,  // ITEM_SWORD_KOKIRI
    AGE_REQ_ADULT,  // ITEM_SWORD_MASTER
    AGE_REQ_ADULT,  // ITEM_SWORD_BIGGORON
    AGE_REQ_CHILD,  // ITEM_SHIELD_DEKU
    AGE_REQ_NONE,   // ITEM_SHIELD_HYLIAN
    AGE_REQ_ADULT,  // ITEM_SHIELD_MIRROR
    AGE_REQ_NONE,   // ITEM_TUNIC_KOKIRI
    AGE_REQ_ADULT,  // ITEM_TUNIC_GORON
    AGE_REQ_ADULT,  // ITEM_TUNIC_ZORA
    AGE_REQ_NONE,   // ITEM_BOOTS_KOKIRI
    AGE_REQ_ADULT,  // ITEM_BOOTS_IRON
    AGE_REQ_ADULT,  // ITEM_BOOTS_HOVER
    AGE_REQ_CHILD,  // ITEM_BULLET_BAG_30
    AGE_REQ_CHILD,  // ITEM_BULLET_BAG_40
    AGE_REQ_CHILD,  // ITEM_BULLET_BAG_50
    AGE_REQ_ADULT,  // ITEM_QUIVER_30
    AGE_REQ_ADULT,  // ITEM_QUIVER_40
    AGE_REQ_ADULT,  // ITEM_QUIVER_50
    AGE_REQ_NONE,   // ITEM_BOMB_BAG_20
    AGE_REQ_NONE,   // ITEM_BOMB_BAG_30
    AGE_REQ_NONE,   // ITEM_BOMB_BAG_40
    AGE_REQ_CHILD,  // ITEM_STRENGTH_GORONS_BRACELET
    AGE_REQ_ADULT,  // ITEM_STRENGTH_SILVER_GAUNTLETS
    AGE_REQ_ADULT,  // ITEM_STRENGTH_GOLD_GAUNTLETS
    AGE_REQ_NONE,   // ITEM_SCALE_SILVER
    AGE_REQ_NONE,   // ITEM_SCALE_GOLDEN
    AGE_REQ_ADULT,  // ITEM_GIANTS_KNIFE
};

u8 gAreaGsFlags[] = {
    0x0F, 0x1F, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x07, 0x07, 0x03,
    0x0F, 0x07, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0x1F, 0x0F, 0x03, 0x0F,
};

static void* sCursorTexs[] = {
    gPauseMenuCursorTopLeftTex,
    gPauseMenuCursorTopRightTex,
    gPauseMenuCursorBottomLeftTex,
    gPauseMenuCursorBottomRightTex,
};

static Color_RGB8 sCursorColors[3] = {
    { 255, 255, 255 },
    { 255, 255, 0 },
    { 0, 50, 255 },
};

const Color_RGB8 Cursor_ABTN_ori = {0, 255, 50};
const Color_RGB8 Cursor_ABTNN64_ori = {0, 50, 255};
const Color_RGB8 Cursor_CBTN_ori = {255, 255, 0};
const Color_RGB8 Cursor_StartBTN_ori = {255, 255, 255};
Color_RGB8 Cursor_ABTN;
Color_RGB8 Cursor_CBTN;

static void* sSavePromptTexs[] = {
    gPauseSavePromptENGTex,
    gPauseSavePromptGERTex,
    gPauseSavePromptFRATex,
};

static void* sSaveConfirmationTexs[] = {
    gPauseSaveConfirmationENGTex,
    gPauseSaveConfirmationGERTex,
    gPauseSaveConfirmationFRATex,
};

static void* sContinuePromptTexs[] = {
    gContinuePlayingENGTex,
    gContinuePlayingGERTex,
    gContinuePlayingFRATex,
};

static void* sPromptChoiceTexs[][2] = {
    { gPauseYesENGTex, gPauseNoENGTex },
    { gPauseYesGERTex, gPauseNoGERTex },
    { gPauseYesFRATex, gPauseNoFRATex },
};

static u8 sButtonStatusSave[ARRAY_COUNT(gSaveContext.buttonStatus)];
static PreRender sPlayerPreRender;
static void* sPreRenderCvg;
extern int fbTest;

void KaleidoScope_ProcessPlayerPreRender(void) {
    PreRender_Calc(&sPlayerPreRender);
    PreRender_Destroy(&sPlayerPreRender);
}

Gfx* KaleidoScope_QuadTextureIA4(Gfx* gfx, void* texture, s16 width, s16 height, u16 point) {
    gDPLoadTextureBlock_4b(gfx++, texture, G_IM_FMT_IA, width, height, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(gfx++, point, point + 2, point + 3, point + 1, 0);

    return gfx;
}

Gfx* KaleidoScope_QuadTextureIA8(Gfx* gfx, void* texture, s16 width, s16 height, u16 point) {
    u8 mirrorMode = CVarGetInteger("gMirroredWorld", 0) ? G_TX_MIRROR : G_TX_NOMIRROR;
    gDPLoadTextureBlock(gfx++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, width, height, 0, mirrorMode | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(gfx++, point, point + 2, point + 3, point + 1, 0);

    return gfx;
}

void KaleidoScope_OverridePalIndexCI4(u8* texture, s32 size, s32 targetIndex, s32 newIndex) {
    s32 i;

    targetIndex &= 0xF;
    newIndex &= 0xF;

    if ((size == 0) || (targetIndex == newIndex) || (texture == NULL)) {
        return;
    }

    for (i = 0; i < size; i++) {
        s32 index1;
        s32 index2;

        index1 = index2 = texture[i];

        index1 = (index1 >> 4) & 0xF;
        index2 = index2 & 0xF;

        if (index1 == targetIndex) {
            index1 = newIndex;
        }

        if (index2 == targetIndex) {
            index2 = newIndex;
        }

        texture[i] = (index1 << 4) | index2;
    }
}

void KaleidoScope_MoveCursorToSpecialPos(PlayState* play, u16 specialPos) {
    PauseContext* pauseCtx = &play->pauseCtx;

    pauseCtx->cursorSpecialPos = specialPos;
    pauseCtx->pageSwitchTimer = 0;

    Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

void KaleidoScope_DrawQuadTextureRGBA32(GraphicsContext* gfxCtx, void* texture, u16 width, u16 height, u16 point) {
    OPEN_DISPS(gfxCtx);

    gDPLoadTextureBlock(POLY_KAL_DISP++, texture, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, height, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_KAL_DISP++, point, point + 2, point + 3, point + 1, 0);

    CLOSE_DISPS(gfxCtx);
}

void KaleidoScope_SetDefaultCursor(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 s;
    s16 i;
    gSelectingMask = false;

    switch (pauseCtx->pageIndex) {
        case PAUSE_ITEM:
            s = pauseCtx->cursorSlot[PAUSE_ITEM];
            if (gSaveContext.inventory.items[s] == ITEM_NONE) {
                i = s + 1;
                while (true) {
                    if (gSaveContext.inventory.items[i] != ITEM_NONE) {
                        break;
                    }
                    i++;
                    if (i >= 24) {
                        i = 0;
                    }
                    if (i == s) {
                        pauseCtx->cursorItem[PAUSE_ITEM] = pauseCtx->namedItem = PAUSE_ITEM_NONE;
                        return;
                    }
                }
                pauseCtx->cursorItem[PAUSE_ITEM] = gSaveContext.inventory.items[i];
                pauseCtx->cursorSlot[PAUSE_ITEM] = i;
            }
            break;
        case PAUSE_MAP:
        case PAUSE_QUEST:
        case PAUSE_EQUIP:
            break;
    }
}

void KaleidoScope_SwitchPage(PauseContext* pauseCtx, u8 pt) {
    pauseCtx->unk_1E4 = 1;
    pauseCtx->unk_1EA = 0;
    gSelectingMask = false;

    if (!pt) {
        pauseCtx->mode = pauseCtx->pageIndex * 2 + 1;
        Audio_PlaySoundGeneral(NA_SE_SY_WIN_SCROLL_LEFT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        pauseCtx->cursorSpecialPos = PAUSE_CURSOR_PAGE_RIGHT;
    } else {
        pauseCtx->mode = pauseCtx->pageIndex * 2;
        Audio_PlaySoundGeneral(NA_SE_SY_WIN_SCROLL_RIGHT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        pauseCtx->cursorSpecialPos = PAUSE_CURSOR_PAGE_LEFT;
    }

    for (int buttonIndex = 1; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
        gSaveContext.buttonStatus[buttonIndex] = D_8082AB6C[pauseCtx->pageIndex + pt][buttonIndex];
    }

    if ((CVarGetInteger("gAssignableTunicsAndBoots", 0) != 0) && (D_8082ABEC[pauseCtx->mode] == PAUSE_EQUIP)) {
        gSaveContext.buttonStatus[1] = BTN_ENABLED;
        gSaveContext.buttonStatus[2] = BTN_ENABLED;
        gSaveContext.buttonStatus[3] = BTN_ENABLED;
        // A-Button is already enabled
        gSaveContext.buttonStatus[5] = BTN_ENABLED;
        gSaveContext.buttonStatus[6] = BTN_ENABLED;
        gSaveContext.buttonStatus[7] = BTN_ENABLED;
        gSaveContext.buttonStatus[8] = BTN_ENABLED;
    }

    osSyncPrintf("kscope->kscp_pos+pt = %d\n", pauseCtx->pageIndex + pt);

    gSaveContext.unk_13EA = 0;
    Interface_ChangeAlpha(50);

    KaleidoScope_ResetTradeSelect();
}

void KaleidoScope_HandlePageToggles(PauseContext* pauseCtx, Input* input) {
    s16 Debug_BTN = BTN_L;
    s16 PageLeft_BTN = BTN_Z;
    if (CVarGetInteger("gNGCKaleidoSwitcher", 0) != 0) {
        Debug_BTN = BTN_Z;
        PageLeft_BTN = BTN_L;
    }

    if (CVarGetInteger("gDebugEnabled", 0) && (pauseCtx->debugState == 0) && CHECK_BTN_ALL(input->press.button, Debug_BTN)) {
        pauseCtx->debugState = 1;
        return;
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_R)) {
        KaleidoScope_SwitchPage(pauseCtx, 2);
        return;
    }

    if (CHECK_BTN_ALL(input->press.button, PageLeft_BTN)) {
        KaleidoScope_SwitchPage(pauseCtx, 0);
        return;
    }

    bool dpad = CVarGetInteger("gDpadPause", 0);
    if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
        if ((pauseCtx->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->cur.button, BTN_DLEFT))) {
            pauseCtx->pageSwitchTimer++;
            if ((pauseCtx->pageSwitchTimer >= 10) || (pauseCtx->pageSwitchTimer == 0)) {
                KaleidoScope_SwitchPage(pauseCtx, 0);
            }
        } else {
            pauseCtx->pageSwitchTimer = -1;
        }
    } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) {
        if ((pauseCtx->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT))) {
            pauseCtx->pageSwitchTimer++;
            if ((pauseCtx->pageSwitchTimer >= 10) || (pauseCtx->pageSwitchTimer == 0)) {
                KaleidoScope_SwitchPage(pauseCtx, 2);
            }
        } else {
            pauseCtx->pageSwitchTimer = -1;
        }
    }
}

void KaleidoScope_DrawCursor(PlayState* play, u16 pageIndex) {
    PauseContext* pauseCtx = &play->pauseCtx;
    u16 temp;

    OPEN_DISPS(play->state.gfxCtx);

    temp = pauseCtx->unk_1E4;

    if (CVarGetInteger("gCosmetics.Hud_AButton.Changed", 0)) {
        sCursorColors[2] = CVarGetColor24("gCosmetics.Hud_AButton.Value", sCursorColors[2]);
    } else if (CVarGetInteger("gCosmetics.DefaultColorScheme", COLORSCHEME_N64) == COLORSCHEME_GAMECUBE) {
        sCursorColors[2] = (Color_RGB8){ 0, 255, 50 };
    }

    if (CVarGetInteger("gCosmetics.Hud_CButtons.Changed", 0)) {
        sCursorColors[1] = CVarGetColor24("gCosmetics.Hud_CButtons.Value", sCursorColors[1]);
    }

    if ((((pauseCtx->unk_1E4 == 0) || (temp == 8)) && (pauseCtx->state == 6)) ||
        ((pauseCtx->pageIndex == PAUSE_QUEST) && ((temp < 3) || (temp == 5) || (temp == 8)))) {

        if (pauseCtx->pageIndex == pageIndex) {
            s16 i;
            s16 j;

            gDPPipeSync(POLY_KAL_DISP++);
            gDPSetCombineLERP(POLY_KAL_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, sCursorColors[pauseCtx->cursorColorSet >> 2].r,
                            sCursorColors[pauseCtx->cursorColorSet >> 2].g,
                            sCursorColors[pauseCtx->cursorColorSet >> 2].b, 255);
            gDPSetEnvColor(POLY_KAL_DISP++, D_8082AB8C, D_8082AB90, D_8082AB94, 255);
            gSPVertex(POLY_KAL_DISP++, pauseCtx->cursorVtx, 16, 0);

            for (i = j = 0; i < 4; i++, j += 4) {
                gDPLoadTextureBlock_4b(POLY_KAL_DISP++, sCursorTexs[i], G_IM_FMT_IA, 16, 16, 0,
                                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                       G_TX_NOLOD, G_TX_NOLOD);
                gSP1Quadrangle(POLY_KAL_DISP++, j, j + 2, j + 3, j + 1, 0);
            }
        }

        gDPPipeSync(POLY_KAL_DISP++);
        gDPSetEnvColor(POLY_KAL_DISP++, 0, 0, 0, 255);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

Gfx* KaleidoScope_DrawPageSections(Gfx* gfx, Vtx* vertices, void** textures) {
    s32 i;
    s32 j;

    gSPVertex(gfx++, vertices, 32, 0);

    i = 0;
    j = 0;
    while (j < 32) {
        gDPPipeSync(gfx++);
        gDPLoadTextureBlock(gfx++, textures[i], G_IM_FMT_IA, G_IM_SIZ_8b, 80, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(gfx++, j, j + 2, j + 3, j + 1, 0);

        j += 4;
        i++;
    }

    gSPVertex(gfx++, vertices + 32, 28, 0);

    j = 0;
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

void KaleidoScope_DrawPages(PlayState* play, GraphicsContext* gfxCtx) {
    static Color_RGB8 D_8082ACF4[12] = {
        { 0, 0, 0 }, { 0, 0, 0 },     { 0, 0, 0 },    { 0, 0, 0 }, { 255, 255, 0 }, { 0, 0, 0 },
        { 0, 0, 0 }, { 255, 255, 0 }, { 0, 50, 255 }, { 0, 0, 0 }, { 0, 0, 0 },     { 0, 50, 255 },
    };
    Color_RGB8 aButtonColor = { 100, 100, 255 };
    if (CVarGetInteger("gCosmetics.Hud_AButton.Changed", 0)) {
        aButtonColor = CVarGetColor24("gCosmetics.Hud_AButton.Value", aButtonColor);
        D_8082ACF4[8] = CVarGetColor24("gCosmetics.Hud_AButton.Value", D_8082ACF4[8]);
        D_8082ACF4[11] = CVarGetColor24("gCosmetics.Hud_AButton.Value", D_8082ACF4[11]);
    } else if (CVarGetInteger("gCosmetics.DefaultColorScheme", COLORSCHEME_N64) == COLORSCHEME_GAMECUBE) {
        aButtonColor = (Color_RGB8){ 100, 255, 100 };
        D_8082ACF4[8] = (Color_RGB8){ 0, 255, 50 };
        D_8082ACF4[11] = (Color_RGB8){ 0, 255, 50 };
    }

    if (CVarGetInteger("gCosmetics.Hud_CButtons.Changed", 0)) {
        D_8082ACF4[4] = CVarGetColor24("gCosmetics.Hud_CButtons.Value", D_8082ACF4[4]);
        D_8082ACF4[7] = CVarGetColor24("gCosmetics.Hud_CButtons.Value", D_8082ACF4[7]);
    }

    static s16 D_8082AD3C = 20;
    static s16 D_8082AD40 = 0;
    static s16 D_8082AD44 = 0;
    static s16 D_8082AD48 = 0;
    static s16 D_8082AD4C = 0;
    static s16 D_8082AD50 = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = &play->state.input[0];
    s16 stepR;
    s16 stepG;
    s16 stepB;

    FrameInterpolation_RecordOpenChild(NULL, pauseCtx->state + pauseCtx->pageIndex * 100);
    OPEN_DISPS(gfxCtx);

    if ((pauseCtx->state < 8) || (pauseCtx->state > 0x11)) {
        if (pauseCtx->state != 7) {
            stepR = ABS(D_8082AB8C - D_8082ACF4[pauseCtx->cursorColorSet + D_8082AD40].r) / D_8082AD3C;
            stepG = ABS(D_8082AB90 - D_8082ACF4[pauseCtx->cursorColorSet + D_8082AD40].g) / D_8082AD3C;
            stepB = ABS(D_8082AB94 - D_8082ACF4[pauseCtx->cursorColorSet + D_8082AD40].b) / D_8082AD3C;
            if (D_8082AB8C >= D_8082ACF4[pauseCtx->cursorColorSet + D_8082AD40].r) {
                D_8082AB8C -= stepR;
            } else {
                D_8082AB8C += stepR;
            }
            if (D_8082AB90 >= D_8082ACF4[pauseCtx->cursorColorSet + D_8082AD40].g) {
                D_8082AB90 -= stepG;
            } else {
                D_8082AB90 += stepG;
            }
            if (D_8082AB94 >= D_8082ACF4[pauseCtx->cursorColorSet + D_8082AD40].b) {
                D_8082AB94 -= stepB;
            } else {
                D_8082AB94 += stepB;
            }

            D_8082AD3C--;
            if (D_8082AD3C == 0) {
                D_8082AB8C = D_8082ACF4[pauseCtx->cursorColorSet + D_8082AD40].r;
                D_8082AB90 = D_8082ACF4[pauseCtx->cursorColorSet + D_8082AD40].g;
                D_8082AB94 = D_8082ACF4[pauseCtx->cursorColorSet + D_8082AD40].b;
                D_8082AD3C = ZREG(28 + D_8082AD40);
                D_8082AD40++;
                if (D_8082AD40 >= 4) {
                    D_8082AD40 = 0;
                }
            }

            if (CVarGetInteger("gDpadHoldChange", 1) && CVarGetInteger("gDpadPause", 0)) {
                if (CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
                        D_8082AD44 = XREG(8);
                        D_8082AD4C = -1;
                    } else if (--D_8082AD44 < 0) {
                        D_8082AD44 = XREG(6);
                        input->press.button |= BTN_DLEFT;
                    }
                } else if (CHECK_BTN_ALL(input->rel.button, BTN_DLEFT)) {
                    D_8082AD4C = 0;
                } else if (CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
                        D_8082AD44 = XREG(8);
                        D_8082AD4C = 1;
                    } else if (--D_8082AD44 < 0) {
                        D_8082AD44 = XREG(6);
                        input->press.button |= BTN_DRIGHT;
                    }
                } else if (CHECK_BTN_ALL(input->rel.button, BTN_DRIGHT)) {
                    D_8082AD4C = 0;
                }

                if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
                        D_8082AD48 = XREG(8);
                        D_8082AD50 = -1;
                    } else if (--D_8082AD48 < 0) {
                        D_8082AD48 = XREG(6);
                        input->press.button |= BTN_DDOWN;
                    }
                } else if (CHECK_BTN_ALL(input->rel.button, BTN_DDOWN)) {
                    D_8082AD50 = 0;
                } else if (CHECK_BTN_ALL(input->cur.button, BTN_DUP)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
                        D_8082AD48 = XREG(8);
                        D_8082AD50 = 1;
                    } else if (--D_8082AD48 < 0) {
                        D_8082AD48 = XREG(6);
                        input->press.button |= BTN_DUP;
                    }
                } else if (CHECK_BTN_ALL(input->rel.button, BTN_DUP)) {
                    D_8082AD50 = 0;
                }
            }

            if (pauseCtx->stickRelX < -30) {
                if (D_8082AD4C == -1) {
                    if (--D_8082AD44 < 0) {
                        D_8082AD44 = XREG(6);
                    } else {
                        pauseCtx->stickRelX = 0;
                    }
                } else {
                    D_8082AD44 = XREG(8);
                    D_8082AD4C = -1;
                }
            } else if (pauseCtx->stickRelX > 30) {
                if (D_8082AD4C == 1) {
                    if (--D_8082AD44 < 0) {
                        D_8082AD44 = XREG(6);
                    } else {
                        pauseCtx->stickRelX = 0;
                    }
                } else {
                    D_8082AD44 = XREG(8);
                    D_8082AD4C = 1;
                }
            } else {
                D_8082AD4C = 0;
            }

            if (pauseCtx->stickRelY < -30) {
                if (D_8082AD50 == -1) {
                    if (--D_8082AD48 < 0) {
                        D_8082AD48 = XREG(6);
                    } else {
                        pauseCtx->stickRelY = 0;
                    }
                } else {
                    D_8082AD48 = XREG(8);
                    D_8082AD50 = -1;
                }
            } else if (pauseCtx->stickRelY > 30) {
                if (D_8082AD50 == 1) {
                    if (--D_8082AD48 < 0) {
                        D_8082AD48 = XREG(6);
                    } else {
                        pauseCtx->stickRelY = 0;
                    }
                } else {
                    D_8082AD48 = XREG(8);
                    D_8082AD50 = 1;
                }
            } else {
                D_8082AD50 = 0;
            }
        }

        if (pauseCtx->pageIndex) { // pageIndex != PAUSE_ITEM
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);

            Matrix_Translate(0.0f, (f32)WREG(2) / 100.0f, -(f32)WREG(3) / 100.0f, MTXMODE_NEW);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateX(-pauseCtx->unk_1F4 / 100.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            POLY_KAL_DISP = KaleidoScope_DrawPageSections(POLY_KAL_DISP, pauseCtx->itemPageVtx,
                                                          sSelectItemTexs[gSaveContext.language]);

            KaleidoScope_DrawItemSelect(play);
        }

        if (pauseCtx->pageIndex != PAUSE_EQUIP) {
            gDPPipeSync(POLY_KAL_DISP++);
            gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);

            Matrix_Translate(-(f32)WREG(3) / 100.0f, (f32)WREG(2) / 100.0f, 0.0f, MTXMODE_NEW);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateZ(pauseCtx->unk_1F8 / 100.0f, MTXMODE_APPLY);
            Matrix_RotateY(1.57f, MTXMODE_APPLY);

            gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            POLY_KAL_DISP = KaleidoScope_DrawPageSections(POLY_KAL_DISP, pauseCtx->equipPageVtx,
                                                          sEquipmentTexs[gSaveContext.language]);

            KaleidoScope_DrawEquipment(play);
        }

        if (pauseCtx->pageIndex != PAUSE_QUEST) {
            gDPPipeSync(POLY_KAL_DISP++);
            gDPSetTextureFilter(POLY_KAL_DISP++, G_TF_BILERP);
            gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);

            Matrix_Translate(0.0f, (f32)WREG(2) / 100.0f, (f32)WREG(3) / 100.0f, MTXMODE_NEW);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateX(pauseCtx->unk_200 / 100.0f, MTXMODE_APPLY);
            Matrix_RotateY(3.14f, MTXMODE_APPLY);

            gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            POLY_KAL_DISP = KaleidoScope_DrawPageSections(POLY_KAL_DISP, pauseCtx->questPageVtx,
                                                          sQuestStatusTexs[gSaveContext.language]);

            KaleidoScope_DrawQuestStatus(play, gfxCtx);
        }

        if (pauseCtx->pageIndex != PAUSE_MAP) {
            gDPPipeSync(POLY_KAL_DISP++);

            gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);

            Matrix_Translate((f32)WREG(3) / 100.0f, (f32)WREG(2) / 100.0f, 0.0f, MTXMODE_NEW);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateZ(-pauseCtx->unk_1FC / 100.0f, MTXMODE_APPLY);
            Matrix_RotateY(-1.57f, MTXMODE_APPLY);

            gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            POLY_KAL_DISP =
                KaleidoScope_DrawPageSections(POLY_KAL_DISP, pauseCtx->mapPageVtx, sMapTexs[gSaveContext.language]);

            if (sInDungeonScene) {
                KaleidoScope_DrawDungeonMap(play, gfxCtx);
                Gfx_SetupDL_42Opa(gfxCtx);

                gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

                if (CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, gSaveContext.mapIndex)) {
                    PauseMapMark_Draw(play);
                }
            } else {
                KaleidoScope_DrawWorldMap(play, gfxCtx);
            }
        }

        gDPPipeSync(POLY_KAL_DISP++);
        gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);

        switch (pauseCtx->pageIndex) {
            case PAUSE_ITEM:
                Matrix_Translate(0.0f, (f32)WREG(2) / 100.0f, -(f32)WREG(3) / 100.0f, MTXMODE_NEW);
                Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                Matrix_RotateX(-pauseCtx->unk_1F4 / 100.0f, MTXMODE_APPLY);

                gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                POLY_KAL_DISP = KaleidoScope_DrawPageSections(POLY_KAL_DISP, pauseCtx->itemPageVtx,
                                                              sSelectItemTexs[gSaveContext.language]);

                KaleidoScope_DrawItemSelect(play);
                break;

            case PAUSE_MAP:
                Matrix_Translate((f32)WREG(3) / 100.0f, (f32)WREG(2) / 100.0f, 0.0f, MTXMODE_NEW);
                Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                Matrix_RotateZ(-pauseCtx->unk_1FC / 100.0f, MTXMODE_APPLY);
                Matrix_RotateY(-1.57f, MTXMODE_APPLY);

                gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                POLY_KAL_DISP =
                    KaleidoScope_DrawPageSections(POLY_KAL_DISP, pauseCtx->mapPageVtx, sMapTexs[gSaveContext.language]);

                if (sInDungeonScene) {
                    KaleidoScope_DrawDungeonMap(play, gfxCtx);
                    Gfx_SetupDL_42Opa(gfxCtx);

                    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

                    if (pauseCtx->cursorSpecialPos == 0) {
                        KaleidoScope_DrawCursor(play, PAUSE_MAP);
                    }

                    if (CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, gSaveContext.mapIndex)) {
                        PauseMapMark_Draw(play);
                    }
                } else {
                    KaleidoScope_DrawWorldMap(play, gfxCtx);
                }
                break;

            case PAUSE_QUEST:
                gDPSetTextureFilter(POLY_KAL_DISP++, G_TF_BILERP);

                Matrix_Translate(0.0f, (f32)WREG(2) / 100.0f, (f32)WREG(3) / 100.0f, MTXMODE_NEW);
                Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                Matrix_RotateX(pauseCtx->unk_200 / 100.0f, MTXMODE_APPLY);
                Matrix_RotateY(3.14f, MTXMODE_APPLY);

                gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                POLY_KAL_DISP = KaleidoScope_DrawPageSections(POLY_KAL_DISP, pauseCtx->questPageVtx,
                                                              sQuestStatusTexs[gSaveContext.language]);

                KaleidoScope_DrawQuestStatus(play, gfxCtx);

                if (pauseCtx->cursorSpecialPos == 0) {
                    KaleidoScope_DrawCursor(play, PAUSE_QUEST);
                }
                break;

            case PAUSE_EQUIP:
                Matrix_Translate(-(f32)WREG(3) / 100.0f, (f32)WREG(2) / 100.0f, 0.0f, MTXMODE_NEW);
                Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                Matrix_RotateZ(pauseCtx->unk_1F8 / 100.0f, MTXMODE_APPLY);
                Matrix_RotateY(1.57f, MTXMODE_APPLY);

                gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                POLY_KAL_DISP = KaleidoScope_DrawPageSections(POLY_KAL_DISP, pauseCtx->equipPageVtx,
                                                              sEquipmentTexs[gSaveContext.language]);

                KaleidoScope_DrawEquipment(play);

                if (pauseCtx->cursorSpecialPos == 0) {
                    KaleidoScope_DrawCursor(play, PAUSE_EQUIP);
                }
                break;
        }
    }

    Gfx_SetupDL_42Opa(gfxCtx);

    if ((pauseCtx->state == 7) || ((pauseCtx->state >= 8) && (pauseCtx->state < 0x12))) {
        KaleidoScope_UpdatePrompt(play);

        gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);

        if (!pauseCtx->pageIndex) { // pageIndex == PAUSE_ITEM
            pauseCtx->unk_1F4 = pauseCtx->unk_204 + 314.0f;

            Matrix_Translate(0.0f, (f32)WREG(2) / 100.0f, -pauseCtx->unk_1F0 / 10.0f, MTXMODE_NEW);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateX(-pauseCtx->unk_204 / 100.0f, MTXMODE_APPLY);
        } else if (pauseCtx->pageIndex == PAUSE_MAP) {
            pauseCtx->unk_1FC = pauseCtx->unk_204 + 314.0f;

            Matrix_Translate(pauseCtx->unk_1F0 / 10.0f, (f32)WREG(2) / 100.0f, 0.0f, MTXMODE_NEW);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateZ(-pauseCtx->unk_204 / 100.0f, MTXMODE_APPLY);
            Matrix_RotateY(-1.57f, MTXMODE_APPLY);
        } else if (pauseCtx->pageIndex == PAUSE_QUEST) {
            pauseCtx->unk_200 = pauseCtx->unk_204 + 314.0f;

            Matrix_Translate(0.0f, (f32)WREG(2) / 100.0f, pauseCtx->unk_1F0 / 10.0f, MTXMODE_NEW);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateX(pauseCtx->unk_204 / 100.0f, MTXMODE_APPLY);
            Matrix_RotateY(3.14f, MTXMODE_APPLY);
        } else {
            pauseCtx->unk_1F8 = pauseCtx->unk_204 + 314.0f;

            Matrix_Translate(-pauseCtx->unk_1F0 / 10.0f, (f32)WREG(2) / 100.0f, 0.0f, MTXMODE_NEW);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateZ(pauseCtx->unk_204 / 100.0f, MTXMODE_APPLY);
            Matrix_RotateY(1.57f, MTXMODE_APPLY);
        }

        gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        if ((pauseCtx->state >= 8) && (pauseCtx->state <= 0x11)) {
            POLY_KAL_DISP = KaleidoScope_DrawPageSections(POLY_KAL_DISP, pauseCtx->saveVtx, sGameOverTexs);
        } else {
            POLY_KAL_DISP =
                KaleidoScope_DrawPageSections(POLY_KAL_DISP, pauseCtx->saveVtx, sSaveTexs[gSaveContext.language]);
        }

        gSPVertex(POLY_KAL_DISP++, &pauseCtx->saveVtx[60], 32, 0);

        if (((pauseCtx->state == 7) && (pauseCtx->unk_1EC < 4)) || (pauseCtx->state == 0xE)) {
            POLY_KAL_DISP =
                KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, sSavePromptTexs[gSaveContext.language], 152, 16, 0);

            gDPSetCombineLERP(POLY_KAL_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0,
                              0, PRIMITIVE, 0);
            gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, aButtonColor.r, aButtonColor.g, aButtonColor.b, VREG(61)); //Save prompt cursor colour

            if (pauseCtx->promptChoice == 0) {
                gSPDisplayList(POLY_KAL_DISP++, gPromptCursorLeftDL);
            } else {
                gSPDisplayList(POLY_KAL_DISP++, gPromptCursorRightDL);
            }

            gDPPipeSync(POLY_KAL_DISP++);
            gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);
            gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

            POLY_KAL_DISP =
                KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, sPromptChoiceTexs[gSaveContext.language][0], 48, 16, 12);

            POLY_KAL_DISP =
                KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, sPromptChoiceTexs[gSaveContext.language][1], 48, 16, 16);
        } else if (((pauseCtx->state == 7 && pauseCtx->unk_1EC >= 4) || pauseCtx->state == 0xF) &&
                   !CVarGetInteger("gSkipSaveConfirmation", 0)) {
            POLY_KAL_DISP =
                KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, sSaveConfirmationTexs[gSaveContext.language], 152, 16, 0);
        } else if ((pauseCtx->state != 7) || (pauseCtx->unk_1EC < 4)) {
            if ((pauseCtx->state != 0xF) && ((pauseCtx->state == 0x10) || (pauseCtx->state == 0x11))) {
                POLY_KAL_DISP =
                    KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, sContinuePromptTexs[gSaveContext.language], 152, 16, 0);

                gDPSetCombineLERP(POLY_KAL_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0,
                                  TEXEL0, 0, PRIMITIVE, 0);
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, aButtonColor.r, aButtonColor.g, aButtonColor.b, VREG(61)); //Continue prompt cursor colour

                if (pauseCtx->promptChoice == 0) {
                    gSPDisplayList(POLY_KAL_DISP++, gPromptCursorLeftDL);
                } else {
                    gSPDisplayList(POLY_KAL_DISP++, gPromptCursorRightDL);
                }

                gDPPipeSync(POLY_KAL_DISP++);
                gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

                if (!IS_BOSS_RUSH) {
                    POLY_KAL_DISP = KaleidoScope_QuadTextureIA8(
                        POLY_KAL_DISP, sPromptChoiceTexs[gSaveContext.language][0], 48, 16, 12);
                } else {
                    // Show "No" twice in Boss Rush because the player can't save within it.
                    POLY_KAL_DISP = KaleidoScope_QuadTextureIA8(
                        POLY_KAL_DISP, sPromptChoiceTexs[gSaveContext.language][1], 48, 16, 12);
                }

                POLY_KAL_DISP =
                    KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, sPromptChoiceTexs[gSaveContext.language][1], 48, 16, 16);
            }
        }

        gDPPipeSync(POLY_KAL_DISP++);
        gDPSetCombineLERP(POLY_KAL_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        if ((pauseCtx->state != 0x10) && (pauseCtx->state != 0x11)) {
            gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 0, pauseCtx->alpha);
            gDPSetEnvColor(POLY_KAL_DISP++, 0, 0, 0, 0);
        }
    }

    CLOSE_DISPS(gfxCtx);
    FrameInterpolation_RecordCloseChild();
}

void KaleidoScope_DrawInfoPanel(PlayState* play) {
    Color_RGB8 aButtonColor = { 0, 100, 255 };
    if (CVarGetInteger("gCosmetics.Hud_AButton.Changed", 0)) {
        aButtonColor = CVarGetColor24("gCosmetics.Hud_AButton.Value", aButtonColor);
    } else if (CVarGetInteger("gCosmetics.DefaultColorScheme", COLORSCHEME_N64) == COLORSCHEME_GAMECUBE) {
        aButtonColor = (Color_RGB8){ 0, 255, 100 };
    }

    Color_RGB8 cButtonsColor = {255, 160, 0};
    if (CVarGetInteger("gCosmetics.Hud_CButtons.Changed", 0)) {
        cButtonsColor = CVarGetColor24("gCosmetics.Hud_CButtons.Value", cButtonsColor);
    }
    Color_RGB8 cUpButtonColor = cButtonsColor;
    if (CVarGetInteger("gCosmetics.Hud_CUpButton.Changed", 0)) {
        cUpButtonColor = CVarGetColor24("gCosmetics.Hud_CUpButton.Value", cUpButtonColor);
    }
    Color_RGB8 cDownButtonColor = cButtonsColor;
    if (CVarGetInteger("gCosmetics.Hud_CDownButton.Changed", 0)) {
        cDownButtonColor = CVarGetColor24("gCosmetics.Hud_CDownButton.Value", cDownButtonColor);
    }
    Color_RGB8 cLeftButtonColor = cButtonsColor;
    if (CVarGetInteger("gCosmetics.Hud_CLeftButton.Changed", 0)) {
        cLeftButtonColor = CVarGetColor24("gCosmetics.Hud_CLeftButton.Value", cLeftButtonColor);
    }
    Color_RGB8 cRightButtonColor = cButtonsColor;
    if (CVarGetInteger("gCosmetics.Hud_CRightButton.Changed", 0)) {
        cRightButtonColor = CVarGetColor24("gCosmetics.Hud_CRightButton.Value", cRightButtonColor);
    }

    static const void* sToEquipTextures[3] = {
        gPauseToEquipENGTex,
        gPauseToEquipGERTex,
        gPauseToEquipFRATex,
    };
    static const void* sToDecideTextures[3] = {
        gPauseToDecideENGTex,
        gPauseToDecideGERTex,
        gPauseToDecideFRATex,
    };
    static const void* sPlayMelodyTextures[3] = {
        gPauseToPlayMelodyENGTex,
        gPauseToPlayMelodyGERTex,
        gPauseToPlayMelodyFRATex,
    };
    static const void* D_8082AD78[][3] = {
        { gPauseToEquipmentENGTex, gPauseToEquipmentGERTex, gPauseToEquipmentFRATex },
        { gPauseToSelectItemENGTex, gPauseToSelectItemGERTex, gPauseToSelectItemFRATex },
        { gPauseToMapENGTex, gPauseToMapGERTex, gPauseToMapFRATex },
        { gPauseToQuestStatusENGTex, gPauseToQuestStatusGERTex, gPauseToQuestStatusFRATex },
    };
    static void* D_8082ADA8[][3] = {
        { gPauseToMapENGTex, gPauseToMapGERTex, gPauseToMapFRATex },
        { gPauseToQuestStatusENGTex, gPauseToQuestStatusGERTex, gPauseToQuestStatusFRATex },
        { gPauseToEquipmentENGTex, gPauseToEquipmentGERTex, gPauseToEquipmentFRATex },
        { gPauseToSelectItemENGTex, gPauseToSelectItemGERTex, gPauseToSelectItemFRATex },
    };
    static u16 D_8082ADD8[3] = { 56, 88, 80 };
    static u16 D_8082ADE0[3] = { 64, 88, 72 };
    static u16 D_8082ADE8[3] = { 80, 104, 112 };
    static s16 D_8082ADF0[][4] = {
        { 180, 210, 255, 220 },
        { 100, 100, 150, 220 },
    };
    static s16 D_8082AE00 = 20;
    static s16 D_8082AE04 = 0;
    static s16 D_8082AE08[] = {
        10, 16, 16, 17, 12, 13, 18, 17, 17, 19, 13, 21, 20, 21, 14, 15, 15, 15, 11, 14,
    };
    static s16 D_8082AE30[] = {
        21, 20, 19, 18, 11, 14, 10, 15, 16, 13, 12, 17,
    };
    static s16 D_808321A0;
    static s16 D_808321A2;
    static s16 D_808321A4;
    static s16 D_808321A6;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 stepR;
    s16 stepG;
    s16 stepB;
    s16 stepA;
    s16 temp;
    s16 i;
    s16 j;

    OPEN_DISPS(play->state.gfxCtx);

    stepR = ABS(D_808321A0 - D_8082ADF0[D_8082AE04][0]) / D_8082AE00;
    stepG = ABS(D_808321A2 - D_8082ADF0[D_8082AE04][1]) / D_8082AE00;
    stepB = ABS(D_808321A4 - D_8082ADF0[D_8082AE04][2]) / D_8082AE00;
    stepA = ABS(D_808321A6 - D_8082ADF0[D_8082AE04][3]) / D_8082AE00;
    if (D_808321A0 >= D_8082ADF0[D_8082AE04][0]) {
        D_808321A0 -= stepR;
    } else {
        D_808321A0 += stepR;
    }
    if (D_808321A2 >= D_8082ADF0[D_8082AE04][1]) {
        D_808321A2 -= stepG;
    } else {
        D_808321A2 += stepG;
    }
    if (D_808321A4 >= D_8082ADF0[D_8082AE04][2]) {
        D_808321A4 -= stepB;
    } else {
        D_808321A4 += stepB;
    }
    if (D_808321A6 >= D_8082ADF0[D_8082AE04][3]) {
        D_808321A6 -= stepA;
    } else {
        D_808321A6 += stepA;
    }

    D_8082AE00--;
    if (D_8082AE00 == 0) {
        D_808321A0 = D_8082ADF0[D_8082AE04][0];
        D_808321A2 = D_8082ADF0[D_8082AE04][1];
        D_808321A4 = D_8082ADF0[D_8082AE04][2];
        D_808321A6 = D_8082ADF0[D_8082AE04][3];
        D_8082AE00 = ZREG(28);
        D_8082AE04 ^= 1;
    }

    temp = pauseCtx->infoPanelOffsetY - 76;
    for (j = 0, i = 0; i < 7; i++, j += 4) {
        pauseCtx->infoPanelVtx[j + 0].v.ob[0] = pauseCtx->infoPanelVtx[j + 2].v.ob[0] = -72;

        pauseCtx->infoPanelVtx[j + 1].v.ob[0] = pauseCtx->infoPanelVtx[j + 3].v.ob[0] = 0;

        pauseCtx->infoPanelVtx[j + 0].v.ob[1] = pauseCtx->infoPanelVtx[j + 1].v.ob[1] = temp;

        pauseCtx->infoPanelVtx[j + 2].v.ob[1] = pauseCtx->infoPanelVtx[j + 3].v.ob[1] = temp - 24;

        pauseCtx->infoPanelVtx[j + 0].v.ob[2] = pauseCtx->infoPanelVtx[j + 1].v.ob[2] =
            pauseCtx->infoPanelVtx[j + 2].v.ob[2] = pauseCtx->infoPanelVtx[j + 3].v.ob[2] = 0;

        pauseCtx->infoPanelVtx[j + 0].v.flag = pauseCtx->infoPanelVtx[j + 1].v.flag =
            pauseCtx->infoPanelVtx[j + 2].v.flag = pauseCtx->infoPanelVtx[j + 3].v.flag = 0;

        pauseCtx->infoPanelVtx[j + 0].v.tc[0] = pauseCtx->infoPanelVtx[j + 0].v.tc[1] =
            pauseCtx->infoPanelVtx[j + 1].v.tc[1] = pauseCtx->infoPanelVtx[j + 2].v.tc[0] = 0;

        pauseCtx->infoPanelVtx[j + 1].v.tc[0] = pauseCtx->infoPanelVtx[j + 3].v.tc[0] = 0x900;

        pauseCtx->infoPanelVtx[j + 2].v.tc[1] = pauseCtx->infoPanelVtx[j + 3].v.tc[1] = 0x300;

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

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) && (pauseCtx->unk_1E4 == 0)) {
        pauseCtx->infoPanelVtx[8].v.ob[0] = pauseCtx->infoPanelVtx[10].v.ob[0] = WREG(16);

        pauseCtx->infoPanelVtx[9].v.ob[0] = pauseCtx->infoPanelVtx[11].v.ob[0] = pauseCtx->infoPanelVtx[8].v.ob[0] + 24;

        pauseCtx->infoPanelVtx[8].v.ob[1] = pauseCtx->infoPanelVtx[9].v.ob[1] = WREG(18);

        pauseCtx->infoPanelVtx[10].v.ob[1] = pauseCtx->infoPanelVtx[11].v.ob[1] =
            pauseCtx->infoPanelVtx[8].v.ob[1] - 32;
    } else {
        pauseCtx->infoPanelVtx[8].v.ob[0] = pauseCtx->infoPanelVtx[10].v.ob[0] = WREG(16) + 3;

        pauseCtx->infoPanelVtx[9].v.ob[0] = pauseCtx->infoPanelVtx[11].v.ob[0] = pauseCtx->infoPanelVtx[8].v.ob[0] + 18;

        pauseCtx->infoPanelVtx[8].v.ob[1] = pauseCtx->infoPanelVtx[9].v.ob[1] = WREG(18) - 3;

        pauseCtx->infoPanelVtx[10].v.ob[1] = pauseCtx->infoPanelVtx[11].v.ob[1] =
            pauseCtx->infoPanelVtx[8].v.ob[1] - 26;
    }

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) && (pauseCtx->unk_1E4 == 0)) {
        pauseCtx->infoPanelVtx[12].v.ob[0] = pauseCtx->infoPanelVtx[14].v.ob[0] = WREG(17);

        pauseCtx->infoPanelVtx[13].v.ob[0] = pauseCtx->infoPanelVtx[15].v.ob[0] =
            pauseCtx->infoPanelVtx[12].v.ob[0] + 24;

        pauseCtx->infoPanelVtx[12].v.ob[1] = pauseCtx->infoPanelVtx[13].v.ob[1] = WREG(18);

        pauseCtx->infoPanelVtx[14].v.ob[1] = pauseCtx->infoPanelVtx[15].v.ob[1] =
            pauseCtx->infoPanelVtx[12].v.ob[1] - 32;
    } else {
        pauseCtx->infoPanelVtx[12].v.ob[0] = pauseCtx->infoPanelVtx[14].v.ob[0] = WREG(17) + 3;

        pauseCtx->infoPanelVtx[13].v.ob[0] = pauseCtx->infoPanelVtx[15].v.ob[0] =
            pauseCtx->infoPanelVtx[12].v.ob[0] + 18;

        pauseCtx->infoPanelVtx[12].v.ob[1] = pauseCtx->infoPanelVtx[13].v.ob[1] = WREG(18) - 3;

        pauseCtx->infoPanelVtx[14].v.ob[1] = pauseCtx->infoPanelVtx[15].v.ob[1] =
            pauseCtx->infoPanelVtx[12].v.ob[1] - 26;
    }

    pauseCtx->infoPanelVtx[9].v.tc[0] = pauseCtx->infoPanelVtx[11].v.tc[0] = pauseCtx->infoPanelVtx[13].v.tc[0] =
        pauseCtx->infoPanelVtx[15].v.tc[0] = 0x300;

    pauseCtx->infoPanelVtx[10].v.tc[1] = pauseCtx->infoPanelVtx[11].v.tc[1] = pauseCtx->infoPanelVtx[14].v.tc[1] =
        pauseCtx->infoPanelVtx[15].v.tc[1] = 0x400;

    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    Matrix_Translate(0.0f, 0.0f, -144.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

    gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 90, 100, 130, 255);
    gSPVertex(POLY_KAL_DISP++, &pauseCtx->infoPanelVtx[0], 16, 0);

    gSPDisplayList(POLY_KAL_DISP++, gItemNamePanelDL);

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) && (pauseCtx->unk_1E4 == 0)) {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, D_808321A0, D_808321A2, D_808321A4, D_808321A6);
    } else {
      if (CVarGetInteger("gUniformLR", 0) != 0) {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 180, 210, 255, 255);
      }
    }

    gSPDisplayList(POLY_KAL_DISP++, gLButtonIconDL);

    if (CVarGetInteger("gUniformLR", 0) == 0) { //Restore the misplace gDPSetPrimColor
      gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 180, 210, 255, 255);
    }

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) && (pauseCtx->unk_1E4 == 0)) {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, D_808321A0, D_808321A2, D_808321A4, D_808321A6);
    } else {
      if (CVarGetInteger("gUniformLR", 0) != 0) {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 180, 210, 255, 255);
      }
    }

    gSPDisplayList(POLY_KAL_DISP++, gRButtonIconDL);

    if (pauseCtx->cursorSpecialPos != 0) {
        j = (pauseCtx->cursorSpecialPos * 4) - 32;
        pauseCtx->cursorVtx[0].v.ob[0] = pauseCtx->infoPanelVtx[j].v.ob[0];
        pauseCtx->cursorVtx[0].v.ob[1] = pauseCtx->infoPanelVtx[j].v.ob[1];
        KaleidoScope_UpdateCursorSize(pauseCtx);
        KaleidoScope_DrawCursor(play, pauseCtx->pageIndex);
    }

    temp = pauseCtx->infoPanelOffsetY - 80;
    pauseCtx->infoPanelVtx[16].v.ob[1] = pauseCtx->infoPanelVtx[17].v.ob[1] = temp;

    pauseCtx->infoPanelVtx[18].v.ob[1] = pauseCtx->infoPanelVtx[19].v.ob[1] = pauseCtx->infoPanelVtx[16].v.ob[1] - 16;

    pauseCtx->infoPanelVtx[18].v.tc[1] = pauseCtx->infoPanelVtx[19].v.tc[1] = 0x200;

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetCombineLERP(POLY_KAL_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetEnvColor(POLY_KAL_DISP++, 20, 30, 40, 0);

    if ((pauseCtx->state == 6) && (pauseCtx->namedItem != PAUSE_ITEM_NONE) && (pauseCtx->nameDisplayTimer < WREG(89)) &&
        (!pauseCtx->unk_1E4 || (pauseCtx->unk_1E4 == 2) || ((pauseCtx->unk_1E4 >= 4) && (pauseCtx->unk_1E4 <= 7)) ||
         (pauseCtx->unk_1E4 == 8)) &&
        (pauseCtx->cursorSpecialPos == 0)) {
        if (!pauseCtx->unk_1E4 || (pauseCtx->unk_1E4 == 2) || ((pauseCtx->unk_1E4 >= 4) && (pauseCtx->unk_1E4 <= 7)) ||
            (pauseCtx->unk_1E4 == 8)) {
            pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -63;

            pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 128;

            pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 0x1000;

            gSPVertex(POLY_KAL_DISP++, &pauseCtx->infoPanelVtx[16], 4, 0);

            if (pauseCtx->nameColorSet == 1) {
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 70, 70, 70, 255);
            } else {
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, 255);
            }

            POLY_KAL_DISP = KaleidoScope_QuadTextureIA4(POLY_KAL_DISP, pauseCtx->nameSegment, 128, 16, 0);
        }

        if (pauseCtx->pageIndex == PAUSE_MAP && CVarGetInteger("gSkulltulaDebugEnabled", 0) != 0) {
            if (YREG(7) != 0) {
                osSyncPrintf(VT_FGCOL(YELLOW));
                osSyncPrintf("キンスタ数(%d) Get_KIN_STA=%x (%x)  (%x)\n", YREG(6), GET_GS_FLAGS(YREG(6)),
                             gAreaGsFlags[YREG(6)], gSaveContext.gsFlags[YREG(6) >> 2]);
                osSyncPrintf(VT_RST);

                YREG(7) = 0;
                SET_GS_FLAGS(D_8082AE30[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]], gAreaGsFlags[D_8082AE30[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]]);
            }
        }

        if ((pauseCtx->pageIndex == PAUSE_MAP) && !sInDungeonScene) {
            if (GET_GS_FLAGS(D_8082AE30[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]) ==
                gAreaGsFlags[D_8082AE30[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]]) {

                pauseCtx->infoPanelVtx[24].v.ob[0] = pauseCtx->infoPanelVtx[26].v.ob[0] = -74;

                pauseCtx->infoPanelVtx[25].v.ob[0] = pauseCtx->infoPanelVtx[27].v.ob[0] =
                    pauseCtx->infoPanelVtx[24].v.ob[0] + 19;

                pauseCtx->infoPanelVtx[24].v.ob[1] = pauseCtx->infoPanelVtx[25].v.ob[1] =
                    pauseCtx->infoPanelVtx[24].v.ob[1] - 2;

                pauseCtx->infoPanelVtx[26].v.ob[1] = pauseCtx->infoPanelVtx[27].v.ob[1] =
                    pauseCtx->infoPanelVtx[24].v.ob[1] - 19;

                pauseCtx->infoPanelVtx[25].v.tc[0] = pauseCtx->infoPanelVtx[27].v.tc[0] = 0x300;

                gDPPipeSync(POLY_KAL_DISP++);
                gSPVertex(POLY_KAL_DISP++, &pauseCtx->infoPanelVtx[24], 4, 0);

                gDPSetCombineLERP(POLY_KAL_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                                  PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
                gDPSetEnvColor(POLY_KAL_DISP++, 0, 0, 0, 0);

                KaleidoScope_DrawQuadTextureRGBA32(play->state.gfxCtx, gQuestIconGoldSkulltulaTex, 24, 24, 0);
            }
        }
    } else if ((pauseCtx->unk_1E4 < 3) || (pauseCtx->unk_1E4 == 7) || (pauseCtx->unk_1E4 == 8)) {
        pauseCtx->infoPanelVtx[20].v.ob[1] = pauseCtx->infoPanelVtx[21].v.ob[1] = temp;

        pauseCtx->infoPanelVtx[22].v.ob[1] = pauseCtx->infoPanelVtx[23].v.ob[1] =
            pauseCtx->infoPanelVtx[20].v.ob[1] - 16;

        pauseCtx->infoPanelVtx[22].v.tc[1] = pauseCtx->infoPanelVtx[23].v.tc[1] = 0x200;

        gSPVertex(POLY_KAL_DISP++, &pauseCtx->infoPanelVtx[16], 8, 0);

        if (pauseCtx->state == 7) {
            pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = WREG(61 + gSaveContext.language);

            pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 24;

            pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + WREG(52 + gSaveContext.language);

            pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                pauseCtx->infoPanelVtx[20].v.ob[0] + D_8082ADE0[gSaveContext.language];

            pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 0x300;

            pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] = D_8082ADE0[gSaveContext.language]
                                                                                      << 5;
            gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, aButtonColor.r, aButtonColor.g, aButtonColor.b, 255);
            //gSPDisplayList(POLY_KAL_DISP++, gAButtonIconDL);//This is changed to load the texture only so we can prim color it.
            gDPLoadTextureBlock(POLY_KAL_DISP++, gABtnSymbolTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);

            gDPPipeSync(POLY_KAL_DISP++);
            gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, 255);

            POLY_KAL_DISP = KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, sToDecideTextures[gSaveContext.language],
                                                        D_8082ADE0[gSaveContext.language], 16, 4);
        } else if (pauseCtx->cursorSpecialPos != 0) {
            if ((pauseCtx->state == 6) && (pauseCtx->unk_1E4 == 0)) {
                pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -63;

                pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + 128;

                pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 0x1000;

                gDPPipeSync(POLY_KAL_DISP++);
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 200, 0, 255);

                if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
                    POLY_KAL_DISP = KaleidoScope_QuadTextureIA8(
                        POLY_KAL_DISP, D_8082AD78[pauseCtx->pageIndex][gSaveContext.language], 128, 16, 0);
                } else {
                    POLY_KAL_DISP = KaleidoScope_QuadTextureIA8(
                        POLY_KAL_DISP, D_8082ADA8[pauseCtx->pageIndex][gSaveContext.language], 128, 16, 0);
                }
            }
        } else {
            bool pauseAnyCursor =
                (CVarGetInteger("gPauseAnyCursor", 0) == PAUSE_ANY_CURSOR_RANDO_ONLY && IS_RANDO) ||
                (CVarGetInteger("gPauseAnyCursor", 0) == PAUSE_ANY_CURSOR_ALWAYS_ON);
            if (!pauseCtx->pageIndex && (!pauseAnyCursor || (gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]] != ITEM_NONE))) { // pageIndex == PAUSE_ITEM
                pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] =
                    WREG(49 + gSaveContext.language);

                pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + 48;

                pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + WREG(58 + gSaveContext.language);

                pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                    pauseCtx->infoPanelVtx[20].v.ob[0] + D_8082ADD8[gSaveContext.language];

                pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 0x600;

                pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] =
                    D_8082ADD8[gSaveContext.language] << 5;

                s16 PosX; // General Pos of C button icon
                if (gSaveContext.language == LANGUAGE_ENG) {
                    PosX = 112;
                } else if (gSaveContext.language == LANGUAGE_GER) {
                    PosX = 175;
                } else { // French
                    PosX = 98;
                }
                s16 PosY = 200 - pauseCtx->infoPanelOffsetY; //General Pos of C button icon
                s16 icon_w = 46; // Original texture size
                s16 icon_h = 16;
                s32 icon_x_offset;
                s16 icon_w_crop = 17.0f; //Left
                int height = icon_h * 1.0f; //Adjust Height with scale
                int width = icon_w * 1.0f; //Adjust Width with scale
                int width_crop = icon_w_crop * 1.0f; //Adjust Width with scale
                int height_factor = (1 << 10) * icon_h / height;
                int width_factor = (1 << 10) * icon_w / width;
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, cButtonsColor.r, cButtonsColor.g, cButtonsColor.b, 255);
                for (s16 i=0; i < 3; i++) {
                    if (i == 0) {
                        icon_x_offset = width_crop*3-3;
                        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, cRightButtonColor.r, cRightButtonColor.g, cRightButtonColor.b, 255);
                    } else if (i == 1) {
                        icon_x_offset = width_crop*2-3;
                        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, cDownButtonColor.r, cDownButtonColor.g, cDownButtonColor.b, 255);
                    } else if (i == 2) {
                        icon_x_offset = width_crop;
                        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, cLeftButtonColor.r, cLeftButtonColor.g, cLeftButtonColor.b, 255);
                    }
                    gDPLoadTextureBlock(POLY_KAL_DISP++, gCBtnSymbolsTex, G_IM_FMT_IA, G_IM_SIZ_8b, icon_w, icon_h, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                    //gSPScisTextureRectangle(POLY_KAL_DISP++,PosX << 2, PosY << 2, (PosX + icon_x_offset) << 2, (PosY + height) << 2, G_TX_RENDERTILE, 0, 0, width_factor, height_factor);
                    gSPWideTextureRectangle(POLY_KAL_DISP++, PosX << 2, PosY << 2, (PosX + icon_x_offset) << 2, (PosY + height) << 2, G_TX_WRAP, 0, 0, width_factor, height_factor);
                }
                gDPPipeSync(POLY_KAL_DISP++);
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, 255);
                POLY_KAL_DISP = KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, sToEquipTextures[gSaveContext.language],
                                                            D_8082ADD8[gSaveContext.language], 16, 4);
            } else if ((pauseCtx->pageIndex == PAUSE_MAP) && sInDungeonScene) {

            } else if ((pauseCtx->pageIndex == PAUSE_QUEST) && (pauseCtx->cursorSlot[PAUSE_QUEST] >= 6) &&
                       (pauseCtx->cursorSlot[PAUSE_QUEST] <= 0x11)) {
                if (pauseCtx->namedItem != PAUSE_ITEM_NONE) {
                    pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] =
                        WREG(55 + gSaveContext.language);

                    pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                        pauseCtx->infoPanelVtx[16].v.ob[0] + 24;

                    pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                        pauseCtx->infoPanelVtx[16].v.ob[0] + WREG(52 + gSaveContext.language);

                    if (gSaveContext.language == LANGUAGE_GER) {
                        pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                            pauseCtx->infoPanelVtx[16].v.ob[0] - 99;
                    }

                    pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                        pauseCtx->infoPanelVtx[20].v.ob[0] + D_8082ADE8[gSaveContext.language];

                    pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 0x300;

                    pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] =
                        D_8082ADE8[gSaveContext.language] << 5;

                    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, aButtonColor.r, aButtonColor.g, aButtonColor.b, 255);
                    //gSPDisplayList(POLY_KAL_DISP++, gAButtonIconDL);
                    gDPLoadTextureBlock(POLY_KAL_DISP++, gABtnSymbolTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
                    gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);

                    gDPPipeSync(POLY_KAL_DISP++);
                    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, 255);

                    POLY_KAL_DISP = KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, sPlayMelodyTextures[gSaveContext.language],
                                                                D_8082ADE8[gSaveContext.language], 16, 4);
                }
            } else if (pauseCtx->pageIndex == PAUSE_EQUIP) {
                pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] =
                    WREG(64 + gSaveContext.language);

                pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + 24;

                pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + WREG(52 + gSaveContext.language);

                pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                    pauseCtx->infoPanelVtx[20].v.ob[0] + D_8082ADD8[gSaveContext.language];

                pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 0x300;

                pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] =
                    D_8082ADD8[gSaveContext.language] << 5;

                 if (!(CHECK_OWNED_EQUIP(pauseCtx->cursorY[PAUSE_EQUIP], pauseCtx->cursorX[PAUSE_EQUIP] - 1)) && (pauseCtx->pageIndex == PAUSE_EQUIP) && (pauseCtx->cursorX[PAUSE_EQUIP] != 0)) {
                    return;
                 }

                //gSPDisplayList(POLY_KAL_DISP++, gAButtonIconDL);
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, aButtonColor.r, aButtonColor.g, aButtonColor.b, 255);
                gDPLoadTextureBlock(POLY_KAL_DISP++, gABtnSymbolTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
                gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);

                gDPPipeSync(POLY_KAL_DISP++);
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, 255);

                POLY_KAL_DISP = KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, sToEquipTextures[gSaveContext.language],
                                                            D_8082ADD8[gSaveContext.language], 16, 4);
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void KaleidoScope_UpdateNamePanel(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    u16 sp2A;
    bool pauseAnyCursor = (CVarGetInteger("gPauseAnyCursor", 0) == PAUSE_ANY_CURSOR_RANDO_ONLY && IS_RANDO) ||
                          (CVarGetInteger("gPauseAnyCursor", 0) == PAUSE_ANY_CURSOR_ALWAYS_ON);

    if ((pauseCtx->namedItem != pauseCtx->cursorItem[pauseCtx->pageIndex]) ||
        ((pauseCtx->pageIndex == PAUSE_MAP) && (pauseCtx->cursorSpecialPos != 0))) {

        pauseCtx->namedItem = pauseCtx->cursorItem[pauseCtx->pageIndex];
        sp2A = pauseCtx->namedItem;

        osCreateMesgQueue(&pauseCtx->loadQueue, &pauseCtx->loadMsg, 1);

        if (pauseAnyCursor &&
        ((pauseCtx->pageIndex == PAUSE_EQUIP && pauseCtx->cursorX[PAUSE_EQUIP] != 0 && !CHECK_OWNED_EQUIP(pauseCtx->cursorY[PAUSE_EQUIP], pauseCtx->cursorX[PAUSE_EQUIP] - 1)) ||
        (pauseCtx->pageIndex == PAUSE_ITEM && gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]] == ITEM_NONE))) {
            pauseCtx->namedItem = PAUSE_ITEM_NONE;
        }

        if (pauseCtx->namedItem != PAUSE_ITEM_NONE) {
            if ((pauseCtx->pageIndex == PAUSE_MAP) && !sInDungeonScene) {
                if (gSaveContext.language) {
                    sp2A += 12;
                }
                if (gSaveContext.language == LANGUAGE_FRA) {
                    sp2A += 12;
                }

                const char* textureName = mapNameTextures[sp2A];
                memcpy(pauseCtx->nameSegment, textureName, strlen(textureName) + 1);
            } else {
                osSyncPrintf("zoom_name=%d\n", pauseCtx->namedItem);

                if (gSaveContext.language) {
                    sp2A += 123;
                }
                if (gSaveContext.language == LANGUAGE_FRA) {
                    sp2A += 123;
                }

                osSyncPrintf("J_N=%d  point=%d\n", gSaveContext.language, sp2A);

                const char* textureName = iconNameTextures[sp2A];
                memcpy(pauseCtx->nameSegment, textureName, strlen(textureName) + 1);
            }

            pauseCtx->nameDisplayTimer = 0;
        }
    } else if (pauseCtx->nameColorSet == 0) {
        if (((pauseCtx->pageIndex == PAUSE_QUEST) && (pauseCtx->cursorSlot[PAUSE_QUEST] >= 6) &&
             (pauseCtx->cursorSlot[PAUSE_QUEST] <= 0x11) && (pauseCtx->unk_1E4 == 8)) ||
            (pauseCtx->pageIndex == PAUSE_ITEM) ||
            ((pauseCtx->pageIndex == PAUSE_EQUIP) && (pauseCtx->cursorX[PAUSE_EQUIP] != 0))) {
            if (pauseCtx->namedItem != ITEM_SOLD_OUT) {
                pauseCtx->nameDisplayTimer++;
                if (pauseCtx->nameDisplayTimer > WREG(88)) {
                    pauseCtx->nameDisplayTimer = 0;
                }
            }
        } else {
            pauseCtx->nameDisplayTimer = 0;
        }
    } else {
        pauseCtx->nameDisplayTimer = 0;
    }
}

void func_808237B4(PlayState* play, Input* input) {
    PauseContext* pauseCtx = &play->pauseCtx;
    s32 cond = false;
    s32 mode;

    if (ZREG(13) && !CHECK_BTN_ALL(input->press.button, BTN_L)) {
        cond = true;
    }

    if (!cond) {
        mode = pauseCtx->mode;
        pauseCtx->eye.x += D_8082ABAC[mode];
        pauseCtx->eye.z += D_8082ABCC[mode];

        if (pauseCtx->unk_1EA < 32) {
            WREG(16) -= WREG(25) / WREG(6);
            WREG(17) -= WREG(26) / WREG(6);
        } else {
            WREG(16) += WREG(25) / WREG(6);
            WREG(17) += WREG(26) / WREG(6);
        }

        pauseCtx->unk_1EA += 4;

        if (pauseCtx->unk_1EA == 64) {
            pauseCtx->unk_1EA = 0;
            pauseCtx->pageIndex = D_8082ABEC[pauseCtx->mode];
            pauseCtx->unk_1E4 = 0;
        }
    }
}

void KaleidoScope_SetView(PauseContext* pauseCtx, f32 x, f32 y, f32 z) {
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    eye.x = x;
    eye.y = y;
    eye.z = z;
    lookAt.x = lookAt.y = lookAt.z = 0.0f;
    up.x = up.z = 0.0f;
    up.y = 1.0f;

    func_800AA358(&pauseCtx->view, &eye, &lookAt, &up);
    func_800AAA50(&pauseCtx->view, 127);
}

static u8 D_8082AE48[][4] = {
    { 10, 70, 70, 10 },   { 10, 90, 90, 10 },   { 80, 140, 140, 80 },
    { 80, 120, 120, 80 }, { 80, 140, 140, 80 }, { 50, 110, 110, 50 },
};
static u8 D_8082AE60[][4] = {
    { 50, 100, 100, 50 }, { 50, 100, 100, 50 }, { 40, 60, 60, 40 },
    { 80, 120, 120, 80 }, { 40, 60, 60, 40 },   { 50, 110, 110, 50 },
};
static u8 D_8082AE78[][4] = {
    { 80, 130, 130, 80 }, { 40, 60, 60, 40 }, { 30, 60, 60, 30 },
    { 50, 70, 70, 50 },   { 30, 60, 60, 30 }, { 50, 110, 110, 50 },
};

static s16 D_8082AE90[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AE94[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AE98[] = {
    0xFFDC, 0x000C, 0xFFEE, 0x0046, 0x0046, 0x0046, 0xFFA8, 0xFFA8, 0xFFA8,
    0xFFA8, 0xFFA8, 0xFFA8, 0xFFA8, 0xFFA8, 0xFF96, 0xFFC2, 0xFFD8, 0x0000,
};
static s16 D_8082AEBC[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AEC0[] = {
    0x002F, 0xFFCF, 0xFFEF, 0xFFF1, 0xFFF7, 0x0018, 0x002B, 0x000E, 0x0009, 0x0026, 0x0052,
    0x0047, 0xFFB4, 0xFFA9, 0xFF94, 0xFFCA, 0xFFA3, 0xFFBD, 0xFFC8, 0xFFDF, 0xFFF6, 0x0001,
    0x000E, 0x0018, 0x0023, 0x003A, 0x004A, 0x0059, 0x0000, 0xFFC6, 0x0013, 0x001C,
};
static s16 D_8082AF00[] = {
    0xFFB4, 0xFFC6, 0x000A, 0xFFC6, 0x000A, 0x0000,
};
static s16 D_8082AF0C[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AF10[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AF14[] = {
    0x0030, 0x0030, 0x0060, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018,
    0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0010, 0x0010, 0x0018, 0x0000,
};
static s16 D_8082AF38[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AF3C[] = {
    0x0098, 0x0030, 0x0030, 0x0030, 0x0030, 0x0000,
};
static s16 D_8082AF48[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AF4C[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AF50[] = {
    0x001C, 0x001C, 0x002E, 0x001C, 0xFFFE, 0xFFE0, 0x0032, 0x0024, 0x0016,
    0x0008, 0xFFFA, 0xFFEC, 0xFFDE, 0xFFD0, 0x0012, 0x0012, 0x0032, 0x0000,
};
static s16 D_8082AF74[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AF78[] = {
    0x000F, 0x0028, 0x000B, 0x002D, 0x0034, 0x0025, 0x0024, 0x0039, 0x0036, 0x0021, 0x001F,
    0x002D, 0x0020, 0x002A, 0x0031, 0xFFF6, 0x001F, 0x001B, 0x000F, 0xFFCF, 0x0008, 0x0026,
    0x0007, 0x002F, 0x001E, 0x0001, 0xFFF7, 0x0019, 0x0000, 0x0001, 0xFFE0, 0xFFE6,
};
static s16 D_8082AFB8[] = {
    0x0024, 0x000A, 0x000A, 0xFFFA, 0xFFFA, 0x0000,
};
static s16 D_8082AFC4[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AFC8[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AFCC[] = {
    0x0055, 0x0055, 0x0010, 0x0018, 0x0018, 0x0018, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0018, 0x0000,
};
static s16 D_8082AFF0[] = {
    0x0000,
    0x0000,
};
static s16 D_8082AFF4[] = {
    0x0010, 0x0030, 0x0030, 0x0010, 0x0010, 0x0000,
};

static s16* D_8082B000[] = {
    D_8082AE90, D_8082AE94, D_8082AE98, D_8082AEBC, D_8082AEC0, D_8082AF00,
};

static s16* D_8082B018[] = {
    D_8082AF0C, D_8082AF10, D_8082AF14, D_8082AF38, D_8082AAEC, D_8082AF3C,
};

static s16* D_8082B030[] = {
    D_8082AF48, D_8082AF4C, D_8082AF50, D_8082AF74, D_8082AF78, D_8082AFB8,
};

static s16* D_8082B048[] = {
    D_8082AFC4, D_8082AFC8, D_8082AFCC, D_8082AFF0, D_8082AB2C, D_8082AFF4,
};

static s16 D_8082B060[] = {
    0xFFC6, 0x000B, 0x001E, 0x001E, 0x000F, 0x0026, 0xFFC2, 0x003C, 0x003D, 0xFFB2, 0xFED4,
    0xFFAA, 0xFFBF, 0xFED4, 0xFED4, 0xFFEB, 0x000E, 0x000D, 0x0014, 0xFFDE, 0xFED4, 0x0000,
};

static s16 D_8082B08C[] = {
    0x0059, 0x0014, 0x000E, 0x0023, 0x0020, 0x0011, 0x0032, 0x0010, 0x0015, 0x0014, 0xFFFF,
    0x0020, 0x0010, 0xFFFF, 0xFFFF, 0x0013, 0x0013, 0x0015, 0x0010, 0x0014, 0xFFFF, 0x0000,
};

static s16 D_8082B0B8[] = {
    0x0001, 0x000F, 0x0014, 0x0009, 0xFFE2, 0xFFEF, 0xFFDE, 0x000F, 0x001E, 0x0001, 0xFED4,
    0x002A, 0x0007, 0xFED4, 0xFED4, 0x0018, 0x0024, 0x0035, 0x0025, 0xFFF3, 0xFED4, 0x0000,
};

static s16 D_8082B0E4[] = {
    0x0024, 0x000F, 0x0010, 0x0017, 0x0017, 0x0010, 0x0018, 0x000D, 0x0011, 0x0012, 0x0001,
    0x0019, 0x000D, 0x0001, 0x0001, 0x000D, 0x0015, 0x000F, 0x000D, 0x000C, 0x0001, 0x0000,
};

s16 func_80823A0C(PlayState* play, Vtx* vtx, s16 arg2, s16 arg3) {
    static s16 D_8082B110 = 0;
    static s16 D_8082B114 = 1;
    static s16 D_8082B118 = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16* ptr1;
    s16* ptr2;
    s16* ptr3;
    s16* ptr4;
    s16 phi_s2;
    s16 phi_t0;
    s16 phi_a1;
    s16 phi_a2;
    s16 phi_t3;
    s16 phi_t1;

    phi_t0 = -200;

    for (phi_t1 = 0, phi_t3 = 0; phi_t3 < 3; phi_t3++) {
        phi_t0 += 80;

        for (phi_a1 = 80, phi_a2 = 0; phi_a2 < 5; phi_a2++, phi_t1 += 4, phi_a1 -= 32) {
            vtx[phi_t1 + 0].v.ob[0] = vtx[phi_t1 + 2].v.ob[0] = phi_t0;

            vtx[phi_t1 + 1].v.ob[0] = vtx[phi_t1 + 3].v.ob[0] = vtx[phi_t1 + 0].v.ob[0] + 80;

            vtx[phi_t1 + 0].v.ob[1] = vtx[phi_t1 + 1].v.ob[1] = phi_a1 + pauseCtx->offsetY;

            vtx[phi_t1 + 2].v.ob[1] = vtx[phi_t1 + 3].v.ob[1] = vtx[phi_t1 + 0].v.ob[1] - 32;

            vtx[phi_t1 + 0].v.ob[2] = vtx[phi_t1 + 1].v.ob[2] = vtx[phi_t1 + 2].v.ob[2] = vtx[phi_t1 + 3].v.ob[2] = 0;

            vtx[phi_t1 + 0].v.flag = 0;
            vtx[phi_t1 + 1].v.flag = 0;
            vtx[phi_t1 + 2].v.flag = 0;
            vtx[phi_t1 + 3].v.flag = 0;

            vtx[phi_t1 + 0].v.tc[0] = vtx[phi_t1 + 0].v.tc[1] = vtx[phi_t1 + 1].v.tc[1] = vtx[phi_t1 + 2].v.tc[0] = 0;

            vtx[phi_t1 + 1].v.tc[0] = vtx[phi_t1 + 3].v.tc[0] = 0xA00;

            vtx[phi_t1 + 2].v.tc[1] = vtx[phi_t1 + 3].v.tc[1] = 0x400;

            vtx[phi_t1 + 0].v.cn[0] = vtx[phi_t1 + 2].v.cn[0] = D_8082AE48[arg2][phi_t3 + 0];

            vtx[phi_t1 + 0].v.cn[1] = vtx[phi_t1 + 2].v.cn[1] = D_8082AE60[arg2][phi_t3 + 0];

            vtx[phi_t1 + 0].v.cn[2] = vtx[phi_t1 + 2].v.cn[2] = D_8082AE78[arg2][phi_t3 + 0];

            vtx[phi_t1 + 1].v.cn[0] = vtx[phi_t1 + 3].v.cn[0] = D_8082AE48[arg2][phi_t3 + 1];

            vtx[phi_t1 + 1].v.cn[1] = vtx[phi_t1 + 3].v.cn[1] = D_8082AE60[arg2][phi_t3 + 1];

            vtx[phi_t1 + 1].v.cn[2] = vtx[phi_t1 + 3].v.cn[2] = D_8082AE78[arg2][phi_t3 + 1];

            vtx[phi_t1 + 0].v.cn[3] = vtx[phi_t1 + 2].v.cn[3] = vtx[phi_t1 + 1].v.cn[3] = vtx[phi_t1 + 3].v.cn[3] =
                pauseCtx->alpha;
        }
    }

    phi_s2 = phi_t1;

    if (arg3 != 0) {
        ptr1 = D_8082B000[arg2];
        ptr2 = D_8082B018[arg2];
        ptr3 = D_8082B030[arg2];
        ptr4 = D_8082B048[arg2];

        for (phi_t3 = 0; phi_t3 < arg3; phi_t3++, phi_t1 += 4) {
            vtx[phi_t1 + 2].v.ob[0] = vtx[phi_t1 + 0].v.ob[0] = ptr1[phi_t3];

            vtx[phi_t1 + 1].v.ob[0] = vtx[phi_t1 + 3].v.ob[0] = vtx[phi_t1 + 0].v.ob[0] + ptr2[phi_t3];

            if (!((pauseCtx->state >= 8) && (pauseCtx->state <= 0x11))) {
                vtx[phi_t1 + 0].v.ob[1] = vtx[phi_t1 + 1].v.ob[1] = ptr3[phi_t3] + pauseCtx->offsetY;
            } else {
                vtx[phi_t1 + 0].v.ob[1] = vtx[phi_t1 + 1].v.ob[1] = YREG(60 + phi_t3) + pauseCtx->offsetY;
            }

            vtx[phi_t1 + 2].v.ob[1] = vtx[phi_t1 + 3].v.ob[1] = vtx[phi_t1 + 0].v.ob[1] - ptr4[phi_t3];

            vtx[phi_t1 + 0].v.ob[2] = vtx[phi_t1 + 1].v.ob[2] = vtx[phi_t1 + 2].v.ob[2] = vtx[phi_t1 + 3].v.ob[2] = 0;

            vtx[phi_t1 + 0].v.flag = vtx[phi_t1 + 1].v.flag = vtx[phi_t1 + 2].v.flag = vtx[phi_t1 + 3].v.flag = 0;

            vtx[phi_t1 + 0].v.tc[0] = vtx[phi_t1 + 0].v.tc[1] = vtx[phi_t1 + 1].v.tc[1] = vtx[phi_t1 + 2].v.tc[0] = 0;

            vtx[phi_t1 + 1].v.tc[0] = vtx[phi_t1 + 3].v.tc[0] = ptr2[phi_t3] << 5;

            vtx[phi_t1 + 2].v.tc[1] = vtx[phi_t1 + 3].v.tc[1] = ptr4[phi_t3] << 5;

            vtx[phi_t1 + 0].v.cn[0] = vtx[phi_t1 + 2].v.cn[0] = vtx[phi_t1 + 0].v.cn[1] = vtx[phi_t1 + 2].v.cn[1] =
                vtx[phi_t1 + 0].v.cn[2] = vtx[phi_t1 + 2].v.cn[2] = vtx[phi_t1 + 1].v.cn[0] = vtx[phi_t1 + 3].v.cn[0] =
                    vtx[phi_t1 + 1].v.cn[1] = vtx[phi_t1 + 3].v.cn[1] = vtx[phi_t1 + 1].v.cn[2] =
                        vtx[phi_t1 + 3].v.cn[2] = 255;

            vtx[phi_t1 + 0].v.cn[3] = vtx[phi_t1 + 2].v.cn[3] = vtx[phi_t1 + 1].v.cn[3] = vtx[phi_t1 + 3].v.cn[3] =
                pauseCtx->alpha;
        }

        if (arg2 == 4) {
            phi_t1 -= 12;

            phi_t3 = gSaveContext.worldMapArea;

            vtx[phi_t1 + 0].v.ob[0] = vtx[phi_t1 + 2].v.ob[0] = D_8082B060[phi_t3];

            if (phi_t3) {}

            vtx[phi_t1 + 1].v.ob[0] = vtx[phi_t1 + 3].v.ob[0] = vtx[phi_t1 + 0].v.ob[0] + D_8082B08C[phi_t3];

            vtx[phi_t1 + 0].v.ob[1] = vtx[phi_t1 + 1].v.ob[1] = D_8082B0B8[phi_t3] + pauseCtx->offsetY;

            vtx[phi_t1 + 2].v.ob[1] = vtx[phi_t1 + 3].v.ob[1] = vtx[phi_t1 + 0].v.ob[1] - D_8082B0E4[phi_t3];

            phi_t1 += 12;

            if (pauseCtx->tradeQuestLocation != 0xFF) {
                if (D_8082B114 == 0) {
                    D_8082B118++;
                    switch (D_8082B118) {
                        case 1:
                            D_8082B110 = 3;
                            D_8082B114 = 8;
                            break;
                        case 2:
                            D_8082B110 = 0;
                            D_8082B114 = 6;
                            D_8082B118 = 0;
                            break;
                    }
                } else {
                    D_8082B114--;
                }

                phi_t3 = phi_s2 + (pauseCtx->tradeQuestLocation * 4) + 64;
                phi_a2 = phi_s2 + 116;

                vtx[phi_a2 + 0].v.ob[0] = vtx[phi_a2 + 2].v.ob[0] = vtx[phi_t3 + 0].v.ob[0];

                vtx[phi_a2 + 1].v.ob[0] = vtx[phi_a2 + 3].v.ob[0] = vtx[phi_a2 + 0].v.ob[0] + 8;

                vtx[phi_a2 + 0].v.ob[1] = vtx[phi_a2 + 1].v.ob[1] = vtx[phi_t3 + 0].v.ob[1] - D_8082B110 + 10;

                vtx[phi_a2 + 0].v.ob[2] = vtx[phi_a2 + 1].v.ob[2] = vtx[phi_a2 + 2].v.ob[2] = vtx[phi_a2 + 3].v.ob[2] =
                    0;

                vtx[phi_a2 + 2].v.ob[1] = vtx[phi_a2 + 3].v.ob[1] = vtx[phi_a2 + 0].v.ob[1] - 8;

                vtx[phi_a2 + 0].v.flag = vtx[phi_a2 + 1].v.flag = vtx[phi_a2 + 2].v.flag = vtx[phi_a2 + 3].v.flag = 0;

                vtx[phi_t1].v.tc[0] = vtx[phi_t1].v.tc[1] = vtx[phi_a2 + 1].v.tc[1] = vtx[phi_a2 + 2].v.tc[0] = 0;

                vtx[phi_a2 + 1].v.tc[0] = vtx[phi_a2 + 3].v.tc[0] = 0x100;

                vtx[phi_a2 + 2].v.tc[1] = vtx[phi_a2 + 3].v.tc[1] = 0x100;

                vtx[phi_a2 + 0].v.cn[0] = vtx[phi_a2 + 2].v.cn[0] = vtx[phi_a2 + 0].v.cn[1] = vtx[phi_a2 + 2].v.cn[1] =
                    vtx[phi_a2 + 0].v.cn[2] = vtx[phi_a2 + 2].v.cn[2] = vtx[phi_a2 + 1].v.cn[0] =
                        vtx[phi_a2 + 3].v.cn[0] = vtx[phi_a2 + 1].v.cn[1] = vtx[phi_a2 + 3].v.cn[1] =
                            vtx[phi_a2 + 1].v.cn[2] = vtx[phi_a2 + 3].v.cn[2] = 255;

                vtx[phi_a2 + 0].v.cn[3] = vtx[phi_a2 + 2].v.cn[3] = vtx[phi_a2 + 1].v.cn[3] = vtx[phi_a2 + 3].v.cn[3] =
                    pauseCtx->alpha;
            }
        }
    }

    return phi_t1;
}

static s16 D_8082B11C[] = { 0, 4, 8, 12, 24, 32, 56 };

static s16 D_8082B12C[] = { -114, 12, 44, 76 };

static u8 D_8082B134[] = { 1, 5, 9, 13 };

static s16 D_8082B138[] = {
    74,  74,  46,  18,  18,  46,   -108, -90,  -72, -54, -36, -18, -108, -90, -72, -54,
    -36, -18, 20,  46,  72,  -110, -86,  -110, -54, -98, -86, -74, -62,  -50, -38, -26,
    -14, -98, -86, -74, -62, -50,  -38,  -26,  -14, -88, -81, -72, -90,  -83, -74,
};

static s16 D_8082B198[] = {
    38, 6,   -12, 6,   38,  56,  -20, -20, -20, -20, -20, -20, 2,   2,   2,   2,   2,   2,  -46, -46, -46, 58, 58, 34,
    58, -52, -52, -52, -52, -52, -52, -52, -52, -52, -52, -52, -52, -52, -52, -52, -52, 34, 34,  34,  36,  36, 36,
};

static s16 D_8082B1F8[] = {
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    48, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
};

void KaleidoScope_InitVertices(PlayState* play, GraphicsContext* gfxCtx) {
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 phi_t1;
    s16 phi_t2;
    s16 phi_t2_2;
    s16 phi_t3;
    s16 phi_t4;
    s16 phi_t5;

    pauseCtx->offsetY = 0;

    if ((pauseCtx->state == 4) || (pauseCtx->state >= 0x12) ||
        ((pauseCtx->state == 7) && ((pauseCtx->unk_1EC == 2) || (pauseCtx->unk_1EC == 5))) ||
        ((pauseCtx->state >= 8) && (pauseCtx->state <= 0xD))) {
        pauseCtx->offsetY = 80;
    }

    pauseCtx->itemPageVtx = Graph_Alloc(gfxCtx, 60 * sizeof(Vtx));
    func_80823A0C(play, pauseCtx->itemPageVtx, 0, 0);

    pauseCtx->equipPageVtx = Graph_Alloc(gfxCtx, 60 * sizeof(Vtx));
    func_80823A0C(play, pauseCtx->equipPageVtx, 1, 0);

    if (!sInDungeonScene) {
        pauseCtx->mapPageVtx = Graph_Alloc(gfxCtx, 248 * sizeof(Vtx));
        phi_t3 = func_80823A0C(play, pauseCtx->mapPageVtx, 4, 32);

        for (phi_t2 = 0, phi_t5 = 58; phi_t2 < 15; phi_t2++, phi_t3 += 4, phi_t5 -= 9) {
            pauseCtx->mapPageVtx[phi_t3 + 2].v.ob[0] = -108;
            pauseCtx->mapPageVtx[phi_t3 + 0].v.ob[0] = pauseCtx->mapPageVtx[phi_t3 + 2].v.ob[0];

            pauseCtx->mapPageVtx[phi_t3 + 1].v.ob[0] = pauseCtx->mapPageVtx[phi_t3 + 3].v.ob[0] =
                pauseCtx->mapPageVtx[phi_t3 + 0].v.ob[0] + 216;

            pauseCtx->mapPageVtx[phi_t3 + 0].v.ob[1] = pauseCtx->mapPageVtx[phi_t3 + 1].v.ob[1] =
                phi_t5 + pauseCtx->offsetY;

            pauseCtx->mapPageVtx[phi_t3 + 2].v.ob[1] = pauseCtx->mapPageVtx[phi_t3 + 3].v.ob[1] =
                pauseCtx->mapPageVtx[phi_t3 + 0].v.ob[1] - 9;

            pauseCtx->mapPageVtx[phi_t3 + 0].v.ob[2] = pauseCtx->mapPageVtx[phi_t3 + 1].v.ob[2] =
                pauseCtx->mapPageVtx[phi_t3 + 2].v.ob[2] = pauseCtx->mapPageVtx[phi_t3 + 3].v.ob[2] = 0;

            pauseCtx->mapPageVtx[phi_t3 + 0].v.flag = pauseCtx->mapPageVtx[phi_t3 + 1].v.flag =
                pauseCtx->mapPageVtx[phi_t3 + 2].v.flag = pauseCtx->mapPageVtx[phi_t3 + 3].v.flag = 0;

            pauseCtx->mapPageVtx[phi_t3 + 0].v.tc[0] = pauseCtx->mapPageVtx[phi_t3 + 0].v.tc[1] =
                pauseCtx->mapPageVtx[phi_t3 + 1].v.tc[1] = pauseCtx->mapPageVtx[phi_t3 + 2].v.tc[0] = 0;

            pauseCtx->mapPageVtx[phi_t3 + 1].v.tc[0] = pauseCtx->mapPageVtx[phi_t3 + 3].v.tc[0] = 0x1B00;

            pauseCtx->mapPageVtx[phi_t3 + 2].v.tc[1] = pauseCtx->mapPageVtx[phi_t3 + 3].v.tc[1] = 0x120;

            pauseCtx->mapPageVtx[phi_t3 + 0].v.cn[0] = pauseCtx->mapPageVtx[phi_t3 + 2].v.cn[0] =
                pauseCtx->mapPageVtx[phi_t3 + 0].v.cn[1] = pauseCtx->mapPageVtx[phi_t3 + 2].v.cn[1] =
                    pauseCtx->mapPageVtx[phi_t3 + 0].v.cn[2] = pauseCtx->mapPageVtx[phi_t3 + 2].v.cn[2] =
                        pauseCtx->mapPageVtx[phi_t3 + 1].v.cn[0] = pauseCtx->mapPageVtx[phi_t3 + 3].v.cn[0] =
                            pauseCtx->mapPageVtx[phi_t3 + 1].v.cn[1] = pauseCtx->mapPageVtx[phi_t3 + 3].v.cn[1] =
                                pauseCtx->mapPageVtx[phi_t3 + 1].v.cn[2] = pauseCtx->mapPageVtx[phi_t3 + 3].v.cn[2] =
                                    pauseCtx->mapPageVtx[phi_t3 + 0].v.cn[3] =
                                        pauseCtx->mapPageVtx[phi_t3 + 2].v.cn[3] =
                                            pauseCtx->mapPageVtx[phi_t3 + 1].v.cn[3] =
                                                pauseCtx->mapPageVtx[phi_t3 + 3].v.cn[3] = pauseCtx->alpha;
        }

        pauseCtx->mapPageVtx[phi_t3 - 2].v.ob[1] = pauseCtx->mapPageVtx[phi_t3 - 1].v.ob[1] =
            pauseCtx->mapPageVtx[phi_t3 - 4].v.ob[1] - 2;

        pauseCtx->mapPageVtx[phi_t3 - 2].v.tc[1] = pauseCtx->mapPageVtx[phi_t3 - 1].v.tc[1] = 0x40;
    } else {
        pauseCtx->mapPageVtx = Graph_Alloc(gfxCtx, 128 * sizeof(Vtx));
        func_80823A0C(play, pauseCtx->mapPageVtx, 2, 17);
    }

    pauseCtx->questPageVtx = Graph_Alloc(gfxCtx, 60 * sizeof(Vtx));
    func_80823A0C(play, pauseCtx->questPageVtx, 3, 0);

    pauseCtx->cursorVtx = Graph_Alloc(gfxCtx, 20 * sizeof(Vtx));

    for (phi_t2 = 0; phi_t2 < 20; phi_t2++) {
        pauseCtx->cursorVtx[phi_t2].v.ob[0] = pauseCtx->cursorVtx[phi_t2].v.ob[1] =
            pauseCtx->cursorVtx[phi_t2].v.ob[2] = 0;

        pauseCtx->cursorVtx[phi_t2].v.flag = 0;

        pauseCtx->cursorVtx[phi_t2].v.tc[0] = pauseCtx->cursorVtx[phi_t2].v.tc[1] = 0;

        pauseCtx->cursorVtx[phi_t2].v.cn[0] = pauseCtx->cursorVtx[phi_t2].v.cn[1] =
            pauseCtx->cursorVtx[phi_t2].v.cn[2] = pauseCtx->cursorVtx[phi_t2].v.cn[3] = 255;
    }

    pauseCtx->cursorVtx[1].v.tc[0] = pauseCtx->cursorVtx[2].v.tc[1] = pauseCtx->cursorVtx[3].v.tc[0] =
        pauseCtx->cursorVtx[3].v.tc[1] = pauseCtx->cursorVtx[5].v.tc[0] = pauseCtx->cursorVtx[6].v.tc[1] =
            pauseCtx->cursorVtx[7].v.tc[0] = pauseCtx->cursorVtx[7].v.tc[1] = pauseCtx->cursorVtx[9].v.tc[0] =
                pauseCtx->cursorVtx[10].v.tc[1] = pauseCtx->cursorVtx[11].v.tc[0] = pauseCtx->cursorVtx[11].v.tc[1] =
                    pauseCtx->cursorVtx[13].v.tc[0] = pauseCtx->cursorVtx[14].v.tc[1] =
                        pauseCtx->cursorVtx[15].v.tc[0] = pauseCtx->cursorVtx[15].v.tc[1] = 0x200;

    pauseCtx->cursorVtx[17].v.tc[0] = pauseCtx->cursorVtx[18].v.tc[1] = pauseCtx->cursorVtx[19].v.tc[0] =
        pauseCtx->cursorVtx[19].v.tc[1] = 0x400;

    // 24 items, 7 "item selected" backgrounds, 14 ammo digits (2 each for 7 items) -- then 4 vertices for each
    pauseCtx->itemVtx = Graph_Alloc(gfxCtx, (24 + 7 + 14) * 4 * sizeof(Vtx));

    for (phi_t4 = 0, phi_t2 = 0, phi_t5 = 58; phi_t4 < 4; phi_t4++, phi_t5 -= 32) {
        for (phi_t1 = -96, phi_t3 = 0; phi_t3 < 6; phi_t3++, phi_t2 += 4, phi_t1 += 32) {
            pauseCtx->itemVtx[phi_t2 + 0].v.ob[0] = pauseCtx->itemVtx[phi_t2 + 2].v.ob[0] = phi_t1 + 2;

            pauseCtx->itemVtx[phi_t2 + 1].v.ob[0] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[0] =
                pauseCtx->itemVtx[phi_t2 + 0].v.ob[0] + 0x1C;

            pauseCtx->itemVtx[phi_t2 + 0].v.ob[1] = pauseCtx->itemVtx[phi_t2 + 1].v.ob[1] =
                phi_t5 + pauseCtx->offsetY - 2;

            pauseCtx->itemVtx[phi_t2 + 2].v.ob[1] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[1] =
                pauseCtx->itemVtx[phi_t2 + 0].v.ob[1] - 0x1C;

            pauseCtx->itemVtx[phi_t2 + 0].v.ob[2] = pauseCtx->itemVtx[phi_t2 + 1].v.ob[2] =
                pauseCtx->itemVtx[phi_t2 + 2].v.ob[2] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[2] = 0;

            pauseCtx->itemVtx[phi_t2 + 0].v.flag = pauseCtx->itemVtx[phi_t2 + 1].v.flag =
                pauseCtx->itemVtx[phi_t2 + 2].v.flag = pauseCtx->itemVtx[phi_t2 + 3].v.flag = 0;

            pauseCtx->itemVtx[phi_t2 + 0].v.tc[0] = pauseCtx->itemVtx[phi_t2 + 0].v.tc[1] =
                pauseCtx->itemVtx[phi_t2 + 1].v.tc[1] = pauseCtx->itemVtx[phi_t2 + 2].v.tc[0] = 0;

            pauseCtx->itemVtx[phi_t2 + 1].v.tc[0] = pauseCtx->itemVtx[phi_t2 + 2].v.tc[1] =
                pauseCtx->itemVtx[phi_t2 + 3].v.tc[0] = pauseCtx->itemVtx[phi_t2 + 3].v.tc[1] = 0x400;

            pauseCtx->itemVtx[phi_t2 + 0].v.cn[0] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[0] =
                pauseCtx->itemVtx[phi_t2 + 2].v.cn[0] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[0] =
                    pauseCtx->itemVtx[phi_t2 + 0].v.cn[1] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[1] =
                        pauseCtx->itemVtx[phi_t2 + 2].v.cn[1] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[1] =
                            pauseCtx->itemVtx[phi_t2 + 0].v.cn[2] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[2] =
                                pauseCtx->itemVtx[phi_t2 + 2].v.cn[2] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[2] = 255;

            pauseCtx->itemVtx[phi_t2 + 0].v.cn[3] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[3] =
                pauseCtx->itemVtx[phi_t2 + 2].v.cn[3] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[3] = 255;
        }
    }

    for (phi_t3 = 1; phi_t3 < ARRAY_COUNT(gSaveContext.equips.buttonItems); phi_t3++, phi_t2 += 4) {
        if (gSaveContext.equips.cButtonSlots[phi_t3 - 1] != ITEM_NONE &&
            ((phi_t3 < 4) || CVarGetInteger("gDpadEquips", 0))) {
            phi_t4 = gSaveContext.equips.cButtonSlots[phi_t3 - 1] * 4;

            pauseCtx->itemVtx[phi_t2 + 0].v.ob[0] = pauseCtx->itemVtx[phi_t2 + 2].v.ob[0] =
                pauseCtx->itemVtx[phi_t4].v.ob[0] - 2;

            pauseCtx->itemVtx[phi_t2 + 1].v.ob[0] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[0] =
                pauseCtx->itemVtx[phi_t2 + 0].v.ob[0] + 32;

            pauseCtx->itemVtx[phi_t2 + 0].v.ob[1] = pauseCtx->itemVtx[phi_t2 + 1].v.ob[1] =
                pauseCtx->itemVtx[phi_t4].v.ob[1] + 2;

            pauseCtx->itemVtx[phi_t2 + 2].v.ob[1] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[1] =
                pauseCtx->itemVtx[phi_t2 + 0].v.ob[1] - 32;

            pauseCtx->itemVtx[phi_t2 + 0].v.ob[2] = pauseCtx->itemVtx[phi_t2 + 1].v.ob[2] =
                pauseCtx->itemVtx[phi_t2 + 2].v.ob[2] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[2] = 0;

            pauseCtx->itemVtx[phi_t2 + 0].v.flag = pauseCtx->itemVtx[phi_t2 + 1].v.flag =
                pauseCtx->itemVtx[phi_t2 + 2].v.flag = pauseCtx->itemVtx[phi_t2 + 3].v.flag = 0;

            pauseCtx->itemVtx[phi_t2 + 0].v.tc[0] = pauseCtx->itemVtx[phi_t2 + 0].v.tc[1] =
                pauseCtx->itemVtx[phi_t2 + 1].v.tc[1] = pauseCtx->itemVtx[phi_t2 + 2].v.tc[0] = 0;

            pauseCtx->itemVtx[phi_t2 + 1].v.tc[0] = pauseCtx->itemVtx[phi_t2 + 2].v.tc[1] =
                pauseCtx->itemVtx[phi_t2 + 3].v.tc[0] = pauseCtx->itemVtx[phi_t2 + 3].v.tc[1] = 0x400;

            pauseCtx->itemVtx[phi_t2 + 0].v.cn[0] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[0] =
                pauseCtx->itemVtx[phi_t2 + 2].v.cn[0] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[0] =
                    pauseCtx->itemVtx[phi_t2 + 0].v.cn[1] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[1] =
                        pauseCtx->itemVtx[phi_t2 + 2].v.cn[1] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[1] =
                            pauseCtx->itemVtx[phi_t2 + 0].v.cn[2] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[2] =
                                pauseCtx->itemVtx[phi_t2 + 2].v.cn[2] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[2] = 255;

            pauseCtx->itemVtx[phi_t2 + 0].v.cn[3] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[3] =
                pauseCtx->itemVtx[phi_t2 + 2].v.cn[3] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[3] = pauseCtx->alpha;
        } else {
            pauseCtx->itemVtx[phi_t2 + 0].v.ob[0] = pauseCtx->itemVtx[phi_t2 + 2].v.ob[0] = -300;

            pauseCtx->itemVtx[phi_t2 + 1].v.ob[0] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[0] =
                pauseCtx->itemVtx[phi_t2 + 0].v.ob[0] + 32;

            pauseCtx->itemVtx[phi_t2 + 0].v.ob[1] = pauseCtx->itemVtx[phi_t2 + 1].v.ob[1] = 300;

            pauseCtx->itemVtx[phi_t2 + 2].v.ob[1] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[1] =
                pauseCtx->itemVtx[phi_t2 + 0].v.ob[1] - 32;
        }
    }

    for (phi_t3 = 0; phi_t3 < 7; phi_t3++) {
        phi_t4 = D_8082B11C[phi_t3];

        pauseCtx->itemVtx[phi_t2 + 0].v.ob[0] = pauseCtx->itemVtx[phi_t2 + 2].v.ob[0] =
            pauseCtx->itemVtx[phi_t4].v.ob[0];

        pauseCtx->itemVtx[phi_t2 + 1].v.ob[0] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[0] =
            pauseCtx->itemVtx[phi_t2 + 0].v.ob[0] + 8;

        pauseCtx->itemVtx[phi_t2 + 0].v.ob[1] = pauseCtx->itemVtx[phi_t2 + 1].v.ob[1] =
            pauseCtx->itemVtx[phi_t4].v.ob[1] - 22;

        pauseCtx->itemVtx[phi_t2 + 2].v.ob[1] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[1] =
            pauseCtx->itemVtx[phi_t2 + 0].v.ob[1] - 8;

        pauseCtx->itemVtx[phi_t2 + 4].v.ob[0] = pauseCtx->itemVtx[phi_t2 + 6].v.ob[0] =
            pauseCtx->itemVtx[phi_t2 + 0].v.ob[0] + 6;

        pauseCtx->itemVtx[phi_t2 + 5].v.ob[0] = pauseCtx->itemVtx[phi_t2 + 7].v.ob[0] =
            pauseCtx->itemVtx[phi_t2 + 4].v.ob[0] + 8;

        pauseCtx->itemVtx[phi_t2 + 4].v.ob[1] = pauseCtx->itemVtx[phi_t2 + 5].v.ob[1] =
            pauseCtx->itemVtx[phi_t2 + 0].v.ob[1];

        pauseCtx->itemVtx[phi_t2 + 6].v.ob[1] = pauseCtx->itemVtx[phi_t2 + 7].v.ob[1] =
            pauseCtx->itemVtx[phi_t2 + 4].v.ob[1] - 8;

        for (phi_t4 = 0; phi_t4 < 2; phi_t4++, phi_t2 += 4) {
            pauseCtx->itemVtx[phi_t2 + 0].v.ob[2] = pauseCtx->itemVtx[phi_t2 + 1].v.ob[2] =
                pauseCtx->itemVtx[phi_t2 + 2].v.ob[2] = pauseCtx->itemVtx[phi_t2 + 3].v.ob[2] = 0;

            pauseCtx->itemVtx[phi_t2 + 0].v.flag = pauseCtx->itemVtx[phi_t2 + 1].v.flag =
                pauseCtx->itemVtx[phi_t2 + 2].v.flag = pauseCtx->itemVtx[phi_t2 + 3].v.flag = 0;

            pauseCtx->itemVtx[phi_t2 + 0].v.tc[0] = pauseCtx->itemVtx[phi_t2 + 0].v.tc[1] =
                pauseCtx->itemVtx[phi_t2 + 1].v.tc[1] = pauseCtx->itemVtx[phi_t2 + 2].v.tc[0] = 0;

            pauseCtx->itemVtx[phi_t2 + 1].v.tc[0] = pauseCtx->itemVtx[phi_t2 + 2].v.tc[1] =
                pauseCtx->itemVtx[phi_t2 + 3].v.tc[0] = pauseCtx->itemVtx[phi_t2 + 3].v.tc[1] = 0x100;

            pauseCtx->itemVtx[phi_t2 + 0].v.cn[0] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[0] =
                pauseCtx->itemVtx[phi_t2 + 2].v.cn[0] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[0] =
                    pauseCtx->itemVtx[phi_t2 + 0].v.cn[1] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[1] =
                        pauseCtx->itemVtx[phi_t2 + 2].v.cn[1] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[1] =
                            pauseCtx->itemVtx[phi_t2 + 0].v.cn[2] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[2] =
                                pauseCtx->itemVtx[phi_t2 + 2].v.cn[2] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[2] = 255;

            pauseCtx->itemVtx[phi_t2 + 0].v.cn[3] = pauseCtx->itemVtx[phi_t2 + 1].v.cn[3] =
                pauseCtx->itemVtx[phi_t2 + 2].v.cn[3] = pauseCtx->itemVtx[phi_t2 + 3].v.cn[3] = pauseCtx->alpha;
        }
    }

    pauseCtx->equipVtx = Graph_Alloc(gfxCtx, 112 * sizeof(Vtx));

    for (phi_t4 = 0, phi_t2 = 0, phi_t5 = 58; phi_t2 < 4; phi_t2++, phi_t5 -= 32) {
        for (phi_t3 = 0; phi_t3 < 4; phi_t3++, phi_t4 += 4) {
            pauseCtx->equipVtx[phi_t4 + 0].v.ob[0] = pauseCtx->equipVtx[phi_t4 + 2].v.ob[0] = D_8082B12C[phi_t3] + 2;

            pauseCtx->equipVtx[phi_t4 + 1].v.ob[0] = pauseCtx->equipVtx[phi_t4 + 3].v.ob[0] =
                pauseCtx->equipVtx[phi_t4 + 0].v.ob[0] + 28;

            pauseCtx->equipVtx[phi_t4 + 0].v.ob[1] = pauseCtx->equipVtx[phi_t4 + 1].v.ob[1] =
                phi_t5 + pauseCtx->offsetY - 2;

            pauseCtx->equipVtx[phi_t4 + 2].v.ob[1] = pauseCtx->equipVtx[phi_t4 + 3].v.ob[1] =
                pauseCtx->equipVtx[phi_t4 + 0].v.ob[1] - 28;

            pauseCtx->equipVtx[phi_t4 + 0].v.ob[2] = pauseCtx->equipVtx[phi_t4 + 1].v.ob[2] =
                pauseCtx->equipVtx[phi_t4 + 2].v.ob[2] = pauseCtx->equipVtx[phi_t4 + 3].v.ob[2] = 0;

            pauseCtx->equipVtx[phi_t4 + 0].v.flag = pauseCtx->equipVtx[phi_t4 + 1].v.flag =
                pauseCtx->equipVtx[phi_t4 + 2].v.flag = pauseCtx->equipVtx[phi_t4 + 3].v.flag = 0;

            pauseCtx->equipVtx[phi_t4 + 0].v.tc[0] = pauseCtx->equipVtx[phi_t4 + 0].v.tc[1] =
                pauseCtx->equipVtx[phi_t4 + 1].v.tc[1] = pauseCtx->equipVtx[phi_t4 + 2].v.tc[0] = 0;

            pauseCtx->equipVtx[phi_t4 + 1].v.tc[0] = pauseCtx->equipVtx[phi_t4 + 2].v.tc[1] =
                pauseCtx->equipVtx[phi_t4 + 3].v.tc[0] = pauseCtx->equipVtx[phi_t4 + 3].v.tc[1] = 0x400;

            pauseCtx->equipVtx[phi_t4 + 0].v.cn[0] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[0] =
                pauseCtx->equipVtx[phi_t4 + 2].v.cn[0] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[0] =
                    pauseCtx->equipVtx[phi_t4 + 0].v.cn[1] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[1] =
                        pauseCtx->equipVtx[phi_t4 + 2].v.cn[1] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[1] =
                            pauseCtx->equipVtx[phi_t4 + 0].v.cn[2] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[2] =
                                pauseCtx->equipVtx[phi_t4 + 2].v.cn[2] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[2] = 255;

            pauseCtx->equipVtx[phi_t4 + 0].v.cn[3] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[3] =
                pauseCtx->equipVtx[phi_t4 + 2].v.cn[3] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[3] = pauseCtx->alpha;
        }
    }

    for (phi_t3 = 0; phi_t3 < 4; phi_t3++, phi_t4 += 4) {
        if (CUR_EQUIP_VALUE(phi_t3) != 0) {
            phi_t2 = (CUR_EQUIP_VALUE(phi_t3) + D_8082B134[phi_t3] - 1) * 4;

            pauseCtx->equipVtx[phi_t4 + 0].v.ob[0] = pauseCtx->equipVtx[phi_t4 + 2].v.ob[0] =
                pauseCtx->equipVtx[phi_t2].v.ob[0] - 2;

            pauseCtx->equipVtx[phi_t4 + 1].v.ob[0] = pauseCtx->equipVtx[phi_t4 + 3].v.ob[0] =
                pauseCtx->equipVtx[phi_t4 + 0].v.ob[0] + 32;

            pauseCtx->equipVtx[phi_t4 + 0].v.ob[1] = pauseCtx->equipVtx[phi_t4 + 1].v.ob[1] =
                pauseCtx->equipVtx[phi_t2].v.ob[1] + 2;

            pauseCtx->equipVtx[phi_t4 + 2].v.ob[1] = pauseCtx->equipVtx[phi_t4 + 3].v.ob[1] =
                pauseCtx->equipVtx[phi_t4 + 0].v.ob[1] - 32;

            pauseCtx->equipVtx[phi_t4 + 0].v.ob[2] = pauseCtx->equipVtx[phi_t4 + 1].v.ob[2] =
                pauseCtx->equipVtx[phi_t4 + 2].v.ob[2] = pauseCtx->equipVtx[phi_t4 + 3].v.ob[2] = 0;

            pauseCtx->equipVtx[phi_t4 + 0].v.flag = pauseCtx->equipVtx[phi_t4 + 1].v.flag =
                pauseCtx->equipVtx[phi_t4 + 2].v.flag = pauseCtx->equipVtx[phi_t4 + 3].v.flag = 0;

            pauseCtx->equipVtx[phi_t4 + 0].v.tc[0] = pauseCtx->equipVtx[phi_t4 + 0].v.tc[1] =
                pauseCtx->equipVtx[phi_t4 + 1].v.tc[1] = pauseCtx->equipVtx[phi_t4 + 2].v.tc[0] = 0;

            pauseCtx->equipVtx[phi_t4 + 1].v.tc[0] = pauseCtx->equipVtx[phi_t4 + 2].v.tc[1] =
                pauseCtx->equipVtx[phi_t4 + 3].v.tc[0] = pauseCtx->equipVtx[phi_t4 + 3].v.tc[1] = 0x400;

            pauseCtx->equipVtx[phi_t4 + 0].v.cn[0] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[0] =
                pauseCtx->equipVtx[phi_t4 + 2].v.cn[0] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[0] =
                    pauseCtx->equipVtx[phi_t4 + 0].v.cn[1] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[1] =
                        pauseCtx->equipVtx[phi_t4 + 2].v.cn[1] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[1] =
                            pauseCtx->equipVtx[phi_t4 + 0].v.cn[2] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[2] =
                                pauseCtx->equipVtx[phi_t4 + 2].v.cn[2] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[2] = 255;

            pauseCtx->equipVtx[phi_t4 + 0].v.cn[3] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[3] =
                pauseCtx->equipVtx[phi_t4 + 2].v.cn[3] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[3] = pauseCtx->alpha;
        }
    }

    phi_t1 = 112;
    phi_t5 = 50;
    while (true) {
        pauseCtx->equipVtx[phi_t4 + 0].v.ob[0] = pauseCtx->equipVtx[phi_t4 + 2].v.ob[0] = -64;

        pauseCtx->equipVtx[phi_t4 + 1].v.ob[0] = pauseCtx->equipVtx[phi_t4 + 3].v.ob[0] =
            pauseCtx->equipVtx[phi_t4 + 0].v.ob[0] + 64;

        pauseCtx->equipVtx[phi_t4 + 0].v.ob[1] = pauseCtx->equipVtx[phi_t4 + 1].v.ob[1] = phi_t5 + pauseCtx->offsetY;

        pauseCtx->equipVtx[phi_t4 + 2].v.ob[1] = pauseCtx->equipVtx[phi_t4 + 3].v.ob[1] =
            pauseCtx->equipVtx[phi_t4 + 0].v.ob[1] - 32;

        pauseCtx->equipVtx[phi_t4 + 0].v.ob[2] = pauseCtx->equipVtx[phi_t4 + 1].v.ob[2] =
            pauseCtx->equipVtx[phi_t4 + 2].v.ob[2] = pauseCtx->equipVtx[phi_t4 + 3].v.ob[2] = 0;

        pauseCtx->equipVtx[phi_t4 + 0].v.flag = pauseCtx->equipVtx[phi_t4 + 1].v.flag =
            pauseCtx->equipVtx[phi_t4 + 2].v.flag = pauseCtx->equipVtx[phi_t4 + 3].v.flag = 0;

        pauseCtx->equipVtx[phi_t4 + 0].v.tc[0] = pauseCtx->equipVtx[phi_t4 + 0].v.tc[1] =
            pauseCtx->equipVtx[phi_t4 + 1].v.tc[1] = pauseCtx->equipVtx[phi_t4 + 2].v.tc[0] = 0;

        pauseCtx->equipVtx[phi_t4 + 1].v.tc[0] = pauseCtx->equipVtx[phi_t4 + 3].v.tc[0] = 0x800;

        pauseCtx->equipVtx[phi_t4 + 2].v.tc[1] = pauseCtx->equipVtx[phi_t4 + 3].v.tc[1] = 0x400;

        pauseCtx->equipVtx[phi_t4 + 0].v.cn[0] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[0] =
            pauseCtx->equipVtx[phi_t4 + 2].v.cn[0] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[0] =
                pauseCtx->equipVtx[phi_t4 + 0].v.cn[1] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[1] =
                    pauseCtx->equipVtx[phi_t4 + 2].v.cn[1] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[1] =
                        pauseCtx->equipVtx[phi_t4 + 0].v.cn[2] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[2] =
                            pauseCtx->equipVtx[phi_t4 + 2].v.cn[2] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[2] = 255;

        pauseCtx->equipVtx[phi_t4 + 0].v.cn[3] = pauseCtx->equipVtx[phi_t4 + 1].v.cn[3] =
            pauseCtx->equipVtx[phi_t4 + 2].v.cn[3] = pauseCtx->equipVtx[phi_t4 + 3].v.cn[3] = pauseCtx->alpha;

        phi_t1 -= 32;
        phi_t5 -= 32;
        if (phi_t1 < 0) {
            pauseCtx->equipVtx[phi_t4 + 2].v.ob[1] = pauseCtx->equipVtx[phi_t4 + 3].v.ob[1] =
                pauseCtx->equipVtx[phi_t4 + 0].v.ob[1] - 0x10;

            pauseCtx->equipVtx[phi_t4 + 2].v.tc[1] = pauseCtx->equipVtx[phi_t4 + 3].v.tc[1] = 0x200;
            break;
        }

        phi_t4 += 4;
    }

    pauseCtx->questVtx = Graph_Alloc(gfxCtx, 188 * sizeof(Vtx));

    for (phi_t4 = 0, phi_t3 = 0; phi_t3 < 47; phi_t3++, phi_t4 += 4) {
        phi_t2_2 = D_8082B1F8[phi_t3];

        if ((phi_t3 < 6) || (phi_t3 >= 41)) {
            pauseCtx->questVtx[phi_t4 + 0].v.ob[0] = pauseCtx->questVtx[phi_t4 + 2].v.ob[0] = D_8082B138[phi_t3];

            pauseCtx->questVtx[phi_t4 + 1].v.ob[0] = pauseCtx->questVtx[phi_t4 + 3].v.ob[0] =
                pauseCtx->questVtx[phi_t4 + 0].v.ob[0] + D_8082B1F8[phi_t3];

            pauseCtx->questVtx[phi_t4 + 0].v.ob[1] = pauseCtx->questVtx[phi_t4 + 1].v.ob[1] =
                D_8082B198[phi_t3] + pauseCtx->offsetY;

            pauseCtx->questVtx[phi_t4 + 2].v.ob[1] = pauseCtx->questVtx[phi_t4 + 3].v.ob[1] =
                pauseCtx->questVtx[phi_t4 + 0].v.ob[1] - D_8082B1F8[phi_t3];

            if (phi_t3 >= 41) {
                pauseCtx->questVtx[phi_t4 + 1].v.ob[0] = pauseCtx->questVtx[phi_t4 + 3].v.ob[0] =
                    pauseCtx->questVtx[phi_t4 + 0].v.ob[0] + 8;

                pauseCtx->questVtx[phi_t4 + 0].v.ob[1] = pauseCtx->questVtx[phi_t4 + 1].v.ob[1] =
                    D_8082B198[phi_t3] + pauseCtx->offsetY - 6;

                pauseCtx->questVtx[phi_t4 + 2].v.ob[1] = pauseCtx->questVtx[phi_t4 + 3].v.ob[1] =
                    pauseCtx->questVtx[phi_t4 + 0].v.ob[1] - 16;

                phi_t2_2 = 8;
            }
        } else {
            if ((phi_t3 >= 6) && (phi_t3 <= 17)) {
                phi_t2_2 = 16;
            }

            pauseCtx->questVtx[phi_t4 + 0].v.ob[0] = pauseCtx->questVtx[phi_t4 + 2].v.ob[0] = D_8082B138[phi_t3] + 2;

            pauseCtx->questVtx[phi_t4 + 1].v.ob[0] = pauseCtx->questVtx[phi_t4 + 3].v.ob[0] =
                pauseCtx->questVtx[phi_t4 + 0].v.ob[0] + phi_t2_2 - 4;

            pauseCtx->questVtx[phi_t4 + 0].v.ob[1] = pauseCtx->questVtx[phi_t4 + 1].v.ob[1] =
                D_8082B198[phi_t3] + pauseCtx->offsetY - 2;

            pauseCtx->questVtx[phi_t4 + 2].v.ob[1] = pauseCtx->questVtx[phi_t4 + 3].v.ob[1] =
                pauseCtx->questVtx[phi_t4 + 0].v.ob[1] - D_8082B1F8[phi_t3] + 4;
        }

        pauseCtx->questVtx[phi_t4 + 0].v.ob[2] = pauseCtx->questVtx[phi_t4 + 1].v.ob[2] =
            pauseCtx->questVtx[phi_t4 + 2].v.ob[2] = pauseCtx->questVtx[phi_t4 + 3].v.ob[2] = 0;

        pauseCtx->questVtx[phi_t4 + 0].v.flag = pauseCtx->questVtx[phi_t4 + 1].v.flag =
            pauseCtx->questVtx[phi_t4 + 2].v.flag = pauseCtx->questVtx[phi_t4 + 3].v.flag = 0;

        pauseCtx->questVtx[phi_t4 + 0].v.tc[0] = pauseCtx->questVtx[phi_t4 + 0].v.tc[1] =
            pauseCtx->questVtx[phi_t4 + 1].v.tc[1] = pauseCtx->questVtx[phi_t4 + 2].v.tc[0] = 0;

        pauseCtx->questVtx[phi_t4 + 1].v.tc[0] = pauseCtx->questVtx[phi_t4 + 3].v.tc[0] = phi_t2_2 << 5;
        pauseCtx->questVtx[phi_t4 + 2].v.tc[1] = pauseCtx->questVtx[phi_t4 + 3].v.tc[1] = D_8082B1F8[phi_t3] << 5;

        pauseCtx->questVtx[phi_t4 + 0].v.cn[0] = pauseCtx->questVtx[phi_t4 + 1].v.cn[0] =
            pauseCtx->questVtx[phi_t4 + 2].v.cn[0] = pauseCtx->questVtx[phi_t4 + 3].v.cn[0] =
                pauseCtx->questVtx[phi_t4 + 0].v.cn[1] = pauseCtx->questVtx[phi_t4 + 1].v.cn[1] =
                    pauseCtx->questVtx[phi_t4 + 2].v.cn[1] = pauseCtx->questVtx[phi_t4 + 3].v.cn[1] =
                        pauseCtx->questVtx[phi_t4 + 0].v.cn[2] = pauseCtx->questVtx[phi_t4 + 1].v.cn[2] =
                            pauseCtx->questVtx[phi_t4 + 2].v.cn[2] = pauseCtx->questVtx[phi_t4 + 3].v.cn[2] = 255;

        pauseCtx->questVtx[phi_t4 + 0].v.cn[3] = pauseCtx->questVtx[phi_t4 + 1].v.cn[3] =
            pauseCtx->questVtx[phi_t4 + 2].v.cn[3] = pauseCtx->questVtx[phi_t4 + 3].v.cn[3] = pauseCtx->alpha;
    }

    pauseCtx->infoPanelVtx = Graph_Alloc(gfxCtx, 28 * sizeof(Vtx));

    pauseCtx->saveVtx = Graph_Alloc(gfxCtx, 80 * sizeof(Vtx));
    func_80823A0C(play, pauseCtx->saveVtx, 5, 5);
}

void KaleidoScope_DrawGameOver(PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL_39Opa(gfxCtx);

    gDPSetCycleType(POLY_KAL_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_KAL_DISP++, G_RM_PASS, G_RM_XLU_SURF2);
    gDPSetCombineLERP(POLY_KAL_DISP++, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, 0, 0, 0, TEXEL0, PRIMITIVE, ENVIRONMENT,
                      COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);

    gDPSetPrimColor(POLY_KAL_DISP++, 0, 80, D_8082AB8C, D_8082AB90, D_8082AB94, D_8082AB98);
    gDPSetEnvColor(POLY_KAL_DISP++, D_8082AB9C, D_8082ABA0, D_8082ABA4, 255);

    VREG(89) -= 2;

    gDPLoadMultiBlock(POLY_KAL_DISP++, gGameOverP1Tex, 0, 0, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 32, 0,
                      G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                      G_TX_NOLOD);

    gDPLoadMultiBlock(POLY_KAL_DISP++, gGameOverMaskTex, 256, 1, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 32, 0,
                      G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, 5, G_TX_NOLOD, G_TX_NOLOD);

    gDPSetTileSize(POLY_KAL_DISP++, 1, 0, VREG(89) & 0x7F, 252, (VREG(89) & 0x7F) + 0x7C);

    gSPTextureRectangle(POLY_KAL_DISP++, VREG(87) << 2, VREG(88) << 2, (VREG(87) + 64) << 2, (VREG(88) + 32) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    gDPLoadMultiBlock(POLY_KAL_DISP++, gGameOverP2Tex, 0, 0, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 32, 0,
                      G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                      G_TX_NOLOD);

    gSPTextureRectangle(POLY_KAL_DISP++, (VREG(87) + 64) << 2, VREG(88) << 2, (VREG(87) + 128) << 2,
                        (VREG(88) + 32) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    gDPLoadMultiBlock(POLY_KAL_DISP++, gGameOverP3Tex, 0, 0, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 32, 0,
                      G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                      G_TX_NOLOD);
    gSPTextureRectangle(POLY_KAL_DISP++, (VREG(87) + 128) << 2, VREG(88) << 2, (VREG(87) + 192) << 2,
                        (VREG(88) + 32) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    CLOSE_DISPS(gfxCtx);
}

void KaleidoScope_Draw(PlayState* play) {
    Input* input = &play->state.input[0];
    PauseContext* pauseCtx = &play->pauseCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    OPEN_DISPS(play->state.gfxCtx);

    pauseCtx->stickRelX = input->rel.stick_x;
    pauseCtx->stickRelY = input->rel.stick_y;

    //gSPSegment(POLY_KAL_DISP++, 0x02, interfaceCtx->parameterSegment);
    gSPSegment(POLY_KAL_DISP++, 0x07, pauseCtx->playerSegment);
    //gSPSegment(POLY_KAL_DISP++, 0x08, pauseCtx->iconItemSegment);
    //gSPSegment(POLY_KAL_DISP++, 0x09, pauseCtx->iconItem24Segment);
    gSPSegment(POLY_KAL_DISP++, 0x0A, pauseCtx->nameSegment);
    //gSPSegment(POLY_KAL_DISP++, 0x0C, pauseCtx->iconItemAltSegment);
    //gSPSegment(POLY_KAL_DISP++, 0x0D, pauseCtx->iconItemLangSegment);

    if (pauseCtx->debugState == 0)
    {
        KaleidoScope_SetView(pauseCtx, pauseCtx->eye.x, pauseCtx->eye.y, pauseCtx->eye.z);

        Gfx_SetupDL_42Kal(play->state.gfxCtx);
        KaleidoScope_InitVertices(play, play->state.gfxCtx);
        KaleidoScope_DrawPages(play, play->state.gfxCtx);

        Gfx_SetupDL_42Kal(play->state.gfxCtx);
        gDPSetCombineLERP(POLY_KAL_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        KaleidoScope_SetView(pauseCtx, 0.0f, 0.0f, 64.0f);

        if (!((pauseCtx->state >= 8) && (pauseCtx->state <= 0x11))) {
            KaleidoScope_DrawInfoPanel(play);
        }
    }

    if ((pauseCtx->state >= 0xB) && (pauseCtx->state <= 0x11)) {
        KaleidoScope_DrawGameOver(play);
    }

    if ((pauseCtx->debugState == 1) || (pauseCtx->debugState == 2)) {
        KaleidoScope_DrawDebugEditor(play);
    }

    func_800AAA50(&play->view, 15);

    // Flip the OPA and XLU projections again as the set view call above reset the original flips from z_play
    if (CVarGetInteger("gMirroredWorld", 0)) {
        gSPMatrix(POLY_OPA_DISP++, play->view.projectionFlippedPtr, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
        gSPMatrix(POLY_XLU_DISP++, play->view.projectionFlippedPtr, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
        gSPMatrix(POLY_OPA_DISP++, play->view.viewingPtr, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
        gSPMatrix(POLY_XLU_DISP++, play->view.viewingPtr, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

uint32_t _bswap32(uint32_t a)
{
    a = ((a & 0x000000FF) << 24) |
        ((a & 0x0000FF00) << 8) |
        ((a & 0x00FF0000) >> 8) |
        ((a & 0xFF000000) >> 24);
    return a;
}

void func_808265BC(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;

    pauseCtx->eye.x += D_8082ABAC[pauseCtx->mode] * ZREG(46);
    pauseCtx->eye.z += D_8082ABCC[pauseCtx->mode] * ZREG(46);
    pauseCtx->unk_1EA += 4 * ZREG(46);

    if (pauseCtx->unk_1EA == (64 * ZREG(47))) {
        func_80084BF4(play, 1);

        for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
            gSaveContext.buttonStatus[buttonIndex] = D_8082AB6C[pauseCtx->pageIndex][buttonIndex];
        }

        pauseCtx->pageIndex = D_8082ABEC[pauseCtx->mode];

        if ((CVarGetInteger("gAssignableTunicsAndBoots", 0) != 0) && (pauseCtx->pageIndex == PAUSE_EQUIP)) {
            gSaveContext.buttonStatus[1] = BTN_ENABLED;
            gSaveContext.buttonStatus[2] = BTN_ENABLED;
            gSaveContext.buttonStatus[3] = BTN_ENABLED;
            // A-Button is already enabled
            gSaveContext.buttonStatus[5] = BTN_ENABLED;
            gSaveContext.buttonStatus[6] = BTN_ENABLED;
            gSaveContext.buttonStatus[7] = BTN_ENABLED;
            gSaveContext.buttonStatus[8] = BTN_ENABLED;
        }

        pauseCtx->unk_1E4 = 0;
        pauseCtx->state++;
        pauseCtx->alpha = 255;
        Interface_LoadActionLabelB(play, DO_ACTION_SAVE);
    } else if (pauseCtx->unk_1EA == 64) {
        pauseCtx->pageIndex = D_8082ABEC[pauseCtx->mode];
        pauseCtx->mode = (u16)(pauseCtx->pageIndex * 2) + 1;
    }
}

void KaleidoScope_UpdateCursorSize(PauseContext* pauseCtx) {
    s32 temp1;
    s32 temp2;
    s32 temp3;
    s32 temp4;

    if (pauseCtx->cursorSpecialPos == 0) {
        temp1 = -1;
        temp2 = 1;
        temp3 = 14;
        temp4 = 14;
        if (pauseCtx->pageIndex == PAUSE_MAP) {
            if (!sInDungeonScene) {
                temp1 = -6;
                temp2 = 6;
                temp3 = 4;
                temp4 = 4;
            } else if (pauseCtx->cursorSlot[pauseCtx->pageIndex] >= 3) {
                temp1 = -6;
                temp2 = 5;
                temp4 = 7;
                temp3 = 19;
            } else {
                temp1 = -3;
                temp2 = 3;
                temp3 = 13;
                temp4 = 13;
            }
        } else if (pauseCtx->pageIndex == PAUSE_QUEST) {
            temp1 = -4;
            temp2 = 4;
            temp3 = 12;
            temp4 = 12;
            if (pauseCtx->cursorSlot[pauseCtx->pageIndex] == 0x18) {
                temp1 = -2;
                temp2 = 2;
                temp3 = 32;
                temp4 = 32;
            } else if (pauseCtx->cursorSlot[pauseCtx->pageIndex] == 0x17) {
                temp1 = -4;
                temp2 = 4;
                temp4 = 13;
                temp3 = 34;
            } else if (pauseCtx->cursorSlot[pauseCtx->pageIndex] < 6) {
                temp1 = -1;
                temp2 = 1;
                temp3 = 10;
                temp4 = 10;
            } else if ((pauseCtx->cursorSlot[pauseCtx->pageIndex] >= 6) &&
                       (pauseCtx->cursorSlot[pauseCtx->pageIndex] < 0x12)) {
                temp1 = -5;
                temp2 = 3;
                temp3 = 8;
                temp4 = 8;
            }
        }
    } else {
        temp1 = -4;
        temp2 = 4;
        temp3 = 16;
        temp4 = 16;
    }

    pauseCtx->cursorVtx[0].v.ob[0] = pauseCtx->cursorVtx[2].v.ob[0] = pauseCtx->cursorVtx[0].v.ob[0] + temp1;
    pauseCtx->cursorVtx[1].v.ob[0] = pauseCtx->cursorVtx[3].v.ob[0] = pauseCtx->cursorVtx[0].v.ob[0] + 16;
    pauseCtx->cursorVtx[0].v.ob[1] = pauseCtx->cursorVtx[1].v.ob[1] = pauseCtx->cursorVtx[0].v.ob[1] + temp2;
    pauseCtx->cursorVtx[2].v.ob[1] = pauseCtx->cursorVtx[3].v.ob[1] = pauseCtx->cursorVtx[0].v.ob[1] - 16;

    pauseCtx->cursorVtx[4].v.ob[0] = pauseCtx->cursorVtx[6].v.ob[0] = pauseCtx->cursorVtx[0].v.ob[0] + temp3;
    pauseCtx->cursorVtx[5].v.ob[0] = pauseCtx->cursorVtx[7].v.ob[0] = pauseCtx->cursorVtx[4].v.ob[0] + 16;
    pauseCtx->cursorVtx[4].v.ob[1] = pauseCtx->cursorVtx[5].v.ob[1] = pauseCtx->cursorVtx[0].v.ob[1];
    pauseCtx->cursorVtx[6].v.ob[1] = pauseCtx->cursorVtx[7].v.ob[1] = pauseCtx->cursorVtx[4].v.ob[1] - 16;

    pauseCtx->cursorVtx[8].v.ob[0] = pauseCtx->cursorVtx[10].v.ob[0] = pauseCtx->cursorVtx[0].v.ob[0];
    pauseCtx->cursorVtx[9].v.ob[0] = pauseCtx->cursorVtx[11].v.ob[0] = pauseCtx->cursorVtx[8].v.ob[0] + 16;
    pauseCtx->cursorVtx[8].v.ob[1] = pauseCtx->cursorVtx[9].v.ob[1] = pauseCtx->cursorVtx[0].v.ob[1] - temp4;
    pauseCtx->cursorVtx[10].v.ob[1] = pauseCtx->cursorVtx[11].v.ob[1] = pauseCtx->cursorVtx[8].v.ob[1] - 16;

    pauseCtx->cursorVtx[12].v.ob[0] = pauseCtx->cursorVtx[14].v.ob[0] = pauseCtx->cursorVtx[0].v.ob[0] + temp3;
    pauseCtx->cursorVtx[13].v.ob[0] = pauseCtx->cursorVtx[15].v.ob[0] = pauseCtx->cursorVtx[12].v.ob[0] + 16;
    pauseCtx->cursorVtx[12].v.ob[1] = pauseCtx->cursorVtx[13].v.ob[1] = pauseCtx->cursorVtx[0].v.ob[1] - temp4;
    pauseCtx->cursorVtx[14].v.ob[1] = pauseCtx->cursorVtx[15].v.ob[1] = pauseCtx->cursorVtx[12].v.ob[1] - 16;
}

void KaleidoScope_LoadDungeonMap(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    interfaceCtx->mapSegmentName[0] = sDungeonMapTexs[R_MAP_TEX_INDEX];
    interfaceCtx->mapSegmentName[1] = sDungeonMapTexs[R_MAP_TEX_INDEX + 1];
    interfaceCtx->mapSegment[0] = ResourceGetDataByName(sDungeonMapTexs[R_MAP_TEX_INDEX]);
    interfaceCtx->mapSegment[1] = ResourceGetDataByName(sDungeonMapTexs[R_MAP_TEX_INDEX + 1]);
}

void KaleidoScope_UpdateDungeonMap(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    osSyncPrintf("ＭＡＰ ＤＭＡ = %d\n", play->interfaceCtx.mapPaletteIndex);

    KaleidoScope_LoadDungeonMap(play);
    Map_SetFloorPalettesData(play, pauseCtx->dungeonMapSlot - 3);

    if ((play->sceneNum >= SCENE_DEKU_TREE) && (play->sceneNum <= SCENE_TREASURE_BOX_SHOP)) {
        if ((VREG(30) + 3) == pauseCtx->cursorPoint[PAUSE_MAP]) {
            // HDTODO: Handle Runtime Modified Textures (HD)
            KaleidoScope_OverridePalIndexCI4(interfaceCtx->mapSegment[0], 2040, interfaceCtx->mapPaletteIndex, 14);
        }
    }

    if ((play->sceneNum >= SCENE_DEKU_TREE) && (play->sceneNum <= SCENE_TREASURE_BOX_SHOP)) {
        if ((VREG(30) + 3) == pauseCtx->cursorPoint[PAUSE_MAP]) {
            // HDTODO: Handle Runtime Modified Textures (HD)
            KaleidoScope_OverridePalIndexCI4(interfaceCtx->mapSegment[1], 2040, interfaceCtx->mapPaletteIndex, 14);
        }
    }
}

void KaleidoScope_Update(PlayState* play)
{
    static s16 D_8082B258 = 0;
    static s16 D_8082B25C = 10;
    static s16 D_8082B260 = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    GameOverContext* gameOverCtx = &play->gameOverCtx;
    Player* player = GET_PLAYER(play);
    Input* input = &play->state.input[0];
    size_t size = 0;
    size_t size0 = 0;
    size_t size1 = 0;
    size_t size2 = 0;
    u16 i = 0;
    s16 stepR;
    s16 stepG;
    s16 stepB;
    s16 stepA;
    s32 pad;

    if ((R_PAUSE_MENU_MODE >= 3) && (((pauseCtx->state >= 4) && (pauseCtx->state <= 7)) ||
                                     ((pauseCtx->state >= 0xA) && (pauseCtx->state <= 0x12)))) {

        if ((!pauseCtx->unk_1E4 || (pauseCtx->unk_1E4 == 8)) && (pauseCtx->state == 6)) {
            pauseCtx->stickRelX = input->rel.stick_x;
            pauseCtx->stickRelY = input->rel.stick_y;
            KaleidoScope_UpdateCursorSize(&play->pauseCtx);
            KaleidoScope_HandlePageToggles(pauseCtx, input);
        } else if ((pauseCtx->pageIndex == PAUSE_QUEST) && ((pauseCtx->unk_1E4 < 3) || (pauseCtx->unk_1E4 == 5))) {
            KaleidoScope_UpdateCursorSize(&play->pauseCtx);
        }

        if (pauseCtx->state == 6) {
            KaleidoScope_UpdateNamePanel(play);
        }
    }

    switch (pauseCtx->state) {
        case 3:
            for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
                sButtonStatusSave[buttonIndex] = gSaveContext.buttonStatus[buttonIndex];
            }

            pauseCtx->cursorX[PAUSE_MAP] = 0;
            pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->dungeonMapSlot =
                VREG(30) + 3;

            WREG(16) = -175;
            WREG(17) = 155;

            pauseCtx->unk_204 = -314.0f;

            pauseCtx->playerSegment = (void*)(((uintptr_t)play->objectCtx.spaceStart + 0x30) & ~0x3F);

            size1 = func_80091738(play, pauseCtx->playerSegment, &pauseCtx->playerSkelAnime);
            osSyncPrintf("プレイヤー size1＝%x\n", size1);

            pauseCtx->iconItemSegment = (void*)(((uintptr_t)pauseCtx->playerSegment + size1 + 0xF) & ~0xF);

            size0 = (uintptr_t)_icon_item_staticSegmentRomEnd - (uintptr_t)_icon_item_staticSegmentRomStart;
            osSyncPrintf("icon_item size0=%x\n", size0);
            DmaMgr_SendRequest1(pauseCtx->iconItemSegment, (uintptr_t)_icon_item_staticSegmentRomStart, size0,
                                __FILE__, __LINE__);

            gSegments[8] = VIRTUAL_TO_PHYSICAL(pauseCtx->iconItemSegment);

            pauseCtx->iconItem24Segment = (void*)(((uintptr_t)pauseCtx->iconItemSegment + size0 + 0xF) & ~0xF);

#if 1
            //size = (uintptr_t)_icon_item_24_staticSegmentRomEnd - (uintptr_t)_icon_item_24_staticSegmentRomStart;
            //osSyncPrintf("icon_item24 size=%x\n", size);
            //DmaMgr_SendRequest1(pauseCtx->iconItem24Segment, (uintptr_t)_icon_item_24_staticSegmentRomStart, size,
                                //__FILE__, __LINE__);

            //pauseCtx->iconItemAltSegment = (void*)(((uintptr_t)pauseCtx->iconItem24Segment + size + 0xF) & ~0xF);
#endif

            switch (play->sceneNum) {
                case SCENE_DEKU_TREE:
                case SCENE_DODONGOS_CAVERN:
                case SCENE_JABU_JABU:
                case SCENE_FOREST_TEMPLE:
                case SCENE_FIRE_TEMPLE:
                case SCENE_WATER_TEMPLE:
                case SCENE_SPIRIT_TEMPLE:
                case SCENE_SHADOW_TEMPLE:
                case SCENE_BOTTOM_OF_THE_WELL:
                case SCENE_ICE_CAVERN:
                case SCENE_DEKU_TREE_BOSS:
                case SCENE_DODONGOS_CAVERN_BOSS:
                case SCENE_JABU_JABU_BOSS:
                case SCENE_FOREST_TEMPLE_BOSS:
                case SCENE_FIRE_TEMPLE_BOSS:
                case SCENE_WATER_TEMPLE_BOSS:
                case SCENE_SPIRIT_TEMPLE_BOSS:
                case SCENE_SHADOW_TEMPLE_BOSS:
#if 1
                    sInDungeonScene = true;
                    //size2 = (uintptr_t)_icon_item_dungeon_staticSegmentRomEnd - (uintptr_t)_icon_item_dungeon_staticSegmentRomStart;
                    //osSyncPrintf("icon_item_dungeon dungeon-size2=%x\n", size2);
                    //DmaMgr_SendRequest1(pauseCtx->iconItemAltSegment, (uintptr_t)_icon_item_dungeon_staticSegmentRomStart,
                                        //size2, __FILE__, __LINE__);
#endif

                    interfaceCtx->mapPalette[28] = 6;
                    interfaceCtx->mapPalette[29] = 99;
                    KaleidoScope_UpdateDungeonMap(play);
                    break;

                default:
#if 1
                    sInDungeonScene = false;
                    //size2 = (uintptr_t)_icon_item_field_staticSegmentRomEnd - (uintptr_t)_icon_item_field_staticSegmentRomStart;
                    //osSyncPrintf("icon_item_field field-size2=%x\n", size2);
                    //DmaMgr_SendRequest1(pauseCtx->iconItemAltSegment, (uintptr_t)_icon_item_field_staticSegmentRomStart,
                                        //size2, __FILE__, __LINE__);
#endif
                    break;
            }

            // OTRTODO: LANGUAGE SUPPORT
#if 1
            //pauseCtx->iconItemLangSegment = (void*)(((uintptr_t)pauseCtx->iconItemAltSegment + size2 + 0xF) & ~0xF);

            if (gSaveContext.language == LANGUAGE_ENG) {
                //size = (uintptr_t)_icon_item_nes_staticSegmentRomEnd - (uintptr_t)_icon_item_nes_staticSegmentRomStart;
                //osSyncPrintf("icon_item_dungeon dungeon-size=%x\n", size);
                //DmaMgr_SendRequest1(pauseCtx->iconItemLangSegment, _icon_item_nes_staticSegmentRomStart, size,
                                    //__FILE__, __LINE__);
            } else if (gSaveContext.language == LANGUAGE_GER) {
                //size = (uintptr_t)_icon_item_ger_staticSegmentRomEnd - (uintptr_t)_icon_item_ger_staticSegmentRomStart;
                //osSyncPrintf("icon_item_dungeon dungeon-size=%x\n", size);
                //DmaMgr_SendRequest1(pauseCtx->iconItemLangSegment, (uintptr_t)_icon_item_ger_staticSegmentRomStart, size,
                                    //__FILE__, __LINE__);
            } else {
                //size = (uintptr_t)_icon_item_fra_staticSegmentRomEnd - (uintptr_t)_icon_item_fra_staticSegmentRomStart;
                //osSyncPrintf("icon_item_dungeon dungeon-size=%x\n", size);
                //DmaMgr_SendRequest1(pauseCtx->iconItemLangSegment, (uintptr_t)_icon_item_fra_staticSegmentRomStart, size,
                                    //__FILE__, __LINE__);
            }
#endif

            //pauseCtx->nameSegment = (void*)(((uintptr_t)pauseCtx->iconItemLangSegment + size + 0xF) & ~0xF);
            pauseCtx->nameSegment = malloc(0x400 + 0xA00); // OTRTODO: GET RID OF THIS

            osSyncPrintf("サイズ＝%x\n", size2 + size1 + size0 + size);
            osSyncPrintf("item_name I_N_PT=%x\n", 0x800);
            Interface_SetDoAction(play, DO_ACTION_DECIDE);
            osSyncPrintf("サイズ＝%x\n", size2 + size1 + size0 + size + 0x800);

            if (((void)0, gSaveContext.worldMapArea) < 22) {
                const uint8_t offsets[] = { 36, 58, 80 };
                const char* textureName = mapNameTextures[offsets[gSaveContext.language] + gSaveContext.worldMapArea];
                memcpy(pauseCtx->nameSegment + 0x400, textureName, strlen(textureName) + 1);
            }
            // OTRTODO - player on pause
            #if 1
            // HDTODO: Remove sPreRenderCvg stuff?
            sPreRenderCvg = (void*)(((uintptr_t)pauseCtx->nameSegment + 0x400 + 0xA00 + 0xF) & ~0xF);

            PreRender_Init(&sPlayerPreRender);
            PreRender_SetValuesSave(&sPlayerPreRender, 64, 112, pauseCtx->playerSegment, NULL, sPreRenderCvg);

            KaleidoScope_DrawPlayerWork(play);
            //KaleidoScope_SetupPlayerPreRender(play);
            #endif
            for (i = 0; i < ARRAY_COUNT(pauseCtx->worldMapPoints); i++) {
                pauseCtx->worldMapPoints[i] = 0;
            }

            if (CHECK_QUEST_ITEM(QUEST_GERUDO_CARD)) {
                pauseCtx->worldMapPoints[0] = 2;
            }

            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                pauseCtx->worldMapPoints[0] = 1;
            }

            if (INV_CONTENT(ITEM_LONGSHOT) == ITEM_LONGSHOT) {
                pauseCtx->worldMapPoints[1] = 2;
            }

            if (CHECK_QUEST_ITEM(QUEST_GERUDO_CARD)) {
                pauseCtx->worldMapPoints[1] = 1;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_ENTERED_GERUDO_VALLEY)) {
                pauseCtx->worldMapPoints[2] = 1;
            }

            if (INV_CONTENT(ITEM_LONGSHOT) == ITEM_LONGSHOT) {
                pauseCtx->worldMapPoints[2] = 2;
            }

            if (CHECK_QUEST_ITEM(QUEST_GERUDO_CARD)) {
                pauseCtx->worldMapPoints[2] = 1;
            }

            if (CUR_UPG_VALUE(UPG_SCALE)) {
                pauseCtx->worldMapPoints[3] = 1;
            }

            if (CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON)) {
                pauseCtx->worldMapPoints[3] = 2;
            }

            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER)) {
                pauseCtx->worldMapPoints[3] = 1;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                pauseCtx->worldMapPoints[4] = 1;
            }

            if (INV_CONTENT(ITEM_OCARINA_FAIRY) != ITEM_NONE) {
                pauseCtx->worldMapPoints[4] = 2;
            }

            if (CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
                pauseCtx->worldMapPoints[4] = 1;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_TALON_WOKEN_IN_KAKARIKO)) {
                pauseCtx->worldMapPoints[4] = 2;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_EPONA_OBTAINED)) {
                pauseCtx->worldMapPoints[4] = 1;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                pauseCtx->worldMapPoints[5] = 2;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_ZELDAS_LETTER)) {
                pauseCtx->worldMapPoints[5] = 1;
            }

            if (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME) {
                pauseCtx->worldMapPoints[5] = 2;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL)) {
                pauseCtx->worldMapPoints[5] = 1;
            }

            if (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT) {
                pauseCtx->worldMapPoints[5] = 2;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                pauseCtx->worldMapPoints[6] = 1;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_ZELDAS_LETTER)) {
                pauseCtx->worldMapPoints[7] = 2;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
                pauseCtx->worldMapPoints[7] = 1;
            }

            if (INV_CONTENT(ITEM_HOOKSHOT) == ITEM_HOOKSHOT) {
                pauseCtx->worldMapPoints[7] = 2;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP)) {
                pauseCtx->worldMapPoints[7] = 1;
            }

            if (gBitFlags[1] & gSaveContext.worldMapAreaData) {
                pauseCtx->worldMapPoints[8] = 1;
            }

            if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
                pauseCtx->worldMapPoints[8] = 2;
            }

            if (CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
                pauseCtx->worldMapPoints[8] = 1;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL)) {
                pauseCtx->worldMapPoints[8] = 2;
            }

            if (INV_CONTENT(ITEM_HOOKSHOT) == ITEM_HOOKSHOT) {
                pauseCtx->worldMapPoints[8] = 1;
            }

            if (CHECK_QUEST_ITEM(QUEST_SONG_STORMS)) {
                pauseCtx->worldMapPoints[8] = 2;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_DRAINED_WELL_IN_KAKARIKO)) {
                pauseCtx->worldMapPoints[8] = 1;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL)) {
                pauseCtx->worldMapPoints[8] = 2;
            }

            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
                pauseCtx->worldMapPoints[8] = 1;
            }

            if (gBitFlags[10] & gSaveContext.worldMapAreaData) {
                pauseCtx->worldMapPoints[9] = 1;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_MIDO_ABOUT_SARIA)) {
                pauseCtx->worldMapPoints[9] = 2;
            }

            if (CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
                pauseCtx->worldMapPoints[9] = 1;
            }

            if (INV_CONTENT(ITEM_HOOKSHOT) == ITEM_HOOKSHOT) {
                pauseCtx->worldMapPoints[9] = 2;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP)) {
                pauseCtx->worldMapPoints[9] = 1;
            }

            pauseCtx->worldMapPoints[10] = 2;

            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                pauseCtx->worldMapPoints[10] = 1;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_6E)) {
                pauseCtx->worldMapPoints[10] = 2;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_MIDO_ABOUT_SARIA)) {
                pauseCtx->worldMapPoints[10] = 1;
            }

            if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
                pauseCtx->worldMapPoints[11] = 1;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
                pauseCtx->worldMapPoints[11] = 2;
            }

            if (Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                pauseCtx->worldMapPoints[11] = 1;
            }

            if (INV_CONTENT(ITEM_HOOKSHOT) == ITEM_HOOKSHOT) {
                pauseCtx->worldMapPoints[11] = 2;
            }

            if (CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON)) {
                pauseCtx->worldMapPoints[11] = 1;
            }

            pauseCtx->tradeQuestLocation = 0xFF;

            i = INV_CONTENT(ITEM_TRADE_ADULT);
            if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
                if ((i <= ITEM_POCKET_CUCCO) || (i == ITEM_ODD_MUSHROOM)) {
                    pauseCtx->tradeQuestLocation = 8;
                }
                if ((i == ITEM_COJIRO) || (i == ITEM_ODD_POTION)) {
                    pauseCtx->tradeQuestLocation = 9;
                }
                if (i == ITEM_SAW) {
                    pauseCtx->tradeQuestLocation = 2;
                }
                if ((i == ITEM_SWORD_BROKEN) || (i == ITEM_EYEDROPS)) {
                    pauseCtx->tradeQuestLocation = 7;
                }
                if (i == ITEM_PRESCRIPTION) {
                    pauseCtx->tradeQuestLocation = 11;
                }
                if (i == ITEM_FROG) {
                    pauseCtx->tradeQuestLocation = 3;
                }
                if ((i == ITEM_CLAIM_CHECK) && (gSaveContext.bgsFlag == 0)) {
                    pauseCtx->tradeQuestLocation = 7;
                }
            }

            KaleidoScope_ResetTradeSelect();

            pauseCtx->state = 4;
            break;

        case 4:
            if (pauseCtx->unk_1F4 == 160.0f) {
                KaleidoScope_SetDefaultCursor(play);
                //OTRTODO - Player on pause
                //KaleidoScope_ProcessPlayerPreRender();
            }

            pauseCtx->unk_1F4 = pauseCtx->unk_1F8 = pauseCtx->unk_1FC = pauseCtx->unk_200 -= 160.0f / WREG(6);
            pauseCtx->infoPanelOffsetY += 40 / WREG(6);
            interfaceCtx->startAlpha += 255 / WREG(6);
            WREG(16) += WREG(25) / WREG(6);
            WREG(17) += WREG(26) / WREG(6);
            XREG(5) += 150 / WREG(6);
            pauseCtx->alpha += (u16)(255 / (WREG(6) + WREG(4)));

            if (pauseCtx->unk_1F4 == 0) {
                interfaceCtx->startAlpha = 255;
                WREG(2) = 0;
                pauseCtx->state = 5;
            }

            func_808265BC(play);
            break;

        case 5:
            pauseCtx->alpha += (u16)(255 / (WREG(6) + WREG(4)));
            func_808265BC(play);
            if (pauseCtx->state == 6) {
                KaleidoScope_UpdateNamePanel(play);
            }
            break;

        case 6:
            switch (pauseCtx->unk_1E4) {
                case 0:
                    // Boss Rush skips past the "Save?" window when pressing B while paused.
                    if (CHECK_BTN_ALL(input->press.button, BTN_START) ||
                        (CHECK_BTN_ALL(input->press.button, BTN_B) && IS_BOSS_RUSH)) {
                        if (CVarGetInteger("gCheatEasyPauseBufferEnabled", 0) || CVarGetInteger("gCheatEasyInputBufferingEnabled", 0)) {
                            // Easy pause buffer is 13 frames, 12 for kaledio to end, and one more to advance a single frame
                            CVarSetInteger("gCheatEasyPauseBufferTimer", 13);
                        }
                        Interface_SetDoAction(play, DO_ACTION_NONE);
                        pauseCtx->state = 0x12;
                        WREG(2) = -6240;
                        func_800F64E0(0);
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        pauseCtx->mode = 0;
                        pauseCtx->promptChoice = 0;
                        Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                            gSaveContext.buttonStatus[3] = BTN_DISABLED;
                        gSaveContext.buttonStatus[4] = BTN_ENABLED;
                        gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                            gSaveContext.buttonStatus[8] = BTN_DISABLED;
                        gSaveContext.unk_13EA = 0;
                        Interface_ChangeAlpha(50);
                        pauseCtx->unk_1EC = 0;
                        pauseCtx->state = 7;
                    }
                    break;

                case 1:
                    func_808237B4(play, play->state.input);
                    break;

                case 2:
                    pauseCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
                    if (pauseCtx->ocarinaStaff->state == 0) {
                        pauseCtx->unk_1E4 = 4;
                        Audio_OcaSetInstrument(0);
                    }
                    break;

                case 3:
                    KaleidoScope_UpdateItemEquip(play);
                    break;

                case 4:
                    break;

                case 5:
                    pauseCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();

                    if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
                        Audio_OcaSetInstrument(0);
                        Interface_SetDoAction(play, DO_ACTION_NONE);
                        pauseCtx->state = 0x12;
                        WREG(2) = -6240;
                        func_800F64E0(0);
                        pauseCtx->unk_1E4 = 0;
                        break;
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        Audio_OcaSetInstrument(0);
                        pauseCtx->unk_1E4 = 0;
                        pauseCtx->mode = 0;
                        pauseCtx->promptChoice = 0;
                        Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                            gSaveContext.buttonStatus[3] = BTN_DISABLED;
                        gSaveContext.buttonStatus[4] = BTN_ENABLED;
                        gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                            gSaveContext.buttonStatus[8] = BTN_DISABLED;
                        gSaveContext.unk_13EA = 0;
                        Interface_ChangeAlpha(50);
                        pauseCtx->unk_1EC = 0;
                        pauseCtx->state = 7;
                    } else if (pauseCtx->ocarinaStaff->state == pauseCtx->ocarinaSongIdx) {
                        Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        D_8082B258 = 0;
                        D_8082B25C = 30;
                        pauseCtx->unk_1E4 = 6;
                    } else if (pauseCtx->ocarinaStaff->state == 0xFF) {
                        Audio_PlaySoundGeneral(NA_SE_SY_OCARINA_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        D_8082B258 = 4;
                        D_8082B25C = 20;
                        pauseCtx->unk_1E4 = 6;
                    }
                    break;

                case 6:
                    D_8082B25C--;
                    if (D_8082B25C == 0) {
                        pauseCtx->unk_1E4 = D_8082B258;
                        if (pauseCtx->unk_1E4 == 0) {
                            Audio_OcaSetInstrument(0);
                        }
                    }
                    break;

                case 7:
                    break;

                case 8:
                    if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
                        Audio_OcaSetInstrument(0);
                        Interface_SetDoAction(play, DO_ACTION_NONE);
                        pauseCtx->state = 0x12;
                        WREG(2) = -6240;
                        func_800F64E0(0);
                        pauseCtx->unk_1E4 = 0;
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        Audio_OcaSetInstrument(0);
                        pauseCtx->unk_1E4 = 0;
                        pauseCtx->mode = 0;
                        pauseCtx->promptChoice = 0;
                        Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                            gSaveContext.buttonStatus[3] = BTN_DISABLED;
                        gSaveContext.buttonStatus[4] = BTN_ENABLED;
                        gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                            gSaveContext.buttonStatus[8] = BTN_DISABLED;
                        gSaveContext.unk_13EA = 0;
                        Interface_ChangeAlpha(50);
                        pauseCtx->unk_1EC = 0;
                        pauseCtx->state = 7;
                    }
                    break;

                case 9:
                    break;

                default:
                    pauseCtx->unk_1E4 = 0;
                    break;
            }
            break;

        case 7:
            switch (pauseCtx->unk_1EC) {
                case 0:
                    pauseCtx->unk_204 -= 314.0f / WREG(6);
                    WREG(16) -= WREG(25) / WREG(6);
                    WREG(17) -= WREG(26) / WREG(6);
                    if (pauseCtx->unk_204 <= -628.0f) {
                        pauseCtx->unk_204 = -628.0f;
                        pauseCtx->unk_1EC = 1;
                    }
                    break;

                case 1:
                    if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                        if (pauseCtx->promptChoice != 0) {
                            Interface_SetDoAction(play, DO_ACTION_NONE);
                            gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                                gSaveContext.buttonStatus[3] = BTN_ENABLED;
                            gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                                gSaveContext.buttonStatus[8] = BTN_ENABLED;
                            gSaveContext.unk_13EA = 0;
                            Interface_ChangeAlpha(50);
                            pauseCtx->unk_1EC = 2;
                            WREG(2) = -6240;
                            YREG(8) = pauseCtx->unk_204;
                            func_800F64E0(0);
                        } else {
                            Audio_PlaySoundGeneral(NA_SE_SY_PIECE_OF_HEART, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            Play_PerformSave(play);
                            pauseCtx->unk_1EC = 4;
                            D_8082B25C = CVarGetInteger("gSkipSaveConfirmation", 0) ? 3 /* 0.1 sec */ : 90 /* 3 secs */;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_START) ||
                               CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        Interface_SetDoAction(play, DO_ACTION_NONE);
                        pauseCtx->unk_1EC = 2;
                        WREG(2) = -6240;
                        YREG(8) = pauseCtx->unk_204;
                        func_800F64E0(0);
                        gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                            gSaveContext.buttonStatus[3] = BTN_ENABLED;
                        gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                            gSaveContext.buttonStatus[8] = BTN_ENABLED;
                        gSaveContext.unk_13EA = 0;
                        Interface_ChangeAlpha(50);
                    }
                    break;

                case 4:
                    if (CHECK_BTN_ALL(input->press.button, BTN_B) || CHECK_BTN_ALL(input->press.button, BTN_A) ||
                        CHECK_BTN_ALL(input->press.button, BTN_START) || (--D_8082B25C == 0)) {
                        Interface_SetDoAction(play, DO_ACTION_NONE);
                        gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                            gSaveContext.buttonStatus[3] = BTN_ENABLED;
                        gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                            gSaveContext.buttonStatus[8] = BTN_ENABLED;
                        gSaveContext.unk_13EA = 0;
                        Interface_ChangeAlpha(50);
                        pauseCtx->unk_1EC = 5;
                        WREG(2) = -6240;
                        YREG(8) = pauseCtx->unk_204;
                        func_800F64E0(0);
                    }
                    break;

                case 3:
                case 6:
                    pauseCtx->unk_204 += 314.0f / WREG(6);
                    WREG(16) += WREG(25) / WREG(6);
                    WREG(17) += WREG(26) / WREG(6);
                    if (pauseCtx->unk_204 >= -314.0f) {
                        pauseCtx->state = 6;
                        pauseCtx->unk_1EC = 0;
                        pauseCtx->unk_1F4 = pauseCtx->unk_1F8 = pauseCtx->unk_1FC = pauseCtx->unk_200 = 0.0f;
                        pauseCtx->unk_204 = -314.0f;
                    }
                    break;

                case 2:
                case 5:
                    if (pauseCtx->unk_204 != (YREG(8) + 160.0f)) {
                        pauseCtx->unk_1F4 = pauseCtx->unk_1F8 = pauseCtx->unk_1FC = pauseCtx->unk_200 +=
                            160.0f / WREG(6);
                        pauseCtx->unk_204 += 160.0f / WREG(6);
                        pauseCtx->infoPanelOffsetY -= 40 / WREG(6);
                        WREG(16) -= WREG(25) / WREG(6);
                        WREG(17) -= WREG(26) / WREG(6);
                        XREG(5) -= 150 / WREG(6);
                        pauseCtx->alpha -= (u16)(255 / WREG(6));
                        if (pauseCtx->unk_204 == (YREG(8) + 160.0f)) {
                            pauseCtx->alpha = 0;
                        }
                    } else {
                        pauseCtx->debugState = 0;
                        pauseCtx->state = 0x13;
                        pauseCtx->unk_1F4 = pauseCtx->unk_1F8 = pauseCtx->unk_1FC = pauseCtx->unk_200 = 160.0f;
                        pauseCtx->namedItem = PAUSE_ITEM_NONE;
                        pauseCtx->unk_1E4 = 0;
                        pauseCtx->unk_204 = -434.0f;
                    }
                    break;
            }
            break;

        case 0xA:
            pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->dungeonMapSlot =
                VREG(30) + 3;
            WREG(16) = -175;
            WREG(17) = 155;
            pauseCtx->unk_204 = -434.0f;
            Interface_ChangeAlpha(1);

#if 1
            pauseCtx->iconItemSegment = (void*)(((uintptr_t)play->objectCtx.spaceStart + 0x30) & ~0x3F);
            size0 = (uintptr_t)_icon_item_staticSegmentRomEnd - (uintptr_t)_icon_item_staticSegmentRomStart;
            osSyncPrintf("icon_item size0=%x\n", size0);
            DmaMgr_SendRequest1(pauseCtx->iconItemSegment, (uintptr_t)_icon_item_staticSegmentRomStart, size0,
                                __FILE__, __LINE__);

            pauseCtx->iconItem24Segment = (void*)(((uintptr_t)pauseCtx->iconItemSegment + size0 + 0xF) & ~0xF);
            //size = (uintptr_t)_icon_item_24_staticSegmentRomEnd - (uintptr_t)_icon_item_24_staticSegmentRomStart;
            osSyncPrintf("icon_item24 size=%x\n", size);
            //DmaMgr_SendRequest1(pauseCtx->iconItem24Segment, (uintptr_t)_icon_item_24_staticSegmentRomStart, size,
                                //__FILE__, __LINE__);

            pauseCtx->iconItemAltSegment = (void*)(((uintptr_t)pauseCtx->iconItem24Segment + size + 0xF) & ~0xF);
            //size2 = (uintptr_t)_icon_item_gameover_staticSegmentRomEnd - (uintptr_t)_icon_item_gameover_staticSegmentRomStart;
            //osSyncPrintf("icon_item_dungeon gameover-size2=%x\n", size2);
            //DmaMgr_SendRequest1(pauseCtx->iconItemAltSegment, (uintptr_t)_icon_item_gameover_staticSegmentRomStart, size2,
                                //__FILE__, __LINE__);

            pauseCtx->iconItemLangSegment = (void*)(((uintptr_t)pauseCtx->iconItemAltSegment + size2 + 0xF) & ~0xF);

            /*
            if (gSaveContext.language == LANGUAGE_ENG) {
                size = (uintptr_t)_icon_item_nes_staticSegmentRomEnd - (uintptr_t)_icon_item_nes_staticSegmentRomStart;
                osSyncPrintf("icon_item_dungeon dungeon-size=%x\n", size);
                DmaMgr_SendRequest1(pauseCtx->iconItemLangSegment, (uintptr_t)_icon_item_nes_staticSegmentRomStart, size,
                                    __FILE__, __LINE__);
            } else if (gSaveContext.language == LANGUAGE_GER) {
                size = (uintptr_t)_icon_item_ger_staticSegmentRomEnd - (uintptr_t)_icon_item_ger_staticSegmentRomStart;
                osSyncPrintf("icon_item_dungeon dungeon-size=%x\n", size);
                DmaMgr_SendRequest1(pauseCtx->iconItemLangSegment, (uintptr_t)_icon_item_ger_staticSegmentRomStart, size,
                                    __FILE__, __LINE__);
            } else {
                size = (uintptr_t)_icon_item_fra_staticSegmentRomEnd - (uintptr_t)_icon_item_fra_staticSegmentRomStart;
                osSyncPrintf("icon_item_dungeon dungeon-size=%x\n", size);
                DmaMgr_SendRequest1(pauseCtx->iconItemLangSegment, (uintptr_t)_icon_item_fra_staticSegmentRomStart, size,
                                    __FILE__, __LINE__);
            }
            */
#endif

            D_8082AB8C = 255;
            D_8082AB90 = 130;
            D_8082AB94 = 0;
            D_8082AB98 = 0;
            D_8082AB9C = 30;
            D_8082ABA0 = 0;
            D_8082ABA4 = 0;
            D_8082B260 = 30;
            VREG(88) = 98;
            pauseCtx->promptChoice = 0;
            pauseCtx->state++;
            break;

        case 0xB:
            stepR = ABS(D_8082AB8C - 30) / D_8082B260;
            stepG = ABS(D_8082AB90) / D_8082B260;
            stepB = ABS(D_8082AB94) / D_8082B260;
            stepA = ABS(D_8082AB98 - 255) / D_8082B260;
            if (D_8082AB8C >= 30) {
                D_8082AB8C -= stepR;
            } else {
                D_8082AB8C += stepR;
            }
            if (D_8082AB90 >= 0) {
                D_8082AB90 -= stepG;
            } else {
                D_8082AB90 += stepG;
            }
            if (D_8082AB94 >= 0) {
                D_8082AB94 -= stepB;
            } else {
                D_8082AB94 += stepB;
            }
            if (D_8082AB98 >= 255) {
                D_8082AB98 -= stepA;
            } else {
                D_8082AB98 += stepA;
            }

            stepR = ABS(D_8082AB9C - 255) / D_8082B260;
            stepG = ABS(D_8082ABA0 - 130) / D_8082B260;
            stepB = ABS(D_8082ABA4) / D_8082B260;
            if (D_8082AB9C >= 255) {
                D_8082AB9C -= stepR;
            } else {
                D_8082AB9C += stepR;
            }
            if (D_8082ABA0 >= 130) {
                D_8082ABA0 -= stepG;
            } else {
                D_8082ABA0 += stepG;
            }
            if (D_8082ABA4 >= 0) {
                D_8082ABA4 -= stepB;
            } else {
                D_8082ABA4 += stepB;
            }

            D_8082B260--;
            if (D_8082B260 == 0) {
                D_8082AB8C = 30;
                D_8082AB90 = 0;
                D_8082AB94 = 0;
                D_8082AB98 = 255;

                D_8082AB9C = 255;
                D_8082ABA0 = 130;
                D_8082ABA4 = 0;

                pauseCtx->state++;
                D_8082B260 = 40;
            }
            break;

        case 0xC:
            D_8082B260--;
            if (D_8082B260 == 0) {
                pauseCtx->state = 0xD;
            }
            break;

        case 0xD:
            pauseCtx->unk_1F4 = pauseCtx->unk_1F8 = pauseCtx->unk_1FC = pauseCtx->unk_200 = pauseCtx->unk_204 -=
                160.0f / WREG(6);
            pauseCtx->infoPanelOffsetY += 40 / WREG(6);
            interfaceCtx->startAlpha += 255 / WREG(6);
            VREG(88) -= 3;
            WREG(16) += WREG(25) / WREG(6);
            WREG(17) += WREG(26) / WREG(6);
            XREG(5) += 150 / WREG(6);
            pauseCtx->alpha += (u16)(255 / (WREG(6) + WREG(4)));
            if (pauseCtx->unk_204 < -628.0f) {
                pauseCtx->unk_204 = -628.0f;
                interfaceCtx->startAlpha = 255;
                VREG(88) = 66;
                WREG(2) = 0;
                pauseCtx->alpha = 255;
                if (!IS_BOSS_RUSH) {
                    pauseCtx->state = 0xE;
                } else {
                    pauseCtx->state = 0xF;
                }
                gSaveContext.deaths++;
                if (gSaveContext.deaths > 999) {
                    gSaveContext.deaths = 999;
                }
            }
            osSyncPrintf("kscope->angle_s = %f\n", pauseCtx->unk_204);
            break;

        case 0xE:
            if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                if (pauseCtx->promptChoice != 0) {
                    pauseCtx->promptChoice = 0;
                    Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    pauseCtx->state = 0x10;
                    gameOverCtx->state++;
                } else {
                    Audio_PlaySoundGeneral(NA_SE_SY_PIECE_OF_HEART, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    pauseCtx->promptChoice = 0;
                    Play_SaveSceneFlags(play);
                    gSaveContext.savedSceneNum = play->sceneNum;
                    Save_SaveFile();
                    pauseCtx->state = 0xF;
                    D_8082B25C = CVarGetInteger("gSkipSaveConfirmation", 0) ? 3 /* 0.1 sec */ : 90 /* 3 secs */;
                }
            }
            break;

        case 0xF:
            D_8082B25C--;
            if (D_8082B25C == 0) {
                pauseCtx->state = 0x10;
                gameOverCtx->state++;
            } else if ((D_8082B25C <= 80) &&
                       (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_START))) {
                pauseCtx->state = 0x10;
                gameOverCtx->state++;
                func_800F64E0(0);
            }
            break;

        case 0x10:
            if (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_START)) {
                if (pauseCtx->promptChoice == 0 && !IS_BOSS_RUSH) {
                    Audio_PlaySoundGeneral(NA_SE_SY_PIECE_OF_HEART, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    Play_SaveSceneFlags(play);

                    switch (gSaveContext.entranceIndex) {
                        case 0x0000:
                        case 0x0004:
                        case 0x0028:
                        case 0x0169:
                        case 0x0165:
                        case 0x0010:
                        case 0x0082:
                        case 0x0037:
                        case 0x041B:
                        case 0x0008:
                        case 0x0088:
                        case 0x0486:
                        case 0x0098:
                        case 0x0467:
                        case 0x0179:
                            break;
                        case 0x040F:
                            gSaveContext.entranceIndex = 0x0000;
                            break;
                        case 0x040B:
                            gSaveContext.entranceIndex = 0x0004;
                            break;
                        case 0x0301:
                            gSaveContext.entranceIndex = 0x0028;
                            break;
                        case 0x000C:
                            gSaveContext.entranceIndex = 0x0169;
                            break;
                        case 0x0305:
                            gSaveContext.entranceIndex = 0x0165;
                            break;
                        case 0x0417:
                            gSaveContext.entranceIndex = 0x0010;
                            break;
                        case 0x008D:
                            gSaveContext.entranceIndex = 0x0082;
                            break;
                        case 0x0413:
                            gSaveContext.entranceIndex = 0x0037;
                            break;
                        case 0x041F:
                            gSaveContext.entranceIndex = 0x041B;
                            break;
                    }

                    // In ER, handle overriding the game over respawn entrance
                    if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
                        Entrance_SetGameOverEntrance();
                    }
                } else {
                    Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                }

                pauseCtx->state = 0x11;
            }
            break;

        case 0x11:
            if (interfaceCtx->unk_244 != 255) {
                interfaceCtx->unk_244 += 10;
                if (interfaceCtx->unk_244 >= 255) {
                    interfaceCtx->unk_244 = 255;
                    pauseCtx->state = 0;
                    R_UPDATE_RATE = 3;
                    R_PAUSE_MENU_MODE = 0;
                    func_800981B8(&play->objectCtx);
                    func_800418D0(&play->colCtx, play);
                    if (pauseCtx->promptChoice == 0 && !IS_BOSS_RUSH) {
                        Play_TriggerRespawn(play);
                        gSaveContext.respawnFlag = -2;
                        // In ER, handle death warp to last entrance from grottos
                        if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
                            Grotto_ForceGrottoReturn();
                        }
                        // Reset frame counter to prevent autosave on respawn
                        play->gameplayFrames = 0;
                        gSaveContext.nextTransitionType = 2;
                        gSaveContext.health = CVarGetInteger("gFullHealthSpawn", 0) ? gSaveContext.healthCapacity : 0x30;
                        Audio_QueueSeqCmd(0xF << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0xA);
                        gSaveContext.healthAccumulator = 0;
                        gSaveContext.magicState = MAGIC_STATE_IDLE;
                        gSaveContext.prevMagicState = MAGIC_STATE_IDLE;
                        osSyncPrintf(VT_FGCOL(YELLOW));
                        osSyncPrintf("MAGIC_NOW=%d ", gSaveContext.magic);
                        osSyncPrintf("Z_MAGIC_NOW_NOW=%d   →  ", gSaveContext.magicFillTarget);
                        gSaveContext.magicCapacity = 0;
                        gSaveContext.magicFillTarget = gSaveContext.magic;
                        gSaveContext.magicLevel = gSaveContext.magic = 0;
                        osSyncPrintf("MAGIC_NOW=%d ", gSaveContext.magic);
                        osSyncPrintf("Z_MAGIC_NOW_NOW=%d\n", gSaveContext.magicFillTarget);
                        osSyncPrintf(VT_RST);
                    } else {
                        play->state.running = 0;
                        SET_NEXT_GAMESTATE(&play->state, Opening_Init, OpeningContext);
                        GameInteractor_ExecuteOnExitGame(gSaveContext.fileNum);
                    }
                }
            }
            break;

        case 0x12:
            if (pauseCtx->unk_1F4 != 160.0f) {
                pauseCtx->unk_1F4 = pauseCtx->unk_1F8 = pauseCtx->unk_1FC = pauseCtx->unk_200 += 160.0f / WREG(6);
                pauseCtx->infoPanelOffsetY -= 40 / WREG(6);
                interfaceCtx->startAlpha -= 255 / WREG(6);
                WREG(16) -= WREG(25) / WREG(6);
                WREG(17) -= WREG(26) / WREG(6);
                XREG(5) -= 150 / WREG(6);
                pauseCtx->alpha -= (u16)(255 / WREG(6));
                if (pauseCtx->unk_1F4 == 160.0f) {
                    pauseCtx->alpha = 0;
                }
            } else {
                pauseCtx->debugState = 0;
                pauseCtx->state = 0x13;
                pauseCtx->unk_200 = 160.0f;
                pauseCtx->unk_1FC = 160.0f;
                pauseCtx->unk_1F8 = 160.0f;
                pauseCtx->unk_1F4 = 160.0f;
                pauseCtx->namedItem = PAUSE_ITEM_NONE;
                play->interfaceCtx.startAlpha = 0;
            }
            break;

        case 0x13:
            pauseCtx->state = 0;
            R_UPDATE_RATE = 3;
            R_PAUSE_MENU_MODE = 0;

            CVarSetInteger("gPauseTriforce", 0);

            func_800981B8(&play->objectCtx);
            func_800418D0(&play->colCtx, play);

            switch (play->sceneNum) {
                case SCENE_DEKU_TREE:
                case SCENE_DODONGOS_CAVERN:
                case SCENE_JABU_JABU:
                case SCENE_FOREST_TEMPLE:
                case SCENE_FIRE_TEMPLE:
                case SCENE_WATER_TEMPLE:
                case SCENE_SPIRIT_TEMPLE:
                case SCENE_SHADOW_TEMPLE:
                case SCENE_BOTTOM_OF_THE_WELL:
                case SCENE_ICE_CAVERN:
                case SCENE_DEKU_TREE_BOSS:
                case SCENE_DODONGOS_CAVERN_BOSS:
                case SCENE_JABU_JABU_BOSS:
                case SCENE_FOREST_TEMPLE_BOSS:
                case SCENE_FIRE_TEMPLE_BOSS:
                case SCENE_WATER_TEMPLE_BOSS:
                case SCENE_SPIRIT_TEMPLE_BOSS:
                case SCENE_SHADOW_TEMPLE_BOSS:
                    Map_InitData(play, play->interfaceCtx.mapRoomNum);
                    break;
            }

            for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
                gSaveContext.buttonStatus[buttonIndex] = sButtonStatusSave[buttonIndex];
            }

            // Used to clear swordless temp B after unpause so minigame/epona handling restarts
            Interface_RandoRestoreSwordless();

            interfaceCtx->unk_1FA = interfaceCtx->unk_1FC = 0;
            osSyncPrintf(VT_FGCOL(YELLOW));
            osSyncPrintf("i=%d  LAST_TIME_TYPE=%d\n", i, gSaveContext.unk_13EE);
            gSaveContext.unk_13EA = 0;
            Interface_ChangeAlpha(gSaveContext.unk_13EE);
            player->targetActor = NULL;
            Player_SetEquipmentData(play, player);
            osSyncPrintf(VT_RST);
            break;
    }

    GameInteractor_ExecuteOnKaleidoscopeUpdate(sInDungeonScene);
}
