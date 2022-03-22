#include "ZPath.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Path, ZPath);

ZPath::ZPath(ZFile* nParent) : ZResource(nParent)
{
	numPaths = 1;
	RegisterOptionalAttribute("NumPaths", "1");
}

void ZPath::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	numPaths = StringHelper::StrToL(registeredAttributes.at("NumPaths").value);

	if (numPaths < 1)
	{
		HANDLE_ERROR_RESOURCE(
			WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
			StringHelper::Sprintf("invalid value '%d' found for 'NumPaths' attribute", numPaths),
			"Should be at least '1'");
	}
}

void ZPath::ParseRawData()
{
	ZResource::ParseRawData();

	uint32_t currentPtr = rawDataIndex;

	for (size_t pathIndex = 0; pathIndex < numPaths; pathIndex++)
	{
		PathwayEntry path(parent);
		path.ExtractFromFile(currentPtr);

		if (path.GetListAddress() == 0)
			break;

		currentPtr += path.GetRawDataSize();
		pathways.push_back(path);
	}
}

void ZPath::DeclareReferences(const std::string& prefix)
{
	ZResource::DeclareReferences(prefix);

	for (auto& entry : pathways)
		entry.DeclareReferences(prefix);
}

Declaration* ZPath::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), name, pathways.size(), bodyStr);
	decl->staticConf = staticConf;
	return decl;
}

std::string ZPath::GetBodySourceCode() const
{
	std::string declaration;

	size_t index = 0;
	for (const auto& entry : pathways)
	{
		declaration += StringHelper::Sprintf("\t{ %s },", entry.GetBodySourceCode().c_str());

		if (index < pathways.size() - 1)
			declaration += "\n";

		index++;
	}

	return declaration;
}

std::string ZPath::GetSourceTypeName() const
{
	return "Path";
}

ZResourceType ZPath::GetResourceType() const
{
	return ZResourceType::Path;
}

size_t ZPath::GetRawDataSize() const
{
	return pathways.size() * pathways.at(0).GetRawDataSize();
}

void ZPath::SetNumPaths(uint32_t nNumPaths)
{
	numPaths = nNumPaths;
}

/* PathwayEntry */

PathwayEntry::PathwayEntry(ZFile* nParent) : ZResource(nParent)
{
}

void PathwayEntry::ParseRawData()
{
	ZResource::ParseRawData();
	auto parentRawData = parent->GetRawData();
	numPoints = parentRawData.at(rawDataIndex + 0);
	unk1 = parentRawData.at(rawDataIndex + 1);
	unk2 = BitConverter::ToInt16BE(parentRawData, rawDataIndex + 2);
	listSegmentAddress = BitConverter::ToInt32BE(parentRawData, rawDataIndex + 4);

	uint32_t currentPtr = GETSEGOFFSET(listSegmentAddress);

	for (int32_t i = 0; i < numPoints; i++)
	{
		ZVector vec(parent);
		vec.ExtractFromBinary(currentPtr, ZScalarType::ZSCALAR_S16, 3);

		currentPtr += vec.GetRawDataSize();
		points.push_back(vec);
	}
}

void PathwayEntry::DeclareReferences(const std::string& prefix)
{
	ZResource::DeclareReferences(prefix);
	if (points.empty())
		return;

	std::string pointsName;
	bool addressFound =
		Globals::Instance->GetSegmentedPtrName(listSegmentAddress, parent, "Vec3s", pointsName);
	if (addressFound)
		return;

	std::string declaration = "";

	size_t index = 0;
	for (const auto& point : points)
	{
		declaration += StringHelper::Sprintf("\t{ %s },", point.GetBodySourceCode().c_str());

		if (index < points.size() - 1)
			declaration += "\n";

		index++;
	}

	uint32_t pointsOffset = Seg2Filespace(listSegmentAddress, parent->baseAddress);
	Declaration* decl = parent->GetDeclaration(pointsOffset);
	if (decl == nullptr)
	{
		pointsName = StringHelper::Sprintf("%sPathwayList_%06X", prefix.c_str(), pointsOffset);
		parent->AddDeclarationArray(pointsOffset, points.at(0).GetDeclarationAlignment(),
		                            points.size() * 6, points.at(0).GetSourceTypeName(), pointsName,
		                            points.size(), declaration);
	}
	else
		decl->text = declaration;
}

std::string PathwayEntry::GetBodySourceCode() const
{
	std::string declaration;
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(listSegmentAddress, parent, "Vec3s", listName);

	if (Globals::Instance->game == ZGame::MM_RETAIL)
		declaration +=
			StringHelper::Sprintf("%i, %i, %i, %s", numPoints, unk1, unk2, listName.c_str());
	else
		declaration += StringHelper::Sprintf("%i, %s", numPoints, listName.c_str());

	return declaration;
}

std::string PathwayEntry::GetSourceTypeName() const
{
	return "Path";
}

ZResourceType PathwayEntry::GetResourceType() const
{
	return ZResourceType::Path;
}

size_t PathwayEntry::GetRawDataSize() const
{
	return 0x08;
}

segptr_t PathwayEntry::GetListAddress() const
{
	return listSegmentAddress;
}
