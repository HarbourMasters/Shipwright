#include "soh/resource/importer/PathFactory.h"
#include "soh/resource/type/Path.h"
#include "soh/resource/logging/PathLogger.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource> ResourceFactoryBinaryPathV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto path = std::make_shared<Path>(file->InitData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

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

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogPathAsXML(path);
    }

    return path;
}

std::shared_ptr<Ship::IResource> ResourceFactoryXMLPathV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto path = std::make_shared<Path>(file->InitData);
    auto reader = std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader);

    auto pathElement = reader->RootElement();

    //path->numPaths = pathElement->IntAttribute("NumPaths");
    //path->paths.reserve(path->numPaths);

    auto pathDataElement = pathElement->FirstChildElement();

    while (pathDataElement != nullptr) {
        std::vector<Vec3s> points;
        //uint32_t pointCount = pathDataElement->IntAttribute("NumPoints");
        //points.reserve(pointCount);

        auto pointElement = pathDataElement->FirstChildElement();

        while (pointElement != nullptr) {
            Vec3s point;
            point.x = pointElement->IntAttribute("X");
            point.y = pointElement->IntAttribute("Y");
            point.z = pointElement->IntAttribute("Z");

            points.push_back(point);

            pointElement = pointElement->NextSiblingElement();
        }

        PathData pathDataEntry;
        //pathDataEntry.count = pointCount;
        pathDataEntry.count = points.size();

        path->paths.push_back(points);
        pathDataEntry.points = path->paths.back().data();

        path->pathData.push_back(pathDataEntry);

        pathDataElement = pathDataElement->NextSiblingElement();
    }

    path->numPaths = path->paths.size();

    return path;
};
} // namespace SOH
