#include "ZLimb.h"

#include <cassert>

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "WarningHandler.h"

REGISTER_ZFILENODE(Limb, ZLimb);

ZLimb::ZLimb(ZFile* nParent) : ZResource(nParent), segmentStruct(nParent)
{
	RegisterOptionalAttribute("LimbType");
	RegisterOptionalAttribute("Type");
}

void ZLimb::ExtractFromBinary(uint32_t nRawDataIndex, ZLimbType nType)
{
	rawDataIndex = nRawDataIndex;
	type = nType;

	// Don't parse raw data of external files
	if (parent->GetMode() == ZFileMode::ExternalFile)
		return;

	ParseRawData();
}

void ZLimb::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	// Reading from a <Skeleton/>
	std::string limbType = registeredAttributes.at("LimbType").value;
	if (limbType == "")  // Reading from a <Limb/>
		limbType = registeredAttributes.at("Type").value;

	if (limbType == "")
	{
		HANDLE_ERROR_RESOURCE(WarningType::MissingAttribute, parent, this, rawDataIndex,
		                      "missing 'LimbType' attribute in <Limb>", "");
	}

	type = GetTypeByAttributeName(limbType);
	if (type == ZLimbType::Invalid)
	{
		HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
		                      "invalid value found for 'LimbType' attribute", "");
	}
}

void ZLimb::ParseRawData()
{
	ZResource::ParseRawData();
	const auto& rawData = parent->GetRawData();

	if (type == ZLimbType::Curve)
	{
		childIndex = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0);
		siblingIndex = BitConverter::ToUInt8BE(rawData, rawDataIndex + 1);

		dListPtr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
		dList2Ptr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
		return;
	}
	if (type == ZLimbType::Legacy)
	{
		dListPtr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x00);
		legTransX = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x04);
		legTransY = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x08);
		legTransZ = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x0C);
		rotX = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x10);
		rotY = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x12);
		rotZ = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x14);
		childPtr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x18);
		siblingPtr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x1C);
		return;
	}

	transX = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	transY = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	transZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);

	childIndex = rawData.at(rawDataIndex + 6);
	siblingIndex = rawData.at(rawDataIndex + 7);

	switch (type)
	{
	case ZLimbType::LOD:
		dList2Ptr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
		[[fallthrough]];
	case ZLimbType::Standard:
		dListPtr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
		break;

	case ZLimbType::Skin:
		skinSegmentType =
			static_cast<ZLimbSkinType>(BitConverter::ToInt32BE(rawData, rawDataIndex + 8));
		skinSegment = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
		if (skinSegmentType == ZLimbSkinType::SkinType_4)
		{
			if (skinSegment != 0 && GETSEGNUM(skinSegment) == parent->segment)
			{
				uint32_t skinSegmentOffset = Seg2Filespace(skinSegment, parent->baseAddress);
				segmentStruct.ExtractFromFile(skinSegmentOffset);
			}
		}
		break;

	case ZLimbType::Curve:
	case ZLimbType::Legacy:
		break;

	case ZLimbType::Invalid:
		assert(!"whoops");
		break;
	}
}

void ZLimb::DeclareReferences(const std::string& prefix)
{
	std::string varPrefix = name;
	if (varPrefix == "")
		varPrefix = prefix;

	ZResource::DeclareReferences(varPrefix);

	std::string suffix;
	switch (type)
	{
	case ZLimbType::Legacy:
		if (childPtr != 0 && GETSEGNUM(childPtr) == parent->segment)
		{
			uint32_t childOffset = Seg2Filespace(childPtr, parent->baseAddress);
			if (!parent->HasDeclaration(childOffset))
			{
				ZLimb* child = new ZLimb(parent);
				child->ExtractFromBinary(childOffset, ZLimbType::Legacy);
				child->DeclareVar(varPrefix, "");
				child->DeclareReferences(varPrefix);
				parent->AddResource(child);
			}
		}
		if (siblingPtr != 0 && GETSEGNUM(siblingPtr) == parent->segment)
		{
			uint32_t siblingdOffset = Seg2Filespace(siblingPtr, parent->baseAddress);
			if (!parent->HasDeclaration(siblingdOffset))
			{
				ZLimb* sibling = new ZLimb(parent);
				sibling->ExtractFromBinary(siblingdOffset, ZLimbType::Legacy);
				sibling->DeclareVar(varPrefix, "");
				sibling->DeclareReferences(varPrefix);
				parent->AddResource(sibling);
			}
		}
		break;

	case ZLimbType::Curve:
	case ZLimbType::LOD:
		suffix = "Far";
		if (type == ZLimbType::Curve)
			suffix = "Curve2";
		DeclareDList(dList2Ptr, varPrefix, suffix);
		[[fallthrough]];
	case ZLimbType::Standard:
		suffix = "";
		if (type == ZLimbType::Curve)
			suffix = "Curve";
		DeclareDList(dListPtr, varPrefix, suffix);
		break;

	case ZLimbType::Skin:
		switch (skinSegmentType)
		{
		case ZLimbSkinType::SkinType_4:
			if (skinSegment != 0 && GETSEGNUM(skinSegment) == parent->segment)
			{
				segmentStruct.DeclareReferences(varPrefix);
				segmentStruct.GetSourceOutputCode(varPrefix);
			}
			break;

		case ZLimbSkinType::SkinType_DList:
			DeclareDList(skinSegment, varPrefix, "");
			break;

		default:
			break;
		}
		break;

	case ZLimbType::Invalid:
		break;
	}
}

size_t ZLimb::GetRawDataSize() const
{
	switch (type)
	{
	case ZLimbType::Standard:
	case ZLimbType::Curve:
		return 0x0C;

	case ZLimbType::LOD:
	case ZLimbType::Skin:
		return 0x10;

	case ZLimbType::Legacy:
		return 0x20;

	case ZLimbType::Invalid:
		break;
	}

	return 0x0C;
}

std::string ZLimb::GetBodySourceCode() const
{
	std::string dListStr;
	std::string dListStr2;
	Globals::Instance->GetSegmentedArrayIndexedName(dListPtr, 8, parent, "Gfx", dListStr);
	Globals::Instance->GetSegmentedArrayIndexedName(dList2Ptr, 8, parent, "Gfx", dListStr2);

	std::string entryStr = "\n\t";
	if (type == ZLimbType::Legacy)
	{
		std::string childName;
		std::string siblingName;
		Globals::Instance->GetSegmentedPtrName(childPtr, parent, "LegacyLimb", childName);
		Globals::Instance->GetSegmentedPtrName(siblingPtr, parent, "LegacyLimb", siblingName);

		entryStr += StringHelper::Sprintf("%s,\n", dListStr.c_str());
		entryStr +=
			StringHelper::Sprintf("\t{ %ff, %ff, %ff },\n", legTransX, legTransY, legTransZ);
		entryStr += StringHelper::Sprintf("\t{ 0x%04X, 0x%04X, 0x%04X },\n", rotX, rotY, rotZ);
		entryStr += StringHelper::Sprintf("\t%s,\n", childName.c_str());
		entryStr += StringHelper::Sprintf("\t%s\n", siblingName.c_str());
	}
	else
	{
		if (type != ZLimbType::Curve)
		{
			entryStr += StringHelper::Sprintf("{ %i, %i, %i }, ", transX, transY, transZ);
		}
		entryStr += StringHelper::Sprintf("0x%02X, 0x%02X,\n", childIndex, siblingIndex);

		switch (type)
		{
		case ZLimbType::Standard:
			entryStr += StringHelper::Sprintf("\t%s\n", dListStr.c_str());
			break;

		case ZLimbType::LOD:
		case ZLimbType::Curve:
			entryStr +=
				StringHelper::Sprintf("\t{ %s, %s }\n", dListStr.c_str(), dListStr2.c_str());
			break;

		case ZLimbType::Skin:
		{
			std::string skinSegmentStr;
			Globals::Instance->GetSegmentedPtrName(skinSegment, parent, "", skinSegmentStr);
			entryStr +=
				StringHelper::Sprintf("\t0x%02X, %s\n", skinSegmentType, skinSegmentStr.c_str());
		}
		break;

		case ZLimbType::Legacy:
			break;

		case ZLimbType::Invalid:
			break;
		}
	}

	return entryStr;
}

std::string ZLimb::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sLimb_%06X", prefix.c_str(), rawDataIndex);
}

std::string ZLimb::GetSourceTypeName() const
{
	return GetSourceTypeName(type);
}

ZResourceType ZLimb::GetResourceType() const
{
	return ZResourceType::Limb;
}

ZLimbType ZLimb::GetLimbType()
{
	return type;
}

void ZLimb::SetLimbType(ZLimbType value)
{
	type = value;
}

const char* ZLimb::GetSourceTypeName(ZLimbType limbType)
{
	switch (limbType)
	{
	case ZLimbType::Standard:
		return "StandardLimb";

	case ZLimbType::LOD:
		return "LodLimb";

	case ZLimbType::Skin:
		return "SkinLimb";

	case ZLimbType::Curve:
		return "SkelCurveLimb";

	case ZLimbType::Legacy:
		return "LegacyLimb";

	default:
		return "StandardLimb";
	}
}

ZLimbType ZLimb::GetTypeByAttributeName(const std::string& attrName)
{
	if (attrName == "Standard")
	{
		return ZLimbType::Standard;
	}
	if (attrName == "LOD")
	{
		return ZLimbType::LOD;
	}
	if (attrName == "Skin")
	{
		return ZLimbType::Skin;
	}
	if (attrName == "Curve")
	{
		return ZLimbType::Curve;
	}
	if (attrName == "Legacy")
	{
		return ZLimbType::Legacy;
	}
	return ZLimbType::Invalid;
}

void ZLimb::DeclareDList(segptr_t dListSegmentedPtr, const std::string& prefix,
                         const std::string& limbSuffix)
{
	if (dListSegmentedPtr == 0 || GETSEGNUM(dListSegmentedPtr) != parent->segment)
		return;

	uint32_t dlistOffset = Seg2Filespace(dListSegmentedPtr, parent->baseAddress);
	if (parent->HasDeclaration(dlistOffset))
		return;

	if (!parent->IsOffsetInFileRange(dlistOffset) || dlistOffset >= parent->GetRawData().size())
		return;

	std::string dlistName;
	bool declFound = Globals::Instance->GetSegmentedArrayIndexedName(dListSegmentedPtr, 8, parent,
	                                                                 "Gfx", dlistName);
	if (declFound)
		return;

	int32_t dlistLength = ZDisplayList::GetDListLength(
		parent->GetRawData(), dlistOffset,
		Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX);
	ZDisplayList* dlist = new ZDisplayList(parent);
	dlist->ExtractFromBinary(dlistOffset, dlistLength);

	std::string dListStr =
		StringHelper::Sprintf("%s%sDL_%06X", prefix.c_str(), limbSuffix.c_str(), dlistOffset);
	dlist->SetName(dListStr);
	dlist->DeclareVar(prefix, "");
	parent->AddResource(dlist);
}
