#include "Exporter.h"
#include "VersionInfo.h"

void OTRExporter::WriteHeader(ZResource* res, BinaryWriter* writer, Ship::ResourceType resType, Ship::Version majorVersion, bool writeFullHeader)
{
	if (writeFullHeader)
	{
		writer->Write((uint8_t)Endianess::Little); // 0x00
		writer->Write((uint8_t)0); // 0x01
		writer->Write((uint8_t)0); // 0x02
		writer->Write((uint8_t)0); // 0x03

		writer->Write((uint32_t)resType); // 0x04
	}

	//writer->Write((uint32_t)MAJOR_VERSION); // 0x08
	writer->Write((uint32_t)majorVersion); // 0x08

	if (writeFullHeader)
	{
		writer->Write((uint64_t)0xDEADBEEFDEADBEEF); // id, 0x0C
		writer->Write((uint32_t)resourceVersions[resType]); // 0x10
		writer->Write((uint64_t)0); // ROM CRC, 0x14
		writer->Write((uint32_t)0); // ROM Enum, 0x1C

		while (writer->GetBaseAddress() < 0x40)
			writer->Write((uint32_t)0); // To be used at a later date!
	}
}

Ship::ResourceType OTRExporter::GetResourceType(ZResourceType zResType)
{
	switch (zResType)
	{
	case ZResourceType::Animation:				return Ship::ResourceType::Animation;
	case ZResourceType::Array:					return Ship::ResourceType::Array;
	case ZResourceType::Background:				return Ship::ResourceType::Blob;
	case ZResourceType::Blob:					return Ship::ResourceType::Blob;
	case ZResourceType::CollisionHeader:		return Ship::ResourceType::CollisionHeader;
	case ZResourceType::Cutscene:				return Ship::ResourceType::Cutscene;
	case ZResourceType::DisplayList:			return Ship::ResourceType::DisplayList;
	case ZResourceType::Limb:					return Ship::ResourceType::SkeletonLimb;
	case ZResourceType::Mtx:					return Ship::ResourceType::Matrix;
	case ZResourceType::Path:					return Ship::ResourceType::Path;
	case ZResourceType::PlayerAnimationData:	return Ship::ResourceType::PlayerAnimation;
	case ZResourceType::Room:					return Ship::ResourceType::Room;
	case ZResourceType::Scene:					return Ship::ResourceType::Room;
	case ZResourceType::Skeleton:				return Ship::ResourceType::Skeleton;
	case ZResourceType::Text:					return Ship::ResourceType::Text;
	case ZResourceType::Texture:				return Ship::ResourceType::Texture;
	case ZResourceType::Vector:					return Ship::ResourceType::Vector;
	case ZResourceType::Vertex:					return Ship::ResourceType::Vertex;
	}
}
