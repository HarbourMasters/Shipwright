#include "Exporter.h"
#include "VersionInfo.h"

void OTRExporter::WriteHeader(ZResource* res, const fs::path& outPath, BinaryWriter* writer, Ship::ResourceType resType, Ship::Version resVersion)
{
	writer->Write((uint8_t)Endianess::Little); // 0x00
	writer->Write((uint8_t)0); // 0x01
	writer->Write((uint8_t)0); // 0x02
	writer->Write((uint8_t)0); // 0x03

	writer->Write((uint32_t)resType); // 0x04
	//writer->Write((uint32_t)MAJOR_VERSION); // 0x08
	writer->Write((uint32_t)resVersion); // 0x08
	writer->Write((uint64_t)0xDEADBEEFDEADBEEF); // id, 0x0C
	writer->Write((uint32_t)resourceVersions[resType]); // 0x10
	writer->Write((uint64_t)0); // ROM CRC, 0x14
	writer->Write((uint32_t)0); // ROM Enum, 0x1C
	
	while (writer->GetBaseAddress() < 0x40)
		writer->Write((uint32_t)0); // To be used at a later date!
}
