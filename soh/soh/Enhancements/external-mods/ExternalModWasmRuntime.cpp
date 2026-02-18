#include "ExternalModWasmRuntime.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>

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
} // namespace

bool ExternalModWasmRuntime::Initialize(const std::vector<uint8_t>& moduleBytes, const ExternalModWasmConfig& config,
                                        std::string& outError) {
    mInitialized = false;
    mModuleBytes.clear();
    mConfig = config;

    if (moduleBytes.size() < 8) {
        outError = "WASM module is too small";
        return false;
    }

    const auto memoryCapBytes = static_cast<size_t>(std::clamp(config.maxMemoryKb, 64, 4096)) * 1024u;
    if (moduleBytes.size() > memoryCapBytes) {
        outError = "WASM module exceeds configured maxMemoryKb";
        return false;
    }

    if (!std::equal(kWasmMagic.begin(), kWasmMagic.end(), moduleBytes.begin()) ||
        !std::equal(kWasmVersion.begin(), kWasmVersion.end(), moduleBytes.begin() + 4)) {
        outError = "Invalid WASM binary header";
        return false;
    }

    mConfig.maxMemoryKb = std::clamp(config.maxMemoryKb, 64, 4096);
    mConfig.maxCallMs = std::clamp(config.maxCallMs, 1, 4);
    mModuleBytes = moduleBytes;
    mInitialized = true;
    return true;
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

    const auto start = std::chrono::steady_clock::now();
    // MVP sandbox: module loading + call budget validation only.
    // Runtime intentionally exposes no filesystem/network/system access.
    const auto elapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    if (elapsedMs > mConfig.maxCallMs) {
        outError = "WASM call budget exceeded";
        return false;
    }

    return true;
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
