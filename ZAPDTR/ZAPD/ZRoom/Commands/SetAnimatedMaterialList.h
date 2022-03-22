#pragma once

#include "ZRoom/ZRoomCommand.h"
#include "ZTextureAnimation.h"

class SetAnimatedMaterialList : public ZRoomCommand
{
public:
	SetAnimatedMaterialList(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

private:
	ZTextureAnimation textureAnimation;
};
