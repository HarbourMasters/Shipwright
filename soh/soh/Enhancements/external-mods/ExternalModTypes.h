#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Ship {
class Archive;
}

namespace SOH {

class ExternalModWasmRuntime;

enum class ExternalModInputTriggerType {
    Pressed,
    Held,
    Released,
};

enum class ExternalModItemSlot {
    Stick,
    Nut,
    Bomb,
    Bow,
    FireArrow,
    DinsFire,
    Slingshot,
    Ocarina,
    Bombchu,
    Hookshot,
    IceArrow,
    FaroresWind,
    Boomerang,
    Lens,
    Bean,
    Hammer,
    LightArrow,
    NayrusLove,
    Bottle1,
    Bottle2,
    Bottle3,
    Bottle4,
    TradeAdult,
    TradeChild,
};

enum class ExternalModItemAgePolicy {
    RespectVanilla,
    AllowChild,
    AllowAdult,
};

enum class ExternalModItemUseMode {
    Vanilla,
    Override,
    Augment,
};

enum class ExternalModItemUseTrigger {
    OnUse,
    HammerGroundImpact,
};

enum class ExternalModItemPlacement {
    Legacy,
    Virtual,
};

enum class ExternalModMovementMode {
    Modifier,
    Surf,
};

enum class ExternalModAimCameraContext {
    CUp,
    Bow,
    Hookshot,
    Slingshot,
    Boomerang,
};

enum class ExternalModAimCameraMode {
    FirstPerson,
    OverShoulder,
};

enum class ExternalModAimMouseButton {
    Left,
    Middle,
    Right,
    Backward,
    Forward,
};

enum class ExternalModAimMouseFireMode {
    Both,
    FirstPerson,
    OverShoulder,
};

enum class ExternalModAimReticleVisibility {
    AimOnly,
    ButtonHold,
    Selected,
};

enum class ExternalModModelUvOrigin {
    Auto,
    BottomLeft,
    TopLeft,
};

enum class ExternalModModelTextureFilter {
    Auto,
    Point,
    Bilerp,
};

enum class ExternalModActorArchetype {
    Npc,
    Prop,
    Trigger,
};

enum class ExternalModRuntimeModuleFormat {
    WasmBinary,
    WatText,
};

struct ExternalModManifest {
    std::string id;
    std::string name;
    std::string version;
    int32_t apiVersion = 0;
    std::string gameVersionMin;
    std::string engineVersionRange;
    std::string entryScript;
    std::vector<std::string> assets;
    struct Dependency {
        std::string modId;
        std::string versionRange;
    };
    std::vector<Dependency> dependencies;
    std::vector<std::string> permissions;
    int32_t loadOrder = 0;
    int32_t loadPriority = 0;
    struct Entrypoints {
        std::string items;
        std::string combat;
        std::string movement;
        std::string camera;
        std::string ui;
        std::string actors;
        std::string quests;
        std::string wasm;
    } entrypoints;

    std::string runtimeType;
    std::string runtimeModule;
    int32_t runtimeMaxMemoryKb = 1024;
    int32_t runtimeMaxCallMs = 2;
    int32_t runtimeMaxFrameBudgetMs = 2;
    int32_t runtimeMaxHookCallsPerFrame = 256;
    int32_t runtimeMaxActorInstances = 64;
    int32_t runtimeMaxActiveStatuses = 256;
    std::string itemDefinitions;
    std::string inputDefinitions;
    std::string hookDefinitions;
    std::string actorDefinitions;
    std::string behaviorDefinitions;
    std::string sceneDefinitions;
    std::string statusDefinitions;
    std::string damageDefinitions;
    std::string targetingDefinitions;
    std::string projectileDefinitions;
    std::string aoeDefinitions;
    std::string movementDefinitions;
    std::string itemUseProfiles;
    std::string vanillaItemPatches;
    std::string cameraDefinitions;
    std::string itemStateDefinitions;
    std::string equippedModelDefinitions;
    std::string hudWidgetDefinitions;
    std::string hudReticleDefinitions;
    std::string effectGraphDefinitions;
    std::string combatHitRuleDefinitions;
    std::string surfDefinitions;
    std::string actorTagDefinitions;
    std::string worldPatchDefinitions;
    std::string questDefinitions;
    std::string dialogDefinitions;
    std::string sdkGeneratorDefinitions;
    std::vector<std::string> capabilities;
};

enum class ExternalModActionType {
    ShowNotification,
    TeleportToEntrance,
    LoadModScene,
    PressButton,
    ShowEquippedItemGet,
    SpawnSmoke,
    SpawnKusa,
    LanternLight,
    ApplyStatus,
    UseItemProfile,
    DealDamage,
    SpawnProjectile,
    SpawnAoE,
    ApplyMovementProfile,
    ApplyImpulse,
    GetGroundInfo,
    Raycast,
    RaycastAll,
    ClearStatus,
    ClearAllStatuses,
    SpawnActor,
    DespawnActor,
    SetActorState,
    MoveActorToPathNode,
    OpenDialog,
    SetSwitchFlag,
    ClearSwitchFlag,
    SetEventChkInf,
    ClearEventChkInf,
    SetInfTable,
    ClearInfTable,
    GiveRupees,
    TakeRupees,
    GrantModItem,
    RevokeModItem,
    SetVar,
    AddVar,
    ClampVar,
    EmitSignal,
    CallBehavior,
    ToggleAimCameraMode,
    SetAimCameraMode,
    SetAimCameraProfile,
    InvokeWasm,
};

enum class ExternalModStatusType {
    Fire,
    Freeze,
    Stun,
    Poison,
    Blind,
    Speed,
    Slow,
    HighJump,
    Strength,
    Weakness,
    Custom,
};

enum class ExternalModStatusTarget {
    FrontTarget,
    Self,
    Player,
    ActorHandle,
};

struct ExternalModAction {
    ExternalModActionType type = ExternalModActionType::ShowNotification;
    std::string text;
    int16_t entranceIndex = 0;
    std::string modSceneId;
    int32_t sceneSpawnId = 0;
    int32_t buttonMask = 0;
    std::string itemId;
    std::string exportName;
    std::vector<int32_t> args;

    std::string actorDefinitionId;
    uint32_t actorHandle = 0;
    std::string actorStateKey;
    std::string actorStateValue;
    int32_t pathNodeIndex = 0;
    int32_t dialogId = 0;
    int32_t intValue = 0;

    std::string variableScope;
    std::string variableKey;
    std::string variableValue;
    float variableNumber = 0.0f;
    float variableMin = 0.0f;
    float variableMax = 0.0f;
    bool variableHasNumber = false;
    bool variableHasRange = false;
    std::string signalName;
    std::string behaviorId;
    ExternalModStatusType statusType = ExternalModStatusType::Freeze;
    ExternalModStatusTarget statusTarget = ExternalModStatusTarget::FrontTarget;
    bool hasStatusTarget = false;
    int32_t durationFrames = 90;
    int32_t tickFrames = 15;
    int32_t damagePerTick = 1;
    int32_t shakeFrames = 12;
    float freezeRange = 180.0f;
    int32_t intensity = 255;
    int32_t blueIntensity = 255;
    float speedMultiplier = 0.5f;
    float jumpMultiplier = 1.5f;
    float strengthMultiplier = 2.0f;
    float weaknessMultiplier = 2.0f;
    float blindSkipChance = 0.35f;
    float blindYawJitterDeg = 20.0f;
    std::string statusId;
    std::string damageProfileId;
    std::string targetingProfileId;
    std::string itemUseProfileId;
    std::string projectileProfileId;
    std::string aoeProfileId;
    std::string movementProfileId;
    std::string aimCameraProfileId;
    ExternalModAimCameraMode aimCameraMode = ExternalModAimCameraMode::FirstPerson;
    std::string patchOperation;
    float range = 180.0f;
    float angle = 0.0f;
    float radius = 0.0f;
    float impulseStrength = 0.0f;
};

struct ExternalModSceneAction {
    int16_t sceneId = 0;
    std::vector<ExternalModAction> actions;
};

struct ExternalModTriggerVolume {
    std::string id;
    int16_t sceneId = 0;
    float minX = 0.0f;
    float minY = 0.0f;
    float minZ = 0.0f;
    float maxX = 0.0f;
    float maxY = 0.0f;
    float maxZ = 0.0f;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
    bool wasInside = false;
    std::vector<ExternalModAction> actions;
};

struct ExternalModInputBinding {
    std::string id;
    int32_t defaultMask = 0;
    ExternalModInputTriggerType defaultTrigger = ExternalModInputTriggerType::Pressed;
    bool allowUserRemap = true;
    std::vector<int32_t> defaultKeyboardScancodes;
};

struct ExternalModCameraHotkeyDefinition {
    std::string id;
    bool allowUserRemap = true;
    std::vector<int32_t> defaultKeyboardScancodes;
    ExternalModAction action;
};

struct ExternalModInputActionTrigger {
    std::string id;
    std::string bindingId;
    ExternalModInputTriggerType trigger = ExternalModInputTriggerType::Pressed;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
    std::vector<ExternalModAction> actions;
};

struct ExternalModItemDefinition {
    std::string id;
    std::string sourceModId;
    std::string displayName;
    ExternalModItemSlot slot = ExternalModItemSlot::Hookshot;
    bool hasSlot = true;
    ExternalModItemPlacement placement = ExternalModItemPlacement::Legacy;
    uint8_t assignableButtonsMask = 0x7F;
    std::string iconAsset;
    std::vector<uint8_t> iconRgba32;
    std::string modelAsset;
    std::string modelTextureAsset;
    std::string modelDisplayList;
    std::string hookshotMetalTextureAsset;
    std::string hookshotHandleTextureAsset;
    std::string hookshotDesignTextureAsset;
    std::string hookshotChainTextureAsset;
    std::string hookshotReticleTextureAsset;
    std::string aimReticleTextureAsset;
    float modelScale = 1.0f;
    ExternalModModelUvOrigin modelUvOrigin = ExternalModModelUvOrigin::Auto;
    ExternalModModelTextureFilter modelTextureFilter = ExternalModModelTextureFilter::Auto;
    int32_t modelTextureTargetWidth = 0;
    int32_t modelTextureTargetHeight = 0;
    struct CustomModelVertex {
        int16_t x = 0;
        int16_t y = 0;
        int16_t z = 0;
        int16_t s = 0;
        int16_t t = 0;
    };
    struct CustomModelTriangle {
        std::array<CustomModelVertex, 3> vertices{};
    };
    std::vector<CustomModelTriangle> customModelTriangles;
    std::vector<uint8_t> modelTextureRgba32;
    int32_t modelTextureWidth = 0;
    int32_t modelTextureHeight = 0;
    bool modelTextureHasTransparency = false;
    std::vector<uint8_t> hookshotMetalTextureRgba16;
    std::vector<uint8_t> hookshotHandleTextureCi8;
    std::vector<uint8_t> hookshotHandleTextureTlutRgba16;
    std::vector<uint8_t> hookshotDesignTextureCi8;
    std::vector<uint8_t> hookshotDesignTextureTlutRgba16;
    std::vector<uint8_t> hookshotChainTextureRgba16;
    std::vector<uint8_t> hookshotReticleTextureI8;
    std::vector<uint8_t> aimReticleTextureI8;
    std::string description;
    std::string onUseExport;
    std::string onUpdateExport;
    std::string onUseBehavior;
    std::string onEquipBehavior;
    std::string useProfile;
    bool aimSelectToggle = false;
    bool aimAttackButtonFire = false;
    ExternalModItemUseTrigger useTrigger = ExternalModItemUseTrigger::OnUse;
    int32_t acquireTextId = 0;
    std::string persistentStateKey;
    ExternalModItemAgePolicy agePolicy = ExternalModItemAgePolicy::AllowChild;
    ExternalModItemUseMode useMode = ExternalModItemUseMode::Vanilla;
    bool hasGrantItemId = false;
    int32_t grantItemId = -1;
    bool hasGrantAmmo = false;
    int32_t grantAmmo = 0;
    std::unordered_map<std::string, float> params;
    bool granted = false;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
};

enum class ExternalModTargetingMode {
    FrontTarget,
    LockedOnTarget,
    Raycast,
    Cone,
    Sphere,
    Self,
    Player,
};

enum class ExternalModAoETargetScope {
    AllNonPlayer,
    EnemiesBosses,
    EnemiesBossesProps,
    PlayerEnemiesBosses,
    AllWithPlayer,
};

enum class ExternalModFreezeMode {
    LegacyTimer,
    IceTrapNoDamage,
};

enum class ExternalModFreezeShellSize {
    Auto,
    Small,
    Medium,
    Large,
};

struct ExternalModFreezeProfile {
    ExternalModFreezeMode mode = ExternalModFreezeMode::LegacyTimer;
    bool spawnIceShell = false;
    ExternalModFreezeShellSize iceShellSize = ExternalModFreezeShellSize::Auto;
    bool lockPosition = true;
    bool lockRotation = false;
    bool playerInputLock = false;
    bool breakEffectOnExpire = true;
};

struct ExternalModStatusDefinition {
    std::string id;
    std::string displayName;
    std::string baseStatusId;
    ExternalModStatusType baseStatusType = ExternalModStatusType::Custom;
    std::string stackingMode = "refresh";
    int32_t maxStacks = 1;
    int32_t durationFrames = 90;
    int32_t tickFrames = 15;
    int32_t damagePerTick = 0;
    int32_t intensity = 255;
    int32_t shakeFrames = 0;
    bool hasFreezeProfile = false;
    ExternalModFreezeProfile freezeProfile;
    std::vector<ExternalModAction> onApply;
    std::vector<ExternalModAction> onTick;
    std::vector<ExternalModAction> onExpire;
};

struct ExternalModDamageProfile {
    std::string id;
    int32_t amount = 1;
    std::string type = "physical";
    int32_t iframesFrames = 0;
    std::string propInteraction = "none";
};

struct ExternalModTargetingProfile {
    std::string id;
    ExternalModTargetingMode mode = ExternalModTargetingMode::FrontTarget;
    float range = 180.0f;
    float radius = 0.0f;
    float angle = 0.0f;
    bool stopOnWall = true;
    bool includeProps = true;
    bool includeEnemies = true;
};

struct ExternalModUseProfileEffect {
    std::string action;
    std::string damageProfileId;
    std::string statusId;
    std::string projectileProfileId;
    std::string aoeProfileId;
    std::string movementProfileId;
    std::string shockwaveOrigin = "player";
    std::array<uint8_t, 4> shockwavePrimColor = { { 255, 255, 255, 255 } };
    std::array<uint8_t, 4> shockwaveEnvColor = { { 200, 200, 200, 255 } };
    int32_t shockwaveLife = 10;
    bool shockwaveSpawnIceSmoke = false;
    int32_t durationFrames = 0;
    int32_t tickFrames = 0;
    int32_t damagePerTick = 0;
    int32_t intensity = 255;
};

struct ExternalModItemUseProfile {
    std::string id;
    std::string targetingProfileId;
    int32_t cooldownFrames = 0;
    std::vector<ExternalModUseProfileEffect> effects;
};

struct ExternalModProjectileProfile {
    std::string id;
    std::string shape = "sphere";
    float speed = 0.0f;
    float gravityScale = 0.0f;
    int32_t lifetimeFrames = 1;
    float radius = 0.0f;
    int32_t maxHits = 1;
    bool stopOnWall = true;
    std::string damageProfileId;
    std::vector<std::string> applyStatuses;
};

struct ExternalModAoEProfile {
    std::string id;
    std::string shape = "sphere";
    ExternalModAoETargetScope targetScope = ExternalModAoETargetScope::AllNonPlayer;
    float range = 0.0f;
    float radius = 0.0f;
    float angle = 0.0f;
    int32_t durationFrames = 1;
    int32_t tickFrames = 0;
    std::vector<ExternalModUseProfileEffect> onEnter;
    std::vector<ExternalModUseProfileEffect> onTick;
    std::vector<ExternalModUseProfileEffect> onExit;
};

struct ExternalModMovementProfile {
    std::string id;
    ExternalModMovementMode mode = ExternalModMovementMode::Modifier;
    int32_t durationFrames = 0;
    float speedMultiplier = 1.0f;
    float accelMultiplier = 1.0f;
    float gravityScale = 1.0f;
    bool boardRequired = true;
    std::string boardSpawnMode = "persistent_under_player";
    std::string idlePose = "stand";
    bool idleLock = true;
    float boardHeightOffset = 10.0f;
    bool boardPitchRollFromGround = true;
    bool boardVisibleWhenIdle = true;
    std::string boardModelAsset;
    float boardScale = 1.0f;
    float surfMaxSpeed = 12.0f;
    float surfDownhillAccel = 0.45f;
    float surfUphillBrake = 0.35f;
    float surfFlatDrag = 0.08f;
    float surfTurnRateDeg = 6.0f;
    float idleSpeedThreshold = 0.10f;
    float surfForwardAccel = 0.20f;
    int32_t surfBoostButtonMask = 0;
    float surfBoostAccel = 0.45f;
    float surfBoostMaxSpeed = 16.0f;
    int32_t surfBoostCooldownFrames = 12;
    float boardPitchOffsetDeg = 0.0f;
    float boardYawOffsetDeg = 0.0f;
    float boardRollOffsetDeg = 0.0f;
    float boardForwardOffset = 0.0f;
    float boardRightOffset = 0.0f;
    float boardUpOffset = 0.0f;
    float riderHeightOffset = 0.0f;
};

struct ExternalModAimCameraProfile {
    std::string id;
    uint8_t contextsMask = 0x1F;
    int16_t cUpFirstPersonMode = 6;
    int16_t bowFirstPersonMode = 7;
    int16_t hookshotFirstPersonMode = 9;
    int16_t slingshotFirstPersonMode = 11;
    int16_t boomerangFirstPersonMode = 7;
    int16_t cUpOverShoulderMode = 8;
    int16_t bowOverShoulderMode = 8;
    int16_t hookshotOverShoulderMode = 8;
    int16_t slingshotOverShoulderMode = 8;
    int16_t boomerangOverShoulderMode = 8;
    std::string shoulder = "right";
    std::string aimRay = "camera_center";
    float reticleX = 0.5f;
    float reticleY = 0.5f;
    bool mouseFireEnabled = false;
    ExternalModAimMouseButton mouseFireButton = ExternalModAimMouseButton::Left;
    ExternalModAimMouseFireMode mouseFireMode = ExternalModAimMouseFireMode::Both;
    ExternalModAimReticleVisibility reticleVisibility = ExternalModAimReticleVisibility::AimOnly;
};

struct ExternalModAimCameraState {
    bool overShoulderEnabled = false;
    std::string activeProfileId;
    std::string activeProfileOwnerModId;
};

struct ExternalModBehaviorCondition {
    std::string type;
    std::string scope;
    std::string key;
    std::string op;
    std::string value;
    float numberValue = 0.0f;
    bool hasNumberValue = false;
};

struct ExternalModBehaviorRule {
    std::vector<ExternalModBehaviorCondition> conditions;
    std::vector<ExternalModAction> actions;
    float randomChance = 1.0f;
};

struct ExternalModBehaviorDefinition {
    std::string id;
    std::unordered_map<std::string, std::vector<ExternalModBehaviorRule>> events;
};

struct ExternalModSceneDefinition {
    std::string id;
    bool hasEntrance = false;
    int16_t entranceIndex = 0;
    bool useNamespacedScene = false;
    std::string sceneResourcePath;
    bool hasHostEntrance = false;
    int16_t hostEntranceIndex = 0;
    bool hasFallbackEntrance = false;
    int16_t fallbackEntranceIndex = 0;
    bool fallbackPlayable = true;
};

struct ExternalModPendingSceneLoadRequest {
    bool pending = false;
    std::string modId;
    std::string sceneId;
    std::string sceneResourcePath;
    int16_t expectedHostSceneId = -1;
    bool hasHostEntrance = false;
    int16_t hostEntranceIndex = 0;
    bool hasFallbackEntrance = false;
    int16_t fallbackEntranceIndex = 0;
    bool fallbackPlayable = true;
    int32_t spawnId = 0;
};

struct ExternalModActorDefinition {
    std::string id;
    ExternalModActorArchetype archetype = ExternalModActorArchetype::Npc;
    int16_t sceneId = -1;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
    int32_t maxInstances = 1;
    int32_t tickRate = 1;
    float lodDistance = 5000.0f;
    bool interactable = false;
    float interactDistance = 80.0f;
    std::string behaviorId;
    std::vector<std::string> components;
    std::string exportOnInit;
    std::string exportOnUpdate;
    std::string exportOnInteract;
    std::string exportOnDestroy;
};

struct ExternalModActorInstance {
    uint32_t handle = 0;
    std::string definitionId;
    bool active = false;
    int16_t sceneId = -1;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
    int32_t tickCounter = 0;
    bool wasNearPlayer = false;
    bool wasInteracting = false;
    int32_t timerFrames = 0;
    std::unordered_map<std::string, std::string> state;
};

enum class ExternalModHookDispatchType {
    Actions,
    WasmExport,
};

enum class ExternalModHookType {
    OnLoadGame,
    OnExitGame,
    OnSceneInit,
    AfterSceneCommands,
    OnTransitionEnd,
    OnFlagSet,
    OnFlagUnset,
    OnSceneFlagSet,
    OnSceneFlagUnset,
    OnPlayerUpdate,
    OnPlayerUseItem,
    OnPlayerHealthChange,
    OnItemReceive,
    OnActorInit,
    OnActorSpawn,
    OnActorUpdate,
    OnActorKill,
    OnActorDestroy,
    OnEnemyDefeat,
    OnBossDefeat,
    OnPlayDestroy,
    OnGameFrameUpdate,
};

struct ExternalModHookFilter {
    bool hasScene = false;
    int16_t scene = 0;
    bool hasActorId = false;
    int16_t actorId = 0;
    bool hasCategory = false;
    int16_t category = 0;
    bool hasItemId = false;
    int16_t itemId = 0;
    bool hasFlagType = false;
    int16_t flagType = 0;
    bool hasFlagId = false;
    int16_t flagId = 0;
    bool hasHealthDeltaRange = false;
    int16_t healthDeltaMin = 0;
    int16_t healthDeltaMax = 0;
};

struct ExternalModHookSubscription {
    std::string id;
    ExternalModHookType hook = ExternalModHookType::OnGameFrameUpdate;
    ExternalModHookDispatchType dispatch = ExternalModHookDispatchType::Actions;
    std::vector<ExternalModAction> actions;
    std::string wasmExport;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
    ExternalModHookFilter filters;
};

struct ExternalModRuntime {
    bool enabled = false;
    int32_t apiVersion = 1;
    std::vector<ExternalModAction> onGameLoadedActions;
    std::vector<ExternalModSceneAction> onSceneInitActions;
    std::vector<ExternalModTriggerVolume> frameTriggers;
    std::vector<ExternalModInputBinding> inputBindings;
    std::vector<ExternalModCameraHotkeyDefinition> cameraHotkeys;
    std::vector<ExternalModInputActionTrigger> inputTriggers;
    std::vector<ExternalModItemDefinition> itemDefinitions;
    std::vector<ExternalModHookSubscription> hookSubscriptions;
    std::vector<ExternalModActorDefinition> actorDefinitions;
    std::vector<ExternalModActorInstance> actorInstances;
    std::vector<ExternalModBehaviorDefinition> behaviorDefinitions;
    std::vector<ExternalModSceneDefinition> sceneDefinitions;
    std::vector<ExternalModStatusDefinition> statusDefinitions;
    std::vector<ExternalModDamageProfile> damageProfiles;
    std::vector<ExternalModTargetingProfile> targetingProfiles;
    std::vector<ExternalModItemUseProfile> itemUseProfiles;
    std::vector<ExternalModProjectileProfile> projectileProfiles;
    std::vector<ExternalModAoEProfile> aoeProfiles;
    std::vector<ExternalModMovementProfile> movementProfiles;
    std::vector<ExternalModAimCameraProfile> cameraProfiles;
    bool hasEffectImpactPosition = false;
    float effectImpactPosX = 0.0f;
    float effectImpactPosY = 0.0f;
    float effectImpactPosZ = 0.0f;
    bool useProfileSpawnedShockwave = false;
    struct StatusEffectState {
        uintptr_t actorAddress = 0;
        int16_t actorId = -1;
        std::string statusId;
        std::string sourceModId;
        ExternalModStatusType statusType = ExternalModStatusType::Freeze;
        bool isPlayerTarget = false;
        int32_t framesRemaining = 0;
        int32_t totalDurationFrames = 0;
        int32_t tickFrames = 15;
        int32_t tickCountdown = 15;
        int32_t damagePerTick = 1;
        int32_t shakeFrames = 0;
        int32_t intensity = 255;
        float speedMultiplier = 0.5f;
        float jumpMultiplier = 1.5f;
        float strengthMultiplier = 2.0f;
        float weaknessMultiplier = 2.0f;
        float blindSkipChance = 0.35f;
        float blindYawJitterDeg = 20.0f;
        float baseX = 0.0f;
        float baseY = 0.0f;
        float baseZ = 0.0f;
        int16_t baseRotX = 0;
        int16_t baseRotY = 0;
        int16_t baseRotZ = 0;
        bool fallbackLogged = false;
        int32_t stacks = 1;
        bool justApplied = true;
        std::string stackingMode = "refresh";
        int32_t maxStacks = 1;
        ExternalModFreezeProfile freezeProfile;
        uintptr_t freezeShellActorAddress = 0;
    };
    struct ActiveAoEState {
        std::string profileId;
        std::string sourceItemId;
        int32_t framesRemaining = 0;
        int32_t tickFrames = 1;
        int32_t tickCountdown = 1;
        float originX = 0.0f;
        float originY = 0.0f;
        float originZ = 0.0f;
    };
    struct SurfState {
        bool active = false;
        std::string sourceModId;
        std::string movementProfileId;
        std::string sourceItemId;
        int32_t framesRemaining = 0;
        float speed = 0.0f;
        int16_t headingYaw = 0;
        float boardPosX = 0.0f;
        float boardPosY = 0.0f;
        float boardPosZ = 0.0f;
        int16_t boardRotX = 0;
        int16_t boardRotY = 0;
        int16_t boardRotZ = 0;
        float boardScale = 1.0f;
        int32_t boostCooldownRemaining = 0;
        bool idle = false;
    };
    std::vector<StatusEffectState> statusEffects;
    std::vector<ActiveAoEState> activeAoEs;
    SurfState surfState;
    std::unordered_map<std::string, std::string> globalBlackboard;
    std::unordered_map<int16_t, std::unordered_map<std::string, std::string>> sceneBlackboard;
    std::vector<std::pair<uint32_t, std::string>> pendingSignals;
    int16_t lastSceneSeen = -1;
    int16_t lastRoomSeen = -1;
    bool hasLastDayNight = false;
    bool lastIsNight = false;
    bool switchSnapshotInitialized = false;
    std::array<uint8_t, 64> switchSnapshot{};
    uint32_t nextActorHandle = 1;
    int32_t frameBudgetMs = 2;
    int32_t maxHookCallsPerFrame = 256;
    int32_t hookCallsThisFrame = 0;
    int32_t maxActorInstances = 64;
    int32_t wasmCallsThisFrame = 0;
    int32_t wasmBudgetDropsThisFrame = 0;
    std::unordered_map<int32_t, uintptr_t> wasmTargetHandles;
    int32_t wasmNextTargetHandle = 1;
    int32_t behaviorMaxStepsPerActorPerFrame = 64;
    int32_t behaviorMaxStepsPerModPerFrame = 5000;
    int32_t maxActiveStatusEffects = 256;
    int32_t behaviorStepsThisFrame = 0;
    ExternalModRuntimeModuleFormat moduleFormat = ExternalModRuntimeModuleFormat::WasmBinary;
    std::string moduleSourcePath;
    size_t compiledModuleSizeBytes = 0;
    int32_t moduleCompileTimeMs = 0;
    std::string moduleCompileDiagnostics;
    std::vector<std::filesystem::path> generatedAssetPaths;
    std::vector<std::shared_ptr<Ship::Archive>> generatedAssetArchives;
    std::unique_ptr<ExternalModWasmRuntime> wasmRuntime;
};

struct ExternalModPackage {
    std::filesystem::path sourcePath;
    bool isZip = false;
    bool valid = false;
    std::string error;
    ExternalModManifest manifest;
    std::vector<std::filesystem::path> mountedAssets;
    ExternalModRuntime runtime;
};

struct ExternalModHookEventContext {
    int16_t scene = -1;
    int16_t actorId = -1;
    int16_t actorCategory = -1;
    int16_t itemId = -1;
    int16_t flagType = -1;
    int16_t flagId = -1;
    int16_t healthDelta = 0;
};

struct ExternalModInventoryCellView {
    size_t index = 0;
    bool occupied = false;
    std::string modId;
    std::string modName;
    std::string itemId;
    std::string displayName;
    ExternalModItemSlot slot = ExternalModItemSlot::Hookshot;
    bool hasSlot = true;
    ExternalModItemPlacement placement = ExternalModItemPlacement::Legacy;
    bool granted = false;
    uint8_t assignableButtonsMask = 0x7F;
    uint8_t assignedButtonsMask = 0;
    const uint8_t* iconRgba32 = nullptr;
};

} // namespace SOH
