#pragma once

#include "ZFile.h"
#include "ZResource.h"

class ZCollisionPoly : public ZResource
{
public:
	uint16_t type;
	uint16_t vtxA, vtxB, vtxC;
	uint16_t normX, normY, normZ;
	uint16_t dist;

	ZCollisionPoly(ZFile* nParent);
	~ZCollisionPoly();

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;
	std::string GetDefaultName(const std::string& prefix) const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	bool DoesSupportArray() const override;

	size_t GetRawDataSize() const override;
};