#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include <string>
#include "SceneCommand.h"
#include "soh/resource/type/Scene.h"

namespace SOH {

class SetAlternateHeaders : public SceneCommand<void> {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    uint32_t numHeaders;
    std::vector<std::string> headerFileNames;
    std::vector<std::shared_ptr<Scene>> headers;
};
}; // namespace SOH
