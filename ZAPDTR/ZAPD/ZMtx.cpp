#include "ZMtx.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Mtx, ZMtx);

ZMtx::ZMtx(ZFile* nParent) : ZResource(nParent)
{
}

void ZMtx::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();
	for (size_t i = 0; i < 4; ++i)
		for (size_t j = 0; j < 4; ++j)
			mtx[i][j] = BitConverter::ToInt32BE(rawData, rawDataIndex + (i * 4 + j) * 4);
}

size_t ZMtx::GetRawDataSize() const
{
	return 64;
}

std::string ZMtx::GetBodySourceCode() const
{
	std::string bodyStr = "\n";

	for (const auto& row : mtx)
	{
		bodyStr += "    ";

		for (int32_t val : row)
			bodyStr += StringHelper::Sprintf("%-11i, ", val);

		bodyStr += "\n";
	}

	return bodyStr;
}

std::string ZMtx::GetSourceTypeName() const
{
	return "Mtx";
}

ZResourceType ZMtx::GetResourceType() const
{
	return ZResourceType::Mtx;
}

DeclarationAlignment ZMtx::GetDeclarationAlignment() const
{
	return DeclarationAlignment::Align8;
}
