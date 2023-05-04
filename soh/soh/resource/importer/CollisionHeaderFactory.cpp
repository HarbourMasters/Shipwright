#include "soh/resource/importer/CollisionHeaderFactory.h"
#include "soh/resource/type/CollisionHeader.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> CollisionHeaderFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                               std::shared_ptr<ResourceInitData> initData,
                                                               std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<CollisionHeader>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	factory = std::make_shared<CollisionHeaderFactoryV0>();
	break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Collision Header with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::CollisionHeaderFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                     std::shared_ptr<Resource> resource)
{
    std::shared_ptr<CollisionHeader> collisionHeader = std::static_pointer_cast<CollisionHeader>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, collisionHeader);

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
}
}
