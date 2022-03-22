#pragma once

#include <array>
#include <cstdint>
#include "ZResource.h"

class ZMtx : public ZResource
{
public:
	ZMtx(ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
	DeclarationAlignment GetDeclarationAlignment() const override;

public:
	std::array<std::array<int32_t, 4>, 4> mtx;
};
