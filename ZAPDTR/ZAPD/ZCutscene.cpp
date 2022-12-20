#include "ZCutscene.h"

#include <cassert>

#include "Globals.h"
#include "OtherStructs/CutsceneMM_Commands.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZResource.h"

REGISTER_ZFILENODE(Cutscene, ZCutscene);

ZCutscene::ZCutscene(ZFile* nParent) : ZResource(nParent)
{
	genOTRDef = true;
}

ZCutscene::~ZCutscene()
{
	for (CutsceneCommand* cmd : commands)
		delete cmd;
}

std::string ZCutscene::GetBodySourceCode() const
{
	std::string output = "";

	output += StringHelper::Sprintf("    CS_BEGIN_CUTSCENE(%i, %i),\n", commands.size(), endFrame);

	for (size_t i = 0; i < commands.size(); i++)
	{
		CutsceneCommand* cmd = commands[i];
		output += "    " + cmd->GenerateSourceCode();
	}

	output += StringHelper::Sprintf("    CS_END(),", commands.size(), endFrame);

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
	}

	// End
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		size += 4;
	}
	else
	{
		size += 8;
	}

	return size;
}

void ZCutscene::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();

	numCommands = BitConverter::ToInt32BE(rawData, rawDataIndex + 0);
	commands = std::vector<CutsceneCommand*>();

	endFrame = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
	offset_t currentPtr = rawDataIndex + 8;
	commands.reserve(numCommands);
	for (int32_t i = 0; i < numCommands; i++)
	{
		uint32_t id = BitConverter::ToUInt32BE(rawData, currentPtr);

		if (id == 0xFFFFFFFF)
			break;

		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		{
			printf("Cutscene Command: 0x%X (%i)\n", id, id);
		}

		currentPtr += 4;

		CutsceneCommand* cmd = nullptr;

		if (Globals::Instance->game == ZGame::MM_RETAIL)
		{
			cmd = GetCommandMM(id, currentPtr);
		}
		else
		{
			cmd = GetCommandOoT(id, currentPtr);
		}

		if (cmd == nullptr)
		{
			HANDLE_WARNING_RESOURCE(
				WarningType::NotImplemented, parent, this, rawDataIndex,
				StringHelper::Sprintf("Cutscene command not implemented"),
				StringHelper::Sprintf("Command ID: 0x%X\nIndex: %d\ncurrentPtr-rawDataIndex: 0x%X",
			                          id, i, currentPtr - rawDataIndex));
			cmd = new CutsceneMMCommand_NonImplemented(rawData, currentPtr);
		}

		assert(cmd != nullptr);

		cmd->commandIndex = i;
		cmd->SetCommandID(id);
		size_t commmandSize = cmd->GetCommandSize();
		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		{
			printf("\t Command size: 0x%zX (%zu)\n", commmandSize, commmandSize);
		}
		currentPtr += commmandSize - 4;

		commands.push_back(cmd);
	}
}

CutsceneCommand* ZCutscene::GetCommandOoT(uint32_t id, offset_t currentPtr) const
{
	CutsceneCommands cmdID = static_cast<CutsceneCommands>(id);

	const auto& rawData = parent->GetRawData();

	switch (id)
	{
	case 10:  // CutsceneCommands::SetPlayerAction
	case 15:  // CutsceneCommands::SetActorAction1
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
	case 14:  // CutsceneCommands::SetActorAction2
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
	case 25:  // CutsceneCommands::SetActorAction3
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
	case 29:  // CutsceneCommands::SetActorAction4
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
	case 30:  // CutsceneCommands::SetActorAction5
	case 38:
	case 43:
	case 47:
	case 54:
	case 79:
	case 83:
	case 128:
	case 135:
	case 44:  // CutsceneCommands::SetActorAction6
	case 55:
	case 77:
	case 84:
	case 90:
	case 129:
	case 136:
	case 31:  // CutsceneCommands::SetActorAction7
	case 52:
	case 57:
	case 58:
	case 88:
	case 115:
	case 130:
	case 137:
	case 49:  // CutsceneCommands::SetActorAction8
	case 60:
	case 89:
	case 111:
	case 114:
	case 134:
	case 142:
	case 62:   // CutsceneCommands::SetActorAction9
	case 143:  // CutsceneCommands::SetActorAction10
		return new CutsceneCommand_ActorAction(rawData, currentPtr);

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
		return new CutsceneCommand_GenericCmd(rawData, currentPtr, cmdID);
	}

	switch (cmdID)
	{
	case CutsceneCommands::Misc:
	case CutsceneCommands::SetLighting:
	case CutsceneCommands::PlayBGM:
	case CutsceneCommands::StopBGM:
	case CutsceneCommands::FadeBGM:
		return new CutsceneCommand_GenericCmd(rawData, currentPtr, cmdID);

	case CutsceneCommands::SetCameraPos:
	case CutsceneCommands::SetCameraFocus:
	case CutsceneCommands::SetCameraPosLink:
	case CutsceneCommands::SetCameraFocusLink:
		return new CutsceneCommandSetCameraPos(rawData, currentPtr);

	case CutsceneCommands::Cmd07:
		break;
	case CutsceneCommands::Cmd08:
		break;
	case CutsceneCommands::Cmd09:
		return new CutsceneCommand_Rumble(rawData, currentPtr);
	case CutsceneCommands::Textbox:
		return new CutsceneCommand_TextBox(rawData, currentPtr);

	case CutsceneCommands::SetPlayerAction:
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
		break;

	case CutsceneCommands::SetSceneTransFX:
		return new CutsceneCommandSceneTransFX(rawData, currentPtr);

	case CutsceneCommands::SetTime:
		return new CutsceneCommand_SetTime(rawData, currentPtr);
	case CutsceneCommands::Terminator:
		return new CutsceneCommand_Terminator(rawData, currentPtr);
	}

	return nullptr;
}

CutsceneCommand* ZCutscene::GetCommandMM(uint32_t id, offset_t currentPtr) const
{
	CutsceneMMCommands cmdID = static_cast<CutsceneMMCommands>(id);

	const auto& rawData = parent->GetRawData();

	if (((id >= 100) && (id < 150)) || (id == 201) || ((id >= 450) && (id < 600)))
	{
		return new CutsceneCommand_ActorAction(rawData, currentPtr);
	}

	switch (cmdID)
	{
	case CutsceneMMCommands::CS_CMD_MISC:
	case CutsceneMMCommands::CS_CMD_SET_LIGHTING:
	case CutsceneMMCommands::CS_CMD_SCENE_TRANS_FX:
	case CutsceneMMCommands::CS_CMD_MOTIONBLUR:
	case CutsceneMMCommands::CS_CMD_GIVETATL:
	case CutsceneMMCommands::CS_CMD_PLAYSEQ:
	case CutsceneMMCommands::CS_CMD_130:
	case CutsceneMMCommands::CS_CMD_131:
	case CutsceneMMCommands::CS_CMD_132:
	case CutsceneMMCommands::CS_CMD_STOPSEQ:
	case CutsceneMMCommands::CS_CMD_PLAYAMBIENCE:
	case CutsceneMMCommands::CS_CMD_FADEAMBIENCE:
	case CutsceneMMCommands::CS_CMD_TERMINATOR:
	case CutsceneMMCommands::CS_CMD_CHOOSE_CREDITS_SCENES:

	case CutsceneMMCommands::CS_CMD_UNK_FA:
	case CutsceneMMCommands::CS_CMD_UNK_FE:
	case CutsceneMMCommands::CS_CMD_UNK_FF:
	case CutsceneMMCommands::CS_CMD_UNK_100:
	case CutsceneMMCommands::CS_CMD_UNK_101:
	case CutsceneMMCommands::CS_CMD_UNK_102:
	case CutsceneMMCommands::CS_CMD_UNK_103:
	case CutsceneMMCommands::CS_CMD_UNK_104:
	case CutsceneMMCommands::CS_CMD_UNK_105:
	case CutsceneMMCommands::CS_CMD_UNK_108:
	case CutsceneMMCommands::CS_CMD_UNK_109:
	case CutsceneMMCommands::CS_CMD_UNK_12D:
		return new CutsceneMMCommand_GenericCmd(rawData, currentPtr, cmdID);

	case CutsceneMMCommands::CS_CMD_TEXTBOX:
		return new CutsceneCommand_TextBox(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_CAMERA:
		return new CutsceneMMCommand_Camera(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_FADESCREEN:
		return new CutsceneMMCommand_FadeScreen(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_FADESEQ:
		return new CutsceneMMCommand_FadeSeq(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_SETTIME:
		return new CutsceneCommand_SetTime(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_SET_PLAYER_ACTION:
		return new CutsceneCommand_ActorAction(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_RUMBLE:
		return new CutsceneCommand_Rumble(rawData, currentPtr);
	}

	return nullptr;
}

Declaration* ZCutscene::DeclareVar(const std::string& prefix, const std::string& bodyStr)
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

std::string ZCutscene::GetSourceTypeName() const
{
	return "CutsceneData";
}

ZResourceType ZCutscene::GetResourceType() const
{
	return ZResourceType::Cutscene;
}
