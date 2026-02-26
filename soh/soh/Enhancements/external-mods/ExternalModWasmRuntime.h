#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace SOH {

struct ExternalModWasmGroundInfo {
    float slopeAngleDeg = 0.0f;
    float normalX = 0.0f;
    float normalY = 1.0f;
    float normalZ = 0.0f;
    int32_t surfaceType = 0;
    int32_t isOnGround = 0;
};

struct ExternalModWasmRaycastHit {
    int32_t hasHit = 0;
    int32_t actorHandle = 0;
    float distance = 0.0f;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
};

struct ExternalModWasmHostApi {
    std::function<int32_t(const std::string&)> useItemProfile;
    std::function<int32_t(const std::string&, std::vector<int32_t>&)> resolveTarget;
    std::function<int32_t(int32_t, const std::string&)> dealDamage;
    std::function<int32_t(int32_t, const std::string&, int32_t)> applyStatus;
    std::function<int32_t(const std::string&, const std::string&)> spawnProjectile;
    std::function<int32_t(const std::string&, const std::string&)> spawnAoE;
    std::function<int32_t(const std::string&, int32_t)> applyMovementProfile;
    std::function<int32_t(int32_t, float, float, float, float)> applyImpulse;
    std::function<int32_t(ExternalModWasmGroundInfo&)> getGroundInfo;
    std::function<int32_t(const std::string&, ExternalModWasmRaycastHit&)> raycast;
    std::function<int32_t(const std::string&, int32_t, std::vector<ExternalModWasmRaycastHit>&)> raycastAll;
};

struct ExternalModWasmConfig {
    std::string modId;
    int32_t maxMemoryKb = 1024;
    int32_t maxCallMs = 2;
    int32_t maxFrameBudgetMs = 2;
    ExternalModWasmHostApi hostApi;
};

class ExternalModWasmRuntime {
  public:
    ExternalModWasmRuntime();
    ~ExternalModWasmRuntime();

    bool Initialize(const std::vector<uint8_t>& moduleBytes, const ExternalModWasmConfig& config, std::string& outError);
    bool InvokeExport(const std::string& exportName, const std::vector<int32_t>& args, std::string& outError);
    bool InvokeExport(const std::string& exportName, std::string& outError);

    bool IsInitialized() const;
    const ExternalModWasmConfig& GetConfig() const;
    void BeginFrame();
    int32_t GetCallsThisFrame() const;
    int32_t GetBudgetDropsThisFrame() const;

  private:
    ExternalModWasmConfig mConfig;
    std::vector<uint8_t> mModuleBytes;
    bool mInitialized = false;
    int32_t mCallsThisFrame = 0;
    int32_t mBudgetDropsThisFrame = 0;
    int32_t mFrameSpentMs = 0;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

} // namespace SOH
