#include "soh/resource/type/Path.h"
#include "spdlog/spdlog.h"

namespace SOH {
    void LogPathAsXML(std::shared_ptr<Ship::IResource> resource) {
        std::shared_ptr<Path> path = std::static_pointer_cast<Path>(resource);

        tinyxml2::XMLDocument doc;
        tinyxml2::XMLElement* root = doc.NewElement("Path");
        doc.InsertFirstChild(root);

        for (size_t i = 0; i < path->paths.size(); i += 1) {
            tinyxml2::XMLElement* pathData = doc.NewElement("PathData");

            for (size_t j = 0; j < path->paths[i].size(); j += 1) {
                tinyxml2::XMLElement* pathPoint = doc.NewElement("PathPoint");

                pathPoint->SetAttribute("X", path->paths[i][j].x);
                pathPoint->SetAttribute("Y", path->paths[i][j].y);
                pathPoint->SetAttribute("Z", path->paths[i][j].z);

                pathData->InsertEndChild(pathPoint);
            }

            root->InsertEndChild(pathData);
        }

        tinyxml2::XMLPrinter printer;
        doc.Accept(&printer);

        SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
    }
}