#include "resource/importer/SkeletonLimbFactory.h"
#include "resource/type/SkeletonLimb.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SkeletonLimbFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<Text>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SkeletonLimbFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load Skeleton Limb with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SkeletonLimbFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                  std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SkeletonLimb> skeletonLimb = std::static_pointer_cast<SkeletonLimb>(resource);
	ResourceFile::ParseFileBinary(reader, skeletonLimb);

    skeletonLimb->limbType = (LimbType)reader->ReadInt8();
    skeletonLimb->skinSegmentType = (ZLimbSkinType)reader->ReadInt8();
    skeletonLimb->skinDList = reader->ReadString();

    skeletonLimb->skinVtxCnt = reader->ReadUInt16();
    
    skeletonLimb->skinLimbModifCount = reader->ReadUInt32();
    skeletonLimb->skinLimbModifArray.reserve(skeletonLimb->skinLimbModifCount);
    skeletonLimb->skinLimbModifVertexArrays.reserve(skeletonLimb->skinLimbModifCount);
    skeletonLimb->skinLimbModifTransformationArrays.reserve(skeletonLimb->skinLimbModifCount);
    for (size_t i = 0; i < skinLimbModifCount; i++) {
        SkinLimbModif skinLimbModif;
        skinLimbModif.unk_4 = reader->ReadUInt16();
        skeletonLimb->skinLimbModifArray.push_back(skinLimbModif);

        std::vector<SkinVertex> skinVertexArray;
        int32_t skinVertexCount = reader->ReadInt32();
        skinVertexArray.reserve(skinVertexCount);
        for (int32_t k = 0; k < skinVertexCount; k++) {
            SkinVertex skinVertex;

            skinVertex.index = reader->ReadUInt16();
            skinVertex.s = reader->ReadInt16();
            skinVertex.t = reader->ReadInt16();
            skinVertex.normX = reader->ReadInt8();
            skinVertex.normY = reader->ReadInt8();
            skinVertex.normZ = reader->ReadInt8();
            skinVertex.alpha = reader->ReadUByte();

            skinVertexArray.push_back(skinVertex);
        }
        skeletonLimb->skinLimbModifVertexArrays.push_back(skinVertexArray);

        std::vector<SkinVertex> skinTransformationArray;
        int32_t skinTransformationCount = reader->ReadInt32();
        skinTransformationArray.reserve(skinTransformationCount);
        for (int32_t k = 0; k < skinTransformationCount; k++) {
            SkinTransformation skinTransformation;

            skinTransformation.limbIndex = reader->ReadUByte();
            skinTransformation.x = reader->ReadInt16();
            skinTransformation.y = reader->ReadInt16();
            skinTransformation.z = reader->ReadInt16();
            skinTransformation.scale = reader->ReadUByte();

            skinTransformationArray.push_back(skinTransformation);
        }
        skeletonLimb->skinLimbModifTransformationArrays.push_back(skinTransformationArray);
    }

    skeletonLimb->skinDList2 = reader->ReadString();

    skeletonLimb->legTransX = reader->ReadFloat();
    skeletonLimb->legTransY = reader->ReadFloat();
    skeletonLimb->legTransZ = reader->ReadFloat();

    skeletonLimb->rotX = reader->ReadUInt16();
    skeletonLimb->rotY = reader->ReadUInt16();
    skeletonLimb->rotZ = reader->ReadUInt16();

    skeletonLimb->childPtr = reader->ReadString();
    skeletonLimb->siblingPtr = reader->ReadString();
    skeletonLimb->dListPtr = reader->ReadString();
    skeletonLimb->dList2Ptr = reader->ReadString();

    skeletonLimb->transX = reader->ReadInt16();
    skeletonLimb->transY = reader->ReadInt16();
    skeletonLimb->transZ = reader->ReadInt16();

    skeletonLimb->childIndex = reader->ReadUByte();
    skeletonLimb->siblingIndex = reader->ReadUByte();

}
} // namespace Ship
