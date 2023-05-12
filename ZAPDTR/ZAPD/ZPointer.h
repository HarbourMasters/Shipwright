#pragma once

#include "ZResource.h"

class ZPointer : public ZResource
{
public:
	segptr_t ptr = SEGMENTED_NULL;
	std::string type;

	ZPointer(ZFile* nParent);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	std::string GetBodySourceCode() const override;

	bool DoesSupportArray() const override;
	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
};
