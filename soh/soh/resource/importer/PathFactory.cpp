#include "soh/resource/importer/PathFactory.h"
#include "soh/resource/type/Path.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> PathFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<Path>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<PathFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load Path with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::PathFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                          std::shared_ptr<Resource> resource)
{
	std::shared_ptr<Path> path = std::static_pointer_cast<Path>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, path);

    path->numPaths = reader->ReadUInt32();
    path->paths.reserve(path->numPaths);
    for (uint32_t k = 0; k < path->numPaths; k++) {
        std::vector<Vec3s> points;
        uint32_t pointCount = reader->ReadUInt32();
        points.reserve(pointCount);
        for (uint32_t i = 0; i < pointCount; i++) {
            int16_t x = reader->ReadInt16();
            int16_t y = reader->ReadInt16();
            int16_t z = reader->ReadInt16();

            points.push_back(Vec3s(x, y, z));
        }

        PathData pathDataEntry;
        pathDataEntry.count = pointCount;
        
        path->paths.push_back(points);
        pathDataEntry.points = path->paths.back().data();

        path->pathData.push_back(pathDataEntry);
    }
}
} // namespace Ship
