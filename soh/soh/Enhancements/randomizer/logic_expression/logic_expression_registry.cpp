#include "logic_expression_impl.h"

#include "../dungeon.h"
#include "../location_access.h"
#include "../randomizerTypes.h"
#include "../SeedContext.h"
#include "variables.h"

#include <stdexcept>
#include <unordered_map>

extern SaveContext gSaveContext;

#define REGISTER_FUNCTION(fn) \
    { #fn, LogicExpression::Impl::RegisterFunction(#fn, fn) }
#define REGISTER_FUNCTION_WITH_DEFAULTS(fn, ...) \
    { #fn, LogicExpression::Impl::RegisterFunctionWithDefaults(#fn, fn, std::make_tuple(__VA_ARGS__)) }
#define REGISTER_LOGIC_FUNCTION(fn) \
    { #fn, LogicExpression::Impl::RegisterLogicFunction(#fn, &Rando::Logic::fn) }
#define REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(fn, ...)                                              \
    {                                                                                               \
#fn, LogicExpression::Impl::RegisterLogicFunctionWithDefaults(#fn, &Rando::Logic::fn,       \
                                                                      std::make_tuple(__VA_ARGS__)) \
    }
#define REGISTER_LOGIC_VARIABLE(var) \
    { #var, LogicExpression::Impl::RegisterLogicVariable(#var, &Rando::Logic::var) }

#pragma region Forwarding Functions
static uint8_t GetOption(const RandomizerSettingKey key) {
    return Rando::Context::GetInstance()->GetOption(key).Get();
}

static uint8_t GetTrickOption(const RandomizerTrick trick) {
    return Rando::Context::GetInstance()->GetTrickOption(trick).Get();
}

static bool IsDungeonVanilla(const Rando::DungeonKey dungeon) {
    return Rando::Context::GetInstance()->GetDungeon(dungeon)->IsVanilla();
}

static bool IsDungeonMQ(const Rando::DungeonKey dungeon) {
    return Rando::Context::GetInstance()->GetDungeon(dungeon)->IsMQ();
}

static bool IsTrialSkipped(const TrialKey trial) {
    return Rando::Context::GetInstance()->GetTrial(trial)->IsSkipped();
}

static uint8_t TriforcePiecesCollected() {
    return gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected;
}
#pragma endregion

std::unordered_map<std::string, LogicExpression::Impl::FunctionAdapter> LogicExpression::Impl::functionAdapters;

void LogicExpression::Impl::PopulateFunctionAdapters() {
    functionAdapters = {
        REGISTER_FUNCTION(AnyAgeTime),
        REGISTER_FUNCTION_WITH_DEFAULTS(GetCheckPrice, RC_UNKNOWN_CHECK),
        REGISTER_FUNCTION(GetWalletCapacity),
        REGISTER_FUNCTION(GetOption),
        REGISTER_FUNCTION(GetTrickOption),
        REGISTER_FUNCTION(ChildCanAccess),
        REGISTER_FUNCTION(IsDungeonVanilla),
        REGISTER_FUNCTION(IsDungeonMQ),
        REGISTER_FUNCTION(IsTrialSkipped),
        REGISTER_FUNCTION(TriforcePiecesCollected),
        REGISTER_FUNCTION(CanPlantBean),
        REGISTER_FUNCTION_WITH_DEFAULTS(SpiritShared, RandomizerRegion{}, ConditionFn{}, false, RR_NONE, ConditionFn{},
                                        RR_NONE, ConditionFn{}),
        REGISTER_FUNCTION(SpiritCertainAccess),

        REGISTER_LOGIC_FUNCTION(CanUse),
        REGISTER_LOGIC_FUNCTION(HasProjectile),
        REGISTER_LOGIC_FUNCTION(HasItem),
        REGISTER_LOGIC_FUNCTION(HasBossSoul),
        REGISTER_LOGIC_FUNCTION(CanOpenOverworldDoor),
        REGISTER_LOGIC_FUNCTION(SmallKeys),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanGroundJump, false),
        REGISTER_LOGIC_FUNCTION(CanOpenUnderwaterChest),
        REGISTER_LOGIC_FUNCTION(CanDoGlitch),
        REGISTER_LOGIC_FUNCTION(CanEquipSwap),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanKillEnemy, RandomizerEnemy{}, ED_CLOSE, true, uint8_t{ 1 }, false,
                                              false),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanPassEnemy, RandomizerEnemy{}, ED_CLOSE, true),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanAvoidEnemy, RandomizerEnemy{}, false, uint8_t{ 1 }),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanGetEnemyDrop, RandomizerEnemy{}, ED_CLOSE, false),
        REGISTER_LOGIC_FUNCTION(CanBreakMudWalls),
        REGISTER_LOGIC_FUNCTION(CanGetDekuBabaSticks),
        REGISTER_LOGIC_FUNCTION(CanGetDekuBabaNuts),
        REGISTER_LOGIC_FUNCTION(CanHitEyeTargets),
        REGISTER_LOGIC_FUNCTION(CanDetonateBombFlowers),
        REGISTER_LOGIC_FUNCTION(CanDetonateUprightBombFlower),
        REGISTER_LOGIC_FUNCTION(BottleCount),
        REGISTER_LOGIC_FUNCTION(OcarinaButtons),
        REGISTER_LOGIC_FUNCTION(HasBottle),
        REGISTER_LOGIC_FUNCTION(CanUseSword),
        REGISTER_LOGIC_FUNCTION(CanJumpslashExceptHammer),
        REGISTER_LOGIC_FUNCTION(CanJumpslash),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanHitSwitch, ED_CLOSE, false),
        REGISTER_LOGIC_FUNCTION(CanDamage),
        REGISTER_LOGIC_FUNCTION(CanAttack),
        REGISTER_LOGIC_FUNCTION(BombchusEnabled),
        REGISTER_LOGIC_FUNCTION(BombchuRefill),
        REGISTER_LOGIC_FUNCTION(HookshotOrBoomerang),
        REGISTER_LOGIC_FUNCTION(ScarecrowsSong),
        REGISTER_LOGIC_FUNCTION(BlueFire),
        REGISTER_LOGIC_FUNCTION(HasExplosives),
        REGISTER_LOGIC_FUNCTION(BlastOrSmash),
        REGISTER_LOGIC_FUNCTION(CanSpawnSoilSkull),
        REGISTER_LOGIC_FUNCTION(CanReflectNuts),
        REGISTER_LOGIC_FUNCTION(CanCutShrubs),
        REGISTER_LOGIC_FUNCTION(CanStunDeku),
        REGISTER_LOGIC_FUNCTION(CallGossipFairy),
        REGISTER_LOGIC_FUNCTION(CallGossipFairyExceptSuns),
        REGISTER_LOGIC_FUNCTION(EffectiveHealth),
        REGISTER_LOGIC_FUNCTION(Hearts),
        REGISTER_LOGIC_FUNCTION(StoneCount),
        REGISTER_LOGIC_FUNCTION(MedallionCount),
        REGISTER_LOGIC_FUNCTION(DungeonCount),
        REGISTER_LOGIC_FUNCTION(FireTimer),
        REGISTER_LOGIC_FUNCTION(WaterTimer),
        REGISTER_LOGIC_FUNCTION(TakeDamage),
        REGISTER_LOGIC_FUNCTION(CanOpenBombGrotto),
        REGISTER_LOGIC_FUNCTION(CanOpenStormsGrotto),
        REGISTER_LOGIC_FUNCTION(CanGetNightTimeGS),
        REGISTER_LOGIC_FUNCTION(CanBreakUpperBeehives),
        REGISTER_LOGIC_FUNCTION(CanBreakLowerBeehives),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanBreakPots, ED_CLOSE, true, false),
        REGISTER_LOGIC_FUNCTION(CanBreakCrates),
        REGISTER_LOGIC_FUNCTION(CanBreakSmallCrates),
        REGISTER_LOGIC_FUNCTION(CanBonkTrees),
        REGISTER_LOGIC_FUNCTION(HasFireSource),
        REGISTER_LOGIC_FUNCTION(HasFireSourceWithTorch),
        REGISTER_LOGIC_FUNCTION(TradeQuestStep),
        REGISTER_LOGIC_FUNCTION(CanStandingShield),
        REGISTER_LOGIC_FUNCTION(CanShield),
        REGISTER_LOGIC_FUNCTION(CanUseProjectile),
        REGISTER_LOGIC_FUNCTION(CanBuildRainbowBridge),
        REGISTER_LOGIC_FUNCTION(CanTriggerLACS),
        REGISTER_LOGIC_FUNCTION(IsFireLoopLocked),
        REGISTER_LOGIC_FUNCTION(Get),
        REGISTER_LOGIC_FUNCTION(GetGSCount),
        REGISTER_LOGIC_FUNCTION(CanClearStalagmite),
        REGISTER_LOGIC_FUNCTION(ReachScarecrow),
        REGISTER_LOGIC_FUNCTION(ReachDistantScarecrow),
        REGISTER_LOGIC_FUNCTION(SummonEpona),
        REGISTER_LOGIC_FUNCTION(Water3FCentralToHighEmblem),
        REGISTER_LOGIC_FUNCTION(WaterRisingTargetTo3FCentral),
        REGISTER_LOGIC_FUNCTION(WaterLevel),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanHammerRecoilHover, false),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanGroundJumpJumpSlash, false),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanMiddairGroundJump, false),
        REGISTER_LOGIC_FUNCTION(SunlightArrows),
        REGISTER_LOGIC_FUNCTION(SpiritEastToSwitch),
        REGISTER_LOGIC_FUNCTION(SpiritSunBlockSouthLedge),
        REGISTER_LOGIC_FUNCTION(MQSpiritStatueToSunBlock),
        REGISTER_LOGIC_FUNCTION(MQSpirit4KeyColossus),
        REGISTER_LOGIC_FUNCTION(MQSpirit4KeyWestHand),
    };
}

std::unordered_map<std::string, int> LogicExpression::Impl::enumMap;

void LogicExpression::Impl::PopulateEnumMap() {
#define DEFINE_DungeonKey(value) { #value, Rando::value },
#define DEFINE_ModIndex(value) { #value, value },
#define DEFINE_TableIndex(value) { #value, value },
#define DEFINE_HintType(value) { #value, value },
#define DEFINE_LogicVal(value) { #value, value },
#define DEFINE_RAND_INF(value) { #value, value },
#define DEFINE_RandomizerArea(value) { #value, value },
#define DEFINE_TrialKey(value) { #value, value },
#define DEFINE_RandomizerCheckType(value) { #value, value },
#define DEFINE_RandomizerCheckQuest(value) { #value, value },
#define DEFINE_RandomizerCheckArea(value) { #value, value },
#define DEFINE_RandomizerCheckStatus(value) { #value, value },
#define DEFINE_RandomizerRegion(value) { #value, value },
#define DEFINE_RandomizerCheck(value) { #value, value },
#define DEFINE_RandomizerTrick(value) { #value, value },
#define DEFINE_RandomizerGet(value) { #value, value },
#define DEFINE_RandomizerHint(value) { #value, value },
#define DEFINE_RandomizerHintTextKey(value) { #value, value },
#define DEFINE_RandomizerSettingGroupKey(value) { #value, value },
#define DEFINE_RandomizerSettingKey(value) { #value, value },
#define DEFINE_RandoOptionGenericOffOn(value) { #value, value },
#define DEFINE_RandoOptionGenericNoYes(value) { #value, value },
#define DEFINE_RandoOptionGenericSkip(value) { #value, value },
#define DEFINE_RandoOptionForest(value) { #value, value },
#define DEFINE_RandoOptionDoorOfTime(value) { #value, value },
#define DEFINE_RandoOptionZorasFountain(value) { #value, value },
#define DEFINE_RandoOptionSleepingWaterfall(value) { #value, value },
#define DEFINE_RandoOptionJabu(value) { #value, value },
#define DEFINE_RandoOptionStartingAge(value) { #value, value },
#define DEFINE_RandoOptionGerudoFortress(value) { #value, value },
#define DEFINE_RandoOptionKakarikoGate(value) { #value, value },
#define DEFINE_RandoOptionRainbowBridge(value) { #value, value },
#define DEFINE_RandoOptionBridgeRewards(value) { #value, value },
#define DEFINE_RandoOptionShopsanity(value) { #value, value },
#define DEFINE_RandoOptionShopsanityCount(value) { #value, value },
#define DEFINE_RandoOptionPrices(value) { #value, value },
#define DEFINE_RandoOptionScrubsanity(value) { #value, value },
#define DEFINE_RandoOptionAmmoDrops(value) { #value, value },
#define DEFINE_RandoOptionBombchuBag(value) { #value, value },
#define DEFINE_RandoOptionBossSouls(value) { #value, value },
#define DEFINE_RandoOptionsFishsanity(value) { #value, value },
#define DEFINE_RandoOptionInfiniteUpgrades(value) { #value, value },
#define DEFINE_RandoOptionDungeonItemLocation(value) { #value, value },
#define DEFINE_RandoOptionDungeonRewards(value) { #value, value },
#define DEFINE_RandoOptionKeyrings(value) { #value, value },
#define DEFINE_RandoOptionKeyringForDungeon(value) { #value, value },
#define DEFINE_RandoOptionGanonsBossKey(value) { #value, value },
#define DEFINE_RandoOptionLACSCondition(value) { #value, value },
#define DEFINE_RandoOptionLACSRewards(value) { #value, value },
#define DEFINE_RandoOptionGanonsTrials(value) { #value, value },
#define DEFINE_RandoOptionDungeonEntranceShuffle(value) { #value, value },
#define DEFINE_RandoOptionBossRoomEntranceShuffle(value) { #value, value },
#define DEFINE_RandoOptionInteriorEntranceShuffle(value) { #value, value },
#define DEFINE_RandoOptionSongShuffle(value) { #value, value },
#define DEFINE_RandoOptionShuffleMerchants(value) { #value, value },
#define DEFINE_RandoOptionStartingOcarina(value) { #value, value },
#define DEFINE_RandoOptionMaskQuest(value) { #value, value },
#define DEFINE_RandoOptionItemPool(value) { #value, value },
#define DEFINE_RandoOptionIceTraps(value) { #value, value },
#define DEFINE_RandoOptionGossipStones(value) { #value, value },
#define DEFINE_RandoOptionHintClarity(value) { #value, value },
#define DEFINE_RandoOptionHintDistribution(value) { #value, value },
#define DEFINE_RandoOptionGerudoKeys(value) { #value, value },
#define DEFINE_RandoOptionTokensanity(value) { #value, value },
#define DEFINE_RandoOptionFreestanding(value) { #value, value },
#define DEFINE_RandoOptionShufflePots(value) { #value, value },
#define DEFINE_RandoOptionShuffleGrass(value) { #value, value },
#define DEFINE_RandoOptionShuffleCrates(value) { #value, value },
#define DEFINE_RandoOptionLinksPocket(value) { #value, value },
#define DEFINE_RandoOptionLogic(value) { #value, value },
#define DEFINE_RandoOptionDamageMultiplier(value) { #value, value },
#define DEFINE_RandoOptionMQDungeons(value) { #value, value },
#define DEFINE_RandoOptionTriforceHunt(value) { #value, value },
#define DEFINE_RandoOptionLocationInclusion(value) { #value, value },
#define DEFINE_RandoOptionChestGame(value) { #value, value },
#define DEFINE_RandoOptionMQSet(value) { #value, value },
#define DEFINE_ItemObtainability(value) { #value, value },
#define DEFINE_TrackerWindowType(value) { #value, value },
#define DEFINE_TrackerDisplayType(value) { #value, value },
#define DEFINE_TrackerComboButton(value) { #value, value },
#define DEFINE_TriforceHuntMessages(value) { #value, value },
#define DEFINE_RandomizerEnemy(value) { #value, value },
#define DEFINE_EnemyDistance(value) { #value, value },
#define DEFINE_RandoWaterLevel(value) { #value, value },
#define DEFINE_GrottoEntranceOffsets(value) { #value, value },

    struct Pair {
        const char* key;
        int value;
    };

    static const Pair kEnumPairs[] = {
#include "../randomizerEnums.h"
        { "HasProjectileAge::Adult", (int)Rando::HasProjectileAge::Adult },
        { "HasProjectileAge::Child", (int)Rando::HasProjectileAge::Child },
        { "HasProjectileAge::Both", (int)Rando::HasProjectileAge::Both },
        { "HasProjectileAge::Either", (int)Rando::HasProjectileAge::Either },
        { "GlitchType::EquipSwapDins", (int)Rando::GlitchType::EquipSwapDins },
        { "GlitchType::EquipSwap", (int)Rando::GlitchType::EquipSwap },
#define DEFINE_SCENE(_0, _1, value, _3, _4, _5) { #value, value },
#include "tables/scene_table.h"
#undef DEFINE_SCENE
    };

    enumMap.reserve(sizeof(kEnumPairs) / sizeof(Pair));
    for (const auto& p : kEnumPairs) {
        enumMap.emplace(p.key, p.value);
    }
}

std::unordered_map<std::string, LogicExpression::Impl::FunctionAdapter> LogicExpression::Impl::variableAdapters;

void LogicExpression::Impl::PopulateVariableAdapters() {
    variableAdapters = {
        REGISTER_LOGIC_VARIABLE(Bottles),      REGISTER_LOGIC_VARIABLE(NumBottles),
        REGISTER_LOGIC_VARIABLE(PieceOfHeart), REGISTER_LOGIC_VARIABLE(HeartContainer),
        REGISTER_LOGIC_VARIABLE(IsChild),      REGISTER_LOGIC_VARIABLE(IsAdult),
        REGISTER_LOGIC_VARIABLE(BigPoes),      REGISTER_LOGIC_VARIABLE(BaseHearts),
        REGISTER_LOGIC_VARIABLE(AtDay),        REGISTER_LOGIC_VARIABLE(AtNight),
    };
}
