#include "CutsceneExporter.h"
#include <Resource.h>

void OTRExporter_Cutscene::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer)
{
	ZCutscene* cs = (ZCutscene*)res;

	WriteHeader(cs, outPath, writer, Ship::ResourceType::Cutscene);

	//writer->Write((uint32_t)cs->commands.size() + 2 + 2);
	writer->Write((uint32_t)0);

	int currentStream = writer->GetBaseAddress();

	writer->Write(CS_BEGIN_CUTSCENE(cs->numCommands, cs->endFrame));

	for (size_t i = 0; i < cs->commands.size(); i++)
	{
		switch (cs->commands[i]->commandID)
		{
		case (uint32_t)CutsceneCommands::SetCameraPos:
		{
			CutsceneCommandSetCameraPos* cmdCamPos = (CutsceneCommandSetCameraPos*)cs->commands[i];

			writer->Write(CS_CMD_CAM_EYE);
			writer->Write(CMD_HH(0x0001, ((CutsceneCommandSetCameraPos*)cs->commands[i])->startFrame));
			writer->Write(CMD_HH(cmdCamPos->endFrame, 0x0000));

			for (auto& e : ((CutsceneCommandSetCameraPos*)cs->commands[i])->entries)
			{
				writer->Write(CMD_BBH(e->continueFlag, e->cameraRoll, e->nextPointFrame));
				writer->Write(e->viewAngle);
				writer->Write(CMD_HH(e->posX, e->posY));
				writer->Write(CMD_HH(e->posZ, e->unused));
			}
		}
		break;
		case (uint32_t)CutsceneCommands::SetCameraFocus:
		{
			CutsceneCommandSetCameraPos* cmdCamPos = (CutsceneCommandSetCameraPos*)cs->commands[i];

			writer->Write(CS_CMD_CAM_AT);
			writer->Write(CMD_HH(0x0001, cmdCamPos->startFrame));
			writer->Write(CMD_HH(cmdCamPos->endFrame, 0x0000));

			for (auto& e : ((CutsceneCommandSetCameraPos*)cs->commands[i])->entries)
			{
				writer->Write(CMD_BBH(e->continueFlag, e->cameraRoll, e->nextPointFrame));
				writer->Write(e->viewAngle);
				writer->Write(CMD_HH(e->posX, e->posY));
				writer->Write(CMD_HH(e->posZ, e->unused));
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SpecialAction:
		{
			writer->Write(CS_CMD_MISC);
			writer->Write((uint32_t)CMD_W(((CutsceneCommandSpecialAction*)cs->commands[i])->entries.size()));
			for (auto& e : ((CutsceneCommandSpecialAction*)cs->commands[i])->entries) //All in OOT seem to only have 1 entry
			{
				writer->Write(CMD_HH(e->base, e->startFrame));
				writer->Write(CMD_HH(e->endFrame, e->unused0));
				writer->Write(CMD_W(e->unused1));
				writer->Write(CMD_W(e->unused2));
				writer->Write(CMD_W(e->unused3));
				writer->Write(CMD_W(e->unused4));
				writer->Write(CMD_W(e->unused5));
				writer->Write(CMD_W(e->unused6));
				writer->Write(CMD_W(e->unused7));
				writer->Write(CMD_W(e->unused8));
				writer->Write(CMD_W(e->unused9));
				writer->Write(CMD_W(e->unused10));
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetLighting:
		{
			writer->Write(CS_CMD_SET_LIGHTING);
			writer->Write((uint32_t)CMD_W(((CutsceneCommandEnvLighting*)cs->commands[i])->entries.size()));
			for (auto& e : ((CutsceneCommandEnvLighting*)cs->commands[i])->entries)
			{
				writer->Write(CMD_HH(e->setting, e->startFrame));
				writer->Write(CMD_HH(e->endFrame, e->unused0));
				writer->Write(CMD_W(e->unused1));
				writer->Write(CMD_W(e->unused2));
				writer->Write(CMD_W(e->unused3));
				writer->Write(CMD_W(e->unused4));
				writer->Write(CMD_W(e->unused5));
				writer->Write(CMD_W(e->unused6));
				writer->Write(CMD_W(e->unused7));
				writer->Write((uint32_t)0x0);
				writer->Write((uint32_t)0x0);
				writer->Write((uint32_t)0x0);
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetCameraPosLink:
		{
			CutsceneCommandSetCameraPos* cmdCamPos = (CutsceneCommandSetCameraPos*)cs->commands[i];

			writer->Write(CS_CMD_CAM_EYE_REL_TO_PLAYER);
			writer->Write(CMD_HH(0x0001, ((CutsceneCommandSetCameraPos*)cs->commands[i])->startFrame));
			writer->Write(CMD_HH(cmdCamPos->endFrame, 0x0000));

			for (auto& e : ((CutsceneCommandSetCameraPos*)cs->commands[i])->entries)
			{
				writer->Write(CMD_BBH(e->continueFlag, e->cameraRoll, e->nextPointFrame));
				writer->Write(e->viewAngle);
				writer->Write(CMD_HH(e->posX, e->posY));
				writer->Write(CMD_HH(e->posZ, e->unused));
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetCameraFocusLink:
		{
			CutsceneCommandSetCameraPos* cmdCamPos = (CutsceneCommandSetCameraPos*)cs->commands[i];

			writer->Write(CS_CMD_CAM_AT_REL_TO_PLAYER);
			writer->Write(CMD_HH(0x0001, ((CutsceneCommandSetCameraPos*)cs->commands[i])->startFrame));
			writer->Write(CMD_HH(cmdCamPos->endFrame, 0x0000));

			for (auto& e : ((CutsceneCommandSetCameraPos*)cs->commands[i])->entries)
			{
				writer->Write(CMD_BBH(e->continueFlag, e->cameraRoll, e->nextPointFrame));
				writer->Write(e->viewAngle);
				writer->Write(CMD_HH(e->posX, e->posY));
				writer->Write(CMD_HH(e->posZ, e->unused));
			}
			break;
		}

		case (uint32_t)CutsceneCommands::Cmd07: // Not used in OOT
			break;
		case (uint32_t)CutsceneCommands::Cmd08: // Not used in OOT
			break;

		case (uint32_t)CutsceneCommands::Cmd09:
		{
			writer->Write(CS_CMD_09);
			writer->Write((uint32_t)CMD_W(((CutsceneCommandUnknown9*)cs->commands[i])->entries.size()));

			for (auto& e : ((CutsceneCommandUnknown9*)cs->commands[i])->entries)
			{
				writer->Write(CMD_HH(e->base, e->startFrame));
				writer->Write(CMD_HBB(e->endFrame, e->unk2, e->unk3));
				writer->Write(CMD_BBH(e->unk4, e->unused0, e->unused1));
			}
			break;
		}
		case 0x15:
		case (uint32_t)CutsceneCommands::Unknown:
		{
			CutsceneCommandUnknown* cmdUnk = (CutsceneCommandUnknown*)cs->commands[i];
			writer->Write((uint32_t)cs->commands[i]->commandID);
			writer->Write((uint32_t)cmdUnk->entries.size());

			for (auto e : cmdUnk->entries)
			{
				writer->Write(CMD_W(e->unused0));
				writer->Write(CMD_W(e->unused1));
				writer->Write(CMD_W(e->unused2));
				writer->Write(CMD_W(e->unused3));
				writer->Write(CMD_W(e->unused4));
				writer->Write(CMD_W(e->unused5));
				writer->Write(CMD_W(e->unused6));
				writer->Write(CMD_W(e->unused7));
				writer->Write(CMD_W(e->unused8));
				writer->Write(CMD_W(e->unused9));
				writer->Write(CMD_W(e->unused10));
				writer->Write(CMD_W(e->unused11));
			}
		}
		break;
		case (uint32_t)CutsceneCommands::Textbox:
		{
			writer->Write(CS_CMD_TEXTBOX);
			writer->Write((uint32_t)CMD_W(((CutsceneCommandTextbox*)cs->commands[i])->entries.size()));

			for (auto& e : ((CutsceneCommandTextbox*)cs->commands[i])->entries)
			{
				if (e->base == 0xFFFF) // CS_TEXT_NONE
				{
					writer->Write(CMD_HH(0xFFFF, e->startFrame));
					writer->Write(CMD_HH(e->endFrame, 0xFFFF));
					writer->Write(CMD_HH(0xFFFF, 0xFFFF));
				}
				else // CS_TEXT_DISPLAY_TEXTBOX
				{
					writer->Write(CMD_HH(e->base, e->startFrame));
					writer->Write(CMD_HH(e->endFrame, e->type));
					writer->Write(CMD_HH(e->textID1, e->textID2));
				}
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetActorAction0:
		case (uint32_t)CutsceneCommands::SetActorAction1:
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
		case (uint32_t)CutsceneCommands::SetActorAction2:
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
		case (uint32_t)CutsceneCommands::SetActorAction3:
		case 36:
		case 41:
		case 50:
		case 67:
		case 69:
		case 72:
		case 74:
		case 81:
		case 106:
		case 117:
		case 121:
		case 126:
		case 132:
		case (uint32_t)CutsceneCommands::SetActorAction4:
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
		case (uint32_t)CutsceneCommands::SetActorAction5:
		case 38:
		case 43:
		case 47:
		case 54:
		case 79:
		case 83:
		case 128:
		case 135:
		case (uint32_t)CutsceneCommands::SetActorAction6:
		case 55:
		case 77:
		case 84:
		case 90:
		case 129:
		case 136:
		case (uint32_t)CutsceneCommands::SetActorAction7:
		case 52:
		case 57:
		case 58:
		case 88:
		case 115:
		case 130:
		case 137:
		case (uint32_t)CutsceneCommands::SetActorAction8:
		case 60:
		case 89:
		case 111:
		case 114:
		case 134:
		case 142:
		case (uint32_t)CutsceneCommands::SetActorAction9:
		case (uint32_t)CutsceneCommands::SetActorAction10:
		{
			writer->Write((uint32_t)(CutsceneCommands)cs->commands[i]->commandID);
			writer->Write((uint32_t)CMD_W(((CutsceneCommandActorAction*)cs->commands[i])->entries.size()));

			for (auto& actorAct : ((CutsceneCommandActorAction*)cs->commands[i])->entries)
			{
				writer->Write(CMD_HH(actorAct->action, actorAct->startFrame));
				writer->Write(CMD_HH(actorAct->endFrame, actorAct->rotX));
				writer->Write(CMD_HH(actorAct->rotY, actorAct->rotZ));
				writer->Write(CMD_W(actorAct->startPosX));
				writer->Write(CMD_W(actorAct->startPosY));
				writer->Write(CMD_W(actorAct->startPosZ));
				writer->Write(CMD_W(actorAct->endPosX));
				writer->Write(CMD_W(actorAct->endPosY));
				writer->Write(CMD_W(actorAct->endPosZ));
				writer->Write(CMD_W(actorAct->normalX));
				writer->Write(CMD_W(actorAct->normalY));
				writer->Write(CMD_W(actorAct->normalZ));
			}

			break;
		}
		case (uint32_t)CutsceneCommands::SetSceneTransFX:
		{
			CutsceneCommandSceneTransFX* cmdTFX = (CutsceneCommandSceneTransFX*)cs->commands[i];

			writer->Write(CS_CMD_SCENE_TRANS_FX);
			writer->Write((uint32_t)1);
			writer->Write(CMD_HH((((CutsceneCommandSceneTransFX*)cs->commands[i])->base), ((CutsceneCommandSceneTransFX*)cs->commands[i])->startFrame));
			writer->Write(CMD_HH((((CutsceneCommandSceneTransFX*)cs->commands[i])->endFrame), ((CutsceneCommandSceneTransFX*)cs->commands[i])->endFrame));
			break;
		}
		case (uint32_t)CutsceneCommands::Nop: //Not used in OOT
			break;
		case (uint32_t)CutsceneCommands::PlayBGM:
		{
			writer->Write(CS_CMD_PLAYBGM);
			writer->Write((uint32_t)CMD_W(((CutsceneCommandPlayBGM*)cs->commands[i])->entries.size()));

			for (auto& e : ((CutsceneCommandPlayBGM*)cs->commands[i])->entries)
			{
				writer->Write(CMD_HH(e->sequence, e->startFrame));
				writer->Write(CMD_HH(e->endFrame, e->unknown0));
				writer->Write(CMD_W(e->unknown1));
				writer->Write(CMD_W(e->unknown2));
				writer->Write(CMD_W(e->unknown3));
				writer->Write(CMD_W(e->unknown4));
				writer->Write(CMD_W(e->unknown5));
				writer->Write(CMD_W(e->unknown6));
				writer->Write(CMD_W(e->unknown7));
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
			}
			break;
		}
		case (uint32_t)CutsceneCommands::StopBGM:
		{
			writer->Write(CS_CMD_STOPBGM);
			writer->Write((uint32_t)CMD_W(((CutsceneCommandStopBGM*)cs->commands[i])->entries.size()));

			for (auto& e : ((CutsceneCommandStopBGM*)cs->commands[i])->entries)
			{
				writer->Write(CMD_HH(e->sequence, e->startFrame));
				writer->Write(CMD_HH(e->endFrame, e->unknown0));
				writer->Write(CMD_W(e->unknown1));
				writer->Write(CMD_W(e->unknown2));
				writer->Write(CMD_W(e->unknown3));
				writer->Write(CMD_W(e->unknown4));
				writer->Write(CMD_W(e->unknown5));
				writer->Write(CMD_W(e->unknown6));
				writer->Write(CMD_W(e->unknown7));
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
			}
			break;
		}
		case (uint32_t)CutsceneCommands::FadeBGM:
		{
			writer->Write(CS_CMD_FADEBGM);
			writer->Write((uint32_t)CMD_W(((CutsceneCommandFadeBGM*)cs->commands[i])->entries.size()));

			for (auto& e : ((CutsceneCommandFadeBGM*)cs->commands[i])->entries)
			{
				writer->Write(CMD_HH(e->base, e->startFrame));
				writer->Write(CMD_HH(e->endFrame, e->unknown0));
				writer->Write(CMD_W(e->unknown1));
				writer->Write(CMD_W(e->unknown2));
				writer->Write(CMD_W(e->unknown3));
				writer->Write(CMD_W(e->unknown4));
				writer->Write(CMD_W(e->unknown5));
				writer->Write(CMD_W(e->unknown6));
				writer->Write(CMD_W(e->unknown7));
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetTime:
		{
			writer->Write(CS_CMD_SETTIME);
			writer->Write((uint32_t)CMD_W(((CutsceneCommandDayTime*)cs->commands[i])->entries.size()));

			for (auto& e : ((CutsceneCommandDayTime*)cs->commands[i])->entries)
			{
				writer->Write(CMD_HH(e->base, e->startFrame));
				writer->Write(CMD_HBB(e->endFrame, e->hour, e->minute));
				writer->Write((uint32_t)CMD_W(e->unused));
			}
			break;
		}
		case (uint32_t)CutsceneCommands::Terminator:
		{
			writer->Write(CS_CMD_TERMINATOR);
			writer->Write((uint32_t)1);
			writer->Write(CMD_HH(((CutsceneCommandTerminator*)cs->commands[i])->base, ((CutsceneCommandTerminator*)cs->commands[i])->startFrame));
			writer->Write(CMD_HH(((CutsceneCommandTerminator*)cs->commands[i])->endFrame, ((CutsceneCommandTerminator*)cs->commands[i])->endFrame));
			break;
		}
		default:
		{
			//writer->Write((uint32_t)cs->commands[i]->commandID);
			printf("Undefined CS Opcode: %04X\n", cs->commands[i]->commandID);
		}
		break;
		}
	}

	//CS_END
	writer->Write(0xFFFFFFFF);
	writer->Write((uint32_t)0);

	int endStream = writer->GetBaseAddress();
	writer->Seek(currentStream - 4, SeekOffsetType::Start);
	writer->Write((uint32_t)((endStream - currentStream) / 4));
	writer->Seek(endStream, SeekOffsetType::Start);
}