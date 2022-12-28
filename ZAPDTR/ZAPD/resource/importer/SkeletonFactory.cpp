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
        auto limb = std::static_pointer_cast<Ship::SkeletonLimb>(GetResourceDataByName(limbStr.c_str()));

        if (limb->limbType == Ship::LimbType::LOD) {
            LodLimb* limbC = (LodLimb*)malloc(sizeof(LodLimb));
            limbC->jointPos.x = limb->transX;
            limbC->jointPos.y = limb->transY;
            limbC->jointPos.z = limb->transZ;
            limbC->child = limb->childIndex;
            limbC->sibling = limb->siblingIndex;

            if (limb->dListPtr != "") {
                auto dList = ResourceMgr_LoadGfxByName(limb->dListPtr.c_str());
                limbC->dLists[0] = dList;
            } else {
                limbC->dLists[0] = nullptr;
            }

            if (limb->dList2Ptr != "") {
                auto dList = ResourceMgr_LoadGfxByName(limb->dList2Ptr.c_str());
                limbC->dLists[1] = dList;
            } else {
                limbC->dLists[1] = nullptr;
            }

            baseHeader->segment[i] = limbC;
        }
        else if (limb->limbType == Ship::LimbType::Standard)
        {
            const auto limbC = new StandardLimb;
            limbC->jointPos.x = limb->transX;
            limbC->jointPos.y = limb->transY;
            limbC->jointPos.z = limb->transZ;
            limbC->child = limb->childIndex;
            limbC->sibling = limb->siblingIndex;
            limbC->dList = nullptr;

            if (!limb->dListPtr.empty()) {
                const auto dList = ResourceMgr_LoadGfxByName(limb->dListPtr.c_str());
                limbC->dList = dList;
            }

            baseHeader->segment[i] = limbC;
        }
        else if (limb->limbType == Ship::LimbType::Curve)
        {
            const auto limbC = new SkelCurveLimb;

            limbC->firstChildIdx = limb->childIndex;
            limbC->nextLimbIdx = limb->siblingIndex;
            limbC->dList[0] = nullptr;
            limbC->dList[1] = nullptr;

            if (!limb->dListPtr.empty()) {
                const auto dList = ResourceMgr_LoadGfxByName(limb->dListPtr.c_str());
                limbC->dList[0] = dList;
            }

            if (!limb->dList2Ptr.empty()) {
                const auto dList = ResourceMgr_LoadGfxByName(limb->dList2Ptr.c_str());
                limbC->dList[1] = dList;
            }

            const auto curve = reinterpret_cast<SkelCurveLimbList*>(baseHeader);
            curve->limbs[i] = limbC;
        }
        else if (limb->limbType == Ship::LimbType::Skin)
        {
            const auto limbC = new SkinLimb;
            limbC->jointPos.x = limb->transX;
            limbC->jointPos.y = limb->transY;
            limbC->jointPos.z = limb->transZ;
            limbC->child = limb->childIndex;
            limbC->sibling = limb->siblingIndex;

            if (limb->skinSegmentType == Ship::ZLimbSkinType::SkinType_DList)
                limbC->segmentType = static_cast<int32_t>(limb->skinSegmentType);
            else if (limb->skinSegmentType == Ship::ZLimbSkinType::SkinType_4)
                limbC->segmentType = 4;
            else if (limb->skinSegmentType == Ship::ZLimbSkinType::SkinType_5)
                limbC->segmentType = 5;
            else
                limbC->segmentType = 0;

            if (limb->skinSegmentType == Ship::ZLimbSkinType::SkinType_DList)
                limbC->segment = ResourceMgr_LoadGfxByName(limb->skinDList.c_str());
            else if (limb->skinSegmentType == Ship::ZLimbSkinType::SkinType_4) {
                const auto animData = new SkinAnimatedLimbData;
                const int skinDataSize = limb->skinData.size();

                animData->totalVtxCount = limb->skinVtxCnt;
                animData->limbModifCount = skinDataSize;
                animData->limbModifications = new SkinLimbModif[animData->limbModifCount];
                animData->dlist = ResourceMgr_LoadGfxByName(limb->skinDList2.c_str());

                for (size_t i = 0; i < skinDataSize; i++)
                {
                    animData->limbModifications[i].vtxCount = limb->skinData[i].unk_8_arr.size();
                    animData->limbModifications[i].transformCount = limb->skinData[i].unk_C_arr.size();
                    animData->limbModifications[i].unk_4 = limb->skinData[i].unk_4;

                    animData->limbModifications[i].skinVertices = new SkinVertex[limb->skinData[i].unk_8_arr.size()];

                    for (int k = 0; k < limb->skinData[i].unk_8_arr.size(); k++)
                    {
                        animData->limbModifications[i].skinVertices[k].index = limb->skinData[i].unk_8_arr[k].unk_0;
                        animData->limbModifications[i].skinVertices[k].s = limb->skinData[i].unk_8_arr[k].unk_2;
                        animData->limbModifications[i].skinVertices[k].t = limb->skinData[i].unk_8_arr[k].unk_4;
                        animData->limbModifications[i].skinVertices[k].normX = limb->skinData[i].unk_8_arr[k].unk_6;
                        animData->limbModifications[i].skinVertices[k].normY = limb->skinData[i].unk_8_arr[k].unk_7;
                        animData->limbModifications[i].skinVertices[k].normZ = limb->skinData[i].unk_8_arr[k].unk_8;
                        animData->limbModifications[i].skinVertices[k].alpha = limb->skinData[i].unk_8_arr[k].unk_9;
                    }

                    animData->limbModifications[i].limbTransformations =
                        new SkinTransformation[limb->skinData[i].unk_C_arr.size()];

                    for (int k = 0; k < limb->skinData[i].unk_C_arr.size(); k++)
                    {
                        animData->limbModifications[i].limbTransformations[k].limbIndex = limb->skinData[i].unk_C_arr[k].unk_0;
                        animData->limbModifications[i].limbTransformations[k].x = limb->skinData[i].unk_C_arr[k].x;
                        animData->limbModifications[i].limbTransformations[k].y = limb->skinData[i].unk_C_arr[k].y;
                        animData->limbModifications[i].limbTransformations[k].z = limb->skinData[i].unk_C_arr[k].z;
                        animData->limbModifications[i].limbTransformations[k].scale = limb->skinData[i].unk_C_arr[k].unk_8;
                    }
                }

                limbC->segment = animData;

                //limbC->segment = nullptr;
            }

            baseHeader->segment[i] = limbC;
        }
        else
        {
            // OTRTODO: Print error here...
        }
    }

    return baseHeader;
}

*/


}








} // namespace Ship