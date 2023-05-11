#pragma once

#include <array>
#include "ZFile.h"
#include "ZResource.h"

class ZSurfaceType : public ZResource
{
public:
	std::array<uint32_t, 2> data;

	ZSurfaceType(ZFile* nParent);
	~ZSurfaceType();

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;
	std::string GetDefaultName(const std::string& prefix) const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	bool DoesSupportArray() const override;

	size_t GetRawDataSize() const override;
};
