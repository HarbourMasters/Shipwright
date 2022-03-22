#pragma once

#include "ZRoom/ZRoomCommand.h"

class RoomEntry
{
public:
	int32_t virtualAddressStart;
	int32_t virtualAddressEnd;

	RoomEntry(uint32_t nVAS, uint32_t nVAE);
	RoomEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	size_t GetRawDataSize() const;
};

class RomFile : public ZResource
{
public:
	RomFile(ZFile* nParent);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& body) override;
	std::string GetBodySourceCode() const override;
	void GetSourceOutputCode(const std::string& prefix) override;

	std::string GetSourceTypeName() const override;
	virtual ZResourceType GetResourceType() const override;

	virtual size_t GetRawDataSize() const override;

	uint8_t numRooms = 0;
	std::vector<RoomEntry> rooms;
};

class SetRoomList : public ZRoomCommand
{
public:
	// Borrowed reference. Don't delete.
	RomFile* romfile = nullptr;

	SetRoomList(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
