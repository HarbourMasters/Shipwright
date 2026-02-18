#pragma once

#include <cstdint>

namespace SOH {

class ExternalModItemRuntime {
  public:
    static bool IsHookshotItemId(int32_t itemId);
    static void ApplySkyhookImpulse(void* playerPtr, float pullForce, float speed);
};

} // namespace SOH
