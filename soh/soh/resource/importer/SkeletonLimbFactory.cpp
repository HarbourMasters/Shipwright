#include "soh/resource/importer/SkeletonLimbFactory.h"
#include "soh/resource/type/SkeletonLimb.h"
#include "spdlog/spdlog.h"
#include "libultraship/libultraship.h"

namespace LUS {
std::shared_ptr<IResource>
SkeletonLimbFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SkeletonLimb>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	    factory = std::make_shared<SkeletonLimbFactoryV0>();
	    break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Skeleton Limb with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

std::shared_ptr<IResource>
SkeletonLimbFactory::ReadResourceXML(std::shared_ptr<ResourceInitData> initData, tinyxml2::XMLElement *reader) {
    auto resource = std::make_shared<SkeletonLimb>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
        case 0:
            factory = std::make_shared<SkeletonLimbFactoryV0>();
            break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Skeleton Limb with version {}", resource->GetInitData()->ResourceVersion);
        return nullptr;
    }

    factory->ParseFileXML(reader, resource);

    return resource;
}

void LUS::SkeletonLimbFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                  std::shared_ptr<IResource> resource)
{
    std::shared_ptr<SkeletonLimb> skeletonLimb = std::static_pointer_cast<SkeletonLimb>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, skeletonLimb);

    skeletonLimb->limbType = (LimbType)reader->ReadInt8();
    skeletonLimb->skinSegmentType = (ZLimbSkinType)reader->ReadInt8();
    skeletonLimb->skinDList = reader->ReadString();

    skeletonLimb->skinVtxCnt = reader->ReadUInt16();
    
    skeletonLimb->skinLimbModifCount = reader->ReadUInt32();
    skeletonLimb->skinLimbModifArray.reserve(skeletonLimb->skinLimbModifCount);
    skeletonLimb->skinLimbModifVertexArrays.reserve(skeletonLimb->skinLimbModifCount);
    skeletonLimb->skinLimbModifTransformationArrays.reserve(skeletonLimb->skinLimbModifCount);
    for (size_t i = 0; i <  skeletonLimb->skinLimbModifCount; i++) {
        SkinLimbModif skinLimbModif;
        skinLimbModif.unk_4 = reader->ReadUInt16();
        skeletonLimb->skinLimbModifArray.push_back(skinLimbModif);

        std::vector<SkinVertex> skinVertexArray;
        int32_t skinVertexCount = reader->ReadInt32();
        skinVertexArray.reserve(skinVertexCount);
        for (int32_t k = 0; k < skinVertexCount; k++) {
            SkinVertex skinVertex;

            skinVertex.index = reader->ReadInt16();
            skinVertex.s = reader->ReadInt16();
            skinVertex.t = reader->ReadInt16();
            skinVertex.normX = reader->ReadInt8();
            skinVertex.normY = reader->ReadInt8();
            skinVertex.normZ = reader->ReadInt8();
            skinVertex.alpha = reader->ReadUByte();

            skinVertexArray.push_back(skinVertex);
        }
        skeletonLimb->skinLimbModifVertexArrays.push_back(skinVertexArray);

        std::vector<SkinTransformation> skinTransformationArray;
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

    if (skeletonLimb->limbType == LUS::LimbType::LOD) {
        skeletonLimb->limbData.lodLimb.jointPos.x = skeletonLimb->transX;
        skeletonLimb->limbData.lodLimb.jointPos.y = skeletonLimb->transY;
        skeletonLimb->limbData.lodLimb.jointPos.z = skeletonLimb->transZ;
        skeletonLimb->limbData.lodLimb.child = skeletonLimb->childIndex;
        skeletonLimb->limbData.lodLimb.sibling = skeletonLimb->siblingIndex;

        if (skeletonLimb->dListPtr != "") {
            auto dList = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(skeletonLimb->dListPtr.c_str());
            skeletonLimb->limbData.lodLimb.dLists[0] = (Gfx*)(dList ? dList->GetRawPointer() : nullptr);
        } else {
            skeletonLimb->limbData.lodLimb.dLists[0] = nullptr;
        }

        if (skeletonLimb->dList2Ptr != "") {
            auto dList = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(skeletonLimb->dList2Ptr.c_str());
            skeletonLimb->limbData.lodLimb.dLists[1] = (Gfx*)(dList ? dList->GetRawPointer() : nullptr);
        } else {
            skeletonLimb->limbData.lodLimb.dLists[1] = nullptr;
        }
    } else if (skeletonLimb->limbType == LUS::LimbType::Standard) {
        skeletonLimb->limbData.standardLimb.jointPos.x = skeletonLimb->transX;
        skeletonLimb->limbData.standardLimb.jointPos.y = skeletonLimb->transY;
        skeletonLimb->limbData.standardLimb.jointPos.z = skeletonLimb->transZ;
        skeletonLimb->limbData.standardLimb.child = skeletonLimb->childIndex;
        skeletonLimb->limbData.standardLimb.sibling = skeletonLimb->siblingIndex;
        skeletonLimb->limbData.standardLimb.dList = nullptr;

        if (!skeletonLimb->dListPtr.empty()) {
            const auto dList = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(skeletonLimb->dListPtr.c_str());
            skeletonLimb->limbData.standardLimb.dList = (Gfx*)(dList ? dList->GetRawPointer() : nullptr);
        }
    } else if (skeletonLimb->limbType == LUS::LimbType::Curve) {
        skeletonLimb->limbData.skelCurveLimb.firstChildIdx = skeletonLimb->childIndex;
        skeletonLimb->limbData.skelCurveLimb.nextLimbIdx = skeletonLimb->siblingIndex;
        skeletonLimb->limbData.skelCurveLimb.dList[0] = nullptr;
        skeletonLimb->limbData.skelCurveLimb.dList[1] = nullptr;

        if (!skeletonLimb->dListPtr.empty()) {
            const auto dList = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(skeletonLimb->dListPtr.c_str());
            skeletonLimb->limbData.skelCurveLimb.dList[0] = (Gfx*)(dList ? dList->GetRawPointer() : nullptr);
        }

        if (!skeletonLimb->dList2Ptr.empty()) {
            const auto dList = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(skeletonLimb->dList2Ptr.c_str());
            skeletonLimb->limbData.skelCurveLimb.dList[1] = (Gfx*)(dList ? dList->GetRawPointer() : nullptr);
        }
    } else if (skeletonLimb->limbType == LUS::LimbType::Skin) {
        skeletonLimb->limbData.skinLimb.jointPos.x = skeletonLimb->transX;
        skeletonLimb->limbData.skinLimb.jointPos.y = skeletonLimb->transY;
        skeletonLimb->limbData.skinLimb.jointPos.z = skeletonLimb->transZ;
        skeletonLimb->limbData.skinLimb.child = skeletonLimb->childIndex;
        skeletonLimb->limbData.skinLimb.sibling = skeletonLimb->siblingIndex;

        if (skeletonLimb->skinSegmentType == LUS::ZLimbSkinType::SkinType_DList) {
            skeletonLimb->limbData.skinLimb.segmentType = static_cast<int32_t>(skeletonLimb->skinSegmentType);
        } else if (skeletonLimb->skinSegmentType == LUS::ZLimbSkinType::SkinType_4) {
            skeletonLimb->limbData.skinLimb.segmentType = 4;
        } else if (skeletonLimb->skinSegmentType == LUS::ZLimbSkinType::SkinType_5) {
            skeletonLimb->limbData.skinLimb.segmentType = 5;
        } else {
            skeletonLimb->limbData.skinLimb.segmentType = 0;
        }

        if (skeletonLimb->skinSegmentType == LUS::ZLimbSkinType::SkinType_DList) {
            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(skeletonLimb->skinDList.c_str());
            skeletonLimb->limbData.skinLimb.segment = res ? res->GetRawPointer() : nullptr;
        } else if (skeletonLimb->skinSegmentType == LUS::ZLimbSkinType::SkinType_4) {
            skeletonLimb->skinAnimLimbData.totalVtxCount = skeletonLimb->skinVtxCnt;
            skeletonLimb->skinAnimLimbData.limbModifCount = skeletonLimb->skinLimbModifCount;
            skeletonLimb->skinAnimLimbData.limbModifications = skeletonLimb->skinLimbModifArray.data();
            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(skeletonLimb->skinDList2.c_str());
            skeletonLimb->skinAnimLimbData.dlist = (Gfx*)(res ? res->GetRawPointer() : nullptr);

            for (size_t i = 0; i < skeletonLimb->skinLimbModifArray.size(); i++) {
                skeletonLimb->skinAnimLimbData.limbModifications[i].vtxCount = skeletonLimb->skinLimbModifVertexArrays[i].size();
                skeletonLimb->skinAnimLimbData.limbModifications[i].skinVertices = skeletonLimb->skinLimbModifVertexArrays[i].data();
                
                skeletonLimb->skinAnimLimbData.limbModifications[i].transformCount = skeletonLimb->skinLimbModifTransformationArrays[i].size();
                skeletonLimb->skinAnimLimbData.limbModifications[i].limbTransformations = skeletonLimb->skinLimbModifTransformationArrays[i].data();

                skeletonLimb->skinAnimLimbData.limbModifications[i].unk_4 = skeletonLimb->skinLimbModifArray[i].unk_4;
            }

           skeletonLimb->limbData.skinLimb.segment = &skeletonLimb->skinAnimLimbData;
        }
    }
}
void SkeletonLimbFactoryV0::ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<IResource> resource)
{
    std::shared_ptr<SkeletonLimb> skelLimb = std::static_pointer_cast<SkeletonLimb>(resource);

    std::string limbType = reader->Attribute("Type");

    // OTRTODO
    skelLimb->limbType = LimbType::LOD;

    // skelLimb->legTransX = reader->FloatAttribute("LegTransX");
    // skelLimb->legTransY = reader->FloatAttribute("LegTransY");
    // skelLimb->legTransZ = reader->FloatAttribute("LegTransZ");
    skelLimb->rotX = reader->IntAttribute("RotX");
    skelLimb->rotY = reader->IntAttribute("RotY");
    skelLimb->rotZ = reader->IntAttribute("RotZ");

    // skelLimb->transX = reader->IntAttribute("TransX");
    // skelLimb->transY = reader->IntAttribute("TransY");
    // skelLimb->transZ = reader->IntAttribute("TransZ");

    skelLimb->transX = (int)reader->FloatAttribute("LegTransX");
    skelLimb->transY = (int)reader->FloatAttribute("LegTransY");
    skelLimb->transZ = (int)reader->FloatAttribute("LegTransZ");

    skelLimb->childIndex = reader->IntAttribute("ChildIndex");
    skelLimb->siblingIndex = reader->IntAttribute("SiblingIndex");

    // skelLimb->childPtr = reader->Attribute("ChildLimb");
    // skelLimb->siblingPtr = reader->Attribute("SiblingLimb");
    skelLimb->dListPtr = reader->Attribute("DisplayList1");

    if (std::string(reader->Attribute("DisplayList1")) == "gEmptyDL") {
        skelLimb->dListPtr = "";
    }

    auto& limbData = skelLimb->limbData;

    limbData.lodLimb.jointPos.x = skelLimb->transX;
    limbData.lodLimb.jointPos.y = skelLimb->transY;
    limbData.lodLimb.jointPos.z = skelLimb->transZ;

    if (skelLimb->dListPtr != "") {
        auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess((const char*)skelLimb->dListPtr.c_str());
        limbData.lodLimb.dLists[0] = (Gfx*)(res ? res->GetRawPointer() : nullptr);
    } else {
        limbData.lodLimb.dLists[0] = nullptr;
    }

    limbData.lodLimb.dLists[1] = nullptr;

    limbData.lodLimb.child = skelLimb->childIndex;
    limbData.lodLimb.sibling = skelLimb->siblingIndex;

    // skelLimb->dList2Ptr = reader->Attribute("DisplayList2");
}

} // namespace LUS
