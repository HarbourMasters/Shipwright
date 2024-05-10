#pragma once

#include <memory>
#include "Resource.h"
#include "ResourceFactory.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"
#include "public/bridge/consolevariablebridge.h"

namespace SOH {
class SceneCommandFactoryBinaryV0 {
    public:
        virtual std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) = 0;

    protected:
        void ReadCommandId(std::shared_ptr<ISceneCommand> command, std::shared_ptr<Ship::BinaryReader> reader);
};

class SceneCommandFactoryXMLV0 {
    public:
        virtual std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, tinyxml2::XMLElement* reader) = 0;

    protected:
        void ReadCommandId(std::shared_ptr<ISceneCommand> command, tinyxml2::XMLElement* reader);
};
} // namespace SOH
