#include "ZPlayerAnimationData.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(PlayerAnimationData, ZPlayerAnimationData);

ZPlayerAnimationData::ZPlayerAnimationData(ZFile* nParent) : ZResource(nParent)
{
	RegisterRequiredAttribute("FrameCount");
}

void ZPlayerAnimationData::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	std::string& frameCountXml = registeredAttributes.at("FrameCount").value;

	frameCount = StringHelper::StrToL(frameCountXml);
}

void ZPlayerAnimationData::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();

	size_t totalSize = GetRawDataSize();
	// Divided by 2 because each value is an s16
	limbRotData.reserve(totalSize * frameCount / 2);

	for (size_t i = 0; i < totalSize; i += 2)
	{
		limbRotData.push_back(BitConverter::ToUInt16BE(rawData, rawDataIndex + i));
	}
}

Declaration* ZPlayerAnimationData::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (auxName == "")
		auxName = GetDefaultName(prefix);

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), name, limbRotData.size(), bodyStr);
	decl->staticConf = staticConf;
	return decl;
}

std::string ZPlayerAnimationData::GetBodySourceCode() const
{
	std::string declaration = "";

	size_t index = 0;
	for (const auto& entry : limbRotData)
	{
		if (index % 8 == 0)
		{
			declaration += "\t";
		}

		declaration += StringHelper::Sprintf("0x%04X, ", entry);

		if ((index + 1) % 8 == 0)
		{
			declaration += "\n";
		}

		index++;
	}

	return declaration;
}

std::string ZPlayerAnimationData::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sPlayerAnimationData_%06X", prefix.c_str(), rawDataIndex);
}

std::string ZPlayerAnimationData::GetSourceTypeName() const
{
	return "s16";
}

ZResourceType ZPlayerAnimationData::GetResourceType() const
{
	return ZResourceType::PlayerAnimationData;
}

size_t ZPlayerAnimationData::GetRawDataSize() const
{
	// (sizeof(Vec3s) * limbCount + 2) * frameCount
	return (6 * 22 + 2) * frameCount;
}
