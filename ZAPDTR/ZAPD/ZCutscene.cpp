#include "ZCutscene.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZResource.h"

REGISTER_ZFILENODE(Cutscene, ZCutscene);

ZCutscene::ZCutscene(ZFile* nParent) : ZCutsceneBase(nParent)
{
	genOTRDef = true;
}

ZCutscene::~ZCutscene()
{
	for (CutsceneCommand* cmd : commands)
		delete cmd;
}
CutsceneCommandSetCameraPos::~CutsceneCommandSetCameraPos()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandSpecialAction::~CutsceneCommandSpecialAction()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandFadeBGM::~CutsceneCommandFadeBGM()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandPlayBGM::~CutsceneCommandPlayBGM()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandStopBGM::~CutsceneCommandStopBGM()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandEnvLighting::~CutsceneCommandEnvLighting()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandUnknown9::~CutsceneCommandUnknown9()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandUnknown::~CutsceneCommandUnknown()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandDayTime::~CutsceneCommandDayTime()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandTextbox::~CutsceneCommandTextbox()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandActorAction::~CutsceneCommandActorAction()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandSceneTransFX::~CutsceneCommandSceneTransFX()
{
}

std::string ZCutscene::GetBodySourceCode() const
{
	std::string output = "";
	uint32_t curPtr = 0;

	output += StringHelper::Sprintf("    CS_BEGIN_CUTSCENE(%i, %i),\n", commands.size(), endFrame);

	for (size_t i = 0; i < commands.size(); i++)
	{
		CutsceneCommand* cmd = commands[i];
		output += "    " + cmd->GenerateSourceCode(curPtr);
		curPtr += cmd->GetCommandSize();
	}

	output += StringHelper::Sprintf("    CS_END(),\n", commands.size(), endFrame);

	return output;
}

size_t ZCutscene::GetRawDataSize() const
{
	size_t size = 0;

	// Beginning
	size += 8;

	for (size_t i = 0; i < commands.size(); i++)
	{
		CutsceneCommand* cmd = commands[i];
		size += cmd->GetCommandSize();
		size += 4;
	}

	// End
	size += 8;

	return size;
}

void ZCutscene::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();

	numCommands = BitConverter::ToInt32BE(rawData, rawDataIndex + 0);
	commands = std::vector<CutsceneCommand*>();

	endFrame = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
	uint32_t currentPtr = rawDataIndex + 8;

	for (int32_t i = 0; i < numCommands; i++)
	{
		int32_t id = BitConverter::ToInt32BE(rawData, currentPtr);

		if (id == -1)
			break;

		CutsceneCommands cmdID = (CutsceneCommands)GetCommandFromID(id);
		currentPtr += 4;

		int32_t numEntries = 1;

		for (int32_t j = 0; j < numEntries; j++)
		{
			CutsceneCommand* cmd = nullptr;

			switch (cmdID)
			{
			case CutsceneCommands::Cmd00:
				break;
			case CutsceneCommands::SetCameraPos:
				cmd = new CutsceneCommandSetCameraPos(rawData, currentPtr);
				break;
			case CutsceneCommands::SetCameraFocus:
				cmd = new CutsceneCommandSetCameraPos(rawData, currentPtr);
				break;
			case CutsceneCommands::SpecialAction:
				cmd = new CutsceneCommandSpecialAction(rawData, currentPtr);
				break;
			case CutsceneCommands::SetLighting:
				cmd = new CutsceneCommandEnvLighting(rawData, currentPtr);
				break;
			case CutsceneCommands::SetCameraPosLink:
				cmd = new CutsceneCommandSetCameraPos(rawData, currentPtr);
				break;
			case CutsceneCommands::SetCameraFocusLink:
				cmd = new CutsceneCommandSetCameraPos(rawData, currentPtr);
				break;
			case CutsceneCommands::Cmd07:
				break;
			case CutsceneCommands::Cmd08:
				break;
			case CutsceneCommands::Cmd09:
				cmd = new CutsceneCommandUnknown9(rawData, currentPtr);
				break;
			case CutsceneCommands::Textbox:
				cmd = new CutsceneCommandTextbox(rawData, currentPtr);
				break;
			case CutsceneCommands::Unknown:
				cmd = new CutsceneCommandUnknown(rawData, currentPtr);
				break;
			case CutsceneCommands::SetActorAction0:
			case CutsceneCommands::SetActorAction1:
			case CutsceneCommands::SetActorAction2:
			case CutsceneCommands::SetActorAction3:
			case CutsceneCommands::SetActorAction4:
			case CutsceneCommands::SetActorAction5:
			case CutsceneCommands::SetActorAction6:
			case CutsceneCommands::SetActorAction7:
			case CutsceneCommands::SetActorAction8:
			case CutsceneCommands::SetActorAction9:
			case CutsceneCommands::SetActorAction10:
				cmd = new CutsceneCommandActorAction(rawData, currentPtr);
				break;
			case CutsceneCommands::SetSceneTransFX:
				cmd = new CutsceneCommandSceneTransFX(rawData, currentPtr);
				break;
			case CutsceneCommands::Nop:
				cmd = new CutsceneCommandNop(rawData, currentPtr);
				break;
			case CutsceneCommands::PlayBGM:
				cmd = new CutsceneCommandPlayBGM(rawData, currentPtr);
				break;
			case CutsceneCommands::StopBGM:
				cmd = new CutsceneCommandStopBGM(rawData, currentPtr);
				break;
			case CutsceneCommands::FadeBGM:
				cmd = new CutsceneCommandFadeBGM(rawData, currentPtr);
				break;
			case CutsceneCommands::SetTime:
				cmd = new CutsceneCommandDayTime(rawData, currentPtr);
				break;
			case CutsceneCommands::Terminator:
				cmd = new CutsceneCommandTerminator(rawData, currentPtr);
				break;
			case CutsceneCommands::End:
				cmd = new CutsceneCommandEnd(rawData, currentPtr);
				break;
			case CutsceneCommands::Error:
				HANDLE_WARNING_RESOURCE(WarningType::NotImplemented, parent, this, rawDataIndex,
				                        StringHelper::Sprintf("cutscene command error %d", cmdID),
				                        "");
				break;
			}

			cmd->commandIndex = i;
			cmd->commandID = (uint32_t)id;
			currentPtr += (uint32_t)cmd->GetCommandSize();

			commands.push_back(cmd);
		}
	}
}

CutsceneCommands ZCutscene::GetCommandFromID(int32_t id)
{
	switch (id)
	{
	case 0x0003:
		return CutsceneCommands::SpecialAction;
	case 0x0004:
		return CutsceneCommands::SetLighting;
	case 0x0056:
		return CutsceneCommands::PlayBGM;
	case 0x0057:
		return CutsceneCommands::StopBGM;
	case 0x007C:
		return CutsceneCommands::FadeBGM;
	case 0x0009:
		return CutsceneCommands::Cmd09;
	case 0x0013:
		return CutsceneCommands::Textbox;
	case 0x008C:
		return CutsceneCommands::SetTime;
	case 0x0001:
		return CutsceneCommands::SetCameraPos;
	case 0x0002:
		return CutsceneCommands::SetCameraFocus;
	case 0x0005:
		return CutsceneCommands::SetCameraPosLink;
	case 0x0006:
		return CutsceneCommands::SetCameraFocusLink;
	case 0x0007:
		return CutsceneCommands::Cmd07;
	case 0x0008:
		return CutsceneCommands::Cmd08;
	case 0x03E8:
		return CutsceneCommands::Terminator;
	case 0xFFFF:
		return CutsceneCommands::End;
	case 0x002D:
		return CutsceneCommands::SetSceneTransFX;
	case 10:
		return CutsceneCommands::SetActorAction0;
	case 15:
	case 17:
	case 18:
	case 23:
	case 34:
	case 39:
	case 46:
	case 76:
	case 85:
	case 93:
	case 105:
	case 107:
	case 110:
	case 119:
	case 123:
	case 138:
	case 139:
	case 144:
		return CutsceneCommands::SetActorAction1;
	case 14:
	case 16:
	case 24:
	case 35:
	case 40:
	case 48:
	case 64:
	case 68:
	case 70:
	case 78:
	case 80:
	case 94:
	case 116:
	case 118:
	case 120:
	case 125:
	case 131:
	case 141:
		return CutsceneCommands::SetActorAction2;
	case 25:
	case 36:
	case 41:
	case 50:
	case 67:
	case 69:
	case 72:
	case 81:
	case 106:
	case 117:
	case 121:
	case 126:
	case 132:
		return CutsceneCommands::SetActorAction3;
	case 29:
	case 37:
	case 42:
	case 51:
	case 53:
	case 63:
	case 65:
	case 66:
	case 75:
	case 82:
	case 108:
	case 127:
	case 133:
		return CutsceneCommands::SetActorAction4;
	case 30:
	case 38:
	case 43:
	case 47:
	case 54:
	case 79:
	case 83:
	case 128:
	case 135:
		return CutsceneCommands::SetActorAction5;
	case 44:
	case 55:
	case 77:
	case 84:
	case 90:
	case 129:
	case 136:
		return CutsceneCommands::SetActorAction6;
	case 31:
	case 52:
	case 57:
	case 58:
	case 88:
	case 115:
	case 130:
	case 137:
		return CutsceneCommands::SetActorAction7;
	case 49:
	case 60:
	case 89:
	case 111:
	case 114:
	case 134:
	case 142:
		return CutsceneCommands::SetActorAction8;
	case 62:
		return CutsceneCommands::SetActorAction9;
	case 143:
		return CutsceneCommands::SetActorAction10;
	case 0x0B:
	case 0x0D:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x20:
	case 0x21:
	case 0x3B:
	case 0x3D:
	case 0x47:
	case 0x49:
	case 0x6D:
	case 0x15:
	case 0x16:
	case 0x70:
	case 0x71:
	case 0x4A:
		return CutsceneCommands::Unknown;
	}

	HANDLE_WARNING_RESOURCE(
		WarningType::NotImplemented, parent, this, rawDataIndex,
		StringHelper::Sprintf("could not identify cutscene command. ID 0x%04X", id), "");

	return CutsceneCommands::Error;
}

ZResourceType ZCutscene::GetResourceType() const
{
	return ZResourceType::Cutscene;
}

CutsceneCommand::CutsceneCommand([[maybe_unused]] const std::vector<uint8_t>& rawData,
                                 [[maybe_unused]] uint32_t rawDataIndex)
{
}

CutsceneCommand::~CutsceneCommand()
{
}

std::string CutsceneCommand::GetCName()
{
	return "SCmdCutsceneData";
}

std::string CutsceneCommand::GenerateSourceCode(uint32_t baseAddress)
{
	return StringHelper::Sprintf("%s CutsceneData%04XCmd%02X = { 0x%02X,", GetCName().c_str(),
	                             baseAddress, commandIndex, commandID);
}

size_t CutsceneCommand::GetCommandSize()
{
	return 4;
}

CutsceneCameraPoint::CutsceneCameraPoint(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	const uint8_t* data = rawData.data();

	continueFlag = data[rawDataIndex + 0];
	cameraRoll = data[rawDataIndex + 1];
	nextPointFrame = BitConverter::ToInt16BE(data, rawDataIndex + 2);
	viewAngle = BitConverter::ToFloatBE(data, rawDataIndex + 4);

	posX = BitConverter::ToInt16BE(data, rawDataIndex + 8);
	posY = BitConverter::ToInt16BE(data, rawDataIndex + 10);
	posZ = BitConverter::ToInt16BE(data, rawDataIndex + 12);

	unused = BitConverter::ToInt16BE(data, rawDataIndex + 14);
}

CutsceneCommandSetCameraPos::CutsceneCommandSetCameraPos(const std::vector<uint8_t>& rawData,
                                                         uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	const uint8_t* data = rawData.data();

	base = BitConverter::ToUInt16BE(data, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(data, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(data, rawDataIndex + 4);
	unused = BitConverter::ToUInt16BE(data, rawDataIndex + 6);

	entries = std::vector<CutsceneCameraPoint*>();

	bool shouldContinue = true;

	uint32_t currentPtr = rawDataIndex + 8;

	while (shouldContinue)
	{
		CutsceneCameraPoint* camPoint = new CutsceneCameraPoint(rawData, currentPtr);
		entries.push_back(camPoint);

		if (camPoint->continueFlag == -1)
			shouldContinue = false;

		currentPtr += 16;
	}
}

// TODO
std::string CutsceneCommandSetCameraPos::GetCName()
{
	return "";
}

std::string CutsceneCommandSetCameraPos::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	std::string listStr;
	std::string posStr;

	if (commandID == (int32_t)CutsceneCommands::SetCameraFocus)
	{
		listStr = "CS_CAM_FOCUS_POINT_LIST";
		posStr = "CS_CAM_FOCUS_POINT";
	}
	else if (commandID == (int32_t)CutsceneCommands::SetCameraFocusLink)
	{
		listStr = "CS_CAM_FOCUS_POINT_PLAYER_LIST";
		posStr = "CS_CAM_FOCUS_POINT_PLAYER";
	}
	else if (commandID == (int32_t)CutsceneCommands::SetCameraPosLink)
	{
		listStr = "CS_CAM_POS_PLAYER_LIST";
		posStr = "CS_CAM_POS_PLAYER";
	}
	else
	{
		listStr = "CS_CAM_POS_LIST";
		posStr = "CS_CAM_POS";
	}

	result += StringHelper::Sprintf("%s(%i, %i),\n", listStr.c_str(), startFrame, endFrame);

	for (size_t i = 0; i < entries.size(); i++)
	{
		std::string continueMacro = "CS_CMD_CONTINUE";
		if (entries[i]->continueFlag != 0)
			continueMacro = "CS_CMD_STOP";
		result += StringHelper::Sprintf("        %s(%s, 0x%02X, %i, %ff, %i, %i, %i, 0x%04X),\n",
		                                posStr.c_str(), continueMacro.c_str(),
		                                entries[i]->cameraRoll, entries[i]->nextPointFrame,
		                                entries[i]->viewAngle, entries[i]->posX, entries[i]->posY,
		                                entries[i]->posZ, entries[i]->unused);
	}

	return result;
}

size_t CutsceneCommandSetCameraPos::GetCommandSize()
{
	return 8 + (entries.size() * 16);
}

MusicFadeEntry::MusicFadeEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
	unknown0 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);
	unknown1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
	unknown2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
	unknown3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 16);
	unknown4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 20);
	unknown5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 24);
	unknown6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 28);
	unknown7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 32);
	unknown8 = BitConverter::ToUInt32BE(rawData,
	                                    rawDataIndex + 36);  // Macro hardcodes it as zero
	unknown9 = BitConverter::ToUInt32BE(rawData,
	                                    rawDataIndex + 40);  // Macro hardcodes it as zero
	unknown10 = BitConverter::ToUInt32BE(rawData,
	                                     rawDataIndex + 44);  // Macro hardcodes it as zero
}

CutsceneCommandFadeBGM::CutsceneCommandFadeBGM(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	uint32_t numEntries = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);

	rawDataIndex += 4;

	for (uint32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new MusicFadeEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandFadeBGM::GetCName()
{
	return "CsCmdMusicFade";
}

std::string CutsceneCommandFadeBGM::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_FADE_BGM_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"        CS_FADE_BGM(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),\n", entries[i]->base,
			entries[i]->startFrame, entries[i]->endFrame, entries[i]->unknown0,
			entries[i]->unknown1, entries[i]->unknown2, entries[i]->unknown3, entries[i]->unknown4,
			entries[i]->unknown5, entries[i]->unknown6, entries[i]->unknown7);
	}

	return result;
}

size_t CutsceneCommandFadeBGM::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + 0x30 * entries.size();
}

MusicChangeEntry::MusicChangeEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	sequence = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
	unknown0 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);
	unknown1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
	unknown2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
	unknown3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 16);
	unknown4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 20);
	unknown5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 24);
	unknown6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 28);
	unknown7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 32);
}

CutsceneCommandPlayBGM::CutsceneCommandPlayBGM(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	uint32_t numEntries = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);

	rawDataIndex += 4;

	for (uint32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new MusicChangeEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandPlayBGM::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_PLAY_BGM_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"        CS_PLAY_BGM(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),\n",
			entries[i]->sequence, entries[i]->startFrame, entries[i]->endFrame,
			entries[i]->unknown0, entries[i]->unknown1, entries[i]->unknown2, entries[i]->unknown3,
			entries[i]->unknown4, entries[i]->unknown5, entries[i]->unknown6, entries[i]->unknown7);
	}

	return result;
}

std::string CutsceneCommandPlayBGM::GetCName()
{
	return "CsCmdMusicChange";
}

size_t CutsceneCommandPlayBGM::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + 0x30;
}

CutsceneCommandStopBGM::CutsceneCommandStopBGM(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	uint32_t numEntries = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);

	rawDataIndex += 4;

	for (uint32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new MusicChangeEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandStopBGM::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_STOP_BGM_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"\t\tCS_STOP_BGM(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),\n", entries[i]->sequence,
			entries[i]->startFrame, entries[i]->endFrame, entries[i]->unknown0,
			entries[i]->unknown1, entries[i]->unknown2, entries[i]->unknown3, entries[i]->unknown4,
			entries[i]->unknown5, entries[i]->unknown6, entries[i]->unknown7);
	}

	return result;
}

std::string CutsceneCommandStopBGM::GetCName()
{
	return "CsCmdMusicChange";
}

size_t CutsceneCommandStopBGM::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + 0x30;
}

EnvLightingEntry::EnvLightingEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	setting = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	startFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	endFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	unused0 = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	unused1 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 8);
	unused2 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 12);
	unused3 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 16);
	unused4 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 20);
	unused5 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 24);
	unused6 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 28);
	unused7 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 32);
}

CutsceneCommandEnvLighting::CutsceneCommandEnvLighting(const std::vector<uint8_t>& rawData,
                                                       uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex + 0);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new EnvLightingEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandEnvLighting::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_LIGHTING_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"\t\tCS_LIGHTING(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),\n", entries[i]->setting,
			entries[i]->startFrame, entries[i]->endFrame, entries[i]->unused0, entries[i]->unused1,
			entries[i]->unused2, entries[i]->unused3, entries[i]->unused4, entries[i]->unused5,
			entries[i]->unused6, entries[i]->unused7);
	}

	return result;
}

std::string CutsceneCommandEnvLighting::GetCName()
{
	return "CsCmdEnvLighting";
}

size_t CutsceneCommandEnvLighting::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + (0x30 * entries.size());
}

Unknown9Entry::Unknown9Entry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	base = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	startFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	endFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	unk2 = rawData[rawDataIndex + 6];
	unk3 = rawData[rawDataIndex + 7];
	unk4 = rawData[rawDataIndex + 8];
	unused0 = rawData[rawDataIndex + 10];
	unused1 = rawData[rawDataIndex + 11];
	;
}

CutsceneCommandUnknown9::CutsceneCommandUnknown9(const std::vector<uint8_t>& rawData,
                                                 uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new Unknown9Entry(rawData, rawDataIndex));
		rawDataIndex += 0x0C;
	}
}

std::string CutsceneCommandUnknown9::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_CMD_09_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf("\t\tCS_CMD_09(%i, %i, %i, %i, %i, %i, %i, %i),\n",
		                                entries[i]->base, entries[i]->startFrame,
		                                entries[i]->endFrame, entries[i]->unk2, entries[i]->unk3,
		                                entries[i]->unk4, entries[i]->unused0, entries[i]->unused1);
	}

	return result;
}

std::string CutsceneCommandUnknown9::GetCName()
{
	return "CsCmdUnknown9";
}

size_t CutsceneCommandUnknown9::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + (entries.size() * 12);
}

UnkEntry::UnkEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	unused0 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 0);
	unused1 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
	unused2 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 8);
	unused3 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 12);
	unused4 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 16);
	unused5 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 20);
	unused6 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 24);
	unused7 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 28);
	unused8 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 32);
	unused9 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 36);
	unused10 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 40);
	unused11 = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex + 44);
}

CutsceneCommandUnknown::CutsceneCommandUnknown(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new UnkEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandUnknown::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_UNK_DATA_LIST(0x%02X, %i),\n", commandID, entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"    CS_UNK_DATA(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),\n",
			entries[i]->unused0, entries[i]->unused1, entries[i]->unused2, entries[i]->unused3,
			entries[i]->unused4, entries[i]->unused5, entries[i]->unused6, entries[i]->unused7,
			entries[i]->unused8, entries[i]->unused9, entries[i]->unused10, entries[i]->unused11);
	}

	return result;
}

std::string CutsceneCommandUnknown::GetCName()
{
	return "CsCmdUnknown1A";
}

size_t CutsceneCommandUnknown::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + (entries.size() * 0x30);
}

DayTimeEntry::DayTimeEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	base = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	startFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	endFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	hour = rawData[rawDataIndex + 6];
	minute = rawData[rawDataIndex + 7];
	unused = rawData[rawDataIndex + 8];
}

CutsceneCommandDayTime::CutsceneCommandDayTime(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new DayTimeEntry(rawData, rawDataIndex));
		rawDataIndex += 12;
	}
}

std::string CutsceneCommandDayTime::GetCName()
{
	return "CsCmdDayTime";
}

std::string CutsceneCommandDayTime::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_TIME_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"        CS_TIME(%i, %i, %i, %i, %i, %i),\n", entries[i]->base, entries[i]->startFrame,
			entries[i]->endFrame, entries[i]->hour, entries[i]->minute, entries[i]->unused);
	}

	return result;
}

size_t CutsceneCommandDayTime::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + (entries.size() * 12);
}

TextboxEntry::TextboxEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	base = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	startFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	endFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	type = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	textID1 = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	textID2 = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
}

CutsceneCommandTextbox::CutsceneCommandTextbox(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new TextboxEntry(rawData, rawDataIndex));
		rawDataIndex += 12;
	}
}

std::string CutsceneCommandTextbox::GetCName()
{
	return "CsCmdTextbox";
}

std::string CutsceneCommandTextbox::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_TEXT_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		if (entries[i]->base == 0xFFFF)
		{
			result += StringHelper::Sprintf("        CS_TEXT_NONE(%i, %i),\n",
			                                entries[i]->startFrame, entries[i]->endFrame);
		}
		else
		{
			result += StringHelper::Sprintf(
				"        CS_TEXT_DISPLAY_TEXTBOX(%i, %i, %i, %i, %i, %i),\n", entries[i]->base,
				entries[i]->startFrame, entries[i]->endFrame, entries[i]->type, entries[i]->textID1,
				entries[i]->textID2);
		}
	}

	return result;
}

size_t CutsceneCommandTextbox::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + (entries.size() * 12);
}

ActorAction::ActorAction(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	const uint8_t* data = rawData.data();

	action = (uint16_t)BitConverter::ToInt16BE(data, rawDataIndex + 0);
	startFrame = (uint16_t)BitConverter::ToInt16BE(data, rawDataIndex + 2);
	endFrame = (uint16_t)BitConverter::ToInt16BE(data, rawDataIndex + 4);
	rotX = (uint16_t)BitConverter::ToInt16BE(data, rawDataIndex + 6);
	rotY = (uint16_t)BitConverter::ToInt16BE(data, rawDataIndex + 8);
	rotZ = (uint16_t)BitConverter::ToInt16BE(data, rawDataIndex + 10);
	startPosX = BitConverter::ToInt32BE(data, rawDataIndex + 12);
	startPosY = BitConverter::ToInt32BE(data, rawDataIndex + 16);
	startPosZ = BitConverter::ToInt32BE(data, rawDataIndex + 20);
	endPosX = BitConverter::ToInt32BE(data, rawDataIndex + 24);
	endPosY = BitConverter::ToInt32BE(data, rawDataIndex + 28);
	endPosZ = BitConverter::ToInt32BE(data, rawDataIndex + 32);
	normalX = BitConverter::ToFloatBE(data, rawDataIndex + 36);
	normalY = BitConverter::ToFloatBE(data, rawDataIndex + 40);
	normalZ = BitConverter::ToFloatBE(data, rawDataIndex + 44);
}

CutsceneCommandActorAction::CutsceneCommandActorAction(const std::vector<uint8_t>& rawData,
                                                       uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new ActorAction(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandActorAction::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;
	std::string subCommand;

	if (commandID == 10)
	{
		result += StringHelper::Sprintf("CS_PLAYER_ACTION_LIST(%i),\n", entries.size());
		subCommand = "CS_PLAYER_ACTION";
	}
	else
	{
		result += StringHelper::Sprintf("CS_NPC_ACTION_LIST(%i, %i),\n", commandID, entries.size());
		subCommand = "CS_NPC_ACTION";
	}

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"\t\t%s(0x%04X, %i, %i, 0x%04X, 0x%04X, 0x%04X, %i, %i, %i, %i, %i, %i, %.11ef, "
			"%.11ef, %.11ef),\n",
			subCommand.c_str(), entries[i]->action, entries[i]->startFrame, entries[i]->endFrame,
			entries[i]->rotX, entries[i]->rotY, entries[i]->rotZ, entries[i]->startPosX,
			entries[i]->startPosY, entries[i]->startPosZ, entries[i]->endPosX, entries[i]->endPosY,
			entries[i]->endPosZ, entries[i]->normalX, entries[i]->normalY, entries[i]->normalZ);
	}

	return result;
}

std::string CutsceneCommandActorAction::GetCName()
{
	return "CsCmdBase";
}

size_t CutsceneCommandActorAction::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + (entries.size() * 0x30);
}

CutsceneCommandTerminator::CutsceneCommandTerminator(const std::vector<uint8_t>& rawData,
                                                     uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	base = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	startFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	endFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	unknown = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 6);  // endFrame duplicate
}

std::string CutsceneCommandTerminator::GetCName()
{
	return "CsCmdBase";
}

std::string CutsceneCommandTerminator::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_TERMINATOR(0x%04X, %i, %i),\n", base, startFrame, endFrame);

	return result;
}

size_t CutsceneCommandTerminator::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + 8;
}

CutsceneCommandEnd::CutsceneCommandEnd(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	base = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	startFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	endFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
}

std::string CutsceneCommandEnd::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_END(),\n");

	return result;
}

std::string CutsceneCommandEnd::GetCName()
{
	return "CsCmdBase";
}

size_t CutsceneCommandEnd::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + 6;
}

SpecialActionEntry::SpecialActionEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	const uint8_t* data = rawData.data();

	base = BitConverter::ToUInt16BE(data, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(data, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(data, rawDataIndex + 4);
	unused0 = BitConverter::ToUInt16BE(data, rawDataIndex + 6);
	unused1 = BitConverter::ToUInt32BE(data, rawDataIndex + 8);
	unused2 = BitConverter::ToUInt32BE(data, rawDataIndex + 12);
	unused3 = BitConverter::ToUInt32BE(data, rawDataIndex + 16);
	unused4 = BitConverter::ToUInt32BE(data, rawDataIndex + 20);
	unused5 = BitConverter::ToUInt32BE(data, rawDataIndex + 24);
	unused6 = BitConverter::ToUInt32BE(data, rawDataIndex + 28);
	unused7 = BitConverter::ToUInt32BE(data, rawDataIndex + 32);
	unused8 = BitConverter::ToUInt32BE(data, rawDataIndex + 36);
	unused9 = BitConverter::ToUInt32BE(data, rawDataIndex + 40);
	unused10 = BitConverter::ToUInt32BE(data, rawDataIndex + 44);
}

CutsceneCommandSpecialAction::CutsceneCommandSpecialAction(const std::vector<uint8_t>& rawData,
                                                           uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex + 0);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new SpecialActionEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandSpecialAction::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	std::string result;

	result += StringHelper::Sprintf("CS_MISC_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"\t\tCS_MISC(0x%04X, %i, %i, 0x%04X, 0x%04X, 0x%04X, %i, %i, %i, %i, %i, %i, %i, "
			"%i),\n",
			entries[i]->base, entries[i]->startFrame, entries[i]->endFrame, entries[i]->unused0,
			entries[i]->unused1, entries[i]->unused2, entries[i]->unused3, entries[i]->unused4,
			entries[i]->unused5, entries[i]->unused6, entries[i]->unused7, entries[i]->unused8,
			entries[i]->unused9, entries[i]->unused10);
	}

	return result;
}

std::string CutsceneCommandSpecialAction::GetCName()
{
	return "CsCmdBase";
}

size_t CutsceneCommandSpecialAction::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + (0x30 * entries.size());
}

CutsceneCommandNop::CutsceneCommandNop(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	base = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	startFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	endFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
}

std::string CutsceneCommandNop::GetCName()
{
	return "CsCmdBase";
}

size_t CutsceneCommandNop::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + 6;
}

CutsceneCommandSceneTransFX::CutsceneCommandSceneTransFX(const std::vector<uint8_t>& rawData,
                                                         uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	base = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	startFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	endFrame = (uint16_t)BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
}

std::string CutsceneCommandSceneTransFX::GenerateSourceCode([[maybe_unused]] uint32_t baseAddress)
{
	return StringHelper::Sprintf("CS_SCENE_TRANS_FX(%i, %i, %i),\n", base, startFrame, endFrame);
}

std::string CutsceneCommandSceneTransFX::GetCName()
{
	return "CsCmdBase";
}

size_t CutsceneCommandSceneTransFX::GetCommandSize()
{
	return CutsceneCommand::GetCommandSize() + 8;
}

ZCutsceneBase::ZCutsceneBase(ZFile* nParent) : ZResource(nParent)
{
}

Declaration* ZCutsceneBase::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (auxName == "")
		auxName = GetDefaultName(prefix);

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), auxName, 0, bodyStr);
	decl->staticConf = staticConf;
	return decl;
}

std::string ZCutsceneBase::GetSourceTypeName() const
{
	return "CutsceneData";
}
