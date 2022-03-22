#pragma once

#include "ZRoom/ZRoom.h"
#include "ZRoom/ZRoomCommand.h"

class SetAlternateHeaders : public ZRoomCommand
{
public:
	std::vector<uint32_t> headers;

	SetAlternateHeaders(ZFile* nParent);

	void DeclareReferences(const std::string& prefix) override;
	void ParseRawDataLate() override;
	void DeclareReferencesLate(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

private:
};
