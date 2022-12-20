#include "ZSkeleton.h"

#include <cassert>

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"

REGISTER_ZFILENODE(Skeleton, ZSkeleton);
REGISTER_ZFILENODE(LimbTable, ZLimbTable);

ZSkeleton::ZSkeleton(ZFile* nParent) : ZResource(nParent)
{
	RegisterRequiredAttribute("Type");
	RegisterRequiredAttribute("LimbType");
	RegisterOptionalAttribute("EnumName");
	RegisterOptionalAttribute("LimbNone");
	RegisterOptionalAttribute("LimbMax");

	genOTRDef = true;
}

void ZSkeleton::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	std::string skelTypeXml = registeredAttributes.at("Type").value;

	if (skelTypeXml == "Flex")
		type = ZSkeletonType::Flex;
	else if (skelTypeXml == "Curve")
		type = ZSkeletonType::Curve;
	else if (skelTypeXml != "Normal")
	{
		HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
		                      "invalid value found for 'Type' attribute", "");
	}

	std::string limbTypeXml = registeredAttributes.at("LimbType").value;
	limbType = ZLimb::GetTypeByAttributeName(limbTypeXml);
	if (limbType == ZLimbType::Invalid)
	{
		HANDLE_ERROR_RESOURCE(
			WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
			StringHelper::Sprintf("invalid value '%s' found for 'LimbType' attribute",
		                          limbTypeXml.c_str()),
			"Defaulting to 'Standard'.");
	}

	enumName = registeredAttributes.at("EnumName").value;
	limbNoneName = registeredAttributes.at("LimbNone").value;
	limbMaxName = registeredAttributes.at("LimbMax").value;

	if (enumName != "")
	{
		if (limbNoneName == "" || limbMaxName == "")
		{
			HANDLE_ERROR_RESOURCE(WarningType::MissingAttribute, parent, this, rawDataIndex,
			                      "'EnumName' attribute was used but either 'LimbNone' or "
			                      "'LimbMax' attribute is missing",
			                      "");
		}
	}

	if (limbNoneName != "")
	{
		if (limbMaxName == "")
		{
			HANDLE_ERROR_RESOURCE(
				WarningType::MissingAttribute, parent, this, rawDataIndex,
				"'LimbNone' attribute was used but 'LimbMax' attribute is missing", "");
		}
	}

	if (limbMaxName != "")
	{
		if (limbNoneName == "")
		{
			HANDLE_ERROR_RESOURCE(
				WarningType::MissingAttribute, parent, this, rawDataIndex,
				"'LimbMax' attribute was used but 'LimbNone' attribute is missing", "");
		}
	}
}

void ZSkeleton::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();
	limbsArrayAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex);
	limbCount = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4);

	if (type == ZSkeletonType::Flex)
	{
		dListCount = BitConverter::ToUInt8BE(rawData, rawDataIndex + 8);
	}
}

void ZSkeleton::DeclareReferences(const std::string& prefix)
{
	std::string defaultPrefix = name;
	if (defaultPrefix == "")
		defaultPrefix = prefix;

	ZResource::DeclareReferences(defaultPrefix);

	if (limbsArrayAddress != SEGMENTED_NULL && GETSEGNUM(limbsArrayAddress) == parent->segment)
	{
		offset_t ptr = Seg2Filespace(limbsArrayAddress, parent->baseAddress);

		if (!parent->HasDeclaration(ptr))
		{
			limbsTable = new ZLimbTable(parent);
			limbsTable->ExtractFromBinary(ptr, limbType, limbCount);
			limbsTable->SetName(StringHelper::Sprintf("%sLimbs", defaultPrefix.c_str()));
			parent->AddResource(limbsTable);
		}
		else
		{
			limbsTable = static_cast<ZLimbTable*>(parent->FindResource(ptr));
		}

		if (limbsTable->enumName == "")
		{
			limbsTable->enumName = enumName;
		}
		if (limbsTable->limbNoneName == "")
		{
			limbsTable->limbNoneName = limbNoneName;
		}
		if (limbsTable->limbMaxName == "")
		{
			limbsTable->limbMaxName = limbMaxName;
		}
	}
}

std::string ZSkeleton::GetBodySourceCode() const
{
	std::string limbArrayName;
	Globals::Instance->GetSegmentedPtrName(limbsArrayAddress, parent, "", limbArrayName,
	                                       parent->workerID);

	std::string countStr;
	assert(limbsTable != nullptr);
	// There are some Skeletons with the wrong limb count on them, so this check is necessary.
	if (limbsTable->count == limbCount)
	{
		countStr = StringHelper::Sprintf("ARRAY_COUNT(%s)", limbArrayName.c_str());
	}
	else
	{
		countStr = StringHelper::Sprintf("%i", limbCount);
	}

	switch (type)
	{
	case ZSkeletonType::Normal:
	case ZSkeletonType::Curve:
		return StringHelper::Sprintf("\n\t%s, %s\n", limbArrayName.c_str(), countStr.c_str());

	case ZSkeletonType::Flex:
		return StringHelper::Sprintf("\n\t{ %s, %s }, %i\n", limbArrayName.c_str(),
		                             countStr.c_str(), dListCount);
	}

	// TODO: Throw exception?
	return "ERROR";
}

size_t ZSkeleton::GetRawDataSize() const
{
	switch (type)
	{
	case ZSkeletonType::Flex:
		return 0xC;
	case ZSkeletonType::Normal:
	case ZSkeletonType::Curve:
	default:
		return 0x8;
	}
}

std::string ZSkeleton::GetSourceTypeName() const
{
	switch (type)
	{
	case ZSkeletonType::Normal:
		return "SkeletonHeader";
	case ZSkeletonType::Flex:
		return "FlexSkeletonHeader";
	case ZSkeletonType::Curve:
		return "SkelCurveLimbList";
	}

	return "SkeletonHeader";
}

ZResourceType ZSkeleton::GetResourceType() const
{
	return ZResourceType::Skeleton;
}

DeclarationAlignment ZSkeleton::GetDeclarationAlignment() const
{
	return DeclarationAlignment::Align4;
}

uint8_t ZSkeleton::GetLimbCount()
{
	return limbCount;
}

/* ZLimbTable */

ZLimbTable::ZLimbTable(ZFile* nParent) : ZResource(nParent)
{
	RegisterRequiredAttribute("LimbType");
	RegisterRequiredAttribute("Count");
	RegisterOptionalAttribute("EnumName");
	RegisterOptionalAttribute("LimbNone");
	RegisterOptionalAttribute("LimbMax");
}

void ZLimbTable::ExtractFromBinary(uint32_t nRawDataIndex, ZLimbType nLimbType, size_t nCount)
{
	rawDataIndex = nRawDataIndex;
	limbType = nLimbType;
	count = nCount;

	ParseRawData();
}

void ZLimbTable::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	std::string limbTypeXml = registeredAttributes.at("LimbType").value;
	limbType = ZLimb::GetTypeByAttributeName(limbTypeXml);
	if (limbType == ZLimbType::Invalid)
	{
		HANDLE_WARNING_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
		                        "invalid value found for 'LimbType' attribute.",
		                        "Defaulting to 'Standard'.");
		limbType = ZLimbType::Standard;
	}

	count = StringHelper::StrToL(registeredAttributes.at("Count").value);

	enumName = registeredAttributes.at("EnumName").value;
	limbNoneName = registeredAttributes.at("LimbNone").value;
	limbMaxName = registeredAttributes.at("LimbMax").value;

	if (enumName != "")
	{
		if (limbNoneName == "" || limbMaxName == "")
		{
			HANDLE_ERROR_RESOURCE(
				WarningType::MissingAttribute, parent, this, rawDataIndex,
				"'EnumName' attribute was used but 'LimbNone'/'LimbMax' attributes is missing", "");
		}
	}

	if (limbNoneName != "")
	{
		if (limbMaxName == "")
		{
			HANDLE_ERROR_RESOURCE(
				WarningType::MissingAttribute, parent, this, rawDataIndex,
				"'LimbNone' attribute was used but 'LimbMax' attribute is missing", "");
		}
	}

	if (limbMaxName != "")
	{
		if (limbNoneName == "")
		{
			HANDLE_ERROR_RESOURCE(
				WarningType::MissingAttribute, parent, this, rawDataIndex,
				"'LimbMax' attribute was used but 'LimbNone' attribute is missing", "");
		}
	}
}

void ZLimbTable::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();
	uint32_t ptr = rawDataIndex;

	limbsAddresses.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		limbsAddresses.push_back(BitConverter::ToUInt32BE(rawData, ptr));
		ptr += 4;
	}
}

void ZLimbTable::DeclareReferences(const std::string& prefix)
{
	std::string varPrefix = name;
	if (varPrefix == "")
		varPrefix = prefix;

	ZResource::DeclareReferences(varPrefix);
	limbsReferences.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		segptr_t limbAddress = limbsAddresses[i];

		if (limbAddress != 0 && GETSEGNUM(limbAddress) == parent->segment)
		{
			uint32_t limbOffset = Seg2Filespace(limbAddress, parent->baseAddress);
			ZLimb* limb;

			if (!parent->HasDeclaration(limbOffset))
			{
				limb = new ZLimb(parent);
				limb->ExtractFromBinary(limbOffset, limbType);
				limb->SetName(limb->GetDefaultName(varPrefix));
				limb->DeclareVar(varPrefix, "");
				limb->DeclareReferences(varPrefix);
				parent->AddResource(limb);
			}
			else
			{
				limb = static_cast<ZLimb*>(parent->FindResource(limbOffset));
				assert(limb != nullptr);
				assert(limb->GetResourceType() == ZResourceType::Limb);
			}

			limb->limbsTable = this;
			limb->SetLimbIndex(i + 1);

			limbsReferences.push_back(limb);
		}
	}
}

Declaration* ZLimbTable::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), auxName, limbsAddresses.size(), bodyStr);
	decl->staticConf = staticConf;
	return decl;
}

std::string ZLimbTable::GetBodySourceCode() const
{
	std::string body;

	for (size_t i = 0; i < count; i++)
	{
		std::string limbName;
		Globals::Instance->GetSegmentedPtrName(limbsAddresses[i], parent, "", limbName,
		                                       parent->workerID);
		body += StringHelper::Sprintf("\t%s,", limbName.c_str());

		auto& limb = limbsReferences.at(i);
		std::string limbEnumName = limb->enumName;
		if (limbEnumName != "")
		{
			body += StringHelper::Sprintf(" /* %s */", limbEnumName.c_str());
		}

		if (i + 1 < count)
			body += "\n";
	}

	return body;
}

std::string ZLimbTable::GetSourceOutputHeader([[maybe_unused]] const std::string& prefix)
{
	if (limbNoneName == "" || limbMaxName == "" || enumName == "")
	{
		// Don't produce a enum of any of those attributes is missing
		return "";
	}

	std::string limbEnum = StringHelper::Sprintf("typedef enum %s {\n", enumName.c_str());

	// This assumes there isn't any skeleton with more than 0x100 limbs

	limbEnum += StringHelper::Sprintf("    /* 0x00 */ %s,\n", limbNoneName.c_str());

	size_t i = 0;
	for (; i < count; i++)
	{
		auto& limb = limbsReferences.at(i);
		std::string limbEnumName = limb->enumName;

		if (limbEnumName == "")
		{
			HANDLE_ERROR_RESOURCE(
				WarningType::MissingAttribute, parent, this, rawDataIndex,
				"Skeleton's enum attributes were used but at least one limb is missing its "
				"'LimbName' attribute",
				StringHelper::Sprintf("When processing limb %02i, named '%s' at offset '0x%X'",
			                          i + 1, limb->GetName().c_str(), limb->GetRawDataIndex()));
		}

		limbEnum += StringHelper::Sprintf("    /* 0x%02X */ %s,\n", i + 1, limbEnumName.c_str());
	}

	limbEnum += StringHelper::Sprintf("    /* 0x%02X */ %s\n", i + 1, limbMaxName.c_str());

	limbEnum += StringHelper::Sprintf("} %s;\n", enumName.c_str());

	return limbEnum;
}

std::string ZLimbTable::GetSourceTypeName() const
{
	switch (limbType)
	{
	case ZLimbType::Standard:
	case ZLimbType::LOD:
	case ZLimbType::Skin:
		return "void*";

	case ZLimbType::Curve:
	case ZLimbType::Legacy:
		return StringHelper::Sprintf("%s*", ZLimb::GetSourceTypeName(limbType));

	case ZLimbType::Invalid:
		// TODO: Proper error message or something.
		assert("Invalid limb type.\n");
	}

	return "ERROR";
}

ZResourceType ZLimbTable::GetResourceType() const
{
	return ZResourceType::LimbTable;
}

size_t ZLimbTable::GetRawDataSize() const
{
	return 4 * limbsAddresses.size();
}

std::string ZLimbTable::GetLimbEnumName(uint8_t limbIndex) const
{
	if (limbIndex == 0xFF)
	{
		return "LIMB_DONE";
	}

	if (limbIndex < count)
	{
		std::string limbEnumName = limbsReferences.at(limbIndex)->enumName;
		if (limbEnumName != "")
		{
			return StringHelper::Sprintf("%s - 1", limbEnumName.c_str());
		}
	}
	else
	{
		HANDLE_WARNING_RESOURCE(WarningType::InvalidExtractedData, parent, this, rawDataIndex,
		                        StringHelper::Sprintf("Limb index '%02i' out of range", limbIndex),
		                        "");
	}

	return StringHelper::Sprintf("0x%02X", limbIndex);
}
