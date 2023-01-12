#include "resource/importer/SetMeshFactory.h"
#include "resource/type/SetMesh.h"
#include "spdlog/spdlog.h"
#include "resourcebridge.h"

namespace Ship {
std::shared_ptr<Resource> SetMeshFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetMesh>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
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
	ResourceVersionFactory::ParseFileBinary(reader, text);

	ReadCommandId(setMesh, reader);
	
    setMesh->data = reader->ReadInt8();

	setMesh->meshHeader->base.type = reader->ReadInt8();
    setMesh->meshHeader->polygon0.num = 1;

    if (setMesh->meshHeaderType != 1) {
        setMesh->meshHeader->polygon0.num = reader->ReadInt8();
    }

	if (setMesh->meshHeaderType == 2) {
		setMesh->dlists2.reserve(setMesh->numPoly);
		setMesh->meshHeader->polygon0.start = setMesh->dlists2.data();
	} else {
		setMesh->dlists.reserve(setMesh->numPoly);
		setMesh->meshHeader->polygon0.start = setMesh->dlists.data();
	}

	for (int32_t i = 0; i < setMesh->meshHeader->polygon0.num; i++) {
		if (setMesh->meshHeader->base.type == 0) {
			PolygonDlist dlist;

			int32_t polyType = reader->ReadInt8(); // Unused
			std::string meshOpa = reader->ReadString();
			std::string meshXlu = reader->ReadString();

			dlist.opa = meshOpa != "" ? (Gfx*)GetResourceDataByName(meshOpa.c_str()) : 0;
			dlist.xlu = meshXlu != "" ? (Gfx*)GetResourceDataByName(meshXlu.c_str()) : 0;

			setMesh->dlists.push_back(dlist);
			setMesh->meshHeader->polygon0.start = &dlists.back();
		} else if (setMesh->meshHeader->base.type == 1) {
			PolygonDlist pType;

			setMesh->meshHeader->polygon1.format = reader->ReadUByte();
			std::string imgOpa = reader->ReadString();
			std::string imgXlu = reader->ReadString();

			pType->opa = imgOpa != "" ? (Gfx*)GetResourceDataByName(imgOpa.c_str()) : 0;
			pType->xlu = imgXlu != "" ? (Gfx*)GetResourceDataByName(imgXlu.c_str()) : 0;

			uint32_t imageCount = reader->ReadUint32();
			setMesh->images.reserve(imageCount);

			for (int32_t i = 0; i < imageCount; i++) {
				BgImage image;
				image.unk_00 = reader->ReadUin16();
				image.id = reader->ReadUByte();
				image.source = GetResourceDataByName(reader->ReadString());
				image.unk_0C = reader->ReadUInt32();
				image.tlut = reader->ReadUInt32();
				image.width = reader->ReadUInt16();
				image.height = reader->ReadUInt16();
				image.fmt = reader->ReadUByte();
				image.siz = reader->ReadUInt16();
				image.mode0 = reader->ReadUInt16();
				image.tlutCount = reader->ReadUInt16();

				if (setMesh->meshHeader->polygon1.format == 1) {
					play->roomCtx.curRoom.meshHeader->polygon1.single = image;
				} else {
					images.push_back(image);
				}
			}

			int32_t polyType = reader->ReadInt8(); // Unused??

			play->roomCtx.curRoom.meshHeader->polygon1.multi.list = images.data();

			std::string meshOpa = reader->ReadString();
			std::string meshXlu = reader->ReadString();

			pType.opa = meshOpa != "" ? (Gfx*)GetResourceDataByName(meshOpa.c_str()) : 0;
			pType.xlu = meshXlu != "" ? (Gfx*)GetResourceDataByName(meshXlu.c_str()) : 0;

			setMesh->dlists.push_back(pType);
			setMesh->meshHeader->polygon1.dlist = (Gfx*)&dlists.back();
		} else if (setMesh->meshHeader->base.type == 2) {
			PolygonDlist2 dlist;
			
			int32_t polyType = reader->ReadInt8(); // Unused
			dlist.pos.x = reader->ReadInt16();
			dlist.pos.y = reader->ReadInt16();
			dlist.pos.z = reader->ReadInt16();
			dlist.unk_06 = reader->ReadInt16();

			std::string meshOpa = reader->ReadString();
			std::string meshXlu = reader->ReadString();

			dlist.opa = meshOpa != "" ? (Gfx*)GetResourceDataByName(meshOpa.c_str()) : 0;
			dlist.xlu = meshXlu != "" ? (Gfx*)GetResourceDataByName(meshXlu.c_str()) : 0;

			setMesh->dlists2.push_back(dlist);
			setMesh->meshHeader->polygon0.start = &dlists2.back();
		} else {
			SPDLOG_ERROR("Tried to load mesh in SetMesh scene header with type that doesn't exist: {}", setMesh->meshHeader->base.type);
		}
	}
}

} // namespace Ship
