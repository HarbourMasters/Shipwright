#pragma once

#include "ZResource.h"
#include "tinyxml2.h"

class ZSymbol : public ZResource
{
protected:
	std::string type;
	size_t typeSize;
	bool isArray = false;
	uint32_t count = 0;

public:
	ZSymbol(ZFile* nParent);

	void ParseXML(tinyxml2::XMLElement* reader) override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;

	std::string GetSourceOutputHeader(const std::string& prefix) override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
};
