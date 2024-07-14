#include "soh/resource/importer/SkeletonFactory.h"
#include "soh/resource/type/Skeleton.h"
#include <spdlog/spdlog.h>
#include <libultraship/libultraship.h>

namespace SOH {
std::shared_ptr<Ship::IResource> ResourceFactoryBinarySkeletonV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto skeleton = std::make_shared<Skeleton>(file->InitData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

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

    if (skeleton->type == SkeletonType::Curve) {
	skeleton->skeletonData.skelCurveLimbList.limbCount = skeleton->limbCount;
	skeleton->curveLimbArray.reserve(skeleton->skeletonData.skelCurveLimbList.limbCount);
    } else if (skeleton->type == SkeletonType::Flex) {
	skeleton->skeletonData.flexSkeletonHeader.dListCount = skeleton->dListCount;
    }

    if (skeleton->type == SkeletonType::Normal) {
        skeleton->skeletonData.skeletonHeader.limbCount = skeleton->limbCount;
	skeleton->standardLimbArray.reserve(skeleton->skeletonData.skeletonHeader.limbCount);
    } else if (skeleton->type == SkeletonType::Flex) {
        skeleton->skeletonData.flexSkeletonHeader.sh.limbCount = skeleton->limbCount;
	skeleton->standardLimbArray.reserve(skeleton->skeletonData.flexSkeletonHeader.sh.limbCount);
    }

    for (size_t i = 0; i < skeleton->limbTable.size(); i++) {
        std::string limbStr = skeleton->limbTable[i];
        auto limb = Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(limbStr.c_str());
        skeleton->skeletonHeaderSegments.push_back(limb ? limb->GetRawPointer() : nullptr);
    }

    if (skeleton->type == SkeletonType::Normal) {
        skeleton->skeletonData.skeletonHeader.segment = (void**)skeleton->skeletonHeaderSegments.data();
    } else if (skeleton->type == SkeletonType::Flex) {
        skeleton->skeletonData.flexSkeletonHeader.sh.segment = (void**)skeleton->skeletonHeaderSegments.data();
    } else if (skeleton->type == SkeletonType::Curve) {
        skeleton->skeletonData.skelCurveLimbList.limbs = (SkelCurveLimb**)skeleton->skeletonHeaderSegments.data();
    } else {
        SPDLOG_ERROR("unknown skeleton type {}", (uint32_t)skeleton->type);
    }

    skeleton->skeletonData.skeletonHeader.skeletonType = (uint8_t)skeleton->type;

    return skeleton;
}

std::shared_ptr<Ship::IResource> ResourceFactoryXMLSkeletonV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto skel = std::make_shared<Skeleton>(file->InitData);
    auto reader = std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement();
    auto child = reader->FirstChildElement();

    skel->type = SkeletonType::Flex; // Default to Flex for legacy reasons
    if (reader->FindAttribute("Type")) {
        std::string skeletonType = reader->Attribute("Type");

        if (skeletonType == "Flex") {
            skel->type = SkeletonType::Flex;
        } else if (skeletonType == "Curve") {
            skel->type = SkeletonType::Curve;
        } else if (skeletonType == "Normal") {
            skel->type = SkeletonType::Normal;
        }
    }

    skel->limbType = LimbType::LOD; // Default to LOD for legacy reasons
    if (reader->FindAttribute("LimbType")) {
        std::string skeletonLimbType = reader->Attribute("LimbType");

        if (skeletonLimbType == "Standard") {
            skel->limbType = LimbType::Standard;
        } else if (skeletonLimbType == "LOD") {
            skel->limbType = LimbType::LOD;
        } else if (skeletonLimbType == "Curve") {
            skel->limbType = LimbType::Curve;
        } else if (skeletonLimbType == "Skin") {
            skel->limbType = LimbType::Skin;
        } else if (skeletonLimbType == "Legacy") {
            skel->limbType = LimbType::Legacy;
        }
    }


    skel->limbCount = reader->IntAttribute("LimbCount");
    skel->dListCount = reader->IntAttribute("DisplayListCount");

    while (child != nullptr) {
        std::string childName = child->Name();

        if (childName == "SkeletonLimb") {
            std::string limbName = child->Attribute("Path");
            skel->limbTable.push_back(limbName);

            auto limb = Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(limbName.c_str());
            skel->skeletonHeaderSegments.push_back(limb ? limb->GetRawPointer() : nullptr);
        }

        child = child->NextSiblingElement();
    }
    
    skel->skeletonData.flexSkeletonHeader.sh.limbCount = skel->limbCount;
    skel->skeletonData.flexSkeletonHeader.sh.segment = (void**)skel->skeletonHeaderSegments.data();
    skel->skeletonData.flexSkeletonHeader.dListCount = skel->dListCount;
    skel->skeletonData.skeletonHeader.skeletonType = (uint8_t)skel->type;

    return skel;
}
} // namespace SOH
