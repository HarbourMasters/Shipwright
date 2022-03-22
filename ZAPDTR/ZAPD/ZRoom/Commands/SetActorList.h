#pragma once

#include "ZRoom/ZRoomCommand.h"

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
	uint16_t initVar;
	size_t largestActorName = 16;

	ActorSpawnEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const;

	std::string GetSourceTypeName() const;
	int32_t GetRawDataSize() const;

	uint16_t GetActorId() const;
	void SetLargestActorName(size_t nameSize);
};

class SetActorList : public ZRoomCommand
{
public:
	uint8_t numActors;
	std::vector<ActorSpawnEntry> actors;

	SetActorList(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	void ParseRawDataLate() override;
	void DeclareReferencesLate(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

protected:
	size_t GetActorListArraySize() const;
};
