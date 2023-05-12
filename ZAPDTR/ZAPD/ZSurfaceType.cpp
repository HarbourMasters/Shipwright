#include "ZSurfaceType.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

REGISTER_ZFILENODE(SurfaceType, ZSurfaceType);

ZSurfaceType::ZSurfaceType(ZFile* nParent) : ZResource(nParent)
{
}

ZSurfaceType::~ZSurfaceType()
{
}

void ZSurfaceType::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	data[0] = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);
	data[1] = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
}

void ZSurfaceType::DeclareReferences(const std::string& prefix)
{
	std::string declaration;
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
	                       GetSourceTypeName(), name.c_str(), GetBodySourceCode());
}

std::string ZSurfaceType::GetBodySourceCode() const
{
	return StringHelper::Sprintf("{0x%08X, 0x%08X}", data[0], data[1]);
}

std::string ZSurfaceType::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sSurfaceType_%06X", prefix.c_str(), rawDataIndex);
}

ZResourceType ZSurfaceType::GetResourceType() const
{
	return ZResourceType::SurfaceType;
}

size_t ZSurfaceType::GetRawDataSize() const
{
	return 8;
}

std::string ZSurfaceType::GetSourceTypeName() const
{
	return "SurfaceType";
}

bool ZSurfaceType::DoesSupportArray() const
{
	return true;
}
