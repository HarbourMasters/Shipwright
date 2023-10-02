#include "soh/resource/importer/SkeletonFactory.h"
#include "soh/resource/type/Skeleton.h"
#include <spdlog/spdlog.h>
#include <libultraship/libultraship.h>

namespace LUS {
std::shared_ptr<IResource>
SkeletonFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<Skeleton>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	factory = std::make_shared<SkeletonFactoryV0>();
	break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Skeleton with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

std::shared_ptr<IResource>
SkeletonFactory::ReadResourceXML(std::shared_ptr<ResourceInitData> initData, tinyxml2::XMLElement *reader) {
    auto resource = std::make_shared<Skeleton>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
        case 0:
            factory = std::make_shared<SkeletonFactoryV0>();
            break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Skeleton with version {}", resource->GetInitData()->ResourceVersion);
        return nullptr;
    }

    factory->ParseFileXML(reader, resource);

    return resource;
}

void SkeletonFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<IResource> resource)
{
    std::shared_ptr<Skeleton> skeleton = std::static_pointer_cast<Skeleton>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, skeleton);

    skeleton->type = (SkeletonType)reader->ReadInt8();
    skeleton->limbType = (LimbType)reader->ReadInt8();
    skeleton->limbCount = reader->ReadUInt32();
    skeleton->dListCount = reader->ReadUInt32();
    skeleton->limbTableType = (LimbType)reader->ReadInt8();
    skeleton->limbTableCount = reader->ReadUInt32();

    skeleton->limbTable.reserve(skeleton->limbTableCount);
    for (uint32_t i = 0; i < skeleton->limbTableCount; i++) {
        std::string limbPath = reader->ReadString();

        skeleton->limbTable.push_back(limbPath);
    }

    if (skeleton->type == LUS::SkeletonType::Curve) {
	skeleton->skeletonData.skelCurveLimbList.limbCount = skeleton->limbCount;
	skeleton->curveLimbArray.reserve(skeleton->skeletonData.skelCurveLimbList.limbCount);
    } else if (skeleton->type == LUS::SkeletonType::Flex) {
	skeleton->skeletonData.flexSkeletonHeader.dListCount = skeleton->dListCount;
    }

    if (skeleton->type == LUS::SkeletonType::Normal) {
        skeleton->skeletonData.skeletonHeader.limbCount = skeleton->limbCount;
	skeleton->standardLimbArray.reserve(skeleton->skeletonData.skeletonHeader.limbCount);
    } else if (skeleton->type == LUS::SkeletonType::Flex) {
        skeleton->skeletonData.flexSkeletonHeader.sh.limbCount = skeleton->limbCount;
	skeleton->standardLimbArray.reserve(skeleton->skeletonData.flexSkeletonHeader.sh.limbCount);
    }

    for (size_t i = 0; i < skeleton->limbTable.size(); i++) {
        std::string limbStr = skeleton->limbTable[i];
        auto limb = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(limbStr.c_str());
        skeleton->skeletonHeaderSegments.push_back(limb ? limb->GetRawPointer() : nullptr);
    }

    if (skeleton->type == LUS::SkeletonType::Normal) {
        skeleton->skeletonData.skeletonHeader.segment = (void**)skeleton->skeletonHeaderSegments.data();
    } else if (skeleton->type == LUS::SkeletonType::Flex) {
        skeleton->skeletonData.flexSkeletonHeader.sh.segment = (void**)skeleton->skeletonHeaderSegments.data();
    } else if (skeleton->type == LUS::SkeletonType::Curve) {
        skeleton->skeletonData.skelCurveLimbList.limbs = (SkelCurveLimb**)skeleton->skeletonHeaderSegments.data();
    } else {
        SPDLOG_ERROR("unknown skeleton type {}", (uint32_t)skeleton->type);
    }

    skeleton->skeletonData.skeletonHeader.skeletonType = (uint8_t)skeleton->type;
}
void SkeletonFactoryV0::ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<IResource> resource)
{
    std::shared_ptr<Skeleton> skel = std::static_pointer_cast<Skeleton>(resource);

    std::string skeletonType = reader->Attribute("Type");
    // std::string skeletonLimbType = reader->Attribute("LimbType");
    int numLimbs = reader->IntAttribute("LimbCount");
    int numDLs = reader->IntAttribute("DisplayListCount");

    if (skeletonType == "Flex") {
        skel->type = SkeletonType::Flex;
    } else if (skeletonType == "Curve") {
        skel->type = SkeletonType::Curve;
    } else if (skeletonType == "Normal") {
        skel->type = SkeletonType::Normal;
    }

    skel->type = SkeletonType::Flex;
    skel->limbType = LimbType::LOD;

    // if (skeletonLimbType == "Standard")
    // skel->limbType = LimbType::Standard;
    // else if (skeletonLimbType == "LOD")
    // skel->limbType = LimbType::LOD;
    // else if (skeletonLimbType == "Curve")
    // skel->limbType = LimbType::Curve;
    // else if (skeletonLimbType == "Skin")
    // skel->limbType = LimbType::Skin;
    // else if (skeletonLimbType == "Legacy")
    // Sskel->limbType = LimbType::Legacy;

    auto child = reader->FirstChildElement();

    skel->limbCount = numLimbs;
    skel->dListCount = numDLs;

    while (child != nullptr) {
        std::string childName = child->Name();

        if (childName == "SkeletonLimb") {
            std::string limbName = child->Attribute("Path");
            skel->limbTable.push_back(limbName);

            auto limb = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(limbName.c_str());
            skel->skeletonHeaderSegments.push_back(limb ? limb->GetRawPointer() : nullptr);
        }

        child = child->NextSiblingElement();
    }
    
    skel->skeletonData.flexSkeletonHeader.sh.limbCount = skel->limbCount;
    skel->skeletonData.flexSkeletonHeader.sh.segment = (void**)skel->skeletonHeaderSegments.data();
    skel->skeletonData.flexSkeletonHeader.dListCount = skel->dListCount;
    skel->skeletonData.skeletonHeader.skeletonType = (uint8_t)skel->type;
}

} // namespace LUS
