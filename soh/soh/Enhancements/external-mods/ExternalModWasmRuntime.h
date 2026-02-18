#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace SOH {

struct ExternalModWasmConfig {
    std::string modId;
    int32_t maxMemoryKb = 1024;
    int32_t maxCallMs = 2;
};

class ExternalModWasmRuntime {
  public:
    bool Initialize(const std::vector<uint8_t>& moduleBytes, const ExternalModWasmConfig& config, std::string& outError);
    bool InvokeExport(const std::string& exportName, const std::vector<int32_t>& args, std::string& outError);
    bool InvokeExport(const std::string& exportName, std::string& outError);

    bool IsInitialized() const;
    const ExternalModWasmConfig& GetConfig() const;

  private:
    ExternalModWasmConfig mConfig;
    std::vector<uint8_t> mModuleBytes;
    bool mInitialized = false;
};

} // namespace SOH
