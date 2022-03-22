#include "ZString.h"

#include "Utils/File.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(String, ZString);

ZString::ZString(ZFile* nParent) : ZResource(nParent)
{
}

void ZString::ParseRawData()
{
	size_t size = 0;
	const auto& rawData = parent->GetRawData();
	const auto& rawDataArr = rawData.data();
	size_t rawDataSize = rawData.size();
	for (size_t i = rawDataIndex; i < rawDataSize; ++i)
	{
		++size;
		if (rawDataArr[i] == '\0')
		{
			break;
		}
	}

	auto dataStart = rawData.begin() + rawDataIndex;
	strData.assign(dataStart, dataStart + size);
}

Declaration* ZString::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), auxName, 0, bodyStr);
	decl->staticConf = staticConf;
	return decl;
}

std::string ZString::GetBodySourceCode() const
{
	return StringHelper::Sprintf("\t\"%s\"", strData.data());
}

std::string ZString::GetSourceOutputHeader([[maybe_unused]] const std::string& prefix)
{
	return StringHelper::Sprintf("#define %s_macro \"%s\"", name.c_str(), strData.data());
}

std::string ZString::GetSourceTypeName() const
{
	return "char";
}

ZResourceType ZString::GetResourceType() const
{
	return ZResourceType::String;
}

size_t ZString::GetRawDataSize() const
{
	return strData.size();
}
