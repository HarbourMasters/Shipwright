#include "resource/importer/SkeletonFactory.h"
#include "resource/type/Skeleton.h"
#include "spdlog/spdlog.h"
#include "resourcebridge.h"

namespace Ship {
std::shared_ptr<Resource> SkeletonFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<Skeleton>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SkeletonFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load Skeleton with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void SkeletonFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
    std::shared_ptr<Skeleton> skeleton = std::static_pointer_cast<Skeleton>(resource);
    ResourceFile::ParseFileBinary(reader, skeleton);

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

    if (skeleton->type == Ship::SkeletonType::Curve) {
		skeleton->skeletonData.skelCurveLimbList.limbCount = skeleton->limbCount;
		skeleton->curveLimbArray.reserve(skeleton->skeletonData.skelCurveLimbList.limbCount);
    } else if (skeleton->type == Ship::SkeletonType::Flex) {
		skeleton->skeletonData.flexSkeletonHeader.dListCount = skeleton->dListCount;
    }

    if (skeleton->type == Ship::SkeletonType::Normal) {
        skeleton->skeletonData.skeletonHeader.limbCount = skeleton->limbCount;
		skeleton->standardLimbArray.reserve(skeleton->skeletonData.skeletonHeader.limbCount);
    } else if (skeleton->type == Ship::SkeletonType::Flex) {
        skeleton->skeletonData.flexSkeletonHeader.sh.limbCount = skeleton->limbCount;
		skeleton->standardLimbArray.reserve(skeleton->skeletonData.flexSkeletonHeader.sh.limbCount);
    }

    for (size_t i = 0; i < skeleton->limbTable.size(); i++) {
        std::string limbStr = skeleton->limbTable[i];
        auto limb = GetResourceDataByName(limbStr.c_str());
        if (skeleton->type == Ship::SkeletonType::Normal) {
            skeleton->skeletonData.skeletonHeader.segment[i] = limb;
        } else if (skeleton->type == Ship::SkeletonType::Flex) {
            skeleton->skeletonData.flexSkeletonHeader.sh.segment[i] = limb;
        } else if (skeleton->type == Ship::SkeletonType::Curve) {
            skeleton->skeletonData.skelCurveLimbList.limbs[i] = (SkelCurveLimb*)limb;
        } else {
            SPDLOG_ERROR("unknown skeleton type {}", skeleton->type);
        }
    }
}
} // namespace Ship
