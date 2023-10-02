#include "Cutscene_Commands.h"

#include <cassert>
#include <unordered_map>

#include "CutsceneMM_Commands.h"
#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

/* CutsceneSubCommandEntry */

CutsceneSubCommandEntry::CutsceneSubCommandEntry(const std::vector<uint8_t>& rawData,
                                                 offset_t rawDataIndex)
{
	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
	pad = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);
}

std::string CutsceneSubCommandEntry::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X), CMD_HH(0x%04X, 0x%04X)", base, startFrame,
	                             endFrame, pad);
}

size_t CutsceneSubCommandEntry::GetRawSize() const
{
	return 0x08;
}

/* CutsceneCommand */

CutsceneCommand::CutsceneCommand(const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
{
	numEntries = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);
}

CutsceneCommand::~CutsceneCommand()
{
	for (auto& entry : entries)
	{
		delete entry;
	}
}

std::string CutsceneCommand::GetCommandMacro() const
{
	return StringHelper::Sprintf("CMD_W(0x%08X), CMD_W(0x%08X)", commandID, numEntries);
}

std::string CutsceneCommand::GenerateSourceCode() const
{
	std::string result;

	result += GetCommandMacro();
	result += ",\n";

	for (auto& entry : entries)
	{
		result += "        ";
		result += entry->GetBodySourceCode();
		result += ",\n";
	}

	return result;
}

size_t CutsceneCommand::GetCommandSize() const
{
	size_t size = 0;
	if (entries.size() > 0)
	{
		size = entries.at(0)->GetRawSize() * entries.size();
	}
	else
	{
		size = 0x08 * numEntries;
	}
	return 0x08 + size;
}

void CutsceneCommand::SetCommandID(uint32_t nCommandID)
{
	commandID = nCommandID;

	for (auto& entry : entries)
	{
		entry->commandID = commandID;
	}
}

// Specific for command lists where each entry has size 0x30 bytes
const std::unordered_map<CutsceneCommands, CsCommandListDescriptor> csCommandsDesc = {
	{CutsceneCommands::Misc,
     {"CS_MISC", "(0x%04X, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)"}},
	{CutsceneCommands::SetLighting,
     {"CS_LIGHTING", "(0x%02X, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)"}},
	{CutsceneCommands::PlayBGM, {"CS_PLAY_BGM", "(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)"}},
	{CutsceneCommands::StopBGM, {"CS_STOP_BGM", "(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)"}},
	{CutsceneCommands::FadeBGM, {"CS_FADE_BGM", "(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)"}},
};

CutsceneSubCommandEntry_GenericCmd::CutsceneSubCommandEntry_GenericCmd(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex, CutsceneCommands cmdId)
	: CutsceneSubCommandEntry(rawData, rawDataIndex), commandId(cmdId)
{
	word0 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x0);
	word1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x4);

	unused1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x8);
	unused2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0xC);
	unused3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x10);
	unused4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x14);
	unused5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x18);
	unused6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x1C);
	unused7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x20);
	unused8 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x24);
	unused9 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x28);
	unused10 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x2C);
}

std::string CutsceneSubCommandEntry_GenericCmd::GetBodySourceCode() const
{
	const auto& element = csCommandsDesc.find(commandId);

	if (element != csCommandsDesc.end())
	{
		std::string entryFmt = element->second.cmdMacro;
		entryFmt += element->second.args;

		return StringHelper::Sprintf(entryFmt.c_str(), base, startFrame, endFrame, pad, unused1,
		                             unused2, unused3, unused4, unused5, unused6, unused7, unused8,
		                             unused9, unused10);
	}

	return StringHelper::Sprintf("CS_UNK_DATA(0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, "
	                             "0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X)",
	                             word0, word1, unused1, unused2, unused3, unused4, unused5, unused6,
	                             unused7, unused8, unused9, unused10);
}

size_t CutsceneSubCommandEntry_GenericCmd::GetRawSize() const
{
	return 0x30;
}

CutsceneCommand_GenericCmd::CutsceneCommand_GenericCmd(const std::vector<uint8_t>& rawData,
                                                       offset_t rawDataIndex,
                                                       CutsceneCommands cmdId)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	commandID = static_cast<uint32_t>(cmdId);
	entries.reserve(numEntries);

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_GenericCmd(rawData, rawDataIndex, cmdId);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_GenericCmd::GetCommandMacro() const
{
	const auto& element = csCommandsDesc.find(static_cast<CutsceneCommands>(commandID));

	if (element != csCommandsDesc.end())
	{
		return StringHelper::Sprintf("%s_LIST(%i)", element->second.cmdMacro, numEntries);
	}

	return StringHelper::Sprintf("CS_UNK_DATA_LIST(0x%X, %i)", commandID, numEntries);
}

CutsceneCameraPoint::CutsceneCameraPoint(const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	continueFlag = BitConverter::ToInt8BE(rawData, rawDataIndex + 0);
	cameraRoll = BitConverter::ToInt8BE(rawData, rawDataIndex + 1);
	nextPointFrame = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	viewAngle = BitConverter::ToFloatBE(rawData, rawDataIndex + 4);

	posX = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	posY = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
	posZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);

	unused = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
}

std::string CutsceneCameraPoint::GetBodySourceCode() const
{
	std::string result = "";

	if (commandID == (int32_t)CutsceneCommands::SetCameraFocus)
	{
		result += "CS_CAM_FOCUS_POINT";
	}
	else if (commandID == (int32_t)CutsceneCommands::SetCameraFocusLink)
	{
		result += "CS_CAM_FOCUS_POINT_PLAYER";
	}
	else if (commandID == (int32_t)CutsceneCommands::SetCameraPosLink)
	{
		result += "CS_CAM_POS_PLAYER";
	}
	else
	{
		result += "CS_CAM_POS";
	}

	std::string continueMacro = "CS_CMD_CONTINUE";
	if (continueFlag != 0)
		continueMacro = "CS_CMD_STOP";

	result +=
		StringHelper::Sprintf("(%s, 0x%02X, %i, %ff, %i, %i, %i, 0x%04X)", continueMacro.c_str(),
	                          cameraRoll, nextPointFrame, viewAngle, posX, posY, posZ, unused);
	return result;
}

size_t CutsceneCameraPoint::GetRawSize() const
{
	return 0x10;
}

CutsceneCommandSetCameraPos::CutsceneCommandSetCameraPos(const std::vector<uint8_t>& rawData,
                                                         offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
	unused = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);

	bool shouldContinue = true;

	uint32_t currentPtr = rawDataIndex + 8;

	while (shouldContinue)
	{
		CutsceneCameraPoint* camPoint = new CutsceneCameraPoint(rawData, currentPtr);
		entries.push_back(camPoint);

		if (camPoint->continueFlag == -1)
			shouldContinue = false;

		currentPtr += camPoint->GetRawSize();
	}
}

std::string CutsceneCommandSetCameraPos::GetCommandMacro() const
{
	std::string result;

	std::string listStr;

	if (commandID == (int32_t)CutsceneCommands::SetCameraFocus)
	{
		listStr = "CS_CAM_FOCUS_POINT_LIST";
	}
	else if (commandID == (int32_t)CutsceneCommands::SetCameraFocusLink)
	{
		listStr = "CS_CAM_FOCUS_POINT_PLAYER_LIST";
	}
	else if (commandID == (int32_t)CutsceneCommands::SetCameraPosLink)
	{
		listStr = "CS_CAM_POS_PLAYER_LIST";
	}
	else
	{
		listStr = "CS_CAM_POS_LIST";
	}

	result += StringHelper::Sprintf("%s(%i, %i)", listStr.c_str(), startFrame, endFrame);

	return result;
}

size_t CutsceneCommandSetCameraPos::GetCommandSize() const
{
	return 0x0C + entries.at(0)->GetRawSize() * entries.size();
}

CutsceneSubCommandEntry_Rumble::CutsceneSubCommandEntry_Rumble(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unk_06 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x06);
	unk_07 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x07);
	unk_08 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
	unk_09 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
	unk_0A = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0A);
	unk_0B = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0B);
}

std::string CutsceneSubCommandEntry_Rumble::GetBodySourceCode() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_RUMBLE(%i, %i, %i, 0x%02X, 0x%02X, 0x%02X)", base,
		                             startFrame, endFrame, unk_06, unk_07, unk_08);
	}

	return StringHelper::Sprintf("CS_CMD_09(%i, %i, %i, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X)",
	                             base, startFrame, endFrame, unk_06, unk_07, unk_08, unk_09, unk_0A,
	                             unk_0B);
}

size_t CutsceneSubCommandEntry_Rumble::GetRawSize() const
{
	return 0x0C;
}

CutsceneCommand_Rumble::CutsceneCommand_Rumble(const std::vector<uint8_t>& rawData,
                                               offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Rumble(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_Rumble::GetCommandMacro() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_RUMBLE_LIST(%i)", numEntries);
	}
	return StringHelper::Sprintf("CS_CMD_09_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_SetTime::CutsceneSubCommandEntry_SetTime(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	hour = BitConverter::ToUInt8BE(rawData, rawDataIndex + 6);
	minute = BitConverter::ToUInt8BE(rawData, rawDataIndex + 7);
	unk_08 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
}

std::string CutsceneSubCommandEntry_SetTime::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_TIME(%i, %i, %i, %i, %i, %i)", base, startFrame, endFrame,
	                             hour, minute, unk_08);
}

size_t CutsceneSubCommandEntry_SetTime::GetRawSize() const
{
	return 0x0C;
}

CutsceneCommand_SetTime::CutsceneCommand_SetTime(const std::vector<uint8_t>& rawData,
                                                 offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_SetTime(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_SetTime::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_TIME_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_TextBox::CutsceneSubCommandEntry_TextBox(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	type = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x6);
	textId1 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x8);
	textId2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0xA);
}

std::string CutsceneSubCommandEntry_TextBox::GetBodySourceCode() const
{
	if (type == 0xFFFF)
	{
		return StringHelper::Sprintf("CS_TEXT_NONE(%i, %i)", startFrame, endFrame);
	}
	if (type == 2)
	{
		return StringHelper::Sprintf("CS_TEXT_LEARN_SONG(%i, %i, %i, 0x%X)", base, startFrame,
		                             endFrame, textId1);
	}

	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		switch (type)
		{
		case 0:
			return StringHelper::Sprintf("CS_TEXT_DEFAULT(0x%X, %i, %i, 0x%X, 0x%X)", base,
			                             startFrame, endFrame, textId1, textId2);

		case 1:
			return StringHelper::Sprintf("CS_TEXT_TYPE_1(0x%X, %i, %i, 0x%X, 0x%X)", base,
			                             startFrame, endFrame, textId1, textId2);

		case 3:
			return StringHelper::Sprintf("CS_TEXT_TYPE_3(0x%X, %i, %i, 0x%X, 0x%X)", base,
			                             startFrame, endFrame, textId1, textId2);

		case 4:
			return StringHelper::Sprintf("CS_TEXT_BOSSES_REMAINS(0x%X, %i, %i, 0x%X)", base,
			                             startFrame, endFrame, textId1);

		case 5:
			return StringHelper::Sprintf("CS_TEXT_ALL_NORMAL_MASKS(0x%X, %i, %i, 0x%X)", base,
			                             startFrame, endFrame, textId1);
		}
	}

	return StringHelper::Sprintf("CS_TEXT_DISPLAY_TEXTBOX(0x%X, %i, %i, %i, 0x%X, 0x%X)", base,
	                             startFrame, endFrame, type, textId1, textId2);
}

size_t CutsceneSubCommandEntry_TextBox::GetRawSize() const
{
	return 0x0C;
}

CutsceneCommand_TextBox::CutsceneCommand_TextBox(const std::vector<uint8_t>& rawData,
                                                 offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_TextBox(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_TextBox::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_TEXT_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_ActorAction::CutsceneSubCommandEntry_ActorAction(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	rotX = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x6);
	rotY = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x8);
	rotZ = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0xA);
	startPosX = BitConverter::ToInt32BE(rawData, rawDataIndex + 0xC);
	startPosY = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x10);
	startPosZ = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x14);
	endPosX = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x18);
	endPosY = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x1C);
	endPosZ = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x20);
	normalX = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x24);
	normalY = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x28);
	normalZ = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x2C);
}

std::string CutsceneSubCommandEntry_ActorAction::GetBodySourceCode() const
{
	std::string result;

	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		if (static_cast<CutsceneMMCommands>(commandID) ==
		    CutsceneMMCommands::CS_CMD_SET_PLAYER_ACTION)
		{
			result = "CS_PLAYER_ACTION";
		}
		else
		{
			result = "CS_ACTOR_ACTION";
		}
	}
	else
	{
		if (static_cast<CutsceneCommands>(commandID) == CutsceneCommands::SetPlayerAction)
		{
			result = "CS_PLAYER_ACTION";
		}
		else
		{
			result = "CS_NPC_ACTION";
		}
	}

	result +=
		StringHelper::Sprintf("(%i, %i, %i, 0x%04X, 0x%04X, 0x%04X, %i, %i, "
	                          "%i, %i, %i, %i, %.11ef, %.11ef, %.11ef)",
	                          base, startFrame, endFrame, rotX, rotY, rotZ, startPosX, startPosY,
	                          startPosZ, endPosX, endPosY, endPosZ, normalX, normalY, normalZ);
	return result;
}

size_t CutsceneSubCommandEntry_ActorAction::GetRawSize() const
{
	return 0x30;
}

CutsceneCommand_ActorAction::CutsceneCommand_ActorAction(const std::vector<uint8_t>& rawData,
                                                         offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_ActorAction(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_ActorAction::GetCommandMacro() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		if (static_cast<CutsceneMMCommands>(commandID) ==
		    CutsceneMMCommands::CS_CMD_SET_PLAYER_ACTION)
		{
			return StringHelper::Sprintf("CS_PLAYER_ACTION_LIST(%i)", numEntries);
		}
		return StringHelper::Sprintf("CS_ACTOR_ACTION_LIST(0x%03X, %i)", commandID, numEntries);
	}

	if (static_cast<CutsceneCommands>(commandID) == CutsceneCommands::SetPlayerAction)
	{
		return StringHelper::Sprintf("CS_PLAYER_ACTION_LIST(%i)", entries.size());
	}
	return StringHelper::Sprintf("CS_NPC_ACTION_LIST(0x%03X, %i)", commandID, entries.size());
}

CutsceneCommand_Terminator::CutsceneCommand_Terminator(const std::vector<uint8_t>& rawData,
                                                       offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
	unknown = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);  // endFrame duplicate
}

std::string CutsceneCommand_Terminator::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_TERMINATOR(%i, %i, %i),\n", base, startFrame, endFrame);

	return result;
}

size_t CutsceneCommand_Terminator::GetCommandSize() const
{
	return 0x10;
}

CutsceneCommandSceneTransFX::CutsceneCommandSceneTransFX(const std::vector<uint8_t>& rawData,
                                                         offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
}

std::string CutsceneCommandSceneTransFX::GenerateSourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_TRANS_FX(%i, %i, %i),\n", base, startFrame, endFrame);
}

size_t CutsceneCommandSceneTransFX::GetCommandSize() const
{
	return 0x10;
}
