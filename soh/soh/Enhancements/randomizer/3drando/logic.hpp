#pragma once

#include "keys.hpp"
#include <cstdint>

namespace Logic {
extern bool noVariable;

// Child item logic
extern bool KokiriSword;
extern bool Slingshot;
extern bool ZeldasLetter;
extern bool WeirdEgg;
extern bool HasBottle;
extern bool BombBag;
extern bool Bombchus;
extern bool Bombchus5;
extern bool Bombchus10;
extern bool Bombchus20;
extern bool MagicBean;
extern bool MagicBeanPack;
extern bool RutosLetter;
extern bool Boomerang;
extern bool DinsFire;
extern bool FaroresWind;
extern bool NayrusLove;
extern bool LensOfTruth;
extern bool ShardOfAgony;
extern bool SkullMask;
extern bool MaskOfTruth;

// Adult logic
extern bool Bow;
extern bool Hammer;
extern bool IronBoots;
extern bool HoverBoots;
extern bool MirrorShield;
extern bool GoronTunic;
extern bool ZoraTunic;
extern bool Epona;
extern bool BigPoe;
extern bool GerudoToken;
extern bool FireArrows;
extern bool IceArrows;
extern bool LightArrows;
extern bool MasterSword;
extern bool BiggoronSword;

// Trade Quest
extern bool PocketEgg;
extern bool Cojiro;
extern bool OddMushroom;
extern bool OddPoultice;
extern bool PoachersSaw;
extern bool BrokenSword;
extern bool Prescription;
extern bool EyeballFrog;
extern bool Eyedrops;
extern bool ClaimCheck;

// Trade Quest Events
extern bool WakeUpAdultTalon;
extern bool CojiroAccess;
extern bool OddMushroomAccess;
extern bool OddPoulticeAccess;
extern bool PoachersSawAccess;
extern bool BrokenSwordAccess;
extern bool PrescriptionAccess;
extern bool EyeballFrogAccess;
extern bool EyedropsAccess;
extern bool DisableTradeRevert;

// Songs
extern bool ZeldasLullaby;
extern bool SariasSong;
extern bool SunsSong;
extern bool SongOfStorms;
extern bool EponasSong;
extern bool SongOfTime;
extern bool MinuetOfForest;
extern bool BoleroOfFire;
extern bool SerenadeOfWater;
extern bool RequiemOfSpirit;
extern bool NocturneOfShadow;
extern bool PreludeOfLight;

// Stones and Meddallions
extern bool ForestMedallion;
extern bool FireMedallion;
extern bool WaterMedallion;
extern bool SpiritMedallion;
extern bool ShadowMedallion;
extern bool LightMedallion;
extern bool KokiriEmerald;
extern bool GoronRuby;
extern bool ZoraSapphire;

// Dungeon Clears
extern bool DekuTreeClear;
extern bool DodongosCavernClear;
extern bool JabuJabusBellyClear;
extern bool ForestTempleClear;
extern bool FireTempleClear;
extern bool WaterTempleClear;
extern bool SpiritTempleClear;
extern bool ShadowTempleClear;

// Trial Clears
extern bool ForestTrialClear;
extern bool FireTrialClear;
extern bool WaterTrialClear;
extern bool SpiritTrialClear;
extern bool ShadowTrialClear;
extern bool LightTrialClear;

// Progression Items
extern uint8_t ProgressiveBulletBag;
extern uint8_t ProgressiveBombBag;
extern uint8_t ProgressiveScale;
extern uint8_t ProgressiveHookshot;
extern uint8_t ProgressiveBow;
extern uint8_t ProgressiveStrength;
extern uint8_t ProgressiveWallet;
extern uint8_t ProgressiveMagic;
extern uint8_t ProgressiveOcarina;
extern uint8_t ProgressiveGiantKnife;

// Keysanity
extern bool IsKeysanity;

// Keys
extern uint8_t ForestTempleKeys;
extern uint8_t FireTempleKeys;
extern uint8_t WaterTempleKeys;
extern uint8_t SpiritTempleKeys;
extern uint8_t ShadowTempleKeys;
extern uint8_t BottomOfTheWellKeys;
extern uint8_t GerudoTrainingGroundsKeys;
extern uint8_t GerudoFortressKeys;
extern uint8_t GanonsCastleKeys;
extern uint8_t TreasureGameKeys;

// Boss Keys
extern bool BossKeyForestTemple;
extern bool BossKeyFireTemple;
extern bool BossKeyWaterTemple;
extern bool BossKeySpiritTemple;
extern bool BossKeyShadowTemple;
extern bool BossKeyGanonsCastle;

// Gold Skulltula Count
extern uint8_t GoldSkulltulaTokens;

// Bottle Count, with and without Ruto's Letter
extern uint8_t Bottles;
extern uint8_t NumBottles;
extern bool NoBottles;

// item and bottle drops
extern bool DekuNutDrop;
extern bool NutPot;
extern bool NutCrate;
extern bool DekuBabaNuts;
extern bool DekuStickDrop;
extern bool StickPot;
extern bool DekuBabaSticks;
extern bool BugsAccess;
extern bool BugShrub;
extern bool WanderingBugs;
extern bool BugRock;
extern bool BlueFireAccess;
extern bool FishAccess;
extern bool FishGroup;
extern bool LoneFish;
extern bool FairyAccess;
extern bool GossipStoneFairy;
extern bool BeanPlantFairy;
extern bool ButterflyFairy;
extern bool FairyPot;
extern bool FreeFairies;
extern bool FairyPond;
extern bool BombchuDrop;

extern bool BuyBombchus10;
extern bool BuyBombchus20;
extern bool BuyArrow;
extern bool BuyBomb;
extern bool BuyGPotion;
extern bool BuyBPotion;
extern bool BuySeed;
extern bool MagicRefill;

extern uint8_t PieceOfHeart;
extern uint8_t HeartContainer;
extern bool DoubleDefense;

/* --- HELPERS --- */
/* These are used to simplify reading the logic, but need to be updated
/  every time a base value is updated.                       */

extern bool Ocarina;
extern bool OcarinaOfTime;
extern bool MagicMeter;
extern bool Hookshot;
extern bool Longshot;
extern bool GoronBracelet;
extern bool SilverGauntlets;
extern bool GoldenGauntlets;
extern bool SilverScale;
extern bool GoldScale;
extern bool AdultsWallet;

extern bool ChildScarecrow;
extern bool AdultScarecrow;
extern bool ScarecrowSong;
extern bool Scarecrow;
extern bool DistantScarecrow;

extern bool Bombs;
extern bool DekuShield;
extern bool HylianShield;
extern bool Nuts;
extern bool Sticks;
extern bool Bugs;
extern bool BlueFire;
extern bool Fish;
extern bool Fairy;
extern bool BottleWithBigPoe;

extern bool Bombs;
extern bool FoundBombchus;
extern bool CanPlayBowling;
extern bool HasBombchus;
extern bool HasExplosives;
extern bool HasBoots;
extern bool IsChild;
extern bool IsAdult;
extern bool IsGlitched;
extern bool CanBlastOrSmash;
extern bool CanChildAttack;
extern bool CanChildDamage;
extern bool CanCutShrubs;
extern bool CanDive;
extern bool CanLeaveForest;
extern bool CanPlantBugs;
extern bool CanRideEpona;
extern bool CanStunDeku;
extern bool CanSummonGossipFairy;
extern bool CanSummonGossipFairyWithoutSuns;
extern bool NeedNayrusLove;
extern bool CanSurviveDamage;
extern bool CanTakeDamage;
extern bool CanTakeDamageTwice;
// extern bool CanPlantBean;
extern bool CanOpenBombGrotto;
extern bool CanOpenStormGrotto;
extern bool HookshotOrBoomerang;
extern bool CanGetNightTimeGS;
extern bool BigPoeKill;

extern uint8_t BaseHearts;
extern uint8_t Hearts;
extern uint8_t Multiplier;
extern uint8_t EffectiveHealth;
extern uint8_t FireTimer;
extern uint8_t WaterTimer;

extern bool GuaranteeTradePath;
extern bool GuaranteeHint;
extern bool HasFireSource;
extern bool HasFireSourceWithTorch;

// Gerudo Fortress
extern bool CanFinishGerudoFortress;

extern bool HasShield;
extern bool CanShield;
extern bool CanJumpslash;
extern bool CanUseProjectile;
extern bool CanUseMagicArrow;

// Bridge Requirements
extern bool HasAllStones;
extern bool HasAllMedallions;
extern bool CanBuildRainbowBridge;
extern bool CanTriggerLACS;

// Other
extern bool AtDay;
extern bool AtNight;
extern bool LinksCow;
extern uint8_t Age;

// Events
extern bool ShowedMidoSwordAndShield;
extern bool CarpenterRescue;
extern bool DampesWindmillAccess;
extern bool GF_GateOpen;
extern bool GtG_GateOpen;
extern bool DrainWell;
extern bool GoronCityChildFire;
extern bool GCWoodsWarpOpen;
extern bool GCDaruniasDoorOpenChild;
extern bool StopGCRollingGoronAsAdult;
extern bool WaterTempleLow;
extern bool WaterTempleMiddle;
extern bool WaterTempleHigh;
extern bool KingZoraThawed;
extern bool AtDampeTime;
extern bool DeliverLetter;
extern bool KakarikoVillageGateOpen;
extern bool ForestTempleJoelle;
extern bool ForestTempleBeth;
extern bool ForestTempleJoAndBeth;
extern bool ForestTempleAmy;
extern bool ForestTempleMeg;
extern bool ForestTempleAmyAndMeg;
extern bool FireLoopSwitch;
extern bool TimeTravel;

/* --- END OF HELPERS --- */

extern uint8_t AddedProgressiveBulletBags;
extern uint8_t AddedProgressiveBombBags;
extern uint8_t AddedProgressiveMagics;
extern uint8_t AddedProgressiveScales;
extern uint8_t AddedProgressiveHookshots;
extern uint8_t AddedProgressiveBows;
extern uint8_t AddedProgressiveWallets;
extern uint8_t AddedProgressiveStrengths;
extern uint8_t AddedProgressiveOcarinas;
extern uint8_t TokensInPool;

enum class HasProjectileAge {
    Adult,
    Child,
    Both,
    Either,
};

enum class GlitchType {
    RestrictedItems,
    SuperStab,
    ISG,
    BombHover,
    BombOI,
    OutdoorBombOI,
    WindmillBombOI,
    IndoorBombOI,
    DungeonBombOI,
    HoverBoost,
    SuperSlide,
    Megaflip,
    ASlide,
    HammerSlide,
    LedgeCancel,
    ActionSwap,
    QPA,
    HookshotClip,
    HookshotJump_Bonk,
    HookshotJump_Boots,
    CutsceneDive,
    NaviDive_Stick,
    TripleSlashClip,
    LedgeClip,
    SeamWalk,
};

enum class GlitchDifficulty {
    NOVICE = 1,
    INTERMEDIATE,
    ADVANCED,
    EXPERT,
    HERO,
};

void UpdateHelpers();
bool CanPlay(bool song);
bool CanUse(uint32_t itemName);
bool HasProjectile(HasProjectileAge age);
bool SmallKeys(Key dungeon, uint8_t requiredAmount);
bool SmallKeys(Key dungeon, uint8_t requiredAmountGlitchless, uint8_t requiredAmountGlitched);
bool CanDoGlitch(GlitchType glitch, GlitchDifficulty difficulty);
bool EventsUpdated();
void LogicReset();
} // namespace Logic