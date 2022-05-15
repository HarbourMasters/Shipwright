#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "OtherStructs/SkinLimbStructs.h"
#include "ZDisplayList.h"
#include "ZFile.h"

enum class ZLimbType
{
	Invalid,
	Standard,
	LOD,
	Skin,
	Curve,
	Legacy,
};

class ZLimb : public ZResource
{
public:
	ZLimbType type = ZLimbType::Standard;

	ZLimbSkinType skinSegmentType = ZLimbSkinType::SkinType_0;  // Skin only
	segptr_t skinSegment = 0;                                   // Skin only
	Struct_800A5E28 segmentStruct = {0};                              // Skin only

	// Legacy only
	float legTransX = 0, legTransY = 0, legTransZ = 0;  // Vec3f
	uint16_t rotX = 0, rotY = 0, rotZ = 0;              // Vec3s
	segptr_t childPtr = 0;                      // LegacyLimb*
	segptr_t siblingPtr = 0;                    // LegacyLimb*

	segptr_t dListPtr = 0;
	segptr_t dList2Ptr = 0;  // LOD and Curve Only

	int16_t transX = 0, transY = 0, transZ = 0;
	uint8_t childIndex = 0, siblingIndex = 0;

	ZLimb(ZFile* nParent);

	void ExtractFromBinary(uint32_t nRawDataIndex, ZLimbType nType);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;
	std::string GetDefaultName(const std::string& prefix) const override;

	size_t GetRawDataSize() const override;
	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	ZLimbType GetLimbType();
	void SetLimbType(ZLimbType value);
	static const char* GetSourceTypeName(ZLimbType limbType);
	static ZLimbType GetTypeByAttributeName(const std::string& attrName);

protected:
	void DeclareDList(segptr_t dListSegmentedPtr, const std::string& prefix,
	                  const std::string& limbSuffix);
};
