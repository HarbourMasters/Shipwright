#include "resource/importer/CollisionHeaderFactory.h"
#include "resource/type/CollisionHeader.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> CollisionHeaderFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<CollisionHeader>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
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
	ResourceFile::ParseFileBinary(reader, collisionHeader);

    collisionHeader->absMinX = reader->ReadInt16();
    collisionHeader->absMinY = reader->ReadInt16();
    collisionHeader->absMinZ = reader->ReadInt16();

    collisionHeader->absMaxX = reader->ReadInt16();
    collisionHeader->absMaxY = reader->ReadInt16();
    collisionHeader->absMaxZ = reader->ReadInt16();

    collisionHeader->vertexCount = reader->ReadInt32();
    collisionHeader->vertices.reserve(collisionHeader->vertexCount);
    for (int32_t i = 0; i < collisionHeader->vertexCount; i++) {
        float x = reader->ReadInt16();
        float y = reader->ReadInt16();
        float z = reader->ReadInt16();
        collisionHeader->vertices.push_back(Vec3f(x, y, z));
    }

    collisionHeader->polygonCount = reader->ReadUInt32();
    collisionHeader->polygons.reserve(collisionHeader->polygonCount);
    for (uint32_t i = 0; i < collisionHeader->polygonCount; i++) {
        CollisionPoly polygon;

        polygon.type = reader->ReadUInt16();
        
        polygon.flags_vIA = reader->ReadUInt16();
        polygon.flags_vIB = reader->ReadUInt16();
        polygon.vIC = reader->ReadUInt16();

        int16_t a = reader->ReadUInt16();
        int16_t b = reader->ReadUInt16();
        int16_t c = reader->ReadUInt16();
        polygon.normal = Vec3s(a, b, c);

        polygon.dist = reader->ReadUInt16();

        collisionHeader->polygons.push_back(polygon);
    }

    collisionHeader->polygonTypesCount = reader->ReadUInt32();
    collisionHeader->PolygonTypes.reserve(collisionHeader->polygonTypesCount);
    for (uint32_t i = 0; i < collisionHeader->polygonTypesCount; i++) {
        uint32_t col1 = reader->ReadUInt32();
        uint32_t col2 = reader->ReadUInt32();

        collisionHeader->PolygonTypes.push_back({ col1, col2 });
    }

    collisionHeader->camDataCount = reader->ReadUInt32();
    collisionHeader->camData.reserve(collisionHeader->camDataCount);
    collisionHeader->camPosDataIndices.reserve(collisionHeader->camDataCount);
    collisionHeader->camPosData.reserve(collisionHeader->camDataCount);
    for (uint32_t i = 0; i < collisionHeader->camDataCount; i++) {
        CamData camDataEntry;
        camDataEntry.cameraSType = reader->ReadUInt16();
        camDataEntry.numCameras = reader->ReadInt16();
        int32_t camPosDataIdx = reader->ReadInt32();
        collisionHeader->camPosDataIndices.push_back(camPosDataIdx);
        camDataEntry.camPosData = collisionHeader->camPosData[i].data();
        collisionHeader->camData.push_back(camDataEntry);
    }

    // TODO: still not sure what's going on here
    //  
    // uint32_t camPosCnt = reader->ReadInt32();
    // col->camData->cameraPositionData.reserve(camPosCnt);

    // for (uint32_t i = 0; i < camPosCnt; i++) {
    //     Ship::CameraPositionData* entry = new Ship::CameraPositionData();
    //     entry->x = reader->ReadInt16();
    //     entry->y = reader->ReadInt16();
    //     entry->z = reader->ReadInt16();
    //     col->camData->cameraPositionData.push_back(entry);
    // }

}
} // namespace Ship






//     uint32_t waterBoxCnt = reader->ReadInt32();
//     col->waterBoxes.reserve(waterBoxCnt);

//     for (uint32_t i = 0; i < waterBoxCnt; i++) {
//         Ship::WaterBoxHeader waterBox;
//         waterBox.xMin = reader->ReadInt16();
//         waterBox.ySurface = reader->ReadInt16();
//         waterBox.zMin = reader->ReadInt16();
//         waterBox.xLength = reader->ReadInt16();
//         waterBox.zLength = reader->ReadInt16();
//         waterBox.properties = reader->ReadInt32();

//         col->waterBoxes.push_back(waterBox);
//     }
// }

// Ship::PolygonEntry::PolygonEntry(BinaryReader* reader) {
//     type = reader->ReadUInt16();

//     vtxA = reader->ReadUInt16();
//     vtxB = reader->ReadUInt16();
//     vtxC = reader->ReadUInt16();

//     a = reader->ReadUInt16();
//     b = reader->ReadUInt16();
//     c = reader->ReadUInt16();
//     d = reader->ReadUInt16();
// }