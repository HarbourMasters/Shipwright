#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ZResource.h"
#include "ZScalar.h"
#include "tinyxml2.h"

class ZVector : public ZResource
{
public:
	std::vector<ZScalar> scalars;
	ZScalarType scalarType;
	uint32_t dimensions;

	ZVector(ZFile* nParent);

	void ExtractFromBinary(uint32_t nRawDataIndex, ZScalarType nScalarType, uint32_t nDimensions);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	bool DoesSupportArray() const override;
	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;
	size_t GetRawDataSize() const override;
	DeclarationAlignment GetDeclarationAlignment() const override;
};
