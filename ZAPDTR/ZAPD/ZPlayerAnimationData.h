#pragma once

#include <cstdint>
#include <vector>

#include "ZResource.h"

class ZPlayerAnimationData : public ZResource
{
public:
	int16_t frameCount = 0;
	std::vector<uint16_t> limbRotData;

	ZPlayerAnimationData(ZFile* nParent);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;

	std::string GetBodySourceCode() const override;
	std::string GetDefaultName(const std::string& prefix) const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
};
