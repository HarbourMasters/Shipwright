#include "ExternalModWasmRuntime.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_map>

#if defined(SOH_HAS_WABT)
#include <wabt/binary-reader.h>
#include <wabt/error-formatter.h>
#include <wabt/interp/binary-reader-interp.h>
#include <wabt/interp/interp.h>
#endif

namespace SOH {
namespace {
constexpr std::array<uint8_t, 4> kWasmMagic = { 0x00, 0x61, 0x73, 0x6D };
constexpr std::array<uint8_t, 4> kWasmVersion = { 0x01, 0x00, 0x00, 0x00 };
constexpr size_t kMaxArgs = 16;

bool IsSafeExportName(const std::string& name) {
    if (name.empty() || name.size() > 64) {
        return false;
    }
    return std::all_of(name.begin(), name.end(), [](unsigned char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    });
}

bool IsSupportedHostModule(const std::string& moduleName) {
    return moduleName == "env" || moduleName == "host";
}

} // namespace

struct ExternalModWasmRuntime::Impl {
#if defined(SOH_HAS_WABT)
    std::unique_ptr<wabt::interp::Store> store;
    wabt::interp::Module::Ptr module;
    wabt::interp::Instance::Ptr instance;
    std::vector<wabt::interp::HostFunc::Ptr> hostFuncs;
    std::unordered_map<std::string, wabt::interp::Func::Ptr> exports;
#endif
};

ExternalModWasmRuntime::ExternalModWasmRuntime() = default;
ExternalModWasmRuntime::~ExternalModWasmRuntime() = default;

void ExternalModWasmRuntime::BeginFrame() {
    mCallsThisFrame = 0;
    mBudgetDropsThisFrame = 0;
    mFrameSpentMs = 0;
}

int32_t ExternalModWasmRuntime::GetCallsThisFrame() const {
    return mCallsThisFrame;
}

int32_t ExternalModWasmRuntime::GetBudgetDropsThisFrame() const {
    return mBudgetDropsThisFrame;
}

bool ExternalModWasmRuntime::Initialize(const std::vector<uint8_t>& moduleBytes, const ExternalModWasmConfig& config,
                                        std::string& outError) {
    mInitialized = false;
    mModuleBytes.clear();
    mImpl.reset();

    mConfig = config;
    mConfig.maxMemoryKb = std::clamp(config.maxMemoryKb, 64, 4096);
    mConfig.maxCallMs = std::clamp(config.maxCallMs, 1, 4);
    mConfig.maxFrameBudgetMs = std::clamp(config.maxFrameBudgetMs, 1, 8);

    BeginFrame();

    if (moduleBytes.size() < 8) {
        outError = "WASM module is too small";
        return false;
    }

    const auto memoryCapBytes = static_cast<size_t>(mConfig.maxMemoryKb) * 1024u;
    if (moduleBytes.size() > memoryCapBytes) {
        outError = "WASM module exceeds configured maxMemoryKb";
        return false;
    }

    if (!std::equal(kWasmMagic.begin(), kWasmMagic.end(), moduleBytes.begin()) ||
        !std::equal(kWasmVersion.begin(), kWasmVersion.end(), moduleBytes.begin() + 4)) {
        outError = "Invalid WASM binary header";
        return false;
    }

    mModuleBytes = moduleBytes;

#if !defined(SOH_HAS_WABT)
    mInitialized = true;
    return true;
#else
    using namespace wabt;
    using namespace wabt::interp;

    auto impl = std::make_unique<Impl>();
    impl->store = std::make_unique<Store>();

    ModuleDesc moduleDesc;
    Errors errors;
    ReadBinaryOptions options;
    options.stop_on_first_error = true;
    options.read_debug_names = true;

    if (Failed(ReadBinaryInterp(mConfig.modId, mModuleBytes.data(), mModuleBytes.size(), options, &errors, &moduleDesc))) {
        outError = errors.empty() ? "ReadBinaryInterp failed" : FormatErrorsToString(errors, Location::Type::Binary);
        return false;
    }

    impl->module = Module::New(*impl->store, std::move(moduleDesc));
    if (!impl->module) {
        outError = "Failed to allocate wasm module in interpreter store";
        return false;
    }

    RefVec importRefs;
    importRefs.reserve(impl->module->desc().imports.size());

    auto makeTrapError = [&](const std::string& message) {
        outError = message;
        return false;
    };

    auto hostInvoke = [this, implPtr = impl.get()](const std::string& moduleName, const std::string& funcName,
                                                    Thread& thread, const Values& params, Values& results,
                                                    Trap::Ptr* outTrap) -> Result {
        auto writeTrap = [&](const std::string& msg) -> Result {
            if (outTrap != nullptr) {
                *outTrap = Trap::New(*implPtr->store, msg);
            }
            return Result::Error;
        };

        if (!IsSupportedHostModule(moduleName)) {
            return writeTrap("Unsupported host import module: " + moduleName);
        }

        Instance* callerInstance = thread.GetCallerInstance();
        if (callerInstance == nullptr) {
            return writeTrap("Host import called without caller instance");
        }

        if (callerInstance->memories().empty()) {
            return writeTrap("Host import requires module memory");
        }

        auto memory = implPtr->store->UnsafeGet<Memory>(callerInstance->memories().front());
        if (!memory) {
            return writeTrap("Failed to access module memory for host import");
        }

        auto readStringArg = [&](size_t ptrIndex, size_t lenIndex, std::string& outValue) -> bool {
            if (ptrIndex >= params.size() || lenIndex >= params.size()) {
                return false;
            }
            const uint32_t ptr = params[ptrIndex].Get<u32>();
            const uint32_t len = params[lenIndex].Get<u32>();
            if (len == 0) {
                outValue.clear();
                return true;
            }
            if (!memory->IsValidAccess(ptr, 0, len)) {
                return false;
            }
            const auto* data = memory->UnsafeData() + ptr;
            outValue.assign(reinterpret_cast<const char*>(data), reinterpret_cast<const char*>(data) + len);
            return true;
        };

        auto writeI32Result = [&](int32_t value) -> Result {
            if (!results.empty()) {
                results[0] = Value::Make(static_cast<u32>(value));
            }
            return Result::Ok;
        };

        if (funcName == "host_useItemProfile") {
            std::string itemId;
            if (!readStringArg(0, 1, itemId)) {
                return writeTrap("host_useItemProfile invalid memory string args");
            }
            if (!mConfig.hostApi.useItemProfile) {
                return writeTrap("host_useItemProfile is not bound in runtime host API");
            }
            return writeI32Result(mConfig.hostApi.useItemProfile(itemId));
        }

        if (funcName == "host_resolveTarget") {
            std::string profileId;
            if (!readStringArg(0, 1, profileId)) {
                return writeTrap("host_resolveTarget invalid profile string args");
            }

            if (params.size() < 4) {
                return writeTrap("host_resolveTarget expects 4 parameters");
            }
            const uint32_t outPtr = params[2].Get<u32>();
            const int32_t outCapacity = static_cast<int32_t>(params[3].Get<u32>());

            if (!mConfig.hostApi.resolveTarget) {
                return writeTrap("host_resolveTarget is not bound in runtime host API");
            }

            std::vector<int32_t> handles;
            const int32_t hostResult = mConfig.hostApi.resolveTarget(profileId, handles);
            if (hostResult < 0) {
                return writeI32Result(hostResult);
            }

            const int32_t clampedCapacity = std::max(0, outCapacity);
            const int32_t writeCount = std::min<int32_t>(clampedCapacity, static_cast<int32_t>(handles.size()));
            const size_t bytesToWrite = static_cast<size_t>(writeCount) * sizeof(int32_t);
            if (writeCount > 0) {
                if (!memory->IsValidAccess(outPtr, 0, bytesToWrite)) {
                    return writeTrap("host_resolveTarget output buffer out of bounds");
                }
                std::memcpy(memory->UnsafeData() + outPtr, handles.data(), bytesToWrite);
            }
            return writeI32Result(writeCount);
        }

        if (funcName == "host_dealDamage") {
            if (params.size() < 3) {
                return writeTrap("host_dealDamage expects 3 parameters");
            }
            const int32_t targetHandle = static_cast<int32_t>(params[0].Get<u32>());
            std::string damageProfileId;
            if (!readStringArg(1, 2, damageProfileId)) {
                return writeTrap("host_dealDamage invalid damageProfileId string args");
            }
            if (!mConfig.hostApi.dealDamage) {
                return writeTrap("host_dealDamage is not bound in runtime host API");
            }
            return writeI32Result(mConfig.hostApi.dealDamage(targetHandle, damageProfileId));
        }

        if (funcName == "host_applyStatus") {
            if (params.size() < 4) {
                return writeTrap("host_applyStatus expects 4 parameters");
            }
            const int32_t targetHandle = static_cast<int32_t>(params[0].Get<u32>());
            std::string statusId;
            if (!readStringArg(1, 2, statusId)) {
                return writeTrap("host_applyStatus invalid statusId string args");
            }
            const int32_t durationOverrideFrames = static_cast<int32_t>(params[3].Get<u32>());
            if (!mConfig.hostApi.applyStatus) {
                return writeTrap("host_applyStatus is not bound in runtime host API");
            }
            return writeI32Result(mConfig.hostApi.applyStatus(targetHandle, statusId, durationOverrideFrames));
        }

        if (funcName == "host_spawnProjectile") {
            if (params.size() < 4) {
                return writeTrap("host_spawnProjectile expects 4 parameters");
            }
            std::string profileId;
            std::string overrides;
            if (!readStringArg(0, 1, profileId) || !readStringArg(2, 3, overrides)) {
                return writeTrap("host_spawnProjectile invalid profile/overrides string args");
            }
            if (!mConfig.hostApi.spawnProjectile) {
                return writeTrap("host_spawnProjectile is not bound in runtime host API");
            }
            return writeI32Result(mConfig.hostApi.spawnProjectile(profileId, overrides));
        }

        if (funcName == "host_spawnAoE") {
            if (params.size() < 4) {
                return writeTrap("host_spawnAoE expects 4 parameters");
            }
            std::string profileId;
            std::string origin;
            if (!readStringArg(0, 1, profileId) || !readStringArg(2, 3, origin)) {
                return writeTrap("host_spawnAoE invalid profile/origin string args");
            }
            if (!mConfig.hostApi.spawnAoE) {
                return writeTrap("host_spawnAoE is not bound in runtime host API");
            }
            return writeI32Result(mConfig.hostApi.spawnAoE(profileId, origin));
        }

        if (funcName == "host_applyMovementProfile") {
            if (params.size() < 3) {
                return writeTrap("host_applyMovementProfile expects 3 parameters");
            }
            std::string profileId;
            if (!readStringArg(0, 1, profileId)) {
                return writeTrap("host_applyMovementProfile invalid profile string args");
            }
            const int32_t durationFrames = static_cast<int32_t>(params[2].Get<u32>());
            if (!mConfig.hostApi.applyMovementProfile) {
                return writeTrap("host_applyMovementProfile is not bound in runtime host API");
            }
            return writeI32Result(mConfig.hostApi.applyMovementProfile(profileId, durationFrames));
        }

        if (funcName == "host_applyImpulse") {
            if (params.size() < 3) {
                return writeTrap("host_applyImpulse expects 3 parameters");
            }
            const int32_t mode = static_cast<int32_t>(params[0].Get<u32>());
            const float strength = params[1].Get<f32>();
            const uint32_t vecPtr = params[2].Get<u32>();

            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            if (vecPtr != 0) {
                if (!memory->IsValidAccess(vecPtr, 0, sizeof(float) * 3u)) {
                    return writeTrap("host_applyImpulse vector pointer out of bounds");
                }
                std::memcpy(&x, memory->UnsafeData() + vecPtr, sizeof(float));
                std::memcpy(&y, memory->UnsafeData() + vecPtr + sizeof(float), sizeof(float));
                std::memcpy(&z, memory->UnsafeData() + vecPtr + sizeof(float) * 2u, sizeof(float));
            }

            if (!mConfig.hostApi.applyImpulse) {
                return writeTrap("host_applyImpulse is not bound in runtime host API");
            }
            return writeI32Result(mConfig.hostApi.applyImpulse(mode, strength, x, y, z));
        }

        if (funcName == "host_getGroundInfo") {
            if (params.empty()) {
                return writeTrap("host_getGroundInfo expects 1 parameter");
            }
            const uint32_t outStructPtr = params[0].Get<u32>();
            if (!mConfig.hostApi.getGroundInfo) {
                return writeTrap("host_getGroundInfo is not bound in runtime host API");
            }

            ExternalModWasmGroundInfo info;
            const int32_t hostResult = mConfig.hostApi.getGroundInfo(info);
            if (hostResult < 0) {
                return writeI32Result(hostResult);
            }

            if (!memory->IsValidAccess(outStructPtr, 0, sizeof(ExternalModWasmGroundInfo))) {
                return writeTrap("host_getGroundInfo output pointer out of bounds");
            }
            std::memcpy(memory->UnsafeData() + outStructPtr, &info, sizeof(ExternalModWasmGroundInfo));
            return writeI32Result(0);
        }

        if (funcName == "host_raycast") {
            if (params.size() < 3) {
                return writeTrap("host_raycast expects 3 parameters");
            }
            std::string query;
            if (!readStringArg(0, 1, query)) {
                return writeTrap("host_raycast invalid query string args");
            }
            const uint32_t outHitPtr = params[2].Get<u32>();
            if (!mConfig.hostApi.raycast) {
                return writeTrap("host_raycast is not bound in runtime host API");
            }

            ExternalModWasmRaycastHit hit;
            const int32_t hostResult = mConfig.hostApi.raycast(query, hit);
            if (hostResult < 0) {
                return writeI32Result(hostResult);
            }

            if (!memory->IsValidAccess(outHitPtr, 0, sizeof(ExternalModWasmRaycastHit))) {
                return writeTrap("host_raycast output pointer out of bounds");
            }
            std::memcpy(memory->UnsafeData() + outHitPtr, &hit, sizeof(ExternalModWasmRaycastHit));
            return writeI32Result(hostResult);
        }

        if (funcName == "host_raycastAll") {
            if (params.size() < 4) {
                return writeTrap("host_raycastAll expects 4 parameters");
            }
            std::string query;
            if (!readStringArg(0, 1, query)) {
                return writeTrap("host_raycastAll invalid query string args");
            }

            const uint32_t outHitsPtr = params[2].Get<u32>();
            const int32_t outCapacity = static_cast<int32_t>(params[3].Get<u32>());
            const int32_t clampedCapacity = std::max(0, outCapacity);

            if (!mConfig.hostApi.raycastAll) {
                return writeTrap("host_raycastAll is not bound in runtime host API");
            }

            std::vector<ExternalModWasmRaycastHit> hits;
            const int32_t hostResult = mConfig.hostApi.raycastAll(query, clampedCapacity, hits);
            if (hostResult < 0) {
                return writeI32Result(hostResult);
            }

            const int32_t writeCount = std::min<int32_t>(clampedCapacity, static_cast<int32_t>(hits.size()));
            if (writeCount > 0) {
                const size_t bytesToWrite = static_cast<size_t>(writeCount) * sizeof(ExternalModWasmRaycastHit);
                if (!memory->IsValidAccess(outHitsPtr, 0, bytesToWrite)) {
                    return writeTrap("host_raycastAll output buffer out of bounds");
                }
                std::memcpy(memory->UnsafeData() + outHitsPtr, hits.data(), bytesToWrite);
            }
            return writeI32Result(writeCount);
        }

        return writeTrap("Unsupported host import function: " + moduleName + "." + funcName);
    };

    for (const auto& importDesc : impl->module->desc().imports) {
        if (importDesc.type.type->kind != ExternKind::Func) {
            return makeTrapError("Unsupported non-function import: " + importDesc.type.module + "." + importDesc.type.name);
        }

        if (!IsSupportedHostModule(importDesc.type.module)) {
            return makeTrapError("Unsupported import module: " + importDesc.type.module +
                                 " (supported: env, host)");
        }

        auto* funcType = wabt::cast<FuncType>(importDesc.type.type.get());
        auto hostFunc = HostFunc::New(
            *impl->store, *funcType,
            [moduleName = importDesc.type.module, funcName = importDesc.type.name, hostInvoke](Thread& thread,
                                                                                                 const Values& params,
                                                                                                 Values& results,
                                                                                                 Trap::Ptr* outTrap) -> Result {
                return hostInvoke(moduleName, funcName, thread, params, results, outTrap);
            });

        impl->hostFuncs.push_back(hostFunc);
        importRefs.push_back(hostFunc->self());
    }

    Trap::Ptr trap;
    impl->instance = Instance::Instantiate(*impl->store, impl->module.ref(), importRefs, &trap);
    if (!impl->instance) {
        outError = trap ? trap->message() : "Failed to instantiate wasm module";
        return false;
    }

    for (const auto& exportDesc : impl->module->desc().exports) {
        if (exportDesc.type.type->kind != ExternKind::Func) {
            continue;
        }
        if (exportDesc.index >= impl->instance->funcs().size()) {
            continue;
        }
        auto func = impl->store->UnsafeGet<Func>(impl->instance->funcs()[exportDesc.index]);
        if (func) {
            impl->exports[exportDesc.type.name] = func;
        }
    }

    mImpl = std::move(impl);
    mInitialized = true;
    return true;
#endif
}

bool ExternalModWasmRuntime::InvokeExport(const std::string& exportName, const std::vector<int32_t>& args,
                                          std::string& outError) {
    if (!mInitialized) {
        outError = "WASM runtime not initialized";
        return false;
    }
    if (!IsSafeExportName(exportName)) {
        outError = "Invalid export name";
        return false;
    }
    if (args.size() > kMaxArgs) {
        outError = "Too many arguments for WASM export";
        return false;
    }

    if (mFrameSpentMs >= mConfig.maxFrameBudgetMs) {
        ++mBudgetDropsThisFrame;
        outError = "WASM frame budget exceeded before call";
        return false;
    }

    const auto start = std::chrono::steady_clock::now();

#if !defined(SOH_HAS_WABT)
    const auto elapsedMs = static_cast<int32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count());
    mFrameSpentMs += elapsedMs;
    ++mCallsThisFrame;
    if (elapsedMs > mConfig.maxCallMs) {
        ++mBudgetDropsThisFrame;
        outError = "WASM call budget exceeded";
        return false;
    }
    return true;
#else
    using namespace wabt::interp;

    if (!mImpl || !mImpl->store || !mImpl->instance) {
        outError = "WASM interpreter state is not ready";
        return false;
    }

    const auto exportIt = mImpl->exports.find(exportName);
    if (exportIt == mImpl->exports.end() || !exportIt->second) {
        outError = "WASM export not found: " + exportName;
        return false;
    }

    auto func = exportIt->second;
    const auto& type = func->type();

    if (args.size() > type.params.size()) {
        outError = "WASM export argument overflow for '" + exportName + "'";
        return false;
    }

    Values params;
    params.reserve(type.params.size());
    for (size_t i = 0; i < type.params.size(); ++i) {
        const int32_t arg = i < args.size() ? args[i] : 0;
        const auto paramType = type.params[i];
        switch (paramType) {
            case wabt::Type::I32:
                params.push_back(Value::Make(static_cast<u32>(arg)));
                break;
            case wabt::Type::I64:
                params.push_back(Value::Make(static_cast<u64>(arg)));
                break;
            case wabt::Type::F32:
                params.push_back(Value::Make(static_cast<f32>(arg)));
                break;
            case wabt::Type::F64:
                params.push_back(Value::Make(static_cast<f64>(arg)));
                break;
            default:
                outError = "Unsupported wasm param type in export '" + exportName + "'";
                return false;
        }
    }

    Values results(type.results.size());
    Trap::Ptr trap;
    if (Failed(func->Call(*mImpl->store, params, results, &trap))) {
        outError = trap ? trap->message() : "WASM export call failed";
        return false;
    }

    const auto elapsedMs = static_cast<int32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count());
    mFrameSpentMs += elapsedMs;
    ++mCallsThisFrame;

    if (elapsedMs > mConfig.maxCallMs) {
        ++mBudgetDropsThisFrame;
        outError = "WASM call budget exceeded";
        return false;
    }
    if (mFrameSpentMs > mConfig.maxFrameBudgetMs) {
        ++mBudgetDropsThisFrame;
        outError = "WASM frame budget exceeded";
        return false;
    }

    return true;
#endif
}

bool ExternalModWasmRuntime::InvokeExport(const std::string& exportName, std::string& outError) {
    static const std::vector<int32_t> kNoArgs;
    return InvokeExport(exportName, kNoArgs, outError);
}

bool ExternalModWasmRuntime::IsInitialized() const {
    return mInitialized;
}

const ExternalModWasmConfig& ExternalModWasmRuntime::GetConfig() const {
    return mConfig;
}

} // namespace SOH
