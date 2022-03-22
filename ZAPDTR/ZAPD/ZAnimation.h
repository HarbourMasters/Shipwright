#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Vec3s.h"
#include "ZResource.h"
#include "ZSkeleton.h"
#include "tinyxml2.h"

struct RotationIndex
{
	// uint16_t transX, transY, transZ;
	uint16_t x, y, z;

	RotationIndex(uint16_t nX, uint16_t nY, uint16_t nZ) : x(nX), y(nY), z(nZ) {}
};

class ZAnimation : public ZResource
{
public:
	int16_t frameCount;

	ZAnimation(ZFile* nParent);

	//std::string GetSourceOutputHeader(const std::string& prefix) override;
	ZResourceType GetResourceType() const override;

protected:
	void ParseRawData() override;
};

class ZNormalAnimation : public ZAnimation
{
public:
	std::vector<uint16_t> rotationValues;
	std::vector<RotationIndex> rotationIndices;
	segptr_t rotationValuesSeg = 0;
	segptr_t rotationIndicesSeg = 0;
	offset_t rotationValuesOffset = 0;
	offset_t rotationIndicesOffset = 0;
	int16_t limit = 0;

	ZNormalAnimation(ZFile* nParent);

	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	size_t GetRawDataSize() const override;
	std::string GetSourceTypeName() const override;

	void ParseRawData() override;
};

class ZLinkAnimation : public ZAnimation
{
public:
	segptr_t segmentAddress;

	ZLinkAnimation(ZFile* nParent);

	std::string GetBodySourceCode() const override;

	size_t GetRawDataSize() const override;
	std::string GetSourceTypeName() const override;

	void ParseRawData() override;
};

class TransformData
{
public:
	ZFile* parent;

	///* 0x0000 */ u16 unk_00; // appears to be flags
	uint16_t unk_00;
	///* 0x0002 */ s16 unk_02;
	int16_t unk_02;
	///* 0x0004 */ s16 unk_04;
	int16_t unk_04;
	///* 0x0006 */ s16 unk_06;
	int16_t unk_06;
	///* 0x0008 */ f32 unk_08;
	float unk_08;

public:
	TransformData() = default;
	TransformData(ZFile* parent, const std::vector<uint8_t>& rawData, uint32_t fileOffset);
	TransformData(ZFile* parent, const std::vector<uint8_t>& rawData, uint32_t fileOffset,
	              size_t index);

	[[nodiscard]] std::string GetBody(const std::string& prefix) const;

	size_t GetRawDataSize() const;
	std::string GetSourceTypeName();
};

class ZCurveAnimation : public ZAnimation
{
public:
	segptr_t skelOffset = 0;

	///* 0x0000 */ u8* refIndex;
	segptr_t refIndex = 0;
	///* 0x0004 */ TransformData* transformData;
	segptr_t transformData = 0;
	///* 0x0008 */ s16* copyValues;
	segptr_t copyValues = 0;
	///* 0x000C */ s16 unk_0C;
	int16_t unk_0C;
	///* 0x000E */ s16 unk_10;
	int16_t unk_10;

	uint8_t limbCount = 0;

	std::vector<uint8_t> refIndexArr;
	std::vector<TransformData> transformDataArr;
	std::vector<int16_t> copyValuesArr;

public:
	ZCurveAnimation(ZFile* nParent);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;

	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	size_t GetRawDataSize() const override;
	DeclarationAlignment GetDeclarationAlignment() const override;

	std::string GetSourceTypeName() const override;
};
// TransformUpdateIndex

/* ZLegacyAnimation */

class JointKey : public ZResource
{
public:
	JointKey(ZFile* nParent);

	void ParseRawData() override;
	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;

protected:
	int16_t xMax, x;
	int16_t yMax, y;
	int16_t zMax, z;
};

class ZLegacyAnimation : public ZAnimation
{
public:
	ZLegacyAnimation(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;

	size_t GetRawDataSize() const override;

protected:
	int16_t limbCount;
	segptr_t frameData;  // s16*
	segptr_t jointKey;   // JointKey*

	std::vector<uint16_t> frameDataArray;
	std::vector<JointKey> jointKeyArray;
};
