#include "soh/resource/type/scenecommand/EndMarker.h"
#include "soh/resource/type/scenecommand/SetActorList.h"
#include "soh/resource/type/scenecommand/SetAlternateHeaders.h"
#include "soh/resource/type/scenecommand/SetCameraSettings.h"
#include "soh/resource/type/scenecommand/SetCollisionHeader.h"
#include "soh/resource/type/scenecommand/SetCsCamera.h"
#include "soh/resource/type/scenecommand/SetCutscenes.h"
#include "soh/resource/type/scenecommand/SetEchoSettings.h"
#include "soh/resource/type/scenecommand/SetEntranceList.h"
#include "soh/resource/type/scenecommand/SetExitList.h"
#include "soh/resource/type/scenecommand/SetLightingSettings.h"
#include "soh/resource/type/scenecommand/SetLightList.h"
#include "soh/resource/type/scenecommand/SetMesh.h"
#include "soh/resource/type/scenecommand/SetObjectList.h"
#include "soh/resource/type/scenecommand/SetPathways.h"
#include "soh/resource/type/scenecommand/SetRoomBehavior.h"
#include "soh/resource/type/scenecommand/SetRoomList.h"
#include "soh/resource/type/scenecommand/SetSkyboxModifier.h"
#include "soh/resource/type/scenecommand/SetSkyboxSettings.h"
#include "soh/resource/type/scenecommand/SetSoundSettings.h"
#include "soh/resource/type/scenecommand/SetSpecialObjects.h"
#include "soh/resource/type/scenecommand/SetStartPositionList.h"
#include "soh/resource/type/scenecommand/SetTimeSettings.h"
#include "soh/resource/type/scenecommand/SetTransitionActorList.h"
#include "soh/resource/type/scenecommand/SetWindSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {

void LogEndMarkerAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<EndMarker> endMarker = std::static_pointer_cast<EndMarker>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("EndMarker");
    doc.InsertFirstChild(root);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogActorListAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetActorList> setActorList = std::static_pointer_cast<SetActorList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetActorList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setActorList->numActors; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("ActorEntry");
        entry->SetAttribute("Id", setActorList->actorList[i].id);
        entry->SetAttribute("PosX", setActorList->actorList[i].pos.x);
        entry->SetAttribute("PosY", setActorList->actorList[i].pos.y);
        entry->SetAttribute("PosZ", setActorList->actorList[i].pos.z);
        entry->SetAttribute("RotX", setActorList->actorList[i].rot.x);
        entry->SetAttribute("RotY", setActorList->actorList[i].rot.y);
        entry->SetAttribute("RotZ", setActorList->actorList[i].rot.z);
        entry->SetAttribute("Params", setActorList->actorList[i].params);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogAlternateHeadersAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetAlternateHeaders> setAlternateHeaders = std::static_pointer_cast<SetAlternateHeaders>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetAlternateHeaders");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setAlternateHeaders->headerFileNames.size(); i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("Header");
        entry->SetAttribute("Path", setAlternateHeaders->headerFileNames[i].c_str());
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogCameraSettingsAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetCameraSettings> setCameraSettings = std::static_pointer_cast<SetCameraSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetCameraSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("CameraMovement", setCameraSettings->settings.cameraMovement);
    root->SetAttribute("WorldMapArea", setCameraSettings->settings.worldMapArea);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogSetCollisionHeaderAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetCollisionHeader> setCollisionHeader = std::static_pointer_cast<SetCollisionHeader>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetCollisionHeader");
    doc.InsertFirstChild(root);

    root->SetAttribute("FileName", setCollisionHeader->fileName.c_str());

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogCsCameraAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetCsCamera> setCsCamera = std::static_pointer_cast<SetCsCamera>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetCsCamera");
    doc.InsertFirstChild(root);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogCutscenesAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetCutscenes> setCutscenes = std::static_pointer_cast<SetCutscenes>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetCutscenes");
    doc.InsertFirstChild(root);

    root->SetAttribute("FileName", setCutscenes->fileName.c_str());

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogEchoSettingsAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetEchoSettings> setEchoSettings = std::static_pointer_cast<SetEchoSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetEchoSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("Echo", setEchoSettings->settings.echo);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogEntranceListAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetEntranceList> setEntranceList = std::static_pointer_cast<SetEntranceList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetEntranceList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setEntranceList->numEntrances; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("EntranceEntry");
        entry->SetAttribute("Spawn", setEntranceList->entrances[i].spawn);
        entry->SetAttribute("Room", setEntranceList->entrances[i].room);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogExitListAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetExitList> setExitList = std::static_pointer_cast<SetExitList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetExitList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setExitList->numExits; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("ExitEntry");
        entry->SetAttribute("Id", setExitList->exits[i]);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogLightingSettingsAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetLightingSettings> setLightingSettings = std::static_pointer_cast<SetLightingSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetLightingSettings");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setLightingSettings->settings.size(); i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("LightingSetting");
        entry->SetAttribute("AmbientColorR", setLightingSettings->settings[i].ambientColor[0]);
        entry->SetAttribute("AmbientColorG", setLightingSettings->settings[i].ambientColor[1]);
        entry->SetAttribute("AmbientColorB", setLightingSettings->settings[i].ambientColor[2]);

        entry->SetAttribute("Light1DirX", setLightingSettings->settings[i].light1Dir[0]);
        entry->SetAttribute("Light1DirY", setLightingSettings->settings[i].light1Dir[1]);
        entry->SetAttribute("Light1DirZ", setLightingSettings->settings[i].light1Dir[2]);
        entry->SetAttribute("Light1ColorR", setLightingSettings->settings[i].light1Color[0]);
        entry->SetAttribute("Light1ColorG", setLightingSettings->settings[i].light1Color[1]);
        entry->SetAttribute("Light1ColorB", setLightingSettings->settings[i].light1Color[2]);

        entry->SetAttribute("Light2DirX", setLightingSettings->settings[i].light2Dir[0]);
        entry->SetAttribute("Light2DirY", setLightingSettings->settings[i].light2Dir[1]);
        entry->SetAttribute("Light2DirZ", setLightingSettings->settings[i].light2Dir[2]);
        entry->SetAttribute("Light2ColorR", setLightingSettings->settings[i].light2Color[0]);
        entry->SetAttribute("Light2ColorG", setLightingSettings->settings[i].light2Color[1]);
        entry->SetAttribute("Light2ColorB", setLightingSettings->settings[i].light2Color[2]);

        entry->SetAttribute("FogColorR", setLightingSettings->settings[i].fogColor[0]);
        entry->SetAttribute("FogColorG", setLightingSettings->settings[i].fogColor[1]);
        entry->SetAttribute("FogColorB", setLightingSettings->settings[i].fogColor[2]);
        entry->SetAttribute("FogNear", setLightingSettings->settings[i].fogNear);
        entry->SetAttribute("FogFar", setLightingSettings->settings[i].fogFar);

        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogLightListAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetLightList> setLightList = std::static_pointer_cast<SetLightList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetLightList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setLightList->numLights; i++) {
        tinyxml2::XMLElement* light = doc.NewElement("LightInfo");
        light->SetAttribute("Type", setLightList->lightList[i].type);
        if (false/*setLightList->lightList[i].type == LIGHT_DIRECTIONAL*/) {
            light->SetAttribute("X", setLightList->lightList[i].params.dir.x);
            light->SetAttribute("Y", setLightList->lightList[i].params.dir.y);
            light->SetAttribute("Z", setLightList->lightList[i].params.dir.z);
            light->SetAttribute("ColorR", setLightList->lightList[i].params.dir.color[0]);
            light->SetAttribute("ColorG", setLightList->lightList[i].params.dir.color[1]);
            light->SetAttribute("ColorB", setLightList->lightList[i].params.dir.color[2]);
        } else {
            light->SetAttribute("X", setLightList->lightList[i].params.point.x);
            light->SetAttribute("Y", setLightList->lightList[i].params.point.y);
            light->SetAttribute("Z", setLightList->lightList[i].params.point.z);
            light->SetAttribute("ColorR", setLightList->lightList[i].params.point.color[0]);
            light->SetAttribute("ColorG", setLightList->lightList[i].params.point.color[1]);
            light->SetAttribute("ColorB", setLightList->lightList[i].params.point.color[2]);
            light->SetAttribute("DrawGlow", setLightList->lightList[i].params.point.drawGlow);
            light->SetAttribute("Radius", setLightList->lightList[i].params.point.radius);
        }
        root->InsertEndChild(light);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogMeshAsXML(std::shared_ptr<Ship::IResource> resource) {
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

void LogObjectListAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetObjectList> setObjectList = std::static_pointer_cast<SetObjectList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetObjectList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setObjectList->numObjects; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("ObjectEntry");
        entry->SetAttribute("Id", setObjectList->objects[i]);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogPathwaysAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetPathways> setPathways = std::static_pointer_cast<SetPathways>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetPathways");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setPathways->pathFileNames.size(); i++) {
        tinyxml2::XMLElement* pathway = doc.NewElement("Pathway");
        pathway->SetAttribute("FilePath", setPathways->pathFileNames[i].c_str());
        root->InsertEndChild(pathway);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogRoomBehaviorAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetRoomBehavior> setRoomBehavior = std::static_pointer_cast<SetRoomBehavior>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetRoomBehavior");
    doc.InsertFirstChild(root);

    root->SetAttribute("GameplayFlags1", setRoomBehavior->roomBehavior.gameplayFlags);
    root->SetAttribute("GameplayFlags2", setRoomBehavior->roomBehavior.gameplayFlags2);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogRoomListAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetRoomList> setRoomList = std::static_pointer_cast<SetRoomList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetRoomList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setRoomList->numRooms; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("RoomEntry");
        entry->SetAttribute("Path", setRoomList->fileNames[i].c_str());
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogSkyboxModifierAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetSkyboxModifier> setSkyboxModifier = std::static_pointer_cast<SetSkyboxModifier>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetSkyboxModifier");
    doc.InsertFirstChild(root);

    root->SetAttribute("SkyboxDisabled", setSkyboxModifier->modifier.skyboxDisabled);
    root->SetAttribute("SunMoonDisabled", setSkyboxModifier->modifier.sunMoonDisabled);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogSkyboxSettingsAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetSkyboxSettings> setSkyboxSettings = std::static_pointer_cast<SetSkyboxSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetSkyboxSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("Unknown", setSkyboxSettings->settings.unk);
    root->SetAttribute("SkyboxId", setSkyboxSettings->settings.skyboxId);
    root->SetAttribute("Weather", setSkyboxSettings->settings.weather);
    root->SetAttribute("Indoors", setSkyboxSettings->settings.indoors);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogSoundSettingsAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetSoundSettings> setSoundSettings = std::static_pointer_cast<SetSoundSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetSoundSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("Reverb", setSoundSettings->settings.reverb);
    root->SetAttribute("NatureAmbienceId", setSoundSettings->settings.natureAmbienceId);
    root->SetAttribute("SeqId", setSoundSettings->settings.seqId);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogSpecialObjectsAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetSpecialObjects> setSpecialObjects = std::static_pointer_cast<SetSpecialObjects>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetSpecialObjects");
    doc.InsertFirstChild(root);

    root->SetAttribute("ElfMessage", setSpecialObjects->specialObjects.elfMessage);
    root->SetAttribute("GlobalObject", setSpecialObjects->specialObjects.globalObject);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogStartPositionListAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetStartPositionList> setStartPositionList = std::static_pointer_cast<SetStartPositionList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetStartPositionList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setStartPositionList->numStartPositions; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("StartPositionEntry");
        entry->SetAttribute("Id", setStartPositionList->startPositions[i].id);
        entry->SetAttribute("PosX", setStartPositionList->startPositions[i].pos.x);
        entry->SetAttribute("PosY", setStartPositionList->startPositions[i].pos.y);
        entry->SetAttribute("PosZ", setStartPositionList->startPositions[i].pos.z);
        entry->SetAttribute("RotX", setStartPositionList->startPositions[i].rot.x);
        entry->SetAttribute("RotY", setStartPositionList->startPositions[i].rot.y);
        entry->SetAttribute("RotZ", setStartPositionList->startPositions[i].rot.z);
        entry->SetAttribute("Params", setStartPositionList->startPositions[i].params);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogTimeSettingsAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetTimeSettings> setTimeSettings = std::static_pointer_cast<SetTimeSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetTimeSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("Hour", setTimeSettings->settings.hour);
    root->SetAttribute("Minute", setTimeSettings->settings.minute);
    root->SetAttribute("TimeIncrement", setTimeSettings->settings.timeIncrement);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogTransitionActorListAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetTransitionActorList> setTransitionActorList = std::static_pointer_cast<SetTransitionActorList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetTransitionActorList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setTransitionActorList->numTransitionActors; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("TransitionActorEntry");
        entry->SetAttribute("FrontSideRoom", setTransitionActorList->transitionActorList[i].sides[0].room);
        entry->SetAttribute("FrontSideEffects", setTransitionActorList->transitionActorList[i].sides[0].effects);
        entry->SetAttribute("BackSideRoom", setTransitionActorList->transitionActorList[i].sides[1].room);
        entry->SetAttribute("BackSideEffects", setTransitionActorList->transitionActorList[i].sides[1].effects);
        entry->SetAttribute("Id", setTransitionActorList->transitionActorList[i].id);
        entry->SetAttribute("PosX", setTransitionActorList->transitionActorList[i].pos.x);
        entry->SetAttribute("PosY", setTransitionActorList->transitionActorList[i].pos.y);
        entry->SetAttribute("PosZ", setTransitionActorList->transitionActorList[i].pos.z);
        entry->SetAttribute("RotY", setTransitionActorList->transitionActorList[i].rotY);
        entry->SetAttribute("Params", setTransitionActorList->transitionActorList[i].params);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

void LogWindSettingsAsXML(std::shared_ptr<Ship::IResource> resource) {
    std::shared_ptr<SetWindSettings> setWindSettings = std::static_pointer_cast<SetWindSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetWindSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("WindWest", setWindSettings->settings.windWest);
    root->SetAttribute("WindVertical", setWindSettings->settings.windVertical);
    root->SetAttribute("WindSouth", setWindSettings->settings.windSouth);
    root->SetAttribute("WindSpeed", setWindSettings->settings.windSpeed);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}

} //namespace SOH