#include "ZResource.h"

#include <cassert>
#include <regex>

#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZFile.h"
#include <Globals.h>
#include <set>
#include <ZDisplayList.h>
#include <ZArray.h>

ZResource::ZResource(ZFile* nParent)
{
	// assert(nParent != nullptr);
	parent = nParent;
	name = "";
	outName = "";
	sourceOutput = "";
	rawDataIndex = 0;
	outputDeclaration = true;
	hash = 0;

	RegisterRequiredAttribute("Name");
	RegisterOptionalAttribute("OutName");
	RegisterOptionalAttribute("Offset");
	RegisterOptionalAttribute("Custom");
	RegisterOptionalAttribute("Static", "Global");
}

void ZResource::ExtractFromXML(tinyxml2::XMLElement* reader, offset_t nRawDataIndex)
{
	rawDataIndex = nRawDataIndex;
	declaredInXml = true;

	if (reader != nullptr)
		ParseXML(reader);

	// Don't parse raw data of external files
	if (parent->GetMode() != ZFileMode::ExternalFile)
	{
		ParseRawData();
		CalcHash();
	}

	if (!isInner)
	{
		Declaration* decl = DeclareVar(parent->GetName(), "");
		if (decl != nullptr)
		{
			decl->declaredInXml = true;
			decl->staticConf = staticConf;
		}
	}
}

void ZResource::ExtractFromFile(offset_t nRawDataIndex)
{
	rawDataIndex = nRawDataIndex;

	// Don't parse raw data of external files
	if (parent->GetMode() == ZFileMode::ExternalFile)
		return;

	ParseRawData();
	CalcHash();
}

void ZResource::ParseXML(tinyxml2::XMLElement* reader)
{
	if (reader != nullptr)
	{
		// If it is an inner node, then 'Name' isn't required
		if (isInner)
		{
			registeredAttributes.at("Name").isRequired = false;
		}

		auto attrs = reader->FirstAttribute();
		while (attrs != nullptr)
		{
			std::string attrName = attrs->Name();
			bool attrDeclared = false;

			if (registeredAttributes.find(attrName) != registeredAttributes.end())
			{
				registeredAttributes[attrName].value = attrs->Value();
				registeredAttributes[attrName].wasSet = true;
				attrDeclared = true;
			}

			if (!attrDeclared)
			{
				HANDLE_WARNING_RESOURCE(
					WarningType::UnknownAttribute, parent, this, rawDataIndex,
					StringHelper::Sprintf("unexpected '%s' attribute in resource <%s>",
				                          attrName.c_str(), reader->Name()),
					"");
			}
			attrs = attrs->Next();
		}

		if (!Globals::Instance->otrMode)
		{
			if (!canHaveInner && !reader->NoChildren())
			{
				std::string errorHeader = StringHelper::Sprintf(
					"resource '%s' with inner element/child detected", reader->Name());
				HANDLE_ERROR_PROCESS(WarningType::InvalidXML, errorHeader, "");
			}
		}

		for (const auto& attr : registeredAttributes)
		{
			if (attr.second.isRequired && attr.second.value == "")
			{
				std::string headerMsg =
					StringHelper::Sprintf("missing required attribute '%s' in resource <%s>",
				                          attr.first.c_str(), reader->Name());
				HANDLE_ERROR_RESOURCE(WarningType::MissingAttribute, parent, this, rawDataIndex,
				                      headerMsg, "");
			}
		}

		name = registeredAttributes.at("Name").value;

		// Disable this check for OTR file generation for now since it takes up a considerable amount of CPU time
		if (!Globals::Instance->otrMode)
		{
			static std::regex r("[a-zA-Z_]+[a-zA-Z0-9_]*",
			                    std::regex::icase | std::regex::optimize);

			if (!isInner || (isInner && name != ""))
			{
				if (!std::regex_match(name, r))
				{
					HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this,
					                      rawDataIndex, "invalid value found for 'Name' attribute",
					                      "");
				}
			}
		}

		outName = registeredAttributes.at("OutName").value;
		if (outName == "")
			outName = name;

		isCustomAsset = registeredAttributes["Custom"].wasSet;

		std::string& staticXml = registeredAttributes["Static"].value;
		if (staticXml == "Global")
		{
			staticConf = StaticConfig::Global;
		}
		else if (staticXml == "On")
		{
			staticConf = StaticConfig::On;
		}
		else if (staticXml == "Off")
		{
			staticConf = StaticConfig::Off;
		}
		else
		{
			HANDLE_ERROR_RESOURCE(
				WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
				StringHelper::Sprintf("invalid value '%s' for 'Static' attribute", staticConf), "");
		}

		declaredInXml = true;
	}
}

void ZResource::ParseRawData()
{
}

void ZResource::DeclareReferences([[maybe_unused]] const std::string& prefix)
{
}

void ZResource::ParseRawDataLate()
{
}

void ZResource::DeclareReferencesLate([[maybe_unused]] const std::string& prefix)
{
}

Declaration* ZResource::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	Declaration* decl =
		parent->AddDeclaration(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                           GetSourceTypeName(), auxName, bodyStr);
	decl->staticConf = staticConf;
	return decl;
}

void ZResource::Save([[maybe_unused]] const fs::path& outFolder)
{
}

const std::string& ZResource::GetName() const
{
	return name;
}

const std::string& ZResource::GetOutName() const
{
	return outName;
}

void ZResource::SetOutName(const std::string& nName)
{
	outName = nName;
}

void ZResource::SetName(const std::string& nName)
{
	name = nName;
}

bool ZResource::IsExternalResource() const
{
	return false;
}

bool ZResource::DoesSupportArray() const
{
	return false;
}

std::string ZResource::GetExternalExtension() const
{
	return "";
}

DeclarationAlignment ZResource::GetDeclarationAlignment() const
{
	return DeclarationAlignment::Align4;
}

bool ZResource::WasDeclaredInXml() const
{
	return declaredInXml;
}

StaticConfig ZResource::GetStaticConf() const
{
	return staticConf;
}

offset_t ZResource::GetRawDataIndex() const
{
	return rawDataIndex;
}

std::string ZResource::GetBodySourceCode() const
{
	return "ERROR";
}

std::string ZResource::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%s%s_%06X", prefix.c_str(), GetSourceTypeName().c_str(),
	                             rawDataIndex);
}

void ZResource::GetSourceOutputCode([[maybe_unused]] const std::string& prefix)
{
	std::string bodyStr = GetBodySourceCode();

	if (bodyStr != "ERROR")
	{
		Declaration* decl = parent->GetDeclaration(rawDataIndex);

		if (decl == nullptr || decl->isPlaceholder)
			decl = DeclareVar(prefix, bodyStr);
		else
			decl->text = bodyStr;

		// OTRTODO: This is a hack and we need something more elegant in the future...
		if (GetResourceType() == ZResourceType::Array)
		{
			ZArray* arr = (ZArray*)this;
			if (arr->resList[0]->GetResourceType() == ZResourceType::Vertex)
			{
				for (int i = 0; i < arr->resList.size(); i++)
				{
					ZVtx* vtx = (ZVtx*)arr->resList[i];
					decl->vertexHack.push_back(vtx);

				}
			}
		}

		if (decl != nullptr)
			decl->staticConf = staticConf;
	}
}

std::string ZResource::GetSourceOutputHeader([[maybe_unused]] const std::string& prefix, std::set<std::string> *nameSet)
{
	if (Globals::Instance->otrMode && genOTRDef)
	{
		std::string str = "";;
		std::string nameStr = StringHelper::Strip(StringHelper::Strip(name, "\n"), "\r");

		std::string outName = parent->GetOutName();
		std::string prefix = "";

		if (GetResourceType() == ZResourceType::DisplayList || GetResourceType() == ZResourceType::Texture)
		{
			//ZDisplayList* dList = (ZDisplayList*)this;

			if (StringHelper::Contains(outName, "_room_"))
			{
				outName = StringHelper::Split(outName, "_room")[0] + "_scene";
			}
		}

		std::string xmlPath = StringHelper::Replace(parent->GetXmlFilePath().string(), "\\", "/");

		if (StringHelper::Contains(outName, "_room_") || StringHelper::Contains(outName, "_scene"))
			prefix = "scenes/nonmq";
		else if (StringHelper::Contains(xmlPath, "objects/"))
			prefix = "objects";
		else if (StringHelper::Contains(xmlPath, "textures/"))
			prefix = "textures";
		else if (StringHelper::Contains(xmlPath, "overlays/"))
			prefix = "overlays";
		else if (StringHelper::Contains(xmlPath, "misc/"))
			prefix = "misc";
		else if (StringHelper::Contains(xmlPath, "code/"))
			prefix = "code";
		else if (StringHelper::Contains(xmlPath, "text/"))
			prefix = "text";

		if (prefix != "") {
			str += StringHelper::Sprintf("#define d%s \"__OTR__%s/%s/%s\"", name.c_str(), prefix.c_str(), outName.c_str(), nameStr.c_str());
        }
		else
			str += StringHelper::Sprintf("#define d%s \"__OTR__%s/%s\"", name.c_str(), outName.c_str(), nameStr.c_str());

		if (nameSet && nameSet->find(name) == nameSet->end()) {
			str += StringHelper::Sprintf(R"(
#ifdef _WIN32
static const __declspec(align(2)) char %s[] = d%s;
#else
static const char %s[] __attribute__((aligned (2))) = d%s;
#endif
			)", name.c_str(), name.c_str(), name.c_str(), name.c_str());

			if (nameSet) {
				nameSet->insert(name);
			}
		}

		if (name == "gTitleZeldaShieldLogoMQTex")
		{
			std::string addName = "gTitleZeldaShieldLogoTex";
			nameStr = StringHelper::Strip(StringHelper::Strip(addName, "\n"), "\r");
			str += StringHelper::Sprintf("\n#define d%s \"__OTR__%s/%s/%s\"", addName.c_str(), prefix.c_str(), outName.c_str(), nameStr.c_str());
			if (nameSet && nameSet->find(addName) == nameSet->end())
			{
				str += StringHelper::Sprintf(R"(
#ifdef _WIN32
static const __declspec(align(2)) char %s[] = d%s;
#else
static const char %s[] __attribute__((aligned (2))) = d%s;
#endif
			)", addName.c_str(), addName.c_str(), addName.c_str(), addName.c_str());

				if (nameSet)
				{
					nameSet->insert(addName);
				}
			}
		}

		return str;
	}
	else
		return "";
}

ZResourceType ZResource::GetResourceType() const
{
	return ZResourceType::Error;
}

void ZResource::CalcHash()
{
	hash = 0;
}

void ZResource::SetInnerNode(bool inner)
{
	isInner = inner;
}

void ZResource::RegisterRequiredAttribute(const std::string& attr)
{
	ResourceAttribute resAtrr;
	resAtrr.key = attr;
	resAtrr.isRequired = true;
	registeredAttributes[attr] = resAtrr;
}

void ZResource::RegisterOptionalAttribute(const std::string& attr, const std::string& defaultValue)
{
	ResourceAttribute resAtrr;
	resAtrr.key = attr;
	resAtrr.value = defaultValue;
	registeredAttributes[attr] = resAtrr;
}

offset_t Seg2Filespace(segptr_t segmentedAddress, uint32_t parentBaseAddress)
{
	offset_t currentPtr = GETSEGOFFSET(segmentedAddress);

	if (GETSEGNUM(segmentedAddress) == 0x80)  // Is defined in code?
	{
		uint32_t parentBaseOffset = GETSEGOFFSET(parentBaseAddress);
		if (parentBaseOffset > currentPtr)
		{
			HANDLE_ERROR(WarningType::Always,
			             StringHelper::Sprintf(
							 "resource address 0x%08X is smaller than 'BaseAddress' 0x%08X",
							 segmentedAddress, parentBaseAddress),
			             "Maybe your 'BaseAddress' is wrong?");
		}

		currentPtr -= parentBaseOffset;
	}

	return currentPtr;
}
