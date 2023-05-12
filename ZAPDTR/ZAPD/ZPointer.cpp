#include "ZPointer.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Pointer, ZPointer);

ZPointer::ZPointer(ZFile* nParent) : ZResource(nParent)
{
	RegisterRequiredAttribute("Type");
}

void ZPointer::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	type = registeredAttributes.at("Type").value;
}

void ZPointer::ParseRawData()
{
	auto& rawData = parent->GetRawData();

	ptr = BitConverter::ToUInt32BE(rawData, rawDataIndex);
}

std::string ZPointer::GetBodySourceCode() const
{
	std::string ptrName;

	Globals::Instance->GetSegmentedPtrName(ptr, parent, "", ptrName);

	return ptrName;
}

bool ZPointer::DoesSupportArray() const
{
	return true;
}

std::string ZPointer::GetSourceTypeName() const
{
	return type + "*";
}

ZResourceType ZPointer::GetResourceType() const
{
	return ZResourceType::Pointer;
}

size_t ZPointer::GetRawDataSize() const
{
	return 0x04;
}
