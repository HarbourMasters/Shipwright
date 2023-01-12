#include "soh/resource/importer/CollisionHeaderFactory.h"
#include "soh/resource/type/CollisionHeader.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> CollisionHeaderFactory::ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<CollisionHeader>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	switch (version)
	{
	case 0:
		factory = std::make_shared<CollisionHeaderFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load Collision Header with version {}", version);
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

    collisionHeader->absMinX = reader->ReadInt16();
    collisionHeader->absMinY = reader->ReadInt16();
    collisionHeader->absMinZ = reader->ReadInt16();

    collisionHeader->absMaxX = reader->ReadInt16();
    collisionHeader->absMaxY = reader->ReadInt16();
    collisionHeader->absMaxZ = reader->ReadInt16();

    collisionHeader->vertexCount = reader->ReadInt32();
    collisionHeader->vertices.reserve(collisionHeader->vertexCount);
    for (int32_t i = 0; i < collisionHeader->vertexCount; i++) {
        int16_t x = reader->ReadInt16();
        int16_t y = reader->ReadInt16();
        int16_t z = reader->ReadInt16();
        collisionHeader->vertices.push_back(Vec3s{x, y, z});
    }
    collisionHeader->collisionHeaderData.vtxList = collisionHeader->vertices.data();

    collisionHeader->polygonCount = reader->ReadUInt32();
    collisionHeader->polygons.reserve(collisionHeader->polygonCount);
    for (uint32_t i = 0; i < collisionHeader->polygonCount; i++) {
        CollisionPoly polygon;

        polygon.type = reader->ReadUInt16();
        
        polygon.flags_vIA = reader->ReadUInt16();
        polygon.flags_vIB = reader->ReadUInt16();
        polygon.vIC = reader->ReadUInt16();

        int16_t a = reader->ReadInt16();
        int16_t b = reader->ReadInt16();
        int16_t c = reader->ReadInt16();
        polygon.normal = Vec3s{a, b, c};

        polygon.dist = reader->ReadUInt16();

        collisionHeader->polygons.push_back(polygon);
        
    }
    collisionHeader->collisionHeaderData.polyList = collisionHeader->polygons.data();

    collisionHeader->surfaceTypesCount = reader->ReadUInt32();
    collisionHeader->surfaceTypes.reserve(collisionHeader->surfaceTypesCount);
    for (uint32_t i = 0; i < collisionHeader->surfaceTypesCount; i++) {
        SurfaceType surfaceType;

        surfaceType.data[0] = reader->ReadUInt32();
        surfaceType.data[1] = reader->ReadUInt32();
        
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
        int16_t x = reader->ReadInt16();
        int16_t y = reader->ReadInt16();
        int16_t z = reader->ReadInt16();
        collisionHeader->camPosData.push_back(Vec3s{x, y, z});
    }

    for (size_t i = 0; i < collisionHeader->camDataCount; i++) {
        int32_t idx = collisionHeader->camPosDataIndices[i];

        if (collisionHeader->camPosCount > 0) {
            collisionHeader->camData[i].camPosData->x = collisionHeader->camPosData[idx].x;
            collisionHeader->camData[i].camPosData->y = collisionHeader->camPosData[idx].y;
            collisionHeader->camData[i].camPosData->z = collisionHeader->camPosData[idx].z;
        } else {
            collisionHeader->camData[i].camPosData->x = 0;
            collisionHeader->camData[i].camPosData->y = 0;
            collisionHeader->camData[i].camPosData->z = 0;
        }
    }
    collisionHeader->collisionHeaderData.cameraDataList = collisionHeader->camData.data();
    collisionHeader->collisionHeaderData.cameraDataListLen = collisionHeader->camDataCount;

    collisionHeader->waterBoxCount = reader->ReadInt32();
    collisionHeader->waterBoxes.reserve(collisionHeader->waterBoxCount);
    for (int32_t i = 0; i < collisionHeader->waterBoxCount; i++) {
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
