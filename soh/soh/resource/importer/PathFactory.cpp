#include "soh/resource/importer/PathFactory.h"
#include "soh/resource/type/Path.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> PathFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                    std::shared_ptr<ResourceInitData> initData,
                                                    std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<Path>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	factory = std::make_shared<PathFactoryV0>();
	break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Path with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::PathFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                          std::shared_ptr<Resource> resource) {
    std::shared_ptr<Path> path = std::static_pointer_cast<Path>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, path);

    path->numPaths = reader->ReadUInt32();
    path->paths.reserve(path->numPaths);
    for (uint32_t k = 0; k < path->numPaths; k++) {
        std::vector<Vec3s> points;
        uint32_t pointCount = reader->ReadUInt32();
        points.reserve(pointCount);
        for (uint32_t i = 0; i < pointCount; i++) {
            Vec3s point;
            point.x = reader->ReadInt16();
            point.y = reader->ReadInt16();
            point.z = reader->ReadInt16();

            points.push_back(point);
        }

        PathData pathDataEntry;
        pathDataEntry.count = pointCount;
        
        path->paths.push_back(points);
        pathDataEntry.points = path->paths.back().data();

        path->pathData.push_back(pathDataEntry);
    }
}
} // namespace Ship
