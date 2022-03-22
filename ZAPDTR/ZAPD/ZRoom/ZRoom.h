#pragma once

#include <map>
#include <string>
#include <vector>

#include "ZResource.h"
#include "ZRoomCommand.h"
#include "tinyxml2.h"

class ZRoom : public ZResource
{
public:
	std::vector<ZRoomCommand*> commands;
	int32_t roomCount;  // Only valid for scenes

	std::string hackMode;

	ZResourceType zroomType = ZResourceType::Error;
	ZResourceType parentZroomType = ZResourceType::Error;

	ZRoom(ZFile* nParent);
	virtual ~ZRoom();

	void ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex) override;
	void ExtractFromBinary(uint32_t nRawDataIndex, ZResourceType parentType);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;
	void ParseRawDataLate() override;
	void DeclareReferencesLate(const std::string& prefix) override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& body) override;
	std::string GetBodySourceCode() const override;

	void GetSourceOutputCode(const std::string& prefix) override;

	std::string GetDefaultName(const std::string& prefix) const override;
	size_t GetDeclarationSizeFromNeighbor(uint32_t declarationAddress);
	size_t GetCommandSizeFromNeighbor(ZRoomCommand* cmd);
	ZRoomCommand* FindCommandOfType(RoomCommand cmdType);

	size_t GetRawDataSize() const override;
	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

protected:
	void SyotesRoomHack();
};
