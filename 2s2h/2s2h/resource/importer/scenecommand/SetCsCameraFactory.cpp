#include "2s2h/resource/importer/scenecommand/SetCsCameraFactory.h"
#include "2s2h/resource/type/scenecommand/SetCsCamera.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetCsCameraFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                  std::shared_ptr<Ship::BinaryReader> reader) {
    auto setCsCamera = std::make_shared<SetCsCamera>(initData);

    ReadCommandId(setCsCamera, reader);

    size_t camSize = reader->ReadUInt32();

    setCsCamera->csCamera.reserve(camSize);

    for (size_t i = 0; i < camSize; i++) {
        ActorCsCamInfoData data;
        data.setting = reader->ReadUInt16();
        data.count = reader->ReadUInt16();
        if (data.count == 0) {
            data.actorCsCamFuncData = nullptr;
            continue;
        }
        data.actorCsCamFuncData = new z64Vec3s[data.count];
        for (size_t j = 0; j < data.count; j++) {
            data.actorCsCamFuncData[j].x = reader->ReadInt16();
            data.actorCsCamFuncData[j].y = reader->ReadInt16();
            data.actorCsCamFuncData[j].z = reader->ReadInt16();
        }
        setCsCamera->csCamera.emplace_back(data);
    }

    return setCsCamera;
}
} // namespace SOH
