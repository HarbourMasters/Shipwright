#pragma once

#include "Vec3s.h"
#include "ZPath.h"
#include "ZResource.h"
#include "ZRoom/ZRoomCommand.h"

class SetPathways : public ZRoomCommand
{
public:
	ZPath pathwayList;

	SetPathways(ZFile* nParent);

	void DeclareReferences(const std::string& prefix) override;

	void ParseRawDataLate() override;
	void DeclareReferencesLate(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
