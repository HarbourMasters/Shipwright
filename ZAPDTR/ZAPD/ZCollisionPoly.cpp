#include "ZCollisionPoly.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

REGISTER_ZFILENODE(CollisionPoly, ZCollisionPoly);

ZCollisionPoly::ZCollisionPoly(ZFile* nParent) : ZResource(nParent)
{
}

ZCollisionPoly::~ZCollisionPoly()
{
}

void ZCollisionPoly::ParseRawData()
{
	const auto& rawData = parent->GetRawData();
	type = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);

	vtxA = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	vtxB = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
	vtxC = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);

	normX = BitConverter::ToUInt16BE(rawData, rawDataIndex + 8);
	normY = BitConverter::ToUInt16BE(rawData, rawDataIndex + 10);
	normZ = BitConverter::ToUInt16BE(rawData, rawDataIndex + 12);

	dist = BitConverter::ToUInt16BE(rawData, rawDataIndex + 14);
}

void ZCollisionPoly::DeclareReferences(const std::string& prefix)
{
	std::string declaration;
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
	                       GetSourceTypeName(), name.c_str(), GetBodySourceCode());
}

std::string ZCollisionPoly::GetBodySourceCode() const
{
	std::string declaration;

	declaration +=
		StringHelper::Sprintf("{0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X}",
	                          type, vtxA, vtxB, vtxC, normX, normY, normZ, dist);
	return declaration;
}

std::string ZCollisionPoly::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sCollisionPoly_%06X", prefix.c_str(), rawDataIndex);
}

ZResourceType ZCollisionPoly::GetResourceType() const
{
	return ZResourceType::CollisionPoly;
}

size_t ZCollisionPoly::GetRawDataSize() const
{
	return 16;
}

std::string ZCollisionPoly::GetSourceTypeName() const
{
	return "CollisionPoly";
}

bool ZCollisionPoly::DoesSupportArray() const
{
	return true;
}