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

class Logic {
  public:
    uint8_t Bottles = 0;
    uint8_t NumBottles = 0;
    uint8_t PieceOfHeart = 0;
    uint8_t HeartContainer = 0;
    bool IsChild = false;
    bool IsAdult = false;
    uint8_t BigPoes = 0;
    uint8_t BaseHearts = 0;
    bool AtDay = false;
    bool AtNight = false;

    bool CalculatingAvailableChecks = false;
    bool ACProcessUndiscoveredExits = false;

    SaveContext* mSaveContext = nullptr;
    Logic();
    bool CanUse(RandomizerGet itemName);
    bool HasProjectile(HasProjectileAge age);
    bool HasItem(RandomizerGet itemName);
    bool HasBossSoul(RandomizerGet itemName);
    bool CanOpenOverworldDoor(RandomizerGet itemName);
    bool SmallKeys(s16 scene, uint8_t requiredAmount);
    bool CanGroundJump(bool hasBombflower = false);
    bool CanOpenUnderwaterChest();
    bool CanDoGlitch(GlitchType glitch);
    bool CanEquipSwap(RandomizerGet itemName);
    bool CanKillEnemy(RandomizerEnemy enemy, EnemyDistance distance = ED_CLOSE, bool wallOrFloor = true,
                      uint8_t quantity = 1, bool timer = false, bool inWater = false);
    bool CanPassEnemy(RandomizerEnemy enemy, EnemyDistance distance = ED_CLOSE, bool wallOrFloor = true);
    bool CanAvoidEnemy(RandomizerEnemy enemy, bool grounded = false, uint8_t quantity = 1);
    bool CanGetEnemyDrop(RandomizerEnemy enemy, EnemyDistance distance = ED_CLOSE, bool aboveLink = false);
    bool CanBreakMudWalls();
    bool CanGetDekuBabaSticks();
    bool CanGetDekuBabaNuts();
    bool CanHitEyeTargets();
    bool CanDetonateBombFlowers();
    bool CanDetonateUprightBombFlower();
    bool MQWaterLevel(RandoWaterLevel level);
    uint8_t BottleCount();
    uint8_t OcarinaButtons();
    bool HasBottle();
    bool CanUseSword();
    bool CanJumpslashExceptHammer();
    bool CanJumpslash();
    bool CanClearStalagmite();
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
    bool CanBreakPots();
    bool CanBreakCrates();
    bool CanBreakSmallCrates();
    bool CanBonkTrees();
    bool HasFireSource();
    bool HasFireSourceWithTorch();
    bool TradeQuestStep(RandomizerGet rg);
    bool CanStandingShield();
    bool CanShield();
    bool CanUseProjectile();
    bool CanBuildRainbowBridge();
    bool CanTriggerLACS();
    bool IsFireLoopLocked();
    void Reset(bool resetSaveContext = true);
    void SetContext(std::shared_ptr<Context> _ctx);
    bool Get(LogicVal logicVal);
    void Set(LogicVal logicVal, bool remove);
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
    int8_t GetUsedSmallKeyCount(SceneID sceneId);
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