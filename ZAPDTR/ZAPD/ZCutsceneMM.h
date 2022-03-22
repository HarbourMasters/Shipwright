#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ZCutscene.h"
#include "ZFile.h"
#include "tinyxml2.h"

class ZCutsceneMM : public ZCutsceneBase
{
public:
	ZCutsceneMM(ZFile* nParent);
	virtual ~ZCutsceneMM();

	std::string GetBodySourceCode() const override;

	size_t GetRawDataSize() const override;

	void ParseRawData() override;
	ZResourceType GetResourceType() const override;

protected:
	int32_t numCommands;
	int32_t endFrame;
	std::vector<CutsceneCommand*> commands;

	std::vector<uint32_t> data;
};
