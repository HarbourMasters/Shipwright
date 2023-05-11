#pragma once

#include "ZResource.h"

class ActorSpawnEntry
{
public:
	uint16_t actorNum;
	int16_t posX;
	int16_t posY;
	int16_t posZ;
	uint16_t rotX;
	uint16_t rotY;
	uint16_t rotZ;
	uint16_t params;
	size_t largestActorName = 16;

	ActorSpawnEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const;

	std::string GetSourceTypeName() const;
	size_t GetRawDataSize() const;

	uint16_t GetActorId() const;
	void SetLargestActorName(size_t nameSize);
};

class ZActorList : public ZResource
{
public:
	std::vector<ActorSpawnEntry> actors;
	uint32_t numActors = 0;

	ZActorList(ZFile* nParent);

	void ExtractFromBinary(offset_t nRawDataIndex, uint8_t nNumActors);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;
	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;

protected:
	size_t GetActorListArraySize() const;
};
