#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "tinyxml2.h"

#include "OtherStructs/Cutscene_Commands.h"
#include "ZFile.h"
#include "ZResource.h"

class ZCutscene : public ZResource
{
public:
	ZCutscene(ZFile* nParent);
	~ZCutscene();

	void ParseRawData() override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;

	std::string GetBodySourceCode() const override;

	size_t GetRawDataSize() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	int32_t numCommands;
	int32_t endFrame;
	std::vector<CutsceneCommand*> commands;

protected:
	CutsceneCommand* GetCommandOoT(uint32_t id, offset_t currentPtr) const;
	CutsceneCommand* GetCommandMM(uint32_t id, offset_t currentPtr) const;
};
