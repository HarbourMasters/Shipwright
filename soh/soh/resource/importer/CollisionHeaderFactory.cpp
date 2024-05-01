#include "soh/resource/importer/CollisionHeaderFactory.h"
#include "soh/resource/type/CollisionHeader.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource> ResourceFactoryBinaryCollisionHeaderV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto collisionHeader = std::make_shared<CollisionHeader>(file->InitData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    collisionHeader->collisionHeaderData.minBounds.x = reader->ReadInt16();
    collisionHeader->collisionHeaderData.minBounds.y = reader->ReadInt16();
    collisionHeader->collisionHeaderData.minBounds.z = reader->ReadInt16();

    collisionHeader->collisionHeaderData.maxBounds.x = reader->ReadInt16();
    collisionHeader->collisionHeaderData.maxBounds.y = reader->ReadInt16();
    collisionHeader->collisionHeaderData.maxBounds.z = reader->ReadInt16();

    collisionHeader->collisionHeaderData.numVertices = reader->ReadInt32();
    collisionHeader->vertices.reserve(collisionHeader->collisionHeaderData.numVertices);
    for (int32_t i = 0; i < collisionHeader->collisionHeaderData.numVertices; i++) {
        Vec3s vtx;
        vtx.x = reader->ReadInt16();
        vtx.y = reader->ReadInt16();
        vtx.z = reader->ReadInt16();
        collisionHeader->vertices.push_back(vtx);
    }
    collisionHeader->collisionHeaderData.vtxList = collisionHeader->vertices.data();

    collisionHeader->collisionHeaderData.numPolygons = reader->ReadUInt32();
    collisionHeader->polygons.reserve(collisionHeader->collisionHeaderData.numPolygons);
    for (uint32_t i = 0; i < collisionHeader->collisionHeaderData.numPolygons; i++) {
        CollisionPoly polygon;

        polygon.type = reader->ReadUInt16();
        
        polygon.flags_vIA = reader->ReadUInt16();
        polygon.flags_vIB = reader->ReadUInt16();
        polygon.vIC = reader->ReadUInt16();

        polygon.normal.x = reader->ReadUInt16();
        polygon.normal.y = reader->ReadUInt16();
        polygon.normal.z = reader->ReadUInt16();

        polygon.dist = reader->ReadUInt16();

        collisionHeader->polygons.push_back(polygon);
        
    }
    collisionHeader->collisionHeaderData.polyList = collisionHeader->polygons.data();

    collisionHeader->surfaceTypesCount = reader->ReadUInt32();
    collisionHeader->surfaceTypes.reserve(collisionHeader->surfaceTypesCount);
    for (uint32_t i = 0; i < collisionHeader->surfaceTypesCount; i++) {
        SurfaceType surfaceType;

        surfaceType.data[1] = reader->ReadUInt32();
        surfaceType.data[0] = reader->ReadUInt32();

        collisionHeader->surfaceTypes.push_back(surfaceType);
    }
    collisionHeader->collisionHeaderData.surfaceTypeList = collisionHeader->surfaceTypes.data();

    collisionHeader->camDataCount = reader->ReadUInt32();
    collisionHeader->camData.reserve(collisionHeader->camDataCount);
    collisionHeader->camPosDataIndices.reserve(collisionHeader->camDataCount);
    for (uint32_t i = 0; i < collisionHeader->camDataCount; i++) {
        CamData camDataEntry;
        camDataEntry.cameraSType = reader->ReadUInt16();
        camDataEntry.numCameras = reader->ReadInt16();
        collisionHeader->camData.push_back(camDataEntry);
        
        int32_t camPosDataIdx = reader->ReadInt32();
        collisionHeader->camPosDataIndices.push_back(camPosDataIdx);
    }

    collisionHeader->camPosCount = reader->ReadInt32();
    collisionHeader->camPosData.reserve(collisionHeader->camPosCount);
    for (int32_t i = 0; i < collisionHeader->camPosCount; i++) {
        Vec3s pos;
        pos.x = reader->ReadInt16();
        pos.y = reader->ReadInt16();
        pos.z = reader->ReadInt16();
        collisionHeader->camPosData.push_back(pos);
    }

    Vec3s zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    collisionHeader->camPosDataZero = zero; 

    for (size_t i = 0; i < collisionHeader->camDataCount; i++) {
        int32_t idx = collisionHeader->camPosDataIndices[i];

        if (collisionHeader->camPosCount > 0) {
            collisionHeader->camData[i].camPosData = &collisionHeader->camPosData[idx];
        } else {
            collisionHeader->camData[i].camPosData = &collisionHeader->camPosDataZero;
        }
    }
    collisionHeader->collisionHeaderData.cameraDataList = collisionHeader->camData.data();
    collisionHeader->collisionHeaderData.cameraDataListLen = collisionHeader->camDataCount;

    collisionHeader->collisionHeaderData.numWaterBoxes = reader->ReadInt32();
    collisionHeader->waterBoxes.reserve(collisionHeader->collisionHeaderData.numWaterBoxes);
    for (int32_t i = 0; i < collisionHeader->collisionHeaderData.numWaterBoxes; i++) {
        WaterBox waterBox;
        waterBox.xMin = reader->ReadInt16();
        waterBox.ySurface = reader->ReadInt16();
        waterBox.zMin = reader->ReadInt16();
        waterBox.xLength = reader->ReadInt16();
        waterBox.zLength = reader->ReadInt16();
        waterBox.properties = reader->ReadInt32();

        collisionHeader->waterBoxes.push_back(waterBox);
    }
    collisionHeader->collisionHeaderData.waterBoxes = collisionHeader->waterBoxes.data();

    return collisionHeader;
}

std::shared_ptr<Ship::IResource> ResourceFactoryXMLCollisionHeaderV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto collisionHeader = std::make_shared<CollisionHeader>(file->InitData);

    auto reader = std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement();
    auto child = reader->FirstChildElement();

    collisionHeader->collisionHeaderData.minBounds.x = reader->IntAttribute("MinBoundsX");
    collisionHeader->collisionHeaderData.minBounds.y = reader->IntAttribute("MinBoundsY");
    collisionHeader->collisionHeaderData.minBounds.z = reader->IntAttribute("MinBoundsZ");

    collisionHeader->collisionHeaderData.maxBounds.x = reader->IntAttribute("MaxBoundsX");
    collisionHeader->collisionHeaderData.maxBounds.y = reader->IntAttribute("MaxBoundsY");
    collisionHeader->collisionHeaderData.maxBounds.z = reader->IntAttribute("MaxBoundsZ");

    Vec3s zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    collisionHeader->camPosDataZero = zero;

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "Vertex") {
            Vec3s vtx;
            vtx.x = child->IntAttribute("X");
            vtx.y = child->IntAttribute("Y");
            vtx.z = child->IntAttribute("Z");
            collisionHeader->vertices.push_back(vtx);
        } else if (childName == "Polygon") {
            CollisionPoly polygon;

            polygon.type = child->UnsignedAttribute("Type");
        
            polygon.flags_vIA = child->UnsignedAttribute("VertexA");
            polygon.flags_vIB = child->UnsignedAttribute("VertexB");
            polygon.vIC = child->UnsignedAttribute("VertexC");

            polygon.normal.x = child->IntAttribute("NormalX");
            polygon.normal.y = child->IntAttribute("NormalY");
            polygon.normal.z = child->IntAttribute("NormalZ");

            polygon.dist = child->IntAttribute("Dist");

            collisionHeader->polygons.push_back(polygon);
        } else if (childName == "PolygonType") {
            SurfaceType surfaceType;

            surfaceType.data[0] = child->UnsignedAttribute("Data1");
            surfaceType.data[1] = child->UnsignedAttribute("Data2");

            collisionHeader->surfaceTypes.push_back(surfaceType);
        } else if (childName == "CameraData") {
            CamData camDataEntry;
            camDataEntry.cameraSType = child->UnsignedAttribute("SType");
            camDataEntry.numCameras = child->IntAttribute("NumData");
            collisionHeader->camData.push_back(camDataEntry);
        
            int32_t camPosDataIdx = child->IntAttribute("CameraPosDataSeg");
            collisionHeader->camPosDataIndices.push_back(camPosDataIdx);
        } else if (childName == "CameraPositionData") {
            //each camera position data is made up of 3 Vec3s
            Vec3s pos;
            pos.x = child->IntAttribute("PosX");
            pos.y = child->IntAttribute("PosY");
            pos.z = child->IntAttribute("PosZ");
            collisionHeader->camPosData.push_back(pos);
            Vec3s rot;
            rot.x = child->IntAttribute("RotX");
            rot.y = child->IntAttribute("RotY");
            rot.z = child->IntAttribute("RotZ");
            collisionHeader->camPosData.push_back(rot);
            Vec3s other;
            other.x = child->IntAttribute("FOV");
            other.y = child->IntAttribute("JfifID");
            other.z = child->IntAttribute("Unknown");
            collisionHeader->camPosData.push_back(other);
        } else if (childName == "WaterBox") {
            WaterBox waterBox;
            waterBox.xMin = child->IntAttribute("XMin");
            waterBox.ySurface = child->IntAttribute("Ysurface");
            waterBox.zMin = child->IntAttribute("ZMin");
            waterBox.xLength = child->IntAttribute("XLength");
            waterBox.zLength = child->IntAttribute("ZLength");
            waterBox.properties = child->IntAttribute("Properties");

            collisionHeader->waterBoxes.push_back(waterBox);
        }

        child = child->NextSiblingElement();
    }

    for (size_t i = 0; i < collisionHeader->camData.size(); i++) {
        int32_t idx = collisionHeader->camPosDataIndices[i];

        if (collisionHeader->camPosData.size() > 0) {
            collisionHeader->camData[i].camPosData = &collisionHeader->camPosData[idx];
        } else {
            collisionHeader->camData[i].camPosData = &collisionHeader->camPosDataZero;
        }
    }
    
    collisionHeader->collisionHeaderData.numVertices = collisionHeader->vertices.size();
    collisionHeader->collisionHeaderData.numPolygons = collisionHeader->polygons.size();
    collisionHeader->surfaceTypesCount = collisionHeader->surfaceTypes.size();
    collisionHeader->camDataCount = collisionHeader->camData.size();
    collisionHeader->camPosCount = collisionHeader->camPosData.size();
    collisionHeader->collisionHeaderData.numWaterBoxes = collisionHeader->waterBoxes.size();

    collisionHeader->collisionHeaderData.vtxList = collisionHeader->vertices.data();
    collisionHeader->collisionHeaderData.polyList = collisionHeader->polygons.data();
    collisionHeader->collisionHeaderData.surfaceTypeList = collisionHeader->surfaceTypes.data();
    collisionHeader->collisionHeaderData.cameraDataList = collisionHeader->camData.data();
    collisionHeader->collisionHeaderData.cameraDataListLen = collisionHeader->camDataCount;
    collisionHeader->collisionHeaderData.waterBoxes = collisionHeader->waterBoxes.data();

    return collisionHeader;
}
} // namespace SOH
