// NPC soul figurines: an NPC soul is drawn as a tiny, still copy of the NPC it belongs to.
// The models come from the player's own game data at run time, through the same resource paths the
// NPCs' actors use, so they draw anywhere without the NPC's object being loaded in the scene.

#include "npc_soul_figurines.h"

#include "soh/Enhancements/randomizer/npc_souls.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/OTRGlobals.h"
#include "soh/frame_interpolation.h"

#include <algorithm>
#include <cfloat>
#include <memory>
#include <unordered_map>
#include <vector>

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "objects/object_ane/object_ane.h"
#include "objects/object_ani/object_ani.h"
#include "objects/object_aob/object_aob.h"
#include "objects/object_bg/object_bg.h"
#include "objects/object_boj/object_boj.h"
#include "objects/object_daiku/object_daiku.h"
#include "objects/object_ds/object_ds.h"
#include "objects/object_ds2/object_ds2.h"
#include "objects/object_du/object_du.h"
#include "objects/object_dy_obj/object_dy_obj.h"
#include "objects/object_fa/object_fa.h"
#include "objects/object_fish/object_fish.h"
#include "objects/object_fr/object_fr.h"
#include "objects/object_fu/object_fu.h"
#include "objects/object_ge1/object_ge1.h"
#include "objects/object_geldb/object_geldb.h"
#include "objects/object_gla/object_gla.h"
#include "objects/object_gm/object_gm.h"
#include "objects/object_hs/object_hs.h"
#include "objects/object_im/object_im.h"
#include "objects/object_in/object_in.h"
#include "objects/object_js/object_js.h"
#include "objects/object_ka/object_ka.h"
#include "objects/object_km1/object_km1.h"
#include "objects/object_kw1/object_kw1.h"
#include "objects/object_kz/object_kz.h"
#include "objects/object_ma1/object_ma1.h"
#include "objects/object_mastergolon/object_mastergolon.h"
#include "objects/object_masterkokiri/object_masterkokiri.h"
#include "objects/object_masterkokirihead/object_masterkokirihead.h"
#include "objects/object_masterzoora/object_masterzoora.h"
#include "objects/object_md/object_md.h"
#include "objects/object_mk/object_mk.h"
#include "objects/object_mm/object_mm.h"
#include "objects/object_ms/object_ms.h"
#include "objects/object_mu/object_mu.h"
#include "objects/object_nb/object_nb.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"
#include "objects/object_os/object_os.h"
#include "objects/object_os_anime/object_os_anime.h"
#include "objects/object_ossan/object_ossan.h"
#include "objects/object_owl/object_owl.h"
#include "objects/object_ps/object_ps.h"
#include "objects/object_rl/object_rl.h"
#include "objects/object_rs/object_rs.h"
#include "objects/object_ru1/object_ru1.h"
#include "objects/object_sa/object_sa.h"
#include "objects/object_sd/object_sd.h"
#include "objects/object_shopnuts/object_shopnuts.h"
#include "objects/object_skj/object_skj.h"
#include "objects/object_ssh/object_ssh.h"
#include "objects/object_ta/object_ta.h"
#include "objects/object_tk/object_tk.h"
#include "objects/object_toryo/object_toryo.h"
#include "objects/object_ts/object_ts.h"
#include "objects/object_xc/object_xc.h"
#include "objects/object_zl4/object_zl4.h"
#include "objects/object_zo/object_zo.h"
}

namespace {

// The figurine's height in world units when it is drawn as a freestanding item: a miniature, the size of the
// boss soul models.
constexpr f32 kFigurineHeight = 9.0f;
// Freestanding items are drawn at this scale (EnItem00: actor scale 0.015, times 10 for randomizer items).
constexpr f32 kItemScale = 0.15f;
// Pose the figurine in the last frame of its animation instead of a set one.
constexpr f32 kLastFrame = -1.0f;

struct SegmentTexture {
    u8 segment;
    const char* texture;
};

struct SegmentEnvColor {
    u8 segment;
    Color_RGB8 color;
};

struct LimbDList {
    s8 limb; // numbered as limb callbacks number them (the root is 1)
    const char* dList;
};

struct Figurine {
    RandomizerGet soul;
    const char* skeleton;
    const char* animation;
    f32 frame = 0.0f;                       // the frame of `animation` the figurine is posed in, or kLastFrame
    std::vector<SegmentTexture> textures;   // eyes, mouths and skins the NPC binds before drawing
    std::vector<SegmentEnvColor> envColors; // clothes colours the NPC binds before drawing
    LimbDList head = { -1, nullptr };       // a head drawn from another model in place of a limb's own
    std::vector<LimbDList> extras;          // hair, hats and masks drawn on top of a limb
    std::vector<s8> hiddenLimbs;            // limbs the NPC never draws (weapons, props)
    bool setupDL37 = false;                 // the material setup the NPC draws with (25 unless it uses 37)
    bool hasEnvColor = false;               // an env colour set for the whole model (carpenters' clothes)
    Color_RGB8 envColor = { 0, 0, 0 };
    f32 size = 1.0f;               // relative to the common figurine height, for NPCs that should stand out
    std::vector<Color_RGB8> group; // a soul shared by a set of NPCs drawn together: one each, in its env colour
};

// clang-format off
const std::vector<Figurine> sFigurines = {
    // Merchants
    { .soul = RG_KOKIRI_SHOPKEEPER_SOUL, .skeleton = gKm1Skel, .animation = object_masterkokiri_Anim_0004A8, .frame = kLastFrame,
      .textures = { { 0x0A, gKokiriShopkeeperEyeDefaultTex } },
      .envColors = { { 0x08, { 0, 130, 70 } }, { 0x09, { 110, 170, 20 } } }, .head = { 15, gKokiriShopkeeperHeadDL } },
    { .soul = RG_POTION_SHOPKEEPER_SOUL, .skeleton = object_ds2_Skel_004258, .animation = object_ds2_Anim_0002E4,
      .textures = { { 0x08, gPotionShopkeeperEyeOpenTex } }, .size = 0.75f },
    { .soul = RG_BAZAAR_SHOPKEEPER_SOUL, .skeleton = gObjectOssanSkel, .animation = gObjectOssanAnim_000338,
      .textures = { { 0x08, gOssanEyeOpenTex } } },
    { .soul = RG_GORON_SHOPKEEPER_SOUL, .skeleton = gGoronSkel, .animation = gGoronShopkeeperAnim, .frame = kLastFrame,
      .textures = { { 0x08, gGoronCsEyeOpenTex }, { 0x09, gGoronCsMouthNeutralTex } } },
    { .soul = RG_ZORA_SHOPKEEPER_SOUL, .skeleton = gZoraSkel, .animation = gZoraShopkeeperAnim, .frame = kLastFrame,
      .textures = { { 0x08, gZoraEyeOpenTex } }, .size = 1.3f },
    { .soul = RG_BOMBCHU_SHOPKEEPER_SOUL, .skeleton = gBombchuShopkeeperSkel, .animation = gBombchuShopkeeperIdleAnim,
      .textures = { { 0x08, gBombchuShopkeeperEyeOpenTex } }, .size = 0.8f },
    { .soul = RG_MASK_SALESMAN_SOUL, .skeleton = gHappyMaskSalesmanSkel, .animation = gHappyMaskSalesmanIdleAnim,
      .textures = { { 0x08, gHappyMaskSalesmanEyeClosedTex } }, .size = 0.8f },
    { .soul = RG_BEAN_SALESMAN_SOUL, .skeleton = gBeanSalesmanSkel, .animation = gBeanSalesmanEatingAnim },
    { .soul = RG_CARPET_SALESMAN_SOUL, .skeleton = gCarpetMerchantSkel, .animation = gCarpetMerchantIdleAnim,
      .setupDL37 = true },
    { .soul = RG_MEDIGORON_SOUL, .skeleton = gGoronSkel, .animation = object_gm_Anim_0002B8,
      .textures = { { 0x08, gGoronCsEyeOpenTex }, { 0x09, gGoronCsMouthNeutralTex } }, .size = 1.2f },
    { .soul = RG_GRANNY_SOUL, .skeleton = gPotionShopLadySkel, .animation = gPotionShopLadyAnim, .setupDL37 = true, .size = 0.8f },
    { .soul = RG_BEGGAR_SOUL, .skeleton = object_boj_Skel_0000F0, .animation = gObjOsAnim_31B0,
      .envColors = { { 0x08, { 50, 80, 0 } }, { 0x09, { 50, 80, 0 } } }, .head = { 15, object_boj_DL_005528 } },
    // The Poe Collector's skeleton is his whole stall: its root limb is the stall, the rest is him.
    { .soul = RG_POE_COLLECTOR_SOUL, .skeleton = gPoeSellerSkel, .animation = gPoeSellerIdleAnim, .hiddenLimbs = { 1 } },
    { .soul = RG_GROG_SOUL, .skeleton = object_hs_Skel_006260, .animation = object_hs_Anim_0005C0,
      .hiddenLimbs = { 10, 11 }, .setupDL37 = true },
    { .soul = RG_FADO_SOUL, .skeleton = gKw1Skel, .animation = gKokiriStandingAnim,
      .textures = { { 0x0A, gFaEyeOpenTex } },
      .envColors = { { 0x08, { 70, 190, 60 } }, { 0x09, { 100, 30, 0 } } }, .head = { 15, gFaDL } },
    { .soul = RG_MUTOH_SOUL, .skeleton = object_toryo_Skel_007150, .animation = object_toryo_Anim_000E50 },
    { .soul = RG_BIGGORON_SOUL, .skeleton = gGoronSkel, .animation = gGoronAnim_004930, .frame = kLastFrame,
      .textures = { { 0x08, gGoronCsEyeOpenTex }, { 0x09, gGoronCsMouthNeutralTex } }, .size = 1.35f },
    // Minigames
    { .soul = RG_SHOOTING_GALLERY_MAN_SOUL, .skeleton = gObjectOssanSkel, .animation = gObjectOssanAnim_000338,
      .head = { 8, gObjectOssanEnSyatekiManDL_007E28 }, .size = 0.8f },
    { .soul = RG_BOMBCHU_BOWLING_LADY_SOUL, .skeleton = gChuGirlSkel, .animation = gChuGirlLeanOverCounterAnim,
      .textures = { { 0x08, gChuGirlEyeOpenTex } }, .size = 0.8f },
    { .soul = RG_TREASURE_CHEST_MAN_SOUL, .skeleton = object_ts_Skel_004FE0, .animation = object_ts_Anim_000498,
      .textures = { { 0x08, object_ts_Tex_000970 } }, .size = 0.8f },
    { .soul = RG_DIVING_GAME_ZORA_SOUL, .skeleton = gZoraSkel, .animation = gZoraIdleAnim,
      .textures = { { 0x08, gZoraEyeOpenTex } }, .size = 1.15f },
    { .soul = RG_DAMPE_SOUL, .skeleton = gDampeSkel, .animation = gDampeRestAnim, .frame = kLastFrame,
      .textures = { { 0x08, gDampeEyeOpenTex } } },
    { .soul = RG_HORSEBACK_ARCHER_SOUL, .skeleton = gGerudoWhiteSkel, .animation = gGerudoWhiteIdleAnim,
      .textures = { { 0x08, gGerudoWhiteEyeOpenTex } }, .extras = { { 15, gGerudoWhiteHairstyleBobDL } },
      .setupDL37 = true },
    { .soul = RG_FISHING_POND_OWNER_SOUL, .skeleton = gFishingOwnerSkel, .animation = gFishingOwnerAnim,
      .textures = { { 0x08, gFishingOwnerEyeOpenTex } }, .extras = { { 8, gFishingOwnerHatDL } }, .size = 0.85f },
    { .soul = RG_SCIENTIST_SOUL, .skeleton = object_mk_Skel_005DF0, .animation = object_mk_Anim_000D88,
      .setupDL37 = true, .size = 0.85f },
    // Rolled up, the way the hot rodder Goron spends his day.
    { .soul = RG_HOT_RODDER_SOUL, .skeleton = gGoronSkel, .animation = gGoronAnim_004930,
      .textures = { { 0x08, gGoronCsEyeOpenTex }, { 0x09, gGoronCsMouthNeutralTex } } },
    { .soul = RG_SKULL_KID_SOUL, .skeleton = gSkullKidSkel, .animation = gSkullKidPlayFluteAnim },
    { .soul = RG_FROGS_SOUL, .skeleton = object_fr_Skel_00B498, .animation = object_fr_Anim_001534,
      .textures = { { 0x08, object_fr_Tex_0059A0 }, { 0x09, object_fr_Tex_0059A0 } },
      .size = 0.55f,
      .group = { { 200, 170, 0 }, { 0, 170, 200 }, { 210, 120, 100 }, { 120, 130, 230 }, { 190, 190, 190 } } },
    { .soul = RG_ANJU_SOUL, .skeleton = gCuccoLadySkel, .animation = gObjOsAnim_07D0,
      .textures = { { 0x08, gCuccoLadyEyeOpenTex } } },
    // Gatekeepers
    { .soul = RG_KAKARIKO_GATEKEEPER_SOUL, .skeleton = gEnHeishiSkel, .animation = gEnHeishiSlamSpearAnim },
    { .soul = RG_MIDO_SOUL, .skeleton = gMidoSkel, .animation = gMidoHandsOnHipsIdleAnim,
      .textures = { { 0x08, gMidoEyeOpenTex } }, .size = 0.9f },
    { .soul = RG_KOKIRI_EXIT_GUARD_SOUL, .skeleton = gKm1Skel, .animation = gKokiriBlockingAnim, .frame = 2.0f,
      .envColors = { { 0x08, { 0, 130, 70 } }, { 0x09, { 110, 170, 20 } } }, .head = { 15, gKm1DL }, .size = 0.9f },
    { .soul = RG_KING_ZORA_SOUL, .skeleton = gKzSkel, .animation = gKzIdleAnim,
      .textures = { { 0x08, gKzEyeOpenTex } }, .setupDL37 = true, .size = 1.15f },
    { .soul = RG_WASTELAND_GATEKEEPER_SOUL, .skeleton = gGerudoWhiteSkel, .animation = gGerudoWhiteIdleAnim,
      .textures = { { 0x08, gGerudoWhiteEyeOpenTex } }, .extras = { { 15, gGerudoWhiteHairstyleStraightFringeDL } },
      .setupDL37 = true },
    { .soul = RG_TRAINING_GROUND_GATEKEEPER_SOUL, .skeleton = gGerudoWhiteSkel, .animation = gGerudoWhiteDismissiveAnim,
      .frame = kLastFrame, .textures = { { 0x08, gGerudoWhiteEyeOpenTex } },
      .extras = { { 15, gGerudoWhiteHairstyleStraightFringeDL } }, .setupDL37 = true },
    { .soul = RG_GERUDO_CARD_GIVER_SOUL, .skeleton = gGerudoRedSkel, .animation = gGerudoRedStandAnim,
      .textures = { { 0x08, gGerudoRedEyeOpenTex } }, .hiddenLimbs = { 0x05, 0x0B, 0x10 }, .setupDL37 = true },
    { .soul = RG_ICHIRO_SOUL, .skeleton = object_daiku_Skel_007958, .animation = object_daiku_Anim_001AB0,
      .extras = { { 15, object_daiku_DL_005BD0 } }, .hasEnvColor = true, .envColor = { 170, 10, 70 } },
    { .soul = RG_SABOORO_SOUL, .skeleton = object_daiku_Skel_007958, .animation = object_daiku_Anim_001AB0,
      .extras = { { 15, object_daiku_DL_005880 } }, .hasEnvColor = true, .envColor = { 200, 0, 150 } },
    { .soul = RG_JIRO_SOUL, .skeleton = object_daiku_Skel_007958, .animation = object_daiku_Anim_001AB0,
      .extras = { { 15, object_daiku_DL_005990 } }, .hasEnvColor = true, .envColor = { 0, 230, 70 } },
    { .soul = RG_SHIRO_SOUL, .skeleton = object_daiku_Skel_007958, .animation = object_daiku_Anim_001AB0,
      .extras = { { 15, object_daiku_DL_005AC0 } }, .hasEnvColor = true, .envColor = { 170, 200, 255 } },
    { .soul = RG_TALON_SOUL, .skeleton = gTalonSkel, .animation = gTalonStandAnim,
      .textures = { { 0x08, gTalonEyeOpenTex }, { 0x09, gTalonHeadSkinTex } }, .setupDL37 = true },
    { .soul = RG_MALON_SOUL, .skeleton = gMalonChildSkel, .animation = gMalonChildIdleAnim,
      .textures = { { 0x08, gMalonChildEyeOpenTex }, { 0x09, gMalonChildNeutralMouthTex } }, .hiddenLimbs = { 2, 5 } },
    { .soul = RG_INGO_SOUL, .skeleton = gIngoSkel, .animation = object_in_Anim_001CC0,
      .textures = { { 0x08, gIngoEyeOpenTex }, { 0x09, gIngoHeadGradient2Tex } }, .size = 1.15f },
    { .soul = RG_KAEPORA_GAEBORA_SOUL, .skeleton = gOwlPerchingSkel, .animation = gOwlPerchAnim,
      .textures = { { 0x08, gObjOwlEyeOpenTex } }, .setupDL37 = true },
    // Story
    { .soul = RG_SARIA_SOUL, .skeleton = gSariaSkel, .animation = gSariaWaitArmsToSideAnim,
      .textures = { { 0x08, gSariaEyeOpenTex }, { 0x09, gSariaEyeOpenTex }, { 0x0A, gSariaMouthClosed2Tex } } },
    { .soul = RG_DARUNIA_SOUL, .skeleton = gDaruniaSkel, .animation = gDaruniaIdleAnim,
      .textures = { { 0x08, gDaruniaEyeOpenTex }, { 0x09, gDaruniaMouthSeriousTex }, { 0x0A, gDaruniaNoseSeriousTex } },
      .size = 1.15f },
    { .soul = RG_RUTO_SOUL, .skeleton = gRutoChildSkel, .animation = gRutoChildWaitHandsBehindBackAnim,
      .textures = { { 0x08, gRutoChildEyeOpenTex }, { 0x09, gRutoChildMouthClosedTex } } },
    { .soul = RG_NABOORU_SOUL, .skeleton = gNabooruSkel, .animation = gNabooruStandingHandsOnHipsChamberOfSagesAnim,
      .textures = { { 0x08, gNabooruEyeOpenTex }, { 0x09, gNabooruEyeOpenTex } }, .size = 1.15f },
    { .soul = RG_ZELDA_SOUL, .skeleton = gChildZeldaSkel, .animation = gChildZeldaAnim_000654,
      .textures = { { 0x08, gChildZeldaEyeOpenTex }, { 0x09, gChildZeldaEyeOpenTex }, { 0x0A, gChildZeldaMouthNeutralTex } },
      .hiddenLimbs = { 3, 4, 5, 6 } },
    { .soul = RG_IMPA_SOUL, .skeleton = gImpaSkel, .animation = gImpaIdleAnim,
      .textures = { { 0x08, gImpaEyeOpenTex } }, .size = 1.1f },
    { .soul = RG_RAURU_SOUL, .skeleton = object_rl_Skel_007B38, .animation = object_rl_Anim_000A3C,
      .textures = { { 0x08, object_rl_Tex_003620 }, { 0x09, object_rl_Tex_003620 } } },
    { .soul = RG_GREAT_FAIRIES_SOUL, .skeleton = gGreatFairySkel, .animation = gGreatFairySittingTransitionAnim,
      .frame = kLastFrame,
      .textures = { { 0x08, gGreatFairyEyeOpenTex }, { 0x09, gGreatFairyEyeOpenTex }, { 0x0A, gGreatFairyMouthClosedTex } },
      .size = 1.3f},
    { .soul = RG_SHEIK_SOUL, .skeleton = gSheikSkel, .animation = gSheikArmsCrossedIdleAnim,
      .textures = { { 0x08, gSheikEyeOpenTex }, { 0x09, gSheikEyeOpenTex } } },
    // Townsfolk
    { .soul = RG_WINDMILL_MAN_SOUL, .skeleton = gWindmillManSkel, .animation = gWindmillManPlayStillAnim,
      .textures = { { 0x08, gWindmillManEyeClosedTex }, { 0x09, gWindmillManMouthOpenTex } }, .setupDL37 = true },
    { .soul = RG_MAN_ON_ROOF_SOUL, .skeleton = gRoofManSkel, .animation = gRoofManIdleAnim,
      .textures = { { 0x08, gRoofManEyeOpenTex } }, .setupDL37 = true, .size = 0.85f },
    { .soul = RG_DOG_LADY_SOUL, .skeleton = gDogLadySkel, .animation = gObjOsAnim_092C,
      .textures = { { 0x0A, gDogLadyEyeOpenTex } },
      .envColors = { { 0x08, { 255, 255, 255 } }, { 0x09, { 255, 255, 255 } } }, .head = { 15, gDogLadyHeadDL } },
    { .soul = RG_SKULLTULA_FAMILY_SOUL, .skeleton = object_ssh_Skel_0052E0, .animation = object_ssh_Anim_000304,
      .textures = { { 0x08, object_ssh_Tex_0007E0 } }, .size = 0.65f },
    { .soul = RG_HYLIAN_GUARDS_SOUL, .skeleton = gEnHeishiSkel, .animation = gEnHeishiIdleAnim },
    { .soul = RG_RUNNING_MAN_SOUL, .skeleton = gRunningManSkel, .animation = gRunningManRunAnim, .frame = 3.0f,
      .textures = { { 0x08, gRunningManMouthOpenTex } } },
    { .soul = RG_GORON_LINK_SOUL, .skeleton = gGoronSkel, .animation = gGoronAnim_004930, .frame = kLastFrame,
      .textures = { { 0x08, gGoronCsEyeOpenTex }, { 0x09, gGoronCsMouthNeutralTex } }, .size = 0.9f},
    { .soul = RG_KNOW_IT_ALL_BROTHERS_SOUL, .skeleton = gKm1Skel, .animation = gKokiriStandingHandOnChestAnim,
      .envColors = { { 0x08, { 0, 130, 70 } }, { 0x09, { 110, 170, 20 } } }, .head = { 15, gKm1DL }, .size = 0.85f },
    { .soul = RG_HONEY_AND_DARLING_SOUL, .skeleton = gDancingCoupleSkel, .animation = gDancingCoupleAnim,
      .envColors = { { 0x08, { 0, 50, 160 } }, { 0x09, { 255, 255, 255 } } } },
    { .soul = RG_PIERRE_SOUL, .skeleton = object_ka_Skel_0065B0, .animation = object_ka_Anim_000214 },
    { .soul = RG_BONOORU_SOUL, .skeleton = object_ka_Skel_0065B0, .animation = object_ka_Anim_000214,
      .frame = kLastFrame },
    { .soul = RG_KOKIRI_SOUL, .skeleton = gKw1Skel, .animation = gKokiriStandingAnim,
      .textures = { { 0x0A, gKw1EyeOpenTex } },
      .envColors = { { 0x08, { 70, 190, 60 } }, { 0x09, { 100, 30, 0 } } }, .head = { 15, object_kw1_DL_002C10 }, .size = 0.85f },
    { .soul = RG_HYLIANS_SOUL, .skeleton = object_boj_Skel_0000F0, .animation = gObjOsAnim_3D84,
      .textures = { { 0x0A, object_boj_Tex_0005FC } },
      .envColors = { { 0x08, { 220, 0, 80 } }, { 0x09, { 255, 255, 255 } } }, .head = { 15, object_boj_DL_0026F0 } },
    { .soul = RG_GORONS_SOUL, .skeleton = gGoronSkel, .animation = gGoronAnim_004930, .frame = kLastFrame,
      .textures = { { 0x08, gGoronCsEyeOpenTex }, { 0x09, gGoronCsMouthNeutralTex } } },
    { .soul = RG_ZORAS_SOUL, .skeleton = gZoraSkel, .animation = gZoraIdleAnim, .frame = kLastFrame,
      .textures = { { 0x08, gZoraEyeOpenTex } }, .size = 1.15f },
    { .soul = RG_GERUDO_SOUL, .skeleton = gGerudoPurpleSkel, .animation = gGerudoPurpleLookingAboutAnim,
      .textures = { { 0x08, gGerudoPurpleEyeOpenTex } }, .size = 1.25f },
    { .soul = RG_DEKU_SCRUBS_SOUL, .skeleton = gBusinessScrubSkel, .animation = gBusinessScrubLookAroundAnim },
};
// clang-format on

// A figurine's skeleton, posed once, and its height.
struct PosedSkeleton {
    SkelAnime skelAnime{};
    std::vector<Vec3s> jointTable;
    std::vector<Vec3s> morphTable;
    f32 bottom = 0.0f; // model-space height of its feet
    f32 height = 1.0f; // model-space height, feet to the top of the head
    bool valid = false;
};

std::unordered_map<RandomizerGet, std::unique_ptr<PosedSkeleton>> sPosed;

const Figurine* FindFigurine(RandomizerGet soul) {
    for (const Figurine& figurine : sFigurines) {
        if (figurine.soul == soul) {
            return &figurine;
        }
    }
    return nullptr;
}

struct Bounds {
    Vec3f min = { FLT_MAX, FLT_MAX, FLT_MAX };
    Vec3f max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
};

// Walks the posed limb tree as the skeleton draw does, recording where each joint ends up.
void MeasureLimb(void** skeleton, const Vec3s* jointTable, s32 limbIndex, const std::vector<s8>& hidden,
                 Bounds& bounds) {
    StandardLimb* limb = (StandardLimb*)SEGMENTED_TO_VIRTUAL(skeleton[limbIndex]);
    Vec3f pos;
    if (limbIndex == 0) {
        pos = { (f32)jointTable[0].x, (f32)jointTable[0].y, (f32)jointTable[0].z };
    } else {
        pos = { (f32)limb->jointPos.x, (f32)limb->jointPos.y, (f32)limb->jointPos.z };
    }
    Vec3s rot = jointTable[limbIndex + 1];
    Matrix_Push();
    Matrix_TranslateRotateZYX(&pos, &rot);
    Vec3f origin = { 0.0f, 0.0f, 0.0f };
    Vec3f joint;
    Matrix_MultVec3f(&origin, &joint);
    bool isHidden = std::find(hidden.begin(), hidden.end(), (s8)(limbIndex + 1)) != hidden.end();
    if (!isHidden) {
        bounds.min = { (std::min)(bounds.min.x, joint.x), (std::min)(bounds.min.y, joint.y),
                       (std::min)(bounds.min.z, joint.z) };
        bounds.max = { (std::max)(bounds.max.x, joint.x), (std::max)(bounds.max.y, joint.y),
                       (std::max)(bounds.max.z, joint.z) };
    }
    if (limb->child != LIMB_DONE) {
        MeasureLimb(skeleton, jointTable, limb->child, hidden, bounds);
    }
    Matrix_Pop();
    if (limb->sibling != LIMB_DONE) {
        MeasureLimb(skeleton, jointTable, limb->sibling, hidden, bounds);
    }
}

PosedSkeleton* GetPosedSkeleton(PlayState* play, const Figurine& figurine) {
    auto it = sPosed.find(figurine.soul);
    if (it != sPosed.end()) {
        return it->second->valid ? it->second.get() : nullptr;
    }
    auto posed = std::make_unique<PosedSkeleton>();
    PosedSkeleton* result = nullptr;
    SkeletonHeader* header = ResourceMgr_LoadSkeletonByName(figurine.skeleton, &posed->skelAnime);
    if (header != nullptr) {
        s32 limbCount = header->limbCount + 1;
        posed->jointTable.resize(limbCount);
        posed->morphTable.resize(limbCount);
        if (header->skeletonType == SKELANIME_TYPE_FLEX) {
            SkelAnime_InitFlex(play, &posed->skelAnime, (FlexSkeletonHeader*)figurine.skeleton,
                               (AnimationHeader*)figurine.animation, posed->jointTable.data(), posed->morphTable.data(),
                               limbCount);
        } else {
            SkelAnime_Init(play, &posed->skelAnime, (SkeletonHeader*)figurine.skeleton,
                           (AnimationHeader*)figurine.animation, posed->jointTable.data(), posed->morphTable.data(),
                           limbCount);
        }
        s32 frame =
            (figurine.frame == kLastFrame) ? Animation_GetLastFrame((void*)figurine.animation) : (s32)figurine.frame;
        SkelAnime_GetFrameData((AnimationHeader*)figurine.animation, frame, limbCount, posed->jointTable.data());

        Bounds bounds;
        Matrix_Push();
        Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_NEW);
        MeasureLimb((void**)posed->skelAnime.skeleton, posed->jointTable.data(), 0, figurine.hiddenLimbs, bounds);
        Matrix_Pop();
        // Joints sit inside the model: the head reaches well above its joint, the soles a little below.
        // A figurine that isn't standing (sitting, lying down) is sized by its longest side instead.
        f32 spanY = (std::max)(bounds.max.y - bounds.min.y, 1.0f);
        f32 spanXZ = (std::max)(bounds.max.x - bounds.min.x, bounds.max.z - bounds.min.z);
        posed->bottom = bounds.min.y - spanY * 0.05f;
        posed->height = (std::max)(spanY * 1.3f, spanXZ * 1.1f);
        posed->valid = true;
        result = posed.get();
    }
    sPosed[figurine.soul] = std::move(posed);
    return result;
}

Gfx* EnvColorDL(GraphicsContext* gfxCtx, Color_RGB8 color) {
    Gfx* dList = (Gfx*)Graph_Alloc(gfxCtx, 2 * sizeof(Gfx));
    gDPSetEnvColor(dList, color.r, color.g, color.b, 255);
    gSPEndDisplayList(dList + 1);
    return dList;
}

Gfx* EmptyDL(GraphicsContext* gfxCtx) {
    Gfx* dList = (Gfx*)Graph_Alloc(gfxCtx, sizeof(Gfx));
    gSPEndDisplayList(dList);
    return dList;
}

s32 FigurineOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* arg) {
    const Figurine* figurine = static_cast<const Figurine*>(arg);
    if (limbIndex == figurine->head.limb && figurine->head.dList != nullptr) {
        *dList = (Gfx*)figurine->head.dList;
    }
    for (s8 hidden : figurine->hiddenLimbs) {
        if (limbIndex == hidden) {
            *dList = nullptr;
        }
    }
    return false;
}

} // namespace

// Outside any namespace: OPEN_DISPS declares the frame interpolation hooks at block scope.
static void FigurinePostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* arg) {
    const Figurine* figurine = static_cast<const Figurine*>(arg);
    for (const LimbDList& extra : figurine->extras) {
        if (limbIndex == extra.limb) {
            OPEN_DISPS(play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)extra.dList);
            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

static bool DrawFigurine(PlayState* play, RandomizerGet soul) {
    const Figurine* figurine = FindFigurine(soul);
    if (figurine == nullptr) {
        return false;
    }
    PosedSkeleton* posed = GetPosedSkeleton(play, *figurine);
    if (posed == nullptr) {
        return false;
    }

    OPEN_DISPS(play->state.gfxCtx);

    if (figurine->setupDL37) {
        Gfx_SetupDL_37Opa(play->state.gfxCtx);
    } else {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
    }
    gDPPipeSync(POLY_OPA_DISP++);
    if (figurine->hasEnvColor) {
        gDPSetEnvColor(POLY_OPA_DISP++, figurine->envColor.r, figurine->envColor.g, figurine->envColor.b, 255);
    } else {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    }
    gSPSegment(POLY_OPA_DISP++, 0x0C, (uintptr_t)EmptyDL(play->state.gfxCtx));
    for (const SegmentTexture& texture : figurine->textures) {
        gSPSegment(POLY_OPA_DISP++, texture.segment, (uintptr_t)SEGMENTED_TO_VIRTUAL(texture.texture));
    }
    for (const SegmentEnvColor& envColor : figurine->envColors) {
        gSPSegment(POLY_OPA_DISP++, envColor.segment, (uintptr_t)EnvColorDL(play->state.gfxCtx, envColor.color));
    }

    // Scale the figurine to the common height and stand it on the bottom of the item's space.
    f32 height = kFigurineHeight * figurine->size / kItemScale;
    f32 scale = height / posed->height;
    f32 floor = -(kFigurineHeight / 2.0f) / kItemScale;
    // A group stands in a ring, each member facing out in its own colour.
    size_t count = figurine->group.empty() ? 1 : figurine->group.size();
    f32 ringRadius = (count > 1) ? height * 0.45f : 0.0f;
    for (size_t i = 0; i < count; i++) {
        if (!figurine->group.empty()) {
            const Color_RGB8& color = figurine->group[i];
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, color.r, color.g, color.b, 255);
        }
        s16 yaw = (s16)(i * 0x10000 / count);
        Matrix_Push();
        Matrix_RotateY(BINANG_TO_RAD(yaw), MTXMODE_APPLY);
        Matrix_Translate(0.0f, floor - posed->bottom * scale, ringRadius, MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        SkelAnime_DrawSkeletonOpa(play, &posed->skelAnime, FigurineOverrideLimbDraw, FigurinePostLimbDraw,
                                  (void*)figurine);
        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
    return true;
}

bool Rando::DrawNpcSoulFigurine(PlayState* play, RandomizerGet soul) {
    return DrawFigurine(play, soul);
}
