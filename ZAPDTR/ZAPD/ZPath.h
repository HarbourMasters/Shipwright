#pragma once

#include "ZResource.h"
#include "ZVector.h"

class PathwayEntry : public ZResource
{
public:
	PathwayEntry(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
	segptr_t GetListAddress() const;

public:
	int32_t numPoints;
	int8_t unk1;   // (MM Only)
	int16_t unk2;  // (MM Only)
	segptr_t listSegmentAddress;
	std::vector<ZVector> points;
};

class ZPath : public ZResource
{
public:
	ZPath(ZFile* nParent);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;
	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
	void SetNumPaths(uint32_t nNumPaths);

public:
	uint32_t numPaths;
	std::vector<PathwayEntry> pathways;
};
