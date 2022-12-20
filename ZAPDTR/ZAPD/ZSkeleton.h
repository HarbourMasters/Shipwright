#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ZDisplayList.h"
#include "ZFile.h"
#include "ZLimb.h"

enum class ZSkeletonType
{
	Normal,
	Flex,
	Curve,
};

class ZLimbTable : public ZResource
{
public:
	ZLimbType limbType = ZLimbType::Standard;
	size_t count = 0;

	std::vector<segptr_t> limbsAddresses;
	std::vector<ZLimb*> limbsReferences;  // borrowed pointers, do not delete!

	// XML attributes
	std::string enumName;
	std::string limbNoneName;
	std::string limbMaxName;

	ZLimbTable(ZFile* nParent);

	void ExtractFromBinary(uint32_t nRawDataIndex, ZLimbType nLimbType, size_t nCount);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceOutputHeader(const std::string& prefix) override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;

	std::string GetLimbEnumName(uint8_t limbIndex) const;
};

class ZSkeleton : public ZResource
{
public:
	ZSkeletonType type = ZSkeletonType::Normal;
	ZLimbType limbType = ZLimbType::Standard;
	std::string enumName;
	std::string limbNoneName;
	std::string limbMaxName;

	segptr_t limbsArrayAddress;
	uint8_t limbCount = 0;
	uint8_t dListCount = 0;  // FLEX SKELETON ONLY
	ZLimbTable limbsTable;

	ZSkeleton(ZFile* nParent);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
	DeclarationAlignment GetDeclarationAlignment() const override;

	uint8_t GetLimbCount();

protected:
	ZLimbTable* limbsTable = nullptr;  // borrowed pointer, do not delete!

};
