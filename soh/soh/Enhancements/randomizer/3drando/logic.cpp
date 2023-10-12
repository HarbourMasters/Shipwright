#include "logic.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

#include "settings.hpp"
#include "dungeon.hpp"

using namespace Settings;

namespace Logic {

  bool noVariable    = false;

  //Child item logic
  bool KokiriSword   = false;
  bool ZeldasLetter  = false;
  bool WeirdEgg      = false;
  bool HasBottle     = false;
  bool Bombchus      = false;
  bool Bombchus5     = false;
  bool Bombchus10    = false;
  bool Bombchus20    = false;
  bool MagicBean     = false;
  bool MagicBeanPack = false;
  bool RutosLetter   = false;
  bool Boomerang     = false;
  bool DinsFire      = false;
  bool FaroresWind   = false;
  bool NayrusLove    = false;
  bool LensOfTruth   = false;
  bool ShardOfAgony  = false;
  bool SkullMask     = false;
  bool MaskOfTruth   = false;

  //Adult logic
  bool Hammer        = false;
  bool IronBoots     = false;
  bool HoverBoots    = false;
  bool MirrorShield  = false;
  bool GoronTunic    = false;
  bool ZoraTunic     = false;
  bool Epona         = false;
  bool BigPoe        = false;
  bool GerudoToken   = false;
  bool FireArrows    = false;
  bool IceArrows     = false;
  bool LightArrows   = false;
  bool MasterSword   = false;
  bool BiggoronSword = false;

  //Trade Quest
  bool PocketEgg     = false;
  bool Cojiro        = false;
  bool OddMushroom   = false;
  bool OddPoultice   = false;
  bool PoachersSaw   = false;
  bool BrokenSword   = false;
  bool Prescription  = false;
  bool EyeballFrog   = false;
  bool Eyedrops      = false;
  bool ClaimCheck    = false;

  //Trade Quest Events
  bool WakeUpAdultTalon   = false;
  bool CojiroAccess       = false;
  bool OddMushroomAccess  = false;
  bool OddPoulticeAccess  = false;
  bool PoachersSawAccess  = false;
  bool BrokenSwordAccess  = false;
  bool PrescriptionAccess = false;
  bool EyeballFrogAccess  = false;
  bool EyedropsAccess     = false;
  bool DisableTradeRevert = false;

  //Songs
  bool ZeldasLullaby    = false;
  bool SariasSong       = false;
  bool SunsSong         = false;
  bool SongOfStorms     = false;
  bool EponasSong       = false;
  bool SongOfTime       = false;
  bool MinuetOfForest   = false;
  bool BoleroOfFire     = false;
  bool SerenadeOfWater  = false;
  bool RequiemOfSpirit  = false;
  bool NocturneOfShadow = false;
  bool PreludeOfLight   = false;

  //Stones and Meddallions
  bool ForestMedallion = false;
  bool FireMedallion   = false;
  bool WaterMedallion  = false;
  bool SpiritMedallion = false;
  bool ShadowMedallion = false;
  bool LightMedallion  = false;
  bool KokiriEmerald   = false;
  bool GoronRuby       = false;
  bool ZoraSapphire    = false;

  //Dungeon Clears
  bool DekuTreeClear       = false;
  bool DodongosCavernClear = false;
  bool JabuJabusBellyClear = false;
  bool ForestTempleClear   = false;
  bool FireTempleClear     = false;
  bool WaterTempleClear    = false;
  bool SpiritTempleClear   = false;
  bool ShadowTempleClear   = false;

  //Trial Clears
  bool ForestTrialClear = false;
  bool FireTrialClear   = false;
  bool WaterTrialClear  = false;
  bool SpiritTrialClear = false;
  bool ShadowTrialClear = false;
  bool LightTrialClear  = false;

  //Greg
  bool Greg = false;
  bool GregInLogic = false;

  //Progressive Items
  uint8_t ProgressiveBulletBag  = 0;
  uint8_t ProgressiveBombBag    = 0;
  uint8_t ProgressiveMagic      = 0;
  uint8_t ProgressiveScale      = 0;
  uint8_t ProgressiveHookshot   = 0;
  uint8_t ProgressiveBow        = 0;
  uint8_t ProgressiveWallet     = 0;
  uint8_t ProgressiveStrength   = 0;
  uint8_t ProgressiveOcarina    = 0;
  uint8_t ProgressiveGiantKnife = 0;

  //Logical keysanity
  bool IsKeysanity = false;

  //Keys
  uint8_t ForestTempleKeys          = 0;
  uint8_t FireTempleKeys            = 0;
  uint8_t WaterTempleKeys           = 0;
  uint8_t SpiritTempleKeys          = 0;
  uint8_t ShadowTempleKeys          = 0;
  uint8_t GanonsCastleKeys          = 0;
  uint8_t GerudoFortressKeys        = 0;
  uint8_t GerudoTrainingGroundsKeys = 0;
  uint8_t BottomOfTheWellKeys       = 0;
  uint8_t TreasureGameKeys          = 0;

  //Triforce Pieces
  uint8_t TriforcePieces = 0;

  //Boss Keys
  bool BossKeyForestTemple = false;
  bool BossKeyFireTemple   = false;
  bool BossKeyWaterTemple  = false;
  bool BossKeySpiritTemple = false;
  bool BossKeyShadowTemple = false;
  bool BossKeyGanonsCastle = false;

  //Gold Skulltula Count
  uint8_t GoldSkulltulaTokens = 0;

  //Bottle Count
  uint8_t   Bottles    = 0;
  uint8_t   NumBottles = 0;
  bool NoBottles  = false;

  //Drops and Bottle Contents Access
  bool DekuNutDrop      = false;
  bool NutPot           = false;
  bool NutCrate         = false;
  bool DekuBabaNuts     = false;
  bool DekuStickDrop    = false;
  bool StickPot         = false;
  bool DekuBabaSticks   = false;
  bool BugsAccess       = false;
  bool BugShrub         = false;
  bool WanderingBugs    = false;
  bool BugRock          = false;
  bool BlueFireAccess   = false;
  bool FishAccess       = false;
  bool FishGroup        = false;
  bool LoneFish         = false;
  bool FairyAccess      = false;
  bool GossipStoneFairy = false;
  bool BeanPlantFairy   = false;
  bool ButterflyFairy   = false;
  bool FairyPot         = false;
  bool FreeFairies      = false;
  bool FairyPond        = false;
  bool BombchuDrop      = false;
  bool AmmoCanDrop      = false;

  bool BuyBombchus10    = false;
  bool BuyBombchus20    = false;
  bool BuySeed          = false;
  bool BuyArrow         = false;
  bool BuyBomb          = false;
  bool BuyGPotion       = false;
  bool BuyBPotion       = false;
  bool MagicRefill      = false;

  uint8_t   PieceOfHeart     = 0;
  uint8_t   HeartContainer   = 0;
  bool DoubleDefense    = false;

  /* --- HELPERS, EVENTS, AND LOCATION ACCESS --- */
  /* These are used to simplify reading the logic, but need to be updated
  /  every time a base value is updated.                       */

  bool Slingshot        = false;
  bool Ocarina          = false;
  bool OcarinaOfTime    = false;
  bool BombBag          = false;
  bool MagicMeter       = false;
  bool Hookshot         = false;
  bool Longshot         = false;
  bool Bow              = false;
  bool GoronBracelet    = false;
  bool SilverGauntlets  = false;
  bool GoldenGauntlets  = false;
  bool SilverScale      = false;
  bool GoldScale        = false;
  bool AdultsWallet     = false;

  bool ChildScarecrow   = false;
  bool AdultScarecrow   = false;
  bool ScarecrowSong    = false;
  bool Scarecrow        = false;
  bool DistantScarecrow = false;

  bool Bombs            = false;
  bool DekuShield       = false;
  bool HylianShield     = false;
  bool Nuts             = false;
  bool Sticks           = false;
  bool Bugs             = false;
  bool BlueFire         = false;
  bool Fish             = false;
  bool Fairy            = false;
  bool BottleWithBigPoe = false;

  bool FoundBombchus    = false;
  bool CanPlayBowling   = false;
  bool HasBombchus      = false;
  bool HasExplosives    = false;
  bool HasBoots         = false;
  bool IsChild          = false;
  bool IsAdult          = false;
  bool IsGlitched       = false;
  bool CanBlastOrSmash  = false;
  bool CanChildAttack   = false;
  bool CanChildDamage   = false;
  bool CanCutShrubs     = false;
  bool CanDive          = false;
  bool CanLeaveForest   = false;
  bool CanPlantBugs     = false;
  bool CanRideEpona     = false;
  bool CanStunDeku      = false;
  bool CanSummonGossipFairy = false;
  bool CanSummonGossipFairyWithoutSuns = false;
  bool NeedNayrusLove      = false;
  bool CanSurviveDamage    = false;
  bool CanTakeDamage       = false;
  bool CanTakeDamageTwice  = false;
  //bool CanPlantBean        = false;
  bool CanOpenBombGrotto   = false;
  bool CanOpenStormGrotto  = false;
  bool BigPoeKill          = false;
  bool HookshotOrBoomerang = false;
  bool CanGetNightTimeGS   = false;

  uint8_t   BaseHearts      = 0;
  uint8_t   Hearts          = 0;
  uint8_t   Multiplier      = 0;
  uint8_t   EffectiveHealth = 0;
  uint8_t   FireTimer       = 0;
  uint8_t   WaterTimer      = 0;

  bool GuaranteeTradePath     = false;
  bool GuaranteeHint          = false;
  bool HasFireSource          = false;
  bool HasFireSourceWithTorch = false;

  bool CanFinishGerudoFortress = false;

  bool HasShield          = false;
  bool CanShield          = false;
  bool ChildShield        = false;
  bool AdultReflectShield = false;
  bool AdultShield        = false;
  bool CanShieldFlick     = false;
  bool CanJumpslash       = false;
  bool CanUseProjectile   = false;
  bool CanUseMagicArrow   = false;

  //Bridge and LACS Requirements
  uint8_t StoneCount              = 0;
  uint8_t MedallionCount          = 0;
  uint8_t DungeonCount            = 0;
  bool HasAllStones          = false;
  bool HasAllMedallions      = false;
  bool CanBuildRainbowBridge = false;
  bool BuiltRainbowBridge    = false;
  bool CanTriggerLACS        = false;

  //Other
  bool AtDay         = false;
  bool AtNight       = false;
  uint8_t Age             = 0;
  bool CanCompleteTriforce = false;

  //Events
  bool ShowedMidoSwordAndShield  = false;
  bool CarpenterRescue           = false;
  bool GF_GateOpen               = false;
  bool GtG_GateOpen              = false;
  bool DampesWindmillAccess      = false;
  bool DrainWell                 = false;
  bool GoronCityChildFire        = false;
  bool GCWoodsWarpOpen           = false;
  bool GCDaruniasDoorOpenChild   = false;
  bool StopGCRollingGoronAsAdult = false;
  bool WaterTempleLow            = false;
  bool WaterTempleMiddle         = false;
  bool WaterTempleHigh           = false;
  bool KakarikoVillageGateOpen   = false;
  bool KingZoraThawed            = false;
  bool ForestTempleJoelle        = false;
  bool ForestTempleBeth          = false;
  bool ForestTempleJoAndBeth     = false;
  bool ForestTempleAmy           = false;
  bool ForestTempleMeg           = false;
  bool ForestTempleAmyAndMeg     = false;
  bool FireLoopSwitch            = false;
  bool LinksCow                  = false;
  bool AtDampeTime               = false;
  bool DeliverLetter             = false;
  bool TimeTravel                = false;

  /* --- END OF HELPERS AND LOCATION ACCESS --- */

  //Placement Tracking
  uint8_t AddedProgressiveBulletBags = 0;
  uint8_t AddedProgressiveBombBags   = 0;
  uint8_t AddedProgressiveMagics     = 0;
  uint8_t AddedProgressiveScales     = 0;
  uint8_t AddedProgressiveHookshots  = 0;
  uint8_t AddedProgressiveBows       = 0;
  uint8_t AddedProgressiveWallets    = 0;
  uint8_t AddedProgressiveStrengths  = 0;
  uint8_t AddedProgressiveOcarinas   = 0;
  uint8_t TokensInPool               = 0;

  //Event checking past
  bool DrainWellPast            = false;
  bool DampesWindmillAccessPast = false;
  bool DekuTreeClearPast        = false;
  bool GoronRubyPast            = false;
  bool ZoraSapphirePast         = false;
  bool ForestTrialClearPast     = false;
  bool FireTrialClearPast       = false;
  bool WaterTrialClearPast      = false;
  bool SpiritTrialClearPast     = false;
  bool ShadowTrialClearPast     = false;
  bool LightTrialClearPast      = false;
  bool BuyDekuShieldPast        = false;
  bool TimeTravelPast           = false;

  bool CanPlay(bool song) {
    return Ocarina && song;
  }

  static bool IsMagicItem(uint32_t item) {
    return item == DINS_FIRE    ||
           item == FARORES_WIND ||
           item == NAYRUS_LOVE  ||
           item == LENS_OF_TRUTH;
  }

  static bool IsMagicArrow(uint32_t item) {
    return item == FIRE_ARROWS ||
           item == ICE_ARROWS  ||
           item == LIGHT_ARROWS;
  }

  bool HasItem(uint32_t itemName) {
    return (itemName == DINS_FIRE         && DinsFire)        ||
           (itemName == FARORES_WIND      && FaroresWind)     ||
           (itemName == NAYRUS_LOVE       && NayrusLove)      ||
           (itemName == LENS_OF_TRUTH     && LensOfTruth)     ||
           (itemName == BOW               && Bow)             ||
           (itemName == MEGATON_HAMMER    && Hammer)          ||
           (itemName == IRON_BOOTS        && IronBoots)       ||
           (itemName == HOVER_BOOTS       && HoverBoots)      ||
           (itemName == HOOKSHOT          && Hookshot)        ||
           (itemName == LONGSHOT          && Longshot)        ||
           (itemName == SILVER_GAUNTLETS  && SilverGauntlets) ||
           (itemName == GOLDEN_GAUNTLETS  && GoldenGauntlets) ||
           (itemName == GORON_TUNIC       && GoronTunic)      ||
           (itemName == ZORA_TUNIC        && ZoraTunic)       ||
           (itemName == SCARECROW         && Scarecrow)       ||
           (itemName == DISTANT_SCARECROW && DistantScarecrow)||
           (itemName == HYLIAN_SHIELD     && HylianShield)    ||
           (itemName == MIRROR_SHIELD     && MirrorShield)    ||
           (itemName == MASTER_SWORD      && MasterSword)     ||
           (itemName == BIGGORON_SWORD    && BiggoronSword)   ||
           (itemName == SLINGSHOT         && Slingshot)       ||
           (itemName == BOOMERANG         && Boomerang)       ||
           (itemName == KOKIRI_SWORD      && KokiriSword)     ||
           (itemName == STICKS            && Sticks)          ||
           (itemName == DEKU_SHIELD       && DekuShield)      ||
           (itemName == FIRE_ARROWS       && FireArrows)      ||
           (itemName == ICE_ARROWS        && IceArrows)       ||
           (itemName == LIGHT_ARROWS      && LightArrows);

  }

  //Can the passed in item be used?
  bool CanUse(uint32_t itemName) {
    if (!HasItem(itemName))
      return false;

    switch (itemName) {
      // Adult items
      case BOW:               return IsAdult || BowAsChild;
      case MEGATON_HAMMER:    return IsAdult || HammerAsChild;
      case IRON_BOOTS:        return IsAdult || IronBootsAsChild;
      case HOVER_BOOTS:       return IsAdult || HoverBootsAsChild;
      case HOOKSHOT:          return IsAdult || HookshotAsChild;
      case LONGSHOT:          return IsAdult || HookshotAsChild;
      case SILVER_GAUNTLETS:  return IsAdult;
      case GOLDEN_GAUNTLETS:  return IsAdult;
      case GORON_TUNIC:       return IsAdult || GoronTunicAsChild;
      case ZORA_TUNIC:        return IsAdult || ZoraTunicAsChild;
      case SCARECROW:         return IsAdult || HookshotAsChild;
      case DISTANT_SCARECROW: return IsAdult || HookshotAsChild;
      case HYLIAN_SHIELD:     return IsAdult;
      case MIRROR_SHIELD:     return IsAdult || MirrorShieldAsChild;
      case MASTER_SWORD:      return IsAdult || MasterSwordAsChild;
      case BIGGORON_SWORD:    return IsAdult || BiggoronSwordAsChild;

      // Child items
      case SLINGSHOT:         return IsChild || SlingshotAsAdult;
      case BOOMERANG:         return IsChild || BoomerangAsAdult;
      case KOKIRI_SWORD:      return IsChild || KokiriSwordAsAdult;
      case STICKS:            return IsChild || StickAsAdult;
      case DEKU_SHIELD:       return IsChild || DekuShieldAsAdult;

      // Magic items
      default: return MagicMeter && (IsMagicItem(itemName) || (IsMagicArrow(itemName) && CanUse(BOW)));
    }
  }

  bool HasProjectile(HasProjectileAge age) {
    return HasExplosives ||
           (age == HasProjectileAge::Child  && (Slingshot || Boomerang)) ||
           (age == HasProjectileAge::Adult  && (Hookshot  || Bow      )) ||
           (age == HasProjectileAge::Both   && (Slingshot || Boomerang)  && (Hookshot || Bow)) ||
           (age == HasProjectileAge::Either && (Slingshot || Boomerang   ||  Hookshot || Bow));
  }

  uint8_t GetDifficultyValueFromString(Option& glitchOption) {
    return 0;
  }

//todo rewrite glitch section

  bool CanEquipSwap(uint32_t itemName) {
    if (!HasItem(itemName))
      return false;
    
    if (CanDoGlitch(GlitchType::EquipSwapDins) || CanDoGlitch(GlitchType::EquipSwap))
      return true;

    return false;
  }

  bool CanDoGlitch(GlitchType glitch) {
    switch(glitch) {
      case GlitchType::EquipSwapDins:
        return ((IsAdult && HasItem(DINS_FIRE)) || (IsChild && (HasItem(STICKS) || HasItem(DINS_FIRE)))) && GlitchEquipSwapDins;
      case GlitchType::EquipSwap: // todo: add bunny hood to adult item equippable list and child trade item to child item equippable list
        return ((IsAdult && (HasItem(DINS_FIRE) || HasItem(FARORES_WIND) || HasItem(NAYRUS_LOVE))) || (IsChild && (HasItem(STICKS) || HasItem(SLINGSHOT) || HasItem(BOOMERANG) || HasBottle || Nuts || Ocarina || HasItem(LENS_OF_TRUTH) || HasExplosives || (MagicBean || MagicBeanPack) || HasItem(DINS_FIRE) || HasItem(FARORES_WIND) || HasItem(NAYRUS_LOVE)))) && GlitchEquipSwap;
    }

    //Shouldn't be reached
    return false;
  }

  //Updates all logic helpers. Should be called whenever a non-helper is changed
  void UpdateHelpers() {
    NumBottles      = ((NoBottles) ? 0 : (Bottles + ((DeliverLetter) ? 1 : 0)));
    HasBottle       = NumBottles >= 1;
    Slingshot       = (ProgressiveBulletBag >= 1) && (BuySeed || AmmoCanDrop);
    Ocarina         = ProgressiveOcarina    >= 1;
    OcarinaOfTime   = ProgressiveOcarina    >= 2;
    MagicMeter      = (ProgressiveMagic     >= 1) && (AmmoCanDrop || (HasBottle && (BuyGPotion || BuyBPotion)));
    BombBag         = (ProgressiveBombBag   >= 1) && (BuyBomb || AmmoCanDrop);
    Hookshot        = ProgressiveHookshot   >= 1;
    Longshot        = ProgressiveHookshot   >= 2;
    Bow             = (ProgressiveBow       >= 1) && (BuyArrow || AmmoCanDrop);
    GoronBracelet   = ProgressiveStrength   >= 1;
    SilverGauntlets = ProgressiveStrength   >= 2;
    GoldenGauntlets = ProgressiveStrength   >= 3;
    SilverScale     = ProgressiveScale      >= 1;
    GoldScale       = ProgressiveScale      >= 2;
    AdultsWallet    = ProgressiveWallet     >= 1;
    MasterSword     = MasterSword   || IsAdult;
    BiggoronSword   = BiggoronSword || ProgressiveGiantKnife >= 2;

    ScarecrowSong    = ScarecrowSong || FreeScarecrow || (ChildScarecrow && AdultScarecrow);
    Scarecrow        = Hookshot && CanPlay(ScarecrowSong);
    DistantScarecrow = Longshot && CanPlay(ScarecrowSong);

    //Drop Access
    DekuStickDrop = StickPot || DekuBabaSticks;
    DekuNutDrop   = (NutPot  || NutCrate         || DekuBabaNuts) && AmmoCanDrop;
    BugsAccess    = BugShrub || WanderingBugs    || BugRock;
    FishAccess    = LoneFish || FishGroup;
    FairyAccess   = FairyPot || GossipStoneFairy || BeanPlantFairy || ButterflyFairy || FreeFairies || FairyPond;


    //refills
    Bombs        = BombBag;
    Nuts         = DekuNutDrop || Nuts;
    Sticks       = DekuStickDrop || Sticks;
    Bugs         = HasBottle && BugsAccess;
    BlueFire     = (HasBottle && BlueFireAccess) || (BlueFireArrows && CanUse(ICE_ARROWS));
    Fish         = HasBottle && FishAccess;
    Fairy        = HasBottle && FairyAccess;

    FoundBombchus   = (BombchuDrop || Bombchus || Bombchus5 || Bombchus10 || Bombchus20);
    CanPlayBowling  = (BombchusInLogic && FoundBombchus) || (!BombchusInLogic && BombBag);
    HasBombchus     = (BuyBombchus10 || BuyBombchus20 || (AmmoDrops.Is(AMMODROPS_BOMBCHU) && FoundBombchus));

    HasExplosives =  Bombs || (BombchusInLogic && HasBombchus);

    HasBoots = IronBoots || HoverBoots;

    //Unshuffled adult trade quest
    Eyedrops     = Eyedrops     || (!ShuffleAdultTradeQuest && ClaimCheck);
    EyeballFrog  = EyeballFrog  || (!ShuffleAdultTradeQuest && Eyedrops);
    Prescription = Prescription || (!ShuffleAdultTradeQuest && EyeballFrog);
    BrokenSword  = BrokenSword  || (!ShuffleAdultTradeQuest && Prescription);
    PoachersSaw  = PoachersSaw  || (!ShuffleAdultTradeQuest && BrokenSword);
    OddPoultice  = OddPoultice  || (!ShuffleAdultTradeQuest && PoachersSaw);
    OddMushroom  = OddMushroom  || (!ShuffleAdultTradeQuest && OddPoultice);
    Cojiro       = Cojiro       || (!ShuffleAdultTradeQuest && OddMushroom);
    PocketEgg    = PocketEgg    || (!ShuffleAdultTradeQuest && Cojiro);

    // IsChild = Age == AGE_CHILD;
    // IsAdult = Age == AGE_ADULT;

    CanBlastOrSmash = HasExplosives || CanUse(MEGATON_HAMMER);
    CanChildAttack  = IsChild && (Slingshot || Boomerang || Sticks || KokiriSword || HasExplosives || CanUse(DINS_FIRE) || CanUse(MASTER_SWORD) || CanUse(MEGATON_HAMMER) || CanUse(BIGGORON_SWORD));
    CanChildDamage  = IsChild && (Slingshot ||              Sticks || KokiriSword || HasExplosives || CanUse(DINS_FIRE) || CanUse(MASTER_SWORD) || CanUse(MEGATON_HAMMER) || CanUse(BIGGORON_SWORD));
    CanStunDeku     = IsAdult || CanChildAttack || Nuts || HasShield;
    CanCutShrubs    = IsAdult /*|| Sticks*/ || KokiriSword || Boomerang || HasExplosives || CanUse(MASTER_SWORD) || CanUse(MEGATON_HAMMER) || CanUse(BIGGORON_SWORD);
    CanDive         = ProgressiveScale >= 1;
    CanLeaveForest  = OpenForest.IsNot(OPENFOREST_CLOSED) || IsAdult || DekuTreeClear || ShuffleInteriorEntrances || ShuffleOverworldEntrances;
    CanPlantBugs    = IsChild && Bugs;
    CanRideEpona    = IsAdult && Epona && CanPlay(EponasSong);
    CanSummonGossipFairy            = Ocarina && (ZeldasLullaby || EponasSong || SongOfTime || SunsSong);
    CanSummonGossipFairyWithoutSuns = Ocarina && (ZeldasLullaby || EponasSong || SongOfTime);
    Hearts          = BaseHearts + HeartContainer + (PieceOfHeart >> 2);
    EffectiveHealth = ((Hearts << (2 + DoubleDefense)) >> Multiplier) + ((Hearts << (2 + DoubleDefense)) % (1 << Multiplier) > 0); //Number of half heart hits to die, ranges from 1 to 160
    FireTimer       = CanUse(GORON_TUNIC) ? 255 : (LogicFewerTunicRequirements) ? (Hearts * 8) : 0;
    WaterTimer      = CanUse( ZORA_TUNIC) ? 255 : (LogicFewerTunicRequirements) ? (Hearts * 8) : 0;
    NeedNayrusLove      = (EffectiveHealth == 1);
    CanSurviveDamage    = !NeedNayrusLove || CanUse(NAYRUS_LOVE);
    CanTakeDamage       = Fairy || CanSurviveDamage;
    CanTakeDamageTwice  = (Fairy && NumBottles >= 2) || ((EffectiveHealth == 2) && (CanUse(NAYRUS_LOVE) || Fairy)) || (EffectiveHealth > 2);
    //CanPlantBean        = IsChild && (MagicBean || MagicBeanPack);
    CanOpenBombGrotto   = CanBlastOrSmash       && (ShardOfAgony || LogicGrottosWithoutAgony);
    CanOpenStormGrotto  = CanPlay(SongOfStorms) && (ShardOfAgony || LogicGrottosWithoutAgony);
    HookshotOrBoomerang = CanUse(HOOKSHOT) || CanUse(BOOMERANG);
    CanGetNightTimeGS   = (CanPlay(SunsSong) || !NightGSExpectSuns);

    GuaranteeTradePath     = ShuffleInteriorEntrances || ShuffleOverworldEntrances || LogicBiggoronBolero || CanBlastOrSmash || StopGCRollingGoronAsAdult;
  //GuaranteeHint          = (hints == "Mask" && MaskofTruth) || (hints == "Agony") || (hints != "Mask" && hints != "Agony");
    HasFireSource          = CanUse(DINS_FIRE) || CanUse(FIRE_ARROWS);
    HasFireSourceWithTorch = HasFireSource || CanUse(STICKS);

    //Gerudo Fortress
    CanFinishGerudoFortress = (GerudoFortress.Is(GERUDOFORTRESS_NORMAL)    && GerudoFortressKeys >= 4 && (IsAdult || KokiriSword || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD)) && (GerudoToken || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS) || LogicGerudoKitchen)) ||
                              (GerudoFortress.Is(GERUDOFORTRESS_FAST)      && GerudoFortressKeys >= 1 && (IsAdult || KokiriSword || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD))) ||
                              (GerudoFortress.IsNot(GERUDOFORTRESS_NORMAL) && GerudoFortress.IsNot(GERUDOFORTRESS_FAST));

    HasShield          = CanUse(HYLIAN_SHIELD) || CanUse(DEKU_SHIELD); //Mirror shield can't reflect attacks
    CanShield          = CanUse(MIRROR_SHIELD) || HasShield;
    ChildShield        = IsChild && CanUse(DEKU_SHIELD); //hylian shield is not helpful for child
    AdultReflectShield = IsAdult && CanUse(HYLIAN_SHIELD); //Mirror shield can't reflect attacks
    AdultShield        = IsAdult && (CanUse(HYLIAN_SHIELD) || CanUse(MIRROR_SHIELD));
    CanShieldFlick     = ChildShield || AdultShield;
    CanJumpslash       = IsAdult || Sticks || KokiriSword;
    CanUseProjectile   = HasExplosives || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(SLINGSHOT) || CanUse(BOOMERANG);
    CanUseMagicArrow   = CanUse(FIRE_ARROWS) || CanUse(ICE_ARROWS) || CanUse(LIGHT_ARROWS);

    //Bridge and LACS Requirements
    MedallionCount        = (ForestMedallion ? 1:0) + (FireMedallion ? 1:0) + (WaterMedallion ? 1:0) + (SpiritMedallion ? 1:0) + (ShadowMedallion ? 1:0) + (LightMedallion ? 1:0);
    StoneCount            = (KokiriEmerald ? 1:0) + (GoronRuby ? 1:0) + (ZoraSapphire ? 1:0);
    DungeonCount          = (DekuTreeClear ? 1:0) + (DodongosCavernClear ? 1:0) + (JabuJabusBellyClear ? 1:0) + (ForestTempleClear ? 1:0) + (FireTempleClear ? 1:0) + (WaterTempleClear ? 1:0) + (SpiritTempleClear ? 1:0) + (ShadowTempleClear ? 1:0);
    HasAllStones          = StoneCount == 3;
    HasAllMedallions      = MedallionCount == 6;
    GregInLogic           = BridgeRewardOptions.Is(BRIDGE_OPTION_GREG) || LACSRewardOptions.Is(LACS_OPTION_GREG);

    CanBuildRainbowBridge = Bridge.Is(RAINBOWBRIDGE_OPEN)                                                                         ||
                           (Bridge.Is(RAINBOWBRIDGE_VANILLA)    && ShadowMedallion && SpiritMedallion && LightArrows)             ||
                           (Bridge.Is(RAINBOWBRIDGE_STONES)     && StoneCount + (Greg && GregInLogic ? 1 : 0) >= BridgeStoneCount.Value<uint8_t>())                    ||
                           (Bridge.Is(RAINBOWBRIDGE_MEDALLIONS) && MedallionCount + (Greg && GregInLogic ? 1 : 0) >= BridgeMedallionCount.Value<uint8_t>())            ||
                           (Bridge.Is(RAINBOWBRIDGE_REWARDS)    && StoneCount + MedallionCount + (Greg && GregInLogic ? 1 : 0) >= BridgeRewardCount.Value<uint8_t>())  ||
                           (Bridge.Is(RAINBOWBRIDGE_DUNGEONS)   && DungeonCount + (Greg && GregInLogic ? 1 : 0) >= BridgeDungeonCount.Value<uint8_t>())                ||
                           (Bridge.Is(RAINBOWBRIDGE_TOKENS)     && GoldSkulltulaTokens >= BridgeTokenCount.Value<uint8_t>()) ||
                           (Bridge.Is(RAINBOWBRIDGE_GREG)       && Greg);

    CanTriggerLACS = (LACSCondition == LACSCONDITION_VANILLA    && ShadowMedallion && SpiritMedallion)                          ||
                     (LACSCondition == LACSCONDITION_STONES     && StoneCount + (Greg && GregInLogic ? 1 : 0) >= LACSStoneCount.Value<uint8_t>())                    ||
                     (LACSCondition == LACSCONDITION_MEDALLIONS && MedallionCount + (Greg && GregInLogic ? 1 : 0) >= LACSMedallionCount.Value<uint8_t>())            ||
                     (LACSCondition == LACSCONDITION_REWARDS    && StoneCount + MedallionCount + (Greg && GregInLogic ? 1 : 0) >= LACSRewardCount.Value<uint8_t>())  ||
                     (LACSCondition == LACSCONDITION_DUNGEONS   && DungeonCount + (Greg && GregInLogic ? 1 : 0) >= LACSDungeonCount.Value<uint8_t>())                ||
                     (LACSCondition == LACSCONDITION_TOKENS     && GoldSkulltulaTokens >= LACSTokenCount.Value<uint8_t>());
    CanCompleteTriforce = TriforcePieces >= TriforceHuntRequired.Value<uint8_t>();
  }

  bool SmallKeys(Key dungeon, uint8_t requiredAmount) {
    return SmallKeys(dungeon, requiredAmount, requiredAmount);
  }

  bool SmallKeys(Key dungeon, uint8_t requiredAmountGlitchless, uint8_t requiredAmountGlitched) {
    switch (dungeon) {
      case FOREST_TEMPLE:
        /*if (IsGlitched && (GetDifficultyValueFromString(GlitchHookshotJump_Boots) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE) || GetDifficultyValueFromString(GlitchHoverBoost) >= static_cast<uint8_t>(GlitchDifficulty::NOVICE) ||
                          (GetDifficultyValueFromString(GlitchHover) >= static_cast<uint8_t>(GlitchDifficulty::NOVICE) && GetDifficultyValueFromString(GlitchISG) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE)))) {
          return ForestTempleKeys >= requiredAmountGlitched;
        }*/
        return ForestTempleKeys >= requiredAmountGlitchless;

      case FIRE_TEMPLE:
        /*if (IsGlitched && (GetDifficultyValueFromString(GlitchLedgeClip) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE) || GetDifficultyValueFromString(GlitchHover) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE))) {
          return FireTempleKeys >= requiredAmountGlitched;
        }*/
        return FireTempleKeys >= requiredAmountGlitchless;

      case WATER_TEMPLE:
        /*if (IsGlitched && (false)) {
          return WaterTempleKeys >= requiredAmountGlitched;
        }*/
        return WaterTempleKeys >= requiredAmountGlitchless;

      case SPIRIT_TEMPLE:
        /*if (IsGlitched && (false)) {
          return SpiritTempleKeys >= requiredAmountGlitched;
        }*/
        return SpiritTempleKeys >= requiredAmountGlitchless;

      case SHADOW_TEMPLE:
        /*if (IsGlitched && (GetDifficultyValueFromString(GlitchHookshotClip) >= static_cast<uint8_t>(GlitchDifficulty::NOVICE))) {
          return ShadowTempleKeys >= requiredAmountGlitched;
        }*/
        return ShadowTempleKeys >= requiredAmountGlitchless;

      case BOTTOM_OF_THE_WELL:
        /*if (IsGlitched && (false)) {
          return BottomOfTheWellKeys >= requiredAmountGlitched;
        }*/
        return BottomOfTheWellKeys >= requiredAmountGlitchless;

      case GERUDO_TRAINING_GROUNDS:
        /*if (IsGlitched && (false)) {
          return GerudoTrainingGroundsKeys >= requiredAmountGlitched;
        }*/
        return GerudoTrainingGroundsKeys >= requiredAmountGlitchless;

      case GANONS_CASTLE:
        /*if (IsGlitched && (false)) {
          return GanonsCastleKeys >= requiredAmountGlitched;
        }*/
        return GanonsCastleKeys >= requiredAmountGlitchless;

      case MARKET_TREASURE_CHEST_GAME:
        /*if (IsGlitched && (false)) {
          return TreasureGameKeys >= requiredAmountGlitched;
        }*/
        return TreasureGameKeys >= requiredAmountGlitchless;

      default:
        return false;
    }
  }

  bool EventsUpdated() {

      if (DekuTreeClearPast        != DekuTreeClear        ||
          GoronRubyPast            != GoronRuby            ||
          ZoraSapphirePast         != ZoraSapphire         ||
          ForestTrialClearPast     != ForestTrialClear     ||
          FireTrialClearPast       != FireTrialClear       ||
          WaterTrialClearPast      != WaterTrialClear      ||
          ShadowTrialClearPast     != ShadowTrialClear     ||
          SpiritTrialClearPast     != SpiritTrialClear     ||
          LightTrialClearPast      != LightTrialClear      ||
          DrainWellPast            != DrainWell            ||
          DampesWindmillAccessPast != DampesWindmillAccess ||
          TimeTravelPast           != TimeTravel) {
            DekuTreeClearPast        = DekuTreeClear;
            GoronRubyPast            = GoronRuby;
            ZoraSapphirePast         = ZoraSapphire;
            ForestTrialClearPast     = ForestTrialClear;
            FireTrialClearPast       = FireTrialClear;
            WaterTrialClearPast      = WaterTrialClear;
            ShadowTrialClearPast     = ShadowTrialClear;
            SpiritTrialClearPast     = SpiritTrialClear;
            LightTrialClearPast      = LightTrialClear;
            DrainWellPast            = DrainWell;
            DampesWindmillAccessPast = DampesWindmillAccess;
            return true;
          }
     return false;
   }

   //Reset All Logic to false
   void LogicReset() {
     //Settings-dependent variables
     IsKeysanity = Keysanity.Is(KEYSANITY_ANYWHERE) || Keysanity.Is(KEYSANITY_OVERWORLD) || Keysanity.Is(KEYSANITY_ANY_DUNGEON);
     AmmoCanDrop = AmmoDrops.IsNot(AMMODROPS_NONE);

     //Child item logic
     KokiriSword   = false;
     ZeldasLetter  = false;
     WeirdEgg      = false;
     HasBottle     = false;
     Bombchus      = false;
     Bombchus5     = false;
     Bombchus10    = false;
     Bombchus20    = false;
     MagicBean     = false;
     MagicBeanPack = false;
     RutosLetter   = false;
     Boomerang     = false;
     DinsFire      = false;
     FaroresWind   = false;
     NayrusLove    = false;
     LensOfTruth   = false;
     ShardOfAgony  = false;
     SkullMask     = false;
     MaskOfTruth   = false;

     //Adult logic
     Hammer        = false;
     IronBoots     = false;
     HoverBoots    = false;
     MirrorShield  = false;
     GoronTunic    = false;
     ZoraTunic     = false;
     Epona         = false;
     BigPoe        = false;
     GerudoToken   = false;
     FireArrows    = false;
     IceArrows     = false;
     LightArrows   = false;
     MasterSword   = false;
     BiggoronSword = false;

     //Trade Quest
     PocketEgg     = false;
     Cojiro        = false;
     OddMushroom   = false;
     OddPoultice   = false;
     PoachersSaw   = false;
     BrokenSword   = false;
     Prescription  = false;
     EyeballFrog   = false;
     Eyedrops      = false;
     ClaimCheck    = false;

     //Trade Quest Events
     WakeUpAdultTalon   = false;
     CojiroAccess       = false;
     OddMushroomAccess  = false;
     OddPoulticeAccess  = false;
     PoachersSawAccess  = false;
     BrokenSwordAccess  = false;
     PrescriptionAccess = false;
     EyeballFrogAccess  = false;
     EyedropsAccess     = false;
     DisableTradeRevert = false;

     //Songs
     ZeldasLullaby    = false;
     SariasSong       = false;
     SunsSong         = false;
     SongOfStorms     = false;
     EponasSong       = false;
     SongOfTime       = false;
     MinuetOfForest   = false;
     BoleroOfFire     = false;
     SerenadeOfWater  = false;
     RequiemOfSpirit  = false;
     NocturneOfShadow = false;
     PreludeOfLight   = false;

     //Stones and Meddallions
     ForestMedallion = false;
     FireMedallion   = false;
     WaterMedallion  = false;
     SpiritMedallion = false;
     ShadowMedallion = false;
     LightMedallion  = false;
     KokiriEmerald   = false;
     GoronRuby       = false;
     ZoraSapphire    = false;

     //Dungeon Clears
     DekuTreeClear       = false;
     DodongosCavernClear = false;
     JabuJabusBellyClear = false;
     ForestTempleClear   = false;
     FireTempleClear     = false;
     WaterTempleClear    = false;
     SpiritTempleClear   = false;
     ShadowTempleClear   = false;

     //Trial Clears
     ForestTrialClear = false;
     FireTrialClear   = false;
     WaterTrialClear  = false;
     SpiritTrialClear = false;
     ShadowTrialClear = false;
     LightTrialClear  = false;

     //Greg
     Greg = false;

     //Progressive Items
     ProgressiveBulletBag  = 0;
     ProgressiveBombBag    = 0;
     ProgressiveMagic      = 0;
     ProgressiveScale      = 0;
     ProgressiveHookshot   = 0;
     ProgressiveBow        = 0;
     ProgressiveWallet     = 0;
     ProgressiveStrength   = 0;
     ProgressiveOcarina    = 0;
     ProgressiveGiantKnife = 0;

     //Keys
     ForestTempleKeys          = 0;
     //If not keysanity, start with 1 logical key to account for automatically unlocking the basement door in vanilla FiT
     FireTempleKeys            = IsKeysanity || Dungeon::FireTemple.IsMQ() ? 0 : 1;
     WaterTempleKeys           = 0;
     SpiritTempleKeys          = 0;
     ShadowTempleKeys          = 0;
     GanonsCastleKeys          = 0;
     GerudoFortressKeys        = 0;
     GerudoTrainingGroundsKeys = 0;
     BottomOfTheWellKeys       = 0;
     TreasureGameKeys          = 0;

     //Boss Keys
     BossKeyForestTemple = 0;
     BossKeyFireTemple   = 0;
     BossKeyWaterTemple  = 0;
     BossKeySpiritTemple = 0;
     BossKeyShadowTemple = 0;
     BossKeyGanonsCastle = 0;

     //Gold Skulltula Count
     GoldSkulltulaTokens = 0;

     //Bottle Count
     Bottles    = 0;
     NumBottles = 0;
     NoBottles  = false;

     //Triforce Pieces
     TriforcePieces = 0;

     //Drops and Bottle Contents Access
     DekuNutDrop      = false;
     NutPot           = false;
     NutCrate         = false;
     DekuBabaNuts     = false;
     DekuStickDrop    = false;
     StickPot         = false;
     DekuBabaSticks   = false;
     BugsAccess       = false;
     BugShrub         = false;
     WanderingBugs    = false;
     BugRock          = false;
     BlueFireAccess   = false;
     FishAccess       = false;
     FishGroup        = false;
     LoneFish         = false;
     FairyAccess      = false;
     GossipStoneFairy = false;
     BeanPlantFairy   = false;
     ButterflyFairy   = false;
     FairyPot         = false;
     FreeFairies      = false;
     FairyPond        = false;
     BombchuDrop      = false;

     BuyBombchus10    = false;
     BuyBombchus20    = false;
     BuySeed          = false;
     BuyArrow         = false;
     BuyBomb          = false;
     BuyGPotion       = false;
     BuyBPotion       = false;
     MagicRefill      = false;

     PieceOfHeart     = 0;
     HeartContainer   = 0;
     DoubleDefense    = false;

     /* --- HELPERS, EVENTS, AND LOCATION ACCESS --- */
     /* These are used to simplify reading the logic, but need to be updated
     /  every time a base value is updated.                       */

     Slingshot        = false;
     Ocarina          = false;
     OcarinaOfTime    = false;
     BombBag          = false;
     MagicMeter       = false;
     Hookshot         = false;
     Longshot         = false;
     Bow              = false;
     GoronBracelet    = false;
     SilverGauntlets  = false;
     GoldenGauntlets  = false;
     SilverScale      = false;
     GoldScale        = false;
     AdultsWallet     = false;

     ChildScarecrow   = false;
     AdultScarecrow   = false;
     ScarecrowSong    = false;
     Scarecrow        = false;
     DistantScarecrow = false;

     Bombs            = false;
     DekuShield       = false;
     HylianShield     = false;
     Nuts             = false;
     Sticks           = false;
     Bugs             = false;
     BlueFire         = false;
     Fish             = false;
     Fairy            = false;
     BottleWithBigPoe = false;

     FoundBombchus    = false;
     CanPlayBowling   = false;
     HasBombchus      = false;
     HasExplosives    = false;
     HasBoots         = false;
     IsChild          = false;
     IsAdult          = false;
     IsGlitched       = Settings::Logic.Is(LOGIC_GLITCHED);
     CanBlastOrSmash  = false;
     CanChildAttack   = false;
     CanChildDamage   = false;
     CanCutShrubs     = false;
     CanDive          = false;
     CanLeaveForest   = false;
     CanPlantBugs     = false;
     CanRideEpona     = false;
     CanStunDeku      = false;
     CanSummonGossipFairy = false;
     CanSummonGossipFairyWithoutSuns = false;
     //CanPlantBean        = false;
     CanOpenBombGrotto   = false;
     CanOpenStormGrotto  = false;
     BigPoeKill          = false;
     HookshotOrBoomerang = false;

     BaseHearts      = StartingHearts.Value<uint8_t>() + 1;
     Hearts          = 0;
     Multiplier      = (DamageMultiplier.Value<uint8_t>() < 6) ? DamageMultiplier.Value<uint8_t>() : 10;
     EffectiveHealth = 0;
     FireTimer       = 0;
     WaterTimer      = 0;

     GuaranteeTradePath     = false;
     GuaranteeHint          = false;
     HasFireSource          = false;
     HasFireSourceWithTorch = false;

     CanFinishGerudoFortress = false;

     HasShield          = false;
     CanShield          = false;
     ChildShield        = false;
     AdultReflectShield = false;
     AdultShield        = false;
     CanShieldFlick     = false;
     CanJumpslash       = false;
     CanUseProjectile   = false;
     CanUseMagicArrow   = false;

     //Bridge Requirements
     HasAllStones          = false;
     HasAllMedallions      = false;
     CanBuildRainbowBridge = false;
     BuiltRainbowBridge    = false;
     CanTriggerLACS        = false;

     //Other
     AtDay         = false;
     AtNight       = false;
     Age           = Settings::ResolvedStartingAge;

     //Events
     ShowedMidoSwordAndShield  = false;
     CarpenterRescue           = false;
     GF_GateOpen               = false;
     GtG_GateOpen              = false;
     DampesWindmillAccess      = false;
     DrainWell                 = false;
     GoronCityChildFire        = false;
     GCWoodsWarpOpen           = false;
     GCDaruniasDoorOpenChild   = false;
     StopGCRollingGoronAsAdult = false;
     WaterTempleLow            = false;
     WaterTempleMiddle         = false;
     WaterTempleHigh           = false;
     KakarikoVillageGateOpen   = false;
     KingZoraThawed            = false;
     ForestTempleJoelle        = false;
     ForestTempleBeth          = false;
     ForestTempleJoAndBeth     = false;
     ForestTempleAmy           = false;
     ForestTempleMeg           = false;
     ForestTempleAmyAndMeg     = false;
     FireLoopSwitch            = false;
     LinksCow                  = false;
     AtDampeTime               = false;
     DeliverLetter             = false;
     TimeTravel                = false;

     DrainWellPast            = false;
     DampesWindmillAccessPast = false;
     DekuTreeClearPast        = false;
     GoronRubyPast            = false;
     ZoraSapphirePast         = false;
     ForestTrialClearPast     = false;
     FireTrialClearPast       = false;
     WaterTrialClearPast      = false;
     SpiritTrialClearPast     = false;
     ShadowTrialClearPast     = false;
     LightTrialClearPast      = false;
     BuyDekuShieldPast        = false;
     TimeTravelPast           = false;
   }
}
