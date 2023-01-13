#include "soh/resource/importer/scenecommand/SetMeshFactory.h"
#include "soh/resource/type/scenecommand/SetMesh.h"
#include "spdlog/spdlog.h"
#include "libultraship/bridge.h"

namespace Ship {
std::shared_ptr<Resource> SetMeshFactory::ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetMesh>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	switch (version)
	{
	case 0:
		factory = std::make_shared<SetMeshFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetMesh with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetMeshFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetMesh> setMesh = std::static_pointer_cast<SetMesh>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setMesh);

	ReadCommandId(setMesh, reader);
	
    setMesh->data = reader->ReadInt8();

	setMesh->meshHeader.base.type = reader->ReadInt8();
    setMesh->meshHeader.polygon0.num = 1;

    if (setMesh->meshHeader.base.type != 1) {
        setMesh->meshHeader.polygon0.num = reader->ReadInt8();
    }

	if (setMesh->meshHeader.base.type == 2) {
		setMesh->dlists2.reserve(setMesh->meshHeader.polygon0.num);
	} else {
		setMesh->dlists.reserve(setMesh->meshHeader.polygon0.num);
	}

	for (int32_t i = 0; i < setMesh->meshHeader.polygon0.num; i++) {
		if (setMesh->meshHeader.base.type == 0) {
			PolygonDlist dlist;

			int32_t polyType = reader->ReadInt8(); // Unused
			std::string meshOpa = reader->ReadString();
			std::string meshXlu = reader->ReadString();

			dlist.opa = meshOpa != "" ? (Gfx*)GetResourceDataByName(meshOpa.c_str(), true) : 0;
			dlist.xlu = meshXlu != "" ? (Gfx*)GetResourceDataByName(meshXlu.c_str(), true) : 0;

			setMesh->dlists.push_back(dlist);
		} else if (setMesh->meshHeader.base.type == 1) {
			PolygonDlist pType;

			setMesh->meshHeader.polygon1.format = reader->ReadUByte();
			std::string imgOpa = reader->ReadString();
			std::string imgXlu = reader->ReadString();

			pType.opa = imgOpa != "" ? (Gfx*)GetResourceDataByName(imgOpa.c_str(), true) : 0;
			pType.xlu = imgXlu != "" ? (Gfx*)GetResourceDataByName(imgXlu.c_str(), true) : 0;

			uint32_t imageCount = reader->ReadUInt32();
			setMesh->images.reserve(imageCount);

			for (int32_t i = 0; i < imageCount; i++) {
				BgImage image;
				image.unk_00 = reader->ReadUInt16();
				image.id = reader->ReadUByte();
				image.source = GetResourceDataByName(reader->ReadString().c_str(), true);
				image.unk_0C = reader->ReadUInt32();
				image.tlut = reader->ReadUInt32();
				image.width = reader->ReadUInt16();
				image.height = reader->ReadUInt16();
				image.fmt = reader->ReadUByte();
				image.siz = reader->ReadUInt16();
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

			int32_t polyType = reader->ReadInt8(); // Unused??

			setMesh->meshHeader.polygon1.multi.list = setMesh->images.data();

			std::string meshOpa = reader->ReadString();
			std::string meshXlu = reader->ReadString();

			pType.opa = meshOpa != "" ? (Gfx*)GetResourceDataByName(meshOpa.c_str(), true) : 0;
			pType.xlu = meshXlu != "" ? (Gfx*)GetResourceDataByName(meshXlu.c_str(), true) : 0;

			setMesh->dlists.push_back(pType);
			setMesh->meshHeader.polygon1.dlist = (Gfx*)&setMesh->dlists.back();
		} else if (setMesh->meshHeader.base.type == 2) {
			PolygonDlist2 dlist;
			
			int32_t polyType = reader->ReadInt8(); // Unused
			dlist.pos.x = reader->ReadInt16();
			dlist.pos.y = reader->ReadInt16();
			dlist.pos.z = reader->ReadInt16();
			dlist.unk_06 = reader->ReadInt16();

			std::string meshOpa = reader->ReadString();
			std::string meshXlu = reader->ReadString();

			dlist.opa = meshOpa != "" ? (Gfx*)GetResourceDataByName(meshOpa.c_str(), true) : 0;
			dlist.xlu = meshXlu != "" ? (Gfx*)GetResourceDataByName(meshXlu.c_str(), true) : 0;

			setMesh->dlists2.push_back(dlist);
		} else {
			SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}", setMesh->meshHeader.base.type);
		}
	}

	if (setMesh->meshHeader.base.type == 2) {
		setMesh->meshHeader.polygon2.start = setMesh->dlists2.data();
	} else {
		setMesh->meshHeader.polygon0.start = setMesh->dlists.data();
	}
}

} // namespace Ship
