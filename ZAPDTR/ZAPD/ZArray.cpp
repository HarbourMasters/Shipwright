#include "ZArray.h"

#include <cassert>

#include "Globals.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Array, ZArray);

ZArray::ZArray(ZFile* nParent) : ZResource(nParent)
{
	canHaveInner = true;
	genOTRDef = true;
	RegisterRequiredAttribute("Count");
}

ZArray::~ZArray()
{
	for (auto res : resList)
		delete res;
}

void ZArray::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	arrayCnt = reader->IntAttribute("Count", 0);
	if (arrayCnt <= 0)
	{
		HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
		                      "invalid value found for 'Count' attribute", "");
	}

	tinyxml2::XMLElement* child = reader->FirstChildElement();
	if (child == nullptr)
	{
		HANDLE_ERROR_RESOURCE(WarningType::InvalidXML, parent, this, rawDataIndex,
		                      "<Array> needs one sub-element", "");
	}

	childName = child->Name();

	auto nodeMap = ZFile::GetNodeMap();
	size_t childIndex = rawDataIndex;
	for (size_t i = 0; i < arrayCnt; i++)
	{
		ZResource* res = nodeMap->at(childName)(parent);
		if (!res->DoesSupportArray())
		{
			std::string errorHeader = StringHelper::Sprintf(
				"resource <%s> does not support being wrapped in an <Array>", childName.c_str());
			HANDLE_ERROR_RESOURCE(WarningType::InvalidXML, parent, this, rawDataIndex, errorHeader,
			                      "");
		}
		res->parent = parent;
		res->SetInnerNode(true);
		res->ExtractFromXML(child, childIndex);

		childIndex += res->GetRawDataSize();
		resList.push_back(res);
	}
}

Declaration* ZArray::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	ZResource* res = resList.at(0);
	Declaration* decl;
	if (res->IsExternalResource())
	{
		auto filepath = Globals::Instance->outputPath / name;
		std::string includePath = StringHelper::Sprintf("%s.%s.inc", filepath.c_str(),
		                                                res->GetExternalExtension().c_str());
		decl = parent->AddDeclarationIncludeArray(rawDataIndex, includePath, GetRawDataSize(),
		                                          GetSourceTypeName(), name, arrayCnt);
		decl->text = bodyStr;
		decl->isExternal = true;
	}
	else
	{
		decl =
			parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
		                                GetSourceTypeName(), name, arrayCnt, bodyStr);
	}

	decl->staticConf = staticConf;
	return decl;
}

std::string ZArray::GetBodySourceCode() const
{
	std::string output = "";

	for (size_t i = 0; i < arrayCnt; i++)
	{
		const auto& res = resList[i];
		output += "\t";

		if (res->GetResourceType() == ZResourceType::Scalar ||
		    res->GetResourceType() == ZResourceType::Vertex)
			output += resList.at(i)->GetBodySourceCode();
		else
			output += StringHelper::Sprintf("{ %s }", resList.at(i)->GetBodySourceCode().c_str());

		if (i < arrayCnt - 1 || res->IsExternalResource())
			output += ",\n";
	}

	return output;
}

size_t ZArray::GetRawDataSize() const
{
	size_t size = 0;
	for (auto res : resList)
		size += res->GetRawDataSize();
	return size;
}

std::string ZArray::GetSourceTypeName() const
{
	return resList.at(0)->GetSourceTypeName();
}

ZResourceType ZArray::GetResourceType() const
{
	return ZResourceType::Array;
}

DeclarationAlignment ZArray::GetDeclarationAlignment() const
{
	if (resList.size() == 0)
	{
		return DeclarationAlignment::Align4;
	}
	return resList.at(0)->GetDeclarationAlignment();
}
