#include "soh/resource/importer/scenecommand/SetMeshFactory.h"
#include "soh/resource/type/scenecommand/SetMesh.h"
#include "spdlog/spdlog.h"
#include "libultraship/libultraship.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetMeshFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
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
            SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}", setMesh->meshHeader.base.type);
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

            int32_t polyType = reader->ReadInt8(); // Unused
            std::string meshOpa = reader->ReadString();
            std::string meshXlu = reader->ReadString();

            auto opaRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshOpa.c_str());
            auto xluRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshXlu.c_str());
            dlist.opa = meshOpa != "" ? (Gfx*)(opaRes ? opaRes->GetRawPointer() : nullptr) : 0;
            dlist.xlu = meshXlu != "" ? (Gfx*)(xluRes ? xluRes->GetRawPointer() : nullptr) : 0;

            setMesh->opaPaths.push_back(meshOpa);
            setMesh->xluPaths.push_back(meshXlu);
            setMesh->dlists.push_back(dlist);
        } else if (setMesh->meshHeader.base.type == 1) {
            PolygonDlist pType;

            setMesh->meshHeader.polygon1.format = reader->ReadUByte();
            std::string imgOpa = reader->ReadString();
            std::string imgXlu = reader->ReadString();

            auto opaRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(imgOpa.c_str());
            auto xluRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(imgXlu.c_str());
            pType.opa = imgOpa != "" ? (Gfx*)(opaRes ? opaRes->GetRawPointer() : nullptr) : 0;
            pType.xlu = imgXlu != "" ? (Gfx*)(xluRes ? xluRes->GetRawPointer() : nullptr) : 0;

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
                    setMesh->meshHeader.polygon1.single.tlut = (void*)image.tlut; // OTRTODO: type of bgimage.tlut should be uintptr_t
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

            opaRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshOpa.c_str());
            xluRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshXlu.c_str());
            pType.opa = meshOpa != "" ? (Gfx*)(opaRes ? opaRes->GetRawPointer() : nullptr) : 0;
            pType.xlu = meshXlu != "" ? (Gfx*)(xluRes ? xluRes->GetRawPointer() : nullptr) : 0;

            setMesh->opaPaths.push_back(meshOpa);
            setMesh->xluPaths.push_back(meshXlu);
            setMesh->dlists.push_back(pType);
        } else if (setMesh->meshHeader.base.type == 2) {
            PolygonDlist2 dlist;
            
            int32_t polyType = reader->ReadInt8(); // Unused
            dlist.pos.x = reader->ReadInt16();
            dlist.pos.y = reader->ReadInt16();
            dlist.pos.z = reader->ReadInt16();
            dlist.unk_06 = reader->ReadInt16();

            std::string meshOpa = reader->ReadString();
            std::string meshXlu = reader->ReadString();

            auto opaRes = meshOpa != "" ? LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshOpa.c_str()) : nullptr;
            auto xluRes = meshXlu != "" ? LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshXlu.c_str()) : nullptr;
            dlist.opa = (Gfx*)(opaRes ? opaRes->GetRawPointer() : nullptr);
            dlist.xlu = (Gfx*)(xluRes ? xluRes->GetRawPointer() : nullptr);

            setMesh->opaPaths.push_back(meshOpa);
            setMesh->xluPaths.push_back(meshXlu);
            setMesh->dlists2.push_back(dlist);
        } else {
            SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}", setMesh->meshHeader.base.type);
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
        SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}", setMesh->meshHeader.base.type);
    }

    //LogMeshAsXML(setMesh);

    return setMesh;
}

std::shared_ptr<LUS::IResource> SetMeshFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
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
            SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}", setMesh->meshHeader.base.type);
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

            int32_t polyType = child->IntAttribute("PolyType"); // Unused
            std::string meshOpa = child->Attribute("MeshOpa");
            std::string meshXlu = child->Attribute("MeshXlu");

            auto opaRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshOpa.c_str());
            auto xluRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshXlu.c_str());
            dlist.opa = meshOpa != "" ? (Gfx*)(opaRes ? opaRes->GetRawPointer() : nullptr) : 0;
            dlist.xlu = meshXlu != "" ? (Gfx*)(xluRes ? xluRes->GetRawPointer() : nullptr) : 0;

            setMesh->opaPaths.push_back(meshOpa);
            setMesh->xluPaths.push_back(meshXlu);
            setMesh->dlists.push_back(dlist);
        } else if (setMesh->meshHeader.base.type == 1) {
            PolygonDlist pType;

            setMesh->meshHeader.polygon1.format = child->IntAttribute("Format");
            std::string imgOpa = child->Attribute("ImgOpa");
            std::string imgXlu = child->Attribute("ImgXlu");

            auto opaRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(imgOpa.c_str());
            auto xluRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(imgXlu.c_str());
            pType.opa = imgOpa != "" ? (Gfx*)(opaRes ? opaRes->GetRawPointer() : nullptr) : 0;
            pType.xlu = imgXlu != "" ? (Gfx*)(xluRes ? xluRes->GetRawPointer() : nullptr) : 0;

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
                    setMesh->meshHeader.polygon1.single.tlut = (void*)image.tlut; // OTRTODO: type of bgimage.tlut should be uintptr_t
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

            opaRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshOpa.c_str());
            xluRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshXlu.c_str());
            pType.opa = meshOpa != "" ? (Gfx*)(opaRes ? opaRes->GetRawPointer() : nullptr) : 0;
            pType.xlu = meshXlu != "" ? (Gfx*)(xluRes ? xluRes->GetRawPointer() : nullptr) : 0;

            setMesh->opaPaths.push_back(meshOpa);
            setMesh->xluPaths.push_back(meshXlu);
            setMesh->dlists.push_back(pType);
        } else if (setMesh->meshHeader.base.type == 2) {
            PolygonDlist2 dlist;

            int32_t polyType = child->IntAttribute("PolyType"); // Unused
            dlist.pos.x = child->IntAttribute("PosX");
            dlist.pos.y = child->IntAttribute("PosY");
            dlist.pos.z = child->IntAttribute("PosZ");
            dlist.unk_06 = child->IntAttribute("Unknown");

            std::string meshOpa = child->Attribute("MeshOpa");
            std::string meshXlu = child->Attribute("MeshXlu");

            auto opaRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshOpa.c_str());
            auto xluRes = LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(meshXlu.c_str());
            dlist.opa = meshOpa != "" ? (Gfx*)(opaRes ? opaRes->GetRawPointer() : nullptr) : 0;
            dlist.xlu = meshXlu != "" ? (Gfx*)(xluRes ? xluRes->GetRawPointer() : nullptr) : 0;

            setMesh->opaPaths.push_back(meshOpa);
            setMesh->xluPaths.push_back(meshXlu);
            setMesh->dlists2.push_back(dlist);
        } else {
            SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}", setMesh->meshHeader.base.type);
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
        SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}", setMesh->meshHeader.base.type);
    }

    return setMesh;
}

void LogMeshAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetMesh> setMesh = std::static_pointer_cast<SetMesh>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetMesh");
    doc.InsertFirstChild(root);

    root->SetAttribute("Data", setMesh->data);
    root->SetAttribute("MeshHeaderType", setMesh->meshHeader.base.type);

    if (setMesh->meshHeader.base.type == 0) {
        root->SetAttribute("PolyNum", setMesh->meshHeader.polygon0.num);
        PolygonDlist* dlist = (PolygonDlist*)setMesh->meshHeader.polygon0.start;
        for (int i = 0; i < setMesh->meshHeader.polygon0.num; i += 1) {
            tinyxml2::XMLElement* polygon = doc.NewElement("Polygon");
            polygon->SetAttribute("PolyType", "0");
            polygon->SetAttribute("MeshOpa", setMesh->opaPaths[i].c_str());
            polygon->SetAttribute("MeshXlu", setMesh->xluPaths[i].c_str());

            root->InsertEndChild(polygon);
        }
        dlist += 1;
    } else if (setMesh->meshHeader.base.type == 1) {
        root->SetAttribute("PolyNum", "1");
        tinyxml2::XMLElement* polygon = doc.NewElement("Polygon");
        polygon->SetAttribute("Format", setMesh->meshHeader.polygon1.format);
        polygon->SetAttribute("ImgOpa", "");
        polygon->SetAttribute("ImgXlu", "");

        if (setMesh->meshHeader.polygon1.format == 1) {
            polygon->SetAttribute("BgImageCount", "0");
        } else {
            polygon->SetAttribute("BgImageCount", setMesh->meshHeader.polygon1.multi.count);
        }

        polygon->SetAttribute("PolyType", "0");

        polygon->SetAttribute("MeshOpa", setMesh->opaPaths[0].c_str());
        polygon->SetAttribute("MeshXlu", setMesh->xluPaths[0].c_str());

        root->InsertEndChild(polygon);

        BgImage* image = setMesh->meshHeader.polygon1.multi.list;
        int count = setMesh->meshHeader.polygon1.format == 1 ? 1 : setMesh->meshHeader.polygon1.multi.count;
        for (int i = 0; i < count; i += 1) {
            tinyxml2::XMLElement* bgImage = doc.NewElement("BgImage");
            if (setMesh->meshHeader.polygon1.format == 1) {
                bgImage->SetAttribute("Unknown_00", image->unk_00);
                bgImage->SetAttribute("Id", image->id);
                bgImage->SetAttribute("ImagePath", setMesh->imagePaths[i].c_str());
                bgImage->SetAttribute("Unknown_0C", setMesh->meshHeader.polygon1.single.unk_0C);
                bgImage->SetAttribute("TLUT", setMesh->meshHeader.polygon1.single.tlut);
                bgImage->SetAttribute("Width", setMesh->meshHeader.polygon1.single.width);
                bgImage->SetAttribute("Height", setMesh->meshHeader.polygon1.single.height);
                bgImage->SetAttribute("Fmt", setMesh->meshHeader.polygon1.single.fmt);
                bgImage->SetAttribute("Siz", setMesh->meshHeader.polygon1.single.siz);
                bgImage->SetAttribute("Mode0", setMesh->meshHeader.polygon1.single.mode0);
                bgImage->SetAttribute("TLUTCount", setMesh->meshHeader.polygon1.single.tlutCount);
            } else {
                bgImage->SetAttribute("Unknown_00", image->unk_00);
                bgImage->SetAttribute("Id", image->id);
                bgImage->SetAttribute("ImagePath", setMesh->imagePaths[i].c_str());
                bgImage->SetAttribute("Unknown_0C", image->unk_0C);
                bgImage->SetAttribute("TLUT", image->tlut);
                bgImage->SetAttribute("Width", image->width);
                bgImage->SetAttribute("Height", image->height);
                bgImage->SetAttribute("Fmt", image->fmt);
                bgImage->SetAttribute("Siz", image->siz);
                bgImage->SetAttribute("Mode0", image->mode0);
                bgImage->SetAttribute("TLUTCount", image->tlutCount);
            }
            polygon->InsertEndChild(bgImage);

            image += 1;
        }
    } else if (setMesh->meshHeader.base.type == 2) {
        root->SetAttribute("PolyNum", setMesh->meshHeader.polygon2.num);
        PolygonDlist2* dlist = (PolygonDlist2*)setMesh->meshHeader.polygon2.start;
        for (int i = 0; i < setMesh->meshHeader.polygon2.num; i += 1) {
            tinyxml2::XMLElement* polygon = doc.NewElement("Polygon");
            polygon->SetAttribute("PolyType", "0");

            polygon->SetAttribute("PosX", dlist->pos.x);
            polygon->SetAttribute("PosY", dlist->pos.y);
            polygon->SetAttribute("PosZ", dlist->pos.z);
            polygon->SetAttribute("Unknown", dlist->unk_06);

            polygon->SetAttribute("MeshOpa", setMesh->opaPaths[i].c_str());
            polygon->SetAttribute("MeshXlu", setMesh->xluPaths[i].c_str());

            root->InsertEndChild(polygon);
            dlist += 1;
        }
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
