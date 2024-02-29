#pragma once

#include <memory>
#include "Resource.h"
#include "ResourceFactory.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"

namespace SOH {
class SceneCommandFactoryBinaryV0 {
    public:
        virtual std::shared_ptr<LUS::IResource> ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) = 0;

    protected:
        void ReadCommandId(std::shared_ptr<ISceneCommand> command, std::shared_ptr<LUS::BinaryReader> reader);
};
} // namespace SOH
