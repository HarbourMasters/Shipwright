#include "ZVtx.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Vtx, ZVtx);

ZVtx::ZVtx(ZFile* nParent) : ZResource(nParent)
{
	x = 0;
	y = 0;
	z = 0;
	flag = 0;
	s = 0;
	t = 0;
	r = 0;
	g = 0;
	b = 0;
	a = 0;
}

void ZVtx::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();
	x = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	y = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	z = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	flag = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	s = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	t = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
	r = rawData[rawDataIndex + 12];
	g = rawData[rawDataIndex + 13];
	b = rawData[rawDataIndex + 14];
	a = rawData[rawDataIndex + 15];
}

Declaration* ZVtx::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	Declaration* decl = ZResource::DeclareVar(prefix, bodyStr);
	decl->isExternal = true;
	return decl;
}

std::string ZVtx::GetBodySourceCode() const
{
	return StringHelper::Sprintf("VTX(%i, %i, %i, %i, %i, %i, %i, %i, %i)", x, y, z, s, t, r, g, b,
	                             a);
}

size_t ZVtx::GetRawDataSize() const
{
	return 16;
}

bool ZVtx::DoesSupportArray() const
{
	return true;
}

ZResourceType ZVtx::GetResourceType() const
{
	return ZResourceType::Vertex;
}

bool ZVtx::IsExternalResource() const
{
	return true;
}

std::string ZVtx::GetSourceTypeName() const
{
	return "Vtx";
}

std::string ZVtx::GetExternalExtension() const
{
	return "vtx";
}

DeclarationAlignment ZVtx::GetDeclarationAlignment() const
{
	return DeclarationAlignment::Align8;
}
