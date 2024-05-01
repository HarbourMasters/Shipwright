#include "soh/resource/importer/SkeletonLimbFactory.h"
#include "soh/resource/type/SkeletonLimb.h"
#include "spdlog/spdlog.h"
#include "libultraship/libultraship.h"

namespace SOH {
std::shared_ptr<Ship::IResource> ResourceFactoryBinarySkeletonLimbV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto skeletonLimb = std::make_shared<SkeletonLimb>(file->InitData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

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

    if (skeletonLimb->limbType == LimbType::LOD) {
        skeletonLimb->limbData.lodLimb.jointPos.x = skeletonLimb->transX;
        skeletonLimb->limbData.lodLimb.jointPos.y = skeletonLimb->transY;
        skeletonLimb->limbData.lodLimb.jointPos.z = skeletonLimb->transZ;
        skeletonLimb->limbData.lodLimb.child = skeletonLimb->childIndex;
        skeletonLimb->limbData.lodLimb.sibling = skeletonLimb->siblingIndex;

        if (skeletonLimb->dListPtr != "") {
            skeletonLimb->dListPtr = "__OTR__" + skeletonLimb->dListPtr;
            skeletonLimb->limbData.lodLimb.dLists[0] = (Gfx*)skeletonLimb->dListPtr.c_str();
        } else {
            skeletonLimb->limbData.lodLimb.dLists[0] = nullptr;
        }

        if (skeletonLimb->dList2Ptr != "") {
            skeletonLimb->dList2Ptr = "__OTR__" + skeletonLimb->dList2Ptr;
            skeletonLimb->limbData.lodLimb.dLists[1] = (Gfx*)skeletonLimb->dList2Ptr.c_str();
        } else {
            skeletonLimb->limbData.lodLimb.dLists[1] = nullptr;
        }
    } else if (skeletonLimb->limbType == LimbType::Standard) {
        skeletonLimb->limbData.standardLimb.jointPos.x = skeletonLimb->transX;
        skeletonLimb->limbData.standardLimb.jointPos.y = skeletonLimb->transY;
        skeletonLimb->limbData.standardLimb.jointPos.z = skeletonLimb->transZ;
        skeletonLimb->limbData.standardLimb.child = skeletonLimb->childIndex;
        skeletonLimb->limbData.standardLimb.sibling = skeletonLimb->siblingIndex;
        skeletonLimb->limbData.standardLimb.dList = nullptr;

        if (!skeletonLimb->dListPtr.empty()) {
            skeletonLimb->dListPtr = "__OTR__" + skeletonLimb->dListPtr;
            skeletonLimb->limbData.standardLimb.dList = (Gfx*)skeletonLimb->dListPtr.c_str();
        }
    } else if (skeletonLimb->limbType == LimbType::Curve) {
        skeletonLimb->limbData.skelCurveLimb.firstChildIdx = skeletonLimb->childIndex;
        skeletonLimb->limbData.skelCurveLimb.nextLimbIdx = skeletonLimb->siblingIndex;
        skeletonLimb->limbData.skelCurveLimb.dList[0] = nullptr;
        skeletonLimb->limbData.skelCurveLimb.dList[1] = nullptr;

        if (!skeletonLimb->dListPtr.empty()) {
            skeletonLimb->dListPtr = "__OTR__" + skeletonLimb->dListPtr;
            skeletonLimb->limbData.skelCurveLimb.dList[0] = (Gfx*)skeletonLimb->dListPtr.c_str();
        }

        if (!skeletonLimb->dList2Ptr.empty()) {
            skeletonLimb->dList2Ptr = "__OTR__" + skeletonLimb->dList2Ptr;
            skeletonLimb->limbData.skelCurveLimb.dList[1] = (Gfx*)skeletonLimb->dList2Ptr.c_str();
        }
    } else if (skeletonLimb->limbType == LimbType::Skin) {
        skeletonLimb->limbData.skinLimb.jointPos.x = skeletonLimb->transX;
        skeletonLimb->limbData.skinLimb.jointPos.y = skeletonLimb->transY;
        skeletonLimb->limbData.skinLimb.jointPos.z = skeletonLimb->transZ;
        skeletonLimb->limbData.skinLimb.child = skeletonLimb->childIndex;
        skeletonLimb->limbData.skinLimb.sibling = skeletonLimb->siblingIndex;

        if (skeletonLimb->skinSegmentType == ZLimbSkinType::SkinType_DList) {
            skeletonLimb->limbData.skinLimb.segmentType = static_cast<int32_t>(skeletonLimb->skinSegmentType);
        } else if (skeletonLimb->skinSegmentType == ZLimbSkinType::SkinType_4) {
            skeletonLimb->limbData.skinLimb.segmentType = 4;
        } else if (skeletonLimb->skinSegmentType == ZLimbSkinType::SkinType_5) {
            skeletonLimb->limbData.skinLimb.segmentType = 5;
        } else {
            skeletonLimb->limbData.skinLimb.segmentType = 0;
        }

        if (skeletonLimb->skinSegmentType == ZLimbSkinType::SkinType_DList) {
            if (skeletonLimb->skinDList != "") {
                skeletonLimb->skinDList = "__OTR__" + skeletonLimb->skinDList;
                skeletonLimb->limbData.skinLimb.segment = (Gfx*)skeletonLimb->skinDList.c_str();
            } else {
                skeletonLimb->limbData.skinLimb.segment = nullptr;
            }
        } else if (skeletonLimb->skinSegmentType == ZLimbSkinType::SkinType_4) {
            skeletonLimb->skinAnimLimbData.totalVtxCount = skeletonLimb->skinVtxCnt;
            skeletonLimb->skinAnimLimbData.limbModifCount = skeletonLimb->skinLimbModifCount;
            skeletonLimb->skinAnimLimbData.limbModifications = skeletonLimb->skinLimbModifArray.data();

            if (skeletonLimb->skinDList2 != "") {
                skeletonLimb->skinDList2 = "__OTR__" + skeletonLimb->skinDList2;
                skeletonLimb->skinAnimLimbData.dlist = (Gfx*)skeletonLimb->skinDList2.c_str();
            } else {
                skeletonLimb->skinAnimLimbData.dlist = nullptr;
            }

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

    return skeletonLimb;
}

std::shared_ptr<Ship::IResource> ResourceFactoryXMLSkeletonLimbV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto skelLimb = std::make_shared<SkeletonLimb>(file->InitData);
    auto reader = std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement();

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
        skelLimb->dListPtr = "__OTR__" + skelLimb->dListPtr;
        limbData.lodLimb.dLists[0] = (Gfx*)skelLimb->dListPtr.c_str();
    } else {
        limbData.lodLimb.dLists[0] = nullptr;
    }

    limbData.lodLimb.dLists[1] = nullptr;

    limbData.lodLimb.child = skelLimb->childIndex;
    limbData.lodLimb.sibling = skelLimb->siblingIndex;

    // skelLimb->dList2Ptr = reader->Attribute("DisplayList2");

    return skelLimb;
}
} // namespace SOH
