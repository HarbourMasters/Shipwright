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

static bool IsDungeonVanilla(const DungeonKey dungeon) {
    return Rando::Context::GetInstance()->GetDungeon(dungeon)->IsVanilla();
}

static bool IsDungeonMQ(const DungeonKey dungeon) {
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
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanGroundJumpslash, false),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanMiddairGroundJump, false),
        REGISTER_LOGIC_FUNCTION(SunlightArrows),
        REGISTER_LOGIC_FUNCTION(SpiritEastToSwitch),
        REGISTER_LOGIC_FUNCTION(SpiritSunBlockSouthLedge),
        REGISTER_LOGIC_FUNCTION(MQSpiritStatueToSunBlock),
        REGISTER_LOGIC_FUNCTION(MQSpirit4KeyColossus),
        REGISTER_LOGIC_FUNCTION(MQSpirit4KeyWestHand),
        REGISTER_LOGIC_FUNCTION(CanClimbLadder),
        REGISTER_LOGIC_FUNCTION(CanClimbHighLadder),
        REGISTER_LOGIC_FUNCTION(DMCPotsToPad),
        REGISTER_LOGIC_FUNCTION(DMCUpperToPots),
        REGISTER_LOGIC_FUNCTION(DMCPadToPots),
        REGISTER_LOGIC_FUNCTION(CanBreakRocks),
        REGISTER_LOGIC_FUNCTION(CanRead),
    };
}

std::unordered_map<std::string, int> LogicExpression::Impl::enumMap;

void LogicExpression::Impl::PopulateEnumMap() {

    struct Pair {
        const char* key;
        int value;
    };

    static const Pair kEnumPairs[] = {
#define RANDO_ENUM_ITEM(value, ...) { #value, value },
#include "../randomizerEnums.h"
#undef RANDO_ENUM_ITEM
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
