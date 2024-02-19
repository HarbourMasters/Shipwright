#include "soh/resource/importer/scenecommand/SetCsCameraFactory.h"
#include "soh/resource/type/scenecommand/SetCsCamera.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetCsCameraFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setCsCamera = std::make_shared<SetCsCamera>(initData);

    ReadCommandId(setCsCamera, reader);
	
    reader->ReadInt8();  // camSize
    reader->ReadInt32(); // segOffset

    // OTRTODO: FINISH!

    return setCsCamera;
}
} // namespace SOH
