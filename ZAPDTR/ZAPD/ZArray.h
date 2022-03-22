#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ZResource.h"
#include "tinyxml2.h"

class ZArray : public ZResource
{
public:
	ZArray(ZFile* nParent);
	~ZArray();

	void ParseXML(tinyxml2::XMLElement* reader) override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;
	std::string GetBodySourceCode() const override;

	size_t GetRawDataSize() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	DeclarationAlignment GetDeclarationAlignment() const override;

	size_t arrayCnt;
	std::vector<ZResource*> resList;
protected:
	std::string childName;
};
