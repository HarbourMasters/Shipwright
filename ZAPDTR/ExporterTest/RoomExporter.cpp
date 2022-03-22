#include "RoomExporter.h"
#include "CollisionExporter.h"
#include "Utils/BinaryWriter.h"
#include "Utils/File.h"
#include "Utils/MemoryStream.h"
#include "ZRoom/Commands/SetCameraSettings.h"
#include "ZRoom/Commands/SetCollisionHeader.h"
#include "ZRoom/Commands/SetCsCamera.h"
#include "ZRoom/Commands/SetEchoSettings.h"
#include "ZRoom/Commands/SetEntranceList.h"
#include "ZRoom/Commands/SetLightingSettings.h"
#include "ZRoom/Commands/SetMesh.h"
#include "ZRoom/Commands/SetRoomBehavior.h"
#include "ZRoom/Commands/SetRoomList.h"
#include "ZRoom/Commands/SetSkyboxModifier.h"
#include "ZRoom/Commands/SetSkyboxSettings.h"
#include "ZRoom/Commands/SetSoundSettings.h"
#include "ZRoom/Commands/SetSpecialObjects.h"
#include "ZRoom/Commands/SetStartPositionList.h"
#include "ZRoom/Commands/SetTimeSettings.h"
#include "ZRoom/Commands/SetWind.h"

void ExporterExample_Room::Save(ZResource* res, fs::path outPath, BinaryWriter* writer)
{
	ZRoom* room = dynamic_cast<ZRoom*>(res);

	// MemoryStream* memStream = new MemoryStream();
	// BinaryWriter* writer = new BinaryWriter(memStream);

	for (size_t i = 0; i < room->commands.size() * 8; i++)
		writer->Write((uint8_t)0);

	for (size_t i = 0; i < room->commands.size(); i++)
	{
		ZRoomCommand* cmd = room->commands[i];

		writer->Seek(i * 8, SeekOffsetType::Start);

		writer->Write((uint8_t)cmd->cmdID);

		switch (cmd->cmdID)
		{
		case RoomCommand::SetWind:
		{
			SetWind* cmdSetWind = (SetWind*)cmd;

			writer->Write((uint8_t)0);  // 0x01
			writer->Write((uint8_t)0);  // 0x02
			writer->Write((uint8_t)0);  // 0x03

			writer->Write(cmdSetWind->windWest);               // 0x04
			writer->Write(cmdSetWind->windVertical);           // 0x05
			writer->Write(cmdSetWind->windSouth);              // 0x06
			writer->Write(cmdSetWind->clothFlappingStrength);  // 0x07
		}
		break;
		case RoomCommand::SetTimeSettings:
		{
			SetTimeSettings* cmdTime = (SetTimeSettings*)cmd;

			writer->Write((uint8_t)0);  // 0x01
			writer->Write((uint8_t)0);  // 0x02
			writer->Write((uint8_t)0);  // 0x03

			writer->Write(cmdTime->hour);  // 0x04
			writer->Write(cmdTime->min);   // 0x05
			writer->Write(cmdTime->unk);   // 0x06
			writer->Write((uint8_t)0);     // 0x07
		}
		break;
		case RoomCommand::SetSkyboxModifier:
		{
			SetSkyboxModifier* cmdSkybox = (SetSkyboxModifier*)cmd;

			writer->Write((uint8_t)0);  // 0x01
			writer->Write((uint8_t)0);  // 0x02
			writer->Write((uint8_t)0);  // 0x03

			writer->Write(cmdSkybox->disableSky);      // 0x04
			writer->Write(cmdSkybox->disableSunMoon);  // 0x05
			writer->Write((uint8_t)0);                 // 0x06
			writer->Write((uint8_t)0);                 // 0x07
		}
		break;
		case RoomCommand::SetEchoSettings:
		{
			SetEchoSettings* cmdEcho = (SetEchoSettings*)cmd;

			writer->Write((uint8_t)0);              // 0x01
			writer->Write((uint8_t)0);              // 0x02
			writer->Write((uint8_t)0);              // 0x03
			writer->Write((uint8_t)0);              // 0x04
			writer->Write((uint8_t)0);              // 0x05
			writer->Write((uint8_t)0);              // 0x06
			writer->Write((uint8_t)cmdEcho->echo);  // 0x07
		}
		break;
		case RoomCommand::SetSoundSettings:
		{
			SetSoundSettings* cmdSound = (SetSoundSettings*)cmd;

			writer->Write((uint8_t)cmdSound->reverb);  // 0x01
			writer->Write((uint8_t)0);                 // 0x02
			writer->Write((uint8_t)0);                 // 0x03
			writer->Write((uint8_t)0);                 // 0x04
			writer->Write((uint8_t)0);                 // 0x05

			writer->Write(cmdSound->nightTimeSFX);   // 0x06
			writer->Write(cmdSound->musicSequence);  // 0x07
		}
		break;
		case RoomCommand::SetSkyboxSettings:
		{
			SetSkyboxSettings* cmdSkybox = (SetSkyboxSettings*)cmd;

			writer->Write((uint8_t)cmdSkybox->unk1);          // 0x01
			writer->Write((uint8_t)0);                        // 0x02
			writer->Write((uint8_t)0);                        // 0x03
			writer->Write((uint8_t)cmdSkybox->skyboxNumber);  // 0x04
			writer->Write((uint8_t)cmdSkybox->cloudsType);    // 0x05
			writer->Write((uint8_t)cmdSkybox->isIndoors);     // 0x06
		}
		break;
		case RoomCommand::SetRoomBehavior:
		{
			SetRoomBehavior* cmdRoom = (SetRoomBehavior*)cmd;

			writer->Write((uint8_t)cmdRoom->gameplayFlags);  // 0x01
			writer->Write((uint8_t)0);                       // 0x02
			writer->Write((uint8_t)0);                       // 0x03
			writer->Write(cmdRoom->gameplayFlags2);          // 0x04
		}
		break;
		case RoomCommand::SetCsCamera:
		{
			SetCsCamera* cmdCsCam = (SetCsCamera*)cmd;

			writer->Write((uint8_t)cmdCsCam->cameras.size());  // 0x01
			writer->Write((uint8_t)0);                         // 0x02
			writer->Write((uint8_t)0);                         // 0x03

			writer->Write(cmdCsCam->segmentOffset);  // 0x04
		}
		break;
		case RoomCommand::SetMesh:
		{
			SetMesh* cmdMesh = (SetMesh*)cmd;

			int baseStreamEnd = writer->GetStream().get()->GetLength();

			writer->Write((uint8_t)cmdMesh->data);  // 0x01
			writer->Write((uint8_t)0);              // 0x02
			writer->Write((uint8_t)0);              // 0x03

			writer->Write(baseStreamEnd);  // 0x04

			uint32_t oldOffset = writer->GetBaseAddress();
			writer->Seek(baseStreamEnd, SeekOffsetType::Start);

			// TODO: NOT DONE

			writer->Write(cmdMesh->meshHeaderType);

			if (cmdMesh->meshHeaderType == 0)
			{
				// writer->Write(cmdMesh->)
			}
			else if (cmdMesh->meshHeaderType == 1)
			{
			}
			else if (cmdMesh->meshHeaderType == 2)
			{
			}

			writer->Seek(oldOffset, SeekOffsetType::Start);
		}
		break;
		case RoomCommand::SetCameraSettings:
		{
			SetCameraSettings* cmdCam = (SetCameraSettings*)cmd;

			writer->Write((uint8_t)cmdCam->cameraMovement);  // 0x01
			writer->Write((uint8_t)0);                       // 0x02
			writer->Write((uint8_t)0);                       // 0x03
			writer->Write(cmdCam->mapHighlight);             // 0x04
		}
		break;
		case RoomCommand::SetLightingSettings:
		{
			SetLightingSettings* cmdLight = (SetLightingSettings*)cmd;

			writer->Write((uint8_t)cmdLight->settings.size());  // 0x01
			writer->Write((uint8_t)0);                          // 0x02
			writer->Write((uint8_t)0);                          // 0x03
			writer->Write(cmdLight->segmentOffset);             // 0x04

			uint32_t oldOffset = writer->GetBaseAddress();
			writer->Seek(cmdLight->segmentOffset, SeekOffsetType::Start);

			for (LightingSettings setting : cmdLight->settings)
			{
				writer->Write(setting.ambientClrR);
				writer->Write(setting.ambientClrG);
				writer->Write(setting.ambientClrB);

				writer->Write(setting.diffuseClrA_R);
				writer->Write(setting.diffuseClrA_G);
				writer->Write(setting.diffuseClrA_B);

				writer->Write(setting.diffuseDirA_X);
				writer->Write(setting.diffuseDirA_Y);
				writer->Write(setting.diffuseDirA_Z);

				writer->Write(setting.diffuseClrB_R);
				writer->Write(setting.diffuseClrB_G);
				writer->Write(setting.diffuseClrB_B);

				writer->Write(setting.diffuseDirB_X);
				writer->Write(setting.diffuseDirB_Y);
				writer->Write(setting.diffuseDirB_Z);

				writer->Write(setting.fogClrR);
				writer->Write(setting.fogClrG);
				writer->Write(setting.fogClrB);

				writer->Write(setting.unk);
				writer->Write(setting.drawDistance);
			}

			writer->Seek(oldOffset, SeekOffsetType::Start);
		}
		break;
		case RoomCommand::SetRoomList:
		{
			SetRoomList* cmdRoom = (SetRoomList*)cmd;

			writer->Write((uint8_t)cmdRoom->romfile->rooms.size());  // 0x01
			writer->Write((uint8_t)0);                               // 0x02
			writer->Write((uint8_t)0);                               // 0x03

			auto baseStreamEnd = writer->GetLength();
			writer->Write(baseStreamEnd);  // 0x04

			uint32_t oldOffset = writer->GetBaseAddress();
			writer->Seek(baseStreamEnd, SeekOffsetType::Start);

			for (const auto& entry : cmdRoom->romfile->rooms)
			{
				writer->Write(entry.virtualAddressStart);
				writer->Write(entry.virtualAddressEnd);
			}

			writer->Seek(oldOffset, SeekOffsetType::Start);
		}
		break;
		case RoomCommand::SetCollisionHeader:
		{
			SetCollisionHeader* cmdCollHeader = (SetCollisionHeader*)cmd;

			int streamEnd = writer->GetStream().get()->GetLength();

			writer->Write((uint8_t)0);  // 0x01
			writer->Write((uint8_t)0);  // 0x02
			writer->Write((uint8_t)0);  // 0x03
			writer->Write(streamEnd);   // 0x04

			// TODO: NOT DONE

			uint32_t oldOffset = writer->GetBaseAddress();
			writer->Seek(streamEnd, SeekOffsetType::Start);

			ExporterExample_Collision colExp = ExporterExample_Collision();

			colExp.Save(cmdCollHeader->collisionHeader, outPath, writer);

			writer->Seek(oldOffset, SeekOffsetType::Start);
		}
		break;
		case RoomCommand::SetEntranceList:
		{
			SetEntranceList* cmdEntrance = (SetEntranceList*)cmd;

			uint32_t baseStreamEnd = writer->GetStream().get()->GetLength();

			writer->Write((uint8_t)0);     // 0x01
			writer->Write((uint8_t)0);     // 0x02
			writer->Write((uint8_t)0);     // 0x03
			writer->Write(baseStreamEnd);  // 0x04

			uint32_t oldOffset = writer->GetBaseAddress();
			writer->Seek(baseStreamEnd, SeekOffsetType::Start);

			for (EntranceEntry entry : cmdEntrance->entrances)
			{
				writer->Write((uint8_t)entry.startPositionIndex);
				writer->Write((uint8_t)entry.roomToLoad);
			}

			writer->Seek(oldOffset, SeekOffsetType::Start);
		}
		break;
		case RoomCommand::SetSpecialObjects:
		{
			SetSpecialObjects* cmdSpecObj = (SetSpecialObjects*)cmd;

			writer->Write((uint8_t)cmdSpecObj->elfMessage);     // 0x01
			writer->Write((uint8_t)0);                          // 0x02
			writer->Write((uint8_t)0);                          // 0x03
			writer->Write((uint8_t)0);                          // 0x04
			writer->Write((uint8_t)0);                          // 0x05
			writer->Write((uint16_t)cmdSpecObj->globalObject);  // 0x06
		}
		break;
		case RoomCommand::SetStartPositionList:
		{
			SetStartPositionList* cmdStartPos = (SetStartPositionList*)cmd;

			uint32_t baseStreamEnd = writer->GetStream().get()->GetLength();

			writer->Write((uint8_t)cmdStartPos->actors.size());  // 0x01
			writer->Write((uint8_t)0);                           // 0x02
			writer->Write((uint8_t)0);                           // 0x03
			writer->Write(baseStreamEnd);                        // 0x04

			uint32_t oldOffset = writer->GetBaseAddress();
			writer->Seek(baseStreamEnd, SeekOffsetType::Start);

			for (ActorSpawnEntry entry : cmdStartPos->actors)
			{
				writer->Write(entry.actorNum);
				writer->Write(entry.posX);
				writer->Write(entry.posY);
				writer->Write(entry.posZ);
				writer->Write(entry.rotX);
				writer->Write(entry.rotY);
				writer->Write(entry.rotZ);
				writer->Write(entry.initVar);
			}

			writer->Seek(oldOffset, SeekOffsetType::Start);
		}
		break;
		case RoomCommand::EndMarker:
		{
			writer->Write((uint8_t)0);  // 0x01
			writer->Write((uint8_t)0);  // 0x02
			writer->Write((uint8_t)0);  // 0x03
			writer->Write((uint8_t)0);  // 0x04
			writer->Write((uint8_t)0);  // 0x05
			writer->Write((uint8_t)0);  // 0x06
			writer->Write((uint8_t)0);  // 0x07
		}
		break;
		default:
			printf("UNIMPLEMENTED COMMAND: %i\n", (int)cmd->cmdID);

			writer->Write((uint8_t)0);  // 0x01
			writer->Write((uint8_t)0);  // 0x02
			writer->Write((uint8_t)0);  // 0x03
			writer->Write((uint8_t)0);  // 0x04
			writer->Write((uint8_t)0);  // 0x05
			writer->Write((uint8_t)0);  // 0x06
			writer->Write((uint8_t)0);  // 0x07

			break;
		}
	}

	// writer->Close();
	// File::WriteAllBytes(StringHelper::Sprintf("%s", res->GetName().c_str()),
	// memStream->ToVector());
}
