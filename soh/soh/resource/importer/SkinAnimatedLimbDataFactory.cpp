#include "soh/resource/importer/SkinAnimatedLimbDataFactory.h"
#include "soh/resource/type/SkinAnimatedLimbData.h"
#include "spdlog/spdlog.h"
#include <tinyxml2.h>
#include "libultraship/libultraship.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
ResourceFactoryXMLSkinVertV0::ReadResource(std::shared_ptr<Ship::File> file,
                                           std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto skinVert = std::make_shared<SkinVert>(initData);

    auto child =
        std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement()->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "SkinVertex") {
            SkinVertex data;
            data.index = child->IntAttribute("Index");
            data.s = child->IntAttribute("S");
            data.t = child->IntAttribute("T");
            data.normX = child->IntAttribute("NormX");
            data.normY = child->IntAttribute("NormY");
            data.normZ = child->IntAttribute("NormZ");
            data.alpha = child->IntAttribute("Alpha");

            skinVert->skinVertexList.push_back(data);
        }

        child = child->NextSiblingElement();
    }

    return skinVert;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryXMLSkinTransformV0::ReadResource(std::shared_ptr<Ship::File> file,
                                                std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto skinTransform = std::make_shared<SkinTransform>(initData);

    auto child =
        std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement()->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "SkinTransformation") {
            SkinTransformation data;
            data.limbIndex = child->IntAttribute("LimbIndex");
            data.x = child->IntAttribute("X");
            data.y = child->IntAttribute("Y");
            data.z = child->IntAttribute("Z");
            data.scale = child->IntAttribute("Scale");

            skinTransform->skinTransformationList.push_back(data);
        }

        child = child->NextSiblingElement();
    }

    return skinTransform;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryXMLSkinModifV0::ReadResource(std::shared_ptr<Ship::File> file,
                                            std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto skinModif = std::make_shared<SkinModif>(initData);

    auto child =
        std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement()->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "SkinLimbModif") {
            SkinLimbModif data;
            data.vtxCount = child->IntAttribute("VtxCount");
            data.transformCount = child->IntAttribute("TransformCount");
            data.unk_4 = child->IntAttribute("Unk_4");

            std::string skinVerticesPath = child->Attribute("SkinVertices");
            auto skinVerts = std::static_pointer_cast<SkinVert>(
                Ship::Context::GetRawInstance()->GetResourceManager()->LoadResourceProcess(skinVerticesPath.c_str()));
            skinModif->skinVerticesArrays.push_back(std::move(skinVerts->skinVertexList));
            data.skinVertices = skinModif->skinVerticesArrays.back().data();

            std::string transformsPath = child->Attribute("LimbTransformations");
            auto skinTransforms = std::static_pointer_cast<SkinTransform>(
                Ship::Context::GetRawInstance()->GetResourceManager()->LoadResourceProcess(transformsPath.c_str()));
            skinModif->skinTransformationsArrays.push_back(std::move(skinTransforms->skinTransformationList));
            data.limbTransformations = skinModif->skinTransformationsArrays.back().data();

            skinModif->skinLimbModifArray.push_back(data);
        }

        child = child->NextSiblingElement();
    }
    return skinModif;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryXMLSkinAnimDataV0::ReadResource(std::shared_ptr<Ship::File> file,
                                               std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto skinAnimatedData = std::make_shared<SkinAnimData>(initData);
    auto reader = std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement();

    skinAnimatedData->data.totalVtxCount = reader->IntAttribute("TotalVtxCount");
    skinAnimatedData->data.limbModifCount = reader->IntAttribute("LimbModifCount");
    skinAnimatedData->skinLimbModifArray.reserve(skinAnimatedData->data.limbModifCount);
    // skinAnimatedData->skinLimbModifVertexArrays.reserve(skinAnimatedData->data.limbModifCount);
    // skinAnimatedData->skinLimbModifTransformationArrays.reserve(skinAnimatedData->data.limbModifCount);

    std::string modifsPath = reader->Attribute("LimbModifications");

    auto skinModifs = std::static_pointer_cast<SkinModif>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResourceProcess(modifsPath.c_str()));
    skinAnimatedData->skinLimbModifArray = std::move(skinModifs->skinLimbModifArray);
    skinAnimatedData->data.limbModifications = skinAnimatedData->skinLimbModifArray.data();

    skinAnimatedData->dListPtr = reader->Attribute("DList");

    if (std::string(reader->Attribute("DList")) == "gEmptyDL") {
        skinAnimatedData->dListPtr = "";
    }

    if (skinAnimatedData->dListPtr != "") {
        skinAnimatedData->dListPtr = "__OTR__" + skinAnimatedData->dListPtr;
        skinAnimatedData->data.dlist = (Gfx*)skinAnimatedData->dListPtr.c_str();
    } else {
        skinAnimatedData->data.dlist = nullptr;
    }

    return skinAnimatedData;
}
} // namespace SOH
