#include "soh/resource/importer/PathFactory.h"
#include "soh/resource/type/Path.h"
#include "spdlog/spdlog.h"

std::shared_ptr<LUS::IResource> ResourceFactoryBinaryPathV0::ReadResource(std::shared_ptr<LUS::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto path = std::make_shared<Path>(file->InitData);

    path->numPaths = file->Reader->ReadUInt32();
    path->paths.reserve(path->numPaths);
    for (uint32_t k = 0; k < path->numPaths; k++) {
        std::vector<Vec3s> points;
        uint32_t pointCount = file->Reader->ReadUInt32();
        points.reserve(pointCount);
        for (uint32_t i = 0; i < pointCount; i++) {
            Vec3s point;
            point.x = file->Reader->ReadInt16();
            point.y = file->Reader->ReadInt16();
            point.z = file->Reader->ReadInt16();

            points.push_back(point);
        }

        PathData pathDataEntry;
        pathDataEntry.count = pointCount;
        
        path->paths.push_back(points);
        pathDataEntry.points = path->paths.back().data();

        path->pathData.push_back(pathDataEntry);
    }

    return path;
}
