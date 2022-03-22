#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ZResource.h"
#include "ZScalar.h"
#include "tinyxml2.h"

class ZVtx : public ZResource
{
public:
	int16_t x, y, z;
	uint16_t flag;
	int16_t s, t;
	uint8_t r, g, b, a;

	ZVtx(ZFile* nParent);

	void ParseRawData() override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;
	std::string GetBodySourceCode() const override;

	bool IsExternalResource() const override;
	bool DoesSupportArray() const override;
	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;
	std::string GetExternalExtension() const override;

	size_t GetRawDataSize() const override;
	DeclarationAlignment GetDeclarationAlignment() const override;
};
