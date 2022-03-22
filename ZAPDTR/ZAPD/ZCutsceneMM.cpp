#include "ZCutsceneMM.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

ZCutsceneMM::ZCutsceneMM(ZFile* nParent) : ZCutsceneBase(nParent)
{
}

ZCutsceneMM::~ZCutsceneMM()
{
	for (CutsceneCommand* cmd : commands)
		delete cmd;
}

std::string ZCutsceneMM::GetBodySourceCode() const
{
	std::string output;

	output += StringHelper::Sprintf("    CS_BEGIN_CUTSCENE(%i, %i),", numCommands, endFrame);

	for (size_t i = 0; i < data.size(); i++)
	{
		if ((i % 4) == 0)
			output += "\n    ";
		output += StringHelper::Sprintf("0x%08X, ", data[i]);
	}

	return output;
}

size_t ZCutsceneMM::GetRawDataSize() const
{
	return 8 + data.size() * 4;
}

void ZCutsceneMM::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	numCommands = BitConverter::ToInt32BE(rawData, rawDataIndex + 0);
	commands = std::vector<CutsceneCommand*>();

	endFrame = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
	uint32_t currentPtr = rawDataIndex + 8;
	uint32_t lastData = 0;

	// TODO currently cutscenes aren't being parsed, so just consume words until we see an end
	// marker.
	do
	{
		lastData = BitConverter::ToInt32BE(rawData, currentPtr);
		data.push_back(lastData);
		currentPtr += 4;
	} while (lastData != 0xFFFFFFFF);
}

ZResourceType ZCutsceneMM::GetResourceType() const
{
	return ZResourceType::Cutscene;
}
