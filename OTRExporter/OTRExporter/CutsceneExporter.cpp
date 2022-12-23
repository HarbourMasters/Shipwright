#include "CutsceneExporter.h"
#include <libultraship/bridge.h>

void OTRExporter_Cutscene::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer)
{
	ZCutscene* cs = (ZCutscene*)res;

	WriteHeader(cs, outPath, writer, Ship::ResourceType::SOH_Cutscene);

	writer->Write((uint32_t)0);

	const auto currentStream = writer->GetBaseAddress();

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

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneCameraPoint* point = (CutsceneCameraPoint*)e;
				writer->Write(CMD_BBH(point->continueFlag, point->cameraRoll, point->nextPointFrame));
				writer->Write(point->viewAngle);
				writer->Write(CMD_HH(point->posX, point->posY));
				writer->Write(CMD_HH(point->posZ, point->unused));
			}
		}
		break;
		case (uint32_t)CutsceneCommands::SetCameraFocus:
		{
			CutsceneCommandSetCameraPos* cmdCamPos = (CutsceneCommandSetCameraPos*)cs->commands[i];

			writer->Write(CS_CMD_CAM_AT);
			writer->Write(CMD_HH(0x0001, cmdCamPos->startFrame));
			writer->Write(CMD_HH(cmdCamPos->endFrame, 0x0000));

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneCameraPoint* point = (CutsceneCameraPoint*)e;
				writer->Write(CMD_BBH(point->continueFlag, point->cameraRoll, point->nextPointFrame));
				writer->Write(point->viewAngle);
				writer->Write(CMD_HH(point->posX, point->posY));
				writer->Write(CMD_HH(point->posZ, point->unused));
			}
			break;
		}
		case (uint32_t)CutsceneCommands::Misc:
		{
			writer->Write(CS_CMD_MISC);
			writer->Write((uint32_t)CMD_W((cs->commands[i])->entries.size()));
			for (const auto& e : cs->commands[i]->entries) //All in OOT seem to only have 1 entry
			{
				CutsceneSubCommandEntry_GenericCmd* cmd = (CutsceneSubCommandEntry_GenericCmd*)e;
				writer->Write(CMD_HH(cmd->base, cmd->startFrame));
				writer->Write(CMD_HH(cmd->endFrame, cmd->pad));
				writer->Write(CMD_W(cmd->unused1));
				writer->Write(CMD_W(cmd->unused2));
				writer->Write(CMD_W(cmd->unused3));
				writer->Write(CMD_W(cmd->unused4));
				writer->Write(CMD_W(cmd->unused5));
				writer->Write(CMD_W(cmd->unused6));
				writer->Write(CMD_W(cmd->unused7));
				writer->Write(CMD_W(cmd->unused8));
				writer->Write(CMD_W(cmd->unused9));
				writer->Write(CMD_W(cmd->unused10));
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetLighting:
		{
			writer->Write(CS_CMD_SET_LIGHTING);
			writer->Write((uint32_t)CMD_W((cs->commands[i])->entries.size()));
			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneSubCommandEntry_GenericCmd* cmd = (CutsceneSubCommandEntry_GenericCmd*)e;
				writer->Write(CMD_HH(cmd->base, cmd->startFrame));
				writer->Write(CMD_HH(cmd->endFrame, cmd->pad));
				writer->Write(CMD_W(cmd->unused1));
				writer->Write(CMD_W(cmd->unused2));
				writer->Write(CMD_W(cmd->unused3));
				writer->Write(CMD_W(cmd->unused4));
				writer->Write(CMD_W(cmd->unused5));
				writer->Write(CMD_W(cmd->unused6));
				writer->Write(CMD_W(cmd->unused7));
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

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneCameraPoint* point = (CutsceneCameraPoint*)e;
				writer->Write(CMD_BBH(point->continueFlag, point->cameraRoll, point->nextPointFrame));
				writer->Write(point->viewAngle);
				writer->Write(CMD_HH(point->posX, point->posY));
				writer->Write(CMD_HH(point->posZ, point->unused));
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetCameraFocusLink:
		{
			CutsceneCommandSetCameraPos* cmdCamPos = (CutsceneCommandSetCameraPos*)cs->commands[i];

			writer->Write(CS_CMD_CAM_AT_REL_TO_PLAYER);
			writer->Write(CMD_HH(0x0001, ((CutsceneCommandSetCameraPos*)cs->commands[i])->startFrame));
			writer->Write(CMD_HH(cmdCamPos->endFrame, 0x0000));

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneCameraPoint* point = (CutsceneCameraPoint*)e;
				writer->Write(CMD_BBH(point->continueFlag, point->cameraRoll, point->nextPointFrame));
				writer->Write(point->viewAngle);
				writer->Write(CMD_HH(point->posX, point->posY));
				writer->Write(CMD_HH(point->posZ, point->unused));
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
			writer->Write((uint32_t)CMD_W(((CutsceneCommand_Rumble*)cs->commands[i])->entries.size()));

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneSubCommandEntry_Rumble* r = (CutsceneSubCommandEntry_Rumble*)e;
				writer->Write(CMD_HH(r->base, r->startFrame));
				writer->Write(CMD_HBB(e->endFrame, r->unk_06, r->unk_07));
				writer->Write(CMD_BBH(r->unk_08, r->unk_09, r->unk_0A));
			}
			break;
		}
		case 0x15://Both unused in OoT
		case 0x1A://(uint32_t)CutsceneCommands::Unknown: 
		{
#if 0
			CutsceneCommandUnknown* cmdUnk = (CutsceneCommandUnknown*)cs->commands[i];
			writer->Write((uint32_t)cs->commands[i]->commandID);
			writer->Write((uint32_t)cmdUnk->entries.size());

			for (const auto e : cmdUnk->entries)
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
#endif
		}
		break;
		case (uint32_t)CutsceneCommands::Textbox:
		{
			writer->Write(CS_CMD_TEXTBOX);
			writer->Write((uint32_t)CMD_W((cs->commands[i])->entries.size()));

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneSubCommandEntry_TextBox* textBox = (CutsceneSubCommandEntry_TextBox*)e;
				if (textBox->base == 0xFFFF) // CS_TEXT_NONE
				{
					writer->Write(CMD_HH(0xFFFF, textBox->startFrame));
					writer->Write(CMD_HH(textBox->endFrame, 0xFFFF));
					writer->Write(CMD_HH(0xFFFF, 0xFFFF));
				}
				else // CS_TEXT_DISPLAY_TEXTBOX
				{
					writer->Write(CMD_HH(textBox->base, textBox->startFrame));
					writer->Write(CMD_HH(textBox->endFrame, textBox->type));
					writer->Write(CMD_HH(textBox->textId1, textBox->textId2));
				}
			}
			break;
		}
		case 10: //ActorAction0
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
			writer->Write((uint32_t)CMD_W(cs->commands[i]->entries.size()));

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneSubCommandEntry_ActorAction* actorAct = (CutsceneSubCommandEntry_ActorAction*)e;
				writer->Write(CMD_HH(actorAct->base, actorAct->startFrame));
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
		case (uint32_t)CutsceneCommands::PlayBGM:
		{
			writer->Write(CS_CMD_PLAYBGM);
			writer->Write((uint32_t)CMD_W(cs->commands[i]->entries.size()));

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneSubCommandEntry_GenericCmd* cmd = (CutsceneSubCommandEntry_GenericCmd*)e;
				writer->Write(CMD_HH(cmd->base, cmd->startFrame));
				writer->Write(CMD_HH(cmd->endFrame, cmd->pad));
				writer->Write(CMD_W(cmd->unused1));
				writer->Write(CMD_W(cmd->unused2));
				writer->Write(CMD_W(cmd->unused3));
				writer->Write(CMD_W(cmd->unused4));
				writer->Write(CMD_W(cmd->unused5));
				writer->Write(CMD_W(cmd->unused6));
				writer->Write(CMD_W(cmd->unused7));
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
			}
			break;
		}
		case (uint32_t)CutsceneCommands::StopBGM:
		{
			writer->Write(CS_CMD_STOPBGM);
			writer->Write((uint32_t)CMD_W(cs->commands[i]->entries.size()));

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneSubCommandEntry_GenericCmd* cmd = (CutsceneSubCommandEntry_GenericCmd*)e;
				writer->Write(CMD_HH(cmd->base, cmd->startFrame));
				writer->Write(CMD_HH(cmd->endFrame, cmd->pad));
				writer->Write(CMD_W(cmd->unused1));
				writer->Write(CMD_W(cmd->unused2));
				writer->Write(CMD_W(cmd->unused3));
				writer->Write(CMD_W(cmd->unused4));
				writer->Write(CMD_W(cmd->unused5));
				writer->Write(CMD_W(cmd->unused6));
				writer->Write(CMD_W(cmd->unused7));
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
			}
			break;
		}
		case (uint32_t)CutsceneCommands::FadeBGM:
		{
			writer->Write(CS_CMD_FADEBGM);
			writer->Write((uint32_t)CMD_W(cs->commands[i]->entries.size()));

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneSubCommandEntry_GenericCmd* cmd = (CutsceneSubCommandEntry_GenericCmd*)e;
				writer->Write(CMD_HH(cmd->base, cmd->startFrame));
				writer->Write(CMD_HH(cmd->endFrame, cmd->pad));
				writer->Write(CMD_W(cmd->unused1));
				writer->Write(CMD_W(cmd->unused2));
				writer->Write(CMD_W(cmd->unused3));
				writer->Write(CMD_W(cmd->unused4));
				writer->Write(CMD_W(cmd->unused5));
				writer->Write(CMD_W(cmd->unused6));
				writer->Write(CMD_W(cmd->unused7));
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
				writer->Write((uint32_t)0);
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetTime:
		{
			writer->Write(CS_CMD_SETTIME);
			writer->Write((uint32_t)CMD_W(cs->commands[i]->entries.size()));

			for (const auto& e : cs->commands[i]->entries)
			{
				CutsceneSubCommandEntry_SetTime* t = (CutsceneSubCommandEntry_SetTime*)e;
				writer->Write(CMD_HH(t->base, t->startFrame));
				writer->Write(CMD_HBB(t->endFrame, t->hour, t->minute));
				writer->Write((uint32_t)CMD_W(t->unk_08));
			}
			break;
		}
		case (uint32_t)CutsceneCommands::Terminator:
		{
			CutsceneCommand_Terminator* t = (CutsceneCommand_Terminator*)cs->commands[i];
			writer->Write(CS_CMD_TERMINATOR);
			writer->Write((uint32_t)1);
			writer->Write(CMD_HH(t->base, t->startFrame));
			writer->Write(CMD_HH(t->endFrame, t->endFrame));
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

	const auto endStream = writer->GetBaseAddress();
	writer->Seek((uint32_t)currentStream - 4, SeekOffsetType::Start);
	writer->Write((uint32_t)((endStream - currentStream) / 4));
	writer->Seek((uint32_t)endStream, SeekOffsetType::Start);
}