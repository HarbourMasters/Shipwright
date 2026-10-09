#pragma once

#include <cstdint>
#include <vector>
#include <ship/resource/Resource.h>
#include "SceneCommand.h"
#include "2s2h/resource/type/Scene.h"

namespace SOH {
class SetAlternateHeaders : public SceneCommand<void> {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    uint32_t numHeaders;
    std::vector<std::shared_ptr<Scene>> headers;
};
}; // namespace SOH
