#include "2s2h/resource/importer/scenecommand/SetAnimatedMaterialListFactory.h"
#include "2s2h/resource/type/scenecommand/SetAnimatedMaterialList.h"
#include "2s2h/resource/type/TextureAnimation.h"
#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>

namespace SOH {

std::shared_ptr<Ship::IResource>
SetAnimatedMaterialListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                             std::shared_ptr<Ship::BinaryReader> reader) {
    auto setAnimatedMat = std::make_shared<SetAnimatedMaterialList>(initData);

    ReadCommandId(setAnimatedMat, reader);

    std::string str = reader->ReadString();
    const auto data = std::static_pointer_cast<TextureAnimation>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResourceProcess(str.c_str()));

    AnimatedMaterial* res = data->GetPointer();
    setAnimatedMat->mat = res;

    return setAnimatedMat;
}
} // namespace SOH
