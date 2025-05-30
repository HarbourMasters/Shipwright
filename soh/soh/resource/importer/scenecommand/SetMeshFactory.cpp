#include "soh/resource/importer/scenecommand/SetMeshFactory.h"
#include "soh/resource/type/scenecommand/SetMesh.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"
#include <tinyxml2.h>
#include "libultraship/libultraship.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetMeshFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                              std::shared_ptr<Ship::BinaryReader> reader) {
    auto setMesh = std::make_shared<SetMesh>(initData);

    ReadCommandId(setMesh, reader);

    setMesh->data = reader->ReadInt8();

    setMesh->meshHeader.base.type = reader->ReadInt8();
    int32_t polyNum = 1;

    if (setMesh->meshHeader.base.type != 1) {
        polyNum = reader->ReadInt8();
        if (setMesh->meshHeader.base.type == 0) {
            setMesh->meshHeader.polygon0.num = polyNum;
        } else if (setMesh->meshHeader.base.type == 2) {
            setMesh->meshHeader.polygon2.num = polyNum;
        } else {
            SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}",
                         setMesh->meshHeader.base.type);
        }
    }

    if (setMesh->meshHeader.base.type == 2) {
        setMesh->dlists2.reserve(polyNum);
    } else {
        setMesh->dlists.reserve(setMesh->meshHeader.polygon0.num);
    }

    for (int32_t i = 0; i < polyNum; i++) {
        if (setMesh->meshHeader.base.type == 0) {
            PolygonDlist dlist;
            dlist.opa = nullptr;
            dlist.xlu = nullptr;

            int32_t polyType = reader->ReadInt8(); // Unused
            std::string meshOpa = reader->ReadString();
            std::string meshXlu = reader->ReadString();

            // Enables alt-toggling support by setting maintained c_str references to DList resource after pushing to
            // vector the first Defers resource loading later in game when the scene is drawn
            if (meshOpa != "") {
                meshOpa = "__OTR__" + meshOpa;
                setMesh->opaPaths.push_back(meshOpa);
                dlist.opa = (Gfx*)setMesh->opaPaths.back().c_str();
            }
            if (meshXlu != "") {
                meshXlu = "__OTR__" + meshXlu;
                setMesh->xluPaths.push_back(meshXlu);
                dlist.xlu = (Gfx*)setMesh->xluPaths.back().c_str();
            }

            setMesh->dlists.push_back(dlist);
        } else if (setMesh->meshHeader.base.type == 1) {
            PolygonDlist pType;
            pType.opa = nullptr;
            pType.xlu = nullptr;

            setMesh->meshHeader.polygon1.format = reader->ReadUByte();

            // These strings are the same that are read and used below. Not sure why they get exported twice from the
            // exporter. We read and ignore these to advance the reader.
            reader->ReadString();
            reader->ReadString();

            int32_t bgImageCount = reader->ReadUInt32();
            setMesh->images.reserve(bgImageCount);

            for (int32_t i = 0; i < bgImageCount; i++) {
                BgImage image;
                image.unk_00 = reader->ReadUInt16();
                image.id = reader->ReadUByte();
                std::string imagePath = "__OTR__" + reader->ReadString();
                setMesh->imagePaths.push_back(imagePath);
                image.source = (void*)setMesh->imagePaths.back().c_str();
                image.unk_0C = reader->ReadUInt32();
                image.tlut = reader->ReadUInt32();
                image.width = reader->ReadUInt16();
                image.height = reader->ReadUInt16();
                image.fmt = reader->ReadUByte();
                image.siz = reader->ReadUByte();
                image.mode0 = reader->ReadUInt16();
                image.tlutCount = reader->ReadUInt16();

                if (setMesh->meshHeader.polygon1.format == 1) {
                    setMesh->meshHeader.polygon1.single.source = image.source;
                    setMesh->meshHeader.polygon1.single.unk_0C = image.unk_0C;
                    setMesh->meshHeader.polygon1.single.tlut =
                        (void*)image.tlut; // OTRTODO: type of bgimage.tlut should be uintptr_t
                    setMesh->meshHeader.polygon1.single.width = image.width;
                    setMesh->meshHeader.polygon1.single.height = image.height;
                    setMesh->meshHeader.polygon1.single.fmt = image.fmt;
                    setMesh->meshHeader.polygon1.single.siz = image.siz;
                    setMesh->meshHeader.polygon1.single.mode0 = image.mode0;
                    setMesh->meshHeader.polygon1.single.tlutCount = image.tlutCount;
                } else {
                    setMesh->images.push_back(image);
                }
            }

            if (setMesh->meshHeader.polygon1.format != 1) {
                setMesh->meshHeader.polygon1.multi.count = bgImageCount;
            }

            int32_t polyType = reader->ReadInt8(); // Unused??

            std::string meshOpa = reader->ReadString();
            std::string meshXlu = reader->ReadString();

            // Use long-lived maintained c_str references
            if (meshOpa != "") {
                meshOpa = "__OTR__" + meshOpa;
                setMesh->opaPaths.push_back(meshOpa);
                pType.opa = (Gfx*)setMesh->opaPaths.back().c_str();
            }
            if (meshXlu != "") {
                meshXlu = "__OTR__" + meshXlu;
                setMesh->xluPaths.push_back(meshXlu);
                pType.xlu = (Gfx*)setMesh->xluPaths.back().c_str();
            }

            setMesh->dlists.push_back(pType);
        } else if (setMesh->meshHeader.base.type == 2) {
            PolygonDlist2 dlist;
            dlist.opa = nullptr;
            dlist.xlu = nullptr;

            int32_t polyType = reader->ReadInt8(); // Unused
            dlist.pos.x = reader->ReadInt16();
            dlist.pos.y = reader->ReadInt16();
            dlist.pos.z = reader->ReadInt16();
            dlist.unk_06 = reader->ReadInt16();

            std::string meshOpa = reader->ReadString();
            std::string meshXlu = reader->ReadString();

            // Use long-lived maintained c_str references
            if (meshOpa != "") {
                meshOpa = "__OTR__" + meshOpa;
                setMesh->opaPaths.push_back(meshOpa);
                dlist.opa = (Gfx*)setMesh->opaPaths.back().c_str();
            }
            if (meshXlu != "") {
                meshXlu = "__OTR__" + meshXlu;
                setMesh->xluPaths.push_back(meshXlu);
                dlist.xlu = (Gfx*)setMesh->xluPaths.back().c_str();
            }

            setMesh->dlists2.push_back(dlist);
        } else {
            SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}",
                         setMesh->meshHeader.base.type);
        }
    }

    if (setMesh->meshHeader.base.type == 2) {
        setMesh->meshHeader.polygon2.start = setMesh->dlists2.data();
    } else if (setMesh->meshHeader.base.type == 0) {
        setMesh->meshHeader.polygon0.start = setMesh->dlists.data();
    } else if (setMesh->meshHeader.base.type == 1) {
        setMesh->meshHeader.polygon1.multi.list = setMesh->images.data();
        setMesh->meshHeader.polygon1.dlist = (Gfx*)setMesh->dlists.data();
    } else {
        SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}",
                     setMesh->meshHeader.base.type);
    }

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogMeshAsXML(setMesh);
    }

    return setMesh;
}

std::shared_ptr<Ship::IResource> SetMeshFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                 tinyxml2::XMLElement* reader) {
    auto setMesh = std::make_shared<SetMesh>(initData);

    setMesh->cmdId = SceneCommandID::SetMesh;

    setMesh->data = reader->IntAttribute("Data");

    setMesh->meshHeader.base.type = reader->IntAttribute("MeshHeaderType");
    int32_t polyNum = 1;

    if (setMesh->meshHeader.base.type != 1) {
        polyNum = reader->IntAttribute("PolyNum");
        if (setMesh->meshHeader.base.type == 0) {
            setMesh->meshHeader.polygon0.num = polyNum;
        } else if (setMesh->meshHeader.base.type == 2) {
            setMesh->meshHeader.polygon2.num = polyNum;
        } else {
            SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}",
                         setMesh->meshHeader.base.type);
        }
    }

    if (setMesh->meshHeader.base.type == 2) {
        setMesh->dlists2.reserve(polyNum);
    } else {
        setMesh->dlists.reserve(setMesh->meshHeader.polygon0.num);
    }

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName != "Polygon") {
            child = child->NextSiblingElement();
            continue;
        }

        if (setMesh->meshHeader.base.type == 0) {
            PolygonDlist dlist;
            dlist.opa = nullptr;
            dlist.xlu = nullptr;

            int32_t polyType = child->IntAttribute("PolyType"); // Unused
            std::string meshOpa = child->Attribute("MeshOpa");
            std::string meshXlu = child->Attribute("MeshXlu");

            // Enables alt-toggling support by setting maintained c_str references to DList resource after pushing to
            // vector the first Defers resource loading later in game when the scene is drawn
            if (meshOpa != "") {
                meshOpa = "__OTR__" + meshOpa;
                setMesh->opaPaths.push_back(meshOpa);
                dlist.opa = (Gfx*)setMesh->opaPaths.back().c_str();
            }
            if (meshXlu != "") {
                meshXlu = "__OTR__" + meshXlu;
                setMesh->xluPaths.push_back(meshXlu);
                dlist.xlu = (Gfx*)setMesh->xluPaths.back().c_str();
            }

            setMesh->dlists.push_back(dlist);
        } else if (setMesh->meshHeader.base.type == 1) {
            PolygonDlist pType;
            pType.opa = nullptr;
            pType.xlu = nullptr;

            setMesh->meshHeader.polygon1.format = child->IntAttribute("Format");

            int32_t bgImageCount = child->IntAttribute("BgImageCount");
            setMesh->images.reserve(bgImageCount);

            auto grandChild = child->FirstChildElement();

            while (grandChild != nullptr) {
                std::string grandChildName = grandChild->Name();
                if (grandChildName != "BgImage") {
                    grandChild = grandChild->NextSiblingElement();
                    continue;
                }
                BgImage image;
                image.unk_00 = grandChild->IntAttribute("Unknown_00");
                image.id = grandChild->IntAttribute("Id");
                std::string imagePath = "__OTR__" + std::string(grandChild->Attribute("ImagePath"));
                setMesh->imagePaths.push_back(imagePath);
                image.source = (void*)setMesh->imagePaths.back().c_str();
                image.unk_0C = grandChild->IntAttribute("Unknown_0C");
                image.tlut = grandChild->IntAttribute("TLUT");
                image.width = grandChild->IntAttribute("Width");
                image.height = grandChild->IntAttribute("Height");
                image.fmt = grandChild->IntAttribute("Fmt");
                image.siz = grandChild->IntAttribute("Siz");
                image.mode0 = grandChild->IntAttribute("Mode0");
                image.tlutCount = grandChild->IntAttribute("TLUTCount");

                if (setMesh->meshHeader.polygon1.format == 1) {
                    setMesh->meshHeader.polygon1.single.source = image.source;
                    setMesh->meshHeader.polygon1.single.unk_0C = image.unk_0C;
                    setMesh->meshHeader.polygon1.single.tlut =
                        (void*)image.tlut; // OTRTODO: type of bgimage.tlut should be uintptr_t
                    setMesh->meshHeader.polygon1.single.width = image.width;
                    setMesh->meshHeader.polygon1.single.height = image.height;
                    setMesh->meshHeader.polygon1.single.fmt = image.fmt;
                    setMesh->meshHeader.polygon1.single.siz = image.siz;
                    setMesh->meshHeader.polygon1.single.mode0 = image.mode0;
                    setMesh->meshHeader.polygon1.single.tlutCount = image.tlutCount;
                } else {
                    setMesh->images.push_back(image);
                }

                grandChild = grandChild->NextSiblingElement();
            }

            if (setMesh->meshHeader.polygon1.format != 1) {
                setMesh->meshHeader.polygon1.multi.count = bgImageCount;
            }

            int32_t polyType = child->IntAttribute("PolyType"); // Unused??

            std::string meshOpa = child->Attribute("MeshOpa");
            std::string meshXlu = child->Attribute("MeshXlu");

            // Use long-lived maintained c_str references
            if (meshOpa != "") {
                meshOpa = "__OTR__" + meshOpa;
                setMesh->opaPaths.push_back(meshOpa);
                pType.opa = (Gfx*)setMesh->opaPaths.back().c_str();
            }
            if (meshXlu != "") {
                meshXlu = "__OTR__" + meshXlu;
                setMesh->xluPaths.push_back(meshXlu);
                pType.xlu = (Gfx*)setMesh->xluPaths.back().c_str();
            }

            setMesh->dlists.push_back(pType);
        } else if (setMesh->meshHeader.base.type == 2) {
            PolygonDlist2 dlist;
            dlist.opa = nullptr;
            dlist.xlu = nullptr;

            int32_t polyType = child->IntAttribute("PolyType"); // Unused
            dlist.pos.x = child->IntAttribute("PosX");
            dlist.pos.y = child->IntAttribute("PosY");
            dlist.pos.z = child->IntAttribute("PosZ");
            dlist.unk_06 = child->IntAttribute("Unknown");

            std::string meshOpa = child->Attribute("MeshOpa");
            std::string meshXlu = child->Attribute("MeshXlu");

            // Use long-lived maintained c_str references
            if (meshOpa != "") {
                meshOpa = "__OTR__" + meshOpa;
                setMesh->opaPaths.push_back(meshOpa);
                dlist.opa = (Gfx*)setMesh->opaPaths.back().c_str();
            }
            if (meshXlu != "") {
                meshXlu = "__OTR__" + meshXlu;
                setMesh->xluPaths.push_back(meshXlu);
                dlist.xlu = (Gfx*)setMesh->xluPaths.back().c_str();
            }

            setMesh->dlists2.push_back(dlist);
        } else {
            SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}",
                         setMesh->meshHeader.base.type);
        }

        child = child->NextSiblingElement();
    }

    if (setMesh->meshHeader.base.type == 2) {
        setMesh->meshHeader.polygon2.start = setMesh->dlists2.data();
    } else if (setMesh->meshHeader.base.type == 0) {
        setMesh->meshHeader.polygon0.start = setMesh->dlists.data();
    } else if (setMesh->meshHeader.base.type == 1) {
        setMesh->meshHeader.polygon1.multi.list = setMesh->images.data();
        setMesh->meshHeader.polygon1.dlist = (Gfx*)setMesh->dlists.data();
    } else {
        SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}",
                     setMesh->meshHeader.base.type);
    }

    return setMesh;
}
} // namespace SOH
