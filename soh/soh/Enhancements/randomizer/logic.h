#pragma once

#include "randomizerTypes.h"
#include "context.h"
#include <cstdint>

namespace Rando {

enum class HasProjectileAge {
    Adult,
    Child,
    Both,
    Either,
};

enum class GlitchType {
    EquipSwapDins,
    EquipSwap,
};

enum class GlitchDifficulty {
    NOVICE = 1,
    INTERMEDIATE,
    ADVANCED,
    EXPERT,
    HERO,
};

class Logic {
  public:
    bool noVariable = false;

    // Child item logic
    bool SkullMask = false;
    bool MaskOfTruth = false;

    // Adult logic
    bool FreedEpona = false;
    //bool BigPoe = false; //unused

    // Trade Quest Events
    bool WakeUpAdultTalon = false;

    // Dungeon Clears
    bool DekuTreeClear = false;
    bool DodongosCavernClear = false;
    bool JabuJabusBellyClear = false;
    bool ForestTempleClear = false;
    bool FireTempleClear = false;
    bool WaterTempleClear = false;
    bool SpiritTempleClear = false;
    bool ShadowTempleClear = false;

    // Trial Clears
    bool ForestTrialClear = false;
    bool FireTrialClear = false;
    bool WaterTrialClear = false;
    bool SpiritTrialClear = false;
    bool ShadowTrialClear = false;
    bool LightTrialClear = false;

    // Logical keysanity
    bool IsKeysanity = false;

    // Bottle Count
    uint8_t Bottles = 0;
    uint8_t NumBottles = 0;
    bool CanEmptyBigPoes = true;

    // Drops and Bottle Contents Access
    bool NutPot = false;
    bool NutCrate = false;
    bool DekuBabaNuts = false;
    bool StickPot = false;
    bool DekuBabaSticks = false;
    bool BugShrub = false;
    bool WanderingBugs = false;
    bool BugRock = false;
    bool BlueFireAccess = false;
    bool FishGroup = false;
    bool LoneFish = false;
    bool GossipStoneFairy = false;
    bool BeanPlantFairy = false;
    bool ButterflyFairy = false;
    bool FairyPot = false;
    bool FreeFairies = false;
    bool FairyPond = false;
    bool AmmoCanDrop = false;

    uint8_t PieceOfHeart = 0;
    uint8_t HeartContainer = 0;

    bool ChildScarecrow = false;
    bool AdultScarecrow = false;

    bool CarpetMerchant = false;
    bool CouldPlayBowling = false;
    bool IsChild = false;
    bool IsAdult = false;
    bool BigPoeKill = false;
    uint8_t BaseHearts = 0;

    // Bridge and LACS Requirements
    bool BuiltRainbowBridge = false;

    // Other
    bool AtDay = false;
    bool AtNight = false;

    // Events
    bool ShowedMidoSwordAndShield = false;
    bool CarpenterRescue = false;
    bool GF_GateOpen = false;
    bool GtG_GateOpen = false;
    bool DampesWindmillAccess = false;
    bool DrainWell = false;
    bool GoronCityChildFire = false;
    bool GCWoodsWarpOpen = false;
    bool GCDaruniasDoorOpenChild = false;
    bool StopGCRollingGoronAsAdult = false;
    bool WaterTempleLow = false;
    bool WaterTempleMiddle = false;
    bool WaterTempleHigh = false;
    bool KakarikoVillageGateOpen = false;
    bool KingZoraThawed = false;
    bool ForestTempleJoelle = false;
    bool ForestTempleBeth = false;
    bool ForestTempleAmy = false;
    bool ForestTempleMeg = false;
    bool FireLoopSwitch = false;
    bool LinksCow = false;
    bool DeliverLetter = false;
    bool ClearMQDCUpperLobbyRocks = false;
    bool LoweredWaterInsideBotw = false;
    bool OpenedWestRoomMQBotw = false;
    bool OpenedMiddleHoleMQBotw = false;
    bool BrokeDeku1FWeb = false;
    bool ClearedMQDekuSERoom = false;
    bool MQDekuWaterRoomTorches = false;
    bool PushedDekuBasementBlock = false;
    bool OpenedLowestGoronCage = false;
    bool OpenedUpperFireShortcut = false;
    bool HitFireTemplePlatform = false;
    bool OpenedFireMQFireMazeDoor = false;
    bool MQForestBlockRoomTargets = false;
    bool ForestCanTwistHallway = false;
    bool ForestClearBelowBowChest = false; //a better name that covers both versions would be nice
    bool ForestOpenBossCorridor = false;
    bool ShadowTrialFirstChest = false;
    bool MQGTGMazeSwitch = false;
    bool MQGTGRightSideSwitch = false;
    bool GTGPlatformSilverRupees = false;
    bool MQJabuHolesRoomDoor = false;
    bool JabuWestTentacle = false;
    bool JabuNorthTentacle = false;
    bool LoweredJabuPath = false;
    bool MQJabuLiftRoomCow = false;
    bool MQShadowFloorSpikeRupees = false;
    bool ShadowShortcutBlock = false;

    /* --- END OF HELPERS AND LOCATION ACCESS --- */

    SaveContext* mSaveContext = nullptr;
    Logic();
    bool CanUse(RandomizerGet itemName);
    bool HasProjectile(HasProjectileAge age);
    bool HasItem(RandomizerGet itemName);
    bool HasBossSoul(RandomizerGet itemName);
    bool SmallKeys(RandomizerRegion dungeon, uint8_t requiredAmount);
    bool SmallKeys(RandomizerRegion dungeon, uint8_t requiredAmountGlitchless, uint8_t requiredAmountGlitched);
    bool CanDoGlitch(GlitchType glitch);
    bool CanEquipSwap(RandomizerGet itemName);
    bool CanKillEnemy(RandomizerEnemy enemy, EnemyDistance distance = ED_CLOSE, bool wallOrFloor = true, uint8_t quantity = 1, bool timer = false, bool inWater = false);
    bool CanPassEnemy(RandomizerEnemy enemy, EnemyDistance distance = ED_CLOSE, bool wallOrFloor = true);
    bool CanAvoidEnemy(RandomizerEnemy enemy);
    bool CanGetEnemyDrop(RandomizerEnemy enemy, EnemyDistance distance = ED_CLOSE, bool aboveLink = false);
    bool CanBreakMudWalls();
    bool CanGetDekuBabaSticks();
    bool CanGetDekuBabaNuts();
    bool CanHitEyeTargets();
    bool CanDetonateBombFlowers();
    bool CanDetonateUprightBombFlower();
    uint8_t BottleCount();
    uint8_t OcarinaButtons();
    bool HasBottle();
    bool CanJumpslashExceptHammer();
    bool CanJumpslash();
    bool CanHitSwitch(EnemyDistance distance = ED_CLOSE, bool inWater = false);
    bool CanDamage();
    bool CanAttack();
    bool BombchusEnabled();
    bool BombchuRefill();
    bool HookshotOrBoomerang();
    bool ScarecrowsSong();
    bool BlueFire();
    bool HasExplosives();
    bool BlastOrSmash();
    bool CanSpawnSoilSkull();
    bool CanReflectNuts();
    bool CanCutShrubs();
    bool CanStunDeku();
    bool CanLeaveForest();
    bool CallGossipFairy();
    bool CallGossipFairyExceptSuns();
    uint8_t EffectiveHealth();
    uint8_t Hearts();
    uint8_t StoneCount();
    uint8_t MedallionCount();
    uint8_t DungeonCount();
    uint8_t FireTimer();
    uint8_t WaterTimer();
    bool TakeDamage();
    bool CanOpenBombGrotto();
    bool CanOpenStormsGrotto();
    bool CanGetNightTimeGS();
    bool CanBreakUpperBeehives();
    bool CanBreakLowerBeehives();
    bool HasFireSource();
    bool HasFireSourceWithTorch();
    bool TradeQuestStep(RandomizerGet rg);
    bool CanFinishGerudoFortress();
    bool CanStandingShield();
    bool CanShield();
    bool CanUseProjectile();
    bool CanBuildRainbowBridge();
    bool CanTriggerLACS();
    void Reset();
    void SetContext(std::shared_ptr<Context> _ctx);
    bool GetInLogic(LogicVal logicVal);
    void SetInLogic(LogicVal logicVal, bool remove);
    void ApplyItemEffect(Item& item, bool state);
    uint8_t InventorySlot(uint32_t item);
    void SetUpgrade(uint32_t upgrade, uint8_t level);
    uint32_t CurrentUpgrade(uint32_t upgrade);
    uint32_t CurrentInventory(uint32_t item);
    bool CheckInventory(uint32_t item, bool exact);
    void SetInventory(uint32_t itemSlot, uint32_t item);
    bool CheckEquipment(uint32_t item);
    bool CheckQuestItem(uint32_t item);
    void SetQuestItem(uint32_t item, bool state);
    bool HasAdultTrade(uint32_t item);
    void SetAdultTrade(uint32_t item, bool state);
    uint8_t GetSmallKeyCount(uint32_t dungeonIndex);
    void SetSmallKeyCount(uint32_t dungeonIndex, uint8_t count);
    bool CheckDungeonItem(uint32_t item, uint32_t dungeonIndex);
    void SetDungeonItem(uint32_t item, uint32_t dungeonIndex, bool state);
    bool CheckRandoInf(uint32_t flag);
    void SetRandoInf(uint32_t flag, bool state);
    bool CheckEventChkInf(int32_t flag);
    uint8_t GetGSCount();
    void SetEventChkInf(int32_t flag, bool state);
    uint8_t GetAmmo(uint32_t item);
    void SetAmmo(uint32_t item, uint8_t count);
    SaveContext* GetSaveContext();
    void SetSaveContext(SaveContext* context);
    void InitSaveContext();
    void NewSaveContext();
    static std::map<uint32_t, uint32_t> RandoGetToQuestItem;
    static std::map<uint32_t, uint32_t> RandoGetToDungeonScene;
    static std::map<RandomizerGet, uint32_t> RandoGetToEquipFlag;
    static std::map<RandomizerGet, uint32_t> RandoGetToRandInf;

  private:
    std::shared_ptr<Context> ctx;
    bool inLogic[LOGIC_MAX];
}; // class Logic
} // namespace Rando